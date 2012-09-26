// $Header: /CAMCAD/4.6/GerberEducatorUi.cpp 44    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "GerberEducatorUi.h"
#include "Lic.h"
#include "Crypt.h"
#include "GerberEducator.h"
#include "GerberEducatorDialog.h"
#include "GerberEducatorAlignLayer.h"
#include "GerberEducatorReviewGeometries.h"
#include "GerberEducatorReviewInserts.h"
#include "MainFrm.h"
#include "CCEtoODB.h"
#include "EnumIterator.h"
#include ".\gerbereducatorui.h"

//_____________________________________________________________________________
CString gerberEducatorColorToString(GerberEducatorColorTag tagValue)
{
   const char* value;

   switch (tagValue)
   {
   // Colors used in Setup Data dialog
   case gerberEducatorSetupDataPadTopFlashesColor:     value = "gerberEducatorSetupDataPadTopFlashesColor";     break;
   case gerberEducatorSetupDataPadTopDrawsColor:       value = "gerberEducatorSetupDataPadTopDrawsColor";       break;
   case gerberEducatorSetupDataPadBottomFlashesColor:  value = "gerberEducatorSetupDataPadBottomFlashesColor";  break;
   case gerberEducatorSetupDataPadBottomDrawsColor:    value = "gerberEducatorSetupDataPadBottomDrawsColor";    break;
   case gerberEducatorSetupDataSilkscreenTopColor:     value = "gerberEducatorSetupDataSilkscreenTopColor";     break;
   case gerberEducatorSetupDataSilkscreenBottomColor:  value = "gerberEducatorSetupDataSilkscreenBottomColor";  break;
   case gerberEducatorSetupDataCentroidTopColor:       value = "gerberEducatorSetupDataCentroidTopColor";       break;
   case gerberEducatorSetupDataCentroidBottomColor:    value = "gerberEducatorSetupDataCentroidBottomColor";    break;
   case gerberEducatorSetupDataUnknownColor:           value = "gerberEducatorSetupDataUnknownColor";           break;

   // Colors used in Create Geometries dialog
   case gerberEducatorCreateGeometriesUnmatchedPadTopColor:          value = "gerberEducatorCreateGeometriesUnmatchedPadTopColor";          break;
   case gerberEducatorCreateGeometriesUnmatchedPadBottomColor:       value = "gerberEducatorCreateGeometriesUnmatchedPadBottomColor";       break;
   case gerberEducatorCreateGeometriesUnmatchedCentroidTopColor:     value = "gerberEducatorCreateGeometriesUnmatchedCentroidTopColor";     break;
   case gerberEducatorCreateGeometriesUnmatchedCentroidBottomColor:  value = "gerberEducatorCreateGeometriesUnmatchedCentroidBottomColor";  break;
   case gerberEducatorCreateGeometriesSilkscreenTopColor:            value = "gerberEducatorCreateGeometriesSilkscreenTopColor";            break;
   case gerberEducatorCreateGeometriesSilkscreenBottomColor:         value = "gerberEducatorCreateGeometriesSilkscreenBottomColor";         break;
   case gerberEducatorCreateGeometriesCentroidTopColor:              value = "gerberEducatorCreateGeometriesCentroidTopColor";              break;
   case gerberEducatorCreateGeometriesCentroidBottomColor:           value = "gerberEducatorCreateGeometriesCentroidBottomColor";           break;
   case gerberEducatorCreateGeometriesSmdPadTopColor:                value = "gerberEducatorCreateGeometriesSmdPadTopColor";                break;
   case gerberEducatorCreateGeometriesSmdPadBottomColor:             value = "gerberEducatorCreateGeometriesSmdPadBottomColor";             break;
   case gerberEducatorCreateGeometriesThPadTopColor:                 value = "gerberEducatorCreateGeometriesThPadTopColor";                 break;
   case gerberEducatorCreateGeometriesThPadBottomColor:              value = "gerberEducatorCreateGeometriesThPadBottomColor";              break;
   case gerberEducatorCreateGeometriesComponentOutlineTopColor:      value = "gerberEducatorCreateGeometriesComponentOutlineTopColor";      break;
   case gerberEducatorCreateGeometriesComponentOutlineBottomColor:   value = "gerberEducatorCreateGeometriesComponentOutlineBottomColor";   break;

   // Colors used in Review Geometries dialog
   case gerberEducatorReviewGeometriesCentroidTopColor:             value = "gerberEducatorReviewGeometriesCentroidTopColor";             break;
   case gerberEducatorReviewGeometriesCentroidBottomColor:          value = "gerberEducatorReviewGeometriesCentroidBottomColor";          break;
   case gerberEducatorReviewGeometriesSmdPadTopColor:               value = "gerberEducatorReviewGeometriesSmdPadTopColor";               break;
   case gerberEducatorReviewGeometriesSmdPadBottomColor:            value = "gerberEducatorReviewGeometriesSmdPadBottomColor";            break;
   case gerberEducatorReviewGeometriesThPadTopColor:                value = "gerberEducatorReviewGeometriesThPadTopColor";                break;
   case gerberEducatorReviewGeometriesThPadBottomColor:             value = "gerberEducatorReviewGeometriesThPadBottomColor";             break;
   case gerberEducatorReviewGeometriesComponentOutlineTopColor:     value = "gerberEducatorReviewGeometriesComponentOutlineTopColor";     break;
   case gerberEducatorReviewGeometriesComponentOutlineBottomColor:  value = "gerberEducatorReviewGeometriesComponentOutlineBottomColor";  break;

   // Colors used in Review Component dialog
   case gerberEducatorReviewComponentCentroidTopColor:             value = "gerberEducatorReviewComponentCentroidTopColor";             break;
   case gerberEducatorReviewComponentCentroidBottomColor:          value = "gerberEducatorReviewComponentCentroidBottomColor";          break;
   case gerberEducatorReviewComponentSmdPadTopColor:               value = "gerberEducatorReviewComponentSmdPadTopColor";               break;
   case gerberEducatorReviewComponentSmdPadBottomColor:            value = "gerberEducatorReviewComponentSmdPadBottomColor";            break;
   case gerberEducatorReviewComponentThPadTopColor:                value = "gerberEducatorReviewComponentThPadTopColor";                break;
   case gerberEducatorReviewComponentThPadBottomColor:             value = "gerberEducatorReviewComponentThPadBottomColor";             break;
   case gerberEducatorReviewComponentComponentOutlineTopColor:     value = "gerberEducatorReviewComponentComponentOutlineTopColor";     break;
   case gerberEducatorReviewComponentComponentOutlineBottomColor:  value = "gerberEducatorReviewComponentComponentOutlineBottomColor";  break;

   case gerberEducatorReviewComponentApprovedTopColor:             value = "gerberEducatorReviewComponentApprovedTopColor";             break;
   case gerberEducatorReviewComponentApprovedBottomColor:          value = "gerberEducatorReviewComponentApprovedBottomColor";          break;
   }

   return CString(value);
}

GerberEducatorColorTag stringToGerberEducatorColor(const CString& tagValue)
{
   GerberEducatorColorTag retval = GerberEducatorColorTagMax;

   for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
   {
      GerberEducatorColorTag colorTag = colorIterator.getNext();

      if (gerberEducatorColorToString(colorTag).CompareNoCase(tagValue) == 0)
      {
         retval = colorTag;
         break;
      }
   }

   return retval;
}

//_____________________________________________________________________________
void CCEtoODBDoc::OnGerberEducatorConnectToSession()
{
   //if (!get_license(LIC_GerberEducator)) 
   //{
   //   ErrorAccess("You do not have a License for Gerber Educator!");
   //}
   //else
   //{
   //   CGerberEducatorUi::getGerberEducatorUi(*this).OnEventConnectToSession();
   //}
}

void CCEtoODBDoc::OnGerberEducatorSetupDataType()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator)) 
   {
      ErrorAccess("Gerber Educator License not found!");
   }
	else
	{
		CGerberEducatorUi::getGerberEducatorUi(*this).OnEventSetupDataType();
	}*/
}

void CCEtoODBDoc::OnGerberEducatorCreateGeometries()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator)) 
   {
      ErrorAccess("Gerber Educator License not found!");
   }
	else
	{
	   CGerberEducatorUi::getGerberEducatorUi(*this).OnEventCreateGeometries();
	}*/
}

void CCEtoODBDoc::OnGerberEducatorEditGeometries()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator)) 
   {
      ErrorAccess("Gerber Educator License not found!");
   }
	else
	{
	   CGerberEducatorUi::getGerberEducatorUi(*this).OnEventEditGeometries();
	}*/
}

void CCEtoODBDoc::OnGerberEducatorEditComponents()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator)) 
   {
      ErrorAccess("Gerber Educator License not found!");
   }
	else
	{
	   CGerberEducatorUi::getGerberEducatorUi(*this).OnEventEditComponents();
	}*/
}

void CCEtoODBDoc::OnGerberEducatorCleanUpData()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator)) 
   {
      ErrorAccess("Gerber Educator License not found!");
   }
	else
	{
		CGerberEducatorUi::getGerberEducatorUi(*this).OnEventCleanUpData();
	}*/
}

void CCEtoODBDoc::OnGerberEducatorColorsDialog()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator)) 
   {
      ErrorAccess("Gerber Educator License not found!");
   }
	else
	{
		CGerberEducatorColorsDialog dialog;

		dialog.DoModal();
	}*/
}

void CCEtoODBDoc::OnGerberEducatorTerminateSession()
{
   CGerberEducatorUi::getGerberEducatorUi(*this).OnEventTerminateSession();
}


//-----------------------------------------------------------------------------
// CGerberEducatorColorScheme
//-----------------------------------------------------------------------------
CGerberEducatorColorScheme::CGerberEducatorColorScheme()
{
	initializeColors();
   loadFromRegistry();
}

CGerberEducatorColorScheme::~CGerberEducatorColorScheme()
{
}

CGerberEducatorColorScheme& CGerberEducatorColorScheme::operator=(const CGerberEducatorColorScheme& other)
{
   if (&other != this)
   {
      for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
      {
         GerberEducatorColorTag colorTag = colorIterator.getNext();
         m_colors[colorTag] = other.m_colors[colorTag];
      }
   }

   return *this;
}

void CGerberEducatorColorScheme::initializeColors()
{
   // Colors used in Setup Data dialog
   m_colors[gerberEducatorSetupDataPadTopFlashesColor   ] = colorHtmlSeaGreen;
   m_colors[gerberEducatorSetupDataPadTopDrawsColor     ] = colorHtmlLime;
   m_colors[gerberEducatorSetupDataPadBottomFlashesColor] = colorHtmlMaroon;
   m_colors[gerberEducatorSetupDataPadBottomDrawsColor  ] = colorHtmlRed;
   m_colors[gerberEducatorSetupDataSilkscreenTopColor   ] = colorHtmlSilver;
   m_colors[gerberEducatorSetupDataSilkscreenBottomColor] = colorHtmlGray;
   m_colors[gerberEducatorSetupDataCentroidTopColor     ] = colorHtmlYellowGreen;
   m_colors[gerberEducatorSetupDataCentroidBottomColor  ] = colorHtmlOrange;
   m_colors[gerberEducatorSetupDataUnknownColor         ] = colorHtmlTan;

   // Colors used in Create Geometries dialog
   m_colors[gerberEducatorCreateGeometriesUnmatchedPadTopColor        ] = colorHtmlSeaGreen;
   m_colors[gerberEducatorCreateGeometriesUnmatchedPadBottomColor     ] = colorHtmlMaroon;
   m_colors[gerberEducatorCreateGeometriesUnmatchedCentroidTopColor   ] = colorHtmlYellowGreen;
   m_colors[gerberEducatorCreateGeometriesUnmatchedCentroidBottomColor] = colorHtmlOrange;
   m_colors[gerberEducatorCreateGeometriesSilkscreenTopColor          ] = colorHtmlSilver;
   m_colors[gerberEducatorCreateGeometriesSilkscreenBottomColor       ] = colorHtmlGray;
   m_colors[gerberEducatorCreateGeometriesCentroidTopColor            ] = colorHtmlNavy;
   m_colors[gerberEducatorCreateGeometriesCentroidBottomColor         ] = colorHtmlBlue;
   m_colors[gerberEducatorCreateGeometriesSmdPadTopColor              ] = colorHtmlLime;
   m_colors[gerberEducatorCreateGeometriesSmdPadBottomColor           ] = colorHtmlRed;
   m_colors[gerberEducatorCreateGeometriesThPadTopColor               ] = colorHtmlYellow;
   m_colors[gerberEducatorCreateGeometriesThPadBottomColor            ] = colorHtmlYellow;
   m_colors[gerberEducatorCreateGeometriesComponentOutlineTopColor    ] = colorHtmlDeepSkyBlue;
   m_colors[gerberEducatorCreateGeometriesComponentOutlineBottomColor ] = colorHtmlPaleTurquoise;

   // Colors used in Review Geometries dialog
   m_colors[gerberEducatorReviewGeometriesCentroidTopColor           ] = colorHtmlNavy;
   m_colors[gerberEducatorReviewGeometriesCentroidBottomColor        ] = colorHtmlBlue;
   m_colors[gerberEducatorReviewGeometriesSmdPadTopColor             ] = colorHtmlLime;
   m_colors[gerberEducatorReviewGeometriesSmdPadBottomColor          ] = colorHtmlRed;
   m_colors[gerberEducatorReviewGeometriesThPadTopColor              ] = colorHtmlYellow;
   m_colors[gerberEducatorReviewGeometriesThPadBottomColor           ] = colorHtmlYellow;
   m_colors[gerberEducatorReviewGeometriesComponentOutlineTopColor   ] = colorHtmlDeepSkyBlue;
   m_colors[gerberEducatorReviewGeometriesComponentOutlineBottomColor] = colorHtmlPaleTurquoise;

   // Colors used in Review Component dialog
   m_colors[gerberEducatorReviewComponentCentroidTopColor           ] = colorHtmlNavy;
   m_colors[gerberEducatorReviewComponentCentroidBottomColor        ] = colorHtmlBlue;
   m_colors[gerberEducatorReviewComponentSmdPadTopColor             ] = colorHtmlLime;
   m_colors[gerberEducatorReviewComponentSmdPadBottomColor          ] = colorHtmlRed;
   m_colors[gerberEducatorReviewComponentThPadTopColor              ] = colorHtmlYellow;
   m_colors[gerberEducatorReviewComponentThPadBottomColor           ] = colorHtmlYellow;
   m_colors[gerberEducatorReviewComponentComponentOutlineTopColor   ] = colorHtmlDeepSkyBlue;
   m_colors[gerberEducatorReviewComponentComponentOutlineBottomColor] = colorHtmlPaleTurquoise;
   m_colors[gerberEducatorReviewComponentApprovedTopColor           ] = colorHtmlPaleGreen;
   m_colors[gerberEducatorReviewComponentApprovedBottomColor        ] = colorHtmlPaleVioletRed;
}

COLORREF CGerberEducatorColorScheme::getColor(GerberEducatorColorTag colorTag) const
{
   return m_colors[colorTag];
}

void CGerberEducatorColorScheme::setColor(GerberEducatorColorTag colorTag,COLORREF color)
{
   m_colors[colorTag] = color;
}

void CGerberEducatorColorScheme::setDefaultViewColor(CCamCadDatabase& camCadDatabase, FileStruct& pcbFile)
{
	int dataSourceKw = camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataSource);
	int dataTypeKw = camCadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeDataType);

	// Set layer colors
	for (int i=0; i<camCadDatabase.getCamCadDoc().getLayerCount(); i++)
	{
		LayerStruct* layer = camCadDatabase.getCamCadDoc().getLayerAt(i);
		if (layer == NULL)
			continue;

      COLORREF layerColor = layer->getColor();
		bool visible = false;

		CString dataSource, dataType;
		camCadDatabase.getAttributeStringValue(dataSource, &layer->getAttributesRef(), dataSourceKw);
		camCadDatabase.getAttributeStringValue(dataType, &layer->getAttributesRef(), dataTypeKw);

		GerberEducatorDataSourceTag dataSourceTag         = stringToGerberEducatorDataSource(dataSource);
		GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(dataType);

		if (dataSourceTypeTag == gerberEducatorDataSourceTypePadTop)
		{
			layerColor = getColor(gerberEducatorSetupDataPadTopFlashesColor);
			visible = true;
		}
		else if (dataSourceTypeTag == gerberEducatorDataSourceTypePadBottom)
		{
			layerColor = getColor(gerberEducatorSetupDataPadBottomFlashesColor);
			visible = true;
		}
		else if (dataSourceTypeTag == gerberEducatorDataSourceTypeSilkscreenTop)
		{
			layerColor = getColor(gerberEducatorSetupDataSilkscreenTopColor);
			visible = true;
		}
		else if (dataSourceTypeTag == gerberEducatorDataSourceTypeSilkscreenBottom)
		{
			layerColor = getColor(gerberEducatorSetupDataSilkscreenBottomColor);
			visible = true;
		}
		else if (dataSourceTag == gerberEducatorDataSourceCentroid && dataSourceTypeTag == gerberEducatorDataSourceTypeTop)
		{
			layerColor = getColor(gerberEducatorSetupDataCentroidTopColor);
			visible = true;
		}
		else if (dataSourceTag == gerberEducatorDataSourceCentroid && dataSourceTypeTag == gerberEducatorDataSourceTypeBottom)
		{
			layerColor = getColor(gerberEducatorSetupDataCentroidBottomColor);
			visible = true;
		}
		else if (layer->getName().CompareNoCase(camCadDatabase.getCamCadLayerName(ccLayerCentroidTop)) == 0)
      {
         layerColor = getColor(gerberEducatorCreateGeometriesCentroidTopColor);
			visible = true;
      }
		else if (layer->getName().CompareNoCase(camCadDatabase.getCamCadLayerName(ccLayerCentroidBottom)) == 0)
      {
         layerColor = getColor(gerberEducatorCreateGeometriesCentroidBottomColor);
			visible = true;
      }
	   else if (layer->getName().CompareNoCase(camCadDatabase.getCamCadLayerName(ccLayerPadTop)) == 0)
      {
         layerColor = getColor(gerberEducatorCreateGeometriesSmdPadTopColor);
			visible = true;
      }
	   else if (layer->getName().CompareNoCase(camCadDatabase.getCamCadLayerName(ccLayerPadBottom)) == 0)
      {
         layerColor = getColor(gerberEducatorCreateGeometriesSmdPadBottomColor);
			visible = true;
      }
		else if (layer->getName().CompareNoCase(camCadDatabase.getCamCadLayerName(ccLayerAssemblyTop)) == 0)
      {
         layerColor = getColor(gerberEducatorCreateGeometriesComponentOutlineTopColor);
			visible = true;
      }
		else if (layer->getName().CompareNoCase(camCadDatabase.getCamCadLayerName(ccLayerAssemblyBottom)) == 0)
      {
         layerColor = getColor(gerberEducatorCreateGeometriesComponentOutlineBottomColor);
			visible = true;
      }
		else if (layer->getName().CompareNoCase(camCadDatabase.getCamCadLayerName(ccLayerDrillHoles)) == 0)
      {
			visible = true;
      }		

      layer->setColor(layerColor);
		layer->setVisible(visible);
	}

	// Turn off all color override except for draws on Pad Top and Pad Bottom layers
	for (CDataListIterator dataListIterator(*pcbFile.getBlock()); dataListIterator.hasNext();)
	{
		DataStruct* data = dataListIterator.getNext();

		LayerStruct* layer = camCadDatabase.getCamCadDoc().getLayerAt(data->getLayerIndex());
		if (layer == NULL)
		{
			data->setColorOverride(false);
			continue;
		}

      COLORREF overrideColor = layer->getColor();
		bool colorOverride = false;

		CString dataType;
		camCadDatabase.getAttributeStringValue(dataType, &layer->getAttributesRef(), dataTypeKw);
		GerberEducatorDataSourceTypeTag dataSourceTypeTag = stringToGerberEducatorDataSourceType(dataType);

		if (data->getDataType() == dataTypePoly)
		{
			if (dataSourceTypeTag == gerberEducatorDataSourceTypePadTop)
			{
				overrideColor = getColor(gerberEducatorSetupDataPadTopDrawsColor);
				colorOverride = true;
			}
			else if (dataSourceTypeTag == gerberEducatorDataSourceTypePadBottom)
			{
				overrideColor = getColor(gerberEducatorSetupDataPadBottomDrawsColor);
				colorOverride = true;
			}		
		}

		data->setOverrideColor(overrideColor);
		data->setColorOverride(colorOverride);
	}

	camCadDatabase.getCamCadDoc().UpdateAllViews(NULL);
}

void CGerberEducatorColorScheme::loadFromRegistry()
{
   CAppRegistryKey appKey; 
   CRegistryKey settingsKey = appKey.createSubKey("Settings");

   if (settingsKey.isOpen())
   {
      CRegistryKey gerberEducatorColorsKey = settingsKey.createSubKey("GerberEducatorColors");

      if (gerberEducatorColorsKey.isOpen())
      {
         CString tagValue,colorValue;

         for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
         {
            GerberEducatorColorTag colorTag = colorIterator.getNext();
            CString tagValue = gerberEducatorColorToString(colorTag);
            DWORD value;

            if (gerberEducatorColorsKey.getIntValue(tagValue,value))
            {
               m_colors[colorTag] = value;
            }
         }
      }
   }
}

void CGerberEducatorColorScheme::storeInRegistry()
{
   CAppRegistryKey appKey; 
   CRegistryKey settingsKey = appKey.createSubKey("Settings");

   if (settingsKey.isOpen())
   {
      CRegistryKey gerberEducatorColorsKey = settingsKey.createSubKey("GerberEducatorColors");

      if (gerberEducatorColorsKey.isOpen())
      {
         CString tagValue,colorValue;

         for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
         {
            GerberEducatorColorTag colorTag = colorIterator.getNext();
            CString tagValue = gerberEducatorColorToString(colorTag);
            DWORD value = m_colors[colorTag];

            gerberEducatorColorsKey.setValue(tagValue,value);
         }
      }
   }
}

//_____________________________________________________________________________
CGerberEducatorUi* CGerberEducatorUi::m_gerberEducatorUi = NULL;

CGerberEducatorUi::CGerberEducatorUi(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_dataTyperDialog(NULL)
, m_alignLayerDialog(NULL)
, m_reviewComponentsDialog(NULL)
, m_reviewGeometriesDialog(NULL)
, m_previousUiState(GerberEducatorUiStateUndefined)
, m_uiState(GerberEducatorUiStateUndefined)
, m_fileStruct(NULL)
, m_alignLayerArray(20, false)
{
}

CGerberEducatorUi& CGerberEducatorUi::getGerberEducatorUi(CCEtoODBDoc& camCadDoc)
{
   if (m_gerberEducatorUi != NULL)
   {
      if (&camCadDoc != &(m_gerberEducatorUi->m_camCadDoc))
      {
         deleteGerberEducatorUi();
      }
   }

   if (m_gerberEducatorUi == NULL)
   {
      m_gerberEducatorUi = new CGerberEducatorUi(camCadDoc);
   }

   return *m_gerberEducatorUi;
}

FileStruct* CGerberEducatorUi::getFileStruct()
{
   if (m_fileStruct == NULL)
   {
      m_fileStruct = m_camCadDoc.getFileList().GetFirstShown(blockTypeUndefined);
   }

   return m_fileStruct;
}

CGerberEducatorUi* CGerberEducatorUi::getGerberEducatorUi()
{
   return m_gerberEducatorUi;
}

bool CGerberEducatorUi::deleteGerberEducatorUi()
{
   bool retval = (m_gerberEducatorUi != NULL);

   delete m_gerberEducatorUi;
   m_gerberEducatorUi = NULL;

   return retval;
}

void CGerberEducatorUi::setUiState(GerberEducatorUiStateTag uiState)
{
   m_previousUiState = m_uiState;
   m_uiState         = uiState;
}

void CGerberEducatorUi::generateEvent(int commandId)
{
   getActiveView()->PostMessage(WM_COMMAND,commandId,0);
}

void CGerberEducatorUi::updateSelectedEntity()
{
	if (getUiState() == GerberEducatorUiStateEditingGeometries || getUiState() == GerberEducatorUiStateSubEditingGeometries)
	{
		if (m_reviewGeometriesDialog != NULL)
			m_reviewGeometriesDialog->updateSelectedEntity();
	}
	else if (getUiState() == GerberEducatorUiStateEditingComponents)
	{
		if (m_reviewComponentsDialog != NULL)
			m_reviewComponentsDialog->updateSelectedEntity();
	}
	else if (getUiState() == GerberEducatorUiStateAlignLayer)
	{
		if (m_alignLayerDialog != NULL)
			m_alignLayerDialog->updateSelectedEntity();
	}
}

BOOL CGerberEducatorUi::PreTranslateMessage(MSG* pMsg)
{
   BOOL retval = FALSE;
   if (getUiState() == GerberEducatorUiStateTypingData && m_dataTyperDialog != NULL)
   {
      retval = m_dataTyperDialog->PreTranslateMessage(pMsg);
   }

   if (getUiState() == GerberEducatorUiStateCreatingGeometries && getGerberEducator().isInitialized())
   {
      retval = getGerberEducator().getDialog().PreTranslateMessage(pMsg);
   }

	if ((getUiState() == GerberEducatorUiStateEditingGeometries || getUiState() == GerberEducatorUiStateSubEditingGeometries) &&
      m_reviewGeometriesDialog != NULL)
   {
      retval = m_reviewGeometriesDialog->PreTranslateMessage(pMsg);
   }

	if (getUiState() == GerberEducatorUiStateEditingComponents && m_reviewComponentsDialog != NULL)
   {
      retval = m_reviewComponentsDialog->PreTranslateMessage(pMsg);
   }

   return retval;
}

CGerberEducator& CGerberEducatorUi::getGerberEducator() 
{
   CGerberEducator& gerberEducator = m_camCadDoc.getDefinedGerberEducator(educatorFunctionalityCreateGeometry);
   gerberEducator.setGerberEducatorUi(this);

   return gerberEducator;
}

//bool isWindowToolbar(CWnd* wnd)
//{
//   WINDOWINFO windowInfo;
//
//   windowInfo.cbSize = sizeof(WINDOWINFO);
//   wnd->GetWindowInfo(&windowInfo);
//
//   ATOM atomWindowType = windowInfo.atomWindowType;
//   LPCTSTR lpClassName = atomWindowType;
//
//   WNDCLASS wndClass;
//   BOOL status = GetClassInfo(NULL,lpClassName,&wndClass);
//
//   if (status)
//   {
//      CString className = wndClass.lpszClassName;
//      int iii = 3;
//   }
//}

void CGerberEducatorUi::addAlignLayer(LayerStruct* layer)
{
   if (layer != NULL)
   {
      m_alignLayerArray.SetAtGrow(m_alignLayerArray.GetCount(), layer);
   }
}

void CGerberEducatorUi::emptyAlignLayerArry()
{
   m_alignLayerArray.empty();
}

int CGerberEducatorUi::getAlignLayerCount() const 
{ 
   return m_alignLayerArray.GetCount(); 
}

LayerStruct* CGerberEducatorUi::getAlignLayer(int index) 
{ 
   if (index < m_alignLayerArray.GetCount())
      return m_alignLayerArray.GetAt(index); 
   else
      return NULL;
}

void CGerberEducatorUi::arrangeWindows(CDialog& dialog,ArrangeWindowsMethodTag arrangeWindowsMethod) 
{
   CCEtoODBView* view = getActiveView();

   int nonClientVerticalPadding   = 4;
   int nonClientHorizontalPadding = 4;

   WINDOWPLACEMENT dialogWindowPlacement;
   WINDOWPLACEMENT viewWindowPlacement;

   dialog.GetWindowPlacement(&dialogWindowPlacement);
   view->GetWindowPlacement(&viewWindowPlacement);

   CMainFrame* mainFrame = getMainFrame();
   BOOL childFrameMaximizedFlag;
   CMDIChildWnd* childFrame = mainFrame->MDIGetActive(&childFrameMaximizedFlag);

   if (arrangeWindowsMethod == arrangeWindowsMethodBestFit)
   {
      mainFrame->RecalcLayout();

      CRect mainFrameClientRect;
      mainFrame->GetClientRect(&mainFrameClientRect);

      CRect& dialogRect = *((CRect*)(&(dialogWindowPlacement.rcNormalPosition)));
      CRect& viewRect   = *((CRect*)(&(viewWindowPlacement.rcNormalPosition)));

      if (mainFrameClientRect.Width() > 2*dialogRect.Width())
      {
         if (childFrameMaximizedFlag)
         {
            view->GetWindowRect(&mainFrameClientRect);
            mainFrame->ScreenToClient(&mainFrameClientRect);
            childFrame->MDIRestore();
         }
         else if (getPreviousUiState() != GerberEducatorUiStateUndefined)
         {
            CRect statusBarRect;
            mainFrame->getStatusBar().GetWindowRect(&statusBarRect);
            nonClientVerticalPadding += statusBarRect.Height() - 1;
         }

         childFrame->MoveWindow(0,0,mainFrameClientRect.Width() - dialogRect.Width() - 1 - nonClientHorizontalPadding,
                                mainFrameClientRect.Height() - nonClientVerticalPadding);

         CRect childFrameRect;
         childFrame->GetWindowRect(&childFrameRect);

         dialogRect.SetRect(childFrameRect.right,
                            childFrameRect.top,
                            childFrameRect.right + dialogRect.Width(),
                            childFrameRect.top   + dialogRect.Height());

         dialog.SetWindowPlacement(&dialogWindowPlacement);
      }
   }
   else if (arrangeWindowsMethod == arrangeWindowsMethodMaximumNormal)
   {
      mainFrame->normalizeToMaximumActiveChildFrame();
      //mainFrame->RecalcLayout();

      //CRect mainFrameClientRect;
      //mainFrame->getClientRectWithoutToolbars(&mainFrameClientRect);

      //CRect& dialogRect = *((CRect*)(&(dialogWindowPlacement.rcNormalPosition)));
      //CRect& viewRect   = *((CRect*)(&(viewWindowPlacement.rcNormalPosition)));

      //if (childFrameMaximizedFlag)
      //{
      //   view->GetWindowRect(&mainFrameClientRect);
      //   mainFrame->ScreenToClient(&mainFrameClientRect);
      //   childFrame->MDIRestore();
      //}
      //else if (getPreviousUiState() != GerberEducatorUiStateUndefined)
      //{
      //   CRect statusBarRect;
      //   mainFrame->getStatusBar().GetWindowRect(&statusBarRect);
      //   nonClientVerticalPadding += statusBarRect.Height() - 1;
      //}

      //childFrame->MoveWindow(0,0,mainFrameClientRect.Width() - 1 - nonClientHorizontalPadding,
      //                        mainFrameClientRect.Height() - nonClientVerticalPadding);

      //CRect childFrameRect;
      //childFrame->GetWindowRect(&childFrameRect);

      //dialogRect.SetRect(childFrameRect.right,
      //                     childFrameRect.top,
      //                     childFrameRect.right + dialogRect.Width(),
      //                     childFrameRect.top   + dialogRect.Height());

      //dialog.SetWindowPlacement(&dialogWindowPlacement);
      dialog.Invalidate();
      dialog.SendMessage(WM_NCPAINT,1,0);
   }
}

void CGerberEducatorUi::OnEventCreateGeometries() 
{
   if (getUiState() != GerberEducatorUiStateCreatingGeometries)
   {
      closeDataTyperDialog();
      closeAlignLayerDialog();
      closeReviewGeometriesDialog();
      closeReviewComponentsDialog();
   }

   if (getFileStruct() != NULL)
   {
      setUiState(GerberEducatorUiStateCreatingGeometries);

      if (!getGerberEducator().isInitialized())
      {
         closeGerberEducator();
      }
      else
      {
         getApp().setRunMode(runModeDataProtectedModelessDialog);
         //getMainFrame()->displayToolBars(false);
         getMainFrame()->disableToolBars();
         getMainFrame()->disableMenuItems();
         //getActiveView()->enableContextMenu(false);
         getGerberEducator().showDialog();

         arrangeWindows(getGerberEducator().getDialog(),arrangeWindowsMethodMaximumNormal);
      }
   }
   else
   {
      closeGerberEducator();
   }
}

void CGerberEducatorUi::OnEventSetupDataType()
{
   if (getUiState() != GerberEducatorUiStateTypingData)
   {
      closeGerberEducator();
      closeAlignLayerDialog();
      closeReviewGeometriesDialog();
      closeReviewComponentsDialog();
   }

   if (getFileStruct() != NULL)
   {
      setUiState(GerberEducatorUiStateTypingData);

      if (m_dataTyperDialog == NULL)
      {
	      m_dataTyperDialog = new CGESetupDataType(*this);
	      m_dataTyperDialog->Create(IDD_GE_SetupDataType);
      }

      getApp().setRunMode(runModeDataProtectedModelessDialog);
      //getMainFrame()->displayToolBars(false);
      getMainFrame()->disableToolBars();
      getMainFrame()->disableMenuItems();
      //getActiveView()->enableContextMenu(false);
	   m_dataTyperDialog->ShowWindow(SW_SHOW);
	   m_dataTyperDialog->UpdateWindow();

      arrangeWindows(*m_dataTyperDialog,arrangeWindowsMethodMaximumNormal);

	   if (!m_dataTyperDialog->isInitialized())
      {
		   closeDataTyperDialog();
      }
   }
   else
   {
      closeDataTyperDialog();
   }
}

void CGerberEducatorUi::OnEventAlignLayer()
{
   if (getUiState() != GerberEducatorUiStateAlignLayer)
   {
      closeDataTyperDialog();
      closeGerberEducator();
      closeReviewGeometriesDialog();
      closeReviewComponentsDialog();
   }

   if (getFileStruct() != NULL)
   {
      setUiState(GerberEducatorUiStateAlignLayer);

      if (m_alignLayerDialog == NULL)
      {        
         m_alignLayerDialog = (CGEAlignLayerDlg*)&getActiveView()->getDefinedMeasure(measureDialogTypeAlignLayer).getDialog();
         m_alignLayerDialog->setGerberEducatorUi(this);
         m_alignLayerDialog->Create(IDD_GE_AlignLayer);
      }

      getApp().setRunMode(runModeDataProtectedModelessDialog);
      getMainFrame()->disableToolBars();
      getMainFrame()->disableMenuItems();
      //getActiveView()->enableContextMenu(false);
	   m_alignLayerDialog->ShowWindow(SW_SHOW);
	   m_alignLayerDialog->UpdateWindow();

      arrangeWindows(*m_alignLayerDialog,arrangeWindowsMethodMaximumNormal);

	   if (!m_alignLayerDialog->isInitialized())
      {
		   closeAlignLayerDialog();
      }
   }
   else
   {
      closeAlignLayerDialog();
   }
}   

void CGerberEducatorUi::OnEventEditGeometries() 
{
   if (getUiState() != GerberEducatorUiStateEditingGeometries)
   {
      closeDataTyperDialog();
      closeAlignLayerDialog();
      closeGerberEducator();
      closeReviewComponentsDialog();
   }

   if (getFileStruct() != NULL)
   {
      if (getUiState() == GerberEducatorUiStateEditingComponents)
      {
         setUiState(GerberEducatorUiStateSubEditingGeometries);
      }
      else
      {
         setUiState(GerberEducatorUiStateEditingGeometries);
         m_currentEditGeometryName.Empty();
         m_currentEditComponentRefdes.Empty();
      }

      if (m_reviewGeometriesDialog == NULL)
      {
	      m_reviewGeometriesDialog = new CGEReviewGeometriesDlg(*this);
	      m_reviewGeometriesDialog->Create(IDD_GE_ReviewGeometries);
      }

      getApp().setRunMode(runModeDataProtectedModelessDialog);
      //getMainFrame()->displayToolBars(false);
      getMainFrame()->disableToolBars();
      getMainFrame()->disableMenuItems();
      //getActiveView()->enableContextMenu(false);
	   m_reviewGeometriesDialog->ShowWindow(SW_SHOW);
	   m_reviewGeometriesDialog->UpdateWindow();

      arrangeWindows(*m_reviewGeometriesDialog,arrangeWindowsMethodMaximumNormal);

	   if (!m_reviewGeometriesDialog->isInitialized())
      {
		   closeReviewGeometriesDialog();
      }
   }
   else
   {
      closeReviewGeometriesDialog();
   }
}

void CGerberEducatorUi::OnEventEditComponents() 
{
   if (getUiState() != GerberEducatorUiStateEditingComponents)
   {
      closeDataTyperDialog();
      closeAlignLayerDialog();
      closeGerberEducator();
      closeReviewGeometriesDialog();
   }

   if (getFileStruct() != NULL)
   {
      setUiState(GerberEducatorUiStateEditingComponents);

      if (m_reviewComponentsDialog == NULL)
      {
	      m_reviewComponentsDialog = new CGEReviewInsertsDlg(*this);
	      m_reviewComponentsDialog->Create(IDD_GE_ReviewInserts);
      }

      getApp().setRunMode(runModeDataProtectedModelessDialog);
      //getMainFrame()->displayToolBars(false);
      getMainFrame()->disableToolBars();
      getMainFrame()->disableMenuItems();
      //getActiveView()->enableContextMenu(false);
	   m_reviewComponentsDialog->ShowWindow(SW_SHOW);
	   m_reviewComponentsDialog->UpdateWindow();

      arrangeWindows(*m_reviewComponentsDialog,arrangeWindowsMethodMaximumNormal);

	   if (!m_reviewComponentsDialog->isInitialized())
      {
		   closeReviewComponentsDialog();
      }
   }
   else
   {
      closeReviewComponentsDialog();
   }
}

void CGerberEducatorUi::OnEventCleanUpData()
{
   CGerberEducatorCleanUpDataDialog cleanUpDataDialog;

   if (cleanUpDataDialog.DoModal() == IDOK)
   {
      closeDataTyperDialog();
      closeGerberEducator();
      closeReviewGeometriesDialog();
      closeReviewComponentsDialog();

      FileStruct* educatorFileStruct = getFileStruct();

      if (educatorFileStruct != NULL)
      {
         //m_camCadDoc.clearSelected(true,true);

         if (cleanUpDataDialog.getRemoveUnusedLayersFlag())
         {
            m_camCadDoc.RemoveUnusedLayers();
         }

         if (cleanUpDataDialog.getChangeFileTypesFlag())
         {
            for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
            {
               POSITION oldPos = filePos;
               FileStruct* fileStruct = m_camCadDoc.getFileList().GetNext(filePos);

               if (fileStruct != educatorFileStruct)
               {
                  fileStruct->setBlockType(blockTypeUnknown);

                  BlockStruct* fileBlock = fileStruct->getBlock();
                  fileBlock->getDataList().empty();
               }
            }
         }

         if (cleanUpDataDialog.getRemoveNonGerberEducatorLayersFlag())
         {
            CCamCadDatabase camCadDatabase(m_camCadDoc);

            LayerStruct* padTop           = camCadDatabase.getLayer(ccLayerPadTop);
            LayerStruct* padBottom        = camCadDatabase.getLayer(ccLayerPadBottom);
            LayerStruct* pasteTop         = camCadDatabase.getLayer(ccLayerPasteTop);
            LayerStruct* pasteBottom      = camCadDatabase.getLayer(ccLayerPasteBottom);
            LayerStruct* assemblyTop      = camCadDatabase.getLayer(ccLayerAssemblyTop);
            LayerStruct* assemblyBottom   = camCadDatabase.getLayer(ccLayerAssemblyBottom);
            LayerStruct* centroidTop      = camCadDatabase.getLayer(ccLayerCentroidTop);
            LayerStruct* centroidBottom   = camCadDatabase.getLayer(ccLayerCentroidBottom);
            LayerStruct* drillHoles       = camCadDatabase.getLayer(ccLayerDrillHoles);
            LayerStruct* layerZero        = camCadDatabase.getDefinedLayer("0",true);

            CCamCadLayerMask layerMask(camCadDatabase);
            layerMask.addAll();
            layerMask.remove(padTop->getLayerIndex());
            layerMask.remove(padBottom->getLayerIndex());
            layerMask.remove(pasteTop->getLayerIndex());
            layerMask.remove(pasteBottom->getLayerIndex());
            layerMask.remove(assemblyTop->getLayerIndex());
            layerMask.remove(assemblyBottom->getLayerIndex());
            layerMask.remove(centroidTop->getLayerIndex());
            layerMask.remove(centroidBottom->getLayerIndex());
            layerMask.remove(drillHoles->getLayerIndex());
            layerMask.remove(layerZero->getLayerIndex());

            CString floatingLayerName = camCadDatabase.getCamCadLayerName(ccLayerFloat);
            LayerStruct* floatLayer = camCadDatabase.getLayer(floatingLayerName);

            if (floatLayer != NULL)
            {
               layerMask.remove(floatLayer->getLayerIndex());
            }

            camCadDatabase.deleteLayers(layerMask);
         }

         if (cleanUpDataDialog.getRemoveStandAloneCentroidsFlag())
         {
            CDataList& educatorDataList = educatorFileStruct->getBlock()->getDataList();

            for (POSITION pos = educatorDataList.GetHeadPosition();pos != NULL;)
            {
               POSITION oldPos = pos;
               DataStruct* data = educatorDataList.GetNext(pos);
               
               if (data->getDataType() == dataTypeInsert &&
                  data->getInsert()->getInsertType() == insertTypeCentroid)
               {
                  educatorDataList.RemoveAt(oldPos);
                  delete data;
               }
            }
         }

         m_camCadDoc.OnRedraw();
      }
   }
}

void CGerberEducatorUi::OnEventTerminateSession() 
{
   if (getUiState() == GerberEducatorUiStateSubEditingGeometries)
   {
      generateEvent(ID_GerberEducatorCommand_EditComponents);
   }
   else if (getUiState() == GerberEducatorUiStateAlignLayer)
   {
      generateEvent(ID_GerberEducatorCommand_SetupDataType);
   }
   else
   {
      closeDataTyperDialog();
      closeAlignLayerDialog();
      closeGerberEducator();
      closeReviewGeometriesDialog();
      closeReviewComponentsDialog();

      getCamCadDoc().getInsertTypeFilter().setToDefault();
      getCamCadDoc().getGraphicsClassFilter().setToDefault();

      getMainFrame()->getNavigator().setDoc(NULL);
      getMainFrame()->getNavigator().setDoc(&(getCamCadDoc()));

      deleteGerberEducatorUi();
      getApp().setRunMode(runModeNormal);

      CMainFrame* mainFrame = getMainFrame();
      //mainFrame->displayToolBars(true);
      mainFrame->enableToolBars(true);
      mainFrame->enableMenuItems();
      //getActiveView()->enableContextMenu(true);

      mainFrame->maximizeActiveChildFrame();

      //m_fileStruct = NULL;
   }
}

void CGerberEducatorUi::closeGerberEducator()
{
   m_camCadDoc.terminateGerberEducatorSession();
}

void CGerberEducatorUi::closeDataTyperDialog()
{
	if (m_dataTyperDialog != NULL)
   {
		m_dataTyperDialog->DestroyWindow();
   }

	delete m_dataTyperDialog;
	m_dataTyperDialog = NULL;
}

void CGerberEducatorUi::closeAlignLayerDialog()
{
	//if (m_alignLayerDialog != NULL)
 //  {
	//	m_alignLayerDialog->DestroyWindow();
 //  }

	//delete m_alignLayerDialog;

   getActiveView()->OnMeasureTerminate();
	m_alignLayerDialog = NULL;
}

void CGerberEducatorUi::closeReviewGeometriesDialog()
{
	if (m_reviewGeometriesDialog != NULL)
   {
		m_reviewGeometriesDialog->DestroyWindow();
   }

	delete m_reviewGeometriesDialog;
	m_reviewGeometriesDialog = NULL;
}

void CGerberEducatorUi::closeReviewComponentsDialog()
{
	if (m_reviewComponentsDialog != NULL)
   {
		m_reviewComponentsDialog->DestroyWindow();
   }

	delete m_reviewComponentsDialog;
	m_reviewComponentsDialog = NULL;
}

//_____________________________________________________________________________
CGerberEducatorColorsDialog::CGerberEducatorColorsDialog(CWnd* pParent /*=NULL*/)
: CResizingDialog(CGerberEducatorColorsDialog::IDD, pParent)
, m_initializedFlag(false)
//, m_colorBoxes(false)
{
   addFieldControl(IDC_ColorPalette,anchorLeft,growBoth);
}

CGerberEducatorColorsDialog::~CGerberEducatorColorsDialog()
{
}

void CGerberEducatorColorsDialog::setColors(const CGerberEducatorColorScheme& layerColors)
{
   m_layerColors = layerColors;

   if (m_initializedFlag)
   {
      UpdateData(false);
   }
}

void CGerberEducatorColorsDialog::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);

   if (m_initializedFlag && !(pDX->m_bSaveAndValidate))
   {
      for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
      {
         GerberEducatorColorTag colorTag = colorIterator.getNext();
         getColorBox(colorTag).SetBkColor(m_layerColors.getColor(colorTag));
      }
   }

   DDX_Control(pDX, IDC_SuGerberPadTopFlashes, m_suGerberPadTopFlashesStatic);
   DDX_Control(pDX, IDC_SuGerberPadTopDraws, m_suGerberPadTopDrawsStatic);
   DDX_Control(pDX, IDC_SuGerberPadBottomFlashes, m_suGerberPadBottomFlashesStatic);
   DDX_Control(pDX, IDC_SuGerberPadBottomDraws, m_suGerberPadBottomDrawsStatic);
   DDX_Control(pDX, IDC_SuGerberSilkscreenTop, m_suGerberSilkscreenTopStatic);
   DDX_Control(pDX, IDC_SuGerberSilkscreenBottom, m_suGerberSilkscreenBottomStatic);
   DDX_Control(pDX, IDC_SuCentroidTop, m_suCentroidTopStatic);
   DDX_Control(pDX, IDC_SuCentroidBottom, m_suCentroidBottomStatic);
   DDX_Control(pDX, IDC_GeUnmatchedPadTop, m_geUnmatchedPadTopStatic);
   DDX_Control(pDX, IDC_GeUnmatchedPadBottom, m_geUnmatchedPadBottomStatic);
   DDX_Control(pDX, IDC_GeUnmatchedCentroidTop, m_geUnmatchedCentroidTopStatic);
   DDX_Control(pDX, IDC_GeUnmatchedCentroidBottom, m_geUnmatchedCentroidBottomStatic);
   DDX_Control(pDX, IDC_GeSilkscreenTop, m_geSilkscreenTopStatic);
   DDX_Control(pDX, IDC_GeSilkscreenBottom, m_geSilkscreenBottomStatic);
   DDX_Control(pDX, IDC_GeCentroidTop, m_geCentroidTopStatic);
   DDX_Control(pDX, IDC_RgCentroidTop, m_rgCentroidTopStatic);
   DDX_Control(pDX, IDC_UaCentroidTop, m_uaCentroidTopStatic);
   DDX_Control(pDX, IDC_ApCentroidTop, m_apCentroidTopStatic);
   DDX_Control(pDX, IDC_GeCentroidBottom, m_geCentroidBottomStatic);
   DDX_Control(pDX, IDC_RgCentroidBottom, m_rgCentroidBottomStatic);
   DDX_Control(pDX, IDC_UaCentroidBottom, m_uaCentroidBottomStatic);
   DDX_Control(pDX, IDC_ApCentroidBottom, m_apCentroidBottomStatic);
   DDX_Control(pDX, IDC_GeSmdCompPadTop, m_geSmdCompPadTopStatic);
   DDX_Control(pDX, IDC_RgSmdCompPadTop, m_rgSmdCompPadTopStatic);
   DDX_Control(pDX, IDC_UaSmdCompPadTop, m_uaSmdCompPadTopStatic);
   DDX_Control(pDX, IDC_ApSmdCompPadTop, m_apSmdCompPadTopStatic);
   DDX_Control(pDX, IDC_GeSmdCompPadBottom, m_geSmdCompPadBottomStatic);
   DDX_Control(pDX, IDC_RgSmdCompPadBottom, m_rgSmdCompPadBottomStatic);
   DDX_Control(pDX, IDC_UaSmdCompPadBottom, m_uaSmdCompPadBottomStatic);
   DDX_Control(pDX, IDC_ApSmdCompPadBottom, m_apSmdCompPadBottomStatic);
   DDX_Control(pDX, IDC_GeThCompPadTop, m_geThCompPadTopStatic);
   DDX_Control(pDX, IDC_RgThCompPadTop, m_rgThCompPadTopStatic);
   DDX_Control(pDX, IDC_UaThCompPadTop, m_uaThCompPadTopStatic);
   DDX_Control(pDX, IDC_ApThCompPadTop, m_apThCompPadTopStatic);
   DDX_Control(pDX, IDC_GeThCompPadBottom, m_geThCompPadBottomStatic);
   DDX_Control(pDX, IDC_RgThCompPadBottom, m_rgThCompPadBottomStatic);
   DDX_Control(pDX, IDC_UaThCompPadBottom, m_uaThCompPadBottomStatic);
   DDX_Control(pDX, IDC_ApThCompPadBottom, m_apThCompPadBottomStatic);
   DDX_Control(pDX, IDC_GeCompOutlineTop, m_geCompOutlineTopStatic);
   DDX_Control(pDX, IDC_RgCompOutlineTop, m_rgCompOutlineTopStatic);
   DDX_Control(pDX, IDC_UaCompOutlineTop, m_uaCompOutlineTopStatic);
   DDX_Control(pDX, IDC_ApCompOutlineTop, m_apCompOutlineTopStatic);
   DDX_Control(pDX, IDC_GeCompOutlineBottom, m_geCompOutlineBottomStatic);
   DDX_Control(pDX, IDC_RgCompOutlineBottom, m_rgCompOutlineBottomStatic);
   DDX_Control(pDX, IDC_UaCompOutlineBottom, m_uaCompOutlineBottomStatic);
   DDX_Control(pDX, IDC_ApCompOutlineBottom, m_apCompOutlineBottomStatic);
   DDX_Control(pDX, IDC_ColorPalette, m_colorPaletteStatic);
   DDX_Control(pDX, IDC_CurrentColor, m_currentColorStatic);
   DDX_Control(pDX, IDC_SuUnknown, m_suUnknownStatic);
   DDX_Control(pDX, IDC_DummyDefault, m_dummyDefault);

   if (m_initializedFlag && pDX->m_bSaveAndValidate)
   {
      for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
      {
         GerberEducatorColorTag colorTag = colorIterator.getNext();
         m_layerColors.setColor(colorTag,getColorBox(colorTag).getBkColor());
      }
   }
}

CStaticLabel& CGerberEducatorColorsDialog::getColorBox(GerberEducatorColorTag colorTag)
{
   CStaticLabel* colorBox = m_colorBox[colorTag];

   if (colorBox == NULL)
   {
      colorBox = &m_dummyDefault;
   }

   return *colorBox;
}

CStaticLabel* CGerberEducatorColorsDialog::getColorBox(CPoint point)
{
   CStaticLabel* colorBox = NULL;
   CRect rect;
   ClientToScreen(&point);

   for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
   {
      GerberEducatorColorTag colorTag = colorIterator.getNext();
      colorBox = &(getColorBox(colorTag));
      colorBox->GetWindowRect(&rect);

      if (rect.PtInRect(point))
      {
         break;
      }

      colorBox = NULL;
   }

   return colorBox;
}

BEGIN_MESSAGE_MAP(CGerberEducatorColorsDialog, CResizingDialog)
   ON_BN_CLICKED(ID_Save, OnBnClickedSave)
   ON_BN_CLICKED(ID_Restore, OnBnClickedRestore)
   ON_BN_CLICKED(ID_RestoreDefault, OnBnClickedRestoreDefault)
   ON_WM_SIZE()
   ON_WM_LBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CGerberEducatorColorsDialog message handlers

BOOL CGerberEducatorColorsDialog::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   for (EnumIterator(GerberEducatorColorTag,colorIterator);colorIterator.hasNext();)
   {
      GerberEducatorColorTag colorTag = colorIterator.getNext();
      m_colorBox[colorTag] = NULL;
   }

   m_colorBox[gerberEducatorSetupDataPadTopFlashesColor]     = &m_suGerberPadTopFlashesStatic;
   m_colorBox[gerberEducatorSetupDataPadTopDrawsColor]       = &m_suGerberPadTopDrawsStatic;
   m_colorBox[gerberEducatorSetupDataPadBottomFlashesColor]  = &m_suGerberPadBottomFlashesStatic;
   m_colorBox[gerberEducatorSetupDataPadBottomDrawsColor]    = &m_suGerberPadBottomDrawsStatic;
   m_colorBox[gerberEducatorSetupDataSilkscreenTopColor]     = &m_suGerberSilkscreenTopStatic;
   m_colorBox[gerberEducatorSetupDataSilkscreenBottomColor]  = &m_suGerberSilkscreenBottomStatic;
   m_colorBox[gerberEducatorSetupDataCentroidTopColor]       = &m_suCentroidTopStatic;
   m_colorBox[gerberEducatorSetupDataCentroidBottomColor]    = &m_suCentroidBottomStatic;
   m_colorBox[gerberEducatorSetupDataUnknownColor]           = &m_suUnknownStatic;

   m_colorBox[gerberEducatorCreateGeometriesUnmatchedPadTopColor]         = &m_geUnmatchedPadTopStatic;
   m_colorBox[gerberEducatorCreateGeometriesUnmatchedPadBottomColor]      = &m_geUnmatchedPadBottomStatic;
   m_colorBox[gerberEducatorCreateGeometriesUnmatchedCentroidTopColor]    = &m_geUnmatchedCentroidTopStatic;
   m_colorBox[gerberEducatorCreateGeometriesUnmatchedCentroidBottomColor] = &m_geUnmatchedCentroidBottomStatic;
   m_colorBox[gerberEducatorCreateGeometriesSilkscreenTopColor]           = &m_geSilkscreenTopStatic;
   m_colorBox[gerberEducatorCreateGeometriesSilkscreenBottomColor]        = &m_geSilkscreenBottomStatic;
   m_colorBox[gerberEducatorCreateGeometriesCentroidTopColor]             = &m_geCentroidTopStatic;
   m_colorBox[gerberEducatorCreateGeometriesCentroidBottomColor]          = &m_geCentroidBottomStatic;
   m_colorBox[gerberEducatorCreateGeometriesSmdPadTopColor]               = &m_geSmdCompPadTopStatic;
   m_colorBox[gerberEducatorCreateGeometriesSmdPadBottomColor]            = &m_geSmdCompPadBottomStatic;
   m_colorBox[gerberEducatorCreateGeometriesThPadTopColor]                = &m_geThCompPadTopStatic;
   m_colorBox[gerberEducatorCreateGeometriesThPadBottomColor]             = &m_geThCompPadBottomStatic;
   m_colorBox[gerberEducatorCreateGeometriesComponentOutlineTopColor]     = &m_geCompOutlineTopStatic;
   m_colorBox[gerberEducatorCreateGeometriesComponentOutlineBottomColor]  = &m_geCompOutlineBottomStatic;

   m_colorBox[gerberEducatorReviewGeometriesCentroidTopColor]             = &m_rgCentroidTopStatic;
   m_colorBox[gerberEducatorReviewGeometriesCentroidBottomColor]          = &m_rgCentroidBottomStatic;
   m_colorBox[gerberEducatorReviewGeometriesSmdPadTopColor]               = &m_rgSmdCompPadTopStatic;
   m_colorBox[gerberEducatorReviewGeometriesSmdPadBottomColor]            = &m_rgSmdCompPadBottomStatic;
   m_colorBox[gerberEducatorReviewGeometriesThPadTopColor]                = &m_rgThCompPadTopStatic;
   m_colorBox[gerberEducatorReviewGeometriesThPadBottomColor]             = &m_rgThCompPadBottomStatic;
   m_colorBox[gerberEducatorReviewGeometriesComponentOutlineTopColor]     = &m_rgCompOutlineTopStatic;
   m_colorBox[gerberEducatorReviewGeometriesComponentOutlineBottomColor]  = &m_rgCompOutlineBottomStatic;

   m_colorBox[gerberEducatorReviewComponentCentroidTopColor]             = &m_uaCentroidTopStatic;
   m_colorBox[gerberEducatorReviewComponentCentroidBottomColor]          = &m_uaCentroidBottomStatic;
   m_colorBox[gerberEducatorReviewComponentSmdPadTopColor]               = &m_uaSmdCompPadTopStatic;
   m_colorBox[gerberEducatorReviewComponentSmdPadBottomColor]            = &m_uaSmdCompPadBottomStatic;
   m_colorBox[gerberEducatorReviewComponentThPadTopColor]                = &m_uaThCompPadTopStatic;
   m_colorBox[gerberEducatorReviewComponentThPadBottomColor]             = &m_uaThCompPadBottomStatic;
   m_colorBox[gerberEducatorReviewComponentComponentOutlineTopColor]     = &m_uaCompOutlineTopStatic;
   m_colorBox[gerberEducatorReviewComponentComponentOutlineBottomColor]  = &m_uaCompOutlineBottomStatic;

   m_colorBox[gerberEducatorReviewComponentApprovedTopColor]             = &m_apCompOutlineTopStatic;
   m_colorBox[gerberEducatorReviewComponentApprovedBottomColor]          = &m_apCompOutlineBottomStatic;

   //m_colorBox[gerberEducatorCentroidTopColor]             = &m_apCentroidTopStatic;
   //m_colorBox[gerberEducatorCentroidBottomColor]          = &m_apCentroidBottomStatic;
   //m_colorBox[gerberEducatorSmdCompPadTopColor]           = &m_apSmdCompPadTopStatic;
   //m_colorBox[gerberEducatorSmdCompPadBottomColor]        = &m_apSmdCompPadBottomStatic;
   //m_colorBox[gerberEducatorThCompPadTopColor]            = &m_apThCompPadTopStatic;
   //m_colorBox[gerberEducatorThCompPadBottomColor]         = &m_apThCompPadBottomStatic;
   //m_colorBox[gerberEducatorCompOutlineTopColor]          = &m_apCompOutlineTopStatic;
   //m_colorBox[gerberEducatorCompOutlineBottomColor]       = &m_apCompOutlineBottomStatic;

   m_colorPaletteStatic.setColorModel(colorModelHsl);
   m_colorPaletteStatic.setComponentIndices(2,0);  // 2-Lum, 0-Hue
   m_colorPaletteStatic.setMinComponent(0,.2);  // Lum
   m_colorPaletteStatic.setMaxComponent(0,.9);
   m_colorPaletteStatic.setMinComponent(1,0.);  // Hue
   m_colorPaletteStatic.setMaxComponent(1,.96);
   m_colorPaletteStatic.setMinComponent(2,1.);  // Sat
   m_colorPaletteStatic.setMaxComponent(2,1.);

   m_dummyDefault.ShowWindow(SW_HIDE);

   m_initializedFlag = true;

   UpdateData(false);
   Invalidate();

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CGerberEducatorColorsDialog::OnBnClickedSave()
{
   UpdateData(true);

   m_layerColors.storeInRegistry();
}

void CGerberEducatorColorsDialog::OnBnClickedRestore()
{
   m_layerColors.loadFromRegistry();

   UpdateData(false);
   Invalidate();
}

void CGerberEducatorColorsDialog::OnBnClickedRestoreDefault()
{
   m_layerColors.initializeColors();

   UpdateData(false);

   Invalidate();
}

void CGerberEducatorColorsDialog::OnSize(UINT nType, int cx, int cy)
{
   CResizingDialog::OnSize(nType, cx, cy);

   CRect rect;
   m_colorPaletteStatic.GetClientRect(&rect);

   if (m_colorPaletteStatic.itemCountsChange(rect))
   {
      m_colorPaletteStatic.invalidatePalette();
      m_colorPaletteStatic.Invalidate();
   }
}

void CGerberEducatorColorsDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
   CStaticLabel* colorBox = this->getColorBox(point);

   if (colorBox != NULL)
   {
      COLORREF color = m_currentColorStatic.getBkColor();
      colorBox->SetBkColor(color);
      colorBox->Invalidate();
   }

   CResizingDialog::OnLButtonDown(nFlags, point);
}

void CGerberEducatorColorsDialog::OnRButtonDown(UINT nFlags, CPoint point)
{
   CStaticLabel* colorBox = this->getColorBox(point);

   if (colorBox != NULL)
   {
      COLORREF color = colorBox->getBkColor();
      m_currentColorStatic.SetBkColor(color);
      m_currentColorStatic.Invalidate();

      m_colorPaletteStatic.setSelectedItemIndex(-1);
   }
   else
   {
      ClientToScreen(&point);
      m_colorPaletteStatic.ScreenToClient(&point);
      int selectedIndex = m_colorPaletteStatic.setSelected(point);

      if (selectedIndex >= 0)
      {
         COLORREF color = m_colorPaletteStatic.getSelectedItemColor().getColor();
         m_currentColorStatic.SetBkColor(color);
         m_currentColorStatic.Invalidate();
      }
   }

   CResizingDialog::OnRButtonDown(nFlags, point);
}

//_____________________________________________________________________________
IMPLEMENT_DYNCREATE(CGerberEducatorCleanUpDataDialog, CDialog)

CGerberEducatorCleanUpDataDialog::CGerberEducatorCleanUpDataDialog(CWnd* pParent /*=NULL*/)
: CDialog(CGerberEducatorCleanUpDataDialog::IDD, pParent)
, m_removeUnusedLayers(TRUE)
, m_removeNonGerberEducatorLayers(FALSE)
, m_changeFileTypes(FALSE)
, m_removeStandAloneCentroids(FALSE)
{
}

CGerberEducatorCleanUpDataDialog::~CGerberEducatorCleanUpDataDialog()
{
}

void CGerberEducatorCleanUpDataDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_CHECK1, m_removeUnusedLayers);
   DDX_Check(pDX, IDC_CHECK8, m_removeNonGerberEducatorLayers);
   DDX_Check(pDX, IDC_CHECK9, m_changeFileTypes);
   DDX_Check(pDX, IDC_CHECK10, m_removeStandAloneCentroids);
}

BOOL CGerberEducatorCleanUpDataDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CGerberEducatorCleanUpDataDialog, CDialog)
END_MESSAGE_MAP()


// CGerberEducatorCleanUpDataDialog message handlers
