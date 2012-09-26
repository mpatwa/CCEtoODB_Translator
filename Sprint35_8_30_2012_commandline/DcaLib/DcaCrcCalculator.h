// $Header: /CAMCAD/4.5/CrcCalculator.h 1     2/10/06 10:19a Kurt Van Ness $

/*
History: CrcCalculator.h
 * 
 * *****************  Version 1  *****************
 * User: Kvanness     Date: 9/11/00    Time: 11:33p
 * Created in $/LibKnvPP
 * Initial add.
*/

#if !defined(__DcaCrcCalculator_H__)
#define __DcaCrcCalculator_H__

#pragma once

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef unsigned int ulong;

typedef struct
{
   int   cm_width;   /* Parameter: Width in bits [8,32].       */
   ulong cm_poly;    /* Parameter: The algorithm's polynomial. */
   ulong cm_init;    /* Parameter: Initial register value.     */
   bool  cm_refin;   /* Parameter: Reflect input bytes?        */
   bool  cm_refot;   /* Parameter: Reflect output CRC?         */
   ulong cm_xorot;   /* Parameter: XOR this to output CRC.     */

   ulong cm_reg;     /* Context: Context during execution.     */
} cm_t;

typedef cm_t *p_cm_t;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define CrcTableSize 256

//________________________________________________________________________________________
class CCrcCalculator
{
private:
   unsigned int m_crcTable[CrcTableSize];
   unsigned int m_crc;
   unsigned int m_poly;
   unsigned int m_init;
   unsigned int m_xorOutput;
   unsigned char* m_shadowRegister;
   int m_shadowRegisterSize;
   int m_byteCount;
   bool m_tableValid;

   cm_t m_cmt;

public:
   CCrcCalculator(unsigned int poly = 0x04c11db7,unsigned int init = 0xffffffff,unsigned int xorOutput = 0xffffffff);
   ~CCrcCalculator();

   void setPoly(unsigned int poly);
   void setInit(unsigned int init);
   void setXorOutput(unsigned int xorOutput);
   void setShadowRegisterSize(int shadowRegisterSize);
   void reset();
   void input(const unsigned char* block,int blockLength);
   void input(const CString& inputString);
   void input2(const unsigned char* block,int blockLength);
   void input2(const CString& inputString);
   unsigned int getCrc();
   unsigned __int64 getCrc64();
   int getNumCrcBytes();
   unsigned char getCrcByte(int index);

protected:
   void generateTable();
   unsigned int bitCrc(unsigned char byte);
};

#endif
