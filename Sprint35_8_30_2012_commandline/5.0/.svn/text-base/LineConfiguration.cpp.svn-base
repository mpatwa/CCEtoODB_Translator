
#include "stdafx.h"
#include "CCEtoODB.h"
#include "mainfrm.h"
#include "AssemblyTemplate.h"
#include "lineconfiguration.h"
#include "PrepDlg.h"
#include ".\lineconfiguration.h"

extern FileTypeTag getFileType(CString machineName);

////////////////////////////////////////////////////////////////////////////////
// CCustomMachineGrid
////////////////////////////////////////////////////////////////////////////////
CCustomMachineGrid::CCustomMachineGrid(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CDialog *dlg)
: m_camCadDoc(camCadDoc)
, m_pcbFile(pPCBFile)
, m_cmdlg(dlg)
{
   m_redBackgroundColor       = RGB(0xff,0xf0,0xf5);
   m_greenBackgroundColor     = RGB(0xf0,0xff,0xf0);
   m_lightGrayBackgroundColor = RGB(226,226,226);
   m_whiteBackgroundColor     = RGB(0xff,0xff,0xff);
   

   m_gridHeader.RemoveAll();
   m_gridHeader.Add(QMACHINENAME);
   m_gridHeader.Add(QMACHINETEMPLATE);

   m_ColumnSize = m_gridHeader.GetCount();
   m_SelectRow = -1;
   m_SelectCol = -1;
}

void CCustomMachineGrid::initGrid()
{
   if (GetNumberCols() < m_gridHeader.GetCount())
   {     
      int defaultColSize = m_gridHeader.GetCount();
      SetSH_Width(0);
      SetNumberRows(0,false);
      SetNumberCols(defaultColSize,false);

      for(int idx = 0; idx < defaultColSize; idx++)
      {
         if( m_gridHeader.GetAt(idx))
            QuickSetText(idx, -1, m_gridHeader.GetAt(idx));
      }
   }

   SetNumberRows(0);
   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CCustomMachineGrid::SetCellOption(int col, int option)
{
   //First Column is a droplist
   CUGCell cell;
   GetColDefault(col, &cell); 
   cell.SetCellType(option); 
   SetColDefault(col, &cell);
}

void CCustomMachineGrid::QuickSetDropList(int col, int row, CString options)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   CString optionlist(options); 
   optionlist.Replace("|", "\n");
   cell.SetLabelText( optionlist );

   CSupString supOption(options);
   CStringArray optionArray;   
   supOption.ParseQuote(optionArray,"|");
   cell.SetText(optionArray.GetCount()?optionArray.GetAt(0):"");
   SetCell(col, row, &cell);
}


void CCustomMachineGrid::AddRowData(CCustomMachineData machinedata)
{
   int RowSize = GetNumberRows();
   SetNumberRows(RowSize + 1);

   QuickSetText(T_CTMachineName, RowSize, machinedata.getMachineName());  
   QuickSetText(T_CTMachineTemplate, RowSize, machinedata.getTemplateFile());
   
   SetCellReadOnly(T_CTMachineTemplate, RowSize, true);
   SetRowBackColor(RowSize, m_greenBackgroundColor);
   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CCustomMachineGrid::GetRowData(CCustomMachineData& machinedata)
{
   GetRowData(m_SelectRow, machinedata);
}

void CCustomMachineGrid::GetRowData(int row, CCustomMachineData& machinedata)
{
   QuickGetText(T_CTMachineName, row, &machinedata.getMachineName());
   QuickGetText(T_CTMachineTemplate, row, &machinedata.getTemplateFile());
}

void CCustomMachineGrid::SetCellReadOnly(int row, bool readonly)
{
   for(int col = 0; col < this->GetNumberCols(); col ++)
      SetCellReadOnly(col,row,readonly);
}

void CCustomMachineGrid::SetCellReadOnly(int col, int row, bool readonly)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   cell.SetReadOnly(readonly);   
   SetCell(col, row, &cell);  
}

void CCustomMachineGrid::DeleteSelectedRow()
{
   if(m_SelectRow > -1)
   {
      DeleteRow(m_SelectRow);

      if(GetNumberRows())
         OnUpdateSelection((m_SelectRow == 0)?0:m_SelectRow - 1);      

      m_SelectRow = (m_SelectRow == 0 && GetNumberRows())?0:m_SelectRow - 1;
   }

   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CCustomMachineGrid::OnUpdateSelection(int curRow)
{
   if (curRow >= 0 && curRow < GetNumberRows() && m_ColumnSize > 0)
   {
      SetMultiSelectMode(TRUE);
      GotoRow(curRow);
      SelectRange(0, curRow, m_ColumnSize, curRow);
   }
}

int CCustomMachineGrid::OnEditVerifyAttributeCell(int col, long row, CWnd *edit)
{
   CString curTxt, restoreText;

   restoreText = QuickGetText(col, row);   
   CEdit *cedit = (CEdit*)edit;
   edit->GetWindowText(curTxt);
   
   int errorCode = CTMachineOK;
   CCustomeMachineDlg *dlg = (CCustomeMachineDlg *)m_cmdlg;

   errorCode = (curTxt.IsEmpty())?CTMachine_NameEmpty:CTMachineOK;
   errorCode += (!errorCode)?dlg->VerifyMachineNameInBuiltInMachines(curTxt, restoreText):CTMachineOK;
   errorCode += (!errorCode)?dlg->VerifyMachineNameInGrid(row,curTxt):CTMachineOK;
   
   if(errorCode)
   {
      dlg->showErrorMessages(errorCode);
      edit->SetWindowText(restoreText);
   }

   return !(errorCode);
}

void CCustomMachineGrid::OnDClicked(int col,long row, RECT *rect,POINT *point,BOOL processed)
{
   if(col == T_CTMachineTemplate && !processed)
   {
      CString selectTemplate = ((CCustomeMachineDlg *)m_cmdlg)->OnSelectTemplateDlg();

      if(!selectTemplate.IsEmpty())
      {  
         QuickSetText(col, row, selectTemplate);
         BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
      }
   }
   else
   {
      StartEdit(col, row, 0);
   }
}

void CCustomMachineGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   m_SelectRow = row;
   m_SelectCol = col;
   OnUpdateSelection(row);   
}

//------------------------------------------------------------------
// CCustomMachineGrid: extended Grid Control event
//------------------------------------------------------------------
void CCustomMachineGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320341/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(FALSE);
   m_myCUGEdit.m_ctrl = this;	

}

void CCustomMachineGrid::OnCharDown(UINT* vcKey,BOOL processed)
{
   // start editing when the user hits a character key on a cell in the grid.  
   // Pass that key to the edit control, so it doesn't get lost
   StartEdit(*vcKey); 
} 


int CCustomMachineGrid::OnEditStart(int col, long row, CWnd **edit)
{
   *edit = &m_myCUGEdit;
   return true;
}

int CCustomMachineGrid::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	return TRUE;
}

int CCustomMachineGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{    
   return OnEditVerifyAttributeCell(col,row,edit)?true:false;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CCustomeMachineDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

IMPLEMENT_DYNAMIC(CCustomeMachineDlg, CPropertyPage)
CCustomeMachineDlg::CCustomeMachineDlg(CDialog *parent, CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CDBInterface &database)
	: CPropertyPage(CCustomeMachineDlg::IDD)
   , m_linecfg(parent)
   , m_camCadDoc(camCadDoc)
   , m_pcbFile(pPCBFile)
   , m_database(database)
   , m_MachineListGrid(camCadDoc, pPCBFile, this)
{
   m_initial = false;
}

CCustomeMachineDlg::~CCustomeMachineDlg()
{
}

void CCustomeMachineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDIT_MACHINENAME, m_machineName);   
   DDX_Text(pDX, IDC_EDIT_TEMPLATENAME,m_TemplateName);
   DDX_Text(pDX, IDC_TEMPLATE_FILEPATH,m_FilePath);
}


BEGIN_MESSAGE_MAP(CCustomeMachineDlg, CDialog)
   ON_BN_CLICKED(IDC_BNT_ADDMACHINE, OnBnClickedBntAddmachine)
   ON_BN_CLICKED(IDC_SET_TEMPLATE, OnBnClickedSetTemplate)
   ON_BN_CLICKED(IDC_BNT_REMOVEMACHINE, OnBnClickedBntRemovemachine)
END_MESSAGE_MAP()

BOOL CCustomeMachineDlg::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_FilePath = m_database.GetLibraryDatabaseName();

   m_MachineListGrid.AttachGrid(this,IDC_MACHINELIST_GRID);
   m_MachineListGrid.initGrid();

   LoadMachines();
   m_initial = true;

   UpdateData(FALSE);
   return TRUE;
}

void CCustomeMachineDlg::LoadMachines()
{
   LoadMachinesFromLibrary();
}

int CCustomeMachineDlg::VerifyMachineNameInGrid(int exclrow, CString srcMachineName)
{
   //Comapre with the Grid to see if new machine name is duplicate
   for(int idx = 0; idx < m_MachineListGrid.GetNumberRows(); idx++)
   {
      CCustomMachineData machineData;
      m_MachineListGrid.GetRowData(idx, machineData);
      if(idx != exclrow && !srcMachineName.CompareNoCase(machineData.getMachineName()))
      {
         return CTMachine_GridDuplicate;
      }
   }

   return CTMachineOK;
}

int CCustomeMachineDlg::VerifyMachineNameInBuiltInMachines(CString srcMachineName, CString excMachineName)
{
   //Compare with machine list to see if new machine name is duplicate
   /*for(POSITION machinePos = m_camCadDoc.GetMachineHeadPosition(m_pcbFile);machinePos;)
   {
      CMachine* machine = m_camCadDoc.GetMachineNext(m_pcbFile,machinePos);

      if(machine)
      {
         if(!srcMachineName.CompareNoCase(machine->GetName()) && machine->GetName().CompareNoCase(excMachineName))
         {
            return CTMachine_BuiltInDuplicate;
         }
      }
   }*/ //for

   return CTMachineOK;
}

void CCustomeMachineDlg::showErrorMessages(int errorCode)
{
   CString ErrMessages = "";
   switch(errorCode)
   {
   case CTMachine_NameEmpty:
      ErrMessages = "Machine Name can't be empty!!";
      break;
   case CTMachine_TemplateEmpty:
      ErrMessages = "Template can't be empty!!";
      break;
   case CTMachine_GridDuplicate:
   case CTMachine_BuiltInDuplicate:
      ErrMessages = "Machine Name is duplicate in Grid or Bult-In machine list!!";
      break;

   }

   if(errorCode)
      formatMessageBox(ErrMessages);
}

void CCustomeMachineDlg::LoadMachinesFromLibrary()
{
   if (m_database.IsConnected() && !m_MachineListGrid.GetNumberRows())
   {
      CDBCustomMachineList *machineList = m_database.LookupCustomMachines("");
      if(machineList)
      {
         for(int idx = 0; idx < machineList->GetCount(); idx++)
         {
            CDBCustomMachine *cmdata =  machineList->GetAt(idx);
            if(cmdata)
            {
               CCustomMachineData machineData;
               machineData.Add(cmdata->getMachineName(), cmdata->getTemplateName());
               m_MachineListGrid.AddRowData(machineData);
            }
         }
         delete machineList;
      }//if
   }

   UpdateData(FALSE);
}

void CCustomeMachineDlg::UpdateMachineOrigin()
{
   if (!m_database.IsConnected()) return;

   // Remove the machine origin that has different template
   for(int row = 0; row < m_MachineListGrid.GetNumberRows(); row++)
   {
      CCustomMachineData machineData;
      m_MachineListGrid.GetRowData(row, machineData);

      CDBCustomMachineList *machineList = m_database.LookupCustomMachines(machineData.getMachineName());
      if(machineList && machineList->GetCount())
      {
         CDBCustomMachine *cdbmachine = machineList->GetAt(0);
         /*if(cdbmachine && cdbmachine->getTemplateName().CompareNoCase(machineData.getTemplateFile()))
            m_camCadDoc.DeleteMachineOrigin(&m_pcbFile, machineData.getMachineName());*/
      }

      delete machineList;
   }
}

void CCustomeMachineDlg::StoreMachinesToLibrary()
{
   if (m_database.IsConnected())
   {
      UpdateMachineOrigin();

      m_database.DeleteCustomMachine("");
      for(int row = 0; row < m_MachineListGrid.GetNumberRows(); row++)
      {
         CCustomMachineData machineData;
         m_MachineListGrid.GetRowData(row, machineData);

         CDBCustomMachine cmdata(machineData.getMachineName(),machineData.getTemplateFile());
         m_database.SaveCustomMachine(&cmdata);
      }
   }
}

CString CCustomeMachineDlg::OnSelectTemplateDlg()
{
   CString selectedTemplate = "";
   if (m_database.IsConnected())
   {
      CSelectDlg dlg("Select Template", false, true);
      
      CDBCustomAssemblyTemplateList *cdbtemplatelist = m_database.LookupCustomAssemblyTemplates("");
      if(cdbtemplatelist)
      {
         for(int idx = 0 ; idx < cdbtemplatelist->GetCount(); idx++)
         {
            CDBCustomAssemblyTemplate *cdbtemplate = cdbtemplatelist->GetAt(idx);
            if(cdbtemplate)
            {
               CSelItem *templateItem = new CSelItem(cdbtemplate->getTemplateName(),(!idx)?true:false);
               dlg.AddItem(templateItem);
            }
         }
         delete cdbtemplatelist;
      }

      if (dlg.DoModal() == IDOK)
      {
         selectedTemplate = dlg.GetSelItemHead()->GetName();
      }
   }  
    
   return selectedTemplate;
}

// TODO: Add your control notification handler code here
void CCustomeMachineDlg::OnBnClickedBntAddmachine()
{
   UpdateData(TRUE);

   int errorCode = CTMachineOK;
   errorCode = ( m_machineName.IsEmpty())?CTMachine_NameEmpty:CTMachineOK;
   errorCode += (!errorCode && m_TemplateName.IsEmpty())?CTMachine_TemplateEmpty:CTMachineOK;
   errorCode += (!errorCode)?VerifyMachineNameInBuiltInMachines(m_machineName,""):CTMachineOK;
   errorCode += (!errorCode)?VerifyMachineNameInGrid(-1, m_machineName):CTMachineOK;      

   if(!errorCode)
   {
      CCustomMachineData machineData;
      machineData.Add(m_machineName,m_TemplateName);
      m_MachineListGrid.AddRowData(machineData);
   }

   showErrorMessages(errorCode);

   UpdateData(FALSE);
}

void CCustomeMachineDlg::OnBnClickedSetTemplate()
{
   UpdateData(TRUE);

   m_TemplateName = OnSelectTemplateDlg();
   UpdateData(FALSE);
}

void CCustomeMachineDlg::OnBnClickedBntRemovemachine()
{
   m_MachineListGrid.DeleteSelectedRow();
}

void CCustomeMachineDlg::SavetoLibrary()
{
   UpdateData(TRUE);

   StoreMachinesToLibrary();
   //Update Machine Origin
   if(m_linecfg)
   {
      ((CLineConfigurationDlg*)m_linecfg)->getMachineOriginDlg().LoadMachines();
   }
}

BOOL CCustomeMachineDlg::OnKillActive()
{
   SavetoLibrary();
   return TRUE;
}

void CCustomeMachineDlg::OnCancel()
{
   if(m_initial)
      SavetoLibrary();
}

////////////////////////////////////////////////////////////////////////////////
// CMachineListGrid
////////////////////////////////////////////////////////////////////////////////
CMachineListGrid::CMachineListGrid(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile)
: m_camCadDoc(camCadDoc)
, m_pcbFile(pPCBFile)
{
   m_redBackgroundColor       = RGB(0xff,0xf0,0xf5);
   m_greenBackgroundColor     = RGB(0xf0,0xff,0xf0);
   m_whiteBackgroundColor     = RGB(0xff,0xff,0xff);
   m_lightGrayBackgroundColor = GetSysColor(COLOR_BTNFACE);

   m_gridHeader.RemoveAll();
   m_gridHeader.Add(QMACHINENAME);
   m_gridHeader.Add(QMACHINETYPE);
   m_gridHeader.Add(QMACHINESURFACE);
   m_gridHeader.Add(QMACHINEFROM);
   m_gridHeader.Add(QMACHINESET);

   m_ColumnSize = m_gridHeader.GetCount();
   m_SelectRow = -1;
   m_SelectCol = -1;

}

void CMachineListGrid::initGrid()
{
   if (GetNumberCols() < m_gridHeader.GetCount())
   {     
      int defaultColSize = m_gridHeader.GetCount();
      SetSH_Width(0);
      SetNumberRows(0,false);
      SetNumberCols(defaultColSize,false);

      for(int idx = 0; idx < defaultColSize; idx++)
      {
         if( m_gridHeader.GetAt(idx))
            QuickSetText(idx, -1, m_gridHeader.GetAt(idx));
      }
   }
   SetNumberRows(0);
   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CMachineListGrid::SetCellOption(int col, int option)
{
   //First Column is a droplist
   CUGCell cell;
   GetColDefault(col, &cell); 
   cell.SetCellType(option); 
   SetColDefault(col, &cell);
}

void CMachineListGrid::QuickSetDropList(int col, int row, CString options)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   CString optionlist(options); 
   optionlist.Replace("|", "\n");
   cell.SetLabelText( optionlist );

   CSupString supOption(options);
   CStringArray optionArray;   
   supOption.ParseQuote(optionArray,"|");
   cell.SetText(optionArray.GetCount()?optionArray.GetAt(0):"");
   SetCell(col, row, &cell);
}

void CMachineListGrid::AddRowData(CMachine* machine)
{
   int RowSize = GetNumberRows();
   if(machine)
   {
      AddRowData(machine->GetName(), machine->GetMachineTypeString(),  machine->GetMachineSurface(), machine->GetMachineAttributeName(false));
   }

}

void CMachineListGrid::AddRowData(CString machineName, CString MachineType, CString MachineSide, CString AttribName)
{
   int RowSize = GetNumberRows();
   SetNumberRows(RowSize + 1);
   QuickSetText(T_MachineName, RowSize, machineName);  
   QuickSetText(T_MachineType, RowSize, MachineType);
   QuickSetText(T_MachineSurface, RowSize, MachineSide);
   QuickSetText(T_MachineSet,RowSize,"...");

   CString MachineSettings = GetMachineAttribute(AttribName);
   QuickSetText(T_MachineFrom, RowSize, (MachineSettings.IsEmpty())?"Library":"File");   

   SetRowBackColor(RowSize, (MachineSettings.IsEmpty())?m_redBackgroundColor:m_greenBackgroundColor);
   QuickSetBackColor(T_MachineSet,RowSize,m_lightGrayBackgroundColor);

   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CMachineListGrid::GetRowData(int row, CString &machineName, CString &MachineType, CString &MachineSide)
{
   if(row > -1 && row < GetNumberRows())
   {
      machineName = QuickGetText(T_MachineName, row);
      MachineType = QuickGetText(T_MachineType, row);
      MachineSide = QuickGetText(T_MachineSurface, row);
   }
}

void CMachineListGrid::SetCellReadOnly(int row, bool readonly)
{
   for(int col = 0; col < this->GetNumberCols(); col ++)
      SetCellReadOnly(col,row,readonly);
}

void CMachineListGrid::SetCellReadOnly(int col, int row, bool readonly)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   cell.SetReadOnly(readonly);   
   SetCell(col, row, &cell);  
}

void CMachineListGrid::OnDClicked(int col,long row, RECT *rect,POINT *point,BOOL processed)
{
   if(col == T_MachineSet && !processed)
      OnPushSetButton(col,row);
}

void CMachineListGrid::OnPushSetButton(int col,long row)
{
   CString machineName = QuickGetText(T_MachineName, row);
   CString surface  = QuickGetText(T_MachineSurface, row);
   bool isBothSide = !(surface.CompareNoCase(QSURFACEBOTH))?true:false;

   CPrepDlg m_predlg(machineName, isBothSide);

   if(m_predlg.DoModal() == IDOK)
   {
      CString MachineSettings = GetMachineAttribute(GetMachineAttributeName(machineName,false));      
      QuickSetText(T_MachineFrom, row, (MachineSettings.IsEmpty())?"Library":"File");   
      
      SetRowBackColor(row, (MachineSettings.IsEmpty())?m_redBackgroundColor:m_greenBackgroundColor);
      QuickSetBackColor(T_MachineSet,row,m_lightGrayBackgroundColor);   
      BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
   } 
}

CString CMachineListGrid::GetMachineAttribute(CString MachineAttribName)
{
   Attrib *attrib =  is_attvalue(&m_camCadDoc, m_pcbFile.getBlock()->getDefinedAttributes(), MachineAttribName, 0);
   CString attribString = (attrib)?attrib->getStringValue():"";

   return attribString;
}

void CMachineListGrid::DeleteMachinesByType(CString MachineType)
{
   for(int row = 0; row < GetNumberRows(); row++)
   {
      CString cur_machineType;
      QuickGetText(T_MachineType, row, &cur_machineType);

      if(!cur_machineType.CompareNoCase(MachineType))
      {
         DeleteRow(row);
         row --;
      }
   }
   BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
}

void CMachineListGrid::DeleteAll()
{
   for(m_SelectRow = GetNumberRows();m_SelectRow > -1; )
      DeleteRow(m_SelectRow--);
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CMachineOriginDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

IMPLEMENT_DYNAMIC(CMachineOriginDlg, CPropertyPage)
CMachineOriginDlg::CMachineOriginDlg(CDialog *parent, CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CDBInterface &database)
	: CPropertyPage(CMachineOriginDlg::IDD)
   , m_linecfg(parent)
   , m_camCadDoc(camCadDoc)
   , m_pcbFile(pPCBFile)
   , m_database(database)
   , m_MachineListGrid(camCadDoc, pPCBFile)
{
}

CMachineOriginDlg::~CMachineOriginDlg()
{
}

void CMachineOriginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMachineOriginDlg, CDialog)
END_MESSAGE_MAP()

BOOL CMachineOriginDlg::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_MachineListGrid.AttachGrid(this,IDC_MACHINELIST_GRID);
   m_MachineListGrid.initGrid();

   LoadMachines();
   return TRUE;
}

void CMachineOriginDlg::LoadMachines()
{
   LoadMachinesFromLibrary();
   LoadMachinesFromMachineList();
}

void CMachineOriginDlg::LoadMachinesFromMachineList()
{
   m_MachineListGrid.DeleteAll();
   /*for(POSITION machinePos = m_camCadDoc.GetMachineHeadPosition(m_pcbFile);machinePos;)
   {
      CMachine* machine = m_camCadDoc.GetMachineNext(m_pcbFile,machinePos);

      if(machine && (machine->IsPrepSettingMachine()) && 
         machine->IsFileTypeforMachine(m_pcbFile.getBlockType()) &&
         machine->HasConfiguration())
      {
         m_MachineListGrid.AddRowData(machine);
      }
   }*/ 
}

void CMachineOriginDlg::LoadMachinesFromLibrary()
{  
   if (m_database.IsConnected())
   {
      DeleteMachinesByType(MACHINE_TYPE_CUT);
      CDBCustomMachineList *machineList = m_database.LookupCustomMachines("");      

      if(machineList)
      {
         for(int idx = 0; idx < machineList->GetCount(); idx++)
         {
            CDBCustomMachine *cmdata =  machineList->GetAt(idx);
            CString machineName = "", TemplateName = "";
            if(cmdata)
            {
               machineName = cmdata->getMachineName();
               TemplateName = cmdata->getTemplateName();
            }

            if(!machineName.IsEmpty() && !TemplateName.IsEmpty())
            {
               CDBCustomAssemblyTemplateList *cdbtemplateList = m_database.LookupCustomAssemblyTemplates(TemplateName);
               //CMachine *machine = m_camCadDoc.getMachineList(m_pcbFile)->FindMachine(machineName);
               
               CString PCBside = (cdbtemplateList && cdbtemplateList->GetCount())?cdbtemplateList->GetAt(0)->getPCBSide():"";
               PCBside = (!PCBside.CompareNoCase(QSURFACEBOTH))?QSURFACEBOTH:QSURFACETOPBOT;

               /*if(!machine)
               {         
                  machine = new CMachine(fileTypeCustomAssembly,machineName,PCBside);
                  m_camCadDoc.getMachineList(m_pcbFile)->AddTail(machine);
               }
               else 
               {
                  if(!PCBside.IsEmpty())
                     machine->SetMachineSurface(PCBside);
               }*/

               delete cdbtemplateList;
            }            
         }
         delete machineList;
      }//if
   }

   UpdateData(FALSE);
}

void CMachineOriginDlg::DeleteMachinesByType(int MachineType)
{
   /*for(int row = 0; row < m_MachineListGrid.GetNumberRows(); row ++)
   {
      CString machineName, machineType, machineSide;
      m_MachineListGrid.GetRowData(row, machineName,machineType,machineSide);
      if(!machineName.IsEmpty() && machineType == getMachineTypeString(MachineType))
      {
         CDBCustomMachineList *machineList = m_database.LookupCustomMachines(machineName);
         if(machineList && !machineList->GetCount())
         {
            m_camCadDoc.DeleteMachineOrigin(&m_pcbFile,machineName);
            m_camCadDoc.getMachineList(m_pcbFile)->DeleteMachines(machineName);
         }
         delete machineList;
      }
   }*/  
}

// CMachineOriginDlg message handlers
void CMachineOriginDlg::OnCancel()
{
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CLineConfigurationDlg dialog
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
IMPLEMENT_DYNAMIC(CLineConfigurationDlg, CDialog)
CLineConfigurationDlg::CLineConfigurationDlg(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CWnd* pParent)
: CDialog(CLineConfigurationDlg::IDD, pParent)
, m_camCadDoc(camCadDoc)
, m_pPCBFile(pPCBFile)
, m_machineOriginDlg(this, camCadDoc, pPCBFile, m_database)
, m_customMachineDlg(this, camCadDoc, pPCBFile, m_database)
{
   ConnectDatabase();
}

CLineConfigurationDlg::~CLineConfigurationDlg()
{
}

void CLineConfigurationDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLineConfigurationDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CLineConfigurationDlg, CDialog)
END_EVENTSINK_MAP()

BOOL CLineConfigurationDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   CreateTabDialog();

   return TRUE; 
}

void CLineConfigurationDlg::CreateTabDialog()
{
   // Add pages to the tab control
   m_propSheet.AddPage(&m_machineOriginDlg);
   m_propSheet.AddPage(&m_customMachineDlg);

   // Create the property sheet
   m_propSheet.Create(this, WS_CHILD | WS_VISIBLE/* | WS_BORDER*/, 0);
   m_propSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
   m_propSheet.ModifyStyle( 0, WS_TABSTOP );

   // Place property sheet at the location of IDC_STATIC_LINESHEET
   CRect rcSheet;
   GetDlgItem( IDC_STATIC_PROPSHEET )->GetWindowRect( &rcSheet );
   ScreenToClient( &rcSheet );
   m_propSheet.SetWindowPos( NULL, rcSheet.left, rcSheet.top, rcSheet.Width(), rcSheet.Height(), 
         SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
}

void CLineConfigurationDlg::ConnectDatabase()
{
   if(!m_database.IsConnected())
      m_database.Connect();
}

void CLineConfigurationDlg::OnCancel()
{
   m_customMachineDlg.OnCancel();
   m_machineOriginDlg.OnCancel();

   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   if(frame) 
      frame->getNavigator().FillMachineList(&m_camCadDoc,&m_pPCBFile); 
   m_camCadDoc.UpdateAllViews(NULL);

   CDialog::OnCancel();
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CCEtoODBDoc
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
void CCEtoODBDoc::OnLineConfiguration()
{
   FileStruct *pcbFile = this->getFileList().GetOnlyShown(blockTypePcb);
   if(!pcbFile) pcbFile = this->getFileList().GetOnlyShown(blockTypePanel);

   if (pcbFile)
   {   
      //CreatMachineList(*pcbFile);
      CLineConfigurationDlg dlg(*this, *pcbFile, AfxGetMainWnd());
      dlg.DoModal();
   }
   else
   {
      ErrorMessage("No visible PCB/Panel file detected.\n\nLine Configuration requires a visible PCB/Panel file.",
                   "Line Configuration");
   }

}


