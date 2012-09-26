// $Header: /CAMCAD/5.0/ProbeStyles.cpp 35    3/12/07 12:44p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "probestyles.h"
#include "xmldomwrapper.h"
#include "fixture_out.h"
#include "graph.h"
#include "GeneralDialog.h"
#include "DeviceType.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDC_VSFLEX_PROBESTYLES			65431

#define FIRST_ROW								2

CString unitsString(int units);


/******************************************************************************
* CCEtoODBDoc::OnDftanalysisProbestylematrix
*/
void CCEtoODBDoc::OnDftanalysisProbestylematrix()
{
	FileStruct *file = getFileList().GetOnlyShown(blockTypePcb);

	if (file == NULL)
	{
		ErrorMessage("There must only be one visible file to apply probe styles.");
		return;
	}

	CProbeStylesDlg dlg(getSettings().getPageUnits(), AfxGetMainWnd());

	if (dlg.DoModal() == IDCANCEL)
		return;

	CProbeStyleList probeStyles;
	dlg.GetProbeStyles(probeStyles);

	applyProbeStyles(*file,probeStyles);
	UpdateAllViews(NULL);
}


static DeviceTypeTag getComponentDeviceType(CCEtoODBDoc *doc, DataStruct *pcbComponentData)
{
	if (pcbComponentData != NULL)
	{
		WORD devTypeKW = doc->RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);

		Attrib *attrib = NULL;
		if (pcbComponentData->getAttributesRef() && 
			pcbComponentData->getAttributesRef()->Lookup(devTypeKW, attrib))
		{
			CString typeValue = get_attvalue_string(doc, attrib);
			DeviceTypeTag type = stringToDeviceTypeTag(typeValue);
			return type;
		}
	}

	return deviceTypeUnknown;
}

static bool componentHasTestAttr(CCEtoODBDoc *doc, DataStruct *pcbComponentData)
{
	if (pcbComponentData != NULL)
	{
		WORD attrKW = doc->RegisterKeyWord(ATT_TEST, 0, VT_STRING);

		Attrib *attrib = NULL;
		if (pcbComponentData->getAttributesRef() && 
			pcbComponentData->getAttributesRef()->Lookup(attrKW, attrib))
		{
			// The attr value doesn't matter, just presence of attr matters
			return true;
		}
	}

	return false;
}

static DataStruct *findComponentData(CCEtoODBDoc *doc, CString refdes)
{
	// Was originally limited to looking for INSERT TYPE PCBCOMPONENT.
	// That was too restrictive, as it left out TEST POINT inserts, which
	// are definitely of interest. So maybe any sort of insert is of interest,
	// so long as the refname matches, no more quibbling on insert type.

	FileStruct *file = doc->getFileList().GetOnlyShown(blockTypePcb);

	POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

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

static DataStruct *findProbedPcbComponent(CCEtoODBDoc *doc, CProbe *probe)
{
	if (probe != NULL)
	{
		DataStruct *data = probe->GetProbeData();

		Attrib *attrib;

		attrib = is_attvalue(doc, data->getAttributesRef(), ATT_DDLINK, 0);
		if (attrib != NULL) 
		{
			long entityLink = attrib->getIntValue();
			if (entityLink > 0)
			{
				DataStruct *accessMarkData = FindDataEntity(doc, entityLink, NULL, NULL);//CDataList **DataList, BlockStruct **Block)
				if (accessMarkData != NULL)
				{
					attrib = is_attvalue(doc, accessMarkData->getAttributesRef(), ATT_DDLINK, 0);
					if (attrib != NULL)
					{
						entityLink = attrib->getIntValue();
						if (entityLink > 0)
						{
							// We only care about getting at PCB components. If access marker
							// is on via or something else, we don't want it anyway.
							// So we can restrict search to comp pins here.
							CompPinStruct *compPin = FindCompPinEntity(doc, entityLink, NULL, NULL);
							if (compPin != NULL)
							{
								CString refdes = compPin->getRefDes();
								DataStruct *pcbCompData = findComponentData(doc, refdes);
								return pcbCompData;
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}

void CCEtoODBDoc::applyProbeStyles(FileStruct& fileStruct,CProbeStyleList& probeStyles)
{
   CCEtoODBDoc& doc = *this;

	BlockStruct *fileBlock = fileStruct.getBlock();
	if (fileBlock == NULL)
		return;

	CMapStringToInt styleMap;
	CString reportFileName = GetLogfilePath("ProbeStylesReport.txt");
	FILE *reportfp = fopen(reportFileName, "wt");
	if (reportfp)
		fprintf(reportfp, "Probe Styles Report\n\n");

	// gather the probes & reapply original test probe blocks
	CFixtureProbeList probeList;
	CUnits curUnit(doc.getSettings().getPageUnits());
	WORD psKw = (WORD)doc.getStandardAttributeKeywordIndex(standardAttributeProbeStyle);
	for (POSITION pos=fileBlock->getHeadDataInsertPosition(); pos!=NULL; fileBlock->getNextDataInsert(pos))
	{
		DataStruct *data = fileBlock->getAtData(pos);
		InsertStruct *insert = data->getInsert();

		if (insert->getInsertType() != insertTypeTestProbe)
			continue;

		// set the original probe block
		BlockStruct *probeBlock = doc.getBlockAt(insert->getBlockNumber());
		if (!probeBlock)
			continue;

		CString blockName = probeBlock->getName();
		CString origBlockName = blockName.Mid(0, blockName.ReverseFind('_'));

		probeBlock = Graph_Block_Exists(&doc, origBlockName, -1, blockTypeTestProbe);
		if (probeBlock != NULL)
			insert->setBlockNumber(probeBlock->getBlockNumber());

		probeList.AddTail(new CProbe(&doc, &fileStruct, data, CTMatrix()));
	}

	// remove all the existing test probe blocks previously created by applying probe styles
	for (int cnt=0; cnt<doc.getMaxBlockIndex(); cnt++)
	{
		BlockStruct *block = doc.getBlockAt(cnt);

		if (block == NULL)
			continue;

		if (block->getBlockType() != blockTypeTestProbe)
			continue;

		CString blockName = block->getName();
		CString blockProbeSurface = blockName.Mid(blockName.ReverseFind('_')+1);
		if (blockName.ReverseFind('_') < 0 || blockProbeSurface == "Top" || blockProbeSurface == "Bot")
			continue;

		doc.RemoveBlock(block);
	}

	// create the new blocks and make sure we set the inserted block on all probes
	for (POSITION pos=probeList.GetHeadPosition(); pos!=NULL; probeList.GetNext(pos))
	{
		CProbe *probe = (CProbe*)probeList.GetAt(pos);
		DataStruct *data = probe->GetProbeData();
		InsertStruct *insert = data->getInsert();
		BlockStruct *probeBlock = doc.getBlockAt(insert->getBlockNumber());
		if (!probeBlock)
			continue;

		InsertTypeTag probedComponentInsertType = insertTypeUnknown;
		CString probedComponentTechnology = probe->GetTechnology();

		// Get the probed component
		DataStruct *probedComponent = probe->GetProbedComponent();
		if (probedComponent == NULL)
		{
			CompPinStruct *compPin = probe->GetProbedCompPin();
			if (compPin != NULL)
			{
				CString refdes = compPin->getRefDes();
				probedComponent = findComponentData(&doc, refdes);
			}
		}

		// Get some info from the probed component
		if (probedComponent)
		{
			probedComponentInsertType = probedComponent->getInsert()->getInsertType();

			// CFixtureProbe did an incomplete job of determining technology.
			// Maybe it was complete enough for "fixture out" (and not knowing for sure is
			// why I didn't change it) but it was not complete enough for purposes here.
			// In particular, it gets technology only for things that are insert type
			// pcb_component. That leaves out insert type TESTPOINT ! which is rather
			// significant here.
			WORD techKW = doc.RegisterKeyWord(ATT_TECHNOLOGY, 0, valueTypeString);
			Attrib *attrib = NULL;
			if (probedComponent->lookUpAttrib(techKW, attrib))
			{
				probedComponentTechnology = get_attvalue_string(&doc, attrib);
			}
		}

		DeviceTypeTag devType = getComponentDeviceType(&doc, probedComponent);	

		CProbeStyle *pStyle = probeStyles.FindMatchingProfile(probedComponentTechnology,
																probe->IsOnTop() ? testSurfaceTop : testSurfaceBottom,
																curUnit.convertTo(pageUnitsInches, probe->GetFeatureDrillSize()),
																probe->GetTemplateName(),
																curUnit.convertTo(pageUnitsInches, probe->GetExposedMetalDiameter()),
																probe->GetTargetType(), 
																// coerce anything with TEST attrib present to insertTypeTestPoint, regardless of what is really is
																componentHasTestAttr(&doc, probedComponent) ? insertTypeTestPoint : probedComponentInsertType,
																devType );

		// set a probe style attribute
		CString probeStyle = (pStyle!=NULL)?pStyle->GetName():"";
      data->setAttrib(doc.getCamCadData(), psKw, valueTypeString, (void*)probeStyle.GetBuffer(0), attributeUpdateOverwrite, NULL);

		EProbeShape shape = (pStyle!=NULL)?pStyle->GetShape():probeShapeDefaultTarget;
		
		CString pName = probeBlock->getName ();
		CString newProbeName;
		if (pName != "" )
		{
			if (probeStyle != "" )			
				newProbeName = pName + "_" + probeStyle;
			else
				newProbeName = pName;

			BlockStruct *newBlock = CreateTestProbeGeometry(&doc, newProbeName, probe->GetDrillSize(), newProbeName, probe->GetDrillSize(), shape);
			if (newBlock != NULL)
			{
				insert->setBlockNumber(newBlock->getBlockNumber());

				if (!probeStyle.IsEmpty())
				{
					int count = 0;
					styleMap.Lookup(probeStyle, count); // if it's there, fine, if not, fine too
					count++;
					styleMap.SetAt(probeStyle, count);
				}
			}
			data->setColorOverride(FALSE);
			if (pStyle != NULL)
			{
				data->setOverrideColor(pStyle->GetColor());
				data->setColorOverride(TRUE);
			}
		}
	}

	if (reportfp)
	{
		if (styleMap.GetCount() > 0)
		{
			// Find longest probe style name for report formatting
			int len = 0;
			POSITION styleMapPos = styleMap.GetStartPosition();
			while (styleMapPos)
			{
				CString styleName;
				int count;
				styleMap.GetNextAssoc(styleMapPos, styleName, count);
				if (styleName.GetLength() > len)
					len = styleName.GetLength();
			}
			// Write report
			CString underline = "-----------------------------------------------------------";
			len = max(len, (int)strlen("Style Name"));
			fprintf(reportfp, "%-*s   %s\n", len, "Style Name", "Count");
			fprintf(reportfp, "%-*.*s   %.5s\n", len, len, underline, underline);
			styleMapPos = styleMap.GetStartPosition();
			while (styleMapPos)
			{
				CString styleName;
				int count;
				styleMap.GetNextAssoc(styleMapPos, styleName, count);
				fprintf(reportfp, "%-*s   %5d\n", len, styleName, count);
			}
			fprintf(reportfp, "\n");
		}
		else
		{
			fprintf(reportfp, "No probe styles assigned.\n");
		}
		
		fclose(reportfp);
	}
}

/******************************************************************************
* CProbeStyle::CProbeStyle
*/
CProbeStyle::CProbeStyle() : CObject()
	, m_bUse(false)
	, m_rDrillDiameterRange(0.0, SMALLNUMBER)
	, m_rExposedMetalDiameterRange(0.0, SMALLNUMBER)
	, m_eSurface(testSurfaceTop)
	, m_eShape(probeShapeDefaultTarget)
	, m_color(RGB(255, 0, 0))
{
}

CProbeStyle::CProbeStyle(const CProbeStyle& other)
{
	if (this != &other)
	{
		m_bUse = other.m_bUse;
		m_sName = other.m_sName;
		m_sTechnology = other.m_sTechnology;
		m_eSurface = other.m_eSurface;
		m_rDrillDiameterRange = other.m_rDrillDiameterRange;
		m_sProbeName = other.m_sProbeName;
		m_rExposedMetalDiameterRange = other.m_rExposedMetalDiameterRange;
		m_sTargetType = other.m_sTargetType;
		m_eShape = other.m_eShape;
		m_color = other.m_color;
	}
}

CString CProbeStyle::GetSurfaceString() const
{
	CString surface;
	if (m_eSurface == testSurfaceTop)
		surface = "TOP";
	else if (m_eSurface == testSurfaceBottom)
		surface = "BOTTOM";
	else if (m_eSurface == testSurfaceBoth)
		surface = "BOTH";

	return surface;
}

void CProbeStyle::SetSurfaceString(CString surface)
{
	if (!surface.CompareNoCase("TOP"))
		m_eSurface = testSurfaceTop;
	else if (!surface.CompareNoCase("BOTTOM"))
		m_eSurface = testSurfaceBottom;
	else if (!surface.CompareNoCase("BOTH"))
		m_eSurface = testSurfaceBoth;
}

void CProbeStyle::SetShape(EProbeShape shape)
{
	if (shape < probeShapeMIN || shape >= probeShapeMAX)
		return;

	m_eShape = shape;
}

bool CProbeStyle::IsMatchingProfile(CString technology, ETestSurface surface, double drillDiameter, CString probeName, double exposedMetalDiameter,
												CString targetTypeCSV, InsertTypeTag insertType, DeviceTypeTag deviceType)
{
	if (!m_bUse)
		return false;

	if (technology.IsEmpty() || m_sTechnology.IsEmpty() || m_sTechnology.Find(technology) < 0)
		return false;

	if (surface != testSurfaceUnset && !(surface == m_eSurface || m_eSurface == testSurfaceBoth))
		return false;
	
	if (drillDiameter > 0.0 && !m_rDrillDiameterRange.IsInRange(drillDiameter))
		return false;

#define ORIGINAL_STYLE_NAME_CHECK
#ifdef  ORIGINAL_STYLE_NAME_CHECK
	if (probeName.IsEmpty() || (!m_sProbeName.IsEmpty() && probeName != m_sProbeName))
		return false;
#else
	// If m_sProbeName is empty, then it doesn't matter what probeName is,
	// including also empty.  In fact, also empty would be a match!
	// BUT probeName is not supposed to be empty, this would indicate that the
	// probe template name is not set.
	if (!m_sProbeName.IsEmpty() && probeName != m_sProbeName)
		return false;
#endif

	if (exposedMetalDiameter > 0.0 && !m_rExposedMetalDiameterRange.IsInRange(exposedMetalDiameter))
		return false;

	CString targetTypes = m_sTargetType.MakeUpper();
	targetTypeCSV = targetTypeCSV.MakeUpper();
	if (targetTypeCSV.IsEmpty() || targetTypes.IsEmpty())
		return false;

	int tokPos = 0;
	bool targetFound = false;
	CString targetType = targetTypeCSV.Tokenize(", ", tokPos);

	// Case 1577, make target type testing more strict, the 
	// following is "too flexible", and causes an order-dependence when
	// seeking a probe style profile match. E.g. a probe on a via with
	// attribute "TEST" would be matched by a preceding "VIA" rule, due
	// "catch all" target type testing further below. Essentially, such an
	// item has two target types: the real underlying item's true type and
	// the type coerced by presence of the TEST attribute.
	// NOTE that any item with the TEST attribute present has been coerced to be
	// "insertTypeTestPoint" in the call to this function, regardless of what
	// insertType it really is.

	//if (targetTypes.Find("TESTPOINT") >= 0)
	//	targetFound = (insertType == insertTypeTestPoint);
	//if (targetFound) return true;
	if (insertType == insertTypeTestPoint)
		return (targetTypes.Find("TESTPOINT") >= 0);

	// Perhaps these tests should also be more strict, but there are no complaints
	// involving these, so they've been left as-is.

	if (targetTypes.Find("CONNECTOR") >= 0)
		targetFound = (deviceType == deviceTypeConnector);

	if (targetFound) return true;

	if (targetTypes.Find("VIA") >= 0)
		targetFound = (insertType == insertTypeVia);

	if (targetFound) return true;

	if (targetTypes.Find("COMPONENT") >= 0)
		targetFound = (insertType == insertTypePcbComponent);

	if (targetFound) return true;

	// if THRU or SMD is encountered, within probe styles, this is a COMP
	if (targetType == "THRU" || targetType == "SMD")
		targetType = "COMPONENT";

	while (!targetType.IsEmpty() && !targetFound)
	{
		targetFound = (targetTypes.Find(targetType) >= 0);
		targetType = targetTypeCSV.Tokenize(",", tokPos);

		// if THRU or SMD is encountered, within probe styles, this is a COMP
		if (targetType == "THRU" || targetType == "SMD")
			targetType = "COMPONENT";
	}

	return targetFound;
}

void CProbeStyle::Dump(CStreamFileWriteFormat &file)
{
	file.writef("Use                    : %s\n", m_bUse?"True":"False");
	file.writef("Name                   : %s\n", m_sName);
	file.writef("Technology             : %s\n", m_sTechnology);
	file.writef("Surface                : %s\n", GetSurfaceString());
	file.writef("Drill Diameter         : %0.3f to %0.3f\n", m_rDrillDiameterRange.GetMin(), m_rDrillDiameterRange.GetMax());
	file.writef("Probe Name             : %s\n", m_sProbeName);
	file.writef("Exposed Metal Diameter : %0.3f to %0.3f\n", m_rExposedMetalDiameterRange.GetMin(), m_rExposedMetalDiameterRange.GetMax());
	file.writef("Target Type            : %s\n", m_sTargetType);
	file.writef("Color                  : %i\n", m_color);
	file.writef("Shape                  : %i\n", m_eShape);
}



/******************************************************************************
* CProbeStyleList::FindMatchingProfile
*/
CProbeStyle *CProbeStyleList::FindMatchingProfile(CString technology, ETestSurface surface, double drillDiameter, CString probeName,
																  double exposedMetalDiameter, CString targetType, InsertTypeTag insertType, DeviceTypeTag deviceType)
{
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{
		CProbeStyle *pStyle = GetAt(pos);

		if (pStyle->IsMatchingProfile(technology, surface, drillDiameter, probeName, exposedMetalDiameter, targetType, insertType, deviceType))
			return pStyle;
	}

	return NULL;
}

void CProbeStyleList::Dump(const CString &filename)
{
	FILE *file = fopen(filename, "w");
	if (file == NULL)
		return;

	CStreamFileWriteFormat writeFormat(file, 1024);
	writeFormat.setNewLineMode(true);

	int count = 0;
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{
		CProbeStyle *pStyle = GetAt(pos);

		writeFormat.writef("Style %d\n", ++count);
		
		writeFormat.pushHeader("   ");
		pStyle->Dump(writeFormat);
		writeFormat.writef("\n");
		writeFormat.popHeader();
	}

	fclose(file);
}

void CProbeStyleList::Save(const CString &filename)
{
	FILE *file = fopen(filename, "w");
	if (file == NULL)
		return;

	CStreamFileWriteFormat writeFormat(file, 1024);
	writeFormat.setNewLineMode(true);

	Save(writeFormat);

	fclose(file);
}

void CProbeStyleList::Save(CStreamFileWriteFormat &writeFormat)
{
	writeFormat.writef("<ProbeStyleList Name=\"%s\">\n", m_sStyleListName);

	writeFormat.pushHeader("   ");
	int count = 0;
	for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
	{
		CProbeStyle *pStyle = GetAt(pos);
		CString buf;

		writeFormat.writef("<ProbeStyle Use=\"%s\"", pStyle->IsUsed()?"1":"0");
		writeFormat.writef(" Name=\"%s\"", SwapSpecialCharacters(pStyle->GetName()));
		writeFormat.writef(" Technology=\"%s\"", SwapSpecialCharacters(pStyle->GetTechnology()));
		writeFormat.writef(" Surface=\"%s\"", SwapSpecialCharacters(pStyle->GetSurfaceString()));

		// Case 1577, was saving values only to 1 mil (%0.3f) accuracy.
		// Not enough to keep 3 decimal places of accuracy for mm.
		// Users have option to goiong to 6 decimal places accuracy for mm.
		// That means we need to save at least 10 decimal places in inches.
		// i.e. .000001 mm == .0000000393 inches.
		buf.Empty();
		if (pStyle->GetDrillRange().GetMin() > (-DBL_MAX+10.))
			buf.Format("%0.10f", pStyle->GetDrillRange().GetMin());
		writeFormat.writef(" DrillDiameterMin=\"%s\"", buf);

		buf.Empty();
		if (pStyle->GetDrillRange().GetMax() < (DBL_MAX-10.))
			buf.Format("%0.10f", pStyle->GetDrillRange().GetMax());
		writeFormat.writef(" DrillDiameterMax=\"%s\"", buf);
		writeFormat.writef(" ProbeName=\"%s\"", SwapSpecialCharacters(pStyle->GetProbeName()));

		writeFormat.writef(" Color=\"%d\"", pStyle->GetColor());
		writeFormat.writef(" Shape=\"%d\"", pStyle->GetShape());

		buf.Empty();
		if (pStyle->GetExposedRange().GetMin() > (-DBL_MAX+10.))
			buf.Format("%0.10f", pStyle->GetExposedRange().GetMin());
		writeFormat.writef(" ExposedMetalDiameterMin=\"%s\"", buf);

		buf.Empty();
		if (pStyle->GetExposedRange().GetMax() < (DBL_MAX+10.))
			buf.Format("%0.10f", pStyle->GetExposedRange().GetMax());
		writeFormat.writef(" ExposedMetalDiameterMax=\"%s\"", buf);

		writeFormat.writef(" TargetType=\"%s\" />\n", SwapSpecialCharacters(pStyle->GetTargetType()));
	}
	writeFormat.popHeader();

	writeFormat.writef("</ProbeStyleList>\n");
}

void CProbeStyleList::Load(const CString &filename)
{
	CXMLDocument xmlDoc;
	if (!xmlDoc.LoadXMLFile(filename))
		return;

	CXMLNodeList *listNode = xmlDoc.GetElementsByTagName("ProbeStyleList");
	if (listNode == NULL)
		return;

	listNode->Reset();
	while (CXMLNode *styleListNode = listNode->NextNode())
	{
		CString buf;
		if (styleListNode->GetAttrValue("Name", buf))
			m_sStyleListName = buf;
		
		styleListNode->ResetChildList();
		while (CXMLNode *styleNode = styleListNode->NextChild())
		{
			CProbeStyle *pStyle = new CProbeStyle();

			if (styleNode->GetAttrValue("Use", buf))
				pStyle->SetUse(atoi(buf)?true:false);
			if (styleNode->GetAttrValue("Name", buf))
				pStyle->SetName(buf);
			if (styleNode->GetAttrValue("Technology", buf))
				pStyle->SetTechnology(buf);
			if (styleNode->GetAttrValue("Surface", buf))
				pStyle->SetSurfaceString(buf);
			if (styleNode->GetAttrValue("DrillDiameterMin", buf))
				pStyle->SetDrillMinRange(buf);
			if (styleNode->GetAttrValue("DrillDiameterMax", buf))
				pStyle->SetDrillMaxRange(buf);
			if (styleNode->GetAttrValue("ProbeName", buf))
				pStyle->SetProbeName(buf);
			if (styleNode->GetAttrValue("Color", buf))
				pStyle->SetColor(atoi(buf));
			if (styleNode->GetAttrValue("Shape", buf))
				pStyle->SetShape((EProbeShape)atoi(buf));
			if (styleNode->GetAttrValue("ExposedMetalDiameterMin", buf))
				pStyle->SetExposedMinRange(buf);
			if (styleNode->GetAttrValue("ExposedMetalDiameterMax", buf))
				pStyle->SetExposedMaxRange(buf);
			if (styleNode->GetAttrValue("TargetType", buf))
				pStyle->SetTargetType(buf);

			AddTail(pStyle);
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
// CProbeShapeColorDialog dialog
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CProbeShapeColorDialog, CDialog)
CProbeShapeColorDialog::CProbeShapeColorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProbeShapeColorDialog::IDD, pParent)
	, m_centerStyle(FALSE)
{
}

CProbeShapeColorDialog::~CProbeShapeColorDialog()
{
}

void CProbeShapeColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHAPE_COMBO, m_cmbShape);
	DDX_Control(pDX, IDC_COLOR, m_probeColorBtn);
	DDX_Radio(pDX, IDC_CENTERSTYLE_RADIO, m_centerStyle);
}


BEGIN_MESSAGE_MAP(CProbeShapeColorDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_SHAPE_COMBO, OnCbnSelchangeShapeCombo)
END_MESSAGE_MAP()


// CProbeShapeColorDialog message handlers

BOOL CProbeShapeColorDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cmbShape.Clear();
	m_cmbShape.AddString("Standard Target");
	m_cmbShape.AddString("Round");
	m_cmbShape.AddString("Square");
	m_cmbShape.AddString("Diamond");
	m_cmbShape.AddString("Hexagon");
	m_cmbShape.AddString("Hourglass");



	if (m_eShape == probeShapeDefaultTarget)
	{
		m_cmbShape.SetCurSel(0);
		m_centerStyle = 0;
	}
	else if (m_eShape == probeShapeRoundCross)
	{
		m_cmbShape.SetCurSel(1);
		m_centerStyle = 0;
	}
	else if (m_eShape == probeShapeRoundX)
	{
		m_cmbShape.SetCurSel(1);
		m_centerStyle = 1;
	}
	else if (m_eShape == probeShapeSquareCross)
	{
		m_cmbShape.SetCurSel(2);
		m_centerStyle = 0;
	}
	else if (m_eShape == probeShapeSquareX)
	{
		m_cmbShape.SetCurSel(2);
		m_centerStyle = 1;
	}
	else if (m_eShape == probeShapeDiamondCross)
	{
		m_cmbShape.SetCurSel(3);
		m_centerStyle = 0;
	}
	else if (m_eShape == probeShapeDiamondX)
	{
		m_cmbShape.SetCurSel(3);
		m_centerStyle = 1;
	}
	else if (m_eShape == probeShapeHexagonCross)
	{
		m_cmbShape.SetCurSel(4);
		m_centerStyle = 0;
	}
	else if (m_eShape == probeShapeHexagonX)
	{
		m_cmbShape.SetCurSel(4);
		m_centerStyle = 1;
	}
	else if (m_eShape == probeShapeHourglass)
	{
		m_cmbShape.SetCurSel(5);
		m_centerStyle = 0;
	}

	m_probeColorBtn.tempColor = m_color;

	UpdateData(FALSE);
	OnCbnSelchangeShapeCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProbeShapeColorDialog::OnOK()
{
	UpdateData();

	m_color = m_probeColorBtn.tempColor;

	switch (m_cmbShape.GetCurSel())
	{
	case 0:		// Standard Target
		m_eShape = probeShapeDefaultTarget;
		break;
	case 1:		// Round
		if (m_centerStyle == 0)		// +
			m_eShape = probeShapeRoundCross;
		else if (m_centerStyle == 1)		// X
			m_eShape = probeShapeRoundX;
		break;
	case 2:		// Square
		if (m_centerStyle == 0)		// +
			m_eShape = probeShapeSquareCross;
		else if (m_centerStyle == 1)		// X
			m_eShape = probeShapeSquareX;
		break;
	case 3:		// Diamond
		if (m_centerStyle == 0)		// +
			m_eShape = probeShapeDiamondCross;
		else if (m_centerStyle == 1)		// X
			m_eShape = probeShapeDiamondX;
		break;
	case 4:		// Hexagon
		if (m_centerStyle == 0)		// +
			m_eShape = probeShapeHexagonCross;
		else if (m_centerStyle == 1)		// X
			m_eShape = probeShapeHexagonX;
		break;
	case 5:		// Hour glass
		m_eShape = probeShapeHourglass;
		break;

	}


	CDialog::OnOK();
}

void CProbeShapeColorDialog::OnCbnSelchangeShapeCombo()
{
	int curSel = m_cmbShape.GetCurSel();

	GetDlgItem(IDC_CENTERSTYLE_RADIO)->EnableWindow(curSel != 0 && curSel != 5);
	GetDlgItem(IDC_CENTERSTYLE_RADIO2)->EnableWindow(curSel != 0 && curSel != 5);
	GetDlgItem(IDC_COLOR)->EnableWindow(curSel != 0);
}



/////////////////////////////////////////////////////////////////////////////////////////
// CProbeStylesDlg dialog
//IMPLEMENT_DYNAMIC(CProbeStylesDlg, CResizingDialog)
CProbeStylesDlg::CProbeStylesDlg(PageUnitsTag curPageUnits, CWnd* pParent /*=NULL*/)
	: CResizingDialog(CProbeStylesDlg::IDD, pParent)
	, m_curPageUnits(curPageUnits)
{
   addFieldControl(IDC_ProbeStylesGridStatic, anchorBottomRight, growBoth);
   addFieldControl(IDC_ADD, anchorRight);
   addFieldControl(IDC_REMOVE, anchorRight);
   addFieldControl(IDC_MOVE_UP, anchorRight);
   addFieldControl(IDC_MOVE_DOWN, anchorRight);
   addFieldControl(IDOK, anchorBottomRight);
   addFieldControl(IDCANCEL, anchorBottomRight);
}

CProbeStylesDlg::~CProbeStylesDlg()
{
}

void CProbeStylesDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProbeStylesDlg, CResizingDialog)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_MOVE_UP, OnBnClickedMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnBnClickedMoveDown)
	ON_WM_SIZING()
END_MESSAGE_MAP()


// CProbeStylesDlg message handlers

BOOL CProbeStylesDlg::OnInitDialog()
{
	CResizingDialog::OnInitDialog();

   m_probeStylesGrid.AttachGrid(this, IDC_ProbeStylesGridStatic);
   UpdateGridColumnSizes();


	m_lProbeStyles.Load(getApp().getCamcadExeFolderPath() + "probestyles.dat");
	updateGridWithProbeStyles();

	CString title;
	title.Format("Probe Styles in %s", unitsString(m_curPageUnits.GetPageUnits()));
	SetWindowText(title);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProbeStylesDlg::UpdateGridColumnSizes()
{
   CRect rcGridWindow;
   GetDlgItem( IDC_ProbeStylesGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_probeStylesGrid.SizeToFit(rcGridWindow);
}

void CProbeStylesDlg::updateGridWithProbeStyles()
{
	for (POSITION pos=m_lProbeStyles.GetHeadPosition(); pos!=NULL; m_lProbeStyles.GetNext(pos))
	{
		CProbeStyle *pStyle = m_lProbeStyles.GetAt(pos);
		addStyleToGrid(pStyle);
	}
}

void CProbeStylesDlg::addStyleToGrid(bool use, CString name, CString technology, CString surface, CString drillMin, CString drillMax, CString probeName, EProbeShape shape, COLORREF color, CString exposedMin, CString exposedMax, CString targetType, long atRow)
{
   int rowIndx = atRow;
   if (rowIndx < 0)
   {
      rowIndx = m_probeStylesGrid.GetNumberRows();
      m_probeStylesGrid.SetNumberRows(rowIndx+1);
   }

   CString shapeAndColor;
   CProbeStyleShapeCellType::EncodeVal(shape, color, shapeAndColor);

   m_probeStylesGrid.QuickSetBool(probeStyleColumnUse,             rowIndx, use);
   m_probeStylesGrid.QuickSetText(probeStyleColumnName,            rowIndx, name);
   m_probeStylesGrid.QuickSetText(probeStyleColumnProbeName,       rowIndx, probeName);
   m_probeStylesGrid.QuickSetText(probeStyleColumnShapeColor,      rowIndx, shapeAndColor);
   m_probeStylesGrid.QuickSetText(probeStyleColumnTechnology,      rowIndx, technology);
   m_probeStylesGrid.QuickSetText(probeStyleColumnSide,            rowIndx, surface);
   m_probeStylesGrid.QuickSetText(probeStyleColumnDrillMin,        rowIndx, drillMin);
   m_probeStylesGrid.QuickSetText(probeStyleColumnDrillMax,        rowIndx, drillMax);
   m_probeStylesGrid.QuickSetText(probeStyleColumnExposedMetalMin, rowIndx, exposedMin);
   m_probeStylesGrid.QuickSetText(probeStyleColumnExposedMetalMax, rowIndx, exposedMax);
   m_probeStylesGrid.QuickSetText(probeStyleColumnTargetType,      rowIndx, targetType);

   COLORREF backColor = RGB(255, 255, 255);
   m_probeStylesGrid.SetRowBackColor(rowIndx, backColor);


}

void CProbeStylesDlg::addStyleToGrid(bool use, CString name, CString technology, CString surface, double drillMin, double drillMax, CString probeName, EProbeShape shape, COLORREF color, double exposedMin, double exposedMax, CString targetType, long atRow)
{
	int decimals = GetDecimals(m_curPageUnits.GetPageUnits());
	CString sDrillMin, sDrillMax, sExposedMin, sExposedMax;

	// if the minimum range is significantly (10) greater than the smallest possible double
	if (drillMin > (-DBL_MAX+10.))
		sDrillMin.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, drillMin));

	// if the maximum range is significantly (10) less than the largest possible double
	if (drillMax < (DBL_MAX-10.))
		sDrillMax.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, drillMax));

	// if the minimum range is significantly (10) greater than the smallest possible double
	if (exposedMin > (-DBL_MAX+10.))
		sExposedMin.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, exposedMin));

	// if the maximum range is significantly (10) less than the largest possible double
	if (exposedMax < (DBL_MAX-10.))
		sExposedMax.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, exposedMax));

   addStyleToGrid(use, name, technology, surface, sDrillMin, sDrillMax, probeName, shape, color, sExposedMin, sExposedMax,targetType, atRow);

}

void CProbeStylesDlg::addStyleToGrid(CProbeStyle *probeStyle, long atRow)
{
	int decimals = GetDecimals(m_curPageUnits.GetPageUnits());
	CString drillMin, drillMax, exposedMin, exposedMax;

	// if the minimum range is significantly (10) greater than the smallest possible double
	if (probeStyle->GetDrillRange().GetMin() > (-DBL_MAX+10.))
		drillMin.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, probeStyle->GetDrillRange().GetMin()));

	// if the maximum range is significantly (10) less than the largest possible double
	if (probeStyle->GetDrillRange().GetMax() < (DBL_MAX-10.))
		drillMax.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, probeStyle->GetDrillRange().GetMax()));

	// if the minimum range is significantly (10) greater than the smallest possible double
	if (probeStyle->GetExposedRange().GetMin() > (-DBL_MAX+10.))
		exposedMin.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, probeStyle->GetExposedRange().GetMin()));

	// if the maximum range is significantly (10) less than the largest possible double
	if (probeStyle->GetExposedRange().GetMax() < (DBL_MAX-10.))
		exposedMax.Format("%.*lf", decimals, m_curPageUnits.convertFrom(pageUnitsInches, probeStyle->GetExposedRange().GetMax()));

   addStyleToGrid(probeStyle->IsUsed(), probeStyle->GetName(), probeStyle->GetTechnology(), probeStyle->GetSurfaceString(), 
      drillMin, drillMax, probeStyle->GetProbeName(), probeStyle->GetShape(), probeStyle->GetColor(), exposedMin, exposedMax, 
      probeStyle->GetTargetType(), atRow);

}

void CProbeStylesDlg::OnBnClickedAdd()
{
   int nextRowIndx = m_probeStylesGrid.GetNumberRows();

	CString styleName, probeName;
	styleName.Format("ProbeStyle%d", nextRowIndx);
	probeName.Format("Probe%d", nextRowIndx);
	addStyleToGrid(true, styleName, "SMD&THRU", "BOTH",  "", "", probeName, probeShapeDefaultTarget, RGB(255, 0, 0), "", "", "");
   m_probeStylesGrid.RedrawAll();
}

void CProbeStylesDlg::OnBnClickedRemove()
{
   int rowIndx = m_probeStylesGrid.GetSelectedRow();
   if (rowIndx > -1)
   {
      m_probeStylesGrid.DeleteRow(rowIndx);
      m_probeStylesGrid.RedrawAll();
      m_probeStylesGrid.ClearSelections();
   }
}

void CProbeStylesDlg::MoveRow(int directionFlag)
{
   // Use UG_LINEUP and UG_LINEDOWN for direction flags

   int selectedRow = m_probeStylesGrid.GetSelectedRow();
   
   // Can't move first row up or last row down
   int limitRow = (directionFlag == UG_LINEUP) ? 0 : (m_probeStylesGrid.GetNumberRows() - 1);
   bool moveAllowed = (directionFlag == UG_LINEUP) ? (selectedRow > limitRow) : (selectedRow < limitRow);

   if (moveAllowed)
   {
      int otherRow = (directionFlag == UG_LINEUP) ? (selectedRow - 1) : (selectedRow + 1);
      int selectedCol = m_probeStylesGrid.GetSelectedCol();

      bool use1;
      CString styleName1, probeName1, technology1, surface1; 
      EProbeShape shape1;
      COLORREF color1;
      CString drillMin1, drillMax1, exposedMin1; 
      CString exposedMax1, targetType1;

      m_probeStylesGrid.GetRowValues( otherRow , use1, styleName1, probeName1, technology1, surface1, 
         shape1, color1, drillMin1, drillMax1, exposedMin1, 
         exposedMax1, targetType1);

      bool use2;
      CString styleName2, probeName2, technology2, surface2; 
      EProbeShape shape2;
      COLORREF color2;
      CString drillMin2, drillMax2, exposedMin2; 
      CString exposedMax2, targetType2;

      m_probeStylesGrid.GetRowValues( selectedRow , use2, styleName2, probeName2, technology2, surface2, 
         shape2, color2, drillMin2, drillMax2, exposedMin2, 
         exposedMax2, targetType2);

      addStyleToGrid(use2, styleName2, technology2, surface2, drillMin2, drillMax2, probeName2, shape2, color2, exposedMin2, exposedMax2, targetType2,
         otherRow);

      addStyleToGrid(use1, styleName1, technology1, surface1, drillMin1, drillMax1, probeName1, shape1, color1, exposedMin1, exposedMax1, targetType1,
         selectedRow);

      m_probeStylesGrid.MoveCurrentRow(directionFlag);

      m_probeStylesGrid.RedrawRow(otherRow       );
      m_probeStylesGrid.RedrawRow(selectedRow    );
   }

}

void CProbeStylesDlg::OnBnClickedMoveUp()
{
   MoveRow(UG_LINEUP);
}

void CProbeStylesDlg::OnBnClickedMoveDown()
{
   MoveRow(UG_LINEDOWN);
}
	
void CProbeStylesDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CResizingDialog::OnSizing(fwSide, pRect);

	UpdateGridColumnSizes();
}

BEGIN_EVENTSINK_MAP(CProbeStylesDlg, CResizingDialog)
END_EVENTSINK_MAP()


void CProbeStylesDlg::OnOK()
{
	m_lProbeStyles.empty();

	for (int rowIndx = 0; rowIndx < m_probeStylesGrid.GetNumberRows(); rowIndx++)
	{
      bool use;
      CString styleName, probeName, technology, surface; 
      EProbeShape shape;
      COLORREF color;
      CString drillMin, drillMax, exposedMin; 
      CString exposedMax, targetType;

      m_probeStylesGrid.GetRowValues( rowIndx , use, styleName, probeName, technology, surface, 
         shape, color, drillMin, drillMax, exposedMin, exposedMax, targetType);

		CProbeStyle *ps = new CProbeStyle();

		ps->SetUse(use);
		ps->SetName(styleName);
      ps->SetProbeName(probeName);
      ps->SetTechnology(technology);

		if (surface == "TOP")
			ps->SetSurface(testSurfaceTop);
		else if (surface == "BOTTOM")
			ps->SetSurface(testSurfaceBottom);
		else if (surface == "BOTH")
			ps->SetSurface(testSurfaceBoth);

		ps->SetDrillMinRange(drillMin.IsEmpty()?-DBL_MAX:m_curPageUnits.convertTo(pageUnitsInches, atof(drillMin)));
		ps->SetDrillMaxRange(drillMax.IsEmpty()? DBL_MAX:m_curPageUnits.convertTo(pageUnitsInches, atof(drillMax)));

		ps->SetShape(shape);
		ps->SetColor(color);

		ps->SetExposedMinRange(exposedMin.IsEmpty()?-DBL_MAX:m_curPageUnits.convertTo(pageUnitsInches, atof(exposedMin)));
		ps->SetExposedMaxRange(exposedMax.IsEmpty()? DBL_MAX:m_curPageUnits.convertTo(pageUnitsInches, atof(exposedMax)));

		ps->SetTargetType(targetType);

		m_lProbeStyles.AddTail(ps);
	}


	FILE *file = fopen(getApp().getCamcadExeFolderPath() + "probestyles.dat", "w");
	if (file == NULL)
		return;

	CStreamFileWriteFormat writeFormat(file, 1024);
	writeFormat.setNewLineMode(true);

	writeFormat.writef("<ProbeStyleConfig>\n");

	writeFormat.pushHeader("   ");
	m_lProbeStyles.Save(writeFormat);
	writeFormat.popHeader();

	writeFormat.writef("</ProbeStyleConfig>\n");
	fclose(file);

	CResizingDialog::OnOK();
}

void CProbeStylesDlg::GetProbeStyles(CProbeStyleList &probeStyles)
{
	for (POSITION pos=m_lProbeStyles.GetHeadPosition(); pos!=NULL; m_lProbeStyles.GetNext(pos))
		probeStyles.AddTail(new CProbeStyle(*m_lProbeStyles.GetAt(pos)));
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CProbeStylesGrid::CProbeStylesGrid()
{
   SetSortEnabled(false);  // Turn off grid sort on heading left click
}

//----------------------------------------------------------------------------------

void CProbeStylesGrid::SizeToFit(CRect &rect)
{
   int width = rect.Width();

   // The constant factors for size here are essentially percentages of
   // overall width for given column.

   SetColWidth(probeStyleColumnUse,             (int)(width * 0.05));
   SetColWidth(probeStyleColumnName,            (int)(width * 0.14));
   SetColWidth(probeStyleColumnProbeName,       (int)(width * 0.10));
   SetColWidth(probeStyleColumnShapeColor,      (int)(width * 0.10));
   SetColWidth(probeStyleColumnTechnology,      (int)(width * 0.08));  
   SetColWidth(probeStyleColumnSide,            (int)(width * 0.08));

   SetColWidth(probeStyleColumnDrillMin,        (int)(width * 0.075));
   SetColWidth(probeStyleColumnDrillMax,        (int)(width * 0.075));

   SetColWidth(probeStyleColumnExposedMetalMin, (int)(width * 0.075));
   SetColWidth(probeStyleColumnExposedMetalMax, (int)(width * 0.075));

   SetColWidth(probeStyleColumnTargetType,      (int)(width * 0.15));

   RedrawAll();
}

//----------------------------------------------------------------------------------

void CProbeStylesGrid::OnSetup()
{

   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;


   SetNumberCols(11);
   SetNumberRows(0);

   int singleRowThHeight = GetTH_Height();
   int topRowHeight = singleRowThHeight;
   int secondRowHeight = singleRowThHeight;
   int overallHeadingHeight = topRowHeight + secondRowHeight;
   SetTH_NumberRows(2);
   SetTH_RowHeight(-1, secondRowHeight);
   SetTH_RowHeight(-2, topRowHeight);
   SetTH_Height(overallHeadingHeight);

   EnableJoins(TRUE);
   SetCurrentCellMode(UG_CELLMODE_FOCUS | UG_CELLMODE_HIGHLIGHT);
   SetHighlightRow(TRUE);

   SetMultiColHeading(0, -2, 0, -1, "Use");
   SetMultiColHeading(1, -2, 1, -1, "Probe Style Name");
   SetMultiColHeading(2, -2, 2, -1, "Probe Name");
   SetMultiColHeading(3, -2, 3, -1, "Probe Style");
   SetMultiColHeading(4, -2, 4, -1, "Technology");
   SetMultiColHeading(5, -2, 5, -1, "Side");
   SetMultiColHeading(6, -2, 7, -2, "Drill Diameter");
   SetMultiColHeading(8, -2, 9, -2, "Exposed Metal Diameter");
   SetMultiColHeading(10, -2, 10, -1, "Target Type");

   QuickSetText(6, -1, "Min");
   QuickSetText(7, -1, "Max");
   QuickSetText(8, -1, "Min");
   QuickSetText(9, -1, "Max");


   CUGCell cell;

   // USE checkbox
   GetColDefault(probeStyleColumnUse, &cell); 
   cell.SetCellType(UGCT_CHECKBOX);  // To make it a checkbox cell
   cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
   SetColDefault(probeStyleColumnUse, &cell);

   // PROBE STYLE aka the Shape and Color column
   int probeStyleShapeCellTypeID = AddCellType(&m_probeStyleShapeCellType);
   GetColDefault(probeStyleColumnShapeColor, &cell); 
   int res1 = cell.SetCellType(probeStyleShapeCellTypeID);  // To make it a checkbox cell
   int res2 = SetColDefault(probeStyleColumnShapeColor, &cell);



   // TARGET TYPE ellipsis
#ifdef DOESNT_REALLY_WORK_NICELY
   GetColDefault(probeStyleColumnTargetType, &cell); 
   cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);  
   //cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
   SetColDefault(probeStyleColumnTargetType, &cell);
#endif

   // TECHNOLOGY option menu
   GetColDefault(probeStyleColumnTechnology, &cell); 
   cell.SetCellType(UGCT_DROPLIST); 
   CString optionlist("SMD\nTHRU\nSMD&THRU\n");
   cell.SetLabelText( optionlist ); 
   SetColDefault(probeStyleColumnTechnology, &cell);

   // SIDE option menu
   GetColDefault(probeStyleColumnSide, &cell); 
   cell.SetCellType(UGCT_DROPLIST); 
   optionlist = "TOP\nBOTTOM\nBOTH\n";
   cell.SetLabelText( optionlist ); 
   SetColDefault(probeStyleColumnSide, &cell);


   // Center all cells
   for (int colIndx = 0; colIndx < GetNumberCols(); colIndx++)
   {
      GetColDefault(colIndx, &cell); 
      cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
      SetColDefault(colIndx, &cell);
   }

   // No rows now so nothing to fit:   BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   SetColWidth(-1, 0); // get rid of "row heading"
}

void CProbeStylesGrid::OnDClicked(int col, long row, RECT *rect,POINT *point,BOOL processed)
{
   if (col == probeStyleColumnTargetType)
   {
      CString targetTypes( QuickGetText(col, row) );  // Get current target types setting

      // Set up the popup dialog
      CMultiSelectDlg dlg("Probe Style Target Types", this);
      CSelItem *item = new CSelItem("Component", targetTypes.Find("Component")>=0);
      dlg.AddItem(item);
      item = new CSelItem("Via", targetTypes.Find("Via")>=0);
      dlg.AddItem(item);
      item = new CSelItem("TestPoint", targetTypes.Find("TestPoint")>=0);
      dlg.AddItem(item);
      item = new CSelItem("Connector", targetTypes.Find("Connector")>=0);
      dlg.AddItem(item);

      if (dlg.DoModal() == IDOK)
      {
         targetTypes.Empty();
         for (POSITION pos=dlg.GetItemHeadPosition(); pos!=NULL;dlg.GetItemNext(pos))
         {
            item = dlg.GetItemAt(pos);
            if (!item->GetSelect())
               continue;

            if (!targetTypes.IsEmpty())
               targetTypes += ",";

            targetTypes += item->GetName();
         }

         QuickSetText(col, row, targetTypes);
         RedrawRow(row);
      }

   }
   else if (col == probeStyleColumnShapeColor)
   {
      EProbeShape shape;
      COLORREF color;
      CString cellText( QuickGetText(col, row) );  // Get current setting

      CProbeStyleShapeCellType::DecodeVal(cellText, shape, color);

      CProbeShapeColorDialog shapeDlg;
      shapeDlg.SetProbeShape(shape);
      shapeDlg.SetProbeColor(color);

      if (shapeDlg.DoModal() == IDOK)
      {
         shape = shapeDlg.GetProbeShape();
         color = shapeDlg.GetProbeColor();
         CString encodedStr;
         CProbeStyleShapeCellType::EncodeVal(shape, color, encodedStr);
         QuickSetText(col, row, encodedStr);
         RedrawRow(row);
      }
   }
   else
   {
      StartEdit(col, row, 0);
   }
}

int CProbeStylesGrid::GetSelectedRow()
{
   int nCol;
   long nRow;
   int nRet = this->EnumFirstSelected( &nCol, &nRow );
   if (nRet == UG_SUCCESS)
      return nRow;

   return -1; // No selected cell found, so no selected row
}

int CProbeStylesGrid::GetSelectedCol()
{
   int nCol;
   long nRow;
   int nRet = this->EnumFirstSelected( &nCol, &nRow );
   if (nRet == UG_SUCCESS)
      return nCol;

   return -1; // No selected cell found, so no selected row
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

int CProbeStylesGrid::OnEditStart(int col, long row, CWnd **edit)
{
   *edit = &m_myCUGEdit;

   int allowEdit = false;

   switch (col)
   {
      // Allow in-cell text edit for these
   case probeStyleColumnName:
   case probeStyleColumnProbeName:
   case probeStyleColumnDrillMin:
   case probeStyleColumnDrillMax:
   case probeStyleColumnExposedMetalMin:
   case probeStyleColumnExposedMetalMax:
      allowEdit = true;
      break;

      // Disallow in-cell text edit for these
   case probeStyleColumnUse:
   case probeStyleColumnTechnology:
   case probeStyleColumnShapeColor:
   case probeStyleColumnSide:
   case probeStyleColumnTargetType:
      allowEdit = false;
      break;
   }

   return allowEdit;
}

void CProbeStylesGrid::GetRowValues(int rowIndx, bool &use, CString &styleName, CString &probeName, CString &technology, CString &surface, 
                                    EProbeShape &shape, COLORREF &color, CString &drillMin, CString &drillMax, CString &exposedMin, 
                                    CString &exposedMax, CString &targetType)
{
   use = false;
   styleName.Empty();
   probeName.Empty();
   technology.Empty();
   surface.Empty();
   shape = probeShapeDefaultTarget;
   color = colorBlack;
   drillMin.Empty();
   drillMax.Empty();
   exposedMin.Empty();
   exposedMax.Empty();
   targetType.Empty();

   if (rowIndx >= 0 && rowIndx < GetNumberRows())
   {
      CUGCell cell;
      GetCell(probeStyleColumnUse, rowIndx, &cell);
      use = cell.GetBool()?true:false;

      CString cellText( QuickGetText(probeStyleColumnShapeColor, rowIndx) );
      COLORREF foreColor = colorBlack;
      EProbeShape shapeTag = probeShapeDefaultTarget;
      CProbeStyleShapeCellType::DecodeVal(cellText, shapeTag, foreColor);
      shape = shapeTag;
      color = foreColor;

      styleName = QuickGetText(probeStyleColumnName, rowIndx);
      probeName = QuickGetText(probeStyleColumnProbeName, rowIndx);

      technology = QuickGetText(probeStyleColumnTechnology, rowIndx);
      surface = QuickGetText(probeStyleColumnSide, rowIndx);

      drillMin = QuickGetText(probeStyleColumnDrillMin, rowIndx);
      drillMax = QuickGetText(probeStyleColumnDrillMax, rowIndx);

      exposedMin = QuickGetText(probeStyleColumnExposedMetalMin, rowIndx);
      exposedMax = QuickGetText(probeStyleColumnExposedMetalMax, rowIndx);

      targetType = QuickGetText(probeStyleColumnTargetType, rowIndx);
   }

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
int CProbeStylesGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	//UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	//UNREFERENCED_PARAMETER(*vcKey);

   // Only float numbers allowed in the min and max cells.
   // Anything goes for the rest.

   switch (col)
   {
   case probeStyleColumnDrillMin:
   case probeStyleColumnDrillMax:
   case probeStyleColumnExposedMetalMin:
   case probeStyleColumnExposedMetalMax:
      return OnEditVerifyFloatCell(col, row, edit, vcKey);
   }

   return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////

CProbeStyleShapeCellType::CProbeStyleShapeCellType()
{
}

CProbeStyleShapeCellType::~CProbeStyleShapeCellType()
{
}

/***************************************************
GetName  - overloaded CUGCellType::GetName
		Returns a readable name for the cell type.

    **See CUGCellType::GetName for more details
	about this function

Params:
	<none>
Return:
	LPCTSTR representation of the user friendly
			cell type name
***************************************************/
LPCTSTR CProbeStyleShapeCellType::GetName()
{
	return _T("ProbeStyleShape");
}

/***************************************************
GetUGID  - overloaded CUGCellType::GetUGID
	Returns a GUID for the cell type, this number
	is unique for each cell type and never changes.
	This number can be used to find the cell types
	added to this instance of the Ultimate Grid.

    **See CUGCellType::GetUGID for more details
	about this function

Params:
	<none>
Returns:
	UGID (which is actually a GUID)
***************************************************/
LPCUGID CProbeStyleShapeCellType::GetUGID()
{
   // from UG button
	//static const UGID ugid = { 0x93aab8d0, 0xf749, 0x11d0, 
	//						{ 0x9c, 0x7f, 0x0, 0x80, 0xc8, 
	//						0x3f, 0x71, 0x2f } };

   // Generated from dev studio
   // {8C4F8031-7C37-4c8a-9A34-997BEA17FA0A}
   static const UGID ugid = { 0x8c4f8031, 0x7c37, 0x4c8a, 
                   { 0x9a, 0x34, 0x99, 0x7b, 0xea, 
                   0x17, 0xfa, 0xa } };


	return &ugid;
}


/***************************************************
OnLClicked  - overloaded CUGCellType::OnLClicked
	The handling of the OnLClicked event in the check
	box cell type is the most important for this cell 
	type.  Here is where the desired user actions are
	evaluated and if necessary the check state is
	updated.
	This function determines if the user clicked the
	left mouse button over the check area on the cell
	if he did, than the state of the check is changed.

    **See CUGCellType::OnLClicked for more details
	about this function

Params:
	col - column that was clicked in
	row - row that was clicked in
	rect - rectangle of the cell that was clicked in
	point - point where the mouse was clicked
Return:
	TRUE - if the event was processed
	FALSE - if the event was not
***************************************************/
BOOL CProbeStyleShapeCellType::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point)
{
   return TRUE;
}

/***************************************************
OnDClicked  - overloaded CUGCellType::OnDClicked
	The check box does not have a special behaviour
	when it comes to the double click event.  It does
	however pass handling of this event to the
	OnLClicked virtual function.

    **See CUGCellType::OnDClicked for more details
	about this function

Params:
	col - column that was clicked in
	row - row that was clicked in
	rect - rectangle of the cell that was clicked in
	point - point where the mouse was clicked
Return:
	TRUE - if the event was processed
	FALSE - if the event was not
***************************************************/
BOOL CProbeStyleShapeCellType::OnDClicked(int col,long row,RECT *rect,POINT *point)
{
	return TRUE;
}


/***************************************************
OnDraw  - overloaded CUGCellType::OnDraw
	This function will draw the checkbox according to
	the extended style for the checkbox.

    **See CUGCellType::OnDraw for more details
	about this function

Params
	dc		- device context to draw the cell with
	rect	- rectangle to draw the cell in
	col		- column that is being drawn
	row		- row that is being drawn
	cell	- cell that is being drawn
	selected- TRUE if the cell is selected, otherwise FALSE
	current - TRUE if the cell is the current cell, otherwise FALSE
Return
	<none>
****************************************************/
void CProbeStyleShapeCellType::OnDraw(CDC *dc,RECT *rect,int col,long row,CUGCell *cell,int selected,int current)
{
   if (dc == NULL || rect == NULL || cell == NULL)
      return;

   // draw border of the cell using build-in routine
	DrawBorder( dc, rect, rect, cell ); //*rcf Bug Still doesn't draw border to match the rest of the grid

   COLORREF foreColor = colorBlack;
   //COLORREF backColor = (selected || current)?colorHtmlLightCyan:colorWhite;  // This one looked pretty good.
   COLORREF aPaleBlue = RGB(196, 236, 255);  // This one was even better.
   COLORREF backColor = (selected || current)?aPaleBlue:colorWhite;
   EProbeShape shapeTag = probeShapeDefaultTarget;

   CString cellText(cell->GetText());
   DecodeVal(cellText, shapeTag, foreColor);

   DrawProbeStyleShape(*dc, shapeTag, foreColor, backColor, rect);
}

void CProbeStyleShapeCellType::EncodeVal(EProbeShape shapeTag, COLORREF foreColor, CString &encodedStr)
{
   encodedStr.Format("%d|%d", (int)shapeTag, (int)foreColor);
}

void CProbeStyleShapeCellType::DecodeVal(CString &encodedStr, EProbeShape &shapeTag, COLORREF &foreColor)
{
   shapeTag = probeShapeDefaultTarget;
   foreColor = colorBlack;

   int delimPos = encodedStr.Find("|");
   if (delimPos > -1)
   {
      CString shapeTagStr( encodedStr.Left(delimPos) );
      CString colorStr( encodedStr.Right( encodedStr.GetLength() - delimPos -1 ) );
      shapeTag = (EProbeShape)(atoi(shapeTagStr));
      foreColor = (COLORREF)(atoi(colorStr));
   }
}



void CProbeStyleShapeCellType::DrawProbeStyleShape(CDC &dc, EProbeShape shape, COLORREF color, COLORREF bkColor, CRect rect)
{
	int shortestSize = min(rect.Width(), rect.Height());
	int celPadding = 1;
	CPoint center = rect.CenterPoint();
	CRect centerSquare(center.x - (shortestSize/2 - celPadding), center.y - (shortestSize/2 - celPadding), center.x + (shortestSize/2 - celPadding), center.y + (shortestSize/2 - celPadding));

	CBrush bkBrush(bkColor);
	HGDIOBJ oldBrush = dc.SelectObject(bkBrush);
	dc.Rectangle(rect);
	DeleteObject(dc.SelectObject(oldBrush));

	CPen colorPen;
	if (shape != probeShapeDefaultTarget)
		colorPen.CreatePen(PS_SOLID, 1, color);
	else
		colorPen.CreatePen(PS_SOLID, 1, RGB(100, 0, 0));
	HGDIOBJ oldPen = dc.SelectObject(colorPen);

	switch (shape)
	{
	case probeShapeDefaultTarget:
		{
			CRect innerSquare(centerSquare.left + (int)((double)centerSquare.Width() / 4.0), centerSquare.top + (int)((double)centerSquare.Width() / 4.0),
				centerSquare.right - (int)((double)centerSquare.Width() / 4.0), centerSquare.bottom - (int)((double)centerSquare.Width() / 4.0));
			dc.Ellipse(innerSquare);

			// +
			dc.MoveTo(centerSquare.CenterPoint().x, centerSquare.top);
			dc.LineTo(centerSquare.CenterPoint().x, centerSquare.bottom);
			dc.MoveTo(centerSquare.left, centerSquare.CenterPoint().y);
			dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
		}
		break;
	case probeShapeRoundCross:
		dc.Ellipse(centerSquare);

		// +
		dc.MoveTo(centerSquare.CenterPoint().x, centerSquare.top);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.bottom);
		dc.MoveTo(centerSquare.left, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
		break;
	case probeShapeRoundX:
		dc.Ellipse(centerSquare);

		// X
		{
			int halfSide = (int)(((double)centerSquare.Width()/2.0) * sin(DegToRad(45.0)));
			CRect squareInCircle(center.x - halfSide, center.y - halfSide, center.x + halfSide, center.y + halfSide);

			dc.MoveTo(squareInCircle.right, squareInCircle.top);
			dc.LineTo(squareInCircle.left, squareInCircle.bottom);
			dc.MoveTo(squareInCircle.left, squareInCircle.top);
			dc.LineTo(squareInCircle.right, squareInCircle.bottom);
		}
		break;
	case probeShapeSquareCross:
		dc.Rectangle(centerSquare);

		// +
		dc.MoveTo(centerSquare.CenterPoint().x, centerSquare.top);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.bottom);
		dc.MoveTo(centerSquare.left, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
		break;
	case probeShapeSquareX:
		dc.Rectangle(centerSquare);

		// X
		dc.MoveTo(centerSquare.right, centerSquare.top);
		dc.LineTo(centerSquare.left, centerSquare.bottom);
		dc.MoveTo(centerSquare.left, centerSquare.top);
		dc.LineTo(centerSquare.right, centerSquare.bottom);
		break;
	case probeShapeDiamondCross:
		// diamond
		dc.MoveTo(centerSquare.CenterPoint().x, centerSquare.top);
		dc.LineTo(centerSquare.left, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.bottom);
		dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.top);

		// +
		dc.MoveTo(centerSquare.CenterPoint().x, centerSquare.top);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.bottom);
		dc.MoveTo(centerSquare.left, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
		break;
	case probeShapeDiamondX:
		// diamond
		dc.MoveTo(centerSquare.CenterPoint().x, centerSquare.top);
		dc.LineTo(centerSquare.left, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.bottom);
		dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.top);

		// X
		{
			CPoint2d topLeft(centerSquare.TopLeft());
			int distance = (int)(topLeft.distance(centerSquare.CenterPoint()) / 2.0);
			CRect diamondInSquare(center.x - distance, center.y - distance, center.x + distance, center.y + distance);

			dc.MoveTo(diamondInSquare.right, diamondInSquare.top);
			dc.LineTo(diamondInSquare.left, diamondInSquare.bottom);
			dc.MoveTo(diamondInSquare.left, diamondInSquare.top);
			dc.LineTo(diamondInSquare.right, diamondInSquare.bottom);
		}
		break;
	case probeShapeHexagonCross:
		{
			//         hexWidth
			//				|   |
			//				----|-----
			//         /          \
			//        /            \
			//       /              \
			//       \              /
			//        \            /
			//         \          /
			//          ----------
			//
			int hexWidth = (int)(((double)centerSquare.Width() / 2.0) * (3.0 / 5.0));
			dc.MoveTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.top);
			dc.LineTo(centerSquare.CenterPoint().x - hexWidth, centerSquare.top);
			dc.LineTo(centerSquare.left, centerSquare.CenterPoint().y);
			dc.LineTo(centerSquare.CenterPoint().x - hexWidth, centerSquare.bottom);
			dc.LineTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.bottom);
			dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
			dc.LineTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.top);
		}

		// +
		dc.MoveTo(centerSquare.CenterPoint().x, centerSquare.top);
		dc.LineTo(centerSquare.CenterPoint().x, centerSquare.bottom);
		dc.MoveTo(centerSquare.left, centerSquare.CenterPoint().y);
		dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
		break;
	case probeShapeHexagonX:
		{
			//         hexWidth
			//				|   |
			//				----|-----
			//         /          \
			//        /            \
			//       /              \
			//       \              /
			//        \            /
			//         \          /
			//          ----------
			//
			int hexWidth = (int)(((double)centerSquare.Width() / 2.0) * (3.0 / 5.0));
			dc.MoveTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.top);
			dc.LineTo(centerSquare.CenterPoint().x - hexWidth, centerSquare.top);
			dc.LineTo(centerSquare.left, centerSquare.CenterPoint().y);
			dc.LineTo(centerSquare.CenterPoint().x - hexWidth, centerSquare.bottom);
			dc.LineTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.bottom);
			dc.LineTo(centerSquare.right, centerSquare.CenterPoint().y);
			dc.LineTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.top);

			// X
			dc.MoveTo(centerSquare.CenterPoint().x - hexWidth, centerSquare.top);
			dc.LineTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.bottom);
			dc.MoveTo(centerSquare.CenterPoint().x + hexWidth, centerSquare.top);
			dc.LineTo(centerSquare.CenterPoint().x - hexWidth, centerSquare.bottom);
		}

		break;

	case probeShapeHourglass:
		{
			dc.MoveTo(centerSquare.right, centerSquare.top);
			dc.LineTo(centerSquare.left, centerSquare.top);
			dc.LineTo(centerSquare.right, centerSquare.bottom);
			dc.LineTo(centerSquare.left, centerSquare.bottom);
			dc.LineTo(centerSquare.right, centerSquare.top);
		}

		break;
	}

	DeleteObject(dc.SelectObject(oldPen));
}
