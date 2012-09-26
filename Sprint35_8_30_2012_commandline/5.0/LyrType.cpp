// $Header: /CAMCAD/5.0/LyrType.cpp 48    6/14/07 1:18p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "Lyrtype.h"
#include "mainfrm.h"
#include "response.h"
#include "xmllayertypecontent.h"
#include "GeneralDialog.h"
#include "PersistantColorDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern char *layergroups[];					// from DBUTIL.CPP

void CCEtoODBDoc::OnLayertypes() 
{
	CLayerTypeDlg dlg(this);
	dlg.DoModal();
}

void LoadLayerTypeInfo(CCEtoODBDoc *doc, const char *filename)
{
   CWaitCursor();
   
	// try reading an xml layertype file first
	LayerTypeXMLReader reader(*doc);
	FileStatusTag retval = reader.open(filename);

	// if not successful, then assume its the old one
	if (retval == statusSucceeded)
	{
		// init
		for (int i=0; i<MAX_LAYERSETS; i++)
		{
			for (int j=0; j<MAX_LAYTYPE; j++)
			{
				doc->LayerTypeArray[i][j].color = RGB(255, 255, 255);
				doc->LayerTypeArray[i][j].show = false;
			}
		}

		//reader.setDoc(doc);

		retval = reader.parse();
		
		return;
	}

	FILE *stream;
   if ((stream = fopen(filename, "rt")) == NULL)
   {
      ErrorMessage(filename, "Unable to open file");
      return;
   }

   char     line[500], *tok;
   int      i, j;
   int      version = 1;

   if (fgets(line,500,stream) == NULL)
      return;

   if (strcmp(line, "! Layertype Information\n"))
   {
      if (!strcmp(line, "! Layertype Information V2\n"))
      {
         version = 2;
      }
      else
      if (!strcmp(line, "! Layertype Information V3\n"))
      {
         version = 3;
      }
      else
      {
         ErrorMessage("This is not a Layertype Information File");
         return;
      }
   }

   // init
   for (i=0; i<MAX_LAYERSETS; i++)
   {
      for (j=0; j<MAX_LAYTYPE; j++)
      {
         doc->LayerTypeArray[i][j].color = RGB(255, 255, 255);
         doc->LayerTypeArray[i][j].show = false;
      }
   }

   if (version == 1)
   {
      int red, green, blue;
      for (i=0; i<MAX_LAYERSETS; i++)  // Normal, Top, Bottom, Cust 1, 2, 3, 4, 5
      {
         j=0;
         while (j<31)   // this was 31 before version 2
         {
            if (fgets(line,500,stream) == NULL) return;
   
            if (line[0] == '!' || line[0] == '\n') continue; // remark

            if ((tok = strtok(line, " \t\n(),")) == NULL) continue;
            doc->LayerTypeArray[i][j].show = atoi(tok);

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            red = atoi(tok);

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            green = atoi(tok);

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            blue = atoi(tok);

            doc->LayerTypeArray[i][j].color = RGB(red, green, blue);

            j++;
         }
      }
   }
   else
   if (version >= 2)
   {
      int red, green, blue;
         
      while (fgets(line,500,stream) != NULL) 
      {
         if (line[0] == '!' || line[0] == '\n') continue; // remark

         if ((tok = strtok(line, " \t\n(),")) == NULL) continue;

         if (!STRCMPI(tok,".NAME"))
         {
            CString  colorsetname;
            if ((tok = strtok(NULL, " \t")) == NULL) continue;
            int id = atoi(tok);
            if ((tok = strtok(NULL, "\n")) == NULL) continue;
            colorsetname = tok;
            colorsetname = tok;
            colorsetname.TrimLeft();
            colorsetname.TrimRight();
            doc->CustomLayersetNames.SetAt(id, tok);
         }
         else
         {
            i = atoi(tok);
            if (i >= MAX_LAYERSETS)
               continue;

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            j = atoi(tok);
            if (j >= MAX_LAYTYPE)
               continue;

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            doc->LayerTypeArray[i][j].show = atoi(tok);

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            red = atoi(tok);

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            green = atoi(tok);

            if ((tok = strtok(NULL, " \t\n(),")) == NULL) continue;
            blue = atoi(tok);
            doc->LayerTypeArray[i][j].color = RGB(red, green, blue);
         }
      }
   }
   fclose(stream);
}

void SaveLayerTypeInfo(CCEtoODBDoc *doc, const char *filename)
{
	int i, j;
	CWaitCursor();

	FILE *stream;
   if ((stream = fopen(filename, "wt")) == NULL)
   {
      ErrorMessage(filename, "Unable to open file");
      return;
   }

   //fprintf(stream, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	fprintf(stream, "<%s major=\"%d\" minor=\"%d\" revision=\"%03d\">\n", LT_LAYERTYPEDOC, 1, 0, 0);
   for (i=0; i<MAX_LAYERSETS; i++)
   {
      switch (i)
      {
      case 0:  fprintf(stream, "   <%s name=\"%s\" index=\"%d\">\n", LT_COLORSET, "Standard", i);    break;
      case 1:  fprintf(stream, "   <%s name=\"%s\" index=\"%d\">\n", LT_COLORSET, "Top",      i);    break;
      case 2:  fprintf(stream, "   <%s name=\"%s\" index=\"%d\">\n", LT_COLORSET, "Bottom",   i);    break;

      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
         fprintf(stream, "   <%s name=\"%s\" index=\"%d\">\n", LT_COLORSET, doc->CustomLayersetNames[i-3], i);	
         break;
      }
      for (j=0; j<MAX_LAYTYPE; j++)
			fprintf(stream, "      <%s type=\"%d\" color=\"%d\" show=\"%s\"/>\n",
					LT_LAYERTYPE, j, doc->LayerTypeArray[i][j].color, (doc->LayerTypeArray[i][j].show)?"Yes":"No");

		fprintf(stream, "   </%s>\n", LT_COLORSET);
	}
	fprintf(stream, "</%s>\n", LT_LAYERTYPEDOC);
   fclose(stream);
}

void SetColorsetButtons(CCEtoODBDoc *doc)
{
   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   CToolBarCtrl *c = &(frame->m_ViewToolBar.GetToolBarCtrl());
   c->PressButton(ID_COLORSETS_NORMAL, doc->ColorSet == 0);
   c->PressButton(ID_COLORSETS_TOP, doc->ColorSet == 1);
   c->PressButton(ID_COLORSETS_BOTTOM, doc->ColorSet == 2);
   c->PressButton(ID_COLORSETS_CUSTOM, doc->ColorSet > 2);
/* c->PressButton(ID_COLORSETS_CUSTOM1, doc->ColorSet == 3);
   c->PressButton(ID_COLORSETS_CUSTOM2, doc->ColorSet == 4);
   c->PressButton(ID_COLORSETS_CUSTOM3, doc->ColorSet == 5);
   c->PressButton(ID_COLORSETS_CUSTOM4, doc->ColorSet == 6);
   c->PressButton(ID_COLORSETS_CUSTOM5, doc->ColorSet == 7);*/
}

/******************************************************************************
* UseColorSet
*/
void CCEtoODBDoc::UseColorSet(int set, BOOL newValueForBottom) 
{
   UnselectAll(FALSE);

   // loop layers
   for (int i=0; i < getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = getLayerArray()[i];
      
      if (layer == NULL)
         continue;

      // set color and show
      layer->setColor( LayerTypeArray[set][layer->getLayerType()].color);
      layer->setVisible( LayerTypeArray[set][layer->getLayerType()].show);
   }

   SetColorsetButtons(this);

	FitPageKeepingZoom(newValueForBottom);
}

void CCEtoODBDoc::OnColorsetsNormal() 
{
   ColorSet = 0;
   UseColorSet(ColorSet, getBottomView()); 
   OnRegen();
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnColorsetsTop() 
{
	OnColorsetsTop(getBottomView());
}

void CCEtoODBDoc::OnColorsetsTop(BOOL newValueForBottom) 
{
   ColorSet = 1;
   UseColorSet(ColorSet, newValueForBottom); 
   OnRegen();
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnColorsetsBottom() 
{
	OnColorsetsBottom(getBottomView());
}

void CCEtoODBDoc::OnColorsetsBottom(BOOL newValueForBottom) 
{
   ColorSet = 2;
   UseColorSet(ColorSet, newValueForBottom); 
   OnRegen();
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnColorsetsCustomN(int setNum) 
{
   ColorSet = setNum + MAX_STANDARD_COLORSETS - 1;
   UseColorSet(ColorSet, getBottomView()); 
   OnRegen();
   UpdateAllViews(NULL);
}


void CCEtoODBDoc::OnColorsetsCustom1() 
{
   OnColorsetsCustomN(1);  
}

void CCEtoODBDoc::OnColorsetsCustom2() 
{
   OnColorsetsCustomN(2);
}

void CCEtoODBDoc::OnColorsetsCustom3() 
{
   OnColorsetsCustomN(3);
}

void CCEtoODBDoc::OnColorsetsCustom4() 
{
   OnColorsetsCustomN(4);
}

void CCEtoODBDoc::OnColorsetsCustom5() 
{
   OnColorsetsCustomN(5);
}

void CCEtoODBDoc::OnColorsetsCustom6() 
{
   OnColorsetsCustomN(6);
}

void CCEtoODBDoc::OnColorsetsCustom7() 
{
   OnColorsetsCustomN(7);
}

void CCEtoODBDoc::OnColorsetsCustom8() 
{
   OnColorsetsCustomN(8);
}

void CCEtoODBDoc::OnColorsetsCustom9() 
{
   OnColorsetsCustomN(9);
}

void CCEtoODBDoc::OnColorsetsCustom10() 
{
   OnColorsetsCustomN(10);
}

void CCEtoODBDoc::OnColorsetsCustom11() 
{
   OnColorsetsCustomN(11);
}

void CCEtoODBDoc::OnColorsetsCustom12() 
{
   OnColorsetsCustomN(12);
}

void CCEtoODBDoc::OnTopview() 
{
   OnColorsetsTop(FALSE); 

   if (showPinnrsBottom)
      showPinnrsTop = TRUE;
   showPinnrsBottom = FALSE;

   if (showPinnetsBottom)
      showPinnetsTop = TRUE;
   showPinnetsBottom = FALSE;

   BOOL temp = showTAOffsetsTop;
   showTAOffsetsTop = showTAOffsetsBottom;
   showTAOffsetsBottom = temp;
   
   SendResponse("View", "Top");
}

void CCEtoODBDoc::OnBottomview() 
{
   OnColorsetsBottom(TRUE); 

   if (showPinnrsTop)
      showPinnrsBottom = TRUE;
   showPinnrsTop = FALSE;

   if (showPinnetsTop)
      showPinnetsBottom = TRUE;
   showPinnetsTop = FALSE;

   BOOL temp = showTAOffsetsBottom;
   showTAOffsetsBottom = showTAOffsetsTop;
   showTAOffsetsTop = temp;

   SendResponse("View", "Bottom");
}

void CCEtoODBDoc::FitPageKeepingZoom(BOOL newValueForBottom)
{
   // make sure we stay at the same zoom level
   POSITION viewPos = GetFirstViewPosition();

   if (viewPos == NULL)
   {
      return;
   }

   CCEtoODBView *view = (CCEtoODBView*)GetNextView(viewPos);

   CClientDC dc(view);
   //view->OnPrepareDC(&dc);

   CRect rect;
   view->GetClientRect(&rect);
   dc.DPtoLP(&rect);

   double scaleFactor = view->getScaleFactor();
	double xmin, xmax, ymin, ymax;

	if ((int)getBottomView() ^ newValueForBottom)	// if changing Bottom
	{
	   xmin = -(double)rect.right / scaleFactor;
		xmax = -(double)rect.left / scaleFactor;
	}
	else
	{
		xmin = (double)rect.left / scaleFactor;
		xmax = (double)rect.right / scaleFactor;
	}

   ymin = (double)rect.bottom / scaleFactor;
   ymax = (double)rect.top / scaleFactor;

   double zoomLevel = view->ScaleDenom / view->ScaleNum;

	setBottomView(newValueForBottom != 0);
   OnFitPageToImage();
   
   if (zoomLevel - 1 >= 0.001)
   {
      view->ZoomBox(xmin, xmax, ymin, ymax);
   }
}


////////////////////////////////////////////////////////////////////////////////////////////
// CLayerTypeDlg dialog for new color set dialog
//IMPLEMENT_DYNAMIC(CLayerTypeDlg, CResizingDialog)
CLayerTypeDlg::CLayerTypeDlg(CCEtoODBDoc *doc, CWnd* pParent /*=NULL*/)
	: CResizingDialog(CLayerTypeDlg::IDD, pParent)
{
	pDoc = doc;
	layerSetNameChanged = false;

	addFieldControl(IDC_LAYERSETS			,anchorLeft, growHorizontal);
	addFieldControl(IDC_RENAME				,anchorRight);
	addFieldControl(IDC_COPYFROM			,anchorRight);

   addFieldControl(IDC_LayerTypeGridStatic	,anchorBottomRight,growBoth);

	addFieldControl(IDOK						,anchorRight);
	addFieldControl(IDCANCEL				,anchorRight);
	addFieldControl(IDC_SAVE				,anchorRight);
	addFieldControl(IDC_LOAD				,anchorRight);

	addFieldControl(IDC_COLLAPSEALL		,anchorBottomRight);
	addFieldControl(IDC_EXPANDALL			,anchorBottomRight);
}

CLayerTypeDlg::~CLayerTypeDlg()
{
}

void CLayerTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizingDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAYERSETS, m_layerSets);
}


BEGIN_MESSAGE_MAP(CLayerTypeDlg, CResizingDialog)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_LAYERSETS, OnCbnSelchangeLayersets)
	ON_BN_CLICKED(IDC_COLLAPSEALL, OnBnClickedCollapseAll)
	ON_BN_CLICKED(IDC_EXPANDALL, OnBnClickedExpandAll)
	ON_BN_CLICKED(IDC_RENAME, OnBnClickedRename)
	ON_BN_CLICKED(IDC_COPYFROM, OnBnClickedCopyfrom)
END_MESSAGE_MAP()


//----------------------------------------------------------------------

// CLayerTypeDlg message handlers
BOOL CLayerTypeDlg::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   m_layerTypeGrid.AttachGrid(this, IDC_LayerTypeGridStatic);
	
   int layerTypeColIndx = m_layerTypeGrid.QuickAddColumn("Layer Type");

   int standardColIndx = m_layerTypeGrid.QuickAddColumn("Standard");

   int topColIndx = m_layerTypeGrid.QuickAddColumn("Top");

   int botColIndx = m_layerTypeGrid.QuickAddColumn("Bottom");

   for (int i=0; i<pDoc->CustomLayersetNames.GetCount(); i++)
   {
      CString layerSetName( pDoc->CustomLayersetNames[i] );
      int addedColIndx = m_layerTypeGrid.QuickAddColumn(layerSetName);

   }

   int hiddenTypeColIndx = m_layerTypeGrid.QuickAddColumn("Hidden Type Index");

	fillColorSetsComboBox();
	fillLayerTypesInGrid();
	fillLayerColorsInGrid();

   m_layerTypeGrid.ResizeAll();
   m_layerTypeGrid.SetExpandedRowHeight( m_layerTypeGrid.GetRowHeight(0) );
   this->showColorSet(0);

	return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CLayerTypeDlg::fillColorSetsComboBox()
{
	m_layerSets.ResetContent();

	m_layerSets.AddString("Standard");
	m_layerSets.AddString("Top");
	m_layerSets.AddString("Bottom");
	for (int layerSet=3; layerSet<MAX_LAYERSETS;layerSet++)
		m_layerSets.AddString(pDoc->CustomLayersetNames[layerSet-3]);

	m_layerSets.SetCurSel(0);
	showColorSet(0);
}

void CLayerTypeDlg::fillColorSetNameColumnTitles()
{
   m_layerTypeGrid.QuickSetText(1, -1, "Standard");
   m_layerTypeGrid.QuickSetText(2, -1, "Top");
   m_layerTypeGrid.QuickSetText(3, -1, "Bottom");

   for (int layerSet=3; layerSet<MAX_LAYERSETS;layerSet++)
   {
      CString colorSetName( pDoc->CustomLayersetNames[layerSet-3] );
      m_layerTypeGrid.QuickSetText(layerSet+1, -1, colorSetName);
   }

}

void CLayerTypeDlg::fillLayerTypesInGrid()
{
	int rows = 0;

	// count the number of rows needed
	for (int layerGroup=0; layerGroup<MAX_LAYGROUP; layerGroup++)
	{
		bool firstTypeInGroup = true;

		for (int layerType=0; layerType<MAX_LAYTYPE; layerType++)
		{
			if (getLayerGroupForLayerType(layerType) != layerGroup)
				continue;

			// add row for group name
			if (firstTypeInGroup)
			{
				rows++;
				firstTypeInGroup = false;
			}

			// add row for layer type
			rows++;
		}
	}

   m_layerTypeGrid.SetNumberRows(rows);

   // We will not be adding columns so we can get the last column index now and keep using it.
   int lastColIndx = m_layerTypeGrid.GetNumberCols() - 1;

	// fill in the types
	int curRow = 0;
	for (int layerGroup=0; layerGroup<MAX_LAYGROUP; layerGroup++)
	{
		bool firstTypeInGroup = true;

		for (int layerType=0; layerType<MAX_LAYTYPE; layerType++)
		{
			if (getLayerGroupForLayerType(layerType) != layerGroup)
				continue;

			// Add layer group name
			if (firstTypeInGroup)
			{
            // Initially grid is filled in expanded mode.
            CString layerGroupName( (CString)"- " + layergroups[layerGroup] );
            m_layerTypeGrid.QuickSetText(0, curRow, layerGroupName);
            m_layerTypeGrid.QuickSetBackColor(0, curRow, m_layerTypeGrid.m_lightGrayBackgroundColor);

            m_layerTypeGrid.QuickSetNumber(lastColIndx, curRow, -1);

				curRow++;
				firstTypeInGroup = false;
			}

         CString layerTypeString( "   " + layerTypeToString(layerType) );
         m_layerTypeGrid.QuickSetText(0, curRow, layerTypeString);

         m_layerTypeGrid.QuickSetNumber(lastColIndx, curRow, layerType);
         m_layerTypeGrid.SetRowCellsToCheckBox(curRow);

			curRow++;
		}
	}
}

void CLayerTypeDlg::fillLayerColorsInGrid()
{
   int lastColIndx = m_layerTypeGrid.GetNumberCols() - 1;
   int rowCnt = m_layerTypeGrid.GetNumberRows();

	for (int layerSet=0; layerSet<MAX_LAYERSETS; layerSet++)
	{
		for (int row=0; row < rowCnt; row++)
		{
			// get the layer type in the last column (hidden)
         CString layerTypeStrVal( m_layerTypeGrid.QuickGetText(lastColIndx, row) );
         int layerTypeIntVal = atoi(layerTypeStrVal);

         if (layerTypeIntVal < 0)
            continue;

			LayerTypeStruct *lt = &pDoc->LayerTypeArray[layerSet][layerTypeIntVal];

         int col = layerSet+1;
         m_layerTypeGrid.QuickSetBackColor(col, row, lt->color);

         CUGCell cell;
         m_layerTypeGrid.GetCell(col, row, &cell);
         cell.SetBool(lt->show);
         m_layerTypeGrid.SetCell(col, row, &cell);
		}
	}
}

void CLayerTypeDlg::showColorSet(int colorSet)
{
   int colCnt = m_layerTypeGrid.GetNumberCols();
   for (long i = 1; i < colCnt; i++)
      m_layerTypeGrid.SetColWidth(i, 0);

   int theCol = colorSet+1;
   m_layerTypeGrid.BestFit(theCol, theCol, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   m_layerTypeGrid.RedrawAll();
}

void CLayerTypeDlg::applyChanges()
{
   int rows = m_layerTypeGrid.GetNumberRows();
	
	// apply all layer set colors
	for (int layerSet = 0; layerSet < MAX_LAYERSETS; layerSet++)
	{
		for (int row = 0; row < rows; row++)
		{
			// get the layer type in the last column (hidden)
         int lastColIndx = m_layerTypeGrid.GetNumberCols() - 1;
         CString layerTypeStr( m_layerTypeGrid.QuickGetText(lastColIndx, row) );
         int layerType = atoi(layerTypeStr);

			if (layerType < 0)
				continue;

         int layerSetColIndx = layerSet + 1;

			LayerTypeStruct *lt = &pDoc->LayerTypeArray[layerSet][layerType];
			
         CUGCell cell;
         m_layerTypeGrid.GetCell(layerSetColIndx, row, &cell);
         COLORREF color = cell.GetBackColor();
         BOOL show = cell.GetBool();

			lt->color = color;
			lt->show = show;
		}
	}
}

void CLayerTypeDlg::OnCbnSelchangeLayersets()
{
	showColorSet(m_layerSets.GetCurSel());
}

void CLayerTypeDlg::OnBnClickedRename()
{
	int curSel = m_layerSets.GetCurSel();

   // Not an actual selection
	if (curSel < 0)
		return;

   // Can't change first three: Standard, Top, and Bottom.
	if (curSel < 3)
	{
		ErrorMessage("This colorset cannot be renamed!", "Color Set", MB_OK);
		return;
	}

   // Proceed with change.
	CString oldName;
	m_layerSets.GetLBText(curSel, oldName);

	InputDlg dlg;
	dlg.m_prompt = "Enter new color set name.";
	dlg.m_input = oldName;
   if (dlg.DoModal() == IDOK)
   {
      pDoc->CustomLayersetNames[curSel - MAX_STANDARD_COLORSETS] = dlg.m_input;

      m_layerSets.InsertString(curSel, dlg.m_input);
      m_layerSets.DeleteString(curSel+1);

      m_layerSets.SetCurSel(curSel);

      // Update grid column title with new name, if we can find the old name
      int colIndx = -1;
      if (m_layerTypeGrid.GetColFromName(oldName, &colIndx) == UG_SUCCESS)
      {
         m_layerTypeGrid.QuickSetText(colIndx, -1, dlg.m_input);
         m_layerTypeGrid.RedrawAll();
      }
   }
}

void CLayerTypeDlg::OnBnClickedCopyfrom()
{
	int curSel = m_layerSets.GetCurSel();

	if (curSel < 0)
		return;

	CSelectDlg dlg("Choose the colorset to copy", false, false);
	dlg.AddItem(new CSelItem("Standard", false));
	dlg.AddItem(new CSelItem("Top", false));
	dlg.AddItem(new CSelItem("Bottom", false));
	for (int i = 0; i < MAX_CUSTOM_COLORSETS; i++)
		dlg.AddItem(new CSelItem(pDoc->CustomLayersetNames[i], false));

	if (dlg.DoModal() == IDCANCEL)
		return;

	CSelItem* selItem = dlg.GetSelItemHead();
	int index = m_layerSets.FindString(0, selItem->GetName());
	int colFrom = index+1;
	int colTo = curSel+1;

   // Update the grid with copied values.
   int rowCnt = m_layerTypeGrid.GetNumberRows();
   int lastColIndx = m_layerTypeGrid.GetNumberCols() - 1;

   for (int rowIndx = 0; rowIndx < rowCnt; rowIndx++)
   {
      // Skip it if it is a layer group row
      CString cellText(m_layerTypeGrid.QuickGetText(lastColIndx, rowIndx));
      int layerType = atoi(cellText);
      if (layerType < 0)
         continue;

      // Copy entry
      CUGCell cellFrom;
      m_layerTypeGrid.GetCell(colFrom, rowIndx, &cellFrom);
      m_layerTypeGrid.SetCell(colTo, rowIndx, &cellFrom);
   }
   m_layerTypeGrid.RedrawAll();
}

void CLayerTypeDlg::OnBnClickedSave()
{
   CFileDialog FileDialog(FALSE, "LT", "DEFAULT.LT",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layertype Info File (*.LT)|*.LT|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

	applyChanges();

	SaveLayerTypeInfo(pDoc, FileDialog.GetPathName());	
}

void CLayerTypeDlg::OnBnClickedLoad()
{
   CFileDialog FileDialog(TRUE, "LT", "DEFAULT.LT",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Layertype Info File (*.LT)|*.LT|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

	LoadLayerTypeInfo(pDoc, FileDialog.GetPathName());

   fillColorSetNameColumnTitles();

	fillColorSetsComboBox();
	fillLayerTypesInGrid();
	fillLayerColorsInGrid();
}

void CLayerTypeDlg::OnBnClickedCollapseAll()
{
   int rowCnt = m_layerTypeGrid.GetNumberRows();

   int lastColIndx = m_layerTypeGrid.GetNumberCols() - 1;

   for (int rowIndx = 0; rowIndx < rowCnt; rowIndx++)
   {
      CString cellText(m_layerTypeGrid.QuickGetText(lastColIndx, rowIndx));
      int layerType = atoi(cellText);
      if (layerType < 0) // Is layer group name row
      {
         m_layerTypeGrid.CollapseLayerGroup(rowIndx);
      }
   }

   m_layerTypeGrid.RedrawAll();
}

void CLayerTypeDlg::OnBnClickedExpandAll()
{
   int rowCnt = m_layerTypeGrid.GetNumberRows();

   int lastColIndx = m_layerTypeGrid.GetNumberCols() - 1;

   for (int rowIndx = 0; rowIndx < rowCnt; rowIndx++)
   {
      CString cellText(m_layerTypeGrid.QuickGetText(lastColIndx, rowIndx));
      int layerType = atoi(cellText);
      if (layerType < 0) // Is layer group name row
      {
         m_layerTypeGrid.ExpandLayerGroup(rowIndx);
      }
   }

   m_layerTypeGrid.RedrawAll();
}

void CLayerTypeDlg::OnBnClickedOk()
{
	applyChanges();

	OnOK();
}

BEGIN_EVENTSINK_MAP(CLayerTypeDlg, CResizingDialog)
END_EVENTSINK_MAP()


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CLayerTypeGrid::CLayerTypeGrid()
{
   SetSortEnabled(false);  // Turn off grid sort on heading left click
}

//----------------------------------------------------------------------------------

void CLayerTypeGrid::SetRowCellsToCheckBox(int row)
{
   // Not the first column and not the last column, but all columns in between.
   int rowCnt = this->GetNumberRows();
   int colCnt = this->GetNumberCols();

   for (int colIndx = 1; colIndx < (colCnt-1); colIndx++)
   {
      CUGCell cell;

      GetCell(colIndx, row, &cell); 
      cell.SetCellType(UGCT_CHECKBOX);  // To make it a checkbox cell
      cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
      SetCell(colIndx, row, &cell);
   }
}

//----------------------------------------------------------------------------------

void CLayerTypeGrid::OnSetup()
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

   SetNumberCols(0);
   SetNumberRows(0);

   BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   SetColWidth(-1, 0); // get rid of "row heading"
}

void CLayerTypeGrid::OnDClicked(int col,long row, RECT *rect,POINT *point,BOOL processed)
{
   int lastColIndx = GetNumberCols() - 1;
   CString cellText(QuickGetText(lastColIndx, row));
   int layerType = atoi(cellText);

   // Edit color if a layerType row (not a group name row)
   // and double click was not in the layer type column.
   if (layerType > -1 && col > 0)
   {
      CUGCell cell;
      GetCell(col, row, &cell); 

      COLORREF color = cell.GetBackColor(); // current color

	   CPersistantColorDialog dialog(color);
	   if (dialog.DoModal() == IDOK)
      {
	      color = dialog.GetColor();
         cell.SetBackColor(color);
         SetCell(col, row, &cell);
         RedrawAll();
      }
   }
}

void CLayerTypeGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   // A click will get two event calls here, one for button down (updn = true) and one
   // for button up. We don't want to do this twice for a single click.
   // So react only to the button up, that is the end of the click event.
   if (updn)
      return;  // Ignore the button down.

   int lastColIndx = GetNumberCols() - 1;
   CString cellText(QuickGetText(lastColIndx, row));
   int layerType = atoi(cellText);

   // Toggle expand/collapse if layer type indicates this is a layer group row.
   // React only if cell was first column cell.
   if (layerType < 0 && col == 0)
   {
      int rowCnt = GetNumberRows();

      CUGCell cell;
      GetCell(col, row, &cell);

      // We use the "+" and "-" in name to track expanded/collapsed state.
      // The char shows the current state.
      CString groupNameText( cell.GetText() );
      bool isExpanded = (groupNameText.Left(1).Compare("-") == 0);

      if (isExpanded)
      {
         // Is expanded, perform collapse.
         CollapseLayerGroup(row);
         RedrawAll();
      }
      else
      {
         // Is collapsed, perform expand.
         ExpandLayerGroup(row);
         RedrawAll();
      }
   }
}

void CLayerTypeGrid::ExpandLayerGroup(int groupNameRowIndx)
{
   int rowCnt = GetNumberRows();
   int lastColIndx = GetNumberCols() - 1;

   bool keepGoing = true;
   for (int rowIndx = groupNameRowIndx+1; rowIndx < rowCnt && keepGoing; rowIndx++)
   {
      CString cellText( QuickGetText(lastColIndx, rowIndx) );
      int layerType = atoi(cellText);
      if (layerType < 0)
      {
         keepGoing = false;
      }
      else
      {
         SetRowHeight(rowIndx, m_expandedRowHeight);
      }
   }

   CString groupNameText( QuickGetText(0, groupNameRowIndx) );
   groupNameText.SetAt(0, '-');  // Is now expanded, show operator to collapse.
   QuickSetText(0, groupNameRowIndx, groupNameText);
}

 void CLayerTypeGrid::CollapseLayerGroup(int groupNameRowIndx)
 {
   int rowCnt = GetNumberRows();
   int lastColIndx = GetNumberCols() - 1;

   bool keepGoing = true;
   for (int rowIndx = groupNameRowIndx+1; rowIndx < rowCnt && keepGoing; rowIndx++)
   {
      CString cellText( QuickGetText(lastColIndx, rowIndx) );
      int layerType = atoi(cellText);
      if (layerType < 0)
      {
         keepGoing = false;
      }
      else
      {
         SetRowHeight(rowIndx, 0);
      }
   }

   CString groupNameText( QuickGetText(0, groupNameRowIndx) );
   groupNameText.SetAt(0, '+');  // Is now collapsed, show operator to expand.
   QuickSetText(0, groupNameRowIndx, groupNameText);
 }

