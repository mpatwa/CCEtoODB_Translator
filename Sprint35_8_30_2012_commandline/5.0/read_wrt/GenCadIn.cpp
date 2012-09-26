
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright ?1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "graph.h"
#include "geomlib.h"
#include "pcbutil.h"
#include "pcbutil2.h"
#include "net_util.h"
#include "format_s.h"
#include <math.h>
#include "gauge.h"
#include "attrib.h"
#include "polylib.h"
#include "gencadin.h"
#include "CCEtoODB.h"
#include "dft.h"
#include "RwUiLib.h"

extern CProgressDlg        *progress;
//extern LayerTypeInfoStruct	layertypes[];
extern char                *testaccesslayers[];

/* Globals ****************************************************************/
static CCEtoODBDoc          *doc;
static FileStruct          *file;
static FILE                *ifp;
static char                ifp_line[GCAD_MAX_LINE];
static long                ifp_linecnt = 0;
static char                lastchar = 0; // needed for xfgets
static FILE                *ferr;
static int                 display_error = 0;

static int                 pageunits;
static double              scale_factor = 1;
static int                 file_type = GEN_UNKNOWN;
static int                 cur_filenum = 0;
static int                 cur_layer;
static int                 cur_entity;
static CString             boardoutline;     

static DataStruct          *cur_data;
static int                 routcont;  
static double              oldx;
static double              oldy;
static int                 oldwidthindex;
static int                 oldlay;
static CString             oldnetname;

static double              refname_height;
static double              viadrill_oversize;
static int                 gencad_version = 13; // (1.3 * 10)

static int                 ComponentSMDrule;    // 0 = take existing ATT_SMD
                                                // 1 = if all pins of comp are marked as SMD
                                                // 2 = if most pins of comp are marked as SMD
                                                // 3 = if one pin of comp are marked as SMD

static CPlaneList          planeList;  
static CPolyList           *planePolyList;
static CPoly               *planePoly;

static GENCADLayers        layer[GCAD_MAX_LAYERS];         /* Array of layer names from the actual gencad file */
static int                 max_layers;

static Adef                layer_attr[GCAD_MAX_LAYERS];       /* Array of layers from gencad.in   */
static int                 layer_attr_cnt;

static CString             compoutline[30];
static int                 compoutlinecnt;

static GENCADPolyPntArray	polypntarray;
static int						polypntcnt;

static GENCADPadArray      padarray;   
static int                 padcnt;

static GENCADCompArray     comparray;  
static int                 compcnt;

static CDrillArray         drillarray;
static int                 drillcnt;

static GENCADMirrorShapeArray mirrorshapearray; 
static int                 mirrorshapecnt;
  
static GENCADNetpinArray   netpinarray;
static int                 netpincnt;

static GENCADTestpinArray  testpinarray;
static int                 testpincnt;

static GENCADFeatureArray  featurearray;
static int                 featurecnt;

static CString             padprefix;
static CString             padstackprefix;

// Variable from load setting
static BOOL                ForceText2Silk = FALSE;
static BOOL                USE_PLANE_NAMES = TRUE;
static BOOL						REFRESH_DEVICETOCOMP_ATTRIBUTES;

static CTypeChanger MakeInto_CommandProcessor;

/* List Section ***********************************************************/
/* This comand list need to stay in here because "gencadin.h" is included in 
   "port_lib.cpp" which makes everything in header file public.
*/
static int gcad_header();
static int gcad_board();
static int gcad_pads();
static int gcad_padstacks();
static int gcad_artworks();
static int gcad_shapes();
static int gcad_components(); 
static int gcad_tracks();
static int gcad_routes();
static int gcad_signals();
static int gcad_devices();
static int gcad_layers();
static int gcad_pseudos();
static int gcad_changes();
static int gcad_mech();
static int gcad_testpins();
static int gcad_powerpins();
static int gcad_panel();
static int gcad_boards();

static List command_lst[] =
{
   {"$HEADER",       gcad_header},  
   {"$BOARD",        gcad_board},
   {"$PADS",         gcad_pads},
   {"$PADSTACKS",    gcad_padstacks},
   {"$ARTWORKS",     gcad_artworks},
   {"$SHAPES",       gcad_shapes},
   {"$COMPONENTS",   gcad_components},
   {"$TRACKS",       gcad_tracks},
   {"$ROUTES",       gcad_routes},
   {"$SIGNALS",      gcad_signals},
   {"$DEVICES",      gcad_devices},
   {"$LAYERS",       gcad_layers},
   {"$PSEUDOS",      gcad_pseudos},
   {"$CHANGES",      gcad_changes},
   {"$MECH",         gcad_mech},
   {"$TESTPINS",     gcad_testpins},
   {"$POWERPINS",    gcad_powerpins},
   {"$PANEL",        gcad_panel},
   {"$BOARDS",       gcad_boards},
};


/* Prototype Section ****************************************************/
static void InitializeMembers();
static void FreeMembers();
static int load_gencadsettings(const CString fname);
static char *get_nextline(char *string,int n,FILE *fp);
static int is_command(char *l);
static int loop_to_end();
static int update_modified_devices();
static int write_netlist();
static int assign_layers();
static int assign_testprobes();
static int clean_padprefix();
static int clean_padstackprefix();
static Region *CheckPlaneIntersect(CPolyList *polylist, int width, int lay, BOOL voidPlane);
extern void ArrangePolysForMerge(CPoly *poly1, CPoly *poly2);
/******************************************************************************
* ReadGENCAD
*/
void ReadGENCAD(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{
   doc = Doc;
   pageunits = PageUnits;
   scale_factor = Units_Factor(UNIT_INCHES, PageUnits);

   CString gencadLogFile = GetLogfilePath(GENCADERR);
   if ((ferr = fopen(gencadLogFile, "wt")) == NULL)
   {
      CString message;
      message.Format("Error open [%s] file", gencadLogFile);
      ErrorMessage(message, "Error");
      return;
   }
   display_error = 0;

   // need to do this, because panel only knows the path.ext
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath( path_buffer, drive, dir, fname, ext );

   char f[_MAX_FNAME+4];
   strcpy(f, fname);
   strcat(f, ext);

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(path_buffer, "r")) == NULL)
   {
      ErrorMessage("Error open file", path_buffer);
      return;
   }

   CString settingsFile( getApp().getImportSettingsFilePath("gencad.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nGENCAD Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);

   InitializeMembers();
   load_gencadsettings(settingsFile);

   CString fil = f;
   fil.MakeLower();  // need to be lower so that the panel can load and find the file.
   file = Graph_File_Start(fil, Type_GENCAD);
   cur_data = NULL;
   ifp_linecnt = 0;
   Graph_Level("0", "", 1);
   cur_filenum = file->getFileNumber();  

   // here do a while loop
   while (get_nextline(ifp_line, GCAD_MAX_LINE, ifp))
   {
      int code;
      char *lp;

      // \r needed for VMS
      if ((lp = get_string(ifp_line, " \r\t\n")) == NULL)
         continue;

      if ((code = is_command(lp)) < 0)
      {
         if (!STRNICMP(lp, "$REN", 4))
         {
            // $RENPROG=2000 //cdf_to_gencad 1.1.A_2_sep97
            // $RENTOOL=t90676
            // $RENASE=349
         }
         else
         {
            // logical file end error - eof before $end???
            CString message;
            message.Format("Unknown GENCAD Section [%s] at %ld.", lp, ifp_linecnt);
            ErrorMessage(message, "Unknown GENCAD section - goto $END", MB_OK | MB_ICONHAND);

            // loop to next end
            if (loop_to_end() < 0)
            {
               // logical file end error - eof before $end???
               message.Format("GENCAD ERROR at %ld.", ifp_linecnt);
               ErrorMessage(message, "Logical EOF before $END", MB_OK | MB_ICONHAND);
            }
         } // $REN
      }
      else
      {
         (*command_lst[code].function)();
      }
   }

   update_modified_devices();
   write_netlist();
   assign_layers();

   // a padprefix was given because in GENCAD a PAD can have the same name as a PADSTACK !
   if (strlen(padprefix))
      clean_padprefix();
   if (strlen(padstackprefix))
      clean_padstackprefix();

   fclose(ifp);

   update_smdpads(doc);
   update_smdrule_geometries(doc, ComponentSMDrule);
   update_smdrule_components(doc, file, ComponentSMDrule);
   RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.
	
	if (REFRESH_DEVICETOCOMP_ATTRIBUTES)
		RefreshTypetoCompAttributes(doc, SA_OVERWRITE);

   OptimizePadstacks(doc, pageunits, 0); // this can create unused blocks
   generate_PADSTACKACCESSFLAG(doc, 1);
   doc->purgeUnusedBlocks(); // use after optimize_PADSTACKS
   MakeInto_CommandProcessor.Apply(doc, file);
   generate_PINLOC(doc, file, 1);   // do it always

   int laynr = Graph_Level("SILKSCREEN_TOP", "", 0);
   double minref = 0.05 * Units_Factor(UNIT_INCHES, pageunits);
   double maxref = 0.05 * Units_Factor(UNIT_INCHES, pageunits);
   //update_manufacturingrefdes(doc, file,&(file->getBlock()->getDataList()), minref, maxref, laynr, FALSE);

   // here report which pins did not make it.   
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);
      // SaveAttribs(stream, &net->getAttributesRef());
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);
         if (!compPin->getPinCoordinatesComplete())
         {
            display_error++;
            fprintf(ferr, "Error: Component [%s] Pin [%s] in Net [%s] could not be found in Design ->Removed.\n",
                  compPin->getRefDes(), compPin->getPinName(), net->getNetName());
            RemoveCompPin(file, compPin->getRefDes(), compPin->getPinName());
         }
      }
   }

   assign_testprobes();
	ProcessLoadedProbes(doc);
   Generate_Unique_DCodes(doc);

   fclose(ferr);
   FreeMembers();
   MakeInto_CommandProcessor.Reset();
   if (display_error)
      Logreader(gencadLogFile);

   return;
}

/******************************************************************************
* InitializeMembers
*/
static void InitializeMembers()
{
	polypntarray.SetSize(2000, 1000);
	polypntcnt = 0;

   padarray.SetSize(100, 100);
   padcnt = 0;

   comparray.SetSize(100, 100);
   compcnt = 0;

   mirrorshapearray.SetSize(100, 100);
   mirrorshapecnt = 0;

   netpinarray.SetSize(100, 100);
   netpincnt = 0;

   testpinarray.SetSize(100, 100);
   testpincnt = 0;

   featurearray.SetSize(100, 100);
   featurecnt = 0;

   drillarray.SetSize(100, 100);
   drillcnt = 0;
}

/******************************************************************************
* FreeMembers
*/
static void FreeMembers()
{
	int i=0;
	for (i=0; i<polypntcnt; i++)
      delete polypntarray[i];
   polypntarray.RemoveAll();

   for (i=0; i<padcnt; i++)
      delete padarray[i];
   padarray.RemoveAll();

   for (i=0; i<compcnt; i++)
      delete comparray[i];
   comparray.RemoveAll();

   for (i=0; i<mirrorshapecnt; i++)
      delete mirrorshapearray[i];
   mirrorshapearray.RemoveAll();

   for (i=0; i<netpincnt; i++)
      delete netpinarray[i];
   netpinarray.RemoveAll();
   
   for (i=0; i<testpincnt; i++)
      delete testpinarray[i];
   testpinarray.RemoveAll();

   for (i=0; i<featurecnt; i++)
      delete featurearray[i];
   featurearray.RemoveAll();

   drillarray.RemoveAll();

   while(planeList.GetCount())
   {
      Plane *Node = planeList.GetTail();
      delete Node->planeRegion;
      delete planeList.RemoveTail(); 
   }
}

/******************************************************************************
* standard_gencadlayer
*/
static int  standard_gencadlayer( const char *l)
{
   if (!STRCMPI(l,"BOTTOM"))           return 1;
   if (!STRCMPI(l,"TOP"))              return 1;
   if (!STRCMPI(l,"ALL"))              return 1;
   if (!STRNICMP(l,"INNER", 5))        return 1;
   if (!STRNICMP(l,"SOLDERPASTE", 11)) return 1;
   if (!STRNICMP(l,"SILKSCREEN",  10)) return 1;
   if (!STRNICMP(l,"SOLDERMASK",  10)) return 1;

   return 0;
}

/******************************************************************************
* get_attval
*/
static int get_attval(char *key, char *val)
{
   char *lp;
   key[0] = '\0';
   val[0] = '\0';

   if ((lp = get_string(NULL, " \r\t")) == NULL) 
      return 0;
   if ((lp = get_string(NULL, " \r\t")) == NULL)
      return 0;
   strcpy(key, lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 1;
   strcpy(val, lp);

   return 1;
}

/******************************************************************************
* set_usedlayer
*/
static int set_usedlayer(const char* l)
{
   for (int i=0; i<max_layers; ++i)
   {
      if (!STRCMPI(layer[i].layer_name,l))
      {
         layer[i].used = 1;
         return(i);
      }
   }

   if (max_layers < GCAD_MAX_LAYERS)
   {
      layer[max_layers].layer_name = l;
      layer[max_layers].flg = 0;
      layer[max_layers].used = 1;
      layer[max_layers].color = 0;
      max_layers++;
   }
   else
   {
      fprintf(ferr, "Too many layers!\n");
      display_error++;
      return -1;
   }

   return max_layers-1;
}

/******************************************************************************
* get_padshape
*/
static int get_padshape(const char* p)
{
   for (int i=0; i<padcnt; i++)
   {
      if (!STRCMPI(padarray[i]->padname,p))
         return i;
   }

   return -1;
}

/******************************************************************************
* get_comp
*/
static int get_comp(const char* p)
{
   for (int i=0; i<compcnt; i++)
   {
      if (!STRCMPI(comparray[i]->compname,p))
         return i;
   }

   return -1;
}

/******************************************************************************
* get_mirrorshape1
*/
static int get_mirrorshape1(const char* p)
{
   for (int i=0; i<mirrorshapecnt; i++)
   {
      if (!STRCMPI(mirrorshapearray[i]->shape1, p))
         return i;
   }
   return -1;
}

/******************************************************************************
* get_mirrorshape2
*/
static int get_mirrorshape2(const char* p)
{
   for (int i=0; i<mirrorshapecnt; i++)
   {
      if (!STRCMPI(mirrorshapearray[i]->shape2,p))
         return i;
   }

   return -1;
}

/******************************************************************************
* cnv_tok
*/
static double cnv_tok(char *l)
{
   double x = 0.0;

   if (l && strlen(l))
      x = atof(l);

   return x * scale_factor;
}

/******************************************************************************
* xfgets
   This is to be able to read files which only have 0D and not 0A line ends
   return *s or NULL of EOF
*/
static char * xfgets(char *s, int len, FILE *fp)
{
   int wc;
   int wcnt = 0;

   s[0] = '\0';
   while ((wc = fgetc(fp)) != EOF)
   {
      // here is end of line
      if (lastchar == 0x0D && wc == 0x0A)
         continue;
      if (lastchar == 0x0A && wc == 0x0D)
         continue;
      lastchar = wc;

      if (wcnt < len-1)
      {
         if (wc == 0x0D || wc == 0x0A)
         {
            ifp_linecnt++;
            s[wcnt]   = '\0';
            return s;
         }
         else
         {
            s[wcnt++] = wc;
         }
      }
      else
      {
         s[wcnt] = '\0';
         return s;
      }
   }

   if (wcnt)
   {
      s[wcnt] = '\0';
      return s;
   }

   return NULL;
}

/******************************************************************************
* get_nextline
*/
static char *get_nextline(char *string,int n,FILE *fp)
{
   char *res = xfgets(string, n, fp);
   return res;
}

/******************************************************************************
* MergeOrderedPolys
*/
void MergeOrderedPolys(CPolyList *polyList)
{
   if(!polyList) return;

   while (polyList->GetCount() > 1)
   {
      POSITION polyPos1 = polyList->GetHeadPosition();            
      CPoly *poly1 = polyList->GetNext(polyPos1);
      if(polyPos1 != NULL)
      {
         POSITION tmpPos = polyPos1;
         CPoly *poly2 = polyList->GetNext(polyPos1);
         ArrangePolysForMerge(poly1, poly2);
         
         POSITION pos1 = poly1->getPntList().GetTailPosition();
         CPnt* poly1_tail = poly1->getPntList().GetPrev(pos1);
         pos1 = poly2->getPntList().GetHeadPosition();
         CPnt* poly2_head = poly2->getPntList().GetNext(pos1);
         
         if(fpeq(poly1_tail->x, poly2_head->x)
            && fpeq(poly1_tail->y, poly2_head->y))
         {
            if(poly1_tail->bulge == 0)
               delete poly1->getPntList().RemoveTail();

            POSITION pos = poly2->getPntList().GetHeadPosition();
            while (pos)
            {               
               CPnt *newPnt = new CPnt(*(poly2->getPntList().GetNext(pos)));
               poly1->getPntList().AddTail(newPnt);
            }
            polyList->deleteAt(tmpPos);
         }
         else 
            break;
      }//if
   }//while
}

/******************************************************************************
* getExtendPlanePoly : new PlanePoly can be combined by MergeOrderedPolys
*/
CPoly *getExtendPlanePoly()
{
   CPoly *newPoly = planePoly;
   if(planePoly->getPntList().GetCount())
   {
      newPoly = new CPoly;
      planePolyList->AddTail(newPoly);
   }

   return newPoly;
}

/******************************************************************************
* get_drillindex
*/
static int get_drillindex(double size, int layernum)
{
   if (size == 0)
      return -1;

   for (int i=0; i<drillcnt; i++)
   {
      GENCADDrill drill = drillarray.ElementAt(i);
      if (drill.d == size)
         return drill.toolindex;         
   }

   CString name;
   name.Format("DRILL_%d", drillcnt);
   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   GENCADDrill drill;
   drill.d = size;
   drill.toolindex = drillcnt;
   drillarray.SetAtGrow(drillcnt++, drill);  

   return drillcnt-1;
}

/******************************************************************************
* do_circle
*/
static DataStruct *do_circle(int lay, int widthindex, int filled, int plane)
{
   char *lp;
   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double x1 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double y1 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double r = cnv_tok(lp);

   if (plane)
   {
      CPnt *pnt;
      pnt = new CPnt;      
      pnt->bulge = (DbUnit)1.0;
      pnt->x = (DbUnit)x1;
      pnt->y = (DbUnit)(y1 - r);
      planePoly->getPntList().AddTail(pnt);
      
      pnt = new CPnt;
      pnt->bulge = (DbUnit)1.0;
      pnt->x = (DbUnit)x1;
      pnt->y = (DbUnit)(y1 + r);
      planePoly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->bulge = (DbUnit)1.0;
      pnt->x = (DbUnit)x1;
      pnt->y = (DbUnit)(y1 - r);
      planePoly->getPntList().AddTail(pnt);

      return NULL;
   }
   else
   {
      return Graph_Circle(lay, x1, y1, r, 0L, widthindex, FALSE, filled); 
   }
}

/******************************************************************************
* clean_prosa
*/
static void clean_prosa(char *p)
{
   if (strlen(p) == 0)
      return;

   // delete all traling isspace
   while (strlen(p) && isspace(p[strlen(p)-1])) 
         p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')
      p[strlen(p)-1] = '\0';

   /* Fix for dts0100469299 - Commenting code which removes leading spaces
      Leading spaces are required to maintain alignment */
   /*STRREV(p);
   while (strlen(p) && isspace(p[strlen(p)-1])) 
         p[strlen(p)-1] = '\0';

   if (p[strlen(p)-1] == '\"')
      p[strlen(p)-1] = '\0';
   STRREV(p);*/

   return;
}

/******************************************************************************
* do_text
*/
static int do_text(CString *refname, double *x, double *y, double *height, double *charwidth, double *rot, int *mir, int*lay, BOOL CompText)
{
   char *lp;
   int textlay;

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double textx = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double texty = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double texth = cnv_tok(lp);
   double textw = texth*TEXT_WIDTHFACTOR;

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double textrot = atof(lp);

   while (textrot < 0)
      textrot += 360;
   while (textrot >= 360)
      textrot -= 360;

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;

   // 0, MIRRORX, MIRRORY
   if (STRCMPI(lp,"0") && STRCMPI(lp, "MIRRORX") && STRCMPI(lp, "MIRRORY"))
   {
      fprintf(ferr, "TEXT Element Mirror Field [%s] wrong at %ld\n", lp, ifp_linecnt);
      display_error++;
      return 0;
   }

   int textmir = FALSE;
   if (!STRCMPI(lp, "MIRRORY"))
   {
      textmir = TRUE;
   }
   else if (!STRCMPI(lp, "MIRRORX"))
   {
      textrot += 180;
      textmir = TRUE;
   }

   // here is the layer
   if ((lp = get_string(NULL, " \r\t\n")) == NULL) 
      return 0;

   if (ForceText2Silk)
   {
      if (strstr(lp,"SILK") && strstr(lp, "BOT"))
      {
         set_usedlayer("SILKSCREEN_BOTTOM");
         textlay = Graph_Level("SILKSCREEN_BOTTOM","", 0);
      }
      else
      {
          set_usedlayer("SILKSCREEN_TOP");
         textlay = Graph_Level("SILKSCREEN_TOP","", 0);
      }
   }
   else
   {
      set_usedlayer(lp);
      textlay = Graph_Level(lp, "", 0);
   }

   // text string
   if ((lp = get_string(NULL," \r\t\n")) == NULL)
      return 0;
   char tmp[255];
   strcpy(tmp, lp);
   clean_prosa(tmp);

   double x1, y1, x2 = -1, y2 = -1;
   // and here are x1 y1 x2 y2 rectangle - not needed
   if (lp = get_string(NULL, " \r\t\n")) 
   {
      x1 = cnv_tok(lp);
      if (lp = get_string(NULL, " \r\t\n")) 
      {
         y1 = cnv_tok(lp);
         if (lp = get_string(NULL, " \r\t\n")) 
         {
            x2 = cnv_tok(lp);
            if (lp = get_string(NULL, " \r\t\n")) 
            {
               y2 = cnv_tok(lp);
            }
         }
      }
   }

  
	//case 961: When we have rotations, the dimensions of the text are getting messed up.
	/*int r = round(textrot);
   if (x2 > 0 && y2 > 0 && ((r % 90) == 0))
   {
      // recalculate width and height from extens
      switch (r)
      {
      case 0:
         texth = fabs(y2);
         textw = (fabs(x2) / strlen(tmp)) *0.9;
         break;
      case 90:
         texth = fabs(x2);
         textw = (fabs(y2) / strlen(tmp)) *0.9;
         break;
      case 180:
         texth = fabs(y2);
         textw = (fabs(x2) / strlen(tmp)) *0.9;
         break;
      case 270:
         texth = fabs(x2);
         textw = (fabs(y2) / strlen(tmp)) *0.9;
         break;
      }
   }*/

   if (!CompText)
      Graph_Text(textlay, tmp, textx, texty, texth, textw, DegToRad(textrot), 0L, 1, textmir, 0, 0, 0, 0); 
   *lay = textlay;
   *refname = tmp;
   *x = textx;
   *y = texty;
   *height = texth;
   *charwidth = textw;
   *rot = textrot;
   *mir = textmir;

   return 1;
}

/******************************************************************************
* do_arc
*/
static DataStruct *do_arc(int lay, int widthindex, int plane)
{
   DataStruct *data = NULL; 
 
   char *lp;
   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double xs = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double ys = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL) 
      return 0;
   double xe = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double ye = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double xc = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL) 
      return 0;
   double yc = cnv_tok(lp);

   double r,startangle, deltaangle;
   ArcCenter2(xs, ys, xe, ye, xc, yc, &r, &startangle, &deltaangle, FALSE);

   if (r == 0) // arc was illegal - return NULL
   {
      fprintf(ferr, "Illegal ARC detected at %ld -> skipped\n", ifp_linecnt);
      display_error++;
      return NULL;
   }
   double degree = RadToDeg(deltaangle);

   // here compensate for rounding errors.
   if (fabs(360 - degree) < 0.1)
   {
      r = sqrt((xs-xc)*(xs-xc) + (ys-yc)*(ys-yc));
      xs = xe;
      ys = ye;
   }

   if (xs == xe && ys == ye)
   {
      double bulge = tan(deltaangle/4);
      if (!plane)
      {
         data = Graph_PolyStruct(lay, 0L ,FALSE);
         Graph_Poly(NULL, widthindex, 0, 0, TRUE); // closed
         Graph_Vertex(xc - r, yc, 1.0);
         Graph_Vertex(xc + r, yc, 1.0);
         Graph_Vertex(xc - r, yc, 0.0);
      }
      else
      {
         CPoly *newPoly = getExtendPlanePoly();
 
         CPnt *pnt;
         pnt = new CPnt;
         pnt->bulge = (DbUnit)1;
         pnt->x = (DbUnit)(xc - r);
         pnt->y = (DbUnit)yc;
         newPoly->getPntList().AddTail(pnt);

         pnt = new CPnt;
         pnt->bulge = (DbUnit)1;
         pnt->x = (DbUnit)(xc + r);
         pnt->y = (DbUnit)yc;
         newPoly->getPntList().AddTail(pnt);

         pnt = new CPnt;
         pnt->bulge = (DbUnit)1;
         pnt->x = (DbUnit)(xc - r);
         pnt->y = (DbUnit)yc;
         newPoly->getPntList().AddTail(pnt);

         MergeOrderedPolys(planePolyList);

      }
      
      routcont = FALSE;
   }
   else
   {
      if (deltaangle == 0)
      {
         fprintf(ferr, "Arc Error at %ld\n", ifp_linecnt);
         display_error++;
      }
      else
      {
         double bulge = tan(deltaangle/4);

         if (!plane)
         {
            data = Graph_PolyStruct(lay, 0L, FALSE);
            Graph_Poly(NULL,widthindex, 0, 0, 0);
            Graph_Vertex(xs, ys, bulge);
            Graph_Vertex(xe, ye, 0.0);
         }
         else
         {
            CPoly *newPoly = getExtendPlanePoly();

            CPnt *pnt;
            pnt = new CPnt;
            pnt->bulge = (DbUnit)bulge;
            pnt->x = (DbUnit)xs;
            pnt->y = (DbUnit)ys;
            newPoly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            pnt->x = (DbUnit)xe;
            pnt->y = (DbUnit)ye;
            newPoly->getPntList().AddTail(pnt);

            MergeOrderedPolys(planePolyList);

         }
      }
   }

   oldx = xe;
   oldy = ye;
   oldwidthindex = widthindex;
   oldlay = lay;

   return data;
}

/******************************************************************************
* do_line
*/
static DataStruct *do_line(int lay, int widthindex)
{
   char *lp;
   if ((lp = get_string(NULL, " \r\t\n")) == NULL) 
      return 0;
   double x1 = cnv_tok(lp); 

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double y1 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double x2 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double y2 = cnv_tok(lp);

   if (routcont)  // only test if no other switched off routcont
   {
      if (oldx != x1)
         routcont = FALSE;
      if (oldy != y1)
         routcont = FALSE;
      if (oldwidthindex != widthindex)
         routcont = FALSE;
      if (oldlay != lay) 
         routcont = FALSE;
   }

   DataStruct *data = NULL;
   if (!routcont)
   {
      data = Graph_PolyStruct(lay, 0L, 0);
      Graph_Poly(NULL, widthindex, 0, 0, 0);
      Graph_Vertex(x1, y1, 0.0);
      routcont = TRUE;
   }
   Graph_Vertex(x2, y2, 0.0);

   oldx = x2;
   oldy = y2;
   oldwidthindex = widthindex;
   oldlay = lay;

   return data;
}

/******************************************************************************
* do_routeline
*/
static int do_routeline(int lay, int widthindex, const char *netname, int plane, int gclass, BOOL FirstPlanePoint)
{
   char *lp;
   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double x1 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double y1 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double x2 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL) 
      return 0;
   double y2 = cnv_tok(lp);

   if (routcont)  // only test if no other switched off routcont
   {
      if (oldx != x1)
         routcont = FALSE;
      if (oldy != y1)
         routcont = FALSE;
      if (oldwidthindex != widthindex)
         routcont = FALSE;
      if (oldlay != lay)
         routcont = FALSE;
      if (STRCMPI(oldnetname, netname))
         routcont = FALSE;
   }

   if(plane)
   {
      CPoly *newPoly = getExtendPlanePoly();

      CPnt *pnt;
      if (FirstPlanePoint)
      {
         pnt = new CPnt;
         pnt->bulge = 0.0;
         pnt->x = (DbUnit)x1;
         pnt->y = (DbUnit)y1;
         newPoly->getPntList().AddTail(pnt);
         FirstPlanePoint = FALSE;         
      }

      pnt = new CPnt;
      pnt->bulge = 0.0;
      pnt->x = (DbUnit)x2;
      pnt->y = (DbUnit)y2;
      newPoly->getPntList().AddTail(pnt);

      MergeOrderedPolys(planePolyList);
   }
   else
   {
      if (!routcont )
      {
         DataStruct *data = Graph_PolyStruct(lay, 0L, 0);
         data->setGraphicClass(gclass);

         if (strlen(netname) && gclass == GR_CLASS_ETCH)
            doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, (char *)netname, SA_APPEND, NULL); 

         CPoly *cp = Graph_Poly(NULL, widthindex, 0, 0, 0);
         Graph_Vertex(x1, y1, 0.0);
         routcont = TRUE;
      }  
      Graph_Vertex(x2, y2, 0.0);
   }

   oldx = x2;
   oldy = y2;
   oldwidthindex = widthindex;
   oldnetname = netname;
   oldlay = lay;

   return 1;
}

/******************************************************************************
* do_rectangle
*/
static DataStruct *do_rectangle(int lay, int widthindex, int filled, int plane)
{
   char *lp;
   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double x1 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double y1 = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double w = cnv_tok(lp);

   if ((lp = get_string(NULL, " \r\t\n")) == NULL)
      return 0;
   double h = cnv_tok(lp);

   double x2 = x1 + w;
   double y2 = y1 + h;

   DataStruct *data;
   if (!plane)  // already defined poly structure when plane is found
   {
      data = Graph_PolyStruct(lay, 0, FALSE);
      Graph_Poly(NULL, widthindex, filled, 0, 1);
      Graph_Vertex(x1,y1,0.0);
      Graph_Vertex(x2,y1,0.0);
      Graph_Vertex(x2,y2,0.0);
      Graph_Vertex(x1,y2,0.0);
      Graph_Vertex(x1,y1,0.0);
   } 
   else
   {   
      CPnt *pnt;
      pnt = new CPnt;
      pnt->bulge = (DbUnit)0.0;
      pnt->x = (DbUnit)x1;
      pnt->y = (DbUnit)y1;
      planePoly->getPntList().AddTail(pnt);
      
      pnt = new CPnt;
      pnt->bulge = (DbUnit)0.0;
      pnt->x = (DbUnit)x2;
      pnt->y = (DbUnit)y1;
      planePoly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->bulge = (DbUnit)0.0;
      pnt->x = (DbUnit)x2;
      pnt->y = (DbUnit)y2;
      planePoly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->bulge = (DbUnit)0.0;
      pnt->x = (DbUnit)x1;
      pnt->y = (DbUnit)y2;
      planePoly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->bulge = (DbUnit)0.0;
      pnt->x = (DbUnit)x1;
      pnt->y = (DbUnit)y1;
      planePoly->getPntList().AddTail(pnt);
   }

   return data;
}

/******************************************************************************
* write_netlist
*/
static int write_netlist()
{
   for (int i=0; i<netpincnt; i++)
   {
      if (netpinarray[i]->netpintype == netpinTestpoint || get_comp(netpinarray[i]->compname) > -1)
      {
         NetStruct *net = add_net(file, netpinarray[i]->netname);
         CompPinStruct *comppin = add_comppin(file, net, netpinarray[i]->compname, netpinarray[i]->pinname);
      }
      else
      {
         fprintf(ferr, "Component [%s] in Net [%s] not in $COMPONENT section\n", netpinarray[i]->compname, netpinarray[i]->netname);
         display_error++;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int is_command(char *l)
{
   int   i;

   for (i=0;i<SIZ_OF_COMMAND;i++)
   {
      if (!STRCMPI(command_lst[i].token,l))
         return i;
   }

   return -1;
}

/******************************************************************************
* loop_to_end
*/
static int loop_to_end()
{
   char *lp;
   while (get_nextline(ifp_line, GCAD_MAX_LINE, ifp))
   {
      // \r needed for VMS
      if ((lp = get_string(ifp_line, " \r\t\n")) == NULL)
         continue;
      if (!STRNICMP(lp, "$END", 4))
         return 1;
   }

   return -1;
}

/******************************************************************************
* clean_padprefix
   In GENCAD, a PAD can have the same name as a padstack.
*/
static int clean_padprefix()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (!(block->getFlags() & BL_APERTURE))
         continue;

      // here now check if a block with the original name exist. If not - change the real name and
      // kill the original name
      if (Graph_Block_Exists(doc, block->getOriginalName(), -1) == NULL)
      {
         block->setName(block->getOriginalName());
         block->setOriginalName("");
      }
   }

   return 1;
}

/******************************************************************************
* clean_padstackprefix
   In GENCAD, a PADSTACK can have the same name as other things
*/
static int clean_padstackprefix()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;
      if (block->getBlockType() != BLOCKTYPE_PADSTACK) 
         continue;

      // here now check if a block with the original name exist. If not - change the real name and
      // kill the original name
      if (Graph_Block_Exists(doc, block->getOriginalName(), -1) == NULL)
      {
         block->setName(block->getOriginalName());
         block->setOriginalName("");
      }
   }

   return 1;
}

/******************************************************************************
* update_modified_devices
*/
static int update_modified_devices()
{
   for (int i=0; i<compcnt; i++)
   {
      GENCADComp *comp = comparray[i];
      if (strlen(comp->modified_devicename))
      {
         // here copy from original device to modified_device
         TypeStruct *torig = AddType(file, comp->devicename);
         TypeStruct *tmod = AddType(file, comp->modified_devicename);
         doc->CopyAttribs(&tmod->getAttributesRef(), torig->getAttributesRef());
      }
   }

   return 1;
}

/******************************************************************************
* gcad_header
*/
static int gcad_header()
{
   char *lp;

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
     if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

     if (!STRNICMP(lp,"$END",4))
         return 1;
     // here do what header does     
     if (!STRCMPI(lp,"GENCAD"))
     {
         file->setBlockType(blockTypePcb);
         file->getBlock()->setBlockType(file->getBlockType());

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         double ver = atof(lp);
         if (ver != 1.3 && ver != 1.4 && ver != 1.2)
         {
            char tmp[80];
            sprintf(tmp,"Tested File Version [1.2, 1.3, 1.4] - detected Version [%1.1lf] at %ld.",ver,ifp_linecnt);
               ErrorMessage(tmp, "Untested GENCAD File Version - Proceed with CARE !", MB_OK | MB_ICONHAND);
         }
         file_type = GEN_CAD;
         gencad_version = round(ver*10);
     }
     else
     // here do what header does     
     if (!STRCMPI(lp,"GENPANEL"))
     {
         file->setBlockType(blockTypePanel);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         double ver = atof(lp);
         if (ver != 1.3 && ver != 1.4)
         {
            char tmp[80];
            sprintf(tmp,"Tested File Version [1.3, 1.4] - detected Version [%1.1lf] at %ld.",ver,ifp_linecnt);
               ErrorMessage(tmp,"Untested GENPANEL File Version - Proceed with CARE !", MB_OK | MB_ICONHAND);
         }
         file_type = GEN_PANEL;
         gencad_version = round(ver*10);

         // switch off the visible files
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos != NULL)
         {
            FileStruct *f = doc->getFileList().GetNext(pos);
            if (f->getBlockType() == blockTypePcb)  
            {
               f->setShow(false);
            }
         }
     }
     else
     if (!STRCMPI(lp,"USER"))
     {
       // do nothing
     }
     else
     if (!STRCMPI(lp,"DRAWING"))
     {
       // do nothing
     }
     else
     if (!STRCMPI(lp,"REVISION"))
     {
       // do nothing
     }
     else
     if (!STRCMPI(lp,"CADID"))
     {
       // do nothing
     }
     else
     if (!STRCMPI(lp,"ORIGIN"))
     {
       // do nothing
     }
     else
     if (!STRCMPI(lp,"INTERTRACK"))
     {
       // do nothing
     }
     else
     if (!STRCMPI(lp,"ATTRIBUTE"))
     {
       // do nothing
     }
     else
     if (!STRCMPI(lp,"UNITS"))
     {
       // 
       if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
       if (!STRCMPI(lp,"INCH"))
       {
         // this is OK, because all units are normalized to INCHES.
         scale_factor = Units_Factor(UNIT_INCHES, pageunits);
       }
       else
       if (!STRCMPI(lp,"THOU"))
       {
         // all following units are in MILS.
         scale_factor = Units_Factor(UNIT_MILS, pageunits);
       }
       else
       if (!STRCMPI(lp,"MM"))
       {
         // all following units are in Millimeter
         scale_factor = Units_Factor(UNIT_MM, pageunits);
       }
       else
       if (!STRCMPI(lp,"MM100"))
       {
         scale_factor = Units_Factor(UNIT_MM, pageunits);
         scale_factor = scale_factor / 100.0;
       }
       else
       if (!STRCMPI(lp,"USER"))
       {
         double a;
         // all following units are in INCHES
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         a = atof(lp);
         scale_factor = Units_Factor(UNIT_INCHES, pageunits);
         scale_factor = scale_factor / a;
       }
       else
       if (!STRCMPI(lp,"USERM"))
       {
         double a;
         // all following units are in centimeter
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         a = atof(lp);

         scale_factor = Units_Factor(UNIT_MM, pageunits) * 10; // centimeter, not mm
         scale_factor = scale_factor / a;
       }
       else
       if (!STRCMPI(lp,"USERMM"))
       {
         double a;
         // all following units are in millimeter
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         a = atof(lp);
         scale_factor = Units_Factor(UNIT_MM, pageunits);   // 
         scale_factor = scale_factor / a;
       }
       else
       {
          fprintf(ferr,"Unknown UNIT command [%s] unknown at %ld\n", lp,ifp_linecnt);
          display_error++;
       }
     }
     else
     {
         fprintf(ferr,"Error in $HEADER : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
     }

   }

   // error could not find logical end of section

   return -1;
}

/******************************************************************************
*/
static CString ArtworkName(CString basename)
{
   return "ART_" + basename;
}

/******************************************************************************
* gcad_board
*/
static int gcad_board()
{
   char     *lp;
   int      lay = Graph_Level("BOARD","", 0);
   int      widthindex = 0;     // Small width
   DataStruct  *d = NULL;
   CString  cur_type;
   CString  artworkname;
   int      graph_class = GR_CLASS_BOARDOUTLINE;

   set_usedlayer("BOARD");

   artworkname = "";
   cur_type = "";

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
         return 1;
   
      if (!STRCMPI(lp,"ARTWORK"))
      {
         graph_class = 0;
         if ((lp = get_string(NULL," \t")) == NULL)  continue; // artwork name
         artworkname = ArtworkName(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // layer
         set_usedlayer(lp);
         lay = Graph_Level(lp,"", 0);
         
         d = NULL;
         cur_type = "";
         routcont = FALSE;
      }
      else
      if (!STRCMPI(lp,"TYPE"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;
         cur_type = lp;
      }
      else
      if (!STRCMPI(lp,"LINE"))
      {
         d = do_line(lay,widthindex);

         if (d)
         {
            d->setGraphicClass(graph_class);
   
            if (strlen(cur_type) && cur_type.CompareNoCase(ATT_DDLINK) != 0)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->RegisterKeyWord(cur_type, TRUE, VT_STRING),
                  VT_STRING,
                  NULL,
                  SA_OVERWRITE, NULL); //  
            }
            if (strlen(artworkname))
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NAME, TRUE),
                  VT_STRING,
                  artworkname.GetBuffer(0),
                  SA_OVERWRITE, NULL); //  
            }
         }
      }
      else
      if (!STRCMPI(lp,"ARC"))
      {
         d = do_arc(lay,widthindex, 0);   // return NULL if arc is illegal
         if (d)
         {
            d->setGraphicClass(graph_class);

            if (strlen(artworkname))
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NAME, TRUE),
                  VT_STRING,
                  artworkname.GetBuffer(0),
                  SA_OVERWRITE, NULL); //  
            }
            d = NULL;
         }
      }
      else
      if (!STRCMPI(lp,"CIRCLE"))
      {
         d = do_circle(lay,widthindex,0,0);
         d->setGraphicClass(graph_class);

         if (strlen(artworkname))
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NAME, TRUE),
               VT_STRING,
               artworkname.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
         }
         d = NULL;
      }
      else
      if (!STRCMPI(lp,"RECTANGLE"))
      {
         d = do_rectangle(lay, widthindex, 0,0);       
         d->setGraphicClass(graph_class);

         if (strlen(artworkname))
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NAME, TRUE),
               VT_STRING,
               artworkname.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height
         }
         d = NULL;
      }
      else
      if (!STRCMPI(lp,"ATTRIBUTE"))
      {
         char  key[80],val[255];
         if (!get_attval(key, val)) continue;

         if (d == NULL)
            doc->SetUnknownAttrib(&file->getBlock()->getAttributesRef(), key,  val, SA_OVERWRITE, NULL); //  
         else
            doc->SetUnknownAttrib(&d->getAttributesRef(), key, val, SA_OVERWRITE, NULL); //  
      }
      else
      if (!STRCMPI(lp,"TEXT"))
      {        
         CString temp ="";
         double dbtemp;
         int inttemp;
         do_text(&temp,&dbtemp,&dbtemp,&dbtemp,&dbtemp,&dbtemp,&inttemp,&inttemp, 0);      
      }
      else
      if (!STRCMPI(lp,"TYPE"))
      {
         // here need to fix up attribute.
      }
      else
      if (!STRCMPI(lp,"CUTOUT"))
      {
         // do not do it.
         graph_class = graphicClassBoardCutout;
         lay = Graph_Level("BOARD","", 0);
         d = NULL;
      }
      else
      if (!STRCMPI(lp,"MASK"))
      {
         graph_class = 0;
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         set_usedlayer(lp);
         lay = Graph_Level(lp,"", 0);
         d = NULL;
      }
      else
      if (!STRCMPI(lp,"THICKNESS"))
      {
         double thickness;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         thickness = cnv_tok(lp);
         doc->SetAttrib(&file->getBlock()->getAttributesRef(),doc->IsKeyWord(BOARD_THICKNESS, TRUE),
            VT_UNIT_DOUBLE,
            &thickness, SA_OVERWRITE, NULL); // x, y, rot, height
      }
      else
      {
         fprintf(ferr,"Error in $BOARD : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
         d = NULL;
      }
   }

   // error could not find logical end of section

   return -1;
}


/****************************************************************************/
/*
   A pad is filled if aqll Line/Arc segments are consecutive. A gap
   will make it not filled.
*/
static int write_lastpad(const char *padname, const char *origpadname, int bnum, int lay, 
                         int lastapp)
{
   BlockStruct *b;

   // bnum is not the blocknumber, but the return from Graph_Ap... which returns the
   // widthindex.
   if (bnum > -1)
   {
      b= doc->Find_Block_by_Name(padname, -1);
      b->setOriginalName(origpadname);
   }
   else
   {
      // no aperture done ???
      //int r = 0;
   }

	if (polypntcnt == 0) return 0;

   switch (lastapp)
   {
      case T_OBLONG:
      {
         // width is the diameter of the arc
         // length and offset is taken from line
         // orientation is take from line
         double   offx = 0., offy = 0., w = 0., h = 0., ori = 0.;
         int err;
         int   i;


         // here get width
			for (i=1;i<polypntcnt;i++)
         {
				GENCADPolyPnt *pntI   = polypntarray.ElementAt(i);
				GENCADPolyPnt *pntI_1 = polypntarray.ElementAt(i - 1);

				if (polypntarray.ElementAt(i-1)->bulge)
            {
               // arc element
					w = sqrt(((pntI->y - pntI_1->y) * (pntI->y - pntI_1->y)) +
						      ((pntI->x - pntI_1->x) * (pntI->x - pntI_1->x)));
				}
            else
            {
               // line element
					h = sqrt(((pntI->y - pntI_1->y) * (pntI->y - pntI_1->y)) +
						((pntI->x - pntI_1->x) * (pntI->x - pntI_1->x)));
					ori = atan2((pntI->y - pntI_1->y), (pntI->x - pntI_1->x));
            }
         }

         //double www = RadToDeg(ori);
         if (w < (h+w)) // CAMCAD automatically adjusts a OBLONG so that W is always longer than H
         {
            ori += PI/2;
         }

         // find the aperture, which was defined at the PAD .. FINGER statement and 
         // overwrite the values with the correct values.
         // Graph_Aperture returns the widthtable, not the block !
         bnum = Graph_Aperture(padname, T_OBLONG, fabs(w), fabs(h+w), offx, offy, ori, 0, 
            BL_APERTURE, TRUE, &err);
         b = doc->getWidthTable()[bnum];
         b->setOriginalName(origpadname);
      }
      break;
      default:
      {
         // POLYGON, BULLET
         CString  complexpadname;

         // make a complex shape
         complexpadname.Format("COMPLEX_%s", padname);
         b = Graph_Block_On(GBO_APPEND,complexpadname,cur_filenum,0);

         DataStruct *d = Graph_PolyStruct(lay,0L,FALSE);
         int filled = 1;
         int closed = 1;
         int   i;

			for (i=1;i<polypntcnt;i++)
         {
				if (polypntarray.ElementAt(i)->startnew)
            {
               filled = closed = 0;
            }
         }

         Graph_Poly(NULL, 0, filled, 0, closed);
			for (i=0;i<polypntcnt;i++)
         {
				if (polypntarray.ElementAt(i)->startnew)
               Graph_Poly(NULL, 0, filled, 0, closed);

				Graph_Vertex(polypntarray.ElementAt(i)->x, polypntarray.ElementAt(i)->y, polypntarray.ElementAt(i)->bulge);
         }
         if(0 == filled)
         {
            MergeOrderedPolys(d->getPolyList());
            d->getPolyList()->GetHead()->setFilled(true);
            d->getPolyList()->GetHead()->setClosed(true);
         }
         Graph_Block_Off();

         // assign complex shape to complex aperture
         Graph_Complex(padname, 0, b, 0.0, 0.0, 0.0);

         // here just to have a complex aperture flagged as a PADSHAPE
         BlockStruct *cmplxap = Graph_Block_On(GBO_APPEND,padname,b->getFileNumber(),0); // complex ap's filenumber will match sub-block's filenumber
         cmplxap->setBlockType(blockTypePadshape);
         cmplxap->setOriginalName(origpadname);
         Graph_Block_Off();

      }
      break;
   }  // switch
   	
	polypntcnt = 0;

   return 1;
}

/****************************************************************************/
/*
  Pad defintions (padshapes) should be defined as apertures, where ever 
  possible. If a padopen == FALSE, then the aperture definition is done.
*/
static int gcad_pads()
{
   char     *lp;
   int      lay;                 // floating layers
   int      widthindex = 0;      // Small width
   CString  padname, origpadname;
   int      lastapp = T_COMPLEX;
   int      bnum = -1;                 // assigned aperture block

   padname = "DUMMY_PAD";
   origpadname = "";
   lay = Graph_Level("0","", 1);

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
      {
         if (padcnt) // must have had one pad minimum
            write_lastpad(padname, origpadname, bnum, lay, lastapp);
         return 1;
      }

      if (!STRCMPI(lp,"PAD"))
      {
         if (padcnt) // must have had one pad minimum
            write_lastpad(padname, origpadname, bnum, lay, lastapp);

         if ((lp = get_string(NULL," \r\t\n"))  == NULL) continue;

         GENCADPad *p = new GENCADPad;
         padarray.SetAtGrow(padcnt,p);  
         padcnt++;   
         p->padname = lp;
         origpadname = lp;
         bnum = -1;

         // here check if a block already exist with the same name.
         padname.Format("%s%s",padprefix, lp);
         if ((lp = get_string(NULL," \r\t\n"))  == NULL) continue;   // padtype

         // make an aperture where ever possible
         if (!STRCMPI(lp,"ROUND"))
         {
            // if a pad was not open , to make an aperture
            int err;
            Graph_Aperture(padname, T_ROUND, 0.0, 0.0 , 0.0, 0.0, 0.0, 0, 
               BL_APERTURE, TRUE, &err);
            lastapp = T_ROUND;
         }
         else
         if (!STRCMPI(lp,"RECTANGULAR"))
         {
            // if a pad was not open , to make an aperture
            int err;
            bnum = Graph_Aperture(padname, T_RECTANGLE, 0.0, 0.0 , 0.0, 0.0, 0.0, 0, 
               BL_APERTURE, TRUE, &err);
            lastapp = T_RECTANGLE;
         }
         else
         if (!STRCMPI(lp,"FINGER"))
         {
            // if a pad was not open , to make an aperture
            int err;
            bnum = Graph_Aperture(padname, T_OBLONG, 0.0, 0.0 , 0.0, 0.0, 0.0, 0, 
               BL_APERTURE, TRUE, &err);
            lastapp = T_OBLONG;
         }
         else
         if (!STRCMPI(lp,"ANNULAR"))
         {
            // if a pad was not open , to make an aperture
            int err;
            bnum = Graph_Aperture(padname, T_DONUT, 0.0, 0.0 , 0.0, 0.0, 0.0, 0, 
               BL_APERTURE, TRUE, &err);
            lastapp = T_DONUT;
         }
         else
/*
         if (!STRCMPI(lp,"HEXAGON"))
         {
            // if a pad was not open , to make an aperture
            int err;
            //Graph_Aperture(padname, T_RECTANGLE, 0.0, 0.0 , 0.0, 0.0, 0.0, 0, 
            // BL_APERTURE, TRUE, &err);
            //lastapp = T_ROUND;
         }
         else
         if (!STRCMPI(lp,"OCTAGON"))
         {
            // if a pad was not open , to make an aperture
            int err;
            Graph_Aperture(padname, T_OCTAGON, 0.0, 0.0 , 0.0, 0.0, 0.0, 0, 
               BL_APERTURE, TRUE, &err);
            lastapp = T_OCTAGON;
         }
         else
*/
         {
            lastapp = T_COMPLEX;
         }

         if ((lp = get_string(NULL," \r\t\n"))  == NULL) continue;   // drill
         p->drill = cnv_tok(lp);
      }
      else
      if (!STRCMPI(lp,"ARTWORK"))
      {
         if ((lp = get_string(NULL,"\n")) == NULL)  continue;
         set_usedlayer(lp);
         lay = Graph_Level(lp,"", 0);


      }
      else
      if (!STRCMPI(lp,"LINE"))
      {
         // here write polyline
         char     *lp;
         double   x1,y1,x2,y2;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         x1 = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         y1 = cnv_tok(lp);

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         x2 = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         y2 = cnv_tok(lp);

			GENCADPolyPnt *newpt;
			if (polypntcnt > 0)
         {
				int lastIndx = polypntcnt - 1;
				GENCADPolyPnt *oldpt = polypntarray[lastIndx];
				//if (polypntarray.ElementAt(lastIndx)->x != x1 || polypntarray.ElementAt(lastIndx)->y != y1)
            if (polypntarray[lastIndx]->x != x1 || polypntarray[lastIndx]->y != y1)
            {
					newpt = new GENCADPolyPnt;
               newpt->x = x1;
               newpt->y = y1;
               newpt->bulge = 0;
               newpt->startnew = 1;
					polypntarray.SetAtGrow(polypntcnt, newpt);
					polypntcnt++;
            }
         }
         else
         {
				newpt = new GENCADPolyPnt;
            newpt->x = x1;
            newpt->y = y1;
            newpt->bulge = 0;
            newpt->startnew = 0;
				polypntarray.SetAtGrow(polypntcnt, newpt);
				polypntcnt++;
         }

			newpt = new GENCADPolyPnt;
			newpt->x = x2;
			newpt->y = y2;
			newpt->bulge = 0;
			newpt->startnew = 0;
			polypntarray.SetAtGrow(polypntcnt, newpt);
			polypntcnt++;
      }
      else
      if (!STRCMPI(lp,"ARC"))
      {
         double   xs,ys,xe,ye,xc,yc;
         char     *lp;
         double   r,startangle, deltaangle;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         xs = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         ys = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         xe = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         ye = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         xc = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         yc = cnv_tok(lp);

         if (xs == xe && ys == ye)
         {
            ArcCenter2(xs,ys,xe,ye,xc,yc,&r,&startangle,&deltaangle, FALSE);
            // if a pad was not open , to make an aperture
            int err;
            bnum = Graph_Aperture(padname, T_ROUND, fabs(r*2), 0.0 , xc, yc, 0.0, 0, 
               BL_APERTURE, TRUE, &err);
            //lastapp = T_ROUND;
         }
         else
         {
            ArcCenter2(xs,ys,xe,ye,xc,yc,&r,&startangle,&deltaangle, FALSE);
            double   bulge = tan(deltaangle/4);
				GENCADPolyPnt *newPt;

				if (polypntcnt > 0)
            {
					int lastIndx = polypntcnt - 1;
					if (polypntarray.ElementAt(lastIndx)->x == xs && polypntarray.ElementAt(lastIndx)->y == ys)
               {
						polypntarray.ElementAt(lastIndx)->bulge = bulge;
               }
               else
               if (polypntarray.ElementAt(lastIndx)->x == xe && polypntarray.ElementAt(lastIndx)->y == ye)
               {
                  double   tmp;

                  // switch s <> e
                  tmp = xe;
                  xe = xs;
                  xs = tmp;
                  tmp = ye;
                  ye = ys;
                  ys = tmp;

						polypntarray.ElementAt(lastIndx)->bulge = -bulge;
               }
               else
               {
						newPt = new GENCADPolyPnt;
                  newPt->x = xs;
                  newPt->y = ys;
                  newPt->bulge = bulge;
                  newPt->startnew = 1;
						polypntarray.SetAtGrow(polypntcnt, newPt);
						polypntcnt++;
               }
            }
            else
            {
					newPt = new GENCADPolyPnt;
               newPt->x = xs;
               newPt->y = ys;
               newPt->bulge = bulge;
               newPt->startnew = 0;
					polypntarray.SetAtGrow(polypntcnt, newPt);
					polypntcnt++;
            }

				newPt = new GENCADPolyPnt;
            newPt->x = xe;
            newPt->y = ye;
            newPt->bulge = 0;
            newPt->startnew = 0;
				polypntarray.SetAtGrow(polypntcnt, newPt); 
				polypntcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"CIRCLE"))
      {
         double   x1,y1,r;
         char     *lp;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         x1 = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         y1 = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         r = cnv_tok(lp);

         // if a pad was not open , to make an aperture
         int err;
         bnum = Graph_Aperture(padname, T_ROUND, fabs(r*2), 0.0 , x1, y1, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
      }
      else
      if (!STRCMPI(lp,"RECTANGLE"))
      {
         // here make an aperture
         double   x1,y1;
         double   w,h;
         char     *lp;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         x1 = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         y1 = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         w = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  return 0;
         h = cnv_tok(lp);

         double ox = w/2 + x1;
         double oy = h/2 + y1;
         // if a pad was not open , to make an aperture
         int err;
         bnum = Graph_Aperture(padname, T_RECTANGLE, fabs(w), fabs(h), ox, oy, 0.0, 0, 
            BL_APERTURE, TRUE, &err);
      }
      else
      if (!STRCMPI(lp,"ATTRIBUTE"))
      {
         // do not do it.
      }
      else
      {
         fprintf(ferr,"Error in $PADS : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
   }

   write_lastpad(padname, origpadname, bnum, lay, lastapp);

   return -1;
}

/****************************************************************************/
/*
   check if this layer is a correct etch layer. Gencad files from other companies
   (i.e. GCT have silkscreen in a ROUTE section
*/
static int get_etchlayer(const char *l)
{
   if (!STRCMPI(l,"TOP"))                       return GR_CLASS_ETCH;
   if (!STRCMPI(l,"BOTTOM"))                    return GR_CLASS_ETCH;
   if (!STRNICMP(l,"INNER", strlen("INNER")))   return GR_CLASS_ETCH;
   if (!STRNICMP(l,"POWER", strlen("POWER")))   return GR_CLASS_ETCH;
   if (!STRNICMP(l,"GROUND", strlen("GROUND"))) return GR_CLASS_ETCH;
   return 0;
}

/****************************************************************************/
/* Region *CheckPlaneIntersect(CPolyList *polylist, int width)
*/
Region *CheckPlaneIntersect(CPolyList *polylist, int width, int lay, BOOL *voidPlane)  // if the plane is not inside of the previous planes, return its region
{  
   float scaleFactor = (float)(200.0 * Units_Factor(doc->getSettings().getPageUnits(), UNIT_INCHES)); //create 200 boxes per unit inch square

   if (polylist->GetCount() > 0)
   {
      Region *planeRegion, *intersection;
      Plane *Node;
      BOOL FoundPlane = FALSE;
      planeRegion = RegionFromPolylist(doc, polylist, scaleFactor);
      POSITION Pos = planeList.GetTailPosition();
      int curlay = -1;
      if (Pos != NULL)
      {
         Node = planeList.GetPrev(Pos);  // this is the plane to check if this is void or not 
         curlay = Node->PlaneData->getLayerIndex();
      }
      while(Pos != NULL)
      {
         Node = planeList.GetPrev(Pos);
         if (curlay != Node->PlaneData->getLayerIndex()) // if planes are on different layers then ignore 
            continue;
         if (Node->planeRegion == NULL)
            continue;
         intersection = IntersectRegions(Node->planeRegion, planeRegion);
         if (intersection)
         {           
            if (Node->voidPlane == TRUE)
               *voidPlane = FALSE;
            else
            {
               *voidPlane = TRUE;
               Graph_Poly(Node->PlaneData, width, 1, 1, 1);  // make Plane void if the plane is inside of the previous plane
                                                // points will be added after this funtion call
               Plane *LastNode = planeList.GetTail();  // since the the plane is void, delete the polystruct and get the parent polystruct
               RemoveOneEntityFromDataList(doc, &file->getBlock()->getDataList(), LastNode->PlaneData);
               LastNode->PlaneData = Node->PlaneData;
            }
            delete intersection;
            return planeRegion;        
         }
      }
      *voidPlane = FALSE;
      return planeRegion;  //if the plane is not inside of any previous planes, then keep the plane region to test against other following planes
   }
   *voidPlane = TRUE;
   return NULL;
}

/****************************************************************************/
/* void DrawPlane(int widthindex, int lay, int gclass, CString netname)
*/
void DrawPlane(int widthindex, int lay, int gclass, CString netname, CString planename)
{
   if (planePolyList->GetCount() == 0)
   {
      planePoly->setClosed(true);  
      planePoly->setFilled(true);
      planePoly->setVoid(false);
      planePoly->setWidthIndex(widthindex);
      planePolyList->remove(planePoly);
      planePolyList->AddTail(planePoly);
   }
   else
   {
      BOOL voidPlane;
      Region *newPlaneRegion = CheckPlaneIntersect(planePolyList, planePoly->getWidthIndex(), lay, &voidPlane);
      if (newPlaneRegion != NULL && !voidPlane) // If plane is not void
      {        
         Plane *Node = planeList.GetTail();
         DataStruct *d = Node->PlaneData;

         CPoly *cp = Graph_Poly(d, planePoly->getWidthIndex(), 1, 0, 1); 
         cp->setFloodBoundary(true);
         Node->planeRegion = newPlaneRegion; // Since plane is not void, keep the region to test against following planes
         Node->voidPlane = FALSE;
      }
      else
      {        
         if (newPlaneRegion !=NULL)
         {
            Plane *Node = planeList.GetTail();
            Node->planeRegion = newPlaneRegion; 
            Node->voidPlane = TRUE;
         }
         
      }

      POSITION polyListPos = planePolyList->GetHeadPosition();
      CPoly *poly = planePolyList->GetAt(polyListPos);  // Add the points to the poly
      POSITION polyPos = poly->getPntList().GetHeadPosition();
      while(polyPos != NULL)
      {
         CPnt *point = poly->getPntList().GetNext(polyPos);
         Graph_Vertex(point->x,point->y,point->bulge);
      }

      while(planePoly->getPntList().GetCount())            
         delete planePoly->getPntList().RemoveHead(); 

      planePolyList->remove(planePoly);
      planePolyList->empty();
      planePoly->setClosed(true);  
      planePoly->setFilled(true);
      planePoly->setVoid(false);
      planePoly->setWidthIndex(widthindex);
      planePolyList->AddTail(planePoly);           
   }
   //first add to the data structure so it gets right layer number and netname, etc
   DataStruct *d = Graph_PolyStruct(lay,0L,0);
   d->setGraphicClass(gclass);
   if (strlen(netname))
   {
      doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
         VT_STRING,
         netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
   } 

   // keep all new data in planelist. Tf this data is void planes then it will be deleted from planelist later
   Plane *NewPlane = new Plane();
   NewPlane->PlaneData = d;
   NewPlane->planeRegion = NULL;
   NewPlane->voidPlane = FALSE;
   //NewPlane->planename = planename;
   planeList.AddTail(NewPlane);
}

/****************************************************************************/
/* void ByName(int widthindex, int lay, int gclass, CString netname, CString planename)
*/
void DrawPlaneByName(DataStruct* &curData, int widthindex, int lay, int gclass, CString netname, CString planename)
{
   // Needs curdata cause it needs to know where to add the poly points
   if (planePolyList->GetCount() == 0)
   {
      planePolyList->remove(planePoly);
      planePolyList->AddTail(planePoly);
   }
   else
   {
      CPoly *curPoly = curData->getPolyList()->GetTail();
      POSITION polyListPos = planePolyList->GetHeadPosition();
      CPoly *poly = planePolyList->GetAt(polyListPos);  // Add the points to the poly
      POSITION polyPos = poly->getPntList().GetHeadPosition();
      while (polyPos != NULL)
      {        
         CPnt *point = poly->getPntList().GetNext(polyPos);
         CPnt *newPnt = new CPnt(*point);
         curPoly->getPntList().AddTail(newPnt);
      }

      while(planePoly->getPntList().GetCount())            
         delete planePoly->getPntList().RemoveHead();  

      planePolyList->remove(planePoly);
      planePolyList->empty();
      planePolyList->AddTail(planePoly);  
   }

   if (!planename.IsEmpty())
   {
      bool isVoid = (planename.Find("VOID") > -1);

      if(!curData || !isVoid)
      {
         curData = Graph_PolyStruct(lay,0L,0);
         curData->setGraphicClass(gclass);
         if (strlen(netname))
         {
            doc->SetAttrib(&curData->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
               VT_STRING,
               netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
         }  
      }

      //create poly for curData
      if (isVoid) // if find void then make the poly void under the same poly structure
         CPoly *cp = Graph_Poly(NULL, widthindex, 1, 1, 1);
      else
      {
         CPoly *cp = Graph_Poly(NULL, widthindex, 1, 0, 1); 
         cp->setFloodBoundary(true);         
      }
   }

}

/****************************************************************************/
/* gcad_routes()
*/
int gcad_routes()
{
   int retval = -1;

   char     *lp;
   CString  netname = "", layername = "", vianame = "";
   int      lay =-1;
   int      widthindex = 0;     // Small width
   CString  lastvia = "";
   CString  origtrackcode, lasttrackcode;
   CString  padname;
   CString  origpadname;
   int      plane = 0;
   CString  planename;
   int      drilllayernum = Graph_Level("DRILLHOLE","",0);
   int      gclass;
   BOOL     FirstPlanePoint;
   planePolyList = new CPolyList;
   planePoly = new CPoly;
   origtrackcode = "";
   lasttrackcode = "x";
   netname = "";
   routcont = FALSE;

   DataStruct* planeDataStruct = NULL;

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp, "$END", 4))
      {
         if (USE_PLANE_NAMES)
            DrawPlaneByName(planeDataStruct, widthindex, lay, gclass, netname, "");
         else
         {
            DrawPlane(widthindex, lay, gclass, netname, "");
            Plane *LastNode = planeList.GetTail();  // delete last node datastruct since it's a dummy datastruct
            RemoveOneEntityFromDataList(doc, &file->getBlock()->getDataList(), LastNode->PlaneData);
         }
         FreePolyList(planePolyList);
         retval = 1;

         break;
      }
   
      if (!STRCMPI(lp, "PLANE"))
      {
         if ((lp = get_string(NULL,"\n")) == NULL)  continue;  // planename
         planename = lp;
         plane = 1;
         FirstPlanePoint = TRUE;
         planename.MakeUpper();

         if (USE_PLANE_NAMES)
            DrawPlaneByName(planeDataStruct, widthindex, lay, gclass, netname, planename);
         else     
            DrawPlane(widthindex, lay, gclass, netname, planename);
      }
      else if (!STRCMPI(lp, "ARTWORK"))
      {
         plane = 0;
         if ((lp = get_string(NULL,"\n")) == NULL)  continue;
         layername = lp;
         set_usedlayer(lp);
         lay = Graph_Level(lp,"", 0);

         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "LINE"))
      {
         if (lay < 0)
         {
            lay = Graph_Level("ROUTES","", 0);
            gclass = 0;
         }
         do_routeline(lay,widthindex,netname, plane, gclass, FirstPlanePoint);
      }
      else if (!STRCMPI(lp, "VIA"))
      {
         plane = 0;

         // via
         double x, y, drillsize;
         DataStruct  *d;
         routcont = FALSE;

         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         origpadname = lp;
         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         y = cnv_tok(lp);

         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         layername = lp;
         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         drillsize = cnv_tok(lp);

         vianame = "";
         if ((lp = get_string(NULL," \r\t\n")) != NULL) 
            vianame = lp;

         // if no padname is there, that means that a $PAD is used and not a $padstack
         // if $pad is used
         if (lastvia != origpadname)
         {
            padname.Format("%s%s",padstackprefix, origpadname);
            if (get_padshape(origpadname) > -1)
            {
               // here check if not defined yet
               padname.Format("%s_%s",origpadname,layername);
               if (Graph_Block_Exists(doc, padname, -1))
               {
                  // do nothing
               }
               else
               {
                  set_usedlayer(layername);
                  lay = Graph_Level(layername,"", 0);
   
                  CString  pname;
                  // here make one.
                  BlockStruct *b = Graph_Block_On(GBO_APPEND,padname,cur_filenum,0);
                  b->setBlockType(BLOCKTYPE_PADSTACK);
                  pname.Format("%s%s",padprefix,origpadname);
                  Graph_Block_Reference(pname, NULL, cur_filenum, 0.0, 0.0,0.0, 0, 1.0, lay, TRUE);
                  Graph_Block_Off();         
               }
            }
            else
            {
               // not found in get_padshape
               if (!Graph_Block_Exists(doc, padname, -1))
               {
                  // there was no padshape and padstack. This is found on old lucent designs.
                  // make a padstack with and oversized drill size on all layers.
                  BlockStruct *b = Graph_Block_On(GBO_APPEND,padname,cur_filenum,0);
                  b->setBlockType(BLOCKTYPE_PADSTACK);

                  if (drillsize > 0)
                  {
                     int drillindex = get_drillindex(drillsize, drilllayernum);
                     if (drillindex > -1)
                     {
                        CString  drillname;
                        drillname.Format("DRILL_%d",drillindex);
                        // must be filenum 0, because apertures are global.
                        Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, drilllayernum, TRUE);
                     }
                     double   viasize = viadrill_oversize * Units_Factor(UNIT_INCHES, pageunits);
                     viasize += drillsize;
                     int err;
                     Graph_Aperture("$$VIA", T_ROUND, viasize, 0.0 , 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
                     int lay = Graph_Level("ALL","",0);
                     Graph_Block_Reference("$$VIA", NULL, cur_filenum, 0.0, 0.0, 0.0, 0, 1.0, lay, TRUE);
                  }
                  Graph_Block_Off();
               }
            }

            lastvia = origpadname;
         }
         d = Graph_Block_Reference(padname, vianame, cur_filenum,x,y,0.0,0,1.0, -1,FALSE);
         d->getInsert()->setInsertType(insertTypeVia);
         if (strlen(netname))
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
               VT_STRING, netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
         }
         GENCADFeature *c = new GENCADFeature;
         featurearray.SetAtGrow(featurecnt,c);  
         featurecnt++;  
         c->x = x;
         c->y = y;
         c->netname = netname;
         c->type = 1;
         c->feature_data = d;
      }
      else if (!STRCMPI(lp, "TESTPAD"))
      {

         plane = 0;

         // via
         double x, y, rot;
         DataStruct  *d;
         routcont = FALSE;
         int   mir = 0;

         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         origpadname = lp;
         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         y = cnv_tok(lp);

         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         rot = atof(lp);

         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue; // mirror
         if (!STRNICMP(lp,"MIRR",4))
            mir = TRUE;

         vianame = "";
         if ((lp = get_string(NULL," \r\t\n")) != NULL) 
            vianame = lp;

			CString testPointBlockName;
			testPointBlockName.Format("TESTPOINT %s", origpadname);

			if (!Graph_Block_Exists(doc, testPointBlockName, cur_filenum, blockTypeUndefined))
			{
				BlockStruct *testPointBlock = Graph_Block_On(GBO_APPEND, testPointBlockName, cur_filenum, 0);

				padname.Format("%s%s",padstackprefix, origpadname);
				if (get_padshape(origpadname) > -1)
				{
					// here check if not defined yet
					padname.Format("%s_%s",origpadname,layername);
					if (Graph_Block_Exists(doc, padname, -1))
					{
						// do nothing
					}
					else
					{
						set_usedlayer(layername);
						lay = Graph_Level(layername,"", 0);

						CString  pname;
						// here make one.
						BlockStruct *b = Graph_Block_On(GBO_APPEND,padname,cur_filenum,0);
						b->setBlockType(BLOCKTYPE_PADSTACK);
						pname.Format("%s%s",padprefix,origpadname);
						Graph_Block_Reference(pname, NULL, cur_filenum, 0.0, 0.0,0.0, 0, 1.0, lay, TRUE);
						Graph_Block_Off();         
					}
				}

				// Make pin 1 insert of padstack
				d = Graph_Block_Reference(padname, "1", cur_filenum,0.0,0.0,0.0, 0, 1.0, -1, FALSE);
				d->getInsert()->setInsertType(insertTypePin);

				Graph_Block_Off();
			}


			d = Graph_Block_Reference(testPointBlockName, vianame, cur_filenum,
				x, y, DegToRad(rot), 0, 1.0, -1, FALSE);
         d->getInsert()->setInsertType(insertTypeTestPoint);

         if (mir) // graphic mirror
            d->getInsert()->setMirrorFlags(MIRROR_FLIP);
         if (!layername.CompareNoCase("BOTTOM"))
            d->getInsert()->setPlacedBottom(true);

         if (strlen(netname))
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
               VT_STRING,
               netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
         }

         GENCADFeature *c = new GENCADFeature;
         featurearray.SetAtGrow(featurecnt,c);  
         featurecnt++;  
         c->x = x;
         c->y = y;
         c->netname = netname;
         c->type = 0;
         c->feature_data = d;

			// Add to netpin list, to get it an entry as member of this net
         GENCADNetpin *np = new GENCADNetpin;
         netpinarray.SetAtGrow(netpincnt,np);  
         netpincnt++;   
         np->netname = netname;
         np->compname = vianame;
         np->pinname = "1";
			np->netpintype = netpinTestpoint;

      }
      else if (!STRCMPI(lp, "ROUTE"))
      {
         plane = 0;
         // signal name
			// Get the netname using backslash as delimiter only if it precedes a quote
         if ((lp = get_string(NULL," \r\t\n", "\"")) == NULL)  continue;
         netname = lp;
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "TRACK"))
      {
         plane = 0;

         // track width
         CString  tname;
         int   err;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         origtrackcode = lp;
   
         if (origtrackcode != lasttrackcode)
         {
            tname.Format("TRACK_%s",lp);
            widthindex = Graph_Aperture(tname, T_ROUND, 0.0, 0.0,0.0, 0.0, 0.0,0,BL_WIDTH,FALSE, &err);
            routcont = FALSE;
            lasttrackcode = origtrackcode;
         }
      }
      else if (!STRCMPI(lp, "ARC"))
      {
         DataStruct  *d = do_arc(lay,widthindex, plane);
         if (d && !plane)  // returns NULL if arc is wrong
         {
            d->setGraphicClass(gclass);
            if (strlen(netname) && gclass == GR_CLASS_ETCH)
            {
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE),
                  VT_STRING,
                  netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
            }
         }
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "CIRCLE"))
      {           
         DataStruct *d = do_circle(lay,widthindex,0,plane);
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "RECTANGLE"))
      {
         DataStruct *d = do_rectangle(lay, widthindex, 0,plane);
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "ATTRIBUTE"))
      {
         // do not do it.
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "LAYER"))
      {
         plane = 0;
         if ((lp = get_string(NULL," \r\t\n")) == NULL)
				continue;         
         layername = lp;
         set_usedlayer(lp);            
         lay = Graph_Level(lp,"", 0);           
         //prelay = lay;
         routcont = FALSE;
         gclass = get_etchlayer(lp);
      }
      else
      {
         fprintf(ferr,"Error in $ROUTES : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
   }

   // error could not find logical end of section

   return retval;
}

/****************************************************************************/
/*
*/
static int gcad_tracks()
{
   char     *lp;
   int      lay =-1;
   int      widthindex = 0;     // Small width

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
     if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

     if (!STRNICMP(lp,"$END",4))
         return 1;
   
     if (!STRCMPI(lp,"TRACK"))
     {
       // track width
       char  tname[80];
       int   err;
       double    w;

       if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
       sprintf(tname,"TRACK_%s",lp);
       if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
       w = cnv_tok(lp);
       if (w < 0) // unknown trackwidth
          w = 0;
       Graph_Aperture(tname, T_ROUND, w, 0.0,0.0, 0.0, 0.0,0, BL_WIDTH,TRUE, &err);
     }
     else
     {
       fprintf(ferr,"Error in $TRACKS : command [%s] unknown at %ld\n",lp,ifp_linecnt);
       display_error++;
     }
   }

   // error could not find logical end of section
   return -1;
}

/****************************************************************************/
/*
*/
static int gcad_padstacks()
{
   char     *lp;
   int      lay =-1;
   int      widthindex = 0;     // Small width
   int      padstackopen = FALSE;
   CString  padstackname;
   BlockStruct *curblock;
   int      cur_smdpad = 0;
   int      drilllayernum = Graph_Level("DRILLHOLE","",0);

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
      {
         if (padstackopen)
         {
            Graph_Block_Off();

            if (cur_smdpad == 1)
            {
               doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
               VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height
            }
         }
         return 1;
      }

      if (!STRCMPI(lp,"PADSTACK"))
      {
         if (padstackopen)
         {
            Graph_Block_Off();
         }
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         padstackname.Format("%s%s", padstackprefix, lp);

         double drill = 0;

         // UniCAM Gencad output does not have a drill size.
         if ((lp = get_string(NULL," \r\t\n")) != NULL)  
            drill = cnv_tok(lp);

         curblock = Graph_Block_On(GBO_APPEND,padstackname,cur_filenum,0);
         curblock->setBlockType(BLOCKTYPE_PADSTACK);
         cur_smdpad = 0;
         padstackopen = TRUE;

         if (drill > 0)
         {
            int drillindex = get_drillindex(drill, drilllayernum);
            if (drillindex > -1)
            {
               CString  drillname;
               drillname.Format("DRILL_%d",drillindex);
               // must be filenum 0, because apertures are global.
               Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                         0, 1.0, drilllayernum, TRUE);
            }
         }
      }
      else
      if (!STRCMPI(lp,"PAD"))
      {
         CString  pname;
         double rot;
         int   mir;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         pname.Format("%s%s",padprefix,lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         set_usedlayer(lp);
         lay = Graph_Level(lp,"", 0);

         if (!STRCMPI(lp,"TOP"))
            cur_smdpad = 1;
         else
            cur_smdpad = 0;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         rot = atof(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         mir = atoi(lp);
         Graph_Block_Reference(pname, NULL, cur_filenum, 0.0, 0.0, DegToRad(rot), 0, 1.0, lay, TRUE);
      }
      else
      if (!STRCMPI(lp,"ATTRIBUTE"))
      {
         /// nothing
      }
      else
      {
         fprintf(ferr,"Error in $PADSTACKS : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
   }

   // error could not find logical end of section

   return -1;
}

/****************************************************************************/
/*
*/
static int write_mechanical(const char *mechanical, const char *shapename, 
                            const char *layer, double x, double y, double rot, int mirror)
{
   if (strlen(mechanical) == 0)  return 0;
   if (strlen(shapename) == 0)   return 0;
   if (strlen(layer) == 0)       return 0;
   
   DataStruct *d = Graph_Block_Reference(shapename, mechanical, cur_filenum, x, y,DegToRad(rot),
        mirror, 1.0, Graph_Level(layer,"",0), FALSE);
   d->getInsert()->setInsertType(insertTypeMechanicalComponent);

/*
   if (complayerbottom && !mirror)
      d->getInsert()->getMirrorFlags() = PLACE_BOTTOM;
   else
   if (!complayerbottom && mirror)
      d->getInsert()->getMirrorFlags() = MIRROR_FLIP;
*/
   return 1;
}

/****************************************************************************/
/*
*/
static int write_oldfiducial(const char *fiducialname, const char *layer, double x, double y)
{
   if (strlen(layer) == 0) return 0;
   if (strlen(fiducialname) == 0)   return 0;

   if (Graph_Block_Exists(doc, "GENCAD_FIDUCIAL", -1))
   {
      // do nothing
   }
   else
   {
      int   err;
      double   w = 0.1 * Units_Factor(UNIT_INCHES, pageunits);
      Graph_Aperture("FID", T_TARGET, w, w, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
      BlockStruct *b = Graph_Block_On(GBO_APPEND,"GENCAD_FIDUCIAL",cur_filenum,0);
      b->setBlockType(BLOCKTYPE_FIDUCIAL);
      Graph_Block_Reference("FID", NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, Graph_Level("0","",1), TRUE);
      Graph_Block_Off();
   }
      
   DataStruct *d = Graph_Block_Reference("GENCAD_FIDUCIAL", fiducialname, cur_filenum, x, y, 0.0, 
      0, 1.0, Graph_Level(layer,"",0), TRUE);
   if (!STRCMPI(layer,"BOTTOM"))
      d->getInsert()->setPlacedBottom(true);
   d->getInsert()->setInsertType(insertTypeFiducial);
   
   return 1;
}

static bool isBottomBuilt(CString blockname)
{
   bool hasTop = false;
   bool hasBot = false;

   BlockStruct *b = doc->Find_Block_by_Name(blockname, -1);
   if (b != NULL)
   {
      POSITION pos = b->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *d = b->getNextDataInsert(pos);
         if (d->isInsert())
         {
            int inBlkNum = d->getInsert()->getBlockNumber();
            bool isBot = d->getInsert()->getPlacedBottom();
            int layIndx = d->getLayerIndex();
            LayerStruct *lp = doc->getLayer(layIndx);
            CString layName = lp->getName();
            layName.MakeUpper();
            if (isBot || layName.Find("BOT") > -1)
            {
               hasBot = true;
            }
            else
            {
               hasTop = true;
            }
         }
      }
   }

   return (hasBot && !hasTop);
}

/****************************************************************************/
/*
*/
static int gcad_shapes()
{
   char        *lp;
   int         lay;
   int         silklay = Graph_Level("SILKSCREEN_TOP","", 0);
   int         widthindex = 0;     // Small width
   int         shapeopen = FALSE;
   CString     shapename;
   int         graph_class = GR_CLASS_COMPOUTLINE;
   DataStruct  *d;
   BlockStruct *cur_shape;
   int         cur_smdpad = 0;
   CString     fiducialname, layer;
   double      x,y;
   bool        bottomBuilt = false;

   routcont = FALSE;
   fiducialname = "";

   set_usedlayer("SILKSCREEN_TOP");
   set_usedlayer("SILKSCREEN_BOTTOM");

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         if (shapeopen)
            Graph_Block_Off();
         return 1;
      }
      
      if (!STRCMPI(lp,"SHAPE"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         if (shapeopen)
            Graph_Block_Off();
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         shapename = lp;
         cur_shape = Graph_Block_On(GBO_APPEND,shapename,cur_filenum,0);
         cur_shape->setBlockType(BLOCKTYPE_PCBCOMPONENT);
         shapeopen = TRUE;
         routcont = FALSE;
         bottomBuilt = false;
      }
      else if (!STRCMPI(lp, "PIN"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         CString pinname = lp;

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         CString origpadname = lp;
			CString padname = "";
			padname.Format("%s%s", padstackprefix, lp);

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         double x = cnv_tok(lp);

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         double y = cnv_tok(lp);

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         CString layername = lp; // this layer is used only if a padshape is defined, not a padstack

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         double rot = atof(lp);

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;   // Graphic mirror
         int mir = FALSE;
         if (!STRCMPI(lp, "MIRRORY"))
            mir = TRUE;
         else if (!STRCMPI(lp, "MIRRORX"))
         {
            rot += 180;
            mir = TRUE;
         }

         // if no padname is there, that means that a $PAD is used and not a $padstack
         // if $pad is used
         if (get_padshape(origpadname) > -1)
         {
				// Make the pad stack name unique for each pad shape on a specific layer
				padname.Format("%s%s_%s", padstackprefix, origpadname, layername);

            // padname is the newly created padstack. This name must be different from the $PADS name
            if (!Graph_Block_Exists(doc, padname, -1))
            {
               set_usedlayer(layername);
               lay = Graph_Level(layername,"", 0);

               CString  pname;
               // here make one.
					// Create padstack
               BlockStruct *b = Graph_Block_On(GBO_APPEND, padname, cur_filenum, 0);
               b->setBlockType(BLOCKTYPE_PADSTACK);
					// Add pad insert
               pname.Format("%s%s", padprefix, origpadname);
               Graph_Block_Reference(pname, NULL, cur_filenum, 0.0, 0.0, 0.0, 0, 1.0, lay, TRUE);
               Graph_Block_Off();         

               if (!STRCMPI(layername, "TOP"))
                  cur_smdpad = 1;
               else
                  cur_smdpad = 0;

               if (cur_smdpad == 1)
                  doc->SetAttrib(&b->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE), VT_NONE,  NULL, SA_OVERWRITE, NULL); 
            }
         }

			// Create insert of padstack aka insert of pin
         DataStruct *data = Graph_Block_Reference(padname, pinname, cur_filenum, x, y,DegToRad(rot), 0, 1.0, -1, TRUE);
         
         if (mir) // graphic mirror
            data->getInsert()->setMirrorFlags(MIRROR_FLIP);

         if (!layername.CompareNoCase("BOTTOM"))
         {
            data->getInsert()->setPlacedBottom(true);
         }
         else
         {
            if (isBottomBuilt(padname))
            {
               bottomBuilt = true;
            }
         }

         data->getInsert()->setInsertType(insertTypePin);
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "ARTWORK"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         routcont = FALSE;
         graph_class = 0;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // artwork name
         CString aname( ArtworkName(lp) );
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // x
         double x = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // y
         double y = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // rot
         double rot = atof(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // mirror
         int mir = atoi(lp);
         DataStruct *d = Graph_Block_Reference(aname, NULL, cur_filenum, x, y, DegToRad(rot), mir, 1.0, -1, TRUE);
      }
      else
      if (!STRCMPI(lp,"MASK"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         graph_class = 0;
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         set_usedlayer(lp);
         lay = Graph_Level(lp,"", 0);
      }
      else
      if (!STRCMPI(lp,"LINE"))
      {
         d = do_line(silklay,widthindex);
         if (d)
            d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"ARC"))
      {
         d = do_arc(silklay,widthindex, 0);
         if (d)
            d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"CIRCLE"))
      {
         routcont = FALSE;
         d = do_circle(silklay,widthindex,0,0);
         d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"RECTANGLE"))
      {
         routcont = FALSE;
         d = do_rectangle(silklay, widthindex, 0,0);
         d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"INSERT"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         CString  ins;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         ins = lp;

         if(!STRCMPI(ins,"SMD"))
         {
            doc->SetAttrib(&cur_shape->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
               VT_NONE, NULL, SA_OVERWRITE, NULL); // x, y, rot, height
         }
      }
      else
      if (!STRCMPI(lp,"FIDUCIAL"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";
         // if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
         // this is followed by place and layer
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // fid_name
         fiducialname = lp;
      }
      else
      if (!STRCMPI(lp,"PLACE"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         y = cnv_tok(lp);
      }
      else
      if (!STRCMPI(lp,"LAYER"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         layer = lp;
      }
      else
      if (!STRCMPI(lp,"FID"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         double x, y, rot;
         int    mir, lay;
         CString fidname, padname, origpadname, layername;

         //FID partfid_0 SM184QFP256L_F_fid_0 -6555 -6555 TOP 0 0
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // fid_name
         fidname = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // pad_name
         origpadname = lp;
         padname.Format("%s%s",padstackprefix, lp);

         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // x
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // y
         y = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // layer
         layername = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // rot
         rot = atof(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // mirror
         mir = atoi(lp);

         // if no padname is there, that means that a $PAD is used and not a $padstack
         // if $pad is used
         if (get_padshape(origpadname) > -1)
         {
            // here check if not defined yet

            // if a padshape is BOTTOM, this means, use this for BOTTOM placement.
            // however, on bottom placement, the mirror layer is used.
            if (!strcmp(layername,"BOTTOM"))
               layername = "TOP";

            // padname is the newly created padstack. This name must be different from the $PADS name
            //padname.Format("%s_%s",origpadname,layername);
            //padname = origpadname;

            if (Graph_Block_Exists(doc, padname, -1))
            {
               // do nothing
            }
            else
            {
               set_usedlayer(layername);
               lay = Graph_Level(layername,"", 0);

               CString  pname;
               // here make one.
               BlockStruct *b = Graph_Block_On(GBO_APPEND,padname,cur_filenum,0);
               b->setBlockType(BLOCKTYPE_PADSTACK);
               pname.Format("%s%s",padprefix, origpadname);
               Graph_Block_Reference(pname, NULL, cur_filenum, 0.0, 0.0,0.0, 0, 1.0, lay, TRUE);
               Graph_Block_Off();
            }
         }
         DataStruct *d = Graph_Block_Reference(padname, fidname, cur_filenum, x, y,DegToRad(rot), mir, 1.0, -1, TRUE);
         d->getInsert()->setInsertType(insertTypeFiducial);
      }
      else
      if (!STRCMPI(lp,"HEIGHT"))
      {
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // y
         double height = cnv_tok(lp);
         doc->SetAttrib(&cur_shape->getAttributesRef(),doc->IsKeyWord(ATT_COMPHEIGHT, TRUE),
            VT_UNIT_DOUBLE, &height, SA_OVERWRITE, NULL); // x, y, rot, height
      }
      else
      if (!STRCMPI(lp,"ATTRIBUTE"))
      {
         char  key[80],val[255];

         if (!get_attval(key, val)) continue;

         doc->SetUnknownAttrib(&cur_shape->getAttributesRef(), key,  val, SA_APPEND, NULL); // x, y, rot, height
      }
      else
      {
         fprintf(ferr,"Error in $SHAPES : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }

      if (bottomBuilt)
      {
         // All silkscreen layer stuff was put on top. If item is bottom
         // built then move all these to silkscreen bottom.

         if (cur_shape != NULL)
         {
            POSITION pos = cur_shape->getHeadDataPosition();
            while (pos != NULL)
            {
               DataStruct *d = cur_shape->getNextData(pos);
               int layIndx = d->getLayerIndex();
               LayerStruct *layptr = doc->getLayer(layIndx);
               if (layptr != NULL)
               {
                  CString lnam = layptr->getName();
                  if (lnam.Compare("SILKSCREEN_TOP") == 0)
                  {
                     d->setLayerIndex(Graph_Level("SILKSCREEN_BOTTOM","", 0));
                  }
               }
            }
         }
      }

   }

   // error could not find logical end of section

   return -1;
}

/****************************************************************************/
/*
*/
static int gcad_components()
{
   char     *lp;
   int      lay =-1;
   int      startcomp = FALSE;
   CString  compname;
   CString  partname;
   CString  shapename;
   CString textrefname;
   double   textx, texty, textheight, textcharwidth, textrot;
   int textmir, textlay;
   double   x,y,rot = 0;
   int      mirror = 0, mirrorlayer = 0;
   Att      *att;
   int      attcnt;
   int      complayerbottom = FALSE;

   att = (Att *)calloc(GCAD_MAX_ATT,sizeof(Att));
   attcnt = 0;

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
      {
         // here write comp
         if (startcomp) 
         {  
            GENCADComp *c = new GENCADComp;
            comparray.SetAtGrow(compcnt,c);  
            compcnt++;  
            c->compname = compname;
            c->devicename = partname;
            c->modified_devicename = "";

            // here check layer bottom without flip
            if (complayerbottom && !mirror)
            {
               fprintf(ferr,"Component [%s] [%s] placed on BOTTOM, but not mirrored\n",compname, shapename);
               display_error++;
/*
               // make a mirror shapename
               int   i;

               if ((i = get_mirrorshape1(shapename)) > -1)
               {
                  strcpy(shapename,mirrorshapearray[i]->shape2);
                  mirror = TRUE;
               }
               else
               if ((i = get_mirrorshape2(shapename)) > -1)
               {
                  strcpy(shapename,mirrorshapearray[i]->shape1);
                  mirror = TRUE;
               }
               else
               {
                  // if mirrorshape does not exist - make it
                  fprintf(ferr,"Component [%s] [%s] placed on BOTTOM, but not mirrored\n",compname, shapename);
                  display_error++;
               }
*/
            }

            if (!strlen(shapename))
            {
               fprintf(ferr, "Component [%s] has no SHAPE defined!\n", compname); 
               display_error++;
            }
            else
            {
               DataStruct *d = Graph_Block_Reference(shapename, compname, cur_filenum, x, y,DegToRad(rot),
                 mirror, 1.0, lay, FALSE);
               d->getInsert()->setInsertType(insertTypePcbComponent);
   
               // in component the layer and the mirror flag is explicit.  If layer is bottom, simply means to put it in bottom.
               // mirro flag also simply means to turn on the flag.
               if (complayerbottom)
                  d->getInsert()->setPlacedBottom(complayerbottom);
               if (mirror)  // Graphic mirror               
                  d->getInsert()->setMirrorFlags(MIRROR_FLIP);             
               if (mirrorlayer) // layer mirror 
                  if (d->getInsert()->getMirrorFlags() == MIRROR_FLIP)
                     d->getInsert()->setMirrorFlags(MIRROR_ALL);
                  else
                     d->getInsert()->setMirrorFlags(MIRROR_LAYERS);

               // add refdes
               if (strlen(compname))
               {
                  // done in manufacturing refdes
               }

               // add device
               if (strlen(partname))
               {
                  TypeStruct *t = AddType(file, partname);

                  if (t->getBlockNumber() > -1 && (t->getBlockNumber() != d->getInsert()->getBlockNumber()))
                  {
                     // problem - a device has different shapes
                     fprintf(ferr,"Device [%s] on Component [%s] already has Shape [%s] assigned !\n", 
                        t->getName(), compname, shapename);
                     display_error++;
            
                     partname += "_";
                     partname += shapename;
                     t = AddType(file, partname);
                     c->modified_devicename = partname;
                  }

                  t->setBlockNumber( d->getInsert()->getBlockNumber());
   
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TYPELISTLINK, TRUE),
                     VT_STRING,
                     partname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
               }

               for (int i=0;i<attcnt;i++)
               {
                  doc->SetUnknownAttrib(&d->getAttributesRef(),att[i].key, att[i].val,
                     SA_APPEND, NULL); // x, y, rot, height
               }
            } // no shape defined

            for (int i=0;i<attcnt;i++)
            {
               free(att[i].key);
               free(att[i].val);
            }

            startcomp = TRUE;          
            mirror = FALSE;
            mirrorlayer = FALSE;
         }
         free(att);
         return 1;                         
      }
      else
      if (!STRCMPI(lp,"COMPONENT"))
      {
         if (startcomp)
         {

            GENCADComp *c = new GENCADComp;
            comparray.SetAtGrow(compcnt,c);  
            compcnt++;  
            c->compname = compname;
            c->devicename = partname;
            c->modified_devicename = "";

            // here check layer bottom without flip
            if (complayerbottom && !mirror)
            {
               fprintf(ferr,"Component [%s] [%s] placed on BOTTOM, but not mirrored\n",compname, shapename);
               display_error++;
            }

            if (strlen(shapename) == 0)
            {
               fprintf(ferr, "Component [%s] has no SHAPE defined!\n", compname); 
               display_error++;
            }
            else
            {
               DataStruct *d = Graph_Block_Reference(shapename, compname, cur_filenum, x, y,DegToRad(rot),
                 mirror, 1.0, lay, FALSE);
               d->getInsert()->setInsertType(insertTypePcbComponent);

               // in component the layer and the mirror flag is explicit.  If layer is bottom, simply means to put it in bottom.
               // mirro flag also simply means to turn on the flag.

               if (complayerbottom)
                  d->getInsert()->setPlacedBottom(complayerbottom);
               if (mirror)  // Graphic mirror               
                  d->getInsert()->setMirrorFlags(MIRROR_FLIP);             
               if (mirrorlayer) // layer mirror 
                  if (d->getInsert()->getMirrorFlags() == MIRROR_FLIP)
                     d->getInsert()->setMirrorFlags(MIRROR_ALL);
                  else
                     d->getInsert()->setMirrorFlags(MIRROR_LAYERS);
               // add refdes
               if (strlen(compname))
               {
                  // done in manufacturing refdes
               }
               // add device
               if (strlen(partname))
               {
                  TypeStruct *t = AddType(file, partname);
               
                  if (t->getBlockNumber() > -1 && (t->getBlockNumber() != d->getInsert()->getBlockNumber()))
                  {
                     // problem - a device has different shapes
                     fprintf(ferr,"Device [%s] on Component [%s] already has Shape [%s] assigned !\n", 
                        t->getName(), compname, shapename);
                     display_error++;
            
                     partname += "_";
                     partname += shapename;
                     t = AddType(file, partname);
                     c->modified_devicename = partname;
                  }

                  t->setBlockNumber( d->getInsert()->getBlockNumber());
   
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_TYPELISTLINK, TRUE),
                     VT_STRING, partname.GetBuffer(0), SA_OVERWRITE, NULL); //  
               }

               for (int i=0;i<attcnt;i++)
               {
                  doc->SetUnknownAttrib(&d->getAttributesRef(), att[i].key, att[i].val, SA_OVERWRITE, NULL); //  
               }

               char     tmp[255];
               strcpy(tmp, compname);
               //Since defining component executes the last after reading TEXT definition, I need to check if the text
               //description is really for this particular component
               //If TEXT definition is not defined, then don't show the refname on the graph and make the visibility to false
               // (drawings on the graph is done on do_text() function)

               if (textrefname == "")  
                  doc->SetVisAttrib(&d->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, tmp, 0.0 ,0.0, 
                     DegToRad(0), 0.0, 0.0, 1, 0, FALSE, SA_OVERWRITE, 0L, textlay, 0, 0, 0);
               else
               {
                  if (!compname.CompareNoCase(textrefname))
                  {
                     doc->SetVisAttrib(&d->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, tmp, textx ,texty, 
                        DegToRad(textrot), textheight, textcharwidth, 1, 0, TRUE, SA_OVERWRITE, 0L, textlay, textmir, 0, 0);
                     textrefname = "";
                  }
               }
         
            }  // no shape defined

            for (int i=0;i<attcnt;i++)
            {
               free(att[i].key);
               free(att[i].val);
            }

            attcnt = 0;
            startcomp = TRUE;          
            complayerbottom = FALSE;
            mirror = FALSE;
            mirrorlayer = FALSE;
            rot = 0;

            //doc->SetVisAttrib(&d->getAttributesRef(), doc->IsKeyWord(ATT_REFNAME, TRUE), VT_STRING, compname, textx ,texty, 
            // DegToRad(textrot), textheight, textcharwidth, 1, 0, TRUE, SA_OVERWRITE, 0L, textlay, FALSE);
         }

         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         compname = lp;
         startcomp = TRUE;
         partname = "";
         shapename = "";
      }
      else
      if (!STRCMPI(lp,"DEVICE"))
      {
         if ((lp = get_string(NULL,"\n")) == NULL) continue;   // device can include whitespace
         partname = lp;
         partname.TrimLeft();
         partname.TrimRight();
      }
      else
      if (!STRCMPI(lp,"PLACE"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         y = cnv_tok(lp);
      }
      else
      if (!STRCMPI(lp,"LAYER"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         complayerbottom = !STRCMPI(lp,"BOTTOM");
      }
      else
      if (!STRCMPI(lp,"ROTATION"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         rot += atof(lp);  // also mirrorx will change rot
      }
      else
      if (!STRCMPI(lp,"SHAPE"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         shapename = lp;
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue; // mirrorx or mirrory
         if (!STRCMPI(lp,"MIRRORY"))
            mirror = TRUE;
         else
         if (!STRCMPI(lp,"MIRRORX"))
         {
            rot += 180;
            mirror = TRUE;
         }
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue; // flip
         if (!STRCMPI(lp,"FLIP"))
            mirrorlayer = TRUE;
      }
      else
      if (!STRCMPI(lp,"TEXT"))
      { 
			if (!compname.CompareNoCase("r403"))
				int x = 0;
         do_text(&textrefname, &textx, &texty, &textheight, &textcharwidth, &textrot, &textmir, &textlay, TRUE);
      }
      else
      if (!STRCMPI(lp,"SHEET"))
      {


      }
      else
      if (!STRCMPI(lp,"ATTRIBUTE"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            char  key[80],val[255];

            if(!get_attval(key, val))  continue;
            att[attcnt].key = STRDUP(key);
            att[attcnt].val = STRDUP(val);
            attcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"VALUE"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            att[attcnt].key = STRDUP(ATT_VALUE);
            if ((lp = get_string(NULL," \t\r\n")) == NULL) continue;
            att[attcnt].val = STRDUP(lp);
            attcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"TOL"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            att[attcnt].key = STRDUP(ATT_TOLERANCE);
            if ((lp = get_string(NULL," \t\r\n")) == NULL) continue;
            att[attcnt].val = STRDUP(lp);
            attcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"NTOL"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            att[attcnt].key = STRDUP(ATT_MINUSTOLERANCE);
            if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
            att[attcnt].val = STRDUP(lp);
            attcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"PTOL"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            att[attcnt].key = STRDUP(ATT_PLUSTOLERANCE);
            if ((lp = get_string(NULL," \t\r\n")) == NULL) continue;
            att[attcnt].val = STRDUP(lp);
            attcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"DESC"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            att[attcnt].key = STRDUP("DESC");
            if ((lp = get_string(NULL," \t\r\n")) == NULL) continue;
            att[attcnt].val = STRDUP(lp);
            attcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"PACKAGE"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            att[attcnt].key = STRDUP("PACKAGE");
            if ((lp = get_string(NULL," \t\r\n")) == NULL) continue;
            att[attcnt].val = STRDUP(lp);
            attcnt++;
         }
      }
      else
      if (!STRCMPI(lp,"TYPE"))
      {
         if (attcnt < GCAD_MAX_ATT)
         {
            att[attcnt].key = STRDUP("TYPE");
            if ((lp = get_string(NULL," \t\r\n")) == NULL) continue;
            att[attcnt].val = STRDUP(lp);
            attcnt++;
         }
      }
      else
      {
         fprintf(ferr,"Error in $COMPONENTS : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
   }

   // error could not find logical end of section
   return -1;
}

/******************************************************************************
* gcad_signals
   just skip it.
*/
static int gcad_signals()
{
   CString  cur_netname;
   CString  cur_compname;
   CString  cur_pinname;
   NetStruct   *n = NULL;
   char     *lp;

   while (get_nextline(ifp_line, GCAD_MAX_LINE, ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp, "$END", 4))
      {
         return 1;
      }
      else if (!STRCMPI(lp, "SIGNAL"))
      {
			// Get the netname using backslash as delimiter only if it precedes a quote
         if ((lp = get_string(NULL," \r\t\n", "\"")) == NULL) continue;
         cur_netname = lp;

         n = add_net(file, cur_netname);
      }
      else if (!STRCMPI(lp, "NODE"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         cur_compname = lp;
         if ((lp = get_string(NULL," \r\t\n")) == NULL) continue;
         cur_pinname = lp;

         GENCADNetpin *c = new GENCADNetpin;
         netpinarray.SetAtGrow(netpincnt,c);  
         netpincnt++;   
         c->netname = cur_netname;
         c->compname = cur_compname;
         c->pinname = cur_pinname;
			c->netpintype = netpinComponent;
      }
      else if (!STRCMPI(lp, "ATTRIBUTE"))
      {
         char key[80],val[255];
         get_attval(key, val);
      }
      else
      {
         fprintf(ferr,"Error in $SIGNALS : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
   }

   return -1;
}

/******************************************************************************
* gcad_artworks
*/
static int gcad_artworks()
{
   char *lp;
   int artworkopen = 0;
   CString cur_type = "";
   DataStruct *data = NULL;
   int lay = 0;
   int widthindex = 0;

   routcont = FALSE;

   while (get_nextline(ifp_line, GCAD_MAX_LINE, ifp))
   {
      if ((lp = get_string(ifp_line, " \r\t\n")) == NULL)
         continue;

      if (!STRNICMP(lp, "$END", 4))
      {
         if (artworkopen)
         {
            Graph_Block_Off();
            artworkopen = FALSE;
         }
         return 1;
      }
      else if (!STRCMPI(lp, "ARTWORK"))
      {
         if (artworkopen)
         {
            Graph_Block_Off();
            artworkopen = FALSE;
         }

         if ((lp = get_string(NULL, " \r\t\n"))  == NULL)
            continue;
         CString aname( ArtworkName(lp) );
         Graph_Block_On(GBO_APPEND, aname, cur_filenum, 0);

         artworkopen = TRUE;
         cur_type = "";
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "TYPE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         cur_type = lp;
      }
      else if (!STRCMPI(lp, "TRACK"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL) 
            continue;
         CString tname;
         tname.Format("TRACK_%s", lp);

         int err;
         widthindex = Graph_Aperture(tname, T_ROUND, 0.0, 0.0,0.0, 0.0, 0.0,0,BL_WIDTH,FALSE, &err);
      }
      else if (!STRCMPI(lp, "LINE"))
      {
         data = do_line(lay, widthindex);
         if (data && strlen(cur_type))
            doc->SetUnknownAttrib(&data->getAttributesRef(), cur_type, "", SA_APPEND, NULL);
      }
      else if (!STRCMPI(lp, "ARC"))
      {
         do_arc(lay, widthindex, 0);
         data = NULL;
      }
      else if (!STRCMPI(lp, "CIRCLE"))
      {
         routcont = FALSE;
         do_circle(lay,widthindex, 0, 0);
         data = NULL;
      }
      else if (!STRCMPI(lp, "RECTANGLE"))
      {
         routcont = FALSE;
         do_rectangle(lay, widthindex, 0,0);     
         data = NULL;
      }
      else if (!STRCMPI(lp, "TEXT"))
      {
         routcont = FALSE;
         CString temp ="";
         double dbtemp;
         int inttemp;
         do_text(&temp, &dbtemp, &dbtemp, &dbtemp, &dbtemp, &dbtemp, &inttemp, &inttemp, 0);
         data = NULL;
      }
      else if (!STRCMPI(lp, "ATTRIBUTE"))
      {
         char key[80], val[255];
         if (!get_attval(key, val))
            continue;

         if (data)
            doc->SetUnknownAttrib(&data->getAttributesRef(), key, val, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, "CUTOUT"))
      {
         // do not do it.
         data = NULL;
         routcont = FALSE;
      }
      else if (!STRCMPI(lp, "MASK"))
      {
         routcont = FALSE;
         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         set_usedlayer(lp);
         lay = Graph_Level(lp, "", 0);
         data = NULL;
      }
      else if (!STRCMPI(lp, "LAYER"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         set_usedlayer(lp);
         lay = Graph_Level(lp, "", 0);

         // if not a standard gencad layer, make a mirror layer.
         if (!standard_gencadlayer(lp))
         {
            CString  ll;
            ll.Format("M_%s", lp);
            Graph_Level(ll, "", 0);
            Graph_Level_Mirror(lp, ll, "");
         }

         data = NULL;
         routcont = FALSE;
      }
      else
      {
         fprintf(ferr, "Error in $ARTWORK : command [%s] unknown at %ld\n", lp, ifp_linecnt);
         display_error++;
         data = NULL;
      }
   }

   return -1;
}

/******************************************************************************
* gcad_devices
*/
static int gcad_devices()
{
   char *lp;
   CString devicename = "";
   TypeStruct *deviceType = NULL; 

   while (get_nextline(ifp_line, GCAD_MAX_LINE, ifp))
   {
      if ((lp = get_string(ifp_line, " \r\t\n")) == NULL)
         continue;
      lp = _strupr(lp);

      if (!STRNICMP(lp, "$END", 4))
      {
         return 1;
      }
      else if (!STRCMPI(lp, "DEVICE"))
      {
         // Reset it to NULL at every new DEVICE
         deviceType = NULL;

         // can have spaces
         if ((lp = get_string(NULL, "\n")) == NULL)
         {
            fprintf(ferr, "Error in $DEVICE : Improper device given at line %d\n", ifp_linecnt);
            display_error++;
            continue;
         }
         CString devicename = lp;
         devicename.TrimLeft();
         devicename.TrimRight();

         char tmp[100];
         strcpy(tmp, devicename);
         if (tmp[strlen(tmp)-1] == '"')
            tmp[strlen(tmp)-1] = '\0';
         STRREV(tmp);

         if (tmp[strlen(tmp)-1] == '"')
            tmp[strlen(tmp)-1] = '\0';
         STRREV(tmp);
         devicename = tmp;

         // Add the device
         deviceType = AddType(file, devicename);
      }
      else if (!STRCMPI(lp, "GenCAD"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         int keyIndex = doc->RegisterKeyWord("CAMCAD", 0, VT_STRING);
         doc->SetAttrib(&deviceType->getAttributesRef(), keyIndex, VT_STRING, (char *)lp, SA_APPEND, NULL);
      }
      else if (!STRCMPI(lp, "PART"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         int keyIndex = doc->RegisterKeyWord(ATT_PARTNUMBER, 0, VT_STRING);
         doc->SetAttrib(&deviceType->getAttributesRef(), keyIndex, VT_STRING, (char *)lp, SA_APPEND, NULL);
      }
      else if (!STRCMPI(lp, "TYPE"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         int keyIndex = doc->RegisterKeyWord("TYPE", 0, VT_STRING);
         doc->SetAttrib(&deviceType->getAttributesRef(), keyIndex, VT_STRING, (char *)lp, SA_APPEND, NULL);
      }
      else if (!STRCMPI(lp, "PACKAGE"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         int keyIndex = doc->RegisterKeyWord("PACKAGE", 0, VT_STRING);
         doc->SetAttrib(&deviceType->getAttributesRef(), keyIndex, VT_STRING, (char *)lp, SA_OVERWRITE, NULL);
      }
      else if (!STRCMPI(lp, "STYLE"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         int keyIndex = doc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);
         doc->SetAttrib(&deviceType->getAttributesRef(), keyIndex, VT_STRING, (char *)lp, SA_OVERWRITE, NULL); 
      }
      else if (!STRCMPI(lp, "VALUE"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL || !deviceType)
            continue;
         doc->SetAttrib(&deviceType->getAttributesRef(), doc->IsKeyWord(ATT_VALUE, 0), VT_STRING, (char *)lp, SA_OVERWRITE, NULL); 
      }
      else if (!STRCMPI(lp, "PINCOUNT") || !STRCMPI(lp, "PINDESC") || !STRCMPI(lp, "PINFUNCT"))
      {
         // is not yet implemented.
      }
      else if (!STRCMPI(lp, "TOL"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         double value = atof(lp);
         doc->SetAttrib(&deviceType->getAttributesRef(), doc->IsKeyWord(ATT_TOLERANCE, 0), VT_DOUBLE, &value, SA_OVERWRITE, NULL);   
      }
      else if (!STRCMPI(lp, "PTOL"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         double value = atof(lp);
         doc->SetAttrib(&deviceType->getAttributesRef(), doc->IsKeyWord(ATT_PLUSTOLERANCE, 0), VT_DOUBLE, &value, SA_APPEND, NULL); 
      }
      else if (!STRCMPI(lp, "NTOL"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         double value = atof(lp);
         doc->SetAttrib(&deviceType->getAttributesRef(), doc->IsKeyWord(ATT_MINUSTOLERANCE, 0), VT_DOUBLE, &value, SA_APPEND, NULL);
      }
      else if (!STRCMPI(lp, "VOLTS"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL || !deviceType)
            continue;
         double value = atoi(lp);
         doc->SetAttrib(&deviceType->getAttributesRef(), doc->IsKeyWord(ATT_VOLTAGE, 0), VT_DOUBLE, &value, SA_APPEND, NULL);
      }
      else if (!STRCMPI(lp, "DESC"))
      {
         if ((lp = get_string(NULL, "\n")) == NULL || !deviceType)
            continue;
         int keyIndex = doc->RegisterKeyWord("DESCRIPTION", 0, VT_STRING);
         doc->SetAttrib(&deviceType->getAttributesRef(), keyIndex, VT_STRING, (char *)lp, SA_APPEND, NULL);
      }
      else if (!STRCMPI(lp, "ATTRIBUTE"))
      {
         char key[80], val[255];
         if (!get_attval(key, val) || !deviceType)
            continue;
         doc->SetUnknownAttrib(&deviceType->getAttributesRef(), key, val, SA_APPEND, NULL);
      }
      else
      {
         fprintf(ferr, "Error in $DEVICE : command [%s] unknown at %ld\n", lp, ifp_linecnt);
         display_error++;
      }     
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int gcad_layers()
{
   char     *lp;

   Graph_Level_Mirror("SILKSCREEN_TOP", "SILKSCREEN_BOTTOM", "");
   Graph_Level_Mirror("TOP", "BOTTOM", "");

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
      {
         return 1;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int gcad_mech()
{
   char     *lp;
   double   x = 0, y = 0, drill, rotation = 0;
   CString  fiducialname, mechanical, shapename, attachedto;
   CString  layer;
   int      mirror = 0;

   fiducialname = "";
   mechanical = "";
   attachedto = "";

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
      {
         write_mechanical(mechanical, shapename, layer, x, y, rotation, mirror);
         mechanical = "";

         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         return 1;
      }
      else
      if (!STRCMPI(lp,"SHAPE"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         shapename = lp;
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue; // mirrorx or mirrory
         if (!STRCMPI(lp,"MIRRORY"))
            mirror = TRUE;
         else
         if (!STRCMPI(lp,"MIRRORX"))
         {
            rotation += 180;
            mirror = TRUE;
         }
         else
            mirror = FALSE;

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue; // flip
      }
      else
      if (!STRCMPI(lp,"FHOLE"))
      {
         write_mechanical(mechanical, shapename, layer, x, y, rotation, mirror);
         mechanical = "";
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         y = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         drill = cnv_tok(lp);

         CString  bname;
         bname.Format("FHOLE_%s",lp);

         if (Graph_Block_Exists(doc, bname, -1) == NULL)
         {
            CString  drillname;
            int layernum = Graph_Level("DRILLHOLE","",0);
            drillname.Format("DRILL_%d",get_drillindex(drill, layernum));
            Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
            BlockStruct *b = Graph_Block_On(GBO_APPEND, bname,-1,0);
            Graph_Block_Reference( drillname, NULL, 0, 0, 0, 0.0, 0 , 1.0, layernum, TRUE);
            Graph_Circle(layernum, 0.0 , 0.0, drill/2, 0L, 0 , FALSE, 0); 
            Graph_Block_Off();
            b->setBlockType(BLOCKTYPE_TOOLING);
         }

         DataStruct *d = Graph_Block_Reference(bname, NULL, 0, x, y, 0.0, 0 , 1.0, -1, TRUE);
         d->getInsert()->setInsertType(insertTypeDrillTool);

      }
      else
      if (!STRCMPI(lp,"HOLE"))
      {
         write_mechanical(mechanical, shapename, layer, x, y, rotation, mirror);
         mechanical = "";
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         y = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         drill = cnv_tok(lp);

         CString  bname;
         bname.Format("HOLE_%s",lp);

         if (Graph_Block_Exists(doc, bname, -1) == NULL)
         {
            CString  drillname;
            int layernum = Graph_Level("DRILLHOLE","",0);
            drillname.Format("DRILL_%d",get_drillindex(drill, layernum));
            Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
            BlockStruct *b = Graph_Block_On(GBO_APPEND, bname,-1,0);
            Graph_Block_Reference( drillname, NULL, 0, 0, 0, 0.0, 0 , 1.0, layernum, TRUE);
            Graph_Circle(layernum, 0.0 , 0.0, drill/2, 0L, 0 , FALSE, 0); 
            Graph_Block_Off();
            b->setBlockType(BLOCKTYPE_DRILLHOLE);
         }

         DataStruct *d = Graph_Block_Reference(bname, NULL, 0, x, y, 0.0, 0 , 1.0, -1, TRUE);
         d->getInsert()->setInsertType(insertTypeDrillHole);

      }
      else
      if (!STRCMPI(lp,"MECHANICAL"))
      {
         write_mechanical(mechanical, shapename, layer, x, y, rotation, mirror);
         mechanical = "";
         attachedto = "";
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";
         // if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
         // this is followed by place and layer
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // partname
         mechanical = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // attached to 
         attachedto = lp;
      }
      else
      if (!STRCMPI(lp,"FIDUCIAL"))
      {
         write_mechanical(mechanical, shapename, layer, x, y, rotation, mirror);
         mechanical = "";
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";
         // if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
         // this is followed by place and layer
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // fid_name
         fiducialname = lp;
      }
      else
      if (!STRCMPI(lp,"PLACE"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         y = cnv_tok(lp);
      }
      else
      if (!STRCMPI(lp,"ROTATION"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         rotation = atof(lp);  // also mirrorx will change rot
      }
      else
      if (!STRCMPI(lp,"LAYER"))
      {
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         layer = lp;
      }
      else
      if (!STRCMPI(lp,"FID"))
      {
         double rot;
         int    mir, lay, flip;
         CString fidname, padname, origpadname, layername;

         write_mechanical(mechanical, shapename, layer, x, y, rotation, mirror);
         mechanical = "";
         write_oldfiducial(fiducialname, layer, x, y);
         fiducialname = "";

         //FID partfid_0 SM184QFP256L_F_fid_0 -6555 -6555 TOP 0 0
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // fid_name
         fidname = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // pad_name
         origpadname = lp;
         padname.Format("%s%s",padstackprefix, lp);

         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // x
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // y
         y = cnv_tok(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // layer
         layername = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // rot
         rot = atof(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // mirror
         mir = atoi(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // flip
         flip = atoi(lp);

         // if no padname is there, that means that a $PAD is used and not a $padstack
         // if $pad is used
         if (get_padshape(origpadname) > -1)
         {
            // here check if not defined yet

            // if a padshape is BOTTOM, this means, use this for BOTTOM placement.
            // however, on bottom placement, the mirror layer is used.
            if (!strcmp(layername,"BOTTOM"))
               layername = "TOP";

            // padname is the newly created padstack. This name must be different from the $PADS name
            //padname.Format("%s_%s",origpadname,layername);
            //padname = origpadname;

            if (Graph_Block_Exists(doc, padname, -1))
            {
               // do nothing
            }
            else
            {
               set_usedlayer(layername);
               lay = Graph_Level(layername,"", 0);

               CString  pname;
               // here make one.
               BlockStruct *b = Graph_Block_On(GBO_APPEND,padname,cur_filenum,0);
               b->setBlockType(BLOCKTYPE_PADSTACK);
               pname.Format("%s%s",padprefix, origpadname);
               Graph_Block_Reference(pname, NULL, cur_filenum, 0.0, 0.0,0.0, 0, 1.0, lay, TRUE);
               Graph_Block_Off();
            }
         }
         DataStruct *d = Graph_Block_Reference(padname, fidname, cur_filenum, x, y,DegToRad(rot), mir, 1.0, -1, TRUE);
         d->getInsert()->setInsertType(insertTypeFiducial);
         

         //This section of codes is added based upon the specification of GENCAD reader v1.4 by Lynn 03/19/02
         d->getInsert()->setMirrorFlags(0);        
         if (!layername.CompareNoCase("BOTTOM"))
         {
            //This is added because if a fiducial is defined as BOTTOM 
            //set PLACE_BOTTOM = 1 and set MIRROR_LAYERS
            d->getInsert()->setPlacedBottom(true);
            d->getInsert()->setMirrorFlagBits(MIRROR_LAYERS);
         }
         if (mir > 0)
            //If fiducial is defined to be mirror then set MIRROR_FLIP
            d->getInsert()->setMirrorFlagBits(MIRROR_FLIP);
         if (flip > 0)
         {
            //If fiducial is defined to be flip 
            if (d->getInsert()->getMirrorFlags() & MIRROR_LAYERS)
               //If MIRROR_LAYERS is already set, then remove it to flip back
               d->getInsert()->clearMirrorFlagBits(MIRROR_LAYERS);
            else
               //If MIRROR_LAYERS is not set, then set to flip
               d->getInsert()->setMirrorFlagBits(MIRROR_LAYERS);
         }

      }
      else
      {
         fprintf(ferr,"Error in $MECH : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int gcad_pseudos()
{
   char     *lp;

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
     if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

     if (!STRNICMP(lp,"$END",4))
     {
      return 1;
     }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int gcad_changes()
{
   char     *lp;

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
     if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

     if (!STRNICMP(lp,"$END",4))
     {
      return 1;
     }
   }

   return -1;
}

/****************************************************************************/
/*
   This is a testprobe
   I need to place a probe and an access point and link it up correctly.

   In CAMCAD a feature links to an access point and an access point links to a probe.

*/
static int testpins_and_powerpins(CString keyword)
{
   char     *lp;
	DataStruct *d = NULL;

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
      {
         return 1;
      }

      if (!STRCMPI(lp,keyword))
      {
         CString  refname, netname, probegeom;
         double   x, y;
         int      mir = 0;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // 
         refname = lp;
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // x
         x = cnv_tok(lp);
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // y
         y = cnv_tok(lp);

			// Get the netname using backslash as delimiter only if it precedes a quote
         if ((lp = get_string(NULL," \t\n", "\"")) == NULL)  continue;  // 
         netname = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // tan
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // tin
         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // probeparam
         if (!STRCMPI(lp,"-1"))
            probegeom = "GENCADPROBE";
         else
            probegeom = lp;

         if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // layer
         if (!STRCMPI(lp,"BOTTOM"))
            mir = TRUE;

			//still need to read it even if it is unplaced
         /*if (!strcmp(netname,"-1"))      // probe not placed !!!
            continue;*/

         double probesize = 0.1 * Units_Factor(UNIT_INCHES, pageunits);
         double drillsize = 0.05 * Units_Factor(UNIT_INCHES, pageunits);
         if (!Graph_Block_Exists(doc, probegeom, -1))
         {
            generate_TestProbeGeometry(doc, probegeom, probesize, "T0", drillsize);
         }

         d = Graph_Block_Reference(probegeom, refname, 0, x,  y, 0.0, mir, 1.0, -1, TRUE);
         d->getInsert()->setInsertType(insertTypeTestProbe);

         CreateTestProbeRefnameAttr(doc, d, refname, drillsize, true /*nevermirror*/);
      
         // testpin name x y netname test_assign_number test_interface_number probename layer
         GENCADTestpin *c = new GENCADTestpin;
         testpinarray.SetAtGrow(testpincnt,c);  
         testpincnt++;  
         c->testprobe_data = d;
         c->x = x;
         c->y = y;
         c->bottom = mir;
         c->netname = netname;
/*
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_DDLINK, 1),
            VT_INTEGER,
            &(probeplacearray[i]->access_entitynum),   // this makes a "real" char *
            0.0,0.0,0.0, 0.0, 0.0,0,0,FALSE, SA_APPEND, 0L, 0,0); // x, y, rot, height
*/
         // do not make a netname for nonconn probes.
         if (strlen(netname))
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
               VT_STRING,
               netname.GetBuffer(0), SA_APPEND, NULL); // x, y, rot, height
         }

			CString testResVal = (keyword.CompareNoCase("POWERPIN") == 0) ? "Power Injection" : "Test";
         doc->SetUnknownAttrib(&d->getAttributesRef(), ATT_TESTRESOURCE, testResVal, SA_OVERWRITE, NULL);

      }
      else
      if (!STRCMPI(lp,"TEXT"))
      {
         CString temp ="";
         double dbtemp;
         int inttemp;
         do_text(&temp,&dbtemp,&dbtemp,&dbtemp,&dbtemp,&dbtemp,&inttemp,&inttemp, 0);
      }
      else
      if (!STRCMPI(lp,"ATTRIBUTE")) //case 1096, add the ability to read attributes
      {
			CString key = "";
			CString val = "";
			if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // 
			if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // keyword
			key = lp;
			if ((lp = get_string(NULL," \t\n")) == NULL)  continue;  // value
			val = lp;		

			int keyword = doc->RegisterKeyWord(key, 1, !key.CompareNoCase(ATT_DDLINK)?VT_INTEGER:VT_STRING);

			if (d)
			{
				doc->SetAttrib(&d->getAttributesRef(), keyword, !key.CompareNoCase(ATT_DDLINK)?VT_INTEGER:VT_STRING, 
					(void*)val.GetBuffer(0), SA_OVERWRITE, NULL);
				if (!key.CompareNoCase("PROBE_PLACEMENT") && !val.CompareNoCase("Unplaced"))
					d->setHidden(true);
			}
      }
   }

   return -1;
}

/******************************************************************************
* gcad_powerpins
*/
static int gcad_testpins()
{
	return testpins_and_powerpins("TESTPIN");
}

/******************************************************************************
* gcad_powerpins
*/
static int gcad_powerpins()
{
	return testpins_and_powerpins("POWERPIN");
}

/******************************************************************************
* gcad_boards
   This command only exist in Panel file.
*/
static int gcad_boards()
{
   char *lp;
   int done = TRUE;  // placed
   double x,y,rot;
   int mir = 0;
   int flip = 0;     // 0 no mirror, 1 xmirror, 2 ymirror
   CString fname = "";
   CString boardname = "";

   while (get_nextline(ifp_line , GCAD_MAX_LINE, ifp))
   {
      if ((lp = get_string(ifp_line, " \r\t\n")) == NULL)
         continue;

      if (!STRNICMP(lp, "$END", 4))
      {
         if (!done)
         {
            // place previous
            BlockStruct *block = doc->Find_Block_by_Name(fname, -1); 

            if (block)
            {
               int pcb_filenum = block->getFileNumber();
   
               DataStruct *data = Graph_Block_Reference(fname, boardname, pcb_filenum,x, y, DegToRad(rot), 0, 1.0, 0, FALSE);
               data->getInsert()->setInsertType(insertTypePcb);
               if (mir)
               {
                  data->getInsert()->setMirrorFlags(MIRROR_LAYERS);
                  data->getInsert()->setPlacedBottom(true);
               }
               if(flip)
                  data->getInsert()->setMirrorFlagBits(MIRROR_FLIP);
            }
            else
            {
               fprintf(ferr, " Called out BOARD FILE [%s] not loaded !\n", fname);
               display_error++;
            }
         }
         return 1;
      }
      else if (!STRCMPI(lp, "FILE"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         fname = lp;
         fname.MakeLower(); // needs to be lower because filenames are not case sensitive in DOS.
      }
      else if (!STRCMPI(lp, "PLACE"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         x = cnv_tok(lp);

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         y = cnv_tok(lp);
      }
      else if (!STRCMPI(lp, "ROTATION"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         rot = atof(lp);  // also mirrorx will change rot
      }
      else if (!STRCMPI(lp, "ELEMENT"))
      {
         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         CString m = lp;

         mir = 0;
         flip = 0;
         if (m.CompareNoCase("MIRRORX") == 0)
            mir = 1;
         else if (m.CompareNoCase("MIRRORY") == 0)
            mir = 2;

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         if (!STRCMPI(lp, "FLIP"))  flip = TRUE;
      }
      else if (!STRCMPI(lp, "BOARD"))
      {
         if (!done)
         {
            // place previous
            BlockStruct *block = doc->Find_Block_by_Name(fname, -1);
            if (block)
            {
               int pcb_filenum = block->getFileNumber();
   
               DataStruct *data = Graph_Block_Reference(fname, boardname, pcb_filenum, x, y, DegToRad(rot), 0, 1.0, 0, FALSE);
               data->getInsert()->setInsertType(insertTypePcb);

               if (mir)
               {
                  data->getInsert()->setMirrorFlags(MIRROR_LAYERS);
                  data->getInsert()->setPlacedBottom(true);
               }

               if(flip) 
                  data->getInsert()->setMirrorFlagBits(MIRROR_FLIP);
            }
            else
            {
               fprintf(ferr, " Called out BOARD FILE [%s] not loaded !\n", fname);
               display_error++;
            }

            done = TRUE;
         }

         if ((lp = get_string(NULL, " \r\t\n")) == NULL)
            continue;
         boardname = lp;
         mir = 0;
         flip = 0;
         done = FALSE;
      }
      else
      {
         fprintf(ferr, "Error in $PANEL : command [%s] unknown at %ld\n", lp, ifp_linecnt);
         display_error++;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int gcad_panel()
{
   char        *lp;
   int         lay = Graph_Level("PANEL","", 0);
   int         widthindex = 0;     // Small width
   DataStruct  *d;
   int         graph_class = GR_CLASS_PANELOUTLINE; 

   routcont = FALSE;

   while (get_nextline(ifp_line,GCAD_MAX_LINE,ifp))
   {
      if ((lp = get_string(ifp_line," \r\t\n")) == NULL) continue;

      if (!STRNICMP(lp,"$END",4))
         return 1;
   
      if (!STRCMPI(lp,"ARTWORK"))
      {
         routcont = FALSE;
         graph_class = 0;
         if ((lp = get_string(NULL,"\n")) == NULL)  continue;
         lay = Graph_Level(lp,"", 0);
      }
      else
      if (!STRCMPI(lp,"LINE"))
      {
         d = do_line(lay,widthindex);
         if (d)
            d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"ARC"))
      {
         routcont = FALSE;
         d = do_arc(lay,widthindex, 0);
         if (d)
            d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"CIRCLE"))
      {
         routcont = FALSE;
         d = do_circle(lay,widthindex,0,0);
         d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"RECTANGLE"))
      {
         routcont = FALSE;
         d = do_rectangle(lay, widthindex, 0,0);       
         d->setGraphicClass(graph_class);
      }
      else
      if (!STRCMPI(lp,"ATTRIBUTE"))
      {
         fprintf(ferr,"Error in $PANEL : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
      else
      if (!STRCMPI(lp,"CUTOUT"))
      {
         // do not do it.
         graph_class = graphicClassPanelCutout;
         lay = Graph_Level("PANEL","", 0);
         //d = NULL;
      }
      else
      if (!STRCMPI(lp,"TYPE"))
      {
         fprintf(ferr,"Error in $PANEL : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
      else
      if (!STRCMPI(lp,"THICKNESS"))
      {
         fprintf(ferr,"Error in $PANEL : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
      else
      if (!STRCMPI(lp,"MASK"))
      {
         routcont = FALSE;
         graph_class = 0;
         if ((lp = get_string(NULL," \r\t\n")) == NULL)  continue;
         lay = Graph_Level(lp,"", 0);
      }
      else
      {
         fprintf(ferr,"Error in $PANEL : command [%s] unknown at %ld\n",lp,ifp_linecnt);
         display_error++;
      }
   }

   // error could not find logical end of section

   return -1;
}

/****************************************************************************/
/*
*/
//static int get_layertype(const char *l)
//{
//   int   i;
//
//   for (i=0;i<MAX_LAYTYPE;i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return   LAYTYPE_UNKNOWN; 
//}

/****************************************************************************/
/*
*/
static int load_gencadsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   compoutlinecnt = 0;
   boardoutline = "";
   refname_height = 0.07;
   viadrill_oversize = 0.01;
   ComponentSMDrule = 0;

   padprefix = "PAD_";
   padstackprefix = "PADSTACK_";

   ForceText2Silk = FALSE;
   USE_PLANE_NAMES = TRUE;
	REFRESH_DEVICETOCOMP_ATTRIBUTES = FALSE;

   MakeInto_CommandProcessor.Reset();

   if ((fp = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "GENCAD Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, fp))
   {
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
            if (compoutlinecnt < 30)
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
						continue; 
               compoutline[compoutlinecnt] = lp;
               compoutlinecnt++;
            }
         }
         else if (!STRICMP(lp, ".BOARDOUTLINE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 
            boardoutline = lp;
         }
         else if (!STRICMP(lp,".REFNAME_HEIGHT"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 
            refname_height = atof(lp);
         }
         else if (!STRICMP(lp, ".VIADRILL_OVERSIZE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 
            viadrill_oversize = atof(lp);
         }
         else if (!STRICMP(lp, ".MIRRORSHAPE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 
            CString shape1 = lp;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 
            CString shape2 = lp;         
         
            GENCADMirrorShape *c = new GENCADMirrorShape;
            mirrorshapearray.SetAtGrow(mirrorshapecnt++, c);  
            c->shape1 = shape1;
            c->shape2 = shape2;
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 
            CString glayer = lp;
            glayer.TrimLeft();
            glayer.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue; 
            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);

            if (layer_attr_cnt < GCAD_MAX_LAYERS)
            {
               layer_attr[layer_attr_cnt].name = glayer;
               layer_attr[layer_attr_cnt].attr = laytype;
               layer_attr_cnt++;
            }
            else
            {
               fprintf(ferr, "Too many layer attributes\n");
               display_error++;
            }
         }
         else if (!STRCMPI(lp, ".PADPREFIX"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp)
               padprefix = lp;
            else
               padprefix = "";
         }
         else if (!STRCMPI(lp, ".PADSTACKPREFIX"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp)
               padstackprefix = lp;
            else
               padstackprefix = "";
         }
         else if (!STRCMPI(lp, ".FORCETEXT2SILK"))
         {
            lp = get_string(NULL, " \t\n");
				if (toupper(lp[0]) == 'Y')
               ForceText2Silk = TRUE;
         }
         else if (!STRCMPI(lp, ".USE_PLANE_NAMES"))
         {
            lp = get_string(NULL, " \t\n");
				if (toupper(lp[0]) == 'N')
               USE_PLANE_NAMES = FALSE;
         }        
			else if (!STRCMPI(lp, ".REFRESH_DEVICETOCOMP_ATTRIBUTES"))
			{
				lp = get_string(NULL, " \t\n");
				if (toupper(lp[0]) == 'Y')
					REFRESH_DEVICETOCOMP_ATTRIBUTES = TRUE;
			}
         else if (MakeInto_CommandProcessor.ParseCommand(line)) // send whole text line, not just command
         {
            // Already did what it needed to do
         }
      }
   }

   fclose(fp);
   return 1;
}

/*****************************************************************************/
/*
*/
static int get_lay_att(const char *l)
{
   int   i;

   for (i=0;i<layer_attr_cnt;i++)
   {
      if (!STRCMPI(l,layer_attr[i].name))
      {
         return layer_attr[i].attr;
      }
   }
   return LAYTYPE_UNKNOWN;
}

/*****************************************************************************/
/*
*/
static CompPinStruct *find_comppin(const char *netname, double x, double y, int bottom)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   double   onemil = Units_Factor(UNIT_MILS, pageunits);
   
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      
      if (!net->getNetName().Compare(netname))
      {
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            compPin = net->getNextCompPin(compPinPos);
            if (compPin->getPinCoordinatesComplete())
            {
               if (fabs(compPin->getOriginX() - x) < onemil && fabs(compPin->getOriginY() - y) < onemil)
               {
                  // here also check layer.
                  return compPin;
               }
            }
         }
      }
   }

   return NULL;
}

/*****************************************************************************/
/*
*/
static int find_feature(double x, double y, int bottom)
{
   int      i;
   double   onemil = Units_Factor(UNIT_MILS, pageunits);

   for (i=0;i<featurecnt;i++)
   {
      if (fabs(featurearray[i]->x - x) < onemil && fabs(featurearray[i]->y - y) < onemil)
      {
         // here also check layer.
         return i;
      }
   }

   return -1;
}

/*****************************************************************************/
/*
   need to make access points and assign datalink to probe
   need to assign datalink to accesspoint from features and netlist pins
   need to set a test attribute on the features and netlist pins.
*/
static int assign_testprobes()
{
   int   i;
   // testpincnt;
   for (i=0;i<testpincnt;i++)
   {
#ifdef _DEBUG
   GENCADTestpin *c = testpinarray[i];
#endif
      // a testpin can be either a feature or a comppin ?
      int ptr = find_feature(testpinarray[i]->x, testpinarray[i]->y, testpinarray[i]->bottom);
      if (ptr < 0)
      {
         // find in netlist
         CompPinStruct *cp = find_comppin(testpinarray[i]->netname, 
                                          testpinarray[i]->x, testpinarray[i]->y, testpinarray[i]->bottom);
         if (cp)
         {
            int t = ATT_TEST_ACCESS_TOP;
            if (testpinarray[i]->bottom)  
               t = ATT_TEST_ACCESS_BOT;

            doc->SetUnknownAttrib(&cp->getAttributesRef(), 
               ATT_TEST, testaccesslayers[t], SA_OVERWRITE, NULL); // x, y, rot, height
         }
         else
         {
            // no match found !!!
            fprintf(ferr,"No TESTPIN match found!\n");
            display_error++;
         }
      }
      else
      {
         // found in features
#ifdef _DEBUG
   GENCADFeature *ff = featurearray[ptr];
#endif
         // set an attribute to the feature
         int t = ATT_TEST_ACCESS_TOP;
         if (testpinarray[i]->bottom)  
            t = ATT_TEST_ACCESS_BOT;

         doc->SetUnknownAttrib(&(featurearray[ptr]->feature_data)->getAttributesRef(), 
               ATT_TEST, testaccesslayers[t], SA_OVERWRITE, NULL); //  

         // need to set a datalink to the feature.

      }
   }

   return 1;
}

/******************************************************************************
* assign_layers
*/
static int assign_layers()
{
   Graph_Level_Mirror("SOLDERMASK_TOP", "SOLDERMASK_BOTTOM", "");
   Graph_Level_Mirror("SOLDERPASTE_TOP", "SOLDERPASTE_BOTTOM", "");
   Graph_Level_Mirror("SILKSCREEN_TOP", "SILKSCREEN_BOTTOM", "");

   int layerNum = Graph_Level("REFNAME_TOP","", 0);
   LayerStruct *layer = doc->FindLayer(layerNum);
   layer->setLayerType(LAYTYPE_TOP);

   layerNum = Graph_Level("REFNAME_BOTTOM","", 0);
   layer = doc->FindLayer(layerNum);
   layer->setLayerType(LAYTYPE_BOTTOM);

   Graph_Level_Mirror("TOP", "BOTTOM", "");

   set_usedlayer("TOP");
   set_usedlayer("BOTTOM");

   int max_innerlayers = 0;
   for (int j=0; j<doc->getMaxLayerIndex(); j++)
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)
         continue; // could have been deleted.

      int lt = get_lay_att(layer->getName());
      if (lt > 0)
         layer->setLayerType(lt);

      if (layer->getName().CompareNoCase("TOP") == 0)
      {
         layer->setElectricalStackNumber(1);
      }
      else if (!STRNICMP(layer->getName(), "INNER", 5))
      {
         int lnr;
         if (strlen(layer->getName()) > 5 && (lnr = atoi(layer->getName().Right(strlen(layer->getName())-5))))
         {
            if (lnr > max_innerlayers)
               max_innerlayers = lnr;
            layer->setElectricalStackNumber(lnr + 1);   // inner1 is 2nd layer
         }
      }
   }

   layerNum =  Graph_Level("BOTTOM","", 0);
   layer = doc->FindLayer(layerNum);
   layer->setElectricalStackNumber(max_innerlayers+2);

   return 1;
}

/******************************************************************************
* tst_gencadfiletype
   This function is only use by port_lib.cpp to find out the gencadtype
*/
int tst_gencadfiletype(FILE *ifp, const char *f)
{
   // GENCAD_UNKNOWN          
   // GENCAD_CAD
   // GENCAD_PANEL         
         
   int ftype = GENCAD_UNKNOWN;
   char iline[GCAD_MAX_LINE];

   while (get_nextline(iline, GCAD_MAX_LINE, ifp) != NULL)
   {
      CString tmp = iline;

      if (!strncmp(iline, "GENPANEL", strlen("GENPANEL")))
      {
         rewind(ifp);
         return GENCAD_PANEL;
      }

      if (!strncmp(iline, "GENCAD", strlen("GENCAD")))
      {
         rewind(ifp);
         return GENCAD_CAD;
      }
   }
   return ftype;
}