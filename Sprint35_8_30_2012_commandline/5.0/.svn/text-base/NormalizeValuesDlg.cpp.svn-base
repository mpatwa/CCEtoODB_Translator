// $Header: /CAMCAD/5.0/NormalizeValuesDlg.cpp 22    6/17/07 8:53p Kurt Van Ness $

#include "stdafx.h"
#include "ccdoc.h"
#include <math.h>
#include "compvalues.h"
#include "NormalizeValuesDlg.h"

//functions declaration
BOOL GetLicenseString(CLSID clsid, BSTR &licStr);

/******************************************************************************
* OnNormalizeValues
*/
void CCEtoODBDoc::OnNormalizeValues() 
{
	NormalizeValuesDlg dlg(this);
	dlg.DoModal();
}

///////////////////////////////////////////////////////////////////////////////////////////////

// NormalizeValuesDlg dialog

IMPLEMENT_DYNAMIC(NormalizeValuesDlg, CDialog)
//NormalizeValuesDlg::NormalizeValuesDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(NormalizeValuesDlg::IDD, pParent)
//{
//}

NormalizeValuesDlg::NormalizeValuesDlg(CCEtoODBDoc *document, CWnd* pParent /*=NULL*/)
	: CDialog(NormalizeValuesDlg::IDD, pParent)
{
	doc = document;
}

NormalizeValuesDlg::~NormalizeValuesDlg()
{
}

void NormalizeValuesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NormalizeValuesDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(NormalizeValuesDlg, CDialog)
   ON_EVENT(NormalizeValuesDlg, IDC_VSFLEX, DISPID_MOUSEUP, MouseUpVsflex, VTS_I2 VTS_I2 VTS_R4 VTS_R4)
END_EVENTSINK_MAP()

// NormalizeValuesDlg message handlers

/******************************************************************************
* NormalizeValuesDlg::OnInitDialog
*/
BOOL NormalizeValuesDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   BSTR licenseKey = NULL;
   CRect rc;
    
   if (!GetLicenseString(m_flexGrid.GetClsid(), licenseKey))
      licenseKey = NULL;

   
   GetDlgItem(IDC_OCXBOX)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_OCXBOX)->GetWindowRect(&rc);
   ScreenToClient(&rc);
   m_flexGrid.Create("FlexGrid", WS_VISIBLE, rc, this, IDC_VSFLEX,
                      NULL, FALSE, licenseKey);

   m_flexGrid.put_Editable(CFlexGrid::flexEDNone);
   m_flexGrid.put_ExplorerBar(CFlexGrid::flexExSortShow);
   m_flexGrid.put_AllowSelection(TRUE);
   m_flexGrid.put_SelectionMode(CFlexGrid::flexSelectionByRow);
   m_flexGrid.put_AllowBigSelection(FALSE);
   m_flexGrid.put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   m_flexGrid.put_GridLines(CFlexGrid::flexGridFlat);
   m_flexGrid.put_ExtendLastCol(TRUE);
   m_flexGrid.put_WordWrap(FALSE);
   m_flexGrid.put_AutoSearch(CFlexGrid::flexSearchFromCursor);
   m_flexGrid.put_AutoResize(FALSE);
   m_flexGrid.put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);
   //m_flexGrid.put_BackColorAlternate(;
   m_flexGrid.put_ColAlignment(0,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(1,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(2,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(3,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(4,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(5,CFlexGrid::flexAlignLeftBottom);
  
  
   m_flexGrid.put_Cols(4);
   m_flexGrid.put_Rows(1);
   m_flexGrid.put_FixedCols(0);
   m_flexGrid.put_FixedRows(1);
   
   COleVariant row1, col, newValue;
   row1 = (GRID_CNT_TYPE)0;
   col = (GRID_CNT_TYPE)0;
   newValue = "DEVICE";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col, row1, col, newValue);
   
   row1 = (GRID_CNT_TYPE)0;
   col = (GRID_CNT_TYPE)1;
   newValue = "VALUE";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col, row1, col, newValue);

   row1 = (GRID_CNT_TYPE)0;
   col = (GRID_CNT_TYPE)2;
   newValue = "NORMALIZED VALUE";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col, row1, col, newValue);

   row1 = (GRID_CNT_TYPE)0;
   col = (GRID_CNT_TYPE)3;
   newValue = "VALUE UNITS";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col, row1, col, newValue);

  //check_value_attributes(doc);

   fillGrid();
   return TRUE;
}


/******************************************************************************
* NormalizeValuesDlg::fillGrid
*/
void NormalizeValuesDlg::fillGrid()
{

	/*setItemToGrid(1, 0, "CAPACITOR");
	setItemToGrid(2, 0, "INDUCTOR");
	setItemToGrid(3, 0, "RESITOR");*/
    
	//m_flexGrid.ColComboList(6) = "Default|Component Outline|Geometry Extents|Pin Centers|Pin Extents|XY Value"
	CMapStringToString strMap;
	CString temp;
		
	WORD keyWord = doc->RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);
	WORD valKeyword = doc->RegisterKeyWord(ATT_VALUE, 0, VT_STRING);
	// If there was a cross reference loaded, make sure the matches show up
	CString device, value, normValue;
	for (int cnt=0; cnt<doc->getMaxBlockIndex(); cnt++)
	{
		BlockStruct *block = doc->getBlockAt(cnt);

		if (!block)
			continue;

		Attrib* attrib = NULL;

		POSITION dataPos = block->getDataList().GetHeadPosition();
		while (dataPos)
		{
			DataStruct* data = block->getDataList().GetNext(dataPos);

			if (!data || !data->getAttributesRef() || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
				continue;
			
			if (!data->getAttributesRef()->Lookup(keyWord, attrib))
				continue;

			CString type;

			if (!attrib)
				continue;

         type = attrib->getStringValue();			
			
			if (!type.Left(9).CompareNoCase("Capacitor"))
			{
				if (!data->getAttributesRef()->Lookup(valKeyword, attrib))
					continue;

            if (!attrib)
					continue;

				CString itemName = attrib->getStringValue();	

				// Don't allow duplicates
				if (strMap.Lookup(itemName, temp))
				   continue;

				strMap.SetAt(itemName, itemName);

				// add
				long row = strMap.GetCount();
				m_flexGrid.put_Rows(row);
				setItemToGrid(row, 3, "uF");
				setItemToGrid(row, 0, type);	
				setItemToGrid(row, 1, itemName);				
			}
			else if (!type.Left(8).CompareNoCase("Inductor"))
			{
				if (!data->getAttributesRef()->Lookup(valKeyword, attrib))
					continue;

				if (!attrib)
					continue;

				CString itemName = attrib->getStringValue();	

				// Don't allow duplicates
				if (strMap.Lookup(itemName, temp))
				   continue;

				strMap.SetAt(itemName, itemName);

				// add
				long row = strMap.GetCount();
				m_flexGrid.put_Rows(row);
				setItemToGrid(row, 3, "mH");
				setItemToGrid(row, 0, type);	
				setItemToGrid(row, 1, itemName);	
			}
			else if (!type.Left(8).CompareNoCase("Resistor"))
			{
				if (!data->getAttributesRef()->Lookup(valKeyword, attrib))
					continue;

				if (!attrib)
					continue;

				CString itemName = attrib->getStringValue();	

				// Don't allow duplicates
				if (strMap.Lookup(itemName, temp))
				   continue;

				strMap.SetAt(itemName, itemName);

				// add
				long row = strMap.GetCount();
				m_flexGrid.put_Rows(row);
				setItemToGrid(row, 3, "OHM");
				setItemToGrid(row, 0, type);	
				setItemToGrid(row, 1, itemName);	
			}
		}
	}

	WORD normKw = doc->RegisterKeyWord("VALUE_NORMALIZED", 0, VT_STRING);
	WORD valUnitKw = doc->RegisterKeyWord("VALUE_UNIT", 0, VT_STRING);
	COleVariant rowi;
	COleVariant col0 = (GRID_CNT_TYPE)0;
	COleVariant col1 = (GRID_CNT_TYPE)1;
	COleVariant col2 = (GRID_CNT_TYPE)2;
	COleVariant col3 = (GRID_CNT_TYPE)3;

	for (int i=1; i< m_flexGrid.get_Rows(); i++)
	{
		rowi = (GRID_CNT_TYPE)i;

		CString deviceType = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col0, rowi, col0);
		CString gridValue      = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col1, rowi, col1);
		CString normValue  = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col2, rowi, col2);
		CString unit       = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col3, rowi, col3);

		for (int j=0; j<doc->getMaxBlockIndex(); j++)
		{
			BlockStruct*block = doc->getBlockAt(j);

			if (!block)
				continue;

			POSITION dataPos = block->getDataList().GetHeadPosition();
			while (dataPos)
			{
				DataStruct* data = block->getDataList().GetNext(dataPos);

				if (!data || !data->getAttributesRef() || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
					continue;

            Attrib* attrib;

				if (!data->getAttributesRef()->Lookup(keyWord, attrib))
					continue;

				if (!attrib)
					continue;

				CString type = attrib->getStringValue();	

				if (type.CompareNoCase(deviceType))
					continue;

				attrib = NULL;

				if (!data->getAttributesRef()->Lookup(valKeyword, attrib))
					continue;

            if (!attrib)
					continue;

				CString value = attrib->getStringValue();	

				if (value.CompareNoCase(gridValue))
					continue;

            attrib = NULL;
				double valueNumber;
				CString valueUnit;
				CString valueString;
				ComponentValues compValue(value);
   /*OHM == (nothing)
      K = Kilo
      M = Mega

   FARAD = F
      p = pico e-12
      n = nano e-9
	  u = micro e-6
      m = milli e-3*/
				if (compValue.GetValue(valueNumber, valueUnit) &&  compValue.GetPrintableString(valueString))
				{
					CString temp = valueString;
					temp = temp.Trim("0123456789.");
					CString normValueStr = valueString;
					normValueStr.Replace(temp, "");

					double numVal = atof(normValueStr);
					int exponent;
					scientificNormalization(valueNumber,exponent);
					
					if (!type.Left(9).CompareNoCase("Capacitor"))
					{
						CString prefix;

						if (exponent == 0)
						{
							numVal *= pow(10.0, 0);
							prefix = "";
						}
						else if (exponent == -3)
						{
							numVal *= pow(10.0, 3);
							prefix = "m";
						}
						else if (exponent == -6)
						{
							numVal *= pow(10.0, 0);
							prefix = "u";
						}
						else if (exponent == -9)
						{
							numVal *= pow(10.0, -3);
							prefix = "n";
						}
						else if (exponent == -12)
						{
							numVal *= pow(10.0, -6);
							prefix = "p";
						}
					}
					else if (!type.Left(8).CompareNoCase("Inductor"))
					{
						CString prefix;
						if (exponent == 0)
						{
							numVal *= pow(10.0, 3);
							prefix = "";
						}
						else if (exponent == -3)
						{
							numVal *= pow(10.0, 0);
							prefix = "m";
						}
						else if (exponent == -6)
						{
							numVal *= pow(10.0, -3);
							prefix = "u";
						}
						else if (exponent == -9)
						{
							numVal *= pow(10.0, -6);
							prefix = "n";
						}
						else if (exponent == -12)
						{
							numVal *= pow(10.0, -9);
							prefix = "p";
						}
					}
					else if (!type.Left(8).CompareNoCase("Resistor"))
					{
						CString prefix;
						if (exponent == 0)
						{
							numVal *= pow(10.0, 0);
							prefix = "";
						}
						else if (exponent == 3)
						{
							numVal *= pow(10.0, 3);
							prefix = "K";
						}
						else if (exponent == 6)
						{
							numVal *= pow(10.0, 6);
							prefix = "M";
						}
					}

					valueString.Format("%f" , numVal);
					valueString.TrimRight("0");

					if (!valueString.Right(1).CompareNoCase("."))
						valueString += "0";

					setItemToGrid((GRID_CNT_TYPE)i, 2, valueString); 
				}
			}
		}

	}

	COleVariant lastCol = (GRID_CNT_TYPE)(m_flexGrid.get_Cols() - 1);
	COleVariant extraSpace = (GRID_CNT_TYPE)0;
	m_flexGrid.AutoSize(0, lastCol, extraSpace, extraSpace);
	m_flexGrid.put_ColSort(0,CFlexGrid::flexSortGenericAscending);
	m_flexGrid.put_Sort(CFlexGrid::flexSortUseColSort);
	strMap.RemoveAll();

}


/******************************************************************************
* NormalizeValuesDlg::setItemToGrid
*/
void NormalizeValuesDlg::setItemToGrid(GRID_CNT_TYPE row, GRID_CNT_TYPE col, CString itemValue)
{
    COleVariant row1(row), col1(col), newValue(itemValue);

	if (row >= m_flexGrid.get_Rows())
        m_flexGrid.put_Rows(row+1);
	
	m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);
}

/******************************************************************************
* NormalizeValuesDlg::OnBnClickedOk
*/
void NormalizeValuesDlg::OnBnClickedOk()
{
	COleVariant rowi;
	WORD keyWord = doc->RegisterKeyWord(ATT_DEVICETYPE, 0, VT_STRING);
	WORD valKeyword = doc->RegisterKeyWord(ATT_VALUE, 0, VT_STRING);
	WORD normKw = doc->RegisterKeyWord("VALUE_NORMALIZED", 0, VT_STRING);
	WORD valUnitKw = doc->RegisterKeyWord("VALUE_UNIT", 0, VT_STRING);
	//TODO: Update Camcad

	COleVariant col0 = (GRID_CNT_TYPE)0;
	COleVariant col1 = (GRID_CNT_TYPE)1;
	COleVariant col2 = (GRID_CNT_TYPE)2;
	COleVariant col3 = (GRID_CNT_TYPE)3;

	for (int i=1; i< m_flexGrid.get_Rows(); i++)
	{
		rowi = (GRID_CNT_TYPE)i;

		CString deviceType = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col0, rowi, col0);
		CString gridValue  = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col1, rowi, col1);
		CString normValue  = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col2, rowi, col2);
		CString unit       = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col3, rowi, col3);

		for (int j=0; j<doc->getMaxBlockIndex(); j++)
		{
			BlockStruct*block = doc->getBlockAt(j);

			if (!block)
				continue;

			POSITION dataPos = block->getDataList().GetHeadPosition();
			while (dataPos)
			{
				DataStruct* data = block->getDataList().GetNext(dataPos);

				if (!data || !data->getAttributesRef() || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
					continue;

				Attrib* attrib = NULL;

				if (!data->getAttributesRef()->Lookup(keyWord, attrib))
					continue;

				if (!attrib)
					continue;

				CString type = attrib->getStringValue();	

				if (type.CompareNoCase(deviceType))
					continue;

				attrib = NULL;

				if (!data->getAttributesRef()->Lookup(valKeyword, attrib))
					continue;

				if (!attrib)
					continue;

				CString value = attrib->getStringValue();	

				if (value.CompareNoCase(gridValue))
					continue;

				void* valuePtr = NULL;
				valuePtr = (void*)normValue.GetString();
				
				void* unitPtr = NULL;
				unitPtr = (void*)unit.GetString();	

				doc->SetAttrib(&data->getAttributesRef(), normKw, VT_STRING, valuePtr, SA_OVERWRITE, NULL);
				doc->SetAttrib(&data->getAttributesRef(), valUnitKw, VT_STRING, unitPtr, SA_OVERWRITE, NULL);
			}
		}
	}

	OnOK();
}

/******************************************************************************
* NormalizeValuesDlg::MouseUpVsflex
*/
void NormalizeValuesDlg::MouseUpVsflex(short Button, short Shift, float X, float Y)
{
	COleVariant col0, col1, col2, col3;
	COleVariant nRow;
	long rowIndex = m_flexGrid.get_Row();
	nRow = (GRID_CNT_TYPE)rowIndex;
	col0 = (GRID_CNT_TYPE)0;
	col1 = (GRID_CNT_TYPE)1;
	col2 = (GRID_CNT_TYPE)2;
	col3 = (GRID_CNT_TYPE)3;

	if ((m_flexGrid.get_Col() == 2 || m_flexGrid.get_Col() == 3) && rowIndex > 0)
	{
		m_flexGrid.EditCell();		
	}
}

