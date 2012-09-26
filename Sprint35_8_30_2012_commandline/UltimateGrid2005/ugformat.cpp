// $Header: /CAMCAD/4.6/UltimateGrid2005/ugformat.cpp 1     5/08/06 8:28p Kurt Van Ness $

/***********************************************
	Ultimate Grid
	Copyright 1994 - 2002 The Code Project


	class CUGFormat
************************************************/

#include "stdafx.h"

#include "UGCtrl.h"
//#include "UGFormat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/***********************************************
************************************************/
CUGCellFormat::CUGCellFormat(){
}

/***********************************************
************************************************/
CUGCellFormat::~CUGCellFormat(){
}

/***********************************************
************************************************/
void CUGCellFormat::ApplyDisplayFormat(CUGCell *cell){
	UNREFERENCED_PARAMETER(cell);
}
	
/***********************************************
return 
	0 - information valid
	1 - information invalid
************************************************/
int CUGCellFormat::ValidateCellInfo(CUGCell *cell){
	UNREFERENCED_PARAMETER(cell);
	return 0;
}
