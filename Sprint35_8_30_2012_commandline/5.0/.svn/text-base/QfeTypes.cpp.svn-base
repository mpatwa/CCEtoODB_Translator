// $Header: /CAMCAD/5.0/QfeTypes.cpp 3     3/12/07 12:44p Kurt Van Ness $

#include "StdAfx.h"
#include "QfeTypes.h"
#include "DcaData.h"

//_____________________________________________________________________________
CDataStructQfe::CDataStructQfe(const CPoint2d& origin,DataStruct* dataStruct) :
   CTypedQfe<DataStruct>(origin,dataStruct)
{
}

CDataStructQfe::~CDataStructQfe()
{
}

//_____________________________________________________________________________
CDataStructQfeTree::CDataStructQfeTree(bool isContainer) :
   CDerivedQfeTree<CDataStructQfe>(isContainer)
{
}
