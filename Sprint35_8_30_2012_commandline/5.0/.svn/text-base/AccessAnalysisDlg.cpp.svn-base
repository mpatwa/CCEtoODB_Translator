// $Header: /CAMCAD/5.0/AccessAnalysisDlg.cpp 177   6/17/07 8:49p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// ACCESSANALYSISDLG.CPP

#include "stdafx.h"
#include "AccessAnalysisDlg.h"
#include "DirDlg.h"
#include <direct.h>
#include "GenerateCentroidAndOutline.h"
#include "Sm_Anal.h"
#include "PcbUtil.h"
#include "GeomLib.h"
#include "graph.h"
#include "rgn.h"
#include "float.h"
#include "LyrManip.h"
#include "xform.h"
#include "PolyLib.h"
#include "mainfrm.h"
#include "net_util.h"
#include "lic.h"
#include "crypt.h"
#include "CCEtoODB.h"
#include "Drc_util.h"
#include "RealPart.h"
#include "MultipleMachine.h"
#include "Debug.h"
#include "EnumIterator.h"
#include "Gauge.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);

static bool GetXYonExposeMetal(CCEtoODBDoc *doc, FileStruct *pPCBFile, DataStruct *exposeData, double featureSize, double &x, double &y);
static void DeleteFeatureSize(CCEtoODBDoc *doc, FileStruct *pcbFile);
//static CString GenerateSortKey(CString name);

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// Tools->DFT Analysis->Access Analysis
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
void CCEtoODBDoc::OnDftanalysisAccessanalysis()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft))
   {
      ErrorAccess("Access Analysis!");
      return;
   }*/

   FileStruct *pcbFile = this->getFileList().GetOnlyShown(blockTypePcb);

   if (pcbFile)
   {
      /*if (GetCurrentMachine(*pcbFile) != NULL )
      {
         if (GetCurrentMachine(*pcbFile)->GetDFTSolution() == NULL)
         {
            ErrorMessage("The selected Machine " + GetCurrentMachine(*pcbFile)->GetName() + " does not use DFT Solution.\n", "Access Analysis");
            return;
         }
         else if (GetCurrentMachine(*pcbFile)->GetDFTSolution() != GetCurrentDFTSolution(*pcbFile))
         {
            // The current DFT Solution is not the same as the DFT Solution of the current machine so fix it
            SetCurrentDFTSolution(*pcbFile,GetCurrentMachine(*pcbFile)->GetDFTSolution());
         }
      }*/

      generate_CENTROIDS(this);
      CAccessAnalysisDlg dlg(*this, *pcbFile, AfxGetMainWnd());
      if (dlg.GetError() == DFT_ERROR_NONE)
         dlg.DoModal();

      UpdateAllViews(NULL);
   }
   else
   {
      int pcbVisible =  this->getFileList().GetVisibleCount();
      if (pcbVisible < 1)
         ErrorMessage("No visible PCB file detected.\n\nAccess Analysis only supports single visible PCB file.", "Access Analysis");
      else if (pcbVisible > 1)
         ErrorMessage("Multiple visible PCB files detected.\n\nAccess Analysis only supports single visible PCB file.", "Access Analysis");
   }
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CAccessAnalysisDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CAccessAnalysisDlg, CDialog)
CAccessAnalysisDlg::CAccessAnalysisDlg(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CWnd* pParent)
: CDialog(CAccessAnalysisDlg::IDD, pParent)
, m_camCadDoc(camCadDoc)
, m_pPCBFile(pPCBFile)
, m_heightAnalysisDlg(&camCadDoc)
, m_physicalConstraintsDlg(&camCadDoc)
, m_targetTypesDlg(&camCadDoc)
, m_optionDlg(*this,&camCadDoc)
, m_createUnconnectedNet(false)
, m_explodedMetalLayer(false)
, m_processedSMAnalysis(false)
, m_DFTSolution("AASolution_Temp", false/*isFlipped*/, camCadDoc.getSettings().getPageUnits())
, m_ExposedMetalMap(camCadDoc)
, m_metalAnalysis(camCadDoc, pPCBFile, *m_DFTSolution.GetTestPlan())
{
   m_heightAnalysisDlg.SetParent(this);
   m_physicalConstraintsDlg.SetParent(this);
   //m_optionDlg.SetParent(this);
   m_targetTypesDlg.SetParent(this);
   m_error = DFT_ERROR_NONE;

   // Set test plan 
   /*CDFTSolution *curSolution = camCadDoc.GetCurrentDFTSolution(m_pPCBFile);
   if (curSolution != NULL)
   {
      //Call ValidateSettings ONLY when we are getting the DFT solution from the CCZ read.
      curSolution->GetTestPlan()->ValidateSettings(camCadDoc.getSettings().getPageUnits());
      m_DFTSolution.SetTestPlan(*curSolution->GetTestPlan());
   }
   else
      m_DFTSolution.GetTestPlan()->LoadFile(getApp().getUserPath() + "\\default.pln", getCamCadDoc().getSettings().getPageUnits());*/
}

CAccessAnalysisDlg::~CAccessAnalysisDlg()
{
   m_metalAnalysis.empty();

#if !defined (_DEBUG) && !defined (_RDEBUG)
   CleanUpUnusedLayer(&m_camCadDoc);
#endif
}

void CAccessAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDOK, m_btnOK);
   DDX_Control(pDX, IDC_DisplayReport, m_btnReport);
   DDX_Control(pDX, IDC_BUTTON_PROCESS, m_btnProcess);
}

BEGIN_MESSAGE_MAP(CAccessAnalysisDlg, CDialog)
   ON_BN_CLICKED(IDC_BUTTON_PROCESS, OnBnClickedProcess)
   ON_BN_CLICKED(IDC_DisplayReport, OnBnClickedDisplayReport)
   ON_WM_PARENTNOTIFY()
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAccessAnalysisDlg, CDialog)
END_EVENTSINK_MAP()

BOOL CAccessAnalysisDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_aaResultsGrid.AttachGrid(this, IDC_AAResultsGridStatic);
   
   CRect rcGridWindow;
   GetDlgItem( IDC_AAResultsGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_aaResultsGrid.SizeToFit(rcGridWindow);

   CreateTabDialog();
   UpdateGridColor();
   CheckSoldermask();

   m_physicalConstraintsDlg.EnableRealPartCheckbox(HasRealPart(m_pPCBFile));

	COperationProgress progress;
   // collect data for net access
   getCamCadDoc().GenerateBlockDesignSurfaceInfo(true);

	CAccessAnalysisSolution* accessAnalysisSolution = m_DFTSolution.CreateAccessAnalysisSolution(&(getCamCadDoc()), &m_pPCBFile);

	m_error = accessAnalysisSolution->CreateNetAccess(m_metalAnalysis, &m_ExposedMetalMap, true, netAccessAll, &progress);

   return TRUE; 
}

void CAccessAnalysisDlg::UpdateChange(BOOL updateGridColor)
{
   if (m_DFTSolution.GetTestPlan()->IsModified())
   {
      m_btnOK.EnableWindow(FALSE);
      m_btnProcess.EnableWindow(TRUE);
      m_btnReport.EnableWindow(FALSE);

      if (updateGridColor)
         UpdateGridColor();
   }
}

void CAccessAnalysisDlg::LoadTestPlan()
{
   m_heightAnalysisDlg.LoadTestPlanSettings();
   m_physicalConstraintsDlg.LoadTestPlanSettings();
   m_targetTypesDlg.LoadTestPlanSettings();
   m_optionDlg.LoadTestPlanSettings();
   UpdateChange(TRUE);
   this->m_btnProcess.EnableWindow(TRUE);
}

bool CAccessAnalysisDlg::CheckSoldermask()
{
   bool found = false;
   bool used = false;

   getCamCadDoc().MarkUnusedLayers();
   
   for (int i=0; i < getCamCadDoc().getLayerCount(); i++)
   {
      LayerStruct *layer = getCamCadDoc().getLayerAt(i);
      if (layer == NULL)
         continue;

      if (layer->getLayerType() == layerTypeMaskTop || layer->getLayerType() == layerTypeMaskBottom ||
          layer->getLayerType() == layerTypeMaskAll)
      {
         found = true;
         if (layer->isUsed() == true)
         {
            used = true;
            break;
         }
      }
   }

   if (found)
      m_physicalConstraintsDlg.EnableSoldermaskAnalysisCheckbox(TRUE);
   else
      m_physicalConstraintsDlg.EnableSoldermaskAnalysisCheckbox(FALSE);

   if (!used)
      m_DFTSolution.GetTestPlan()->SetEnableSoldermaskAnalysis(false);

   return found;
}

void CAccessAnalysisDlg::CreateTabDialog()
{
   // Need to set TestPlan pointer before adding the dialog to property sheet
   CTestPlan *testPlan = m_DFTSolution.GetTestPlan();
   m_heightAnalysisDlg.SetTestPlanPointer(testPlan);
   m_physicalConstraintsDlg.SetTestPlanPointer(testPlan);
   m_targetTypesDlg.SetTestPlanPointer(testPlan);
   m_optionDlg.SetTestPlanPointer(testPlan);

   // Add pages to the tab control
   m_propSheet.AddPage(&m_heightAnalysisDlg);
   m_propSheet.AddPage(&m_physicalConstraintsDlg);
   m_propSheet.AddPage(&m_targetTypesDlg);
   m_propSheet.AddPage(&m_optionDlg);

   // Create the property sheet
   m_propSheet.Create(this, WS_CHILD | WS_VISIBLE/* | WS_BORDER*/, 0);
   m_propSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
   m_propSheet.ModifyStyle( 0, WS_TABSTOP );

   // Place property sheet at the location of IDC_STATIC_PROPSHEET
   CRect rcSheet;
   GetDlgItem( IDC_STATIC_PROPSHEET )->GetWindowRect( &rcSheet );
   ScreenToClient( &rcSheet );
   m_propSheet.SetWindowPos( NULL, rcSheet.left, rcSheet.top, rcSheet.Width(), rcSheet.Height(), 
         SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
}


void CAccessAnalysisDlg::UpdateCellResult(int rowIndx, int totColIndx, int totValue, int accColIndx, int accValue)
{
   CUGCell cell;
   m_aaResultsGrid.GetCell(accColIndx, rowIndx, &cell);
   COLORREF backColor = cell.GetBackColor();

   CString totString;
   CString accString;

   totString.Format("%d\n ", totValue);
   if (backColor != m_aaResultsGrid.GetColorGray())
   {
      if(totValue > 0)
         // Case 2227 - Do not round percent, truncate it. E.G. Stop reporting 9999 out of 10000 as 100%.
         // And Mark doesn't care about lower end, e.g. that 0.9% goes to 0%.
         accString.Format("%d\n%d%%", accValue, int((double)accValue/(double)totValue * 100));
      else
         accString = "0\n0%";
   }

   m_aaResultsGrid.QuickSetText(totColIndx, rowIndx, totString);
   m_aaResultsGrid.QuickSetText(accColIndx, rowIndx, accString);

}

void CAccessAnalysisDlg::UpdateGridResult()
{
   CTestPlan *testPlan = m_DFTSolution.GetTestPlan();
   CAccessAnalysisSolution *aaSolution = m_DFTSolution.GetAccessAnalysisSolution();
   CAANetResultCount netResultCount;
   CString tmp;

   // Update Top Only net result
   netResultCount = aaSolution->GetTopNetResultCount();

   UpdateCellResult(ROW_TOP, COL_ALL_TOT, netResultCount.m_iAllNetTot, COL_ALL_ACC, netResultCount.m_iAllNetAcc);
   UpdateCellResult(ROW_TOP, COL_MUL_PIN_TOT, netResultCount.m_iMulPinNetTot, COL_MUL_PIN_ACC, netResultCount.m_iMulPinNetAcc);
   UpdateCellResult(ROW_TOP, COL_SNG_PIN_TOT, netResultCount.m_iSngPinNetTot, COL_SNG_PIN_ACC, netResultCount.m_iSngPinNetAcc);
   UpdateCellResult(ROW_TOP, COL_NC_PIN_TOT, netResultCount.m_iNCPinNetTot, COL_NC_PIN_ACC, netResultCount.m_iNCPinNetAcc);

   // Update Bottom Only net result
   netResultCount = aaSolution->GetBotNetResultCount();

   UpdateCellResult(ROW_BOTTOM, COL_ALL_TOT, netResultCount.m_iAllNetTot, COL_ALL_ACC, netResultCount.m_iAllNetAcc);
   UpdateCellResult(ROW_BOTTOM, COL_MUL_PIN_TOT, netResultCount.m_iMulPinNetTot, COL_MUL_PIN_ACC, netResultCount.m_iMulPinNetAcc);
   UpdateCellResult(ROW_BOTTOM, COL_SNG_PIN_TOT, netResultCount.m_iSngPinNetTot, COL_SNG_PIN_ACC, netResultCount.m_iSngPinNetAcc);
   UpdateCellResult(ROW_BOTTOM, COL_NC_PIN_TOT, netResultCount.m_iNCPinNetTot, COL_NC_PIN_ACC, netResultCount.m_iNCPinNetAcc);

   // Update Both net result
   netResultCount = aaSolution->GetBothNetResultCount();

   UpdateCellResult(ROW_BOTH, COL_ALL_TOT, netResultCount.m_iAllNetTot, COL_ALL_ACC, netResultCount.m_iAllNetAcc);
   UpdateCellResult(ROW_BOTH, COL_MUL_PIN_TOT, netResultCount.m_iMulPinNetTot, COL_MUL_PIN_ACC, netResultCount.m_iMulPinNetAcc);
   UpdateCellResult(ROW_BOTH, COL_SNG_PIN_TOT, netResultCount.m_iSngPinNetTot, COL_SNG_PIN_ACC, netResultCount.m_iSngPinNetAcc);
   UpdateCellResult(ROW_BOTH, COL_NC_PIN_TOT, netResultCount.m_iNCPinNetTot, COL_NC_PIN_ACC, netResultCount.m_iNCPinNetAcc);

   // Update Total net result
   netResultCount = aaSolution->GetTotalNetResultCount();

   UpdateCellResult(ROW_TOTAL, COL_ALL_TOT, netResultCount.m_iAllNetTot, COL_ALL_ACC, netResultCount.m_iAllNetAcc);
   UpdateCellResult(ROW_TOTAL, COL_MUL_PIN_TOT, netResultCount.m_iMulPinNetTot, COL_MUL_PIN_ACC, netResultCount.m_iMulPinNetAcc);
   UpdateCellResult(ROW_TOTAL, COL_SNG_PIN_TOT, netResultCount.m_iSngPinNetTot, COL_SNG_PIN_ACC, netResultCount.m_iSngPinNetAcc);
   UpdateCellResult(ROW_TOTAL, COL_NC_PIN_TOT, netResultCount.m_iNCPinNetTot, COL_NC_PIN_ACC, netResultCount.m_iNCPinNetAcc);

   m_aaResultsGrid.RedrawAll();
}

void CAccessAnalysisDlg::UpdateGridColor()
{

   CTestPlan *testPlan = m_DFTSolution.GetTestPlan();
   if (testPlan == NULL)
      return;

   int surface = testPlan->GetSurface();

   m_aaResultsGrid.SetCellRangeBackColor(COL_ALL_TOT, ROW_TOP, COL_NC_PIN_ACC, ROW_BOTH, colorWhite);


   //////////////////////////////////////////////////////////////////////////
   // All Nets
   //////////////////////////////////////////////////////////////////////////
   if (!testPlan->GetIncludeMulPinNet() && !testPlan->GetIncludeSngPinNet() && !testPlan->GetIncludeNCNet())
   {
      // Turn off everything
      m_aaResultsGrid.SetCellRangeBackColor(COL_ALL_ACC, ROW_HEADING_1, COL_ALL_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorGray());
   }
   else
   {
      // Turn on all surfaces
      m_aaResultsGrid.SetCellRangeBackColor(COL_ALL_TOT, ROW_HEADING_1, COL_ALL_ACC, ROW_BOTH, colorWhite);
      m_aaResultsGrid.SetCellRangeBackColor(COL_ALL_TOT, ROW_TOTAL, COL_ALL_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorOrange());

      if (surface == DFT_SURFACE_TOP)
      {
         // Turn off Bottom
         m_aaResultsGrid.QuickSetBackColor(COL_ALL_ACC, ROW_BOTTOM, m_aaResultsGrid.GetColorGray());
      }
      else if (surface == DFT_SURFACE_BOT)
      {
         // Turn off Top and Both
         m_aaResultsGrid.QuickSetBackColor(COL_ALL_ACC, ROW_TOP, m_aaResultsGrid.GetColorGray());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   // Include Multiple Pin Nets
   //////////////////////////////////////////////////////////////////////////
   if (!testPlan->GetIncludeMulPinNet())
   {
      // Turn off all surface
      m_aaResultsGrid.SetCellRangeBackColor(COL_MUL_PIN_ACC, ROW_HEADING_1, COL_MUL_PIN_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorGray());
   }
   else
   {
      // Turn on all surfaces
      m_aaResultsGrid.SetCellRangeBackColor(COL_MUL_PIN_TOT, ROW_HEADING_1, COL_MUL_PIN_ACC, ROW_BOTH, colorWhite);
      m_aaResultsGrid.SetCellRangeBackColor(COL_MUL_PIN_TOT, ROW_TOTAL, COL_MUL_PIN_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorOrange());

      if (surface == DFT_SURFACE_TOP)
      {
         // Turn off Bottom and Both
         m_aaResultsGrid.QuickSetBackColor(COL_MUL_PIN_ACC, ROW_BOTTOM, m_aaResultsGrid.GetColorGray());
      }
      else if (surface == DFT_SURFACE_BOT)
      {
         // Turn off Top and Both
         m_aaResultsGrid.QuickSetBackColor(COL_MUL_PIN_ACC, ROW_TOP, m_aaResultsGrid.GetColorGray());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   // Include Single Pin Nets
   //////////////////////////////////////////////////////////////////////////
   if (!testPlan->GetIncludeSngPinNet())
   {
      // Turn off all surfaces
      m_aaResultsGrid.SetCellRangeBackColor(COL_SNG_PIN_ACC, ROW_HEADING_1, COL_SNG_PIN_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorGray());
   }
   else
   {
      // Turn on all surfaces
      m_aaResultsGrid.SetCellRangeBackColor(COL_SNG_PIN_TOT, ROW_HEADING_1, COL_SNG_PIN_ACC, ROW_BOTH, colorWhite);
      m_aaResultsGrid.SetCellRangeBackColor(COL_SNG_PIN_TOT, ROW_TOTAL, COL_SNG_PIN_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorOrange());

      if (surface == DFT_SURFACE_TOP)
      {
         // Turn off Bottom and Both
         m_aaResultsGrid.QuickSetBackColor(COL_SNG_PIN_ACC, ROW_BOTTOM, m_aaResultsGrid.GetColorGray());
      }
      else if (surface == DFT_SURFACE_BOT)
      {
         // Turn off Top and Both
         m_aaResultsGrid.QuickSetBackColor(COL_SNG_PIN_ACC, ROW_TOP, m_aaResultsGrid.GetColorGray());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   // Include Non-connected Pin Nets
   //////////////////////////////////////////////////////////////////////////
   if (!testPlan->GetIncludeNCNet())
   {
      // Turn off all surfaces
      m_aaResultsGrid.SetCellRangeBackColor(COL_NC_PIN_ACC, ROW_HEADING_1, COL_NC_PIN_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorGray());
   }
   else
   {
      // Turn on all surfaces
      m_aaResultsGrid.SetCellRangeBackColor(COL_NC_PIN_TOT, ROW_HEADING_1, COL_NC_PIN_ACC, ROW_BOTH, colorWhite);
      m_aaResultsGrid.SetCellRangeBackColor(COL_NC_PIN_TOT, ROW_TOTAL, COL_NC_PIN_ACC, ROW_TOTAL, m_aaResultsGrid.GetColorOrange());

      if (surface == DFT_SURFACE_TOP)
      {
         // Turn off Bottom and Both
         m_aaResultsGrid.QuickSetBackColor(COL_NC_PIN_ACC, ROW_BOTTOM, m_aaResultsGrid.GetColorGray());
      }
      else if (surface == DFT_SURFACE_BOT)
      {
         // Turn off Top and Both
         m_aaResultsGrid.QuickSetBackColor(COL_NC_PIN_ACC, ROW_TOP, m_aaResultsGrid.GetColorGray());
      }
   }

   this->m_aaResultsGrid.RedrawAll();
}
   
void CAccessAnalysisDlg::OnBnClickedProcess()
{
   CWaitCursor wait;

   getCamCadDoc().OnDoneEditing();

   m_metalAnalysis.prepareForAnalysis();
   
   // Create non-connected pin nets
   if (!m_createUnconnectedNet && m_DFTSolution.GetTestPlan()->GetCreateNCNet())
   {
      if (ExplodeNcPins(&(getCamCadDoc()), &m_pPCBFile) > 0)
      {
			// Add only non-connected pin nets to NetAccess
         m_DFTSolution.GetAccessAnalysisSolution()->CreateNetAccess(m_metalAnalysis, &m_ExposedMetalMap, false, netAccessNonConnectedOnly);
      }
      m_createUnconnectedNet = true;
   }

   // Remove the previous feature size and process Access Analysis
   DeleteFeatureSize(&(getCamCadDoc()), &m_pPCBFile);

   CAccessAnalysisSolution *aaSolution = m_DFTSolution.GetAccessAnalysisSolution();
   double tolerance = Units_Factor(UNIT_MILS, getCamCadDoc().getSettings().getPageUnits()) * .001;

   CTestPlan* solutionTestPlan = m_DFTSolution.GetTestPlan();
   CAccessOffsetOptions accessOffsetOptions = solutionTestPlan->getAccessOffsetOptions();

   if (solutionTestPlan->GetTesterType() != testerTypeFixtureless)
   {
      accessOffsetOptions.setEnableOffsetFlag(false);
   }

   aaSolution->ProcessAccessAnalysis(m_metalAnalysis, &m_ExposedMetalMap,accessOffsetOptions, tolerance);

   m_DFTSolution.GetTestPlan()->ResetModified();

   // Create the report
   aaSolution->CreateAccessAnalysisReport( GetReportFilename() );

   // Update feature lost
   m_physicalConstraintsDlg.UpdateFeatureLost(aaSolution->GetTopFeatureLost(), aaSolution->GetBotFeatureLost());
   m_targetTypesDlg.UpdateFeatureLost(aaSolution->GetTopFeatureLost(), aaSolution->GetBotFeatureLost());

   // Update result table
   UpdateGridResult();
   UpdateData(FALSE);

   // Enable the OK button now that Process is done
   m_btnOK.EnableWindow(TRUE);
   m_btnProcess.EnableWindow(FALSE);
   m_btnReport.EnableWindow(TRUE);
}

//*****************************************************************************

void CAccessAnalysisDlg::OnBnClickedDisplayReport()
{
   CString reportFilePath( GetReportFilename() );

   if (! reportFilePath.IsEmpty())
   {
      if (fileExists(reportFilePath))
      {
         ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",reportFilePath,NULL,NULL,SW_SHOW);
      }
   }
}

/******************************************************************************
* GetXYonExposeMetal
   - featureSize is the diameter of the feature
*/
static bool GetXYonExposeMetal(CCEtoODBDoc *doc, FileStruct *pPCBFile, DataStruct *exposeData, double featureSize, double &x, double &y)
{
   bool res = false;
   if (exposeData->getDataType() != dataTypePoly)
      return res;

   // If the extent of the exposed metal is not bigger or equal to feature size then skip
   CExtent exposedExtent = exposeData->getPolyList()->getExtent();
   if ((featureSize - exposedExtent.getXsize()) > SMALLNUMBER && (featureSize - exposedExtent.getYsize() > SMALLNUMBER))
      return res;

   int featureSizeLayer = Graph_Level(DFT_LAYER_FEATURE_SIZE, "", 0L);
   LayerStruct *layer = doc->getLayerAt(featureSizeLayer);
   layer->setVisible(false);

   // If there is a largest exposed metal on the exposeData and is >= the feature size
   // then use the center of the exposeData as the x & y
   double metalDiameter = 0.0;
   Attrib *attrib =  NULL;
   WORD keyword = (WORD)doc->RegisterKeyWord(DFT_ATT_EXPOSE_METAL_DIAMETER, 0, valueTypeUnitDouble);

   if (exposeData->getAttributeMap()->Lookup(keyword, attrib))
      metalDiameter = attrib->getDoubleValue();
   
   if (metalDiameter >= featureSize)
   {
      x = exposeData->getPolyList()->getExtent().getCenter().x;
      y = exposeData->getPolyList()->getExtent().getCenter().y;

      Graph_Block_On(pPCBFile->getBlock());
      DataStruct *featureData= Graph_Circle(featureSizeLayer, x, y, featureSize/2, 0L, 0, FALSE, FALSE);
      Graph_Block_Off();
      return true;
   }

   // No largest exposed metal on exposedData so loop throught the exposeData to find where feature size can fit
   Region *exposeRegion = NULL;
   float scaleFactor = (float)(1000.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES));

   POSITION pos = exposeData->getPolyList()->GetHeadPosition();
   while (pos)
   {
      CPoly *poly = exposeData->getPolyList()->GetNext(pos);
      CExtent polyExtent = poly->getExtent();

      if ((polyExtent.getXsize() - featureSize) > SMALLNUMBER)
      {
         CPoint2d point = polyExtent.getCenter();

         Graph_Block_On(pPCBFile->getBlock());
         DataStruct *featureData= Graph_Circle(featureSizeLayer, point.x, point.y, featureSize/2, 0L, 0, FALSE, FALSE);
         Graph_Block_Off();

         Region *featureRegion = RegionFromPolylist(doc, featureData->getPolyList(), scaleFactor);
         if (!exposeRegion)
            exposeRegion = RegionFromPolylist(doc, exposeData->getPolyList(), scaleFactor);

         Region *intersectRegion = IntersectRegions(exposeRegion, featureRegion);
         if (intersectRegion)
         {
            if (AreRegionsEqual(intersectRegion, featureRegion)) // copper completely covered -> don't need to check anymore soldermask
            {
               x = point.x;
               y = point.y;
               res = true;
            }
         }

         delete featureRegion;

         if (res == true)
            break;
         else
         {
            //pPCBFile->getBlock()->RemoveDataFromList(doc, featureData);
            doc->removeDataFromDataList(*(pPCBFile->getBlock()),featureData);
         }
      }
   }

   if (exposeRegion)
      delete exposeRegion;

   return res;
}

void CAccessAnalysisDlg::OnBnClickedOk()
{
   OnOK();
   getCamCadDoc().OnDoneEditing();

   m_metalAnalysis.graphicData();

   CAccessAnalysisSolution *aaSolution = m_DFTSolution.GetAccessAnalysisSolution();
   aaSolution->PlaceAccessSolution(NULL);

   /*CDFTSolution* curSolution = getCamCadDoc().GetCurrentDFTSolution(m_pPCBFile);
   if (curSolution == NULL)
   {
      curSolution = getCamCadDoc().AddNewDFTSolution(m_pPCBFile, DFT_GENERIC_SOLUTION_NAME, false/*isFlipped, getCamCadDoc().getSettings().getPageUnits());
      getCamCadDoc().SetCurrentDFTSolution(m_pPCBFile,curSolution);
   }
   curSolution->CopyDFTSolution(m_DFTSolution);
*/
   CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(&(getCamCadDoc()));

#ifndef RELEASE
   //curSolution->DumpToFile("C:\\Development\\Application\\~ Project\\CAMCAD 4.4 - Access Analysis\\dump.txt");
#endif
}

void CAccessAnalysisDlg::OnBnClickedCancel()
{
   OnCancel();
   getCamCadDoc().OnDoneEditing();
}

static void DeleteFeatureSize(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   int featureSizeLayer = Graph_Level(DFT_LAYER_FEATURE_SIZE, "", 0L);

   for (POSITION dataPos = pcbFile->getBlock()->getHeadDataPosition(); dataPos != NULL;)
   {
      POSITION curPos = dataPos;
      DataStruct *data = pcbFile->getBlock()->getNextData(dataPos);
      if (data->getDataType() == dataTypePoly && data->getLayerIndex() == featureSizeLayer)
      {
         //pcbFile->getBlock()->RemoveDataFromList(doc, data, curPos);
         doc->removeDataFromDataList(*(pcbFile->getBlock()),data,curPos);
      }
   }
}


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CHeightAnalysisDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CHeightAnalysisDlg, CPropertyPage)
CHeightAnalysisDlg::CHeightAnalysisDlg(CCEtoODBDoc *doc)
   : CPropertyPage(CHeightAnalysisDlg::IDD)
{
   m_pDoc = doc;
   m_pParent = NULL;
   m_pTestPlan = NULL;
   m_pActiveGrid = NULL;
   m_mouseDownPoint.x = 0;
   m_mouseDownPoint.y = 0;
   m_bAlreadyOnInit = false;

   m_decimals = GetDecimals(m_pDoc->getPageUnits());
}

CHeightAnalysisDlg::~CHeightAnalysisDlg()
{
}

void CHeightAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_CHK_TOP_HEIGHT_ANALYSIS, m_chkUseTopHeightAnalysis);
   DDX_Control(pDX, IDC_CHK_BOT_HEIGHT_ANALYSIS, m_chkUseBotHeightAnalysis);
}

BEGIN_MESSAGE_MAP(CHeightAnalysisDlg, CPropertyPage)
   ON_WM_CONTEXTMENU()
   ON_BN_CLICKED(IDC_CHK_TOP_HEIGHT_ANALYSIS, OnBnClickedChkTopHeightAnalysis)
   ON_BN_CLICKED(IDC_CHK_BOT_HEIGHT_ANALYSIS, OnBnClickedChkBotHeightAnalysis)
   ON_MESSAGE(ID_GRIDROWEDITFINISHED, OnRowEditFinished)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHeightAnalysisDlg, CDialog)
END_EVENTSINK_MAP()

BOOL CHeightAnalysisDlg::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_bAlreadyOnInit = true;

   // Create the grids

   m_topHeightGrid.AttachGrid(this, IDC_TopHeightGridStatic);
   CRect rcGridWindow;
   GetDlgItem( IDC_TopHeightGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_topHeightGrid.SizeToFit(rcGridWindow);
   m_topHeightGrid.SetHeightAnalysis(m_pTestPlan->GetTopHeightAnalysis());

   m_botHeightGrid.AttachGrid(this, IDC_BotHeightGridStatic);
   //CRect rcGridWindow;
   GetDlgItem( IDC_BotHeightGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_botHeightGrid.SizeToFit(rcGridWindow);
   m_botHeightGrid.SetHeightAnalysis(m_pTestPlan->GetBotHeightAnalysis());

   LoadTestPlanSettings();

   return TRUE; 
}

/******************************************************************************
*/
LRESULT CHeightAnalysisDlg::OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   // Can't tell which grid without putting in some more hoopla.
   // Just redraw them both.

   m_topHeightGrid.RedrawAll();
   m_botHeightGrid.RedrawAll();

   return 0;
}


void CHeightAnalysisDlg::SetParent(CWnd* pParent)
{
   m_pParent = pParent;
}

void CHeightAnalysisDlg::SetTestPlanPointer(CTestPlan *testPlan)
{
   m_pTestPlan = testPlan;
}

void CHeightAnalysisDlg::LoadTestPlanSettings()
{
   if (m_bAlreadyOnInit == false)
      return;

   if (m_pTestPlan != NULL)
   {
      loadGrid(&m_topHeightGrid, m_pTestPlan->GetTopHeightAnalysis());
      loadGrid(&m_botHeightGrid, m_pTestPlan->GetBotHeightAnalysis());
   }

   m_chkUseTopHeightAnalysis.SetCheck(m_pTestPlan->GetTopHeightAnalysis()->IsUse());
   m_chkUseBotHeightAnalysis.SetCheck(m_pTestPlan->GetBotHeightAnalysis()->IsUse());
   m_chkUseTopHeightAnalysis.EnableWindow(m_pTestPlan->GetSurface() != testSurfaceBottom);
   m_chkUseBotHeightAnalysis.EnableWindow(m_pTestPlan->GetSurface() != testSurfaceTop);

   enableGrid(&m_topHeightGrid, m_pTestPlan->GetTopHeightAnalysis()->IsUse() && m_pTestPlan->GetSurface() != testSurfaceBottom);
   enableGrid(&m_botHeightGrid, m_pTestPlan->GetTopHeightAnalysis()->IsUse() && m_pTestPlan->GetSurface() != testSurfaceTop);

   UpdateData(FALSE);
}

void CHeightAnalysisDlg::enableGrid(CHeightAnalysisGrid *grid, bool enable)
{
   if (grid == NULL)
      return;

   COLORREF backColor = (enable)?(colorWhite):(grid->GetColorGray());
   int rowCnt = grid->GetNumberRows();
   for (int rowIndx = 0; rowIndx < rowCnt; rowIndx++)
   {
      grid->SetRowBackColor(rowIndx, backColor);
   }
   grid->RedrawAll();
}

void CHeightAnalysisDlg::loadGrid(CHeightAnalysisGrid *uggrid, CHeightAnalysis* heightAnalysis)
{
   POSITION pos = heightAnalysis->GetStartPosition();
   while (pos != NULL)
   {
      CHeightRange* heightRange = heightAnalysis->GetNext(pos);
      if (heightRange == NULL)
         continue;

      CString name( heightRange->GetName() );

      CString outlineDist;
      outlineDist.Format("%0.*f", m_decimals, heightRange->GetOutlineDistance());

      CString minHeight;
      minHeight.Format("%0.*f", m_decimals, heightRange->GetMinHeight());

      CString maxHeight;
      if (heightRange->GetMaxHeight() > 0)
         maxHeight.Format("%0.*f", m_decimals, heightRange->GetMaxHeight());
      else
         maxHeight = "Infinity";

      int nextRowIndx = uggrid->GetNumberRows();
      uggrid->SetNumberRows(nextRowIndx+1);
      uggrid->SetRow(nextRowIndx, name, outlineDist, minHeight, maxHeight, heightRange);

   }

   uggrid->RedrawAll();
}

BOOL CHeightAnalysisDlg::OnSetActive()
{
   if (m_pTestPlan)
   {
      m_chkUseTopHeightAnalysis.SetCheck(m_pTestPlan->GetTopHeightAnalysis()->IsUse());
      m_chkUseBotHeightAnalysis.SetCheck(m_pTestPlan->GetBotHeightAnalysis()->IsUse());
      m_chkUseTopHeightAnalysis.EnableWindow(m_pTestPlan->GetSurface() != testSurfaceBottom);
      m_chkUseBotHeightAnalysis.EnableWindow(m_pTestPlan->GetSurface() != testSurfaceTop);

      enableGrid(&m_topHeightGrid, m_pTestPlan->GetTopHeightAnalysis()->IsUse() && m_pTestPlan->GetSurface() != testSurfaceBottom);
      enableGrid(&m_botHeightGrid, m_pTestPlan->GetBotHeightAnalysis()->IsUse() && m_pTestPlan->GetSurface() != testSurfaceTop);
   }

   return CPropertyPage::OnSetActive();
}

void CHeightAnalysisDlg::OnBnClickedChkTopHeightAnalysis()
{
   bool enabled = m_chkUseTopHeightAnalysis.GetCheck() == BST_CHECKED;

   m_pTestPlan->GetTopHeightAnalysis()->SetUse(enabled);

   enableGrid(&m_topHeightGrid, enabled);

   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);
}


void CHeightAnalysisDlg::OnBnClickedChkBotHeightAnalysis()
{
   bool enabled = m_chkUseBotHeightAnalysis.GetCheck() == BST_CHECKED;

   m_pTestPlan->GetBotHeightAnalysis()->SetUse(enabled);
   enableGrid(&m_botHeightGrid, enabled);

   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CPhysicalConstraints dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CPhysicalConstraints, CPropertyPage)
CPhysicalConstraints::CPhysicalConstraints(CCEtoODBDoc *doc)
   : CPropertyPage(CPhysicalConstraints::IDD)
   , m_pDoc(doc)
   , m_chkUseRealPartOutline(TRUE)
   , m_chkUseDFTOutline(TRUE)
   , m_chkUsePkgOutline(TRUE)
   , m_optOutlinePriority(DFT_OUTLINE_REALPART_PRIORITY)
   , m_topBoardNetLost(_T(""))
   , m_topBoardFtrLost(_T(""))
   , m_topCompNetLost(_T(""))
   , m_topCompFtrLost(_T(""))
   , m_topSizeNetLost(_T(""))
   , m_topSizeFtrLost(_T(""))
   , m_botBoardNetLost(_T(""))
   , m_botBoardFtrLost(_T(""))
   , m_botCompNetLost(_T(""))
   , m_botCompFtrLost(_T(""))
   , m_botSizeNetLost(_T(""))
   , m_botSizeFtrLost(_T(""))
   , m_bEnableSoldermaskCheckbox(true)
   , m_bEnableRealPartCheckbox(true)
{
   alreadyOnInit = false;

   m_decimals = GetDecimals(m_pDoc->getPageUnits());
}

CPhysicalConstraints::~CPhysicalConstraints()
{
}

void CPhysicalConstraints::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_CHECK_SOLDERMASK_ANALYSIS, m_chkSoldermaskAnalysis);
   DDX_Check(pDX, IDC_CHECK_USE_DFT_OUTLINE, m_chkUsePkgOutline);
   DDX_Control(pDX, IDC_CHECK_IGNORE_1PIN_COMPOUTLINE, m_chkIgnore1PinCompOutline);
   DDX_Control(pDX, IDC_CHECK_IGNORE_UNLOADED_COMPOUTLINE, m_chkIgnoreUnloadedCompOutline);

   // Top constraints control
   DDX_Control(pDX, IDC_STATIC_TOP_CONSTRAINSTS, m_topConstraints);
   DDX_Control(pDX, IDC_CHECK_TOP_BOARD_DIST, m_chkTopBoardDist);
   DDX_Control(pDX, IDC_CHECK_TOP_COMP_DIST, m_chkTopCompDist);
   DDX_Control(pDX, IDC_CHECK_TOP_FEATURE_SIZE, m_chkTopFeatureSize);
   DDX_Control(pDX, IDC_CHECK_TOP_BEAD_PROBE_SIZE, m_chkTopBeadProbeSize);
   DDX_Control(pDX, IDC_EDIT_TOP_BOARD_DIST, m_txtTopBoardDist);
   DDX_Control(pDX, IDC_EDIT_TOP_COMP_DIST, m_txtTopCompDist);
   DDX_Control(pDX, IDC_EDIT_TOP_FEATURE_SIZE, m_txtTopFeatureSize);
   DDX_Control(pDX, IDC_EDIT_TOP_BEAD_PROBE_SIZE, m_txtTopBeadProbeSize);   
   DDX_Text(pDX, IDC_STATIC_TOP_BRD_NET_LOST, m_topBoardNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_BRD_FTR_LOST, m_topBoardFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_CMP_NET_LOST, m_topCompNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_CMP_FTR_LOST, m_topCompFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_SIZE_NET_LOST, m_topSizeNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_SIZE_FTR_LOST, m_topSizeFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_BEAD_PROBE_SIZE_NET_LOST, m_topBeadProbeSizeNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_BEAD_PROBE_SIZE_FTR_LOST, m_topBeadProbeSizeFtrLost);

   // Bottom constraints control
   DDX_Control(pDX, IDC_STATIC_BOT_CONSTRIANTS, m_botConstraints);
   DDX_Control(pDX, IDC_CHECK_BOT_BOARD_DIST, m_chkBotBoardDist);
   DDX_Control(pDX, IDC_CHECK_BOT_COMP_DIST, m_chkBotCompDist);
   DDX_Control(pDX, IDC_CHECK_BOT_FEATURE_SIZE, m_chkBotFeatureSize);
   DDX_Control(pDX, IDC_CHECK_BOT_BEAD_PROBE_SIZE, m_chkBotBeadProbeSize);
   DDX_Control(pDX, IDC_EDIT_BOT_BOARD_DIST, m_txtBotBoardDist);
   DDX_Control(pDX, IDC_EDIT_BOT_COMP_DIST, m_txtBotCompDist);
   DDX_Control(pDX, IDC_EDIT_BOT_FEATURE_SIZE, m_txtBotFeatureSize);
   DDX_Control(pDX, IDC_EDIT_BOT_BEAD_PROBE_SIZE, m_txtBotBeadProbeSize);
   DDX_Text(pDX, IDC_STATIC_BOT_BRD_NET_LOST, m_botBoardNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_BRD_FTR_LOST, m_botBoardFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_COMP_NET_LOST, m_botCompNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_COMP_FTR_LOST, m_botCompFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_SIZE_NET_LOST, m_botSizeNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_SIZE_FTR_LOST, m_botSizeFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_BEAD_PROBE_SIZE_NET_LOST, m_botBeadProbeSizeNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_BEAD_PROBE_SIZE_FTR_LOST, m_botBeadProbeSizeFtrLost);
}

BEGIN_MESSAGE_MAP(CPhysicalConstraints, CPropertyPage)
   ON_BN_CLICKED(IDC_CHECK_SOLDERMASK_ANALYSIS, OnBnClickedCheckSoldermaskAnalysis)
   ON_BN_CLICKED(IDC_CHECK_USE_DFT_OUTLINE, OnBnClickedCheckUseOutline)

   ON_BN_CLICKED(IDC_CHECK_TOP_BOARD_DIST, OnBnClickedCheckTopBoardDist)
   ON_BN_CLICKED(IDC_CHECK_TOP_COMP_DIST, OnBnClickedCheckTopCompDist)
   ON_BN_CLICKED(IDC_CHECK_TOP_FEATURE_SIZE, OnBnClickedCheckTopFeatureSize)
   ON_BN_CLICKED(IDC_CHECK_TOP_BEAD_PROBE_SIZE, OnBnClickedCheckTopBeadProbeSize)
   ON_BN_CLICKED(IDC_CHECK_BOT_BOARD_DIST, OnBnClickedCheckBotBoardDist)
   ON_BN_CLICKED(IDC_CHECK_BOT_COMP_DIST, OnBnClickedCheckBotCompDist)
   ON_BN_CLICKED(IDC_CHECK_BOT_FEATURE_SIZE, OnBnClickedCheckBotFeatureSize)
   ON_BN_CLICKED(IDC_CHECK_BOT_BEAD_PROBE_SIZE, OnBnClickedCheckBotBeadProbeSize)

   ON_EN_KILLFOCUS(IDC_EDIT_TOP_BOARD_DIST, OnKillFocusEditTopBoardDist)
   ON_EN_KILLFOCUS(IDC_EDIT_TOP_COMP_DIST, OnKillFocusEditTopCompDist)
   ON_EN_KILLFOCUS(IDC_EDIT_TOP_FEATURE_SIZE, OnKillFocusEditTopFeatureSize)
   ON_EN_KILLFOCUS(IDC_EDIT_TOP_BEAD_PROBE_SIZE, OnKillFocusEditTopBeadProbeSize)
   ON_EN_KILLFOCUS(IDC_EDIT_BOT_BOARD_DIST, OnKillFocusEditBotBoardDist)
   ON_EN_KILLFOCUS(IDC_EDIT_BOT_COMP_DIST, OnKillFocusEditBotCompDist)
   ON_EN_KILLFOCUS(IDC_EDIT_BOT_FEATURE_SIZE, OnKillFocusEditBotFeatureSize)
   ON_EN_KILLFOCUS(IDC_EDIT_BOT_BEAD_PROBE_SIZE, OnKillFocusEditBotBeadProbeSize)

   ON_EN_CHANGE(IDC_EDIT_TOP_BOARD_DIST, OnEnChangeEditTopBoardDist)
   ON_EN_CHANGE(IDC_EDIT_TOP_COMP_DIST, OnEnChangeEditTopCompDist)
   ON_EN_CHANGE(IDC_EDIT_TOP_FEATURE_SIZE, OnEnChangeEditTopFeatureSize)
   ON_EN_CHANGE(IDC_EDIT_TOP_BEAD_PROBE_SIZE, OnEnChangeEditTopBeadProbeSize)
   ON_EN_CHANGE(IDC_EDIT_BOT_BOARD_DIST, OnEnChangeEditBotBoardDist)
   ON_EN_CHANGE(IDC_EDIT_BOT_COMP_DIST, OnEnChangeEditBotCompDist)
   ON_EN_CHANGE(IDC_EDIT_BOT_FEATURE_SIZE, OnEnChangeEditBotFeatureSize)
   ON_EN_CHANGE(IDC_EDIT_BOT_BEAD_PROBE_SIZE, OnEnChangeEditBotBeadProbeSize)


   ON_BN_CLICKED(IDC_CHECK_IGNORE_1PIN_COMPOUTLINE, OnBnClickedCheckIgnore1pinCompoutline)
   ON_BN_CLICKED(IDC_CHECK_IGNORE_UNLOADED_COMPOUTLINE, OnBnClickedCheckIgnoreUnloadedCompoutline)
END_MESSAGE_MAP()

BOOL CPhysicalConstraints::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   alreadyOnInit = true;
   m_chkSoldermaskAnalysis.EnableWindow(m_bEnableSoldermaskCheckbox);
   GetDlgItem(IDC_CHECK_USE_REALPART_OUTLINE)->EnableWindow(m_bEnableRealPartCheckbox);

   LoadTestPlanSettings();

   return TRUE;
}

void CPhysicalConstraints::SetParent(CWnd* pParent)
{
   m_pParent = pParent;
}

void CPhysicalConstraints::SetTestPlanPointer(CTestPlan *testPlan)
{
   m_pTestPlan = testPlan;
}

void CPhysicalConstraints::UpdateFeatureLost(CAAFeatureLost topFeatureLost, CAAFeatureLost botFeatureLost)
{
   m_topBoardFtrLost.Format("%d", topFeatureLost.m_iBoardOutline);
   m_topCompFtrLost.Format("%d", topFeatureLost.m_iCompOutline);
   m_topSizeFtrLost.Format("%d", topFeatureLost.m_iFeatureSize);
   m_topBeadProbeSizeFtrLost.Format("%d", topFeatureLost.m_iBeadProbeSize);

   m_botBoardFtrLost.Format("%d", botFeatureLost.m_iBoardOutline);
   m_botCompFtrLost.Format("%d", botFeatureLost.m_iCompOutline);
   m_botSizeFtrLost.Format("%d", botFeatureLost.m_iFeatureSize);
   m_botBeadProbeSizeFtrLost.Format("%d", botFeatureLost.m_iBeadProbeSize);

   if (alreadyOnInit == true)
      UpdateData(FALSE);
}

void CPhysicalConstraints::EnableSoldermaskAnalysisCheckbox(bool enableCheckbox)
{
   m_bEnableSoldermaskCheckbox = enableCheckbox;
}

void CPhysicalConstraints::EnableRealPartCheckbox(bool enableCheckbox)
{
   m_bEnableRealPartCheckbox = enableCheckbox;
}

void CPhysicalConstraints::LoadDefaultSettings()
{
   m_chkSoldermaskAnalysis.SetCheck(TRUE);
   m_chkIgnore1PinCompOutline.SetCheck(TRUE);
   m_chkIgnoreUnloadedCompOutline.SetCheck(TRUE);
   m_chkUseRealPartOutline = TRUE;
   m_chkUseDFTOutline = TRUE;
   m_chkUsePkgOutline = TRUE;
   m_optOutlinePriority = DFT_OUTLINE_REALPART_PRIORITY;

   // Set top constraints
   m_chkTopBoardDist.SetCheck(TRUE);
   m_chkTopCompDist.SetCheck(TRUE);
   m_chkTopFeatureSize.SetCheck(TRUE);
   m_chkTopBeadProbeSize.SetCheck(TRUE);
   m_txtTopBoardDist.SetWindowText("0.100");
   m_txtTopCompDist.SetWindowText("0.010");
   m_txtTopFeatureSize.SetWindowText("0.040");
   m_txtTopBeadProbeSize.SetWindowText("0.040");
   EnableTopConstraints(TRUE);

   // Set bottom constraints
   m_chkBotBoardDist.SetCheck(TRUE);
   m_chkBotCompDist.SetCheck(TRUE);
   m_chkBotFeatureSize.SetCheck(TRUE);
   m_chkBotBeadProbeSize.SetCheck(TRUE);
   m_txtBotBoardDist.SetWindowText("0.100");
   m_txtBotCompDist.SetWindowText("0.010");
   m_txtBotFeatureSize.SetWindowText("0.040");
   m_txtBotBeadProbeSize.SetWindowText("0.040");
   EnableBottomConstraints(TRUE);
}

void CPhysicalConstraints::LoadTestPlanSettings()
{
   if (alreadyOnInit == false)
      return;

   CString tmp = "";
   CPhysicalConst *constraints; 

   int outlineToUse = m_pTestPlan->GetOutlineToUse();
#ifdef SEPARATE_OUTLINE_CONTROLS
   if (outlineToUse == DFT_OUTLINE_NONE)
   {
      m_chkUseRealPartOutline = FALSE;
      m_chkUseDFTOutline = FALSE;
      EnableOutlinePriority(false);
   }
   else if (outlineToUse == DFT_OUTLINE_BOTH)
   {
      m_chkUseRealPartOutline = TRUE;
      m_chkUseDFTOutline = TRUE;
      EnableOutlinePriority(true);
   }
   else if (outlineToUse == DFT_OUTLINE_REALPART)
   {
      m_chkUseRealPartOutline = TRUE;
      m_chkUseDFTOutline = FALSE;
      EnableOutlinePriority(false);
   }
   else if (outlineToUse == DFT_OUTLINE_DFT)
   {
      m_chkUseRealPartOutline = FALSE;
      m_chkUseDFTOutline = TRUE;
      EnableOutlinePriority(false);
   }
   m_optOutlinePriority = m_pTestPlan->GetOutlinePriority();
#else
   // Both go on and off together
   if (outlineToUse == DFT_OUTLINE_NONE)
   {
      m_chkUsePkgOutline = FALSE;
   }
   else
   {
      m_chkUsePkgOutline = TRUE;
   }
   m_chkUseRealPartOutline = m_chkUseDFTOutline = m_chkUsePkgOutline;
   m_optOutlinePriority = DFT_OUTLINE_REALPART_PRIORITY;

   // Sync the testplan
   m_pTestPlan->SetOutlineToUse(m_chkUsePkgOutline ? DFT_OUTLINE_BOTH : DFT_OUTLINE_NONE);
   m_pTestPlan->SetOutlinePriority(DFT_OUTLINE_REALPART_PRIORITY);
#endif

   m_chkSoldermaskAnalysis.SetCheck(m_pTestPlan->GetEnableSoldermaskAnalysis());
   m_chkIgnore1PinCompOutline.SetCheck(m_pTestPlan->GetIgnore1PinCompOutline());
   m_chkIgnoreUnloadedCompOutline.SetCheck(m_pTestPlan->GetIgnoreUnloadedCompOutline());
   m_chkIgnore1PinCompOutline.EnableWindow(m_chkUseRealPartOutline || m_chkUseDFTOutline);
   m_chkIgnoreUnloadedCompOutline.EnableWindow(m_chkUseRealPartOutline || m_chkUseDFTOutline);

   // Top constraint settings
   constraints = m_pTestPlan->GetTopConstraints();
   m_chkTopBoardDist.SetCheck(constraints->GetEnableBoardOutline());
   m_chkTopCompDist.SetCheck(constraints->GetEnableCompOutline());
   m_chkTopFeatureSize.SetCheck(constraints->GetEnableMinFeatureSize());
   m_chkTopBeadProbeSize.SetCheck(constraints->GetEnableBeadProbeSize());

   tmp.Format("%0.*f", m_decimals, constraints->GetValueBoardOutline());
   m_txtTopBoardDist.SetWindowText(tmp);
   tmp.Format("%0.*f", m_decimals, constraints->GetValueCompOutline());
   m_txtTopCompDist.SetWindowText(tmp);
   tmp.Format("%0.*f", m_decimals, constraints->GetValueMinFeatureSize());
   m_txtTopFeatureSize.SetWindowText(tmp);
   tmp.Format("%0.*f", m_decimals, constraints->GetValueBeadProbeSize());
   m_txtTopBeadProbeSize.SetWindowText(tmp);

   EnableTopConstraints(m_pTestPlan->GetSurface() != DFT_SURFACE_BOT);

   // Bottom constraint settings
   constraints = m_pTestPlan->GetBotConstraints();
   m_chkBotBoardDist.SetCheck(constraints->GetEnableBoardOutline());
   m_chkBotCompDist.SetCheck(constraints->GetEnableCompOutline());
   m_chkBotFeatureSize.SetCheck(constraints->GetEnableMinFeatureSize());
   m_chkBotBeadProbeSize.SetCheck(constraints->GetEnableBeadProbeSize());

   tmp.Format("%0.*f", m_decimals, constraints->GetValueBoardOutline());
   m_txtBotBoardDist.SetWindowText(tmp);
   tmp.Format("%0.*f", m_decimals, constraints->GetValueCompOutline());
   m_txtBotCompDist.SetWindowText(tmp);
   tmp.Format("%0.*f", m_decimals, constraints->GetValueMinFeatureSize());
   m_txtBotFeatureSize.SetWindowText(tmp);
   tmp.Format("%0.*f", m_decimals, constraints->GetValueBeadProbeSize());
   m_txtBotBeadProbeSize.SetWindowText(tmp);

   EnableBottomConstraints(m_pTestPlan->GetSurface() != DFT_SURFACE_TOP);

   UpdateData(FALSE);
}

void CPhysicalConstraints::EnableOutlinePriority(bool enable)
{
#ifdef SEPARATE_OUTLINE_CONTROLS
   GetDlgItem(IDC_STATIC_OUTLINE_PRIORITY)->EnableWindow(enable);
   GetDlgItem(IDC_RADIO_USE_REALPART_PRIORITY)->EnableWindow(enable);
   GetDlgItem(IDC_RADIO_USE_DFT_PRIORITY)->EnableWindow(enable);
#else
   // make sure they stay off
   GetDlgItem(IDC_STATIC_OUTLINE_PRIORITY)->EnableWindow(FALSE);
   GetDlgItem(IDC_RADIO_USE_REALPART_PRIORITY)->EnableWindow(FALSE);
   GetDlgItem(IDC_RADIO_USE_DFT_PRIORITY)->EnableWindow(FALSE);
#endif
}

void CPhysicalConstraints::EnableTopConstraints(BOOL enable)
{
   m_topConstraints.EnableWindow(enable);
   m_chkTopBoardDist.EnableWindow (enable);
   m_chkTopCompDist.EnableWindow((enable && (m_chkUseRealPartOutline || m_chkUseDFTOutline)));
   m_chkTopFeatureSize.EnableWindow(enable);
   m_chkTopBeadProbeSize.EnableWindow(enable);

   m_txtTopBoardDist.EnableWindow(m_chkTopBoardDist.GetCheck() && enable);
   m_txtTopCompDist.EnableWindow(m_chkTopCompDist.GetCheck() && enable && (m_chkUseRealPartOutline || m_chkUseDFTOutline));
   m_txtTopFeatureSize.EnableWindow(m_chkTopFeatureSize.GetCheck() && enable);
   m_txtTopBeadProbeSize.EnableWindow(m_chkTopBeadProbeSize.GetCheck() && enable);
}

void CPhysicalConstraints::EnableBottomConstraints(BOOL enable)
{
   m_botConstraints.EnableWindow(enable);
   m_chkBotBoardDist.EnableWindow(enable);
   m_chkBotCompDist.EnableWindow((enable && (m_chkUseRealPartOutline || m_chkUseDFTOutline)));
   m_chkBotFeatureSize.EnableWindow(enable);
   m_chkBotBeadProbeSize.EnableWindow(enable);

   m_txtBotBoardDist.EnableWindow(m_chkBotBoardDist.GetCheck() && enable);
   m_txtBotCompDist.EnableWindow(m_chkBotCompDist.GetCheck() && enable && (m_chkUseRealPartOutline || m_chkUseDFTOutline));
   m_txtBotFeatureSize.EnableWindow(m_chkBotFeatureSize.GetCheck() && enable);
   m_txtBotBeadProbeSize.EnableWindow(m_chkBotBeadProbeSize.GetCheck() && enable);
}

void CPhysicalConstraints::OnBnClickedCheckSoldermaskAnalysis()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetEnableSoldermaskAnalysis(m_chkSoldermaskAnalysis.GetCheck() && m_chkSoldermaskAnalysis.IsWindowEnabled());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckUseOutline()
{
   UpdateData(TRUE);

   // As of dts0100527657 there are no longer separate controls for realpart and dft outlines.
   // We're not totally sure about this, so the code is being modified only a little, so we
   // can easily go back, that is why the old separate m_chkUseXXX vars are still here.
   // We are just funnelling them together so they always carry the same values, i.e. either
   // both are on or both are off, and there the priority is permanent at REALPART outline.
   m_chkUseRealPartOutline = m_chkUseDFTOutline = m_chkUsePkgOutline;

   m_chkIgnore1PinCompOutline.EnableWindow(m_chkUseRealPartOutline || m_chkUseDFTOutline);
   m_chkIgnoreUnloadedCompOutline.EnableWindow(m_chkUseRealPartOutline || m_chkUseDFTOutline);

   EnableTopConstraints(m_pTestPlan->GetSurface() != DFT_SURFACE_BOT);
   EnableBottomConstraints(m_pTestPlan->GetSurface() != DFT_SURFACE_TOP);
   if (m_chkUseRealPartOutline && m_chkUseDFTOutline)
      EnableOutlinePriority(true);
   else
      EnableOutlinePriority(false);

   // Update test plan
   if (!m_pTestPlan)
      return;

   if (m_chkUseRealPartOutline && m_chkUseDFTOutline)
      m_pTestPlan->SetOutlineToUse(DFT_OUTLINE_BOTH);
   else if (m_chkUseRealPartOutline)
      m_pTestPlan->SetOutlineToUse(DFT_OUTLINE_REALPART);
   else if (m_chkUseDFTOutline)
      m_pTestPlan->SetOutlineToUse(DFT_OUTLINE_DFT);
   else
      m_pTestPlan->SetOutlineToUse(DFT_OUTLINE_NONE);

   // No longer user controllable, force realpart priority
   //m_pTestPlan->SetOutlinePriority(m_optOutlinePriority);
   m_pTestPlan->SetOutlinePriority(DFT_OUTLINE_REALPART_PRIORITY);

   m_pTestPlan->SetIgnore1PinCompOutline(m_chkIgnore1PinCompOutline.GetCheck() && m_chkIgnore1PinCompOutline.IsWindowEnabled());
   m_pTestPlan->SetIgnoreUnloadedCompOutline(m_chkIgnoreUnloadedCompOutline.GetCheck() && m_chkIgnoreUnloadedCompOutline.IsWindowEnabled());

   m_pTestPlan->GetTopConstraints()->SetEnableBoardOutline(m_chkTopBoardDist.GetCheck() && m_chkTopBoardDist.IsWindowEnabled());
   m_pTestPlan->GetTopConstraints()->SetEnableCompOutline(m_chkTopCompDist.GetCheck() && m_chkTopCompDist.IsWindowEnabled());
   m_pTestPlan->GetTopConstraints()->SetEnableMinFeatureSize(m_chkTopFeatureSize.GetCheck() && m_chkTopFeatureSize.IsWindowEnabled());
   m_pTestPlan->GetTopConstraints()->SetEnableBeadProbeSize(m_chkTopBeadProbeSize.GetCheck() && m_chkTopBeadProbeSize.IsWindowEnabled());

   m_pTestPlan->GetBotConstraints()->SetEnableBoardOutline(m_chkBotBoardDist.GetCheck() && m_chkBotBoardDist.IsWindowEnabled());
   m_pTestPlan->GetBotConstraints()->SetEnableCompOutline(m_chkBotCompDist.GetCheck() && m_chkBotCompDist.IsWindowEnabled());
   m_pTestPlan->GetBotConstraints()->SetEnableMinFeatureSize(m_chkBotFeatureSize.GetCheck() && m_chkBotFeatureSize.IsWindowEnabled());
   m_pTestPlan->GetBotConstraints()->SetEnableBeadProbeSize(m_chkBotBeadProbeSize.GetCheck() && m_chkBotBeadProbeSize.IsWindowEnabled());

   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedRadioUseRealpart()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetOutlinePriority(m_optOutlinePriority);
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);
}

void CPhysicalConstraints::OnBnClickedCheckIgnore1pinCompoutline()
{
   m_pTestPlan->SetIgnore1PinCompOutline(m_chkIgnore1PinCompOutline.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);
}

void CPhysicalConstraints::OnBnClickedCheckIgnoreUnloadedCompoutline()
{
   m_pTestPlan->SetIgnoreUnloadedCompOutline(m_chkIgnoreUnloadedCompOutline.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);
}

void CPhysicalConstraints::OnBnClickedCheckTopBoardDist()
{
   if (!m_pTestPlan)
      return;

   m_txtTopBoardDist.EnableWindow(m_chkTopBoardDist.GetCheck());
   m_pTestPlan->GetTopConstraints()->SetEnableBoardOutline(m_chkTopBoardDist.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckTopCompDist()
{
   if (!m_pTestPlan)
      return;

   m_txtTopCompDist.EnableWindow(m_chkTopCompDist.GetCheck());
   m_pTestPlan->GetTopConstraints()->SetEnableCompOutline(m_chkTopCompDist.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckTopFeatureSize()
{
   if (!m_pTestPlan)
      return;

   m_txtTopFeatureSize.EnableWindow(m_chkTopFeatureSize.GetCheck());
   m_pTestPlan->GetTopConstraints()->SetEnableMinFeatureSize(m_chkTopFeatureSize.GetCheck());
   
   // Set Top BeadProbe size
   if(!m_chkTopFeatureSize.GetCheck())
      m_chkTopBeadProbeSize.SetCheck(FALSE);

   m_chkTopBeadProbeSize.EnableWindow(m_chkTopFeatureSize.GetCheck());
   m_txtTopBeadProbeSize.EnableWindow(m_chkTopBeadProbeSize.GetCheck());
   m_pTestPlan->GetTopConstraints()->SetEnableBeadProbeSize(m_chkTopBeadProbeSize.GetCheck()); 
   
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckTopBeadProbeSize()
{
   if (!m_pTestPlan)
      return;

   m_txtTopBeadProbeSize.EnableWindow(m_chkTopBeadProbeSize.GetCheck());
   m_pTestPlan->GetTopConstraints()->SetEnableBeadProbeSize(m_chkTopBeadProbeSize.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckBotBoardDist()
{
   if (!m_pTestPlan)
      return;

   m_txtBotBoardDist.EnableWindow(m_chkBotBoardDist.GetCheck());
   m_pTestPlan->GetBotConstraints()->SetEnableBoardOutline(m_chkBotBoardDist.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckBotCompDist()
{
   if (!m_pTestPlan)
      return;

   m_txtBotCompDist.EnableWindow(m_chkBotCompDist.GetCheck());
   m_pTestPlan->GetBotConstraints()->SetEnableCompOutline(m_chkBotCompDist.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckBotFeatureSize()
{
   if (!m_pTestPlan)
      return;

   m_txtBotFeatureSize.EnableWindow(m_chkBotFeatureSize.GetCheck());
   m_pTestPlan->GetBotConstraints()->SetEnableMinFeatureSize(m_chkBotFeatureSize.GetCheck());

   // Set Bottom BeadProbe size
   if(!m_chkBotFeatureSize.GetCheck())
      m_chkBotBeadProbeSize.SetCheck(FALSE);

   m_chkBotBeadProbeSize.EnableWindow(m_chkBotFeatureSize.GetCheck());
   m_txtBotBeadProbeSize.EnableWindow(m_chkBotBeadProbeSize.GetCheck());
   m_pTestPlan->GetBotConstraints()->SetEnableBeadProbeSize(m_chkBotBeadProbeSize.GetCheck()); 

   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnBnClickedCheckBotBeadProbeSize()
{
   if (!m_pTestPlan)
      return;

   m_txtBotBeadProbeSize.EnableWindow(m_chkBotBeadProbeSize.GetCheck());
   m_pTestPlan->GetBotConstraints()->SetEnableBeadProbeSize(m_chkBotBeadProbeSize.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

bool IsValidNumber(CString value)
{
   // Ascii 44 is comma
   // Ascii 46 is period
   // Ascii 48 to 57 is number 0 to 9

   for (int i=0; i<value.GetLength(); i++)
   {
      int ascii = (int)value[i];
      if (ascii != 44 && ascii != 46 && !(ascii >= 48 && ascii <= 57))
         return false;
   }

   return true;
}

void CPhysicalConstraints::OnKillFocusEditTopBoardDist()
{
   CString value = "";
   m_txtTopBoardDist.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Top Constraints:  Board Outline Distance value is invalid", "Error");
      m_txtTopBoardDist.SetFocus();
      return ;
   }
}

void CPhysicalConstraints::OnKillFocusEditTopCompDist()
{
   CString value = "";
   m_txtTopCompDist.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Top Constraints:  Component Outline Distance value is invalid", "Error");
      m_txtTopCompDist.SetFocus();
      return ;
   }
}

void CPhysicalConstraints::OnKillFocusEditTopFeatureSize()
{
   CString value = "";
   m_txtTopFeatureSize.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Top Constraints:  Feature Size value is invalid", "Error");
      m_txtTopFeatureSize.SetFocus();
      return ;
   }
   else
   {
      double minFeatureSize = m_pTestPlan->GetTopConstraints()->GetMinValueFeatureSize(m_pDoc->getSettings().getPageUnits());
      if (atof(value) < minFeatureSize)
      {
         CString errMessage = "";
         errMessage.Format("Top Constraints:  Current Feature Size settting is less than %.*f minimum allowed.", m_decimals, minFeatureSize);
         ErrorMessage(errMessage, "Error");
         m_txtTopFeatureSize.SetFocus();
         return ;
      }
   }
}

void CPhysicalConstraints::OnKillFocusEditTopBeadProbeSize()
{
   CString value = "";
   m_txtTopBeadProbeSize.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Top Constraints:  Bead Probe Size value is invalid", "Error");
      m_txtTopBeadProbeSize.SetFocus();
      return ;
   }
   else
   {
      double minBeadProbeSize = m_pTestPlan->GetTopConstraints()->GetMinValueBeadProbeSize(m_pDoc->getSettings().getPageUnits());
      if (atof(value) < minBeadProbeSize)
      {
         CString errMessage = "";
         errMessage.Format("Top Constraints:  Current Bead Probe Size settting is less than %.*f minimum allowed.", m_decimals, minBeadProbeSize);
         ErrorMessage(errMessage, "Error");
         m_txtTopBeadProbeSize.SetFocus();
         return ;
      }
   }
}

void CPhysicalConstraints::OnKillFocusEditBotBoardDist()
{
   CString value = "";
   m_txtBotBoardDist.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Bottom Constraints:  Board Outline Distance value is invalid", "Error");
      m_txtBotBoardDist.SetFocus();
      return ;
   }
}

void CPhysicalConstraints::OnKillFocusEditBotCompDist()
{
   CString value = "";
   m_txtBotCompDist.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Bottom Constraints:  Component Outline Distance value is invalid", "Error");
      m_txtBotCompDist.SetFocus();
      return ;
   }
}

void CPhysicalConstraints::OnKillFocusEditBotFeatureSize()
{
   CString value = "";
   m_txtBotFeatureSize.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Bottom Constraints:  Feature Size Distance value is invalid", "Error");
      m_txtBotFeatureSize.SetFocus();
      return ;
   }
   else
   {
      double minFeatureSize = m_pTestPlan->GetBotConstraints()->GetMinValueFeatureSize(m_pDoc->getSettings().getPageUnits());
      if (atof(value) < minFeatureSize)
      {
         CString errMessage = "";
         errMessage.Format("Top Constraints:  Current Feature Size settting is less than %.4f minimum allowed.", minFeatureSize);
         ErrorMessage(errMessage, "Error");
         m_txtBotFeatureSize.SetFocus();
         return ;
      }
   }
}

void CPhysicalConstraints::OnKillFocusEditBotBeadProbeSize()
{
   CString value = "";
   m_txtBotBeadProbeSize.GetWindowText(value);

   if (!IsValidNumber(value))
   {
      ErrorMessage("Bottom Constraints:  Bead Probe Size Distance value is invalid", "Error");
      m_txtBotBeadProbeSize.SetFocus();
      return ;
   }
   else
   {
      double minBeadProbeSize = m_pTestPlan->GetBotConstraints()->GetMinValueBeadProbeSize(m_pDoc->getSettings().getPageUnits());
      if (atof(value) < minBeadProbeSize)
      {
         CString errMessage = "";
         errMessage.Format("Bottom Constraints:  Current Bead Probe Size settting is less than %.4f minimum allowed.", minBeadProbeSize);
         ErrorMessage(errMessage, "Error");
         m_txtBotBeadProbeSize.SetFocus();
         return ;
      }
   }
}

void CPhysicalConstraints::OnEnChangeEditTopBoardDist()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtTopBoardDist.GetWindowText(value);
   m_pTestPlan->GetTopConstraints()->SetValueBoardOutline(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnEnChangeEditTopCompDist()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtTopCompDist.GetWindowText(value);
   m_pTestPlan->GetTopConstraints()->SetValueCompOutline(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnEnChangeEditTopFeatureSize()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtTopFeatureSize.GetWindowText(value);
   m_pTestPlan->GetTopConstraints()->SetValueMinFeatureSize(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnEnChangeEditTopBeadProbeSize()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtTopBeadProbeSize.GetWindowText(value);
   m_pTestPlan->GetTopConstraints()->SetValueBeadProbeSize(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnEnChangeEditBotBoardDist()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtBotBoardDist.GetWindowText(value);
   m_pTestPlan->GetBotConstraints()->SetValueBoardOutline(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnEnChangeEditBotCompDist()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtBotCompDist.GetWindowText(value);
   m_pTestPlan->GetBotConstraints()->SetValueCompOutline(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnEnChangeEditBotFeatureSize()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtBotFeatureSize.GetWindowText(value);
   m_pTestPlan->GetBotConstraints()->SetValueMinFeatureSize(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CPhysicalConstraints::OnEnChangeEditBotBeadProbeSize()
{
   if (!m_pTestPlan)
      return;

   CString value = "";
   m_txtBotBeadProbeSize.GetWindowText(value);
   m_pTestPlan->GetBotConstraints()->SetValueBeadProbeSize(atof(value));
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

BOOL CPhysicalConstraints::OnSetActive()
{
   if (m_pTestPlan)
   {
      EnableTopConstraints(m_pTestPlan->GetSurface() != DFT_SURFACE_BOT);
      EnableBottomConstraints(m_pTestPlan->GetSurface() != DFT_SURFACE_TOP);
   }

   return CPropertyPage::OnSetActive();
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CTargetTypeDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CTargetTypeDlg, CPropertyPage)
CTargetTypeDlg::CTargetTypeDlg(CCEtoODBDoc *doc)
   : CPropertyPage(CTargetTypeDlg::IDD)
   , m_topTestAttribNetLost(_T(""))
   , m_topTestAttribFtrLost(_T(""))
   , m_topViaNetLost(_T(""))
   , m_topViaFtrLost(_T(""))
   , m_topConNetLost(_T(""))
   , m_topConFtrLost(_T(""))
   , m_topSMDNetLost(_T(""))
   , m_topSMDFtrLost(_T(""))
   , m_topSinglePinSMDNetLost(_T(""))
   , m_topSinglePinSMDFtrLost(_T(""))
   , m_topTHURNetLost(_T(""))
   , m_topTHURFtrLost(_T(""))
   , m_topPadstackNetLost(_T(""))
   , m_topPadstackFtrLost(_T(""))
   , m_botTestAttribNetLost(_T(""))
   , m_botTestAttribFtrLost(_T(""))
   , m_botViaNetLost(_T(""))
   , m_botViaFtrLost(_T(""))
   , m_botConNetLost(_T(""))
   , m_botConFtrLost(_T(""))
   , m_botSMDNetLost(_T(""))
   , m_botSMDFtrLost(_T(""))
   , m_botSinglePinSMDNetLost(_T(""))
   , m_botSinglePinSMDFtrLost(_T(""))
   , m_botTHURNetLost(_T(""))
   , m_botTHURFtrLost(_T(""))
   , m_botPadstackNetLost(_T(""))
   , m_botPadstackFtrLost(_T(""))
{
   m_pDoc = doc;
   alreadyOnInit = false;
}

CTargetTypeDlg::~CTargetTypeDlg()
{
}

void CTargetTypeDlg::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);

   // Top target type controls
   DDX_Control(pDX, IDC_STATIC_TOP_TARGET, m_staticTopTargetTypes);
   DDX_Control(pDX, IDC_CHECK_TOP_TESTATTRIB, m_chkTopTestAttrib);
   DDX_Control(pDX, IDC_CHECK_TOP_VIA, m_chkTopVia);
   DDX_Control(pDX, IDC_CHECK_TOP_CONNECTOR, m_chkTopConnector);
   DDX_Control(pDX, IDC_CHECK_TOP_SMD, m_chkTopSMD);
   DDX_Control(pDX, IDC_CHECK_TOP_1PINSMD, m_chkTopSinglePinSMD);
   DDX_Control(pDX, IDC_CHECK_TOP_THUR, m_chkTopTHUR);
   DDX_Control(pDX, IDC_CHECK_TOP_PADSTACK, m_chkTopPadstack);
   DDX_Text(pDX, IDC_STATIC_TOP_TESTATTRIB_NET_LOST, m_topTestAttribNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_TESTATTRIB_FTR_LOST, m_topTestAttribFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_VIA_NET_LOST, m_topViaNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_VIA_FTR_LOST, m_topViaFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_CON_NET_LOST, m_topConNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_CON_FTR_LOST, m_topConFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_SMD_NET_LOST, m_topSMDNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_SMD_FTR_LOST, m_topSMDFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_1PINSMD_NET_LOST, m_topSinglePinSMDNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_1PINSMD_FTR_LOST, m_topSinglePinSMDFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_THUR_NET_LOST, m_topTHURNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_THUR_FTR_LOST, m_topTHURFtrLost);
   DDX_Text(pDX, IDC_STATIC_TOP_PADSTACK_NET_LOST, m_topPadstackNetLost);
   DDX_Text(pDX, IDC_STATIC_TOP_PADSTACK_FTR_LOST, m_topPadstackFtrLost);
   DDX_Control(pDX, IDC_LIST_TOP_PADSTACK, m_lstTopPadstack);
   DDX_Control(pDX, IDC_BUTTON_TOP_EDIT, m_btnTopEditPadstack);

   // Bottom target type controls
   DDX_Control(pDX, IDC_STATIC_BOT_TARGET, m_staticBotTargetTypes);
   DDX_Control(pDX, IDC_CHECK_BOT_TESTATTRIB, m_chkBotTestAttrib);
   DDX_Control(pDX, IDC_CHECK_BOT_VIA, m_chkBotVia);
   DDX_Control(pDX, IDC_CHECK_BOT_CONNECTOR, m_chkBotConnector);
   DDX_Control(pDX, IDC_CHECK_BOT_SMD, m_chkBotSMD);
   DDX_Control(pDX, IDC_CHECK_BOT_1PINSMD, m_chkBotSinglePinSMD);
   DDX_Control(pDX, IDC_CHECK_BOT_THUR, m_chkBotTHUR);
   DDX_Control(pDX, IDC_CHECK_BOT_PADSTACK, m_chkBotPadstack);
   DDX_Text(pDX, IDC_STATIC_BOT_TESTATTRIB_NET_LOST, m_botTestAttribNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_TESTATTRIB_FTR_LOST, m_botTestAttribFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_VIA_NET_LOST, m_botViaNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_VIA_FTR_LOST, m_botViaFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_CON_NET_LOST, m_botConNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_CON_FTR_LOST, m_botConFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_SMD_NET_LOST, m_botSMDNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_SMD_FTR_LOST, m_botSMDFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_1PINSMD_NET_LOST, m_botSinglePinSMDNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_1PINSMD_FTR_LOST, m_botSinglePinSMDFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_THUR_NET_LOST, m_botTHURNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_THUR_FTR_LOST, m_botTHURFtrLost);
   DDX_Text(pDX, IDC_STATIC_BOT_PADSTACK_NET_LOST, m_botPadstackNetLost);
   DDX_Text(pDX, IDC_STATIC_BOT_PADSTACK_FTR_LOST, m_botPadstackFtrLost);
   DDX_Control(pDX, IDC_LIST_BOT_PADSTACK, m_lstBotPadstack);
   DDX_Control(pDX, IDC_BUTTON_BOT_EDIT, m_btnBotEditPadstack);
}

BEGIN_MESSAGE_MAP(CTargetTypeDlg, CPropertyPage)
   ON_BN_CLICKED(IDC_CHECK_TOP_TESTATTRIB, OnBnClickedCheckTopTestattrib)
   ON_BN_CLICKED(IDC_CHECK_TOP_VIA, OnBnClickedCheckTopVia)
   ON_BN_CLICKED(IDC_CHECK_TOP_CONNECTOR, OnBnClickedCheckTopConnector)
   ON_BN_CLICKED(IDC_CHECK_TOP_SMD, OnBnClickedCheckTopSmd)
   ON_BN_CLICKED(IDC_CHECK_TOP_1PINSMD, OnBnClickedCheckTopSinglePinSmd)
   ON_BN_CLICKED(IDC_CHECK_TOP_THUR, OnBnClickedCheckTopThur)
   ON_BN_CLICKED(IDC_CHECK_TOP_PADSTACK, OnBnClickedCheckTopPadstack)
   ON_BN_CLICKED(IDC_BUTTON_TOP_EDIT, OnBnClickedButtonTopEdit)
   ON_BN_CLICKED(IDC_CHECK_BOT_TESTATTRIB, OnBnClickedCheckBotTestattrib)
   ON_BN_CLICKED(IDC_CHECK_BOT_VIA, OnBnClickedCheckBotVia)
   ON_BN_CLICKED(IDC_CHECK_BOT_CONNECTOR, OnBnClickedCheckBotConnector)
   ON_BN_CLICKED(IDC_CHECK_BOT_SMD, OnBnClickedCheckBotSmd)
   ON_BN_CLICKED(IDC_CHECK_BOT_1PINSMD, OnBnClickedCheckBotSinglePinSmd)
   ON_BN_CLICKED(IDC_CHECK_BOT_THUR, OnBnClickedCheckBotThur)
   ON_BN_CLICKED(IDC_CHECK_BOT_PADSTACK, OnBnClickedCheckBotPadstack)
   ON_BN_CLICKED(IDC_BUTTON_BOT_EDIT, OnBnClickedButtonBotEdit)
END_MESSAGE_MAP()

BOOL CTargetTypeDlg::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   alreadyOnInit = true;
   LoadTestPlanSettings();

   return TRUE;
}

void CTargetTypeDlg::SetParent(CWnd *pParent)
{
   m_pParent = pParent;
}

void CTargetTypeDlg::SetTestPlanPointer(CTestPlan *testPlan)
{
   m_pTestPlan = testPlan;
}

void CTargetTypeDlg::UpdateFeatureLost(CAAFeatureLost topFeatureLost, CAAFeatureLost botFeatureLost)
{
   m_topTestAttribFtrLost.Format("%d", topFeatureLost.m_iTestAttrib);
   m_topViaFtrLost.Format("%d", topFeatureLost.m_iVia);
   m_topConFtrLost.Format("%d", topFeatureLost.m_iConnector);
   m_topSMDFtrLost.Format("%d", topFeatureLost.m_iMultiPinSMD);
   m_topSinglePinSMDFtrLost.Format("%d", topFeatureLost.m_iSinglePinSMD);
   m_topTHURFtrLost.Format("%d", topFeatureLost.m_iTHRU);
   m_topPadstackFtrLost.Format("%d", topFeatureLost.m_iCADPadstack);

   m_botTestAttribFtrLost.Format("%d", botFeatureLost.m_iTestAttrib);
   m_botViaFtrLost.Format("%d", botFeatureLost.m_iVia);
   m_botConFtrLost.Format("%d", botFeatureLost.m_iConnector);
   m_botSMDFtrLost.Format("%d", botFeatureLost.m_iMultiPinSMD);
   m_botSinglePinSMDFtrLost.Format("%d", botFeatureLost.m_iSinglePinSMD);
   m_botTHURFtrLost.Format("%d", botFeatureLost.m_iTHRU);
   m_botPadstackFtrLost.Format("%d", botFeatureLost.m_iCADPadstack);

   if (alreadyOnInit == true)
      UpdateData(FALSE);
}

void CTargetTypeDlg::LoadDefaultSettings()
{
   m_chkTopTestAttrib.SetCheck(TRUE);                 
   m_chkTopVia.SetCheck(TRUE);                        
   m_chkTopConnector.SetCheck(TRUE);               
   m_chkTopSMD.SetCheck(TRUE); 
   m_chkTopSinglePinSMD.SetCheck(TRUE);
   m_chkTopTHUR.SetCheck(TRUE);                       
   m_chkTopPadstack.SetCheck(TRUE); 
   m_lstTopPadstack.ResetContent();
   EnableTopTargetTypes(TRUE);

   m_chkBotTestAttrib.SetCheck(TRUE);                 
   m_chkBotVia.SetCheck(TRUE);                        
   m_chkBotConnector.SetCheck(TRUE);               
   m_chkBotSMD.SetCheck(TRUE);
   m_chkBotSinglePinSMD.SetCheck(TRUE);
   m_chkBotTHUR.SetCheck(TRUE);                       
   m_chkBotPadstack.SetCheck(TRUE);             
   m_lstBotPadstack.ResetContent();
   EnableBottomTargetTypes(TRUE);
}

void CTargetTypeDlg::LoadTestPlanSettings()
{
   if (alreadyOnInit == false)
      return;

   if (!m_pTestPlan )
      return;

   CTargetType *targetTypes;
   
   // Top Target Types
   targetTypes = m_pTestPlan->GetTopTargetTypes();
   m_chkTopTestAttrib.SetCheck(targetTypes->GetEnableTestAttrib());                 
   m_chkTopVia.SetCheck(targetTypes->GetEnableVia());                      
   m_chkTopConnector.SetCheck(targetTypes->GetEnableConnector());             
   m_chkTopSMD.SetCheck(targetTypes->GetEnableSMD());
   m_chkTopSinglePinSMD.SetCheck(targetTypes->GetEnableSinglePinSMD()); 
   m_chkTopTHUR.SetCheck(targetTypes->GetEnableTHUR());                       
   m_chkTopPadstack.SetCheck(targetTypes->GetEnableCADPadstack());               
   m_lstTopPadstack.ResetContent();

   POSITION pos = targetTypes->GetPadstackHeadPosition();
   while (pos)
   {
      int geomNum = targetTypes->GetPadstackNext(pos);
      BlockStruct *block = m_pDoc->getBlockAt(geomNum);
      if (block)
      {
         int index = m_lstTopPadstack.AddString(block->getName());
         m_lstTopPadstack.SetItemData(index, (DWORD)block);
      }
   }

   EnableTopTargetTypes(m_pTestPlan->GetSurface() != DFT_SURFACE_BOT);

   // Bottom Target Types
   targetTypes = m_pTestPlan->GetBotTargetTypes();
   m_chkBotTestAttrib.SetCheck(targetTypes->GetEnableTestAttrib());                 
   m_chkBotVia.SetCheck(targetTypes->GetEnableVia());                      
   m_chkBotConnector.SetCheck(targetTypes->GetEnableConnector());             
   m_chkBotSMD.SetCheck(targetTypes->GetEnableSMD());
   m_chkBotSinglePinSMD.SetCheck(targetTypes->GetEnableSinglePinSMD()); 
   m_chkBotTHUR.SetCheck(targetTypes->GetEnableTHUR());                       
   m_chkBotPadstack.SetCheck(targetTypes->GetEnableCADPadstack());               
   m_lstBotPadstack.ResetContent();

   pos = targetTypes->GetPadstackHeadPosition();
   while (pos)
   {
      int geomNum = targetTypes->GetPadstackNext(pos);
      BlockStruct *block = m_pDoc->getBlockAt(geomNum);
      if (block)
      {
         int index = m_lstBotPadstack.AddString(block->getName());
         m_lstBotPadstack.SetItemData(index, (DWORD)block);
      }
   }

   EnableBottomTargetTypes(m_pTestPlan->GetSurface() != DFT_SURFACE_TOP);

   UpdateData(FALSE);
}

void CTargetTypeDlg::EnableTopTargetTypes(BOOL enable)
{
   m_staticTopTargetTypes.EnableWindow(enable);
   m_chkTopTestAttrib.EnableWindow(enable);
   m_chkTopVia.EnableWindow(enable);
   m_chkTopConnector.EnableWindow(enable);
   m_chkTopSMD.EnableWindow(enable);
   m_chkTopSinglePinSMD.EnableWindow(enable);
   m_chkTopTHUR.EnableWindow(enable);
   m_chkTopPadstack.EnableWindow(enable);

   m_lstTopPadstack.EnableWindow(m_chkTopPadstack.GetCheck() && enable);
   m_btnTopEditPadstack.EnableWindow(m_chkTopPadstack.GetCheck() && enable);
}

void CTargetTypeDlg::EnableBottomTargetTypes(BOOL enable)
{
   m_staticBotTargetTypes.EnableWindow(enable);
   m_chkBotTestAttrib.EnableWindow(enable);
   m_chkBotVia.EnableWindow(enable);
   m_chkBotConnector.EnableWindow(enable);
   m_chkBotSMD.EnableWindow(enable);
   m_chkBotSinglePinSMD.EnableWindow(enable);
   m_chkBotTHUR.EnableWindow(enable);
   m_chkBotPadstack.EnableWindow(enable);

   m_lstBotPadstack.EnableWindow(m_chkBotPadstack.GetCheck() && enable);
   m_btnBotEditPadstack.EnableWindow(m_chkBotPadstack.GetCheck() && enable);
}

void CTargetTypeDlg::EditPadstackList(CListBox *listbox, CString selectDialogCaption)
{
   CMultiSelectDlg dialog;
   dialog.SetDialogCaption(selectDialogCaption);
   for (int i=0; i<m_pDoc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = m_pDoc->getBlockAt(i);
      if (block == NULL || block->getBlockType() != blockTypePadstack)
         continue;

      BOOL selected = FALSE;
      for (int j=0; j<listbox->GetCount(); j++)
      {
         BlockStruct *padstack = (BlockStruct *)listbox->GetItemData(j);
         if (block == padstack)
         {
            selected = TRUE;
            break;
         }
      }

      CPadstackItem *item = new CPadstackItem(block->getName(), block, selected);
      dialog.AddItem((CSelItem*)item);
   }

   if (dialog.DoModal() != IDOK)
      return;

   listbox->ResetContent();   
   POSITION pos = dialog.GetItemHeadPosition();
   while (pos)
   {
      CPadstackItem *item = (CPadstackItem *)dialog.GetItemNext(pos);
      int index = listbox->AddString(item->GetName());
      listbox->SetItemData(index, (DWORD)item->GetPadstack());
   }
}

void CTargetTypeDlg::OnBnClickedCheckTopTestattrib()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetTopTargetTypes()->SetEnableTestAttrib(m_chkTopTestAttrib.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckTopVia()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetTopTargetTypes()->SetEnableVia(m_chkTopVia.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckTopConnector()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetTopTargetTypes()->SetEnableConnector(m_chkTopConnector.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckTopSmd()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetTopTargetTypes()->SetEnableSMD(m_chkTopSMD.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckTopSinglePinSmd()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetTopTargetTypes()->SetEnableSinglePinSMD(m_chkTopSinglePinSMD.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckTopThur()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetTopTargetTypes()->SetEnableTHUR(m_chkTopTHUR.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckTopPadstack()
{
   m_lstTopPadstack.EnableWindow(m_chkTopPadstack.GetCheck());
   m_btnTopEditPadstack.EnableWindow(m_chkTopPadstack.GetCheck());

   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetTopTargetTypes()->SetEnableCADPadstack(m_chkTopPadstack.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedButtonTopEdit()
{
   EditPadstackList(&m_lstTopPadstack, "Select Top Target Type Padstack");

   CTargetType *targetTypes = m_pTestPlan->GetTopTargetTypes();

   targetTypes->RemovePadstackAll();
   for (int i=0; i<m_lstTopPadstack.GetCount(); i++)
   {     
      BlockStruct *padstack = (BlockStruct*)m_lstTopPadstack.GetItemData(i);
      targetTypes->AddPadstackHead(padstack->getBlockNumber());
   }
}

void CTargetTypeDlg::OnBnClickedCheckBotTestattrib()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetBotTargetTypes()->SetEnableTestAttrib(m_chkBotTestAttrib.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckBotVia()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetBotTargetTypes()->SetEnableVia(m_chkBotVia.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckBotConnector()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetBotTargetTypes()->SetEnableConnector(m_chkBotConnector.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckBotSmd()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetBotTargetTypes()->SetEnableSMD(m_chkBotSMD.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckBotSinglePinSmd()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetBotTargetTypes()->SetEnableSinglePinSMD(m_chkBotSinglePinSMD.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckBotThur()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetBotTargetTypes()->SetEnableTHUR(m_chkBotTHUR.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedCheckBotPadstack()
{
   m_lstBotPadstack.EnableWindow(m_chkBotPadstack.GetCheck());
   m_btnBotEditPadstack.EnableWindow(m_chkBotPadstack.GetCheck());

   if (!m_pTestPlan)
      return;

   m_pTestPlan->GetBotTargetTypes()->SetEnableCADPadstack(m_chkBotPadstack.GetCheck());
   ((CAccessAnalysisDlg *)(m_pParent))->UpdateChange(FALSE);   
}

void CTargetTypeDlg::OnBnClickedButtonBotEdit()
{
   EditPadstackList(&m_lstBotPadstack, "Select Bottom Target Type Padstack");

   CTargetType *targetTypes = m_pTestPlan->GetBotTargetTypes();

   targetTypes->RemovePadstackAll();
   for (int i=0; i<m_lstBotPadstack.GetCount(); i++)
   {     
      BlockStruct *padstack = (BlockStruct*)m_lstBotPadstack.GetItemData(i);
      targetTypes->AddPadstackHead(padstack->getBlockNumber());
   }
}

BOOL CTargetTypeDlg::OnSetActive()
{
   if (m_pTestPlan)
   {
      EnableTopTargetTypes(m_pTestPlan->GetSurface() != DFT_SURFACE_BOT);
      EnableBottomTargetTypes(m_pTestPlan->GetSurface() != DFT_SURFACE_TOP);
   }

   return CPropertyPage::OnSetActive();
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(COptionDlg, CPropertyPage)

COptionDlg::COptionDlg(CAccessAnalysisDlg& parent,CCEtoODBDoc *doc)
: CPropertyPage(COptionDlg::IDD)
, m_parent(parent)
, m_optSurface(DFT_SURFACE_BOT)
, m_optTesterType(DFT_TESTER_FIXTURE)
, m_chkMultiplePinNet(FALSE)
, m_chkSinglPinNet(FALSE)
, m_chkUnconnectedNet(FALSE)
, m_chkRetainExistingProbe(FALSE)
, m_chkMultipleReasonCodes(FALSE)
, m_txtDirectory(_T(""))
, m_chkWriteIgnoreSurface(TRUE)
{
   m_pDoc = doc;
   m_pTestPlan = NULL;
   alreadyOnInit = false;
}

COptionDlg::~COptionDlg()
{
}

CAccessOffsetOptions& COptionDlg::getAccessOffsetOptions()
{
   //CAccessOffsetOptions& accessOffsetOptions = ((m_pTestPlan != NULL) ? m_pTestPlan->getAccessOffsetOptions() : m_shadowAccessOffsetOptions);

   return m_accessOffsetOptions;
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
   CAccessOffsetItem& pinItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin);
   CAccessOffsetItem& viaItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia);
   CAccessOffsetItem&  tpItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp );

   if (alreadyOnInit && !pDX->m_bSaveAndValidate)
   {
      bool accessOffsetEnabledFlag         = m_accessOffsetOptions.getEnableOffsetFlag();
      bool accessOffsetControlsEnabledFlag = accessOffsetEnabledFlag && (m_chkAccessOffset.IsWindowEnabled() != 0);

      m_smdPinCheckBox.EnableWindow(accessOffsetControlsEnabledFlag);
      m_thPinCheckBox.EnableWindow(accessOffsetControlsEnabledFlag);
      m_thPinEditBox.EnableWindow(accessOffsetControlsEnabledFlag && pinItem.getEnableThOffsetFlag());
      m_smdViaCheckBox.EnableWindow(accessOffsetControlsEnabledFlag);
      m_thViaCheckBox.EnableWindow(accessOffsetControlsEnabledFlag);
      m_thViaEditBox.EnableWindow(accessOffsetControlsEnabledFlag && viaItem.getEnableThOffsetFlag());
      m_smdTpCheckBox.EnableWindow(accessOffsetControlsEnabledFlag);
      m_thTpCheckBox.EnableWindow(accessOffsetControlsEnabledFlag);
      m_thTpEditBox.EnableWindow(accessOffsetControlsEnabledFlag && tpItem.getEnableThOffsetFlag());
      m_smdPinComboBox.EnableWindow(accessOffsetControlsEnabledFlag && pinItem.getEnableSmdOffsetFlag());

      CString thPinAccessPriority = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getDirectionList().getEncodedString();
      CString thViaAccessPriority = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).getDirectionList().getEncodedString();
      CString thTpAccessPriority  = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp ).getDirectionList().getEncodedString();

      m_thPinEditBox.SetWindowText(thPinAccessPriority);
      m_thViaEditBox.SetWindowText(thViaAccessPriority);
      m_thTpEditBox.SetWindowText(thTpAccessPriority);

      m_chkAccessOffset.SetCheck(accessOffsetEnabledFlag);

      m_thPinCheckBox.SetCheck(pinItem.getEnableThOffsetFlag());
      m_thViaCheckBox.SetCheck(viaItem.getEnableThOffsetFlag());
      m_thTpCheckBox.SetCheck(tpItem.getEnableThOffsetFlag());

      m_smdPinCheckBox.SetCheck(pinItem.getEnableSmdOffsetFlag());
      m_smdViaCheckBox.SetCheck(viaItem.getEnableSmdOffsetFlag());
      m_smdTpCheckBox.SetCheck(tpItem.getEnableSmdOffsetFlag());

      if (pinItem.getEnableSmdOffsetFlag())
      {
         m_smdPinComboBox.SelectString(0,accessOffsetPlacementTagToSimpleString(pinItem.getSmdOffsetPlacement()));
      }
   }

   CPropertyPage::DoDataExchange(pDX);

   DDX_Radio(pDX, IDC_RADIO_SURFACE_TOP, m_optSurface);
   DDX_Radio(pDX, IDC_RADIO_TESTER_FIXTURE, m_optTesterType);
   DDX_Check(pDX, IDC_CHECK_MULTIPLE_PIN_NET, m_chkMultiplePinNet);
   DDX_Check(pDX, IDC_CHECK_SINGLE_PIN_NET, m_chkSinglPinNet);
   DDX_Check(pDX, IDC_CHECK_UNCONNECTED_NET, m_chkUnconnectedNet);
   DDX_Check(pDX, IDC_CHECK_RETAINEXISTINGPROBE, m_chkRetainExistingProbe);
   DDX_Check(pDX, IDC_CHECK_MULTIPLE_REASON_CODES, m_chkMultipleReasonCodes); 
   DDX_Check(pDX, IDC_CHECK_WRITE_IGNORED_SURFACE, m_chkWriteIgnoreSurface);
   DDX_Control(pDX, IDC_CHECK_ACCESS_OFFSET, m_chkAccessOffset);
   //DDX_Control(pDX, IDC_CHECK_VIA_OFFSET, m_chkViaOffset);
   //DDX_Control(pDX, IDC_CHECK_SMD_OFFSET, m_chkSmdOffset);
   //DDX_Control(pDX, IDC_CHECK_THRU_OFFSET, m_chkThruOffset);
   DDX_Text(pDX, IDC_EDIT_DIRECTORY, m_txtDirectory);
   DDX_Control(pDX, IDC_LIST_TEST_PLANS, m_lstTestPlans);
   DDX_Control(pDX, IDC_CHECK_CREATE_SINGLE_PIN_NET, m_chkCreateUnconnectedNet);
   DDX_Control(pDX, IDC_SmdPinCheckBox, m_smdPinCheckBox);
   DDX_Control(pDX, IDC_ThPinCheckBox, m_thPinCheckBox);
   DDX_Control(pDX, IDC_ThPinEditBox, m_thPinEditBox);
   DDX_Control(pDX, IDC_SmdViaCheckBox, m_smdViaCheckBox);
   DDX_Control(pDX, IDC_ThViaCheckBox, m_thViaCheckBox);
   DDX_Control(pDX, IDC_ThViaEditBox, m_thViaEditBox);
   DDX_Control(pDX, IDC_SmdTpCheckBox, m_smdTpCheckBox);
   DDX_Control(pDX, IDC_ThTpCheckBox, m_thTpCheckBox);
   DDX_Control(pDX, IDC_ThTpEditBox, m_thTpEditBox);
   DDX_Control(pDX, IDC_SmdPinComboBox, m_smdPinComboBox);

   if (pDX->m_bSaveAndValidate)
   {
      CString thMaskString,offsetPlacementString;
      bool accessOffsetEnabledFlag = (m_chkAccessOffset.IsWindowEnabled() && m_chkAccessOffset.GetCheck() != 0);

      m_accessOffsetOptions.setEnableOffsetFlag(accessOffsetEnabledFlag);

      m_thPinEditBox.GetWindowText(thMaskString);
      pinItem.setEnableSmdOffsetFlag(m_smdPinCheckBox.GetCheck() != 0);
      pinItem.setEnableThOffsetFlag(m_thPinCheckBox.GetCheck()   != 0);
      pinItem.setThOffsetDirectionMaskString(thMaskString);
      m_smdPinComboBox.GetWindowText(offsetPlacementString);
      pinItem.setSmdOffsetPlacement(offsetPlacementString);

      m_thViaEditBox.GetWindowText(thMaskString);
      viaItem.setEnableSmdOffsetFlag(m_smdViaCheckBox.GetCheck() != 0);
      viaItem.setEnableThOffsetFlag(m_thViaCheckBox.GetCheck()   != 0);
      viaItem.setThOffsetDirectionMaskString(thMaskString);

      m_thTpEditBox.GetWindowText(thMaskString);
      tpItem.setEnableSmdOffsetFlag(m_smdTpCheckBox.GetCheck()   != 0);
      tpItem.setEnableThOffsetFlag(m_thTpCheckBox.GetCheck()     != 0);
      tpItem.setThOffsetDirectionMaskString(thMaskString);

      if (m_accessOffsetOptions != m_pTestPlan->getAccessOffsetOptions())
      {
         m_pTestPlan->setAccessOffsetOptions(m_accessOffsetOptions);
         m_parent.UpdateChange(TRUE);
      }
   }
}

BEGIN_MESSAGE_MAP(COptionDlg, CPropertyPage)
   ON_BN_CLICKED(IDC_RADIO_SURFACE_TOP, OnBnClickedRadioSurfaceTop)
   ON_BN_CLICKED(IDC_RADIO_SURFACE_BOT, OnBnClickedRadioSurfaceTop)
   ON_BN_CLICKED(IDC_RADIO_SURFACE_BOTH, OnBnClickedRadioSurfaceTop)
   ON_BN_CLICKED(IDC_RADIO_TESTER_FIXTURE, OnBnClickedRadioTesterFixture)
   ON_BN_CLICKED(IDC_RADIO_TESTER_FIXTURELESS, OnBnClickedRadioTesterFixture)
   ON_BN_CLICKED(IDC_CHECK_MULTIPLE_PIN_NET, OnBnClickedCheckMultiplePinNet)
   ON_BN_CLICKED(IDC_CHECK_SINGLE_PIN_NET, OnBnClickedCheckSinglePinNet)
   ON_BN_CLICKED(IDC_CHECK_UNCONNECTED_NET, OnBnClickedCheckUnconnectedNet)
   ON_BN_CLICKED(IDC_CHECK_RETAINEXISTINGPROBE, OnBnClickedCheckRetainexistingProbes)
   ON_BN_CLICKED(IDC_CHECK_MULTIPLE_REASON_CODES, OnBnClickedMultipleReasonCodes)  
   ON_BN_CLICKED(IDC_CHECK_ACCESS_OFFSET, OnBnClickedCheckAccessOffset)
   //ON_BN_CLICKED(IDC_CHECK_VIA_OFFSET, OnBnClickedCheckViaOffset)
   //ON_BN_CLICKED(IDC_CHECK_SMD_OFFSET, OnBnClickedCheckSmdOffset)
   //ON_BN_CLICKED(IDC_CHECK_THRU_OFFSET, OnBnClickedCheckThruOffset)
   ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
   ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnBnClickedButtonSaveas)
   ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
   ON_BN_CLICKED(IDC_BUTTON_SETASDEFAULT, OnBnClickedButtonSetasdefault)
   ON_BN_CLICKED(IDC_BUTTON_RESTOREDEFAULT, OnBnClickedButtonRestoredefault)
   ON_BN_CLICKED(IDC_CHECK_WRITE_IGNORED_SURFACE, OnBnClickedCheckWriteIgnoredSurface)
   ON_WM_ACTIVATE()
   ON_WM_PARENTNOTIFY()
   ON_BN_CLICKED(IDC_CHECK_CREATE_SINGLE_PIN_NET, OnBnClickedCheckCreateUnconnectedNet)
   ON_LBN_SELCHANGE(IDC_LIST_TEST_PLANS, OnLbnSelchangeListTestPlans)
   ON_BN_CLICKED(IDC_ThPinCheckBox, OnBnClickedThPinCheckBox)
   ON_BN_CLICKED(IDC_ThViaCheckBox, OnBnClickedThViaCheckBox)
   ON_BN_CLICKED(IDC_ThTpCheckBox, OnBnClickedThTpCheckBox)
   ON_EN_SETFOCUS(IDC_ThPinEditBox, OnSetFocusThPinEditBox)
   ON_EN_SETFOCUS(IDC_ThViaEditBox, OnSetFocusThViaEditBox)
   ON_EN_SETFOCUS(IDC_ThTpEditBox, OnSetFocusThTpEditBox)
   ON_BN_CLICKED(IDC_SmdPinCheckBox, OnBnClickedSmdPinCheckBox)
   ON_BN_CLICKED(IDC_SmdViaCheckBox, OnBnClickedSmdViaCheckBox)
   ON_BN_CLICKED(IDC_SmdTpCheckBox, OnBnClickedSmdTpCheckBox)
   ON_CBN_SELCHANGE(IDC_SmdPinComboBox, OnSelChangeSmdPinComboBox)
END_MESSAGE_MAP()

BOOL COptionDlg::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   for (EnumIterator(AccessOffsetPlacementTag,placementIterator);placementIterator.hasNext();)
   {
      AccessOffsetPlacementTag placementTag = placementIterator.getNext();
      CString tagValue = accessOffsetPlacementTagToSimpleString(placementTag);
      m_smdPinComboBox.AddString(tagValue);
   }

   alreadyOnInit = true;

   m_txtDirectory = GetDirectoryFromRegistry();
   FillTestPlanListbox(m_txtDirectory);
   LoadTestPlanSettings();

   UpdateData(false);
   UpdateData(true);

   return TRUE;
}

//void COptionDlg::SetParent(CWnd* pParent)
//{
//   m_pParent = pParent;
//}

void COptionDlg::SetTestPlanPointer(CTestPlan *testPlan)
{
   m_pTestPlan = testPlan;
}

void COptionDlg::LoadDefaultSettings()
{
   m_optSurface = DFT_SURFACE_BOT;
   m_optTesterType = DFT_TESTER_FIXTURE;
   m_chkMultiplePinNet = TRUE;
   m_chkSinglPinNet = TRUE;
   m_chkUnconnectedNet = FALSE;
   m_chkRetainExistingProbe = FALSE;
   m_chkMultipleReasonCodes = FALSE;
   m_chkWriteIgnoreSurface = TRUE;

   m_chkCreateUnconnectedNet.SetCheck(FALSE);

   if (m_optTesterType == DFT_TESTER_FIXTURE)
      m_chkAccessOffset.EnableWindow(FALSE);
   else
      m_chkAccessOffset.EnableWindow(TRUE);
   m_chkAccessOffset.SetCheck(TRUE);

   m_accessOffsetOptions.initialize();

   //m_chkViaOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());
   //m_chkSmdOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());
   //m_chkThruOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());

   //m_chkViaOffset.SetCheck(TRUE);
   //m_chkSmdOffset.SetCheck(TRUE);
   //m_chkThruOffset.SetCheck(TRUE);
}

void COptionDlg::LoadTestPlanSettings()
{
   if (alreadyOnInit == false)
      return;

   m_optSurface            = m_pTestPlan->GetSurface();    
   m_optTesterType         = m_pTestPlan->GetTesterType();
   m_chkMultiplePinNet     = m_pTestPlan->GetIncludeMulPinNet(); 
   m_chkSinglPinNet        = m_pTestPlan->GetIncludeSngPinNet(); 
   m_chkUnconnectedNet     = m_pTestPlan->GetIncludeNCNet(); 
   m_chkRetainExistingProbe = m_pTestPlan->GetRetainedExistingProbe();
   m_chkMultipleReasonCodes = m_pTestPlan->GetMultipleReasonCodes();
   m_chkWriteIgnoreSurface = m_pTestPlan->GetAAWriteIgnoreSurface();
   m_accessOffsetOptions   = m_pTestPlan->getAccessOffsetOptions();

   m_chkCreateUnconnectedNet.SetCheck(m_pTestPlan->GetCreateNCNet());

   if (m_optTesterType == DFT_TESTER_FIXTURE)
      m_chkAccessOffset.EnableWindow(FALSE);
   else
      m_chkAccessOffset.EnableWindow(TRUE);

   m_chkAccessOffset.SetCheck(m_accessOffsetOptions.getEnableOffsetFlag());

   //m_chkViaOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());
   //m_chkSmdOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());
   //m_chkThruOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());

   //m_chkViaOffset.SetCheck(m_pTestPlan->GetEnableViaOffset());
   //m_chkSmdOffset.SetCheck(m_pTestPlan->GetEnableSmdOffset());
   //m_chkThruOffset.SetCheck(m_pTestPlan->GetEnableThruOffset());

   UpdateData(false);
}

void COptionDlg::OnBnClickedRadioSurfaceTop()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetSurface(m_optSurface);
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedRadioTesterFixture()
{
   if (!m_pTestPlan)
      return;

   UpdateData(true);

   if (m_optTesterType == testerTypeFixture)
      m_chkAccessOffset.EnableWindow(FALSE);
   else
      m_chkAccessOffset.EnableWindow(TRUE);

   //m_chkViaOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());
   //m_chkSmdOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());
   //m_chkThruOffset.EnableWindow(m_chkAccessOffset.GetCheck() && m_chkAccessOffset.IsWindowEnabled());

   m_pTestPlan->SetTesterType(m_optTesterType);
   m_parent.UpdateChange(FALSE);

   UpdateData(false);
}

void COptionDlg::OnBnClickedCheckMultiplePinNet()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetIncludeMulPinNet(m_chkMultiplePinNet);
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedCheckSinglePinNet()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetIncludeSngPinNet(m_chkSinglPinNet);
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedCheckUnconnectedNet()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetIncludeNCNet(m_chkUnconnectedNet);
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedCheckRetainexistingProbes()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetRetainedExistingProbe(m_chkRetainExistingProbe==TRUE?true:false);
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedMultipleReasonCodes()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetMultipleReasonCodes(m_chkMultipleReasonCodes==TRUE?true:false);
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedCheckCreateUnconnectedNet()
{
   if (!m_pTestPlan)
      return;

   m_pTestPlan->SetCreateNCNet(m_chkCreateUnconnectedNet.GetCheck());
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedCheckAccessOffset()
{
   UpdateData(true);
   UpdateData(false);

   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedButtonBrowse()
{
   CBrowse dirDlg;
   dirDlg.m_strSelDir = m_txtDirectory;
   dirDlg.m_strTitle = "Select directory for Test Plan";

   if (!dirDlg.DoBrowse())
      return;

   m_txtDirectory = dirDlg.m_strPath;
   WriteDirectoryToRegistry(dirDlg.m_strPath);
   FillTestPlanListbox(dirDlg.m_strPath);
   UpdateData(FALSE);
}

void COptionDlg::FillTestPlanListbox(CString directoryPath)
{
   m_lstTestPlans.ResetContent();

   CFileFind finder;
   BOOL bWorking = finder.FindFile(directoryPath + "\\*.pln");
   while (bWorking)
   {
      bWorking = finder.FindNextFile();
      m_lstTestPlans.AddString(finder.GetFileName());
   }

   GetDlgItem(IDC_BUTTON_LOAD)->EnableWindow(FALSE);
}

void COptionDlg::WriteDirectoryToRegistry(CString directoryPath)
{
   // Save directory path into registry

   CAppRegistrySetting("DFT", "Testplan Directory").Set(directoryPath);
}

CString COptionDlg::GetDirectoryFromRegistry()
{
   CString dirPath = CAppRegistrySetting("DFT", "Testplan Directory").Get();

   if (dirPath.IsEmpty())
   {
      dirPath = CBackwardCompatRegistry().DFT_GetTestplanDir();

      // Update from old obsolete key to current key
      if (!dirPath.IsEmpty())
      {
         WriteDirectoryToRegistry(dirPath);
      }
   }

   return dirPath;
}

void COptionDlg::OnLbnSelchangeListTestPlans()
{
   GetDlgItem(IDC_BUTTON_LOAD)->EnableWindow(TRUE);
}

void COptionDlg::OnBnClickedButtonSaveas()
{
   CString filename = "TestPlan.pln";
   if (m_lstTestPlans.GetSelCount() == 0)
   {
      m_lstTestPlans.GetText(m_lstTestPlans.GetSel(0), filename);
   }
   filename.Insert(0, m_txtDirectory + "\\");

   CFileDialog fileDlg(FALSE, "*.pln", filename, OFN_EXPLORER, "TestPlan Files(*.pln)|*.pln|All Files(*.*)|*.*||", this);
   if (fileDlg.DoModal() != IDOK)
      return;

   filename = fileDlg.GetPathName().Trim();
   if (!filename.IsEmpty())
   {
      m_pTestPlan->SaveToFile(filename, m_pDoc->getSettings().getPageUnits());

      CString testPlanDir = filename.Left(filename.ReverseFind('\\'));
      if (!STRCMPI(testPlanDir, m_txtDirectory))
      {
         bool found = false;
         for (int i=0; i<m_lstTestPlans.GetCount(); i++)
         {
            CString tmp = "";
            m_lstTestPlans.GetText(i, tmp);
            if (!tmp.CompareNoCase(fileDlg.GetFileName()))
            {
               found = true;
               break;
            }
         }

         if (found == false)
            m_lstTestPlans.AddString(fileDlg.GetFileName());
      }
   }
}

void COptionDlg::OnBnClickedButtonLoad()
{
   if (m_lstTestPlans.GetSelCount() == 0)
      return;
   if (m_lstTestPlans.GetCurSel() < 0)
      return;

   CString filename = "";
   m_lstTestPlans.GetText(m_lstTestPlans.GetCurSel(), filename);
   filename.Insert(0, m_txtDirectory + "\\");

   m_pTestPlan->LoadFile(filename, m_pDoc->getSettings().getPageUnits());
   m_parent.LoadTestPlan();
   MessageBox("Loading of test plan is completed.", "Load Test Plan");
}

void COptionDlg::OnBnClickedButtonSetasdefault()
{
   m_pTestPlan->SaveToFile(getApp().getUserPath() + "\\default.pln", m_pDoc->getSettings().getPageUnits());
   MessageBox("Current setting is set as default.", "Set as default");
}

void COptionDlg::OnBnClickedButtonRestoredefault()
{
   // Reset to default
   m_pTestPlan->SetDefaults(m_pDoc->getSettings().getPageUnits());
   m_pTestPlan->LoadFile(getApp().getUserPath() + "\\default.pln", m_pDoc->getSettings().getPageUnits());
   m_parent.LoadTestPlan();
   MessageBox("Restore of test plan is completed.", "Restore Default Test Plan");
}

void COptionDlg::OnBnClickedCheckWriteIgnoredSurface()
{
   if (!m_pTestPlan)
      return;

   UpdateData(TRUE);
   m_pTestPlan->SetAAWriteIgnoreSurface(m_chkWriteIgnoreSurface);
   m_parent.UpdateChange(TRUE);
}

void COptionDlg::OnBnClickedThPinCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void COptionDlg::OnBnClickedThViaCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void COptionDlg::OnBnClickedThTpCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void COptionDlg::OnBnClickedSmdPinCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void COptionDlg::OnBnClickedSmdViaCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void COptionDlg::OnBnClickedSmdTpCheckBox()
{
   UpdateData(true);
   UpdateData(false);
}

void COptionDlg::OnSetFocusThPinEditBox()
{
   UpdateData(true);

   m_thPinCheckBox.SetFocus();

   CAccessOffsetItem& accessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin);

   // debug
   CString directionPriority = accessOffsetItem.getDirectionList().getEncodedString();

   CAccessOffsetDirectionPriorityDialog directionPriorityDialog(accessOffsetItem.getDirectionList());

   if (directionPriorityDialog.DoModal() == IDOK)
   {
      accessOffsetItem.getDirectionList() = directionPriorityDialog.getAllowedList();
      UpdateData(false);
      UpdateData(true);
   }
   else
   {
      UpdateData(false);
   }
}

void COptionDlg::OnSetFocusThViaEditBox()
{
   UpdateData(true);

   m_thViaCheckBox.SetFocus();

   CAccessOffsetItem& accessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia);
   CAccessOffsetDirectionPriorityDialog directionPriorityDialog(accessOffsetItem.getDirectionList());

   if (directionPriorityDialog.DoModal() == IDOK)
   {
      accessOffsetItem.getDirectionList() = directionPriorityDialog.getAllowedList();
      UpdateData(false);
      UpdateData(true);
   }
   else
   {
      UpdateData(false);
   }
}

void COptionDlg::OnSetFocusThTpEditBox()
{
   UpdateData(true);

   m_thTpCheckBox.SetFocus();

   CAccessOffsetItem& accessOffsetItem = m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp);
   CAccessOffsetDirectionPriorityDialog directionPriorityDialog(accessOffsetItem.getDirectionList());

   if (directionPriorityDialog.DoModal() == IDOK)
   {
      accessOffsetItem.getDirectionList() = directionPriorityDialog.getAllowedList();
      UpdateData(false);
      UpdateData(true);
   }
   else
   {
      UpdateData(false);
   }
}

void COptionDlg::OnSelChangeSmdPinComboBox()
{
   UpdateData(true);
   UpdateData(false);
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CPadstackItem Class
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
CPadstackItem::CPadstackItem(CString name, BlockStruct *padstack, BOOL selected):CSelItem(name, selected)
{
   m_pPadstack = padstack;
}

BlockStruct *CPadstackItem::GetPadstack()
{
   return m_pPadstack;
}

void CPadstackItem::SetName(CString name)
{
   m_name = name;
}

void CPadstackItem::SetPadstack(BlockStruct *pPadstack)
{
   m_pPadstack = pPadstack;
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CAccessOffsetDirectionPriorityDialog, CDialog)

CAccessOffsetDirectionPriorityDialog::CAccessOffsetDirectionPriorityDialog(const CPrioritizedAccessOffsetDirectionsList& allowedList,CWnd* pParent /*=NULL*/)
: CDialog(CAccessOffsetDirectionPriorityDialog::IDD, pParent)
, m_allowedList(allowedList)
{
}

CAccessOffsetDirectionPriorityDialog::~CAccessOffsetDirectionPriorityDialog()
{
}

void CAccessOffsetDirectionPriorityDialog::DoDataExchange(CDataExchange* pDX)
{
   if (! pDX->m_bSaveAndValidate)
   {
      int allowedCount         = m_allowedListBox.GetCount();
      int allowedSelCount      = m_allowedListBox.GetSelCount();
      int disallowedSelCount   = m_disallowedListBox.GetSelCount();
      int allowedFirstSelected = getFirstSelected(m_allowedListBox);

      m_moveUpButton.EnableWindow(allowedSelCount == 1 && allowedFirstSelected > 0);
      m_moveDownButton.EnableWindow(allowedSelCount == 1 && allowedFirstSelected < allowedCount - 1);
      m_disallowButton.EnableWindow(allowedSelCount > 0);
      m_allowButton.EnableWindow(disallowedSelCount > 0);

      // Make sure window exists before trying to set its text.
      // It does not yet exist in first pass through here, during dialog init, and
      // trying to set text will cause invalid parameter error (exception) from MFC.
      if (m_directionPriorityEditBox.GetWindow(0))
      {
         CString directionPriority = m_allowedList.getEncodedString();
         m_directionPriorityEditBox.SetWindowText(directionPriority);
      }
   }

   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_MoveUp, m_moveUpButton);
   DDX_Control(pDX, IDC_MoveDown, m_moveDownButton);
   DDX_Control(pDX, IDC_Allow, m_allowButton);
   DDX_Control(pDX, IDC_Disallow, m_disallowButton);
   DDX_Control(pDX, IDC_AllowedList, m_allowedListBox);
   DDX_Control(pDX, IDC_DisallowedList, m_disallowedListBox);
   DDX_Control(pDX, IDC_DirectionPriority, m_directionPriorityEditBox);

   if (pDX->m_bSaveAndValidate)
   {
   }
}

void CAccessOffsetDirectionPriorityDialog::fillDirectionList(CListBox& listBox,const CPrioritizedAccessOffsetDirectionsList& directionList)
{
   listBox.ResetContent();

   for (POSITION pos = directionList.GetHeadPosition();pos != NULL;)
   {
      AccessOffsetDirectionTag accessOffsetDirection = directionList.GetNext(pos);

      listBox.AddString(getDisplayString(accessOffsetDirection));
   }
}

int CAccessOffsetDirectionPriorityDialog::getFirstSelected(CListBox& listBox)
{
   int firstSelectedIndex = -1;

   CArray<int,int> selectedArray;
   selectedArray.SetSize(listBox.GetCount());

   int selectedCount = listBox.GetSelItems(selectedArray.GetSize(),selectedArray.GetData());

   if (selectedCount > 0)
   {
      firstSelectedIndex = selectedArray.GetAt(0);
   }

   return firstSelectedIndex;
}

void CAccessOffsetDirectionPriorityDialog::getSelectedFromList(CPrioritizedAccessOffsetDirectionsList& directionList,CListBox& listBox)
{
   directionList.empty();

   for (int index = 0;index < listBox.GetCount();index++)
   {
      bool selectedFlag = (listBox.GetSel(index) > 0);

      if (selectedFlag)
      {
         CString value;
         listBox.GetText(index,value);
         AccessOffsetDirectionTag accessOffsetDirection = stringToAccessOffsetDirectionsTag(value);

         directionList.add(accessOffsetDirection);
      }
   }
}

CString CAccessOffsetDirectionPriorityDialog::getDisplayString(AccessOffsetDirectionTag accessOffsetDirection)
{
   CString displayString;

   if (accessOffsetDirection == accessOffsetDirectionUndefined)
   {
      displayString = "Any";
   }
   else
   {
      displayString = accessOffsetDirectionsTagToString(accessOffsetDirection);
   }

   return displayString;
}

BEGIN_MESSAGE_MAP(CAccessOffsetDirectionPriorityDialog, CDialog)
   ON_EN_KILLFOCUS(IDC_DirectionPriority, OnKillFocusDirectionPriority)
   ON_LBN_SELCHANGE(IDC_AllowedList, OnSelChangeAllowedList)
   ON_LBN_SELCHANGE(IDC_DisallowedList, OnSelChangeDisallowedList)
   ON_BN_CLICKED(IDC_MoveUp, OnBnClickedMoveUp)
   ON_BN_CLICKED(IDC_MoveDown, OnBnClickedMoveDown)
   ON_BN_CLICKED(IDC_Allow, OnBnClickedAllow)
   ON_BN_CLICKED(IDC_Disallow, OnBnClickedDisallow)
END_MESSAGE_MAP()

// CAccessOffsetDirectionPriorityDialog message handlers

BOOL CAccessOffsetDirectionPriorityDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   // TODO:  Add extra initialization here
   m_disallowedList.setToComplement(m_allowedList);

   fillDirectionList(m_allowedListBox   ,m_allowedList   );
   fillDirectionList(m_disallowedListBox,m_disallowedList);

   UpdateData(false);

   m_directionPriorityEditBox.SetFocus();

   return FALSE;
}

void CAccessOffsetDirectionPriorityDialog::OnKillFocusDirectionPriority()
{
   UpdateData(true);

   CString directionPriority;
   m_directionPriorityEditBox.GetWindowText(directionPriority);

   m_allowedList.setFromEncodedString(directionPriority);
   m_disallowedList.setToComplement(m_allowedList);

   fillDirectionList(m_allowedListBox   ,m_allowedList   );
   fillDirectionList(m_disallowedListBox,m_disallowedList);

   UpdateData(false);
}

void CAccessOffsetDirectionPriorityDialog::OnSelChangeAllowedList()
{
   UpdateData(true);
   UpdateData(false);
}

void CAccessOffsetDirectionPriorityDialog::OnSelChangeDisallowedList()
{
   UpdateData(true);
   UpdateData(false);
}

void CAccessOffsetDirectionPriorityDialog::OnBnClickedMoveUp()
{
   int index = getFirstSelected(m_allowedListBox);

   if (index >= 0)
   {
      m_allowedList.moveTowardsHead(index);

      fillDirectionList(m_allowedListBox,m_allowedList);

      m_allowedListBox.SetSel(index - 1,true);
      m_allowedListBox.Invalidate();
   }

   UpdateData(false);
}

void CAccessOffsetDirectionPriorityDialog::OnBnClickedMoveDown()
{
   int index = getFirstSelected(m_allowedListBox);

   if (index >= 0)
   {
      m_allowedList.moveTowardsTail(index);

      fillDirectionList(m_allowedListBox,m_allowedList);

      m_allowedListBox.SetSel(index + 1,true);
      m_allowedListBox.Invalidate();
   }

   UpdateData(false);
}

void CAccessOffsetDirectionPriorityDialog::OnBnClickedAllow()
{
   CPrioritizedAccessOffsetDirectionsList selected;
   getSelectedFromList(selected,m_disallowedListBox);

   m_allowedList.add(selected);
   m_disallowedList.setToComplement(m_allowedList);

   fillDirectionList(m_allowedListBox   ,m_allowedList   );
   fillDirectionList(m_disallowedListBox,m_disallowedList);

   UpdateData(false);
}

void CAccessOffsetDirectionPriorityDialog::OnBnClickedDisallow()
{
   CPrioritizedAccessOffsetDirectionsList selected;
   getSelectedFromList(selected,m_allowedListBox);

   m_allowedList.remove(selected);
   m_disallowedList.setToComplement(m_allowedList);

   fillDirectionList(m_allowedListBox   ,m_allowedList   );
   fillDirectionList(m_disallowedListBox,m_disallowedList);

   UpdateData(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CAAResultsGrid::CAAResultsGrid()
{
   SetSortEnabled(false);  // Turn off grid sort on heading left click

   m_colorBlue = COLOR_BLUE;
   m_colorOrange = COLOR_ORANGE;
   m_colorGray = colorLtGray;
}

//----------------------------------------------------------------------------------

void CAAResultsGrid::SizeToFit(CRect &rect)
{
   int width = rect.Width();

   // 8 grid cols plus left side row heading col

   int virtualNumCols = 10;  

   int fieldWidth = width / virtualNumCols;

   int colCnt = GetNumberCols();

   // Double wide for row heading column.
   SetColWidth(-1, fieldWidth * 2);

   // Single wide for data columns.
   for (int colIndx = 0; colIndx < colCnt; colIndx++)
   {
      SetColWidth(colIndx, fieldWidth);
   }

   RedrawAll();
}

//----------------------------------------------------------------------------------

void CAAResultsGrid::OnSetup()
{
#ifdef CELL_EDIT
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;
#endif

   SetNumberCols(8, false);
   SetNumberRows(4, false);

   int singleRowThHeight = GetTH_Height();
   int topRowHeight = (int)(singleRowThHeight * 1.5);
   int secondRowHeight = singleRowThHeight;
   int overallHeadingHeight = topRowHeight + secondRowHeight;
   SetTH_NumberRows(2);
   SetTH_RowHeight(-1, secondRowHeight);
   SetTH_RowHeight(-2, topRowHeight);
   SetTH_Height(overallHeadingHeight);
   //SetTH_RowHeight(-3, singleRowThHeight);

   for (int col = 0; col < 8; col++)
      for (int row = 0; row < 4; row++)
         QuickSetCellTypeEx(col, row, UGCT_NORMALMULTILINE);

   int dataRowHeight = (int)(singleRowThHeight * 1.5);
   for (int row = 0; row < 4; row++)
         SetRowHeight(row, dataRowHeight);

   EnableJoins(TRUE);
   CUGCell cell;


   // Grouped column headings
   SetMultiColHeading(0, -2, 1, -2, "All\nNets");
   SetMultiColHeading(2, -2, 3, -2, "Multiple Pin Nets");
   SetMultiColHeading(4, -2, 5, -2, "Single Pin Nets");
   SetMultiColHeading(6, -2, 7, -2, "NC Pin Nets");
   
   QuickSetBackColor(0, -2, m_colorBlue);
   QuickSetBackColor(2, -2, m_colorBlue);
   QuickSetBackColor(4, -2, m_colorBlue);
   QuickSetBackColor(6, -2, m_colorBlue);

   // Individual columns headings
   QuickSetText(0, -1, "Tot");
   QuickSetText(1, -1, "Acc");
   QuickSetText(2, -1, "Tot");
   QuickSetText(3, -1, "Acc");
   QuickSetText(4, -1, "Tot");
   QuickSetText(5, -1, "Acc");
   QuickSetText(6, -1, "Tot");
   QuickSetText(7, -1, "Acc");

   // Row headings
   QuickSetCellTypeEx(-1, -1, UGCT_NORMALMULTILINE);
   QuickSetText(-1, -1, "\nNet\nSurface");
   QuickSetText(-1,  0, "Top");
   QuickSetText(-1,  1, "Bottom");
   QuickSetText(-1,  2, "Both");
   QuickSetText(-1,  3, "Total");

   QuickSetBackColor(-1, -1, m_colorBlue);
   QuickSetBackColor(-1,  0, m_colorBlue);
   QuickSetBackColor(-1,  1, m_colorBlue);
   QuickSetBackColor(-1,  2, m_colorBlue);

   QuickSetBackColor(-1,  3, m_colorOrange);
   for (int col = 0; col < 7;  col++)
      QuickSetBackColor(col,  3, m_colorOrange);

   for (int row = -1; row < 4; row++)
      QuickSetBackColor(7,  row, m_colorGray);


   //BestFit(-1, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);

   //BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   //SetColWidth(-1, 0); // get rid of "row heading"
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CHeightAnalysisGrid::CHeightAnalysisGrid()
{
   SetSortEnabled(false);  // Turn off grid sort on heading left click

   m_colorBlue = COLOR_BLUE;
   m_colorOrange = COLOR_ORANGE;
   m_colorGray = colorLtGray;
}

//----------------------------------------------------------------------------------

void CHeightAnalysisGrid::SizeToFit(CRect &rect)
{
   int width = rect.Width();

   //int fieldWidth = width / virtualNumCols;
   int width0 = (int)(width * 0.28);
   int width1 = (int)(width * 0.32);
   int width2 = (int)(width * 0.20);
   int width3 = (int)(width * 0.20);

   SetColWidth(0, width0);
   SetColWidth(1, width1);
   SetColWidth(2, width2);
   SetColWidth(3, width3);

   RedrawAll();
}

//----------------------------------------------------------------------------------

void CHeightAnalysisGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;

   int numCols = 4;

   SetNumberCols(numCols, false);
   SetNumberRows(0, false);

   int singleRowThHeight = GetTH_Height();
   int topRowHeight = (int)(singleRowThHeight * 1.5);
   int overallHeadingHeight = topRowHeight;
   SetTH_NumberRows(1);
   SetTH_RowHeight(-1, topRowHeight);
   SetTH_Height(overallHeadingHeight);

   for (int col = 0; col < numCols; col++)
       QuickSetCellTypeEx(col, -1, UGCT_NORMALMULTILINE);

   // Get rid of row heading column
   SetColWidth(-1, 0);

   //int dataRowHeight = singleRowThHeight; //(int)(singleRowThHeight * 1.5);
   //for (int row = 0; row < 4; row++)
   //      SetRowHeight(row, dataRowHeight);

   // Individual columns headings
   QuickSetText(0, -1, "Name");
   QuickSetText(1, -1, "Package Outline\nDistance");
   QuickSetText(2, -1, "Min\nHeight");
   QuickSetText(3, -1, "Max\nHeight");


   CUGCell cell;

   GetCell(0, -1, &cell);
   cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
   SetCell(0, -1, &cell);

   GetColDefault(0 ,&cell); 
   cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
   SetColDefault(0,&cell);

   GetColDefault(1 ,&cell); 
   cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
   SetColDefault(1,&cell);

   GetColDefault(2 ,&cell); 
   cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
   SetColDefault(2,&cell);

   GetColDefault(3 ,&cell); 
   cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
   SetColDefault(3,&cell);



   EnableMenu(TRUE); // Enable right click menu


   //BestFit(-1, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);

   //BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   //SetColWidth(-1, 0); // get rid of "row heading"
}



/////////////////////////////////////////////////////////////////////////////
//	OnEditStart
//		This message is sent whenever the grid is ready to start editing a cell
//	Params:
//		col, row - location of the cell that edit was requested over
//		edit -	pointer to a pointer to the edit control, allows for swap of edit control
//				if edit control is swapped permanently (for the whole grid) is it better
//				to use 'SetNewEditClass' function.
//	Return:
//		TRUE - to allow the edit to start
//		FALSE - to prevent the edit from starting

/// SURPRISE - Return FALSE for cells with Option Menus, else it will go into text edit mode !

int CHeightAnalysisGrid::OnEditStart(int col, long row, CWnd **edit)
{
   *edit = &m_myCUGEdit;

   // Only Name and Outline Distance can be edited.

   // Old test from vsflex days
   // Seems to say: Can't edit MAX col, row=0=heading in vsflex so can't edit heading,
   // and Col==min and row==1 was min cell in first row, can't edit that.
   //if (mouseCol == COL_HEIGHT_MAX || mouseRow == 0 || (mouseCol == COL_HEIGHT_MIN && mouseRow == 1))
   //   return;

   // We'll do the test on what is allowed  to be edited, as opposed to old vsflex style
   // that quit on what is not allowed to be edited.

   // Skip out if disabled, go by background color
   CUGCell cell;
   GetCell(col, row, &cell);
   COLORREF currentBackColor = cell.GetBackColor();
   if (currentBackColor == this->GetColorGray())
      return FALSE;

   bool editNameOrDistance = (col == 0 || col == 1);  // Always allowed
   bool editMinHeightNotFirstRow = (col == 2 && row > 0); // Allowed on all except first row

   if (
      editNameOrDistance || editMinHeightNotFirstRow
      )
   {
      return true;
   }


	return false;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditVerify
//		This notification is sent every time the user hits a key while in edit mode.
//		It is mostly used to create custom behavior of the edit control, because it is
//		so easy to allow or disallow keys hit.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int CHeightAnalysisGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	//UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	//UNREFERENCED_PARAMETER(*vcKey);

   if (col == 0)
   {
      return TRUE;
   }
   else if (col == 1)
   {
      return OnEditVerifyFloatCell(col, row, edit, vcKey);
   }

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finished
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being canceled
//	Return:
//		TRUE - to allow the edit to proceed
//		FALSE - to force the user back to editing of that same cell
int CHeightAnalysisGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   //BOOL pm = this->m_packagesPage->PostMessage(ID_GRIDROWEDITFINISHED, row, col);
   this->GetParent()->PostMessage(ID_GRIDROWEDITFINISHED, row, col);

   if (cancelFlag)
      return TRUE;

   if (row >= 0 && row < GetNumberRows())
   {
      if (col == 0) // Name
      {
         // The heightRange object associated with this row
         CHeightRange* thisRowHeightRange = (CHeightRange*)GetRowData(row);

         // Make sure name is unique
         CHeightRange* existingHeightRange = m_heightAnalysis->FindHeightRange(string);
         if (existingHeightRange != NULL)
         {
            // It exists, but if it is the same as one in this row already then it really is not a change or duplicate.
            
            if (existingHeightRange == thisRowHeightRange) // is same one and only
               return true;
            else
            {
               MessageBox("Name already exists.  Please enter a unique name.", "Name Already Exists");
               return false;
            }
         }         

         // Update the heightRange with the new name
         if (thisRowHeightRange != NULL)
            thisRowHeightRange->SetName(string);
         return true;
      }
      else if (col == 1) // distance
      {
         // Update the heightRange with the new package outline distance
         CHeightRange* heightRange = (CHeightRange*)GetRowData(row);
         if (heightRange != NULL)
            heightRange->SetOutlineDistance(atof(string));
         return true;
      }
      else if (col == 2)
      {
         CString minHeightStr = string;
         if (!is_number(minHeightStr))
         {
            MessageBox("Must enter a numeric value.", "Invalid Min Height");
            return false;
         }

         int rowCnt = GetNumberRows();
         double minHeight = atof(minHeightStr);
         if (row == (rowCnt-1))
         {
            // Last row edited, only need to check against previous row.
            CString prevRowMinHeightStr( QuickGetText(2, row-1) );
            double prevRowMinHeight = atof(prevRowMinHeightStr);

            if (minHeight <= prevRowMinHeight)
            {
               MessageBox("Must enter a value greater than the Min Height of previous row.", "Invalid Min Height");
               return false;
            }
         }
         else
         {
            // Row in middle some place, check against previous and following.
            CString prevRowMinHeightStr( QuickGetText(2, row-1) );
            double prevRowMinHeight = atof(prevRowMinHeightStr);

            CString nextRowMinHeightStr( QuickGetText(2, row+1) );
            double nextRowMinHeight = atof(nextRowMinHeightStr);

            if (minHeight <= prevRowMinHeight || minHeight >= nextRowMinHeight)
            {
               MessageBox("Must enter a value between the Min Height of previous row and next row.", "Invalid Min Height");
               return false;
            }
         }

         // Prev row max height is same as this row min height.
         CString prevRowMaxHeight = string;

         if (!prevRowMaxHeight.IsEmpty())
         {
            QuickSetText(3, row-1, prevRowMaxHeight);
            UpdateHeightRangeFromGridSettings(row-1);
         }

         CHeightRange* heightRange = (CHeightRange*)GetRowData(row);
         if (heightRange != NULL)
            heightRange->SetMinHeight(atof(string));

         return true;

      }
      else if (col == 3)
      {
         // Should not happen
      }
   }

	return TRUE;
}



void CHeightAnalysisGrid::SetRow(int row, CString name, CString distance, CString min, CString max, CHeightRange* heightRange)
{
   if (row >= 0 && row < GetNumberRows())
   {
      QuickSetText(0, row, name);
      QuickSetText(1, row, distance);
      QuickSetText(2, row, min);
      QuickSetText(3, row, max);
      SetRowData(row, heightRange);
   }
}

void CHeightAnalysisGrid::UpdateHeightRangeFromGridSettings(int row)
{
   if (row >= 0 && row < GetNumberRows())
   {
      CString name( QuickGetText(0, row) );
      CString distance( QuickGetText(1, row) );
      CString minH( QuickGetText(2, row) );
      CString maxH( QuickGetText(3, row) );

      CHeightRange *hr = (CHeightRange*)GetRowData(row);

      if (hr == NULL)
      {
         // Add new
         hr = m_heightAnalysis->AddHeightRange(name, distance, minH, maxH);
         SetRowData(row, hr);
      }
      else
      {
         // Update existing
         hr->SetName(name);
         hr->SetOutlineDistance(atof(distance));
         hr->SetMinHeight(atof(minH));
         hr->SetMaxHeight(atof(maxH));
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuCommand
//		This notification is called when the user has selected a menu item
//		in the pop-up menu.
//	Params:
//		col, row - the cell coordinates of where the menu originated from
//		section - identify for which portion of the gird the menu is for.
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//		item - ID of the menu item selected
//	Return:
//		<none>
void CHeightAnalysisGrid::OnMenuCommand(int col,long row,int section,int item)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	//UNREFERENCED_PARAMETER(item);

   if (m_heightAnalysis != NULL)
   {

      switch (item)
      {
      case ID_HEIGHTANAYSIS_INSERT_BEFORE_SELECTED:
         {
            CString newHeightName( m_heightAnalysis->GetNewHeightName() );
            CString newDistance( QuickGetText(1, row-1) );
            CString newMinHeight( QuickGetText(2, row) );
            CString newMaxHeight( QuickGetText(3, row) );

            // InsertRow creates a new row at given index. So to "insert before"
            // we add a new row in place of the current row, current row shifts down.
            int newRowIndx = row;
            InsertRow(newRowIndx);
            CHeightRange *newHR = m_heightAnalysis->AddHeightRange(newHeightName, newDistance, newMinHeight, newMaxHeight);
            SetRow(newRowIndx, newHeightName, newDistance, newMinHeight, newMaxHeight, newHR);
         }
         break;

      case ID_HEIGHTANAYSIS_INSERT_AFTER_SELECTED:
         {
            CString newHeightName( m_heightAnalysis->GetNewHeightName() );
            CString newDistance( QuickGetText(1, row) );
            CString newMinHeight( QuickGetText(2, row) );
            CString newMaxHeight( QuickGetText(3, row) );

            if (row == (GetNumberRows() - 1))
            {
               newMaxHeight = "Infinity";
            }

            // InsertRow creates a new row at given index. So to "insert after"
            // we add a new row in place of the row that follows the current row.
            // If we are inserting after last existing row then need to use append.
            int newRowIndx = row+1;
            if (newRowIndx < GetNumberRows())
               InsertRow(newRowIndx);
            else
               AppendRow();
            
            CHeightRange *newHR = m_heightAnalysis->AddHeightRange(newHeightName, newDistance, newMinHeight, newMaxHeight);
            SetRow(newRowIndx, newHeightName, newDistance, newMinHeight, newMaxHeight, newHR);
         }
         break;

      case ID_HEIGHTANAYSIS_DELETE_SELECTED:
         if (GetNumberRows() > 1)
         {
            int rowCnt = GetNumberRows();

            if (row == 0) // deleting first row
            {
               // The deleting row is the first row so change the "min height" of 
               // the row after the deleting row to zero
               QuickSetText(2, row+1, "0");
               UpdateHeightRangeFromGridSettings(row+1);
            }
            else if (row == (rowCnt-1)) // deleting last row
            {
               // The deleting row is the last row so change the "max height" of
               // the row before the deleting row to infinity
               QuickSetText(2, row-1, "Infinity");
               UpdateHeightRangeFromGridSettings(row-1);
            }
            else // deleting some middle row, neither first nor last
            {
               // Change the "min height" of the row after the deleting row
               // to the "max height" of the deleting row
               CString curRowMaxHeight( QuickGetText(3, row) );
               QuickSetText(2, row+1, curRowMaxHeight);
               UpdateHeightRangeFromGridSettings(row+1);
            }

            // Save name of entry we're about to nuke.
            CString gonerName( QuickGetText(0, row) );

            // Update the testplan
            m_heightAnalysis->DeleteHeightRange(gonerName);

            // Delete the row from grid
            DeleteRow(row);
         }
         break;
      }

      RedrawAll();
   }
}

/////////////////////////////////////////////////////////////////////////////
//	OnMenuStart
//		Is called when the pop up menu is about to be shown
//	Params:
//		col, row	- the cell coordinates of where the menu originated from
//		setcion		- identify for which portion of the gird the menu is for.
//					possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//	Return:
//		TRUE - to allow menu to show
//		FALSE - to prevent the menu from poping up
int CHeightAnalysisGrid::OnMenuStart(int col,long row,int section)
{
   // If R click is not within grid-proper, then cancel the menu.
   // e.g. no menu for top heading or scroll bars
   if (section != UG_GRID)
      return FALSE;

   // Might be in the grid section but not in a grid cell.
   // Reject these too.
   if (row < 0 || col < 0)
      return FALSE;

   // Skip out if disabled, go by background color
   CUGCell cell;
   GetCell(col, row, &cell);
   COLORREF currentBackColor = cell.GetBackColor();
   if (currentBackColor == this->GetColorGray())
      return FALSE;
   
   // Proceed with menu

   CString name = QuickGetText(0, row);
   if (name.IsEmpty())
      name = "selected";

   // The popup choices are connected to commands, the commands
   // will operate on the currently selected row, so set the selection.
   this->ClearSelections();
   SetMultiSelectMode(FALSE); // this is the default, but seems to be getting turned on somehow, not on purpose by our code
   this->Select(col, row);
   this->RedrawAll(); // maybe for speed should get old selection and just redraw that row

   EmptyMenu();

   CString option;

   // Insert before first row not allowed. This is same behavior as original flexgrid implementation.
   if (row > 0)
   {
      option.Format("Insert new before %s", name);
      this->AddMenuItem(ID_HEIGHTANAYSIS_INSERT_BEFORE_SELECTED, option);
   }

   // Can always insert after
   option.Format("Insert new after %s", name);
   this->AddMenuItem(ID_HEIGHTANAYSIS_INSERT_AFTER_SELECTED, option);

   // Not allowed to delete final single remaining row
   if (GetNumberRows() > 1)
   {
      option.Format("Delete %s", name);
      this->AddMenuItem(ID_HEIGHTANAYSIS_DELETE_SELECTED, option);
   }

	return TRUE;
}
