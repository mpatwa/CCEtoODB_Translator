
#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "dft.h"
#include "Net_Util.h"
#include "AeroflexNailRead.h"


// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


void ReadAeroflexNail(const char *infilename, CCEtoODBDoc *doc, FormatStruct *Format)
{
   FileStruct *file = doc->getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
   {
      ErrorMessage("Only one PCB file can be worked on at a time.\n\nThere is either zero or more than one PCB file visible.", "Aeroflex Nail Reader", MB_OK);
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
      ErrorMessage("There is no current DFT Solution.  Run Access Analysis before importing Nail file.", "Aeroflex Nail Reader", MB_OK);
      return;
   }

   
   CFilePath logFilePath(infilename);
   logFilePath.setBaseFileName("AeroflexNail");
   logFilePath.setExtension("log");

   CAeroflexNailReader reader(infilename, logFilePath.getPath(), doc, file);
   reader.Process();

}

//---------------------------------------------------------------

CAeroflexNailReader::CAeroflexNailReader(CString infilename, CString logfilename, CCEtoODBDoc *doc, FileStruct *pcbfile)
: m_inputFilename(infilename)
, m_doc(doc)
, m_pcbfile(pcbfile)
, m_fixtureFileUnits(pageUnitsUndefined)
, m_scaleFactor(1.0)
, m_offsetX(0.0)
, m_offsetY(0.0)
, m_decimals(7)
{
	if (!m_infile.Open(infilename, CFile::modeRead))
	{
      m_infileIsOpen = false;
      CString tmp;
      tmp.Format("Can not open Aeroflex Nail file [%s]", infilename);
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

   m_decimals = GetDecimals(m_doc->getPageUnits());
}

CAeroflexNailReader::~CAeroflexNailReader()
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

int CAccessMarkerMap::AscendingNetNameSortFunc(const void *a, const void *b)
{
   CAeroflexAccessMarker* itemA = (CAeroflexAccessMarker*)(((SElement*) a )->pObject->m_object);
   CAeroflexAccessMarker* itemB = (CAeroflexAccessMarker*)(((SElement*) b )->pObject->m_object);

   CString aref = itemA->GetNetName();
   CString bref = itemB->GetNetName();

   return (aref.CompareNoCase(bref));
}

//--------------------------------------------------------------------

void CAeroflexNailReader::WriteFinalReport()
{
   if (m_logIsOpen)
   {
      m_accessMarkerMap.setSortFunction(CAccessMarkerMap::AscendingNetNameSortFunc);
      m_accessMarkerMap.Sort();

      m_logfile.writef("\n\nFinal Probe Results\n");
      m_logfile.writef("%10s\t%16s\t%10s\t%10s\t%10s\n", "Target", "Location", "Net Name", "Old Probe", "New Probe");

      CString *keyStr;
      CAeroflexAccessMarker *aeroAccessMark;
      for(m_accessMarkerMap.GetFirstSorted(keyStr, aeroAccessMark); aeroAccessMark != NULL; m_accessMarkerMap.GetNextSorted(keyStr, aeroAccessMark))
      {
         CStringArray &oldProbeNames = aeroAccessMark->GetOldProbeNames();
         int oldProbeCount = oldProbeNames.GetCount();

         int newProbeCount = aeroAccessMark->GetNewProbeCount();

         int reportCount = max(oldProbeCount, newProbeCount);

         // One of these will be set, the other will be NULL, depends on what the target actually is
         DataStruct *ccAccessMark = aeroAccessMark->GetCamcadAccessMarker();
         CompPinStruct *compPin = aeroAccessMark->GetCompPin();
         DataStruct *targetData = aeroAccessMark->GetTargetData();
         CString targetID("Unknown");
         if (compPin != NULL)          // Use comp pin ref for ID
            targetID = compPin->getPinRef();
         else if (targetData != NULL)  // Refname is unreliable, use insert type (note fixture file is generic for these, no specific ID)
            targetID = insertTypeToDisplayString( targetData->getInsert()->getInsertType() );

         if (reportCount > 0)
         {
            // This is to convert location to string so print format is more likely to justify/align
            CString locationStr;
            locationStr.Format("(%1.*f,%1.*f)", m_decimals, ccAccessMark->getInsert()->getOriginX(), m_decimals, ccAccessMark->getInsert()->getOriginY());

            for (int i = 0; i < reportCount; i++)
            {
               m_logfile.writef("%10s", targetID);
               m_logfile.writef("\t%16s", locationStr);
               m_logfile.writef("\t%10s", aeroAccessMark->GetNetName(m_doc));
               m_logfile.writef("\t%10s", (i < oldProbeCount) ? oldProbeNames.GetAt(i) : "None");
               m_logfile.writef("\t%10s", (i < newProbeCount) ? aeroAccessMark->GetNewProbeData(i)->getInsert()->getRefname() : "None");
               m_logfile.writef("\n");
            }
         }
      }

   }
}

bool CAeroflexNailReader::Process()
{
   if (m_infileIsOpen && m_doc != NULL && m_pcbfile != NULL)
   {
      m_accessMarkerMap.CollectAccessMarkers(m_doc, m_pcbfile);

      m_infile.SeekToBegin();
      Parse();

      // Fuzz factor for location comparison.
      // Data sample provided for implementation used metric units with two decimal places.
      // So accuracy is to nearest 1/100 mm.
      // Because there is some leeway in probe placement, i.e. can be placed anywhere in area of pad,
      // we'll allow 0.0254 mm mismatch as being "the same". Note that it is same one 1 mil.
      double fuzzPageunits = m_doc->convertToPageUnits(pageUnitsMilliMeters, 0.0254);

      if (m_logIsOpen)
      {
         m_logfile.writef("\n");
         m_logfile.writef("CAMCAD Drawing Units: %s\n", PageUnitsTagToString(m_doc->getPageUnits()));
         m_logfile.writef("Fixture File Units:   %s\n", PageUnitsTagToString(m_fixtureFileUnits));
         m_logfile.writef("Scale Factor: %f  (applied to Fixture values to get CAMCAD values)\n", m_scaleFactor);
         m_logfile.writef("Offset XY: %1.*f, %1.*f (in Fixture File units, applied to Fixture location to align with CAMCAD data)\n", m_decimals, -(m_offsetX), m_decimals, -(m_offsetY));
         m_logfile.writef("\n");
      }

      // Get rid of current test probes, if any
      this->DeleteTestProbes();

      // And now add them back ...

      m_doc->PrepareAddEntity(m_pcbfile); // For potential addition of probe geometry and various inserts


      for (int probeI = 0; probeI < this->m_placedProbeList.GetCount(); probeI++)
      {
         CAeroflexNailRecord *nail = this->m_placedProbeList.GetAt(probeI);

         // We search for an access marker at the location, and then operate on what the access and its target it. 
         // Note that access is not necessarily centered on the via pad, so it
         // is the access location that is more important than the target's actual location.

         double nailPageunitsX = ConvertX(nail->GetXLoc());
         double nailPageunitsY = ConvertY(nail->GetYLoc());
         CAeroflexAccessMarker *aeroAccessMark = m_accessMarkerMap.FindAccessDataAtLocation(nailPageunitsX, nailPageunitsY, fuzzPageunits);
         if (aeroAccessMark != NULL)
         {
            // We place probe anyway, even if target has mismatch
            ValidateTargetName(aeroAccessMark, nail);

            // We place probe anyway, even if net has mismatch
            ValidateNetName(aeroAccessMark, nail);

            // Surface mismatch is deal breaker.
            if (ValidateSurface(aeroAccessMark, nail))
            {
               ETestSurface surfaceFixture = nail->GetSurface();
               CString netName( nail->GetNetName() );
               DataStruct *accessData = aeroAccessMark->GetCamcadAccessMarker();
               double accessOriginX = accessData->getInsert()->getOriginX();
               double accessOriginY = accessData->getInsert()->getOriginY();
               int probeNominalSizeMils = nail->GetProbeSizeMils();

#ifdef BOTH_N_AND_P_ARE_POWERINJ
               // Power Injection probes start with N (for ground) or P (for power).
               // For CAMCAD both are TEST_RESOURCE="Power Injection", all others are "Test".
               ETestResourceType testResType = (nail->GetRefname().Left(1).FindOneOf("NPnp") > -1) ? testResourceTypePowerInjection : testResourceTypeTest;
#else
               // It seems our original information, or atleast our interpretation of that information must have been off.
               // In the sample provided the N probes did not appear to be what we call Power Injection.
               // So we've backed off here, and only P probes get PowerInj.
               ETestResourceType testResType = (nail->GetRefname().Left(1).CompareNoCase("P") == 0) ? testResourceTypePowerInjection : testResourceTypeTest;
#endif

               DataStruct *probeData = PlaceTestProbe(probeNominalSizeMils, this->m_doc,  this->m_pcbfile->getBlock(),
                  nail->GetRefname(), accessOriginX, accessOriginY, 0.0, surfaceFixture, testResType,
                  netName, accessData);
         
               if (probeData != NULL)
               {
                  // Additional attributes
                  if (!nail->GetTesterInterfaceName().IsEmpty())
                  {
	                  WORD kw = m_doc->RegisterKeyWord(ATT_TESTERINTERFACE, 0, valueTypeString);
                     probeData->setAttrib(m_doc->getCamCadData(), kw, valueTypeString, nail->GetTesterInterfaceName().GetBuffer(0), attributeUpdateOverwrite, NULL);
                  }

                  if (!nail->GetWireSize().IsEmpty() && nail->GetWireSize().Compare(AERO_FIELD_SKIPPED) != 0)
                  {
	                  WORD kw = m_doc->RegisterKeyWord(ATT_WIRE_SIZE, 0, valueTypeString);
                     probeData->setAttrib(m_doc->getCamCadData(), kw, valueTypeString, nail->GetWireSize().GetBuffer(0), attributeUpdateOverwrite, NULL);

                  }

                  if (!nail->GetWireColor().IsEmpty() && nail->GetWireColor().Compare(AERO_FIELD_SKIPPED) != 0)
                  {
	                  WORD kw = m_doc->RegisterKeyWord(ATT_WIRE_COLOR, 0, valueTypeString);
                     probeData->setAttrib(m_doc->getCamCadData(), kw, valueTypeString, nail->GetWireColor().GetBuffer(0), attributeUpdateOverwrite, NULL);

                  }

                  // Add to access mark's list of probes
                  aeroAccessMark->AddNewProbeData(probeData);
               }
            }

         }
         else if (m_logIsOpen)
         {
            m_logfile.writef("No Access Marker for Probe: %9s, Location: (%1.*f,%1.*f) Target: %s, Net: %s\n",
               nail->GetRefname(), m_decimals, nailPageunitsX, m_decimals, nailPageunitsY, nail->GetTargetRefname(), nail->GetNetName());
            // Note - Could create Unplaced probe here, but such has not been asked for. Was offered, but no
            // response given, so we just skip it.
         }
      }

      WriteFinalReport();
   }

   return false;
}

bool CAeroflexNailReader::ValidateTargetName(CAeroflexAccessMarker *aeroAccessMark, CAeroflexNailRecord *nail)
{
   // Return true if actual target at access marker seems to match what nail calls for.
   // Otherwise return false.
   // Write discrepancies to log file.

   if (aeroAccessMark != NULL && nail != NULL)
   {
      // CompPins come in all variety of name, the two tested here are sorts of generic names.
      // Because of dot in name, they have same syntax as CompPin, but are not CompPin.
      // So if target is one of these, then we'll accept any target as okay.
      // If not one of these then we assume it is a CompPin and we report if actual target at
      // location is something else.
      if ((nail->GetTargetRefname().CompareNoCase("TEST.PAD") == 0) ||
         (nail->GetTargetRefname().CompareNoCase("VIA.HOLE")) == 0)
      {
         // Accept any target
         return true;
      }
      else
      {
         // Validate CompPin at target
         CString compRefName( nail->GetTargetCompRefname() );
         CString pinRefName( nail->GetTargetCompPinname() );

         CompPinStruct *compPin = aeroAccessMark->GetCompPin();

         if (compPin == NULL)
         {
            if (m_logIsOpen)
            {
               double nailPageunitsX = ConvertX(nail->GetXLoc());
               double nailPageunitsY = ConvertY(nail->GetYLoc());
               m_logfile.writef("Target for Probe is not a CompPin, Probe: %9s, Location (%1.*f,%1.*f)\n",
                  nail->GetRefname(), m_decimals, nailPageunitsX, m_decimals, nailPageunitsY);
            }
         }
         else if (compPin->getPinRef('.').Compare(nail->GetTargetRefname()) != 0)
         {
            if (m_logIsOpen)
            {
               m_logfile.writef("Actual CompPin target differs, Probe: %9s, CAMCAD CompPin: %s, Fixture CompPin: %s\n",
                  nail->GetRefname(), compPin->getPinRef('.'), nail->GetTargetRefname());
            }
         }
         else
         {
            // Okay
            return true;
         }
      }
   }

   return false;
}

CString CAeroflexAccessMarker::GetNetName(CCEtoODBDoc *doc)
{
   // Sometimes netname is attrib on camcad access marker, sometimes not, depends on how it came to exist.
   // Try it first, if not there try the target.

   // Calls w/ doc ptr will set netname cache member var.
   // Calls w/o doc ptr return cached value.

   if (doc != NULL)
   {
      Attrib *attrib = NULL;
      int netnameKW = doc->getStandardAttributeKeywordIndex(standardAttributeNetName);

      DataStruct *accessData = this->GetCamcadAccessMarker();
      DataStruct *targetData = this->GetTargetData();
      CompPinStruct *compPin = this->GetCompPin();

      // First check if netname is on access marker, but these do not always get set (depends how access marker came to be)

      if (accessData != NULL &&
         accessData->getAttributesRef() != NULL  && accessData->getAttributesRef()->Lookup(netnameKW, attrib))
      {
         m_netName = attrib->getStringValue();
      }
      else if (compPin != NULL &&
         compPin->getAttributesRef() != NULL  && compPin->getAttributesRef()->Lookup(netnameKW, attrib))
      {
         m_netName = attrib->getStringValue();
      }
      else if (targetData != NULL &&
         targetData->getAttributesRef() != NULL  && targetData->getAttributesRef()->Lookup(netnameKW, attrib))
      {
         m_netName = attrib->getStringValue();
      }
   }

   return m_netName;
}

bool CAeroflexNailReader::ValidateNetName(CAeroflexAccessMarker *aeroAccessMark, CAeroflexNailRecord *nail)
{
   // Return true if actual net at access marker seems to match what nail calls for.
   // Otherwise return false.
   // Write discrepancies to log file.

   if (aeroAccessMark != NULL && nail != NULL)
   {
      CString accessNetname( aeroAccessMark->GetNetName(m_doc) );

      if (accessNetname.Compare(nail->GetNetName()) == 0) // Case sensitive on purpose
      {
         return true;
      }
      else
      {
         if (m_logIsOpen)
         {
            m_logfile.writef("NetName Mismatch, Probe: %9s, CAMCAD Net: %s,  Fixture Net: %s\n",
               nail->GetRefname(), accessNetname, nail->GetNetName());
         }
      }
   }

   return false;
}

bool CAeroflexNailReader::ValidateSurface(CAeroflexAccessMarker *aeroAccessMark, CAeroflexNailRecord *nail)
{
   // Return true if same surface.
   // Otherwise return false.
   // Write discrepancies to log file.

   if (aeroAccessMark != NULL && nail != NULL)
   {
      ETestSurface surfaceFixture = nail->GetSurface();
      ETestSurface surfaceCamcad = aeroAccessMark->GetAccessMarkSurface();

      if (surfaceCamcad == surfaceFixture)
      {
         return true;
      }
      else if (m_logIsOpen && surfaceCamcad != surfaceFixture)
      {
         m_logfile.writef("PCB Surface Mismatch, Probe: %9s, CAMCAD: %s, Fixture: %s.\n",
            nail->GetTargetRefname(), testSurfaceTagToString(surfaceCamcad), testSurfaceTagToString(surfaceFixture));
      }
   }

   return false;
}

void CAeroflexNailReader::DeleteTestProbes()
{
   // Mark changed his mind, we no longer want exceptions for Power Inj probes, so we don't need this
   // special probe deleter. I'll leave it here a little while in case "our" minds get changed back.
   // The DEADCODE denotes the one that was made to leave PowerInj probes behind.
   // Since we don't want to do that, we can just use teh "standard" probe deleter:

   ::DeleteTestProbes(m_doc, m_pcbfile, true, true);

#ifdef DEADCODE
   // Note that there is a function by this same name in ProbePlacement.cpp.
   // It would have been preferable to use that one where this one gets used, but there is
   // a requirement here to LEAVE the Power Injection probes intact.
   // Also preferable would have been to modify that other one and make it optional to
   // leave Power Inj intact. But it goes on to rename geometries that are probe-name-style based
   // and muck about, because it is expecting to have deleted ALL probes.
   // I did not want to dump more time into changing that func at this point in release
   // cycle, with all the additional testing that SHOULD go with such a change.
   // So... punt. Here is a copy that is specific to this reader's needs.

   // Remove any probe inserts that currently exist

   bool top = true;     // this is just to keep the form more like func in ProbePlacement.cpp
   bool bottom = true;

   int testresourceKW = m_doc->getStandardAttributeKeywordIndex(standardAttributeTestResource);

   POSITION dataPos = this->m_pcbfile->getBlock()->getHeadDataInsertPosition();
   while (dataPos)
   {
      DataStruct *data = this->m_pcbfile->getBlock()->getNextDataInsert(dataPos);

      if (data->getInsert()->getInsertType() == insertTypeTestProbe)
      {
         bool isPowerInj = false;
         Attrib *attrib;
         if (data->getAttributesRef() != NULL && data->getAttributes()->Lookup(testresourceKW, attrib))
         {
            CString val( attrib->getStringValue() );
            val.MakeUpper();
            isPowerInj = (val.Find("POWER") > -1);
         }

         if (!isPowerInj)
         {
            if ((top && data->getInsert()->getPlacedTop()) || (bottom && data->getInsert()->getPlacedBottom()))
            {
               m_doc->removeDataFromDataList(*(m_pcbfile->getBlock()),data);
            }
         }
      }
   }
#endif
}


bool CAeroflexNailReader::Parse()
{
   // Get next data record, skips comment lines.
   // Note data records may span multiple lines, when so, line continuation marker is present.

   bool inPlacedProbeSection = false;
   bool inPilotDrillSection = false;
   bool inExtentsSection = false;

   if (this->m_infileIsOpen)
   {

      CString buf;
      while (m_infile.ReadString(buf))
      {
         buf.Trim();
         if (buf.Left(1) == "/")
         {
            // Comment line, but all we have to denote different sections are comments, so
            // we have to look at the comment. Also, file units are listed as a comment.
            buf.MakeUpper();
            if (buf.Find("UNITS") > -1)
            {
               // Determine scale factor to current page units
               // / Units................: METRIC mm
               if (buf.Find("METRIC MM"))
               {
                  m_fixtureFileUnits = pageUnitsMilliMeters;
                  m_scaleFactor = Units_Factor(pageUnitsMilliMeters, this->m_doc->getPageUnits());
               }
               // else leave as-is, fixture file must match page units
            }
            else if (buf.Find("TEST PROBES AND POWER PINS") > -1)
            {
               inPlacedProbeSection = true;
               inPilotDrillSection = false;
               inExtentsSection = false;
            }
            else if (buf.Find("PILOT HOLES") > -1)
            {
               inPlacedProbeSection = false;
               inPilotDrillSection = true;
               inExtentsSection = false;
            }
            else if (buf.Find("EXTENTS AND LOCATIONS") > -1)
            {
               inPlacedProbeSection = false;
               inPilotDrillSection = false;
               inExtentsSection = true;
            }
         }
         else
         {
            // Data record
            if (inPlacedProbeSection)
            {
               CAeroflexNailRecord *nail = new CAeroflexNailRecord(buf);
               this->m_placedProbeList.Add(nail);
            }
            else if (inPilotDrillSection)
            {
               CAeroflexNailRecord *nail = new CAeroflexNailRecord(buf);  // Pilot hole records have same form
               this->m_pilotHoleList.Add(nail);
            }
            else if (inExtentsSection)
            {
               // Use extents for origin offset.
               // Locations look to be tooling holes, we're ignoring them.
               buf.MakeUpper();
               if (buf.Find("MIN EXTENT") > -1) // we'll try this for origin
               {
                  CSupString supbuf(buf);
                  CStringArray params;
                  supbuf.Parse(params, " ");
                  if (params.GetCount() > 2)
                  {
                     m_offsetX = atof( params.GetAt(0) );
                     m_offsetY = atof( params.GetAt(1) );
                  }
               }
               // else ignore the rest
            }
            // else ignore
         }
      }

   }

   return true;
}


//---------------------------------------------------------------
//---------------------------------------------------------------


void CAeroflexNailRecord::Reset()
{
   m_xLoc.Empty();
   m_yLoc.Empty();
   m_refname.Empty(); // Pin Number in Aeroflex terms
   m_pcbSide.Empty();
   m_probeType.Empty(); // Embodies probe size
   m_testerInterfaceName.Empty();
   m_wireSize.Empty();
   m_wireColor.Empty();
   m_targetRefname.Empty(); // Where access marker should be
   m_netname.Empty();

   m_targetCompRefname.Empty();
   m_targetCompPinname.Empty();
}


void CAeroflexNailRecord::Set(CString recordStr)
{
   this->Reset();

   CSupString supstr(recordStr);

   CStringArray params;
   supstr.ParseQuote(params, " ");

   if (params.GetCount() > 0)
      m_xLoc = params.GetAt(0);

   if (params.GetCount() > 1)
      m_yLoc = params.GetAt(1);

   if (params.GetCount() > 2)
      m_refname = params.GetAt(2);

   if (params.GetCount() > 3)
      m_pcbSide = params.GetAt(3);

   if (params.GetCount() > 4)
      m_probeType = params.GetAt(4);

   if (params.GetCount() > 5)
      m_testerInterfaceName = params.GetAt(5);

   if (params.GetCount() > 6)
      m_wireSize = params.GetAt(6);

   if (params.GetCount() > 7)
      m_wireColor = params.GetAt(7);

   if (params.GetCount() > 8)
      m_targetRefname = params.GetAt(8);

   if (params.GetCount() > 9)
      m_netname = params.GetAt(9);

   // Process PinRef to get Comp Refname and Pin Refname
   int dotIndx = m_targetRefname.Find(".");
   if (dotIndx > -1)
   {
      m_targetCompRefname = m_targetRefname.Left(dotIndx);
      int pinRefnameLen = m_targetRefname.GetLength() - dotIndx - 1;
      if (pinRefnameLen > 0)
         m_targetCompPinname = m_targetRefname.Right(pinRefnameLen);
   }
}


//---------------------------------------------------------------
//---------------------------------------------------------------

void CAccessMarkerMap::CollectAccessMarkers(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   this->RemoveAll();

   BlockStruct *pcbBlk = pcbFile != NULL ? pcbFile->getBlock() : NULL;

   if (pcbBlk != NULL && doc != NULL)
   {
      int datalinkKW = doc->getStandardAttributeKeywordIndex(standardAttributeDataLink);

      POSITION pos = pcbBlk->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *data = pcbBlk->getNextDataInsert(pos);

         if (data->isInsertType(insertTypeTestAccessPoint))
         {
            CString accessRefname( data->getInsert()->getRefname() );

            Attrib *attrib;
            if (data->getAttributesRef() != NULL && data->getAttributes()->Lookup(datalinkKW, attrib))
            {
               int entityNum = attrib->getIntValue();
               CEntity entity = CEntity::findEntity(doc->getCamCadData(), entityNum);

               CAeroflexAccessMarker *aeroAccessMark = NULL;
               CString keyStr;

               // Map key is access mark's own entity number
               keyStr.Format("%d", data->getEntityNumber());

               if (entity.getEntityType() == entityTypeCompPin)
               {
                  CompPinStruct *compPin = entity.getCompPin();
                  aeroAccessMark = new CAeroflexAccessMarker(data, compPin);
               }
               else if (entity.getEntityType() == entityTypeData)
               {
                  DataStruct* entityData = entity.getData();
		            InsertStruct *insert = entityData->getInsert();
                  aeroAccessMark = new CAeroflexAccessMarker(data, entityData);
               }
               else
               {
                  // ? Unexpected entity type
               }

               if (aeroAccessMark != NULL && !keyStr.IsEmpty())
               {
                  aeroAccessMark->GetNetName(doc); // Force caching of netname, needed for report at end, otherwise won't be set if no probe gets placed here
                  this->SetAt(keyStr, aeroAccessMark);
               }
               else
               {
                  int jjjjj = 0; //*rcf ERROR
               }

            }

         }
      }

      CollectProbes(doc, pcbFile);
   }
}

void CAccessMarkerMap::CollectProbes(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   BlockStruct *pcbBlk = pcbFile != NULL ? pcbFile->getBlock() : NULL;

   if (pcbBlk != NULL && doc != NULL)
   {
      int datalinkKW = doc->getStandardAttributeKeywordIndex(standardAttributeDataLink);

      POSITION pos = pcbBlk->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *data = pcbBlk->getNextDataInsert(pos);

         if (data->isInsertType(insertTypeTestProbe))
         {
            CString refname( data->getInsert()->getRefname() );

            Attrib *attrib;
            if (data->getAttributesRef() != NULL && data->getAttributes()->Lookup(datalinkKW, attrib))
            {
               int entityNum = attrib->getIntValue();
               CString entityNumStr;
               entityNumStr.Format("%d", entityNum);
               CAeroflexAccessMarker *aeroAccessMark = NULL;
               if (this->Lookup(entityNumStr, aeroAccessMark))
               {
                  aeroAccessMark->GetOldProbeNames().Add(refname);
               }
            }
         }
      }
   }
}

CAeroflexAccessMarker *CAccessMarkerMap::FindAccessDataAtLocation(double pageunitsX, double pageunitsY, double tolerance)
{
   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      CAeroflexAccessMarker *aeroAccessMarker = NULL;
      CString key;
      this->GetNextAssoc(pos, key, aeroAccessMarker);

      DataStruct *data = (aeroAccessMarker != NULL) ? aeroAccessMarker->GetCamcadAccessMarker() : NULL;

      if (data != NULL)//this is already guaranteed: && data->isInsertType(insertTypeTestAccessPoint))
      {
         double insertX = data->getInsert()->getOriginX();
         double insertY = data->getInsert()->getOriginY();
         if (fpnear(insertX, pageunitsX, tolerance) && fpnear(insertY, pageunitsY, tolerance))
         {
            return aeroAccessMarker;
         }
      }
   }

   return NULL;
}
