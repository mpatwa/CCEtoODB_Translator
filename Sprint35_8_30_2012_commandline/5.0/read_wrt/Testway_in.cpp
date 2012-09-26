// $Header: /CAMCAD/4.5/read_wrt/Testway_in.cpp 6     10/18/05 8:13p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "testway_in.h"
#include "dft.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool CTestPlan::ReadTestWayFile(CString fileName, bool caseSensitive)
{
	CTestWayReader twReader(this, caseSensitive);

	return twReader.ReadFile(fileName);
}

bool CTestWayReader::ReadFile(CString fileName)
{
   int errorCount = 0;
   int lineCnt = 0;

   try
   {
      m_sFilename = fileName;

      CTime t;
      CString logFileName = GetLogfilePath("testway.log");
      if (m_fLog.Open(logFileName, CFile::modeCreate|CFile::modeWrite))
      {
         m_fLog.WriteString("Filename: %s\n", logFileName);
         t = t.GetCurrentTime();
         m_fLog.WriteString("%s\n\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));
      }

      if (!m_fFile.Open(m_sFilename, CFile::modeRead))
      {
         m_fLog.WriteString("Error opening %s\n", m_sFilename);
         m_fLog.Close();
         return false;
      }

      if (m_pTestPlan == NULL)
      {
         m_fLog.WriteString("Error: No valid testplan.\n");
         m_fLog.Close();
         return false;
      }


      CDFTNetConditions &netConditions = m_pTestPlan->GetNetConditions();

      CString line;
      while (m_fFile.ReadString(line))
      {
         lineCnt++;
         line.Trim();

         if (line[0] == ';')
            continue;

         CSupString supLine( line );
         CStringArray fields;
         supLine.ParseQuote(fields, ",");

         CString netName( fields.GetCount() > 0 ? fields.GetAt(0) : "" );
         CString devName( fields.GetCount() > 1 ? fields.GetAt(1) : "" );
         CString reqStrn( fields.GetCount() > 2 ? fields.GetAt(2) : "" );
         CString netType( fields.GetCount() > 3 ? fields.GetAt(3) : "" );

         // The "netname" being tested for here is the column title, so not actual data.
         // If there is a net named netname, it is going to be skipped too.
         if (netName.CompareNoCase("netname") != 0)
         {

            CString pinName;
            if (!devName.IsEmpty())
            {
               int dashDelimPos = devName.Find("-");
               if (dashDelimPos >= 0)
               {
                  int charsInPinName = devName.GetLength() - dashDelimPos - 1;
                  pinName = devName.Right(charsInPinName);
                  devName.Truncate(dashDelimPos);
               }
            }

            int reqCount = atoi(reqStrn);

            // Prior to DR 758798 it was required that all fields be present in every line.
            // But in the related CCZ, there is a net with no pins, that makes a blank "devName" cell.
            // It seems according to that data, the devName and pinName ought to be optional.
            // So blank devName and/or pinName is not considered an error now.
            // Note that we only set resource required count, no-probe, and net type.
            // The no-probe is determined by required resource count.
            // So we don't even use devName or pinName for anything, was silly to require it.
            //
            // if (netName.IsEmpty() || devName.IsEmpty() || pinName.IsEmpty() || netType.IsEmpty())

            if (netName.IsEmpty() || netType.IsEmpty())
            {
               m_fLog.WriteString("Error: Invalid line format [%d].\n", lineCnt);
               errorCount++;
               continue;
            }

            POSITION pos = NULL;
            CDFTProbeableNet *pNet = netConditions.Find_ProbeableNet(netName, pos, m_bCaseSensitive);
            if (pNet != NULL)
            {
               if (reqCount > 0)
               {
                  pNet->SetTestResourcesPerNet(reqCount);
                  pNet->SetNoProbe(false);
               }
               else
                  pNet->SetNoProbe(true);

               netType.MakeLower();
               if (netType == "power")
                  pNet->SetNetType(probeableNetTypePower);
               else if (netType == "ground")
                  pNet->SetNetType(probeableNetTypeGround);
               else
                  pNet->SetNetType(probeableNetTypeSignal);
            }
            else
            {
               m_fLog.WriteString("Error: No such net %s found [%d].\n", netName, lineCnt);
               errorCount++;
            }
         }
      }

      m_fFile.Close();

      m_fLog.WriteString("%d errors encountered.\n\n", errorCount);
      t = t.GetCurrentTime();
      m_fLog.WriteString("%s\n", t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));
      m_fLog.WriteString("Logfile closed\n");
      m_fLog.Close();

      if (errorCount)
         Logreader(logFileName);
   }
   catch(...)
   {
      errorCount++;
      CString errmsg;
      errmsg.Format("Error: Exception encountered during read at line %d.\n", lineCnt);
      m_fLog.WriteString(errmsg);
      ErrorMessage(errmsg, "TestWay Reader");
   }

	return true;
}