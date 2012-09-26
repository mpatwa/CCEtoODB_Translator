// $Header: /CAMCAD/4.6/read_wrt/GerberReader.h 2     10/09/06 12:23p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2006. All Rights Reserved.
*/

#if !defined(__GerberReader_h__)
#define __GerberReader_h__

#pragma once

//#define ImplementNewGerberReader

#if defined(ImplementNewGerberReader)

#include "CamCadDatabase.h"

//_____________________________________________________________________________
class CGerberReader
{
private:
   CCamCadDatabase& m_camCadDatabase;


public:
   CGerberReader(CCamCadDatabase& camCadDatabase);

   bool read(const CString& filePath);
};

//_____________________________________________________________________________


#endif




#endif
