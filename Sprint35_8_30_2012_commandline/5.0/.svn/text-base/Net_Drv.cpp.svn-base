
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "floodfil.h"
#include "attrib.h"
#include "pcbutil.h"
#include "gauge.h"
#include "crypt.h"
#include "net_drv.h"
#include "drc.h"
#include "math.h"
#include "net_util.h"
#include "xform.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum NetItemType
{
   nettype_pin = 1,
   nettype_via,
   nettype_etch,
   nettype_border,
};                

struct CNetlistStruct
{
   CAttributes** map;
   double x, y;
   int netIndex;
   enum NetItemType type;
   CompPinStruct *comppin;
   CString netName;
   CByteArray layermap;
};

static int DeriveNetlist(CCEtoODBDoc *doc, FileStruct *file, FileStruct *augmentFile);
static int Prep(CCEtoODBDoc *doc);
static void BuildFileLayerArray(CCEtoODBDoc *doc, BlockStruct *block, CByteArray *fileLayerArray);
static BOOL DrawThisLayer(LayerStruct *layer, int stackNum, CByteArray *FileLayerArray);
static void FillItemList(CCEtoODBDoc *doc, FileStruct *file, BOOL AugmentOnly);
static void FillPadstackLayerMap(CCEtoODBDoc *doc, CDataList *datalist, CNetlistStruct *padstack, int mirror);
static int GetPadstackCount(CCEtoODBDoc *doc, BlockStruct *block);
static void MakeOldNetnames(FileStruct *file);

#define  GERBERPCBLogFile        "gpcb.log"

BOOL bmpBW;
BOOL algRecursive;
BOOL bmpStart; 
BOOL bmpFills;

static int algIndexShorts, algIndexOpens;
static CProgressDlg *progress;
static CString logFile;
static CStdioFile stream;
static double resolution;
static BOOL allowNetsWitoutPadstacks;
static BOOL allowSinglePinNets;
static BOOL RenameUnconnectedNets;
static int maxStackupNum;
static CPtrList *itemList; 
static CPtrArray *connectedNetArray;
static WORD netnameKW, oldNetnameKW;
static int decimals;

/****************************************************************** 
* OnDeriveNetlist
*/
void CCEtoODBDoc::OnDeriveNetlist() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   // one file
   FileStruct *file = NULL;
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *fp = getFileList().GetNext(filePos);
      if (!fp->isShown())
         continue;

      if (!file)
         file = fp;
      else
      {
         ErrorMessage("Only one file can be on");
         return;
      }
   }

   if (!file)
   {
      ErrorMessage("No File");
      return;
   }

   algIndexShorts = GetAlgorithmNameIndex(this, "Net Derive - Shorts");
   algIndexOpens = GetAlgorithmNameIndex(this, "Net Derive - Opens");

   allowNetsWitoutPadstacks = FALSE;
   allowSinglePinNets = FALSE;
   RenameUnconnectedNets = FALSE;

   if (DeriveNetlist(this, file, NULL))
   {
      file->setBlockType(blockTypePcb);
   }
   else
   {
      // failed.
   }

   return;
}

/*****************************************************************************/
/*
   Make sure that all SignalBottom and PadBottom match on stackup

   #define  LAYTYPE_SIGNAL_BOT            2  // solder layer
   #define  LAYTYPE_PAD_BOTTOM            7  // no param

*/
static int CheckLayerStackup(CCEtoODBDoc *doc)
{
   int   j;
   int   highest_stacknum = -1;

   // initialize highest_stacknum
   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.
      if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT || layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         highest_stacknum = layer->getElectricalStackNumber();
         break;
      }
   }
   
   // now make sure all PAD_BOTTOM and SIGNAL_BOT has the same
   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.
      if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT || layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         if (highest_stacknum != layer->getElectricalStackNumber())
            return 1;
      }
   }

   return 0;   // return good
}

/******************************************************************************
* OnGerberCADNetlistCompare
*/
void CCEtoODBDoc::OnGerberCADNetlistCompare() 
{
   FileStruct *gerberFile = NULL;
   FileStruct *cadFile = NULL;
   
   // find gerberFile
   POSITION pos = this->getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = this->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getCadSourceFormat() == Type_Gerber || file->getCadSourceFormat() == Type_Barco)
      {
         if (!gerberFile)
            gerberFile = file;
         else
         {
            ErrorMessage("Multiple visible Gerber or Barco files found!", "Gerber Netlist Compare");
            return;
         }
      }
   }

   if (!gerberFile)
   {
      ErrorMessage("No visible Gerber or Barco file found!", "Gerber Netlist Compare");
      return;
   }


   // find cadFile
   pos = this->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = this->getFileList().GetNext(pos);
      if (!file->isShown())        continue;
      if (file == gerberFile) continue;

      if (file->getNetList().GetHeadPosition())
      {
         if (!cadFile)
            cadFile = file;
         else
         {
            ErrorMessage("Multiple visible CAD files with Netlist found!", "Gerber Netlist Compare");
            return;
         }
      }
   }

   if (!cadFile)
   {
      ErrorMessage("No visible CAD file with Netlist found!", "Gerber Netlist Compare");
      return;
   }

   if (CheckLayerStackup(this))
   {
      ErrorMessage("The Layerstackup between Gerber and CAD file does not match!", "Gerber Netlist Compare");
      return;
   }

   RestructureFile(this, gerberFile);
   RestructureFile(this, cadFile);


   algIndexShorts = GetAlgorithmNameIndex(this, "Gerber-CAD Net Compare - Shorts");
   algIndexOpens = GetAlgorithmNameIndex(this, "Gerber-CAD Net Compare - Opens");


   // clear old DRCs
   POSITION drcPos = gerberFile->getDRCList().GetHeadPosition();
   while (drcPos)
   {
      DRCStruct *drc = gerberFile->getDRCList().GetNext(drcPos);
      if (drc->getAlgorithmIndex() == algIndexShorts || drc->getAlgorithmIndex() == algIndexOpens)
         RemoveOneDRC(this, drc, gerberFile);
   }
   
   
   // clear gerber netnames
   POSITION dataPos = gerberFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = gerberFile->getBlock()->getDataList().GetNext(dataPos);

      if (data->getAttributesRef())
      {
         Attrib* attrib;

         if (data->getAttributesRef()->Lookup(oldNetnameKW, attrib))
         {
            data->getAttributesRef()->deleteAttribute(oldNetnameKW);
            //delete (Attrib*)voidPtr;
         }

         if (data->getAttributesRef()->Lookup(netnameKW, attrib))
         {
            data->getAttributesRef()->deleteAttribute(netnameKW);
            //delete (Attrib*)voidPtr;
         }           
      }
   }

   // remove all gerber nets
   //FreeNetList(gerberFile);
   gerberFile->getNetList().empty();

   allowNetsWitoutPadstacks = FALSE;
   allowSinglePinNets = TRUE;
   RenameUnconnectedNets = FALSE;

   if (DeriveNetlist(this, gerberFile, cadFile))
   {
      gerberFile->setBlockType(blockTypePcb);
   }
}

/******************************************************************************
* DeriveNetlist
*/
int DeriveNetlist(CCEtoODBDoc *doc, FileStruct *file, FileStruct *augmentFile)
{
   CWaitCursor wait;

   if (!Prep(doc))
      return 0;

   // check if padstacks are defined.
   int padstackCount = GetPadstackCount(doc, file->getBlock());
   if (augmentFile)
      padstackCount += GetPadstackCount(doc, augmentFile->getBlock());
   if (!padstackCount && !allowNetsWitoutPadstacks)
   {
      progress->DestroyWindow();
      delete progress;
      stream.Close();

      ErrorMessage("No Padstacks were generated in the design.", "Error: Netlist Derive");
      return 0;   
   }

   generate_PINLOC(doc, file, 0);
   if (augmentFile)
      generate_PINLOC(doc, augmentFile, 0);
   remove_zero_size_pads(doc);


   // init data structures
   itemList = new CPtrList;
   connectedNetArray = new CPtrArray;
   connectedNetArray->SetSize(100, 100);
   int netIndex = 1;
   CByteArray fileLayerArray;


   // build array of layers used in this file
   fileLayerArray.SetSize(doc->getMaxLayerIndex());
	int i=0;
   for (i=0; i<doc->getMaxLayerIndex(); i++)
      fileLayerArray[i] = 0;
   BuildFileLayerArray(doc, file->getBlock(), &fileLayerArray);

   
   // get maxStackupNum
   maxStackupNum = GetMaxStackupNum(doc);
   if (!maxStackupNum)
   {
      progress->DestroyWindow();
      delete progress;
      stream.Close();

      ErrorMessage("No Electrical Layer Stackup", "Error: Netlist Derive");
      return 0;   
   }


   // move NETNAME to OldNetname
   MakeOldNetnames(file);


   // put traces and vias and pins in itemList
   FillItemList(doc, file, FALSE);
   if (augmentFile)
      FillItemList(doc, augmentFile, TRUE);


   BOOL WrittenOnce = FALSE;
   BOOL AlgorithmB = FALSE;

   // do each stackup number
   for (int stackNum=1; stackNum<=maxStackupNum; stackNum++)
   {
      CString banner;
      banner.Format("Finding Connectivity on Layer Stackup #%d...", stackNum);
      progress->SetStatus(banner);

      switch (FF_Start(doc, resolution))
      {
         case 1: // compatible DC (color)
         break;

         case 0: // failure
            progress->DestroyWindow();
            delete progress;
            stream.Close();
         return 0;

         case -1: // black & white
            AlgorithmB = TRUE;
            //stream.WriteString("Using algorithm B.\n\n");
         break;
      }
         
      // get plane netnames
      CStringList layerNetnamesList;
		int l=0;
      for (l=0; l<doc->getMaxLayerIndex(); l++)
      {
         LayerStruct *layer = doc->getLayerArray()[l];
         if (layer == NULL || !DrawThisLayer(layer, stackNum, &fileLayerArray))
            continue;

         Attrib *layerAttrib;

         if (layer->getAttributesRef() && layer->getAttributesRef()->Lookup(netnameKW, layerAttrib))
         {
            char *buf = STRDUP(layerAttrib->getStringValue());
            char *tok = strtok(buf, "\n");

            while (tok)
            {
               layerNetnamesList.AddTail(tok);
               tok = strtok(NULL, "\n");
            }

            free(buf);
         }
      }

      BOOL NegativeLayer = FALSE;
      // draw appropriate layers
      for (l=0; l<doc->getMaxLayerIndex(); l++)
      {
         LayerStruct *layer = doc->getLayerArray()[l];

         if (layer == NULL || !DrawThisLayer(layer, stackNum, &fileLayerArray))
            continue;

         FF_Draw(file, l, &layerNetnamesList);
         if (layer->getLayerType() == LAYTYPE_POWERNEG || layer->getLayerType() == LAYTYPE_SPLITPLANE)
            NegativeLayer = TRUE;
      }

      if (bmpStart) 
      {
         CString buf;
         buf.Format("Stack # %d", stackNum);
         FF_ShowBitmap(buf);
      }

      POSITION pos = itemList->GetHeadPosition();
      while (pos != NULL)
      {
         CNetlistStruct *n = (CNetlistStruct*)itemList->GetNext(pos);
         if (n->type == nettype_border)
            continue;

         if (n->layermap[stackNum]) // on this stackup
         {
            // figure which list for this item
            CPtrList *list;
            if (n->netIndex)
            {
               list = (CPtrList*)connectedNetArray->GetAt(n->netIndex);
            }
            else
            {
               n->netIndex = netIndex++;
               list = new CPtrList;
               connectedNetArray->SetAtGrow(n->netIndex, list);
            }
            list->AddTail(n);

            n->layermap[stackNum] = FALSE; // so we don't check this item again so we don't have to call FF_Refresh()


            // flood starting at this point
            int res = FF_Fill(n->x, n->y, NegativeLayer);
            switch (res)
            {
            case 0:
               ErrorMessage("Failure");
               continue;

            case -1:
               continue;
            }

            if (bmpFills)
            {
               CString buf;
               buf.Format("Stack # %d, Fill ( %.*lf , %.*lf )", stackNum, decimals, n->x, decimals, n->y);
               FF_ShowBitmap(buf);
            }

            POSITION pos2 = pos;
            while (pos2 != NULL)
            {
               CNetlistStruct *n2 = (CNetlistStruct*)itemList->GetNext(pos2);
               if (n2->layermap[stackNum])
               {
                  if (FF_Query(n2->x, n2->y))
                  {
                     n2->layermap[stackNum] = FALSE; // so we don't check this item again so we don't have to call FF_Refresh()
                     if (!n2->netIndex) // if it didn't have a net, add it to this net
                     {
                        n2->netIndex = n->netIndex;
                        list->AddTail(n2);
                     }
                     else if (n->netIndex != n2->netIndex) // if it had a different net, combine them
                     {
                        CPtrList *list2 = (CPtrList*)connectedNetArray->GetAt(n2->netIndex);
                        while (list2->GetCount())
                        {
                           CNetlistStruct *temp = (CNetlistStruct*)list2->RemoveHead();
                           temp->netIndex = n->netIndex;
                           list->AddTail(temp);
                        }
                     }
                  }
               }
            }
            //FF_Refresh();

            // DRC - Check if netnames collide - first chance error message to help debug collisions
            if (!WrittenOnce)
            {
               CString name = "";
               BOOL Collision = FALSE;
               CNetlistStruct *temp;
               POSITION testPos = list->GetHeadPosition();
               while (testPos)
               {
                  CNetlistStruct *item = (CNetlistStruct*)list->GetNext(testPos);

                  // remember netname and check for collision
                  if (!item->netName.IsEmpty())
                  {
                     if (name.IsEmpty())
                        name = item->netName;
                     else if (name.Compare(item->netName))
                     {
                        Collision = TRUE;
                        temp = item;
                        break;
                     }
                  }
               }
               if (Collision)
               {
                  // put all unique netnames in stringlist
                  CStringList *stringList = new CStringList;

                  POSITION pos = list->GetHeadPosition();
                  while (pos)
                  {
                     CNetlistStruct *item = (CNetlistStruct*)list->GetNext(pos);

                     if (!item->netName.IsEmpty())
                        if (!stringList->Find(item->netName))
                           stringList->AddTail(item->netName);
                  }

                  CString collisionString = "First net collision between ";

                  pos = stringList->GetHeadPosition();
                  while (pos)
                  {
                     CString string = stringList->GetNext(pos);
                     collisionString += string;
                     if (pos)
                        collisionString += " & ";
                  }
                  CString buf;
                  buf.Format(" on stackup #%d", stackNum);
                  collisionString += buf;
                  stream.WriteString(collisionString);
                  stream.WriteString("\n\n");
                  WrittenOnce = TRUE;

                  DRCStruct *drc = CreateDRC(file, collisionString, DRC_CLASS_NETS, 0, algIndexShorts, DFT_ALG_NET_COLLISION);
                  drc->setVoidPtr(stringList);
               }
            } 
         }
      }

      FF_End();
   }

   if (AlgorithmB)
      stream.WriteString("Using algorithm B.\n\n");
   
   // ASSIGN NETNAMES
   progress->SetStatus("Assigning Netnames...");

   // get nonet from netlist
   NetStruct *nonet = NULL;
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *temp = file->getNetList().GetNext(netPos);
      if (temp->getFlags() & NETFLAG_NONET)
      {
         nonet = temp;
         break;
      }
   }

   CStringList usedNetNames, warnedNetNames;
   int digits = 1;
   while (netIndex / round(pow(10.0, digits)))
      digits++;

   for (i=1; i<netIndex; i++)
   {
      CPtrList *list = (CPtrList*)connectedNetArray->GetAt(i);
      if (list->IsEmpty())
      {
         delete list;
         continue;
      }

      CString netname = "";
      BOOL Collision = FALSE;

      int padstackCount = 0, etchCount = 0;

      POSITION pos = list->GetHeadPosition();
      while (pos)
      {
         CNetlistStruct *n = (CNetlistStruct*)list->GetNext(pos);

         switch (n->type)
         {
            case nettype_pin:
            case nettype_via:
               padstackCount++;
               break;
            case nettype_etch:
               etchCount++;
               break;
         }

         if (!n->netName.IsEmpty())
         {       
            if (netname.IsEmpty())
               netname = n->netName;
            else if (netname.Compare(n->netName))
               Collision = TRUE;
         }
      }

      // skip nets without pads (ex. Gerber text)
      if (!allowNetsWitoutPadstacks && !padstackCount)
      {
         delete list;
         continue;
      }

      // skip single pad nets
      if (!allowSinglePinNets && !etchCount && netname.IsEmpty()) // if no netname, then skip; but if netname, then continue to check for unconnected pin
      {
         delete list;
         continue;
      }

      NetStruct *net = NULL;
      if (netname.IsEmpty() || Collision)
         netname.Format("$ DERIVE_%0*d $", digits, i);


      // check for unconnected nets with same netname
      BOOL OverrideNetname = FALSE;
      if (usedNetNames.Find(netname))
      {
         if (RenameUnconnectedNets)
         {
            CString firstNetname = netname;
            int i = 1;
            while (usedNetNames.Find(netname))
               netname.Format("%s_%d", firstNetname, i++);
            usedNetNames.AddTail(netname);
            OverrideNetname = TRUE;
            if (!warnedNetNames.Find(netname))
            {
               CString buf;
               buf.Format("Unconnected nets with same netname \"%s\".  Adding net \"%s\".", firstNetname, netname);

               DRCStruct *drc = CreateDRC(file, buf, DRC_CLASS_NETS, 2, algIndexOpens, DFT_ALG_NET_COLLISION);
               CStringList *stringList = new CStringList;
               drc->setVoidPtr(stringList);
               stringList->AddTail(firstNetname);
               stringList->AddTail(netname);

               buf += "\n";
               stream.WriteString(buf);
               warnedNetNames.AddTail(netname);
            }
         }
         else
         {
            if (!warnedNetNames.Find(netname))
            {
               CString buf;
               buf.Format("Unconnected nets with same netname \"%s\".", netname);

               DRCStruct *drc = CreateDRC(file, buf, DRC_CLASS_NETS, 2, algIndexOpens, DFT_ALG_NET_COLLISION);
               CStringList *stringList = new CStringList;
               drc->setVoidPtr(stringList);
               stringList->AddTail(netname);

               buf += "\n";
               stream.WriteString(buf);
               warnedNetNames.AddTail(netname);
            }
         }
      }
      else
         usedNetNames.AddTail(netname);

      net = add_net(file, netname);

      CString collisionString;
      if (Collision)
      {
         CString buf;
         buf.Format("Net Collision in %s between ", netname);
         collisionString = buf;
         stream.WriteString(buf);
      }

      CStringList *stringList;
      if (Collision)
         stringList = new CStringList;
      pos = list->GetHeadPosition();
      while (pos)
      {
         CNetlistStruct *n = (CNetlistStruct*)list->GetNext(pos);

         if (Collision && !n->netName.IsEmpty())
         {
            if (!stringList->Find(n->netName))
               stringList->AddTail(n->netName);

            CString buf;
            stream.WriteString("\n\t");
            buf.Format("%s (%+.4lg, %+.4lg), ", n->netName, n->x, n->y);
            stream.WriteString(buf);
         }

         if (n->comppin && n->netName.IsEmpty() && nonet) // move comppin from nonet to this net
         {
            POSITION cpPos = nonet->getCompPinList().find(n->comppin);
            if (cpPos)
               nonet->getCompPinList().removeAt(cpPos);
            net->getCompPinList().addTail(n->comppin);
         }

         if (!n->comppin)
         {
            CString temp = netname;
            if (!n->netName.IsEmpty() && !OverrideNetname)
               temp = n->netName;

            doc->SetAttrib(n->map, netnameKW, VT_STRING, temp.GetBuffer(0), SA_APPEND, NULL);
         }
      }

      if (Collision)
      {
         stream.WriteString("\n");

         pos = stringList->GetHeadPosition();
         while (pos)
         {
            CString string = stringList->GetNext(pos);
            collisionString += string;
            if (pos)
               collisionString += " & ";
         }

         stringList->AddTail(netname);

         DRCStruct *drc = CreateDRC(file, collisionString, DRC_CLASS_NETS, 0, algIndexShorts, DFT_ALG_NET_COLLISION);
         drc->setVoidPtr(stringList);
      }

      delete list;
   }


   // remove nonet
   if (nonet)
   {
      file->getNetList().deleteNet(nonet->getNetName());

      //if (nonet->getCompPinList().IsEmpty())
      //{
      //   file->getNetList().RemoveAt(file->getNetList().Find(nonet));
      //   if (nonet->getAttributesRef())
      //   {
      //      delete nonet->getAttributesRef();
      //      nonet->getAttributesRef() = NULL;
      //   }
      //   delete nonet;
      //}
   }


   // free memory
   while (itemList->GetCount())
   {
      CNetlistStruct *n = (CNetlistStruct*)itemList->RemoveHead();
      delete n;
   }
   delete itemList;
   delete connectedNetArray;


   CTime time = CTime::GetCurrentTime();
   stream.WriteString(time.Format("\nEnded at %I:%M:%S:%p\n"));
   stream.Close();
   Notepad(logFile);

   progress->DestroyWindow();
   delete progress;

   return 1;
}

/******************************************************************************
* Prep
*/
int Prep(CCEtoODBDoc *doc)
{
   if (Platform != WINNT)
   {
      if (ErrorMessage("We recommend only using Net Derive on Windows NT and Windows 2000.\nDo you want to continue anyways?", "WARNING!", MB_ICONEXCLAMATION | MB_YESNO) != IDYES)
         return 0;
   }


   DeriveNetlistOptions dlg;
   dlg.m_padstack = allowNetsWitoutPadstacks;
   dlg.m_singlePin = allowSinglePinNets;
   dlg.m_renameUnconnected = RenameUnconnectedNets;
   
   double accuracy = get_accuracy(doc);
   decimals = GetDecimals(doc->getSettings().getPageUnits());

   static BOOL ResolutionInitialized = FALSE;

   if (ResolutionInitialized)
      dlg.m_resolution.Format("%.*lf", decimals, resolution);
   
   if (!ResolutionInitialized || atof(dlg.m_resolution) < SMALLNUMBER)
   {
      switch (doc->getSettings().getPageUnits())
      {
      case UNIT_MM:
         dlg.m_resolution = "0.04";
         break;

      case UNIT_MILS:
         dlg.m_resolution = "10";
         break;

      case UNIT_INCHES:
      default:
         dlg.m_resolution = "0.001";
         break;
      }
   }

   dlg.m_bmpStart = FALSE;
   dlg.m_bmpFills = FALSE;
   dlg.m_bmpBW = FALSE;
   dlg.m_recursive = FALSE;

   if (dlg.DoModal() != IDOK)
      return 0;

   ResolutionInitialized = TRUE;

   resolution = atof(dlg.m_resolution);
/* if (resolution < accuracy)
      resolution = accuracy;*/

   allowNetsWitoutPadstacks = dlg.m_padstack;
   allowSinglePinNets = dlg.m_singlePin;
   RenameUnconnectedNets = dlg.m_renameUnconnected;
   bmpStart = dlg.m_bmpStart;
   bmpFills = dlg.m_bmpFills;
   bmpBW = dlg.m_bmpBW;
   algRecursive = dlg.m_recursive;


   logFile = GetLogfilePath("netlist.log");
   if (!stream.Open(logFile, CFile::modeCreate | CFile::modeWrite))
   {
      ErrorMessage(logFile, "Could not open log file");
      return 0;
   }

   CTime time = CTime::GetCurrentTime();
   stream.WriteString(time.Format("Started at %I:%M:%S:%p\n\n"));

   
   CString buf;
   buf.Format("Resolution = %lg\n", resolution);
   stream.WriteString(buf);

   buf.Format("Padstack = %d\n", dlg.m_padstack );
   stream.WriteString(buf);
   buf.Format("SinglePin = %d\n", dlg.m_singlePin );
   stream.WriteString(buf);
   buf.Format("bmpFills = %d\n", dlg.m_bmpFills );
   stream.WriteString(buf);
   buf.Format("bmpBW = %d\n", dlg.m_bmpBW );
   stream.WriteString(buf);
   buf.Format("Rename Unconnected = %d\n", dlg.m_renameUnconnected );
   stream.WriteString(buf);
   
   bmpStart = dlg.m_bmpStart;
   bmpFills = dlg.m_bmpFills;
   bmpBW = dlg.m_bmpBW;
   algRecursive = dlg.m_recursive;

   progress = new CProgressDlg("Deriving Netlist...", FALSE);
   progress->Create();

   progress->SetStatus("Preparing Data...");

   netnameKW = doc->RegisterKeyWord(ATT_NETNAME, 1, VT_STRING);
   oldNetnameKW = doc->RegisterKeyWord("OldNetname", 1, VT_STRING);

   return 1;
}

/******************************************************************************
* BuildFileLayerArray
*/
void BuildFileLayerArray(CCEtoODBDoc *doc, BlockStruct *block, CByteArray *fileLayerArray)
{
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getLayerIndex() >= 0)
      {
         fileLayerArray->SetAt(data->getLayerIndex(), TRUE);
         fileLayerArray->SetAt(doc->getLayerArray()[data->getLayerIndex()]->getMirroredLayerIndex(), TRUE);
      }

      if (data->getDataType() == T_INSERT)
      {
         BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (subblock && (subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) && subblock->getShape() == T_COMPLEX)
            subblock = doc->Find_Block_by_Num((int)(subblock->getSizeA()));

         if (subblock)
                BuildFileLayerArray(doc, subblock, fileLayerArray);
      }
   }
}

/******************************************************************************
* DrawThisLayer
*/
BOOL DrawThisLayer(LayerStruct *layer, int stackNum, CByteArray *fileLayerArray)
{
   if (!fileLayerArray->GetAt(layer->getLayerIndex()))
      return FALSE;

   if (!layer->getElectricalStackNumber())
   {
      switch (layer->getLayerType())
      {
      case LAYTYPE_PAD_ALL:
         return TRUE;
      case LAYTYPE_PAD_OUTER:
         return (stackNum == 1 || stackNum == maxStackupNum);
      case LAYTYPE_PAD_INNER:
         return (stackNum != 1 && stackNum != maxStackupNum);
      default:
         return FALSE;
      }
   }

   else if (layer->getElectricalStackNumber() == stackNum)
      return TRUE;
   else
      return FALSE;
}

/******************************************************************************
* FillItemList
*/
void FillItemList(CCEtoODBDoc *doc, FileStruct *file, BOOL AugmentOnly)
{
   // traces & vias
   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);                      

      if (data->getDataType() == T_POLY && !AugmentOnly)
      {
         LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];

         if (layer->getElectricalStackNumber()) 
         {
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            CPoly *poly = NULL;
            
            while (!poly && polyPos)
            {
               poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isFloodBoundary() || poly->isVoid())
                  poly = NULL;
            }

            if (!poly)
               continue;

            if (data->getGraphicClass() == graphicClassNormal)
               data->setGraphicClass(graphicClassEtch);

            CNetlistStruct *n = new CNetlistStruct;
            itemList->AddTail(n);
            n->map = &data->getAttributesRef();
            n->netIndex = 0;
            n->comppin = NULL;
            if (layer->getLayerType() == LAYTYPE_SPLITPLANE)
               n->type = nettype_border;
            else
               n->type = nettype_etch;
            n->netName = "";

            Point2 point2;
            CPnt *pnt = poly->getPntList().GetHead();
            point2.x = pnt->x * file->getScale();
            if (file->isMirrored()) point2.x = -point2.x;
            point2.y = pnt->y * file->getScale();
            Mat2x2 m;
            RotMat2(&m, file->getRotation());
            TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());
            n->x = point2.x;
            n->y = point2.y;

            if (data->getAttributesRef())
            {
               Attrib *attrib;

               if (data->getAttributesRef()->Lookup(oldNetnameKW, attrib) || data->getAttributesRef()->Lookup(netnameKW, attrib))
               {
                  n->netName = attrib->getStringValue();
               }
            }

            n->layermap.SetSize(maxStackupNum+1);

            for (int i=0; i<=maxStackupNum; i++)
               n->layermap.SetAt(i, (i == layer->getElectricalStackNumber()));
         }
      }

      if (data->getDataType() == T_INSERT)
      {
         BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (subblock)
         {
            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * file->getScale();
            point2.y = data->getInsert()->getOriginY() * file->getScale();

            if (file->isMirrored() & MIRROR_FLIP) point2.x = -point2.x;

            Mat2x2 m;
            RotMat2(&m, file->getRotation());
            TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

            if (subblock && subblock->getBlockType() == BLOCKTYPE_PADSTACK)
            {
               if (data->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)
                  data->getInsert()->setInsertType(insertTypeVia);

               CNetlistStruct *n = new CNetlistStruct;
               itemList->AddTail(n);
               n->map = &data->getAttributesRef();
               n->x = point2.x;
               n->y = point2.y;
               n->netIndex = 0;
               n->type = nettype_via;
               n->netName = "";
               n->comppin = NULL;

               if (data->getAttributesRef())
               {
                  Attrib* attrib;

                  if (data->getAttributesRef()->Lookup(oldNetnameKW, attrib) || data->getAttributesRef()->Lookup(netnameKW, attrib))
                  {
                     n->netName = attrib->getStringValue();
                  }
               }

               n->layermap.SetSize(maxStackupNum+1);
               for (int i=0; i<=maxStackupNum; i++)
                  n->layermap.SetAt(i, FALSE);

               double _rot;
               if (file->isMirrored())
                  _rot = file->getRotation() - data->getInsert()->getAngle();
               else
                  _rot = file->getRotation() + data->getInsert()->getAngle();

               FillPadstackLayerMap(doc, &subblock->getDataList(), n, file->getResultantMirror(data->getInsert()->getMirrorFlags()));
            }

            else if (subblock && (subblock->getFlags() & BL_APERTURE || subblock->getFlags() & BL_BLOCK_APERTURE) && !AugmentOnly)
            {
               if (data->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN)
                  data->getInsert()->setInsertType(insertTypeVia);

               CNetlistStruct *n = new CNetlistStruct;
               itemList->AddTail(n);
               n->map = &data->getAttributesRef();
               n->netIndex = 0;
               n->comppin = NULL;
               n->type = nettype_etch;
               n->netName = "";
               n->x = point2.x;
               n->y = point2.y;

               if (data->getAttributesRef())
               {
                  Attrib* attrib;

                  if (data->getAttributesRef()->Lookup(oldNetnameKW, attrib) || data->getAttributesRef()->Lookup(netnameKW, attrib))
                  {
                     n->netName = attrib->getStringValue();
                  }
               }

               LayerStruct *layer = doc->getLayerArray()[data->getLayerIndex()];
               n->layermap.SetSize(maxStackupNum+1);
               for (int i=0; i<=maxStackupNum; i++)
                  n->layermap.SetAt(i, (i == layer->getElectricalStackNumber()));
            }
            else if (subblock && subblock->getFlags() & BL_TOOL || subblock->getFlags() & BL_BLOCK_TOOL)
            {
               CNetlistStruct *n = new CNetlistStruct;
               itemList->AddTail(n);
               n->map = &data->getAttributesRef();
               n->x = point2.x;
               n->y = point2.y;
               n->netIndex = 0;
               n->type = nettype_via;
               n->netName = "";
               n->comppin = NULL;

               Attrib* attrib;

               if (data->getAttributesRef())
               {
                  if (data->getAttributesRef()->Lookup(oldNetnameKW, attrib) || data->getAttributesRef()->Lookup(netnameKW, attrib))
                  {
                     n->netName = attrib->getStringValue();
                  }
               }

               int startStackup = 1, endStackup = maxStackupNum;

               WORD drillStartLayerKW = doc->IsKeyWord(DRILL_STARTLAYER, 0);

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(drillStartLayerKW, attrib))
               {
                  LayerStruct *layer = doc->FindLayer_by_Name(attrib->getStringValue());

                  if (layer->getElectricalStackNumber())
                     startStackup = layer->getElectricalStackNumber();
               }

               WORD drillEndLayerKW = doc->IsKeyWord(DRILL_ENDLAYER, 0);

               if (data->getAttributesRef() && data->getAttributesRef()->Lookup(drillEndLayerKW, attrib))
               {
                  LayerStruct *layer = doc->FindLayer_by_Name(attrib->getStringValue());

                  if (layer->getElectricalStackNumber())
                     endStackup = layer->getElectricalStackNumber();
               }

               // Do Layers by Attrib
               n->layermap.SetSize(maxStackupNum+1);

               for (int i=startStackup; i<=endStackup; i++)
                  n->layermap.SetAt(i, TRUE);
            }
         }
      }
   }

   // put compPins in itemList
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      POSITION cpPos = net->getHeadCompPinPosition();
      while (cpPos != NULL)
      {
         CompPinStruct *cp = net->getNextCompPin(cpPos);

         BlockStruct *padstack = doc->Find_Block_by_Num(cp->getPadstackBlockNumber());
         if (!padstack)
            continue;

         CNetlistStruct *n = new CNetlistStruct;
         itemList->AddTail(n);
         n->map = &cp->getAttributesRef();
         n->netIndex = 0;
         n->type = nettype_pin;
         n->comppin = cp;
         n->x = cp->getOriginX();
         n->y = cp->getOriginY();
         n->layermap.SetSize(maxStackupNum+1);
         if (net->getFlags() & NETFLAG_NONET)
            n->netName = "";
         else
            n->netName = net->getNetName();

         for (int i=0; i<=maxStackupNum; i++)
            n->layermap.SetAt(i, 0);
         FillPadstackLayerMap(doc, &padstack->getDataList(), n, cp->getMirror());
      }
   }
}

/******************************************************************************
* FillPadstackLayerMap
*/
void FillPadstackLayerMap(CCEtoODBDoc *doc, CDataList *datalist, CNetlistStruct *padstack, int mirror)
{
   POSITION dataPos = datalist->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = datalist->GetNext(dataPos);                       

      if (data->getDataType() == T_INSERT)
      {
         BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (subblock && subblock->getFlags() & BL_APERTURE) // thermals on negative layers
         {
            if (data->getLayerIndex() != -1)
            {
               int layerNum = data->getLayerIndex();
               if (mirror & MIRROR_LAYERS)
                  layerNum = doc->getLayerArray()[data->getLayerIndex()]->getMirroredLayerIndex();
               LayerStruct *layer = doc->getLayerArray()[layerNum];
               int stackupNum = layer->getElectricalStackNumber();
               if (stackupNum)
               {
                  switch (layer->getLayerType())
                  {
                  case LAYTYPE_POWERNEG:
                  case LAYTYPE_SPLITPLANE:
                  case LAYTYPE_PAD_THERMAL:
                     if (subblock->getShape() == T_THERMAL)
                        padstack->layermap[stackupNum] = TRUE;
                     break;

                  default:
                     padstack->layermap[stackupNum] = TRUE;
                     break;
                  }
               }
               else
               {
                  switch (layer->getLayerType())
                  {
                  case LAYTYPE_PAD_ALL:
                     {
                        for (int i=1; i <= maxStackupNum; i++)
                           padstack->layermap[i] = TRUE;
                     }
                     break;
                  case LAYTYPE_PAD_INNER:
                     {
                        for (int i=2; i < maxStackupNum; i++)
                           padstack->layermap[i] = TRUE;
                     }
                     break;
                  case LAYTYPE_PAD_OUTER:
                     padstack->layermap[1] = TRUE;
                     padstack->layermap[maxStackupNum] = TRUE;
                     break;
                  }
               }
            }
         }
         else
            FillPadstackLayerMap(doc, &(subblock->getDataList()), padstack, mirror ^ data->getInsert()->getMirrorFlags());
      }
   }
}
               
/******************************************************************************
* GetPadstackCount
*/
int GetPadstackCount(CCEtoODBDoc *doc, BlockStruct *block)
{
   int padstackCount = 0;

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);                       

      if (data->getDataType() == T_INSERT)
      {
         BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         if (subblock && subblock->getBlockType() == BLOCKTYPE_PADSTACK)
            padstackCount++;
         else if (subblock)
            padstackCount += GetPadstackCount(doc, subblock);
      }
   }

   return padstackCount;
}

/******************************************************************************
* MakeOldNetnames 
*/
void MakeOldNetnames(FileStruct *file)
{   
   for (POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();dataPos != NULL;)
   {
      DataStruct* data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getAttributesRef() != NULL)
      {
         data->getAttributesRef()->changeAttributeKey(oldNetnameKW,netnameKW);

         //void *voidPtr;
         //if (data->getAttributesRef()->Lookup(oldNetnameKW, voidPtr))
         //{
         //   data->getAttributesRef()->RemoveKey(oldNetnameKW);
         //   delete (Attrib*)voidPtr;
         //}
         //if (data->getAttributesRef()->Lookup(netnameKW, voidPtr))
         //{
         //   data->getAttributesRef()->RemoveKey(netnameKW);
         //   data->getAttributesRef()->SetAt(oldNetnameKW, voidPtr);
         //}           
      }
   }
}


/******************************************************************************
* DumpNetStruct
*/
void DumpNetStruct()
{
   CStdioFile dumpStream;

   dumpStream.Open("C:\\temp\\nets.txt", CFile::modeReadWrite);
   dumpStream.SeekToEnd();

   POSITION tpos = itemList->GetHeadPosition();
   while (tpos != NULL)
   {
      CNetlistStruct *n = (CNetlistStruct*)itemList->GetNext(tpos);
      CString buf;
      buf.Format("%s (%+.2lg, %+.2lg), %d [", n->netName, n->x, n->y, n->netIndex);
      for (int i=0; i<=maxStackupNum; i++)
         if (n->layermap.GetAt(i))
         {
            CString buf2;
            buf2.Format("%d, ", i);
            buf += buf2;
         }
      buf += "\n";
      dumpStream.WriteString(buf);
   }
   dumpStream.Close();
}


/////////////////////////////////////////////////////////////////////////////
// DeriveNetlistOptions dialog
DeriveNetlistOptions::DeriveNetlistOptions(CWnd* pParent /*=NULL*/)
   : CDialog(DeriveNetlistOptions::IDD, pParent)
{
   //{{AFX_DATA_INIT(DeriveNetlistOptions)
   m_resolution = _T("");
   m_padstack = FALSE;
   m_singlePin = FALSE;
   m_bmpFills = FALSE;
   m_bmpStart = FALSE;
   m_bmpBW = FALSE;
   m_recursive = FALSE;
   m_renameUnconnected = FALSE;
   //}}AFX_DATA_INIT
}

void DeriveNetlistOptions::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DeriveNetlistOptions)
   DDX_Text(pDX, IDC_RESOLUTION, m_resolution);
   DDX_Check(pDX, IDC_PADSTACK, m_padstack);
   DDX_Check(pDX, IDC_PIN, m_singlePin);
   DDX_Check(pDX, IDC_BMP_FILLS, m_bmpFills);
   DDX_Check(pDX, IDC_BMP_START, m_bmpStart);
   DDX_Check(pDX, IDC_BMP_BW, m_bmpBW);
   DDX_Check(pDX, IDC_ALG_RECURSIVE, m_recursive);
   DDX_Check(pDX, IDC_RENAME_UNCONNECTED, m_renameUnconnected);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DeriveNetlistOptions, CDialog)
   //{{AFX_MSG_MAP(DeriveNetlistOptions)
   //}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PIN, OnBnClickedPin)
END_MESSAGE_MAP()

BOOL DeriveNetlistOptions::OnInitDialog() 
{
   CDialog::OnInitDialog();

#ifndef _DEBUG
   GetDlgItem(IDC_ALG_RECURSIVE)->ShowWindow(FALSE);
   GetDlgItem(IDC_BMP_START)->ShowWindow(FALSE);
   GetDlgItem(IDC_BMP_FILLS)->ShowWindow(FALSE);
   GetDlgItem(IDC_BMP_BW)->ShowWindow(FALSE);
#endif
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
          
/******************************************************************************
* GetSelCoord
*/
static void GetSelCoord(SelectStruct *s, double *x, double *y)
{
   switch (s->getData()->getDataType())
   {
   case T_POLY:
      {
         Point2 point2;
         CPoly *poly = s->getData()->getPolyList()->GetHead();
         CPnt *pnt = poly->getPntList().GetHead();
         point2.x = pnt->x * s->scale;
         if (s->mirror) point2.x = -point2.x;
         point2.y = pnt->y * s->scale;
         Mat2x2 m;
         RotMat2(&m, s->rotation);
         TransPoint2(&point2, 1, &m, s->insert_x, s->insert_y);
         *x = point2.x;
         *y = point2.y;
      }
      break;

   case T_INSERT:
      break;
   }
}

/******************************************************************************
* DeriveConnectivityOnSelected
*
void DeriveConnectivityOnSelected(CCEtoODBDoc *doc, CString attribName)
{
   double accuracy = get_accuracy(doc) * 2;
   FF_Start(doc, accuracy);

   FF_DrawSelected(doc);

//FF_ShowBitmap(TRUE);

   static int num = 1;
   CString temp;
   double x, y;
   WORD keyword = doc->RegisterKeyWord(attribName, 0, VT_INTEGER);

   while (doc->SelectList.GetCount())
   {
      SelectStruct *s = doc->SelectList.RemoveHead();
      s->p->setSelected(false);
      s->p->setMarked(false);
      doc->SetAttrib(&s->p->getAttributesRef(), keyword, VT_INTEGER, &num, SA_OVERWRITE, NULL);
      GetSelCoord(s, &x, &y);
      delete s;
      if (!FF_Fill(x, y, FALSE))
      {
         ErrorMessage("Failure");
         return;
      }

//FF_ShowBitmap(TRUE);

      POSITION selPos = doc->SelectList.GetHeadPosition();
      while (selPos)
      {
         s = doc->SelectList.GetAt(selPos);

         GetSelCoord(s, &x, &y);

         if (FF_Query(x, y))
         {
            POSITION tempPos = selPos;
            doc->SelectList.GetNext(selPos);
            s->p->setSelected(false);
            s->p->setMarked(false);
            doc->SetAttrib(&s->p->getAttributesRef(), keyword, VT_INTEGER, &num, SA_OVERWRITE, NULL);
            doc->SelectList.RemoveAt(tempPos);
            delete s;
         }
         else
            doc->SelectList.GetNext(selPos);
      }

      num++;
   }
}


/******************************************************************************
* SearchForNet
*
static int SearchForNet(CCEtoODBDoc *doc, double tolerance, int maxStackup, 
                        WORD netnameKeyword, CDataList *dataList, CompPinStruct *cp, CString *gerberNetName, 
                        DTransform *cadXForm, DTransform *gerberXForm)
{
   BOOL Found = FALSE;

   POSITION pos = dataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = dataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_FREEPAD)
         continue;

      Point2 cPnt, gPnt;
      cPnt.x = cp->x;
      cPnt.y = cp->y;
      cadXForm->TransformPoint(&cPnt);

      gPnt.x = data->getInsert()->getOriginX();
      gPnt.y = data->getInsert()->getOriginY();
      gerberXForm->TransformPoint(&gPnt);

      if (fabs(cPnt.x - gPnt.x) < tolerance && fabs(cPnt.y - gPnt.y) < tolerance)
      {
         BlockStruct *block = doc->BlockArray[data->getInsert()->getBlockNumber()];

         BOOL LayerGood = (cp->visible & VISIBLE_TOP && block->getFlags() & BL_ACCESS_TOP) ||
               (cp->visible & VISIBLE_BOTTOM && block->getFlags() & BL_ACCESS_BOTTOM);

         if (!LayerGood)
            continue;

         if (!data->getAttributesRef())
         {
            Found = TRUE;
            continue;
         }

         void *voidPtr;
         if (data->getAttributesRef()->Lookup(netnameKeyword, voidPtr))
         {
            Attrib *attrib = (Attrib*)voidPtr;
            *gerberNetName = attrib->getStringValue();
            return 1;
         }
      }
   }

   pos = dataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = dataList->GetNext(pos);

      if (data->getDataType() != T_POLY)
         continue;

      if (data->getGraphicClass() != graphicClassEtch)
         continue;

      CPnt *first, *second, *last;
      CPoly *poly = data->getPolyList()->GetHead();
      first = poly->getPntList().GetHead();
      last = poly->getPntList().GetTail();
      POSITION tempPos = poly->getPntList().GetHeadPosition();
      poly->getPntList().GetNext(tempPos);
      second = poly->getPntList().GetNext(tempPos);

      BOOL LayerGood = (cp->visible & VISIBLE_TOP && doc->LayerArray[data->getLayerIndex()]->getElectricalStackNumber() == 1) ||
            (cp->visible & VISIBLE_BOTTOM && doc->LayerArray[data->getLayerIndex()]->getElectricalStackNumber() == maxStackup);

      if (!LayerGood)
         continue;

      if ((fabs(cp->x - first->x) < tolerance && fabs(cp->y - first->y) < tolerance) ||
            (fabs(cp->x - last->x) < tolerance && fabs(cp->y - last->y) < tolerance) ||
            (fabs(cp->x - (first->x + second->x) / 2) < tolerance && fabs(cp->y - (first->y + second->y) / 2) < tolerance))
      {
         if (!data->getAttributesRef())
         {
            Found = TRUE;
            continue;
         }

         void *voidPtr;
         if (data->getAttributesRef()->Lookup(netnameKeyword, voidPtr))
         {
            Attrib *attrib = (Attrib*)voidPtr;
            *gerberNetName = attrib->getStringValue();
            return 1;
         }
      }
   }

   if (Found)
      return -1;

   return 0;
}

/******************************************************************************
* 
   The algorith is:
   1. See if there is a gerber file.
   2. Check if it has padstacks.
   3. See if there is a CAD/IPC file.
   4. Make a generate_nonconn and pinloc.
   5. Assign the gerber padstacks the netnames from the ipc/cad file netlist!
   6. Run derive_netlist!
*
void CCEtoODBDoc::OnGerberCADNetlistCompare() 
{
   CProgressDlg *progress;

   progress = new CProgressDlg("Netlist Compare", FALSE);
   progress->Create();

   double tolerance = 5 * Units_Factor(UNIT_MILS, Settings.PageUnits);

   // prep
// generate_PINLOC(this, file, 1);
   generate_PADSTACKACCESSFLAG(this, 1);

   FileStruct *gerberFile = NULL;
   FileStruct *cadFile = NULL;


   progress->SetStatus("Search for Files Sets...");

   
   // find gerberFile
   POSITION pos = this->FileList.GetHeadPosition();
   while (pos)
   {
      FileStruct *file = this->FileList.GetNext(pos);
      if (!file->isShown())  continue;
      if (file->source_cad == Type_Gerber || file->source_cad == Type_Barco)
      {
         if (!gerberFile)
            gerberFile = file;
         else
         {
            progress->DestroyWindow();
            delete progress;
            ErrorMessage("Multiple visible Gerber or Barco files found!", "Gerber Netlist Compare");
            return;
         }
      }
   }

   if (!gerberFile)
   {
      progress->DestroyWindow();
      delete progress;
      ErrorMessage("No visible Gerber or Barco file found!", "Gerber Netlist Compare");
      return;
   }


   // find cadFile
   pos = this->FileList.GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = this->FileList.GetNext(pos);
      if (!file->isShown())        continue;
      if (file == gerberFile) continue;

      if (file->getNetList().GetHeadPosition())
      {
         if (!cadFile)
            cadFile = file;
         else
         {
            progress->DestroyWindow();
            delete progress;
            ErrorMessage("Multiple visible CAD files with Netlist found!", "Gerber Netlist Compare");
            return;
         }
      }
   }

   if (!cadFile)
   {
      progress->DestroyWindow();
      delete progress;
      ErrorMessage("No visible CAD file with Netlist found!", "Gerber Netlist Compare");
      return;
   }


   DTransform cadXForm(cadFile->insert_x, cadFile->insert_y, cadFile->scale, cadFile->rotation, cadFile->mirror);
   DTransform gerberXForm(gerberFile->insert_x, gerberFile->insert_y, gerberFile->scale, gerberFile->rotation, gerberFile->mirror);


   ExplodeNcPins(this, cadFile);


   CWaitCursor wait;


   progress->SetStatus("Comparing Netlists...");

   
   // log strings
   CString logShorts, logOpens, logPins, buf;
   int algIndexShorts = GetAlgorithmNameIndex(this, "Net Compare - Shorts");
   int algIndexOpens = GetAlgorithmNameIndex(this, "NetCompare - Opens");
   int algIndexPadStacks = GetAlgorithmNameIndex(this, "NetCompare - PadStacks");

   // compare netlists
   CMapStringToOb gerberToCadNetNameMap;
   WORD netnameKeyword = IsKeyWord(ATT_NETNAME, 0);
   int decimals = GetDecimals(Settings.PageUnits);
   int maxStackup = GetMaxStackupNum(this);

   POSITION cadNetPos = cadFile->getNetList().GetHeadPosition();
   while (cadNetPos)
   {
      NetStruct *cadNet = cadFile->getNetList().GetNext(cadNetPos);

      CStringList gerberNetsList; // list of gerber netnames hit on this cad net

      POSITION cpPos = cadNet->getHeadCompPinPosition();
      while (cpPos)
      {
         CompPinStruct *cp = cadNet->getNextCompPin(cpPos);

         CString gerberNetName;
         switch (SearchForNet(this, tolerance, maxStackup, 
                              netnameKeyword, &gerberFile->getBlock()->getDataList(), cp, &gerberNetName,
                              &cadXForm, &gerberXForm))
         {
         case 1:
            if (!gerberNetsList.Find(gerberNetName))
               gerberNetsList.AddTail(gerberNetName);
            break;
         case 0:
            {
               buf.Format("No Gerber PadStack found for Net \"%s\" for pin \"%s\"-\"%s\" at (%.*lf,%.*lf)", cadNet->NetName, cp->comp, cp->pin, decimals, cp->x, decimals, cp->y);
               
               DRCStruct *drc = CreateDRC(gerberFile, buf, DRC_CLASS_SIMPLE, 0, algIndexPadStacks, DRC_ALG_GENERIC);

               buf += ".\n";
               logPins += buf;
            }
            break;
         case -1:
            {
               buf.Format("No NetName on PadStack found for Net \"%s\" for pin \"%s\"-\"%s\" at (%.*lf,%.*lf)", cadNet->NetName, cp->comp, cp->pin, decimals, cp->x, decimals, cp->y);
               
               DRCStruct *drc = CreateDRC(gerberFile, buf, DRC_CLASS_SIMPLE, 0, algIndexPadStacks, DRC_ALG_GENERIC);

               buf += ".\n";
               logPins += buf;
            }
            break;
         }
      }


      // open net
      if (gerberNetsList.GetCount() > 1) // if hit more than one gerber netname on this cad net
      {
         CStringList *stringList = new CStringList;
         CString gerberNetname;

         POSITION gerberNetsListPos = gerberNetsList.GetHeadPosition();
         CString buf2 = "\"";
         gerberNetname = gerberNetsList.GetNext(gerberNetsListPos);
         stringList->AddTail(gerberNetname);
         buf2 += gerberNetname;
         buf2 += "\"";
         while (gerberNetsListPos)
         {
            buf2 += ", \"";
            gerberNetname = gerberNetsList.GetNext(gerberNetsListPos);
            stringList->AddTail(gerberNetname);
            buf2 += gerberNetname;
            buf2 += "\"";
         }

         buf.Format("Open Net: CAD Net \"%s\" maps to %s", cadNet->NetName, buf2);

         DRCStruct *drc = CreateDRC(gerberFile, buf, DRC_CLASS_NETS, 0, algIndexOpens, DFT_ALG_NET_COLLISION);
         drc->voidPtr = stringList;

         drc = CreateDRC(cadFile, buf, DRC_CLASS_NETS, 0, algIndexOpens, DFT_ALG_NET_COLLISION);
         stringList = new CStringList;
         stringList->AddTail(cadNet->NetName);
         drc->voidPtr = stringList;

         buf += ".\n";
         logOpens += buf;
      }


      POSITION gerberNetsListPos = gerberNetsList.GetHeadPosition();
      while (gerberNetsListPos)
      {
         CString gerberNet = gerberNetsList.GetNext(gerberNetsListPos);
         CObject *object;
         if (gerberToCadNetNameMap.Lookup(gerberNet, object))
         {
            CStringList *stringList = (CStringList*)object;
            if (!stringList->Find(cadNet->NetName))
               stringList->AddTail(cadNet->NetName);
         }
         else
         {
            CStringList *stringList = new CStringList;
            stringList->AddTail(cadNet->NetName);
            gerberToCadNetNameMap.SetAt(gerberNet, stringList);
         }
      }
   }


   // shorted nets
   POSITION gerberToCadNetNameMapPos = gerberToCadNetNameMap.GetStartPosition();
   while (gerberToCadNetNameMapPos)
   {
      CString netName;
      CObject *object;

      gerberToCadNetNameMap.GetNextAssoc(gerberToCadNetNameMapPos, netName, object);

      CStringList *stringList = (CStringList*)object;

      if (stringList->GetCount() > 1)
      {
         POSITION stringListPos = stringList->GetHeadPosition();
         CString buf2 = "\"";
         buf2 += stringList->GetNext(stringListPos);
         buf2 += "\"";
         while (stringListPos)
         {
            buf2 += ", \"";
            buf2 += stringList->GetNext(stringListPos);
            buf2 += "\"";
         }

         buf.Format("Shorted Net: Gerber Net \"%s\" maps to %s", netName, buf2);

         DRCStruct *drc = CreateDRC(cadFile, buf, DRC_CLASS_NETS, 0, algIndexShorts, DFT_ALG_NET_COLLISION);
         drc->voidPtr = stringList;

         drc = CreateDRC(gerberFile, buf, DRC_CLASS_NETS, 0, algIndexShorts, DFT_ALG_NET_COLLISION);
         stringList = new CStringList;
         stringList->AddTail(netName);
         drc->voidPtr = stringList;

         buf += ".\n";
         logShorts += buf;
      }
      else
         delete stringList;
   }


   progress->DestroyWindow();
   delete progress;

   
   // open log file
   CString gpcbLogFile = getApp().getUserPath() + GERBERPCBLogFile;
   FILE  *flog;

   if ((flog = fopen(GERBERPCBLogFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", GERBERPCBLogFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fputs("SHORTS\n------\n", flog);
   fputs(logShorts, flog);

   fputs("\nOPENS\n-----\n", flog);
   fputs(logOpens, flog);

   fputs("\nPINS\n----\n", flog);
   fputs(logPins, flog);

   fclose(flog);

   Notepad(GERBERPCBLogFile);
};
*/
void DeriveNetlistOptions::OnBnClickedPin()
{
	// TODO: Add your control notification handler code here
}
