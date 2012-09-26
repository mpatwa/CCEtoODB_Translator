// $Header: /CAMCAD/4.6/TestAttr2DFT.cpp 3     12/07/06 12:11p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/


#include "stdafx.h"
///#include "CAMCAD 4.5.h"

#include "ccdoc.h"
#include "CCEtoODB.h"
#include "graph.h"
#include "dft.h"
#include "Net_Util.h"
#include "TestAttr2Dft.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//------------------------------------------------------------------------

void CCEtoODBDoc::OnTestAttr2Dft() 
{
   int stat = 0; // 0 = no PCB file, 1 = PCB, but not visible, 2 = PCB and visible

   FileStruct *pcbFile = NULL;
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);
      if (file->getBlockType() == BLOCKTYPE_PCB)
      {
         if (file->isShown())
         {
            stat |= 2;
            pcbFile = file;
            break;
         }
         else
         {
            stat |= 1;
         }
      }
      if (!stat && file->isShown() && !pcbFile)
         pcbFile = file;
   }

   if (!pcbFile)
   {
      if (stat == 1)
         ErrorMessage("PCB File loaded, but not visible -> will be ignored!");
      else
         ErrorMessage("No PCB File loaded!");
      return;
   }

   TestAttr2DftDlg dlg(this, pcbFile);
   dlg.DoModal();
}

//-------------------------------------------------------------------
// TestAttr2DftDlg dialog

IMPLEMENT_DYNAMIC(TestAttr2DftDlg, CDialog)
#ifdef BYBYBYE
TestAttr2DftDlg::TestAttr2DftDlg(CWnd* pParent /*=NULL*/)
	: CResizingDialog(TestAttr2DftDlg::IDD, pParent)
	, m_radioProbeSide(0)
	, m_strPageUnits(_T(""))
	, m_strProbeSize(_T(""))
{
}
#endif
TestAttr2DftDlg::TestAttr2DftDlg(CCEtoODBDoc *doc, FileStruct *file, CWnd* pParent /*=NULL*/)
	: CResizingDialog(TestAttr2DftDlg::IDD, pParent)
	, m_doc(doc)
	, m_file(file)
	, m_radioProbeSide(0)
	, m_strPageUnits(_T(""))
	, m_strProbeSize(_T(""))
{
	m_strPageUnits = GetUnitName(doc->getPageUnits());
	
	double size = doc->convertToPageUnits(pageUnitsInches, 0.100);
	CString sizeStr;
	sizeStr.Format("%f", size);
	m_strProbeSize = sizeStr;
}

TestAttr2DftDlg::~TestAttr2DftDlg()
{
}

void TestAttr2DftDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Radio(pDX, IDC_RADIO_VIEWALL, m_radioView);
	DDX_Radio(pDX, IDC_TA2DFT_SIDE, m_radioProbeSide);
	DDX_Text(pDX, IDC_PAGE_UNITS, m_strPageUnits);
	DDX_Text(pDX, IDC_TA2DFT_PROBE_SIZE, m_strProbeSize);
}


BEGIN_MESSAGE_MAP(TestAttr2DftDlg, CDialog)
	ON_BN_CLICKED(ID_CREATE_DFT_SOUTION, OnBnClickedCreateDftSoution)
END_MESSAGE_MAP()


//------------------------------------------------------------------------

// TestAttr2DftDlg message handlers

void TestAttr2DftDlg::OnBnClickedCreateDftSoution()
{
	UpdateData(TRUE);

	double probeSize = atof(m_strProbeSize);

	DeleteAllProbes(m_doc, m_file);
	DeleteAllTestAccessPoints(m_doc, m_file);


	TestAttr2Dft solver(m_doc, m_file, m_radioProbeSide, probeSize);
	solver.ProcessInserts();
	solver.ProcessCompPins();

	m_doc->OnRedraw();
	//getActiveView()->GetDocument()->OnRedraw();
}

//------------------------------------------------------------------------

TestAttr2Dft::TestAttr2Dft(CCEtoODBDoc *doc, FileStruct *file, int probeSide, double probeSize)
{
   m_pDoc = doc;
	m_pcbFile = file;
	
	m_testSurface = testSurfaceBoth; // default
	if (probeSide == 0)
		m_testSurface = testSurfaceTop;
	else if (probeSide == 1)
		m_testSurface = testSurfaceBottom;
	// else already Both

	m_probeSize = probeSize;

	CString logFile = GetLogfilePath("TestAttr2DFT.LOG");
	m_logfp = NULL;
	if ((m_logfp = fopen(logFile, "wt")) == NULL)   // rewrite file
	{
		MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
		// continue anyway, just be sure to check m_logfp before trying to write
	}

	if (m_logfp)
		fprintf(m_logfp, "Test Attribute to Probe Placement\n\n");

	m_probeNamer = new CProbeNumberer(doc, file);
	m_probeNamer->Report(m_logfp);
}

//------------------------------------------------------------------------

TestAttr2Dft::~TestAttr2Dft()
{
	if (m_logfp != NULL)
		fclose(m_logfp);

	if (m_probeNamer != NULL)
		delete m_probeNamer;
}

//------------------------------------------------------------------------

void TestAttr2Dft::ProcessCompPins()
{
   POSITION netPos = m_pcbFile->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = m_pcbFile->getNetList().GetNext(netPos);
      
		// process unused nets ?
		//if ((net->getFlags() & NETFLAG_UNUSEDNET))
		//	continue;

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         if (compPin != NULL)
			{
		
				Attrib *attrib = is_attvalue(m_pDoc, compPin->getAttributesRef(), ATT_TEST, 0);
				if (attrib != NULL)
				{
					PlacePair(compPin, net->getNetName(), NULL);
				}
			}
      }
	}
}

//------------------------------------------------------------------------

void TestAttr2Dft::ProcessInserts()
{
	// Process all insert types except vias

	if (m_pcbFile != NULL)
	{
		POSITION pos = m_pcbFile->getBlock()->getHeadDataInsertPosition();
		while (pos)
		{
			DataStruct *data = m_pcbFile->getBlock()->getNextDataInsert(pos);
			InsertStruct *insert = data->getInsert();
			CString testAttrVal;

			bool placeProbe = false;
			if (insert->getInsertType() == insertTypeTestPoint)
			{
				placeProbe = true;
				// Special treatment for test points, if refname contains number use it
				// for probe number. Everything else just gets next number in sequence.
				// We support only the refname form of Axxx, where A is alpha zero or
				// more chars, and xxx is some number. E.g. I22
				CString refname = insert->getRefname();
				while (!refname.IsEmpty() && !isdigit(refname.GetAt(0)))
					refname.Delete(0,1);
				int probeNumber = atoi(refname);
				if (probeNumber > 0 && m_probeNamer->NumberIsReservedForEntity(probeNumber, data->getEntityNumber()))
					m_probeNamer->SetProbeNumber(probeNumber);
			}
			else
			{
				Attrib *attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_TEST, 0);
				if (attrib)
				{
					testAttrVal = get_attvalue_string(m_pDoc, attrib);
					placeProbe = true;
				}
			}

			if (placeProbe)
			{
				if (insert->getInsertType() == insertTypeVia)
					PlacePair(data);
				else
					PlacePairOnEachPin(data);
			}

		}
	}
}

//------------------------------------------------------------------------

CString TestAttr2Dft::GetTechnology(CompPinStruct *compPin, DataStruct *componentdata)
{
	// compPin is particular compPin
	// component data is associated "parent" pcbcomponent data

	CString technology;
	Attrib *techAttr;

	// Favor technology attr on compPin, if there is one

	if (compPin != NULL)
	{
		techAttr = is_attvalue(m_pDoc, compPin->getAttributesRef(), ATT_TECHNOLOGY, 0);
		if (techAttr)
			technology = get_attvalue_string(m_pDoc, techAttr);
	}
	
	if (technology.IsEmpty() && componentdata != NULL)
	{
		techAttr = is_attvalue(m_pDoc, componentdata->getAttributesRef(), ATT_TECHNOLOGY, 0);
		if (techAttr)
			technology = get_attvalue_string(m_pDoc, techAttr);
	}

	return (technology.IsEmpty() ? "SMD" : technology);
}

//------------------------------------------------------------------------

ETestSurface TestAttr2Dft::GetSurface(bool bothAllowed, CString testAttrVal, CString technologyAttrVal, bool placedTop)
{
	// Determination of Test Surface
	// If testAttrVal is "Top" or "Bottom", then that is the test surface.
	// If testAttrVal is "Both" or blank, then if "Both" is allowed then
	// that is the test surface. "Both" is allowed for Vias and Test Points.
	// I.e. for Vias and Test Points that are THRU, Both really means both surfaces.
	// For other things Both means either/or.
	// If component technology attr is SMD then access/probe go on same side as insert.
	// If THRU, then access/probe go on opposite side.
	// If technology is not set, treat as SMD.

	if (testAttrVal.CompareNoCase("TOP") == 0)
		return testSurfaceTop;
	else if (testAttrVal.CompareNoCase("BOTTOM") == 0)
		return testSurfaceBottom;
	// else it is either "Both" or blank, both of which are treated the same as follows

	if (technologyAttrVal.CompareNoCase("THRU") == 0)
	{
		if (bothAllowed)
		{
			// Chris and Mark concur that any TEST value that is not "TOP" or "BOTTOM" should
			// be treated as "BOTH". Since "TOP" and "BOTTOM" are handled up front, if we get
			// here then the only possibility is "BOTH", or a BOTH-equivalent. So don't
			// bother checking the value, it must be BOTH.
			//////if (testAttrVal.IsEmpty() || testAttrVal.CompareNoCase("BOTH") == 0)
				return testSurfaceBoth;
		}

		if (placedTop)
			return testSurfaceBottom;
		else
			return testSurfaceTop;
	}

	// Else SMD rule

	return placedTop ? testSurfaceTop : testSurfaceBottom;
}

//------------------------------------------------------------------------

DataStruct *TestAttr2Dft::FindComponentData(CString refdes)
{
	// Was originally limited to looking for INSERT TYPE PCBCOMPONENT.
	// That was too restrictive, as it left out TEST POINT inserts, which
	// are definitely of interest. So maybe any sort of insert is of interest,
	// so long as the refname matches, no more quibbling on insert type.

	//FileStruct *file = doc->FileList.GetOnlyShown(blockTypePcb);

	POSITION pos = m_pcbFile->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = m_pcbFile->getBlock()->getDataList().GetNext(pos);

		if (data->getDataType() == T_INSERT)
		{
			InsertStruct *insert = data->getInsert();
			CString insertname = insert->getRefname();
			if (data->getInsert()->getRefname().Compare(refdes) == 0)
			{
				return data;
			}
		}
	}

	return NULL;
}

//------------------------------------------------------------------------

void TestAttr2Dft::PlacePairOnEachPin(DataStruct *componentData)
{
	if (componentData != NULL && componentData->getInsert() != NULL)
	{
		InsertStruct *insert = componentData->getInsert();
		BlockStruct *insertedBlock = m_pDoc->Find_Block_by_Num(insert->getBlockNumber());
		
      if (insertedBlock != NULL)
      {
         for (CDataListIterator dataListIterator(*insertedBlock,insertTypePin);dataListIterator.hasNext();)
         {
            DataStruct* pinData = dataListIterator.getNext();
            CString pinName = pinData->getInsert()->getRefname();

            NetStruct *net = NULL;
            CString netName;
            CompPinStruct *compPin = FindCompPin(m_pcbFile, insert->getRefname(), pinName, &net);
            if (net != NULL)
               netName = net->getNetName();

            if (!compPin && m_logfp)
               fprintf(m_logfp, "No compPin for pin %s, component %s with TEST attribute\n", pinName, insert->getRefname());

            if (compPin)
               PlacePair(compPin, netName, componentData);
         }
      }

	}
}

//------------------------------------------------------------------------

void TestAttr2Dft::PlacePair(DataStruct *componentData)
{
	if (componentData != NULL && componentData->getInsert() != NULL)
	{
		CString netName;
		Attrib *attrib = is_attvalue(m_pDoc, componentData->getAttributesRef(), ATT_NETNAME, 0);
		if (attrib)
			netName = get_attvalue_string(m_pDoc, attrib);
		
		if (netName.IsEmpty())
		{
			if (m_logfp)
			{
				fprintf(m_logfp, "No net name for  %s %s\n",
					insertTypeToString(componentData->getInsert()->getInsertType()),
					componentData->getInsert()->getRefname());
			}
		}
		else
		{
			PlacePair(NULL, netName, componentData);
		}

	}
}

//------------------------------------------------------------------------

void TestAttr2Dft::PlacePair(CompPinStruct *compPin, CString netName, DataStruct *componentData)
{

	InsertTypeTag insertType = insertTypeUnknown;

	if (componentData == NULL && compPin != NULL)
		componentData = this->FindComponentData(compPin->getRefDes());

	CString testAttrVal;
	bool placedTop = true; // useless default
	if (componentData != NULL && componentData->getInsert() != NULL)
	{
		placedTop = componentData->getInsert()->getPlacedTop();
		Attrib *attrib = is_attvalue(m_pDoc, componentData->getAttributesRef(), ATT_TEST, 0);
		if (attrib)
			testAttrVal = get_attvalue_string(m_pDoc, attrib);
		insertType = componentData->getInsert()->getInsertType();
	}

	// If we have a compPin then override the componentData TEST attrib
	// with one from compPin if it is present
	if (compPin != NULL)
	{
		Attrib *attrib = is_attvalue(m_pDoc, compPin->getAttributesRef(), ATT_TEST, 0);
		if (attrib)
			testAttrVal = get_attvalue_string(m_pDoc, attrib);
	}

	// If we have one or the other or both, we can proceed
	if (componentData != NULL || compPin != NULL)
	{
	  CString technology = this->GetTechnology(compPin, componentData);
	  bool bothSurfacesAllowed = (insertType == insertTypeVia || insertType == insertTypeTestPoint);
	  ETestSurface surface = this->GetSurface(bothSurfacesAllowed, testAttrVal, technology, placedTop);

	  // If surface determined is BOTH and active surface is BOTH, we actually place
	  // two probes, one on each side. If determined surface is BOTH and active 
	  // surface is TOP or BOTTOM, then place one probe on active side.
	  // If determined surface is TOP or BOTTOM then place probe if that side
	  // is active or if BOTH sides are active.

		if (surface == m_testSurface // determined surface matches user  setting
			|| m_testSurface == testSurfaceBoth // both surfaces are active
			|| surface == testSurfaceBoth) // at least one or the other must be active
		{
			//*rcf* debug only
			CString pinName = compPin ? compPin->getPinName() : "none";
			CString refName = compPin ? compPin->getRefDes() : componentData->getInsert()->getRefname();
			CString debugStr;
			debugStr.Format("Ref(%s) Pin(%s)", refName, pinName);

			CPoint2d location;
			int dataLink;
			// Favor compPin
			if (compPin != NULL)
			{
				location = compPin->getOrigin();
				dataLink = compPin->getEntityNumber();
			}
			else
			{
				location = componentData->getInsert()->getOrigin2d();
				dataLink = componentData->getEntityNumber();
			}

			CString targetType = technology;
			double exposedMetalDiameter = m_pDoc->convertToPageUnits(pageUnitsInches, 0.125);

			// Top Side Probe
			if ((surface == testSurfaceTop || surface == testSurfaceBoth) &&
				(m_testSurface == testSurfaceTop || m_testSurface == testSurfaceBoth))
			{
				int probeNumber = m_probeNamer->GetProbeNumber();
				PlacePair(probeNumber, location, testSurfaceTop, netName, targetType, 
					dataLink, exposedMetalDiameter, debugStr);
			}

			// Bottom Side Probe
			if ((surface == testSurfaceBottom || surface == testSurfaceBoth) &&
				(m_testSurface == testSurfaceBottom || m_testSurface == testSurfaceBoth))
			{
				int probeNumber = m_probeNamer->GetProbeNumber();
				PlacePair(probeNumber, location, testSurfaceBottom, netName, targetType, 
					dataLink, exposedMetalDiameter, debugStr);
			}
		}
	}

}

//------------------------------------------------------------------------

void TestAttr2Dft::PlacePair(int probeNumber, CPoint2d location, ETestSurface surface, 
									  CString netName, CString targetType, int dataLink,
									  double exposedMetalDiameter, CString debugStr)
{
	// All the code here is patterned after some found in CAMCADNavigagtor.cpp

	CString accessName;
	accessName.Format("$$ACCESS_%d", probeNumber);

	double x = location.x;
	double y = location.y;

	DataStruct *accessData = PlaceTestAccessPoint(m_pDoc, m_pcbFile->getBlock(), accessName, x, y, 
		surface, netName, targetType, dataLink,
		exposedMetalDiameter, m_pDoc->getPageUnits());

	//*rcf*debug
	///m_pDoc->SetUnknownAttrib(&accessData->getAttributesRef(), "RCF_DEBUG", debugStr, SA_OVERWRITE, NULL);

	// Using tempate intermediate here, to ease correlation of this with what
	// CAMCADNavigator and other probe placing code does.

	CDFTProbeTemplate fakeTemplate;
	CString probeGeomName;
	probeGeomName.Format("Probe-%f", m_probeSize); // Do not use underscore, messes up Probe Styles Matrix operation
	fakeTemplate.SetName(probeGeomName);
	fakeTemplate.SetDiameter(m_pDoc->convertPageUnitsTo(pageUnitsMils, m_probeSize)); // template stores size in mils
   CDFTProbeTemplate *pTemplate = &fakeTemplate;
   
	CString toolName;
	toolName.Format("DRILL_%f", m_probeSize);

   BlockStruct *probeBlock = CreateTestProbeGeometry(m_pDoc, pTemplate->GetName(), pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()),
      toolName, pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()));         

   //create the probe data and add it to the board
   m_pDoc->PrepareAddEntity(m_pcbFile);
	CString probeName;
	probeName.Format("%d", probeNumber);
	double rot = 0.0;
	int mirror = surface == testSurfaceTop ? 0 : (MIRROR_FLIP | MIRROR_LAYERS);
   DataStruct *probeData = Graph_Block_Reference(probeBlock->getName(), probeName, probeBlock->getFileNumber(), x, y, rot, mirror, 1.0, -1, FALSE);
   probeData->getInsert()->setInsertType(insertTypeTestProbe);


	CString ddlinkStr;
	ddlinkStr.Format("%d", accessData->getEntityNumber());
	m_pDoc->SetUnknownAttrib(&probeData->getAttributesRef(), ATT_DDLINK, ddlinkStr, SA_OVERWRITE, NULL);

	m_pDoc->SetUnknownAttrib(&probeData->getAttributesRef(), ATT_NETNAME, netName, SA_OVERWRITE, NULL);

	m_pDoc->SetUnknownAttrib(&probeData->getAttributesRef(), ATT_TESTRESOURCE, "Test", SA_OVERWRITE, NULL);

	m_pDoc->SetUnknownAttrib(&probeData->getAttributesRef(), ATT_REFNAME, probeName, SA_OVERWRITE, NULL);

	CString placementStr = "Placed";
	m_pDoc->SetUnknownAttrib(&probeData->getAttributesRef(), ATT_PROBEPLACEMENT, placementStr, SA_OVERWRITE, NULL);

}

//-------------------------------------------------------------------------

// The purpose of this is to determine and contain the Test Point probe numbers.
// Probes on Test Points will be numbered based on Test Point refname, if it 
// contains a number. Everything else just gets a number in the sequence starting
// at 1. This class keeps track of the Test Point probe numbers and keeps from
// duplicating numbers while providing means to fill in holes in the 
// sequence. Nicer than just, say, finding the highest Test Point probe number
// and starting everything else from there.

CProbeNumberer::CProbeNumberer(CCEtoODBDoc *doc, FileStruct *file)
{
	m_doc = doc;
	m_file = file;
	m_nextProbeNumber = 1;
	m_specialNextProbeNumber = 0;


	if (file != NULL)
	{
		POSITION pos = file->getBlock()->getHeadDataInsertPosition();
		while (pos)
		{
			DataStruct *data = file->getBlock()->getNextDataInsert(pos);
			InsertStruct *insert = data->getInsert();

			if (insert->getInsertType() == insertTypeTestPoint)
			{
				CString refname = insert->getRefname();
				while (!refname.IsEmpty() && !isdigit(refname.GetAt(0)))
					refname.Delete(0,1);
				int probeNumber = atoi(refname);
				if (probeNumber > 0)
				{
					void *entityNumber;
					if (!m_reservedProbeNumbers.Lookup((void*)probeNumber, entityNumber))
						m_reservedProbeNumbers.SetAt((void*)probeNumber,(void*)data->getEntityNumber());
				}
			}
		}
	}
}

int CProbeNumberer::GetProbeNumber(bool clearSpecial)
{
	int pnum = -1; // init not valid

	if (m_specialNextProbeNumber > 0)
	{
		pnum = m_specialNextProbeNumber;
		if (clearSpecial)
			m_specialNextProbeNumber = 0;
	}
	else
	{
		void *junk;
		while (m_reservedProbeNumbers.Lookup((void*)(pnum = m_nextProbeNumber++), junk));
	}

	return pnum;
}

bool CProbeNumberer::NumberIsReservedForEntity(int probeNumber, int entityNumber)
{
	void *mappedEntityNumber;
	if (m_reservedProbeNumbers.Lookup((void*)probeNumber, mappedEntityNumber))
	{
		if (entityNumber == (int)mappedEntityNumber)
			return true;
	}
	return false;
}

void CProbeNumberer::Report(FILE *fp)
{
	if (fp != NULL)
	{
#ifdef FIRST_SHOT // report appears in random order
		fprintf(fp, "\n");
		POSITION pos = m_reservedProbeNumbers.GetStartPosition();
		while (pos)
		{
			void *probeNum;
			void *entityNum;
			m_reservedProbeNumbers.GetNextAssoc(pos, probeNum, entityNum);
			
			DataStruct *data = FindDataEntity(m_doc, (long)entityNum);

			fprintf(fp, "Probe %3d reserved for %s %s\n",
				(int)probeNum,
				insertTypeToDisplayString((insertTypeTag)(data ? data->getInsert()->getInsertType() : 0)),
				data ? data->getInsert()->getRefname() : "?");
		}
		fprintf(fp, "\n\n");
#else
		fprintf(fp, "\n");
		int count = 0;
		for (int probeNum = 1; count < m_reservedProbeNumbers.GetCount(); probeNum++)
		{
			void *entityNum;
			if (m_reservedProbeNumbers.Lookup((void*)probeNum, entityNum))
			{
				DataStruct *data = FindDataEntity(m_doc, (long)entityNum);

				fprintf(fp, "Probe %3d reserved for %s %s\n",
					(int)probeNum,
					insertTypeToDisplayString((insertTypeTag)(data ? data->getInsert()->getInsertType() : 0)),
					data ? data->getInsert()->getRefname() : "?");

				count++;
			}
		}
		fprintf(fp, "\n\n");
#endif
	}
}



