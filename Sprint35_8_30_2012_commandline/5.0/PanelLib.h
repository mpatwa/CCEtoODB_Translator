// $Header: /CAMCAD/4.5/PanelLib.h 4     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#pragma once

#ifndef __PANELLIB__
#define __PANELLIB__

#include "data.h"

class FileStruct;

enum EPanelSortAlg
{
	panelSortAlgSerpentineLike = 0,
	panelSortAlgProgressiveRaster = 1,
};

enum EPanelSortStartLocation
{
	panelSortStartLocationBotLt = 0,
	panelSortStartLocationTopLt = 1,
	panelSortStartLocationBotRt = 2,
	panelSortStartLocationTopRt = 3,
};

class CBoardCollectionItem : public CObject
{
public:
	CBoardCollectionItem(DataStruct &boardData, CCEtoODBDoc &doc);
	~CBoardCollectionItem();

private:
	CPoint2d m_ptCenter;
	DataStruct *m_pBoardData;

public:
	CPoint2d GetCenter() const			{ return m_ptCenter; };
	DataStruct *GetData() const		{ return m_pBoardData; };
};

class CBoardCollection : public CTypedPtrArrayContainer<CBoardCollectionItem*>
{
public:
	CBoardCollection(bool useAsRows);
	~CBoardCollection();

	static int DescendingCompareByRow(const void* elem1,const void* elem2);
	static int DescendingCompareByCol(const void* elem1,const void* elem2);
	static int AscendingCompareByRow(const void* elem1,const void* elem2);
	static int AscendingCompareByCol(const void* elem1,const void* elem2);

private:
	bool m_bUseAsRows;
	double m_dMax;
	double m_dMin;
	double m_dToleranceDistance;

public:
	double GetMin() const				{ return m_dMin; };
	double GetMax() const				{ return m_dMax; };
	double GetTolerance() const		{ return m_dToleranceDistance; };
	double GetRangeCenter() const		{ return (m_dMax + m_dMin) / 2.; };

	void SetTolerance(double tolerance);
	bool Add(DataStruct &boardInsert, CCEtoODBDoc &doc);
	bool Sort(bool ascending = true, bool usingRows = true);
};

class CBoardCollectionArray : public CTypedPtrArrayContainer<CBoardCollection*>
{
public:
	CBoardCollectionArray();
	~CBoardCollectionArray() {};

	static int DescendingCompare(const void* elem1,const void* elem2);
	static int AscendingCompare(const void* elem1,const void* elem2);

private:
	bool m_bUseAsRows;
	int m_iTolerancePercent;

public:
	bool Add(DataStruct &board, CCEtoODBDoc &doc);
	int SetTolerancePercent(int tolerancePercent);
	bool Sort(EPanelSortAlg sortAlg, EPanelSortStartLocation startLocation);
};

class CSortPanel
{
public:
	CSortPanel(CCEtoODBDoc &doc, FileStruct *file = NULL, int tolerancePercent = 25);
	~CSortPanel();

private:
	bool m_bValid;
	bool m_bSorted;
	EPanelSortAlg m_eSortAlg;
	EPanelSortStartLocation m_eStartLocation;

	CCEtoODBDoc *m_pDoc;
	FileStruct *m_pPanelFile;
	CBoardCollectionArray m_arBoards;

	void fillBoardArray();

public:
	bool IsValid() const					{ return m_bValid; };
	bool IsSorted() const				{ return m_bSorted; };

	EPanelSortAlg SetSortAlg(EPanelSortAlg sortAlg);
	EPanelSortStartLocation SetStartSortLocation(EPanelSortStartLocation startLocation);
	bool SetPanel(FileStruct *file);
	void SetTolerancePercent(int tolerancePercent);

	bool Sort();
	bool ApplyToPanelFile();

	void Dump(CWriteFormat &file);
};

#endif // __PANELLIB__
