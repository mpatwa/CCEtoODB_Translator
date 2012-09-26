/**********************************************
Copyright Mentor Graphics Corporation 2008

All Rights Reserved.

THIS WORK CONTAINS TRADE SECRET
AND PROPRIETARY INFORMATION WHICH IS THE
PROPERTY OF MENTOR GRAPHICS
CORPORATION OR ITS LICENSORS AND IS
SUBJECT TO LICENSE TERMS. 
**********************************************/
#ifndef __INC_ENCRYPT_STREAM_OUTPUT_
#define __INC_ENCRYPT_STREAM_OUTPUT_

#ifndef UNIX
#include <fstream>
using namespace std;
#else
#include <fstream.h>
#endif

#include "fileGarbler.h"


//xxx//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
class CMgcEncryptStreamOutput : public std::ofstream {

private:

   FILE *m_pFile;
   fileGarbler::cEncrypt *m_pEncryptionObject;

public:

   CMgcEncryptStreamOutput()
   {
      m_pFile= NULL; m_pEncryptionObject = NULL;
   }
   CMgcEncryptStreamOutput(const char *pFileName)
   {
      if ((m_pFile=fopen(pFileName, "wb")) == NULL) m_pEncryptionObject = NULL;
      else m_pEncryptionObject = new fileGarbler::cEncrypt(m_pFile);
   }
   ~CMgcEncryptStreamOutput() { delete m_pEncryptionObject; }

   void SetOutputStream(const char *pFileName)
   {
      if ((m_pFile=fopen(pFileName, "wb")) == NULL) m_pEncryptionObject = NULL;
      else m_pEncryptionObject = new fileGarbler::cEncrypt(m_pFile);
   }
   bool IsOpen() { return m_pFile != NULL; }
   void Close() { if (m_pFile) fclose(m_pFile); m_pFile = NULL; }
   FILE * GetFilePointer() { return m_pFile; }
   fileGarbler::cEncrypt * GetEncryptionObject() { return m_pEncryptionObject; }

   //Defined but not implemented - prevent usage and compiler/Klocwork warnings.
   CMgcEncryptStreamOutput(CMgcEncryptStreamOutput &);
   CMgcEncryptStreamOutput & operator= (const CMgcEncryptStreamOutput &);

private:

   friend CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, int n);
   friend CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, char c);
   friend CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, long n);
   friend CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, unsigned long n);
   friend CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, double d);
   friend CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, const char *str);
};

inline CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, int n)
{
   if (ostr.GetFilePointer()) ostr.GetEncryptionObject()->fprintf(ostr.GetFilePointer(), "%d", n);
   return ostr;
}
inline CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, char c)
{
   if (ostr.GetFilePointer()) ostr.GetEncryptionObject()->fprintf(ostr.GetFilePointer(), "%c", c);
   return ostr;
}
inline CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, long n)
{
   if (ostr.GetFilePointer()) ostr.GetEncryptionObject()->fprintf(ostr.GetFilePointer(), "%ld", n);
   return ostr;
}

inline CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, unsigned long n)
{
   if (ostr.GetFilePointer()) ostr.GetEncryptionObject()->fprintf(ostr.GetFilePointer(), "%lu", n);
   return ostr;
}
inline CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, double d)
{
   if (ostr.GetFilePointer())
   {
      char formatPrecision[10]; sprintf(formatPrecision, ".%d", ostr.precision());
      std::string sFormat("%f"); sFormat.insert(1, formatPrecision);
      ostr.GetEncryptionObject()->fprintf(ostr.GetFilePointer(), sFormat.c_str(), d);
   }
   return ostr;
}
inline CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, const char *str)
{
   if (!str) return ostr;
   if (ostr.GetFilePointer()) ostr.GetEncryptionObject()->fprintf(ostr.GetFilePointer(), "%s", str);
   return ostr;
}
//This one was added to get MaterialDB2HKP running.
inline CMgcEncryptStreamOutput &operator<<(CMgcEncryptStreamOutput &ostr, const std::string &str)
{
   ostr << str.c_str();
   return ostr;
}

#endif
