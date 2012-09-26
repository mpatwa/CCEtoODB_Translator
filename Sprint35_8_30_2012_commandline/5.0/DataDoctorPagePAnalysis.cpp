// $Header: /CAMCAD/4.6/DataDoctorPagePAnalysis.cpp 3     4/06/07 4:47p Rick Faltersack $

#include "stdafx.h"

#include "DataDoctorDialog.h"
#include "DataDoctorPagePAnalysis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDataDoctorParallelAnalysisPropertyPage, CDataDoctorPropertyPageType)

CDataDoctorParallelAnalysisPropertyPage::CDataDoctorParallelAnalysisPropertyPage(CDataDoctorPropertySheet& parent)
	: CDataDoctorPropertyPageType(CDataDoctorParallelAnalysisPropertyPage::IDD)
   , m_parent(parent)
   , m_analyzeResistors(true)
   , m_analyzeCapacitors(true)
   , m_analyzeInductors(true)
   , m_ignoreUnloadedComponents(true)
   , m_mergeValues(true)
   , m_updatePending(false)
{
}

CDataDoctorParallelAnalysisPropertyPage::~CDataDoctorParallelAnalysisPropertyPage()
{
}

void CDataDoctorParallelAnalysisPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CDataDoctorPropertyPageType::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_AnalyzeResistorsCheckBox, m_analyzeResistors);
   DDX_Check(pDX, IDC_AnalyzeCapacitorsCheckBox, m_analyzeCapacitors);
   DDX_Check(pDX, IDC_AnalyzeInductorsCheckBox, m_analyzeInductors);
   DDX_Check(pDX, IDC_IgnoreUnloadedComponentsCheckBox, m_ignoreUnloadedComponents);
   DDX_Check(pDX, IDC_MergeValuesCheckBox, m_mergeValues);
}


BEGIN_MESSAGE_MAP(CDataDoctorParallelAnalysisPropertyPage, CDataDoctorPropertyPageType)
   ON_BN_CLICKED(IDC_ProcessButton, OnBnClickedProcess)
   ON_BN_CLICKED(IDC_ClearResultsButton, OnBnClickedClearResults)
END_MESSAGE_MAP()


// CDataDoctorParallelAnalysisPropertyPage message handlers

void CDataDoctorParallelAnalysisPropertyPage::OnBnClickedProcess()
{
   UpdateData(true);

   getParent().getDataDoctor().performParallelAnalysis(
      m_analyzeResistors != 0,m_analyzeCapacitors != 0,m_analyzeInductors != 0,
      m_ignoreUnloadedComponents != 0,m_mergeValues != 0);

   getParent().getDataDoctor().displayParallelAnalysisReport();
}

void CDataDoctorParallelAnalysisPropertyPage::OnBnClickedClearResults()
{
   UpdateData(true);

   getParent().getDataDoctor().removeParallelAnalysisResults();

   //getParent().getDataDoctor().displayParallelAnalysisReport();
}


BOOL CDataDoctorParallelAnalysisPropertyPage::OnSetActive()
{
   getParent().SaveAsInitialPage(DdParallelAnalysisPage);

   if (IsUpdatePending())
      m_updatePending = false; // this page has no update function, apparantly no update needs

   return CDataDoctorPropertyPageType::OnSetActive();
}
