// $Header: /CAMCAD/5.0/read_wrt/RwLib.h 66    5/22/07 1:46a Rick Faltersack $

#if !defined(__RwLib_h__)
#define __RwLib_h__

#pragma once

#include "Point2d.h"
#include "Colors.h"
#include <afxtempl.h>
#include <math.h> 

#include "TypedContainer.h"
#include "DcaCollections.h"
#include "DcaSortedMap.h"
#include "DcaLib.h"
#include "DcaRegistryKey.h"

//=============================================================================

class CWriteFormat;

double normalizeDegrees(double degrees);
double normalizeDegrees(double degrees,double multiple);
int    normalizeDegrees(int degrees);
double normalizeRadians(double radians);
bool radiansApproximatelyEqual(double radians0,double radians1,double tolerance=.002);
CString quoteString(const CString& string);
CString quoteString(const CString& string,const CString& quoteTriggerList);
CString properCase(CString string);
int getFileSize(const CString& path);
bool isFolder(const CString& pathname);
bool fileExists(const CString& path);
int trchar(CString& string,char fromChar,char toChar);
CString dirname(CString path);
int mkdirtree(const CString& dirPath);
int mkdirpath(const CString& filePath);
bool removeDirectoryTreeAbsolute(const CString& fullPath);
bool removeDirectoryTree(const CString& path);
CString RecursiveFindFilePath(CString parentPath, CString fileName); // Find file fileName in folder tree starting at parentPath

bool fgetcs(CString &string, FILE *ifp);  // Like fgets only better, reads into a CString, no pain in the neck arbitrary fixed size buffers.

void WriteStandardExportLogHeader(FILE *logFp, CString exportFormatName); // Write a standard log header tailored for exporters.

void displayProgress(CWriteFormat& log,const char* format,...);
void hideProgress();

class CProgressDlg;


//_____________________________________________________________________________
class CAppRegistrySetting
{
   // This is a conveinece class that fits the most commonly used profile for settings
   // I've been dealing with. It is not one-size-fits-all, nor necessarily the implementation
   // of a standard. But it is simple and covers a wide variety of my usage so far.
   // It could be expanded to cover more data types, as is supported by CRegistryKey.
private:
   CString m_category; // aka feature, profile, etc; thing like "PCB Navigator" or "Part Library"
   CString m_name;     // name of the setting

public:
   CAppRegistrySetting(CString category, CString name) { m_category = category; m_name = name; }
   CString Get();
   bool Get(bool& value);
   bool Set(CString value);
   bool Set(bool value);

};


//=============================================================================

// During conversion of registry settings key from Router Solutions to Mentor Graphics
// some special lookups were required for implementing backward compatibility.
// Rather then leaving them spread around the various places that did these lookups
// they have been grouped together in this class. Someday we might not care about this
// backward compatibility anymore, so we can just delete this class and then delete
// anything that referenced it and that is that.

class CBackwardCompatRegistry
{
public:
   CString Asymtek_GetDirectoryFromRegistry();
   CString DFT_GetTestplanDir();
   //void PropagateKey(HKEY hkey, CString profileName, CString valueName);
   void PropagateRegistry(HKEY hkey, CString keyname);
};


//=============================================================================


//_____________________________________________________________________________
#define MaxParserParams 256
#define MaxNumLength 20

class CFileReader : public CObject
{
protected:
   int m_numParams;
   int m_maxParams;
   double m_inchesPerUnit;
   double m_pageUnitsPerUnit;
   int m_numRecordsRead;

public:
   CFileReader(int maxParams = MaxParserParams) { m_maxParams = maxParams; }
   virtual ~CFileReader(){}

   virtual bool open(const CString& filePath) = 0;
   virtual void close() = 0;
   virtual bool readRecord() = 0;
   virtual CString getRecord() = 0;
   virtual void setInchesPerUnit(double inchesPerUnit) { m_inchesPerUnit = inchesPerUnit; }
   virtual void setPageUnitsPerUnit(double pageUnitsPerUnit) { m_pageUnitsPerUnit = pageUnitsPerUnit; }

   virtual int getNumParams() { return m_numParams; }
   virtual int getNumRecordsRead() { return m_numRecordsRead; }
   virtual CString getParam(int index) = 0;
   virtual void getParam(CString& param,int index) = 0;
   virtual void getParam(int& iParam,int index) = 0;
   virtual void getParam(double& fParam,int index) = 0;
   virtual void getParamInInches(double& fParam,int index) { getParam(fParam,index);  fParam *= m_inchesPerUnit; }
   virtual void getParamInPageUnits(double& fParam,int index) { getParam(fParam,index);  fParam *= m_pageUnitsPerUnit; }
   virtual int getFileLength() = 0;
   virtual int getFilePosition() = 0;
};

//_____________________________________________________________________________
class COperationProgress : public CDcaOperationProgress
{
private:
   CProgressDlg* m_allocatedDialog;

public:
   COperationProgress(double length = 0.);
   virtual ~COperationProgress();

   virtual void setLength(double length);
   virtual double updateProgress(double position);
   virtual double incrementProgress(double increment = 1.0);
   virtual void updateStatus(const CString& status);
   virtual void updateStatus(const CString& status,double length,double position=0.);
   CProgressDlg* getProgressDlg() { return m_allocatedDialog;}


protected:
   virtual void init();
};

//_____________________________________________________________________________
class CStdioCompressedFile;

class CFileReadProgress : public COperationProgress
{
private:
   CFile* m_CFile;
   FILE* m_FILE;
   CFileReader* m_fileReader;
   CStdioCompressedFile* m_stdioCompressedFile;

public:
   CFileReadProgress(CFile& file);
   CFileReadProgress(FILE* file);
   CFileReadProgress(CFileReader& fileReader);
   CFileReadProgress(CStdioCompressedFile& stdioCompressedFile);

   double updateProgress();
};

//_____________________________________________________________________________
class CMemoryStatus
{
private:
   CString m_description;
   MEMORYSTATUSEX m_memoryStatus;

public:
   CMemoryStatus(const CString& description="");

   CString getDescription() const { return m_description; }
   void setDescription(const CString& description) { m_description = description; }

   bool sampleMemoryStatus();
   const MEMORYSTATUSEX& getMemoryStatus() { return m_memoryStatus; }
   CString int64DisplayString(unsigned __int64 value);
   void writeStatusReport(CWriteFormat& writeFormat);
   void writeComparitiveStatusReport(CWriteFormat& writeFormat,const CMemoryStatus& other);
   void writeStatusReportLegend(CWriteFormat& writeFormat);
};

#endif

