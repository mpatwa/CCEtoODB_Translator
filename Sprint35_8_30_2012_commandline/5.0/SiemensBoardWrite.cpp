// $Header: /CAMCAD/5.0/SiemensBoardWrite.cpp 18    6/17/07 8:54p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "ccdoc.h"
#include "SiemensBoardWrite.h"
#include "PcbUtil.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// SiemensBoardDlg dialog

IMPLEMENT_DYNAMIC(SiemensBoardDlg, CDialog)

SiemensBoardDlg::SiemensBoardDlg(CCEtoODBDoc& camcadDoc, FileStruct& fileStruct, CWnd* pParent /*=NULL*/)
: CDialog(SiemensBoardDlg::IDD, pParent)
, m_camcadDoc(camcadDoc)
, m_fileStruct(fileStruct)
, m_ProgNr(_T(""))
, m_Product(_T(""))
, m_Version(_T(""))
{
   // 1st get the project directory and extra the ProgNr, BoardGroup, and Version out of it
   CString projectDirectory = camcadDoc.GetProjectPath();
   projectDirectory.Delete(projectDirectory.GetLength()-1, 1);
   int index = projectDirectory.ReverseFind('\\');
   if (index > 0)
   {
      projectDirectory.Delete(0, index+1);
      //projectDirectory = projectDirectory.Right(projectDirectory.GetLength() - index - 1);
   }
   CString progNr = projectDirectory.Left(7);
   CString boardGroup = projectDirectory.Mid(7, 11);
   CString version = projectDirectory.Mid(5, 2);

   // 2nd get the setting on file
   LoadSettingFromAttribute ();

   // 3rd if any setting is missing then use those extracted from project directory
   if (m_ProgNr.IsEmpty())
      m_ProgNr = progNr;
   if (m_Product.IsEmpty())
      m_Product = boardGroup;
   if (m_Version.IsEmpty())
      m_Version = version;
}

SiemensBoardDlg::~SiemensBoardDlg()
{
}

void SiemensBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ProgNr);
	DDX_Text(pDX, IDC_EDIT2, m_Product);
	DDX_Text(pDX, IDC_EDIT3, m_Version);
}


BEGIN_MESSAGE_MAP(SiemensBoardDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()


// SiemensBoardDlg message handlers
void SiemensBoardDlg::LoadSettingFromAttribute()
{
	WORD keyword = (WORD)m_camcadDoc.getStandardAttributeKeywordIndex(standartAttributeSiemensDialogSetting);
	Attrib* attrib = NULL;
	if (m_fileStruct.getBlock()->getAttributes() == NULL || !m_fileStruct.getBlock()->getAttributes()->Lookup(keyword, attrib))
		return;
	
	CStringArray atributesStringArray;
   CSupString attributesSupString = attrib->getStringValue();
	attributesSupString.Parse(atributesStringArray, "|");
	
	for (int i=0; i<atributesStringArray.GetCount(); i++)
	{	
		CString attributeString = atributesStringArray.GetAt(i);
		CStringArray params;
		CSupString attributeSupString = atributesStringArray.GetAt(i);
		attributeSupString.ParseQuote(params, "=");
		
      if (attributeString.Find("ProgNr") > -1 && params.GetCount() > 1)
		{
         m_ProgNr = params[1];
      }
		else if (attributeString.Find("BoardGroup") > -1 && params.GetCount() > 1)
		{
         m_Product = params[1];
		}
		else if (attributeString.Find("Version") > -1 && params.GetCount() > 1)
		{
         m_Version = params[1];
		}
	}
}

void SiemensBoardDlg::SaveSettingToAttribute()
{
	int keyword = m_camcadDoc.getStandardAttributeKeywordIndex(standartAttributeSiemensDialogSetting);

	CString attributeString;
	attributeString.AppendFormat("%s=\"%s\"|", "ProgNr", m_ProgNr);
	attributeString.AppendFormat("%s=\"%s\"|", "BoardGroup", m_Product);
	attributeString.AppendFormat("%s=\"%s\"", "Version", m_Version);

   m_fileStruct.getBlock()->setAttrib(m_camcadDoc.getCamCadData(), keyword, valueTypeString, attributeString.GetBuffer(0), attributeUpdateOverwrite, NULL);
}

void SiemensBoardDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
	UpdateData(true);

   SaveSettingToAttribute();
}

void SiemensBoardDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void SiemensBoard_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
	

	
	SiemensBoardWrite writer(&doc, format->Scale);

	if (!writer.Write(filename))
	{
		ErrorMessage("CAMCAD could not finish writing the Siemens Board file.", "", MB_OK);
	}
}

/*
Constructor
*/
SiemensBoardWrite::SiemensBoardWrite(CCEtoODBDoc *document, double l_Scale)
{
	m_pDoc = document;
	
}

/*
SiemensBoardWrite::Write
*/
BOOL SiemensBoardWrite::Write(CString boardFileName)
{
	GeneratePinPitch(m_pDoc);
	if (!BoardFile.Open(boardFileName, CFile::modeCreate|CFile::modeWrite))
	{
		ErrorMessage("File could not be opened", "", MB_OK);
		return FALSE;
	}
	
	// generate centroid in order to get centroid of the insert.
	//m_pDoc->OnGenerateComponentCentroid();
	// must generate pin pitch here

	//BoardFile.WriteString ("ProgNr Baugruppe  Version   Gen-Datum-Zeit    Anz-Nutz EP      BE-Sachnummer    Gehäuse           Seite     Raster  Technologie\n");
	

	FileStruct * lFile = m_pDoc->getFileList().GetFirstShown(blockTypePcb);
	if (lFile == NULL)
	{
		SiemensBoardWrite::ProcessPanel();
		return TRUE;
	}

	// it is not a panel but we need to check the number of boards if a panel file exist

	
	FileStruct* fileStruct = NULL; 
	int pcbCount  = 0;
	for (POSITION pos = m_pDoc->getFileList().GetHeadPosition();pos != NULL;)
	{
		fileStruct = m_pDoc->getFileList().GetNext(pos);
	
		if (fileStruct->getBlockType() == blockTypePanel)
		{
				
			BlockStruct* block = fileStruct->getBlock();
			
			POSITION dataPosPCB = block->getHeadDataInsertPosition();
			while (dataPosPCB)
			{
				
				DataStruct* dataPCB = block->getNextDataInsert(dataPosPCB);
				// if i used 'data != NULL' instead of !data, nothing is written.
				if (dataPCB->getInsert()->getInsertType() != insertTypePcb)
					continue;
				else
					pcbCount++;
			} 
		}
	}
	CString sPCBCount = "";
	if (pcbCount == 0 )
		pcbCount = 1;
	sPCBCount.Format("%d", pcbCount);
	if (pcbCount < 10)
		sPCBCount = "00" + sPCBCount;
	else if (pcbCount < 100)
		sPCBCount = "0" + sPCBCount;

	 
	// end get panel boards count code
	
	SiemensBoardDlg dlg(*m_pDoc, *lFile);
	dlg.DoModal();// != IDOK);
	//	return false;

	CString lFileName = lFile->getName();
	BlockStruct* block = lFile->getBlock();

	POSITION dataPos = block->getHeadDataInsertPosition();
	while (dataPos)
	{
		Attrib *attrib;
		CPoint2d compCentroid;
		DataStruct* data = block->getNextDataInsert(dataPos);
		// if i used 'data != NULL' instead of !data, nothing is written.
		if (data == NULL || data->getInsert()->getInsertType() != insertTypePcbComponent) 
			continue;
		
		CString refName  = data->getInsert()->getRefname();
		
		/*CString PartNumber = "";
		Attrib *attrib = is_attvalue(m_pDoc, data->getAttributeMap(), "PARTNUMBER", 0);
		PartNumber = attrib?attrib->getStringValue():"";
		*/
		//CString geomName = "";		
		//geomName = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())->getName():"";
	
		CString PackageStyle;
		attrib = is_attvalue(m_pDoc, data->getAttributeMap(),"Package_Style" , 0);
      PackageStyle = ((attrib != NULL) ? attrib->getStringValue() : "NOPACKAGESTYLE");
		
		
		CString loaded = "TRUE";
      CString partNumber;
		if (attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_LOADED, 1))
		{
         CString value = attrib->getStringValue();
         if (value.CompareNoCase("false") == 0)
				loaded = "FALSE";
		}
      
      if (loaded == "FALSE")
		{
			partNumber = "NOINSERT";
			PackageStyle = "NOINSERT";
		}
		else
		{
			attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
         partNumber = ((attrib != NULL) ? attrib->getStringValue() : "No_Part_Number");
		}
			
		//rot.Format("%.2f", RadToDeg(data->getInsert()->getAngle()));
		// needs to get the surface
		CString surface;
		if (data->getInsert()->getGraphicMirrored() == true)
			surface = "Bottom";
		else
			surface = "Top";

		CString technology = "";
		attrib = is_attvalue(m_pDoc, data->getAttributeMap(),"TECHNO_SIPLACE" , 0);
      technology = ((attrib != NULL) ? attrib->getStringValue() : "");
		if (technology.MakeUpper() == "SMD")
			technology = "SMT";
		else if (technology.MakeUpper() == "THRU")
			technology = "THT";


		double pitch;
		attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PIN_PITCH, 0);
		if (attrib != NULL)
			pitch = attrib->getDoubleValue();
		else pitch = 0.00;
		CString sPitch = "";
		sPitch.Format("%.2f", pitch);
		
		CTime t;
      t = t.GetCurrentTime();
		CString sTime;
		sTime = t.Format("%m/%d/%Y  %H:%M:%S");
/*Feld 1 = prognr                 size   8 
Feld 2 =  layout name       size 18 
Feld 3 = version                size   8 
Feld 4 =  date             size 12 
Feld 5 =  time                     size 13 
Feld 6 =  number of boards           size    7 
Feld 7 =  refdes                       size    7 
Feld 8 =  partnumber         size 17 
Feld 8 =  package style             size 21 
Feld 8 =  page                   size   9 
Feld 8 =  pin pitch                  size   7 
Feld 8 =  technology               size   8*/
				
		BoardFile.WriteString("%-8.8s%-18.18s%-8.8s%-25.25s%-7.7s%-7.7s%-17.17s%-21.21s%-9.9s%-7.7s%-8.8s\n",dlg.m_ProgNr, dlg.m_Product, 
			dlg.m_Version, sTime, sPCBCount,  refName, partNumber, PackageStyle, surface, sPitch, technology);
		
	}

	return TRUE;
}

int SiemensBoardWrite::ProcessPanel()
{			
		CString l_pcbName;
		CString s_conversion;
		FileStruct * lFile = m_pDoc->getFileList().GetOnlyShown(blockTypePanel);
		CTMatrix l_panelMatrix;
		
		SiemensBoardDlg dlg(*m_pDoc, *lFile);
		dlg.DoModal();// != IDOK);
		//return false;
		
		if (lFile->getResultantMirror(False))
			l_panelMatrix.scale(-1 * lFile->getScale(), lFile->getScale());
		else
			l_panelMatrix.scale(lFile->getScale(), lFile->getScale());

		l_panelMatrix.rotateRadians(lFile->getRotation());

		l_panelMatrix.translate(lFile->getInsertX(), lFile->getInsertY());
		

		BlockStruct* block = lFile->getBlock();
		
		CString lFileName = lFile->getName();
		
		int pcbCount = 0;
		POSITION dataPosPCB = block->getHeadDataInsertPosition();
		while (dataPosPCB)
		{
			CTMatrix l_boardMatrix(l_panelMatrix);
			
			DataStruct* dataPCB = block->getNextDataInsert(dataPosPCB);
			// if i used 'data != NULL' instead of !data, nothing is written.
			if (dataPCB->getInsert()->getInsertType() != insertTypePcb)
				continue;
			else
				pcbCount++;
		}
		
		POSITION dataPos = block->getHeadDataInsertPosition();
		bool foundBoard = false;
		while (dataPos && foundBoard == false)
		{
			CTMatrix l_boardMatrix(l_panelMatrix);
			
			DataStruct* data = block->getNextDataInsert(dataPos);
			// if i used 'data != NULL' instead of !data, nothing is written.
			if (data->getInsert()->getInsertType() != insertTypePcb)
			{
				continue;
			}	
			l_pcbName = data->getInsert()->getRefname();

			CPoint2d boardXY = data->getInsert()->getOrigin2d();
			l_panelMatrix.transform(boardXY);

			if (data->getInsert()->getGraphicMirrored())
				l_boardMatrix.scale(-1 * data->getInsert()->getScale(), data->getInsert()->getScale());
			else
				l_boardMatrix.scale(data->getInsert()->getScale(), data->getInsert()->getScale());

			l_boardMatrix.rotateRadians(data->getInsert()->getAngle());

			l_boardMatrix.translate(boardXY);

			

			BlockStruct* pcb_Block = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber());
			if (!pcb_Block)
				continue;
			POSITION dataPos = pcb_Block->getHeadDataInsertPosition();
			while (dataPos)
			{
				Attrib *attrib;
				CPoint2d compCentroid;
				
				DataStruct* data = pcb_Block->getNextDataInsert(dataPos);
				// if i used 'data != NULL' instead of !data, nothing is written.
				if (data->getInsert()->getInsertType() != insertTypePcbComponent)
						continue;
				CPoint2d compXY = data->getInsert()->getOrigin2d();
				

				CTMatrix l_compMatrix;
				CString refName  = data->getInsert()->getRefname();
				
				//CString geomName = "";
				//geomName = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())->getName():"";

				CString PackageStyle;
				attrib = is_attvalue(m_pDoc, data->getAttributeMap(),"Package_Style" , 0);
            PackageStyle = ((attrib != NULL) ? attrib->getStringValue() : "NOPACKAGESTYLE");
		
				CString loaded = "TRUE";
				CString partNumber;
				if (attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_LOADED, 1))
				{
               CString value = attrib->getStringValue();
					if (value.CompareNoCase("false") == 0)
						loaded = "FALSE";
				}
		      
				if (loaded == "FALSE")
				{
					partNumber = "NOINSERT";
					PackageStyle = "NOINSERT";
				}
				else
				{
					attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
               partNumber = ((attrib != NULL) ? attrib->getStringValue() : "No_Part_Number");
				}

				data->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid);
				l_boardMatrix.transform(compCentroid);
				
				CString technology = "";
				attrib = is_attvalue(m_pDoc, data->getAttributeMap(), "TECHNO_SIPLACE" , 0);
            technology = ((attrib != NULL) ? attrib->getStringValue() : "");
				if (technology.MakeUpper() == "SMD")
					technology = "SMT";
				else if (technology.MakeUpper() == "THRU")
					technology = "THT";

				CString surface;
				if (data->getInsert()->getGraphicMirrored() == true)
					surface = "Bottom";
				else
					surface = "Top";

				double pitch;
				attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PIN_PITCH, 0);
				if (attrib != NULL)
					pitch = attrib->getDoubleValue();
				else pitch = 0.00;
				CString sPitch = "";
				sPitch.Format("%.2f", pitch);

				CTime t;
				t = t.GetCurrentTime();
				CString sTime;
				sTime = t.Format("%m/%d/%Y  %H:%M:%S");
				CString sPCBCount;
				sPCBCount.Format("%d", pcbCount);
				if (pcbCount < 10)
					sPCBCount = "00" + sPCBCount;
				else if (pcbCount < 100)
					sPCBCount = "0" + sPCBCount;

				//BoardFile.WriteString("%-8.8s%-18.18s%-8.8s%-25.25s%-7.7s%-7.7s%-17.17s%-21.21s%-9.9s%0.2f%-8.8s\n",dlg.m_ProgNr, dlg.m_Product, 
				//					dlg.m_Version, sTime, "1",  refName, partNumber, geomName, surface, pitch, technology);
				//BoardFile.WriteString("%s  %s   %s	 %s  %d   %s	 %s	 %s	 %s	 %0.2f	 %s\n",dlg.m_ProgNr, dlg.m_Product, dlg.m_Version, sTime, pcbCount,  refName, partNumber, geomName, surface, pitch, technology);
			
				BoardFile.WriteString("%-8.8s%-18.18s%-8.8s%-25.25s%-7.7s%-7.7s%-17.17s%-21.21s%-9.9s%-7.7s%-8.8s\n",dlg.m_ProgNr, dlg.m_Product, 
									dlg.m_Version, sTime, sPCBCount,  refName, partNumber, PackageStyle, surface, sPitch, technology);
				//BoardFile.WriteString("%-8.8s%-18.18s%-8.8s%-25.25s%-7.7s%-7.7s%-17.17s%-21.21s%-9.9s%0.2f   %-8.8s\n",dlg.m_ProgNr, dlg.m_Product, 
			//dlg.m_Version, sTime, "1",  refName, partNumber, PackageStyle, surface, pitch, technology);
		
		
			}
			foundBoard = true;
		}
	
	
	return TRUE;
		
}






// C:\Development\CAMCAD\4.5\SiemensBoardWrite.cpp : implementation file
//

