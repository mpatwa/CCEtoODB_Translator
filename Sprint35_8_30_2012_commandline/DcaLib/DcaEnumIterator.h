// $Header: /CAMCAD/DcaLib/DcaEnumIterator.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaEnumIterator_h__)
#define __DcaEnumIterator_h__

#pragma once

//_____________________________________________________________________________
template < typename EnumType >
class CEnumIterator
{
private:
   int m_min;
   int m_max;
   int m_index;

public:
   CEnumIterator(EnumType min,EnumType max);

   bool hasNext();

   EnumType getNext();
};

//_____________________________________________________________________________
template <typename EnumType>
CEnumIterator<EnumType>::CEnumIterator(EnumType min,EnumType max)
: m_min(min)
, m_max(max)
, m_index(min)
{
}

template <typename EnumType>
bool CEnumIterator<EnumType>::hasNext()
{
   return (m_index <= m_max);
}

template <typename EnumType>
EnumType CEnumIterator<EnumType>::getNext()
{
   return (EnumType)(m_index++);
}

//_____________________________________________________________________________
#define EnumIterator(tagType,variable) CEnumIterator<tagType> variable(tagType##Min,tagType##Max)

#endif
