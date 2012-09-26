
#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "dft.h"
#include "Net_Util.h"
#include "SpectrumNailreqIn.h"


// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


void ReadSpectrumNailreq(const char *infilename, CCEtoODBDoc *doc, FormatStruct *Format)
{
   FileStruct *file = doc->getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
   {
      ErrorMessage("Only one PCB file can be worked on at a time.\n\nThere is either zero or more than one PCB file visible.", "Spectrum Nailreq Reader", MB_OK);
      return;
   }

   /* PROBE PLACEMENT DOES THIS, NOT SURE IF IT SHOULD MATTER HERE
   if (file->GetCurrentMachine() != NULL )
   {
      // check to see if the select machine support DFT solution
      if (file->GetCurrentMachine()->GetDFTSolution() == NULL)
      {
         ErrorMessage("The selected Machine " + file->GetCurrentMachine()->GetName() + " does not use DFT Solution.\n", "Access Analysis");
         return;
      }
      else if (file->GetCurrentMachine()->GetDFTSolution() != file->GetCurrentDFTSolution())
      {
         // The current DFT Solution is not the same as the DFT Solution of the current machine so fix it
         file->SetCurrentDFTSolution(file->GetCurrentMachine()->GetDFTSolution());
      }
   }
   */

   // check for DFT Solutions (must exist)
#if CamCadMajorMinorVersion > 406  //  > 4.6
   //if (doc->GetCurrentDFTSolution(*file) == NULL)
#else
   if (file->GetCurrentDFTSolution() == NULL)
#endif
   {
      ErrorMessage("There is no current DFT Solution.  Run Access Analysis before importing Nailreq file.", "Spectrum Nailreq Reader", MB_OK);
      return;
   }

   
   CFilePath logFilePath(infilename);
   logFilePath.setBaseFileName("SpectrumNailreq");
   logFilePath.setExtension("log");

   CSpectrumNailreqReader reader(infilename, logFilePath.getPath(), doc, file);
   reader.Process();

}

//---------------------------------------------------------------

CSpectrumNailreqReader::CSpectrumNailreqReader(CString infilename, CString logfilename, CCEtoODBDoc *doc, FileStruct *pcbfile)
: m_inputFilename(infilename)
, m_doc(doc)
, m_pcbfile(pcbfile)
{
	if (!m_infile.Open(infilename, CFile::modeRead))
	{
      m_infileIsOpen = false;
      CString tmp;
      tmp.Format("Can not open Nailreq file [%s]", infilename);
      ErrorMessage(tmp);
	}
   else
   {
      m_infileIsOpen = true;
   }

   if (!logfilename.IsEmpty() && m_logfile.open(logfilename))
   {
      m_logIsOpen = true;

		CTime time = CTime::GetCurrentTime();
		m_logfile.writef("/* --------------------------------------------------- */\n");
		m_logfile.writef("/* %-51s */\n", infilename);
		m_logfile.writef("/* Created by CCE to ODB++ v.%-31s */\n", getApp().getVersionString());
		m_logfile.writef("/* %-51s */\n", time.Format("%A, %B %d, %Y"));
		m_logfile.writef("/* --------------------------------------------------- */\n\n");
   }
   else
   {
      m_logIsOpen = false;
   }

   m_preassigned_chan_kw = m_doc->RegisterKeyWord("SPECTRUM_PREASSIGNED_CHANNELS", 0, VT_STRING);
}

CSpectrumNailreqReader::~CSpectrumNailreqReader()
{
   if (m_infileIsOpen)
   {
      m_infile.Close();
      m_infileIsOpen = false;
   }

   if (m_logIsOpen)
   {
      m_logfile.close();
      m_logIsOpen = false;
   }
}

bool CSpectrumNailreqReader::Process()
{
   if (m_infileIsOpen && m_doc != NULL && m_pcbfile != NULL)
   {
      m_infile.SeekToBegin();
#if CamCadMajorMinorVersion > 406  //  > 4.6
      //CDFTSolution *dftSolution = m_doc->GetCurrentDFTSolution(*m_pcbfile);
#else
      CDFTSolution *dftSolution = m_pcbfile->GetCurrentDFTSolution();
#endif
      /*if (dftSolution != NULL)
      {
         CTestPlan *testPlan = dftSolution->GetTestPlan();
         if (testPlan != NULL)
         {
            CDFTNetConditions &netConditions = testPlan->GetNetConditions();

            if (netConditions.GetCount_ProbeableNets() < 1)
               netConditions.Initialize(m_doc, m_pcbfile, dftSolution);

            CNailreqRecord rec;
            while (this->GetNextRecord(rec))
            {
               // The net name in the NailReq may be a modified net name that was concocted during
               // an export by the Spectrum Writer. If everything was done properly then the modified
               // name was saved as an attribute on the net, and the ccz file saved after export.
               // Favor the attrib lookup to gind the net. If that fails then try the name
               // directly. If that fails, log an error and move on.

               NetStruct *net = FindNet(m_doc, m_pcbfile, "SPECTRUM_NETNAME", rec.getNetName());
               if (net == NULL)
                  net = FindNet(m_pcbfile, rec.getNetName());

               if (net == NULL)
               {
                  if (m_logIsOpen)
                  {
                     m_logfile.writef("Nailreq contains net \"%s\", not found in currently visible CAMCAD file.\n",
                        rec.getNetName());
                  }
               }
               else
               {
                  // Save Net Conditions settings

                  POSITION dftnetpos = NULL;
                  CDFTProbeableNet *dftnet = netConditions.Find_ProbeableNet(net->getNetName(), dftnetpos);

                  if (dftnet != NULL)
                  {
                     int numTestResources  = atoi(rec.getChannelReq());
                     int numPowerInjection = atoi(rec.getNailsReq()) - numTestResources;
                     dftnet->SetTestResourcesPerNet(numTestResources);
                     dftnet->SetPowerInjectionResourcesPerNet(numPowerInjection);
                  }
                  else
                  {
                     if (m_logIsOpen)
                     {
                        m_logfile.writef("Nailreq contains net \"%s\", not found in DFT solution probeable nets.\n",
                           net->getNetName());
                     }
                  }

                  // Save Pre-Assigned Channels settings

                  if (net != NULL)
                  {
                     if (!rec.getChanNumber().IsEmpty())
                     { 
#if CamCadMajorMinorVersion > 406  //  > 4.6
                        net->setAttrib(m_doc->getCamCadData(), m_preassigned_chan_kw, valueTypeString, rec.getChanNumber().GetBuffer(0), attributeUpdateOverwrite, NULL);
#else
                        net->setAttrib(m_doc, m_preassigned_chan_kw, valueTypeString, rec.getChanNumber().GetBuffer(0), attributeUpdateOverwrite, NULL);
#endif
                     }
                     else
                     {
                        // One way to look at it is to make sure  there is no attrib, but
                        // this has the effect of causing the exporter to not know that
                        // nailreq was imported, and so seeing no preassigned channels
                        // the spectrum writer will use teh .out file settings.
                        // I percieve that as not good.
                        //if (net->getAttributesRef() != NULL)
                        //   net->getAttributesRef()->deleteAttribute(m_preassigned_chan_kw);

                        // So instead let's try setting a special "None" value.
                        // All "good" preassigned channel values are white space separated
                        // string of integers, easy to tell from "None".
#if CamCadMajorMinorVersion > 406  //  > 4.6
                        net->setAttrib(m_doc->getCamCadData(), m_preassigned_chan_kw, valueTypeString, "None", attributeUpdateOverwrite, NULL);
#else
                        net->setAttrib(m_doc, m_preassigned_chan_kw, valueTypeString, "None", attributeUpdateOverwrite, NULL);
#endif
                     }
                  }
               }
            }

            netConditions.UpdateNetsInFile(m_pcbfile, m_doc);
         }
      }*/
   }

   return false;
}

bool CSpectrumNailreqReader::GetNextRecord(CString &recordRetval)
{
   // Get next data record, skips comment lines.
   // Note data records may span multiple lines, when so, line continuation marker is present.

   recordRetval.Empty();
   bool retval = false;

   if (this->m_infileIsOpen)
   {
      // Lines ending in "\" are continued on next line
      bool lineIsContinued = true;
      
      do
      {
         CString buf;
         if (retval = (m_infile.ReadString(buf)?true:false))
         {
            if (buf.GetLength() > 1 && buf.Left(2).Compare("//") == 0)  //  Example file has  comment lines that  start with //
            {
               // skip comment
            }
            else
            {
               buf.Trim();
               recordRetval += buf;

               if (recordRetval.GetLength() > 0 && recordRetval.Right(1).Compare("\\") == 0)
               {
                  lineIsContinued = true;
                  recordRetval.TrimRight("\\");
               }
               else
               {
                  lineIsContinued = false;
               }
            }
         }
      } while (lineIsContinued && retval);
   }

   return retval;
}

bool CSpectrumNailreqReader::GetNextRecord(CNailreqRecord &recordRetval)
{
   CString recordStr;

   if (this->GetNextRecord(recordStr))
   {
      recordRetval.set(recordStr);
      return true;
   }

   return false;
}



//---------------------------------------------------------------

void CNailreqRecord::reset()
{
   m_netname.Empty();
   m_channelreq.Empty();
   m_nailsreq.Empty();
   m_dscanwire.Empty();
   m_sensewire.Empty();
   m_drivelevel.Empty();
   m_channumber.Empty();
   m_conditions.Empty();
   m_relatedps.Empty();
}


void CNailreqRecord::set(CString recordStr)
{
   this->reset();

   CSupString supstr(recordStr);

   CStringArray params;
   supstr.ParseQuote(params, ",;");

   if (params.GetCount() > 0)
      m_netname = params.GetAt(0);

   if (params.GetCount() > 1)
      m_channelreq = params.GetAt(1);

   if (params.GetCount() > 2)
      m_nailsreq = params.GetAt(2);

   if (params.GetCount() > 3)
      m_dscanwire = params.GetAt(3);

   if (params.GetCount() > 4)
      m_sensewire = params.GetAt(4);

   if (params.GetCount() > 5)
      m_drivelevel = params.GetAt(5);

   if (params.GetCount() > 6)
      m_channumber = params.GetAt(6);

   if (params.GetCount() > 7)
      m_conditions = params.GetAt(7);

   if (params.GetCount() > 8)
      m_relatedps = params.GetAt(8);
}


//---------------------------------------------------------------
