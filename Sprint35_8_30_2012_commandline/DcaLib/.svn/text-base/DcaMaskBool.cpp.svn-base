// $Header: /CAMCAD/DcaLib/DcaMaskBool.cpp 1     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaMaskBool.h"

//_____________________________________________________________________________
CMaskBool::CMaskBool(bool initialValue)
: m_initialValue(initialValue)
{
   m_mask.SetSize(0,20);
}

CMaskBool::CMaskBool(const CMaskBool& other)
{
   m_initialValue = other.m_initialValue;
   m_mask.SetSize(0,20);
   m_mask.Append(other.m_mask);
}

CMaskBool::CMaskBool(int maskIndex)
: m_initialValue(false)
{
   m_mask.SetSize(0,20);
   add(maskIndex);
}

CMaskBool::CMaskBool(int maskIndex1,int maskIndex2)
: m_initialValue(false)
{
   m_mask.SetSize(0,20);
   add(maskIndex1);
   add(maskIndex2);
}

CMaskBool::~CMaskBool()
{
}

CMaskBool& CMaskBool::operator=(const CMaskBool& other)
{
   if (&other != this)
   {
      m_initialValue = other.m_initialValue;
      m_mask.RemoveAll();
      m_mask.SetSize(0,20);
      m_mask.Append(other.m_mask);
   }

   return *this;
}

bool CMaskBool::operator==(const CMaskBool& other) const
{
   bool retval = (m_initialValue == other.m_initialValue);

   int maxIndex = max(m_mask.GetSize(),other.m_mask.GetSize());

   for (int index = 0;index < maxIndex && retval;index++)
   {
      retval = (contains(index) == other.contains(index));
   }

   return retval;
}

bool CMaskBool::operator!=(const CMaskBool& other) const
{
   return !(*this == other);
}

void CMaskBool::add(int maskIndex)
{
   if (maskIndex >= 0)
   {
      if (maskIndex < m_mask.GetSize() || m_initialValue == false)
      {
         m_mask.SetAtGrow(maskIndex,true);
      }
   }
}

void CMaskBool::add(const CMaskBool& other)
{
   for (int index = 0;index < other.m_mask.GetSize();index++)
   {
      if (other.m_mask.GetAt(index))
      {
         if (index < m_mask.GetSize() || m_initialValue == false)
         {
            m_mask.SetAtGrow(index,true);
         }
      }
   }
}

void CMaskBool::addAll()
{
   m_initialValue = true;

   m_mask.RemoveAll();
}

void CMaskBool::remove(int maskIndex)
{
   if (maskIndex >= 0)
   {
      if (maskIndex < m_mask.GetSize() || m_initialValue == true)
      {
         m_mask.SetAtGrow(maskIndex,false);
      }
   }
}

void CMaskBool::remove(const CMaskBool& other)
{
   for (int index = 0;index < other.m_mask.GetSize();index++)
   {
      if (other.m_mask.GetAt(index))
      {
         if (index < m_mask.GetSize() || m_initialValue == true)
         {
            m_mask.SetAtGrow(index,false);
         }
      }
   }
}

void CMaskBool::removeAll()
{
   m_initialValue = false;

   m_mask.RemoveAll();
}

bool CMaskBool::contains(int maskIndex) const
{
   bool retval = false;

   if (maskIndex >= 0)
   {
      retval = m_initialValue;

      if (maskIndex < m_mask.GetSize())
      {
         retval = m_mask.GetAt(maskIndex);
      }
   }

   return retval;
}

bool CMaskBool::isEmpty() const
{
   bool retval = !m_initialValue;

   if (retval)
   {
      for (int index = 0;index < m_mask.GetSize() && retval;index++)
      {
         retval = !m_mask.GetAt(index);
      }
   }

   return retval;
}

bool CMaskBool::isFull() const
{
   bool retval = m_initialValue;

   if (retval)
   {
      for (int index = 0;index < m_mask.GetSize() && retval;index++)
      {
         retval = m_mask.GetAt(index);
      }
   }

   return retval;
}

CString CMaskBool::getContentsString() const
{
   CString contentsString;

   for (int index = 0;index < m_mask.GetSize();index++)
   {
      if (m_mask.GetAt(index))
      {
         contentsString.AppendFormat("%s%d",(contentsString.IsEmpty() ? "" : ","),index);
      }
   }

   return contentsString;
}
