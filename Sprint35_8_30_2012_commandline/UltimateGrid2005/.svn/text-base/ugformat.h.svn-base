// $Header: /CAMCAD/4.6/UltimateGrid2005/ugformat.h 1     5/08/06 8:28p Kurt Van Ness $

/*************************************************************************
				Class Declaration : CUGCellFormat
**************************************************************************
	Source file : ugformat.cpp
	Header file : ugformat.h
	Copyright © The Code Project 1994 - 2002, All Rights Reserved

	Purpose
		This class is used for the formating the 
		cells data for display and/or for editing.
*************************************************************************/
#ifndef _ugformat_H_
#define _ugformat_H_

class CUGCell;

class UG_CLASS_DECL CUGCellFormat: public CObject
{
public:
	CUGCellFormat();
	virtual ~CUGCellFormat();

	virtual void ApplyDisplayFormat(CUGCell *cell);
	
	virtual int  ValidateCellInfo(CUGCell *cell);
};

#endif	// _ugformat_H_