// $Header: /CAMCAD/5.0/QfeTypes.h 3     3/12/07 12:44p Kurt Van Ness $

/*****************************************************************************/
/*  
    Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2005. All Rights Reserved.
*/ 

#if !defined(__QfeTypes_h__)
#define __QfeTypes_h__

#pragma once

#include "QfeLib.h"
#include "Qfe.h"

class DataStruct;
class CPoint2d;

//_____________________________________________________________________________
class CDataStructQfe : public CTypedQfe<DataStruct>
{
public:
   CDataStructQfe(const CPoint2d& origin,DataStruct* dataStruct);
   virtual ~CDataStructQfe();

   DataStruct& getDataStruct() { return *getObject(); }
};

//_____________________________________________________________________________
class CDataStructQfeTree : public CDerivedQfeTree<CDataStructQfe>
{
public:
   CDataStructQfeTree(bool isContainer = true);
};

//_____________________________________________________________________________
class CDataStructQfeList : public CDerivedQfeList<CDataStructQfe>
{
};

#endif
