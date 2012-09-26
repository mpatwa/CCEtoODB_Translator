
#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "dft.h"
#include "Net_Util.h"
#include "CamCadDatabase.h"
#include "AeroflexNailReadWir.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)

//---------------------------------------------------------------
// ReadAeroflexNailWir
void ReadAeroflexNailWir(const char *infilename, CCEtoODBDoc *doc, FormatStruct *Format)
{
   FileStruct *file = doc->getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
   {
      ErrorMessage("Only one PCB file can be worked on at a time.\n\nThere is either zero or more than one PCB file visible.", "Aeroflex Nail Wire Reader", MB_OK);
      return;
   }

      
   CFilePath logFilePath(infilename);
   logFilePath.setBaseFileName("AeroflexNailWir");
   logFilePath.setExtension("log");

   CAeroflexNailWirReader reader(infilename, logFilePath.getPath(), doc, file);
   reader.Process();

}

//---------------------------------------------------------------
// CAeroflexNailWirReader
CAeroflexNailWirReader::CAeroflexNailWirReader(CString infilename, CString logfilename, CCEtoODBDoc *doc, FileStruct *pcbfile)
: m_inputFilename(infilename)
, m_doc(doc)
, m_pcbfile(pcbfile)
{
   if (!m_infile.Open(infilename, CFile::modeRead | CFile::typeBinary ))
	{
      m_infileIsOpen = false;
      CString tmp;
      tmp.Format("Can not open Aeroflex Nail Wir file [%s]", infilename);
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
		m_logfile.writef("# --------------------------------------------------- \n");
		m_logfile.writef("# %-51s \n", infilename);
		m_logfile.writef("# Created by CCE to ODB++ v.%-31s \n", getApp().getVersionString());
		m_logfile.writef("# %-51s \n", time.Format("%A, %B %d, %Y"));
		m_logfile.writef("# --------------------------------------------------- \n\n");
   }
   else
   {
      m_logIsOpen = false;
   }
}

CAeroflexNailWirReader::~CAeroflexNailWirReader()
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

//--------------------------------------------------------------------
void CAeroflexNailWirReader::RegisterAeroflexKeyWords()
{
   m_doc->RegisterKeyWord(ATT_WIRE, 0, valueTypeString);
   m_doc->RegisterKeyWord(ATT_TESTERINTERFACE, 0, valueTypeString);
}

void CAeroflexNailWirReader::SortplacedProbeMap(CAeroflexNailWirRecordList &recordlist)
{
   recordlist.SetSize(m_placedProbeMap.getMaxWireIndex() + 1);

   POSITION probepos = m_placedProbeMap.GetStartPosition();
   while(probepos)
   {
      CString cbNetname;
      CAeroflexNailWirRecordList *naillist = NULL;
      m_placedProbeMap.GetNextAssoc(probepos,cbNetname,naillist);
      for(int nailIndex = 0; nailIndex < naillist->GetCount(); nailIndex++)
      {
         CAeroflexNailWirRecord *nail = naillist->GetAt(nailIndex);
         if(nail)
         {
            int index = atoi(nail->GetWireIndex());
            recordlist.setAt(index,nail);

         }/*if*/
      }
   }/*while*/
}

void CAeroflexNailWirReader::WriteUpdatedProbes()
{
   // Report for updated Probes
   m_logfile.writef("# --------------------------------------------------- \n");
   m_logfile.writef("# All Updated Probes \n\n");
   if(m_cbNetlistMap.IsEmpty())
      m_logfile.writef("No probes are updated.\n\n");

   m_logfile.writef("%10s\t%16s\t%10s\t%10s\t%10s\t%10s\t%10s\n", "Wire", "Net", "TIN", "Dev.Pin", "CompPin",  "Old Probe", "New Probe");   
   CAeroflexNailWirRecordList naillist;
   SortplacedProbeMap(naillist);

   for(int nailIndex = 0; nailIndex < naillist.GetCount(); nailIndex++)
   {
      CAeroflexNailWirRecord *nail = naillist.GetAt(nailIndex);
      if(nail)
      {
         CAeroflexTestProbe* afxtestprobe = nail->GetAfxTestprobe();
         
         m_logfile.writef("%10s", nail->GetWireIndex());
         m_logfile.writef("\t%16s", nail->GetNetName());
         m_logfile.writef("\t%10s", nail->GetTesterInterfaceName());
         m_logfile.writef("\t%10s", nail->GetPinRefname());
         m_logfile.writef("\t%10s", (afxtestprobe)?afxtestprobe->getPinRefname():"None");
         m_logfile.writef("\t%10s", (afxtestprobe)?afxtestprobe->getTargetRefname():"None");
         m_logfile.writef("\t%10s", (afxtestprobe)?nail->GetTargetRefname():"None");
         m_logfile.writef("\n");
      
      }/*if*/    
   }/*for*/

   // clean the buffer size but not free the nail records
   // nail record will be cleaned later
   naillist.RemoveAll();
}

void CAeroflexNailWirReader::WriteNonUpdatedProbes(int placedflag)
{
   // Report for no updated Probes
   m_logfile.writef("\n\n# --------------------------------------------------- \n");
   m_logfile.writef("# All Non-updated %s Probes\n\n",(placedflag)?"Placed":"Unplaced");
   m_logfile.writef("%16s\t%10s\n", "Net",  "Probe");

   int placementKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeProbePlacement);

   POSITION netpos = m_cbNetlistMap.GetStartPosition();
   while(netpos)
   {
      CString cbNetname;
      CCBNetList *cbnetlist = NULL;
      m_cbNetlistMap.GetNextAssoc(netpos,cbNetname,cbnetlist);

      if(cbnetlist && !cbNetname.IsEmpty())
      {
         POSITION testprobePos = cbnetlist->getTestProbeList().GetHeadPosition();
         while(testprobePos)
         {
            CAeroflexTestProbe *testprobet = cbnetlist->getTestProbeList().GetNext(testprobePos);
            
            if(testprobet && !testprobet->isAssigned())
            {
               DataStruct *data = testprobet->getTestProbe();
               Attrib *attrib = NULL;
               CString placements = ((data && data->getAttributesRef()->Lookup(placementKW, attrib) && attrib))?attrib->getStringValue():"";

               if((!placements.CompareNoCase("Placed")) == placedflag)
               {
                  m_logfile.writef("%16s", cbNetname);
                  m_logfile.writef("\t%10s", testprobet->getTargetRefname());
                  m_logfile.writef("\n");
               }
            }
         }/*while*/
      }
   }/*while*/
  
}

void CAeroflexNailWirReader::WriteFinalReport()
{
   if (m_logIsOpen)
   {
      m_logfile.writef("\n\n# --------------------------------------------------- \n");
      m_logfile.writef("# Final Probe Results\n\n");

      WriteUpdatedProbes();

      WriteNonUpdatedProbes(true);      
      WriteNonUpdatedProbes(false);      
   }/*if*/         
}

bool CAeroflexNailWirReader::Process()
{
   if (m_infileIsOpen && m_doc != NULL && m_pcbfile != NULL)
   {
      RegisterAeroflexKeyWords();      
      Parse();

      if(BuildCBNeListMap())
      {
         UpdateTestProbesByAeroflexNailWir();
         WriteFinalReport();
      }
   }
   return false;
}

CAeroflexTestProbe* CAeroflexNailWirReader::FindAeroflexTestProbe(CAeroflexNailWirRecord *nail,CCBNetList *cbnetlist)
{
   if(!nail || !cbnetlist)
      return NULL;

   CAeroflexTestProbe* afxTestprobe = NULL;
          
   POSITION TPlistpos = cbnetlist->getTestProbeList().GetHeadPosition();
   while(TPlistpos)
   {
      CAeroflexTestProbe* curTestprobe = cbnetlist->getTestProbeList().GetNext(TPlistpos);
      if(curTestprobe && !nail->GetPinRefname().CompareNoCase(curTestprobe->getPinRefname()))
      {
         afxTestprobe = curTestprobe;
         break;
      }
   }/*while*/


   return afxTestprobe;
}

CAeroflexTestProbe* CAeroflexNailWirReader::FindUncertainAeroflexTestProbe(CAeroflexNailWirRecord *nail,CCBNetList *cbnetlist)
{
    if(!nail || !cbnetlist)
      return NULL;

   CAeroflexTestProbe* afxTestprobe = NULL;

   // If only one testprobe in the net, select that one 
   // pinname at NailRecord and Net might be different 
   if(cbnetlist->getTestProbeList().GetCount() == 1)
   {
      afxTestprobe = cbnetlist->getTestProbeList().GetHead();
      if(afxTestprobe && afxTestprobe->isAssigned())
         afxTestprobe = NULL;           
   }
   else // If more than one testprobe in the net, compare the pin refname
   {      

      // choose the first probe that was not assigned at first round
      POSITION TPlistpos = cbnetlist->getTestProbeList().GetHeadPosition();
      while(TPlistpos)
      {
         CAeroflexTestProbe* curTestprobe = cbnetlist->getTestProbeList().GetNext(TPlistpos);
         if(curTestprobe && !curTestprobe->isAssigned())
         {
            afxTestprobe = curTestprobe;
            break;
         }
      }/*while*/
   }
 
   //No probe or only prower injection probes in the net
   if(!afxTestprobe && m_logIsOpen)
   {
      m_logfile.writef("No probe found for Net:%s, Dev.Pin:%s, ICR.Pin:%s \n",
         nail->GetNetName(),nail->GetPinRefname(),nail->GetTargetRefname());
   }

   return afxTestprobe;
}

bool CAeroflexNailWirReader::UpdateTestProbesAttributes(CAeroflexNailWirRecord *nail, CAeroflexTestProbe *afxtestprobe)
{
   if(!nail || !afxtestprobe || !afxtestprobe->getTestProbe())
      return false;

   //assign testprobe to nailrecord
   nail->SetAfxTestprobe(afxtestprobe);
   afxtestprobe->setAssignedFlag(true);

   CCamCadDatabase camCadDatabase(*m_doc);
   DataStruct *data = afxtestprobe->getTestProbe();
   CString TargetRefname = nail->GetTargetRefname();

   // modified attributes
   data->getInsert()->setRefname(TargetRefname);
   CAttributes **attrbuteMap = getAttributeMap(data);

   Attrib * attrib = NULL;   
   int KeywordIndex = m_doc->getStandardAttributeKeywordIndex(standardAttributeRefName);
   if(data->getAttributesRef()->Lookup(KeywordIndex, attrib) && attrib)
   {      
      attrib->setValueFromString(TargetRefname);
      attrib->setVisible(true);
   }
   else // if no refname is assigned, create one
   {
      BlockStruct *probeBlock = m_doc->getBlockAt(data->getInsert()->getBlockNumber()); 
      
      CProbeTypeConversion probeCovert((probeBlock)?probeBlock->getName():"50",m_doc);
      ProbeTypeTag probeType = probeCovert.getProbeType();
      double drillsize = probeCovert.getProbeDrillSize((probeType != ProbeTypeTag_UNKNOWN)?probeType:ProbeTypeTag_50);
      
      CreateTestProbeRefnameAttr(m_doc, data, TargetRefname, drillsize);
      
      if(data->getAttributesRef()->Lookup(KeywordIndex, attrib) && attrib)
         attrib->setVisible(true);
   }

   camCadDatabase.addAttribute(attrbuteMap, m_doc->getStandardAttributeKeywordIndex(standardAttributeWire),nail->GetWireIndex());
   camCadDatabase.addAttribute(attrbuteMap, m_doc->getStandardAttributeKeywordIndex(standardAttributeTesterInterface),nail->GetTesterInterfaceName());

   return true;
}

void CAeroflexNailWirReader::UpdateTestProbesByAeroflexNailWir()
{
   POSITION probepos = m_placedProbeMap.GetStartPosition();
   while(probepos)
   {
      CString cbNetname;
      CAeroflexNailWirRecordList *naillist = NULL;
      m_placedProbeMap.GetNextAssoc(probepos,cbNetname,naillist);
      
      if(!cbNetname.IsEmpty() && naillist)
      {
         CCBNetList *cbnetlist = NULL;
         if(m_cbNetlistMap.Lookup(cbNetname,cbnetlist) && cbnetlist)
         {
            // First round, update the testprobes by nail record 
            // (have matching pinrefname)
            for(int nailIndex = 0; nailIndex < naillist->GetCount(); nailIndex++)
            {
               CAeroflexNailWirRecord *nail = naillist->GetAt(nailIndex);
               CAeroflexTestProbe* afxTestprobe = FindAeroflexTestProbe(nail,cbnetlist);
               UpdateTestProbesAttributes(nail,afxTestprobe);                    
            }/*for*/

            // Second round, update the uncertain testprobes by nail record 
            // (no matching pinrefname and multiple tp in then et)
            for(int nailIndex = 0; nailIndex < naillist->GetCount(); nailIndex++)
            {
               CAeroflexNailWirRecord *nail = naillist->GetAt(nailIndex);
               if(nail && !nail->GetAfxTestprobe())
               {
                  CAeroflexTestProbe* afxTestprobe = FindUncertainAeroflexTestProbe(nail,cbnetlist);
                  UpdateTestProbesAttributes(nail,afxTestprobe);                    
               }
            }/*for*/
         }
         else
         {
            if(m_logIsOpen)
               m_logfile.writef("NetName Mismatch, Net: %s \n",cbNetname);

         }
      }
   }/*while*/ 
}

CCBNetList *CAeroflexNailWirReader::getCBNetListByNet(NetStruct *net)
{
   CCBNetList *cbnetlist = NULL;
   if(net)
   {
      CAttribute *cbnetAttr = net->getAttributesRef()->lookupAttribute(standardAttributeCBNetname);
      CString cbNetname = (cbnetAttr)?cbnetAttr->getStringValue():"";
      
      if(!cbNetname.IsEmpty())
      {         
         if(!m_cbNetlistMap.Lookup(cbNetname,cbnetlist) || !cbnetlist)
         {
            cbnetlist = new CCBNetList;
            m_cbNetlistMap.SetAt(cbNetname,cbnetlist);
         }       
      }
   }

   return cbnetlist;
}

bool CAeroflexNailWirReader::LinkTestProbeToCBNetListMap()
{
   if (!m_pcbfile)
      return false;

   BlockStruct *pcbBlock = m_pcbfile->getBlock();
   int netnameKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeNetName);
   int testrsKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeTestResource);

   for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* data = insertIterator.getNext();
      if (data->isInsertType(insertTypeTestProbe))
      {
         Attrib *attrib = NULL;
         CString netname = ((data->getAttributesRef()->Lookup(netnameKW, attrib) && attrib))?attrib->getStringValue():"";
         CString testresource = ((data->getAttributesRef()->Lookup(testrsKW, attrib) && attrib))?attrib->getStringValue():"";
         
         // only testprobe can be modified
         if(!netname.IsEmpty() && testresource.MakeUpper().Find("TEST") > -1)
         {
            NetStruct *net = FindNet(m_pcbfile, netname);
            CPoint2d comppinLocation = data->getInsert()->getOrigin().getPoint2d();

            CCBNetList *cbnetlist = NULL;
            CompPinStruct *matchedPin = NULL;
            if(net)
            {
               cbnetlist = getCBNetListByNet(net);               
               matchedPin = net->findCompPin(comppinLocation);
            }

            if(cbnetlist)
            {    
               //add testprobe and refname of comppin to cbnetlist 
               CAeroflexTestProbe *testprobet = new CAeroflexTestProbe(data,(matchedPin)?matchedPin->getPinRef():"");
               cbnetlist->getTestProbeList().AddTail(testprobet);               
            }
         }/*if*/                       
      }
   }/*for*/

   return true;
}

bool CAeroflexNailWirReader::LinkNetToCBNetListMap()
{
   if(!m_pcbfile)
      return false;

   POSITION netPos = m_pcbfile->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = m_pcbfile->getNetList().GetNext(netPos);
      CCBNetList *cbnetlist = getCBNetListByNet(net);
      if(cbnetlist)
         cbnetlist->AddTail(net);
   }/*while*/

   return (m_cbNetlistMap.GetCount())?true:false;
}

bool CAeroflexNailWirReader::BuildCBNeListMap()
{   
   m_cbNetlistMap.RemoveAll();
   if(LinkNetToCBNetListMap())
      return LinkTestProbeToCBNetListMap();
   else
      ErrorMessage("Can't find Attribute CB_NETNAME. \nPlease do Aeroflex Exporter first.", "Aeroflex Nail Wire Reader", MB_OK);

   return false;
}

bool CAeroflexNailWirReader::Parse()
{
   // Get next data record, skips comment lines.
   // Note data records may span multiple lines, when so, line continuation marker is present.

   bool inPlacedProbeSection = false;
   m_infile.SeekToBegin();

   if (this->m_infileIsOpen)
   {
      // get file size 
      CFileStatus fileStatus;
      m_infile.GetStatus( fileStatus ); 

      CString buf;
      while (m_infile.ReadString(buf) || m_infile.GetPosition() < fileStatus.m_size)
      {
         buf.Trim();
         
         // Probe Section
         if(buf.Find("Wire  ATE-Connector") == 0)
         {
            inPlacedProbeSection = true;
         }
         // End of Section
         else if(buf.Find(")") == 0)
         {
            inPlacedProbeSection = false;
         }
         else // Data record
         {            
            // Probe Section
            if (inPlacedProbeSection && buf.Find("(CON") > -1)
            {               
               CAeroflexNailWirRecord *nail = new CAeroflexNailWirRecord(buf);
               CAeroflexNailWirRecordList *naillist = NULL;
               if(!m_placedProbeMap.Lookup(nail->GetNetName(),naillist) || !naillist)
               {
                  naillist = new CAeroflexNailWirRecordList;
                  m_placedProbeMap.SetAt(nail->GetNetName(),naillist);
               }/*if*/

               if(naillist)
               {
                  naillist->Add(nail);

                  int wireIndex = atoi(nail->GetWireIndex());
                  if(m_placedProbeMap.getMaxWireIndex() < wireIndex)
                     m_placedProbeMap.setMaxWireIndex(wireIndex);
               }

            }/*if*/ 
         }
      }/*while*/
   }

   return true;
}

//---------------------------------------------------------------
// CAeroflexTestProbe
CAeroflexTestProbe::CAeroflexTestProbe(DataStruct *data, CString refname)
: m_testprobe(data)
, m_pinRefname(refname)
, m_targetRefname((data)?data->getInsert()->getRefname():"")
, m_assignedFlag(false)
{
}

//---------------------------------------------------------------
// CAeroflexNailWirRecord
void CAeroflexNailWirRecord::Reset()
{
   m_dataSection = WDS_UNKNOWN;
   m_wireIndex.Empty();
   m_testerInterfaceName.Empty();
   m_pinRefname.Empty(); // Pin Number in Aeroflex terms
   m_targetRefname.Empty(); // TestProbe Refname should be placed
   m_netname.Empty();

   m_afxTestProbe = NULL;
}


void CAeroflexNailWirRecord::Set(CString recordStr)
{
   this->Reset();

   CSupString supstr(recordStr);

   CStringArray params;
   supstr.ParseQuote(params, " ");

   //Wire  
   if (params.GetCount() > 0)
      m_wireIndex = params.GetAt(0);

   //ATE-Connector  
   if (params.GetCount() > 2 && !params.GetAt(1).Find("(CON"))
   {
      m_dataSection = WDS_TESTPROBE;
      m_testerInterfaceName = params.GetAt(2);
   }

   //Dev.Pin
   if (params.GetCount() > 4 && !params.GetAt(3).Find("DEV"))
      m_pinRefname = params.GetAt(4);

   //ICR.Pin 
   if (params.GetCount() > 6 && !params.GetAt(5).Find("PIN"))
      m_targetRefname = params.GetAt(6);

   //Signal
   if (params.GetCount() > 8)
      m_netname = params.GetAt(8);

}

//---------------------------------------------------------------
// CAeroflexNailWirRecordList
CAeroflexNailWirRecordList::~CAeroflexNailWirRecordList()
{
   for(int i = 0 ; i < GetCount(); i++)
   {
      CAeroflexNailWirRecord *nail = GetAt(i);
      if(nail)
         delete nail;
   }
   RemoveAll();
}

//---------------------------------------------------------------
// CAeroflexNailWirRecordMap
CAeroflexNailWirRecordMap::CAeroflexNailWirRecordMap()
{
   m_maxWireIndex = 0;
}

CAeroflexNailWirRecordMap::~CAeroflexNailWirRecordMap()
{
   POSITION pos = GetStartPosition();
   while(pos)
   {
      CString key;
      CAeroflexNailWirRecordList *naillist = NULL;
      this->GetNextAssoc(pos,key,naillist);
      if(naillist)
         delete naillist;
   }
   RemoveAll();
}

//---------------------------------------------------------------
// CAeroflexTestProbeList
CAeroflexTestProbeList::~CAeroflexTestProbeList()
{
   POSITION pos = GetHeadPosition();
   while(pos)
   {
      CAeroflexTestProbe *testprobe = GetNext(pos);
      if(testprobe)
         delete testprobe;
   }
   RemoveAll();
}

//---------------------------------------------------------------
// CCBNetListMap
CCBNetListMap::~CCBNetListMap()
{
   POSITION pos = GetStartPosition();
   while(pos)
   {
      CString key;
      CCBNetList *cbnetlist = NULL;
      this->GetNextAssoc(pos,key,cbnetlist);
      if(cbnetlist)
         delete cbnetlist;
   }

   RemoveAll();
}