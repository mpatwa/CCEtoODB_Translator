// $Header: /CAMCAD/4.6/read_wrt/AllegIn.cpp 125   5/30/07 4:50p Lynn Phung $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.

    to do
      - text slant
      - text penwidth
                                                       
*/            

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"             
#include "geomlib.h"
#include "format_s.h"
#include "pcbutil.h"
#include <math.h>
#include <string.h>
#include <limits.h>
#include <direct.h>
#include "gauge.h"
#include "attrib.h"
#include "allegin.h"
#include "RwLib.h"
#include "CCEtoODB.h"
#include "dft.h"
#include "net_util.h"
#include "RwUiLib.h"
// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)

#if CamCadMajorMinorVersion > 406  //  > 4.6
#define ATTRIB_BASE(doc) ((doc)->getCamCadData())
#else
#define ATTRIB_BASE(doc)   (doc)
#endif

extern FileStruct *currentPCBFile; // from PORT_LIB.CPP
//extern void       ExplodeInsert(CCEtoODBDoc *doc, DataStruct *data, CDataList *dataList);

/* Static Variables **************************************************************/
  
static CommandArray commandArray;
static int commandArrayCount;

static RecordArray recordArray;
static int recordArrayCount;

static   ALLEGROAdef layer_attr[MAX_LAYERS];       /* Array of layers from allegro.in   */
static   int         layer_attr_cnt;

static   ALLEGROLayer *layers;
static   int         layerscnt;

static   ALLEGROAttr *attr;
static   int         attrcnt;

static   CPrimArray  primarray;
static   int         primarraycnt;

static   CompArray   comparray;
static   int         comparraycnt;

static   CPinEditGeomArray pinEditGeomArray;
static   int        pinEditGeomCnt;

static   ALLEGROStepRepeatArray  stepreparray;
static   int         stepreparraycnt;

static CAllegroMasterList masterList;

static   CMessageFilter *messageFilter = NULL;

static   int         display_error = 0;

static   FILE        *ifp;
static   char        *ifp_line;
static   char        *inl;          // pointer to ifp_line

static   CString     cur_layer_class, cur_layer_subclass;
static   int         cur_layer;
static   double      cur_width;

static   int         cur_entity;
static   int         cur_create = 0;
static   char        cur_block[80];
static   char        cur_placeblock[80];
static   ALLEGROExtent     cur_boardext;
static   CString     cur_filename;  // currently file that is reading.
static   CString     cur_refname;
static   CMapPtrToPtr tempMap;

#define  MAXPRIMOUT        10
#define  MAXTEXTCOMPATTR   10

static   ALLEGROPrimout    prim_refdes[MAXPRIMOUT];
static   int         prim_refdescnt;
static   ALLEGROPrimout    prim_compoutline[MAXPRIMOUT];
static   int         prim_compoutlinecnt;
static   ALLEGROPrimout    prim_boardoutline[MAXPRIMOUT];
static   int         prim_boardoutlinecnt;
static   ALLEGROPrimout    prim_paneloutline[MAXPRIMOUT];
static   int         prim_paneloutlinecnt;
static   ALLEGROPrimout    prim_compheight[MAXPRIMOUT];
static   int         prim_compheightcnt;

static   ALLEGROTextCompAttr  textcompattr[MAXTEXTCOMPATTR];
static   int         textcompattrcnt;

static   double      scale_factor = 1.0;

static   char        token[MAX_TOKEN];
static   long        line_cnt;
static   CString     tmpline;

static   CCEtoODBDoc  *globalDoc;

static   int         explode_format;
static   int         suppress_class;

static   FileStruct  *panelFile;
static   CString     panelName;

extern   char						*cadsystems[];
extern   CProgressDlg			*progress;
//extern   LayerTypeInfoStruct	layertypes[];

static   int         lastmaj;
static   int         lastlittlej;
static   char        oldcur_block[80];
static   int         panelmode;

static CStringArray padshapeNameArray;
static int padshapeNameArrayCount;

static   double      drill[MAX_PADSHAPE];
static   int         drillcnt;

static   ALLEGROPadshape      pd_circle[MAX_PADSHAPE];
static   ALLEGROPadshape      pd_rect[MAX_PADSHAPE];
static   ALLEGROPadshape      pd_square[MAX_PADSHAPE];
static   ALLEGROPadshape      pd_octagon[MAX_PADSHAPE];
static   ALLEGROPadshape      pd_obl[MAX_PADSHAPE];
static   int         pd_circlecnt, pd_rectcnt, pd_oblcnt, pd_squarecnt, pd_octagoncnt;
static   int         electricalstack = 0, physicalstack = 0;
static   int         PageUnits;

static   int         number_of_layers = 0;
static   int         usepinname, useinternalpaddef;
static	int			useHeaderBoardThickness;
static   int         drill_layer_done = 0;

static   DataStruct  *cur_reference;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static   ALLEGROAttrNameMapArray attrmaparray;
static   int         attrmapcnt;

static   CString     oldrefdes;           // speed up rte file read for netlist check.
static   int         old_widthindex;
static   int         cur_pcbcomponent;
static   CPnt        *lastvertex = NULL;
static   BOOL        hasPinEdited;
static   bool        exploded = false;

static   int         AllegroExtractType;

static int viaNumber = 1;

static CAllegroRefdesMap allegroRefdesMap;

static CMapStringToInt AllegroComplexApGeomMap;
static int  CmplxLastRecId = -1;
static int  CmplxLastSeqId = -1;
static CTMatrix CmplxHome;
static CPoly *CmplxPoly = NULL;
static int TotalFileCount = 0;


/* Function Prototypes ********************************************************/

static void init();
static void load_allegrosettings(CString fname);
static int do_master_geom(FileStruct *file);
static int execute_recordbuffer(int filetype);
static int do_layerfile();
static int do_padstackfile();
static int do_padshapefile();
static int do_symfile();
static int do_classic_symfile();
static int do_netfile();
static int do_classic_netfile();
static int do_fabmaster_layer();
static int do_fabmaster_vias();
static int do_fabmaster_geompins(int version);
static int do_fabmaster_traces();
static int do_fabmaster_geomgraphics();
static int do_fabmaster_netlist();
static int do_fabmaster_refdes();

static int do_graphic(const char *netname, double sym_x, double sym_y, double sym_rot, int sym_mirror, int majj, int minj,
                      int littlej, int graphic_class, int calc_ext, CAttributes** map);
static int do_attgraphic(CAttributes** map, double sym_x, double sym_y, double sym_rot, int sym_mirror, int majj, int minj,
                      int littlej, int graphic_class, int calc_ext, const char *layerName, int write_prosa, char *keyword);
static int do_line(double *x1,double *y1, double *x2, double *y2, int *widthindex, int *grtype);
static int do_rectangle(double *x1,double *y1, double *x2, double *y2, int *fill);
static int do_text(double *x,double *y, double *rot, int *mirror, double *h,double *charw, double *penw, char *prosa);
static int do_arc(double *x1,double *y1, double *x2, double *y2, double *xc, double *yc, double *rad, int *widthindex,
                  int *direction, int *grtype);

static int master_block(const CString geomName, const CString refName);
static bool InMasterList(const CString geometryName, const CString refname);
static void setMasterPinedited(const CString geometryName, const CString refname);
static void load_commandbuffer();
static void load_recordbuffer();
static int label_commandbuffer();
static int label_recordbuffer();
static int check_filetype(int extract, int tot_filecnt);
static int assign_padlayertypes(int c_l);

static void update_boardext(double x, double y);
static int update_padshape(CString padshapeName);
static int update_attr(const char *lp,int typ);
static int update_comparray(const char *refdes, const char *sname, int pin_edited, double x, double y, double rot, int mir);
static void UpdatePinEditGeomArray(CString originalGeomName, BlockStruct *pinEditBlock);
static int GetPinEditGeomIndex(CString originalGeomName);
//static int get_layertype(CString layerName);
static int get_lay_index(CString layerName);
static int get_label(int label);
static int get_attrib(const char *s);
static const char* get_attrmap(const char *a);
static int is_attrib(const char *s, int atttype);
static int get_drillindex(double size, bool plated, int layernum);
static int make_pshapename(char *pshapename, ApertureShapeTag shapeTag, double w, double h);
static int get_graphic_class(CString classString, BOOL NetnameIsEmpty);
static int get_graphic_subclass(CString subclassString);
static int is_primboardoutline(const char *lay, const char *class_name);
static int is_primpaneloutline(const char *lay, const char *class_name);
static int is_primcompoutline(const char *lay, const char *class_name);
static int is_primarray(CString classString, CString subClassString, int typ);
static int is_primcompheight(const char *lay, const char *class_name);
static int is_primrefdes(const char *lay, const char *class_name);
static int is_textcompattr(const char *lay, const char *class_name, char *attr);

static int update_layertypes();
static int update_layer(const char *l);
static void set_pcb();
static void set_panel();

static int read_line(FILE *fp, char *line, int len);
static int get_token();
static void p_error();
static void lab_error(const char *l);
static int do_attrib(CAttributes** map,const char *keyw, int typ, const char *val0, double val1, int val2);
static void explode_geometries();
static void setToFiducialOrTool();

static double getToPageUnitsScaleFactor(CString units);
static bool ReadAllegro(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits, int cur_filecnt, int tot_filecnt, int subFileIndex, bool hasNextSubFile);

static CStringArray filePaths;
static int startFileIndex;

/******************************************************************************
* ReadAllegro
*/
void ReadAllegro(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits, int cur_filecnt, int tot_filecnt)
{
   filePaths.Add(path_buffer);

   if (cur_filecnt == (tot_filecnt - 1))
   {
      bool hasNextSubFileFlag = true;

      for (int subFileIndex = 0;hasNextSubFileFlag;subFileIndex++)
      {
         for (int fileIndex = 0;fileIndex < filePaths.GetCount();fileIndex++)
         {
            CString filePath = filePaths.GetAt(fileIndex);

            if (!ReadAllegro(filePath,Doc,Format,pageunits,fileIndex,tot_filecnt,subFileIndex, hasNextSubFileFlag))
            {
               hasNextSubFileFlag = false;
            }
         }
      }

      filePaths.RemoveAll();
   }
}

bool ReadAllegro(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits, int cur_filecnt, int tot_filecnt, int subFileIndex, bool hasNextSubFile)
{
   bool retval = false;

   globalDoc = Doc;
   PageUnits = pageunits;
   TotalFileCount = tot_filecnt;

   explode_format = Format->ExplodeFormat;
   suppress_class = Format->SuppressClass;
   AllegroExtractType = Format->Allegro_Format;
   bool fabmasterExtractFlag = (AllegroExtractType == ALLEGRO_FABMASTER);

   AllegroComplexApGeomMap.RemoveAll();
   CmplxLastRecId = -1;
   CmplxLastSeqId = -1;

   bool firstSubFile = (cur_filecnt == 0 && subFileIndex == 0);

 	if (firstSubFile)   // warn only on the first one
   {
      switch (AllegroExtractType)
      {
      case ALLEGRO_FABMASTER:
         {
            CString err;
            err = "Using FABMASTER extract will limit the interpretation of the data!\n\n";
            err +="1.Fabmaster Extract does not contain LayerStack information!\n";
            err +=" -> This can be corrected in CAMCAD.\n\n";
            err +="It is recommended to use the CAMCAD EXTRACT Script!";
            ErrorMessage(err, "Extract Limitations");
         }
         break;
      case ALLEGRO_LAYDBASE:
            ErrorMessage("Using CLASSIC extract will limit the interpretation of the data!\n\nIt is recommended to use the CAMCAD EXTRACT Script!", "Extract Limitations");
         break;
      }
   }

	// need to do this, because panel only knows the path.ext
   char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
   _splitpath(path_buffer, drive, dir, fname, ext );
   CString fileName = fname;
   fileName += ext;

   // scale_factor - all units are normalized to INCHES
   scale_factor = Units_Factor(UNIT_INCHES, PageUnits);

   CString logFile = GetLogfilePath("allegro.log");
   // if this is not the first file, append to file
   if (!firstSubFile)
   {
      // add blank line in log file between file processing message sections
      FILE *fp;
      if ((fp = fopen(logFile, "a+t")) != NULL)
      {
         fprintf(fp, "\n");
         fclose(fp);
      }
      messageFilter = new CMessageFilter(messageFilterTypeMessage, logFile, true /*append*/, messageFilterFileFormatPlain);
   }
   else
   {
      messageFilter = new CMessageFilter(messageFilterTypeMessage, logFile, false, messageFilterFileFormatPlain);
      display_error = 0;
   }

   messageFilter->formatMessage("File : [%s] subfile %d", path_buffer,subFileIndex);
   
   init();

   CString settingsFile = Format->filename;
   load_allegrosettings(settingsFile);

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(path_buffer, "rt")) == NULL)
   {
      ErrorMessage(path_buffer, "Error opening file");
      return retval;
   }
   cur_filename = path_buffer;

   if (subFileIndex == 0 && cur_filecnt == 0)
   {
      // There will only be one panel.
      // It will be created in set_panel() as it is needed.
      panelFile = NULL;
      panelName.Empty();
      startFileIndex = globalDoc->getFileList().GetCount();
   }

   if (currentPCBFile == NULL || 
       currentPCBFile->getBlockType() != blockTypePcb || 
       currentPCBFile->getCadSourceFormat() != Type_Allegro_Layout ||
		(/*subFileIndex > 0 && */cur_filecnt == 0))
   {
      // Create the board here if it is not created
      CString boardname;
      boardname.Format("Board_%d_%d",startFileIndex,subFileIndex);
      currentPCBFile = Graph_File_Start(boardname, Type_Allegro_Layout);
      currentPCBFile->setBlockType(blockTypePcb);
      currentPCBFile->getBlock()->setBlockType(currentPCBFile->getBlockType());

      if (panelName.IsEmpty())
      {
         panelName.Format("Panel_%d",startFileIndex);
      }
   }

   set_pcb();
   
   line_cnt = 0;
   lastlittlej = 0;
   int headerCount = 0;

   int valout_filetype = UNKNOWN_EXTRACT;

   CFileReadProgress progress(ifp);

   int res = 0;
   while (res > -2 && read_line(ifp, ifp_line, MAX_LINE))
   {
      if ((line_cnt % 200) == 0)
      {
         progress.updateProgress();
      }

      char  t[MAX_TOKEN];

      tmpline = ifp_line;

      if (!get_token())
         p_error();

      strcpy(t, token);

      if (strlen(token) != 1)
         continue;

      if (token[0] == 'A')
      {
         headerCount++;
      }

      int headerIndex = headerCount - 1;

      if (!fabmasterExtractFlag)
      {
         if (headerIndex < subFileIndex)
         {
            continue;
         }
         else if (headerIndex > subFileIndex && hasNextSubFile)
         {
            retval = true;
            break;
         }
      }
      else
      {
         retval = false;
      }

      switch(token[0])
      {
      case 'A':
         {
            // load command buffer
            for (int i=0; i<commandArrayCount; i++)
               delete commandArray[i];
            commandArrayCount = 0;

            load_commandbuffer();
            label_commandbuffer();

            // here check if this a
            // CAMCAD Valout extract
            // Fabmaster extract
            if ((valout_filetype = check_filetype(valout_filetype, tot_filecnt )) < -1)
            {
               res = -3;
               break;
            }
         }
         break;
      case 'J':
         {
            CString u;
            // here get units
            recordArrayCount = 0;
            load_recordbuffer();
         
            if (recordArrayCount < 11)
            {
               CString tmp;
               tmp.Format("Error: Invalid Header ('J' Record) Format found in [%s] -> can not continue!", path_buffer);
               ErrorMessage(tmp, "Allegro Read", MB_OK | MB_ICONHAND);
               res = -3;
               break;   
            }
            else
            {
               // units is the 8th field
               u = recordArray[7]->tok;
               if (u.CompareNoCase("MILS") == 0)
                  scale_factor = Units_Factor(UNIT_MILS, PageUnits);
               else if (u.CompareNoCase("INCHES") == 0)
                  scale_factor = Units_Factor(UNIT_INCHES, PageUnits);
               else if (u.CompareNoCase("MILLIMETERS") == 0)
                  scale_factor = Units_Factor(UNIT_MM, PageUnits);
               else if (u.CompareNoCase("MICRONS") == 0) // 1/1000 of a MM
                  scale_factor = Units_Factor(UNIT_MM, PageUnits)/1000;
               else
               {
                  CString tmp;
                  tmp.Format("Unknown File Units [%s] found -> set to MILS", u);
                  ErrorMessage(tmp,"Allegro Read", MB_OK | MB_ICONHAND);
                  scale_factor = Units_Factor(UNIT_MILS, PageUnits);
               }

					if (useHeaderBoardThickness)
					{        
						// board thickness, 10th field
						CString thicknessSpec = recordArray[9]->tok;
						int thkPos = 0;
						CString thicknessStr = thicknessSpec.Tokenize(" ", thkPos);
						CString thicknessUnits = thicknessSpec.Tokenize(" ", thkPos);
						double thkScaleFactor = getToPageUnitsScaleFactor(thicknessUnits);
						
						double boardThickness = 0.0;
						if (!thicknessStr.IsEmpty())
						{
							boardThickness = atof(thicknessStr);
							boardThickness *= thkScaleFactor;
						}

						globalDoc->SetAttrib(&currentPCBFile->getBlock()->getAttributesRef(), globalDoc->IsKeyWord(BOARD_THICKNESS, 0),
							valueTypeUnitDouble, &boardThickness, attributeUpdateOverwrite, NULL);
					}

               // number of electrical layers, 11th field
               number_of_layers = atoi(recordArray[10]->tok);
               globalDoc->SetAttrib(&currentPCBFile->getBlock()->getAttributesRef(), globalDoc->IsKeyWord(BOARD_NUMBER_OF_LAYERS, 0),
                     valueTypeInteger, &number_of_layers, attributeUpdateOverwrite, NULL);
            }
            // free men
            for (int i=0; i<recordArrayCount; i++)
               delete recordArray[i];

            recordArrayCount = 0;
         }
         break;
      case 'S':
         {
            int write_to_panel = FALSE;

            recordArrayCount = 0;
            load_recordbuffer();

            if (recordArrayCount < commandArrayCount)
            {
               CString tmp;
               tmp.Format("Corrupt file : Commands [%d] != Records [%d] in file [%s] at line %ld",
                     commandArrayCount,recordArrayCount, cur_filename, line_cnt);
               ErrorMessage(tmp, "Allegro Read", MB_OK | MB_ICONHAND);
               res = -3;
               break;   
            }

            // label command records
            if (label_recordbuffer() == 0)
            {
               for (int i=0; i<recordArrayCount; i++)
                  delete recordArray[i];
               recordArrayCount = 0;
               break;
            }

				int i=0;
            for (i=0; i<commandArrayCount; i++)
            {
               Command *command = commandArray[i];
               if (is_attrib(command->tok, PANEL_ATT) && strlen(recordArray[i]->tok))
               {
                  write_to_panel = TRUE;
                  break;
               }
            }

            // do it
            if (write_to_panel)
            {
               set_panel();
            }

            cur_layer_class = "";
            cur_layer_subclass = "";

            // this is the CAMCAD Valout.txt
            if ((res=execute_recordbuffer(valout_filetype)) < 0)
            {
               // bad file format.
               // free men
               for (int i=0; i<recordArrayCount; i++)
                  delete recordArray[i];
               recordArrayCount = 0;
               break;            
            }

            set_pcb();  // always
            
            // free men
            for (i=0; i<recordArrayCount; i++)
               delete recordArray[i];
            recordArrayCount = 0;
         }
         break;
      } // switch
   }
 
   fclose(ifp);
   
   update_layertypes();

   //if (AllegroExtractType == ALLEGRO_FABMASTER)
   //{
   //   generate_PADSTACKACCESSFLAG(globalDoc, 1);
   //   generate_PINLOC(globalDoc, currentPCBFile, 1);  // this function generates the PINLOC argument for all pins.
   //} Must always do this after the pin_edited checks, regardless of format

   if (valout_filetype == CAMCAD_EXTRACT_RTE)
   {
      if (!hasPinEdited)
      {
         CString msg = "This Allegro design was generated using the old Extract Script \"valout.txt\".";
         msg += "This old script doesn't support the Allegro \"PIN_EDITED\" option, so it will assumed that there are no pin_edits.";
         messageFilter->formatMessage(msg);
         display_error++;
      }

      do_master_geom(currentPCBFile);
      globalDoc->purgeUnusedBlocks();

#if CamCadMajorMinorVersion > 406  //  > 4.6
      currentPCBFile->getTypeList().removeUnusedDeviceTypes();
#else
      // loop through devices with no geometry and remove them
      POSITION pos = currentPCBFile->getTypeList().GetHeadPosition();
      while (pos)
      {
         POSITION tmpPos = pos;
         TypeStruct *type = currentPCBFile->getTypeList().GetNext(pos);
         if (type)
         {
            if(type->getBlockNumber() == -1)
               currentPCBFile->getTypeList().RemoveAt(tmpPos);
         }
      }
#endif
   }

   generate_PADSTACKACCESSFLAG(globalDoc, 1); // This function generates pin surface access flag
   generate_PINLOC(globalDoc, currentPCBFile, 1);  // this function generates the PINLOC argument for all pins.

   update_smdpads(globalDoc);
   RefreshInheritedAttributes(globalDoc, attributeUpdateOverwrite);  // smd pins are after the pin instance is done.

   update_smdrule_geometries(globalDoc, ComponentSMDrule);
   update_smdrule_components(globalDoc, currentPCBFile, ComponentSMDrule);

   if (physicalstack)
   {
      // I do not exactly how to calculate the zheigth, this is why I do it like ANSOFT
      // here calculate heights according to ANSOFT
      // Layer  thickness   zheight
      // bottom   0.72        0
      // dielec   6.0         0
      // l7       1.44        6
      // dielec   6.0         6
      // l6       1.44        12
      // dielec   6.0         12
      // l5       1.44        18
      // dielec   6.0         18
      // l4       1.44        24
      // dielec   6.0         24
      // l3       1.44        30
      // dielec   6.0         30
      // l2       1.44        36
      // dielec   6.0         36
      // top      1.44        42
      int i;
      double zheight[MAX_LAYERS];
      int low_physical = layers[0].physicalstack, hi_physical = layers[0].physicalstack;

      for (i=0; i<MAX_LAYERS; i++)
         zheight[i] = 0.0;

      // calculate lowest and highest stacking layer
      for (i=0; i<layerscnt; i++)
      {
         if (layers[i].physicalstack == 0)
            continue;
         if (layers[i].physicalstack < low_physical)
            low_physical = layers[i].physicalstack;
         if (layers[i].physicalstack > hi_physical)
            hi_physical = layers[i].physicalstack;
      }

      for (i=0; i<layerscnt; i++)
      {
         if (layers[i].physicalstack == 0)
            continue;
         if (layers[i].typ != 2)
            continue;   // only dielectric
         
         // reverse layers
         zheight[hi_physical - layers[i].physicalstack] = layers[i].thickness;
      }

      // add up heights, do not start with the first and second, 
      // which is 0 and the next dialect is 1
      for (i=1;i<hi_physical-1;i++)
         zheight[i+1] += zheight[i];

      // up one layer.
      for (i=0;i<hi_physical-1;i++)
         zheight[hi_physical-i] = zheight[hi_physical-i-1];

      // set dielectric closest to bottom also to 0
      zheight[1] = 0;

      double thickness = 0.0;

      for (i=0; i<layerscnt; i++)
      {
         if (layers[i].physicalstack == 0)
            continue;

         int w = Graph_Level(layers[i].name, "", 0);
         LayerStruct *l = globalDoc->FindLayer(w);
			if (l!= NULL)
			{
				l->setZHeight( zheight[hi_physical-layers[i].physicalstack]);
				if (l->getZHeight() > thickness)
					thickness = l->getZHeight();
			}
      }

		if (!useHeaderBoardThickness)
		{
			// If not useHeaderThickness then use sum of layer thicknesses
			globalDoc->SetAttrib(&currentPCBFile->getBlock()->getAttributesRef(), globalDoc->IsKeyWord(BOARD_THICKNESS, 0),
				valueTypeUnitDouble, &thickness, attributeUpdateOverwrite, NULL);
		}

   }

	int i=0;
   for (i=0; i<layerscnt; i++)
      free(layers[i].name);
   free(layers);
   free(ifp_line);

   for (i=0; i<commandArrayCount; i++)
      delete commandArray[i];
   commandArray.RemoveAll();

   // indexes are already deleted after a record is processed.
   recordArray.RemoveAll();

	masterList.empty();

   for (i=0; i<primarraycnt; i++)
      delete primarray[i];

   int w = stepreparraycnt;
   // here calc correct panel offsets
   if (stepreparraycnt > 0)
   {
      double offx = cur_boardext.maxx;
      double offy = cur_boardext.maxy;
      double rot = 0;
      int master_steprep = -1;   

      set_panel();

      // This section of code is commented out to fix case dts0100412932 because there is no
      // documentation indicating that it is necessary to offset the board insert.  

      //if (cur_boardext.extent_found == FALSE)
      //{
      //   // no board extent found
      //}
      //else
      //{
      //   // find stepreparray, which is closest, but inside the lowerleft board outline
      //   int w = stepreparraycnt;

      //   for (int i=0; i<stepreparraycnt; i++)
      //   {
      //      double tx = stepreparray[i]->x;
      //      double ty = stepreparray[i]->y; // just for debug
      //      // this is inside
      //      if (tx >= cur_boardext.minx && ty >= cur_boardext.miny)
      //      {
      //         // now this is closer
      //         if ((tx - cur_boardext.minx) < offx || (ty - cur_boardext.miny) < offy)
      //         {
      //            offx = tx - cur_boardext.minx;
      //            offy = ty - cur_boardext.miny;
      //            master_steprep = i;
      //         }
      //      }
      //   }
      //}

      if (master_steprep > -1)
      {
         offx = stepreparray[master_steprep]->x;
         offy = stepreparray[master_steprep]->y;
         rot  = stepreparray[master_steprep]->rot;
      }
      else
      {
         offx = offy = 0.0;   // none found
         rot = 0;
      }

      int w = stepreparraycnt;
      for (int i=0; i<stepreparraycnt; i++)
      {
			//ALLEGROStepRepeat *s = stepreparray[i];

         double ox;
         double oy;
         double sx = stepreparray[i]->x;
         double sy = stepreparray[i]->y;
         double r = stepreparray[i]->rot - rot;

         CString stepname;
         stepname.Format("BOARD_%d", i+1);

			if (stepreparray[i]->mir)
			{
				sx = -sx;
				Rotate(offx, offy, -r, &ox, &oy);
			}
			else
			{
				Rotate(offx, offy, r, &ox, &oy);
			}

         double pcbX, pcbY;
			pcbX = sx - ox;
			pcbY = sy - oy;
			if (stepreparray[i]->mir)
				pcbX = -pcbX;

         DataStruct *data = Graph_Block_Reference(currentPCBFile->getName(), stepname, 0, pcbX, pcbY, DegToRad(r), stepreparray[i]->mir , 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypePcb);

         delete stepreparray[i];
      }
      set_pcb();
   }

   for (i=0; i<attrmapcnt; i++)
      delete attrmaparray[i];  
   attrmaparray.RemoveAll();
   attrmapcnt = 0;

   for (i=0; i<comparraycnt; i++)
      delete comparray[i];
   comparraycnt = 0;
   comparray.RemoveAll();

   for (i=0; i<pinEditGeomCnt; i++)
   {
      ALLEGROPinEditGeom *a = pinEditGeomArray[i];
      a->pinEditedGeomMap.RemoveAll();
      delete a;
   }
   pinEditGeomArray.RemoveAll();
   pinEditGeomCnt = 0;

	// go through each board to to make sure we link up the probes with the test access points and the features they probe
	ProcessLoadedProbes(globalDoc);

   messageFilter->formatMessage("End : [%s]", path_buffer);
   delete messageFilter;

   // only display if the file is the last in the list
   if (display_error && cur_filecnt == (tot_filecnt-1))
      Logreader(logFile);

   if (panelFile)
      panelFile->setShow(false);   // do not show the panel file.

	//explode some geometries based on the the .in command
	if (cur_filecnt == (tot_filecnt-1))
	{
		explode_geometries();
		setToFiducialOrTool();
		allegroRefdesMap.Empty();
      AllegroComplexApGeomMap.RemoveAll();
	}

	//setToFiducialOrTool();

   for (i=0; i<attrcnt; i++)
      free(attr[i].name);
   free(attr);
	
   return retval;
}

static double getToPageUnitsScaleFactor(CString units)
{
	double scaleFactor = 1.0;

	units.MakeUpper();

	if (units.Find("MILLIMETER") > -1)				// millimieter, millimeters
		scaleFactor = Units_Factor(UNIT_MM, PageUnits);
	else if (units.Find("MICRON") > -1)				// micron, microns
		scaleFactor = Units_Factor(UNIT_MM, PageUnits)/1000;  // 1/1000 of a MM
	else if (units.Find("MIL") > -1)				// mil, mils
		scaleFactor = Units_Factor(UNIT_MILS, PageUnits);
	else if (units.Find("INCH") > -1)	// inch, inches
		scaleFactor = Units_Factor(UNIT_INCHES, PageUnits);
	else if (units.IsEmpty())
		scaleFactor = scale_factor; // default to overall units specified in header
	else
	{
		messageFilter->formatMessage("Units \"%s\" in header not recognized. Using mils.", units);
		scaleFactor = Units_Factor(UNIT_MILS, PageUnits);
	}

	return scaleFactor;
}

/******************************************************************************
* init
*/
static void init()
{
   pd_circlecnt = 0;
   pd_rectcnt   = 0;
   pd_oblcnt    = 0;
   pd_squarecnt = 0;
   pd_octagoncnt = 0;
   drillcnt     = 0;
   electricalstack = 0;
   physicalstack = 0;

   drill_layer_done = FALSE;
   hasPinEdited = FALSE;

   cur_width = 0.0;
   strcpy(cur_placeblock,"");
   strcpy(cur_block,"");
   strcpy(oldcur_block,"");
   lastmaj = -1;
   cur_reference = NULL;

   commandArray.SetSize(100,100);
   commandArrayCount = 0;

   recordArray.SetSize(100,100);
   recordArrayCount = 0;

   attrmaparray.SetSize(100,100);
   attrmapcnt = 0;

   masterList.empty();

   if ((attr = (ALLEGROAttr  *)calloc(MAX_ATTR,sizeof(ALLEGROAttr))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   attrcnt = 0;

   if ((layers = (ALLEGROLayer*)calloc(MAX_LAYERS,sizeof(ALLEGROLayer))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   layerscnt = 0;

   if ((ifp_line = (char  *)calloc(MAX_LINE,sizeof(char))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   primarray.SetSize(10,10);
   primarraycnt = 0;    // need

   comparray.SetSize(10,10);
   comparraycnt = 0;    // need

   pinEditGeomArray.SetSize(10, 10);
   pinEditGeomCnt = 0;

   stepreparray.SetSize(10,10);
   stepreparraycnt = 0;    // need

   padshapeNameArrayCount = 0;
   oldrefdes = "";
   old_widthindex = -1;

	viaNumber = 1; // for making via Refnames

}

/******************************************************************************
* load_allegrosettings
*/
static void load_allegrosettings(CString fname)
{
   FILE *fp;
   char line[255];
   char *lp;
   long linecnt = 0;

   prim_compoutline[0].Class  = "";
   prim_compoutlinecnt = 0;

   prim_boardoutline[0].Class = "";
   prim_boardoutlinecnt = 0;

   prim_paneloutline[0].Class = "";
   prim_paneloutlinecnt = 0;

   prim_compheight[0].Class = "";
   prim_compheightcnt = 0;

   textcompattrcnt = 0;

   prim_refdes[0].Class = "";
   prim_refdescnt = 0;

   layer_attr_cnt = 0;

   usepinname = FALSE;
   useinternalpaddef = FALSE;
	useHeaderBoardThickness = FALSE; // Case 1606, new feature defaults to old behavior, not new behavior
   cur_boardext.extent_found = FALSE;
   ComponentSMDrule = 0;

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp,"Allegro Settings", MB_OK | MB_ICONHAND);
      return;
   }
              
   while (fgets(line, 255, fp))
   {
      linecnt++;
      CString w = line;
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            ComponentSMDrule = atoi(lp);
         }
         else if (!STRICMP(lp, ".COMPOUTLINE"))
         {
            if (prim_compoutlinecnt < MAXPRIMOUT)
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_compoutline[prim_compoutlinecnt].Class = lp;
               prim_compoutline[prim_compoutlinecnt].Class.TrimLeft();
               prim_compoutline[prim_compoutlinecnt].Class.TrimRight();
   
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_compoutline[prim_compoutlinecnt].SubClass = lp;
               prim_compoutline[prim_compoutlinecnt].SubClass.TrimLeft();
               prim_compoutline[prim_compoutlinecnt].SubClass.TrimRight();
               prim_compoutlinecnt++;
            }
            else
            {
               messageFilter->formatMessage("Too many .COMPOUTLINE statements in %s", fname);
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".refdes"))
         {
            if (prim_refdescnt < MAXPRIMOUT)
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_refdes[prim_refdescnt].Class = lp;
               prim_refdes[prim_refdescnt].Class.TrimLeft();
               prim_refdes[prim_refdescnt].Class.TrimRight();
   
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_refdes[prim_refdescnt].SubClass = lp;
               prim_refdes[prim_refdescnt].SubClass.TrimLeft();
               prim_refdes[prim_refdescnt].SubClass.TrimRight();
               prim_refdescnt++;
            }
            else
            {
               messageFilter->formatMessage("Too many .REFDES statements in %s", fname);
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".textcompattr"))
         {
            if (textcompattrcnt < MAXTEXTCOMPATTR)
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               textcompattr[textcompattrcnt].Attribute = lp;
               textcompattr[textcompattrcnt].Attribute.TrimLeft();
               textcompattr[textcompattrcnt].Attribute.TrimRight();

               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               textcompattr[textcompattrcnt].Class = lp;
               textcompattr[textcompattrcnt].Class.TrimLeft();
               textcompattr[textcompattrcnt].Class.TrimRight();
   
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               textcompattr[textcompattrcnt].SubClass = lp;
               textcompattr[textcompattrcnt].SubClass.TrimLeft();
               textcompattr[textcompattrcnt].SubClass.TrimRight();
               textcompattrcnt++;
            }
            else
            {
               messageFilter->formatMessage("Too many .TEXTCOMPATTR statements in %s", fname);
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".COMPHEIGHT"))
         {
            if (prim_compheightcnt < MAXPRIMOUT)
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_compheight[prim_compheightcnt].Class = lp;
               prim_compheight[prim_compheightcnt].Class.TrimLeft();
               prim_compheight[prim_compheightcnt].Class.TrimRight();
   
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_compheight[prim_compheightcnt].SubClass = lp;
               prim_compheight[prim_compheightcnt].SubClass.TrimLeft();
               prim_compheight[prim_compheightcnt].SubClass.TrimRight();
               prim_compheightcnt++;
            }
            else
            {
               messageFilter->formatMessage("Too many .COMPHEIGHT statements in %s", fname);
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".BOARDOUTLINE"))
         {
            if (prim_boardoutlinecnt < MAXPRIMOUT)
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_boardoutline[prim_boardoutlinecnt].Class = lp;
               prim_boardoutline[prim_boardoutlinecnt].Class.TrimLeft();
               prim_boardoutline[prim_boardoutlinecnt].Class.TrimRight();
   
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_boardoutline[prim_boardoutlinecnt].SubClass = lp;
               prim_boardoutline[prim_boardoutlinecnt].SubClass.TrimLeft();
               prim_boardoutline[prim_boardoutlinecnt].SubClass.TrimRight();
               prim_boardoutlinecnt++;
            }
            else
            {
               messageFilter->formatMessage("Too many .BOARDOUTLINE statements in %s", fname);
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".PANELGRAPHIC"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            ALLEGROPrimout *p = new ALLEGROPrimout; 
            primarray.SetAtGrow(primarraycnt, p);
            p->Class = lp;
            p->Class.TrimLeft();
            p->Class.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            p->SubClass = lp;          
            p->SubClass.TrimLeft();
            p->SubClass.TrimRight();
            p->typ = PANEL_GRAPHIC;
            primarraycnt++;
         }
         else if (!STRICMP(lp, ".PANELOUTLINE"))
         {
            if (prim_paneloutlinecnt < MAXPRIMOUT)
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_paneloutline[prim_paneloutlinecnt].Class = lp;
               prim_paneloutline[prim_paneloutlinecnt].Class.TrimLeft();
               prim_paneloutline[prim_paneloutlinecnt].Class.TrimRight();
   
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue; 
               prim_paneloutline[prim_paneloutlinecnt].SubClass = lp;
               prim_paneloutline[prim_paneloutlinecnt].SubClass.TrimLeft();
               prim_paneloutline[prim_paneloutlinecnt].SubClass.TrimRight();
               prim_paneloutlinecnt++;
            }
            else
            {
               messageFilter->formatMessage("Too many .PANELOUTLINE statements in %s", fname);
               display_error++;
            }
         }
         else if (!STRICMP(lp, ".BOARDSYMBOL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, BOARD_SYMBOL);
         }
         else if (!STRICMP(lp, ".MECHANICALSYMBOL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, MECHANICAL_SYMBOL);
         }
         else if (!STRICMP(lp, ".PANELATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, PANEL_ATT);
         }
         else if (!STRICMP(lp, ".FIDUCIALPAD"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PADSTACK_FIDUCIAL);
         }
         else if (!STRICMP(lp, ".FIDUCIALTOPPAD"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PADSTACK_FIDUCIALTOP);
         }
         else if (!STRICMP(lp, ".FIDUCIALBOTPAD"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PADSTACK_FIDUCIALBOT);
         }
         else if (!STRICMP(lp, ".TOOLPAD"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PADSTACK_TOOL);
         }
         else if (!STRICMP(lp, ".FIDUCIALPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PART_FIDUCIAL);
         }
         else if (!STRICMP(lp, ".TOOLPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PART_TOOL);
         }
         else if (!STRICMP(lp, ".FIDUCIALTOPPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PART_FIDUCIALTOP);
         }
         else if (!STRICMP(lp, ".FIDUCIALBOTPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString p = _strupr(lp);
            p.TrimLeft();
            p.TrimRight();
            update_attr(p, PART_FIDUCIALBOT);
         }
         else if (!STRICMP(lp, ".EXPLODESYMBOL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, EXPLODE_SYMBOL);
         }
         else if (!STRICMP(lp, ".PANELSYMBOL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, PANEL_SYMBOL);
         }
         else if (!STRICMP(lp, ".SMDATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, SMD_ATT);
         }
         else if (!STRICMP(lp, ".COMPATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, COMP_ATT);
         }
         else if (!STRICMP(lp, ".GEOMATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, GEOM_ATT);
         }
         else if (!STRICMP(lp, ".TYPEATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            update_attr(lp, TYPE_ATT);
         }
         else if (!STRICMP(lp, ".CLASS_2_COMPATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString classatt = lp;
            classatt.TrimLeft();
            classatt.TrimRight();
            update_attr(classatt, CLASS_2_COMPATT);
         }
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString attr = _strupr(lp);
            attr.TrimLeft();
            attr.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString ccattr = _strupr(lp);
            ccattr.TrimLeft();
            ccattr.TrimRight();

            ALLEGROAttrNameMap *c = new ALLEGROAttrNameMap;
            attrmaparray.SetAtGrow(attrmapcnt++, c);  
            c->allegro_name = attr;
            c->cc_name = ccattr;
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString tllayer = _strupr(lp);
            tllayer.TrimLeft();
            tllayer.TrimRight();

            if ((lp = get_string(NULL, " \t;\n")) == NULL)
               continue; 
            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

				int laytype = stringToLayerTypeTag(cclayer);
            int layindex = get_lay_index(tllayer);	
				if (layindex >=0 && layindex < MAX_LAYERS)
					layer_attr[layindex].attr = laytype;
         }
         else if (!STRICMP(lp, ".MIRRORLAYER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            int l1 = update_layer(lp);

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            int l2 = update_layer(lp);
            layers[l1].mirror = l2;
            layers[l2].mirror = l1;
         }
         else if (!STRICMP(lp, ".USEPINNAME"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRCMPI(lp, "Y"))
               usepinname = TRUE;
         }
         else if (!STRICMP(lp, ".USE_INTERNAL_PAD_DEF"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRCMPI(lp, "Y"))
               useinternalpaddef = TRUE;
         }
			else if (!STRICMP(lp, ".BOARD_THICKNESS_DERIVED_FROM"))
         {
				//	Possible values of THICKNESS_FIELD or SUM_OF_LAYERS.
				// But really only THICKNESS_FIELD matters, 
				// defaults to SUM_OF_LAYERS for backward compatibility.
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (!STRCMPI(lp, "THICKNESS_FIELD"))
               useHeaderBoardThickness = TRUE;
         }
         else
         {
            messageFilter->formatMessage("Unknown keyword [%s] in [%s] at %ld", lp, fname, linecnt);
            display_error++;
         }
      }
   }

   fclose(fp);
}

/******************************************************************************
* execute_recordbuffer
*/
static int execute_recordbuffer(int filetype)
{
   // this orders all electrical and dialoetrical layers
   // the first POSITIVE conductor is always SIGNAL_TOP and PAD_TOP
   // the last POSITIVE conductor is always SIGNAL_BOTTOM and PAD_BOTTOM

   // Layer/Brd file
   switch (filetype)
   {
   case CAMCAD_EXTRACT_LAYER:
      do_layerfile();
      break;
   case EXTRACT_PAD: // PAD file
      do_padstackfile();
      break;
   case CAMCAD_EXTRACT_SYM:   // sym file
      do_symfile();
      break;
   case CAMCAD_EXTRACT_CLASSIC_SYM: // sym file
      do_classic_symfile();
      break;
   case CAMCAD_EXTRACT_RTE:   // this is the .r file
      do_netfile();
      break;
   case CAMCAD_EXTRACT_CLASSIC_RTE: // this is the .r file
      do_classic_netfile();
      break;
   case FABMASTER_EXTRACT_LAYERS:   // fabmaster
      do_fabmaster_layer();
      break;
   case FABMASTER_EXTRACT_CLASS_VIA:   // fabmaster
      do_fabmaster_vias();
      break;
   case FABMASTER_EXTRACT_PINS1: // fabmaster
      do_fabmaster_geompins(1);
      break;
   case FABMASTER_EXTRACT_PINS:  // fabmaster
      do_fabmaster_geompins(0);
      break;
   case FABMASTER_EXTRACT_CLASS_GRAPHIC:  // fabmaster
      do_fabmaster_traces();
      break;
   case FABMASTER_EXTRACT_GRAPHIC:  // fabmaster
      // not implemented;
      do_fabmaster_geomgraphics();
      break;
   case FABMASTER_EXTRACT_NET_NAME: // fabmaster
      do_fabmaster_netlist();
      break;
   case FABMASTER_EXTRACT_REFDES:   // fabmaster
      suppress_class = TRUE;        // fabmaster must supress because not always CLASS is extracted
      do_fabmaster_refdes();
      break;
   case EXTRACT_PAD_SHAPE_NAME: // shape file
      do_padshapefile();
      break;
   default:
      // unknown filetype
      return -1;
   }

   return 0;
}

/******************************************************************************
* do_layerfile
*/
static int do_layerfile()
{
   int      i=0;
   CString  layermaterial = "";
   CString  lay;
   CString  layer_conductor = "NO";
   int      artwork = 0;   // 0 unknown 1 positive, 2 negative
   int      layersortnumber;

   if (!(i=get_label(VAL_LAYER_SUBCLASS)))
   {
      lab_error("LAYER_SUBCLASS expected");
      return 0;
   }
   lay = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_LAYER_SORT)))
   {
      lab_error("LAYER_SORT expected");
      return 0;
   }
   layersortnumber = atoi(recordArray[i-1]->tok); // this is not the correct stack number, just the layer sort number

   if (i=get_label(VAL_LAYER_CONDUCTOR))
   {
      layer_conductor = recordArray[i-1]->tok;
   }
   
   if (i=get_label(VAL_LAYER_MATERIAL))
   {
      layermaterial = recordArray[i-1]->tok;
   }

   if (layermaterial.CompareNoCase("AIR") == 0)
      return 0;   // do not process an AIR layers. (This are done as the 2 outer layers.

   // has no name
   if (strlen(lay) == 0)
   {
      if (layer_conductor.CompareNoCase("NO") == 0)
         lay.Format("DIALECTRIC_NONAME_%d",layersortnumber);
      else
         lay.Format("ELECTRICAL_NONAME_%d",layersortnumber);
   }

   int w1 = update_layer(lay);
   layers[w1].used = TRUE;
   cur_layer = Graph_Level(lay,"", 0);
   LayerStruct *l = globalDoc->FindLayer(cur_layer);
		
   if (i=get_label(VAL_LAYER_ARTWORK))
   {
      // here is avail
      if (!STRCMPI(recordArray[i-1]->tok,"POSITIVE"))
         artwork = 1;
      else
      if (!STRCMPI(recordArray[i-1]->tok,"NEGATIVE"))
         artwork = 2;
   }

   if (i=get_label(VAL_LAYER_USE))
   {
      // here is avail
   }
   // lay = recordArray[i-1]->tok;

   physicalstack++;

   layers[w1].physicalstack = physicalstack;
	if (l != NULL)
	{
		l->setPhysicalStackNumber(physicalstack);      
	}
	if (!STRCMPI(layer_conductor,"NO"))
	{
		if (l!=NULL)
			l->setLayerType(LAYTYPE_DIALECTRIC);
		layers[w1].typ = 2;
	}

	if (!STRCMPI(layer_conductor,"YES"))
	{
		layers[w1].typ = 1;

		electricalstack++;
		if (l!= NULL)
		{
			l->setElectricalStackNumber(electricalstack);     
			if (!lay.CompareNoCase("TOP"))
				l->setLayerType(LAYTYPE_SIGNAL_TOP);
			else
			if (!lay.CompareNoCase("BOTTOM"))
				l->setLayerType(LAYTYPE_SIGNAL_BOT);
			else
			if(artwork == 1)
				l->setLayerType(LAYTYPE_SIGNAL_INNER);
			else
			if(artwork == 2)
				l->setLayerType(LAYTYPE_POWERNEG);
		}
	}
	
   
   if (i=get_label(VAL_LAYER_DIELECTRIC_CONSTANT))
   {
      int r = globalDoc->RegisterKeyWord("LAYER_DIELECTRIC_CONSTANT", 0,valueTypeString );
      globalDoc->SetAttrib(&l->getAttributesRef(), r,
         valueTypeString, recordArray[i-1]->tok.GetBuffer(0), attributeUpdateOverwrite, NULL); //
   }

   if (i=get_label(VAL_LAYER_ELECTRICAL_CONDUCTIVITY))
   {
      int r = globalDoc->RegisterKeyWord("LAYER_ELECTRICAL_CONDUCTIVITY", 0,valueTypeString );
      globalDoc->SetAttrib(&l->getAttributesRef(),r,
         valueTypeString, recordArray[i-1]->tok.GetBuffer(0), attributeUpdateOverwrite, NULL); //
   }

      // here is avail
   if (strlen(layermaterial))
      globalDoc->SetAttrib(&l->getAttributesRef(),globalDoc->IsKeyWord(LAYATT_MATERIAL, 0),
         valueTypeString, layermaterial.GetBuffer(0), attributeUpdateOverwrite, NULL); //

   if (i=get_label(VAL_LAYER_SHIELD_LAYER))
   {
      // ???
   }

   if (i=get_label(VAL_LAYER_THERMAL_CONDUCTIVITY))
   {
      globalDoc->SetAttrib(&l->getAttributesRef(),globalDoc->IsKeyWord(LAYATT_THERMAL_CONDUCTIVITY, 0),
         valueTypeString, recordArray[i-1]->tok.GetBuffer(0), attributeUpdateOverwrite, NULL); //
   }

   if (i=get_label(VAL_LAYER_THICKNESS))
   {
		CString layerThicknessSpec = recordArray[i-1]->tok;

		int thkPos = 0;
		CString thicknessStr = layerThicknessSpec.Tokenize(" ", thkPos);
		CString thicknessUnits = layerThicknessSpec.Tokenize(" ", thkPos);
		double layerThkScaleFactor = getToPageUnitsScaleFactor(thicknessUnits);

      double h = atof(thicknessStr) * layerThkScaleFactor;
      globalDoc->SetAttrib(&l->getAttributesRef(),globalDoc->IsKeyWord(LAYATT_THICKNESS, 0),
         valueTypeUnitDouble, &h, attributeUpdateOverwrite, NULL); //
      layers[w1].thickness = h;
   }

   return 1;
}

static CString getComplexApGeomName(CString shapename, CString layername)
{
   CString complexGeomName;

#ifdef UNIQUE_AP_GEOM_PER_SHAPE_AND_LAYER
   // Deans says don't do this, just make it one shape regardless of fact that format supports
   // unique geometry per shape and layer pair. Dean says the cad sys can't actually to that even
   // though teh format would support it.
   complexGeomName.Format("CMPLX_AP_GEOM_%s$%s", shapename, layername);
#else
   complexGeomName.Format("CMPLX_AP_GEOM_%s", shapename);
#endif

   return complexGeomName;
}

/******************************************************************************
* do_padstackfile 
*/
static int do_padstackfile()
{
   BlockStruct *b;
   int   i=0;
   CString padstackname, pd, lay;

   if (!(i=get_label(VAL_PAD_NAME)))
   {
      lab_error("PAD_NAME expected");
      return 0;
   }
   // cadence allows a padstack be the same name as a symbol
   padstackname.Format("PADSTACK_%s",recordArray[i-1]->tok);
   b = Graph_Block_On(GBO_APPEND,padstackname,currentPCBFile->getFileNumber(),0);
   b->setBlockType(BLOCKTYPE_PADSTACK);
   b->setOriginalName(recordArray[i-1]->tok);

   int pt = get_attrib(b->getOriginalName());
   if (pt > -1)
   {
      switch (attr[pt].atttype)
      {
         case PADSTACK_TOOL:
            b->setBlockType(BLOCKTYPE_TOOLING);
         break;
         case PADSTACK_FIDUCIALTOP:
         case PADSTACK_FIDUCIALBOT:
         case PADSTACK_FIDUCIAL:
            b->setBlockType(BLOCKTYPE_FIDUCIAL);
         break;
      }
   }

   if (is_attrib(recordArray[i-1]->tok, SMD_ATT))
   {
      globalDoc->SetAttrib(&b->getAttributesRef(),globalDoc->IsKeyWord(ATT_SMDSHAPE, 0), valueTypeNone, NULL, attributeUpdateOverwrite, NULL); //
   }
   if (!(i=get_label(VAL_PADSHAPE1)))
   {
      lab_error("PADSHAPE1 expected");
      return 0;
   }
   pd = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_LAYER)))
   {
      lab_error("LAYER expected");
      return 0;
   }
   lay = recordArray[i-1]->tok;

   // internal_pad_def
   if (!STRCMPI(lay,"internal_pad_def"))
   {
      // it seems that internal pad def is only a place holder
      if (!useinternalpaddef)
         return 0;   // do not use internal paddef
   }

   if (!STRCMPI(lay, "~DRILL"))
   {
      double d = atof(pd)*scale_factor;

      Graph_Block_Off();   // close the open padshape.

      if (d > 0)
      {
         int drilllayer = Graph_Level("DRILLHOLE","",0);

         if (!drill_layer_done)
         {
            LayerStruct *l = globalDoc->FindLayer(drilllayer);
				if (l != NULL)
				{
					l->setComment("Drill Hole");
					l->setVisible(false);
				}
				drill_layer_done = TRUE;				
         }

			bool plated = false;
			if ((i = get_label(VAL_PADSHAPENAME)) > 0)
				plated = (recordArray[i-1]->tok.CompareNoCase("p") == 0)?true:false;

         CString drillname;
			drillname.Format("DRILL_%d", get_drillindex(d, plated, drilllayer));

         Graph_Block_On(GBO_APPEND, padstackname, currentPCBFile->getFileNumber(), 0);
         Graph_Block_Reference(drillname, NULL,0, 0, 0, 0.0, 0, 1.0, drilllayer, TRUE);
         Graph_Block_Off();

         // only if a drill.
         update_padshape(padstackname);
      }
      return 0;
   }

   Graph_Block_Off();

   if (strlen(pd) == 0)
      return 0;

   double w,h;
   if (!(i=get_label(VAL_PADWIDTH)))
   {
      lab_error("PADWIDTH expected");
      return 0;
   }
   w = atof(recordArray[i-1]->tok) * scale_factor;
   // get PADHGHT
   // get PADWIDTH
   if (!(i=get_label(VAL_PADHGHT)))
   {
      lab_error("PADHGHT expected");
      return 0;
   }
   h = atof(recordArray[i-1]->tok) * scale_factor;

   // no width, no height
   if (w == 0 && h == 0)
      return 0;

   int w1 = update_layer(lay);
   layers[w1].used = TRUE;
   cur_layer = Graph_Level(lay,"", 0);

   LayerStruct *ll = globalDoc->FindLayer(cur_layer);

   assign_padlayertypes(cur_layer);

   if (!STRCMPI(pd,"CIRCLE"))
   {
      double offx = 0, offy = 0;
      if (w > 0)
      {
         char  pshapename[80];
         // make a circle 
         make_pshapename(pshapename,apertureRound,w,0);
         // add to pd
         Graph_Block_On(GBO_APPEND,padstackname,currentPCBFile->getFileNumber(),0);
         Graph_Block_Reference(pshapename, NULL, 0, offx, offy, 0.0, 0 , 1.0, cur_layer, TRUE);
         Graph_Block_Off();
      }
   }
   else if (!STRCMPI(pd,"RECTANGLE"))
   {
      double offx = 0, offy = 0;

      if (!(i=get_label(VAL_PADXOFF)))
      {
         lab_error("PADXOFF expected");
         return 0;
      }
      offx = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_PADYOFF)))
      {
         lab_error("PADYOFF expected");
         return 0;
      }
      offy = atof(recordArray[i-1]->tok) * scale_factor;

      if (w > 0)
      {
         char  pshapename[80];
         // make a rectangle
         make_pshapename(pshapename,apertureRectangle,w,h);
         // add to pd
         Graph_Block_On(GBO_APPEND,padstackname, currentPCBFile->getFileNumber(),0);
         Graph_Block_Reference(pshapename, NULL, 0, offx, offy, 0.0, 0 , 1.0, cur_layer, TRUE);
         Graph_Block_Off();
      }
   }
   else if (!STRCMPI(pd,"OCTAGON"))
   {
      double offx = 0, offy = 0;

      if (!(i=get_label(VAL_PADXOFF)))
      {
         lab_error("PADXOFF expected");
         return 0;
      }
      offx = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_PADYOFF)))
      {
         lab_error("PADYOFF expected");
         return 0;
      }
      offy = atof(recordArray[i-1]->tok) * scale_factor;

      if (w > 0)
      {
         char  pshapename[80];
         // make an octagon
         make_pshapename(pshapename,apertureOctagon,w,h);
         // add to pd
         Graph_Block_On(GBO_APPEND,padstackname, currentPCBFile->getFileNumber(),0);
         Graph_Block_Reference(pshapename, NULL, 0, offx, offy, 0.0, 0 , 1.0, cur_layer, TRUE);
         Graph_Block_Off();
      }
   }
   else if (!STRCMPI(pd,"SHAPE"))
   {
      double offx = 0, offy = 0;
      CString  padshapename;

      if (!(i=get_label(VAL_PADXOFF)))
      {
         lab_error("PADXOFF expected");
         return 0;
      }
      offx = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i=get_label(VAL_PADYOFF)))
      {
         lab_error("PADYOFF expected");
         return 0;
      }
      offy = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i=get_label(VAL_PADSHAPENAME)))
      {
         lab_error("PADSHAPENAME expected");
         return 0;
      }
      padshapename = recordArray[i-1]->tok;

      if (AllegroExtractType == ALLEGRO_FABMASTER ||  // fabmaster has fig_shape section, in single file
         TotalFileCount == 5)  // newer camcad extract has five files, 5th is shape file
      {
         CString complexGeomName = getComplexApGeomName(padshapename, lay);
         BlockStruct *complexGeomBlk = Graph_Block_On(GBO_APPEND, complexGeomName, currentPCBFile->getFileNumber(), 0);
         // no geometry available here, fill it in later ... Graph_Block_Reference(pshapename, NULL, 0, offx, offy, 0.0, 0 , 1.0, cur_layer, TRUE);
         Graph_Block_Off();

#ifdef UNIQUE_AP_GEOM_PER_SHAPE_AND_LAYER
         // If the geoms are unique per layer then the aperture name must also be
         CString complexApName;
         complexApName.Format("%s$%s", padshapename, lay);
#else
         // If not unique per layer then shapename can be used directly
         CString complexApName(padshapename); 
#endif
         Graph_Complex(complexApName, 0, complexGeomBlk, 0., 0., 0.);

         Graph_Block_On(GBO_APPEND, padstackname, currentPCBFile->getFileNumber(), 0);
         // Something is peculiar about offx & offy, don't use them here.
         // It is looking like offsets get redundantly specified in the data, and putting them here messes
         // up reconciling the complex ap geometry from the pcb-origin-relative graphic data elsewhere.
         Graph_Block_Reference(complexApName, NULL, currentPCBFile->getFileNumber(), 0./*offx*/, 0./*offy*/, 0.0, 0 , 1.0, cur_layer, TRUE);
         Graph_Block_Off();
      }
      else
      {
         // "classic" behavior, make surrogate rectangle shape because shape definition is not available
         if (w > 0)
         {
            char  surrogateshapename[80];
            make_pshapename(surrogateshapename, apertureRectangle, w, h);

            Graph_Block_On(GBO_APPEND, padstackname, currentPCBFile->getFileNumber(), 0);
            Graph_Block_Reference(surrogateshapename, NULL, 0, offx, offy, 0.0, 0 , 1.0, cur_layer, TRUE);
            Graph_Block_Off();
         }
         // messageFilter->formatMessage("Padshape1 [SHAPE] is read in as [RECTANGLE]\n");
         messageFilter->formatMessage("Complex Padshape \"%s\" found in padstack \"%s\" on layer \"%s\", read in as rectangle.",
            padshapename, padstackname, lay);
         display_error++;
      }
   }
   else if (!STRCMPI(pd, "SQUARE"))
   {
      double offx = 0, offy = 0;

      if (!(i=get_label(VAL_PADXOFF)))
      {
         lab_error("PADXOFF expected");
         return 0;
      }
      offx = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_PADYOFF)))
      {
         lab_error("PADYOFF expected");
         return 0;
      }
      offy = atof(recordArray[i-1]->tok) * scale_factor;

      if (w > 0)
      {
         char  pshapename[80];
         // make a rectangle
         make_pshapename(pshapename,apertureSquare,w,h);
         // add to pd
         Graph_Block_On(GBO_APPEND, padstackname, currentPCBFile->getFileNumber(),0);
         Graph_Block_Reference(pshapename, NULL, 0, offx, offy, 0.0, 0 , 1.0, cur_layer, TRUE);
         Graph_Block_Off();
      }
   }
   else if (!STRCMPI(pd, "OBLONG") || !STRCMPI(pd, "OBLONG_X") || !STRCMPI(pd, "OBLONG_Y"))
   {
      if (!(i=get_label(VAL_PADXOFF)))
      {
         lab_error("PADXOFF expected");
         return 0;
      }
      double offx = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i=get_label(VAL_PADYOFF)))
      {
         lab_error("PADYOFF expected");
         return 0;
      }
      double offy = atof(recordArray[i-1]->tok) * scale_factor;

      if (w > 0)
      {
         char pshapename[80];
         // make a oblong 
         make_pshapename(pshapename, apertureOblong, w, h);
         // add to pd
         Graph_Block_On(GBO_APPEND, padstackname, currentPCBFile->getFileNumber(), 0);
         Graph_Block_Reference(pshapename, NULL, 0, offx, offy, 0, 0, 1, cur_layer, TRUE);
         Graph_Block_Off();
      }
   }
   else
   {
      // unknown padshape
      CString tmp;
      tmp.Format("Unknown Padshape [%s] for Padstack [%s]", pd, padstackname);
      lab_error(tmp);
   }
   update_padshape(padstackname);

   return 1;
}

/******************************************************************************
*/
DataStruct *FindComponentData(FileStruct *pcbFile, CString refname)
{
   if (pcbFile != NULL)
   {
      BlockStruct *pcbBlock = pcbFile->getBlock();

	   for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext(); /**/ )
	   {
		   DataStruct* data = insertIterator.getNext();
         // For this usage, we don't care what particular insert type it is
         //if (data->isInsertType(insertTypePcbComponent) || data->isInsertType(insertTypeTestPoint))
         {
            if (data->getInsert()->getRefname().CompareNoCase(refname) == 0)
               return data;
         }
      }
   }

	return NULL;
}

static DataStruct *FindComponentPinData(CCEtoODBDoc *doc, DataStruct *pcbComp, CString pinName)
{
   if (pcbComp != NULL && pcbComp->getDataType() == dataTypeInsert && pcbComp->getInsert() != NULL)
   {
      int insertedBlockNum = pcbComp->getInsert()->getBlockNumber();
      BlockStruct *compBlock = doc->getBlockAt(insertedBlockNum);
		if (compBlock!=NULL)
		{
			for (CDataListIterator insertIterator(*compBlock, insertTypePin);insertIterator.hasNext();)
			{
				DataStruct* data = insertIterator.getNext();
				if (data->isInsertType(insertTypePin))  // waste? interator is supposed to already only return insertTypePin
				{
					if (data->getInsert()->getRefname().CompareNoCase(pinName) == 0)
						return data;
				}
			}			
		}
	}

   return NULL;
}

static DataStruct *FindInsertedBlockData(CCEtoODBDoc *doc, DataStruct *parentdata, CString lookingForBlockName)
{
   if (parentdata != NULL && parentdata->getDataType() == dataTypeInsert && parentdata->getInsert() != NULL)
   {
      int insertedBlockNum = parentdata->getInsert()->getBlockNumber();
      BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
		if (insertedBlock !=NULL)
		{
			for (CDataListIterator insertIterator(*insertedBlock,dataTypeInsert);insertIterator.hasNext();)
			{
				DataStruct* subdata = insertIterator.getNext();
				BlockStruct *insertedSubblock = doc->getBlockAt(subdata->getInsert()->getBlockNumber());
				if (insertedSubblock != NULL && insertedSubblock->getName().CompareNoCase(lookingForBlockName) == 0)
				{
					return subdata;
				}
			}
		}
   }

   return NULL;
}


/******************************************************************************
* do_padshapefile 
*/
static int do_padshapefile()
{
   int i;

   if (!(i=get_label(VAL_SUBCLASS)))
   {
      lab_error("SUBCLASS expected");
      return 0;
   }
   CString subclassStr( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_PADSHAPENAME)))
   {
      lab_error("PADSHAPENAME expected");
      return 0;
   }
   CString padShapeNameStr( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_NAME)))
   {
      lab_error("GRAPHIC_DATA_NAME expected");
      return 0;
   }
   CString graphicDataNameStr( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_NUMBER)))
   {
      lab_error("GRAPHIC_DATA_NAME expected");
      return 0;
   }
   CString graphicDataNumberStr( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_RECORD_TAG)))
   {
      lab_error("RECORD_TAG expected");
      return 0;
   }
   CString recordTagStr( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_1)))
   {
      lab_error("GRAPHIC_DATA_1 expected");
      return 0;
   }
   CString graphicData1Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_2)))
   {
      lab_error("GRAPHIC_DATA_2 expected");
      return 0;
   }
   CString graphicData2Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_3)))
   {
      lab_error("GRAPHIC_DATA_3 expected");
      return 0;
   }
   CString graphicData3Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_4)))
   {
      lab_error("GRAPHIC_DATA_4 expected");
      return 0;
   }
   CString graphicData4Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_5)))
   {
      lab_error("GRAPHIC_DATA_5 expected");
      return 0;
   }
   CString graphicData5Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_6)))
   {
      lab_error("GRAPHIC_DATA_6 expected");
      return 0;
   }
   CString graphicData6Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_7)))
   {
      lab_error("GRAPHIC_DATA_7 expected");
      return 0;
   }
   CString graphicData7Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_8)))
   {
      lab_error("GRAPHIC_DATA_8 expected");
      return 0;
   }
   CString graphicData8Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_GRAPHIC_DATA_9)))
   {
      lab_error("GRAPHIC_DATA_9 expected");
      return 0;
   }
   CString graphicData9Str( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_PAD_STACK_NAME)))
   {
      lab_error("PAD_STACK_NAME expected");
      return 0;
   }
   CString padStackNameStr( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_REFDES)))
   {
      lab_error("REFDES expected");
      return 0;
   }
   CString refDesStr( recordArray[i-1]->tok );

   if (!(i=get_label(VAL_PIN_NUMBER)))
   {
      lab_error("PIN_NUMBER expected");
      return 0;
   }
   CString pinNumberStr( recordArray[i-1]->tok );

   // We are interested only in FIG_SHAPE records.
   // If this is not one, then skip it.
   if (padShapeNameStr.Find("FIG_SHAPE") < 0)
      return 1;

   // The shape name has the form "FIG_SHAPE figshapename". Remove the "FIG_SHAPE" portion.
   padShapeNameStr.Replace("FIG_SHAPE", "");
   padShapeNameStr.Trim();

   CString layerName(subclassStr);
   layerName.MakeUpper();

   // Pad Shapes appear in the data on a per-component-pin basis, i.e. the pad is defined for every pin in every component
   // that uses it. Dean's instructions say to use the first occurence of a given pad shape to define a reusable block
   // for all. 

   // The subclass value defines the layer for the shape.
   // If we do not yet have a definition for this shape on this layer, then make one.
   // We can't use Graph_Block_Exists() because the blocks likely do exist, they are made
   // when reading padstacks, but there is no graphics there so they are empty.

   CString complexGeomName = getComplexApGeomName(padShapeNameStr, layerName);

   // Extract record ID and sequence number
   CSupString recordTagSupStr( recordTagStr );
   CStringArray recordTagParams;
   recordTagSupStr.ParseWhite(recordTagParams);
   int curRecId = -1;
   int curSeqId = -1;
   if (recordTagParams.GetCount() > 0)
      curRecId = atoi(recordTagParams.GetAt(0));
   if (recordTagParams.GetCount() > 1)
      curSeqId = atoi(recordTagParams.GetAt(1));

   // Check for continuing definition.
   // If this padshapname has not been processed then it is new, define it.
   // If name has been seen and record Id matches and sequence Id is incrementing, we are continuing definition.
   // If neither of the above, then it is a duplicate definition.
   if (curRecId > 0 && curSeqId > 0)
   {
      int ignoredVal;
      if (!AllegroComplexApGeomMap.Lookup(complexGeomName, ignoredVal))
      {
         // Not seen before, define now. curSeqId should be 1.
         CmplxLastRecId = curRecId;
         CmplxLastSeqId = 0;
         AllegroComplexApGeomMap.SetAt(complexGeomName, 0);
      }

      if (curRecId == CmplxLastRecId && curSeqId == (CmplxLastSeqId + 1))
      {
         // Define graphic element
         CmplxLastSeqId = curSeqId;

         BlockStruct *b = b = Graph_Block_On(GBO_APPEND, complexGeomName, currentPCBFile->getFileNumber(), 0);

         if (curSeqId == 1)
         {
            LayerStruct *floatingLayer = globalDoc->getDefinedFloatingLayer();
            DataStruct *d = Graph_PolyStruct(floatingLayer->getLayerIndex(), 0, FALSE);
         
            // Data is in as-placed location (relative to board 0,0), get transform to change it back
            // to reletive to own origin.
            DataStruct *compData = FindComponentData(currentPCBFile, refDesStr);
            DataStruct *pinData = FindComponentPinData(globalDoc, compData, pinNumberStr);
            DataStruct *apertureData = FindInsertedBlockData(globalDoc, pinData, padShapeNameStr);
            if (compData != NULL && pinData != NULL && apertureData != NULL)
            {
               CTMatrix compMat(compData->getInsert()->getTMatrix());

               CBasesVector pinBV(pinData->getInsert()->getBasesVector());
					pinBV.transform(compMat);

               CBasesVector apertureBV(apertureData->getInsert()->getBasesVector());
               CTMatrix apertureMat(apertureBV.getTransformationMatrix());
               
               CBasesVector finalBV(apertureMat.getBasesVector());
               finalBV.transform(pinBV.getTransformationMatrix());

               CmplxHome = finalBV.getTransformationMatrix();
               CmplxHome.invert();
            }
            else
            {
               if (compData == NULL)
               {
                  messageFilter->formatMessage("Can not find component \"%s\" to resolve geoemetry transform for aperture \"%s\".",
                     refDesStr, padShapeNameStr);
                  display_error++;
               }
               else if (pinData == NULL)
               {
                  messageFilter->formatMessage("Can not find component \"%s\" pin \"%s\" to resolve geoemetry transform for aperture \"%s\".",
                     refDesStr, pinNumberStr, padShapeNameStr);
                  display_error++;
               }
               else
               {
                  messageFilter->formatMessage("Can not find aperture \"%s\" insert in padstack \"%s\" in component \"%s\" pin \"%s\" to resolve geoemetry transform for aperture \"%s\".",
                     padShapeNameStr, padStackNameStr, refDesStr, pinNumberStr, padShapeNameStr);
                  display_error++;
               }
            }
         }

         if (graphicDataNameStr.CompareNoCase("LINE") == 0)
         {
            double x1 = atof(graphicData1Str) * scale_factor;
            double y1 = atof(graphicData2Str) * scale_factor;
            double x2 = atof(graphicData3Str) * scale_factor;
            double y2 = atof(graphicData4Str) * scale_factor;

            CmplxHome.transform(x1, y1);
            CmplxHome.transform(x2, y2);


            double width = atof(graphicData5Str) * scale_factor;

            int widthindex = 0;
            if (width > 0.0)
               widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, NULL);

            if (curSeqId == 1)
               CmplxPoly = Graph_Poly(NULL, widthindex, TRUE/*filled*/, FALSE/*void*/, TRUE/*closed*/); // filled and closed

            CPnt *lastPnt = (CmplxPoly->getVertexCount() > 0) ? (CmplxPoly->getPntList().GetTail()) : NULL;
            if (lastPnt == NULL || fabs(lastPnt->x - x1) > SMALLNUMBER || fabs(lastPnt->y - y1) > SMALLNUMBER)
            {
               Graph_Vertex(x1, y1, 0.0);
            }
            Graph_Vertex(x2, y2, 0.0);
         }
         else if (graphicDataNameStr.CompareNoCase("ARC") == 0)
         {
            double x1 = atof(graphicData1Str) * scale_factor;
            double y1 = atof(graphicData2Str) * scale_factor;
            double x2 = atof(graphicData3Str) * scale_factor;
            double y2 = atof(graphicData4Str) * scale_factor;
            double xc = atof(graphicData5Str) * scale_factor;
            double yc = atof(graphicData6Str) * scale_factor;
            double rad = atof(graphicData7Str) * scale_factor;
            double width = atof(graphicData8Str) * scale_factor;

            bool clockwise = (graphicData9Str.CompareNoCase("CLOCKWISE") == 0);

            CmplxHome.transform(x1, y1);
            CmplxHome.transform(x2, y2);
            CmplxHome.transform(xc, yc);

            int widthindex = 0;
            if (width > 0.0)
               widthindex = Graph_Aperture("", T_ROUND, width , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, NULL);

            if (curSeqId == 1)
               CmplxPoly = Graph_Poly(NULL, widthindex, TRUE/*filled*/, FALSE/*void*/, TRUE/*closed*/); // filled and closed

            if (CmplxHome.getMirror())
               clockwise = !clockwise;
             
            double r, sa, da;
            ArcCenter2(x1, y1, x2, y2, xc, yc, &r, &sa, &da, clockwise);



            //if (fabs(da) < ALMOSTZERO) // straight line
            //   da = 0;
            //else if (clockwise)
            //   da = da - PI2;

            CPnt *lastPnt = (CmplxPoly->getVertexCount() > 0) ? (CmplxPoly->getPntList().GetTail()) : NULL;
            if (lastPnt != NULL && fabs(lastPnt->x - x1) < SMALLNUMBER && fabs(lastPnt->y - y1) < SMALLNUMBER)
            { 
               double bulge = tan(da/4);
               lastPnt->bulge = (DbUnit)bulge;
               Graph_Vertex(x2, y2, 0.0);
            }
            else
            {
               Graph_Poly_Arc(xc, yc, rad, sa, da);
            }
            
         }
         else
         {
            messageFilter->formatMessage("Complex Padshape \"%s\" contains unsupported graphic primitive \"%s\".", 
               padShapeNameStr, graphicDataNameStr);
            display_error++;
         }

         Graph_Block_Off();
      }

   }

   return 1;
}

/******************************************************************************
* do_symfile
*/
static int do_symfile()
{
	int cnt = 0;
   int i = 0;
   if (!(i = get_label(VAL_SYM_TYPE)))
   {
      lab_error("SYM_TYPE expected");
      return 0;
   }
   CString stype = recordArray[i-1]->tok;

   // component, shape graphic
   // get symname+symx+symy
   if (!(i = get_label(VAL_SYM_NAME)))
   {
      lab_error("SYM_NAME expected");
      return 0;
   }
   CString sname = recordArray[i-1]->tok;


   if (sname.CompareNoCase("panel") == 0)
   {
      // this are restricted names
      messageFilter->formatMessage("Restricted Symbol name [panel] used -> changed to [!panel]");
      sname = "!panel";
   }
   if (sname.CompareNoCase("board") == 0)
   {
      // this are restricted names
      messageFilter->formatMessage("Restricted Symbol name [board] used -> changed to [!board]");
      sname = "!board";
   }

   if (is_attrib(sname, MECHANICAL_SYMBOL))  
      stype = "MECHANICAL";

   if (is_attrib(sname, PANEL_SYMBOL)) 
      set_panel();

   // record_tag
   if (!(i = get_label(VAL_RECORD_TAG)))
   {
      lab_error("RECORD_TAG expected");
      return 0;
   }

   char rtag[80];
   strcpy(rtag, recordArray[i-1]->tok);

   int majj = 0;
   int minj = 0;
   int littlej = 0;
   char *lp;

   if ((lp = strtok(rtag, " \t")) != NULL)
   {
      majj = atoi(lp);

      if ((lp = strtok(NULL, " \t")) != NULL)
      {
         minj = atoi(lp);
         if ((lp = strtok(NULL, " \t")) != NULL)
            littlej = atoi(lp);
      }
   }
   lastmaj = majj;

   if (!(i = get_label(VAL_SUBCLASS)))
   {
      lab_error("SUBCLASS expected");
      return 0;
   }
   CString subclass = recordArray[i-1]->tok;
   CString lay = recordArray[i-1]->tok;
   cur_layer_subclass = subclass;

   if (!(i = get_label(VAL_CLASS)))
   {
      lab_error("CLASS expected");
      return 0;
   }
   char class_name[80];
   strcpy(class_name, recordArray[i-1]->tok);
   cur_layer_class = class_name;
   
   if (!suppress_class)
   {
      //lay gets prefix, but not if it is etch.
      if (STRCMPI(class_name, "ETCH"))
      {
         CString tmp;
         tmp = class_name;
         tmp += ",";
         tmp += lay;
         lay = tmp;
      }
   }

   int w1 = update_layer(lay);
   layers[w1].used = TRUE;
    cur_layer = Graph_Level(lay, "", 0);

   // get symbol type
   int graphic_class = 0;                    
   
   // First, check if this will be a probe placement and handle seperate from othe cases (always at board level)
   CString gdname = "";
   if (i=get_label(VAL_GRAPHIC_DATA_NAME))
      gdname = recordArray[i-1]->tok;
   if ((graphic_class = get_graphic_class(class_name, TRUE)) == 0)
      graphic_class = get_graphic_subclass(subclass);

   if ((!gdname.CompareNoCase("TEXT") && graphic_class == graphicClassManufacturing) && (!subclass.CompareNoCase("PROBE_TOP") || !subclass.CompareNoCase("PROBE_BOTTOM")))
   {
	   do_graphic("", 0, 0, 0, 0, majj, minj, littlej, graphic_class, FALSE, NULL);
   }
   else if (/*is_attrib(sname, EXPLODE_SYMBOL) ||*/ strlen(stype) == 0)
   {
      // trace and other graphic
      strcpy(cur_block, "");
      if (!(i=get_label(VAL_NET_NAME)))
      {
         lab_error("NET_NAME expected");
         return 0;
      }
      CString netname = recordArray[i-1]->tok;
      
      if ((graphic_class = get_graphic_class(class_name, netname.IsEmpty())) == 0)
         graphic_class = get_graphic_subclass(subclass);

      if (is_primboardoutline(lay, class_name))
      {
         graphic_class = graphicClassBoardOutline;
         set_pcb();
      }
      else
      {
         if (is_primpaneloutline(lay, class_name))
         {
            graphic_class = graphicClassPanelOutline;
            set_panel();
         }
         else if (is_primarray(class_name, subclass, PANEL_GRAPHIC))
         {
            set_panel();
         }
      }
      cur_layer = Graph_Level(lay, "", 0);
      do_graphic(netname, 0, 0, 0, 0, majj, minj, littlej, graphic_class, TRUE, NULL);
   }
   else if (!strcmp(stype, "PACKAGE") || !strcmp(stype, "MECHANICAL"))
   {
      if (!(i = get_label(VAL_REFDES)))
      {
         lab_error("REFDES expected");
         return 0;
      }
      CString refdes = recordArray[i-1]->tok;


      if (!(i = get_label(VAL_SYM_X)))
      {
         lab_error("SYM_X expected");
         return 0;
      }
      double sym_x = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i = get_label(VAL_SYM_Y)))
      {
         lab_error("SYM_Y expected");
         return 0;
      }
      double sym_y = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i = get_label(VAL_SYM_ROTATE)))
      {
         lab_error("SYM_ROTATE expected");
         return 0;
      }
      double sym_rot = atof(recordArray[i-1]->tok);

      if (!(i = get_label(VAL_SYM_MIRROR)))
      {
         lab_error("SYM_MIRROR expected");
         return 0;
      }
      int sym_mir = !STRICMP(recordArray[i-1]->tok, "YES");

      // if a refname ends in * than it is an unnamed component and needs to get 
      // the sym_koos added to the name
      if (!STRICMP(refdes.Right(1), "*"))
      {
         CString t;
         t.Format("%s_%2.3lf_%2.3lf_%1.0lf_%d", refdes, sym_x, sym_y, sym_rot, sym_mir);
         refdes = t;
      }
		else if (strlen(refdes) == 0)
		{
			CString t;
			t.Format("%s_%2.3lf_%2.3lf_%1.0lf_%d", sname, sym_x, sym_y, sym_rot, sym_mir);
			refdes = t;
		}

      // always create if it is a new instance
      CString name;
      name.Format("%s_%s", sname, refdes);

      if (is_attrib(sname, PANEL_ATT) || is_attrib(sname, BOARD_SYMBOL))   // if this is a step and repeat
      {
         if (STRCMPI(cur_placeblock, name))
         {
            // the sym_x, sym_y placement is not the placement of the PCB, but the
            // placemement of the steprep symbol. The placement of the pcb has
            // the offset of the steprep symbols. The first steprep symbol is
            // the symbol lowerleft of the primaryboard outline.
            ALLEGROStepRepeat *p = new ALLEGROStepRepeat; 
            stepreparray.SetAtGrow(stepreparraycnt++, p);
            p->x = sym_x;
            p->y = sym_y;
            p->rot = sym_rot;
            p->mir = sym_mir;
            strcpy(cur_placeblock, name);
         }
		}
      else
      {
         // if DIP14_IC1 is now DIP14_IC2 kill curblock
         // it still will find it in the masterlist
         if (strcmp(name, cur_placeblock))
            strcpy(cur_block, "");

         // here also check for other classes, which should not be text, but attribute
         if (strcmp(class_name, "REF DES") && !is_attrib(class_name, CLASS_2_COMPATT)) 
         {
            if ((graphic_class = get_graphic_class(class_name, TRUE)) == 0)
               graphic_class = get_graphic_subclass(subclass);

				if (is_primpaneloutline(lay, class_name))
				{
					graphic_class = graphicClassPanelOutline;
					set_panel();

					if (is_attrib(sname, EXPLODE_SYMBOL))
						name.Empty();
				}
				else if (is_primarray(class_name, subclass, PANEL_GRAPHIC))
				{
					set_panel();

					if (is_attrib(sname, EXPLODE_SYMBOL))
						name.Empty();
				}
            else if (is_attrib(sname, PANEL_SYMBOL))
            {
               set_panel();
            }
				else
				{
					if (is_primboardoutline(lay, class_name))
						graphic_class = graphicClassBoardOutline;

					set_pcb();
				}


            // board geometry should only be used in Board outline and graphic definitions, not in
            // components.
            if (graphic_class == GR_CLASS_BOARD_GEOM)
               graphic_class = 0;
            // no etch in a symbol
            if (graphic_class == GR_CLASS_ETCH)
               graphic_class = 0;

				i = get_label(VAL_NET_NAME);
				if (graphic_class == graphicClassBoardOutline || graphic_class == graphicClassPanelOutline)
				{
					cur_layer = Graph_Level(lay, "", 0);

					// board outline or panel outline
					do_graphic("", 0, 0, 0, 0, majj, minj, littlej, graphic_class, FALSE, NULL);
				}
				else 
				{
					CString netname = recordArray[i-1]->tok;
					if (!STRCMPI(class_name, "ETCH"))
					{
						cur_layer = Graph_Level(lay, "", 0);

						// if there are NETNAME and class name ETCH, then insert at board level,
						do_graphic("", 0, 0, 0, 0, majj, minj, littlej, GR_CLASS_ETCH, FALSE, NULL);
					}
					else if (InMasterList(sname, refdes))
					{
						if (sym_mir)
						{                                           
							int w1 = update_layer(lay);
							lay = layers[layers[w1].mirror].name;
							cur_layer = Graph_Level(lay, "", 0);
						}
						else
							cur_layer = Graph_Level(lay, "", 0);

						// case 1891, determine comp outline class after layer determination is final
						if (is_primcompoutline(lay, class_name))
							graphic_class = graphicClassComponentOutline;

						if (is_primcompheight(lay, class_name))
						{
							for (int j=0; j<commandArrayCount; j++)
							{
								if (!STRCMPI("GRAPHIC_DATA_NAME", commandArray[j]->tok) && !STRCMPI(recordArray[j]->tok, "TEXT"))
								{
									graphic_class = COMPHEIGHT_CLASS;
									break;
								}
							}
						}

						if (!(i = get_label(VAL_NET_NAME)))
						{
							lab_error("NET_NAME expected");
							return 0;
						}
						CString netname = recordArray[i-1]->tok;

						BlockStruct *block = NULL;
						if (strlen(netname) <= 0  && STRCMPI(class_name, "ETCH"))
						{
							// if there are NETNAME and class name ETCH, then insert at board level,
							// but since there are NOT so insert at geometry level
							block = Graph_Block_On(GBO_APPEND, sname,currentPCBFile->getFileNumber(), 0);

							if (is_attrib(sname, PART_TOOL))
								block->setBlockType(blockTypeTooling);
							else if (is_attrib(sname, PART_FIDUCIALTOP))
								block->setBlockType(BLOCKTYPE_FIDUCIAL);
							else if (is_attrib(sname, PART_FIDUCIALBOT))
								block->setBlockType(BLOCKTYPE_FIDUCIAL);
							else if (is_attrib(sname, PART_FIDUCIAL))
								block->setBlockType(BLOCKTYPE_FIDUCIAL);
							else if (!STRCMPI(stype, "MECHANICAL"))
								block->setBlockType(BLOCKTYPE_MECHCOMPONENT);
							else     
								block->setBlockType(BLOCKTYPE_PCBCOMPONENT);

							if (is_attrib(sname, SMD_ATT))
								globalDoc->SetUnknownAttrib(&block->getAttributesRef(), ATT_SMDSHAPE, "", attributeUpdateOverwrite, NULL);
							
							do_graphic("", sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, graphic_class, FALSE, &block->getAttributesRef());
							Graph_Block_Off();
						}
					}
				}
			}

         // only place the first unique refdes by coordinate
			CString uniqueRefdes;
			DataStruct *data = NULL;

			if (allegroRefdesMap.GetUniqueRefdesByCoordinate(refdes, sym_x, sym_y, uniqueRefdes))
			{
				data = Graph_Block_Reference(sname, uniqueRefdes,currentPCBFile->getFileNumber(), sym_x, sym_y, DegToRad(sym_rot), sym_mir, 1.0, -1, TRUE);

				BlockStruct* block = globalDoc->getBlockAt(data->getInsert()->getBlockNumber());
				if (block == NULL)
					return 0;

				if (block->getBlockType() == BLOCKTYPE_UNKNOWN)
				{
					// Block is new so check for block type
					if (is_attrib(sname, PART_TOOL))
						block->setBlockType(blockTypeTooling);
					else if (is_attrib(sname, PART_FIDUCIALTOP))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (is_attrib(sname, PART_FIDUCIALBOT))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (is_attrib(sname, PART_FIDUCIAL))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (!STRCMPI(stype, "MECHANICAL"))
						block->setBlockType(BLOCKTYPE_MECHCOMPONENT);
					else     
						block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
				}
            data->getInsert()->setInsertType(default_inserttype(block->getBlockType()));

				if (is_attrib(sname, PART_TOOL))
					block->setBlockType(blockTypeTooling);
            else if (is_attrib(sname, PART_FIDUCIALTOP))
               data->getInsert()->setPlacedBottom(false);
            else if (is_attrib(sname, PART_FIDUCIALBOT))
               data->getInsert()->setPlacedBottom(true);

            if (panelmode)
            {
               if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  messageFilter->formatMessage(" A PACKAGE COMPONENT [%s] is placed on Panel -> Changed to MECHANICAL in [%s] at %ld", 
                     refdes, cur_filename, line_cnt);
                  display_error++;
                  data->getInsert()->setInsertType(insertTypeMechanicalComponent);
               }
            }

            CString devicename = "";
            if (i = get_label(VAL_COMP_DEVICE_TYPE))
               devicename = recordArray[i-1]->tok;
   
            if (!STRICMP(devicename.Right(1), "*") || strlen(devicename) == 0)   // DEV* <-- not assigned
               devicename = sname;

            if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
               devicename = "";

            // here do type list
            // here add attributes
            if (strlen(devicename))
            {
               TypeStruct *type = AddType(currentPCBFile, devicename);
               if (type->getBlockNumber() == -1)
               {
                  type->setBlockNumber( data->getInsert()->getBlockNumber());
               }
               else if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
               {
                  BlockStruct *insertedBlock = globalDoc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
                  BlockStruct *already_assigned = globalDoc->Find_Block_by_Num(type->getBlockNumber());
                  messageFilter->formatMessage("Try to assign Package [%s] to Device [%s] -> already a different Package [%s] assigned!",
                        insertedBlock->getName(), type->getName(), already_assigned->getName());
                  display_error++;

                  CString newdevice;
                  newdevice.Format("%s_%s", type->getName(), insertedBlock->getName());
                  type = AddType(currentPCBFile, newdevice);

                  if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
                     type->setBlockNumber( data->getInsert()->getBlockNumber());
               }
               globalDoc->SetAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord(ATT_TYPELISTLINK, 0), type->getName(), attributeUpdateOverwrite, NULL);

               for (int i=0; i<commandArrayCount; i++)
               {
                  if (is_attrib(commandArray[i]->tok, TYPE_ATT) && strlen(recordArray[i]->tok))
                  {
                     if (!STRCMPI("COMP_DEVICE_TYPE", commandArray[i]->tok))
                        continue;
                     do_attrib(&type->getAttributesRef(), get_attrmap(commandArray[i]->tok), valueTypeString, recordArray[i]->tok, 0.0, 0);
                  }
               }
            }
            cur_reference = data;

            // add refdes
            if (strlen(refdes))
               globalDoc->SetAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord(ATT_REFNAME, 0), valueTypeString, refdes.GetBuffer(0), SA_RETURN, NULL);

            strcpy(cur_placeblock, name);

				// do .COMPATTR
				for (int i=0; i<commandArrayCount; i++)
				{
					Command *command = commandArray[i];
					Record *record = recordArray[i];

					if (is_attrib(command->tok, COMP_ATT) && strlen(record->tok))
						globalDoc->SetUnknownAttrib(&cur_reference->getAttributesRef(), get_attrmap(command->tok), record->tok, attributeUpdateOverwrite, NULL);
	            
				}

				// do .TEXTCOMPATTR
				char attr[80];
				if (is_textcompattr(lay, class_name, attr))
				{
					do_attgraphic(&cur_reference->getAttributesRef(), sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, 
							graphic_class, FALSE, lay, TRUE, attr);
				}

				// here check for refdes
            // dts0100429248 - Deactivated, moved below.
            // When this processing is done here, it is only done the first time a given component refname is encountered.
            // If that wasn't in a REF NAME text record, then this spot will be skipped, and for this component the
            // reader will never pass through this code again, hence, visible refname gets lost.
            // PROBABLY the textcompattr processing above has same bug and needs same move, but this is not
            // part of this case, so no specific data supplied to test it, so it is being left as-is.
				//if (is_primrefdes(lay, class_name))
				//{
				//	for (int i=0; i<commandArrayCount; i++)
				//	{
				//		if (!STRCMPI("GRAPHIC_DATA_NAME", commandArray[i]->tok) && !STRCMPI(recordArray[i]->tok, "TEXT"))
				//		{
				//			graphic_class = REFDES_CLASS;
				//			do_attgraphic(&cur_reference->getAttributesRef(), sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, 
				//					graphic_class,FALSE, lay, TRUE, "");
				//		}
				//	}
				//}
	            
				if (is_attrib(class_name, CLASS_2_COMPATT))
				{
					do_attgraphic(&cur_reference->getAttributesRef(), sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, 
							graphic_class, FALSE, lay, TRUE, class_name);
				}
			}

         // here check for refdes
         // dts0100429248 - code block moved here, otherwise was only activated if REF DES data was first line in symbol definition.
         // dts0100792436 - added check for cur_reference != null, can get here with cur_reference == null.
         if (cur_reference != NULL && is_primrefdes(lay, class_name))
         {
            for (int i=0; i<commandArrayCount; i++)
            {
               if (!STRCMPI("GRAPHIC_DATA_NAME", commandArray[i]->tok) && !STRCMPI(recordArray[i]->tok, "TEXT"))
               {
                  graphic_class = REFDES_CLASS;
                  do_attgraphic(&cur_reference->getAttributesRef(), sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, 
                     graphic_class, FALSE, lay, TRUE, "");
               }
            }
         }

      }
   }
   else if (!strcmp(stype, "FORMAT"))
   {
      if (!(i = get_label(VAL_SYM_X)))
      {
         lab_error("SYM_X expected");
         return 0;
      }
      double sym_x = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i = get_label(VAL_SYM_Y)))
      {
         lab_error("SYM_Y expected");
         return 0;
      }
      double sym_y = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i = get_label(VAL_SYM_ROTATE)))
      {
         lab_error("SYM_ROTATE expected");
         return 0;
      }
      double sym_rot = atof(recordArray[i-1]->tok);

      if (!(i = get_label(VAL_SYM_MIRROR)))
      {
         lab_error("SYM_MIRROR expected");
         return 0;
      }
      int sym_mir = !STRICMP(recordArray[i-1]->tok, "YES");

      // always create if it is a new instance
      CString name, bname;
      name.Format("%s_%2.3lf_%2.3lf", sname, sym_x, sym_y);
         
      if (explode_format)
         bname = name;
      else
         bname = sname;

      if (master_block(bname, name))
      {
         BlockStruct *block = Graph_Block_On(GBO_APPEND, bname,currentPCBFile->getFileNumber(), 0);
         block->setBlockType(BLOCKTYPE_GENERICCOMPONENT);

         if (!panelmode)
         {
            if (is_primboardoutline(lay, class_name))
               graphic_class = graphicClassBoardOutline;
         }
         else
         {
            if (is_primpaneloutline(lay, class_name))
               graphic_class = graphicClassPanelOutline;
         }

         do_graphic("", sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, graphic_class, FALSE, NULL);
         Graph_Block_Off();
      }

      // only place the first time if it is a new block
      if (STRCMPI(oldcur_block, name))
      {
         DataStruct *data = Graph_Block_Reference(bname, name,currentPCBFile->getFileNumber(), sym_x, sym_y, DegToRad(sym_rot), sym_mir , 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypeGenericComponent);  // GENERIC insert
         strcpy(oldcur_block, name);

         
         // here add attributes
         for (int i=0; i<commandArrayCount; i++)
         {
            if ((is_attrib(commandArray[i]->tok, COMP_ATT) || is_attrib(commandArray[i]->tok, PANEL_ATT)) && strlen(recordArray[i]->tok))
            {
               BlockStruct *block = globalDoc->getBlockAt(data->getInsert()->getBlockNumber());
               do_attrib(&block->getAttributesRef(), get_attrmap(commandArray[i]->tok), valueTypeString, recordArray[i]->tok, 0.0, 0);
            }
         }
         set_pcb();
      }
   }
   else if (!strcmp(stype, "DRAFTING"))
   {
      if (!(i=get_label(VAL_SYM_X)))
      {
         lab_error("SYM_X expected");
         return 0;
      }
      double sym_x = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i=get_label(VAL_SYM_Y)))
      {
         lab_error("SYM_Y expected");
         return 0;
      }
      double sym_y = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i=get_label(VAL_SYM_ROTATE)))
      {
         lab_error("SYM_ROTATE expected");
         return 0;
      }
      double sym_rot = atof(recordArray[i-1]->tok);

      if (!(i=get_label(VAL_SYM_MIRROR)))
      {
         lab_error("SYM_MIRROR expected");
         return 0;
      }
      int sym_mir = !STRICMP(recordArray[i-1]->tok, "YES");

      // always create if it is a new instance
      CString name;
      name.Format("%s_%ld", sname, line_cnt);
         
      CString bname = name;

      if (master_block(bname, name))
      {
         BlockStruct *block = Graph_Block_On(GBO_APPEND, bname,currentPCBFile->getFileNumber(), 0);
         block->setBlockType(BLOCKTYPE_GENERICCOMPONENT);

         if (!panelmode)
         {
            if (is_primboardoutline(lay, class_name))
               graphic_class = graphicClassBoardOutline;
         }
         else
         {
            if (is_primpaneloutline(lay, class_name))
               graphic_class = graphicClassPanelOutline;
         }

         do_graphic("", sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, graphic_class, FALSE, NULL);
         Graph_Block_Off();
      }

      // only place the first time if it is a new block
      if (STRCMPI(oldcur_block, name))
      {
         DataStruct *data = Graph_Block_Reference(bname, NULL,currentPCBFile->getFileNumber(), sym_x, sym_y, DegToRad(sym_rot), sym_mir, 1.0, -1, TRUE);
         data->getInsert()->setInsertType(insertTypeGenericComponent);  // GENERIC insert
         strcpy(oldcur_block, name);

         // here add attributes
         for (int i=0; i<commandArrayCount; i++)
         {
            if ((is_attrib(commandArray[i]->tok, COMP_ATT) || is_attrib(commandArray[i]->tok, PANEL_ATT)) && strlen(recordArray[i]->tok))
            {
               BlockStruct *block = globalDoc->getBlockAt(data->getInsert()->getBlockNumber());
               do_attrib(&block->getAttributesRef(), get_attrmap(commandArray[i]->tok), valueTypeString, recordArray[i]->tok, 0.0, 0);
            }
         }
         set_pcb();
      }
   }
   else
   {
      messageFilter->formatMessage("Unknown SYMBOL [%s] in [%s] at %ld", stype, cur_filename, line_cnt);
      display_error++;
   }

   return 1;
}

/******************************************************************************
* do_classic_symfile
*/
static int do_classic_symfile()
{
   int i = 0;
   if (!(i = get_label(VAL_SYM_NAME)))
   {
      lab_error("SYM_NAME expected");
      return 0;
   }
   CString sname = recordArray[i-1]->tok;
   
   if (sname.CompareNoCase("panel") == 0)
   {
      // this are restricted names
      messageFilter->formatMessage("Restricted Symbol name [panel] used -> changed to [!panel]");
      sname = "!panel";
   }
   if (sname.CompareNoCase("board") == 0)
   {
      // this are restricted names
      messageFilter->formatMessage("Restricted Symbol name [board] used -> changed to [!board]");
      sname = "!board";
   }

   if (is_attrib(sname,PANEL_SYMBOL))  
      set_panel();

   // record_tag
   if (!(i = get_label(VAL_RECORD_TAG)))
   {
      lab_error("RECORD_TAG expected");
      return 0;
   }

   char rtag[80];
   strcpy(rtag, recordArray[i-1]->tok);

   char *lp;
   int majj = 0;
   int minj = 0;
   int littlej = 0;
   if ((lp = strtok(rtag, " \t")) != NULL)
   {
      majj = atoi(lp);
      if ((lp = strtok(NULL, " \t")) != NULL)
      {
         minj = atoi(lp);
         if ((lp = strtok(NULL, " \t")) != NULL)
            littlej = atoi(lp);
      }
   }

   lastmaj = majj;

   if (!(i=get_label(VAL_SUBCLASS)))
   {
      lab_error("SUBCLASS expected");
      return 0;
   }
   CString subclass = recordArray[i-1]->tok;
   CString lay = recordArray[i-1]->tok;
   cur_layer_subclass = subclass;

   if (!(i=get_label(VAL_CLASS)))
   {
      lab_error("CLASS expected");
      return 0;
   }
   char class_name[80];
   strcpy(class_name,recordArray[i-1]->tok);
   cur_layer_class = class_name;

   if (!suppress_class)
   {
      //lay gets prefix, but not if it is etch.
      if (STRCMPI(class_name, "ETCH"))
      {
         CString  tmp;
         tmp = class_name;
         tmp += ",";
         tmp += lay;
         lay = tmp;
      }
   }

   int w1 = update_layer(lay);
   layers[w1].used = TRUE;
   cur_layer = Graph_Level(lay,"", 0);

   // get symbol type
   int graphic_class = 0;                    
   if (is_attrib(sname, EXPLODE_SYMBOL) || strlen(sname) == 0)
   {
      // trace and other graphic
      strcpy(cur_block, "");
      if (!(i = get_label(VAL_NET_NAME)))
      {
         lab_error("NET_NAME expected");
         return 0;
      }
      CString netname = recordArray[i-1]->tok;

      if ((graphic_class = get_graphic_class(class_name, netname.IsEmpty())) == 0)
         graphic_class = get_graphic_subclass(subclass);
      
      if (is_primboardoutline(lay, class_name))
      {
         graphic_class = graphicClassBoardOutline;
         set_pcb();
      }
      else
      {
         if (is_primpaneloutline(lay, class_name))
         {
            graphic_class = graphicClassPanelOutline;
            set_panel();
         }
         else if (is_primarray(class_name, subclass, PANEL_GRAPHIC))
         {
            set_panel();
         }
      }
      cur_layer = Graph_Level(lay, "", 0);
      do_graphic(netname, 0, 0, 0, 0, majj, minj, littlej, graphic_class, TRUE, NULL);
   }
   else
   {
      if (!(i=get_label(VAL_REFDES)))
      {
         lab_error("REFDES expected");
         return 0;
      }
      CString refdes = recordArray[i-1]->tok;

      if (!(i=get_label(VAL_SYM_X)))
      {
         lab_error("SYM_X expected");
         return 0;
      }
      double sym_x = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i=get_label(VAL_SYM_Y)))
      {
         lab_error("SYM_Y expected");
         return 0;
      }
      double sym_y = atof(recordArray[i-1]->tok) * scale_factor;

      if (!(i=get_label(VAL_SYM_ROTATE)))
      {
         lab_error("SYM_ROTATE expected");
         return 0;
      }
      double sym_rot = atof(recordArray[i-1]->tok);

      if (!(i=get_label(VAL_SYM_MIRROR)))
      {
         lab_error("SYM_MIRROR expected");
         return 0;
      }
      int sym_mir = !STRICMP(recordArray[i-1]->tok, "YES");

      // if a refname ends in * than it is an unnamed component and needs to get 
      // the sym_koos added to the name
      if (!STRICMP(refdes.Right(1), "*") || (strlen(refdes) == 0))
         refdes.Format("%s_%2.3lf_%2.3lf_%1.0lf_%d", sname, sym_x, sym_y, sym_rot, sym_mir);

      // always create if it is a new instance
      CString name;
      name.Format("%s_%s", sname, refdes);

      if (is_attrib(sname, BOARD_SYMBOL))   // if this is a step and repeat
      {
         if (STRCMPI(cur_placeblock, name))
         {
            // the sym_x, sym_y placement is not the placement of the PCB, but the
            // placemement of the steprep symbol. The placement of the pcb has
            // the offset of the steprep symbols. The first steprep symbol is
            // the symbol lowerleft of the primaryboard outline.
            ALLEGROStepRepeat *p = new ALLEGROStepRepeat; 
            stepreparray.SetAtGrow(stepreparraycnt++, p);
            p->x = sym_x;
            p->y = sym_y;
            p->rot = sym_rot;
            p->mir = sym_mir;
            strcpy(cur_placeblock, name);
         }
      }
      else
		{
			// if DIP14_IC1 is now DIP14_IC2 kill curblock
			// it still will find it in the masterlist
			if (strcmp(name, cur_placeblock))
				strcpy(cur_block, "");

			if (master_block(sname, name))
			{
				BlockStruct *block;
				block = Graph_Block_On(GBO_APPEND, sname,currentPCBFile->getFileNumber(), 0);

				
				if (block->getBlockType() == BLOCKTYPE_UNKNOWN)
				{
					// Block is new so check for block type
					if (is_attrib(sname, PART_TOOL))
						block->setBlockType(blockTypeTooling);
					else if (is_attrib(sname, PART_FIDUCIALTOP))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (is_attrib(sname, PART_FIDUCIALBOT))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (is_attrib(sname, PART_FIDUCIAL))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (is_attrib(sname, MECHANICAL_SYMBOL))
						// if mechanical symbol
						block->setBlockType(BLOCKTYPE_MECHCOMPONENT);
					else     
						block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
				}


				if (is_attrib(sname, SMD_ATT))
					globalDoc->SetUnknownAttrib(&block->getAttributesRef(), ATT_SMDSHAPE, "", attributeUpdateOverwrite, NULL); 

				// here also check for other classes, which should not be text, but attribute
				if (strcmp(class_name, "REF DES") && !is_attrib(class_name,CLASS_2_COMPATT))  
				{
					if (sym_mir)
					{                                           
						int w1 = update_layer(lay);
						lay = layers[layers[w1].mirror].name;
						cur_layer = Graph_Level(lay, "", 0);
					}
					else
						cur_layer = Graph_Level(lay, "", 0);

					if ((graphic_class = get_graphic_class(class_name, TRUE)) == 0)
						graphic_class = get_graphic_subclass(subclass);

					if (is_primcompoutline(lay, class_name))
						graphic_class = graphicClassComponentOutline;

					if (!panelmode)
					{
						if (is_primboardoutline(lay, class_name))
							graphic_class = graphicClassBoardOutline;
					}
					else
					{
						if (is_primpaneloutline(lay, class_name))
							graphic_class = graphicClassPanelOutline;
					}

					if (is_primcompheight(lay, class_name))
					{
						for (int i=0; i<commandArrayCount; i++)
						{
							if (!STRCMPI("GRAPHIC_DATA_NAME",commandArray[i]->tok) && !STRCMPI(recordArray[i]->tok, "TEXT"))
							{
								graphic_class = COMPHEIGHT_CLASS;
								break;
							}
						}
					}
					// board geometry should only be used in Board outline and graphic definitions, not in
					// components.
					if (graphic_class == GR_CLASS_BOARD_GEOM)
						graphic_class = 0;

					do_graphic("", sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, graphic_class, FALSE, &block->getAttributesRef());
				}

				Graph_Block_Off();
			}
	            
			// only place the first time if it is a new block
			if (STRCMPI(cur_placeblock,name))
			{
				int pm = panelmode;
				char *r = refdes.GetBuffer(0);

				DataStruct *data = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), refdes);
				if (data == NULL)
					data = Graph_Block_Reference(sname, r ,currentPCBFile->getFileNumber(), sym_x, sym_y, DegToRad(sym_rot), sym_mir, 1.0, -1, TRUE);

				if (is_attrib(sname, PART_FIDUCIALTOP))
					data->getInsert()->setInsertType(insertTypeFiducial);
				else if (is_attrib(sname, PART_FIDUCIALBOT))
					data->getInsert()->setInsertType(insertTypeFiducial);
				else if (is_attrib(sname, PART_FIDUCIAL))
					data->getInsert()->setInsertType(insertTypeFiducial);
				else if (is_attrib(sname, MECHANICAL_SYMBOL))  
					data->getInsert()->setInsertType(insertTypeMechanicalComponent);
				else     
					data->getInsert()->setInsertType(insertTypePcbComponent);

				if (panelmode)
				{
					if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
					{
						messageFilter->formatMessage(" A PACKAGE COMPONENT [%s] is placed on Panel -> Changed to MECHANICAL", r);
						display_error++;
						data->getInsert()->setInsertType(insertTypeMechanicalComponent);
					}
				}

				CString devicename = "";
				if (i = get_label(VAL_COMP_DEVICE_TYPE))
					devicename = recordArray[i-1]->tok;

				// here do type list
				// here add attributes
				if (strlen(devicename))
				{
					TypeStruct *type = AddType(currentPCBFile, recordArray[i-1]->tok);
					if (type->getBlockNumber() == -1)
					{
						type->setBlockNumber( data->getInsert()->getBlockNumber());
					}
					else if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
					{
						BlockStruct *insertedBlock = globalDoc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
						BlockStruct *already_assigned = globalDoc->Find_Block_by_Num(type->getBlockNumber());
						messageFilter->formatMessage("Try to assign Package [%s] to Device [%s] -> already a different Package [%s] assigned!",
								insertedBlock->getName(), type->getName(), already_assigned->getName());
						display_error++;

						CString newdevice;
						newdevice.Format("%s_%s", type->getName(), insertedBlock->getName());
						type = AddType(currentPCBFile, newdevice);

						if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
							type->setBlockNumber( data->getInsert()->getBlockNumber());
					}
					globalDoc->SetAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord(ATT_TYPELISTLINK, 0), type->getName(), attributeUpdateOverwrite, NULL);

					for (int i=0; i<commandArrayCount; i++)
					{
						if (is_attrib(commandArray[i]->tok, TYPE_ATT) && strlen(recordArray[i]->tok))
						{
							if (!STRCMPI("COMP_DEVICE_TYPE", commandArray[i]->tok))
								continue;
							do_attrib(&type->getAttributesRef(),get_attrmap(commandArray[i]->tok), valueTypeString, recordArray[i]->tok, 0.0, 0);
						}
					}
				}  // if device_type found

				cur_reference = data;

				// add refdes
				if (strlen(refdes))
					globalDoc->SetAttrib(&data->getAttributesRef(),globalDoc->IsKeyWord(ATT_REFNAME, 0), valueTypeString, refdes.GetBuffer(0), SA_RETURN, NULL);

				strcpy(cur_placeblock, name);

				// here add attributes
				for (int i=0; i<commandArrayCount; i++)
				{
					if (is_attrib(commandArray[i]->tok, COMP_ATT) && strlen(recordArray[i]->tok))
						do_attrib(&cur_reference->getAttributesRef(),get_attrmap(commandArray[i]->tok), valueTypeString, recordArray[i]->tok, 0.0, 0);      
				}

				// do .TEXTCOMPATTR

				char attr[80];
				if (is_textcompattr(lay, class_name, attr))
				{
					do_attgraphic(&cur_reference->getAttributesRef(), sym_x, sym_y, sym_rot, sym_mir, majj, minj,littlej, 
							graphic_class, FALSE, lay, TRUE, attr);
				}

				// here check for refdes
				if (is_primrefdes(lay, class_name))
				{
					for (int i=0; i<commandArrayCount; i++)
					{
						if (!STRCMPI("GRAPHIC_DATA_NAME", commandArray[i]->tok) && !STRCMPI(recordArray[i]->tok, "TEXT"))
						{
							graphic_class = REFDES_CLASS;
							do_attgraphic(&cur_reference->getAttributesRef(), sym_x, sym_y, sym_rot, sym_mir, majj, minj, littlej, 
									graphic_class,FALSE, lay, TRUE,"");
						}
					}
				}
		         
				if (is_attrib(class_name, CLASS_2_COMPATT))
				{
					do_attgraphic(&cur_reference->getAttributesRef(), sym_x, sym_y, sym_rot, sym_mir, majj, minj,littlej, 
							graphic_class, FALSE, lay, TRUE, class_name);
				}
			}
		}
   }

   return 1;
}

/******************************************************************************
* do_netfile
*/
static int do_netfile()
{
#if CamCadMajorMinorVersion > 406  //  > 4.6
   CCamCadData& camCadData = globalDoc->getCamCadData();
#endif

   CString  systemName;
   char     class_name[80];
   int      i;

	CString netname = recordArray[0]->tok;

   // component, shape graphic
   // get symname+symx+symy
   if (!(i=get_label(VAL_SYM_NAME)))
   {
      lab_error("SYM_NAME expected");
      return 0;
   }
   systemName = recordArray[i-1]->tok;

   if (systemName.CompareNoCase("panel") == 0)
   {
      // this are restricted names
      messageFilter->formatMessage("Restricted Symbol name [panel] used -> changed to [!panel]");
      systemName = "!panel";
   }
   if (systemName.CompareNoCase("board") == 0)
   {
      // this are restricted names
      messageFilter->formatMessage("Restricted Symbol name [board] used -> changed to [!board]");
      systemName = "!board";
   }

   // this is only for checking
   if (!(i=get_label(VAL_PIN_NUMBER_SORT)))
   {
      lab_error("PIN_NUMBER_SORT expected"); // continue reading the file !
      //return 0;
   }

   // here check if via or package or mechanical
   // this is only for checking
   if (!(i=get_label(VAL_CLASS)))
   {
      lab_error("CLASS expected");
      return 0;
   }
   strcpy(class_name,recordArray[i-1]->tok);

   if (!strcmp(class_name,"PIN"))
   {
      CString  stype;
      if (!(i=get_label(VAL_SYM_TYPE)))
      {
         lab_error("SYM_TYPE expected");
         return 0;
      }     
      stype = recordArray[i-1]->tok;

      if (is_attrib(systemName,MECHANICAL_SYMBOL))
         stype = "MECHANICAL";
         
		if (is_attrib(systemName, PANEL_SYMBOL)) 
			set_panel();

      if (!strcmp(stype,"PACKAGE") || !strcmp(stype,"MECHANICAL"))
      {
         char     pname[80]; // padstack name
         CString  refdes;
         CString  padstackname, pinname, pinnr;
         double   sym_x,sym_y,sym_rot;
         int      sym_mir;
         double   pin_x, pin_y, pin_rot;
         double   orig_x, orig_y;

         cur_pcbcomponent = TRUE;
         
         if (!(i=get_label(VAL_REFDES)))
         {
            lab_error("REFDES expected");
            return 0;
         }
         refdes = recordArray[i-1]->tok;

         if (!(i=get_label(VAL_SYM_X)))
         {
            lab_error("SYM_X expected");
            return 0;
         }
         sym_x = atof(recordArray[i-1]->tok) * scale_factor;
         if (!(i=get_label(VAL_SYM_Y)))
         {
            lab_error("SYM_Y expected");
            return 0;
         }
         sym_y = atof(recordArray[i-1]->tok) * scale_factor;
         if (!(i=get_label(VAL_SYM_ROTATE)))
         {
            lab_error("SYM_ROTATE expected");
            return 0;
         }
         sym_rot = atof(recordArray[i-1]->tok);

         if (!(i=get_label(VAL_SYM_MIRROR)))
         {
            lab_error("SYM_MIRROR expected");
            return 0;
         }
         sym_mir = !STRICMP(recordArray[i-1]->tok,"YES");

         // if a refname ends in * than it is an unnamed component and needs to get 
         // the sym_koos added to the name
         if (!STRICMP(refdes.Right(1), "*"))
         {
            CString t;
            t.Format("%s_%2.3lf_%2.3lf_%1.0lf_%d", refdes, sym_x, sym_y, sym_rot, sym_mir);
            refdes = t;
            cur_pcbcomponent = FALSE;
         }
			else if (strlen(refdes) == 0)
			{
				CString t;
				t.Format("%s_%2.3lf_%2.3lf_%1.0lf_%d", systemName, sym_x, sym_y, sym_rot, sym_mir);
				refdes = t;
            cur_pcbcomponent = FALSE;
			}

         if (!(i=get_label(VAL_PAD_STACK_NAME)))
         {
            lab_error("PAD_STACK_NAME expected");
            return 0;
         }
         padstackname = recordArray[i-1]->tok;
         sprintf(pname,"PADSTACK_%s",recordArray[i-1]->tok);

         if (!(i=get_label(VAL_PIN_NUMBER)))
         {
            lab_error("PIN_NUMBER expected");
            return 0;
         }
         pinnr = recordArray[i-1]->tok;

         // pin rotation is relative to defintion and not to placement.
			pin_rot = 0;
         if (!(i=get_label(VAL_PIN_ROTATION)))
            lab_error("PIN_ROTATION expected");
			else
				pin_rot = atof(recordArray[i-1]->tok);

         // pin rotation is relative to defintion and not to placement.
         int pin_edited = FALSE;

         if ((i=get_label(VAL_PIN_EDITED)))
         {
				// To indicate the file has pin edit field
            hasPinEdited = TRUE;

				CString w = recordArray[i-1]->tok;
            if (w.CompareNoCase("Yes") == 0)
               pin_edited = TRUE;
         }

         if (!(i=get_label(VAL_PIN_NAME)))
         {
            lab_error("PIN_NAME expected");
            return 0;
         }
         pinname = recordArray[i-1]->tok;

         if (!(i=get_label(VAL_PIN_X)))
         {
            lab_error("SYM_X expected");
            return 0;
         }
         orig_x = pin_x = atof(recordArray[i-1]->tok) * scale_factor;
         if (!(i=get_label(VAL_PIN_Y)))
         {
            lab_error("PIN_Y expected");
            return 0;
         }
         orig_y = pin_y = atof(recordArray[i-1]->tok) * scale_factor;
         // normalize
         pin_x = pin_x - sym_x;
         pin_y = pin_y - sym_y;

         if (sym_rot)
         {
            double x = pin_x, y=pin_y;
            Rotate(x,y, 360-sym_rot,&pin_x, &pin_y);
         }
         if (sym_mir)
         {
            // change layer
            pin_x = -pin_x;
         }

         if (strlen(refdes) == 0)
         {
            // this can happen if a single pin or fiducial is placed.
            messageFilter->formatMessage("Pad/Pin without Refdes in [%s] at %ld -> skipped", 
                  cur_filename, line_cnt);
            display_error++;
            return 0;
         }

         // pinnames do not need to be unique in Cadence.
         if (!usepinname)
            pinname = pinnr;

         // if there is no pinname, than take pinnumber as pinname.
         if (strlen(pinname) == 0)
            pinname = pinnr;

         // a pin with a netname must have a name
         if (strlen(pinname) == 0 && strlen(netname))
         {
            // unknown extract
            messageFilter->formatMessage("Pinname String expected in [%s] at %ld", cur_filename, line_cnt);
            display_error++;
            return -1;
         }

			// Get unique refdes for those refdes that has the same but difference insert location
			CString uniqueRefdes;
			allegroRefdesMap.GetUniqueRefdesByCoordinate(refdes, sym_x, sym_y, uniqueRefdes);
			refdes = uniqueRefdes;

         if (refdes.Compare(oldrefdes))   // unequal to old.
         {
            if (cur_pcbcomponent)
            {
               // check here if it is a PCB COMPONENT
               DataStruct *data = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), refdes);
 
               if (!data || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
               {
                  cur_pcbcomponent = FALSE;
                  messageFilter->formatMessage("Netlist Component [%s] not found as a PCB COMPONENT -> skipped in [%s] at %ld",
                        refdes, cur_filename, line_cnt);
                  display_error++;
               }
               else
                  cur_pcbcomponent = TRUE;
            }
         }
            
         oldrefdes = refdes;
         int pin_is_electrical = TRUE;

         if (!strlen(pinname))
            pin_is_electrical = FALSE;
         else
         if (!strcmp(stype,"MECHANICAL"))
            pin_is_electrical = FALSE;
         else
         if (is_attrib(padstackname, PADSTACK_TOOL))
            pin_is_electrical = FALSE;
         else
         if (is_attrib(padstackname, PADSTACK_FIDUCIAL))
            pin_is_electrical = FALSE;
         else
         if (is_attrib(padstackname, PADSTACK_FIDUCIALTOP))
            pin_is_electrical = FALSE;
         else
         if (is_attrib(padstackname, PADSTACK_FIDUCIALBOT))
            pin_is_electrical = FALSE;

         if (cur_pcbcomponent && pin_is_electrical)
         {
            // make netlist
            CompPinStruct *pin;
            if (strlen(netname) == 0)
            {
               NetStruct *net = add_net(currentPCBFile, NET_UNUSED_PINS);
               net->setFlagBits(NETFLAG_UNUSEDNET);
               pin = add_comppin(currentPCBFile, net, refdes, pinname);
            }
            else
            {
               NetStruct *net = add_net(currentPCBFile, netname);
               pin = add_comppin(currentPCBFile, net, refdes ,pinname);
            }

            // here now put PINLOC
            if (pin)
            {
               pin->setPinCoordinatesComplete(true);
               pin->setOrigin(orig_x,orig_y);
               pin->setRotationDegrees(sym_rot + pin_rot);
               pin->setMirror(sym_mir);
               BlockStruct *b = Graph_Block_On(GBO_APPEND, pname,currentPCBFile->getFileNumber(), 0);
               Graph_Block_Off();
               pin->setPadstackBlockNumber( b->getBlockNumber());

               if (i=get_label(VAL_TEST_POINT))
               {
                  CString testaccess = recordArray[i-1]->tok;

                  // values are BOTTOM, TOP, TOP_MANUAL, BOTTOM_MANUAL
                  if (!testaccess.CompareNoCase("BOTTOM") || !testaccess.CompareNoCase("BOTTOM_MANUAL"))
                     globalDoc->SetAttrib(&pin->getAttributesRef(), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString, "BOTTOM", attributeUpdateOverwrite, NULL);
                  else if (!testaccess.CompareNoCase("TOP") || !testaccess.CompareNoCase("TOP_MANUAL"))
                     globalDoc->SetAttrib(&pin->getAttributesRef(), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString, "TOP",  attributeUpdateOverwrite, NULL);
               }
            }
         } 

         // always create if it is a new instance
         CString instanceSpecificName;
         instanceSpecificName.Format("%s_%s", systemName, refdes);

			// If pin has no name by the time it gets here then create a unique name by location
			if (pinname.IsEmpty())
				pinname.Format("Pin_%2.3lf_%2.3lf", pin_x, pin_y);

         if (strcmp(cur_placeblock, instanceSpecificName))
            strcpy(cur_block, "");

         CString geomName = systemName; // the symbol name can chage if a new geometry is generated.

         if (pin_edited)
         {
				//if pin edited and it is a master
            if (InMasterList(systemName, refdes))
					setMasterPinedited(systemName, refdes); //set the master pin eidted flag to true

				int comparrayptr = update_comparray(refdes, systemName, pin_edited, sym_x, sym_y, sym_rot, sym_mir);

            // need to make a new block, like in Mentor on PIN move
            geomName = instanceSpecificName;

            // if this new block already exists, I do not need top copy it.
				BlockStruct *instanceBlock = Graph_Block_Exists(globalDoc, instanceSpecificName, -1);
            if (instanceBlock == NULL)
            {
               BlockStruct *b = Graph_Block_Exists(globalDoc, systemName, -1);
               if (b)
               {
                  instanceBlock = Graph_Block_On(GBO_APPEND, instanceSpecificName,currentPCBFile->getFileNumber(), 0);
                  Graph_Block_Copy(b, 0, 0, 0, 0, 1, -1, TRUE);
                  Graph_Block_Off();

                  instanceBlock->setBlockType(BLOCKTYPE_PCBCOMPONENT);
                  globalDoc->SetUnknownAttrib(&instanceBlock->getAttributesRef(),ATT_DERIVED, systemName, attributeUpdateOverwrite, NULL);   

                  // also link the refdes to a new comp
                  DataStruct *data = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), refdes);

                  // Add the instance specific geometry created for pin edit to the map
                  UpdatePinEditGeomArray(systemName, instanceBlock);

                  if (data)   
                  {
                     data->getInsert()->setBlockNumber(instanceBlock->getBlockNumber());
                     comparray[comparrayptr]->usedsymname = instanceSpecificName;
                  
                     if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT && data->getAttributesRef())
                     {
                        Attrib* a = NULL;
                        WORD keyword = globalDoc->IsKeyWord(ATT_TYPELISTLINK, 0);

                        // Loop for the device type attribe from the insert
                        data->getAttributesRef()->Lookup(keyword, a);

                        if (a)
                        {
                           // Get the device structure
#if CamCadMajorMinorVersion > 406  //  > 4.6
                           CString deviceName = a->getStringValue();
#else
                           CString deviceName = globalDoc->ValueArray[a->getStringValueIndex()];
#endif
                           TypeStruct *deviceType = AddType(currentPCBFile, deviceName);

                           if (deviceType->getBlockNumber() == -1)
                           {
                              // device is not tied to any geometry so tie to this insert's geometry
                              deviceType->setBlockNumber( data->getInsert()->getBlockNumber());
                           }
                           else if (deviceType->getBlockNumber() != data->getInsert()->getBlockNumber())
                           {
                              // Device is already tied to another geometry so creat a new
                              // Tie the new device to this insert's geometry
                              CString newDeviceName;
                              newDeviceName.Format("%s_%s", deviceName, refdes);
                              TypeStruct *newDeviceType = AddType(currentPCBFile, newDeviceName);

                              // Copy all the attribe from the device to the new device
                              globalDoc->CopyAttribs(&newDeviceType->getAttributesRef(), deviceType->getAttributesRef());
                              newDeviceType->setBlockNumber( data->getInsert()->getBlockNumber());

                              // Set the "$$DEVICE$$" and "COMP_DEVICE_TYPE" attribute to the new device name
                              globalDoc->SetAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord(ATT_TYPELISTLINK, 0), newDeviceType->getName(), attributeUpdateOverwrite, NULL);
                              globalDoc->SetAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord("COMP_DEVICE_TYPE", 0), newDeviceType->getName(), attributeUpdateOverwrite, NULL);
                           }
                        }
                     }
                  }
               }
					else
					{
						instanceBlock = Graph_Block_On(GBO_APPEND, instanceSpecificName,currentPCBFile->getFileNumber(), 0);
                  Graph_Block_Off();

						// Block is new so check for block type
						if (is_attrib(systemName, PART_TOOL))
							instanceBlock->setBlockType(blockTypeTooling);
						else if (is_attrib(systemName, PART_FIDUCIALTOP))
							instanceBlock->setBlockType(BLOCKTYPE_FIDUCIAL);
						else if (is_attrib(systemName, PART_FIDUCIALBOT))
							instanceBlock->setBlockType(BLOCKTYPE_FIDUCIAL);
						else if (is_attrib(systemName, PART_FIDUCIAL))
							instanceBlock->setBlockType(BLOCKTYPE_FIDUCIAL);
						else if (!STRCMPI(stype, "MECHANICAL"))
							instanceBlock->setBlockType(BLOCKTYPE_MECHCOMPONENT);
						else     
							instanceBlock->setBlockType(BLOCKTYPE_PCBCOMPONENT);
					}
				}

				if (instanceBlock)
				{

					// new block already exist, so check to see if the same pin is already exist
					// if it is then remove the old one and add the new one
					DataStruct *pinData = datastruct_from_refdes(globalDoc, instanceBlock, pinname);
					if (pinData)
						RemoveOneEntityFromDataList(globalDoc, &instanceBlock->getDataList(), pinData, NULL); 

					Graph_Block_On(GBO_APPEND, instanceSpecificName,currentPCBFile->getFileNumber(), 0);
					pinData = Graph_Block_Reference(pname, pinname,currentPCBFile->getFileNumber(), pin_x, pin_y, DegToRad(pin_rot), 0 , 1.0, -1, TRUE);
					Graph_Block_Off();

					if (pin_is_electrical)
					{
						pinData->getInsert()->setInsertType(insertTypePin);
					}
					else
					{
						if (is_attrib(padstackname, PADSTACK_TOOL))
							pinData->getInsert()->setInsertType(insertTypeDrillTool);
						else
						if (is_attrib(padstackname, PADSTACK_FIDUCIAL))
							pinData->getInsert()->setInsertType(insertTypeFiducial);
						else
						if (is_attrib(padstackname, PADSTACK_FIDUCIALTOP))
							pinData->getInsert()->setInsertType(insertTypeFiducial);
						else
						if (is_attrib(padstackname, PADSTACK_FIDUCIALBOT))
							pinData->getInsert()->setInsertType(insertTypeFiducial);
						else
							pinData->getInsert()->setInsertType(insertTypeMechanicalPin);
					}

					// do pinnr here. Do not make a pinnumber 0.
					if (strlen(pinnr) && is_number(pinnr))
					{
						int pnr = atoi(pinnr);
						globalDoc->SetAttrib(&pinData->getAttributesRef(),globalDoc->IsKeyWord(ATT_COMPPINNR, 0), valueTypeInteger, &pnr, attributeUpdateOverwrite, NULL);   
					}
				}
         }
			
			if (InMasterList(systemName, refdes) && !pin_edited) //if the pin is edited, don't add it to the master
			{
				BlockStruct* block = Graph_Block_On(GBO_APPEND, systemName,currentPCBFile->getFileNumber(), 0);
				if (block->getBlockType() == BLOCKTYPE_UNKNOWN)
				{
					// Block is new so check for block type
					if (is_attrib(systemName, PART_TOOL))
						block->setBlockType(blockTypeTooling);
					else if (is_attrib(systemName, PART_FIDUCIALTOP))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (is_attrib(systemName, PART_FIDUCIALBOT))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (is_attrib(systemName, PART_FIDUCIAL))
						block->setBlockType(BLOCKTYPE_FIDUCIAL);
					else if (!STRCMPI(stype, "MECHANICAL"))
						block->setBlockType(BLOCKTYPE_MECHCOMPONENT);
					else     
						block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
				}

				DataStruct *data = datastruct_from_refdes(globalDoc, block, pinname);
				if (data == NULL)
				{
					// Fixed Case #1162
					// Don't not add if the same pin is already in the block
					data = Graph_Block_Reference(pname, pinname,currentPCBFile->getFileNumber(), pin_x, pin_y, DegToRad(pin_rot), 0 , 1.0, -1, TRUE);
				}
				Graph_Block_Off();

				if (pin_is_electrical)
				{
					data->getInsert()->setInsertType(insertTypePin);
				}
				else
				{
					if (is_attrib(padstackname, PADSTACK_TOOL))
						data->getInsert()->setInsertType(insertTypeDrillTool);
					else
					if (is_attrib(padstackname, PADSTACK_FIDUCIAL))
						data->getInsert()->setInsertType(insertTypeFiducial);
					else
					if (is_attrib(padstackname, PADSTACK_FIDUCIALTOP))
						data->getInsert()->setInsertType(insertTypeFiducial);
					else
					if (is_attrib(padstackname, PADSTACK_FIDUCIALBOT))
						data->getInsert()->setInsertType(insertTypeFiducial);
					else
						data->getInsert()->setInsertType(insertTypeMechanicalPin);
				}

				// do pinnr here. Do not make a pinnumber 0.
				if (strlen(pinnr) && is_number(pinnr))
				{
					int pnr = atoi(pinnr);
					globalDoc->SetAttrib(&data->getAttributesRef(),globalDoc->IsKeyWord(ATT_COMPPINNR, 0), valueTypeInteger, &pnr, attributeUpdateOverwrite, NULL);   
				}

				// If there are pin-edited block, then add the new pin to all the pin-edited block
				int pinEditIndex = GetPinEditGeomIndex(geomName);
				if (pinEditIndex > -1)
				{
					ALLEGROPinEditGeom *pinEditGeom = pinEditGeomArray[pinEditIndex];

					POSITION pos = pinEditGeom->pinEditedGeomMap.GetStartPosition();
					while (pos)
					{
						CString key = "";
						void *voidPtr;
						pinEditGeom->pinEditedGeomMap.GetNextAssoc(pos, key, voidPtr);

						BlockStruct *pinEditBlock = (BlockStruct*)voidPtr;
						if (!pinEditBlock)
							continue;

						// Only add the new pin if it is not already in the pin-edited block
						DataStruct *pinData = datastruct_from_refdes(globalDoc, pinEditBlock, pinname);
						if (pinData)
							continue;   
#if CamCadMajorMinorVersion > 406  //  > 4.6
                  pinData = camCadData.getNewDataStruct(*data);
#else        
						pinData = new DataStruct(*data);
#endif
						pinEditBlock->getDataList().AddTail(pinData);
					}
				}           
			}

			DataStruct *refdesInsertData = NULL;
			if (panelmode == TRUE)
				refdesInsertData = datastruct_from_refdes(globalDoc, panelFile->getBlock(), refdes);
			else
				refdesInsertData = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), refdes);

			BlockStruct* block = Graph_Block_Exists(globalDoc, geomName, -1);
			if (refdesInsertData == NULL && block != NULL)
			{
				refdesInsertData = Graph_Block_Reference(geomName, refdes,currentPCBFile->getFileNumber(), sym_x, sym_y, DegToRad(sym_rot), sym_mir, 1.0, -1, TRUE);
            refdesInsertData->getInsert()->setInsertType(default_inserttype(block->getBlockType()));
			}

         strcpy(cur_placeblock,instanceSpecificName);
      }
      else
      if (!strcmp(stype,"FORMAT"))
      {
         /*          
            not supported in netlist file
         */
      }
   }
   else
   if (!strcmp(class_name,"VIA CLASS"))
   {
      double via_x, via_y;
		bool via_mirror = false;

      CString  pname;
      // get via_x
      // get via_y
      if (!(i=get_label(VAL_VIA_X)))
      {
         lab_error("VIA_X expected");
         return 0;
      }
      via_x = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_VIA_Y)))
      {
         lab_error("VIA_Y expected");
         return 0;
      }
      via_y = atof(recordArray[i-1]->tok) * scale_factor;
		// Case 1696, add support for via mirror. We'll call via_mirror optional.
      if (i=get_label(VAL_VIA_MIRROR))
      {
			// Values are YES/NO
			if (recordArray[i-1]->tok.CompareNoCase("YES") == 0)
				via_mirror = true;
      }
      // get pad_stack
      if (!(i=get_label(VAL_PAD_STACK_NAME)))
      {
         lab_error("PAD_STACK_NAME expected");
         return 0;
      }

      pname.Format("PADSTACK_%s",recordArray[i-1]->tok);

		CString refname;
		refname.Format("Via_%d", viaNumber++);

      Graph_Block_On(GBO_APPEND,pname,currentPCBFile->getFileNumber(),0);
      Graph_Block_Off(); 

      DataStruct* data = Graph_Block_Reference(pname, refname,currentPCBFile->getFileNumber(),
                                               via_x, via_y, DegToRad(0), via_mirror , 1.0, -1, TRUE);

      data->getInsert()->setInsertType(insertTypeVia);

      if (strlen(netname))
      {
         data->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, netname.GetBuffer(0), attributeUpdateOverwrite, NULL); 
		}

      // other attributes
      if (i=get_label(VAL_TEST_POINT))
      {
         BOOL mirror = FALSE;
			CString testaccess = recordArray[i-1]->tok;	

			// values are BOTTOM, TOP, TOP_MANUAL, BOTTOM_MANUAL
			testaccess.Trim();
			if (testaccess.IsEmpty())
			{
				return 0;
			}
			else if (!testaccess.CompareNoCase("BOTTOM") || !testaccess.CompareNoCase("BOTTOM_MANUAL"))
			{
				mirror = TRUE;
            data->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString,"BOTTOM", attributeUpdateOverwrite, NULL);
			}
         else if (!testaccess.CompareNoCase("TOP") || !testaccess.CompareNoCase("TOP_MANUAL"))
			{
            data->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString,"TOP", attributeUpdateOverwrite, NULL);
			}

         if (i = get_label(VAL_NET_PROBE_NUMBER))
         {
				CString refname;
				CString modifiedRefname;
				refname = recordArray[i-1]->tok;
            modifiedRefname.Format("Probe_%s", refname);

				// place probe
				double drillSize = 0.04 * Units_Factor(UNIT_INCHES, globalDoc->getSettings().getPageUnits());

				BlockStruct *probeGeom = generate_TestProbeGeometry(globalDoc, "Test Probe", 0.1 * Units_Factor(UNIT_INCHES, globalDoc->getSettings().getPageUnits()), "100", drillSize);
				DataStruct *probeData = Graph_Block_Reference(probeGeom->getName(), modifiedRefname,currentPCBFile->getFileNumber(), via_x, via_y, 0, mirror, 1, cur_layer, TRUE);
				probeData->getInsert()->setInsertType(insertTypeTestProbe);
				probeData->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, netname.GetBuffer(0), attributeUpdateOverwrite, NULL);

				// Add probe number as REFNAME attribute
				CreateTestProbeRefnameAttr(globalDoc, probeData, refname, drillSize);
			}
		}
   }
   else
   {
      // unknown class
      CString tmp;
      tmp.Format("Unknown Class [%s]",class_name);
      ErrorMessage(tmp,"ERROR", MB_OK | MB_ICONHAND);
   }

   return 1;
}

/******************************************************************************
* do_classic_netfile
*/
static int do_classic_netfile()
{
   CString  sname;
   char     class_name[80];
   int      i;

	CString netname = recordArray[0]->tok;

   // component, shape graphic
   // get symname+symx+symy

   // here check if via or package or mechanical
   // this is only for checking
   if (!(i=get_label(VAL_CLASS)))
   {
      lab_error("CLASS expected");
      return 0;
   }
   strcpy(class_name,recordArray[i-1]->tok);

   if (!strcmp(class_name,"PIN"))
   {
      CString  pname; // padstack name
      CString  refdes;
      char     pinnr[80];
      double   sym_x,sym_y,sym_rot = 0;
      int      sym_mir = 0;
      double   pin_x, pin_y, pin_rot;
      double   orig_x, orig_y;

      if (!(i=get_label(VAL_REFDES)))
      {
         lab_error("REFDES expected");
         return 0;
      }
      refdes = recordArray[i-1]->tok;
      refdes.TrimLeft();
      refdes.TrimRight();

      if (strlen(refdes) == 0)
      {
         messageFilter->formatMessage("Pin without refdes ignored !" );
         display_error++;
         return 0;
      }

      if (!(i=get_label(VAL_SYM_X)))
      {
         lab_error("SYM_X expected");
         return 0;
      }
      sym_x = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_SYM_Y)))
      {
         lab_error("SYM_Y expected");
         return 0;
      }
      sym_y = atof(recordArray[i-1]->tok) * scale_factor;

      // check here if it is a PCB COMPONENT
      DataStruct *data = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), refdes);
      if (data)
      {
         BlockStruct *block = globalDoc-> Find_Block_by_Num(data->getInsert()->getBlockNumber());
         sname = block->getName();
         sym_x = data->getInsert()->getOriginX();
         sym_y = data->getInsert()->getOriginY();
         sym_rot = RadToDeg(data->getInsert()->getAngle());
         sym_mir = (data->getInsert()->getMirrorFlags())?1:0;
      }

      if (!(i=get_label(VAL_PAD_STACK_NAME)))
      {
         lab_error("PAD_STACK_NAME expected");
         return 0;
      }
      pname.Format("PADSTACK_%s",recordArray[i-1]->tok);

      if (!(i=get_label(VAL_PIN_NUMBER)))
      {
         lab_error("PIN_NUMBER expected");
         return 0;
      }
      strcpy(pinnr,recordArray[i-1]->tok);

      // pin rotation is relative to defintion and not to placement.
		pin_rot = 0;
      if (!(i=get_label(VAL_PIN_ROTATION)))
         lab_error("PIN_ROTATION expected");
		else
			pin_rot = atof(recordArray[i-1]->tok);


      if (!(i=get_label(VAL_PIN_X)))
      {
         lab_error("SYM_X expected");
         return 0;
      }
      orig_x = pin_x = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_PIN_Y)))
      {
         lab_error("PIN_Y expected");
         return 0;
      }
      orig_y = pin_y = atof(recordArray[i-1]->tok) * scale_factor;

      if (refdes.Compare(oldrefdes))   // unequal to old.
      {
         // check here if it is a PCB COMPONENT

         DataStruct *ddd = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), refdes);
         if (!ddd || ddd->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         {
            cur_pcbcomponent = FALSE;
            messageFilter->formatMessage("Netlist Component [%s] not found as a PCB COMPONENT -> skipped in [%s] at %ld",
               refdes, cur_filename, line_cnt);
            display_error++;
         }
         else
            cur_pcbcomponent = TRUE;
      }
         
      // normalize
      pin_x = pin_x - sym_x;
      pin_y = pin_y - sym_y;

      if (sym_rot)
      {
         double x = pin_x, y=pin_y;
         Rotate(x,y, 360-sym_rot,&pin_x, &pin_y);
      }
      if (sym_mir)
      {
         // change layer
         pin_x = -pin_x;
      }
      if (strlen(refdes) == 0)
      {
         // this can happen if a single pin or fiducial is placed.
         messageFilter->formatMessage("Pad/Pin without Refdes in [%s] at %ld -> skipped", 
               cur_filename, line_cnt);
         display_error++;
         return 0;
      }

      oldrefdes = refdes;
      if (cur_pcbcomponent)
      {
         // make netlist
         CompPinStruct  *pin;
         if (strlen(netname) == 0)
         {
            NetStruct *net = add_net(currentPCBFile, NET_UNUSED_PINS);
            net->setFlagBits(NETFLAG_UNUSEDNET);
            pin = add_comppin(currentPCBFile, net, refdes, pinnr);
         }
         else
         {
            NetStruct *net = add_net(currentPCBFile, netname);
            pin = add_comppin(currentPCBFile, net, refdes, pinnr);
         }
         // here now put PINLOC
         if (pin)
         {
            pin->setPinCoordinatesComplete(true);
            pin->setOrigin(orig_x,orig_y);
            pin->setRotationDegrees(sym_rot + pin_rot);
            pin->setMirror(sym_mir);
            BlockStruct *b = Graph_Block_On(GBO_APPEND, pname,currentPCBFile->getFileNumber(), 0);
            Graph_Block_Off();
            pin->setPadstackBlockNumber( b->getBlockNumber());

            if (i=get_label(VAL_TEST_POINT))
            {
               CString  testaccess;
               testaccess = recordArray[i-1]->tok;

               // values are BOTTOM, TOP, TOP_MANUAL, BOTTOM_MANUAL
               if (!testaccess.CompareNoCase("BOTTOM") || !testaccess.CompareNoCase("BOTTOM_MANUAL"))
                  globalDoc->SetAttrib(&pin->getAttributesRef(), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString, "BOTTOM", attributeUpdateOverwrite, NULL);
               else if (!testaccess.CompareNoCase("TOP") || !testaccess.CompareNoCase("TOP_MANUAL"))
                  globalDoc->SetAttrib(&pin->getAttributesRef(), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString, "TOP", attributeUpdateOverwrite, NULL);
            }
         }  
      }

      // always create if it is a new instance
      CString name;
      name.Format("%s_%s", sname, refdes);

      if (strcmp(cur_placeblock, name))
         strcpy(cur_block, "");

      CString  s_name = sname;   // the symbol name can change if a new geometry is generated.
         
      if (master_block(s_name, refdes))
      {
         BlockStruct *b;
         b = Graph_Block_On(GBO_APPEND,s_name,currentPCBFile->getFileNumber(),0);
         // b->blocktype = BLOCKTYPE_PCBCOMPONENT;
         DataStruct *d = Graph_Block_Reference(pname, pinnr,currentPCBFile->getFileNumber(), pin_x, pin_y,
            DegToRad(pin_rot), 0 , 1.0, -1, TRUE);
         d->getInsert()->setInsertType(insertTypePin);
         Graph_Block_Off();
      }
      strcpy(cur_placeblock,name);
   }
   else
   if (!strcmp(class_name,"VIA CLASS"))
   {
      double via_x, via_y;
      CString  pname;
      // get via_x
      // get via_y
      if (!(i=get_label(VAL_VIA_X)))
      {
         lab_error("VIA_X expected");
         return 0;
      }
      via_x = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_VIA_Y)))
      {
         lab_error("VIA_Y expected");
         return 0;
      }
      via_y = atof(recordArray[i-1]->tok) * scale_factor;
         // get pad_stack
      if (!(i=get_label(VAL_PAD_STACK_NAME)))
      {
         lab_error("PAD_STACK_NAME expected");
         return 0;
      }
      pname.Format("PADSTACK_%s",recordArray[i-1]->tok);

      Graph_Block_On(GBO_APPEND,pname,currentPCBFile->getFileNumber(),0);
      Graph_Block_Off();
      DataStruct* data = Graph_Block_Reference(pname, NULL,currentPCBFile->getFileNumber(), via_x, via_y, DegToRad(0), 0 , 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypeVia);
      if (strlen(netname))
      {
         data->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, netname.GetBuffer(0), attributeUpdateOverwrite, NULL);      
      }

      // other attributes
      if (i=get_label(VAL_TEST_POINT))
      {
         BOOL mirror = FALSE;
			CString testaccess = recordArray[i-1]->tok;	
			
			// values are BOTTOM, TOP, TOP_MANUAL, BOTTOM_MANUAL
			testaccess.Trim();
			if (testaccess.IsEmpty())
			{
				return 0;
			}
			else if (!testaccess.CompareNoCase("BOTTOM") || !testaccess.CompareNoCase("BOTTOM_MANUAL"))
			{
				mirror = TRUE;
            data->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString,"BOTTOM", attributeUpdateOverwrite, NULL);
			}
         else if (!testaccess.CompareNoCase("TOP") || !testaccess.CompareNoCase("TOP_MANUAL"))
			{
            data->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_TEST, 0), valueTypeString,"TOP", attributeUpdateOverwrite, NULL);
			}

         if (i = get_label(VAL_NET_PROBE_NUMBER))
         {
				CString refname;
				CString modifiedRefname;
				refname = recordArray[i-1]->tok;
            modifiedRefname.Format("Probe_%s", refname);

				// place probe
				double drillSize = 0.04 * Units_Factor(UNIT_INCHES, globalDoc->getSettings().getPageUnits());

				BlockStruct *probeGeom = generate_TestProbeGeometry(globalDoc, "Test Probe", 0.1 * Units_Factor(UNIT_INCHES, globalDoc->getSettings().getPageUnits()), "100", drillSize);
				DataStruct *probeData = Graph_Block_Reference(probeGeom->getName(), modifiedRefname,currentPCBFile->getFileNumber(), via_x, via_y, 0, mirror, 1, cur_layer, TRUE);
				probeData->getInsert()->setInsertType(insertTypeTestProbe);
				probeData->setAttrib(ATTRIB_BASE(globalDoc), globalDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, netname.GetBuffer(0), attributeUpdateOverwrite, NULL);

				// Add probe number as REFNAME attribute
				CreateTestProbeRefnameAttr(globalDoc, probeData, refname, drillSize);
			}
      }
   }
   else
   {
      // unknown class
      CString tmp;
      tmp.Format("Unknown Class [%s]",class_name);
      ErrorMessage(tmp,"ERROR", MB_OK | MB_ICONHAND);
   }
   return 1;
}

/******************************************************************************
* do_fabmaster_layer
A!CLASS!SUBCLASS!
*/
static int do_fabmaster_layer()
{
   int   i;
   CString  subclass, lay;
   char     class_name[80];
   int      graphic_class = 0;                     

   if (!(i=get_label(VAL_CLASS)))
   {
      lab_error("CLASS expected");
      return 0;
   }
   strcpy(class_name,recordArray[i-1]->tok);
   cur_layer_class = class_name;

   if (!(i=get_label(VAL_SUBCLASS)))
   {
      lab_error("SUBCLASS expected");
      return 0;
   }
   subclass = lay = recordArray[i-1]->tok;
   cur_layer_subclass = subclass;


   if (!suppress_class)
   {
      //lay gets prefix, but not if it is etch.
      if (STRCMPI(class_name, "ETCH"))
      {
         CString  tmp;
         tmp = class_name;
         tmp += ",";
         tmp += lay;
         lay = tmp;
      }
   }

   int w1 = update_layer(lay);
   layers[w1].used = TRUE;
   cur_layer = Graph_Level(lay,"", 0);

   LayerStruct *l = globalDoc->FindLayer(cur_layer);
	if (l != NULL)
	{
   // fabmaster has not layerstacking defined !
		if (!STRCMPI(class_name, "ETCH"))
		{
			if (!STRCMPI(subclass, "TOP"))
			{
				l->setLayerType(LAYTYPE_SIGNAL_TOP);
			}
			else
			if (!STRCMPI(subclass, "BOTTOM"))
			{
				l->setLayerType(LAYTYPE_SIGNAL_BOT);
			}
			else
			{
				l->setLayerType(LAYTYPE_SIGNAL_INNER);
			}
		}
	}	
    
   return 1;
}

/******************************************************************************
* do_fabmaster_vias
A!VIA_X!VIA_Y!PAD_STACK_NAME!NET_NAME!TEST_POINT!
*/
static int do_fabmaster_vias()
{
   int      i;
   double   via_x, via_y;
   CString  pname, netname, testpoint;
   
   // get via_x
   // get via_y
   if (!(i=get_label(VAL_VIA_X)))
   {
      lab_error("VIA_X expected");
      return 0;
   }
   via_x = atof(recordArray[i-1]->tok) * scale_factor;
   if (!(i=get_label(VAL_VIA_Y)))
   {
      lab_error("VIA_Y expected");
      return 0;
   }
   via_y = atof(recordArray[i-1]->tok) * scale_factor;
   // get pad_stack
   if (!(i=get_label(VAL_PAD_STACK_NAME)))
   {
      lab_error("PAD_STACK_NAME expected");
      return 0;
   }
   pname.Format("PADSTACK_%s",recordArray[i-1]->tok);

   // get pad_stack
   if (!(i=get_label(VAL_NET_NAME)))
   {
      lab_error("NET_NAME expected");
      return 0;
   }
   netname = recordArray[i-1]->tok;

   BlockStruct *b = Graph_Block_On(GBO_APPEND,pname,currentPCBFile->getFileNumber(),0);
   // b->blocktype = 
   Graph_Block_Off();      // just make sure it is there.
   DataStruct *d = Graph_Block_Reference(pname, NULL,currentPCBFile->getFileNumber(), via_x, via_y,
                        DegToRad(0), 0 , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeVia);
   if (strlen(netname))
   {
      globalDoc->SetUnknownAttrib(&d->getAttributesRef(),ATT_NETNAME, netname,
            SA_APPEND, NULL); // x, y, rot, height
   }

   // other attributes
   if (i=get_label(VAL_TEST_POINT))
   {
      CString  testaccess;
      testaccess = recordArray[i-1]->tok;
      // values are BOTTOM, TOP, TOP_MANUAL, BOTTOM_MANUAL

      if (!testaccess.CompareNoCase("BOTTOM") || !testaccess.CompareNoCase("BOTTOM_MANUAL"))
         globalDoc->SetAttrib(&d->getAttributesRef(),globalDoc->IsKeyWord(ATT_TEST, 0),
               valueTypeString,"BOTTOM", attributeUpdateOverwrite, NULL); // x, y, rot, height
      else
      if (!testaccess.CompareNoCase("TOP") || !testaccess.CompareNoCase("TOP_MANUAL"))
         globalDoc->SetAttrib(&d->getAttributesRef(),globalDoc->IsKeyWord(ATT_TEST, 0),
               valueTypeString,"TOP", attributeUpdateOverwrite, NULL); // x, y, rot, height

   }

   return 1;
}

/******************************************************************************
* do_fabmaster_geompins
version 0
A!SYM_NAME!PIN_NAME!PIN_NUMBER!PIN_X!PIN_Y!PAD_STACK_NAME!REFDES!TEST_POINT!
version 1
A!SYM_NAME!SYM_MIRROR|PIN_NAME!PIN_NUMBER!PIN_X!PIN_Y!PAD_STACK_NAME!REFDES!PIN_ROTATION!TEST_POINT!
*/
static int do_fabmaster_geompins(int version)
{
   CString  sname;
   CString  name;
   int      i;


   if (!(i=get_label(VAL_SYM_NAME)))
   {
      lab_error("SYM_NAME expected");
      return 0;
   }
   sname = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_REFDES)))
   {
      lab_error("REFDES expected");
      return 0;
   }
   name = recordArray[i-1]->tok;
                
   if (!STRICMP(name.Right(1),"*"))      // can not find the master block for this !
   {
      sname = "";
      name = "";
   }        

   // always create if it is a new instance
   CString  xname;
   xname.Format("%s_%s",sname,name);

   // if DIP14_IC1 is now DIP14_IC2 kill curblock
   // it still will find it in the masterlist
   if (strcmp(xname,cur_placeblock))
      strcpy(cur_block,"");

   //strcpy(cur_placeblock,xname);

   if (strlen(sname) && strlen(name))
   {
      if (master_block(sname, name))
      {  
         
         strcpy(cur_placeblock,xname);

         double   sym_x,sym_y,sym_rot;
         int      sym_mir;
         double   pin_x, pin_y, pin_rot = 0;  // pinrot is not used in fabmaster
         char     pinname[80], pinnr[80];
         char     pname[80];

         DataStruct *data = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), name);

         if (data == NULL)
         {
            messageFilter->formatMessage("Can not find [%s] at %ld", name, line_cnt); 
            display_error++;
            return 1;
         }

         sym_x = data->getInsert()->getOriginX();
         sym_y = data->getInsert()->getOriginY();
         sym_rot = RadToDeg(data->getInsert()->getAngle());
         sym_mir = (data->getInsert()->getMirrorFlags())?1:0;

         if (!(i = get_label(VAL_PAD_STACK_NAME)))
         {
            lab_error("PAD_STACK_NAME expected");
            return 0;
         }
         sprintf(pname, "PADSTACK_%s",recordArray[i-1]->tok);

         if (!(i = get_label(VAL_PIN_NAME)))
         {
            lab_error("PIN_NAME expected");
            return 0;
         }
         strcpy(pinname, recordArray[i-1]->tok);

         if (!(i = get_label(VAL_PIN_NUMBER)))
         {
            lab_error("PIN_NUMBER expected");
            return 0;
         }
         strcpy(pinnr, recordArray[i-1]->tok);

         if (!(i = get_label(VAL_PIN_X)))
         {
            lab_error("SYM_X expected");
            return 0;
         }
         pin_x = atof(recordArray[i-1]->tok) * scale_factor;

         if (!(i = get_label(VAL_PIN_Y)))
         {
            lab_error("PIN_Y expected");
            return 0;
         }
         pin_y = atof(recordArray[i-1]->tok) * scale_factor;

         // pin rotation is relative to defintion and not to placement.
			pin_rot = 0;
         if (!(i=get_label(VAL_PIN_ROTATION)))
            lab_error("PIN_ROTATION expected");
			else
				pin_rot = atof(recordArray[i-1]->tok);

         // normalize
         pin_x = pin_x - sym_x;
         pin_y = pin_y - sym_y;
         if (sym_rot)
         {
            double x = pin_x, y=pin_y;
            Rotate(x,y, 360-sym_rot, &pin_x, &pin_y);
         }
         if (sym_mir)
         {
            // change layer
            pin_x = -pin_x;
         }

         if (strlen(name) == 0)
         {
            // this can happen if a single pin or fiducial is placed.
            messageFilter->formatMessage("Pad/Pin without Refdes in [%s] at %ld -> skipped", cur_filename, line_cnt);
            display_error++;
            return 0;
         }

         // pinnames do not need to be unique in Cadence.
         if (!usepinname)
            strcpy(pinname, pinnr);
                     // if there is no pinname, than take pinnumber as pinname.
         if (strlen(pinname) == 0)
            strcpy(pinname, pinnr);
         
         if (strlen(pinname) == 0)
         {
            // unknown extract
            messageFilter->formatMessage("Pinname String expected in [%s] at %ld", cur_filename, line_cnt);
            display_error++;
            return -1;
         }


         BlockStruct *block = Graph_Block_On(GBO_APPEND, sname,currentPCBFile->getFileNumber(), 0);
         DataStruct *insertdata = Graph_Block_Reference(pname, pinname,currentPCBFile->getFileNumber(), pin_x, pin_y, DegToRad(pin_rot), 0 , 1.0, -1, TRUE);
         insertdata->getInsert()->setInsertType(insertTypePin);

         // do pinnr here. Do not make a pinnumber 0.
         if (strlen(pinnr) && is_number(pinnr))
         {
            int pnr = atoi(pinnr);
            globalDoc->SetAttrib(&insertdata->getAttributesRef(), globalDoc->IsKeyWord(ATT_COMPPINNR, 0), valueTypeInteger, &pnr, attributeUpdateOverwrite, NULL); 
         }
         Graph_Block_Off();
      }
   }
   else
   {
      // pin without a refdes found.
      // this should be placed as free pads
      double pin_x, pin_y, pin_rot = 0;
      char   pname[80];
      char   pinname[80];

      if (version > 0)
      {
         // pin rotation is relative to defintion and not to placement.
			pin_rot = 0;
         if (!(i=get_label(VAL_PIN_ROTATION)))
            lab_error("PIN_ROTATION expected");
			else
				pin_rot = atof(recordArray[i-1]->tok);
      }

      if (!(i=get_label(VAL_PAD_STACK_NAME)))
      {
         lab_error("PAD_STACK_NAME expected");
         return 0;
      }
      sprintf(pname,"PADSTACK_%s",recordArray[i-1]->tok);

      if (!(i=get_label(VAL_PIN_NAME)))
      {
         lab_error("PIN_NAME expected");
         return 0;
      }
      strcpy(pinname,recordArray[i-1]->tok);

      if (!(i=get_label(VAL_PIN_X)))
      {
         lab_error("SYM_X expected");
         return 0;
      }
      pin_x = atof(recordArray[i-1]->tok) * scale_factor;
      if (!(i=get_label(VAL_PIN_Y)))
      {
         lab_error("PIN_Y expected");
         return 0;
      }
      pin_y = atof(recordArray[i-1]->tok) * scale_factor;

      DataStruct *d = Graph_Block_Reference(pname, pinname,currentPCBFile->getFileNumber(), pin_x, pin_y,
         DegToRad(pin_rot), 0 , 1.0, -1, TRUE);
      d->getInsert()->setInsertType(insertTypeFreePad);
   }
   return 1;
}

/******************************************************************************
* do_fabmaster_traces
A!CLASS!SUBCLASS!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!RECORD_TAG!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!NET_NAME!
*/
static int do_fabmaster_traces()
{
   int   i;
   CString  subclass, lay;
   CString  class_name;
   int      graphic_class = 0;                     

   if (!(i=get_label(VAL_CLASS)))
   {
      lab_error("CLASS expected");
      return 0;
   }
   class_name = recordArray[i-1]->tok;
   cur_layer_class = class_name;

   if (!(i=get_label(VAL_SUBCLASS)))
   {
      lab_error("SUBCLASS expected");
      return 0;
   }
   subclass = lay = recordArray[i-1]->tok;
   cur_layer_subclass = subclass;

   if (!suppress_class)
   {
      //lay gets prefix, but not if it is etch.
      if (class_name.CompareNoCase("ETCH"))
      {
         CString  tmp;
         tmp = class_name;
         tmp += ",";
         tmp += lay;
         lay = tmp;
      }
   }

   int w1 = update_layer(lay);
   layers[w1].used = TRUE;
   cur_layer = Graph_Level(lay,"", 0);

   int majj = 0, minj = 0, littlej = 0;
   char rtag[80];

   // record_tag
   if (!(i=get_label(VAL_RECORD_TAG)))
   {
      lab_error("RECORD_TAG expected");
      return 0;
   }
   strcpy(rtag,recordArray[i-1]->tok);
   char *lp;
   if ((lp = strtok(rtag," \t")) != NULL)
   {
      majj = atoi(lp);
      if ((lp = strtok(NULL," \t")) != NULL)
      {
         minj = atoi(lp);
         if ((lp = strtok(NULL," \t")) != NULL)
         {
            littlej = atoi(lp);
         }
      }
   }

   lastmaj = majj;

   // trace and other graphic
   CString  netname;

   netname = "";
   if (!(i=get_label(VAL_NET_NAME)))
   {
      lab_error("NET_NAME expected");
      return 0;
   }
   netname = recordArray[i-1]->tok;

   if (is_primboardoutline(subclass, class_name))
      graphic_class = graphicClassBoardOutline;
	else if ((graphic_class = get_graphic_class(class_name, netname.IsEmpty())) == 0)
      graphic_class = get_graphic_subclass(subclass);
         
   cur_layer = Graph_Level(lay,"", 0);
   do_graphic(netname,0,0,0,0,majj,minj,littlej,graphic_class,TRUE, NULL);

   return 1;
}

/******************************************************************************
* do_fabmaster_geomgraphics
A!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!RECORD_TAG!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!SUBCLASS!SYM_NAME!REFDES!
*/
static int do_fabmaster_geomgraphics()
{
   CString  sname;
   CString  name;
   CString  subclass, lay;
   int      i, graphic_class;

   if (!(i=get_label(VAL_SUBCLASS)))
   {
      lab_error("SUBCLASS expected");
      return 0;
   }
   subclass = lay = recordArray[i-1]->tok;
   cur_layer_subclass = subclass;

   graphic_class = get_graphic_subclass(subclass);

   int w1 = update_layer(lay);
   layers[w1].used = TRUE;
   cur_layer = Graph_Level(lay,"", 0);

   int majj = 0, minj = 0, littlej = 0;
   char rtag[80];

   // record_tag
   if (!(i=get_label(VAL_RECORD_TAG)))
   {
      lab_error("RECORD_TAG expected");
      return 0;
   }
   strcpy(rtag,recordArray[i-1]->tok);
   char *lp;
   if ((lp = strtok(rtag," \t")) != NULL)
   {
      majj = atoi(lp);

      if ((lp = strtok(NULL," \t")) != NULL)
      {
         minj = atoi(lp);
         if ((lp = strtok(NULL," \t")) != NULL)
         {
            littlej = atoi(lp);
         }
      }
   }

   lastmaj = majj;


   if (!(i=get_label(VAL_SYM_NAME)))
   {
      lab_error("SYM_NAME expected");
      return 0;
   }
   sname = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_REFDES)))
   {
      lab_error("REFDES expected");
      return 0;
   }
   name = recordArray[i-1]->tok;
                
   if (!STRICMP(name.Right(1),"*"))      // can not find the master block for this !
   {
      sname = "";
      name = "";
   }        

   // always create if it is a new instance
   CString  xname;
   xname.Format("%s_%s",sname,name);

   // if DIP14_IC1 is now DIP14_IC2 kill curblock
   // it still will find it in the masterlist
   if (strcmp(xname,cur_placeblock))
      strcpy(cur_block,"");

   //strcpy(cur_placeblock,xname);

   if (strlen(sname) && strlen(name))
   {
      if (master_block(sname, name))
      {  
         
         strcpy(cur_placeblock,xname);

         double   sym_x,sym_y,sym_rot;
         int      sym_mir;

         DataStruct *ddd = datastruct_from_refdes(globalDoc, currentPCBFile->getBlock(), name);

         if (ddd == NULL)
         {
            // error ???
            messageFilter->formatMessage("Can not find [%s] at %ld", name, line_cnt); 
            display_error++;
            return 1;
         }

         sym_x = ddd->getInsert()->getOriginX();
         sym_y = ddd->getInsert()->getOriginY();
         sym_rot = RadToDeg(ddd->getInsert()->getAngle());
         sym_mir = (ddd->getInsert()->getMirrorFlags())?1:0;

         BlockStruct *b;
         b = Graph_Block_On(GBO_APPEND,sname,currentPCBFile->getFileNumber(),0);

         // if mechanical symbol
         b->setBlockType(BLOCKTYPE_PCBCOMPONENT);
         if (is_attrib(sname, SMD_ATT))
         {
            globalDoc->SetUnknownAttrib(&b->getAttributesRef(), ATT_SMDSHAPE, "",
             attributeUpdateOverwrite, NULL); //
         }

         if (sym_mir)
         {                                           
            int w1 = update_layer(lay);
            // Debug
            // int m = layers[w1].mirror;
            // Layers mm = layers[w1];
            lay = layers[layers[w1].mirror].name;
            cur_layer = Graph_Level(lay,"", 0);
         }
         else
            cur_layer = Graph_Level(lay,"", 0);
         
         //if (is_primcompoutline(lay, sclass_name))
         // graphic_class = graphicClassComponentOutline;

         if (!panelmode)
         {
            //if (is_primboardoutline(lay, class_name))
            // graphic_class = graphicClassBoardOutline;
         }
         else
         {
            //if (is_primpaneloutline(lay, class_name))
            // graphic_class = graphicClassPanelOutline;
         }
      
         //if (is_primcompheight(lay, class_name))
         //{
         // for (i=0;i<commandArrayCount;i++)
         // {
         //    if (!STRCMPI("GRAPHIC_DATA_NAME",commandArray[i]->tok) &&
         //        !STRCMPI(recordArray[i]->tok,"TEXT"))
         //    {
         //       graphic_class = COMPHEIGHT_CLASS;
         //       break;
         //    }
         // }
         //}

         // board geometry should only be used in Board outline and graphic definitions, not in
         // components.
         //if (graphic_class == GR_CLASS_BOARD_GEOM)
         // graphic_class = 0;
         do_graphic("",sym_x, sym_y, sym_rot, sym_mir, majj, minj,littlej, graphic_class,FALSE, &b->getAttributesRef());
   
         Graph_Block_Off();
      }
   }
   else
   {
      strcpy(cur_placeblock, "");
      // no sym or refname 
      do_graphic("",0,0,0,0,majj,minj,littlej,graphic_class,TRUE, NULL);
   }

   return 1;
}

/******************************************************************************
* do_fabmaster_netlist
*/
static int do_fabmaster_netlist()
{
   int      i;
   CString  refdes, netname;
   char     pinname[80], pinnr[80];

   if (!(i=get_label(VAL_NET_NAME)))
   {
      lab_error("NET_NAME expected");
      return 0;
   }
   netname = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_REFDES)))
   {
      lab_error("REFDES expected");
      return 0;
   }
   refdes = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_PIN_NUMBER)))
   {
      lab_error("PIN_NUMBER expected");
      return 0;
   }
   strcpy(pinnr,recordArray[i-1]->tok);

   if (!(i=get_label(VAL_PIN_NAME)))
   {
      lab_error("PIN_NAME expected");
      return 0;
   }
   strcpy(pinname,recordArray[i-1]->tok);


   if (strlen(refdes) == 0)
   {
      // this can happen if a single pin or fiducial is placed.
      messageFilter->formatMessage("Pad/Pin without Refdes in [%s] at %ld -> skipped", 
            cur_filename, line_cnt);
      display_error++;
      return 0;
   }

   // pinnames do not need to be unique in Cadence.
   if (!usepinname)
      strcpy(pinname,pinnr);

   // if there is no pinname, than take pinnumber as pinname.
   if (strlen(pinname) == 0)
      strcpy(pinname,pinnr);

   if (strlen(pinname) == 0)
   {
      // unknown extract
      messageFilter->formatMessage("Pinname String expected in [%s] at %ld", cur_filename, line_cnt);
      display_error++;
      return -1;
   }

   // make netlist
   CompPinStruct *pin;
   if (strlen(netname) == 0)
   {
      NetStruct *net = add_net(currentPCBFile,NET_UNUSED_PINS);
      net->setFlagBits(NETFLAG_UNUSEDNET);
      pin = add_comppin(currentPCBFile, net, refdes, pinname);
   }
   else
   {
      NetStruct *net = add_net(currentPCBFile,netname);
      pin = add_comppin(currentPCBFile, net, refdes ,pinname);
   }


   return 1;
}

/******************************************************************************
* do_fabmaster_refdes
A!REFDES!COMP_CLASS!COMP_PART_NUMBER!COMP_HEIGHT!COMP_DEVICE_LABEL!COMP_INSERTION_CODE!SYM_TYPE!SYM_NAME!SYM_MIRROR!SYM_ROTATE!SYM_X!SYM_Y!COMP_VALUE!COMP_TOL!COMP_VOLTAGE!COMP_RATED_CURRENT!COMP_RATED_POWER!COMP_RATED_VOLTAGE!
*/
static int do_fabmaster_refdes()
{
   int i;
   if (!(i=get_label(VAL_REFDES)))
   {
      lab_error("REFDES expected");
      return 0;

   }
   CString refdes = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_COMP_CLASS)))
   {
      lab_error("COMP_CLASS expected");
      return 0;

   }
   CString comp_class = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_COMP_PART_NUMBER)))
   {
      lab_error("COMP_PART_NUMBER expected");
      return 0;

   }
   CString comp_part_number = recordArray[i-1]->tok;

//COMP_HEIGHT!COMP_DEVICE_LABEL!COMP_INSERTION_CODE!
//COMP_VALUE!COMP_TOL!COMP_VOLTAGE!COMP_RATED_CURRENT!COMP_RATED_POWER!COMP_RATED_VOLTAGE!

   if (!(i=get_label(VAL_SYM_TYPE)))
   {
      lab_error("SYM_TYPE expected");
      return 0;
   }
   CString stype = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_SYM_NAME)))
   {
      lab_error("SYM_NAME expected");
      return 0;
   }
   CString sname = recordArray[i-1]->tok;

   if (!(i=get_label(VAL_SYM_X)))
   {
      lab_error("SYM_X expected");
      return 0;
   }
   double sym_x = atof(recordArray[i-1]->tok) * scale_factor;

   if (!(i=get_label(VAL_SYM_Y)))
   {
      lab_error("SYM_Y expected");
      return 0;
   }
   double sym_y = atof(recordArray[i-1]->tok) * scale_factor;

   if (!(i=get_label(VAL_SYM_ROTATE)))
   {
      lab_error("SYM_ROTATE expected");
      return 0;
   }
   double sym_rot = atof(recordArray[i-1]->tok);
   
   if (!(i=get_label(VAL_SYM_MIRROR)))
   {
      lab_error("SYM_MIRROR expected");
      return 0;
   }
   int sym_mir = !STRICMP(recordArray[i-1]->tok, "YES");

   // if a refname ends in * than it is an unnamed component and needs to get 
   // the sym_koos added to the name
   if (!STRICMP(refdes.Right(1), "*"))
   {
      CString t;
      t.Format("%s_%2.3lf_%2.3lf_%1.0lf_%d", refdes, sym_x, sym_y, sym_rot, sym_mir);
      refdes = t;
   }
	else if (strlen(refdes) == 0)
	{
      CString t;
      t.Format("%s_%2.3lf_%2.3lf_%1.0lf_%d", sname, sym_x, sym_y, sym_rot, sym_mir);
      refdes = t;
	}

   BlockStruct *block = Graph_Block_On(GBO_APPEND, sname,currentPCBFile->getFileNumber(), 0);
   block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
   Graph_Block_Off();

   DataStruct *data = Graph_Block_Reference(sname, refdes ,currentPCBFile->getFileNumber(), sym_x, sym_y, DegToRad(sym_rot), sym_mir, 1.0, -1, TRUE);

   // if not mechanical symbol
   if (is_attrib(sname, MECHANICAL_SYMBOL))  
      data->getInsert()->setInsertType(insertTypeMechanicalComponent);
   else
      data->getInsert()->setInsertType(insertTypePcbComponent);

   if (panelmode)
   {
      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
      {
         data->getInsert()->setInsertType(insertTypeMechanicalComponent);
         messageFilter->formatMessage(" A PACKAGE COMPONENT [%s] is placed on Panel -> Changed to MECHANICAL in [%s] at %ld", 
               refdes, cur_filename, line_cnt);
         display_error++;
      }
   }

   if (!(i = get_label(VAL_COMP_VALUE)))  // use the COMP_VALUE as the device
   {
      lab_error("COMP_VALUE expected");
      return 0;
   }
   CString devicename = recordArray[i-1]->tok;

   if (strlen(devicename) == 0)
      devicename = sname; // no device in Fabmaster

   // here add attributes
   if (strlen(devicename))
   {
      TypeStruct *type = AddType(currentPCBFile, devicename);
      if (type->getBlockNumber() == -1)
      {
         type->setBlockNumber( data->getInsert()->getBlockNumber());
      }
      else if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
      {
         CString newdevice;
         newdevice.Format("%s_%s", devicename, sname);
         type = AddType(currentPCBFile, newdevice);

         if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
            type->setBlockNumber( data->getInsert()->getBlockNumber());
      }
      globalDoc->SetAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord(ATT_TYPELISTLINK, 0), type->getName(), attributeUpdateOverwrite, NULL);

      for (int i=0; i<commandArrayCount; i++)
      {
         if (is_attrib(commandArray[i]->tok, TYPE_ATT) && strlen(recordArray[i]->tok))
         {
            if (!STRCMPI("COMP_VALUE", commandArray[i]->tok))
               continue;
            do_attrib(&type->getAttributesRef(), get_attrmap(commandArray[i]->tok), valueTypeString, recordArray[i]->tok, 0.0, 0);
         }
      }
   }

   // add refdes
   if (strlen(refdes))
      globalDoc->SetAttrib(&data->getAttributesRef(),globalDoc->IsKeyWord(ATT_REFNAME, 0), valueTypeString, refdes.GetBuffer(0), attributeUpdateOverwrite, NULL); 

   return 1;
}

/******************************************************************************
* do_graphic
   Attention : If you return from this function make sure that set_pcb is done
*/
static int do_graphic(const char *netname, double sym_x, double sym_y, double sym_rot, int sym_mirror, int majj, int minj,
      int littlej, int graphic_class, int calc_ext, CAttributes** map)
{
   DataStruct *d = NULL;
   char  gname[20];
   int   i;
   DbFlag   flg = 0;

   if (!(i=get_label(VAL_GRAPHIC_DATA_NAME)))
   {
      lab_error("GRAPHIC_DATA_NAME expected");
      return 0;
   }

   strcpy(gname,recordArray[i-1]->tok);
   LayerStruct* layerStruct = globalDoc->FindLayer(cur_layer);

   if (layerStruct != NULL && layerStruct->getType() == LAYTYPE_BOARD_OUTLINE && 
       graphic_class == GR_CLASS_BOARD_GEOM)
   {
      graphic_class = graphicClassBoardOutline;
   }

   // here check for cutout
   for (i=0;i<commandArrayCount;i++)
   {
      if (!STRCMPI(commandArray[i]->tok,"CUTOUT") && !STRCMPI(recordArray[i]->tok,"YES"))
      {
         if (panelmode)
            graphic_class = graphicClassPanelCutout;
         else
            graphic_class = graphicClassBoardCutout;
         break;
      }
   }

   if (!STRCMPI(gname,"LINE"))
   {
      double x1,y1,x2,y2;
      int    widthindex, fill = FALSE;
      int    grtype, close = FALSE, gvoid = FALSE;

      do_line(&x1,&y1,&x2,&y2,&widthindex, &grtype);
      x1 = x1 - sym_x;
      y1 = y1 - sym_y;
      x2 = x2 - sym_x;
      y2 = y2 - sym_y;
      if (sym_rot)
      {
         double x = x1, y=y1;
         Rotate(x,y, 360-sym_rot,&x1, &y1);
         x = x2, y = y2;
         Rotate(x,y, 360-sym_rot,&x2, &y2);
      }
      if (sym_mirror)
      {
         x1 = -x1;
         x2 = -x2;
      }

      if (calc_ext && graphic_class == graphicClassBoardOutline)
      {
         update_boardext(x1,y1);
         update_boardext(x2,y2);
      }

      if (minj == 1)
      {
         d = Graph_PolyStruct(cur_layer, 0L,FALSE);
         d->setGraphicClass(graphic_class);
      }
      else
      {
         if (old_widthindex != widthindex)
         {
            d = Graph_PolyStruct(cur_layer, 0L,FALSE);
            d->setGraphicClass(graphic_class);
            minj = 1;
         }
      }
         
      old_widthindex = widthindex;

      if (grtype == VAL_SHAPE)
      {
         fill = TRUE;
         close = TRUE;
      }
      if (grtype == VAL_VOID)
      {
         fill = TRUE;
         close = TRUE;
         gvoid = TRUE;
      }
   
      // no fill on these classes
      if (graphic_class == GR_CLASS_COMPONENT_BOUNDARY ||
          graphic_class == GR_CLASS_PLACEKEEPIN ||
          graphic_class == GR_CLASS_PLACEKEEPOUT ||
          graphic_class == GR_CLASS_VIAKEEPIN ||
          graphic_class == GR_CLASS_VIAKEEPOUT ||
          graphic_class == GR_CLASS_ROUTKEEPIN ||
          graphic_class == GR_CLASS_ROUTKEEPOUT)
      {
         fill = FALSE;
      }

      if (minj == 1 || (littlej && littlej != lastlittlej))
      {
         Graph_Poly(NULL, widthindex,fill, gvoid, close);
         lastvertex = Graph_Vertex(x1,y1, 0.0);
      }
      lastlittlej = littlej;
      lastvertex = Graph_Vertex(x2,y2, 0.0);
   
      int i = get_label(VAL_NET_NAME); 
      if (!i)
      {
         lab_error("NET_NAME expected");
         return 0;
      }
		CString netname = recordArray[i-1]->tok;

		if (strlen(netname) > 0 && d != NULL)
			globalDoc->SetAttrib(&d->getAttributesRef(), globalDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, netname.GetBuffer(0), attributeUpdateOverwrite, NULL);
	}
   else if (!STRCMPI(gname,"RECTANGLE"))
   {
      double x1,y1,x2,y2;
      int    widthindex = 0, fill = FALSE;
      do_rectangle(&x1,&y1,&x2,&y2,&fill);
      x1 = x1 - sym_x;
      y1 = y1 - sym_y;
      x2 = x2 - sym_x;
      y2 = y2 - sym_y;
      if (sym_rot)
      {
         double x = x1, y=y1;
         Rotate(x,y, 360-sym_rot,&x1, &y1);
         x = x2, y = y2;
         Rotate(x,y, 360-sym_rot,&x2, &y2);
      }
      if (sym_mirror)
      {
         // change layer
         x1 = -x1;
         x2 = -x2;
      }

      if (calc_ext && graphic_class == graphicClassBoardOutline)
      {
         update_boardext(x1,y1);
         update_boardext(x2,y2);
      }

      if (minj == 1)
      {
         d = Graph_PolyStruct(cur_layer, 0, FALSE);
         d->setGraphicClass(graphic_class);
         Graph_Poly(NULL, widthindex, fill, 0, 1);
      }
      Graph_Vertex(x1,y1,0.0);
      Graph_Vertex(x2,y1,0.0);
      Graph_Vertex(x2,y2,0.0);
      Graph_Vertex(x1,y2,0.0);
      Graph_Vertex(x1,y1,0.0);
   }
   else if (!STRCMPI(gname, "TEXT"))
   {
      int i = get_label(VAL_SUBCLASS); 
      CString subClass;
      if (i)
      {
         Record *record = recordArray[i-1];
         subClass = record->tok;
      }

      if (graphic_class == graphicClassManufacturing && (!subClass.CompareNoCase("PROBE_TOP") || !subClass.CompareNoCase("PROBE_BOTTOM")))
      {
   //      i = get_label(VAL_GRAPHIC_DATA_1); 
   //      if (!i)
   //      {
   //         lab_error("GRAPHIC_DATA_1 expected");
   //         return 0;
   //      }
   //      Record *record = recordArray[i-1];
   //      double x = atof(record->tok) * scale_factor;

   //      i = get_label(VAL_GRAPHIC_DATA_2); 
   //      if (!i)
   //      {
   //         lab_error("GRAPHIC_DATA_2 expected");
   //         return 0;
   //      }
   //      record = recordArray[i-1];
   //      double y = atof(record->tok) * scale_factor;

   //      i = get_label(VAL_NET_NAME); 
   //      if (!i)
   //      {
   //         lab_error("NET_NAME expected");
   //         return 0;
   //      }
   //      record = recordArray[i-1];
   //      CString netname = record->tok;

   //      i = get_label(VAL_GRAPHIC_DATA_7); 
   //      CString refname = "";
			//CString modifiedRefname;
   //      if (i > 0)
   //      {
   //         record = recordArray[i-1];
	//			  refname = record->tok;
   //         modifiedRefname.Format("Probe_%s", record->tok);
   //      }

   //      BOOL mirror = FALSE;
   //      if (!subClass.CompareNoCase("PROBE_BOTTOM"))
   //         mirror = TRUE;

   //      // place probe
			//double drillSize = 0.04 * Units_Factor(UNIT_INCHES, globalDoc->getSettings().getPageUnits());

   //      BlockStruct *probeGeom = generate_TestProbeGeometry(globalDoc, "Test Probe", 0.1 * Units_Factor(UNIT_INCHES, globalDoc->getSettings().getPageUnits()), "100", drillSize);
   //      DataStruct *data = Graph_Block_Reference(probeGeom->getName(), modifiedRefname, 0, x, y, 0, mirror, 1, cur_layer, TRUE);
   //      data->getInsert()->setInsertType(insertTypeTestProbe);

   //      globalDoc->SetAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, netname.GetBuffer(0), attributeUpdateOverwrite, NULL);

			//// Add probe number as REFNAME, offset X & Y to 1/4 of diameter, height is 1/2 of diameter and width is 1/4 of diameter,
   //      globalDoc->SetVisAttrib(&data->getAttributesRef(), globalDoc->IsKeyWord(ATT_REFNAME, 0), valueTypeString, refname.GetBuffer(0),
   //               drillSize*0.25, drillSize*0.25, 0.0, drillSize*0.5, drillSize*0.25, 1, 0,  TRUE, attributeUpdateOverwrite,
			//			0L, Graph_Level("PROBE_TOP", "", FALSE), FALSE, horizontalPositionLeft, verticalPositionBaseline); 
		}
      else
      {
         double x1, y1, rot, h, w, penw;
         int    mirror = 0;
         char   prosa[255];

         //h = char height, w = char width
         do_text(&x1, &y1, &rot, &mirror, &h, &w, &penw,  prosa);

         x1 = x1 - sym_x;
         y1 = y1 - sym_y;
			rot = rot - sym_rot;

         if (sym_rot)
         {
            double x = x1, y=y1;
            Rotate(x,y, 360-sym_rot,&x1, &y1);
         }
         if (sym_mirror)
         {
            // change layer
            x1 = -x1;
            mirror = !mirror;
         }

         // dependend on class_name, convert it to text or attribute
         // returns a NULL if text is empty
         if (graphic_class == COMPHEIGHT_CLASS)
         {
            if (strlen(prosa) && map != NULL)
            {
               double height = atof(prosa) * scale_factor;
               globalDoc->SetAttrib(map, globalDoc->IsKeyWord(ATT_COMPHEIGHT, 0), valueTypeUnitDouble, &height, attributeUpdateOverwrite, NULL);
            }
         }
         else if (strlen(prosa))
         {
            CString CompName;
            if (i = get_label(VAL_REFDES))            
               CompName = recordArray[i-1]->tok;

            if (CompName.CompareNoCase(prosa))  // if the text(prosa) is the component name, don't put the component name in the geometry cause
                                       // the geometry is being used in many different components.  
            {           
               int err;
               int penwidthapp = Graph_Aperture("", T_ROUND, penw, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE, &err);

               d = Graph_Text(cur_layer, prosa, x1, y1, h,w, DegToRad(rot), flg, FALSE, mirror, 0, FALSE, penwidthapp, 0); // prop flag, mirror flag, oblique angle in deg
            }
         }
      }
   }
   else if (!STRCMPI(gname, "ARC"))
   {
      double x1, y1, x2, y2, xc, yc, rad, r, sa, da;
      int    widthindex, direction, fill = FALSE;
      int    grtype, close = FALSE, gvoid = FALSE;

      do_arc(&x1,&y1,&x2,&y2,&xc,&yc,&rad,&widthindex,&direction,&grtype);

      // here normalize
      x1 = x1 - sym_x;
      y1 = y1 - sym_y;
      x2 = x2 - sym_x;
      y2 = y2 - sym_y;
      xc = xc - sym_x;
      yc = yc - sym_y;
      if (sym_rot)
      {
         double x = x1, y=y1;
         Rotate(x,y, 360-sym_rot,&x1, &y1);
         x = x2, y = y2;
         Rotate(x,y, 360-sym_rot,&x2, &y2);
         x = xc, y = yc;
         Rotate(x,y, 360-sym_rot,&xc, &yc);
      }
      if (sym_mirror)
      {
         // change layer
         x1 = -x1;
         x2 = -x2;
         xc = -xc;
         direction = !direction;
      }

      // do not chage the coos, because if arc is part of poly, we can keep it.
      ArcCenter2(x1, y1, x2, y2, xc, yc, &r, &sa, &da, FALSE);
      
      if (fabs(da) < ALMOSTZERO) // straight line
         da = 0;
      else if (direction)
         da = da - PI2;
      
      if (minj == 1)
      {
         d = Graph_PolyStruct(cur_layer, 0L, FALSE);
         d->setGraphicClass(graphic_class);
         lastvertex = NULL;
      }

      old_widthindex = widthindex;

      if (grtype == VAL_SHAPE)
      {
         fill = TRUE;
         close = TRUE;
      }
      if (grtype == VAL_VOID)
      {
         fill = TRUE;
         close = TRUE;
         gvoid = TRUE;
      }
      
      // make a new poly if
      // start minj == 1 or
      // start of void shape,
      if (lastvertex == NULL || minj == 1 || (littlej && littlej != lastlittlej))
      {
         Graph_Poly(NULL,widthindex, fill, gvoid,close);
         lastvertex = Graph_Vertex(x1, y1, 0.0);
      }
      lastlittlej = littlej;

      if (fabs(x1-x2) < SMALLNUMBER && fabs(y1-y2) < SMALLNUMBER) // full circle
      {
         lastvertex->bulge = 1;
         double rx = xc - x1;
         double ry = yc - y1;
         lastvertex = Graph_Vertex(xc + rx, yc + ry, 1);
         lastvertex = Graph_Vertex(x2, y2, 0);
      }
      else
      {
         double bulge = tan(da/4);
         lastvertex->bulge = (DbUnit)bulge;
         lastvertex = Graph_Vertex(x2, y2, 0.0);
      }
   }
   else
   {
      // unknown Graphic Data
      CString tmp;
      tmp.Format("Unknown Graphic Data Name [%s]",gname);
      // lab_error(tmp);
      return 0;
   }

   // d can be null if i.e a text has no prosa 
   if (d == NULL)
      return 1;

   if (minj != 1)
      return 1;   // only add attributes to the first entity

   // if not text but a netname ....
   if (STRCMPI(gname,"TEXT") && strlen(netname))
      globalDoc->SetAttrib(&d->getAttributesRef(), globalDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, (char *)netname, attributeUpdateOverwrite, NULL);

   // here add attributes
   for (i=0;i<commandArrayCount;i++)
   {
		CString keyword = get_attrmap(commandArray[i]->tok);

      if (!STRCMPI(keyword, "CUTOUT"))
         continue; // used above to make panel or board cutout.

      if (is_attrib(keyword, COMP_ATT) || is_attrib(keyword, TYPE_ATT))
         continue;

		CString value = recordArray[i]->tok;
      if (recordArray[i]->label == 0 && strlen(value))
         if (minj == 1) // update polystructure with current attributes
			{
            do_attrib(&d->getAttributesRef(), keyword, valueTypeString, value, 0, 0);
				
				// if GEOMATTR from .in file, float attrib up to geometry
				if (is_attrib(keyword, GEOM_ATT) && map != NULL)
	            do_attrib(map, keyword, valueTypeString, value, 0, 0);
			}
   }

   return 1;
}

/******************************************************************************
* do_attgraphic
   Attention : If you return from this function make sure that set_pcb is done

   write_prosa is used for attribute text, which may only be used as a place holder, such as REFDES
   Also takes care of visibility

*/
static int do_attgraphic(CAttributes** map, double sym_x, double sym_y, double sym_rot, int sym_mirror, int majj, int minj,
      int littlej, int graphic_class, int calc_ext, const char *layerName, int write_prosa, char *keyword)
{
   char  gname[20];
   int   i;
   DbFlag   flg = 0;
   
   if (!(i=get_label(VAL_GRAPHIC_DATA_NAME)))
   {
      lab_error("GRAPHIC_DATA_NAME expected");
      return 0;
   }
   strcpy(gname,recordArray[i-1]->tok);

   if (!STRCMPI(gname,"LINE"))
   {
      messageFilter->formatMessage("LINE ignored in CLASS_2_ATTR record");
      display_error++;
   }
   else
   if (!STRCMPI(gname,"RECTANGLE"))
   {
      messageFilter->formatMessage("RECTANGLE ignored in CLASS_2_ATTR record");
      display_error++;
   }
   else
   if (!STRCMPI(gname,"TEXT"))
   {
      double x1,y1,rot, h,w, penw;
      int    mirror;
      char   prosa[255];

      //h = char height, w = char width
      do_text(&x1, &y1, &rot, &mirror, &h, &w, &penw, prosa);

      if (!write_prosa)
      {
         strcpy(prosa, "");
      }

      x1 = x1 - sym_x;
      y1 = y1 - sym_y;
      rot = rot - sym_rot;
      if (sym_rot)
      {
         double x = x1, y=y1;
         Rotate(x,y, 360-sym_rot,&x1, &y1);
      }
      if (sym_mirror)
      {
         // change layer
         x1 = -x1;
         mirror = ~mirror;
      }

      if (graphic_class == REFDES_CLASS)
      {
         int laynr = Graph_Level("REFNAME_TOP", "", 0);
         globalDoc->SetVisAttrib(map,globalDoc->IsKeyWord(ATT_REFNAME, 0),
            valueTypeString,
            prosa,
            x1, y1, DegToRad(rot), h, w, 1, 0, TRUE, attributeUpdateOverwrite, 0, laynr, 0, 0, 0);
      }
      else
      {
         // here need to check if this a an existing attribute.
         int laynr = Graph_Level(layerName, "", 0);
         globalDoc->SetUnknownVisAttrib(map, get_attrmap(keyword),   prosa,
               x1, y1, DegToRad(rot), h, w, 1, 0, TRUE, attributeUpdateOverwrite, 0, laynr, 0, 0, 0);
      }
   }
   else
   if (!STRCMPI(gname,"ARC"))
   {
      messageFilter->formatMessage("ARC ignored in CLASS_2_ATTR record");
      display_error++;
   }
   else
   {
      // unknown Graphic Data
      CString tmp;
      tmp.Format("Unknown Graphic Data Name [%s]",gname);
      // lab_error(tmp);
      return 0;
   }

   return 1;
}

/******************************************************************************
* do_line
*/
static int do_line(double *x1,double *y1, double *x2, double *y2, int *widthindex, int *grtype)
{
   double w;
   int    err, i;

   // data_1   x1
   if (!(i=get_label(VAL_GRAPHIC_DATA_1)))
   {
      lab_error("GRAPHIC_DATA_1 expected");
      return 0;
   }
   *x1 = atof(recordArray[i-1]->tok) * scale_factor;

   // data_2   y1
   if (!(i=get_label(VAL_GRAPHIC_DATA_2)))
   {
      lab_error("GRAPHIC_DATA_2 expected");
      return 0;
   }
   *y1 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_3   x2
   if (!(i=get_label(VAL_GRAPHIC_DATA_3)))
   {
      lab_error("GRAPHIC_DATA_3 expected");
      return 0;
   }
   *x2 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_4   y2
   if (!(i=get_label(VAL_GRAPHIC_DATA_4)))
   {
      lab_error("GRAPHIC_DATA_4 expected");
      return 0;
   }
   *y2 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_5   w
   if (!(i=get_label(VAL_GRAPHIC_DATA_5)))
   {
      lab_error("GRAPHIC_DATA_5 expected");
      return 0;
   }
   w = atof(recordArray[i-1]->tok) * scale_factor;

   if (w == 0)
      *widthindex = 0;  // make it small width.
   else
      *widthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   *grtype = 0;
   if (AllegroExtractType != ALLEGRO_FABMASTER)
   {
      // data_10
      if (!(i=get_label(VAL_GRAPHIC_DATA_10)))
      {
         lab_error("GRAPHIC_DATA_10 expected");
         return 0;
      }

      if (!STRICMP("CONNECT",recordArray[i-1]->tok))
         *grtype = VAL_CONNECT;
      else
      if (!STRICMP("NOTCONNECT",recordArray[i-1]->tok))
         *grtype = VAL_NOTCONNECT;
      else
      if (!STRICMP("SHAPE",recordArray[i-1]->tok))
         *grtype = VAL_SHAPE;
      else     
      if (!STRICMP("VOID",recordArray[i-1]->tok))
         *grtype = VAL_VOID;
      else
      if (!STRICMP("POLYGON",recordArray[i-1]->tok))
         *grtype = VAL_POLYGON;
   }

   return 1;
}

/******************************************************************************
* do_rectangle
*/
static int do_rectangle(double *x1,double *y1, double *x2, double *y2, int *fill)
{
   int    i;

   // data_1   x1
   if (!(i=get_label(VAL_GRAPHIC_DATA_1)))
   {
      lab_error("GRAPHIC_DATA_1 expected");
      return 0;
   }
   *x1 = atof(recordArray[i-1]->tok) * scale_factor;

   // data_2   y1
   if (!(i=get_label(VAL_GRAPHIC_DATA_2)))
   {
      lab_error("GRAPHIC_DATA_2 expected");
      return 0;
   }
   *y1 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_3   x2
   if (!(i=get_label(VAL_GRAPHIC_DATA_3)))
   {
      lab_error("GRAPHIC_DATA_3 expected");
      return 0;
   }
   *x2 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_4   y2
   if (!(i=get_label(VAL_GRAPHIC_DATA_4)))
   {
      lab_error("GRAPHIC_DATA_4 expected");
      return 0;
   }
   *y2 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_5   w
   if (!(i=get_label(VAL_GRAPHIC_DATA_5)))
   {
      lab_error("GRAPHIC_DATA_5 expected");
      return 0;
   }
   *fill = atoi(recordArray[i-1]->tok);

   return 1;
}

/******************************************************************************
* do_text
*/
static int do_text(double *x,double *y, double *rot, int *mirror, double *h,double *charw, double *penw, char *prosa)
{
   int   i;
   char  *lp;
   char  tmp[80];
   int   just = 0;

   // data_1   x
   if (!(i=get_label(VAL_GRAPHIC_DATA_1)))
   {
      lab_error("GRAPHIC_DATA_1 expected");
      return 0;
   }
   *x = atof(recordArray[i-1]->tok) * scale_factor;

   // data_2   y
   if (!(i=get_label(VAL_GRAPHIC_DATA_2)))
   {
      lab_error("GRAPHIC_DATA_2 expected");
      return 0;
   }
   *y = atof(recordArray[i-1]->tok) * scale_factor;
   // data_3   rot
   if (!(i=get_label(VAL_GRAPHIC_DATA_3)))
   {
      lab_error("GRAPHIC_DATA_3 expected");
      return 0;
   }
   *rot = atof(recordArray[i-1]->tok);
   // data_4   mirror
   if (!(i=get_label(VAL_GRAPHIC_DATA_4)))
   {
      lab_error("GRAPHIC_DATA_4 expected");
      return 0;
   }
   *mirror = FALSE;
   *mirror = !STRICMP("YES",recordArray[i-1]->tok);

   // data_5   justification
   if (!(i=get_label(VAL_GRAPHIC_DATA_5)))
   {
      lab_error("GRAPHIC_DATA_5 expected");
      return 0;
   }
   // is is always bottom of the text
   if (!STRICMP("LEFT",recordArray[i-1]->tok))
      just = GRTEXT_W_L;
   else
   if (!STRICMP("CENTER",recordArray[i-1]->tok))
      just = GRTEXT_W_C;
   else
   if (!STRICMP("RIGHT",recordArray[i-1]->tok))
      just = GRTEXT_W_R;

   // data_6   fontdata
   if (!(i=get_label(VAL_GRAPHIC_DATA_6)))
   {
      lab_error("GRAPHIC_DATA_6 expected");
      return 0;
   }
   strcpy(tmp,recordArray[i-1]->tok);
   if ((lp = strtok(tmp," \t")) != NULL)  // size
   {
      
   }

   if ((lp = strtok(NULL," \t")) != NULL) // font
   {
      
   }

   if ((lp = strtok(NULL," \t")) != NULL) // height
   {
      *h = atof(lp) * scale_factor;    
   }

   if ((lp = strtok(NULL," \t")) != NULL) // char width
   {
      *charw = atof(lp) * scale_factor;      
   }

   if ((lp = strtok(NULL," \t")) != NULL) // slant
   {
   }
   if ((lp = strtok(NULL," \t")) != NULL) // character spacing
   {
   }
   if ((lp = strtok(NULL," \t")) != NULL) // line spacing
   {
   }
   if ((lp = strtok(NULL," \t")) != NULL) // pen / stroke width
   {
      *penw = atof(lp) * scale_factor;    
   }

   // data_7   text
   if (!(i=get_label(VAL_GRAPHIC_DATA_7)))
   {
      lab_error("GRAPHIC_DATA_7 expected");
      return 0;
   }
   strcpy(prosa,recordArray[i-1]->tok);

   // normalize text - debugging
   double r= *rot, th = *h, cw = *charw, tx = *x, ty = *y;  
   int    m= *mirror;
   normalize_text(&tx, &ty, just, r, m, th, cw * strlen(prosa));
   *x = tx;
   *y = ty;
   return 1;
}

/******************************************************************************
* do_arc
*/
static int do_arc(double *x1,double *y1, double *x2, double *y2, double *xc, double *yc, double *rad, int *widthindex,
      int *direction, int *grtype)
{
   double w;
   int    err, i;

   *direction = FALSE; // counter clock
   // data_1   x1
   if (!(i=get_label(VAL_GRAPHIC_DATA_1)))
   {
      lab_error("GRAPHIC_DATA_1 expected");
      return 0;
   }
   *x1 = atof(recordArray[i-1]->tok) * scale_factor;

   // data_2   y1
   if (!(i=get_label(VAL_GRAPHIC_DATA_2)))
   {
      lab_error("GRAPHIC_DATA_2 expected");
      return 0;
   }
   *y1 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_3   x2
   if (!(i=get_label(VAL_GRAPHIC_DATA_3)))
   {
      lab_error("GRAPHIC_DATA_3 expected");
      return 0;
   }
   *x2 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_4   y2
   if (!(i=get_label(VAL_GRAPHIC_DATA_4)))
   {
      lab_error("GRAPHIC_DATA_4 expected");
      return 0;
   }
   *y2 = atof(recordArray[i-1]->tok) * scale_factor;
   // data_5   xc
   if (!(i=get_label(VAL_GRAPHIC_DATA_5)))
   {
      lab_error("GRAPHIC_DATA_5 expected");
      return 0;
   }
   *xc = atof(recordArray[i-1]->tok) * scale_factor;
   // data_6   yc
   if (!(i=get_label(VAL_GRAPHIC_DATA_6)))
   {
      lab_error("GRAPHIC_DATA_6 expected");
      return 0;
   }
   *yc = atof(recordArray[i-1]->tok) * scale_factor;
   // data_4   rad
   if (!(i=get_label(VAL_GRAPHIC_DATA_7)))
   {
      lab_error("GRAPHIC_DATA_7 expected");
      return 0;
   }
   *rad = atof(recordArray[i-1]->tok) * scale_factor;
   // data_8   w
   if (!(i=get_label(VAL_GRAPHIC_DATA_8)))
   {
      lab_error("GRAPHIC_DATA_8 expected");
      return 0;
   }
   w = atof(recordArray[i-1]->tok) * scale_factor;

   if (w == 0)
      *widthindex = 0;  // make it small width.
   else
      *widthindex = Graph_Aperture("", T_ROUND, w , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   // data_9
   if (!(i=get_label(VAL_GRAPHIC_DATA_9)))
   {
      lab_error("GRAPHIC_DATA_9 expected");
      return 0;
   }
   if (!STRICMP("CLOCKWISE",recordArray[i-1]->tok))
      *direction = TRUE;
   
   *grtype = 0;
   if (AllegroExtractType != ALLEGRO_FABMASTER)
   {
      // data_10
      if (!(i=get_label(VAL_GRAPHIC_DATA_10)))
      {
         lab_error("GRAPHIC_DATA_10 expected");
         return 0;
      }

      if (!STRICMP("CONNECT",recordArray[i-1]->tok))
         *grtype = VAL_CONNECT;
      else
      if (!STRICMP("NOTCONNECT",recordArray[i-1]->tok))
         *grtype = VAL_NOTCONNECT;
      else
      if (!STRICMP("SHAPE",recordArray[i-1]->tok))
         *grtype = VAL_SHAPE;
      else     
      if (!STRICMP("VOID",recordArray[i-1]->tok))
         *grtype = VAL_VOID;
      else
      if (!STRICMP("POLYGON",recordArray[i-1]->tok))
         *grtype = VAL_POLYGON;
   }

   return 1;
}

/******************************************************************************
* get_attrmap
*/
static const char* get_attrmap(const char *a)
{
   for (int i=0; i<attrmapcnt; i++)
   {
      if (attrmaparray[i]->allegro_name.CompareNoCase(a) == 0)
         return attrmaparray[i]->cc_name.GetBuffer(0);
   }

   return a;
}

/******************************************************************************
* assign_padlayertypes
*/
static int assign_padlayertypes(int c_l)
{
   CString  lay;

   LayerStruct *l = globalDoc->FindLayer(c_l);
   if (l == NULL) return 1; // could have been deleted.

   lay = l->getName();
   if (lay.CompareNoCase("internal_pad_def") == 0)
      l->setLayerType(LAYTYPE_PAD_INNER);       
   else
   if (lay.CompareNoCase("~TSM") == 0)
   {
         l->setLayerType(LAYTYPE_MASK_TOP);
      l->setComment("Solder Mask Top");
      l->setVisible(false);
   }
   else
   if (lay.CompareNoCase("~BSM") == 0)
   {
      l->setLayerType(LAYTYPE_MASK_BOTTOM);
      l->setComment("Solder Mask Bottom");
      l->setVisible(false);
   }
   else
   if (lay.CompareNoCase("~TPM") == 0)
   {
      l->setLayerType(LAYTYPE_PASTE_TOP);
      l->setComment("Paste Mask Top");
      l->setVisible(false);
   }
   else
   if (lay.CompareNoCase("~BPM") == 0)
   {
      l->setLayerType(LAYTYPE_PASTE_BOTTOM);
      l->setComment("Paste Mask Bottom");
      l->setVisible(false);
   }
   else
   if (lay.CompareNoCase("~BFM") == 0)    // ?? layers
      l->setVisible(false);
   else
   if (lay.CompareNoCase("~TFM") == 0)    // ?? layers
      l->setVisible(false);

   return 1;
}

/******************************************************************************
* do_attrib
*/
static int do_attrib(CAttributes** map,const char *keyw, int typ, const char *val0, double val1, int val2)
{
   globalDoc->SetUnknownAttrib(map, keyw, val0, attributeUpdateOverwrite, NULL);

   return 1;
}

/****************************************************************************/
/*
*/
static void clean_prosa(char *p)
{
   if (strlen(p) == 0)  return;

   // delete all traling isspace
   while (strlen(p) && isspace(p[strlen(p)-1])) 
         p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')   p[strlen(p)-1] = '\0';

   STRREV(p);
   while (strlen(p) && isspace(p[strlen(p)-1])) 
         p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')   p[strlen(p)-1] = '\0';
   STRREV(p);
   return;
}

/****************************************************************************/
/*
*/
static double   cnv_tok(char *l)
{
    double x;

    if (l == NULL)
    {
      return 0.0;
    }

    if (l && strlen(l))
    {
        x = atof(l);
    }
    else
        return 0.0;

    return x * scale_factor;
}

/****************************************************************************/
/*
*/
static int do_circle(int lay, int widthindex)
{
   double   x1,y1,r;
   char     *lp;

   if ((lp = strtok(NULL," \r\t\n")) == NULL)  return 0;
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \r\t\n")) == NULL)  return 0;
   y1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \r\t\n")) == NULL)  return 0;
   r = cnv_tok(lp);
   Graph_Circle(lay,x1,y1,r, 0L, widthindex , FALSE, TRUE); 

   return 1;
}

/******************************************************************************
* update_attr
*/
static int update_attr(const char *lp, int typ)
{
   if (attrcnt < MAX_ATTR)
   {
      if ((attr[attrcnt].name = STRDUP(lp)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
      attr[attrcnt].atttype = typ;
      attrcnt++;
   }
   else
   {
      // error
      messageFilter->formatMessage("Too many attributes");
      display_error++;
   }
   return attrcnt-1;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(CString layerName)
//{
//   for (int i=0; i<MAX_LAYTYPE; i++)
//      if (!layerName.CompareNoCase(layerTypeToString(i)))
//         return i;
//
//   return LAYTYPE_UNKNOWN; 
//}

/******************************************************************************
* get_lay_index
*/
static int get_lay_index(CString layerName)
{
   for (int i=0; i<layer_attr_cnt; i++)
   {
      if (!layerName.CompareNoCase(layer_attr[i].name))
         return i;
   }

   if (layer_attr_cnt < MAX_LAYERS)
   {
      layer_attr[layer_attr_cnt].name = layerName;
      layer_attr[layer_attr_cnt].attr = 0;
      layer_attr_cnt++;
   }
   else
   {
      messageFilter->formatMessage("Too many layers");
      return -1;
   }

   return layer_attr_cnt-1;
}


/******************************************************************************
* read_line
   return 1 on ok
          0 on eof
*/
static int read_line(FILE *fp, char *line, int len)
{
	CString inputLine;
	for (int i=0; ((i < len-1) && (feof(fp) == 0)); i++)
	{
		int ch = fgetc(fp);
		if (ch == 0 || ch == '\n' || ch == 10 || ch == 13)
		{
			if (i > 0)
				break;
			else
				continue;
		}

		inputLine.AppendChar((char)ch);
	}

	inputLine.Trim();
	strcpy(line, inputLine);
	line[inputLine.GetLength()] = '\0';
	line_cnt++;

	inl = line;

	return(strlen(line));
}

/******************************************************************************
* get_token
   read until !
*/
static int get_token()
{
   int   cnt = 0;

   if (*inl == '\0') return 0;

   while (*inl != '!' && *inl != '\n' && *inl != '\0')
   {
      if (cnt >= 0 && cnt < MAX_TOKEN)
         token[cnt++] = *(inl++);
      else
         return 0;
   }
   inl++;
	if (cnt >= 0 && cnt < MAX_TOKEN)
      token[cnt] = '\0';

   return 1;
}

/******************************************************************************
* p_error
*/
static void p_error()
{
   CString err;
   err.Format("Token expected at %ld", line_cnt);
   // logical file end error - eof before $end???
   ErrorMessage(err, "ERROR", MB_OK | MB_ICONHAND);
}

/******************************************************************************
* 
   Non fatal error
*/
static void lab_error(const char *l)
{
   messageFilter->formatMessage("%s in [%s] at %ld",l, cur_filename, line_cnt);
   display_error++;
}

/******************************************************************************
* load_commandbuffer
*/
static void load_commandbuffer()
{
   while (get_token())
   {
      Command *command = new Command;
      command->tok = token; 
      command->compressedTok = token;
      command->compressedTok.Remove('_');  // Strip underscores in command name
      command->label = 0;
      commandArray.SetAtGrow(commandArrayCount++, command);
   }
}

/******************************************************************************
* load_recordbuffer
*/
static void load_recordbuffer()
{
   while (get_token())
   {
      Record *record = new Record;
      record->tok = token; 
      record->label = 0;
      recordArray.SetAtGrow(recordArrayCount++, record);
   }
}

/******************************************************************************
* get_label
*/
static int get_label(int label)
{
   for (int i=0; i<recordArrayCount; i++)
   {
      Record *record = recordArray[i];

      if (record->label == label)
         return i+1; 
   }

   return 0;
}

/******************************************************************************
* label_recordbuffer
   Assign every field a RecognizedCommandAry index;
*/
static int label_recordbuffer()
{
   if (!commandArrayCount)
      return 0;

   // here assign labels
   // OBSERVATION - This loop is wasting time. The command buffer should already
   // be labeld, so all that should be needed here is to match field positions
   // and assign the command's index, i.e. a single loop and no string compares.
   // I think this could be made faster, but have to investigate all paths
   // to here to make sure command array truly is set up first.
	int i=0;
   for (i=0; i<commandArrayCount; i++)
   {
      for (int j = 0; j < SIZ_CMD_ARY; j++)
      {
         if (!STRCMPI(commandArray[i]->compressedTok, RecognizedCommandAry[j].token))
         {
            recordArray[i]->label = RecognizedCommandAry[j].index;
            break;
         }
      }
   }

   if (commandArrayCount != recordArrayCount)
   {
      CString  tmp;

      if (recordArrayCount > commandArrayCount)
      {

         // if it is a text - fix it and continue.
         // graphic data 7 is the textstring
         // if record buffer is greater, it counts the number of ! in a text.

         // this can happen if there is an error, but this also can happen if there is a text
         // this has a ! as part of it.
         if (i=get_label(VAL_GRAPHIC_DATA_NAME))
         {
            CString  gname;
            gname = recordArray[i-1]->tok;
            if (gname.Compare("TEXT") == 0)
            {
               //only do it on text
               if (i=get_label(VAL_GRAPHIC_DATA_7))
               {
                  int   cnt;
                  // here make text correct
                  gname = recordArray[i-1]->tok;

                  for (cnt = 0; cnt < (recordArrayCount-commandArrayCount); cnt++)
                  {
                     gname = recordArray[i+cnt]->tok;
                     recordArray[i-1]->tok += gname;
                  }

                  // this is the fixed up text
                  //gname = recordArray[i-1]->tok;

                  for (cnt=i;cnt <commandArrayCount; cnt++)
                  {
                     gname = recordArray[cnt+(recordArrayCount-commandArrayCount)]->tok;
                     recordArray[cnt]->tok = gname;
                  }
                  return 1;
               }
            }
         }
      }

      tmp.Format("Corrupt file : Commands [%d] != Records [%d] in file [%s] at line %ld",
            commandArrayCount,recordArrayCount, cur_filename, line_cnt);
      messageFilter->formatMessage(tmp);
      display_error++;
      return 0;
   }

   return 1;
}

/******************************************************************************
* *get_attributename
*/
static char *get_attributename(int a)
{
   if (a == PANEL_ATT)     return "Panel Attribute";
   if (a == COMP_ATT)      return "Component Attribute";
   if (a == PANEL_SYMBOL)  return "Panel Symbol";
   if (a == BOARD_SYMBOL)  return "Board Symbol";
   if (a == TYPE_ATT)      return "Type Attribute";

   return "Unknown";
}

/******************************************************************************
* label_commandbuffer
   Assign every field aval_lst index;
*/
static int label_commandbuffer()
{
   if (!commandArrayCount)
      return 0;

   // here assign labels
	int i = 0;
   for (i = 0; i < commandArrayCount; i++)
   {
      for (int j = 0; j < SIZ_CMD_ARY;j++)
      {
         if (!STRCMPI(commandArray[i]->compressedTok, RecognizedCommandAry[j].token))
         {
            commandArray[i]->label = RecognizedCommandAry[j].index;
            break;
         }
      }
   }

   // now, this unknown records can be userdefined attributes.
   int found = 0;
   for (i=0; i<commandArrayCount; i++)
   {
      int ii;
      for (ii=0;ii<attrcnt;ii++)
      {
         if (!STRICMP(attr[ii].name, commandArray[i]->tok) && strlen(commandArray[i]->tok))
         {
            if (found == 0)
               messageFilter->formatMessage("List of known EXTRACT Attribute records in [%s]:", cur_filename);
            found++;
            messageFilter->formatMessage("   Field [%s] known as [%s] Attribute", commandArray[i]->tok, get_attributename(attr[ii].atttype));
            display_error++;
            commandArray[i]->label = -1;
            break;
         }
      }
   }

   found = 0;
   for (i=0;i<commandArrayCount;i++)
   {
      if (commandArray[i]->label == 0 && strlen(commandArray[i]->tok))
      {
         if (found == 0)
            messageFilter->formatMessage("List of unknown EXTRACT records in [%s]:", cur_filename);
         found++;
         messageFilter->formatMessage(" Record [%s] unknown",commandArray[i]->tok);
         display_error++;
      }
   }

   return 1;
}

/******************************************************************************
* update_boardext
*/
static void update_boardext(double x, double y)
{
   // calc extens
   if (!cur_boardext.extent_found)
   {  
      cur_boardext.minx = x;
      cur_boardext.miny = y;
      cur_boardext.maxx = x;
      cur_boardext.maxy = y;
      cur_boardext.extent_found = TRUE;
   }
   else
   {
      cur_boardext.minx = min(x,cur_boardext.minx);
      cur_boardext.maxx = max(x,cur_boardext.maxx);

      cur_boardext.miny = min(y,cur_boardext.miny);
      cur_boardext.maxy = max(y,cur_boardext.maxy);
   }
}
         
/******************************************************************************
* is_attrib
*/
static int is_attrib(const char *s, int atttype)
{
	// This was changed to fix case #959 and also refixed case #875

   for (int i=0; i<attrcnt; i++)
   {
      // wildcard_compare to get * going.
      if (attr[i].atttype == atttype && wildcmp(attr[i].name, s))
         return 1;
   }


   return 0;
}

/******************************************************************************
* get_attrib
*/
static int get_attrib(const char *s)
{
	// This was changed to fix case #959 and also refixed case #875

   for (int i=0; i<attrcnt; i++)
	{
		if (wildcmp(attr[i].name, s))
			return i;
	}

   return -1;
}

/******************************************************************************
* is_primarray
*/
static int is_primarray(CString classString, CString subClassString, int typ)
{
   for (int i=0; i<primarraycnt; i++)
   {
      ALLEGROPrimout *prim = primarray[i];

      if (prim->Class == classString && prim->SubClass == subClassString && prim->typ == typ)
         return 1;
   }

   return 0;
}


/******************************************************************************
* master_block
  This name must be unique, name_x_y_rot, or name_refname because there can be 2 DIP14 
  placed after each other !

   The master is a block, which is 
      1. not in the master list
      2. not during the same definition
*/
static int master_block(const CString geomName, const CString refName)
{
	if (!strcmp(cur_block, geomName))
		return 1;


	POSITION pos = masterList.GetHeadPosition();
	while (pos)
	{
		CAllegroMaster* pMaster = masterList.GetNext(pos);
		if (pMaster == NULL)
			continue;

		if (pMaster->GetGeomName().CompareNoCase(geomName) == 0)
			return 0;
	}

	CAllegroMaster* pMaster = new CAllegroMaster(geomName, refName, false);
	masterList.AddTail(pMaster);

   strcpy(cur_block, geomName);
   return 1;
}

/******************************************************************************
* setMasterPinedited
*/
static void setMasterPinedited(const CString geometryName, const CString refname)
{
	POSITION pos = masterList.GetHeadPosition();
	while (pos)
	{
		CAllegroMaster* pMaster = masterList.GetNext(pos);
		if (pMaster == NULL)
			continue;

		if (pMaster->GetGeomName().CompareNoCase(geometryName) != 0)
			continue;

		if (pMaster->GetRefName().CompareNoCase(refname) == 0)
		{
			pMaster->SetPinEdited(true);
			return;
		}
	}
}

/******************************************************************************
* InMasterList
*/
static bool InMasterList(const CString geometryName, const CString refName)
{
	POSITION pos = masterList.GetHeadPosition();
	while (pos)
	{
		CAllegroMaster* pMaster = masterList.GetNext(pos);
		if (pMaster == NULL)
			continue;

		if (pMaster->GetGeomName().CompareNoCase(geometryName) != 0)
			continue;

		if (pMaster->GetRefName().CompareNoCase(refName) == 0)
			return true;
		else if (pMaster->IsPinEdited())
		{
			pMaster->SetRefName(refName);
			pMaster->SetPinEdited(false);
			return true;
		}
		else
			return false;
	}

	CAllegroMaster* pMaster = new CAllegroMaster(geometryName, refName, false);
	masterList.AddTail(pMaster);

	strcpy(cur_block, geometryName);
	return TRUE;
}

/******************************************************************************
* get_graphic_subclass
*/
static int get_graphic_subclass(CString subclassString)
{
   if (!STRCMPI(subclassString, "PLACE_BOUND_TOP"))      
      return GR_CLASS_COMPONENT_BOUNDARY;
   if (!STRCMPI(subclassString, "PLACE_BOUND_BOTTOM"))      
      return GR_CLASS_COMPONENT_BOUNDARY;
 
   return 0;
}

/******************************************************************************
* get_graphic_class
*/
static int get_graphic_class(CString classString, BOOL NetnameIsEmpty)
{
   if (!classString.CompareNoCase("ETCH") && !NetnameIsEmpty)              
      return GR_CLASS_ETCH;
   if (!classString.CompareNoCase("ANTI ETCH")) 
      return GR_CLASS_ANTI_ETCH;
   if (!classString.CompareNoCase("BOARD GEOMETRY"))  
      return GR_CLASS_BOARD_GEOM;
   if (!classString.CompareNoCase("MANUFACTURING"))      
      return graphicClassManufacturing;
   if (!classString.CompareNoCase("ROUTE KEEPIN"))    
      return GR_CLASS_ROUTKEEPIN;
   if (!classString.CompareNoCase("ROUTE KEEPOUT"))      
      return GR_CLASS_ROUTKEEPOUT;
   if (!classString.CompareNoCase("VIA KEEPOUT"))     
      return GR_CLASS_VIAKEEPOUT;
   if (!classString.CompareNoCase("VIA KEEPIN"))      
      return GR_CLASS_VIAKEEPIN;
   if (!classString.CompareNoCase("PACKAGE KEEPOUT"))    
      return GR_CLASS_PLACEKEEPOUT;
   if (!classString.CompareNoCase("PACKAGE KEEPIN"))     
      return GR_CLASS_PLACEKEEPIN;
   if (!classString.CompareNoCase("DRAWING FORMAT"))     
      return GR_CLASS_DRAWING;

   // this is done with allegro.in
   if (!classString.CompareNoCase("PACKAGE GEOMETRY"))      
      return 0;
   if (!classString.CompareNoCase("REF DES"))      
      return 0;
   if (!classString.CompareNoCase("USER PART NUMBER"))      
      return 0;
   if (!classString.CompareNoCase("DEVICE TYPE"))     
      return 0;
 
   return 0;
}

/****************************************************************************/
/*
*/
static int pad_circle(double w)
{
   int   i;
   for (i=0;i<pd_circlecnt;i++)
   {
      if (pd_circle[i].w == w)
         return i;
   }

   if (pd_circlecnt < MAX_PADSHAPE)
   {
      pd_circle[pd_circlecnt].w = w;
      pd_circlecnt++;
   }
   else
   {
      ErrorMessage("Too many Circles");
   }

   return -1;
}

/******************************************************************************
* get_drillindex
*/
static int get_drillindex(double size, bool plated, int layernum)
{
   if (size == 0)
      return -1;


	for (int i=0; i<drillcnt; i++)
	{
      if (drill[i] == size)
		{
			CString drillname;
			drillname.Format("DRILL_%d", i);

			int toolIndex = Graph_Tool(drillname, 0, size, 0, 0, 0, 0L);
			BlockStruct* drillTool = globalDoc->getWidthBlock(toolIndex);

			if (drillTool->getToolHolePlated() == plated)
				return i;
		}
	}

   CString drillname;
   drillname.Format("DRILL_%d", drillcnt);
   Graph_Tool(drillname, 0, size, 0, 0, 0, 0L, plated?TRUE:FALSE);

   if (drillcnt < MAX_PADSHAPE)
   {
      drill[drillcnt] = size;
      drillcnt++; 
   }
   else
	{
      ErrorMessage("Too many drills");
	}

   return drillcnt-1;
}

/****************************************************************************/
/*
*/
static int pad_rect(double w, double h)
{
   int   i;
   for (i=0;i<pd_rectcnt;i++)
   {
      if (pd_rect[i].w == w && pd_rect[i].h == h)
         return i;
   }

   if (pd_rectcnt < MAX_PADSHAPE)
   {
      pd_rect[pd_rectcnt].w = w;
      pd_rect[pd_rectcnt].h = h;
      pd_rectcnt++;
   }
   else
   {
      ErrorMessage("Too many Rectangles");
   }

   return -1;
}/****************************************************************************/
/*
*/
static int pad_octagon(double w, double h)
{
   int   i;
   for (i=0;i<pd_octagoncnt;i++)
   {
      if (pd_octagon[i].w == w && pd_octagon[i].h == h)
         return i;
   }

   if (pd_octagoncnt < MAX_PADSHAPE)
   {
      pd_octagon[pd_rectcnt].w = w;
      pd_octagon[pd_rectcnt].h = h;
      pd_octagoncnt++;
   }
   else
   {
      ErrorMessage("Too many Octagons");
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pad_square(double w)
{
   int   i;
   for (i=0;i<pd_squarecnt;i++)
   {
      if (pd_square[i].w == w)
         return i;
   }

   if (pd_squarecnt < MAX_PADSHAPE)
   {
      pd_square[pd_squarecnt].w = w;
      pd_squarecnt++;
   }
   else
   {
      ErrorMessage("Too many Squares");
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int pad_obl(double w, double h)
{
   int   i;
   for (i=0;i<pd_oblcnt;i++)
   {
      if (pd_obl[i].w == w && pd_obl[i].h == h)
         return i;
   }

   if (pd_oblcnt < MAX_PADSHAPE)
   {
      pd_obl[pd_oblcnt].w = w;
      pd_obl[pd_oblcnt].h = h;
      pd_oblcnt++;
   }
   else
   {
      ErrorMessage("Too many Oblong");
   }

   return -1;
}

/******************************************************************************
* make_pshapename
*/
static int make_pshapename(char *pshapename, ApertureShapeTag shapeTag, double w, double h)
{
   int p;

   switch (shapeTag)
   {
   case apertureRound:
      if ((p = pad_circle(w)) < 0)
      {
         sprintf(pshapename,"PS_CIRC_%d",pd_circlecnt-1);
         int   err;
         Graph_Aperture(pshapename, T_ROUND, w, 0, 0, 0, 0, 0, BL_APERTURE, TRUE, &err);
      }
      else
      {
         sprintf(pshapename,"PS_CIRC_%d",p);
      }

      break;
   case apertureRectangle:
      if ((p = pad_rect(w,h)) < 0)
      {
         sprintf(pshapename,"PS_RECT_%d",pd_rectcnt-1);
         int   err;
         Graph_Aperture(pshapename, T_RECTANGLE, w, h, 0, 0, 0, 0, BL_APERTURE, TRUE, &err);
      }
      else
      {
         sprintf(pshapename,"PS_RECT_%d",p);
      }

      break;
   case apertureSquare:
      if ((p = pad_square(w)) < 0)
      {
         sprintf(pshapename,"PS_SQUARE_%d",pd_squarecnt-1);
         int   err;
         Graph_Aperture(pshapename, T_SQUARE, w, 0, 0, 0, 0, 0, BL_APERTURE, TRUE, &err);
      }
      else
      {
         sprintf(pshapename,"PS_SQUARE_%d",p);
      }

      break;
   case apertureOctagon:
      if ((p = pad_octagon(w,h)) < 0)
      {
         sprintf(pshapename,"PS_OCTAGON_%d",pd_octagoncnt-1);
         int   err;
         Graph_Aperture(pshapename, T_OCTAGON, w, h, 0, 0, 0, 0, BL_APERTURE, TRUE, &err);
      }
      else
      {
         sprintf(pshapename,"PS_OCTAGON_%d",p);
      }

      break;
   case apertureOblong:
      if ((p = pad_obl(w,h)) < 0)
      {
         sprintf(pshapename, "PS_OBL_%d", pd_oblcnt-1);
         int err;
         Graph_Aperture(pshapename, T_OBLONG, w, h, 0, 0, 0, 0, BL_APERTURE, TRUE, &err);
      }
      else
      {
         sprintf(pshapename, "PS_OBL_%d", p);
      }

      break;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int  update_inserts(CDataList *DataList, const char *bname, int insert_type)
{
   DataStruct *np;
   int         found = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() == T_INSERT)
      {
         BlockStruct *block = globalDoc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
         if (!strcmp(block->getName(),bname))
         {
            np->getInsert()->setInsertType(insert_type);
            found++;
         }
      }
   } // end for

   return found;
}

/******************************************************************************
* update_padshape
*/
static int update_padshape(CString padshapeName)
{
   for (int i=0; i<padshapeNameArrayCount; i++) 
   {
      if (!padshapeNameArray[i].Compare(padshapeName))
         return i;
   }

   if (padshapeNameArrayCount < MAX_PADSHAPE)
      padshapeNameArray.SetAtGrow(padshapeNameArrayCount++, padshapeName);
   else
   {
      messageFilter->formatMessage("Too many Padshapes");    
      display_error++;
   }

   return padshapeNameArrayCount-1; 
}

/******************************************************************************
* is_primrefdes
*/
static int is_primrefdes(const char *lay, const char *class_name)
{
   // class is REF DES
   // subclass = SILKSCREEN_???
   for (int i=0; i<prim_refdescnt; i++)
   {
      if (prim_refdes[i].Class.CompareNoCase(class_name) == 0 && STRNICMP(prim_refdes[i].SubClass,lay, strlen(prim_refdes[i].SubClass)) == 0)
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* is_textcompattr
*/
static int is_textcompattr(const char *lay, const char *class_name, char *attr)
{
   for (int i=0; i<textcompattrcnt; i++)
   {
      if (textcompattr[i].Class.CompareNoCase(class_name) == 0 && STRNICMP(textcompattr[i].SubClass,lay, strlen(textcompattr[i].SubClass)) == 0)
      {
         strcpy(attr, textcompattr[i].Attribute);
         return TRUE;
      }
   }

   return FALSE;
}

/******************************************************************************
* is_primcompheight
*/
static int is_primcompheight(const char *lay, const char *class_name)
{
   for (int i=0; i<prim_compheightcnt; i++)
   {
      if (prim_compheight[i].Class.CompareNoCase(class_name) == 0 && prim_compheight[i].SubClass.CompareNoCase(lay) == 0)
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* is_primboardoutline
*/
static int is_primboardoutline(const char *lay, const char *class_name)
{
   int   i;

   for (i=0;i<prim_boardoutlinecnt;i++)
   {
      if (prim_boardoutline[i].Class.CompareNoCase(class_name) == 0 &&
          prim_boardoutline[i].SubClass.CompareNoCase(lay) == 0)
          return TRUE;
   }
   return FALSE;
}

/******************************************************************************
* is_primpaneloutline
*/
static int is_primpaneloutline(const char *lay, const char *class_name)
{
   for (int i=0; i<prim_paneloutlinecnt; i++)
      if (prim_paneloutline[i].Class.CompareNoCase(class_name) == 0 && prim_paneloutline[i].SubClass.CompareNoCase(lay) == 0)
         return TRUE;

   return FALSE;
}

/******************************************************************************
* is_primcompoutline
*/
static int is_primcompoutline(const char *lay, const char *class_name)
{
   for (int i=0; i<prim_compoutlinecnt; i++)
   {
      if (prim_compoutline[i].Class.CompareNoCase(class_name) == 0 && prim_compoutline[i].SubClass.CompareNoCase(lay) == 0)
         return TRUE;
   }

   return FALSE;
}


/******************************************************************************
* update_comparray
*/
static int update_comparray(const char *refdes, const char *sname, int pin_edited, double x, double y, double rot, int mir)
{
   for (int i=0; i<comparraycnt; i++)
   {
      if (comparray[i]->refname.Compare(refdes) == 0)
      {
         comparray[i]->pin_edited |= pin_edited;
         return i;
      }
   }

   ALLEGROComp *c = new ALLEGROComp;
   comparray.SetAtGrow(comparraycnt++, c);
   c->refname = refdes;
   c->symname = sname;
   c->usedsymname = "";
   c->x = x;
   c->y = y;
   c->rot = rot;
   c->mir = mir;
   c->pin_edited = pin_edited;

   return comparraycnt-1;
}

/******************************************************************************
* UpdatePinEditedGeomArray
   1) if there is no entry in the array for the original geometry name, then create one
   2) add the instance specific geometry created for pin edit component to the map
      if it is not ready there
*/
static void UpdatePinEditGeomArray(CString originalGeomName, BlockStruct *pinEditBlock)
{  
   int i = GetPinEditGeomIndex(originalGeomName);
   if (i > -1)
   {
      pinEditGeomArray[i]->pinEditedGeomMap.SetAt(pinEditBlock->getName(), pinEditBlock);
      return;
   }

   ALLEGROPinEditGeom *pinEditGeom = new ALLEGROPinEditGeom;
   pinEditGeomArray.SetAtGrow(pinEditGeomCnt++, pinEditGeom);
   pinEditGeom->originalGeomName = originalGeomName;
   pinEditGeom->pinEditedGeomMap.RemoveAll();
   pinEditGeom->pinEditedGeomMap.SetAt(pinEditBlock->getName(), pinEditBlock);
}

/******************************************************************************
* GetPinEditGeomIndex
*/
static int GetPinEditGeomIndex(CString originalGeomName)
{
   for (int i=0; i<pinEditGeomCnt; i++)
   {
      ALLEGROPinEditGeom *pinEditGeom = pinEditGeomArray[i];
      if (!pinEditGeom->originalGeomName.CompareNoCase(originalGeomName))
         return i;
   }

   return -1;
}

/******************************************************************************
* get_symbolcnt
*/
static int get_symbolcnt(const char *symname)
{
   int cnt = 0;
   for (int i=0; i<comparraycnt; i++)
   {
      if (!STRCMPI(comparray[i]->symname, symname))
         cnt++;
   }
   return cnt;
}

/******************************************************************************
* update_geomtryANDdevice
*/
static int update_geomtryANDdevice(FileStruct *file, const char *oldBlockName, int oldBlockNum, int newBlockNum)
{
   // Update the devices that are tied to the old geometry to the new geometry
   POSITION pos = file->getTypeList().GetHeadPosition();
   while (pos)
   {
      TypeStruct *type = currentPCBFile->getTypeList().GetNext(pos);
      if (type->getBlockNumber() == oldBlockNum)
         type->setBlockNumber( newBlockNum);
   }
   
   
   int keyindex = globalDoc->IsKeyWord(ATT_DERIVED, TRUE);
   WORD typeListKeyword = globalDoc->IsKeyWord(ATT_TYPELISTLINK, 0);

   // Update all inserts that derived from the old geometry to the new geometry
   for (int i=0; i<comparraycnt; i++)
   {
      ALLEGROComp *allegroComp = comparray[i];

      if (strlen(allegroComp->refname) == 0)
         continue;
      if (allegroComp->symname.Compare(oldBlockName))
         continue;
      if (allegroComp->pin_edited)
         continue;
      allegroComp->pin_edited = -1; // indicate that this will be rename

      DataStruct *data = datastruct_from_refdes(globalDoc, file->getBlock(), allegroComp->refname);
      if (data == NULL) 
         continue;

      // Update to the new geometry number
      if (data->getInsert()->getBlockNumber() != newBlockNum)
         data->getInsert()->setBlockNumber(newBlockNum);

      if (data->getAttributesRef() == NULL)
         continue;

      // Remove the ATT_DERIVED attribute
      if (is_attvalue(globalDoc, data->getAttributesRef(),ATT_DERIVED, 1)) 
         RemoveAttrib(keyindex, &data->getAttributesRef());

      // Find the device and update to the new geometry number
      Attrib* attrib;

      if (data->getAttributesRef()->Lookup(typeListKeyword, attrib))
      {
         if (attrib)
         {
#if CamCadMajorMinorVersion > 406  //  > 4.6
            CString deviceName = attrib->getStringValue();
#else
            CString deviceName = globalDoc->ValueArray[attrib->getStringValueIndex()];
#endif

            TypeStruct *deviceType = FindType(file, deviceName);

            if (deviceType)
               deviceType->setBlockNumber( newBlockNum);
         }
      }
   }

   return 1;
}


/******************************************************************************
* do_master_geom
      All shapes are done shapename_refname. This is because a pinedit could come
      at any time. 
*/
static int do_master_geom(FileStruct *file)
{
   int keyindex = globalDoc->IsKeyWord(ATT_DERIVED, TRUE);

   for (int i=0; i<comparraycnt; i++)
   {
      ALLEGROComp *allegroComp = comparray[i];

      if (allegroComp->pin_edited == -1)
         continue; //pin_edited -1 means it is already renamed.
      if (allegroComp->pin_edited)
         continue; 
      if (strlen(allegroComp->refname) == 0)
         continue;

      // delete original geom for the first time
      BlockStruct *originalBlock = Graph_Block_Exists(globalDoc, allegroComp->symname, -1);
      if (originalBlock == NULL)
         continue;

      BlockStruct *usedBlock = Graph_Block_Exists(globalDoc, allegroComp->usedsymname, -1);
      if (usedBlock == NULL)
         continue;
      usedBlock->setName(originalBlock->getName());
      
      if (is_attvalue(globalDoc, usedBlock->getAttributesRef(), ATT_DERIVED, 1)) 
         RemoveAttrib(keyindex, &usedBlock->getAttributesRef());

      update_geomtryANDdevice(file, originalBlock->getName(), originalBlock->getBlockNumber(), usedBlock->getBlockNumber());
   }

   return 1;
}

/****************************************************************************/
/*
*/
static void set_pcb()
{
   globalDoc->PrepareAddEntity(currentPCBFile);       
   panelmode = FALSE;
}

/****************************************************************************/
/*
*/
static void set_panel()
{
   if (panelFile == NULL)
   {
      // Try to find the panel associated with the current board file
      //CString panelName;
      //panelName.Format("Panel_%s", currentPCBFile->getName());
      //POSITION pos = globalDoc->FileList.GetHeadPosition();
      //while (pos)
      //{
      //   FileStruct *tmpPanel = globalDoc->FileList.GetNext(pos);
      //   if (tmpPanel->getBlockType() == blockTypePanel && !tmpPanel->getName().CompareNoCase(panelName))
      //   {
      //      panelFile = tmpPanel;
      //      break;
      //   }
      //}

      // If no panel is found, then create one
      //if (panelFile == NULL)
      //{
         panelFile = Graph_File_Start(panelName, Type_Allegro_Layout);
         panelFile->setBlockType(blockTypePanel);
         panelFile->getBlock()->setBlockType(blockTypePanel);
      //}
   }

   globalDoc->PrepareAddEntity(panelFile);         
   panelmode = TRUE;
}

/******************************************************************************
* check_filetype
CAMCAD Headers only need thew first token to be OK.
===================================================
A!LAYER_SORT!LAYER_SUBCLASS!LAYER_ARTWORK!LAYER_USE!LAYER_CONDUCTOR!LAYER_DIELECTRIC_CONSTANT!LAYER_ELECTRICAL_CONDUCTIVITY!LAYER_MATERIAL!LAYER_SHIELD_LAYER!LAYER_THERMAL_CONDUCTIVITY!LAYER_THICKNESS!
A!PAD_NAME!REC_NUMBER!LAYER!FIXFLAG!VIAFLAG!PADSHAPE1!PADWIDTH!PADHGHT!PADXOFF!PADYOFF!PADFLASH!PADSHAPENAME!TRELSHAPE1!TRELWIDTH!TRELHGHT!TRELXOFF!TRELYOFF!TRELFLASH!TRELSHAPENAME!APADSHAPE1!APADWIDTH!APADHGHT!APADXOFF!APADYOFF!APADFLASH!APADSHAPENAME!
A!SYM_TYPE!SYM_NAME!REFDES!SYM_X!SYM_Y!SYM_ROTATE!SYM_MIRROR!NET_NAME!CLASS!SUBCLASS!RECORD_TAG!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!GRAPHIC_DATA_10!COMP_DEVICE_TYPE!COMP_PACKAGE!COMP_PART_NUMBER!COMP_VALUE!CLIP_DRAWING!
A!NET_NAME!PIN_NUMBER_SORT!CLASS!REFDES!SYM_TYPE!SYM_NAME!SYM_X!SYM_Y!SYM_ROTATE!SYM_MIRROR!PIN_NAME!PIN_NUMBER!PIN_X!PIN_Y!PAD_STACK_NAME!VIA_X!VIA_Y!PIN_ROTATION!TEST_POINT!NET_PROBE_NUMBER!


Fabmaster headers
==================
A!REFDES!COMP_CLASS!COMP_PART_NUMBER!COMP_HEIGHT!COMP_DEVICE_LABEL!COMP_INSERTION_CODE!SYM_TYPE!SYM_NAME!SYM_MIRROR!SYM_ROTATE!SYM_X!SYM_Y!COMP_VALUE!COMP_TOL!COMP_VOLTAGE!COMP_RATED_CURRENT!COMP_RATED_POWER!COMP_RATED_VOLTAGE!
A!NET_NAME!REFDES!PIN_NUMBER!PIN_NAME!PIN_GROUND!PIN_POWER!
A!CLASS!SUBCLASS!
A!PAD_NAME!REC_NUMBER!LAYER!FIXFLAG!VIAFLAG!PADSHAPE1!PADWIDTH!PADHGHT!PADXOFF!PADYOFF!PADFLASH!PADSHAPENAME!TRELSHAPE1!TRELWIDTH!TRELHGHT!TRELXOFF!TRELYOFF!TRELFLASH!TRELSHAPENAME!APADSHAPE1!APADWIDTH!APADHGHT!APADXOFF!APADYOFF!APADFLASH!APADSHAPENAME!
A!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!RECORD_TAG!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!SUBCLASS!SYM_NAME!REFDES!
A!SYM_NAME!PIN_NAME!PIN_NUMBER!PIN_X!PIN_Y!PAD_STACK_NAME!REFDES!TEST_POINT!
A!VIA_X!VIA_Y!PAD_STACK_NAME!NET_NAME!TEST_POINT!
A!CLASS!SUBCLASS!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!RECORD_TAG!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!NET_NAME!

Newer Fabmaster Version

changes
A!SYM_NAME!SYM_MIRROR|PIN_NAME!PIN_NUMBER!PIN_X!PIN_Y!PAD_STACK_NAME!REFDES!PIN_ROTATION!TEST_POINT!

additional
A!SUBCLASS!PAD_SHAPE_NAME!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!RECORD_TAG!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!PAD_STACK_NAME!REFDES!PIN_NUMBER!
*/
static int check_filetype(int extract, int tot_filecnt)
{
   CString tok0(commandArray[0]->tok);
   CString ctok0(commandArray[0]->compressedTok);
   int label0 = commandArray[0]->label;
   CString tok1(commandArray[1]->tok);
   CString ctok1(commandArray[1]->compressedTok);
   int label1 = commandArray[1]->label;
 
   if (AllegroExtractType == ALLEGRO_FABMASTER)   // if only 1 file, check if this is a FABMASTER extract
   {
      if (commandArray[0]->label == VAL_REFDES && 
          commandArray[1]->label == VAL_COMP_CLASS )  // fabmaster section  
      {
         messageFilter->formatMessage("FABMASTER script does not extract SHAPE/VOID properties!");
         display_error++;

         return FABMASTER_EXTRACT_REFDES;
      }
      else
      if (commandArray[0]->label == VAL_NET_NAME && 
          commandArray[1]->label == VAL_REFDES )   // fabmaster section  
      {
         return FABMASTER_EXTRACT_NET_NAME;
      }
      else
      if (commandArray[0]->label == VAL_CLASS && 
          commandArray[1]->label == VAL_SUBCLASS &&
          commandArrayCount == 2)   // fabmaster section  
      {
         messageFilter->formatMessage("FABMASTER script does not extract LayerStacking/Typing!");
         display_error++;

         return FABMASTER_EXTRACT_LAYERS;
      }
      else
      if (commandArray[0]->label == VAL_GRAPHIC_DATA_NAME && 
          commandArray[1]->label == VAL_GRAPHIC_DATA_NUMBER )  // fabmaster section  
      {
         return FABMASTER_EXTRACT_GRAPHIC;        // geom graphic
      }
      else
      if (commandArray[0]->label == VAL_CLASS && 
          commandArray[1]->label == VAL_SUBCLASS &&
          commandArray[2]->label == VAL_GRAPHIC_DATA_NAME &&
          commandArrayCount > 2) // fabmaster section  
      {
         return FABMASTER_EXTRACT_CLASS_GRAPHIC;
      }
      else
      if (commandArray[0]->label == VAL_SYM_NAME && 
          commandArray[1]->label == VAL_PIN_NAME)  // fabmaster section  
      {
         // free the original masterlist and make a new one
			masterList.empty();

         messageFilter->formatMessage("FABMASTER script does not extract Padstack Rotation!");
         display_error++;

         return FABMASTER_EXTRACT_PINS;
      }
      else
      if (commandArray[0]->label == VAL_SYM_NAME && 
          commandArray[1]->label == VAL_SYM_MIRROR &&  
          commandArray[2]->label == VAL_PIN_NAME)  // fabmaster section  
      {
         // free the original masterlist and make a new one
			masterList.empty();

         return FABMASTER_EXTRACT_PINS1;
      }
      else
      if (commandArray[0]->label == VAL_VIA_X && 
          commandArray[1]->label == VAL_VIA_Y)  // fabmaster section  
      {
         return FABMASTER_EXTRACT_CLASS_VIA;
      }
      else
      if (commandArray[0]->label == VAL_SUBCLASS && 
          commandArray[1]->label == VAL_PADSHAPENAME)  
      {
         return EXTRACT_PAD_SHAPE_NAME;
      }
      else
      if (commandArray[0]->label == VAL_PAD_NAME)  // PAD file
      {
         return EXTRACT_PAD;
      }
      else
      {
         // unknown extract
         CString  tmp;
         tmp.Format("Unknown EXTRACT Section Label at %ld\n[%s]\nPlease go to http://www.mentor.com/supportnet for technical support!\nDo you want to proceed ?",
            line_cnt, tmpline);
         if (ErrorMessage(tmp, "Read Format Error !", MB_YESNO | MB_DEFBUTTON2)==IDYES)
            return -1;
         else
            return -2;
      }
   }

   if (commandArray[0]->label == VAL_LAYER_SORT)
   {
      return CAMCAD_EXTRACT_LAYER;
   }
   else
   if (commandArray[0]->label == VAL_PAD_NAME)  // PAD file
   {
      return EXTRACT_PAD;
   }
   else
   if (commandArray[0]->label == VAL_SYM_TYPE)  // sym file
   {
      return CAMCAD_EXTRACT_SYM;
   }
   else
   if (commandArray[0]->label == VAL_SYM_NAME)  // sym file
   {
      messageFilter->formatMessage("CLASSIC script does not extract Symbol Types -> All symbols converted to PCB COMPONENTS!");
      display_error++;

      return CAMCAD_EXTRACT_CLASSIC_SYM;
   }
   else
   if (commandArray[0]->label == VAL_NET_NAME && 
       commandArray[1]->label == VAL_PIN_NUMBER_SORT) // this is the .r file
   {
      return CAMCAD_EXTRACT_RTE;
   }
   else
   if (commandArray[0]->label == VAL_NET_NAME &&
       commandArray[1]->label == VAL_CLASS)  // this is the .r file
   {
      return CAMCAD_EXTRACT_CLASSIC_RTE;
   }
   else
   if (commandArray[0]->label == VAL_SUBCLASS && 
         commandArray[1]->label == VAL_PADSHAPENAME)  
   {
      return EXTRACT_PAD_SHAPE_NAME;
   }
   else
   {
      // unknown extract
      CString  tmp;
      tmp.Format("Unknown EXTRACT File Start Label\n[%s]\nDo you want to proceed ?",
         tmpline);
      if (ErrorMessage(tmp, "Read Format Error !", MB_YESNO | MB_DEFBUTTON2)==IDYES)
         return -1;
      else
         return -2;
   }

   return extract;
}


/****************************************************************************/
/*
*/
static int update_layertypes()
{
   int   i, j;
   LayerStruct *lp;

   Graph_Level("REFNAME_TOP","",0);
   Graph_Level("REFNAME_BOT","",0);
   Graph_Level_Mirror("REFNAME_TOP", "REFNAME_BOT", "");

   // now all mirror layers
   for (j = 0; j< globalDoc->getMaxLayerIndex(); j++)
   {
      lp = globalDoc->getLayerArray()[j];
      if (lp == NULL)   continue; // could have been deleted.

      for (i=0;i<layerscnt;i++)
      {
         if (lp->getName().CompareNoCase(layers[i].name) == 0)
         {
            if (layers[i].mirror != i)
            {
               Graph_Level(layers[layers[i].mirror].name,"", 0);
               Graph_Level_Mirror(layers[i].name, layers[layers[i].mirror].name, "");
            }
            break;
         }
      }
   }

   // now attributes after mirror layers
   for (j = 0; j< globalDoc->getMaxLayerIndex(); j++)
   {
      lp = globalDoc->getLayerArray()[j];
      if (lp == NULL)   continue; // could have been deleted.

      for (i=0;i<layer_attr_cnt;i++)
      {
         if (lp->getName().CompareNoCase(layer_attr[i].name) == 0)
         {
            lp->setLayerType(layer_attr[i].attr);
            break;
         }
      }
   }

   // rename all layer numbers to their comment names
   for (i=0; i< globalDoc->getMaxLayerIndex(); i++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      if (globalDoc->getLayerArray()[i] == NULL)  continue;
      assign_padlayertypes(i);
   }

   return 1;
}

/****************************************************************************/
/*
*/
int   update_layer(const char *l)
{
   int   i;

   for (i=0;i<layerscnt;i++)
   {
      if (!strcmp(layers[i].name,l))
         return i;      
   }


   if (layerscnt < MAX_LAYERS)
   {
      layers[layerscnt].name = STRDUP(l);
      layers[layerscnt].mirror = layerscnt;
      layerscnt++;
   }
   else
   {
      messageFilter->formatMessage("Too many layers");
      display_error++;
   }
   return layerscnt-1;
}

/******************************************************************************
* tst_allegtoextractfiletype
*/
int tst_allegtoextractfiletype(const char *f)
{
   int ftype = ALLEGRO_UNKNOWN;
   CString  tmp;
   char line[MAX_LINE];

   FILE *fp;
   if ((fp = fopen(f, "rt")) == NULL)
   {
      ErrorMessage(f, "Error opening file");
      return -1;
   }

	while (read_line(fp, &line[0], MAX_LINE))
   {
      tmp = line;
      tmp.TrimLeft(" ");
      tmp.TrimRight(" ");
		int tmpType = ALLEGRO_UNKNOWN;

      if (!STRNICMP(tmp,"A!SYM_TYPE!SYM_NAME",strlen("A!SYM_TYPE!SYM_NAME")))
      {
         tmpType = ALLEGRO_CAMCAD;
      }
      else if (!STRNICMP(tmp,"A!SYM_NAME!REFDES",strlen("A!SYM_NAME!REFDES")))
      {
         tmpType = ALLEGRO_LAYDBASE;
      }
      else if (!STRNICMP(tmp,"A!NET_NAME!PIN_NUMBER_SORT!",strlen( "A!NET_NAME!PIN_NUMBER_SORT!" )))
      {
         tmpType = ALLEGRO_CAMCAD;
      }
      else if (!STRNICMP(tmp,"A!NET_NAME!CLASS!REFDES!",strlen( "A!NET_NAME!CLASS!REFDES!" )))
      {
         tmpType = ALLEGRO_LAYDBASE;
      }
      else if (!STRNICMP(tmp,"A!REFDES!COMP_CLASS!COMP_PART_NUMBER!", strlen("A!REFDES!COMP_CLASS!COMP_PART_NUMBER!")))
      {
         tmpType = ALLEGRO_FABMASTER;
      }
      else if (!STRNICMP(tmp,"A!REFDES!COMP_CLASS!COMP_DEVICE_TYPE!COMP_HEIGHT!COMP_PART_NUMBER!", strlen("A!REFDES!COMP_CLASS!COMP_DEVICE_TYPE!COMP_HEIGHT!COMP_PART_NUMBER!")))
      {
         tmpType = ALLEGRO_FABMASTER;
      }
      else if (!STRNICMP(tmp,"A!LAYER_SORT!LAYER_SUBCLASS!",strlen( "A!LAYER_SORT!LAYER_SUBCLASS!" )))
      {
         tmpType = ALLEGRO_ALL;
      }
      else if (!STRNICMP(tmp,"A!PAD_NAME!REC_NUMBER!",strlen( "A!PAD_NAME!REC_NUMBER!" )))
      {
         tmpType = ALLEGRO_ALL;
      }

		if (ftype == ALLEGRO_UNKNOWN || (tmpType != ALLEGRO_UNKNOWN && tmpType != ALLEGRO_ALL))
		{
			ftype = tmpType;
		}
   }

  fclose(fp);

	//file.Close();

   return ftype;
}

static void explode_geometries()
{
	for (int i=0; i<globalDoc->getMaxBlockIndex(); i++)
	{
		BlockStruct* block = globalDoc->getBlockAt(i);
		if (!block)
			continue;

		POSITION dataPos = block->getHeadDataInsertPosition();
		while (dataPos)
		{
			DataStruct* data = block->getNextDataInsert(dataPos);
			if (!data)
				continue;
			
			BlockStruct* geometry = globalDoc->getBlockAt(data->getInsert()->getBlockNumber());
			if (!geometry)
				continue;
			
			/*if (is_attrib(geometry->getName(), EXPLODE_SYMBOL))
				ExplodeInsert(globalDoc, data, &currentPCBFile->getBlock()->getDataList());*/
		}
	}
}

static void setToFiducialOrTool()
{
	for (int i=0; i<globalDoc->getMaxBlockIndex(); i++)
	{
		BlockStruct* block = globalDoc->getBlockAt(i);
		if (!block)
			continue;

		POSITION insertPos = block->getHeadDataInsertPosition();
		while (insertPos)
		{
			DataStruct* insertData = block->getNextDataInsert(insertPos);
			if (!insertData)
				continue;

			BlockStruct* geom = globalDoc->getBlockAt(insertData->getInsert()->getBlockNumber());
			if (!geom)
				continue;

			if (is_attrib(geom->getName(), PADSTACK_FIDUCIAL))
				insertData->getInsert()->setInsertType(insertTypeFiducial);
			else if (is_attrib(geom->getName(), PADSTACK_TOOL))
				insertData->getInsert()->setInsertType(insertTypeDrillTool);
		}
	}
}


//---------------------------------------------------------------------------
// CAllegroRefdes::CAllegroRefdes
//---------------------------------------------------------------------------
CAllegroRefdes::CAllegroRefdes(CString refdes)
{
	m_refdes = refdes;
	m_uniquerRefdesMap.RemoveAll();
}

CAllegroRefdes::~CAllegroRefdes()
{
}

bool CAllegroRefdes::GetUniqueRefdesByCoordinate(double x, double y, CString &uniqueRefdes)
{	
	CString uniqueRefdesKey;
	uniqueRefdesKey.Format("%s_%0.3f_%0.3f", m_refdes, x, y);
	if (m_uniquerRefdesMap.Lookup(uniqueRefdesKey, uniqueRefdes))
	{
		// Same refdes at same location already existed
		return false;
	}

	if (m_uniquerRefdesMap.GetCount() > 0)
	{
		// Same refdes but different location
		uniqueRefdes.Format("%s_%d", m_refdes, m_uniquerRefdesMap.GetCount());
	}
	else
	{
		// Refdes never existed 
		uniqueRefdes = m_refdes;
	}
	m_uniquerRefdesMap.SetAt(uniqueRefdesKey, uniqueRefdes);

	return true;
}


//---------------------------------------------------------------------------
// CAllegroRefdes::CAllegroRefdes
//---------------------------------------------------------------------------
CAllegroRefdesMap::CAllegroRefdesMap()
{
}

CAllegroRefdesMap::~CAllegroRefdesMap()
{
	m_refdesMap.empty();
}

void CAllegroRefdesMap::Empty()
{
	m_refdesMap.empty();
}

bool CAllegroRefdesMap::GetUniqueRefdesByCoordinate(CString refdes, double x, double y, CString &uniqueRefdes)
{
	CAllegroRefdes* allegroRefdes = NULL;
	if (!m_refdesMap.Lookup(refdes, allegroRefdes))
	{
		allegroRefdes = new CAllegroRefdes(refdes);
		m_refdesMap.SetAt(refdes, allegroRefdes);
	}

	return allegroRefdes->GetUniqueRefdesByCoordinate(x, y, uniqueRefdes);
}
