// $Header: /CAMCAD/4.5/read_wrt/Orbotech_Out.h 10    11/15/05 2:20p Moustafa Noureddine $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

#include "ccdoc.h"
#include "General.h"
#define DEFAULT_ORBOTECH_UNITS	unit_o_Mils;

enum EOrbotechUnits
{
	unitUnknown = -1,
	unit_o_Mils = 0,		// thousandths of an inch
	unit_o_MM = 1,	// millemeter
};

class OrbotechWrite
{
public:

	OrbotechWrite(CCEtoODBDoc *document, double l_Scale);
	OrbotechWrite();
	BOOL Write(CString newTopFile, CString newBotFile);
	int LoadSettings(CString settingsFile);
	int ProcessPanel();
	CString m_PackageName;
	int intFidID;
private:
	EOrbotechUnits o_Units;
	CFormatStdioFile topFile;
	CFormatStdioFile botFile;
	CCEtoODBDoc *m_pDoc;
	double m_dUnitFactor;
	double p_Scale;
	CString exportUNLoaded;
   bool     exportThroughHolePins;
   bool     exportThru;
   double m_originX;
	double m_originY;
	   
};

