// $Header: /CAMCAD/5.0/read_wrt/GerbIn.cpp 68    6/04/07 5:21p Lynn Phung $

/*****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.

   All aperture must be rectangle, not square. This will draw the "Gerber" lines correct.
*/ 

#include <stdafx.h>
#include <stdio.h>
#include <math.h>
#include "graph.h"
#include "geomlib.h"
#include "format_s.h"
#include "txtscan.h"
#include "gauge.h"
#include "attrib.h"
#include "pcbutil.h"
#include "ee.h"
#include "gerbin.h"
#include "CCEtoODB.h"
#include "RwLib.h"
#include "GerberEducatorToolbar.h"  // include this header file to use const QDataSourceGerber
#include "InFile.h"
#include "StandardAperture.h"
#include "RwUiLib.h"


//#define doc_camCadData doc->getCamCadData()
//#define camCadData_camCadDatabase doc->getCamCadData()
//#define camCadDatabase_camCadData camCadDatabase.getCamCadData()


#define newMacroCommandParseMethod
#define newApertureMacroExpressionEvaluationMethod

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgressDlg *progress;

static void FreeGPointArray();
static int load_gerbersettings(const CString fname);
static void change_to_controlcode(CString& string);
static void do_blockend();
static void do_am(CString macro, int filenum);
static int do_ad(char *m, int filenum);
static int do_fs(char *f);
static void make_comment();
static void make_fire9000_comment();

#ifndef newApertureMacroExpressionEvaluationMethod
static CString clean_comments(const CString& string);
static void clean_equations(CString& equation);
#endif

static const char *resolve_complex_macro(const char *p1, char **plist, int pcnt, int *exchanged);
static void do_koo_adjust();
static void write_db(CString layername,int forceout);
static int Check_Aperture(const char *name, int apptyp, double sizea, double sizeb, double xc, double yc, double rot,
                  int  appcode, DbFlag flg);
static int Gerber_Read_Token(FILE *text_file, char *token, int max_length);
//static void get_ap_rename_name(char *orig,int *app);
static char is_block_end(char l);
static double cnv_tok(char *);
static int cont_flag = FALSE;
static void do_gerber(const char *fname, int filenum, CString logFilePath);
static int get_macroptr(CString macroName, int fileNum);

static GerberLocal G;
static CCEtoODBDoc *doc;
static FILE *Global_fp;

static double GBscale, OriginalGBscale;

static   MArray      marray;
static   int         mcnt;

static GPointArray gPointArray;
static int gPointArrayCount = 0;

static AMacro *aMacroArray;
static int aMacroArrayCount = 0;

//static   Ap_Rename   *ap_rename;
//static   int         ap_renamecnt = 0;
static CApertureRenameTable apertureRenameTable;

static   int         pageUnits; 
static   double      unitscale = 1;
static   CString     last_draw_ap_name;
static   CString     lastcharstring;
static   int         macro_thermals;

static   CString     cur_layername;
static   int         cur_negative = FALSE;

static   CString     cur_app_name;
static   CString     prefix;

static   int         G4type;
static   CString     G4comment;
static   CString     G4key;
static   bool        ATTACH_G4COMMENT;
static   bool        CREATE_NEGATIVE_LAYER;
static   bool        CREATE_NAMED_LAYER;
static   bool        USE_FILEPREFIX_NAMED_LAYER;
static   bool        s_optionPrimitive21TransformOrderRotateFirst;
static   bool        s_optionPrimitive22TransformOrderRotateFirst;
static   bool        s_optionGenerateComplexThermalApertures;

// Apertre Macro polygon ambiguity choices
static   PolygonApertureMacroOriginTypeTag      s_polygonApertureMacroOriginType;
static   PolygonApertureMacroDiameterTypeTag    s_polygonApertureMacroDiameterType;
static   bool                                   s_polygonApertureOrientationZeroFlag;

static   char        token[MAX_LINE+1];   // MAX_LINE

static   int         cur_filenum = 0;

static   int         display_error, display_log;
static   FILE        *flog;

static   CString     worddelimeter;
static   CString     nonchar;
static   CString     blockendchar;

static   FileStruct  *file;               // this is static because on multiple file 
                                          // load I need to remember the last file.

static   int         AM_PRIMITIVE;
static   int         LOWEST_APPDCODE;     // for DELPHI, we need to start with D5

static   CMessageFilter *GerberMessageFilter = NULL;

static void logError(const char* format,...)
{
}

static int getFloatingLayerIndex()
{
   return doc->getDefinedFloatingLayer()->getLayerIndex();
}

static int getZeroWidthIndex()
{
   return doc->getZeroWidthIndex();
}

void CCEtoODBDoc::OnGerberEducatorGerberImport()
{
   //DoFileImport(fileTypeGerber);
}

CCamCadDatabase* s_camCadDatabase = NULL;

CCamCadDatabase& getCamCadDatabase()
{
   if (s_camCadDatabase == NULL)
   {
      s_camCadDatabase = new CCamCadDatabase(*doc);
   }

   return *s_camCadDatabase;
}

void releaseCamCadDatabase()
{
   delete s_camCadDatabase;
   s_camCadDatabase = NULL;
}

/******************************************************************************
* ReadGerber
struct GRstruct {
   int format;    // 0-G RS-274; 1-G RS-274-X;
   int type;      // 0-Absolut; 1-Incremental
   int zero;      // 0-Leading; 1-Trailing; 2-None suppression
   int units;     // 0-English; 1-Metric
   int digits;    // Number of Digits before decimal
   int decimal;   // Number of decimal places
   int G75Circle; // 360° circluar interpolation
}
vor_komma = Format.GR.digits;
nach_komma = Format.GR.decimal;
*/
void ReadGerber(const char *path_buffer,CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits,
                 int cur_filecnt, int tot_filecnt)
{
   releaseCamCadDatabase();
   doc = Doc;

   try
   {
   char drive[_MAX_DRIVE]; 
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   //CCoordinateBreakPoint::setSearchPoint(3.625,.770,.05);

#ifdef newMacroProcessing
   CGerberMacros::getGerberMacros(getCamCadDatabase());
#else
   getCamCadDatabase();
#endif

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );
   strcpy(f, fname);
   strcat(f, ext);

   display_error = FALSE;
   display_log   = FALSE;

   CString logFile = GetLogfilePath("gerber.log");
   if (cur_filecnt > 0)
   {
      if ((flog = fopen(logFile, "a+t")) == NULL)  // append file
      {
         ErrorMessage(logFile, "Can not open Logfile", MB_ICONEXCLAMATION | MB_OK);
         return;
      }
   }
   else
   {
      if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
      {
         ErrorMessage(logFile, "Can not open Logfile", MB_ICONEXCLAMATION | MB_OK);
         return;
      }

      // Reset the message filter on first gerber file import
      if (GerberMessageFilter != NULL)
         delete GerberMessageFilter;
      GerberMessageFilter = new CMessageFilter(messageFilterTypeFormat);

      // filetype is unknown -- graphic
      file = Graph_File_Start(f, Type_Gerber);

      cur_filenum = file->getFileNumber();
      if (tot_filecnt > 1)
      {
         // if more then 1 gerber file loaded, make the filename the directory name
         CString  tmp;
         if (strlen(dir))
         {
            tmp = dir;
            if (dir[0] == '/' || dir[0] == '\\')
               tmp.Delete(0, 1);
            if (dir[strlen(dir)-1] == '/' || dir[strlen(dir)-1] == '\\')
               tmp.Delete(strlen(tmp)-1, 1);
            file->setName(tmp);
         }
      }
   }

#ifdef _DEBUG
   CTime t;
   t = t.GetCurrentTime();
   fprintf(flog, "DEBUG: Start Time %s - %s\n", path_buffer, t.Format("date :%A, %B %d, %Y at %H:%M:%S"));
#endif
   
   pageUnits = PageUnits;
   cur_negative = FALSE;
   G.lpd = TRUE;
   G.parstart = FALSE;
   G.apstart = FALSE;
   G.vor_komma = Format->GR.digits;
   prefix = Format->prefix;

   // metrix - mil is done in scaleUnitsFactor.
   OriginalGBscale = GBscale = Format->Scale;

   if (Format->GR.units == 0)
      unitscale = Units_Factor(UNIT_INCHES, PageUnits);
   else
      unitscale = Units_Factor(UNIT_MM, PageUnits);

   G.nach_komma = Format->GR.decimal;
   G.absolute = !Format->GR.type;
   G.zeroleading  = Format->GR.zero;
   macro_thermals = Format->GR.thermalMacros;

	int i=0;
   for (i=0; i<G.nach_komma; i++)
      GBscale /=10;

   G.G75code = Format->GR.circleMode; // 0=Sectorize 1=360° circular interpolation - switch off by G74
   G.GerberX = (Format->GR.format == 1);
   G.Fire9000= (Format->GR.format == 2);

   if ((Global_fp = fopen(path_buffer, "rb")) == NULL)
   {
      ErrorMessage("Error open file", "Error");
      return;
   }

   gPointArray.SetSize(100, 100);
   gPointArrayCount = 0;
   marray.SetSize(100, 100);
   mcnt = 0;

   if ((aMacroArray = (AMacro *) calloc(MAX_MACRO,sizeof(AMacro))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   aMacroArrayCount = 0;

   //if ((ap_rename = (Ap_Rename *) calloc(MAX_AP_RENAME,sizeof(Ap_Rename))) == NULL)
   //   MemErrorMessage(__FILE__, __LINE__);
   //ap_renamecnt = 0;

   G.width0index = -1;  // initialize to -1 (not defined)
   LOWEST_APPDCODE = 10;

   CString settingsFile( getApp().getImportSettingsFilePath("gerber.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nGerber Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_gerbersettings(settingsFile);

   Graph_Level("0","",1);
   G.lpcnt = 0;
   G.layernum = -1;
   do_gerber(f, cur_filecnt, logFile);

   if (G.layernum == -1) // graph a layer for an empty file
   {
      int lay = Graph_Level(f, "", 0);

      LayerStruct *layer = doc->FindLayer(lay);
      if (layer)
      {
         CString dataSource = QDataSourceGerber;
         int dataSourceKw = doc->getStandardAttributeKeywordIndex(standardAttributeDataSource);
         layer->setAttrib(doc->getCamCadData(), dataSourceKw, valueTypeString, dataSource.GetBuffer(0), attributeUpdateOverwrite, NULL);  
      }
   }

   fclose(Global_fp);

   // only do it on the last file.
   if (cur_filecnt == (tot_filecnt-1))
   {
      progress->SetStatus("Optimize Data...");
      double accuracy = get_accuracy(doc);
      EliminateSinglePointPolys(doc);                
      Generate_Unique_DCodes(doc);
      //BreakSpikePolys(file->getBlock()); 
      //progress->SetStatus("Crack");
      //Crack(doc,file->getBlock(), FALSE);      <== speed problem  
      //progress->SetStatus("Elim traces");
      //EliminateOverlappingTraces(doc,file->getBlock(), FALSE, accuracy);  // all polys, not electrical only
   }

#ifdef newMacroProcessing
   CGerberMacros::releaseGerberMacros();
#else
#endif

   FreeGPointArray();
   marray.RemoveAll();
   mcnt = 0;

   for (i=0; i<aMacroArrayCount; i++)
   {
      if (aMacroArray[i].name)     
         free(aMacroArray[i].name);
      if (aMacroArray[i].command)  
         free(aMacroArray[i].command);
   }

   free(aMacroArray);
   aMacroArrayCount = 0;

   //for (i=0; i<ap_renamecnt; i++)
   //{
   //   if (ap_rename[i].name)  
   //      free(ap_rename[i].name);
   //   if (ap_rename[i].orig)  
   //      free(ap_rename[i].orig);
   //}

   //free(ap_rename);

   apertureRenameTable.empty();

#ifdef _DEBUG
   t = t.GetCurrentTime();
   fprintf(flog, "DEBUG: Close Time %s\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));
   display_log = TRUE;
   display_error = TRUE;
#endif

   fclose(flog);

   // Clean out and dispose of message filter when finished processing
   // last file in group
   if (cur_filecnt == tot_filecnt)
   {
      GerberMessageFilter->empty();
      delete GerberMessageFilter;
      GerberMessageFilter = NULL;
   }

   if (display_error && display_log && cur_filecnt == (tot_filecnt-1))
      Logreader(logFile);
   }
   catch (...)
   {
      ErrorMessage("Exception caught: ReadGerber()");
   }

   releaseCamCadDatabase();
}

/******************************************************************************
* load_gerbersettings
*/
static int load_gerbersettings(const CString fname)
{
   worddelimeter = "$*XYZIJKGDMN";
   nonchar       = "\t \r\a\b\v\n";
   blockendchar  = "\r*$";
   AM_PRIMITIVE               = 50;
   ATTACH_G4COMMENT           = false;
   CREATE_NEGATIVE_LAYER      = true;
   CREATE_NAMED_LAYER         = true;
   USE_FILEPREFIX_NAMED_LAYER = false;
   G4type                     = G4TYPE_UNKNOWN;
   s_optionPrimitive21TransformOrderRotateFirst = false;
   s_optionPrimitive22TransformOrderRotateFirst = false;
   s_optionGenerateComplexThermalApertures      = true;
   s_polygonApertureMacroOriginType   = polygonApertureMacroOriginFromExtent;
   s_polygonApertureMacroDiameterType = polygonApertureMacroDiameterFromExtentWidth;
   s_polygonApertureOrientationZeroFlag = true;

   CInFile inFile;

   if (!inFile.open(fname))
   {
      // if no settings file found, do not display a message.
      return 0;
   }

   while (inFile.getNextCommandLine())
   {
      if (inFile.isCommand(".ATTACH_G4COMMENT",2))
      {
         inFile.parseYesNoParam(1,ATTACH_G4COMMENT,false);
      }
      else if (inFile.isCommand(".CREATE_NEGATIVE_LAYER",2))
      {
         inFile.parseYesNoParam(1,CREATE_NEGATIVE_LAYER,false);
      }
      else if (inFile.isCommand(".CREATE_NAMED_LAYER",2))
      {
         inFile.parseYesNoParam(1,CREATE_NAMED_LAYER,false);
      }
      else if (inFile.isCommand(".USE_FILEPREFIX_NAMED_LAYER",2))
      {
         inFile.parseYesNoParam(1,USE_FILEPREFIX_NAMED_LAYER,false);
      }
      else if (inFile.isCommand(".G4COMMENTTYPE",2))
      {
         if (inFile.getParam(1).CompareNoCase("GERBTOOL") == 0)
         {
            G4type = G4TYPE_GERBTOOL;
         }
      }
      else if (inFile.isCommand(".AM_PRIMITIVE",2))
      {
         AM_PRIMITIVE = inFile.getIntParam(1);
      }
      else if (inFile.isCommand(".WORDDELIMETER",2))
      {
         if (!inFile.getParam(1).IsEmpty())
         {
            worddelimeter = inFile.getParam(1);
         }

         change_to_controlcode(worddelimeter);
      }
      else if (inFile.isCommand(".NONCHAR",2))
      {
         if (!inFile.getParam(1).IsEmpty())
         {
            nonchar = inFile.getParam(1);
         }

         change_to_controlcode(nonchar);
      }
      else if (inFile.isCommand(".BLOCKENDCHAR",2))
      {
         if (!inFile.getParam(1).IsEmpty())
         {
            blockendchar = inFile.getParam(1);
         }

         change_to_controlcode(blockendchar);
      }
      else if (inFile.isCommand(".LOWESTDCODE",2))
      {
         LOWEST_APPDCODE = inFile.getIntParam(1);
      }
      else if (inFile.isCommand(".Primitive21TransformOrderRotateFirst",2))
      {
         inFile.parseYesNoParam(1,s_optionPrimitive21TransformOrderRotateFirst,false);
      }
      else if (inFile.isCommand(".Primitive22TransformOrderRotateFirst",2))
      {
         inFile.parseYesNoParam(1,s_optionPrimitive22TransformOrderRotateFirst,false);
      }
      else if (inFile.isCommand(".GenerateComplexThermalApertures",2))
      {
         inFile.parseYesNoParam(1,s_optionGenerateComplexThermalApertures,true);
      }
      else if (inFile.isCommand(".PolygonApertureMacroOriginType",2))
      {
         // fromCircumCircle, fromExtent
         CString macroOriginType = inFile.getParam(1);

         if (inFile.parameterStringsEqual(macroOriginType,"fromCircumCircle"))
         {
            s_polygonApertureMacroOriginType = polygonApertureMacroOriginFromCircumCircle;
         }
         else if (inFile.parameterStringsEqual(macroOriginType,"fromExtent"))
         {
            s_polygonApertureMacroOriginType = polygonApertureMacroOriginFromExtent;
         }
      }
      else if (inFile.isCommand(".PolygonApertureMacroDiameterType",2))
      {
         // fromCircumCircle, fromInCircle, fromExtentWidth
         CString macroOriginType = inFile.getParam(1);

         if (inFile.parameterStringsEqual(macroOriginType,"fromCircumCircle"))
         {
            s_polygonApertureMacroDiameterType = polygonApertureMacroDiameterFromCircumCircle;
         }
         else if (inFile.parameterStringsEqual(macroOriginType,"fromInCircle"))
         {
            s_polygonApertureMacroDiameterType = polygonApertureMacroODiameterFromInCircle;
         }
         else if (inFile.parameterStringsEqual(macroOriginType,"fromExtentWidth"))
         {
            s_polygonApertureMacroDiameterType = polygonApertureMacroDiameterFromExtentWidth;
         }
      }
      else if (inFile.isCommand(".PolygonApertureOrientationZero",2))
      {         
         inFile.parseYesNoParam(1,s_polygonApertureOrientationZeroFlag,true);
      }
      else if (inFile.isCommand(".AM_Polygon_RevD",2))
      {
         bool revDFlag;
         inFile.parseYesNoParam(1,revDFlag,true);

         if (revDFlag)
         {
            s_polygonApertureMacroOriginType     = polygonApertureMacroOriginFromExtent;
            s_polygonApertureMacroDiameterType   = polygonApertureMacroDiameterFromExtentWidth;
            s_polygonApertureOrientationZeroFlag = true;
         }
         else
         {
            s_polygonApertureMacroOriginType     = polygonApertureMacroOriginFromExtent;
            s_polygonApertureMacroDiameterType   = polygonApertureMacroDiameterFromExtentWidth;
            s_polygonApertureOrientationZeroFlag = false;
         }
      }
   }

   return 1;
}

static void setApertureName(int dcode,int uniqueIndex = -1)
{
   if (uniqueIndex < 0)
   {
      cur_app_name.Format("%sD%d",prefix,dcode);
   }
   else
   {
      cur_app_name.Format("%sD%d_%d",prefix,dcode,uniqueIndex);
   }
}

static CString getApertureName()
{
   return cur_app_name;
}

static void setApertureName(const CString& apertureName)
{
   cur_app_name = apertureName;
}

static void setUniqueApertureName()
{
   int err = 0;
   CString originalApertureName = getApertureName();

   // check if cur_app_name already is assigned to  a mmacroname.
   while (Graph_Block_Exists(doc, getApertureName(), -1))
   {
      setApertureName(G.cur_app,++err);
   }

   if (err) 
   {
      apertureRenameTable.renameAperture(getApertureName(),originalApertureName);
      G.cur_app = 0;
   }
}

//----------------------------------------------------------------------------

static bool arePercentSignsMatched(FILE *fp)
{
   // Percent signs are like quote marks, at beginning and end of parameter commands.
   // Check to see if this data file conforms.

   bool percentOpen = false;

   if (fp != NULL)
   {
      rewind(fp);

      int linepos = -1;
      bool percentCloseIfNewlineNext = false;

      char c;
      while ((c = fgetc(fp)) != EOF)
      {
         linepos++;

         if (c == '\n' || c == '\r')
         {
            // Process newline chars

            if (percentCloseIfNewlineNext)
            {
               // all is well, reset
               percentCloseIfNewlineNext = false;
               percentOpen = false;
            }

            linepos = -1; // reset for new line
         }
         else
         {
            // Process non-newline chars

            if (c == '%')
            {
               if (linepos == 0)
               {
                  if (!percentOpen)
                  {
                     percentOpen = true;
                  }
                  else
                  {
                     // is already open, so this is a close only if it is only thing on the line
                     percentCloseIfNewlineNext = true;
                  }
               }
               else
               {
                  // opening % is always line pos 0, so this has to be a close
                  percentOpen = false;
               }
            }
            else
            {
               // Check for prev char being closing %
               if (percentOpen && percentCloseIfNewlineNext)
               {
                  // Encountered another % while % was already open, and this one is
                  // not on a line by itself, it is followed by current character c,
                  // so it can't have been a closing %. Therefore the
                  // syntax rule has been violated and we can not treat % as text
                  // delimiter in parser (i.e. treat it like quotes, as original code
                  // did and Gerber syntax rules say we should). It is a syntax error in
                  // the Gerber file, and the result is we can not process multi-line
                  // parameter commands in this file. (But maybe  it doesn't have any, so
                  // it will import okay anyway.)

                  return false;
               }
            }
         }
      }

      rewind(fp);
   }

   // If we get here and % is still open, then file has "no closing %" syntax error
   return !percentOpen;
}

/******************************************************************************
* do_gerber
*  - Do all scan
*/
void do_gerber(const char *fname, int filenum, CString logFilePath)
{
   char lastChar;
   double i, j;
   int res, lastres, err;
   int correction = FALSE;
   int steprep = FALSE;
   int penmode_before_36;
   int steprepeatcnt = 0;

   // Case 2004, Compensate for syntax error in gerber file
   // Case sample data has a single % at begining of file. If used for "text delimiter" the
   // entire file becomes one "token" (actually two, the first % because it is special,
   // and then the rest of the file is one "quoted" token since no terminating % is
   // ever encountered).
   char *textdelimiter = "%";
   if (!arePercentSignsMatched(Global_fp))
   {
      fprintf(flog, "Syntax error in input file \"%s\".  Mismatched parameter command delimiters (%%).\n", fname);
      GerberMessageFilter->formatMessageBoxApp("Syntax error(s) encountered in one or more of the files loaded. See log file \"%s\" for details.", logFilePath);
      textdelimiter = ""; // syntax error in gerber file, cannot use % like quote (i.e. can't be text delimiter)
   }

   Set_Comment("\001", "\001");
   Set_Text_Delimeter( textdelimiter );
   Set_Word_Delimeters( worddelimeter );  
   Set_White_Characters( "" );   // white char will terminate the token.
   Set_Non_Characters( nonchar );
   Set_Linecnt(0L);
   Reset_File(Global_fp);

   G4comment = "";
   G4key = "COMMENT";
   lastChar = ' ';                        
   penmode_before_36 = G.drawmode = PENUP;
   G.done = TRUE;
   G.arc_interpolation = FALSE;
   G.sequencecnt = 0;
   G.cur_app = LOWEST_APPDCODE;  // initialize to default. gerberfile may start with nothing
   setApertureName(G.cur_app);
   last_draw_ap_name = getApertureName();
   G.cur_x = 0.0;
   G.cur_y = 0.0;

   // just in case
   cur_layername = fname;

   while ((res = Gerber_Read_Token(Global_fp, token, MAX_LINE)) != EOF_CHARACTER)
   {
      lastres = res; // res -99 is that token was longer than MAX_LINE, which only should happen in a Macro 
                     // definition;
      CString test_tok = token;  // for debugging

      if (res == STOP_TEXT) // this is happening when a file start %AMxxx
         lastChar = Delimeter();

      // is block end looks at the blockend default
      lastChar = is_block_end(lastChar);

      switch (lastChar)
      {
      case '%':
         {
            // needs to be written and saved 
            write_db(cur_layername, 1);    // force out
            G4comment = "";
            FreeGPointArray();
            G.old_x = G.cur_x;
            G.old_y = G.cur_y;

            CString macrotoken = token;
            //long l= Get_Linecnt();
            if (lastres == -99)  // if this did not end with a % token string was too short
            {
               char  c, tok[MAX_LINE];
               int   tcnt = 0;
               while ((res = Read_Char(Global_fp, &c)) != EOF_CHARACTER)
               {
                  if (c == '%')  
                     break;
                  if (isspace(c))   
                     continue;

                  tok[tcnt++] = c;
                  if (tcnt < MAX_LINE-1)
                  {
                     // ok
                  }
                  else
                  {
                     tok[tcnt] = '\0';
                     macrotoken += tok;
                     tcnt = 0;
                  }
               }
               tok[tcnt] = '\0';
               macrotoken += tok;
            }
            // macro from VALOR can get really long (over 100KB)
            // int w=strlen(macrotoken);
            // l= Get_Linecnt();

            if (!STRNICMP(token, "SR", 2))
            {
               CString blockdef;
               blockdef.Format("STEP&REPEAT_%s_%d", fname, ++steprepeatcnt);
               CString blockref;
               
               // this is a step and repeat command.
               char    *lp;   
               CString t=token;
               int     xrep=1, yrep=1;
               double  xdist=0,ydist=0;

               if ((lp = strchr(token, 'X')))
                  xrep = atoi(&lp[1]);
               if ((lp = strchr(token, 'Y')))
                  yrep = atoi(&lp[1]);

               if ((lp = strchr(token, 'I')))
                  xdist= atof(&lp[1]) * unitscale;
               if ((lp = strchr(token, 'J')))
                  ydist= atof(&lp[1]) * unitscale;

               if (steprep)
               {
                  Graph_Block_Off();
                  steprep = FALSE;
               }
               if (xrep > 1 || yrep > 1)
               {
                  // make graph_block_on to capture the following command.
                  Graph_Block_On(GBO_APPEND, blockdef, -1, 0);
                  Graph_Block_Off();
                  // make graph_block_off on the end.
                  // place graph_block_ref according to step-rep command.
                  double x = 0, y = 0;
                  for (int i=0; i<xrep; i++)
                  {
                     y = 0;
                     for (int j=0; j<yrep; j++)
                     {
                        blockref.Format("%s_%d_%d", blockdef, i, j);
                        Graph_Block_Reference(blockdef, blockref, 0, x, y, 0.0, 0, 1.0, -1, TRUE);
                        y += ydist;
                     }
                     x += xdist;
                  }
                  steprep = TRUE;
                  Graph_Block_On(GBO_APPEND, blockdef, -1, 0);
               }
            }
            else if (!STRNICMP(token, "FS", 2))
            {
               do_fs(token);
            }
            else if (!STRNICMP(token, "AM", 2) && G.GerberX)
            {
#ifdef newMacroProcessing
// lines in file ""
/*
%AMVB_RCRECTANGLE*
$3=$3X2*
21,1,$1-$3,$2,0,0,0*
21,1,$1,$2-$3,0,0,0*
$1=$1/2*
$2=$2/2*
$3=$3/2*
$1=$1-$3*
$2=$2-$3*
1,1,$3X2,0-$1,0-$2*
1,1,$3X2,0-$1,$2*
1,1,$3X2,$1,$2*
1,1,$3X2,$1,0-$2*
%
*/

// macrotoken contains "AMVB_RCRECTANGLE*$3=$3X2*21,1,$1-$3,$2,0,0,0*21,1,$1,$2-$3,0,0,0*$1=$1/2*$2=$2/2*$3=$3/2*$1=$1-$3*$2=$2-$3*1,1,$3X2,0-$1,0-$2*1,1,$3X2,0-$1,$2*1,1,$3X2,$1,$2*1,1,$3X2,$1,0-$2*"

               CGerberMacros::getGerberMacros().addMacroString(macrotoken,filenum);
#else
               do_am(macrotoken, filenum);  // Aperature macro
#endif
            }
            else if (!STRNICMP(token, "AD", 2) && G.GerberX) // only use it if Extented Gerber
            {
               char t[MAX_LINE];

//#ifdef newMacroProcessing
//               // The following is not implemented yet, for lack of time
//               //CGerberMacros::getGerberMacros().instantiateMacros(token,filenum);
//
//               // Instead, construct the macro line and use the old processing method
//               // knv
//               // line in file "%ADD42VB_RCRECTANGLE,0.3000X0.3000X0.0500*%"
//               // token "ADD42VB_RCRECTANGLE,0.3000X0.3000X0.0500*"
//               CString macrosString = CGerberMacros::getGerberMacros().getMacroString(token,filenum);
//
//               strcpy(t, macrosString);
//#else
               // here now can be multiple ADD....* in this line.
               
               strcpy(t, token);
//#endif

               char *l;

               // yes I know this is hiddeous, but I had to glue this in afterwards.
               while (strlen(t) && (l = strtok(t, "*")))
               {
                  char tok[1000];
                  sprintf(tok, "%s*", l);

                  if (l = strtok(NULL, "\n"))
                     sprintf(t, "%s", l);
                  else
                     strcpy(t,"");

                  if (!do_ad(tok, filenum))  // Aperture definition
                  {
                     // bad stuff
                     i = 0;
                  }
               }
            }
            else if (!STRNICMP(token, "MO", 2))
            {
                if (!STRNICMP(token, "MOIN", 4))
                   unitscale = Units_Factor(UNIT_INCHES, pageUnits);
                else
                   unitscale = Units_Factor(UNIT_MM, pageUnits);
            }
            else if (!STRNICMP(token, "LN", 2))
            {
               if (CREATE_NAMED_LAYER)
               {
                  // this is update a layer name
                  char *lp;
                  if ((lp = strtok(&token[2], "*")) != NULL)
                  {
                     CString tmp = lp;
                     tmp.TrimLeft();
                     tmp.TrimRight();
                     if (USE_FILEPREFIX_NAMED_LAYER)
                        cur_layername.Format("%s_%s", fname, tmp);
                     else
                        cur_layername = tmp;

                     G.layernum = -1;   // needs to reinit the current layer
                  }
               }
            }
            else if  (!STRNICMP(token, "LP", 2))
            {
               CString  tmp;
               
               // only create a new layer if LPC is mentioned.
               // D = dark -> cur_negative is false;
               if (token[2] == 'D' && !G.lpd)   // is now lpd and was not before
               {
                  if (!CREATE_NEGATIVE_LAYER)
                     cur_negative = FALSE;
                  else
                  {
                     tmp.Format("%s_D_%d", fname, ++G.lpcnt);
                     cur_layername = tmp;

                     G.layernum = -1;   // needs to reinit the current layer
                  }
                  G.lpd = TRUE;
               }
               else if (token[2] == 'C' && G.lpd)  // is now lpc and was not before
               {
                  if (!CREATE_NEGATIVE_LAYER)
                     cur_negative = TRUE;
                  else
                  {
                     tmp.Format("%s_C_%d", fname, ++G.lpcnt);
                     cur_layername = tmp;
                     int lay = Graph_Level(tmp, "", 0); // makes every gerber file a different color

                     LayerStruct *layer = doc->FindLayer(lay);
                     if (layer)
                     {
                        CString dataSource = QDataSourceGerber;
                        int dataSourceKw = doc->getStandardAttributeKeywordIndex(standardAttributeDataSource);
   
                        layer->setNegative(true);
                        layer->setAttrib(doc->getCamCadData(), dataSourceKw, valueTypeString, dataSource.GetBuffer(0), attributeUpdateOverwrite, NULL);  
                     }

                     G.layernum = -1;   // needs to reinit the current layer
                  }
                  G.lpd = FALSE;
               }
            }
         }
         break;

      case 'G':
         G.cur_gcode = atoi(token);
         G.arc_interpolation = FALSE; // any Gcode will reset arc interpolation.

         switch (G.cur_gcode)
         {
         case 54: // Tool Prepare
            break;
         case 1:
            G.arc_interpolation = FALSE;
            break;
         case 36:
            penmode_before_36 = G.drawmode;
            write_db(cur_layername, 1);   // force the buffer out;
            G.G36fill = TRUE;
            break;
         case 37:
            write_db(cur_layername, 1);   // force fill out
            G.G36fill = FALSE;
            G.drawmode = penmode_before_36;
            //G.drawmode = PENUP; <- there is no forced Penup after G37 !!
            break;
         case 70:
            // english
            unitscale = Units_Factor(UNIT_INCHES, pageUnits);
            break;
         case 71:
            // metric
            unitscale = Units_Factor(UNIT_MM, pageUnits);
            break;
         case 75:
            G.G75code = TRUE;
            break;
         case 74:
            G.G75code = FALSE;
            G.arc_interpolation = FALSE;
            break;
         case 90:
            G.absolute = TRUE;
            break;
         case 91:
            G.absolute = FALSE;
            break;
         case 2:
         case 20:
         case 21:
            G.arc_interpolation = TRUE;
            G.counterclock = FALSE;
            break;
         case 3:
         case 30:
         case 31:
            G.arc_interpolation = TRUE;
            G.counterclock = TRUE;
            break;
         case  4:
            {
               char ch = Delimeter();
               // if token is not * ending, than do it.
               // get char until $ or *
               G4comment = lastcharstring;
               if (G4comment.Left(1) == "0")
                  G4comment.Delete(0, 1);
               if (G4comment.Left(1) == "4")
                  G4comment.Delete(0, 1);

               G4key = "COMMENT";
               if (ch == '*' || ch == '$')
               {
                  make_comment();
                  break;    
               }

               if (G.Fire9000)
               {
                  while (Read_Char(Global_fp, &ch) != EOF_CHARACTER)
                  {
                     if (ch == '*' || ch == '$' || ch == 13 || ch == 10)
                     {
                        make_fire9000_comment();
                        break;
                     }
                     G4comment += Get_ReadCharString();
                  }
               }
               else
               {
                  while (Read_Char(Global_fp, &ch) != EOF_CHARACTER)
                  {
                     if (ch == '*' || ch == '$')
                     {
                        make_comment();
                        break;
                     }
                     G4comment += Get_ReadCharString();
                  }
               }
            }
            break;
         case 28:
         case 53:
         case 59:
            {
               char ch = Delimeter();
               // if token is not * ending, than do it.
               if (ch == '*' || ch == '$')
                  break;
               // get char until $ or *

               while (Read_Char(Global_fp, &ch) != EOF_CHARACTER)
               {
                  if (ch == '*' || ch == '$')
                     break;
               }
            }
            break;
         }
         break; // case G

      case 'N':
         {
            // numerator
            int num = atoi(token);
         }
         break;
      case 'X':
         G.done = FALSE;
         if (G.absolute)
            G.cur_x = cnv_tok(token);
         else
            G.cur_x += cnv_tok(token);
         break;
      case 'Y':
         G.done = FALSE;
         if (G.absolute)
            G.cur_y = cnv_tok(token);
         else
            G.cur_y += cnv_tok(token);
         break;
      case 'I': // circular interpolation x
         G.done = FALSE;  // the xy koo does not have to be repeated for a full arc
         if (token[0] == '+' || token[0] == '-')
            G.G75code = TRUE;
         i = cnv_tok(token);
         // here figure out the sign -> no sign on quadrant
         if (G.G75code)
            G.cur_i = i;
         else
            G.cur_i = fabs(i);
         break;
      case 'J':   // circular interpolation y
         G.done = FALSE;  // the xy koo does not have to be repeated for a full arc
         if (token[0] == '+' || token[0] == '-')
            G.G75code = TRUE;
         j = cnv_tok(token);
         // here figure out the sign -> no sign on quadrant
         if (G.G75code)
            G.cur_j = j;
         else
            G.cur_j = fabs(j);
         break;
      case 'K':   // circular interpolation  not needed
         break;
      case BLOCKENDCHAR:
         do_blockend();
         break;
      case 'D':
         if (atoi(token) < LOWEST_APPDCODE)  // for DELPHI we need to be able to read D5 as an aperture
         {
            // D01 is plot, D02 is move, D03 is flash.
            int dmode = atoi(token);
            if (dmode == PENFLASH && G.drawmode != PENFLASH)
            {
               // Must set drawmode here after the "IF" statement
               G.drawmode = dmode;

               // write force out everything before a flash is done.
               write_db(cur_layername, 2); // force out = 2

               G.done = (gPointArray.GetCount() == 0); // knv - Case #1020
            }
            else
            {
               G.drawmode = dmode;
            }
         }
         else
         {
            // write force out everything before a new aperture is selected
            write_db(cur_layername,1);

            //char t[80];
            G.cur_app = atoi(token);
            setApertureName(G.cur_app);

            CString renamedApertureName = apertureRenameTable.getRenamedApertureName(getApertureName());

            setApertureName(renamedApertureName);

            //strcpy(t, getApertureName());   // here I do not know how to change a CString in a function
            //get_ap_rename_name(t, &G.cur_app);  
            //setApertureName(t);
            // name will be overwritten, if changed and G.cur_app is set to Zero.

            G.cur_widthindex = Graph_Aperture(getApertureName(), 0, 0.0, 0.0, 0.0, 0.0, 0.0, G.cur_app,
                  BL_APERTURE, FALSE, &err); // ensure that aperature is in database
            cont_flag = FALSE;
            G.drawmode = PENUP;
         }
         break;
      case 'M':
         G.done = TRUE;
         break;
      default:
         // there can be other unknown commands, GerberX etc...
         break;
      }

      if (correction)   // if there was a AM correction, do set the lastch to %
         lastChar = '%';
      else
         lastChar = Delimeter();

      correction = FALSE;
   }

   // write out the last one, if one exists
   write_db(cur_layername, 1);

   // close step and repeat block
   if (steprep)
      Graph_Block_Off();
}

/******************************************************************************
* do_blockend
*/
static void do_blockend()
{
   G.tmp_arc_interpolation = G.arc_interpolation;
   // this are ignore codes
   if (G.cur_gcode != 4   &&     // remark
       G.cur_gcode != 28  &&     // remark
       G.cur_gcode != 53  &&     // remark
       G.cur_gcode != 59  &&     // remark
       G.cur_gcode != 54)        // tool select
   {
      do_koo_adjust();
      write_db(cur_layername, 0);
      G.old_x = G.cur_x;
      G.old_y = G.cur_y;
   }
   else
   {
      // I must keep G code for Arc and others
      // but not remark or tool select
      G.cur_gcode = 0;
   }
   G.cur_i = 0.0;
   G.cur_j = 0.0;
   G.sequencecnt++;
}

/******************************************************************************
* do_am
   Define a macro
*/
static void do_am(CString macro, int filenum)
{
   int i = macro.Find('*');
   if (i < 0)
      return;

   CString macroName = macro.Mid(2, i-2); // Skip AM

   macro = macro.Mid(i+1);

   // here is now
   // 1  = Circle
   // 20 = Line vector
   // 21 = Line center = rectangle
   // 22 = Line lower left
   // 4  = Outline
   // 5  = polygon
   // 6  = Moire
   // 7 =  Thermal

   if (aMacroArrayCount < MAX_MACRO)
   {
      AMacro *aMacro = &aMacroArray[aMacroArrayCount++];
      aMacro->filenum = filenum;

      aMacro->name = STRDUP(macroName);

      if (!aMacro->name)
         MemErrorMessage(__FILE__, __LINE__);

      macro.Remove(' ');
      macro.Remove('\t');
      macro.Remove('\n');

#ifdef newApertureMacroExpressionEvaluationMethod
      CApertureMacro apertureMacro(macro);
      macro = apertureMacro.getCleanMacroString();
#else
      macro = clean_comments(macro);
      clean_equations(macro);
#endif
      
      // here I need to resolve. The equations have to be exchanged to it's elements

/*
%AMMTHOLE*
1) 1,1,$1,0,0*
2) 1,0,$2,0,0*
3) $1=$1-$2*
4) $1=$1/2*
5) 21,1,$2+$1,$3,0,0,$4*
6) 21,1,$3,$2+$1,0,0,$4*%

this need to be resolved to
1)
2)
3) $1-$2
4) ($1-$2)/2
5) 21,1,$2+(($1-$2)/2),$3,0,0,$4*

  etc...
*/

      /* complex macro command
         %AMBOXS*
         $10=$1/2*$11=$2/2*$12=$10-$3*$13=$11-$4*
         4,1-$6,8,0-$12,$11,$12,$11,$10,$13,$10,0-$13,$12,0-$11,0-$12,0-$11,0-$10,0-$13,0-$10,$13,0-$12,$11,$5*%
         %ADD10BOXS,0.03300X0.03300X0.00967X0.00967X0.00X0*%
         %ADD11BOXS,0.06400X0.06400X0.01875X0.01875X0.00X0*%
      */

      char *parameterArray[20]; // 20 is maximum allowed parameters according to Gerber Description 
      int parameterArrayCount = 0;
      BOOL Exchanged = TRUE;
      CString macroCommand;

      while (Exchanged)
      {  
         i = macro.Find('*');
         CString tok;

         if (i >= 0)
         {
            tok = macro.Left(i);
            macro = macro.Mid(i+1);
         }         

         Exchanged = FALSE;

         while (tok)
         {
            if (tok.Find('$') >= 0)
            {
// when does parameter get added to parameterArray

               // here check if a paramter needs to be resolved.
               CString resolvedString = resolve_complex_macro(tok, parameterArray, parameterArrayCount, &Exchanged);
               macroCommand += resolvedString;
            }
            else
               macroCommand += tok;

            macroCommand += "*";

            i = macro.Find('*');

            if (i < 0)
            {
               Exchanged = FALSE;
               break;
            }

            tok = macro.Left(i);
            macro = macro.Mid(i+1);
         }
      }

      // free param buffer
      for (int w=0; w<parameterArrayCount; w++)
         free(parameterArray[w]);

      aMacro->command = STRDUP(macroCommand);

      if (!aMacro->command)
         MemErrorMessage(__FILE__, __LINE__);
   }
   else
   {              
      ErrorMessage("Too many macros (AM)", "Error in Gerber Read", MB_ICONHAND | MB_OK);
      return;
   }
}

/******************************************************************************
* make_comment
*/
static void make_comment()
{
   if (G4type == G4TYPE_GERBTOOL)
   {
      if (G4comment.Left(2) == "_C")
      {
         G4key = "COMMENT";
         G4comment.Delete(0,2);
         G4comment.TrimLeft();
         G4comment.TrimRight();
      }
      else if (G4comment.Left(2) == "_U")
      {
         // G4_U plt plate_base:*
         G4comment.Delete(0,2);
         G4comment.TrimLeft();
         G4comment.TrimRight();
         char  *lp, *t = STRDUP(G4comment);
         G4key = "USER";
         if (lp = strtok(t," \t"))
         {
            CString  k1;
            k1 = lp;
            if (lp = strtok(NULL," \t"))
            {
               G4key = k1;
               G4key.MakeUpper();
               G4comment = lp;
            }
         }
         free(t);
      }
      else if (G4comment.Left(2) == "_N")
      {
         G4key = ATT_NETNAME;
         G4comment.Delete(0,2);
         G4comment.TrimLeft();
         G4comment.TrimRight();
      }
   }
   else
      G4key = "COMMENT";

   if (G4comment.Right(1) == '*' || G4comment.Right(1) == '$')
      G4comment.Delete(strlen(G4comment)-1, 1);
}

/******************************************************************************
* make_fire9000_comment 
*/
static void make_fire9000_comment()
{
   CString  comment;

   comment = G4comment;
   comment.TrimLeft();
   comment.TrimRight();

   if (comment.Left(1) == '%')
      comment.Delete(0, 1);
   if (comment.Right(1) == '%')
      comment.Delete(strlen(comment)-1, 1);
      
   // ; and . are the end of a command.
   if (comment.Right(1) == '.')
      comment.Delete(strlen(comment)-1, 1);
   if (comment.Right(1) == ';')
      comment.Delete(strlen(comment)-1, 1);

   int f = comment.Find("=");

   if (f > -1)
   {
      G4key = comment.Left(f);
      comment.Delete(0,f+1);
   }
   else
   {
      G4key = comment;
   }

   // G4comment
   if (!G4key.CompareNoCase("PAR"))
   {
      // start of parameter
      G.parstart = TRUE;
   }
   else if (!G4key.CompareNoCase("EOP"))
   {
      // start of parameter
      G.parstart = FALSE;
   }
   else if (G4key.Left(3) == "APR") // syntax is APR,100
   {
      // start of parameter
      G.apstart = TRUE;
   }
   else if (!G4key.CompareNoCase("EOA"))
   {
      // start of parameter
      G.apstart = FALSE;
   }
   else if (G.apstart)
   {
      // Axxx:REC,12,34
      char  *tmp = STRDUP(comment);
      CString  apname = strtok(tmp,":");
      CString  apform = strtok(NULL,",");

      // appnames are defined as Axxx but used as Dxxx
      if(apname.Left(1) == 'A')
      {
         apname.SetAt(0,'D');
      }

      if (!apform.CompareNoCase("REC"))
      {
         int      dcode = 0;
         double   xsize=0, ysize=0, xoff=0, yoff=0;
         char     *lp;

         lp = strtok(NULL,",");
         xsize = cnv_tok(lp);
         lp = strtok(NULL,",");
         ysize = cnv_tok(lp);

         Check_Aperture(apname, T_RECTANGLE, xsize, ysize, xoff, yoff, 0, dcode, BL_APERTURE);
      }
      else if (!apform.CompareNoCase("CIR"))
      {
         int      dcode = 0;
         double   xsize=0, ysize=0, xoff=0, yoff=0;
         char     *lp;

         lp = strtok(NULL,",");
         xsize = cnv_tok(lp);

         Check_Aperture(apname, T_ROUND, xsize, ysize, xoff, yoff, 0, dcode, BL_APERTURE);
      }
      else
      {
         fprintf(flog,"Unsupported Aperture form [%s]\n", apform);
         display_error++;
         display_log = TRUE;     // make this error appear always
      }

      free(tmp);
   }
   else if (G.parstart)
   {
      if (!G4key.CompareNoCase("MODE"))
      {
         if (!comment.CompareNoCase("ABSOLUTE"))
            G.absolute = TRUE;
         else if (!comment.CompareNoCase("INCREMENTAL"))
            G.absolute = FALSE;
      }
      else if (!G4key.CompareNoCase("UNIT"))
      {
         if (!comment.CompareNoCase("INCHES"))
            unitscale = Units_Factor(UNIT_INCHES, pageUnits);
         else if (!comment.CompareNoCase("MILLIMETERS"))
            unitscale = Units_Factor(UNIT_MM, pageUnits);
      }
      else if (!G4key.CompareNoCase("ZERO"))
      {
         if (!comment.CompareNoCase("NONE"))
            G.zeroleading = 0;   // leading ommitted - must be trailing. This is normal case
         else if (!comment.CompareNoCase("LEADING"))
            G.zeroleading = 0;   // leading ommitted - must be trailing. This is normal case
         else if (!comment.CompareNoCase("TRAILING"))
            G.zeroleading = 1;   // leading ommitted - must be trailing. This is normal case
      }
      else if (!G4key.CompareNoCase("FORM"))
      {
         // here format
         G.vor_komma  = atoi(comment.Left(1));
         G.nach_komma = atoi(comment.Right(1));

         double b = GBscale;  // just debug
         GBscale = OriginalGBscale;
         for (int i=0;i<G.nach_komma;i++)
            GBscale /=10;
         b = GBscale;         // just debug
      }
      else if (!G4key.CompareNoCase("IMTP"))
      {
         if (!comment.CompareNoCase("POSITIVE"))
            cur_negative = FALSE;
         else if (!comment.CompareNoCase("NEGATIVE"))
            cur_negative = TRUE;
      }
      else if (!G4key.CompareNoCase("CROS"))
      {
         // do nothing;
      }
      else if (!G4key.CompareNoCase("XSCL"))
      {
         // do nothing;
         double sc = atof(comment);
         if (fabs(sc) != 1)
         {
            fprintf(flog,"Unsupported FIRE9000 command [%s] [%s]\n", G4key, comment);
            display_error++;
            display_log = TRUE;     // make this error appear always
         }
      }
      else if (!G4key.CompareNoCase("YSCL"))
      {
         // do nothing;
         double sc = atof(comment);
         if (fabs(sc) != 1)
         {
            fprintf(flog,"Unsupported FIRE9000 command [%s] [%s]\n", G4key, comment);
            display_error++;
            display_log = TRUE;     // make this error appear always
         }
      }
      else if (!G4key.CompareNoCase("MIRR"))
      {
         if (comment.Left(1) != 'N')
         {
            fprintf(flog,"Unsupported FIRE9000 command [%s] [%s]\n", G4key, comment);
            display_error++;
            display_log = TRUE;     // make this error appear always
         }
      }
      else if (!G4key.CompareNoCase("XOFS"))
      {
         double o = atof(comment);
         if (fabs(o) != 0)
         {
            fprintf(flog,"Unsupported FIRE9000 command [%s] [%s]\n", G4key, comment);
            display_error++;
            display_log = TRUE;     // make this error appear always
         }

      }
      else if (!G4key.CompareNoCase("YOFS"))
      {
         double o = atof(comment);
         if (fabs(o) != 0)
         {
            fprintf(flog,"Unsupported FIRE9000 command [%s] [%s]\n", G4key, comment);
            display_error++;
            display_log = TRUE;     // make this error appear always
         }

      }
      else if (!G4key.CompareNoCase("FSZE"))
      {
         // do nothing
      }
      else if (!G4key.CompareNoCase("NEXT"))
      {
         // do nothing
      }
      else
      {
         fprintf(flog,"Unknown FIRE9000 command [%s] [%s]\n", G4key, comment);
         display_error++;
         display_log = TRUE;     // make this error appear always
      }
   }
}

#ifndef newApertureMacroExpressionEvaluationMethod
/******************************************************************************
* clean_comments
*/
static CString clean_comments(const CString& string)
{
   CSupString macroString(string);
   CStringArray primitives;

   macroString.Parse(primitives,"*");

   CString retval;

   for (int ind = 0;ind < primitives.GetSize();ind++)
   {
      CString primitive = primitives.GetAt(ind);

      if (primitive.GetLength() > 0)
      {
         if (primitive.Left(1) != "0")
         {
            retval += primitive + "*";
         }
      }
   }

   return retval;
}
#endif

//_____________________________________________________________________________
CApertureMacro::CApertureMacro(const CString& macroString)
: m_macroString(macroString)
{
   parse();
}

void CApertureMacro::parse()
{
   CSupString macroStatementsString(m_macroString);
   macroStatementsString.Parse(m_statements,"*");

   for (int index = 0;index < m_statements.GetCount();index++)
   {
      CSupString statement = m_statements.GetAt(index);

      if (statement.GetLength() > 0)
      {
         if (statement.Left(1) == "0")
         {  // comment
            continue;
         }

         CStringArray equationParts;
         statement.Parse(equationParts,"=");

         if (equationParts.GetCount() == 2)
         {  // equation
            CString variableName = equationParts.GetAt(0);
            CString expression   = equationParts.GetAt(1);

            CString expressionValue = evaluateExpression(expression,variableName);

            m_symbolTable.SetAt(variableName,expressionValue);
         }
         else
         {  // primitive
            CString primitive = evaluateExpression(statement); 
            m_primitives.Add(primitive);
         }
      }
   }
}

CString CApertureMacro::evaluateExpression(const CString& expression,const CString& variableName)
{
   CString expressionValue(expression);
   CString symbolName,symbolValue;

   for (POSITION pos = m_symbolTable.GetStartPosition();pos != NULL;)
   {
      m_symbolTable.GetNextAssoc(pos,symbolName,symbolValue);

      if (symbolName != variableName)
      {
         expressionValue.Replace(symbolName,symbolValue);
      }
   }

   return expressionValue;
}

CString CApertureMacro::getCleanMacroString()
{
   CString cleanMacroString;

   for (int index = 0;index < m_primitives.GetCount();index++)
   {
      cleanMacroString += m_primitives.GetAt(index) + "*";
   }

   return cleanMacroString;
}

#ifndef newApertureMacroExpressionEvaluationMethod
/******************************************************************************
* clean_equations
%AMMTHOLE*
1) 1,1,$1,0,0*
2) 1,0,$2,0,0*
3) $1=$1-$2*
4) $1=$1/2*
5) 21,1,$2+$1,$3,0,0,$4*
6) 21,1,$3,$2+$1,0,0,$4*%

this need to be resolved to
1)
2)
3) $1-$2
4) ($1-$2)/2
5) 21,1,$2+(($1-$2)/2),$3,0,0,$4*

  etc...
*/
static void clean_equations(CString& equation)
{
   // here I need to resolve. The equations have to be exchanged to it's elements
   int i;
   int equationfound = FALSE;
   char *a = STRDUP(equation);
   char *lp = strtok(a,"*");
   mcnt = 0;

   while (lp)
   {
      GerbMStruct *c = new GerbMStruct;
      marray.SetAtGrow(mcnt,c);
      mcnt++;
      c->left = "";
      c->right = lp;
      c->equation = 0;
      if (strchr(lp, '='))
      {
         equationfound = TRUE;
         c->equation = 1;
      }
      lp = strtok(NULL,"*");
   }

   if (equationfound)
   {
      // set left and right
      for (i=0; i<mcnt; i++)
      {
         if (marray[i]->equation)
         {
            strcpy(a, marray[i]->right);
            lp = strtok(a, "=");
            marray[i]->left = lp;
            lp = strtok(NULL, "\0");
            marray[i]->right = lp;
         }
      }

      int equfound = TRUE;
      while (equfound)
      {
         equfound = FALSE;
         for (i=0; i<mcnt; i++)
         {
            if (marray[i]->equation)
            {
               CString currentLeft, currentRight;
               currentLeft.Format("%s", marray[i]->left);
               currentRight.Format("(%s)", marray[i]->right);
               // now look forward to see if the left of the current equation matches 
               // any variable in the right of the other equations.  If it deose, then
               // replace them.  And remove the current equation.
               for (int k=i+1; k<mcnt; k++)
               {
                  marray[k]->right.Replace(currentLeft, currentRight);
                  if (marray[k]->equation)   
                     equfound = TRUE; // Indicate there is still more equation
                     //break; // Removed because it will break before looking through all other equation. By Lynn 03/21/02
               }
               marray.RemoveAt(i);
               mcnt--;
            }
         }
      }

      equation = "";
      for (i=0; i<mcnt; i++)
      {
         equation += marray[i]->right;
         equation += "*";
      }
   }

   // free array
   for (i=0; i<mcnt; i++)
      delete marray[i];
   mcnt = 0;
   free(a);
}
#endif

/******************************************************************************
* change_to_controlcode
   a read \t in 2 char but a '\t' char is 0x09
*/
static void change_to_controlcode(CString& string)
{
   const char* l = string;
   char  tmp[200];
   int   tmpcnt = 0;

   for (unsigned int i=0;i<strlen(l);i++)
   {
      if (l[i] == '\\')
      {
         // here start of control char
         i++;
         if (l[i] == 'a')
         {
            tmp[tmpcnt] = '\a';
            tmpcnt++;
         }
         else if (l[i] == 'b')
         {
            tmp[tmpcnt] = '\b';
            tmpcnt++;
         }
         else if (l[i] == 'f')
         {
            tmp[tmpcnt] = '\f';
            tmpcnt++;
         }
         else if (l[i] == 'n')
         {
            tmp[tmpcnt] = '\n';
            tmpcnt++;
         }
         else if (l[i] == 'r')
         {
            tmp[tmpcnt] = '\r';
            tmpcnt++;
         }
         else if (l[i] == 't')
         {
            tmp[tmpcnt] = '\t';
            tmpcnt++;
         }
         else if (l[i] == 'v')
         {
            tmp[tmpcnt] = '\v';
            tmpcnt++;
         }
         else if (l[i] == '\\')
         {
            tmp[tmpcnt] = '\\';
            tmpcnt++;
         }
      }
      else
      {
         tmp[tmpcnt] = l[i];        
         tmpcnt++;
      }
   }

   tmp[tmpcnt] = '\0';

   string = tmp;

   return;
}

/****************************************************************************
*  true returned if an existing ap_rename entry is renamed
*/
//static bool rename_app(const char *newname, const char *orgname)
//{
//   bool retval = false;
//
//   for (int i=0; i<ap_renamecnt; i++)
//   {
//      if (strcmp(ap_rename[i].orig,orgname) == 0)
//      {
//         strcpy(ap_rename[i].name,newname);
//
//         retval = true;
//
//         break;
//      }
//   }
//
//   if (!retval)
//   {
//      if (ap_renamecnt < MAX_AP_RENAME)
//      {
//         if ((ap_rename[ap_renamecnt].name = STRDUP(newname)) == NULL)
//            MemErrorMessage(__FILE__, __LINE__);
//         if ((ap_rename[ap_renamecnt].orig = STRDUP(orgname)) == NULL)
//            MemErrorMessage(__FILE__, __LINE__);
//         ap_renamecnt++;
//      }
//      else
//      {
//         ErrorMessage("Too many aperture renames");
//         // error - too many ap_renames - so rename is ignored.
//      }
//   }
//
//   return retval;
//}

/******************************************************************************
* Check_Aperture
   Do not overwrite an existing aperture
*/
static int Check_Aperture(const char *name, int apptyp, double sizea, double sizeb, double xc, double yc, double rot, int appcode, DbFlag flg)
{
   int   code, err;
   CString  tmp;
   int   errcnt = 0;
   int   changed = FALSE;
   int   dcode = appcode;

   tmp = name;

   if (apptyp == T_THERMAL)
      rot -= DegToRad(45); // '+' Thermal is default in Gerber, but 'x' Thermal is default in CAMCAD

   while (TRUE)
   {
      code = Graph_Aperture(tmp, apptyp, sizea, sizeb, xc, yc, rot, dcode, flg, FALSE, &err);

      if (err)
      {
         tmp.Format("%s_%d", name, ++errcnt);
         changed = TRUE;
         dcode = 0;
      }
      else
      {
         break;
      }
   }

   if (changed)   // update ap_rename list
   {
      apertureRenameTable.renameAperture(tmp, name);
   }

   return code;
}

/******************************************************************************
* Check_ComplexAperture
   Do not overwrite an existing aperture
*/
//static int Check_ComplexAperture(const CString& apertureName,int dcode,const CString& subBlockName)
//{
//   int   code, err;
//   CString  tmp;
//   int   errcnt = 0;
//   int   changed = FALSE;
//   int   dcode = appcode;
//
//   tmp = name;
//
//   if (apptyp == T_THERMAL)
//      rot -= DegToRad(45); // '+' Thermal is default in Gerber, but 'x' Thermal is default in CAMCAD
//
//   while (TRUE)
//   {
//      code = Graph_Complex(tmp,dcode, apptyp, sizea, sizeb, xc, yc, rot, dcode, flg, FALSE, &err);
//      if (err)
//      {
//         tmp.Format("%s_%d",name,++errcnt);
//         changed = TRUE;
//         dcode = 0;
//      }
//      else
//         break;
//   }
//
//   if (changed)   // update ap_rename list
//   {
//      apertureRenameTable.renameAperture(tmp, name);
//   }
//
//   return code;
//}

/******************************************************************************
* get_macroptr
*/
static int get_macroptr(CString macroName, int fileNum)
{
   for (int i=0; i<aMacroArrayCount; i++)
   {
      AMacro *aMacro = &aMacroArray[i];
      if (!macroName.CompareNoCase(aMacro->name) && aMacro->filenum == fileNum)
         return i;
   }

   return -1;
}

/****************************************************************************
*
*/
static double do_operation(double result, char *tmp, int lastop)
{
   double res = 0;

   if (strlen(tmp) == 0)   return result;

   switch (lastop)
   {
   case OP_ASSIGN:
      res = atof(tmp);
      break;
   case OP_ADD:
      res = result + atof(tmp);
      break;
   case OP_MINUS:
      res = result - atof(tmp);
      break;
   case OP_MUL:
      res = result * atof(tmp);
      break;
   case OP_DIV:
      res = result / atof(tmp);
      break;
   }

   return res;
}

/****************************************************************************
*
   allow
   + add
   - minus
   / divide
   x multiple
*/
static double calc_param(const char* p1,CDoubleArray& param)
{
   double   result = 0;

   // check that we do not support and equal sign
   if (strchr(p1,'='))
   {
      fprintf(flog,"Marco Parameter [=] in [%s] not supported\n",p1);
      display_error++;

      return 0.0;
   }

   static   CString  res;
   CString  par;
   int      parammode = FALSE;
   res = "";
   par = "";

   for (unsigned int w=0; w<strlen(p1); w++)
   {
      if (p1[w] == '$')
      {
         parammode = TRUE;
         par = "";
      }
      else if (strchr("-X/+(),",p1[w]))
      {
         // here now check if param is ok or needs to be resolved
         if (strlen(par))
         {
            int w = atoi(par);

            if (w < 1 || w > param.GetCount())
            {
               fprintf(flog,"Scan error in Marco Parameter [%s] at position [%d]\n", p1, w);
               display_error++;
               return 0.0;
            }

            CString  num;
            num.Format("%lg",param[w-1]);
            res += num;
         }

         par = "";
         parammode = FALSE;
      }

      if (!parammode)
         res += p1[w];
      else if (p1[w] != '$')
         par += p1[w];
   }

   if (strlen(par))
   {
      int w = atoi(par);
      
      if (w < 1 || w > param.GetCount())
      {
         fprintf(flog,"Scan error in Marco Parameter [%s] at position [%d]\n", p1, w);
         display_error++;
         return 0.0;
      }

      CString  num;
      num.Format("%lg",param[w-1]);
      res += num;
   }
   
   res.Replace('X','*');   // in Gerber mulitplier is a X in Arithmatics it is a *

   int a, ec;
   char  *res1 = STRDUP(res); // evaluate needs char*, not const char*

   if( (ec = EE_Evaluate( res1, &result, &a )) != E_OK )
   {
      ErrorMessage("Arithmetic Function Error!");
   }

   free(res1);

   return result;
}

/****************************************************************************
*
   Define an aperature
   Either a simple aperature
      ROUND
      RECTANGLE
      OVAL
      - POLYGON
      - Special Macro
   Simple aperatures are defined immediately and not stored.
*/
int do_ad(char *m, int filenum)
{
   CString apertureDefinitionString(m);

   double *nCommand = NULL; 

   char     *lp;
   int      ncnt = 0, dccnt = 0;
   int      mcnt = 0;
   int      prim,onoff;
   int      appform;
   int      negative = FALSE;
   int      err;
   char     dd[MAX_LINE];
   char     dc[MAX_LINE];
   char     macroname[MAX_LINE];
   CDoubleArray param;
   int      float_layer = Graph_Level("0","",1);

   strcpy(dd,&(m[3])); // AD D #

   // there is always an * on the end.
   if (dd[strlen(dd)-1] == '*' || dd[strlen(dd)-1] == '$')
   {
      dd[strlen(dd)-1] = '\0';
   }

   if (m[2] != 'D')  
   {
      return 0;   // this must be a D Code
   }

   STRREV(dd); // get dcode by reversing and from the end until char

   while (strlen(dd) && isdigit(dd[strlen(dd)-1]))
   {
      dc[dccnt++] = dd[strlen(dd)-1];
      dd[strlen(dd)-1] = '\0';
   }

   dc[dccnt] = '\0';
   STRREV(dd);
   int dCode = atoi(dc);
   G.cur_app = dCode;

   //if (dCode == 66)
   //{
   //   int iii = 3;
   //}

   setApertureName(G.cur_app);

   bool macroFoundFlag = false;

#ifdef newMacroProcessing

   bool standardApertureFlag;

   if (apertureDefinitionString.GetLength() > 0 && apertureDefinitionString.Right(1) == "*")
   {
      apertureDefinitionString.GetBufferSetLength(apertureDefinitionString.GetLength() - 1);
   }

   CString macroString = CGerberMacros::getGerberMacros().getMacroPrimitivesString(apertureDefinitionString,filenum,standardApertureFlag);
   CGerberMacroCommands gerberMacroCommands(macroString);
   mcnt = gerberMacroCommands.getCount();

   macroFoundFlag = !standardApertureFlag;

   lp = strtok(dd, ",");

   sprintf(macroname, "%s", lp);

#else // newMacroProcessing

#ifndef newMacroCommandParseMethod
   if ((nCommand = (double *) calloc(MAX_MACROCOMMAND, sizeof(double))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
#endif // newMacroCommandParseMethod

   if ((lp = strtok(dd, ",")) == NULL)   
   {
#ifndef newMacroCommandParseMethod
      free (nCommand);
#endif // newMacroCommandParseMethod

      return 0;
   }

   sprintf(macroname, "%s", lp);

   // here check if macro or primitive
   int aMacroIndex = get_macroptr(macroname, filenum);

   macroFoundFlag = (aMacroIndex > -1);

#endif // newMacroProcessing

   if (macroFoundFlag)
   {

#ifndef newMacroProcessing

      AMacro *aMacro = &aMacroArray[aMacroIndex];

      // here is a macro found
      while (lp = strtok(NULL, "X \t"))
         param.Add(atof(lp));

#ifdef newMacroCommandParseMethod
      CGerberMacroCommands gerberMacroCommands(aMacro->command);
      mcnt = gerberMacroCommands.getCount();
#else // newMacroCommandParseMethod

      char *mdd = STRDUP(aMacro->command);

      // here cnt how many forms are in the list
      mcnt = 0;
      unsigned int len = strlen(mdd);

      for (unsigned int i=0; i<len; i++)
      {
         if (mdd[i] == '*') // can not be $, because this is a parameter
            mcnt++;
      }

      char *tt = mdd;

      while ((lp = strtok(tt,"*,")) != NULL && ncnt < MAX_MACROCOMMAND)
      {
         tt = NULL;
         // max 100000 entries allowed
         if (ncnt < MAX_MACROCOMMAND)
         {
            // parameter $1, $1-$, 0-$1-%2
            // allowed is * (multiply)
            // + add, - minus, / divide
            if (strchr(lp,'$'))  // if a placeholder is part of it, do calc
            {
               
               if (strchr(lp, '='))
               {
                  CString index = lp[1];  // if there is a equation to change the parameter, change the parameter value. ex. $3=$3X2
                  int paramindex = atoi(index) - 1;
                  CString line = lp;
                  strcpy(lp, line.Mid(line.Find("=") + 1));
                  param.SetAtGrow(paramindex,calc_param(lp, param));
               }
               else
               {
                  nCommand[ncnt] = calc_param(lp, param);
                  ncnt++;
               }
            }
            else
            {
               nCommand[ncnt] = atof(lp);
               ncnt++;
            }
         }
         else
         {
            ErrorMessage("Too many macros" , "Error in Gerber Read", MB_ICONHAND | MB_OK);
            free(nCommand);

            return 0;
         }
      }

      free(mdd);  // free temp macro buffer.
#endif // newMacroCommandParseMethod

#endif // newMacroProcessing

      // this is only temporary fix.
      // if a line vector 20 is centered and has a circle on each end convert
      // it to line center 21
      double cd1,cx1,cy1; // circle 1
      double cd2,cx2,cy2; // circle 2
      int ccnt = 0, lcnt = 0;  // circle cnt, line cnt
      double lw, lx1,ly1,lx2,ly2,rot; // line 20

#ifdef newMacroCommandParseMethod

      for (int commandIndex = 0;commandIndex < gerberMacroCommands.getCount();commandIndex++)
      {
         CGerberMacroCommand* macroCommand = gerberMacroCommands.getAt(commandIndex);

         CString commandString = macroCommand->getCommand();

         macroCommand->setVariableParams(param);
         int paramCount = macroCommand->getCount();

         if (paramCount > 2)
         {
            int primitive = macroCommand->getPrimitive();
            bool onFlag   = macroCommand->getOnFlag();

            switch (primitive)
            {
            case 1:
               if (ccnt == 0)
               {
                  cd1 = macroCommand->getAt(2);
                  cx1 = macroCommand->getAt(3);
                  cy1 = macroCommand->getAt(4);
               }
               else if (ccnt == 1)
               {
                  cd2 = macroCommand->getAt(2);
                  cx2 = macroCommand->getAt(3);
                  cy2 = macroCommand->getAt(4);
               }

               ccnt++;
               break;
            case 20:
               {
                  lw  = macroCommand->getAt(2);
                  lx1 = macroCommand->getAt(3);
                  ly1 = macroCommand->getAt(4);
                  lx2 = macroCommand->getAt(5);
                  ly2 = macroCommand->getAt(6);
                  rot = macroCommand->getAt(7);

                  for (int r=0; r<rot/90; r++)
                  {
                     double t = lx1;
                     lx1 = -ly1;
                     ly1 = t;
                     t   = lx2;
                     lx2 = -ly2;
                     ly2 = t;
                  }

                  lcnt++;
               }

               break;
            }
         }
      }

#else
      for (int nn=0; nn<ncnt; nn++)
      {
         prim = round(nCommand[nn]);
         nn++;
         onoff =round(nCommand[nn]);

         if (onoff) 
             negative = FALSE;
         else
             negative = TRUE;

         nn++;

         switch (prim)
         {
         case 1:
            if (ccnt == 0)
            {
               cd1 = nCommand[nn];
               cx1 = nCommand[nn+1];
               cy1 = nCommand[nn+2];
            }
            else if (ccnt == 1)
            {
               cd2 = nCommand[nn];
               cx2 = nCommand[nn+1];
               cy2 = nCommand[nn+2];
            }

            ccnt++;
            nn +=2;
            break;
         case 20:
            {
               lw = nCommand[nn];
               lx1 = nCommand[nn+1];
               ly1= nCommand[nn+2];
               lx2= nCommand[nn+3];
               ly2= nCommand[nn+4];
               rot=nCommand[nn+5];

               for (int r=0; r<rot/90; r++)
               {
                  double t=lx1;
                  lx1=-ly1;
                  ly1=t;
                  t=lx2;
                  lx2=-ly2;
                  ly2=t;
               }

               lcnt++;
               nn += 5;
            }
            break;
         case 22: // line lower left
            nn +=4;
            break;
         case 4:  // outline
            nn += round(nCommand[nn]) * 2 + 3;
            break;
         case 5:  // polygon
            nn += 6;   // ?? i dont know
            break;
         case 6:  // moire
            nn += 10;   // ?? i dont know
            break;
         case 7:
            nn += 4;
            break;
         case 21:
            nn +=4;
            break;
         }  // switch
      }
#endif

#ifdef newMacroCommandParseMethod

      // here is a line 20 with circles on both ends - this is optimized to a oblong
      if (ccnt == 2 && lcnt == 1 && cd1 == cd2 && cd1 == lw)
      {
         if (ly1 == 0.0 && ly2 == 0.0 && (lx1 + lx2) == 0.0 && cy1 == 0.0 && cy2 == 0.0)
         {
            // horizontal element
            if ((cx1 == lx1 || cx2 == lx1) && (cx2 == lx1 || cx2 == lx2))
            {
               // thats it
               gerberMacroCommands.empty();
               CGerberMacroCommand* command = gerberMacroCommands.getAt(0);

               command->setAt(0,-21); // this is our oblong
               command->setAt(1,1);   //
               command->setAt(2,cd1 + fabs(lx2 - lx1)); // width
               command->setAt(3,cd1);       // height
               command->setAt(4,0.0);       // centerx
               command->setAt(5,0.0);       // center y
               command->setAt(6,0.0);       // rotation
               mcnt = 1;
            }
         }
         else if (lx1 == 0.0 && lx2 == 0.0 && (ly1 + ly2) == 0.0 && cx1 == 0.0 && cx2 == 0.0)
         {
            // vertical element
            if ((cy1 == ly1 || cy2 == ly1) && (cy2 == ly1 || cy2 == ly2))
            {
               // thats it
               gerberMacroCommands.empty();
               CGerberMacroCommand* command = gerberMacroCommands.getAt(0);

               command->setAt(0,-21);    // this is our oblong - 
               command->setAt(1,1);                           
               command->setAt(2,cd1);    // width
               command->setAt(3,cd1 + fabs(ly2-ly1));   // height
               command->setAt(4,0.0);    // centerx
               command->setAt(5,0.0);    // centery
               command->setAt(6,0.0);    // rotation
               mcnt = 1;
            }
         }
         else
         {
            // did not qualify
         }
      }
#else

      // here is a line 20 with circles on both ends - this is optimized to a oblong
      if (ccnt == 2 && lcnt == 1 && cd1 == cd2 && cd1 == lw)
      {
         if (ly1 == 0.0 && ly2 == 0.0 && (lx1 + lx2) == 0.0 && cy1 == 0.0 && cy2 == 0.0)
         {
            // horizontal element
            if ((cx1 == lx1 || cx2 == lx1) && (cx2 == lx1 || cx2 == lx2))
            {
               // thats it
               nCommand[0] = -21; // this is our oblong
               nCommand[1] = 1;   //
               nCommand[2] = cd1 + fabs(lx2 - lx1); // width
               nCommand[3] = cd1;       // height
               nCommand[4] = 0.0;       // centerx
               nCommand[5] = 0.0;       // center y
               nCommand[6] = 0.0;       // rotation
               ncnt = 7;
               mcnt = 1;
            }
         }
         else if (lx1 == 0.0 && lx2 == 0.0 && (ly1 + ly2) == 0.0 && cx1 == 0.0 && cx2 == 0.0)
         {
            // vertical element
            if ((cy1 == ly1 || cy2 == ly1) && (cy2 == ly1 || cy2 == ly2))
            {
               // thats it
               nCommand[0] = -21;    // this is our oblong - 
               nCommand[1] = 1;                           
               nCommand[2] = cd1;    // width
               nCommand[3] = cd1 + fabs(ly2-ly1);   // height
               nCommand[4] = 0.0;    // centerx
               nCommand[5] = 0.0;    // centery
               nCommand[6] = 0.0;    // rotation
               ncnt = 7;
               mcnt = 1;
            }
         }
         else
         {
            // did not qualify
         }
      }
#endif

#ifdef newMacroCommandParseMethod
      // here optimize a donut, which are 2 circle one on other off
      appform = T_ROUND;

      //    0     1 2 3 4    5     6 7 8 9
      // prim,onOff,d,x,y,prim,onOff,d,x,y,

      if (gerberMacroCommands.getCount() == 2 &&
          gerberMacroCommands.getAt(0)->getPrimitive() == 1 &&
          gerberMacroCommands.getAt(1)->getPrimitive() == 1 &&
          gerberMacroCommands.getAt(0)->getOnFlag()    &&
          gerberMacroCommands.getAt(1)->getOffFlag()   &&
          gerberMacroCommands.getAt(0)->getAt(2) == 0. &&
          gerberMacroCommands.getAt(1)->getAt(2) == 0. &&
          gerberMacroCommands.getAt(0)->getAt(3) == 0. &&
          gerberMacroCommands.getAt(1)->getAt(3) == 0. &&
          gerberMacroCommands.getAt(0)->getAt(4) == 0. &&
          gerberMacroCommands.getAt(1)->getAt(4) == 0.    )
      {
         // here donut found
         mcnt = 1;
         appform = T_DONUT;
      }
      // here optimize for a thermo
      else if (!macro_thermals && 
               gerberMacroCommands.getCount() == 4 &&
               gerberMacroCommands.getAt(0)->getPrimitive() == 1  &&
               gerberMacroCommands.getAt(1)->getPrimitive() == 1  &&
               gerberMacroCommands.getAt(2)->getPrimitive() == 21 &&
               gerberMacroCommands.getAt(3)->getPrimitive() == 21 &&
               gerberMacroCommands.getAt(0)->getOnFlag()  &&
               gerberMacroCommands.getAt(1)->getOffFlag() &&
               gerberMacroCommands.getAt(2)->getOffFlag() &&
               gerberMacroCommands.getAt(3)->getOffFlag()    )
      {
         mcnt = 1;
         appform = T_THERMAL;
      }
      else if (!macro_thermals && 
               gerberMacroCommands.getCount() > 0 &&
               gerberMacroCommands.getAt(0)->getAt(0) == 6.)
      {
         mcnt = 1;
         appform = T_TARGET;
      }

#else // newMacroCommandParseMethod
      // here optimize a donut, which are 2 circle one on other off
      appform = T_ROUND;

      //    0     1 2 3 4    5     6 7 8 9
      // prim,onOff,d,x,y,prim,onOff,d,x,y,

      if (ncnt == 10 && nCommand[0] == 1 && nCommand[1] == 1 && nCommand[5] == 1 && nCommand[6] == 0 && 
          nCommand[3] == 0 && nCommand[4] == 0 && nCommand[8] == 0 && nCommand[9] == 0) // also check offset
      {
         // here donut found
         mcnt = 1;
         appform = T_DONUT;
      }
      // here optimize for a thermo
      else if (!macro_thermals &&     // Setting from user
          ncnt == 24 && nCommand[0] == 1 && nCommand[1] == 1 && nCommand[5] == 1 && nCommand[6] == 0 &&
          nCommand[10] == 21 && nCommand[11] == 0 && nCommand[17] == 21 && nCommand[18] == 0)
      {
         mcnt = 1;
         appform = T_THERMAL;
      }
      else if (!macro_thermals && nCommand[0] == 6) // here optimize for a moire / target
      {
         mcnt = 1;
         appform = T_TARGET;
      }
#endif // newMacroCommandParseMethod

#ifdef newMacroCommandParseMethod

      CDoubleArray& nCommand = gerberMacroCommands.getCommandParameters();
      ncnt = nCommand.GetCount();
#endif // newMacroCommandParseMethod


      if (ncnt > 0)
      {
         // here write aperature
         // if mcnt == 1 this is a simple app, otherwise complex
         CString  mmacroname;    // make sure macroname is not equal to app name
         // two macros can be identical names but different in 2 files.
         // this is not doable with complex apertures

         // make a unique complex aperture block.
         int mnamecnt = 0;

         while (TRUE)
         {
            mmacroname.Format("D%d_%s_%d", G.cur_app, macroname, ++mnamecnt );
            if (!Graph_Block_Exists(doc, mmacroname, -1))
               break;
         }

         if (mcnt > 1)
         {
            Graph_Block_On(GBO_APPEND, mmacroname, -1, BL_GLOBAL);   // generate sub block
         }

         for (int nn=0; nn<ncnt; nn++)
         {
            prim = round(nCommand[nn]);
            nn++;
            onoff = round(nCommand[nn]);

            if (onoff) 
                negative = FALSE;
            else
                negative = TRUE;

            nn++;

            switch (prim)
            {
            case 1:  // circle ONOFF diameter xcenter ycenter
               if (mcnt == 1)
               {
                  // simple
                  if (appform == T_ROUND)
                  {
                     Check_Aperture(getApertureName(), T_ROUND,
                           unitscale*nCommand[nn], 0.0,
                           unitscale*nCommand[nn+1],unitscale*nCommand[nn+2],0.0,
                           G.cur_app, BL_APERTURE);
                  }
                  else if (appform == T_THERMAL)
                  {
                     int code = Check_Aperture(getApertureName(), T_THERMAL, unitscale*nCommand[2], unitscale*nCommand[7],
                           0, 0, 0, G.cur_app, BL_APERTURE);
                     BlockStruct *bb = doc->getWidthTable()[code];
                     bb->setSizeC((DbUnit)(unitscale*nCommand[13])); // gapsize
                     bb->setSizeD((DbUnit)DegToRad(nCommand[16]));   // spoke angle
                     bb->setSpokeCount(4);           // number of spokes
                  }
                  else
                  {
                     Check_Aperture(getApertureName(), T_DONUT, unitscale*nCommand[nn], unitscale*nCommand[nn+5], 0, 0, 0,
                           G.cur_app, BL_APERTURE);
                  }
                  // finish the for loop
                  nn = ncnt;
               }
               else
               {
                  // complex
                  double   x = nCommand[nn+1];
                  double   y = nCommand[nn+2];
                  if (G.width0index < 0)
                  {
                     int err;
                     G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                  }
                  Graph_Circle(float_layer, unitscale*x, unitscale*y, unitscale*nCommand[nn]/2, 0, G.width0index,
                        negative, TRUE);  // filled
               }
               nn +=2;
               break;
            case 20: // line vector
               {
                  double x,y;
                  double w   = nCommand[nn];
                  double x1  = nCommand[nn+1];
                  double y1  = nCommand[nn+2];
                  double x2  = nCommand[nn+3];
                  double y2  = nCommand[nn+4];
                  double rot = nCommand[nn+5];
                  
                  // this is always a complex aperature
                  // this needs to be fixed.
                  if (mcnt == 1)
                  {
                     double h1 = w;
                     double w1 = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
                     double offx = 0;  //h1 / 2;
                     double offy = 0;  //w1 / 2;

                     double angle = ArcTan2(y2-y1, x2-x1);
                     rot += RadToDeg(angle);

                     //double h1,w1, offx = 0, offy = 0;
                     //if ((x2-x1) < 0.001)
                     //{
                     //   w1 = w;
                     //   h1 = fabs(y2-y1);
                     //   offy = y1 + (y2-y1)/2;
                     //}
                     //else
                     //{
                     //   w1 = fabs(x2-x1);
                     //   h1 = w;
                     //   offx = x1 + (x2-x1)/2;
                     //}

                     Check_Aperture(getApertureName(), T_RECTANGLE, unitscale*w1, unitscale*h1,
                           unitscale*offx, unitscale*offy, DegToRad(rot), G.cur_app, BL_APERTURE);
                  }
                  else
                  {
                     double angle = ArcTan2(y2-y1, x2-x1);
                     double sine = sin(angle);
                     double cosine = cos(angle);
                     double len = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
   
                     if (len <= w)  // if length is smaller than the endcap width, make a poly
                     {
                        Point2 p1a,p1b,p2a,p2b;
                        Point2 p1, p2;

                        p1.x = x1;
                        p1.y = y1;
                        p2.x = x2;
                        p2.y = y2;
                        FindCorners(w, w, &p1, &p2, &p1a, &p1b, &p2a, &p2b);
                        Graph_PolyStruct(float_layer,  0L, negative);

                        if (G.width0index < 0)
                        {
                           int err;
                           G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                        }
                        Graph_Poly(NULL, G.width0index, TRUE, FALSE, TRUE);
                        Graph_Vertex(p1a.x*unitscale, p1a.y*unitscale, 0.0);    // p1 coords
                        Graph_Vertex(p1b.x*unitscale, p1b.y*unitscale, 0.0);   // p1 coords
                        Graph_Vertex(p2b.x*unitscale, p2b.y*unitscale, 0.0);   // p1 coords
                        Graph_Vertex(p2a.x*unitscale, p2a.y*unitscale, 0.0);   // p1 coords
                        Graph_Vertex(p1a.x*unitscale, p1a.y*unitscale, 0.0);    // p1 coords
                     }
                     else
                     {
                        x1 = x1 + cosine*w/2; // + sine*w;
                        y1 = y1 + sine*w/2; // + cos;
                        x2 = x2 - cosine*w/2;
                        y2 = y2 - sine*w/2;

                        Rotate(x1, y1, rot, &x, &y); 
                        x1= x; y1 = y;
                        Rotate(x2, y2, rot, &x, &y); 
                        x2= x; y2 = y;

                        // complex
                        Graph_Line(float_layer, unitscale*x1, unitscale*y1, unitscale*x2, unitscale*y2, 0,
                              Graph_Aperture("", T_SQUARE, unitscale * w, unitscale * w,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err),
                              negative);
                     }
                  }
                  nn +=5;
               }
               break;
            case -21:// This is an RSI definition for OBLONG - 
            case 21: // line center is a rectangle defined and not a line
                     // vector
               {
                  double w = nCommand[nn];
                  double h = nCommand[nn+1];
                  double rot=nCommand[nn+4];

                  if (mcnt == 1)
                  {
                     // simple
                     if (w == h)
                     {
                        if (prim < 0)
                        {
                           Check_Aperture(getApertureName(), T_ROUND, unitscale*w, 0.0, 0.0, 0.0, DegToRad(rot), 
                                 G.cur_app, BL_APERTURE);
                        }
                        else
                        {
                           Check_Aperture(getApertureName(), T_RECTANGLE, unitscale*w, unitscale*w, 0.0, 0.0, 
                                 DegToRad(rot), G.cur_app, BL_APERTURE);
                        }
                     }
                     else
                     {
                        Check_Aperture(getApertureName(), (prim < 0)?T_OBLONG:T_RECTANGLE, unitscale*w, unitscale*h,
                              0.0, 0.0, DegToRad(rot), G.cur_app, BL_APERTURE);
                     }
                  }
                  else
                  {
                     // complex
                     // 1 = width
                     // 2 = height
                     // 3 = xcenter
                     // 4 = ycenter

                     double halfWidth       = nCommand[nn  ]/2.;
                     double halfHeight      = nCommand[nn+1]/2.;
                     double xCenter         = nCommand[nn+2];
                     double yCenter         = nCommand[nn+3];
                     double rotationDegrees = nCommand[nn+4];

                     Graph_PolyStruct(float_layer, 0, negative);

                     if (G.width0index < 0)
                     {
                        int err;
                        G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                     }

                     CPoly* poly = Graph_Poly(NULL, G.width0index, TRUE, FALSE, TRUE);

                     Graph_Vertex(-halfWidth,-halfHeight,0.);
                     Graph_Vertex(-halfWidth, halfHeight,0.);
                     Graph_Vertex( halfWidth, halfHeight,0.);
                     Graph_Vertex( halfWidth,-halfHeight,0.);
                     Graph_Vertex(-halfWidth,-halfHeight,0.);

                     CTMatrix matrix;

                     if (s_optionPrimitive21TransformOrderRotateFirst)
                     {
                        matrix.rotateDegrees(rotationDegrees);
                        matrix.translate(xCenter,yCenter);
                     }
                     else
                     {
                        matrix.translate(xCenter,yCenter);
                        matrix.rotateDegrees(rotationDegrees);
                     }

                     matrix.scale(unitscale);

                     poly->transform(matrix);
                  }

                  nn +=4;
               }
               break;
            case 22: // line lower left
               {
                  //double w = nCommand[nn];
                  //double h = nCommand[nn+1];
                  //double rot=nCommand[nn+4];

                  double width           = nCommand[nn+0];
                  double height          = nCommand[nn+1];
                  double xMin            = nCommand[nn+2];
                  double yMin            = nCommand[nn+3];
                  double rotationDegrees = nCommand[nn+4];

                  if (mcnt == 1)
                  {
                     Check_Aperture(getApertureName(),T_RECTANGLE, unitscale*width, unitscale*height,
                        width/2.,height/2., DegToRad(rotationDegrees), G.cur_app, BL_APERTURE);
                  }
                  else
                  {
                     // complex
                     // 1 = width
                     // 2 = height
                     // 3 = xcenter
                     // 4 = ycenter

                     Graph_PolyStruct(float_layer, 0, negative);

                     if (G.width0index < 0)
                     {
                        int err;
                        G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                     }

                     CPoly* poly = Graph_Poly(NULL, G.width0index, TRUE, FALSE, TRUE);

                     Graph_Vertex(0.   ,0.    ,0.);
                     Graph_Vertex(0.   ,height,0.);
                     Graph_Vertex(width,height,0.);
                     Graph_Vertex(width,0.    ,0.);
                     Graph_Vertex(0.   ,0.    ,0.);

                     CTMatrix matrix;

                     if (s_optionPrimitive22TransformOrderRotateFirst)
                     {
                        matrix.rotateDegrees(rotationDegrees);
                        matrix.translate(xMin,yMin);
                     }
                     else
                     {
                        matrix.translate(xMin,yMin);
                        matrix.rotateDegrees(rotationDegrees);
                     }

                     matrix.scale(unitscale);

                     poly->transform(matrix);
                  }

                  nn +=4;
               }

               break;
            case 4:  // outline
               {
                  int good_rectangle = FALSE;
                  int wcnt = round(nCommand[nn]);
                  int pointCount = wcnt + 1;
                  double x1, y1, x2, y2;
                  int rotationIndex = 2*pointCount + 1;
                  double rot = nCommand[nn + rotationIndex];

                  // here do rotation first
                  if (wcnt == 4)
                  {
                     x1 = nCommand[nn+1];
                     y1 = nCommand[nn+2];
                     x2 = nCommand[nn+5];
                     y2 = nCommand[nn+6];

                     if (fabs(x1 + x2) < ALMOSTZERO && fabs(y1 + y2) < ALMOSTZERO &&
                         (fabs(nCommand[nn+1] + nCommand[nn+3]) <  ALMOSTZERO || fabs(nCommand[nn+2] + nCommand[nn+4]) <  ALMOSTZERO))
                        good_rectangle = TRUE;
                  }
                  // this is always a complex aperature
                  // this needs to be fixed.

                  if (mcnt == 1 && good_rectangle)
                  {
                     // simple
                     Check_Aperture(getApertureName(), T_RECTANGLE, unitscale * fabs(x2-x1), unitscale * fabs(y2-y1), 
                           0.0, 0.0, DegToRad(rot), G.cur_app, BL_APERTURE);
                  }
                  else
                  {
                     // complex
                     if (mcnt == 1)
                        Graph_Block_On(GBO_APPEND, mmacroname,-1, BL_GLOBAL); // generate sub block
                     Graph_PolyStruct(float_layer, 0, negative);
                     if (G.width0index < 0)
                     {
                        int err;
                        G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                     }
                     Graph_Poly(NULL, G.width0index, TRUE, FALSE, TRUE);

                     double firstx, firsty;
                     for (int i=0; i<wcnt+1; i++)
                     {
                        double x, y, x1, y1;
                        x1 = nCommand[nn+1+(i*2)];
                        y1 = nCommand[nn+1+(i*2+1)];
                        Rotate(x1, y1, rot, &x, &y);

                        if (i == 0)
                        {
                           firstx = x;
                           firsty = y;
                        }

                        Graph_Vertex(unitscale*x, unitscale*y, 0.0);
                     }
                     if (wcnt)   
                        Graph_Vertex(unitscale*firstx, unitscale*firsty, 0.0);


                     if (mcnt == 1)
                     {
                        Graph_Block_Off();
                        setUniqueApertureName();

                        //int err = 0;
                        //CString old_appname = getApertureName();
                        //// check if cur_app_name already is assigned to  a mmacroname.
                        //while (Graph_Block_Exists(doc, getApertureName(), -1))
                        //{
                        //   setApertureName(G.cur_app,++err);
                        //}
                        //if (err) 
                        //{
                        //   apertureRenameTable.renameAperture(getApertureName(), old_appname);
                        //   G.cur_app = 0;
                        //}

                        Graph_Complex(getApertureName(), G.cur_app, mmacroname, 0.0, 0.0, 0.0);
                     }
                  }

                  nn += wcnt*2 + 3;
               }

               break;
            case 5:  // polygon
               {
                  int    sideCount        = DcaRound(nCommand[nn]);
                  bool   evenSideCount    = (sideCount % 2 == 0);
                  double xCenter          = unitscale * nCommand[nn + 1];
                  double yCenter          = unitscale * nCommand[nn + 2];
                  double diameter         = unitscale * nCommand[nn + 3];
                  double rotationDegrees  =             nCommand[nn + 4];
                  double rotationRadians  = degreesToRadians(rotationDegrees);
                  double circumCircleRadius,inCircleRadius;
                  double theta = TwoPi / sideCount;
                  double halfTheta = theta/2.;
                  double cosHalfTheta = cos(halfTheta);
                  double offsetAngleRadians = 0.;
                  double xOffset = 0.;

                  if (s_polygonApertureOrientationZeroFlag)
                  {
                     offsetAngleRadians = 0.;
                  }
                  else
                  {
                     offsetAngleRadians = (evenSideCount ? halfTheta : Pi);
                  }

                  switch (s_polygonApertureMacroDiameterType)
                  {
                  case polygonApertureMacroDiameterFromExtentWidth:
                     if (evenSideCount)
                     {
                        if (s_polygonApertureOrientationZeroFlag)
                        {
                           circumCircleRadius = diameter/2.;
                           inCircleRadius = circumCircleRadius*cosHalfTheta;
                        }
                        else
                        {
                           inCircleRadius = diameter/2.;
                           circumCircleRadius = inCircleRadius/cosHalfTheta;
                        }
                     }
                     else
                     {
                        inCircleRadius     = diameter/(1. + 1./cosHalfTheta);
                        circumCircleRadius = inCircleRadius / cosHalfTheta;
                     }

                     break;
                  case polygonApertureMacroODiameterFromInCircle:
                     inCircleRadius = diameter/2.;
                     circumCircleRadius = inCircleRadius/cosHalfTheta;

                     break;
                  case polygonApertureMacroDiameterFromCircumCircle:
                     circumCircleRadius = diameter/2.;
                     inCircleRadius = circumCircleRadius * cosHalfTheta;

                     break;
                  }

                  switch (s_polygonApertureMacroOriginType)
                  {
                  case polygonApertureMacroOriginFromCircumCircle:
                     xOffset = 0.;

                     break;
                  case polygonApertureMacroOriginFromExtent:
                     if (evenSideCount)
                     {
                        xOffset = 0.;
                     }
                     else
                     {
                        xOffset = -(circumCircleRadius - diameter/2.);
                     }

                     break;
                  }

                  double inCircleDiameter = 2. * inCircleRadius;

                  if (sideCount == 4)
                  {
                     // if number of sides = 4 square 
                     offsetAngleRadians = (s_polygonApertureOrientationZeroFlag ? halfTheta : 0.);

                     Check_Aperture(getApertureName(), T_RECTANGLE,inCircleDiameter,inCircleDiameter,xCenter,yCenter,rotationRadians + offsetAngleRadians, G.cur_app, BL_APERTURE);
                  }
                  else if (sideCount == 8)
                  {
                     // if number of sides = 8 octagon
                     offsetAngleRadians = (s_polygonApertureOrientationZeroFlag ? halfTheta : 0.);

                     Check_Aperture(getApertureName(), T_OCTAGON,inCircleDiameter,0.0,xCenter,yCenter,rotationRadians + offsetAngleRadians, G.cur_app, BL_APERTURE);
                  }
                  else
                  {
                     if (mcnt == 1)
                     {
                        Graph_Block_On(GBO_APPEND, mmacroname, -1, BL_GLOBAL);   // generate sub block
                     }

                     Graph_PolyStruct(float_layer, 0, negative);

                     if (G.width0index < 0)
                     {
                        int err;
                        G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
                     }

                     CPoly* poly = Graph_Poly(NULL, G.width0index, TRUE, FALSE, TRUE);

                     for (int sideIndex = 0;sideIndex <= sideCount;sideIndex++)
                     {
                        double angleRadians = (sideIndex * theta);
                        double x = xCenter + circumCircleRadius * cos(angleRadians);
                        double y = yCenter + circumCircleRadius * sin(angleRadians);

                        Graph_Vertex(x,y,0.0);
                     }

                     CTMatrix matrix;
                     matrix.translate(xOffset,0.);
                     matrix.rotateRadians(rotationRadians + offsetAngleRadians);

                     poly->transform(matrix);

                     if (mcnt == 1)
                     {
                        Graph_Block_Off();
                        setUniqueApertureName();

                        //int err = 0;
                        //CString  old_appname = getApertureName();

                        //// check if cur_app_name already is assigned to  a mmacroname.
                        //while (Graph_Block_Exists(doc, getApertureName(), -1))
                        //{
                        //   setApertureName(G.cur_app,++err);
                        //}

                        //if (err) 
                        //{
                        //   apertureRenameTable.renameAperture(getApertureName(), old_appname);
                        //   G.cur_app = 0;
                        //}

                        Graph_Complex(getApertureName(), G.cur_app, mmacroname, 0.0, 0.0, 0.0);
                     }
                  }

                  nn += 4;   // ?? i dont know
               }

               break;
            case 6:  // moire
               if (!macro_thermals)
               {
                  double w1 = nCommand[nn+1];
                  Check_Aperture(getApertureName(), T_TARGET, unitscale*w1, 0, 0, 0, 0, G.cur_app, BL_APERTURE);
               }
               else
               {
                  CString pname;
                  pname.Format("M_%s", getApertureName());

                  if (mcnt == 1)
                     Graph_Block_On(GBO_APPEND, pname, -1, BL_GLOBAL);  // generate sub block

                  double x = nCommand[nn-1], y = nCommand[nn];
                  double rad = nCommand[nn+1]/2;
                  double w = nCommand[nn+2];  // line thickness
                  double gap = nCommand[nn+3];
                  double w1 = nCommand[nn+5]; // crosshair thickness
                  double len = nCommand[nn+6];
                  double rot = nCommand[nn+7];
                  int cnt = (int)nCommand[nn+4];
                  int err;

                  for (int i=0; i<cnt; i++)
                  {
                     Graph_Circle(float_layer, unitscale*x, unitscale*y, unitscale*rad, 0L, 0,
                           Graph_Aperture("", T_ROUND, unitscale * w, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err),
                           0);
                     rad -= (gap+w);
                  }

                  // also 2 lines 90 degree offset
                  double x1, y1, x2, y2, xx, yy;
                  x1 = x;
                  x2 = x;
                  y1 = y - len/2;
                  y2 = y + len/2;
                  Rotate(x1, y1 ,rot, &xx, &yy);
                  x1=xx;
                  y1=yy;
                  Rotate(x2, y2, rot, &xx, &yy);
                  x2=xx;
                  y2=yy;
                  Graph_Line(float_layer, unitscale*x1, unitscale*y1, unitscale*x2, unitscale*y2, 0,
                        Graph_Aperture("", T_RECTANGLE, unitscale * w1, unitscale * w1, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err),
                        0);  

                  // 90 degree offset to first line
                  x1 = x-len/2;
                  x2 = x+len/2;
                  y1 = y;
                  y2 = y;
                  Rotate(x1, y1 ,rot, &xx, &yy);
                  x1=xx;
                  y1=yy;
                  Rotate(x2, y2, rot, &xx, &yy);
                  x2=xx;
                  y2=yy;
                  Graph_Line(float_layer, unitscale*x1, unitscale*y1, unitscale*x2, unitscale*y2, 0,
                        Graph_Aperture("", T_RECTANGLE, unitscale * w1, unitscale * w1, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err), 
                        0);

                  if (mcnt == 1)
                  {
                     Graph_Block_Off();

                     setUniqueApertureName();
                     //int err = 0;
                     //CString old_appname = getApertureName();

                     //// check if cur_app_name already is assigned to  a mmacroname.
                     //while (Graph_Block_Exists(doc, getApertureName(), -1))
                     //{
                     //   setApertureName(G.cur_app,++err);
                     //}
                     //if (err) 
                     //{
                     //   apertureRenameTable.renameAperture(getApertureName(), old_appname);
                     //   G.cur_app = 0;
                     //}

                     Graph_Complex(getApertureName(), G.cur_app, pname, 0.0, 0.0, 0.0);
                  }
               }

               nn += 8;   // ?? i dont know

               break;
            case 7:
               {
                  double x               = nCommand[nn-1] * unitscale;  // there is no onoff
                  double y               = nCommand[nn  ] * unitscale;
                  double outerDiameter   = nCommand[nn+1] * unitscale;
                  double innerDiameter   = nCommand[nn+2] * unitscale;
                  double spokeWidth      = nCommand[nn+3] * unitscale;
                  double rotationDegrees = nCommand[nn+4];
                  double rotationRadians = degreesToRadians(rotationDegrees);
                  double spokeCount      = 4.;

                  if (s_optionGenerateComplexThermalApertures)
                  {
                     CStandardAperture thermalAperture(standardApertureRoundThermalSquare,doc->getPageUnits());
                     thermalAperture.setDimensions(outerDiameter,innerDiameter,rotationDegrees,spokeCount,spokeWidth);

                     CString complexApertureSubBlockName = thermalAperture.getDescriptor() + "-cmplx";
                     BlockStruct* apertureSubBlock = Graph_Block_On(GBO_APPEND,complexApertureSubBlockName,-1,BL_GLOBAL);

                     DataStruct* polyStruct = Graph_PolyStruct(float_layer, 0, 0);

                     thermalAperture.getAperturePoly(doc->getCamCadData(),apertureSubBlock->getDataList(),polyStruct);

                     Graph_Block_Off();
                     setUniqueApertureName();
                     Graph_Complex(getApertureName(), G.cur_app, complexApertureSubBlockName, 0.0, 0.0, 0.0);

                     //BlockStruct* apertureBlock = thermalAperture.getDefinedAperture(doc->getCamCadData());

                     //setApertureName(apertureBlock->getName());
                  }
                  else
                  {
                     Check_Aperture(getApertureName(), T_THERMAL, outerDiameter, innerDiameter, y,y, rotationRadians, G.cur_app, BL_APERTURE);
                  }

                  nn = ncnt;
               }

               break;
            default:
                  // unknown
               break;
            }  // end switch
         }

         if (mcnt > 1)
         {
            // now attach it to an aperture.
            Graph_Block_Off();
            setUniqueApertureName();

            //int err = 0;
            //CString  old_appname;
            //old_appname = getApertureName();

            //// check if cur_app_name already is assigned to  a mmacroname.
            //while (Graph_Block_Exists(doc, getApertureName(), -1))
            //   setApertureName(G.cur_app,++err);

            //if (err) 
            //{
            //   apertureRenameTable.renameAperture(getApertureName(), old_appname);
            //   G.cur_app = 0;
            //}

            Graph_Complex(getApertureName(), G.cur_app, mmacroname, 0.0, 0.0, 0.0);
         }
      }  // if ncnt > 1
   }
   else
   {
      if ((nCommand = (double *) calloc(MAX_MACROCOMMAND, sizeof(double))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

      // here is a primitive
      // Rectangle is R,0.07X0.07*
      while ((lp = strtok(NULL, "X,")) != NULL && ncnt < 50)
      {
         // max 50 entries allowed
         if (ncnt < 50)
         {
            nCommand[ncnt] = atof(lp);
            ncnt++;
         }
         else
         {
            ErrorMessage("Too many macros" , "Error in Gerber Read", MB_ICONHAND | MB_OK);
            return 0;
         }
      }

      if (ncnt > 0)
      {
         // write aperature block to database
         if (!strcmp(macroname, "C"))
         {  
            if (ncnt == 1) // circle
               Check_Aperture(getApertureName(), T_ROUND, unitscale*nCommand[0], 0.0, 0.0, 0.0, 0.0, G.cur_app, BL_APERTURE);
            else  // donut
               Check_Aperture(getApertureName(), T_DONUT, unitscale*nCommand[0], unitscale*nCommand[1], 0.0, 0.0, 0.0, G.cur_app, BL_APERTURE);
         }
         else if (!strcmp(macroname, "P"))
         {
            int numOfSide = (int)nCommand[1];

            if (numOfSide < 3)
            {
               fprintf(flog, "Invalid polygon! [%s] has less than 3 sides; therefore it is kipped\nCommand", getApertureName());
               display_error++;
               display_log = TRUE;

               if (nCommand != NULL)
                  free(nCommand);
               return 1;
            }

            double rot = nCommand[2];           // start rotation
            double hypotenuse = nCommand[0]/2;  // here is radius
            double angle = DegToRad(360/numOfSide);
            if (G.width0index < 0)
               G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, NULL);

            CString pname = "P_" + getApertureName();
            Graph_Block_On(GBO_APPEND, pname, -1, BL_GLOBAL);  // generate sub block
            Graph_PolyStruct(float_layer, 0, 0);
            Graph_Poly(NULL, G.width0index, TRUE, FALSE, TRUE);            

            // The first point always start at the x-axis, so x is hypotenuse
            double firstX = 0.0;
            double firstY = 0.0;
            Rotate(hypotenuse, 0.0, rot, &firstX, &firstY);
            Graph_Vertex(unitscale * firstX, unitscale * firstY, 0.0);

            for (int i=1; i<numOfSide; i++)
            {
               double x = hypotenuse * cos(angle * i);
               double y = hypotenuse * sin(angle * i);
               double xrot = 0.0;
               double yrot = 0.0;

               Rotate(x, y, rot, &xrot, &yrot);
               Graph_Vertex(unitscale * xrot, unitscale * yrot, 0.0);
            }

            //Close the poly by adding the first point into the poly
            Graph_Vertex(unitscale * firstX, unitscale * firstY, 0.0);

            Graph_Block_Off();
            setUniqueApertureName();
            Graph_Complex(getApertureName(), G.cur_app, pname, 0.0, 0.0, 0.0);
         }
         else if (!strcmp(macroname,"R"))
         {  // Rectangle if ncnt > 2 than the rectangle has a hole - not yet done, because
            // we need to implement negative aperture flash
            if (ncnt > 2)
            {
                // Graph_Block(macroname, BL_GLOBAL); // generate sub block
            }

            Check_Aperture(getApertureName(), T_RECTANGLE, unitscale*nCommand[0], unitscale*nCommand[1], 0.0, 0.0, 0.0, G.cur_app, BL_APERTURE);

            if (ncnt > 2)
            {
                //Graph_Block_Off();
                // now attach it to an aperture.
                //Graph_Complex(cur_app_name, G.cur_app, macroname);
            }

         }
         else if (!strcmp(macroname,"O"))
         {  // Rectangle
            Check_Aperture(getApertureName(), T_OBLONG, unitscale*nCommand[0], unitscale*nCommand[1], 0.0, 0.0, 0.0, G.cur_app, BL_APERTURE);
         }
      }
   }

   if (nCommand != NULL)
      free(nCommand);

   return 1;
}

/****************************************************************************
/*
*/
static const char *get_param(const char *p1, char **plist, int pcnt, int *exchanged)
{
   static char w[MAX_LINE];
   char  *lp;
   
   for (int i=0;i<pcnt;i++)
   {
      strcpy(w,plist[i]);

      // no strtok allowed - used in parent function
      //lp = strtok(w,"=");
      if (lp = strchr(w,'='))
      {
         *lp = '\0';

         if (!STRCMPI(p1,w))
         {
            *lp++;
            //lp = strtok(NULL,"\n");
            // here any of this macros can be another macro
            *exchanged |= TRUE;
            CString  w1;
            w1.Format("(%s)",lp);
            strcpy(w, w1);
            return w;
         }
      }
   }

   return p1;
}

/******************************************************************************
* resolve_complex_macro
*/
static const char *resolve_complex_macro(const char *p1, char **plist, int pcnt, int *exchanged)
{
   static   CString  res;
   CString  par;
   int      parammode = FALSE;
   res = "";
   par = "";

   for (unsigned int w=0; w<strlen(p1); w++)
   {
      if (p1[w] == '$')
      {
         parammode = TRUE;
         par = "";
      }
      else if (strchr("-X/+(),=",p1[w]))
      {
         // here now check if param is ok or needs to be resolved
         if (strlen(par))
         {
            res += get_param(par, plist, pcnt, exchanged);
         }
         par = "";
         parammode = FALSE;
      }

      if (!parammode)
      {
         res += p1[w];
      }
      else
      {
         par += p1[w];
      }
   }
            
   if (strlen(par))
   {
      // also here resolve
      res += get_param(par, plist, pcnt, exchanged);
   }

   return res.GetBuffer(0);
}

/****************************************************************************
*
   FS       = Format statement
     L      = Leading zeros ommitted
     T      = Trailing zeros ommited
     D      = explicit decimal point
      A     = Absolute
      I     = Incremental
       Nn   = sequence number
       Gn   = ???
       Xa   = x format 3.4
       Yb   = y format 2.4 upto 5.5
       Zc
       Dn
       Mn
   * delimeter
   ans than fs goes to Mass paramrters
*/
static int do_fs(char *f)
{
   char  dd[20];
   int   done = FALSE;

	unsigned int i=0;
   for (i=0;i<strlen(f) && !done;i++)
   {
      switch (f[i])
      {
      case '*':
         done = TRUE;
         break;
      case 'L':
         G.zeroleading = 0;   // leading ommitted - must be trailing. This is normal case
         break;
      case 'T':
         G.zeroleading = 1;   // trailing ommitted - must be leading
         break;
      case 'D':
         // this D means 2 different things !
         break;
      case 'A':
         G.absolute = TRUE;
         break;
      case 'I':
         G.absolute = FALSE;
         break;
      case 'N':
         break;
      case 'G':
         break;
      case 'X':
      case 'Y':
      case 'Z':
         {
            // here format
            i++;
            sprintf(dd,"%c",f[i]);
            G.vor_komma  = atoi(dd);
            i++;
            sprintf(dd,"%c",f[i]);
            G.nach_komma = atoi(dd);

            double b = GBscale;  // just debug
            GBscale = OriginalGBscale;
            for (int ii=0;ii<G.nach_komma;ii++)
               GBscale /=10;
            b = GBscale;         // just debug
         }
         break;
      case 'M':
         break;
      case '%':
         break;
      }
   }

   if (i < strlen(f))
   {
      // here are now mass parameters, which are DbUnit char codes.
   }

   return 1;
}

/****************************************************************************
* do_koo_adjust
            |
   Quadrant1|Quadrant0
            |           
   -------------------
            |
   Quadrant2|Quadrant3
            |


   the slope is the arc (old to cur) line slope.


*/
static void do_koo_adjust()
{
   if (G.done)                return;
   if (G.drawmode != PENDOWN) return;

   if (G.arc_interpolation)
   {
      if (G.G75code) // Multi-Quadrant Interpolation
      {
         //multi quadrant
         G.cur_i += G.old_x;
         G.cur_j += G.old_y;
      }
      else // Single Quadrant Interpolation
      {
         // this I and J always do towards the x/y axis
         if (G.counterclock)
         {
            if (G.old_x > G.cur_x && G.old_y < G.cur_y)
            {
               // quad 0 
               G.cur_i = G.old_x - fabs(G.cur_i);
               G.cur_j = G.old_y - fabs(G.cur_j);
            }
            else if (G.old_x > G.cur_x && G.old_y > G.cur_y)
            {
               // quad 1
               G.cur_i = G.old_x + fabs(G.cur_i);
               G.cur_j = G.old_y - fabs(G.cur_j);
            }
            else if (G.old_x < G.cur_x && G.old_y > G.cur_y)
            {
               // quad 2
               G.cur_i = G.old_x + fabs(G.cur_i);
               G.cur_j = G.old_y + fabs(G.cur_j);
            }
            else if (G.old_x < G.cur_x && G.old_y < G.cur_y)
            {
               // quad 3
               G.cur_i = G.old_x - fabs(G.cur_i);
               G.cur_j = G.old_y + fabs(G.cur_j);
            }
            else
            {
               // illegal arc
               G.tmp_arc_interpolation = FALSE;
            }
         }
         else
         {
            // clock
            if (G.old_x < G.cur_x && G.old_y > G.cur_y)
            {
               // quad 0 
               G.cur_i = G.old_x - fabs(G.cur_i);
               G.cur_j = G.old_y - fabs(G.cur_j);
            }
            else if (G.old_x < G.cur_x && G.old_y < G.cur_y)
            {
               // quad 1 
               G.cur_i = G.old_x + fabs(G.cur_i);
               G.cur_j = G.old_y - fabs(G.cur_j);
            }
            else if (G.old_x > G.cur_x && G.old_y < G.cur_y)
            {
               // quad 2 
               G.cur_i = G.old_x + fabs(G.cur_i);
               G.cur_j = G.old_y + fabs(G.cur_j);
            }
            else if (G.old_x > G.cur_x && G.old_y > G.cur_y)
            {
               // quad 3 like counterclock 1
               G.cur_i = G.old_x - fabs(G.cur_i);
               G.cur_j = G.old_y + fabs(G.cur_j);
            }
            else
            {
               // illegal arc
               G.tmp_arc_interpolation = FALSE;
            }
         }
      } // else not G75
   } // if arc
}


/******************************************************************************
* cnv_tok
*/
double cnv_tok(char *t)
{
   if (G.zeroleading == 1)    // 
   {
      char  t1[20];
      int   tcnt = 0;

      if (t[0] == '-' || t[0] == '+')
      {
         t1[tcnt] = t[0];
         tcnt++;
         t++;
      }

      if ((int)strlen(t) > (G.vor_komma + G.nach_komma))
      {
         G.vor_komma = strlen(t) - G.nach_komma;
      }

      // a string can be shorter than leading.
      while ((int)strlen(t) < G.vor_komma)
      { 
         int   tlen = strlen(t);
         t[tlen] = '0';
         t[tlen+1] = '\0';
      }

		int i=0;
      for (i=0; i<G.vor_komma; i++)
         t1[tcnt++]=t[i];

      t1[tcnt++] = '.';

      for (i=G.vor_komma; i<(int)strlen(t); i++)
         t1[tcnt++] = t[i];

      t1[tcnt] = '\0';
      double x = atof(t1);    
      return x * OriginalGBscale * unitscale;
   }
   
   // trailing zeros. This is 6250 = 6.250 
   double x = atof(t);
   return x * GBscale * unitscale;
}

/******************************************************************************
* do_arc_poly
*/
static int do_arc_poly()
{
   double rad, sa, da;
   ArcCenter2(G.old_x, G.old_y, G.cur_x, G.cur_y, G.cur_i, G.cur_j, &rad, &sa, &da, !G.counterclock);

   if (gPointArrayCount < 1)
   {
      // error !!!!!
      fprintf(flog, "Arc error !!!\n");
      display_error++;
   }
   else
   {
      if (rad > SMALLNUMBER)
      {

//G01X93D01*
//G03X274Y496I0J-21D01*    <-- same koo
//G01X274Y496D01*          <-- 
//G03X274Y496I19J-10D01*   <--
//G01X274Y495D01*          <--
//G02X272Y494I-2J1D01*     <--
//G01X272D01*

         if (fabs(da) > PI2 - SMALLANGLE) // circle
         {
            GPoint *startPoint = gPointArray[gPointArrayCount-1];
            startPoint->bulge = 1;

            double distx = G.old_x - G.cur_i;
            double disty = G.old_y - G.cur_j;

            GPoint *gPoint = new GPoint;
            gPoint->x = G.cur_x - 2*distx;
            gPoint->y = G.cur_y - 2*disty;
            gPoint->bulge = 1;
            gPointArray.SetAtGrow(gPointArrayCount++, gPoint);

            // close circle
            gPoint = new GPoint;
            gPoint->x = startPoint->x;
            gPoint->y = startPoint->y;
            gPoint->bulge = 0;
            gPointArray.SetAtGrow(gPointArrayCount++, gPoint);
         }
         else
         {
            GPoint *gPoint = gPointArray[gPointArrayCount-1];
            gPoint->bulge = tan(da / 4);
         }
      } // bad arc = rad set to 0
   }

   // can not reset the arc interpolation. G01 will do it.
   //G.arc_interpolation = FALSE;

   return 1;
}  

/******************************************************************************
* write_db
*/
void write_db(CString layername, int forceout)
{
   bool forceOutPenDown      = (forceout && G.drawmode == PENDOWN);
   bool pointsWithPenNotDown = (G.drawmode != PENDOWN && gPointArrayCount > 1);

   // also write polylines if there is an arc
   if (forceOutPenDown || pointsWithPenNotDown) 
   {      
      int widthIndex = Graph_Aperture(last_draw_ap_name, 0, 0.0, 0.0,0.0, 0.0, 0.0, G.cur_app, BL_APERTURE, FALSE, NULL);

      // if current mode is pen down and a force, the gPointArray needs to be updated
      // This is incorrect, because if it comes from a flash force, cur_xy is the flash position.
      if (forceout == 1 && G.drawmode == PENDOWN)
      {
         if (!G.done && G.tmp_arc_interpolation) // if done, this means the do_arc was already done before and no new records added.
            do_arc_poly();
         
         if (gPointArrayCount > 0) // if points exist
         {
            GPoint *gPoint = gPointArray[gPointArrayCount-1]; // always greater 1

            // do not repeat if coordinate has not changed
            if (gPoint->x != G.cur_x || gPoint->y != G.cur_y)
            {
               gPoint = new GPoint;
               gPoint->x = G.cur_x;
               gPoint->y = G.cur_y;
               gPoint->bulge = 0;
               gPointArray.SetAtGrow(gPointArrayCount++, gPoint);
            }
         }
      }

      // according to CAM350, the fill uses a zero-width line
      if (G.G36fill) 
      {
         if (G.width0index < 0)
            G.width0index = Graph_Aperture("", T_ROUND, 0, 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, NULL);

         widthIndex = G.width0index; // if a G36 is detected, the polyline gets a 0 width boundary.
      }

      if (gPointArrayCount && G.layernum < 0)
      {
         G.layernum = Graph_Level(layername, "", 0); // makes every gerber file a different color

         LayerStruct *layer = doc->FindLayer(G.layernum);
         if (layer)
         {
            CString dataSource = QDataSourceGerber;
            int dataSourceKw = doc->getStandardAttributeKeywordIndex(standardAttributeDataSource);
            layer->setAttrib(doc->getCamCadData(), dataSourceKw, valueTypeString, dataSource.GetBuffer(0), attributeUpdateOverwrite, NULL);  
         }
      }
      
      if(gPointArrayCount > 0)
      {
         GPoint *startPoint = gPointArray[0];
         GPoint *endPoint = gPointArray[gPointArrayCount-1];

         BOOL Closed = startPoint->x == endPoint->x && startPoint->y == endPoint->y;
      

         if (gPointArrayCount == 2 && Closed && G.drawmode != PENDOWN)
         {
            // shutter on/off on same coordinate is a flash
            Graph_Aperture(last_draw_ap_name, 0, 0, 0, 0, 0, 0, G.cur_app, BL_APERTURE, FALSE, NULL);
            DataStruct *data = Graph_Block_Reference(last_draw_ap_name, NULL, 0, startPoint->x, startPoint->y, 0, 0, 1, G.layernum, TRUE);

            if (ATTACH_G4COMMENT && strlen(G4comment))
            {
               doc->SetUnknownAttrib(&data->getAttributesRef(), G4key, G4comment, SA_OVERWRITE, NULL);
               G4comment = "";
            }
         }
         else
         {
            DataStruct *data = Graph_PolyStruct(G.layernum, 0, cur_negative); 
            Graph_Poly(NULL, widthIndex, G.G36fill, 0, Closed);

            CPnt *prevAddedPnt = NULL;
            GPoint *prevGPoint = NULL;

            double firstx, firsty, firstbulge;

            for (int i=0; i<gPointArrayCount; i++)
            {
               GPoint *gPoint = gPointArray[i];

               if (i == 0)
               {
                  firstx = gPoint->x;
                  firsty = gPoint->y;
                  firstbulge = gPoint->bulge;
               }

               if (i && gPoint->x == prevGPoint->x && gPoint->y == prevGPoint->y)
               {
                  if (gPoint->bulge)
                     prevAddedPnt->bulge = (DbUnit)gPoint->bulge;
                  continue;
               }

               prevAddedPnt = Graph_Vertex(gPoint->x, gPoint->y, gPoint->bulge);
               prevGPoint = gPoint;
            }

            if (gPointArrayCount > 2 && (Closed || G.G36fill))
            {
               GPoint *lastPoint = gPointArray[gPointArrayCount -1];

               if (firstx != lastPoint->x || firsty != lastPoint->y)
                  Graph_Vertex(firstx, firsty, 0);
            }

         }
      }
      
      // remember the last koo i.e
      // x1y1PENDOWN
      // PENUP
      // x2y2PENDOWN

      FreeGPointArray();

      GPoint *newGPoint = new GPoint;
      newGPoint->x = G.cur_x;
      newGPoint->y = G.cur_y;
      newGPoint->bulge = 0;
      gPointArray.SetAtGrow(gPointArrayCount++, newGPoint);
      G.done = TRUE;

      return;
   }

   if (G.done) // already written
      return;

   if (G.layernum < 0)
   {
      G.layernum = Graph_Level(layername, "", 0); // makes every gerber file a different color

      LayerStruct *layer = doc->FindLayer(G.layernum);
      if (layer)
      {
         CString dataSource = QDataSourceGerber;
         int dataSourceKw = doc->getStandardAttributeKeywordIndex(standardAttributeDataSource);
         layer->setAttrib(doc->getCamCadData(), dataSourceKw, valueTypeString, dataSource.GetBuffer(0), attributeUpdateOverwrite, NULL);  
      }
   }

   switch (G.drawmode)
   {
   case PENDOWN:
      {
         if (G.tmp_arc_interpolation)
            do_arc_poly();

         // here update gPointArrayCount with next entry, because a polyline can follow an arc
         GPoint *gPoint = new GPoint;
         gPoint->x = G.cur_x;
         gPoint->y = G.cur_y;
         gPoint->bulge = 0;
         gPointArray.SetAtGrow(gPointArrayCount++, gPoint);
         last_draw_ap_name = getApertureName(); // remember this aperture name when on next aperture and drawing this one
         G.done = TRUE;
      }
      break;

   case PENFLASH:
      {
         Graph_Aperture(getApertureName(), 0, 0.0, 0, 0, 0, 0, G.cur_app, BL_APERTURE, FALSE, NULL);

         //CCoordinateBreakPoint::breakOnCoordinate(G.cur_x,G.cur_y);

         // must be filenum 0, because apertures are global.
         CString refName;
         refName.Format("Line%d",CTextScanner::getLineCount());
         DataStruct *data = Graph_Block_Reference(getApertureName(),refName, 0, G.cur_x, G.cur_y, 0, 0, 1.0, G.layernum, TRUE);

         if (cur_negative) 
            data->setNegative(true);

         if (ATTACH_G4COMMENT)
         {
            if (strlen(G4comment))
            {
               doc->SetUnknownAttrib(&data->getAttributesRef(), G4key, G4comment, SA_OVERWRITE, NULL);
               G4comment = "";
            }
         }

         FreeGPointArray();

         // always store last position because after a flash a pendown can happen.
         GPoint *gPoint = new GPoint;
         gPoint->x = G.cur_x;
         gPoint->y = G.cur_y;
         gPoint->bulge = 0;
         gPointArray.SetAtGrow(gPointArrayCount++, gPoint);

         G.done = TRUE;
         G.drawmode = PENUNDEFINED;
      }
      break;

   case PENUP:
      {
         // this is penup
         cont_flag = FALSE;

         FreeGPointArray();

         GPoint *gPoint = new GPoint;
         gPoint->x = G.cur_x;
         gPoint->y = G.cur_y;
         gPoint->bulge = 0;
         gPointArray.SetAtGrow(gPointArrayCount++, gPoint);
         last_draw_ap_name = getApertureName(); // remember this aperture name when on next aperture and drawing this one
         G.drawmode = PENUP;
      }

      break;
   }
}

/******************************************************************************
* Gerber_Read_Token
*/
static int Gerber_Read_Token(FILE *text_file, char *token, int max_length)
{
   int   res, cnt = 0, ncnt = 0;
   char  c;
   token[0] = '\0';

   lastcharstring = "";

   while ((res = Read_Char(text_file,&c)) == NORMAL_TEXT)
   {
      lastcharstring += Get_ReadCharString();

      if (cnt < max_length-1) // still get the last char,
      {
         token[cnt] = c;
         cnt++;
      }
      else
      {
         //ErrorMessage("Token length Error","Error in Gerber Read");
         token[cnt] = c;
         cnt++;
         res = -99;
         break;
      }
   }

   lastcharstring += Get_ReadCharString();

   token[cnt] = '\0';

   if (res != STOP_TEXT) // not is %AM ZXNxxx % 
   {
      // here take out the Nxxx number
      char  *lp;

      if (lp = strrchr(token,'N'))
      {
         if (*(lp + 1))
         {
            int num = atoi(lp+1);
         }

         // Nx number is sequence count - kill it.
         //lp++;
         if (*(lp+1) && isdigit(*(lp+1)))
         {
            *lp = '\0';
            //token = lp;
         }
      }
   }

   return res;
}

/******************************************************************************
* get_ap_rename_name
*/
//static void get_ap_rename_name(char *orig,int *app)
//{
//   for (int i=0; i<ap_renamecnt; i++)
//   {
//      if (!strcmp(ap_rename[i].orig,orig))
//      {
//         strcpy(orig,ap_rename[i].name);
//         *app = 0;
//
//         return;
//      }
//   }
//}

/******************************************************************************
* is_block_end 
*/
static char is_block_end(char l)
{
   //if (strchr(blockendchar, l))
   if (blockendchar.Find(l,0) >= 0)
      return BLOCKENDCHAR;

   return l;
}

/******************************************************************************
* FreeGPointArray
*/
static void FreeGPointArray()
{
   for (int i=0; i<gPointArrayCount; i++)
      delete gPointArray[i];

   gPointArrayCount = 0;
}


//_____________________________________________________________________________
CExtent CCoordinateBreakPoint::m_extent;
double CCoordinateBreakPoint::m_searchTolerance=0.;

void CCoordinateBreakPoint::setExtent(double x0,double y0,double x1,double y1)
{
   m_extent.reset();
   m_extent.update(x0,y0);
   m_extent.update(x1,y1);
}

void CCoordinateBreakPoint::setSearchTolerance(double searchTolerance)
{
   m_searchTolerance = searchTolerance;
}

void CCoordinateBreakPoint::setSearchPoint(double x,double y)
{
   setSearchPoint(x,y,m_searchTolerance);
}

void CCoordinateBreakPoint::setSearchPoint(double x,double y,double searchTolerance)
{
   m_extent.reset();
   m_extent.update(x - searchTolerance,y - searchTolerance);
   m_extent.update(x + searchTolerance,y + searchTolerance);
}

bool CCoordinateBreakPoint::breakOnCoordinate(double x,double y)
{
   bool retval = false;

   if (m_extent.isValid() && m_extent.isOnOrInside(CPoint2d(x,y)))
   {
      retval = true;
   }

   return retval;
}

//_____________________________________________________________________________
CGerberMacroCommand::CGerberMacroCommand()
{
   m_params.SetSize(0,10);
}

void CGerberMacroCommand::setVariableParams(CDoubleArray& variableParams)
{
   for (int paramIndex = 0;paramIndex < m_params.GetSize();paramIndex++)
   {
      double value = calc_param(m_params.GetAt(paramIndex),variableParams);
      m_paramValues.SetAtGrow(paramIndex,value);
   }
}

CGerberMacroCommand::CGerberMacroCommand(const CString& command) :
   m_command(command)
{
   CSupString macroCommand(command);
   CStringArray params;

   macroCommand.Parse(m_params,",");
}

int CGerberMacroCommand::getCount() const 
{ 
   return m_paramValues.GetCount(); 
}

double CGerberMacroCommand::getAt(int index) 
{ 
   double value = ((index < m_paramValues.GetSize()) ? m_paramValues.GetAt(index) : 0.); 

   return value;
}

void CGerberMacroCommand::setAt(int index,double value)
{
   m_paramValues.SetAtGrow(index,value);
}

int CGerberMacroCommand::getStandardParamCount()
{
   int standardParamCount = 0;

   switch (getPrimitive())
   {
   case 1:   standardParamCount =  5;  break;
   case 2:   standardParamCount =  8;  break;
   case 20:  standardParamCount =  8;  break;
   case 21:  standardParamCount =  7;  break;
   case 22:  standardParamCount =  7;  break;
   case 3:   standardParamCount =  1;  break;
   case 5:   standardParamCount =  7;  break;
   case 6:   standardParamCount = 10;  break;
   case 7:   standardParamCount =  7;  break;
   case 4:  
   default:
      standardParamCount = getCount();
      break;
   }

   return standardParamCount;
}

//_____________________________________________________________________________
CGerberMacroCommands::CGerberMacroCommands(const CString& commands)
{
   CSupString macroCommands(commands);
   CStringArray commandStrings;

   macroCommands.Replace("\n","");
   macroCommands.Parse(commandStrings,"*");
   m_commands.SetSize(0,commandStrings.GetSize());

   for (int commandIndex = 0;commandIndex < commandStrings.GetSize();commandIndex++)
   {
      CSupString commandString = commandStrings.GetAt(commandIndex);

      if (! commandString.IsEmpty())
      {
         CGerberMacroCommand* macroCommand = new CGerberMacroCommand(commandString);
         m_commands.Add(macroCommand);
      }
   }
}

CGerberMacroCommand* CGerberMacroCommands::getAt(int index) 
{ 
   CGerberMacroCommand* command = NULL;

   if (index < m_commands.GetSize())
   {
      command = m_commands.GetAt(index);
   }
   
   if (command == NULL)
   {
      command = new CGerberMacroCommand();
      m_commands.SetAtGrow(index,command);
   }

   return command; 
}

CDoubleArray& CGerberMacroCommands::getCommandParameters()
{
   int count = 0;

   for (int commandIndex = 0;commandIndex < m_commands.GetSize();commandIndex++)
   {
      CGerberMacroCommand* command = m_commands.GetAt(commandIndex);
      count += command->getStandardParamCount();
   }

   m_commandArray.SetSize(0,count);

   for (int commandIndex = 0;commandIndex < m_commands.GetSize();commandIndex++)
   {
      CGerberMacroCommand* command = m_commands.GetAt(commandIndex);

      for (int paramIndex = 0;paramIndex < command->getStandardParamCount();paramIndex++)
      {
         m_commandArray.Add(command->getAt(paramIndex));
      }
   }

   return m_commandArray;
}

//_____________________________________________________________________________
CApertureRenameEntry::CApertureRenameEntry(const CString& name,const CString& originalName)
: m_name(name)
, m_originalName(originalName)
{
}

//_____________________________________________________________________________
bool CApertureRenameTable::renameAperture(const CString& newApertureName,const CString& originalApertureName)
{
   bool retval = false;

   for (POSITION pos = m_entries.GetHeadPosition();pos != NULL;)
   {
      CApertureRenameEntry* entry = m_entries.GetNext(pos);

      if (entry->getOriginalName().CompareNoCase(originalApertureName) == 0)
      {
         entry->setName(newApertureName);
         retval = true;
         break;
      }
   }

   if (!retval)
   {
      CApertureRenameEntry* entry = new CApertureRenameEntry(newApertureName,originalApertureName);

      m_entries.AddTail(entry);
   }

   return retval;
}

CString CApertureRenameTable::getRenamedApertureName(const CString& apertureName)
{
   CString retval(apertureName);

   for (POSITION pos = m_entries.GetHeadPosition();pos != NULL;)
   {
      CApertureRenameEntry* entry = m_entries.GetNext(pos);

      if (entry->getOriginalName().CompareNoCase(apertureName) == 0)
      {
         retval = entry->getName();
         break;
      }
   }

   return retval;
}

#ifdef newMacroProcessing

//_____________________________________________________________________________
CGerberMacroSymbolTable::CGerberMacroSymbolTable(const CStringArray& symbols)
{
   CString name,value;

   for (int index = 0;index < symbols.GetSize();index++)
   {
      value = symbols.GetAt(index);

      name.Format("$%d",index + 1);

      m_symbolTable.SetAt(name,value);
   }
}

void CGerberMacroSymbolTable::expand(CString& expressionValue)
{
   CString symbolName,symbolValue;

   for (POSITION pos = m_symbolTable.GetStartPosition();pos != NULL;)
   {
      m_symbolTable.GetNextAssoc(pos,symbolName,symbolValue);

      expressionValue.Replace(symbolName,symbolValue);
   }

   expressionValue.Replace("X","*");
}

int CGerberMacroSymbolTable::getIntResult(const CString& expression)
{
   return round(getDoubleResult(expression));
}

double CGerberMacroSymbolTable::getDoubleResult(const CString& expression)
{
   CString expressionValue(expression);

   expand(expressionValue);

   double value = 0.;
   BOOL topLevelAssignmentFlag;

   bool validFlag = (EE_Evaluate(expressionValue.GetBuffer(),&value,&topLevelAssignmentFlag) == E_OK);

   if (!validFlag)
   {
      logError("Invalid expression '%s' in '%s'\n",expression,__FUNCTION__);

      value = 0.;
   }

   return value;
}

CString CGerberMacroSymbolTable::getIntResultString(const CString& expression)
{
   CString result;

   result.Format("%d",getIntResult(expression));

   return result;
}

CString CGerberMacroSymbolTable::getDoubleResultString(const CString& expression)
{
   CString result;

   result = fpfmt(getDoubleResult(expression));

   return result;
}

bool CGerberMacroSymbolTable::assign(const CString& name,const CString& expression)
{
   double value = getDoubleResult(expression);

   setAt(name,value);

   return true;
}

void CGerberMacroSymbolTable::setAt(const CString& name,double value)
{
   CString stringValue;
   stringValue.Format("%f",value);

   m_symbolTable.SetAt(name,stringValue);
}

//_____________________________________________________________________________
CGerberMacroStatement::CGerberMacroStatement(const CStringArray& parameters,const CString& statement)
: m_statement(statement)
{
   m_parameters.Copy(parameters);
}

CString CGerberMacroStatement::getStringAt(int index)
{
   CString parameter = (index >= 0 && index < m_parameters.GetCount())?m_parameters.GetAt(index):"";

   return parameter;
}

CString CGerberMacroStatement::getApertureDefinitionParameterDelimeter()
{
   return CString("X");
}

CString CGerberMacroStatement::getApertureDefinitionDelimeter()
{
   return CString(",");
}

//_____________________________________________________________________________
CGerberMacroPrimitive::CGerberMacroPrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroStatement(parameters,statement)
{
}

bool CGerberMacroPrimitive::getOnFlag() const
{
   bool retval = (m_parameters.GetAt(1).Compare("1") == 0);

   return retval;
}

bool CGerberMacroPrimitive::getOffFlag() const
{
   bool retval = (m_parameters.GetAt(1).Compare("0") == 0);

   return retval;
}
//
//CString CGerberMacroPrimitive::getStatement()
//{
//   CString statement;
//
//   for (int index = 0;index < m_parameters.GetSize();index++)
//   {
//      if (index > 0)
//      {
//         statement += ",";
//      }
//
//      statement += m_parameters.GetAt(index);
//   }
//
//   return statement;
//}

//_____________________________________________________________________________
CGerberMacroCirclePrimitive::CGerberMacroCirclePrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

int CGerberMacroCirclePrimitive::getDiameter() const
{
   int diameter = atoi(m_parameters.GetAt(1));

   return diameter;
}

CPoint CGerberMacroCirclePrimitive::getCenter() const
{
   CPoint center;
   center.x = atoi(m_parameters.GetAt(2));
   center.y = atoi(m_parameters.GetAt(3));

   return center;
}

void CGerberMacroCirclePrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   // 1,Exposure,Diameter,x,y
   int    exposure = symbolTable.getIntResult(getStringAt(1));
   double diameter = symbolTable.getDoubleResult(getStringAt(2)) * unitscale;
   double        x = symbolTable.getDoubleResult(getStringAt(3)) * unitscale;
   double        y = symbolTable.getDoubleResult(getStringAt(4)) * unitscale;
}

CString CGerberMacroCirclePrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(   getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(2)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(3)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(4));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroLinePrimitive::CGerberMacroLinePrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

int CGerberMacroLinePrimitive::getWidth() const
{
   int width = atoi(m_parameters.GetAt(1));

   return width;
}

CPoint CGerberMacroLinePrimitive::getStartPoint() const
{
   CPoint point;
   point.x = atoi(m_parameters.GetAt(2));
   point.y = atoi(m_parameters.GetAt(3));

   return point;
}

CPoint CGerberMacroLinePrimitive::getEndPoint() const
{
   CPoint point;
   point.x = atoi(m_parameters.GetAt(4));
   point.y = atoi(m_parameters.GetAt(5));

   return point;
}

void CGerberMacroLinePrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   //  2,Exposure,LineWidth,xStart,yStart,xEnd,yEnd,rotationDegrees
   // 20,Exposure,LineWidth,xStart,yStart,xEnd,yEnd,rotationDegrees
   m_exposure  = symbolTable.getIntResult(getStringAt(1));
   m_lineWidth = symbolTable.getDoubleResult(getStringAt(2)) * unitscale;
   m_xStart    = symbolTable.getDoubleResult(getStringAt(3)) * unitscale;
   m_yStart    = symbolTable.getDoubleResult(getStringAt(4)) * unitscale;
   m_xEnd      = symbolTable.getDoubleResult(getStringAt(5)) * unitscale;
   m_yEnd      = symbolTable.getDoubleResult(getStringAt(6)) * unitscale;
   m_degrees   = symbolTable.getDoubleResult(getStringAt(7));
}

CString CGerberMacroLinePrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(   getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(2)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(3)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(4)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(5)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(6)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(7));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroCenteredRectanglePrimitive::CGerberMacroCenteredRectanglePrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

void CGerberMacroCenteredRectanglePrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   // 21,Exposure,width,height,xCenter,yCenter,rotationDegrees
   int   exposure = symbolTable.getIntResult(getStringAt(1));
   double   width = symbolTable.getDoubleResult(getStringAt(2)) * unitscale;
   double  height = symbolTable.getDoubleResult(getStringAt(3)) * unitscale;
   double xCenter = symbolTable.getDoubleResult(getStringAt(4)) * unitscale;
   double yCenter = symbolTable.getDoubleResult(getStringAt(5)) * unitscale;
   double degrees = symbolTable.getDoubleResult(getStringAt(6));
}

CString CGerberMacroCenteredRectanglePrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(   getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(2)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(3)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(4)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(5)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(6));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroCorneredRectanglePrimitive::CGerberMacroCorneredRectanglePrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

void CGerberMacroCorneredRectanglePrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   // 22,Exposure,width,height,llx,lly,rotationDegrees
   int   exposure = symbolTable.getIntResult(getStringAt(1));
   double   width = symbolTable.getDoubleResult(getStringAt(2)) * unitscale;
   double  height = symbolTable.getDoubleResult(getStringAt(3)) * unitscale;
   double     llx = symbolTable.getDoubleResult(getStringAt(4)) * unitscale;
   double     lly = symbolTable.getDoubleResult(getStringAt(5)) * unitscale;
   double degrees = symbolTable.getDoubleResult(getStringAt(6));
}

CString CGerberMacroCorneredRectanglePrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(   getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(2)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(3)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(4)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(5)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(6));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroOutlinePrimitive::CGerberMacroOutlinePrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

void CGerberMacroOutlinePrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   // 4,Exposure,N(pointCount),xPoint1,yPoint1, ... ,xPointN,yPointN,rotationDegrees
   int    exposure = symbolTable.getIntResult(getStringAt(1));
   int  pointCount = symbolTable.getIntResult(getStringAt(2)) + 1;

   for (int index = 0;index < pointCount;index++)
   {
      double x = symbolTable.getDoubleResult(getStringAt(index*2 + 3)) * unitscale;
      double y = symbolTable.getDoubleResult(getStringAt(index*2 + 4)) * unitscale;
   }

   int rotationIndex = pointCount*2 + 2;

   double rotationDegrees = symbolTable.getDoubleResult(getStringAt(rotationIndex));
}

CString CGerberMacroOutlinePrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   int  pointCount = symbolTable.getIntResult(getStringAt(2)) + 1;

   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(getStringAt(2));

   for (int index = 0;index < pointCount;index++)
   {
      statement += getApertureDefinitionDelimeter() + symbolTable.getDoubleResultString(getStringAt(index*2 + 3)) + 
                   getApertureDefinitionDelimeter() + symbolTable.getDoubleResultString(getStringAt(index*2 + 4));
   }

   int rotationIndex = pointCount*2 + 3;

   statement += getApertureDefinitionDelimeter() + symbolTable.getDoubleResultString(getStringAt(rotationIndex));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroPolygonPrimitive::CGerberMacroPolygonPrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

void CGerberMacroPolygonPrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   // 5,Exposure,vertexCount,xCenter,yCenter,diameter,rotationDegrees
   int   exposure  = symbolTable.getIntResult(getStringAt(1));
   int vertexCount = symbolTable.getIntResult(getStringAt(2));
   double xCenter  = symbolTable.getDoubleResult(getStringAt(3)) * unitscale;
   double yCenter  = symbolTable.getDoubleResult(getStringAt(4)) * unitscale;
   double diameter = symbolTable.getDoubleResult(getStringAt(5)) * unitscale;
   double degrees  = symbolTable.getDoubleResult(getStringAt(6));
}

CString CGerberMacroPolygonPrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(   getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(   getStringAt(2)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(3)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(4)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(5)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(6));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroMoirePrimitive::CGerberMacroMoirePrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

void CGerberMacroMoirePrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   // 6,xCenter,yCenter,outsideDiameter,circleLineThickness,gap,circleCount,crossHairThickness,crossHairLength,rotation
   double xCenter         = symbolTable.getDoubleResult(getStringAt(1)) * unitscale;
   double yCenter         = symbolTable.getDoubleResult(getStringAt(2)) * unitscale;
   double outerDiameter   = symbolTable.getDoubleResult(getStringAt(3)) * unitscale;
   double circleWidth     = symbolTable.getDoubleResult(getStringAt(4)) * unitscale;
   int    circleCount     = symbolTable.getIntResult(getStringAt(5));
   double crossHairWidth  = symbolTable.getDoubleResult(getStringAt(6)) * unitscale;
   double crossHairLength = symbolTable.getDoubleResult(getStringAt(7)) * unitscale;
   double degrees         = symbolTable.getDoubleResult(getStringAt(8));
}

CString CGerberMacroMoirePrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(2)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(3)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(4)) + getApertureDefinitionDelimeter() +
      symbolTable.getIntResultString(   getStringAt(5)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(6)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(7)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(8));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroThermalPrimitive::CGerberMacroThermalPrimitive(const CStringArray& parameters,const CString& statement)
: CGerberMacroPrimitive(parameters,statement)
{
}

void CGerberMacroThermalPrimitive::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   // 7,xCenter,yCenter,outsideDiameter,insideDiameter,crossHairThickness,rotation
   double xCenter         = symbolTable.getDoubleResult(getStringAt(1)) * unitscale;
   double yCenter         = symbolTable.getDoubleResult(getStringAt(2)) * unitscale;
   double outerDiameter   = symbolTable.getDoubleResult(getStringAt(3)) * unitscale;
   double innerDiameter   = symbolTable.getDoubleResult(getStringAt(4)) * unitscale;
   double crossHairWidth  = symbolTable.getDoubleResult(getStringAt(5)) * unitscale;
   double degrees         = symbolTable.getDoubleResult(getStringAt(6));
}

CString CGerberMacroThermalPrimitive::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement = /*getApertureDefinitionPrefix() +*/
      getStringAt(0) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(1)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(2)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(3)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(4)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(5)) + getApertureDefinitionDelimeter() +
      symbolTable.getDoubleResultString(getStringAt(6));

   return statement;
}

//_____________________________________________________________________________
CGerberMacroExpression::CGerberMacroExpression(const CStringArray& parameters,const CString& statement)
: CGerberMacroStatement(parameters,statement)
{
}

void CGerberMacroExpression::evaluate(CGerberMacroSymbolTable& symbolTable)
{
   CString variableName    = m_parameters.GetAt(0);
   CString expressionValue = m_parameters.GetAt(1);

   symbolTable.assign(variableName,expressionValue);
}

CString CGerberMacroExpression::getStatement(CGerberMacroSymbolTable& symbolTable)
{
   CString statement;

   for (int index = 0;index < m_parameters.GetSize();index++)
   {
      if (index > 0)
      {
         statement += "=";
      }

      statement += m_parameters.GetAt(index);
   }

   return statement;
}

//_____________________________________________________________________________
CGerberMacro::CGerberMacro(const CString& macroDefinitionString)
{
   if (! macroDefinitionString.IsEmpty())
   {
      CSupString macroDefinition(macroDefinitionString);

      if (macroDefinition.Right(1) == "%")
      {
         macroDefinition.GetBufferSetLength(macroDefinition.GetLength() - 1);
      }

      CStringArray statements;

      int statementCount = macroDefinition.Parse(statements,"*");

      if (statementCount > 0)
      {
         m_name = statements.GetAt(0);

         if (m_name.Left(3) == "%AM")
         {
            m_name = m_name.Mid(3);
         }
         else if (m_name.Left(2) == "AM")
         {
            m_name = m_name.Mid(2);
         }
         else
         {
            logError("Expected aperture macro definition but '%s' found in '%s'\n",m_name,__FUNCTION__);
         }

         for (int statementIndex = 1;statementIndex < statementCount;statementIndex++)
         {
            CSupString statement = statements.GetAt(statementIndex);

            if (statement.GetLength() > 0 && statement.Left(1) == "0")
            {
               // comment
               continue;
            }

            CStringArray params;

            int paramCount = statement.Parse(params,"=");

            if (paramCount > 1)
            {
               CGerberMacroExpression* expression = new CGerberMacroExpression(params,statement);

               m_statements.AddTail(expression);
            }
            else
            {
               paramCount = statement.Parse(params,",");
               CGerberMacroPrimitive* macroPrimitive = NULL;

               if (paramCount > 1)
               {
                  int primitiveType = atoi(params.GetAt(0));

                  switch (primitiveType)
                  {
                  case  1:  macroPrimitive = new CGerberMacroCirclePrimitive(params,statement);             break;
                  case  2:
                  case 20:  macroPrimitive = new CGerberMacroLinePrimitive(params,statement);               break;
                  case 21:  macroPrimitive = new CGerberMacroCenteredRectanglePrimitive(params,statement);  break;
                  case 22:  macroPrimitive = new CGerberMacroCorneredRectanglePrimitive(params,statement);  break;
                  case  4:  macroPrimitive = new CGerberMacroOutlinePrimitive(params,statement);            break;
                  case  5:  macroPrimitive = new CGerberMacroPolygonPrimitive(params,statement);            break;
                  case  6:  macroPrimitive = new CGerberMacroMoirePrimitive(params,statement);              break;
                  case  7:  macroPrimitive = new CGerberMacroThermalPrimitive(params,statement);            break;
                  }

                  if (macroPrimitive != NULL)
                  {
                     m_statements.AddTail(macroPrimitive);
                  }
               }
            }
         }
      }
   }
}

void CGerberMacro::instantiateMacro(CCamCadDatabase& camCadDatabase,CGerberMacroSymbolTable& symbolTable,int fileNumber,int dcode) const
{
   BlockStruct* apertureGeometry = NULL;
   CBasesVector apertureBasesVector;

   int primitiveCount = 0;
   int circleCount    = 0;
   int lineCount      = 0;

   CGerberMacroCirclePrimitive* circle1 = NULL;
   CGerberMacroCirclePrimitive* circle2 = NULL;
   CGerberMacroLinePrimitive*   line1   = NULL;
   CGerberMacroLinePrimitive*   line2   = NULL;

   for (POSITION pos = m_statements.GetHeadPosition();pos != NULL;)
   {
      CGerberMacroStatement* statement = m_statements.GetNext(pos);

      statement->evaluate(symbolTable);

      switch (statement->getType())
      {
      case gerberMacroStatementTypeCirclePrimitive:  
         primitiveCount++;
         circleCount++;

         if (circleCount == 1)
         {
            circle1 = (CGerberMacroCirclePrimitive*)statement;
         }

         if (circleCount == 2)
         {
            circle2 = (CGerberMacroCirclePrimitive*)statement;

            if (circle1->getDiameter() < circle2->getDiameter())
            {
               circle2 = circle1;
               circle1 = (CGerberMacroCirclePrimitive*)statement;
            }
         }

         break;
      case gerberMacroStatementTypeLinePrimitive:
         primitiveCount++;
         lineCount++;

         if (lineCount == 1)
         {
            line1 = (CGerberMacroLinePrimitive*)statement;
         }

         if (lineCount == 2)
         {
            line2 = (CGerberMacroLinePrimitive*)statement;
         }

         break;
      case gerberMacroStatementTypeCenteredRectanglePrimitive:
      case gerberMacroStatementTypeCorneredRectanglePrimitive:
      case gerberMacroStatementTypeOutlinePrimitive:
      case gerberMacroStatementTypePolygonPrimitive:
      case gerberMacroStatementTypeMoirePrimitive:
      case gerberMacroStatementTypeThermalPrimitive:
      case gerberMacroStatementTypeExpression:
         primitiveCount++;

         break;
      }
   }

   bool optimizedFlag = false;

   if (primitiveCount == 3 && circleCount == 2 && lineCount == 1)
   {
      // oblong
      int width = line1->getWidth();

      if (circle1->getDiameter() == width && circle2->getDiameter() == width)
      {
         CPoint center1 = circle1->getCenter();
         CPoint center2 = circle2->getCenter();
         CPoint start   = line1->getStartPoint();
         CPoint end     = line1->getEndPoint();

         if ((center1 == start && center2 == end) ||
             (center2 == start && center1 == end)    )
         {
            optimizedFlag = true;

            CPoint2d point0(start);
            CPoint2d point1(end);

            double oblongWidth  = width;
            double oblongLength = point0.distance(point1) + 2.*oblongWidth;

            CStandardAperture standardAperture(apertureOblong,oblongWidth,oblongLength,0.,0.,camCadDatabase.getPageUnits());
            apertureGeometry = standardAperture.getDefinedAperture(camCadDatabase.getCamCadData(),fileNumber);
         }
      }
   }
   else if (primitiveCount == 2 && circleCount == 2)
   {
      // donut
      if (circle1->getOnFlag() && circle2->getOffFlag())
      {
         optimizedFlag = true;

         double outerDiameter = circle1->getDiameter();
         double innerDiameter = circle2->getDiameter();

         CStandardAperture standardAperture(apertureThermal,outerDiameter,innerDiameter,0.,0.,camCadDatabase.getPageUnits());
         apertureGeometry = standardAperture.getDefinedAperture(camCadDatabase.getCamCadData(),fileNumber);
      }
   }
   else if (primitiveCount == 4 && circleCount == 2 && lineCount == 2)
   {
      // thermal
      if (line1->getOffFlag() && line2->getOffFlag())
      {
         if (circle1->getOnFlag() && circle2->getOffFlag())
         {
            optimizedFlag = true;

            double outerDiameter   = circle1->getDiameter();
            double innerDiameter   = circle2->getDiameter();
            double gapWidth        = line1->getWidth();
            double gapAngleRadians = degreesToRadians(45.);

            CStandardAperture standardAperture(apertureThermal,outerDiameter,innerDiameter,gapWidth,gapAngleRadians,camCadDatabase.getPageUnits());
            apertureGeometry = standardAperture.getDefinedAperture(camCadDatabase.getCamCadData(),fileNumber);
         }
      }
   }

   if (!optimizedFlag && primitiveCount > 0)
   {
      bool complexFlag = (primitiveCount > 1);

      if (complexFlag)
      {
         CString prefixFormat;
         prefixFormat.Format("D%d_%s_%%d",dcode,m_name);

         BlockStruct* apertureGeometry = camCadDatabase.getNewBlock(prefixFormat,blockTypeUnknown,fileNumber);

         for (POSITION pos = m_statements.GetHeadPosition();pos != NULL;)
         {
            CGerberMacroStatement* statement = m_statements.GetNext(pos);
            CBasesVector subApertureBasesVector;

            BlockStruct* subApertureGeometry = instantiatePrimitiveAperture(camCadDatabase,fileNumber,*statement,subApertureBasesVector);

            if (subApertureGeometry != NULL)
            {
               DataStruct* subApertureData = camCadDatabase.insertBlock(subApertureGeometry,insertTypeUnknown,NULL,camCadDatabase.getFloatingLayerIndex(),
                  subApertureBasesVector.getOrigin().x,subApertureBasesVector.getOrigin().y,subApertureBasesVector.getRotationRadians());

               apertureGeometry->getDataList().AddTail(subApertureData);
            }
         }
      }
      else
      {
         CGerberMacroStatement* gerberMacroStatement = m_statements.GetHead();

         apertureGeometry = instantiatePrimitiveAperture(camCadDatabase,fileNumber,*gerberMacroStatement,apertureBasesVector);
      }
   }
}

BlockStruct* CGerberMacro::instantiatePrimitiveAperture(CCamCadDatabase& camCadDatabase,int fileNumber,CGerberMacroStatement& gerberMacroStatement,CBasesVector& apertureBasesVector) const
{
   BlockStruct* apertureGeometry = NULL;

   return apertureGeometry;
}

CString CGerberMacro::getMacroString(CCamCadDatabase& camCadDatabase,CGerberMacroSymbolTable& symbolTable) const
{
   CString macroString;

   for (POSITION pos = m_statements.GetHeadPosition();pos != NULL;)
   {
      CGerberMacroStatement* statement = m_statements.GetNext(pos);

      statement->evaluate(symbolTable);

      if (statement->getType() != gerberMacroStatementTypeExpression)
      {
         macroString += statement->getStatement(symbolTable) + "*\n";
      }
   }

   return macroString;
}

//_____________________________________________________________________________
CGerberMacro* CGerberMacroMap::addMacroDefinition(const CString& macroDefinitionString)
{
   CGerberMacro* macro = new CGerberMacro(macroDefinitionString);
   CString macroName = macro->getName();

   CGerberMacro* existingMacro = getMacro(macroName);

   if (macroName.IsEmpty() || existingMacro != NULL)
   {
      delete macro;

      logError("duplicate or illegal macro name of '%s' encountered in '%s'\n",macroName,__FUNCTION__);

      macro = existingMacro;
   }
   else
   {
      CString macroNameKey(macroName);
      macroNameKey.MakeLower();

      m_map.SetAt(macroNameKey,macro);
   }

   return macro;
}

bool CGerberMacroMap::hasMacro(const CString& macroName)
{
   CGerberMacro* macro = getMacro(macroName);

   bool retval = (macro != NULL); 

   return retval;
}

CGerberMacro* CGerberMacroMap::getMacro(const CString& macroName)
{
   CString macroNameKey(macroName);
   macroNameKey.MakeLower();
   CGerberMacro* macro = NULL;

   m_map.Lookup(macroNameKey,macro);

   return macro;
}

CGerberMacro& CGerberMacroMap::getDefinedMacro(const CString& macroName)
{
   CString macroNameKey(macroName);
   macroNameKey.MakeLower();
   CGerberMacro* macro;

   if (! m_map.Lookup(macroNameKey,macro))
   {
      CString macroDefinitionString = "%AM" + macroName;

      macro = new CGerberMacro(macroDefinitionString);

      m_map.SetAt(macroNameKey,macro);
   }

   return *macro;
}

//_____________________________________________________________________________
CGerberMacros*   CGerberMacros::m_gerberMacros = NULL;
CCamCadDatabase* CGerberMacros::m_allocatedCamCadDatabase = NULL;

CGerberMacros::CGerberMacros(CCamCadDatabase& camCadDatabase)
: m_camCadDatabase(camCadDatabase)
{
}

void CGerberMacros::empty()
{
}

CGerberMacro* CGerberMacros::addMacroString(const CString& macroString,int fileIndex)
{
   CGerberMacroMap* map = NULL;

   if (fileIndex < m_array.GetSize())
   {
      map = m_array.GetAt(fileIndex);
   }

   if (map == NULL)
   {
      map = new CGerberMacroMap();
      m_array.SetAtGrow(fileIndex,map);
   }

   CGerberMacro* macro = map->addMacroDefinition(macroString);

   return macro;
}

CGerberMacro& CGerberMacros::getDefinedMacro(const CString& macroName,int fileIndex)
{
   CGerberMacroMap* map = NULL;

   if (fileIndex < m_array.GetSize())
   {
      map = m_array.GetAt(fileIndex);
   }

   if (map == NULL)
   {
      map = new CGerberMacroMap();
      m_array.SetAtGrow(fileIndex,map);
   }

   CGerberMacro& macro = map->getDefinedMacro(macroName);

   return macro;
}

CString CGerberMacros::getMacrosDefinitions(const CString& definitionsString,int fileIndex)
{
   CString macrosString,delimeter;

   CSupString definitions(definitionsString);
   CStringArray definitionArray;
   bool standardApertureFlag;

   int numDefinitions = definitions.Parse(definitionArray,"*");

   for (int index = 0;index < numDefinitions;index++)
   {
      CString definitionString = definitionArray.GetAt(index);
      CString primitivesString = getMacroPrimitivesString(definitionString,fileIndex,standardApertureFlag);

      if (!standardApertureFlag)
      {
         macrosString += primitivesString + delimeter;

         delimeter = "*";
      }
   }

   return macrosString;
}

CString CGerberMacros::getMacrosPrimitiveStrings(const CString& definitionsString,int fileIndex)
{
   CString macrosString,delimeter;

   CSupString definitions(definitionsString);
   CStringArray definitionArray;
   bool standardApertureFlag;

   int numDefinitions = definitions.Parse(definitionArray,"*");

   for (int index = 0;index < numDefinitions;index++)
   {
      CString definitionString = definitionArray.GetAt(index);
      CString primitivesString = getMacroPrimitivesString(definitionString,fileIndex,standardApertureFlag);

      if (!standardApertureFlag)
      {
         macrosString += primitivesString + delimeter;

         delimeter = "*";
      }
   }

   return macrosString;
}

CString CGerberMacros::getMacroPrimitivesString(const CString& definitionString,int fileIndex,bool& standardApertureFlag)
{
   CString macroString;

   // %ADD<D-code number><aperture type>,<modifier>[X<modifer>]*%
   const char* p = definitionString;

   if (*p == '%')
   {
      p++;
   }

   if (p[0] == 'A' && p[1] == 'D' && p[2] == 'D')
   {
      p += 3;

      int dcode = 0;

      while (isdigit(*p))
      {
         dcode = 10*dcode + (*p - '0');
         p++;
      }

      macroString = p;
      CSupString definition(p);
      CStringArray params;

      int numPar = definition.Parse(params,",");

      if (numPar > 0)
      {
         CString apertureType = params.GetAt(0);

         CSupString modifiersString;
         CStringArray modifiers;
         int modifierCount = 0;

         if (numPar > 1)
         {
            modifiersString = params.GetAt(1);
            modifierCount = modifiersString.Parse(modifiers,"X");
         }

         if (apertureType.CompareNoCase("C") == 0)
         {
            //instantiateCircleStandardAperture(modifiers,dcode);
            macroString = p;
            standardApertureFlag = true;
         }
         else if (apertureType.CompareNoCase("R") == 0)
         {
            //instantiateRectangleStandardAperture(modifiers,dcode);
            macroString = p;
            standardApertureFlag = true;
         }
         else if (apertureType.CompareNoCase("O") == 0)
         {
            //instantiateObroundStandardAperture(modifiers,dcode);
            macroString = p;
            standardApertureFlag = true;
         }
         else if (apertureType.CompareNoCase("P") == 0)
         {
            //instantiateRegularPolygonStandardAperture(modifiers,dcode);
            macroString = p;
            standardApertureFlag = true;
         }
         else if (apertureType.CompareNoCase("T") == 0)
         {
            // ???
            macroString = p;
            standardApertureFlag = true;
         }
         else
         {
            // aperture macro
            CGerberMacro& macro = getDefinedMacro(apertureType,fileIndex);
            CGerberMacroSymbolTable symbolTable(modifiers);

            //macro.instantiateMacro(m_camCadDatabase,symbolTable,fileIndex,dcode);
            macroString = macro.getMacroString(m_camCadDatabase,symbolTable);
            standardApertureFlag = false;
         }
      }
   }

   return macroString;
}

void CGerberMacros::instantiateMacros(const CString& definitionsString,int fileIndex)
{
   CSupString definitions(definitionsString);
   CStringArray definitionArray;

   int numDefinitions = definitions.Parse(definitionArray,"*");

   for (int index = 0;index < numDefinitions;index++)
   {
      CString definitionString = definitionArray.GetAt(index);

      instantiateMacro(definitionString,fileIndex);
   }
}

void CGerberMacros::instantiateMacro(const CString& definitionString,int fileIndex)
{
   // %ADD<D-code number><aperture type>,<modifier>[X<modifer>]*%
   const char* p = definitionString;

   if (*p == '%')
   {
      p++;
   }

   if (p[0] == 'A' && p[1] == 'D' && p[2] == 'D')
   {
      p += 3;

      int dcode = 0;

      while (isdigit(*p))
      {
         dcode = 10*dcode + (*p - '0');
         p++;
      }

      CSupString definition(p);
      CStringArray params;

      int numPar = definition.Parse(params,",");

      if (numPar > 0)
      {
         CString apertureType = params.GetAt(0);

         CSupString modifiersString;
         CStringArray modifiers;
         int modifierCount = 0;

         if (numPar > 1)
         {
            modifiersString = params.GetAt(1);
            modifierCount = modifiersString.Parse(modifiers,"X");
         }

         if (apertureType.CompareNoCase("C") == 0)
         {
            instantiateCircleStandardAperture(modifiers,dcode);
         }
         else if (apertureType.CompareNoCase("R") == 0)
         {
            instantiateRectangleStandardAperture(modifiers,dcode);
         }
         else if (apertureType.CompareNoCase("O") == 0)
         {
            instantiateObroundStandardAperture(modifiers,dcode);
         }
         else if (apertureType.CompareNoCase("P") == 0)
         {
            instantiateRegularPolygonStandardAperture(modifiers,dcode);
         }
         else if (apertureType.CompareNoCase("T") == 0)
         {
            // ???
         }
         else
         {
            // aperture macro
            CGerberMacro& macro = getDefinedMacro(apertureType,fileIndex);
            CGerberMacroSymbolTable symbolTable(modifiers);

            macro.instantiateMacro(m_camCadDatabase,symbolTable,fileIndex,dcode);
         }
      }
   }
}

void CGerberMacros::instantiateCircleStandardAperture(const CStringArray& modifiers,int dcode) const
{
   // Circle
   // <outside diameter>[X<X-axis hole dimension >[X<Y-axis hole dimension>]]
   // To define a solid aperture, enter only
   // the diameter. To define a hole, enter one
   // dimension for a round hole, two for a
   // rectangle. The hole must fit within the
   // aperture. For a square hole, X must equal Y.
   // Both aperture and hole will be centered.

   int modifierCount = modifiers.GetCount();

   if (modifierCount > 0)
   {
      double outsideDiameter = atof(modifiers.GetAt(0)) * unitscale;

      if (modifierCount > 1)
      {
         // only allow round holes
         double holeDiameter = atof(modifiers.GetAt(1)) * unitscale;

         Check_Aperture(getApertureName(),T_DONUT,outsideDiameter,holeDiameter,0.,0.,0.,dcode,BL_APERTURE);
      }
      else
      {
         Check_Aperture(getApertureName(),T_ROUND,outsideDiameter,0.,0.,0.,0.,dcode,BL_APERTURE);
      }
   }
   else
   {
      logError("Invalid modifier count of %d in '%s'\n",modifierCount,__FUNCTION__);
   }
}

void CGerberMacros::instantiateRectangleStandardAperture(const CStringArray& modifiers,int dcode) const
{
   // Rectangle
   //<X-axis dimension>X<Y-axis dimension>[X<X-axis hole dimension>X<Y-axis hole dimension>]
   //Rectangle or square. May be solid or open. If
   //the X axis dimension equals the Y dimension,
   //the aperture will be square. To define a solid
   //aperture, enter only the X and Y dimensions;
   //omit the hole dimensions. To define a hole,
   //enter one dimension for a round hole, two for a
   //rectangle. The hole must fit within the
   //aperture. Both rectangle and hole will be
   //centered.         

   int modifierCount = modifiers.GetCount();

   if (modifierCount > 1)
   {
      double width  = atof(modifiers.GetAt(0)) * unitscale;
      double height = atof(modifiers.GetAt(1)) * unitscale;

      Check_Aperture(getApertureName(),T_RECTANGLE,width,height,0.,0.,0.,dcode,BL_APERTURE);
   }
   else
   {
      logError("Invalid modifier count of %d in '%s'\n",modifierCount,__FUNCTION__);
   }
}

void CGerberMacros::instantiateObroundStandardAperture(const CStringArray& modifiers,int dcode) const
{
   // Obround
   //<X-axis dimension>X<Y-axis dimension>[X<X-axis hole dimension>[X<Y-axis hole dimension>]]
   //Obround (oval). May be solid or open. If
   //the X dimension is larger than Y, the shape
   //will be horizontal. If the X dimension is
   //smaller than Y, the shape will be vertical. To
   //define a solid aperture, enter only the X and
   //Y dimensions; omit the hole dimensions. To
   //define a hole, enter one hole dimension for a
   //round hole, two for a rectangular or square
   //hole. If open, the hole must fit within the
   //aperture.        

   int modifierCount = modifiers.GetCount();

   if (modifierCount > 1)
   {
      double width  = atof(modifiers.GetAt(0)) * unitscale;
      double height = atof(modifiers.GetAt(1)) * unitscale;

      Check_Aperture(getApertureName(),T_OBLONG,width,height,0.,0.,0.,dcode,BL_APERTURE);
   }
   else
   {
      logError("Invalid modifier count of %d in '%s'\n",modifierCount,__FUNCTION__);
   }
}

void CGerberMacros::instantiateRegularPolygonStandardAperture(const CStringArray& modifiers,int dcode) const
{
   // Regular polygon
   //<outside dimension>X<number of sides>[X<degrees of rotation>[X<X-axis hole dimension>X<Y-axis hole dimension>]]
   //Regular polygon. May be solid or
   //open. To define a solid aperture, enter
   //only the outside dimension and
   //number of sides (3 to 12). The first
   //point is located on the X axis. May be
   //rotated ±360° from the X-axis. If
   //open, the hole must fit within the
   //outside dimension. Note: If you use
   //the hole dimension modifiers, you
   //must enter a rotation (even if it is 0).

   int modifierCount = modifiers.GetCount();

   if (modifierCount > 1)
   {
      int sideCount = atoi(modifiers.GetAt(1));

      if (sideCount > 2)
      {
         double outsideDiameter = atof(modifiers.GetAt(0)) * unitscale;
         double radius = outsideDiameter/2.;
         double rotationDegrees = 0.;

         if (modifierCount > 2)
         {
            rotationDegrees = atof(modifiers.GetAt(2));
         }

         double rot = degreesToRadians(rotationDegrees);
         CString pname = "P_" + getApertureName();
         Graph_Block_On(GBO_APPEND, pname, -1, BL_GLOBAL);  // generate sub block
         Graph_PolyStruct(getFloatingLayerIndex(), 0, 0);
         Graph_Poly(NULL,getZeroWidthIndex(),TRUE,FALSE,TRUE);          

         // The first point always start at the x-axis, so x is hypotenuse
         double firstX = 0.0;
         double firstY = 0.0;
         Rotate(radius, 0.0, rot, &firstX, &firstY);
         Graph_Vertex(unitscale * firstX, unitscale * firstY, 0.0);

         double deltaAngleRadians = 2.*Pi/sideCount;

         for (int i=1;i < sideCount;i++)
         {
            double angleRadians = deltaAngleRadians * i;
            double x = radius * cos(angleRadians);
            double y = radius * sin(angleRadians);
            double xrot = 0.0;
            double yrot = 0.0;

            Rotate(x, y, rot, &xrot, &yrot);
            Graph_Vertex(unitscale * xrot, unitscale * yrot, 0.0);
         }

         //Close the poly by adding the first point into the poly
         Graph_Vertex(unitscale * firstX, unitscale * firstY, 0.0);

         Graph_Block_Off();
         setUniqueApertureName();
         Graph_Complex(getApertureName(), G.cur_app, pname, 0.0, 0.0, 0.0);
      }
      else
      {
         logError("Invalid side count of %d in '%s'\n",sideCount,__FUNCTION__);
      }
   }
   else
   {
      logError("Invalid modifier count of %d in '%s'\n",modifierCount,__FUNCTION__);
   }
}

CGerberMacros& CGerberMacros::getGerberMacros()
{
   if (m_gerberMacros == NULL)
   {
      releaseGerberMacros();

      m_allocatedCamCadDatabase = new CCamCadDatabase(*doc);
      m_gerberMacros = new CGerberMacros(*m_allocatedCamCadDatabase);
   }

   return *m_gerberMacros;
}

CGerberMacros& CGerberMacros::getGerberMacros(CCamCadDatabase& camCadDatabase)
{
   releaseGerberMacros();

   m_gerberMacros = new CGerberMacros(camCadDatabase);

   return *m_gerberMacros;
}

void CGerberMacros::releaseGerberMacros()
{
   if(m_gerberMacros)
      delete m_gerberMacros;
   m_gerberMacros = NULL;

   if(m_allocatedCamCadDatabase)
      delete m_allocatedCamCadDatabase;
   m_allocatedCamCadDatabase = NULL;
}

#endif  // #ifdef newMacroProcessing


