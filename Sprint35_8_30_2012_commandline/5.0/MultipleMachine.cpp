// $Header: /CAMCAD/5.0/MultipleMachine.cpp 21    6/30/07 2:31a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "DFT.h"
#include "CCEtoODB.h"
#include "mainfrm.h"
#include "MultipleMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern License *licenses;


bool isTestMachine(FileTypeTag fileType);
int getMachineType(FileTypeTag fileType);
CString getMachineName(FileTypeTag fileType);
FileTypeTag getFileType(CString machineName);
CString getMachineTypeString(FileTypeTag fileType);
CString getMachineTypeString(int machineType);

/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CMachine
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
CMachine::CMachine(FileTypeTag fileType, CString machineName, CString PCBside)
{
   m_eFileType = fileType;
   m_pDFTSolution = NULL;
   m_ptOrigin.x = 0.0;
   m_ptOrigin.y = 0.0;
   m_machineName = (machineName.IsEmpty())?getMachineName(m_eFileType):machineName;
   m_PCBSide = (PCBside.IsEmpty())?GetMachineDefaultSurface():PCBside;
}

CMachine::~CMachine()
{
   // Do not delete the DFT solution, just set the pointer to NULL
   if (m_pDFTSolution != NULL)
      m_pDFTSolution = NULL;
}

CString CMachine::GetName()
{
   return m_machineName;
}

CString CMachine::GetMachineTypeString()
{
   return getMachineTypeString(m_eFileType);
}

int CMachine::GetMachineType()
{
   return getMachineType(m_eFileType);
}

MachineCategoryTag CMachine::GetMachineCategory()
{
   MachineCategoryTag category = MachineCategoryUnknown;
   int machineType = GetMachineType();
   
   int DFTMachineType = MACHINE_TYPE_ICT | MACHINE_TYPE_FPT | MACHINE_TYPE_MDA;
   int PrepSetMachineType = MACHINE_TYPE_AXI | MACHINE_TYPE_AOI | MACHINE_TYPE_PNP | MACHINE_TYPE_CUT;

   if (machineType & DFTMachineType)
      category = MachineCategoryDFT;
   else if(machineType & PrepSetMachineType)
      category = MachineCategoryPrepSet;

   return category;
}

CString CMachine::GetMachineDefaultSurface()
{      
   m_PCBSide = QSURFACETOPBOT;

   switch(m_eFileType)
   {
   case fileTypeHp5dx:
   case fileTypeFujiFlexa:
      m_PCBSide = QSURFACEBOTH;
      break;
   }

   return m_PCBSide;
}

//Filter machine by blockType
bool CMachine::IsFileTypeforMachine(BlockTypeTag blockType)
{
   bool retVal = false;
   if(blockType == blockTypePcb)
   {
     switch(m_eFileType)
     {
     case fileTypeSiemensQd:
        retVal = false;
        break;
     default:
        retVal = true;
        break;      
     }
   }
   else if(blockType == blockTypePanel)
   {
      retVal = true;
   }

   return retVal;
}

bool CMachine::HasConfiguration()
{
   bool isSupported = false;
   switch(m_eFileType)
   {
   case fileTypeSiemensQd:
   case fileTypeFujiFlexa:
   case fileTypeSakiAOI:
   case fileTypeOmronAOI:
   case fileTypeCyberOpticsAOI:
   case fileTypeYestechAOI:
   case fileTypeMYDATAPCB:
   case fileTypeCustomAssembly:
      isSupported = true;
      break;
   }

   return isSupported;
}

Attrib *CMachine::GetMachineAttribute(CCEtoODBDoc *doc, FileStruct* curFile, bool isBottomSide)
{
   return is_attvalue(doc, curFile->getBlock()->getDefinedAttributes(), GetMachineAttributeName(isBottomSide), 0); 
}

/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CMachineList
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
CMachineList::CMachineList(CCEtoODBDoc& camCadDoc,const FileStruct* pPCBFile)
: m_camCadDoc(camCadDoc)
, m_pPCBFile(pPCBFile)
{
   m_InitializedDFT = false;
   CamcadLicense& camcadLicense = getApp().getCamcadLicense();

   /*for (POSITION pos=camcadLicense.getProductStartPosition(); pos != NULL;)
   {
      CamcadProduct* camcadProduct = camcadLicense.getNextProduct(pos);
      if (camcadProduct != NULL)
      {
         // licenses to omit when in testing phase for any builds not mentioned
         #if !defined _RDEBUG && !defined _RELTEST && !defined _DEBUG
            if (camcadProduct->getAccessTypeFlag() & ACCESS_FORTEST)
               continue;
         #endif

         if (camcadProduct->getAccessTypeFlag() & ACCESS_OUTPUT && camcadProduct->getProductTypeFlag() & Product &&
             (camcadProduct->getAllowed() || camcadProduct->getAccessTypeFlag() & ACCESS_FORTEST))
         {
            if (!isTestMachine(camcadProduct->getCamcadFileType()))
               continue;

            CMachine* machine = new CMachine(camcadProduct->getCamcadFileType());            
            this->AddTail(machine);
         }
      }
   }*/

   InitDFTSolution();
}

CMachineList::~CMachineList()
{
   this->empty();
}

CMachine* CMachineList::FindMachine(CString name)
{
   POSITION pos = this->GetHeadPosition();
   while (pos)
   {
      CMachine* machine = this->GetNext(pos);
      if (machine == NULL)
         continue;

      if (machine->GetName().CompareNoCase(name) == 0)
         return machine;
   }

   return NULL;
}

void CMachineList::DeleteMachines(CString name)
{
   for (POSITION pos = GetHeadPosition();pos;)
   {
      POSITION curPos = pos;
      CMachine* machine = GetNext(pos);
      if (machine == NULL)
         continue;

      if (machine->GetName().CompareNoCase(name) == 0)
      {
         this->RemoveAt(curPos);
         delete machine;
      }         
   }
}

bool CMachineList::InitDFTSolution()
{
   // Check to see if there is a generic DFT solution, if there is then assign it to the machine
   CDFTSolution* genericSolution = NULL;

   /*if (!m_InitializedDFT && m_pPCBFile && getCamCadDoc().getDFTSolutions(*m_pPCBFile).GetCount() > 0)
   {
      genericSolution = getCamCadDoc().getDFTSolutions(*m_pPCBFile).GetHead();
   
      for(POSITION machinePOS = GetHeadPosition();machinePOS;)
      {
         CMachine* machine = GetNext(machinePOS);
         if (machine && machine->IsDFTSolutionMachine())
         {
            machine->SetDFTSolution(genericSolution);
         }
      }

      m_InitializedDFT = true;
   }*/

   return m_InitializedDFT;
}

void CMachineList::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   if (GetCount() <= 0)
      return;

   writeFormat.writef("<MultipleMachines>\n");
   writeFormat.pushHeader("  ");

   POSITION pos = GetHeadPosition();
   while (pos)
   {
      CMachine *machine = GetNext(pos);
      if (machine == NULL)
         continue;

      writeFormat.writef("<Machine Name=\"%s\" DFTName=\"%s\" X=\"%0.3f\" Y=\"%0.3f\"/>\n", 
                         SwapSpecialCharacters(getMachineName(machine->GetFileType())),
                         SwapSpecialCharacters((machine->GetDFTSolution()!=NULL)?machine->GetDFTSolution()->GetName():""),
                         machine->GetOrigin().x, machine->GetOrigin().y);
   }

   writeFormat.popHeader();
   writeFormat.writef("</MultipleMachines>\n");
}

int CMachineList::LoadXML(CString xmlString, CCEtoODBDoc *doc, FileStruct *file)
{
   CXMLDocument xmlDoc;

// ErrorMessage("Start to Load Mutliple Machine.");

   if (xmlDoc.LoadXML(xmlString))
   {
      CXMLNodeList *nodeList = xmlDoc.GetElementsByTagName("Machine");
      if (nodeList == NULL)
         return 0;      // no machine

      nodeList->Reset();
      CXMLNode *node = NULL;
      while (node = nodeList->NextNode())
      {
         CString machineName;
         if (!node->GetAttrValue("Name", machineName))
            continue;

//       ErrorMessage("Load Mutliple Machine = " + machineName);

         CString dftSolutionName;
         if (!node->GetAttrValue("DFTName", dftSolutionName))
            dftSolutionName = DFT_GENERIC_SOLUTION_NAME;

         CString buf;
         CPoint2d origin;
         if (!node->GetAttrValue("X", buf))
            origin.x = 0.0;
         else
            origin.x = atof(buf);

         if (!node->GetAttrValue("Y", buf))
            origin.y = 0.0;
         else
            origin.y = atof(buf);


         /*CMachine *machine = doc->FindMachine(*file,machineName);
         if (machine == NULL)
            continue;

         machine->SetOrigin(origin);

         if (!dftSolutionName.IsEmpty())
         {
            CDFTSolution* dftSolution = doc->FindDFTSolution(*file, dftSolutionName, false);
            machine->SetDFTSolution(dftSolution);
         }

         delete node;
         node = NULL;*/
      }

      delete nodeList;
      nodeList = NULL;
   }

// ErrorMessage("Finished Loading Mutliple Machine.");

   return 0;
}


/******************************************************************************
* CCEtoODBDoc::OnMultipleMachineManagement
*/
void CCEtoODBDoc::OnMultipleMachineManagement()
{
   // TODO: Add your command handler code here

   FileStruct *file = getFileList().GetFirstShown(blockTypePcb);

   if (file != NULL)
   {
      /*if (getDFTSolutions(*file).GetCount() > 0)
      {
         CMultipleMachineDlg dlg(this, file);
         dlg.DoModal();

         CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
         if(frame) 
         {
            frame->getNavigator().FillMachineList(this,file);
         }
      }
      else
      {
         ErrorMessage("No DFT Solution detected.\n\nMultiple Machine Management requires at least one DFT Solution.  Run DFT and try again.",
                      "Multiple Machine Management");
      }*/
   }
   else
   {
      ErrorMessage("No visible PCB file detected.\n\nMultiple Machine Management requires a visible PCB file.",
                   "Multiple Machine Management");
   }
}


/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CMultipleMachineDlg dialog
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CMultipleMachineDlg, CDialog)
CMultipleMachineDlg::CMultipleMachineDlg(CCEtoODBDoc *pDoc, FileStruct* pPCBFile, CWnd* pParent /*=NULL*/)
   : CDialog(CMultipleMachineDlg::IDD, pParent)
{
   m_pDoc = pDoc;
   m_pPCBFile = pPCBFile;
}

CMultipleMachineDlg::~CMultipleMachineDlg()
{
}

void CMultipleMachineDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_STATIC_MACHINE, m_staMachineGrid);
}

BEGIN_MESSAGE_MAP(CMultipleMachineDlg, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CMultipleMachineDlg, CDialog)
   ON_EVENT(CMultipleMachineDlg, IDC_MACHINE_GRID, 0x22, CellButtonClickOnGrid, VTS_I4 VTS_I4)
   ON_EVENT(CMultipleMachineDlg, IDC_MACHINE_GRID, DISPID_CLICK, ClickOnGrid, VTS_NONE)
   ON_EVENT(CMultipleMachineDlg, IDC_MACHINE_GRID, 0x15, BeforeEditGrid, VTS_I4 VTS_I4 VTS_PBOOL)
END_EVENTSINK_MAP()

BOOL CMultipleMachineDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Must call this first to create machine
   //m_pDoc->CreatMachineList(*m_pPCBFile);

   // Create the grids
   m_staMachineGrid.substituteForDlgItem(IDC_MACHINE_GRID, IDC_STATIC_MACHINE, "FlexGrid1", *this);
   CExtendedFlexGrid* machineGrid = m_staMachineGrid.getGrid();

   initGrid(machineGrid);
   loadGrid(machineGrid);

   return TRUE;
}

void CMultipleMachineDlg::initGrid(CExtendedFlexGrid* grid)
{
   COleVariant rowZero((GRID_CNT_TYPE)0);
   COleVariant rowOne((GRID_CNT_TYPE)1);
   COleVariant colZero((GRID_CNT_TYPE)0);
   COleVariant colFour((GRID_CNT_TYPE)4);

   grid->clear();

   // Initialize top Height Analysis data grid
   grid->put_AllowSelection(FALSE);
   grid->put_AllowUserResizing(FALSE);
   grid->put_Appearance(CFlexGrid::flex3D);
   grid->put_BackColor(GRID_COLOR_WHITE);       
   grid->put_BackColorAlternate(GRID_COLOR_IVORY);
   grid->put_BackColorBkg(GRID_COLOR_GRAY);     
   grid->put_Cols(5);
// grid->put_Editable(CFlexGrid::flexEDNone);
   grid->put_ExtendLastCol(TRUE);
   grid->put_FixedCols(0);
   grid->put_FixedRows(1);
   grid->put_FocusRect(CFlexGrid::flexFocusInset);
   grid->put_HighLight(CFlexGrid::flexHighlightAlways);
   grid->put_SelectionMode(CFlexGrid::flexSelectionListBox); // :flexSelectionByRow);
   grid->put_Rows(1);
   grid->put_WordWrap(TRUE);
   grid->put_ScrollBars(CFlexGrid::flexScrollBarBoth);

   // Set headers for grid
   //    Col 0 = Varify
   //    Col 1 = Machine Type
   //    Col 2 = Machine Name
   //    col 3 = DFT solution to copy from -- this column is hidden
   //    Col 4 = DFT Solution
   CString tmp = "Verify\tMachine Type\tMachine Name\t\tDFT Solution";
   grid->put_Cell(CFlexGrid::flexcpText, rowZero, colZero, rowZero, colFour, COleVariant(tmp));
   grid->put_Cell(CFlexGrid::flexcpAlignment, rowZero, colZero, rowZero, colFour, COleVariant((long)CFlexGrid::flexAlignCenterCenter));
   //grid->put_RowHeight(0, 500);   

   // Set the width of columns
   grid->put_ColWidth(COL_VERIFY, 600);
   grid->put_ColWidth(COL_MACHINE_TYPE, 1200);
   grid->put_ColWidth(COL_MACHINE_NAME, 2500);
   grid->put_ColWidth(COL_COPY_FROM_DFT, 800);
   grid->put_ColWidth(COL_MACHINE_DFT, 800);

   // Hide the COPY FROM DFT column because it is only use as a place holder
   grid->put_ColHidden(COL_COPY_FROM_DFT, TRUE);

   // Set the alignement of columns
   grid->put_ColAlignment(COL_VERIFY, CFlexGrid::flexAlignLeftCenter);
   grid->put_ColAlignment(COL_MACHINE_TYPE, CFlexGrid::flexAlignLeftCenter);
   grid->put_ColAlignment(COL_MACHINE_NAME, CFlexGrid::flexAlignLeftCenter);
   grid->put_ColAlignment(COL_MACHINE_DFT, CFlexGrid::flexAlignLeftCenter);

   //grid->put_Editable(CFlexGrid::flexEDKbdMouse);
   grid->put_ColComboList(COL_MACHINE_DFT, "...");
}

void CMultipleMachineDlg::loadGrid(CExtendedFlexGrid* grid)
{
   /*POSITION pos = m_pDoc->GetMachineHeadPosition(*m_pPCBFile);
   while (pos != NULL)
   {
      CMachine* machine = m_pDoc->GetMachineNext(*m_pPCBFile,pos);
      if (machine == NULL)
         continue;

      if(machine->IsDFTSolutionMachine())
      {
         CString machineInfo;
         machineInfo.Format("OK\t%s\t%s", machine->GetMachineTypeString(), machine->GetName());

         // If after checking for generic and it is NULL then DFT solution is NONE
         if (machine->GetDFTSolution() == NULL)
            machineInfo.AppendFormat("\t\tNot Applicable");
         else
            machineInfo.AppendFormat("\t\t%s", machine->GetDFTSolution()->GetName());

         grid->AddItem(machineInfo, COleVariant(grid->get_Rows()));    
      }
   }

   // Set the column or order for sorting
   grid->put_Col(COL_MACHINE_NAME);
   grid->put_Sort(CFlexGrid::flexSortStringAscending);*/
}

void CMultipleMachineDlg::CellButtonClickOnGrid(long Row, long Col)
{
   if (Col == COL_MACHINE_DFT)
   {
      CExtendedFlexGrid* machineGrid = m_staMachineGrid.getGrid();
      CString machineName = machineGrid->getCellText((int)Row, (int)COL_MACHINE_NAME);
      CString dftSolutionName = machineGrid->getCellText((int)Row, (int)COL_MACHINE_DFT);
      if (dftSolutionName.CompareNoCase("Not Applicable") == 0)
         return;

      CDFTSelection dlg(getCamCadDoc(),m_pPCBFile, machineName);
      if (dlg.DoModal() == IDOK)
      {
         CString newDFTName = dlg.GetNewDFTName();
         CString copyFromDFTName = dlg.GetCopyFromDFTName();

         CExtendedFlexGrid* machineGrid = m_staMachineGrid.getGrid();
         machineGrid->setCell((int)Row, (int)COL_COPY_FROM_DFT, copyFromDFTName);
         machineGrid->setCell((int)Row, (int)Col, newDFTName);
      }
   }
}

void CMultipleMachineDlg::ClickOnGrid()
{
   CExtendedFlexGrid* machineGrid = m_staMachineGrid.getGrid();
   machineGrid->put_Editable(CFlexGrid::flexEDNone);     

   CString dftSolutionName = machineGrid->getCellText((int)machineGrid->get_Row(), (int)COL_MACHINE_DFT);
   if (dftSolutionName.CompareNoCase("Not Applicable") != 0)
   {
      machineGrid->select(machineGrid->get_Row(), (int)COL_MACHINE_DFT, machineGrid->get_Row(), 0);
      machineGrid->put_Editable(CFlexGrid::flexEDKbdMouse);    
   }
}

void CMultipleMachineDlg::BeforeEditGrid(long Row, long Col, VARIANT_BOOL* Cancel)
{
}

void CMultipleMachineDlg::OnBnClickedOk()
{
   OnOK();

   CExtendedFlexGrid* machineGrid = m_staMachineGrid.getGrid();

   for (int i=1; i<machineGrid->get_Rows(); i++)
   {
      CString machineDFTName = machineGrid->getCellText(i, (int)COL_MACHINE_DFT);
      if (machineDFTName.CompareNoCase(DFT_GENERIC_SOLUTION_NAME) == 0)
         continue;

      CString copyFromDFTName = machineGrid->getCellText(i, (int)COL_COPY_FROM_DFT);
      /*CDFTSolution* copyFromDFT = getCamCadDoc().FindDFTSolution(*m_pPCBFile, copyFromDFTName, false/*isFlipped);
      if (copyFromDFT == NULL)
         continue;*/

      /*CDFTSolution* newDFT = getCamCadDoc().AddNewDFTSolution(*m_pPCBFile,machineDFTName, false/*isFlipped, m_pDoc->getPageUnits());
      newDFT->CopyDFTSolution(*copyFromDFT);*/

      CString machineName = machineGrid->getCellText(i, (int)COL_MACHINE_NAME);
      /*CMachine* machine = getCamCadDoc().FindMachine(*m_pPCBFile,machineName);
      if (machine != NULL)
         machine->SetDFTSolution(newDFT);*/
   }
}


/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CDFTSelection dialog
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CDFTSelection, CDialog)
CDFTSelection::CDFTSelection(CCEtoODBDoc& camCadDoc,FileStruct* pFile, CString machineName, CWnd* pParent /*=NULL*/)
: CDialog(CDFTSelection::IDD, pParent)
, m_camCadDoc(camCadDoc)
, m_sNewDFTName(_T(""))
{
   m_pPCBFile = pFile;
   m_sNewDFTName.Format("%s DFT", machineName);
   m_sCopyFromDFTName = "";
}

CDFTSelection::~CDFTSelection()
{
}

void CDFTSelection::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDIT_NEW_DFT_NAME, m_sNewDFTName);
   DDX_Control(pDX, IDC_COMBO_DFT_SOLUTIONS, m_cmbDFTSolutions);
}

BEGIN_MESSAGE_MAP(CDFTSelection, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDFTSelection::OnInitDialog()
{
   CDialog::OnInitDialog();

   loadDFTSolutionList();

   return TRUE;
}

void CDFTSelection::loadDFTSolutionList()
{
   m_cmbDFTSolutions.ResetContent();

   /*POSITION pos = getCamCadDoc().getDFTSolutions(*m_pPCBFile).GetHeadPosition();

   while (pos != NULL)
   {
      CDFTSolution* dftSolution = getCamCadDoc().getDFTSolutions(*m_pPCBFile).GetNext(pos);
      if (dftSolution == NULL)
         continue;

      m_cmbDFTSolutions.AddString(dftSolution->GetName());
   }

   m_cmbDFTSolutions.SetCurSel(0);*/
}

void CDFTSelection::OnBnClickedOk()
{
   OnOK();

   m_cmbDFTSolutions.GetWindowText(m_sCopyFromDFTName);

   UpdateData(TRUE);
}
