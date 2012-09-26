
#if ! defined (__CustomAssembly_Writer_h__)
#define __CustomAssembly_Writer_h__
#pragma once
#include "ccdoc.h"
#include "General.h"
#include "CamCadDatabase.h"
#include "DcaCamcadFileWriter.h"
#include "afxwin.h"
#include "DcaWriteFormat.h"

#define QNextLine "\n"
#define QTECHNOLOGY_SMD "SMD"
#define QTECHNOLOGY_THRU "THRU"
#define QDIGITS    "+-.0123456789"

#define OPT_PLUS        "+"
#define OPT_MINUS       "-"
#define OPT_STRING      "String"
#define OPT_NUMBER      "Number"
#define OPT_UNITS       "Units"
#define OPT_INSERT      "Insert"
#define OPT_CENTROID    "Centroid"
#define OPT_REALPART    "RealPart"

#define CENT_BOLYOUTLINE "Body Outline"
#define CENT_PINCENTER   "Pin Center"
#define CENT_PINEXTENTS  "Pin Extents"
#define CENT_XYVALUE     "XY Value"

#define ATT_METHOD      "METHOD" 

class CMapInt : public CMap<int, int, int, int>
{
};

typedef CTypedPtrArrayContainer<CReportParameters*> CReportParameterList;

/////////////////////////////////////////////////////////////////////////////
// CSelectMultiMachinesDlg
/////////////////////////////////////////////////////////////////////////////
class CSelectMultiMachinesDlg : public CDialog
{
private:
   CListBox m_list;
   CString  m_directory;
   int m_machineCount;
   CDBCustomMachineList *m_cdbmachineList;
   CDBCustomMachineList m_selmachineList;

// Construction
public:
   CSelectMultiMachinesDlg(CString filename, CWnd* pParent = NULL);   // standard constructor
   ~CSelectMultiMachinesDlg();
// Dialog Data
   //{{AFX_DATA(CSelectMultiMachinesDlg)
   enum { IDD = IDD_REPORTS_SPREADSHEET };
   //}}AFX_DATA

   //output 
   int getSelMachineCount() {return m_selmachineList.GetCount();}

   CDBCustomMachine* getSelMachineAt(int idx) {return m_selmachineList.GetAt(idx);}
   CString getSelMachineNameAt(int idx) {return (m_selmachineList.GetAt(idx))?m_selmachineList.GetAt(idx)->getMachineName():"";}
   CString getSelTemplateNameAt(int idx) {return (m_selmachineList.GetAt(idx))?m_selmachineList.GetAt(idx)->getTemplateName():"";}
   void getSelMachineDataAt(int idx, CString &MachineName, CString &TemplateName);
   
   CString getDirectory() {return m_directory;}

   bool isMachinesExist();

private:
   void LoadMachines();

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CSelectMultiMachinesDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL
// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CSelectMultiMachinesDlg)
   afx_msg void OnBrowse();
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCustomAssemblyWriter
/////////////////////////////////////////////////////////////////////////////
class CCustomAssemblyWriter
{   
private:
   CProgressDlg*           m_progressDlg;
   CCEtoODBDoc&             m_cadDoc;
   FileStruct*             m_pcbfile;
   CCamCadData&            m_camCadData;
   CString                 m_reportFile;
   CMapInt                 m_InsertTypeMap;
   CReportParameterList    m_ParamList;
   CFormatStdioFile        m_fReport;  
   CDBInterface            m_database;
   CComponentRealpartLinkMap*  m_realpartMap;
   CDBCustomMachine&       m_cdbmachine;
   Attrib                  *m_attrib;
   CLogWriteFormat         m_textLogFile;

   bool           m_isRestructure; //ApplyCCZ is applied
   double         m_unitFactor;
   bool           m_writeHeader;
   bool           m_explodePanel;

   CString        m_delimiter;
   CString        m_topHeader;
   CString        m_botHeader;
   CString        m_incKeyword;
   CString        m_excKeyword;
   CString        m_incValue;
   CString        m_excValue;
   CString        m_boardName;

   int            m_decimals;
   int            m_mirrorX;
   int            m_mirrorY;
   ETestSurface   m_surface;
   PageUnitsTag   m_targetUnit;
   int            m_OverWriteCentroid;

public:
   CCustomAssemblyWriter(CString filename, CCEtoODBDoc &doc, FileStruct *pcbfile, FormatStruct *format, CDBCustomMachine &cdbmachine);
   ~CCustomAssemblyWriter();
   void writeFile();

private:
   CProgressDlg &getProgressDlg();
   int UpdateProcess(int pos, int length);

   void InitParameters(CString TemplateName);
   void InitSettings(CString TemplateName);
   void BuildInsertTypeMap(CAssemblyTemplat &m_tmplateData);

   void writeReport(bool isBottom);
   void writeReport();
   void writeReport(CString HeaderFileName);
   void writeReport(bool writeHeader, CReportParameterList &ParamList);
   void writeReport(CFileList &fileList);
   void writeReport(FileStruct *file);
   void writeReport(FileStruct *file, CTMatrix transformMatrix);
   void writeReport(DataStruct *dataList, CTMatrix transformMatrix, int option);
   void writeReport(DataStruct *data, CTMatrix transformMatrix);
   void writeReport(DataStruct *data, CTMatrix transformMatrix, CReportParameters* param); 

   void writeReport_String(CString value);
   void writeReport_Number(int value);
   void writeReport_Double(double value);
   void writeReport_Surface(DataStruct *data, CString options);
   void writeReport_Tolerance(DataStruct *data, CString options);
   void writeReport_Value(DataStruct *data, CString options);
   void writeReport_Location(DataStruct *data, CTMatrix transformMatrix, CString options, int ParamIndex);

   DataStruct *getCentroidInsertData(DataStruct *data);
   CString getGeometryName(DataStruct *data);
   CString getAttribStringValue(DataStruct *data, CString keywordString);
   int getParamIndex(CString Commands);
   int getCentroidMethod(CString option);
   bool IsMatchedTechnology(DataStruct *data, int option);
   bool getAttribDoubleValue(DataStruct *data, CString keywordString, double &value);
   void resetRealpartMap(FileStruct * file);
   double getLocationValue(CBasesVector insertBasesVector, int ParamIndex);
   double getLocationValue(double locPos, int mirror, double scale){return (locPos * mirror * scale);}

};

#endif /*__CustomAssembly_Writer_h__*/