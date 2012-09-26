// $Header: /CAMCAD/5.0/read_wrt/Asymtek_Out.cpp 11    6/17/07 8:55p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "DirDlg.h"
#include "asymtek_out.h"
#include "FileList.h"
#include "xform.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void Asymtek_WriteFiles(CString pathName, CCEtoODBDoc &doc, FormatStruct *format, int pageUnits)
{
	AsymtekWrite asymtekWrite(doc);

	CString logFile = GetLogfilePath("asymtek.log");
	asymtekWrite.WriteFile(pathName, logFile, format, pageUnits);
}


/******************************************************************************
* AsymtekWrite
*/
AsymtekWrite::AsymtekWrite(CCEtoODBDoc &doc) :
	m_pDoc(doc),
	m_displayErr(0)
{
}

AsymtekWrite::~AsymtekWrite()
{
}

int AsymtekWrite::WriteFile(CString pathName, CString logFileName, FormatStruct *format, int pageUnit)
{
	CDialogAsymtek dlg;
	if (dlg.DoModal() == IDCANCEL)
		return -1;

	EOutputFormatType formatType = dlg.GetOutputFormat();
	CString headerFileName = dlg.GetHeaderDirectory() + "\\asymtek.hdr";
	CString avwFileName = dlg.GetHeaderDirectory() + "\\asymtek.avw";
	CString outAvwFileName = pathName + dlg.GetOutputFileName() + ".avw";
	CString topFileName = pathName + dlg.GetOutputFileName() + "_top.FMC";
	CString botFileName = pathName + dlg.GetOutputFileName() + "_bot.FMC";


	// open top file for writing
	CFormatStdioFile topFile;
	if (!topFile.Open(topFileName, CFile::modeCreate|CFile::modeWrite))
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", topFileName);
      ErrorMessage(tmp);
      return -1;
   }

   // open bottom file for writing
	CFormatStdioFile botFile;
	if (!botFile.Open(botFileName, CFile::modeCreate|CFile::modeWrite))
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", botFileName);
      ErrorMessage(tmp);
      return -1;
   }

	// open log file for writing
	if (!m_logFile.Open(logFileName, CFile::modeCreate|CFile::modeWrite))   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFileName, MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }


	// Read the header file and copy to both top and bottom file
	CString asymtekUnit = "";
	if (readAndCopyHeaderFile(headerFileName, topFile, botFile, asymtekUnit) < 0)
		return -1;

	// Copy the aysmtek.avw file
	if (copyAVWFile(avwFileName, outAvwFileName) == false)
      MessageBox(NULL, "Cannot copy the Asymtek.avw file.  Make sure the file exists.", "Error", MB_ICONEXCLAMATION | MB_OK);		

	// Set the unit factor base on the output unit of Asymtek
	setUnitFactor(format, pageUnit, asymtekUnit);


	// Get active panel
	FileStruct *panel = NULL;
	int count = m_pDoc.getFileList().GetVisibleCount(blockTypePanel);
	if (count > 1)
	{
		SelectFileDlg dlg;
		dlg.doc = &m_pDoc;
		dlg.SetFileType(blockTypePanel);
		if (dlg.DoModal() != IDOK)
			return -1;

		panel = dlg.file;
	}
	else
	{
		panel = m_pDoc.getFileList().GetFirstShown(blockTypePanel);
	}

	if (panel != NULL)
	{
		topFile.WriteString("SET ACTIVE VALVE: 1\n");
		botFile.WriteString("SET ACTIVE VALVE: 1\n");


		// Check for centroid in panel
		if (formatType == formatGlue)
		{
			BlockStruct *block = panel->getBlock();
			POSITION pos = block->getHeadDataInsertPosition();
			while (pos)
			{
				InsertStruct *insert = block->getNextDataInsert(pos)->getInsert();
				if (insert->getInsertType() != insertTypePcb)
					continue;

				BlockStruct *subBlock = m_pDoc.getBlockAt(insert->getBlockNumber());
				if (AllCompHasCentriod(subBlock) == false)
				{
					MessageBox(NULL, "Cannot output as GLUE format due to some components missing centroids. Please generate component centroid and try again.",
								  "Export Error",  MB_ICONEXCLAMATION | MB_OK);
					return -1;
				}
			}
		}

		writePanelDesign(topFile, botFile, formatType, panel);

		topFile.WriteString(".end\n");
		topFile.WriteString(".endpattlist\n");

		botFile.WriteString(".end\n");
		botFile.WriteString(".endpattlist\n");
	}
	else
	{
		FileStruct *board = m_pDoc.getFileList().GetFirstShown(blockTypePcb);
		if (board == NULL)
		{
			MessageBox(NULL, "Not visible board or panel.  Abort outputing file!", "Export Error",  MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}

		if (formatType == formatGlue)
		{
			if (AllCompHasCentriod(board->getBlock()) == false)
			{
				MessageBox(NULL, "Cannot output as GLUE format due to some components missing centroids. Please generate component centroid and try again.",
								"Export Error",  MB_ICONEXCLAMATION | MB_OK);
				return -1;
			}
		}

		topFile.WriteString("SET ACTIVE VALVE: 1\n");
		botFile.WriteString("SET ACTIVE VALVE: 1\n");

		BlockStruct *block = board->getBlock();
		int mirror = board->isMirrored()?MIRROR_FLIP|MIRROR_LAYERS:0;

		writeBoardDesign(topFile, botFile, formatType, block, board->getInsertX(), board->getInsertY(), board->getScale(),
							  board->getRotation(), mirror);

		topFile.WriteString(".end\n");
		topFile.WriteString(".endpattlist\n");

		botFile.WriteString(".end\n");
		botFile.WriteString(".endpattlist\n");
	}


	topFile.Close();
	botFile.Close();
	m_logFile.Close();

   if (m_displayErr)
      Logreader(logFileName);

	return 0;
}


bool AsymtekWrite::AllCompHasCentriod(BlockStruct *block)
{
	POSITION pos = block->getHeadDataInsertPosition();
	while (pos)
	{
		InsertStruct *insert = block->getNextDataInsert(pos)->getInsert();
		if (insert->getInsertType() != insertTypePcbComponent)
			continue;

		BlockStruct *subBlock = m_pDoc.getBlockAt(insert->getBlockNumber());
		if (subBlock->GetCentroidData() == NULL)
			return false;
	}

	return true;
}

int AsymtekWrite::readAndCopyHeaderFile(CString headerFileName, CFormatStdioFile &topFile, CFormatStdioFile &botFile, CString &asymtekUnit)
{
   // open header file for reading
	CFormatStdioFile headerFile;
	if (!headerFile.Open(headerFileName, CFile::modeRead))
   {
      CString tmp;
      tmp.Format("Can not open header file [%s]", headerFileName);
      ErrorMessage(tmp);
      return -1;
   }

	// read header file and copy to both top & bottom output file
	asymtekUnit = "";
	CString line;
	while (headerFile.ReadString(line))
	{
		int tokPos = 0;		
		CString tok = line.Tokenize("=", tokPos);
		
      if (!tok.IsEmpty() && !STRCMPI(tok,"UNITS"))
		{
			tok = line.Tokenize(" \t\n", tokPos);
			if (!tok.IsEmpty())
            asymtekUnit = tok;
		}

		topFile.WriteString("%s\n", line );
		botFile.WriteString("%s\n", line);
	}

	headerFile.Close();
	return 0;
}

bool AsymtekWrite::copyAVWFile(CString avwFileName, CString outAvwFileName)
{
   // open header file for reading
	//CFormatStdioFile avwFile;
	//if (!avwFile.Open(avwFileName, CFile::modeRead))
 //  {
 //     CString tmp;
 //     tmp.Format("Can not open file [%s]", avwFileName);
 //     ErrorMessage(tmp);
 //     return -1;
 //  }

	//CFormatStdioFile outAwvFile;
	//if (!outAwvFile.Open(outAVWFileName, CFile::modeCreate|CFile::modeWrite))
 //  {
 //     CString tmp;
 //     tmp.Format("Can not open file [%s]", outAVWFileName);
 //     ErrorMessage(tmp);
 //     return -1;
 //  }

	//// read header file and copy to both top & bottom output file
	//CString line;
	//while (avwFile.ReadString(line))
	//{
	//	outAwvFile.WriteString("%s\n", line );
	//}

	//outAwvFile.Close();
	//avwFile.Close();

	return (CopyFile(avwFileName, outAvwFileName, FALSE)?true:false);
}

void AsymtekWrite::setUnitFactor(FormatStruct *format, int pageUnit, CString asymtekUnit)
{
	asymtekUnit.MakeLower();
	if (asymtekUnit == "um")
		m_unitFactor = Units_Factor(pageUnit, UNIT_MM) * format->Scale * 1000;
		
	else if (asymtekUnit == "mm")
		m_unitFactor = Units_Factor(pageUnit, UNIT_MM) * format->Scale;
		
	else if (asymtekUnit == "cm")
		m_unitFactor = (Units_Factor(pageUnit, UNIT_MM) * format->Scale) / 10;

	else if (asymtekUnit == "mil")
		m_unitFactor = Units_Factor(pageUnit, UNIT_MILS) * format->Scale;

	else if (asymtekUnit == "tenthmil")
		m_unitFactor = Units_Factor(pageUnit, UNIT_MILS) * format->Scale * 10;

	else if (asymtekUnit == "inches")
		m_unitFactor = Units_Factor(pageUnit, UNIT_INCHES) * format->Scale;

	else
		m_unitFactor = format->Scale;
}

void AsymtekWrite::writePanelDesign(CFormatStdioFile &topFile, CFormatStdioFile &botFile, EOutputFormatType outputFormat, FileStruct *panel)
{
	int mirror = panel->isMirrored()?MIRROR_FLIP|MIRROR_LAYERS:0;
	DTransform xForm(panel->getInsertX(), panel->getInsertY(), panel->getScale(), panel->getRotation(), mirror);

	BlockStruct *block = panel->getBlock();
	POSITION pos = block->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = block->getDataList().GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;

		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypePcb)
			continue;

		int insertMirror = mirror ^ insert->getMirrorFlags();
		double insertRot = panel->getRotation() + block->getRotation(); 

		if (mirror & MIRROR_FLIP)
			insertRot -= insert->getAngle();
		else
			insertRot += insert->getAngle();

      Point2 point2;
      point2.x = insert->getOriginX();
      point2.y = insert->getOriginY();
      xForm.TransformPoint(&point2);

		BlockStruct *subBlock = m_pDoc.getBlockAt(insert->getBlockNumber());
		writeBoardDesign(topFile, botFile, outputFormat, subBlock, point2.x, point2.y, insert->getScale(), insertRot, insertMirror);
	}
}

void AsymtekWrite::writeBoardDesign(CFormatStdioFile &topFile, CFormatStdioFile &botFile, EOutputFormatType outputFormat,
												BlockStruct *block, double x, double y, double scale, double rotation, int mirror)
{
	WORD partNumKey = m_pDoc.RegisterKeyWord(ATT_PARTNUMBER, 0, VT_STRING);
	DTransform xForm(x, y, scale, rotation, mirror);

	POSITION pos = block->getDataList().GetHeadPosition();
	while (pos)
	{
		DataStruct *data = block->getDataList().GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;

		InsertStruct *insert = data->getInsert();
		if (insert->getInsertType() != insertTypePcbComponent)
			continue;

		BlockStruct *subBlock = m_pDoc.getBlockAt(insert->getBlockNumber());
		if (subBlock == NULL)
			continue;

		bool insertMirror = insert->getGraphicMirrored();
		if (mirror & (MIRROR_FLIP|MIRROR_LAYERS))
			insertMirror = !insertMirror;

		double insertRot = rotation; 
		if (mirror & MIRROR_FLIP)
			insertRot -= insert->getAngle();
		else
			insertRot += insert->getAngle();

      Point2 point2;
      point2.x = insert->getOriginX();
      point2.y = insert->getOriginY();
      xForm.TransformPoint(&point2);


		CString packageName = m_pDoc.getBlockAt(insert->getBlockNumber())->getName();
		CString compRefdes = insert->getRefname();
		CString partNumber = "NOPARTNUMBER";

		Attrib *attrib = NULL;
		if (data->getAttributes() && data->getAttributes()->Lookup(partNumKey, attrib))
		{
			if (attrib != NULL)
				partNumber = attrib->getStringValue();
		}

		if (outputFormat == formatGlue)
		{
			// output the component if it is SMD
			DTransform xForm2(point2.x, point2.y, insert->getScale(), insertRot, insert->getMirrorFlags());
			
			DataStruct *centroid = subBlock->GetCentroidData();
			if (centroid == NULL)
				continue;

			InsertStruct *insert = centroid->getInsert();
			if (insert == NULL)
				continue;

			Point2 centrioidPoint;
			centrioidPoint.x = insert->getOriginX();
			centrioidPoint.y = insert->getOriginY();
			xForm2.TransformPoint(&centrioidPoint);
			

			writeSMDdata(topFile, botFile, data, partNumber, packageName, compRefdes, centrioidPoint, insertMirror);
		}
		else
		{
			// output the pins of the component is they SMD
			DTransform xForm2(point2.x, point2.y, insert->getScale(), insertRot, insert->getMirrorFlags());

			POSITION pinPos = subBlock->getDataList().GetHeadPosition();
			while (pinPos)
			{
				DataStruct *pinData = subBlock->getDataList().GetNext(pinPos);
				if (pinData == NULL || pinData->getDataType() != dataTypeInsert)
					continue;

				InsertStruct *pinInsert = pinData->getInsert();
				if (pinInsert->getInsertType() != insertTypePin)
					continue;

				Point2 pinPoint;
				pinPoint.x = pinInsert->getOriginX();
				pinPoint.y = pinInsert->getOriginY();
				xForm2.TransformPoint(&pinPoint);

				writeSMDdata(topFile, botFile, pinData, partNumber, packageName, compRefdes, pinPoint, insertMirror);
			}
		}
	}
}

void AsymtekWrite::writeSMDdata(CFormatStdioFile &topFile, CFormatStdioFile &botFile, DataStruct *data,
										 CString partNumber, CString packageName, CString refdes, Point2 point, bool mirror)
{
	WORD technologyKey = m_pDoc.RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);
	Attrib *attrib = NULL;

	if (data == NULL)
		return;
	if (data->getAttributes() == NULL || !data->getAttributes()->Lookup(technologyKey, attrib))
		return;
	if (attrib == NULL)
		return;

	CString value = attrib->getStringValue();
	if (value.CompareNoCase("SMD"))
		return;

	InsertStruct *insert = data->getInsert();
	if (mirror != true)
	{
		topFile.WriteString("COMMENT: %s %s %s\n", partNumber, packageName, refdes);
		topFile.WriteString("DOT: 1,(%.6f,%6f)\n", point.x * m_unitFactor, point.y * m_unitFactor);
	}
	else
	{
		botFile.WriteString("COMMENT: %s %s %s\n", partNumber, packageName, refdes);
		botFile.WriteString("DOT: 1,(-%.6f,%6f)\n", point.x * m_unitFactor, point.y * m_unitFactor);
	}
}



/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CDialogAsymtek dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CDialogAsymtek, CDialog)
CDialogAsymtek::CDialogAsymtek( CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAsymtek::IDD, pParent)
	, m_txtHeaderDirectory("")
	, m_optOutputFormat(FALSE)
	, m_txtOutputFileName(_T("asymtek"))
{
}

CDialogAsymtek::~CDialogAsymtek()
{
}

void CDialogAsymtek::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIRECTORY, m_txtHeaderDirectory);
	DDX_Radio(pDX, IDC_RADIO_PASTE, m_optOutputFormat);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_txtOutputFileName);
}

BEGIN_MESSAGE_MAP(CDialogAsymtek, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
END_MESSAGE_MAP()

BOOL CDialogAsymtek::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_txtHeaderDirectory = GetDirectoryFromRegistry();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDialogAsymtek::OnBnClickedButtonBrowse()
{
	CBrowse dirDlg;
	dirDlg.m_strSelDir = getApp().getUserPath();
   dirDlg.m_strTitle = "Select directory for header file";

	if (!dirDlg.DoBrowse())
      return;

	m_txtHeaderDirectory = dirDlg.m_strPath;
	WriteDirectoryToRegistry(dirDlg.m_strPath);
	UpdateData(FALSE);
}

void CDialogAsymtek::WriteDirectoryToRegistry(CString directoryPath)
{
	// Save directory path into registry

   CAppRegistrySetting("Asymtek Write", "Asymtek Header Directory").Set(directoryPath);
}

CString CDialogAsymtek::GetDirectoryFromRegistry()
{
   // Retrieve directory path from registry

   CString dirPath = CAppRegistrySetting("Asymtek Write", "Asymtek Header Directory").Get();

   if (dirPath.IsEmpty())
   {
      dirPath = CBackwardCompatRegistry().Asymtek_GetDirectoryFromRegistry();

      // Update from old obsolete key to current key
      if (!dirPath.IsEmpty())
      {
         WriteDirectoryToRegistry(dirPath);
      }
   }

	return dirPath;
}

