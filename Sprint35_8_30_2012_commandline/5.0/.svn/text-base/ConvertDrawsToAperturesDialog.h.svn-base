// $Header: /CAMCAD/4.5/ConvertDrawsToAperturesDialog.h 14    1/27/05 7:42p Kurt Van Ness $

/*****************************************************************************/
/*  
    Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2004. All Rights Reserved.
*/ 

#if !defined(__ConvertDrawsToFlashesDialog_h__)
#define __ConvertDrawsToFlashesDialog_h__

#pragma once

#include <afxwin.h>
#include "Resource.h"

class CFlashRecognizerParameters;

//_____________________________________________________________________________
class CConvertDrawsToAperturesDialog : public CDialog
{
	DECLARE_DYNAMIC(CConvertDrawsToAperturesDialog)

private:
   CFlashRecognizerParameters& m_parameters;
   CString m_minFeatureSize;
   CString m_maxFeatureSize;
   CString m_ignoreWidthBelow;
   CString m_minSetWidth;
   CString m_minimumCornerRadius;
   CString m_roundedCornerTolerance;
   CString m_radialTolerancePercentage;
   CString m_baseUnit;
   CString m_apertureNormalizationAffinity;
   CString m_rectangleCoordTolerance;
   CString m_maxClusterSegments;
   CString m_clusterBoundaryAffinity;
   CString m_clusterBoundaryFitnessFactor;

   BOOL m_selected;
   BOOL m_generateClusterDisplay;
   BOOL m_searchForBoundedClusters;
   BOOL m_mergeClustersIntoPolygons;
   BOOL m_generateDiagnosticLayers;

   int m_advancedWidth;
   int m_basicWidth;
   bool m_advancedFlag;

public:
	CConvertDrawsToAperturesDialog(CFlashRecognizerParameters& parameters,CWnd* pParent = NULL);
	virtual ~CConvertDrawsToAperturesDialog();

// Dialog Data
	enum { IDD = IDD_ConvertDrawsToApertures };

   void getParameters(CFlashRecognizerParameters& parameters);
   void setParameters(const CFlashRecognizerParameters& parameters);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   void updateAdvancedOptions();

	DECLARE_MESSAGE_MAP()
public:
   //{{AFX_MSG(CConvertDrawsToAperturesDialog)
   afx_msg void OnBnClickedOk();
   afx_msg void OnBnClickedLoadDefaults();
   afx_msg void OnBnClickedAdvanced();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
};

#endif

