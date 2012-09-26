
#if !defined(__LINECONFIGURATION_H__)
#define LINECONFIGURATION_H

#pragma once
#include "CCEtoODB.h"
#include "GeneralDialog.h"
#include "FlexGridLib.h"  // Use the CFlexGridStatic class from it
#include "ccdoc.h"
#include "afxwin.h"
#include "RwUiLib.h"
#include "MultipleMachine.h"

#undef __AFXOLE_H__
#include "DataDoctorDialog.h"

#define  QMACHINETYPE      "Machine Type"
#define  QMACHINENAME      "Machine Name"
#define  QMACHINESURFACE   "Side"
#define  QMACHINEFROM      "From"
#define  QMACHINESET       "Set"
#define  QMACHINETEMPLATE  "Template Name"

#define  QTEMPLATEFILEEXT        "rpt"
#define  QTEMPLATEFILE           "*.rpt"
#define  QTEMPLATEFILTER         "Template File (*.rpt)|*.RPT|All Files (*.*)|*.*||"

enum MachineGridTag
{
   T_MachineName = 0,
   T_MachineType,
   T_MachineSurface,
   T_MachineFrom,
   T_MachineSet,
   T_MachineLast,

   T_CTMachineName = 0,
   T_CTMachineTemplate,
   T_CTMachineLast,
};

enum CTMachineErrorCode
{
   CTMachineOK,
   CTMachine_NameEmpty,
   CTMachine_GridDuplicate,
   CTMachine_BuiltInDuplicate,
   CTMachine_TemplateEmpty,
};

////////////////////////////////////////////////////////////////////////////////
// CCustomMachineData
////////////////////////////////////////////////////////////////////////////////
class CCustomMachineData
{
private:
   CString m_machineName;
   CString m_TemplateFile;

public:
   CString &getMachineName(){return m_machineName;}
   CString &getTemplateFile(){return m_TemplateFile;}
   void Add(CString machineName, CString TemplateFile){m_machineName = machineName; m_TemplateFile = TemplateFile;}
};

////////////////////////////////////////////////////////////////////////////////
// CCustomMachineGrid
////////////////////////////////////////////////////////////////////////////////
class CCustomMachineGrid : public CDDBaseGrid
{
private:
   CCEtoODBDoc& m_camCadDoc;
   FileStruct& m_pcbFile;
   CStringArray   m_gridHeader;
   CUGEdit m_myCUGEdit;  
   CDialog *m_cmdlg;

   CString  m_FilePath;
   int   m_ColumnSize;
   int   m_SelectRow;
   int   m_SelectCol;

   COLORREF m_redBackgroundColor;
   COLORREF m_greenBackgroundColor;
   COLORREF m_lightGrayBackgroundColor;
   COLORREF m_whiteBackgroundColor;

private:
   void SetCellReadOnly(int row, bool readonly);
   void SetCellReadOnly(int col, int row, bool readonly);
   void SetCellOption(int col, int option);
   int OnEditVerifyAttributeCell(int col, long row, CWnd *edit);   

   //event
   void OnUpdateSelection(int curRow);
   void OnDClicked(int col,long row, RECT *rect,POINT *point,BOOL processed);
   void OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed);

   //control event
   void OnSetup();
   void OnCharDown(UINT* vcKey,BOOL processed);
   int OnEditStart(int col, long row, CWnd **edit);
   int OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey);
   int OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag);

public:
   CCustomMachineGrid(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CDialog *dlg);
   
   void  initGrid();
   void  QuickSetDropList(int col, int row, CString options);

   void  AddRowData(CCustomMachineData machinedata);
   void  GetRowData(CCustomMachineData& machinedata);
   void  GetRowData(int row, CCustomMachineData& machinedata);
   void  DeleteSelectedRow();

};

////////////////////////////////////////////////////////////////////////////////
// CCustomeMachineDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CCustomeMachineDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CCustomeMachineDlg)

public:
	CCustomeMachineDlg(CDialog *parent, CCEtoODBDoc& camCadDoc, FileStruct &pcbFile, CDBInterface &database);   // standard constructor
	virtual ~CCustomeMachineDlg();

// Dialog Data
	enum { IDD = IDD_CUSTOMMACHINE };

private:
   FileStruct& m_pcbFile;
   CCEtoODBDoc& m_camCadDoc;
   CDBInterface& m_database;
   CDialog *m_linecfg;

   //Control Objects
   CCustomMachineGrid   m_MachineListGrid;
   CAssemblyTemplateDirectory m_Templatedir;
   
   CString  m_machineName;
   CString  m_FilePath;
   CString  m_TemplateName;
   bool     m_initial;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   
   
	DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnBnClickedBntAddmachine();
   afx_msg void OnBnClickedSetTemplate();
   afx_msg void OnBnClickedBntRemovemachine();
   afx_msg BOOL OnKillActive();
   afx_msg void OnCancel();

   CString OnSelectTemplateDlg();
   void  showErrorMessages(int errorCode);

   void  LoadMachines();
   void  LoadMachinesFromLibrary();
   void  StoreMachinesToLibrary();
   void  UpdateMachineOrigin();
   int   VerifyMachineNameInGrid(int exclrow, CString srcMachineName);
   int   VerifyMachineNameInBuiltInMachines(CString srcMachineName, CString excMachineName);
   void  SavetoLibrary();

};

////////////////////////////////////////////////////////////////////////////////
// CMachineListGrid
////////////////////////////////////////////////////////////////////////////////
class CMachineListGrid : public CDDBaseGrid
{
private:
   CCEtoODBDoc& m_camCadDoc;
   FileStruct& m_pcbFile;
   CStringArray   m_gridHeader;
   CUGEdit m_myCUGEdit;    
   
   int   m_ColumnSize;
   int   m_SelectRow;
   int   m_SelectCol;

   COLORREF m_redBackgroundColor;
   COLORREF m_greenBackgroundColor;
   COLORREF m_lightGrayBackgroundColor;
   COLORREF m_whiteBackgroundColor;

private:
   void SetCellReadOnly(int row, bool readonly);
   void SetCellReadOnly(int col, int row, bool readonly);
   void SetCellOption(int col, int option);
   
   CString  GetMachineAttribute(CString MachineAttribName);

   //event
   void  OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
   void  OnPushSetButton(int col,long row);

public:
   CMachineListGrid(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile);
   
   void  initGrid();
   void  AddRowData(CMachine* machine);
   void  AddRowData(CString machineName, CString MachineType, CString MachineSide, CString AttribName);
   
   void  GetRowData(int row, CString &machineName, CString &MachineType, CString &MachineSide);
   void  QuickSetDropList(int col, int row, CString options);
   void  DeleteMachinesByType(CString MachineType);
   void  DeleteAll();
};


////////////////////////////////////////////////////////////////////////////////
// CMachineOriginDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CMachineOriginDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CMachineOriginDlg)

public:
	CMachineOriginDlg(CDialog *parent, CCEtoODBDoc& camCadDoc, FileStruct &pcbFile, CDBInterface &database);   // standard constructor
	virtual ~CMachineOriginDlg();

// Dialog Data
	enum { IDD = IDD_MACHINEORIGIN };

private:
   FileStruct& m_pcbFile;
   CCEtoODBDoc& m_camCadDoc;
   CDBInterface& m_database;
   CDialog *m_linecfg;

   //Control Objects
   CMachineListGrid   m_MachineListGrid;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

public:
   void  LoadMachines();
   void  LoadMachinesFromMachineList();
   void  LoadMachinesFromLibrary();
   void  DeleteMachinesByType(int MachineType);

   afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////
// CLineConfigurationDlg dialog
////////////////////////////////////////////////////////////////////////////////
class CLineConfigurationDlg : public CDialog
{
	DECLARE_DYNAMIC(CLineConfigurationDlg)

public:
	CLineConfigurationDlg(CCEtoODBDoc& camCadDoc, FileStruct& pPCBFile, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLineConfigurationDlg();
	enum { IDD = IDD_LINECONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

   afx_msg void OnCancel();

public:
	DECLARE_EVENTSINK_MAP()

private:
	CCEtoODBDoc& m_camCadDoc;
	FileStruct& m_pPCBFile;
   CDBInterface m_database;	

	// Controls members
	CPropertySheet m_propSheet;
   CMachineOriginDlg m_machineOriginDlg;
   CCustomeMachineDlg m_customMachineDlg;

   void CreateTabDialog();
   void ConnectDatabase();

public:
   virtual BOOL OnInitDialog();

   CMachineOriginDlg& getMachineOriginDlg(){return m_machineOriginDlg;}
   CCustomeMachineDlg& getCustomeMachineDlg(){return m_customMachineDlg;}

};
#endif /* LINECONFIGURATION_H*/
