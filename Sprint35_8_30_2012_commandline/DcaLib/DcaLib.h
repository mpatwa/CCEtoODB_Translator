// $Header: /CAMCAD/DcaLib/DcaLib.h 6     6/21/07 8:29p Kurt Van Ness $

#if !defined(__DcaLib_h__)
#define __DcaLib_h__

#pragma once

int nextPrime2n(int targetPrime);
int DcaRound(double f);
const char* fpfmt(double number,int maxPrecision=4);
CString fpfmtExactPrecision(double number,int precision);
int formatMessageBox(const char* format,...);
int formatMessageBox(UINT flags,const char* format,...);

double normalizeDegrees(double degrees);
double normalizeDegrees(double degrees,double multiple);
int normalizeDegrees(int degrees);
double normalizeRadians(double radians);
bool radiansApproximatelyEqual(double radians0,double radians1,double tolerance);
CString quoteString(const CString& string);
CString quoteString(const CString& string,const CString& quoteTriggerList);
CString properCase(CString string);
int getFileSize(const CString& path);
bool isFolder(const CString& pathname);
bool fileExists(const CString& path);

//=============================================================================

/*
History: RwLib.h $
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 3/18/01    Time: 11:35a
 * Updated in $/LibKnvPP
 * Added support for default delimiter.
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 1/04/00    Time: 3:29a
 * Created in $/DocGen
 * Initial add
*/

/*
   paths ending with a delimiter are considered not to contain a fileName 
*/

//________________________________________________________________________________________
class CFilePath : public CObject
{
   //DECLARE_DYNAMIC(CFilePath);

private:
   // constructs
   mutable CString m_path;

   // components
   mutable CString m_driveLetter;
   mutable CString m_serverName;
   mutable CStringList m_directoryList;
   mutable CString m_baseFileName;
   mutable CString m_extension;

   // state
   mutable bool m_parsed;
   mutable bool m_built;
   bool m_delimiterSlash;
   bool m_beautify;
   char m_delimiter;

   // other
   static bool m_defaultDelimiterSlash;
   static const CString m_slash;
   static const CString m_backSlash;

public:
   CFilePath();
   CFilePath(const CString& path,bool beautify=true);
   CFilePath(const CString& path,char delimiter,bool beautify=true);
   CFilePath(const CFilePath& filePath);
   virtual ~CFilePath();
   CFilePath& operator=(const CFilePath& filePath);
   operator CString() { return getPath(); }
   void empty();

   // entire file path
   CString getPath() const; 

   // entire file path (short path form)
   CString getShortPath() const;   

   // drive letter and ':'
   CString getDrive() const;    

   // drive letter only, no ':'
   CString getDriveLetter() const;      

   // "//" or "\\" prefix and server name
   CString getServer() const;    

   // server name only, no "//" or "\\" prefix
   CString getServerName() const; 

   // server name or drive letter component, getServer(), or getDrive()
   CString getFoundation() const;

   // file base name and extension
   CString getFileName() const;  

   // file name only, no extension
   CString getBaseFileName() const; 

   // extension, no '.'
   CString getExtension() const; 

   // extension and '.'
   CString getFullExtension() const; 

   // entire path with last delimiter and file name truncated
   CString getDirectoryPath() const;  

   // only the immediate parent directory, no delimiters
   CString getDirectory() const;   

   // returns either "/" or "\"
   CString getDelimiter() const { return m_delimiterSlash ? m_slash : m_backSlash; }  

   int getDirectoryDepth() const;
   bool isAbsolutePath() const;
   bool isRelativePath() const { return ! isAbsolutePath(); }

   // returns top level directory name and removes this directory from the path.
   // "c:/Projects/Brick/ViewDraw.ini" returns "c:/", and leaves "Projects/Brick/ViewDraw.ini"
   // "/Projects/Brick/ViewDraw.ini" returns "/", and leaves "Projects/Brick/ViewDraw.ini"
   // "Projects/Brick/ViewDraw.ini" returns "Projects", and leaves "Brick/ViewDraw.ini"
   CString popRoot(); 
   void pushRoot(const CString& directoryName);  

   // returns bottom level file/directory name and removes this file/directory from the path.
   // "c:/Projects/Brick/ViewDraw.ini" returns "ViewDraw.ini", and leaves "c:/Projects/Brick"
   // "c:/Projects/Brick" returns "Brick", and leaves "c:/Projects"
   // "c:/Projects" returns "Projects", and leaves "c:/"
   // "c:/" returns "c:/", and leaves ""
   //
   // "/Projects/Brick/ViewDraw.ini" returns "ViewDraw.ini", and leaves "/Projects/Brick"
   // "/Projects/Brick" returns "Brick", and leaves "/Projects"
   // "/Projects" returns "Projects", and leaves "/"
   // "/" returns "/", and leaves ""
   //
   // "Projects/Brick/ViewDraw.ini" returns "ViewDraw.ini", and leaves "Projects/Brick"
   // "Projects/Brick" returns "Brick", and leaves "Projects"
   // "Projects" returns "Projects", and leaves ""
   // "" returns "", and leaves ""
   //
   CString popLeaf();
   void pushDirectory(const CString& directoryName);
   void pushLeaf(const CString& leafName);

   void setPath(const CString& filePath);
   void setPath(const CString& filePath,char delimiter);
   void setDrive(const CString& drive) { setDriveLetter(drive); };
   void setDriveLetter(const CString& driveLetter);
   void setServerName(CString serverName);
   void setFileName(const CString& fileName);
   void setBaseFileName(const CString& baseFileName);
   void setExtension(const CString& extension);
   void setFullExtension(const CString& fullExtension);
   void setDelimiter(const CString& delimiter);

   void beautify();
   static bool setDefaultDelimiterSlash(bool useSlash);
   void setDelimiterSlash();
   void setDelimiterBackSlash();

protected:
   void parse() const { parse(m_delimiter,m_delimiter); }
   void parse(const char delimit1,const char delimit2) const;
   void build() const;
   void beautifyWord(CString& word);
   void setDelimiterSlash(bool useSlash);

};

//=============================================================================

/*
History: RwLib.h $
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 6/12/99    Time: 8:56a
 * Updated in $/Include
 * Added non standard color value definitions.
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 4/22/98    Time: 11:43a
 * Updated in $/include
 * Added colorXxxxx constants
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 2/18/98    Time: 4:14p
 * Created in $/include
 * Initial add
*/

//=============================================================================

#define RecordTypeError      0
#define RecordTypeWarning    1
#define RecordTypeTask       2
#define RecordTypeDisaster   3
#define RecordTypeSummary    4
#define RecordTypeStatus     5
#define RecordTypeDebug      6
#define RecordTypeColorTest  7
#define RecordTypeOutput     8
#define RecordTypeSuccess    9
#define RecordTypeFailure   10
#define RecordTypeLastUsed  10

//=============================================================================

/*
History: RwLib.h $
 * 
 * *****************  Version 9  *****************
 * User: Kvanness     Date: 9/16/99    Time: 6:53a
 * Updated in $/LibKnvPP
 * Added comments.
 * 
 * *****************  Version 8  *****************
 * User: Kurtv        Date: 11/19/98   Time: 10:24a
 * Updated in $/include
 * Change BOOL to bool, where appropriate.
 * 
 * *****************  Version 7  *****************
 * User: Kurtv        Date: 4/22/98    Time: 12:00p
 * Updated in $/include
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 2/12/98    Time: 1:42p
 * Updated in $/include
 * Added ParseQuote()
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 9/22/97    Time: 7:41a
 * Updated in $/include
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 7/19/97    Time: 8:28p
 * Updated in $/include
 * Added various Parse() methods
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 2/27/97    Time: 5:27p
 * Updated in $/include
 * Changes for longer file names
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 1/25/96    Time: 9:11p
 * Updated in $/include
*/

//_____________________________________________________________________________
class CSupString : public CString
{
private:
   char m_singleCharacterEscape;

public:
// Constructors
   CSupString();
   CSupString(const CString& stringSrc);
   CSupString(char ch, int nRepeat = 1);
   CSupString(const char* psz);
   CSupString(const unsigned char* psz);
   CSupString(const char* pch, int nLength);

public:
   char getSingleCharacterEscape() const;
   char getDefinedSingleCharacterEscape() const;
   void setSingleCharacterEscape(char singleCharacterEscape);
   bool isSingleCharacterEscape(char c);

// Splits the string into 3 pieces at the first occurance of pszSplitString.
// The first piece is returned as the function value, the second piece (equal to pszSplitString)
// is discarded, the third piece becomes the value of this.
   CString Scan(const char* pszSplitString); 
   
// Splits the string into 3 pieces at the first occurance of pszSplitString.
// The first piece is returned as the function value, the second piece (equal to pszSplitString)
// is returned in brk, the third piece becomes the value of this.
   CString Scan(const char* pszSplitString,CString& brk); 

   // Parses a string delimited by commas or spaces
   int Parse(CStringArray& params,int maxParams=16384);  

   // Performs a similarly to the above function, except all white space is treated like spaces. 
   int ParseWhite(CStringArray& params,int maxParams=16384); 
    
   // Parses a string delimited by delimitList
   int Parse(CStringArray& params,const char* delimitList,bool trim=true,int maxParams=16384);

   // Parses a string delimited by delimitList.  Embedded double or single quote characters or a backslash        
   // will hide detection of characters in delimitList and should also not be included in delimitList.                                  
   // Special characters following the backslash will be converted similar to c escapes except that octal 
   // or hex chars are not recognized.  
   // ParseQuote removes embedded quote characters from the parsed parameters.
   // ParseQuutePreserveQuotes does not remove embedded quote characters.
   // Embedded backslash chars are backslash characters are removed from the substrings.  
   // If trim is true, then leading and trailing spaces around the parameter are deleted
   // (except those enclosed in quotes).  
   int ParseQuote(CStringArray& params,const char* delimitList,bool trim=true,int maxParams=16384)
      { return ParseQuoteBase(params,delimitList,trim,maxParams,true); }
   int ParseQuotePreserveQuotes(CStringArray& params,const char* delimitList,bool trim=true,int maxParams=16384)
      { return ParseQuoteBase(params,delimitList,trim,maxParams,false); }

private:
   int ParseQuoteBase(CStringArray& params,const char* delimitList,bool trim,int maxParams,bool stripQuotes);
};

//_____________________________________________________________________________

class CStringSupArray : public CStringArray
{
public:
   // Finds return index of item or -1 if not found
   int Find(CString findme);  // Case sensitive find
   int FindNoCase(CString findme);
};

//_____________________________________________________________________________
class CDcaOperationProgress : public CObject
{
protected:
   double m_length;
   double m_position;
   double m_nextUpdatePosition;

public:
   CDcaOperationProgress(double length = 0.);
   virtual ~CDcaOperationProgress();

   virtual void setLength(double length);
   virtual double updateProgress(double position);
   virtual double updateProgress();
   virtual double incrementProgress(double increment = 1.0);
   virtual void updateStatus(const CString& status);
   virtual void updateStatus(const CString& status,double length,double position=0.);

protected:
   virtual void init();
};

//_____________________________________________________________________________
class CDcaFileReadProgress : public CDcaOperationProgress
{
private:
   CFile* m_CFile;
   FILE* m_FILE;
   //CFileReader* m_fileReader;
   //CStdioCompressedFile* m_stdioCompressedFile;

public:
   CDcaFileReadProgress(CFile& file);
   CDcaFileReadProgress(FILE* file);
   //CFileReadProgress(CFileReader& fileReader);
   //CFileReadProgress(CStdioCompressedFile& stdioCompressedFile);

   double updateProgress();
};

//_____________________________________________________________________________
//class CStdioCompressedFile;
//
//class CDcaFileReadProgress : public CDcaOperationProgress
//{
//protected:
//   CFile* m_CFile;
//   FILE* m_FILE;
//   CFileReader* m_fileReader;
//   CStdioCompressedFile* m_stdioCompressedFile;
//
//public:
//   CDcaFileReadProgress(CFile& file);
//   CDcaFileReadProgress(FILE* file);
//   CDcaFileReadProgress(CFileReader& fileReader);
//   CDcaFileReadProgress(CStdioCompressedFile& stdioCompressedFile);
//
//   virtual double updateProgress();
//};

#endif
