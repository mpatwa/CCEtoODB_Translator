// $Header: /CAMCAD/5.0/read_wrt/FatfIn.cpp 57    5/23/07 6:53p Lynn Phung $
           
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.

   Fabmaster must either have a PAD or a BLOCK assigned as a padstack.
*/

/**************RECENT UPDATES****************
*  
*  06-07-02: TSR 3656 - Sadek Noureddine
*
*  07-03-02: TSR 3665 - Sadek Noureddine
*
*
*
*
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"
#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "polylib.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "lyrmanip.h"
#include "ee.h"
#include "fatfin.h"
#include "CCEtoODB.h"
#include "dft.h"
#include "InFile.h"
#include "RwUiLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LineStatusUpdateInterval 200
     
/* Variables Declaration *****************************************************************************/
static CDrillArray   drillarray;
static int        drillcnt;

static CAttrmapArray attrmaparray;
static int           attrmapcnt;

static CNailArray    nailarray;  // used in fab_nail
static int           nailcnt;

static CNaildefArray naildefarray;  // used in fab_nail
static int           naildefcnt;

static CFontArray    fontarray;
static int           fontcnt;

static CApertureArray   aperturearray;
static int           aperturecnt;

static CNetnameArray netnamearray;
static int           netnamecnt;

static FATFCompArray compArray;
static int           compArrayCount;

static CPacklistArray   packlistarray;
static int           packlistcnt;

static CComppinArray comppinarray;
static int           comppincnt;

static CPackpinArray packpinarray;
static int           packpincnt;

static CLayernameArray  layernamearray;
static int           layernamecnt;

static CLayersetArray   layersetarray;
static int           layersetcnt;

static CPadstackArray   padstackarray;
static int           padstackcnt;

static PackagePinArray  packagePinArray;
static int           packagePinArrayCount;

static CCEtoODBDoc    *doc;
static FileStruct    *file = NULL;
static int           cur_filenum = 0;
static DataStruct    *cur_compdata;    // global pointer to the current component.

static FILE          *ifp;                         /* File pointers.    */
static long          ifp_line = 0;                 /* Current line number. */
static FILE          *ferr;
static int           display_error = 0;
static char          token[MAX_LINE];              /* Current token.       */
static int           Push_tok = FALSE;
static int           token_name = FALSE;
static int           end_of_file;
static char          cur_line[MAX_LINE];
static int           cur_new = TRUE;
static bool          default_top = true;

static int           polycnt = 0;
static CPolyArray    poly_l;

static int           PageUnits;
static double        unitsfactor = 1;
static double        attheight, attwidth;
static int           ComponentSMDrule; // 0 = take existing ATT_SMD
                                 // 1 = if all pins of comp are marked as SMD
                                 // 2 = if most pins of comp are marked as SMD
                                 // 3 = if one pin of comp are marked as SMD
static bool          UsePinNum;
static bool          IgnoreNetsAndTraces;
static bool          IgnoreVias;

static CFileReadProgress*  fileReadProgress = NULL;

/* Function Prototype *****************************************************************************/
static void InitializeMembers();
static void FreeMembers();
static double cnv_unit(const char *);
static int go_command(List *, int);
static int get_tok();
static int p_error();
static int tok_search(List *, int);
static int push_tok();
static int loop_command(List *, int);
static int update_pdef(int, int);
static int get_next(char *, int);
static int get_line(char *, int);
static int get_pincnt(const char *, int, int *);
static int do_planenet(char *);
static GraphicClassTag get_layerclass(const char *layername);
static int assign_layer();
static int make_netlist(COperationProgress* progress=NULL);
static int make_dummypads(int page_unit);
static int tst_fabmaster_format();
static int load_fatfsettings(CString settingsFileName);
static int load_fabasciisettings(CString settingsFileName);
static const char *get_probename(int typ);
static int fabnail_netcheck(FileStruct *file);
static int fabnail_calcoffset(FileStruct *file);
static int fabnail_associatevias(FileStruct *file);
static int fabnail_storenails(FileStruct *file);

static CFatfReader* s_fatfReader = NULL;

CFatfReader& getFatfReader()
{
   if (s_fatfReader == NULL)
   {
      s_fatfReader = new CFatfReader(*doc);
   }

   return *s_fatfReader;
}


/******************************************************************************
* ReadFATF
*/
void ReadFATF(const char *f, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits )
{
   file = NULL;
   doc = Doc;
   display_error = 0;

   PageUnits = pageUnits;
   unitsfactor = Units_Factor(UNIT_INCHES, PageUnits);

   Push_tok = FALSE;
   cur_new = TRUE;
   cur_line[0] = '\0';
   ifp_line = 0;

   attheight = 0.07;
   attwidth  = 0.05;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(f, "rt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", f);
      ErrorMessage(t, "Error");
      return;
   }

   delete fileReadProgress;
   fileReadProgress = new CFileReadProgress(ifp);

   CString fatfLogFile = GetLogfilePath(FATFERR);
   if ((ferr = fopen(fatfLogFile, "wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", fatfLogFile);
      ErrorMessage(t, "Error");
      return;
   }

   InitializeMembers();

   /* initialize scanning parameters */
   ComponentSMDrule = 0;
   UsePinNum           = false;
   IgnoreNetsAndTraces = false;
   IgnoreVias          = false;

   CString settingsFile( getApp().getImportSettingsFilePath("fatf.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nFATF Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_fatfsettings(settingsFile);

   // 
   // SPLIT THE FILE NAME TO GET RID OF THE PATH
   
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   _splitpath(f, drive, dir, fname, ext);
   CString fileName = fname;
   fileName += ext;

   file = Graph_File_Start(fileName, Type_FABMASTER);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());

   getFatfReader().setPcbFile(file);

   cur_filenum = file->getFileNumber();

   // make it the first layer
   Graph_Level("0","",1);

   make_dummypads(pageUnits);

   if (!tst_fabmaster_format())
   {
      ErrorMessage("This is not a FABMASTER FATF file format","Wrong File Format");
   }
   else
   {
      Push_tok = FALSE;
      cur_new = TRUE;
      cur_line[0] = '\0';
      ifp_line = 0;
      end_of_file = FALSE;
      rewind(ifp);
      loop_command(init_lst, SIZ_INIT_LST);  // fonts and layersets can be out of order
   
      Push_tok = FALSE;
      cur_new = TRUE;
      cur_line[0] = '\0';
      ifp_line = 0;
      end_of_file = FALSE;
      rewind(ifp);
      loop_command(padstack_lst, SIZ_PADSTACK_LST);

      // Case dts0100466362 - Nets may appear before or after the general graphics that
      // define traces and such, we need to be sure to get it before (not intrinsicly, but due 
      // to the way the rest of this importer is written), so this process step specifically 
      // extracts the net mapping info.
      Push_tok = FALSE;
      cur_new = TRUE;
      cur_line[0] = '\0';
      ifp_line = 0;
      end_of_file = FALSE;
      rewind(ifp);
      loop_command(nets_lst, SIZ_NETS_LST);

      Push_tok = FALSE;
      cur_new = TRUE;
      cur_line[0] = '\0';
      ifp_line = 0;
      end_of_file = FALSE;
      rewind(ifp);
      loop_command(final_lst, SIZ_FINAL_LST);
   }

   fclose(ifp);

   COperationProgress progress;
   
   assign_layer();

   progress.updateStatus("Generating Net List",0.);
   make_netlist(&progress);

   update_smdpads(doc);
   update_smdrule_geometries(doc, ComponentSMDrule);

   progress.updateStatus("Updating SMD Rules for components",0.);
   update_smdrule_components(doc, file, ComponentSMDrule,&progress);

   progress.updateStatus("Generating Padstack access flags",0.);
   generate_PADSTACKACCESSFLAG(doc, 1);

   doc->purgeUnusedWidthsAndBlocks(false);

   getFatfReader().fixBlockTypesAndInsertTypes();

   // this function generates the PINLOC argument for all pins.
   progress.updateStatus("Generating Pin Locations",0.);
   generate_PINLOC(doc, file, 1,&progress);   

   progress.updateStatus("Freeing resources",0.);
   FreeMembers();

   fclose(ferr);

   if (display_error)
      Logreader(fatfLogFile);

   delete fileReadProgress;
   fileReadProgress = NULL;

   Format->setEnableGenerateSmdComponents(false);

   delete s_fatfReader;
   s_fatfReader = NULL;
}

/******************************************************************************
* InitializeMembers
*/
static void InitializeMembers()
{
   poly_l.SetSize(100, 100);
   polycnt = 0;

   attrmaparray.SetSize(100, 100);
   attrmapcnt = 0;

   fontarray.SetSize(100, 100);
   fontcnt = 0;

   aperturearray.SetSize(100, 100);
   aperturecnt = 0;

   netnamearray.SetSize(100, 100);
   netnamecnt = 0;

   compArray.SetSize(100, 100);
   compArrayCount = 0;

   packlistarray.SetSize(100, 100);
   packlistcnt = 0;

   comppinarray.SetSize(100, 100);
   comppincnt = 0;

   packpinarray.SetSize(100, 100);
   packpincnt = 0;

   layernamearray.SetSize(100, 100);
   layernamecnt = 0;

   layersetarray.SetSize(100, 100);
   layersetcnt = 0;

   padstackarray.SetSize(100, 100);
   padstackcnt = 0;

   packagePinArray.SetSize(100, 100);
   packagePinArrayCount = 0;

   drillarray.SetSize(100, 100);
   drillcnt = 0;
}

/******************************************************************************
* FreeMembers
*/
static void FreeMembers()
{
   poly_l.RemoveAll();
   polycnt = 0;

	int i=0;
   for (i=0; i<attrmapcnt; i++)
      delete attrmaparray[i];  
   attrmaparray.RemoveAll();
   attrmapcnt = 0;

   for (i=0; i<fontcnt; i++)
      delete fontarray[i];  
   fontarray.RemoveAll();
   fontcnt = 0;

   for (i=0; i<aperturecnt; i++)
      delete aperturearray[i];  
   aperturearray.RemoveAll();
   aperturecnt = 0;

   for (i=0; i<netnamecnt; i++)
      delete netnamearray[i];  
   netnamearray.RemoveAll();
   netnamecnt = 0;

   for (i=0; i<compArrayCount; i++)
      delete compArray[i];  
   compArray.RemoveAll();
   compArrayCount = 0;

   for (i=0; i<packlistcnt; i++)
      delete packlistarray[i];  
   packlistarray.RemoveAll();
   packlistcnt = 0;

   for (i=0; i<comppincnt; i++)
      delete comppinarray[i];  
   comppinarray.RemoveAll();
   comppincnt = 0;

   for (i=0; i<packpincnt; i++)
      delete packpinarray[i];  
   packpinarray.RemoveAll();
   packpincnt = 0;

   for (i=0; i<layernamecnt; i++)
      delete layernamearray[i];  
   layernamearray.RemoveAll();
   layernamecnt = 0;

   for (i=0; i<layersetcnt; i++)
      delete layersetarray[i];  
   layersetarray.RemoveAll();
   layersetcnt = 0;

   for (i=0; i<padstackcnt; i++)
      delete padstackarray[i];  
   padstackarray.RemoveAll();
   padstackcnt = 0;

   packagePinArray.RemoveAll(); // package pins are assigned and removed in do_package
   packagePinArrayCount = 0;

   drillarray.RemoveAll();
   drillcnt = 0;
}

// Case 1761 has a data file that does not work with regular fgets.
// It does not have standard newline chars.
char *local_fgets(char *buf, int cnt, FILE *fp)
{
   int i = 0;
   buf[0] = '\0';
   if (fp && !feof(fp))
   {
      char c;
      while ((c = fgetc(fp))!= EOF && i < (cnt - 1))
      {
         if (c == 13)
            c = '\n'; // convert linefeed to standard newline
         buf[i++] = c;
         buf[i] = '\0';
         if (c == '\n')
            return buf;
      }
   }
   return NULL;
}



/******************************************************************************
* ReadFabmasterNail
*/
void ReadFabmasterNail(const char *f, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits )
{
   file = NULL;
   doc = Doc;
   display_error = 0;
   ifp_line = 0;
   PageUnits = pageUnits;
   unitsfactor = Units_Factor(UNIT_INCHES, PageUnits);

   nailarray.SetSize(100, 100);
   nailcnt = 0;

   naildefarray.SetSize(100, 100);
   naildefcnt = 0;

   attheight = 0.07;
   attwidth = 0.05;
   end_of_file = FALSE;

   // this can not be rt (because of LB terminator)
   if ((ifp = fopen(f, "rt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", f);
      ErrorMessage(t, "Error");
      return;
   }

   CString fatfLogFile = GetLogfilePath(FATFERR);
   if ((ferr = fopen(fatfLogFile, "wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", fatfLogFile);
      ErrorMessage(t, "Error");
      return;
   }

   /* initialize scanning parameters */
   CString settingsFile( getApp().getImportSettingsFilePath("fabascii.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nFABMaster Nail Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_fabasciisettings(settingsFile);

   // make it the first layer
   Graph_Level("0", "", 1);

   rewind(ifp);

   char line[255];
   char *lp;
   int   start_found = FALSE;

   // get header
   while (local_fgets(line, 255, ifp))
   {
      if ((ifp_line % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
      {
         fileReadProgress->updateProgress();
      }

      ++ifp_line;

      if ((lp = get_string(line," \t\n")) == NULL)
         continue;
      if (!STRICMP(lp, "Nail"))
      {
         //Nail         X         Y   Type Grid T/B  Net   Net Name   Virtual Pin/Via
         start_found = TRUE;
         continue;
      }
      
      if (start_found)
      {
         FABNail c;
         c.nailname = lp; // $nail

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // x
         c.x = cnv_unit(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // y
         c.y = cnv_unit(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // type
         c.type = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // grid
         c.grid = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // t/b
         if (!STRCMPI(lp,"(B)"))
            c.bottom = TRUE;
         else if (!STRCMPI(lp,"(T)"))
            c.bottom = FALSE;
         else
            c.bottom = default_top?FALSE:TRUE;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // #Netnr
         c.netnr = atoi(&lp[1]);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // Net Name
         c.netname = lp;

         // this can be virtual or pins ????
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;   // Pin or Virtual
         if (STRCMPI(lp, "VIA") && STRCMPI(lp, "PIN") && STRCMPI(lp, "PAD"))
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;   // Pin
         
         if (!STRCMPI(lp, "VIA"))
         {
            c.via = TRUE;
            c.comp = "";
            c.pin = "";
         }
         else
         {
            // PIN and PAD, same thing
            c.via = FALSE;
            if ((lp = get_string(NULL, " .\t\n")) == NULL)
               continue;   // Via
            c.comp = lp;

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;   // Via
            c.pin = lp;
         }

         CString probedefname = get_probename(c.type);

         FABNail *n = new FABNail;
         n->nailname = c.nailname;
         n->x = c.x;
         n->y = c.y;
         n->bottom = c.bottom;
         n->type = c.type;
         n->grid = c.grid;
         n->netname = c.netname;
         n->netnr = c.netnr;
         n->via = c.via;
         n->comp = c.comp;
         n->pin = c.pin;   
         n->probedef = probedefname;
         n->pinxyfound = FALSE;
         n->targetPin = NULL;
         n->targetVia = NULL;

         nailarray.SetAtGrow(nailcnt, n);  
         nailcnt++;
      }
      else
      {
         // find units
         CString  lastlp = lp;
         while (lp = get_string(NULL, " \t\n"))
         {
            if (!STRCMPI(lp, "units"))
            {
               if (!lastlp.CompareNoCase("INCH"))
                  unitsfactor = Units_Factor(UNIT_INCHES, PageUnits);
               else if (!lastlp.CompareNoCase("MM"))
                  unitsfactor = Units_Factor(UNIT_MM, PageUnits);
               else
               {
                  fprintf(ferr, "Unknown Units [%s]\n", lastlp);
                  display_error++;
               }
            }
            lastlp = lp;
         }
      }
   }
   fclose(ifp);

   // here now merge to existing file or place by itself

   // here find a file->getBlockType() which is marked as the blockTypePcb
   // the return fill be a NULL file, because this only updates
   // an existing file and does not create a new one.
   cur_filenum = 0;
   int   existing_file = FALSE;

   // write file blocks as exploded
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      file = doc->getFileList().GetNext(pos);
      if (file->getBlockType() == blockTypePcb)
      {
         cur_filenum = file->getFileNumber();
         doc->PrepareAddEntity(file);

         doc->CalcBlockExtents(file->getBlock());
         generate_PINLOC(doc, file, 0);      // this function generates the PINLOC argument for all pins.

         existing_file = TRUE;
         break;
      }
   }

   if (cur_filenum == 0)
   {
      // place by itself
      file = Graph_File_Start(f, fileTypeFabNail);
      file->setBlockType(blockTypePcb);
      file->getBlock()->setBlockType(file->getBlockType());
      cur_filenum = file->getFileNumber();
   }

   if (existing_file)
   {
      // check matching netnames and comp/pins
      int err = fabnail_netcheck(file);

      // calculate probe offset
      fabnail_calcoffset(file);
   }

   // Link via entities to nails
   fabnail_associatevias(file);

   // Create/save probes and access points
   fabnail_storenails(file);

	int i=0;
   for (i=0; i<nailcnt; i++)
      delete nailarray[i];  
   nailarray.RemoveAll();
   nailcnt = 0;

   for (i=0; i<naildefcnt; i++)
      delete naildefarray[i];  
   naildefarray.RemoveAll();
   naildefcnt = 0;

    fclose(ferr);

   if (display_error)
      Logreader(fatfLogFile);
}

/******************************************************************************
* load_fatfsettings
*/
static int load_fatfsettings(CString settingsFileName)
{
   CInFile inFile;

   if (!inFile.open(settingsFileName))
   {
      CString tmp;
      tmp.Format("File [%s] not found", settingsFileName);
      ErrorMessage(tmp, "FATF Settings", MB_OK | MB_ICONHAND);

      return 0;
   }

   while (inFile.getNextCommandLine())
   {
      if (inFile.isCommand(".ATTRIBMAP",3))
      {
         CString a1 = inFile.getParam(1).MakeUpper();
         CString a2 = inFile.getParam(2).MakeUpper();

         UAttrmap* c = new UAttrmap;
         attrmaparray.SetAtGrow(attrmapcnt++, c);  
         c->uni_name = a1;
         c->cc_name  = a2;
      }
      else if (inFile.isCommand(".ComponentSMDrule",2))
      {
         ComponentSMDrule = inFile.getIntParam(1);
      }
      else if (inFile.isCommand(".UsePinNum",2))
      {
         inFile.parseYesNoParam(1,UsePinNum,false);
      }
      else if (inFile.isCommand(".IgnoreNetsAndTraces",2))
      {
         inFile.parseYesNoParam(1,IgnoreNetsAndTraces,false);
      }
      else if (inFile.isCommand(".IgnoreVias",2))
      {
         inFile.parseYesNoParam(1,IgnoreVias,false);
      }
      else if (inFile.isCommand(".MakeIntoTestpoint",2))  // need to process after geometry name fixup
      {
         getFatfReader().addTestPointExpression(inFile.getParam(1));
      }
      else if (inFile.isCommand(".MakeIntoFiducial",2))  // need to process after geometry name fixup
      {
         getFatfReader().addFiducialExpression(inFile.getParam(1));
      }
      else if (inFile.isCommand(".MakeIntoTooling",2))  // need to process after geometry name fixup
      {
         getFatfReader().addToolingExpression(inFile.getParam(1));
      }
   }

   return 1;
}

/******************************************************************************
* load_fabasciisettings
*/
static int load_fabasciisettings(CString settingsFileName)
{
   // reset settings info
   default_top = true;

   FILE *fSet = fopen(settingsFileName, "rt");
   if (!fSet)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", settingsFileName);
      ErrorMessage(tmp, "FABASCII Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   char line[255];
   while (fgets(line, 255, fSet))
   {
      char *tok;
      if ((tok = get_string(line, " \t\n")) == NULL)
         continue;

      if (tok[0] == '.')
      {
         // .NAILTYPE   1  "PROBE_50"  
         if (!STRCMPI(tok, ".NAILTYPE"))
         {
            if ((tok = get_string(NULL," \t\n")) == NULL)
               continue;
            int type = atoi(tok);
            if ((tok = get_string(NULL," \t\n")) == NULL)
               continue;
            CString probename = tok;

            FABNaildef *n = new FABNaildef;
            n->type = type;
            n->probename = probename;
            naildefarray.SetAtGrow(naildefcnt++, n);  
         }
         // .NAILSURFACE   T/B
         else if (!STRCMPI(tok, ".NAILSURFACE"))
         {
            if ((tok = get_string(NULL," \t\n")) == NULL)
               continue;
            default_top = STRICMP(tok, "b")?true:false;
         }
      }
   }

   fclose(fSet);

   return 1;
}

/******************************************************************************
* cnv_rot
   skip to a ; on the end of a line

   fabmaster has clockwise rotation

*/
static double cnv_rot(double rot, int mirror)
{
   rot = -rot; // convert clockwise to couter-clockwise

   if (mirror)
      rot = -rot;

   while (rot < 0)
      rot += 360;

   while (rot >= 360)
      rot -= 360;

   return rot;
}

/******************************************************************************
* skip_endofline
*  - skip to a ; on the end of a line
*/
static int skip_endofline()
{
   cur_line[0] = '\0';
   cur_new = TRUE;
   Push_tok = FALSE;

   return 1;
}

/******************************************************************************
* skip_endofrecord
*  - skip to a ; on the end of a line
*/
static int skip_endofrecord()
{
   while (TRUE)
   {
      if (!get_tok())
         return p_error();   

      if (!STRCMPI(token, ";"))
         break;

      // case dts0100417492, do not skip past end of section
      if (!STRCMPI(token, ":EOD"))
      {
         push_tok(); // put it back, needed to end a loop higher up
         break;
      }
   }

   return 1;
}

/******************************************************************************
* skip_endofsection
*  - skip to a :eod
*/
static int skip_endofsection()
{
   while (TRUE)
   {
      if (!get_tok())
         return p_error(); 
      
      if (!STRCMPI(token, ":EOD"))
         break;

      skip_endofline();
   }

   return 1;
}

/******************************************************************************
* make_dummypads
*/
static int make_dummypads(int page_unit)
{
   int err;
   Graph_Aperture("DUMMYPADSHAPE", T_TARGET, 0.03 * Units_Factor(UNIT_INCHES, page_unit), 
         0.02 * Units_Factor(UNIT_INCHES, page_unit), 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);

   CString padname;
   int layernum;
   BlockStruct *block;
   LayerStruct *layer;

   if (TRUE)   // all
   {
      layernum = Graph_Level("PAD_THRU", "", 0);
      layer = doc->FindLayer(layernum);
      layer->setLayerType(LAYTYPE_PAD_ALL);

      padname.Format("%s_T", DUMMYPADSTACK);
      block = Graph_Block_On(GBO_APPEND, padname, -1, 0);
      block->setBlockType(BLOCKTYPE_PADSTACK);
      Graph_Block_Reference("DUMMYPADSHAPE", NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
      Graph_Block_Off();
   }

   if (TRUE)   // top
   {
      layernum = Graph_Level("PAD_SMD_BOT","",0);
      layer = doc->FindLayer(layernum);
      layer->setLayerType(LAYTYPE_PAD_BOTTOM);

      layernum = Graph_Level("PAD_SMD_TOP","",0);
      layer = doc->FindLayer(layernum);
      layer->setLayerType(LAYTYPE_PAD_TOP);

      Graph_Level_Mirror("PAD_SMD_TOP", "PAD_SMD_BOT", "");

      padname.Format("%s_S", DUMMYPADSTACK);
      block = Graph_Block_On(GBO_APPEND, padname, -1, 0);
      block->setBlockType(BLOCKTYPE_PADSTACK);
      Graph_Block_Reference("DUMMYPADSHAPE", NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layernum, TRUE);
      Graph_Block_Off();
   }
   return 1;
}

/******************************************************************************
* tst_fabmaster_format
*/
static int tst_fabmaster_format()
{
   while (TRUE)
   {
      if (!get_line(cur_line,MAX_LINE))
         return(FALSE);
      if (strstr(cur_line, ":FABMASTER"))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* get_attrmap
*/
static const char* get_attrmap(const char *a)
{
   for (int i=0; i<attrmapcnt; i++)
   {
      if (attrmaparray[i]->uni_name.CompareNoCase(a) == 0)
         return attrmaparray[i]->cc_name.GetBuffer(0);
   }

   return a;
}

/******************************************************************************
//* get_layertype
*/
//static int get_layertype(const char *l)
//{
//
//   for (int i=0; i<MAX_LAYTYPE; i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return   LAYTYPE_UNKNOWN; 
//}

/****************************************************************************/
/*
   Call function associated with next token.
   The token must be enclosed by brackets ( {} or [] ).
   Tokens enclosed by {} are searched for on the local
   token list.  Tokens enclosed by [] are searched for
   in the global token list.
*/
int go_command(List *tok_lst, int lst_size)
{
   if (!get_tok())
      return p_error();

   int res = 0;
   int brk = tok_search(brk_lst, SIZ_BRK_LST);

   if (brk == BRK_SQUIGLY)
   {
      if (get_tok()) 
      {
         brk = tok_search(brk_lst, SIZ_BRK_LST);

         if (brk == BRK_B_SQUIGLY)
         {
            push_tok();
            return 1;
         }
         else
         {
            int i = tok_search(tok_lst, lst_size);
            if (i >= 0)
            {
               res = (*tok_lst[i].function)();
               if (res < 0)
               {
                  CString ls = tok_lst[i].token;
                  int w = 0;
               }
            }
            else
            {
               skip_endofsection();
               // return p_error();
            }
         }
      }
   }
   else if (brk == BRK_B_SQUIGLY)
   {
      push_tok();
      return 1;
   }
   else
   {
      int i = tok_search(tok_lst, lst_size);
      if (i >= 0)
      {
         res = (*tok_lst[i].function)();
         if (res == -2) // logical end of file
            return -2;
      }
      else
      {
         // unknown token - just skip this line
         fprintf(ferr, "Unsupported Token [%s] at %ld -> skipped\n", token, ifp_line);
         display_error++;
         return 1;
      }
   }

   if (res == -1)
   {
      // big error occured.
      CString t = "";
      t.Format("FATF Read error : Token [%s] at %ld", token, ifp_line);
      ErrorMessage(t, "Error");
      return -1;
   }

   return res;
}                                   


/****************************************************************************/
/*
   Loop through a section.
*/
static int loop_command(List *list,int size)
{
   int      repeat = TRUE;

   while (repeat)
   {
      if (go_command(list,size) < 0)   
         return -1;

      if (!get_tok())
      {
         if (end_of_file)     // some fabmaster files end without :eof
            return -2;
         return p_error();
      }
      if (tok_search(brk_lst,SIZ_BRK_LST) == BRK_B_SQUIGLY)
         repeat = FALSE;
      else
         push_tok();
   }
   return 1;
}


/****************************************************************************/
/*
   NULL function.
   Skips over any tokens to next }] endpoint.
*/
static int fnull()
{
   int      brk_count = 0;
   
   fprintf(ferr,"Unsupported Token [%s] at %ld -> skipped\n",token,ifp_line);
   display_error++;
   while (TRUE)
   {
      if (get_tok())
      {
         if (token_name)   continue;   // do not evaluate a name []{} token.
         switch (tok_search(brk_lst,SIZ_BRK_LST))
         {
            case BRK_SQUIGLY:
               ++brk_count;
            break;
            case BRK_B_SQUIGLY:
               if (--brk_count == 0)
               {
                  return 1;
               }
            break;
            default:
            {

            }
            break;
         }
      }
      else
         return p_error();
   }
   return 1;
}

/****************************************************************************/
/*
   fabmaster start.
*/
static int get_xybulge(double *x, double *y, double *bulge)
{
   if (!get_tok())   return p_error();    // get (
   if (!STRCMPI(token, "("))
   {
      if (!get_tok())   return p_error();    // x
      *x = cnv_unit(token);
      if (!get_tok())   return p_error();    // y
      *y = cnv_unit(token);
      if (!get_tok())   return p_error();    // bulge
      *bulge = atof(token);
   }
   else
   {
      fprintf(ferr,"( expected in Line Read at %ld\n", ifp_line);
      return 0;
   }
   if (!get_tok())   return p_error();    // )

   return 1;
}

/****************************************************************************/
/*
   fabmaster start.
*/
static int get_xy(double *x, double *y)
{
   if (!get_tok())   return p_error();    // x
   *x = cnv_unit(token);
   if (!get_tok())   return p_error();    // y
   *y = cnv_unit(token);

   return 1;
}

/******************************************************************************
* write_poly
*/
static DataStruct *write_poly(const char *layername, double width, int fill, double xoffset, double yoffset)
{
   if (polycnt == 0)
      return 0;

   int layer = Graph_Level(layername, "", 0);

   int err;
   int widthindex = 0;
   if (width > 0)
      widthindex = Graph_Aperture("", T_ROUND, width , 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   Point2 p1 = poly_l.GetAt(0);
   Point2 p2 = poly_l.GetAt(polycnt-1);
   int close = (p1.x == p2.x && p1.y == p2.y);

   DataStruct *data = Graph_PolyStruct(layer, 0, 0);
   Graph_Poly(NULL, widthindex, fill, 0, close);

   // here write lines.
   for (int i=0; i<polycnt; i++)
   {
      p1 = poly_l.GetAt(i);
      Graph_Vertex(p1.x - xoffset, p1.y - yoffset, p1.bulge);
   }

   polycnt = 0;
   return data;
}

/****************************************************************************/
/*
*/
static int do_contour()
{
   Point2   p;

   polycnt = 0;
   if (!get_tok())   return p_error();    // get (

   while (!STRCMPI(token,"("))
   {
      if (get_xybulge(&p.x, &p.y, &p.bulge))
      {
         poly_l.SetAtGrow(polycnt,p);
         polycnt++;
      }
      if (!get_tok())   return p_error();    // 
      push_tok();
   }
   Push_tok = FALSE;
   if (!get_tok())   return p_error();    // get ;
   
   return polycnt;
}

/****************************************************************************/
/*
*/
static int do_track(double *width)
{
   Point2   p;

   polycnt = 0;
   if (!get_tok())   return p_error();    // get (
   if (STRCMPI(token, "("))
   {
      *width = cnv_unit(token);
      if (!get_tok())   return p_error();
   }
// if (!get_tok())   return p_error(); 

   while (!STRCMPI(token,"("))
   {
      if (get_xy(&p.x, &p.y))
      {
         p.bulge = 0;
         poly_l.SetAtGrow(polycnt,p);
         polycnt++;
      }
      if (!get_tok())   return p_error();    // close 
      if (!get_tok())   return p_error();    // 
   }
   
   return polycnt;
}

/****************************************************************************/
/*
*/
static int do_package_label()
{
   
   if (!get_tok())   return p_error();    // (
   if (!get_tok())   return p_error();    // font_id
   if (!get_tok())   return p_error();    // x
   if (!get_tok())   return p_error();    // y
   if (!get_tok())   return p_error();    // justify
   if (!get_tok())   return p_error();    // rotation
   if (!get_tok())   return p_error();    // )

   // now see if next is the next pins
   if (!get_tok())   return p_error();    // get ( or ) of pins

   return 1;
}

/****************************************************************************/
/*
*/
static int symbol_text(int *fontid, double *x, double *y, CString *just, 
                       int *rot, int *mirror, CString *prosa)
{
   if (!get_tok())   return p_error();    // (

   if (!get_tok())   return p_error();    // 
   *fontid = atoi(token);

   if (!get_tok())   return p_error();    // 
   *x = cnv_unit(token);

   if (!get_tok())   return p_error();    // 
   *y = cnv_unit(token);

   if (!get_tok())   return p_error();    // 
   *just = token;

   if (!get_tok())   return p_error();    // 
   *rot = atoi(token);

   *mirror = FALSE;
   if (!get_tok())   return p_error();    // 
   if (strlen(token) && token[0] == 'M')
      *mirror = TRUE;

   if (!get_tok())   return p_error();    // 
   *prosa = token;

   if (!get_tok())   return p_error();    // )

   return 1;
}

/****************************************************************************/
/*
*/
static int symbol_block(double *x1, double *y1, double *x2, double *y2)
{
   if (!get_tok())   return p_error();    // (

   if (!get_tok())   return p_error();    // xlow
   *x1 = cnv_unit(token);
   if (!get_tok())   return p_error();    // ylow
   *y1 = cnv_unit(token);
   if (!get_tok())   return p_error();    // xhigh
   *x2 = cnv_unit(token);
   if (!get_tok())   return p_error();    // yhigh
   *y2 = cnv_unit(token);

   if (!get_tok())   return p_error();    // )

   return 1;
}

/****************************************************************************/
/*
*/
static int symbol_circle(double *rmax, double *rmin, double *x, double *y)
{
   if (!get_tok())   return p_error();    // (

   if (!get_tok())   return p_error();    // rmax ??
   *rmax = cnv_unit(token);
   if (!get_tok())   return p_error();    // rmin
   *rmin = cnv_unit(token);
   if (!get_tok())   return p_error();    // x
   *x = cnv_unit(token);
   if (!get_tok())   return p_error();    // y
   *y = cnv_unit(token);

   if (!get_tok())   return p_error();    // )

   return 1;
}

/****************************************************************************/
/*
*/
static int symbol_round(double *diam, double *x, double *y)
{
   if (!get_tok())   return p_error();    // (

   if (!get_tok())   return p_error();    // 
   *diam = cnv_unit(token);
   if (!get_tok())   return p_error();    // 
   *x = cnv_unit(token);
   if (!get_tok())   return p_error();    // 
   *y = cnv_unit(token);

   if (!get_tok())   return p_error();    // )

   return 1;
}

/******************************************************************************
* symbol_track
*/
static int symbol_track(double *width)
{
   if (!get_tok())
      return p_error();    // width
   do_track(width);

   return 1;
}

/******************************************************************************
* symbol_line
*/
static int symbol_line(double *width)
{
   if (!get_tok())
      return p_error();    // width
   do_track(width);

   return 1;
}

/******************************************************************************
* symbol_polygon
*/
static int symbol_polygon(double *width)
{
   if (!get_tok())
      return p_error();    
   do_track(width);

   return 1;
}

/****************************************************************************/
/*
   (1,"",-12300,0,T)
*/
static int package_pin(int *pinid, double *x, double *y, CString *pinname, char *pintype)
{
   if (!get_tok())   return p_error();    // pinid
   *pinid = atoi(token);

   if (!get_tok())   return p_error();    // pinname
   *pinname = token;

   if (!get_tok())   return p_error();    // x
   *x = cnv_unit(token);
   if (!get_tok())   return p_error();    // y
   *y = cnv_unit(token);
   if (!get_tok())   return p_error();    // type
   *pintype = token[0];
   
   return 1;
}

/****************************************************************************/
/*
*/
static int get_fontptr(int index)
{

   for (int i=0;i<fontcnt;i++)
   {

      if (fontarray[i]->index == index)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_netptr(int index)
{

   for (int i=0;i<netnamecnt;i++)
   {
      if (netnamearray[i]->index == index)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_complistptr(int index)
{
   for (int i=0;i<compArrayCount;i++)
   {
      if (compArray[i]->index == index)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_packlistptr(const char *packname)
{

   for (int i=0;i<packlistcnt;i++)
   {

      FATFPacklist *packList = packlistarray[i];

      if (!STRCMPI(packList->name, packname))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_packpinptr(int packindex, int pinindex)
{

   for (int i=0;i<packpincnt;i++)
   {
      if (packpinarray[i]->packindex == packindex && packpinarray[i]->pinindex == pinindex)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
FATFLayername *getLayerFromMirrorIndex(int mirrorIndex)
{

   for (int i=0;i<layernamecnt;i++)
   {
      if (layernamearray[i]->index == mirrorIndex)
         return layernamearray[i];
   }

   return NULL;
}

/****************************************************************************/
/*
*/
static int get_layerptr(int index)
{

   for (int i=0;i<layernamecnt;i++)
   {
      if (layernamearray[i]->index == index)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_padstackptr(int index)
{

   for (int i=0;i<padstackcnt;i++)
   {

      if (padstackarray[i]->index == index)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_layersetptr(int index)
{
   for (int i=0;i<layersetcnt;i++)
   {

      if (layersetarray[i]->index == index)
         return i;
   }

   return -1;
}

/******************************************************************************
* get_packagepinptr
*/
static int get_packagepinptr(int pinID)
{
   for (int i=0; i<packagePinArrayCount; i++)
   {
      FATFPackagePin *packagePin = packagePinArray[i];

      if (packagePin->pinID == pinID)
         return i;
   }

   return -1;
}

/******************************************************************************
* get_blockshape
*/
static int get_blockshape(double x1, double y1, double x2, double y2, double insertX, double insertY)
{
   // figure out rectangle or complex
   double offx = x1 + (x2-x1)/2 - insertX;
   double offy = y1 + (y2-y1)/2 - insertY;
   double sizeA = fabs(x2 - x1);
   double sizeB = fabs(y2 - y1);
   double rotation = 0; 

   int form = T_RECTANGLE;
   if (!(fabs(offx) > SMALLNUMBER && fabs(offy) > SMALLNUMBER))
   {
      if (fabs(sizeA - sizeB) < SMALLNUMBER)
      {
         // square
         form = T_SQUARE;
         sizeB = 0;
      }
      else if (sizeA > sizeB)
      {
         // rectangle horiz
         form = T_RECTANGLE;
         /*double tmp = offx;
         offx = offy;
         offy = tmp;
         rotation = 90; //this was removed to fix case# 274/249.*/
      }
   }

   for (int i=0; i<aperturecnt; i++)
   {
      if (aperturearray[i]->form == form &&
          aperturearray[i]->getSizeA() == sizeA &&
          aperturearray[i]->getSizeB() == sizeB &&
          aperturearray[i]->offx == offx &&
          aperturearray[i]->offy == offy &&
          aperturearray[i]->rot == rotation)
      {
          return i;
      }
   }

   CString name = "";
   name.Format("PADSHAPE_%d", aperturecnt);
   Graph_Aperture(name, form, sizeA, sizeB, offx, offy, DegToRad(rotation), 0, BL_APERTURE, TRUE, NULL);

   FATFAperture *a = new FATFAperture;
   a->form = form;
   a->setSizeA(sizeA);
   a->setSizeB(sizeB);
   a->offx = offx;
   a->offy = offy;
   a->rot = rotation;
   aperturearray.SetAtGrow(aperturecnt++, a);  

   return aperturecnt-1;
}

/******************************************************************************
* get_trackshape
*/
static int get_trackshape(double width, double x1, double y1, double x2, double y2, double insertX, double insertY)
{
   // figure out rectangle or complex
   double offx = x1 + (x2-x1)/2 - insertX;
   double offy = y1 + (y2-y1)/2 - insertY;

   double sizeA = width + sqrt(((y2 - y1) * (y2 - y1)) + ((x2 - x1) * (x2 - x1)));
   double sizeB = width;

   double rotation = RadToDeg(atan2(y2 - y1, x2 - x1));

   // If the rotation is -90 it is because the second point was given before the first point
   // and the shape is an vertical OBLONG, so just rotation 90 instead of -90
   if (rotation == -90)
      rotation = 90;

   int form = T_OBLONG;

   if (!(fabs(offx) > SMALLNUMBER && fabs(offy) > SMALLNUMBER))
   {
      if (fabs(sizeA - sizeB) < SMALLNUMBER)
      {
         // square
         form = T_ROUND;
         sizeB = 0;
      }
   }

   if (sizeA > sizeB)
   {
      double tmp = offx;
      offx = offy;
      offy = tmp;
   }

   for (int i=0;i<aperturecnt;i++)
   {
      if (aperturearray[i]->form == form &&
          aperturearray[i]->getSizeA() == sizeA &&
          aperturearray[i]->getSizeB() == sizeB &&
          aperturearray[i]->offx == offx &&
          aperturearray[i]->offy == offy &&
          aperturearray[i]->rot == rotation)
      {
          return i;
      }
   }

   CString name = "";
   name.Format("PADSHAPE_%d", aperturecnt);
   int err;
   Graph_Aperture(name, form, sizeA, sizeB, offx, offy, DegToRad(rotation), 0, BL_APERTURE, TRUE, &err);

   FATFAperture *a = new FATFAperture;
   a->form = form;
   a->setSizeA(sizeA);
   a->setSizeB(sizeB);
   a->offx = offx;
   a->offy = offy;
   a->rot = rotation;
   aperturearray.SetAtGrow(aperturecnt++, a);  

   return aperturecnt-1;
}

/******************************************************************************
* get_roundshape
*/
static int get_roundshape(double diam)
{
   // figure out rectangle or complex

   for (int i=0;i<aperturecnt;i++)
   {
      if (aperturearray[i]->form == T_ROUND &&
          aperturearray[i]->getSizeA() == diam &&
          aperturearray[i]->getSizeB() == 0 &&
          aperturearray[i]->offx == 0 &&
          aperturearray[i]->offy == 0 &&
          aperturearray[i]->rot == 0)
      {
          return i;
      }
   }

   CString  name;
   name.Format("PADSHAPE_%d",aperturecnt);
   //BlockStruct  *curblock = Graph_Block_On(GBO_APPEND,name,-1,0);
   //curblock->blocktype = BLOCKTYPE_PADSHAPE;
   int   err;
   Graph_Aperture(name, T_ROUND, diam, 0.0, 0.0, 0.0, 0.0, 0, 
            BL_APERTURE, TRUE, &err);

   FATFAperture *a = new FATFAperture;
   a->form = T_ROUND;
   a->setSizeA(diam);
   a->setSizeB(0);
   a->offx = 0;
   a->offy = 0;
   a->rot = 0;
   aperturearray.SetAtGrow(aperturecnt,a);  
   aperturecnt++;

   return aperturecnt-1;
}


/******************************************************************************
* graphic_layer
   if packagesection the id is pinid for id > 0
   if netsection the id is netid
*/
static int graphic_layer(int id, int packagesection, const char *packagename)
{
   if (!get_tok()) // layerid
      return p_error();
   int layerID = atoi(token);

   CString layername;
   layername.Format("L%d", layerID);

   if (!get_tok()) // (
      return p_error();

   while (STRCMPI(token, ")")) // while not end bracket
   {
      if (!get_tok()) // TRACK, BLOCK etc...
         return p_error();

      if (!STRCMPI(token, "BLOCK"))
      {
         double x1, y1, x2, y2;
         symbol_block(&x1, &y1, &x2, &y2);

         if (packagesection && id) // this is a pingraphic
         {
            int pinIndex = get_packagepinptr(id);
            FATFPackagePin *packagePin = packagePinArray[pinIndex];
            if (packagePin->padstackName.IsEmpty())
               packagePin->padstackName.Format("$_%s_%d", packagename, id);

            CString padName, padstackName;
            padName.Format("PADSHAPE_%d", get_blockshape(x1, y1, x2, y2, packagePin->x, packagePin->y));

            BlockStruct *padstack = Graph_Block_On(GBO_APPEND, packagePin->padstackName, -1, 0);
            padstack->setBlockType(BLOCKTYPE_PADSTACK);          
            Graph_Block_Reference(padName, NULL, -1, 0, 0, 0, 0, 1, Graph_Level(layername, "", 0), TRUE);
            Graph_Block_Off();
         }
         else
         {
            Point2 p;
            p.bulge = 0;

            p.x = x1;
            p.y = y1;
            poly_l.SetAtGrow(polycnt++, p);

            p.x = x2;
            p.y = y1;
            poly_l.SetAtGrow(polycnt++, p);

            p.x = x2;
            p.y = y2;
            poly_l.SetAtGrow(polycnt++, p);
            
            p.x = x1;
            p.y = y2;
            poly_l.SetAtGrow(polycnt++, p);
            
            p.x = x1;
            p.y = y1;
            poly_l.SetAtGrow(polycnt++, p);
            
            write_poly(layername, 0.0, TRUE, 0.0, 0.0);
         }
      }
      else if (!STRCMPI(token, "ROUND"))
      {
         double diam, x, y;
         int pinIndex = -1;

         symbol_round(&diam, &x, &y);

         if (packagesection && id) // this is a pingraphic
         {
            pinIndex = get_packagepinptr(id);
            FATFPackagePin *packagePin = packagePinArray[pinIndex];

            double x1 = x - packagePin->x;
            double y1 = y - packagePin->y;

            if (!(fabs(x1) < SMALLNUMBER && fabs(y1) < SMALLNUMBER))
               pinIndex = -1;
         }

         if (pinIndex > -1)
         {
            FATFPackagePin *packagePin = packagePinArray[pinIndex];
            if (packagePin->padstackName.IsEmpty())
               packagePin->padstackName.Format("$_%s_%d", packagename, id);

            packagePin->x = x;
            packagePin->y = y;

            CString padname;
            padname.Format("PADSHAPE_%d", get_roundshape(diam));

            BlockStruct *block = Graph_Block_On(GBO_APPEND, packagePin->padstackName, -1, 0);
            block->setBlockType(BLOCKTYPE_PADSTACK);       
            Graph_Block_Reference(padname, NULL, -1, 0.0, 0.0, DegToRad(0), 0 , 1.0,Graph_Level(layername,"",0), TRUE);
            Graph_Block_Off();
         }
         else
         {
            // do round symbol
            int layer = Graph_Level(layername, "", 0);
            Graph_Circle(layer, x, y, diam/2, 0L, 0 , FALSE, TRUE);  // filled
         }
      }
      else if (!STRCMPI(token, "CIRCLE"))
      {
         double rmax, rmin, x, y;
         symbol_circle(&rmax, &rmin, &x, &y);

         int layer = Graph_Level(layername,"",0);
         Graph_Circle(layer, x, y, (rmax - rmin)/2, 0L, 0 , FALSE, FALSE); // not filled
      }
      else if (!STRCMPI(token, "TRACK"))
      {
         polycnt = 0;
         double width;
         symbol_track(&width);

         if (packagesection && id && polycnt == 2)
         {
            int pinIndex = get_packagepinptr(id);
            FATFPackagePin *packagePin = packagePinArray[pinIndex];
            if (packagePin->padstackName.IsEmpty())
               packagePin->padstackName.Format("$_%s_%d", packagename, id);

            Point2 p1 = poly_l.GetAt(0);
            Point2 p2 = poly_l.GetAt(polycnt-1);

            CString padname;
            padname.Format("PADSHAPE_%d", get_trackshape(width, p1.x, p1.y, p2.x, p2.y, packagePin->x, packagePin->y));

            BlockStruct *block = Graph_Block_On(GBO_APPEND, packagePin->padstackName, -1, 0);
            block->setBlockType(BLOCKTYPE_PADSTACK);
            Graph_Block_Reference(padname, NULL, -1, 0.0, 0.0, DegToRad(0), 0 , 1.0,Graph_Level(layername,"",0), TRUE);
            Graph_Block_Off();
         }
         else
         {
            DataStruct *data = write_poly(layername, width, 0, 0.0, 0.0);
            int netptr = get_netptr(id);
            if (netptr > -1)
            {
               CString  netname = netnamearray[netptr]->name;

               data->setGraphicClass(GR_CLASS_ETCH);
               doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, (void *)netname.GetBuffer(0), attributeUpdateOverwrite, NULL); 
            }
         }
      }
      else if (!STRCMPI(token, "LINE"))   // looks same as track
      {
         polycnt = 0;
         double width;
         symbol_line(&width);

         DataStruct *data = write_poly(layername, width, 0, 0.0, 0.0);
         if (!packagesection && id)
         {
            int netptr = get_netptr(id);
            if (netptr > -1)
            {
               CString  netname = netnamearray[netptr]->name;
               if (data)
               {
                  data->setGraphicClass(GR_CLASS_ETCH);
                  doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
                        valueTypeString, (void *)netname.GetBuffer(0), attributeUpdateOverwrite, NULL); //  
               }
            }
         }
      }
      else if (!STRCMPI(token, "POLYGON"))
      {
         double width = 0;
         polycnt = 0;
         symbol_polygon(&width);

         if (packagesection && id)
         {
            int pinIndex = get_packagepinptr(id);
            FATFPackagePin *packagePin = packagePinArray[pinIndex];
            if (packagePin->padstackName.IsEmpty())
               packagePin->padstackName.Format("$_%s_%d", packagename, id);

            CString padshapeName, padcomplexName;
            padshapeName.Format("PADSHAPE_%d", aperturecnt);
            padcomplexName.Format("PADCOMPLEX_%d", aperturecnt);

            int layer = Graph_Level(layername,"",0);

            BlockStruct *padshapeBlock = Graph_Block_On(GBO_APPEND, padshapeName, -1, 0);
            padshapeBlock->setBlockType(BLOCKTYPE_PADSHAPE);
            write_poly(layername, 0, TRUE, packagePin->x, packagePin->y);
            Graph_Block_Off();
            Graph_Complex(padcomplexName, 0, padshapeName, 0.0, 0.0, 0.0);

            BlockStruct *padstackBlock = Graph_Block_On(GBO_APPEND, packagePin->padstackName, -1, 0);
            padstackBlock->setBlockType(BLOCKTYPE_PADSTACK);
            Graph_Block_Reference(padcomplexName, NULL, -1, 0.0, 0.0, DegToRad(0), 0, 1.0, layer, TRUE);
            Graph_Block_Off();

            FATFAperture *a = new FATFAperture;
            a->form = T_COMPLEX;
            a->setSizeA((DbUnit)padshapeBlock->getBlockNumber());
            a->setSizeB(0);
            a->offx = 0;
            a->offy = 0;
            a->rot = 0;
            aperturearray.SetAtGrow(aperturecnt++, a);  
         }
         else
         {
            DataStruct *data = write_poly(layername, 0.0, 0, 0.0, 0.0);
            int netptr = get_netptr(id);
            if (netptr > -1)
            {
               CString  netname = netnamearray[netptr]->name;
               if (data) // write_poly can return a NULL if polycnt == 0
               {
                  data->setGraphicClass(GR_CLASS_ETCH);
                  doc->SetAttrib(&data->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
                        valueTypeString, (void *)netname.GetBuffer(0), attributeUpdateOverwrite, NULL); //  
               }
            }
         }
      }
      else if (!STRCMPI(token, "TEXT"))
      {
         double x, y;
         int fontid, rot, mirror;
         CString  just, prosa;

         symbol_text(&fontid, &x, &y, &just, &rot, &mirror, &prosa);

         prosa.TrimLeft();
         prosa.TrimRight();

         if (strlen(prosa))
         {
            int   prop = TRUE;
            int   fptr = get_fontptr(fontid);
            int   layer = Graph_Level(layername, "", 0);
   
            // case dts0100417492 - stop crash, protect from array bounds error
            double charheight = doc->convertToPageUnits(pageUnitsInches, 0.150); // default text height
            double charwidth = charheight * 0.65;                                // default width
            if (fptr >= 0 && fptr < fontcnt)
            {
               charheight = fontarray[fptr]->charheight;
               charwidth = fontarray[fptr]->charwidth;
            }
   
            DataStruct *d = Graph_Text(layer, prosa, x, y, charheight, charwidth,
                  DegToRad(cnv_rot(rot, mirror)), 0, prop, mirror, 0, FALSE, -1, 0);
         }
      }
      else
      {
         fprintf(ferr, "Unknown Layer symbol [%s] at %ld\n", token, ifp_line);
         display_error++;
      }

      // now see if next is the next layer symbols
      if (!get_tok())
         return p_error();    // get (
      push_tok();
   }
      
   if (!get_tok()) // end token of layerid
      return p_error();

   return 1;
}

/****************************************************************************/
/*
*/
static int graphic_pad(const char *packagename, int pinid)
{
   int      padid;
   double   x, y;
   CString  padstack;

   if (!get_tok())   return p_error();    // padid
   padid = atoi(token);

   CString  padname;
   int pptr = get_padstackptr(padid);

   if (pptr < 0)
      padname = "";
   else
      padname = padstackarray[pptr]->name;

   if (!get_tok())   return p_error();    // (

   while (STRCMPI(token,")")) // while not
   {
      if (!get_tok())   return p_error();    // padid

      if (get_xy(&x, &y))
      {
         // do pad
         // here find 
         // make a complex aperture, later we will optimize this

         if (pinid == 0)
         {
            // mechanical pad
            if (strlen(padname) == 0)
               padname.Format("%s_T", DUMMYPADSTACK);

            DataStruct *d = Graph_Block_Reference(padname, NULL, -1, 
                           x, y,
                           DegToRad(0), 0 , 1.0,Graph_Level("0","",1), TRUE);
            d->getInsert()->setInsertType(insertTypeMechanicalPin);
         }
         else
         {
            int pinIndex = get_packagepinptr(pinid);

            if (pinIndex < 0)
            {
               fprintf(ferr, "Pin not assigned at %ld\n", ifp_line);
               display_error++;
            }
            else if (strlen(packagePinArray[pinIndex]->padstackName))
            {
// this pin has already a pad assigned ????
               fprintf(ferr, "Pin has already a pad assigned at %ld\n", ifp_line);
               display_error++;
            }
            else
            {
               FATFPackagePin *pp = packagePinArray[pinIndex];
               if (strlen(padname) == 0)
               {
                  if (packagePinArray[pinIndex]->pinType == 'D')
                     padname.Format("%s_T", DUMMYPADSTACK);
                  else
                     padname.Format("%s_S", DUMMYPADSTACK);
               }

               pp->x = x;
               pp->y = y;
               pp->padstackName = padname;

               // Create unique pin padstack name in case there are different graphic define later
               // for different pin that use the same padstack
               BlockStruct* padstackBlock = doc->Find_Block_by_Name(padname, -1);
               if (padstackBlock != NULL)
               {
                  CString uniquePadstackName;
                  uniquePadstackName.Format("$_%s_%d", packagename, pp->pinID);

                  BlockStruct* uniquePadstack = Graph_Block_On(GBO_APPEND, uniquePadstackName, -1, 0, padstackBlock->getBlockType());
                  Graph_Block_Copy(padstackBlock, 0.0, 0.0, 0.0, 0, 1.0, -1, TRUE, FALSE);
                  Graph_Block_Off();
                  pp->padstackName = uniquePadstackName;
               }           
            }
         }  // mechanical pad
      }
      if (!get_tok())   return p_error();    // padid

      if (!get_tok())   return p_error();    // 
      push_tok();
   }

   return 1;
}

/******************************************************************************
* do_package_pins
*/
static int do_package_pins(const char *packagename)
{
   int      pinid;
   double   x, y;
   CString  pinname;
   char     pintype;

   if (!get_tok())
      return p_error();    // get (

   while (!STRCMPI(token, "(")) // if it is closed, it is the end of pins
   {
      package_pin(&pinid, &x, &y, &pinname, &pintype);

      if (!strlen(pinname) || UsePinNum)
         pinname.Format("%d", pinid);

      FATFPackagePin *pp = new FATFPackagePin;
      pp->pinID = pinid;
      pp->x = x;
      pp->y = y;
      pp->pinName = pinname;
      pp->pinType = pintype;
      pp->padstackName = "";
      packagePinArray.SetAtGrow(packagePinArrayCount++, pp);  

      if (!get_tok())   return p_error();    // get ) from pin

      // now see if next is the next pins
      if (!get_tok())   return p_error();    // get ( or ) of pins
   }
   return 1;
}

/****************************************************************************/
/*
 (
  2,LAYER (2 (BLOCK (7850,-5950,16750,5950)   ) )
  1,LAYER (2 (BLOCK (-16750,-5950,-7850,5950) ) )
  0,LAYER (12 (
   TRACK (4,(18250,-10000),(18250,10100),(-18250,10100),(-18250,-10000),
         (18250,-10000))))
  0,LAYER (19 (
   TRACK (800,(-15700,8900),(-14900,9700)),
   TRACK (800,(-15000,-9500),(-15700,-8800)),
   TRACK (800,(17000,-9600),(-17000,-9600)),
   TRACK (800,(17000,9700),(-17000,9700)),
   TRACK (800,(-15700,8900),(-17000,8900)),
   TRACK (800,(-15700,-8800),(-17000,-8800)),
   TRACK (800,(-17000,8300),(-17000,9700)),
   TRACK (800,(-17000,-9600),(-17000,-8200)),
   TRACK (800,(17000,8300),(17000,9700)),
   TRACK (800,(17000,-9600),(17000,-8200))
   )
   )
)

*/
static int package_layers(const char *packagename, int pinid)
{
   
   if (!get_tok())
      return p_error(); // get LAYER

   if (!STRCMPI(token, "LAYER"))
   {
      if (!get_tok())
         return p_error(); // get (

      while (!STRCMPI(token, "("))
      {
         graphic_layer(pinid, 1, packagename);

         // now see if next is the next pins
         if (!get_tok())
            return p_error(); // get ( or ) of layer
         push_tok();
      }
   }
   else if (!STRCMPI(token, "PAD"))
   {
      if (!get_tok())
         return p_error(); // get (

      while (!STRCMPI(token, "("))
      {
         graphic_pad(packagename, pinid);

         // now see if next is the next pins
         if (!get_tok())
            return p_error(); // get ( or ) of layer
      }
   }
   else
   {
      // here I am lost !!
      fprintf(ferr, "Lost (%s) : LAYER / PAD keyword expected at %ld\n", token, ifp_line);
      display_error++;
      return -1;
   }

   return 1;
}

/****************************************************************************/
/*

)

*/
static int netlist_layers(int netid)
{
   
   if (!get_tok())   return p_error();    // get LAYER

   if (!STRCMPI(token, "LAYER"))
   {
      if (!get_tok())   return p_error();    // get (

      while (!STRCMPI(token,"("))
      {
         graphic_layer(netid, 0, "");

         // now see if next is the next pins
         if (!get_tok())   return p_error();    // get ( or ) of layer
      }
   }
   else
/*
   if (!STRCMPI(token, "PAD"))
   {
      if (!get_tok())   return p_error();    // get (

      while (!STRCMPI(token,"("))
      {
         graphic_pad();

         // now see if next is the next pins
         if (!get_tok())   return p_error();    // get ( or ) of layer
      }
   }
   else
*/
   {
      // here I am lost !!
      fprintf(ferr,"Lost : LAYER / PAD keyword expected at %ld\n", ifp_line);
      display_error++;
      return -1;
   }

   //push_tok();
   return 1;
}

/****************************************************************************
* do_package
   LABELS
   PINS
   LAYERS
*/
static int do_package(const char *packagename)
{
   if (!get_tok())
      return p_error(); // get (

   if (!STRCMPI(token, ")"))  
   {
      skip_endofline();
      return 1; // no contens found !
   }

   while (STRCMPI(token, ";")) // if it is not the end of the package
   {
      if (!get_tok())
         return p_error();
      if (!STRCMPI(token, ")"))
         break;
         
      if (!STRCMPI(token, "LABEL"))
         do_package_label();
      else if (!STRCMPI(token, "PINS"))
         do_package_pins(packagename);
      else
      {
         // layers
         int pinid = atoi(token);

         // here check the next !!! This funny thing is done because of a syntax problem in
         // fatf
         if (!get_tok())
            return p_error(); // get next (

         if (!STRCMPI(token, ":EOD"))  // this happens if the last record is not terminated by an ;
         {
            push_tok();
            return 0;
         }
         push_tok();

         if (!STRCMPI(token, ";"))
            break;
         package_layers(packagename, pinid);
      }
      if (!get_tok())
         return p_error(); // get next (
   }
   Push_tok = FALSE;
   skip_endofline();
   
   return 1;
}

/****************************************************************************/
/*
*/
static int get_location(double *x, double *y, double *rot, int *side)
{
   if (!get_tok())   return p_error();    // get {       
   if (STRCMPI(token,"{"))
   {
      fprintf(ferr,"{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())   return p_error();    // x   
   *x = cnv_unit(token);

   if (!get_tok())   return p_error();    // y   
   *y = cnv_unit(token);

   if (!get_tok())   return p_error();    // rot   
   *rot = atof(token) / 10;

   if (!get_tok())   return p_error();    //     
   *side = atoi(token);

   if (*side == 1)
   {
      *rot = 360 - *rot;
   }

   if (!get_tok())   return p_error();    // get }       

   return 1;
}

/****************************************************************************/
/*
*/
static int fatf_remark()
{
   int res = 1;

   skip_endofline();

   return res;
}

/****************************************************************************/
/*
*/
static int fatf_notrace()
{
   return 1;
}

/****************************************************************************/
/*
*/
static int fatf_no_pad_stack_group()
{
   return 1;
}

/****************************************************************************/
/*
*/
static int fatf_noautorotate()
{
   return 1;
}

/******************************************************************************
* fatf_fabmaster
*  - :FABMASTER FATF REV 11.1;
*/
static int fatf_fabmaster()
{
   if (!get_tok())
      return p_error();

   skip_endofline();

   return 1;
}

/******************************************************************************
* fatf_units
*  - :UNITS = 1/100000 INCH;   
*/
static int fatf_units()
{
   if (!get_tok())
      return p_error();

   if (!STRCMPI(token,"="))   
   {
      if (!get_tok())
         return p_error();    // get unit string
   }
   else if (strlen(token) && token[0] == '=')
   {
      token[0] = ' ';
   }
   else
   {
      fprintf(ferr, "UNITS = expected at %ld\n", ifp_line);
      display_error++;
      return -1;
   }


   int a, ec;
   double factor;
   if( (ec = EE_Evaluate(token, &factor, &a)) != E_OK )
   {
      ErrorMessage("Arithmetic Function Error!");
   }

   if (!get_tok())
      return p_error();
   CString unitsName = token;

   if (!STRCMPI(unitsName, "INCH"))
      unitsfactor = Units_Factor(UNIT_INCHES, PageUnits) * factor;
   else
   {
      fprintf(ferr,"Unknown UNITS [%s] found at %ld\n", unitsName, ifp_line);
      display_error++;
   }

   return 1;
}

/****************************************************************************/
/*
*/
int   fatf_board_data()
{
   int   index = 0;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      index = atoi(token); // index.
      go_command(board_data_lst,SIZ_BOARD_DATA_LST);
   }

   return 1;
}

/****************************************************************************/
/*
   1,JOB ("482~ET from Protel",,09-06-2000,09-06-2000);
*/
int   fatf_board_data_job()
{
   if (!get_tok())   return p_error();    // (
   if (!get_tok())   return p_error();    // jobname
   file->setName(token);

   skip_endofrecord();

   return 1;
}

/****************************************************************************/
/*
   2,CONTOUR ((-237500,423500,0),(-108000,423500,0));
*/
int   fatf_board_data_contour()
{
   polycnt = 0;
   do_contour();

   DataStruct *data = write_poly("BOARDOUTLINE", 0.0, 0, 0.0, 0.0);
   data->setGraphicClass(GR_CLASS_BOARDOUTLINE);
   polycnt = 0;

   return 1;
}

/****************************************************************************/
/*
   26,WORK_SPACE (-404500,438500,-499500,529600);
*/
int   fatf_board_data_work_space()
{
   if (!get_tok())   return p_error();    // 
   skip_endofline();

   return 1;
}

/****************************************************************************/
/*
*/
int   fatf_board_data_fiducials()
{
   if (!get_tok())   return p_error();    // 
   skip_endofrecord();

   return 1;
}

/****************************************************************************/
/*
*/
int   fatf_board_data_thickness()
{
   if (!get_tok())   return p_error();    // 
   skip_endofline();

   return 1;
}

/****************************************************************************/
/*
*/
int   get_packageindex(const char *p)
{
   int   i;

   for (i=0;i<packlistcnt;i++)
   {
      FATFPacklist *packList = packlistarray[i];

      if (!STRCMPI(packList->name, p))
         return packList->index;
   }

   return -1;
}

/******************************************************************************
* fatf_parts
   1,"DB1","","DB-TITLE",607100,-513300,2700,T;
*/
static int fatf_parts()
{
   while (TRUE)
   {
      if (!get_tok())
         return p_error();    // 

      if (!STRCMPI(token, ":EOD"))  
         break;

      int index = atoi(token); // index.

      if (!get_tok())
         return p_error();    // comp name

      CString compname = token;

      if (!get_tok())
         return p_error();    // type name

      CString typname = token;
   
      if (!get_tok())
         return p_error();    // part name which is package name
      CString partname = "";
      partname.Format("PACKAGE_%s", token);  // Sometime the package name and padstack name are the same,
                                             // so add "PACKAGE_" to the name to make it differenct
      if (!get_tok())
         return p_error();    //  

      double x = cnv_unit(token);

      if (!get_tok())
         return p_error();    //  

      double y = cnv_unit(token);

      if (!get_tok())
         return p_error();    //  

      double rot = atoi(token) / 10;

      BOOL mirror = FALSE;
      if (!get_tok())   
         return p_error();    //  

      //TSR 3656
      //rotation is counterclockwise and we mirror on x.
      if (strlen(token) && token[0] == 'B')
      {
         mirror ^= TRUE;
         rot = 180 - rot;
         if(rot < 0)
            rot += 360;
      }

      FATFComp *comp = new FATFComp;
      comp->index = index;
      comp->name = compname;
      comp->package = partname;
      compArray.SetAtGrow(compArrayCount++, comp);  

      DataStruct *data = Graph_Block_Reference(partname, compname, 0, x, y, DegToRad(cnv_rot(rot, mirror)), mirror, 1.0, -1, TRUE);
      data->getInsert()->setInsertType(insertTypePcbComponent);

      if (typname.IsEmpty())
         typname = partname;

      TypeStruct *type = AddType(file, typname);
      if (type->getBlockNumber() < 0)
         type->setBlockNumber( data->getInsert()->getBlockNumber());
      else
      {
         if (type->getBlockNumber() != data->getInsert()->getBlockNumber())
         {
            BlockStruct *block = doc->Find_Block_by_Num(type->getBlockNumber());

            fprintf(ferr, "Component [%s] %s [%s] Shape [%s] has already a different Shape [%s] assigned!\n",
                  compname, ATT_TYPELISTLINK, typname, partname, block->getName());
            display_error++;

            CString newTypeName;
            newTypeName.Format("%s_%s", partname, typname);
            typname = newTypeName;
            TypeStruct *type = AddType(file, typname);
            if (type->getBlockNumber() < 0)
               type->setBlockNumber( data->getInsert()->getBlockNumber());
         }
      }

      doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_TYPELISTLINK, typname, attributeUpdateOverwrite, NULL);

      if (!get_tok())   
         return p_error();
   }

   return 1;
}

/****************************************************************************/
/*
*/
int fatf_nets()
{
   if (IgnoreNetsAndTraces)
   {
      return skip_endofsection();
   }

   int      index = 0;
   CString  netname;
   char     power_or_signal;
   int      compindex, pinindex;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      index = atoi(token); // index.

      if (!get_tok())   return p_error();    // 
      netname = token;

      if (!get_tok())   return p_error();    // 
      power_or_signal = token[0];

      if (strlen(netname) == 0)
         netname.Format("$NET%d", index);
      NetStruct *n = add_net(file, netname);

      FATFNetname *c = new FATFNetname;
      c->index = index;
      c->name = netname;
      c->power_or_signal = power_or_signal;
   
      netnamearray.SetAtGrow(netnamecnt,c);  

      if (!get_tok())   return p_error();    // (

      while (!STRCMPI(token,"("))
      {
         if (!get_tok())   return p_error();    // (

         if (!STRCMPI(token,")"))   // empty record
         {
            //52,,S,((214,1));
            //53,,S,();
            break;
         }

         if (!get_tok())   return p_error();    // comp
         compindex = atoi(token);
         if (!get_tok())   return p_error();    // pin
         pinindex = atoi(token);

         FATFComppin *c = new FATFComppin;
         c->netindex = index;
         c->compindex = compindex;
         c->pinindex = pinindex;
         c->ifp_line = ifp_line;
         comppinarray.SetAtGrow(comppincnt,c);  
         comppincnt++;
         if (!get_tok())   return p_error();    // )

         if (!get_tok())   return p_error();    // (
         push_tok();
      }
      skip_endofrecord();

      netnamecnt++;
   }
   return 1;
}

/****************************************************************************/
/*
*/
int fatf_pad_symbols()
{
   int      index = 0;
   CString  psname;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      index = atoi(token); // index.

      psname.Format("PAD_SYMBOL_%d", index);

      if (!get_tok())   return p_error();    // P_ROUND, P_BLOCK, TRACK

      if (!STRCMPI(token,"P_ROUND"))   // T_ROUND
      {
         double   xsize;
         int      err;

         if (!get_tok())   return p_error();     // (
         if (!get_tok())   return p_error();     
         xsize = cnv_unit(token);
         if (!get_tok())   return p_error();    // )
        
         Graph_Aperture(psname, T_ROUND, xsize , 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, 0, &err);

      }
      else
      if (!STRCMPI(token,"P_BLOCK"))   // T_SQUARE
      {
         double   llx, lly, urx, ury;
         int      err;

         if (!get_tok())   return p_error();    // (
         if (!get_tok())   return p_error();    // llx    
         llx = cnv_unit(token);
         if (!get_tok())   return p_error();    // lly      
         lly = cnv_unit(token);
         if (!get_tok())   return p_error();    // urx
         urx = cnv_unit(token);
         if (!get_tok())   return p_error();    // ury 
         ury = cnv_unit(token);
         if (!get_tok())   return p_error();    // )
        
         double offx = llx + urx;
         double offy = lly + ury;
         double xsize = urx - llx;
         double ysize = ury - lly;

         Graph_Aperture(psname, T_SQUARE, xsize , 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, 0, &err);

      }
      else
      if (!STRCMPI(token,"TRACK"))     // T_OBLONG
      {
         if (!get_tok())   return p_error();    // (
         if (!get_tok())   return p_error();    // width 
         double width = cnv_unit(token);

         double x1, y1, x2, y2;
      
         if (!get_tok())   return p_error();    // (
         if (get_xy(&x1, &y1))         // if (0,y) then vertical, if (x,0) then horizontal
         {

         }
         if (!get_tok())   return p_error();    // )

         if (!get_tok())   return p_error();    // (
         if (get_xy(&x2, &y2))
         {

         }
         if (!get_tok())   return p_error();    // )
         
         if (fabs(x1) < SMALLNUMBER)
         {
            // vertical
            double offx = (y1 + y2) /2;
            double xsize = y2 - y1 + width;
            double rot = 90;
            int    err;

            Graph_Aperture(psname, T_OBLONG, xsize , width, offx, 0.0, DegToRad(rot), 0, BL_APERTURE, 0, &err);

         }
         else
         {
            // horizontal
            double offx = (x1 + x2) /2;
            double xsize = x2 - x1 + width;
            double rot = 0;
            int    err;

            Graph_Aperture(psname, T_OBLONG, xsize , width, offx, 0.0, DegToRad(rot), 0, BL_APERTURE, 0, &err);

         }
      }
      else
      {
         fprintf(ferr,"Unknown PAD_SYMBOL [%s] at %ld\n", token, ifp_line);
         display_error++;
      }

      skip_endofrecord();

   }
   return 1;
}

/****************************************************************************/
/*
1,"COMMON",COMMON,0,ELECTRICAL;
2,"TOP___COPPER",TOP,3,ELECTRICAL;
3,"BOTTOMCOPPER",BOTTOM,2,ELECTRICAL;
*/
int fatf_layer_names()
{
   int      index = 0;
   CString  layername;
   CString  side;
   int      mirrorindex;
   CString  layertype;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      index = atoi(token); // index.

      if (!get_tok())   return p_error();    // 
      layername = token;
      layername.TrimLeft();
      layername.TrimRight();

      if (!get_tok())   return p_error();    // 
      side = token;

      if (!get_tok())   return p_error();    // 
      mirrorindex = atoi(token);

      if (!get_tok())   return p_error();    // 
      layertype = token;

      FATFLayername  *c = new FATFLayername;
      c->index = index;
      c->name = layername;
      c->side = side;
      c->mirrorindex = mirrorindex;
      c->layertype = layertype;

      layernamearray.SetAtGrow(layernamecnt,c);  

      skip_endofrecord();

      layernamecnt++;
   }
   return 1;
}

/****************************************************************************/
/*
*/
int fatf_layer_sets()
{
   int      index = 0;
   CString  layername;
   CString  layerset;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      index = atoi(token); // index.

      if (!get_tok())   return p_error();    // 
      layername = token;
      layername.TrimLeft();
      layername.TrimRight();

      if (!get_tok())   return p_error();    // (
      
      layerset = "";
      if (!get_tok())   return p_error();    // get first

      while (STRCMPI(token,")")) // while not )
      {
         layerset += "L";
         layerset += token;
         layerset += " ";
         if (!get_tok())   return p_error();    // 
      }

      FATFLayerset   *c = new FATFLayerset;
      c->index = index;
      c->name = layername;
      c->set = layerset;

      layersetarray.SetAtGrow(layersetcnt,c);  

      skip_endofrecord();

      layersetcnt++;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int get_drillindex(double size, int layernum)
{
   FATFDrill      drill;

   if (size == 0) return -1;

   for (int i=0;i<drillcnt;i++)
   {
      drill = drillarray.ElementAt(i);
      if (drill.d == size)
         return i;         
   }

   CString  name;
   name.Format("DRILL_%d",drillcnt);

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   drill.d = size;
   drillarray.SetAtGrow(drillcnt,drill);  
   drillcnt++;

   return drillcnt -1;
}

/******************************************************************************
* fatf_pad_stacks
*/
int fatf_pad_stacks()
{
   int index = 0;
   CString psname;

   while (TRUE)
   {
      if (!get_tok())
         return p_error();    // 
      if (!STRCMPI(token,":EOD"))
         break;

      index = atoi(token); // index.      
      if (!get_tok())
         return p_error();    // name
      if (strlen(token))
         psname.Format("PAD_STACK_%s", token);
      else
         psname.Format("PAD_STACK_%d", index);

      if (!get_tok())
         return p_error();    // drill
      double drill; 
   
      if (!strlen(token))
         drill = 0;
      else
         drill = cnv_unit(token);

      if (!get_tok())
         return p_error();    // drillflag B = partially, P = plated, N = non plated

      BlockStruct *b = Graph_Block_On(GBO_APPEND, psname, -1, 0);

      if (drill > 0)
      {
         CString  drillname;
         int drillindex;
         int layernum = Graph_Level("DRILLHOLE","",0);

         if ((drillindex =get_drillindex(drill, layernum)) < 0)
            return 1;

         drillname.Format("DRILL_%d",drillindex);
         Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
      }

      // 1,,0,B,((2,1));
      if (!get_tok())
         return p_error();    // ( first bracket
      if (!STRCMPI(token,";"))
      {
         // just a drill hole
         b->setBlockType(BLOCKTYPE_PADSTACK);
         skip_endofline();
      }
      else
      {
         b->setBlockType(BLOCKTYPE_PADSTACK);

         if (!get_tok())
            return p_error();    // ( second
      
         if (!STRCMPI(token,")"))   // empty record
         {

         }
         else
         {
            if (!get_tok())
               return p_error();    // get first layerset

            while (STRCMPI(token,")")) // while not )
            {
               // layerset
               int layerset = atoi(token);
            
               if (!get_tok())
                  return p_error();    // padsymbol
               int padsymbol = atoi(token);

               int lptr = get_layersetptr(layerset);
         
               CString  pname;
               pname.Format("PAD_SYMBOL_%d", padsymbol);

               int layernum;
               char  *lp, tmp[1000];

               if (lptr < 0)  // layerset is unknown, because it is defined layer in the file.
                  sprintf(tmp, "LS:%d", layerset); 
               else
                  strcpy(tmp, layersetarray[lptr]->set);

               lp = strtok(tmp," ");

               while (lp)
               {
                  layernum = Graph_Level(lp, "", 0);
                  Graph_Block_Reference(pname, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
                  lp = strtok(NULL," ");
               }

               if (!get_tok())
                  return p_error();    // get ) from record
               if (!get_tok())
                  return p_error();    // get start of next record.

               if (!STRCMPI(token,"("))   // now get layer set
               {
                  if (!get_tok())
                     return p_error();    
               }
            }
         } // empty padstack layer section
         skip_endofrecord();
      }

      Graph_Block_Off();

      FATFPadstack   *c = new FATFPadstack;
      c->index = index;
      c->name = psname;

      padstackarray.SetAtGrow(padstackcnt++, c);  
   }
   return 1;
}

/****************************************************************************/
/*
*/
int   fatf_pads()
{
   if (IgnoreVias)
   {
      return skip_endofsection();
   }

   int      index = 0;
   int      padstackindex;
   CString  netname;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      index = atoi(token); // netindex.
      int nptr = get_netptr(index);

      if (nptr < 0)
         netname = "";
      else
         netname = netnamearray[nptr]->name;

      if (!get_tok())   return p_error();    // padstackindex
      padstackindex = atoi(token);  

      CString  padname;
      int pptr = get_padstackptr(padstackindex);
      if (pptr < 0)
      {
         // padstack not found
         padname.Format("PADSTACK_%d_NOT_FOUND",padstackindex);
      }
      else
      {
         padname = padstackarray[pptr]->name;
      }
      if (!get_tok())   return p_error();    // (

      while (!STRCMPI(token,"("))
      {
         if (!get_tok())   return p_error();    // (

         while (!STRCMPI(token,"("))
         {
            double   x, y;
            if (get_xy(&x, &y))
            {
               DataStruct *d = Graph_Block_Reference(padname, NULL, -1, x, y,
                           DegToRad(0), 0 , 1.0,Graph_Level("0","",1), TRUE);
               if (strlen(netname))
               {
                  d->getInsert()->setInsertType(insertTypeVia);
                  doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
                     valueTypeString,
                     netname.GetBuffer(0),attributeUpdateOverwrite, NULL); // x, y, rot, height
               }
               else
               {
                  d->getInsert()->setInsertType(insertTypeFreePad);
               }
            }
            if (!get_tok())   return p_error();    // ;
         }
         if (!get_tok())   return p_error();    // ;
         push_tok();
      }
      skip_endofrecord();
   }


   return 1;
}

/****************************************************************************/
/*
   1,"FONT1",3600,2376,3564,360; 

  TSR 3665: Not all font lines have the same number of parameters. 
  Assuming so caused error messages and illegal tokens. 

*/
int   fatf_fonts()
{
   int      index = 0;
   CString  fontname;
   double   height, width, space, penwidth;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      index = atoi(token); // index.

      // font name
      if (!get_tok())   
         return p_error();
      if(!strncmp(token,";",1))  //if no other parameters exist (end of line).
         continue;      
      fontname = token;

       // char height
      if (!get_tok())   
         return p_error();
      if(!strncmp(token,";",1)) //if no other parameters exist (end of line).
         continue;
      height = cnv_unit(token);

      // char width
      if (!get_tok())   
         return p_error();    
      if(!strncmp(token,";",1)) //if no other parameters exist (end of line).
         continue;
      width = cnv_unit(token);

      // char space 
      if (!get_tok())   
         return p_error();    
      if(!strncmp(token,";",1)) //if no other parameters exist (end of line).
         continue;
      space = cnv_unit(token);

      // spot width
      if (!get_tok())   
         return p_error();    
      if(!strncmp(token,";",1)) //if no other parameters exist (end of line).
         continue;
      penwidth = cnv_unit(token);

      FATFFont *c = new FATFFont;
      c->index = index;
      c->name = fontname;
      c->charheight = height;
      c->charwidth = width;
      c->charspace = space;
      c->strokewidth = penwidth;

      fontarray.SetAtGrow(fontcnt,c);  
      fontcnt++;

      if (!get_tok())   return p_error();    // ;

   }

   return 1;
}

/******************************************************************************
* check_pinnames
   if a pinname is double or no piname, then the pinid makes the name and not the name.
*/
static bool check_pinnames(const char *package)
{
   bool found = false;

   for (int i=0; i<packagePinArrayCount; i++)
   {
      FATFPackagePin *packagePin1 = packagePinArray[i];

      if (packagePin1->pinName.IsEmpty())
      {
         fprintf(ferr, "Empty Pinname found in Package [%s] -> revert to pinindex\n", package);
         display_error++;
         found = true;
         break;
      }

      for (int ii=i+1; ii<packagePinArrayCount; ii++)
      {
         FATFPackagePin *packagePin2 = packagePinArray[ii];

         if (!packagePin1->pinName.Compare(packagePin2->pinName))
         {
            fprintf(ferr, "Duplicated Pinname [%s] found in Package [%s] -> revert to pinindex\n", packagePin1->pinName, package);
            display_error++;
            found = true;
            break;
         }
      }

      if (found)
         break;
   }

   if (found)
   {
      for (int i=0; i<packagePinArrayCount; i++)
      {
         FATFPackagePin *packagePin = packagePinArray[i];
         packagePin->pinName.Format("%d", packagePin->pinID);
      }
   }

   return found;
}

/******************************************************************************
* fatf_packages
*/
int fatf_packages()
{
   while (TRUE)
   {
      if (!get_tok())
         return p_error();
      if (!STRCMPI(token, ":EOD"))
         break;
      int index = atoi(token); // index.

      if (!get_tok())
         return p_error();    // package name
      CString package = "";
      package.Format("PACKAGE_%s", token);   // Sometime the package name and padstack name are the same,
                                             // so add "PACKAGE_" to the name to make it differenct
      if (!get_tok())
         return p_error();    // xmin
      if (!get_tok())   
         return p_error();    // xmax
      if (!get_tok())
         return p_error();    // ymin
      if (!get_tok())
         return p_error();    // ymax

      if (!STRICMP(token, "(")) // if ymax is empty(allowed), get_tok gets "(" from next line and throws off parser
         push_tok();

      FATFPacklist *c = new FATFPacklist;
      c->index = index;
      c->name = package;
      packlistarray.SetAtGrow(packlistcnt++, c);  

      packagePinArrayCount = 0;

      BlockStruct *block = Graph_Block_On(GBO_APPEND, package, -1, 0);
      block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
      do_package(package);
      check_pinnames(package);

		int i=0;
      for (i=0; i<packagePinArrayCount; i++)
      {
         FATFPackagePin *packagePin = packagePinArray[i];

         CString padstackName = packagePin->padstackName;
         if (padstackName.IsEmpty())
         {
            padstackName.Format("$_%s_%d", package, packagePin->pinID);
            fprintf(ferr, "No PADSTACK found for Package [%s] Pin [%d]\n", package, packagePin->pinID);
            display_error++;
         }

         // make sure it is marked as a PADSTACK, even if it is empty
         BlockStruct *padstackGeom = Graph_Block_On(GBO_APPEND, padstackName, -1, 0);
         padstackGeom->setBlockType(BLOCKTYPE_PADSTACK);
         Graph_Block_Off();

         DataStruct *pinInsert = Graph_Block_Reference(padstackName, packagePin->pinName, 0, packagePin->x, packagePin->y, 0, 0, 1, Graph_Level("0", "", 1), TRUE);
         pinInsert->getInsert()->setInsertType(insertTypePin);
         doc->SetAttrib(&pinInsert->getAttributesRef(), doc->IsKeyWord(ATT_COMPPINNR, 0), valueTypeInteger, &packagePin->pinID, attributeUpdateOverwrite, NULL);
      }

      Graph_Block_Off();

      for (i=0; i<packagePinArrayCount; i++)
      {
         // here copy pins into packpin
         FATFPackpin *packPin = new FATFPackpin;
         packPin->packindex = index;
         packPin->pinindex = packagePinArray[i]->pinID;
         packPin->pinname = packagePinArray[i]->pinName;

         packpinarray.SetAtGrow(packpincnt++, packPin);  
      }

      for (i=0; i<packagePinArrayCount; i++)
         delete packagePinArray[i];
      packagePinArrayCount = 0;
   }

   return 1;
}

/****************************************************************************
* fatf_test_points
*/
int   fatf_test_points()
{
   fprintf(ferr, "TEST_POINTS not implemented.\n");
   display_error++;

   return skip_endofsection();
}

/******************************************************************************
* fatf_layers
*/
int   fatf_layers()
{
   if (IgnoreNetsAndTraces)
   {
      return skip_endofsection();
   }

   int      netid = 0;

   while (TRUE)
   {
      if (!get_tok())   return p_error();    // 
      if (!STRCMPI(token,":EOD"))   break;

      netid = atoi(token); // index.

      // here check the next !!! This funny thing is done because of a syntax problem in
      // fatf

      while (TRUE)
      {
         if (!get_tok())   return p_error();    // get next (
         push_tok();

         if (!STRCMPI(token,";"))
            break;

         netlist_layers(netid);
      }        
      Push_tok = FALSE;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int get_textpar(double *x, double *y, double *rot, char *just)
{
   if (!get_tok())   return p_error();    // get {       
   if (STRCMPI(token,"{"))
   {
      fprintf(ferr,"{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())   return p_error();    // x   
   *x = cnv_unit(token);

   if (!get_tok())   return p_error();    // y   
   *y = cnv_unit(token);

   if (!get_tok())   return p_error();    // rot   
   *rot = atof(token) / 10;

   if (!get_tok())   return p_error();    // just   
   just[0] = token[0];
   just[1] = token[1];

   if (!get_tok())   return p_error();    // get }       

   return 1;
}

/****************************************************************************/
/*
*/
static int get_textfont( char *font, double *size)
{
   if (!get_tok())   return p_error();    // get {       
   if (STRCMPI(token,"{"))
   {
      fprintf(ferr,"{ expected in Line Read at %ld\n", ifp_line);
      return -1;
   }

   if (!get_tok())   return p_error();    // font  
   strcpy(font, token);
   if (!get_tok())   return p_error();    // ??   
   if (!get_tok())   return p_error();    // size
   *size = cnv_unit(token);
   
   if (!get_tok())   return p_error();    // get }       

   return 1;
}

/****************************************************************************/
/*
*/
static int fatf_end()
{
   return -2;
}

/******************************************************************************
* tok_search
*  - Search for the token in a command token list.
*/
static int tok_search(List *tok_lst, int tok_size)
{
   for (int i=0; i<tok_size; i++)
      if (!STRCMPI(token, tok_lst[i].token))
         return i;

   return -1;
}

/******************************************************************************
* get_tok
*  - Get a token from the input file.
*/
int get_tok()
{
   if (!Push_tok)
   {
      while (!get_next(cur_line,cur_new))
      {
         if (!get_line(cur_line,MAX_LINE))
            return FALSE;
         else
            cur_new = TRUE;
      }

      cur_new = FALSE;
   }
   else
      Push_tok = FALSE;

   return TRUE;
}

/******************************************************************************
* push_tok
*  - Push back last token.
*/
static int push_tok()
{
   Push_tok = TRUE;

   return TRUE;
}

/****************************************************************************/
/*
   Unicam delimeters are whitespace and comma
*/
static int isdelimeter(char c)
{
   if (isspace(c))   return TRUE;
   if (c == ',')     return TRUE;
   //if (c == ':')      return TRUE;

   return FALSE;
}

/****************************************************************************/
/*
   Get the next token from line.
*/
int get_next(char *lp,int newlp)
{
   static   char  *cp;
   int      i = 0;

   token_name = FALSE;

   if (newlp)  cp = lp;
   
   //for (; isdelimeter(*cp) && *cp != '\0'; ++cp) ;
   // here kill the next, but only 1 comma {"j21",21,,,3}
   if (*cp == ',')   ++cp;
   // there can be whitespace after the comma
   for (; isspace(*cp) && *cp != '\0'; ++cp) ;  

   switch(*cp)
   {
      case '\0':
         return 0;
      case ';':
         token[i++] = *(cp++);  // <= this is a remark !!!
         cur_line[0] = '\0';    // force to end of line
         cur_new = TRUE;
      break;
      case '(':
      case ')':
         token[i++] = *(cp++);
      break;
      case '\"':
         token[i] = '\0';
         token_name = TRUE;
         for (++cp; *cp != '\"'; ++cp, ++i)
         {
            if (*cp != '\0')
               token[i] = *cp;
         }
         ++cp;
      break;
      default:
         for (; !isdelimeter(*cp) && *cp != ',' && *cp != ';' &&
                           *cp != '(' && *cp != ')' &&
                           *cp != '\0'; ++cp, ++i)
         {
            if (*cp == '\\')
               ++cp;
            token[i] = *cp;
         }
      break;
   }
/*
   if (!i && !token_name)
      return(FALSE);
*/
   token[i] = '\0';
   return(TRUE);
}

/****************************************************************************/
/*
   Get a line from the input file.
*/
static int get_line(char *cp,int size)
{
   CString  t;
   long     lc;

   do
   {
      if (fgets(cp,size,ifp) == NULL)
      {
         if (!feof(ifp))
         {
            fprintf(ferr,"Read error\n");
            return -1;
         }
         else
         {
            end_of_file = TRUE;
            cp[0] = '\0';
            return(FALSE);
         }
      }
      t = cp;
      t.TrimLeft();
      t.TrimRight();
      if (t.Left(1) == ';')   
         t = "";
      strcpy(cp,t);

      if ((ifp_line % LineStatusUpdateInterval) == 0 && fileReadProgress != NULL)
      {
         fileReadProgress->updateProgress();
      }

      lc = ++ifp_line;
   }while (t.IsEmpty());

   return(TRUE);
}

/****************************************************************************/
/*
   Parsing error.
*/
int p_error()
{
   CString  tmp;
   CString  t;
   tmp.Format("Fatal Parsing Error : Token \"%s\" on line %ld\n",token,ifp_line);
   t  = tmp;
   tmp.Format("A Parsing Error indicates a corrupt FABMASTER file !\n");
   t += tmp;
   
   ErrorMessage(t,"Fatal FATF Read Error", MB_OK | MB_ICONHAND);
   
   return -1;
}

/****************************************************************************/
/*
*/
double   cnv_unit(const char  *x)
{
   return(atof(x) * unitsfactor);
}

/****************************************************************************/
/*
:LAYER_NAMES
1,"COMMON",COMMON,0,ELECTRICAL;
2,"TOP___COPPER",TOP,3,ELECTRICAL;
3,"BOTTOMCOPPER",BOTTOM,2,ELECTRICAL;
4,"PLANE_3",TRANSPARENT,0,ELECTRICAL;
5,"PLANE_1",TRANSPARENT,0,ELECTRICAL;
6,"INNER_3",TRANSPARENT,0,ELECTRICAL;
7,"PLANE_2",TRANSPARENT,0,ELECTRICAL;
8,"INNER_2",TRANSPARENT,0,ELECTRICAL;
9,"INNER_1",TRANSPARENT,0,ELECTRICAL;
10,"INNER_9",TRANSPARENT,0,ELECTRICAL;
11,"MECANI_1",TRANSPARENT,0,BOARD_CUTOUT;
12,"BODY__PROTEL",TRANSPARENT,0,DOCUMENTATION;
13,"MECANI_4",TRANSPARENT,0,BOARD_CUTOUT;
14,"MECANI_2",TRANSPARENT,0,BOARD_CUTOUT;
15,"FORMAT",TRANSPARENT,0,BOARD_CUTOUT;
16,"DRILL_DRAWIN",TRANSPARENT,0,DOCUMENTATION;
17,"TOP___MASKIN",TOP,18,MASKING;
18,"BOTTOMMASKIN",BOTTOM,17,MASKING;
19,"TOP___ASSEMB",TOP,20,ASSEMBLY;
20,"BOTTOMASSEMB",BOTTOM,19,ASSEMBLY;
21,"TOP___REF",TOP,22,SILKSCREEN;
22,"BOTTOMPASTE",BOTTOM,21,MASKING;
23,"TOP___PASTE",TOP,24,MASKING;
24,"BOTTOM_REF",BOTTOM,23,SILKSCREEN;
25,"TOP_COMMENT",TOP,26,SILKSCREEN;
26,"BOTTOMCOMMEN",BOTTOM,25,SILKSCREEN;
:EOD

*/
int assign_layer()
{
   int      lptr;
   LayerStruct *l;
   int      stackcnt = 0;
   CString  commonlayer;

   commonlayer = "";
	int i=0;
   for (i=0;i<layernamecnt;i++)
   {
      FATFLayername *ll = layernamearray[i];

      CString  layerName;
      layerName.Format("L%d", layernamearray[i]->index);

      lptr = Graph_Level(layerName, "", 0); 
      l = doc->FindLayer(lptr);
      l->setName( layernamearray[i]->name);

      if (!STRCMPI(layernamearray[i]->side,"COMMON"))
      {
         if (!STRCMPI(layernamearray[i]->layertype,"ELECTRICAL")) // drill layer
         {
            commonlayer = layernamearray[i]->name;
            l->setLayerType(LAYTYPE_DRILL);
         }
      }
      else
      if (!STRCMPI(layernamearray[i]->side,"TOP"))
      {
         if (!STRCMPI(layernamearray[i]->layertype,"ELECTRICAL"))
            l->setLayerType(LAYTYPE_SIGNAL_TOP);
         else
         if (!STRCMPI(layernamearray[i]->layertype,"MASKING"))
            l->setLayerType(LAYTYPE_MASK_TOP);
         else
         if (!STRCMPI(layernamearray[i]->layertype,"SILKSCREEN"))
            l->setLayerType(LAYTYPE_SILK_TOP);
         else
            l->setLayerType(LAYTYPE_TOP);
      }
      else
      if (!STRCMPI(layernamearray[i]->side,"BOTTOM"))
      {
         if (!STRCMPI(layernamearray[i]->layertype,"ELECTRICAL"))
            l->setLayerType(LAYTYPE_SIGNAL_BOT);
         else
         if (!STRCMPI(layernamearray[i]->layertype,"MASKING"))
            l->setLayerType(LAYTYPE_MASK_BOTTOM);
         else
         if (!STRCMPI(layernamearray[i]->layertype,"SILKSCREEN"))
            l->setLayerType(LAYTYPE_SILK_BOTTOM);
         else
            l->setLayerType(LAYTYPE_BOTTOM);
      }
      else
      {
         if (!STRCMPI(layernamearray[i]->layertype,"ELECTRICAL"))
            l->setLayerType(LAYTYPE_SIGNAL_INNER);
      }
   }

   // here do mirror
   for (i=0;i<layernamecnt;i++)
   {
      FATFLayername *layName = layernamearray[i];

      if (layName->mirrorindex)
      {
         FATFLayername *mirrorLayer = getLayerFromMirrorIndex(layName->mirrorindex);

         if (mirrorLayer)
            Graph_Level_Mirror(layName->name, mirrorLayer->name, "");
      }
   }

   // here do stack and electrical
   stackcnt = 2;  // top and bottom
   for (i=0;i<layernamecnt;i++)
   {
      FATFLayername *layerName = layernamearray[i];
      if (!STRCMPI(layerName->side,"COMMON"))
         continue;
      if (!STRCMPI(layerName->side,"TOP"))   
         continue;
      if (!STRCMPI(layerName->side,"BOTTOM"))
         continue;

      if (!STRCMPI(layerName->layertype,"ELECTRICAL"))
      {
         stackcnt++;
      }
   }

   int maxstackcnt = stackcnt;
   stackcnt = 1; // top is 1
   for (i=0;i<layernamecnt;i++)
   {
      FATFLayername *layerName = layernamearray[i];

      if (!STRCMPI(layerName->side,"COMMON"))
         continue;

      l = doc->FindLayer_by_Name(layerName->name);

      if (!STRCMPI(layerName->layertype,"ELECTRICAL"))
      {
         if (!STRCMPI(layerName->side,"TOP"))
         {
            l->setElectricalStackNumber(1);
         }
         else
         if (!STRCMPI(layerName->side,"BOTTOM"))
         {
            l->setElectricalStackNumber(maxstackcnt);
         }
         else
         {
            l->setElectricalStackNumber(++stackcnt);
         }
      }
   }

   lptr = Graph_Level("DRILLHOLE", "", 0); 
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_DRILL);

   lptr = Graph_Level("BOARDOUTLINE", "", 0); 
   l = doc->FindLayer(lptr);
   l->setLayerType(LAYTYPE_BOARD_OUTLINE);

   // move drillhole into common, electrical
   LayerStruct *origLayer;
   LayerStruct *newLayer;

   lptr = Graph_Level("DRILLHOLE", "", 0); 
   origLayer = doc->FindLayer(lptr);

   if (strlen(commonlayer))
   {
      lptr = Graph_Level(commonlayer, "", 0); 
      newLayer = doc->FindLayer(lptr);

      MoveLayer(doc, origLayer, newLayer, TRUE);
   }

   return 1;
}

/****************************************************************************/
/*
*/
int make_netlist(COperationProgress* progress)
{
   if (progress != NULL)
   {
      progress->setLength(comppincnt);
      progress->updateProgress(0.);
   }

   for (int i=0;i<comppincnt;i++)
   {
      if (progress != NULL)
      {
         progress->incrementProgress();
      }

      FATFComppin *compPin = comppinarray[i];

      int netptr = get_netptr(compPin->netindex); // find netname from netindex
      if (netptr > -1)
      {
         CString  netname, compname, pinname;
         netname = netnamearray[netptr]->name;

         int compptr = get_complistptr(compPin->compindex); // find compname from compindex
         if (compptr > -1)
         {
            compname = compArray[compptr]->name;

            FATFComp *cc = compArray[compptr];
            int packptr = get_packlistptr(compArray[compptr]->package);

            if (packptr < 0)
            {
               fprintf(ferr, "Package [%s] not defined in :PACKAGE section\n", compArray[compptr]->package);
               display_error++;
            }
            else
            {
               // find pinname from pinindex;
               int packpinptr = get_packpinptr(packlistarray[packptr]->index, compPin->pinindex);
               if (packpinptr < 0)
               {
                  fprintf(ferr, "Pin number [%d] for  Component [%s] not defined in :PACKAGES section found at %ld\n", 
                     compPin->pinindex, compname, compPin->ifp_line);
                  display_error++;
               }
               else
               {
                  pinname = packpinarray[packpinptr]->pinname;
   
                  NetStruct *n = add_net(file,netname);
                  CompPinStruct *p = add_comppin(file, n, compname, pinname);
               }
            }
         }
         else
         {
            fprintf(ferr, "Component index [%d] not defined in :PARTS section found at %ld\n", 
               compPin->compindex, compPin->ifp_line);
            display_error++;
         }
      }
      else
      {
         fprintf(ferr, "Net index [%d] not defined\n", compPin->netindex);
         display_error++;
      }
   }

   return 1;
}

/******************************************************************************
* fabnail_netcheck
*/
static int fabnail_netcheck(FileStruct *file)
{
   int err = 0;

   for (int i=0;i<nailcnt;i++)
   {
      FABNail *nail = nailarray[i];

      NetStruct *net = find_net(file, nail->netname);
      if (net == NULL)
      {
         fprintf(ferr, "Netname check error: Nail Netname [%s] could not be found in loaded file.\n", nail->netname);
         display_error++;
         err++;
      }
      else if (!nail->via)
      {
         // Component Pin
         /* Fabmaster seems to do pinnr vs. pinnames. */
         CompPinStruct *comppin = test_add_comppin(nail->comp, nail->pin, file);
         if (comppin == NULL)
         {
            nail->pinxyfound = FALSE;
            fprintf(ferr, "Netname check error: Nail Comp [%s] Pin [%s] could not be found in loaded file.\n", nail->comp, nail->pin);
            display_error++;
            err++;
         }
         else
         {
            nail->targetPin = comppin;
            if (comppin->getPinCoordinatesComplete())
            {
               nail->pinxyfound = TRUE;
               nail->pinx = comppin->getOriginX();
               nail->piny = comppin->getOriginY();
            }
         }
      }
   }
   // else via, can't find/save via yet, nail coords are yet to be transformed

   return err;
}

/******************************************************************************
* fabnail_associatevias
*/
static int fabnail_associatevias(FileStruct *file)
{
   int err = 0;

   for (int i=0;i<nailcnt;i++)
   {
      FABNail *nail = nailarray[i];

      if (nail->via)
      {
         POSITION pos = file->getBlock()->getHeadDataInsertPosition();
         while (pos != NULL && nail->targetVia == NULL)
         {
            DataStruct *data = file->getBlock()->getNextDataInsert(pos);
            InsertStruct *insert = NULL;

            if (data != NULL && (insert = data->getInsert()) != NULL)
            {
               if (insert->getInsertType() == insertTypeVia)
               {
                  if ( (fabs(insert->getOriginX() - nail->x) < 0.005) &&
                     (fabs(insert->getOriginY() - nail->y) < 0.005) )
                  {
                     nail->targetVia = data;
                  }
               }
            }
         }
      }
   }
   // could do error check on unfound vias, but we're not
   return 1;
}


/******************************************************************************
* fabnail_storenails
*/
static int fabnail_storenails(FileStruct *file)
{
   double diameter = 0.1 * Units_Factor(pageUnitsInches, doc->getSettings().getPageUnits());

	int i=0;
   for (i=0;i<nailcnt;i++)
   {
      if (!Graph_Block_Exists(doc, nailarray[i]->probedef, -1))
      {
         generate_TestProbeGeometry(doc, nailarray[i]->probedef, diameter,
            "T1", 0.04 * Units_Factor(pageUnitsInches, doc->getSettings().getPageUnits()));
      }
   }
   
   // One TEST_ACCESS geometry for all access pts
   char *accessptBlockName = "TEST_ACCESS";
   generate_TestAccessGeometry(doc, accessptBlockName, diameter);


   WORD dLinkKW = doc->IsKeyWord(ATT_DDLINK, 0);
   WORD testResKW = doc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
   WORD probePlacementKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
   WORD netNameKW = doc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKW = doc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);

   for (i=0;i<nailcnt;i++)
   {
      FABNail *nail = nailarray[i];

      DataStruct *probe = Graph_Block_Reference(nail->probedef, nail->nailname, 0, nail->x, nail->y, 0.0, nail->bottom, 1.0, -1, TRUE);
      probe->getInsert()->setInsertType(insertTypeTestProbe);

      CString accessptName = "$$ACCESS_" + nail->nailname;
      DataStruct *accesspt = Graph_Block_Reference(accessptBlockName, accessptName, 0, nail->x, nail->y, 0.0, nail->bottom, 1.0, -1, TRUE);
      accesspt->getInsert()->setInsertType(insertTypeTestAccessPoint);
   
      // Set attributes
      long datalink = accesspt->getEntityNumber();
      probe->setAttrib(doc->getCamCadData(), dLinkKW, valueTypeInteger, (void*)&datalink, attributeUpdateOverwrite, NULL);
      probe->setAttrib(doc->getCamCadData(), testResKW, valueTypeString, /*testResourceType*/"Test", attributeUpdateOverwrite, NULL);
      probe->setAttrib(doc->getCamCadData(), probePlacementKW, valueTypeString, "Placed", attributeUpdateOverwrite, NULL);

      // Add probe number as REFNAME attribute
      CreateTestProbeRefnameAttr(doc, probe, nail->nailname, diameter);

      // Link access pnt to either via or comp/pin
      if (nail->via)
         datalink = nail->targetVia ? nail->targetVia->getEntityNumber() : 0;
      else
         datalink = nail->targetPin ? nail->targetPin->getEntityNumber() : 0;

      accesspt->setAttrib(doc->getCamCadData(), dLinkKW, valueTypeInteger, (void*)&datalink, attributeUpdateOverwrite, NULL);

      // Do not make a netname for nonconn probes.
      if (strlen(nail->netname))
      {
         doc->SetAttrib(&probe->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 1),
            valueTypeString, nail->netname.GetBuffer(0), SA_APPEND, NULL);

         probe->setAttrib(doc->getCamCadData(), netNameKW, valueTypeString, nail->netname.GetBuffer(0), attributeUpdateOverwrite, NULL);
         accesspt->setAttrib(doc->getCamCadData(), netNameKW, valueTypeString, nail->netname.GetBuffer(0), attributeUpdateOverwrite, NULL);
      }
   }

   return 1;
}

/******************************************************************************
* get_probename
*/
static const char *get_probename(int typ)
{
   static   CString  pname;

   for (int i=0;i<naildefcnt;i++)
   {
      if (naildefarray[i]->type == typ)
         return naildefarray[i]->probename;
   }

   fprintf(ferr,".NAIL TYPE [%d] is not defined in IN file.\n", typ);
   display_error++;

   pname.Format("FABMASTER_PROBE_%d", typ);

   return pname;
}

/******************************************************************************
* fabnail_calcoffset
*/
static int fabnail_calcoffset(FileStruct *file)
{
   int cnt = 0;
   CPnt refPnt, origA, origB, origC, newA, newB, newC;
   
   refPnt.x = 0;
   refPnt.y = 0;
   
	int i=0;
   for (i=0;i<nailcnt;i++)
   {
      FABNail* nail = nailarray[i];
      if (nail == NULL || nail->pinxyfound == FALSE)
         continue;

      // make them as far apart as possible
      if (cnt == 0)  
      {
         origA.x = (DbUnit)nail->x;
         origA.y = (DbUnit)nail->y;
         newA.x = (DbUnit)nail->pinx;
         newA.y = (DbUnit)nail->piny;
         cnt++;
      }
      else
      if (cnt == 1)  
      {
         origB.x = (DbUnit)nail->x;
         origB.y = (DbUnit)nail->y;
         newB.x = (DbUnit)nail->pinx;
         newB.y = (DbUnit)nail->piny;
         cnt++;
      }
      else
      if (cnt == 2)  
      {
         origC.x = (DbUnit)nail->x;
         origC.y = (DbUnit)nail->y;
         newC.x = (DbUnit)nail->pinx;
         newC.y = (DbUnit)nail->piny;
         cnt++;
      }

   }

   if (cnt != 3)
   {
      ErrorMessage("Can not auto align!");
      return 0;
   }

   for (i=0;i<nailcnt;i++)
   {
      FABNail* nail = nailarray[i];
      if (nail == NULL || nail->pinxyfound == FALSE)
         continue;

      // make them as far apart as possible A == lower left
      //                                    B == upper left
      //                                    C == upper right
      if (nailarray[i]->x < origA.x)
      {
         if (nailarray[i]->y < origA.y)
         {
            origA.x = (DbUnit)nail->x;
            origA.y = (DbUnit)nail->y;
            newA.x = (DbUnit)nail->pinx;
            newA.y = (DbUnit)nail->piny;
         }
         else
         if (nailarray[i]->y > origB.y)
         {
            origB.x = (DbUnit)nail->x;
            origB.y = (DbUnit)nail->y;
            newB.x = (DbUnit)nail->pinx;
            newB.y = (DbUnit)nail->piny;

         }
      }
      else
      if (nailarray[i]->x > origC.x)
      {
         origC.x = (DbUnit)nail->x;
         origC.y = (DbUnit)nail->y;
         newC.x = (DbUnit)nail->pinx;
         newC.y = (DbUnit)nail->piny;
      }
   }

   double offX = 0;
   double offY = 0;
   double scale = 0;
   double rotation = 0;
   BOOL mirror;

   if (CalcTransform(&refPnt, &origA, &origB, &origC, &newA, &newB, &newC,
                 &offX, &offY, &scale, &rotation, &mirror))
   {
      Mat2x2 m;

      // must make sure it is on 90 degree steps.
      double rot = RadToDeg(rotation);
      RotMat2(&m, rotation);

      Point2 point2;

      for (i=0;i<nailcnt;i++)
      {
         FABNail* nail = nailarray[i];
         if (nail == NULL)
            continue;

         point2.x = nail->x;
         if (mirror) point2.x = -point2.x;
         point2.y = nail->y;
         TransPoint2(&point2, 1, &m, offX, offY);

         nail->x = point2.x;
         nail->y = point2.y;
      }
   }

   return 1;
}

//_____________________________________________________________________________
CFatfReader::CFatfReader(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_pcbFile(NULL)
{
}

void CFatfReader::addFiducialExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);

   m_fiducialExpressionList.AddTail(regularExpression);
}

bool CFatfReader::isFiducialEntityName(const CString& entityName)
{
   bool retval = false;

   for (POSITION pos = m_fiducialExpressionList.GetHeadPosition();pos != NULL && !retval;)
   {
      CRegularExpression* regularExpression = m_fiducialExpressionList.GetNext(pos);

      retval = regularExpression->matches(entityName);
   }

   return retval;
}

void CFatfReader::addTestPointExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);

   m_testPointExpressionList.AddTail(regularExpression);
}

bool CFatfReader::isTestPointEntityName(const CString& entityName)
{
   bool retval = false;

   for (POSITION pos = m_testPointExpressionList.GetHeadPosition();pos != NULL && !retval;)
   {
      CRegularExpression* regularExpression = m_testPointExpressionList.GetNext(pos);

      retval = regularExpression->matches(entityName);
   }

   return retval;
}

void CFatfReader::addToolingExpression(const CString& regularExpressionString)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);

   m_toolingExpressionList.AddTail(regularExpression);
}

bool CFatfReader::isToolingEntityName(const CString& entityName)
{
   bool retval = false;

   for (POSITION pos = m_toolingExpressionList.GetHeadPosition();pos != NULL && !retval;)
   {
      CRegularExpression* regularExpression = m_toolingExpressionList.GetNext(pos);

      retval = regularExpression->matches(entityName);
   }

   return retval;
}

void CFatfReader::fixBlockTypesAndInsertTypes()
{
   for (int blockIndex = m_startingBlockIndex;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* geometry = m_camCadDoc.getBlockAt(blockIndex);

      if (geometry != NULL)
      {
         CString geometryName = geometry->getName();

         //if (geometryName.Find("FID") >= 0)
         //{
         //   int iii = 3;
         //}

         if (isFiducialEntityName(geometryName))
         {
            geometry->setBlockType(blockTypeFiducial);
            fprintf(ferr,"Block type set to Fiducial for geometry '%s'\n",geometryName);
         }

         if (isTestPointEntityName(geometryName))
         {
            geometry->setBlockType(blockTypeTestPoint);
            fprintf(ferr,"Block type set to TestPoint for geometry '%s'\n",geometryName);
         }

         if (isToolingEntityName(geometryName))
         {
            geometry->setBlockType(blockTypeTooling);
            fprintf(ferr,"Block type set to DrillHole for geometry '%s'\n",geometryName);
         }
      }
   }

   if (m_pcbFile != NULL)
   {
      BlockStruct* pcbBlock = m_pcbFile->getBlock();

      for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         BlockStruct* geometry = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());
         CString geometryName = geometry->getName();

         if (isFiducialEntityName(geometryName))
         {
            data->getInsert()->setInsertType(insertTypeFiducial);
         }

         if (isTestPointEntityName(geometryName))
         {
            data->getInsert()->setInsertType(insertTypeTestPoint);
            m_camCadDoc.validateAndRepair(*data);
         }

         if (isToolingEntityName(geometryName))
         {
            data->getInsert()->setInsertType(insertTypeDrillTool);
            m_camCadDoc.validateAndRepair(*data);
         }
      }
   }
}

