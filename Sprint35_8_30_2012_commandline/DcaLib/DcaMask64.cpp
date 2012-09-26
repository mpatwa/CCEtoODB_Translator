// $Header: /CAMCAD/DcaLib/DcaMask64.cpp 1     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaMask64.h"

//_____________________________________________________________________________
CMask64::CMask64()
{
   m_mask = 0ui64;
}

CMask64::CMask64(const CMask64& other)
{
   m_mask = other.m_mask;
}

CMask64::CMask64(int maskIndex)
{
   m_mask = 0ui64;

   add(maskIndex);
}

CMask64::CMask64(int maskIndex1,int maskIndex2)
{
   m_mask = 0ui64;

   add(maskIndex1);
   add(maskIndex2);
}

CMask64::CMask64(int maskIndex1,int maskIndex2,int maskIndex3)
{
   m_mask = 0ui64;

   add(maskIndex1);
   add(maskIndex2);
   add(maskIndex3);
}

CMask64::CMask64(int maskIndex1,int maskIndex2,int maskIndex3,int maskIndex4)
{
   m_mask = 0ui64;

   add(maskIndex1);
   add(maskIndex2);
   add(maskIndex3);
   add(maskIndex4);
}

CMask64::CMask64(int maskIndex1,int maskIndex2,int maskIndex3,int maskIndex4,int maskIndex5)
{
   m_mask = 0ui64;

   add(maskIndex1);
   add(maskIndex2);
   add(maskIndex3);
   add(maskIndex4);
   add(maskIndex5);
}

CMask64::~CMask64()
{
}

CMask64& CMask64::operator=(const CMask64& other)
{
   m_mask = other.m_mask;

   return *this;
}

void CMask64::add(int maskIndex)
{
   m_mask |= getBitMask(maskIndex);
}

void CMask64::add(const CMask64& other)
{
   m_mask |= other.m_mask;
}

void CMask64::addAll()
{
   m_mask = ~(0ui64);
}

void CMask64::remove(int maskIndex)
{
   m_mask &= ~getBitMask(maskIndex);
}

void CMask64::remove(const CMask64& other)
{
   m_mask &= ~(other.m_mask);
}

void CMask64::set(int maskIndex,bool value)
{
   if (value)
   {
      add(maskIndex);
   }
   else
   {
      remove(maskIndex);
   }
}

void CMask64::toggle(int maskIndex)
{
   m_mask ^= getBitMask(maskIndex);
}

void CMask64::empty()
{
   m_mask = 0;
}

bool CMask64::contains(int maskIndex) const
{
   bool retval = ((m_mask & getBitMask(maskIndex)) != 0);

   return retval;
}

bool CMask64::isEmpty() const
{
   return (m_mask == 0ui64);
}

unsigned __int64 CMask64::getBitMask(int maskIndex) const
{
   unsigned __int64 bitMask = 0ui64;

   if (maskIndex >= 0 && maskIndex < 64)
   {
      bitMask = 1ui64 << maskIndex;
   }

   return bitMask;
}
