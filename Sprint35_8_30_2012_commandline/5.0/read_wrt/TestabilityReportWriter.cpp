

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "port.h"
#include "report.h"
#include "TypedContainer.h"
#include "CompValues.h"
#include "Net_Util.h"
#include "RwLib.h"
#include "DFT.h"
#include "DcaSettingsFile.h"
#include "TestabilityReportWriter.h"
#include ".\testabilityreportwriter.h"


//---------------------------------------------------------------------------

void TestabilityReport_WriteFile(const CString& filename, CCEtoODBDoc &doc, FormatStruct *format)
{
#ifdef TESREP_DISABLED
   ErrorMessage("Under construction.", "Testability Report Writer");
   return;
#else
	try
	{
		CTestabilityReportWriter reportWriter(doc, filename);

		reportWriter.WriteFiles(filename);
	}
	catch (CString exception)
	{
		ErrorMessage(exception, "Testability Report Writer", MB_OK);
	}
#endif
}

//***************************************************************************

CTestabilityReportWriter::CTestabilityReportWriter(CCEtoODBDoc& doc, CString outfilename)
: m_doc(doc)
, m_decimals(3)
, m_pageUnits(pageUnitsMilliMeters)
, m_boardOutlineFound(false)
, m_boardSizeX(0.)
, m_boardSizeY(0.)
, m_singlePinNetCount(0)
, m_multiPinNetCount(0)
, m_noPinNetCount(0)
, m_fullyProbedNetCount(0)
, m_partiallyProbedNetCount(0)
, m_notProbedNetCount(0)
, m_fittedCompCount(0)
, m_notFittedCompCount(0)
, m_fullyProbedCompCount(0)
, m_partiallyProbedCompCount(0)
, m_notProbedCompCount(0)
, m_electLayerCount(0)
, m_topHeightViolationCount(0)
, m_botHeightViolationCount(0)
, m_topDensityViolationCount(0)
, m_botDensityViolationCount(0)
, m_testPlan(NULL)
{
   m_pageUnits = doc.getSettings().getPageUnits();
   m_decimals = GetDecimals(m_pageUnits);

   GetSettings().LoadStandardSettingsFile();
   GetSettings().ApplyAttributes(&m_doc);

   //*rcf To make handling of cancel cleaner move this to a DefineReport func, that
   // happens at start of WriteFiles (probably) or pick something else.
   // Such would also make it easier to check for visible file before presenting this dialog.
   // Right now one goes through all the settings and then it fails, should detect such inevitable failure sooner.
   CTRSettingsDlg settingsDlg(GetSettings(), m_pageUnits);
   if (settingsDlg.DoModal() == IDOK)
   {
      //                      Section             Section                     User Selectable
      //                       Name                Title                     Write or Not Write
      m_sectionAry.AddSection("BOARD",       "Board Statistics",           GetSettings().GetWriteBoard());
      m_sectionAry.AddSection("ISSUES",      "Top Issues",                 GetSettings().GetWriteTopIssues());
      m_sectionAry.AddSection("TESTABILITY", "Testability Detail",         GetSettings().GetWriteTestabilityDetail());
      m_sectionAry.AddSection("NET",         "Net Detail",                 GetSettings().GetWriteNetDetail());
      m_sectionAry.AddSection("PROBES",      "Probe Detail",               GetSettings().GetWriteProbeDetail());
      m_sectionAry.AddSection("HEIGHT",      "Height Detail",              GetSettings().GetWriteHeightDetail());
      m_sectionAry.AddSection("KELVIN",      "Kelvin (4-Wire) Results",    GetSettings().GetWriteKelvin());
      m_sectionAry.AddSection("PI",          "Power Injection Results",    GetSettings().GetWritePowerInjection());
      m_sectionAry.AddSection("PARALLEL",    "Parallel Analysis Results",  GetSettings().GetWriteParallelAnalysis());
      m_sectionAry.AddSection("PARTS",       "Part List",                  GetSettings().GetWriteParts());
   }



}

//---------------------------------------------------------------------------


CTestabilityReportWriter::~CTestabilityReportWriter()
{
}

//---------------------------------------------------------------------------

CString CTestabilityReportWriter::PercentStr(int numerator, int denominator)
{
   // For purposes here we don't expect things like 250%, so flag an error
   if (numerator > denominator)
      return "---";

   // Allow zero only if numerator is zero
   if (numerator == 0)
      return "0%";

   // Allow 100 only if numerator and denominator are exact match
   if (numerator == denominator)
      return "100%";

   // All else returns integer percent between 1 and 99, inclusive

   double numer = numerator;
   double denom = denominator;
   double fraction = numer / denom;

   if (fraction <= 0.010)
      return "1%";

   if (fraction >= 0.990)
      return "99%";

   CString buf;
   buf.Format("%.0f%%", fraction * 100.0);

   return buf;
}

//---------------------------------------------------------------------------

// Could instead cache probe count on ctrcomppin during analyze, then
// ctrcomponent could provide this count

int CTestabilityReportWriter::GetProbedPinCount(CTRComponent *trcomp)
{
   // Does not include no-connect pins

   int count = 0;

   if (trcomp != NULL)
   {
      POSITION pos = trcomp->GetCompPinMap().GetStartPosition();
      while (pos != NULL)
      {
         CString pinrefname;
         CTRCompPin *trcomppin;
         trcomp->GetCompPinMap().GetNextAssoc(pos, pinrefname, trcomppin);
         if (!trcomppin->IsNoConnect())
         {
            CTRNet *trnet;
            m_netMap.Lookup(trcomppin->GetNetName(), trnet);
            if (trnet != NULL)
            {
               if (trnet->GetPlacedTestProbeCount() > 0)
               {
                  count++;
               }
            }
         }
      }
   }

   return count;
}

//***************************************************************************

CTRSettings::CTRSettings()
: m_dpmo(0)
, m_maxHeightTop(0.)
, m_maxHeightBot(0.)
, m_topHeightUnits(pageUnitsInches)
, m_botHeightUnits(pageUnitsInches)
, m_maxProbesPerSquare(0)
, m_writeBoardSection(true)
, m_writeTopIssuesSection(true)
, m_writeTestabilityDetailSection(true)
, m_writeNetDetailSection(true)
, m_writeProbeDetailSection(true)
, m_writeHeightDetailSection(true)
, m_writeKelvinSection(true)
, m_writePowerInjectionSection(true)
, m_writeParallelAnalysisSection(true)
, m_writePartsSection(true)
{
}


//---------------------------------------------------------------------------

void CTRSettings::LoadStandardSettingsFile()
{
   CFilePath settingsFilePath(getApp().getExportSettingsFilePath("TestabilityReport.Out"));

   if (fileExists(settingsFilePath.getPath()))
   {
      LoadFile(settingsFilePath.getPath());
   }
}

//---------------------------------------------------------------------------

void CTRSettings::LoadFile(const CString& filePath)
{
   CSettingsFile sf;

   if (! sf.open(filePath))
   {
      formatMessageBox("Could not open settings file '%s'\n", filePath);
      return;
   }

   while (sf.getNextCommandLine())
   {
      if      (sf.isCommand(".WriteBoardSection"               ,2))  sf.parseYesNoParam(1, m_writeBoardSection                ,true);
      else if (sf.isCommand(".WriteTopIssuesSection"           ,2))  sf.parseYesNoParam(1, m_writeTopIssuesSection            ,true);
      else if (sf.isCommand(".WriteTestabilityDetailSection"   ,2))  sf.parseYesNoParam(1, m_writeTestabilityDetailSection    ,true);
      else if (sf.isCommand(".WriteNetDetailSection"           ,2))  sf.parseYesNoParam(1, m_writeNetDetailSection            ,true);
      else if (sf.isCommand(".WriteProbeDetailSection"         ,2))  sf.parseYesNoParam(1, m_writeProbeDetailSection          ,true);
      else if (sf.isCommand(".WriteHeightSection"              ,2))  sf.parseYesNoParam(1, m_writeHeightDetailSection         ,true);
      else if (sf.isCommand(".WriteKelvinSection"              ,2))  sf.parseYesNoParam(1, m_writeKelvinSection               ,true);
      else if (sf.isCommand(".WritePowerInjectionSection"      ,2))  sf.parseYesNoParam(1, m_writePowerInjectionSection       ,true);
      else if (sf.isCommand(".WriteParallelAnalysisSection"    ,2))  sf.parseYesNoParam(1, m_writeParallelAnalysisSection     ,true);
      else if (sf.isCommand(".WritePartsSection"               ,2))  sf.parseYesNoParam(1, m_writePartsSection                ,true);
      else if (sf.isCommand(".DPMO"                            ,2))  m_dpmo = sf.getIntParam(1);
      else if (sf.isCommand(".MaxProbesPerSquare"              ,2))  m_maxProbesPerSquare = sf.getIntParam(1);
      else if (sf.isCommand(".ProbeAreaSideLength"   ,2))  
      {
         m_areaSideLength = sf.getDoubleParam(1);
         if (sf.getParamCount() > 2)
         {
            CString units = sf.getParam(2);
            if (units.CompareNoCase("inch") == 0)
               m_sideLengthUnits = pageUnitsInches;
            else if (units.CompareNoCase("mm") == 0 || units.CompareNoCase("millimeters") == 0)
               m_sideLengthUnits = pageUnitsMilliMeters;
         }
      }
      else if (sf.isCommand(".MaxHeightTop"   ,2))  
      {
         m_maxHeightTop = sf.getDoubleParam(1);
         if (sf.getParamCount() > 2)
         {
            CString units = sf.getParam(2);
            if (units.CompareNoCase("inch") == 0)
               m_topHeightUnits = pageUnitsInches;
            else if (units.CompareNoCase("mm") == 0 || units.CompareNoCase("millimeters") == 0)
               m_topHeightUnits = pageUnitsMilliMeters;
         }
      }
      else if (sf.isCommand(".MaxHeightBottom"   ,2))  
      {
         m_maxHeightBot = sf.getDoubleParam(1);
         if (sf.getParamCount() > 2)
         {
            CString units = sf.getParam(2);
            if (units.CompareNoCase("inch") == 0)
               m_botHeightUnits = pageUnitsInches;
            else if (units.CompareNoCase("mm") == 0 || units.CompareNoCase("millimeters") == 0)
               m_botHeightUnits = pageUnitsMilliMeters;
         }
      }
   }

}

//---------------------------------------------------------------------------

void CTRSettings::ApplyAttributes(CCEtoODBDoc *doc)
{
   if (doc != NULL)
   {
      FileStruct *visibleFile = doc->getCamCadData().getSingleVisiblePcb();

      if (visibleFile != NULL)
      {
         BlockStruct *fileBlk = visibleFile->getBlock();

         if (fileBlk != NULL)
         {
            int kw = doc->RegisterKeyWord(ATT_TESTABILITY_REPORT_SETTINGS, valueTypeString);
            Attrib *attrib = NULL;

            if (fileBlk->lookUpAttrib(kw, attrib))
            {
               CSupString settingSupStr( attrib->getStringValue() );
               CStringArray settingNVPs;
               settingSupStr.ParseQuote(settingNVPs, "[]");

               //*rcf This is a little crude, maybe be more graceful?
               //*rcf At least do some error checkc on commaPos before substringing with it.

               for (int i = 0; i < settingNVPs.GetCount(); i++)
               {
                  CString nameValuePair = settingNVPs.GetAt(i);
                  int commaPos = nameValuePair.Find(",");
                  CString name( nameValuePair.Left(commaPos) );
                  CString value(nameValuePair);
                  value.Delete(0, commaPos+1);

                  if (name.CompareNoCase("brd") == 0)
                     m_writeBoardSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("tpi") == 0)
                     m_writeTopIssuesSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("tst") == 0)
                     m_writeTestabilityDetailSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("net") == 0)
                     m_writeNetDetailSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("prb") == 0)
                     m_writeProbeDetailSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("hgh") == 0)
                     m_writeHeightDetailSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("klv") == 0)
                     m_writeKelvinSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("pwr") == 0)
                     m_writePowerInjectionSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("pll") == 0)
                     m_writeParallelAnalysisSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("prt") == 0)
                     m_writePartsSection = (value.CompareNoCase("Y") == 0);
                  else if (name.CompareNoCase("dpmo") == 0)
                  {
                     int val = atoi(value);
                     if (val < 0)
                        val = 0;
                     this->m_dpmo = val;
                  }
                  else if (name.CompareNoCase("tophgh") == 0)
                  {
                     CSupString multiVal( value );
                     CStringArray subVals;
                     multiVal.ParseQuote(subVals, ",");
                     double hgh = -1.;
                     PageUnitsTag units = pageUnitsUndefined;
                     if (subVals.GetCount() > 0)
                        hgh = atof(subVals.GetAt(0));
                     if (subVals.GetCount() > 1)
                        units = intToPageUnitsTag( atoi(subVals.GetAt(1)) );
                     if (hgh >= 0. && units != pageUnitsUndefined)
                        SetMaxHeightTop(hgh, units);
                  }
                  else if (name.CompareNoCase("bothgh") == 0)
                  {
                     CSupString multiVal( value );
                     CStringArray subVals;
                     multiVal.ParseQuote(subVals, ",");
                     double hgh = -1.;
                     PageUnitsTag units = pageUnitsUndefined;
                     if (subVals.GetCount() > 0)
                        hgh = atof(subVals.GetAt(0));
                     if (subVals.GetCount() > 1)
                        units = intToPageUnitsTag( atoi(subVals.GetAt(1)) );
                     if (hgh >= 0. && units != pageUnitsUndefined)
                        SetMaxHeightBot(hgh, units);
                  }
                  else if (name.CompareNoCase("prbden") == 0)
                  {
                     CSupString multiVal( value );
                     CStringArray subVals;
                     multiVal.ParseQuote(subVals, ",");
                     int cnt = -1;
                     double len = -1.;
                     PageUnitsTag units = pageUnitsUndefined;
                     if (subVals.GetCount() > 0)
                        cnt = atoi(subVals.GetAt(0));
                     if (subVals.GetCount() > 1)
                        len = atof(subVals.GetAt(1));
                     if (subVals.GetCount() > 2)
                        units = intToPageUnitsTag( atoi(subVals.GetAt(2)) );
                     if (cnt >= 0 && len >= 0. && units != pageUnitsUndefined)
                     {
                        SetMaxProbesPerSqr(cnt);
                        SetSideLength(len, units);
                     }
                  }

               }
            }
         }
      }
   }
}

//---------------------------------------------------------------------------

void CTRSettings::SaveAttributes(CCEtoODBDoc *doc)
{
   if (doc != NULL)
   {
      FileStruct *visibleFile = doc->getCamCadData().getSingleVisiblePcb();

      if (visibleFile != NULL)
      {
         BlockStruct *fileBlk = visibleFile->getBlock();

         if (fileBlk != NULL)
         {
            CString buf;
            int kw = doc->RegisterKeyWord(ATT_TESTABILITY_REPORT_SETTINGS, valueTypeString);

            // kw,val|kw,val|  or   kw|val,kw|val,kw|val   [kw,val][kw,val][kw,val][kw|val]
            CString settingStr;
            settingStr += "[brd," + (CString)(m_writeBoardSection?"Y":"N") + "]";
            settingStr += "[tpi," + (CString)(m_writeTopIssuesSection?"Y":"N") + "]";
            settingStr += "[tst," + (CString)(m_writeTestabilityDetailSection?"Y":"N") + "]";
            settingStr += "[net," + (CString)(m_writeNetDetailSection?"Y":"N") + "]";
            settingStr += "[prb," + (CString)(m_writeProbeDetailSection?"Y":"N") + "]";
            settingStr += "[hgh," + (CString)(m_writeHeightDetailSection?"Y":"N") + "]";
            settingStr += "[klv," + (CString)(m_writeKelvinSection?"Y":"N") + "]";
            settingStr += "[pwr," + (CString)(m_writePowerInjectionSection?"Y":"N") + "]";
            settingStr += "[pll," + (CString)(m_writeParallelAnalysisSection?"Y":"N") + "]";
            settingStr += "[prt," + (CString)(m_writePartsSection?"Y":"N") + "]";

            buf.Format("%d", m_dpmo);
            settingStr += "[dpmo," + buf + "]";

            buf.Format("%f,%d", m_maxHeightTop, m_topHeightUnits);
            settingStr += "[tophgh," + buf + "]";

            buf.Format("%f,%d", m_maxHeightBot, m_botHeightUnits);
            settingStr += "[bothgh," + buf + "]";

            buf.Format("%d,%f,%d", m_maxProbesPerSquare, m_areaSideLength, m_sideLengthUnits);
            settingStr += "[prbden," + buf + "]";

            doc->SetAttrib(&fileBlk->getAttributesRef(), kw, settingStr);
         }
      }

   }
}

//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CTRSettingsDlg, CDialog)

CTRSettingsDlg::CTRSettingsDlg(CTRSettings &settings, PageUnitsTag currentPageUnits, CWnd* pParent /*=NULL*/)
: CDialog(CTRSettingsDlg::IDD, pParent)
, m_settings(settings)
, m_cczPageUnits(currentPageUnits)
, m_writeBoardStats(TRUE)
, m_writeTopIssues(TRUE)
, m_writeTestability(TRUE)
, m_writeNetDetails(TRUE)
, m_writeProbeDetails(TRUE)
, m_writeHeightDetails(TRUE)
, m_writeKelvinDetails(TRUE)
, m_writePowerDetails(TRUE)
, m_writeParallelAnalysis(TRUE)
, m_writeParts(TRUE)
, m_baseDPMOStr(_T(""))
{
}

CTRSettingsDlg::~CTRSettingsDlg()
{
}

void CTRSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_CHK_BOARD_STATS,   m_writeBoardStats);
   DDX_Check(pDX, IDC_CHK_TOP_ISSUES,    m_writeTopIssues);
   DDX_Check(pDX, IDC_CHK_TESTABILITY,   m_writeTestability);
   DDX_Check(pDX, IDC_CHK_NET_DETAILS,   m_writeNetDetails);
   DDX_Check(pDX, IDC_CHK_PROBE_DETAILS, m_writeProbeDetails);
   DDX_Check(pDX, IDC_CHK_COMP_HEIGHT,   m_writeHeightDetails);
   DDX_Check(pDX, IDC_CHK_KELVIN,        m_writeKelvinDetails);
   DDX_Check(pDX, IDC_CHK_POWER_DETAILS, m_writePowerDetails);
   DDX_Check(pDX, IDC_CHK_PARALLEL_ANAL, m_writeParallelAnalysis);
   DDX_Check(pDX, IDC_CHK_PARTS,         m_writeParts);

   DDX_Text(pDX,  IDC_BASE_DPMO,         m_baseDPMOStr);
   DDX_Text(pDX,  IDC_HEIGHT_TOP,        m_topHeightStr);
   DDX_Text(pDX,  IDC_HEIGHT_BOT,        m_botHeightStr);
   DDX_Text(pDX,  IDC_MAX_PROBES,        m_maxProbesStr);
   DDX_Text(pDX,  IDC_AREA_SIDE_LENGTH,  m_sideLengthStr);
}


BOOL CTRSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

   m_writeBoardStats = m_settings.GetWriteBoard();
   m_writeTopIssues = m_settings.GetWriteTopIssues();
   m_writeTestability = m_settings.GetWriteTestabilityDetail();
   m_writeNetDetails = m_settings.GetWriteNetDetail();
   m_writeProbeDetails = m_settings.GetWriteProbeDetail();
   m_writeHeightDetails = m_settings.GetWriteHeightDetail();
   m_writeKelvinDetails = m_settings.GetWriteKelvin();
   m_writePowerDetails = m_settings.GetWritePowerInjection();
   m_writeParallelAnalysis = m_settings.GetWriteParallelAnalysis();
   m_writeParts = m_settings.GetWriteParts();

   if (m_settings.GetDPMO() > 0)
      m_baseDPMOStr.Format("%d", m_settings.GetDPMO());
   else
      m_baseDPMOStr = "1000";

   if (m_settings.GetMaxHeightTop(m_cczPageUnits) >= 0.) 
      m_topHeightStr.Format("%f", m_settings.GetMaxHeightTop(m_cczPageUnits));

   if (m_settings.GetMaxHeightBot(m_cczPageUnits) >= 0.) 
      m_botHeightStr.Format("%f", m_settings.GetMaxHeightBot(m_cczPageUnits));

   if (m_settings.GetMaxProbesPerSqr() > 0)
      m_maxProbesStr.Format("%d", m_settings.GetMaxProbesPerSqr());

   if (m_settings.GetAreaSideLength(m_cczPageUnits) > 0.) 
      m_sideLengthStr.Format("%f", m_settings.GetAreaSideLength(m_cczPageUnits));

   UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CTRSettingsDlg, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTRSettingsDlg, CDialog)
END_EVENTSINK_MAP()


void CTRSettingsDlg::OnBnClickedOk()
{
   UpdateData(TRUE);

   m_settings.SetWriteBoard(m_writeBoardStats?true:false);
   m_settings.SetWriteTopIssues(m_writeTopIssues?true:false);
   m_settings.SetWriteTestabilityDetail(m_writeTestability?true:false);
   m_settings.SetWriteNetDetail(m_writeNetDetails?true:false);
   m_settings.SetWriteProbeDetail(m_writeProbeDetails?true:false);
   m_settings.SetWriteHeightDetail(m_writeHeightDetails?true:false);
   m_settings.SetWriteKelvin(m_writeKelvinDetails?true:false);
   m_settings.SetWritePowerInjection(m_writePowerDetails?true:false);
   m_settings.SetWriteParallelAnalysis(m_writeParallelAnalysis?true:false);
   m_settings.SetWriteParts(m_writeParts?true:false);

   int baseDPMO = atoi(m_baseDPMOStr);
   if (baseDPMO < 1)
      baseDPMO = 0;
   m_settings.SetDPMO(baseDPMO);

   double topHeight = atof(m_topHeightStr);
   if (topHeight < 0.)
      topHeight = 0.;
   m_settings.SetMaxHeightTop(topHeight, m_cczPageUnits);

   double botHeight = atof(m_botHeightStr);
   if (botHeight < 0.)
      botHeight = 0.;
   m_settings.SetMaxHeightBot(botHeight, m_cczPageUnits);

   int maxProbes = atoi(m_maxProbesStr);
   if (maxProbes < 1)
      maxProbes = 0;
   m_settings.SetMaxProbesPerSqr(maxProbes);

   double sideLength = atof(m_sideLengthStr);
   if (sideLength < 0.)
      sideLength = 0.;
   m_settings.SetSideLength(sideLength, m_cczPageUnits);

   OnOK();
}


//***************************************************************************

bool CTestabilityReportWriter::WriteFiles(CString filepath)
{
   //*rcf Temp way to handle CANCEL
   // If cancel was clicked in the settings dialog then there will be no active sections
   // in the report, so just silently leave.
   if (m_sectionAry.GetCount() < 1)
      return false;

   // Save current settings
   GetSettings().SaveAttributes(&m_doc);  //* change from ptr to reference someday

   // Must have a visible PCB file
   m_visibleFile = m_doc.getCamCadData().getSingleVisiblePcb();
   if (m_visibleFile == NULL)
   {
      ErrorMessage("Must have a single visible PCB file to create report.", "Testability Report Writer", MB_OK | MB_ICONHAND);
      return false;
   }

   // Dig deep, does almost all the work here
   Analyze();

   // Now just spill the resuls of analysis
	CFileException e;
	CString line;

	if (!m_file.Open(filepath, CFile::modeCreate|CFile::modeWrite, &e))
	{
      CString tmp;
		char msg[255];
		if (e.m_cause != e.none && e.GetErrorMessage(msg, 255))
			tmp.Format("%s [%s]", msg, filepath);
      else
         tmp.Format("Can not open %s for writing.", filepath);

      ErrorMessage(tmp, "Testability Report Writer", MB_OK | MB_ICONHAND);

      return false;
   }

   CTime time = CTime::GetCurrentTime();

   m_file.WriteString("<HTML>\n");

   m_file.WriteString("<HEAD>\n");
   m_file.WriteString("<TITLE>Testability Report</TITLE>\n");
   m_file.WriteString("</HEAD>\n");

   m_file.WriteString("<BODY>\n");
   m_file.WriteString("<H1>CAMCAD Professional Testability Report</H1>\n");
   
   m_file.WriteString("<H3>%s %s %s</H3>\n", m_visibleFile != NULL ? m_visibleFile->getName() : "", 
      getApp().getCamCadSubtitle(), time.Format("%A, %B %d, %Y - %X"));

   WriteTableOfContents();
   WriteSections();
   
   m_file.WriteString("</BODY>\n");
   m_file.WriteString("</HTML>\n");


   return true;
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteTableOfContents()
{
   m_file.WriteString("<H2><A NAME=\"TOC\">Table Of Contents</A></H2>\n");

   m_file.WriteString("<UL>\n");

   for (int i = 0; i < m_sectionAry.GetCount(); i++)
   {
      CTRSection *section = m_sectionAry.GetAt(i);
      if (section->IsVisible())
      {
         m_file.WriteString("<LI><A HREF=\"#%s\">%s</A></LI>\n",
            section->GetName(), section->GetTitle());
      }
   }

   m_file.WriteString("</UL>\n");
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteSections()
{
   for (int i = 0; i < m_sectionAry.GetCount(); i++)
   {
      CTRSection *section = m_sectionAry.GetAt(i);
      if (section->IsVisible())
      {
         CString sectionname( section->GetName() );  //*rcf maybe add an enum so this is not dependent on names

         if (sectionname.CompareNoCase("BOARD") == 0)
            WriteBoardSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("ISSUES") == 0)
            WriteIssuesSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("TESTABILITY") == 0)
            WriteTestabilitySection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("NET") == 0)
            WriteNetSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("PROBES") == 0)
            WriteProbeSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("HEIGHT") == 0)
            WriteHeightSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("KELVIN") == 0)
            WriteKelvinSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("PI") == 0)
            WritePowerSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("PARALLEL") == 0)
            WriteParallelAnalysisSection(section->GetName(), section->GetTitle());

         else if (sectionname.CompareNoCase("PARTS") == 0)
            WritePartSection(section->GetName(), section->GetTitle());
      }
   }

}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteBogusSection(CString name, CString title)
{
   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   m_file.WriteString("<P>\n");
   m_file.WriteString("Implementation Pending For This Report Section");
   m_file.WriteString("</P>\n");
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteBoardSection(CString name, CString title)
{
   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   CString boardsizeStr("Valid board outline not found.");
   if (m_boardOutlineFound)
      boardsizeStr.Format("%s x %s %s\n", FormatDecimal(m_boardSizeX, m_decimals) , FormatDecimal(m_boardSizeY, m_decimals), pageUnitsTagToString(m_pageUnits));


   //----------

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" ROWSPAN=\"2\">%s</TH>\n", "CAD File");
   m_file.WriteString("<TH ALIGN=\"CENTER\" ROWSPAN=\"2\">%s</TH>\n", "Board Size");
   m_file.WriteString("<TH ALIGN=\"CENTER\" ROWSPAN=\"2\">%s</TH>\n", "Electrical Layers");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"3\" WIDTH=\"45%%\">%s</TH>\n", "Component Count");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"12%%\">%s</TH>\n", "Total");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"12%%\">%s</TH>\n", "Fitted");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"12%%\">%s</TH>\n", "Not Fitted");
   m_file.WriteString("</TR>\n");

   int totalCompCount = m_fittedCompCount + m_notFittedCompCount;

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"CENTER\" >%s</TD>\n", NonEmptyStr(m_visibleFile != NULL ? m_visibleFile->getName() : "Error: Not exactly 1 visible file"));
   m_file.WriteString("<TD ALIGN=\"CENTER\" >%s</TD>\n", NonEmptyStr(boardsizeStr));
   m_file.WriteString("<TD ALIGN=\"CENTER\" >%d</TD>\n", m_electLayerCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"12%%\">%d</TD>\n", totalCompCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"12%%\">%d</TD>\n", m_fittedCompCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"12%%\">%d</TD>\n", m_notFittedCompCount);
   m_file.WriteString("<TR>\n");

   m_file.WriteString("</TABLE>\n");

   //----------

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"4\" WIDTH=\"60%%\">%s</TH>\n", "Net Count");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"2\" WIDTH=\"40%%\">%s</TH>\n", "Height Violations");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Total");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Multi Pin");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Single Pin");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Unconnected");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Top");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Bottom");
   m_file.WriteString("</TR>\n");

   int totalNetCount = m_singlePinNetCount + m_multiPinNetCount + m_noPinNetCount;

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%d</TD>\n", totalNetCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%d</TD>\n", m_multiPinNetCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%d</TD>\n", m_singlePinNetCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%d</TD>\n", m_noPinNetCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%d</TD>\n", m_topHeightViolationCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%d</TD>\n", m_botHeightViolationCount);
   m_file.WriteString("</TR>\n");

   m_file.WriteString("</TABLE>\n");

   //----------

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"3\" WIDTH=\"48%%\">%s</TH>\n", "Nets Probed");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"3\" WIDTH=\"48%%\">%s</TH>\n", "Components Probed");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Fully");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Partially");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Not");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Fully");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Partially");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Not");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%d (%s)</TD>\n", m_fullyProbedNetCount,      PercentStr(m_fullyProbedNetCount,      totalNetCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%d (%s)</TD>\n", m_partiallyProbedNetCount,  PercentStr(m_partiallyProbedNetCount,  totalNetCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%d (%s)</TD>\n", m_notProbedNetCount,        PercentStr(m_notProbedNetCount,        totalNetCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%d (%s)</TD>\n", m_fullyProbedCompCount,     PercentStr(m_fullyProbedCompCount,     totalCompCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%d (%s)</TD>\n", m_partiallyProbedCompCount, PercentStr(m_partiallyProbedCompCount, totalCompCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%d (%s)</TD>\n", m_notProbedCompCount,       PercentStr(m_notProbedCompCount,       totalCompCount));
   m_file.WriteString("</TR>\n");

   m_file.WriteString("</TABLE>\n");

   //----------

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"2\" >%s</TH>\n", "Areas Exceeding Probe Density Threshold");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"50%%\">%s</TH>\n", "Top");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"50%%\">%s</TH>\n", "Bottom");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d</TD>\n", m_topDensityViolationCount);
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d</TD>\n", m_botDensityViolationCount);
   m_file.WriteString("</TR>\n");

   m_file.WriteString("</TABLE>\n");

   //----------

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" ROWSPAN=\"2\" >%s</TH>\n", "PCB Side");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"2\" >%s</TH>\n", "Probe Count");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"33%%\">%s</TH>\n", "Placed");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"33%%\">%s</TH>\n", "Unplaced");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%s</TD>\n", "Top");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%d</TD>\n", m_topProbeMap.GetPlacedProbeCount());
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%d</TD>\n", m_topProbeMap.GetUnplacedProbeCount());
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%s</TD>\n", "Bottom");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%d</TD>\n", m_botProbeMap.GetPlacedProbeCount());
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%d</TD>\n", m_botProbeMap.GetUnplacedProbeCount());
   m_file.WriteString("</TR>\n");

   m_file.WriteString("</TABLE>\n");

   //----------

   // Tooling Holes

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"4\" WIDTH=\"100%%\">%s</TH>\n", "Tooling Holes");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "X");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Y");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Size");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Plated");
   m_file.WriteString("</TR>\n");

   if (m_visibleFile != NULL && m_visibleFile->getBlock() != NULL)
   {
	   POSITION pos = m_visibleFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = m_visibleFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypeDrillTool))
         {
            InsertStruct *insert = datum->getInsert();
            BlockStruct *toolBlk = this->m_doc.getBlockAt( insert->getBlockNumber() );

            double size = 0.;
            CString plated("No");

            if (toolBlk != NULL)
            {
               size = toolBlk->getToolSize();
               plated = toolBlk->getToolHolePlated()?"Yes":"No";
            }

            m_file.WriteString("<TR>\n");
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", FormatDecimal(insert->getOriginX(), m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", FormatDecimal(insert->getOriginY(), m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", FormatDecimal(size                , m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", NonEmptyStr(plated));
            m_file.WriteString("</TR>\n");            
         }
      }
   }

   m_file.WriteString("</TABLE>\n");

   //----------

   // Fiducials

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"4\" WIDTH=\"100%%\">%s</TH>\n", "Fiducials");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Name");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "PCB Side");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "X");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Y");
   m_file.WriteString("</TR>\n");

   if (m_visibleFile != NULL && m_visibleFile->getBlock() != NULL)
   {
	   POSITION pos = m_visibleFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = m_visibleFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypeFiducial))
         {
            InsertStruct *insert = datum->getInsert();

            m_file.WriteString("<TR>\n");
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", NonEmptyStr(insert->getRefname()));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", NonEmptyStr(insert->getPlacedTop()?"Top":"Bottom"));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", FormatDecimal(insert->getOriginX(), m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TD>\n", FormatDecimal(insert->getOriginY(), m_decimals));
            m_file.WriteString("</TR>\n");            
         }
      }
   }

   m_file.WriteString("</TABLE>\n");

   //----------
   
   // Dies

   if(m_componentMap.GetDieCount())
   {
      m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"4\" WIDTH=\"25%%\">%s</TH>\n", "Die Count");
      m_file.WriteString("</TR>\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d</TD>\n", m_componentMap.GetDieCount());
      m_file.WriteString("</TR>\n");            

      m_file.WriteString("</TABLE>\n");
   }

   //----------
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteIssuesSection(CString name, CString title)
{
   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   //----------

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"2\" WIDTH=\"50%%\">%s</TH>\n", "Nets");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"2\" WIDTH=\"50%%\">%s</TH>\n", "Components");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Partially Probed");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Unprobed");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Partially Probed");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"25%%\">%s</TH>\n", "Unprobed");
   m_file.WriteString("</TR>\n");

   int totalNetCount = m_singlePinNetCount + m_multiPinNetCount + m_noPinNetCount;
   int totalCompCount = m_fittedCompCount + m_notFittedCompCount;

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d (%s)</TD>\n", m_partiallyProbedNetCount,  PercentStr(m_partiallyProbedNetCount,  totalNetCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d (%s)</TD>\n", m_notProbedNetCount,        PercentStr(m_notProbedNetCount,        totalNetCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d (%s)</TD>\n", m_partiallyProbedCompCount, PercentStr(m_partiallyProbedCompCount, totalCompCount));
   m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d (%s)</TD>\n", m_notProbedCompCount,       PercentStr(m_notProbedCompCount,       totalCompCount));
   m_file.WriteString("</TR>\n");

   m_file.WriteString("</TABLE>\n");

   //----------

   if (m_topDensityViolationCount > 0 || m_botDensityViolationCount > 0)
   {
      m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"2\" >%s</TH>\n", "Areas Exceeding Probe Density Threshold");
      m_file.WriteString("</TR>\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"50%%\">%s</TH>\n", "Top");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"50%%\">%s</TH>\n", "Bottom");
      m_file.WriteString("</TR>\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d</TD>\n", m_topDensityViolationCount);
      m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d</TD>\n", m_botDensityViolationCount);
      m_file.WriteString("</TR>\n");

      m_file.WriteString("</TABLE>\n");
   }

   //----------

   if (m_topHeightViolationCount > 0 || m_botHeightViolationCount > 0)
   {
      m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"2\" >%s</TH>\n", "Height Violations");
      m_file.WriteString("</TR>\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"50%%\">%s</TH>\n", "Top");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"50%%\">%s</TH>\n", "Bottom");
      m_file.WriteString("</TR>\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d</TD>\n", m_topHeightViolationCount);
      m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"25%%\">%d</TD>\n", m_botHeightViolationCount);
      m_file.WriteString("</TR>\n");

      m_file.WriteString("</TABLE>\n");
   }

   //----------

   CAccessAnalysisSolution *aasol = NULL;
   /*CDFTSolution *dftSolution = m_doc.GetCurrentDFTSolution(*m_visibleFile); // can't get this from CCamCadData, too bad
   if (dftSolution != NULL)
      aasol = dftSolution->GetAccessAnalysisSolution();

   if (aasol != NULL)
   {
      m_netMap.setSortFunction(CTRNetMap::DescendingDPMOSortFunc);
      m_netMap.Sort();

      
      //m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

      m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"25%%\">%s</TH>\n",                "Net Name");
      m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"30%%\" COLSPAN=\"2\">%s</TH>\n", "Probes Required");
      m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"30%%\">%s</TH>\n",               "Probes Placed");
      m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"15%%\">%s</TH>\n",               "Potential DPMO");
      m_file.WriteString("</TR>\n");


      CString *netname;
      CTRNet *trnet = NULL;
      for (m_netMap.GetFirstSorted(netname, trnet); trnet != NULL; m_netMap.GetNextSorted(netname, trnet)) 
      {
         if (trnet->GetTestProbeStatus() != ProbeStatusFullyProbed && trnet->GetTestResourcesRequired() > 0)
         {
            CAANetAccess *netaccess = aasol->GetNetAccess(trnet->GetCamCadNet()->getNetName());

            if (netaccess != NULL)
            {
               int rowcount = 0;

               POSITION locpos = netaccess->GetHeadPosition_AccessibleLocations();
               while (locpos != NULL)
               {
                  CAAAccessLocation *accloc = netaccess->GetNext_AccessibleLocations(locpos);
                  bool accessible = accloc->GetAccessible();

                  if (!accessible)
                  {
                     rowcount++;
                  }
               }


               m_file.WriteString("<TR>\n");
               m_file.WriteString("<TD ALIGN=\"LEFT\" VALIGN=\"TOP\" WIDTH=\"25%%\" ROWSPAN=\"%d\">%s</TD>\n", rowcount+1, NonEmptyStr(trnet->GetCamCadNet()->getNetName()));
               m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"30%%\" COLSPAN=\"2\">%d</TD>\n", trnet->GetTestResourcesRequired());
               m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"30%%\">%d</TD>\n",               trnet->GetPlacedTestProbeCount());
               m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"15%%\">%d</TD>\n",               trnet->GetPotentialDPMO());
               m_file.WriteString("</TR>\n");

               locpos = netaccess->GetHeadPosition_AccessibleLocations();
               while (locpos != NULL)
               {
                  CAAAccessLocation *accloc = netaccess->GetNext_AccessibleLocations(locpos);
                  bool accessible = accloc->GetAccessible();

                  if (!accessible)
                  {
                     CString refname = accloc->GetRefDes();
                     CString targettype = accloc->GetTargetTypeToString();
                     CString name;
                     name.Format("%s [%s]", refname, targettype);
                     CString errmsg = accloc->GetErrorMessage(",<BR>");

                    /* CString surface("Unknown");
                     switch(accloc->GetAccessSurface())
                     {
                     case testSurfaceTop:
                        surface = "Top";
                        break;
                     case testSurfaceBottom:
                        surface = "Bottom";
                        break;
                     case testSurfaceBoth:
                        surface = "Both";
                        break;
                     }

                     m_file.WriteString("<TR>\n");
                     //m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(""));  // SKIP 1st col
                     m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(name));
                     m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TD>\n", NonEmptyStr(AccessSurfaceStr(accloc->GetAccessSurface())));
                     m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"30%%\">%s</TD>\n", NonEmptyStr(errmsg));
                     m_file.WriteString("<TD ALIGN=\"RIGHT\"  WIDTH=\"15%%\">%s</TD>\n", NonEmptyStr(""));
                     m_file.WriteString("</TR>\n");
                  }
               }
            }
         }
      }
      m_file.WriteString("</TABLE>\n");
   }*/

}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteTestabilitySection(CString name, CString title)
{
   m_componentMap.setSortFunction(CTRComponentMap::DescendingFinalDPMOSortFunc);
   m_componentMap.Sort();

   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");
   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TH>\n",  "Component");
   m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TH>\n", "Base DPMO");
   m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TH>\n", "Pin Count");
   m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TH>\n", "Process DPMO");
   m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TH>\n", "Pins Probed");
   m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TH>\n", "Final DPMO");
   m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TH>\n", "Reason");
   m_file.WriteString("</TR>\n");

   CString *ignoredKey;
   CTRComponent *trcomp = NULL;
   for (m_componentMap.GetFirstSorted(ignoredKey, trcomp); trcomp != NULL; m_componentMap.GetNextSorted(ignoredKey, trcomp)) 
   {
      if (trcomp->GetLoaded())
      {
         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",  NonEmptyStr(trcomp->GetComponentData()->getInsert()->getRefname() + (trcomp->IsDie()?"(Die)":"")));
         m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n", trcomp->GetBaseDPMO());
         m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n", trcomp->GetPinCount());
         m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n", trcomp->GetProcessDPMO());
         m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n", GetProbedPinCount(trcomp));
         m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n", trcomp->GetFinalDPMO());
         m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n", NonEmptyStr(trcomp->GetTestabilityReason(m_netMap)));
         m_file.WriteString("</TR>\n");
      }
   }

   // Total DPMO
   int totalProcessDPMO = m_componentMap.GetTotalProcessDPMO();
   int totalFinalDPMO = m_componentMap.GetTotalFinalDPMO();
   int totalTheoreticalDPMO = m_componentMap.GetTotalTheoreticalBestDPMO();

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",  NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",  NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n", NonEmptyStr("Total Process DPMO:"));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n", totalProcessDPMO);
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n", NonEmptyStr("Total Final DPMO:"));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n", totalFinalDPMO);
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n", NonEmptyStr());
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",    NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",    NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr("Theoretical Best DPMO:"));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%d</TD>\n",   totalTheoreticalDPMO);
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr());
   m_file.WriteString("</TR>\n");


   // Yield
   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",      NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",      NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",     NonEmptyStr("Process Yield:"));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%.2f%%</TD>\n", this->GetYield( totalProcessDPMO ));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",     NonEmptyStr("Final Yield:"));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%.2f%%</TD>\n", this->GetYield( totalFinalDPMO ));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",     NonEmptyStr());
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",    NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"14%%\">%s</TD>\n",    NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr());
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr("Theoretical Best Yield:"));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%.2f%%</TD>\n", this->GetYield( totalTheoreticalDPMO ));
   m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"14%%\">%s</TD>\n",   NonEmptyStr());
   m_file.WriteString("</TR>\n");


   m_file.WriteString("</TABLE>\n");
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteKelvinSection(CString name, CString title)
{
   m_componentMap.setSortFunction(CTRComponentMap::KelvinSectionSortFunc);
   m_componentMap.Sort();

   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   for (int pass = 0; pass < 3; pass++)
   {
      CString subheading;
      if (pass == 0)
         subheading = "Kelvin Resistors";
      else if (pass == 1)
         subheading = "Kelvin Capacitors";
      else
         subheading = "Kelvin Inductors";

      m_file.WriteString("<H3><A NAME=\"KELVIN%d\">%s</A></H2>\n", pass, subheading);

      m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");
      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TH>\n",   "Component");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Value");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Probe Status");
      m_file.WriteString("</TR>\n");


      CString *ignoredKey;
      CTRComponent *trcomp = NULL;
      for (m_componentMap.GetFirstSorted(ignoredKey, trcomp); trcomp != NULL; m_componentMap.GetNextSorted(ignoredKey, trcomp)) 
	   {
         if (trcomp->GetKelvin())
         {
            if ((pass == 0 && trcomp->GetDeviceType() == deviceTypeResistor) || 
               (pass == 1 && trcomp->GetDeviceType() == deviceTypeCapacitor) ||
               (pass == 2 && trcomp->GetDeviceType() == deviceTypeInductor))
            {
               m_file.WriteString("<TR>\n");
               m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TD>\n",   trcomp->GetComponentData()->getInsert()->getRefname());
               m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", trcomp->GetValue());
               m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", trcomp->GetProbeStatusStr());
               m_file.WriteString("</TR>\n");
            }
         }
      }
      
      m_file.WriteString("</TABLE>\n");
   }

}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteHeightSection(CString name, CString title)
{
   m_componentMap.setSortFunction(CTRComponentMap::DescendingHeightSortFunc);
   m_componentMap.Sort();

   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   for (int pass = 0; pass < 2; pass++)
   {
      CString subheading("Subheading Error");
      if (pass == 0)
         subheading = "Top";
      else if (pass == 1)
         subheading = "Bottom";

      m_file.WriteString("<H3><A NAME=\"HEIGHT%d\">%s</A></H2>\n", pass, subheading);

      m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");
      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TH>\n",   "Component");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Height");
      m_file.WriteString("</TR>\n");

      double topThreshold = GetSettings().GetMaxHeightTop(m_doc.getPageUnits());
      double botThreshold = GetSettings().GetMaxHeightBot(m_doc.getPageUnits());

      if ((pass == 0 && topThreshold >= 0.0) ||
         (pass == 1 && botThreshold >= 0.0))
      {
         CString *ignoredKey;
         CTRComponent *trcomp = NULL;
         for (m_componentMap.GetFirstSorted(ignoredKey, trcomp); trcomp != NULL; m_componentMap.GetNextSorted(ignoredKey, trcomp)) 
         {
            bool writeComp = false;
            if (pass == 0 && trcomp->GetComponentData()->getInsert()->getPlacedTop())
            {
               if (trcomp->GetHeight() > topThreshold)
                  writeComp = true;
            }
            else if (pass == 1 && trcomp->GetComponentData()->getInsert()->getPlacedBottom())
            {
               if (trcomp->GetHeight() > botThreshold)
                  writeComp = true;
            }

            if (writeComp)
            {
               m_file.WriteString("<TR>\n");
               m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TD>\n",   trcomp->GetComponentData()->getInsert()->getRefname() + (trcomp->IsDie()?"(Die)":""));
               m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%*f</TD>\n", m_decimals, trcomp->GetHeight());
               m_file.WriteString("</TR>\n");
            }
         }
      }

      m_file.WriteString("</TABLE>\n");
      
   }
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteParallelAnalysisSection(CString name, CString title)
{
   m_componentMap.setSortFunction(CTRComponentMap::MergedStatusSortFunc);
   m_componentMap.Sort();

   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");
   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"15%%\">%s</TH>\n",   "Component");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Merged Status");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Value");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Merged Value");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Net 1");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TH>\n", "Net 2");
   m_file.WriteString("</TR>\n");

   CString *ignoredKey;
   CTRComponent *trcomp = NULL;
   for (m_componentMap.GetFirstSorted(ignoredKey, trcomp); trcomp != NULL; m_componentMap.GetNextSorted(ignoredKey, trcomp)) 
   {
      if (trcomp->GetMergedStatus() == MergedStatusPrimary || trcomp->GetMergedStatus() == MergedStatusIgnored)
      {
         CTRCompPin *pin1 = trcomp->GetCompPinMap().GetPin(1);
         CTRCompPin *pin2 = trcomp->GetCompPinMap().GetPin(2);

         // Want to show net names consistently in columns, regardless of whether
         // net is on pin1 or pin 2
         CString net1("Not Found");
         CString net2("Not Found");
         if (pin1 != NULL && pin2 != NULL)
         {
            if (pin1->GetNetName().CompareNoCase(pin2->GetNetName()) <= 0)
            {
               net1 = pin1->GetNetName();
               net2 = pin2->GetNetName();
            }
            else
            {
               net1 = pin2->GetNetName();
               net2 = pin1->GetNetName();
            }
         }
         else 
         {
            if (pin1 != NULL)
               net1 = pin1->GetNetName();
            if (pin2 != NULL)
               net2 = pin2->GetNetName();
         }

         CString justify("LEFT");
         if (trcomp->GetMergedStatus() != MergedStatusPrimary)
            justify = "CENTER";

         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TD ALIGN=\"%s\" WIDTH=\"15%%\">%s</TD>\n",   justify, trcomp->GetComponentData()->getInsert()->getRefname());
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TD>\n", trcomp->GetMergedStatusStr());
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TD>\n", trcomp->GetValue());
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TD>\n", trcomp->GetMergedValue());
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TD>\n", net1);
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"15%%\">%s</TD>\n", net2);
         m_file.WriteString("</TR>\n");
      }
   }

   m_file.WriteString("</TABLE>\n");
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteNetSection(CString name, CString title)
{
   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   CAccessAnalysisSolution *aasol = NULL;
   /*CDFTSolution *dftSolution = m_doc.GetCurrentDFTSolution(*m_visibleFile); // can't get this from CCamCadData, too bad
   if (dftSolution != NULL)
      aasol = dftSolution->GetAccessAnalysisSolution();

   if (aasol != NULL)
   {
      m_netMap.setSortFunction(CTRNetMap::NetDetailsSortFunc);
      m_netMap.Sort();
      
      {
         // Analyzed nets

         m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"34%%\">%s</TH>\n",                "Net Name");
         m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"33%%\" COLSPAN=\"2\">%s</TH>\n", "Probes Required");
         m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"33%%\">%s</TH>\n",               "Probes Placed");
         /////m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"20%%\">%s</TH>\n", "Potential DPMO");  // removed by Mark's request
         m_file.WriteString("</TR>\n");


         CString *netname;
         CTRNet *trnet = NULL;
         for (m_netMap.GetFirstSorted(netname, trnet); trnet != NULL; m_netMap.GetNextSorted(netname, trnet)) 
         {
            /////if (trnet->GetTestProbeStatus() != ProbeStatusFullyProbed && trnet->GetTestResourcesRequired() > 0)
            {
               CAANetAccess *netaccess = aasol->GetNetAccess(trnet->GetCamCadNet()->getNetName());

               if (netaccess != NULL && netaccess->IsNetAnalyzed())
               {
                  int rowcount = 0;

                  POSITION locpos = netaccess->GetHeadPosition_AccessibleLocations();
                  while (locpos != NULL)
                  {
                     CAAAccessLocation *accloc = netaccess->GetNext_AccessibleLocations(locpos);
                     rowcount++;
                  }

                  m_file.WriteString("<TR>\n");
                  m_file.WriteString("<TD ALIGN=\"LEFT\" VALIGN=\"TOP\" WIDTH=\"34%%\" ROWSPAN=\"%d\">%s</TD>\n", rowcount+1, NonEmptyStr(trnet->GetCamCadNet()->getNetName()));
                  m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"33%%\" COLSPAN=\"2\">%d</TD>\n", trnet->GetTestResourcesRequired());
                  m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"33%%\">%d</TD>\n",               trnet->GetPlacedTestProbeCount());
                  /////m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"20%%\">%d</TD>\n", trnet->GetPotentialDPMO());   //*rcf removed
                  m_file.WriteString("</TR>\n");

                  for (int pass = 0; pass < 2; pass++)
                  {
                     locpos = netaccess->GetHeadPosition_AccessibleLocations();
                     while (locpos != NULL)
                     {
                        CAAAccessLocation *accloc = netaccess->GetNext_AccessibleLocations(locpos);
                        bool accessible = accloc->GetAccessible();
                        ETestSurface testsurf = accloc->GetAccessSurface();

                        CString surface("Unknown");
                        switch(testsurf)
                        {
                        case testSurfaceTop:
                           surface = "Top";
                           break;
                        case testSurfaceBottom:
                           surface = "Bottom";
                           break;
                        case testSurfaceBoth:
                           surface = "Both";
                           break;
                        }

                        if ((pass == 0 && !accessible) || (pass == 1 && accessible))
                        {
                           CString refname = accloc->GetRefDes();
                           CString targettype = accloc->GetTargetTypeToString();
                           CString name;
                           name.Format("%s [%s] %sAccessible", refname, targettype, accessible ? "" : "NOT ");
                           CString msg = accloc->GetErrorMessage(",<BR>");

                           int targetEntityNum = -1;
                           if (accloc->getFeatureEntity().getEntityType() == entityTypeData)
                           {
                              DataStruct* targetdata = accloc->getFeatureEntity().getData();
                              targetEntityNum = targetdata->getEntityNumber();
                           }
                           else if (accloc->getFeatureEntity().getEntityType() == entityTypeCompPin)
                           {
                              targetEntityNum = accloc->getFeatureEntity().getCompPin()->getEntityNumber();
                           }
                           CTRProbe *topProbe = NULL;
                           CTRProbe *botProbe = NULL;
                           if (targetEntityNum > -1)
                           {
                              topProbe = this->m_topProbeMap.GetProbeForTargetEntity(targetEntityNum);
                              botProbe = this->m_botProbeMap.GetProbeForTargetEntity(targetEntityNum);
                           }
                           CString probeMsg;
                           if (topProbe != NULL || botProbe != NULL)
                           {
                              bool plural = false;
                              CString probeNumberStr;
                              if (topProbe != NULL && topProbe->IsPlaced() && (testsurf == testSurfaceTop || testsurf == testSurfaceBoth))
                              {
                                 probeNumberStr += topProbe->GetCCProbeData()->getInsert()->getRefname();
                              }
                              if (botProbe != NULL && botProbe->IsPlaced() && (testsurf == testSurfaceBottom || testsurf == testSurfaceBoth))
                              {
                                 if (!probeNumberStr.IsEmpty())
                                 {
                                    probeNumberStr += ", ";
                                    plural = true;
                                 }

                                 probeNumberStr += botProbe->GetCCProbeData()->getInsert()->getRefname();
                              }
                              if (!probeNumberStr.IsEmpty())
                                 probeMsg.Format("Placed Probe%s: %s", plural ? "s" : "", probeNumberStr);
                           }

                           if (msg.IsEmpty())
                              msg = probeMsg;


                           m_file.WriteString("<TR>\n");
                           //m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"33%%\">%s</TD>\n", NonEmptyStr(""));  // SKIP 1st col
                           m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"23%%\">%s</TD>\n", NonEmptyStr(name));
                           m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TD>\n", NonEmptyStr(surface));
                           m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%s</TD>\n", NonEmptyStr(msg));
                           ///////m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(""));  //*rcf removed
                           m_file.WriteString("</TR>\n");
                        }
                     }
                  }
               }
            }
         }
         m_file.WriteString("</TABLE>\n");
      }


      {
         // Not Analyzed Nets

         m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"100%%\" COLSPAN=\"4\" >%s</TH>\n", "Nets Not Analyzed");
         m_file.WriteString("</TR>\n");

         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"34%%\">%s</TH>\n",                "Net Name");
         m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"33%%\" COLSPAN=\"2\">%s</TH>\n", "Probes Required");
         m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"33%%\">%s</TH>\n",               "Probes Placed");
         /////*rcf m_file.WriteString("<TH ALIGN=\"RIGHT\" WIDTH=\"20%%\">%s</TH>\n", "Potential DPMO");
         m_file.WriteString("</TR>\n");


         CString *netname;
         CTRNet *trnet = NULL;
         for (m_netMap.GetFirstSorted(netname, trnet); trnet != NULL; m_netMap.GetNextSorted(netname, trnet)) 
         {
            /////if (trnet->GetTestProbeStatus() != ProbeStatusFullyProbed && trnet->GetTestResourcesRequired() > 0)
            {
               CAANetAccess *netaccess = aasol->GetNetAccess(trnet->GetCamCadNet()->getNetName());

               if (netaccess == NULL)
               {
                  m_file.WriteString("<TR>\n");
                  m_file.WriteString("<TD ALIGN=\"LEFT\" VALIGN=\"TOP\" WIDTH=\"34%%\" ROWSPAN=\"%d\">%s</TD>\n", 1, NonEmptyStr(trnet->GetCamCadNet()->getNetName()));
                  m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"66%%\" COLSPAN=\"3\">%s</TD>\n", "Not present in Access Analysis Solution");
                  //m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"20%%\">%d</TD>\n", trnet->GetPlacedTestProbeCount());
                  //m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(dpmoStr));
                  m_file.WriteString("</TR>\n");
               }
               else if (!netaccess->IsNetAnalyzed())
               {
                  int rowcount = 0;

                  POSITION locpos = netaccess->GetHeadPosition_AccessibleLocations();
                  while (locpos != NULL)
                  {
                     CAAAccessLocation *accloc = netaccess->GetNext_AccessibleLocations(locpos);
                     rowcount++;
                  }

                  m_file.WriteString("<TR>\n");
                  m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"34%%\" ROWSPAN=\"%d\">%s</TD>\n", rowcount+1, NonEmptyStr(trnet->GetCamCadNet()->getNetName()));
                  m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"33%%\" COLSPAN=\"2\">%d</TD>\n", trnet->GetTestResourcesRequired());
                  m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"33%%\">%d</TD>\n",               trnet->GetPlacedTestProbeCount());
                  ////m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(dpmoStr));
                  m_file.WriteString("</TR>\n");

                  for (int pass = 0; pass < 2; pass++)
                  {
                     locpos = netaccess->GetHeadPosition_AccessibleLocations();
                     while (locpos != NULL)
                     {
                        CAAAccessLocation *accloc = netaccess->GetNext_AccessibleLocations(locpos);
                        bool accessible = accloc->GetAccessible();

                        if ((pass == 0 && !accessible) || (pass == 1 && accessible))
                        {
                           CString surface("Unknown");
                           switch(accloc->GetAccessSurface())
                           {
                           case testSurfaceTop:
                              surface = "Top";
                              break;
                           case testSurfaceBottom:
                              surface = "Bottom";
                              break;
                           case testSurfaceBoth:
                              surface = "Both";
                              break;
                           }

                           CString refname = accloc->GetRefDes();
                           CString targettype = accloc->GetTargetTypeToString();
                           CString name;
                           name.Format("%s [%s] %sAccessible", refname, targettype, accessible ? "" : "NOT ");
                           CString errmsg = accloc->GetErrorMessage(",<BR>");

                           m_file.WriteString("<TR>\n");
                           //m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(""));  // SKIP 1st col
                           m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"23%%\">%s</TD>\n", NonEmptyStr(name));
                           m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TD>\n", NonEmptyStr(surface));
                           m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"33%%\">%s</TD>\n", NonEmptyStr(errmsg));
                           //////m_file.WriteString("<TD ALIGN=\"RIGHT\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(""));
                           m_file.WriteString("</TR>\n");
                        }
                     }
                  }
               }
            }
         }
         m_file.WriteString("</TABLE>\n");
      }
   }*/

}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteProbeSection(CString name, CString title)
{
   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   WriteProbeDensityViolations();
   WritePlacedProbes();
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WritePlacedProbes()
{
   m_topProbeMap.setSortFunction(CTRProbeMap::DescendingProbeSizeSortFunc);
   m_topProbeMap.Sort();

   m_botProbeMap.setSortFunction(CTRProbeMap::DescendingProbeSizeSortFunc);
   m_botProbeMap.Sort();

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"6\">%s</TH>\n",   "Placed Probes");
   m_file.WriteString("</TR>\n");

   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Probe Name");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Probe Size");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "PCB Side");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "X");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Y");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TH>\n", "Net Name");
   m_file.WriteString("</TR>\n");

   CString *probename;
   CTRProbe *trprobe = NULL;
   for (m_topProbeMap.GetFirstSorted(probename, trprobe); trprobe != NULL; m_topProbeMap.GetNextSorted(probename, trprobe)) 
   {
      if (trprobe->IsPlaced())
      {
         InsertStruct *insert = trprobe->GetCCProbeData()->getInsert();

         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", NonEmptyStr(insert->getRefname()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", NonEmptyStr(trprobe->GetProbeSizeStr()) );
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", "Top");
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", FormatDecimal(insert->getOriginX(), m_decimals));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", FormatDecimal(insert->getOriginY(), m_decimals));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", NonEmptyStr(trprobe->GetNetName()));
         m_file.WriteString("</TR>\n");
      }
   }

   for (m_botProbeMap.GetFirstSorted(probename, trprobe); trprobe != NULL; m_botProbeMap.GetNextSorted(probename, trprobe)) 
   {
      if (trprobe->IsPlaced())
      {
         InsertStruct *insert = trprobe->GetCCProbeData()->getInsert();

         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", NonEmptyStr(insert->getRefname()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", NonEmptyStr(trprobe->GetProbeSizeStr()) );
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", "Bottom");
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", FormatDecimal(insert->getOriginX(), m_decimals));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", FormatDecimal(insert->getOriginY(), m_decimals));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"16%%\">%s</TD>\n", NonEmptyStr(trprobe->GetNetName()));
         m_file.WriteString("</TR>\n");
      }
   }

   m_file.WriteString("</TABLE>\n");
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WriteProbeDensityViolations()
{
   // Reported only if violations exist

   if (this->m_topDensityViolationCount > 0 || this->m_botDensityViolationCount > 0)
   {
      m_topProbeMap.setSortFunction(CTRProbeMap::DescendingDensitySortFunc);
      m_topProbeMap.Sort();

      m_botProbeMap.setSortFunction(CTRProbeMap::DescendingDensitySortFunc);
      m_botProbeMap.Sort();

      m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"CENTER\" COLSPAN=\"5\">%s</TH>\n",   "Probe Density Violations");
      m_file.WriteString("</TR>\n");

      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Probe Name");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "PCB Side");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "X");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Y");
      m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Density In Vicinity");
      m_file.WriteString("</TR>\n");

      CString *probename;
      CTRProbe *trprobe = NULL;
      for (m_topProbeMap.GetFirstSorted(probename, trprobe); trprobe != NULL; m_topProbeMap.GetNextSorted(probename, trprobe)) 
      {
         if (trprobe->IsDensityCenter())
         {
            InsertStruct *insert = trprobe->GetCCProbeData()->getInsert();

            m_file.WriteString("<TR>\n");
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(insert->getRefname()));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", "Top");
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", FormatDecimal(insert->getOriginX(), m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", FormatDecimal(insert->getOriginY(), m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%d</TD>\n", trprobe->GetAllNeighborCount() + 1);  // plus 1 is for self
            m_file.WriteString("</TR>\n");
         }
      }

      for (m_botProbeMap.GetFirstSorted(probename, trprobe); trprobe != NULL; m_botProbeMap.GetNextSorted(probename, trprobe)) 
      {
         if (trprobe->IsDensityCenter())
         {
            InsertStruct *insert = trprobe->GetCCProbeData()->getInsert();

            m_file.WriteString("<TR>\n");
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", NonEmptyStr(insert->getRefname()));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", "Bottom");
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", FormatDecimal(insert->getOriginX(), m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", FormatDecimal(insert->getOriginY(), m_decimals));
            m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%d</TD>\n", trprobe->GetAllNeighborCount() + 1);  // plus 1 is for self
            m_file.WriteString("</TR>\n");
         }
      }

      m_file.WriteString("</TABLE>\n");

   }
}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WritePowerSection(CString name, CString title)
{
   m_netMap.setSortFunction(CTRNetMap::PowerProbeStatusSortFunc);
   m_netMap.Sort();

   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");
   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TH>\n",   "Net Name");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Net Type");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Probe Status");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Required Power Resources");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TH>\n", "Placed Power Probes");
   m_file.WriteString("</TR>\n");

   CString *netname;
   CTRNet *trnet = NULL;
   for (m_netMap.GetFirstSorted(netname, trnet); trnet != NULL; m_netMap.GetNextSorted(netname, trnet)) 
	{
      if (trnet->GetNetType() == NetTypeGround || trnet->GetNetType() == NetTypePower || trnet->GetPowerResourcesRequired() > 0)
      {
         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"20%%\">%s</TD>\n", netname? *netname : "NULL");
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", trnet->GetNetTypeStr());
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%s</TD>\n", trnet->GetPowerProbeStatusStr());
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%d</TD>\n", trnet->GetPowerResourcesRequired());
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"20%%\">%d</TD>\n", trnet->GetPlacedPowerProbeCount());
         m_file.WriteString("</TR>\n");
      }
   }
   m_file.WriteString("</TABLE>\n");

}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::WritePartSection(CString name, CString title)
{
   m_partMap.setSortFunction(CTRPartMap::AscendingPartNumberSortFunc);
   m_partMap.Sort();

   m_file.WriteString("<H2><A NAME=\"%s\">%s</A></H2>\n", name, title);

   m_file.WriteString("<TABLE BORDER=\"1\" WIDTH=\"90%%\">\n");
   m_file.WriteString("<TR>\n");
   m_file.WriteString("<TH ALIGN=\"LEFT\" WIDTH=\"10%%\">%s</TH>\n",   "Part Number");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"17%%\">%s</TH>\n", "Ref Des");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"7%%\">%s</TH>\n", "Count");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"17%%\">%s</TH>\n", "Description");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TH>\n", "Device Type");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"7%%\">%s</TH>\n", "Value");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"7%%\">%s</TH>\n", "+Tol");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"7%%\">%s</TH>\n", "-Tol");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TH>\n", "Subclass");
   m_file.WriteString("<TH ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TH>\n", "Pin Count");
   m_file.WriteString("</TR>\n");

   CTRSmartRefdesList notFittedRefdesList;

   CString *partnumber;
   CTRPart *trpart;
   for (m_partMap.GetFirstSorted(partnumber, trpart); trpart != NULL; m_partMap.GetNextSorted(partnumber, trpart)) 
	{
      bool atleastOneLoadedComponent = false;

      CTRComponent *sampleComp = trpart->GetCompAt(0);

      CTRSmartRefdesList refdesList;
      for (int i = 0; i < trpart->GetCompCount(); i++)
      {
         CTRComponent *trcomp = trpart->GetCompAt(i);
         //Skip Die for part list because 
         // Data Doctor does not support
         if(trcomp && !trcomp->IsDie())
         {
            if (trcomp->GetLoaded())
            {
               atleastOneLoadedComponent = true;
               refdesList.AddRefname( trcomp->GetComponentData()->getInsert()->getRefname() );
            }
            else
            {
               notFittedRefdesList.AddRefname( trcomp->GetComponentData()->getInsert()->getRefname() );
            }
         }
      }
      CString compressedRefnames = refdesList.GetCompressedRefdes();

      if (atleastOneLoadedComponent)
      {
         m_file.WriteString("<TR>\n");
         m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"10%%\">%s</TD>\n",   NonEmptyStr(trpart->GetPartNumber()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"17%%\">%s</TD>\n", NonEmptyStr(compressedRefnames));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"7%%\">%d</TD>\n",  refdesList.GetCount()); // count of Loaded comps
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"17%%\">%s</TD>\n", NonEmptyStr(sampleComp->GetDescription()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TD>\n", NonEmptyStr(deviceTypeTagToFriendlyString(sampleComp->GetDeviceType())));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"7%%\">%s</TD>\n",  NonEmptyStr(sampleComp->GetValue()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"7%%\">%s</TD>\n",  NonEmptyStr(sampleComp->GetPlusToleranceStr()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"7%%\">%s</TD>\n",  NonEmptyStr(sampleComp->GetMinusToleranceStr()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"10%%\">%s</TD>\n", NonEmptyStr(sampleComp->GetSubclass()));
         m_file.WriteString("<TD ALIGN=\"CENTER\" WIDTH=\"10%%\">%d</TD>\n", sampleComp->GetPinCount());
         m_file.WriteString("</TR>\n");
      }
   }

   if ( ! notFittedRefdesList.IsEmpty())
   {
      CString compressedRefnames = notFittedRefdesList.GetCompressedRefdes();
      m_file.WriteString("<TR>\n");
      m_file.WriteString("<TD ALIGN=\"LEFT\" WIDTH=\"10%%\">%s</TD>\n",   "NOT FITTED");
      m_file.WriteString("<TD ALIGN=\"CENTER\" COLSPAN=\"8\">%s</TD>\n", NonEmptyStr(compressedRefnames));
      m_file.WriteString("</TR>\n");
   }

   m_file.WriteString("</TABLE>\n");

}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::Analyze()
{
   //happens elsewhere now:    m_visibleFile = m_doc.getCamCadData().getSingleVisiblePcb();
   /*if (m_visibleFile == NULL)
      return; // can't make report without file to report on
   CDFTSolution *dftSolution = (m_visibleFile != NULL) ? m_doc.GetCurrentDFTSolution(*m_visibleFile) : NULL;

   m_componentMap.CollectComponents(m_doc.getCamCadData(), m_visibleFile, GetSettings());
   m_netMap.CollectNets(m_doc.getCamCadData(), m_visibleFile, dftSolution);
   m_partMap.CollectParts(m_doc.getCamCadData(), m_componentMap);
   m_topProbeMap.CollectProbes(m_doc.getCamCadData(), m_visibleFile, true);
   m_botProbeMap.CollectProbes(m_doc.getCamCadData(), m_visibleFile, false);

   bool kelvinUseResistors = false;
   bool kelvinUseCapacitors = false;
   bool kelvinUseInductors = false;
   double kelvinResistorValue = 0.; // Ohms
   double kelvinCapacitorValue = 0.; // uF

   if (m_visibleFile != NULL)
   {
      //*rcf CDFTSolution *dftSolution = m_doc.GetCurrentDFTSolution(*m_visibleFile); // can't get this from CCamCadData, too bad
      //CProbePlacementSolution *ppSolution = dftSolution ? dftSolution->GetProbePlacementSolution() : NULL;
      m_testPlan = dftSolution ? dftSolution->GetTestPlan() : NULL;
      if (m_testPlan != NULL)
      {
         kelvinUseResistors = m_testPlan->GetUseResistor();
         kelvinUseCapacitors = m_testPlan->GetUseCapacitor();
         kelvinUseInductors = m_testPlan->GetUseInductor();
         kelvinResistorValue = m_testPlan->GetResistorValue();
         kelvinCapacitorValue = m_testPlan->GetCapacitorValue();
      }

      CalculatePowerInjectionRequirements();
   }


   if (m_visibleFile != NULL && m_visibleFile->getBlock() != NULL)
   {
      // Board Size
      // This is patterned after "manufacturing.csv" report, from Report.cpp.
      // Both places have the flaw that if the outline is not found it should use extents of what is on the BOARD OUTLINE layer.
      // There should be a new PCBUtil func made that applies all three rules in succession:
      // 1. Try graphic class board outline
      // 2. Try items on layer type Board Outline
      // 3. Last ditch, use extents of pcb file block.
      double xmin, ymin, xmax, ymax;
      if (GetBoardSize(&m_doc, &m_visibleFile->getBlock()->getDataList(), &xmin, &ymin, &xmax, &ymax))
      {
         m_boardOutlineFound = true;
         m_boardSizeX = xmax - xmin;
         m_boardSizeY = ymax - ymin;
      }
		else
		{
         m_boardOutlineFound = true; // an outright lie
			m_doc.CalcBlockExtents(m_visibleFile->getBlock());
			m_boardSizeX = m_visibleFile->getBlock()->getXmax() - m_visibleFile->getBlock()->getXmin();
			m_boardSizeY = m_visibleFile->getBlock()->getYmax() - m_visibleFile->getBlock()->getYmin();
		}

      // Net Stats
      m_singlePinNetCount = 0;
      m_multiPinNetCount = 0;
      m_noPinNetCount = 0;
      m_fullyProbedNetCount = 0;
      m_partiallyProbedNetCount = 0;
      m_notProbedNetCount = 0;

      POSITION trnetpos = m_netMap.GetStartPosition();
      while (trnetpos != NULL)
      {
         CString netname;
         CTRNet *trnet = NULL;
         m_netMap.GetNextAssoc(trnetpos, netname, trnet);

         if (trnet != NULL)
         {
            NetStruct *ccnet = trnet->GetCamCadNet();
            if (ccnet != NULL)
            {
               if (ccnet->getCompPinCount() == 1)
                  m_singlePinNetCount++;
               else if (ccnet->getCompPinCount() > 1)
                  m_multiPinNetCount++;
               else
                  m_noPinNetCount++;
            }

            trnet->CalculatePotentialDPMO(m_componentMap);

            switch (trnet->GetTestProbeStatus())
            {
            case ProbeStatusFullyProbed:
               m_fullyProbedNetCount++;
               break;

            case ProbeStatusPartiallyProbed:
               m_partiallyProbedNetCount++;
               break;

            case ProbeStatusNotProbed:
               m_notProbedNetCount++;
               break;

            case ProbeStatusUnknown:
            default:
               break;
            }
         }
      }


      // Component Stats
      m_fittedCompCount = 0;
      m_notFittedCompCount = 0;
      m_topHeightViolationCount = 0;
      m_botHeightViolationCount = 0;
      m_fullyProbedCompCount = 0;
      m_partiallyProbedCompCount = 0;
      m_notProbedCompCount = 0;

      ComponentValues resCompVal(kelvinResistorValue, "O");
      // THIS CONSTUCTOR DOES NOT WORK!!!  ComponentValues capCompVal(kelvinCapacitorValue, "uF"); // value comes in as uF
      ComponentValues capCompVal(valueUnitFarad);
      CString capValStr;
      capValStr.Format("%fuF", kelvinCapacitorValue);
      capCompVal.SetValue(capValStr);

      double topHeightThreshold = GetSettings().GetMaxHeightTop(m_doc.getPageUnits());
      double botHeightThreshold = GetSettings().GetMaxHeightBot(m_doc.getPageUnits());

      CString *ignoredKey;
      CTRComponent *trcomp = NULL;
      for (this->m_componentMap.GetFirstSorted(ignoredKey, trcomp); trcomp != NULL; this->m_componentMap.GetNextSorted(ignoredKey, trcomp)) 
      {
         // Fitted Count
         if (trcomp->GetLoaded())
            m_fittedCompCount++;
         else
            m_notFittedCompCount++;

         // Height Violation
         // An absent part can not violate height, regardless of part height.
         if (trcomp->GetLoaded())
         {
            if (trcomp->GetComponentData()->getInsert()->getPlacedTop())
            {
               if (topHeightThreshold >= 0. && trcomp->GetHeight() > topHeightThreshold)
                  m_topHeightViolationCount++;
            }
            else
            {
               if (botHeightThreshold >= 0. && trcomp->GetHeight() > botHeightThreshold)
                  m_botHeightViolationCount++;
            }
         }

         //*rcf probably a bug that all this is being applied to both Loaded and Not Loaded

         // Kelvin Test
         if (kelvinUseResistors && trcomp->GetDeviceType() == deviceTypeResistor)
         {
            ComponentValues compVal(trcomp->GetValue());
            if (compVal.IsUnitEqual(resCompVal))
            {
               if (compVal.CompareValueTo(resCompVal) <= 0)
                  trcomp->SetKelvin(true);
            }
         }
         else if (kelvinUseCapacitors && trcomp->GetDeviceType() == deviceTypeCapacitor)
         {
            //ComponentValues compVal(trcomp->GetValue());  // Does not work well for Farads,  F is ambiguous, Farads or Femto ?
            ComponentValues compVal(valueUnitFarad);
            compVal.SetValue(trcomp->GetValue());
            if (compVal.IsUnitEqual(capCompVal))
            {
               if (compVal.CompareValueTo(capCompVal) >= 0)
                  trcomp->SetKelvin(true);
            }
         }
         else if (kelvinUseInductors && trcomp->GetDeviceType() == deviceTypeInductor)
         {
            trcomp->SetKelvin(true);
         }

         int probesRequiredPerPin = 1;
         if (trcomp->GetKelvin())
            probesRequiredPerPin = 2;

         bool hasAllProbes = true; // assume all is well, prove false
         bool hasAtleastOneProbe = false; // assume none, prove at least one
         int fullyProbedPinCount = 0;
         int partiallyProbedPinCount = 0;

         POSITION comppinPos = trcomp->GetCompPinMap().GetStartPosition();
         while (comppinPos != NULL)
         {
            CString pinrefname;
            CTRCompPin *trcomppin;
            trcomp->GetCompPinMap().GetNextAssoc(comppinPos, pinrefname, trcomppin);

            if (!trcomppin->IsNoConnect())
            {
               if (trcomppin->GetNet() != NULL)
               {
                  CTRNet *trpinnet;
                  if (m_netMap.Lookup(trcomppin->GetNet()->getNetName(), trpinnet))
                  {
                     if (trcomp->GetKelvin())
                        trpinnet->SetKelvin(true);

                     int probecount = trpinnet->GetPlacedTestProbeCount();

                     if (probecount > 0)
                        hasAtleastOneProbe = true;
                     if (probecount < probesRequiredPerPin)
                        hasAllProbes = false;

                     if (probecount >= probesRequiredPerPin)
                        fullyProbedPinCount++;
                     else if (probecount > 0)
                        partiallyProbedPinCount++;
                  }
               }
            }
         }

         // Base DPMO from comp attribute, if not set default to settings
         int baseDPMO = trcomp->GetBaseDPMO();
         if (baseDPMO < 1)
            baseDPMO = GetSettings().GetDPMO();

         // Process DPMO is fixed, based on component
         // One baseDPMO for "body" and one for each pin (no-connect pins not included)
         int processDPMO = (1 + trcomp->GetPinCount()) * baseDPMO;
         int finalDPMO = 0;    // to be calculated

         if (trcomp->GetMergedStatus() == MergedStatusIgnored)
            finalDPMO = processDPMO;  // By definition, ignored components are not (can not be) tested
         else if (hasAllProbes)
            finalDPMO = 0;            // Fully probed and tested component considered "no risk" for our purposes
         else
         {
            // Partially tested components have various DPMO calc rules.
            // Current rules boil down to Kelvin being different, the rest are all the same.

            if (trcomp->GetKelvin())
            {
               if ((fullyProbedPinCount + partiallyProbedPinCount) == trcomp->GetPinCount())
               {
                  // All pins have at least one probe, DPMO depends on partially probed pins
                  // Use half body DPMO plus one baseDPMO for each partially probed pin
                  finalDPMO = (baseDPMO / 2) + (baseDPMO * partiallyProbedPinCount);
               }
               else
               {
                  // Some pins not probed, comp not tested, DPMO is full process DPMO
                  finalDPMO = processDPMO;
               }
            }
            else
            {
               // One DPMO for body plus one for each unprobed pin
               int unprobedPinCount = trcomp->GetPinCount() - (fullyProbedPinCount + partiallyProbedPinCount);
               if (unprobedPinCount < 1) // should never happen
               {
                  ErrorMessage("Unprobed pin count failure in Analyze()", "");
               }
               else
               {
                  finalDPMO = (1 + unprobedPinCount) * baseDPMO;
               }
            }
         }

         trcomp->SetProcessDPMO(processDPMO);
         trcomp->SetFinalDPMO(finalDPMO);

         if (hasAllProbes)
         {
            trcomp->SetProbeStatus(ProbeStatusFullyProbed);
            m_fullyProbedCompCount++;
         }
         else if (hasAtleastOneProbe)
         {
            trcomp->SetProbeStatus(ProbeStatusPartiallyProbed);
            m_partiallyProbedCompCount++;
         }
         else
         {
            trcomp->SetProbeStatus(ProbeStatusNotProbed);
            m_notProbedCompCount++;
         }
      }



      // Probe Density
      CTRProbeDensityTool densityTool(GetSettings(), m_doc.getPageUnits());
      //*rcf m_topDensityViolationCount = densityTool.GetViolationsTop   (m_doc.getCamCadData(), m_visibleFile);
      //*rcf m_botDensityViolationCount = densityTool.GetViolationsBottom(m_doc.getCamCadData(), m_visibleFile);
      m_topDensityViolationCount = densityTool.DetermineViolations(m_topProbeMap);
      m_botDensityViolationCount = densityTool.DetermineViolations(m_botProbeMap);



      // Layers
      m_electLayerCount = 0;
      for (int i = 0; i < m_doc.getMaxLayerIndex(); i++)
      {
         LayerStruct *layer = m_doc.getLayerAt(i);
         {
            if (layer != NULL && layer->getElectricalStackNumber() > 0)
               m_electLayerCount++;
         }
      }

   }*/

}

//---------------------------------------------------------------------------

void CTestabilityReportWriter::CalculatePowerInjectionRequirements()
{
   //*rcf I'm thinking this should move to net construction (some time later)

   if (m_testPlan != NULL && m_visibleFile != NULL)
   {
      int powerUsage = m_testPlan->GetPowerInjectionUsage();
      int powerValue = m_testPlan->GetPowerInjectionValue();

      if (powerUsage != 0) // No power injection
      {
         int newPowerInjectionValue = 0;

         if (powerUsage == 1)    // Probes per power rail
            newPowerInjectionValue = powerValue;
         else if (powerUsage == 3)     // Probes per Number of Nets per Power Rail
         {
            // default to one so we don't divide by zero
            if (powerValue == 0)
               powerValue = 1;
            newPowerInjectionValue = m_visibleFile->getNetCount() / powerValue;
         }

         POSITION pos = m_netMap.GetStartPosition();
         while (pos)
         {
            CString netname;
            CTRNet *trnet = NULL;
            m_netMap.GetNextAssoc(pos, netname, trnet);

            if (trnet->GetNetType() == NetTypePower || trnet->GetNetType() == NetTypeGround)
            {

               //if (ppNet->GetNoProbeFlag())  //*rcf general bug invlving UnProbed nets
               //continue;

               if (powerUsage == 2)    // Probes per Number of Connections per Power Rail
               {
                  NetStruct *ccnet = trnet->GetCamCadNet();
                  int connectionCount = ccnet->getCompPinCount();

                  // default to one so we don't divide by zero
                  if (powerValue == 0)
                     powerValue = 1;
                  newPowerInjectionValue = connectionCount / powerValue;
               }

               if (trnet->GetPowerResourcesRequired() < newPowerInjectionValue)
                  trnet->SetPowerResourcesReq(newPowerInjectionValue);

            }
         }
      }
   }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CTRCompPin::CTRCompPin(CCamCadData &ccdata, CString refname, CompPinStruct *comppin, NetStruct *net)
: m_refname(refname)
, m_comppin(comppin)
, m_net(net)
, m_noconnect(false)
{
   if (m_comppin != NULL)
   {
      int pinmapKWI  = ccdata.getAttributeKeywordIndex(standardAttributeDeviceToPackagePinMap);

      Attrib *attrib;
      if (m_comppin->lookUpAttrib(pinmapKWI, attrib))
         m_noconnect = (attrib->getStringValue().CompareNoCase(ATT_VALUE_NO_CONNECT) == 0);
   }
}

//---------------------------------------------------------------------------

int CTRCompPin::GetPlacedTestProbeCount(CTRNetMap &trnetMap)
{
   if (m_net != NULL)
   {
      CTRNet *trnet = NULL;
      if (trnetMap.Lookup(m_net->getNetName(), trnet))
      {
         return trnet->GetPlacedTestProbeCount();
      }
   }

   return 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool CTRCompPinMap::HasUnprobedPin(CTRNetMap &trnetMap)
{
   POSITION comppinPos = this->GetStartPosition();
   while (comppinPos != NULL)
   {
      CString pinrefname;
      CTRCompPin *trcomppin;
      this->GetNextAssoc(comppinPos, pinrefname, trcomppin);

      if (!trcomppin->IsNoConnect() && trcomppin->GetPlacedTestProbeCount(trnetMap) < 1)
         return true;
   }

   return false;
}

//---------------------------------------------------------------------------

CTRCompPin *CTRCompPinMap::GetPin(int n)
{
   // Get nth pin, not necessarily pin with refname n,  n starts at 1.
   // Skip No Connect pins.

   int counter = 0;

   POSITION comppinPos = this->GetStartPosition();
   while (comppinPos != NULL)
   {
      CString pinrefname;
      CTRCompPin *trcomppin;
      this->GetNextAssoc(comppinPos, pinrefname, trcomppin);

      if (!trcomppin->IsNoConnect())
         counter++;

      if (n == counter)
         return trcomppin;
   }

   return NULL;
}
//---------------------------------------------------------------------------

CString CTRCompPinMap::GetPinPairCombinedNetName(int n1, int n2)
{
   CTRCompPin *pin1 = this->GetPin(n1);
   CTRCompPin *pin2 = this->GetPin(n2);

   CString netname1("Not Found");
   CString netname2("Not Found");

   if (pin1 != NULL && pin1->GetNet() != NULL)
      netname1 = pin1->GetNetName();

   if (pin2 != NULL && pin2->GetNet() != NULL)
      netname2 = pin2->GetNetName();

   CString netpair;

   if (netname1.CompareNoCase(netname2) <= 0)
      netpair = netname1 + "|" + netname2;
   else
      netpair = netname2 + "|" + netname1;

   return netpair;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CTRComponent::CTRComponent(CCamCadData &ccdata, DataStruct *compdata, CTRSettings &settings)
: m_component(compdata)
, m_kelvin(false)
, m_loaded(true)
, m_height(-1.)
, m_devicetype(deviceTypeUnknown)
, m_plustolerance(0.)
, m_minustolerance(0.)
, m_dpmoBase(-1)
, m_dpmoProcess(0)
, m_dpmoFinal(0)
{
   if (m_component != NULL)
   {
      int valueKWI  = ccdata.getAttributeKeywordIndex(standardAttributeValue);
      int loadedKWI = ccdata.getAttributeKeywordIndex(standardAttributeLoaded);
      int devtypeKWI = ccdata.getAttributeKeywordIndex(standardAttributeDeviceType);
      int heightKWI = ccdata.getAttributeKeywordIndex(standardAttributeComponentHeight);
      int partnumKWI = ccdata.getAttributeKeywordIndex(standardAttributePartNumber);
      int plustolKWI = ccdata.getAttributeKeywordIndex(standardAttributePlusTolerance);
      int minustolKWI = ccdata.getAttributeKeywordIndex(standardAttributeMinusTolerance);
      int subclassKWI = ccdata.getAttributeKeywordIndex(standardAttributeSubclass);
      int descriptionKWI = ccdata.getAttributeKeywordIndex(standardAttributeDescription);
      int mergedstatusKWI = ccdata.getDefinedAttributeKeywordIndex("MergedStatus", valueTypeString);
      int mergedvalueKWI = ccdata.getDefinedAttributeKeywordIndex("MergedValue", valueTypeString);
      int dpmoKWI = ccdata.getDefinedAttributeKeywordIndex("DPMO", valueTypeInteger);

      Attrib *attrib;

      if (m_component->lookUpAttrib(valueKWI, attrib))
         m_value = attrib->getStringValue();

      if (m_component->lookUpAttrib(loadedKWI, attrib))
         if (attrib->getStringValue().CompareNoCase("false") == 0)
            m_loaded = false;

      if (m_component->lookUpAttrib(devtypeKWI, attrib))
         m_devicetype = stringToDeviceTypeTag(attrib->getStringValue());

      if (m_component->lookUpAttrib(heightKWI, attrib))
         m_height = attrib->getDoubleValue();

      if (m_component->lookUpAttrib(partnumKWI, attrib))
         m_partnumber = attrib->getStringValue();

      if (m_component->lookUpAttrib(subclassKWI, attrib))
         m_subclass = attrib->getStringValue();

      if (m_component->lookUpAttrib(descriptionKWI, attrib))
         m_description = attrib->getStringValue();

      if (m_component->lookUpAttrib(plustolKWI, attrib))
         m_plustolerance = attrib->getDoubleValue();

      if (m_component->lookUpAttrib(minustolKWI, attrib))
         m_minustolerance = attrib->getDoubleValue();

      if (m_component->lookUpAttrib(mergedstatusKWI, attrib))
         m_mergedStatus.Set(attrib->getStringValue());

      if (m_component->lookUpAttrib(mergedvalueKWI, attrib))
         m_mergedValue = attrib->getStringValue();

      if (m_component->lookUpAttrib(dpmoKWI, attrib))
         m_dpmoBase = attrib->getIntValue();
      else
         m_dpmoBase = settings.GetDPMO();


      int insertedBlkNum = m_component->getInsert()->getBlockNumber();
      BlockStruct *insertedBlk = ccdata.getBlock(insertedBlkNum);
      if (insertedBlk != NULL)
      {
         POSITION pos = insertedBlk->getHeadDataInsertPosition();
         while (pos != NULL)
         {
            DataStruct *pindata = insertedBlk->getNextDataInsert(pos);
            if (pindata->isInsertType(insertTypePin)
               || pindata->isInsertType(insertTypeDiePin))
            {
               NetStruct *pinnet = NULL;
               CompPinStruct *comppin = FindCompPin(ccdata.getSingleVisiblePcb(), m_component->getInsert()->getRefname(),
                  pindata->getInsert()->getRefname(), &pinnet);

               this->GetCompPinMap().SetAt(pindata->getInsert()->getRefname(), new CTRCompPin(ccdata, pindata->getInsert()->getRefname(), comppin, pinnet));
            }
         }
      }

   }
}

//---------------------------------------------------------------------------

int CTRComponent::GetPinCount()
{
   // Does not include no-connect pins

   int count = 0;

   POSITION pos = GetCompPinMap().GetStartPosition();
   while (pos != NULL)
   {
      CString pinrefname;
      CTRCompPin *trcomppin;
      GetCompPinMap().GetNextAssoc(pos, pinrefname, trcomppin);
      if (!trcomppin->IsNoConnect())
      {
         count++;
      }
   }

   return count;
}

//---------------------------------------------------------------------------

int CTRComponent::GetTheoreticalBestDPMO()
{
   // Theoretical best DPMO is the DPMO if all pins are probed.
   // If all pins are probed and the probes can test the part, then DPMO is zero.
   // But if component has merged state Ignored, then the component can't be
   // tested even if all pins are probed.
   // So theoretical best DPMO pretty much depends solely on merged status.

   if (this->GetMergedStatus() == MergedStatusIgnored)
      return ( (1 + this->GetPinCount()) * this->GetBaseDPMO() );

   return 0;
}

//---------------------------------------------------------------------------

CString CTRComponent::GetTestabilityReason(CTRNetMap &trnetMap)
{
   if (GetMergedStatus() == MergedStatusIgnored)
   {
      return "Parallel ignored component";
   }
   else if (GetKelvin() && GetProbeStatus() == ProbeStatusPartiallyProbed && !HasUnprobedPin(trnetMap))
   {
      return "Partially probed Kelvin component";
   }
   else if (GetProbeStatus() != ProbeStatusFullyProbed)
   {
      return "Insufficient probes placed";
   }

   // No reason
   return "";
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


void CTRComponentMap::CollectComponents(CCamCadData &ccdata, FileStruct *pcbFile, CTRSettings &settings)
{
   this->RemoveAll();
   m_CompCount = 0;
   m_DieCount = 0;

   if (pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);
         bool IsDie = false, IsComp = false;
         if(datum)
         {
            IsDie = datum->isInsertType(insertTypeDie) ;
            IsComp = datum->isInsertType(insertTypePcbComponent) ;
         }

         if (IsComp || IsDie)// || datum->isInsertType(insertTypeTestPoint)
         {
            CTRComponent *trcomp = new CTRComponent(ccdata, datum, settings);
            CString key( datum->getInsert()->getSortableRefDes() );
            this->SetAt(key, trcomp);
            
            if(IsDie) m_DieCount++;
            else if(IsComp) m_CompCount++;
         }
      }
   }
}

//---------------------------------------------------------------------

int CTRComponentMap::GetTotalProcessDPMO()
{
   int dpmo = 0;

   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      CString ignoredKey;
      CTRComponent *trcomp = NULL;
      this->GetNextAssoc(pos, ignoredKey, trcomp);

      if (trcomp->GetLoaded())
         dpmo += trcomp->GetProcessDPMO();
   }

   return dpmo;
}

//---------------------------------------------------------------------

int CTRComponentMap::GetTotalFinalDPMO()
{
   int dpmo = 0;

   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      CString ignoredKey;
      CTRComponent *trcomp = NULL;
      this->GetNextAssoc(pos, ignoredKey, trcomp);

      if (trcomp->GetLoaded())
         dpmo += trcomp->GetFinalDPMO();
   }

   return dpmo;
}

//---------------------------------------------------------------------

int CTRComponentMap::GetTotalTheoreticalBestDPMO()
{
   int dpmo = 0;

   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      CString ignoredKey;
      CTRComponent *trcomp = NULL;
      this->GetNextAssoc(pos, ignoredKey, trcomp);

      if (trcomp->GetLoaded())
         dpmo += trcomp->GetTheoreticalBestDPMO();
   }

   return dpmo;
}

//---------------------------------------------------------------------

int CTRComponentMap::AscendingValueSortFunc(const void *a, const void *b)
{
   CTRComponent* itemA = (CTRComponent*)(((SElement*) a )->pObject->m_object);
   CTRComponent* itemB = (CTRComponent*)(((SElement*) b )->pObject->m_object);

   double aval = itemA->GetValueDouble();
   double bval = itemB->GetValueDouble();

   if (aval < bval)
      return -1;
   else if (aval > bval)
      return 1;

   // Same values, use refname
   CString aref = itemA->GetComponentData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetComponentData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRComponentMap::AscendingHeightSortFunc(const void *a, const void *b)
{
   CTRComponent* itemA = (CTRComponent*)(((SElement*) a )->pObject->m_object);
   CTRComponent* itemB = (CTRComponent*)(((SElement*) b )->pObject->m_object);

   double aval = itemA->GetHeight();
   double bval = itemB->GetHeight();

   if (aval < bval)
      return -1;
   else if (aval > bval)
      return 1;

   // Same height, use refname
   CString aref = itemA->GetComponentData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetComponentData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRComponentMap::DescendingFinalDPMOSortFunc(const void *a, const void *b)
{
   CTRComponent* itemA = (CTRComponent*)(((SElement*) a )->pObject->m_object);
   CTRComponent* itemB = (CTRComponent*)(((SElement*) b )->pObject->m_object);

   int aval = itemA->GetFinalDPMO();
   int bval = itemB->GetFinalDPMO();

   if (aval > bval)
      return -1;
   else if (aval < bval)
      return 1;

   // Same final dpmo, use pin count
   aval = itemA->GetPinCount();
   bval = itemB->GetPinCount();

   if (aval > bval)
      return -1;
   else if (aval < bval)
      return 1;

   // Last ditch, use refname
   CString aref = itemA->GetComponentData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetComponentData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRComponentMap::DescendingHeightSortFunc(const void *a, const void *b)
{
   CTRComponent* itemA = (CTRComponent*)(((SElement*) a )->pObject->m_object);
   CTRComponent* itemB = (CTRComponent*)(((SElement*) b )->pObject->m_object);

   double aval = itemA->GetHeight();
   double bval = itemB->GetHeight();

   if (aval > bval)
      return -1;
   else if (aval < bval)
      return 1;

   // Same height, use refname
   CString aref = itemA->GetComponentData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetComponentData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRComponentMap::KelvinSectionSortFunc(const void *a, const void *b)
{
   CTRComponent* itemA = (CTRComponent*)(((SElement*) a )->pObject->m_object);
   CTRComponent* itemB = (CTRComponent*)(((SElement*) b )->pObject->m_object);

   ProbeStatusTag atag = itemA->GetProbeStatus();
   ProbeStatusTag btag = itemB->GetProbeStatus();

   if (atag != btag)
   {
      // Since they are not the same then just need to pick one with highest precedence
      if (atag == ProbeStatusNotProbed)
         return -1; // A goes first
      else if (btag == ProbeStatusNotProbed)
         return 1; // B goes first

      if (atag == ProbeStatusPartiallyProbed)
         return -1;
      else if (btag == ProbeStatusPartiallyProbed)
         return 1;

      if (atag == ProbeStatusFullyProbed)
         return -1;
      else if (btag == ProbeStatusFullyProbed)
         return 1;
   }

   // Same Kelvin tags, order by value
   //*rcf BUG caps should use decreasing order and res/ind use increasing value
   //*rcf don't think we can do that in one list.
   double aval = itemA->GetValueDouble();
   double bval = itemB->GetValueDouble();

   if (aval < bval)
      return -1;
   else if (aval > bval)
      return 1;

   // Same value, use refname
   CString aref = itemA->GetComponentData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetComponentData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRComponentMap::MergedStatusSortFunc(const void *a, const void *b)
{
   CTRComponent* itemA = (CTRComponent*)(((SElement*) a )->pObject->m_object);
   CTRComponent* itemB = (CTRComponent*)(((SElement*) b )->pObject->m_object);

   CString netpairA = itemA->GetCompPinMap().GetPinPairCombinedNetName(1, 2);
   CString netpairB = itemB->GetCompPinMap().GetPinPairCombinedNetName(1, 2);
   int netnamecomparison = netpairA.CompareNoCase(netpairB);
   if (netnamecomparison != 0)
      return netnamecomparison;

   MergedStatusTag ams = itemA->GetMergedStatus();
   MergedStatusTag bms = itemB->GetMergedStatus();

   if (ams != bms)
   {
      if (ams == MergedStatusPrimary)
         return -1;
      else if (bms == MergedStatusPrimary)
         return 1;

      if (ams == MergedStatusSolitary)
         return -1;
      else if (bms == MergedStatusSolitary)
         return 1;

      if (ams == MergedStatusIgnored)
         return -1;
      else if (bms == MergedStatusIgnored)
         return 1;

      // All that's left is both MergedStatusUnkown, and we
      // won't get here is both are Unknown.
   }

   // Same values, use refname
   CString aref = itemA->GetComponentData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetComponentData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

/////////////////////////////////////////////////////////////////////////////

CTRPart::CTRPart(CString partnumber)
: m_partNumber(partnumber)
{
}

//---------------------------------------------------------------------

void CTRPart::AddComponent(CTRComponent *trcomp)
{
   if (trcomp != NULL)
      m_componentAry.Add(trcomp);
}

//---------------------------------------------------------------------

void CTRPartMap::CollectParts(CCamCadData &ccdata, CTRComponentMap &compMap)
{
   this->RemoveAll();

   POSITION pos = compMap.GetStartPosition();
   while (pos != NULL)
   {
      CString ignoredKey;
      CTRComponent *trcomp;
      compMap.GetNextAssoc(pos, ignoredKey, trcomp);

      CTRPart *trpart = this->AddPart(trcomp);
   }
   
}
      
//---------------------------------------------------------------------

CTRPart *CTRPartMap::AddPart(CString partnumber)
{
   CTRPart *trpart = NULL;

   if (!partnumber.IsEmpty())
   {   
      Lookup(partnumber, trpart);
      if (trpart == NULL)
         this->SetAt(partnumber, (trpart = new CTRPart(partnumber)));
   }

   return trpart;
}

//---------------------------------------------------------------------

CTRPart *CTRPartMap::AddPart(CTRComponent *trcomp)
{
   CTRPart *trpart = NULL;

   if (trcomp != NULL)
   {
      trpart = AddPart( trcomp->GetPartNumber() );

      if (trpart != NULL)
         trpart->AddComponent(trcomp);
   }

   return trpart;
}

//---------------------------------------------------------------------

int CTRPartMap::AscendingPartNumberSortFunc(const void *a, const void *b)
{
   CTRPart* itemA = (CTRPart*)(((SElement*) a )->pObject->m_object);
   CTRPart* itemB = (CTRPart*)(((SElement*) b )->pObject->m_object);

   CString aref = itemA->GetPartNumber();
   CString bref = itemB->GetPartNumber();

   return (aref.CompareNoCase(bref));
}

/////////////////////////////////////////////////////////////////////////////

CTRNet::CTRNet(CCamCadData &ccdata, NetStruct *net, CAccessAnalysisSolution *aasol)
: m_ccnet(net)
, m_netType(NetTypeSignal)
, m_netaccess(NULL)
, m_testResourcesRequired(0)
, m_powerResourcesRequired(0)
, m_kelvinPlayer(false)
, m_placedTestProbeCount(0)
, m_placedPowerProbeCount(0)
, m_unplacedProbeCount(0)
, m_potentialDPMO(0)
{
   if (m_ccnet != NULL)
   {
      int nettypeKWI  = ccdata.getAttributeKeywordIndex(standardAttributeNetType);
      int tesresreqKWI  = ccdata.getAttributeKeywordIndex(standardAttributeTrRequired);
      int powresreqKWI  = ccdata.getAttributeKeywordIndex(standardAttributePirRequired);

      Attrib *attrib;

      if (m_ccnet->lookUpAttrib(nettypeKWI, attrib))
      {
         CString nettypeStr = attrib->getStringValue();
         if (nettypeStr.CompareNoCase("Power") == 0)
            m_netType = NetTypePower;
         else if (nettypeStr.CompareNoCase("Ground") == 0)
            m_netType = NetTypeGround;
         // else already NetTypeSignal
      }

      if (m_ccnet->lookUpAttrib(tesresreqKWI, attrib))
         m_testResourcesRequired = attrib->getIntValue();

      if (m_ccnet->lookUpAttrib(powresreqKWI, attrib))
         m_powerResourcesRequired = attrib->getIntValue();

      // There is an attrib on the net that would indicate if it plays in the
      // Kelvin game, but we disregard it. Since we have to re-create the Kelvin
      // component identification, we also re-set the Kelvin participation.
      // This happens during analysis.

      if (aasol != NULL)
      {
         m_netaccess = aasol->GetNetAccess( m_ccnet->getNetName() );
      }
   }
}

//---------------------------------------------------------------------------

CString CTRNet::GetNetTypeStr()
{
   switch (m_netType)
   {
   case NetTypeGround:
      return "Ground";
   case NetTypePower:
      return "Power";
   case NetTypeSignal:
      return "Signal";
   }

   return "Invalid";
}

//---------------------------------------------------------------------------

ProbeStatusTag CTRNet::GetTestProbeStatus()
{
   // If no probes then answer is easy
   if (m_placedTestProbeCount < 1)
      return ProbeStatusNotProbed;

   // At least one probe present, check against requirement
   int requiredTestProbes = 0;

   if (GetTestResourcesRequired() > requiredTestProbes)
      requiredTestProbes = GetTestResourcesRequired();  // Includes Kelvin consideration

   if (m_placedTestProbeCount >= requiredTestProbes)
      return ProbeStatusFullyProbed;

   // At least one but less than requirement
   return ProbeStatusPartiallyProbed;
}

//---------------------------------------------------------------------------

ProbeStatusTag CTRNet::GetPowerProbeStatus()
{
   // If no probes then answer is easy
   if (m_placedPowerProbeCount < 1)
      return ProbeStatusNotProbed;

   // At least one probe present, check against requirement
   int requiredPowerProbes = 0;

   if (GetPowerResourcesRequired() > 0)
      requiredPowerProbes = GetPowerResourcesRequired();

   if (m_placedPowerProbeCount >= requiredPowerProbes)
      return ProbeStatusFullyProbed;

   // At least one but less than requirement
   return ProbeStatusPartiallyProbed;
}

//---------------------------------------------------------------------------

ProbeStatusTag CTRNet::GetCombinedProbeStatus()
{
   CTRProbeStatus testpstatus( GetTestProbeStatus() );
   CTRProbeStatus powrpstatus( GetPowerProbeStatus() );

   // If no probes then answer is easy
   if (m_placedTestProbeCount < 1 && m_placedPowerProbeCount < 1)
      return ProbeStatusNotProbed;

   // At least one probe present, check against requirement
   int requiredTestProbes = 0;
   int requiredPowerProbes = 0;

   if (GetTestResourcesRequired() > 0)
      requiredTestProbes = GetTestResourcesRequired();

   if (GetPowerResourcesRequired() > 0)
      requiredPowerProbes = GetPowerResourcesRequired();

   if (m_placedTestProbeCount >= requiredTestProbes && m_placedPowerProbeCount >= requiredPowerProbes)
      return ProbeStatusFullyProbed;

   // At least one but less than requirement
   return ProbeStatusPartiallyProbed;
}

//---------------------------------------------------------------------------

CString CTRNet::GetTestProbeStatusStr()
{
   CTRProbeStatus probestatus( GetTestProbeStatus() );

   return probestatus.GetStr();
}

//---------------------------------------------------------------------------

CString CTRNet::GetPowerProbeStatusStr()
{
   CTRProbeStatus probestatus( GetPowerProbeStatus() );

   return probestatus.GetStr();
}

//---------------------------------------------------------------------------

CString CTRNet::GetCombinedProbeStatusStr()
{
   CTRProbeStatus probestatus( GetCombinedProbeStatus() );

   return probestatus.GetStr();
}

//---------------------------------------------------------------------------

int CTRNet::CalculatePotentialDPMO(CTRComponentMap &trCompMap)
{
   // Valid only after Component DPMOs have been determined.
   // I.e. after Analyze() has been run, or carefully placed within Analyze().

   m_potentialDPMO = 0;

   if (this->m_ccnet != NULL)
   {
      POSITION pos = m_ccnet->getHeadCompPinPosition();
      while (pos != NULL)
      {
         CompPinStruct *comppin = m_ccnet->getNextCompPin(pos);
         if (comppin != NULL)
         {
            CTRComponent *trcomp = NULL;
            if (trCompMap.Lookup(comppin->getSortableRefDes(), trcomp))
            {
               m_potentialDPMO += trcomp->GetBaseDPMO();
            }
            //else Error: No such component
         }
      }
   }

   return m_potentialDPMO;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CTRNetMap::CollectNets(CCamCadData &ccdata, FileStruct *pcbFile, CDFTSolution *dftSolution)
{
   this->RemoveAll();

   CAccessAnalysisSolution *aasol = NULL;
   if (dftSolution != NULL)
      aasol = dftSolution->GetAccessAnalysisSolution();

   // Collect the nets
   POSITION netpos = pcbFile->getHeadNetPosition();
   while (netpos != NULL)
   {
      NetStruct *net = pcbFile->getNextNet(netpos);
      if (net != NULL)
      {
         CTRNet *trnet = new CTRNet(ccdata, net, aasol);
         this->SetAt(net->getNetName(), trnet);
      }
   }

   // Get probe counts
   if (pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
      int netnameKWI  = ccdata.getAttributeKeywordIndex(standardAttributeNetName);
      int placementKWI  = ccdata.getAttributeKeywordIndex(standardAttributeProbePlacement);
      int testresourceKWI  = ccdata.getAttributeKeywordIndex(standardAttributeTestResource);

	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypeTestProbe))
         {
            Attrib *attrib = NULL;

            bool placed = false;
            if (datum->lookUpAttrib(placementKWI, attrib))
               placed = (attrib->getStringValue().CompareNoCase("Placed") == 0);

            bool power = false;
            if (datum->lookUpAttrib(testresourceKWI, attrib))
               power = (attrib->getStringValue().CompareNoCase("Power Injection") == 0);

            if (datum->lookUpAttrib(netnameKWI, attrib))
            {
               CString netname = attrib->getStringValue();
               if (!netname.IsEmpty())
               {
                  CTRNet *trnet = NULL;
                  if (this->Lookup(netname, trnet))
                  {
                     if (!placed)
                        trnet->IncrementUnplacedProbeCount();
                     else if (power)
                        trnet->IncrementPlacedPowerProbeCount();
                     else
                        trnet->IncrementPlacedTestProbeCount();
                  }
               }
            }

         }
      }
   }

}

//---------------------------------------------------------------------

int CTRNetMap::AscendingNetNameSortFunc(const void *a, const void *b)
{
   CTRNet* itemA = (CTRNet*)(((SElement*) a )->pObject->m_object);
   CTRNet* itemB = (CTRNet*)(((SElement*) b )->pObject->m_object);

   CString aref = itemA->GetCamCadNet()->getNetName();
   CString bref = itemB->GetCamCadNet()->getNetName();

   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRNetMap::DescendingDPMOSortFunc(const void *a, const void *b)
{
   CTRNet* itemA = (CTRNet*)(((SElement*) a )->pObject->m_object);
   CTRNet* itemB = (CTRNet*)(((SElement*) b )->pObject->m_object);

   // Descending DPMO
   int dpmoA = itemA->GetPotentialDPMO();
   int dpmoB = itemB->GetPotentialDPMO();

   if (dpmoA > dpmoB)
      return -1;
   else if (dpmoB > dpmoA)
      return 1;

   // Ascending netname within same DPMO
   CString aref = itemA->GetCamCadNet()->getNetName();
   CString bref = itemB->GetCamCadNet()->getNetName();

   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRNetMap::NetDetailsSortFunc(const void *a, const void *b)
{
   /*
   Sorted into groups, in each group sort into descending DPMO values.
   For same DPMO, Ascending net name.
	   Unprobed Nets
		   Multi-pin Nets
		   Single pin Nets
		   Unconnected Nets
	   Partially Probed Nets
		   Multi-pin Nets
		   Single pin Nets
		   Unconnected Nets
	   Fully Probed Nets
		   Multi-pin Nets
		   Single pin Nets
		   Unconnected Nets
   */

   CTRNet* itemA = (CTRNet*)(((SElement*) a )->pObject->m_object);
   CTRNet* itemB = (CTRNet*)(((SElement*) b )->pObject->m_object);

   ProbeStatusTag probeStatA = itemA->GetTestProbeStatus();
   ProbeStatusTag probeStatB = itemB->GetTestProbeStatus();

   if (probeStatA != probeStatB)
   {
      if (probeStatA == ProbeStatusNotProbed)
         return -1;
      else if (probeStatB == ProbeStatusNotProbed)
         return 1;

      if (probeStatA == ProbeStatusPartiallyProbed)
         return -1;
      else if (probeStatB == ProbeStatusPartiallyProbed)
         return 1;

      if (probeStatA == ProbeStatusFullyProbed)
         return -1;
      else if (probeStatB == ProbeStatusFullyProbed)
         return 1;
   }

   CAANetAccess *accessA = itemA->GetNetAccess();
   CAANetAccess *accessB = itemB->GetNetAccess();

   if (accessA != NULL && accessB == NULL)
      return -1;
   if (accessA == NULL && accessB != NULL)
      return 1;
   if (accessA != NULL && accessB != NULL)
   {
      EIncludedNetType nettypeA = accessA->GetIncludeNetType();
      EIncludedNetType nettypeB = accessB->GetIncludeNetType();

      if (nettypeA != nettypeB)
      {
         if (nettypeA == includeMultiPinNet)
            return -1;
         else if (nettypeB == includeMultiPinNet)
            return 1;

         if (nettypeA == includeSinglePinNet)
            return -1;
         else if (nettypeB == includeSinglePinNet)
            return 1;

         if (nettypeA == includeUnconnecedNet)
            return -1;
         else if (nettypeB == includeUnconnecedNet)
            return 1;
      }
   }

   // Descending DPMO
   CString aref = itemA->GetCamCadNet()->getNetName();
   CString bref = itemB->GetCamCadNet()->getNetName();

   int dpmoA = itemA->GetPotentialDPMO();
   int dpmoB = itemB->GetPotentialDPMO();

   if (dpmoA > dpmoB)
      return -1;
   else if (dpmoB > dpmoA)
      return 1;

   // Ascending netname within same DPMO
   return (aref.CompareNoCase(bref));
}

//---------------------------------------------------------------------

int CTRNetMap::TestProbeStatusSortFunc(const void *a, const void *b)
{
   return ProbeStatusSortFuncHelper(a, b, 1);
}

//---------------------------------------------------------------------

int CTRNetMap::PowerProbeStatusSortFunc(const void *a, const void *b)
{
   return ProbeStatusSortFuncHelper(a, b, 2);
}

//---------------------------------------------------------------------

int CTRNetMap::CombinedProbeStatusSortFunc(const void *a, const void *b)
{
   return ProbeStatusSortFuncHelper(a, b, 3);
}

//---------------------------------------------------------------------

int CTRNetMap::ProbeStatusSortFuncHelper(const void *a, const void *b, int probetype)
{
   // probetype:   1=test  2=power  3 = combined

   CTRNet* itemA = (CTRNet*)(((SElement*) a )->pObject->m_object);
   CTRNet* itemB = (CTRNet*)(((SElement*) b )->pObject->m_object);

   ProbeStatusTag atag;
   ProbeStatusTag btag;

   if (probetype == 1)
   {
      atag = itemA->GetTestProbeStatus();
      btag = itemB->GetTestProbeStatus();
   }
   else if (probetype == 2)
   {
      atag = itemA->GetPowerProbeStatus();
      btag = itemB->GetPowerProbeStatus();
   }
   else if (probetype == 3)
   {
      atag = itemA->GetCombinedProbeStatus();
      btag = itemB->GetCombinedProbeStatus();
   }
   else
   {
      return 0;
   }

   if (atag != btag)
   {
      // Since they are not the same then just need to pick one with highest precedence
      if (atag == ProbeStatusNotProbed)
         return -1; // A goes first
      else if (btag == ProbeStatusNotProbed)
         return 1; // B goes first

      if (atag == ProbeStatusPartiallyProbed)
         return -1;
      else if (btag == ProbeStatusPartiallyProbed)
         return 1;

      if (atag == ProbeStatusFullyProbed)
         return -1;
      else if (btag == ProbeStatusFullyProbed)
         return 1;
   }

   // Same Probe Status, order by net type
   NetTypeTag atype = itemA->GetNetType();
   NetTypeTag btype = itemB->GetNetType();

   if (atype != btype)
   {
      if (atype == NetTypeGround)
         return -1;
      else if (btype == NetTypeGround)
         return 1;

      if (atype == NetTypePower)
         return -1;
      else if (btype == NetTypePower)
         return 1;

      // The only thing left is signal, and they would have
      // to both be signal, so we wouldn't even get here.
   }

   // Same type, use net name
   CString aref = itemA->GetCamCadNet()->getNetName();
   CString bref = itemB->GetCamCadNet()->getNetName();

   return (aref.CompareNoCase(bref));
}

//***************************************************************************

CTRProbeStatus::CTRProbeStatus()
: m_tag(ProbeStatusUnknown)
{
}

//---------------------------------------------------------------------------

CString CTRProbeStatus::GetStr()
{
   CString stat("Invalid");

   switch (m_tag)
   {
   case ProbeStatusUnknown:
      stat = "Unknown";
      break;
   case ProbeStatusFullyProbed:
      stat = "Fully Probed";
      break;
   case ProbeStatusPartiallyProbed:
      stat = "Partially Probed";
      break;
   case ProbeStatusNotProbed:
      stat = "Not Probed";
      break;
   }

   return stat;
}

//***************************************************************************

CTRMergedStatus::CTRMergedStatus()
: m_tag(MergedStatusUnknown)
{
}

//---------------------------------------------------------------------------

CString CTRMergedStatus::GetStr()
{
   CString stat("Invalid");

   switch (m_tag)
   {
   case MergedStatusUnknown:
      stat = "Unknown";
      break;
   case MergedStatusPrimary:
      stat = "Primary";
      break;
   case MergedStatusSolitary:
      stat = "Solitary";
      break;
   case MergedStatusIgnored:
      stat = "Ignored";
      break;
   }

   return stat;
}

//---------------------------------------------------------------------------

void CTRMergedStatus::Set(CString mstat)
{
   if (mstat.CompareNoCase("Primary") == 0)
      m_tag = MergedStatusPrimary;
   else if (mstat.CompareNoCase("Solitary") == 0)
      m_tag = MergedStatusSolitary;
   else if (mstat.CompareNoCase("Ignored") == 0)
      m_tag = MergedStatusIgnored;
   else
      m_tag = MergedStatusUnknown;
}

//***************************************************************************

CTRSmartRefdes::CTRSmartRefdes(const CString& wholeRefdes)
{
   m_prefix = "";
   m_number = 0;

   if (!wholeRefdes.IsEmpty()) {
      m_prefix = wholeRefdes;
      
      // Work from the end backwards, so you don't get fooled
      // by the likes of R3_3-R3_5.

      char c;
      int powerOfTenFactor = 1;
      int i = wholeRefdes.GetLength() - 1;
      while (i >= 0 && isdigit(c = wholeRefdes.GetAt(i)))
      {
         m_number = m_number + ((c - '0') * powerOfTenFactor);
         m_prefix.Truncate(i);
         i--;
         powerOfTenFactor *= 10;
      }
   }
}

//---------------------------------------------------------------------

int CTRSmartRefdes::CompareNoCase(CTRSmartRefdes &otherrefdes)
{
   // First compare prefixes, i.e. the "C" in "C10"
   CString thisPrefix = m_prefix;
   CString otherPrefix = otherrefdes.GetPrefix();

   int prefixComparison = thisPrefix.MakeUpper().Compare(otherPrefix.MakeUpper());

   if (prefixComparison != 0)
      return prefixComparison;

   // Same prefixes, go by the number
   return (m_number - otherrefdes.GetNumber());

}

//---------------------------------------------------------------------

void CTRSmartRefdesList::AddRefname(CString refname)
{
   if (!refname.IsEmpty())
   {
      CTRSmartRefdes *smref = NULL;
      this->Lookup(refname, smref);
      if (smref == NULL)
         this->SetAt(refname, new CTRSmartRefdes(refname));
   }
}

//---------------------------------------------------------------------

int CTRSmartRefdesList::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CTRSmartRefdes* itemA = (CTRSmartRefdes*)(((SElement*) a )->pObject->m_object);
   CTRSmartRefdes* itemB = (CTRSmartRefdes*)(((SElement*) b )->pObject->m_object);

   return itemA->CompareNoCase(itemB);
}

//---------------------------------------------------------------------

CString CTRSmartRefdesList::GetCompressedRefdes()
{
   CString compressedRefnames;

   this->setSortFunction(CTRSmartRefdesList::AscendingRefnameSortFunc);
   this->Sort();

   CString *originalRefname;
   CTRSmartRefdes *smartRefname;
   CString anchorPrefix;
   int anchorNumber = 0;
   int prevNumber = 0;
   int count = 0;
   bool lastOne = false;
   for (this->GetFirstSorted(originalRefname, smartRefname); smartRefname != NULL; this->GetNextSorted(originalRefname, smartRefname))
   {
      count++;
      lastOne = (count == this->GetCount());

      CString curPrefix = smartRefname->GetPrefix();
      int curNumber = smartRefname->GetNumber();

      if (compressedRefnames.IsEmpty())
      {
         compressedRefnames.Format("%s%d", curPrefix, curNumber);
         anchorPrefix = curPrefix;
         anchorNumber = curNumber;
         prevNumber = curNumber;
      }
      else if ((anchorPrefix.CompareNoCase(curPrefix) != 0) || (curNumber > (prevNumber + 1)))
      {
         // Prefix changed, close section, start again
         CString closure;
         if (prevNumber > (anchorNumber + 1))
            closure.Format("-%s%d", anchorPrefix, prevNumber);
         else if (prevNumber > anchorNumber)
            closure.Format(", %s%d", anchorPrefix, prevNumber);
         // else if same number we don't want to add it again, leave closure blank
         compressedRefnames += closure;

         CString nextSegStart;
         nextSegStart.Format(", %s%d", curPrefix, curNumber);
         compressedRefnames += nextSegStart;

         anchorPrefix = curPrefix;
         anchorNumber = curNumber;
         prevNumber = curNumber;
      }
      else if (lastOne)
      {
         // If prefix had changed on this last one then if block above would have
         // caught it. So we only get here on last one if prefix is still the same
         // and increment was by 1.
         CString closure;
         if (curNumber > (anchorNumber + 1))
            closure.Format("-%s%d", anchorPrefix, curNumber);
         else if (curNumber > anchorNumber)
            closure.Format(", %s%d", anchorPrefix, curNumber);
         compressedRefnames += closure;
      }
      else // prefix did not change, is not last one, and increment is by 1
      {
         prevNumber = curNumber;
      }
   }

   return compressedRefnames;
}

//***************************************************************************
CTRProbe::CTRProbe(CCamCadData &ccdata, DataStruct *ccprobedata)
: m_probe(ccprobedata)
, m_placed(false)
, m_active(false)
, m_densitycenter(false)
, m_probesize(0)
, m_targetentity(0)
{
   // Activate if is a placed probe

   if (m_probe != NULL)
   {
      int placedKWI  = ccdata.getAttributeKeywordIndex(standardAttributeProbePlacement);
      int netnameKWI  = ccdata.getAttributeKeywordIndex(standardAttributeNetName);
      int datalinkKWI  = ccdata.getAttributeKeywordIndex(standardAttributeDataLink);

      Attrib *attrib;
      if (m_probe->lookUpAttrib(placedKWI, attrib))
         m_placed = (attrib->getStringValue().CompareNoCase("PLACED") == 0);

      if (m_probe->lookUpAttrib(netnameKWI, attrib))
         m_netname = attrib->getStringValue();

      m_active = m_placed;


      // This is crude, it is based on what Fixture Out does to get probe size.
      // Try to derive size from inserted block name. Block name started out as template name, which we
      // over-use as probe size (but it isn't size). It gets prefixed and suffixed and so try to
      // extract the number.
      m_probesize = 0;
      BlockStruct *insertedBlk = ccdata.getBlockAt( m_probe->getInsert()->getBlockNumber() );
      if (insertedBlk != NULL)
      {
         CString tmpname( insertedBlk->getName() );
         while (m_probesize == 0 && !tmpname.IsEmpty())
         {
            m_probesize = atoi( tmpname );
            tmpname.Delete(1);
         }
      }


      // Cache entity number of probed target, i.e. what the access marker is on.
      if (m_probe->lookUpAttrib(datalinkKWI, attrib))
      {
         int accessMarkerEntityNum = attrib->getIntValue();
         CEntity accessMarkerEntity = CEntity::findEntity(ccdata, accessMarkerEntityNum);
         if (accessMarkerEntity.getEntityType() == entityTypeData)
         {
            DataStruct *accessMarkerData = accessMarkerEntity.getData();
            if (accessMarkerData != NULL)
            {
               if (accessMarkerData->lookUpAttrib(datalinkKWI, attrib))
               {
                  m_targetentity = attrib->getIntValue();
               }
            }
         }
      }



   }
}

int CTRProbe::GetActiveNeighborCount()
{
   int count = 0;

   for (int i = 0; i < this->m_probesInProximity.GetCount(); i++)
   {
      CTRProbe *p = this->m_probesInProximity.GetAt(i);

      if (p->IsActive())
         count++;
   }

   return count;
}

//--------------------------------------------------------------------

void CTRProbe::DeactivateNeighborhood()
{
   // Deactivate self and neighbors

   this->SetActive(false);

   for (int i = 0; i < this->m_probesInProximity.GetCount(); i++)
   {
      CTRProbe *p = this->m_probesInProximity.GetAt(i);
      p->SetActive(false);
   }
}

//--------------------------------------------------------------------

void CTRProbeMap::CollectProbes(CCamCadData &ccdata, FileStruct *pcbFile, bool topSide)
{
   this->RemoveAll();

   if (pcbFile != NULL && pcbFile->getBlock() != NULL)
   {
	   POSITION pos = pcbFile->getBlock()->getHeadDataInsertPosition();
	   while (pos != NULL)
      {
         DataStruct *datum = pcbFile->getBlock()->getNextDataInsert(pos);

         if (datum->isInsertType(insertTypeTestProbe))
         {
            if ((topSide && datum->getInsert()->getPlacedTop()) ||
               (!topSide && datum->getInsert()->getPlacedBottom()))
            {
               CTRProbe *trprobe = new CTRProbe(ccdata, datum);
               this->SetAt(datum->getInsert()->getRefname(), trprobe);
            }
         }
      }
   }
}

//--------------------------------------------------------------------

CTRProbe *CTRProbeMap::GetProbeForTargetEntity(int targetEntityNum)
{
   POSITION subjectpos = this->GetStartPosition();
   while (subjectpos != NULL)
   {
      CString subjectrefname;
      CTRProbe *subject = NULL;
      this->GetNextAssoc(subjectpos, subjectrefname, subject);

      if (subject->IsPlaced() && subject->GetProbedTargetEntityNum() == targetEntityNum)
      {
         return subject;
      }
   }

   return NULL;
}

//--------------------------------------------------------------------

int CTRProbeMap::GetPlacedProbeCount()
{
   int placedCount = 0;

   POSITION subjectpos = this->GetStartPosition();
   while (subjectpos != NULL)
   {
      CString subjectrefname;
      CTRProbe *subject = NULL;
      this->GetNextAssoc(subjectpos, subjectrefname, subject);

      if (subject->IsPlaced())
      {
         placedCount++;
      }
   }

   return placedCount;
}

//--------------------------------------------------------------------

int CTRProbeMap::GetUnplacedProbeCount()
{
   // If it isn't placed then it must be unplaced

   return (this->GetCount() - this->GetPlacedProbeCount());
}

//--------------------------------------------------------------------

CTRProbe *CTRProbeMap::GetOverCrowdedProbe(int maxNeighbors)
{
   // Pick lowest-leftest, this makes it work in vertical stripes from
   // left to right, avoids pulling a chunk out of the middle and leaving
   // semi-high density on the sides.

   // Actually just getting the left-est probe

   CTRProbe *candidate = NULL;

   POSITION subjectpos = this->GetStartPosition();
   while (subjectpos != NULL)
   {
      CString subjectrefname;
      CTRProbe *subject = NULL;
      this->GetNextAssoc(subjectpos, subjectrefname, subject);

      if (subject->IsActive() && (subject->GetActiveNeighborCount() + 1 /*for self*/) > maxNeighbors)
      {
         if (candidate == NULL)
         {
            candidate = subject;
         }
         else if (subject->GetActiveNeighborCount() > candidate->GetActiveNeighborCount())
         {
            candidate = subject;
         }
         else if ( (subject->GetActiveNeighborCount() == candidate->GetActiveNeighborCount()) &&
            (subject->GetCCProbeData()->getInsert()->getOriginX() < candidate->GetCCProbeData()->getInsert()->getOriginX()))
         {
            candidate = subject;
         }
      }

   }

   return candidate;
}

//--------------------------------------------------------------------

static int DescendingDensitySortFunc(const void *a, const void *b);
   static int DescendingProbeSizeSortFunc(const void *a, const void *b);

int CTRProbeMap::DescendingDensitySortFunc(const void *a, const void *b)
{
   CTRProbe* itemA = (CTRProbe*)(((SElement*) a )->pObject->m_object);
   CTRProbe* itemB = (CTRProbe*)(((SElement*) b )->pObject->m_object);

   int aval = itemA->GetAllNeighborCount();
   int bval = itemB->GetAllNeighborCount();

   if (aval > bval)
      return -1;
   else if (aval < bval)
      return  1;

   // Same values, use ascending refname
   CString aref = itemA->GetCCProbeData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetCCProbeData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

//--------------------------------------------------------------------

int CTRProbeMap::DescendingProbeSizeSortFunc(const void *a, const void *b)
{
   CTRProbe* itemA = (CTRProbe*)(((SElement*) a )->pObject->m_object);
   CTRProbe* itemB = (CTRProbe*)(((SElement*) b )->pObject->m_object);

   int aval = itemA->GetProbeSize();
   int bval = itemB->GetProbeSize();

   if (aval > bval)
      return -1;
   else if (aval < bval)
      return  1;

   // Same values, use ascending refname
   CString aref = itemA->GetCCProbeData()->getInsert()->getSortableRefDes();
   CString bref = itemB->GetCCProbeData()->getInsert()->getSortableRefDes();

   return (aref.CompareNoCase(bref));
}

//--------------------------------------------------------------------

CTRProbeDensityTool::CTRProbeDensityTool(CTRSettings &settings, PageUnitsTag currentPageUnits)
: m_settings(settings)
, m_currentPageUnits(currentPageUnits)
{
}

//--------------------------------------------------------------------


void CTRProbeDensityTool::CollectNeighbors(CTRProbeMap &probeMap)
{
   // Determine neighbors within designated area
   double areaBoundary = this->m_settings.GetAreaSideLength(m_currentPageUnits);
   double halfBoundary = areaBoundary / 2.;

   POSITION subjectpos = probeMap.GetStartPosition();
   while (subjectpos != NULL)
   {
      CString subjectrefname;
      CTRProbe *subject = NULL;
      probeMap.GetNextAssoc(subjectpos, subjectrefname, subject);

      subject->SetDensityCenter(false);

      if (!subject->IsPlaced())
      {
         subject->SetActive(false);
      }
      else
      {
         // Placed
         subject->SetActive(true);

         InsertStruct *subjectProbeInsert = subject->GetCCProbeData()->getInsert();
         CPoint2d boundaryLL( subjectProbeInsert->getOriginX() - halfBoundary,  subjectProbeInsert->getOriginY() - halfBoundary );
         CPoint2d boundaryUR( subjectProbeInsert->getOriginX() + halfBoundary,  subjectProbeInsert->getOriginY() + halfBoundary );

         POSITION intruderpos = probeMap.GetStartPosition();
         while (intruderpos != NULL)
         {
            CString intruderrefname;
            CTRProbe *intruder = NULL;
            probeMap.GetNextAssoc(intruderpos, intruderrefname, intruder);

            if (!intruder->IsPlaced())
            {
               intruder->SetActive(false);
            }
            else
            {
               intruder->SetActive(true);

               InsertStruct *intruderProbeInsert = intruder->GetCCProbeData()->getInsert();
               CPoint2d intruderLoc( intruderProbeInsert->getOrigin2d() );

               if (intruderLoc.x >= boundaryLL.x && intruderLoc.y > boundaryLL.y &&
                  intruderLoc.x < boundaryUR.x && intruderLoc.y < boundaryUR.y)
               {
                  subject->AddNeighbor(intruder);
               }
            }
         }
      }
   }

}


//--------------------------------------------------------------------

int CTRProbeDensityTool::DetermineViolations(CTRProbeMap &probeMap)
{
   int maxNeighbors = m_settings.GetMaxProbesPerSqr();

   int violationCount = 0;

   if (maxNeighbors > 0)
   {
      CollectNeighbors(probeMap);

      CTRProbe *menace = NULL;
      while ((menace = probeMap.GetOverCrowdedProbe(maxNeighbors)) != NULL)
      {
         violationCount++;
         menace->SetDensityCenter(true);
         menace->DeactivateNeighborhood();
      }
   }

   return violationCount;
}


//--------------------------------------------------------------------

int CTRProbeDensityTool::GetViolationsTop(CCamCadData &ccdata, FileStruct *pcbFile)
{
   // Collect probes from cc data
   CTRProbeMap probeMap;
   probeMap.CollectProbes(ccdata, pcbFile, true);

   return this->DetermineViolations(probeMap);
}

//--------------------------------------------------------------------

int CTRProbeDensityTool::GetViolationsBottom(CCamCadData &ccdata, FileStruct *pcbFile)
{
   // Collect probes from cc data
   CTRProbeMap probeMap;
   probeMap.CollectProbes(ccdata, pcbFile, false);

   return this->DetermineViolations(probeMap);
}
