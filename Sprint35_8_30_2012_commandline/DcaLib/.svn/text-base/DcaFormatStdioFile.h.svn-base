// $Header: /CAMCAD/DcaLib/DcaFormatStdioFile.h 1     3/09/07 5:10p Kurt Van Ness $

#if !defined(__DcaFormatStdioFile_h__)
#define __DcaFormatStdioFile_h__

#pragma once

//_____________________________________________________________________________
class CFormatStdioFile : public CStdioFile
{
public:
	CFormatStdioFile();
	~CFormatStdioFile();

private:
	bool fileOpen;
	int m_iLineCnt;

public:
	int GetLinePosition() const;

	virtual BOOL Open(LPCTSTR fileName, UINT openFlags,CFileException* pError = NULL);
	virtual void Close();

	void SeekToBegin();
	bool WriteString(LPCTSTR format, ...);
	virtual BOOL ReadString(CString& rString);
};

#endif
