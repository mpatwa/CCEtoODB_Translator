// $Header: /CAMCAD/4.5/read_wrt/RTI_Out.h 5     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"
#include "General.h"
#define DEFAULT_RTI_UNITS	unit_r_Inches;

enum EOrbotechUnits
{
	unitUnknown = -1,
	unit_r_Inches = 0,		// inch
	unit_r_Mils = 1,	// thousands of an inch
};

class RTIWrite
{
public:

	RTIWrite(CCEtoODBDoc *document, double l_Scale);
	RTIWrite();
	BOOL Write(CString newTopFile, CString newBotFile);
	int LoadSettings(CString settingsFile);
	
	int ProcessPanel();
private:
	EOrbotechUnits o_Units;
	CFormatStdioFile topFile;
	CFormatStdioFile botFile;
	CCEtoODBDoc *m_pDoc;
	double m_dUnitFactor;
	double p_Scale;
};

