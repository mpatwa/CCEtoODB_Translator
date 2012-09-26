
#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "Sch_Lib.h"
#include "CadFormatRecognizer.h"
#include "PcbUtil.h"
#include "Net_Util.h"
#include "SchematicNetlistIn.h"


// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)

// This is so same reader object can be shared for multi-file-set import
static CSchematicNetlistFileReader *netlistReader = NULL;

void ReadSchematicNetlist(const char *infilename, CCEtoODBDoc *doc, FormatStruct *Format, int currentFile, int fileCount)
{
   //FileStruct *file = doc->getFileList().GetOnlyShown(blockTypePcb);

   //if (file == NULL)
   //{
   //   ErrorMessage("Only one PCB file can be worked on at a time.\n\nThere is either zero or more than one PCB file visible.", "Spectrum Nailreq Reader", MB_OK);
   //   return;
   //}

   // currentFile is zero-based, fileCount is 1-based.
   // E.g. if fileCount is 6 then currentFile will range from 0 to 5 in a
   // series of calls, one call for each file in set.

   // Determine specific file type

   CCadFormatRecognizer cadFormatRecognizer;

   FileTypeTag filetype = cadFormatRecognizer.determineFileType(infilename);

   FileTypeTag specificFiletype = fileTypeUnknown;

   CCadFormatAcceptor *acceptor = cadFormatRecognizer.getCadFormatAcceptor(fileTypeSchematicNetlist);

   if (acceptor != NULL)
   {
      if (acceptor->accepts(infilename))
      {
         specificFiletype = acceptor->getSpecificFileType();
      }
   }

   // Read the input

   if (currentFile == 0)
   {
      if (netlistReader != NULL)
         delete netlistReader;

       netlistReader = new CSchematicNetlistFileReader;
   }


   switch (specificFiletype)
   {
   case fileTypeCamcadNetlist:
      netlistReader->ReadCamcadNetlistFile(infilename);
      break;

   case fileTypeVbKeyinNetlist:
      netlistReader->ReadVBKeyinNetlistFile(infilename);
      break;

   case fileTypeBoardstationNetlist:
      netlistReader->ReadBoardstationNetlistFile(infilename);
      break;

   case fileTypeViewlogicNetlist:
      netlistReader->ReadViewlogicNetlistFile(infilename);
      break;

   case fileTypePadsPowerPcbNetlist:
      netlistReader->ReadPadsPowerPcbNetlist(infilename);
      break;

   default:
      ErrorMessage(infilename, "Can not import netlist, selected file is not a recognized supported netlist format.");
      break;
   }



   // Build the schematic file after reading the last file
   // Allocate netlist reader with new before first file is read.
   // Keep filling same netlist/reader.
   // When last file is read then make Schematic CCZ
   // Or maybeit is the schb that needs caching?... no, I think the reader.

   if (currentFile == (fileCount - 1)) // Last file
   {
      CNetList *sourceNetList = netlistReader->GetNetList();

      if (sourceNetList != NULL && sourceNetList->getCount() > 0)
      {
         CSchematicBuilder schb;

         CFilePath schFilePath( infilename );
         CString fileName( schFilePath.getBaseFileName() );
         fileName += "_Schematic";
         schFilePath.setBaseFileName(fileName);
         schFilePath.setExtension("ccz");
         CString schematicCczFileName( schFilePath.getPath() );

         FileStruct *generatedSchFile = schb.GenerateSchematicDrawing(doc, NULL, sourceNetList);

         //bool isloaded = doc->isFileLoaded();

         if (doc != NULL)
            doc->setFileLoaded(true);

         // Make that new one the only visible file.
         // New one is created visible, so just need to set rest to not visible.
         POSITION filepos = doc->getFileList().GetHeadPosition();
         while (filepos != NULL)
         {
            FileStruct *f = doc->getFileList().GetNext(filepos);
            if (f != generatedSchFile)
               f->setShow(false);
         }
      }

      delete netlistReader;
      netlistReader = NULL;
   }


}

//---------------------------------------------------------------

CSchematicNetlistFileReader::CSchematicNetlistFileReader()
{
   m_netList = new CNetList(m_tempCamCadData);
}

//---------------------------------------------------------------

CSchematicNetlistFileReader::~CSchematicNetlistFileReader()
{
   if (m_netList != NULL)
   {
      m_netList->empty();
      delete m_netList;
   }
}

//---------------------------------------------------------------

bool CSchematicNetlistFileReader::GetNextRecord(CFormatStdioFile &infile, CString &recordRetval)
{
   // Get next data record, skips comment lines.
   // Note data records may span multiple lines, when so, line continuation marker is present.

   recordRetval.Empty();
   bool retval = false;

   if (true)//this->m_infileIsOpen)
   {
      // Lines starting with "*" are continued from previous line
      bool lineIsContinued = true;
      
      do
      {
         CString buf;
         if (retval = (infile.ReadString(buf)?true:false))
         {
            if (buf.GetLength() > 0 && buf.Left(1).Compare(";") == 0)  //  Example file has  comment lines that  start with semicolon
            {
               // skip comment
            }
            else
            {
               buf.Trim();
               recordRetval += buf;

               /*
               if (recordRetval.GetLength() > 0 && recordRetval.Right(1).Compare("\\") == 0)
               {
                  lineIsContinued = true;
                  recordRetval.TrimRight("\\");
               }
               else
               {
               */
                  lineIsContinued = false;
               /*
               }
               */
            }
         }
      } while (lineIsContinued && retval);
   }

   return retval;
}

//---------------------------------------------------------------

void CSchematicNetlistFileReader::ReadVBKeyinNetlistFile(CString filename)
{
   if (m_netList != NULL)
   {
      this->m_netList->empty();

      CFormatStdioFile infile;

      if (!infile.Open(filename, CFile::modeRead))
      {
         CString tmp;
         tmp.Format("Can not open input file [%s]", filename);
         ErrorMessage(tmp);
         return;
      }

      CString recordStr;

      int state = 0;  // 0 = non-data section, 1 = page section (has nets), 2 = part section (though we're not using it)

      CString prevNetName; // line continuations are marked on next line, so need to save netname from prev line

      while (GetNextRecord(infile, recordStr))
      {
         if (recordStr.Left(1).Compare("%") == 0)
         {
            // Keyword line, clear state, look for new state
            state = 0;
            CString lowerCase(recordStr);
            lowerCase.MakeLower();
            if (lowerCase.Find("%page") > -1)
               state = 1;
            // we don't care about the other possible sections for now
         }
         else
         {
            // Parse a net/comppins

            // This format uses backslash as a quote. These mess up the CSupString ParseQuote, they get
            // treated as C-style escape sequence chars. So convert them to regular double quotes.
            recordStr.Replace('\\', '"');

            CSupString recordSupStr(recordStr);
            CStringArray fields;
            recordSupStr.ParseQuote(fields, " ");

            if (fields.GetCount() > 0)
            {
               CString curNetName = fields.GetAt(0);
               curNetName.Trim("\\");

               if (curNetName.Compare("*") == 0)
                  curNetName = prevNetName;
               else
                  prevNetName = curNetName;

               NetStruct *net = m_netList->addNet(curNetName);

               for (int i = 1 /*skip 0, is netname*/; i < fields.GetCount(); i++)
               {
                  CString pinref( fields.GetAt(i) );

                  CSupString pinrefSupStr(pinref);
                  CStringArray pinrefFields;
                  pinrefSupStr.Parse(pinrefFields, "-");

                  CString refname;
                  CString pinname;

                  if (pinrefFields.GetCount() > 0)
                     refname = pinrefFields.GetAt(0);
                  if (pinrefFields.GetCount() > 1)
                     pinname = pinrefFields.GetAt(1);

                  if (!refname.IsEmpty() && !pinname.IsEmpty())
                     net->addCompPin(refname, pinname);
               }
            }
         }

     


      }

      // All done with file
      infile.Close();

   }


}


//---------------------------------------------------------------

void CSchematicNetlistFileReader::ReadViewlogicNetlistFile(CString filename)
{
   // This uses the Net List file reader from the Boardstation importer, aka MentorIn

   if (m_netList != NULL)
   {
      CViewlogicNetlistReader reader(filename);

      reader.ReadNetlist(*m_netList);
   }
}

//---------------------------------------------------------------

void CSchematicNetlistFileReader::ReadBoardstationNetlistFile(CString filename)
{
   // This uses the Net List file reader from the Boardstation importer, aka MentorIn

   if (m_netList != NULL)
   {
      CMentorBoardstationNetlistReader reader(filename);

      reader.ReadNetlist(*m_netList);
   }
}

//---------------------------------------------------------------

void CSchematicNetlistFileReader::ReadPadsPowerPcbNetlist(CString filename)
{
   // open file for reading
   FILE *fp;
   if ((fp = fopen(filename, "rt")) == NULL)
   {
      CString tmp;
      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }

   CCamCadData &ccData = m_netList->getCamCadData();

   CString inputLine;
   int lcnt = 0;

   CString currentSignal;
   int state = 0;  // 0 = in no interesting section
                   // 1 = in nets section
                   // 9 = all done, quit, close up and go home
                   // Other sections possible but not of current interest
                   // Using blank/non-blank currentSignal to indicate in progress in SIGNAL section

   while (fgetcs(inputLine, fp) && state < 9)
   {
      lcnt++;
      inputLine.TrimRight("\n");

      if (inputLine.Left(1) == "*")
      {
         // Is a command line

         CSupString commandLine(inputLine);
         CStringArray commandFields;
         commandLine.ParseQuote(commandFields, " ");

         CString command;
         if (commandFields.GetCount() > 0)
            command = commandFields.GetAt(0);

         if (command.CompareNoCase("*NET*") == 0)  
         {
            state = 1; // starts section, no additional data on same line
         }
         else if (command.CompareNoCase("*SIGNAL*") == 0)
         {
            // Start of new net, net name is next field
            if (commandFields.GetCount() > 1)
               currentSignal = commandFields.GetAt(1);
         }
         else
         {
            // Some other command, no other commands are known to exist within *NET* and *SIGNAL* sections, so
            // we must not be in such a section. Clear the state.
            // In fact, if we have been in the *nets* state and now we find a command that is neither
            // *NET* nor *SIGNAL*, we're not only out of the nets section, we're done with this file, so
            // no need to waste time reading the rest of the file since we won't do anything with it.
            // Maybe someday we will want to, we can change it then.
            if (state == 1)
               state = 9;
            else
               state = 0;
         }
      }
      else
      {
         // Is not a command line, might be data of interest, depending on current state

         if (state == 1 && !currentSignal.IsEmpty())
         {
            // Process comppins from line

            CSupString dataLine(inputLine);
            CStringArray dataFields;
            dataLine.ParseQuote(dataFields, " ");

            NetStruct *net = m_netList->addNet(currentSignal); // find or add

            for (int i = 0; i < dataFields.GetCount(); i++)
            {
               CString datum( dataFields.GetAt(i) );

               int delimIndx = datum.Find(".");

               // Yes, > 0, not > -1, on purpose, look at usage in .Mid()
               if (delimIndx > 0 && (delimIndx + 1) < datum.GetLength())
               {
                  CString refname( datum.Mid(0, delimIndx) );
                  CString pinname( datum.Mid(delimIndx + 1)); // remainder

                  if (!refname.IsEmpty() && !pinname.IsEmpty())
                  {
                     net->addCompPin(refname, pinname);
                  }
               }

            }
         }
         // else of no interest
      }
   }

   fclose(fp);
}

//---------------------------------------------------------------

void CSchematicNetlistFileReader::ReadCamcadNetlistFile(CString filename)
{
   // Modified from:
   // static int LoadNetlist(CCEtoODBDoc *doc, FILE *wfp, FILE *log, FileStruct *file,
   //                   CNetPinsArray *netpinarray, int netpincnt, bool validateCompPin)


   // open file for reading
   FILE *wfp;
   if ((wfp = fopen(filename, "rt")) == NULL)
   {
     CString tmp;
     tmp.Format("Can not open file [%s]", filename);
     ErrorMessage(tmp);
     return;
   }


   char line[255];
   char *lp;
   long lcnt = 0;
	CString cur_netname = "";
   NetStruct *cur_netptr = NULL;
   CompPinStruct *cur_comppinptr = NULL;
   int err = 0;
   int netloaded = 0;
   int comppinloaded = 0;

   while (fgets(line, 255, wfp))
   {
      lcnt++;
      if ((lp = get_string(line, " \t\n")) == NULL)
			continue;
      CString cmd = lp;
      cmd.TrimLeft();
      cmd.TrimRight();

      if (cmd.GetLength() == 0 || cmd.Left(1) != '.')
			continue;

      if (!cmd.CompareNoCase(".PAGEUNIT"))
      {
         if ((lp = get_string(NULL, " :\t\n")) == NULL)
				continue;
         int units = GetUnitIndex(lp);
      }
      else if (!cmd.CompareNoCase(".NET"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
				continue;
         cur_netname = lp;
         cur_netptr = m_netList->addNet(cur_netname);  // add_net(file,lp);
         netloaded++;
      }
      else if (!cmd.CompareNoCase(".COMPPIN"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
				continue;
         CString comp = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
				continue;
         CString pin = lp;

         //if (validateCompPin && !comppin_exist(netpinarray, netpincnt, comp, pin))
         //{
         //   fprintf(log, "Compoment [%s] Pin [%s] is not placed at Line %ld\n", comp, pin, lcnt);
         //   err++;
         //}
         //else 
         if (strlen(cur_netname))
         {
            //if (test_add_comppin(comp, pin, file) == NULL)
            //{
               cur_comppinptr = cur_netptr->addCompPin(comp, pin); // add_comppin(file, cur_netptr, comp, pin);
					comppinloaded++;
            //}
            //else
            //{
            //   //fprintf(log, "Compoment [%s] Pin [%d] already assigned to Net. Skip add to Net [%s] at Line %ld\n", comp, pin, cur_netname, lcnt);
            //   err++;
            //}
         }
         else
         {
            //fprintf(log, ".COMPPIN without a preceeding .NET or .UNUSEDNET at %ld\n", lcnt);
            err++;
         }
      }
      else if (!cmd.CompareNoCase(".COMPPINPOS"))
      {
#ifdef _DEBUG
         //fprintf(log, ".COMPPINPOS not supported, found at line %ld\n", lcnt);
         //err++;
#endif
      }
      else if (!cmd.CompareNoCase(".COMPPINPADSTACK"))
      {
#ifdef _DEBUG
         //fprintf(log, ".COMPPINPADSTACK not supported, found at line %ld\n", lcnt);
         //err++;
#endif
      }
      else if (!cmd.CompareNoCase(".ATTACH2NET"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attachTo = lp;
   
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int garbage = atoi(lp);  // this one is constant 2, seems to be a throw-away

         lcnt++;
         if (!fgets(line,255,wfp))              
         {
            //fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }

         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            //fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString key = lp;

         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            //fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (cur_netptr != NULL && cur_netptr->getNetName().CompareNoCase(attachTo) == 0)
         {
            cur_netptr->attributes(); // To make sure not null, need to make a getDefinedAttributes() 

            // doc->SetUnknownAttrib(&cur_netptr->getAttributesRef(), key, val, valtype2, NULL); 
            ValueTypeTag valueType = intToValueTypeTag(valtype);
            if (valueType == valueTypeUndefined)
               valueType = valueTypeString;
            int kw = this->m_tempCamCadData.getDefinedAttributeKeywordIndex(key, valueType);
            this->m_tempCamCadData.setAttribute(*cur_netptr->getAttributesRef(), kw, val);
            
         }
      }
      else if (!cmd.CompareNoCase(".ATTACH2COMPPIN"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attachTo = lp;
   
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int garbage = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype = atoi(lp);

         lcnt++;
         if (!fgets(line,255,wfp))              
         {
            //fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }

         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            //fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString key = lp;

         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            //fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (cur_comppinptr != NULL && cur_comppinptr->getRefDes().CompareNoCase(attachTo) == 0)
         {
            //doc->SetUnknownAttrib(&cur_comppinptr->getDefinedAttributes(), key, val, valtype2, NULL);
            ValueTypeTag valueType = intToValueTypeTag(valtype);
            if (valueType == valueTypeUndefined)
               valueType = valueTypeString;
            int kw = this->m_tempCamCadData.getDefinedAttributeKeywordIndex(key, valueType);;
            this->m_tempCamCadData.setAttribute(*cur_comppinptr->getDefinedAttributes(), kw, val);
         }
      }
      else if (!cmd.CompareNoCase(".UNUSEDNET"))
      {
         NetStruct *n = m_netList->addNet(NET_UNUSED_PINS); // add_net(file, NET_UNUSED_PINS);
         n->setFlagBits(NETFLAG_UNUSEDNET);
         cur_netname = NET_UNUSED_PINS;
      }
      else
      {
         //fprintf(log, "Unknown command [%s] in Netlist at %ld\n", cmd, lcnt);
         err++;
      }

   }
   
   //CString tmp = "";
   //tmp.Format("Netlist load completed:\nTotal nets loaded          : %-6d\nTotal Comp/Pins loaded: %-6d\n",
   //      netloaded, comppinloaded);
   //ErrorMessage(tmp, "Netlist Load", MB_OK | MB_ICONEXCLAMATION);

   //return err;
} 


/******************************************************************************
*/

CViewlogicNetlistReader::CViewlogicNetlistReader(CString filename)
: m_inputFilename(filename)
{
}

CViewlogicNetlistReader::~CViewlogicNetlistReader()
{
}

bool CViewlogicNetlistReader::ParseNameValuePair(CString inputStr, CString &outputName, CString &outputValue)
{
   outputName.Empty();
   outputValue.Empty();

   int delimIndx = inputStr.Find("=");

   // Yes, > 0, not > -1, on purpose, look at usage in .Mid()
   if (delimIndx > 0 && (delimIndx + 1) < inputStr.GetLength())
   {
      outputName  = inputStr.Mid(0, delimIndx);
      outputValue = inputStr.Mid(delimIndx + 1); // remainder
   }

   return !(outputName.IsEmpty() || outputValue.IsEmpty());
}

void CViewlogicNetlistReader::ReadNetlist(CNetList &netlistToPopulate)
{


   if (true) //m_netList != NULL)
   {
      // open file for reading
      FILE *fp;
      if ((fp = fopen(m_inputFilename, "rt")) == NULL)
      {
         CString tmp;
         tmp.Format("Can not open file [%s]", m_inputFilename);
         ErrorMessage(tmp);
         return;
      }

      CCamCadData &ccData = netlistToPopulate.getCamCadData();

      CViewlogicPackageMap     pkgMap;
      CViewlogicSymInstanceMap instMap;

      CString inputLine;
      int lcnt = 0;
      char *lp = NULL;

      while (fgetcs(inputLine, fp))
      {
         lcnt++;
         inputLine.TrimRight("\n");

         // Update peculiar quotes to friendly quotes, helps supstring ParseQuote
         //inputLine.Replace("'", " \" ");
         //inputLine.Replace("`", " \" ");

         CSupString supInputLine( inputLine );
         CStringArray fields;
         //supInputLine.ParseQuote(fields, " ");
         supInputLine.Parse(fields, " '`");

         if (fields.GetCount() > 0)
         {
            CString cmd( fields.GetAt(0) );

            // Cmds that start with A appear to be Attribute settings.
            // AN - Attrib Net
            // AP - Attrib Part
            // etc
            if (cmd.CompareNoCase("AN") == 0)
            {
               if (fields.GetCount() > 1)
               {
                  CString netname( fields.GetAt(1) );

                  if (!netname.IsEmpty())
                  {
                     NetStruct *n = netlistToPopulate.addNet(netname);

                     if (fields.GetCount() > 2)
                     {
                        CString attrNam;
                        CString attrVal;
                        ParseNameValuePair(fields.GetAt(2), attrNam, attrVal);

                        if (!attrNam.IsEmpty())
                        {
                           int kw = ccData.getDefinedAttributeKeywordIndex(attrNam, valueTypeString);
                           ccData.setAttribute(n->attributes(), kw, attrVal);
                        }

                     }
                  }
               }
            }
            else if (cmd.CompareNoCase("AS") == 0)
            {
               // Attributes on what seems to be a package definition.

               // We want the PINORDER setting from this, it looks like this is the pin names.
               CString pkgName;
               if (fields.GetCount() > 1)
                  pkgName = fields.GetAt(1);

               CString paramName;
               if (fields.GetCount() > 2)
                  paramName = fields.GetAt(2);

               if (paramName.Find("=") > -1)
                  paramName.Truncate(paramName.Find("="));

               if (!pkgName.IsEmpty() && paramName.CompareNoCase("PINORDER") == 0)
               {
                  // The rest of the fields are pin names, get them and add them to package.
                  // Reparse the line, first param might be "attached" to original paramName in first go.

                  CSupString secondSupStr( inputLine );
                  CStringArray fields_2;
                  secondSupStr.Parse(fields_2, " '`=");

                  CViewlogicPackage *vpkg = NULL;
                  if (!pkgMap.Lookup(pkgName, vpkg))
                  {
                     vpkg = new CViewlogicPackage(pkgName);
                     pkgMap.setAt(pkgName, vpkg);
                  }
                  
                  if (vpkg != NULL && vpkg->GetPinCount() == 0) // only apply PINORDER once per pkg
                  {
                     for (int indx = 3; indx < fields_2.GetCount(); indx++)
                     {
                        CString pinName( fields_2.GetAt(indx) );
                        vpkg->AddPin(pinName);
                     }
                  }
               }
               else if (!pkgName.IsEmpty() && paramName.CompareNoCase("SIGNAL") == 0)
               {
                  CString fieldVal;
                  if (fields.GetCount() > 2)
                     fieldVal = fields.GetAt(2);

                  int delimIndx = fieldVal.Find("=");
                  CString signalVal( fieldVal.Mid(delimIndx+1) );
                  delimIndx = signalVal.Find(";");

                  // Yes, > 0, not > -1, on purpose, look at usage in .Mid()
                  if (delimIndx > 0 && (delimIndx + 1) < signalVal.GetLength())
                  {
                     CString sigName( signalVal.Mid(0, delimIndx)  );
                     CString pinName( signalVal.Mid(delimIndx + 1) ); // Remainder, (attrNVP.GetLength() - 1)) );

                     if (!sigName.IsEmpty() && !pinName.IsEmpty())
                     {
                        CViewlogicPackage *vpkg = NULL;
                        if (!pkgMap.Lookup(pkgName, vpkg))
                        {
                           vpkg = new CViewlogicPackage(pkgName);
                           pkgMap.setAt(pkgName, vpkg);
                        }

                        vpkg->AddSignal(sigName, pinName);
                     }
                  }
               }
            }
            else if (cmd.CompareNoCase("AP") == 0)
            {
               CString pkgName;
               if (fields.GetCount() > 1)
                  pkgName = fields.GetAt(1);

               CString logicalPinNum;
               if (fields.GetCount() > 2)
                  logicalPinNum = fields.GetAt(2);

               CString attrNVP;
               if (fields.GetCount() > 3)
                  attrNVP = fields.GetAt(3);

               CString attrNam;
               CString attrVal;
               ParseNameValuePair(attrNVP, attrNam, attrVal);

               if (attrNam.Compare("#") == 0)
               {
                  // Is logical to physical pin mapping
                  int pinLogNum  = atoi(logicalPinNum);

                  if (pinLogNum > 0 && !attrVal.IsEmpty())
                  {
                     CViewlogicPackage *vpkg = NULL;
                     if (pkgMap.Lookup(pkgName, vpkg))
                     {
                        vpkg->AddPinNum(pinLogNum, attrVal);  //*rcf BUG The val can be a list of physical pins, eg quad pack part
                                                              // has multiple physical pins for same logical purpose in different elements
                     }
                  }
               }
            }
            else if (cmd.CompareNoCase("API") == 0)
            {
               // pin instance

               CString instanceName;
               if (fields.GetCount() > 1)
                  instanceName = fields.GetAt(1);

               CString symbolName;
               if (fields.GetCount() > 2)
                  symbolName = fields.GetAt(2);

               CString logicalPinNumStr;
               if (fields.GetCount() > 3)
                  logicalPinNumStr = fields.GetAt(3);

               CString attrNVP;
               if (fields.GetCount() > 4)
                  attrNVP = fields.GetAt(4);

               CString attrNam;
               CString attrVal;
               ParseNameValuePair(attrNVP, attrNam, attrVal);

               if (attrNam.Compare("#") == 0)
               {
                  // Is logical to physical pin mapping
                  int pinLogNum  = atoi(logicalPinNumStr);

                  if (pinLogNum > 0 && !attrVal.IsEmpty())
                  {
                     CViewlogicSymInstance *symInst = NULL;
                     if (instMap.Lookup(instanceName, symInst))
                     {
                        int pinIndx = pinLogNum - 1;  // convert 1-based to 0-based
                        CString refname( symInst->GetRefname() );
                        CString pinname( attrVal );

                        CString portname;
                        CViewlogicPackage *vpkg = NULL;
                        if (pkgMap.Lookup(symbolName, vpkg))
                           portname = vpkg->GetPinPortNameAt(pinIndx);

                        if (pinIndx >= 0 && pinIndx < symInst->GetNetCount() && 
                           !refname.IsEmpty() && !pinname.IsEmpty())
                        {
                           CString netname( symInst->GetNetAt(pinIndx) );
                           if (!netname.IsEmpty())
                           {
                              NetStruct *net = netlistToPopulate.addNet(netname);
                              CompPinStruct *cp = net->addCompPin(refname, pinname);

                              if (!portname.IsEmpty())
                              {
                                 int portnameKW = ccData.getDefinedAttributeKeywordIndex(SCH_ATT_PORTNAME, valueTypeString);
                                 ccData.setAttribute(cp->attributes(), portnameKW, portname);
                              }
                           }
                        }
                     }
                  }
               }

            }
            else if (cmd.CompareNoCase("I") == 0)
            {

               CViewlogicSymInstance *thisInstance = NULL;

               CString instanceName;
               if (fields.GetCount() > 1)
                  instanceName = fields.GetAt(1);

               CString symbolName;
               if (fields.GetCount() > 2)
                  symbolName = fields.GetAt(2);

               // Remainder of line is nets for pins, and attribs.
               // The attribs are after nets, but we need attrib REFDES in order to
               // make CompPins, so scan attribs in one pass and make CompPins in
               // second pass.

               // First pass, only look at attribs.
               // Looking for REFDES in particular.
               // Right now we just extract refname aka REFDES, but could collect all
               // attribs and put them on CompPins.

               CString refname;
               for (int fieldI = 3; fieldI < fields.GetCount() && refname.IsEmpty(); fieldI++)
               {
                  CString fieldVal( fields.GetAt(fieldI) );

                  if (fieldVal.Find("=") > -1)
                  {
                     // is attrib name=value
                     CString attrNVP( fieldVal);
                     int delimIndx = attrNVP.Find("=");
                     // Yes, > 0, not > -1, on purpose, look at usage in .Mid()
                     if (delimIndx > 0 && (delimIndx + 1) < attrNVP.GetLength())
                     {
                        CString attrNam( attrNVP.Mid(0, delimIndx)  );
                        CString attrVal( attrNVP.Mid(delimIndx + 1) ); // Remainder, (attrNVP.GetLength() - 1)) );

                        if (attrNam.CompareNoCase("REFDES") == 0)
                        {
                           refname = attrVal;
                        }
                     }
                  }
                  else
                  {
                     // Is net, No Op for this pass
                  }
               }

               // Second pass, make CompPins

               if (refname.IsEmpty())      // must have non-blank refname
                  refname = instanceName;

               thisInstance = new CViewlogicSymInstance(instanceName, refname);
               instMap.setAt(instanceName, thisInstance);
         
               CViewlogicPackage *vpkg = NULL;
               pkgMap.Lookup(symbolName, vpkg);

               int fieldIndx = 3; // Where net names start

               if (!refname.IsEmpty() && vpkg != NULL)
               {
                  for (int pinIndx = 0; pinIndx < vpkg->GetPinCount(); pinIndx++)
                  {
                     CString fieldVal( fieldIndx < fields.GetCount() ? fields.GetAt(fieldIndx++) : "" );

                     if (fieldVal.Find("=") > -1)
                     {
                        // Is attrib name=value, error, means we ran out of net names too soon
                        int jj = 0;
                     }
                     else if (!fieldVal.IsEmpty()) // no blank net names
                     {
                        thisInstance->AddNet(fieldVal);
                     }
                  }
               }

               // Add CompPins for SIGNALs defined in package area
               
               if (vpkg != NULL && vpkg->GetSignalCount() > 0)
               {
                  for (int sigI = 0; sigI < vpkg->GetSignalCount(); sigI++)
                  {
                     CViewlogicSignal *sigptr = vpkg->GetSignalAt(sigI);

                     NetStruct *net = netlistToPopulate.addNet(sigptr->GetSignalName());
                     // This will get existing or add if not present, we just want
                     // the add if not present feature.
                     CompPinStruct *cp = net->getDefinedCompPin(refname, sigptr->GetPinName(), true);

                     if (cp != NULL)
                     {
                        int portnameKW = ccData.getDefinedAttributeKeywordIndex(SCH_ATT_PORTNAME, valueTypeString);
                        ccData.setAttribute(cp->attributes(), portnameKW, sigptr->GetSignalName());
                     }
                  }
               }

            }
         }

      }
   }
}

CViewlogicSignal::CViewlogicSignal(CString signame, CString pinname) : CViewlogicDataPair(signame, pinname)
{
}
