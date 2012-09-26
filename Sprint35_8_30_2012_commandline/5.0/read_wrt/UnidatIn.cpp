
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.

   The component outline is not correct in the Shape section.
   It seems that sometimes top or bottom is the master. However there is no
   indication which one is correct.

*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "net_util.h"
#include "format_s.h"
#include <math.h>
#include <float.h>
#include "gauge.h"
#include "attrib.h"
#include "lyrmanip.h"
#include "UnidatIn.h"
#include "CCEtoODB.h"
#include "DFT.h"
#include "Consolidator.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;

#define  MAX_ATT     100         // max attributes per component
#define  MAX_LINE    10000       // yes, 10K

#define  UNIDATERR            "unidat.log"

// defines for CompPinRules  (Location and Rotation)
#define CP_RULE_RELATIVE	0
#define CP_RULE_ABSOLUTE	1

#define CD_RULE_RADIUS		0
#define CD_RULE_DIAMETER	1


/****************************************************************************/
/*
   Global Commands
*/
static int get_version();
static int loop_unidat();		//List *c_list, int siz_of_command);
static int load_unidatsettings(const CString fname, CGeometryConsolidatorControl &consolControl);
static int get_drillindex(double size, int layernum);
static CString	make_padstack(FileStruct *pcbfile, const char *ptop, double torot, const char *padbot, double botrot);
static int load_toelist(CString footprint);
static int updateComponents_v21(FileStruct *pcbfile);
static void unidat_add_testprobes(FileStruct *pcbfile);
static void LoadPin(UNIDATToe& toe);
static void resolve_mechanical_comppins(CCEtoODBDoc *doc, FileStruct *pcbfile);

static FileStruct*      GetPcbFile();
static FileStruct*		global_pcbfile;  // sometimes it is a panel file instead of pcb file
static CCEtoODBDoc*		doc;

static FILE*				ferr;
static int					display_error = 0;

static bool      			PushTok = false;
static int      			eof_found;
static CString  			delimiter_string;
static int      			top_layer, bottom_layer;

static List*				cur_list;
static int					cur_list_size;

static FILE*    			ifp;
static char     			ifp_line[MAX_LINE];
static long     			ifp_linecnt = 0;

static char     			buf[MAX_LINE]; // tmp line

static char     			cur_tok[MAX_LINE];
static char     			cur_line[MAX_LINE];

static CPolyArray			polyarray;
static CAttrmapArray		attrmaparray;
static CCompPinArray		comppinarray;
static CCompArray			comparray;
static CDrillArray		drillarray;
static CFootPrintMap		footprintmap;
static CPackageMap		packageMap;
static CTestProbeArray  probearray;

static double   			scale_factor = 1;

static DataStruct*		cur_data;
static int      			cur_stat;
static double   			oldx, oldy;
static int      			oldwidthindex;
static int      			oldlay;
static CString  			oldnetname;

static int					PageUnits;
static char					last_delimeter;

static int					CompPinLocationRule;
static int					CompPinRotationRule;


static int					ComponentSMDrule; // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static int					CircleDefRule;		// 0 = Radius
														// 1 = Diameter

static double				arc_unit;
static int					CREATE_LIBRARY = TRUE;
static int					IGNORE_SHAPEOUTLINE = FALSE;

static CFileReadProgress*	fileReadProgress = NULL; 

static CString DataProducer; // CAD system name, alleged author of cad file being imported

static CTypedPtrArray<CPtrArray, FileStruct*> PcbFiles;
static CUnidatPCBList PanelPcbList;

static void SetVisiblePCB(); 

/******************************************************************************
* ReadUNIDAT
*  - UNIDAT is now the new format for UNICAM/FABMASTER
*/
void ReadUNIDAT(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   doc = Doc;

   CString unidatLogFile;
   ferr = getApp().OpenOperationLogFile(UNIDATERR, unidatLogFile);
   if (ferr == NULL) // error message already issued, just return.
      return;

   CGeometryConsolidatorControl consolidatorControl;
   display_error = 0;
      
   unidat_create_layer("0","",1);   

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(f,fname);
   strcat(f,ext);
   PageUnits = pageunits;

   scale_factor = Units_Factor(UNIT_INCHES, PageUnits);
   // all units are normalized to INCHES

   polyarray.SetSize(0,100);
   attrmaparray.SetSize(0,100);
   drillarray.SetSize(0,100);
   comppinarray.SetSize(0,100);
   comparray.SetSize(0,100);
   probearray.SetSize(0,100);

   CString settingsFile = getApp().getImportSettingsFilePath("unidat.in");
   {
      CString msg;
      msg.Format("\nUNIDAT: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_unidatsettings(settingsFile, consolidatorControl);

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(path_buffer,"rt")) == NULL)
   {
      ErrorMessage("Error open file", path_buffer);
      return;
   }

   setFileReadProgress("Reading Unidat file");

   // init globals
   cur_data    = NULL;
   ifp_linecnt = 0;
   PushTok     = false;
   eof_found   = FALSE;
   delimiter_string = "|";    // this is the default.
	cur_list = NULL;
	cur_list_size = 0;

   unidat_create_layer("0","",1);     // global layer 

	// get the version of the UNIDAT file
	cur_list = start_lst_v12;
	cur_list_size = SIZ_START_LST_V12;

	int version = get_version();
	if (version == UNIDAT_VERSION_2_1)
	{
		cur_list = start_lst_v21;
		cur_list_size = SIZ_START_LST_V21;
	}

	// re-initialize and rewind the file pointer
   cur_data    = NULL;
   ifp_linecnt = 0;
   PushTok     = false;
   eof_found   = FALSE;
   delimiter_string = "|";    // this is the default.
   DataProducer = "";
	rewind(ifp);

   // Format 2 has PCB and Panel creation keywords, format 1 does not.
   // If file is format 1 then create global camcad file now.
   // If file is format 2 the camcad files will be created as part of file processing.

   PanelPcbList.RemoveAll();
   PcbFiles.RemoveAll();
   global_pcbfile = NULL;

   if (version == UNIDAT_VERSION_1_2)
   {
      global_pcbfile = Graph_File_Start(f, fileTypeUnidat);
      global_pcbfile->setBlockType(blockTypePcb);
      PcbFiles.Add(global_pcbfile);
   }

	// This reads and processes on the fly most of the file
	loop_unidat();

	if (version == UNIDAT_VERSION_1_2)
	{
		// make local component if not a library
      // 1.2 format can only have one pcb file, no panel
		if (!CREATE_LIBRARY)
			make_localcomps(global_pcbfile);
	}
	else if (version == UNIDAT_VERSION_2_1)
   {
      for (int pcbindx = 0; pcbindx < PcbFiles.GetCount(); pcbindx++)
      {
         FileStruct *pcbfile = PcbFiles.GetAt(pcbindx);
         if (pcbfile != NULL)
         {
            updateComponents_v21(pcbfile);
         }
      }
   }

   PanelPcbList.MakePCBInserts(doc);
 
   // Some of the post processing is done in multiple loops in order to keep the overall
   // sequence the same as it was when this reader created one and only one PCB file.

	int refnameLayerNum = do_assign_layers();

	update_smdpads(doc);

   for (int pcbindx = 0; pcbindx < PcbFiles.GetCount(); pcbindx++)
   {
      FileStruct *pcbfile = PcbFiles.GetAt(pcbindx);
      if (pcbfile != NULL)
      {
         update_padstackddrill(pcbfile);
      }
   }

	RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.
	generate_PADSTACKACCESSFLAG(doc, 1);

   for (int pcbindx = 0; pcbindx < PcbFiles.GetCount(); pcbindx++)
   {
      FileStruct *pcbfile = PcbFiles.GetAt(pcbindx);
      if (pcbfile != NULL)
      {
         if (pcbfile->getBlock() != NULL)
         {
            double minref = 0.05 * Units_Factor(UNIT_INCHES, PageUnits);
            double maxref = 0.05 * Units_Factor(UNIT_INCHES, PageUnits);
            update_manufacturingrefdes(doc, pcbfile,*(pcbfile->getBlock()), minref, maxref, refnameLayerNum, FALSE);
         }

	      generate_PINLOC(doc, pcbfile, 1);   // update all comppin locations
      }
   }

	update_smdrule_geometries(doc, ComponentSMDrule);

   for (int pcbindx = 0; pcbindx < PcbFiles.GetCount(); pcbindx++)
   {
      FileStruct *pcbfile = PcbFiles.GetAt(pcbindx);
      if (pcbfile != NULL)
      {
         update_smdrule_components(doc, pcbfile, ComponentSMDrule);
         unidat_add_testprobes(pcbfile);
         resolve_mechanical_comppins(doc, pcbfile);
      }
   }

   SetVisiblePCB();

   fclose(ifp);

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!unidatLogFile.IsEmpty())
      fclose(ferr);

   PcbFiles.RemoveAll();
   PanelPcbList.RemoveAll();
   polyarray.empty();
   attrmaparray.empty();
   comppinarray.empty();
   comparray.empty();
   drillarray.empty();
	footprintmap.Empty();
	packageMap.Empty();
   probearray.empty();


   if (consolidatorControl.GetIsEnabled())
   {
      CGeometryConsolidator squisher(doc, "" /*no report*/);
      squisher.Go( &consolidatorControl );
   }
   else
   {
      // Geometry Consolidator purges unused widths and blocks, don't need to do it
      // twice but make sure it is done at least once.
      doc->purgeUnusedWidthsAndBlocks(false);
   }
      
   doc->RemoveUnusedLayers(false);

   if (display_error && !unidatLogFile.IsEmpty())
      Logreader(unidatLogFile);

   setFileReadProgress("");

   return;
}

static FileStruct*   GetPcbFile()
{
   if (global_pcbfile == NULL)
   {
      // Start a new file
      global_pcbfile = Graph_File_Start("PCBOARD", fileTypeUnidat);
      global_pcbfile->setBlockType(blockTypePcb);
      PcbFiles.Add(global_pcbfile);
   }

   return global_pcbfile;
}

//_____________________________________________________________________________
void SetVisiblePCB()
{
   // Import may create zero or more panels and zero or more PCBs.
   // If there is a panel make the first panel visible and all else invisible.
   // If there is no panel, make the first PCB visible and all else invisible.

   FileStruct *firstPanel = NULL;
   FileStruct *firstPCB = NULL;

   for (int i = 0; i < PcbFiles.GetCount(); i++)
   {
      FileStruct *f = PcbFiles.GetAt(i);
      if (f != NULL)
      {
         f->setShow(false);
         if (firstPanel == NULL && f->getBlockType() == blockTypePanel)
            firstPanel = f;
         if (firstPCB == NULL && f->getBlockType() == blockTypePcb)
            firstPCB = f;
      }
   }

   if (firstPanel != NULL)
      firstPanel->setShow(true);
   else if (firstPCB != NULL)
      firstPCB->setShow(true);

}

//_____________________________________________________________________________

static void resolve_mechanical_comppins(CCEtoODBDoc *doc, FileStruct *pcbfile)
{
   // The data sample for case dts0100405632 has some "components" defined without
   // pins, yet there are Component Pin records that reference these non-existent pins.
   // (They also usually happen to reference nets not defined in cad either, but we add them on the fly.)
   // These comppins cause problems, e.g. in Access Analysis in the step that
   // validates comp pins, because there is no component insert in the design with
   // any such pin.
   // After confering with Mark, it was decided these components should
   // be made into Mechanical Components instead of PCB Components, and we should
   // eliminate the comppins. This is awkward to do on the fly, because the component
   // definition may be spread out, e.g. the component is basically defined first, then
   // the comppins, then the footprints and toeprints (which might change/add pins in
   // the component). So we end up with another post process step. This step should
   // be performed after all others so everything is as complete as it is going to get.

   if (pcbfile != NULL)
   {
      // Find any PCB Component inserts that do not have any pins and change
      // them to Mechanical Components
      BlockStruct *pcbblock = pcbfile->getBlock();
      POSITION insertpos = pcbblock->getHeadDataInsertPosition();
      while (insertpos != NULL)
      {
         DataStruct *insertdata = pcbblock->getNextDataInsert(insertpos);
         if (insertdata != NULL && insertdata->isInsertType(insertTypePcbComponent))
         {
            int insertedBlkNum = insertdata->getInsert()->getBlockNumber();
            BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
            // What we care about is pin count, if inserted block does not exist then it
            // doesn't have pins.
            if (insertedBlk == NULL || insertedBlk->getPinCount() == 0)
            {
               // No pins, change to Mechanical Component
               insertdata->getInsert()->setInsertType(insertTypeMechanicalComponent);
            }
         }
      }

      POSITION netpos = pcbfile->getNetList().GetHeadPosition();
      while (netpos != NULL)
      {
         NetStruct *net = pcbfile->getNetList().GetNext(netpos);
         if (net != NULL)
         {
            POSITION cppos = net->getCompPinList().getHeadPosition();
            while (cppos != NULL)
            {
               POSITION thiscppos = cppos;
               CompPinStruct *cp = net->getCompPinList().getNext(cppos);
               CString refname = cp->getRefDes();

               DataStruct *compdata = NULL;
               UNIDATComp *uc = comparray.Find(refname);
               if (uc != NULL)
                  compdata = uc->GetData();
               // If it wasn't in the comparray then it might be a testpoint, have to look in
               // pcbfile for a testpoint.
               if (compdata == NULL)
                  compdata = pcbfile->FindInsertData(refname, insertTypeTestPoint);

               if (compdata == NULL)
               {
                  // No comp, then no comppin
                  net->getCompPinList().deleteAt(thiscppos);
                  //FreeCompPin(cp);
               }
               else
               {
                  // If comp had no pins then we have already set it to insertTypeMechanicalComp above.
                  // If it is a mechanical comp then get rid of the comppin.
                  // If it is a pcb comp, then get rid of the test pin if pcb comp has no such pin name.
                  // If it is still a pcb comp then we also know the inserted block exists, don't have to check that again.
                  if (compdata->isInsertType(insertTypeMechanicalComponent))
                  {
                     net->getCompPinList().deleteAt(thiscppos);
                     //FreeCompPin(cp);
                  }
                  else
                  {
                     int insertedBlkNum = compdata->getInsert()->getBlockNumber();
                     BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
                     if (insertedBlk == NULL) // Check anyway, it makes me nervous not to
                     {
                        // No block, no pins, then no comppins
                        net->getCompPinList().deleteAt(thiscppos);
                        //FreeCompPin(cp);
                     }
                     else if (!insertedBlk->FindData(cp->getPinName()))
                     {
                        // No such pin, then no comppin
                        net->getCompPinList().deleteAt(thiscppos);
                        //FreeCompPin(cp);
                     }
                  }
               }
            }
         }
      }
   }
}

//_____________________________________________________________________________
static void setFileReadProgress(const CString& status)
{
   delete fileReadProgress;

   if (status.IsEmpty())
   {
      fileReadProgress = NULL;
   }
   else
   {
      fileReadProgress = new CFileReadProgress(ifp);
      fileReadProgress->updateStatus(status);
   }
}

/****************************************************************************/
/*
*/
static const char* get_attrmap(const char *a)
{
   int   i;

   for (i=0;i < attrmaparray.GetCount();i++)
   {
      if (attrmaparray[i]->unidat_name.CompareNoCase(a) == 0)
         return attrmaparray[i]->cc_name.GetBuffer(0);
   }

   return a;
}

/****************************************************************************/
/*
   a complete record always end with a ;
   only keywords do not, but they start with a $
*/
static int fskip()
{
   fprintf(ferr,"Skip at %ld\n", ifp_linecnt);
   display_error++;

   return 1;
}

/****************************************************************************/
/*
*/
static int fnull()
{
   return 1;
}

/****************************************************************************/
/*
*/
static int p_error()
{
   fprintf(ferr,"Error at %ld -> token expected.\n", ifp_linecnt);
   display_error++;

   return -1;
}

/****************************************************************************/
/*
*/
static double   cnv_tok(char *l)
{
   double x;

   if (l && strlen(l))
   {
      x = atof(l);
   }
   else
   {
      return 0.0;
   }

   return x * scale_factor;
}


/****************************************************************************/
/*
*/
static char *get_nextline(char *string,int n,FILE *fp)
{
   static CString last_line;
   static char    *last_res;
   long           last_linecnt;
   char   *res;
                                   
   if (PushTok)
   {
      strcpy(string,last_line);
      res = last_res;
      PushTok = false;
   }
   else
   {
      while (TRUE)
      {
         ifp_linecnt++;

         if (((ifp_linecnt % 200) == 1) && fileReadProgress != NULL)
         {
            fileReadProgress->updateProgress();
         }

         if ((res = fgets(string,MAX_LINE,fp)) == NULL)
         {
            eof_found = TRUE;
            return NULL;
         }

         last_line = string;
         last_line.TrimLeft();
         last_line.TrimRight();
         last_linecnt = ifp_linecnt;
         CString l = last_line.Left(4);
         l.TrimRight(); 

         if (l.GetLength() == 0) continue;

         last_res = res;
         break;
      }
   }
   
   // clean right 
   CString  tmp = last_line;
   tmp.TrimRight();
   strcpy(string,tmp);

   return res;
}

/****************************************************************************/
/*
   make layers more understandable for PADS users.
*/
static int do_assign_layers()
{
   LayerStruct *l;
   int      i;
   CString  lname;
   int      stacknr = 0;

   if ((l = doc->FindLayer_by_Name("PLACE_TOP")) != NULL)
   {
      l->setLayerType(LAYTYPE_TOP);
      int laynr = unidat_create_layer("PLACE_BOTTOM","",0);
      l = doc->FindLayer(laynr);
      l->setLayerType(LAYTYPE_BOTTOM);
      Graph_Level_Mirror("PLACE_TOP", "PLACE_BOTTOM", "");
   }

   if ((l = doc->FindLayer_by_Name("DRILLHOLE")) != NULL)
   {
      l->setLayerType(LAYTYPE_DRILL);
   }

   // check for top = 2 and bottom = 1
   if (top_layer > bottom_layer)
   {
      for (i=top_layer;i>= bottom_layer;i--)
      {
         lname.Format("%d", i);

         if ((l = doc->FindLayer_by_Name(lname)) != NULL)
         {
            if (i == top_layer)
            {
               l->setLayerType(LAYTYPE_SIGNAL_TOP);
            }
            else if (i == bottom_layer)
            {
               l->setLayerType(LAYTYPE_SIGNAL_BOT);
            }
            else
            {
               l->setLayerType(LAYTYPE_SIGNAL_INNER);
            }

            l->setElectricalStackNumber(++stacknr);
         }
      }
   }
   else
   {
      for (i=top_layer;i<= bottom_layer;i++)
      {
         lname.Format("%d", i);
         if ((l = doc->FindLayer_by_Name(lname)) != NULL)
         {
            if (i == top_layer)
            {
               l->setLayerType(LAYTYPE_SIGNAL_TOP);
            }
            else if (i == bottom_layer)
            {
               l->setLayerType(LAYTYPE_SIGNAL_BOT);
            }
            else
            {
               l->setLayerType(LAYTYPE_SIGNAL_INNER);
            }

            l->setElectricalStackNumber(++stacknr);
         }
      }
   } // top > bottom


   int refnameLaynr;

   if (IGNORE_SHAPEOUTLINE)
   {
      refnameLaynr = unidat_create_layer("PLACE_TOP","",0);

      LayerStruct *l = doc->FindLayer_by_Name("COMPOUTLINE_TOP");
      DeleteLayer(doc, l, TRUE);
      l = doc->FindLayer_by_Name("COMPOUTLINE_BOTTOM");
      DeleteLayer(doc, l, TRUE);
   }
   else
   {
      refnameLaynr = unidat_create_layer("COMPOUTLINE_TOP","",0);
   }

   return refnameLaynr;
}

/****************************************************************************/
/*
*/
static int update_padstackddrill(FileStruct *pcbfile)
{
   if (pcbfile != NULL)
   {
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (block != NULL && block->getFileNumber() == pcbfile->getFileNumber())
         {
            switch (block->getBlockType())
            {
            case BLOCKTYPE_PADSTACK:  
               {
                  Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 2);

                  if (a)   break; // no drill for SMD

                  doc->CalcBlockExtents(block);
                  Point2   pmin, pmax; 
                  pmin.x = block->getXmin();
                  pmin.y = block->getYmin();
                  pmax.x = block->getXmax();
                  pmax.y = block->getYmax();
                  double drill = (pmax.x - pmin.x) * 0.5;

                  BlockStruct *b = Graph_Block_On(GBO_APPEND, block->getName(), pcbfile->getFileNumber(), 0);
                  int layernum = unidat_create_layer("DRILLHOLE","",0);
                  if (doc->getLayerAt(layernum) != NULL)
                     doc->getLayerAt(layernum)->setLayerType(LAYTYPE_DRILL);
                  int drillindex = get_drillindex(drill, layernum);
                  CString drillname;
                  drillname.Format("DRILL_%d",drillindex);
                  DataStruct *data = Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
                  if (data->getInsert() != NULL)
                     data->getInsert()->setInsertType(insertTypeDrillHole);
                  Graph_Block_Off();
               }
            }
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
   The question is if the component_pin position is absolute (as described in the deocuments)
   or relative to the shape (as found in examples).
*/
static int compare_pinloc()
{
   int      i;
   double   onemil = Units_Factor(UNIT_MILS, PageUnits);
   int      error = 0;

   for (i=0;i < comppinarray.GetCount();i++)
   {
#ifdef _DEBUG
      CompPinStruct *cp = comppinarray[i]->comppin;
#endif
      if (comppinarray[i]->comppin)
      {
         // check for 1 mil accuracy
         if (fabs(comppinarray[i]->comppin->getOriginX() - comppinarray[i]->x) > onemil || 
             fabs(comppinarray[i]->comppin->getOriginY() - comppinarray[i]->y) > onemil)
         {
            fprintf(ferr, "Component [%s] Pin [%s] Location change [%lg:%lg] [%lg:%lg]\n",
               comppinarray[i]->comppin->getRefDes(), comppinarray[i]->comppin->getPinName(), 
               comppinarray[i]->comppin->getOriginX(), comppinarray[i]->comppin->getOriginY(),
               comppinarray[i]->x, comppinarray[i]->y);

            display_error++;
            error++;
         }
      }
      else
      {
         // pin not found ?????
      }
   }

   return error;
}

/****************************************************************************/
/*
*/
static int update_pin(FileStruct *pcbfile, BlockStruct *compGeomBlk, const char *pinname, double x, double y, const char *padstack)
{
   int      found = 0;

   if (pcbfile != NULL)
   {
      DataStruct *d = datastruct_from_refdes(doc, compGeomBlk, pinname);

      if (d)
      {
         BlockStruct *b = Graph_Block_Exists(doc, padstack, pcbfile->getFileNumber());
         d->getInsert()->setBlockNumber(doc->Get_Block_Num(padstack, pcbfile->getFileNumber(), FALSE));
         d->getInsert()->setOriginX(x);
         d->getInsert()->setOriginY(y);
      }
      else
      {
#ifdef _DEBUG
         fprintf(ferr, "Can not find pin [%s] [%s]!\n", compGeomBlk->getName(), pinname);
         display_error++;
#endif
      }
   }

   return found;
}


/****************************************************************************/
/*
   Copy every block with the compname
   Update all pins with comppin_list
*/
static int make_localcomps(FileStruct *pcbfile)
{
   if (pcbfile == NULL)
      return 1;

   int i, p;
	CString newgeom;

   for (i=0; i<comparray.GetCount(); i++)
   {
      UNIDATComp* unidatComponent = comparray[i];
		if (unidatComponent == NULL)
			continue;

      BlockStruct *b = Graph_Block_Exists(doc, unidatComponent->GetGeom(), pcbfile->getFileNumber());
		if (b == NULL)
			continue;

      // if there is an alphapin, need to copy decal into type_decal and
      // later loop through every pinnr and update the pinname
      newgeom.Format("%s_%s", unidatComponent->GetGeom(), unidatComponent->GetRefname());
      BlockStruct *b1 = Graph_Block_On(GBO_APPEND,newgeom, pcbfile->getFileNumber(),0);
      b1->setBlockType(BLOCKTYPE_PCBCOMPONENT);
      Graph_Block_Copy(b, 0, 0, 0, 0, 1, -1, TRUE);
      Graph_Block_Off();

		// recalculate the extents for the outline if the user expects them to be absolution pin locations
		CTMatrix mat;
		if (CompPinLocationRule == CP_RULE_ABSOLUTE)
		{
         if (unidatComponent->GetMirror())
				mat.scale(-1, 1);
         mat.translate(-unidatComponent->GetData()->getInsert()->getOrigin2d());
         mat.rotateDegrees(-unidatComponent->GetRotation());
		}

      double xmin = FLT_MAX, ymin = FLT_MAX, xmax = -FLT_MAX, ymax = -FLT_MAX;
      for (p=0;p < comppinarray.GetCount();p++)
      {
         UNIDATCompPin* unidatPin = comppinarray[p];

         if (unidatPin->comppin->getRefDes().Compare(unidatComponent->GetRefname()) == 0)
         {
            CString padstack;
            double x, y;
            x = unidatPin->x;
            y = unidatPin->y;

            if (unidatComponent->GetMirror())
            {
					x = -x;

					if (CompPinRotationRule == CP_RULE_ABSOLUTE)
					{
						padstack = make_padstack(pcbfile, unidatPin->GetBottomPadName(),unidatPin->GetBottomPadRotation_InDegs(), 
							unidatPin->GetTopPadName(),unidatPin->GetTopPadRotation_InDegs());
					}
					else
					{
                  padstack = make_padstack(pcbfile, unidatPin->GetBottomPadName(),unidatPin->GetBottomPadRotation_InDegs() - unidatComponent->GetRotation(), 
                     unidatPin->GetTopPadName(),unidatPin->GetTopPadRotation_InDegs() - unidatComponent->GetRotation());
					}
            }
            else
            {
					if (CompPinRotationRule == CP_RULE_ABSOLUTE)
					{
						padstack = make_padstack(pcbfile, unidatPin->GetTopPadName(),unidatPin->GetTopPadRotation_InDegs(), 
														unidatPin->GetBottomPadName(),unidatPin->GetBottomPadRotation_InDegs());
					}
					else
					{
                  padstack = make_padstack(pcbfile, unidatPin->GetTopPadName(),unidatPin->GetTopPadRotation_InDegs() - unidatComponent->GetRotation(), 
                     unidatPin->GetBottomPadName(),unidatPin->GetBottomPadRotation_InDegs() - unidatComponent->GetRotation());
					}
            }

				mat.transform(x, y);
            if (xmin > x)  xmin = x;
            if (ymin > y)  ymin = y;
            if (xmax < x)  xmax = x;
            if (ymax < y)  ymax = y;

            update_pin(pcbfile, b1, unidatPin->comppin->getPinName(), x, y, padstack);
         }
      }

      if (xmin < xmax)
      {
         double   offsetmil = 25 * Units_Factor(UNIT_MILS, PageUnits);
         xmin -= offsetmil;
         ymin -= offsetmil;
         xmax += offsetmil;
         ymax += offsetmil;

         // here make a new outline 50 mil larger than pin extents
         BlockStruct *b1 = Graph_Block_On(GBO_APPEND,newgeom, pcbfile->getFileNumber(),0);

         int laynr = unidat_create_layer("PLACE_BOTTOM","",0);
         laynr = unidat_create_layer("PLACE_TOP","",0);
         DataStruct *d = Graph_PolyStruct(laynr, 0 , FALSE);
         Graph_Poly(NULL, 0, 0, 0, 1);
         Graph_Vertex(xmin, ymin, 0.0);
         Graph_Vertex(xmax, ymin, 0.0);
         Graph_Vertex(xmax, ymax, 0.0);
         Graph_Vertex(xmin, ymax, 0.0);
         Graph_Vertex(xmin, ymin, 0.0);
         Graph_Block_Off();
      }

      // change old block to new one !
      unidatComponent->GetData()->getInsert()->setBlockNumber(b1->getBlockNumber());

      TypeStruct *t = AddType(pcbfile, newgeom);
      t->setBlockNumber( unidatComponent->GetData()->getInsert()->getBlockNumber());
      doc->SetUnknownAttrib(&unidatComponent->GetData()->getAttributesRef(), ATT_TYPELISTLINK, newgeom, SA_OVERWRITE, NULL); //  
   }
   
   return 1;
}

/******************************************************************************
* create_layer
*/
static int unidat_create_layer(CString layerName, CString prefix, BOOL floating)
{
	int layerNum = Graph_Level(layerName, prefix, floating);
	CString baseLayerName = layerName.Mid(0, layerName.GetLength()-3);
	CString layerNameSuffix = layerName.Mid(layerName.GetLength()-3);

	LayerStruct *layer = doc->getLayerAt(layerNum);
	if (layerNameSuffix == "_CS")
	{
		layer->setLayerType(layerTypeSilkTop);

		CString botLayerName = baseLayerName + "_SS";
		int botLayerNum = Graph_Level(botLayerName, prefix, floating);
		doc->getLayerAt(botLayerNum)->setLayerType(layerTypeSilkBottom);

		Graph_Level_Mirror(layerName, botLayerName, "");
	}

	return layerNum;
}

/****************************************************************************/
/*
   Search for the token in a command token list.
*/
static int tok_search(char *lp, List *tok_lst,int tok_size)
{
   int      i;

   for (i = 0; i < tok_size; ++i)
   {
      // this must look for the complete token, because
      // PAD and PADSTACK is a difference
      if (!STRICMP(lp,tok_lst[i].token))  
         return(i);
   }
   return(-1);
}

/****************************************************************************/
/*
*/
static int nextsect(char *n)
{
	if (cur_list == NULL || cur_list_size <= 0)
		return -1;

   for (int i=0;i<cur_list_size;i++)
   {
      if (!_strnicmp(cur_list[i].token,n,strlen(cur_list[i].token)))
      {
         PushTok = true;
         return i;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int unidat_skip()
{
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char  *lp;

      if ((lp = strtok(ifp_line," \t\n")) == NULL) continue;
      if (nextsect(lp) > -1)  break;
   }

   return 1;
}
    

/****************************************************************************/
/*
*/
static int get_version()
{
	int version = UNIDAT_VERSION_1_2;
   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

		if (!STRCMPI(lp,"UNIDAT_VERSION"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         CString buf = lp;
			int pos = 0;
			CString version_string = buf.Tokenize(".", pos) + ".";
			version_string += buf.Tokenize(".", pos);

			if (version_string == "2.1")
				version = UNIDAT_VERSION_2_1;

			break;
		}
	}

	rewind(ifp);

	return version;
}

/****************************************************************************/
/*
*/
static int loop_unidat()		//List *c_list, int siz_of_command)
{
   // here do a while loop
   while (get_nextline(ifp_line,MAX_LINE,ifp) && !eof_found)
   {
      int code;
      char  *lp;
      CString  tmp = ifp_line;

      if ((lp = strtok(ifp_line," \t\n")) == NULL) continue;

      if ((code = nextsect(lp)) < 0)
      {
         fprintf(ferr,"Unknown UNIDAT Section [%s] at %ld\n", lp, ifp_linecnt);
         display_error++;
         unidat_skip();
      }
      else
      {
			if ((*cur_list[code].function)() < 0)
            return -1;
      }
   }


   return 1;
}

/****************************************************************************/
/*
*/
static int unidat_info()
{
   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;

      if (!STRCMPI(lp,"DATE"))
      {
         if ((lp = strtok(NULL,"\n"))  == NULL) continue;

      }
      else if (!STRCMPI(lp,"DELIMITER"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         delimiter_string = lp;
      }
      else if (!STRCMPI(lp,"DELIMETER")) // type error in some unidat files
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         delimiter_string = lp;
      }
      else if (!STRCMPI(lp,"UNITS"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         // MM, UM, MIL, INCH

         if (!STRCMPI(lp,"MM"))
            scale_factor = Units_Factor(UNIT_MM, PageUnits);
         else if (!STRCMPI(lp,"UM"))
            scale_factor = Units_Factor(UNIT_MM, PageUnits);
         else if (!STRCMPI(lp,"MIL"))
            scale_factor = Units_Factor(UNIT_MILS, PageUnits);
         else if (!STRCMPI(lp,"INCH"))
            scale_factor = Units_Factor(UNIT_INCHES, PageUnits);
         else
         {
            ErrorMessage("Units Unknown");
         }
      }
      else if (!STRCMPI(lp,"TOP_LAYER"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         top_layer = atoi(lp);
      }
      else if (!STRCMPI(lp,"BOTTOM_LAYER"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         bottom_layer = atoi(lp);
      }
      else if (!STRCMPI(lp,"CADSOFTWARE"))
      {
         if ((lp = strtok(NULL,"=\t\n")) == NULL) continue; // space removed from delimiter list for "THEDA AUTOBOARD"
         DataProducer = lp;
      }
      else
      {
         int r = 0;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int do_grafitem(FileStruct *pcbfile, const char *layername, double width, int graphclass, 
                       const char *name, int aperture)
{  
   char     *lp;
   CString  layerName, lastcommand;
   int      filled = FALSE;
   char     tmp_line[MAX_LINE];

   DataStruct *currentpoly = NULL;

   layerName = layername;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      strcpy(tmp_line, ifp_line);

      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (!STRCMPI(lp,"%ENDGRAFITEM")) break;

      if (!STRCMPI(lp,"FC"))
      {
         // fill structures 0 == no fill, > is one
         if ((lp = strtok(NULL," ,\t\n")) == NULL) continue;
         filled = atoi(lp);
      }
      else if (!STRCMPI(lp,"N"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         layerName = lp;
      }
      else if (!STRCMPI(lp,"W"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         width = cnv_tok(lp);
      }
      else if (!STRCMPI(lp,"DT"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         CString drawingType = lp;  // not used in camcad
      }
      else if (!STRCMPI(lp,"M"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         CString material = lp;  // not used in camcad
      }
      else if (!STRCMPI(lp,"H"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         CString height = lp;  // not used in camcad
      }
      else if (!STRCMPI(lp,"Z"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         CString z_offset = lp;  // not used in camcad
      }
      else if (!STRCMPI(lp,"%POLY"))
      {
         // %POLY, says collect all the grafitem stuff into this one polystruct
         int laynr = unidat_create_layer(layerName,"",0);
         LayerStruct *layer = doc->getLayerAt(laynr);
         if (graphclass == graphicClassEtch)
            layer->setLayerType(layerTypeSignal);

         if (pcbfile != NULL)
         {
            currentpoly = Graph_PolyStruct(laynr, 0 , FALSE);
            currentpoly->setGraphicClass(graphclass);

            if (strlen(name))
            {
               doc->SetUnknownAttrib(&currentpoly->getAttributesRef(), ATT_NETNAME, name, SA_OVERWRITE, NULL); 
            }
         }
      }
      else if (!STRCMPI(lp,"%ENDPOLY"))
      {
         currentpoly = NULL;
      }
      else if (!STRCMPI(lp,"%GRAFITEM"))
      {
         // This command is not exactly correct within a grafitem, grafitems are not recursive, but
         // it is being tolerated. It is caused by %PACKAGEOUTLINE and %PACKAGEDIMENSION launching into
         // do_grafitem without first reading the %GRAFITEM command line in cad. Because this function is 
         // shared between 1.2 and 2.1 formats, and because there is no way to tell if this was necessary
         // for some particular cad samples, the handling for those two commands is being left as-is
         // and we are just swallowing up this command so it does not go to the log as unrecognized.
      }
      else if (!STRCMPI(lp,"L")) // L (x,y) (x, y) ..
      {
         polyarray.empty();

         /* this allows       
         %GRAFITEM
         L (  1.200 ,  -0.250) (  1.200 ,   0.250) ( -0.070 ,   0.650) ( -1.060 ,   0.650) ( -1.060 ,  -0.650) ( -0.075 ,  -0.650) 
         ( -0.070 ,  -0.650) (  1.200 ,  -0.250) 
         %ENDGRAFITEM
         */

         if ((lp = strtok(NULL,"(), \t\n"))  == NULL) continue;

         while (lp)
         {
            Point2& p = *(new Point2());

            p.x = cnv_tok(lp);

            if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

            p.y = cnv_tok(lp);

            p.bulge = 0;
            polyarray.Add(&p);  // do it double because this point is used

            lp = strtok(NULL,"(), \t\n");
         }

         // here test if  the line continues
         while (get_nextline(ifp_line,MAX_LINE,ifp))
         {
            strcpy(tmp_line, ifp_line);

            if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

            if (lp[0] == '(') // line continues
            {
               if ((lp = strtok(tmp_line,"(), \t\n")) == NULL) continue;

               while (lp)
               {
                  Point2& p = *(new Point2());

                  p.x = cnv_tok(lp);

                  if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

                  p.y = cnv_tok(lp);

                  p.bulge = 0;
                  polyarray.Add(&p);  // do it double because this point is used

                  lp = strtok(NULL,"(), \t\n");
               }
            }
            else
            {
               PushTok = true;
               break;
            }
         }

         if (pcbfile != NULL)
         {
            if (aperture)
            {
               int      i, err;
               double   width, height;
               double   x1, y1, x2, y2;

               Point2& p2 = *(polyarray.ElementAt(0));
               x2 = x1 = p2.x;
               y2 = y1 = p2.y;

               for (i=0;i < polyarray.GetCount();i++)
               {
                  Point2& p2 = *(polyarray.ElementAt(i));

                  if (p2.x < x1) x1 = p2.x;
                  if (p2.x > x2) x2 = p2.x;
                  if (p2.y < y1) y1 = p2.y;
                  if (p2.y > y2) y2 = p2.y;
               }

               width = x2 - x1;
               height = y2 - y1;
               double offx = x1 + width/2;
               double offy = y1 + height/2;
               // Named apertures are file specific
               int wi = Graph_Aperture(pcbfile->getFileNumber(), name , T_RECTANGLE, width, height, offx,offy, 0.0, 0, BL_WIDTH, 0, &err);
            }
            else
            {
               if (currentpoly == NULL)
               {
                  // Not collecting all the grafitem stuff into one polystruct so make a new
                  // polystruct for this polyline.
                  int laynr = unidat_create_layer(layerName,"",0);
                  LayerStruct *layer = doc->getLayerAt(laynr);
                  if (graphclass == graphicClassEtch)
                     layer->setLayerType(layerTypeSignal);

                  DataStruct *d = Graph_PolyStruct(laynr, 0 , FALSE);
                  d->setGraphicClass(graphclass);

                  if (strlen(name))
                  {
                     doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, name, SA_OVERWRITE, NULL); //  
                  }
               }

               int err;
               // Unnamed widths are global, no file number
               int wi = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
               int voidp = FALSE;
               int closed = FALSE;
               Graph_Poly(NULL, wi, filled, voidp, closed);

               for (int i = 0; i < polyarray.GetCount(); i++)
               {
                  Point2& p2 = *(polyarray.ElementAt(i));
                  Graph_Vertex(p2.x, p2.y, p2.bulge);
               }
            }
         }
      }
      else if (!STRCMPI(lp,"C")) // C x,y,r
      {
         double x, y ,r;

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         x = cnv_tok(lp);

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         y = cnv_tok(lp);

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         r = cnv_tok(lp);
         if (CircleDefRule == CD_RULE_DIAMETER)
            r /= 2.0;

         if (pcbfile != NULL)
         {
            if (aperture)
            {
               int err;
               // Name apertures are file specific
               int wi = Graph_Aperture(pcbfile->getFileNumber(), name , T_ROUND, r , 0.0, x, y, 0.0, 0, BL_WIDTH, 0, &err);
            }
            else
            {
               int err;
               // Unnamed widths are global, no file number
               int wi = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
               int laynr = unidat_create_layer(layerName,"",0);
               LayerStruct *layer = doc->getLayerAt(laynr);
               if (graphclass == graphicClassEtch)
                  layer->setLayerType(layerTypeSignal);

               int neg = FALSE;

               if (currentpoly == NULL)
               {
                  // Individual circly
                  DataStruct *d = Graph_Circle(laynr,x, y, r, 0, wi, neg, filled);
                  d->setGraphicClass(graphclass);
                  if (strlen(name))
                  {
                     doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, name,
                        SA_OVERWRITE, NULL); // x, y, rot, height
                  }
               }
               else
               {
                  // Collecting into current polystruct
                  int voidp = FALSE;
                  int closed = FALSE;
                  Graph_Poly(NULL, wi, filled, voidp, closed);
                  CPoly *p = Graph_Poly_Circle(x, y, r);
                  p->setFilled(filled);
                  p->setWidthIndex(wi);
               }
            }
         }
      }
      else if (!STRCMPI(lp,"A")) // A x,y,r,startangle,deltaangle
      {
         double x, y , r, start, delta;

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         x = cnv_tok(lp);

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         y = cnv_tok(lp);

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         r = cnv_tok(lp);

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         start = atof(lp)*arc_unit;

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

         delta = atof(lp)*arc_unit;

         if (fabs(delta) < 1)
         {
            // delta in arc is wrong !
         }
         else if (pcbfile != NULL)
         {
				int err;
            // Unnamed widths are global, no file number
				int wi = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
				int laynr = unidat_create_layer(layerName,"",0);
				LayerStruct *layer = doc->getLayerAt(laynr);
				if (graphclass == graphicClassEtch)
					layer->setLayerType(layerTypeSignal);

            if (currentpoly == NULL)
            {
				   DataStruct *d = Graph_Arc( laynr, x, y, // center coords
												   r,                // radius
												   DegToRad(start), DegToRad(delta), 0, // start & delta angle,flag
												   wi, FALSE); // index of width table

				   d->setGraphicClass(graphclass);

				   if (strlen(name))
				   {
					   doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, name,
						   SA_OVERWRITE, NULL); // x, y, rot, height
				   }
            }
            else
            {
               int voidp = FALSE;
               int closed = FALSE;
               Graph_Poly(NULL, wi, filled, voidp, closed);
               CPoly *p = Graph_Poly_Arc(x, y, r, DegToRad(start), DegToRad(delta));
               p->setWidthIndex(wi);
            }
         }
      }
      else if (!STRCMPI(lp,"T")) // x,y,r,height,textstring
      {
         // Skipping text
      }
      else if (!STRCMPI(lp,"TR")) // x,y,r,align,height,textstring
      {
         // Skipping text refname (we alredy do reframes another way)
      }
      else
      {
         fprintf(ferr,"Unknown Grafitem [%s] at %ld\n", lp, ifp_linecnt);
         display_error++;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_outline()
{

   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;

      if (!STRCMPI(lp,"%GRAFITEM"))
      {
         // Defaults for board
         CString layername = "BOARD_OUTLINE";
         int layertype = LAYTYPE_BOARD_OUTLINE;
         int gr_class = GR_CLASS_BOARD_GEOM;
         
         // Override if panel
         //if (global_pcbfile->getBlockType() == blockTypePanel)
         if (GetPcbFile()->getBlockType() == blockTypePanel)
         {
            layername = "PANEL_OUTLINE";
            layertype = LAYTYPE_PANEL_OUTLINE;
            gr_class = GR_CLASS_PANELOUTLINE;
         }

         int laynr = unidat_create_layer(layername, "", 0);
         LayerStruct *l = doc->FindLayer(laynr);
         l->setLayerType(layertype);

         do_grafitem(GetPcbFile(), layername, 0, gr_class, "", FALSE);
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_fiducials()
{
   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;

   }


   return 1;
}

/****************************************************************************/
/*
*/
static char *expand_delimiter(char *ifp_line, const char *delimeter)
{
   int   i;
   CString s;
   s = "";

   for (i=0;i<(int)strlen(ifp_line);i++)
   {
      if (strchr(delimeter,ifp_line[i]))
      {
         s += " ";
      }

      s += ifp_line[i];
   }

   strcpy(ifp_line, s);

   return ifp_line;
}

/****************************************************************************/
/*
*/
int unidat_component()
{
   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"\n")) == NULL) continue;
      if (nextsect(lp) > -1)  break;

      // here once more with the delimiter
      expand_delimiter(ifp_line, delimiter_string);

      if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;   // component name
      CString compname(lp);
      compname.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // part number
      CString partnumber(lp);
      partnumber.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // cad part identifier

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // part type
      CString parttype(lp);
      parttype.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // full user desc

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // shape
      CString shapename(lp);
      shapename.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // degree

      double rot = atof(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // top or bottom

      int mir = !STRCMPI(lp,"bottom ");

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // x

      double x = cnv_tok(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // y

      double y = cnv_tok(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // x

      double xcen = cnv_tok(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // y

      double ycen = cnv_tok(lp);

      CString  smd, value;
      double   postol, negtol;

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // smd or tht

      smd = lp;

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // optional value

      value = lp;

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // plustol

      postol = atof(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // negtol

      negtol = atof(lp);

      CString user1; 
      CString user2; 
      CString user3; 

      if (lp = strtok(NULL, delimiter_string))
         user1 = lp;    // user1

      if (lp = strtok(NULL, delimiter_string))
         user2 = lp;    // user2

      if (lp = strtok(NULL, delimiter_string))
         user3 = lp;    // user3

      int filenumber = GetPcbFile()->getFileNumber();
      UNIDATComp  *c = new UNIDATComp(filenumber, compname, shapename, "", mir, round(rot));
      comparray.Add(c);

      //if (mir)
      //{
      //   rot = 360 - rot;
      //}  

      while (rot < 0)      rot += 360;
      while (rot >= 360)   rot -= 360;

      if (global_pcbfile != NULL)
      {
         DataStruct *d = Graph_Block_Reference(shapename, compname, 
            global_pcbfile->getFileNumber(), x, y, DegToRad(rot), mir , 1.0, 0, FALSE);

         c->SetData( d );

         if (strlen(compname))
         {
            user1.Trim();
            user2.Trim();
            user3.Trim();

            // Case 2007 says make parttype (4th field) the part number attribute
            // for Theda Autoboard produced files. All others get the part number for
            // the part number attribute.
            if (DataProducer.CompareNoCase("THEDA AUTOBOARD") == 0)
            {
               if (!parttype.IsEmpty())
                  doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_PARTNUMBER, parttype, SA_OVERWRITE, NULL);        
            }
            else if (!partnumber.IsEmpty())
            {
               doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_PARTNUMBER, partnumber, SA_OVERWRITE, NULL); 
            }

            if (postol == negtol && postol > 0)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TOLERANCE, 1),
                  VT_DOUBLE,
                  &postol, SA_OVERWRITE,NULL); // x, y, rot, height
            }
            else
            {
               if (postol > 0)
               {
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_PLUSTOLERANCE, 1),
                     VT_DOUBLE,
                     &postol, SA_OVERWRITE,NULL); // x, y, rot, height
               }

               if (negtol > 0)
               {
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_MINUSTOLERANCE, 1),
                     VT_DOUBLE,
                     &negtol, SA_OVERWRITE,NULL); // x, y, rot, height
               }
            }

            if (strlen(value))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap(ATT_VALUE), value, SA_OVERWRITE,NULL); // 

            if (strlen(user1))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap("USER1"), user1,SA_APPEND, NULL);  

            if (strlen(user2))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap("USER2"), user2,SA_APPEND, NULL);  

            if (strlen(user3))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap("USER3"), user3,SA_APPEND, NULL);  

            d->getInsert()->setInsertType(insertTypePcbComponent);

            if (CREATE_LIBRARY)
            {
               TypeStruct *t = AddType(global_pcbfile, shapename);
               t->setBlockNumber( d->getInsert()->getBlockNumber());
               doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TYPELISTLINK, shapename, SA_OVERWRITE, NULL); //  
            }
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_other_drillings()
{
   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_pad()
{
   char     *lp;
   int      pad_open = FALSE;
   CString  padname;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;

      if (!STRCMPI(lp,"PAD"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         padname.Format("PAD %s", lp); // can have the same name as other shapes.
      }
      else if (!STRCMPI(lp,"%GRAFITEM"))
      {
         int laynr = unidat_create_layer("0","",1);
         do_grafitem(global_pcbfile, "0", 0, 0, padname, TRUE);
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int get_drillindex(double size, int layernum)
{
   int            i;

   if (size == 0) return -1;

   for (i=0;i < drillarray.GetCount();i++)
   {
      UNIDATDrill& p = *(drillarray.ElementAt(i));

      if (p.d == size)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d",drillarray.GetCount());

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   UNIDATDrill& p = *(new UNIDATDrill());
   p.d = size;
   drillarray.Add(&p);  

   return drillarray.GetCount() - 1;
}

/*****************************************************************************/
/*
*/
static CString make_padstack(FileStruct *pcbfile, const char *ptop, double toprot, const char *pbot, double botrot)
{
   CString  padstack;

   if (pcbfile != NULL)
   {
      CString padtop, padbot;

      if (strlen(ptop))
         padtop.Format("PAD %s", ptop);   // can have the same name as other shapes.
      else
         padtop = "";

      padtop.TrimLeft();
      padtop.TrimRight();

      if (strlen(pbot))
         padbot.Format("PAD %s", pbot);   // can have the same name as other shapes.
      else
         padbot = "";

      padbot.TrimLeft();
      padbot.TrimRight();

      // padstack is top_rot_bot_rot
      
      int      smd = FALSE;

      if (strlen(padtop) && strlen(padbot))
      {
         padstack.Format("ALL_%s_%1.1lf_%s_%1.1lf", padtop, toprot, padbot, botrot);
      }
      else
      {
         smd = TRUE;

         if (strlen(padtop))
         {
            padstack.Format("TOP_%s_%1.1lf", padtop, toprot);
         }
         else if (strlen(padbot))
         {
            padstack.Format("BOT_%s_%1.1lf", padbot, botrot);
         }
      }

      if (!Graph_Block_Exists(doc, padstack, pcbfile->getFileNumber()))
      {
         BlockStruct *b = Graph_Block_On(GBO_APPEND, padstack, pcbfile->getFileNumber(), 0);
         b->setBlockType(BLOCKTYPE_PADSTACK);

         if (strlen(padtop))
         {
            Graph_Block_Reference(padtop, "", pcbfile->getFileNumber(), 
               0.0, 0.0, DegToRad(toprot), 0, 1.0, unidat_create_layer("PADTOP","",0), FALSE);
         }

         if (strlen(padbot))
         {
            Graph_Block_Reference(padbot, "", pcbfile->getFileNumber(), 
               0.0, 0.0, DegToRad(botrot), 0, 1.0, unidat_create_layer("PADBOT","",0), FALSE);
         }

         if (strlen(padbot))
         {
            Graph_Block_Reference(padbot, "", pcbfile->getFileNumber(), 
               0.0, 0.0, DegToRad(botrot), 0, 1.0, unidat_create_layer("PADINNER","",0), FALSE);
         }

         Graph_Block_Off();
      }
   }

   return padstack;
}

/****************************************************************************/
/*
*/
int unidat_shape()
{
   if (global_pcbfile != NULL)
   {
      int laynr = unidat_create_layer("COMPOUTLINE_TOP","",0);
      LayerStruct *l = doc->FindLayer(laynr);
      l->setLayerType(LAYTYPE_SILK_TOP);

      laynr = unidat_create_layer("COMPOUTLINE_BOTTOM","",0);
      l = doc->FindLayer(laynr);
      l->setLayerType(LAYTYPE_SILK_BOTTOM);

      laynr = unidat_create_layer("PADTOP","",0);
      l = doc->FindLayer(laynr);
      l->setLayerType(LAYTYPE_PAD_TOP);

      laynr = unidat_create_layer("PADBOT","",0);
      l = doc->FindLayer(laynr);
      l->setLayerType(LAYTYPE_PAD_BOTTOM);

      laynr = unidat_create_layer("PADINNER","",0);
      l = doc->FindLayer(laynr);
      l->setLayerType(LAYTYPE_PAD_INNER);

      Graph_Level_Mirror("PADTOP", "PADBOT", "");
      Graph_Level_Mirror("PACKAGE_TOP", "PACKAGE_BOTTOM", "");
      Graph_Level_Mirror("COMPOUTLINE_TOP", "COMPOUTLINE_BOTTOM", "");
   }

   char     *lp;
   int      shape_open = FALSE;
   int      stat;
   char     tmp_line[MAX_LINE];

   PushTok = false;

   CString  shapename;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      strcpy(tmp_line, ifp_line);

      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;


      if (!STRCMPI(lp,"SHAPE"))
      {
         if (global_pcbfile != NULL)
         {
            if (shape_open)   Graph_Block_Off();
            if ((lp = strtok(NULL,"\n")) == NULL) continue;

            shapename = lp;
            BlockStruct *b = Graph_Block_On(GBO_APPEND, shapename, global_pcbfile->getFileNumber(), 0);
            b->setBlockType(BLOCKTYPE_PCBCOMPONENT);
            shape_open = TRUE;
         }
      }
      else if (!STRCMPI(lp,"%SHAPEOUTLINE"))
      {
         stat = 1;
      }
      else if (!STRCMPI(lp,"%PACKAGEDIMENSION"))
      {
         stat = 2;
      }
      else if (!STRCMPI(lp,"%GRAFITEM"))
      {
         if (stat == 1)
         {
            do_grafitem(global_pcbfile, "COMPOUTLINE_TOP", 0, GR_CLASS_COMPOUTLINE, "", FALSE);
         }
         else if (stat == 2)
         {
            do_grafitem(global_pcbfile, "PACKAGE_TOP", 0, 0, "", FALSE);
         }
      }
      else if (!STRCMPI(lp,"%PINLIST"))
      {
         stat = 3;
      }
      else
      {
         if (stat == 3)
         {
            expand_delimiter(tmp_line, delimiter_string);

            if ((lp = strtok(tmp_line, delimiter_string)) == NULL)   // pinnr
               continue;  

            CString pinnr = lp;
            pinnr.TrimLeft();
            pinnr.TrimRight();

            if ((lp = strtok(NULL, delimiter_string)) == NULL)       // x
               continue;

            double x = cnv_tok(lp);

            if ((lp = strtok(NULL, delimiter_string)) == NULL)       // y
               continue;   

            double y = cnv_tok(lp);

            if ((lp = strtok(NULL, delimiter_string)) == NULL)       // padtop, optional
               continue;   

            CString padtop = lp;
            padtop.TrimLeft();
            padtop.TrimRight();

            if ((lp = strtok(NULL, delimiter_string)) == NULL)       // rot top
               continue;  

            double toprot = atof(lp);

            CString padbot = "";
            double botrot = 0;

            if (lp = strtok(NULL, delimiter_string))                 // padbot is optional
            {
               padbot = lp;
               padbot.TrimLeft();
               padbot.TrimRight();

               if (lp = strtok(NULL, delimiter_string))              // rot bot
                  botrot = atof(lp);
            }

            if (global_pcbfile != NULL)
            {
               CString padstack  = make_padstack(global_pcbfile, padtop, toprot, padbot, botrot);

               DataStruct *data = Graph_Block_Reference(padstack, pinnr, global_pcbfile->getFileNumber(), x, y, DegToRad(0), 0, 1.0,unidat_create_layer("0", "", 1), FALSE);
               data->getInsert()->setInsertType(insertTypePin);
            }
         }
      }
   }

   if (shape_open)   Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_via()
{
   char     *lp;

   int laynr = unidat_create_layer("PADALL","",0);
   LayerStruct *l = doc->FindLayer(laynr);
   l->setLayerType(LAYTYPE_PAD_ALL);

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (nextsect(lp) > -1)  break;

      if (global_pcbfile != NULL)
      {
         CString   vianame, netname, pad, via;
         double    x, y, rot; 

         // here once more with the delimiter
         expand_delimiter(ifp_line, delimiter_string);

         if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;   // via name

         vianame = lp;
         vianame.TrimLeft();
         vianame.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // net name

         netname = lp;
         netname.TrimLeft();
         netname.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // x

         x = cnv_tok(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // x

         y = cnv_tok(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // begin layer
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // endlayer
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // padtop

         via = lp;
         via.TrimLeft();
         via.TrimRight();
         pad.Format("PAD %s", via); // can have the same name as other shapes.

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // rot top

         rot = atof(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // accesstop
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // accessbot

         // padstack is top_rot_bot_rot
         CString  padstack;

         padstack.Format("VIA_%s_%1.1lf", via, rot);

         if (!Graph_Block_Exists(doc, padstack, global_pcbfile->getFileNumber()))
         {
            BlockStruct *b = Graph_Block_On(GBO_APPEND, padstack, global_pcbfile->getFileNumber(), 0);
            b->setBlockType(BLOCKTYPE_PADSTACK);

            Graph_Block_Reference(pad, "", global_pcbfile->getFileNumber(), 
               0.0, 0.0, DegToRad(rot), 0, 1.0, unidat_create_layer("PADALL","",0), FALSE);
            Graph_Block_Off();
         }

         DataStruct  *d = Graph_Block_Reference(padstack, vianame, global_pcbfile->getFileNumber(), 
            x, y, 0.0, 0, 1.0, -1, FALSE);
         d->getInsert()->setInsertType(insertTypeVia);

         if (strlen(netname))
         {
            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, netname, SA_OVERWRITE, NULL); //  
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_track()
{
   char     *lp;
   CString  netname;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;

      if (!STRCMPI(lp,"NET"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;
         netname = lp;
      }
      else if (!STRCMPI(lp,"%GRAFITEM"))
      {
         do_grafitem(global_pcbfile, "", 0, GR_CLASS_ETCH, netname, FALSE);

      }
      else
      {
         int r = 0;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_skipsection()
{
   char     *lp;
   CString  netname;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_component_pin()
{
   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (nextsect(lp) > -1)  break;

      if (global_pcbfile != NULL)
      {
         CString  compname, pinname, pinnr, netname;
         // here once more with the delimiter
         expand_delimiter(ifp_line, delimiter_string);

         if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;   // component name

         compname = lp;
         compname.Trim();

         // this is wrong in the description first pin_nr and than name
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      //pinnr

         pinnr = lp;
         pinnr.Trim();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // pinname

         pinname = lp;
         pinname.Trim();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // netname

         netname = lp;
         netname.Trim();

         NetStruct   *n = NULL;
         if (netname.IsEmpty())
         {
            n = add_net(global_pcbfile, NET_UNUSED_PINS);
            n->setFlags(NETFLAG_UNUSEDNET);
         }
         else
         {
            n = add_net(global_pcbfile, netname);
         }

         // Case dts0100405632 NOT!  This case should have been applied to 2.1 format only, following case
         // is because it was applied to 1.2 format also.
         // Case dts0100409390 
         // Format 2.1 uses pin names, in format 1.2 we need to stick with pin numbers, in case
         // sample data the pin name is sometimes absent and someimes duplicated.
         // Cad sample udr.uni has both situation one one part: CN3.
         CompPinStruct *p = add_comppin(global_pcbfile, n, compname, pinnr);

         double x, y;

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // pinx
         x = cnv_tok(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // piny
         y = cnv_tok(lp);

         CString ptop, pbot;
         double  toprot, botrot;

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // ptop
         ptop = lp;
         ptop.TrimLeft();
         ptop.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // toprot
         toprot = atof(lp);

         pbot = "";

         if (lp = strtok(NULL, delimiter_string))        // pbot
         {
            pbot = lp;
            pbot.TrimLeft();
            pbot.TrimRight();

            if (lp = strtok(NULL, delimiter_string))     // botrot
               botrot = atof(lp);
         }

         UNIDATCompPin  *c = new UNIDATCompPin(p);
         comppinarray.Add(c);
         c->x       = x;
         c->y       = y;
         c->padbot  = pbot;
         c->SetBottomPadRotation_InDegs(botrot);
         c->padtop  = ptop;
         c->SetTopPadRotation_InDegs(toprot);
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_via_v21()
{
   char     *lp;
   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (nextsect(lp) > -1)  break;

      if (global_pcbfile != NULL)
      {
         CString   vianame, netname, padTop, padBot;
         double    x, y, topRot, botRot;

         // here once more with the delimiter
         expand_delimiter(ifp_line, delimiter_string);

         if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;   // via name
         vianame = lp;
         vianame.TrimLeft();
         vianame.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // net name
         netname = lp;
         netname.TrimLeft();
         netname.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // x
         x = cnv_tok(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // y
         y = cnv_tok(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // begin layer
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // endlayer
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // padtop
         padTop = lp;
         padTop.Trim();
         //padTop.MakeUpper(); Case dts0100405632, Don't do MakeUpper here, it is not done consistently and is causing failure to find geometries

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // rot top
         topRot = atof(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // material top

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // padbot
         padBot = lp;
         padBot.Trim();
         //padBot.MakeUpper(); Case dts0100405632, Don't do MakeUpper here, it is not done consistently and is causing failure to find geometries

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // rot bot
         botRot = atof(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // material bot

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // drill
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // rot drill
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // material drill
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // accesstop
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // accessbot

         BOOL shouldMirror = FALSE;
         if (padTop.IsEmpty() && !padBot.IsEmpty())	// no top pad
            shouldMirror = TRUE;

         // padstack is top_rot_bot_rot
         CString padstack;
         if (shouldMirror)
            padstack = make_padstack(global_pcbfile, padBot, botRot, padTop, topRot);
         else
            padstack = make_padstack(global_pcbfile, padTop, topRot, padBot, botRot);

         DataStruct  *d = Graph_Block_Reference(padstack, vianame, global_pcbfile->getFileNumber(), 
            x, y, 0.0, shouldMirror, 1.0, -1, FALSE);
         d->getInsert()->setInsertType(insertTypeVia);

         if (strlen(netname))
         {
            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, netname, SA_OVERWRITE, NULL); //  
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_testpoints_v21()
{
   char     *lp;
   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (nextsect(lp) > -1)  break;

      if (global_pcbfile != NULL)
      {
         CString   tpname, netname, padTop, padBot;
         double    x, y, topRot, botRot;

         // here once more with the delimiter
         expand_delimiter(ifp_line, delimiter_string);

         if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;   // via name
         tpname = lp;
         tpname.TrimLeft();
         tpname.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // net name
         netname = lp;
         netname.TrimLeft();
         netname.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // x
         x = cnv_tok(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // y
         y = cnv_tok(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // padtop
         padTop = lp;
         padTop.TrimLeft();
         padTop.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // rot top
         topRot = atof(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // material top

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // padbot
         padBot = lp;
         padBot.TrimLeft();
         padBot.TrimRight();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // rot bot
         botRot = atof(lp);

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;   // material bot

         BOOL shouldMirror = FALSE;
         if (padTop.IsEmpty() && !padBot.IsEmpty())	// no top pad
            shouldMirror = TRUE;

         // padstack is top_rot_bot_rot
         CString padstack;
         if (shouldMirror)
            padstack = make_padstack(global_pcbfile, padBot, botRot, padTop, topRot);
         else
            padstack = make_padstack(global_pcbfile, padTop, topRot, padBot, botRot);


         // Testpoint that use the same padstack can share the same pin geometry definition.
         // So differentiate pin geom block name by padstack.

         CString pinName;
         pinName.Format("TESTPOINT_PIN__%s", padstack);
         BlockStruct *block = Graph_Block_Exists(doc, pinName, global_pcbfile->getFileNumber());
         if (block == NULL)
         {
            block = Graph_Block_On(GBO_OVERWRITE, pinName, global_pcbfile->getFileNumber(), 0);
            DataStruct *d = Graph_Block_Reference(padstack, "1", global_pcbfile->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, -1, FALSE);
            d->getInsert()->setInsertType(insertTypePin);
            Graph_Block_Off();
         }

         DataStruct *d = Graph_Block_Reference(pinName, tpname, global_pcbfile->getFileNumber(), 
            x, y, 0.0, shouldMirror, 1.0, -1, FALSE);
         d->getInsert()->setInsertType(insertTypeTestPoint);

         if (strlen(netname))
         {
            doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, netname, SA_OVERWRITE, NULL);
            add_comppin(global_pcbfile, FindNet(global_pcbfile, netname), tpname, "1");
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int do_grafitem_pads_v21(FileStruct *pcbfile, const char *layername, double width, int graphclass, 
                       const char *name, int aperture)
{  
   char     *lp;
   CString  layer, lastcommand;
   int      filled = FALSE;
   char     tmp_line[MAX_LINE];
	CStringArray strArray;
	bool		foundLineCommand = false;

   lastcommand = "";
   layer = layername;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      strcpy(tmp_line, ifp_line);

      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (!STRCMPI(lp,"%ENDGRAFITEM")) break;

      if (!STRCMPI(lp,"FC"))
      {
         // fill structures 0 == no fill, > is one
         if ((lp = strtok(NULL," ,\t\n")) == NULL) continue;
         filled = atoi(lp);
      }
      else if (!STRCMPI(lp,"N"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         layer = lp;
      }
      else if (!STRCMPI(lp,"W"))
      {
         if ((lp = strtok(NULL," \t\n"))  == NULL) continue;
         width = cnv_tok(lp);
      }
		else if (!STRCMPI(lp,"L")) // L (x,y) (x, y) ..
      {
			/* this is allows       
			%GRAFITEM
				L (  1.200 ,  -0.250) (  1.200 ,   0.250) ( -0.070 ,   0.650) ( -1.060 ,   0.650) ( -1.060 ,  -0.650) ( -0.075 ,  -0.650) 
					( -0.070 ,  -0.650) (  1.200 ,  -0.250) 
			%ENDGRAFITEM
			*/
			int lastAddition = strArray.Add(tmp_line);
			foundLineCommand = true;

         // here test if  the line continues
         while (get_nextline(ifp_line,MAX_LINE,ifp))
         {
            strcpy(tmp_line, ifp_line);

            if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

            if (lp[0] == '(') // line continues
            {
					strArray.SetAt(lastAddition, strArray[lastAddition] + tmp_line);
            }
            else
            {
               PushTok = true;
               break;
            }
         }
		}
      else if (!STRCMPI(lp,"C")) // C x,y,r
			strArray.Add(tmp_line);
      else if (!STRCMPI(lp,"A")) // A x,y,r,startangle,deltaangle
			strArray.Add(tmp_line);
	}

	if (strArray.GetCount() == 1 && !foundLineCommand)
	{
		strcpy(ifp_line, strArray[0]);
		strcpy(tmp_line, strArray[0]);

		if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) return 1;

		if (!STRCMPI(lp,"L")) // L (x,y) (x, y) ..
		{
			polyarray.empty();
	   
			/* this is allows       
			%GRAFITEM
				L (  1.200 ,  -0.250) (  1.200 ,   0.250) ( -0.070 ,   0.650) ( -1.060 ,   0.650) ( -1.060 ,  -0.650) ( -0.075 ,  -0.650) 
					( -0.070 ,  -0.650) (  1.200 ,  -0.250) 
			%ENDGRAFITEM
			*/
			if ((lp = strtok(NULL,"(), \t\n"))  == NULL) return 1;

			while (lp)
			{
				Point2& p = *(new Point2());

				p.x = cnv_tok(lp);

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) return 1;

				p.y = cnv_tok(lp);

				p.bulge = 0;
				polyarray.Add(&p);  // do it double because this point is used

				lp = strtok(NULL,"(), \t\n");
			}

			// here test if  the line continues
			while (get_nextline(ifp_line,MAX_LINE,ifp))
			{
				strcpy(tmp_line, ifp_line);

				if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) return 1;

				if (lp[0] == '(') // line continues
				{
					if ((lp = strtok(tmp_line,"(), \t\n")) == NULL) return 1;

					while (lp)
					{
						Point2& p = *(new Point2());

						p.x = cnv_tok(lp);

						if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) return 1;

						p.y = cnv_tok(lp);

						p.bulge = 0;
						polyarray.Add(&p);  // do it double because this point is used

						lp = strtok(NULL,"(), \t\n");
					}
				}
				else
				{
					PushTok = true;
					break;
				}
			}

         if (pcbfile != NULL)
         {
            if (aperture)
            {
               int      i, err;
               double   width, height;
               double   x1, y1, x2, y2;

               Point2& p2 = *(polyarray.ElementAt(0));
               x2 = x1 = p2.x;
               y2 = y1 = p2.y;

               for (i=0;i < polyarray.GetCount();i++)
               {
                  Point2& p2 = *(polyarray.ElementAt(i));

                  if (p2.x < x1) x1 = p2.x;
                  if (p2.x > x2) x2 = p2.x;
                  if (p2.y < y1) y1 = p2.y;
                  if (p2.y > y2) y2 = p2.y;
               }

               width = x2 - x1;
               height = y2 - y1;
               double offx = x1 + width/2;
               double offy = y1 + height/2;
               // Named apertures are file specific
               Graph_Aperture(pcbfile->getFileNumber(), name , T_RECTANGLE, width, height, offx,offy, 0.0, 0, BL_WIDTH, 0, &err);
            }
            else
            {
               int i;
               int err;
               // Unnamed widths are global, no file number
               int wi = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

               int laynr = unidat_create_layer(layer,"",0);
               DataStruct *d = Graph_PolyStruct(laynr, 0 , FALSE);
               d->setGraphicClass(graphclass);

               if (strlen(name))
               {
                  doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, name, SA_OVERWRITE, NULL); //  
               }

               int voidp = FALSE;
               int closed = FALSE;
               Graph_Poly(NULL, wi, filled, voidp, closed);

               for (i=0;i < polyarray.GetCount();i++)
               {
                  Point2& p2 = *(polyarray.ElementAt(i));
                  Graph_Vertex(p2.x, p2.y, p2.bulge);
               }
            }
         }
		}
      else if (!STRCMPI(lp,"C")) // C x,y,r
      {
         double x, y ,r;

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) return 1;

         x = cnv_tok(lp);

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) return 1;

         y = cnv_tok(lp);

         if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) return 1;

         r = cnv_tok(lp);
         if (CircleDefRule == CD_RULE_DIAMETER)
            r /= 2.0;

         if (pcbfile != NULL)
         {
            if (aperture)
            {
               int err;
               // Named apertures are file specific
               int wi = Graph_Aperture(pcbfile->getFileNumber(), name , T_ROUND, r*2, 0.0, x, y, 0.0, 0, BL_WIDTH, 0, &err);
            }
            else
            {
               int err;
               // Unnamed widths are global, no file number
               int wi = Graph_Aperture("", T_ROUND, width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
               int laynr = unidat_create_layer(layer,"",0);

               int neg = FALSE;

               DataStruct *d = Graph_Circle(laynr, x, y, r, 0, wi, neg, filled);
               d->setGraphicClass(graphclass);

               if (strlen(name))
               {
                  doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_NETNAME, name,
                     SA_OVERWRITE, NULL);
               }
            }
         }
      }
	}
	else if (pcbfile != NULL)
	{
		CString subBlockName;
		subBlockName.Format("COMPLEX_%s", name);
		if (aperture)
		{
			// Create the file specific block used for the complex aperture
         Graph_Block_On(GBO_OVERWRITE, subBlockName, pcbfile->getFileNumber(), 0);

			int laynr = unidat_create_layer(layer,"",0);
			DataStruct *data = Graph_PolyStruct(laynr, 0, 0);
			data->setGraphicClass(graphclass);
         data->setAttrib(doc->getCamCadData(), doc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString), valueTypeString, (void*)name, attributeUpdateOverwrite, NULL);

			int err;
         // Unnamed widths are global, no file number
			int wi = Graph_Aperture("", T_ROUND, width, 0.0, 0, 0, 0.0, 0, BL_WIDTH, 0, &err);
			Graph_Poly(data, wi, TRUE, FALSE, TRUE);
		}

		for (int strCount=0; strCount<strArray.GetCount(); strCount++)
		{
			strcpy(ifp_line, strArray[strCount]);
			strcpy(tmp_line, strArray[strCount]);

			if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

			if (!STRCMPI(lp,"L")) // L (x,y) (x, y) ..
			{
				polyarray.empty();
	      
				/* this is allows       
				%GRAFITEM
					L (  1.200 ,  -0.250) (  1.200 ,   0.250) ( -0.070 ,   0.650) ( -1.060 ,   0.650) ( -1.060 ,  -0.650) ( -0.075 ,  -0.650) 
						( -0.070 ,  -0.650) (  1.200 ,  -0.250) 
				%ENDGRAFITEM
				*/
				if ((lp = strtok(NULL,"(), \t\n"))  == NULL) continue;

				while (lp)
				{
					Point2& p = *(new Point2());

					p.x = cnv_tok(lp);

					if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

					p.y = cnv_tok(lp);

					p.bulge = 0;
					polyarray.Add(&p);  // do it double because this point is used

					lp = strtok(NULL,"(), \t\n");
				}

				for (int polyCount=0; polyCount<polyarray.GetCount(); polyCount++)
				{
					Point2& p2 = *(polyarray.ElementAt(polyCount));
					Graph_Vertex(p2.x, p2.y, p2.bulge);
				}
			}
			else if (!STRCMPI(lp,"C")) // C x,y,r
			{
				double x, y ,r;

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				x = cnv_tok(lp);

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				y = cnv_tok(lp);

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				r = cnv_tok(lp);
				if (CircleDefRule == CD_RULE_DIAMETER)
					r /= 2.0;

				Graph_Poly_Circle(x, y, r);
			}
			else if (!STRCMPI(lp,"A")) // A x,y,r,startangle,deltaangle
			{
				double x, y , r, start, delta;

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				x = cnv_tok(lp);

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				y = cnv_tok(lp);

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				r = cnv_tok(lp);

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				start = atof(lp)*arc_unit;

				if ((lp = strtok(NULL,"() ,\t\n"))  == NULL) continue;

				delta = atof(lp)*arc_unit;

				if (fabs(delta) < 1)
				{
					// delta in arc is wrong !
				}
				else
					Graph_Poly_Arc(x, y, r, DegToRad(start), DegToRad(delta));
			}
			else if (!STRCMPI(lp,"T")) // x,y,r,height,textstring
			{
				int r = 0;
			}
			else
			{
				fprintf(ferr,"Unknown Grafitem [%s] at %ld\n", lp, ifp_linecnt);
				display_error++;
			}
		}

		if (aperture && pcbfile != NULL)
		{
			Graph_Block_Off();
         // Complex apertures are file specific
         Graph_Complex(pcbfile->getFileNumber(), name, 0, subBlockName, 0, 0, 0.0);
		}
	}

   return 1;
}

static void defineStandardLayers()
{
   int laynr = unidat_create_layer("COMPOUTLINE_TOP","",0);
   LayerStruct *l = doc->FindLayer(laynr);
   l->setLayerType(LAYTYPE_SILK_TOP);

   laynr = unidat_create_layer("COMPOUTLINE_BOTTOM","",0);
   l = doc->FindLayer(laynr);
   l->setLayerType(LAYTYPE_SILK_BOTTOM);

   laynr = unidat_create_layer("PADTOP","",0);
   l = doc->FindLayer(laynr);
   l->setLayerType(LAYTYPE_PAD_TOP);

   laynr = unidat_create_layer("PADBOT","",0);
   l = doc->FindLayer(laynr);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   laynr = unidat_create_layer("PADINNER","",0);
   l = doc->FindLayer(laynr);
   l->setLayerType(LAYTYPE_PAD_INNER);

   Graph_Level_Mirror("PADTOP", "PADBOT", "");
   Graph_Level_Mirror("PACKAGE_TOP", "PACKAGE_BOTTOM", "");
   Graph_Level_Mirror("COMPOUTLINE_TOP", "COMPOUTLINE_BOTTOM", "");
}

/****************************************************************************/
/*
*/
int updateComponents_v21(FileStruct *pcbfile)
{
   if (pcbfile != NULL)
   {
      int      i;

      defineStandardLayers();

      for (i=0;i < comparray.GetCount();i++)
      {
         UNIDATComp* unidatComponent = comparray[i];

         // checking filenumber keeps from processing previously processed components over again
         if (unidatComponent->GetFileNumber() == pcbfile->getFileNumber() && unidatComponent->GetData() != NULL)
         {
            // get the geometry of the component
            BlockStruct *block = doc->getBlockAt(unidatComponent->GetData()->getInsert()->getBlockNumber());
            if (block != NULL)
            {
               // Look for the footprint in order to properly insert the pins.
               // If there is no footprint, then use the pin list associated with the component.

               UNIDATFootPrint *ft = NULL;
               if (footprintmap.Lookup(unidatComponent->GetFootprint(), ft))
               {
                  // add the footprints to the geometry
                  for (int i=0; i<ft->GetToeCount() && !ft->IsUsed(); i++)
                  {
                     UNIDATToe *toe = ft->GetToeAt(i);

                     CString padstack  = make_padstack(pcbfile, toe->GetTopPadName(), toe->GetTopPadRotation_InDegs(),
                        toe->GetBottomPadName(), toe->GetBottomPadRotation_InDegs());

                     if (block->FindData(toe->GetName()) == NULL)
                     {
                        Graph_Block_On(block);
                        DataStruct *pin = Graph_Block_Reference(padstack, toe->GetName(), pcbfile->getFileNumber(), toe->GetPinLocation().x, toe->GetPinLocation().y,
                           0.0, 0, 1.0, -1, FALSE);
                        pin->getInsert()->setInsertType(insertTypePin);
                        Graph_Block_Off();
                     }

                     // Update compPin name to match footprint name for this pin
                     NetStruct *net = NULL;
                     CompPinStruct *cp = FindCompPin(pcbfile, unidatComponent->GetRefname(), toe->GetNumberStr(), &net);
                     if (cp != NULL)
                        cp->setPinName(toe->GetName());
                  }
               }
               else if (!CREATE_LIBRARY)  // use local definition of the geometry
               {
                  CString newGeomName;
                  newGeomName.Format("%s_%s", unidatComponent->GetGeom(), unidatComponent->GetRefname());
                  BlockStruct *newBlock = Graph_Block_On(GBO_APPEND, newGeomName, pcbfile->getFileNumber(), 0);
                  newBlock->setBlockType(BLOCKTYPE_PCBCOMPONENT);
                  Graph_Block_Copy(block, 0, 0, 0, 0, 1, -1, TRUE);
                  Graph_Block_Off();

                  for (int i=0; i<unidatComponent->GetPinCount(); i++)
                  {
                     UNIDATToe *toe = unidatComponent->GetPinAt(i);

                     // The pins in the component section are apparantly "absolute" layer, not relative.
                     // So if comp is bottom side then we have to mirror the padstack to get a top-built geom defined.

                     CString padstack;
                     if (unidatComponent->GetMirror())
                     {
                        padstack = make_padstack(pcbfile, toe->GetBottomPadName(), toe->GetBottomPadRotation_InDegs(),
                                                          toe->GetTopPadName(), toe->GetTopPadRotation_InDegs());
                     }
                     else
                     {
                        padstack = make_padstack(pcbfile, toe->GetTopPadName(), toe->GetTopPadRotation_InDegs(),
                                                          toe->GetBottomPadName(), toe->GetBottomPadRotation_InDegs());
                     }

                     update_pin(pcbfile, newBlock, toe->GetName(), toe->GetPinLocation().x, toe->GetPinLocation().y, padstack);

                     // Update compPin name to match footprint name for this pin
                     NetStruct *net = NULL;
                     CompPinStruct *cp = FindCompPin(pcbfile, unidatComponent->GetRefname(), toe->GetNumberStr(), &net);
                     if (cp != NULL)
                        cp->setPinName(toe->GetName());
                  }

                  // change old block to new one !
                  unidatComponent->GetData()->getInsert()->setBlockNumber(newBlock->getBlockNumber());

                  TypeStruct* type = AddType(pcbfile, newGeomName);
                  type->setBlockNumber(newBlock->getBlockNumber());
                  doc->SetUnknownAttrib(&unidatComponent->GetData()->getAttributesRef(), ATT_TYPELISTLINK, newGeomName, SA_OVERWRITE, NULL);  
               }
            }
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_component_v21()
{
   char     *lp;
   CString	footprintname;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"\n")) == NULL) continue;
      if (nextsect(lp) > -1)  break;

      // read the pin list section if there is no specific footprint
      if (!strcmp(lp, "%PINLIST") && footprintname.IsEmpty() && comparray.GetCount() > 0)
      {
         // get the last one comp added so we add pins to the correct place
         UNIDATComp  *c = comparray.GetAt(comparray.GetCount()-1);

         // read the pin in
         c->LoadPinList();

         continue;
      }

      // here once more with the delimiter
      expand_delimiter(ifp_line, delimiter_string);

      if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;   // component name
      CString compname(lp);
      compname.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // part number
      CString partnumber(lp);
      partnumber.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // cad part identifier

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // part type
      CString parttype(lp);
      parttype.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // full user desc

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // shape/package
      CString shapename(lp);
      shapename.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // Footprint
      footprintname = lp;
      footprintname.Trim();

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // Bond Web

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // degree
      double rot = atof(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // top or bottom
      int mir = atoi(lp) == bottom_layer;

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // x
      double x = cnv_tok(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // y
      double y = cnv_tok(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // x
      double xcen = cnv_tok(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // y
      double ycen = cnv_tok(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // smd or tht
      CString smd(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // optional value
      CString value(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // plustol
      double postol = atof(lp);

      if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // negtol
      double negtol = atof(lp);

      CString user1; 
      CString user2; 
      CString user3; 

      if (lp = strtok(NULL, delimiter_string))
         user1 = lp;    // user1

      if (lp = strtok(NULL, delimiter_string))
         user2 = lp;    // user2

      if (lp = strtok(NULL, delimiter_string))
         user3 = lp;    // user3

      int filenumber = global_pcbfile != NULL ? global_pcbfile->getFileNumber() : -1;
      UNIDATComp  *c = new UNIDATComp(filenumber, compname, shapename, footprintname, mir, round(rot));
      comparray.Add(c);

      if (mir)
      {
         rot -= 180;
      }  

      while (rot < 0)      rot += 360;
      while (rot >= 360)   rot -= 360;

      if (global_pcbfile != NULL)
      {
         DataStruct *d = Graph_Block_Reference(shapename, compname, 
            global_pcbfile->getFileNumber(), x, y, DegToRad(rot), mir , 1.0, 0, FALSE);

         c->SetData( d );

         if (strlen(compname))
         {
            user1.Trim();
            user2.Trim();
            user3.Trim();

            // Case 2007 says make partType (4th field) the part number attribute
            // for Theda Autoboard produced files. All others get the part number for
            // the part number attribute.
            if (DataProducer.CompareNoCase("THEDA AUTOBOARD") == 0)
            {
               if (!parttype.IsEmpty())
                  doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_PARTNUMBER, parttype, SA_OVERWRITE, NULL);        
            }
            else if (!partnumber.IsEmpty())
            {
               doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_PARTNUMBER, partnumber, SA_OVERWRITE, NULL); 
            }

            if (postol == negtol && postol > 0)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TOLERANCE, 1),
                  VT_DOUBLE,
                  &postol, SA_OVERWRITE,NULL); // x, y, rot, height
            }
            else
            {
               if (postol > 0)
               {
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_PLUSTOLERANCE, 1),
                     VT_DOUBLE,
                     &postol, SA_OVERWRITE,NULL); // x, y, rot, height
               }

               if (negtol > 0)
               {
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_MINUSTOLERANCE, 1),
                     VT_DOUBLE,
                     &negtol, SA_OVERWRITE,NULL); // x, y, rot, height
               }
            }

            if (strlen(value))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap(ATT_VALUE), value, SA_OVERWRITE,NULL); // 

            if (strlen(user1))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap("USER1"), user1,SA_APPEND, NULL);  

            if (strlen(user2))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap("USER2"), user2,SA_APPEND, NULL);  

            if (strlen(user3))
               doc->SetUnknownAttrib(&d->getAttributesRef(),get_attrmap("USER3"), user3,SA_APPEND, NULL);  

            d->getInsert()->setInsertType(insertTypePcbComponent);

            if (CREATE_LIBRARY)
            {
               TypeStruct *t = AddType(global_pcbfile, shapename);
               t->setBlockNumber( d->getInsert()->getBlockNumber());
               doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TYPELISTLINK, shapename, SA_OVERWRITE, NULL); //  
            }
         }
      }
   }

   return 1;
}

static void LoadPin(UNIDATToe& toe)
{
   char *lp;

	//PinNo|PinName|Xrel|Yrel|Pad top|PadRotation top|Material top|Pad bottom|PadRotation bottom|Material bottom|Drill|DrillRotation|DrillDiameter
	CString pinNum, pinName, toppad, botpad, drill;
	double x, y, toprot, botrot, drillrot, drilldiam = 0.0;

   // here once more with the delimiter
   expand_delimiter(ifp_line, delimiter_string);

   if ((lp = strtok(ifp_line, delimiter_string))   == NULL) return;   // toe number
	pinNum = lp;
	pinNum.Trim();

   if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // toe name
	pinName = lp;
	pinName.Trim();

   if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // x
	x = cnv_tok(lp);

   if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // y
	y = cnv_tok(lp);

   if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // pad top
	toppad = lp;
	toppad.Trim();
	//toppad.MakeUpper(); Case dts0100405632, Don't do MakeUpper here, it is not done consistently and is causing failure to find geometries

	if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // top rot
	toprot = atof(lp);

	if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // top material
	if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // pad bot
	botpad = lp;
	botpad.Trim();
	//botpad.MakeUpper(); Case dts0100405632, Don't do MakeUpper here, it is not done consistently and is causing failure to find geometries

	if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // bot rot
	botrot = atof(lp);

	if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // bot material
	if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // drill name
	drill = lp;
	drill.Trim();

	if ((lp = strtok(NULL, delimiter_string))   == NULL) return;   // drill rot
	drillrot = atof(lp);

	if ((lp = strtok(NULL, delimiter_string))   != NULL)             // drill diameter
		drilldiam = cnv_tok(lp);

   toe.SetNumber(pinNum);
   toe.SetName(pinName);
	toe.SetLocation(x, y);
	toe.SetTopPadName(toppad);
	toe.SetTopPadRotation_InDegs(toprot);
	toe.SetBottomPadName(botpad);
	toe.SetBottomPadRotation_InDegs(botrot);
	toe.SetDrillName(drill);
	toe.SetDrillRotation_InDegs(drillrot);
	toe.SetDrillDiameter(drilldiam);
}

int UNIDATComp::LoadPinList()
{
   char     *lp;
	PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (nextsect(lp) > -1)  break;

		if (lp[0] == '%')
		{
			PushTok = true;
			break;
		}

		UNIDATToe *toe = new UNIDATToe();
      LoadPin(*toe);
		m_pins.Add(toe);
	}

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_component_pin_v21()
{
   char     *lp;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (nextsect(lp) > -1)  break;

      if (global_pcbfile != NULL)
      {
         CString  compname, pinname, pinnr, netname;
         // here once more with the delimiter
         expand_delimiter(ifp_line, delimiter_string);

         if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;   // component name
         compname = lp;
         compname.Trim();

         // this is wrong in the description first pin_nr and than name
         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      //pinnr
         pinnr = lp;
         pinnr.Trim();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // pinname
         pinname = lp;
         pinname.Trim();

         if ((lp = strtok(NULL, delimiter_string)) == NULL) continue;      // netname
         netname = lp;
         netname.Trim();

         // Case dts0100405632
         // Sample case data has multiple comppins with same name but on different nets.
         // This will ot work in camcad whereever we search for a comp pin by name, which is a lot of places.
         // So keep the first instance only, if there are duplicates just skip them.
         // Also, only add the net if we are adding the comppin.
         NetStruct *net = NULL;
         if (FindCompPin(global_pcbfile, compname, pinname, &net) != NULL)
         {
            // Log error
            fprintf(ferr, "Dropping duplicate compin %s.%s on net %s, compin %s.%s already on net %s.\n",
               compname, pinname, netname,   compname, pinname, net->getNetName());
         }
         else
         {
            // A fresh one, okay to add 

            NetStruct   *n = NULL;
            if (netname.IsEmpty())
            {
               n = add_net(global_pcbfile, NET_UNUSED_PINS);
               n->setFlags(NETFLAG_UNUSEDNET);
            }
            else
            {
               n = add_net(global_pcbfile, netname);
            }

            // Case dts0100405632
            // We use pin name when building the comp, so must also use pin name here, NOT pin number
            CompPinStruct *p = add_comppin(global_pcbfile, n, compname, pinname);
         }
      }
   }

   return 1;
}

int UNIDATFootPrint::LoadToes()
{
   char     *lp;
   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;
      if (nextsect(lp) > -1)  break;

		if (!STRCMPI(lp,"FOOTPRINT"))
		{
			PushTok = true;
			break;
		}

      // Skip this keyword, toelist is only thing we expect to find.
      // Probably should be verifying that this is the only thing besides the toes themselves. 
      if (!STRCMPI(lp, "%TOELIST"))
         continue;

		UNIDATToe *toe = new UNIDATToe();
      LoadPin(*toe);
		m_toes.Add(toe);
	}

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_footprints_v21()
{
   char     *lp;

   PushTok = false;
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n")) == NULL) continue;
      if (nextsect(lp) > -1)  break;

      if (STRCMPI(lp,"FOOTPRINT"))  continue;

		CString  footprintName;
      if ((lp = strtok(NULL,"\n")) == NULL) continue;
		footprintName = lp;

		UNIDATFootPrint *ft = new UNIDATFootPrint(footprintName);
		ft->LoadToes();

		footprintmap.SetAt(footprintName, ft);
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_pad_v21()
{
   char     *lp;
   CString  padname;

   PushTok = false;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n"))   == NULL) continue;

      if (nextsect(lp) > -1)  break;

      if (!STRCMPI(lp,"PAD"))
      {
         if ((lp = strtok(NULL,"= \t\n")) == NULL) continue;

         padname.Format("PAD %s", lp); // can have the same name as other shapes.
			//padname.MakeUpper(); Case dts0100405632, Don't do MakeUpper here, it is not done consistently and is causing failure to find geometries
      }
      else if (!STRCMPI(lp,"%GRAFITEM"))
      {
         int laynr = unidat_create_layer("0","",1);
         do_grafitem_pads_v21(global_pcbfile, "0", 0, 0, padname, TRUE);
      }
      else
      {
         int r = 0;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_packages_v21()
{
   char *lp;
   BlockStruct *b = NULL;
   bool isPinList = false;

   PushTok = false;
   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"= \t\n")) == NULL) continue;
      if (nextsect(lp) > -1)  break;

      if (!STRCMPI(lp, "PACKAGE"))
      {
         isPinList = false;
         if (b != NULL)
            Graph_Block_Off();

         CString  packageName;
         if ((lp = strtok(NULL,"\n")) == NULL) continue;
         packageName = lp;

         if (global_pcbfile != NULL)
         {
            b = Graph_Block_On(GBO_APPEND, packageName, global_pcbfile->getFileNumber(), 0);
            b->setBlockType(BLOCKTYPE_PCBCOMPONENT);

            UNIDATPackage *pkg = new UNIDATPackage(packageName);
            packageMap.SetAt(packageName, pkg);

            pkg->SetGeometryNumber(b->getBlockNumber());
         }
      }
      else if (!STRCMPI(lp, "%PACKAGEOUTLINE"))
      {
         isPinList = false;
         do_grafitem(global_pcbfile, "PACKAGEOUTLINE", 0, GR_CLASS_COMPOUTLINE, "", FALSE);
      }
      else if (!STRCMPI(lp, "%PACKAGEDIMENSION"))
      {
         isPinList = false;
         do_grafitem(global_pcbfile, "PACKAGEDIMENSION", 0, graphicClassNormal, "", FALSE);
      }
      else if (!STRCMPI(lp,"%PINLIST"))
      {
         isPinList = true;
      }
      else if (isPinList)
      {
         UNIDATToe toe;
         LoadPin(toe);

         CString padstack  = make_padstack(global_pcbfile, toe.GetTopPadName(), toe.GetTopPadRotation_InDegs(), toe.GetBottomPadName(), toe.GetBottomPadRotation_InDegs());

         if (!padstack.IsEmpty() && global_pcbfile != NULL)
         {
            DataStruct *pin = Graph_Block_Reference(padstack, toe.GetName(), global_pcbfile->getFileNumber(), toe.GetPinLocation().x, toe.GetPinLocation().y,  0.0, 0, 1.0, -1, FALSE);
            pin->getInsert()->setInsertType(insertTypePin);
         }
      }
   }

   if (b != NULL)
      Graph_Block_Off();

   return 1;
}

/****************************************************************************/
/*
*/
int unidat_testneadles_v21()
{
   // What we have here are test probes. Parse and cache the
   // info. Ultimately we want to place probes and access markers, but we can't
   // do it on the fly here. Why? Because CompPins have not yet been processed.
   // We need those pesky compins in order to complete the chain of datalinks.
   // So cache the info and add the probes and access markers as a post process step.

	PushTok = false;

   char *lp;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"\n")) == NULL) continue;
      if (nextsect(lp) > -1)  break;

		// here once more with the delimiter
		expand_delimiter(ifp_line, delimiter_string);

		if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;
		CString probeRefname(lp);
		probeRefname.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString probeType(lp);
      probeType.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString targetType(lp);
      targetType.Trim();


		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString netName(lp);
      netName.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString xxx(lp);
      xxx.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString yyy(lp);
      yyy.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // Drill top
		CString unknown1(lp);
      unknown1.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // DrillRotation top
		CString unk2(lp);
      unk2.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // DrillDiameter top
		CString unk3(lp);
      unk3.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // Drill bottom
		CString unk4(lp);
      unk4.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // DrillRotation bottom
		CString unk5(lp);
      unk5.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // DrillDiameter bottom
		CString unk6(lp);
      unk6.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // TopSideAccess
		CString accessTop(lp);
      accessTop.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue; // BottomSideAccess
		CString accessBottom(lp);
      accessBottom.Trim();

      double x = cnv_tok(xxx.GetBuffer(0));
      double y = cnv_tok(yyy.GetBuffer(0));

      int filenum = 0;
      if (global_pcbfile != NULL)
         filenum = global_pcbfile->getFileNumber();

      UNIDATTestProbe *probe = new UNIDATTestProbe(filenum, probeRefname, probeType, targetType, netName, x, y, accessTop, accessBottom);
      probearray.Add(probe);
   }


	return 1;
}

static void PlaceProbe(CCEtoODBDoc *doc, FileStruct *file, DataStruct *accData, UNIDATTestProbe *uniprobe)
{
   // Pattern from: CProbePlacementSolution::PlaceProbesResultOnBoard

   // If accData is not NULL, put probe on that access point.
   // If accData is NULL, ad probe as unplaced.

   WORD dLinkKW = doc->IsKeyWord(ATT_DDLINK, 0);
   WORD testResKW = doc->RegisterKeyWord(ATT_TESTRESOURCE, 0, VT_STRING);
   WORD probePlacementKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
   WORD netNameKW = doc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKW = doc->RegisterKeyWord(ATT_REFNAME, 0, VT_STRING);

   int mirror = 0;
   double insertX = 0.0;
   double insertY = 0.0;
   char *placedOrUnPlaced = "Unplaced";

   if (accData != NULL)
   {
      mirror = accData->getInsert()->getMirrorFlags();
      insertX = accData->getInsert()->getOriginX();
      insertY = accData->getInsert()->getOriginY();
      placedOrUnPlaced = "Placed";
   }

   Graph_Block_On(file->getBlock());

   //POSITION probeTemplatePos = NULL;
   //CDFTProbeTemplate *pTemplate =  m_pTestPlan->GetProbes().Find_Probe(probeResult->GetProbeTemplateName(),
   //      probeTemplatePos, mirror?testSurfaceBottom:testSurfaceTop);
   //if (pTemplate == NULL && m_pTestPlan->GetTesterType() == DFT_TESTER_FIXTURE)
   //   continue;


   // Create probe geometry
#ifdef USETEMPLATE
   CString probeName = pTemplate->GetName() + ((mirror == 0)?"_Top":"_Bot");
   double diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
   double drillsize = pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
#else
   // Probe names are in imported data, don't necessarily have templates for them.
   CString probeName = uniprobe->m_probeType + ((mirror == 0)?"_Top":"_Bot");
   double diameter = doc->convertToPageUnits(pageUnitsInches, 0.090); // Std dia for 100 mil probe
   double drillsize = doc->convertToPageUnits(pageUnitsInches, 0.067); // Std drill for 100 mil probe
#endif

   BlockStruct* probeBlock = CreateTestProbeGeometry(doc, probeName, diameter, probeName, drillsize);


   // Insert the probe 
   CString probeNumber(uniprobe->m_refname);
   //probeNumber.Format("%d", probeResult->GetProbeNumber());
   DataStruct *probe = Graph_Block_Reference(probeBlock->getName(), probeNumber, -1, 
      insertX, insertY, 0.0, mirror, 1.0, -1, 0);
   probe->getInsert()->setInsertType(insertTypeTestProbe);

#ifdef MAYBELATER
   // Determine resource type
   CString testResourceType;
   switch (probeResult->GetResourceType())
   {
   case testResourceTypeTest:
      testResourceType = "Test";
      break;
   case testResourceTypePowerInjection:
      testResourceType = "Power Injection";
      {
         CString probeName;
         CDFTProbeableNet *pNet = m_pTestPlan->GetNetConditions().Find_ProbeableNet(probeResult->GetNetName(), probeTemplatePos);
         switch (pNet->GetNetType())
         {
         case probeableNetTypePower:
            probeName = (CString)"P" + probe->getInsert()->getRefname();
            probe->getInsert()->setRefname(strdup(probeName));
            break;
         case probeableNetTypeGround:
            probeName = (CString)"G" + probe->getInsert()->getRefname();
            probe->getInsert()->setRefname(strdup(probeName));
            break;
         }
      }
      break;
   }
#else
   CString testResourceType("Test");
#endif

   // Set attributes
   if (accData != NULL)
   {
      long datalink = accData->getEntityNumber();
      probe->setAttrib(doc->getCamCadData(), dLinkKW, valueTypeInteger, (void*)&datalink,  attributeUpdateOverwrite, NULL);
   }
   probe->setAttrib(doc->getCamCadData(), testResKW, valueTypeString, testResourceType.GetBuffer(0), attributeUpdateOverwrite, NULL);
   probe->setAttrib(doc->getCamCadData(), probePlacementKW, valueTypeString, placedOrUnPlaced, attributeUpdateOverwrite, NULL);
   probe->setAttrib(doc->getCamCadData(), netNameKW, valueTypeString, uniprobe->m_netName.GetBuffer(0), attributeUpdateOverwrite, NULL);

   CreateTestProbeRefnameAttr(doc, probe, probeNumber, drillsize);

   if (accData == NULL)
      probe->setHidden(true);

   Graph_Block_Off();
}

DataStruct *PlaceAccessMarker(FileStruct *pcbfile, UNIDATTestProbe *probe, int probedEntityNumber)
{
   if (pcbfile != NULL && probe != NULL)
   {
      ETestSurface testSurface = testSurfaceBottom; 
      if (probe->m_surfaceTop.CompareNoCase("N") != 0)
         testSurface = testSurfaceTop;

      CString refname;
      refname.Format("$$ACCESS_%s", probe->m_refname);
      // Use probe location, not compPin (etc) location. Probe is not necesssarily centered on target.
      DataStruct *accessPt = PlaceTestAccessPoint(doc, pcbfile->getBlock(), refname, probe->m_x, probe->m_y, 
         testSurface, probe->m_netName, "SMD",probedEntityNumber, 0, doc->getSettings().getPageUnits()); 

      return accessPt;

   }
   return NULL;
}

static DataStruct *findViaAt(FileStruct *pcbfile, CPoint2d location)
{
   if (pcbfile != NULL)
   {
      BlockStruct *pcbBlock = pcbfile->getBlock();

      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         if (data->isInsertType(insertTypeVia))
         {
            InsertStruct *via = data->getInsert();
            CPoint2d viaLocation = via->getOrigin2d();
            if (location.distance(viaLocation) < 0.005)
               return data;
         }
      }
   }

   return NULL;
}

static void unidat_add_testprobes(FileStruct *pcbfile)
{
   if (pcbfile != NULL)
   {
      for (int i = 0; i < probearray.GetCount(); i++)
      {
         UNIDATTestProbe *p = probearray.GetAt(i);

         if (pcbfile->getFileNumber() == p->m_pcbFileNum)
         {
            NetStruct *net = FindNet(pcbfile, p->m_netName);

            if (net != NULL)
            {
               double x = p->m_x;
               double y = p->m_y;

               if (p->m_targetType.CompareNoCase("VIA") == 0)
               {
                  // Place probe on via

                  CPoint2d location(x,y);
                  DataStruct *viaData = findViaAt(pcbfile, location);
                  if (viaData != NULL)
                  {
                     DataStruct *accessPt = PlaceAccessMarker(pcbfile, p, viaData->getEntityNumber());
                     PlaceProbe(doc, pcbfile, accessPt, p);
                     doc->SetUnknownAttrib(&viaData->getDefinedAttributes(), ATT_TEST, "", SA_OVERWRITE, NULL);
                  }
                  else
                  {
                     fprintf(ferr,"Unplaced Probe: Could not locate via target for \"%s\" at %.03f, %.03f.\n", p->m_refname, p->m_x, p->m_y);
                     PlaceProbe(doc, pcbfile, NULL, p);
                  }
               }
               else
               {
                  // Place probe on compPin

                  // Find the comp pin. The loop with increasing fuzz factor is to fake out
                  // finding the real pcb component with the real pin to get the real pad size.
                  // The probe is not necessarily centered on pin/pad, so landing anywhere on pad
                  // makes it the right compPin. If this cheap easy way ever fails, then change
                  // it to get the real pad extents from real pin of real pcb component, and
                  // make a new findCompPin() that will take an extent instead of fuzz.
                  CPoint2d location(x,y);
                  double fuzz = doc->convertToPageUnits(pageUnitsMilliMeters, 0.01);
                  CompPinStruct *cp = net->findCompPin(location, fuzz);
                  while (cp == NULL && fuzz < doc->convertToPageUnits(pageUnitsMilliMeters, 3.5))
                  {
                     fuzz *= 2.0;
                     cp = net->findCompPin(location, fuzz);
                  }

                  if (cp != NULL)
                  {
                     DataStruct *accessPt = PlaceAccessMarker(pcbfile, p, cp->getEntityNumber());
                     PlaceProbe(doc, pcbfile, accessPt, p);

#ifdef TEST_ATTR_ON_PCBCOMP_PIN_INSERT
                     SetPcbComponentPinTestAttr(cp);
#else
                     // TEST attr directly on compPin
                     doc->SetUnknownAttrib(&cp->getDefinedAttributes(), ATT_TEST, "", SA_OVERWRITE, NULL);

#endif
                  }
                  else
                  {
                     fprintf(ferr,"Unplaced Probe: Could not locate compPin target for \"%s\" at %.03f, %.03f.\n", p->m_refname, p->m_x, p->m_y);
                     PlaceProbe(doc, pcbfile, NULL, p);
                  }
               }
            }
            else
            {
               fprintf(ferr,"Dropped Probe: Could not locate net \"%s\" for \"%s\".\n", p->m_netName, p->m_refname, p->m_x, p->m_y);
            }
         }
      }
   }
}

/****************************************************************************/
/*
*/
static int load_unidatsettings(const CString fname, CGeometryConsolidatorControl &consolControl)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   ComponentSMDrule = 0;
	CompPinLocationRule = CP_RULE_RELATIVE;
	CompPinRotationRule = CP_RULE_RELATIVE;
   arc_unit = 1;
   CREATE_LIBRARY = TRUE;
   IGNORE_SHAPEOUTLINE = FALSE;
   consolControl.SetIsEnabled(true); // Reset to defaul=enabled

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp, "UNIDAT Settings", MB_OK | MB_ICONHAND);

      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL)
			continue;

      if (lp[0] == '.')
      {
         if (!STRCMPI(lp, ".Arc_Unit"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if (!STRNICMP(lp,"TENTH", 1))
               arc_unit = 0.1;
         }
         else if (!STRCMPI(lp, ".CreateLibrary"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if (toupper(lp[0]) == 'N')
               CREATE_LIBRARY = FALSE;
         }
         else if (!STRCMPI(lp, ".IGNORE_SAHPEOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            if (toupper(lp[0]) == 'Y')
               IGNORE_SHAPEOUTLINE = TRUE;
         }
         else if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

            ComponentSMDrule = atoi(lp);
         }
         else if (!STRCMPI(lp, ".ComponentPinCoordinates") // for backward compat.
				|| !STRCMPI(lp, ".ComponentPinLocations"))      // preferred.
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

				if (!STRCMPI(lp, "Relative"))
               CompPinLocationRule = CP_RULE_RELATIVE;
				else if (!STRCMPI(lp, "Absolute"))
					CompPinLocationRule = CP_RULE_ABSOLUTE;
         }
			else if (!STRCMPI(lp, ".ComponentPinRotations"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;

				if (!STRCMPI(lp, "Relative"))
               CompPinRotationRule = CP_RULE_RELATIVE;
				else if (!STRCMPI(lp, "Absolute"))
					CompPinRotationRule = CP_RULE_ABSOLUTE;
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            CString  attr;
            CString  ccattr;
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 

            attr = _strupr(lp);
            attr.TrimLeft();
            attr.TrimRight();

            UNIDATAttrmap  *c = new UNIDATAttrmap;
            attrmaparray.Add(c);  
            c->unidat_name = attr;
            c->cc_name = ccattr;
         }
         else if (!STRICMP(lp, ".CIRCLE_FORMAT"))
         {
            CString  attr;
            CString  ccattr;
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 

				if (!STRCMPI(lp, "r"))
               CircleDefRule = CD_RULE_RADIUS;
				else if (!STRCMPI(lp, "d"))
					CircleDefRule = CD_RULE_DIAMETER;
         }
         else if (consolControl.Parse(line))
         {
            // Nothing more to do, Parse() already did it if it was a Consolidator command.
         }
      }
   }

   fclose(fp);

   return 1;
}

CString CUnidatPCBLocation::GetPcbGeomName()
{
   // The raw pcb name in cad may look like a file path, or may be just the pcb name.
   // Here are three examples from real data samples:
   //  %%PANELSTRUCTURE
   //  %PANEL
   //  /usr/pcb/1268311632_v02/1268311632_v02|6.5|-24.5|90|
   //  /usr/pcb/1268311632_v02/1268311632_v02|64.3|-24.5|90|
   //
   //  %%PANELSTRUCTURE
   //  %PANEL
   //  ../3288415794_v06/3288415794_v06|18.2|-16.85|0|
   //  ../3288415794_v06/3288415794_v06|145.2|-16.85|0|
   //
   //  %%PANELSTRUCTURE
   //  %PANEL
   //  1268311632_v02|6.5|-24.5|90|
   //  1268311632_v02|64.3|-24.5|90

   // In all cases we want the leaf, the part that looks like a file name.
   // CFilePath is an easy way to get it.

   CFilePath pcbpath(this->m_pcbname, false /*do not beautify*/);
   CString pcbgeom(pcbpath.getFileName());

   return pcbpath.getFileName();
}

void CUnidatPCBLocation::MakePCBInsert(CCEtoODBDoc *doc)
{
   if (doc != NULL)
   {
      int panelfilenum = this->GetPanelFileNum();
      FileStruct *panelfile = doc->Find_File(panelfilenum);
     
      if (panelfile != NULL && panelfile->getBlock() != NULL)
      {
         BlockStruct *panelblock = panelfile->getBlock();

         CString pcbname = this->GetPcbGeomName();
         BlockStruct *pcbblk = doc->Find_Block_by_Name(pcbname, -1, blockTypePcb);
         if (pcbblk != NULL)
         {
            Graph_Block_On(panelblock);
            DataStruct *d = Graph_Block_Reference(pcbname, this->GetRefname(), pcbblk->getFileNumber(), this->GetX(), this->GetY(), DegToRad(this->GetRot()), 0, 1.0, -1, FALSE);
            d->getInsert()->setInsertType(insertTypePcb);
            Graph_Block_Off();
         }
      }
   }
}

void CUnidatPCBList::MakePCBInserts(CCEtoODBDoc *doc)
{
   for (int pcbindx = 0; pcbindx < this->GetCount(); pcbindx++)
   {
      CUnidatPCBLocation *pcb = this->GetAt(pcbindx);
      pcb->MakePCBInsert(doc);
   }
}

static int unidat_panel_pcb_inserts()
{
   // Generally the panel definition precedes tbe PCB definitions in the CAD file, therefore
   // the PCBs referenced have not yet been instantiated. Save the definitions and then
   // create the PCB inserts as a post-process.

   // %PANEL
   // 1038312209_v03|42.4962|-8.774|180|
   // 1038312210_v03|252.759|-228.45|0|

   PushTok = false;

   int pcbnum = 1; // for building refnames, refnames are not present in the data

   char *lp;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = strtok(ifp_line,"\n")) == NULL) continue;
      if (nextsect(lp) > -1)  break;

		// here once more with the delimiter
		expand_delimiter(ifp_line, delimiter_string);

		if ((lp = strtok(ifp_line, delimiter_string))   == NULL) continue;
		CString pcbgeomname(lp);
		pcbgeomname.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString xstr(lp);
		xstr.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString ystr(lp);
		ystr.Trim();

		if ((lp = strtok(NULL, delimiter_string))   == NULL) continue;
		CString rstr(lp);
		rstr.Trim();

      double x = cnv_tok(xstr.GetBuffer(0));
      double y = cnv_tok(ystr.GetBuffer(0));
      double r = atof(rstr);

      // note  that if we are here then global_pcbfile is actually the panel file
      int panelfilenum = 0;
      if (global_pcbfile != NULL)
         panelfilenum = global_pcbfile->getFileNumber();

      CString refname;
      refname.Format("Board_%d", pcbnum++);

      CUnidatPCBLocation *pcb = new CUnidatPCBLocation(panelfilenum, pcbgeomname, refname, x, y, r);
      PanelPcbList.Add(pcb);

   }

   return 1;
}

static int unidat_panel()
{
   // So far there isn't much of a difference between processing a panel and
   // processing a pcb. They can both have text, polylines, fiducials, etc.
   // The only real difference is a panel ought to have pcb inserts and a
   // pcb ought not. That, and the FileStruct's block type.
   //
   // And yes, a panel can have a %%COMPONENTS section, so let it go through
   // all the regular post processing for components. In the sample that
   // drove this implementation, the components seemed to be reject marks.

   PushTok = FALSE;

   char *lp;
   if ((lp = strtok(NULL, " \t\n")) != NULL)
   {
      /*
      if (global_pcbfile != NULL)
      {
         // This would be the place to do any cleanup, re-init, or otherwise prep for 
         // a new pcb file. So far, nothing has been identified as needing to be done.
      }
      */

      // Start a new file
      global_pcbfile = Graph_File_Start(lp, fileTypeUnidat);
      global_pcbfile->setBlockType(blockTypePanel);
      PcbFiles.Add(global_pcbfile); // yes, the panel gets tossed in with the PCBs here
   }

   return 1;
}

static int unidat_pcb()
{
   PushTok = FALSE;

   char *lp;
   if ((lp = strtok(NULL, " \t\n")) != NULL)
   {
      /*
      if (global_pcbfile != NULL)
      {
         // This would be the place to do any cleanup, re-init, or otherwise prep for 
         // a new pcb file. So far, nothing has been identified as needing to be done.
      }
      */

      // Start a new file
      global_pcbfile = Graph_File_Start(lp, fileTypeUnidat);
      global_pcbfile->setBlockType(blockTypePcb);
      PcbFiles.Add(global_pcbfile);
   }

   return 1;
}

UNIDATTestProbe::UNIDATTestProbe(int pcbfilenum, CString refname, CString probetype, CString targettype, CString netname, double x, double y, CString surfTop, CString surfBot)
: m_pcbFileNum(pcbfilenum)
, m_refname(refname)
, m_probeType(probetype)
, m_targetType(targettype)
, m_netName(netname)
, m_x(x)
, m_y(y)
, m_surfaceTop(surfTop)
, m_surfaceBot(surfBot)
{
}

UNIDATComp *CCompArray::Find(CString refname)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      UNIDATComp *c = this->GetAt(i);
      if (c->GetRefname().CompareNoCase(refname) == 0)
      {
         return c;
      }
   }

   return NULL;
}
