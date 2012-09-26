// $Header: /CAMCAD/4.5/PanelLib.cpp 4     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "panellib.h"
#include "ccdoc.h"
#include "file.h"
#include "writeformat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString GetSortAlgString(EPanelSortAlg sortAlg)
{
	switch (sortAlg)
	{
	case panelSortAlgSerpentineLike:
		return "Serpentine Like";
	case panelSortAlgProgressiveRaster:
		return "Progressive Raster";
	default:
		return "Unknown or Unset";
	}
}

CString GetStartLocationString(EPanelSortStartLocation startLocation)
{
	switch (startLocation)
	{
	case panelSortStartLocationBotLt:
		return "Bottom Left";
	case panelSortStartLocationTopLt:
		return "Top Left";
	case panelSortStartLocationBotRt:
		return "Bottom Right";
	case panelSortStartLocationTopRt:
		return "Top Right";
	default:
		return "Default (Bottom Left)";
	}
}



/******************************************************************************
* CBoardCollectionItem::CBoardCollectionItem
*/
CBoardCollectionItem::CBoardCollectionItem(DataStruct &boardData, CCEtoODBDoc &doc)
{
	m_pBoardData = &boardData;

	BlockStruct *block = doc.getBlockAt(boardData.getInsert()->getBlockNumber());
	if (block == NULL)
		return;
	CExtent ext = block->getExtent();
	CPoint2d center = ext.getCenter();

	CTMatrix mat;
	mat.scale(boardData.getInsert()->getScale() * boardData.getInsert()->getGraphicMirrored()?-1:1, boardData.getInsert()->getScale());
	mat.rotateRadians(boardData.getInsert()->getAngle());
	mat.translate(boardData.getInsert()->getOrigin2d());
	mat.transform(center);

	m_ptCenter = center;
}

CBoardCollectionItem::~CBoardCollectionItem()
{
}



/******************************************************************************
* CBoardCollection::CBoardCollection
*/
CBoardCollection::CBoardCollection(bool useAsRows) : CTypedPtrArrayContainer<CBoardCollectionItem*>()
{
	m_bUseAsRows = useAsRows;
	m_dMax = -DBL_MAX;
	m_dMin = DBL_MAX;
	m_dToleranceDistance = 0.;
}

CBoardCollection::~CBoardCollection()
{
}

int CBoardCollection::DescendingCompareByRow(const void* elem1,const void* elem2)
{
	const CBoardCollectionItem *item1 = *(const CBoardCollectionItem**)elem1;
	const CBoardCollectionItem *item2 = *(const CBoardCollectionItem**)elem2;

	if (item1->GetCenter().x < item2->GetCenter().x)
		return 1;
	else if (item1->GetCenter().x > item2->GetCenter().x)
		return -1;
	else
		return 0;
}

int CBoardCollection::DescendingCompareByCol(const void* elem1,const void* elem2)
{
	const CBoardCollectionItem *item1 = *(const CBoardCollectionItem**)elem1;
	const CBoardCollectionItem *item2 = *(const CBoardCollectionItem**)elem2;

	if (item1->GetCenter().y < item2->GetCenter().y)
		return 1;
	else if (item1->GetCenter().y > item2->GetCenter().y)
		return -1;
	else
		return 0;
}

int CBoardCollection::AscendingCompareByRow(const void* elem1,const void* elem2)
{
	const CBoardCollectionItem *item1 = *(const CBoardCollectionItem**)elem1;
	const CBoardCollectionItem *item2 = *(const CBoardCollectionItem**)elem2;

	if (item1->GetCenter().x < item2->GetCenter().x)
		return -1;
	else if (item1->GetCenter().x > item2->GetCenter().x)
		return 1;
	else
		return 0;
}

int CBoardCollection::AscendingCompareByCol(const void* elem1,const void* elem2)
{
	const CBoardCollectionItem *item1 = *(const CBoardCollectionItem**)elem1;
	const CBoardCollectionItem *item2 = *(const CBoardCollectionItem**)elem2;

	if (item1->GetCenter().y < item2->GetCenter().y)
		return -1;
	else if (item1->GetCenter().y > item2->GetCenter().y)
		return 1;
	else
		return 0;
}

void CBoardCollection::SetTolerance(double tolerance)
{
	if (tolerance < 0)
		return;

	m_dToleranceDistance = tolerance;
}

bool CBoardCollection::Add(DataStruct &boardInsert, CCEtoODBDoc &doc)
{
	if (boardInsert.getDataType() != dataTypeInsert)
		return false;

	if (boardInsert.getInsert()->getInsertType() != insertTypePcb)
		return false;

	CBoardCollectionItem brdItem(boardInsert, doc);
	double value = 0.;
	if (m_bUseAsRows)
		value = brdItem.GetCenter().y;
	else
		value = brdItem.GetCenter().x;

	if (GetCount() > 0)
	{
		if (m_dMin + m_dToleranceDistance < value || m_dMax - m_dToleranceDistance > value)
			return false;
	}

	if (m_dMax < value)
		m_dMax = value;
	if (m_dMin > value)
		m_dMin = value;

	SetAtGrow(GetCount(), new CBoardCollectionItem(boardInsert, doc));

	return true;
}

bool CBoardCollection::Sort(bool ascending, bool usingRows)
{
	if (ascending && usingRows)
		setSortFunction(CBoardCollection::AscendingCompareByRow);
	else if (ascending && !usingRows)
		setSortFunction(CBoardCollection::AscendingCompareByCol);
	else if (!ascending && usingRows)
		setSortFunction(CBoardCollection::DescendingCompareByRow);
	else if (!ascending && !usingRows)
		setSortFunction(CBoardCollection::DescendingCompareByCol);

	sort();

	return false;
}



/******************************************************************************
* CBoardCollectionArray::Add
*/
CBoardCollectionArray::CBoardCollectionArray() : CTypedPtrArrayContainer<CBoardCollection*>()
{
	m_bUseAsRows = true;
}

int CBoardCollectionArray::DescendingCompare(const void* elem1,const void* elem2)
{
	const CBoardCollection *brdColl1 = *(const CBoardCollection**)elem1;
	const CBoardCollection *brdColl2 = *(const CBoardCollection**)elem2;

	if (brdColl1->GetRangeCenter() < brdColl2->GetRangeCenter())
		return 1;
	else if (brdColl1->GetRangeCenter() > brdColl2->GetRangeCenter())
		return -1;
	else
		return 0;
}

int CBoardCollectionArray::AscendingCompare(const void* elem1,const void* elem2)
{
	const CBoardCollection *brdColl1 = *(const CBoardCollection**)elem1;
	const CBoardCollection *brdColl2 = *(const CBoardCollection**)elem2;

	if (brdColl1->GetRangeCenter() < brdColl2->GetRangeCenter())
		return -1;
	else if (brdColl1->GetRangeCenter() > brdColl2->GetRangeCenter())
		return 1;
	else
		return 0;
}

bool CBoardCollectionArray::Add(DataStruct &board, CCEtoODBDoc &doc)
{
	// look to see if this data will fit
	for (int i=0; i<GetCount(); i++)
	{
		if (GetAt(i)->Add(board, doc))
			return true;
	}

	CBoardCollection *brdColl = new CBoardCollection(m_bUseAsRows);
	BlockStruct *block = doc.getBlockAt(board.getInsert()->getBlockNumber());
	if (block == NULL)
		return false;
	CExtent ext = block->getExtent();

	double toleranceDistance = 0.;
	if (m_bUseAsRows)
		toleranceDistance = ext.getYsize() * ((double)m_iTolerancePercent / 100.);
	else
		toleranceDistance = ext.getXsize() * ((double)m_iTolerancePercent / 100.);
	brdColl->SetTolerance(toleranceDistance);

	if (brdColl->Add(board, doc))
		CTypedArrayContainer<CPtrArray, CBoardCollection*>::Add(brdColl);
	else
		return false;

	return true;
}

int CBoardCollectionArray::SetTolerancePercent(int tolerancePercent)
{
	if (tolerancePercent < 0 || tolerancePercent > 100)
		return m_iTolerancePercent;

	int lastTolerancePercent = m_iTolerancePercent;
	m_iTolerancePercent = tolerancePercent;

	return lastTolerancePercent;
}

bool CBoardCollectionArray::Sort(EPanelSortAlg sortAlg, EPanelSortStartLocation startLocation)
{
	sort();

	bool sortAscending = true;

	if (startLocation == panelSortStartLocationBotRt || startLocation == panelSortStartLocationTopRt)
		sortAscending = false;

	// start from left to right
	for (int i=0; i<GetCount(); i++)
	{
		CBoardCollection *brdColl = GetAt(i);

		if (sortAlg == panelSortAlgSerpentineLike)
			brdColl->Sort(i%2?!sortAscending:sortAscending, m_bUseAsRows);
		else if (sortAlg == panelSortAlgProgressiveRaster)
			brdColl->Sort(sortAscending, m_bUseAsRows);
	}

	return true;
}



/******************************************************************************
* CSortPanel::CSortPanel
*/
CSortPanel::CSortPanel(CCEtoODBDoc &doc, FileStruct *file, int tolerancePercent)
{
	m_bValid = false;
	m_bSorted = false;

	m_eSortAlg = panelSortAlgSerpentineLike;
	m_eStartLocation = panelSortStartLocationBotLt;

	m_pDoc = &doc;
	m_pDoc->OnRegen();

	SetTolerancePercent(tolerancePercent);
	SetPanel(file);
}

CSortPanel::~CSortPanel()
{
}

void CSortPanel::fillBoardArray()
{
	m_arBoards.empty();

	BlockStruct *fileBlock = m_pPanelFile->getBlock();

	POSITION pos = fileBlock->getHeadDataInsertPosition();
	while (pos)
	{
		DataStruct *boardData = fileBlock->getNextDataInsert(pos);
		InsertStruct *boardInsert = boardData->getInsert();

		if (m_arBoards.Add(*boardData, *m_pDoc))
			m_bValid = true;
	}
}

EPanelSortAlg CSortPanel::SetSortAlg(EPanelSortAlg sortAlg)
{
	EPanelSortAlg lastSortAlg = m_eSortAlg;

	m_bSorted &= m_eSortAlg == sortAlg;
	m_eSortAlg = sortAlg;
	
	return lastSortAlg;
}

EPanelSortStartLocation CSortPanel::SetStartSortLocation(EPanelSortStartLocation startLocation)
{
	EPanelSortStartLocation lastStartLoc = m_eStartLocation;

	m_bSorted &= m_eStartLocation == startLocation;
	m_eStartLocation = startLocation;

	if (m_eStartLocation == panelSortStartLocationTopLt || m_eStartLocation == panelSortStartLocationTopRt)
		// need to sort desending
		m_arBoards.setSortFunction(CBoardCollectionArray::DescendingCompare);
	else
		// need to sort ascending
		m_arBoards.setSortFunction(CBoardCollectionArray::AscendingCompare);

	return lastStartLoc;
}

bool CSortPanel::SetPanel(FileStruct *panelFile)
{
	m_bValid = false;

	if (panelFile == NULL)
		return false;

	if (panelFile->getBlockType() != blockTypePanel)
		return false;

	m_pPanelFile = panelFile;
	fillBoardArray();

	return m_bValid;
}

void CSortPanel::SetTolerancePercent(int tolerancePercent)
{
	m_bSorted &= tolerancePercent == m_arBoards.SetTolerancePercent(tolerancePercent);
	m_bValid = m_bSorted;
}

bool CSortPanel::Sort()
{
	if (!m_bValid)
		fillBoardArray();

	if (!m_bSorted)
		m_arBoards.Sort(m_eSortAlg, m_eStartLocation);

	m_bSorted = true;

	/// USED FOR DEBUGGING PURPOSES ////////////////////////////////////////
	//CStdioFileWriteFormat file;
	//CExtFileException e;
	//if (file.open("C:\\Development\\CC Samples\\Motorola Panels\\PanelSort.txt", &e))
	//{
	//	Dump(file);
	//	file.close();
	//}
	////////////////////////////////////////////////////////////////////////

	return false;
}

bool CSortPanel::ApplyToPanelFile()
{
	if (!m_bSorted || !m_bValid)
		return false;

	//// remove all board inserts
	// copy all datas that are not pcb boards
	CDataList dataList(false);
	BlockStruct *fileBlock = m_pPanelFile->getBlock();
	POSITION pos = fileBlock->getHeadDataPosition();
	while (pos)
	{
		DataStruct *data = fileBlock->getNextData(pos);
		if (data->getDataType() == dataTypeInsert)
		{
			InsertStruct *insert = data->getInsert();
			if (insert->getInsertType() == insertTypePcb)
				continue;
		}

		dataList.AddTail(data);
	}
	fileBlock->getDataList().RemoveAll();

	// copy all datas that were not pcb boards
	pos = dataList.GetHeadPosition();
	while (pos)
		fileBlock->getDataList().AddTail(dataList.GetNext(pos));

	// add all the pbc boards in the correct order
	for (int collCnt=0; collCnt<m_arBoards.GetCount(); collCnt++)
	{
		CBoardCollection *brdColl = m_arBoards.GetAt(collCnt);
		for (int itemCnt=0; itemCnt<brdColl->GetCount(); itemCnt++)
		{
			CBoardCollectionItem *brdItem = brdColl->GetAt(itemCnt);

			fileBlock->getDataList().AddTail(brdItem->GetData());
		}
	}

	return true;
}

void CSortPanel::Dump(CWriteFormat &file)
{
	file.writef("Algorithm : %s\n", GetSortAlgString(m_eSortAlg));
	file.writef("Start Location : %s\n", GetStartLocationString(m_eStartLocation));
	file.pushHeader("   ");
	
	for (int collCnt=0; collCnt<m_arBoards.GetCount(); collCnt++)
	{
		CBoardCollection *brdColl = m_arBoards.GetAt(collCnt);

		file.writef("Row %d : \n", collCnt);
		file.pushHeader("   ");

		file.writef("Range : %.3f to %.3f w/ Tolerance of %.3f\n", brdColl->GetMax(), brdColl->GetMin(), brdColl->GetTolerance());
		file.pushHeader("   ");

		for (int itemCnt=0; itemCnt<brdColl->GetCount(); itemCnt++)
		{
			CBoardCollectionItem *boardData = brdColl->GetAt(itemCnt);

			file.writef("%s at (%.3f, %.3f)\n", boardData->GetData()->getInsert()->getRefname(), boardData->GetCenter().x, boardData->GetCenter().y);
		}
		file.writef("\n");

		file.popHeader();
		file.popHeader();
	}

	file.popHeader();
}
