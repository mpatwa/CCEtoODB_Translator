// $Header: /CAMCAD/DcaLib/DcaLib.cpp 6     6/22/07 3:22p Moustafa Noureddine $

#include "StdAfx.h"
#include "DcaLib.h"
#include "DcaFpeq.h"

#include <io.h>
#include <sys/stat.h>
#include <direct.h>
#include <math.h>

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 3/24/99    Time: 5:54a
 * Created in $/LibKnvPP
 * Initial add.
*/

//#include "StdAfx.h"

/*
   Returns the next prime larger than 2^n where   2^(n-1) < target <= 2^n
*/

//_____________________________________________________________________________
int nextPrime2n(int targetPrime)
{
   static int primes2n[] =
   {
//      0        1        2        3        4        5        6        7
//      1        2        4        8       16       32       64      128     
        2,       3,       5,      11,      17,      31,      67,     131,

//      8        9       10       11       12       13       14       15
//    256      512     1024     2048     4096     8192    16384    32768     
      257,     521,    1031,    2053,    4099,    8209,   16411,   32771,

//     16       17       18       19       20       21       22       23
//  65536   131072   262144   524288  1048576  2097152  4194304  8388608  
    65537,  131203,  262147,  524309, 1048583, 2097169, 4194319, 8388617

   };

   int retval = targetPrime;

   for (int i = 0;i < sizeof(primes2n)/sizeof(int);i++)
   {
      if (primes2n[i] > targetPrime)
      {
         retval = primes2n[i];
         break;
      }
   }

   return retval;
}

//_____________________________________________________________________________
int DcaRound(double f)
{
   return((int)(f + ((f < 0.0) ? -.5 : .5)));
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 1/04/99    Time: 11:12a
 * Updated in $/PadsDRC
 * change static char* format to static char format[] 
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 4/23/98    Time: 2:20p
 * Created in $/PadsDRC
 * Initial add.
*/

//#include "StdAfx.h"
//#include <math.h>

//_____________________________________________________________________________
const char* fpfmt(double number,int maxPrecision)
{
   const int numBuffers = 10;
   const int bufferSize = 20;
   const double digitFactor = 10000.;
   
   static int bufferIndex = 0;
   static char buffers[numBuffers][bufferSize];
   static char format[] = { '%', '.', '4', 'f', '\0' };
   
   char *buf,*p;
   int i,precision;
   
   buf = buffers[bufferIndex];
   
   if (fabs(number) > 1000000.)
   {
      sprintf(buf,"%g.6",number);
   }
   else
   {
      if (number >= 1000.)
      {
         if (number >= 10000.)
         {
            if (number >= 100000.)
            {
               precision = 1;
            }
            else
            {
               precision = 2;
            }
         }
         else
         {
            precision = 3;
         }
      }
      else if (number <= -1000.)
      {
         if (number <= -10000.)
         {
            if (number <= -100000.)
            {
               precision = 1;
            }
            else
            {
               precision = 2;
            }
         }
         else
         {
            precision = 3;
         }
      }
      else
      {
         precision = 4;
      }
   
      if (precision > maxPrecision)
      {
         precision = maxPrecision;
      }
   
      format[2] = '0' + precision;
   
      sprintf(buf,format,number);
   
      for (i = strlen(buf) - 1,p = buf + i;i >= 0;i--,p--)
      {
         if (*p != '0')
         {
            if (*p == '.')
            {
               *p = '\0';
            }
         
            break;
         }
      
         *p = '\0';
      }

      if (buf[0] == '-' && buf[1] == '0' && buf[2] == '\0')
      {
         buf[0] = '0';
         buf[1] = '\0';
      }
   }
   
   if (++bufferIndex >= numBuffers)
   {
      bufferIndex = 0;
   }
   
   return buf;
}

//_____________________________________________________________________________
CString fpfmtExactPrecision(double number,int precision)
{
   CString retval;
   int decimalPointPosition,sign;

   if (precision < 1)
   {
      precision = 1;
   }

   CString rawDigits(ECVT(number,precision,&decimalPointPosition,&sign));

   if (sign != 0)
   {
      retval = "-";
   }

   if (decimalPointPosition >= precision)
   {
      retval += rawDigits;

      for (int i = precision;i < decimalPointPosition;i++)
      {
         retval += "0";
      }
   }
   else
   {
      if (decimalPointPosition <= 0)
      {
         retval += ".";

         for (int i = decimalPointPosition;i < 0;i++)
         {
            retval += "0";
         }

         retval += rawDigits;
      }
      else
      {
         retval += rawDigits.Left(decimalPointPosition);
         retval += ".";
         retval += rawDigits.Right(precision - decimalPointPosition);
      }

      char* p = retval.GetBuffer() + retval.GetLength() - 1;

      while (*p == '0')
      {
         --p;

         if (*p != '.')
         {
            p[1] = '\0';
         }
      }

      retval.ReleaseBuffer();
   }
   
   return retval;
}

double normalizeDegrees(double degrees)
{
   const double a360 = 360.;
   degrees = fmod(fmod(degrees,a360) + a360,a360);

   return degrees;
}

double normalizeDegrees(double degrees,double multiple)
{
   const double a360 = 360.;

   if (multiple != 0. && degrees != 0.)
   {
      if (multiple < 0.) multiple = -multiple;

      double scaledDegrees = degrees / multiple;

      if (scaledDegrees > 0.)
      {
         scaledDegrees = floor(scaledDegrees + .5);
      }
      else
      {
         scaledDegrees =  ceil(scaledDegrees - .5);
      }

      degrees = scaledDegrees * multiple;
   }

   degrees = fmod(fmod(degrees,a360) + a360,a360);

   return degrees;
}

int normalizeDegrees(int degrees)
{	
   const int a360 = 360;	
	degrees = (degrees % a360 + a360) % a360;

   return degrees;
}

double normalizeRadians(double radians)
{
   const double twoPi = 2.*Pi;
   radians = fmod(fmod(radians,twoPi) + twoPi,twoPi);

   return radians;
}

bool radiansApproximatelyEqual(double radians0,double radians1,double tolerance)
{
   double deltaRadians = normalizeRadians(radians0 - radians1);

   if (deltaRadians > Pi)
   {
      deltaRadians = 2.*Pi - deltaRadians;
   }

   return fpnear(deltaRadians,0.,tolerance);
}

CString quoteString(const CString& string)
{
   if (string.Find(' ') >= 0)
   {
      if (string.Left(1) != "\"" || string.Right(1) != "\"")
      {
         return '"' + string + '"';
      }
   }

   return string;
}

CString quoteString(const CString& string,const CString& quoteTriggerList)
{
   if (string.GetLength() > 0 && (string.Left(1) != "\"" || string.Right(1) != "\""))
   {
      if (string.Find('"') >= 0)
      {
         CString returnString("\"");

         for (const char* p = string;*p != '\0';p++)
         {
            if (*p == '"' || *p == '\\')
            {
               returnString += '\\';
            }

            returnString += *p;
         }

         returnString += "\"";

         return returnString;
      }

      if (string.FindOneOf(quoteTriggerList) >= 0)
      {
         return '"' + string + '"';
      }
   }

   return string;
}

CString properCase(CString string)
{
   string.MakeLower();

   if (string.GetLength() > 0)
   {
      string.SetAt(0,toupper(string.GetAt(0)));
   }

   return string;
}

int getFileSize(const CString& path)
{
   int retval = -1;

   extern int errno;
   struct _stat status;
   
   int statVal    = _stat(path,&status);
   int localErrno = errno;

   if (statVal == 0)
   {
      retval = status.st_size;
   }

   return retval;
}

bool isFolder(const CString& pathname)
{
   struct _stat status;
   bool retval = false;

   if (_stat(pathname,&status) == 0)
   {
      retval =  ((status.st_mode & _S_IFDIR) != 0);
   }

   return retval;
}

bool fileExists(const CString& path)
{
   extern int errno;
   struct _stat status;
   
   int statVal    = _stat(path,&status);
   int localErrno = errno;

   bool retval = (statVal == 0);

   return retval;
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 5/20/98    Time: 4:43p
 * Updated in $/libknvpp
 * added new overload (flags parameter)
 * 
 * *****************  Version 1  *****************
 * User: Kurtv        Date: 3/20/98    Time: 8:46p
 * Created in $/libknvpp
 * Initial add
*/

//#include "StdAfx.h"

//_____________________________________________________________________________
int formatMessageBox(const char* format,...) 
{
   va_list args;
   va_start(args,format);

   CString buf;
   buf.FormatV(format,args);

   int retval = AfxMessageBox(buf); //*rcf this is a bug, does not silence !!!

   return retval;
}

int formatMessageBox(UINT flags,const char* format,...)
{
   va_list args;
   va_start(args,format);

   CString buf;
   buf.FormatV(format,args);

   int retval = AfxMessageBox(buf,flags);  //*rcf this is a bug, does not silence !!!

   return retval;
}

//=============================================================================

/*
History: RwLib.cpp $
 *
 * *****************  Version 5  *****************
 * User: Kvanness     Date: 6/01/01    Time: 2:32a
 * Updated in $/LibKnvPP
 * Bug fix -  CFilePath::getDirectoryPath() - removed occurances of double
 * delimiters.
 * 
 * *****************  Version 4  *****************
 * User: Kvanness     Date: 3/18/01    Time: 11:35a
 * Updated in $/LibKnvPP
 * Added support for default delimiter.
 * 
 * *****************  Version 3  *****************
 * User: Kvanness     Date: 9/11/00    Time: 11:33p
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 2  *****************
 * User: Kvanness     Date: 2/06/00    Time: 12:25p
 * Updated in $/LibKnvPP
 * Fixed bug in setDelimiter()
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 1/04/00    Time: 3:29a
 * Created in $/DocGen
 * Initial add
*/

//________________________________________________________________________________________
bool CFilePath::m_defaultDelimiterSlash = true;
const CString CFilePath::m_slash("/");
const CString CFilePath::m_backSlash("\\");

inline bool isDelimiter(const char c,const char delimit1,const char delimit2)
{
   return (c == delimit1) || (c == delimit2);
}

inline bool isDelimiter(const char c)
{
   return (c == '/') || (c == '\\');
}

inline bool isDelimiter(const CString& string)
{
   return (string == "/") || (string == "\\");
}

//________________________________________________________________________________________
CFilePath::CFilePath()
{
   m_beautify = false;
   m_parsed   = true;
   m_built    = true;
   setDelimiterSlash(m_defaultDelimiterSlash);
}

CFilePath::CFilePath(const CString& path,bool beautifyFlag)
{
   m_path     = path;
   m_beautify = beautifyFlag;

   m_parsed   = false;
   m_built    = false;
   setDelimiterSlash(m_defaultDelimiterSlash);

   parse('/','\\');

   if (m_beautify)
   {
      beautify();
   }

   build();
}

CFilePath::CFilePath(const CString& path,char delimiter,bool beautifyFlag)
{
   m_path     = path;
   m_beautify = beautifyFlag;
   m_parsed   = false;
   m_built    = false;

   setDelimiterSlash(delimiter == '/');

   parse(delimiter,delimiter);

   if (m_beautify)
   {
      beautify();
   }

   build();
}

CFilePath::CFilePath(const CFilePath& otherFilePath)
{
   *this = otherFilePath;
}

CFilePath::~CFilePath()
{
}

CFilePath& CFilePath::operator=(const CFilePath& otherFilePath)
{
   if (&otherFilePath != this)
   {
      m_path           = otherFilePath.getPath();
      m_beautify       = otherFilePath.m_beautify;
      m_delimiter      = otherFilePath.m_delimiter;
      m_delimiterSlash = otherFilePath.m_delimiterSlash;

      m_parsed         = false;
      m_built          = true;
   }

   return *this;
}

void CFilePath::empty()
{
   m_path.Empty();
   m_built  = true;
   m_parsed = false;
}

int CFilePath::getDirectoryDepth() const
{
   parse();

   return m_directoryList.GetCount();
}

void CFilePath::build() const
{
   if (!m_built)
   {
      m_built = true;

      if (m_parsed)
      {
         m_path = getFoundation();

         for (POSITION pos = m_directoryList.GetHeadPosition();pos != NULL;)
         {
            CString dir(m_directoryList.GetNext(pos));
            m_path += dir;

            if (pos != NULL && dir != getDelimiter())
            {
               m_path += getDelimiter();
            }
         }

         if (! m_baseFileName.IsEmpty())
         {
            if (!m_path.IsEmpty() && m_path.Right(1) != getDelimiter())
            {
               m_path += getDelimiter();
            }

            m_path += m_baseFileName;
         }

         if (! m_extension.IsEmpty())
         {
            m_path += "." + m_extension;
         }

         //if (m_beautify)
         //{
         //   beautify();
         //}
      }
   }
}

void CFilePath::parse(const char delimit1,const char delimit2) const
{
   if (!m_parsed)
   {
      const char nil = '\0';
      bool serverNameFlag = false;

      const char* p = m_path.GetBuffer(0);
      m_directoryList.RemoveAll();
      m_extension.Empty();
      m_baseFileName.Empty();
      m_serverName.Empty();
      m_driveLetter.Empty();

      if (isalpha(p[0]) && p[1] == ':')
      {
         m_driveLetter.GetBufferSetLength(1);
         m_driveLetter.SetAt(0,*p);
         m_driveLetter.ReleaseBuffer(1);

         p += 2;
      }
      else
      {
         if (isDelimiter(p[0],delimit1,delimit2) && isDelimiter(p[1],delimit1,delimit2))
         {
            serverNameFlag = true;
            p++;
         }
      }

      if (isDelimiter(*p,delimit1,delimit2))
      {
         m_directoryList.AddTail(getDelimiter());
         p++;
      }

      while (true)
      {
         if (*p == nil)
         {
            if (! m_baseFileName.IsEmpty())
            {
               int position = m_baseFileName.ReverseFind('.');

               if (position > 0 && position + 1 < m_baseFileName.GetLength())
               {
                  m_extension = m_baseFileName.Mid(position + 1);
                  m_baseFileName.GetBufferSetLength(position);
               }
            }

            break;
         }
         else if (isDelimiter(*p,delimit1,delimit2))
         {
            if (!m_baseFileName.IsEmpty())
            {
               if (serverNameFlag)
               {
                  m_serverName = m_baseFileName;
                  serverNameFlag = false;
               }
               else
               {
                  m_directoryList.AddTail(m_baseFileName);
               }

               m_baseFileName.Empty();
            }

            p++;
         }
         else
         {
            m_baseFileName += *p;
            p++;
         }
      }

      m_parsed = true;
   }
}

void CFilePath::beautify()
{
   parse();
   CString dir;
   POSITION pos,oldPos;

   for (pos = m_directoryList.GetHeadPosition();pos != NULL;)
   {
      oldPos = pos;
      dir = m_directoryList.GetNext(pos);
      beautifyWord(dir);
      m_directoryList.SetAt(oldPos,dir);
   }

   beautifyWord(m_baseFileName);
   beautifyWord(m_serverName);

   m_extension.MakeLower();
   m_driveLetter.MakeUpper();

   m_built    = false;
   m_beautify = true;
}

void CFilePath::beautifyWord(CString& word)
{
   if (word.GetLength() > 1)
   {
      CString upper(word);
      CString lower(word);
      upper.MakeUpper();
      lower.MakeLower();

      if (word == upper || word == lower)
      {
         word = lower;
         word.SetAt(0,toupper(word.GetAt(0)));
      }
   }
}

CString CFilePath::getPath() const
{
   build();
   return m_path;
}

CString CFilePath::getShortPath() const
{
   //build();
   int pathLen = max(256,2*getPath().GetLength());
   CString shortPath;

   DWORD status = GetShortPathName(getPath(),shortPath.GetBufferSetLength(pathLen),pathLen);

   if (status == 0)
   {
      DWORD lastError = GetLastError();
      CString messageBuffer;

      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,lastError,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer.GetBufferSetLength(256),256,0);

      messageBuffer.ReleaseBuffer();
   }

   shortPath.ReleaseBuffer();

   return shortPath;
}

CString CFilePath::getDrive() const
{
   parse();

   if (m_driveLetter.IsEmpty())
   {
      return m_driveLetter;
   }
   else
   {
      return m_driveLetter + ":";
   }
}

CString CFilePath::getDriveLetter() const
{
   parse();
   return m_driveLetter;
}

CString CFilePath::getServer() const
{
   parse();

   if (m_serverName.IsEmpty())
   {
      return m_serverName;
   }
   else
   {
      return getDelimiter() + getDelimiter() + m_serverName;
   }
}

CString CFilePath::getServerName() const
{
   parse();
   return m_serverName;
}

CString CFilePath::getFoundation() const
{
   CString foundation;

   if (! m_serverName.IsEmpty())
   {
      foundation = getServer();
   }
   else if (! m_driveLetter.IsEmpty())
   {
      foundation = getDrive();
   }

   return foundation;
}

CString CFilePath::getFileName() const
{
   parse();

   if (m_extension.IsEmpty())
   {
      return m_baseFileName;
   }
   else
   {
      return m_baseFileName + "." + m_extension;
   }
}

CString CFilePath::getBaseFileName() const
{
   parse();
   return m_baseFileName;
}

CString CFilePath::getExtension() const
{
   parse();
   return m_extension;
}

CString CFilePath::getFullExtension() const
{
   parse();

   if (m_extension.IsEmpty())
   {
      return m_extension;
   }
   else
   {
      return "." + m_extension;
   }
}

CString CFilePath::getDirectoryPath() const
{
   parse();

   CString directoryPath(getFoundation());

   for (POSITION pos = m_directoryList.GetHeadPosition();pos != NULL;)
   {
      CString dir(m_directoryList.GetNext(pos));

      if (isDelimiter(dir))
      {
         directoryPath += getDelimiter();
      }
      else
      {
         directoryPath += dir;

         if (pos != NULL)
         {
            directoryPath += getDelimiter();
         }
      }
   }

   return directoryPath;
}

CString CFilePath::getDirectory() const
{
   parse();

   if (m_directoryList.GetCount() > 0)
   {
      return m_directoryList.GetTail();
   }
   else
   {
      return CString();
   }
}

CString CFilePath::popRoot()
{
   parse();
   CString retval;

   if (m_directoryList.GetCount() > 0)
   {
      retval = getFoundation() + m_directoryList.GetHead();

      m_driveLetter.Empty();
      m_serverName.Empty();
      m_directoryList.RemoveHead();
      m_built = false;
   }

   return retval;
}

void CFilePath::pushRoot(const CString& directoryName)
{
   CFilePath directory(directoryName);

   for (POSITION pos = m_directoryList.GetHeadPosition();pos != NULL;)
   {
      CString dirName = m_directoryList.GetNext(pos);
      directory.pushLeaf(dirName);
   }

   directory.pushLeaf(getFileName());
   *this = directory;
}

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
CString CFilePath::popLeaf()
{
   parse();
   CString retval;

   if (m_directoryList.GetCount() > 0)
   {
      if (m_directoryList.GetCount() == 1 && 
          m_directoryList.GetHead() == getDelimiter() )
      {
         if (m_baseFileName.IsEmpty())
         {
            retval = getPath();
            empty();
         }
         else
         {
            retval = getFileName();
            m_baseFileName.Empty();
            m_extension.Empty();
            m_built = false;
         }
      }
      else
      {
         // weird results if the parent directory has an extension
         retval = getFileName();
         m_baseFileName = m_directoryList.RemoveTail();
         m_extension.Empty();
         m_built = false;
      }
   }
   else
   {
      retval = getPath();
      empty();
   }

   return retval;
}

void CFilePath::pushLeaf(const CString& leafPath)
{
   build();

   CFilePath leafFilePath(*this);
   leafFilePath.setPath(leafPath);
   leafFilePath.m_driveLetter.Empty();
   leafFilePath.m_serverName.Empty();
   leafFilePath.m_built = false;

   CString leafName(leafFilePath.getPath());

   if (!leafName.IsEmpty())
   {
      if (!m_path.IsEmpty())
      {
         if (!isDelimiter(m_path.GetAt(m_path.GetLength() - 1)) && !isDelimiter(leafName.GetAt(0)) )
         {
            m_path += getDelimiter();
         }
      }

      setPath(m_path + leafName);
   }
}

bool CFilePath::isAbsolutePath() const
{
   parse();

   bool retval = (m_directoryList.GetCount() > 0 && m_directoryList.GetHead() == getDelimiter());

   return retval;
}

void CFilePath::setPath(const CString& path)
{
   m_path   = path;

   m_parsed = false;
   m_built  = false;

   parse('/','\\');

   //setDelimiterSlash(true);

   if (m_beautify)
   {
      beautify();
   }

   build();
}

void CFilePath::setPath(const CString& path,char delimiter)
{
   m_path     = path;
   m_parsed   = false;
   m_built    = false;

   setDelimiterSlash(delimiter == '/');

   parse();

   if (m_beautify)
   {
      beautify();
   }

   build();
}

void CFilePath::setDriveLetter(const CString& drive)
{
   parse();

   if (! drive.IsEmpty() && isalpha(drive.GetAt(0)))
   {
      m_driveLetter = (char)toupper(drive.GetAt(0));
   }
   else
   {
      m_driveLetter.Empty();
   }

   m_serverName.Empty();

   m_built = false;
}

void CFilePath::setServerName(CString serverName)
{
   parse();

   if (! serverName.IsEmpty())
   {
      const char* p = serverName;

      if (p[0] == m_slash || p[0] == m_backSlash) p++;
      if (p[0] == m_slash || p[0] == m_backSlash) p++;

      CFilePath path(p);

      serverName = path.popRoot();
   }

   m_driveLetter.Empty();
   m_serverName = serverName;

   m_built = false;
}

void CFilePath::setBaseFileName(const CString& baseFileName)
{
   parse();

   m_baseFileName = baseFileName;

   if (m_beautify)
   {
      beautifyWord(m_baseFileName);
   }

   m_built = false;
}

void CFilePath::setFileName(const CString& fileName)
{
   // Allows fileName to be a relative path that is appended to current path
   // as well as a simple file name to be appended.
   // THIS DOES AN APPEND to what ever is already in the path, as opposed to presuming
   // the current leaf is a filename to be replaced.
   // That is what the first (and currently only) usage of this function wants from it.
   // If you want to make this a leaf-replacer then turn this into an appender, give it
   // a different name, and replace the current call(s) with it. Then make your new thing.

   m_path += getDelimiter() + fileName;

   m_parsed = false;
   parse();

   if (m_beautify)
   {
      beautifyWord(m_baseFileName);
   }

   m_built = false;
}

void CFilePath::setExtension(const CString& extension)
{
   parse();

   m_extension = extension;

   if (m_beautify)
   {
      m_extension.MakeLower();
   }

   m_built = false;
}

void CFilePath::setFullExtension(const CString& fullExtension)
{
   parse();

   if (fullExtension.Left(1) == '.')
   {
      m_extension = fullExtension.Mid(1);
   }
   else
   {
      m_extension = fullExtension;
   }

   if (m_beautify)
   {
      m_extension.MakeLower();
   }

   m_built = false;
}

void CFilePath::setDelimiterSlash()
{
   setDelimiter(m_slash);
}

void CFilePath::setDelimiterBackSlash()
{
   setDelimiter(m_backSlash);
}

void CFilePath::setDelimiter(const CString& delimiter)
{
   bool backslash = (! delimiter.IsEmpty() && delimiter.GetAt(0) == '\\');

   if (backslash == m_delimiterSlash)
   {
      parse();
      setDelimiterSlash(!backslash);
      m_built = false;

      for (POSITION pos = m_directoryList.GetHeadPosition();pos != NULL;)
      {
         CString dir(m_directoryList.GetAt(pos));

         if (isDelimiter(dir))
         {
            m_directoryList.SetAt(pos,getDelimiter());
         }

         break;
      }
   }
}

void CFilePath::setDelimiterSlash(bool useSlash)
{
   m_delimiterSlash = useSlash;
   m_delimiter      = (m_delimiterSlash ? '/' : '\\');
}

bool CFilePath::setDefaultDelimiterSlash(bool useSlash)
{
   bool retval = m_defaultDelimiterSlash;
   m_defaultDelimiterSlash = useSlash;

   return retval;
}

//=============================================================================

/*
History: RwLib.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Kvanness     Date: 9/16/99    Time: 6:53a
 * Updated in $/LibKnvPP
 * Added comments.
 * 
 * *****************  Version 8  *****************
 * User: Kurtv        Date: 11/19/98   Time: 10:27a
 * Updated in $/LibKnvPP
 * Change BOOL to bool, where appropriate.
 * 
 * *****************  Version 7  *****************
 * User: Kurtv        Date: 2/12/98    Time: 1:45p
 * Updated in $/LibKnvPP
 * Added ParseQuote()
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 9/22/97    Time: 7:40a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 7/19/97    Time: 8:26p
 * Updated in $/LibKnvPP
 * Added various Parse() methods
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/

//#include "Stdafx.h"
//#include "SupString.h"

//_____________________________________________________________________________________________________________________________________
CSupString::CSupString()
: m_singleCharacterEscape('\\')
{
}

CSupString::CSupString(const CString& stringSrc)
: CString(stringSrc)
, m_singleCharacterEscape('\\')
{
}

CSupString::CSupString(char ch, int nRepeat)
: CString(ch,nRepeat)
, m_singleCharacterEscape('\\')
{
}

CSupString::CSupString(const char* psz)
: CString(psz)
, m_singleCharacterEscape('\\')
{
}

CSupString::CSupString(const unsigned char* psz)
: CString(psz)
, m_singleCharacterEscape('\\')
{
}

CSupString::CSupString(const char* pch, int nLength)
: CString(pch,nLength)
, m_singleCharacterEscape('\\')
{
}

char CSupString::getSingleCharacterEscape() const
{
   return m_singleCharacterEscape;
}

char CSupString::getDefinedSingleCharacterEscape() const
{
   return ((m_singleCharacterEscape == '\0') ? '\\' : m_singleCharacterEscape);
}

void CSupString::setSingleCharacterEscape(char singleCharacterEscape)
{
   m_singleCharacterEscape = singleCharacterEscape;
}

bool CSupString::isSingleCharacterEscape(char c)
{
   bool retval = (m_singleCharacterEscape != '\0' && m_singleCharacterEscape == c);

   return retval;
}

// Splits the string into 3 pieces at the first occurance of pszSplitString.
// The first piece is returned as the function value, the second piece (equal to pszSplitString)
// is returned in brk, the third piece becomes the value of this.
CString CSupString::Scan(const char* pszSplitString,CString& brk)
{
   CString retval;
   
   int pos = Find(pszSplitString);
   brk.Empty();
   
   if (pos >= 0)
   {
      brk = pszSplitString;
      retval = Left(pos);
      *this = Mid(pos + brk.GetLength());
   }
   else
   {
      retval = *this;
      this->Empty();
   }
   
   return retval;
}

// Splits the string into 3 pieces at the first occurance of pszSplitString.
// The first piece is returned as the function value, the second piece (equal to pszSplitString)
// is discarded, the third piece becomes the value of this.
CString CSupString::Scan(const char* pszSplitString)
{
   CString retval;
   
   int pos = Find(pszSplitString);
   
   if (pos >= 0)
   {
      int len = strlen(pszSplitString);
      retval = Left(pos);
      *this = Mid(pos + len);
   }
   else
   {
      retval = *this;
      this->Empty();
   }
   
   return retval;
}

/**********************************************************************/
/*  Parse performs a similarly to the lpar() function.                */
/*  lpar parses a string delimited by commas or blanks                */
/*  into number substrings.                                           */
/*  If there are more substrings in line than maxParams, then         */
/*  the last parameter contains the remainder of the line.            */
/*  To assure that N substrings are completely parsed (both leading   */
/*  and trailing blanks deleted) specify number to be N + 1.          */
/**********************************************************************/
int CSupString::Parse(CStringArray& params,int maxParams)
{
   char *p,*q,*buf;
  
   CString line(*this);
   p = q = buf = line.GetBuffer(0);
   params.RemoveAll();
  
   /* skip leading blanks */
   while (*p == ' ') p++;
  
   while (true)
   {
      /* skip over parameter */
      while(*p != ' ' && *p != ',' && *p != '\0') *(q++) = *(p++);
  
      /* skip trailing spaces */
      while(*p == ' ') p++;
  
      /* check for end of line */
      if (*p == '\0') break;
  
      /* insert a comma */
      *(q++) = ',';
  
      /* skip comma if one is encountered */
      if (*p == ',') p++;
   }
  
   *q = '\0';
   p  = buf;
  
   while (true)
   {
      q = p;        /* set param[i] to first char of the parameter */
  
      /* skip to the end of the string or to a comma */
      while(*p != '\0' && *p != ',') p++;
  
      /* check for end of the string */
      if (*p == '\0')
      {
         if (*q != '\0')
         {
            params.Add(q);
         }
  
         break;
      }
  
      /* null out comma */
      *(p++) = '\0';

      if (params.Add(q) >= maxParams - 1)
      {
         break;
      }
   }
  
   return(params.GetSize());
}

/**********************************************************************/
/*  ParseWhite performs a similarly to the above function, except all */
/*  white space is treated like spaces.                               */
/*  lpar parses a string delimited by commas or white space           */
/*  into number substrings.                                           */
/*  If there are more substrings in line than maxParams, then         */
/*  the last parameter contains the remainder of the line.            */
/*  To assure that N substrings are completely parsed (both leading   */
/*  and trailing white space deleted) specify number to be N + 1.     */
/**********************************************************************/
int CSupString::ParseWhite(CStringArray& params,int maxParams)
{
   char *p,*q,*buf;
  
   CString line(*this);
   p = q = buf = line.GetBuffer(0);
   params.RemoveAll();
  
   /* skip leading blanks */
   while (isspace(*p)) p++;
  
   while (true)
   {
      /* skip over parameter */
      while(!isspace(*p) && *p != ',' && *p != '\0') *(q++) = *(p++);
  
      /* skip trailing spaces */
      while(isspace(*p)) p++;
  
      /* check for end of line */
      if (*p == '\0') break;
  
      /* insert a comma */
      *(q++) = ',';
  
      /* skip comma if one is encountered */
      if (*p == ',') p++;
   }
  
   *q = '\0';
   p  = buf;
  
   while (true)
   {
      q = p;        /* set param[i] to first char of the parameter */
  
      /* skip to the end of the string or to a comma */
      while(*p != '\0' && *p != ',') p++;
  
      /* check for end of the string */
      if (*p == '\0')
      {
         if (*q != '\0')
         {
            params.Add(q);
         }
  
         break;
      }
  
      /* null out comma */
      *(p++) = '\0';

      if (params.Add(q) >= maxParams - 1)
      {
         break;
      }
   }
  
   return(params.GetSize());
}

/**********************************************************************/
/*  Parse performs a similarly to the lparb() function.               */
/*  lparb parses a string delimited by delimitList                    */
/*  into number substrings.                                           */
/*  if trim is true, then leading and trailing spaces around the      */
/*  parameter are deleted                                             */
/*  If there are more substrings in line than param can hold, then    */
/*  the last parameter contains the remainder of the line.            */
/**********************************************************************/
int CSupString::Parse(CStringArray& params,const char* delimitList,bool trim,int maxParams)
{
   char *p,*q,*r,*rr,*param;
  
   CString line(*this);
   q = line.GetBuffer(0);
   params.RemoveAll();
  
   while (true)
   {
      /* skip leading blanks */
      while(trim && *q == ' ') q++;
     
      param = q;        /* set param[i] to first char of the parameter */

      /* set p to point to first delimit character */
      p = strpbrk(q,delimitList);

      if (p != NULL) *p = '\0';

      /* trim off trailing blanks */
      if (trim)
      {
         for (r = q,rr = NULL;*r != '\0';r++)
         {
            if (*r != ' ') rr = NULL;
            else if (rr == NULL) rr = r;
         }

         if (rr != NULL) *rr = '\0';
      }

      if (p == NULL)
      {
         params.Add(param);
         break;
      }

      q = p + 1;

      if (*q == '\0')
      {
         params.Add(param);
         break;
      }

      if (params.Add(param) >= maxParams - 2)
      {
         params.Add(q);
         break;
      }
   }
  
   return(params.GetSize());
}

/**********************************************************************/
/*  ParseQuoteBase() performs a similarly to the lpar() function.     */
/*  lparq parses a string delimited by delimitList                    */
/*  into number substrings.                                           */
/*  Embedded double or single quote characters or a backslash         */
/*  will hide detection of characters in delimitList and should also  */
/*  not be included in delimitList.                                   */
/*  Special characters following the backslash will be converted      */
/*  similar to c escapes except that octal or hex chars are not       */
/*  recognized.                                                       */
/*  If stripQuotes is true the embedded quote characters are removed  */
/*  from the substrings.                                              */
/*  The embedded backslash characters are removed from the            */
/*  substrings.                                                       */
/*  if trim is true, then leading and trailing spaces around the      */
/*  parameter are deleted (except those enclosed in quotes).          */
/*  If there are more substrings in line than param can hold, then    */
/*  the last parameter contains the remainder of the line.            */
/**********************************************************************/
int CSupString::ParseQuoteBase(CStringArray& params,const char* delimitList,bool trim,int maxParams, bool stripQuotes)
{
   enum states
   {
      sSkipLeading, sFirstChar, sLookForEnd, sSingleQuote, 
      sDoubleQuote, sBackSlash, sDelimitFound, sEndOfString
   } state,lastState;

   CString line(*this);
   params.RemoveAll();
  
   char *pvis,*r,*rr,ss[2];
  
   int i = 0;
   char* p = NULL;
   char* param = NULL;
   ss[1] = '\0';
   state = sSkipLeading;

   if (trim)
   {
      char* pline = line.GetBuffer(0);

      for (int ind = strlen(pline) - 1;ind >= 0;ind--)
      {
         if (pline[ind] == ' ')
         {
            pline[ind] = '\0';
         }
         else
         {
            break;
         }
      }

      line.ReleaseBuffer();
   }

   char* q = line.GetBuffer(0);

   for (bool parseFlag = true;parseFlag;)
   {
      switch(state)
      {
         case sSkipLeading:   /* skip leading blanks */
            if (*q == ' ' && trim)
            {
               q++;
            }
            else
            {
               state = sFirstChar;
            }
   
            break;
         case sFirstChar:  /* first character of potential parameter */
            param = p = pvis = q;

            if (++i > maxParams)
            {
               parseFlag = false;

               if (*param != '\0')
               {
                  params.Add(param);
               }
            }
            else
            {
               state = sLookForEnd;
            }

            break;
         case sLookForEnd: /* looking for delimit character or end of string */
            *pvis = *q;

            if (*q == '\0')
            {
               state = sEndOfString;
            }
            else if (*q == '"')
            {
               q++;

               if (!stripQuotes)
               {
                  pvis++;
               }

               state = sDoubleQuote;
            }
            else if (*q == '\'')
            {
               q++;

               if (!stripQuotes)
               {
                  pvis++;
               }

               state = sSingleQuote;
            }
            else if (isSingleCharacterEscape(*q))
            {
               q++;
               lastState = state;
               state = sBackSlash;
            }
            else
            {
               ss[0] = *q;

               if (strpbrk(ss,delimitList) == NULL)
               {
                  q++;     /* delimit character not found */
                  pvis++;
               }
               else
               {
                  state = sDelimitFound;   /* delimit character found */
               }
            }

            break;
         case sDoubleQuote:  /* double quoted string */
            *pvis = *q;

            if (*q == '\0')
            {
               parseFlag = false;
            }
            else
            {
               if (*q == '"')
               {
                  if (!stripQuotes)
                  {
                     pvis++;
                  }

                  state = sLookForEnd;
               }
               else if (isSingleCharacterEscape(*q))
               {
                  lastState = state;
                  state = sBackSlash;
               }
               else
               {
                  pvis++;
               }

               q++;
            }

            break;
         case sSingleQuote:  /* single quoted string */
            *pvis = *q;

            if (*q == '\0')
            {
               parseFlag = false;
            }
            else
            {
               if (*q == '\'')
               {
                  if (!stripQuotes)
                  {
                     pvis++;
                  }

                  state = sLookForEnd;
               }
               else if (isSingleCharacterEscape(*q))
               {
                  lastState = state;
                  state = sBackSlash;
               }
               else
               {
                  pvis++;
               }

               q++;
            }

            break;
         case sBackSlash:  /* single char backslash escape */
            *pvis = *q;

            if (*q == '\0')
            {
               parseFlag = false;
            }
            else
            {
               switch(*q)
               {
                  case 'b':  *pvis = '\b';  break;
                  case 'f':  *pvis = '\f';  break;
                  case 'n':  *pvis = '\n';  break;
                  case 'r':  *pvis = '\r';  break;
                  case 't':  *pvis = '\t';  break;
               }

               state = lastState;
               pvis++;
               q++;
            }

            break;
         case sDelimitFound:  /* delimit character found  */
            *q = '\0';
            *pvis = *q;

            q++;
            state = sSkipLeading;

            /* trim off trailing blanks */
            if (trim)
            {
               for (r = p,rr = NULL;*r != '\0';r++)
               {
                  if (*r != ' ') rr = NULL;
                  else if (rr == NULL) rr = r;
               }

               if (rr != NULL) *rr = '\0';
            }

            params.Add(param);

            break;
         case sEndOfString:   /* end of string */
            /* trim off trailing blanks */

            bool addParam = (*param != '\0');

            if (trim)
            {
               for (r = p,rr = NULL;*r != '\0';r++)
               {
                  if (*r != ' ') rr = NULL;
                  else if (rr == NULL) rr = r;
               }

               if (rr != NULL) *rr = '\0';
            }

            parseFlag = false;

            if (addParam)
            {
               params.Add(param);
            }

            break;
      }
   }
  
   return(params.GetSize());
}
//_____________________________________________________________________________

int CStringSupArray::Find(CString findme)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      if (findme.Compare( this->GetAt(i) ) == 0)
         return i;
   }
   return -1;
}

int CStringSupArray::FindNoCase(CString findme)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      if (findme.CompareNoCase( this->GetAt(i) ) == 0)
         return i;
   }
   return -1;
}

//_____________________________________________________________________________
CDcaOperationProgress::CDcaOperationProgress(double operationLength)
{
   m_position           = 0.;
   m_nextUpdatePosition = 0.;
   m_length             = operationLength;

   init();
}

CDcaOperationProgress::~CDcaOperationProgress()
{
}

void CDcaOperationProgress::init()
{ 
}

void CDcaOperationProgress::setLength(double length)
{
   m_length = length;
   m_nextUpdatePosition = 0.;
}

double CDcaOperationProgress::updateProgress(double position)
{
   m_position = position;

   int pos = 0;

   if (m_length > 0.)
   {
      pos = (int)(1000. * (m_position / m_length));
   }

   m_nextUpdatePosition = ((pos + 9) / 10) * (m_length / 100.);

   return m_position;
}

double CDcaOperationProgress::updateProgress()
{
   return m_position;
}

double CDcaOperationProgress::incrementProgress(double increment)
{
   m_position += increment;

   if (m_position >= m_nextUpdatePosition)
   {
      updateProgress(m_position);
   }

   return m_position;
}

void CDcaOperationProgress::updateStatus(const CString& status)
{
}

void CDcaOperationProgress::updateStatus(const CString& status,double length,double position)
{
   updateStatus(status);

   m_length             = length;
   m_position           = position;
   m_nextUpdatePosition = position;
}

////_____________________________________________________________________________
CDcaFileReadProgress::CDcaFileReadProgress(CFile& file)
: m_CFile(&file)
, m_FILE(NULL)
{
   setLength((double)m_CFile->GetLength());
}

CDcaFileReadProgress::CDcaFileReadProgress(FILE* file)
: m_CFile(NULL)
, m_FILE(file)
{
   setLength((double)_filelength(_fileno(m_FILE)));
}

//CDcaFileReadProgress::CDcaFileReadProgress(CFileReader& fileReader)
//{
//   m_CFile        = NULL;
//   m_FILE         = NULL;
//   m_fileReader   = &fileReader;
//   m_stdioCompressedFile = NULL;
//
//   setLength((double)m_fileReader->getFileLength());
//}
//
//CDcaFileReadProgress::CDcaFileReadProgress(CStdioCompressedFile& stdioCompressedFile)
//{
//   m_CFile        = NULL;
//   m_FILE         = NULL;
//   m_fileReader   = NULL;
//   m_stdioCompressedFile = &stdioCompressedFile;
//
//   setLength((double)m_stdioCompressedFile->getFile().GetLength());
//}

double CDcaFileReadProgress::updateProgress()
{
   //extern CProgressDlg* progress;
   double position = 0.;

   if (m_CFile != NULL)
   {
      position = (double)m_CFile->GetPosition();
   }
   else if (m_FILE != NULL)
   {
      position = (double)ftell(m_FILE);
   }
   //else if (m_fileReader != NULL)
   //{
   //   position = (double)m_fileReader->getFilePosition();
   //}
   //else if (m_stdioCompressedFile != NULL)
   //{
   //   position = (double)m_stdioCompressedFile->getNumBytesInput();
   //}

   CDcaOperationProgress::updateProgress(position);

   return position;
}
