// $Header: /CAMCAD/4.6/ViaSpan.h      8/19/08 10:12a Samrat Sharma $

#if !defined(__ViaSpan_H__)
#define __ViaSpan_H__ 

#include "DcaContainer.h"
#include "DcaData.h"
class FileStruct;

#pragma once

//_____________________________________________________________________________
class CViaSpan : public CObject
{
private:
   CString  m_padstackName;
   int      m_fromLayer;
   int      m_toLayer;
   CString  m_capacitance;
   CString  m_inductance;
   CString  m_delay;
   CString  m_grid;
   CString  m_attributes;

public:
   /* Constructors */
   CViaSpan();
   CViaSpan(CString padstackName, int fromLayer, int toLayer);
   CViaSpan(const CViaSpan& viaSpan);
   
   CViaSpan& operator=(const CViaSpan& viaSpan);

   /* Setter methods */
   void setFromLayer(int val)          { m_fromLayer     = val; }
   void setToLayer(int val)            { m_toLayer       = val; }
   void setPadstackName(CString val)   { m_padstackName  = val; }
   void setCapacitance(CString val)    { m_capacitance   = val; }
   void setInductance(CString val)     { m_inductance    = val; }
   void setDelay(CString val)          { m_delay         = val; }
   void setGrid(CString val)           { m_grid          = val; }
   void setAttributes(CString val)     { m_attributes    = val; }

   /* Getter methods */
   int getFromLayer()         { return m_fromLayer; }
   int getToLayer()           { return m_toLayer; }
   CString getPadstackName()  { return m_padstackName; }
   CString getCapacitance()   { return m_capacitance; }
   CString getInductance()    { return m_inductance; }
   CString getDelay()         { return m_delay; }
   CString getGrid()          { return m_grid; }
   CString getAttributes()    { return m_attributes; }
};

//----------------------------------------------------------------------------
class CViaSpanElementList : public CTypedObArrayContainer<CViaSpan*>
{
public:
   CViaSpanElementList& operator=(const CViaSpanElementList &other);
};

//----------------------------------------------------------------------------
class CViaSpansSection
{
   friend FileStruct;
private:
   CString m_generalClearance;
   CString m_sameNetClearance;
   CString m_useMntOppPads;

   CViaSpanElementList m_elements;

public:
   CViaSpansSection()                        { m_generalClearance = ""; m_sameNetClearance = ""; m_useMntOppPads = ""; }
   CViaSpansSection(CViaSpansSection &other) { *this = other; }

   CViaSpansSection& operator=(const CViaSpansSection &other);

   void setGeneralClearance(CString val)  { m_generalClearance = val; }
   CString getGeneralClearance() const    { return m_generalClearance; }

   void setSameNetClearance(CString val)  { m_sameNetClearance = val; }
   CString getSameNetClearance()   const  { return m_sameNetClearance; }

   void setUseMntOppPads(CString val)     { m_useMntOppPads = val; }
   CString getUseMntOppPads()   const     { return m_useMntOppPads; }

   int getNumOfElements()       const     { return (int)m_elements.GetCount(); }
   void addElement(CViaSpan *el)          { m_elements.Add(el); }
   CViaSpan *getElementAt(int i) const    { if (i >= 0 && i < m_elements.GetCount()) return m_elements.GetAt(i); else return NULL; }
   //void setElementAt(int i, CViaSpan *el)  { if (i >= 0 && i < m_numElements) m_elements.setAtGrow(i, el); }

};

#endif
