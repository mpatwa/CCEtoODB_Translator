// $Header: /CAMCAD/4.5/ImportGroups.h 5     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// CImportGroup
////////////////////////////////////////////////////////////////////////////////
class CImportGroup
{
public:
	CImportGroup(FileTypeTag fileType);
	~CImportGroup();

private:
	FileTypeTag m_eFileType;
	CMapStringToString m_fileNameMap;

public:
	FileTypeTag GetFileType();
	void AddFileName(CString fileName); 
	void ImportFiles();
};


////////////////////////////////////////////////////////////////////////////////
// CImportGroups
////////////////////////////////////////////////////////////////////////////////
class CImportGroups : private CTypedPtrArrayContainer<CImportGroup*>
{
public:
	CImportGroups();
	~CImportGroups();

private:
	CImportGroup* getImportGroup(FileTypeTag fileType);

public:
	void AddFileName(FileTypeTag fileType, CString fileName);
	void ImportFiles();
};

