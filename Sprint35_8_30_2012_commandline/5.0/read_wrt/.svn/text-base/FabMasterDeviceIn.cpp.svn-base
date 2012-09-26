// $Header: /CAMCAD/4.5/read_wrt/FabMasterDeviceIn.cpp 11    9/29/06 8:42p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "StdAfx.h"
#include "DeviceType.h"
#include "FabMasterDeviceIn.h"
#include "CCEtoODB.h"



//_____________________________________________________________________________
void ReadFabMasterDevice(const char* fileName,CCEtoODBDoc* doc,FormatStruct* format)
{
   CFabMasterDeviceFile fabMasterDeviceFile(fileName);

   if (fabMasterDeviceFile.read())
   {
      CCamCadDatabase camCadDatabase(*doc);

      fabMasterDeviceFile.getParts().updateComponentAttributes(camCadDatabase);
   }
}

//_____________________________________________________________________________
DeviceTypeTag fabMasterClassStringToDeviceTypeTag(const CString& classValue)
{
   DeviceTypeTag deviceType = deviceTypeUndefined;

   if (classValue.CompareNoCase("resistor") == 0)
   {
      deviceType = deviceTypeResistor;
   }
   else if (classValue.CompareNoCase("crystal") == 0)
   {
      deviceType = deviceTypeCrystal;
   }
   else if (classValue.CompareNoCase("capacitor") == 0)
   {
      deviceType = deviceTypeCapacitor;
   }
   else if (classValue.CompareNoCase("integrated circuit") == 0)
   {
      deviceType = deviceTypeIC;
   }
   else if (classValue.CompareNoCase("led") == 0)
   {
      deviceType = deviceTypeDiodeLed;
   }
   else if (classValue.CompareNoCase("choke") == 0)
   {
      deviceType = deviceTypeInductor;
   }
   else if (classValue.CompareNoCase("switch") == 0)
   {
      deviceType = deviceTypeSwitch;
   }
   else if (classValue.CompareNoCase("connector") == 0)
   {
      deviceType = deviceTypeConnector;
   }
   else if (classValue.CompareNoCase("diode") == 0)
   {
      deviceType = deviceTypeDiode;
   }

   return deviceType;
}

//_____________________________________________________________________________
int CFabMasterPart::m_deviceTypeKeywordIndex            = -1;
int CFabMasterPart::m_partnumKeywordIndex               = -1;
int CFabMasterPart::m_valueKeywordIndex                 = -1;
int CFabMasterPart::m_pToleranceKeywordIndex            = -1;
int CFabMasterPart::m_nToleranceKeywordIndex            = -1;
int CFabMasterPart::m_subclassKeywordIndex              = -1;
int CFabMasterPart::m_componentHeightKeywordIndex       = -1;
int CFabMasterPart::m_capOpensKeywordIndex              = -1;
int CFabMasterPart::m_diodeOpensKeywordIndex            = -1;

CFabMasterPart::CFabMasterPart(const CString& partNumber) :
      m_partNumber(partNumber)  // Overloaded to also be refdes
{
}

void CFabMasterPart::resetKeywordIndices()
{
   m_deviceTypeKeywordIndex            = -1;
   m_partnumKeywordIndex               = -1;
   m_valueKeywordIndex                 = -1;
   m_pToleranceKeywordIndex            = -1;
   m_nToleranceKeywordIndex            = -1;
   m_subclassKeywordIndex              = -1;
   m_componentHeightKeywordIndex       = -1;
   m_capOpensKeywordIndex              = -1;
   m_diodeOpensKeywordIndex            = -1;
}

int CFabMasterPart::getDeviceTypeKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_deviceTypeKeywordIndex < 0)
   {
      m_deviceTypeKeywordIndex = camCadDatabase.getKeywordIndex(ATT_DEVICETYPE);
   }

   return m_deviceTypeKeywordIndex;
}

int CFabMasterPart::getPartnumKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_partnumKeywordIndex < 0)
   {
      m_partnumKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PARTNUMBER);
   }

   return m_partnumKeywordIndex;
}

int CFabMasterPart::getValueKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_valueKeywordIndex < 0)
   {
      m_valueKeywordIndex = camCadDatabase.getKeywordIndex(ATT_VALUE);
   }

   return m_valueKeywordIndex;
}

int CFabMasterPart::getPToleranceKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_pToleranceKeywordIndex < 0)
   {
      m_pToleranceKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PLUSTOLERANCE);
   }

   return m_pToleranceKeywordIndex;
}

int CFabMasterPart::getNToleranceKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_nToleranceKeywordIndex < 0)
   {
      m_nToleranceKeywordIndex = camCadDatabase.getKeywordIndex(ATT_MINUSTOLERANCE);
   }

   return m_nToleranceKeywordIndex;
}

int CFabMasterPart::getComponentHeightKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_componentHeightKeywordIndex < 0)
   {
      m_componentHeightKeywordIndex = camCadDatabase.getKeywordIndex(ATT_COMPHEIGHT);
   }

   return m_componentHeightKeywordIndex;
}

int CFabMasterPart::getSubclassKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_subclassKeywordIndex < 0)
   {
      m_subclassKeywordIndex = camCadDatabase.getKeywordIndex(ATT_SUBCLASS);

      if (m_subclassKeywordIndex < 0)
      {
         m_subclassKeywordIndex = camCadDatabase.registerKeyword(ATT_SUBCLASS,valueTypeString);
      }
   }

   return m_subclassKeywordIndex;
}


int CFabMasterPart::getCapOpensKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_capOpensKeywordIndex < 0)
   {
      m_capOpensKeywordIndex = camCadDatabase.getKeywordIndex(ATT_CAP_OPENS);

      if (m_capOpensKeywordIndex < 0)
      {
         m_capOpensKeywordIndex = camCadDatabase.registerKeyword(ATT_CAP_OPENS,valueTypeString);
      }
   }

   return m_capOpensKeywordIndex;
}

int CFabMasterPart::getDiodeOpensKeywordIndex(CCamCadDatabase& camCadDatabase)
{
   if (m_diodeOpensKeywordIndex < 0)
   {
      m_diodeOpensKeywordIndex = camCadDatabase.getKeywordIndex(ATT_DIODE_OPENS);

      if (m_diodeOpensKeywordIndex < 0)
      {
         m_diodeOpensKeywordIndex = camCadDatabase.registerKeyword(ATT_DIODE_OPENS,valueTypeString);
      }
   }

   return m_diodeOpensKeywordIndex;
}

void CFabMasterPart::addNamedValue(const CString& name,const CString& value)
{
	m_namedValueMap.SetAt(name, value);
}

void CFabMasterPart::updateComponentAttributes(CCamCadDatabase& camCadDatabase,DataStruct& pcbComponent, CFabMasterDeviceFileSettings& settings)
{
   CString name,value;
   CAttributes** pcbAttributes = &(pcbComponent.getAttributesRef());

   for (POSITION pos = m_namedValueMap.GetStartPosition();pos != NULL;)
   {
      m_namedValueMap.GetNextAssoc(pos,name,value);

      if (name.CompareNoCase("class") == 0)
      {
         DeviceTypeTag deviceType = fabMasterClassStringToDeviceTypeTag(value);

         if (deviceType != deviceTypeUndefined)
         {
            if (deviceType == deviceTypeResistor)
            {
               BlockStruct* componentGeometry = camCadDatabase.getBlock(pcbComponent.getInsert()->getBlockNumber());

               if (componentGeometry->getPinCount() > 2)
               {
                  deviceType = deviceTypeResistorArray;
               }
            }

            camCadDatabase.addAttribute(pcbAttributes,getDeviceTypeKeywordIndex(camCadDatabase),
               deviceTypeTagToValueString(deviceType));
         }
      }
      else if (name.CompareNoCase("fil") == 0)
      {
         if (settings.getUseFilPartnumber())
         {
            camCadDatabase.addAttribute(pcbAttributes,getPartnumKeywordIndex(camCadDatabase),value);
         }
      }
      else if (name.CompareNoCase("val") == 0)
      {
         camCadDatabase.addAttribute(pcbAttributes,getValueKeywordIndex(camCadDatabase),value);
      }
      else if (name.CompareNoCase("tol") == 0)
      {
         camCadDatabase.addAttribute(pcbAttributes,getPToleranceKeywordIndex(camCadDatabase),value);
      }
      else if (name.CompareNoCase("ntol") == 0)
      {
         camCadDatabase.addAttribute(pcbAttributes,getNToleranceKeywordIndex(camCadDatabase),value);
      }
      else if (name.CompareNoCase("height") == 0)
      {
         camCadDatabase.addAttribute(pcbAttributes,getComponentHeightKeywordIndex(camCadDatabase),value);
      }
      else if (name.CompareNoCase("model") == 0 || // devices.asc style keyword
         name.CompareNoCase("nam") == 0)           // devpart.asc style keyword
      {
         camCadDatabase.addAttribute(pcbAttributes,getSubclassKeywordIndex(camCadDatabase),value);
      }
      else if (name.CompareNoCase("typ") == 0)
      {
         // There may be more than one value, in comma separated list
         CSupString valueList(value);
         CStringArray params;
         valueList.ParseQuote(params, ",");

         // Potentially, TYP=<blank>   Clear both Diode and Cap Opens, then process params
         camCadDatabase.addAttribute(pcbAttributes, getCapOpensKeywordIndex(camCadDatabase),     "False");
         camCadDatabase.addAttribute(pcbAttributes, getDiodeOpensKeywordIndex(camCadDatabase),   "False");

         for (int i = 0; i < params.GetCount(); i++)
         {
            if (params.GetAt(i).CompareNoCase("cscan") == 0)
               camCadDatabase.addAttribute(pcbAttributes, getCapOpensKeywordIndex(camCadDatabase),   "True");
            else if (params.GetAt(i).CompareNoCase("dscan") == 0)
               camCadDatabase.addAttribute(pcbAttributes, getDiodeOpensKeywordIndex(camCadDatabase), "True");
            else if (params.GetAt(i).CompareNoCase("fscan") == 0)
               camCadDatabase.addAttribute(pcbAttributes, getCapOpensKeywordIndex(camCadDatabase),   "True");
         }
      }
   }
}

//_____________________________________________________________________________
CFabMasterParts::CFabMasterParts()
{
	m_refs.InitHashTable(nextPrime2n(500));
	m_refmap.InitHashTable(nextPrime2n(500));
	m_parts.InitHashTable(nextPrime2n(500));

	m_settings.read();
}

CFabMasterPart* CFabMasterParts::getPart(const CString& partName)
{
   CFabMasterPart* part = NULL;

   if (!m_parts.Lookup(partName,part))
   {
      part = NULL;
   }

   return part;
}

CFabMasterPart& CFabMasterParts::getDefinedPart(const CString& partName)
{
   CFabMasterPart* part = getPart(partName);

   if (part == NULL)
   {
      part = new CFabMasterPart(partName);
      m_parts.SetAt(partName,part);
   }

   return *part;
}

CFabMasterPart* CFabMasterParts::getRef(const CString& partName)
{
   CFabMasterPart* part = NULL;

   if (!m_refs.Lookup(partName,part))
   {
      part = NULL;
   }

   return part;
}

CFabMasterPart& CFabMasterParts::getDefinedRef(const CString& partName)
{
   CFabMasterPart* part = getRef(partName);

   if (part == NULL)
   {
      part = new CFabMasterPart(partName);
      m_refs.SetAt(partName,part);
   }

   return *part;
}

void CFabMasterParts::updateComponentAttributes(CCamCadDatabase& camCadDatabase)
{
   FileStruct* pcbFile = camCadDatabase.getSingleVisiblePcb();

   if (pcbFile != NULL)
   {
      CFabMasterPart::resetKeywordIndices();
      int partNumberKeywordIndex = camCadDatabase.getKeywordIndex(ATT_PARTNUMBER);

      BlockStruct* pcbBlock = pcbFile->getBlock();

      for (CDataListIterator dataListIterator(*pcbBlock,dataTypeInsert);dataListIterator.hasNext();)
      {
         DataStruct* data = dataListIterator.getNext();

         if (data->getInsert()->getInsertType() == insertTypePcbComponent)
         {
				CString refDes = data->getInsert()->getRefname();
				CString partNum;
				bool bb = camCadDatabase.getAttributeStringValue(partNum,&(data->getAttributesRef()),partNumberKeywordIndex);
				
				CFabMasterPart* fabMasterPart = NULL;

				// Look for part by refdes
				if (m_settings.getCorrelation() == correlateAuto ||
					 m_settings.getCorrelation() == correlateRefDes)
				{
					if (!refDes.IsEmpty())
					{
						CString *masterRefDes;
						if (m_refmap.Lookup(refDes, masterRefDes))
						{
							fabMasterPart = getRef(*masterRefDes);
						}
					}
				}

				// If not found by refdes, look for part number
				if (fabMasterPart == NULL &&
					 (m_settings.getCorrelation() == correlateAuto ||
					  m_settings.getCorrelation() == correlatePartNumber))
				{
					if (!partNum.IsEmpty() /*camCadDatabase.getAttributeStringValue(partName,&(data->getAttributesRef()),selectedKeywordIndex)*/)
					{
						fabMasterPart = getPart(partNum);
					}
				}

				// Do the update
				if (fabMasterPart != NULL)
				{
               fabMasterPart->updateComponentAttributes(camCadDatabase,*data, getSettings());
				}
			}
      }
   }
}

void CFabMasterParts::makeRefDesMap()
{
	// Make entries that map individual refdes's to the original refdes
	// range list.

	POSITION pos = m_refs.GetStartPosition();
	while (pos != NULL)
	{
		CString refdes = "";
		CFabMasterPart *part;
		m_refs.GetNextAssoc(pos, refdes, part);

		// Check for "range list", i.e. like C10,C12,C15-C20,C25-C30,C90
		// First break into comma separated sections.
		// Then for each section, check if it is a range.
		CSupString suprefdes(refdes);
		CStringArray refsary;
		int numrefs;

		numrefs = suprefdes.ParseQuote(refsary, ",");
		if (numrefs > 0) 
		{
			for (int i = 0; i < numrefs; i++) 
			{
				CString refs2 = refsary[i];

				CSupString suprefs2(refs2);
				CStringArray refs2ary;

				int numrefs2 = suprefs2.ParseQuote(refs2ary, "-");

				if (numrefs2 == 1)
				{
					// Single refdes, e.g. C10
					CString r1 = refs2ary[0];
					CString *r1masterRefdes = new CString(refdes);
					m_refmap.SetAt(r1, r1masterRefdes);
				}
				else if (numrefs2 == 2) 
				{
					// Ordinary range, e.g. C10-C22
					CSmartRefdes smref1(refs2ary[0]);
					CSmartRefdes smref2(refs2ary[1]);

					// We could, but we don't, check that the prefixes at each end
					// of the range are the same. Not doing so allows something
					// like C10-20 to work the same as C10-C20.

					if (smref1.getNumber() > 0 && smref2.getNumber() > 0)
					{
						int minN = min(smref1.getNumber(), smref2.getNumber());
						int maxN = max(smref1.getNumber(), smref2.getNumber());

						for (int j = minN; j <= maxN; j++)
						{
							CString refInRange;
							refInRange.Format("%s%d", smref1.getPrefix(), j);
							CString *masterRefdes = new CString(refdes);
							m_refmap.SetAt(refInRange, masterRefdes);
						}
					}
					else
					{
						// Apparantly not a range, or a poorly formed range.
						// Just add the two refs explicitly named.
						CString r1 = refs2ary[0];
						CString *r1masterRefdes = new CString(refdes);
						m_refmap.SetAt(r1, r1masterRefdes);

						CString r2 = refs2ary[0];
						CString *r2masterRefdes = new CString(refdes);
						m_refmap.SetAt(r2, r2masterRefdes);
					}
				}
				else
				{
					// Error, either nothing there or something like C10-C20-C30
					// Not expected, not handling this for now.
				}

			}
		}
	}
}

//_____________________________________________________________________________
CFabMasterDeviceFile::CFabMasterDeviceFile(const CString& filePath)
: m_filePath(filePath)
{
}

bool CFabMasterDeviceFile::read(CWriteFormat* errorWriteFormat)
{
	// The parser makes two lists simultaneously.
	// One is keyed by reference designator, the other is
	// keyed by part number.

	// There may be more than one PART (aka refdes) line in a record. We could
	// coalesce them, but that would make very large refdes keys. So instead
	// we make a separate entry for each occurence, not bothering to relate
	// them. They just end up as if they were separate records with the
	// same attrib values. Sample data has had up to a half dozen such lines.
	// We're ready to handle
#define REF_POOL_SIZE 128
	// such lines in a given record.

   CStdioFile stdioFile;

   bool retval = (stdioFile.Open(m_filePath,CFile::modeRead) != 0);

   if (retval)
   {
      CSupString line;
      CFabMasterPart* ref[REF_POOL_SIZE];
		CFabMasterPart* part = NULL;
		int nextRefIndex = 0;

		for (int i = 0; i < REF_POOL_SIZE; i++)
		{
			ref[i] = NULL;
		}

      // As part of case 2188, this code was rearranged to store name/value pairs
      // with blank or no value, their meaning can be significant, even when
      // value is blank. E.g. these are all valid and meaningful:
      // TYP=DSCAN
      // TYP=CSCAN
      // TYP=
      // TYP=FSCAN,DSCAN

      while (stdioFile.ReadString(line))
      {
         bool nameValuePair = line.Find("=") > -1;

         if (!nameValuePair)
         {
            line.Trim();
				if (line.CompareNoCase(":EOD") == 0)
            {	// End of Data (record)
					nextRefIndex = 0;
					for (int i = 0; i < REF_POOL_SIZE; i++)
					{
						ref[i] = NULL;
					}
					part = NULL;
            }
			}
			else
         {
            CString name;
            CString value;
            CStringArray params;
            line.ParseQuote(params,"=");
            if (params.GetCount() > 0)
               name = params.GetAt(0);
            if (params.GetCount() > 1)
               value = params.GetAt(1);

            if (name.CompareNoCase("MODE") == 0)
            {
               // Spectrum Devpart detection, sort of a hack, but it is what Mark suggested.
               // Fabmaster Devices.asc apparantly does not (or rarely) had MODE=MERGE,
               // but so far all examples of Spectrum Devpart.asc do.
               // I suspect we'll be revisiting this sooner than later.
               // Examples of Devpart.asc have bad FIL values, i.e. they are dup of
               // refname, rather than a part number, so do not use their value
               // to set partnumber attrib. That's what this is all about.
               if (value.CompareNoCase("MERGE") == 0)
                  getParts().getSettings().setUseFilPartnumber(false);
            }
            else if (name.CompareNoCase("PART") == 0)
            {	// RefDes
               ref[nextRefIndex] = &(m_partlist.getDefinedRef(value));
					nextRefIndex++;
            }
				else if (name.CompareNoCase("FIL") == 0)
            {	// Part Number
               part = &(m_partlist.getDefinedPart(value));
					for (int i = 0; i < REF_POOL_SIZE && ref[i] != NULL; i++)
					{
						ref[i]->addNamedValue(name,value);
					}
            }
            else if (name.CompareNoCase(":EOD") == 0)
            {	// End of Data (record)
					nextRefIndex = 0;
					for (int i = 0; i < REF_POOL_SIZE; i++)
					{
						ref[i] = NULL;
					}
					part = NULL;
            }
            else 
            {	// Save parameter
               for (int i = 0; i < REF_POOL_SIZE && ref[i] != NULL; i++)
					{
						ref[i]->addNamedValue(name,value);
					}
               if (part != NULL) part->addNamedValue(name,value);
            }
         }
      }

		m_partlist.makeRefDesMap();
   }
   else
   {
      if (errorWriteFormat != NULL)
		{
			errorWriteFormat->writef(
				"Could not open the file '%s' for reading.\n",
				m_filePath);
		}
   }

   return retval;
}

//_____________________________________________________________________________
CFabMasterDeviceFileSettings::CFabMasterDeviceFileSettings()
{
	m_correlation = correlateAuto;
   m_useFilPartnumber = true; // Fab Devices.asc mode  (false is Spectrum Devpart.asc mode)
}

bool CFabMasterDeviceFileSettings::read(CWriteFormat* errorWriteFormat)
{
   CString settingsFile( getApp().getImportSettingsFilePath("fabdevice.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nFABMaster Device Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);

   CStdioFile stdioFile;
   bool retval = (stdioFile.Open(settingsFile,CFile::modeRead) != 0);

   if (retval)
   {
      CSupString line;
      CStringArray params;
      int numParams;

      while (stdioFile.ReadString(line))
      {
         numParams = line.ParseQuote(params," ");

         if (numParams >= 2 && (params[0].Left(1) == '.'))
         {
            if (params[0].CompareNoCase(".DEVICE_MATCH") == 0)
            {
               CString matchby = params[1];
					if (matchby.CompareNoCase("AUTO") == 0)
					{
						m_correlation = correlateAuto;
					}
					else if (matchby.CompareNoCase("PARTNUMBER") == 0)
					{
						m_correlation = correlatePartNumber;
					}
					else if (matchby.CompareNoCase("REFDES") == 0)
					{
						m_correlation = correlateRefDes;
					}
            }
         }
      }
   }
   else
   {
      if (errorWriteFormat != NULL)
		{
			errorWriteFormat->writef(
				"Could not open the file '%s' for reading.\n",
				settingsFile);
		}
   }

   return retval;
}

//_____________________________________________________________________________
CSmartRefdes::CSmartRefdes(const CString& wholeRefdes)
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
