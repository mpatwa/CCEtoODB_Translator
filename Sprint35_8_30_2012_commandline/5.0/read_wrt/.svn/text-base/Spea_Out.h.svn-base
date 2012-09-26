// $Header: /CAMCAD/4.5/read_wrt/Spea_Out.h 10    12/21/05 4:01p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once


/* Define Section *********************************************************/

// These are contact type codes (CDC) for test point section in SPEA output
#define CDC_THROUGH_HOLE	1
#define CDC_SMD				2
#define CDC_PAD				3
#define CDC_VIA				4
#define CDC_CONNECTOR		8
#define CDC_TESTPOINT		10


enum EExportTracesFlag
{
	eExportTracesNone,
	eExportTracesAll,
	eExportTracesOuter
};

// Part number and Package name used for testpoint related output when
// export-testpoints-as-components option is on/yes/true/go.
#define TESTPOINT_PARTNUMBER   "PNTEST"
#define TESTPOINT_PACKAGE_NAME "PKTEST"

/* Structures Section *********************************************************/

class SpeaPNData
{
public:
	CString	partNumber;
   CString  packageName;
	CString	deviceTypeCode;
	CString	deviceName;
	CString	value;
	CString  tolPos;
	CString  tolNeg;
	int		caseCode;
	double	compXsize;
	double	compYsize;

   SpeaPNData();
   SpeaPNData(CString partnum, CString pkgname, CString devcode, CString devname, CString value, CString plustol, CString minustol, 
      int casecode, double xsize, double ysize);
};


class SpeaUniqueBoard
{
public:
   int blockNumber;
   int mirror;
};
typedef CTypedPtrArray<CPtrArray, SpeaUniqueBoard*> SpeaUniqueBoardArray;


class SpeaTestpoint
{
public:
   int m_testpointNum;           // Test Point Number
   CString m_speaTestpointName;  // Test Point Name for SPEA output
   int m_accessMarkEntityNum;    // Refname of entity that lead to test point (usually access marker or test probe)
   CString m_netName;            // Net Name
   CString m_targetRefname;      // Drawing Reference, Refname of testpoint (access mark) target entity (i.e. component refname if comppin, via refname, etc.).
   CString m_targetPinName;      // Pin Name
   CString m_testpointCode;      // Test Point Type Code
   int m_contactCode;            // Contact Type Code
   double m_testAccessMarkX;     // X Position
   double m_testAccessMarkY;     // Y Position
   char m_contactSide;           // Contact Side
   int m_padCode;                // Pad Code aka blocknumber of padstack that is inserted for pin.

   SpeaTestpoint(int num, CString speaName, int accessEntityNum, CString netName, CString targetRefname, CString targetPinName, CString code,
      int contactCode, double x, double y, char contactSide, int padCode)
   {
      m_testpointNum = num;
      m_speaTestpointName = speaName;
      m_accessMarkEntityNum = accessEntityNum;
      m_netName = netName;
      m_targetRefname = targetRefname;
      m_targetPinName = targetPinName;
      m_testpointCode = code;
      m_contactCode = contactCode;
      m_testAccessMarkX = x;
      m_testAccessMarkY = y;
      m_contactSide = contactSide;
      m_padCode = padCode;
   }
};
typedef CTypedPtrArrayContainer<SpeaTestpoint*> SpeaTestpointArray;


class SpeaPart
{
public:
   InsertTypeTag m_insertType;  // Could be pcb component, fiducial, via.
   CString m_partNumber;
   int m_variantNumber;
   CString m_refname;
   double m_x;
   double m_y;
   char m_mountSide;
   int m_rotation;

   SpeaPart(InsertTypeTag insertType, CString partNumber, int variantNumber, CString refname, double x, double y, char mountSide, int rotation)
   {
      m_insertType = insertType;
      m_partNumber = partNumber;
      m_variantNumber = variantNumber;
      m_refname = refname;
      m_x = x;
      m_y = y;
      m_mountSide = mountSide;
      m_rotation = rotation;
   }
};
typedef CTypedPtrArrayContainer<SpeaPart*> SpeaPartArray;




// CSpeaOutProbeOffsetDlg dialog

class CSpeaOutProbeOffsetDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpeaOutProbeOffsetDlg)

public:
	CSpeaOutProbeOffsetDlg(int defaultOffset, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpeaOutProbeOffsetDlg();
	int getProbeNumberOffset()	{ return m_probeNumberOffset; }

// Dialog Data
	enum { IDD = IDD_SPEA_OUT_PROBE_OFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int m_probeNumberOffset;  // the user changes this
	int m_minimumOffset;      // the user does not change this
};
