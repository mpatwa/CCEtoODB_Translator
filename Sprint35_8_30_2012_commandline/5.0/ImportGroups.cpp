// $Header: /CAMCAD/4.5/ImportGroups.cpp 6     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "TypedContainer.h"
#include "DbUtil.h"
#include "importgroups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////
// CImportGroup
////////////////////////////////////////////////////////////////////////////////
CImportGroup::CImportGroup(FileTypeTag fileType)
{
	m_eFileType = fileType;
	m_fileNameMap.RemoveAll();
}

CImportGroup::~CImportGroup()
{
}

FileTypeTag CImportGroup::GetFileType()
{
	return m_eFileType;	
}

void CImportGroup::AddFileName(CString fileName)
{
	if (!m_fileNameMap.Lookup(fileName, fileName))
		m_fileNameMap.SetAt(fileName, fileName);
}

void CImportGroup::ImportFiles()
{
}


////////////////////////////////////////////////////////////////////////////////
// CImportGroups
////////////////////////////////////////////////////////////////////////////////
CImportGroups::CImportGroups()
{
	this->empty();
	this->SetSize(fileTypeUpperBound + 1);
}

CImportGroups::~CImportGroups()
{
	this->empty();
}

CImportGroup* CImportGroups::getImportGroup(FileTypeTag fileType)
{
	CImportGroup* importGroup = this->GetAt(fileType);
	if (importGroup != NULL)
		return importGroup;

	importGroup = new CImportGroup(fileType);
	this->setAt(fileType, importGroup);
	return importGroup;
}

void CImportGroups::AddFileName(FileTypeTag fileType, CString fileName)
{
	getImportGroup(fileType)->AddFileName(fileName);
}

void CImportGroups::ImportFiles()
{
	for (int i=0; i<this->GetCount(); i++)
	{
		CImportGroup* importGroup = this->GetAt(i);
		if (importGroup == NULL)
			continue;

		importGroup->ImportFiles();
	}
}