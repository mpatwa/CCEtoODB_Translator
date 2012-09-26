// $Header: /CAMCAD/DcaLib/DcaFormatStdioFile.cpp 1     3/09/07 5:10p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaFormatStdioFile.h"

//_____________________________________________________________________________
CFormatStdioFile::CFormatStdioFile()
: CStdioFile()
{
   fileOpen = false;
}

CFormatStdioFile::~CFormatStdioFile()
{
   fileOpen = false;
}

int CFormatStdioFile::GetLinePosition() const
{
   return m_iLineCnt;
}

BOOL CFormatStdioFile::Open(LPCTSTR fileName, UINT openFlags,CFileException* pError)
{
	fileOpen = (CStdioFile::Open(fileName, openFlags, pError))?true:false;
	m_iLineCnt = 0;

	return fileOpen;
}

void CFormatStdioFile::Close()
{
	if (!fileOpen)
		return;

	CStdioFile::Close();

	fileOpen = false;
}

void CFormatStdioFile::SeekToBegin()
{
	CStdioFile::SeekToBegin();
	m_iLineCnt = 0;
}

bool CFormatStdioFile::WriteString(LPCTSTR format, ...)
{
	if (!fileOpen)
		return false;

	CString buf;
	va_list args;
	va_start(args, format);

	buf.FormatV(format, args);

	CStdioFile::WriteString(buf);

	return true;
}

BOOL CFormatStdioFile::ReadString(CString& rString)
{
	m_iLineCnt++;

	return CStdioFile::ReadString(rString);
}
