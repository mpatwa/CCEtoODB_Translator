// $Header: /CAMCAD/5.0/read_wrt/Hp3070In.cpp 18    3/12/07 12:53p Kurt Van Ness $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"

#include "graph.h"             
#include "geomlib.h"
#include "pcbutil.h"
#include "format_s.h"
#include <math.h>
#include <string.h>
#include "gauge.h"
#include "attrib.h"
#include "menlib.h"  // just for date test
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
extern char         *devicetypes[];

#define  MAX_LINE                2000  /* Max line length.           */
#define  TEXT_HEIGHTFACTOR       (1.0/1.4)   // if a text is .12 is HP3070 ASCII, it is really only
                                       // 0.1 in graphic
#define  TEXT_WIDTHFACTOR        (0.55)   

#define  FID_ATT                 1
#define  SMD_ATT                 2
#define  DEVICE_ATT              3

/****************************************************************************/
/*
*/
typedef  struct
{
   char  *token;
} List;

static List command_lst[] =
{
   "SCALE",  
   "UNITS",     
   "INCLUDE",      
   "PLACEMENT",    
   "OUTLINE",      
   "TOOLING",      
   "KEEPOUT",   
   "NODE",         
   "OTHER",     
   "GROUP",     
   "DEVICES",      
   "END",          
};
#define  SIZ_OF_COMMAND (sizeof(command_lst) / sizeof(List))

typedef struct
{
   CString  name;
   int   atttype;
   int   devicetype; // see dbutil.h
}Attr;
typedef CTypedPtrArray<CPtrArray, Attr*> CAttrArray;

typedef struct 
{
   CString  compname;
   CString  pinname;
   double   x,y;
} HP3070CompPin;
typedef CTypedPtrArray<CPtrArray, HP3070CompPin*> CHP3070CompPinArray;

typedef struct
{
   double d;
   int    toolindex;
}HP3070Drill;
typedef CArray<HP3070Drill, HP3070Drill&> CDrillArray;

/****************************************************************************/

static   int      is_command(char *l);
static   char     *get_nextword(char *lpp, int *semicolon_found);
static   char     *get_nextline(char *string,int n,FILE *fp);
static   int      pads_skip();

static   double   cnv_unit(double x);

static   int      write_padstacks();
static   int      do_padstacklayers();
static   int      get_drillindex(double size, int layernum);

/****************************************************************************/
#define  HP3070ERR     "3070.log"


static   int         PageUnits;
static   FileStruct  *file = NULL;
static   FILE        *ferr;
static   FILE        *ifp;                            /* File pointers.    */
static   int         display_error = 0;
static   int         cur_filenum;

static   long        ifp_linecnt;
static   CCEtoODBDoc  *doc;
static   char        ifp_line[MAX_LINE];

static   int         PushTok = FALSE;
static   int         eof_found;
static   char        unit_flag = ' ';           /* I = inch M = metric A = mil B = basic */

static   CHP3070CompPinArray  comppinarray;  
static   int         comppincnt;

static   CAttrArray  attrarray;  
static   int         attrcnt;

static   CDrillArray drillarray;
static   int         drillcnt;

static   int         drilllayernum;

static   double      scale_factor = 1;

/****************************************************************************/
/*
*/
static int loop_to_end()
{
   char  *lp = NULL;
   int   semicolon_found = 0;
   int   code;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   break;
      if (strlen(lp) == 0) continue;   // do not get empty lines.

      if ((code = is_command(lp)) > -1)   break;
   }
   PushTok = TRUE;
   return 1;
}

/****************************************************************************/
/*
*/
static int fnull()
{
   loop_to_end();
   return 1;
}

/****************************************************************************/
/*
*/
static int is_attrib(const char *s,int atttype)
{
   int   i;

   for (i=0;i<attrcnt;i++)
   {
      if (attrarray[i]->atttype == atttype && !STRICMP(attrarray[i]->name,s))
         return 1;
   }

   return 0;
}

/****************************************************************************/
/*
*/
static int update_attr(char *lp,int typ)
{
   Attr  *c = new Attr;
   attrarray.SetAtGrow(attrcnt,c);  
   attrcnt++;  
   c->name     = lp;
   c->atttype  = typ;
   c->devicetype  = -1;
   return attrcnt-1;
}

/****************************************************************************/
/*
*/
static char *clean_quotes(const char *n)
{
   static CString tmp;

   tmp = n;
   if (tmp.Left(1) == "\"")   tmp.Delete(0,1);
   if (tmp.Right(1) == "\"")  tmp.Delete(strlen(tmp)-1, 1);

   return tmp.GetBuffer(0);
}

/****************************************************************************/
/*
*/
static char *get_nextword(char *lpp, int *semicolon_found)
{
   static   CString  lastlp;
   char     *lp;

   if (PushTok)
   {
      PushTok = FALSE;
      lp = lastlp.GetBuffer(0);
   }
   else
   {
      lp = strtok(NULL," ,\t\n");
   }

   while (lp == NULL)
   {  
      if (get_nextline(ifp_line,MAX_LINE,ifp))
      {
         // here maybe blank line
         if ((lp = strtok(ifp_line," ,\t\n")) == NULL)   continue;
         break;
      }
      else
      {
         return NULL;   // no more next line
      }
   }

   *semicolon_found = 0;
   if (lp)
   {
      CString  t;
      t = lp;
      t.TrimLeft();
      t.TrimRight();
      if (lp && (lp[strlen(lp)-1] == ';'))
      {
         *semicolon_found = 1;
         lp[strlen(lp)-1] = '\0';
      }
   }

   lastlp = lp;

   return lp;
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

/****************************************************************************/
/*
*/
static int do_boardxy()
{
   int   semicolon_found = 0;
   char  *lp = strtok(""," ,\t\n"); // init lp here


   while (TRUE)
   {
      if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   break;
      if (strlen(lp) == 0) continue;   // do not get empty lines.

      if (!STRCMPI(lp,"scale"))
      {
         if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   continue;
         if (strlen(lp))   scale_factor = atof(lp);
      }
      else
      if (!STRCMPI(lp,"units"))
      {
         if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   continue;
         if (!STRCMPI(lp,"mils"))
         {
            unit_flag = 'A';
         }
         else
         if (!STRCMPI(lp,"inches"))
         {
            unit_flag = 'I';
         }
         else
         {
            fprintf(ferr,"Unknown UNITS [%s] in board xy file at %ld\n",
               lp, ifp_linecnt);
         }
      }
      else
      if (!STRCMPI(lp,"placement"))
      {
         // x koo
         if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   continue;
         // y koo
         if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   continue;
         // rotation
         if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   continue;

      }
      else
      if (!STRCMPI(lp,"outline"))
      {
         int first_koo = TRUE;

         while (TRUE)
         {
            double   x,y;
            // x koo
            if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   
               break;
            x = cnv_unit(atof(lp));
            // y koo
            if ((lp = get_nextword(lp, &semicolon_found)) == NULL)
               break;
            y = cnv_unit(atof(lp));

            if (first_koo)
            {
               int level = Graph_Level("OUTLINE", "", 0);
               DataStruct *d = Graph_PolyStruct(level,0,0);
               d->setGraphicClass(GR_CLASS_BOARDOUTLINE);

               // need a function to shrink a polygon by width.
               Graph_Poly(NULL, 0, 0 , 0, 1);   // fillflag , negative, closed
               first_koo = FALSE;
            }
            Graph_Vertex(x,y,0);
            if (semicolon_found) break;
         }
      }
      else
      if (!STRCMPI(lp,"tooling"))
      {
         while (TRUE)
         {
            double   tooldiam, x,y;
      
            // the tooling section does not end with a
            // semicolon. every record does.
            // so I check is the next is not a number

            // x koo
            if ((lp = get_nextword(lp, &semicolon_found)) == NULL)
               break;
         
            if (strlen(lp) && isalpha(lp[0]))
            {
               PushTok = TRUE;
               break;
            }
            else
            {
               tooldiam = cnv_unit(atof(lp));

               // x koo
               if ((lp = get_nextword(lp, &semicolon_found)) == NULL)
                  break;
               x = cnv_unit(atof(lp));
               // y koo
               if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   
                  break;
               y = cnv_unit(atof(lp));
               
               int   drilllayernum = Graph_Level("DRILLHOLE","",0);
               // here place and make a tooling hole
               int drillindex = get_drillindex(tooldiam, drilllayernum);
               // here place a tool 
               CString  name;
               name.Format("TOOLING_%d",drillindex);
               DataStruct *d = Graph_Block_Reference(name, NULL, 0, x, y,
                  0.0, 0 , 1.0,Graph_Level("0","",1), TRUE);
               d->getInsert()->setInsertType(insertTypeDrillTool);
            }
         }
      }
      else
      if (!STRCMPI(lp,"node"))
      {
         double    x,y;
         CString  netname;

         // the node section does not end with a
         // semicolon. every record does.
         // so I check is the next is not a number
         PushTok = FALSE;
         if ((lp = strtok(NULL,"\n")) == NULL)
            break;
         netname = lp;
         netname.TrimLeft();
         netname.TrimRight();
         // eliminate start and end quotes
         netname = clean_quotes(netname);
         if (!get_nextline(ifp_line,MAX_LINE,ifp))
         {
            fprintf(ferr,"Next line expected after NODE in file board xy at %ld\n",
            ifp_linecnt);
            display_error++;
            break;
         }
         if ((lp = strtok(ifp_line," \t\n")) == NULL)
         {
            fprintf(ferr,"Keyword ALTERNATES expected in file board xy at %ld\n",
               ifp_linecnt);
            display_error++;
            break;
         }
         if (STRCMPI(lp,"ALTERNATES"))
         {
            fprintf(ferr,"Keyword ALTERNATES expected in file board xy at %ld\n",
               ifp_linecnt);
            display_error++;
            break;
         }

         PushTok = FALSE;
         while (get_nextline(ifp_line,MAX_LINE,ifp))
         {
            char  *lp;
               if ((lp = strtok(ifp_line," ,\t\n")) == NULL)   
               break;
            
            if (strlen(lp) && isalpha(lp[0]))
            {
               PushTok = TRUE;
               break;
            }
            else
            {
               // x koo
               x = cnv_unit(atof(lp));
               // y koo
               if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   
                  break;
               y = cnv_unit(atof(lp));
   
               // here place via
               DataStruct *d = Graph_Block_Reference("HP3070_VIA", NULL, -1, x, y,
                        DegToRad(0), 0 , 1.0,Graph_Level("0","",1), TRUE);
               d->getInsert()->setInsertType(insertTypeVia);
/*       
               doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
                  VT_STRING,
                  netname.GetBuffer(0),
                  0.0,0.0,0.0,0.0, 0.0,0,0, FALSE, SA_APPEND, 0L,0,0); // x, y, rot, height
*/
               while (!semicolon_found)
               {
                  if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   
                     break;
                  // here are attributes like BOTH, MANDATORY etc...
               }
            }
         }
      }
      else
      if (!STRCMPI(lp,"other"))
      {
         double    x,y;
         CString   compname, pinname;

         // the node section does not end with a
         // semicolon. every record does.
         // so I check is the next is not a number

         if (!get_nextline(ifp_line,MAX_LINE,ifp))
         {
            fprintf(ferr,"Next line expected after NODE in file board xy at %ld\n",
               ifp_linecnt);
            display_error++;
            break;
         }
         if ((lp = strtok(ifp_line," \t\n")) == NULL)
         {
            fprintf(ferr,"Keyword ALTERNATES expected in file board xy at %ld\n",
               ifp_linecnt);
            display_error++;
            break;
         }
         if (STRCMPI(lp,"ALTERNATES"))
         {
            fprintf(ferr,"Keyword ALTERNATES expected in file board xy at %ld\n",
               ifp_linecnt);
            display_error++;
            break;
         }

         PushTok = FALSE;
         while (get_nextline(ifp_line,MAX_LINE,ifp))
         {
            char  *lp;

            if ((lp = strtok(ifp_line," \t\n")) == NULL) 
               break;
               
            if (strlen(lp) && isalpha(lp[0]))
            {
               PushTok = TRUE;
               break;
            }
            else
            {
               // x koo
               x = cnv_unit(atof(lp));
               // y koo
               if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   
                  break;
               y = cnv_unit(atof(lp));
      
               if ((lp = strtok(ifp_line," .\t\n")) == NULL)   
                  break;
               compname = lp;
               if ((lp = strtok(ifp_line," \t\n")) == NULL) 
                  break;
               pinname = lp;

               HP3070CompPin  *c = new HP3070CompPin;
               comppinarray.SetAtGrow(comppincnt,c);  
               comppincnt++;  
               c->compname = compname;
               c->pinname = pinname;
               c->x = x;
               c->y = y;
               while (!semicolon_found)
               {
                  if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   
                     break;
                  // here are attributes like BOTH, MANDATORY etc...
               }
            }
         }
      }
      else
      if (!STRCMPI(lp,"devices"))
      {
         if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   break;
            
         while (is_command(lp) < 0)
         {
            if ((lp = get_nextword(lp, &semicolon_found)) == NULL)   
               break;
            // here are attributes like BOTH, MANDATORY etc...
         }
      }
      else
      {
         fprintf(ferr,"Unknown section [%s] in .bxy file at %ld\n",
            lp,ifp_linecnt);
         display_error++;
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int load_hp3070settings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp, "3070 Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".MIRRORLAYER"))
         {
            CString  lay1,lay2;

            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            lay1 = lp;
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            lay2 = lp;
            Graph_Level(lay1,"", 0);
            Graph_Level(lay2,"", 0);
            Graph_Level_Mirror(lay1, lay2, "");
         }
      }
   }

   fclose(fp);
   return 1;
}

/****************************************************************************/
/*
   make layers more understandable for HP3070 users.
*/
static int do_assign_layers()
{
   int layernr = Graph_Level("DECAL_0","",0);
   LayerStruct *l;
   l = doc->FindLayer(layernr);
   l->setComment("Component_Outline_Top");
   l->setLayerType(LAYTYPE_SILK_TOP);
   
   layernr = Graph_Level("DECAL_MIRROR_0","",0);
   l = doc->FindLayer(layernr);
   l->setComment("Component_Outline_Bottom");
   l->setLayerType(LAYTYPE_SILK_BOTTOM);

   layernr = Graph_Level("PADLAYER_-2","",0);
   l = doc->FindLayer(layernr);
   l->setComment("Pad Top");
   l->setLayerType(LAYTYPE_PAD_TOP);

   layernr = Graph_Level("PADLAYER_-1","",0);
   l = doc->FindLayer(layernr);
   l->setComment("Pad Inner");
   l->setLayerType(LAYTYPE_PAD_INNER);

   layernr = Graph_Level("PADLAYER_0","",0);
   l = doc->FindLayer(layernr);
   l->setComment("Pad Bottom");
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   // switch of layers all pad layers not 0, -1, -2


   return 1;
}

/******************************************************************************
* ReadHP3070
*/
void ReadHP3070(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits, int cur_filecnt, int tot_filecnt)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char f[_MAX_FNAME+4], fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   doc = Doc;
   file = NULL;
   display_error = FALSE;

   PageUnits = pageunits;

   // need to do this, because panel only knows the path.ext
   _splitpath( path_buffer, drive, dir, fname, ext );

   if (cur_filecnt > 0)
   {
      return;
   }
   else
   {
      if ((ferr = fopen(GetLogfilePath(HP3070ERR),"wt")) == NULL)
      {
         CString  t;
         t.Format("Error open [%s] file", HP3070ERR);
         ErrorMessage(t, "Error");
         return;
      }
      display_error = 0;
   }

   comppinarray.SetSize(100,100);
   comppincnt = 0;

   attrarray.SetSize(100,100);
   attrcnt = 0;

   drillarray.SetSize(100,100);
   drillcnt = 0;

   // if needs both files with the name
   // file.brd file.bxy in the same serach path.

   file = Graph_File_Start(fname, Type_HP3070);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());

   cur_filenum = file->getFileNumber();

   // settings must be loaded after fileinit.
   CString settingsFile( getApp().getImportSettingsFilePath("3070.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nHP3070 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_hp3070settings(settingsFile);

   strcpy(f,fname);
   strcat(f,".bxy");

   //scale_factor = scaleUnitsFactor;   // all units are normalized to INCHES

   // this can not be rt (because of LB terminator
   progress->SetStatus("Reading Board XY file");
   if ((ifp = fopen(f,"rt")) == NULL)
   {
      ErrorMessage("Error open file", f);
      return;
   }

   ifp_linecnt = 0;
   PushTok = FALSE;
   eof_found = FALSE;

   if (!do_boardxy())
   {  
      ErrorMessage("Error reading Board XY file");
   }

   fclose(ifp);

   // here now do_board();

   // here write all padstacks
   write_padstacks();
/*
   // here do padstack layers
   do_padstacklayers();

   if (unroutelayer)
   {
      int layernr = Graph_Level("UNROUT","",0);
      LayerStruct *l;
      l = doc->FindLayer(layernr);
      l->show = 0; // do not show
   }

   // here assign other layers
   do_assign_layers();
*/
	int i=0;
   for (i=0;i<comppincnt;i++)
   {
      delete comppinarray[i];
   }
   comppinarray.RemoveAll();

   for (i=0;i<attrcnt;i++)
   {
      delete attrarray[i];
   }
   attrarray.RemoveAll();

   drillarray.RemoveAll();

   fclose(ferr); 

   if (display_error)
      Logreader(GetLogfilePath(HP3070ERR));

   return;
}

/****************************************************************************/
/*
*/
static int do_padstacklayers()
{
   CString  lname;
   int      i;
   LayerStruct *l;

   // -2 is top
   lname.Format("PADLAYER_-2");
   i = Graph_Level(lname,"",0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_TOP);

   // -1 is inner
   lname.Format("PADLAYER_-1");
   i = Graph_Level(lname,"",0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_INNER);

   // 0 is bottom
   lname.Format("PADLAYER_0");
   i = Graph_Level(lname,"",0);
   l = doc->FindLayer(i);
   l->setLayerType(LAYTYPE_PAD_BOTTOM);

   return 1;
}

/****************************************************************************/
/*
*/
static int write_padstacks()
{
   int   err;
   CString  name;
   int   layernum = Graph_Level("VIAALL","",0);

   Graph_Aperture("HP3070_VIA_SHAPE", T_ROUND, 0.05, 0.0 , 0.0, 0.0, 0.0, 0, 
      BL_APERTURE, TRUE, &err);

   BlockStruct *curblock = Graph_Block_On(GBO_APPEND,"HP3070_VIA",-1,0);
   curblock->setBlockType(BLOCKTYPE_PADSTACK);

   // must be filenum 0, because apertures are global.
   Graph_Block_Reference("HP3070_VIA_SHAPE", NULL, 0, 0.0, 0.0, 0.0,
                         0, 1.0, layernum, TRUE);
   Graph_Block_Off();

   return 1;
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
      PushTok = FALSE;
   }
   else
   {
      int remark = TRUE;
      while (remark)
      {
         ifp_linecnt++;
         if ((res = fgets(string,MAX_LINE,fp)) == NULL)
         {
            eof_found = TRUE;
            return NULL;
         }
         last_line = string;
         last_line.TrimLeft();
         last_line.TrimRight();
         last_linecnt = ifp_linecnt;

         CString l = last_line.Left(1);
         if (l != "!")  remark = FALSE;
         last_res = res;
      }
   }
   
   // clean right 
   CString  tmp = string;
   tmp.TrimLeft();
   tmp.TrimRight();
   strcpy(string,tmp);

   return res;
}

/****************************************************************************/
/*
   From 
   {
      {
      }
   }  to blacket closed

   short    layertype;              // see dbutil.h dbutil.cpp
   short    artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
   short    electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
   short    physicalstacknumber;    // phusycal manufacturing stacking of layers, 
                                    // signaltop / dialectric / signal_1 / dia / power_1 .... / solder
   
   unsigned long attr;     // layer attributes

*/
static int  do_misclayer_layer(int layernr)
{
   int   bracket_cnt = 0;
   LayerStruct         *ll;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char  *lp;

      if ((lp = strtok(ifp_line," \t\n")) == NULL) continue;

      if (!strcmp(lp,"{")) bracket_cnt++;
      if (!strcmp(lp,"}")) bracket_cnt--;

      if (bracket_cnt == 0)   break;
      
      if (!STRCMPI(lp,"LAYER_NAME"))   // layername are comments.
      {
         CString layer_name;
         CString lname;
         if ((lp = strtok(NULL,"\n")) == NULL)     continue;
         layer_name = lp;
         layer_name.TrimLeft();
         layer_name.TrimRight();

         lname.Format("%d",layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            ll->setComment(layer_name);

         // here now check if a decal layer was 
         lname.Format("DECAL_%d",layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            ll->setComment(layer_name);

/*
         // here now check if a padlayer layer was - padlayers are not updated at this moment.
         // this is done in write_padstacks
         lname.Format("PADLAYER_%d",layernr);
         if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            ll->comment = layer_name;
*/
      }
      else
      if (!STRCMPI(lp,"LAYER_THICKNESS"))
      {
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         double h = cnv_unit(atof(lp));
         if (h > 0)
         {
            CString  lname;
            lname.Format("%d",layernr);

            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
            {
               doc->SetAttrib(&ll->getAttributesRef(),doc->IsKeyWord(LAYATT_THICKNESS, 0),
                  VT_UNIT_DOUBLE, &h, SA_OVERWRITE, NULL); //  
            }
         }
      }
      else
      if (!STRCMPI(lp,"LAYER_TYPE"))
      {
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;

         if (!STRCMPI(lp,"ROUTING"))
         {
            CString  lname;
            lname.Format("%d",layernr);   // always make routing layers
            int l = Graph_Level(lname,"",0);
            ll = doc->FindLayer(l);
            ll->setLayerType(LAYTYPE_SIGNAL);
         }
         else
         if (!STRCMPI(lp,"PLANE"))     // always make plane layers
         {
            CString  lname;
            lname.Format("%d",layernr);
            int l = Graph_Level(lname,"",0);
            ll = doc->FindLayer(l);
            ll->setLayerType(LAYTYPE_POWERNEG);
         }
         else
         if (!STRCMPI(lp,"PASTE_MASK"))
         {
            CString  lname;
            lname.Format("%d",layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               ll->setLayerType(LAYTYPE_PASTE_TOP);
         }
         else
         if (!STRCMPI(lp,"SOLDER_MASK"))
         {
            CString  lname;
            lname.Format("%d",layernr);
            if ((ll = doc->FindLayer_by_Name(lname)) != NULL)
               ll->setLayerType(LAYTYPE_MASK_TOP);
         }
      }
      else
      {
         //fprintf(ferr,"Unknown *MISC LAYER sub description [%s]\n",lp);
         //display_error++;
      }
   }

   return 1;
}

/****************************************************************************/
/*
   From 
   {
      {
      }
   }  to blacket closed
*/
static int  do_misclayer()
{
   int   bracket_cnt = 0;

   while (get_nextline(ifp_line,MAX_LINE,ifp))
   {
      char  *lp;

      if ((lp = strtok(ifp_line," \t\n")) == NULL) continue;

      if (!strcmp(lp,"{")) bracket_cnt++;
      if (!strcmp(lp,"}")) bracket_cnt--;

      if (bracket_cnt == 0)   break;
      
      if (!STRCMPI(lp,"LAYER"))
      {
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         int laynr = atoi(lp);
         do_misclayer_layer(laynr);
      }
      
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int get_drillindex(double size, int layernum)
{
   HP3070Drill p;
   int         i;

   if (size == 0) return -1;

   for (i=0;i<drillcnt;i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size)
         return p.toolindex;         
   }

   CString  name;
   name.Format("DRILL_%d",drillcnt);

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   CString  drillstackname;
   drillstackname.Format("DRILLHOLE_%d",i);

   // here build a drillhole, which has only a drill
   BlockStruct *curblock = Graph_Block_On(GBO_APPEND,drillstackname,-1,0);
   curblock->setBlockType(BLOCKTYPE_DRILLHOLE);
   
   // here make a drill aperture.
   int drillayer = Graph_Level("DRILLHOLE","",0);
   // must be filenum 0, because apertures are global.
   Graph_Block_Reference(name, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, drillayer, TRUE);
   Graph_Block_Off();

   // 
   CString  bname;
   bname.Format("TOOLING_%d",drillcnt);
   BlockStruct *b;
   b = Graph_Block_On(GBO_APPEND,bname,-1,0);
   b->setBlockType(BLOCKTYPE_TOOLING);
   Graph_Block_Reference(drillstackname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, drillayer, TRUE);
   Graph_Block_Off();

   p.d = size;
   p.toolindex = drillcnt;

   drillarray.SetAtGrow(drillcnt,p);  
   drillcnt++;

   return drillcnt-1;
}

/****************************************************************************/
/*
   Converts from HP3070 any units to INCH units.
*/
static double   cnv_unit(double x)
{
   double   faktor;

   switch (unit_flag)
   {
      case  'I':
         /* inch */
         faktor = Units_Factor(UNIT_INCHES, PageUnits);
      break;
      case  'M':
         /* metric */
         faktor = Units_Factor(UNIT_MM, PageUnits);
      break;
      case  'A':
         /* mil */
         faktor = Units_Factor(UNIT_MILS, PageUnits);
      break;
   }

   return(x * faktor * scale_factor);
}

/*Ende **********************************************************************/


