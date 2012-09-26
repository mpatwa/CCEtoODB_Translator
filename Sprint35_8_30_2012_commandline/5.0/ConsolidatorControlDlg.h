#pragma once

#include "afxwin.h"

// ConsolidatorControlDlg dialog

class ConsolidatorControlDlg : public CDialog
{
	DECLARE_DYNAMIC(ConsolidatorControlDlg)

private:
   BOOL m_considerOriginalName;
   int  m_pinsOnlyRadio;
   CString m_toleranceStr;
   double m_tolerance;

public:
	ConsolidatorControlDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ConsolidatorControlDlg();

// Dialog Data
	enum { IDD = IDD_CONSOLIDATOR_CONTROL };

   bool ConsiderOriginalName()      { return m_considerOriginalName?true:false; }
   bool ConsiderPinsOnly()          { return m_pinsOnlyRadio==0?true:false; }
   double Tolerance()               { return atof(m_toleranceStr); }

   void ConsiderOriginalName(bool b)      { m_considerOriginalName = b; }
   void ConsiderPinsOnly(bool b)          { m_pinsOnlyRadio = b?0:1; }
   void Tolerance(double f)               { m_toleranceStr.Format("%f", f); }
   

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
