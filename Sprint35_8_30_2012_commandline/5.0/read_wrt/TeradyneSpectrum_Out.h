// $Header: /CAMCAD/4.5/read_wrt/TeradyneSpectrum_Out.h 13    11/07/06 12:54p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2003. All Rights Reserved.
*/

#if !defined(__TeradyneSpectrum_Out_h__)
#define __TeradyneSpectrum_Out_h__

#pragma once

#include "Ipl_out.h"

class CSpectrumWriterVia;
class CNetChannel;

enum ECHFormat
{
	CH_NetName,				// Channel name is "CH_" + netname
	CH_ChennelNumber,		// Channel name is "CH_" + channelNumber;
};

/******************************************************************************
* CCleanedNetNameMap
*/
class CCleanedNetNameMap : public CMapStringToString
{
private:
   CString polishNetname(CString camcadName);
   CCEtoODBDoc *m_pDoc;

public:
   int CollectNames(CNetList& netlist);
   CCleanedNetNameMap(CCEtoODBDoc* doc);
   void WriteReport(FILE *logFile);
};

//---------------------------------------------------------------------

class CTeradyneSpectrumWriter : public CIplWriter
{
public:
	CTeradyneSpectrumWriter(CCEtoODBDoc* pDoc, CString outfilename);
	~CTeradyneSpectrumWriter();

private:
   CString m_inductorResValAttrName;
   CString m_inductorResTolAttrName;
   bool m_sharedIplInitSucceeded;
	bool m_bExportVias;
	ECHFormat m_eCHFormat;
	int m_sensorNumber;
   CCleanedNetNameMap m_netNameMap; // A tool to process the case sensitive problem

   CMapSortedStringToOb<CSpectrumWriterVia> m_sortedVias;
   void CollectSortedVias();

	void initializeWriter();
	int readSettingsFile(CString filename);

	// CMAP.txt file
	int writeCMAP(CString filepath);
	void writeCMapNets(CFormatStdioFile* file, CMapSortedStringToOb<CNetChannel> &channelNameMap);
	void writeCMapNets5210(CFormatStdioFile* file, CMapSortedStringToOb<CNetChannel> &channelNameMap);
	void writeCMapChannels(CFormatStdioFile* file, CMapSortedStringToOb<CNetChannel> &channelNameMap);

	// IPL.DAT file
	int writeIPL(CString filepath);
	void writeIplComponents(CFormatStdioFile* file);
	void writeIplVias(CFormatStdioFile* file);
	CString getIplString(CIplComponent* iplComp);

	// Nets.asc file
	int writeNets(CString filepath);

	// Parts.asc file
	int writeParts(CString filepath);
	void writePartsComponents(CFormatStdioFile* file);
	void writePartsVias(CFormatStdioFile* file);

	// Pins.asc file
	int writePins(CString filepath);
	void writePinsComponents(CFormatStdioFile* file);
	void writePinsVias(CFormatStdioFile* file);

	// Tstpart.asc file
	int writeTstparts(CString filepath);
	void writeTstpartsComponents(CFormatStdioFile* file);
	void writeTstpartsVias(CFormatStdioFile* file);

	// Testconnect.txt file
	int writeTestConnect(CString filepath);

	// Name adjusters, replace illegal characters
	CString cleanNetname(CString camcadName);
	CString cleanSubclass(CString camcadName);
	CString cleanRefdes(CString camcadName);

   // Value adjuster
   CString trimValue(double val);

public:
	int WriteFiles(CString filepath);

};

//---------------------------------------------------------------------

class CSpectrumWriterVia : public CObject
{
private:
   CString m_sortableRefname;
   CString m_refname;
   double m_x;
   double m_y;
   double m_theta;  // Degrees
   bool m_topside;
   bool m_isSMD;
   CString m_subclass;
   CString m_blockname;
   CString m_netname;
   int m_channelnumber;
   CString m_gridlocation;

public:
   CSpectrumWriterVia(CString sortableRefname, CString refname, CString gridloc, double x, double y, double thetaDegrees, bool topside, bool isSMD, CString subclass, CString blockname, CString netname, int channelnumber)
   {
      m_sortableRefname = sortableRefname;
      m_refname = refname;
      m_gridlocation = gridloc;
      m_x = x;
      m_y = y;
      m_theta = thetaDegrees;
      m_topside = topside;
      m_isSMD = isSMD;
      m_subclass = subclass;
      m_blockname = blockname;
      m_netname = netname;
      m_channelnumber = channelnumber;
   }

   CString getSortableRefname()  { return m_sortableRefname; }
   CString getRefname()       { return m_refname; }
   CString getGridLocation()  { return m_gridlocation; }
   double getX()              { return m_x; }
   double getY()              { return m_y; }
   double getTheta()          { return m_theta; }
   bool isTopSide()           { return m_topside; }
   bool isSMD()               { return m_isSMD; }
   CString getSubclass()      { return m_subclass; }
   CString getBlockname()     { return m_blockname; }
   CString getNetname()       { return m_netname; }
   int getChannelNumber()     { return m_channelnumber; }

   static int AscendingRefnameSortFunc(const void *a, const void *b);

};

class CNetChannel : public CObject
{
private:
   CString m_netname;
   CString m_channelnumber;

public:
   CNetChannel(CString netname, CString channelnumber) { m_netname = netname; m_channelnumber = channelnumber; }

   CString getNetName()       { return m_netname; }
   CString getChannelNumber() { return m_channelnumber; }

   static int AscendingNetNameSortFunc(const void *a, const void *b);

};


#endif
