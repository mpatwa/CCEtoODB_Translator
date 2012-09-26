// $Header: /CAMCAD/4.5/ConvertDrawsToAperturesDialog.cpp 16    1/27/05 7:42p Kurt Van Ness $

#include "StdAfx.h"
#include "Resource.h"
#include "ConvertDrawsToAperturesDialog.h"
#include "FlashRecognizer.h"
#include "CamCadDatabase.h"
#include ".\convertdrawstoaperturesdialog.h"

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CConvertDrawsToAperturesDialog, CDialog)
CConvertDrawsToAperturesDialog::CConvertDrawsToAperturesDialog(CFlashRecognizerParameters& parameters,CWnd* pParent /*=NULL*/)
	: CDialog(CConvertDrawsToAperturesDialog::IDD, pParent)
   , m_parameters(parameters)
   , m_minFeatureSize(_T(""))
   , m_maxFeatureSize(_T(""))
   , m_ignoreWidthBelow(_T(""))
   , m_minSetWidth(_T(""))
   , m_minimumCornerRadius(_T(""))
   , m_roundedCornerTolerance(_T(""))
   , m_radialTolerancePercentage(_T(""))
   , m_baseUnit(_T(""))
   , m_apertureNormalizationAffinity(_T(""))
   , m_selected(FALSE)
   , m_generateClusterDisplay(FALSE)
   , m_searchForBoundedClusters(FALSE)
   , m_rectangleCoordTolerance(_T(""))
   , m_maxClusterSegments(_T(""))
   , m_clusterBoundaryAffinity(_T(""))
   , m_clusterBoundaryFitnessFactor(_T(""))
   , m_mergeClustersIntoPolygons(FALSE)
   , m_generateDiagnosticLayers(FALSE)
{
   setParameters(m_parameters);
   m_advancedFlag = false;
}

CConvertDrawsToAperturesDialog::~CConvertDrawsToAperturesDialog()
{
}

void CConvertDrawsToAperturesDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_MinFeatureSize, m_minFeatureSize);
   DDX_Text(pDX, IDC_MaxFeatureSize, m_maxFeatureSize);
   DDX_Text(pDX, IDC_IgnoreWidth, m_ignoreWidthBelow);
   DDX_Text(pDX, IDC_MinWidthToSet, m_minSetWidth);
   DDX_Text(pDX, IDC_MinCornerRadius, m_minimumCornerRadius);
   DDX_Text(pDX, IDC_RoundedCornerTolerance, m_roundedCornerTolerance);
   DDX_Text(pDX, IDC_RadialTolerancePercentage, m_radialTolerancePercentage);
   DDX_Check(pDX, IDC_SelectedCheckBox, m_selected);
   DDX_Check(pDX, IDC_ClusterDisplayCheckBox, m_generateClusterDisplay);
   DDX_Text(pDX, IDC_BaseUnit, m_baseUnit);
   DDX_Text(pDX, IDC_AffinityDistance, m_apertureNormalizationAffinity);
   DDX_Check(pDX, IDC_SearchForBoundedClustersCheckBox, m_searchForBoundedClusters);
   DDX_Text(pDX, IDC_RectangleCoordTolerance, m_rectangleCoordTolerance);
   DDX_Text(pDX, IDC_MaxClusterSegments, m_maxClusterSegments);
   DDX_Text(pDX, IDC_ClusterBoundaryAffinityDistance, m_clusterBoundaryAffinity);
   DDX_Text(pDX, IDC_ClusterBoundaryFitnessFactor, m_clusterBoundaryFitnessFactor);
   DDX_Check(pDX, IDC_MergeClustersIntoPolygonsCheckBox, m_mergeClustersIntoPolygons);
   DDX_Check(pDX, IDC_DiagnosticLayerCheckBox, m_generateDiagnosticLayers);
}

void CConvertDrawsToAperturesDialog::getParameters(CFlashRecognizerParameters& parameters)
{
   parameters.setMinimumFeatureSize(atof(m_minFeatureSize));
   parameters.setMaximumFeatureSize(atof(m_maxFeatureSize));
   parameters.setIgnoreWidthLimit(atof(m_ignoreWidthBelow));
   parameters.setMinimumWidthToSet(atof(m_minSetWidth));
   parameters.setMinimumCornerRadius(atof(m_minimumCornerRadius));
   parameters.setRoundedCornerRadiusTolerance(atof(m_roundedCornerTolerance));
   parameters.setRadialTolerance(atof(m_radialTolerancePercentage)/100.);
   parameters.setBaseUnit(atof(m_baseUnit));
   parameters.setApertureNormalizationAffinity(atof(m_apertureNormalizationAffinity));
   parameters.setClusterBoundaryAffinity(atof(m_clusterBoundaryAffinity));
   parameters.setClusterBoundaryFitnessFactor(atof(m_clusterBoundaryFitnessFactor));
   parameters.setRectangleCoordTolerance(atof(m_rectangleCoordTolerance));
   parameters.setMaxClusterSegments(atoi(m_maxClusterSegments));

   parameters.setSelectedFlag(m_selected != 0);
   parameters.setGenerateClusterDisplayFlag(m_generateClusterDisplay     != 0);
   parameters.setGenerateDiagnosticLayersFlag(m_generateDiagnosticLayers != 0);
   parameters.setSearchForBoundedClusters(m_searchForBoundedClusters     != 0);
   parameters.setMergeClustersIntoPolygon(m_mergeClustersIntoPolygons    != 0);
}

void CConvertDrawsToAperturesDialog::setParameters(const CFlashRecognizerParameters& parameters)
{
   CString format = (::isEnglish(parameters.getCamCadDatabase().getPageUnits()) ? "%.5f" : "%.4f");
   CString value;

   value.Format(format,parameters.getMinimumFeatureSize());
   m_minFeatureSize = value;

   value.Format(format,parameters.getMaximumFeatureSize());
   m_maxFeatureSize = value;

   value.Format(format,parameters.getMinimumCornerRadius());
   m_minimumCornerRadius = value;

   value.Format(format,parameters.getRoundedCornerRadiusTolerance());
   m_roundedCornerTolerance = value;

   value.Format(format,parameters.getRadialTolerance()*100.);
   m_radialTolerancePercentage = value;

   value.Format(format,parameters.getIgnoreWidthLimit());
   m_ignoreWidthBelow = value;

   value.Format(format,parameters.getMinimumWidthToSet());
   m_minSetWidth = value;

   value.Format(format,parameters.getBaseUnit());
   m_baseUnit = value;

   value.Format(format,parameters.getApertureNormalizationAffinity());
   m_apertureNormalizationAffinity = value;

   value.Format(format,parameters.getClusterBoundaryAffinity());
   m_clusterBoundaryAffinity = value;

   value.Format(format,parameters.getClusterBoundaryFitnessFactor());
   m_clusterBoundaryFitnessFactor = value;

   value.Format(format,parameters.getRectangleCoordTolerance());
   m_rectangleCoordTolerance = value;

   value.Format("%d",parameters.getMaxClusterSegments());
   m_maxClusterSegments = value;

   m_selected                  = parameters.getSelectedFlag();
   m_generateClusterDisplay    = parameters.getGenerateClusterDisplayFlag();
   m_generateDiagnosticLayers  = parameters.getGenerateDiagnosticLayersFlag();
   m_searchForBoundedClusters  = parameters.getSearchForBoundedClusters();
   m_mergeClustersIntoPolygons = parameters.getMergeClustersIntoPolygon();

   //UpdateData(false);
}

void CConvertDrawsToAperturesDialog::updateAdvancedOptions()
{
   GetDlgItem(ID_Advanced)->SetWindowText(m_advancedFlag ? "Hide Advanced" : "Show Advanced");

   int show = (m_advancedFlag ? SW_SHOW : SW_HIDE);

   GetDlgItem(IDC_ClusterDisplayCheckBox)->ShowWindow(show);
   GetDlgItem(IDC_DiagnosticLayerCheckBox)->ShowWindow(show);
   GetDlgItem(IDC_SearchForBoundedClustersCheckBox)->ShowWindow(show);
   GetDlgItem(IDC_MergeClustersIntoPolygonsCheckBox)->ShowWindow(show);

   WINDOWPLACEMENT windowPlacement;
   GetWindowPlacement(&windowPlacement);
   windowPlacement.rcNormalPosition.right = windowPlacement.rcNormalPosition.left + (m_advancedFlag ? m_advancedWidth : m_basicWidth);
   SetWindowPlacement(&windowPlacement);
}

BEGIN_MESSAGE_MAP(CConvertDrawsToAperturesDialog, CDialog)
   //{{AFX_MSG_MAP(CConvertDrawsToAperturesDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(ID_LoadDefaults, OnBnClickedLoadDefaults)
   ON_BN_CLICKED(ID_Advanced, OnBnClickedAdvanced)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CConvertDrawsToAperturesDialog message handlers
BOOL CConvertDrawsToAperturesDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   CRect dialogRect;
   GetWindowRect(dialogRect);

   CRect widthsRect;
   GetDlgItem(IDC_WidthsStatic)->GetWindowRect(widthsRect);

   m_advancedWidth = dialogRect.Width();
   m_basicWidth    = widthsRect.left - dialogRect.left;

   updateAdvancedOptions();

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CConvertDrawsToAperturesDialog::OnBnClickedOk()
{
   // TODO: Add your control notification handler code here
   UpdateData(true);
   getParameters(m_parameters);
   OnOK();
}

void CConvertDrawsToAperturesDialog::OnBnClickedLoadDefaults()
{
   CFlashRecognizerParameters defaultParameters(m_parameters);
   defaultParameters.setToDefaults();
   setParameters(defaultParameters);
   UpdateData(false);
}

void CConvertDrawsToAperturesDialog::OnBnClickedAdvanced()
{
   m_advancedFlag = !m_advancedFlag;

   updateAdvancedOptions();
}
