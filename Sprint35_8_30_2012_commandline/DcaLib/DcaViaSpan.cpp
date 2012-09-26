// $Header: /CAMCAD/4.6/Vector2d.cpp 3     1/23/07 3:40p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaViaSpan.h"

//_____________________________________________________________________________
CViaSpan::CViaSpan()
{
   setPadstackName("");
   setFromLayer(0);
   setToLayer(0);
   setCapacitance("");
   setInductance("");
   setDelay("");
   setGrid("");
   setAttributes("");
}

CViaSpan::CViaSpan(CString padstackName, int fromLayer, int toLayer)
{
   setPadstackName(padstackName);
   setFromLayer(fromLayer);
   setToLayer(toLayer);
   setCapacitance("");
   setInductance("");
   setDelay("");
   setGrid("");
   setAttributes("");
}

CViaSpan::CViaSpan(const CViaSpan& viaSpan)
{
   m_padstackName = viaSpan.m_padstackName;
   m_fromLayer    = viaSpan.m_fromLayer;
   m_toLayer      = viaSpan.m_toLayer;
   m_capacitance  = viaSpan.m_capacitance;
   m_inductance   = viaSpan.m_inductance;
   m_delay        = viaSpan.m_delay;
   m_grid         = viaSpan.m_grid;
   m_attributes   = viaSpan.m_attributes;
}

CViaSpan& CViaSpan::operator=(const CViaSpan& viaSpan)
{
   if (&viaSpan != NULL)
   {
      this->m_padstackName = viaSpan.m_padstackName;
      this->m_fromLayer    = viaSpan.m_fromLayer;
      this->m_toLayer      = viaSpan.m_toLayer;
      this->m_capacitance  = viaSpan.m_capacitance;
      this->m_inductance   = viaSpan.m_inductance;
      this->m_delay        = viaSpan.m_delay;
      this->m_grid         = viaSpan.m_grid;
      this->m_attributes   = viaSpan.m_attributes;
   }

   return *this;
}

CViaSpanElementList& CViaSpanElementList::operator=(const CViaSpanElementList &other)
{
   if (&other != NULL)
   {
      this->empty();

      for (int i = 0; i < other.GetCount(); i++)
      {
         CViaSpan *sourceEl = other.GetAt(i);
         
         if (sourceEl != NULL)
         {
            CViaSpan *destEl = new CViaSpan(*sourceEl);
            this->Add(destEl);
         }
      }
   }

   return *this;
}

CViaSpansSection& CViaSpansSection::operator=(const CViaSpansSection& viaSpansSection)
{
   if (&viaSpansSection != NULL)
   {
      this->m_generalClearance   = viaSpansSection.m_generalClearance;
      this->m_sameNetClearance   = viaSpansSection.m_sameNetClearance;
      this->m_useMntOppPads      = viaSpansSection.m_useMntOppPads;
      this->m_elements           = viaSpansSection.m_elements;
   }
   return *this;
}