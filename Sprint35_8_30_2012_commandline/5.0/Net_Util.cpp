
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "pcbutil.h"
#include "net_util.h"
#include "attrib.h"
#include "mainfrm.h"
#include "CCEtoODB.h"
#include "Gauge.h"
#include "DcaEntity.h"
#include "RwUiLib.h"
#include "Sch_Lib.h"

extern SelectNets *selectNetsDlg; // from NETS.CPP
extern CString    BOM_clean_text(const char *c);   // from bom.cpp

void BOM_write_attributes(FILE *fp, CCEtoODBDoc *doc, const char *s, CAttributes* map); // from bom.cpp
int BOM_LoopNetList(CCEtoODBDoc *doc, CNetList *NetList, const char *refname, const char *pinname, 
                           const char *key, const char *val, int valtype1, int valtype2); // from bom.cpp

int CreateSinglePinNets(CCEtoODBDoc* doc, FileStruct* file, COperationProgress* progress);


typedef struct 
{
   CString  compname;
   CString  pinname;
} NU_NetlistPins;
typedef CTypedPtrArray<CPtrArray, NU_NetlistPins*> CNetPinsArray;

/******************************************************************************
* FindNet()
*/
NetStruct *FindNet(FileStruct *file, CString netname)
{
   // Case sensitive !

   if (file != NULL)
   {
      POSITION pos = file->getNetList().GetHeadPosition();
      while (pos != NULL)
      { 
         NetStruct *net = file->getNetList().GetNext(pos);
         if (!net->getNetName().Compare(netname))
            return net;
      }
   }

   return NULL;
}

/******************************************************************************
* FindNet()
*/
NetStruct *FindNet(CCEtoODBDoc *doc, FileStruct *file, CString attribName, CString searchValue)
{
   // Utility for finding nets based on unique aatribute values.
   // Not intended to find groups of nets with same attrib value.
   // Intended for certain exporters that rename nets and save new name as
   // an attribute. E.g. CKT_NETNAME and SPECTRUM_NETNAME.

   // CASE SENSItive !

   if (doc != NULL && file != NULL && !attribName.IsEmpty())
   {

      POSITION pos = file->getNetList().GetHeadPosition();
      while (pos)
      { 
         NetStruct *net = file->getNetList().GetNext(pos);
         //if (!net->getNetName().Compare(netname))
         //   return net;

         WORD kw = doc->RegisterKeyWord(attribName, 0, valueTypeString);
         Attrib *attrib;
         if (net->lookUpAttrib(kw, attrib))
         {
            CString val = attrib->getStringValue();

            if (val.Compare(searchValue) == 0)
            {
               return net;
            }
         }
      }
   }

   return NULL;
}

/******************************************************************************
* GenerateSinglePinNetname
*/
CString GenerateSinglePinNetname(const char *comp, const char *pin)
{
   CString tmp;
   tmp.Format("NC__%s.%s", comp, pin);
   return tmp;
}

/******************************************************************************
* RemoveCompPin()
*/
BOOL RemoveCompPin(FileStruct *file, CString comp, CString pin)
{
   NetStruct *net = NULL;
   CompPinStruct *cp = FindCompPin(file, comp, pin, &net);

   if (!cp)
      return FALSE;

   net->getCompPinList().deleteCompPin(cp);
   //net->getCompPinList().RemoveAt(net->getCompPinList().Find(cp));
   //FreeCompPin(cp);

   return TRUE;
}

BOOL RemoveCompPins(FileStruct *file, CString comp)
{
   int removeCount = 0;

   for (POSITION netPos = file->getHeadNetPosition();netPos != NULL;)
   {
      NetStruct* net = file->getNextNet(netPos);

      for (POSITION netPos = net->getHeadCompPinPosition();netPos != NULL;)
      {
         POSITION oldNetPos = netPos;

         CompPinStruct* compPin = net->getNextCompPin(netPos);

         if (compPin->getRefDes().Compare(comp) == 0)
         {
            net->getCompPinList().deleteAt(oldNetPos);
            //FreeCompPin(compPin);
            removeCount++;
         }
      }
   }

   return (removeCount > 0);
}

/******************************************************************************
* FindAndMoveCompPin()
*/
BOOL FindAndMoveCompPin(FileStruct *file, CString comp, CString pin, NetStruct *newNet)
{
   NetStruct *net = NULL;
   CompPinStruct *cp = FindCompPin(file, comp, pin, &net);

   if (!cp)
      return FALSE;

   return MoveCompPin(net, newNet, cp);
}

/******************************************************************************
* MoveCompPin()
*/
BOOL MoveCompPin(NetStruct *oldNet, NetStruct *newNet, CompPinStruct *cp)
{
   POSITION pos = oldNet->getCompPinList().find(cp);

   if (!pos)
      return FALSE;

   oldNet->getCompPinList().removeAt(pos);

   newNet->getCompPinList().addTail(cp);

   return TRUE;
}

/******************************************************************************
* FindCompPin()
*/
CompPinStruct *FindCompPin(FileStruct *file, CString comp, CString pin, NetStruct **net)
{
   if (file != NULL)
   {
      NetStruct   *net1;   
      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      { 
         net1 = file->getNetList().GetNext(netPos);
         CompPinStruct* cp = net1->findCompPin(comp, pin, true);
         if (cp != NULL)
         {
            *net = net1;
            return cp;
         }
      }
   }
   
   return NULL;
}

/*****************************************************************************/
/*
*/
static int comppin_exist(CNetPinsArray *netpinarray, int netpincnt, const char *c, const char *p)
{
   int   i;

   for (i=0;i<netpincnt;i++)
   {
      NU_NetlistPins *cp = netpinarray->ElementAt(i);

      if (cp->compname.Compare(c) == 0 && cp->pinname.Compare(p) == 0 )
          return 1;
   }
   return 0;   
}

/******************************************************************************
* LoadNetlist
*/
static int LoadNetlist(CCEtoODBDoc *doc, FILE *wfp, FILE *log, FileStruct *file,
                       CNetPinsArray *netpinarray, int netpincnt, bool validateCompPin)
{
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
         cur_netptr = add_net(file,lp);
         cur_netname = lp;
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

         if (validateCompPin && !comppin_exist(netpinarray, netpincnt, comp, pin))
         {
            fprintf(log, "Compoment [%s] Pin [%s] is not placed at Line %ld\n", comp, pin, lcnt);
            err++;
         }
         else if (strlen(cur_netname))
         {
            if (test_add_comppin(comp, pin, file) == NULL)
            {
               cur_comppinptr = add_comppin(file, cur_netptr, comp, pin);
					comppinloaded++;
            }
            else
            {
               fprintf(log, "Compoment [%s] Pin [%d] already assigned to Net. Skip add to Net [%s] at Line %ld\n", comp, pin, cur_netname, lcnt);
               err++;
            }
         }
         else
         {
            fprintf(log, ".COMPPIN without a preceeding .NET or .UNUSEDNET at %ld\n", lcnt);
            err++;
         }
      }
      else if (!cmd.CompareNoCase(".COMPPINPOS"))
      {
#ifdef _DEBUG
         fprintf(log, ".COMPPINPOS not supported, found at line %ld\n", lcnt);
         err++;
#endif
      }
      else if (!cmd.CompareNoCase(".COMPPINPADSTACK"))
      {
#ifdef _DEBUG
         fprintf(log, ".COMPPINPADSTACK not supported, found at line %ld\n", lcnt);
         err++;
#endif
      }
      else if (!cmd.CompareNoCase(".ATTACH2NET"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attachTo = lp;
   
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype1 = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype2 = atoi(lp);

         lcnt++;
         if (!fgets(line,255,wfp))              
         {
            fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }

         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString key = lp;

         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (cur_netptr != NULL && cur_netptr->getNetName().CompareNoCase(attachTo) == 0)
         {
            cur_netptr->attributes(); //*rcf to make sure not null, need to make a getDefinedAttributes() 

            doc->SetUnknownAttrib(&cur_netptr->getAttributesRef(), key, val, valtype2, NULL);
         }
      }
      else if (!cmd.CompareNoCase(".ATTACH2COMPPIN"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attachTo = lp;
   
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype1 = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype2 = atoi(lp);

         lcnt++;
         if (!fgets(line,255,wfp))              
         {
            fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }

         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString key = lp;

         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            fprintf(log, "Format error in NETLIST at %ld\n",lcnt);
            //display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         if (cur_comppinptr != NULL && cur_comppinptr->getRefDes().CompareNoCase(attachTo) == 0)
         {
            doc->SetUnknownAttrib(&cur_comppinptr->getDefinedAttributes(), key, val, valtype2, NULL);
         }
      }
      else if (!cmd.CompareNoCase(".UNUSEDNET"))
      {
         NetStruct *n = add_net(file, NET_UNUSED_PINS);
         n->setFlagBits(NETFLAG_UNUSEDNET);
         cur_netname = NET_UNUSED_PINS;
      }
      else
      {
         fprintf(log, "Unknown command [%s] in Netlist at %ld\n", cmd, lcnt);
         err++;
      }

   }
   
   CString tmp = "";
   tmp.Format("Netlist load completed:\nTotal nets loaded          : %-6d\nTotal Comp/Pins loaded: %-6d\n",
         netloaded, comppinloaded);
   MessageBox(NULL, tmp, "Netlist Load", MB_OK | MB_ICONEXCLAMATION);

   return err;
} 

//--------------------------------------------------------------
static int WriteNetlist(FILE *fnet, CCEtoODBDoc *doc, CNetList *NetList)
{
   NetStruct      *net;
   CompPinStruct  *compPin;
   POSITION       compPinPos, netPos;
   int            found = 0;
   CString        attach_string;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)   
         fprintf(fnet, ".UNUSEDNET\n");
      else
         fprintf(fnet, ".NET  %s\n", BOM_clean_text(net->getNetName()));

      attach_string.Format(".ATTACH2NET %s ", BOM_clean_text(net->getNetName()));
      BOM_write_attributes(fnet, doc, attach_string, net->getAttributesRef());

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         CString  attach_string;

         compPin = net->getNextCompPin(compPinPos);

         CString  cc, pp;
         cc = BOM_clean_text(compPin->getRefDes());
         pp = BOM_clean_text(compPin->getPinName());
         fprintf(fnet,".COMPPIN %s %s\n", cc, pp);

         //.comppinpos x, y, padstack etc...
         if (compPin->getPinCoordinatesComplete())
         {
            char *vis = "NONE";
            if (compPin->getVisible() == visibleTop) 
               vis = "TOP";
            else
            if (compPin->getVisible() == visibleBottom) 
               vis = "BOTTOM";
            else
            if (compPin->getVisible() == visibleBoth) 
               vis = "BOTH";
            fprintf(fnet,".COMPPINPOS %s %s %lg %lg %s\n", cc, pp, compPin->getOriginX(), compPin->getOriginY(), vis);
            if (compPin->getPadstackBlockNumber() > -1)
            {
               CString padstackname;
               BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
               if (block)
               {
                  padstackname = BOM_clean_text(block->getName());
                  fprintf(fnet,".COMPPINPADSTACK %s %s %s %d %lg\n", cc, pp, padstackname, compPin->getMirror(), compPin->getRotationRadians());
               }
            }
         }
         attach_string.Format(".ATTACH2COMPPIN %s %s", cc, pp);
         BOM_write_attributes(fnet, doc, attach_string, compPin->getAttributesRef());
         found++;
      }
      fprintf(fnet,"\n");
   }
   fprintf(fnet,"\n");
   return found;

} /* end WriteNetList */

/******************************************************************************
* OnNetlistRepair
*/
void CCEtoODBDoc::OnNetlistRepair() 
{
   FILE  *flog;
	CString LogFile = GetLogfilePath("netlist.log");

   if (ErrorMessage("Netlist Repair updates the Netlist to match with Placement.\nRepair can not be Undone and the undo buffer will be cleared. Prior steps will be cleared and can't be undone.\nIt is recommended to Save the Netlist!\nDo you want to Continue Remove?", "Repair Netlist", MB_YESNO | MB_DEFBUTTON2)==IDYES)
   {
      if (ErrorMessage("Are you sure?", "Repair Netlist", MB_YESNO | MB_DEFBUTTON2)!=IDYES)
         return;
   }
   else
      return;

   if ((flog = fopen(LogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", LogFile);
      ErrorMessage(t, "Error");
   }

   // save all netlist info into a temporary ccn

   // generate a new netlist and save in array
   // load temprary netlist back

   // delete temp files.
   // TODO: Add your command handler code here

   POSITION pos = getFileList().GetHeadPosition();
   int found = 0;
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (!file->isShown())              continue;   // do not write files, which are not shows.
      if (file->getBlock() == NULL)  continue;   // empty file
      if (file->getBlockType() != BLOCKTYPE_PCB)  continue;

      POSITION netpos = file->getNetList().GetHeadPosition();
      if (!netpos)   continue;               
      found++;
   }

   if (found == 0)
   {
      ErrorMessage("No Visible PCB File with a Netlist found!", "Netlist Repair");
      return;
   }
   else
   if (found > 1)
   {
      ErrorMessage("Multiple Visible PCB Files found!\nSelect only One File", "Netlist Repair");
      return;
   }

   BeginWaitCursor();
   int   loaderror = 0;
   pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (!file->isShown())              continue;   // do not write files, which are not shows.
      if (file->getBlock() == NULL)  continue;   // empty file

      POSITION netpos = file->getNetList().GetHeadPosition();
      if (!netpos)                  continue;

      // set every cp->koos to false.
      // if on the end there are still set false, the component/pin does not exist.
      NetStruct *net;
      CompPinStruct *compPin; 
      POSITION compPinPos;
   
      while (netpos != NULL)
      {
         net = file->getNetList().GetNext(netpos);
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);
            compPin->setPinCoordinatesComplete(false);
         }
      }

      generate_PINLOC(this, file, 1);  // do it always

      // here report which pins did not make it.
      POSITION netPos;
   
      netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         net = file->getNetList().GetNext(netPos);
         // SaveAttribs(stream, &net->getAttributesRef());
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            compPin = net->getNextCompPin(compPinPos);
            if (!compPin->getPinCoordinatesComplete())
            {
               loaderror++;
               fprintf(flog,"Error: Component [%s] Pin [%s] in Net [%s] could not be found in Design ->Removed.\n",
                  compPin->getRefDes(), compPin->getPinName(), net->getNetName());
               RemoveCompPin(file, compPin->getRefDes(),compPin->getPinName());
            }
         }
      }

      if (loaderror)
         ErrorMessage("Netlist Repair done!", "Netlist Repair");
      else
         ErrorMessage("No Netlist Error found!", "Netlist Repair");

      break;
   }

   fclose(flog);

   if (loaderror)
      Notepad(LogFile);

   return;
}

/******************************************************************************
* OnNetlistLoad
*/
void CCEtoODBDoc::OnNetlistLoad() 
{
   // TODO: Add your command handler code here
   CString  netfilename;
   int      loaderror = 0;

   POSITION pos = getFileList().GetHeadPosition();
   int found = 0;
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (!file->isShown())              continue;   // do not write files, which are not shows.
      if (file->getBlock() == NULL)  continue;   // empty file
      if (file->getBlockType() != BLOCKTYPE_PCB)  continue;

      POSITION netpos = file->getNetList().GetHeadPosition();
      if (netpos)                
      {
         ErrorMessage("Visible PCB File has already a Netlist Loaded!\nRemove Netlist before Loading.", "Netlist Load");
         continue;
      }
      found++;
   }

   if (found == 0)
   {
      ErrorMessage("No Visible PCB File (without a Netlist) found!", "Netlist Load");
      return;
   }
   else
   if (found > 1)
   {
      ErrorMessage("Multiple Visible PCB Files found!\nSelect only One File", "Netlist Load");
      return;
   }

   CFileDialog FileDialog(TRUE, "ccn", "*.ccn",
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, 
         "CAMCAD Netlist (*.ccn)|*.ccn|All Files (*.*)|*.*||", NULL);

   if (FileDialog.DoModal() != IDOK) return;

   netfilename = FileDialog.GetPathName();

   // open file for reading
   FILE *wfp;
   if ((wfp = fopen(netfilename, "rt")) == NULL)
   {
     CString tmp;
     tmp.Format("Can not open file [%s]", netfilename);
     ErrorMessage(tmp);
     return;
   }
   
   FILE  *flog;
	CString LogFile = GetLogfilePath("netlist.log");
   if ((flog = fopen(LogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", LogFile);
      ErrorMessage(t, "Error");
   }

   pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (!file->isShown())              continue;   // do not write files, which are not shows.
      if (file->getBlock() == NULL)  continue;   // empty file
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         // first make a netlist, so that I can get all placed pins. I need this to check against
         // .COMPPIN
         CNetPinsArray  netpinarray;      // this is the device - mapping
         int         netpincnt;

         netpinarray.SetSize(100,100);
         netpincnt = 0;

         generate_NCnet(this,file); // this function generates the NC (non connect) net.

         NetStruct      *net;
         CompPinStruct  *compPin;
         POSITION       compPinPos, netPos;

         netPos = file->getNetList().GetHeadPosition();
         while (netPos != NULL)
         {
            net = file->getNetList().GetNext(netPos);
            compPinPos = net->getHeadCompPinPosition();
            while (compPinPos != NULL)
            {
               compPin = net->getNextCompPin(compPinPos);

               NU_NetlistPins *c = new NU_NetlistPins;
               netpinarray.SetAtGrow(netpincnt,c);  
               netpincnt++;   
               c->compname = compPin->getRefDes();
               c->pinname = compPin->getPinName();
            }
         }

         //FreeNetList(file);
         file->getNetList().empty();

         loaderror += LoadNetlist(this, wfp, flog, file, &netpinarray, netpincnt, true);

         for (int i=0;i<netpincnt;i++)
         {
            delete netpinarray[i];
         }
         netpinarray.RemoveAll();

         generate_PINLOC(this,file,0); // this function generates the PINLOC argument for all pins.

         break;
      }
   }

   fclose(wfp);

   fclose(flog);

   if (loaderror)
      Notepad(LogFile);

}

//**********************************************************************************
//**********************************************************************************

CSchematicBuilder::CSchematicBuilder()
: m_nextSheetNumber(1)  // sheet numbering starts at 1
{
	CString logFilename = GetLogfilePath("SchematicNetList.log");
   if ((m_logfp = fopen(logFilename,"wt")) == NULL)
   {
      //CString  t;
      //t.Format("Error open [%s] file", LogFile);
      //ErrorMessage(t, "Error");
   }
}

//----------------------------------------------------------------------------------

CSchematicBuilder::~CSchematicBuilder()
{
   CloseLogFile();
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::CopyAttributes(CCEtoODBDoc &destDoc, CAttributes &destAttribs, CAttributes &sourceAttribs)
{
   // Copy attributes from source to destination, covers situation where source and destination
   // belong to different CCamCadData object, in which case the CAttributes->CopyAll() will not work correctly.


   POSITION pos = sourceAttribs.GetStartPosition();
   while (pos != NULL)
   {
      Attrib *srcAttr = NULL;
      WORD srcKW = 0;

      sourceAttribs.GetNextAssoc(pos, srcKW, srcAttr);

      int kw = srcAttr->getKeywordIndex();

      if (srcKW != kw)
      {
         int jj = 0; //*rcf Unexpected !
      }

      ValueTypeTag valueType = srcAttr->getValueType();
      CString keywordName( srcAttr->getName() );

      if (!keywordName.IsEmpty())
      {
         int destKW = destDoc.RegisterKeyWord(keywordName, valueType);
         destDoc.getCamCadData().setAttribute(destAttribs, destKW, srcAttr->getStringValue());//, attributeUpdateOverwrite);
      }


   }
}

//----------------------------------------------------------------------------------


int CSchematicBuilder::CreateSchematicNetList(CCEtoODBDoc *doc, FileStruct *file, CNetList *sourceNetList)
{
   // Create netlist in doc/file that matches the one defined in sourceNetList.
   // Returns number of CompPins placed.

   int compPinCount = 0;

   if (doc != NULL && file != NULL && sourceNetList != NULL)
   {
      POSITION netpos = sourceNetList->GetHeadPosition();
      while (netpos != NULL)
      {
         NetStruct *sourceNet = sourceNetList->GetNext(netpos);

         if (sourceNet != NULL)
         {
            NetStruct *destNet = add_net(file, sourceNet->getNetName());

            if (sourceNet->getAttributes() != NULL)
            {
               // The following looks attractive, but can't use it because parent CCamCadData objects
               // are not the same. It will cause crash when later access is attempted because
               // sourceNetList is associated with a temporary CCamCadData object that is gone
               // when this Schematic Builder is destructed.
               ///////  destNet->getDefinedAttributes()->CopyAll( *sourceNet->getAttributesRef() );
               // So instead, iterate on existing attribs and create as new in destination.
               CopyAttributes(*doc, destNet->attributes(), *sourceNet->getAttributesRef());

            }

            POSITION compPinPos = sourceNet->getHeadCompPinPosition();
            while (compPinPos != NULL)
            {
               CompPinStruct *sourceCp = sourceNet->getNextCompPin(compPinPos);

               if (sourceCp != NULL)
               {
                  if (test_add_comppin(sourceCp->getRefDes(), sourceCp->getPinName(), file) == NULL)
                  {
                     CompPinStruct *destCp = add_comppin(file, destNet, sourceCp->getRefDes(), sourceCp->getPinName()); //*rcf does not use file arg, go fix add_comppin
                     compPinCount++;

                     if (sourceCp->getAttributesRef() != NULL)
                        this->CopyAttributes(*doc, destCp->attributes(), *sourceCp->getAttributesRef());
                  }
                  else
                  {
                     // error, comp pin already exists
                  }
               }
            }
         }
      }
   }

   return compPinCount;
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GenerateCompDesAndPinDesAttribs(CCamCadData &ccData, FileStruct *file)
{
   // On each CompPin in file place standard Schematic attributes compDesignator and]
   // pinDesignator. Attribs used by (required by) Schematic Navigator and visECAD.

   // If compDesignator and/or pinDesignator attribs are already present, leave them.
   // If not present, generate them from comppin refname and pinname.

   if (file != NULL)
   {
      int compDesignatorKw = ccData.getDefinedAttributeKeywordIndex(SCH_ATT_COMPDESIGNATOR, valueTypeString);
      int pinDesignatorKw =  ccData.getDefinedAttributeKeywordIndex(SCH_ATT_PINDESIGNATOR, valueTypeString);

      NetStruct      *net;
      CompPinStruct  *compPin;
      POSITION       compPinPos, netPos;

      netPos = file->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         net = file->getNetList().GetNext(netPos);
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            compPin = net->getNextCompPin(compPinPos);

            CString refname( compPin->getRefDes() );
            CString pinname( compPin->getPinName() );

            Attrib *attrib = NULL;

            if (!compPin->lookUpAttrib(compDesignatorKw, attrib))
               ccData.setAttribute(*compPin->getDefinedAttributes(), compDesignatorKw, refname.GetBuffer(0));

            if (!compPin->lookUpAttrib(pinDesignatorKw, attrib))
               ccData.setAttribute(*compPin->getDefinedAttributes(), pinDesignatorKw, pinname.GetBuffer(0));
         }
      }
   }
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::DetermineComponentList(CCamCadData &ccData, FileStruct *file, CMapStringToInt &componentList)
{
   // Scan netlist for CompPins, tally the refnames and pincount into componentList

   componentList.RemoveAll();

   if (file != NULL)
   {
      POSITION netpos = file->getNetList().GetHeadPosition();
      while (netpos != NULL)
      {
         NetStruct *net = file->getNetList().GetNext(netpos);

         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);

            CString compname( compPin->getRefDes() );
            CString pinname( compPin->getPinName() );

            int pinCount = 0;

            // Will set pinCount if in map, leave it alone if not
            componentList.Lookup(compname, pinCount);

            // Now add this pin
            pinCount++;

            // Save it
            componentList.SetAt(compname, pinCount);
         }
      }
   }
}


//----------------------------------------------------------------------------------

BlockStruct *CSchematicBuilder::GetDefinedPortBlock(CCamCadDatabase &ccdb, FileStruct *file, double segmentLen, double &portLen)
{
   // Get Pin aka Port graphic block, define it if not already present.

   // Return length of port vector, currently same as segmentLen, but could be changed for
   // aestetic purposes at any time. Length is used for convenience of placement calc.
   portLen = segmentLen;

   BlockStruct *portBlk = ccdb.getBlock("PinPort", file->getFileNumber());

   if (portBlk == NULL)
   {
      // Make pin with short horizontal segment with X at end for graphic.
      // Actually, X is marks the connect point, so will be at begining, making the connect point
      // and insert pt the same.
      int schematicLayer = ccdb.getDefinedLayerIndex("PortGraphic");
      portBlk = ccdb.getDefinedBlock("PinPort", blockTypeGatePort, file->getFileNumber());
      DataStruct *pinGraphicData = ccdb.addPolyStruct(portBlk, schematicLayer, 0, 0, graphicClassNormal);

      CPoly *poly1 = ccdb.addPoly(pinGraphicData, ccdb.getZeroWidthIndex(), false, false, false);
      poly1->addVertex(0.0, 0.0);        // the straight
      poly1->addVertex(portLen, 0.0);

      double crossOffset = segmentLen / 4.0;
      CPoly *poly2 = ccdb.addPoly(pinGraphicData, ccdb.getZeroWidthIndex(), false, false, false);
      poly2->addVertex(-crossOffset, -crossOffset);        // the / of X
      poly2->addVertex( crossOffset,  crossOffset);
      CPoly *poly = ccdb.addPoly(pinGraphicData, ccdb.getZeroWidthIndex(), false, false, false);
      poly->addVertex(-crossOffset,  crossOffset);         // the \ of X
      poly->addVertex( crossOffset, -crossOffset);
   }

   return portBlk;
}

//----------------------------------------------------------------------------------

BlockStruct *CSchematicBuilder::GenerateSchematicSymbol(CCamCadDatabase &ccdb, FileStruct *file, CString symbolName, int pinCount)
{
   // Get the graphic block for symbol with given pin count.

   int symbolLayer = ccdb.getDefinedLayerIndex("SymbolGraphic");
   int portLayer = ccdb.getDefinedLayerIndex("PortGraphic");

   int portnameKw = ccdb.getCamCadData().getDefinedAttributeKeywordIndex(SCH_ATT_PORTNAME,          valueTypeString);
   int dppinmapKw = ccdb.getCamCadData().getDefinedAttributeKeywordIndex(ATT_DEVICETOPACKAGEPINMAP, valueTypeString);

   // 2 mm line segment is basis of geometry graphic
   double segmentLen = ccdb.convertToPageUnits(pageUnitsMilliMeters, 2.0); 

   // The component symbol block
   BlockStruct *symBlk = ccdb.getNewBlock(symbolName, blockTypeSymbol, file->getFileNumber());

   // Pin graphic
   double portLen = 0.;  // value will be returned from GetDefinedPortBlock
   BlockStruct *portBlk = GetDefinedPortBlock(ccdb, file, segmentLen, portLen);

   // Symbol graphics created with origin in lower left corner of outline.

   // Outline
   double leftX = 0.;
   double rightX = 4.0 * segmentLen;
   double bottomY = 0.;
   double topY = (pinCount > 1) ? (((pinCount / 2) + 1) * segmentLen) : (2 * segmentLen);

   DataStruct *outlineData = ccdb.addPolyStruct(symBlk, symbolLayer, 0, 0, graphicClassComponentOutline); // Not really a comp outline, though
   CPoly *poly = ccdb.addClosedPoly(outlineData, ccdb.getZeroWidthIndex());
   poly->addVertex(leftX, bottomY);
   poly->addVertex(rightX, bottomY);
   poly->addVertex(rightX, topY);
   poly->addVertex(leftX, topY);
   poly->addVertex(leftX, bottomY);


   // Pin names not necessarilly numbers, use comp pin pinrefs.
   // For the even portion of pin count (2, 4, 6, and 2 out of 3, 4 out of 5, 6 out of 7, etc) pins
   // go with odd index on left and even index on right. The final pin of odd pin count part
   // goes centered on bottom.
   // Pins are places such that their connection point is the insert point for the net, and the
   // tail goes toward the comp outline.

   int pinsPerSide = pinCount / 2;
   bool oddTotalPinCount = ((pinCount / 2) * 2) != pinCount;

   //int pinNumber = 0;

   // Collect pin names into sorted string array  //*rcf THIS could be made into func to get sorted cstringarray of pins, maybe should be array of comppins
   CStringArray pinNames;
   CStringArray pinPortNames;

   POSITION netpos = file->getNetList().GetHeadPosition();
   while (netpos != NULL)
   {
      NetStruct *net = file->getNetList().GetNext(netpos);

      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         CString compname( compPin->getRefDes() );

         if (compname.Compare(symbolName) == 0)
         {
            // Insert in string array in sorted order
            CString pinname( compPin->getPinName() );
            CString sortablePinname( compPin->getSortablePinName() );

            // If specific portName attrib is present, use it as-is.
            // If not present, try std devtopinnamemap attrib.
            // If that not present either, flag as -no-port-name-.
            CString portName;
            Attrib *portNameAttr;
            if (compPin->lookUpAttrib(portnameKw, portNameAttr))
               portName = portNameAttr->getStringValue();
            if (portName.IsEmpty() && compPin->lookUpAttrib(dppinmapKw, portNameAttr))
            {
               portName = portNameAttr->getStringValue();
               // Use 1st char of mapped pin name, except for Cathode use "K"  (rules from Mark Laing)
               if (portName.CompareNoCase("Cathode") == 0)
                  portName = "K";  // Cathode maps to "K"
               else
                  portName.Truncate(1);  // Keep only 1st char
            }
            if (portName.IsEmpty())
               portName = "-no-port-name-";

            bool inserted = false;
				int pinIndx = 0;
            for (pinIndx = 0; pinIndx < pinNames.GetCount() && !inserted; pinIndx++)
            {
               CString existingEntry( CompPinStruct::getSortableReference(pinNames.GetAt(pinIndx)) );

               if (sortablePinname.CompareNoCase(existingEntry) < 0)
               {
                  pinNames.InsertAt(pinIndx, pinname);
                  inserted = true;
                  pinPortNames.InsertAt(pinIndx, portName);
               }
            }
            // If not inserted before any in list then goes at end of list
            if (!inserted)
            {
               int addedIndx = pinNames.Add(pinname);
               // Put port name in its array in order that matches pinNames
               pinPortNames.InsertAt(pinIndx, portName);
            }
         }
      }
   }

   for (int pinIndx = 0; pinIndx < pinNames.GetCount(); pinIndx++)
   {
      int pinNumber = pinIndx + 1;
      CString pinname( pinNames.GetAt(pinIndx) );
      CString portname( (pinIndx < pinPortNames.GetCount()) ? pinPortNames.GetAt(pinIndx) : "" );

      if (portname.CompareNoCase("-no-port-name-") == 0)
         portname.Empty();

      bool isOddNumberedPin = ((pinNumber / 2) * 2) != pinNumber;
      int pinNumberOnSide = (pinNumber / 2) + (pinNumber % 2);

      double pinX = 0.;
      double pinY = 0.;
      double pinRot = 0.;

      if (isOddNumberedPin)
      {
         if (pinNumber == pinCount)
         {
            // last odd pin goes on bottom
            pinX = segmentLen * 2.;  // centered on bottom
            pinY = bottomY - portLen;
            pinRot = 90.0; // degrees
         }
         else
         {
            // regular odd numbered pins go on left, increasing value top to bottom
            pinX = leftX - portLen;
            pinY = (pinsPerSide - pinNumberOnSide + 1) * segmentLen;
            pinRot = 0.0;
         }
      }
      else
      {
         // even numbered pins go on right
         pinX = rightX + portLen;
         pinY = (pinsPerSide - pinNumberOnSide + 1) * segmentLen;
         pinRot = 180.0;
      }

      DataStruct *portInsertData = ccdb.referenceBlock(symBlk, portBlk, insertTypePortInstance, pinname, portLayer, pinX, pinY, DegToRad(pinRot), false, 1.);

      CCamCadData &ccData = ccdb.getCamCadData();
      int designatorKw = ccData.getDefinedAttributeKeywordIndex(SCH_ATT_DESIGNATOR, valueTypeString); //*rcf Also do Port_Name attrib ?
      double oneMM = ccdb.convertToPageUnits(pageUnitsMilliMeters, 1.0); //*rcf don't need to keep setting this in loop
      double attrX = oneMM * .2;
      double attrY = oneMM * .2;
      double attrR = -pinRot;
      double height = oneMM * 0.8;
      double width = height * 0.6;
      HorizontalPositionTag textPosH = horizontalPositionLeft;
      VerticalPositionTag   textPosV = verticalPositionBottom;
      double portnameAttrX = oneMM * 2.5; // seems to work for all
      double portnameAttrY = oneMM * -0.35;
      if (pinRot == 180.0)
      {
         textPosH = horizontalPositionRight;
         attrY = -attrY;
         portnameAttrY = -portnameAttrY;
      }
      else if (pinRot == 90.0)
      {
         attrY = -width;  // move over, off of port line graphic
         portnameAttrY = 0.0;
      }

      ccData.setVisibleAttribute(*portInsertData->getDefinedAttributeMap(), designatorKw, valueTypeString, pinname.GetBuffer(0),
         attrX, attrY, DegToRad(attrR), height, width, false, ccData.getZeroWidthIndex(), true, attributeUpdateOverwrite, 0, 
         portLayer, true /*no mirror*/, false, textPosH, textPosV);

      if (!portname.IsEmpty())
      {
         ccData.setVisibleAttribute(*portInsertData->getDefinedAttributeMap(), portnameKw, valueTypeString, portname.GetBuffer(0),
            portnameAttrX, portnameAttrY, DegToRad(attrR), height, width, false, ccData.getZeroWidthIndex(), true, attributeUpdateOverwrite, 0, 
            portLayer, true /*no mirror*/, false, textPosH, textPosV);
      }


   }

   return symBlk;
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file, BlockStruct *sheetBlk, DataStruct *symData)
{
   // Create signal (aka trace) graphics for nets attached to comppins on given symbol in the given sheet.

   if (file != NULL && sheetBlk != NULL && symData != NULL)
   {
      GenerateSignalTraces(ccdb, file, sheetBlk->getDataList(), symData);
   }
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file, CDataList &sheetDataList, DataStruct *symData)
{
   // Create signal (aka trace) graphics for nets attached to comppins on given symbol in the given sheet.

   if (file != NULL && symData != NULL)
   {
      double unitLen = ccdb.convertToPageUnits(pageUnitsMilliMeters, 1.0);
      int schematicLayer = ccdb.getDefinedLayerIndex("SignalGraphic");

      InsertStruct *symInsert = symData->getInsert();
      BlockStruct *insertedSymBlk = ccdb.getBlock( symInsert->getBlockNumber() );

      CTMatrix symMat = symInsert->getTMatrix();

      if (insertedSymBlk != NULL)
      {
         for (CDataListIterator portIterator(*insertedSymBlk, insertTypePortInstance); portIterator.hasNext();)
         {
            DataStruct *portData = portIterator.getNext();
            InsertStruct *portInsert = portData->getInsert();

            // signX and Y, set to 0 for no change in x (or y) or +/- 1.0 for vector direction.
            double signX = 0.;
            double signY = 0.;
            HorizontalPositionTag textPosH = horizontalPositionLeft;
            VerticalPositionTag   textPosV = verticalPositionBottom;

            double portAngle = portInsert->getAngleDegrees();
            if (fpnear(portAngle, 0.0, .01))
            {
               signX = -1.;
               textPosH = horizontalPositionRight;
            }
            else if (fpnear(portAngle, 90.0, .01))
            {
               signY = -1.;
               textPosV = verticalPositionCenter;
            }
            else if (fpnear(portAngle, 180.0, .01))
            {
               signX = 1.;
            }
            else if (fpnear(portAngle, 270.0, .01))
            {
               signY =  1.;
               textPosV = verticalPositionCenter;
            }


            CString refname( symInsert->getRefname() );
            CString pinname( portInsert->getRefname() );

            NetStruct *net = NULL;
            CompPinStruct *compPin = FindCompPin(file, refname, pinname, &net);

            if (compPin != NULL)
            {
               CPoint2d portInsertPt( portInsert->getOrigin2d() );
               portInsertPt.transform(symMat);

               double vectorLen = unitLen * 2.;
               double vectorLenX = vectorLen * signX;
               double vectorLenY = vectorLen * signY;

               //DataStruct *signalData = ccdb.addPolyStruct(sheetBlk, schematicLayer, 0, 0, graphicClassSignal); 
               DataStruct *signalData = ccdb.addPolyStruct(sheetDataList, schematicLayer, 0, 0, graphicClassSignal); 
               CPoly *poly = ccdb.addOpenPoly(signalData, ccdb.getDefinedWidthIndex(unitLen * 0.1));
               poly->addVertex( portInsertPt.x , portInsertPt.y );
               poly->addVertex( portInsertPt.x + vectorLenX , portInsertPt.y + vectorLenY );

               ccdb.getCamCadData().setAttribute(*signalData->getDefinedAttributes(), standardAttributeNetName, net->getNetName());

               // Tried visible attribute for net name on signal, it does not work.
               // CAMCAD apparantly does not draw attribs on polystructs, even if set to visible.
               // So just place some text. Placement point is consistent regardless of net vector direction, but
               // the text origin varies depending on net verctor direction.
               double textX = portInsertPt.x + (vectorLenX / 2.) + (signX == 0.0 ? (unitLen * 0.2) : 0.);
               double textY = portInsertPt.y + (vectorLenY / 2.) + (signY == 0.0 ? (unitLen * 0.2) : 0.);
               double textH = unitLen * 0.90;
               //DataStruct *textData = ccdb.addText(sheetBlk, schematicLayer, net->getNetName(), textX, textY, textH, textH * 0.6);
               DataStruct *textData = ccdb.addText(sheetDataList, schematicLayer, net->getNetName(), textX, textY, textH, textH * 0.6);
               textData->getText()->setHorizontalPosition(textPosH);
               textData->getText()->setVerticalPosition(textPosV);
               textData->getText()->setPenWidthIndex(ccdb.getZeroWidthIndex());
            }
         }
      }     
   }
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file, BlockStruct *sheetBlk)
{
   // Create signal (aka trace) graphics for nets attached to comppins in the given sheet.

   if (file != NULL && sheetBlk != NULL)
   {
      for (CDataListIterator symbolIterator(sheetBlk->getDataList(), insertTypeSymbol); symbolIterator.hasNext();)
      {
         DataStruct *symData = symbolIterator.getNext();
         GenerateSignalTraces(ccdb, file, sheetBlk, symData);
      }
   }
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file)
{
   // Create signal (aka trace) graphics for nets attached to comppins for all sheets in doc.

   //*rcf BUG -- should only do it to sheet blocks with same file number as file, as it is
   // it is potentially altering some other schematic file.

   for (int blkIndx = 0; blkIndx < ccdb.getCamCadData().getMaxBlockIndex(); blkIndx++)
   {
      BlockStruct *blk = ccdb.getCamCadData().getBlockAt(blkIndx);
      if (blk != NULL && blk->getBlockType() == blockTypeSheet)
      {
         GenerateSignalTraces(ccdb, file, blk);
      }
   }

}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GetOutlineSize(CCamCadData &ccData, BlockStruct *block, int graphicClass, double *xSize, double *ySize)
{
   // Yet another incarnation of GetOutlineSize  (see, e.g. QD_Out.cpp)

   *xSize = *ySize = 0;

   double xmin, xmax, ymin, ymax;
   xmin = ymin = DBL_MAX;
   xmax = ymax = -DBL_MAX;

   Mat2x2 m;
   RotMat2(&m, 0);

   // find panel outline
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // found Panel Outline
      if (data->getGraphicClass() == graphicClass && data->getDataType() == T_POLY)
      {
         ExtentRect extents;
         extents = data->getPolyList()->getExtent(ccData);
         ///PolyExtents(doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, FALSE);

         // get extents
         if (extents.left < xmin) xmin = extents.left;
         if (extents.right > xmax) xmax = extents.right;
         if (extents.bottom < ymin) ymin = extents.bottom;
         if (extents.top > ymax) ymax = extents.top;
      }
   }

   if (xmax > xmin)
   {
      *xSize = xmax - xmin;
      *ySize = ymax - ymin;
   }
}

//----------------------------------------------------------------------------------

CString CSchematicBuilder::GetNextNewSheetName()
{
   CString sheetName;
   sheetName.Format("Sheet_%d", m_nextSheetNumber++);
   return sheetName;
}

//----------------------------------------------------------------------------------

BlockStruct *CSchematicBuilder::GetNextNewSheet(CCamCadDatabase &ccdb, CCEtoODBDoc &doc, FileStruct *file)
{
   BlockStruct *sheetBlk = NULL;

   CString sheetName( GetNextNewSheetName() );
   BlockStruct *existingBlk = ccdb.getBlock(sheetName, file->getFileNumber());
   if (existingBlk != NULL)
      doc.RemoveBlock(existingBlk);
   sheetBlk = ccdb.getNewBlock(sheetName, "_%d", blockTypeSheet, file->getFileNumber());
   sheetBlk->setBlockType(blockTypeSheet);

   // EDIF importers do this, visECAD seems to (foolishly) depend upon it
   int keyword = ccdb.getCamCadData().getDefinedAttributeKeywordIndex("SchematicStructure", valueTypeInteger);
   int value = 1;
   ccdb.getCamCadData().setAttribute(*sheetBlk->getDefinedAttributes(), keyword, valueTypeInteger, &value, attributeUpdateOverwrite, NULL);

   return sheetBlk;
}

//----------------------------------------------------------------------------------

DataStruct *CSchematicBuilder::PlaceSymbol(CCamCadDatabase &ccdb, BlockStruct *sheetBlk, BlockStruct *symbolBlk, CString refname, double symX, double symY, int schematicLayer)
{
#ifdef ONE_WAY
   CCamCadData &ccData = ccdb.getCamCadData();

   DataStruct *symbolData = ccdb.referenceBlock(sheetBlk, symbolBlk, insertTypeSymbol, refname, schematicLayer, symX, symY, 0., false, 1.);  
   columnHasSymbol = true;

   //ccData.setAttribute(*symbolData->getDefinedAttributeMap(), designatorKw, refname.GetBuffer(0));
   double outlineSizeX, outlineSizeY;
   GetOutlineSize(ccData, symbolBlk, graphicClassComponentOutline, &outlineSizeX, &outlineSizeY);
   double attrX = outlineSizeX / 2.0; //symExt.getCenter().x;  extent for 1 pin comp gets center off center
   double attrY = outlineSizeY / 2.0; //symExt.getCenter().y;
   double height = oneMM * 1.6;
   double width = height * 0.6;

   ccData.setVisibleAttribute(*symbolData->getDefinedAttributeMap(), designatorKw, valueTypeString, refname.GetBuffer(0),
      attrX, attrY, 0.0, height, width, false, ccData.getZeroWidthIndex(), true, attributeUpdateOverwrite, 0, 
      schematicLayer, true /*no mirror*/, horizontalPositionCenter, verticalPositionCenter);
#else
   // aka
   DataStruct *symbolData = PlaceSymbol(ccdb, sheetBlk->getDataList(), symbolBlk, refname, symX, symY, schematicLayer);
#endif

   return symbolData;
}

//----------------------------------------------------------------------------------

DataStruct *CSchematicBuilder::PlaceSymbol(CCamCadDatabase &ccdb, CDataList &dataList, BlockStruct *symbolBlk, CString refname, double symX, double symY, int schematicLayer)
{
   CCamCadData &ccData = ccdb.getCamCadData();
   double oneMM = ccdb.convertToPageUnits(pageUnitsMilliMeters, 1.0);

   DataStruct *symbolData = ccdb.referenceBlock(dataList, symbolBlk, insertTypeSymbol, refname, schematicLayer, symX, symY, 0., false, 1.);  

   //ccData.setAttribute(*symbolData->getDefinedAttributeMap(), designatorKw, refname.GetBuffer(0));
   double outlineSizeX, outlineSizeY;
   GetOutlineSize(ccData, symbolBlk, graphicClassComponentOutline, &outlineSizeX, &outlineSizeY);
   double attrX = outlineSizeX / 2.0; //symExt.getCenter().x;  extent for 1 pin comp gets center off center
   double attrY = outlineSizeY + oneMM;  ///  / 2.0; //symExt.getCenter().y;
   double height = oneMM * 1.6;
   double width = height * 0.6;

   int designatorKw = ccData.getDefinedAttributeKeywordIndex(SCH_ATT_DESIGNATOR, valueTypeString);
   ccData.setVisibleAttribute(*symbolData->getDefinedAttributeMap(), designatorKw, valueTypeString, refname.GetBuffer(0),
      attrX, attrY, 0.0, height, width, false, ccData.getZeroWidthIndex(), true, attributeUpdateOverwrite, 0, 
      schematicLayer, true /*no mirror*/, false, horizontalPositionCenter, verticalPositionBottom);

   return symbolData;
}

//----------------------------------------------------------------------------------
bool CSchematicBuilder::PlaceColumnInSheet(CCamCadDatabase &ccdb, FileStruct *file, BlockStruct **sheetBlkPtrPtr, CDataList &columnDataList)
{
   // Position column in sheet, move data from columnDataList to sheetBlk's data list.
   // Create new sheet if adding this column to existing sheet exceeds width threshold.
   // Return new sheet in sheetBlk if such is created.
   // Empty the columnDataList to prep for next column (take-data function does this for us).
   // Return true if returned sheetBlk has a column, otherwise false.

   CCamCadData &ccData = ccdb.getCamCadData();
   CCEtoODBDoc &doc = ccdb.getCamCadDoc();
   double oneMM = ccdb.convertToPageUnits(pageUnitsMilliMeters, 1.0);

   BlockStruct *sheetBlk = *sheetBlkPtrPtr;

         // Prepare to dump colum to sheet
         sheetBlk->calculateBlockExtents(ccData, false /*all,not just visible*/); // block extent is cached, force re-calc
         CExtent sheetExtent = sheetBlk->getExtent(ccData);
         CExtent columnExtent = columnDataList.getExtent(&ccData);

         bool sheetHasColumn = sheetExtent.isValid();

         // If sheet already has at least one column and this one puts it over width threshold then start new sheet
         if (sheetHasColumn &&
            ((sheetExtent.getXmax() + columnExtent.getXsize()) > (300.0 * oneMM)))
         {
            *sheetBlkPtrPtr = GetNextNewSheet(ccdb, doc, file);
            sheetBlk = *sheetBlkPtrPtr;
            sheetHasColumn = false;
         }

         // Dump column to sheet
         if (columnDataList.GetCount() > 0)
         {
            // Syms are defined with origin at LL of outline, so min X extent is magnitude of
            // offset we want, and will be less than or equal to zero, so can just use neg of it.
            double colX = -columnExtent.getXmin();
            // Add in a little leeway
            //*rcf BUG? Kind of odd here, extents seem to come out wider than necessary.
            // I suspect extent of text is not accurate, so tmp list comes out calc'd wider than reality.
            // Images are getting spaced out enough without this spacing added in, though I'm not liking that.
            // Look into accuracy of text extent.
            ////colX += (10.0 * oneMM);
            // Offset further by what is already in sheet.
            // If sheet has column then extent we got before is still valid, if sheet does
            // not have column then sheet extent is irrelevant.
            if (sheetHasColumn)
               colX += sheetExtent.getXmax();

            // Make X translation matrix, apply to entire temp data list
            CTMatrix tmat;
            tmat.translate(colX, 0.0);
            columnDataList.transform(tmat, &ccData);

            // Now transfer entire temp datalist to sheet
            sheetBlk->getDataList().takeData(columnDataList);
            
            sheetHasColumn = true;
         }

   return sheetHasColumn;
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GenerateSchematicGraphics(CCEtoODBDoc &doc, CCamCadData &ccData, FileStruct *file)
{
   // Place the symbols, draw the traces, etc., for NetList currently defined in file.

   CCamCadDatabase ccdb(doc);

   // Define a layer to stash everything on
   int schematicLayer = ccdb.getDefinedLayerIndex("SymbolGraphic");

   // Schematic List wants this attrib set, for correlation of symbol inserts to comppins.
   // It will not work on insert refname directly, seems short sighted to me.
   int designatorKw = ccData.getDefinedAttributeKeywordIndex(SCH_ATT_DESIGNATOR, valueTypeString);

   // Component Symbols

   CMapStringToInt componentList;
   DetermineComponentList(ccData, file, componentList);

   double oneMM = ccdb.convertToPageUnits(pageUnitsMilliMeters, 1.0);
   int column = 1;
   bool columnHasSymbol = false;
   bool sheetHasColumn = false;
   double columnStartX = 30.0 * oneMM;
   double columnStartY = 10.0 * oneMM;
   double symX = 0.0; //column * columnStartX;
   double symY = columnStartY;

   BlockStruct *sheetBlk = file->getBlock();

   CDataList columnDataList(true);

   POSITION comppos = componentList.GetStartPosition();
   while (comppos)
   {
      CString refname;
      int pinCount;
      componentList.GetNextAssoc(comppos, refname, pinCount);

      BlockStruct *symbolBlk = GenerateSchematicSymbol(ccdb, file, refname, pinCount);
      CExtent symExt = symbolBlk->getExtent(ccData);


      // If column already has at least one symbol and this one puts it over the top, go to next column.
      if (columnHasSymbol && (symY + symExt.getYsize()) > (250.0 * oneMM))
      {
         sheetHasColumn = PlaceColumnInSheet(ccdb, file, &sheetBlk, columnDataList);

         // Start new column
         column++;
         columnHasSymbol = false;
         symY = columnStartY;
         symX = 0.0; // column * columnStartX;
      }

      // Add this sym to column buffer
      //DataStruct *symbolData = PlaceSymbol(ccdb, sheetBlk, symbolBlk, refname, symX, symY, schematicLayer);
      DataStruct *symbolData = PlaceSymbol(ccdb, columnDataList, symbolBlk, refname, symX, symY, schematicLayer);
      columnHasSymbol = true;

      // Add nets for this sym to column buffer
      GenerateSignalTraces(ccdb, file, columnDataList, symbolData);

      // Move Y up for next symbol, by sym size plus some gap
      symY += symExt.getYsize() + (10.0 * oneMM);
   }


   // Flush final column buffer to sheet
   if (columnDataList.GetCount() > 0)
   {
      PlaceColumnInSheet(ccdb, file, &sheetBlk, columnDataList);
   }


}

//----------------------------------------------------------------------------------

void CSchematicBuilder::DefineSchematicLayers(CCamCadData &ccData)
{
   // Define layers used in schematic graphics/data.

   LayerStruct &lp1 = ccData.getDefinedLayer("SymbolGraphic", false, layerTypeUnknown);
   lp1.setColor(colorMagenta);

   LayerStruct &lp2 = ccData.getDefinedLayer("PortGraphic", false, layerTypeUnknown);
   lp2.setColor(colorRed);

   LayerStruct &lp3 = ccData.getDefinedLayer("SignalGraphic", false, layerTypeUnknown);
   lp3.setColor(colorYellow);
   
}

//----------------------------------------------------------------------------------

void CSchematicBuilder::PurgeSchematicSheets(CCEtoODBDoc *doc, FileStruct *file)
{
   // Get rid of 2nd to nth blocks of type Schematic Sheet.
   // Keep the file's block, but purge its datalist.

   if (doc != NULL && file != NULL)
   {
      int fileNum = file->getFileNumber();
      BlockStruct *fileBlk = file->getBlock();

      for (int i = 0; i < doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *blk = doc->getBlockAt(i);

         if (blk != NULL &&
            blk != fileBlk &&
            blk->getBlockType() == blockTypeSheet &&
            blk->getFileNumber() == fileNum)
         {
            doc->RemoveBlock(blk);
         }
      }

      if (fileBlk != NULL)
      {
         fileBlk->getDataList().empty();
      }


      // This seems like a reasonable thing to do.
      // However, there is a problem when Schematic Sheet blocks are involved.
      // Only the first sheet (the one associated with the file as file's block) will be seen as used.
      // There is nothing that inserts Sheet blocks 2 to n, so they will appear unused.
      // If there is another Schematic file in the current doc, then it's sheets 2 to n will get purged.
      // So what we intended was to get rid of blocks that this file no longer uses, but we'd damage other schematic files.
      ///////doc->purgeUnusedBlocks();
   }

}

//----------------------------------------------------------------------------------

void CSchematicBuilder::GenerateSchematic(CCEtoODBDoc *doc, FileStruct *selectedFile)
{
   if (doc != NULL && selectedFile != NULL)
   {
      // Define layers
      DefineSchematicLayers(doc->getCamCadData());

      // Add compDesignator and pinDesignator attributes in NetList
      GenerateCompDesAndPinDesAttribs(doc->getCamCadData(), selectedFile);

      // Add schematic symbols and traces
      GenerateSchematicGraphics(*doc, doc->getCamCadData(), selectedFile);

      // Stuff that makes doc behave nicely
      doc->UpdateAllViews(NULL); 
      doc->UnselectAll(FALSE);   
      doc->OnFitPageToImage();
   }
}
//----------------------------------------------------------------------------------

FileStruct *CSchematicBuilder::GenerateSchematicDrawing(CCEtoODBDoc *doc, FileStruct *schematicFile, CNetList *sourceNetList)
{
   if (doc != NULL)  
   {
      if (schematicFile == NULL)
         schematicFile = GetNewSchematicFile(doc);

      if (schematicFile != NULL)
      {
         doc->PrepareAddEntity(schematicFile);

         PurgeSchematicSheets(doc, schematicFile);
         
         schematicFile->getNetList().empty();

         //loaderror += LoadNetlist(this, wfp, flog, selectedFile, &netpinarray, netpincnt, false);
         CreateSchematicNetList(doc, schematicFile, sourceNetList);

         // Define layers
         DefineSchematicLayers(doc->getCamCadData());

         // Add compDesignator and pinDesignator attributes in NetList
         GenerateCompDesAndPinDesAttribs(doc->getCamCadData(), schematicFile);

         // Add schematic symbols and traces
         GenerateSchematicGraphics(*doc, doc->getCamCadData(), schematicFile);

         CloseLogFile();  //*rcf This  needs attention

         // Stuff that makes doc behave nicely
         doc->UpdateAllViews(NULL); 
         doc->UnselectAll(FALSE);   
         doc->OnFitPageToImage();
      }
   }

   return schematicFile;
}

//----------------------------------------------------------------------------------

CCEtoODBDoc *CSchematicBuilder::GenerateSchematicDrawing(CString schematicCczFileName, CNetList *sourceNetList)
{
   CCEtoODBDoc *doc = NULL;

   if (sourceNetList != NULL)
   {
      FileStruct *schematicFile = NULL;
      doc = GetNewSchematicCczDoc(schematicCczFileName, &schematicFile);
      GenerateSchematicDrawing(doc, schematicFile, sourceNetList);
   }

   return doc;
}


//----------------------------------------------------------------------------------

CCEtoODBDoc *CSchematicBuilder::GetNewSchematicCczDoc(CString schematicCczFileName, FileStruct **schematicFile)
{
   // Define a new CCEtoODBDoc, create a file of type Sheet, return doc and file ptrs
   ((CCEtoODBApp*)(AfxGetApp()))->OnFileNew(); // OnFileNew();  // New ccz doc file, different than new FileStruct in ccz

   CCEtoODBView *view = getActiveView();
   if (!view)
   {
      // panic
      int jj = 0;
      return NULL;
   }

   CCEtoODBDoc *doc = view->GetDocument();

   doc->SetTitle( schematicCczFileName );
   doc->docTitle = schematicCczFileName;
   doc->CCFileName = schematicCczFileName;
   ///is PRIVATE  doc->fileLoaded = true; // sort of a lie, but needed to trigger the "Do you want to save" upon CAMCAD close

   FileStruct *file = GetNewSchematicFile(doc);

   if (schematicFile != NULL)
      *schematicFile = file;

   return doc;
}

FileStruct *CSchematicBuilder::GetNewSchematicFile(CCEtoODBDoc *doc)
{
   //*rcf THIS could be enhanced to pass specificFileType on down so we can actually set it in Add_File

   FileStruct *file = NULL;

   if (doc != NULL)
   {
      file = doc->Add_File(GetNextNewSheetName(), fileTypeUnknown);  // At this level, file type identifies CAD data source, we have none
      file->setBlockType(blockTypeSheet);
      file->setNotPlacedYet(false);

      // EDIF importers do this, visECAD seems to (foolishly) depend upon it
      int keyword = doc->RegisterKeyWord("SchematicStructure", 0, VT_INTEGER);
      int value = 1;
      doc->SetAttrib(&file->getBlock()->getAttributesRef(), keyword, VT_INTEGER, &value, attributeUpdateOverwrite, NULL);
   }

   return file;
}

//***********************************************************************************
//***********************************************************************************

/***********************************************************************************
* OnSchematicNetlistLoad
*/
void CCEtoODBDoc::OnSchematicNetlistLoad() 
{
   // TODO: Add your command handler code here
   CString  netfilename;
   int      loaderror = 0;

   FileStruct *selectedFile = NULL;

   CSchematicBuilder schb;

   POSITION pos = getFileList().GetHeadPosition();
   int found = 0;
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (file->isShown() &&                         // do not write files which are not shown.
         (file->getBlock() != NULL) &&               // skip empty file
         (file->getBlockType() == BLOCKTYPE_SHEET))  // is schematic sheet
      {
         // Get rid of existing netlist
         if (file->getNetList().GetCount() > 0)                
         {
            file->getNetList().empty();
         }
         found++;

         if (found == 1)  // or could be if selectedFile == NULL, just pick first one either way
            selectedFile = file;
      }
   }

   if (found == 0)
   {
      // No schematic file present, make one
      selectedFile = this->Add_File("Sheet_1", fileTypeUnknown);  // At this level, file type identifies CAD data source, we have none
      selectedFile->setBlockType(blockTypeSheet);
      selectedFile->setNotPlacedYet(false);

      // EDIF importers do this, visECAD seems to (foolishly) depend upon it
      int keyword = this->RegisterKeyWord("SchematicStructure", 0, VT_INTEGER);
      int value = 1;
      this->SetAttrib(&selectedFile->getBlock()->getAttributesRef(), keyword, VT_INTEGER, &value, attributeUpdateOverwrite, NULL);
   }
   else
   if (found > 1)
   {
      ErrorMessage("Multiple Visible Schematic Files found!\nSelect only One File", "Netlist Load");
      return;
   }



   CFileDialog FileDialog(TRUE, "ccn", "*.ccn",
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, 
         "CAMCAD Netlist (*.ccn)|*.ccn|All Files (*.*)|*.*||", NULL);

   if (FileDialog.DoModal() != IDOK) return;

   netfilename = FileDialog.GetPathName();

   // open file for reading
   FILE *wfp;
   if ((wfp = fopen(netfilename, "rt")) == NULL)
   {
     CString tmp;
     tmp.Format("Can not open file [%s]", netfilename);
     ErrorMessage(tmp);
     return;
   }
   
   FILE  *flog;
	CString LogFile = GetLogfilePath("netlist.log");
   if ((flog = fopen(LogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", LogFile);
      ErrorMessage(t, "Error");
   }

   if (selectedFile != NULL)  
   {
      this->PrepareAddEntity(selectedFile);

      schb.PurgeSchematicSheets(this, selectedFile);


      //*rcf Probably don't need this anymore, schematic netlist import does not check against layout comppins
      // first make a netlist, so that I can get all placed pins. I need this to check against
      // .COMPPIN
      CNetPinsArray  netpinarray;      // this is the device - mapping
      int         netpincnt;

      netpinarray.SetSize(100,100);
      netpincnt = 0;

      generate_NCnet(this, selectedFile); // this function generates the NC (non connect) net.

      NetStruct      *net;
      CompPinStruct  *compPin;
      POSITION       compPinPos, netPos;

      netPos = selectedFile->getNetList().GetHeadPosition();
      while (netPos != NULL)
      {
         net = selectedFile->getNetList().GetNext(netPos);
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            compPin = net->getNextCompPin(compPinPos);

            NU_NetlistPins *c = new NU_NetlistPins;
            netpinarray.SetAtGrow(netpincnt,c);  
            netpincnt++;   
            c->compname = compPin->getRefDes();
            c->pinname = compPin->getPinName();
         }
      }

      selectedFile->getNetList().empty();   // redundant, leave for now

      loaderror += LoadNetlist(this, wfp, flog, selectedFile, &netpinarray, netpincnt, false);

      for (int i=0;i<netpincnt;i++)
      {
         delete netpinarray[i];
      }
      netpinarray.RemoveAll();

      //this is a PCB FILE thing:   generate_PINLOC(this,file,0); // this function generates the PINLOC argument for all pins.

      ///break;
   }

   fclose(wfp);


   
   schb.GenerateSchematic(this, selectedFile);


   if (selectedFile != NULL)
   {
      this->UpdateAllViews(NULL); 
      this->UnselectAll(FALSE);   
      this->OnFitPageToImage();
   }

   fclose(flog);

   if (loaderror)
      Notepad(LogFile);

}


/******************************************************************************
* OnSchematicNetlistFromLayout
*/
void CCEtoODBDoc::OnSchematicNetlistFromLayout() 
{
   int      loaderror = 0;

   CNetList *sourceNetList = NULL;

   // Use netlist from first visible PCB file
   POSITION pos = getFileList().GetHeadPosition();
   int found = 0;
   while (pos != NULL && sourceNetList == NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (file->isShown() && (file->getBlockType() == blockTypePcb))
      {
         if (file->getNetList().GetCount() > 0)                
         {
            sourceNetList = &file->getNetList();
         }
      }
   }

   CSchematicBuilder schb;

   if (sourceNetList != NULL)
   {
      CFilePath schFilePath( this->CCFileName );  // Yes, 'this', not doc. We want the source PCB Doc file's name
      CString fileName( schFilePath.getBaseFileName() );
      fileName += "_Schematic";
      schFilePath.setBaseFileName(fileName);
      CString schematicCczFileName( schFilePath.getPath() );

      CCEtoODBDoc *schDoc = schb.GenerateSchematicDrawing(schematicCczFileName, sourceNetList);
      if (schDoc != NULL)
         schDoc->fileLoaded = true; // sort of a lie, but needed to trigger the "Do you want to save" upon CAMCAD close
   }
   else
   {
      CString msg;
      msg += "No PCB Design file currently visible.\n";
      msg += "Schematic creation from Layout requires a currently visible PCB Design file.";
      ErrorMessage(msg);
   }

   if (loaderror)
      Notepad(schb.GetLogFileName());

}
/******************************************************************************
* OnNetlistSave
   syntax simular to bom.cpp *.atr
   .NET  \"netname\"
     .Attach2net attrib from bom.cpp
   .COMPPIN \"Cmp\" \"pin\" 
     .comppinpos x, y, padstack etc...
     .Attach2comppin from bom.cpp
*/
void CCEtoODBDoc::OnNetlistSave() 
{
   CString  netfilename;

   POSITION pos = getFileList().GetHeadPosition();
   int found = 0;
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (!file->isShown())              continue;   // do not write files, which are not shows.
      if (file->getBlock() == NULL)  continue;   // empty file
      POSITION netpos = file->getNetList().GetHeadPosition();
      if (!netpos)                  continue;
      found++;
   }

   if (found == 0)
   {
      ErrorMessage("No Visible File with Netlist found!", "Netlist Save");
      return;
   }
   else
   if (found > 1)
   {
      ErrorMessage("Multiple Visible Files with Netlist found!\nSelect only One File", "Netlist Save");
      return;
   }

   CFileDialog FileDialog(FALSE, "ccn", "*.ccn",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "CAMCAD Netlist (*.ccn)|*.ccn|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   netfilename = FileDialog.GetPathName();

   // open file for writing
   FILE *wfp;
   if ((wfp = fopen(netfilename, "wt")) == NULL)
   {
     CString tmp;
     tmp.Format("Can not open file [%s]", netfilename);
     ErrorMessage(tmp);
     return;
   }

   fprintf(wfp,"; NET listing\n\n");
   fprintf(wfp,"; Created by %s\n\n", getApp().getCamCadTitle());

   pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (!file->isShown())              continue;   // do not write files, which are not shows.
      if (file->getBlock() == NULL)  continue;   // empty file

      POSITION netpos = file->getNetList().GetHeadPosition();
      if (!netpos)                  continue;
      fprintf(wfp,"; FILENAME : %s\n", file->getName());
      fprintf(wfp,".PAGEUNIT : %s\n", GetUnitName(getSettings().getPageUnits()));
      fprintf(wfp,"\n");

      WriteNetlist(wfp, this, &(file->getNetList()));
      break;   
   }

   fprintf(wfp,"; end of net listing.\n");
   fclose(wfp);

   CString  tmp;
   tmp.Format("The Netlist is successfully saved at %s", netfilename);
   ErrorMessage(tmp,"Netlist Save");

   return;
}

/******************************************************************************
* OnNetlistRemove
*/
void CCEtoODBDoc::OnNetlistRemove() 
{
   if (ErrorMessage("Remove can not be Undone, and the undo buffer will be cleared. Prior steps will be cleared and can't be undone.\nIt is recommended to Save the Netlist!\nDo you want to Continue Remove?", "Remove Netlist", MB_YESNO | MB_DEFBUTTON2)==IDYES)
   {
      if (ErrorMessage("Are you sure?", "Remove Netlist", MB_YESNO | MB_DEFBUTTON2)==IDYES)
      {
         BeginWaitCursor();

         POSITION filePos = getFileList().GetHeadPosition();
         while (filePos)
         {
            FileStruct *file = getFileList().GetNext(filePos);
            //FreeNetList(file);
            file->getNetList().empty();
         }
         EndWaitCursor();
      
         if (selectNetsDlg)
            selectNetsDlg->Refresh();

         ErrorMessage("Remove of a netlist leaves Traces, Vias and other Features with Netnames!\nUse Remove Netnames to clear the Net Attributes or Run Repair Netlist.", "Remove Netlist");
      }
   }
}


/*****************************************************************************/
/* 
*/
BOOL RemoveNet(FileStruct *file,  NetStruct *Net)
{
   file->getNetList().deleteNet(Net->getNetName());

   //file->getNetList().RemoveAt(file->getNetList().Find(Net));
   //FreeNet(Net);

   return 1;
}

/******************************************************************************
* ExplodeNcPins
*  - loop through pins and move the pin from the current net to the other
*/
int ExplodeNcPins(CCEtoODBDoc *doc, FileStruct *file, COperationProgress* progress)
{
   return CreateSinglePinNets(doc, file, progress);
}

/******************************************************************************
* MergeNcPins
*  - loop through pins and move the pin from the current net to the other
*/
void MergeNcPins(CCEtoODBDoc *doc, FileStruct *file)
{
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)
      {
         int pinsMoved = 0;

         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos)
         {
            CompPinStruct *compPin = net->getNextCompPin(compPinPos);
            
            if (net->getNetName().Compare(NET_UNUSED_PINS))
            {
               NetStruct *newNet = add_net(file, NET_UNUSED_PINS);
               newNet->setFlagBits(NETFLAG_UNUSEDNET);
               newNet->clearFlagBits(NETFLAG_SINGLEPINNET);
               MoveCompPin(net, newNet, compPin);
               pinsMoved++;
            }
         }

         // delete unused pins net, if pins where moved.
         if (pinsMoved) 
            RemoveNet(file, net);
      }
   }
}

/******************************************************************************
* OnNetlistExplodeNcpins
*  - make singlepin nets
*/
void CCEtoODBDoc::OnNetlistExplodeNcpins() 
{
   BeginWaitCursor();

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      if (!file->getBlock())
         continue;

      if (!file->getNetList().GetCount())
         continue;
      
      CreateSinglePinNets(this, file, NULL);
   }
	
	CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(this);

   EndWaitCursor();
}

/******************************************************************************
* OnNetlistMergeNcpins
*  - merge singlepin nets into NET_UNUSED_PINS
*/
void CCEtoODBDoc::OnNetlistMergeNcpins() 
{
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      if (!file->getBlock())
         continue;

      MergeNcPins(this, file);
   }
}

CString getKey(const long entityNumber, const bool isMirrored)
{
   CString key;
   key.Format("%d%s", entityNumber, isMirrored?"bot":"top");

   return key;
}

int CreateSinglePinNets(CCEtoODBDoc* doc, FileStruct* file, COperationProgress* progress)
{
	long maxRange = 0;
   int totalCompPinMoved = 0;
   int netNameKw = doc->getStandardAttributeKeywordIndex(standardAttributeNetName);
   int dataLinkKw = doc->getStandardAttributeKeywordIndex(standardAttributeDataLink);
   double tolerance = Units_Factor(pageUnitsMils, doc->getSettings().getPageUnits());

   CCamCadDatabase camcadDatabase(*doc);
   CNonConnectedPinList nonConnectedPinList(true);
   CNonConnectedPolyList nonConnectedPolyList(true);
   CNonConnectedViaList nonConnectedViaList(true);
   CTypedMapStringToPtrContainer<DataStruct*> nonConnectedTestProbeMap(nextPrime2n(20), false);
   CTypedMapStringToPtrContainer<DataStruct*> nonConnectedTestAccessMap(nextPrime2n(20), false);

   // Collect all non connected comppins
	if (progress != NULL)
	{
		maxRange = file->getNetCount();
		progress->setLength(maxRange);
	}

   for (POSITION netPos=file->getHeadNetPosition(); netPos != NULL;)
   {
		if (progress != NULL) progress->incrementProgress();

      NetStruct* net = file->getNextNet(netPos);
      if (!(net->getFlags() & NETFLAG_UNUSEDNET))
         continue;

      for (POSITION compPinPos=net->getHeadCompPinPosition(); compPinPos != NULL;)
      {
         POSITION curCompPinPos = compPinPos;
         CompPinStruct* compPin = net->getNextCompPin(compPinPos);
         net->getCompPinList().removeAt(curCompPinPos);

         CNonConnectedPin* nonConnectedPin = new CNonConnectedPin(camcadDatabase, compPin);
         nonConnectedPinList.AddTail(nonConnectedPin);
         totalCompPinMoved++;
      }

      if (net->getCompPinCount() == 0)
      {
         RemoveNet(file, net);
      }
   }

   // Collect all non connected vias
	if (progress != NULL)
	{
		maxRange = file->getBlock()->getDataCount();
	}

	for (CDataListIterator dataList(*file->getBlock(), dataTypeInsert); dataList.hasNext();)
   {
		if (progress != NULL)
         progress->incrementProgress();

      DataStruct* data = dataList.getNext();
      Attrib* attrib = NULL;
      CString netname;
      camcadDatabase.getAttributeStringValue(netname, &data->getAttributesRef(), netNameKw);

      if (data->getInsert()->getInsertType() == insertTypeVia && netname.IsEmpty())
      {
         // Only care about via that does not have a netname or an empty netname
         CNonConnectedVia* nonConnectedVia = new CNonConnectedVia(camcadDatabase, data);
         nonConnectedViaList.AddTail(nonConnectedVia);
      }
      else if (data->getAttributesRef() != NULL && data->getAttributesRef()->Lookup(dataLinkKw, attrib))
      {
         DataStruct* testProbeData = NULL;
         DataStruct* testAccessData = NULL;

         // Get entity pointed to by DataLink
         CEntity entity = CEntity::findEntity(doc->getCamCadData(), attrib->getIntValue());

         if(data->getInsert()->getInsertType() == insertTypeTestProbe)
         {
            // Get Test Access pointed by DataLink
            testProbeData = data;
            testAccessData = entity.getData();
         }
         else if (data->getInsert()->getInsertType() == insertTypeTestAccessPoint)
         {
            testAccessData = data;
         }

         if (testAccessData != NULL && testAccessData->getAttributesRef() != NULL && testAccessData->getAttributesRef()->Lookup(dataLinkKw, attrib))
         {
            // The key is made up of the entity number of the target(via, comppin) and the side of which test access and probe are on
            CString key = getKey(attrib->getIntValue(), testAccessData->getInsert()->getGraphicMirrored());
            
            nonConnectedTestAccessMap.setAt(key, testAccessData);

            if (testProbeData != NULL)
               nonConnectedTestProbeMap.setAt(key, testProbeData);
         }
      }
   }

   // Collect all non connected polys
	if (progress != NULL)
	{
		maxRange = file->getBlock()->getDataCount();
		progress->setLength(maxRange);
	}

   int netnameKw = camcadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeNetName);
	for (CDataListIterator dataList(*file->getBlock(), dataTypePoly); dataList.hasNext();)
   {
		if (progress != NULL) progress->incrementProgress();

      DataStruct* polyData = dataList.getNext();
      CString netname;
      camcadDatabase.getAttributeStringValue(netname, &polyData->getAttributesRef(), netnameKw);

      if (polyData->getGraphicClass() == graphicClassNormal || (polyData->getGraphicClass() == graphicClassEtch && netname.IsEmpty()))
      {
         LayerStruct* layer = camcadDatabase.getLayerAt(polyData->getLayerIndex());
         if (layer == NULL || (!layer->isElectricalBottom() && !layer->isElectricalInner() && !layer->isElectricalTop()))
            continue;

         CNonConnectedPoly* nonConnectedPoly = new CNonConnectedPoly(camcadDatabase, polyData);
         nonConnectedPolyList.AddTail(nonConnectedPoly);

         CPolyList* polyList = polyData->getPolyList();

         // Check to see if what pins is connected to the poly
         for(POSITION pinPos=nonConnectedPinList.GetHeadPosition(); pinPos != NULL;)
         {
            CNonConnectedPin* nonConnectedPin = nonConnectedPinList.GetNext(pinPos);
            if (nonConnectedPin == NULL)
               continue;

            // Check to see if poly is cross a pin
            if (polyList->isPointOnSegment(nonConnectedPin->getOrigin(),doc->getPageUnits(), tolerance))
            {
               // Check to poly is on a layer in the padstack of pin
               if (!nonConnectedPin->isLayerInPadstack(polyData->getLayerIndex()))
                  continue;

               nonConnectedPoly->addNonConnectedPin(nonConnectedPin);
               nonConnectedPin->addNonConnectedPoly(nonConnectedPoly);
            }
         }

         // Check to see if what vias is connected to the poly
         for (POSITION viaPos=nonConnectedViaList.GetHeadPosition(); viaPos != NULL;)
         {
            CNonConnectedVia* nonConnectedVia = nonConnectedViaList.GetNext(viaPos);
            if (nonConnectedVia == NULL)
               continue;

            // Check to see if poly is cross a via
            if (polyList->isPointOnSegment(nonConnectedVia->getOrigin(),doc->getPageUnits(), tolerance))
            {
               // Check to poly is on a layer in the padstack of via
               if (!nonConnectedVia->isLayerInPadstack(polyData->getLayerIndex()))
                  continue;

               nonConnectedPoly->addNonConnectedVia(nonConnectedVia);
               nonConnectedVia->addNonConnectedPoly(nonConnectedPoly);
            }
         }
      }
   }

   // Create single pin net and apply to polys and vias
	if (progress != NULL)
	{
		maxRange = nonConnectedPinList.GetCount();
		progress->setLength(maxRange);
	}

	for(POSITION pinPos=nonConnectedPinList.GetHeadPosition(); pinPos != NULL;)
   {
		if (progress != NULL) progress->incrementProgress();

      CNonConnectedPin* nonConnectedPin = nonConnectedPinList.GetNext(pinPos);
      if (nonConnectedPin == NULL || nonConnectedPin->isDone())
         continue;

      CString netName = GenerateSinglePinNetname(nonConnectedPin->getCompName(), nonConnectedPin->getPinName());
      NetStruct *nonConnectedNet = nonConnectedPin->setNetName(netName, file);
      if(nonConnectedNet) 
         nonConnectedNet->setFlagBits(NETFLAG_UNUSEDNET | NETFLAG_SINGLEPINNET);

      DataStruct* data = NULL;
      CString topKey = getKey(nonConnectedPin->getEntityNumber(), false);
      CString bottomKey = getKey(nonConnectedPin->getEntityNumber(), true);

      // Update netname of test access
      if (nonConnectedTestAccessMap.Lookup(topKey, data))
         data->setAttrib(doc->getCamCadData(), netNameKw, valueTypeString, netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
      if (nonConnectedTestAccessMap.Lookup(bottomKey, data))
         data->setAttrib(doc->getCamCadData(), netNameKw, valueTypeString, netName.GetBuffer(0), attributeUpdateOverwrite, NULL);

      // Update netname of test probe
      if (nonConnectedTestProbeMap.Lookup(topKey, data))
         data->setAttrib(doc->getCamCadData(), netNameKw, valueTypeString, netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
      if (nonConnectedTestProbeMap.Lookup(bottomKey, data))
         data->setAttrib(doc->getCamCadData(), netNameKw, valueTypeString, netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
   }

   return totalCompPinMoved;
}

void getPadstackLayerIndexArray(CCamCadDatabase& camcadDatabase, BlockStruct* padstack, bool isInsertMirrored, CArray<int, int>& layerIndexArray)
{
   if (padstack == NULL)
       return;

   layerIndexArray.RemoveAll();
   layerIndexArray.SetSize(0, 10);

   for (CDataListIterator dataList(*padstack, dataTypeInsert); dataList.hasNext();)
   {  
      DataStruct* padData = dataList.getNext();      
      int layerIndex = padData->getLayerIndex();

      if (layerIndex > -1)
      {
         if (isInsertMirrored ^ padData->getInsert()->getGraphicMirrored())
            layerIndex = camcadDatabase.getLayerAt(layerIndex)->getMirroredLayerIndex();

         if (layerIndex > -1)
            layerIndexArray.SetAtGrow(layerIndexArray.GetCount(), layerIndex);
      }
   }
}

bool isLayerInPadstackLayerIndexArray(CArray<int, int>& padstackLayerIndexArray, int layerIndexToCheck)
{
   for (int i=0; i<padstackLayerIndexArray.GetCount(); i++)
   {
      int layerIndex = padstackLayerIndexArray.GetAt(i);
      if (layerIndexToCheck == layerIndex)
         return true;
   }

   return false;
}

//-----------------------------------------------------------------------------
// CNonConnectedPin
//-----------------------------------------------------------------------------
CNonConnectedPin::CNonConnectedPin(CCamCadDatabase& camcadDatabase, CompPinStruct* compPin)
   : m_camcadDatabase(camcadDatabase)
   , m_polyList(false)
{
   m_compPin = compPin;
   m_netName.Empty();
   m_padstackLayerIndexArray.SetSize(0, 10);

   if (compPin != NULL)
   {
      getPadstackLayerIndexArray(m_camcadDatabase, m_camcadDatabase.getBlock(m_compPin->getPadstackBlockNumber()), m_compPin->getMirror()?true:false, m_padstackLayerIndexArray);
   }
}

CNonConnectedPin::~CNonConnectedPin()
{
   m_compPin = NULL;
   m_polyList.empty();
   m_padstackLayerIndexArray.RemoveAll();
}

bool CNonConnectedPin::isDone() const
{
   return m_netName.IsEmpty()?false:true;
}

bool CNonConnectedPin::isLayerInPadstack(int layerIndex)
{
   return isLayerInPadstackLayerIndexArray(m_padstackLayerIndexArray, layerIndex);
}

NetStruct* CNonConnectedPin::setNetName(const CString netName, FileStruct* pcbFile)
{
   if (netName.IsEmpty() || isDone())
      return NULL;

   m_netName = netName;
   NetStruct* net = m_camcadDatabase.getDefinedNet(netName, pcbFile);
   net->getCompPinList().addTail(m_compPin);

   // Go through all connected poly and set netname
   for (POSITION pos=m_polyList.GetHeadPosition(); pos != NULL;)
   {
      CNonConnectedPoly* nonConnectedPoly = m_polyList.GetNext(pos);
      if (nonConnectedPoly == NULL)
         continue;

      nonConnectedPoly->setNetName(netName, pcbFile);
   }

   return net;
}

void CNonConnectedPin::addNonConnectedPoly(CNonConnectedPoly* nonConnectedPoly) 
{ 
   if (!m_polyList.Find(nonConnectedPoly))
      m_polyList.AddTail(nonConnectedPoly); 
}

//-----------------------------------------------------------------------------
// CNonConnectVia
//-----------------------------------------------------------------------------
CNonConnectedVia::CNonConnectedVia(CCamCadDatabase& camcadDatabase, DataStruct* viaData)
   : m_camcadDatabase(camcadDatabase)
   , m_polyList(false)
{
   m_netName.Empty();
   m_viaData = viaData;
   m_padstackLayerIndexArray.SetSize(0, 10);

   if (m_viaData != NULL)
   {
      getPadstackLayerIndexArray(m_camcadDatabase, m_camcadDatabase.getBlock(m_viaData->getInsert()->getBlockNumber()), m_viaData->getInsert()->getGraphicMirrored()?true:false, m_padstackLayerIndexArray);
   }
}

CNonConnectedVia::~CNonConnectedVia()
{
   m_viaData = NULL;
   m_polyList.empty();
   m_padstackLayerIndexArray.RemoveAll();
}

bool CNonConnectedVia::isDone() const
{
   return m_netName.IsEmpty()?false:true;
}

bool CNonConnectedVia::isLayerInPadstack(int layerIndex)
{
   return isLayerInPadstackLayerIndexArray(m_padstackLayerIndexArray, layerIndex);
}

void CNonConnectedVia::setNetName(const CString netName, FileStruct* pcbFile)
{
   if (netName.IsEmpty() || isDone())
      return;
  
   m_netName = netName;
   int netNameKw =  m_camcadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeNetName);
   m_viaData->setAttrib(m_camcadDatabase.getCamCadData(), netNameKw, valueTypeString, m_netName.GetBuffer(0), attributeUpdateOverwrite, NULL);

   // Go through all connected poly and set netname
   for (POSITION pos=m_polyList.GetHeadPosition(); pos != NULL;)
   {
      CNonConnectedPoly* nonConnectedPoly = m_polyList.GetNext(pos);
      if (nonConnectedPoly == NULL)
         continue;

      nonConnectedPoly->setNetName(netName, pcbFile);
   }
}

void CNonConnectedVia::addNonConnectedPoly(CNonConnectedPoly* nonConnectedPoly) 
{ 
   if (!m_polyList.Find(nonConnectedPoly))
      m_polyList.AddTail(nonConnectedPoly); 
}

//-----------------------------------------------------------------------------
// CNonConnectedPoly
//-----------------------------------------------------------------------------
CNonConnectedPoly::CNonConnectedPoly(CCamCadDatabase& camcadDatabase, DataStruct* polyData)
   : m_camcadDatabase(camcadDatabase)
   , m_pinList(false)
   , m_viaList(false)
{
   m_netName.Empty();
   m_polyData = polyData;
}


CNonConnectedPoly::~CNonConnectedPoly()
{
   m_polyData = NULL;
   m_pinList.empty();
   m_viaList.empty();
}

bool CNonConnectedPoly::isDone() const
{
   return m_netName.IsEmpty()?false:true;
}

void CNonConnectedPoly::setNetName(const CString netName, FileStruct* pcbFile)
{
   if (netName.IsEmpty() || isDone())
      return;
      
   m_netName = netName;
   int netNameKw =  m_camcadDatabase.getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeNetName);
   m_polyData->setAttrib(m_camcadDatabase.getCamCadData(), netNameKw, valueTypeString, m_netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
   m_polyData->setGraphicClass(graphicClassEtch);

   // Go through all connected poly and set netname
   for (POSITION pos=m_pinList.GetHeadPosition(); pos != NULL;)
   {
      CNonConnectedPin* nonConnectedPin = m_pinList.GetNext(pos);
      if (nonConnectedPin == NULL)
         continue;

      nonConnectedPin->setNetName(netName, pcbFile);
   }

   for (POSITION pos=m_viaList.GetHeadPosition(); pos != NULL;)
   {
      CNonConnectedVia* nonConnectedVia = m_viaList.GetNext(pos);
      if (nonConnectedVia == NULL)
         continue;

      nonConnectedVia->setNetName(netName, pcbFile);
   }
}

