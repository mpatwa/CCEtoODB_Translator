// $Header: /CAMCAD/4.3/read_wrt/AgilentSjPlxIn.h 4     11/05/03 7:12p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if ! defined (__AgilentSjPlxIn_h__)
#define __AgilentSjPlxIn_h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "RwLib.h"

//_____________________________________________________________________________
class CAgilentSjPlxFileReader : public CFileReader
{
private:
   CStdioFile* m_stdioFile;
   CString m_filePath;
   CString m_record;

   char* m_start[MaxParserParams];
   char* m_end[MaxParserParams];
   char m_numBuf[MaxNumLength + 1];

public:
   CAgilentSjPlxFileReader(int maxParams = MaxParserParams);
   ~CAgilentSjPlxFileReader();

   void setRecord(const CString& record);
   bool open(const CString& filePath);
   FileStatusTag openFile(const CString& filePath);
   void close();
   bool readRecord();
   FileStatusTag readFileRecord(CWriteFormat* statusLog=NULL);
   CString getRecord() { return m_record; }
   CString getFilePath() { return m_filePath; }
   void setInchesPerUnit(double inchesPerUnit) { m_inchesPerUnit = inchesPerUnit; }

   int getNumParams();
   int getNumRecordsRead() { return m_numRecordsRead; }
   CString getParam(int index);
   void getParam(CString& param,int index);
   void getParam(int& iParam,int index);
   void getParam(double& fParam,int index);
   void getParamInInches(double& fParam,int index) { getParam(fParam,index);  fParam *= m_inchesPerUnit; }
   int getFileLength();
   int getFilePosition();

private:
   void parse();
};

//_____________________________________________________________________________
class CAgilentSjPlxInDialog : public CResizingDialog
{
	//DECLARE_DYNAMIC(CAgilentSjPlxInDialog)

private:
   CString m_filePathTop;
   CString m_filePathBottom;

public:
	CAgilentSjPlxInDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAgilentSjPlxInDialog();

   CString getFilePathTop()    { return m_filePathTop;    }
   CString getFilePathBottom() { return m_filePathBottom; }

// Dialog Data
	enum { IDD = IDD_FORMAT_AgilentSjPlx_In };

   virtual CString GetDialogProfileEntry() { return CString("FormatAgilentSjPlxInDialog"); }

   void addFilePath(const CString& filePath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedFileBrowse();
   afx_msg void OnBnClickedSwapTopBottom();
};

#endif
