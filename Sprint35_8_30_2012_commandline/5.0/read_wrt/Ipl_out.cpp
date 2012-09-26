// $Header: /CAMCAD/4.6/read_wrt/Ipl_out.cpp 86    5/15/07 1:15p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

// IPL_OUT.CPP

#include "stdafx.h"
#include "ipl_out.h"
#include "tmstate.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "pcbutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Teradyne Z1800 Writer


/******************************************************************************
* IPL_WriteFiles
*/
void IPL_WriteFiles(const char *filename, CCEtoODBDoc &Doc, FormatStruct *format, double UnitsFactor)
{
   try
   {
      CIplWriter ipl(Doc, true, filename);

      // output all file info
      ipl.WriteFiles(filename);
   }
   catch (CString exception)
   {
      ErrorMessage(exception, "Teradyne Z18xx IPL Write", MB_OK);
   }
}


/******************************************************************************
* EChannelSpacingToInteger
*/
int EChannelSpacingToInteger(EChannelSpacing channelSpacing);
int EChannelSpacingToInteger(EChannelSpacing channelSpacing) 
{ 
   switch(channelSpacing)
   {
   case ChannelSpacing16:
      return 16;
   case ChannelSpacing32:
      return 32;
   case ChannelSpacing1:
      return 1;
   }

   return 16;
}

/******************************************************************************
* CIplSortedArray::Add
*/
INT_PTR CIplSortedArray::Add(SORTTYPE newElement)
{
   if (GetCount() == 0)
   {
      CArray<SORTTYPE>::InsertAt(0, newElement);

      return 0;
   }

   int i;
   for (i=0; i<GetCount(); i++)
   {
      if (CArray<SORTTYPE>::GetAt(i) >= newElement)
         break;
   }

   // add it to the end of the list
   CArray<SORTTYPE>::InsertAt(i, newElement);
   return i;
}


/******************************************************************************
* CIplSortedArray::GetSmallestValue
*/
long CIplSortedArray::GetSmallestValue()
{
   SORTTYPE smallestValue = LONG_MAX;

   for (int i=0; i<GetCount(); i++)
   {
      SORTTYPE curValue = GetAt(i);

      // if current is smaller, use it
      if (smallestValue > curValue)
         smallestValue = curValue;
   }

   return smallestValue;
}

/******************************************************************************
* CIplSortedArray::GetSmallestValueString
*/
CString CIplSortedArray::GetSmallestValueString()
{
   CString buf;
   SORTTYPE smallestValue = LONG_MAX;

   for (int i=0; i<GetCount(); i++)
   {
      SORTTYPE curValue = GetAt(i);

      // if current is smaller, use it
      if (smallestValue > curValue)
         smallestValue = curValue;
   }

   buf.Format("%d", smallestValue);
   return buf;
}

/******************************************************************************
* CIplSortedArray::GetDelimitedString
*/
CString CIplSortedArray::GetDelimitedString(CString delimiter, bool compress, bool avoidDuplicates)
{
   CString buf, tempBuf;
   SORTTYPE lastValue = 0, startValue = 0;

   for (int i=0; i<GetCount(); i++)
   {
      SORTTYPE curValue = GetAt(i);
      if (i == 0)
      {  // Set the initial values
         startValue = lastValue = curValue;
         continue;
      }

      // avoid duplicates
      if (avoidDuplicates && curValue == lastValue)
         continue;

      // keep track of the sequential items
      if (curValue == lastValue + 1 && compress)
      {
         lastValue = curValue;
         continue;  // continue until non-sequential item is found
      }

      if (compress && startValue < lastValue)
      {
         if (startValue < lastValue - 1)
            tempBuf.Format("%d-%d%s", startValue, lastValue, delimiter);
         else
            tempBuf.Format("%d%s%d%s", startValue, delimiter, lastValue, delimiter);
      }
      else
         tempBuf.Format("%d%s", startValue, delimiter);

      buf += tempBuf;
      startValue = lastValue = curValue;
   }

   if (startValue >= 0)
   {
      if (compress && startValue < lastValue)
      {
         if (startValue < lastValue - 1)
            tempBuf.Format("%d-%d", startValue, lastValue);
         else if (startValue < lastValue)
            tempBuf.Format("%d%s%d", startValue, delimiter, lastValue);
      }
      else
         tempBuf.Format("%d", startValue);
   }
   else
      return "";

   buf += tempBuf;

   CString unassignedPairStr;
   unassignedPairStr.Format("%s&%s", CHNL_UNASSIGNED_STR, CHNL_UNASSIGNED_STR);
   buf.Replace(unassignedPairStr, CHNL_UNASSIGNED_STR);

   return buf;
}


/******************************************************************************
/******************************************************************************
* CChannel::CChannel
*/
CChannel::CChannel(long chnlNum)
{
   avoidList.RemoveAll();
   channelNumber = chnlNum;
   used = false;
   reserved = false;
}

/******************************************************************************
* CChannel::AddToAvoid
*/
void CChannel::AddToAvoid(CChannel *channel)
{
   if (channel == this)
      return;

   POSITION pos = avoidList.GetHeadPosition();
   while (pos)
   {
      CChannel *curChannel = avoidList.GetNext(pos);

      // we don't need to add it again if it's already in the list
      if (channel == curChannel)
         return;
   }

   avoidList.AddTail(channel);
}

/******************************************************************************
*/
bool CChannel::CanUse(CChannel *channel, EChannelSpacing channelSpacing)
{
   CString ignoredErrMsg;
   return CanUse(channel, channelSpacing, ignoredErrMsg);
}

/******************************************************************************
*/
bool CChannel::CanUse(CChannel *channel, EChannelSpacing channelSpacing, CString &errMsg)
{
   errMsg = "";

   if (reserved)
   {
      errMsg.Format("Channel %d is reserved.", this->GetChannelNumber());
      return false;
   }

   POSITION pos = avoidList.GetHeadPosition();
   while (pos)
   {
      CChannel *curChannel = avoidList.GetNext(pos);
      if (abs(channel->GetChannelNumber() - curChannel->GetChannelNumber()) < EChannelSpacingToInteger(channelSpacing))
      {
         errMsg.Format("Insufficent spacing between channels %d and %d, required spacing: %d.",
            channel->GetChannelNumber(), curChannel->GetChannelNumber(), EChannelSpacingToInteger(channelSpacing));

         return false;
      }
   }

   return true;
}

/******************************************************************************
* CChannel::Dump
*/
CString CChannel::Dump(bool header)
{
   CString buf;
   CIplSortedArray sortedArray;

   POSITION pos = avoidList.GetHeadPosition();
   while (pos)
   {
      CChannel *channel = avoidList.GetNext(pos);
      sortedArray.Add(channel->GetChannelNumber());
   }

   buf.Format("%4d %5s %5s [%s]", channelNumber, used?"True":"False", reserved?"True":"False", sortedArray.GetDelimitedString(",", false));

   return buf;
}




/******************************************************************************
* CChannelNumberArray::CChannelNumberArray
*/
CChannelNumberArray::CChannelNumberArray() : CArray<CChannel*>()
{
   channelSpacing = ChannelSpacing16;
   doRemapping = false;
}

/******************************************************************************
* CChannelNumberArray::~CChannelNumberArray
*/
CChannelNumberArray::~CChannelNumberArray()
{
   for (int i=0; i<GetCount(); i++)
      delete GetAt(i);
   RemoveAll();
}

/******************************************************************************
* CChannelNumberArray::GetChannelSpacingNum
*/
long CChannelNumberArray::GetChannelSpacingNum()
{
   return EChannelSpacingToInteger(channelSpacing);
}

/******************************************************************************
* CChannelNumberArray::GetChannelSpacing
*/
EChannelSpacing CChannelNumberArray::GetChannelSpacing()
{
   return channelSpacing;
}

/******************************************************************************
* CChannelNumberArray::SetChannelSpacing
*/
void CChannelNumberArray::SetChannelSpacing(EChannelSpacing chnlSpacing)
{
   channelSpacing = chnlSpacing;
}

/******************************************************************************
* CChannelNumberArray::AddNewChannel
*/
CChannel *CChannelNumberArray::AddNewChannel(long newChannelNumber)
{
   CChannel *newChannel = NULL;

   if (newChannelNumber < GetCount())
      newChannel = GetAt(newChannelNumber);

   // create the channel if we need to
   if (newChannel == NULL)
   {
      newChannel = new CChannel(newChannelNumber);
      SetAtGrow(newChannel->GetChannelNumber(), newChannel);
   }

   return newChannel;
}

/******************************************************************************
* CChannelNumberArray::GetFirstAvailableChannel
*/
CChannel *CChannelNumberArray::GetFirstAvailableChannel()
{
   return GetNextAvailableChannel();
}

/******************************************************************************
* CChannelNumberArray::GetNextAvailableChannel
*/
CChannel *CChannelNumberArray::GetNextAvailableChannel(CChannel *channel)
{
   long startChannelNumber = CHNL_STARTNUMBER;
   if (channel != NULL)
      startChannelNumber = channel->GetChannelNumber()+1;

   for (long i=startChannelNumber; i<(long)GetCount(); i++)
   {
      CChannel *curChannel = GetAt(i);

      // if there isn't a channel here, create it to be used
      if (curChannel == NULL)
      {
         curChannel = new CChannel(i);
         SetAt(curChannel->GetChannelNumber(), curChannel);
      }

      if (!curChannel->IsUsed() && !curChannel->IsReserved())
         return curChannel;
   }

   // add to the end of the list
   long newChannelNumber = GetCount();

   // don't use channel 0
   //if (newChannelNumber == 0)
   // newChannelNumber = 1;

   CChannel *newChannel = new CChannel(newChannelNumber);
   SetAtGrow(newChannel->GetChannelNumber(), newChannel);

   return newChannel;
}
/******************************************************************************
* CChannelNumberArray::GetNextAvailableChannel
*/
CChannel *CChannelNumberArray::GetHighestUsedChannel()
{
	CChannel *highestChannel = NULL;

   long startChannelNumber = CHNL_STARTNUMBER;

   for (long i=startChannelNumber; i<(long)GetCount(); i++)
   {
      CChannel *curChannel = GetAt(i);

      if (curChannel != NULL && curChannel->IsUsed())
		{
			if (highestChannel == NULL || 
				curChannel->GetChannelNumber() > highestChannel->GetChannelNumber())
			{
				highestChannel = curChannel;
			}
		}
   }

   return highestChannel;
}

/******************************************************************************
* CChannelNumberArray::AddPreassignedChannel
*/
void CChannelNumberArray::AddPreassignedChannel(CString netName, long channelNumber)
{
   CArray<int> *channelAry;
   if (preassignedChannels.Lookup(netName, channelAry))
   {
      // Existing net, add channel to existing list
      channelAry->Add(channelNumber);
   }
   else
   {
      // New net, make new entry
      channelAry = new CArray<int>;
      channelAry->Add(channelNumber);

      preassignedChannels.SetAt(netName, channelAry);
   }
}

/******************************************************************************
* CChannelNumberArray::GetPreassignedChannels
*/
CString CChannelNumberArray::GetPreassignedChannels()
{
   POSITION pos = NULL;
   CString netname;
   CArray<int> *channelAry;
   CString buf, buf2, resString;

   pos = preassignedChannels.GetStartPosition();
   while (pos != NULL)
   {
      preassignedChannels.GetNextAssoc(pos, netname, channelAry);

      for (int i = 0; i < channelAry->GetCount(); i++)
      {
         int channelNum = channelAry->GetAt(i);
         buf.Format("/* .PREASSIGNED_CHANNEL %d ", channelNum);
         buf2.Format("%-*s */\n", 51-buf.GetLength()+3, netname);
         buf += buf2;

         resString += buf;
      }
   }

   return resString;
}

/******************************************************************************
*/
bool CChannelNumberArray::HasPreassignedChannelAttribs(CCEtoODBDoc *doc, FileStruct *file)
{
   if (doc != NULL && file != NULL)
   {
      WORD kw = doc->RegisterKeyWord("SPECTRUM_PREASSIGNED_CHANNELS", 0, VT_STRING);

      POSITION netpos = file->getHeadNetPosition();
      while (netpos != NULL)
      {
         NetStruct *net = file->getNextNet(netpos);
         Attrib *attr = NULL;
         if (net->lookUpAttrib(kw, attr))
         {
            // That's all it takes, any one net having the attrib, regardless of value
            return true;
         }
      }
   }
         
   return false;
}

/******************************************************************************
*/
void CChannelNumberArray::SetPreassignedChannels(CCEtoODBDoc *doc, FileStruct *file, CIplNetMap *netMap, FILE *logFile)
{

   if (HasPreassignedChannelAttribs(doc, file))
      SetPreassignedChannelsFromAttribs(doc, file, netMap, logFile);
   else
      SetPreassignedChannelsFromOutFileSettings(netMap, logFile);

}

/******************************************************************************
*/
void CChannelNumberArray::SetPreassignedChannelsFromAttribs(CCEtoODBDoc *doc, FileStruct *file, CIplNetMap *netMap, FILE *logFile)
{
   if (doc != NULL && file != NULL)
   {
      WORD kw = doc->RegisterKeyWord("SPECTRUM_PREASSIGNED_CHANNELS", 0, VT_STRING);

      POSITION netpos = file->getHeadNetPosition();
      while (netpos != NULL)
      {
         NetStruct *net = file->getNextNet(netpos);
         CIplNet *iplNet = netMap->Lookup(net->getNetName());

         Attrib *attr = NULL;
         if (net->lookUpAttrib(kw, attr) && iplNet != NULL)
         {
            CSupString channelList = get_attvalue_string(doc, attr);
            CStringArray channelAry;
            channelList.ParseWhite(channelAry);

            if (channelAry.GetCount() > 0 && channelAry.GetAt(0).CompareNoCase("None") != 0)
            {
               CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();

               for (int i = 0; i < channelAry.GetCount(); i++)
               {
                  CString channelStr = channelAry.GetAt(i);
                  int channelNum = atoi(channelStr);

                  if (iplTP != NULL)
                  {
                     CString errMsg;
                     CChannel *channel = AddNewChannel(channelNum);
                     if (iplNet->SetProbeChannelNumber(channel, iplTP, GetChannelSpacing(), errMsg))
                     {
                        channel->SetReserved();
                     }
                     else
                     {
                        if (logFile != NULL)
                           fprintf(logFile, "Warning:  Cannot assign preassigned channel %d to %s.  %s\n",
                           channelNum, net->getNetName(), errMsg);
                     }
                  }
                  else
                  {
                     if (logFile != NULL)
                        fprintf(logFile, "Warning:  Cannot assign preassigned channel %d to %s.  Not enough test probes on this net.\n",
                        channelNum, net->getNetName());
                  }

                  if (iplTP != NULL)
                     iplTP = iplNet->GetNextTestProbe(iplTP);

               }
            }
         }
      }
   }
}

/******************************************************************************
*/
void CChannelNumberArray::SetPreassignedChannelsFromOutFileSettings(CIplNetMap *netMap, FILE *logFile)
{
   // loop through the nets to make sure they are enough channel numbers apart on a single net  
   POSITION pos = preassignedChannels.GetStartPosition();
   while (pos)
   {
      CString netname;
      CArray<int> *channelAry;

      preassignedChannels.GetNextAssoc(pos, netname, channelAry);

      CIplNet *iplNet = netMap->Lookup(netname);
      if (iplNet == NULL)
      {
         for (int i = 0; i < channelAry->GetCount(); i++)
         {
            int channelNumber = channelAry->GetAt(i);
            if (logFile != NULL)
               fprintf(logFile, "Warning:  Cannot assign preassigned channel %d to %s.  No such net.\n",
                  channelNumber, netname);
         }
      }
      else
      {
         CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();
         if (iplTP == NULL)
         {
            for (int i = 0; i < channelAry->GetCount(); i++)
            {
               int channelNumber = channelAry->GetAt(i);
               if (logFile != NULL)
                  fprintf(logFile, "Warning:  Cannot assign preassigned channel %d to %s.  No test probe on this net.\n",
                  channelNumber, netname);
            }
         }
         else
         {
            for (int i = 0; i < channelAry->GetCount(); i++)
            {
               int channelNumber = channelAry->GetAt(i);
               if (iplTP != NULL)
               {
                  CString errMsg;
                  CChannel *channel = AddNewChannel(channelNumber);
                  if (iplNet->SetProbeChannelNumber(channel, iplTP, GetChannelSpacing(), errMsg))
                  {
                     channel->SetReserved();
                  }
                  else
                  {
                     if (logFile != NULL)
                        fprintf(logFile, "Warning:  Cannot assign preassigned channel %d to %s.  %s\n",
                        channelNumber, netname, errMsg);
                  }
               }
               else
               {
                  if (logFile != NULL)
                     fprintf(logFile, "Warning:  Cannot assign preassigned channel %d to %s.  Not enough test probes on this net.\n",
                     channelNumber, netname);
               }

               if (iplTP != NULL)
                  iplTP = iplNet->GetNextTestProbe(iplTP);
            }
         }
      }
   }
}

/******************************************************************************
* CChannelNumberArray::GetReservedChannels
*/
CString CChannelNumberArray::GetReservedChannels()
{
   CIplSortedArray array; 
   CString resChannels;
   for (int i=1; i<GetCount(); i++)
   {
      CChannel *channel = GetAt(i);
      
      if (channel != NULL && channel->IsReserved())
         array.Add(channel->GetChannelNumber());
   }

   return array.GetDelimitedString(",", true);
}

/******************************************************************************
* CChannelNumberArray::SetReservedChannels
*/
void CChannelNumberArray::SetReservedChannels(CString reservedChannels)
{
   CString resChannels = reservedChannels;
   int pos = 0;
   
   pos = resChannels.FindOneOf(",-");
   if (pos == 0)
      resChannels = resChannels.Mid(1);

   pos = resChannels.FindOneOf(",-");
   while (pos > 0)
   {
      if (resChannels[pos] == ',')
      {
         int channelNumber = atoi(resChannels.Mid(0, pos));
         
         CChannel *channel = AddNewChannel(channelNumber);
         channel->SetReserved();

         resChannels = resChannels.Mid(pos+1);
      }
      else if(resChannels[pos] == '-')
      {
         long start = 0, end = 0;

         start = atoi(resChannels.Mid(0, pos));
         resChannels = resChannels.Mid(pos+1);
         pos = resChannels.FindOneOf(",");
         if (pos > 0)
            end = atoi(resChannels.Mid(0, pos));
         else
            end = atoi(resChannels);

         for (int i=start; i<=end; i++)
         {
            CChannel *channel = AddNewChannel(i);
            channel->SetReserved();
         }

         if (pos > 0)
            resChannels = resChannels.Mid(pos+1);
      }

      pos = resChannels.FindOneOf(",-");
   }
}

/******************************************************************************
* CChannelNumberArray::Dump
*/
void CChannelNumberArray::Dump(CFormatStdioFile *file)
{
   file->WriteString("\n-----------------------List of Probes-----------------------\n");

   for (int i=CHNL_STARTNUMBER; i<GetCount(); i++)
   {
      CChannel *channel = GetAt(i);
      if (channel != NULL)
         file->WriteString("%s\n", channel->Dump());
      else
         file->WriteString("%4d No Channel\n", i);
   }
}



/******************************************************************************
/******************************************************************************
* CIplTestProbe::CIplTestProbes
*/
CIplTestProbe::CIplTestProbe(CCEtoODBDoc *doc, DataStruct *testProbe)
{
   m_camCadDoc = NULL;
   m_testProbeData = m_testAccessPointData = NULL;

   //m_probedType = entityTypeUndefined;
   //m_pProbedItem = NULL;

   m_channel = NULL;
   m_name = "NO_PROBE";

   if (doc == NULL || testProbe == NULL)
      return;

   if (testProbe->getDataType() != T_INSERT)
      return;

   if (testProbe->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)
      return;

   EEntityType eType = entityTypeUndefined;
   Attrib *attrib = NULL;
   //VOID *voidPtr = NULL;
   int linkEntity = 0;

   m_camCadDoc = doc;
   m_testProbeData = testProbe;
   m_name = m_testProbeData->getInsert()->getRefname();

   // get the net we are probing
   int netKW = m_camCadDoc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);

   if (attrib = get_attvalue(m_testProbeData->getAttributesRef(), netKW))
      m_netName = get_attvalue_string(m_camCadDoc, attrib);

   // get the entity of the test access point the test probe is linked to
   int linkKW = m_camCadDoc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);

   if (!(attrib = get_attvalue(m_testProbeData->getAttributesRef(), linkKW)))
      return;

   CEntity accessEntity = CEntity::findEntity(m_camCadDoc->getCamCadData(), attrib->getIntValue());
   
   if (accessEntity.getEntityType() == entityTypeData)
   {
      m_testAccessPointData = accessEntity.getData();
   }

   if (!(attrib = get_attvalue(m_testAccessPointData->getAttributesRef(), linkKW)))
      return;

   m_probedEntity = CEntity::findEntity(m_camCadDoc->getCamCadData(), attrib->getIntValue());
}

CIplTestProbe::CIplTestProbe(CIplTestProbe *other)
{
   this->m_camCadDoc = other->m_camCadDoc;
   this->m_channel = other->m_channel;
   this->m_name = other->m_name;
   this->m_netName = other->m_netName;
   this->m_probedEntity = other->m_probedEntity;
   this->m_testAccessPointData = other->m_testAccessPointData;
   this->m_testProbeData = other->m_testProbeData;
}

/******************************************************************************
* CIplTestProbe::GetChannelNumber
*/
long CIplTestProbe::GetChannelNumber()
{
   long retVal = CHNL_UNASSIGNED;

   retVal = (m_channel != NULL) ? m_channel->GetChannelNumber() : CHNL_UNASSIGNED;

   return retVal;
}

/******************************************************************************
* CIplTestProbe::GetProbeBlockNumber
*/
int CIplTestProbe::GetProbeBlockNumber()
{    
   return  m_testProbeData->getInsert()->getBlockNumber();
}

/******************************************************************************
* CIplTestProbe::SetChannel
*/
void CIplTestProbe::SetChannel(CChannel *newChannel)
{
   if (m_testProbeData == NULL)
      return;

   m_channel = newChannel;
   m_channel->SetUsed();
}

/******************************************************************************
* CIplTestProbe::GetGridLocation
*/
CString CIplTestProbe::GetGridLocation()
{
   if (m_testProbeData != NULL)
   {
      Attrib* attrib = NULL;
      WORD gridlocKW = m_camCadDoc->RegisterKeyWord(ATT_GRID_LOCATION, 0, VT_STRING);
      if (m_testProbeData->getAttributesRef() != NULL && m_testProbeData->getAttributesRef()->Lookup(gridlocKW, attrib))
      {
         CString temp = get_attvalue_string(m_camCadDoc, attrib);
         if (!temp.IsEmpty())
            return temp;
      }
   }

   return "XXX";  // XXX is what was output before grid location support was added, case 2239

}

/******************************************************************************
* CIplTestProbe::isOnTop
*/
bool CIplTestProbe::IsOnTop()
{ 
   if (m_testProbeData != NULL)
   {
      if (m_testProbeData->getInsert()->getMirrorFlags() && MIRROR_LAYERS)
         return false;
      else
         return true;
   }
   else
      return true;
}

bool CIplTestProbe::IsPlaced()
{
   if (m_testProbeData != NULL && m_testProbeData->getAttributes() != NULL)
   {
      Attrib* attrib = NULL;
      WORD placedKw = m_camCadDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);

      if (m_testProbeData->getAttributes()->Lookup(placedKw, attrib) && attrib != NULL)
      {
         if (attrib->getStringValue().CompareNoCase("Placed") == 0)
            return true;
      }
   }

   return false;
}

/******************************************************************************
* CIplTestProbe::GetProbedItemType
*/
InsertTypeTag CIplTestProbe::GetProbedItemType() 
{
   //if (m_probedType != entityTypeData || m_pProbedItem == NULL)
   if (m_probedEntity.getEntityType() != entityTypeData)
   {
      return insertTypeUnknown;
   }
   else
   {
      //DataStruct* data = (DataStruct*)m_pProbedItem;
      DataStruct* data = m_probedEntity.getData();

      //if (data == NULL || data->getInsert() == NULL)
      //   return insertTypeUnknown;
      //else
      //   return data->getInsert()->getInsertType();
      if (data->getDataType() != dataTypeInsert)
      {
         return insertTypeUnknown;
      }
      else
      {
         return data->getInsert()->getInsertType();
      }
   }
}

/******************************************************************************
* GetProbedItemXLocation
*/
double CIplTestProbe::GetProbedItemXLocation()
{
   //if (m_pProbedItem == NULL)
   //   return 0.0;

   //else if (m_probedType == entityTypeCompPin)
   //   return ((CompPinStruct*)m_pProbedItem)->getOriginX();

   //else if (m_probedType == entityTypeData)
   //   return ((DataStruct*)m_pProbedItem)->getInsert()->getOriginX();

   //else 
   //   return 0.0;

   double retval = 0.;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getOriginX();
   }
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = m_probedEntity.getData()->getInsert()->getOriginX();
      }
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::GetProbedItemYLocation
*/
double CIplTestProbe::GetProbedItemYLocation()
{
   //if (m_pProbedItem == NULL)
   //   return 0.0;

   //else if (m_probedType == entityTypeCompPin)
   //   return ((CompPinStruct*)m_pProbedItem)->getOriginY();

   //else if (m_probedType == entityTypeData)
   //   return ((DataStruct*)m_pProbedItem)->getInsert()->getOriginY();

   //else
   //   return 0.0;

   double retval = 0.;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getOriginY();
   }
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = m_probedEntity.getData()->getInsert()->getOriginY();
      }
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::GetProbedItemRotation
*/
double CIplTestProbe::GetProbedItemRotationDegrees()
{
   //if (m_pProbedItem == NULL)
   //   return 0.0;

   //else if (m_probedType == entityTypeCompPin)
   //   return ((CompPinStruct*)m_pProbedItem)->getRotationDegrees();

   //else if (m_probedType == entityTypeData)
   //   return ((DataStruct*)m_pProbedItem)->getInsert()->getAngleDegrees();

   //else
   //   return 0.0;

   double retval = 0.;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getRotationDegrees();
   }
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = m_probedEntity.getData()->getInsert()->getAngleDegrees();
      }
   }

   return retval;
}

double CIplTestProbe::GetProbedItemRotationRadians()
{
   //if (m_pProbedItem == NULL)
   //   return 0.0;

   //else if (m_probedType == entityTypeCompPin)
   //   return ((CompPinStruct*)m_pProbedItem)->getRotationRadians();

   //else if (m_probedType == entityTypeData)
   //   return ((DataStruct*)m_pProbedItem)->getInsert()->getAngle();

   //else
   //   return 0.0;

   double retval = 0.;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getRotationRadians();
   }
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = m_probedEntity.getData()->getInsert()->getAngleRadians();
      }
   }

   return retval;
}

CString CIplTestProbe::GetProbedItemBlockName()
{
   //if (m_pProbedItem == NULL)
   //   return "";

   //else if (m_probedType == entityTypeCompPin)
   //{  
   //   BlockStruct* block = m_camCadDoc->getBlockAt(((CompPinStruct*)m_pProbedItem)->getPadstackBlockNumber());
   //   return block->getName();
   //}
   //else if (m_probedType == entityTypeData)
   //{
   //   BlockStruct* block = m_camCadDoc->getBlockAt(((DataStruct*)m_pProbedItem)->getInsert()->getBlockNumber());
   //   return block->getName();
   //}
   //else
   //   return "";

   CString retval;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      BlockStruct* block = m_camCadDoc->getBlockAt(m_probedEntity.getCompPin()->getPadstackBlockNumber());
      retval = block->getName();
   }
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         BlockStruct* block = m_camCadDoc->getBlockAt(m_probedEntity.getData()->getInsert()->getBlockNumber());
         if(block != NULL)
         {
            if(m_probedEntity.getData()->isInsertType(insertTypeBondPad))
               block = GetBondPadPadstackBlock(m_camCadDoc->getCamCadData(), block);     

            retval = block->getName();
         }
      }
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::IsProbedDiePin
*/
bool CIplTestProbe::IsProbedDiePin()
{
   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      return m_probedEntity.getCompPin()->IsDiePin(m_camCadDoc->getCamCadData());
   }
   return false;
}

/******************************************************************************
* CIplTestProbe::IsProbedItemSMD
*/
bool CIplTestProbe::IsProbedItemSMD()
{
   bool retval = true;
   WORD technologyKW = (WORD)m_camCadDoc->RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);
   Attrib* attrib = NULL;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      CompPinStruct* comppin = m_probedEntity.getCompPin();
      
      if (comppin->getAttributes() != NULL)
         comppin->getAttributes()->Lookup(technologyKW, attrib);
   }
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      DataStruct* via = m_probedEntity.getData();

      if (via->getAttributes() != NULL)
         via->getAttributes()->Lookup(technologyKW, attrib);
   }

   if (attrib != NULL)
   {
      retval = (attrib->getStringValue().CompareNoCase("THRU") != 0);
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::IsProbedItemOnTop
*/
bool CIplTestProbe::IsProbedItemOnTop()
{
   //if (m_pProbedItem == NULL)
   //   return false;

   //else if (m_probedType == entityTypeCompPin)
   //   return (((CompPinStruct*)m_pProbedItem)->getMirror())?false:true;

   //else if (m_probedType == entityTypeData)
   //   return (((DataStruct*)m_pProbedItem)->getInsert()->getMirrorFlags() && MIRROR_LAYERS)?false:true;
   //else
   //   return false;

   bool retval = false;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = (m_probedEntity.getCompPin()->getMirror() ? false : true);
   }
   else if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = ((m_probedEntity.getData()->getInsert()->getMirrorFlags() && MIRROR_LAYERS) ? false : true);
      }
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::IsPowerInjection
*/
bool CIplTestProbe::IsPowerInjection()
{
   bool retval = false;
   WORD testSourceKW = (WORD)m_camCadDoc->getStandardAttributeKeywordIndex(standardAttributeTestResource);
   Attrib* attrib = NULL;

   if (m_testProbeData->getAttributes() != NULL && m_testProbeData->getAttributes()->Lookup(testSourceKW, attrib) && attrib != NULL)
   {
      CString value = attrib->getStringValue(); 
      if (value.CompareNoCase("Power Injection") == 0)
         retval = true;
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::GetPinName
*/
CString CIplTestProbe::GetProbedItemPinName()
{
   CString retval;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getPinName();
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::GetPinNumber
*/
int CIplTestProbe::GetProbedItemPinNumber()
{
   int retval = 0;

   if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      int pinNrKW = m_camCadDoc->RegisterKeyWord(ATT_COMPPINNR, 0, VT_INTEGER);

      Attrib *attrib = NULL;

      if (attrib = get_attvalue(m_probedEntity.getCompPin()->getAttributesRef(), pinNrKW))
      {
         retval = attrib->getIntValue();
      }
      else
      {
         retval = atoi(m_probedEntity.getCompPin()->getPinName());
      }
   }

   return retval;
}

/******************************************************************************
*/
CString CIplTestProbe::GetProbedItemRefName()
{
   //if (m_probedType != EN_DATA || m_pProbedItem == NULL)
   //   return "";

   //return ((DataStruct*)m_pProbedItem)->getInsert()->getRefname();

   CString retval;

   if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = m_probedEntity.getData()->getInsert()->getRefname();
      }
   }
   else if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getRefDes();
   }

   return retval;
}

/******************************************************************************
*/
CString CIplTestProbe::GetProbedItemSortableRefName()
{
   //if (m_probedType != EN_DATA || m_pProbedItem == NULL)
   //   return "";

   //return ((DataStruct*)m_pProbedItem)->getInsert()->getRefname();

   CString retval;

   if (m_probedEntity.getEntityType() == entityTypeData)
   {
      if (m_probedEntity.getData()->getDataType() == dataTypeInsert)
      {
         retval = m_probedEntity.getData()->getInsert()->getSortableRefDes();
      }
   }
   else if (m_probedEntity.getEntityType() == entityTypeCompPin)
   {
      retval = m_probedEntity.getCompPin()->getSortablePinRef();
   }

   return retval;
}

/******************************************************************************
* CIplTestProbe::BackAssignChannelNumber
*/
void CIplTestProbe::BackAssignChannelNumber()
{
   if (m_channel == NULL)
      return;

   SetRefname(m_channel->GetChannelNumber());

   Attrib* attrib = NULL;
   WORD refnameKw = m_camCadDoc->RegisterKeyWord(ATT_REFNAME, 0, VT_STRING);

   if (m_testProbeData->getAttributes() != NULL && m_testProbeData->getAttributes()->Lookup(refnameKw, attrib) && attrib != NULL)
   {
      attrib->setStringValueIndex(m_camCadDoc->RegisterValue(this->GetName()));    
   }
}

/******************************************************************************
* CIplTestProbe::Dump
*/
CString CIplTestProbe::Dump(bool header)
{
   CString buf;

   if (header)
   {
      return " TP[EntNum] ->            TA[EntNum] -> (DataTyp)RefName[EntNum] in NetName[CNum]";
   }
   else
   {
      //switch (m_probedType)
      switch (m_probedEntity.getEntityType())
      {
      case entityTypeData:
         {
            DataStruct* data = m_probedEntity.getData();

            if (data->getDataType() == dataTypeInsert)
            {
               buf.Format("%3s[%6d] -> %13s[%6d] -> (   Data)%7s[%6d] in %7s[%4d]",
                  m_testProbeData->getInsert()->getRefname(), 
                  m_testProbeData->getEntityNumber(),
                  m_testAccessPointData->getInsert()->getRefname(), 
                  m_testAccessPointData->getEntityNumber(),
                  data->getInsert()->getRefname(), 
                  data->getEntityNumber(), 
                  m_netName,
                  (m_channel != NULL) ? m_channel->GetChannelNumber() : CHNL_UNASSIGNED);
            }
         }

         break;
      case entityTypeCompPin:
         {
            CompPinStruct* compPin = m_probedEntity.getCompPin();
            CString compPinStr = compPin->getRefDes() + "," + compPin->getPinName();

            buf.Format("%3s[%6d] -> %13s[%6d] -> (     CP)%7s[%6d] in %7s[%4d]",
               m_testProbeData->getInsert()->getRefname(), 
               m_testProbeData->getEntityNumber(),
               m_testAccessPointData->getInsert()->getRefname(), 
               m_testAccessPointData->getEntityNumber(),
               compPinStr, 
               compPin->getEntityNumber(), 
               m_netName,
               (m_channel != NULL) ? m_channel->GetChannelNumber() : CHNL_UNASSIGNED);
         }

         break;
      default:
         buf = "Unknown type";
      }
   }

   return buf;
}

/******************************************************************************
/******************************************************************************
* CIplTestProbeList::BackAssignChannelNumbers
*/
void CIplTestProbeList::BackAssignChannelNumbers()
{
   POSITION pos = GetHeadPosition();
   while (pos)
   {
      CIplTestProbe *iplTP = GetNext(pos);

      iplTP->BackAssignChannelNumber();
   }
}

/******************************************************************************
* CIplTestProbeList::Dump
*/
void CIplTestProbeList::Dump(CFormatStdioFile *file)
{
   bool firstProbe = true;

   file->WriteString("\n-----------------------List of Probes-----------------------\n");

   POSITION pos = GetHeadPosition();
   while (pos)
   {
      CIplTestProbe *iplTP = GetNext(pos);

      if (firstProbe)
         file->WriteString("%s\n", iplTP->Dump(true));

      file->WriteString("%s\n", iplTP->Dump());
      firstProbe = false;
   }
}



/******************************************************************************
/******************************************************************************
* CIplPin::CIplPin
*/
CIplPin::CIplPin(CCEtoODBDoc *doc, CString pinNm, DataStruct *data, int pinNum, PinTypeTag type, CString netNm)
{
   pDoc = doc;
   pinName = pinNm;
   pinNumber = pinNum;
   pinType = type;
   netName = netNm;
   pPinData = data;

   if (pinNumber >= 0)
      pinNumber = atoi(pinName);

   sortName = makeSortName(pinName);
}

/******************************************************************************
* CIplPin::makeSortName
*/
CString CIplPin::makeSortName(CString name)
{
   // create sort name
   CString temp, buf;
   bool alpha = true, lastAlpha = true;

   name.MakeUpper();
   for (int i=0; i<name.GetLength(); i++)
   {
      if (isalpha(name[i]))
         alpha = true;
      else if (isdigit(name[i]))
         alpha = false;
      else
         continue;

      if (i == 0)
      {
         lastAlpha = alpha;
         temp = name[i];
         continue;
      }

      if (lastAlpha != alpha)
      {
         if (lastAlpha)
         {
            // add string
            temp = (CString)"AAAAAAAAAAAA" + temp;
            temp.Delete(0, temp.GetLength() - 12);
            buf += temp;
         }
         else
         {
            // add number
            temp = (CString)"000000000000" + temp;
            temp.Delete(0, temp.GetLength() - 12);
            buf += temp;
         }

         temp.Empty();
      }
      temp += name[i];
      lastAlpha = alpha;
   }

   if (lastAlpha)
   {
      // add string
      temp = (CString)"AAAAAAAAAAAA" + temp;
      temp.Delete(0, temp.GetLength() - 12);
      buf += temp;
   }
   else
   {
      // add number
      temp = (CString)"000000000000" + temp;
      temp.Delete(0, temp.GetLength() - 12);
      buf += temp;
   }

   return buf;
}

/******************************************************************************
* CIplPin::IsSMD
*/
bool CIplPin::IsSMD()
{
   if (pPinData == NULL)
      return true;

   if (pPinData->getInsert()->getBlockNumber() < 0 || pPinData->getInsert()->getBlockNumber() >= pDoc->getMaxBlockIndex())
      return true;

   WORD smdKW = pDoc->RegisterKeyWord(ATT_SMDSHAPE, 0, VT_NONE);

   Attrib* attrib;

   if (!pPinData->getAttributesRef() || !pPinData->getAttributesRef()->Lookup(smdKW, attrib))
      return false;
   else
      return true;
}

/******************************************************************************
* CIplPin::Dump
*/
CString CIplPin::Dump()
{
   CString buf;

   buf.Format("Pin# %2d(%5d) - PinName: %s[SortName: %s] (%s) on %s",
      pinNumber, (pPinData != NULL)?pPinData->getEntityNumber():0, pinName, sortName, pinTypeTagToValueString(pinType), netName);

   return buf;
}



/******************************************************************************
/******************************************************************************
* CIplComponent::CIplComponent
*/
CIplComponent::CIplComponent(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile)
: m_descriptionAttribName(descAttrName)
, m_iplTestElement(iplTestElement)
, m_insertedCompGeom(NULL)
{
   pDoc = doc;
   componentID = compName;
   pComponentData = data;
   m_sensorNumber = -1;
   m_compHeight = 0.0;
   pins.RemoveAll();

   m_deviceType = deviceTypeUnknown;     // device type through device typer
   if (devType >= deviceTypeUnknown && devType < deviceTypeUndefined)
      m_deviceType = devType;

   m_insertedCompGeom = doc->getBlockAt( data->getInsert()->getBlockNumber() );

   description = LookupDescription();

   if (Attrib *a =  is_attvalue(pDoc, data->getAttributesRef(), ATT_COMPHEIGHT, 0))
   {
      CString str = get_attvalue_string(pDoc, a);
      m_compHeight = atof(str);
   }

   if (description.IsEmpty())
   {
      switch (m_deviceType)
      {
      case deviceTypeJumper:
      case deviceTypeFuse:
         // J       ID,DESC,PIN
         description = "Jumper";
         break;
      case deviceTypeTransistor:
      case deviceTypeTransistorNpn:
         // QN      ID,DESC,BASE,EMIT,COLL      GNPN    ID,DESC,VAL1,VAL2,BASE,EMIT,COLL  // include Gain tests base on user options per Transistor
         // QP      ID,DESC,BASE,EMIT,COLL      GPNP    ID,DESC,VAL1,VAL2,BASE,EMIT,COLL  // include Gain tests base on user options per Transistor
         description = "Transistor";
         break;
      case deviceTypeDiode:
      case deviceTypeDiodeLed:
         // D       ID,DESC,ANODE,CATHODE
         description = "Diode";
         break;
      case deviceTypeNoTest:
      default:
         // DCHG    ID,DESC
         ;
      }
   }

   if (m_deviceType == deviceTypeTransistor && logFile != NULL)
      fprintf(logFile, "Warning:  %s[%d] - Unknown type: %s\n",
         componentID, pComponentData->getEntityNumber(), deviceTypeTagToValueString(deviceTypeTransistor));
   pins.setSortFunction(&CIplComponent::PinSortFunction);
}

/******************************************************************************
* CIplComponent::~CIplComponent
*/
CIplComponent::~CIplComponent()
{
   POSITION pos = pins.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplPin *iplPin = NULL;

      pins.GetNextAssoc(pos, key, (CObject*&)iplPin);

      delete iplPin;
   }

   pins.RemoveAll();

   if (m_iplTestElement != NULL)
      delete m_iplTestElement;
}

/******************************************************************************
* CIplComponent::getPinList
*/
CString CIplComponent::getPinList(CIplNetMap *netMap, FILE *logFile)
{
   CIplSortedArray sortedArray;
   long channelNumber = CHNL_UNASSIGNED;
   CString pinList;

   if (m_deviceType == deviceTypeTransistorNpn || m_deviceType == deviceTypeTransistorPnp ||
       m_deviceType == deviceTypeDiode         || m_deviceType == deviceTypeDiodeLed      ||
       m_deviceType == deviceTypeDiodeZener)
   { 
      // find the starting and ending enumerator 
      PinTypeTag startType = pinTypeUnknown;
      PinTypeTag endType = pinTypeUnknown;
      if (m_deviceType == deviceTypeTransistorNpn || m_deviceType == deviceTypeTransistorPnp)
      {
         startType = pinTypeBase;
         endType = pinTypeCollector;
      }
      else if (m_deviceType == deviceTypeDiode || m_deviceType == deviceTypeDiodeLed || 
               m_deviceType == deviceTypeDiodeZener)
      {
         startType = pinTypeAnode;
         endType = pinTypeCathode;
      }

      // loop through the pin types we are looking for
      for (int pinType=startType; pinType<=endType; pinType++)
      {
         // find a pin with the current type
         CIplPin *iplPin = GetFirstPin();
         while (iplPin != NULL)
         {
            if (iplPin->GetPinType() == pinType)
               break;

            iplPin = GetNextPin();
         }

         if (iplPin == NULL)
         {
            int pinNumber = pinType - startType + 1;
            if (logFile != NULL)
            {
               fprintf(logFile, "Warning:  %s[%d] - No %s pin found.  Using pin %d.\n", 
                  componentID, pComponentData->getEntityNumber(), pinTypeTagToValueString((PinTypeTag)pinType), pinNumber);
            }
            // get the pin we need base on which pin we are trying to find
            //   Base - Pin 1
            //   Emitter - Pin 2
            //   Collector - Pin 3
            iplPin = GetFirstPin();
            for (int pinCount=1; pinCount<pinNumber; pinCount++)
               iplPin = GetNextPin();
         }

         // add the channel number of the net the pin is on to the pin list
         CIplNet *iplNet = netMap->Lookup(iplPin->GetNetName());
         if (iplNet == NULL)
            pinList += CHNL_UNASSIGNED_STR;
         else
         {
            sortedArray.RemoveAll();

            // add all the test probes
            CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();
            while (iplTP != NULL)
            {
               // Case dts0100460827 says use channel number regardless of placed or unplaced
               //channelNumber = iplTP->IsPlaced() ? iplTP->GetChannelNumber() : CHNL_UNASSIGNED;
               channelNumber = iplTP->GetChannelNumber();
               sortedArray.Add(channelNumber);

               iplTP = iplNet->GetNextTestProbe(iplTP);
            }

            pinList += sortedArray.GetCount() > 0 ? sortedArray.GetSmallestValueString() : CHNL_UNASSIGNED_STR;
         }

         if (pinType != endType)
            pinList += ",";
      }
   }
   else
   {
      pinList.Empty();

      CIplPin *iplPin = GetFirstPin();
      while (iplPin != NULL)
      {
         CIplNet *iplNet = netMap->Lookup(iplPin->GetNetName());
         if (iplNet == NULL)
            pinList += CHNL_UNASSIGNED_STR; // add unplaced probe representation
         else
         {
            sortedArray.RemoveAll();
            CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();

            if (m_deviceType == deviceTypeIC       || 
                m_deviceType == deviceTypeICLinear || 
                m_deviceType == deviceTypeICDigital)
            {
               CString buf;
               channelNumber = iplTP != NULL ? iplTP->GetChannelNumber() : CHNL_UNASSIGNED;
               buf.Format("%d", channelNumber);
               pinList += buf;
            }
            else
            {
               while (iplTP != NULL)
               {
                  // Case dts0100460827 says use channel number regardless of placed or unplaced
                  //channelNumber = iplTP->IsPlaced() ? iplTP->GetChannelNumber() : CHNL_UNASSIGNED;
                  channelNumber = iplTP->GetChannelNumber();
                  sortedArray.Add(channelNumber); 

                  iplTP = iplNet->GetNextTestProbe(iplTP);
               }

               if (m_deviceType == deviceTypeResistor || m_deviceType == deviceTypeInductor || m_deviceType == deviceTypeCapacitor || 
                   m_deviceType == deviceTypeCapacitorPolarized || m_deviceType == deviceTypeCapacitorTantalum)
                   pinList += sortedArray.GetCount() > 0 ? sortedArray.GetDelimitedString("&", false) : CHNL_UNASSIGNED_STR;
               else
                  pinList += sortedArray.GetCount() > 0 ? sortedArray.GetSmallestValueString() : CHNL_UNASSIGNED_STR;
            }
         }

         CString delimiter = m_deviceType == deviceTypeJumper ? "&" : ",";
         if (iplPin = GetNextPin())
         {
            if (pinList[pinList.GetLength()-1] != delimiter[0])
               pinList += delimiter;
         }
      }

      if (pinList.IsEmpty() && logFile != NULL)
         fprintf(logFile, "Warning:  %s[%d] - No pins found.\n", componentID, pComponentData->getEntityNumber());
   }

    return pinList;
}

/******************************************************************************
* CIplComponent::populateAvoidLists
*/
void CIplComponent::populateAvoidLists(CIplNetMap *netMap)
{
   // loop through all the pins
   POSITION pos = pins.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplPin *iplPin = NULL;

      // get the pin
      pins.GetNextAssoc(pos, key, (CObject*&)iplPin);

      // get the net on this pin
      CIplNet *iplNet = netMap->Lookup(iplPin->GetNetName());

      // loop through all the test probes on the net
      CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();
      while (iplTP != NULL)
      {
         CChannel *channel = iplTP->GetChannel();

         if (channel == NULL)
         {
            iplTP = iplNet->GetNextTestProbe(iplTP);
            continue;
         }

         // loop through all subsequent pins
         POSITION pos2 = pos;
         while (pos2)
         {
            CString key2;
            CIplPin *iplPin2 = NULL;

            pins.GetNextAssoc(pos2, key2, (CObject*&)iplPin2);

            CIplNet *iplNet2 = netMap->Lookup(iplPin2->GetNetName());
            CIplTestProbe *iplTP2 = iplNet2->GetFirstTestProbe();
            while (iplTP2 != NULL)
            {
               CChannel *channel2 = iplTP2->GetChannel();
               if (channel2 != NULL)
               {
                  // set each channel to avoid each other
                  channel->AddToAvoid(channel2);
                  channel2->AddToAvoid(channel);
               }

               iplTP2 = iplNet2->GetNextTestProbe(iplTP2);
            }
         }

         iplTP = iplNet->GetNextTestProbe(iplTP);
      } // END while (iplTP != NULL) ...
   } // END while (pos) ...
}

/******************************************************************************
* CIplComponent::SetPinChannelNumber
*/
bool CIplComponent::setPinChannelNumber(CChannel *channel, CIplTestProbe *iplTestProbe, CIplNetMap *netMap, CChannelNumberArray *channelStats)
{
   if (channel == NULL || iplTestProbe == NULL)
      return false;

   // check to see if we can assign this new channel
   CChannel *tpChannel = iplTestProbe->GetChannel();
   if (tpChannel != NULL)
   {
      // check based on the current channel's avoid list
      if (!tpChannel->CanUse(channel, channelStats->GetChannelSpacing()))
         return false;

      tpChannel->SetUnused();
   }
   else
   {
      // there was no previous channel, so we need to check all the pins
      CString *key = NULL;
      CIplPin *iplPin = NULL;
      pins.GetFirstSorted(key, (CObject*&)iplPin);
      while (iplPin != NULL)
      {
         CIplNet *iplNet = netMap->Lookup(iplPin->GetNetName());

         // check the spacing between the test probes
         CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();
         while (iplTP != NULL)
         {
            CChannel *curChannel = iplTP->GetChannel();
            if (curChannel == NULL)
            {
               iplTP = iplNet->GetNextTestProbe(iplTP);
               continue;
            }

            // no need to process the same test probe
            if (iplTP == iplTestProbe)
            {
               iplTP = iplNet->GetNextTestProbe(iplTP);
               continue;
            }

            // check to see if the channel numbers are set apart far enough
            if (abs(channel->GetChannelNumber() - curChannel->GetChannelNumber()) < channelStats->GetChannelSpacingNum())
               return false;

            iplTP = iplNet->GetNextTestProbe(iplTP);
         }

         pins.GetNextSorted(key, (CObject*&)iplPin);
      }
   }

   iplTestProbe->SetChannel(channel);

   // set the channels this channel must avoid
   CString *key = NULL;
   CIplPin *iplPin = NULL;
   pins.GetFirstSorted(key, (CObject*&)iplPin);
   while (iplPin != NULL)
   {
      CIplNet *iplNet = netMap->Lookup(iplPin->GetNetName());

      // set the test probe's avoid channels
      CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();
      while (iplTP != NULL)
      {
         CChannel *curChannel = iplTP->GetChannel();
         if (curChannel == NULL)
         {
            iplTP = iplNet->GetNextTestProbe(iplTP);
            continue;
         }

         // no need to process the same test probe
         if (iplTP == iplTestProbe)
         {
            iplTP = iplNet->GetNextTestProbe(iplTP);
            continue;
         }

         // set the avoid list for each other
         channel->AddToAvoid(curChannel);
         curChannel->AddToAvoid(channel);

         iplTP = iplNet->GetNextTestProbe(iplTP);
      }

      pins.GetNextSorted(key, (CObject*&)iplPin);
   }

   return true;
}

/******************************************************************************
* CIplComponent::AddPin
*/
void CIplComponent::AddPin(CString pinNm, int pinNum, PinTypeTag pinType, CString netNm)
{
   CIplPin *iplPin = NULL;

   if (pins.Lookup(pinNm, ((CObject*&)iplPin)))
   {
      iplPin->SetPinNumber(pinNum);
      iplPin->SetNetName(netNm);
      iplPin->SetPinType(pinType);
   }
   else
   {
      if (pComponentData == NULL)
         return;

      if (pComponentData->getInsert()->getBlockNumber() < 0 || pComponentData->getInsert()->getBlockNumber() >= pDoc->getMaxBlockIndex())
         return;

      BlockStruct *block = pDoc->getBlockAt(pComponentData->getInsert()->getBlockNumber());
      if (block == NULL)
         return;
      
      // find the pin data
      DataStruct *pinData = NULL;
      POSITION pos = block->getDataList().GetHeadPosition();
      while (pos)
      {
         pinData = block->getDataList().GetNext(pos);

         if (pinData->getDataType() != T_INSERT)
            continue;

         if (pinData->getInsert()->getInsertType() != INSERTTYPE_PIN)
            continue;

         if (pinNm == pinData->getInsert()->getRefname())
            break;
      }

      iplPin = new CIplPin(pDoc, pinNm, pinData, pinNum, pinType, netNm);
      pins.SetAt(pinNm, iplPin);
   }

}

/******************************************************************************
* CIplComponent::GetSubClass
*/
CString CIplComponent::GetSubClass()
{
   CString subClass;
   Attrib* attrib = NULL;
   WORD subClassKW = pDoc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);

   if (pComponentData->getAttributes() == NULL || !pComponentData->getAttributes()->Lookup(subClassKW, attrib) || attrib == NULL)
      subClass = deviceTypeTagToValueString(this->m_deviceType);
   else
   {
      subClass = attrib->getStringValue();
   }

   return subClass;
}

/******************************************************************************
* CIplComponent::GetSubClass
*/
CString CIplComponent::GetSubClassAttribute()
{
   CString subClass;
   Attrib* attrib = NULL;
   WORD subClassKW = pDoc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);

   if (pComponentData != NULL && pComponentData->getAttributes() != NULL && pComponentData->getAttributes()->Lookup(subClassKW, attrib))
   {
      if (attrib != NULL)
      {
         subClass = attrib->getStringValue();
      }
   }

   return subClass;
}

/******************************************************************************
* CIplComponent::GetStringAttribute
*/
CString CIplComponent::GetAttribute(CString attribName)
{
   CString attrVal;

   if (!attribName.IsEmpty() && pComponentData != NULL && pComponentData->getAttributes() != NULL)
   {
      Attrib* attrib = NULL;
      WORD kw = pDoc->RegisterKeyWord(attribName, 0, VT_STRING);

      if (pComponentData->getAttributes()->Lookup(kw, attrib))
      {
         if (attrib != NULL)
         {
            attrVal = attrib->getStringValue();
         }
      }
   }

   return attrVal;
}

/******************************************************************************
*/
CString CIplComponent::LookupDescription()
{
   CString desc = "";

	if (pComponentData && pComponentData->getAttributes() && !m_descriptionAttribName.IsEmpty())
   {
      Attrib *attrib = NULL;
      int kw = pDoc->IsKeyWord(m_descriptionAttribName, 0);
      if (kw >= 0 && pComponentData->getAttributesRef()->Lookup(kw, attrib))
      {
         if (attrib)
         {
            desc = get_attvalue_string(pDoc, attrib);
         }
      }
   }


   return desc;
}

/******************************************************************************
* CIplComponent::GetPartNumber
*/
CString CIplComponent::GetPartNumber()
{
   CString partNumber = "";

   if (pComponentData && pComponentData->getAttributes())
   {
      Attrib *attrib = NULL;
      int kwPartNumber = pDoc->IsKeyWord(ATT_PARTNUMBER, 0);
      if (kwPartNumber >= 0 && pComponentData->getAttributesRef()->Lookup(kwPartNumber, attrib))
      {
         if (attrib)
         {
            partNumber = get_attvalue_string(pDoc, attrib);
         }
      }
   }


   return partNumber;
}

/******************************************************************************
* CIplComponent::GetCapacitiveOpens
*/
CString CIplComponent::GetCapacitiveOpens()
{
   CString capopens = "FALSE";

   if (pComponentData && pComponentData->getAttributes())
   {
      Attrib *attrib = NULL;
      int kw = pDoc->IsKeyWord(ATT_CAP_OPENS, 0);
      if (kw >= 0 && pComponentData->getAttributesRef()->Lookup(kw, attrib))
      {
         if (attrib)
         {
            capopens = get_attvalue_string(pDoc, attrib);
         }
      }
   }

   return capopens;
}

/******************************************************************************
* CIplComponent::GetDiodeOpens
*/
CString CIplComponent::GetDiodeOpens()
{
   CString diodeopens = "FALSE";

   if (pComponentData && pComponentData->getAttributes())
   {
      Attrib *attrib = NULL;
      int kw = pDoc->IsKeyWord(ATT_DIODE_OPENS, 0);
      if (kw >= 0 && pComponentData->getAttributesRef()->Lookup(kw, attrib))
      {
         if (attrib)
         {
            diodeopens = get_attvalue_string(pDoc, attrib);
         }
      }
   }

   return diodeopens;
}

/******************************************************************************
* CIplComponent::GetPlusTolerance
*/
CString CIplComponent::GetPlusTolerance(CString attribName)
{
   ValueTypeTag valType = valueTypeString;

   int kwIndx = pDoc->IsKeyWord(attribName, 0);
   if (kwIndx > -1)
   {
      const KeyWordStruct *kwStrct = pDoc->getKeyword(kwIndx);
      if (kwStrct != NULL)
         valType = kwStrct->getValueType();
   }

   CString plusTol = "";
   Attrib* attrib = NULL;
   WORD plusTolKW = pDoc->RegisterKeyWord(attribName, 0, valType);

   if (pComponentData->getAttributes() == NULL || !pComponentData->getAttributes()->Lookup(plusTolKW, attrib) || attrib == NULL)
      plusTol = "0";
   else if (valType == valueTypeDouble)
      plusTol.Format("%.3f", fabs(attrib->getDoubleValue()));
   else if (valType == valueTypeInteger)
      plusTol.Format("%.3f", fabs((double)attrib->getIntValue()));
   else if (valType == valueTypeString)
      plusTol.Format("%.3f", fabs(atof(attrib->getStringValue())));
      
   return plusTol;
}

/******************************************************************************
* CIplComponent::GetValue
*/

CString CIplComponent::GetValue()
{
   CString value( m_iplTestElement->GetValue() );

   if (value.IsEmpty())
   {
      Attrib* attrib = NULL;
      WORD valueKW = pDoc->getStandardAttributeKeywordIndex(standardAttributeValue);
      if (pComponentData->getAttributes() == NULL || !pComponentData->getAttributes()->Lookup(valueKW, attrib) || attrib == NULL)
         value = "0";
      else
      {
         value = attrib->getStringValue();
      }
   }

   return value;
}


/******************************************************************************
* CIplComponent::GetPinCount
*/
int CIplComponent::GetPinCount()
{
   return pins.GetCount();
}

/******************************************************************************
* CIplComponent::GetFirstPin
*/
CIplPin *CIplComponent::GetFirstPin()
{
   // make sure the pins are sorted
   pins.Sort();

   CString *key;
   CIplPin *iplPin;

   pins.GetFirstSorted(key, (CObject*&)iplPin);

   return iplPin;
}

/******************************************************************************
* CIplComponent::GetNextPin
*/
CIplPin *CIplComponent::GetNextPin()
{
   CString *key;
   CIplPin *iplPin;

   pins.GetNextSorted(key, (CObject*&)iplPin);

   return iplPin;
}

/******************************************************************************
* CIplComponent::pinSortFunction
*/
int CIplComponent::PinSortFunction(const void* elem1,const void* elem2)
{
   SElement *element1 = (SElement*)elem1, *element2 = (SElement*)elem2;


   CIplPin *pin1 = (CIplPin*)element1->pObject->m_object, *pin2 = (CIplPin*)element2->pObject->m_object;

   return pin1->GetSortName().Compare(pin2->GetSortName());
}

/******************************************************************************
* CIplComponent::ProcessProbeChannelNumbers
*/
void CIplComponent::ProcessProbeChannelNumbers(CChannelNumberArray *channelStats, CIplNetMap *netMap, FILE *logFile)
{
   // if we are not doing the remapping process, then we don't need to concern ourselves
   // with the pin spacing of transistors pins.
   if (!channelStats->DoRemapping())
      return;

   //// if this component isn't a transistor, then we can just return
   if (/*m_deviceType != deviceTypeTransistor  && */    m_deviceType != deviceTypeTransistorArray      &&
       m_deviceType != deviceTypeTransistorFetNpn    && m_deviceType != deviceTypeTransistorFetPnp     &&
       m_deviceType != deviceTypeTransistorMosfetNpn && m_deviceType != deviceTypeTransistorMosfetPnp  && 
       m_deviceType != deviceTypeTransistorNpn       && m_deviceType != deviceTypeTransistorPnp        && 
       m_deviceType != deviceTypeTransistorScr       && m_deviceType != deviceTypeTransistorTriac)
      return;

   populateAvoidLists(netMap);

   CString *key = NULL;
   CIplPin *iplPin = NULL, *basePin = NULL, *emitterPin = NULL, *collectorPin = NULL;

   pins.GetFirstSorted(key, (CObject*&)iplPin);
   while (iplPin != NULL)
   {
      switch (iplPin->GetPinType())
      {
      case pinTypeBase:
         // get base pin
         basePin = iplPin;
         break;
      case pinTypeEmitter:
         // get emitter pin
         emitterPin = iplPin;
         break;
      case pinTypeCollector:
         // get collector pin
         collectorPin = iplPin;
         break;
      }
      pins.GetNextSorted(key, (CObject*&)iplPin);
   }


   if (basePin == NULL || emitterPin == NULL || collectorPin == NULL)
   {
      if (logFile != NULL)
      {
         if (basePin == NULL)
            fprintf(logFile, "Warning:  %s[%d] - Transistor has no base pin",
               componentID, pComponentData->getEntityNumber());
         if (emitterPin == NULL)
            fprintf(logFile, "Warning:  %s[%d] - Transistor has no emitter pin",
               componentID, pComponentData->getEntityNumber());
         if (collectorPin == NULL)
            fprintf(logFile, "Warning:  %s[%d] - Transistor has no collector pin",
               componentID, pComponentData->getEntityNumber());
      }

      return;
   }



   CIplNet *baseNet = netMap->Lookup(basePin->GetNetName());
   CIplNet *emitterNet = netMap->Lookup(emitterPin->GetNetName());
   CIplNet *collectorNet = netMap->Lookup(collectorPin->GetNetName());

   // gather all the channels
   CTypedPtrList<CPtrList, CIplTestProbe*> probes;

   // gather all base test probes
   if (baseNet != NULL)
   {
      CIplTestProbe *baseTP = baseNet->GetFirstTestProbe();
      while (baseTP != NULL)
      {
         probes.AddTail(baseTP);

         baseTP = baseNet->GetNextTestProbe(baseTP);
      }
   }

   // gather all emitter test probes
   if (emitterNet != NULL)
   {
      CIplTestProbe *emitterTP = emitterNet->GetFirstTestProbe();
      while (emitterTP != NULL)
      {
         probes.AddTail(emitterTP);

         emitterTP = emitterNet->GetNextTestProbe(emitterTP);
      }
   }

   // gather all collector test probes
   if (collectorNet != NULL)
   {
      CIplTestProbe *collectorTP = collectorNet->GetFirstTestProbe();
      while (collectorTP != NULL)
      {
         probes.AddTail(collectorTP);

         collectorTP = collectorNet->GetNextTestProbe(collectorTP);
      }
   }


   // loop through and assign all the test probe's channels
   POSITION pos = probes.GetHeadPosition();
   while (pos)
   {
      CIplTestProbe *iplTP = probes.GetNext(pos);

      int curChannelNum = iplTP->GetChannelNumber();

      if (curChannelNum == CHNL_UNASSIGNED)
      {
         CChannel *newChannel = channelStats->GetFirstAvailableChannel();
         while (!setPinChannelNumber(newChannel, iplTP, netMap, channelStats))
            newChannel = channelStats->GetNextAvailableChannel(newChannel);
      }
   }
}

/******************************************************************************
* CIplComponent::GetBlockName
*/
CString CIplComponent::GetBlockName()
{
   if (pComponentData == NULL)
      return "";

   if (pComponentData->getInsert()->getBlockNumber() < 0 || pComponentData->getInsert()->getBlockNumber() >= pDoc->getMaxBlockIndex())
      return "";

   BlockStruct *block = pDoc->getBlockAt(pComponentData->getInsert()->getBlockNumber());
   if (block == NULL)
      return "";

   return block->getName();
}

/******************************************************************************
* CIplComponent::GetExtent
*/
CExtent CIplComponent::GetExtent()
{
   if (pComponentData != NULL && pComponentData->getInsert() != NULL)
   {
      int blockNumber = pComponentData->getInsert()->getBlockNumber();
      if (blockNumber >=0 && blockNumber < pDoc->getMaxBlockIndex())
      {
         BlockStruct *block = pDoc->getBlockAt(blockNumber);
         if (block != NULL)
         {
            return block->getExtent();
         }
      }
   }

   CExtent none(0.0,0.0,0.0,0.0);
   return none;
}

/******************************************************************************
* CIplComponent::GetGridLocation
*/
CString CIplComponent::GetGridLocation()
{
   if (pComponentData != NULL)
   {
      Attrib* attrib = NULL;
      WORD gridlocKW = pDoc->RegisterKeyWord(ATT_GRID_LOCATION, 0, VT_STRING);
      if (pComponentData->getAttributesRef() != NULL && pComponentData->getAttributesRef()->Lookup(gridlocKW, attrib))
      {
         CString temp = get_attvalue_string(pDoc, attrib);
         if (!temp.IsEmpty())
            return temp;
      }
   }

   return "XXX";  // XXX is what was output before grid location support was added, case 2239

}

/******************************************************************************
* CIplComponent::GetComponentDevice
*/
CString CIplComponent::GetComponentDevice()
{
   if (pComponentData == NULL)
      return description;

   Attrib* attrib = NULL;
   WORD teradyneKW = pDoc->RegisterKeyWord(ATT_TERADYNE1800_SUBCLASS, 0, VT_STRING);
   if (pComponentData->getAttributesRef() && pComponentData->getAttributesRef()->Lookup(teradyneKW, attrib))
   {
      CString temp = "";
      temp = get_attvalue_string(pDoc, attrib);
      if (!temp.IsEmpty())
         return temp;
   }

   attrib = NULL;
   WORD styleKW = pDoc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);
   if (pComponentData->getAttributesRef() && pComponentData->getAttributesRef()->Lookup(styleKW, attrib))
      return get_attvalue_string(pDoc, attrib);

   // get the $$DEVICE$$ keyword index
   // See if the device attribute exists, and if it does, return the value
   attrib = NULL;
   WORD listLinkKW = pDoc->RegisterKeyWord(ATT_TYPELISTLINK, 0, VT_STRING);
   if (pComponentData->getAttributesRef() && pComponentData->getAttributesRef()->Lookup(listLinkKW, attrib))
      return get_attvalue_string(pDoc, attrib);

   // get the block inserted
   BlockStruct *block = NULL;
   if (pComponentData->getInsert()->getBlockNumber() >= 0 || pComponentData->getInsert()->getBlockNumber() < pDoc->getMaxBlockIndex())
      block = pDoc->getBlockAt(pComponentData->getInsert()->getBlockNumber());

   // return the block (shape) name
   if (block != NULL)
      return block->getName();

   return description;
}


/******************************************************************************
* 
*/
CPoint2d CIplComponent::GetCentroidLocation()
{
   CPoint2d point;
   point.x = point.y = 0.0;

   InsertStruct *insert = NULL;

   if (pComponentData && (insert = pComponentData->getInsert()))
   {
      if (!insert->getCentroidLocation(pDoc->getCamCadData(), point))
      {
         // No centroid, use insert pt
         //point.x = insert->getOriginX();
         //point.y = insert->getOriginY();

         // Use center of extent
         BlockStruct *insertedBlock = pDoc->getBlockAt(insert->getBlockNumber());
         CExtent extent = insertedBlock->getExtent();
         point.x = extent.getCenter().x;
         point.y = extent.getCenter().y;
      }
   }

   return point;
}

/******************************************************************************
* CIplComoponent::isOnTop
*/
bool CIplComponent::IsOnTop()
{ 
   if (pComponentData != NULL)
   {
      if (pComponentData->getInsert()->getMirrorFlags() && MIRROR_LAYERS)
         return false;
      else
         return true;
   }
   else
      return true;
};

/******************************************************************************
* CIplComponent::IsSMD
*/
bool CIplComponent::IsSMD()
{
   if (pComponentData == NULL)
      return true;

   if (pComponentData->getInsert()->getBlockNumber() < 0 || pComponentData->getInsert()->getBlockNumber() >= pDoc->getMaxBlockIndex())
      return true;

   WORD listLinkKW = pDoc->RegisterKeyWord(ATT_SMDSHAPE, 0, VT_NONE);

   Attrib* attrib;

   if (!pComponentData->getAttributesRef() || !pComponentData->getAttributesRef()->Lookup(listLinkKW, attrib))
      return false;
   else
      return true;
}

/******************************************************************************
* CIplComponent::IsCapOpensQualified
*/
bool CIplComponent::IsCapOpensQualified()
{

	DeviceTypeTag type = this->GetType();

	bool relevantType =  (type == deviceTypeCapacitor || 
		type == deviceTypeCapacitorPolarized || type == deviceTypeCapacitorTantalum ||
		type == deviceTypeConnector ||
		type == deviceTypeIC || type == deviceTypeICDigital || type == deviceTypeICLinear);

	CString capopens = this->GetCapacitiveOpens();

	return (relevantType && capopens.CompareNoCase("true") == 0);
}

/******************************************************************************
* CIplComponent::GetIplString
*/
CString CIplComponent::GetIplString(CIplNetMap *netMap, FILE *logFile)
{
   CString buf, prefixType;

   switch (m_deviceType)
   {
   case deviceTypeJumper:
   case deviceTypeFuse:
      prefixType = "J";
      break;
   case deviceTypeTransistor:
      prefixType = "TRANSISTOR";
      break;
   case deviceTypeTransistorNpn:
      prefixType = "QN";
      break;
   case deviceTypeTransistorPnp:
      prefixType = "QP";
      break;
   case deviceTypeDiode:
   case deviceTypeDiodeLed:
      prefixType = "D";
      break;
   case deviceTypeNoTest:
      return "";
   }

   // TODO: try not to classify as just transistor (use NPN or PNP)
   // comment out transistor line until we know what to classify it
   if (m_deviceType == deviceTypeTransistor)
      buf.Format("/* %s,%s,\"%s\",%s */", prefixType, componentID, description, getPinList(netMap, logFile));
   else
      buf.Format("%s,%s,\"%s\",%s", prefixType, componentID, description, getPinList(netMap, logFile));

   return buf;
}

/******************************************************************************
* CIplComponent::GetGainTestString
*/
bool CIplComponent::GetGainTestString(CString &gainTestLine, int gainLevel, int collectorCurrent, CIplNetMap *netMap, FILE *logFile)
{
   if (m_deviceType != deviceTypeTransistorNpn && m_deviceType != deviceTypeTransistorPnp)
      return false;

   CString gainPrefix;
   if (m_deviceType == deviceTypeTransistorNpn)
      gainPrefix = "GNPN";
   else if (m_deviceType == deviceTypeTransistorPnp)
      gainPrefix = "GPNP";
   gainTestLine.Format("%s,%s,%s,%d Hfe,%d mA,%s", gainPrefix, componentID, description, gainLevel, collectorCurrent, getPinList(netMap));

   return true;
}

/******************************************************************************
* CIplComponent::Dump
*/
void CIplComponent::Dump(CFormatStdioFile *file)
{
   CString *key;
   CIplPin* iplPin = NULL;
   int count = 1;

   pins.Sort();

   file->WriteString("%s [%s] (%d)\n", componentID, deviceTypeTagToValueString(m_deviceType),
      (pComponentData != NULL)?pComponentData->getEntityNumber():0); 
   pins.GetFirstSorted(key, (CObject*&)iplPin);
   while (iplPin)
   {
      file->WriteString("   %d -> %s\n", count, iplPin->Dump());

      pins.GetNextSorted(key, (CObject*&)iplPin);
      count++;
   }
}



/******************************************************************************
/******************************************************************************
* CIplComponentName::CIplComponentName
*/
CIplComponentName::CIplComponentName(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile)
   : CIplComponent(doc, iplTestElement, compName, data, devType, descAttrName, logFile)
{
   if (description.IsEmpty())
   {
      switch (m_deviceType)
      {
      // IC      ID,DESC,NAME,PINLIST
      // VEC     ID,DESC,NAME,PINLIST
      case deviceTypeICDigital:
         description = "logic";
         break;

      // ATMPL   ID,DESC,NAME,PINLIST
      case deviceTypeIC:
      case deviceTypeICLinear:
      case deviceTypeCapacitorArray:
      case deviceTypeDiodeArray:
      case deviceTypeDiodeZener:
      case deviceTypeRelay:
      case deviceTypeTransistorScr:
      case deviceTypeTransistorTriac:
      case deviceTypeVoltageRegulator:
         description = "unknown";
         break;
      case deviceTypeConnector:
         description = "Conn";
         break;
      case deviceTypeCrystal:
         description = "Crystal";
         break;
      case deviceTypeTransformer:
         description = "Transformer";
         break;
      case deviceTypeTransistorFetNpn:
      case deviceTypeTransistorFetPnp:
      case deviceTypeTransistorMosfetNpn:
      case deviceTypeTransistorMosfetPnp:
         description = "Fet";
         break;

      // U       ID,DESC,NAME,PINLIST
      case deviceTypeBattery:
      case deviceTypeFilter:
      case deviceTypeOscillator:
      case deviceTypePowerSupply:
      case deviceTypeSpeaker:
      case deviceTypeSwitch:
      case deviceTypeTestPoint:
      case deviceTypeTransistorArray:
      default:
         description = "unknown";
         break;
      }
   }

   // VCLUST  ID,DESC,NAME
   // DSCAN   ID,DESC,NAME,PINLIST
   // DFP     ID,DESC,NAME

   if (m_deviceType == deviceTypeIC && logFile != NULL)
      fprintf(logFile, "Warning:  %s[%d] - Unknown type: %s.\n",
         compName, pComponentData->getEntityNumber(), deviceTypeTagToValueString(deviceTypeIC));

   componentName = GetComponentDevice();

   if (componentName.IsEmpty() && logFile != NULL)
      fprintf(logFile, "Warning:  %s[%d] - No name found.\n", compName, pComponentData->getEntityNumber());
}

/******************************************************************************
*/
CString CIplComponent::GetIplFSPlusString(CIplNetMap *netMap, FILE *logFile)
{
	CString prefixType("FSPLUS");

	CString buf;
	buf.Format("%s,%s_FS+,\"%s\",\"%s\",X%d,%s", prefixType, componentID, description, GetComponentDevice(), GetSensorNumber(), getPinList(netMap));

   return buf;
}

/******************************************************************************
*/
CString CIplComponent::GetIplCapScanString(CIplNetMap *netMap, FILE *logFile)
{
	CString prefixType("CSCAN");

	CString buf;
	buf.Format("%s,%s_CSCAN,\"%s\",\"%s\",X%d,%s", prefixType, componentID, description, GetComponentDevice(), GetSensorNumber(), getPinList(netMap));

   return buf;
}

/******************************************************************************
* CIplComponentName::GetIplString
*/
CString CIplComponentName::GetIplString(CIplNetMap *netMap, FILE *logFile)
{
   CString buf, prefixType;

   switch (m_deviceType)
   {
   case deviceTypeIC:
   case deviceTypeICDigital:
      prefixType = "IC";
      if (GetPinCount() > 128)
         prefixType = "VEC";
      break;
   case deviceTypeCapacitorArray:
      prefixType = "C&,ATMPL";
      break;
   case deviceTypeDiodeArray:
   case deviceTypeDiodeLedArray:
      prefixType = "D&,ATMPL";
      break;
   case deviceTypeDiodeZener:
      prefixType = "Z";
      break;
   case deviceTypeRelay:
   case deviceTypeTransistorScr:
   case deviceTypeVoltageRegulator:
   case deviceTypeConnector:
   case deviceTypeCrystal:
   case deviceTypeTransformer:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeOpto:
      prefixType = "ADOF&,ATMPL";
      break;
   case deviceTypeTransistorTriac:
   case deviceTypeICLinear:
      prefixType = "ADON&,ATMPL";
      break;
   case deviceTypeBattery:
   case deviceTypeFilter:
   case deviceTypeOscillator:
   case deviceTypePowerSupply:
   case deviceTypeSpeaker:
   case deviceTypeSwitch:
   case deviceTypeTransistorArray:
      prefixType = "MISC&,U";
      break;
   case deviceTypeTestPoint:  // Test points not exported to the IPL file
      return "";
   default:
      prefixType = "U";
   }

   // comment IC type until we know what to show
   buf.Format("%s,%s,\"%s\",\"%s\",%s", prefixType, componentID, description, componentName, getPinList(netMap));

   return buf;
}



/******************************************************************************
/******************************************************************************
* CIplComponentNameInducer::CIplComponentNameInducer
*/
CIplComponentNameInducer::CIplComponentNameInducer(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile)
   :CIplComponentName(doc, iplTestElement, compName, data, devType, descAttrName, logFile)
{
   if (description.IsEmpty())
   {
      /*switch (deviceType)
      {
      case deviceTypeCapacitorPolarized:
         // CSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST (if CAP has a Capacitive Opens test technique requirement)
         description = "Cap";
         break;
      default:
         ;
      }*/
   }

   // WSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST
   // FSCAN   ID,DESC,NAME,INDUCERNUM,PINLIST
   // FSPLUS  ID,DESC,NAME,INDUCERNUM,PINLIST

   Attrib *attrib = NULL;

   // TODO: Get inducer number (need for Capacitive Opens test technique)
   //WORD inducerKW = doc->RegisterKeyWord(ATT_INDUCER, 0, VT_STRING);
   //if (data->getAttributesRef() && data->getAttributesRef()->Lookup(inducerKW, attrib))
   // inducerName = get_attvalue_string(doc, attrib);

   if (inducerName.IsEmpty() && logFile != NULL)
   {
      fprintf(logFile, "Warning:  %s[%d] - No inducer name found. Using '0'.\n", compName, pComponentData->getEntityNumber());
      inducerName = "0";
   }
}

/******************************************************************************
* CIplComponentNameInducer::GetIplString
*/
CString CIplComponentNameInducer::GetIplString(CIplNetMap *netMap, FILE *logFile)
{
   CString buf, prefixType;

   /*switch (deviceType)
   {

   default:
      prefixType = "U";
   }*/
   prefixType = "U";
   buf.Format("%s,%s,\"%s\",\"%s\",%s,%s", prefixType, componentID, description, componentName, inducerName, getPinList(netMap));

   return buf;
}



/******************************************************************************
/******************************************************************************
* CIplComponentValTol::CIplComponentValTol
*/
CIplComponentValTol::CIplComponentValTol(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile)
   :CIplComponent(doc, iplTestElement, compName, data, devType, descAttrName, logFile)
{
   if (description.IsEmpty())
   {
      switch (m_deviceType)
      {
      case deviceTypeInductor:
         //L       ID,DESC,VAL1,TOL1,PIN1,PIN2
         description = "coil";
         break;
      case deviceTypePotentiometer:
      case deviceTypeResistor:
         //R       ID,DESC,VAL1,TOL1,PIN1,PIN2
         //POT     ID,DESC,VAL1,TOL1,PIN1,PIN2,PIN3
         description = "Res";
         break;
      case deviceTypeResistorArray:
         //RP_SI   ID,DESC,VAL1,TOL1,PINLIST
         //RP_SB   ID,DESC,VAL1,TOL1,PINLIST
         //RP_DI   ID,DESC,VAL1,TOL1,PINLIST
         //RP_DB   ID,DESC,VAL1,TOL1,PINLIST
         //RP_XX ...
         description = "Respack";
         break;
      default:
         ;
      }
   }

   //REO     ID,DESC,VAL1,TOL1,PIN1,PIN2
   //Z       ID,DESC,VAL1,TOL1,ANODE,CATHODE

   

   // get value
   CString valueStr( this->GetValue() );
   if (!valueStr.IsEmpty())
      m_value.SetValue(valueStr);
   else if (logFile != NULL)
      fprintf(logFile, "Warning:  %s[%d] - Attribute named %s not found. Using '0'.\n", compName, pComponentData->getEntityNumber(), ATT_VALUE);

   // get tolerance
   if (!iplTestElement->GetPlusTol().IsEmpty())
   {
      double elemTol = atof(iplTestElement->GetPlusTol());
      m_tol.Format("%.3f", fabs(elemTol));
   }
   else
   {
      Attrib *attrib = NULL;
      // 25 Aug 08, per Mark, change to using ATT_PLUSTOLERANCE, because DataDoctor does not support "plain" TOLERANCE
      ////WORD tolKW = doc->RegisterKeyWord(ATT_TOLERANCE, 0, VT_DOUBLE);
      WORD tolKW = doc->RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);
      if (data->getAttributesRef() && data->getAttributesRef()->Lookup(tolKW, attrib))
         m_tol.Format("%.3f", fabs(attrib->getDoubleValue()));
      else if (logFile != NULL)
      {
         ////fprintf(logFile, "Warning:  %s[%d] - Attribute named %s not found. Using '0'.\n", compName, pComponentData->getEntityNumber(), ATT_TOLERANCE);
         fprintf(logFile, "Warning:  %s[%d] - Attribute named %s not found. Using '0'.\n", compName, pComponentData->getEntityNumber(), ATT_PLUSTOLERANCE);
         m_tol = "0";
      }
   }
}

/******************************************************************************
* CIplComponentValTol::GetIplString
*/
CString CIplComponentValTol::GetIplString(CIplNetMap *netMap, FILE *logFile)
{
   CString buf, prefixType;
   Attrib* attrib;
   WORD styleKW = pDoc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);

   switch (m_deviceType)
   {
   case deviceTypeInductor:
      prefixType = "L";
      break;
   case deviceTypePotentiometer:
      prefixType = "POT";
      break;
   case deviceTypeResistor:
      prefixType = "R";
      break;
   case deviceTypeResistorArray:
      prefixType = "RP_XX";

      if (pComponentData->getAttributesRef() && pComponentData->getAttributesRef()->Lookup(styleKW, attrib))
         prefixType = get_attvalue_string(pDoc, attrib);

      break;
   case deviceTypeDiodeZener:
      prefixType = "Z";
      break;

   default:
      prefixType = "U";
   }

   CString value1;

   if (m_value.IsValid())
   {
      value1 = m_value.GetPrintableString();

      if (m_deviceType == deviceTypeResistor)
         value1 += "O";
   }
   else
   {
      value1 = "0O";
   }

   buf.Format("%s,%s,\"%s\",%s,%s%%,%s", prefixType, componentID, description, value1, m_tol, getPinList(netMap));

   return buf;
}



/******************************************************************************
/******************************************************************************
* CIplComponentValTolTol::CIplComponentValTolTol
*/
CIplComponentValTolTol::CIplComponentValTolTol(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile)
   :CIplComponentValTol(doc, iplTestElement, compName, data, devType, descAttrName, logFile)
{
   if (description.IsEmpty())
   {
      switch (m_deviceType)
      {
      case deviceTypeCapacitor:
      case deviceTypeCapacitorPolarized:
      case deviceTypeCapacitorTantalum:
         //C       ID,DESC,VAL1,TOL1,TOL2,PIN1,PIN2
         description = "Cap";
         break;
      default:
         ;
      }
   }


   if (!iplTestElement->GetPlusTol().IsEmpty())
   {
      double elemTol = atof(iplTestElement->GetPlusTol());
      m_tol.Format("%.3f", fabs(elemTol));
   }
   else
   {
      Attrib *attrib = NULL;

      WORD tolKW = doc->RegisterKeyWord(ATT_PLUSTOLERANCE, 0, VT_DOUBLE);
      if (data->getAttributesRef() && data->getAttributesRef()->Lookup(tolKW, attrib))
         m_tol.Format("%.3f", fabs(attrib->getDoubleValue()));
      else if (logFile != NULL && m_tol != "0")
         fprintf(logFile, "Warning:  %s[%d] - Attribute named %s not found. Using Tolerance value [%s].\n", compName, pComponentData->getEntityNumber(), ATT_PLUSTOLERANCE, m_tol);
   }

   if (!iplTestElement->GetMinusTol().IsEmpty())
   {
      double elemTol = atof(iplTestElement->GetMinusTol());
      m_tol2.Format("%.3f", fabs(elemTol));
   }
   else
   {
      Attrib *attrib = NULL;
      WORD tol2KW = doc->RegisterKeyWord(ATT_MINUSTOLERANCE, 0, VT_DOUBLE);
      if (data->getAttributesRef() && data->getAttributesRef()->Lookup(tol2KW, attrib))
         m_tol2.Format("%.3f", fabs(attrib->getDoubleValue()));
      else if (logFile != NULL)
      {
         m_tol2 = m_tol;
         fprintf(logFile, "Warning:  %s[%d] - Attribute named %s not found. Using Tolerance value [%s].\n", compName, pComponentData->getEntityNumber(), ATT_MINUSTOLERANCE, m_tol2);
      }
   }
}

/******************************************************************************
* CIplComponentValTolTol::GetIplString
*/
CString CIplComponentValTolTol::GetIplString(CIplNetMap *netMap, FILE *logFile)
{
   CString buf, prefixType;

   switch (m_deviceType)
   {
   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      prefixType = "C";
      break;
      
   //case deviceTypeCapacitorPolarized:
   //case deviceTypeCapacitorTantalum:
   // prefixType = "C&,U";
   // break;
   default:
      prefixType = "U";
   }

   CString value1;
   if (!m_value.GetPrintableString(value1))
      value1 = "0";

   buf.Format("%s,%s,\"%s\",%s,%s%%,%s%%,%s", prefixType, componentID, description, value1, m_tol, m_tol2, getPinList(netMap));

   return buf;
}

/******************************************************************************
* CIplComponentValTolTol::GetDischargeString
*/
bool CIplComponentValTolTol::GetDischargeString(CString &dischargeLine, ComponentValues dischargeMinValue, CIplNetMap *netMap, FILE *logFile)
{
   CString prefixType;

   switch (m_deviceType)
   {
   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      prefixType = "DCHG";
      break;
   default:
      return false;
   }

   double curValue, dchgValue;
   ValueUnitTag curUnit, dchgUnit;

   m_value.GetValue(curValue, curUnit);
   dischargeMinValue.GetValue(dchgValue, dchgUnit);

   if (curUnit != dchgUnit || curValue < dchgValue)
      return false;

   dischargeLine.Format("%s,%s,\"discharge\",%s", prefixType, componentID, getPinList(netMap));
   return true;
}



/******************************************************************************
/******************************************************************************
* CIplComponentValValTol::CIplComponentValValTol
*/
CIplComponentValValTol::CIplComponentValValTol(CCEtoODBDoc *doc, CIplTestElement *iplTestElement, CString compName, DataStruct *data, DeviceTypeTag devType, CString descAttrName, FILE *logFile)
   :CIplComponentValTol(doc, iplTestElement, compName, data, devType, descAttrName, logFile)
{
   switch (m_deviceType)
   {
   case -1:  // to suppress compiler warning
   default:
      break;
   }

   //RP_ST   ID,DESC,VAL1,VAL2,TOL1,PINLIST
   //RP_DT   ID,DESC,VAL1,VAL2,TOL1,PINLIST

   Attrib *attrib = NULL;

   // TODO: Get Value 2 (still nothing we support)
   //WORD value2KW = doc->RegisterKeyWord(ATT_VALUE2, 0, VT_STRING);
   //if (data->getAttributesRef() && data->getAttributesRef()->Lookup(value2KW, attrib))
   // val2 = get_attvalue_string(doc, attrib);
   //else
   if (logFile != NULL)
      fprintf(logFile, "Warning:  %s[%d] - No 2nd value found. Using '0'.\n", compName, pComponentData->getEntityNumber());
}

/******************************************************************************
* CIplComponentValValTol::GetIplString
*/
CString CIplComponentValValTol::GetIplString(CIplNetMap *netMap, FILE *logFile)
{
   CString buf, prefixType;

   switch (m_deviceType)
   {
   case -1:  // to suppress compiler warning
   default:
      prefixType = "U";
      break;
   }

   CString value1, value2;
   if (!m_value.GetPrintableString(value1))
      value1 = "0";
   if (!val2.GetPrintableString(value2))
      value2 = "0";

   buf.Format("%s,%s,\"%s\",%s,%s,%s%%,%s", prefixType, componentID, description, value1, value2, m_tol, getPinList(netMap));

   return buf;
}



/******************************************************************************
/******************************************************************************
*/
int CIplComponentMap::AscendingIplCompRefnameSortFunc(const void *a, const void *b)
{
   CIplComponent* compA = (CIplComponent*)(((SElement*) a )->pObject->m_object);
   CIplComponent* compB = (CIplComponent*)(((SElement*) b )->pObject->m_object);

   return (compA->GetSortableRefname().CompareNoCase(compB->GetSortableRefname()));
}

/******************************************************************************
* CIplComponentMap::findComponentData
*/
DataStruct *CIplComponentMap::findComponentData(CCEtoODBDoc *doc, CString compRefname, FileStruct *file, DeviceTypeTag &type, CString &subclass)
{
   type = deviceTypeUnknown;

   POSITION pos = file->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

      if (data != NULL &&
         data->isInsertType(insertTypePcbComponent) &&
         data->getInsert()->getRefname().Compare(compRefname) == 0)
      {
         if (data->getAttributesRef() != NULL)
         {
            Attrib *attrib = NULL;

            WORD devTypeKW = doc->getStandardAttributeKeywordIndex(standardAttributeDeviceType);
            if (data->getAttributesRef()->Lookup(devTypeKW, attrib))
               type = stringToDeviceTypeTag( attrib->getStringValue() );

            WORD subclassKW = doc->getStandardAttributeKeywordIndex(standardAttributeSubclass);
            if (data->getAttributesRef()->Lookup(subclassKW, attrib))
               subclass = attrib->getStringValue();
         }

         return data;
      }
   }

   return NULL;
}

/******************************************************************************
* CIplComponentMap::Lookup
*/

CIplComponent *CIplComponentMap::Lookup(LPCTSTR key)
{
   CIplComponent *iplComp = NULL;

   if (!CMapSortedStringToOb<CIplComponent>::Lookup(key, iplComp))
      return NULL;

   return iplComp;
}

/******************************************************************************
* CIplTestElement
*/

CIplTestElement::CIplTestElement(CString parentCompRefname, CSubclassElement *scElement)
: m_parentCompRefname(parentCompRefname)
, m_scElement(scElement)
{
}

CString CIplTestElement::GetRefname()
{
   CString refname( m_parentCompRefname );  // Default, refname as-is

   // If subclass element actually used then mod refname
   if (m_scElement!= NULL)
   {
      if (!m_scElement->getName().IsEmpty())
         refname.Format("%s_%s", m_parentCompRefname, m_scElement->getName());
      else if (m_scElement->getPin3Name().IsEmpty())
         refname.Format("%s_%s_%s", m_parentCompRefname, m_scElement->getPin1Name(), m_scElement->getPin2Name());
      else
         refname.Format("%s_%s_%s_%s", m_parentCompRefname, m_scElement->getPin1Name(), m_scElement->getPin2Name(), m_scElement->getPin3Name());
   }

   return refname;
}

DeviceTypeTag CIplTestElement::GetDeviceTypeTag(DeviceTypeTag defaultDevType)
{
   if (m_scElement != NULL)
   {
      DeviceTypeTag elDevType = m_scElement->getDeviceType();

      if (elDevType != deviceTypeUnknown && elDevType != deviceTypeUndefined)
         return elDevType;
   }

   return defaultDevType;
}

/******************************************************************************
* CIplComponentMap::AddPinProbe
*/
void CIplComponentMap::AddNetComponents(CCEtoODBDoc *doc, FileStruct *file, NetStruct *net, CString descAttrName, FILE *logFile)
{
   CString netName = net->getNetName();
   WORD pinMapKW = doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);

   // loop through all the comp/pins in this net
   POSITION pos = net->getHeadCompPinPosition();
   while (pos)
   {
      CompPinStruct *cp = net->getNextCompPin(pos);
      Attrib *attrib = NULL;
      int pinNumber = 0;

      // get the pin number
      if (cp->getAttributesRef() != NULL && cp->getAttributesRef()->Lookup(pinNumberKeyword, attrib))
         pinNumber = attrib->getIntValue();

      
      DeviceTypeTag compDevTypeTag;
      CString subclassStr;
      DataStruct *data = findComponentData(doc, cp->getRefDes(), file, compDevTypeTag, subclassStr);

      CSubclassElement *el = NULL;
      CSubclass *sc = NULL;
      if (!subclassStr.IsEmpty())
      {
         sc = m_subclassList.findSubclass(subclassStr);
         if (sc != NULL)
         {
            CString errMsg;
            if (sc->isValidMatch(doc->getCamCadData(), data, errMsg))
            {
               // Look for first element using this pin
               el = sc->findNthElementUsingPin(1, cp->getPinName());

               // Subclass is valid, but no element for this component pin, consider it unused pin
               // and skip it.
               if (el == NULL)
               {
                  if (logFile != NULL)
                     fprintf(logFile, "Component %s using Subclass %s, No element contains pin %s, pin skipped.\n",
                     data->getInsert()->getRefname(), sc->getName(), cp->getPinName());
                  continue;
               }
            }
            else
            {
               // Error, leave el NULL (ignore the subclass) and log the error message
               if (logFile != NULL)
                  fprintf(logFile, errMsg);
            }
         }
      }

      // Output in some sections is based on test iplTestElements.
      // We will always create atleast one for this comppin, even when test elements by way of subclass is not used.
      // But if subclass test elements are used, then there may be more than one.
      // So we always create one, even if el is NULL.
      // But if el is not NULL then also look for more.
      // The error messages above related to the subclass element are applied only for the first one, we
      // don't need to repeat that stuff again.

      int testElementNum = 1;

      while (testElementNum == 1 || el != NULL)
      {
         CIplTestElement iplTestElement(cp->getRefDes(), el);

         // create and add new IPL Component if needed
         CIplComponent *iplComp = Lookup( iplTestElement.GetRefname() );
         if (iplComp == NULL && data != NULL)
         {
            iplComp = AddTestElement(doc, iplTestElement, data, compDevTypeTag, descAttrName, logFile);
         }

         if (iplComp != NULL)
         {
            PinTypeTag pinType = pinTypeUnknown;

            // Backward compat, use pin map attrib
            // get the pin type if we know what it is
            if (cp->getAttributesRef() && cp->getAttributesRef()->Lookup(pinMapKW, attrib))
            {
               CString pinTypeStr = get_attvalue_string(doc, attrib);
               pinType = stringToPinTypeTag(pinTypeStr);
            }

            // If we have a test element get possible override from test element
            CString testElemPinFunc( iplComp->GetTestElementPinFunction( cp->getPinName() ) );
            if (!testElemPinFunc.IsEmpty())
               pinType = stringToPinTypeTag(testElemPinFunc);

            // add the pin to the component
            iplComp->AddPin(cp->getPinName(), pinNumber, pinType, netName);
         }

         // See if there is another one
         ++testElementNum;
         if (sc != NULL)
            el = sc->findNthElementUsingPin(testElementNum, cp->getPinName());
      }
   }
}

/******************************************************************************
* CIplComponentMap::AddTestElement
*/
CIplComponent *CIplComponentMap::AddTestElement(CCEtoODBDoc *doc, CIplTestElement &iplTestElement, DataStruct *data, 
                                                DeviceTypeTag compDevTypeTag, CString descAttrName, FILE *logFile)
{
   DeviceTypeTag devTypeTag = iplTestElement.GetDeviceTypeTag(compDevTypeTag);

   CIplTestElement *testEl = new CIplTestElement( iplTestElement );

   CIplComponent *iplComp = NULL;

   // create the right type of component based on the the compType
   switch (devTypeTag)
   {
   case deviceTypeJumper:
   case deviceTypeFuse:
   case deviceTypeTransistor:
   case deviceTypeTransistorNpn:
   case deviceTypeTransistorPnp:
   case deviceTypeDiode:
   case deviceTypeDiodeLed:
   case deviceTypeNoTest:         
      iplComp = new CIplComponent(doc, testEl, iplTestElement.GetRefname(), data, devTypeTag, descAttrName, logFile);
      break;

   case deviceTypeIC:
   case deviceTypeICDigital:
   case deviceTypeICLinear:
   case deviceTypeCapacitorArray:
   case deviceTypeDiodeArray:
   case deviceTypeRelay:
   case deviceTypeTransistorScr:
   case deviceTypeTransistorTriac:
   case deviceTypeVoltageRegulator:
   case deviceTypeConnector:
   case deviceTypeCrystal:
   case deviceTypeTransformer:
   case deviceTypeTransistorFetNpn:
   case deviceTypeTransistorFetPnp:
   case deviceTypeTransistorMosfetNpn:
   case deviceTypeTransistorMosfetPnp:
   case deviceTypeBattery:
   case deviceTypeFilter:
   case deviceTypeOscillator:
   case deviceTypePowerSupply:
   case deviceTypeSpeaker:
   case deviceTypeSwitch:
   case deviceTypeTestPoint:
   case deviceTypeTransistorArray:
   case deviceTypeOpto:
      iplComp = (CIplComponent*)new CIplComponentName(doc, testEl, iplTestElement.GetRefname(), data, devTypeTag, descAttrName, logFile);
      break;

      /*case deviceTypeCapacitorPolarized:
      iplComp = (CIplComponent*)new CIplComponentNameInducer(doc, cp->comp, data, compType, logFile);
      break;*/

   case deviceTypeInductor:
   case deviceTypePotentiometer:
   case deviceTypeResistor:
   case deviceTypeResistorArray:
   case deviceTypeDiodeZener:
      iplComp = (CIplComponent*)new CIplComponentValTol(doc, testEl, iplTestElement.GetRefname(), data, devTypeTag, descAttrName, logFile);
      break;

   case deviceTypeCapacitor:
   case deviceTypeCapacitorPolarized:
   case deviceTypeCapacitorTantalum:
      iplComp = (CIplComponent*)new CIplComponentValTolTol(doc, testEl, iplTestElement.GetRefname(), data, devTypeTag, descAttrName, logFile);
      break;

      //case ??????:
      // iplComp = (CIplComponent*)new CIplComponentValValTol(doc, cp->comp, data, compType, logFile);
      // break;

   default:
      iplComp = (CIplComponent*)new CIplComponentName(doc, testEl, iplTestElement.GetRefname(), data, deviceTypeUnknown, descAttrName, logFile);
      break;
   }

   SetAt(iplTestElement.GetRefname(), iplComp);

   return iplComp;
}

/******************************************************************************
* CIplComponentMap::SetPinNumberKeyword
*/
WORD CIplComponentMap::SetPinNumberKeyword(WORD pinKW)
{
   WORD lastVal = pinNumberKeyword;

   pinNumberKeyword = pinKW;

   return lastVal;
}

/******************************************************************************
* CIplComponentMap::LookupCompPin
*/
CIplComponent *CIplComponentMap::LookupElement(CString originalCompRefname, CString originalPinRefname, int nth)
{
   // Find the nth IplComponent that has Subclass Element for originalPinRefname,
   // using original ccz comp refname. Return NULL if none.
   // Count for nth starts at 1.
   // Nth counter necessary because more than one element for original component may share same pin.
   // Note that returning NULL does not mean there is no IplComponent for this 
   // refname and pin name, it means there is none that uses a Subclass Element for test.
 
   int n = 0;
   POSITION pos = this->GetStartPosition();
   while (pos)
   {
      CString key = "";
      CIplComponent *iplComp = NULL;

      this->GetNextAssoc(pos, key, iplComp);
      if (iplComp != NULL && iplComp->GetOriginalCczRefname().Compare(originalCompRefname) == 0)
      {
         // 
         if (iplComp->HasSubclassElement() &&
            iplComp->SubclassElementContainsPin(originalPinRefname))
         {
            n++; // Found one

            if (n == nth)
               return iplComp;
         }
		}
	}


   return NULL;
}

/******************************************************************************
* CIplComponentMap::Dump
*/
void CIplComponentMap::Dump(CFormatStdioFile *file)
{
   file->WriteString("\n-----------------------Map of Components-----------------------\n");

   POSITION pos = GetStartPosition();
   while (pos)
   {
      CString compName;
      CIplComponent *iplComp = NULL;
      
      GetNextAssoc(pos, compName, iplComp);

      iplComp->Dump(file);
   }
}



/******************************************************************************
/******************************************************************************
* CIplNet::CIplNet
*/
CIplNet::CIplNet(CCEtoODBDoc *doc, NetStruct *net, int netNum, CString netStatus, CIplTestProbe *iplTP)
{
   pDoc = NULL;
   pNet = NULL;
   powerNet = groundNet = false;
   iplTestProbes.RemoveAll();

   if (doc == NULL || net == NULL)
      return;

   pDoc = doc;
   pNet = net;
   netNumber = netNum;

   netStatus.MakeUpper();
   groundNet = (netStatus == "GROUND");
   powerNet = (netStatus == "POWER");
      
   
   if (iplTP != NULL)
      iplTestProbes.AddTail(iplTP);

   netName = net->getNetName();
}

/******************************************************************************
* CIplNet::AddTestProbe
*/
POSITION CIplNet::AddTestProbe(CIplTestProbe *iplTP)
{
   if (iplTP == NULL)
      return NULL;

   return iplTestProbes.AddTail(iplTP);
}

/******************************************************************************
*/
bool CIplNet::SetProbeChannelNumber(CChannel *channel, CIplTestProbe *iplTestProbe, EChannelSpacing channelSpacing)
{
   CString ignoredErrMsg;
   return SetProbeChannelNumber(channel, iplTestProbe, channelSpacing, ignoredErrMsg);
}

/******************************************************************************
*/
bool CIplNet::SetProbeChannelNumber(CChannel *channel, CIplTestProbe *iplTestProbe, EChannelSpacing channelSpacing, CString &errMsg)
{
   errMsg = "";

   if (channel == NULL || iplTestProbe == NULL)
      return false;

   // check to see if we can assign this new channel
   CChannel *tpChannel = iplTestProbe->GetChannel();
   if (tpChannel != NULL)
   {
      if (!tpChannel->CanUse(channel, channelSpacing, errMsg))
         return false;

      tpChannel->SetUnused();
   }
   else if (iplTestProbes.GetCount() > 1)
   {
      // There is more than one testprobe so check channel spacing against the first
      // testprobe with channel

      POSITION pos = iplTestProbes.GetHeadPosition();
      while (pos)
      {
         CIplTestProbe *iplTP = iplTestProbes.GetNext(pos);
         tpChannel = iplTP->GetChannel();

         if (tpChannel == NULL)
            continue;

         if (iplTP != iplTestProbe)
         {
            if (!tpChannel->IsReserved())       
            {
               // Found the non-reserved first testprobe with a channel so check if channel spacing is correct
               if (!tpChannel->CanUse(channel, channelSpacing, errMsg))
                  return false;
            
               // Also need to check channel spacing against the first testprobe found
               if (abs(channel->GetChannelNumber() - tpChannel->GetChannelNumber()) < EChannelSpacingToInteger(channelSpacing))
               {
                  errMsg.Format("Insufficent spacing between channels %d and %d, required spacing: %d.",
                     channel->GetChannelNumber(), tpChannel->GetChannelNumber(), EChannelSpacingToInteger(channelSpacing));
                  return false;
               }
            }
            else if (iplTestProbes.GetCount() == 2)
            {
               // Only check against reserved channel if there is only two probes
               if (abs(channel->GetChannelNumber() - tpChannel->GetChannelNumber()) < EChannelSpacingToInteger(channelSpacing))
               {
                  errMsg.Format("Insufficent spacing between channels %d and %d, required spacing: %d.",
                     channel->GetChannelNumber(), tpChannel->GetChannelNumber(), EChannelSpacingToInteger(channelSpacing));
                  return false;
               }
            }
   
            break;
         }  
      }
   }

   iplTestProbe->SetChannel(channel);

   // set the channels this channel must avoid
   POSITION pos = iplTestProbes.GetHeadPosition();
   while (pos)
   {
      CIplTestProbe *iplTP = iplTestProbes.GetNext(pos);
      CChannel *curChannel = iplTP->GetChannel();

      if (curChannel == NULL)
         continue;

      if (iplTP == iplTestProbe)
         continue;

      // set the avoid list for each other
      channel->AddToAvoid(curChannel);
      curChannel->AddToAvoid(channel);
   }

   return true;
}

/******************************************************************************
* CIplNet::GetTestProbeCount
*/
int CIplNet::GetTestProbeCount()
{
   return iplTestProbes.GetCount();
}

/******************************************************************************
* CIplNet::GetFirstTestProbe
*/
CIplTestProbe *CIplNet::GetFirstTestProbe()
{
   if (iplTestProbes.GetCount() <= 0)
      return NULL;

   return iplTestProbes.GetHead();
}

/******************************************************************************
* CIplNet::GetNextTestProbe
*/
CIplTestProbe *CIplNet::GetNextTestProbe(CIplTestProbe *&iplTP)
{
   POSITION pos =iplTestProbes.Find(iplTP);
   if (!pos)
      return NULL;

   iplTestProbes.GetNext(pos);
   iplTP = (pos != NULL)?iplTestProbes.GetNext(pos):NULL;

   return iplTP;
}

/******************************************************************************
* CIplNet::ProcessProbeChannelNumbers
*/
void CIplNet::ProcessProbeChannelNumbers(CChannelNumberArray *channelStats)
{
   if (iplTestProbes.GetCount() <= 0)
      return;

   POSITION pos = iplTestProbes.GetHeadPosition();
   while (pos)
   {
      CIplTestProbe *iplTP = iplTestProbes.GetNext(pos);
   
      int curChannelNum = iplTP->GetChannelNumber();

      if (curChannelNum == CHNL_UNASSIGNED)
      {
         if (channelStats->DoRemapping())
         {
            CChannel *newChannel = channelStats->GetFirstAvailableChannel();
            while (!SetProbeChannelNumber(newChannel, iplTP, channelStats->GetChannelSpacing()))
               newChannel = channelStats->GetNextAvailableChannel(newChannel);
         }
         else
         {
            int channelNumber = atoi(iplTP->GetName());
            CChannel *newChannel = channelStats->AddNewChannel(channelNumber);
            SetProbeChannelNumber(newChannel, iplTP, channelStats->GetChannelSpacing());
         }
      }
   }
}

/******************************************************************************
* CIplNet::Dump
*/
void CIplNet::Dump(CFormatStdioFile *file)
{
   file->WriteString("%s [%d]\n", netName, netNumber);
   POSITION pos = pNet->getHeadCompPinPosition();
   while (pos)
   {
      CompPinStruct *cp = pNet->getNextCompPin(pos);
      if (pos)
         file->WriteString(" |-- %s/%s\n", cp->getRefDes(), cp->getPinName());
      else
         file->WriteString(" *-- %s/%s\n", cp->getRefDes(), cp->getPinName());
   }
   pos = iplTestProbes.GetHeadPosition();
   while (pos)
   {
      CIplTestProbe *iplTP = iplTestProbes.GetNext(pos);
      file->WriteString(" == %s\n", iplTP->Dump());
   }
}



/******************************************************************************
/******************************************************************************
* CIplNetMap::Lookup
*/
CIplNet *CIplNetMap::Lookup(LPCTSTR key)
{
   CIplNet *iplNet = NULL;

   if (!CMapSortedStringToOb<CIplNet>::Lookup(key, iplNet))
      return NULL;

   return iplNet;
}

/******************************************************************************
*/
int CIplNetMap::AscendingNetNameSortFunc(const void *a, const void *b)
{
   CIplNet* netA = (CIplNet*)(((SElement*) a )->pObject->m_object);
   CIplNet* netB = (CIplNet*)(((SElement*) b )->pObject->m_object);

   return (netA->GetName().CompareNoCase(netB->GetName()));
}

/******************************************************************************
* CIplNetMap::Dump
*/
void CIplNetMap::Dump(CFormatStdioFile *file)
{
   file->WriteString("\n-----------------------Map of nets-----------------------\n");
   POSITION pos = GetStartPosition();
   while (pos)
   {
      CString netName;
      CIplNet *iplNet = NULL;

      GetNextAssoc(pos, netName, iplNet);

      iplNet->Dump(file);
   }
}



/******************************************************************************
/******************************************************************************
* CIplWriter::CIplWriter
*/
CIplWriter::CIplWriter(CCEtoODBDoc &doc)
	: m_descriptionAttribName(ATT_PARTNUMBER)
   , m_unplacedProbeStartChannel(-1)
   , m_compMap(&doc.getCamCadData().getTableList())
{
   maxLineLength = DEFAULT_LINE_LENGTH;
   createGainTests = false;
   gainLevel = 0;
   gainCollectorCurrent = 0;
   pFile = NULL;
   pDoc = NULL;
   m_decimalPlaces = 3;
   outputScale = 1;
}

CIplWriter::CIplWriter(CCEtoODBDoc &doc, bool useIplOutSettings, const char *filename, bool skipPowerInjectionProbesFlag)
	: m_descriptionAttribName(ATT_PARTNUMBER)
   , m_unplacedProbeStartChannel(-1)
   , m_compMap(&doc.getCamCadData().getTableList())
{
   // filename is ipl.dat output filename, we use it here just to get
   // the path so log file can go in same directory.

   maxLineLength = DEFAULT_LINE_LENGTH;
   createGainTests = false;
   gainLevel = 0;
   gainCollectorCurrent = 0;
   m_decimalPlaces = 3;
   outputScale = 1;

   pFile = NULL;

   pDoc = &doc;

   // 4/27/06 Mark wants log to go to output folder, not camcad install folder
   // The default is still the camcad folder if no filename was specified.
   CString logPath = GetLogfilePath("ipl.log");
   if (filename != NULL)
   {
      CFilePath logFilePath(filename);
      logFilePath.setBaseFileName("ipl");
      logFilePath.setExtension("log");
      logPath = logFilePath.getPath();
   }

   // In order to keep the 4/27/06 thing above in place, we need to treat
   // readwrite job aka vPlan usage as a special case.
   m_iplLogFp = NULL;
   m_localLogFilename = logPath; // Default to 4/27/06 fix above.
   if (getApp().m_readWriteJob.IsActivated())
   {
      // Readwrite job aka vPlan usage.
      m_iplLogFp = getApp().OpenOperationLogFile("ipl.log", m_localLogFilename);
      if (m_iplLogFp == NULL) // error message already issued, just return.
         return;
   }
   else
   {
      // Standard CCASM CAMCAD product.
      if ((m_iplLogFp = fopen(m_localLogFilename, "wt")) == NULL) // rewrite file
      {
         ErrorMessage("Can not open Logfile!", m_localLogFilename, MB_ICONEXCLAMATION | MB_OK);
         return;
      }
   }

   if (m_iplLogFp != NULL)
   {
      WriteStandardExportLogHeader(m_iplLogFp, "Teradyne Z1800 IPL");
   }
   
	// Read rules for IPL.
   // Case 1592, Some exporters use IPL output modules, but don't want IPL controls active.
   if (useIplOutSettings)
   {
      CString settingsFile( getApp().getExportSettingsFilePath("ipl_18xx.out") );

      CString msg;
      msg.Format("Teradyne Z1800 IPL: Settings file [%s].\n\n", settingsFile);
      getApp().LogMessage(msg);

      readSettingsFile(settingsFile);
   }


   if (gatherData(skipPowerInjectionProbesFlag))
   {
      if (!channelNumberStats.DoRemapping())
      {
         //check to make sure all probe names are numeric
         BOOL abort = FALSE;
         POSITION pos = testProbes.GetHeadPosition();
         while (pos)
         {
            if (!IsProbeNameNumeric(((CIplTestProbe*)testProbes.GetNext(pos))->GetName()))
            {
               abort = TRUE;
               break;
            }
         }
         if (abort)
         {
            //abort export
            CString err = "Non numeric probe names were found. Export process aborted.";
            throw err;
         }
      }

      // Preprocessing of channel #'s to PLACED probes using rules
      assignChannelNumbers();

      // For Cap Opens tests
      assignCapOpensSensorNumbers();

      // Add in Unplaced probes
      // These ignore the rules (spacing rules, etc). Unplaced probes are simply
      // numbered sequentially increasing, starting at highest placed probe channel plus 1.
      gatherAndAssignUnplacedProbes();
   }

   // dump debug information
#if defined _RDEBUG
   CFormatStdioFile file;
   if (file.Open(getApp().getUserPath() + "ipl.dbg", CFile::modeCreate|CFile::modeWrite))
   {
      channelNumberStats.Dump(&file);
      testProbes.Dump(&file);
      m_netMap.Dump(&file);
      m_compMap.Dump(&file);
   }
   file.Close();
#endif

}

/******************************************************************************
* CIplWriter::~CIplWriter
*/
CIplWriter::~CIplWriter()
{
   // close the log file
   fprintf(m_iplLogFp, "\n\nEND Logfile\n");
   if (!m_localLogFilename.IsEmpty()) // Close log file only if it was opened locally.
   {
      fclose(m_iplLogFp);
      m_iplLogFp = NULL;
   }

   // remove all the nets
   POSITION pos = m_netMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplNet *iplNet = NULL;

      m_netMap.GetNextAssoc(pos, key, iplNet);

      delete iplNet;
   }
   m_netMap.RemoveAll();

   // remove all the components
   pos = m_compMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplComponent *iplComp = NULL;

      m_compMap.GetNextAssoc(pos, key, iplComp);

      delete iplComp;
   }
   m_compMap.RemoveAll();

   // remove all the test probes
   pos = testProbes.GetHeadPosition();
   while (pos)
      delete testProbes.GetNext(pos);
   testProbes.RemoveAll();
}

/******************************************************************************
* CIplWriter::gatherData
*/
bool CIplWriter::gatherData(bool skipPowerInjectionProbesFlag)
{
   // Return true if successful, otherwise false.

   // find the first visible PCB
   bool panelVisible = false;
   POSITION pos = pDoc->getFileList().GetHeadPosition();
   while (pos && pFile == NULL)
   {
      FileStruct *file = pDoc->getFileList().GetNext(pos);

      if (file->getBlockType() == blockTypePcb && file->isShown())
      {
         pFile = file;
      }
      else if (file->getBlockType() == blockTypePanel && file->isShown())
      {
         panelVisible = true;
      }
   }

   if (pFile == NULL)
   {
      CString msg( "Must have a visible PCB file for export." );
      if (panelVisible)
         msg += " This exporter does not support Panel export.";
      ErrorMessage(msg, "");
      fprintf(m_iplLogFp, "No visible PCB loaded!\n");\
      fprintf(m_iplLogFp, "%s\n", msg);
      return false;
   }

   m_compMap.SetPinNumberKeyword(pDoc->RegisterKeyWord(ATT_COMPPINNR, 0, VT_INTEGER));
   // Case 1659, item 2, attribute lookup changed from TEST_NET_STATUS 
   // to NET_TYPE, which is what the probe placement Net Conditions setting
   // is stored in.
   WORD netTypeKW = pDoc->RegisterKeyWord(ATT_NET_TYPE, 0, VT_STRING);

   // gather all nets and comp/pins
   int netCount = 1;
   pos = pFile->getNetList().GetHeadPosition();

   while (pos)
   {
      NetStruct *net = pFile->getNetList().GetNext(pos);
      
      //Skip Single Die Pin Net
      if(net && net->IsSingleDiePinNet())
      {
         fprintf(m_iplLogFp, "Net: %s - Skipped SinglePin Net for Die component.\n",net->getNetName());
         continue;
      }

      // add net
      Attrib* attrib;
      CString netStatus;

      if (net->getAttributesRef() && net->getAttributesRef()->Lookup(netTypeKW, attrib))
         netStatus = get_attvalue_string(pDoc,attrib);

      m_netMap.SetAt(net->getNetName(), new CIplNet(pDoc, net, netCount++, netStatus));

      // add comp pins
      m_compMap.AddNetComponents(pDoc, pFile, net, getDescriptionAttribName(), m_iplLogFp);
   }
   
   // Gather only PLACED probes, we want only placed probes in the list
	// while all the fancy assignment of channels using "the rules" (channel
	// spacing and so on) is taking place. Unplaced probes will be added in
	// when that stage is complete.
   pos = pFile->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = pFile->getBlock()->getDataList().GetNext(pos);

      if (data != NULL && data->isInsertType(insertTypeTestProbe))
      {
         CIplTestProbe *probe = new CIplTestProbe(pDoc, data);

         bool skip = (skipPowerInjectionProbesFlag && probe->IsPowerInjection());

		   if (probe->IsPlaced() && !skip)
		   {
			   testProbes.AddTail(probe);

			   CIplNet *iplNet = m_netMap.Lookup(probe->GetNetName());
			   if (iplNet)
				   iplNet->AddTestProbe(probe);
		   }
		   else
			   delete probe;
      }

   }

   
   // write status to the log file
   fprintf(m_iplLogFp, "Found %d probes.\n", testProbes.GetCount());

   return true;
}

/******************************************************************************
*/
void CIplWriter::gatherAndAssignUnplacedProbes()
{
	// Append unplaced probes to the probe list.
	// Assign channels as you go, starting with highest placed probe channel
	// number plus one.

   int chanNum = 0;
	CChannel *chan = channelNumberStats.GetHighestUsedChannel();
	if (chan != NULL)
      chanNum = chan->GetChannelNumber() + 1;

   if (m_unplacedProbeStartChannel > 0)
   {
      if (m_unplacedProbeStartChannel < chanNum /*which is based on placed probes*/)
      {
         int newChanNum = ((chanNum / 1000) + 1) * 1000;
         CString msg;
         msg.Format("Unplaced probe starting number (from ipl_18xx.out: %d) overlaps already placed probes (highest probe: %d)\nUnplaced probe numbering will start at %d.",
            m_unplacedProbeStartChannel, chanNum, newChanNum);
         ErrorMessage(msg, "");
         chanNum = newChanNum;
      }
      else
      {
         chanNum = m_unplacedProbeStartChannel;
      }
   }

   if (chanNum < 1) // wasn't set by any means above
      chanNum = 1001; // backward compatible for Spectrum

   // gather unplaced test probes
   POSITION pos = pFile->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = pFile->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)
         continue;

      CIplTestProbe *probe = new CIplTestProbe(pDoc, data);

		if (!probe->IsPlaced())
		{
			CChannel *channel = channelNumberStats.AddNewChannel(chanNum++);
			probe->SetChannel(channel);

         if (channelNumberStats.DoRemapping())
            probe->SetRefname(channel->GetChannelNumber());

			testProbes.AddTail(probe);

			CIplNet *iplNet = m_netMap.Lookup(probe->GetNetName());
			if (iplNet)
				iplNet->AddTestProbe(probe);
		}
		else
			delete probe;

   }
}

/******************************************************************************
* CIplWriter::readSettingsFile
*/
int CIplWriter::readSettingsFile(CString filename)
{
   CFileException e;
   CFormatStdioFile file;
   CString line;

   if (!file.Open(filename, CFile::modeRead, &e))
   {
      // no settings file found
      CString tmp;
      char msg[255];
      if (e.m_cause != e.none && e.GetErrorMessage(msg, 255))
         tmp.Format("%s [%s]", msg, filename);
      else
         tmp.Format("File [%s] not found", filename);

      ErrorMessage(tmp, "IPL Settings", MB_OK | MB_ICONHAND);
      fprintf(m_iplLogFp, tmp + "\n");

      return e.m_cause;
   }

   while (file.ReadString(line))
   {
      int tokPos = 0;      
      CString tok = line.Tokenize(" \t\n", tokPos);
      if (tok.IsEmpty())
         continue;

      if (tok[0] == '.')
      {
         // here do it.
         if (!STRCMPI(tok, ".PREASSIGNED_CHANNEL"))
         {
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            int channelNum = atoi(tok);

            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            CString net = tok;

            if (channelNum >= 0 && !net.IsEmpty())
               channelNumberStats.AddPreassignedChannel(net, channelNum);
         }
         else if (!STRCMPI(tok, ".RESERVED_CHANNELS"))
         {
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            CString resChannels = tok;

            channelNumberStats.SetReservedChannels(resChannels);
         }
         else if (!STRCMPI(tok, ".CHANNEL_SPACING"))
         {
            EChannelSpacing channelSpacing = ChannelSpacing16;

            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            if (tok == "CSPACE_32")
               channelSpacing = ChannelSpacing32;

            channelNumberStats.SetChannelSpacing(channelSpacing);
         }
         else if (!STRCMPI(tok, ".MAKE_GAIN_TEST"))
         {
            // get level
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            int val1 = atoi(tok);

            // get current
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            int val2 = atoi(tok);

            createGainTests = true;
            gainLevel = val1;
            gainCollectorCurrent = val2;
         }
         else if (!STRCMPI(tok, ".MAX_LINE_LENGTH"))
         {
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            int lineLength = atoi(tok);

            if (lineLength < DEFAULT_MIN_LINE_LENGTH)
               maxLineLength = DEFAULT_MIN_LINE_LENGTH;
            else if (lineLength > DEFAULT_MAX_LINE_LENGTH)
               maxLineLength = DEFAULT_MAX_LINE_LENGTH;
            else
               maxLineLength = lineLength;
         }
         else if (!STRCMPI(tok, ".CAP_DISCHARGE_MIN"))
         {
            // get discharge value
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            dischargeMin.SetValue(tok);
         }
         else if (!STRCMPI(tok,".APPLY_TESTPIN_REMAPPING"))
         {
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;

            if (!tok.CompareNoCase("Y"))
               channelNumberStats.SetRemappingFlag(true);
         } 
         else if (!STRCMPI(tok,".DESCRIPTION"))
         {
            tok = line.Tokenize("\"\t\n", tokPos);  // do not break on embedded space chars
            if (tok.IsEmpty())
               continue;

            m_descriptionAttribName = tok;
				m_descriptionAttribName.Trim();
         } 
         else if (!STRCMPI(tok,".DECIMAL_PRECISION"))
         {
            tok = line.Tokenize(" \t\n", tokPos);
            if (tok.IsEmpty())
               continue;
            m_decimalPlaces = atoi(tok);
            if (m_decimalPlaces < 1 || m_decimalPlaces > 7)
               m_decimalPlaces = 3;
         }
         else if (!STRCMPI(tok,".UNPLACED_PROBE_START_CHANNEL"))
         {
            tok = line.Tokenize(" \t\n", tokPos);
            if (!tok.IsEmpty())
               m_unplacedProbeStartChannel = atoi(tok);
         }
      }
   } // END while (file.ReadString(line)) ...

   file.Close();
   return 0;
}


/******************************************************************************
* CIplWriter::assignChannelNumbers
*/
int CIplWriter::assignChannelNumbers()
{
   if (channelNumberStats.DoRemapping())
      channelNumberStats.SetPreassignedChannels(pDoc, pFile, &m_netMap, m_iplLogFp);

   // loop through the components to make sure the transistors pins are enough channel numbers apart
   POSITION pos = m_compMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplComponent *iplComp;

      m_compMap.GetNextAssoc(pos, key, iplComp);

      iplComp->ProcessProbeChannelNumbers(&channelNumberStats, &m_netMap);
   }


   // loop through the nets to make sure they are enough channel numbers apart on a single net  
   pos = m_netMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplNet *iplNet = NULL;

      m_netMap.GetNextAssoc(pos, key, iplNet);

      iplNet->ProcessProbeChannelNumbers(&channelNumberStats);
   }

   // save back the newly assigned channel number if we ran the remapping
   if (channelNumberStats.DoRemapping())
      testProbes.BackAssignChannelNumbers();

   return 0;
}

/******************************************************************************
* CIplWriter::IsProbeNameNumeric
*/
BOOL CIplWriter::IsProbeNameNumeric(CString name)
{
   for (int i=0; i< name.GetLength(); i++)
   {
      if (!isdigit(name.GetAt(i)))
         return FALSE;
   }
   return  TRUE;
}

/******************************************************************************
* CIplWriter::splitLine
*/
CString CIplWriter::splitLine(CString line)
{
   if (line.IsEmpty())
      return "";

   // split the line if necessary base on user settings
   CString retString;
   int pos = line.Find(",", maxLineLength);
   while (pos > 0)
   {
      retString += line.Mid(0, pos+1) + "\n";
      line = (CString)"   " + line.Mid(pos+1);
      pos = line.Find(",", maxLineLength);
   }
   retString += line;

   return retString;
}

/******************************************************************************
* CIplWriter::DumpSettings
*/
void CIplWriter::DumpSettings(CFormatStdioFile *file)
{
   file->WriteString("/*                                                     */\n");
   file->WriteString("/* --------------------------------------------------- */\n");
   file->WriteString("/* ---------------- CURRENT SETTINGS ----------------- */\n");

   // write current line length
   {
      file->WriteString("/* --------------------------------------------------- */\n");
      file->WriteString("/*                                                     */\n");
      file->WriteString("/* .MAX_LINE_LENGTH %-34d */\n", maxLineLength);
      file->WriteString("/*                                                     */\n");
   }

   // write preassigned channel numbers
   {
      CString preassignedChannels = channelNumberStats.GetPreassignedChannels();
      if (!preassignedChannels.IsEmpty())
      {
         file->WriteString("/* --------------------------------------------------- */\n");
         file->WriteString("/*                                                     */\n");
         file->WriteString(preassignedChannels);
         file->WriteString("/*                                                     */\n");
      }
   }

   // write reserved channel numbers
   {
      CString resChannels = channelNumberStats.GetReservedChannels();
      if (!resChannels.IsEmpty())
      {
         file->WriteString("/* --------------------------------------------------- */\n");
         file->WriteString("/*                                                     */\n");
         file->WriteString("/* .RESERVED_CHANNELS %-32s */\n", resChannels);
         file->WriteString("/*                                                     */\n");
      }
   }

   // write channel spacing setting
   {
      file->WriteString("/* --------------------------------------------------- */\n");
      file->WriteString("/*                                                     */\n");
      file->WriteString("/* .CHANNEL_SPACING CSPACE_%d */\n", EChannelSpacingToInteger(channelNumberStats.GetChannelSpacing()));
      file->WriteString("/*                                                     */\n");
   }

   // write gain test settings
   {
      file->WriteString("/* --------------------------------------------------- */\n");
      file->WriteString("/*                                                     */\n");

      CString buf, resString;
      buf.Format("/* .MAKE_GAIN_TEST %d %d", gainLevel, gainCollectorCurrent);
      resString.Format("%-*s */\n", 54, buf);
      file->WriteString(resString);
      
      file->WriteString("/*                                                     */\n");
   }

   // write discharge minimum settings
   {
      file->WriteString("/* --------------------------------------------------- */\n");
      file->WriteString("/*                                                     */\n");
      file->WriteString("/* .CAP_DISCHARGE_MIN %-32s */\n", dischargeMin.GetPrintableString());
      file->WriteString("/*                                                     */\n");
   }

   // write remap settings
   {
      file->WriteString("/* --------------------------------------------------- */\n");
      file->WriteString("/*                                                     */\n");
      file->WriteString("/* .APPLY_TESTPIN_REMAPPING %-26s */\n", (channelNumberStats.DoRemapping()?"Y":"N"));
      file->WriteString("/*                                                     */\n");
   }

   file->WriteString("/* --------------------------------------------------- */\n");
   file->WriteString("/* --------------------------------------------------- */\n");
}

/******************************************************************************
* CIplWriter::writeGND
*/
void CIplWriter::writeGND(CFormatStdioFile *file)
{
   CIplSortedArray sortedArray;

   // find the channels for the ground net
   POSITION pos = m_netMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplNet *iplNet = NULL;

      m_netMap.GetNextAssoc(pos, key, iplNet);

      if (!iplNet->IsGroundNet())
         continue;

      // gather all the channel numbers for the test probes on this net
      CIplTestProbe *tp = iplNet->GetFirstTestProbe();
      while (tp)
      {
         sortedArray.Add(tp->GetChannelNumber());

         // stop at 5 channels
         if (sortedArray.GetCount() >= 5)
            break;

         iplNet->GetNextTestProbe(tp);
      }

      // stop at 5 channels
      if (sortedArray.GetCount() >= 5)
         break;
   }

   // only output if there were test probes on ground nets
   if (sortedArray.GetCount() > 0)
      file->WriteString("GND, , ,%s\n", sortedArray.GetDelimitedString(",", true));
}

/******************************************************************************
* CIplWriter::writeSH
*/
void CIplWriter::writeSH(CFormatStdioFile *file)
{
   CIplSortedArray sortedArray;

   POSITION pos = m_netMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplNet *iplNet = NULL;
      m_netMap.GetNextAssoc(pos, key, iplNet);

      CIplTestProbe *tp = iplNet->GetFirstTestProbe();

      if (tp != NULL)
         sortedArray.Add(tp->GetChannelNumber());
   }
   if (sortedArray.GetCount() > 0)
	{
		CString buf("SH," + sortedArray.GetDelimitedString(",", true));
      file->WriteString("%s\n", splitLine(buf));
	}
}

/******************************************************************************
* CIplWriter::writeAPC
*/
void CIplWriter::writeAPC(CFormatStdioFile *file)
{
   CIplSortedArray sortedArray;

   POSITION pos = testProbes.GetHeadPosition();
   while (pos)
   {
      CIplTestProbe *tp = testProbes.GetNext(pos);
      if(tp == NULL) continue;

      if(tp->IsProbedDiePin())
      {
         fprintf(m_iplLogFp, "%s(APC) - Net: %s Probe $%s - Skipped Probe for Die pin.\n", file->GetFileName(),
            tp->GetNetName(), tp->GetName());
      }
      else if (tp->GetChannelNumber() != CHNL_UNASSIGNED)
         sortedArray.Add(tp->GetChannelNumber());
   }
   if (sortedArray.GetCount() > 0)
      file->WriteString("APC,%s\n", sortedArray.GetDelimitedString(",", true));
}

/******************************************************************************
* CIplWriter::writeCONT
*/
void CIplWriter::writeCONT(CFormatStdioFile *file)
{
   CStringList strList;

   POSITION pos = m_netMap.GetStartPosition();
   while (pos)
   {
      CString key, channelNumbers;
      CIplNet *iplNet = NULL;
      m_netMap.GetNextAssoc(pos, key, iplNet);

      if (iplNet->GetTestProbeCount() < 2)
         continue;

      CIplTestProbe *tp = iplNet->GetFirstTestProbe();
      while (tp != NULL)
      {
         CString temp;
         if(tp->IsProbedDiePin())
         {
            fprintf(m_iplLogFp, "%s(CONT) - Net: %s Probe $%s - Skipped Probe for Die pin.\n", file->GetFileName(),
               tp->GetNetName(), tp->GetName());
         }
         else if (channelNumbers.IsEmpty())
            temp.Format("%d", tp->GetChannelNumber());
         else
            temp.Format("&%d", tp->GetChannelNumber());
         channelNumbers += temp;
         iplNet->GetNextTestProbe(tp);
      }
      strList.AddTail(channelNumbers);
   }
   if (strList.GetCount() > 0)
   {
      CString channelNumbers;

      pos = strList.GetHeadPosition();
      while (pos)
      {
         if (channelNumbers.IsEmpty())
            channelNumbers += strList.GetNext(pos);
         else
            channelNumbers += (CString)"," + strList.GetNext(pos);
      }

		CString buf("CONT," + channelNumbers);
      file->WriteString("%s\n", splitLine(buf));
   }
}

/******************************************************************************
* CIplWriter::writePWR5
*/
void CIplWriter::writePWR5(CFormatStdioFile *file)
{
   CIplNet *iplPowerNet = NULL;
   CIplNet *iplGroundNet = NULL;

   // find the ground and power net
   POSITION pos = m_netMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplNet *iplNet = NULL;

      m_netMap.GetNextAssoc(pos, key, iplNet);

      if (iplGroundNet == NULL && iplNet->IsGroundNet())
         iplGroundNet = iplNet;
      if (iplPowerNet == NULL && iplNet->IsPowerNet())
         iplPowerNet = iplNet;

      // stop looking if we found one of each
      if (iplGroundNet != NULL && iplPowerNet != NULL)
         break;
   }

   // write the power entry if there is a power net found
   if (iplPowerNet != NULL)
   {
      long powerChannelNumber = 0;
      long groundChannelNumber = 0;

      CIplTestProbe *iplTP = iplPowerNet->GetFirstTestProbe();
      if (iplTP == NULL)
         return;
         
      powerChannelNumber = iplTP->GetChannelNumber();

      iplTP = iplGroundNet != NULL ? iplGroundNet->GetFirstTestProbe() : NULL;

      if (iplTP != NULL)
         groundChannelNumber = iplTP->GetChannelNumber();

      file->WriteString("PWR5,pwr,\"5 Volt\",%d,%d\n", powerChannelNumber, groundChannelNumber);
   }
}
/******************************************************************************
* CIplWriter::writeOneCapOpensTest
*/
void CIplWriter::writeOneCapOpensTest(CFormatStdioFile *file, CIplComponent *iplComp)
{
	if (file != NULL && iplComp != NULL && iplComp->IsCapOpensQualified())
   {
		CString outbuf;

		DeviceTypeTag type = iplComp->GetType();

		if (type == deviceTypeConnector || type == deviceTypeIC || 
			type == deviceTypeICDigital || type == deviceTypeICLinear)
		{
			outbuf = iplComp->GetIplFSPlusString(&m_netMap, m_iplLogFp);
		}
		else
		{
			outbuf = iplComp->GetIplCapScanString(&m_netMap, m_iplLogFp);
		}

      

      if (!outbuf.IsEmpty())
      {
         file->WriteString("%s\n", splitLine(outbuf));
		}
	}
}

/******************************************************************************
* CIplWriter::writeOneComponent
*/
void CIplWriter::writeOneComponent(CFormatStdioFile *file, CIplComponent *iplComp)
{
   if (file && iplComp)
   {
      // get the ipl line and break the line base on the line length
      CString temp = iplComp->GetIplString(&m_netMap, m_iplLogFp);

      if (!temp.IsEmpty())
      {
         // split the line if necessary base on user settings
         CString compLine = splitLine(temp);
         file->WriteString("%s\n", compLine);

         // see if we need to write out the gain tests and write it if necessary
         if (createGainTests && iplComp->GetGainTestString(temp, gainLevel, gainCollectorCurrent, &m_netMap, m_iplLogFp))
         {
            // split the line if necessary base on user settings
            compLine = splitLine(temp);
            file->WriteString("%s\n", compLine);
         }

         // see if we need to write out the discharge line for capacitors
         if (dischargeMin.IsValid() && 
            (iplComp->GetType() == deviceTypeCapacitor || iplComp->GetType() == deviceTypeCapacitorPolarized || iplComp->GetType() == deviceTypeCapacitorTantalum) &&
            ((CIplComponentValTolTol*)iplComp)->GetDischargeString(temp, dischargeMin, &m_netMap, m_iplLogFp))
         {
            // split the line if necessary base on user settings
            compLine = splitLine(temp);
            file->WriteString("%s\n", compLine);
         }
      }
   }
}

/******************************************************************************
* CIplWriter::writeComponents
*/
void CIplWriter::writeComponents(CFormatStdioFile *file)
{
	// CIplTestOrder really is ordering by device type, not by testing.
	// It orders the tests only by virtue that most parts get a single kind
	// of test. But Cap Opens is an exception. It could be applied to several
	// device types, but we always want that test last (per Mark). So it is handled
	// as a special case "device type".

   CIplTestOrder iplTestOrder;
   POSITION orderPos;
   for (orderPos = iplTestOrder.GetHeadPosition(); orderPos != NULL;)
   {
      DeviceTypeTag nowServingDevType = iplTestOrder.GetNext(orderPos);

      // Collect components of "now serving" type into list sorted by refdes
      CList<CIplComponent*> sortedByRefdes;
      POSITION pos = m_compMap.GetStartPosition();
      while (pos)
      {
         CString key;
         CIplComponent *iplComp = NULL;

         m_compMap.GetNextAssoc(pos, key, iplComp);

         if (iplComp && (iplComp->GetType() == nowServingDevType || nowServingDevType == CAP_OPENS_TEST))
         {
            CIplSmartRefdes newsmRefdes(iplComp->GetName());
				POSITION sbrPos = NULL;
            for (sbrPos = sortedByRefdes.GetHeadPosition(); sbrPos != NULL;)
            {
               CIplComponent *curIplComp = sortedByRefdes.GetAt(sbrPos);
               CIplSmartRefdes cursmRefdes(curIplComp->GetName());
               if (newsmRefdes < cursmRefdes) break;
               sortedByRefdes.GetNext(sbrPos);
            }
            if (sbrPos)
               sortedByRefdes.InsertBefore(sbrPos, iplComp);
            else
               sortedByRefdes.AddTail(iplComp);
         }
      }

      // Output the parts from the sorted list
      for (POSITION sbrPos = sortedByRefdes.GetHeadPosition(); sbrPos != NULL;)
      {
         CIplComponent *iplComp = sortedByRefdes.GetNext(sbrPos);
			if (nowServingDevType == CAP_OPENS_TEST)
			{
				// Special situation for cap opens
				writeOneCapOpensTest(file, iplComp);
			}
			else
			{
				// Standard comp testing based on dev type
				writeOneComponent(file, iplComp);
			}
      }
      sortedByRefdes.RemoveAll();
   }
}

/******************************************************************************
* CIplWriter::WriteFile
*/
int CIplWriter::writeIPL(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "IPL.DAT", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("/* --------------------------------------------------- */\n");
      outFile.WriteString("/* %-51s */\n", outFile.GetFileName().MakeUpper());
      outFile.WriteString("/* Created by CCE to ODB++ v.%-31s */\n", getApp().getVersionString());
      outFile.WriteString("/* %-51s */\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("/* --------------------------------------------------- */\n");
   }

   DumpSettings(&outFile);

   // output all file info
   ///// command section
   //////// Header
   outFile.WriteString("IPL, \"%s\"\n", pFile ? pFile->getName() : "");

   //////// GND
   writeGND(&outFile);

   //////// SH (list of unique probed nets that would create a short)
   writeSH(&outFile);

   //////// APC (List of all channel numbers)
   writeAPC(&outFile);

   //////// CONT (Channel numbers of probes on a single net (never just one))
   writeCONT(&outFile);

   //////// PWR5
   writePWR5(&outFile);

   ///// component section
   writeComponents(&outFile);

   ///// END
   outFile.WriteString("END\n");

   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeBoardOutlin
*/
int CIplWriter::writeBoardOutline(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "BdOutline.txt", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  This is a list of board edge points, all straight line segments.\n");
      outFile.WriteString(";\n");
      outFile.WriteString("OUTLINE\n");
   }

   DataStruct *brdOutline = NULL;
   // look for the board outline

   if (pFile != NULL)
   {
      POSITION pos  = pFile->getBlock()->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *data = pFile->getBlock()->getDataList().GetNext(pos);

         if (data->getDataType() != T_POLY)
            continue;

         if (data->getGraphicClass() != GR_CLASS_BOARDOUTLINE)
            continue;

         brdOutline = data;
         break;
      }
   }

   if (brdOutline != NULL)
   {
      POSITION polyPos = brdOutline->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = brdOutline->getPolyList()->GetNext(polyPos);

         POSITION pntPos = poly->getPntList().GetHeadPosition();
         while (pntPos)
         {
            CPnt *pnt = poly->getPntList().GetNext(pntPos);

            outFile.WriteString("    %.*f,  %.*f\n", 
               m_decimalPlaces, pnt->x, m_decimalPlaces, pnt->y);
         }
      }
   }
   else
      fprintf(m_iplLogFp, "Warning:  No board outline found\n");

   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeBoard
*/
int CIplWriter::writeBoard(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Board.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  Description:\n");
      outFile.WriteString(";     First record: Datum Origin - Board Location\n");
      outFile.WriteString(";        <x> <y> - origin location\n");
      outFile.WriteString(";        <rot> - board rotation (degrees) around origin + CCW\n");
      outFile.WriteString(";     Second and following records: Datum Reference - mechanical hole(s) in the board, typically tooling hole(s)\n");
      outFile.WriteString(";        <x> <y> - hole location relative to Datum Origin\n");
      outFile.WriteString(";        <rot> - holes do not have a rotation property (XXX used)\n");
      outFile.WriteString(";        <name> - name of tooling hole\n");
      outFile.WriteString(";\n");
      outFile.WriteString("User Datums             X         Y         Rotation  Name\n");
      outFile.WriteString("                                            or Grid\n");
   }

   // write the board's location
   if (pFile != NULL)
   {
      outFile.WriteString("%-24s %-9.3f %-9.3f %-.3f\n", "Datum Origin", pFile->getInsertX(), pFile->getInsertY(),
         RadToDeg(pFile->getRotation()));


      // set up the transformation
      CTMatrix transform;
      transform.rotateRadians(pFile->getRotation());
      transform.translate(pFile->getInsertX(), pFile->getInsertY());

      POSITION pos = pFile->getBlock()->getDataList().GetHeadPosition();
      while (pos)
      {
         DataStruct *data = pFile->getBlock()->getDataList().GetNext(pos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (data->getInsert()->getInsertType() != INSERTTYPE_TOOLING)
            continue;

         CPoint2d toolLocation(data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
         transform.transform(toolLocation);

         outFile.WriteString("%-24s %-9.3f %-9.3f %-9s %s\n", "Datum Reference", toolLocation.x, toolLocation.y,
            "XXX", data->getInsert()->getRefname());
      }
   }

   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeIndXRef
*/
int CIplWriter::writeIndXRef(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "IndXRef.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  Description:\n");
      outFile.WriteString(";  Tech=CSCAN  - Capactive open test technique requested on a capacitor\n");
      outFile.WriteString(";  1     - line number\n");
      outFile.WriteString(";  PN-capleaded   - device name\n");
      outFile.WriteString(";  TOP      - mounting surface (TOP, BOTTOM)\n");
      outFile.WriteString(";  0.000       - device height (O if unknown)\n");
      outFile.WriteString(";  X:6.000  Y:-1.100  - device location\n");
      outFile.WriteString(";  dX:0.300  dY:0.100 - device size\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  TYPE   IND# ID   NAME    SIDE   HEIGHT   X-center  Y-center   X-delta   Y-delta\n");
      outFile.WriteString(";\n");
   }

   // TODO: Write IndXRef file (Can't write anything unless I have test techniques)
#ifdef JUSTCOMMENT
   Format is:
 
For each component with a Cap. Opens attribute set to True write the following line:
 
Tech=<tech> <ind#>   <refdes>     <pn>    <side>    <height>   <XC>   <YC>    <Xside>    <Yside>
 
Where:
 
<tech> is FSCAN is used when Cap. Opens attribute on component is true and devicetype is connector, IC, IC_Linear, IC_Digital
<tech> is CSCAN is used when Cap. Opens attribute on component is true and devicetype is Capacitor, Capacitor_Polarized, Capacitor_Tantalum
 
<ind#> is a unique numeric transducer number starting from 0 and incrementing.
 
<refdes> is the refdes name of the component with the Cap. Opens attribute set to True.
 
<pn> is the part number (PARTNUMBER attribute) of the component.
 
<side> is the side of the component (TOP or BOTTOM)
 
<height> is defaulted to 0.0
 
<XC> <YC> is the centroid location of the component
 
<Xside> <Yside> is the X and Y dimensions of the part
#endif

   // write components that have CAP_SCAN or FRAME_SCAN tests
   CString *ignoredKey;
   CIplComponent *iplComp = NULL;
   for (m_compMap.GetFirstSorted(ignoredKey, iplComp); iplComp != NULL; m_compMap.GetNextSorted(ignoredKey, iplComp)) 
   {
      if (iplComp->IsCapOpensQualified())
      {
         DeviceTypeTag type = iplComp->GetType();

			CString scanTech = "CSCAN"; // Default, for all capacitors
			if (type == deviceTypeConnector || type == deviceTypeIC || 
				type == deviceTypeICDigital || type == deviceTypeICLinear)
			{
				scanTech = "FSCAN";
			}

			CPoint2d centroid = iplComp->GetCentroidLocation();
			CExtent extent = iplComp->GetExtent();

			CString str;
			str.Format("Tech=%s  %3d  %-4s  %-12s  %-6s  %0.3f  X:%0.3f    Y:%0.3f    dX:%0.3f    dY:%0.3f", 
				scanTech,
				iplComp->GetSensorNumber(),
				cleanRefdes(iplComp->GetName()),
				iplComp->GetPartNumber(),
				iplComp->IsOnTop() ? "TOP" : "BOTTOM",
				iplComp->GetCompHeight(),
				centroid.x, centroid.y,
				extent.getXsize(), extent.getYsize());

			outFile.WriteString("%s\n", str);

		}
   }

   outFile.Close();

   return 0;
}

int CIplWriter::AscendingChannelNumberSortFunc(const void *a, const void *b)
{
   CIplTestProbe* probeA = (CIplTestProbe*)(((SElement*) a )->pObject->m_object);
   CIplTestProbe* probeB = (CIplTestProbe*)(((SElement*) b )->pObject->m_object);

   // Case 2217 asks for "simple numeric sort", which you know is different
   // from a lexical sort.

   long pA = probeA->GetChannelNumber();
   long pB = probeB->GetChannelNumber();

   return (pA - pB);
}

/******************************************************************************
* CIplWriter::writeNails
*/
int CIplWriter::writeNails(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Nails.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString("; Descripion:\n");
      outFile.WriteString(";  $0 - channel number\n");
      outFile.WriteString(";  <x> <y> - probe location\n");
      outFile.WriteString(";  <type> - Barrel size code, map to the 50, 75, 100 mil diameter\n");
      outFile.WriteString(";        probe fixture hole diameter. Provide this note in a log file.\n");
      outFile.WriteString(";       \"Nails.asc: Type code maps to Barrel Size\n");
      outFile.WriteString(";        Type 1, Size 100\"\n");
      outFile.WriteString(";  <grid> - nail grid location - not supported\n");
      outFile.WriteString(";  <T/B> - probe surface (B = bottom, T = top)\n");
      outFile.WriteString(";  <net> - net number\n");
      outFile.WriteString(";  <net name> - net name\n");
      outFile.WriteString(";  <virtual> - 'Virtual' is constant V\n"); // case 2025 changed this from unused to constant "V"
      outFile.WriteString(";  <pin/via> - probe logical location (PIN - device pin U5.14,\n");
      outFile.WriteString(";        VIA = Via named via123)\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  $32000   = an optional probable location on the net that was not\n");
      outFile.WriteString(";        needed because other locations were used. (The test\n");
      outFile.WriteString(";        engineer could select one of these locations as an\n");
      outFile.WriteString(";        alternate or additional probe location depending\n");
      outFile.WriteString(";        on test requirements.) This section is listed last.\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";\n");
      outFile.WriteString("Nail          X             Y             Type          Grid          T/B           Net           Net Name      Virtual       Pin/Via\n");
   }

   // Case 2217 asks for simple numeric sort on first column (Refname)
   CMapSortedStringToOb<CIplTestProbe> sortedProbes;
   POSITION pos = testProbes.GetHeadPosition();
   // Ultimately the value of the key is unimportant, it is only important that
   // each iplTP in the original list get a unique key, a simple int counter will do.
   int key = 0;
   CString keyStr;
   while (pos)
   {
      CIplTestProbe *iplTP = new CIplTestProbe(testProbes.GetNext(pos)); // CMapSorted... is a container style collection, needs its own copy
      keyStr.Format("%d", key++);
      sortedProbes.SetAt(keyStr, iplTP);
   }

   sortedProbes.setSortFunction(&CIplWriter::AscendingChannelNumberSortFunc);
   sortedProbes.Sort();

   CString *ignoredKey;
   CIplTestProbe *iplTP;
   CMapBlockToProbeVal BlockToProbeMap;
   for (sortedProbes.GetFirstSorted(ignoredKey, iplTP); iplTP != NULL; sortedProbes.GetNextSorted(ignoredKey, iplTP))
   {
      if (iplTP->IsPlaced())
      {
         CIplNet *iplNet = m_netMap.Lookup(iplTP->GetNetName());

         int channelNumber = iplTP->GetChannelNumber();
         if (iplTP->IsPowerInjection())
            channelNumber = 10000;
         else if (iplTP->GetChannelNumber() == CHNL_UNASSIGNED)
            channelNumber = 32000;

         if (iplNet == NULL)
            continue;

         // case 2025 asks for constant V in this field
         CString virtualVal("V"); 

         bool IsDiePin = false;
         // get the Pin/Via section
         CString pinVia;
         if (EN_COMPPIN == iplTP->GetProbedType())
         {
            pinVia.Format("PIN %s.%s", iplTP->GetProbedItemRefName(), iplTP->GetProbedItemPinName());
            IsDiePin = iplNet->GetNet()->IsDiePin(iplTP->GetProbedItemRefName(), iplTP->GetProbedItemPinName());
         }
         else
            if (EN_DATA == iplTP->GetProbedType())
            {
               CString viaName = iplTP->GetProbedItemRefName();
               pinVia.Format("VIA %s", viaName.IsEmpty()?".":viaName);
            }
         if(!IsDiePin)
         {
            BlockStruct *block = pDoc->getBlockAt(iplTP->GetProbeBlockNumber());
            int ProbeNumber = BlockToProbeMap.LookupProbeNumber(block);
            outFile.WriteString("$%-12d %-13.*f %-13.*f %-13d %-13s (%s)%10s #%-12d %-13s %-13s %s\n",
               channelNumber, 
               m_decimalPlaces, iplTP->GetXLocation(), m_decimalPlaces, iplTP->GetYLocation(),
               ProbeNumber, iplTP->GetGridLocation(),
               iplTP->IsOnTop()?"T":"B", "", iplNet->GetNumber(), cleanNetname(iplNet->GetName()), 
               virtualVal, pinVia);
         }
         else
         {
            fprintf(m_iplLogFp, "%s - Net: %s CompPin %s.%s - Skipped CompPin for Die component.\n", outFile.GetFileName(),
               iplNet->GetName(), iplTP->GetProbedItemRefName(), iplTP->GetProbedItemPinName());
         }

      }
   }

   CStringArray probeTable;
   BlockToProbeMap.GetProbeTable(probeTable);
   for(int i = 0; i < probeTable.GetCount(); i++)
   {
      CString lines = probeTable.GetAt(i);
      fprintf(m_iplLogFp, "%s",lines.GetBuffer(0));
   }
   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeNameXRef
*/
int CIplWriter::writeNameXRef(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "NameXRef.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  This file list any name changes that may have occured during processing.\n");
      outFile.WriteString(";  Some tester have restrictions on what is considered valid characters in\n");
      outFile.WriteString(";  a net or device. Some do not like $, \\, _, or other special characters in\n");
      outFile.WriteString(";  the names. In both sections of this file, the first field is the new name\n");
      outFile.WriteString(";  (that will appear in the output files) and the second name is the original\n");
      outFile.WriteString(";  name assigned by the designer.\n");
      outFile.WriteString(";  Example: \"A(1)\" = \"A_1$\"\n");
      outFile.WriteString(";     \"A(1)\" = new \"clean\" name\n");
      outFile.WriteString(";     \"A_1$\" = old name with invalid characters\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  Description:\n");
      outFile.WriteString(";  NETS section: net name changes\n");
      outFile.WriteString(";  IDS section: reference designator and device names\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  Each record in a section ends in a comma, the end of the section ends\n");
      outFile.WriteString(";  in a semicolon.\n");
      outFile.WriteString(";\n");
   }

   // write out all renames that have occured (even those not renamed)
   outFile.WriteString("NETS:\n");  // Mark asked for colon via email 27 Apr 07

   CString *ignoredKey;
   CIplNet *thisIplNet = NULL;
   CIplNet *nextIplNet = NULL;
   m_netMap.GetFirstSorted(ignoredKey, thisIplNet);
   while (thisIplNet != NULL) 
   {
      m_netMap.GetNextSorted(ignoredKey, nextIplNet);

      outFile.WriteString("\"%s\" = \"%s\"%s\n", 
         cleanNetname(thisIplNet->GetName()), cleanNetname(thisIplNet->GetName()),
         nextIplNet != NULL ? "," : ";");

      thisIplNet = nextIplNet;
   }

   // write out all renames that have occured (even those not renamed)
   outFile.WriteString("\nIDS:\n");  // Mark asked for colon via email 27 Apr 07

   CIplComponent *thisIplComp;
   CIplComponent *nextIplComp;
   m_compMap.GetFirstSorted(ignoredKey, thisIplComp);
   while (thisIplComp != NULL)
   {
      m_compMap.GetNextSorted(ignoredKey, nextIplComp);

      outFile.WriteString("\"%s\" = \"%s\"%s\n", 
         cleanRefdes(thisIplComp->GetName()), cleanRefdes(thisIplComp->GetName()),
         nextIplComp != NULL ? "," : ";");

      thisIplComp = nextIplComp;
   }
   


   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeNet2Nail
*/
int CIplWriter::writeNet2Nail(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Net2Nail.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString("; Description;\n");
      outFile.WriteString(";  \"A(0)\" = net name, quotes required\n");
      outFile.WriteString(";  #1     = net number\n");
      outFile.WriteString(";  ($36)  = channel number\n");
      outFile.WriteString(";  1      = number of channels assiged to the net\n");
      outFile.WriteString(";  ;      = end on record (semicolon)\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  **     = if net is not probed then the channel number is $9999,\n");
      outFile.WriteString(";        then list like this .... ($9999) 0;\n");
      outFile.WriteString(";      if net has more probable locations than used, ...\n");
      outFile.WriteString(";        then list like this ... ($2,$52,$10000) 3;\n");
      outFile.WriteString(";\n");
   }

   CIplSortedArray sortedArray;

   // write out all the nets and their channel numbers
   CString *ignoredKey;
   CIplNet *iplNet = NULL;
   for (m_netMap.GetFirstSorted(ignoredKey, iplNet); iplNet != NULL; m_netMap.GetNextSorted(ignoredKey, iplNet)) 
	{
      sortedArray.RemoveAll();
           
      bool powerInjectionProbeAdded = false;
      int probeCount = 0;

      // Collect power injection and placed probes only into output sortedArray.
      // Skip unplaced probes.
      CIplTestProbe *iplTP = iplNet->GetFirstTestProbe(); 
      while (iplTP != NULL)
      {
         if(iplTP->IsProbedDiePin())
         {
            fprintf(m_iplLogFp, "%s - Net: %s Probe $%s - Skipped Probe for Die pin.\n", outFile.GetFileName(),
               iplNet->GetName(), iplTP->GetName());
         }
         else if (iplTP->IsPowerInjection())
         {
            // Only add Power Injection probe once
            if (!powerInjectionProbeAdded)
            {       
               sortedArray.Add(10000);
               powerInjectionProbeAdded = true;
            }

            // This strikes me as odd, but it is consistent with code in earlier rev.
            probeCount++; // tally power inj probe count, even if no probe added for "this one"
         }
         else if (iplTP->IsPlaced())
         {
            // Ordinary placed probe
            sortedArray.Add(iplTP->GetChannelNumber());
            probeCount++;
         }
         // else skip unplaced probes

         iplNet->GetNextTestProbe(iplTP);
      }

      outFile.WriteString("\"%s\" #%d ($%s) %d;\n", cleanNetname(iplNet->GetName()), iplNet->GetNumber(),
         (sortedArray.GetCount() > 0)?sortedArray.GetDelimitedString(",$", false):CHNL_UNASSIGNED_STR,
         probeCount);
   }

   outFile.WriteString("; END OF NET2NAIL\n");
   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeNets
*/
int CIplWriter::writeNets(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Nets.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString("; Description:\n");
      outFile.WriteString(";  #1    = net number\n");
      outFile.WriteString(";  (S)   = net type (S=Signal, P = power/ground)\n");
      outFile.WriteString(";  A(1)  = net name\n");
      outFile.WriteString(";  P1.2  = a pin on the net\n");
      outFile.WriteString(";  U20.4 = a pin on the net\n");
      outFile.WriteString(";\n");
   }

   // output all the nets and their comp/pins
   POSITION pos = m_netMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplNet *iplNet = NULL;

      m_netMap.GetNextAssoc(pos, key, iplNet);

      if (iplNet == NULL)
         continue;

      outFile.WriteString("#%d (%s) %s\n", iplNet->GetNumber(),
         (iplNet->IsPowerNet() || iplNet->IsGroundNet())?"P":"S", cleanNetname(iplNet->GetName()));

      // get all the comp/pins to output
      NetStruct *net = iplNet->GetNet();
      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos)
      {
         CompPinStruct *cp = net->getNextCompPin(cpPos);

         // Refnames may be altered due to use of subclass test elements.
         // Also more than one element may use same pin, so need to process in loop to get them all.
         // If no elements use this pin then is probably a "normal" comp, no test elements used, so use 
         // cp's refname.

         CIplComponent *iplComp = this->m_compMap.LookupElement(cp->getRefDes(), cp->getPinName(), 1);
         if (iplComp != NULL)
         {
            // Output all elements
            int elementN = 1;
            while (iplComp != NULL)
            {
               outFile.WriteString("%s.%s\n", iplComp->GetName(), cp->getPinName());
               iplComp = this->m_compMap.LookupElement(cp->getRefDes(), cp->getPinName(), ++elementN);
            }
         }
         else
         {
            // Output one normal pin
            if(!cp->IsDiePin(pDoc->getCamCadData()))
			      outFile.WriteString("%s.%s\n", cp->getRefDes(), cp->getPinName());
            else
               fprintf(m_iplLogFp, "%s - Net: %s CompPin %s.%s - Skipped CompPin for Die component.\n", outFile.GetFileName(),
                  net->getNetName(), cp->getRefDes(), cp->getPinName());
         }
      }

      outFile.WriteString("\n");
   } // END while (pos) ...

   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeParts
*/
int CIplWriter::writeParts(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Parts.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString("; Filename : Parts.asc\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  Description:\n");
      outFile.WriteString(";  P1 - Reference designator\n");
      outFile.WriteString(";  1,900 0.075 - device origin location on board\n");
      outFile.WriteString(";  0  - device rotation, degrees, CCW\n");
      outFile.WriteString(";  E5   - device location on board grid\n");
      outFile.WriteString(";  (T)   - device surface (T - top, B - bottom)\n");
      outFile.WriteString(";  'PN-conn50' - device name\n");
      outFile.WriteString(";  'PINCONN50' - a device pattern graphic name\n");
      outFile.WriteString(";\n");
      outFile.WriteString("Part          X             Y             Rot           Grid          T/B           'Device', 'Outline'\n");
   }

   // write all the parts
   POSITION pos = m_compMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplComponent *iplComp = NULL;

      m_compMap.GetNextAssoc(pos, key, iplComp);

      if (iplComp == NULL)
         continue;

      outFile.WriteString("%-13s",     cleanRefdes(iplComp->GetName()));
      outFile.WriteString(" %-13.*f",  m_decimalPlaces, iplComp->GetXLocation() * outputScale);
      outFile.WriteString(" %-13.*f",  m_decimalPlaces, iplComp->GetYLocation() * outputScale);
      outFile.WriteString(" %-13.3f",  RadToDeg(iplComp->GetRotation()));
      outFile.WriteString(" %-13s",    iplComp->GetGridLocation());
      outFile.WriteString(" (%s)%10s", iplComp->IsOnTop()?"T":"B", "");
      outFile.WriteString(" '%s'",     iplComp->GetComponentDevice());
      outFile.WriteString(", '%s'\n",  iplComp->GetBlockName());
   }

   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writePins
*/
int CIplWriter::writePins(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Pins.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString(";  Description:\n");
      outFile.WriteString(";     First line:\n");
      outFile.WriteString(";        <part> - reference designator\n");
      outFile.WriteString(";        <T/B> - surface location (T = top, B = bottom)\n");
      outFile.WriteString(";        <device> - device type\n");
      outFile.WriteString(";     Second and following lines:\n");
      outFile.WriteString(";         <pin> - pin number\n");
      outFile.WriteString(";         <name> - pin name\n");
      outFile.WriteString(";         <x> - location of pin on board\n");
      outFile.WriteString(";         <y>  - location of pin on board\n");
      outFile.WriteString(";         <layer> - 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom\n");
      outFile.WriteString(";         <net> - name of net attached to the pin\n");
      outFile.WriteString(":         <nail> - channel number(s) assigned to net\n");
      outFile.WriteString(";                  32000 - alternated probe locations available (see Nails.asc)\n");
      outFile.WriteString(";\n");
      outFile.WriteString("Part          T/B  Device\n");
      outFile.WriteString("Pin           Name          X             Y             Layer         Net           Nail(s)\n");
   }

   // write every component and their pins
   POSITION pos = m_compMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplComponent *iplComp = NULL;

      m_compMap.GetNextAssoc(pos, key, iplComp);

      if (iplComp == NULL)
         continue;

      outFile.WriteString("PART %-8s (%s)  '%s'\n", cleanRefdes(iplComp->GetName()), iplComp->IsOnTop()?"T":"B",
         iplComp->GetDescription());

      // write all the pins
      CIplPin *iplPin = iplComp->GetFirstPin();
      while (iplPin != NULL)
      {
         // calculate the location of the pin in the board
         CPoint2d pinLocation(iplPin->GetXLocation(), iplPin->GetYLocation());
         if (!iplComp->IsOnTop()) pinLocation.x *= -1;
         CTMatrix transform;
         transform.rotateRadians(iplComp->GetRotation());
         transform.translate(iplComp->GetXLocation(), iplComp->GetYLocation());
         transform.transform(pinLocation);

         // get all the channel numbers for the net this pin is on
         CIplSortedArray sortedArray;
         CIplNet *iplNet = m_netMap.Lookup(iplPin->GetNetName());
         if (iplNet != NULL)
         {
            CIplTestProbe *iplTP = iplNet->GetFirstTestProbe();
            while (iplTP != NULL)
            {
               if (iplTP->IsPowerInjection())
                  sortedArray.Add(10000);
               else
               {
                  if(!iplTP->IsProbedDiePin())
                     sortedArray.Add(iplTP->GetChannelNumber());
                  else
                     fprintf(m_iplLogFp, "%s - Net: %s Probe $%s - Skipped Probe for Die pin.\n", outFile.GetFileName(),
                        iplNet->GetName(), iplTP->GetName());
               }
               iplNet->GetNextTestProbe(iplTP);
            }
         }

         // get the layer paramter of the pin
         // 0 = through hole pin location, 1 = SMD top, 2 = SMD bottom
         int layer = iplComp->IsOnTop()?1:2;
         if (!iplPin->IsSMD()) layer = 0;

         outFile.WriteString("%-14d%-14s%-14.*f%-14.*f%-14d%-14s%s\n", 
            iplPin->GetPinNumber(), iplPin->GetName(),
            m_decimalPlaces, pinLocation.x, m_decimalPlaces, pinLocation.y, 
            layer, cleanNetname(iplPin->GetNetName()), sortedArray.GetDelimitedString(",", false));

         iplPin = iplComp->GetNextPin();
      }

      outFile.WriteString("\n");
   }
   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeTstparts
*/
int CIplWriter::writeTstparts(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Tstparts.asc", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
      outFile.WriteString("Part          Grid          T/B           'Device', 'Outline'\n");
   }

   // write all tested parts
   POSITION pos = m_compMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplComponent *iplComp = NULL;

      m_compMap.GetNextAssoc(pos, key, iplComp);

      if (iplComp == NULL)
         continue;

      if (iplComp->GetType() == deviceTypeNoTest)
         continue;

      outFile.WriteString("%-13s %-13s (%s)%10s '%s', '%s'\n", cleanRefdes(iplComp->GetName()), iplComp->GetGridLocation(),
         iplComp->IsOnTop()?"T":"B", "", iplComp->GetComponentDevice(), iplComp->GetBlockName());
   }

   outFile.Close();

   return 0;
}

/******************************************************************************
* CIplWriter::writeZ18_dis
*/
int CIplWriter::writeZ18_dis(CString filepath)
{
   CFormatStdioFile outFile;
   CFileException e;
   
   if (!outFile.Open(filepath + "Z18_dis.ged", CFile::modeCreate|CFile::modeWrite, &e))
      return e.m_cause;
   
   // display the header information
   {
      CTime time = CTime::GetCurrentTime();
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString("; Filename : %s\n", outFile.GetFileName());
      outFile.WriteString("; Created by CCE to ODB++ v.%s\n", getApp().getVersionString());
      outFile.WriteString("; %s\n", time.Format("%A, %B %d, %Y"));
      outFile.WriteString("; ---------------------------------------------------\n");
      outFile.WriteString(";\n");
   }

   // write all untested parts
   POSITION pos = m_compMap.GetStartPosition();
   while (pos)
   {
      CString key;
      CIplComponent *iplComp = NULL;

      m_compMap.GetNextAssoc(pos, key, iplComp);

      if (iplComp == NULL)
         continue;

      if (iplComp->GetType() != deviceTypeNoTest)
         continue;

      outFile.WriteString("id == \"%s\" : Comp_Disable;\n", cleanRefdes(iplComp->GetName()));
   }

   outFile.Close();

   return 0;
}

void CIplWriter::assignCapOpensSensorNumbers()
{
	int sensorNumber = 1;

   POSITION pos = m_compMap.GetStartPosition();
   while (pos)
   {
      CString key = "";
      CIplComponent *iplComp = NULL;

      m_compMap.GetNextAssoc(pos, key, iplComp);
		if (iplComp != NULL && iplComp->IsCapOpensQualified())
      {
			iplComp->SetSensorNumber(sensorNumber++);
		}
	}
}

void CIplWriter::setOutputScale (double scale)
{
	outputScale = scale;
}

/******************************************************************************
* CIplWriter::WriteFiles
*/
int CIplWriter::WriteFiles(CString filepath)
{
   if (pFile != NULL)
   {
      // write the ipl file
      if (writeIPL(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "IPL.DAT");
      }

      // write the board outline file
      if (writeBoardOutline(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "BdOutline.asc");
      }

      // write the board file
      if (writeBoard(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Board.asc");
      }

      // write the Ind xRef file
      if (writeIndXRef(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "IndXRef.asc");
      }

      // write the nails file
      if (writeNails(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Nails.asc");
      }

      // write the name xRef file
      if (writeNameXRef(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "NameXRef.asc");
      }

      // write the net to nail file
      if (writeNet2Nail(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Net2Nail.asc");
      }

      // write the nets file
      if (writeNets(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Nets.asc");
      }

      // write the parts file
      if (writeParts(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Parts.asc");
      }

      // write the pins file
      if (writePins(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Pins.asc");
      }

      // write the test parts file
      if (writeTstparts(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Tstparts.asc");
      }

      // write the z18_dis file
      if (writeZ18_dis(filepath) != 0)
      {
         fprintf(m_iplLogFp, "%s could not be created!\n", "Z18_dix.ged");
      }
   }

   return 0;
}

/******************************************************************************
* CIplTestOrder::CiplTestOrder
*/
CIplTestOrder::CIplTestOrder()
{
   // Add device types to list in order of desired testing

   AddTail( deviceTypeJumper );
   AddTail( deviceTypeFuse );

   AddTail( deviceTypeResistor );
   AddTail( deviceTypeResistorArray );
   AddTail( deviceTypePotentiometer );
   AddTail( deviceTypeCapacitor );
   AddTail( deviceTypeCapacitorArray );
   AddTail( deviceTypeCapacitorPolarized );
   AddTail( deviceTypeCapacitorTantalum );
   AddTail( deviceTypeInductor );

   AddTail( deviceTypeDiode );
   AddTail( deviceTypeDiodeArray );
   AddTail( deviceTypeDiodeLed );
   AddTail( deviceTypeDiodeLedArray );
   AddTail( deviceTypeDiodeZener );

   AddTail( deviceTypeTransistor );
   AddTail( deviceTypeTransistorArray );
   AddTail( deviceTypeTransistorNpn );
   AddTail( deviceTypeTransistorPnp );
   AddTail( deviceTypeTransistorFetNpn );
   AddTail( deviceTypeTransistorFetPnp );
   AddTail( deviceTypeTransistorMosfetNpn );
   AddTail( deviceTypeTransistorMosfetPnp );
   AddTail( deviceTypeTransistorScr );
   AddTail( deviceTypeTransistorTriac );
   AddTail( deviceTypeOpto );

   AddTail( deviceTypeVoltageRegulator );
   AddTail( deviceTypeConnector );
   AddTail( deviceTypeBattery );
   AddTail( deviceTypeOscillator );
   AddTail( deviceTypeSwitch );
   AddTail( deviceTypeSpeaker );
   AddTail( deviceTypeRelay );
   AddTail( deviceTypeFilter );
   AddTail( deviceTypeCrystal );

   AddTail( deviceTypeIC );
   AddTail( deviceTypeICLinear );
   AddTail( deviceTypeICDigital );

   AddTail( deviceTypePowerSupply );
   AddTail( deviceTypeTransformer );

	AddTail( CAP_OPENS_TEST );
}

/******************************************************************************
* CIplSmartRefdes::CIplSmartRefdes
*/
CIplSmartRefdes::CIplSmartRefdes(const CString& wholeRefdes)
{
   m_prefix = "";
   m_number = 0;

   if (!wholeRefdes.IsEmpty()) {
      m_prefix = wholeRefdes;
      
      // Work from the end backwards, so you don't get fooled
      // by the likes of R3_3-R3_5.

      char c;
      int powerOfTenFactor = 1;
      int i = wholeRefdes.GetLength() - 1;
      while (i >= 0 && isdigit(c = wholeRefdes.GetAt(i)))
      {
         m_number = m_number + ((c - '0') * powerOfTenFactor);
         m_prefix.Truncate(i);
         i--;
         powerOfTenFactor *= 10;
      }
   }
}

/******************************************************************************
* CIplSmartRefdes::operator<
*/
bool CIplSmartRefdes::operator<(CIplSmartRefdes &smrefdes)
{
   // First compare prefixes, i.e. the "C" in "C10"
   CString thisPrefix = m_prefix;
   CString outerPrefix = smrefdes.getPrefix();

   int comparison = thisPrefix.MakeUpper().Compare(outerPrefix.MakeUpper());

   if (comparison < 0) return true;
   if (comparison > 0) return false;

   // Same prefixes, go by the number
   return m_number < smrefdes.getNumber();

}

/******************************************************************************
* CMapBlockToProbeVal
*/
CMapBlockToProbeVal::CMapBlockToProbeVal()
{ 
   probeIndex = 1;
   
}

CMapBlockToProbeVal::~CMapBlockToProbeVal()
{
}

int CMapBlockToProbeVal::LookupProbeNumber(BlockStruct *block)
{
   int probeNumber = 0;   

    //If blocknumber is not exist, create one
   if(!blocknNumToProbe.Lookup(block->getBlockNumber(),probeNumber))
   {
      if(!blockNameToProbe.Lookup(block->getName(),probeNumber))
      {
         probeNumber = probeIndex;
         blockNameToProbe.SetAt(block->getName(),probeIndex);
         blocknNumToProbe.SetAt(block->getBlockNumber(),probeIndex);         
         probeIndex++;
      }
      else
      {
         blocknNumToProbe.SetAt(block->getBlockNumber(),probeNumber);
      }
   }

   return probeNumber;
}

void CMapBlockToProbeVal::GetProbeTable(CStringArray &table)
{
   CString lines = "\n-- Probe Name to Probe Size Table --\n";
   lines.Append("Probe Name\tNail Size\t\n");
   table.Add(lines);

   POSITION datapos = blockNameToProbe.GetStartPosition();
   while(datapos)
   {
      CString blockName;
      int probeNumber;

      lines = "";
      blockNameToProbe.GetNextAssoc(datapos,blockName,probeNumber);
      lines.Format("%s\t\t%5d\n",blockName,probeNumber);
      table.Add(lines);
   }
}
