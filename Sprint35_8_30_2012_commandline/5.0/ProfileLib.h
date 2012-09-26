// $Header: /CAMCAD/4.5/ProfileLib.h 1     1/04/05 5:58p Kurt Van Ness $

/*****************************************************************************/
/*  
    Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2004. All Rights Reserved.
*/ 

#if !defined(__ProfileLib_h__)
#define __ProfileLib_h__

#pragma once

#include "Dca.h"
#include "DcaFormatStdioFile.h"
#include "DcaContainer.h"

typedef __int64 TimeIndex; 
typedef __int64 TimeSpan; 

TimeSpan getTimeFrequency();
void ReadCCTimeStampCounter(DWORD &low,long &high);
TimeIndex getTimeIndex();
double timeToMilliseconds(TimeSpan time);

//----------------------------------------------------------------------------
//
// Reference: https://www.cs.tcd.ie/Jeremy.Jones/GetCurrentProcessorNumberXP.htm
//
//  The GetCurrentProcessorNumber() function is not available in XP. 
//  Here is a VC++ version of the function that works with Windows XP on Intel x86 single, hyperthreaded, multicore 
//  and multi-socket systems. It makes use of the APIC ID returned by the CPUID instruction. 
//  This is in the range 0 .. N-1, where N is the number of logical CPUs.
//
DWORD GetCurrentProcessorNumberXP(void);

//----------------------------------------------------------------------------
// This is intended to be a debug info gathering tool, to profile execution time.

class CCamcadTimerNode : public CObject
{
private:
   TimeIndex m_timeIndx;
   CString m_message;
   DWORD m_procNum;

public:
   CCamcadTimerNode(CString msg);
   CCamcadTimerNode(CString msg1, CString msg2);

   TimeIndex GetTimeIndex()      { return m_timeIndx; }
   CString GetMessage()          { return m_message; }
   DWORD GetProcessorNumber()    { return m_procNum; }
};

//-----------------------------------

class CCamcadTimer
{
private:
   bool m_enabled;
   CTypedObArrayContainer<CCamcadTimerNode*> m_messages;

public:
   CCamcadTimer()                   { m_enabled = false; } // Requires explicit activation
   void SetEnabled(bool flag)       { m_enabled = flag; }

   void AddMessage(CString msg)                  { if (m_enabled) m_messages.Add( new CCamcadTimerNode(msg      ) ); }
   void AddMessage(CString msg1, CString msg2)   { if (m_enabled) m_messages.Add( new CCamcadTimerNode(msg1,msg2) ); }

   void Reset()                     { m_messages.empty(); }

   void DumpToFile(CString filePath);
};

CCamcadTimer &GetLicenseTimer();  // returns reference to static in camcadlicese.cpp

//----------------------------------------------------------------------------


//_____________________________________________________________________________


#endif
