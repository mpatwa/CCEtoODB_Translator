// $Header: /CAMCAD/4.5/read_wrt/Men_out.cpp 60    8/05/06 4:55p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include <math.h>
#include "menlib.h"
#include "pcb_net.h"
#include "outline.h"
#include "ck.h"
#include "pcblayer.h"
#include "apertur2.h"
#include "graph.h"   
#include <float.h>
#include "polylib.h"
#include "men_out.h"
#include "centroid.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// dump for test the trace function.
//#define   DUMP  
#undef   DUMP

extern CProgressDlg *progress;
extern CViaArray *viaArray; // from PCB_NET.CPP
extern CSegmentArray *segmentArray; // from PCB_NET.CPP

static CCEtoODBDoc *doc;
static   FileStruct  *cur_file;
static   int      display_error;
static   int      page_units;
static   FILE     *flog;

static   int         max_signalcnt;    // this is the number of Routing Layers layers (SIGNAL_) but not power
static   int         maxXRF;        // this is the number of XRF layers

static   CPolyArray  polyarray;
static   int         polycnt;

static   ViaArray    viaarray;
static   int         viacnt;

static   PadRotArray    padrotarray;
static   int         padrotcnt;

static   PadStackArray     padstackarray1;
static   int         padstackcnt;

static   CMentorLayerArray mentorLayerArray;
static   int         mentorLayerArrayCount = 0;

static   int         SHOWOUTLINE = FALSE; // set in mentor.out

static   int         write_area_prt;

static   int         output_units_accuracy;
static   CString     output_units;
static   CString     output_units_units;
static   double      one_mil;

static   CString     mentorlayer_file;

static   LayerRepArray  layerreparray; // layer report file load
static   int            layerrepcnt;

//static int      number_of_layers = 2;   // this is the Mentor solder layer

static int load_MENTORsettings(CString settingsFileName);
static int load_MENTORlayerfile(CString layerFileName);
static void do_layerlist();
static int edit_layerlist();
static void free_layerlist();
static void update_layerreparray();
static int net_layerlist(FileStruct *file);
static int resequence_signal();
static int update_unassigned_xrflayers();
static int assign_xrf_number();

static int do_layerfile(FILE *fp);
static int do_partfile(FILE *wfp, FileStruct *file, double unitsFactor, double accuracy);
static int do_techfile(FILE *fp);
static int do_componentfile(FILE *wfp, FileStruct *file, double unitsFactor);
static int do_netlistfile(FILE *fp, FileStruct *file);
static int do_tracefile(FILE *wfp, FILE *prtfp, FileStruct *file,double unitsFactor);

static const char *get_uniquegeomname(int blocknum);
static const char *get_defaultlayer(LayerStruct *l, int *ON);
static int get_free_layer_stacknum();

static void Write_Traces(FILE *stream, CResultList *resultList, const char *netname,double scale);

static int MENTOR_OutlineSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
         double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary,
         int gr_class);

static int MENTOR_OutlineBOARDData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
         double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary,
         int gr_class);

static void MENTOR_WriteData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static void MENTOR_WriteGEOMASCIIData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static int MENTOR_GetPADSTACKData(FILE *fp, CDataList *DataList, int insertlevel,
                                  double *drill, double scale, int *top, int *inner, int *bot, CUIntArray *padElecLayerList, int &padElecLayerCount);

static int MENTOR_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, int smdtyp);

static int MENTOR_WritePCBCOMPONENTData(FILE *fp, const char *shapename, CDataList *DataList, 
      double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, MENPinExtent *p);

static void MENTOR_WriteClassPCBCOMPONENTData(FILE *fp, const char *shapename, CDataList *DataList, 
      double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static void PCB_WriteCOMPONENTData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);

static void get_vianames(CDataList *DataList);
static int do_padrules(FILE *fp);

static void PCB_WriteROUTESData(FILE *fp, FILE *prtfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname);

static int do_signalpinloc(NetStruct *net, double scale);

static void MENTOR_FindFillVoidData(CDataList *DataList, int insertLayer);

static CString MentorCheckName(char nametype, CString nametocheck);

/******************************************************************************
* MENTORBoardStation_WriteFiles
*/
void MENTORBoardStation_WriteFiles(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *format, int pageUnits)
{
   FILE *wfp, *prtfp;
   FileStruct *file;
   display_error = FALSE;

   CString logFile = GetLogfilePath("mentor.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fprintf(flog,"\nProcedure to load created data into Mentor 8.x\n");
   fprintf(flog,"1. Make a new directory xxx\n");
   fprintf(flog,"2. Copy all output files from %s into this directory.\n", pathname);
   fprintf(flog,"3. Make sure that these files are UNIX format (dos2unix)\n");
   fprintf(flog,"4. Run Librarian on a NEW project - use standard PCB technology\n");
   fprintf(flog,"5. Choose RESTORE ASCII geometries - read .prt and then do SAVE - DESIGN ALL\n");
   fprintf(flog,"Now you need to do the conversion of files to design objects.\n");
   fprintf(flog,"7. run pcb_design_data_path -objtype nets -writefrom /xxx/xxx/xxx.net -objpath xxx/pcb\n");
   fprintf(flog,"8. Do the same for comps and traces.\n");
   fprintf(flog,"9. Now go into boardstation - choose the design directory (not the pcb) and OK\n");
   fprintf(flog,"10. Now enter the ample command $setup_attempt_reconnect(@segment).\n");
   fprintf(flog,"11. Now the complete design is loaded into Mentor - off you go !\n");

   fprintf(flog,"For better trace conversion use this procedure:\n");
   fprintf(flog,"1. once your in layout, turn checking off.\n");
   fprintf(flog,"2. select the following menu: \"setup placement>interactiveplacement>attempt reconnect\"\n");
   fprintf(flog,"   then click on \"rip up segments only\" , OK.\n");
   fprintf(flog,"3. then use the restore traces command and click on  \"replace existingtraces\",\n");
   fprintf(flog,"   \"fix improperly connected routing\", \"remove duplicate routing\".\n");
   display_error++;

   doc = Doc;
   page_units = pageUnits;
   
   double   unitsFactor = Units_Factor(page_units, UNIT_INCHES);

   // format.PortFileUnits = dlg.m_units; // 0=mm, 1=inch, 2=TN, 3=mils
   switch (format->PortFileUnits)
   {
      case 0:  // mm
         // for inch it should be 4 
         // for mm it should be 5
         output_units_accuracy = 5;
         output_units = "mm"; // for part file
         output_units_units = "MM"; // for UNIT command
         unitsFactor = Units_Factor(page_units, UNIT_MM);
         one_mil = 1 * Units_Factor(UNIT_MILS, UNIT_MM);
      break;
      case 1:  // inch
         // for inch it should be 4
         // for mm it should be 5
         output_units_accuracy = 4;
         output_units = "inches";   // for part file
         output_units_units = "IN"; // for UNIT command
         unitsFactor = Units_Factor(page_units, UNIT_INCHES);
         one_mil = 0.001;
      break;
      default:
         // error - can not open the file.
         CString  tmp;
         tmp.Format("MENTOR UNITS must be INCHES, MM");
         MessageBox(NULL, tmp,"Error Units", MB_OK | MB_ICONHAND);
         return;
      break;
   }

   double accuracy = 1;
   for (int a=0;a<output_units_accuracy;a++)
      accuracy *= 0.1;

   CString settingsFile( getApp().getExportSettingsFilePath("mentor.out") );
   load_MENTORsettings(settingsFile);

   CString checkFile( getApp().getSystemSettingsFilePath("mentor.chk") );
   check_init(checkFile);

   CString  geomfile, compfile, netfile, tracefile, layerfile, techfile;

   geomfile = pathname;
   geomfile += "geom_ascii";
   compfile = pathname;
   compfile += "comps.comp";
   netfile = pathname;
   netfile += "nets.net";
   tracefile = pathname;
   tracefile += "traces.trace";
   layerfile = pathname;
   layerfile += "layers.layer";
   techfile = pathname;
   techfile += "tech.tec";

   layerreparray.SetSize(10,10);
   layerrepcnt = 0;

   polyarray.SetSize(100,100);
   polycnt = 0;

   viaarray.SetSize(10,10);
   viacnt = 0;

   padrotarray.SetSize(10,10);
   padrotcnt = 0;

   padstackarray1.SetSize(10,10);
   padstackcnt = 0;
   
   mentorLayerArray.SetSize(10,10);
   mentorLayerArrayCount = 0;

   load_MENTORlayerfile(mentorlayer_file);

   if (format->Output)  // 0 = PCB, 1 = GRAPHIC
   {
      // open file for writting GRAPHIC files
      if ((wfp = fopen(geomfile, "wt")) == NULL)
      {
         ErrorMessage(geomfile, "Error open file");
         return;
      }

      do_layerlist();
      if (edit_layerlist())
      {
         update_layerreparray();

         CWaitCursor wait;

         // reset status 
         progress->SetStatus("Preparing GRAPHIC Data...");

         wmen_Graph_File_Open_Layout(wfp);
      
         wmen_Graph_Init(output_units_accuracy, MEN_SMALLNUMBER, output_units);

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);
            if (block == NULL)   continue;
            if (block->getFlags() & BL_WIDTH)         continue;
         
            // create necessary aperture blocks
            if (block->getFlags() & BL_TOOL)          continue;
            if (block->getFlags() & BL_APERTURE)      continue;
            if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
      
            // create all other blocks that are not file blocks
            if (!(block->getFlags() & BL_FILE))
            {
               CString  n = get_uniquegeomname(block->getBlockNumber());

               wmen_Graph_Block_On(MentorCheckName('g', n));
               MENTOR_WriteData(&(block->getDataList()), 0.0, 0.0, 0.0, 0, unitsFactor, 0, -1);
            }
         }

         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
   
            if (!file->isShown())  continue;
            if (file->getBlock() == NULL)  continue;   // empty file

            if (strlen(file->getName()) == 0)  file->setName("board");

            wmen_Graph_Block_On(MentorCheckName('g', file->getName()));
            MENTOR_WriteData(&(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);
         }
         // Does end of COMPONENT
         wmen_Graph_File_Close_Layout();

         FILE  *lfp;
         // do layer file
         if ((lfp = fopen(layerfile, "wt")) == NULL)
         {
            ErrorMessage(layerfile, "Error open file");
            //break;
         }
         else
         {
            fprintf(flog,"Layer file %s created\n",layerfile);
            display_error++;

            // reset status for Panel file 
            progress->SetStatus(layerfile);
            do_layerfile(lfp);
            fclose(lfp);
         }
      }  // edit layerlist
      free_layerlist();
      // close write file
      fclose(wfp);
   }
   else
   {
      // PCB translation
      int   pcb_found = FALSE;
      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         cur_file = file = doc->getFileList().GetNext(pos);
         if (!file->isShown())  continue;

         if (file->getBlockType() == BLOCKTYPE_PCB)  
         {
            pcb_found = TRUE;

            do_layerlist();

            // here check if a void/fill layer is needed.
            MENTOR_FindFillVoidData(&(file->getBlock()->getDataList()), -1);

            net_layerlist(file); // the netlist must be after the layer assign, so that first is the layer
                                 // and than the powernets.

            resequence_signal();

            if (edit_layerlist())
            {
               update_unassigned_xrflayers();
               update_layerreparray();

               CWaitCursor wait;
               // reset status 
               progress->SetStatus("Preparing PCB Data...");

               generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

               // do prt file
               progress->SetStatus(geomfile);
               if ((prtfp = fopen(geomfile, "wt")) == NULL)
               {
                  ErrorMessage(geomfile, "Error open file");
                  break;
               }
               fprintf(flog,"Geometry ASCII file %s created\n",geomfile);
               display_error++;

               // reset status for Panel file 
               progress->SetStatus(geomfile);

               do_partfile(prtfp, file, unitsFactor, accuracy);

               if ((wfp = fopen(techfile, "wt")) == NULL)
               {
                  ErrorMessage(techfile, "Error open file");
                  break;
               }
               fprintf(flog,"Technology file %s created\n", techfile);
               display_error++;

               do_techfile(wfp);

               fclose(wfp);

               // do cmp file
               progress->SetStatus(compfile);
               if ((wfp = fopen(compfile, "wt")) == NULL)
               {
                  ErrorMessage(compfile, "Error open file");
                  break;
               }
               fprintf(flog,"Component file %s created\n",compfile);
               display_error++;

               // reset status for Panel file 
               progress->SetStatus(compfile);
               do_componentfile(wfp,file,unitsFactor);

               fclose(wfp);

               // do net file
               progress->SetStatus(netfile);
               if ((wfp = fopen(netfile, "wt")) == NULL)
               {
                  ErrorMessage(netfile, "Error open file");
                  break;
               }
               fprintf(flog,"Netlist file %s created\n",netfile);
               display_error++;

               // reset status for Panel file 
               progress->SetStatus(netfile);
               do_netlistfile(wfp,file);

               fclose(wfp);

               // do trace file
               if ((wfp = fopen(tracefile, "wt")) == NULL)
               {
                  ErrorMessage(tracefile, "Error open file");
                  break;
               }
               fprintf(flog,"Trace file %s created\n",tracefile);
               display_error++;

               // reset status for Panel file 
               progress->SetStatus(tracefile);
               do_tracefile(wfp, prtfp, file, unitsFactor);
               fclose(wfp);

               // do layer file
               if ((wfp = fopen(layerfile, "wt")) == NULL)
               {
                  ErrorMessage(layerfile, "Error open file");
                  break;
               }
               fprintf(flog,"Layer file %s created\n",layerfile);
               display_error++;

               // reset status for Panel file 
               progress->SetStatus(layerfile);
               do_layerfile(wfp);
               fclose(wfp);

               fclose(prtfp);
            }  // end edit layerlist

            free_layerlist();
            break;   // break out of while list
         }
      }

      if (!pcb_found)
      {
         MessageBox(NULL, "No PCB file found !","MENTOR Output", MB_OK | MB_ICONHAND);
      }
   }

   if (check_report(flog)) display_error++;
   check_deinit();

   fclose(flog);

   polyarray.RemoveAll();
   viaarray.RemoveAll();

   int i;
   for (i=0;i<padstackcnt;i++)
   {
      delete padstackarray1[i];
   }
   padstackarray1.RemoveAll();

   for (i=0;i<padrotcnt;i++)
   {
      delete padrotarray[i];
   }
   padrotarray.RemoveAll();

   for (i=0;i<layerrepcnt;i++)
   {
      delete layerreparray[i];
   }
   layerrepcnt = 0;
   layerreparray.RemoveAll();

   if (display_error)
      Logreader(logFile);
}

//******************************************************************************

static CString MentorCheckName(char nametype, CString nametocheck)
{
   // The check_name utility only does substitions.
   // Mentor BS allows many special chars in the names, but not at the first char.
   // The nice thing to do would be exten teh check_name utility to support
   // positional checking, but that is risky because a lot of exporters use it
   // and not straight forward, since it would have to be done in a general fashion.
   // So here we wrap the standard subsitutor, let it do its thing, then
   // apply the first char check and adjust the result as needed.

   CString checkedname( check_name(nametype, nametocheck) );

   CString firstchar = checkedname.Left(1);
   firstchar.MakeUpper();
   if (firstchar.FindOneOf("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") < 0)
   {
      CString chkd2;
      chkd2.Format("%c%s", toupper(nametype), checkedname);
      return chkd2;
   }

   return checkedname;
}
   
/******************************************************************************
* do_layerlist
*/
static void do_layerlist()
{
   LayerStruct *layer;
   int         ON, signr = 0;
   char        typ = 'D';
   CString     menlay;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      ON = TRUE;
      layer = doc->getLayer(j);
      if (layer == NULL)   continue; // could have been deleted.

      signr = 0;

      // here check if layer was in mentor.out. If not set it non visible
      menlay = MentorCheckName('l', get_defaultlayer(layer, &ON));
      
      typ = 'D';

      if (layer->getElectricalStackNumber())
      {
         signr = layer->getElectricalStackNumber();

         if (layer->getLayerType() == LAYTYPE_POWERNEG)
            typ = 'P';
         else
            typ = 'S';

         menlay.Format("SIGNAL_%d",signr);
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
         menlay = "PAD";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
         menlay = "SIGNAL";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {
         signr = LAY_INNER;
         menlay = "SIGNAL";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL)
      {
         signr = LAY_ALL;
         menlay = "SIGNAL";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         signr = LAY_TOP;
         menlay = "PAD";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         signr = LAY_BOT;
         menlay = "PAD_2";
         ON = TRUE;
      }

      if (signr == 0)
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, -1))
            continue;
      }
      else
      {
         // here check if layer->getName() is not already done
         if (!doc->get_layer_visible(j, -1))
         {
            // here a warning that an etch layer is not visible
            CString t;
            t.Format("Electrical Layer [%s] not visible!\nNon Visible Layers will NOT be translated.",layer->getName());
            MessageBox(NULL, t, "Layer Display", MB_ICONEXCLAMATION | MB_OK);
         }
      }

      //mentorLayerArray.SetSizes
      MentorLayerStruct *ml = new MentorLayerStruct;
      ml->stackNum = signr;   // artwork layer 
      ml->xrf = 0;
      ml->layerindex = j;  // index into doc->LayerArray
      ml->on = ON;
      ml->type = typ;
      ml->oldName = layer->getName(); // original Layer
      ml->newName = menlay;      // mentor layer
      mentorLayerArray.SetAtGrow(mentorLayerArrayCount, ml);
      mentorLayerArrayCount++;
   }
}

/******************************************************************************
* edit_layerlist
*/
static int edit_layerlist()
{
   // fill array
   MentorLayerDlg dlg;

   fprintf(flog, "Max_Signalcnt %d \n", max_signalcnt); // used in XRF

   assign_xrf_number();

   dlg.arr = &mentorLayerArray;
   dlg.maxArr = mentorLayerArrayCount;

   if (dlg.DoModal() != IDOK) 
      return FALSE;

   return TRUE;
}

/******************************************************************************
* free_layerlist
*/
static void free_layerlist()
{
   for (int i=0; i<mentorLayerArrayCount; i++)
      delete mentorLayerArray[i];
   mentorLayerArray.RemoveAll();
}

/******************************************************************************
* update_layerreparray
*/
static void update_layerreparray()
{
   // the user layers are in mentorLayerArray
   for (int i=0; i<mentorLayerArrayCount; i++)
   {
      if (mentorLayerArray[i]->on == 0)
         continue; // not visible

      int found = FALSE;
      for (int ii=0; ii<layerrepcnt; ii++)
      {
         if (mentorLayerArray[i]->newName.Compare(layerreparray[ii]->name) == 0)
            found = TRUE;
      }

      if (!found)
      {
         int s = get_free_layer_stacknum();

         MENLayerrepStruct *l = new MENLayerrepStruct;
         layerreparray.SetAtGrow(layerrepcnt, l);
         layerrepcnt++;
         l->setName(mentorLayerArray[i]->newName);
         l->stacking_num = s;
         l->type = "user";
         l->color = 7;
         l->width = 1; 
         l->fill = 0;
         l->patt = 0;  
         l->trans = 0;
         l->path = 0;
         l->space = 0;
         l->style = 1; 
         l->text = 0;
         l->pen = 1;
         l->hilite = 0;
         l->protect = 0;
         l->select = 0;
      }
   }
}

/****************************************************************************/
/*
*/
static UINT64 get_padstacklayermap(const char *padstackName, int mirror)
{
   for (int i=0; i<padstackcnt; i++)
   {
      if (!strcmp(padstackarray1[i]->padstackname, padstackName))
      {
         if (mirror)
            return padstackarray1[i]->mirrormap;
      
         return padstackarray1[i]->layermap;
      }
   }
   return ALL_INT64;
}

/****************************************************************************/
/*
*/
static const char *get_usedpadname(const char *p)
{
   int   i;

   for (i=0;i<padstackcnt;i++)
   {
      if (!strcmp(padstackarray1[i]->padstackname,p))
      {
         return padstackarray1[i]->usedpadname;
      }
   }
   return p;
}

/****************************************************************************/
/*
   return NULL if not visible
*/
static const char *Layer_Mentor(int l)
{
   int   i;

   for (i=0;i<mentorLayerArrayCount;i++)
   {
      if (mentorLayerArray[i]->on == 0)   continue;   // not visible
      if (mentorLayerArray[i]->layerindex == l)
         return mentorLayerArray[i]->newName;
   }

   return NULL;
}

//--------------------------------------------------------------
static void write_shapeattributes(FILE *fp,CAttributes* map)
{
   WORD keyword;
   Attrib* attrib;

   if (map == NULL)  return;
   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      CString  key_word;
      key_word = MentorCheckName('a',doc->getKeyWordArray()[keyword]->out);

      switch (attrib->getValueType())
      {
         case VT_INTEGER:
            //fprintf(fp,"ATTRIBUTE CAD %s %d\n", doc->KeyWordArray[keyword]->out, attrib->getIntValue());
         break;
         case VT_UNIT_DOUBLE:
         case VT_DOUBLE:
            //fprintf(fp,"ATTRIBUTE CAD %s %lg\n", doc->KeyWordArray[keyword]->out, attrib->getDoubleValue());
         break;
         case VT_STRING:
         {
#if CamCadMajorMinorVersion > 406  //  > 4.6
            char *temp = STRDUP(attrib->getStringValue());
#else
            char *temp = STRDUP(doc->ValueArray[attrib->getStringValueIndex()]);
#endif
            
            char *tok = strtok(temp,"\n");
            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  //fprintf(fp,"ATTRIBUTE CAD %s %s\n", doc->KeyWordArray[keyword]->out,tok);   // multiple values are delimited by \n
               }
               else
               {
                  //fprintf(fp,"ATTRIBUTE CAD %s ?\n", doc->KeyWordArray[keyword]->out);
               }
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   return;
}

//--------------------------------------------------------------
// mentor rule:
// a string can not contain " and ' string
// if string contains a " than the attribute is enbedded into ' '
// else it is enbedded into "
//
static CString mentor_attrib_string(const char *l)
{
   CString  a;
   int      single_quote = 0, double_quote = 0;

   a = l;
   for (int i=0; i<(int)strlen(l); i++)
   {
      if (l[i] == '"')  double_quote++;
      if (l[i] == '\'') single_quote++;
   }

   // change is to double quote only
   if (double_quote && single_quote)
   {
      fprintf(flog, "Replace ' with \" in attribute value %s\n", a);
      display_error++;
      a.Replace("'","\"");
      single_quote = 0;
   }

   if (single_quote)
   {
      CString  tmp;
      tmp = '"';
      tmp += a;
      tmp += '"';
      a = tmp;
   }
   else
   {
      CString  tmp;
      tmp = '\'';
      tmp += a;
      tmp += '\'';
      a = tmp;
   }

   return a;
}

//--------------------------------------------------------------
static void write_compattributes(FILE *fp,CAttributes* map, const char *ident)
{
   WORD     keyword;
   Attrib* attrib;
   int      written = FALSE;

   if (map == NULL)  return;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      CString  key_word;
      key_word = mentor_attrib_string(MentorCheckName('a',doc->getKeyWordArray()[keyword]->out));

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         {
            CString l;
            l = get_attvalue_string(doc, attrib);
            written +=fprintf(fp," -\n%s(%s,\"%s\")", ident,  key_word, l);
         }
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         {
            CString l;
            l = get_attvalue_string(doc, attrib);
            written +=fprintf(fp," -\n%s(%s,\"%s\")", ident, key_word, l);
         }
         break;
      case VT_STRING:
         {
#if CamCadMajorMinorVersion > 406  //  > 4.6
            char *temp = STRDUP(attrib->getStringValue());
#else
            char *temp = STRDUP(doc->ValueArray[attrib->getStringValueIndex()]);
#endif
            
            char *tok = strtok(temp,"\n");
            while (tok)
            {
               // the output quotes ' or " are made in mentor_attrib_string
               written +=fprintf(fp," -\n%s(%s,%s)", ident, key_word, mentor_attrib_string(tok));
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   return;
}

/******************************************************************************
* load_MENTORsettings
*/
static int load_MENTORsettings(CString settingsFileName)
{
   write_area_prt = 0;
   mentorlayer_file = "";

   FILE *fSet = fopen(settingsFileName, "rt");
   if (!fSet)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", settingsFileName);
      MessageBox(NULL, tmp, "MENTOR Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   char line[255];
   char *lp;
   while (fgets(line, 255, fSet))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         // here commands
         if (!STRCMPI(lp,".LAYERFILE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            mentorlayer_file =  getApp().getUserPath() + lp;
         }
         else if (!STRCMPI(lp,".WRITE_AREA_PRT"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            if (!STRNICMP(lp,"Y",1))
               write_area_prt = 1;
         }
      }
   }

   fclose(fSet);

   return 1;
}

//--------------------------------------------------------------
static   int   get_layernr_from_index(int index)
{
   int   i;
   MentorLayerStruct *tmp;

   for (i=0;i<mentorLayerArrayCount;i++)
   {
      tmp = mentorLayerArray[i];
      if (mentorLayerArray[i]->layerindex == index)
         return mentorLayerArray[i]->stackNum;
   }

   return -99;
}

///--------------------------------------------------------------
static   const char *get_layername_from_stacknum(int stack)
{
   MentorLayerStruct *tmpLayer;

   for (int i=0;i<mentorLayerArrayCount;i++)
   {
      tmpLayer = mentorLayerArray[i];
      if (tmpLayer->stackNum == stack)
         return tmpLayer->newName.GetBuffer(0);
   }

   return "";
}

/*****************************************************************
*  get_layer_index_from_stacknum()
*  Parameters: Takes a layer stack number.
*  Returns: Returns the layer index of the matching layer.
*/
static int get_layer_index_from_stacknum(int stack)
{
   MentorLayerStruct *tmpLayer;

   for (int i=0;i<mentorLayerArrayCount;i++)
   {
      tmpLayer = mentorLayerArray[i];
      if (tmpLayer->stackNum == stack)
         return tmpLayer->layerindex;
   }

   return -1;
}


//--------------------------------------------------------------
static int get_powernet_layer_stacknum(const char *n, int *ptr)
{
   LayerStruct *layer;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayer(j);
      if (layer == NULL)   continue; // could have been deleted.

      Attrib* attrib;

      if (attrib = is_attvalue(doc, layer->getAttributesRef(), LAYATT_NETNAME, 2))
      {
         if (attrib->getValueType() ==  VT_STRING)
         {
#if CamCadMajorMinorVersion > 406  //  > 4.6
            char *temp = STRDUP(attrib->getStringValue());
#else
            char *temp = STRDUP(doc->ValueArray[attrib->getStringValueIndex()]);
#endif
            
            char *tok = strtok(temp,"\n");

            while (tok)
            {
               if (!strcmp(tok,n))
               {
                  free(temp);
                  *ptr = j;
                  return layer->getElectricalStackNumber();
               }

               tok = strtok(NULL,"\n");
            }

            free(temp);
         }
      }
   }

   CString  tmp;
   tmp.Format("Power Net [%s] is not assigned to a Layer", n);
   ErrorMessage(tmp,"Layer Stack Number Error");
   *ptr = -1;

   return -1;
}

//--------------------------------------------------------------
static int get_mlarr_from_stacknum(int stacknum)
{
   int   i;

   for (i=0;i<mentorLayerArrayCount;i++)
   {
      if (mentorLayerArray[i]->stackNum == stacknum)
         return i;
   }

   return -99;
}

/******************************************************************************
* net_layerlist
*/
static int net_layerlist(FileStruct *file)
{

   NetStruct *net;
   POSITION  netPos;
   Attrib   *a;
   int      powercnt = 0;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      // if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;
      if (a =  is_attvalue(doc, net->getAttributesRef(), ATT_POWERNET, 2))
      {
         int   j;
         int   stackNum = get_powernet_layer_stacknum(net->getNetName(), &j);

         if (doc->getLayer(j) != NULL && doc->getLayer(j)->getLayerType() == LAYTYPE_POWERNEG)
         {
            // this has only 1 netname assigned.
            int m = get_mlarr_from_stacknum(stackNum);
            if (m < 0)
            {
               // can not find stacknum
            }
            else
            {
               mentorLayerArray[m]->type = 'P';
               mentorLayerArray[m]->oldName = net->getNetName();
               mentorLayerArray[m]->newName.Format("POWER_%d", ++powercnt);
               continue;
            }
         }

         // here generate a new powerlayer and the XRF count will change

         //mentorLayerArray.SetSizes
         MentorLayerStruct *ml = new MentorLayerStruct;
         ml->stackNum = stackNum;
         ml->layerindex = j;  // index into doc->LayerArray
         ml->xrf = 0;

         if (ml->stackNum < 0)   // if a powernet does not have a layer assigned, make a new XRF layer
         {
            ml->stackNum = 0;
            ml->on = TRUE;
         }
         else
         {
            ml->on = TRUE;
         }  
         ml->type = 'P';
         ml->oldName = net->getNetName();
         ml->newName.Format("POWER_%d", ++powercnt);
         mentorLayerArray.SetAtGrow(mentorLayerArrayCount++, ml);
      }
   }

   return 1;
}

/******************************************************************************
* assign_xrf_number
// the included e-mail explains how the stacknumber and xrf number work.
*/
static int assign_xrf_number()
{
   int xrf = 0;

   // Get maxStackNum
   int maxStackNum = 0;
   for (int layerNum=0; layerNum<mentorLayerArrayCount; layerNum++)
   {
      MentorLayerStruct* mentorLayer = mentorLayerArray[layerNum];

      if (mentorLayer->stackNum > maxStackNum)
         maxStackNum = mentorLayer->stackNum;
   }

   // loop stackNums
   for (int stackNum=1; stackNum<=maxStackNum; stackNum++)
   {
      CString prevLayer;

      for (int i=0; i<mentorLayerArrayCount; i++)
      {
         MentorLayerStruct *mentorLayer = mentorLayerArray[i];

         if (mentorLayer->stackNum != stackNum)
            continue;

         if (!prevLayer.Compare(mentorLayer->newName)) // change xrf if newname is different.
            mentorLayer->xrf = xrf;
         else
            mentorLayer->xrf = ++xrf;

         prevLayer = mentorLayer->newName;
      }
   }

   maxXRF = xrf;

   return 1;
}

/******************************************************************************
* update_unassigned_xrflayers
// the included e-mail explains how the stacknumber and xrf number work.
*/
static int update_unassigned_xrflayers()
{
   for (int i=0; i<mentorLayerArrayCount; i++)
   {
      MentorLayerStruct *ml = mentorLayerArray[i];

      if (ml->type != 'P') continue;   // here is a power layer   
      if (ml->xrf)         continue;   // already has an XRF layer assigned
      if (ml->newName.Left(6) != "POWER_")   
                           continue;
      // stacknr
      CString  pcnt = ml->newName;
      pcnt.Delete(0, 6); // delete the POWER_ part to get to the number
      int powercnt = atoi(pcnt);
      ml->xrf = max_signalcnt + powercnt;
   }
   
   return 1;
}


/******************************************************************************
* get_other_via_layer
*/  
static int get_other_via_layer(UINT64 layerMap, int layerStackNum)
{
   UINT64 mapForLastStackNum = 1L << (layerStackNum-1);
   UINT64 layerMapWithoutLastStackNum = layerMap & ~mapForLastStackNum;

   if (layerMapWithoutLastStackNum)
   {
      if (layerStackNum > max_signalcnt/2)   // layerStackNum is higher than half of the maxlayer -> find lower one
      {  
         for (int i=1; i<=max_signalcnt; i++)
         {
            UINT64 m3 = 1L << (i-1);
            if (layerMapWithoutLastStackNum & m3)
               return i;
         }
      }
      else
      {
         for (int i=max_signalcnt; i>layerStackNum; i--)
         {
            UINT64 m3 = 1L << (i-1);
            if (layerMapWithoutLastStackNum & m3)
               return i;
         }
      }
   }
   else
   {
      // this is only on bottom layer
   }

   return layerStackNum;
}

/******************************************************************************
* get_xrf_from_stacknum
*/
static int get_xrf_from_stacknum(int stackNum)
{
   for (int i=0; i<mentorLayerArrayCount; i++)
   {
      MentorLayerStruct *mentorLayer = mentorLayerArray[i];

      if (mentorLayer->stackNum == stackNum)
         return mentorLayer->xrf;
   }

   CString tmp;
   tmp.Format("XRF layer not found for Stacknumber [%d]", stackNum);
   ErrorMessage(tmp, "Internal Error");

   return -1;
}

/******************************************************************************
* get_stacknum_from_xrf
*/
static int get_stacknum_from_xrf(int layerXRF)
{
   for (int i=0; i<mentorLayerArrayCount; i++)
   {
      MentorLayerStruct *mentorLayer = mentorLayerArray[i];

      if (mentorLayer->xrf == layerXRF)
         return mentorLayer->stackNum;
   }

   CString tmp;
   tmp.Format("Stacknumber layer not found for XRF [%d]", layerXRF);
   ErrorMessage(tmp, "Internal Error");

   return -1;
}

//-------------------------------------------------------------------
void Write_Traces(FILE *stream, CResultList *resultList, const char *netname, double scale)
{
   // int      viaendlayer = 0; This no longer used

   if (resultList == NULL) return;

   Net_Path *node;
   /* struct Net_Path
      {
         char type; // VIA_NODE, SEGMENT_NODE, ENDPOINT_NODE
         int index_id;
         BOOL reverse; // segment goes from p2 to p1
      }; */
   CPathList *path;

   /* If the netname was used as a power net and is 
   associated with a POWER_# layer we assign viaendlayer
   to the xrf of the POWER_# layer. This is a bad idea as 
   some vias may not have access to the POWER_# layer 
   (i.e. Blind or Buried vias, testpoint vias). 
   This code has been commented to remove this problem.
   
   //endlayer if a via ends a line.
   // check if netname is a mentor.in layer
   for (int l=0;l<mentorLayerArrayCount;l++)
   {
      if (!strcmp(mentorLayerArray[l]->oldName,netname))
         viaendlayer = mentorLayerArray[l]->xrf;
   }
   */

   // loop result paths
   POSITION resPos, pathPos;
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      double startx, endx, starty, endy;
      int    startlayer, endlayer;

      path = resultList->GetNext(resPos)->path;


      if (path->GetCount() == 1 && path->GetHead()->type == ENDPOINT_NODE)
      {
         //fprintf(stream, "# Broken Net!  Moving this pin over.\n");
         continue;
      }

      // loop nodes of this path
      pathPos = path->GetHeadPosition();

      fprintf(stream, "NET \'%s\'\n", netname);

      // is Endpoint start and Endpoint end -> Wir
      // if Endpoint start or Endpoint end -> Antenna
      // else Incomplete.
      int ep = 0; // endpoint count
      int sp = 0; // seg count  (vertex and the first count start and end)
      int vp = 0; // via count

      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
               ep++;
            break;
            case SEGMENT_NODE:
            {
               Net_Segment *seg = segmentArray->GetAt(node->index_id);
               
               // node->index_id, seg->x1, seg->y1, seg->x2, seg->y2, seg->layer, seg->widthIndex,
               // (node->reverse?'T':'f')); 

               if (sp == 0) // first segment
               {
                  if (node->reverse)
                  {
                     startx = seg->x2;
                     starty = seg->y2;
                     endx = seg->x1;
                     endy = seg->y1;
                  }
                  else
                  {
                     startx = seg->x1;
                     starty = seg->y1;
                     endx = seg->x2;
                     endy = seg->y2;
                  }
                  endlayer = startlayer = get_xrf_from_stacknum(seg->layer);
               }
               else
               {
                  if (node->reverse)
                  {
                     endx = seg->x1;
                     endy = seg->y1;
                  }
                  else
                  {
                     endx = seg->x2;
                     endy = seg->y2;
                  }
                  endlayer = get_xrf_from_stacknum(seg->layer);
               }
               sp++;
            }
            break;
            case VIA_NODE:
               Net_Via *via = viaArray->GetAt(node->index_id);
               if (sp == 0 && vp == 0)
               {
                  startx = via->x;
                  starty = via->y;
                  // startlayer for empty, free layer
                  endlayer = get_xrf_from_stacknum(get_other_via_layer(via->layermap, 1));
                  startlayer = get_xrf_from_stacknum(get_other_via_layer(via->layermap, max_signalcnt));
               }
               endx =   via->x;
               endy =   via->y;
               vp++;
            break;
         }
      }

      if (sp == 0 && vp == 0) continue;   // no trace, no via

      // path->getCount is 
      // Endpoint, seg ,seg, via, seg,seg, endpoint
      // fprintf(stream,"# %s Endpoint %d Vias %d Seg %d\n",typ,ep, vp, sp);

      switch (ep)
      {
         case 2:
            // WIR vertex startx starty endx endy
            fprintf(stream,"# Two Pins: Endpoints %d Vias %d Segments %d\n",ep, vp, sp);
            fprintf(stream,"WIR %d %1.*lf %1.*lf %1.*lf %1.*lf\n",sp+vp+1,
               output_units_accuracy, startx, 
               output_units_accuracy, starty, 
               output_units_accuracy, endx, 
               output_units_accuracy, endy);
         break;
         case 1:
            // WIR vertex startx starty endx endy
            //fprintf(stream,"ANT %d %1.4lf %1.4lf %d %1.4lf %1.4lf %d\n",sp+vp+1,
            //    startx, starty, startlayer, endx, endy, endlayer );

            // a single via
            if (ep == 1 && vp == 1 && sp == 0)
            {
               endx = startx;
               endy = starty;
            }

            fprintf(stream,"# One Pin: Endpoints %d Vias %d Segments %d\n",ep, vp, sp);
            fprintf(stream,"WIR %d %1.*lf %1.*lf %1.*lf %1.*lf\n",sp+vp+1,
               output_units_accuracy, startx, 
               output_units_accuracy, starty, 
               output_units_accuracy, endx, 
               output_units_accuracy, endy);
         break;
         default:
            // INC vertex startx starty layerlayer endx endy endlayer
            fprintf(stream,"# No Pins: -> Endpoints %d Vias %d Segments %d\n",ep, vp, sp);
            if (sp == 0 && vp == 1)
            {
               fprintf(stream,"# No Pin: Endpoints %d Vias %d Segments %d\n",ep, vp, sp);
               fprintf(stream,"WIR %d %1.*lf %1.*lf %1.*lf %1.*lf\n",sp+vp+1,
                  output_units_accuracy, startx, 
                  output_units_accuracy, starty, 
                  output_units_accuracy, endx, 
                  output_units_accuracy, endy);
            }
            else
            {
               fprintf(stream,"INC %d %1.*lf %1.*lf %d %1.*lf %1.*lf %d\n",sp+vp+1, 
                  output_units_accuracy, startx, 
                  output_units_accuracy, starty, startlayer, 
                  output_units_accuracy, endx, 
                  output_units_accuracy, endy, endlayer);
               //fprintf(stream,"# No Pins: -> Endpoints %d Vias %d Segments %d\n",ep, vp, sp);
               //fprintf(stream,"WIR %d %1.4lf %1.4lf %1.4lf %1.4lf\n",sp+vp+1,
               // startx, starty, endx, endy);
            }
         break;
      }


      // loop nodes of this path
      pathPos = path->GetHeadPosition();
      int first = TRUE;
      int lastlayer = 0;
      double lastx, lasty;
      
      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
            case ENDPOINT_NODE:
#ifdef DUMP
               fprintf(stream, "# Endpoint ID=%d\n", node->index_id);
#endif
            break;
            case SEGMENT_NODE:
            {
#ifdef DUMP
               fprintf(stream, "# Segment index=%d\n", node->index_id);
#endif
               Net_Segment *seg = segmentArray->GetAt(node->index_id);
               double lineWidth = doc->getWidthTable()[seg->widthIndex]->getSizeA()*scale;
               if (first)
               {
                  if (node->reverse)
                  {
                     fprintf(stream,"SEG %1.*lf %1.*lf %d ", 
                        output_units_accuracy, seg->x2, output_units_accuracy, seg->y2, 
                        get_xrf_from_stacknum(seg->layer));
                     lastx = seg->x2;
                     lasty = seg->y2;
                  }
                  else
                  {
                     fprintf(stream,"SEG %1.*lf %1.*lf %d ", 
                        output_units_accuracy, seg->x1, output_units_accuracy, seg->y1, 
                        get_xrf_from_stacknum(seg->layer));
                     lastx = seg->x1;
                     lasty = seg->y1;
                  }
               }
               
               fprintf(stream," %1.*lf\n",output_units_accuracy, lineWidth);

               if (node->reverse)
               {
                  fprintf(stream,"SEG %1.*lf %1.*lf %d ", 
                     output_units_accuracy, seg->x1, output_units_accuracy, seg->y1, 
                     get_xrf_from_stacknum(seg->layer));
                  lastx = seg->x1;
                  lasty = seg->y1;
               }
               else
               {
                  fprintf(stream,"SEG %1.*lf %1.*lf %d ", 
                     output_units_accuracy, seg->x2, output_units_accuracy, seg->y2, 
                     get_xrf_from_stacknum(seg->layer));
                  lastx = seg->x2;
                  lasty = seg->y2;
               }
               lastlayer = get_xrf_from_stacknum(seg->layer);
#ifdef DUMP
               fprintf(stream, "# %d Segment (x1=%+lf, y1=%+lf) (x2=%+lf, y2=%+lf) l=%d w=%d r=%c\n", 
                  node->index_id, seg->x1, seg->y1, seg->x2, seg->y2, seg->layer, seg->widthIndex,
                  (node->reverse?'T':'f')); 
#endif
               first = FALSE;
            }
            break;
            case VIA_NODE:
            {
               // a via spans from a lastlayer to a tolayer
#ifdef DUMP
               fprintf(stream, "# Via index=%d\n", node->index_id);
#endif
               Net_Via *via = viaArray->GetAt(node->index_id);
#ifdef DUMP
               fprintf(stream, "# %d Via (x=%+lf, y=%+lf) lm=%ul\n", 
                  node->index_id, via->x, via->y, via->layermap); 
#endif
               // get node to find tolayer
               if (pathPos)
               {
                  node = path->GetAt(pathPos);
                  if (node->type == SEGMENT_NODE)
                  {
                     Net_Segment *seg = segmentArray->GetAt(node->index_id);
                     lastlayer = get_xrf_from_stacknum(seg->layer);
                  }
               }
               else
               {
                  // the via was the last element in a net.
                  // Code commented here reflects removal of the viaendlayer variable usage.
                  // if (viaendlayer == 0) // this is the layer for power/ground planes.
                     lastlayer = get_xrf_from_stacknum(get_other_via_layer(via->layermap, get_stacknum_from_xrf(lastlayer)));
                  // else
                     // lastlayer = viaendlayer; // vialayer is xrf
               }

               if (first)
               {
                  /* This code commented to remove via tie connection to POWER layers
                  if the via is the first entity in a routed segment.

                  int   vlayer = viaendlayer;
                  if (viaendlayer == 0)
                  {
                     vlayer = startlayer;
                  }
                  
                  And is replaced with the following line */

                  int   vlayer;
                  vlayer = startlayer;

                  // here is starts with a via - must be from via end layer !
                  fprintf(stream,"SEG %1.*lf %1.*lf %d ",
                     output_units_accuracy, via->x, output_units_accuracy, via->y, vlayer);
                  lastx = via->x;
                  lasty = via->y;
               }
               
               CString v;
               v.Format("VIA_%s",viaarray[via->id]->vianame);  // via must be different than padstacks.
               fprintf(stream,"%s P\n",MentorCheckName('p',v));// write via instead of width

               // here need to make sure that the viacoo matches the last wire coo
               if (fabs(lastx - via->x) > MEN_SMALLNUMBER || fabs(lasty - via->y) > MEN_SMALLNUMBER)
               {
                  fprintf(flog,"Via Koo %1.*lf %1.*lf and Segment Koo %1.*lf %1.*lf is off\n",
                        output_units_accuracy, via->x, output_units_accuracy, via->y, 
                        output_units_accuracy, lastx, output_units_accuracy, lasty);
                  display_error++;
               }

               fprintf(stream,"SEG %1.*lf %1.*lf %d ",
                  output_units_accuracy, lastx, output_units_accuracy, lasty,lastlayer);
               first = FALSE;
            }
            break;
         }
      }
      fprintf(stream, " 0\n"); // last width
   }
   return;
}

//--------------------------------------------------------------
// this gets ther component_placement_outline 
int MENTOR_OutlineSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary,
      int gr_class)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_TEXT)    continue;
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         // never do etch
         if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

         const char  *l = Layer_Mentor(layer);
         if (l == NULL)                               continue;

         if (STRCMPI(l, "COMPONENT_PLACEMENT_OUTLINE"))  // if the layername is component placement outline
         {

            if (primary)
            {
               if (np->getGraphicClass() != gr_class)  
                  continue;
            }
            else
            {
               // do it
            }
         }
         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            continue;

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         point2.x = np->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;

         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         double block_rot = rotation + np->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               block_layer = insertLayer;
            else
               block_layer = np->getLayerIndex();

            found += MENTOR_OutlineSHAPEData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, primary,gr_class);
         } // end else not aperture
      } // if INSERT

   } // end MENTOR_OutlineSHAPEData */

   return found;
}

//--------------------------------------------------------------
int MENTOR_OutlineBOARDData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary,
      int gr_class)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_TEXT)    continue;
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // never do etch
         if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

         if (primary)
         {
            if (np->getGraphicClass() != gr_class)  
               continue;
         }
         else
         {
            // do it
         }
         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)            continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT)  continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)       continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)        continue;

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         point2.x = np->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;

         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         double block_rot = rotation + np->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               block_layer = insertLayer;
            else
               block_layer = np->getLayerIndex();

            found += MENTOR_OutlineBOARDData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, primary,gr_class);
         } // end else not aperture
      } // if INSERT

   } // end MENTOR_OutlineBOARDData */

   return found;
}

/*****************************************************************************/
/*
*/
static int write_easyoutline(FILE *wfp, CPntList *pp, const char *layer, const char *sublayer, double scale)
{
   CPnt *p;
   
   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;
   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt     *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
            ErrorMessage("Poly Bulge data error !");
            else
         p2 = pp->GetAt(pos);
                     
         ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         ppolycnt = 255;
         // start center
         ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(5));  // 5 degree
         for (i=0;i<ppolycnt;i++)
         {
            polyarray.SetAtGrow(polycnt,ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2   p2;
         p2.x = p->x;
         p2.y = p->y;
         p2.bulge = p->bulge;

         polyarray.SetAtGrow(polycnt,p2);
         polycnt++;
      }
   }

   int done = FALSE,i;

   while (!done)
   {
      done = TRUE;
      for (i=1;i<polycnt;i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);
         if (fabs(p1.x - p2.x) < MEN_SMALLNUMBER && 
             fabs(p1.y - p2.y) < MEN_SMALLNUMBER)
         {
               polyarray.RemoveAt(i);
               done = FALSE;
               polycnt--;
               break;
         }
      }
   }

/*
   for (i=0;i<polycnt;i++)
   {
      Point2 p;
      p = polyarray.ElementAt(i);  
      fprintf(wfp,"outline %d [%lg,%lg]\n",i, p.x, p.y);
   }
*/

   //fprintf(wfp,"\t%d\n",polycnt);
   double keepoutScale = 1.0;
   CString layerName = layer;
   layerName.MakeUpper();
   if (layerName.Find("KEEPOUT") > 0) 
      keepoutScale = scale;  //This is not a good solution, but the keepout polys didn't get converted to the current unit

   for (i=0;i<polycnt;i++)
   {
      Point2 p;
      p = polyarray.ElementAt(i); 
      

      if (i == 0)
         fprintf(wfp,"$$initial([%1.*lf,%1.*lf], , @nosnap );\n",
            output_units_accuracy, p.x * keepoutScale, output_units_accuracy, p.y * keepoutScale);
      else     
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
            output_units_accuracy, p.x * keepoutScale, output_units_accuracy, p.y * keepoutScale);
   }
   if (polycnt)
      fprintf(wfp,"$$attribute( \"%s\", \"%s\", @mark, @scale );\n",layer, sublayer);

   if (SHOWOUTLINE)
   {
      doc->PrepareAddEntity(cur_file);       
      int l = Graph_Level("DEBUG","",0);
      DataStruct *d = Graph_PolyStruct(l,0L, FALSE);  // poly is always with 0

      Graph_Poly(NULL,0, 0,0, 0);

      for (i=0;i<polycnt;i++)
      {
         Point2 p;
         p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge);    // p1 coords
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_area(FILE *wfp, CPoly *pp, double insert_x, double insert_y, double scale, 
                      int mirror,   Mat2x2 m, const char *layer)
{
   if (pp == NULL)
   {
      return 0;
   }

   CString  menlay;
   menlay.Format("A_%s", layer);

   wmen_Graph_Level(menlay);

   if (pp->isHatchLine())       fprintf(wfp,"// Hatchline\n");
   else
   if (pp->isVoid())            fprintf(wfp,"// VoidPoly\n");
   else
   if (pp->isFilled())          fprintf(wfp,"// FillPoly\n");
   else
   if (pp->isThermalLine())     fprintf(wfp,"// ThermalLine\n");
   else
   if (pp->isFloodBoundary())   fprintf(wfp,"// Boundary\n");

   CPnt     *pnt;
   POSITION pntPos;

   Point2 *points;
   int    cnt = 0;

   pntPos = pp->getPntList().GetHeadPosition();
   while (pntPos != NULL)
   {
      pnt = pp->getPntList().GetNext(pntPos);
      cnt++;
   }  // while

   points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
         
   Point2 p2;
   cnt = 0;
   pntPos = pp->getPntList().GetHeadPosition();
   while (pntPos != NULL)
   {
      pnt = pp->getPntList().GetNext(pntPos);
      p2.x = pnt->x * scale;
      if (mirror) p2.x = -p2.x;
      p2.y = pnt->y * scale;
      p2.bulge = pnt->bulge;
      TransPoint2(&p2, 1, &m, insert_x, insert_y);
      // here deal with bulge
      points[cnt] = p2;
      cnt++;
   }
   wmen_Graph_Polyline(points, cnt, 0, 0);   // never filled, never closed
   free(points);

   return 1;
}

//--------------------------------------------------------------
// This has to be ordered from POWER_1 to ..n
//
static int write_powernets(FILE *wfp)
{
   int   max_power = 0;
   int   i, ii;
   int   found = 0;

   for (i=0;i<mentorLayerArrayCount;i++)
   {
      if (mentorLayerArray[i]->type == 'P')
         found++;
   }

   fprintf(wfp,"$$attribute( \"POWER_NET_NAMES\", \"");

   if (found)
   {
      CString  p;
      int      first = TRUE;
      for (ii=1;ii<=found;ii++)
      {
         p.Format("POWER_%d", ii);
         for (i=0;i<mentorLayerArrayCount;i++)
         {
            if (mentorLayerArray[i]->newName.CompareNoCase(p) == 0)
            {
               if (!first)
                  fprintf(wfp,",");
               fprintf(wfp,"\\\"%s\\\"",mentorLayerArray[i]->oldName);
               first = FALSE;
            }
         }
      }
   }
   else
   {
      fprintf(wfp,"NO_POWER_LAYERS");
   }

   fprintf(wfp,"\");\n");

   return 1;
}

//--------------------------------------------------------------
static int used_as_via(const char *v)
{
   int   i;

   for (i=0;i<viacnt;i++)
   {
      if (viaarray[i]->vianame.Compare(v) == 0)
         return 1;
   }

   return 0;
}

//---------------------------------------------------------------
static int write_padstack(FILE *wfp, CDataList *DataList,  double scale , double rot, 
                          const char *origpadname, const char *padname)
{
   double   drillsize;
   int top = FALSE, inner = FALSE, bot = FALSE;

   CUIntArray padElecLayerList;
   padElecLayerList.SetSize(100,100);
   int  padElecLayerCount = 0;

   int typ = MENTOR_GetPADSTACKData(wfp, DataList, -1, &drillsize, scale, &top, &inner, &bot, &padElecLayerList, padElecLayerCount);

   if (drillsize > 0) 
   {
      fprintf(wfp,"$$circle( \"POWER\", 0.0, 0.0, %1.*lf, 0.0);\n",
         output_units_accuracy, drillsize*1.5);
      fprintf(wfp,"$$attribute( \"TERMINAL_DRILL_SIZE\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
         output_units_accuracy, drillsize);
      typ |= 0x4;
   }

   int found = MENTOR_WritePADSTACKData(wfp, DataList, 0.0, 0.0, rot, 0, scale, 0, -1, typ);

   if (!found && typ == 0)
   {
      fprintf(flog,"Padstack [%s] has no Graphic\n",padname);
      fprintf(wfp,"$$circle( \"SIGNAL\", 0.0, 0.0, %1.*lf, 0.0);\n", output_units_accuracy, one_mil*2);
      fprintf(wfp,"$$circle( \"POWER\", 0.0, 0.0, %1.*lf, 0.0);\n",  output_units_accuracy, one_mil*2);
      fprintf(wfp,"$$attribute( \"TERMINAL_DRILL_SIZE\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
         output_units_accuracy, one_mil);
      typ = 0x4;
   }

   // typ 0x1 top
   // typ 0x2 bottom
   // typ 0x4 drill

   if (top && inner && bot || !top && !inner && !bot)
   {
      fprintf(wfp,"$$attribute( \"TERMINAL_THRUHOLE_DEFINITION\", \"\");\n");
   }
   else if (top && !inner && !bot || !top && !inner && bot)
      fprintf(wfp,"$$attribute( \"TERMINAL_SURFACE_DEFINITION\", \"\");\n");
   else
      fprintf(wfp,"$$attribute( \"TERMINAL_BLIND_DEFINITION\", \"\");\n");

   MENPadStackStruct *p = new MENPadStackStruct;
   p->padstackname = origpadname;      
   p->usedpadname = padname;
   p->top = top;
   p->inner  = inner;
   p->bot = bot;
   p->padElecLayerCount = padElecLayerCount;

   // save padstack's electrical layers in this array
   for (int i = 0; i<padElecLayerCount; i++)
      p->padElecLayerList.SetAtGrow(i,padElecLayerList.ElementAt(i));

   p->typ = typ;
   if (typ == 1)
   {
      UINT64 baseVal = 1L;
      p->layermap =  0x1;
      p->mirrormap = baseVal << (maxXRF-1);
   }
   else
   if (typ == 2)
   {
      UINT64 baseVal = 1L;
      p->layermap = baseVal << (maxXRF-1);
      p->mirrormap =  0x1;
   }
   else
   {
      p->layermap = ALL_INT64;
      p->mirrormap = ALL_INT64; 
   }

   if (p->used_as_via = used_as_via(origpadname))
   {
      // vias ???
      // testpoints
      CString  vianame;
      vianame.Format("VIA_%s",origpadname);  // via must be different than padstacks.
      wmen_Graph_CreateVia_On(MentorCheckName('p',vianame));

      found = MENTOR_WritePADSTACKData(wfp, DataList, 0.0, 0.0, rot, 0, scale, 0, -1, typ);
      if (!found)
      {
         fprintf(flog,"VIA Padstack [%s] has no Graphic\n",vianame);
      }
      // typ 0x1 top
      // typ 0x2 bottom
      // typ 0x4 drill
      if (top && inner && bot)
      {
         fprintf(wfp,"$$attribute( \"TERMINAL_THRUVIA_DEFINITION\", \"\");\n");
         fprintf(wfp,"$$circle( \"POWER\", 0.0, 0.0, %1.*lf, 0.0);\n",
            output_units_accuracy, drillsize*1.5);
         fprintf(wfp,"$$attribute( \"TERMINAL_DRILL_SIZE\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
            output_units_accuracy, drillsize);
      }
      else
      {
         fprintf(wfp,"$$attribute( \"TERMINAL_BURIED_VIA_DEFINITION\", \"\");\n");
         fprintf(wfp,"$$circle( \"POWER\", 0.0, 0.0, %1.*lf, 0.0);\n",
            output_units_accuracy, drillsize*1.5);
         fprintf(wfp,"$$attribute( \"TERMINAL_DRILL_SIZE\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
            output_units_accuracy, drillsize);
      }

   }

   padstackarray1.SetAtGrow(padstackcnt, p);
   padstackcnt++;                                      

   padElecLayerList.RemoveAll();

   return typ;
}

/******************************************************************************
* stackupname_already_written
// It is allowed to have multiple CAD layers mapping to SIGNAL_1 etc...
*/
static int stackupname_already_written(int mlptr)
{
   // hardcoded pads layers.
   if (mentorLayerArray[mlptr]->newName.Compare("PAD_1") == 0)
      return 1;

   if (mentorLayerArray[mlptr]->newName.Compare("PAD_2") == 0)
      return 1;

   for (int i=0; i<mlptr; i++)
   {
      if (mentorLayerArray[i]->newName.Compare(mentorLayerArray[mlptr]->newName) == 0)
         return 1;
   }

   return 0;
}

/******************************************************************************
* do_partfile
*/
static int do_partfile(FILE *wfp, FileStruct *file, double unitsFactor, double accuracy)
{
   CString  board_defaultpadstack = "";

   wmen_Graph_File_Open_Layout(wfp);                                                        
   wmen_Graph_Init(output_units_accuracy, MEN_SMALLNUMBER, output_units);

   viacnt = 0; // start viaarray
   get_vianames(&(file->getBlock()->getDataList()));

   // here write all blocks 
   // create_pin
   // create_via and make name different from Padstack used for PINS and VIAS.
   // create_generic
	int i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      
      if (block == NULL)   
         continue;

      if (block->getFlags() & BL_WIDTH)            
         continue;
      if (block->getFlags() & BL_TOOL)             
         continue;
      if (block->getFlags() & BL_APERTURE)         
         continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   
         continue;
   
      if (!(block->getFlags() & BL_FILE) || 
           (block->getFlags() & BL_FILE && doc->is_file_inserted(block->getBlockNumber()))) // fileblock in fileblock is now allowed.
      {
         switch (block->getBlockType())
         {
            case BLOCKTYPE_PADSTACK: 
            case BLOCKTYPE_TOOLING:        // Case dts0100469660 says treat tooling blocks exactly the same as padstack blocks
            {
               CString  padname;
               padname = get_uniquegeomname(block->getBlockNumber());

               wmen_Graph_CreatePin_On(MentorCheckName('p',padname));

               int typ = write_padstack(wfp, &(block->getDataList()),file->getScale() * unitsFactor, 0.0, 
                  block->getName(), padname);

               if (typ == 4)
               {
                  if (!strlen(board_defaultpadstack)) // make a thru hole pad the default.
                     board_defaultpadstack = padname;
               }
            }
            break;
            case BLOCKTYPE_PCBCOMPONENT:  
               // do nothing
            break;
            case BLOCKTYPE_TESTPOINT:  
               // do nothing
            break;
            case BLOCKTYPE_TOOLGRAPHIC:
               // do nothing
            break;
            case BLOCKTYPE_CENTROID:
               // do nothing
            break;
            default:
               // also fiducial, toolings
               // dimensions
               CString  n = get_uniquegeomname(block->getBlockNumber());
               //wmen_Graph_CreateGeneric_On(MentorCheckName('g', n));  No case#, Expedition/Boardstation transfer seems to want the next line, not this one
               wmen_Graph_CreateComponent_On(MentorCheckName('g', n));
               MENTOR_WriteGEOMASCIIData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1);
            break;
         }
      }  // if not PCB or PANEL
   } // while


   // must be after PADSTACK, because I need to know about smd padstacks.
   // create_component
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)         continue;
      
      // create necessary aperture blocks
      if (block->getFlags() & BL_TOOL)          continue;
      if (block->getFlags() & BL_APERTURE)      continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
   
      if (!(block->getFlags() & BL_FILE) || 
           (block->getFlags() & BL_FILE && doc->is_file_inserted(block->getBlockNumber()))) // fileblock in fileblock is now allowed.
      {
         switch (block->getBlockType())
         {
            case BLOCKTYPE_TESTPOINT:  
            {
               MENPinExtent   pext;
               pext.x2 = pext.y2 = -FLT_MAX;  // should be maxfloat
               pext.x1 = pext.y1 =  FLT_MAX;

               wmen_Graph_CreateComponent_On(MentorCheckName('g',block->getName()));
               int smd = MENTOR_WritePCBCOMPONENTData(wfp, block->getName(), &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1, &pext);

               // component placement outline
               Outline_Start(doc);
               int found = MENTOR_OutlineSHAPEData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_COMPONENT_BOUNDARY);
               if (!found)
               {
                  found = MENTOR_OutlineSHAPEData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_COMPOUTLINE);
                  if (!found)
                  {
                     // now look for every visible outline
                     int found = MENTOR_OutlineSHAPEData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                        file->getScale() * unitsFactor, 0, -1, FALSE, GR_CLASS_COMPOUTLINE);
                     if (!found)
                     {
                        fprintf(flog,"Did not find a Component Outline for [%s].\n",block->getName());
                        display_error++;

                        if ( pext.x1 > pext.x2)
                        {
                           // No pins found just make a dummy one.
                           fprintf(wfp,"$$initial([-0.1,-0.1], , @nosnap );\n");
                           fprintf(wfp,"$$terminal([0.1,-0.1] );\n");
                           fprintf(wfp,"$$terminal([0.1,0.1] );\n");
                           fprintf(wfp,"$$terminal([-0.1,0.1] );\n");
                           fprintf(wfp,"$$terminal([-0.1,-0.1] );\n");
                        }
                        else
                        {
                           // make pin extens
                           fprintf(wfp,"$$initial([%1.*lf,%1.*lf], , @nosnap );\n",
                              output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x2+(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x2+(one_mil*10), output_units_accuracy, pext.y2+(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y2+(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                        }
                        fprintf(wfp,"$$attribute( \"COMPONENT_PLACEMENT_OUTLINE\", \"\", @mark, @scale );\n");
                     }
                  }
               }
               int returnCode;
               if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "COMPONENT_PLACEMENT_OUTLINE", "", file->getScale() * unitsFactor))
               {
                  fprintf(flog, "Error in easy outline for [%s]\n", block->getName());
                  display_error++;
               }

               Outline_FreeResults();


               // ROUTING_KEEPOUT
               // VIA KEEPOUT

               // pin definitions
               // compheight
               // testpoint does not get a refdes
               // fprintf(wfp,"$$text( \"SILKSCREEN\", \"^$ref\", 0.0, 0.0, 0.1, @CC, 0, 0.7, 0.01, \"std\", \"None\", 0.0, 0.0 );\n");
               // here check for SMD switch
               if (smd)
               {
                  // here look if this is an SMD comp
                  fprintf(wfp,"$$attribute( \"COMPONENT_LAYOUT_TYPE\", \"surface\");\n");
               }
               // here check for component height
               fprintf(wfp,"$$attribute( \"COMPONENT_LAYOUT_SURFACE\", \"both\");\n");
               write_shapeattributes(wfp, block->getAttributesRef());
            }
            break;
            case BLOCKTYPE_PCBCOMPONENT:  
            {
               MENPinExtent   pext;
               pext.x2 = pext.y2 = -FLT_MAX;  // should be maxfloat
               pext.x1 = pext.y1 =  FLT_MAX;

               wmen_Graph_CreateComponent_On(MentorCheckName('g',block->getName()));
               int smd = MENTOR_WritePCBCOMPONENTData(wfp, block->getName(), &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1, &pext);

               MENTOR_WriteClassPCBCOMPONENTData(wfp, block->getName(), &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1);

               Attrib *a;
               if (a = is_attvalue(doc, block->getAttributesRef(), ATT_COMPHEIGHT, 2))
               {
                  CString  h;
                  h = get_attvalue_string(doc, a);
                  if (atof(h) > 0)
                     fprintf(wfp,"$$attribute( \"COMPONENT_HEIGHT\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
                        output_units_accuracy, atof(h)*unitsFactor);
               }

               DataStruct *d;
               double centroidx = 0, centroidy = 0;
               if ((d = centroid_exist_in_block(block)) != NULL)
               {
                  // here update position
                  centroidx = d->getInsert()->getOriginX();
                  centroidy = d->getInsert()->getOriginY();
               }

               fprintf(wfp,"$$attribute( \"COMPONENT_INSERT_CENTER\", \"\", , @scale , , [%1.*lf, %1.*lf]);\n",
                        output_units_accuracy, centroidx* file->getScale() * unitsFactor, output_units_accuracy, centroidx* file->getScale() * unitsFactor);

               // component placement outline
               Outline_Start(doc);
               int found = MENTOR_OutlineSHAPEData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_COMPONENT_BOUNDARY);
               if (!found)
               {
                  found = MENTOR_OutlineSHAPEData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                     file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_COMPOUTLINE);
                  if (!found)
                  {
                     // now look for every visible outline
                     int found = MENTOR_OutlineSHAPEData(wfp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 
                        file->getScale() * unitsFactor, 0, -1, FALSE, GR_CLASS_COMPOUTLINE);
                     if (!found)
                     {
                        fprintf(flog,"Did not find a Component Outline for [%s].\n",block->getName());
                        display_error++;

                        if ( pext.x1 > pext.x2)
                        {
                           fprintf(flog, "PCBCOMPONENT [%s] without pins ???\n", block->getName());
                           // No pins found just make a dummy one.
                           fprintf(wfp,"$$initial([-0.1,-0.1], , @nosnap );\n");
                           fprintf(wfp,"$$terminal([0.1,-0.1] );\n");
                           fprintf(wfp,"$$terminal([0.1,0.1] );\n");
                           fprintf(wfp,"$$terminal([-0.1,0.1] );\n");
                           fprintf(wfp,"$$terminal([-0.1,-0.1] );\n");
                        }
                        else
                        {
                           // make pin extens
                           fprintf(wfp,"$$initial([%1.*lf,%1.*lf], , @nosnap );\n",
                              output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x2+(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x2+(one_mil*10), output_units_accuracy, pext.y2+(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y2+(one_mil*10));
                           fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                              output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                        }
                        fprintf(wfp,"$$attribute( \"COMPONENT_PLACEMENT_OUTLINE\", \"\", @mark, @scale );\n");
                     }
                  }
               }
               int returnCode;
               if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "COMPONENT_PLACEMENT_OUTLINE", "", file->getScale() * unitsFactor))
               {
                  // use pin extens
                  fprintf(wfp,"$$initial([%1.*lf,%1.*lf], , @nosnap );\n",
                     output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                  fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                     output_units_accuracy, pext.x2+(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                  fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                     output_units_accuracy, pext.x2+(one_mil*10), output_units_accuracy, pext.y2+(one_mil*10));
                  fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                     output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y2+(one_mil*10));
                  fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
                     output_units_accuracy, pext.x1-(one_mil*10), output_units_accuracy, pext.y1-(one_mil*10));
                  fprintf(wfp,"$$attribute( \"COMPONENT_PLACEMENT_OUTLINE\", \"\", @mark, @scale );\n");
               }

               Outline_FreeResults();

               // pin definitions
               // compheight
               //                                                                       width/height ratio
               fprintf(wfp,"$$text( \"SILKSCREEN\", \"^$ref\", 0.0, 0.0, %1.*lf, @CC, 0, 0.7, %1.*lf, \"std\", \"None\", 0.0, 0.0 );\n",
                  output_units_accuracy, one_mil*50, output_units_accuracy, one_mil*10);
               // here check for SMD switch
               if (smd)
               {
                  // here look if this is an SMD comp
                  fprintf(wfp,"$$attribute( \"COMPONENT_LAYOUT_TYPE\", \"surface\");\n");
               }
               fprintf(wfp,"$$attribute( \"COMPONENT_LAYOUT_SURFACE\", \"both\");\n");
               // here check for component height
               write_shapeattributes(wfp, block->getAttributesRef());
            }
            break;
         }
      }  // if not PCB or PANEL
   } // while

   // here do the rotated padstacks
   for (i=0;i<padrotcnt;i++)
   {
      fprintf(flog,"Padstack [%s] Rotation [%d] \n",padrotarray[i]->padname, padrotarray[i]->rotation); 
      display_error++;
   
      CString  padname = padrotarray[i]->newpadname;

      wmen_Graph_CreatePin_On(MentorCheckName('p',padname));
      BlockStruct *block = doc->Find_Block_by_Name(padrotarray[i]->padname, -1);
      write_padstack(wfp, &(block->getDataList()), file->getScale() * unitsFactor, 
            DegToRad(padrotarray[i]->rotation), padrotarray[i]->padname, padname);
   }


   // this must be outside of create_board
/*
$$create_stackup("Artwork_Stackup");
$$page(0.0, 0.0, 0.03, @inches, 0.0, 0.0, [0.0,0.0,'ST$Artwork_Stackup'] );
$$point_mode(@vertex);
$$attribute( "ARTWORK_DEFINITION_IDENTIFIER", "");
$$attribute( "ARTWORK_01_LAYER_DEFINITION", "SIGNAL_1,pad_1");
$$attribute( "ARTWORK_02_LAYER_DEFINITION", "SIGNAL_2");
$$attribute( "ARTWORK_02_LAYER_DEFINITION", "POWER");
$$attribute( "ARTWORK_03_LAYER_DEFINITION", "SIGNAL_3");
$$attribute( "ARTWORK_03_LAYER_DEFINITION", "SIGNAL_3");
$$attribute( "ARTWORK_03_LAYER_DEFINITION", "POWER");
$$attribute( "ARTWORK_04_LAYER_DEFINITION", "SIGNAL_2");
$$attribute( "ARTWORK_06_LAYER_DEFINITION", "SIGNAL_6");
$$attribute( "ARTWORK_07_LAYER_DEFINITION", "SIGNAL_7");
$$attribute( "ARTWORK_08_LAYER_DEFINITION", "SIGNAL_8,pad_2");

*/
   fprintf(wfp, "$$create_stackup(\"Artwork_Stackup\");\n");
   fprintf(wfp, "$$page(0.0, 0.0, 0.03, @%s, 0.0, 0.0, [0.0,0.0,'ST$Artwork_Stackup'] );\n", output_units);
   fprintf(wfp, "$$point_mode(@vertex);\n");
   fprintf(wfp, "$$attribute( \"ARTWORK_DEFINITION_IDENTIFIER\", \"\");\n");



   // Get maxStackNum
   int maxStackNum = 0;
   for (int layerNum=0; layerNum<mentorLayerArrayCount; layerNum++)
   {
      MentorLayerStruct* mentorLayer = mentorLayerArray[layerNum];

      if (mentorLayer->stackNum > maxStackNum)
         maxStackNum = mentorLayer->stackNum;
   }

   // loop stackNums
   for (int stackNum=1; stackNum<=maxStackNum; stackNum++)
   {
      BOOL HaveWrittenOne = FALSE;

      // loop XRFs
      for (int xrfNum=0; xrfNum<maxXRF; xrfNum++)
      {
         // loop all layers to find ones on this XRF
         for (int layerNum=0; layerNum<mentorLayerArrayCount; layerNum++)
         {
            MentorLayerStruct* mentorLayer = mentorLayerArray[layerNum];

            if (stackupname_already_written(layerNum))
               continue;

            if (mentorLayer->xrf != xrfNum+1) // this layer is not at this XRF
               continue;

            if (mentorLayer->stackNum != stackNum) // we are doing this stackNum now
               continue;

            if (HaveWrittenOne)
               fprintf(wfp, ", ");
            else
            {
               CString padLayerName;

               if (stackNum == 1)   // top
                  padLayerName = "PAD_1, ";
               else if (stackNum == maxStackNum) // bottom
                  padLayerName = "PAD_2, ";
               else
                  padLayerName = "";

               fprintf(wfp, "$$attribute( \"ARTWORK_%02d_LAYER_DEFINITION\", \"%s", stackNum, padLayerName);
            }

            HaveWrittenOne = TRUE;

            fprintf(wfp, "%s", mentorLayer->newName);
         }
      }

      if (HaveWrittenOne)
         fprintf(wfp, "\");\n");
   }

   // here do now PANEL
   // create_panel
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)         continue;
      
      // create necessary aperture blocks
      if (block->getFlags() & BL_TOOL)          continue;
      if (block->getFlags() & BL_APERTURE)      continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
   
      if (block->getFlags() & BL_FILE) 
      {
         if (file->getBlockType() == BLOCKTYPE_PANEL)
         {
            fprintf(flog,"Panel entry found\n");
/*
            wmen_Graph_CreatePanel_On(file->getBlock()->name);
            MENTOR_WriteGEOMASCIIData(&(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1);
            // power netnames
            // boardoutline
            // board routing layers
            // fixed component locations
            // board default padstack
*/
         }
      }
   } // while

   if (file->getBlockType() == BLOCKTYPE_PCB)
   {
      wmen_Graph_CreateBoard_On(MentorCheckName('g',file->getBlock()->getName()));
      MENTOR_WriteGEOMASCIIData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 
         file->getScale() * unitsFactor, 0, -1);
            
      // power netnames
      //fprintf(wfp,"$$attribute( \"POWER_NET_NAMES\", \"NO_POWER_LAYERS\");\n");
      write_powernets(wfp);

      int found;
      // board_routing_outline
      Outline_Start(doc);
      found = MENTOR_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 
      file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_ROUTKEEPIN);
      if (!found)
      {
         // now look for board outline
         found = MENTOR_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 
            file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_BOARDOUTLINE);
         if (!found)
         {
            // now look for other stuff
            found = MENTOR_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 
                  file->getScale() * unitsFactor, 0, -1, FALSE, GR_CLASS_ROUTKEEPIN);
            if (!found)
            {
               fprintf(flog,"Did not find a closed Board Routing Outline.\n");
               display_error++;
            }
         }
      }

      int returnCode;
      if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "BOARD_ROUTING_OUTLINE", "", file->getScale() * unitsFactor))
      {
         fprintf(flog,"Did not find a closed Board Routing Outline.\n");
         display_error++;
         found = 0;
      }
      Outline_FreeResults();

      doc->validateBlockExtents(file->getBlock());

      if (!found)
      {
         double xmin = (file->getBlock()->getXmin() * file->getScale() * unitsFactor);
         double ymin = (file->getBlock()->getYmin() * file->getScale() * unitsFactor);
         double xmax = (file->getBlock()->getXmax() * file->getScale() * unitsFactor);
         double ymax = (file->getBlock()->getYmax() * file->getScale() * unitsFactor);

         // make one !
         fprintf(wfp,"$$initial([%1.*lf,%1.*lf], , @nosnap );\n",
               output_units_accuracy,  xmin, output_units_accuracy, ymin);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy, xmax, output_units_accuracy, ymin);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy, xmax, output_units_accuracy, ymax);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy,xmin, output_units_accuracy,ymax);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy, xmin, output_units_accuracy, ymin);
         fprintf(wfp,"$$attribute( \"BOARD_ROUTING_OUTLINE\", \"\", @mark, @scale );\n");
      }

      // board_placement_outline
      Outline_Start(doc);
      found = MENTOR_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 
               file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_PLACEKEEPIN);
      if (!found)
      {
         // look for board outline graphic
         found = MENTOR_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 
               file->getScale() * unitsFactor, 0, -1, TRUE, GR_CLASS_BOARDOUTLINE);
         if (!found)
         {
            // now look for every visible outline
            int found = MENTOR_OutlineBOARDData(wfp, &(file->getBlock()->getDataList()), 0.0, 0.0, 0.0, 0, 
               file->getScale() * unitsFactor, 0, -1, FALSE, GR_CLASS_PLACEKEEPIN);
            if (!found)
            {
               fprintf(flog,"Did not find a closed Board Placement Outline for.\n");
               display_error++;
            }
         }
      }
      if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy), "BOARD_PLACEMENT_OUTLINE", "", file->getScale() * unitsFactor))
      {
         fprintf(flog,"Did not find a closed Board Placement Outline for.\n");
         display_error++;
         found = 0;
      }
      Outline_FreeResults();

      if (!found)
      {
         // make one !
         double xmin = (file->getBlock()->getXmin() * file->getScale() * unitsFactor);
         double ymin = (file->getBlock()->getYmin() * file->getScale() * unitsFactor);
         double xmax = (file->getBlock()->getXmax() * file->getScale() * unitsFactor);
         double ymax = (file->getBlock()->getYmax() * file->getScale() * unitsFactor);

         // make one !
         fprintf(wfp,"$$initial([%1.*lf,%1.*lf], , @nosnap );\n",
               output_units_accuracy,  xmin, output_units_accuracy, ymin);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy, xmax, output_units_accuracy, ymin);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy, xmax, output_units_accuracy, ymax);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy,xmin, output_units_accuracy,ymax);
         fprintf(wfp,"$$terminal([%1.*lf,%1.*lf] );\n",
               output_units_accuracy, xmin, output_units_accuracy, ymin);
         fprintf(wfp,"$$attribute( \"BOARD_PLACEMENT_OUTLINE\", \"\", @mark, @scale );\n");
      }

      // board routing layers

      // fixed component locations

      // board default padstack
/* need to check_na routine ????
      if (strlen(board_defaultpadstack))
         fprintf(wfp,"$$attribute( \"BOARD_DEFAULT_PADSTACK\", \"%s\");\n",board_defaultpadstack);
*/
      // place of non PCB components
      /*
      $$add( "ss_brd", -0.15, -2.075, "ss_brd",  1, 1, 1 );
      $$add( "ss_target", 6.4, -2.125, "ss_target",  1, 1, 1 );
      $$add( "ss_target", -0.15, -2.075, "ss_target",  1, 1, 1 );
      $$attribute( "POWER_NET_NAMES", "VCC, GROUND");
      $$attribute( "BOARD_MATERIAL", "fr4");
      $$attribute( "BOARD_THICKNESS", "", , @scale , , [0.062, 0.0]);
      $$attribute( "BOARD_INTERNAL_COPPER", "", , @scale , , [0.001, 0.0]);
      $$attribute( "BOARD_EXTERNAL_COPPER", "", , @scale , , [0.001, 0.0]);
      $$attribute( "DRILL_DEFINITION_UNPLATED", "0.125", , @scale , , [0.0, 0.0]);
      $$attribute( "DRILL_ORIGIN", "0.0", , @scale , , [-0.4, -4.625]);
      $$attribute( "BOARD_WIRE_GRID_X", "", , @scale , , [0.025, 0.0]);
      $$attribute( "BOARD_WIRE_GRID_Y", "", , @scale , , [0.025, 0.0]);
      $$attribute( "BOARD_VIA_GRID_X", "", , @scale , , [0.05, 0.0]);
      $$attribute( "BOARD_VIA_GRID_Y", "", , @scale , , [0.05, 0.0]);
      $$attribute( "BOARD_PIN_GRID_X", "", , @scale , , [0.1, 0.0]);
      $$attribute( "BOARD_PIN_GRID_Y", "", , @scale , , [0.1, 0.0]);
      $$attribute( "DEFAULT_PADSTACK_CLEARANCE", "", , @scale , , [0.013, 0.0]);
      */

      // must attributes
      fprintf(wfp,"$$attribute( \"DEFAULT_PAD_SIZE\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
                  output_units_accuracy, one_mil*50);
      fprintf(wfp,"$$attribute( \"BOARD_DEFINITION_IDENTIFIER\", \"\");\n");
      fprintf(wfp,"$$attribute( \"BOARD_ROUTING_LAYERS\", \"\", , , , [%d, 0]);\n", 
               max_signalcnt);
      fprintf(wfp,"$$attribute( \"BOARD_PLACEMENT_GRID\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
               output_units_accuracy, one_mil*100);

      fprintf(wfp,"$$attribute( \"BOARD_PLACEMENT_CLEARANCE\", \"\", , @scale , , [%1.*lf, 0.0]);\n",
               output_units_accuracy, one_mil);

      fprintf(wfp,"$$attribute( \"DIAGONAL_ROUTING_ALLOWED\", \"yes\");\n");
      fprintf(wfp,"$$attribute( \"TJUNCTIONS_ALLOWED\", \"yes\");\n");

      do_padrules(wfp);

      wmen_Graph_File_Close_Layout();
   }  // if filetype == PCB

   for (i=0;i<viacnt;i++)
   {
      delete viaarray[i];
   }

   return 1;
}

/******************************************************************************
* do_netlistfile
*/
static int do_netlistfile(FILE *fp, FileStruct *file)
{
   fprintf(fp,"#  BOARD STATION RELEASE 8.0 NETS FILE FORMAT 1.0\n");
   wmen_Graph_Date(fp,"#");
   fprintf(fp,"# The netlist includes single pin nets\n");
   fprintf(fp,"#\n");

   NetStruct *net;
   POSITION  netPos;
   int   unusedpincnt = 0;
   
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      int   cnt;
      net = file->getNetList().GetNext(netPos);
      if (!(net->getFlags() & NETFLAG_UNUSEDNET))
         cnt = fprintf(fp, "NET \'%s\' ", net->getNetName());

      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         
         CString c, p;
         c = MentorCheckName('c', compPin->getRefDes());
         p = MentorCheckName('e', compPin->getPinName());

         if (net->getFlags() & NETFLAG_UNUSEDNET)
         {
            fprintf(fp, "NET \'/UNUSED_$%d$\' \'%s-%s\'\n", ++unusedpincnt, c, p);
         }
         else
         {
            if (cnt > 70)
               cnt = fprintf(fp, "\nNET \'%s\' ", net->getNetName());
            cnt += fprintf(fp, " \'%s-%s\' ", c, p);
         }
      }
      fprintf(fp,"\n");
   }
   return 1;
}

/******************************************************************************
* do_componentfile
*/
static int do_componentfile(FILE *wfp, FileStruct *file, double unitsFactor)
{
   fprintf(wfp,"#  BOARD STATION RELEASE 8.0 COMPONENTS FILE FORMAT 2.0\n");
   wmen_Graph_Date(wfp,"#");
   fprintf(wfp,"#\n");
   fprintf(wfp,"UNITS %s\n", output_units_units);
   fprintf(wfp,"#\n");
   fprintf(wfp,"# Component property is presented in the format (prop_name, prop_value).\n");
   fprintf(wfp,"#\n");
   fprintf(wfp,"# Reference  Part_number  Symbol  Geometry  Board_location  Properties\n");

   PCB_WriteCOMPONENTData(wfp,&(file->getBlock()->getDataList()), 
      file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
      file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   return 1;
}

/******************************************************************************
* do_tracefile
*/
static int do_tracefile(FILE *wfp, FILE *prtfp, FileStruct *file,double unitsFactor)
{
   int   i;

   fprintf(wfp,"#  BOARD STATION RELEASE 8.0 TRACES FILE FORMAT 4.1\n");
   wmen_Graph_Date(wfp,"#");
   fprintf(wfp,"#  \n");

   fprintf(wfp,"UNITS %s\n", output_units_units);
   fprintf(wfp,"ST 0 0 0 0\n");


   for (i=1;i<=maxXRF;i++)
   {
      for (int ii=0;ii<mentorLayerArrayCount;ii++)
      {
         if (mentorLayerArray[ii]->xrf == i)
         {
            // only write one per index
            fprintf(wfp,"XRF %d %s\n",i,mentorLayerArray[ii]->newName);
            break;
         }
      }
   }
   fprintf(wfp,"\n");

   // need to make a layername, layerbitmap function
   // understand layerstacking including POWER_, SIGNAL_
   // loop throu netlist and store pins, vias, segments.
   // understand copper areas and voids.
   NetStruct *net;
   POSITION  netPos;
   
   wmen_Graph_Date(flog,"Start trace");
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

#ifdef DUMP
      fprintf(flog, "NET=%s\n", net->getNetName());
#endif   
      viacnt = 0;

      // reset status 
      CString  f;
      f.Format("Processing %s",net->getNetName());
      progress->SetStatus(f);

#ifdef DUMP
      wmen_Graph_Date(flog,f);
#endif
      do_signalpinloc(net, file->getScale());

      // order by signal name
      SetMarginTrace(0.0001); // set rounding margin - do not change it to more or less !!!

      PCB_WriteROUTESData(wfp, prtfp, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, net->getNetName());
      CResultList *resultList = FindShortestNetList();
#ifdef DUMP
      DumpResult(flog, resultList);
#endif
      Write_Traces(wfp, resultList,net->getNetName(), file->getScale() * unitsFactor);

      ClearTrace();

      for (i=0;i<viacnt;i++)
      {
         delete viaarray[i];
      }

   }
   fprintf(wfp,"#  \n");

   wmen_Graph_Date(flog,"Stop trace");
   return 1;
}

/******************************************************************************
* load_MENTORlayerfile
*/
static int load_MENTORlayerfile(CString layerFileName)
{

   FILE *fLay = fopen(layerFileName, "rt");
   if (!fLay)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", layerFileName);
      MessageBox(NULL, tmp,"MENTOR Layer File", MB_OK | MB_ICONHAND);
      return 0;
   }
  
//name  stacking_num   type    color width fill  patt  trans  path space style text   pen hilite protect select

   char line[255];
   while (fgets(line, 255, fLay))
   {
      char *lp;
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '#')
         continue;

      MENLayerrepStruct ll;

      ll.name = lp;
      ll.name.TrimLeft();
      ll.name.TrimRight();
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.stacking_num = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.type = lp;
      ll.type.TrimLeft();
      ll.type.TrimRight();
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.color = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.width = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.fill = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.patt = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.trans = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.path = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.space = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.style = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.text = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.pen = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.hilite = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.protect = atoi(lp);
      if ((lp = get_string(NULL," \t")) == NULL)   continue;
      ll.select = atoi(lp);

      // only copy if a layerline is complete.
      MENLayerrepStruct *l = new MENLayerrepStruct;
      *l = ll;
      layerreparray.SetAtGrow(layerrepcnt, l);
      layerrepcnt++;

   }

   fclose(fLay);

   return 1;
}

/******************************************************************************
* get_free_layer_stacknum
// 200 - 255  but not 201, 202, 203, 237, 238, 241, 242
// 800 - 1023
*/
static int get_free_layer_stacknum()
{
   int   i, ii;
   
   for (i=200;i<=255;i++)
   {
      if (i == 201)  continue;
      if (i == 202)  continue;
      if (i == 203)  continue;
      if (i == 237)  continue;
      if (i == 238)  continue;
      if (i == 241)  continue;
      if (i == 242)  continue;

      int found = FALSE;
      for (ii=0;ii<layerrepcnt;ii++)
      {
         if (layerreparray[ii]->stacking_num == i)
         {
            found = TRUE;
            break;
         }
      }

      if (!found) return i;

   }
   
   for (i=800;i<=1023;i++)
   {
      int found = FALSE;
      for (ii=0;ii<layerrepcnt;ii++)
      {
         if (layerreparray[ii]->stacking_num == i)
         {
            found = TRUE;
            break;
         }
      }

      if (!found) return i;

   }
   
   return -1;
}


/******************************************************************************
* do_layerfile
*/
static int do_layerfile(FILE *fp)
{
   fprintf(fp,"#name             stacking #   type    color width fill  patt  trans  path space style text   pen hilite protect select\n");
   fprintf(fp,"# Written by CAMCAD\n");

   int   i;
   for (i=0;i<layerrepcnt;i++)
   {
      fprintf(fp,"%-40s %5d %10s %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
         layerreparray[i]->name,
         layerreparray[i]->stacking_num,
         layerreparray[i]->type,
         layerreparray[i]->color,
         layerreparray[i]->width, 
         layerreparray[i]->fill,
         layerreparray[i]->patt,  
         layerreparray[i]->trans,  
         layerreparray[i]->path,
         layerreparray[i]->space, 
         layerreparray[i]->style, 
         layerreparray[i]->text,  
         layerreparray[i]->pen,
         layerreparray[i]->hilite,
         layerreparray[i]->protect,
         layerreparray[i]->select);
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int do_padrules(FILE *fp)
{
   int   i;

   int maxxrf = 0;
   for (i=0;i<mentorLayerArrayCount;i++)
   {
      if (maxxrf < mentorLayerArray[i]->xrf)
         maxxrf = mentorLayerArray[i]->xrf;
   }
   int minxrf = 0;
   for (i=0;i<mentorLayerArrayCount;i++)
   {
      if (maxxrf < mentorLayerArray[i]->xrf)
         maxxrf = mentorLayerArray[i]->xrf;
   }


   //Write Pad Rules Section
   fprintf(fp,"// Pad Rules\n");
   for (i=0;i<padstackcnt;i++)
   {
      if (padstackarray1[i]->inner && padstackarray1[i]->top && !padstackarray1[i]->bot || // on top and inner  or,
          padstackarray1[i]->inner && !padstackarray1[i]->top && padstackarray1[i]->bot || // on bottom and inner or,
          padstackarray1[i]->inner && !padstackarray1[i]->top && !padstackarray1[i]->bot)  // inner only.
      {
         UINT minLayer, maxLayer;
         minLayer = padstackarray1[i]->padElecLayerList[0];
         maxLayer = padstackarray1[i]->padElecLayerList[0];
         //find lowest layer
         for (int j = 0; j < padstackarray1[i]->padElecLayerCount; j++)
            if(padstackarray1[i]->padElecLayerList[j] < minLayer && padstackarray1[i]->padElecLayerList[j] <= (UINT)maxxrf)
               minLayer = padstackarray1[i]->padElecLayerList[j];
         //find highest layer
         for (int k = 0; k < padstackarray1[i]->padElecLayerCount; k++)
            if(padstackarray1[i]->padElecLayerList[k] > maxLayer && padstackarray1[i]->padElecLayerList[k] <= (UINT)maxxrf)
               maxLayer = padstackarray1[i]->padElecLayerList[k];

         //get mirror layer of highest layer for this padstack

         if (minLayer == 0)
            minLayer = maxLayer;
         else if (maxLayer == 0)
            maxLayer = minLayer;

         LayerStruct *layer1 = doc->FindLayer(get_layer_index_from_stacknum(maxLayer));
         if (!layer1)
            continue;
         int mirMaxLayer = layer1->getMirroredLayerIndex();
         layer1 = doc->FindLayer(mirMaxLayer);
         
         //get mirror layer of lowest layer for this padstack  
         LayerStruct *layer2 = doc->FindLayer(get_layer_index_from_stacknum(minLayer));
         if (!layer2)
            continue;
            int mirMinLayer = layer2->getMirroredLayerIndex();
         layer2 = doc->FindLayer(mirMinLayer);

         //mirror layers
         fprintf(fp,"$$define_rule_pin(\"%s\", \"PHYSICAL_%d\", \"PHYSICAL_%d\", @NOTOP);\n",
            MentorCheckName('p',padstackarray1[i]->padstackname), layer2->getElectricalStackNumber(), layer1->getElectricalStackNumber());               
         //regular layers
         fprintf(fp,"$$define_rule_pin(\"%s\", \"PHYSICAL_%d\", \"PHYSICAL_%d\", @TOP);\n",
            MentorCheckName('p',padstackarray1[i]->padstackname), minLayer, maxLayer);               
      }  
   }
   //Write Via Rules Section
   fprintf(fp,"//\n");
   fprintf(fp,"// Via Rules\n");
   for (i=0;i<padstackcnt;i++) 
   {
      CString VName;
      VName.Format("VIA_%s", padstackarray1[i]->padstackname);
      if (padstackarray1[i]->used_as_via == 0) // if it is not a via, skip it.
         continue;
      //if it is on top surface
      if (padstackarray1[i]->top && !padstackarray1[i]->inner && !padstackarray1[i]->bot)
      {  
         fprintf(fp,"$$define_rule_via(\"%s\", \"PHYSICAL_%d\", \"PHYSICAL_%d\");\n",
            MentorCheckName('p',VName), 1, 1); 
      }                 
      //if it is on bottom surface
      else
      if (!padstackarray1[i]->top && !padstackarray1[i]->inner && padstackarray1[i]->bot)
      {
         fprintf(fp,"$$define_rule_via(\"%s\", \"PHYSICAL_%d\", \"PHYSICAL_%d\");\n",
            MentorCheckName('p',VName), maxxrf, maxxrf);   
      }
      else  // if it is inner (buried)
      if (padstackarray1[i]->inner && padstackarray1[i]->top && !padstackarray1[i]->bot || // on top and inner  or,
          padstackarray1[i]->inner && !padstackarray1[i]->top && padstackarray1[i]->bot || // on bottom and inner or,
          padstackarray1[i]->inner && !padstackarray1[i]->top && !padstackarray1[i]->bot)  // inner only.
      {
         UINT minLayer, maxLayer;
         minLayer = padstackarray1[i]->padElecLayerList[0];
         maxLayer = padstackarray1[i]->padElecLayerList[0];

         //find the lowest layer for this padstack 
         for (int j = 0; j < padstackarray1[i]->padElecLayerCount; j++)
            if(padstackarray1[i]->padElecLayerList[j] < minLayer && padstackarray1[i]->padElecLayerList[j] <= (UINT)maxxrf)
               minLayer = padstackarray1[i]->padElecLayerList[j];

         //find the highest layer for this padstack   
         for (int k = 0; k < padstackarray1[i]->padElecLayerCount; k++)
            if(padstackarray1[i]->padElecLayerList[k] > maxLayer && padstackarray1[i]->padElecLayerList[k] <= (UINT)maxxrf)
               maxLayer = padstackarray1[i]->padElecLayerList[k];

         if (minLayer == 0)
            minLayer = maxLayer;
         else if (maxLayer == 0)
            maxLayer = minLayer;

         //from the lowest layer to the highest layer
         fprintf(fp,"$$define_rule_via(\"%s\", \"PHYSICAL_%d\", \"PHYSICAL_%d\");\n",
            MentorCheckName('p',VName), minLayer, maxLayer);           
      }

   }
   fprintf(fp,"//\n");
   return 1;
}

/******************************************************************************
* do_techfile
*/
static int do_techfile(FILE *fp)
{
   int   i;

   fprintf(fp,"$set_transcript_mode(@bottom);\n");
   fprintf(fp,"//\n");
   fprintf(fp,"//  BOARD STATION RELEASE 8.0 TECHNOLOGY FILE FORMAT 1.1\n");
   fprintf(fp,"//\n");
   fprintf(fp,"//  Board Technology\n");
   fprintf(fp,"//\n");
   fprintf(fp,"$$define_technology( \"PCB\");\n");
   fprintf(fp,"//\n");
   fprintf(fp,"//  Trace Endcode Type\n");
   fprintf(fp,"//\n");
   fprintf(fp,"$$set_endcode( @Round );\n");
   fprintf(fp,"//\n");
   fprintf(fp,"//  Board Rule\n");
   fprintf(fp,"//\n");
   fprintf(fp,"$$define_rule_board( @DOUBLE); \n");
   fprintf(fp,"//\n");

   int maxxrf = 0;
   for (int ii=0;ii<mentorLayerArrayCount;ii++)
   {
      if (maxxrf < mentorLayerArray[ii]->xrf)
         maxxrf = mentorLayerArray[ii]->xrf;
   }

   fprintf(fp,"//  Physical Layers\n");
   for (i=1;i<=maxxrf;i++) // only 1..n
   {
      for (int ii=0;ii<mentorLayerArrayCount;ii++)
      {
         if (mentorLayerArray[ii]->xrf == i)
         {
            //if (mentorLayerArray[ii]->stackNum == 1)   // top layer
            if (mentorLayerArray[ii]->xrf == 1) // top layer
               fprintf(fp,"$$define_physical_layer( %d, \"PHYSICAL_%d\", \"PAD_1\", \"%s\");\n",
                  mentorLayerArray[ii]->xrf, mentorLayerArray[ii]->xrf, mentorLayerArray[ii]->newName);
            //else if (mentorLayerArray[ii]->stackNum == maxXRF)  // bottom
            else if (mentorLayerArray[ii]->xrf == maxXRF)   // bottom
               fprintf(fp,"$$define_physical_layer( %d, \"PHYSICAL_%d\", \"PAD_2\", \"%s\");\n",
                  mentorLayerArray[ii]->xrf, mentorLayerArray[ii]->xrf, mentorLayerArray[ii]->newName);
            else
               fprintf(fp,"$$define_physical_layer( %d, \"PHYSICAL_%d\", \"%s\");\n",
                  mentorLayerArray[ii]->xrf, mentorLayerArray[ii]->xrf, mentorLayerArray[ii]->newName);
            break;   // only 1 XRF layer output.
         }
      }
   }
   fprintf(fp,"//\n");

   return 1;
}

/******************************************************************************
* resequence_signal
   if a power layer is in between than we need to resequence the signal_%d
*/
static int resequence_signal()
{
   int   i, l;
   int   signalcnt = 0;
   int   powercnt = 0;

   maxXRF = 0;
   for (l=1;l<255;l++)  // loop through stacknum 1..xx
   {
      int sfound = FALSE; // there can be multiple entries for the same stacknumber.

      for (i=0;i<mentorLayerArrayCount;i++)
      {
#ifdef _DEBUG
   MentorLayerStruct *tmp = mentorLayerArray[i];
#endif

         if (maxXRF < mentorLayerArray[i]->xrf)
            maxXRF = mentorLayerArray[i]->xrf;

         if (mentorLayerArray[i]->stackNum != l)   continue;

         if (mentorLayerArray[i]->on == 0)   continue;   // not visible
      
         if (mentorLayerArray[i]->type == 'S')
         {
            if (!sfound)   ++signalcnt;
            sfound = TRUE;
            mentorLayerArray[i]->newName.Format("SIGNAL_%d", signalcnt);
         }
         else
         if (mentorLayerArray[i]->type == 'P')
         {
            mentorLayerArray[i]->newName.Format("POWER_%d", ++powercnt);   // power has to continuealy count up !
         }
      }
   }

   max_signalcnt = signalcnt;

   return 1;
}

//--------------------------------------------------------------
// insertLayer -2, do not write any layers
// this is only in Graphic mode
void MENTOR_WriteData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int layer;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer == -2)
         {
            // layer was written on the outside
         }
         else
         {
            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               layer = insertLayer;
            else
               layer = np->getLayerIndex();

            // insert has not always a layer definition.
            if (!doc->get_layer_visible(layer, mirror))
               continue;
            const char  *l = Layer_Mentor(layer);
            if (l == NULL) 
               continue;
            wmen_Graph_Level(l);
         }
      }

      wmen_Graph_Width(0.0);

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            Point2 *points;
            int    cnt = 0;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->getWidthIndex() > 0) 
                  wmen_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               
               Point2 p2;
               cnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  points[cnt] = p2;
                  cnt++;
               }
               wmen_Graph_Polyline(points, cnt, polyFilled, closed);
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();
            double strokewidth = doc->getWidthTable()[np->getText()->getPenWidthIndex()]->getSizeA() * scale;

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wmen_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale, strokewidth,
                              text_rot, text_mirror);

         }
         break;
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       break;

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror) ) // only write data item if it goes in this file
               {
                  wmen_Graph_Level(Layer_Mentor(layer));
                  const char  *l = Layer_Mentor(layer);
                  if (l == NULL) 
                     break;
                  wmen_Graph_Level(l);
                  // here we need to deal with complex apertures
                  // apertures can not me made a generic part, because the layers are 
                  // not known
                  switch (block->getShape())
                  {
                     case T_UNDEFINED:
                     break;
                     case T_COMPLEX:
                     break;
                     default:
                        wmen_Graph_Aperture(
                              point2.x,
                              point2.y,
                              block->getSizeA() * scale,
                              block->getSizeB() * scale,
                              block->getShape(), np->getInsert()->getAngle());
                     break;
                  }
               }
            }
            else // not aperture
            {
               // generic parts can only be rotated in 45 degrees
               if (TRUE)
               {
                  CString  n = get_uniquegeomname(np->getInsert()->getBlockNumber());
                  CString  cn;

                  // here make a check dependent on what it was
                  // Case dts0100469660 says treat tooling blocks exactly the same as padstack blocks
                  if (block->getBlockType() == BLOCKTYPE_PADSTACK || block->getBlockType() == BLOCKTYPE_TOOLING) 
                     cn = MentorCheckName('p',n);
                  else
                     cn = MentorCheckName('g',n);

                  wmen_Graph_Block_Reference(cn,
                               insert_x + np->getInsert()->getOriginX() * scale,
                               insert_y + np->getInsert()->getOriginY() * scale,
                               block_rot, block_mirror,
                               np->getInsert()->getScale() * scale, 0);
               }
               else
               {
                  // here we should use generic parts
                  int block_layer = -1;

                  if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
                     block_layer = insertLayer;
                  else
                     block_layer = np->getLayerIndex();

                  MENTOR_WriteData(&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
               }
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for
} /* end MENTOR_WriteData */

//--------------------------------------------------------------
static int in_mlarray(const char *n)
{
   for (int l=0;l<mentorLayerArrayCount;l++)
   {
      if (!strcmp(mentorLayerArray[l]->newName, n))
         return l+1;
   }
   return 0;
}

//--------------------------------------------------------------
static void MENTOR_FindFillVoidData(CDataList *DataList, int insertLayer)
{
   DataStruct  *np;
   int         layer;
   int         mirror = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();
         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;

            // loop thru polys
            POSITION polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL polyvoid   = poly->isVoid();            
               BOOL boundary   = poly->isFloodBoundary();           

               // only closed stuff
               if (!polyFilled && !polyvoid &&!boundary)    continue;

               // here is a netname or void is found
               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
               if (a || polyvoid)
               {
                  const char  *lname = Layer_Mentor(layer);
                  if (lname == NULL)                  break;
                  LayerStruct *l = doc->FindLayer(layer);
                  CString  menlay;
                  menlay.Format("A_%s", lname);
                  int signr = 0; // no signal number, because this will be always a doc layer.
                  if (!in_mlarray(menlay))
                  {
                     //mentorLayerArray.SetSizes
                     MentorLayerStruct *ml = new MentorLayerStruct;
                     ml->stackNum = signr;      // artwork layer 
                     ml->xrf = 0;
                     ml->layerindex = l->getLayerIndex();   // index into doc->LayerArray
                     ml->on = TRUE;
                     ml->type = 'A';            // area fill type
                     ml->oldName = l->getName();     // Layer
                     ml->newName = menlay;      // mentor layer
                     mentorLayerArray.SetAtGrow(mentorLayerArrayCount, ml);
                     mentorLayerArrayCount++;
                  }
                  break;   // no need to continue.
               }  
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
         }
         break;
         case T_INSERT:
         {
            // this should include all entities.
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
               // here we should use generic parts
               int block_layer = -1;

               if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
                  block_layer = insertLayer;
               else
                  block_layer = np->getLayerIndex();

               MENTOR_FindFillVoidData(&(block->getDataList()), block_layer);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for
} /* end MENTOR_FindFillVoidData */

//--------------------------------------------------------------
void MENTOR_WriteGEOMASCIIData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int layer;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // no etch data.
      if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;
      if (np->getGraphicClass() == GR_CLASS_UNROUTE)      continue;

      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // this is done in PRIMARY...
         if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)
         {
            continue;   // already done in Primary outline
         }

/* Board geometry layer must be explicitly mapped 
         if (np->getGraphicClass() == GR_CLASS_BOARD_GEOM)
         {
            wmen_Graph_Level("BOARD_OUTLINE");
         }
         else
*/
         if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
         {
            wmen_Graph_Level("BOARD_OUTLINE");
         }
         else
/* this is not a mentor output class for create_board
         //if (np->getGraphicClass() == GR_CLASS_COMPOUTLINE)
         //{
         // wmen_Graph_Level("COMPONENT_PLACEMENT_OUTLINE");
         //}
         //else
*/
         {
            // insert has not always a layer definition.
            if (!doc->get_layer_visible(layer, mirror))
               continue;
            const char  *l = Layer_Mentor(layer);
            if (l == NULL) 
               continue;
            wmen_Graph_Level(l);
         }
      }

      wmen_Graph_Width(0.0);

		switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            Point2 *points;
            int    cnt = 0;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (poly->getWidthIndex() > 0)
                  wmen_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
                  
               Point2 p2;
               cnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  points[cnt] = p2;
                  cnt++;
               }

/*
#define GR_CLASS_ROUTKEEPOUT     9  
#define GR_CLASS_VIAKEEPIN       12 
#define GR_CLASS_ALLKEEPIN       14 
#define GR_CLASS_ALLKEEPOUT      15 
#define GR_CLASS_COMPOUTLINE     17 // this is a primary good component silkscreen outline
#define GR_CLASS_PANELOUTLINE    18 // this is a primary good paneloutline
      layer = doc->LayerArray[layer];

            const char  *l = Layer_Mentor(layer);
            if (l == NULL) 
               continue;
            wmen_Graph_Level(l);

*/
               if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)
               {
                  if (doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_SIGNAL_ALL ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_PAD_ALL ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_ALL)
                     wmen_Graph_Level("ROUTING_KEEPOUT");
                  wmen_Graph_Attribute("ROUTING_KEEPOUT", points, cnt);
               }
               else
               if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)
               {
                  if (doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_SIGNAL_ALL ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_PAD_ALL ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_ALL)
                     wmen_Graph_Level("VIA_KEEPOUT");
                  wmen_Graph_Attribute("VIA_KEEPOUT", points, cnt);
               }
               else
               if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)
               {
                  if (doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_SIGNAL_ALL ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_PAD_ALL ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_ALL)
                  {
                     wmen_Graph_Level("PLACE");
                     wmen_Graph_Attribute("BOARD_PLACEMENT_KEEPOUT", points, cnt);
                  }
                  else
                  if (doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_TOP ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_SIGNAL_TOP)
                  {
                     wmen_Graph_Level("PLACE_1");
                     wmen_Graph_Attribute("BOARD_PLACEMENT_KEEPOUT", points, cnt);
                  }
                  else
                  if (doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_BOTTOM ||
                      doc->getLayerArray()[layer]->getLayerType() == LAYTYPE_SIGNAL_BOT)
                  {
                     wmen_Graph_Level("PLACE_2");
                     wmen_Graph_Attribute("BOARD_PLACEMENT_KEEPOUT", points, cnt);
                  }
                  else
                  {
                     wmen_Graph_Polyline(points, cnt, polyFilled, closed);
                  }
               }
               else
               if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)
               {
                  wmen_Graph_Level("");
                  wmen_Graph_Attribute("BOARD_PLACEMENT_OUTLINE", points, cnt);
               }
               else
                  wmen_Graph_Polyline(points, cnt, polyFilled, closed);
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();
            double strokewidth = doc->getWidthTable()[np->getText()->getPenWidthIndex()]->getSizeA() * scale;

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wmen_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale, strokewidth,
                              text_rot, text_mirror);

         }
         break;
         
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)            break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       break;

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror) ) // only write data item if it goes in this file
               {
                  const char  *l = Layer_Mentor(layer);
                  if (l == NULL) 
                     break;
                  wmen_Graph_Level(l);
                  // here we need to deal with complex apertures
                  // apertures can not me made a generic part, because the layers are 
                  // not known
                  switch (block->getShape())
                  {
                     case T_UNDEFINED:
                     break;
                     case T_COMPLEX:
                     break;
                     default:
                        wmen_Graph_Aperture(
                              point2.x - block->getXoffset() * scale,
                              point2.y - block->getYoffset() * scale,
                              block->getSizeA() * scale,
                              block->getSizeB() * scale,
                              block->getShape(), block->getRotation() + np->getInsert()->getAngle());
                     break;
                  }
               }
            }
            else // not aperture
            {
               // generic parts can only be rotated in 45 degrees
               if (TRUE)
               {
                  CString  n = get_uniquegeomname(np->getInsert()->getBlockNumber());
                  CString  cn;

                  // here make a check dependent on what it was
                  // Case dts0100469660 says treat tooling blocks exactly the same as padstack blocks
                  if (block->getBlockType() == BLOCKTYPE_PADSTACK || block->getBlockType() == BLOCKTYPE_TOOLING)
                     cn = MentorCheckName('p',n);
                  else
                     cn = MentorCheckName('g',n);

                  wmen_Graph_Block_Reference(cn,
                               insert_x + np->getInsert()->getOriginX() * scale,
                               insert_y + np->getInsert()->getOriginY() * scale,
                               block_rot, block_mirror,
                               np->getInsert()->getScale() * scale, 0);
               }
               else
               {
                  // here we should use generic parts
                  int block_layer = -1;

                  if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
                     block_layer = insertLayer;
                  else
                     block_layer = np->getLayerIndex();

                  MENTOR_WriteGEOMASCIIData(fp, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer);
               }
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end for

   return;
 } /* end MENTOR_WriteGEOMASCIIData */

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
int MENTOR_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int typ)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int layer;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // no etch data.
      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() == GR_CLASS_ETCH)            continue;
         if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)      continue;
         if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)     continue;
         if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)     continue;
         if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)    continue;
         if (np->getGraphicClass() == GR_CLASS_VIAKEEPIN)       continue;
         if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)      continue;
         if (np->getGraphicClass() == GR_CLASS_ALLKEEPIN)       continue;
         if (np->getGraphicClass() == GR_CLASS_ALLKEEPOUT)      continue;
      }

      if (np->getDataType() != T_INSERT)
      {
         layer = -1;

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, -1))
            continue;
         const char  *l = Layer_Mentor(layer);
         if (l == NULL) 
            continue;
         wmen_Graph_Level(l);
      }

      wmen_Graph_Width(0.0);

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            Point2 *points;
            int    cnt = 0;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (poly->getWidthIndex() > 0)
                  wmen_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               
               Point2 p2;
               cnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  points[cnt] = p2;
                  cnt++;
               }
               wmen_Graph_Polyline(points, cnt, polyFilled, closed);
            }  // while
            free(points);
            found++;
         }
         break;  // POLYSTRUCT
         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();
            double strokewidth = doc->getWidthTable()[np->getText()->getPenWidthIndex()]->getSizeA() * scale;

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wmen_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale, strokewidth, 
                              text_rot, text_mirror);

            found++;
         }
         break;
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)            break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       break;

            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            CString     lname;

            lname = "";
            int layer = -1;

            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               layer = insertLayer;
            else
               layer = np->getLayerIndex();

            if (block->getFlags() & BL_TOOL) break;

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               layer = Get_ApertureLayer(doc, np, block, insertLayer);
            }

            LayerStruct *l = doc->FindLayer(layer);

            if (l)
            {
               if (l->getLayerType() == LAYTYPE_DRILL)
               {
                  // do not translate a drill layer or drill drawing layer
                  break;
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_TOP || l->getLayerType() == LAYTYPE_SIGNAL_TOP)
               {
                  if (typ == 1)  
                     lname = "PAD";
                  else
                     lname = "PAD_1";
               }
               else
               if ( l->getLayerType() == LAYTYPE_PAD_BOTTOM || l->getLayerType() == LAYTYPE_SIGNAL_BOT)
               {
                  if (typ == 1)  
                     lname = "PAD";
                  else
                     lname = "PAD_2";
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_ALL || l->getLayerType() == LAYTYPE_SIGNAL_ALL)
               {
                  if (typ == 1)  
                     lname = "PAD";
                  else
                     lname = "SIGNAL";
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_OUTER)
               {
                  if (typ == 1)  
                     lname = "PAD";
                  else
                     lname = "PAD";
               }
               else
               if (l->getLayerType() == LAYTYPE_POWERNEG)
               {
                  if (l->getElectricalStackNumber())
                     lname = Layer_Mentor(layer);
                  else
                     lname = "POWER";
               }
               else
               if (l->getLayerType() == LAYTYPE_SPLITPLANE)
               {
                  if (l->getElectricalStackNumber())
                     lname = Layer_Mentor(layer);
                  else
                     lname = "POWER";
               }
               else
               if (l->getLayerType() == LAYTYPE_MASK_TOP)
               {
                  if (typ == 1)  
                     lname = "SOLDER_MASK";
                  else
                     lname = "SOLDER_MASK_1";
               }
               else
               if (l->getLayerType() == LAYTYPE_MASK_BOTTOM)
               {
                  if (typ == 1)  
                     lname = "SOLDER_MASK";
                  else
                     lname = "SOLDER_MASK_2";
               }
               else
               if (l->getLayerType() == LAYTYPE_MASK_ALL)
               {
                  lname = "SOLDER_MASK";
               }
               else
               if (l->getLayerType() == LAYTYPE_PASTE_TOP)
               {
                  if (typ == 1)  
                     lname = "PASTE_MASK";
                  else
                     lname = "PASTE_MASK_1";
               }
               else
               if (l->getLayerType() == LAYTYPE_PASTE_BOTTOM)
               {
                  if (typ == 1)  
                     lname = "PASTE_MASK";
                  else
                     lname = "PASTE_MASK_2";
               }
               else
               if (l->getLayerType() == LAYTYPE_PASTE_ALL)
               {
                  lname = "PASTE_MASK";
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_INNER || l->getLayerType() == LAYTYPE_SIGNAL_INNER)
               {
                  lname = "SIGNAL";
               }
               else
               {
                  lname = Layer_Mentor(layer);
               }
            }

            if (strlen(lname) == 0) break;

            wmen_Graph_Level(lname);
            // here we need to deal with complex apertures
            // apertures can not me made a generic part, because the layers are 
            // not known

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               CPolyList *poly_list;

               if (block->getShape() == T_COMPLEX)
               {
                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  found += MENTOR_WritePADSTACKData(fp, &(subblock->getDataList()), 
                     point2.x, point2.y, block_rot, 0, scale, 0, layer, typ);
                  break;
               }

               if ((poly_list = ApertureToPoly_Base(block, point2.x, point2.y, block_rot, 0)) == NULL)
                  break;

               //draw polylist---------------------------------
               Point2   *points;
               int      cnt = 0;
               CPoly    *poly;
               CPnt     *pnt;
               POSITION polyPos, pntPos;

               // count thru polys
               polyPos = poly_list->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = poly_list->GetNext(polyPos);
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     cnt++;
                  }
               }  // while

               Mat2x2   mm;
               RotMat2(&mm, 0.0);   // do not double rotate. Once in block_rot when aperture to poly and than again.
            
               points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
               // loop thru polys
               polyPos = poly_list->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = poly_list->GetNext(polyPos);

                  BOOL polyFilled = poly->isFilled();
                  BOOL closed     = poly->isClosed();
         
                  double   cx, cy, radius;
                  if (PolyIsCircle(poly, &cx, &cy, &radius))
                  {
                     wmen_Graph_Circle( cx, cy, radius * scale, 0.0 );
                  }
                  else
                  {
                     Point2 p2;
                     cnt = 0;
                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        p2.x = pnt->x * scale;
                        if (mirror) p2.x = -p2.x;
                        p2.y = pnt->y * scale;
                        p2.bulge = pnt->bulge;
                        TransPoint2(&p2, 1, &mm, insert_x, insert_y);
                        // here deal with bulge
                        points[cnt] = p2;
                        cnt++;
                     }
                     wmen_Graph_Polyline(points, cnt, polyFilled, closed);
                  }
               }  // while
               free(points);
               // free_poly_list
               FreePolyList(poly_list);

               found++;
            }
            else
            {
               fprintf(flog,"PADSTACK Graphic for [%s] not supported yet.\n", block->getName());
               display_error++;
            }
////
         } // case INSERT
         break;
      } // end switch
   } // end for

   return found;
} /* end MENTOR_WritePadstackData */

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
int MENTOR_GetPADSTACKData(FILE *fp, CDataList *DataList, int insertLayer, 
                           double *drill, double scale, int *top, int *inner, int *bot, CUIntArray *padElecLayerList, int &padElecLayerCount)
{
   DataStruct *np;
   int layer;
   int typ = 0;   

   *drill = 0;
   int count = 0;


   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // no etch data.
      if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

      if (np->getDataType() != T_INSERT)
      {
         // 
         layer = -1;

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         const char  *l = Layer_Mentor(layer);
         if (l == NULL) 
            continue;

         int lay = get_layernr_from_index(layer);

         //#define   LAY_TOP           -1
         //#define   LAY_BOT           -2
         //#define   LAY_INNER         -3
         //#define   LAY_ALL           -4
         //#define   LAY_POWER         -5
         if (lay == 1 || lay == LAY_TOP)
         {
            typ = 1;
         }
         else
         if (lay > 1 || lay == LAY_BOT || lay == LAY_INNER)
         {
            typ = 2;
         }
         else
         if (lay == LAY_ALL)
         {
            typ = 3;
         }
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)            break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_TOOL))
            {
               *drill = block->getToolSize() * scale;
            }
            else
            // doe not need to be an aperture, can be any drawing
            //if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = -1;

               if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
                  layer = insertLayer;
               else
                  layer = np->getLayerIndex();

               if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               {
                  layer = Get_ApertureLayer(doc, np, block, insertLayer);
               }

               // if multiple electrical layers
            
               // put the electrical layers in array
               int lay = get_layernr_from_index(layer);
               padElecLayerList->SetAtGrow(padElecLayerCount,lay);
               padElecLayerCount++;
      


               //#define   LAY_TOP           -1
               //#define   LAY_BOT           -2
               //#define   LAY_INNER         -3
               //#define   LAY_ALL           -4
               //#define   LAY_POWER         -5
               int maxStackNum = 0;
               for (int i=0;i<mentorLayerArrayCount;i++)
               {
                  if (maxStackNum < mentorLayerArray[i]->stackNum)
                     maxStackNum = mentorLayerArray[i]->stackNum;
               }
               
               if (lay == 1 || lay == LAY_TOP)
               {
                  typ |= 0x1;
                  *top = TRUE;
               }
               else
               if (lay > 1 || lay == LAY_BOT || lay == LAY_INNER)
               {
                  typ |= 0x2;
                  *inner = TRUE;
               }
               if (lay == maxStackNum || lay == LAY_ALL)
               {

                  typ |= 0x3;
                  *bot = TRUE;
               }
               //if the padstack is on PAD_ALL, then it is on top, bottom and inner.
               if (lay == LAY_ALL)
               {
                  *bot = *inner = *top = TRUE;
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
} /* end MENTOR_GetPadstackData */

/*****************************************************************************/
/*
*/
static int get_padrot(const char *p, int rot)
{
   int   i;

   for (i=0;i<padrotcnt;i++)
   {
      if (!strcmp(padrotarray[i]->padname, p) && padrotarray[i]->rotation == rot)
         return i;
   }

   return -1;
}

/******************************************************************************
* MENTOR_WritePCBCOMPONENTData
*/
int MENTOR_WritePCBCOMPONENTData(FILE *wfp, const char *shapename, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, MENPinExtent *pext)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   Point2 point2;
   BOOL SMD = FALSE;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
      {
         int layerNum;

         if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layerNum = insertLayer;
         else
            layerNum = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layerNum, -1))
            continue;
         const char  *l = Layer_Mentor(layerNum);
         if (l == NULL) 
            continue;

         // this must match with the Write_ClassPCBComponent
         switch (data->getGraphicClass())
         {
         case GR_CLASS_VIAKEEPIN:
         case GR_CLASS_VIAKEEPOUT:
         case GR_CLASS_ROUTKEEPIN:
         case GR_CLASS_ROUTKEEPOUT:
         case GR_CLASS_PLACEKEEPOUT:
//       case GR_CLASS_COMPOUTLINE: // allow the outline also for silkscreen
            continue;
         }

         wmen_Graph_Level(l);
      }

      wmen_Graph_Width(0.0);

      switch (data->getDataType())
      {
         case T_POLY:
         {
            int pntCount = 0;

            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);

               if (poly->getWidthIndex() > 0)
                  wmen_Graph_Width(doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale);

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);
                  pntCount++;
               }
            }

            Point2 *points = (Point2*)calloc(pntCount, sizeof(Point2)); // entities+1 because num of points = num of segments +1

            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);
                  
               pntCount = 0;
               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);

                  Point2 point2;
                  point2.x = pnt->x * scale;
                  if (mirror)
                     point2.x = -point2.x;
                  point2.y = pnt->y * scale;
                  point2.bulge = pnt->bulge;
                  TransPoint2(&point2, 1, &m, insert_x, insert_y);
                  points[pntCount++] = point2;
               }
               wmen_Graph_Polyline(points, pntCount, poly->isFilled(), poly->isClosed());
            }

            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = data->getText()->getPnt().x * scale;
            point2.y = data->getText()->getPnt().y * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            double text_rot = rotation + data->getText()->getRotation();
            double strokewidth = doc->getWidthTable()[data->getText()->getPenWidthIndex()]->getSizeA() * scale;

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = data->getText()->isMirrored();
            wmen_Graph_Text(data->getText()->getText(), point2.x, point2.y, data->getText()->getHeight() * scale, data->getText()->getWidth()*scale, strokewidth, 
                  text_rot, text_mirror);
         }
         break;

         case T_INSERT:
         {
            if (data->getInsert()->getInsertType() == INSERTTYPE_CENTROID)
               break;

            point2.x = data->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
            double block_rot = rotation + data->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               // here get pinname
               CString pinname = MentorCheckName('e', data->getInsert()->getRefname());

               if (strlen(pinname) == 0)
               {
                  fprintf(flog, "Pinname not found\n");
                  display_error++;
               }

               fprintf(wfp,"$$attribute( \"COMPONENT_PIN_DEFINITION\", \"%s\", , @scale , , [%1.*lf, %1.*lf]);\n",
                  pinname, output_units_accuracy, point2.x, output_units_accuracy, point2.y);

               if (pext->x1 > point2.x)
                     pext->x1 = point2.x;
               if (pext->y1 > point2.y)
                     pext->y1 = point2.y;
               if (pext->x2 < point2.x)
                     pext->x2 = point2.x;
               if (pext->y2 < point2.y)
                     pext->y2 = point2.y;

               // this pin has no padshape
               if (block->getName() && strlen(block->getName()))
               {
                  CString padname = get_usedpadname(block->getName());

                  if (block_rot)
                  {
                     int rot = round(RadToDeg(block_rot));
                     while (rot < 0)
                        rot += 360;
                     while (rot >= 360)
                        rot -= 360;
                     CString degreeRot;
                     degreeRot.Format("%d", rot);
                     padname += "_";
                     padname += degreeRot;
                     if (get_padrot(block->getName(), rot) < 0)
                     {
                        MENPadRotStruct *p = new MENPadRotStruct;
                        p->padname = block->getName();     
                        p->newpadname = padname;
                        p->rotation = rot;
                        padrotarray.SetAtGrow(padrotcnt, p);
                        padrotcnt++;
                     }
                  }
   
                  if (mirror)
                  {
                     fprintf(flog, "Shape [%s], Pin [%s] -> Pinmirror not implemented\n", shapename, pinname);
                     display_error++;
                  }

                  fprintf(wfp, "$$attribute( \"COMPONENT_PADSTACK_OVERRIDE\", \"%s,%s\");\n", pinname, MentorCheckName('p',padname));
               }

               // check here is SMD
               if (is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
                  SMD = TRUE;
            }
            else if (data->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               fprintf(wfp, "$$add( \"%s\", %1.*lf, %1.*lf, \"%s\", 1, 1, 1 );\n", block->getOriginalName(), 
                     output_units_accuracy, point2.x, output_units_accuracy, point2.y, block->getOriginalName());
            }
            else if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
            {
               int layer = Get_ApertureLayer(doc, data, block, insertLayer);

               if (doc->get_layer_visible(layer, -1) ) // only write data item if it goes in this file
               {
                  const char *l = Layer_Mentor(layer);
                  if (l == NULL) 
                     break;
                  wmen_Graph_Level(l);
                  // here we need to deal with complex apertures
                  // apertures can not me made a generic part, because the layers are 
                  // not known
                  switch (block->getShape())
                  {
                  case T_UNDEFINED:
                  case T_COMPLEX:
                     // if (true)
                     {
                        BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                        if (subblock != NULL)
                        {
                           MENTOR_WritePADSTACKData(wfp, &(subblock->getDataList()), 
                           point2.x, point2.y, block_rot, 0, scale, 0, layer, 1/*typ*/);
                        }
                     }
#ifdef PUNT__JUST_MAKE_RECTANGLE
                     else
                     {
                        CExtent apextent = block->getExtent(*doc);
                        bool isvalid = apextent.isValid();
                        double sizeA = apextent.getXsize();
                        double sizeB = apextent.getYsize();
                        wmen_Graph_Aperture(point2.x, point2.y, sizeA * scale, sizeB * scale, T_RECTANGLE, data->getInsert()->getAngle());
                     }
#endif
                     break;
                  default:
                     wmen_Graph_Aperture(point2.x, point2.y, block->getSizeA() * scale, block->getSizeB() * scale, block->getShape(), data->getInsert()->getAngle());
                     break;
                  }
               }
            }
            else // not aperture
            {
               // here we should use generic parts
               int block_layer = -1;

               if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
                  block_layer = insertLayer;
               else
                  block_layer = data->getLayerIndex();

               MENTOR_WritePCBCOMPONENTData(wfp, shapename, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror,
                     scale * data->getInsert()->getScale(), embeddedLevel+1, block_layer, pext);
            }
         }
         break;
      }
   }

   return SMD;
}


//--------------------------------------------------------------
void MENTOR_WriteClassPCBCOMPONENTData(FILE *wfp, const char *shapename, CDataList *DataList, 
      double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   DataStruct  *np;
   int         layer;
   CString     classname;
   const char  *l;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (np->getGraphicClass() == 0)                     continue;
         if (np->getGraphicClass() == GR_CLASS_COMPOUTLINE)  continue;

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, -1))
            continue;
         l = Layer_Mentor(layer);
         if (l == NULL) 
            continue;

         if (!STRCMPI(l, "COMPONENT_PLACEMENT_OUTLINE")) // this only exist as an attribute, not as a layer.
            continue;

         // for all layer, there is no second argument.
         LayerStruct *ll = doc->FindLayer(layer);
         if (ll->getLayerType() == LAYTYPE_ALL)
            l = "";
         else
         if (ll->getLayerType() == LAYTYPE_TOP)
            l = get_layername_from_stacknum(1);
         else
         if (ll->getLayerType() == LAYTYPE_BOTTOM)
            l = get_layername_from_stacknum(max_signalcnt);

         // this must match with the Write_PCBComponent
         if (np->getGraphicClass() == GR_CLASS_VIAKEEPIN)
            classname = "VIA_KEEPIN";
         else
         if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)
            classname = "VIA_KEEPOUT";
         else
         if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)
            classname = "ROUTING_KEEPIN";
         else
         if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)
            classname = "ROUTING_KEEPOUT";
         else
         if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)
         {
            if (ll->getLayerType() == LAYTYPE_TOP || ll->getElectricalStackNumber() == 1)
               l = "PLACE_1";
            else
            if (ll->getLayerType() == LAYTYPE_BOTTOM || ll->getElectricalStackNumber() == max_signalcnt)
               l = "PLACE_2";
            else
               l = "PLACE";
            classname = "PLACEMENT_KEEPOUT";
         }
         else
         {
            // all other classes are written in Write_PCBComponent
            fprintf(flog,"Unknown Class [%s] in component [%s]\n", graphicClassTagToDisplayString(np->getGraphicClass()), shapename);
            continue;
         }
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            POSITION polyPos;

            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (!write_easyoutline(wfp, &(poly->getPntList()), classname, l, scale))
               {
                  fprintf(flog, "Error in graphic class outline for [%s]\n", shapename);
                  display_error++;
               }
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         break;

         case T_INSERT:
         break;
      } // end switch
   } // end for
   return;
} /* graphic class */

//--------------------------------------------------------------
// tabs are not allowed in the mentor file.
void PCB_WriteCOMPONENTData(FILE *isf, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   char  *ident = "        ";  // <== no tabls are allowed

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())                                        
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (block_mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT || 
                np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
            {
               CString compname = MentorCheckName('c', np->getInsert()->getRefname());

               Attrib* attrib;                                                       
               CString  part_number = "PartNumber";
               CString  symbol = "Symbol";
               CString  geometry = MentorCheckName('g',block->getName());

               if (attrib =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
                  part_number = get_attvalue_string(doc, attrib);

               if (attrib =  is_attvalue(doc, np->getAttributesRef(), "Symbol", 1))
                  symbol = get_attvalue_string(doc, attrib);

               if (strlen(part_number) == 0) part_number = "PartNumber";
               if (strlen(symbol) == 0)      symbol = "Symbol";

               fprintf(isf,"%s %s %s %s ",compname, MentorCheckName('b',part_number), symbol, geometry);

               // x y layer rotation
               fprintf(isf," %1.*lf %1.*lf ", 
                  output_units_accuracy, point2.x, output_units_accuracy, point2.y);
               fprintf(isf," %d ",(block_mirror)?2:1);
               fprintf(isf," %1.0lf ", normalizeDegrees(RadToDeg(block_rot)));

               if ((np->getInsert()->getPlacedBottom()) && !(np->getInsert()->getMirrorFlags() & MIRROR_FLIP))
               {
                  fprintf(flog, "Component [%s] placed on BOTTOM but not mirrored -> not possible in Mentor.\n",
                     np->getInsert()->getRefname());
                  display_error++;
               }

               // attributes
               if (attrib = is_attvalue(doc, np->getAttributesRef(), ATT_REFNAME, 2))
               {
                  if (attrib->isVisible())
                  {
                     int r = round(RadToDeg(attrib->getRotationRadians()));
                     double strokewidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA() * scale;

                     while (r < 0)     r+=360;
                     while (r >=360)   r-=360;

                     // the placement is the offset to the place koo of the block.
                     double x = attrib->getX() ,y = attrib->getY();

                     Attrib *b;
                     if (b = is_attvalue(doc, block->getAttributesRef(), ATT_REFNAME, 2))
                     {
                        x = x - b->getX();
                        y = y - b->getY();
                     }
					      /* Added justification for refname visible attribute
                     enum HorizontalPositionTag
                     {
                        horizontalPositionLeft   = 0,
                        horizontalPositionCenter = 1,
                        horizontalPositionRight  = 2
                     };

                     enum VerticalPositionTag
                     {
                        verticalPositionBaseline = 0,
                        verticalPositionCenter   = 1,
                        verticalPositionTop      = 2,
                        verticalPositionBottom   = 3
                     };
                     */
                     VerticalPositionTag vj;
                     HorizontalPositionTag hj;
					      vj = attrib->getVerticalPosition();
                     hj = attrib->getHorizontalPosition();

                     CString justification;

                     switch (vj)
                     {
                        case verticalPositionBaseline:
                        case verticalPositionBottom:
                           justification = "B";
                        break;
                        case verticalPositionCenter:
                           justification = "C";
                        break;
                        case verticalPositionTop:
                           justification = "T";
                        break;
                        default:
                           justification = "B";
                        break;
                     }

                     switch (hj)
                     {
                        case horizontalPositionLeft:
                           justification.AppendChar('L');
                        break;
                        case horizontalPositionCenter:
                           justification.AppendChar('C');
                        break;
                        case horizontalPositionRight:
                           justification.AppendChar('R');
                        break;
                        default:
                           justification.AppendChar('L');
                        break;
                     }

                     fprintf(isf,"-\n%s(REFLOC,\"%s,%1.*lf,%1.*lf,%d,%s,%1.*lf,%1.*lf,%1.*lf,std,0\") ", ident,
                        output_units_units,
                        output_units_accuracy,x*scale,output_units_accuracy,y*scale,
                        r, justification,
                        output_units_accuracy,attrib->getHeight()*scale,
                        output_units_accuracy,attrib->getWidth()*scale,
                        output_units_accuracy,strokewidth);
                  }
                  else
                  {
                     // Not visible attribute set height, width, track width, etc to 0.0 (not visible in Mentor)
                     fprintf(isf,"-\n%s(REFLOC,\"%s,0.0,0.0,0,BL,0.0,0.0,0.0,std,0\") ", ident,
                        output_units_units);
                  }
               }
               write_compattributes(isf, np->getAttributesRef(), ident);
               fprintf(isf,"\n");

               break;
            }
         } // case INSERT
         break;
      } // end switch
   } // end PCB_WriteCOMPONENTData */
}

//--------------------------------------------------------------
static int do_signalpinloc(NetStruct *net, double scale)
{
   CompPinStruct *compPin;
   POSITION compPinPos;
   int      pinindex = 0;
   
   compPinPos = net->getHeadCompPinPosition();
   while (compPinPos != NULL)
   {
      compPin = net->getNextCompPin(compPinPos);

      if (compPin->getPinCoordinatesComplete())
      {
         double  pinx, piny, pinrot;
         int     padmirror;
         CString padstackname;

         padstackname = "";
         pinx = compPin->getOriginX();
         piny = compPin->getOriginY();
         pinrot = RadToDeg(compPin->getRotationRadians());
         padmirror = compPin->getMirror();

         if (compPin->getPadstackBlockNumber() > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
            if (block)
               padstackname = block->getName();
         }

         
         UINT64 layermap;
            
         if (strlen(padstackname))
            layermap = get_padstacklayermap(padstackname,padmirror);
         else
            layermap = ALL_INT64;

         pinindex++;

#ifdef DUMP
         fprintf(flog, "index %d PIN X=%lf Y=%lf R=%s.%s P=%s layermap [%x] mirror %d\n",
               pinindex, pinx*scale, piny*scale,
               compPin->getRefDes(), compPin->getPinName(),padstackname, layermap, padmirror);
#endif

         LoadEndpoint(pinindex, pinx*scale, piny*scale, layermap);
      }
   }
   return 1;
}                                                        

//--------------------------------------------------------------
void PCB_WriteROUTESData(FILE *wfp, FILE *prtfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   const char  *ln;

   // CString  lay;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != GR_CLASS_ETCH)      continue;

         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         LayerStruct *l = doc->FindLayer(layer);
         ln = Layer_Mentor(layer);
         if (ln == NULL)   continue;

         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
         {
            if (strcmp(get_attvalue_string(doc, a),netname))
               continue;
         }  
         else
            continue;
      }
      else
      {
         // here check if ATTR_NETNAME == netname

         // allow only vias and test points
         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (a)
         {
            if (strcmp(get_attvalue_string(doc, a),netname))
               continue;
         }  
         else
            continue;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // here check if poly contains voids or fills. Netname is already checked.
            int   fill = 0;
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL polyvoid   = poly->isVoid();
               BOOL boundary   = poly->isFloodBoundary();

               if (polyFilled || boundary)
               {
                  fill++;
               }
            }

            if(fill)
            {
               polyPos = np->getPolyList()->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = np->getPolyList()->GetNext(polyPos);
                  if (write_area_prt)
                     write_area(prtfp, poly, insert_x, insert_y, scale, mirror, m, ln);
               }
               break;
            }

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               
               double width = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
            
               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               char lay = get_layernr_from_index(layer);
               if (lay < 0)   
                  break;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  if (first)
                  {
                     firstp = p2;
                     p1 = p2;
                     first = FALSE;
                  }
                  else
                  {
                     // char layer = (char)atoi(lay); // limit 32 layers
                     int err = LoadSegment(p1.x, p1.y, p2.x, p2.y, lay, poly->getWidthIndex());
                     if (err & 1)
                     {
#ifdef DUMP
                        fprintf(flog,"Segment length Error -> SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                           p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, width);
#endif
                     }
                     else
                     if (err & 2)
                     {
                        fprintf(flog,"Layer Error -> SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                           p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, width);
                     }
                     else
                     {
#ifdef DUMP
                        fprintf(flog,"SEG %lf %lf %lf %lf %lf %lf %d %lf\n",
                           p1.x,p1.y, p1.bulge, p2.x, p2.y,p2.bulge, (int)lay, width);
#endif
                     }
                     p1 = p2;
                  }
               }
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            // no text in route section
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CENTROID)       break;

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               Attrib *a;
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_VIALAYER, 1))
                  fprintf(flog,"Via range not implemented\n");
#ifdef DUMP
               fprintf(flog,"%d VIA X=%lf Y=%lf P=%s\n",viacnt, point2.x, point2.y,block->name);
#endif
               UINT64 layermap;

               if (strlen(block->getName()))
                  layermap = get_padstacklayermap(block->getName(),0);
               else
                  layermap = ALL_INT64;

               LoadVia(viacnt, point2.x, point2.y, layermap);
               MENViaStruct *v = new MENViaStruct;
               v->vianame = block->getName();     
               viaarray.SetAtGrow(viacnt, v);
               viacnt++;
               break;
            }

            if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;

               if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
                  block_layer = insertLayer;
               else
                  block_layer = np->getLayerIndex();

               PCB_WriteROUTESData(wfp, prtfp, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, netname);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end PCB_WriteROUTESData */
   return;
}

//--------------------------------------------------------------
static void get_vianames(CDataList *DataList)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
            {
               if (!used_as_via(block->getName())) 
               {
                  MENViaStruct *v = new MENViaStruct;
                  v->vianame = block->getName();     
                  viaarray.SetAtGrow(viacnt, v);
                  viacnt++;
               }
               break;
            }

            if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               get_vianames(&(block->getDataList()));
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end get_vianames */
}

/******************************************************************************
* get_uniquegeomname
// check if the original name is unique.
*/
static const char *get_uniquegeomname(int blocknum)
{
   BlockStruct *blockToCheck = doc->getBlockAt(blocknum);
   int found = 0;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      if (i == blocknum) // do not check itself
         continue;

      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      // check if the original name is equal to a block->name
      if (blockToCheck->getOriginalName().CompareNoCase(block->getName()) == 0)
         found++;
      // or equal to a original name
      else if (blockToCheck->getOriginalName().CompareNoCase(block->getOriginalName()) == 0)
         found++;
   }

   if (found) // use the name 
      return blockToCheck->getName();

   return blockToCheck->getOriginalName();
}

/******************************************************************************
* get_defaultlayer
*/
static const char *get_defaultlayer(LayerStruct *l, int *ON)
{
   *ON = TRUE;

   if (l->getLayerType() == LAYTYPE_PASTE_TOP)
   {
      if (l->getNeverMirror())
         return "PASTE_MASK_1";
      if (l->getMirrorOnly())
         return "PASTE_MASK_2";
      return "PASTE_MASK";
   }

   if (l->getLayerType() == LAYTYPE_PASTE_BOTTOM)
   {
      if (l->getNeverMirror())
         return "PASTE_MASK_1";
      if (l->getMirrorOnly())
         return "PASTE_MASK_2";
      return "PASTE_MASK";
   }

   if (l->getLayerType() == LAYTYPE_MASK_TOP)
   {
      if (l->getNeverMirror())
         return "SOLDER_MASK_1";
      if (l->getMirrorOnly())
         return "SOLDER_MASK_2";
      return "SOLDER_MASK";
   }

   if (l->getLayerType() == LAYTYPE_MASK_BOTTOM)
   {
      if (l->getNeverMirror())
         return "SOLDER_MASK_1";
      if (l->getMirrorOnly())
         return "SOLDER_MASK_2";
      return "SOLDER_MASK";
   }

   if (l->getLayerType() == LAYTYPE_SILK_TOP)
   {
      if (l->getNeverMirror())
         return "SILKSCREEN_1";
      if (l->getMirrorOnly())
         return "SILKSCREEN_2";
      return "SILKSCREEN";
   }

   if (l->getLayerType() == LAYTYPE_SILK_BOTTOM)
   {
      if (l->getNeverMirror())
         return "SILKSCREEN_1";
      if (l->getMirrorOnly())
         return "SILKSCREEN_2";
      return "SILKSCREEN";
   }

   if (l->getLayerType() == LAYTYPE_MASK_ALL)
   {
      return "SOLDER_MASK";
   }

   if (l->getLayerType() == LAYTYPE_PASTE_ALL)
   {
      return "PASTE_MASK";
   }

   if (l->getLayerType() == LAYTYPE_COMPONENTOUTLINE)
   {
      return "COMPONENT_BODY_OUTLINE";
   }

   *ON = FALSE;
   return l->getName();
}

