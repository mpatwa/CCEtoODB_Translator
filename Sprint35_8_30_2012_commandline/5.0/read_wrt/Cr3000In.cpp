// $Header: /CAMCAD/4.5/read_wrt/Cr3000In.cpp 38    8/04/06 8:55p Kurt Van Ness $

/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
          

  Need to do
  - LND_ padstack definitions
  - LIN_ correct padstack definitions
  - drill 
  - LDIM
  - ccf read speed
  - read pma
  - SMD pin recognition
  - Layer attributes
  - refnames
  - autoarc not working
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
#include "CR300IN.h"
#include "attrib.h"
#include "txtscan.h"
#include "lyrmanip.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)



extern CProgressDlg *progress;

/****************************************************************************/
/*
   Library for reading ZUKEN databases

     - Here only the COMP layer is translated, and not the SOLDER
       this means, that all definitions have to exist on COMP and must
       be equal to SOLDER
     - LDIM dimensions are not implemented.
     - Mixed Linewidth in one Linestring is not supported.
     - PMA file is not supported, that means that a user can not chang
       pin names for different MDF shapes.
     - Void areas (WINDOWS...) not supported.
     - LND as part of shape not implemented.


   *.bsf for technology and units
   *.mdf for library definition.
   *.udf for 2d lines and comp placement
   *.wdf for wires and vias.
   *.ccf for netlist

   *.pma for symbol - library assigment

   filled areas containing arcs are converted as non-filled.

*/
/****************************************************************************/

/* Function Prototype *******************************************************************/

static double get_unit(const char *t, int multiplier);
static void clean_blank(char *t);
static void clean_allblank(char *t);
static void clean_prosa(char *t);
static void globmem_init();
static void globmem_free();
static void init_terminal();
static void free_terminal();
static double cnv_unit(double x);
static int get_koo(char *t,double *x, double *y);
static int do_bsf(const char *fname);
static int do_mdf(const char *fname, const char *sname);
static int do_udf(const char *fname);
static int do_pma(const char *fname);
static int do_wdf(const char *fname);
static int do_ccf(const char *fname);
static int write_padstack();
static int Get_Word(FILE *fp,char *tmp,int n);
static int Get_Line(FILE *fp,char *tmp,int n);
static int do_artlayspec(int laynr, char *t);
static int skip_unknown(FILE *ifp);
static int do_lay(int type,char *t, int mirror);
static int do_netheader(char *t);
static int do_recheadr(char *t);
static int set_viatable(int via,int typ);
static int do_lin(char *, const char *netname);
static int update_land(int layer,int turret);
static int get_textptr(int app);
static int do_terminal(char *tok, int mirror);
static int get_terminal(const char *pinname);
static int get_smdpin(int termptr,int layernr,int appnr, int turretptr);
static int write_vertexpoints(char *layername,int appnr, int type, char *fname, int close, int fill);
static int get_vertexpoints(int number, int typ, int *turptr, int mirror);
static int do_vertexpoints(int number, int typ, int *turptr);
static int do_component(char *c, char *l, char *original);
static int check_terminals();
static int write_shapepin(int type, const char *shapename, const char *pinname, char *padname, double x, double y, int level);
static int get_artwork_from_wirelayer(int wirelay);
static int write_layerattr();
static int get_nextpin(FILE *fp, char *compname, char *pinname);
static int write_squarehole(char *layername,double x, double y, double h, double w, double rot);
static int get_wirespec_from_layernr(int lay);
static int make_arctopoly(double xs, double ys, double xe, double ye, double rx, double ry,
                              double da, int w, char *l, DbFlag flg);
static int load_cr3000settings(const CString fname);
static int get_turret(int app);
static int write_turret();
static void DoReverseOrder();
static void RenamePinRefdes(int compNumber, PinNameArray *pinArray, int pinCnt);
static int update_part(const char *p, int typ);
static int get_part(const char *p);

static void CopyAttribValues(FileStruct *file, WORD srcIndex, WORD destIndex, CCEtoODBDoc *doc, AttributeUpdateMethodTag method); 
static void CopyAttribValues(BlockStruct *block, const KeyWordStruct *srcKW, const KeyWordStruct *destKW, CCEtoODBDoc *doc, AttributeUpdateMethodTag method);

/* Globals Variables *******************************************************************/

static FILE             *ferr;
static FILE             *ifp;
static char             token[MAX_LINE];
static int              PushTok = FALSE;
static int              bracket_cnt;

static int              number_of_layers = 0;
static int              contour_lay_no = 0;

static double           scaleFactor;

static CCompArray       comparray;
static int              compcnt;

static ArtLaySpec       artlayspec[MAX_LAYERS];
static int              artlayspeccnt;

static WireLaySpec      wirelayspec[MAX_LAYERS];
static int              wirelayspeccnt;

static Viatable         *viatable;
static int              viatablecnt;

static Texttable        *texttable;
static int              texttablecnt;

static CTerminalArray   terminalarray;
static int              terminalcnt;

static Land             *land;
static int              landcnt;

static Smdpin           *smdpin;
static int              smdpincnt;

static Turret           *turret;
static int              turretcnt;

static Shape            cur_shape;
static NetName          cur_net;

static int              polycnt =0 ;
static Poly_l           *poly_l;

static CDrillArray      drillarray;
static int              drillcnt;

static CompNumberArray  compNumArray;
static int              compNumCnt;

static CString          compoutline[30];
static int              compoutlinecnt;

static CAttribmapArray  attribmaparray;
static int              attribmapcnt = 0;

static CString			copydevice;

static int              lnd_udf_error = FALSE;
static int              brd_cnt = 0;
static double           SMALL_RADIUS = 0.02;  // smallest radius to make a autoarc
static long             lcnt = 0;             // global linecnt

static FileStruct       *file = NULL;
static CCEtoODBDoc       *doc;
static int              display_error = 0;
static int              PageUnits;
static char             cur_file[OFS_MAXPATHNAME];
static int              mirrorlayers[MAX_LAYERS];

static int              USE_COMPSHAPE_NAME;
static int              USE_PIN_NAME;
static int              USE_SOLDER_MDF;
static int              REVERSE_ORDER;
static int              ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD
static CPartArray          partarray;
static int                 partcnt;

/******************************************************************************
* ReadCR3000
*/
void ReadCR3000(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits )
         
{
   // here check it there is a panel file also
   // open file for writting
   char f[_MAX_PATH];
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( filename, drive, dir, fname, ext );
   strcpy(f, drive);
   strcat(f, dir);
   strcat(f, fname);

   doc = Doc;
   PageUnits = pageUnits;

   // general log file
   CString cr3000LogFile = GetLogfilePath("cr3000.log");
   if ((ferr = fopen(cr3000LogFile, "wt")) ==NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", cr3000LogFile);
      ErrorMessage(t, "Error");
      return;
   }
   display_error = 0;

   globmem_init();

   CString settingsFile( getApp().getImportSettingsFilePath("cr3000.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nCR3000 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_cr3000settings(settingsFile);

   file = Graph_File_Start(fname, Type_CR3000_Layout);
   file->setBlockType(blockTypePcb);
   file->getBlock()->setBlockType(file->getBlockType());

   do_bsf(f);  // need to load first to see the units.
   write_turret();

   if (do_mdf(f, settingsFile) > -1)
   {
      if (USE_PIN_NAME)
         do_pma(f);

      do_udf(f);
      if (do_wdf(f) > -1)
      {
         do_ccf(f);
         //L_TLayers(number_of_layers);
         //L_TAttr((DbFlag)(TE_PHYSNUMBER));
         // Grid
         // Clearances
         // Default line width
         // write padstacks
         write_padstack();
         write_layerattr();

         OptimizePadstacks(doc, PageUnits, 0);           // this can create unused blocks
         update_smdpads(doc);
         RefreshInheritedAttributes(doc, attributeUpdateOverwrite);  // smd pins are after the pin instance is done.
         update_smdrule_geometries(doc, ComponentSMDrule);
         update_smdrule_components(doc, file, ComponentSMDrule);

         generate_PADSTACKACCESSFLAG(doc, 1);
         generate_PINLOC(doc, file, 1);   // this function generates the PINLOC argument for all pins.

         doc->purgeUnusedBlocks(); // use after optimize_PADSTACKS

         if (REVERSE_ORDER)
            DoReverseOrder();
      }
      else
      {
         ErrorMessage("File Import aborted due to read errors!");
         fprintf(ferr, "\nFile Import aborted due to read errors!\n");
         display_error++;
      }
   }

   // there are layes created, which are not needed !
   doc->RemoveUnusedLayers();

   if (copydevice.IsEmpty() == false)
   {
		WORD fromKWD = doc->RegisterKeyWord(ATT_TYPELISTLINK, 0, VT_STRING);
		WORD toKWD = doc->RegisterKeyWord(copydevice, 0, VT_STRING);
		CopyAttribValues(file, fromKWD, toKWD, doc, attributeUpdateOverwrite);
   }

   globmem_free();

   fclose(ferr);

   if (display_error)
      Logreader(cr3000LogFile);

   return;
}

/******************************************************************************
* DoReverseOrder
   - The function reverse the order of electical stack number and layer type is
     ".REVERSE_ORDER" is set to "Y" or "y"
   - Example of revered electrical stack number
         Layer    ElectricalStackNumber   ==>   After reversed order
         1        1                             3
         2        2                             2
         3        3                             1
   - Example of revered layer type
         Layer    Type        ==>   After reversed order
         1        PAD TOP           PAD BOTTOM
         2        PAD BOTTOM        PAD TOP
         3        SIGNAL TOP        SIGNAL BOTTOM
         4        SIGNAL BOTTOM     SIGNAL TOP
*/
static void DoReverseOrder()
{
   CStringArray electricalStackArray;
   electricalStackArray.SetSize(100, 100);

   CString stackNum = "";
   int maxElectricalNum = 0;
   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)
         continue;

      switch (layer->getLayerType())
      {
      case layerTypeSignalTop:
         layer->setLayerType(layerTypeSignalBottom);
         break;
      case layerTypeSignalBottom:            
         layer->setLayerType(layerTypeSignalTop);
         break;
      case layerTypePadTop:               
         layer->setLayerType(layerTypePadBottom);
         break;
      case layerTypePadBottom:            
         layer->setLayerType(layerTypePadTop);
         break;
      case layerTypePasteTop:             
         layer->setLayerType(layerTypePasteBottom);
         break;
      case layerTypePasteBottom:          
         layer->setLayerType(layerTypePasteTop);
         break;
      case layerTypeStencilTop:             
         layer->setLayerType(layerTypeStencilBottom);
         break;
      case layerTypeStencilBottom:          
         layer->setLayerType(layerTypeStencilTop);
         break;
      case layerTypeCentroidTop:             
         layer->setLayerType(layerTypeCentroidBottom);
         break;
      case layerTypeCentroidBottom:          
         layer->setLayerType(layerTypeCentroidTop);
         break;
      case layerTypeMaskTop:              
         layer->setLayerType(layerTypeMaskBottom);
         break;
      case layerTypeMaskBottom:           
         layer->setLayerType(layerTypeMaskTop);
         break;
      case layerTypeSilkTop:              
         layer->setLayerType(layerTypeSilkBottom);
         break;
      case layerTypeSilkBottom:           
         layer->setLayerType(layerTypeSilkTop);
         break;
      case layerTypeTop:          
         layer->setLayerType(layerTypeBottom);
         break;
      case layerTypeBottom:       
         layer->setLayerType(layerTypeTop);
         break;
      case layerTypeComponentDftTop:     
         layer->setLayerType(layerTypeComponentDftBottom);
         break;
      case layerTypeComponentDftBottom:  
         layer->setLayerType(layerTypeComponentDftTop);
         break;
      case layerTypeFluxTop:              
         layer->setLayerType(layerTypeFluxBottom);
         break;
      case layerTypeFluxBottom:           
         layer->setLayerType(layerTypeFluxTop);
         break;
      default:
         break;
      }

      if (layer->getElectricalStackNumber() < 1)
         continue;

      // Get the highest electrical stack number
      if (layer->getElectricalStackNumber() > maxElectricalNum)
         maxElectricalNum = layer->getElectricalStackNumber();
      
      // Save the electrical stack number to the corresponsing index in the electrialStackArray
      stackNum.Format("%d", layer->getElectricalStackNumber());
      electricalStackArray.SetAtGrow(layer->getElectricalStackNumber(), stackNum);
   }

   // Reverse the order of the electrical stack number
   int index1 = 1;
   int index2 = maxElectricalNum;
   while (index1 < index2)
   {
      CString stackNum1 = electricalStackArray[index1];
      if (stackNum1 == "")
      {
         index1++;
         continue;
      }

      CString stackNum2 = electricalStackArray[index2];
      while (stackNum2 == "" && index2 > 0)
      {
         index2--;
         stackNum2 = electricalStackArray.GetAt(index2);
      }

      electricalStackArray.SetAt(index1, stackNum2);
      electricalStackArray.SetAt(index2, stackNum1);
      index1++;
      index2--;
   }

   // Reassign the electrical stack number to the layers
   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)
         continue;
      if (layer->getElectricalStackNumber() < 1)
         continue;
      layer->setElectricalStackNumber(atoi(electricalStackArray[layer->getElectricalStackNumber()]));
   }

}

/******************************************************************************
* get_layerclass
*/
static GraphicClassTag get_layerclass(const char *layername)
{
   for (int i=0; i<compoutlinecnt; i++)
   {
      if (!STRCMPI(layername,compoutline[i]))
         return graphicClassComponentOutline; 
   }

   return graphicClassNormal;
}

/******************************************************************************
* get_attribmap
*/
static const char *get_attribmap(const char *c)
{
   for (int i=0; i<attribmapcnt; i++)
   {
      if (attribmaparray[i]->attrib.CompareNoCase(c) == 0)
         return attribmaparray[i]->mapattrib.GetBuffer(0);
   }

   return c;
}

/******************************************************************************
* get_drillindex
*/
static int get_drillindex(double size, int layernum)
{
   CR3000Drill p;

   if (size == 0)
      return -1;

   for (int i=0; i<drillcnt; i++)
   {
      p = drillarray.ElementAt(i);
      if (p.d == size)
         return p.toolindex;         
   }

   CString name;
   name.Format("DRILL_%d", drillcnt);

   Graph_Tool(name, 0, size, 0, 0, 0, 0L);

   p.d = size;
   p.toolindex = drillcnt;
   drillarray.SetAtGrow(drillcnt++, p);  

   return drillcnt-1;
}

/****************************************************************************/
/*
typedef struct
{
   int   number;
   char  type;
   double outer;
   double inner;
}
*/
static int write_turret()
{
   for (int i=0; i<turretcnt; i++)
   {
      int err;
      CString  cur_turretname;
      cur_turretname.Format("TURRET_%d", turret[i].number);

      if (turret[i].type == 'S')
         Graph_Aperture(cur_turretname,T_SQUARE, cnv_unit(turret[i].outer), 0.0, 0.0, 0.0, 0.0, 0, 0L, TRUE, &err);
      else
         Graph_Aperture(cur_turretname,T_ROUND, cnv_unit(turret[i].outer), 0.0, 0.0, 0.0, 0.0, 0, 0L, TRUE, &err);
   }

   return 1;
}

/****************************************************************************/
/*
  all values have to be processed after UNITS
*/
static int do_bsf(const char *fname)
{
   int   res;
   int   i;
   char  *lp;
   char  tmp[80];

   /* initialize scanning parameters */
   Set_Comment( "/*" , "*/" ); // Comment is done in function array
   Set_Text_Delimeter( "\"" );
   Set_Word_Delimeters( " \t\n:," );
   Set_White_Characters( " \t\n" );
   Set_Non_Characters( "\r\b\v" );
   sprintf(cur_file,"%s.bsf",fname);

   if ((ifp = fopen(cur_file,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",cur_file);
      ErrorMessage(tmp, "File Open Error", MB_OK | MB_ICONHAND);
      return 0;
   }

   if (progress != NULL)
      progress->SetStatus(cur_file);

   Reset_File(ifp);
   PushTok = 0;
   Set_Linecnt(1L);
   bracket_cnt = 0;

   res = Get_Word(ifp,token, MAX_LINE);
   if (strcmp(token,"$BSF"))
   {
      fprintf(ferr, "Keyword [$BSF] expected\n");
      display_error++;
      return -1;
   }

   // here do it.
   while ((res = Get_Word(ifp,token, MAX_LINE)) != EOF_CHARACTER)
   {
      if (!STRCMPI(token,"WIRE_LAY_COUNT"))
      {
         res = Get_Line(ifp,token, MAX_LINE);
         number_of_layers = atoi(token);
      }
      else
      if (!STRCMPI(token,"CONTOUR_LAY_NO"))
      {
         res = Get_Line(ifp,token, MAX_LINE);
         contour_lay_no = atoi(token);
      }
      else
      if (!STRCMPI(token,"PLACE_LAYER"))
      {
         res = Get_Line(ifp,token, MAX_LINE);   // Both
      }
      else
      if (!STRCMPI(token,"VIA_SPEC"))
      {
         res = Get_Line(ifp,token, MAX_LINE);   //
      }
      else
      if (!STRCMPI(token,"EDIT_MODE"))
      {
         res = Get_Line(ifp,token, MAX_LINE);   //
      }
      else
      if (!STRCMPI(token,"COMMENT"))
      {
         res = Get_Line(ifp,token, MAX_LINE);   //
         fprintf(ferr, " Comment : %s\n",token); 
      }
      else
      if (!STRCMPI(token,"EDIT_AREA_SIZE"))
      {
         res = Get_Line(ifp,token, MAX_LINE);   //
      }
      else
      if (!STRCMPI(token,"SUB_AXIS"))
      {
         res = Get_Line(ifp,token, MAX_LINE);   //
      }
      else
      if (!STRCMPI(token,"TRAP"))
      {
         res = Get_Line(ifp,token, MAX_LINE);   //
      }
      else
      if (!STRCMPI(token,"UNIT"))
      {
         CString  u;
         res = Get_Word(ifp,token, MAX_LINE);   //
         u = token;
         res = Get_Line(ifp,token, MAX_LINE);   // unit scale multiplier x ^10
         lp = strtok(token,":");

         scaleFactor = get_unit(u, atoi(lp));
      }
      else
      if (!STRCMPI(token,"VIA_TABLE"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               if (viatablecnt < MAX_VIATABLE)
               {
                  viatable[viatablecnt].number = atoi(token);
                  res = Get_Line(ifp,token, MAX_LINE);
                  if (lp = strtok(token,":"))
                     viatable[viatablecnt].holediameter = atof(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].comp_app = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].comp_app_unc = atoi(lp);;
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].comp_app_therm = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].comp_app_clear = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].inner_app = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].inner_app_unc = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].inner_app_therm = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].inner_app_clear = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].solder_app = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].solder_app_unc = atoi(lp);
                  if (lp = strtok(NULL,",:"))
                     viatable[viatablecnt].solder_app_therm = atoi(lp);
                  if (lp = strtok(NULL,";"))
                     viatable[viatablecnt].solder_app_clear = atoi(lp);

                  viatablecnt++;
               }
               else
               {
#ifdef _DEBUG
                  CString  t1;
                  t1.Format( " Too many Viatable");
                  ErrorMessage(t1);
#endif
                  fprintf(ferr, "Too many Viatable\n");
                  display_error++;
                  return -1;
               }
            }

         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"TEXT_TABLE"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               if (texttablecnt < MAX_TEXTTABLE)
               {
                  texttable[texttablecnt].nr = atoi(token);
                  res = Get_Line(ifp,token, MAX_LINE);
                  lp = strtok(token,":");   // height
                  texttable[texttablecnt].height = atof(lp);
                  lp = strtok(NULL,":");    // width
                  texttable[texttablecnt].width = atof(lp);
                  lp = strtok(NULL,":");    // space
                  texttable[texttablecnt].space = atof(lp);
                  texttablecnt++;
               }
               else
               {
#ifdef _DEBUG
                  CString  t1;
                  t1.Format( " Too many text table entries");
                  ErrorMessage(t1);
#endif
                  fprintf(ferr, "Too many text table entries\n");
                  display_error++;
                  return -1;
               }
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"TURRET"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               if (turretcnt < MAX_TURRET)
               {
                  turret[turretcnt].number = atoi(token);
                  res = Get_Line(ifp,token, MAX_LINE);
                  lp = strtok(token,":");
                  strcpy(tmp,lp);
                  clean_blank(tmp);
                  turret[turretcnt].type = tmp[0];
                  lp = strtok(NULL,":");
                  turret[turretcnt].outer = atof(lp);
                  if ((lp = strtok(NULL,":")) != NULL)
                     turret[turretcnt].inner = atof(lp);
                  turretcnt++;
               }
               else
               {
#ifdef _DEBUG
                  CString  t1;
                  t1.Format("Too many turrets");
                  ErrorMessage(t1);
#endif
                  fprintf(ferr, "Too many turrets\n");
                  display_error++;
                  return -1;
               }
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"WIRE_LAY_SPEC"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               if (wirelayspeccnt < MAX_LAYERS)
               {
                  wirelayspec[wirelayspeccnt].wirelay = atoi(token);
                  res = Get_Line(ifp,token, MAX_LINE);
                  lp = strtok(token,":");
                  wirelayspec[wirelayspeccnt].number = atoi(token);
                  wirelayspec[wirelayspeccnt].negative = FALSE;
                  if (lp = strtok(NULL,":"))
                  {
                     if (!STRNICMP(lp,"NEGA", 1))
                        wirelayspec[wirelayspeccnt].negative = TRUE;
                  }
                  wirelayspeccnt++;
               }
               else
               {
#ifdef _DEBUG
                  CString  t1;
                  t1.Format("Too many Wire Lay Spec");
                  ErrorMessage(t1);
#endif

                  fprintf (ferr, "Too many Wire Lay Spec\n");
                  display_error++;
                  return -1;
               }
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"CLEARANCE"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               res = Get_Line(ifp,token, MAX_LINE);
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"BASE_GRID"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               res = Get_Line(ifp,token, MAX_LINE);
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"WIND_LAY"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               res = Get_Line(ifp,token, MAX_LINE);
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"CRSP_LAY"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               int l1 = atoi(token);
               res = Read_Word(ifp,token, MAX_LINE);
               int l2 = atoi(token);

               if (l1 > 0 && l1 < MAX_LAYERS && l2 > 0 && l2 < MAX_LAYERS)
               {
                  mirrorlayers[l1] = l2;
                  mirrorlayers[l2] = l1;
               }
               res = Get_Line(ifp,token, MAX_LINE);
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"AREA_GRID"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               res = Get_Line(ifp,token, MAX_LINE);
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"ART_LAY_SPEC"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
            if (strcmp(token,"}"))
            {
               i = atoi(token); // layer number
               res = Get_Line(ifp,token, MAX_LINE);
               do_artlayspec(i,token);
            }
         } while (strcmp(token,"}"));
      }
      else
      if (!STRCMPI(token,"GRID_SET") ||
          !STRCMPI(token,"WIND_GRID") ||
          !STRCMPI(token,"DISPLAY") || 
          !STRCMPI(token,"EXCLUDE_NOUN") ||
          !STRCMPI(token,"LABEL_SIZE") ||
          !STRCMPI(token,"LINE_LOCK") ||
          !STRNICMP(token,"DF_",3) ||     // all defaults.
          !STRCMPI(token,"DIMS") ||
          !STRCMPI(token,"PRM_ASURF") ||
          !STRCMPI(token,"PRM_CHCNV") ||
          !STRCMPI(token,"PRM_ESURF") ||
          !STRCMPI(token,"PRM_GENPID") ||
          !STRCMPI(token,"PRM_HSCNV") ||
          !STRCMPI(token,"PRM_LCCNV") ||
          !STRCMPI(token,"PRM_RESIST") ||
          !STRCMPI(token,"PRM_SHCNV") ||
          !STRCMPI(token,"PRM_SILKCUT") ||
          !STRCMPI(token,"PRM_SSURF") ||
          !STRCMPI(token,"PRM_SUBLAND") ||
          !STRCMPI(token,"PRM_TEARDROP") ||
          !STRCMPI(token,"PRM_TLCNV") ||
          !STRCMPI(token,"PRM_VCCNV") ||
          !STRCMPI(token,"AGC_SIZ") ||
          !STRCMPI(token,"AGC_RST") ||
          !STRCMPI(token,"AGC_MTL") ||
          !STRCMPI(token,"AGC_DIP") ||
          !STRCMPI(token,"AGC_SIP") ||
          !STRCMPI(token,"AGC_DIS") ||
          !STRCMPI(token,"AGC_QFP") ||
          !STRCMPI(token,"AGC_SOP") ||
          !STRCMPI(token,"AGC_PGA") ||
          !STRCMPI(token,"AGC_ZIP") ||
          !STRCMPI(token,"AGC_TRAN") ||
          !STRCMPI(token,"AGC_3CHIP") ||
          !STRCMPI(token,"AGC_CHIP") ||
          !STRCMPI(token,"AGC_PAD")  )
      {
         // do nothing with known objects, just skip.
         skip_unknown(ifp);
      }
      else
      {
         fprintf(ferr,"Unknown token [%s] in bsf at %ld -> ",token,Get_Linecnt());
         skip_unknown(ifp);
         fprintf(ferr,"Skipped until %ld\n",Get_Linecnt());
         display_error++;
      }
   }

   int lay1, lay2;
   lay1 = get_artwork_from_wirelayer(1);
   lay2 = get_artwork_from_wirelayer(number_of_layers);

   if (lay1 > 0 && lay1 < MAX_LAYERS && lay2 > 0 && lay2 < MAX_LAYERS)
   {
      mirrorlayers[lay1] = lay2;
      mirrorlayers[lay2] = lay1;
   }

   fclose(ifp);
   return 1;
}

/****************************************************************************/
/*
*/
static int do_mdf(const char *fname, const char *sname)
{
   int      res, solder_found = FALSE;
   char     *lp;
   CString  tmp;
   double   height;

   /* initialize scanning parameters */
   Set_Comment( "/*" , "*/" ); // Comment is done in function array
   Set_Text_Delimeter( "\"" );
   Set_Word_Delimeters( " \t\n:," );
   Set_White_Characters( " \t\n" );
   Set_Non_Characters( "\r\b\v" );
   sprintf(cur_file, "%s.mdf", fname);

   if ((ifp = fopen(cur_file, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", cur_file);
      ErrorMessage(tmp, "File Open Error", MB_OK | MB_ICONHAND);
      return 0;
   }

   if (progress != NULL)
      progress->SetStatus(cur_file);

   Reset_File(ifp);
   PushTok = 0;
   Set_Linecnt(1L);
   bracket_cnt = 0;

   // here do it.
   while ((res = Get_Word(ifp,token, MAX_LINE)) != EOF_CHARACTER)
   {
      if (!STRCMPI(token, "$MDF"))  // every shape starts with an MDF 
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {
            res = Read_Word(ifp, token, MAX_LINE);
            // here cmp_headr expted
            if (strcmp(token, "CMP_HEADR"))
            {
               fprintf (ferr, "Token CMP_HEADR expected in mdf at %ld\n", Get_Linecnt());
               display_error++;
               return -1;
            }
            res = Get_Line(ifp, token, MAX_LINE);
            lp = strtok(token, ":");
            cur_shape.name = lp;
            cur_shape.number = atoi(lp);
            cur_shape.name.TrimLeft();
            cur_shape.name.TrimRight();

            init_terminal();        // reset counter for terminal array

            lp = strtok(NULL, ":"); // COMP or SOLDER
            tmp = lp;
            tmp.TrimLeft();
            tmp.TrimRight();

            if (!STRCMPI(tmp, "SOLDER"))
               solder_found = TRUE;

            lp = strtok(NULL, " \t:"); // number of pins
            lp = strtok(NULL, " \t;:"); // SMD for smd compent
                                   // NOTCR not a real comp but graphic
            if (lp && !STRCMPI(lp, "SMD"))
               cur_shape.smd = TRUE;

            res = Read_Word(ifp, token, MAX_LINE);
            if (strcmp(token, "EDIT_AREA_SIZE"))
            {
               fprintf (ferr, "Token EDIT_AREA_SIZE expected in mdf at %ld\n",Get_Linecnt());
               display_error++;
               return -1;
            }
            res = Get_Line(ifp,token, MAX_LINE);

            res = Read_Word(ifp,token, MAX_LINE);
            if (strcmp(token,"CMP_ATTRIBUTE"))
            {
               fprintf (ferr, "Token CMP_ATTRIBUTE expected in mdf at %ld\n",Get_Linecnt());
               display_error++;
               return -1;
            }
            res = Get_Line(ifp,token, MAX_LINE);
            // this is height
            lp = strtok(token,":");
            height = cnv_unit(atof(lp));   
            lp = strtok(NULL,":");  // scale
            lp = strtok(NULL,":");  // Polarity
            lp = strtok(NULL,":");  // component type number
            lp = strtok(NULL,":");  // component name
            clean_prosa(lp);

            if (USE_COMPSHAPE_NAME) 
            {
               cur_shape.name = lp;
            }

            if (solder_found)
            {
               tmp.Format("$M_%s",cur_shape.name); // this is mirrored comp
               cur_shape.name = tmp; 
            }

            BlockStruct *c;
            if (Graph_Block_Exists(doc, cur_shape.name, -1) != NULL)
            {
               CString  tmp, tmp1;
               if (USE_COMPSHAPE_NAME) 
               {
                  tmp.Format("Component ShapeName [%s] already exist!", cur_shape.name, cur_shape.number);
                  tmp1.Format("set .USE_COMPSHAPE_NAME n in [%s] and\n restart the import process !", sname);
               }
               else
               {
                  tmp.Format("Component ShapeNumber [%s] already exist!", cur_shape.name, cur_shape.number);
                  tmp1.Format("Defect MDF file! Sorry, but this can not be loaded!");
               }
               ErrorMessage(tmp1, tmp);
               return -2;
            }

            c = Graph_Block_On(GBO_APPEND, cur_shape.name, -1, 0L);
            c->setBlockType(BLOCKTYPE_PCBCOMPONENT);

            int pt = get_part(cur_shape.name);
            if (pt > -1)
            {
               switch (partarray[pt]->parttype)
               {
                  case PART_TOOL:
                     c->setBlockType(BLOCKTYPE_TOOLING);
                  break;
                  case PART_FIDUCIALTOP:
                  case PART_FIDUCIALBOT:
                  case PART_FIDUCIAL:
                     c->setBlockType(BLOCKTYPE_FIDUCIAL);
                  break;
                  default:
                  break;
               }
            }

            if (height > 0)   // do not write if zero.
               doc->SetAttrib(&c->getAttributesRef(),doc->IsKeyWord(ATT_COMPHEIGHT, 0), VT_UNIT_DOUBLE, &height, attributeUpdateOverwrite, NULL);

            CompNumber compNum;
            compNum.compNumber = cur_shape.number;
            compNum.blockNumber = c->getBlockNumber();
            compNum.done = FALSE;
            compNumArray.SetAtGrow(compNumCnt++, compNum);

            res = Read_Word(ifp,token, MAX_LINE);
            if (strcmp(token,"INSERTION"))
            {
               fprintf (ferr, "Token INSERTION expected in mdf at %ld\n",Get_Linecnt());
               display_error++;
               return -1;
            }
            res = Get_Line(ifp,token, MAX_LINE);   

            res = Read_Word(ifp,token, MAX_LINE);

            // dts0100571706 - Data presented with this DR does not have the SYM_POSITION record.
            // The importer was requiring this record, even though it wasn't actually used for anything.
            // The importer now detects this record and consumes it (but still ignores it) if present.
            // If the record is not present, the importer just moves on to see if it is maybe
            // the next expected record.

            if (strcmp(token,"SYM_POSITION") == 0)
            {
               // Record present, but not used by importer. Consume it.
               res = Get_Line(ifp,token, MAX_LINE);
               // Get net token
               res = Read_Word(ifp,token, MAX_LINE);
            }



            if (!strcmp(token,"}"))
            {
// empty record is allowed
/******************************************************************************************
$MDF {
   CMP_HEADR         3546 : SOLDER : 0 : NOTCR ;
   EDIT_AREA_SIZE       8.000 : 8.000 : 4.000 : 4.000 ;
   CMP_ATTRIBUTE        0.000 : : : 0 : "GUIDE-PIN [CMK}" : : 0.000 : 0.000 : 0 ;
   INSERTION         0 : 0.000 : 0.000 ;
   SYM_POSITION         0.000 , 0.000 : 0.000 , 0.000 : 0.000 , 0.000 : 0.000 , 0.000 ;
}
******************************************************************************************/
            }
            else
            {
               if (strcmp(token, "CMP"))
               {
                  fprintf (ferr, "Token CMP expected in mdf at %ld\n", Get_Linecnt());
                  display_error++;
                  return -1;
               }
               // here bracket open until closed
               res = Read_Word(ifp, token, MAX_LINE);   // this is must be {
               do // here start { info1 info2 }
               {
                  lcnt = Get_Linecnt();
                  res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
                  if (strcmp(token,"}"))
                  {
                     // term
                     if (!STRCMPI(token,"TERM"))
                     {
                        res = Get_Line(ifp, token, MAX_LINE);
                        do_terminal(token,solder_found);
                     }
                     else
                     if (!STRCMPI(token,"LAY"))
                     {
                        // do lay
                        res = Get_Line(ifp,token, MAX_LINE);
                        lcnt = Get_Linecnt();
                        do_lay(MDF_TYPE,token,solder_found);
                     }
                     else
                     {
                        fprintf(ferr, "Unknown keyword [%s] in mdf at %ld\n", token, Get_Linecnt());
                        display_error++;
                        return -1;
                     }
                  }
               } while (strcmp(token,"}"));
            } // empty CMP area
            Graph_Block_Off();

         } while (strcmp(token,"}"));

         // here check for non defined terminal
         BlockStruct *c = Graph_Block_On(GBO_APPEND, cur_shape.name, -1, 0L);
         check_terminals();
         Graph_Block_Off();
         free_terminal();

         if (solder_found && !USE_SOLDER_MDF)
         {
            // delete the SOLDER Defintion.
            int nr = c->getBlockNumber();
            doc->FreeBlock(c);
            //doc->getBlockAt(nr) = NULL;
         }
      }
      else
      {
         fprintf(ferr, "Keyword [$MSF] expected in mdf at %ld\n",Get_Linecnt());
         display_error++;
      }
   }

   fclose(ifp);
   return 1;
}

/****************************************************************************/
/*
*/
static int do_wdf(const char *fname)
{
   int      res;
   CString  cur_netname;
   int      ret = 1;

   /* initialize scanning parameters */
   Set_Comment( "/*" , "*/" ); // Comment is done in function array
   Set_Text_Delimeter( "\"" );
   Set_Word_Delimeters( " \t\n:," );
   Set_White_Characters( " \t\n" );
   Set_Non_Characters( "\r\b\v" );
   sprintf(cur_file,"%s.wdf",fname);

   if ((ifp = fopen(cur_file, "rt")) == NULL)
   {
      // wdf not needed.
      fprintf(ferr, "File [%s] not found\n",cur_file);
      display_error++;
      return 0;
   }

   if (progress != NULL)
      progress->SetStatus(cur_file);

   Reset_File(ifp);
   PushTok = 0;
   Set_Linecnt(1L);
   bracket_cnt = 0;

   // here do it.
   while ((res = Get_Word(ifp,token, MAX_LINE)) != EOF_CHARACTER)
   {
      if (!STRCMPI(token,"$WDF"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         while (TRUE) // here start { info1 info2 }
         {
#ifdef _DEBUG
      long lcnt = Get_Linecnt();
#endif
            res = Read_Word(ifp,token, MAX_LINE);

            // thats the end of the file.
            if (!strcmp(token,"}"))    break;

            // here cmp_headr expted
            if (strcmp(token,"NET"))
            {
               fprintf (ferr, "Token NET expected in wdf at %ld\n",Get_Linecnt());
               display_error++;
               return -1;
            }
            // here bracket open until closed
            res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
            do // here start { info1 info2 }
            {
               res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
               if (strcmp(token,"}"))
               {
                  if (!STRCMPI(token,"NET_HEADER"))
                  {
                     res = Get_Line(ifp,token, MAX_LINE);
                     do_netheader(token);
                  }
                  else
                  {
                     // do lin graphic
                     int res = do_lin(token, cur_net.name);
                     if (res == -1)
                     {
                        // unexpexted EOF found !
                        CString  t1;
                        t1.Format("Fatal Error: Unexpected end_of_file found in %s at %ld\n", cur_file,Get_Linecnt());
                        ErrorMessage(t1);
                        ret = -1;
                        break;
                     }
                  }
               }
            } while (strcmp(token,"}"));
         } // while TRUE
      }
      else
      {
         fprintf(ferr, "Keyword [$WDF] expected in wdf at %ld\n",Get_Linecnt());
         display_error++;
      }
   }

   fclose(ifp);
   return ret;
}

/****************************************************************************/
/*
*/
static int get_compptr(const char *c)
{
   int   i;

   for (i=0;i<compcnt;i++)
   {
      if (!comparray[i]->name.Compare(c))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int get_originalcompptr(const char *c)
{
   int   i;

   for (i=0;i<compcnt;i++)
   {
      if (!comparray[i]->originalname.Compare(c))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int do_ccf(const char *fname)
{
   int      res;
   char     *lp;
   CString  typname;
   char     compname[40];
   char     pinname[40];
   char     lastchar;

   /* initialize scanning parameters */
   Set_Comment( "/*" , "*/" ); // Comment is done in function array
   Set_Text_Delimeter( "\"" );
   Set_Word_Delimeters( ":,{}" );
   Set_White_Characters( "" );
   Set_Non_Characters( "\r\b\v\t\n " );   // no blanks, tabs and newlines
   sprintf(cur_file,"%s.ccf",fname);

   if ((ifp = fopen(cur_file, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",cur_file);
      ErrorMessage(tmp, "File Open Error", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   if (progress != NULL)
      progress->SetStatus(cur_file);

   Reset_File(ifp);
   PushTok = 0;
   Set_Linecnt(1L);
   bracket_cnt = 0;

   // get this here to avoid calling IsKeyWord too many times
   int pinNumKeyword = doc->IsKeyWord(ATT_COMPPINNR, 0);


   // here do it.
   while ((res = Get_Word(ifp,token, MAX_LINE)) != EOF_CHARACTER)
   {
      lastchar = Delimeter();
      if (!_strnicmp(token,"$CCF",4))   // can be $CCF{ without blank
      {
         while (TRUE) // here start { info1 info2 }
         {
            if ((res = Read_Word(ifp,token, MAX_LINE)) == EOF_CHARACTER)
               break;

            if ((lastchar = Delimeter()) == '}')   break;

            if (!_strnicmp(token,"DEFINITION",3))
            {
               res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
               while (TRUE) // here start { info1 info2 }
               {
                  if ((lastchar = Delimeter()) == '}')   break;
                  if ((lp = strtok(token,":")) == NULL)
                  {
                     typname = "NO_TYPE";
                     fprintf(ferr, "Typename missing in %s at %ld\n",cur_file, Get_Linecnt());
                  }
                  else
                     typname = lp;
                     
                  typname.TrimLeft();
                  typname.TrimRight();
                  
                  // here must take out the \n because Getline needs this
                  Set_Non_Characters( "\r\b\v\t " );   // no blanks, tabs and newlines
                  res = Get_Line(ifp,token, MAX_LINE);
                  Set_Non_Characters( "\r\b\v\t\n " );   // no blanks, tabs and newlines

                  lp = strtok(token,",;");
                  while (lp != NULL)
                  {
                     CString  mod_typname;
                     mod_typname = typname;  // make sure a type is not assigned to multiple types.

                     strcpy(compname, lp);
                     clean_blank(compname);

                     TypeStruct *t = AddType(file,mod_typname);

                     DataStruct *d = datastruct_from_refdes(doc, file->getBlock(), compname);
                     if (d)
                     {
                        if (t->getBlockNumber() > -1 && (t->getBlockNumber() != d->getInsert()->getBlockNumber()))
                        {
                           BlockStruct *b = doc->Find_Block_by_Num(d->getInsert()->getBlockNumber());
                           // problem - a device has different shapes
                           fprintf(ferr,"Device [%s] on Component [%s] already has Shape [%s] assigned !\n", 
                              t->getName(), compname, b->getName());
                           display_error++;
            
                           mod_typname += "_";
                           mod_typname += b->getName();
                           t = AddType(file, mod_typname);
                        }

                        t->setBlockNumber( d->getInsert()->getBlockNumber());

                        doc->SetUnknownAttrib(&d->getAttributesRef(),ATT_TYPELISTLINK, mod_typname, attributeUpdateOverwrite, NULL); //  
                     }
                     lp = strtok(NULL,",;");
                  }
                  res = Read_Token(ifp,token, MAX_LINE);
               }
            }
            else
            if (!_strnicmp(token,"PREASSIGN",3))
            {
               res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
               while (TRUE) // here start { info1 info2 }
               {
                  if ((lastchar = Delimeter()) == '}')   break;
                  res = Read_Token(ifp,token, MAX_LINE);
               }
            }
            else
            if (!_strnicmp(token,"NET",3) || !_strnicmp(token,"POWER",3) ||
                   !_strnicmp(token,"GROUND",3))
            {
               res = Read_Word(ifp,token, MAX_LINE);
               while (TRUE) // here start { info1 info2 }
               {
                  if ((lastchar = Delimeter()) == '}')   break;
                  lp = strtok(token,":");
                  strcpy(cur_net.name,lp);
                  clean_blank(cur_net.name);

                  NetStruct *n = add_net(file,cur_net.name);

                  while (get_nextpin(ifp, compname,pinname))
                  {
                     // check, I found that the CCF netlist may contain the original name and not
                     // the changed name
                     if (get_compptr(compname) < 0)
                     {
                        int cptr;
                        if ((cptr = get_originalcompptr(compname)) < 0)
                        {
                           fprintf(ferr,"Component [%s] not placed. Ignore netlist entry\n", compname);
                           display_error++;
                           compname[0] = '\0';
                        }
                        else
                        {
                           fprintf(ferr,"Component [%s] was renamed in UDF. Change component to [%s]\n", 
                              compname, comparray[cptr]->name);
                           display_error++;
                           strcpy(compname, comparray[cptr]->name);
                        }
                     }

                     if (USE_PIN_NAME)
                     {
                        DataStruct *data = datastruct_from_refdes(doc, file->getBlock(), compname);
                        if (data)
                        {
                           BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
                           if (block == NULL)
                              break;

                           POSITION pos = block->getDataList().GetHeadPosition();
                           while(pos)
                           {
                              DataStruct *data = block->getDataList().GetNext(pos);
                              if (data->getDataType() != T_INSERT)
                                 continue;
                              if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
                                 continue;
                              if (data->getAttributesRef() == NULL)
                                 continue;

                              Attrib* attrib = NULL;

                              if (!data->getAttributesRef()->Lookup(pinNumKeyword, attrib))
                                 continue;

                              CString pinNum;
                              pinNum.Format("%d", attrib->getIntValue());

                              if (pinNum.CompareNoCase(pinname))
                                 continue;
                              
                              strcpy(pinname, data->getInsert()->getRefname());
                              break;
                           }
                        }
                     }

                     if (strlen(compname))
                        CompPinStruct  *p = add_comppin(file, n, compname, pinname);
                  }
                  res = Read_Token(ifp,token, MAX_LINE);
               }
            }
            else
            {
              fprintf(ferr, "Unknown token [%s] in ccf at %ld\n", token, Get_Linecnt());
              display_error++;
              return -1;
            }
         }
      }
      else
      {
         fprintf(ferr, "Keyword [$CCF] expected in ccf at %ld\n",Get_Linecnt());
         display_error++;
      }
   }

   fclose(ifp);
   return 1;
}

/****************************************************************************/
/*
*/
static int do_udf(const char *fname)
{
   int   res;
   int   cmp;
   /* initialize scanning parameters */
   Set_Comment( "/*" , "*/" ); // Comment is done in function array
   Set_Text_Delimeter( "\"" );
   Set_Word_Delimeters( " \t\n:," );
   Set_White_Characters( " \t\n" );
   Set_Non_Characters( "\r\b\v" );
   sprintf(cur_file,"%s.udf",fname);

   if ((ifp = fopen(cur_file, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",cur_file);
      ErrorMessage(tmp, "File Open Error", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   if (progress != NULL)
      progress->SetStatus(cur_file);

   Reset_File(ifp);
   PushTok = 0;
   Set_Linecnt(1L);
   bracket_cnt = 0;

   strcpy(cur_net.name,"UDF_NET");

   // here do it.
   while ((res = Get_Word(ifp,token, MAX_LINE)) != EOF_CHARACTER)
   {
      if (!STRCMPI(token,"$UDF"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is must be {
         do // here start { info1 info2 }
         {

            cmp = FALSE;   // Zuken UDF Version 8 has a different format
                           // for component placement.
            if (!PushTok)
               res = Read_Word(ifp,token, MAX_LINE);
            // here cmp_headr expted
            if (strcmp(token,"OTHER") && strcmp(token,"CMP"))
            {
               fprintf (ferr, "Token OTHER or CMP expected in %s at %ld\n", cur_file, Get_Linecnt());
               display_error++;
               return -1;
            }
            cmp = !strcmp(token,"CMP");

            res = Read_Word(ifp,token, MAX_LINE);  // { expexted
            do // here start { info1 info2 }
            {
               res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
               if (strcmp(token,"}"))
               {
                  if (!STRCMPI(token,"REC_HEADR"))
                  {
                     res = Get_Line(ifp,token, MAX_LINE);
                     // do_term
                     do_recheadr(token);
                  }
                  else
                  if (!STRCMPI(token,"LAY"))
                  {
                     res = Get_Line(ifp,token,MAX_LINE);
                     if (cmp)
                     {
                        // lay in component section -> skip
                     }
                     else
                     {
                        // do lay
                        do_lay(UDF_TYPE,token,0);
                     }
                  }
                  else
                  if (!STRCMPI(token,"TERM") && cmp)
                  {
                    // skip until ;
                    res = Get_Line(ifp,token,MAX_LINE);
                  }
                  else
                  if (!STRCMPI(token,"SRF") /*&& cmp*/)  // case dts0100449862, ignore SRF in this position (is processed when subordinate to LAY)
                  {
                    // skip until ;
                    res = Get_Line(ifp,token,MAX_LINE);
                  }
                  else
                  if (!STRCMPI(token,"TOLE1") /*&& cmp*/)  // case dts0100449862, ignore TOLE1
                  {
                    // skip until ;
                    res = Get_Line(ifp,token,MAX_LINE);
                  }
                  else
                  if (!STRCMPI(token,"TOLE2") /*&& cmp*/)  // case dts0100449862, ignore TOLE2
                  {
                    // skip until ;
                    res = Get_Line(ifp,token,MAX_LINE);
                  }
                  else
                  if (!STRCMPI(token,"OBL") /*&& cmp*/)  // case dts0100571706, ignore OBL
                  {
                    // skip until ;
                    res = Get_Line(ifp,token,MAX_LINE);
                  }
                  else
                  if (atoi(token) && cmp) // these are the vertexes of LAY
                  {
                    // skip until ;
                    res = Get_Line(ifp,token,MAX_LINE);
                  }
                  else
                  {
                     fprintf(ferr, "Unknown keyword [%s] in %s at %ld, Processing of this file TERMINATED.\n", token, cur_file, Get_Linecnt());
                     display_error++;
                     return -1;
                  }
               }
            } while (strcmp(token,"}"));
            res = Read_Word(ifp,token, MAX_LINE);
            PushTok = TRUE;
         } while (strcmp(token,"}"));
      }
      else if (!strcmp(token, "}"))
      {
         // close of UDF open.
      }
      else
      {
         fprintf(ferr, "Keyword [$USF] expected in udf at %ld\n", Get_Linecnt());
         display_error++;
      }
   }

   fclose(ifp);
   return 1;
}

/******************************************************************************
* do_pma
*/
static int do_pma(const char *fname)
{
   /* initialize scanning parameters */
   Set_Comment( "/*" , "*/" ); // Comment is done in function array
   Set_Text_Delimeter( "\"" );
   Set_Word_Delimeters( " \t\n:," );
   Set_White_Characters( " \t\n" );
   Set_Non_Characters( "\r\b\v" );
   sprintf(cur_file, "%s.pma", fname);

   if ((ifp = fopen(cur_file, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",cur_file);
      ErrorMessage(tmp, "File Open Error", MB_OK | MB_ICONHAND);
      return 0;
   }
   
   if (progress != NULL)
      progress->SetStatus(cur_file);

   Reset_File(ifp);
   PushTok = 0;
   Set_Linecnt(1L);
   bracket_cnt = 0;

   int res;
   while ((res = Get_Word(ifp,token, MAX_LINE)) != EOF_CHARACTER)
   {
      if (!STRCMPI(token, "$PMA"))
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this must be {
         res = Read_Word(ifp, token, MAX_LINE);  // this must be NAME, if there is no NAME then file error
         if (strcmp(token, "NAME"))
         {
            fprintf (ferr, "Token NAME expected in pma at %ld\n", Get_Linecnt());
            display_error++;
            return -1;
         }
         res = Get_Line(ifp, token, MAX_LINE);  // ignore the rest of the line

         int compNumber[5] = {0, 0, 0, 0, 0};   // it is declare as 5 per specification
         BOOL PCMAC_NO = FALSE;

         while (TRUE) // staring reading until the matching {
         {
            res = Read_Word(ifp, token, MAX_LINE);
            if (!strcmp(token, "}"))   // stop when it finds the matching { of $PMA
               break;

            if (!strcmp(token, "PCMAC_NO") )
            {
               PCMAC_NO = TRUE;
               res = Get_Line(ifp, token, MAX_LINE);  // ignore the rest of the line

               CString tmp = strtok(token, " \t:");
               tmp.Trim();
               if (tmp != "")
                  compNumber[0] = atoi(tmp);

               tmp = strtok(NULL, " \t:");
               tmp.Trim();
               if (tmp != "")
                  compNumber[1] = atoi(tmp);

               tmp = strtok(NULL, " \t:");
               tmp.Trim();
               if (tmp != "")
                  compNumber[2] = atoi(tmp);

               tmp = strtok(NULL, " \t:");
               tmp.Trim();
               if (tmp != "")
                  compNumber[3] = atoi(tmp);

               tmp = strtok(NULL, " \t:;");
               tmp.Trim();
               if (tmp != "")
                  compNumber[4] = atoi(tmp);
            }
            else if (!strcmp(token, "CURRENT"))
            {
               res = Get_Line(ifp, token, MAX_LINE);  // ignore the rest of the line                  
            }
            else if (!strcmp(token, "SYMBOL") || !strcmp(token, "IO_CURRENT") || !strcmp(token, "USER_DEFINITION"))
            {  
               // Read and skip the whole section
               while(TRUE) 
               {
                  res = Read_Word(ifp, token, MAX_LINE); // this is must be {
                  if (!strcmp(token, "}"))   // stop when it finds matching }
                     break;
                  res = Get_Line(ifp, token, MAX_LINE);  // ignore the rest of the line                  
               }
            }
            else if (!strcmp(token, "PIN"))
            {
               // Read PIN section and get the pin number and pin name
               PinNameArray pinArray;
               int pinCnt = 0;
               pinArray.SetSize(100, 100);
               
               res = Read_Word(ifp, token, MAX_LINE); // this is must be {
               while(TRUE) 
               {
                  res = Read_Word(ifp, token, MAX_LINE); // this is must be {
                  if (!strcmp(token, "}"))   // stop when it finds matching }
                     break;

                  CString pinNumber = strtok(token, " \t:");         // pin number
                  res = Get_Line(ifp, token, MAX_LINE);  

                  if (!PCMAC_NO)
                     continue;

                  CString pinName = strtok(token, " \t:");        // pin name
                  CString gateNumber = strtok(NULL, " \t:");      // gate number
                  CString terminalName = strtok(NULL, " \t;:");   // terminal name

                  PinName pin;
                  pin.pinNumber = pinNumber.Trim();
                  pin.pinName = pinName.Trim();
                  pinArray.SetAtGrow(pinCnt++, pin);
               }

               if (PCMAC_NO)
               {
                  for (int i=0; i<5; i++)
                     RenamePinRefdes(compNumber[i], &pinArray, pinCnt);
               }
               
               pinArray.RemoveAll();
            }
         }
      }
      else
      {
         fprintf(ferr, "Keyword [$PMA] expected in pma at %ld\n", Get_Linecnt());
         display_error++;
      }
   }

   fclose(ifp);
   return 1;
}

/******************************************************************************
* RenamePinRefdes
*/
static void RenamePinRefdes(int compNumber, PinNameArray *pinArray, int pinCnt)
{
   if (compNumber < 1)
      return;

   int blockNum = -1;
   for (int i=0; i<compNumCnt; i++)
   {
      CompNumber comp = compNumArray.GetAt(i);
      if (comp.compNumber == compNumber)
      {
         if (comp.done)
            return;

         comp.done = TRUE;
         compNumArray.SetAt(i, comp);
         blockNum = comp.blockNumber;
         break;
      }
   }

   if (blockNum < 0)
      return;

   BlockStruct *block = doc->getBlockAt(blockNum);
   if (block == NULL)
      return;

   POSITION pos = block->getDataList().GetHeadPosition();
   while(pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;
      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

      for (int i=0; i<pinCnt; i++)
      {
         PinName pin = (*pinArray).GetAt(i);
         if (!pin.pinNumber.CompareNoCase(data->getInsert()->getRefname()))
         {
            data->getInsert()->setRefname(STRDUP(pin.pinName));
            doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_COMPPINNR, pin.pinNumber, attributeUpdateOverwrite, NULL);
            break;
         }
      }
   }

}

/****************************************************************************/
/*
*/
static int Get_Word(FILE *fp,char *tmp,int n)
{
   int   res;
   
   if (PushTok)
   {
      PushTok = FALSE;
      return(NORMAL_TEXT);
   }

   while (TRUE)
   {
      res = Read_Word(fp,tmp,n);
      if (!strcmp(tmp,"{"))   bracket_cnt++;
      else
      if (!strcmp(tmp,"}"))   bracket_cnt--;
      else
         return res;
   }
/* printf("Get > %s at %ld\n",tmp,Get_Linecnt()); */
   return(res);
}

/****************************************************************************/
/*
   Read a line until ;
   Make :: into : :
*/
static int Get_Line(FILE *fp,char *tmp,int n)
{
   int      res;
   CHAR     c, lastc = '\0';
   int      cnt = 0;
   int      texton = 0;

   do
   {
      res = Read_Char(fp,&c);

	  //normalize some common error creating extended characters
	  if (c == char(-69))
		  c = 'A'; //this would make char(59), ";", after 'c = c & 127;'
      if (c == char(-121))
		  c = 'A'; //this would make char(7), EOF_CHARACTER, after 'c = c & 127;'
      if (c == char(-122))
		  c = 'A'; //this would make char(6), EOL_CHARACTER, after 'c = c & 127;'

	  c = c & 127;
      
      if (c == '\n')   res = EOL_CHARACTER;
      if (c == '\r')   res = EOL_CHARACTER;

      if (c == '\n') c = ' ';

      // Here spread :: to : :
      if (cnt > 0 && c == ':' && tmp[cnt-1] == ':')
      {
         tmp[cnt++] = ' ';
      }

      tmp[cnt++] = c;
      if (!isspace(c))
         lastc = c;

      if (lastc == ';' || (res == EOL_CHARACTER && lastc == ';'))
         break;
   } while (res != EOF_CHARACTER && cnt < n);

   tmp[cnt-1] = '\0';

   if (lastc != ';')
   {
#ifdef _DEBUG
      CString  t1;
      t1.Format("No end_of_command found [%s] in %s at %ld\n", tmp, cur_file,Get_Linecnt());
      ErrorMessage(t1);
#endif
      fprintf(ferr, "No end_of_command found [%s] in %s at %ld\n", tmp, cur_file,Get_Linecnt());
      display_error++;
   }
   return(res);
}

/****************************************************************************/
/*
*/
double get_unit(const char *t, int scale)
{
   double   f = 1;
   int      i;

   if (scale == 0)
   {
      f = 1;
   }
   else
   if (scale < 0)
   {
      for (i=0;i>scale;i--)
      {
         f = f / 10;
      }
   }
   else
   if (scale > 0)
   {
      for (i=0;i<scale;i++)
      {
         f = f * 10;
      }
   }

   if (!STRCMPI(t,"MILS"))       
      f *= Units_Factor(UNIT_MILS, PageUnits);
   else
   if (!STRCMPI(t,"INCH"))       
      f *= Units_Factor(UNIT_INCHES, PageUnits);
   else
   if (!STRCMPI(t,"MILLIMETER")) 
      f *= Units_Factor(UNIT_MM, PageUnits);
   else
   {
      fprintf(ferr, "Unknown Unit [%s] -> set to  PAGEUNITS\n",t);
      display_error++;
   }
   return f;   // just to keep the compiler happy
}

/****************************************************************************/
/*
*/
void globmem_init()
{
   int   i;

   for (i=0;i<MAX_LAYERS;i++)
      mirrorlayers[i] = i;

   comparray.SetSize(100,100);
   compcnt = 0;

   partarray.SetSize(100, 100);
   partcnt = 0;

   drillarray.SetSize(100,100);
   drillcnt = 0;

   compNumArray.SetSize(100, 100);
   compNumCnt = 0;

   attribmaparray.SetSize(100,100);
   attribmapcnt = 0;

   if ((poly_l = (Poly_l *)calloc(MAX_POLY, sizeof(Poly_l))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   polycnt = 0;

   if ((land = (Land *)calloc(MAX_LAND, sizeof(Land))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   landcnt = 0;

   if ((smdpin = (Smdpin *)calloc(MAX_SMDPIN, sizeof(Smdpin))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   smdpincnt = 0;

   if ((texttable = (Texttable *)calloc(MAX_TEXTTABLE, sizeof(Texttable))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   texttablecnt = 0;

   if ((viatable = (Viatable *)calloc(MAX_VIATABLE, sizeof(Viatable))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   viatablecnt = 0;

   if ((turret = (Turret *)calloc(MAX_TURRET, sizeof(Turret))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   turretcnt = 0;

   artlayspeccnt = 0;
   wirelayspeccnt = 0;

}

void globmem_free()
{
   int   i;
/*
   for (i=0;i<artlayspeccnt;i++)
   {
      if (artlayspec[i].number)
      {
         fprintf(ferr," Zuken Layer [%d] -> [%s]\n", artlayspec[i].number,artlayspec[i].comment);
      }
   }
*/

   for (i=0;i<attribmapcnt;i++)
   {
      delete attribmaparray[i];
   }
   attribmaparray.RemoveAll();
   attribmapcnt = 0;

   for (i=0; i<partcnt; i++)
      delete partarray[i];  
   partarray.RemoveAll();
   partcnt = 0;

   artlayspeccnt = 0;
   wirelayspeccnt = 0;

   free(poly_l);

   free(texttable);
   texttablecnt = 0;

   free(land);
   landcnt = 0;

   for (i=0;i<smdpincnt;i++)
   {
      if (smdpin[i].cnt)   free(smdpin[i].p);
   }
   free(smdpin);
   smdpincnt = 0;

   free(viatable);
   viatablecnt = 0;

   free(turret);
   turretcnt = 0;

   for (i=0;i<compcnt;i++)
   {
      delete comparray[i];
   }
   comparray.RemoveAll();

   drillarray.RemoveAll();

   compNumArray.RemoveAll();

   return;
}

/****************************************************************************/
/*
*/
static int do_artlayspec(int laynr, char *t)
{
   char  tmp[MAX_LINE];
   char  *lp;
   strcpy(tmp,t);

   if (artlayspeccnt < MAX_LAYERS)
   {
      lp = strtok(tmp,":");
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      if ((lp = strtok(NULL,";")) != NULL)
      {
         if (lp[0] == '"') lp++;
         while (strlen(lp) && (lp[strlen(lp)-1] == '"' || isspace(lp[strlen(lp)-1])))
            lp[strlen(lp)-1] = '\0';

         artlayspec[artlayspeccnt].comment = lp;
         artlayspec[artlayspeccnt].comment.TrimLeft();
         artlayspec[artlayspeccnt].comment.TrimRight();
         artlayspec[artlayspeccnt].number = laynr;
      }
      artlayspeccnt++;
   }
   else
   {
#ifdef _DEBUG
      CString  t1;
      t1.Format("Too many Layers");
      ErrorMessage(t1);
#endif

      fprintf(ferr, "Too many layers\n");
      display_error++;
      return -1;
   }
   return artlayspeccnt - 1;
}

/****************************************************************************/
/*
*/
static int do_complexvia(const char *layerarray, double hole, double x, double y)
{
   CString  padname;
   char     *lp;
   char     tmp[255];
   int      pshape[255];
   int      pshapecnt = 0;

   strcpy(tmp, layerarray);

   padname.Format("VIA_%lf", hole);

   lp = strtok(tmp," ,");

   while (lp)
   {
      padname += "_";
      padname += lp;
      pshape[pshapecnt++] = atoi(lp);
      lp = strtok(NULL," ,");
   }

   if (Graph_Block_Exists(doc, padname, -1) == NULL)
   {
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,padname,-1,0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      for (int ii=0;ii<pshapecnt;ii++)
      {
         if (pshape[ii])
         {
            CString  padshapename;
            int      layernum;
            char     layername[80];

            if ((ii+1) == 1)
            {
               strcpy(layername,"PADTOP");
            }
            else
            if ((ii+1) == number_of_layers)
            {
               strcpy(layername,"PADBOT");
            }
            else
            {
               sprintf(layername,"PAD%d", ii+1);
            }

            layernum = Graph_Level(layername,"",0);
            padshapename.Format("TURRET_%d", pshape[ii]);
            Graph_Block_Reference(padshapename, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
         }
      }

      if (hole > 0)
      {
         // here make a drill aperture.
         int drillayer = Graph_Level("DRILLHOLE","",0);
         int drillindex = get_drillindex(cnv_unit(hole), drillayer);
         CString  drillname;
         drillname.Format("DRILL_%d",drillindex);
         // must be filenum 0, because apertures are global.
         Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                               0, 1.0, drillayer, TRUE);
      }
      Graph_Block_Off();
   }

   DataStruct *d = Graph_Block_Reference(padname, NULL, -1, cnv_unit(x),cnv_unit(y),
                        DegToRad(0), 0 , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypeVia);

   return 1;
}

/****************************************************************************/
/*
*/
static int do_lay(int type,char *t,int mirror)
{
   CString  keyw;
   char     tmp[MAX_LINE];
   char     pinname[40], padname[40];
   char     layername[40], prosa[101];
   char     *lp;
   int      cnt,vertexcnt,res,i,mir;
   int      layernr = -1, layerindex = -1;
   int      appnr, tableptr, turretptr;
   double   h,w,x,y,rot;
   int      linecnt = Get_Linecnt();

   strcpy(tmp,t);

   lp = strtok(tmp,":");
   keyw = lp;
   keyw.TrimLeft();
   keyw.TrimRight();

   if (keyw.Left(1) == "(")
   {
      // here is layer
      lp = keyw.GetBuffer(0);
      layerindex = layernr = atoi(lp+1);
      lp = strtok(NULL,":");
      keyw = lp;
      keyw.TrimLeft();
      keyw.TrimRight();
   }

   if (layernr > 0)
   {
      if (mirror)
         layernr = mirrorlayers[layernr];
      switch (type)
      {
         case MDF_TYPE:
            sprintf(layername,"%d",layernr);
            layernr = Graph_Level(layername, "", 0);
         break;
         case UDF_TYPE:
            sprintf(layername,"%d",layernr);
            layernr = Graph_Level(layername, "", 0);
         break;
         default:
            fprintf(ferr, "Unknown type do_lay\n");
            return -1;
         break;
      }
   }

   if (!STRCMPI(keyw,"VIA"))
   {
      // laynr is terminal number
      lp = strtok(NULL,":");     // terminal number
      strcpy(pinname,lp);
      clean_blank(pinname);

      lp = strtok(NULL,":");     // coordinates in 0.0 , 0.1
      get_koo(lp,&x,&y);

      lp = strtok(NULL,":");     // from layer number
      lp = strtok(NULL,":");     // to layer number
      lp = strtok(NULL,":");     // via table number
      appnr = atoi(lp);

      lp = strtok(NULL,":");     // hole type number
      lp = strtok(NULL,":");     // hole type NORM ..
      lp = strtok(NULL,":");     // number of wiring layers during generation
      int number_of_layers = atoi(lp);
      lp = strtok(NULL,":");     // hole diameter
      double holediam = atof(lp);
      lp = strtok(NULL,":");     // land status or app number
      
      CString  landstatus;
      landstatus = lp;     // dependens on appnr.

      if ((lp = strtok(NULL,":")) != NULL) // graphic attributes
      {
         char  *tmp; 
         if (lp[strlen(lp)-1] == ';')
            lp[strlen(lp)-1] = '\0';
         keyw = lp;
         tmp = lp;

         // this is for number of layers. 
         while ((lp = strtok(tmp,",;")) != NULL)
         {
            CString  key, saveline; // need to save because other strtok make it bad.
            key = lp;
            key.TrimLeft();
            key.TrimRight();
            lp = strtok(NULL,"\0"); // get rest of line
            saveline = lp;
            // this is a blank entry, which is OK
            if (strlen(key) == 0)
            {
               // this means no graphic on this layer
            }
            else
            if (!strcmp(key,"SRF"))
            {
               int pinnr;
               // This is a filled area as part of a padstack
               res = Get_Line(ifp,token, MAX_LINE);
               lp = strtok(token,":");
               pinnr = atoi(lp);
               lp = strtok(NULL,":");
               lp = strtok(NULL,":");
               lp = strtok(NULL,":");
               lp = strtok(NULL,":");
               lp = strtok(NULL,":");
               vertexcnt = atoi(lp);
               for (cnt=0;cnt<vertexcnt;cnt++)
               {
                  res = Get_Line(ifp,token, MAX_LINE);
               }
            }
            else
            if (!strcmp(key,"OBL"))
            {
               res = Get_Line(ifp,token, MAX_LINE);
            }
            else
            {
               fprintf(ferr,"Something [%s] here at %ld in %s\n",
                  key, Get_Linecnt(),(type == UDF_TYPE)?"UDF File":"MDF File");
            }
            strcpy(tmp, saveline);
         } // end of while
      }

      if (mirror) x = -x;
      if (type == MDF_TYPE) // only in shape, not FREE vias
      {
         sprintf(padname,"PAD_%d",appnr);
         set_viatable(appnr,1);
         write_shapepin(type, cur_shape.name, pinname, padname, x, y, -1);
      }
      else
      {
         //sprintf(padname,"PAD_%d",appnr);
         //set_viatable(appnr,2);
         if (appnr == 0)
         {
            // if appnr == 0, these are the land for each wiring layer
            do_complexvia(landstatus, holediam, x, y);
         }
         else
         {
            // if appnr != 0, this is the land type.
            CString  padname;
            padname.Format("PAD_%d",appnr);
            set_viatable(appnr,2);
            DataStruct *d = Graph_Block_Reference(padname, NULL, -1, cnv_unit(x),cnv_unit(y),
                              DegToRad(0), 0 , 1.0, -1, TRUE);
            d->getInsert()->setInsertType(insertTypeVia);
         }
      }
   }
   else
   if (!STRCMPI(keyw,"LIN"))
   {
      lp = strtok(NULL,":");  // terminal number
      strcpy(pinname,lp);
      clean_blank(pinname);
      lp = strtok(NULL,":");  // line type
      lp = strtok(NULL,":");  // dash1
      lp = strtok(NULL,":");  // dash space
      lp = strtok(NULL,":");  // dash2
      lp = strtok(NULL,":");
      vertexcnt = atoi(lp);

      //d->setGraphicClass(graphic_class);
      appnr = get_vertexpoints(vertexcnt,VER_LINE,&turretptr, mirror);

      int tptr = -1;

      if (strlen(pinname))
      {
         tptr = get_terminal(pinname);
      }

      if (tptr > -1)
      {
         sprintf(padname,"SMD_%d",get_smdpin(tptr,layerindex,appnr,turretptr));
         write_shapepin(type, cur_shape.name, pinname, padname,
               terminalarray[tptr]->x, terminalarray[tptr]->y, -1);
         polycnt = vertexcnt = 0;
      }
      else
      {
         int err;
         CString  cur_turretname;
         cur_turretname.Format("TURRET_%d",appnr);
         int widthindex = Graph_Aperture(cur_turretname, 0, 0.0, 0.0,0.0, 0.0, 0.0, 0,
                     BL_APERTURE, FALSE, &err); // ensure that aperature is in database

         DataStruct *d = Graph_PolyStruct(layernr,0L,0);

         if (type == MDF_TYPE)
         {
            d->setGraphicClass(get_layerclass(layername));
         }

         write_vertexpoints(layername,widthindex,VER_LINE,(type == UDF_TYPE)?"UDF File":"MDF File", FALSE, FALSE);
      }
   }
   else
   if (!STRCMPI(keyw,"LND")) // land is an SMD with an aperature on only one layer
   {
      lp = strtok(NULL,":");  // terminal number
      strcpy(pinname,lp);
      clean_blank(pinname);

      lp = strtok(NULL,":");  // type number
      lp = strtok(NULL,":");  // appnumber
      i = atoi(lp);
      
      lp = strtok(NULL,":");  // coo
      get_koo(lp,&x,&y);
      if (mirror) x = -x;

      sprintf(padname,"LND_%d_%d",i,layerindex);
      update_land(layerindex,i);

      if (strlen(pinname) == 0)
      {
#ifdef _DEBUG
long linecnt = Get_Linecnt();
#endif
         write_shapepin(type,cur_shape.name,pinname,padname,x,y, Graph_Level(layername,"",0));
      }
      else
      {
         int  tptr = get_terminal(pinname);
         if (tptr > -1)
            write_shapepin(type, cur_shape.name, pinname, padname,
               terminalarray[tptr]->x, terminalarray[tptr]->y, -1);
      }
   }
   else
   if (!STRCMPI(keyw,"COC"))
   {
      // area for checking if component is duplicated ??? skip it.
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      vertexcnt = atoi(lp);
      lp = strtok(NULL,":");
      for (cnt=0;cnt<vertexcnt;cnt++)
      {
         res = Get_Line(ifp,token, MAX_LINE);
      }
   }
   else
   if (!STRCMPI(keyw,"HAR"))  // height limit restriction
   {
      // area for checking if component is duplicated ??? skip it.
      lp = strtok(NULL,":");  // height
      lp = strtok(NULL,":");
      vertexcnt = atoi(lp);
      for (cnt=0;cnt<vertexcnt;cnt++)
      {
         res = Get_Line(ifp,token, MAX_LINE);
      }
   }
   else
   if (!STRCMPI(keyw,"ARC"))
   {
      // this is ARC AREAS needed to checking components against
      // mounting rules. ??? Skip it.
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      lp = strtok(NULL,":");
      vertexcnt = atoi(lp);
      lp = strtok(NULL,":");

      for (cnt=0;cnt<vertexcnt;cnt++)
      {
         res = Get_Line(ifp,token, MAX_LINE);
      }

      //appnr = get_vertexpoints(vertexcnt,VER_ARC);
      //write_vertexpoints(layername,appnr,VER_ARC);
      polycnt = 0;
   }
   else
   if (!STRCMPI(keyw,"HAT"))
   {
      lp = strtok(NULL,":");  // linetype
      lp = strtok(NULL,":");  // dash line length 1
      lp = strtok(NULL,":");  // dash line length 2
      lp = strtok(NULL,":");  // dash line spacing
      lp = strtok(NULL,":");  // angle 1
      lp = strtok(NULL,":");  // angle 2
      lp = strtok(NULL,":");  // pitch 1
      lp = strtok(NULL,":");  // pitch 2
      lp = strtok(NULL,":");  //
      lp = strtok(NULL,":");  //
      lp = strtok(NULL,":");  //
      vertexcnt = atoi(lp);
      lp = strtok(NULL,":");

      for (cnt=0;cnt<vertexcnt;cnt++)
      {
         res = Get_Line(ifp,token, MAX_LINE);
      }

      //appnr = get_vertexpoints(vertexcnt,VER_ARC);
      //write_vertexpoints(layername,appnr,VER_ARC);
      polycnt = 0;
   }
   else
   if (!STRCMPI(keyw,"SRF"))
   {
      char  att[80];
      int   nega = FALSE;
      int   cont = FALSE;
      char  pinname[80];

      //printf("SRF not implemented in %s at %ld\n",cur_file, Get_Linecnt());
      lp = strtok(NULL,":");          // terminal number
      strcpy(pinname,lp);
      clean_blank(pinname);
      lp = strtok(NULL,":");          // Attribute POSI, CONTOUR
      strcpy(att,lp);
      if (strstr(att,"NEGA"))    nega = TRUE;
      if (strstr(att,"CONTOUR")) cont = TRUE;
      lp = strtok(NULL,":");          // App number
      appnr = atoi(lp);
      lp = strtok(NULL,":");          // Paint angle
      lp = strtok(NULL,":");          // number of points
      vertexcnt = atoi(lp);

      //d->setGraphicClass(graphic_class);
      get_vertexpoints(vertexcnt,VER_SRF,&turretptr, mirror);

      int tptr = -1;

      if (strlen(pinname))
      {
         tptr = get_terminal(pinname);
      }

      if (tptr > -1)
      {
         sprintf(padname,"SMD_%d",get_smdpin(tptr,layerindex,appnr,turretptr));
         write_shapepin(type, cur_shape.name, pinname, padname,
               terminalarray[tptr]->x, terminalarray[tptr]->y, -1);
         polycnt = vertexcnt = 0;
      }
      else
      {
         int err;
         CString  cur_turretname;
         cur_turretname.Format("TURRET_%d", appnr);
         int widthindex = Graph_Aperture(cur_turretname, 0, 0.0, 0.0,0.0, 0.0, 0.0, 0,
                     BL_APERTURE, FALSE, &err); // ensure that aperature is in database

         DataStruct *d = Graph_PolyStruct(layernr,0L,0);
         int ltyp = VER_SRF;
         int fill = TRUE;
         if (nega)   fill = FALSE;
         if (layerindex == contour_lay_no && type == UDF_TYPE)
         {
            d->setGraphicClass(GR_CLASS_BOARDOUTLINE);
            ltyp = VER_LINE;  // board outline is a filled surface in CR3000, but should be a open closed 
            fill = FALSE;     // board contour is always not filled.
         }
         else
         if (type == MDF_TYPE)
         {
            d->setGraphicClass(get_layerclass(layername));
         }
         write_vertexpoints(layername,widthindex, ltyp,(type == UDF_TYPE)?"UDF File":"MDF File", TRUE, fill);
      }
      polycnt = 0;
   }
   else
   if (!STRCMPI(keyw,"SYM"))
   {
      // ???
   }
   else
   if (!STRCMPI(keyw,"PNT"))
   {
      // ???
   }
   else
   if (!STRCMPI(keyw,"TXT"))
   {
      double theight = 0.07;  // default

      lp = strtok(NULL,":");  // font number
      lp = strtok(NULL,":");  // text table number
      if ((tableptr = get_textptr(atoi(lp))) >= 0)
         theight = cnv_unit(texttable[tableptr].height);
      lp = strtok(NULL,":");  // app number
      lp = strtok(NULL,":");  // coo
      get_koo(lp,&x,&y);
      lp = strtok(NULL,":");  // angle
      rot = atof(lp);
      lp = strtok(NULL,":");  // mirror
      clean_blank(lp);
      mir = !STRCMPI(lp,"MIRROR");
      mir = mir ^ mirror;  // from solder to unmirror and mirror flag.

      if ((lp = strtok(NULL,":")) != NULL) // height
      {
         // this overwrites the code pointer
         if (atof(lp) > 0)
            theight = cnv_unit(atof(lp));
      }
      lp = strtok(NULL,":");  // width
      lp = strtok(NULL,":");  // spacing
      lp = strtok(NULL,":");  //prosa
      CpyStr(prosa,lp,100);   // check for length
      clean_prosa(prosa);
      
      if (mirror) x = -x;

      if (strlen(prosa))
      {
         Graph_Text(layernr,prosa,cnv_unit(x),cnv_unit(y),theight,theight*6.0/8.0,DegToRad(rot),
               0, TRUE, mir, 0, FALSE, -1, 0); // prop flag, mirror flag, oblique angle in deg
      }
   }
   else
   if (!STRCMPI(keyw,"SHOL"))  // square hole
   {
      lp = strtok(NULL,":");  // angle
      rot = atof(lp);
      lp = strtok(NULL,":");  // height
      h   = atof(lp);
      lp = strtok(NULL,":");  // width
      w   = atof(lp);
      lp = strtok(NULL,":");  // corner radius
      lp = strtok(NULL,":");  // coo
      get_koo(lp,&x,&y);
      lp = strtok(NULL,":");  // name

      if (mirror) x = -x;
      // write vertex
      write_squarehole(layername,x,y,h,w,rot);
   }
   else
   if (!STRCMPI(keyw,"HOL"))
   {
      lp = strtok(NULL,":");  // 1
      lp = strtok(NULL,":");  // NORM
      lp = strtok(NULL,":");  // width/radius
      w   = atof(lp);
      lp = strtok(NULL,":");  // coo
      get_koo(lp,&x,&y);
      if (mirror) x = -x;
      DataStruct *d = Graph_Circle(layernr, cnv_unit(x), cnv_unit(y), cnv_unit(w/2), 0L, 0 , FALSE,FALSE); 
   }
   else
   if (!STRCMPI(keyw,"LDR"))
   {

   }
   else
   if (!STRCMPI(keyw,"LDIM"))
   {

   }
   else
   if (!STRCMPI(keyw,"RDIM"))
   {

   }
   else
   if (!STRCMPI(keyw,"ADIM"))
   {

   }
   else
   {
      fprintf(ferr, "Unknown LAY token [%s] found in %s at %ld\n", lp, cur_file, Get_Linecnt());
      display_error++;
      return -1;
   }
   return 1;
}

/****************************************************************************/
/*
  if l = XXX ( YYY )  then YYY is the "changed" component name
  else
         XXX is the component name.
*/
static int do_component(char *c,char *l, char *o)
{
   char  tmp[40];
   unsigned int   i, cc;
   
   c[0] = '\0';
   o[0] = '\0';
   strcpy(tmp,l);
   if (strchr(tmp,'('))
   {
      // can not use strtok inside here.
      cc = i = 0;
      while (i < strlen(tmp) && tmp[i] != '(')  
      {
         o[cc++] = tmp[i++];
         o[cc] = '\0';
      }
      i++; // advance bracket.
      cc = 0;
      while (i < strlen(tmp) && tmp[i] != ')')
      {
         c[cc++] = tmp[i++];
         c[cc] = '\0';
      }
   }
   else
   {
      strcpy(c, l);
      strcpy(o, l);
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int do_recheadr(char *t)
{
   char     tmp[MAX_LINE];
   char     *lp;
   char     cname[40];
   char     originalcname[40];
   CString  compname, originalcompname, shapename;
   int      mir = 0;
   double   rot,x,y;

   strcpy(tmp,t);

   lp = strtok(tmp,":");
   do_component(cname,lp, originalcname);

   compname = cname;
   compname.TrimLeft();
   compname.TrimRight();

   originalcompname = originalcname;
   originalcompname.TrimLeft();
   originalcompname.TrimRight();

   lp = strtok(NULL,":");
   shapename = lp;
   shapename.TrimLeft();
   shapename.TrimRight();

   lp = strtok(NULL,":");  // placement angle
   rot = atof(lp);
   lp = strtok(NULL,":");  // scale
   lp = strtok(NULL,":");  // coo
   get_koo(lp,&x,&y);

   lp = strtok(NULL,":"); // attributes
   if (strstr(lp,"SOLDER"))  mir = 1;
   CString  compattr = lp;

   lp = strtok(NULL,":");  // lock Attributes
   lp = strtok(NULL,":");  // component level
   CString  complevel = lp;
   complevel.TrimLeft();
   complevel.TrimRight();

   lp = strtok(NULL,":");  // layers excluded
   lp = strtok(NULL,":");  // desc (type ????)
   clean_prosa(lp);
   CString  desc;
   desc = lp;
   desc.TrimLeft();
   desc.TrimRight();

   if (USE_COMPSHAPE_NAME)
   {
      if (strlen(desc))
      {
         shapename = desc;
         desc = "";
      }
      else
      {
         CString  tmp;
         tmp.Format("No Component Name for Component Number [%s] found.\n", shapename);
         ErrorMessage(tmp,".USED_COMPSHAPE_NAME Error");
      }
   }

   if (mir && USE_SOLDER_MDF) 
   {
      // mirrored components are defined in the "SOLDER section" of the mdf file.
      CString tmp;
      tmp.Format("$M_%s",shapename); // this is mirrored comp

      // if no solder definition done, use the COMP definition
      if (Graph_Block_Exists(doc, tmp, -1) == NULL)
      {
         // shapename stays
         mir = 3;
      }
      else
      {
         shapename = tmp;
      }
   }

   if (Graph_Block_Exists(doc, shapename, -1) == NULL)
   {
      if (mir && !USE_SOLDER_MDF)   
      {
         fprintf(ferr,"Mirrored Shape [%s] not defined as a TOP definition in MDF file!\n", shapename);
         fprintf(ferr,"Set .USE_SOLDER_MDF Y in cr3000.in and rerun!\n");
         display_error++;
      }
      else
      {
         // this is a really big problem
         fprintf(ferr,"Error: Shape [%s] not defined in MDF file!\n", shapename);
         display_error++;
      }
   }

   while (rot < 0)   rot += 360;
   while (rot >= 360)   rot -= 360;

   DataStruct *d = Graph_Block_Reference(shapename, compname, 
         0, cnv_unit(x), cnv_unit(y), DegToRad(rot), mir , 1.0, -1, TRUE);
   d->getInsert()->setInsertType(insertTypePcbComponent);

   BlockStruct *b = Graph_Block_Exists(doc, shapename, -1);
   b = Graph_Block_On(GBO_APPEND, shapename, -1, 0);
   
   int pt = get_part(shapename);
   if (pt > -1)
   {
      switch (partarray[pt]->parttype)
      {
         case PART_TOOL:
         {
            b->setBlockType(BLOCKTYPE_TOOLING);
            d->getInsert()->setInsertType(INSERTTYPE_TOOLING); 
         }
         break;
         case PART_FIDUCIALBOT:
            if (d->getInsert()->getGraphicMirrored() == 0)
            {
               d->getInsert()->setPlacedBottom(true);
            }
         case PART_FIDUCIALTOP:
         case PART_FIDUCIAL:
         {   
            b->setBlockType(BLOCKTYPE_FIDUCIAL);
            d->getInsert()->setInsertType(INSERTTYPE_FIDUCIAL);
         }
         break;
         default:
         break;
      }
   }

   Graph_Block_Off();

   doc->SetUnknownAttrib(&d->getAttributesRef(),ATT_REFNAME, compname, attributeUpdateOverwrite, NULL); // x, y, rot, height

   if (mir && USE_SOLDER_MDF) 
   {
      d->getInsert()->setMirrorFlags(MIRROR_FLIP);
      d->getInsert()->setPlacedBottom(true);
   }
   if (strlen(desc))
   {
      doc->SetUnknownAttrib(&d->getAttributesRef(),get_attribmap("DESC"), desc,
            attributeUpdateOverwrite, NULL); // x, y, rot, height
   }
   if (strlen(complevel))
   {
      doc->SetUnknownAttrib(&d->getAttributesRef(),get_attribmap("COMPTYPE"), complevel,
            attributeUpdateOverwrite, NULL); // x, y, rot, height
   }

   if (compname.CompareNoCase(originalcompname))
   {
      // original and new names are different
      doc->SetUnknownAttrib(&d->getAttributesRef(),get_attribmap("ORIGINALNAME"), originalcompname,
            attributeUpdateOverwrite, NULL); // x, y, rot, height

   }

   CR3000Comp  *c = new CR3000Comp;
   comparray.SetAtGrow(compcnt,c);
   compcnt++;

   c->originalname = originalcompname;
   c->name = compname;
   c->x = x;
   c->y = y;
   c->rot = rot;
   c->mirror = mir;

   return 1;
}

/****************************************************************************/
/*
   this is from wdf file
*/
static int do_lin(char *t, const char *netname)
{
   char     keyw[80];
   char     layername[80];
   char     tmp[MAX_LINE];
   CString  tmpline;
   char     padname[80];
   char     *lp;
   int      res;
   int      layernr,vertexcnt,cnt;
   double   x,y;
   int      appnr;
   int      arc;

   strcpy(keyw,t);
   clean_blank(keyw);

   if ((res = Get_Line(ifp,token, MAX_LINE)) == EOF_CHARACTER)
   {
      return -1;
   }
   tmpline = token;

   if (!STRCMPI(keyw,"LIN"))
   {
      // this is a routed line
      lp = strtok(token,":"); // Pattern
      lp = strtok(NULL,":");  // junction number start and end

      if (!strchr(lp,',')) // this is a new version (or old) which has an extra parameter.
      {
         lp = strtok(NULL,":");  // junction number start and end
      }

      lp = strtok(NULL,":");  // wire layer nr.
      layernr = atoi(lp);

      lp = strtok(NULL,":");  // vertex
      vertexcnt = atoi(lp);
      if (vertexcnt >= MAX_POLY)
      {
#ifdef _DEBUG
         CString  t1;
         t1.Format( "Too many polycnt %d in %s at %ld", vertexcnt, cur_file, Get_Linecnt());
         ErrorMessage(t1);
#endif

         fprintf(ferr, " Too many polycnt %d in %s at %ld\n", vertexcnt, cur_file, Get_Linecnt());
         display_error++;
         return -1;
      }

      for (polycnt=0;polycnt<vertexcnt;polycnt++)
      {
         res = Get_Line(ifp,token, MAX_LINE);
         lp = strtok(token,":");   // point nr
         lp = strtok(NULL,":");    // point attrib.
         arc = 0;
         strcpy(keyw,lp);
         clean_blank(keyw);

         if (strlen(keyw) == 0)
            arc = 0;
         else
         if (!STRCMPI(keyw,"ROUND"))
         {
            // ???
         }
         else
         if (!STRCMPI(keyw,"ARC"))
         {
            if (0)
               arc = 1;            // this is autoarc
            else
               arc = 0;
         }
         else
         if (!STRCMPI(keyw,"CIRCLE"))
         {
            arc = 2;            // this is arc start point clockwise
         }
         else
         if (!STRCMPI(keyw,"CIRCLE(CCW)"))
         {
            arc = 3;            // this is arc start point counter clockwise
         }
         else
         if (!STRCMPI(keyw,"WINDOW"))
           arc = -1;
         else
         if (!STRCMPI(keyw, "WINDOW,CIRCLE"))
         {
            arc = -2;
         }
         else
         if (!STRCMPI(keyw,"CIRCLE,WINDOW"))
         {
            arc = -2;
         }
         else
         if (!STRCMPI(keyw,"WINDOW,CIRCLE(CCW)"))
         {
            arc = -3;
         }
         else
         if (!STRCMPI(keyw,"CIRCLE(CCW),WINDOW"))
         {
            arc = -3;
         }
         else
         {
            fprintf(ferr,"Unknown Attribute [%s] in %s at %ld\n", keyw,cur_file,Get_Linecnt());
            display_error++;
         }

         lp = strtok(NULL,":");    // teardrop length or rad
         double rad = atof(lp);    // this is the radius of a autoarc
         lp = strtok(NULL,":");    // app number
         appnr = atoi(lp);
         lp = strtok(NULL,":");    // koos
         get_koo(lp,&x,&y);
         poly_l[polycnt].x = x;
         poly_l[polycnt].y = y;
         poly_l[polycnt].f = arc;
         poly_l[polycnt].rad = rad;
      }

      // here write poly
      sprintf(layername,"%d",get_artwork_from_wirelayer(layernr));
      layernr = Graph_Level(layername, "", 0);

      int err;
      CString  cur_turretname;
      cur_turretname.Format("TURRET_%d",appnr);
      int widthindex = Graph_Aperture(cur_turretname, 0, 0.0, 0.0,0.0, 0.0, 0.0, 0,
                     BL_APERTURE, FALSE, &err); // ensure that aperature is in database

      DataStruct *d = Graph_PolyStruct(layernr,0L,0);
      if (strlen(cur_net.name))
      {
         d->setGraphicClass(GR_CLASS_ETCH);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, (void*)cur_net.name, attributeUpdateAppend, NULL); // x, y, rot, height
      }

      write_vertexpoints(layername, widthindex, VER_LINE, cur_file, FALSE, FALSE);
      polycnt = 0;
   }
   else
   if (!STRCMPI(keyw,"SRF"))
   {
      char  att[80];
      int   nega = FALSE;
      int   cont = FALSE;
      int   turretptr;
      long  lcnt = Get_Linecnt();

      //printf("SRF not implemented in %s at %ld\n",cur_file, Get_Linecnt());
      lp = strtok(token,":");          // pattern attribute
      lp = strtok(NULL,":");           // junction no.
      lp = strtok(NULL,":");           // Attribute POSI, CONTOUR
      strcpy(att,lp);
      if (strstr(att,"NEGA"))    nega = TRUE;
      if (strstr(att,"CONTOUR")) cont = TRUE;
      lp = strtok(NULL,":");           // App number for fill lines, outlines are 0
      appnr = atoi(lp);
      lp = strtok(NULL,":");           // Paint angle
      lp = strtok(NULL,":");           // wire layer nr.
      layernr = atoi(lp);

      lp = strtok(NULL,":");           // number of points
      vertexcnt = atoi(lp);

      //d->setGraphicClass(graphic_class);
      get_vertexpoints(vertexcnt,VER_SRF,&turretptr, 0);

      int   widthindex = 0;

      // here write poly
      sprintf(layername,"%d",get_artwork_from_wirelayer(layernr));
      layernr = Graph_Level(layername, "", 0);

      DataStruct *d = Graph_PolyStruct(layernr,0L,0);

      if (strlen(cur_net.name))
      {
         d->setGraphicClass(GR_CLASS_ETCH);
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING,
            (void*)cur_net.name,
            attributeUpdateOverwrite, NULL); // x, y, rot, height
      }
      int fill = TRUE;
      if (nega)   fill = FALSE;
      write_vertexpoints(layername,widthindex,VER_SRF,cur_file, TRUE, fill);
      polycnt = 0;
   }
   else
   if (!STRCMPI(keyw,"NOD"))
   {
      // nothing
   }
   else
   if (!STRCMPI(keyw,"JOI"))
   {
      // nothing
   }
   else
   if (!STRCMPI(keyw,"VIA"))
   {
      lp = strtok(token,":"); // Pattern
      lp = strtok(NULL,":");  // junction
      lp = strtok(NULL,":");  // coos
      get_koo(lp,&x,&y);
      lp = strtok(NULL,":");  // from layer
      lp = strtok(NULL,":");  // to layer
      lp = strtok(NULL,":");  // viatable
      appnr = atoi(lp);
      sprintf(padname,"PAD_%d",appnr);
      set_viatable(appnr,2);
      lp = strtok(NULL,":");  // hole kind
      lp = strtok(NULL,":");  // hole type
      lp = strtok(NULL,":");  // number_of_wirelayers
      lp = strtok(NULL,":");  // land status

      DataStruct *d = Graph_Block_Reference(padname, NULL, -1, cnv_unit(x),cnv_unit(y),
                           DegToRad(0), 0 , 1.0, -1, TRUE);
      d->getInsert()->setInsertType(insertTypeVia);
      if (strlen(netname))
      {
         doc->SetAttrib(&d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING,
            (void *)netname,
            attributeUpdateOverwrite, NULL); // x, y, rot, height
      }
   }
   else
   if (!STRCMPI(keyw,"TERM"))
   {
      // nothing
   }
   else
   if (!STRCMPI(keyw,"LND"))
   {
      // nothing
   }
   else
   if (!_strnicmp(keyw,"TSHAPE",6))  // TSHAPE will appear as TSHAPE(
   {
      int w = Get_Linecnt();;

      // nothing
      lp = strtok(token,":"); // end of TSHAPE
      w = atoi(lp);
      
      lp = strtok(NULL,":");  // LAY
      lp = strtok(NULL,":");  // LND
      strcpy(tmp,lp);
      clean_blank(tmp);
      if (!STRCMPI(tmp,"LND"))
      {
         lp = strtok(NULL,":");  // lnd kind
         lp = strtok(NULL,":");  // app number
         lp = strtok(NULL,":");  // coo
      }
      else
      if (!STRCMPI(tmp,"VIA"))
      {
         char  *t;
         int   i, loop;
         lp = strtok(NULL,":");  // coos
         lp = strtok(NULL,":");  // from layer
         lp = strtok(NULL,":");  // to layer
         lp = strtok(NULL,":");  // via table nr.
         lp = strtok(NULL,":");  // hole nr
         lp = strtok(NULL,":");  // hole type
         lp = strtok(NULL,":");  // number of wire layers
         // it might be that the number of wire layers determine the number of 
         // elements that follow this record
         lp = strtok(NULL,":");  // hole diam
         lp = strtok(NULL,":");  //
         lp = strtok(NULL,";");  // last is semicolon.

         strcpy(tmp,lp);
         clean_blank(tmp);
         t = tmp;
         loop = 0;
         while (( lp = strtok(t," , \t")) != NULL)
               // need to do this because strtok is used later
         {
            t = NULL;
            loop++;
         }
         for (i=0;i<loop;i++)
         {
            res = Get_Line(ifp,token, MAX_LINE);   // get pad information.
            lp = strtok(token,":");
            strcpy(tmp,lp);

            if ((lp = strtok(NULL," ,\t")) != NULL)
            {
               strcpy(tmp,lp);
               clean_blank(tmp);
               if (!STRCMPI(tmp,"OBL"))
               {
                  // PAD : OBL command was read until the ; even if it
                  // spans over more lines.
                  // res = Get_Line(ifp,token, MAX_LINE);
               }
               else
               if (!STRCMPI(tmp,"SRF"))
               {
                  lp = strtok(NULL,":");
                  lp = strtok(NULL,":");
                  lp = strtok(NULL,":");
                  lp = strtok(NULL,":");
                  lp = strtok(NULL,":");
                  vertexcnt = atoi(lp);
                  for (cnt = 0;cnt < vertexcnt;cnt++)
                  {
                     res = Get_Line(ifp,token, MAX_LINE);
                  }
               }
               else
               {
                  fprintf(ferr, "Unknown PAD token [%s] at %ld\n",tmp,Get_Linecnt());
                  display_error++;
                  return -1;
               }
            }
         }
      }
      else
      if (!STRCMPI(tmp,"LIN"))
      {
         lp = strtok(NULL,":");  // line type
         lp = strtok(NULL,":");  // dashlen1
         lp = strtok(NULL,":");  // dashspace
         lp = strtok(NULL,":");  // dashlen2
         lp = strtok(NULL,":");  //
         vertexcnt = atoi(lp);
         for (cnt = 0;cnt < vertexcnt; cnt++)
         {
            res = Get_Line(ifp,token, MAX_LINE);
         }
      }
      else
      if (!STRCMPI(tmp,"SRF"))
      {
         lp = strtok(NULL,":");  // attributr
         lp = strtok(NULL,":");  // fill pattern
         lp = strtok(NULL,":");  // fill angle
         lp = strtok(NULL,":");  //
         vertexcnt = atoi(lp);
         for (cnt = 0;cnt < vertexcnt; cnt++)
         {
            res = Get_Line(ifp,token, MAX_LINE);
         }
      }
      else
      {
         fprintf(ferr, "Unknown TSHAPE pattern [%s] at %ld\n",tmp,Get_Linecnt());
         display_error++;
         return -1;
      }
   }
   else
   {
      fprintf(ferr, "Unknown keyword [%s] in do_lin-wdf at %ld\n",keyw,Get_Linecnt());
      display_error++;
      return -1;
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int do_netheader(char *t)
{
   char  tmp[80];
   char  *lp;

   strcpy(tmp,t);

   lp = strtok(tmp,":");  // netname
   strcpy(cur_net.name,lp);
   clean_blank(cur_net.name);

   lp = strtok(NULL,":");  // nettype

   return 1;
}

/****************************************************************************/
/*
*/
static int skip_unknown(FILE *ifp)
{
   int res;

   res = Read_Word(ifp,token, MAX_LINE);
   if (!strcmp(token,"{"))
   {
      do // here start { info1 info2 }
      {
         res = Read_Word(ifp,token, MAX_LINE);   // this is either {} or command
         if (strcmp(token,"}"))
         {
            res = Get_Line(ifp,token, MAX_LINE);
         }
      } while (strcmp(token,"}"));
   }
   else
   {
      res = Get_Line(ifp,token, MAX_LINE);
   }

   return 1;
}

/****************************************************************************/
/*
*/
void clean_blank(char *s)
{
   while (strlen(s) && isspace (s[strlen(s)-1]))   s[strlen(s)-1] = '\0';
   STRREV(s);
   while (strlen(s) && isspace (s[strlen(s)-1]))   s[strlen(s)-1] = '\0';
   STRREV(s);
}

/****************************************************************************/
/*
   Unprintable char are converted into .
*/
void clean_prosa(char *s)
{
   unsigned int   i;
   // have to set char to 0..127
   for (i=0;i<strlen(s);i++)
   {
      if (!isprint(s[i]))
         s[i] = '.';      
   }
   clean_blank(s);
   while (strlen(s) && (s[strlen(s)-1]) == ';')   s[strlen(s)-1] = '\0';

   while (strlen(s) && (s[strlen(s)-1]) == '"')   s[strlen(s)-1] = '\0';
   STRREV(s);
   while (strlen(s) && (s[strlen(s)-1]) == '"')   s[strlen(s)-1] = '\0';
   STRREV(s);
}

/****************************************************************************/
/*
*/
void clean_allblank(char *s)
{
   char  tmp[MAX_LINE];
   unsigned int   i,t=0;

   clean_blank(s);
   for (i=0;i<strlen(s);i++)
   {
      if (isspace(s[i]))   continue;
      tmp[t++] = s[i];
   }
   tmp[t] = '\0';
   strcpy(s,tmp);
}

/****************************************************************************/
/*
  Koo's are in for 0.0 , 0.0
  return Zuken-Values
*/
static int get_koo(char *t, double *x, double *y)
{
   // can not use this function int i = sscanf(t,"%lf,%lf",x,y);
   unsigned int    i,ii;
   char   tmp[80];

   for (i=0;i<strlen(t) && t[i] != ','; i++)
   {
      tmp[i] = t[i];
   }
   tmp[i] = '\0';
   *x = atof(tmp);

   i++; // advance i to skip ,
   // yes here is i and ii mixed
   for (ii = 0;i<strlen(t); i++,ii++)
   {
      tmp[ii] = t[i];
   }
   tmp[ii] = '\0';
   *y = atof(tmp);

   return i;
}

/****************************************************************************/
/*
*/
static int get_turret(int app)
{
   int   i;

   for (i=0;i<turretcnt;i++)
   {
      if (turret[i].number == app)
         return i;
   }
   return -1; // for compiler only
}

/****************************************************************************/
/*
  1 = pad
  2 = via
*/
static int set_viatable(int via,int typ)
{
   int   i;
   for (i=0;i<viatablecnt;i++)
   {
      if (viatable[i].number == via)
      {
         viatable[i].used |= typ;
         return 1;
      }
   }
   return 0;
}

/****************************************************************************/
/*
  write the used viatable entries
*/
static int write_padstack()
{
   int      i,ii;
   char     padname[40];
   char     layername[40];
   DbFlag   flg = 0;

   // first write pad
   for (i=0;i<viatablecnt;i++)
   {
      sprintf(padname,"PAD_%d",viatable[i].number);
      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,padname,-1,0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      CString  padshapename;

      if (viatable[i].holediameter > 0)
      {
         // here make a drill aperture.
         int drillayer = Graph_Level("DRILLHOLE","",0);
         int drillindex = get_drillindex(cnv_unit(viatable[i].holediameter), drillayer);
         CString  drillname;
         drillname.Format("DRILL_%d",drillindex);
         // must be filenum 0, because apertures are global.
         Graph_Block_Reference(drillname, NULL, 0, 0.0, 0.0, 0.0,
                               0, 1.0, drillayer, TRUE);
      }

      if (viatable[i].comp_app)
      {
         int layernum = Graph_Level("PADTOP","",0);
         padshapename.Format("TURRET_%d",viatable[i].comp_app);
         Graph_Block_Reference(padshapename, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
      }

      if (viatable[i].inner_app)
      {
         int layernum = Graph_Level("PADINNER","",0);
         padshapename.Format("TURRET_%d",viatable[i].inner_app);
         Graph_Block_Reference(padshapename, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
      }

      if (viatable[i].solder_app)
      {
         int layernum = Graph_Level("PADBOT","",0);
         padshapename.Format("TURRET_%d",viatable[i].solder_app);
         Graph_Block_Reference(padshapename, NULL, 0, 0.0, 0.0, 0.0,
                            0, 1.0, layernum, TRUE);
      }
      Graph_Block_Off();
   }

   // here write land
   for (i=0;i<landcnt;i++)
   {
      int l;
      sprintf(padname,"LND_%d_%d",land[i].turret,land[i].layer);
      l = get_wirespec_from_layernr(land[i].layer);
      if (l == 1)
      {
         strcpy(layername,"PADTOP");
      }
      else
      if (l == number_of_layers)
      {
         strcpy(layername,"PADBOT");
      }
      else
      {
         // strange SMD pin layer ???
         fprintf(ferr,"Strange LND pin layer [%d] in [%s] \n", land[i].layer, padname);
         display_error++;
         sprintf(layername,"LND%d", land[i].layer);
      }

      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,padname,-1,0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      CString  cur_turretname;
      //int    err;
      cur_turretname.Format("TURRET_%d",land[i].turret);

      DataStruct *d = Graph_Block_Reference(cur_turretname, NULL, 0, 
         0.0, 0.0, DegToRad(0.0), 0 , 1.0, Graph_Level(layername,"",0), TRUE);

      int pt = get_part(padname);
      if (pt > -1)
      {
	      switch (partarray[pt]->parttype)
	      {
		      case PART_TOOL:
      		  {
	      		curblock->setBlockType(BLOCKTYPE_TOOLING);
		      	d->getInsert()->setInsertType(INSERTTYPE_TOOLING); 
		      }
		      break;
		      case PART_FIDUCIALBOT:
			      if (d->getInsert()->getGraphicMirrored() == 0)
			      {
                     d->getInsert()->setPlacedBottom(true);
			      }
		      case PART_FIDUCIALTOP:
		      case PART_FIDUCIAL:
		      {   
			      curblock->setBlockType(BLOCKTYPE_FIDUCIAL);
			      d->getInsert()->setInsertType(INSERTTYPE_FIDUCIAL);
		      }
		      break;
		      default:
		      break;
	      }
      }
      Graph_Block_Off();
   }
 
   int smdappcnt = 0;

   // here write smd
   for (i=0;i<smdpincnt;i++)
   {
      int l;
      sprintf(padname,"SMD_%d",i);
      l = get_wirespec_from_layernr(smdpin[i].layer);
      if (l == 1)
      {
         strcpy(layername,"PADTOP");
      }
      else
      if (l == number_of_layers)
      {
         strcpy(layername,"PADBOT");
      }
      else
      {
         // strange SMD pin layer ???
         fprintf(ferr,"Strange SMD pin layer [%d] in [%s]\n",smdpin[i].layer, padname);
         display_error++;
         sprintf(layername,"SMD%d",smdpin[i].layer);
      }

      BlockStruct *curblock = Graph_Block_On(GBO_APPEND,padname,-1,0);
      curblock->setBlockType(BLOCKTYPE_PADSTACK);

      // check if it can be converted to an aperture !
      if (smdpin[i].cnt == 2)
      {
         double   offx = 0, offy = 0, w, h, ori = 0;
         h = sqrt(((smdpin[i].p[1].y - smdpin[i].p[0].y) * (smdpin[i].p[1].y - smdpin[i].p[0].y)) +
                ((smdpin[i].p[1].x - smdpin[i].p[0].x) * (smdpin[i].p[1].x - smdpin[i].p[0].x)));
         ori = atan2((smdpin[i].p[1].y - smdpin[i].p[0].y), (smdpin[i].p[1].x - smdpin[i].p[0].x));
         ori = RadToDeg(ori);
         while (ori < 0)      ori += 360;
         while (ori >= 360)   ori -= 360;
         // this is the aperture 
         int appptr = get_turret(smdpin[i].wcode);
         w = turret[appptr].outer;

         h += w;  // add width to "length"

         CString  appname;
         appname.Format("SMDAPP_%d", ++smdappcnt);

         // I need to optimize here, so that only new apetures are defined, which
         // do not already exist. Either by name or in a lookup list!

         // S = square, else round
         if (fabs(h - w) < SMALLNUMBER)
         {
            // square or round
				int err;
            if (turret[appptr].type == 'S')
               Graph_Aperture(appname,T_SQUARE, cnv_unit(w), 0.0, 0.0, 0.0, DegToRad(ori), 0, 0L, TRUE, &err);
            else
               Graph_Aperture(appname,T_ROUND, cnv_unit(w), 0.0, 0.0, 0.0, 0.0, 0, 0L, TRUE, &err);
         }
         else
         {
            // rectangle or oblong
            // square or round

            // atan returns height over width, so h and w is switched!

				// calcuate any offset to make the aperture centered on (0, 0)
				double xOffset = 0.0, yOffset = 0.0;
				if ((ori - 0.0) < SMALLNUMBER)
				{
					xOffset = (smdpin[i].p[0].x + smdpin[i].p[1].x) / 2.0;
					yOffset = (smdpin[i].p[0].y + smdpin[i].p[1].y) / 2.0;
				}
				else if ((ori - 90.0) < SMALLNUMBER)
				{
					xOffset = (smdpin[i].p[0].y + smdpin[i].p[1].y) / 2.0;
					yOffset = (smdpin[i].p[0].x + smdpin[i].p[1].x) / 2.0;
				}
				else if ((ori - 180.0) < SMALLNUMBER)
				{
					xOffset = (smdpin[i].p[0].x + smdpin[i].p[1].x) / -2.0;
					yOffset = (smdpin[i].p[0].y + smdpin[i].p[1].y) / -2.0;
				}
				else if ((ori - 270.0) < SMALLNUMBER)
				{
					xOffset = (smdpin[i].p[0].y + smdpin[i].p[1].y) / -2.0;
					yOffset = (smdpin[i].p[0].x + smdpin[i].p[1].x) / -2.0;
				}

            int err;
            if (turret[appptr].type == 'S')
               Graph_Aperture(appname,T_RECTANGLE, cnv_unit(h), cnv_unit(w), cnv_unit(xOffset), cnv_unit(yOffset), DegToRad(ori), 0, 0L, TRUE, &err);
            else
               Graph_Aperture(appname,T_OBLONG, cnv_unit(h), cnv_unit(w), cnv_unit(xOffset), cnv_unit(yOffset), DegToRad(ori), 0, 0L, TRUE, &err);
         }

         Graph_Block_Reference(appname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, Graph_Level(layername,"",0), TRUE);

      }
      else
      {
         DataStruct *d = Graph_PolyStruct(Graph_Level(layername,"",0),0L,0);

         CString  cur_turretname;
         int      err;
         cur_turretname.Format("TURRET_%d",smdpin[i].wcode);
         int widthindex = Graph_Aperture(cur_turretname, 0, 0.0, 0.0,0.0, 0.0, 0.0, 0,
                  BL_APERTURE, FALSE, &err); // ensure that aperature is in database

         int fill = TRUE;
         int close = TRUE;
         CPoly *p = Graph_Poly(NULL, widthindex, fill, 0, close);
         for (ii=0;ii<smdpin[i].cnt;ii++)
         {
            Graph_Vertex(cnv_unit(smdpin[i].p[ii].x), cnv_unit(smdpin[i].p[ii].y), 0);
         }
      }

      Graph_Block_Off();
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int update_land(int layer,int turret)
{
   int   i;

   for (i=0;i<landcnt;i++)
   {
      if (land[i].layer == layer && land[i].turret == turret)
         return i;
   }

   if (landcnt < MAX_LAND)
   {
      land[landcnt].layer = layer;
      land[landcnt].turret = turret;
      landcnt++;
   }
   else
   {
#ifdef _DEBUG
         CString  t1;
         t1.Format( "Too many land");
         ErrorMessage(t1);
#endif

      fprintf(ferr, "Too many land\n");
      display_error++;
      return -1;
   }

   return landcnt-1;
}

/****************************************************************************/
/*
*/
static int get_textptr(int app)
{
   int   i;

   if (app == 0)  return -1;

   for (i=0;i<texttablecnt;i++)
   {
      if (texttable[i].nr == app)
         return i;
   }

   fprintf(ferr, "Text aperature [%d] not found at %ld\n",app,Get_Linecnt());
   display_error++;
   return -1;
}

/****************************************************************************/
/*
  Converts Zuken units to inch.
*/
double cnv_unit(double x)
{
   return x * scaleFactor;
}

/****************************************************************************/
/*
*/
void init_terminal()
{
   terminalarray.SetSize(100,100);
   terminalcnt = 0;
}

/****************************************************************************/
/*
*/
void free_terminal()
{
   int   i;

   for (i=0;i<terminalcnt;i++)
   {
      delete terminalarray[i];
   }
   terminalarray.RemoveAll();
   terminalcnt = 0;
}

/****************************************************************************/
/*
*/
static int do_terminal(char *tok, int mirror)
{
   char     *lp;
   char     tmp[80];
   double   x,y;
   int      termlay;
   CString  t;

   lp = strtok(tok,":");   // terminal number
   strcpy(tmp,lp);
   clean_blank(tmp);
   if (tmp[0] != '(')
   {
      fprintf(ferr, "Terminal number ( ) expected in %s at %ld\n", cur_file,Get_Linecnt());
      display_error++;
      return -1;
   }
   tmp[0] = ' ';
   t = tmp;
   t.TrimLeft();
   t.TrimRight();
   
   if (t.Right(1) == ')')
      t.Delete(strlen(t)-1,1);
   t.TrimRight();
   strcpy(tmp, t);

   CR3000Terminal *c = new CR3000Terminal;
   terminalarray.SetAtGrow(terminalcnt,c);
   terminalcnt++;

   c->defined = FALSE;
   c->pinname = tmp;
   lp = strtok(NULL,":");    // attribute
   lp = strtok(NULL,":");    // layernumber
   termlay = atoi(lp);
   
   lp = strtok(NULL,":");    // shape noun
   c->termnoun = lp;
   lp = strtok(NULL,":");    // coor
   get_koo(lp,&x,&y);

   if (mirror)
   {
      x = -x;
      // update termlay
   }

   c->termlayer = termlay;
   c->x = x;
   c->y = y;
   lp = strtok(NULL,":");    // bond edge

   return 1;
}

/****************************************************************************/
/*
*/
static int get_terminal(const char *pinname)
{
   int   i;

   for (i=0;i<terminalcnt;i++)
   {
#ifdef _DEBUG
CR3000Terminal *tt = terminalarray[i];
#endif
      if (terminalarray[i]->pinname.Compare(pinname) == 0)
         return i;
   }
   fprintf(ferr,"Terminal not found for pin [%s] in shape [%s] in [%s]\n",
       pinname, cur_shape.name, cur_file);
   display_error++;

   return -1;
}

/****************************************************************************/
/*
  turretptr is needed to find square or round ends
*/
static int get_smdpin(int termptr,int layernr,int wcode, int turretptr)
{
   int   i,ii,found;
   int   flat = (turret[turretptr].type == 'S');

   // normalize poly to terminal
   if (termptr > -1)
   {
      for (i=0;i<polycnt;i++)
      {
         poly_l[i].x = poly_l[i].x - terminalarray[termptr]->x;
         poly_l[i].y = poly_l[i].y - terminalarray[termptr]->y;
      }
   }

   // here check if already defined
   for (i=0;i<smdpincnt;i++)
   {
      if (smdpin[i].layer != layernr)  continue;
      if (smdpin[i].flat  != flat)     continue;
      if (smdpin[i].wcode != wcode)    continue;
      if (smdpin[i].cnt != polycnt)    continue;
      found = TRUE;
      for (ii = 0;ii<polycnt;ii++)
      {
         if (fabs(cnv_unit(smdpin[i].p[ii].x - poly_l[ii].x)) > 0.001 ||
             fabs(cnv_unit(smdpin[i].p[ii].y - poly_l[ii].y)) > 0.001)
            found = FALSE;
      }
      if (found)  return i;
   }

   if (smdpincnt < MAX_SMDPIN)
   {
      smdpin[smdpincnt].layer = layernr;
      smdpin[smdpincnt].wcode = wcode;
      smdpin[smdpincnt].flat = flat;
      smdpin[smdpincnt].cnt = polycnt;
      if ((smdpin[smdpincnt].p = (Point2 *)calloc(polycnt,sizeof(Point2))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

      // poly_l is not a Point2 struct.
      for (i=0;i<polycnt;i++)
      {
         smdpin[smdpincnt].p[i].x = poly_l[i].x;
         smdpin[smdpincnt].p[i].y = poly_l[i].y;
      }
      smdpincnt++;
   }
   else
   {
#ifdef _DEBUG
         CString  t1;
         t1.Format( "Too many SMD pins");
         ErrorMessage(t1);
#endif

      fprintf(ferr, "Too many SMD pins\n");
      display_error++;
      return -1;
   }

   return smdpincnt-1;
}

/****************************************************************************/
/*
*/
static int write_shapepin(int type, const char *shapename, const char *pinname,
               char *padname, double x,double y, int insertlevel)
{
   int termptr;

   // check that padname is there and a BLOCKTYPE_PADSTACK
   BlockStruct *c = Graph_Block_On(GBO_APPEND, padname, -1, 0L);
   Graph_Block_Off();
   c->setBlockType(BLOCKTYPE_PADSTACK);

   // pin without a pinnumber is just a insert
   if (strlen(pinname) == 0)
   {
      DataStruct *d = Graph_Block_Reference(padname, NULL, 0, 
         cnv_unit(x),cnv_unit(y),DegToRad(0.0), 0 , 1.0, insertlevel, TRUE);

      BlockStruct *b = Graph_Block_Exists(doc, padname, -1);
	  b = Graph_Block_On(GBO_APPEND, padname, -1, 0);
      
      int pt = get_part(padname);
      if (pt > -1)
      {
         switch (partarray[pt]->parttype)
         {
            case PART_TOOL:
            {
               b->setBlockType(blockTypeTooling);
               d->getInsert()->setInsertType(insertTypeDrillTool);
            }
            break;
            case PART_FIDUCIALBOT:
               if (d->getInsert()->getGraphicMirrored() == 0)
               {
                  d->getInsert()->setPlacedBottom(true);
               }
            case PART_FIDUCIALTOP:
            case PART_FIDUCIAL:
            {   
               b->setBlockType(blockTypeFiducial);
               d->getInsert()->setInsertType(insertTypeFiducial);
            }
            break;
            default:
            break;
         }
      }

	   Graph_Block_Off();
      return 1;
   }

   switch (type)
   {
   case MDF_TYPE:
      {
         // update terminal definition
         if ((termptr = get_terminal(pinname)) > -1)
            terminalarray[termptr]->defined = TRUE;

         DataStruct *d = Graph_Block_Reference(padname, pinname, 0, 
            cnv_unit(x),cnv_unit(y),DegToRad(0.0), 0 , 1.0, insertlevel, TRUE);
         d->getInsert()->setInsertType(insertTypePin);
      }
      break;
   case UDF_TYPE:
      fprintf(ferr, "Pin with pinnumber insert in UDF ?\n");
      display_error++;
      break;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int check_terminals()
{
   int   i;

   for (i=0;i<terminalcnt;i++)
   {
      char  *nopad = TERMALL;

      if (terminalarray[i]->defined)   continue;
/*
      fprintf(ferr,"Graphic for Terminal [%d] for Shape [%s] is not defined.\n",
         terminalarray[i]->pinnumber,cur_shape.name);
      display_error++;
*/
      if (cur_shape.smd)   nopad = TERMTOP;

      write_shapepin(MDF_TYPE, cur_shape.name, terminalarray[i]->pinname, nopad,
         terminalarray[i]->x, terminalarray[i]->y, -1);
   }
   return 1;
}

/****************************************************************************/
/*
  return used appnr
*/
static int get_vertexpoints(int vertexcnt,int typ, int *turretpointer, int mirror)
{
   char  *lp;
   int   res;
   char  tmp[80];
   double  rad,x,y, bulge = 0;
   int   appnr = 0, arc = 0;

   *turretpointer = -1;

   if (vertexcnt >= MAX_POLY)
   {
      if (vertexcnt == 350528)
      {
         // there seems to be an error in Mark Geises files,
         fprintf(ferr, " Data error in number of vertices [%d] -> changed to 2 in %s at %ld\n",
            vertexcnt, cur_file, Get_Linecnt());
         display_error++;            
         vertexcnt = 2;
      }
      else
      {
#ifdef _DEBUG
         CString  t1;
         t1.Format( " Too many vertices %d in %s at %ld", vertexcnt, cur_file, Get_Linecnt());
         ErrorMessage(t1);
#endif

         fprintf(ferr, " Too many vertices %d in %s at %ld\n", vertexcnt, cur_file, Get_Linecnt());
         display_error++;
         return -1;
      }
   }

   for (polycnt=0;polycnt<vertexcnt;polycnt++)
   {
      res = Get_Line(ifp,token, MAX_LINE);

      // do not check for blank or tab
      lp = strtok(token,":");   // point number
      lp = strtok(NULL,":");    // point attrib
      strcpy(tmp,lp);
      clean_allblank(tmp);

      if (strlen(tmp) == 0)
         arc = 0;
      else
      if (!STRCMPI(tmp,"ROUND"))
      {
         // fan designation ???
      }
      else
      if (!STRCMPI(tmp,"ARC"))
      {
         if (0)
            arc = 1;            // this is autoarc
         else
            arc = 0;
      }
      else
      if (!STRCMPI(tmp,"CIRCLE"))   // n coo is start, n+1 is center, n+2 is end
      {
         if (mirror)
            arc = 3;
         else
            arc = 2;            // this is arc start point clockwise
      }
      else
      if (!STRCMPI(tmp,"CIRCLE(CCW)")) // n coo is start, n+1 is center, n+2 is end
      {
         if(mirror)
            arc = 2;
         else
            arc = 3;            // this is arc start point counter clockwise
      }
      else
      if (!STRCMPI(tmp,"WINDOW"))
        arc = -1;
      else
      if (!STRCMPI(tmp,"WINDOW,CIRCLE"))  // n coo is start, n+1 is center, n+2 is end
      {
         if (mirror)
            arc = -3;
         else
            arc = -2;
      }
      else
      if (!STRCMPI(tmp,"CIRCLE,WINDOW"))        // n coo is start, n+1 is center, n+2 is end
      {
         if (mirror)
            arc = -3;
         else
            arc = -2;
      }
      else
      if (!STRCMPI(tmp,"WINDOW,CIRCLE(CCW)"))   // n coo is start, n+1 is center, n+2 is end
      {
         if (mirror)
            arc = -2;
         else
            arc = -3;
      }
      else
      if (!STRCMPI(tmp,"CIRCLE(CCW),WINDOW"))   // n coo is start, n+1 is center, n+2 is end
      {
         if(mirror)
            arc  = -2;
         else
            arc = -3;
      }
      else
      {
         fprintf(ferr,"Unknown Attribute [%s] in %s at %ld\n", tmp,cur_file,Get_Linecnt());
         display_error++;
      }
      rad = 0;

      if (typ != VER_SRF && typ != VER_ARC)
      {
         lp = strtok(NULL,":");    // fan lenght
         rad = atof(lp);           // this is the radius of a autoarc
      }

      if (typ != VER_ARC)  // on ARC the next entry is the radius
      {
         if ((lp = strtok(NULL,":")) != NULL)   // appnr
         {
            if (arc == 1)
            {
               rad = atof(lp);
            }
            else
            {
               *turretpointer = get_turret(atoi(lp));
               appnr = atoi(lp);
            }
         }
      }

      lp = strtok(NULL,":");
      get_koo(lp,&x,&y);

      if (mirror)  x = -x;

      poly_l[polycnt].x = x;
      poly_l[polycnt].y = y;
      poly_l[polycnt].rad = rad;
      poly_l[polycnt].f = arc;

   }
   return appnr;
}

/****************************************************************************/
/*
*/
static int write_vertexpoints(char *layername,int widthindex, int type, char *fname, int close, int fill)
{
   int      cnt;
   double   sa,da,rad,lastx=0.0,lasty=0.0;
   CPnt     *lastv = NULL;

   if (polycnt == 0) return 1;

   CPoly *p = Graph_Poly(NULL, widthindex, fill, 0, close);

   for (cnt=0;cnt < polycnt;cnt++)
   {
      double   bulge = 0;

      // a autocircle 1 is not implemented yet.
      // a circle flg 2 or 3 is startpoint, +1 centerpoint, +2 endpoint
      if (poly_l[cnt].f == 2) //clockwise arc
      {
         // cnt is start
         // cnt+1 is center 
         // cnt + 2 is end
         ArcCenter2( poly_l[cnt].x, poly_l[cnt].y, poly_l[cnt+2].x, poly_l[cnt+2].y,
                     poly_l[cnt+1].x, poly_l[cnt+1].y, &rad,&sa,&da, TRUE);

         if (poly_l[cnt].x == poly_l[cnt+2].x && poly_l[cnt].y == poly_l[cnt+2].y )
         {
            // full circle
            // make a new poly if not already new
            double x1,y1;
            x1 = poly_l[cnt+1].x - poly_l[cnt].x;
            y1 = poly_l[cnt+1].y - poly_l[cnt].y;

            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), 1);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x+2*x1), cnv_unit(poly_l[cnt].y+2*y1), 1);
         }
         else
         {
            bulge = tan(da/4);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), bulge);
         }
         cnt = cnt+1;  // 1 is automatically incremented in for loop
         continue;
      }
      else
      if (poly_l[cnt].f == 3) // counter clock
      {
         ArcCenter2( poly_l[cnt].x, poly_l[cnt].y, poly_l[cnt+2].x, poly_l[cnt+2].y,
                     poly_l[cnt+1].x, poly_l[cnt+1].y, &rad,&sa,&da,FALSE);

         if (poly_l[cnt].x == poly_l[cnt+2].x && poly_l[cnt].y == poly_l[cnt+2].y )
         {
            // full circle
            // make a new poly if not already new
            double x1,y1;
            x1 = poly_l[cnt+1].x - poly_l[cnt].x;
            y1 = poly_l[cnt+1].y - poly_l[cnt].y;

            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), 1);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x+2*x1), cnv_unit(poly_l[cnt].y+2*y1), 1);
         }
         else
         {
            bulge = tan(da/4);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), bulge);
         }
         cnt = cnt+1;  // 1 is automatically incremented in for loop
         continue;
      }
      else
      if (poly_l[cnt].f == -1)
      {
         // start a new void
         CPoly *p = Graph_Poly(NULL, widthindex, TRUE, TRUE, TRUE);
      }
      else
      if (poly_l[cnt].f == -2) // void circle
      {
         //if (fill) always void
         CPoly *p = Graph_Poly(NULL, widthindex, TRUE, TRUE, TRUE);

         ArcCenter2( poly_l[cnt+2].x, poly_l[cnt+2].y, poly_l[cnt].x,  poly_l[cnt].y,
                     poly_l[cnt+1].x, poly_l[cnt+1].y, &rad,&sa,&da,TRUE);

         if (poly_l[cnt].x == poly_l[cnt+2].x && poly_l[cnt].y == poly_l[cnt+2].y )
         {
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt+1].x-rad), cnv_unit(poly_l[cnt+1].y), 1);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt+1].x+rad), cnv_unit(poly_l[cnt+1].y), 1);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt+1].x-rad), cnv_unit(poly_l[cnt+1].y), 0);
            cnt = cnt+2;
            continue;
         }
         else
         {
            bulge = tan(da/4);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), bulge);
         }
         cnt = cnt+1;   // 1 is automatically incremented in for loop
         continue;
      }
      else
      if (poly_l[cnt].f == -3) // void circle (ccw)
      {
         //if (fill) always void
         CPoly *p = Graph_Poly(NULL, widthindex, TRUE, TRUE, TRUE);

         ArcCenter2( poly_l[cnt].x, poly_l[cnt].y, poly_l[cnt+2].x, poly_l[cnt+2].y,
                     poly_l[cnt+1].x, poly_l[cnt+1].y, &rad,&sa,&da,FALSE);

         if (poly_l[cnt].x == poly_l[cnt+2].x && poly_l[cnt].y == poly_l[cnt+2].y )
         {
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt+1].x-rad), cnv_unit(poly_l[cnt+1].y), 1);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt+1].x+rad), cnv_unit(poly_l[cnt+1].y), 1);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt+1].x-rad), cnv_unit(poly_l[cnt+1].y), 0);
            cnt = cnt+2;
            continue;
         }
         else
         {
            bulge = tan(da/4);
            lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), bulge);
         }
         cnt = cnt+1;   // 1 is automatically incremented in for loop
         continue;
      }
      else
      {
         if (poly_l[cnt].f == 1 && poly_l[cnt].rad > SMALL_RADIUS)
         {
            // calculate dx, dy from radius and angle
            // find the 2 vertex endpoints
            // calculate arc points

            double p1x, p1y;
            if (FindPointOnLine(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y),
                     cnv_unit(poly_l[cnt-1].x), cnv_unit(poly_l[cnt-1].y), 
                     cnv_unit(poly_l[cnt].rad), &p1x, &p1y))
            {
               lastv = Graph_Vertex(p1x, p1y, 0);
            }
            else
            {
int y = 0;
            }

            double p2x, p2y;
            if (FindPointOnLine(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), 
                     cnv_unit(poly_l[cnt+1].x), cnv_unit(poly_l[cnt+1].y), 
                     cnv_unit(poly_l[cnt].rad), &p2x, &p2y))
            {
               lastv = Graph_Vertex(p2x, p2y, 0);
            }
            else
            {
int y = 0;     
            }
            //cnt = cnt+1;
            continue;
         }
      }

      lastv = Graph_Vertex(cnv_unit(poly_l[cnt].x), cnv_unit(poly_l[cnt].y), bulge);
   }

   polycnt = 0;
   return 1;
}

/****************************************************************************/
/*
*/
static int write_squarehole(char *layername,double x, double y, 
                  double h, double w, double rot)
{
   double   x1,y1,x2,y2;

#ifdef WOLF
   flg = GR_CLOSEDPOLY;
#endif

   x1 = x - w/2;  // koos are the center koos.
   y1 = y - h/2;
   x2 = x + w/2;
   y2 = y + h/2;

   Rotate(x - w/2,y - h/2,rot,&x1,&y1);
   Rotate(x + w/2,y - h/2,rot,&x2,&y2);

#ifdef WOLF
   LG_Segm(0,inch_2_dbunitpoint2(cnv_unit(x1),cnv_unit(y1)),
           flg,layername,0,
           inch_2_dbunit(cnv_unit(x2-x1)),inch_2_dbunit(cnv_unit(y2-y1)));
   flg = GR_CONTINUE;

   x1 = x2;
   y1 = y2;
   Rotate(x + w/2,y + h/2,rot,&x2,&y2);
   LG_Segm(0,inch_2_dbunitpoint2(cnv_unit(x1),cnv_unit(y1)),
           flg,layername,0,
           inch_2_dbunit(cnv_unit(x2-x1)),inch_2_dbunit(cnv_unit(y2-y1)));
   x1 = x2;
   y1 = y2;
   Rotate(x - w/2,y + h/2,rot,&x2,&y2);
   LG_Segm(0,inch_2_dbunitpoint2(cnv_unit(x1),cnv_unit(y1)),
           flg,layername,0,
           inch_2_dbunit(cnv_unit(x2-x1)),inch_2_dbunit(cnv_unit(y2-y1)));

   x1 = x2;
   y1 = y2;
   Rotate(x - w/2,y - h/2,rot,&x2,&y2);
   LG_Segm(0,inch_2_dbunitpoint2(cnv_unit(x1),cnv_unit(y1)),
           flg,layername,0,
           inch_2_dbunit(cnv_unit(x2-x1)),inch_2_dbunit(cnv_unit(y2-y1)));
#endif
   return 1;
}

/****************************************************************************/
/*
*/
static int get_artwork_from_wirelayer(int wirelay)
{
   int   i;

   for (i=0;i<wirelayspeccnt;i++)
   {
      if (wirelayspec[i].wirelay == wirelay)
         return wirelayspec[i].number;
   }

   fprintf(ferr, "Error : No Artwork layer found for Wire layer [%d]\n",wirelay);
   display_error++;

   return wirelay;
}

/****************************************************************************/
/*
*/
static int get_wirespec_from_layernr(int lay)
{
   int   i;

   for (i=0;i<wirelayspeccnt;i++)
   {
      if (wirelayspec[i].number == lay)
         return wirelayspec[i].wirelay;
   }
   return lay;
}

/****************************************************************************/
/*
  get a U1(1) combination until ;
*/
static int get_nextpin(FILE *fp, char *compname, char *pinname)
{
   CHAR c;
   int  res;
   int  ccnt = 0,pcnt = 0;
   int  comp = TRUE;

   compname[0] = '\0';
   pinname [0] = '\0';

   while (TRUE)
   {
      if ((res = Read_Char(fp,&c)) == EOF_CHARACTER)
      {
         fprintf(ferr, "Error in CCF file -> [;] expected\n");
         display_error++;
         return -1;
      }

      if (isspace(c))   continue;
      if (c == ',')     continue;
      if (c == ';')     return FALSE;  // end of net
      if (c == '(')
      {
         comp = FALSE;
         continue;
      }
      if (c == ')')     return TRUE;

      if (comp)
      {
         compname[ccnt++] = c;
         compname[ccnt] = '\0';
      }
      else
      {
         pinname[pcnt++] = c;
         pinname[pcnt] = '\0';
      }

   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int write_layerattr()
{
   int   i;
   CString  toplayer, botlayer;

   LayerStruct *lp;

   for (i=0;i<wirelayspeccnt;i++)
   {
      CString  layname;

      layname.Format("%d",wirelayspec[i].number);

      LayerStruct *l;
      if ((l = doc->FindLayer_by_Name(layname)) != NULL)
      {
         if (wirelayspec[i].wirelay == 1)
         {
            l->setLayerType(LAYTYPE_SIGNAL_TOP);
            toplayer = layname;
         }
         else
         if (wirelayspec[i].wirelay == number_of_layers)
         {
            l->setLayerType(LAYTYPE_SIGNAL_BOT);
            botlayer = layname;
         }
         else
         {
            if (wirelayspec[i].negative)
               l->setLayerType(LAYTYPE_POWERNEG);
            else
               l->setLayerType(LAYTYPE_SIGNAL_INNER);
         }
         l->setElectricalStackNumber(wirelayspec[i].wirelay);
      }
   }

   // top and bottom must be mirrored
   Graph_Level_Mirror(toplayer, botlayer, "");
   // top and bottom pad layer
   Graph_Level_Mirror("PADTOP", "PADBOT", "");

   for (i=0;i<MAX_LAYERS;i++)
   {
      if (mirrorlayers[i] == i)  continue;
      CString  l1, l2;
      l1.Format("%d",i);
      l2.Format("%d",mirrorlayers[i]);
      Graph_Level(l1,"", 0);
      Graph_Level(l2,"", 0);
      Graph_Level_Mirror(l1, l2, "");
   }

   for (i=0;i<artlayspeccnt;i++)
   {
      CString  layname;

      layname.Format("%d",artlayspec[i].number);

      LayerStruct *l;
      if ((l = doc->FindLayer_by_Name(layname)) != NULL)
      {
         l->setComment(artlayspec[i].comment);
      }
   }

   // now all mirror layers
   CString  boardoutlinelayer;
   if (contour_lay_no > 0)
      boardoutlinelayer.Format("%d",contour_lay_no);
   else
      boardoutlinelayer = "";

   for (i = 0; i< doc->getMaxLayerIndex(); i++)
   {
      lp = doc->getLayerArray()[i];
      if (lp == NULL)   continue; // could have been deleted.

      if (!strcmp(lp->getName(),"PADTOP"))
      {
         lp->setLayerType(LAYTYPE_PAD_TOP);
      }
      else
      if (!strcmp(lp->getName(),"PADBOT"))
      {
         lp->setLayerType(LAYTYPE_PAD_BOTTOM);
      }
      else
      if (!strcmp(lp->getName(),"PADINNER"))
      {
         lp->setLayerType(LAYTYPE_PAD_INNER);
      }
      else
      if (!strcmp(lp->getName(),boardoutlinelayer))
      {
         lp->setLayerType(LAYTYPE_BOARD_OUTLINE);
         if (strlen(lp->getComment()) == 0)
            lp->setComment("BOARD OUTLINE");
      }

   }

   return 1;
}

/******************************************************************************
* load_cr3000settings
*/
static int load_cr3000settings(const CString fname)
{
   FILE *zuktec;
   char tmp[255], *lp;

   USE_COMPSHAPE_NAME = FALSE;
   USE_SOLDER_MDF = FALSE;
   REVERSE_ORDER = FALSE;
   USE_PIN_NAME = FALSE;
   compoutlinecnt = 0;
   ComponentSMDrule = 0;

   if ((zuktec = fopen(fname, "rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fname);
      ErrorMessage(tmp, "CR3000 Settings", MB_OK | MB_ICONHAND);
      return 0;
   }

   while (fgets(tmp, 255, zuktec))
   {
      if ((lp = get_string(tmp, " \t\n")) == NULL)
         continue;
      if (lp[0] == '.')
      {
         // here do it.
         if (!STRCMPI(lp, ".ComponentSMDrule"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            ComponentSMDrule = atoi(lp);
         }
         else if (!STRCMPI(lp, ".SMALL_RADIUS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            SMALL_RADIUS = atof(lp);
         }
         else if (!STRCMPI(lp, ".USE_COMPSHAPE_NAME"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               USE_COMPSHAPE_NAME = TRUE;
         }
         else if (!STRCMPI(lp, ".USE_PIN_NAME"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               USE_PIN_NAME = TRUE;
         }
         else if (!STRCMPI(lp, ".USE_SOLDER_MDF"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               USE_SOLDER_MDF = TRUE;
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
         else if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString a1 = lp;
            a1.MakeUpper();
            a1.TrimLeft();
            a1.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString a2 = lp;
            a2.MakeUpper();
            a2.TrimLeft();
            a2.TrimRight();

            CR3000Attribmap *c = new CR3000Attribmap;
            attribmaparray.SetAtGrow(attribmapcnt++, c);
            c->attrib = a1;
            c->mapattrib = a2;
         }
         else if (!STRCMPI(lp, ".REVERSE_ORDER"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               REVERSE_ORDER = TRUE;
         }
         else if (!STRICMP(lp, ".FIDUCIALPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_FIDUCIAL);
         }
         else if (!STRICMP(lp, ".TOOLPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_TOOL);
         }
         else if (!STRICMP(lp, ".FIDUCIALTOPPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_FIDUCIALTOP);
         }
         else if (!STRICMP(lp, ".FIDUCIALBOTPART"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString part = _strupr(lp);
            part.TrimLeft();
            part.TrimRight();
            update_part(part, PART_FIDUCIALBOT);
         }
         else if (!STRICMP(lp, ".COPYDEVICE"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString tempString = _strupr(lp);
            tempString.TrimLeft();
            tempString.TrimRight();
			if (!tempString.IsEmpty())
				copydevice = tempString;
         }

      }
   }
   fclose(zuktec);

   return 1;
}

/****************************************************************************/
/*
*/
static int update_part(const char *p, int typ)
{

   for (int i=0;i<partcnt;i++)
   {
      if (partarray[i]->name.CompareNoCase(p) == 0)
      {
         if (typ > 0)
            partarray[i]->parttype = typ;
         return i;
      }
   }

   Cr3000Part *c = new Cr3000Part;
   partarray.SetAtGrow(partcnt,c);  
   partcnt++;
   c->name = p;
   c->parttype = typ;

   return partcnt-1;
}

/****************************************************************************/
/*
   here allow wildcard * and ? 
*/
static int get_part(const char *p)
{

   for (int i=0;i<partcnt;i++)
   {
      if (wildcard_compare(p,partarray[i]->name, 1) == 0)
      {
         return i;
      }
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int make_arctopoly(double xs,double ys,double xe, double ye,
               double rx,double ry, double da,
               int w, char *l, DbFlag flg)
{
   int      ppolycnt, i;
   Point2   *ppoly;

   ppolycnt = MAX_POLY;
   if ((ppoly = (Point2 *)calloc(MAX_POLY,sizeof(Point2))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);


   ArcPoly2(xs,ys,rx,ry,da,15.0,ppoly,&ppolycnt);

   // make sure that the begin and end points of the arc match the
   // the points specified in the input file
   if ( ppolycnt > 1 )
   {
      ppoly[0].x = xs;
      ppoly[0].y = ys;
      ppoly[ppolycnt-1].x = xe;
      ppoly[ppolycnt-1].y = ye;
   }

   for (i=1;i<ppolycnt;i++)
   {
      if (fabs(ppoly[i].x - ppoly[i-1].x) > 0.001 ||
          fabs(ppoly[i].y - ppoly[i-1].y) > 0.001)
      {
#ifdef WOLF
         LG_Segm(0,inch_2_dbunitpoint2(ppoly[i-1].x,ppoly[i-1].y),flg,
              l,w,
              inch_2_dbunit(ppoly[i].x) - inch_2_dbunit(ppoly[i-1].x),
              inch_2_dbunit(ppoly[i].y) - inch_2_dbunit(ppoly[i-1].y));
         flg = GR_CONTINUE;
#endif
      }
   }

   free((char *)ppoly);
   return 1;
}

/******************************************************************************
* CopyAttribValues
*/
static void CopyAttribValues(FileStruct *file, WORD srcIndex, WORD destIndex, CCEtoODBDoc *doc, AttributeUpdateMethodTag method)
{
	if (file == NULL || doc == NULL)
		return;

	const KeyWordStruct *srcKW = doc->getKeyWordArray()[srcIndex];
	const KeyWordStruct *destKW = doc->getKeyWordArray()[destIndex];

	if (srcKW->getValueType() != destKW->getValueType())
		return;

	CopyAttribValues(file->getBlock(), srcKW, destKW, doc, method);
}

/******************************************************************************
* CopyAttribValues
*/
static void CopyAttribValues(BlockStruct *block, const KeyWordStruct *srcKW, const KeyWordStruct *destKW, CCEtoODBDoc *doc, AttributeUpdateMethodTag method)
{
	if (block == NULL || doc == NULL)
		return;

	if (srcKW->getValueType() != destKW->getValueType())
		return;

	POSITION pos = block->getHeadDataPosition();
	while (pos)
	{
		DataStruct *data = block->getNextData(pos);

		Attrib* attrib = NULL;
		if (!data->lookUpAttrib(srcKW->getIndex(), attrib))
			continue;

		switch (attrib->getValueType())
		{
			case valueTypeString:
			case valueTypeEmailAddress:
			case valueTypeWebAddress:
				{
					const char *value = get_attvalue_string(doc, attrib);
#if CamCadMajorMinorVersion > 406  //  > 4.6
               data->setAttrib(doc->getCamCadData(), destKW->getIndex(), attrib->getValueType(), (VOID*)value, method, NULL);
#else
               data->setAttrib(doc, destKW->getIndex(), attrib->getValueType(), (VOID*)value, method, NULL);
#endif
				}
				break;
			case valueTypeDouble:
			case valueTypeUnitDouble:
				{
					double value = attrib->getDoubleValue();
#if CamCadMajorMinorVersion > 406  //  > 4.6
               data->setAttrib(doc->getCamCadData(), destKW->getIndex(), attrib->getValueType(), (VOID*)&value, method, NULL);
#else
					data->setAttrib(doc, destKW->getIndex(), attrib->getValueType(), (VOID*)&value, method, NULL);
#endif
				}
				break;
			case valueTypeInteger:
				{
					int value = attrib->getIntValue();
#if CamCadMajorMinorVersion > 406  //  > 4.6
               data->setAttrib(doc->getCamCadData(), destKW->getIndex(), attrib->getValueType(), (VOID*)&value, method, NULL);
#else
					data->setAttrib(doc, destKW->getIndex(), attrib->getValueType(), (VOID*)&value, method, NULL);
#endif
				}
				break;
		}
		
		if (data->getDataType() == dataTypeInsert)
		{
			BlockStruct *subBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
			CopyAttribValues(subBlock, srcKW, destKW, doc, method);
		}
	}
}

/*Ende **********************************************************************/