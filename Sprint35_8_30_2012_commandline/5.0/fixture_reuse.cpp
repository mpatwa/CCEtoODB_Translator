// $Header: /CAMCAD/4.6/fixture_reuse.cpp 34    5/15/07 1:16p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "fixture_reuse.h"
#include "lic.h"
#include "crypt.h"
#include ".\fixture_reuse.h"
#include "MainFrm.h"
#include "Net_Util.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define ALIGNMENT_TOLERANCE_MILS 2.0

/******************************************************************************
* CCEtoODBDoc::OnDftanalysisFixturereuse
*/
void CCEtoODBDoc::OnDftanalysisFixturereuse()
{
	FileStruct *pcbFile = this->getFileList().GetOnlyShown(blockTypePcb);

	if (pcbFile)
	{
		/*if (!getApp().getCamcadLicense().isLicensed(camcadProductFixtureReuse)) 
		{
			ErrorAccess("Fixture Reuse Analysis is not licensed!");
			return;
		}*/

		CFixtureReuseDlg dlg(this);

		if (dlg.DoModal() == IDCANCEL)
			return;

		CString logFilePath = GetLogfilePath("FixtureReuse.log");

		CString topFixturePath = dlg.GetTopFixturePath();
		CString botFixturePath = dlg.GetBotFixturePath();

		// Make the dialog go away, Mark doesn't like it hanging around during long processing
		delete dlg;
		CWaitCursor hourglass;

		CFixtureReuse fixReuse(*this);
		fixReuse.ApplyFixtureFiles(topFixturePath, botFixturePath, logFilePath);

      fixReuse.MakeProbeRefnamesUnique();
		
		// Show the new probes in the drawing
		getActiveView()->GetDocument()->OnRedraw();
		
		// Show the new probes in the navigator
		getMainFrame()->getNavigator().UpdateProbes(this);
	}
	else
	{
		int pcbVisible =	this->getFileList().GetVisibleCount();

		if (pcbVisible < 1)
			ErrorMessage("No visible PCB file detected.\n\nFixture Reuse must be applied to a single visible PCB file.", "Fixture Reuse");
		else if (pcbVisible > 1)
			ErrorMessage("Multiple visible PCB files detected.\n\nFixture Reuse must be applied to a single visible PCB file.", "Fixture Reuse");
	}
}

/******************************************************************************
* CFixtureReuseDlg
*/
IMPLEMENT_DYNAMIC(CFixtureReuseDlg, CDialog)
CFixtureReuseDlg::CFixtureReuseDlg(CCEtoODBDoc *doc, CWnd* pParent /*=NULL*/)
	: CDialog(CFixtureReuseDlg::IDD, pParent)
	, m_doc(doc)
	, m_sTopFixturePath(_T(""))
	, m_sBotFixturePath(_T(""))
	, m_fileTypeSel(doc ? doc->getSettings().FixtureReuseFileType : 0)
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductFixtureReuse)) 
   {
      ErrorAccess("Fixture Reuse Analysis is not licensed!");
	  //CDialog::OnCancel();
	  return;
   }*/

}

CFixtureReuseDlg::~CFixtureReuseDlg()
{
}

void CFixtureReuseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TOP_FILE_EDIT, m_sTopFixturePath);
	DDX_Text(pDX, IDC_BOTTOM_FILE_EDIT, m_sBotFixturePath);
	DDX_Control(pDX, IDC_FIXTURE_TOP_LABEL, m_topFileLabel);
	DDX_Control(pDX, IDC_FIXTURE_BOTTOM_LABEL, m_botFileLabel);
	DDX_Radio(pDX, IDC_FIXTURE_TYPE_CAMCAD, m_fileTypeSel);
}


BEGIN_MESSAGE_MAP(CFixtureReuseDlg, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_TOP, OnBnClickedBrowseTop)
	ON_BN_CLICKED(IDC_BROWSE_BOTTOM, OnBnClickedBrowseBottom)
	ON_BN_CLICKED(IDC_FIXTURE_TYPE_CAMCAD, OnBnClickedFixtureTypeCamcad)
	ON_BN_CLICKED(IDC_FIXTURE_TYPE_AGILENT, OnBnClickedFixtureTypeAgilent)
   ON_BN_CLICKED(IDC_FIXTURE_TYPE_FABMASTER, OnBnClickedFixtureTypeFabmaster)
	ON_BN_CLICKED(IDC_FIXTURE_TYPE_TERADYNE_D2B, OnBnClickedFixtureTypeTeradyneD2B)
END_MESSAGE_MAP()


BOOL CFixtureReuseDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

	if (m_fileTypeSel == 3)
	{
		OnBnClickedFixtureTypeTeradyneD2B();
	}
   else if (m_fileTypeSel == 2)
   {
      OnBnClickedFixtureTypeFabmaster();
   }
	else if (m_fileTypeSel == 1)
	{
		OnBnClickedFixtureTypeAgilent();
	}
	else
	{
		OnBnClickedFixtureTypeCamcad();
	}

	return TRUE;
}

void CFixtureReuseDlg::OnBnClickedBrowseTop()
{
	CString fileDesignator;
	if (m_fileTypeSel == 3)
		fileDesignator = "Teradyne D2B Top Fixture File (*.top)|*.top|All Files (*.*)|*.*||";
	else if (m_fileTypeSel == 2)
		fileDesignator = "Fabmaster Nail File (*.asc)|*.asc|All Files (*.*)|*.*||";
	else if (m_fileTypeSel == 1)
		fileDesignator = "Agilent Fixture File (*.o)|*.o|All Files (*.*)|*.*||";
	else
		fileDesignator = "CAMCAD Top Fixture File (*.top)|*.top|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, NULL, fileDesignator, this);
	if (dlg.DoModal() == IDCANCEL)
		return;

	m_sTopFixturePath = dlg.GetPathName();
	UpdateData(FALSE);
}

void CFixtureReuseDlg::OnBnClickedBrowseBottom()
{
	CString fileDesignator;
	if (m_fileTypeSel == 3)
		fileDesignator = "Teradyne D2B Bottom Fixture File (*.bot)|*.bot|All Files (*.*)|*.*||";
	else if (m_fileTypeSel == 2)
		fileDesignator = "Fabmaster Nail File (*.asc)|*.asc|All Files (*.*)|*.*||";
	else if (m_fileTypeSel == 1)
		fileDesignator = "Agilent Fixture File (*.o)|*.o|All Files (*.*)|*.*||";
	else
		fileDesignator = "CAMCAD Bottom Fixture File (*.bot)|*.bot|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, NULL, fileDesignator, this);	
	if (dlg.DoModal() == IDCANCEL)
		return;

	m_sBotFixturePath = dlg.GetPathName();
	UpdateData(FALSE);
}

void CFixtureReuseDlg::OnOK()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductFixtureReuse)) 
	{
      ErrorAccess("You do not have a License for Fixture Reuse Analysis!");
      return;
    }*/
	UpdateData();

	m_doc->getSettings().FixtureReuseFileType = this->m_fileTypeSel;
	
	if (m_sTopFixturePath.IsEmpty() && m_sBotFixturePath.IsEmpty())
	{
		ErrorMessage("You must specify atleast one surface.", "Fixture Reuse");
		return;
	}

	CDialog::OnOK();
}


void CFixtureReuseDlg::OnBnClickedFixtureTypeCamcad()
{
	// Set file name labels
	m_topFileLabel.SetWindowText("CAMCAD Top Fixture File");
	m_botFileLabel.SetWindowText("CAMCAD Bottom Fixture File");
	
	// Make sure bottom chooser is active
	GetDlgItem(IDC_FIXTURE_BOTTOM_LABEL)->EnableWindow(1);
	GetDlgItem(IDC_BOTTOM_FILE_EDIT)->EnableWindow(1);
	GetDlgItem(IDC_BROWSE_BOTTOM)->EnableWindow(1);

	UpdateData();
}

void CFixtureReuseDlg::OnBnClickedFixtureTypeAgilent()
{
	// Set file name labels
	m_topFileLabel.SetWindowText("Agilent Fixture File (Top, Bottom, or Both)");
	m_botFileLabel.SetWindowText("Not Used");

	// Disable bottom side, Agilent files are double sided, uses one chooser box
	GetDlgItem(IDC_FIXTURE_BOTTOM_LABEL)->EnableWindow(0);
	GetDlgItem(IDC_BOTTOM_FILE_EDIT)->EnableWindow(0);
	GetDlgItem(IDC_BROWSE_BOTTOM)->EnableWindow(0);

	UpdateData();
}

void CFixtureReuseDlg::OnBnClickedFixtureTypeFabmaster()
{
	// Set file name labels
	m_topFileLabel.SetWindowText("Fabmaster Nail File (Top, Bottom, or Both)");
	m_botFileLabel.SetWindowText("Not Used");

	// Disable bottom side, Fabmaster files are double sided, uses one chooser box
	GetDlgItem(IDC_FIXTURE_BOTTOM_LABEL)->EnableWindow(0);
	GetDlgItem(IDC_BOTTOM_FILE_EDIT)->EnableWindow(0);
	GetDlgItem(IDC_BROWSE_BOTTOM)->EnableWindow(0);

	UpdateData();
}

void CFixtureReuseDlg::OnBnClickedFixtureTypeTeradyneD2B()
{
	// Set file name labels
	m_topFileLabel.SetWindowText("Teradyne D2B Top Fixture File");
	m_botFileLabel.SetWindowText("Teradyne D2B Bottom Fixture File");
	
	// Make sure bottom chooser is active
	GetDlgItem(IDC_FIXTURE_BOTTOM_LABEL)->EnableWindow(1);
	GetDlgItem(IDC_BOTTOM_FILE_EDIT)->EnableWindow(1);
	GetDlgItem(IDC_BROWSE_BOTTOM)->EnableWindow(1);

	UpdateData();
}

/******************************************************************************
* CFixtureReuseProbe::CFixtureReuseProbe
*/
CFixtureReuseProbe::CFixtureReuseProbe(CCEtoODBDoc *doc, ETestResourceType resType, CString netName)
	: CPPProbe(doc, resType, netName)
	, m_bPilotDrill(false)
{
}

CFixtureReuseProbe::CFixtureReuseProbe(CFixtureReuseProbe &probe)
	: CPPProbe((CFixtureReuseProbe &)probe)
{
	if (&probe != this)
	{
		m_bPilotDrill = probe.m_bPilotDrill;
	}
}

CFixtureReuseProbe::~CFixtureReuseProbe()
{
}

void CFixtureReuseProbe::DumpToFile(CFormatStdioFile &file, int indent)
{
	file.WriteString("%*s::Probe on %s at 0x%08x::\n", indent, " ", m_sNetName, this);

	indent += 3;
	file.WriteString("%*sm_eResourceType = %s Resource\n", indent, " ", (m_eResourceType==testResourceTypeTest)?"Test":"Power Injection");
	file.WriteString("%*sm_sNetName = %s\n", indent, " ", m_sNetName);
	file.WriteString("%*sm_pAccessibleLocation = 0x%08x\n", indent, " ", m_pAccessibleLocation);
	file.WriteString("%*sm_lProbeNumber = %s\n", indent, " ", m_sProbeRefname);
	file.WriteString("%*sm_bPlaced = %s\n", indent, " ", m_bPlaced?"True":"False");
	file.WriteString("%*sm_pProbe = %s\n", indent, " ", m_sProbeTemplateName.IsEmpty()?"Null":m_sProbeTemplateName);
	file.WriteString("%*sm_bExisting = %s\n", indent, " ", m_bExisting?"True":"False");
	file.WriteString("%*sm_bPilotDrill = %s\n", indent, " ", m_bPilotDrill?"True":"False");
	indent -= 3;
}

CFixtureReuseProbe& CFixtureReuseProbe::operator=(const CFixtureReuseProbe &probe)
{
	if (&probe != this)
	{
		m_pDoc = probe.m_pDoc;
		m_eResourceType = probe.m_eResourceType;
		m_sNetName = probe.m_sNetName;
		m_pAccessibleLocation = probe.m_pAccessibleLocation;
		m_sProbeRefname = probe.m_sProbeRefname;
		m_bPlaced = probe.m_bPlaced;
		m_sProbeTemplateName = probe.m_sProbeTemplateName;
		m_bExisting = probe.m_bExisting;
		m_bPilotDrill = probe.m_bPilotDrill;
	}

	return *this;
}



/******************************************************************************
* CFixtureReuseNet::CFixtureReuseNet
*/
CFixtureReuseNet::CFixtureReuseNet(CCEtoODBDoc *doc, CString netName)
	: CPPNet(doc, netName)
{
}

CFixtureReuseNet::CFixtureReuseNet(CFixtureReuseNet &ppNet)
	: CPPNet(ppNet)
{
}

CFixtureReuseNet::~CFixtureReuseNet()
{
}

CFixtureReuseProbe *CFixtureReuseNet::AddTail_Pilots(CString netName)
{
	CFixtureReuseProbe *probe = new CFixtureReuseProbe(m_pDoc, testResourceTypeTest, netName);
	probe->SetAsPilot(true);
	m_probes.AddTail(probe);

	return probe;
}



/******************************************************************************
* CFixtureReuseSolution::CFixtureReuseSolution
*/
CFixtureReuseSolution::CFixtureReuseSolution(CCEtoODBDoc *doc, CTestPlan *testPlan, FileStruct *file)
	: CProbePlacementSolution(doc, testPlan, file)
	, m_ccnetnames(file)
{
}

CFixtureReuseSolution::~CFixtureReuseSolution()
{
}

CPPAccessibleLocation *CFixtureReuseSolution::findAccessLocation(double x, double y)
{
   // Find on either side. Favors top is accLoc is on both sides

	CAccessibleLocationList foundList;
	CExtent ext(CPoint2d(x, y), ALIGNMENT_TOLERANCE_MILS * Units_Factor(pageUnitsMils, m_pDoc->getSettings().getPageUnits()));

	CPPAccessibleLocation *accLoc = NULL;
	if (m_LocationTreeTop.search(ext, foundList) > 0)
	{
		CQfeAccessibleLocation *qfeAccLoc = foundList.GetHead();
		accLoc = qfeAccLoc->GetAccessibleLocation();
	}
	else if(m_LocationTreeBot.search(ext, foundList) > 0)
	{
		CQfeAccessibleLocation *qfeAccLoc = foundList.GetHead();
		accLoc = qfeAccLoc->GetAccessibleLocation();
	}

	return accLoc;
}

CPPAccessibleLocation *CFixtureReuseSolution::findAccessLocation(double x, double y, ETestSurface surface)
{
   // Find on side designated by surface only, ignore one potentially on other side at same location

	CAccessibleLocationList foundList;
	CExtent ext(CPoint2d(x, y), ALIGNMENT_TOLERANCE_MILS * Units_Factor(pageUnitsMils, m_pDoc->getSettings().getPageUnits()));

	CPPAccessibleLocation *accLoc = NULL;
	if (surface == testSurfaceTop && m_LocationTreeTop.search(ext, foundList) > 0)
	{
		CQfeAccessibleLocation *qfeAccLoc = foundList.GetHead();
		accLoc = qfeAccLoc->GetAccessibleLocation();
	}

   if(surface == testSurfaceBottom && m_LocationTreeBot.search(ext, foundList) > 0)
	{
		CQfeAccessibleLocation *qfeAccLoc = foundList.GetHead();
		accLoc = qfeAccLoc->GetAccessibleLocation();
	}

	return accLoc;
}

void CFixtureReuseSolution::addAllNets()
{
	RemoveAll_Nets();

	//////////////////////////////////////////////////
	// Gather nets
	POSITION pos = m_pFile->getNetList().GetHeadPosition();
	while (pos)
	{
		NetStruct *net = m_pFile->getNetList().GetNext(pos);

		CFixtureReuseNet *ppNet = NULL;
		if (m_ppNets.Lookup(net->getNetName(), (CPPNet*&)ppNet))
			continue;

		m_ppNets.SetAt(net->getNetName(), new CFixtureReuseNet(m_pDoc, net->getNetName()));
	}
}

bool CFixtureReuseSolution::GetAccessibleLocations(ETestSurface surface)
{
	bool accessFound = false;
	addAllNets();

	//////////////////////////////////////////////////
	// Gather accessible locations
	BlockStruct *fileBlock = m_pFile->getBlock();
	if (fileBlock != NULL)
	{
		POSITION pos = fileBlock->getHeadDataInsertPosition();
		while (pos)
		{
			DataStruct *data = fileBlock->getNextDataInsert(pos);
			InsertStruct *insert = data->getInsert();

			if (insert->getInsertType() != insertTypeTestAccessPoint)
				continue;

			if (surface != testSurfaceBoth && (insert->getGraphicMirrored() != (surface == testSurfaceBottom)))
				continue;

			if (AddTail_AccessibleLocations(data) != NULL)
				accessFound = true;
		}
	}

	return accessFound;
}

CFixtureReuseSolution::FixtureReuseSolutionReturnCodes CFixtureReuseSolution::PlaceFixturePilot(double x, double y,
			CString drillType, CString probeSize, CString NotUsed_compPin, CString pilotNetname)
{
	if (drillType != "PILOTDRILL")
		return frsRetInvalid;

	CPPAccessibleLocation *accLoc = findAccessLocation(x, y);
	if (accLoc != NULL)
	{
		CFixtureReuseNet *ppNet = NULL;
		CString acclocNetname = accLoc->GetNetName();
		
		// Because of possible prefixes and suffixes on fixture file netnames (especially in
		// Agilent files) there may not be an exact match. A close match is deemed good enough.
		// If the access location net name is contained in the fixture file net name (in this
		// function it is called pilotNetname), it is considered a match. When looking up
		// the net, be sure to use the access location net name, it is the one that
		// actually is in the ccz data.

		////if (!pilotNetname.CompareNoCase(acclocNetname) && m_ppNets.Lookup(pilotNetname, (CPPNet*&)ppNet))
#ifdef DO_NOT_KEEP_PILOT_IF_NET_CHANGED
#ifdef ALLOW_FUZZY_MATCH
		if (pilotNetnameUpr.Find(acclocNetnameUpr) > -1 && m_ppNets.Lookup(acclocNetname, (CPPNet*&)ppNet))
#else
		if (!pilotNetname.CompareNoCase(acclocNetname) && m_ppNets.Lookup(acclocNetname, (CPPNet*&)ppNet))
#endif
#else
		// Original code will not keep a "changed" pilot drill,
		// seems to me we ought to keep them.
		if (m_ppNets.Lookup(acclocNetname, (CPPNet*&)ppNet))
#endif
		{
			CFixtureReuseProbe *probe = ppNet->AddTail_Pilots(acclocNetname);

			// set probe template
			CDFTProbeTemplate *pTemplate = NULL;
			POSITION dummyPos = NULL;
			if (accLoc->GetSurface() == testSurfaceTop)
				pTemplate = m_pTestPlan->GetProbes().Find_TopProbes(probeSize, dummyPos);
			else if (accLoc->GetSurface() == testSurfaceBottom)
				pTemplate = m_pTestPlan->GetProbes().Find_BotProbes(probeSize, dummyPos);
			probe->SetProbeTemplate(pTemplate);
			accLoc->PlaceProbe((CPPProbe*)probe);

#ifdef DO_NOT_KEEP_PILOT_IF_NET_CHANGED			
			return frsRetRetained;
#else
			//if (pilotNetname.CompareNoCase(acclocNetname) == 0)
			CString bestMatch = m_ccnetnames.findBestMatch(pilotNetname);
			if (!bestMatch.IsEmpty())
				return frsRetRetained;
			else
				return frsRetChanged;
#endif
		}

		// removed because net names do not match or net could not be found
	}

	// removed because no net has no access marker at this location

	return frsRetRemoved;
}

CFixtureReuseSolution::FixtureReuseSolutionReturnCodes CFixtureReuseSolution::PlaceFixtureProbe(CFixtureProbeDrill *probeDrill, CPPAccessibleLocation *&accLoc)
{
	if (probeDrill == NULL)
		return frsRetInvalid;

	if (probeDrill->GetDrillType() != "PROBEDRILL")
		return frsRetInvalid;

   ETestSurface surface = probeDrill->GetSurface();

	accLoc = findAccessLocation(probeDrill->GetLocation().x, probeDrill->GetLocation().y, surface);
	if (accLoc != NULL)
	{
		CString netName = accLoc->GetNetName();

		// get the probe template
		CDFTProbeTemplate *pTemplate = NULL;
		POSITION dummyPos = NULL;

		if (accLoc->GetSurface() == testSurfaceTop)
			pTemplate = m_pTestPlan->GetProbes().Find_TopProbes(probeDrill->GetProbeSize(), dummyPos);
		else if (accLoc->GetSurface() == testSurfaceBottom)
			pTemplate = m_pTestPlan->GetProbes().Find_BotProbes(probeDrill->GetProbeSize(), dummyPos);

		// Place a probe on the access location.
      // If no probe already then place as primary probe.
      // If has primary probe already and no double-wire probe, place as double-wire.
      // If has primary and double-wire probe already, report error and don't place.
		if (!accLoc->IsProbed())
		{
			// Use the accLoc->netname here. Remember that we got the accLoc by location, not
			// by netname. This may be a probe that is in the "Changed" list i.e probe netname
			// may not match accLoc netname. It is the accLoc netname that matters. So use the
			// accLoc->netname, as this is what the probe is "changed to".

         CFixtureReuseNet *ppNet = NULL;
			if (m_ppNets.Lookup(accLoc->GetNetName(), (CPPNet*&)ppNet))
			{
				CFixtureReuseProbe *probe = ppNet->AddTail_Pilots(netName);
				probe->SetAsPilot(false);
				probe->SetProbeTemplate(pTemplate);
            probe->SetProbeRefname( probeDrill->GetProbeRefname() );
				accLoc->PlaceProbe((CPPProbe*)probe);
			}
		
		}
      else if (!accLoc->IsDoubleWired())
      {
         CFixtureReuseNet *ppNet = NULL;
			if (m_ppNets.Lookup(accLoc->GetNetName(), (CPPNet*&)ppNet))
			{
				CFixtureReuseProbe *probe = ppNet->AddTail_Pilots(netName);
				probe->SetAsPilot(false);
				probe->SetProbeTemplate(pTemplate);
            probe->SetProbeRefname( probeDrill->GetProbeRefname() );
            accLoc->PlaceDoubleWiredProbe((CPPProbe*)probe);
			}
      }
		else
		{
         // Error, access marker already has two probes, aka already "double wired".
         // We're not intending to support more than two for now, this is considered the practical limit.
         // So mark this probe as removed.
         return frsRetRemoved;
		}

		CString bestMatch = m_ccnetnames.findBestMatch(probeDrill->GetNetName());
		if (pTemplate == NULL)
			return frsRetMissingTemplate;
		else if (!bestMatch.IsEmpty() /*!probeDrill->GetNetName().CompareNoCase(netName)*/)
			return frsRetRetained;
		else
			return frsRetChanged;
	}
	else
	{
		// find a pilot location if available
		CString netName;
		CFixtureReuseNet *ppNet = NULL;
		if (m_ppNets.Lookup(probeDrill->GetNetName(), (CPPNet*&)ppNet))
		{
			netName = probeDrill->GetNetName();
		}
		else
		{
			// Not exact match, incoming netname may have had some special char replacements.
			// Look for best match of camcad net name to incoming netname.
			CString bestMatch = m_ccnetnames.findBestMatch(probeDrill->GetNetName());
			if (!bestMatch.IsEmpty())
				if (m_ppNets.Lookup(bestMatch, (CPPNet*&)ppNet))
					netName = bestMatch;
		}

		if (ppNet != NULL)
		{
			CPPAccessibleLocation *topLowestCostAccLoc = NULL, *botLowestCostAccLoc = NULL;
			int topLowestCost = INT_MAX, botLowestCost = INT_MAX;

			// look for an access marker with a pilot hole and put the probe there
			for (POSITION pos=ppNet->GetHeadPosition_AccessibleLocations(); pos!=NULL; ppNet->GetNext_AccessibleLocations(pos))
			{
				accLoc = ppNet->GetAt_AccessibleLocations(pos);

				if (accLoc->IsProbed() && ((CFixtureReuseProbe*)accLoc->GetPlaceableProbe())->IsPilotDrill())
				{
					if (accLoc->GetSurface() == testSurfaceTop)
					{
						POSITION dummyPos = NULL;
						CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_TopProbes(accLoc->GetPlaceableProbe()->GetProbeTemplateName(), dummyPos);
						if (pTemplate != NULL && topLowestCost > pTemplate->GetCost())
						{
							topLowestCost = pTemplate->GetCost();
							topLowestCostAccLoc = accLoc;
						}
					}
					else if (accLoc->GetSurface() == testSurfaceBottom)
					{
						POSITION dummyPos = NULL;
						CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_BotProbes(accLoc->GetPlaceableProbe()->GetProbeTemplateName(), dummyPos);
						if (pTemplate != NULL && botLowestCost > pTemplate->GetCost())
						{
							botLowestCost = pTemplate->GetCost();
							botLowestCostAccLoc = accLoc;
						}
					}
				}
			}

			if (topLowestCostAccLoc != NULL || botLowestCostAccLoc != NULL)
			{
				CFixtureReuseProbe *probe = NULL;
				if (probeDrill->GetSurface() == testSurfaceTop)
				{
					if (topLowestCostAccLoc != NULL)
					{
						probe = (CFixtureReuseProbe*)topLowestCostAccLoc->GetPlaceableProbe();
						accLoc = topLowestCostAccLoc;
					}
					else
					{
						probe = (CFixtureReuseProbe*)botLowestCostAccLoc->GetPlaceableProbe();
						accLoc = botLowestCostAccLoc;
					}

				}
				else if (probeDrill->GetSurface() == testSurfaceBottom)
				{
					if (botLowestCostAccLoc != NULL)
					{
						probe = (CFixtureReuseProbe*)botLowestCostAccLoc->GetPlaceableProbe();
						accLoc = botLowestCostAccLoc;
					}
					else
					{
						probe = (CFixtureReuseProbe*)topLowestCostAccLoc->GetPlaceableProbe();
						accLoc = topLowestCostAccLoc;
					}
				}

				if (probe != NULL)
				{
					probe->SetAsPilot(false);
               probe->SetProbeRefname( probeDrill->GetProbeRefname() );

					return frsRetMoved;
				}
			}

			// If we get here, probed net exists, but has no access point.
			// Create unplaced probe.
			/// Case 1702, Do not save unplaced probes in ccz data
#ifdef SAVE_UNPLACED_PROBES
			accLoc = NULL;
			CFixtureReuseProbe *probe = ppNet->AddTail_Pilots(netName);
			probe->SetAsPilot(false);
			probe->SetProbeNumber(probeDrill->GetProbeNumber());
#endif

			return frsRetUnplaced;

		}

		// removed because net does not exist
	}

	return frsRetRemoved;
}

long CFixtureReuseSolution::PlaceProbesOnBoard()
{
	// Out with the old
	DeleteAllProbes(m_pDoc, m_pFile);

	// In with the new
	long placedProbeCount = 0;
	CMapWordToPtr accessPointerMap;
	long accPointCount = 1;
	m_netResultList.empty();

	POSITION netPos = m_ppNets.GetStartPosition();
	while (netPos)
	{
		CString netName;
		CPPNet *ppNet = NULL;
		m_ppNets.GetNextAssoc(netPos, netName, ppNet);

		// Create net result
		CPPNetResult* netResult = new CPPNetResult(netName);
		m_netResultList.AddTail(netResult);

		POSITION probePos = ppNet->GetHeadPosition_Probes();
		while (probePos)
		{
			CFixtureReuseProbe *probe = (CFixtureReuseProbe*)ppNet->GetNext_Probes(probePos);

			if (probe->IsPilotDrill())
				continue;

			if (!probe->IsPlaced())
			{
				// Save unplaced proberesult
				CPPProbeResult* probeResult = netResult->AddProbeResult(probe->GetResourceType(), probe->GetNetName(), "", -1, probe->GetProbeTemplateName(), probe->GetProbeRefname());

				probe->AddProbeUnplacedToBoard(m_pFile, *m_pTestPlan);
				continue;
			}

			CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();
			if (accLoc == NULL)
				continue;

			// create an access point if one didn't exist (only for forced probes)
			CString accPointName;
			if (accLoc->GetTAInsert() == NULL)
			{
				// we need to create a test access point
				accPointName.Format("$$PACCESS_%d", accPointCount++);
				DataStruct *accData = PlaceTestAccessPoint(m_pDoc, m_pFile->getBlock(), accPointName, accLoc->GetLocation().x, accLoc->GetLocation().y,
               accLoc->GetSurface(), accLoc->GetNetName(), "SMD", accLoc->getFeatureEntity().getCompPin()->getEntityNumber(), 0.0,
					m_pDoc->getSettings().getPageUnits());
				accLoc->SetTAInsert(accData);
			}
			else
			{
				accPointName = accLoc->GetTAInsert()->getInsert()->getRefname();
			}

			// Save probe result
			netResult->SetNoProbe(false);
			CPPProbeResult* probeResult = netResult->AddProbeResult(probe->GetResourceType(), probe->GetNetName(), 
            accPointName, accLoc->GetTAInsert()->getEntityNumber(), probe->GetProbeTemplateName(), probe->GetProbeRefname());

			// Map access name to access point
         accessPointerMap.SetAt((int)accLoc->GetTAInsert()->getEntityNumber(), accLoc->GetTAInsert());		
		}
	}

	placedProbeCount = PlaceProbesResultOnBoard(accessPointerMap);
	accessPointerMap.RemoveAll();

	return placedProbeCount;
}



/******************************************************************************
* CFixtureReuse::CFixtureReuse
*/
CFixtureReuse::CFixtureReuse(CCEtoODBDoc &doc) : CObject()
, m_doc(doc)
, m_units(doc.getSettings().getPageUnits())
, m_pSolution(NULL)
{
	m_pFile = m_doc.getFileList().GetOnlyShown(blockTypePcb);

	// Don't forget that there just might not be a file shown
	if (m_pFile != NULL)
	{
      /*CDFTSolution *dftSolution = doc.GetCurrentDFTSolution(*m_pFile);

		if (dftSolution != NULL)
		{
			m_pSolution = new CFixtureReuseSolution(&doc, dftSolution->GetTestPlan(), m_pFile);
		}*/
	}
}

CFixtureReuse::~CFixtureReuse()
{
	delete m_pSolution;
}

bool CFixtureReuse::DumpToFile(CString Filename)
{
	if (m_pSolution == NULL)
		return false;

	CFormatStdioFile file;
	CFileException err;
	CString tempBuf;

	if (!file.Open(Filename, CFile::modeCreate|CFile::modeWrite, &err))
		return false;

	int indent = 0;
	file.WriteString("//////////////////////////////////////////////\n");
	file.WriteString("// %s\n", Filename);
	file.WriteString("//\n");
	file.WriteString("// Dump of current Fixture Reuse Info\n");
	file.WriteString("//////////////////////////////////////////////\n");
	file.WriteString("\n");

	m_pSolution->DumpToFile(file, 0);

	file.Close();

	return true;
}

CFixtureReuse::EFixtureFileType CFixtureReuse::determineFileType(CString fixtureFilePath)
{
	// Neither file reliably starts with anything in particular.
	// Look for telltale keywords.
	
	CStdioFile fixFile;
	CExtFileException err;
	if (!fixFile.Open(fixtureFilePath, CFile::modeRead, &err))
	{
		m_sErrMsg = err.getCause();
		return frFileTypeUnknown;
	}


	EFixtureFileType filetype = frFileTypeUnknown;
	bool foundAgilentNode = false;
	bool foundAgilentProbes = false;
	CString lineStr;

	while (fixFile.ReadString(lineStr))
	{
		lineStr.Trim();

		if (!lineStr.IsEmpty())
		{
			// Camcad test
			// If we encounter a Camcad section name, then it is a Camcad fixture file
			if (lineStr[0] == ';')
			{
				if ((lineStr == "; Drill Data") ||
					(lineStr == "; Recepticle Size") ||
					(lineStr == "; Test Resource Wiring") ||
					(lineStr == "; Power Injection Wiring") ||
					(lineStr == "; Tooling Holes") ||
					(lineStr == "; Capacitive Opens") )
				{
					filetype = frCamcadFile;
					break;
				}
			}

			// Agilent test
			// We look for more than one telltale line
			int curPos = 0;
			CString tok;
			tok = lineStr.Tokenize(" \t;", curPos);
			if (tok.CompareNoCase("NODE") == 0)
				foundAgilentNode = true;
			if (tok.CompareNoCase("PROBES") == 0)
				foundAgilentProbes = true;

			if (foundAgilentNode && foundAgilentProbes)
			{
				filetype = frAgilentFile;
				break;
			}

			CString upperCaseLineStr(lineStr);
			upperCaseLineStr.MakeUpper();

         // Fabmaster test (nails.asc file)
         if (upperCaseLineStr.Find("NAIL") == 0  &&  // must be a start of line
            upperCaseLineStr.Find("T/B") > -1   &&  // has a grid column
            upperCaseLineStr.Find("VIRTUAL") > -1)       // has a net column
         {
            filetype = frFabmasterFile;
            break;
         }

			// Teradyne test
			// Look for names Teradyne or Alchemist in header comment line.
			// Unfortunately header comments lines are easily subject to change, when
			// actual data format may not have changed. Look also for an apparant
			// keyword "TARGET CENTERS".

			if (upperCaseLineStr.Find("ALCHEMIST") > -1 ||
				upperCaseLineStr.Find("TERADYNE") > -1 ||
				upperCaseLineStr.Find("TARGET CENTERS") > -1)
			{
				filetype = frTeradyneD2BFile;
				break;
			}

		}
	}

	return filetype;
}

CString CFixtureReuse::StripPrefix(CString name)
{
	if (!m_prefix.IsEmpty())
	{
		int numwanted = name.GetLength() - m_prefix.GetLength();
		return name.Right(numwanted);
	}
	return name;
}

CFixtureProbeDrill *CFixtureReuse::FindProbe(ETestSurface surface, CPoint2d &location, CStdioFileWriteFormat &logFile)
{
	CFixtureProbeDrill *founddrill = NULL;

	for (POSITION pos=m_fixtureProbes.GetHeadPosition(); pos!=NULL; m_fixtureProbes.GetNext(pos))
	{
		CFixtureProbeDrill *drill = m_fixtureProbes.GetAt(pos);

		if (drill->GetSurface() == surface && drill->GetLocation().distance(location) < m_units.convertFrom(pageUnitsMils, 2.0))
		{
			if (founddrill == NULL)
			{
				founddrill = drill;
			}
			else
			{
				logFile.writef("ERROR: More than one probe on %s at %f,%f.\n",
					surface == testSurfaceTop ? "Top" : surface == testSurfaceBottom ? "Bottom" : "board",
					location.x, location.y);
			}
		}
	}

	return founddrill;
}

CFixtureReuse::EFixtureReuseReturnCodes CFixtureReuse::readTeradyneD2BFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface)
{
	CStdioFile fixFile;
	CExtFileException err;
	if (!fixFile.Open(fixtureFilePath, CFile::modeRead, &err))
	{
		m_sErrMsg = err.getCause();
		return frRetCantOpenFixture;
	}

   double unitsFactor = 1.0;

	int pilotCount = 0;

	// read fixture file
	CString lineStr;
	EFixtureFileSections section = frSectionNone;
	while (fixFile.ReadString(lineStr))
	{
		lineStr.Trim();

		if (lineStr.IsEmpty())
			continue;

		// comment
		CString upperLineStr(lineStr);
      upperLineStr.MakeUpper();
		if (lineStr[0] == '\\')
		{
			if (upperLineStr.Find("DRILL INFORMATION") > -1)
				section = frSectionDillData;
			else if (upperLineStr.Find("RECEPTICAL INSERTION INFORMATION") > -1)
				section = frSectionRecepticleSize;
			else if (upperLineStr.Find("WIRING INFORMATION - RESOURCES") > -1)
				section = frSectionTestResourceWiring;
			else if (upperLineStr.Find("WIRING INFORMATION - POWER INJECTION") > -1)
				section = frSectionPowerInjectionWiring;
			else if (upperLineStr.Find("TOOLING HOLES") > -1)
				section = frSectionToolingHoles;
			else if (upperLineStr.Find("CAPACITIVE PROBE PLATES") > -1)
				section = frSectionCapacitiveOpens;
         else if (upperLineStr.Find("ALL UNITS ARE IN") > -1)
         {
            int indx = upperLineStr.Find("ALL UNITS ARE IN");
            int len = ((CString)"ALL UNITS ARE IN").GetLength();
            upperLineStr.Delete(0, indx + len);
            // Test order matters, due to overlap in units names
            if (upperLineStr.Find("INCH") > -1)
               unitsFactor = Units_Factor(pageUnitsInches, m_doc.getSettings().getPageUnits());
            else if (upperLineStr.Find("MM100") > -1)
               unitsFactor = (1./100.) * Units_Factor(pageUnitsMilliMeters, m_doc.getSettings().getPageUnits());
            else if (upperLineStr.Find("MM") > -1)
               unitsFactor = Units_Factor(pageUnitsMilliMeters, m_doc.getSettings().getPageUnits());
            else if (upperLineStr.Find("THOU10") > -1)
               unitsFactor = (1./10.) * Units_Factor(pageUnitsMils, m_doc.getSettings().getPageUnits());
            else if (upperLineStr.Find("THOU") > -1)
               unitsFactor = Units_Factor(pageUnitsMils, m_doc.getSettings().getPageUnits());
            else if (upperLineStr.Find("UM") > -1)
               unitsFactor = (1./1000.) * Units_Factor(pageUnitsMilliMeters, m_doc.getSettings().getPageUnits());

         }
			continue;
		}

		switch (section)
		{
		// XY coordinate in Drill Data Section
		case frSectionDillData:
			{
				CString coord, drillType, probeSize, compPin, net;
				int curPos = 0;

				coord = lineStr.Tokenize(" \t\\", curPos);
				drillType = lineStr.Tokenize(" \t\\", curPos);
				probeSize = lineStr.Tokenize(" \t\\", curPos);
				compPin = lineStr.Tokenize(" \t\\", curPos);
				net = lineStr.Tokenize(" \t\\", curPos);

				coord.Trim();
				//double xCoord = m_units.convertFrom(fileUnits, atof(coord.Mid(1, coord.Find("Y"))));
				//double yCoord = m_units.convertFrom(fileUnits, atof(coord.Mid(coord.Find("Y")+1)));
				double xCoord = atof(coord.Mid(1, coord.Find("Y"))) * unitsFactor;
				double yCoord = atof(coord.Mid(coord.Find("Y")+1)) * unitsFactor;

				// Convert compPin name from Teradyne convention to CAMCAD convention
				compPin.Replace("-", ".");

				if (drillType == "BARRELDRILL")
				{
					CFixtureProbeDrill *drill = new CFixtureProbeDrill();
					drill->SetLocation(CPoint2d(xCoord, yCoord));
					drill->SetSurface(surface);
					drill->SetDrillType("PROBEDRILL");
					drill->SetProbeSize(probeSize);
					drill->SetCompPin(compPin);
					drill->SetNetName(net);

					m_fixtureProbes.AddTail(drill);
				}
				else if (drillType == "PILOTDRILL")
				{
					m_pSolution->PlaceFixturePilot(xCoord, yCoord, "PILOTDRILL", probeSize, compPin, net);
					pilotCount++;
				}
			}
			break;
		case frSectionTestResourceWiring:
			{
				CString probeRefname, testerInterface, compPin, net;
				double xCoord, yCoord;
				int curPos = 0;

				probeRefname = lineStr.Tokenize(" \t;", curPos);
				xCoord = atof(lineStr.Tokenize(" \t;", curPos)) * unitsFactor;
				yCoord = atof(lineStr.Tokenize(" \t;", curPos)) * unitsFactor;
				testerInterface = lineStr.Tokenize(" \t;", curPos);
				compPin = lineStr.Tokenize(" \t;", curPos);
				net = lineStr.Tokenize(" \t;", curPos);

				CPoint2d location(xCoord, yCoord);
				CFixtureProbeDrill *drill = this->FindProbe(surface, location, logFile);
				if (drill != NULL)
				{
					drill->SetProbeRefname(probeRefname);
				}
				else
				{
					logFile.writef("No probe drill on %s at location %f,%f for probe number %s.\n",
						surface == testSurfaceTop ? "Top" : surface == testSurfaceBottom ? "Bottom" : "board",
						location.x, location.y, probeRefname);
				}
			}
			break;
		}
	}

	fixFile.Close();

	logFile.writef("...Probes loaded : %d\n", m_fixtureProbes.GetCount());
	logFile.writef("...Pilots loaded : %d\n", pilotCount);

	return frRetOK;
}

CFixtureReuse::EFixtureReuseReturnCodes CFixtureReuse::readFabmasterFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface)
{
	CStdioFile fixFile;
	CExtFileException err;
	if (!fixFile.Open(fixtureFilePath, CFile::modeRead, &err))
	{
		m_sErrMsg = err.getCause();
		return frRetCantOpenFixture;
	}

	long pilotCount = 0;
	int boardCount = 0;
	bool quit = false;

	CString panelName;

	CString curNetname;
	int section = 0;
	bool foundDataHeader = false;

	CString lineStr;
	//EFixtureFileSections section = frSectionNone;
	while (fixFile.ReadString(lineStr) && !quit)
	{
		lineStr.Trim();

		if (!lineStr.IsEmpty())
      {
         if (!foundDataHeader)
         {
            CString upperCaseLineStr(lineStr);
            upperCaseLineStr.MakeUpper();
            if (upperCaseLineStr.Find("NAIL") == 0  &&  // must be a start of line
               upperCaseLineStr.Find("T/B") > -1   &&   // has a t/b column
               upperCaseLineStr.Find("VIRTUAL") > -1)   // has a virtual column
            {
               foundDataHeader = true;
            }
         }
         else
         {
            // All lines after data header are data records
            // 11 fields in data record
            CSupString superLine(lineStr);
            CStringArray lineParams;
            superLine.ParseQuote(lineParams, " ");
            CString refname, x, y, type, grid, psbside, netnumber, netname, virtualflag, targettype, targetref;
            if (lineParams.GetCount() > 0)
               refname = lineParams.GetAt(0);

            if (lineParams.GetCount() == 11)
            {
               double xcoord = atof(lineParams.GetAt(1));
               double ycoord = atof(lineParams.GetAt(2));
               ETestSurface surface = lineParams.GetAt(5).Find("T") > -1 ? testSurfaceTop : testSurfaceBottom;

					CFixtureProbeDrill *drill = new CFixtureProbeDrill();
               drill->SetProbeRefname(lineParams.GetAt(0));
					drill->SetLocation(CPoint2d(xcoord, ycoord));
					drill->SetSurface(surface);
					drill->SetDrillType("PROBEDRILL");
					drill->SetProbeSize("50");
					drill->SetCompPin(lineParams.GetAt(10));
					drill->SetNetName(lineParams.GetAt(7));

					m_fixtureProbes.AddTail(drill);
				}
         }
      }
   }

   return frRetOK;
}

CFixtureReuse::EFixtureReuseReturnCodes CFixtureReuse::readAgilentFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface)
{
	CStdioFile fixFile;
	CExtFileException err;
	if (!fixFile.Open(fixtureFilePath, CFile::modeRead, &err))
	{
		m_sErrMsg = err.getCause();
		return frRetCantOpenFixture;
	}

	long pilotCount = 0;
	int boardCount = 0;
	bool quit = false;

	CString panelName;

	CString curNetname;
	int section = 0;
	bool readingNode = false;

	CString lineStr;
	//EFixtureFileSections section = frSectionNone;
	while (fixFile.ReadString(lineStr) && !quit)
	{
		lineStr.Trim();

		if (lineStr.IsEmpty())
			continue;

		int curPos = 0;
		CString tok;
		tok = lineStr.Tokenize(" \t;", curPos);
		
		if (tok.CompareNoCase("NODE") == 0)
		{
			curNetname = StripPrefix(lineStr.Tokenize(" \"\t;", curPos)); 
			readingNode = true;
			section = 1;
			continue;
		} 
		else if (tok.CompareNoCase("PROBES") == 0)
		{
			section = 2;
			continue;
		}
		else if (tok.CompareNoCase("ALTERNATES") == 0)
		{
			section = 3;
			continue;
		}
		else if (tok.CompareNoCase("WIRES") == 0)
		{
			section = 4;
			continue;
		}
		else if (tok.CompareNoCase("TESTJET") == 0)
		{
			readingNode = false;
			section = 5;
			continue;
		}
		else if (tok.CompareNoCase("TRANSFERS") == 0)
		{
			readingNode = false;
			section = 6;
			continue;
		}
		else if (tok.CompareNoCase("PINS") == 0)
		{
			section = 7;
			continue;
		}
		else if (tok.CompareNoCase("OTHER") == 0)
		{
			readingNode = false;
			section = 8;
			continue;
		}
		else if (tok.CompareNoCase("PANEL") == 0)
		{
			panelName = lineStr.Tokenize(" \"\t;", curPos); 
			section = 9;
			continue;
		}
		else if (tok.CompareNoCase("BOARD") == 0)
		{
			boardCount++;
			if (boardCount > 1)
			{
				quit = true;
			}
			else
			{
				// If there is a panel, process names to determine prefix.
				// The board name will have a suffix, the first char is a delimiter,
				// the next is a board number. The order is reversed when used
				// as prefix on net names and such.
				if (!panelName.IsEmpty())
				{
					CString boardName = lineStr.Tokenize(" \"\t;", curPos);
					int suffixlen = boardName.GetLength() - panelName.GetLength();
					if (suffixlen > 0)
					{
						m_prefix = boardName.Right(suffixlen);
						m_prefix.MakeReverse();
					}

				}
			}
			section = 10;
			continue;
		}
		else if (tok.CompareNoCase("TOOLING") == 0)
		{
			section = 11;
			continue;
		}
		else if (tok.CompareNoCase("END") == 0)
		{
			readingNode = false;
			section = 999;
			continue;
		}

		switch (section)
		{
		case 11:
			{
				// Tooling Holes
				CString mysteryName = tok;
				CString xstr = lineStr.Tokenize(" \t;,", curPos); 
				CString ystr = lineStr.Tokenize(" \t;,", curPos);

				int ix = atoi(xstr);
				int iy = atoi(ystr);
				double milsx = ix / 10.0;  // Agilent units are 1/10 mil units, 1/10000 inch
				double milsy = iy / 10.0;
				double xCoord = m_units.convertFrom(pageUnitsMils, milsx);
				double yCoord = m_units.convertFrom(pageUnitsMils, milsy);
				CPoint2d *toolHole = new CPoint2d(xCoord,yCoord);
				m_toolHoles.AddTail(toolHole);
			}
			break;
		case 0:
			break;
		case 1:
			break;
		case 2:
			// Probes  (Camcad Probe Drill)
			if (readingNode)
			{
				CString probeName = tok;
				CString xstr = lineStr.Tokenize(" \t;,", curPos); 
				CString ystr = lineStr.Tokenize(" \t;,", curPos);
				CString compPinName;

				ETestSurface agilentSurface = testSurfaceBottom; // assumed default
				CString probeSize;
				tok = lineStr.Tokenize(" \t;", curPos);
				bool foundLWT = false;
				bool foundMandatory = false;
				while (!tok.IsEmpty())
				{
					if (tok.CompareNoCase("TOP") == 0)
						agilentSurface = testSurfaceTop;
					if (tok.Find("MIL", 0) > -1)
						probeSize = tok;
					else if (tok.Find("LWT") > -1)
						foundLWT = true;
					else if (tok.Find("MANDATORY") > -1)
						foundMandatory = true;
					else
					{
						// Might have quotes in peculiar fashion, e.g. "J2".1, get rid of them
						tok.Remove('"');
						// Get rid of board number prefix
						compPinName = StripPrefix(tok);
					}

					tok = lineStr.Tokenize(" \t;", curPos);
				}

				int ix = atoi(xstr);
				int iy = atoi(ystr);
				double milsx = ix / 10.0;  // Agilent units are 1/10 mil units, 1/10000 inch
				double milsy = iy / 10.0;
				double xCoord = m_units.convertFrom(pageUnitsMils, milsx);
				double yCoord = m_units.convertFrom(pageUnitsMils, milsy);

				probeSize.MakeUpper();
				probeSize.Replace("MIL","");
				if (probeSize.IsEmpty())
					probeSize = "100";

				// Drop "P" from name, e.g. P12 --> 12
				CString probeNumber = probeName.Right(probeName.GetLength() - 1);

				if (agilentSurface == surface || surface == testSurfaceBoth)
				{
					if (compPinName.IsEmpty())
						compPinName = "unknown.1";
					CFixtureProbeDrill *drill = new CFixtureProbeDrill();
					drill->SetLocation(CPoint2d(xCoord, yCoord));
					drill->SetSurface(agilentSurface);
					drill->SetDrillType("PROBEDRILL");
					drill->SetProbeSize(probeSize);
					drill->SetCompPin(compPinName);
					drill->SetNetName(curNetname);
					drill->SetProbeRefname(atoi(probeNumber));

					m_fixtureProbes.AddTail(drill);
				}
			}
			break;

		case 3:
			// Alternates  (Camcad Pilot Drill)
			if (readingNode)
			{
				CString xstr = tok;
				CString ystr = lineStr.Tokenize(" \t;,", curPos);

				bool bottom = true; // assumed default
				CString probeSize;
				tok = lineStr.Tokenize(" \t;", curPos);
				while (!tok.IsEmpty())
				{
					if (tok.CompareNoCase("TOP") == 0)
						bottom = false;
					if (tok.Find("MIL", 0) > -1)
						probeSize = tok;

					tok = lineStr.Tokenize(" \t;", curPos);
				}

				int ix = atoi(xstr);
				int iy = atoi(ystr);
				double milsx = ix / 10.0;  // Agilent units are 1/10 mil units, 1/10000 inch
				double milsy = iy / 10.0;
				double xCoord = m_units.convertFrom(pageUnitsMils, milsx);
				double yCoord = m_units.convertFrom(pageUnitsMils, milsy);

				probeSize.MakeUpper();
				probeSize.Replace("MIL","");
				if (probeSize.IsEmpty())
					probeSize = "100";

				CString compPin = "bogus.1";
				m_pSolution->PlaceFixturePilot(xCoord, yCoord, "PILOTDRILL", probeSize, compPin, curNetname);
				pilotCount++;
			}
			break;
		}
	}

	fixFile.Close();

	logFile.writef("...Probes loaded : %d\n", m_fixtureProbes.GetCount());
	logFile.writef("...Pilots loaded : %d\n", pilotCount);

	return frRetOK;
}

CFixtureReuse::EFixtureReuseReturnCodes CFixtureReuse::readCamcadFixtureFile(CString fixtureFilePath, CStdioFileWriteFormat &logFile, ETestSurface surface)
{
	CStdioFile fixFile;
	CExtFileException err;
	if (!fixFile.Open(fixtureFilePath, CFile::modeRead, &err))
	{
		m_sErrMsg = err.getCause();
		return frRetCantOpenFixture;
	}

	long pilotCount = 0;

	// read fixture file
	CString lineStr;
	EFixtureFileSections section = frSectionNone;
	while (fixFile.ReadString(lineStr))
	{
		lineStr.Trim();

		if (lineStr.IsEmpty())
			continue;

		// comment
		if (lineStr[0] == ';')
		{
			if (lineStr == "; Drill Data")
				section = frSectionDillData;
			else if (lineStr == "; Recepticle Size")
				section = frSectionRecepticleSize;
			else if (lineStr == "; Test Resource Wiring")
				section = frSectionTestResourceWiring;
			else if (lineStr == "; Power Injection Wiring")
				section = frSectionPowerInjectionWiring;
			else if (lineStr == "; Tooling Holes")
				section = frSectionToolingHoles;
			else if (lineStr == "; Capacitive Opens")
				section = frSectionCapacitiveOpens;
			continue;
		}

		switch (section)
		{
		// XY coordinate in Drill Data Section
		case frSectionDillData:
			{
				CString coord, drillType, probeSize, compPin, net;
				int curPos = 0;

				coord = lineStr.Tokenize(" \t;", curPos);
				drillType = lineStr.Tokenize(" \t;", curPos);
				probeSize = lineStr.Tokenize(" \t;", curPos);
				compPin = lineStr.Tokenize(" \t;", curPos);
				net = lineStr.Tokenize(" \t;", curPos);

				coord.Trim();
				double xCoord = m_units.convertFrom(pageUnitsMils, (double)atoi(coord.Mid(1, coord.Find("Y"))));
				double yCoord = m_units.convertFrom(pageUnitsMils, (double)atoi(coord.Mid(coord.Find("Y")+1)));

				if (drillType == "PROBEDRILL")
				{
					CFixtureProbeDrill *drill = new CFixtureProbeDrill();
					drill->SetLocation(CPoint2d(xCoord, yCoord));
					drill->SetSurface(surface);
					drill->SetDrillType(drillType);
					drill->SetProbeSize(probeSize);
					drill->SetCompPin(compPin);
					drill->SetNetName(net);

					m_fixtureProbes.AddTail(drill);
				}
				else if (drillType == "PILOTDRILL")
				{
					m_pSolution->PlaceFixturePilot(xCoord, yCoord, drillType, probeSize, compPin, net);
					pilotCount++;
				}
			}
			break;

		case frSectionTestResourceWiring:       // Yes, these two section have exact same
      case frSectionPowerInjectionWiring:     // format and semantics.
			{
				CString probeRefname, testerInterface, compPin, net;
				double xCoord, yCoord;
				int curPos = 0;

				probeRefname = lineStr.Tokenize(" \t;", curPos);
				xCoord = m_units.convertFrom(pageUnitsMils, (double)atoi(lineStr.Tokenize(" \t;", curPos)));
				yCoord = m_units.convertFrom(pageUnitsMils, (double)atoi(lineStr.Tokenize(" \t;", curPos)));
				testerInterface = lineStr.Tokenize(" \t;", curPos);
				compPin = lineStr.Tokenize(" \t;", curPos);
				net = lineStr.Tokenize(" \t;", curPos);

				CPoint2d location(xCoord, yCoord);
				for (POSITION pos=m_fixtureProbes.GetHeadPosition(); pos!=NULL; m_fixtureProbes.GetNext(pos))
				{
					CFixtureProbeDrill *drill = m_fixtureProbes.GetAt(pos);
					
					if (drill->GetSurface() == surface && drill->GetLocation().distance(location) < m_units.convertFrom(pageUnitsMils, 1.0))
						drill->SetProbeRefname(probeRefname);
				}
			}
			break;
		}
	}

	fixFile.Close();

	logFile.writef("...Probes loaded : %d\n", m_fixtureProbes.GetCount());
	logFile.writef("...Pilots loaded : %d\n", pilotCount);

	return frRetOK;
}

bool CFixtureReuse::AlignFixture(CStdioFileWriteFormat &logFile, CString &errmsg)
{
	CString toolAlignMsg;
	if (AlignFixtureByToolHoles(logFile, &toolAlignMsg))
		return true;

	CString compPinAlignMsg;
	if (AlignFixtureByCompPins(logFile, &compPinAlignMsg))
		return true;

	errmsg = "\n" + toolAlignMsg + "\n\n" + compPinAlignMsg;
	logFile.writef("%s\n", errmsg);

	return false;
}

bool CFixtureReuse::IsFixtureAlreadyAligned()
{
	return m_toolHoles.GetCount() > 0 && AreToolHolesAligned(m_toolHoles);
}

bool CFixtureReuse::AreToolHolesAligned(CFixtureToolHoleList &toolHoles)
{
	// If there are appropriate entites already at tooling hole locations
	// as specified in fixture, then fixture is already aligned.

	double fuzz = ALIGNMENT_TOLERANCE_MILS * Units_Factor(pageUnitsMils, m_doc.getSettings().getPageUnits());

	bool alreadyAligned = true; // optimist
	POSITION pos = toolHoles.GetHeadPosition();
	while (pos && alreadyAligned)
	{
		CPoint2d *toolPt = toolHoles.GetNext(pos);
		double xx = toolPt->x;
		double yy = toolPt->y;

		bool found = false;
		BlockStruct *fileBlock = m_pFile->getBlock();
		if (fileBlock != NULL)
		{
			POSITION insertpos = fileBlock->getHeadDataInsertPosition();
			while (insertpos && !found)
			{
				DataStruct *data = fileBlock->getNextDataInsert(insertpos);
				InsertStruct *insert = data->getInsert();

				if (fpnear(xx, insert->getOriginX(), fuzz) &&
					 fpnear(yy, insert->getOriginY(), fuzz) )
				{
					found = true;
				}
			}
		}

		if (!found)
			alreadyAligned = false;

	}
	return alreadyAligned;
}

CExtent CFixtureToolHoleList::DetermineExtent()
{
	CExtent extent;

	if (this->GetCount() > 0)
	{
		CPoint2d *first = (CPoint2d*)this->GetHead();
		extent.set(*first, *first);

		POSITION pos = this->GetHeadPosition();
		while (pos)
		{
			CPoint2d *toolPt = this->GetNext(pos);
			double xx = toolPt->x;
			double yy = toolPt->y;
			if (xx < extent.getXmin())
				extent.setXmin(xx);
			if (xx > extent.getXmax())
				extent.setXmax(xx);
			if (yy < extent.getYmin())
				extent.setYmin(yy);
			if (yy > extent.getYmax())
				extent.setYmax(yy);
		}
	}
	return extent;
}

bool CFixtureReuse::AlignFixtureByToolHoles(CStdioFileWriteFormat &logFile, CString *msgArg)
{
	// Align fixture using tooling holes

	if (IsFixtureAlreadyAligned())
	{
		return true;  // already aligned, just go
	}
	else
	{
		// Collect camcad tooling holes
		CFixtureToolHoleList camcadToolHoles;
		BlockStruct *fileBlock = m_pFile->getBlock();
		if (fileBlock != NULL)
		{
			POSITION insertpos = fileBlock->getHeadDataInsertPosition();
			while (insertpos)
			{
				DataStruct *data = fileBlock->getNextDataInsert(insertpos);
				InsertStruct *insert = data->getInsert();

				if (insert->getInsertType() == insertTypeDrillTool)
				{
					CPoint2d *toolPt = new CPoint2d(insert->getOrigin2d());
					camcadToolHoles.AddTail(toolPt);
				}

			}
		}

		// Must have same count of tool holes in camcad and in fixture file
		int cccnt = camcadToolHoles.GetCount();
		int fxcnt = m_toolHoles.GetCount();
		if (cccnt != fxcnt)
		{
			// Tool hole count mismatch
			CString msg;
			msg.Format("Number of tooling holes do not match.\nThe fixture contains %d tooling holes.\nFound %d tooling holes in currently visible file.\nNumber of tooling holes must match in order to align by tooling holes.",
				fxcnt, cccnt);
			///logFile.writef("%s\n", msg);
			///ErrorMessage(msg, "Fixture Reuse Alignment Error");
			if (msgArg != NULL)
				*msgArg = msg;
			return false;
		}
		else
		{
			// okay, proceed
			CExtent fixtureToolExtent = m_toolHoles.DetermineExtent();
			CExtent camcadToolExtent = camcadToolHoles.DetermineExtent();

			double camSizeX = camcadToolExtent.getXsize();
			double camSizeY = camcadToolExtent.getYsize();
							
			double fuzz = ALIGNMENT_TOLERANCE_MILS * Units_Factor(pageUnitsMils, m_doc.getSettings().getPageUnits());

			// We support 4 orientations of incoming fixture file: 0, 90, 180, 270
			for (int rotation = 0; rotation < 360; rotation += 90)
			{
				CTMatrix tmat;
				tmat.rotateDegrees(rotation);
				fixtureToolExtent.transform(tmat);
				double fixSizeX = fixtureToolExtent.getXsize();
				double fixSizeY = fixtureToolExtent.getYsize();

				if (fabs(fixSizeX - camSizeX) < fuzz &&
					fabs(fixSizeY - camSizeY) < fuzz)
				{
					// Same size, get location translation
					double dx = camcadToolExtent.getXmin() - fixtureToolExtent.getXmin();
					double dy = camcadToolExtent.getYmin() - fixtureToolExtent.getYmin();
					tmat.translate(dx, dy);

					CFixtureToolHoleList tempList;
					POSITION pos = m_toolHoles.GetHeadPosition();
					while (pos)
					{
						CPoint2d *toolPt = m_toolHoles.GetNext(pos);
						CPoint2d *copyPt = new CPoint2d(*toolPt);
						tmat.transform(*copyPt);
						tempList.AddTail(copyPt);
					}

					if (AreToolHolesAligned(tempList))
					{
						// Aligned
						// Apply tranform to probes
						for (POSITION probepos = m_fixtureProbes.GetHeadPosition(); probepos != NULL; m_fixtureProbes.GetNext(probepos))
						{
							CFixtureProbeDrill *drill = m_fixtureProbes.GetAt(probepos);
							CPoint2d location = drill->GetLocation();
							tmat.transform(location);
							drill->SetLocation(location);
						}
                  // Save tranformed tooling holes
                  // Apply tranform to ones in m_toolHoles list, as opposed to emptying m_toolHoles and
                  // copying the tempList holes over because tempList is a container and when it is deleted 
                  // so will be the points it contains, we'd end up with bad memory references.
					   POSITION pos = m_toolHoles.GetHeadPosition();
					   while (pos)
					   {
						   CPoint2d *toolPt = m_toolHoles.GetNext(pos);
                     tmat.transform(*toolPt);
                  }

						return true;
					}
					else
					{
						// Extents match, but tool pts not aligned
						// Could  be rotation issue (e.g. rectange off by 180, or square off by 90,180,270).
						// Do not report, keep trying.
					}
				}
				else
				{
					// Extent mismatch, not aligned
					// Could be rotation issue, e.g. recatngle off by 90,270
					// Do not report, keep trying.
				}

			}
		}

	}

	//logFile.writef("%s\n", "Could not align fixture tooling holes.");
	//ErrorMessage("Could not align fixture tooling holes.", "Fixture Reuse Alignment Error");
	if (msgArg != NULL)
		*msgArg = "Could not align fixture tooling holes.";
	return false; // failed to align
}

bool CFixtureReuse::SelectAlignmentCompPins(CompPinStruct **selectedcp1, CPoint2d *selectedprobe1Pt, CompPinStruct **selectedcp2, CPoint2d *selectedprobe2Pt, CMapWordToPtr& rejects)
{
	//
	// Select some probes that have compPin names, to use for aligning the fixture.
   // Seek two that are furthest apart.
   // Skip any that are in the reject map.
	//
   *selectedcp1 = NULL;
   *selectedcp2 = NULL;

	CCamcadNetnameList camcadNetnameList(this->m_pFile);

	CString compPin1Name;
	CPoint2d probe1Pt;
	CompPinStruct *cp1 = NULL;
	CString compPin2Name;
	CPoint2d probe2Pt;
	CompPinStruct *cp2 = NULL;

	CString oldCompPin1Name;
	CString oldCompPin2Name;

	for (POSITION pos=m_fixtureProbes.GetHeadPosition(); pos!=NULL; m_fixtureProbes.GetNext(pos))
	{
		CFixtureProbeDrill *drill = m_fixtureProbes.GetAt(pos);
		CString netname = drill->GetNetName();
		CString compPinName = drill->GetCompPin();


		if (!compPinName.IsEmpty() && compPinName.Find(".") > 0)
		{
			CString refdes = compPinName;
			int dotIndx = refdes.Find(".");
			refdes.Truncate(dotIndx);
			refdes.Remove('"');

			CString pinNumStr = compPinName.Right(compPinName.GetLength() - dotIndx - 1);
			int pinNum = atoi(pinNumStr);

			CString camcadNetname = camcadNetnameList.findBestMatch(netname);
			NetStruct *net = FindNet(m_pFile, camcadNetname);

         CompPinStruct *cp = NULL;
         // Can save time if we can find cp on net named in fixture file
         if (net != NULL)
				cp = net->findCompPin(refdes, pinNumStr, false/*not case sensitive*/);

         // If cp not yet found, look through all nets in file (fixture net names are probably mangled)
         if (cp == NULL)
            cp = FindCompPin(m_pFile, refdes, pinNumStr, &net);

         // Filter it out if it is already a reject
         void *ignoredVal;
         if (rejects.Lookup((WORD)cp, ignoredVal))  // If in reject map ...
            cp = NULL;                        // then cancel it

         // If we got a cp, check if it is a keeper
         if (cp != NULL)
         {
            if (compPin1Name.IsEmpty())
            {
               compPin1Name = compPinName;
               probe1Pt = drill->GetLocation();
               cp1 = cp;
            }
            else if (compPin2Name.IsEmpty())
            {
               compPin2Name = compPinName;
               probe2Pt = drill->GetLocation();
               cp2 = cp;
            }
            else
            {
               // We have a compPin 1 and 2, now look for compPins that are spread
               // furthest apart to provide best rotation determination.
               double curDistance = probe1Pt.distance(probe2Pt);
               CPoint2d newPt(drill->GetLocation());
               if (newPt.distance(probe2Pt) > curDistance)
               {
                  compPin1Name = compPinName;
                  probe1Pt = drill->GetLocation();
                  cp1 = cp;
               }
               else if (newPt.distance(probe1Pt) > curDistance)
               {
                  compPin2Name = compPinName;
                  probe2Pt = drill->GetLocation();
                  cp2 = cp;
               }
            }
         }

         if (compPin1Name.Compare(oldCompPin1Name) != 0 ||
            compPin2Name.Compare(oldCompPin2Name) != 0)
         {
            CPoint2d cp1Pt(-999.,-999.);
            CPoint2d cp2Pt(-999.,-999.);
            if (cp1) cp1Pt = cp1->getOrigin();
            if (cp2) cp2Pt = cp2->getOrigin();
         }

         oldCompPin1Name = compPin1Name;
         oldCompPin2Name = compPin2Name;
      }
   }

   if (cp1 != NULL && cp2 != NULL)
   {
      *selectedcp1 = cp1;
      *selectedcp2 = cp2;

      *selectedprobe1Pt = probe1Pt;
      *selectedprobe2Pt = probe2Pt;

      return true;
   }

   return false;
}

bool CFixtureReuse::AttempAlignmentByCompPins(CString& alignStatMsg, CompPinStruct *cp1, CPoint2d probe1Pt, CompPinStruct *cp2, CPoint2d probe2Pt, CTMatrix& fixtureTransform, bool& fixtureFileMirrored)
{
   bool alignSucceeded = false;
   
   fixtureFileMirrored = false;

   // Must have two alignment points
   if (cp1 != NULL && cp2 != NULL)
   {
      for (int mirror = 0; mirror < 2 && !alignSucceeded; mirror++)
      {
         CTMatrix tempTransform;
         if (mirror == 1)
         {
            tempTransform.mirrorAboutYAxis();
            fixtureFileMirrored = true;
         }

         for (int rot = 0; rot < 360 && !alignSucceeded; rot += 90)
         {
            if (rot > 0)
               tempTransform.rotateDegrees(90.0); // increment rotation 90 degrees

            double deltaX = 0.0;
            double deltaY = 0.0;
            double tolerance = m_units.convertFrom(pageUnitsMils, ALIGNMENT_TOLERANCE_MILS);
            double deltaX1 = 0.0;
            double deltaY1 = 0.0;
            double deltaX2 = 0.0;
            double deltaY2 = 0.0;

            if (cp1) 
            {
               CPoint2d cpPt = cp1->getOrigin();
               CPoint2d transPt = tempTransform.transform((const CPoint2d)probe1Pt);
               deltaX1 = cpPt.x - transPt.x;
               deltaY1 = cpPt.y - transPt.y;

               //logFile.writef("CP1 (%s) (%f,%f)  DELTA (%f,%f)\n",
               //   cp1->getPinRef(), cpPt.x, cpPt.y, deltaX1, deltaY1);
            }

            if (cp2) 
            {
               CPoint2d cpPt = cp2->getOrigin();
               CPoint2d transPt = tempTransform.transform((const CPoint2d)probe2Pt);
               deltaX2 = cpPt.x - transPt.x;
               deltaY2 = cpPt.y - transPt.y;

               //logFile.writef("CP2 (%s) (%f,%f)  DELTA (%f,%f)\n",
               //   cp2->getPinRef(), cpPt.x, cpPt.y, deltaX2, deltaY2);
            }

            // This test is to see if the two different offsets are consistent.
            // If they are, then scale and rotation is a match.
            if (fabs(deltaX1 - deltaX2) < tolerance &&
               fabs(deltaY1 - deltaY2) < tolerance)
            {
               alignSucceeded = true;

               alignStatMsg.Format("Fixture File Alignment   Mirrored: %s  Rotated: %d  Offset: %.3f, %.3f",
                  mirror == 1 ? "Yes" : "No", rot, deltaX1, deltaY1);           

               if (fabs(deltaX1) < tolerance && fabs(deltaY1) < tolerance)
               {
                  // No offset, do nothing
               }
               else
               {
                  // Set offset translation
                  tempTransform.translate(deltaX1, deltaY1);
                  ///logFile.writef("Translation: %f, %f\n", deltaX1, deltaY1);
               }
            }
            else
            {
               // Mismatched deltas, can't use same offsets for all points.
               // Continue, trying rotations and mirroring
               //logFile.writef("\nAlignment at %d rotation failed. Selected parts have inconsistent offsets.\n", rot);
            }
            if (alignSucceeded)
               fixtureTransform = tempTransform;
         }
      }
   }

   return alignSucceeded;
}

bool CFixtureReuse::AlignFixtureByCompPins(CStdioFileWriteFormat &logFile, CString *msgArg)
{
   CompPinStruct *cp1 = NULL;
	CompPinStruct *cp2 = NULL;
   CPoint2d probe1Pt;
   CPoint2d probe2Pt;
   CTMatrix fixtureTransform;

   CMapWordToPtr rejectMap;
   bool fixtureFileMirrored = false;
   bool alignSucceeded = false;

   int limit = 500; // somewhat arbitrary, but if 500 comp pairs won't align ... ?  (mainly this is to prevent log file explosion)
   int tries = 0;

   logFile.writef(";--------------------------------------------------\n");

   while (!alignSucceeded && (tries++ < limit) && SelectAlignmentCompPins(&cp1, &probe1Pt, &cp2, &probe2Pt, rejectMap))
   {
      CString alignStatMsg;
      alignSucceeded = this->AttempAlignmentByCompPins(alignStatMsg, cp1, probe1Pt, cp2, probe2Pt, fixtureTransform, fixtureFileMirrored);

      if (alignSucceeded)
      {
         // yes, the apparantly out of place semicolon between the \n's does belong there
         logFile.writef("; Alignment using %s and %s succeeded.\n;\n", cp1->getPinRef(), cp2->getPinRef());
         logFile.writef("; %s\n", alignStatMsg);
      }
      else
      {
         rejectMap.SetAt((WORD)cp1, NULL);
         rejectMap.SetAt((WORD)cp2, NULL);
         logFile.writef("; Alignment using %s and %s failed.\n", cp1->getPinRef(), cp2->getPinRef());
      }
   }

   // If tries is less than limit, then we just ran out of comp pins, no error message for this.
   // If tries >= limit then there are more comp pins, we are just choosing to stop trying. Report this.
   if (!alignSucceeded && tries >= limit)
      logFile.writef(";\n; Quitting after %d attempts to align by Comp Pin pairs.\n", (tries - 1));

   logFile.writef(";--------------------------------------------------\n\n");

   if (alignSucceeded)
   {
      // Apply tranform to probes
      for (POSITION pos=m_fixtureProbes.GetHeadPosition(); pos!=NULL; m_fixtureProbes.GetNext(pos))
      {
         CFixtureProbeDrill *drill = m_fixtureProbes.GetAt(pos);
         CPoint2d location = drill->GetLocation();
         fixtureTransform.transform(location);
         drill->SetLocation(location);
         if (fixtureFileMirrored)
            drill->SetSurface(drill->GetSurface() == testSurfaceBottom ? testSurfaceTop : testSurfaceBottom);
      }

      // Apply transform to tooling holes
      for (POSITION pos = m_toolHoles.GetHeadPosition(); pos != NULL; m_toolHoles.GetNext(pos))
      {
         CPoint2d *toolPt = m_toolHoles.GetAt(pos);
         fixtureTransform.transform(*toolPt);
      }

      return true;
   }
   
   // If we get here then alignment failed
	if (msgArg != NULL)
		*msgArg =      "Alignment by probed component pin locations failed.";
   else
      logFile.writef("Alignment by probed component pin locations failed.\n");

	return false;
}

bool CFixtureReuseNet::IsNetProbed()
{
	for (POSITION pos=GetHeadPosition_AccessibleLocations(); pos!=NULL; GetNext_AccessibleLocations(pos))
	{
		CPPAccessibleLocation *accLoc = GetAt_AccessibleLocations(pos);
		if (accLoc->IsProbed())
			return true;
	}
	return false;
}

int CFixtureReuseNet::ReportAccessibleLocations(CStdioFileWriteFormat &logFile)
{	
	int accLocCount = 0;

	for (POSITION pos=GetHeadPosition_AccessibleLocations(); pos!=NULL; GetNext_AccessibleLocations(pos))
	{
		CPPAccessibleLocation *accLoc = GetAt_AccessibleLocations(pos);

		DataStruct *insert = accLoc->GetTAInsert();
		CString targetType = accLoc->GetTargetType();
		CString accPointName = accLoc->GetTAInsert()->getInsert()->getRefname();
		CEntity entity = accLoc->getFeatureEntity();
		EEntityType entityType = entity.getEntityType();

		CString itemName;

		if (entityType == entityTypeCompPin)
		{
			CompPinStruct *compPin = entity.getCompPin();
			if (compPin != NULL)
				itemName = compPin->getPinRef();
		}
		else if (entityType == entityTypeData)
		{
			DataStruct *data = entity.getData();
			if (data != NULL && data->getInsert() != NULL)
				itemName = data->getInsert()->getRefname();
			if (itemName.IsEmpty())
				itemName = insertTypeToString(data->getInsert()->getInsertType());
		}
		else
		{
			itemName = "Unexpected Item Type";
		}

		accLocCount++;

		// Write net name before first accLoc
		if (accLocCount == 1)
         logFile.writef("Net %s\n", this->GetNetName());

		// Write accLoc
		logFile.writef("    has %s accessible at %.3f,%.3f on %s\n",
			itemName,
			accLoc->GetLocation().x, accLoc->GetLocation().y,
			accLoc->GetSurface() == DFT_SURFACE_TOP ? "Top" : "Bottom");
	}
	return accLocCount;
}

bool CFixtureReuseSolution::IsNetProbed(NetStruct *net)
{
	if (net != NULL)
	{
		CFixtureReuseNet *ppNet = NULL;
		if (m_ppNets.Lookup(net->getNetName(), (CPPNet*&)ppNet))
		{
			return ppNet->IsNetProbed();
		}
	}

	return false;
}

int CFixtureReuseSolution::ReportAccessibleLocations(NetStruct *net, CStdioFileWriteFormat &logFile)
{
	// Returns count of access locations reported

	if (net != NULL)
	{
		CFixtureReuseNet *ppNet = NULL;
		if (m_ppNets.Lookup(net->getNetName(), (CPPNet*&)ppNet))
		{
			return ppNet->ReportAccessibleLocations(logFile);
		}
	}
	return 0;
}

bool CFixtureReuse::formatUsesSeparateTopAndBottomFiles(EFixtureFileType filetype)
{
   switch (filetype)
   {
   case frAgilentFile:
   case frFabmasterFile:
      return false;

   case frCamcadFile:
   case frTeradyneD2BFile:
      return true;
   }

   return false;
}

void CFixtureReuse::MakeProbeRefnamesUnique()
{
   // Make probe refnames unique among probes, not necessarily unique throughout entire CC data.
   // It is most common that probe refnames are numeric.
   // So uniqueness will be obtained by appending alpha suffix to name when needed.
   // Not expecting more than a couple dups, like probe 123, 123B, 123C.
   // But there is essentially no limit, will do suffixes B to Y, then start with Z0001, Z0002, etc.

   CMapStringToPtr map;

   POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
   while (pos != NULL)
   {
      DataStruct *data = m_pFile->getBlock()->getNextDataInsert(pos);
      if (data != NULL && data->isInsertType(insertTypeTestProbe))
      {
         CString baseRefname(data->getInsert()->getRefname());
         CString refname( baseRefname );
         int count = 0;

         // Cycle looking up name and incrementing suffix until
         // refname is not in map.
         void *junk;
         while (map.Lookup(refname, junk))
         {
            count++;
            CString suffix;
            if (count <= 25)
               suffix.Format("%c", (char)('A' + count)); // Note, starts with 'B' on purpose
            else
               suffix.Format("Z%04d", count - 25);

            refname.Format("%s%s", baseRefname, suffix);
         }

         // Now have a refname not in map.
         // Reset probe refname, and add to map.
         data->getInsert()->setRefname(refname);
         map.SetAt(refname, (void*)0x01 /*useless value*/);

         // If fixture reuse made refname attribs then they should be set
         // here too. But it doesn't, so we don't. Just thought you'd want
         // to know we thought of that. If someone ever adds the attrib and
         // does not update it here, well, they added a bug.
      }
   }
}

CFixtureReuse::EFixtureReuseReturnCodes CFixtureReuse::ApplyFixtureFiles(CString topFilePath, CString botFilePath, CString logFilePath)
{
	if (m_pFile == NULL || m_pSolution == NULL)
	{
		ErrorMessage("Access Analysis must be run prior to running Fixture Reuse");     // case 1497
		return frRetNoDefaultFileStruct;
	}

	CFileFind findFile;

	ETestSurface surface = testSurfaceUnset;
	if (!topFilePath.IsEmpty() && botFilePath.IsEmpty())
		surface = testSurfaceTop;
	else if (topFilePath.IsEmpty() && !botFilePath.IsEmpty())
		surface = testSurfaceBottom;
	else if (!topFilePath.IsEmpty() && !botFilePath.IsEmpty())
		surface = testSurfaceBoth;
	else
		return frRetCantOpenFixture;

	// Only need to check top file type up front, so get accessible locations has
	// the right surfaces. Must getAccessibeLocations before calling readers, as
	// readers will access them on the fly for pilot holes.
	EFixtureFileType topFileType = determineFileType(topFilePath);
	if (!topFilePath.IsEmpty() && !formatUsesSeparateTopAndBottomFiles(topFileType))
		surface = testSurfaceBoth;

	if (!m_pSolution->GetAccessibleLocations(surface))
		return frRetNoAccess;

	CStdioFileWriteFormat logFile(1024);
	if (logFile.open(logFilePath))
	{
		logFile.writef("Fixture Reuse Log file\n");
		logFile.writef("  Filename: %s\n", logFilePath);
		CTime t;
		t = t.GetCurrentTime();
		logFile.writef("%s\n", t.Format("Created date : %A, %B %d, %Y at %H:%M:%S"));
		logFile.writef("\n");
	}

	
	if (!topFilePath.IsEmpty())
	{
		if (topFileType == frFileTypeUnknown)
		{
			CString msg("Format not recognized for fixture file [" + topFilePath + "].");
			ErrorMessage(msg, "Fixture Reuse");
			logFile.writef("ERROR! %s\n\n", msg);
		}
		else
		{
			logFile.writef("Reading top fixture file [%s]\n", topFilePath);

			EFixtureReuseReturnCodes retcode;
			if (topFileType == frTeradyneD2BFile)
				retcode = readTeradyneD2BFixtureFile(topFilePath, logFile, testSurfaceBoth);
			else if (topFileType == frFabmasterFile)
				retcode = readFabmasterFixtureFile(topFilePath, logFile, testSurfaceBoth);
			else if (topFileType == frAgilentFile)
				retcode = readAgilentFixtureFile(topFilePath, logFile, testSurfaceBoth);
			else if (topFileType == frCamcadFile)
				retcode = readCamcadFixtureFile(topFilePath, logFile, testSurfaceTop);

			if (retcode == frRetCantOpenFixture)
				logFile.writef("...Could not open fixture file.\n\n");
			else
				logFile.writef("...Successful.\n\n");
		}
	}

	EFixtureFileType botFileType = determineFileType(botFilePath);
	if (!botFilePath.IsEmpty() && (formatUsesSeparateTopAndBottomFiles(topFileType) || formatUsesSeparateTopAndBottomFiles(botFileType)))
	{
		if (botFileType == frFileTypeUnknown)
		{
			CString msg("Format not recognized for fixture file [" + botFilePath + "].");
			ErrorMessage(msg, "Fixture Reuse");
			logFile.writef("ERROR! %s\n\n", msg);
		}
		else
		{
			logFile.writef("Reading bottom fixture file [%s]\n", botFilePath);

			EFixtureReuseReturnCodes retcode;
			if (botFileType == frTeradyneD2BFile)
				retcode = readTeradyneD2BFixtureFile(botFilePath, logFile, testSurfaceBottom);
			else if (botFileType == frAgilentFile) // should never happen now, but structure is here if we want it back
				retcode = readAgilentFixtureFile(botFilePath, logFile, testSurfaceBottom);
			else if (botFileType == frCamcadFile)
				retcode = readCamcadFixtureFile(botFilePath, logFile, testSurfaceBottom);

			if (retcode == frRetCantOpenFixture)
				logFile.writef("...Could not open fixture file.\n\n");
			else
				logFile.writef("...Successful.\n\n");
		}

	}

   // Alignment, only for non-CAMCAD files
   if (topFileType == frAgilentFile || botFileType == frAgilentFile ||
      topFileType == frFabmasterFile || botFileType == frFabmasterFile ||
      topFileType == frTeradyneD2BFile || botFileType == frTeradyneD2BFile)
   {
      CString errmsg;
      if (!AlignFixture(logFile, errmsg))
      {
         CString prompt( errmsg + "\n\nProceed with fixture import?" );
         int okCancel = ErrorMessage(prompt, "Fixture Reuse Alignment Failure", MB_OKCANCEL | MB_ICONQUESTION);

         if (okCancel != IDOK)
         {
            logFile.writef("\nImport cancelled by user.\n\n");
            return frRetAlignmentFailure;
         }
         else
         {
            logFile.writef("\nImport proceeding without alignment, using fixture file coordinates as-is.\n\n");
         }
      }
   }

   // Report tooling hole differences.
   // Note that this has to happen AFTER alignment.
   ReportToolHoleDifferences(logFile);

	// Placement
	CFixtureProbeDrillList retainedDrills(false), changedDrills(false), movedDrills(false), unplacedDrills(false), removedDrills(false), missingTemplates(false);
	int probeCount = 0;
	for (POSITION pos=m_fixtureProbes.GetHeadPosition(); pos!=NULL; m_fixtureProbes.GetNext(pos))
	{
		CPPAccessibleLocation *accLoc = NULL;
		CFixtureProbeDrill *drill = m_fixtureProbes.GetAt(pos);
		switch (m_pSolution->PlaceFixtureProbe(drill, accLoc))
		{
		case CFixtureReuseSolution::frsRetRetained:
			drill->SetAccLoc(accLoc);
			retainedDrills.AddTail(drill);
			break;
		case CFixtureReuseSolution::frsRetChanged:
			drill->SetAccLoc(accLoc);
			changedDrills.AddTail(drill);
			break;
		case CFixtureReuseSolution::frsRetMoved:
			drill->SetAccLoc(accLoc);
			movedDrills.AddTail(drill);
			break;
		case CFixtureReuseSolution::frsRetUnplaced:
			drill->SetAccLoc(accLoc);
			unplacedDrills.AddTail(drill);
			break;
		case CFixtureReuseSolution::frsRetRemoved:
			drill->SetAccLoc(accLoc);
			removedDrills.AddTail(drill);
			break;
		case CFixtureReuseSolution::frsRetMissingTemplate:
			drill->SetAccLoc(accLoc);
			missingTemplates.AddTail(drill);
			break;
		}
	}


	// If any templates are missing, i.e. the fixture file calls for a probe size that was not
	// in the probe template list, then it is a fatal error. Do not report all the Changed, Retained,
	// etc. probes, because they will not be added to the ccz. Only report the missing template probes
	// in the log file, also send a pop-up message to the user.

	if (missingTemplates.GetCount() > 0)
	{
		CString msg = "One or more probe sizes in the fixture file do not have matching probe templates defined.\n";
		msg += "See the FixtureReuse.log file for details.\n";
		msg += "Add probe templates via the \"Probe\" tab in the Tools | Analysis | DFT Analysis | Probe Placement dialog.\n";
		ErrorMessage(msg, "Fixture Reuse Aborted - Missing Probe Template");

		logFile.writef(";--------------------------------------------------\n");
		logFile.writef("; Missing Probe Templates : %d probes missing template definitions\n", missingTemplates.GetCount());
		logFile.writef(";--------------------------------------------------\n");
		for (POSITION pos=missingTemplates.GetHeadPosition(); pos!=NULL; missingTemplates.GetNext(pos))
		{
			CFixtureProbeDrill *drill = missingTemplates.GetAt(pos);
			CPPAccessibleLocation *accLoc = drill->GetAccLoc();
			logFile.writef("Template missing for probe size \"%s\". Used for probe %s at (%1.3lf, %1.3lf) on %s side, net %s (%s).\n", 
				drill->GetProbeSize(), drill->GetProbeRefname(),
				drill->GetLocation().x, drill->GetLocation().y, 
				(accLoc->GetSurface()==testSurfaceTop)?"Top":"Bottom",
				drill->GetNetName(), accLoc->GetNetName());
		}
		logFile.writef("\n");
		logFile.writef("Use the \"Probes\" tab in Tools | Analysis | DFT Analysis | Probe Placement to define probe templates.\n");
		logFile.writef("Use the sizes reported above as the probe name in the template definition.\n");
		logFile.writef("\n");
	}
	else
	{
#ifdef ALLOW_FUZZY_MATCH
		if (topFileType == frAgilentFile || botFileType == frAgilentFile)
		{
			// Move probes that are "close enough" from the changed list to
			// the retained list.

			for (POSITION pos=changedDrills.GetHeadPosition(); pos!=NULL; changedDrills.GetNext(pos))
			{
				CFixtureProbeDrill *drill = changedDrills.GetAt(pos);
				CPPAccessibleLocation *accLoc = drill->GetAccLoc();
				CString drillNet = drill->GetNetName();
				CString accNet = accLoc->GetNetName();
				if (drillNet.Find(accNet) > -1)
				{
					// add to retained
					retainedDrills.AddTail(drill);
					// remove from changed drills (by clearing ptr)
					changedDrills.SetAt(pos, NULL);

				}
			}
		}
#endif

		logFile.writef(";--------------------------------------------------\n");
		logFile.writef("; Changed Probes : %d changed\n", changedDrills.GetCount());
		logFile.writef(";--------------------------------------------------\n");
		for (POSITION pos=changedDrills.GetHeadPosition(); pos!=NULL; changedDrills.GetNext(pos))
		{
			CFixtureProbeDrill *drill = changedDrills.GetAt(pos);
			// Some entries may have been removed from Changed list by making them NULL
			if (drill != NULL)
			{
				CPPAccessibleLocation *accLoc = drill->GetAccLoc();
				logFile.writef("Probe %s at (%1.3lf, %1.3lf) on %s changed from net %s to net %s.\n", 
					drill->GetProbeRefname(), drill->GetLocation().x, drill->GetLocation().y, 
					(accLoc->GetSurface()==testSurfaceTop)?"Top":"Bottom",
					drill->GetNetName(), accLoc->GetNetName());
			}
		}
		logFile.writef("\n");

		logFile.writef(";--------------------------------------------------\n");
		logFile.writef("; Moved Probes : %d moved\n", movedDrills.GetCount());
		logFile.writef(";--------------------------------------------------\n");
		for (POSITION pos=movedDrills.GetHeadPosition(); pos!=NULL; movedDrills.GetNext(pos))
		{
			CFixtureProbeDrill *drill = movedDrills.GetAt(pos);
			CPPAccessibleLocation *accLoc = drill->GetAccLoc();
			logFile.writef("Probe %s moved from (%1.3lf, %1.3lf) on %s to (%1.3lf, %1.3lf) on %s on net %s", 
            drill->GetProbeRefname(),
				drill->GetLocation().x, drill->GetLocation().y, (drill->GetSurface()==testSurfaceTop)?"Top":"Bottom",
				accLoc->GetLocation().x, accLoc->GetLocation().y, (accLoc->GetSurface()==testSurfaceTop)?"Top":"Bottom",
				accLoc->GetNetName(), drill->GetNetName() );

			if (accLoc->GetNetName().CompareNoCase(drill->GetNetName()) != 0)
				logFile.writef(" (%s)", drill->GetNetName());

			logFile.writef("\n");
		}
		logFile.writef("\n");

		/// Case 1702, moved Unplaced probes in with Removed probes in report
		///logFile.writef(";--------------------------------------------------\n");
		///logFile.writef("; Unplaced Probes : %d unplaced\n", unplacedDrills.GetCount());
		///logFile.writef(";--------------------------------------------------\n");
		///for (POSITION pos=unplacedDrills.GetHeadPosition(); pos!=NULL; unplacedDrills.GetNext(pos))
		///{
		///	CFixtureProbeDrill *drill = unplacedDrills.GetAt(pos);
		///	logFile.writef("Probe %d unplaced at (%1.3lf, %1.3lf) on net %s.\n", drill->GetProbeNumber(), drill->GetLocation().x, drill->GetLocation().y, drill->GetNetName());
		///}
		///logFile.writef("\n");

		logFile.writef(";--------------------------------------------------\n");
		logFile.writef("; Removed Probes : %d removed\n", removedDrills.GetCount() + unplacedDrills.GetCount());
		logFile.writef(";--------------------------------------------------\n");
		// Case 1702, group unplaced and removed probes together under Removed heading
		for (POSITION pos=unplacedDrills.GetHeadPosition(); pos!=NULL; unplacedDrills.GetNext(pos))
		{
			CFixtureProbeDrill *drill = unplacedDrills.GetAt(pos);
			logFile.writef("Probe %s removed at (%1.3lf, %1.3lf, %s) on net %s.\n", 
				drill->GetProbeRefname(), 
				drill->GetLocation().x, drill->GetLocation().y, 
				drill->GetSurface() == testSurfaceTop ? "Top" : drill->GetSurface() == testSurfaceBottom ? "Bottom" : "None",
				drill->GetNetName());
		}
		for (POSITION pos=removedDrills.GetHeadPosition(); pos!=NULL; removedDrills.GetNext(pos))
		{
			CFixtureProbeDrill *drill = removedDrills.GetAt(pos);
			logFile.writef("Probe %s removed at (%1.3lf, %1.3lf, %s) on net %s.\n", 
				drill->GetProbeRefname(), 
				drill->GetLocation().x, drill->GetLocation().y, 
				drill->GetSurface() == testSurfaceTop ? "Top" : drill->GetSurface() == testSurfaceBottom ? "Bottom" : "None",
				drill->GetNetName());
		}
		logFile.writef("\n");

		logFile.writef(";--------------------------------------------------\n");
		logFile.writef("; Nets Requiring Probes\n");
		logFile.writef(";--------------------------------------------------\n");
		//
		{
			//CMapStringToPtr map;
			//map.RemoveAll();
			//void* voidPtr = NULL;
			//netsWithNoAccessCnt = 0;
			CStringArray netsWithNoAccess;

         //CAccessAnalysisSolution* aaSolution = (m_doc.GetCurrentDFTSolution(*m_pFile)==NULL)?NULL:m_doc.GetCurrentDFTSolution(*m_pFile)->GetAccessAnalysisSolution();

			POSITION netPos = m_pFile->getNetList().GetHeadPosition();
			/*while (netPos)
			{
				NetStruct* net = m_pFile->getNetList().GetNext(netPos);
				if (net != NULL && !m_pSolution->IsNetProbed(net))
				{

					// If solution is present then consider only "analyzed" nets
					if (aaSolution != NULL)
					{
						CAANetAccess* netAccess = aaSolution->GetNetAccess(net->getNetName());
						if (netAccess == NULL || !netAccess->IsNetAnalyzed())
							continue;
					}

					if (m_pSolution->ReportAccessibleLocations(net,logFile) < 1)
					{
						netsWithNoAccess.Add(net->getNetName());
					}
				}
			}*/
			for (int i = 0; i < netsWithNoAccess.GetCount(); i++)
			{
				CString netname = netsWithNoAccess.GetAt(i);
				logFile.writef("Net %s has no accessible location.\n", netname);
			}
			netsWithNoAccess.RemoveAll();
		}
		logFile.writef("\n");


		logFile.writef(";--------------------------------------------------\n");
		logFile.writef("; Retained Probes : %d retained\n", retainedDrills.GetCount());
		logFile.writef(";--------------------------------------------------\n");
		// Case 1702, Separate close match and exact match retained probes.
		// Write retained probes in two passes, first pass write probes where fixture file
		// net name is NOT exact match to ccz net name, in second pass write exact matches.
		// Fixture file net names change, e.g. due to character replacements in exporters.
		for (int pass = 0; pass < 2; pass++)
		{
			for (POSITION pos=retainedDrills.GetHeadPosition(); pos!=NULL; retainedDrills.GetNext(pos))
			{
				CFixtureProbeDrill *drill = retainedDrills.GetAt(pos);
				CPPAccessibleLocation *accLoc = drill->GetAccLoc();
				if ((pass == 0 && accLoc->GetNetName().CompareNoCase(drill->GetNetName()) != 0) ||
					(pass == 1 && accLoc->GetNetName().CompareNoCase(drill->GetNetName()) == 0))
				{
					logFile.writef("Probe %s retained at (%1.3lf, %1.3lf) on %s on net %s", 
                  drill->GetProbeRefname(),
						drill->GetLocation().x, drill->GetLocation().y, (drill->GetSurface()==testSurfaceTop)?"Top":"Bottom",
						accLoc->GetNetName());

					if (accLoc->GetNetName().CompareNoCase(drill->GetNetName()) != 0)
						logFile.writef(" (%s)", drill->GetNetName());

					logFile.writef("\n");
				}
			}
		}
		logFile.writef("\n");
	}

	logFile.close();

#ifdef _DEBUG
	DumpToFile("C:\\Development\\CAMCAD\\FixtureReuse.dbg");
#endif
	// Only add probes to ccz if no template errors occurred.
	// Probably there are other errors that ought to cause the same thing,
	// but they are not detected or tracked as of this writing.
	// Some errors (like missing templates) will be detected by PlaceProbesOnBoard(),
	// and the probes will simply be skipped, with not messages or other  indication.
	// This ends up causing a mismatch between the fixture reuse log count of probes
	// placed and the actual number of probes placed.
	if (missingTemplates.GetCount() == 0)
		m_pSolution->PlaceProbesOnBoard();

	return frRetOK;
}

//-----------------------------------------------------------------------------------------

void CFixtureReuse::ReportToolHoleDifferences(CStdioFileWriteFormat &logFile)
{
   // Report only if fixture contains tooling hole
   if (this->m_toolHoles.GetCount() > 0)
   {
      // Report only if tooling holes are deemed mismatched
      if (!this->AreToolHolesAligned(this->m_toolHoles))
      {
         logFile.writef(";--------------------------------------------------\n");
         logFile.writef("; Tool Holes Mismatch\n");
         logFile.writef(";\n");

         logFile.writef("; Fixture Tool Holes\n");
         logFile.writef("; \tX\t\tY\n");
         POSITION fixPos = m_toolHoles.GetHeadPosition();
         while (fixPos != NULL)
         {
            CPoint2d *toolPt = m_toolHoles.GetNext(fixPos);
            //double xx = toolPt->x;
            //double yy = toolPt->y;
            logFile.writef("; \t%1.3f\t\t%1.3f\n", toolPt->x, toolPt->y);
         }
         logFile.writef(";\n");

         logFile.writef("; PCB Tool Holes\n");
         logFile.writef("; \tX\t\tY\n");
         int count = 0;
         BlockStruct *fileBlock = m_pFile->getBlock();
         if (fileBlock != NULL)
         {
            POSITION insertpos = fileBlock->getHeadDataInsertPosition();
            while (insertpos)
            {
               DataStruct *data = fileBlock->getNextDataInsert(insertpos);
               InsertStruct *insert = data->getInsert();

               if (insert->getInsertType() == insertTypeDrillTool)
               {
                  logFile.writef("; \t%1.3f\t\t%1.3f\n", insert->getOriginX(), insert->getOriginY());
                  count++;
               }
            }
         }
         if (count == 0)
            logFile.writef("; \t--None--\n");

         logFile.writef(";\n");
         logFile.writef(";--------------------------------------------------\n");
         logFile.writef("\n");  // blank line, no semi-colon for "box" on purpose

      }
   }
}

//-----------------------------------------------------------------------------------------

CCamcadNetnameList::CCamcadNetnameList(FileStruct *file, CStdioFileWriteFormat *logFile)
{
	// Create string list of net names in file.
	// Order names from longest to shortest.

	// Camcad names were mangled using this "name_check" facility during HP3070 write.
	// The fixture coming back is expected to still have those names.
	// We can not "unmap" the characters in the Agilent name, because the char replacements
	// are not a 1:1 match.
	// So build the list of mapped camcad names, to be used for matching the netnames
	// that are in the fixture file.

	CString nameCheckFile = getApp().getUserPath() + "3070.chk";
   check_init(nameCheckFile);

	bool doAmbiguousNameMapPopup = true;

	if (file != NULL )
	{
		POSITION pos = file->getNetList().GetHeadPosition();
		while (pos)
		{
			NetStruct *net = file->getNetList().GetNext(pos);
			CString camcadNetname = net->getNetName();
			CString cleanedCamcadNetname = check_name('n', camcadNetname, true);

			// Set map entry to find original camcad name from adjusted name.
			// Glitch: genuinely unique camcad netnames that differ only by characters
			// that get replaced, and whose replacements are the same character become
			// indistinguishable. As of this writing, the Hp3070 writer does not check
			// for such a situation, so genuinely different nets may end up with the
			// same net name. We will end up keeping only one mapping. It will be
			// the first one encountered, but that essentially works out to random
			// chance. We report an error if ambiguous mapping is detected.

			CString mappedCamcadNetname;
			if (!m_nameMap.Lookup(cleanedCamcadNetname, mappedCamcadNetname))
			{
				m_nameMap.SetAt(cleanedCamcadNetname, camcadNetname);
			}
			else
			{
				// Log file ptr turns on/off popup error message too
				if (logFile != NULL)
				{
					if (mappedCamcadNetname.Compare(camcadNetname) != 0)
					{
						if (doAmbiguousNameMapPopup)
						{
							ErrorMessage("More than one CAMCAD net name maps to the same fixture file\nnet name when illegal character remapping is applied (via 3070.chk).\nSee log file for offending net names.", "Fixture File Reuse - Ambiguous Net Name Mapping");
							doAmbiguousNameMapPopup = false; // only want it once
						}

						logFile->writef("Warning: CAMCAD net names \"%s\" and \"%s\" both map to fixture file net name \"%s\"\n",
							camcadNetname, mappedCamcadNetname, cleanedCamcadNetname);
					}
				}
			}
					


			// Save adjusted name in ordered array (longest to shortest)
			// for later searching.
			bool added = false;
			for (int i = 0; i < m_namelist.GetCount() && !added; i++)
			{
				if (cleanedCamcadNetname.GetLength() > m_namelist.GetAt(i).GetLength())
				{
					m_namelist.InsertAt(i,cleanedCamcadNetname);
					added = true;
				}
			}

			if (!added)
				m_namelist.Add(cleanedCamcadNetname);
		}
	}
}

CCamcadNetnameList::~CCamcadNetnameList()
{
	check_deinit();
}

CString CCamcadNetnameList::findBestMatch(CString foreignNetname)
{
	// The best match is the longest camcad net name that is a 
	// substring in the foreign netname. The extra chars in the
	// foreign netname are considered prefix and suffix.
	// If no match is found return empty string.

	// The namelist is kept in order of longest to shortest,
	// so first match found is the best match, by these criteria.

	if (!foreignNetname.IsEmpty())
	{
		for (int i = 0; i < m_namelist.GetCount(); i++)
		{
			CString cleanedCamcadNetname = m_namelist.GetAt(i);
#ifdef ALLOW_FUZZY_MATCH
			if (foreignNetname.Find(cleanedCamcadNetname,0) > -1)
#else
			if (foreignNetname.CompareNoCase(cleanedCamcadNetname) == 0)
#endif
			{
				CString actualCamcadNetname;
				m_nameMap.Lookup(cleanedCamcadNetname, actualCamcadNetname);
				return actualCamcadNetname;
			}
		}
	}

	return "";
}
