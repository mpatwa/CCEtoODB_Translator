// $Header: /CAMCAD/5.0/read_wrt/Ipc_in.cpp 31    5/25/07 6:53p Lynn Phung $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           
 
#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include "attrib.h"
#include "lic.h"
#include "lyrmanip.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// For debugging, write unrecognized, unprocessed, skipped input lines to log
#define WRITE_DEBUG_TO_LOG

/****************************************************************************/
/*
*/
#define  T_ENDCAP       T_SQUARE

#define  MAX_POLY       4096     // I have seen over 255 vertex cnts.
#define  MAX_TESTBLOCK  1000
#define  MAX_COMPPIN    4000
#define  MAX_LAYERS     30

#define  U_ENGLISH      0
#define  U_METRIC       1

#define  MINSIZE        5        // in 1 mil units

#define  IPCERR         "ipc.log"

/****************************************************************************/
/*
*/

typedef struct
{
   double   x,y;
   int      mode;    // 7 = line
                     // 8 = circle i = center i+1.x = start i+1.y = end
                     //            i+2.x = radius, i+2.y = 1(counterclock) -1 (clock)
   int      lineend; // 0 = round
                     // 1 = square
   int      fill;    // 0 not
                     // 1 yes
} IPC_Poly;

typedef struct
{
   char  *name;
} IPC_TestBlock;

typedef struct
{
   CString  compname;
}IPC_CompInst;
typedef CTypedPtrArray<CPtrArray, IPC_CompInst*> CompInstArray;

typedef struct
{
   CString  shortnet;
   CString  longnet;
}IPC_LongNet;
typedef CTypedPtrArray<CPtrArray, IPC_LongNet*> LongNetArray;

typedef struct
{
   char     form;
   double   sizeA;
   double   sizeB;
   double   offsetx, offsety;
   double   rotation;
}IPC_Padform;
typedef CArray<IPC_Padform, IPC_Padform&> CPadformArray;

typedef struct
{
   double d;
   int    toolindex;
}IPC_Drill;
typedef CArray<IPC_Drill, IPC_Drill&> CDrillArray;

typedef struct
{
   CString  compname;
   CString  pinname;
   double   x,y,rot;
   int      layerindex; // Graph_Level
   int      lnr;        // A00 .. A01 ..
   int      padstack_blocknum;
   int      psindex;    // index into IPC_Paddef
}IPC_Pin;
typedef CTypedPtrArray<CPtrArray, IPC_Pin*> CPinArray;

typedef struct
{
   int   toolindex;
   int   padstack[MAX_LAYERS];
   int   typ;           // 1 top, 3 inner, 2 bottom 4 drill
   int   mirror_used;   // to place mirrored components, sometimes a mirrored version of this
                        // padstack has to be created. index+paddefcnt
}IPC_Paddef;
typedef CArray<IPC_Paddef, IPC_Paddef&> CPaddefArray;

/****************************************************************************/
/*
   Global Commands
*/
static   int      write_poly(const char *n);
static   int      do_line(char *line);
static   int      do_holeconductor(char *line);
static   int      do_holeonly(char *line);
static   int      do_holeandfeature(char *line);
static   int      do_featureonly(char *line);
static   int      do_handf(char *line);
static   int      do_handfLDA2(char *line);
static   int      do_linearformat(char *line);
static   int      do_circularformat(char *line);
static   int      do_FDA(char *line);
static   int      do_featureonlyFDA(char *line);
static   int      do_holeonlyFDA(char *line);
static   int      do_TEST(char *line);  // 327 standard electrical test record
static   int      do_subroutine(char *line);
static   int      do_subdef(char *line);
static   int      do_subdef1(char *line);
static   int      do_subdef2(char *line);
static   int      do_subplace(char *line);
static   int      do_annotation(char *line);
static   int      do_linplace(char *line);
static   int      do_rotaryplace(char *line);
static   int      do_annoplace(char *line);
static   int      do_annoplace1(char *line);
static   int      do_annoplace2(char *line);
static   int      do_linplace1(char *l);
static   int      do_linplace2(char *line);
static   int      do_rotaryplace1(char *line);
static   int      do_rotaryplace2(char *line);
static   int      do_linearinterpolation(char *l,int fill);
static   int      do_parameter(char *line);
static   int      do_conductorsegment(char *l);
static   int      do_outlinesegment(char *l);
static   int      do_segment(char *l);
static   int      do_outline(char *l);
  
static   double   _scale2(char sign, double);  // in LDA2 format 0.0001 inch
static   double   _scale1(char sign, double);  // in LDA1 format 0.0010 inch
static   double   _ascale2(char sign, double); // angle scale in LDA2 format 0.0010 degree
static   double   _ascale(char sign, double);  // angle scale in LDA1 format 0.0001 degree

static   int      block_defined(char *n);
static   int      load_ipcsettings(const CString fname);
static   int      write_padstacks();

static   int      make_components(CNetList *NetLis);

static int        get_drillindex(double size, int layernum);
static int        get_padformindex(int f,double sizeA, double sizeB, double rotation, double offset);
static int        get_padstackindex(int drillindex ,int *pad_stackarray);

static   CString     unnamednet[30];
static   int         unnamednetcnt;

static   IPC_TestBlock  *testblock;
static   int      testblockcnt;

static   CPadformArray  padformarray;
static   int            padformcnt;

static   IPC_Poly       *polyline;
static   int            polycnt;

static   FILE           *ferr;
static   FILE           *ifp;
static   char           ipc_line[127];

static   CompInstArray  compinstarray; 
static   int            compinstcnt;

static   LongNetArray   longnetarray;  
static   int            longnetcnt;

static   CPinArray      cpinarray;  
static   int            cpincnt;

static   CDrillArray    drillarray;
static   int            drillcnt;

static   CPaddefArray   paddefarray;
static   int            paddefcnt;

static   double         refnameheight;
static   double         compoutline;

static   int            cur_layer;
static   long           command_cnt = 0;
static   double         cur_width = 0.0;
static   int            cur_blockmode = FALSE;
static   int            cur_textmode = FALSE;
static   int            cur_negative = FALSE; // negative flag  
static   int            cur_units = U_ENGLISH;
static   int            units_found;
static   int            cur_filenum = 0;
static   int            continue_arc;     // 122 record
static   int            continuerecord;   // line record
static   CCEtoODBDoc     *doc;
static   int            display_error = 0;
static   FileStruct     *file;
static   BlockTypeTag   filetype = blockTypePcb;  // set in IPC.in
static   int            makecomponent;
static   int            drilllayernum;
static   int            highest_access_layer;
static   int            unitOverride = MAX_UNITS;

static   CString        cur_nodename;

static   int            ComponentSMDrule;    // 0 = take existing ATT_SMD
                                             // 1 = if all pins of comp are marked as SMD
                                             // 2 = if most pins of comp are marked as SMD
                                             // 3 = if one pin of comp are marked as SMD

static   double         pageUnitsPerIpcUnits1;
static   double         pageUnitsPerIpcUnits2;
static   double         onemil;
static   int            pageunits = UNIT_INCHES;

static void LogSkippedInput(CString inputline, CString otherinfo)
{
#ifdef WRITE_DEBUG_TO_LOG
   fprintf(ferr, "Unknown IPC line [%s] [%s]\n", inputline, otherinfo);
#endif
}

/****************************************************************************/
/*
   This routine can read 80 char of until a line end.
*/
static char *fgetipcline(char *l, int lcnt, FILE *ifp, int newlinerecordend)
{
   int   i;
   int   c;

   if (newlinerecordend)
   {
      if (fgets(l,lcnt,ifp) == NULL)
         return NULL;
      return l;
   }

   // here skip all eventual blanks
   do
   {
      if ((c = fgetc(ifp)) == EOF)     return NULL;
   } while (c < ' ');

   l[0] = c;

   for (i=1;i<80 && i < (lcnt-1);i++)
   {
      if ((c = fgetc(ifp)) == EOF)     break;
      if (c < ' ')   break;   // line end detected
      l[i] = c;
   }
   l[i] = '\0';

   return l;
}

/****************************************************************************/
/*
*/
static int find_unnamed_net(FileStruct *f)
{
   NetStruct *net;
   POSITION  netPos;

   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   
         return TRUE; 
   }

   return FALSE;
}

void setScaleFactor(PageUnitsTag units)
{
   /*             English       English          Metric     Metric
     LDA_Format   Linear        Angular          Linear     Angular
         1        .001 inch     .001 Degree      .01 mm     .01 Radians
         2        .0001 inch    .0001 Degree     .001 mm    .001 Radians
   */

   switch (units)
   {
   case pageUnitsMilliMeters:
      cur_units = U_METRIC;
      pageUnitsPerIpcUnits1 = .01  * Units_Factor(pageUnitsMilliMeters, pageunits);
      pageUnitsPerIpcUnits2 = .001 * Units_Factor(pageUnitsMilliMeters, pageunits);
      break;
   case pageUnitsMils:
      cur_units = U_ENGLISH;
      pageUnitsPerIpcUnits1 = 1.0  * Units_Factor(pageUnitsMils, pageunits);
      pageUnitsPerIpcUnits2 = 0.1  * Units_Factor(pageUnitsMils, pageunits);
      break;
   case pageUnitsInches:
      cur_units = U_ENGLISH;
      pageUnitsPerIpcUnits1 = .001  * Units_Factor(pageUnitsInches, pageunits);
      pageUnitsPerIpcUnits2 = .0001 * Units_Factor(pageUnitsInches, pageunits);
      break;
   }
}

/******************************************************************************
* ReadIPC
*/
void ReadIPC(const char *fullPath, CCEtoODBDoc *Doc, FormatStruct *Format, int Pageunits)
{
   int   ipc_end_of_file;
   char  last0 = 'C';   // remember the last character. (init with comment)
   char  last1 = ' ';
   char  last2 = ' ';
   int   linelength = 127;
   doc = Doc;

   PageUnitsTag units = intToPageUnitsTag(Format->PortFileUnits);

   if (units == pageUnitsUndefined)
   {
      units = pageUnitsInches;
   }

   setScaleFactor(units);

   onemil = Units_Factor(UNIT_MILS, pageunits);

   pageunits = Pageunits;
   units_found = FALSE;

   CString ipcLogFile;
   ferr = getApp().OpenOperationLogFile(IPCERR, ipcLogFile);
   if (ferr == NULL) // error message already issued, just return.
      return;

   display_error = 0;
   int display_comment = 0;

   if ((polyline = (IPC_Poly  *)calloc(MAX_POLY,sizeof(IPC_Poly))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   if ((testblock = (IPC_TestBlock *)calloc(MAX_TESTBLOCK,sizeof(IPC_TestBlock))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   testblockcnt = 0;

   filetype = blockTypeUnknown;

   // if a Gerber file is already loaded with Layer stacking, watch out for it !
   int already_loaded_highest_layer = GetMaxStackupNum(doc);

   highest_access_layer = 2;  // minimum 2. A01 is always TOP !

   compinstarray.SetSize(100,100);
   compinstcnt = 0;

   longnetarray.SetSize(100,100);
   longnetcnt = 0;

   cpinarray.SetSize(100,100);
   cpincnt = 0;

   drillarray.SetSize(100,100);
   drillcnt = 0;

   padformarray.SetSize(100,100);
   padformcnt = 0;

   paddefarray.SetSize(100,100);
   paddefcnt = 0;

   CString settingsFile = getApp().getImportSettingsFilePath("ipc.in");
   {
      CString msg;
      msg.Format("\nIPC356: Settings file [%s].\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_ipcsettings(settingsFile);

   setScaleFactor(intToPageUnitsTag(unitOverride));

   // this can not be rt (because of LB terminator
   if ((ifp = fopen(fullPath,"rt")) == NULL)
   {
      ErrorMessage("Error opening file for reading.", fullPath);
      return;
   }

   if (Product == PRODUCT_GRAPHIC)
      filetype = blockTypeUnknown;

   if (makecomponent == FALSE)
      filetype  = blockTypeUnknown;

   char  drive[_MAX_DRIVE];
   char  dir[_MAX_DIR];
   char  fname[_MAX_FNAME];
   char  ext[_MAX_EXT];
   _splitpath( fullPath, drive, dir, fname, ext );
   CString filename(fname);
   filename += ext;

   file = Graph_File_Start(filename, fileTypeIpc);
   file->setBlockType(filetype);
   file->getBlock()->setBlockType(file->getBlockType());

   Graph_Level("0","",1); // first layer.

   cur_filenum = file->getFileNumber();
   ipc_end_of_file = FALSE;
   command_cnt = 0;
   continue_arc = FALSE;
   continuerecord = FALSE;
   cur_nodename = "";

   cur_layer = Graph_Level("A00","", 0);  // this is the all-layer in IPC.
   LayerStruct *l = doc->FindLayer(cur_layer);
   l->setLayerType(LAYTYPE_PAD_ALL);
   drilllayernum = Graph_Level("DRILLHOLE","",0);
   l = doc->FindLayer(drilllayernum);
   l->setLayerType(LAYTYPE_DRILL);

   while (fgetipcline(ipc_line,127,ifp, Format->newline_recordend) && !ipc_end_of_file && display_error <= 50)
   {
      // if the first line is shorter than 80 chars, it seems to be \n lineend.
      if (!command_cnt) // try to figure out the newline_recordend
      {
         if (strlen(ipc_line) > 4 && strlen(ipc_line) < 78)
            Format->newline_recordend = TRUE;
      }

      command_cnt++;

      if (strlen(ipc_line) < 3)  continue;

      if (ipc_line[0] != '0')
      {
         // this is not continueation - write out write_poly()
         write_poly(cur_nodename);
         continuerecord = FALSE;
      }
      else
         continuerecord = TRUE;

      // change mode if set to 0
      if (ipc_line[0] == '0') ipc_line[0] = last0;
      if (ipc_line[1] == '0') ipc_line[1] = last1;
      if (ipc_line[2] == '0') ipc_line[2] = last2;

      // store last
      if (isdigit(ipc_line[0]))  last0 = ipc_line[0];
      if (isdigit(ipc_line[1]))  last1 = ipc_line[1];
      if (isdigit(ipc_line[2]))  last2 = ipc_line[2];

      // 1..3 is operations code
      switch (ipc_line[0])
      {
      case 'C':
         {
            // remark are stored in the log file
            CString tmp;
            tmp = ipc_line;
            tmp.TrimRight();
            fprintf(ferr,"%s\n", tmp);
            display_comment++; // <- display errors will trigger an error message.
            write_poly(cur_nodename);
         }
         break;
      case 'P':
         // parameter record
         write_poly(cur_nodename);
         do_parameter(ipc_line);
         break;
      case '1':
         // line record
         do_line(ipc_line);
         break;
      case '2':
         // define subroutine
         write_poly(cur_nodename);
         do_subroutine(ipc_line);
         break;
      case '3':
         // new record
         do_holeconductor(ipc_line);
         break;
      case '4':
         // place subroutines
         write_poly(cur_nodename);
         do_subplace(ipc_line);
         break;
      case '5':
         // text record
         if (!cur_textmode)   write_poly(cur_nodename);
         do_annotation(ipc_line);
         break;
      case '6':   
         // netlist - skip
         break;
      case '9':
         if (!STRNICMP(ipc_line,"999",3))
         {
            write_poly(cur_nodename);
            ipc_end_of_file = TRUE;
         }
         break;
      default:
         fprintf(ferr,"Unknown IPC line [%s]\n", ipc_line);
         display_error++;
         break;
      }
   }

   if (display_error > 50)
   {
      fprintf(ferr,"Too many errors ! Please check file format\n");
   }

   fclose(ifp);

   if (filetype == blockTypePcb)
   {
      if (!find_unnamed_net(file))
      {
         fprintf(ferr, "No \"Unused Pin\" net found! Please check the IPC file and update ipc.in if needed.\n");
         display_error++;
      }

      // make components from netlist 
      if (makecomponent)
      {
         make_components(&file->getNetList());
      }

      if (already_loaded_highest_layer != highest_access_layer)
      {
         fprintf(ferr,"There is already some LayerStackup loaded, please make sure that the loaded Stackup matches the IPC Stackup!\n");
         display_error++;
      }

      if (highest_access_layer > 1)
      {
         int   layernr = Graph_Level("A01","",0);
         LayerStruct *l = doc->FindLayer(layernr);
         l->setComment("Pad Top");
         l->setLayerType(LAYTYPE_PAD_TOP);

         CString  ln;
         ln.Format("A%02d",highest_access_layer);

         layernr = Graph_Level(ln,"",0);
         l = doc->FindLayer(layernr);
         l->setComment("Pad Bottom");
         l->setLayerType(LAYTYPE_PAD_BOTTOM);
      
         layernr = Graph_Level("A00","",0);
         l = doc->FindLayer(layernr);
         l->setComment("Pad All");
         l->setLayerType(LAYTYPE_PAD_ALL);
      }
   }

   write_padstacks();

   // here assign A00 to PAD_ALL
   // and electricalstacknumber to A01 to Highest
   int   i;

   if (highest_access_layer > 1)
   {
      for (i=1;i<=highest_access_layer;i++)
      {
         CString  lname;
         lname.Format("A%02d",i);
         int layernum = Graph_Level(lname,"",0);
         LayerStruct *l = doc->FindLayer(layernum);
         l->setElectricalStackNumber(i);

         if (i == 1)
            l->setLayerType(LAYTYPE_PAD_TOP);
         else if (i == highest_access_layer)
            l->setLayerType(LAYTYPE_PAD_BOTTOM);
         else
            l->setLayerType(LAYTYPE_PAD_INNER);
      }

      CString  lname;
      lname.Format("A%02d",highest_access_layer);
      Graph_Level_Mirror("A01", lname, "");

      for (i=1;i<=highest_access_layer;i++)
      {
         CString  lname;
         lname.Format("L%02d",i);
         int layernum = Graph_Level(lname,"",0);
         LayerStruct *l = doc->FindLayer(layernum);
         l->setElectricalStackNumber(i);

         if (i == 1)
            l->setLayerType(LAYTYPE_SIGNAL_TOP);
         else if (i == highest_access_layer)
            l->setLayerType(LAYTYPE_SIGNAL_BOT);
         else
            l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }

   }
   else
   {
      // make a A01 PAD_TOP
      // A02 PAD_BOTTOM
      int layernum = Graph_Level("A01","",0);
      LayerStruct *l = doc->FindLayer(layernum);
      l->setElectricalStackNumber(1);
      l->setLayerType(LAYTYPE_PAD_TOP);

      layernum = Graph_Level("A02","",0);
      l = doc->FindLayer(layernum);
      l->setElectricalStackNumber(2);
      l->setLayerType(LAYTYPE_PAD_BOTTOM);
      Graph_Level_Mirror("A01", "A02", "");
   }

   free(polyline);
   
   update_smdpads(doc);

   generate_PINLOC(doc, file, TRUE);   

   if (makecomponent)
   {
      update_smdrule_geometries(doc, ComponentSMDrule);
      update_smdrule_components(doc, file, ComponentSMDrule);
   }

   RefreshInheritedAttributes(doc, SA_OVERWRITE);  // smd pins are after the pin instance is done.

   // there are layes created, which are not needed !
   doc->RemoveUnusedLayers();
   double accuracy = get_accuracy(doc);
   EliminateSinglePointPolys(doc);                
   BreakSpikePolys(file->getBlock()); 
   //progress->SetStatus("Crack");
   Crack(doc,file->getBlock(), TRUE);          
   //progress->SetStatus("Elim traces");
   EliminateOverlappingTraces(doc,file->getBlock(), TRUE, accuracy);                

   if (!units_found)
   {
      fprintf(ferr, "No UNITS record found.\n");
      display_error++;
   }

   for (i=0;i<testblockcnt;i++)
   {
      free(testblock[i].name);
   }

   free(testblock);

   for (i=0;i<compinstcnt;i++)
   {
      delete compinstarray[i];
   }

   compinstarray.RemoveAll();

   for (i=0;i<longnetcnt;i++)
   {
      delete longnetarray[i];
   }

   longnetarray.RemoveAll();

   for (i=0;i<cpincnt;i++)
   {
      delete cpinarray[i];
   }

   cpinarray.RemoveAll();

   padformarray.RemoveAll();  // 60 mil round
   paddefarray.RemoveAll();   // pad with drill and padstack array
   drillarray.RemoveAll();

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!ipcLogFile.IsEmpty())
      fclose(ferr);

   if ((display_error || display_comment) && !ipcLogFile.IsEmpty())
      Logreader(ipcLogFile);

   return;
}

/****************************************************************************/
/*
*/
static int is_unnamed_net(const char *n)
{
   int   i;
   CString  tmp;

   tmp = n;
   tmp.TrimLeft();
   tmp.TrimRight();

   for (i=0;i<unnamednetcnt;i++)
   {
      if (!tmp.CompareNoCase(unnamednet[i]))
         return TRUE;
   }

   return 0;
}

/****************************************************************************/
/*
*/
static CString get_longnodename(const char *n)
{
   int      i;
   CString  tmp;

   tmp = n;
   tmp.TrimLeft();
   tmp.TrimRight();

   for (i=0;i<longnetcnt;i++)
   {
#ifdef _DEBUG
   IPC_LongNet *ll = longnetarray[i];
#endif
      if (longnetarray[i]->shortnet.CompareNoCase(tmp) == 0)
         return longnetarray[i]->longnet;
   }

   if (is_unnamed_net(tmp))
      return "";

   return tmp;
}

/****************************************************************************/
/*
*/
static int do_nodename(DataStruct *d, const char *nodename)
{
   int   done = FALSE;

   if (d && strlen(nodename))
   {
      CString  n;
      n = nodename;
      n.TrimLeft();
      n.TrimRight();

      if (!strlen(n))
      {
         // ???
      }
      else if (!STRCMPI(n, "BOARD_EDGE"))
      {
         d->setGraphicClass(GR_CLASS_BOARDOUTLINE);
      }
      else if (!STRCMPI(n, "PANEL_EDGE"))
      {
         d->setGraphicClass(GR_CLASS_PANELOUTLINE);
      }
      else if (!STRCMPI(n, "SCORE_LINE"))
      {
         // ???
      }
      else if (!STRCMPI(n, "OTHER_FAB"))
      {
         // ???
      }
      else if (!STRCMPI(n, "NULL"))
      {
         // ???
      }
      else if (!STRCMPI(n, "NO"))
      {
         // NO
      }
      else
      {
         NetStruct *nn = add_net(file, n);
         d->setGraphicClass(GR_CLASS_ETCH);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, (void *)n.GetBuffer(0), SA_OVERWRITE, NULL); //  
         done = TRUE;
      }
   }
   return   done;
}

/****************************************************************************/
/*
*/
static int write_poly(const char *nodename)
{
   int   i, ws =polycnt;
   int   lineend = polyline[0].lineend;
   int   err;
   DataStruct  *d = NULL;

   if (polycnt > 0)
   {
      int fill = 0, close = 0;

      int widthIndex = Graph_Aperture("", lineend, cur_width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      d = Graph_PolyStruct(cur_layer,  0, cur_negative);
     
      if (fill = polyline[0].fill)
         close = 1;

      Graph_Poly(NULL, widthIndex, fill, 0, close);

      for (i=0;i<polycnt;i++)
      {
         // if (i > 0 && polyline[i-1].mode != polyline[i].mode)  continue;
         switch (polyline[i].mode)
         {
         case 1:
            // line
            Graph_Vertex(polyline[i].x, polyline[i].y, 0);
            break;
         case 4:
         case 7:
            // line
            Graph_Vertex(polyline[i].x, polyline[i].y, 0);
            break;
         case 2:
         case 8:
            // arc or circle
            // angles are sometimes over 360
            while (polyline[i+1].x > 360) polyline[i+1].x = polyline[i+1].x - 360;
            while (polyline[i+1].y > 360) polyline[i+1].y = polyline[i+1].y - 360;

            if (polyline[i].y == 360)
            {
               d = Graph_Circle(cur_layer,polyline[i].x, polyline[i].y,
                     fabs(polyline[i+2].x), 0L,
                     Graph_Aperture("", lineend, 0.0, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err), 
                     cur_negative, FALSE); // index of width table
            }
            else
            {
               double da;

               if (polyline[i+1].x > polyline[i+1].y)
                  da = polyline[i+1].y + 360 - polyline[i+1].x;
               else
                  da = polyline[i+1].y - polyline[i+1].x;

               if (polyline[i+1].y < 0)
                  da = da - 360;

               double bulge  = tan(DegToRad(da/4));
               Graph_Vertex(polyline[i].x, polyline[i].y, bulge);
            }

            i += 2;

            break;
         }
      }

      do_nodename(d, nodename);
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
    P record
*/
static int do_parameter(char *l)
{
    char    *lp;
    char    tmp[127];

    strcpy(tmp,l);  // do not work on the original line.

    if ((lp = strtok(tmp," \t\n")) == NULL) return TRUE;    // P record

    if ((lp = strtok(NULL," \t\n")) == NULL) return TRUE;      

    if (!STRCMPI(lp, "UNITS"))
    {
      PageUnitsTag units = pageUnitsInches;

       if ((lp = strtok(NULL," \t\n")) == NULL) return TRUE;   // CUST or SI

       if (STRCMPI(lp,"CUST") == 0)
       {
         if ((lp = strtok(NULL," \t\n")) != NULL)
         {
            if (STRCMPI(lp,"0") == 0)
            {  // inches and degrees
               units = pageUnitsInches;
            }
            else if (STRCMPI(lp,"1") == 0)
            {  // inches and degrees
               units = pageUnitsMilliMeters;
            }
            else if (STRCMPI(lp,"2") == 0)
            {  // inches and degrees
               units = pageUnitsInches;
            }
         }

         units_found = TRUE;
       }
       else if (STRCMPI(lp,"SI") == 0)
       {
         units = pageUnitsMilliMeters;

         units_found = TRUE;
       }
       else
       {
         //  Unknown units
       }

       if (units_found)
       {
         setScaleFactor(units);
       }
    }
    else if (!STRCMPI(lp, "IMAGE"))
    {
       if ((lp = strtok(NULL," \t\n")) == NULL) return TRUE;   // NCON

       cur_negative = FALSE;

       if (!STRCMPI(lp,"COND"))
       {
         if ((lp = strtok(NULL," \t\n")) == NULL) return TRUE;  // POS or NEG

         if (!STRCMPI(lp,"NEG"))
            cur_negative = TRUE;
         else
            cur_negative = FALSE;
       }

       if (!STRCMPI(lp,"NCON"))
       {
        if ((lp = strtok(NULL," \t\n")) == NULL) return TRUE;  // POS or NEG

        if (!STRCMPI(lp,"NEG"))
           cur_negative = TRUE;
        else
           cur_negative = FALSE;
       }
       else
       {
         //  Unknown IMAGE
       }

       cur_negative = FALSE;  // do not evaluate negative !!!
    }
    else if (!STRNICMP(lp, "NNAME", 5))
    {
      // long netname support.
      CString  shortname, longname;
      shortname = lp;

      if (lp = strtok(NULL," \t\n"))
      {
         longname = lp;
         IPC_LongNet *l = new IPC_LongNet;
         longnetarray.SetAtGrow(longnetcnt,l);  
         longnetcnt++;  
         l->shortnet = shortname;
         l->longnet = longname;
      }
    }
    else
    {
        // unknown P record.
    }

    return TRUE;
}

/****************************************************************************/
/*
   colum 1 record = 1
*/
static int do_line(char *l)
{

   switch (l[1])
   {
   case '1':
      // linear interpolation
      do_linearinterpolation(l,FALSE);
      break;
   case '2':
      do_circularformat(l);
      break;
   case '4':   // this is paint in
      //do_linearpaintin(l);
      do_linearinterpolation(l,TRUE);
      break;
   case '5':   // this is paint in.
      do_circularformat(l);
      break;
   case '7':
      // linear outline
      do_linearformat(l);
      break;
   case '8':
      // streches over 2 lines.
      do_circularformat(l);
      break;
   default:
      fprintf(ferr,"Start Line Record Error [%s]\n",l);
      display_error++;
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 1 record = 4
*/
static int do_subplace(char *l)
{
   switch (l[1])
   {
   case '1':
      // linear place
      do_linplace(l);
      break;
   case '2':
      // linear place
      do_rotaryplace(l);
      break;
   default:
      fprintf(ferr,"Sub place Record Error [%s]\n",l);
      display_error++;
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 1 record = 5
*/
static int do_annotation(char *l)
{
   switch (l[1])
   {
   case '1':
      // linear place
      do_annoplace(l);
      break;
   default:
      LogSkippedInput(l, "In do_annotation()");
      display_error++;
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 1 record = 2
*/
static int do_subroutine(char *l)
{
   switch (l[1])
   {
   case '1':
      do_subdef(l);
      break;
   case '2':
      do_subdef(l);
      break;
   case '9':
      if (!cur_blockmode)
      {
         // here try to clos a block without opening
         ErrorMessage(l, "Close without Open", MB_OK | MB_ICONHAND);
      }
      else
      {
         Graph_Block_Off();
      }

      cur_blockmode = FALSE;
      break;
   default:
      fprintf(ferr,"Subroutine Record Error [%s]\n",l);
      display_error++;
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 1 record = 3
   378 is conductor in 378 in IPC356A
*/
static int do_holeconductor(char *l)
{
   switch (l[1])
   {
   case '1':
      // hole and feature
      do_holeandfeature(l);
      break;
   case '2':
      // feature only
      do_featureonly(l);
      break;
   case '3': // 33x
      // hole only
      do_holeonly(l);
      break;
   case '6':   // 367
      // non plated toolinghole only
      do_holeandfeature(l);
      break;
   case '7':   // 378
      // conductor segment
      do_conductorsegment(l);
      break;
   case '8':   // 389
      do_outlinesegment(l);
      break;
   case '9':   // 397
      // features, such as targets or test status marking
      do_holeandfeature(l);
      break;
   default:
      fprintf(ferr,"Unknown IPC line - HOLE : %s\n",l);
      display_error++;
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 2 record = 3
*/
static int do_holeonly(char *l)
{
   switch (l[2])
   {
   case '1':   // feature land and hole
      do_FDA(l);
      break;
   case '2':   // feature land only 332
      do_holeonlyFDA(l);
      break;
   default:
      LogSkippedInput(l, "In do_holeonly()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 2 record = 3
*/
static int do_featureonly(char *l)
{
   switch (l[2])
   {
   case '1':   // feature land and hole
      do_FDA(l);
      break;
   case '2':   // feature land only
      do_featureonlyFDA(l);
      break;
   case '7':
      do_TEST(l);    // standart electrical test record 327
      break;
   default:
      LogSkippedInput(l, "In do_featureonly()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 3 record = 1
*/
static int do_subdef(char *l)
{
   switch (l[2])
   {
   case '1':
      do_subdef1(l);
      break;
   case '2':
      // do point record.
      do_subdef2(l);
      break;
   default:
      LogSkippedInput(l, "In do_subdef()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 2 record = 1
*/
static int do_holeandfeature(char *l)
{
   switch (l[2])
   {
   case '2':
      do_handfLDA2(l);
      break;
   case '7':
      do_handf(l);
      break;
   case '9':
      // 099 continue record for chanel assignment ???
      break;
   default:
      LogSkippedInput(l, "In do_holeandfeature()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 2 record = 1
*/
static int do_conductorsegment(char *l)
{
   switch (l[2])
   {
   case '8':
      do_segment(l);
      break;
   case '9':
      // adjaceny netname record
      break;
   default:
      LogSkippedInput(l, "In do_conductorsegment()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int do_outlinesegment(char *l)
{
   switch (l[2])
   {
   case '9':
      do_outline(l);
      break;
   default:
      LogSkippedInput(l, "In do_outlinesegment()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 2 record = 1
*/
static int do_linplace(char *l)
{
   switch (l[2])
   {
   case '1':
      do_linplace1(l);
      break;
   case '2':
      do_linplace2(l);
      break;
   default:
      LogSkippedInput(l, "In do_linplace()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 2 record = 2
*/
static int do_rotaryplace(char *l)
{
   switch (l[2])
   {
   case '1':
      do_rotaryplace1(l);
      break;
   case '2':
      do_rotaryplace2(l);
      break;
   default:
      LogSkippedInput(l, "In do_rotaryplace()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   colum 2 record = 1
*/
static int do_annoplace(char *l)
{
   switch (l[2])
   {
   case '1':
      do_annoplace1(l);
      break;
   case '2':
      do_annoplace2(l);
      break;
   default:
      LogSkippedInput(l, "In do_annoplace()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   feature description area
*/
static int do_FDA(char *l)
{
   int      res, gcode;
   double   x, y;
   char     xc, yc;
   char     hname[10];
   char     lname[10];
   char     dname[10];

   // 5-8 = feature size
   if (l[3] == 'D')
   {
      strncpy(dname,&l[3],5);
      dname[5] = '\0';
   }

   // 9 L  = datalayer
   if (l[8] == 'L')
   {
      // 10-11 = layerinfo
      strncpy(lname,&l[8],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);
   }
   // 12 S = signal information
   // 13-18 = node name

   // 19 H = Hole info
   strncpy(hname,&l[18],5);
   hname[5] = '\0';
   // 20-23 = size

   // 24-25 P ???
   // 26-27 G ???
   if (l[25] == 'G')
   {
      gcode = l[26] - '0';
   }
   else
   {
      return 0;
   }


   // 28-30 unassigned
   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_FDA()");
   }

   x = _scale2(xc,x);
   y = _scale2(yc,y);

   // ensure that aperature is in database

   /*Graph_Ap_Block(hname,0, 0.0,0.0,0,BL_APERTURE | BL_DRILL,FALSE);*/

   switch (gcode)
   {
   case 0:
      {
         double s;
         s = atof(&hname[1]);
         s = _scale2('+',s);
         // circular
         // hname[0] = '0' in units if inch in 9999 = 0.9999 inch or
         // if in mm 9999 = 0.9999 mm
         if (!block_defined(hname))
         {
/*
            Graph_Aperture(hname, T_ROUND, s, 0.0,0.0, 0.0, 0.0,0,
               BL_APERTURE | BL_DRILL,FALSE, &err);
*/
            Graph_Tool(hname, 0, s, 0, 0, 0, 0L);

         }

         Graph_Block_Reference(hname, NULL,  cur_filenum,x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      }
      break;
   case 1:
      // square
      break;
   case 2:
      // circular landeye 0.020 diameter
      break;
   case 3:
      // sqaure landeye 0.020 diameter
      break;
   case 4:
      // shape defined in D field
      Graph_Block_Reference(dname, NULL, cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      break;
   case 5:
      // same as 4 ????
      break;
   case 6:
      break;
   default:
      LogSkippedInput(l, "Unknown Gcode, In do_FDA()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   feature description area
*/
static int do_holeonlyFDA(char *l)
{
   int      res, gcode;
   double   x, y;
   char     xc, yc;
   char     hname[10];
   char     lname[10];
   char     dname[10];

   // 5-8 = feature size
   if (l[3] == 'D')
   {
      strncpy(dname,&l[3],5);
      dname[5] = '\0';
   }

   // 9 L  = datalayer
   if (l[8] == 'L')
   {
      // 10-11 = layerinfo
      strncpy(lname,&l[8],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);
   }
   // 12 S = signal information
   // 13-18 = node name

   // 19 H = Hole info
   if (l[18] == 'H')
   {
      // 20-23 = size
      strncpy(hname,&l[18],5);
      hname[5] = '\0';
   }

   // 24-25 P ??? = plated
   // 26-27 G ???
   if (l[25] == 'G')
   {
      gcode = l[26] - '0';
   }
   else
      gcode = 0;


   // 28-30 unassigned
   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_holeonlyFDA()");
   }

   x = _scale2(xc,x);
   y = _scale2(yc,y);

   // ensure that aperature is in database

   /*Graph_Ap_Block(hname,0, 0.0,0.0,0,BL_APERTURE | BL_DRILL,FALSE);*/

   switch (gcode)
   {
   case 0:
      {
         double s;
         s = atof(&hname[1]);
         s = _scale2('+',s);
         // circular
         // hname[0] = '0' in units if inch in 9999 = 0.9999 inch or
         // if in mm 9999 = 0.9999 mm
         if (!block_defined(hname))
         {
            Graph_Tool(hname, 0, s, 0, 0, 0, 0L);
/*
            Graph_Aperture(hname, T_ROUND, s, 0.0,0.0, 0.0, 0.0,0,
               BL_DRILL,FALSE, &err);
*/
         }
         Graph_Block_Reference(hname, NULL,  cur_filenum,x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      }
      break;
   case 1:
      // square
      break;
   case 2:
      // circular landeye 0.020 diameter
      break;
   case 3:
      // sqaure landeye 0.020 diameter
      break;
   case 4:
      // shape defined in D field
      //Graph_Block_Reference(dname, NULL, cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      break;
   case 5:
      // same as 4 ????
      break;
   case 6:
      break;
   default:
      LogSkippedInput(l, "Unknown Gcode, In do_holeonlyFDA()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   feature description area
*/
static int do_featureonlyFDA(char *l)
{
   int   res, gcode;
   double x, y, rotation = 0;
   char  xc, yc;
   char  lname[10];
   char  dname[10];
   char  nodename[10];
   DataStruct  *d = NULL;

   // 5-8 = feature size
   if (l[3] == 'D')
   {
      strncpy(dname,&l[3],5);
      dname[5] = '\0';
   }

   // 9 L  = datalayer
   if (l[8] == 'L')
   {
      // 10-11 = layerinfo
      strncpy(lname,&l[8],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);
   }

   // 12 S = signal information
   // 13-18 = node name
   nodename[0] = '\0';

   if (l[11] == 'S')
   {
      // 10-11 = layerinfo
      strncpy(nodename,&l[12],6);
      nodename[6] = '\0';
   }

   // H20-23
   if (l[18] == 'H')
   {
      char r[10];
      strncpy(r,&l[19],4);
      r[4] = '\0';
      rotation = atoi(r);
   }

   // 24-25 P ???
   // 26-27 G ???
   if (l[25] == 'G')
   {
      gcode = l[26] - '0';
   }
   else
   {
      return 0;
   }


   // 28-30 unassigned
   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_featureonlyFDA()");
   }

   x = _scale2(xc,x);
   y = _scale2(yc,y);

   // ensure that aperature is in database

   /*Graph_Ap_Block(hname,0, 0.0,0.0,0,BL_APERTURE | BL_DRILL,FALSE);*/

   switch (gcode)
   {
   case 0:    // ROUND
      {
         double s;
         s = atof(&dname[1]);
         s = _scale2('+',s);
         // circular
         // hname[0] = '0' in units if inch in 9999 = 0.9999 inch or
         // if in mm 9999 = 0.9999 mm

         int findex = get_padformindex(0, s, 0.0, 0.0, 0.0);
         CString  name;
         name.Format("PADSHAPE_%d",findex);

         d = Graph_Block_Reference(name, NULL,  cur_filenum,x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      }

      break;
   case 1:  // square
      {
         double s;
         s = atof(&dname[1]);
         s = _scale2('+',s);
         // circular
         // hname[0] = '0' in units if inch in 9999 = 0.9999 inch or
         // if in mm 9999 = 0.9999 mm

         int findex = get_padformindex(1, s, 0.0, 0.0, 0.0);
         CString  name;
         name.Format("PADSHAPE_%d", findex);

         d = Graph_Block_Reference(name, NULL, cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      }

      break;
   case 2:
      // circular landeye 0.020 diameter
      break;
   case 3:
      // square landeye 0.020 diameter
      break;
   case 4:
      // shape defined in D field
      d = Graph_Block_Reference(dname, NULL, cur_filenum, x, y, DegToRad(rotation), 
         0, 1.0, cur_layer, TRUE);

      break;
   case 5:
      // same as 4 ????
      break;
   case 6:
      break;
   default:
      LogSkippedInput(l, "Unknown Gcode, In do_featureonlyFDA()");
      break;
   }

   if (do_nodename(d, nodename))
   {
      d->getInsert()->setInsertType(insertTypeFreePad);
   }

   return 1;
}

/****************************************************************************/
/*
   feature description area
   record 327 - standard electrical test record
*/
static int do_TEST(char *l)
{
   int      res, i;
   double   x,y, rotDegrees = 0;
   char     xc,yc;
   double   xs,ys;
   int      form = T_ROUND, drillindex = -1;
   CString  testname;
   char     lname[10];
   char     tmp[20]; 
   char     signame[80];      // can be long netname
   char     compname[12], pinname[6];
   CString  t;
   int      pad_stack[MAX_LAYERS]; 
   int      lnr = -1;
   int      featureshape = 1;

   for (i=0;i<MAX_LAYERS;i++)
      pad_stack[i] = -1;      

   strncpy(signame,&l[3],16);
   signame[16] = '\0';
   t = signame;
   strcpy(signame,get_longnodename(t));

   strncpy(compname,&l[20],6);
   compname[6] = '\0';
   t = compname;
   t.TrimLeft();
   t.TrimRight();
   strcpy(compname,t);

   strncpy(pinname,&l[27],4);    // fixed Jan-00
   pinname[4] = '\0';
   t = pinname;
   t.TrimLeft();
   t.TrimRight();
   strcpy(pinname,t);

   testname = "TESTFEATURE";
   
   // 26-27
   if (strlen(l) > 25 && l[25] == 'G')
   {
      // G0 = Circular
      // G1 = square
      // G2 = circular with centric diameter
      // G3 = square with centric diameter
      // G4 = shape as defined in sub routine
      // G5 = Standard IPC feature as described in IPC-D-250 Point D field record
      char  form[2];
      strncpy(form,&l[26],1);
      form[2] = '\0';
      featureshape = atoi(form);
   }

   if (strlen(l) > 38 && l[38] == 'A') // 39-41 test access layer
   {
      strncpy(lname,&l[38],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);

      lnr = atoi(&lname[1]);

      if (lnr > highest_access_layer)  
         highest_access_layer = lnr;
   }
   else
      return 0;
   //

   // 42-57 XY koo
   if ((res = sscanf(&l[41], "X%c%lfY%c%lf", &xc, &x, &yc, &y)) != 4)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_TEST()");
   }
   else
   {
      x = _scale2(xc,x);
      y = _scale2(yc,y);
   }

   // 58-68 XY koo
   if ((res = sscanf(&l[57], "X%lfY%lf", &xs, &ys)) == 2)
   {
      xs = _scale2('+', xs);
      ys = _scale2('+', ys);
   }
   else if ((res = sscanf(&l[57], "X%lf", &xs)) == 1)
   {
      xs = _scale2('+', xs);
      ys = 0;
   }
   else
   {
      // ErrorMessage(l,"Wrong Coos", MB_OK | MB_ICONHAND);
      xs = ys = 0;
   }

   // ensure that aperture is in database
   if (xs < 0.001)  xs = 0.001;
   //if (ys < 0.001)  ys = 0.001;

   // 68-71 rotation. If english (inch, mils) units number in degree,
   //                 If SI units number in 1/100th of radian.
   if (strlen(l) > 67 && l[67] == 'R') // rotation. 
   {
      strncpy(tmp,&l[68],3);
      tmp[3] = '\0';
      rotDegrees = atof(tmp);  // Is actually an integer in data file.

      // If units are english then rotDegrees is already degrees.
      // If metric then convert to 1/100 radians to degrees.
      if (cur_units == U_METRIC)
         rotDegrees = RadToDeg(rotDegrees / 100.0);
   }


   int findex = get_padformindex(featureshape, xs, ys, 0.0, 0.0);
   testname.Format("PADSHAPE_%d", findex);

   // if no compname mentioned, it can not be combined to a component
   if (filetype != blockTypePcb || strlen(compname) == 0)
   {
      DataStruct *d = Graph_Block_Reference(testname, NULL, cur_filenum, x, y, DegToRad(rotDegrees), 
            0, 1.0, cur_layer, TRUE);
      do_nodename(d, signame);
   }
   else
   {
      // 0 = top 1 = inner 2 = bottom
      if (lnr == 0)
      {
         for (i=0;i<MAX_LAYERS;i++)
         {
            pad_stack[i] = findex;
         }
      }
      else if (lnr != -1)
      {
         pad_stack[lnr] = findex;
      }

      int psindex = get_padstackindex(drillindex, &pad_stack[0]);

      CString  name;
      name.Format("PADSTACK_%d", psindex);

      if (!STRCMPI(compname,"VIA"))
      {
         DataStruct *d = Graph_Block_Reference(name, NULL, cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
         d->getInsert()->setInsertType(insertTypeVia);

         do_nodename(d, signame);
      }
      else
      {
         // this is a component
         CompPinStruct  *p = NULL;

         if (strlen(signame))
         {
            NetStruct *n = add_net(file, signame);
            p = add_comppin(file, n, compname, pinname);
         }
         else
         {
            NetStruct *n = add_net(file, NET_UNUSED_PINS);
            n->setFlags(NETFLAG_UNUSEDNET); 
            p = add_comppin(file, n, compname, pinname);
         }

         // here now put PINLOC
         if (p)
         {   
            p->setPinCoordinatesComplete(true);
            p->setVisible(0);

            p->setOrigin(x,y);
            p->setRotationDegrees(rotDegrees);
            p->setMirror(0);
            BlockStruct *b = Graph_Block_On(GBO_APPEND,name,-1,0);
            Graph_Block_Off();
            p->setPadstackBlockNumber( b->getBlockNumber());

            // also in cppin
            IPC_Pin *c = new IPC_Pin;
            cpinarray.SetAtGrow(cpincnt,c);  
            cpincnt++;  
            c->compname = compname;
            c->pinname  = pinname;
            c->x = x;
            c->y = y;
            c->rot = DegToRad(rotDegrees);
            c->layerindex = cur_layer; 
            c->lnr = lnr;
            c->padstack_blocknum = b->getBlockNumber();
            c->psindex = psindex;
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int block_defined(char *n)
{
   int   i;

   for (i=0;i<testblockcnt;i++)
   {
      if (!strcmp(testblock[i].name,n))
      {
         return TRUE;
      }
   }

   if (testblockcnt < MAX_TESTBLOCK)
   {
      if ((testblock[testblockcnt].name = STRDUP(n)) == NULL)
      {
         MemErrorMessage(__FILE__, __LINE__);
      }

      testblockcnt++;
   }
   else
   {
      // need also give message
      return TRUE;
   }

   return FALSE;
}

/****************************************************************************/
/*
   317   hole and feature only in IPC356
*/
static int do_handf(char *l)
{
   int      res, i;
   double   x, y, sizex, sizey;
   double   rot = 0;
   char     hname[40];
   char     lname[10];
   char     signame[80];      // can be long netname
   char     compname[12], pinname[6];
   char     xc,yc;
   int      pad_stack[MAX_LAYERS]; 
   int      lnr = -1;
   int      featureshape = 0;

   for (i=0;i<MAX_LAYERS;i++)
   {
      pad_stack[i] = -1;      
   }

   CString  t;

   strncpy(signame, &l[3],16);
   signame[16] = '\0';
   t = signame;
   strcpy(signame, get_longnodename(t));

   strncpy(compname,&l[20],6);
   compname[6] = '\0';
   t = compname;
   t.TrimLeft();
   t.TrimRight();
   strcpy(compname,t);

   strncpy(pinname, &l[27], 4);  // fixed Jan-00
   pinname[4] = '\0';
   t = pinname;
   t.TrimLeft();
   t.TrimRight();
   strcpy(pinname,t);

   // 26-27
   if (strlen(l) > 25 && l[25] == 'G')
   {
      // G0 = Circular
      // G1 = square
      // G2 = circular with centric diameter
      // G3 = square with centric diameter
      // G4 = shape as defined in sub routine
      // G5 = Standard IPC feature as described in IPC-D-250 Point D field record
      char  form[2];
      strncpy(form, &l[26], 1);
      form[2] = '\0';
      featureshape = atoi(form);
   }

   // 28-30 unassigned
   // 39-41 access layer
   if (strlen(l) > 38 && l[38] == 'A')
   {
      // 10-11 = layerinfo
      strncpy(lname, &l[38], 3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);

      lnr = atoi(&lname[1]);

      if (lnr > highest_access_layer)  
         highest_access_layer = lnr;
   }

   // 42-57 XY koo
   if ((res = sscanf(&l[41], "X%c%lfY%c%lf", &xc, &x, &yc, &y)) == 4)
   {
      x = _scale2(xc, x);
      y = _scale2(yc, y);
   }
   else
   {
      x = y = 0;
   }

   // 58-71 size
   if ((res = sscanf(&l[57],"X%lfY%lf", &sizex, &sizey)) == 2)
   {
      sizex = _scale2('+', sizex);
      sizey = _scale2('+', sizey);
   }
   else if (res == 1)
   {
      sizex = _scale2('+', sizex);
      sizey = 0;
   }
   else
   {
      sizex = sizey = 0;   // no size found
   }

   strncpy(hname, &l[57], 10);
   hname[10] = '\0';
   t = hname;
   t.TrimLeft();
   t.TrimRight();
   strcpy(hname, t);

   // no pad size information found.
   // here use drill hole
   int   drillindex = -1;

   if (l[32] == 'D')
   {
      double d = 0.0;

      if ((res = sscanf(&l[32], "D%lf", &d)) == 1)
      {
         d = _scale2('+',d);
      }
      drillindex = get_drillindex(d, drilllayernum);
   }

   // ensure that aperture is in database
   int findex = get_padformindex(featureshape, sizex, sizey, 0.0, 0.0);

   // 0 = top 1 = inner 2 = bottom
   if (lnr == 0)
   {
      for (i=0;i<MAX_LAYERS;i++)
      {
         pad_stack[i] = findex;
      }
   }
   else if (lnr != -1)
   {
      pad_stack[lnr] = findex;
   }

   // soldermask
   if (l[72] == 'S')
   {
      int s = 0;

      if ((res = sscanf(&l[72], "S%d", &s)) == 1)
      {
         // 0 = none, 1 = top, 2 = bottom, 3 = all
         int r = 0;
      }
   }

   int psindex = get_padstackindex(drillindex,&pad_stack[0]);

   CString  name;
   name.Format("PADSTACK_%d", psindex);

   // unplated tooling hole
   if (!STRNICMP(l,"367",3))
   {
      DataStruct *d = Graph_Block_Reference(name, "", cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      d->getInsert()->setInsertType(insertTypeDrillTool);

      return 1;
   }
   else if (!STRNICMP(l,"397",3)) // fiducial or other non testable stuff
   {
      DataStruct *d = Graph_Block_Reference(name, "", cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      d->getInsert()->setInsertType(insertTypeFiducial);
      return 1;
   }

   if (filetype != blockTypePcb || !makecomponent || !strlen(compname))
   {
      CString  ref;

      if (strlen(compname))
         ref.Format("%s %s %s",signame, compname, pinname);
      else
         ref = "";

      DataStruct *d = Graph_Block_Reference(name, ref, cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
      do_nodename(d, signame);
   }
   else
   {
      if (strlen(signame))
      {
         add_net(file, signame);
      }
      else
      {
         NetStruct *n = add_net(file, NET_UNUSED_PINS);
         n->setFlags(NETFLAG_UNUSEDNET); 
      }

      if (!STRCMPI(compname,"VIA"))
      {
         DataStruct *d = Graph_Block_Reference(name, NULL, cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);
         d->getInsert()->setInsertType(insertTypeVia);

         do_nodename(d, signame);
      }
      else
      {
         // this is a component
         CompPinStruct  *p = NULL;

         if (strlen(signame))
         {
            NetStruct *n = add_net(file, signame);
            p = add_comppin(file, n, compname, pinname);
         }
         else
         {
            NetStruct *n = add_net(file, NET_UNUSED_PINS);
            n->setFlags(NETFLAG_UNUSEDNET); 
            p = add_comppin(file, n, compname, pinname);
         }

         // here now put PINLOC
         if (p)
         {  
            p->setPinCoordinatesComplete(true);
            p->setVisible(0);
            p->setOrigin(x,y);
            p->setRotationDegrees(rot);
            p->setMirror(0);
            BlockStruct *b = Graph_Block_On(GBO_APPEND,name,-1,0);
            Graph_Block_Off();
            p->setPadstackBlockNumber( b->getBlockNumber());

            // also in cppin
            IPC_Pin *c = new IPC_Pin;
            cpinarray.SetAtGrow(cpincnt,c);  
            cpincnt++;  
            c->compname = compname;
            c->pinname  = pinname;
            c->x = x;
            c->y = y;
            c->rot = DegToRad(rot);
            c->layerindex = 0; 
            c->lnr = lnr;
            c->padstack_blocknum = b->getBlockNumber();
            c->psindex = psindex;
         }
      }
   }

   return 1;
}

/****************************************************************************/
/*
   312   hole and feature  in LDA2 format (1/10 mil)
*/
static int do_handfLDA2(char *l)
{
   int      res;
   double   x,y;
   char     lname[10];
   char     xc,yc;
   char     sname[10];
   char     nodename[10];

   // 4-8 = call name
   strncpy(sname,&l[3],5);
   sname[5] = '\0';

   // 9 L  = datalayer
   if (l[8] == 'L')
   {
      // 10-11 = layerinfo
      strncpy(lname,&l[8],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);
   }

   // S signal
   // 12 S = signal information
   // 13-18 = node name
   nodename[0] = '\0';

   if (l[11] == 'S')
   {
      strncpy(nodename,&l[12],6);
      nodename[6] = '\0';
      write_poly(cur_nodename);
      cur_nodename = nodename;
      cur_nodename.TrimLeft();
      cur_nodename.TrimRight();
   }

   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_handfLDA()");
   }

   x = _scale2(xc,x);
   y = _scale2(yc,y);

   DataStruct *d = Graph_Block_Reference(sname, NULL, cur_filenum, x, y, 0.0, 0, 1.0, cur_layer, TRUE);

   if (strlen(nodename))
   {

   }

   return 1;
}

/****************************************************************************/
/*
   feature description area
   412 place
*/
static int do_linplace2(char *l)
{
   int   res, mir =0;
   double x,y,rot;
   char  xc,yc;
   char  sname[10];
   char  lname[10];

   // 4-8 = call name
   strncpy(sname,&l[3],5);
   sname[5] = '\0';

   // 9 L  = datalayer
   if (l[8] == 'L')
   {
      // 10-11 = layerinfo
      strncpy(lname,&l[8],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);
   }

   // 19 H
   // 20 23 Rotation
   if (l[18] == 'H')
   {
      char tmp[10];
      strncpy(tmp,&l[19],4);
      tmp[4] = '\0';
      rot = DegToRad(atof(tmp));
   }
/*
   // 26 G code
   // 27 0 = 0 1=90 2=180 3=270 4=mirror0 5=mirror90
   switch (l[26])
   {
      case '0':
         rot = 0;
         mir = 0;
      break;
      case '1':
         rot = DegToRad(90.0);
         mir = 0;
      break;
      case '2':
         rot = DegToRad(180.0);
         mir = 0;
      break;
      case '3':
         rot = DegToRad(270.0);
         mir = 0;
      break;
      case '4':
         rot = 0;
         mir = 1;
      break;
      case '5':
         rot = DegToRad(90.0);
         mir = 1;
      break;
      case '6':
         rot = DegToRad(180.0);
         mir = 1;
      break;
      case '7':
         rot = DegToRad(270.0);
         mir = 1;
      break;
      default:
         rot = 0;
         mir = 0;
      break;
   }
*/
   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_linplace2()");
   }

   x = _scale2(xc,x);
   y = _scale2(yc,y);

   Graph_Block_Reference(sname, NULL, cur_filenum, x, y, rot, mir, 1.0, cur_layer, TRUE);

   return 1;
}

/****************************************************************************/
/*
   feature description area
   411 place
*/
static int do_linplace1(char *l)
{
   int   res, mir =0;
   double x,y,rot;
   char  xc,yc;
   char  sname[10];

   // 4-8 = call name
   strncpy(sname,&l[3],5);
   sname[5] = '\0';

   // 19 H
   // 20 23 Rotation
   if (l[18] == 'H')
   {
      char tmp[10];
      strncpy(tmp,&l[19],4);
      tmp[4] = '\0';
      rot = DegToRad(atof(tmp));
   }
/*
   // 26 G code
   // 27 0 = 0 1=90 2=180 3=270 4=mirror0 5=mirror90
   switch (l[26])
   {
      case '0':
         rot = 0;
         mir = 0;
      break;
      case '1':
         rot = DegToRad(90.0);
         mir = 0;
      break;
      case '2':
         rot = DegToRad(180.0);
         mir = 0;
      break;
      case '3':
         rot = DegToRad(270.0);
         mir = 0;
      break;
      case '4':
         rot = 0;
         mir = 1;
      break;
      case '5':
         rot = DegToRad(90.0);
         mir = 1;
      break;
      case '6':
         rot = DegToRad(180.0);
         mir = 1;
      break;
      case '7':
         rot = DegToRad(270.0);
         mir = 1;
      break;
      default:
         rot = 0;
         mir = 0;
      break;
   }
*/
   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_linplace1()");
   }

   x = _scale1(xc,x);
   y = _scale1(yc,y);

   Graph_Block_Reference(sname, NULL, cur_filenum, x, y, rot, mir, 1.0, cur_layer, TRUE);

   return 1;
}

/****************************************************************************/
/*
   feature
*/
static int do_rotaryplace1(char *l)
{
   int   res, mir = 0;
   double x,y,rot = 0.0;
   char  xc,yc;
   char  sname[10];
   char  lname[10];

   // 4-8 = call name
   strncpy(sname,&l[3],5);
   sname[5] = '\0';

   // 9 L  = datalayer
   if (l[8] == 'L')
   {
      // 10-11 = layerinfo
      strncpy(lname,&l[8],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);
   }

   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_rotaryplace1()");
   }

   x = _scale1(xc,x);
   y = _scale1(yc,y);

   Graph_Block_Reference(sname, NULL, cur_filenum, x, y, rot, mir, 1.0, cur_layer, TRUE);

   return 1;
}

/****************************************************************************/
/*
   feature
*/
static int do_rotaryplace2(char *l)
{
   int   res, mir = 0;
   double x,y,rot = 0.0;
   char  xc,yc;
   char  sname[10];
   char  lname[10];

   // 4-8 = call name
   strncpy(sname,&l[3],5);
   sname[5] = '\0';

   // 9 L  = datalayer
   if (l[8] == 'L')
   {
      // 10-11 = layerinfo
      strncpy(lname,&l[8],3);
      lname[3] = '\0';
      cur_layer = Graph_Level(lname,"", 0);
   }

   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_rotaryplace2()");
   }

   x = _scale2(xc,x);
   y = _scale2(yc,y);

   Graph_Block_Reference(sname, NULL,  cur_filenum,x, y, rot, mir, 1.0, cur_layer, TRUE);

   return 1;
}

/****************************************************************************/
/*
*/
static int do_annoplace1(char *l)
{
   int   res, mir;
   double x,y,rot = 0,charlinew, w, h;
   char  t[127];
   char  xc,yc, wc;

   if (cur_textmode == FALSE)
   {
      cur_textmode = TRUE;
      // this is first annotation record
      res = sscanf(&l[3],"D%lf",&charlinew); // linewidth of char
      charlinew = _scale1('+',charlinew);

      if (l[8] == 'L')
      {
         // 10-11 = layerinfo
         strncpy(t,&l[8],3);
         t[3] = '\0';
         cur_layer = Graph_Level(t,"", 0);
      }

      res = sscanf(&l[18],"H%lf",&h);  // width of char
      h = _scale2('+',h);  // yes, height gets scale2 while the rest gets scale1, case dts0100393043

      // 26 G code
      // 27 0 = 0 1=90 2=180 3=270 4=mirror0 5=mirror90
      switch (l[26])
      {
      case '0':
         rot = 0;
         mir = 0;
         break;
      case '1':
         rot = DegToRad(90.0);
         mir = 0;
         break;
      case '2':
         rot = DegToRad(180.0);
         mir = 0;
         break;
      case '3':
         rot = DegToRad(270.0);
         mir = 0;
         break;
      case '4':
         rot = 0;
         mir = TRUE;
         break;
      case '5':
         rot = DegToRad(90.0);
         mir = TRUE;
         break;
      case '6':
         rot = DegToRad(180.0);
         mir = TRUE;
         break;
      case '7':
         rot = DegToRad(270.0);
         mir = TRUE;
         break;
      case '8':
         {
            double r;
            res = sscanf(&l[60],"Y%lf",&r);  // rotation
            rot = r / 100;
            if (rot < 0)  rot += 360;
            rot = DegToRad(rot);
            mir = 0;
         }

         break;
      case '9':
         {
            double r;
            res = sscanf(&l[60],"Y%lf",&r);  // roation
            rot = r / 100;
            if (rot < 0)  rot += 360;
            rot = DegToRad(rot);
            mir = TRUE;
         }

         break;
      default:
         rot = 0;
         mir = 0;
         break;
      }

      // 31-72 XY koo
      if ((res = sscanf(&l[30],"X%c%lfY%c%lfX%c%lf",&xc,&x,&yc,&y,&wc,&w)) == 0)
      {
         LogSkippedInput(l, "Incorrect coordinate format, In do_annoplace1()");
      }

      x = _scale1(xc,x);
      y = _scale1(yc,y);
      w = _scale1('+',w); // widht of complete string.
      polyline[0].x = w;
      polyline[0].y = h;
      polyline[0].lineend = T_ROUND;
      polyline[1].x = rot;
      polyline[1].y = mir;
      polyline[2].x = x;
      polyline[2].y = y;
      polyline[3].x = charlinew;
      polycnt = 3;
   }
   else
   {
      // this is textl;
      int mir = round(polyline[1].y);
      rot = polyline[1].x;

      cur_textmode = FALSE;
      strcpy(t,&l[3]);  // from 4-72
      t[72-4] = '\0';

      while (strlen(t) && isspace(t[strlen(t)-1]))
      {
         t[strlen(t)-1] = '\0';
      }

      if (strlen(t))
      {
         double w = 0.9 * (polyline[0].x / strlen(t));   // text is 10% smaller, distance between chars.
         int    prop = 1;

         Graph_Text(cur_layer, t, polyline[2].x, polyline[2].y,
                 polyline[0].y, w, rot, 0,
                 prop, (char)mir, 0, FALSE, 0, 0);
      }

      polycnt = 0;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int do_annoplace2(char *l)
{
   int   res, mir;
   double x,y,rot = 0,charlinew, w, h;
   char  t[127];
   char  xc,yc, wc;

   if (cur_textmode == FALSE)
   {
      cur_textmode = TRUE;
      // this is first annotation record
      res = sscanf(&l[3],"D%lf",&charlinew); // linewidth of char
      charlinew = _scale2('+',charlinew);

      if (l[8] == 'L')
      {
         // 10-11 = layerinfo
         strncpy(t,&l[8],3);
         t[3] = '\0';
         cur_layer = Graph_Level(t,"", 0);
      }

      res = sscanf(&l[18],"H%lf",&h);  // width of char
      h = _scale2('+',h);

      // 26 G code
      // 27 0 = 0 1=90 2=180 3=270 4=mirror0 5=mirror90
      switch (l[26])
      {
      case '0':
         rot = 0;
         mir = 0;
         break;
      case '1':
         rot = DegToRad(90.0);
         mir = 0;
         break;
      case '2':
         rot = DegToRad(180.0);
         mir = 0;
         break;
      case '3':
         rot = DegToRad(270.0);
         mir = 0;
         break;
      case '4':
         rot = 0;
         mir = TRUE;
         break;
      case '5':
         rot = DegToRad(90.0);
         mir = TRUE;
         break;
      case '6':
         rot = DegToRad(180.0);
         mir = TRUE;
         break;
      case '7':
         rot = DegToRad(270.0);
         mir = TRUE;
         break;
      case '8':
         {
            double r;
            res = sscanf(&l[60],"Y%lf",&r);  // rotation
            rot = r / 100;
            if (rot < 0)  rot += 360;
            rot = DegToRad(rot);
            mir = 0;
         }

         break;
      case '9':
         {
            double r;
            res = sscanf(&l[60],"Y%lf",&r);  // roation
            rot = r / 100;
            if (rot < 0)  rot += 360;
            rot = DegToRad(rot);
            mir = TRUE;
         }

         break;
      default:
         rot = 0;
         mir = 0;
         break;
      }

      // 31-72 XY koo
      if ((res = sscanf(&l[30],"X%c%lfY%c%lfX%c%lf",&xc,&x,&yc,&y,&wc,&w)) == 0)
      {
         LogSkippedInput(l, "Incorrect coordinate format, In do_annoplace2()");
      }

      x = _scale2(xc,x);
      y = _scale2(yc,y);
      w = _scale2('+',w); // widht of complete string.
      polyline[0].x = w;
      polyline[0].y = h;
      polyline[0].lineend = T_ROUND;
      polyline[1].x = rot;
      polyline[1].y = mir;
      polyline[2].x = x;
      polyline[2].y = y;
      polyline[3].x = charlinew;
      polycnt = 3;
   }
   else
   {
      // this is textl;
      int mir = round(polyline[1].y);
      rot = polyline[1].x;

      cur_textmode = FALSE;
      strcpy(t,&l[3]);  // from 4-72
      t[72-4] = '\0';

      while (strlen(t) && isspace(t[strlen(t)-1]))
      {
         t[strlen(t)-1] = '\0';
      }

      if (strlen(t))
      {
         double w = 0.9 * (polyline[0].x / strlen(t));   // text is 10% smaller, distance between chars.
         int    prop = 1;

         Graph_Text(cur_layer, t, polyline[2].x, polyline[2].y,
                 polyline[0].y, w, rot, 0,
                 prop, (char)mir, 0, FALSE, 0, 0);
      }

      polycnt = 0;
   }

   return 1;
}

/****************************************************************************/
/*
   subroutine description area
*/
static int do_subdef1(char *l)
{
   int   res;
   double x,y;
   char  sname[10];
   char  xc,yc;

   // 4-8 = subname
   strncpy(sname,&l[3],5);
   sname[5] = '\0';

   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_subdef1()");
   }

   if (cur_blockmode)
   {
      // here try to close a block without opening
      CString  t;
      t.Format("Open without Close at %ld",  command_cnt);
      ErrorMessage(l,t, MB_OK | MB_ICONHAND);
      Graph_Block_Off();
   }

   Graph_Block_On(GBO_APPEND,sname,cur_filenum,0);
   cur_blockmode = TRUE;

   return 1;
}

/****************************************************************************/
/*
   subroutine description area
*/
static int do_subdef2(char *l)
{
   int   res;
   double x,y;
   char  sname[10];
   char  xc,yc;

   // 4-8 = subname
   strncpy(sname,&l[3],5);
   sname[5] = '\0';

   // 31-72 XY koo
   if ((res = sscanf(&l[30],"X%c%lfY%c%lf",&xc,&x,&yc,&y)) == 0)
   {
      LogSkippedInput(l, "Incorrect coordinate format, In do_subdef2()");
   }

   if (cur_blockmode)
   {
      // here try to clos a block without opening
      ErrorMessage(l,"Open without Close", MB_OK | MB_ICONHAND);
      Graph_Block_Off();
   }

   Graph_Block_On(GBO_APPEND,sname,cur_filenum,0);
   cur_blockmode = TRUE;
   cur_layer = Graph_Level("0","",1);  // use floating layer.

   return 1;
}

/****************************************************************************/
/*

  it is also possible to have the * as the first char.

378NET1           L01 X152 X+159824Y+21337 X+162906Y+24419 X+162921Y+24441
078 *X+162989Y+24604 X+163048Y+24746 X+163053Y+24774 Y+25104 Y+26369


   returns token found !
*/
static int get_nexttoken(int *lindex, char *line, char *beginchar, double *number, char *endchar)
{
   int l = *lindex;

   *beginchar = ' ';
   *endchar = ' ';

   while (l < (int)strlen(line))
   {
      if (line[l] == 'X' || line[l] == 'Y')
      {
         *beginchar = line[l];
         l++;  // advance after finding X or Y

         break;
      }
      else if (line[l] == '*')
      {
         *endchar = line[l];
         *lindex = l;

         return TRUE;
      }

      l++;
   }
   
   if (*beginchar == ' ')  
   {
      *lindex = l;
      return FALSE;  // end of line and no start XY found
   }

   CString  num;
   num = "";

   while (l < (int)strlen(line))
   {
      if (isdigit(line[l]) || line[l] == '-' || line[l] == '+')
      {
         num += line[l];
      }
      else
      {
         break;
      }

      l++;
   }

   *endchar = line[l];
   *lindex = l;         // next start

   if (strlen(num))
   {
      *number = atof(num);
   }
   else
   {
      return FALSE;
   }

   return TRUE;
}

/****************************************************************************/
/*
   IPC356A 378
*/
static int do_segment(char *l)
{
   int      res;
   static   double   x1,y1;         // needs to remember on new lines.
   int      line_ends = T_ROUND;
   char     nodename[20];
   int      lindex = 3;
   char     beginchar, endchar;
   double   num;

   nodename[0] = '\0';

   if (!continuerecord) write_poly(cur_nodename);

   if (!continuerecord)
   {
      strncpy(nodename,&l[3],12);
      nodename[12] = '\0';
      cur_nodename = get_longnodename(nodename);

      if (l[18] == 'L')
      {
         char  lay[10];
         strncpy(lay,&l[18],3);
         lay[3] = '\0';

         clean_blank(lay);

         if (strlen(lay))
         {
            cur_layer = Graph_Level(lay,"", 0);
         }
      }

      lindex = 22;   // Unassigned field 22

      // here now width
      res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);

      if (beginchar != 'X')
      {
         ErrorMessage("X Expected!");  // here is the width
         return 0;
      }

      cur_width = _scale2(' ',num);

      res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);

      if (beginchar == 'Y')
      {
         // this is the other width 
         res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);
      }

      if (beginchar == 'X')
      {
         x1 = _scale2(' ',num);
      }
      else
      {
         ErrorMessage("X Expected!");  // here is the width
         return 0;
      }

      res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);

      if (beginchar == 'Y')
      {
         y1 = _scale2(' ',num);
      }
      else
      {
         ErrorMessage("Y Expected!");  // here is the width
         return 0;
      }

      polyline[polycnt].lineend =line_ends;
      polyline[polycnt].fill = 0;
      polyline[polycnt].x = x1;
      polyline[polycnt].y = y1;
      polyline[polycnt].mode = 1;
      polycnt++;
   }

   // if not continue, the next x or xy is width
   while (get_nexttoken(&lindex, l, &beginchar, &num, &endchar))
   {
      int coo = FALSE;

      // now here is the koos, with current x or y or xy
      if (beginchar == 'X')
      {
         x1 = _scale2(' ',num);
         coo = TRUE;
      }
      else if (beginchar == 'Y')
      {
         y1 = _scale2(' ',num);
         coo = TRUE;
      }

      if (endchar == '\0' || endchar == ' ' || endchar == '*' || endchar == 10 || 
          endchar == 13 || endchar == '\n')
      {
         if (coo)
         {
            if (polycnt < MAX_POLY-1)
            {
               polyline[polycnt].lineend =line_ends;
               polyline[polycnt].fill = 0;
               polyline[polycnt].x = x1;
               polyline[polycnt].y = y1;
               polyline[polycnt].mode = 1;
               polycnt++;
            }
         }

         if (endchar == '*')  
         {
            write_poly(cur_nodename);
            lindex++;
         }
      }
   } // while lindex < strlen(l)

   return 1;
}

/****************************************************************************/
/*
*/
static int do_outline(char *l)
{
   int      res;
   double   x1,y1;
   int      line_ends = T_ROUND;
   char     outlinename[20];
   int      lindex = 3;
   char     beginchar, endchar;
   double   num;

   outlinename[0] = '\0';

   if (!continuerecord) write_poly(cur_nodename);

   if (!continuerecord)
   {
      strncpy(outlinename,&l[3],10);
      outlinename[10] = '\0';
      CString tmp;
      tmp = outlinename;
      tmp.TrimLeft();
      tmp.TrimRight();
      cur_layer = Graph_Level(tmp,"", 0);

      if (!STRCMPI(tmp,"BOARD_EDGE"))
      {
         LayerStruct *l = doc->FindLayer(cur_layer);
         l->setLayerType(LAYTYPE_BOARD_OUTLINE);
         cur_nodename = tmp;
      }

      // here find the first X.
      if (l[22] == 'X')
      {
         // according to spec it needs to start at 23, but CAM350 has a problem and starts it earlier.
         lindex = 22;   // Unassigned field 22

         // here now width
         res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);

         if (beginchar != 'X')
         {
            ErrorMessage("X Expected!");  // here is the width
            return 0;
         }

         cur_width = _scale2(' ',num);
      }
      else
      {
         lindex  = 14;
      }

      res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);

      if (beginchar == 'Y')
      {
         // this is the other width 
         res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);
      }

      if (beginchar == 'X')
      {
         x1 = _scale2(' ',num);
      }
      else
      {
         ErrorMessage("X Expected!");  // here is the width
         return 0;
      }

      res = get_nexttoken(&lindex, l, &beginchar, &num, &endchar);

      if (beginchar == 'Y')
      {
         y1 = _scale2(' ',num);
      }
      else
      {
         ErrorMessage("Y Expected!");  // here is the width
         return 0;
      }

      polyline[polycnt].lineend =line_ends;
      polyline[polycnt].fill = 0;
      polyline[polycnt].x = x1;
      polyline[polycnt].y = y1;
      polyline[polycnt].mode = 1;
      polycnt++;
   }

   // if not continue, the next x or xy is width
   while (get_nexttoken(&lindex, l, &beginchar, &num, &endchar))
   {
      // now here is the koos, with current x or y or xy
      if (beginchar == 'X')
      {
         x1 = _scale2(' ',num);
      }
      else if (beginchar == 'Y')
      {
         y1 = _scale2(' ',num);
      }

      if (endchar == '\0' || endchar == ' ' || endchar == '*' || endchar == '\n') 
      {
         if (polycnt < MAX_POLY-1)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].fill = 0;
            polyline[polycnt].x = x1;
            polyline[polycnt].y = y1;
            polyline[polycnt].mode = 1;
            polycnt++;
         }

         if (endchar == '*')  write_poly(cur_nodename);
      }
   } // while lindex < strlen(l)

   return 1;
}

/****************************************************************************/
/*
*/
static int do_linearinterpolation(char *l, int fill)
{
   int   res;
   double x1,y1,x2,y2,x3,y3,width;
   char  xc1,yc1,xc2,yc2,xc3,yc3;
   int   line_ends = T_ROUND;
   char  nodename[10];

   nodename[0] = '\0';

   if (!continuerecord) write_poly(cur_nodename);

   switch (l[2])
   {
   case '1':
      // if D L S write_poly
      if (l[3] == 'D')
      {
         write_poly(cur_nodename);
         res = sscanf(&l[4],"%lf",&width);

         if (res == 1)
            cur_width = _scale2('+',width);
         else
            cur_width = 0;
      }

      if (l[8] == 'L')
      {
         char  lay[10];

         write_poly(cur_nodename);
         strncpy(lay,&l[8],3);
         lay[3] = '\0';
         clean_blank(lay);

         if (strlen(lay))
         {
            if (cur_blockmode && !STRCMPI(lay,"L00"))
               cur_layer = 0;
            else
               cur_layer = Graph_Level(lay,"", 0);
         }
      }

      // S signal
      // 12 S = signal information
      // 13-18 = node name
      nodename[0] = '\0';

      if (l[11] == 'S')
      {
         strncpy(nodename,&l[12],6);
         nodename[6] = '\0';
         cur_nodename = get_longnodename(nodename);
      }

      // G code 26-27
      if (l[25] == 'G')
      {
         if (l[26] == '1')
         {
            line_ends = T_SQUARE;
         }
      }

      // colum 31..72  with 2 setof of coos
      // X sign 8 digits
      res = sscanf(&l[30],"X%c%lfY%c%lfX%c%lfY%c%lfX%c%lfY%c%lf",
         &xc1,&x1,&yc1,&y1,&xc2,&x2,&yc2,&y2,&xc3,&x3,&yc3,&y3);

      if (polycnt > MAX_POLY - 6)   write_poly(cur_nodename);  // make sure it always fits.

      if (polycnt < MAX_POLY-1)
      {
         if (res >= 4)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].fill = fill;
            polyline[polycnt].x = _scale1(xc1,x1);
            polyline[polycnt].y = _scale1(yc1,y1);
            polyline[polycnt].mode = 1;
            polycnt++;
         }

         if (res >= 8)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].fill = fill;
            polyline[polycnt].x = _scale1(xc2,x2);
            polyline[polycnt].y = _scale1(yc2,y2);
            polyline[polycnt].mode = 1;
            polycnt++;
         }

         if (res >= 12)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].fill = fill;
            polyline[polycnt].x = _scale1(xc3,x3);
            polyline[polycnt].y = _scale1(yc3,y3);
            polyline[polycnt].mode = 1;
            polycnt++;
         }
      }
      break;
   case '2':
      // if D L S write_poly
      if (l[3] == 'D')
      {
         write_poly(cur_nodename);
         res = sscanf(&l[4],"%lf",&width);

         if (res == 1)
            cur_width = _scale2('+',width);
         else
            cur_width = 0;
      }
      if (l[8] == 'L')
      {
         char  lay[10];

         write_poly(cur_nodename);
         strncpy(lay,&l[8],3);
         lay[3] = '\0';

         clean_blank(lay);

         if (strlen(lay))
         {
            if (cur_blockmode && !STRCMPI(lay,"L00"))
               cur_layer = 0;
            else
               cur_layer = Graph_Level(lay,"", 0);
         }
      }

      // S signal
      // 12 S = signal information
      // 13-18 = node name
      nodename[0] = '\0';

      if (l[11] == 'S')
      {
         strncpy(nodename,&l[12],6);
         nodename[6] = '\0';
         write_poly(cur_nodename);
         cur_nodename = get_longnodename(nodename);
      }

      // G code 26-27
      if (l[25] == 'G')
      {
         if (l[26] == '1')
         {
            line_ends = T_SQUARE;
         }
         else if (l[26] == '3')
         {
            line_ends = T_ENDCAP;
         }
      }

      // colum 31..72  with 2 setof of coos
      // X sign 8 digits
      res = sscanf(&l[30],"X%c%lfY%c%lfX%c%lfY%c%lf",
         &xc1,&x1,&yc1,&y1,&xc2,&x2,&yc2,&y2);

      if (polycnt > MAX_POLY - 4)   write_poly(cur_nodename);  // make sure it always fits.

      if (polycnt < MAX_POLY-1)
      {
         if (res >= 4)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].fill = fill;
            polyline[polycnt].x = _scale2(xc1,x1);
            polyline[polycnt].y = _scale2(yc1,y1);
            polyline[polycnt].mode = 1;
            polycnt++;
         }

         if (res >= 8)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].fill = fill;
            polyline[polycnt].x = _scale2(xc2,x2);
            polyline[polycnt].y = _scale2(yc2,y2);
            polyline[polycnt].mode = 1;
            polycnt++;
         }
      }
      break;
   default:
      LogSkippedInput(l, "In do_linearinterpolation()");
      break;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int do_linearformat(char *l)
{
   int   res;
   double x1,y1,x2,y2, width;
   char  xc1,yc1,xc2,yc2;
   int   line_ends = T_ROUND;

   // if D L S write_poly
   if (l[3] == 'D')
   {
      res = sscanf(&l[4],"%lf",&width);

      if (res == 1)
         cur_width = _scale2('+',width);
      else
         cur_width = 0;
   }

   if (l[8] == 'L')
   {
      char  lay[10];
      strncpy(lay,&l[8],3);
      lay[3] = '\0';
      clean_blank(lay);

      if (strlen(lay))
      {
         cur_layer = Graph_Level(lay,"", 0);
      }
   }
   // S signal

   // G code 26-27
   if (l[25] == 'G')
   {
      if (l[26] == '1')
      {
         line_ends = T_SQUARE;
      }
   }


   switch (l[2])
   {
   case '2':
      if (polycnt > MAX_POLY - 4)   write_poly(cur_nodename);  // make sure it always fits.

      // colum 31..72  with 2 setof of coos
      // X sign 8 digits
      res = sscanf(&l[30],"X%c%lfY%c%lfX%c%lfY%c%lf",
         &xc1,&x1,&yc1,&y1,&xc2,&x2,&yc2,&y2);

      if (polycnt < MAX_POLY-1)
      {
         if (res >= 4)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].x = _scale2(xc1,x1);
            polyline[polycnt].y = _scale2(yc1,y1);
            polyline[polycnt].mode = 7;
            polycnt++;
         }

         if (res >= 8)
         {
            polyline[polycnt].lineend =line_ends;
            polyline[polycnt].x = _scale2(xc2,x2);
            polyline[polycnt].y = _scale2(yc2,y2);
            polyline[polycnt].mode = 7;
            polycnt++;
         }
      }

      break;
   default:
      LogSkippedInput(l, "In do_linearformat()");
      break;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int do_circularformat(char *l)
{
   int      res, err;
   double   x1, y1, x2, y2, x3, y3;
   char     xc1, yc1, xc2, yc2, xc3, yc3;
   int      line_ends = T_ROUND;
   double   width;

   write_poly(cur_nodename);

   // if D L S write_poly
   if (l[3] == 'D')
   {
      res = sscanf(&l[4],"%lf",&width);

      if (res == 1)
         cur_width = _scale2('+',width);
      else
         cur_width = 0;
   }

   if (l[8] == 'L')
   {
      char  lay[10];
      strncpy(lay,&l[8],3);
      lay[3] = '\0';
      clean_blank(lay);
      if (strlen(lay))
      cur_layer = Graph_Level(lay,"", 0);
   }
   // S signal

   // G code 26-27
   if (l[25] == 'G')
   {
      if (l[26] == '1')
      {
         line_ends = T_SQUARE;
      }
   }

   switch (l[2])
   {
   case '1':
      {
         // colum 31..72  with 2 setof of coos
         // X sign 8 digits
         continue_arc = TRUE;

         if (polycnt > MAX_POLY - 6)   write_poly(cur_nodename);  // make sure it always fits.

         res = sscanf(&l[30],"X%c%lfY%c%lfX%c%lfY%c%lfX%c%lfY%c%lf",
            &xc1,&x1,&yc1,&y1,&xc2,&x2,&yc2,&y2,&xc3,&x3,&yc3,&y3);

         if (polycnt < MAX_POLY-1)
         {
            if (res >= 4)
            {
               polyline[polycnt].lineend =line_ends;
               polyline[polycnt].x = _scale2(xc1,x1);
               polyline[polycnt].y = _scale2(yc1,y1);
               polyline[polycnt].mode = 8;
               polycnt++;
            }

            if (res >= 8)
            {
               // this is the angles
               polyline[polycnt].lineend =line_ends;
               polyline[polycnt].x = _scale2(xc2,x2);
               polyline[polycnt].y = _scale2(yc2,y2);
               polyline[polycnt].mode = 8;
               polycnt++;
            }

            if (res >= 12)
            {
               // this is the angles
               polyline[polycnt].lineend =line_ends;
               polyline[polycnt].x = _scale2(xc3,x3);
               polyline[polycnt].y = _scale2(yc3,y3);
               polyline[polycnt].mode = 8;
               polycnt++;
            }
         }
      }

      break;
   case '2':   // this is format 2
      {
         static   double   xc, yc;  // center
         static   double   starta, enda, da;
         static   double   radius;
         static   int      clock;

         // first xy is center point
         // next x = start angle y = end angle
         // next x = radius, y = 1 clock -1 counter clock

         if (!continue_arc)
         {
            res = sscanf(&l[30],"X%c%lfY%c%lfX%c%lfY%c%lfX%c%lfY%dX%c%lfY%c%lf",
               &xc1,&x1,&yc1,&y1,&xc2,&x2,&yc2,&y2,&xc3,&x3,&yc3,&y3);

            xc = _scale2(xc1,x1);
            yc = _scale2(yc1,y1);
            starta = _ascale2(xc2,x2);
            enda = _ascale2(yc2,y2);

            if (res == 8)
            {
               // the last koo is in the next line
               continue_arc = TRUE;
            }
            else if (res == 12)
            {  
               // all in the same line
               continue_arc = FALSE;

               radius = _scale2(xc3,x3);   

               if (yc3 == '-')
                  da = AngleStartEndDegree(starta, enda, TRUE);
               else
                  da = AngleStartEndDegree(starta, enda, FALSE);

               int widthIndex = Graph_Aperture("", line_ends, cur_width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

               Graph_Arc(cur_layer, xc, yc, radius, DegToRad(starta), DegToRad(da), 0L, widthIndex, 0);

            }
         }
         else
         {
            res = sscanf(&l[30],"X%c%lfY%c%lf", &xc3,&x3,&yc3,&y3);
         
            radius = _scale2(xc3,x3);   

            if (fabs(starta - enda) == 360)
            {
               // full circle
               int widthIndex = Graph_Aperture("", line_ends, cur_width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
               Graph_Circle(cur_layer, xc, yc, radius, 0L, widthIndex, 0, FALSE);
            }
            else
            {
               if (yc3 == '-')
                  da = AngleStartEndDegree(starta, enda, TRUE);   // clockwise
               else
                  da = AngleStartEndDegree(starta, enda, FALSE);

               int widthIndex = Graph_Aperture("", line_ends, cur_width, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

               Graph_Arc(cur_layer, xc, yc, radius, DegToRad(starta), DegToRad(da), 0L, widthIndex, 0);
            }

            continue_arc = FALSE;
         }
      }

      break;
   default:
      continue_arc = TRUE;
      fprintf(ferr,"do_circularformat Record Error [%s]\n",l);
      display_error++;

      break;
   }

   return 1;
}

/****************************************************************************/
/*
   LDA format 2
   0.0001 inch
   0.0001 degree
   0.001  mm
   0.001 radians

   cur_units = U_ENGLISH or U_METRIC

*/
double _scale2(char sign,double xx)
{
   double x = fabs(xx);

   if (sign == ' ')  x = xx;
   if (sign == '-')  x = -x;

   return x*pageUnitsPerIpcUnits2;
}

// angle scale
double _ascale(char sign,double xx)
{
   double x = fabs(xx);

   if (sign == '-')  x = -x;

   if (cur_units == U_ENGLISH)
      return x / 1000;

   return RadToDeg(x/100);
}

/****************************************************************************/
/*
   LDA format 1
   0.001 inch
   0.001 degree
   0.01  mm
   0.01  radians
*/
double _scale1(char sign,double xx)
{
   double x = fabs(xx);

   if (sign == '-')  x = -x;

   return x*pageUnitsPerIpcUnits1;
}

// format 2 angle.
// either 0.0001 degree if units are inch
// or 0.001 radians if units are mm
double _ascale2(char sign,double xx)
{
   double x = fabs(xx);

   if (sign == '-')  x = -x;

   if (cur_units == U_ENGLISH)
      return x / 10000;

   return RadToDeg(x/100);
}

/****************************************************************************/
/*
*/
static int load_ipcsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   filetype = blockTypeUnknown;
   refnameheight = 0.07 * Units_Factor(UNIT_INCHES, pageunits);   // inch unit
   compoutline = 0.05 * Units_Factor(UNIT_INCHES, pageunits);
   makecomponent = TRUE;
   ComponentSMDrule = 0;

   unnamednetcnt = 0;
   unnamednet[unnamednetcnt] = "N/C";
   unnamednetcnt++;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no message
      // no settings file found
      //CString tmp;
      //tmp.Format("File [%s] not found",fname);
      //ErrorMessage(tmp,"IPC Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      CString  w = line;

      if ((lp = get_string(line," \t\n")) == NULL) continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".FILETYPE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 

            if (!STRCMPI(lp,"PCB"))
               filetype  = blockTypePcb;
         }
         else if (!STRICMP(lp,".REFNAME"))
         {
            double   r;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 

            r = atof(lp);
            refnameheight = r * Units_Factor(UNIT_INCHES, pageunits);
            int u = GetUnitIndex(lp);

            if (u > -1)
               refnameheight = r * Units_Factor(u, pageunits);
         }
         else if (!STRICMP(lp,".COMPOUTLINE"))
         {
            double   c;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 

            c = atof(lp);
            compoutline = c * Units_Factor(UNIT_INCHES, pageunits);

            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            int u = GetUnitIndex(lp);

            if (u > -1)
               compoutline = c * Units_Factor(u, pageunits);
         }
         else if (!STRCMPI(lp,".MAKE_COMPONENT"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (lp[0] == 'n' || lp[0] == 'N')
               makecomponent = FALSE;
         }
         else if (!STRCMPI(lp,".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            ComponentSMDrule = atoi(lp);
         }
         else if (!STRICMP(lp,".UNNAMEDNET"))
         {
            CString c;

            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 

            c = lp;
            c.TrimLeft();
            c.TrimRight();

            if (unnamednetcnt < 30)
            {
               unnamednet[unnamednetcnt] = c;
               unnamednetcnt++;
            }
            else
            {
               fprintf(ferr, "Too many unnamed nets defined.\n");
               display_error++;
            }
         }
         else if (!STRICMP(lp,".UNIT_OVERRIDE"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;

            if (STRICMP(lp,"inch") == 0)
            {
               unitOverride = UNIT_INCHES;
            }
            else if (STRICMP(lp,"mil") == 0)
            {
               unitOverride = UNIT_MILS;
            }
            else if (STRICMP(lp,"mm") == 0)
            {
               unitOverride = UNIT_MM;
            }
         }
      }
   }

   fclose(fp);

   return 1;
}

/****************************************************************************/
/*
*/
static int component_already_done(const char *cname)
{
   int   i;

   for (i=0;i<compinstcnt;i++)
   {
      if (!compinstarray[i]->compname.CompareNoCase(cname))
         return 1;
   }

   IPC_CompInst *c = new IPC_CompInst;
   compinstarray.SetAtGrow(compinstcnt,c);  
   compinstcnt++; 

   c->compname = cname;

   return 0;
}

/****************************************************************************/
/*
*/
static int do_component(const char *cname)
{
   int      i;
   IPC_Pin  *comppinarray[MAX_COMPPIN];
   int      comppincnt = 0;

   for (i=0;i<cpincnt;i++)
   {
      if (!strcmp(cpinarray[i]->compname, cname))
      {
         // load into array
         if (comppincnt < MAX_COMPPIN)
         {
            comppinarray[comppincnt] = cpinarray[i]; 
            comppincnt++;
         }
      }
   }

   // here write array
   if (comppincnt == 0) return 1;

   // here check if all pins are placed on bottom
   int   bottom_place = TRUE;
   for (i=0;i<comppincnt;i++)
   {
      if (comppinarray[i]->lnr != highest_access_layer)
         bottom_place = FALSE;
   }

   CString  shapename;

   shapename.Format("SHAPE_%s",cname);

   // Filenumber is -1 for global
   BlockStruct *c = Graph_Block_On(GBO_APPEND,shapename, -1, 0);
   c->setBlockType(BLOCKTYPE_PCBCOMPONENT);

   double   x1,y1;
   double   xmin, ymin, xmax, ymax;

   xmin = ymin = FLT_MAX;
   xmax = ymax = -FLT_MAX;

   for (i=0;i<comppincnt;i++)
   {
      double   x,y, rot;

      x = comppinarray[i]->x;
      y = comppinarray[i]->y;
      rot = comppinarray[i]->rot;

      if (i == 0)
      {
         x1 = x;
         y1 = y;
      }

      if ((x-x1) < xmin)   xmin = x-x1;
      if ((y-y1) < ymin)   ymin = y-y1;
      if ((x-x1) > xmax)   xmax = x-x1;
      if ((y-y1) > ymax)   ymax = y-y1;
      
      int   li = comppinarray[i]->layerindex;

      if (bottom_place)
      {
         // here mirror layer if needed.
         if (comppinarray[i]->lnr == highest_access_layer)
            li = Graph_Level("A01","",0);
      }


      // the padstack_blocknum on the component is wrong if the component is bottom_place.
      // I need to get the un-mirrored padstack.
      BlockStruct *b;

      if (bottom_place)
      {
         // here generate a mirrored version of this padstack.
         CString  name;
         name.Format("PADSTACK_%d",comppinarray[i]->psindex+paddefcnt);
         b = Graph_Block_On(GBO_APPEND,name,-1,0);
         b->setBlockType(BLOCKTYPE_PADSTACK);
         Graph_Block_Off();
         paddefarray[comppinarray[i]->psindex].mirror_used = TRUE;
      }
      else
      {
         b = doc->Find_Block_by_Num(comppinarray[i]->padstack_blocknum);
      }

      DataStruct *d = Graph_Block_Reference(b->getName(), comppinarray[i]->pinname, 0, 
         x-x1, y-y1, rot, 0, 1.0, li, TRUE);
      d->getInsert()->setInsertType(insertTypePin);
      // do pinnr here
      if (strlen(comppinarray[i]->pinname))
      {
         int   pnr = atoi(comppinarray[i]->pinname);

         if (pnr)
         {
            doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_COMPPINNR, TRUE),
               VT_INTEGER, &pnr, SA_OVERWRITE, NULL); //  
         }
      }     
   }

   xmin -= compoutline; 
   ymin -= compoutline;
   xmax += compoutline; 
   ymax += compoutline;
   // component outline

   int   lay;
   lay = Graph_Level("OUTLINE_TOP","",0);

   DataStruct *d = Graph_PolyStruct(lay,  0, FALSE);
   d->setGraphicClass(GR_CLASS_COMPOUTLINE);
   Graph_Poly(NULL, 0, 0, 0, 1);
   Graph_Vertex(xmin, ymin, 0);
   Graph_Vertex(xmax, ymin, 0);
   Graph_Vertex(xmax, ymax, 0);
   Graph_Vertex(xmin, ymax, 0);
   Graph_Vertex(xmin, ymin, 0);

   Graph_Block_Off();

   double xref, yref;

   xref = (xmax - xmin)/2 + xmin - strlen(cname)*refnameheight*0.7/2;
   yref = (ymax - ymin)/2 + ymin - refnameheight/2;

   DataStruct *dd = Graph_Block_Reference(shapename, cname, 0, x1, y1, 0.0, 0, 1.0, -1, TRUE);

   if (bottom_place)
   {
      dd->getInsert()->setMirrorFlags(MIRROR_LAYERS);
      dd->getInsert()->setPlacedBottom(true);
   }

   dd->getInsert()->setInsertType(insertTypePcbComponent);

   lay = Graph_Level("REFNAME_TOP", "", 0);
   doc->SetVisAttrib(&dd->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, TRUE),
               VT_STRING, (char *)cname,
               xref, yref,0.0, refnameheight, refnameheight*0.7, 1, 0, TRUE, SA_OVERWRITE, 0L, lay, 0, 0, 0);

   return 1;
}

/****************************************************************************/
/*
*/
static int make_components(CNetList *NetList)
{
   // loop through netlist 

   NetStruct      *net;
   CompPinStruct  *compPin;
   POSITION       compPinPos, netPos;

   netPos = NetList->GetHeadPosition();

   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();

      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (!component_already_done(compPin->getRefDes()))
         {
            do_component(compPin->getRefDes());
         }
      }

      // layer stuff
      int lay = Graph_Level("OUTLINE_BOTTOM","",0);

      LayerStruct *l = doc->FindLayer(lay);
      l->setComment("Outline Bottom");
      l->setLayerType(LAYTYPE_BOTTOM);
      lay = Graph_Level("OUTLINE_TOP","",0);
      l = doc->FindLayer(lay);
      l->setComment("Outline Top");
      l->setLayerType(LAYTYPE_TOP);
      Graph_Level_Mirror("OUTLINE_BOTTOM", "OUTLINE_TOP", "");

      lay = Graph_Level("REFNAME_BOTTOM","",0);
      l = doc->FindLayer(lay);
      l->setLayerType(LAYTYPE_BOTTOM);
      lay = Graph_Level("REFNAME_TOP","",0);
      l = doc->FindLayer(lay);
      l->setLayerType(LAYTYPE_TOP);
      Graph_Level_Mirror("REFNAME_BOTTOM", "REFNAME_TOP", "");
   }

   return 1;
}

/****************************************************************************/
/*
   f = 
      0 = Circular
      1 = square
      2 = circular with centric diameter
      3 = square with centric diameter
      4 = shape as defined in sub routine
      5 = Standard IPC feature as described in IPC-D-250 Point D field record
*/
static int get_padformindex(int f,double sizeA, double sizeB, double rotation, double offset)
{
   IPC_Padform p;
   int      i;
   double   offsetx, offsety;

   if (f > -1)
   {
      switch (f)
      {
      case 0:
      case 2:
         if (sizeB == 0)
            f = T_ROUND;
         else
         if (sizeA == sizeB)
            f = T_ROUND;
         else
            f = T_OBLONG;
         break;
      case 1:
      case 3:
         if (sizeB == 0)
            f = T_SQUARE;
         else
         if (sizeA == sizeB)
            f = T_SQUARE;
         else
            f = T_RECTANGLE;
         break;
      default:
         // unddefined 
         f = -1;
         break;
      }
   }

   if (f < 0)
   {
      if (sizeA == 0)
         f = T_UNDEFINED;
      else if (sizeB == 0)
         f = T_ROUND;
      else if (sizeA == sizeB)
         f = T_SQUARE;
      else
         f = T_RECTANGLE;
   }

   offsetx = offsety = 0;

   for (i=0;i<padformcnt;i++)
   {
      p = padformarray.ElementAt(i);

      if (p.form == f &&
          p.sizeA == sizeA &&
          p.sizeB == sizeB &&
          p.rotation == rotation &&
          p.offsetx == offsetx &&
          p.offsety == offsety)
      {
         return i;         
      }
   }

   CString  name;
   name.Format("PADSHAPE_%d",padformcnt);
   //BlockStruct  *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
   //curblock->blocktype = BLOCKTYPE_PADSHAPE;
   int   err;
   Graph_Aperture(name, f, sizeA, sizeB, 0.0, 0.0, DegToRad(rotation), 0, 
            BL_APERTURE, TRUE, &err);

   p.form = f;
   p.sizeA = sizeA;
   p.sizeB = sizeB;
   p.rotation = rotation;
   p.offsetx = 0;
   p.offsety = 0;
   padformarray.SetAtGrow(padformcnt,p);  
   padformcnt++;

   return padformcnt -1;
}

/****************************************************************************/
/*
*/
static int get_padstackindex(int drillindex ,int *pad_stackarray)
{
   IPC_Paddef p;
   int    i, ii;

   // here check if the same
   for (i=0;i<paddefcnt;i++)
   {
      int found = TRUE;
      p = paddefarray.ElementAt(i);

      if (p.toolindex != drillindex)   found = FALSE;

      for (ii=0;ii<MAX_LAYERS;ii++)
      {
         if (p.padstack[ii] != pad_stackarray[ii])
            found = FALSE;
      }   

      if (found)
         return i;
   }

   p.toolindex = drillindex;
   p.typ = 0;
   p.mirror_used = 0;

   for (i=0;i<MAX_LAYERS;i++)
   {
      p.padstack[i] = pad_stackarray[i];

      if (i == 0 && pad_stackarray[i] > -1)  // top
         p.typ |= 0x1;

      if (i == 1 && pad_stackarray[i] > -1)  // inner
         p.typ |= 0x3;

      if (i == 2 && pad_stackarray[i] > -1)  // bottom
         p.typ |= 0x2;
   }

   if (p.toolindex > -1)
      p.typ |= 0x4;

   paddefarray.SetAtGrow(paddefcnt,p);  
   paddefcnt++;

   return paddefcnt -1;
}

/****************************************************************************/
/*
*/
static int get_drillindex(double size, int layernum)
{
   IPC_Drill   p;
   int         i;

   if (fabs(size) < SMALLNUMBER) return -1;

   for (i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);

      if (p.d == size)
         return p.toolindex;         
   }

   CString  name;
   name.Format("DRILL_%d",drillcnt);

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   p.d = size;
   p.toolindex = drillcnt;

   drillarray.SetAtGrow(drillcnt,p);  
   drillcnt++;

   return drillcnt-1;
}

/****************************************************************************/
/*
*/
static int write_padstacks()
{
   int   i, ii;
   CString  name;

   for (i=0;i<paddefcnt;i++)
   {
      int smd = 0;
      name.Format("PADSTACK_%d",i);
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);
      IPC_Paddef p = paddefarray.ElementAt(i);

      for (ii=0;ii<=highest_access_layer;ii++)
      {
         CString  lname;
         CString  padshapename;

         if (p.padstack[ii] > -1)
         {
            lname.Format("A%02d",ii);
            int layernum = Graph_Level(lname,"",0);

            padshapename.Format("PADSHAPE_%d",p.padstack[ii]);
            // must be filenum 0, because apertures are global.
            Graph_Block_Reference(padshapename, NULL, 0, 0.0, 0.0, 0.0,
                               0, 1.0, layernum, TRUE);
            if (ii == 1)   // top
               smd |= 1;
            else           // all other are greater 1
               smd |= 2;
         }
      }     

      if (p.toolindex > -1)
      {
         // here make a drill aperture.
         int drillayer = Graph_Level("DRILLHOLE","",0);
         CString  drillname;
         drillname.Format("DRILL_%d",p.toolindex);
         // must be filenum 0, because apertures are global.
         Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                               0, 1.0, drillayer, TRUE);
      }

      Graph_Block_Off();

      // here set SMD flag if no drill and 
      if (smd == 1)
      {
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE, NULL, SA_OVERWRITE, NULL); //  
      }
   }

   // here if mirrored are used
   for (i=0;i<paddefcnt;i++)
   {
      IPC_Paddef p = paddefarray.ElementAt(i);

      if (p.mirror_used == FALSE)   continue;

      int smd = 0;
      name.Format("PADSTACK_%d",i+paddefcnt);
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      for (ii=0;ii<=highest_access_layer;ii++)
      {
         CString  lname;
         CString  padshapename;

         if (p.padstack[ii] > -1)
         {
            if (ii == 0)   // all layers
               lname.Format("A%02d",ii);
            else
               lname.Format("A%02d",highest_access_layer-ii+1);

            int layernum = Graph_Level(lname,"",0);

            padshapename.Format("PADSHAPE_%d",p.padstack[ii]);
            // must be filenum 0, because apertures are global.
            Graph_Block_Reference(padshapename, NULL, 0, 0.0, 0.0, 0.0,
                               0, 1.0, layernum, TRUE);
            if (ii == highest_access_layer)  // top ->but mirrored
               smd |= 1;
            else           // all other are greater 1
               smd |= 2;
         }
      }     

      if (p.toolindex > -1)
      {
         // here make a drill aperture.
         int drillayer = Graph_Level("DRILLHOLE","",0);
         CString  drillname;
         drillname.Format("DRILL_%d",p.toolindex);
         // must be filenum 0, because apertures are global.
         Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                               0, 1.0, drillayer, TRUE);
      }

      Graph_Block_Off();

      // here set SMD flag if no drill and 
      if (smd == 1)
      {
         doc->SetAttrib(&curblock->getAttributesRef(),doc->IsKeyWord(ATT_SMDSHAPE, TRUE),
            VT_NONE, NULL, SA_OVERWRITE, NULL); //  
      }
   }

   return 1;
}

// end IPC_IN.CPP
