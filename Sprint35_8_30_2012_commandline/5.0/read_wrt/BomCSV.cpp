// $Header: /CAMCAD/4.6/read_wrt/BomCSV.cpp 35    4/12/07 3:36p Lynn Phung $

#include "StdAfx.h"
#include "ccdoc.h"               
#include "PcbUtil.h"
#include "SelectDelimiterDlg.h"
#include "BOMTemplate.h"
#include "BomCSV.h"
#include ".\bomcsv.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "InFile.h"

#define ID_BOMGRID 52375
#define ID_FGSORTROW 0
#define ID_FGHeaderROW 1
#define ID_FGIMPORTROW 2
#define ID_FGCLEARROW  3

#define QDEFAULTPNVALUE "Default PartNumber Value"
#define QBOMCSVREAD     "BomCSV Read"
#define QNOPOP          "NOPOP"

void ReadBOMCSV (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format)
{
	FileStruct *fileStruct = Doc->getFileList().GetOnlyShown(blockTypePcb);

	if (fileStruct == NULL || Doc == NULL)
	{
		ErrorMessage("This function can only work with a file loaded.  It can only work on a single file at a time.\nPlease make sure a file is loaded or switch your view to the file you\nwish to import into and try again.", "Cannot Process BOM CSV Import"); // show message box and return
		return;
	}

   CSelectBOMDataTypeDlg dlgBomDataTupe(0);   
   if(dlgBomDataTupe.DoModal() == IDOK) 
   {
      switch(dlgBomDataTupe.getDataTypeOption())
      {
         case OPT_DELIMITED:
            {
	            CSelectDelimiterDlg dlgDelimiter(0);
	            if(dlgDelimiter.DoModal() == IDOK)
               {
                  char delimiter = dlgDelimiter.GetDelimiter();

	               if (delimiter == DELCODE_UNKNOWN)
		               delimiter = DELCODE_COMMA;	
                  
	               int ignoreLines = dlgDelimiter.IgnoreLines();
	               int headerStarts = dlgDelimiter.GetHeaderStartsLine();
	               int rowNumber = 1;
                  ColumnAttributeArray attributeList;

	               CBOMCSVImportDlg dlgBom(fileName, Doc, format, delimiter, ignoreLines, headerStarts, rowNumber);
	               dlgBom.DoModal();
               }
            }
            break;
         case OPT_TEMPLATE:
            {
               CApplyBOMTemplateDlg dlgTemplate(0);
               if(dlgTemplate.DoModal() == IDOK)
               {
                  if(dlgTemplate.isAttributColumn())
                  {
                     CBOMCSVImportDlg dlgBom(fileName, Doc, format, &dlgTemplate);
	                  dlgBom.DoModal();
                  }
                  else
                  {
                     char delimiter =  dlgTemplate.getDelimiterChar();
                     if (delimiter == DELCODE_UNKNOWN)
		                  delimiter = DELCODE_COMMA;

                     int ignoreLines = dlgTemplate.getStartLine();
                     int headerStarts = 0;
                     int rowNumber = 1;

                     CBOMCSVImportDlg dlgBom(fileName, Doc, format, delimiter, ignoreLines, headerStarts, rowNumber, &dlgTemplate);
	                  dlgBom.DoModal();
                  }
               }
            }
            break;
      }
   }
   return;
}


//IMPLEMENT_DYNAMIC(CBOMCSVImportDlg, CDialog)
CBOMCSVImportDlg::CBOMCSVImportDlg(CString fileName , CCEtoODBDoc *Doc, FormatStruct *format, char delimiter, int ignoreLines, int headerStarts, int rowNumber, CApplyBOMTemplateDlg *dlgtemplate, CWnd* pParent /*=NULL*/)
	: CDialog(CBOMCSVImportDlg::IDD, pParent)
{
	m_fileName = fileName;
	m_format = format;
	m_Doc = Doc;
	m_fileStruct = m_Doc->getFileList().GetOnlyShown(blockTypePcb);
	m_delimiter = delimiter;
	m_ignoreLines = ignoreLines;
	
	m_headerStartsLine = headerStarts;
	m_RowNumber = rowNumber;
//	m_defaultPN = defaultPN;

   m_dbgTemplate = dlgtemplate;
   m_TemplateColumnLen = (dlgtemplate)?dlgtemplate->getColumnAttriButeList().GetCount():0;
}

CBOMCSVImportDlg::CBOMCSVImportDlg(CString fileName , CCEtoODBDoc *Doc, FormatStruct *format, CApplyBOMTemplateDlg *dlgtemplate, CWnd* pParent /*=NULL*/)
	: CDialog(CBOMCSVImportDlg::IDD, pParent)
   ,m_RowNumber(1)
{
	m_fileName = fileName;
	m_format = format;
	m_Doc = Doc;
	m_fileStruct = m_Doc->getFileList().GetOnlyShown(blockTypePcb);
	
   m_dbgTemplate = dlgtemplate;
   m_ignoreLines = dlgtemplate->getStartLine();
   m_headerStartsLine = 0;
   m_delimiter = dlgtemplate->getDelimiterChar();
   m_TemplateColumnLen = dlgtemplate->getColumnAttriButeList().GetCount();
}

CBOMCSVImportDlg::~CBOMCSVImportDlg()
{
}

void CBOMCSVImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_gbxBomGrid, m_fgBom);
	DDX_Control(pDX, IDC_cmdBomImport, m_cmdBomImport);
	DDX_Control(pDX, ID_BOMCANCEL, m_cmdCancelBomCSV);
	DDX_Control(pDX, IDC_CboBomImportRefDes, m_cboBomImport);
	DDX_Control(pDX, IDC_chkBomLoadedFlag, m_chkBomLoadedFlag);
	DDX_Control(pDX, IDC_EDIT2, m_defaultPN);

	DDX_Control(pDX, IDC_ChkEnableDefault, m_EnableDefault);
}


BEGIN_MESSAGE_MAP(CBOMCSVImportDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CboBomImportRefDes, cboBomImportRefDes_OnCbnSelchange)
	ON_BN_CLICKED(IDC_cmdBomImport, cmdBomImport_OnBnClicked)
	ON_BN_CLICKED(ID_BOMCANCEL, OnBnClickedBomcancel)
//	ON_WM_KEYDOWN()
ON_BN_CLICKED(IDC_ChkEnableDefault, OnBnClickedChkenabledefault)
ON_BN_CLICKED(IDC_chkBomLoadedFlag, OnBnClickedchkbomloadedflag)
END_MESSAGE_MAP()


// CBOMCSVImportDlg message handlers
BOOL CBOMCSVImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Create the grid
	//m_defaultPN = 	_T("Kassav' - Nou La");
	m_defaultPN.SetWindowText(getdefaultPNValueFromRegistry());
	
	m_chkBomLoadedFlag.SetCheck(TRUE);
	m_EnableDefault.SetCheck(true);
	m_fgBom.substituteForDlgItem(ID_BOMGRID, IDC_gbxBomGrid, "bomgrid", *this);
	this->ModifyStyle(WS_SIZEBOX, 0);
	initGrid();

   if(m_delimiter == DELCODE_COLUMN)
      populateGridbyTemplate();
   else
	   populateGrid();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
BEGIN_EVENTSINK_MAP(CBOMCSVImportDlg, CDialog)
	ON_EVENT(CBOMCSVImportDlg, ID_BOMGRID, DISPID_CLICK, fgBom_Click, VTS_NONE)
	ON_EVENT(CBOMCSVImportDlg, ID_BOMGRID, DISPID_KEYDOWN, fgBom_KeyDown, VTS_PI2 VTS_I2)
	ON_EVENT(CBOMCSVImportDlg, ID_BOMGRID, 26, fgBom_KeyDownEdit, VTS_I4 VTS_I4 VTS_PI2 VTS_I2)
	ON_EVENT(CBOMCSVImportDlg, ID_BOMGRID, 23, fgBom_ValidateEdit, VTS_I4 VTS_I4 VTS_PBOOL)
END_EVENTSINK_MAP()

void CBOMCSVImportDlg::initGrid()
{
	// Initialize current variant data grid
	CExtendedFlexGrid *extFg = m_fgBom.getGrid();
	extFg->put_AllowBigSelection(FALSE);
	extFg->put_AllowSelection(TRUE);
	extFg->put_AllowUserResizing(CFlexGrid::flexResizeColumns);
	extFg->put_Appearance(CFlexGrid::flex3D);
	extFg->put_BackColor(GRID_COLOR_WHITE);			
	extFg->put_BackColorAlternate(GRID_COLOR_IVORY);
	extFg->put_BackColorBkg(GRID_COLOR_GRAY);		
	extFg->put_CellFloodColor(GRID_COLOR_LIGHT_GRAY);
	extFg->put_CellFloodPercent(100);
	extFg->put_ColAlignment(-1, CFlexGrid::flexAlignLeftCenter);
	extFg->put_Editable(CFlexGrid::flexEDNone);
	extFg->put_ExplorerBar(CFlexGrid::flexExNone);
	extFg->put_ExtendLastCol(TRUE);
	extFg->put_FocusRect(CFlexGrid::flexFocusInset);
	extFg->put_HighLight(CFlexGrid::flexHighlightAlways);
	extFg->put_SelectionMode(CFlexGrid::flexSelectionFree);
	extFg->put_Rows(0);
	extFg->put_Cols(0);
	extFg->put_FixedCols(0);
	extFg->put_FrozenCols(0);
	extFg->put_ColWidthMin(1100);
}






void CBOMCSVImportDlg::populateGrid()
{
   CExtendFile cfile;
	if (!cfile.Open(m_fileName, "r"))
	{
		m_cmdBomImport.EnableWindow(FALSE);
		return;
	}

	CString line;   // temp string to hold conversion value
	if (!cfile.ReadString(line))
	{
		cfile.Close();
		m_cmdBomImport.EnableWindow(FALSE);
		return;
	}
	else
	{
		if (m_headerStartsLine == 0)
		{
         CString l_header = line;
   	   CString l_headerFinal = "";
         if(!m_TemplateColumnLen)
         {
            char x[20]; 
			   int found=1; int l_count = 0;
			   while (found > 0)
			   {
				   found = l_header.Find(m_delimiter, found+1);
				   if (found > 0) l_count++;
			   }
			   for (int i = 0; i < l_count; i++)
			   {	_itoa(i+1,x,10);
				   if (i < (l_count -1))
					   l_headerFinal = l_headerFinal + "Header" + x+ m_delimiter;
				   else
					   l_headerFinal = l_headerFinal + "Header"+ x;
			   }
         }
         else //read attribute header from template file
         {
            for(int i = 0; i < m_TemplateColumnLen; i++)
            {
               ColumnAttribute* columParam = m_dbgTemplate->getColumnAttriButeList().GetAt(i);
               if(columParam)
               {
                  if (i == 0)
                     l_headerFinal = columParam->AttributeName;
                  else
                     l_headerFinal = l_headerFinal + m_delimiter + columParam->AttributeName;
               }
            }
         }

         add_GridRow(l_headerFinal);
			add_GridRow(line);
		}
		else
			add_GridRow(line);
	}
	m_cmdBomImport.EnableWindow(TRUE);

	while (cfile.ReadString(line))
		add_GridRow(line);

	cfile.Close();
}

void CBOMCSVImportDlg::populateGridbyTemplate()
{
   CExtendFile cfile;
	if (!cfile.Open(m_fileName, "r"))
	{
		m_cmdBomImport.EnableWindow(FALSE);
		return;
	}

	CString line; // temp string to hold conversion value
	if (!cfile.ReadString(line))
	{
		cfile.Close();
		m_cmdBomImport.EnableWindow(FALSE);
		return;
	}
	else
	{
      if (m_headerStartsLine == 0)
         add_GridRow(QTEMPLATENAME,m_dbgTemplate->getColumnAttriButeList());
      add_GridRow(line,m_dbgTemplate->getColumnAttriButeList());
	}

	m_cmdBomImport.EnableWindow(TRUE);

	while (cfile.ReadString(line))
		add_GridRow(line,m_dbgTemplate->getColumnAttriButeList());

	cfile.Close();
}

void CBOMCSVImportDlg::add_GridRow(CString line)
{
	
	bool firstRow = false;
	if (m_headerStartsLine==0)
	{
		firstRow = true;
		m_headerStartsLine = -1;
	}
	if (line.IsEmpty() || line.Find(m_delimiter) < 0)
	{
		m_RowNumber++;
		m_ignoreLines--;
		return;
	}
	
	if (m_headerStartsLine == m_RowNumber)
	{
		firstRow = true;
		m_ignoreLines--;
		m_RowNumber++;
	}


	if (m_ignoreLines > 1 && !firstRow)
	{
		m_RowNumber++;
		m_ignoreLines--;
		return;
	}

	CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
	if (!firstRow)
		firstRow = fgBom->get_Rows() < 1;
	
	
	bool rowAdded = false;
	bool addTok = false;
	bool startsWithQuote = false;

	int quoteCount = 0;
	int currentCol = 0;

   // For Template
   int selectedCol = 1;

	CString cellData;
	for (int idx = 0; idx < line.GetLength(); idx++)
	{
		char chr = line[idx];

		if (chr == '"')
		{
			// Only count as quoted value if it starts with a quote
			// Eg.  "value" is quoted
			//      va"lue" is not quoted
			// Therefore:
			//			"val,ue" is a single value
			//			va"l,ue" is 2 seperate values one as va"l and the other as ue"
			if (quoteCount == 0)
			{
				CString tempData = cellData;
				tempData.Trim();
				startsWithQuote = tempData.IsEmpty();
			}

			cellData.AppendChar(chr);
			quoteCount++;
			addTok = false; 
		}
		else if (chr == m_delimiter && idx !=0)
		{
			if (quoteCount % 2 == 0)
         {
				addTok = true;	// only add if not within quotes
         }
         else
				cellData.AppendChar(chr); // else add the comma
		}

		else if (chr == m_delimiter && idx == 0)
		{
			// do nothing.  it is the delimiter as the first char, skip it.
         if (quoteCount % 2 == 0)
         {
				addTok = true;	// only add if not within quotes
         }
		}
		else
		{
			cellData.AppendChar(chr);
			addTok = false;	
		}

      //only test for Template with column specify
      if(addTok && (!firstRow || (firstRow && m_headerStartsLine != -1))&& 
         m_TemplateColumnLen)
      {
         if(currentCol < m_TemplateColumnLen)
         {
            ColumnAttribute* columParam = m_dbgTemplate->getColumnAttriButeList().GetAt(currentCol);
            //pick up the column is asigned by template file
            if((columParam && selectedCol != columParam->columnNum))
            {
               addTok = false;
               cellData.Empty();
            }
         }
         else //only show the defined columns in the Template file
            break;
         selectedCol ++; //only used for Template
      }

		if (addTok || 			// only force add if final value and data exists
			(idx == (line.GetLength() -1) && !cellData.IsEmpty()))	
		{
			// if we're adding a quoted value, then set the token string and reset the quote count
			if (quoteCount > 0)
			{
				quoteCount = 0;
				CString copyOfData = cellData;
				copyOfData.Trim();

				if (copyOfData[0] == '"' && copyOfData[copyOfData.GetLength()-1] == '"')
					cellData = copyOfData.Mid(1, copyOfData.GetLength()-2);
			}

			// remove leading/trainling whitespace
			cellData.Trim();

			// if we haven't added our new row yet, then add it
			if (!rowAdded)
			{
				fgBom->addItem("");
				rowAdded = true;

				if (firstRow)
					fgBom->put_FixedRows(1);

							
			}

			// If first row add the column to the grid
			
			if (firstRow) 
			{
				fgBom->put_Cols(currentCol + 1);           
			   m_cboBomImport.AddString(cellData);
			}
			
			// if current column is greater than cols available, then exit
			if (currentCol >= fgBom->get_Cols())  
				return;

			// Set the cell data on last row added
			int curRow;
			
			curRow = fgBom->get_Rows()-1;

			fgBom->setCell(curRow, currentCol++, cellData);

				
			cellData.Empty();
		}
	}

	if (firstRow)
	{
		fgBom->addItem("", ID_FGSORTROW);
		fgBom->put_Cell(CFlexGrid::flexcpAlignment, 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)0), 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)CFlexGrid::flexAlignCenterCenter));
		fgBom->put_Cell(CFlexGrid::flexcpFontBold, 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)0), 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)TRUE));

		fgBom->addItem("");
		fgBom->put_Cell(CFlexGrid::flexcpChecked, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)CFlexGrid::flexChecked));
		fgBom->put_Cell(CFlexGrid::flexcpText, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant("Import"));

      fgBom->addItem("");
		fgBom->put_Cell(CFlexGrid::flexcpChecked, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)CFlexGrid::flexChecked));
		fgBom->put_Cell(CFlexGrid::flexcpText, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant("Clear before importing"));
		
      fgBom->put_FixedRows(4);

		
		int defaultLoc = m_cboBomImport.FindString(0, "Ref");
		if (defaultLoc >= 0)
		{
			m_cboBomImport.SetCurSel(defaultLoc);
			refreshRefDesCol();
		}

		//else
		//	m_cboBomImport.SetCurSel(0);

		//refreshRefDesCol();
	}
}

void CBOMCSVImportDlg::add_GridRow(CString line, ColumnAttributeArray& columnattrlist)
{
	bool firstRow = false;  
	if (m_headerStartsLine == 0)
	{
		firstRow = true;
		m_headerStartsLine = -1;
	}
   else if (m_headerStartsLine == m_RowNumber)
	{
		firstRow = true;
		m_ignoreLines--;
		m_RowNumber++;
	}
   else 
   {
      CString dataline = line;
      if (dataline.Trim().IsEmpty() || m_ignoreLines > 1)
      {
	      m_RowNumber++;
	      m_ignoreLines--;
	      return;
      }
   }	   

	CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
	if (!firstRow)
		firstRow = (fgBom->get_Rows() < 1);	
	
	bool rowAdded = false;
	int currentCol = 0;

   //No columns in template file
   if(!columnattrlist.GetCount())
      return;

   for(int i = 0; i < columnattrlist.GetCount(); i++)
   {
      //read column attribute from template
      ColumnAttribute* columParam = columnattrlist.GetAt(i);
      int currentCol = i;    
      int startIndex = columParam->startcharIdx - 1;
      int endIndex = (columParam->endcharIdx < line.GetLength())?columParam->endcharIdx:line.GetLength();
      int datalen = endIndex - startIndex;         

      //retrieve data from input file
	   CString cellData;   
      if(datalen > 0 && startIndex >= 0 
         && endIndex <= line.GetLength())
      {
         cellData = line.Mid(startIndex,datalen);			
         // remove leading/trainling whitespace
		   cellData.Trim();
       }

       // if we haven't added our new row yet, then add it
		if (!rowAdded)
		{
			fgBom->addItem("");
			rowAdded = true;

			if (firstRow)
				fgBom->put_FixedRows(1);						
		}

       // If first row add the column to the grid
	   if (firstRow) 
	   {		     
         fgBom->put_Cols(currentCol + 1);
         
         //read header from template
         if(m_headerStartsLine == -1)
         {
            m_cboBomImport.AddString(columParam->AttributeName);     
            cellData = columParam->AttributeName;
         }
         else // read header from input file
         {
            m_cboBomImport.AddString(cellData);
         }
	   }
         
      // Set the cell data on last row added
      int curRow = fgBom->get_Rows() - 1;
      fgBom->setCell(curRow, currentCol , cellData);      

   }/*for*/ 

   //importer
   if (firstRow)
	{
		fgBom->addItem("", ID_FGSORTROW);
		fgBom->put_Cell(CFlexGrid::flexcpAlignment, 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)0), 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)CFlexGrid::flexAlignCenterCenter));
		fgBom->put_Cell(CFlexGrid::flexcpFontBold, 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)0), 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)TRUE));

		fgBom->addItem("");
		fgBom->put_Cell(CFlexGrid::flexcpChecked, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)CFlexGrid::flexChecked));
		fgBom->put_Cell(CFlexGrid::flexcpText, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant("Import"));

		fgBom->addItem("");
		fgBom->put_Cell(CFlexGrid::flexcpChecked, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant((long)CFlexGrid::flexChecked));
		fgBom->put_Cell(CFlexGrid::flexcpText, 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)0), 
			COleVariant((long)fgBom->get_Rows()-1), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant("Clear before importing"));

		fgBom->put_FixedRows(4);

      
		int defaultLoc = m_cboBomImport.FindString(0, "Ref");
		if (defaultLoc >= 0)
		{
			m_cboBomImport.SetCurSel(defaultLoc);
			refreshRefDesCol();
		}
	}
}

void CBOMCSVImportDlg::fgBom_Click()
{
	CExtendedFlexGrid *fgBom = m_fgBom.getGrid();

	int mouseRow = fgBom->get_MouseRow();
	int mouseCol = fgBom->get_MouseCol();

	if (mouseRow == ID_FGSORTROW)
	{
		bool currentlyAscending = fgBom->getCellText(mouseRow, mouseCol) == "»";
		fgBom->put_Cell(CFlexGrid::flexcpText, 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)0), 
			COleVariant((long)ID_FGSORTROW), 
			COleVariant((long)fgBom->get_Cols()-1), 
			COleVariant(""));
		fgBom->setCurrentCell(mouseRow, mouseCol);

		if (!currentlyAscending)
		{
			fgBom->put_Sort(CFlexGrid::flexSortGenericAscending);
			fgBom->setCell(mouseRow, mouseCol, "»");
		}
		else
		{
			fgBom->put_Sort(CFlexGrid::flexSortGenericDescending);
			fgBom->setCell(mouseRow, mouseCol, "«");
		}
	}
	else
	{
		if ((mouseRow == ID_FGIMPORTROW || mouseRow == ID_FGCLEARROW)&& m_refDesCol == mouseCol)
			return;

		fgBom->setCurrentCell(mouseRow, mouseCol);
		fgBom->EditCell();
	}
}


void CBOMCSVImportDlg::cboBomImportRefDes_OnCbnSelchange()
{
	refreshRefDesCol();
}

void CBOMCSVImportDlg::refreshRefDesCol()
{
	// m_refDesCol
	CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
	CString lbText;
	m_cboBomImport.GetLBText(m_cboBomImport.GetCurSel(), lbText);
	for (int i=0; i<fgBom->get_Cols(); i++)
	{
		if (fgBom->getCellText(ID_FGHeaderROW, i) == lbText)
		{
			set_refDesCol(i);
			return;
		}
	}
}

int  CBOMCSVImportDlg::set_refDesCol(int col)
{
	CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
	if (col > fgBom->get_Cols()-1 || col < 0)
		return -1;

	m_refDesCol = col;

	fgBom->put_Cell(CFlexGrid::flexcpChecked, 
		COleVariant((long)ID_FGIMPORTROW), 
		COleVariant((long)m_refDesCol), 
		COleVariant((long)ID_FGIMPORTROW), 
		COleVariant((long)m_refDesCol), 
		COleVariant((long)CFlexGrid::flexChecked));

	return col;
}

void CBOMCSVImportDlg::cmdBomImport_OnBnClicked()
{
	importFromGrid();
	CDialog::OnOK();
}

void CBOMCSVImportDlg::ClearAttributes(BlockStruct *block, CExtendedFlexGrid *fgBom)
{
   if(!block || !fgBom) return;

   for (CDataListIterator insertIterator(*block, insertTypePcbComponent);insertIterator.hasNext();)
   {   
      DataStruct* data = insertIterator.getNext();
      CAttributes *attribMap = data->getAttributesRef();
      if(!attribMap) continue;

      for (int idxCol=0; idxCol<fgBom->get_Cols(); idxCol++)
      {
         if (idxCol == m_refDesCol || !fgBom->getCellChecked(ID_FGCLEARROW, idxCol))
            continue;

         CString attribKey = fgBom->getCellText(ID_FGHeaderROW, idxCol);
         int attribKW = m_Doc->getCamCadData().getKeyWordArray().getKeywordIndex(attribKey);
         CAttribute* attrib = NULL;
         if(attribMap->Lookup(attribKW,attrib) && attrib)
         {
            attribMap->deleteAttribute(attribKW);
         }
      }
   }
}

void CBOMCSVImportDlg::importFromGrid()
{
   bool allowLoadedChange = (m_chkBomLoadedFlag.GetCheck() != 0);
   CString defaultUnloadedPN;
   m_defaultPN.GetWindowText(defaultUnloadedPN);
   setdefaultPNValueToRegistry(defaultUnloadedPN);

   if (allowLoadedChange)   // set all the component loaded flag to false as a start then add true to each one found in the bom
      set_CompsLoaded(false);

   CString refDesNotInDoc = "";
   CString refDesNotInBOM = "@#$";
   CString refDesInBOM = "@#$";

   CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
   BlockStruct *block = m_fileStruct->getBlock();

   // clear attribute before importing
   ClearAttributes(block, fgBom);

   for (int idxRow=ID_FGCLEARROW+1, rowShift = 1; idxRow<fgBom->get_Rows(); idxRow+=rowShift, rowShift = 1)
   {
      //Search all components by comma, the last component does not have comma
      CString refDes = fgBom->getCellText(idxRow, m_refDesCol);
      while(!refDes.Trim().IsEmpty() && refDes.GetAt(refDes.GetLength() - 1) == DELCODE_COMMA && (idxRow + rowShift) < fgBom->get_Rows())
      {
         CString cur_refDes = fgBom->getCellText(idxRow + rowShift, m_refDesCol);          
         if(cur_refDes.Trim().IsEmpty())
            break;
         refDes.Append(cur_refDes);
         rowShift ++; 
      }

      //Parse RefDes String
      CStringArray refDesArray;
      CSupString superRefDes = refDes;
      int Num_Comps = superRefDes.ParseQuote(refDesArray,",");

      for(int Idx_Cmp = 0; Idx_Cmp < Num_Comps; Idx_Cmp++)
      {
         refDes = refDesArray.GetAt(Idx_Cmp);
         refDesInBOM = refDesInBOM + refDes + "@#$";     		
         DataStruct *data = block->FindData(refDes);
         if (data == NULL)   // a refdes is in the bom but not in the grid.
         {
            refDesNotInDoc = refDesNotInDoc + refDes + "@#$";
            continue;
         }

         bool loaded = false;
         for (int idxCol=0; idxCol<fgBom->get_Cols(); idxCol++)
         {
            if (idxCol == m_refDesCol || !fgBom->getCellChecked(ID_FGIMPORTROW, idxCol))
               continue;

            CString attribKey = fgBom->getCellText(ID_FGHeaderROW, idxCol);
            CString attribValue = fgBom->getCellText(idxRow, idxCol);
            m_Doc->SetUnknownAttrib(&(data->getAttributesRef()), attribKey, attribValue, SA_OVERWRITE, NULL);
            loaded = true;
         }

         // Set loaded if an attrib was set
         if (loaded && allowLoadedChange)
         {
            m_Doc->SetUnknownAttrib(&(data->getAttributesRef()), ATT_LOADED, "TRUE", SA_OVERWRITE, NULL);
            data->setColorOverride(false);
         }
         //else
         //m_Doc->SetUnknownAttrib(&(data->getAttributesRef()), ATT_LOADED, "FALSE", SA_OVERWRITE, NULL);
      }
   }

   // Optionally set default PN on LOADED=FALSE comps
   if (m_EnableDefault.GetCheck())
      set_UnloadedComponentPNAttribute(m_Doc, m_fileStruct, defaultUnloadedPN);

   // Hide unloaded Real Parts
   m_Doc->HideRealpartsForUnloadedComponents(*m_fileStruct);

   // Fill out refDesNotInBOM
   CString lFileName = m_fileStruct->getName();
   BlockStruct* lBlock = m_fileStruct->getBlock();
   CString tempRef = "";
   POSITION dataPos = lBlock->getHeadDataInsertPosition();
   while (dataPos)
   {
      DataStruct* lData = lBlock->getNextDataInsert(dataPos);
      if (lData == NULL || lData->getInsert()->getInsertType() != insertTypePcbComponent) 
         continue;

      CString refNameDoc  = lData->getInsert()->getRefname();

      refNameDoc = refNameDoc.Trim();
      tempRef = refNameDoc + "@#$";
      if (refNameDoc.IsEmpty())
         continue;
      if (int iFound = refDesInBOM.Find("@#$" + tempRef) > -1)
         continue;
      refDesNotInBOM = refDesNotInBOM + tempRef;


   }
   // end code for what refDesNotInBOM

   // now write the bom diff file
   /*# Log File Generated By BOM Explorer
   # Mentor Graphics
   # 11/14/2005 12:03:19 PM
   # -----------------------------------
   # All Parts in the BOM that Do Not Match a Component in the CAD
   REFDES
   C200
   C201
   ...
   # All Parts in the CAD that Do Not Match a Component in the BOM
   J1
   J2
   ..*/
   CTime t;
   t = t.GetCurrentTime();
   CString sTime;
   sTime = t.Format("%m/%d/%Y  %H:%M:%S");
   CString fileName = GetLogfilePath("UpdateBOMLogFile.log");
   FILE *fp = fopen(fileName,  "wt");
   fprintf(fp, "# Log File Generated By CAMCAD\n");
   fprintf(fp, "# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   fprintf(fp, "# %s\n", sTime);
   fprintf(fp, "# ---------------------------------------\n");
   fprintf(fp, "# All Parts in the BOM that Do Not Match a Component in the CAD\n\n");
   CString temp;
   int count = 0;
   int found = 0;
   while (found > -1)
   {
      found = refDesNotInDoc.Find("@#$", 0);
      if (found > -1)
      {
         temp = refDesNotInDoc.Mid(0, found);
         if (!temp.IsEmpty())
         {
            count++;
            fprintf(fp, "%s\n", temp);
         }
         refDesNotInDoc = refDesNotInDoc.Right(refDesNotInDoc.GetLength() - found - 3);
      }
   }
   if (count == 0)
      fprintf(fp, "None.  All parts in BOM found in CAD.\n");


   fprintf(fp, "\n\n");
   fprintf(fp, "# All Parts in the CAD that Do Not Match a Component in the BOM\n\n");
   count = 0;
   found = 0;
   while (found > -1)
   {
      found = refDesNotInBOM.Find("@#$", 0);
      if (found > -1)
      {
         temp = refDesNotInBOM.Mid(0, found);
         if (!temp.IsEmpty())
         {
            count++;
            fprintf(fp, "%s\n", temp);
         }
         refDesNotInBOM = refDesNotInBOM.Right(refDesNotInBOM.GetLength() - found - 3);
      }
   }
   if (count == 0)
      fprintf(fp, "None.  All parts in CAD found in BOM.\n");

   fclose(fp);
   ShellExecute(NULL,"open",fileName,NULL,NULL,SW_SHOW);

}

void CBOMCSVImportDlg::set_CompsLoaded(bool loaded)
{
	if (m_Doc == NULL)
		return;

	FileStruct *fileStruct = m_Doc->getFileList().GetOnlyShown(blockTypePcb);

	if (loaded)
		set_ComponentLoadedData(m_Doc, m_fileStruct);
	else
	{
		set_ComponentNotLoadedData(m_Doc, m_fileStruct);
		color_ComponentNotLoadedData(m_Doc, m_fileStruct, 120, 120, 120);
	}
}


void CBOMCSVImportDlg::fgBom_KeyDown(short* KeyCode, short Shift)
{
	CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
	if (fgBom->get_Row() == ID_FGSORTROW)
		return;

	int row = fgBom->get_Row();
	int col = fgBom->get_Col();

	switch (*KeyCode)
	{
	//case 9:	// Hard Tab
	//	{
	//		int newCol = Shift ? col-1 : col+1;
	//		if (newCol < 0)
	//			newCol = fgBom->get_Cols() -1;
	//		if (newCol >= fgBom->get_Cols())
	//			newCol = 0;

	//		fgBom->setCurrentCell(row, newCol);
	//		break;
	//	}
	case 13:	// Carriage return
	case 10: // Line feed
		{
			int newRow = Shift ? row-1 : row+1;
			if (newRow < 1)
				newRow = fgBom->get_Rows() -1;
			if (newRow >= fgBom->get_Rows())
				newRow = 0;

			fgBom->setCurrentCell(newRow, col);
			break;
		}
	default:
		if (row == ID_FGSORTROW)
			return;

		fgBom->setCurrentCell(row, col);
		fgBom->EditCell();
		break;
	}
}

void CBOMCSVImportDlg::fgBom_KeyDownEdit(long Row, long Col, short* KeyCode, short Shift)
{
	CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
	if (Row == ID_FGSORTROW)
		return;

	switch (*KeyCode)
	{
	//case 9:	// Hard Tab
	//	{
	//		int newCol = Shift ? Col-1 : Col+1;
	//		if (newCol < 0)
	//			newCol = fgBom->get_Cols() -1;
	//		if (newCol >= fgBom->get_Cols())
	//			newCol = 0;

	//		fgBom->setCurrentCell(Row, newCol);
	//		break;
	//	}
	case 13:	// Carriage return
	case 10: // Line feed
		{
			int newRow = Shift ? Row-1 : Row+1;
			if (newRow < 1)
				newRow = fgBom->get_Rows() -1;
			if (newRow >= fgBom->get_Rows())
				newRow = 0;

			fgBom->setCurrentCell(newRow, Col);
			break;
		}
	}
}

void CBOMCSVImportDlg::setdefaultPNValueToRegistry(CString defaultValue)
{
   CAppRegistrySetting(QBOMCSVREAD, QDEFAULTPNVALUE).Set(defaultValue);
}

CString CBOMCSVImportDlg::getdefaultPNValueFromRegistry()
{
   // Retrieve directory path from registry

   CString  DefaultVal = CAppRegistrySetting(QBOMCSVREAD, QDEFAULTPNVALUE).Get();

   if (DefaultVal.IsEmpty())
   {
      DefaultVal = QNOPOP;
      setdefaultPNValueToRegistry(DefaultVal);
   }

	return DefaultVal;
}

void CBOMCSVImportDlg::OnBnClickedBomcancel()
{
	CDialog::OnCancel();
}

void CBOMCSVImportDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	// CDialog::OnOK();
}

void CBOMCSVImportDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}
void CBOMCSVImportDlg::fgBom_ValidateEdit(long Row, long Col, BOOL* Cancel)
{
	if (Row == ID_FGHeaderROW)
	{
		CExtendedFlexGrid *fgBom = m_fgBom.getGrid();
		CString headerCellBeforeEdit = fgBom->getCellText(Row, Col);
		CString newHeader = fgBom->get_EditText();

		int headerIdx = m_cboBomImport.FindStringExact(0, headerCellBeforeEdit);
		if (headerIdx < 0 || newHeader == headerCellBeforeEdit)
			return;

		int selIdx = m_cboBomImport.GetCurSel();
		m_cboBomImport.DeleteString(headerIdx);
		m_cboBomImport.InsertString(headerIdx, newHeader);
		m_cboBomImport.SetCurSel(selIdx);
		refreshRefDesCol();		
	}
}

void CBOMCSVImportDlg::OnBnClickedChkenabledefault()
{
	if (m_EnableDefault.GetCheck())
		m_defaultPN.EnableWindow(true);
	else
		m_defaultPN.EnableWindow (false);
	// TODO: Add your control notification handler code here
}

void CBOMCSVImportDlg::OnBnClickedchkbomloadedflag()
{
	// TODO: Add your control notification handler code here
}

/////////////////////////////////////////////////////////////////////////////
// CSelectBOMDataTypeDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSelectBOMDataTypeDlg, CDialog)
CSelectBOMDataTypeDlg::CSelectBOMDataTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectBOMDataTypeDlg::IDD, pParent)
	, m_optDelimited(FALSE)
{
   
}

CSelectBOMDataTypeDlg::~CSelectBOMDataTypeDlg()
{
}

void CSelectBOMDataTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_DELIMITED, m_optDelimited);
}

BEGIN_MESSAGE_MAP(CSelectBOMDataTypeDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(IDC_RADIO_DELIMITED, OnBnClickedRadioBOMDataType)
   ON_BN_CLICKED(IDC_RADIO_COLUMNIZED, OnBnClickedRadioBOMDataType)
END_MESSAGE_MAP()

void CSelectBOMDataTypeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
   UpdateData(TRUE);
   OnOK();
}

void CSelectBOMDataTypeDlg::OnBnClickedRadioBOMDataType()
{
	// TODO: Add your control notification handler code here
   UpdateData(TRUE);   

   

}

