// $Header: /CAMCAD/4.5/read_wrt/Testway_in.h 6     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#ifndef __TESTWAY_IN__
#define __TESTWAY_IN__

#include "GeneralDialog.h"

class CTestPlan;

class CTestWayReader
{
public:
	CTestWayReader(CTestPlan *testPlan, bool caseSensitive = true)				{ m_pTestPlan = testPlan; m_bCaseSensitive = caseSensitive;		};
	~CTestWayReader() {};

private:
	CTestPlan *m_pTestPlan;
	bool m_bCaseSensitive;
	CString m_sFilename;
	CFormatStdioFile m_fFile;
	CFormatStdioFile m_fLog;

public:
	bool ReadFile(CString fileName);
};

#endif // __TESTWAY_IN__
