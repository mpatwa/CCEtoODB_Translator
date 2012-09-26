// $Header: /CAMCAD/4.5/ProfileLib.cpp 1     1/04/05 5:58p Kurt Van Ness $

#include "StdAfx.h"
#include "ProfileLib.h"

//_____________________________________________________________________________
void ReadCCTimeStampCounter(DWORD &low,long &high)
{
    DWORD L; long H;

   _asm 
   {                      
       push eax           
       push edx           
       _emit 0Fh          
       _emit 31h          
       mov DWORD PTR [L], eax 
       mov DWORD PTR [H], edx 
       pop edx            
       pop eax
   }

   low = L; high = H;
}

TimeIndex getTimeIndex() 
{
   TimeIndex timeIndex;

   ReadCCTimeStampCounter(((LARGE_INTEGER*)&timeIndex)->LowPart,((LARGE_INTEGER*)&timeIndex)->HighPart);

   return timeIndex;
}

TimeSpan getTimeFrequency() 
{
   static TimeSpan ticksPerSecond = 0;

   if (ticksPerSecond == 0)
   {
      const int numLoops = 100;
      LARGE_INTEGER freq;
      TimeSpan qpf1,qpf2,qc1,qc2;

      QueryPerformanceFrequency(&freq);
      QueryPerformanceCounter((LARGE_INTEGER*)&qpf1);
      ReadCCTimeStampCounter(((LARGE_INTEGER&)qc1).LowPart,((LARGE_INTEGER&)qc1).HighPart);

      for(int i=0;i < numLoops;i++)
      {
         Sleep(1);
      }

      QueryPerformanceCounter((LARGE_INTEGER*)&qpf2);
      ReadCCTimeStampCounter(((LARGE_INTEGER&)qc2).LowPart,((LARGE_INTEGER&)qc2).HighPart);

      __int64 qpcTicks = qpf2 - qpf1;
      __int64 qcTicks  = qc2  - qc1;
      long double ratio = ((long double)qcTicks)/((long double)qpcTicks);
      ticksPerSecond = (TimeSpan)(ratio * (long double)freq.QuadPart);
   }

   return ticksPerSecond;
}

double timeToMilliseconds(TimeSpan time)
{
   double milliSeconds = 1000. * ((double) time) / getTimeFrequency();

   return milliSeconds;
}

//####################################################################################
//##
//## Reference: https://www.cs.tcd.ie/Jeremy.Jones/GetCurrentProcessorNumberXP.htm
//##
//##  The GetCurrentProcessorNumber() function is not available in XP. 
//##  Here is a VC++ version of the function that works with Windows XP on Intel x86 single, hyperthreaded, multicore 
//##  and multi-socket systems. It makes use of the APIC ID returned by the CPUID instruction. 
//##  This is in the range 0 .. N-1, where N is the number of logical CPUs.
//##

//
// GetCurrentProcessorNumber.cpp
//
// an XP version of GetCurrentProcessorNumber()
//
// 14/11/07 first version
//
// GetCurrentProcessorNumberXP
//

DWORD GetCurrentProcessorNumberXP(void)
{
    _asm {mov eax, 1}
    _asm {cpuid}
    _asm {shr ebx, 24}
    _asm {mov eax, ebx}
}

//##
//####################################################################################

//------------------------------------------------------------------
// This timer stuff is here because it was first used here to characterize license acquisition times.
// If it becomes useful elsewhere it probably ought to be moved to a general utility place.
//

CCamcadTimerNode::CCamcadTimerNode(CString msg)
{
   m_timeIndx = getTimeIndex(); 
   m_procNum = GetCurrentProcessorNumberXP(); 
   m_message = msg;
}

CCamcadTimerNode::CCamcadTimerNode(CString msg1, CString msg2)
{
   m_timeIndx = getTimeIndex(); 
   m_procNum = GetCurrentProcessorNumberXP(); 
   m_message.Format("%-45s \t%s", msg1, msg2); 
}

void CCamcadTimer::DumpToFile(CString filePath)
{
   if (!filePath.IsEmpty())
   {
      CFormatStdioFile logFile;
      CFileException e;

      if (logFile.Open(filePath, CFile::modeCreate|CFile::modeWrite, &e))
      {
         logFile.WriteString("CAMCAD License Timer Log File\n\n");

         if (!this->m_enabled)
         {
            logFile.WriteString("License timer is disabled. Enable with CAMCAD command line parameter /LICTIME.\n\n");
         }
         else
         {
            logFile.WriteString("Times reported in milliseconds.\n");
            logFile.WriteString("First column is line number, second is running clock, third is time elapsed since previous message.\n");
            logFile.WriteString("Number in parenthesis is logical CPU number.\n");
            logFile.WriteString("Remainder of line is message chosen by programmer, usually intended to identify location and/or operation.\n\n");

            int lineCountDigits = 2;
            int lineCount = this->m_messages.GetCount();
            while ( (lineCount /= 10) > 0) lineCountDigits++;

            TimeIndex startTime;
            TimeIndex prevTime;

            for (int i = 0; i < this->m_messages.GetCount(); i++)
            {
               CCamcadTimerNode *timeI = this->m_messages.GetAt(i);

               if (i == 0)
               {
                  startTime = timeI->GetTimeIndex();
                  prevTime = startTime;
               }

               logFile.WriteString("%*d", lineCountDigits, i+1);

               logFile.WriteString("\t%11.4lf", timeToMilliseconds(timeI->GetTimeIndex() - startTime) ); // elapsed time overall
               logFile.WriteString("\t%11.4lf", timeToMilliseconds(timeI->GetTimeIndex() - prevTime)  ); // time since last msg

               logFile.WriteString("\t(%d)", timeI->GetProcessorNumber());
               logFile.WriteString("\t%s\n", timeI->GetMessage());

               prevTime = timeI->GetTimeIndex();
            }
         }

         logFile.Close();

         if (m_enabled)
            ErrorMessage(filePath, "License Timer Log File Location");
      }
   }

}
