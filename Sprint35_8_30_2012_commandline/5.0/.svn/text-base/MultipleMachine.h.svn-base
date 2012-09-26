
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// MULTIPLEMACHINE.H

#if ! defined (__MultipleMachine_h__)
#define __MultipleMachine_h__

#pragma once

#include "FlexGridLib.h"  // Use the CFlexGridStatic class from it
#include "afxwin.h"
#include "DcaMachine.h"

#define  QSURFACEBOTH      "BOTH"          
#define  QSURFACETOPBOT    "TOP/BOT"          
#define  QMACHINELOC_TOP   "_LOCATION_TOP"
#define  QMACHINELOC_BOT   "_LOCATION_BOTTOM"

enum MachineCategoryTag
{
   MachineCategoryUnknown,
   MachineCategoryDFT,
   MachineCategoryPrepSet,
};

class CMachineSelItem : public CSelItem
{
public:
	CMachineSelItem(CString name = "", CMachine *machine = NULL, BOOL selected = FALSE):CSelItem(name, selected)
	{
		m_pMachine = machine;
	}
	~CMachineSelItem()
	{
		m_pMachine = NULL;
	}

	CMachine *GetMachine() { return m_pMachine; }

private:
	CMachine *m_pMachine;
};


/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CMachine
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CMachine
{
public:
	CMachine(FileTypeTag fileType, CString machineName = "", CString PCBside = "");
	~CMachine();

private:
	FileTypeTag m_eFileType;
	CDFTSolution* m_pDFTSolution;
	CPoint2d m_ptOrigin;
   CString m_machineName;
   CString m_PCBSide;

public:
	int GetMachineType();

   CString GetName();
	CString GetMachineTypeString();
   CString GetMachineSurface(){return m_PCBSide;}
   CString GetMachineDefaultSurface();
   CString GetMachineAttributeName(bool isBottom){return GetName() + ((isBottom)?QMACHINELOC_BOT:QMACHINELOC_TOP);}
   Attrib *GetMachineAttribute(CCEtoODBDoc *doc, FileStruct* curFile, bool isBottomSide);

   bool IsFileTypeforMachine(BlockTypeTag blockType);
   bool HasConfiguration();

   MachineCategoryTag CMachine::GetMachineCategory();

	FileTypeTag GetFileType()								{ return m_eFileType;				};
	CDFTSolution* GetDFTSolution()						{ return m_pDFTSolution;			};
	CPoint2d GetOrigin()										{ return m_ptOrigin;					};
	
	void SetDFTSolution(CDFTSolution* solution)		{ m_pDFTSolution = solution;		};
	void SetOrigin(CPoint2d origin)						{ m_ptOrigin = origin;				};
   void SetMachineSurface(CString PCBside){m_PCBSide = PCBside;}

   bool IsDFTSolutionMachine()                     {return (GetMachineCategory() == MachineCategoryDFT);        }
   bool IsPrepSettingMachine()                     {return (GetMachineCategory() == MachineCategoryPrepSet);}
   bool IsBothSideSetting()                        {return !(GetMachineSurface().CompareNoCase(QSURFACEBOTH));}
};


/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CMachineList
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CMachineList : public CTypedPtrListContainer<CMachine*>
{
private:
   CCEtoODBDoc& m_camCadDoc;
   const FileStruct* m_pPCBFile;
   bool m_InitializedDFT;

public:
	CMachineList(CCEtoODBDoc& camCadDoc,const FileStruct* pPCBFile);
	~CMachineList();

public:
   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
	CMachine* FindMachine(CString name);
   bool  InitDFTSolution();
   void  DeleteMachines(CString name);

	void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
	int LoadXML(CString xmlString, CCEtoODBDoc *doc, FileStruct *file);
	void Scale(double factor);
};


/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CMultipleMachineDlg dialog
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

// These macros are use for Multiple Machine Dialog
#define COL_VERIFY							(long)0		
#define COL_MACHINE_TYPE					(long)1		
#define COL_MACHINE_NAME					(long)2		
#define COL_COPY_FROM_DFT					(long)3
#define COL_MACHINE_DFT						(long)4		

class CMultipleMachineDlg : public CDialog
{
	DECLARE_DYNAMIC(CMultipleMachineDlg)

public:
	CMultipleMachineDlg(CCEtoODBDoc *pDoc, FileStruct* pFile, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMultipleMachineDlg();
	enum { IDD = IDD_MM_MULTIPLE_MACHINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CCEtoODBDoc* m_pDoc;
	FileStruct* m_pPCBFile;
   CFlexGridStatic m_staMachineGrid;

	void initGrid(CExtendedFlexGrid* grid);
	void loadGrid(CExtendedFlexGrid* grid);

public:
   CCEtoODBDoc& getCamCadDoc() { return *m_pDoc; }

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	DECLARE_EVENTSINK_MAP()
	void CellButtonClickOnGrid(long Row, long Col);
	void ClickOnGrid();
	void BeforeEditGrid(long Row, long Col, VARIANT_BOOL* Cancel);
};


/******************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CDFTSelection dialog
/////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CDFTSelection : public CDialog
{
	DECLARE_DYNAMIC(CDFTSelection)

public:
	CDFTSelection(CCEtoODBDoc& m_camCadDoc,FileStruct* pFile, CString machineName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDFTSelection();
	enum { IDD = IDD_MM_DFT_Selection };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
   CCEtoODBDoc& m_camCadDoc;
	FileStruct* m_pPCBFile;
	CString m_sNewDFTName;
	CString m_sCopyFromDFTName;
	CComboBox m_cmbDFTSolutions;

	void loadDFTSolutionList();

public:
   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }

public:
	virtual BOOL OnInitDialog();

	CString GetNewDFTName()					{ return m_sNewDFTName;			};
	CString GetCopyFromDFTName()			{ return m_sCopyFromDFTName;	};

	afx_msg void OnBnClickedOk();
};


#endif
