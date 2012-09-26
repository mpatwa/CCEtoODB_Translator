// $Header: /CAMCAD/5.0/read_wrt/Sony_AOI_o_GUI.cpp 17    6/17/07 9:00p Kurt Van Ness $


#include "StdAfx.h"
#include "afxdisp.h"
#include "Response.h"
#include "ccdoc.h" 
#include "CCEtoODB.h"
#include "Sony_AOI_o_GUI.h"

// The following is from PanRef.cpp, apparantly not in any .h file.
// References like the following are sprinked about in camcad source.
extern int  PanReference(CCEtoODBView *view, const char *ref) ;


#define ID_GRIDROWEDITFINISHED         (WM_APP + 100)

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CSonyAoiXrefPage, CResizingPropertyPage)


CSonyAoiXrefPage::CSonyAoiXrefPage(CSonyAOIPropertySheet& parent)
	: CResizingPropertyPage(CSonyAoiXrefPage::IDD)
     , m_parent(parent)
     , m_overwriteValues(FALSE)
	  , m_storeOption(FALSE)
{
   m_redBackgroundColor       = RGB(0xff,0xf0,0xf5);
   m_greenBackgroundColor     = RGB(0xf0,0xff,0xf0);
	m_yellowBackgroundColor		= RGB(0xf0,0xf0,0x9f);
   m_lightGrayBackgroundColor = RGB(226,226,226);

   addFieldControl(IDC_SonyAoiPnXrefGridStatic        ,anchorLeft,growBoth);

}

CSonyAoiXrefPage::~CSonyAoiXrefPage()
{
}

BEGIN_MESSAGE_MAP(CSonyAoiXrefPage, CResizingPropertyPage)
   ON_MESSAGE(ID_GRIDROWEDITFINISHED, OnRowEditFinished)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CSonyAoiXrefPage, CResizingPropertyPage)
END_EVENTSINK_MAP()



void CSonyAoiXrefPage::DoDataExchange(CDataExchange* pDX)
{
	CResizingPropertyPage::DoDataExchange(pDX);
}

afx_msg void CSonyAoiXrefPage::OnSize(UINT nType, int cx, int cy)
{
   CResizingPropertyPage::OnSize(nType,cx,cy);
   UpdateGridColumnSizes(false);
}

void CSonyAoiXrefPage::setRowStatus(int rowIndex,CSonyPart& part)
{
   switch (part.getStatus())
   {
   case sonypartAssigned:
      m_pnXrefGrid.SetRowBackColor(rowIndex, m_greenBackgroundColor);
      m_pnXrefGrid.QuickSetTextColor(m_pnXrefGrid.m_colStatus, rowIndex, colorDkGreen);
      break;

   case sonypartError:    
      m_pnXrefGrid.SetRowBackColor(rowIndex, m_redBackgroundColor);
      m_pnXrefGrid.QuickSetTextColor(m_pnXrefGrid.m_colStatus, rowIndex, colorRed);
      break;

   case sonypartUnassigned:   
      m_pnXrefGrid.SetRowBackColor(rowIndex, m_yellowBackgroundColor);
      m_pnXrefGrid.QuickSetTextColor(m_pnXrefGrid.m_colStatus, rowIndex, colorDkYellow);
      break;
   }
}


void CSonyAoiXrefPage::fillGridRow(int rowIndex, CSonyPart *part)
{
   int rowCnt = m_pnXrefGrid.GetNumberRows();
   if (rowIndex < 0 || rowIndex > rowCnt)
      return; // error in call params

	if (part != NULL)
	{
      m_pnXrefGrid.SetRowData(rowIndex, (void*)part);

		m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colStatus, rowIndex          ,part->getStatusText());
      m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colPartNumber, rowIndex      ,part->getPartNumber());
		m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colGeom, rowIndex            ,part->getGeomName());
		m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colBitmap, rowIndex          ,part->getBitmap());

		m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colCategory, rowIndex        ,part->getMasterCategoryName());
		m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colModel, rowIndex           ,part->getMasterModelName());

		if (getParent().getSonyBoard().getMasterXref()->isAvailable())
			m_pnXrefGrid.QuickSetBool(m_pnXrefGrid.m_colSave, rowIndex			,part->getSave());
		else
			m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colSave, rowIndex, QNotAvailable);

      m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colExplode, rowIndex         ,part->getExplode() ? "Yes" : "No");
      m_pnXrefGrid.QuickSetText(m_pnXrefGrid.m_colRotAdj, rowIndex          ,part->getAngleOffsetString());

      CSonyDatabase *masterDB = getParent().getSonyBoard().getMasterSonyDB();
      m_pnXrefGrid.FillModelDroplistOptions(masterDB, rowIndex);


		setRowStatus(rowIndex,*part);
	}
}

void CSonyAoiXrefPage::fillGrid()
{
   m_pnXrefGrid.RemoveAllRows();

	CSonyPartArray& partlist = getParent().getSonyBoard().getUniqueParts();

	CSonyDatabase *masterDB = getParent().getSonyBoard().getMasterSonyDB();

   m_pnXrefGrid.SetNumberRows(partlist.getSize());

	for (int rowIndx = 0; rowIndx < partlist.getSize(); rowIndx++)
   {
		CSonyPart *part = partlist.getAt(rowIndx);
		fillGridRow(rowIndx, part);
   }

	m_pnXrefGrid.FillCategoryDroplistOptions(masterDB);

   UpdateGridColumnSizes();  // This will  do the RedrawAll() too.
}

void CSonyAoiXrefPage::update()
{
   fillGrid();
}

void CSonyAoiXrefPage::updateRow(int rowIndex)
{
	CSonyPart *part = (CSonyPart*)m_pnXrefGrid.GetRowData(rowIndex);
   fillGridRow(rowIndex, part);

   m_pnXrefGrid.RedrawRow(rowIndex);
}


void CSonyAoiXrefPage::updatePartInGrid(long row,long col)
{
   if (row >= 0 && row < m_pnXrefGrid.GetNumberRows())
   {
      CUGCell cell;
      CSonyPart* part = (CSonyPart*)m_pnXrefGrid.GetRowData(row);

      if (col == m_pnXrefGrid.m_colSave)
      {
         m_pnXrefGrid.GetCell(m_pnXrefGrid.m_colSave, row, &cell);
         bool val = cell.GetBool()?true:false;
         part->setSave(val);
      }
      else if (col == m_pnXrefGrid.m_colExplode)
      {
         CString val(m_pnXrefGrid.QuickGetText(m_pnXrefGrid.m_colExplode, row));
         part->setExplode(val.CompareNoCase("yes") == 0);
      }
      else if (col == m_pnXrefGrid.m_colRotAdj)
      {
         CString val(m_pnXrefGrid.QuickGetText(m_pnXrefGrid.m_colRotAdj, row));
         part->setAngleOffset(atoi(val));
      }
      else if (col == m_pnXrefGrid.m_colCategory)
      {
         CString val(m_pnXrefGrid.QuickGetText(m_pnXrefGrid.m_colCategory, row));
         CSonyDatabase *masterDB = getParent().getSonyBoard().getMasterSonyDB();
         if (masterDB != NULL)
            part->setMasterCategory(masterDB->getCategoryNumber(val));
         else
            part->setMasterCategory(0);
      }
      else if (col == m_pnXrefGrid.m_colModel)
      {
         CString val(m_pnXrefGrid.QuickGetText(m_pnXrefGrid.m_colModel, row));
         CSonyDatabase *masterDB = getParent().getSonyBoard().getMasterSonyDB();
         if (masterDB != NULL)
            part->setMasterModel(masterDB->getModelNumber(part->getMasterCategory(), val));
         else
            part->setMasterModel(0);
      }



		// Changes may have made the cat/mod pair invalid, or maybe it was invalid
		// to start with. "Save" is not allowed unless cat/mod is valid.
		CSonyDatabase *masterDB = getParent().getSonyBoard().getMasterSonyDB();
		if (masterDB == NULL || 
			!masterDB->isValidCatModCombo(part->getMasterCategory(), part->getMasterModel()))
		{
			part->setSave(false);
		}


		// If settings have changed then auto-check the Save option.
		// Category and model must already be valid (may have just been made valid by user's
		// last action, that's okay). 
		//if (col == m_colCategory || col == m_colModel || col == m_colExplode || col == m_colRotAdj)
		if (col != m_pnXrefGrid.m_colSave) // don't change Save if that is what user just changed
		{
			if (masterDB != NULL &&
				masterDB->isValidCatModCombo(part->getMasterCategory(), part->getMasterModel()))
			{
				CSonyRsiXref*  xrefDB = getParent().getSonyBoard().getMasterXref();
				if (xrefDB != NULL)
				{
					int category = 0;
					int model = 0;
					bool explode = false;
					int angle = 0;

					if (xrefDB->get(part->getPartNumber(), &category, &model, &explode, &angle))
					{
						// if any setting is different than xref entry, then turn on Save
						if (part->getMasterCategory() != category ||
							part->getMasterModel() != model ||
							part->getExplode() != explode ||
							part->getAngleOffset() != angle)
						{
							part->setSave(true);
						}
					}
					else
					{
						// Not in Xref, turn on Save
						part->setSave(true);
					}
				}

			}
		}

	   updateRow(row);
		//setRowStatus(row, *part);
   }

}

// CSonyAoiXrefPage message handlers
BOOL CSonyAoiXrefPage::OnInitDialog()
{
   CResizingPropertyPage::OnInitDialog();

   m_pnXrefGrid.AttachGrid(this, IDC_SonyAoiPnXrefGridStatic);
   UpdateGridColumnSizes();

   CSonyDatabase *masterDB = getParent().getSonyBoard().getMasterSonyDB();
   m_pnXrefGrid.FillCategoryDroplistOptions(masterDB);

   fillGrid();
	
   return TRUE;  // return TRUE unless you set the focus to a control
}

void CSonyAoiXrefPage::UpdateGridColumnSizes(bool redraw)
{
   if (!m_pnXrefGrid.IsSetUp())
      return;

   CRect rcGridWindow;
   GetDlgItem( IDC_SonyAoiPnXrefGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_pnXrefGrid.SizeToFit(rcGridWindow, redraw);
}


BOOL CSonyAoiXrefPage::OnSetActive()
{
   CResizingPropertyPage::OnSetActive();

	// Update for possible fid selection changes in GUI
	getParent().getSonyBoard().gatherFiducials();

	int nrows = m_pnXrefGrid.GetNumberRows();
	for (int i = nrows - 1; i >= 0; i--)
	{
      CString partnumber( m_pnXrefGrid.QuickGetText(m_pnXrefGrid.m_colPartNumber, i) );

      CSonyPart *p = (CSonyPart*)m_pnXrefGrid.GetRowData(i);
		if (p != NULL && p->getIsFid())
		{
			CString partnumber = sonyAoiGetDatastructPartnumber(getParent().getSonyBoard().getDoc(), p->getData());

			CString tfid1pn = sonyAoiGetDatastructPartnumber(getParent().getSonyBoard().getDoc(), getParent().getSonyBoard().getTopFid1());
			CString tfid2pn = sonyAoiGetDatastructPartnumber(getParent().getSonyBoard().getDoc(), getParent().getSonyBoard().getTopFid2());
			CString bfid1pn = sonyAoiGetDatastructPartnumber(getParent().getSonyBoard().getDoc(), getParent().getSonyBoard().getBotFid1());
			CString bfid2pn = sonyAoiGetDatastructPartnumber(getParent().getSonyBoard().getDoc(), getParent().getSonyBoard().getBotFid2());
			

			// If it is one of the fids in use, it is okay, leave it
			if (partnumber.Compare(tfid1pn) == 0 ||
				partnumber.Compare(tfid2pn) == 0 ||
				partnumber.Compare(bfid1pn) == 0 ||
				partnumber.Compare(bfid2pn) == 0)
				
			{
				// ok
			}
			else
			{
				// No longer used, update item in unique parts list
				p->setIsUsed(false);

				// Remove from grid
            m_pnXrefGrid.DeleteRow(i);
			}

		}
	}


	// Add current fids

	CSonyPartArray& partlist = getParent().getSonyBoard().getUniqueParts();
	
	for (int i = 0; i < partlist.getSize(); i++)
   {
		CSonyPart *part = partlist.getAt(i);
		
		if (part->getIsFid() && part->getIsUsed())
		{
			// Add if not already in the grid
			CString partnumber = sonyAoiGetDatastructPartnumber(getParent().getSonyBoard().getDoc(), part->getData());
			bool alreadyPresent = false;

         nrows = m_pnXrefGrid.GetNumberRows();
			for (int j = 0; j < nrows && !alreadyPresent; j++)
			{
            CString gridpartnumber( m_pnXrefGrid.QuickGetText(m_pnXrefGrid.m_colPartNumber, j) );
				if (gridpartnumber.Compare(partnumber) == 0)
				{
					alreadyPresent = true;
				}
			}

			if (!alreadyPresent)
			{
				getParent().getSonyBoard().applyPartNumberCrossReference(part);

            int rowIndx = m_pnXrefGrid.GetNumberRows();
            m_pnXrefGrid.SetNumberRows(rowIndx+1);

				fillGridRow(rowIndx, part);
			}

		}
   }


	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////

LRESULT CSonyAoiXrefPage::OnRowEditFinished(WPARAM wParamROW, LPARAM lParamCOL)
{
   int row = wParamROW;
   int col = lParamCOL;
   updatePartInGrid(row, col);

   return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSonyAoiBoardFidPage, CResizingPropertyPage)

CSonyAoiBoardFidPage::CSonyAoiBoardFidPage(CSonyAOIPropertySheet& parent)
	: CResizingPropertyPage(CSonyAoiBoardFidPage::IDD)
     , m_parent(parent)
{
}

CSonyAoiBoardFidPage::~CSonyAoiBoardFidPage()
{
}

void CSonyAoiBoardFidPage::DoDataExchange(CDataExchange* pDX)
{
	CResizingPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BOARD_NAME, m_txtBoardName);
	DDX_Control(pDX, IDC_TOP_FID_1, m_cboTopFid1);
	DDX_Control(pDX, IDC_TOP_FID_2, m_cboTopFid2);
	DDX_Control(pDX, IDC_BOT_FID_1, m_cboBotFid1);
	DDX_Control(pDX, IDC_BOT_FID_2, m_cboBotFid2);
	DDX_Control(pDX, IDC_TOP_FID_FRAME, m_topFidFrame);
	DDX_Control(pDX, IDC_BOT_FID_FRAME, m_botFidFrame);
	DDX_Control(pDX, IDC_FIDUCIAL_MSG, m_fidEnableExportMsg);
}


BEGIN_MESSAGE_MAP(CSonyAoiBoardFidPage, CPropertyPage)
	ON_EN_CHANGE(IDC_BOARD_NAME, OnEnChangeBoardName)
	ON_CBN_SELCHANGE(IDC_TOP_FID_1, OnCbnSelchangeTopFid1)
	ON_CBN_SELCHANGE(IDC_TOP_FID_2, OnCbnSelchangeTopFid2)
	ON_CBN_SELCHANGE(IDC_BOT_FID_1, OnCbnSelchangeBotFid1)
	ON_CBN_SELCHANGE(IDC_BOT_FID_2, OnCbnSelchangeBotFid2)
END_MESSAGE_MAP()


// CSonyAoiBoardFidPage message handlers
BOOL CSonyAoiBoardFidPage::OnInitDialog()
{
   CResizingPropertyPage::OnInitDialog();

	CString defaultname = getParent().getSonyBoard().getDefaultBoardName();
	m_txtBoardName.SetWindowText(defaultname);

   fillFiducialCombos();
	enableFiducialCombos();

	// Do this after all default initialization is done
	loadSettingsFromAttribute();

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CSonyAoiBoardFidPage::enableFiducialCombos()
{
	// If the given side has parts, then it needs the fid chooser.
	// If it does not have parts, it does not need fid chooser.

	int topPartCount = getParent().getSonyBoard().getPartCount(sideTop);
	m_cboTopFid1.EnableWindow(topPartCount > 0);
	m_cboTopFid2.EnableWindow(topPartCount > 0);
	m_topFidFrame.EnableWindow(topPartCount > 0);

	int botPartCount = getParent().getSonyBoard().getPartCount(sideBottom);
	m_cboBotFid1.EnableWindow(botPartCount > 0);
	m_cboBotFid2.EnableWindow(botPartCount > 0);
	m_botFidFrame.EnableWindow(botPartCount > 0);

	if (topPartCount > 0 && botPartCount > 0)
		m_fidEnableExportMsg.SetWindowText("Two unique fiducials must be selected for each of top and bottom sides to enable export.");
	else if (topPartCount > 0)
		m_fidEnableExportMsg.SetWindowText("Two unique fiducials must be selected for top side to enable export.");
	else if (botPartCount > 0)
		m_fidEnableExportMsg.SetWindowText("Two unique fiducials must be selected for bottom side to enable export.");
	else
		m_fidEnableExportMsg.SetWindowText("No parts! Nothing to export.");
}

void CSonyAoiBoardFidPage::fillFiducialCombos()
{
	m_cboTopFid1.ResetContent();
	m_cboTopFid2.ResetContent();
	m_cboBotFid1.ResetContent();
	m_cboBotFid2.ResetContent();

	//CSonyAOIPropertySheet parent = getParent();

	CCEtoODBDoc *doc = this->getParent().getSonyBoard().getDoc();

	for (int i=0; i< doc->getMaxBlockIndex(); i++)
	{
		BlockStruct* block = doc->getBlockAt(i);

		if (block != NULL)
		{
			POSITION insertPos = block->getHeadDataInsertPosition();
			while (insertPos)
			{
				DataStruct* fiducial = block->getNextDataInsert(insertPos);

				if (fiducial != NULL && fiducial->isInsertType(insertTypeFiducial))
				{
					if (!fiducial->getInsert()->getRefname().IsEmpty())
					{
						if (fiducial->getInsert()->getPlacedTop())
						{
							m_cboTopFid1.SetItemData(m_cboTopFid1.AddString(fiducial->getInsert()->getRefname()), (DWORD)fiducial);
							m_cboTopFid2.SetItemData(m_cboTopFid2.AddString(fiducial->getInsert()->getRefname()), (DWORD)fiducial);
						}
						else
						{
							m_cboBotFid1.SetItemData(m_cboBotFid1.AddString(fiducial->getInsert()->getRefname()), (DWORD)fiducial);
							m_cboBotFid2.SetItemData(m_cboBotFid2.AddString(fiducial->getInsert()->getRefname()), (DWORD)fiducial);
						}
					}
					else
					{
						CPoint2d insertPt = fiducial->getInsert()->getOrigin2d();
						CString errMsg;
						errMsg.Format("Fiducial at %.3f, %.3f on %s side has no Refname set, can not be  included in fiducial selection list.",
							insertPt.x, insertPt.y, fiducial->getInsert()->getPlacedTop() ? "Top" : "Bottom");
						ErrorMessage(errMsg, "Sony AOI Export - Unusable fiducial encountered.");
					}
				}
			}
		}
	}
}

void CSonyAoiBoardFidPage::saveBoardNameSetting()
{
#define SONYAOI_MAX_NAME_SIZE 255
	char name[SONYAOI_MAX_NAME_SIZE + 1];
	m_txtBoardName.GetWindowText(name, SONYAOI_MAX_NAME_SIZE);
	getParent().getSonyBoard().setBoardName(name);
}

void CSonyAoiBoardFidPage::OnEnChangeBoardName()
{
	// Could save on the fly, like fids, but this gets called for
	// every keystroke, so instead we'll just grab this when
	// the export button is clicked.
}

DataStruct *CSonyAoiBoardFidPage::commitTopFid1()
{
	int fidIndex = 0;
	DataStruct *fid = NULL;
	if ((fidIndex = m_cboTopFid1.GetCurSel()) != CB_ERR)
		fid = (DataStruct*)(m_cboTopFid1.GetItemData(fidIndex));
	getParent().getSonyBoard().setTopFid1(fid);
	getParent().UpdateExportButton();

	return fid;
}

DataStruct *CSonyAoiBoardFidPage::commitTopFid2()
{
	int fidIndex = 0;
	DataStruct *fid = NULL;
	if ((fidIndex = m_cboTopFid2.GetCurSel()) != CB_ERR)
		fid = (DataStruct*)(m_cboTopFid2.GetItemData(fidIndex));
	getParent().getSonyBoard().setTopFid2(fid);
	getParent().UpdateExportButton();

	return fid;
}

DataStruct *CSonyAoiBoardFidPage::commitBotFid1()
{
	int fidIndex = 0;
	DataStruct *fid = NULL;
	if ((fidIndex = m_cboBotFid1.GetCurSel()) != CB_ERR)
		fid = (DataStruct*)(m_cboBotFid1.GetItemData(fidIndex));
	getParent().getSonyBoard().setBotFid1(fid);
	getParent().UpdateExportButton();

	return fid;
}

DataStruct *CSonyAoiBoardFidPage::commitBotFid2()
{
	int fidIndex = 0;
	DataStruct *fid = NULL;
	if ((fidIndex = m_cboBotFid2.GetCurSel()) != CB_ERR)
		fid = (DataStruct*)(m_cboBotFid2.GetItemData(fidIndex));
	getParent().getSonyBoard().setBotFid2(fid);
	getParent().UpdateExportButton();

	return fid;
}

void CSonyAoiBoardFidPage::OnCbnSelchangeTopFid1()
{
	DataStruct *fid = commitTopFid1();

	if (fid != NULL)
		PanReference(getActiveView(), fid->getInsert()->getRefname());
}

void CSonyAoiBoardFidPage::OnCbnSelchangeTopFid2()
{
	DataStruct *fid = commitTopFid2();

	if (fid != NULL)
		PanReference(getActiveView(), fid->getInsert()->getRefname());
}

void CSonyAoiBoardFidPage::OnCbnSelchangeBotFid1()
{
	DataStruct *fid = commitBotFid1();

	if (fid != NULL)
		PanReference(getActiveView(), fid->getInsert()->getRefname());
}

void CSonyAoiBoardFidPage::OnCbnSelchangeBotFid2()
{
	DataStruct *fid = commitBotFid2();

	if (fid != NULL)
		PanReference(getActiveView(), fid->getInsert()->getRefname());
}

CString CSonyAoiBoardFidPage::getSettingsString()
{
	CString settings;
	CString delimiter;

	CString boardname;
	m_txtBoardName.GetWindowText(boardname);
	if (!boardname.IsEmpty())
	{
		settings.AppendFormat("BOARDNAME;%s=\"%s\"|", QBoardName, boardname);
	}

	settings.AppendFormat("FIDUCIALS");
	delimiter = ";";

	int fidIndex = 0;
	DataStruct *fid = NULL;

	if ((fidIndex = m_cboTopFid1.GetCurSel()) != CB_ERR)
	{
		CString txt;
		m_cboTopFid1.GetWindowText(txt);
		settings.AppendFormat("%s%s=\"%s\"", delimiter, QTopFid1, txt);
	}

	if ((fidIndex = m_cboTopFid2.GetCurSel()) != CB_ERR)
	{
		CString txt;
		m_cboTopFid2.GetWindowText(txt);
		settings.AppendFormat("%s%s=\"%s\"", delimiter, QTopFid2, txt);
	}

	if ((fidIndex = m_cboBotFid1.GetCurSel()) != CB_ERR)
	{
		CString txt;
		m_cboBotFid1.GetWindowText(txt);
		settings.AppendFormat("%s%s=\"%s\"", delimiter, QBotFid1, txt);
	}

	if ((fidIndex = m_cboBotFid2.GetCurSel()) != CB_ERR)
	{
		CString txt;
		m_cboBotFid2.GetWindowText(txt);
		settings.AppendFormat("%s%s=\"%s\"", delimiter, QBotFid2, txt);
	}

	return settings;
}

void CSonyAoiBoardFidPage::loadSettingsFromAttribute()
{
	// Sections separated with "|" have a section name as first item.
	// This is currently ignored by the parser, but can be taken advantage
	// of if/when more stuff gets added. I.e. sections have identifiers,
	// we just don't really need them at the moment.

	CCEtoODBDoc *doc = getParent().getSonyBoard().getDoc();
	FileStruct *file = getParent().getSonyBoard().getActiveFile();
	if (file == NULL || doc == NULL)
		return;

	WORD keyword = (WORD)doc->getStandardAttributeKeywordIndex(standardAttributeSonyAoiSettings);
	
	Attrib* attrib = NULL;
	if (file->getBlock()->getAttributes() == NULL || !file->getBlock()->getAttributes()->Lookup(keyword, attrib))
		return;
	
	CStringArray atributesStringArray;
	CSupString attributesSupString = attrib->getStringValue();
	attributesSupString.Parse(atributesStringArray, "|");
	
	for (int i=0; i<atributesStringArray.GetCount(); i++)
	{	
		CString attributeString = atributesStringArray.GetAt(i);

		CStringArray atributeStringArray2;
		CSupString attributeSupString2 = attributeString;
		attributeSupString2.Parse(atributeStringArray2, ";");

		for (int index=0; index<atributeStringArray2.GetCount(); index++)
		{		
			CStringArray params;
			CSupString attributeSupString3(atributeStringArray2.GetAt(index));
			attributeSupString3.ParseQuote(params, "=");

			if (params.GetCount() > 1)
			{
				if (params[0].CompareNoCase(QBoardName) == 0)
				{
					m_txtBoardName.SetWindowText(params[1]);
					saveBoardNameSetting();
				}
				else if (params[0].CompareNoCase(QTopFid1) == 0)
				{
					m_cboTopFid1.SelectString(0, params[1]);
					commitTopFid1();
					
				}
				else if (params[0].CompareNoCase(QTopFid2) == 0)
				{
					m_cboTopFid2.SelectString(0, params[1]);
					commitTopFid2();
				}
				else if (params[0].CompareNoCase(QBotFid1) == 0)
				{
					m_cboBotFid1.SelectString(0, params[1]);
					commitBotFid1();
				}
				else if (params[0].CompareNoCase(QBotFid2) == 0)
				{
					m_cboBotFid2.SelectString(0, params[1]);
					commitBotFid2();
				}
			}

		}
	}
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CSonyAOIPropertySheet, CResizingPropertySheet)

CSonyAOIPropertySheet::CSonyAOIPropertySheet(CSonyBoard& sonyBoard)
	: CResizingPropertySheet("Sony AOI Writer"),
     m_sonyBoard(sonyBoard),
	  m_boardFidPage(*this),
     m_xrefPage(*this)
{
   init();
}

void CSonyAOIPropertySheet::init()
{
	AddPage(&m_boardFidPage);
   AddPage(&m_xrefPage);

   m_psh.dwFlags |= PSH_NOAPPLYNOW;
   m_psh.dwFlags &= ~PSH_HASHELP;
}

void CSonyAOIPropertySheet::updatePropertyPages(CPropertyPage* sendingPage)
{
   //if (sendingPage != &m_partsPage)
   //{
   //   m_partsPage.update();
   //}

   //if (sendingPage != &m_componentsPage)
   //{
   //   m_componentsPage.update();
   //}
}

BEGIN_MESSAGE_MAP(CSonyAOIPropertySheet, CResizingPropertySheet)
   ON_BN_CLICKED(IDOK, OnBnClickedExport)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CSonyAOIPropertySheet::OnInitDialog()
{
   GetDlgItem(IDCANCEL)->SetWindowText("Close");
   GetDlgItem(IDHELP  )->ShowWindow(SW_HIDE);
   //GetDlgItem(IDOK    )->ShowWindow(SW_HIDE);
	GetDlgItem(IDOK)->SetWindowText("Export");
   BOOL bResult = CResizingPropertySheet::OnInitDialog();

	UpdateExportButton();

   return bResult;
}

void CSonyAOIPropertySheet::OnClose()
{
   // TODO: Add your message handler code here and/or call default

   CResizingPropertySheet::OnClose();
}

void CSonyAOIPropertySheet::OnBnClickedExport()
{
	UpdateData(true);

	// Update active CSonyBoard
	m_boardFidPage.saveBoardNameSetting();
	// fidpage fids were saved on the fly

	// Save settings as attributes in ccz, for later restorating in GUI
	saveSettingsToAttribute();

	// Do the export
	CWaitCursor wait;
	bool success = getSonyBoard().exportAll();

	// Bye
	int endId = 9;  // nothing special about 9, just need an arg
	if (success)
		EndDialog(endId);

}

void CSonyAOIPropertySheet::UpdateExportButton()
{
	GetDlgItem(IDOK)->EnableWindow( getSonyBoard().hasValidFidSelections() );
}

void CSonyAOIPropertySheet::saveSettingsToAttribute()
{
	CCEtoODBDoc *doc = this->getSonyBoard().getDoc();
	FileStruct *file = this->getSonyBoard().getActiveFile();

	if (file == NULL || doc == NULL)
		return;
   
	int keyword = doc->getStandardAttributeKeywordIndex(standardAttributeSonyAoiSettings);

	CString fidSettings = m_boardFidPage.getSettingsString();

	if (!fidSettings.IsEmpty())
	{
		CString attributeValue = fidSettings;
      file->getBlock()->setAttrib(doc->getCamCadData(), keyword, valueTypeString, attributeValue.GetBuffer(0), attributeUpdateOverwrite, NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CSonyAoiPnXrefGrid::CSonyAoiPnXrefGrid()
: CDDBaseGrid("Sony AOI PN-XRef Grid")
, m_saveOptionEnabled(true)
{
   SetSortEnabled(true);

   // Column definition map, maps column name to local column number index.
   // Order of definitions here is default left to right order of columns.
   // This will set the column index vars, do not need to initialize them first.
   m_columnDefMap.AddColDef( QStatus,     &m_colStatus     );
   m_columnDefMap.AddColDef( QPartNumber, &m_colPartNumber );
   m_columnDefMap.AddColDef( QGeom,       &m_colGeom       );
   m_columnDefMap.AddColDef( QBitmap,     &m_colBitmap     );
   m_columnDefMap.AddColDef( QCategory,   &m_colCategory   );
   m_columnDefMap.AddColDef( QModel,      &m_colModel      );
   m_columnDefMap.AddColDef( QSave,       &m_colSave       );
   m_columnDefMap.AddColDef( QExplode,    &m_colExplode    );
   m_columnDefMap.AddColDef( QRotAdj,     &m_colRotAdj     );

   // Load possible user overrides (column swaps)
   m_columnDefMap.LoadSettings();
}

//----------------------------------------------------------------------------------

void CSonyAoiPnXrefGrid::SizeToFit(CRect &rect, bool redraw)
{
   if (!this->IsSetUp())
      return;

   int width = rect.Width();

   SetColWidth(m_colStatus,     (int)(width * .100));
   SetColWidth(m_colPartNumber, (int)(width * .140));
   SetColWidth(m_colGeom,       (int)(width * .140));
   SetColWidth(m_colBitmap,     (int)(width * .130));
   SetColWidth(m_colCategory,   (int)(width * .130));
   SetColWidth(m_colModel,      (int)(width * .130));
   SetColWidth(m_colSave,       (int)(width * .070));
   SetColWidth(m_colExplode,    (int)(width * .070));
   SetColWidth(m_colRotAdj,     (int)(width * .070));

   if (redraw)
      RedrawAll();
}

//----------------------------------------------------------------------------------

void CSonyAoiPnXrefGrid::OnSetup()
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

   SetNumberCols(9);
   SetNumberRows(0);

   QuickSetText(m_colStatus, -1,     QStatus);
   QuickSetText(m_colPartNumber, -1, QPartNumber);
   QuickSetText(m_colGeom, -1,       QGeom);
   QuickSetText(m_colBitmap, -1,     QBitmap);
   QuickSetText(m_colCategory, -1,   QCategory);
   QuickSetText(m_colModel, -1,      QModel);
   QuickSetText(m_colSave, -1,       QSave);
   QuickSetText(m_colExplode, -1,    QExplode);
   QuickSetText(m_colRotAdj, -1,     QRotAdj);

   CUGCell cell;

   if (m_saveOptionEnabled)
   {
      GetColDefault(m_colSave, &cell); 
      cell.SetCellType(UGCT_CHECKBOX);  // To make it a checkbox cell
      cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
      SetColDefault(m_colSave, &cell);
   } // else leave it as text cell

   GetColDefault(m_colExplode, &cell); 
   cell.SetCellType(UGCT_DROPLIST); 
   cell.SetLabelText("Yes\nNo\n"); 
   SetColDefault(m_colExplode, &cell);

   GetColDefault(m_colRotAdj, &cell); 
   cell.SetCellType(UGCT_DROPLIST); 
   cell.SetLabelText("0\n90\n180\n270\n"); 
   SetColDefault(m_colRotAdj, &cell);

   // Don't have menu content for this yet,it comes fromm Sony DB
   GetColDefault(m_colCategory, &cell); 
   cell.SetCellType(UGCT_DROPLIST); 
   //*skipped at this stage: cell.SetLabelText("0\n90\n180\n270\n"); 
   SetColDefault(m_colCategory, &cell);

   // Don't have menu content for this yet, it comes from Sony DB
   GetColDefault(m_colModel, &cell); 
   cell.SetCellType(UGCT_DROPLIST); 
   //*skipped at this stage:  cell.SetLabelText("0\n90\n180\n270\n"); 
   SetColDefault(m_colModel, &cell);

   this->SetSetUp(true);

   //BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   SetColWidth(-1, 0); // get rid of "row heading"
}

void CSonyAoiPnXrefGrid::FillCategoryDroplistOptions(CSonyDatabase *masterDB)
{
   if (masterDB != NULL) {
      CString categoryOptions = masterDB->getCategoryNameOptionList();

      CUGCell cell;
      GetColDefault(m_colCategory, &cell); 
      cell.SetCellType(UGCT_DROPLIST); 
      cell.SetLabelText(categoryOptions); 
      SetColDefault(m_colCategory, &cell);
   }
}

void CSonyAoiPnXrefGrid::FillModelDroplistOptions(CSonyDatabase *masterDB, int rowIndx)
{
   // These option menus vary per row, they depend on the setting of Category
   // for the component in the row. So, can't do a column-default style option list.

   if (masterDB != NULL)
   {
      CSonyPart* part = (CSonyPart*)GetRowData(rowIndx);

      if (part != NULL) // should never be null, but jic.
      {
         CString modelOptions( masterDB->getModelNameOptionList(part->getMasterCategory()) );

         CUGCell cell;
         GetCell(m_colModel, rowIndx, &cell); 
         cell.SetCellType(UGCT_DROPLIST); 
         cell.SetLabelText(modelOptions);
         SetCell(m_colModel, rowIndx, &cell);
      }
   }
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
int CSonyAoiPnXrefGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   // Whether it actually changed or not... just say it did.
   this->GetParent()->PostMessage(ID_GRIDROWEDITFINISHED, row, col);

   if (cancelFlag)
      return TRUE;

   return TRUE;
}

/////////////////////////////////////

int CSonyAoiPnXrefGrid::OnCellTypeNotify(long ID,int col,long row,long msg,LONG_PTR param)
{
   if (msg == UGCT_DROPLISTSELECT || msg == UGCT_CHECKBOXSET)
   {
      this->GetParent()->PostMessage(ID_GRIDROWEDITFINISHED, row, col);
   }
   // else various up and down and on the way events, skip them.

   return TRUE;
}

/////////////////////////////////////
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

int CSonyAoiPnXrefGrid::OnEditStart(int col, long row, CWnd **edit)
{
   // *edit = &m_myCUGEdit;

   // No cells in this grid are text edit cells.
   // There are option menus and a checkbox, that's it.
   // Disallow text editing for all cells.

   return FALSE;
}