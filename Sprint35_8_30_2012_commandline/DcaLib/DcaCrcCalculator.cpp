// $Header: /CAMCAD/4.5/CrcCalculator.cpp 1     2/10/06 10:19a Kurt Van Ness $

/*
History: CrcCalculator.cpp
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 9/11/00    Time: 11:33p
 * Created in $/LibKnvPP
 * Initial add.
*/

#include "StdAfx.h"
#include "DcaCrcCalculator.h"

#ifdef USE_DEBUG_NEW
#define new DEBUG_NEW
#endif

#define BITMASK(X) (1L << (X))
#define MASK32 0xFFFFFFFFL
#define LOCAL static

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//LOCAL ulong widmask P_((p_cm_t));
LOCAL ulong widmask (p_cm_t p_cm)
/* Returns a longword whose value is (2^p_cm->cm_width)-1.     */
/* The trick is to do this portably (e.g. without doing <<32). */
{
   return (((1L<<(p_cm->cm_width-1))-1L)<<1)|1L;
}

ulong cm_tab (p_cm_t p_cm,int index)
{
   int   i;
   ulong r;
   ulong topbit = BITMASK(p_cm->cm_width-1);
   ulong inbyte = (ulong) index;

   //if (p_cm->cm_refin) inbyte = reflect(inbyte,8);

   r = inbyte << (p_cm->cm_width-8);

   for (i=0; i<8; i++)
      if (r & topbit)
         r = (r << 1) ^ p_cm->cm_poly;
      else
         r<<=1;

   //if (p_cm->cm_refin) r = reflect(r,p_cm->cm_width);

   return r & widmask(p_cm);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//________________________________________________________________________________________
CCrcCalculator::CCrcCalculator(unsigned int poly,unsigned int init,unsigned int xorOutput)
{
   m_poly      = poly;
   m_init      = init;
   m_xorOutput = xorOutput;

   m_crc                = m_init;
   m_tableValid         = false;
   m_shadowRegisterSize = 0;
   m_shadowRegister     = NULL;

   m_cmt.cm_width = 32;
   m_cmt.cm_poly  = m_poly;
   m_cmt.cm_init  = m_init;
   m_cmt.cm_init  = m_xorOutput;
   m_byteCount    = 0;
}

CCrcCalculator::~CCrcCalculator()
{
   delete [] m_shadowRegister;
}

void CCrcCalculator::reset()
{
   m_crc = m_init;
   m_byteCount = 0;
   generateTable();
}

void CCrcCalculator::setPoly(unsigned int poly)
{
   if (m_poly != poly)
   {
      m_poly       = poly;
      m_tableValid = false;
   }
}

void CCrcCalculator::setInit(unsigned int init)
{
   if (m_init != init)
   {
      m_init       = init;
      m_tableValid = false;
   }
}

void CCrcCalculator::setXorOutput(unsigned int xorOutput)
{
   if (m_xorOutput != xorOutput)
   {
      m_xorOutput  = xorOutput;
      m_tableValid = false;
   }
}

void CCrcCalculator::setShadowRegisterSize(int shadowRegisterSize)
{
   if (m_shadowRegisterSize != shadowRegisterSize)
   {
      delete [] m_shadowRegister;
      m_shadowRegister = NULL;

      if (shadowRegisterSize > 0)
      {
         m_shadowRegisterSize = shadowRegisterSize;
         m_shadowRegister = new unsigned char [m_shadowRegisterSize];
      }
      else
      {
         m_shadowRegisterSize = 0;
      }
   }
}

void CCrcCalculator::generateTable()
{
   if (!m_tableValid)
   {
      unsigned int r;

      for (int index = 0;index < CrcTableSize;index++)
      {
         r = index << 24;

         for (int i = 0;i < 8;i++)
         {
            if (r & 0x80000000)
            {
               r = (r << 1) ^ m_poly;
            }
            else
            {
               r <<= 1;
            }
         }

         m_crcTable[index] = r;

         unsigned int checkVal = cm_tab(&m_cmt,index);

         if (r != checkVal)
         {
            int iii = 3;
         }
      }

      m_tableValid = true;
   }
}

unsigned int CCrcCalculator::bitCrc(unsigned char byte)
{
   unsigned int crc1 = m_crc;
   unsigned int crc;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   crc  = crc1;
   crc1 = ((crc << 1) | (byte >> 7)) ^ (crc & 0x80000000 ? m_poly : 0);
   TRACE("crc = 0x%08x, crc1 = 0x%08x, byte = 0x%02x\n",crc,crc1,(unsigned int)byte);
   byte <<= 1;

   return crc1;
}

void CCrcCalculator::input2(const unsigned char* block,int blockLength)
{
   const unsigned char* p = block;
   int len = blockLength;

   if (m_shadowRegisterSize > 0)
   {
      while (len-- != 0)
      {
         m_crc = bitCrc(*(p++));
         m_shadowRegister[m_byteCount % m_shadowRegisterSize] = (unsigned char)(m_crc >> 24);
         m_byteCount++;
      }
   }
   else
   {
      while (len-- != 0)
      {
         m_crc = bitCrc(*(p++));
      }

      m_byteCount += blockLength;
   }
}

void CCrcCalculator::input(const unsigned char* block,int blockLength)
{
   if (!m_tableValid)
   {
      generateTable();
   }

   const unsigned char* p = block;
   int len = blockLength;

   if (m_shadowRegisterSize > 0)
   {
      while (len-- != 0)
      {
         m_crc = m_crcTable[((m_crc >> 24) ^ *(p++)) & 0xff] ^ (m_crc << 8);
         m_shadowRegister[m_byteCount % m_shadowRegisterSize] = (unsigned char)(m_crc >> 24);
         m_byteCount++;
      }
   }
   else
   {
      while (len-- != 0)
      {
         //unsigned checkCrc = bitCrc(*p);

         //int tableIndex = ((m_crc >> 24) ^ *(p++)) & 0xff;
         //unsigned int tableEntry = m_crcTable[tableIndex];
         //m_crc = tableEntry ^ (m_crc << 8);
         m_crc = m_crcTable[((m_crc >> 24) ^ *(p++)) & 0xff] ^ (m_crc << 8);
      }

      m_byteCount += blockLength;
   }
}

void CCrcCalculator::input(const CString& inputString)
{
   input((const unsigned char*)(const char*)inputString,inputString.GetLength());
}

void CCrcCalculator::input2(const CString& inputString)
{
   input2((const unsigned char*)(const char*)inputString,inputString.GetLength());
}

unsigned int CCrcCalculator::getCrc()
{
   unsigned int crcValue = m_crc ^ m_xorOutput;

   return crcValue;
}

unsigned __int64 CCrcCalculator::getCrc64()
{
   unsigned __int64 crc64value = 0,crcByte;

   for (int i = 0;i < 8;i++)
   {
      crcByte = getCrcByte(i);
      crc64value |= (crcByte << (8 * i));
   }

   return crc64value;
}

int CCrcCalculator::getNumCrcBytes()
{
   return m_shadowRegisterSize + sizeof(int);
}

unsigned char CCrcCalculator::getCrcByte(int index)
{
   unsigned char retval;
   int shadowIndex = index - sizeof(int);

   if (shadowIndex < 0)
   {
      retval = (getCrc() >> (8 * (sizeof(int) + shadowIndex))) & 0xff;
   }
   else if (shadowIndex >= m_shadowRegisterSize)
   {
      retval = 0;
   }
   else
   {
      int registerIndex = (m_byteCount + m_shadowRegisterSize - 1 - shadowIndex) % m_shadowRegisterSize;
      retval = m_shadowRegister[registerIndex];
   }

   return retval;
}










