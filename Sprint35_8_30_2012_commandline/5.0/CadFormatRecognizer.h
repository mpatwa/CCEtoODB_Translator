
#if !defined(__CadFormatRecognizer_h__)
#define __CadFormatRecognizer_h__

#pragma once

#include "TypedContainer.h"

enum FileTypeTag;

class CCadFormatRecognizer;

//_________________________________________________________________________________________________
class CCadFormatAcceptor
{
private:
   CCadFormatRecognizer& m_cadFormatRecognizer;

public:
   CCadFormatAcceptor(CCadFormatRecognizer& cadFormatRecognizer);
   virtual ~CCadFormatAcceptor();

   CCadFormatRecognizer& getCadFormatRecognizer();

   virtual FileTypeTag getFileType() const;           // Overall file type, like class of file, eg SchematicNetlist
   virtual FileTypeTag getSpecificFileType() const;   // Like subtype, specific file type, e.g. VB Keyin netlist, Boardstation netlist
   virtual bool accepts(const CString& filePath);
   virtual bool accepts(const CString& filePath,CStdioFile* file);
   virtual int getAssociateFileList(const CString& filePath, CStringArray &filenameList);
};

//_________________________________________________________________________________________________
class CCadFormatRecognizer
{
private:
   CTypedPtrArrayContainer<CCadFormatAcceptor*> m_acceptors;
   CStringArray m_filePaths;
   bool m_directoryFlag;

public:
   CCadFormatRecognizer();
   virtual ~CCadFormatRecognizer();

   bool getDirectoryFlag() const;
   void setDirectoryFlag(bool flag);

   const CStringArray& getFilePaths() const;
   CStringArray& getFilePaths();

   FileTypeTag determineFileType(const CString& filePath);

   CCadFormatAcceptor* getCadFormatAcceptor(FileTypeTag fileType);
   int getAssociateFileList(FileTypeTag filetype, CString& filePath, CStringArray &filenameList);
};

//_________________________________________________________________________________________________
class CCadFormatAcceptorCr5000Layout : public CCadFormatAcceptor
{
public:
   CCadFormatAcceptorCr5000Layout(CCadFormatRecognizer& cadFormatRecognizer);
   virtual ~CCadFormatAcceptorCr5000Layout();

   virtual FileTypeTag getFileType() const;
   virtual bool accepts(const CString& filePath,CStdioFile* file);
};

//_________________________________________________________________________________________________
class CCadFormatAcceptorUnidat : public CCadFormatAcceptor
{
public:
   CCadFormatAcceptorUnidat(CCadFormatRecognizer& cadFormatRecognizer);
   virtual ~CCadFormatAcceptorUnidat();

   virtual FileTypeTag getFileType() const;
   virtual bool accepts(const CString& filePath,CStdioFile* file);
};

//_________________________________________________________________________________________________
enum Mentor_FileFormat
{
   //mentor layout
   MENFORMAT_LAYER,
   MENFORMAT_GEOM,
   MENFORMAT_GEOM1,
   MENFORMAT_TECHNOLOGY,       
   MENFORMAT_COMP,            
   MENFORMAT_WIRE,             
   MENFORMAT_NETLIST,          
   MENFORMAT_PACKAGE,          
   MENFORMAT_PINS,
   MENFORMAT_TESTPOINTS, 
   //neutral layout
   MENFORMAT_NEUTRAL,         
   MENFORMAT_MAXNUM,
};
#define MAXLINE_LIMIT   30

class CCadFormatAcceptorMentorLayout : public CCadFormatAcceptor
{
private:
   FileTypeTag m_FileType;
   CStringArray m_formatArray[MENFORMAT_MAXNUM];

private:
   int NormalizedFind(CString source, CString dest);

public:
   CCadFormatAcceptorMentorLayout(CCadFormatRecognizer& cadFormatRecognizer, FileTypeTag fileType);
   virtual ~CCadFormatAcceptorMentorLayout();

   virtual FileTypeTag getFileType() const;
   virtual bool accepts(const CString& filePath,CStdioFile* file);
   virtual int getAssociateFileList(const CString& filePath, CStringArray &fileNames);
};

//_________________________________________________________________________________________________

#define MaxAeroflexNailFileNum  2
class CCadFormatAcceptorAeroflexNail : public CCadFormatAcceptor
{
private:
   FileTypeTag m_FileType;
   CStringArray m_formatArray[MaxAeroflexNailFileNum];

private:
   int getFileCommandIndex(FileTypeTag fileType);

public:
   CCadFormatAcceptorAeroflexNail(CCadFormatRecognizer& cadFormatRecognizer, FileTypeTag fileType);
   virtual ~CCadFormatAcceptorAeroflexNail();

   virtual FileTypeTag getFileType() const;
   virtual bool accepts(const CString& filePath,CStdioFile* file);
};

//_________________________________________________________________________________________________

class CCadFormatAcceptorSchematicNetlist : public CCadFormatAcceptor
{
private:
   FileTypeTag m_FileType;            // Overall file type, class of fileTypeSchematicNetlist
   FileTypeTag m_specificFileType;    // Specific file type, e.g. VB Keyin netlist, Boardstation netlist, etc.

public:
   CCadFormatAcceptorSchematicNetlist(CCadFormatRecognizer& cadFormatRecognizer, FileTypeTag fileType);
   virtual ~CCadFormatAcceptorSchematicNetlist();

   virtual FileTypeTag getFileType() const;
   virtual FileTypeTag getSpecificFileType() const;
   virtual bool accepts(const CString& filePath,CStdioFile* file);
   virtual int getAssociateFileList(const CString& filePath, CStringArray &fileNames);
};



//_________________________________________________________________________________________________
//---------------------------------------------------------------

class CFileBufferStringArray : public CStringArray
{
public:
   // Finds return index of item or -1 if not found
   bool Contains(CString findme);       // Case sensitive find, true or false, does any line in array contain this substring
   bool ContainsNoCase(CString findme);

   bool ContainsLeftJustified(CString findme);        // String find me must be at start of line
   bool ContainsNoCaseLeftJustified(CString findme);  
};

//---------------------------------------------------------------
//_________________________________________________________________________________________________


#endif
