// $Header: /CAMCAD/4.6/read_wrt/GerberReader.cpp 2     10/09/06 12:23p Kurt Van Ness $

/*****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2006. All Rights Reserved.
*/ 

#include "StdAfx.h"
#include "GerberReader.h"

#if defined(ImplementNewGerberReader)

void ReadGerberNew(const char* pathBuffer,CCAMCADDoc* doc, FormatStruct* Format,int pageUnits,int fileCount, int totalFileCount)
{ 
   CCamCadDatabase camCadDatabase(*doc);

   CGerberReader gerberReader(camCadDatabase);
   gerberReader.read(pathBuffer);


}

//_____________________________________________________________________________
CGerberReader::CGerberReader(CCamCadDatabase& camCadDatabase)
: m_camCadDatabase(camCadDatabase)
{
}

bool CGerberReader::read(const CString& filePathString)
{
   bool retval = true;

   CFilePath filePath(filePathString);
   CString fileName = filePath.getFileName();


   return retval;
}

#endif
