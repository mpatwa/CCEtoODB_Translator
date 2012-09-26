// $Header: /CAMCAD/4.6/DataDoctorPagePAnalysis.h 3     4/06/07 4:47p Rick Faltersack $

#if ! defined (__DataDoctorPagePAnalysis_h__)
#define __DataDoctorPagePAnalysis_h__

#pragma once

#include "DataDoctorDialog.h"

//---------------------------------------------------------------------------------------

class CDataDoctorParallelAnalysisPropertyPage : public CDataDoctorPropertyPageType
{
	DECLARE_DYNAMIC(CDataDoctorParallelAnalysisPropertyPage)

private:
   CDataDoctorPropertySheet& m_parent;

   BOOL m_analyzeResistors;
   BOOL m_analyzeCapacitors;
   BOOL m_analyzeInductors;
   BOOL m_ignoreUnloadedComponents;
   BOOL m_mergeValues;

   bool m_updatePending;

public:
	CDataDoctorParallelAnalysisPropertyPage(CDataDoctorPropertySheet& parent);
	virtual ~CDataDoctorParallelAnalysisPropertyPage();

   CDataDoctorPropertySheet& getParent() { return m_parent; }

// Dialog Data
	enum { IDD = IDD_DataDoctorParallelAnalysisPage };

   void QueueUpdate()      { m_updatePending = true; }
   bool IsUpdatePending()  { return m_updatePending; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnBnClickedProcess();
   afx_msg void OnBnClickedClearResults();
   virtual BOOL OnSetActive();
};



#endif