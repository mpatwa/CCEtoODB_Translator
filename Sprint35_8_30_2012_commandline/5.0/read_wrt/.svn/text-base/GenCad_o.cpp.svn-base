// $Header: /CAMCAD/4.5/read_wrt/GenCad_o.cpp 80    8/05/06 4:54p Kurt Van Ness $

/****************************************************************************/
/*

  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1999. All Rights Reserved.

  GENCAD is WHITESPACE sensitive !!!!
  Do not make the file "beautifull"


   A boardoutline is graphic marked as Primary Boardoutline or placed on a layer->type LAYTYPE_BOARDOUTLINE
   A silkscreen is a graphic marked as Primary Silkscreen or placed on a layer->type LAYTYPE_SILK_TOP

*/

/****************RECENT UPDATES**************
*
*  Revamping - Sadek Noureddine
*
*  TSR 3674: 07-01-02 - Sadek Noureddine
*
*/
                              
#include "stdafx.h"
#include "ccdoc.h"   
#include "graph.h"             
#include "gencad.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include <math.h>
#include "pcbutil.h"
#include "pcblayer.h"
#include "apertur2.h"
#include "polylib.h"
#include "GenCad_o.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "dcaentity.h"

// E.g. 4.6 becomes 406, 4.10 becomes 410
#define CamCadMajorMinorVersion ((CamCadMajorVersion *100) + CamCadMinorVersion)


extern CProgressDlg*			progress;
//extern LayerTypeInfoStruct layertypes[];

// layer flags for connect layer table
#define LAY_TOP              -1
#define LAY_BOT              -2
#define LAY_INNER            -3
#define LAY_ALL              -4
#define LAY_OUTER            -5

// optimize complex apertures
#define PADSHAPE_UNKNOWN     0x0      
#define PADSHAPE_CIRCLE      0x1
#define PADSHAPE_POLYGON     0x2
#define PADSHAPE_RECTANGULAR 0x4


static CCEtoODBDoc*			doc;
static double					unitsFactor;
static int						output_units_accuracy = 3;

static long						cur_artworkcnt;
static int						display_error;
static FILE*					flog;
static CString					testpinprefix;
static CString					fiducialname;
static CString					panelfiducialname;
static int						viacnt = 0;
static int						testpadcnt = 0;

static CComplistArray		complistarray;    // this is the device - mapping
static int						complistcnt;

static CompPinInstArray		comppininstarray; 
static int						comppininstcnt;

//static GENCADPadstackArray	padstackarray; 
//static int						padstackcnt;

static GENCADRoutesArray	routesarray;   
static int						routescnt;

static CGenCADLayerArray	glArr;
static int						maxArr = 0;

static PadRotArray			padrotarray;
static int						padrotcnt;

static int						component_outline_layer[10];
static int						component_outline_cnt;

static double					defaultdrillsize;
static double					defaultnopadsize;

static int						update_device_attribute;
static int						namespace_allowed;
static int						write_artwork;
static int						write_gen_comp_to_shape;
static int						output_graphic_data;
static int						convert_tp_comps; // convert testpoints to component and create a shape with the 
															// padstack as it's contents.

static CString					testpointshapeprefix;
static CString					generic_insert;
static CString					mechanical_insert;
static CString					unknown_insert;
static bool						do_normalize_bottom_build_geometries;
static bool                treat_mechanical_pins_as_normal_pins;
static CString             outputUnit;
static int                 outputPInt;
static double GetUnitsFactor(double scale,int PageUnits);
static CString GetUnitsString();
static void GatherPadstacksAndRoutesData(CCEtoODBDoc* pDoc, FileStruct* pPCBFile, CGenCadPadstackMap& padstackMap);
static int GENCAD_RotatedPadstackData(CDataList *DataList, DbUnit insertAngle = 0., CGenCadPadstackMap* padstackMap = NULL);

static int GENCAD_WritePADSTACKData(FILE *wfp, const CString padstkname, CDataList *DataList, double rotation, int mirror,
		double scale, int embeddedLevel, int insertLayer, bool toppad, bool dupMirPadstack);

// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void GENCAD_WritePRIMARYBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, GraphicClassTag graphic_class);
// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void GENCAD_WriteBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, int layertype);
// this function only loops throu entities marked other than BL_CLASS_BOARD 
static void GENCAD_WriteARTWORKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
// this function only loops throu entities marked other than BL_CLASS_BOARD 
static long GENCAD_WriteSHAPEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,
      int artwork_or_shape, long acnt, int from_generic_component = FALSE);

static long GENCAD_WriteComplexApertureData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int apptyp);

static int GENCAD_TestComplexApertureData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel);

static void GENCAD_WriteCOMPONENTData(FILE* wfp, FileStruct* file, BlockStruct* fileBlock,
      double insert_x, double insert_y, double rotation, int mirror, double scale,
		int embeddedLevel, int insertLayer, CGenCadDeviceMap& deviceMap);

static void GENCAD_WriteMECHData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int panelfile, int insertlevel);

static void GENCAD_WriteTestProbeData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int panelfile, int insertLayer);

static void GENCAD_WriteROUTESData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname, CGenCadPadstackMap& padstackMap);

static void GENCAD_GetROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static void GENCAD_WritePANELBOARDSData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, 
      const char *fname, const char *ext);

static void GENCAD_WriteTESTPOINTData(FILE *wfp, CDataList *DataList, const char *netname);
static void write_pcbshapes(FILE *fp, double scale, int artwork_or_shape, long *acnt, int *shapeattcnt,  BlockStruct *block);
static void write_mech_shape(FILE *fp, double scale, CDataList *DataList, long *acnt, int *shattcnt, int IsArtwork);
static void FilterIllegalChar(CString & textData);
/****************************************************************************/
/*
*/
//static int get_layertype(const char *l)
//{
//
//   for (int i=0;i<MAX_LAYTYPE;i++)
//   {
//      if (_STRICMP(layerTypeToString(i), l) == 0)
//         return i;
//   }
//
//   return   LAYTYPE_UNKNOWN; 
//}

//--------------------------------------------------------------
static CString gen_string(const char *nn)
{
   CString name, n;
   int space = 0;

   n = nn;
   if (!namespace_allowed)
   {
      n.Replace(" ", "_");
      n.Replace("\t", "_");
   }

   name = "";
   for (int i=0; i<(int)strlen(n); i++)
   {
      if (isspace(n[i]))
         space++;
      if (n[i] == '"')
         name += '\\';
      name += n[i];
   }

   if (space)
   {
      CString  tmp;
      tmp = '"';
      tmp += name;
      tmp += '"';
      name = tmp;
   }

   if (name.CompareNoCase("CLX_pad_13x18") == 0)
   {
      int iii = 3;
   }

   return name;
}

/****************************************************************************/
/*
   return NULL if not visible
*/
static const char *Layer_Gencad(int l)
{

   for (int i=0;i<maxArr;i++)
   {
      if (glArr[i]->on == 0)  
         continue;   // not visible
      if (glArr[i]->layerindex == l)
         return glArr[i]->newName;
   }

   return NULL;
}

/****************************************************************************/
/*
*/
static int load_GENCADsettings(const CString fname, int page_units)
{
   FILE  *fp;
   char  line[127];
   char  *lp;

   fiducialname = "fiducial";
   panelfiducialname = "panelfid";
   testpinprefix = "tstpin_";
   defaultdrillsize = 0.01 * Units_Factor(UNIT_INCHES, page_units);
   defaultnopadsize = 0.01 * Units_Factor(UNIT_INCHES, page_units);
   convert_tp_comps = FALSE;
   testpointshapeprefix = "SHAPE_";
   update_device_attribute = FALSE;
   namespace_allowed = TRUE;
   write_artwork = TRUE;
	write_gen_comp_to_shape = FALSE;
   component_outline_cnt = 0;
   output_graphic_data = FALSE;
   generic_insert = "ignore";
   mechanical_insert = "ignore";
   unknown_insert = "ignore";
	do_normalize_bottom_build_geometries = false;
   treat_mechanical_pins_as_normal_pins = false;
   outputUnit = "USER";
   outputPInt = 1000;
   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"GENCAD Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,127,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) 
         continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".COMPONENT_OUTLINE"))
         {
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue; 
            cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);

            if (component_outline_cnt < 10)
            {
               component_outline_layer[component_outline_cnt] = laytype;
               component_outline_cnt++;
            }
            else
            {
               fprintf(flog, "Too many .COMPONENT_OUTLINE\n");
               display_error++;
            }
         }
         else if (!STRCMPI(lp,".FIDUCIALNAME"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            fiducialname = lp;
         }
         else if (!STRCMPI(lp,".PANELFIDUCIALNAME"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            panelfiducialname = lp;
         }
         else if (!STRCMPI(lp,".TESTPINPREFIX"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            testpinprefix = lp;
         }
         else if (!STRCMPI(lp,".CONVERT_TP_COMPS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               convert_tp_comps = TRUE;
         }
         else if (!STRCMPI(lp,".TREAT_MECHANICAL_PINS_AS_NORMAL_PINS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               treat_mechanical_pins_as_normal_pins = true;
         }
         else if (!STRCMPI(lp,".UPDATE_DEVICE_ATTRIBUTES"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               update_device_attribute = TRUE;
         }
         else if (!STRCMPI(lp,".NAMESPACE_ALLOWED"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'N' || lp[0] == 'n')
               namespace_allowed = FALSE;
         }
         else if (!STRCMPI(lp,".WRITE_ARTWORK"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'N' || lp[0] == 'n')
               write_artwork = FALSE;
         }
         else if (!STRCMPI(lp,".gen_comp_to_shape"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               write_gen_comp_to_shape = TRUE;
         }
         else if (!STRCMPI(lp,".OUTPUT_UNIT_ACCURACY"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            output_units_accuracy = atoi(lp);
         }
         else if (!STRCMPI(lp,".DEFAULTDRILLSIZE"))
         {
            char  c;

            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;   // I = inches M = MM
            c = lp[0];

            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;

            if (c == 'm' || c == 'M')
            {  
               defaultdrillsize = atof(lp) * Units_Factor(UNIT_MM, page_units);
            }
            else
            {
               defaultdrillsize = atof(lp) * Units_Factor(UNIT_INCHES, page_units);
            }
         }
         else if (!STRCMPI(lp,".NO_PADSIZE"))
         {
            char  c;

            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;   // I = inches M = MM
            c = lp[0];

            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;

            if (c == 'm' || c == 'M')
            {  
               defaultnopadsize = atof(lp) * Units_Factor(UNIT_MM, page_units);
            }
            else
            {
               defaultnopadsize = atof(lp) * Units_Factor(UNIT_INCHES, page_units);
            }
         }

         if (!STRCMPI(lp, ".OUTPUT_GRAPHIC_DATA"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               output_graphic_data = TRUE;
         }

         if (!STRCMPI(lp, ".GENERIC_INSERT"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            generic_insert = lp;
         }

         if (!STRCMPI(lp, ".MECHANICAL_INSERT"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            mechanical_insert = lp;
         }

         if (!STRCMPI(lp, ".UNKNOWN_INSERT"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            unknown_insert = lp;
         }
         else if (!STRICMP(lp, ".NORMALIZE_BOTTOM_BUILD_GEOM"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
					continue;
            if (toupper(lp[0]) == 'Y')
               do_normalize_bottom_build_geometries = true;
         }
         else if (!STRICMP(lp, ".OUTPUT_UNITS"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            outputUnit = lp;
            outputUnit.Trim();
            if(!outputUnit.CompareNoCase("USER")
               || !outputUnit.CompareNoCase("USERM")
               || !outputUnit.CompareNoCase("USERMM"))
            {
               if ((lp = get_string(NULL, " \t\n")) == NULL)
                  continue;
               outputPInt = atoi(lp);
            }
         }
      }
   }

/*
   if (update_device_attribute)
   {
      if (ErrorMessage("The .UPDATE_DEVICE_ATTRIBUTE command may change the CAMCAD database.\nDo you want to proceed?", "Update Attribute is not reversable", MB_YESNO | MB_DEFBUTTON2)!=IDYES)
      {
         update_device_attribute = FALSE;
      }
   }
*/
   fclose(fp);
   return 1;
}

/*****************************************************************************/
/*
*/
static int has_blank(const char *l)
{
   CString  tmp;
   tmp  = l;

   if (tmp.Find(" ") > -1)    
      return 1;
   if (tmp.Find("\t") > -1)   
      return 1;

   return 0;
}

/*****************************************************************************/
/*
*/
static int do_layers(FILE *wfp)
{
   LayerStruct *layer;
   const char  *l;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   // could have been deleted.
         continue; 

      if (!layer->isVisible())
         continue;

      if (doc->IsFloatingLayer(layer->getLayerIndex()))
         continue;

      //fprintf(wfp,"DEFINE %s %s\n",layer->getName(), layer->getName());

      l = Layer_Gencad(layer->getLayerIndex());
      if (l == NULL) 
         continue;

      // fixed intername layer
      if (!STRCMPI(l, "SOLDERMASK_TOP"))  
      {
         continue;
      }
      else if (!STRCMPI(l, "SOLDERMASK_BOTTOM"))  
      {
         continue;
      }
      else if (!STRCMPI(l, "SOLDERPASTE_TOP")) 
      {
         continue;
      }
      else if (!STRCMPI(l, "SOLDERPASTE_BOTTOM")) 
      {
         continue;
      }
      else if (!STRCMPI(l, "SOLDERMASK"))   
      {
         fprintf(wfp,"LAYERSET SOLDERMASK\nLAYER SOLDERMASK_TOP\nLAYER SOLDERMASK_BOTTOM\n");
      }
      else if (!STRCMPI(l, "SOLDERPASTE"))  
      {
         fprintf(wfp,"LAYERSET SOLDERPASTE\nLAYER SOLDERPASTE_TOP\nLAYER SOLDERPASTE_BOTTOM\n");
      }
      else if(l && strcmp(layer->getName(),l))   // only write if it is not the same
      {
         if (has_blank(layer->getName()))
            fprintf(wfp,"DEFINE %s \"%s\"\n", l, layer->getName());
         else
            fprintf(wfp,"DEFINE %s %s\n", l, layer->getName());
      }
   }

   fprintf(wfp,"DEFINE SOLDERMASK_TOP SOLDERMASK\n");
   fprintf(wfp,"DEFINE SOLDERMASK_BOTTOM SOLDERMASK\n");
   fprintf(wfp,"DEFINE SOLDERPASTE_TOP SOLDERPASTE\n");
   fprintf(wfp,"DEFINE SOLDERPASTE_BOTTOM SOLDERPASTE\n");

   return 1;
}

//--------------------------------------------------------------
static void write_attributes(FILE *fp,CAttributes* map, 
                                const char *gencadname, int *attcnt )
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;
   int   gencadcnt = *attcnt;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.  
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))              
         continue;

      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))              
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))            
         continue;

      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))         
         continue;

      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0))             
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))           
         continue;

      if (keyword == doc->IsKeyWord(BOARD_THICKNESS, 0))          
         continue;

      if (keyword == doc->IsKeyWord(ATT_FIDUCIAL_PLACE_BOTH, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_DDLINK, 0))  
         continue;

      if (attrib->isInherited()) 
         continue;

      char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));

      if ((tok = strtok(temp,"\n")) == NULL)
      {
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"??\"\n", 
            gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out);  // multiple values are delimited by \n
      }
      else
      {
         while (tok)
         {
            fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
               gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
            tok = strtok(NULL,"\n");
         }
      }

      free(temp);

      // here kill mask and layer
      wGENCAD_Graph_Layer(fp, "");
      wGENCAD_Graph_MaskLevel(fp, "");
   }

   *attcnt = gencadcnt;
   return;
}

/*****************************************************************************/
/*
   If a string contains a blank, it must be inside quotes.
   If a string contains a quote or a \, it must be preceeded by a \
*/
static const char *do_string(const char *t)
{
   static   CString  tmp;
   CString  t1;

   tmp = t;
   tmp.TrimLeft();
   tmp.TrimRight();

   if (tmp.Find("\\\"",0) > -1)
   {
      t1 = "";

      for (int i=0;i<(int)strlen(tmp);i++)
      {
         if (tmp[i] == '\\' || tmp[i] == '"')
            t1 += '\\';
         t1 += tmp[i];
      }

      tmp = t1;
   }

   // find can only do one character at a time
   if (tmp.Find(" ",0) > -1)
   {
      // put in quotes
      t1 = '"';
      t1 += tmp;
      t1 += '"';
      tmp = t1;
   }
   else if (tmp.Find("\t",0) > -1)
   {
      // put in quotes
      t1 = '"';
      t1 += tmp;
      t1 += '"';
      tmp = t1;
   }

   return tmp.GetBuffer(0);
}

/*****************************************************************************/
/*
*/
static int GENCADPinNameCompareFunc( const void *arg1, const void *arg2 )
{
   GENCADCompPinInst **a1, **a2;
   a1 = (GENCADCompPinInst**)arg1;
   a2 = (GENCADCompPinInst**)arg2;

   // order so that drill holes come in last.
   if ((*a1)->drill < (*a2)->drill)
      return -1;
   else if ((*a1)->drill > (*a2)->drill)
      return 1;

   return compare_name((*a1)->pinname, (*a2)->pinname);
}

/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is 
   because of the 2 pass to get artwork info.
*/
static void GENCAD_SortPinData(const char *s)
{
   int   done = FALSE;
   int   sortcnt = 0;

   if (comppininstcnt < 2) 
      return;

   qsort(comppininstarray.GetData(), comppininstcnt, sizeof(GENCADCompPinInst *), GENCADPinNameCompareFunc);
   return;
}

/******************************************************************************
* write_componentattributes
*/
static void write_componentattributes(FILE *fp, CAttributes* map, const CString gencadname, int *attcnt,
                                FileStruct *file, CGenCadDevice* device)
{
   if (map == NULL)
      return;

   int gencadcnt = *attcnt;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
		WORD keyword;
		Attrib* attrib;

      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))     
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))   
         continue;

      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))
         continue;

      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0))    
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))  
         continue;

      if (keyword == doc->IsKeyWord(BOARD_THICKNESS, 0)) 
         continue;

      if (attrib->isInherited()) 
         continue;

		int curPos = 0;
		CString attribValue = get_attvalue_string(doc, attrib);
		CString tok = attribValue.Tokenize("\n", curPos);

      //char* temp = STRDUP(get_attvalue_string(doc, attrib));
      //char* tok = strtok(temp, "\n");
      while (!tok.IsEmpty())
      {
			CString outKeyword = doc->getKeyWordArray()[keyword]->out;
			CString ccKeyword = doc->getKeyWordArray()[keyword]->cc;

         if (!outKeyword.CompareNoCase(ATT_SUBCLASS))
			{
            fprintf(fp,"STYLE %s\n", do_string(tok)); // multiple values are delimited by \n
			}
         else if (!outKeyword.CompareNoCase("TYPE")					||
						!outKeyword.CompareNoCase("PART")					||
						!outKeyword.CompareNoCase("PACKAGE")				||
						!outKeyword.CompareNoCase("DESC")					||
						!outKeyword.CompareNoCase("TOL")						||
						!outKeyword.CompareNoCase(ATT_TOLERANCE)			||
						!outKeyword.CompareNoCase("NTOL")					||
						!outKeyword.CompareNoCase(ATT_MINUSTOLERANCE)	||
						!outKeyword.CompareNoCase("PTOL")					||
						!outKeyword.CompareNoCase(ATT_PLUSTOLERANCE)		||
						!outKeyword.CompareNoCase("VOLTS")					||
						!outKeyword.CompareNoCase(ATT_VOLTAGE)				||
						!outKeyword.CompareNoCase(ATT_VALUE)				)
         {
            fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
               gencadname, ++gencadcnt, outKeyword, tok);

				tok = do_string(tok);

				if (!outKeyword.CompareNoCase(ATT_VALUE))
					device->SetValue(tok);

				if (!outKeyword.CompareNoCase("TOL") || !outKeyword.CompareNoCase(ATT_TOLERANCE))
					device->SetTolerance(tok);

				if (!outKeyword.CompareNoCase("NTOL")|| !outKeyword.CompareNoCase(ATT_MINUSTOLERANCE))
					device->SetMinusTol(tok);

				if (!outKeyword.CompareNoCase("PTOL")|| !outKeyword.CompareNoCase(ATT_PLUSTOLERANCE))
					device->SetPlusTol(tok);

				if (!outKeyword.CompareNoCase(ATT_VALUE))
					device->SetValue(tok);
			}
         else if (outKeyword.CompareNoCase(ATT_DDLINK))
         {
            fprintf(fp, "ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
						gencadname, ++gencadcnt, outKeyword, tok);
         }

			tok = attribValue.Tokenize("\n", curPos); 
         //tok = strtok(NULL, "\n");
      }

      //free(temp);
   } 

   wGENCAD_Graph_Layer(fp, "");
   wGENCAD_Graph_MaskLevel(fp, "");

   *attcnt = gencadcnt;

   return;
}

//--------------------------------------------------------------
static int get_padrot(const char *p, int rot)
{

   for (int i=0;i<padrotcnt;i++)
   {
      if (!strcmp(padrotarray[i]->padname, p) && padrotarray[i]->rotation == rot)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static void write_pinattributes(FILE *fp,CAttributes* map, 
                                const char *gencadname, int *attcnt )
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;
   int   gencadcnt = *attcnt;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))  
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))
         continue;

      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))
         continue;

      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0)) 
         continue;

      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))
         continue;

      if (keyword == doc->IsKeyWord(BOARD_THICKNESS, 0))
         continue;

      if (keyword == doc->IsKeyWord(ATT_DDLINK, 0))  
         continue;

      // do not write the pin attribute, if it was inhereted from the padstack.
      if (attrib->isInherited()) 
         continue;

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %d\n", 
               gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %1.*lf\n", 
               gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out, output_units_accuracy, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            if (attrib->getStringValueIndex() < 0)
               break;

            char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));
            
            tok = strtok(temp,"\n");

            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
                  fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
                     gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
               else
                  fprintf(fp,"ATTRIBUTE %s_%d %s ?\n", 
                     gencadname, ++gencadcnt, doc->getKeyWordArray()[keyword]->out);
               tok = strtok(NULL,"\n");
            }

            free(temp);
         }

         break;
      }

      // here kill mask and layer
      wGENCAD_Graph_Layer(fp, "");
      wGENCAD_Graph_MaskLevel(fp, "");
   }

   *attcnt = gencadcnt;

   return;
}

/*****************************************************************************/
/*
*/
static int write_artworktype(FILE *fp,CAttributes* map, const char *layer, long artworkcnt)
{
   WORD keyword;
   Attrib* attrib;
   int   type_written = FALSE;
   int   out = TRUE;

   if (map != NULL)
   {
      for (POSITION pos = map->GetStartPosition();pos != NULL;)
      {
         map->GetNextAssoc(pos, keyword, attrib);

         // do not write out CAMCAD predefined Keywords.
         if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))  
            continue;

         if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))  
            continue;

         if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0)) 
            continue;

         if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))  
            continue;

         if (keyword == doc->IsKeyWord(ATT_DDLINK, 0))  
            continue;

         if (doc->getKeyWordArray()[keyword]->out == "CUTOUT")
         {
            fprintf(fp,"CUTOUT cutout%ld\n", artworkcnt);
            type_written = TRUE;
         }
         else if (doc->getKeyWordArray()[keyword]->out == "MASK")
         {
            fprintf(fp,"MASK mask%ld\n", artworkcnt);
            type_written = TRUE;
         }
         else
         {
            if (write_artwork)
            {
               wGENCAD_Graph_Artwork(fp, layer,artworkcnt);
               fprintf(fp,"TYPE %s\n", doc->getKeyWordArray()[keyword]->out);
               type_written = TRUE;
            }
            else
               out = FALSE;
         }
      }
   } // if map == NULL

   if (!type_written)
   {
      if (write_artwork)
         wGENCAD_Graph_Artwork(fp, layer,artworkcnt);
      else
         out = FALSE;
   }

   return out;
}

/*****************************************************************************/
/*
*/
static void write_artworktype2(FILE *fp,CAttributes* map)
{
   WORD keyword;
   Attrib* attrib;

   int   type_written = FALSE;

   if (map != NULL)
   {
      for (POSITION pos = map->GetStartPosition();pos != NULL;)
      {
         map->GetNextAssoc(pos, keyword, attrib);

         // do not write out CAMCAD predefined Keywords.
         if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))
            continue;

         if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))  
            continue;

         if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))
            continue;

         if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 0)) 
            continue;

         if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 0))
            continue;

         //a = (Attrib*)voidPtr;
         fprintf(fp,"TYPE %s\n", doc->getKeyWordArray()[keyword]->out);
      }
   } // if map == NULL

   return;
}

/******************************************************************************
* find_rotatedpads
*/
static int find_rotatedpads(double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   
         continue;

      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
         GENCAD_RotatedPadstackData( &(block->getDataList()));
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_padblock(FILE *fp, const char *padname, BlockStruct *block, double scale, 
                          int *padattcnt, int *padstackattcnt, int *padshapeattcnt,
                          double rotation)
{

   // aperture is also used in PADSHAPE
   if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
   {
      double sizeA = block->getSizeA() * scale;
      double sizeB = block->getSizeB() * scale; 
      double xoffset= block->getXoffset() * scale;
      double yoffset= block->getYoffset() * scale;

      switch (block->getShape())
      {
		case apertureRound:
			{
				fprintf(fp,"PAD %s ROUND -1\n",gen_string(padname));
				fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
					output_units_accuracy, xoffset, output_units_accuracy, yoffset, output_units_accuracy, sizeA/2);
			}
         break;

		case apertureSquare:
			{
				fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
				fprintf(fp,"RECTANGLE %1.*lf %1.*lf %1.*lf %1.*lf\n",
					output_units_accuracy, xoffset-sizeA/2, 
					output_units_accuracy, yoffset-sizeA/2, 
					output_units_accuracy, sizeA, 
					output_units_accuracy, sizeA);
			}
         break;

		case apertureRectangle:
         {
            int   r = round(RadToDeg(block->getRotation()+rotation));  // rotation is block->getRotation()

            while (r < 0)     r += 360;
            while (r >= 360)  r -= 360;

            if (r == 0 || r == 90 || r == 180 || r == 270)
            {
               double   sa = sizeA, sb = sizeB, xo = xoffset, yo = yoffset;
               for (int ii=0;ii<r/90;ii++)
               {
                  double tmp = sa;
                  sa = sb;
                  sb = tmp;
                  tmp = xo;
                  xo = yo;
                  yo = tmp;
               }

               // 90 degree round can be done with rectangle
               fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
               fprintf(fp,"RECTANGLE %1.*lf %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, xo-sa/2, 
                  output_units_accuracy, yo-sb/2, 
                  output_units_accuracy, sa, 
                  output_units_accuracy, sb);
            }
            else
            {
               // any other degree is complex
               POSITION polyPos, pntPos;
               CPolyList *polylist;
               CPoly *poly;
               CPnt *pnt;

               polylist = ApertureToPoly_Base(block, 0.0, 0.0, rotation, 0);  // uses block->rot

               if (!polylist)
                  break;

               fprintf(fp,"PAD %s POLYGON -1\n",gen_string(padname));

               // loop thru polys
               polyPos = polylist->GetHeadPosition();
               while (polyPos != NULL)
               {
                  poly = polylist->GetNext(polyPos);
   
                  int first = TRUE;
                  Point2   firstp;
                  Point2   p1,p2;

                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p2.x = pnt->x * scale;
                     p2.y = pnt->y * scale;
                     p2.bulge = pnt->bulge;
                     // here deal with bulge
                     if (first)
                     {
                        firstp = p2;
                        p1 = p2;
                        first = FALSE;
                     }
                     else
                     {
                        wGENCAD_Graph_Line(fp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        p1 = p2;
                     }
                  }
               }
               FreePolyList(polylist);//this function in Polylib.cpp
            }
         }
			break;

		case apertureDonut:
			{
				fprintf(fp,"PAD %s ANNULAR -1\n",gen_string(padname));
				fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
					output_units_accuracy, xoffset, output_units_accuracy, yoffset, output_units_accuracy, sizeA/2);
			}
         break;

		case apertureOctagon:
			{
				fprintf(fp,"PAD %s OCTAGON -1\n",gen_string(padname));
				fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
					output_units_accuracy, xoffset, output_units_accuracy, yoffset, output_units_accuracy, sizeA/2);
			}
         break;

		case apertureOblong:
         {
            POSITION polyPos, pntPos;
            CPolyList *polylist;
            CPoly *poly;
            CPnt *pnt;

            polylist = ApertureToPoly_Base(block, 0.0, 0.0, rotation, 0);  // uses block->rot

            if (!polylist)
               break;

            fprintf(fp,"PAD %s FINGER -1\n",gen_string(padname));

            // loop thru polys
            polyPos = polylist->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = polylist->GetNext(polyPos);
               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  // here deal with bulge
                  if (first)
                  {
                     firstp = p2;
                     p1 = p2;
                     first = FALSE;
                  }
                  else
                  {
                     wGENCAD_Graph_Line(fp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                     p1 = p2;
                  }
               }
            }
            FreePolyList(polylist);//this function in Polylib.cpp
         }
         break;

		case apertureComplex:
         {
            BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
            int apptyp = GENCAD_TestComplexApertureData(fp, &(subblock->getDataList()), 0.0, 0.0, 
               block->getRotation()+rotation, 0, scale, 0);

            if (apptyp == PADSHAPE_CIRCLE)
               fprintf(fp,"PAD %s ROUND -1\n",gen_string(padname));
            else if (apptyp == PADSHAPE_RECTANGULAR)
               fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
            else
               fprintf(fp,"PAD %s POLYGON -1\n",gen_string(padname));

            GENCAD_WriteComplexApertureData(fp, &(subblock->getDataList()), 0.0, 0.0, 
               block->getRotation()+rotation, 0, scale, 0, apptyp);
         }
         break;

      default:
         {
            fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
            fprintf(fp,"RECTANGLE 0.0 0.0 1.0 1.0\n");

				fprintf(flog, "Unknown Aperture [%d] for Geometry [%s]\n", block->getShape(), block->getName());
            display_error++;
         }
         break;
      }

      // attributes
      write_attributes(fp, block->getAttributesRef(), "PAD", padattcnt);
   }
   else if ( (block->getBlockType() == BLOCKTYPE_PADSHAPE))
   {  
		// create necessary aperture blocks
      int apptyp = GENCAD_TestComplexApertureData(fp, &(block->getDataList()), 0.0, 0.0, rotation, 0, scale, 0);

      if (apptyp == PADSHAPE_CIRCLE)
         fprintf(fp,"PAD %s ROUND -1\n",gen_string(padname));
      else if (apptyp == PADSHAPE_RECTANGULAR)
         fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
      else
         fprintf(fp,"PAD %s POLYGON -1\n",gen_string(padname));

      GENCAD_WriteComplexApertureData(fp, &(block->getDataList()), 0.0, 0.0, rotation, 0, scale, 0, apptyp);
      write_attributes(fp, block->getAttributesRef(), "PADSHAPE", padshapeattcnt);
   }
   else if ( (block->getFlags() & BL_TOOL)) // make a dummy pad, used for layer to do a hole in a $SHAPE
   {
      double xoffset= block->getXoffset() * scale;
      double yoffset= block->getYoffset() * scale;
      fprintf(fp,"PAD DRILL_%1.*lf ROUND -1\n",output_units_accuracy, block->getToolSize() * scale);
      fprintf(fp,"CIRCLE %1.*lf %1.*lf %1.*lf\n",
         output_units_accuracy, xoffset, output_units_accuracy, yoffset, 
         output_units_accuracy, block->getToolSize() * scale/2);
   }

   return 1;
}

//--------------------------------------------------------------
static int do_pads(FILE *fp, double scale)
{
   int      padattcnt = 0, padstackattcnt = 0, padshapeattcnt = 0;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;

/* there may be apertures using a width entry.
      if (block->getFlags() & BL_WIDTH)
         continue;
*/
      if (block->getFlags() & BL_FILE)
         continue;

      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
         continue;

      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
         continue;

      // I can never have an unnamed aperute to be part of a pad.
      if (strlen(block->getName()) == 0)             
         continue; 

      // this is done so that I can write rotated PADS which GENCAD can not do.
      // drills are done as single padstacks, so that they can be inserted into a $SHAPE
      CString  pname;
      pname = strlen(block->getOriginalName()) ? block->getOriginalName() : block->getName();

      write_padblock(fp,pname,
         block, scale, &padattcnt, &padstackattcnt, &padshapeattcnt, 0); // do not pass block->getRotation()

      // check here if we need to rotate the pad
      for (int ii=0;ii<padrotcnt;ii++)
      {
         if (padrotarray[ii]->padname.Compare(block->getName()) == 0)
         {
            write_padblock(fp, padrotarray[ii]->newpadname, block, scale, 
               &padattcnt, &padstackattcnt, &padshapeattcnt, DegToRad(padrotarray[ii]->rotation));
         }
      }
   }

   // here check if NO_PAD was written
   if (Graph_Block_Exists(doc, "NO_PAD", -1) == NULL)
   {
      // make one
      fprintf(fp,"PAD %s ROUND -1\n", "NO_PAD");
      fprintf(fp,"CIRCLE 0.0 0.0 %1.*lf\n", output_units_accuracy, defaultnopadsize/2);
   }

   return 1;
}

/******************************************************************************
* do_padstacks
*/
static int do_padstacks(FILE *fp, double scale, CGenCadPadstackMap& padstackMap)
{
   int padstackattcnt = 0;
	POSITION pos = padstackMap.GetStartPosition();
	while (pos != NULL)
	{
		CString padstackName;
		CGenCadPadstack* padstack = NULL;
		padstackMap.GetNextAssoc(pos, padstackName, padstack);
		if (padstack == NULL)
			continue;

		BlockStruct* block = padstack->GetBlock();
		if (block == NULL)
			continue;

		if (padstack->GetIsPadstack())
		{
         double drill = get_drill_from_block_no_scale(doc, block);

			padstackName = gen_string(padstack->GetName());
         fprintf(fp, "PADSTACK %s %1.*lf\n", padstackName, output_units_accuracy, drill * scale);

			// top pad only
         GENCAD_WritePADSTACKData(fp, padstackName, &block->getDataList(), 0.0, 0, scale, 0, padstack->GetInsertLayer(), true, false);
         write_attributes(fp, block->getAttributesRef(), "PADST", &padstackattcnt);
         
			// Write the mirrored version of the padstack if there it needs one
			if (padstack->GetNeedMirVersion())
			{
				padstackName = gen_string(padstack->GetMirName());
				fprintf(fp, "PADSTACK %s %1.*lf\n", padstackName, output_units_accuracy, drill * scale);

				// top pad only
				GENCAD_WritePADSTACKData(fp, padstackName, &block->getDataList(), 0.0, 0, scale, 0, padstack->GetInsertLayer(), true, true);
				write_attributes(fp, block->getAttributesRef(), "PADST", &padstackattcnt);
			}

         // support bondpad rotation
         for (int ii=0; ii < padrotarray.GetCount(); ii++)
         {
            CString padrotName = (padrotarray.GetAt(ii))?padrotarray.GetAt(ii)->padname:"";
            if (!padrotName.Compare(padstackName))
            {
               fprintf(fp, "PADSTACK %s_%d %1.*lf\n", padstackName, padrotarray.GetAt(ii)->rotation, output_units_accuracy, drill * scale);
               GENCAD_WritePADSTACKData(fp, padstackName , &block->getDataList(), DegToRad(padrotarray[ii]->rotation), 0, scale, 0, padstack->GetInsertLayer(), true, false);
               write_attributes(fp, block->getAttributesRef(), "PADST", &padstackattcnt);
            }
         }

		}
		else
		{
			padstackName = gen_string(padstack->GetName());
         fprintf(fp, "PADSTACK %s %1.*lf\n", padstackName, output_units_accuracy, block->getToolSize() * scale);
         fprintf(fp, "PAD DRILL_%1.*lf ALL 0 0\n", output_units_accuracy, block->getToolSize() * scale);
		}
	}

   return 1;
}

// write testpoints into netlist
static int do_netlist_testpoints(FILE *wfp, FileStruct *file, const char *netname)
{
   if (convert_tp_comps)   // if set to FALSE, a via record is generated.
      GENCAD_WriteTESTPOINTData(wfp, &(file->getBlock()->getDataList()), netname);

   return 1;
}

static void write_pcbshapes(FILE *fp, double scale, int artwork_or_shape, long *acnt, int *shapeattcnt,  BlockStruct *block)
{
   if (artwork_or_shape)
      fprintf(fp,"SHAPE %s\n", gen_string(block->getName()));

   comppininstarray.SetSize(100,100);
   comppininstcnt = 0;

   *acnt = GENCAD_WriteSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1,
      artwork_or_shape, *acnt);

   GENCAD_SortPinData(block->getName());

   int   unnamedpin = 0;
   for (int pcnt=0;pcnt<comppininstcnt;pcnt++)
   {
      // no name, this is allowed in drill tools
      if (strlen(comppininstarray[pcnt]->pinname) == 0)
      {
         if (comppininstarray[pcnt]->drill)
            comppininstarray[pcnt]->pinname.Format("HOLE_%d", ++unnamedpin);
         else
            comppininstarray[pcnt]->pinname.Format("NONAME_%d", ++unnamedpin);
      }

      fprintf(fp,"PIN %s ",gen_string(comppininstarray[pcnt]->pinname));
      fprintf(fp," %s %1.*lf %1.*lf %s %1.2lf 0\n",
         gen_string(comppininstarray[pcnt]->padstackname),
         output_units_accuracy, comppininstarray[pcnt]->pinx, 
         output_units_accuracy, comppininstarray[pcnt]->piny, "TOP", 
         RadToDeg(comppininstarray[pcnt]->rotation));
   }

   for (int pcnt=0;pcnt<comppininstcnt;pcnt++)
   {
      delete comppininstarray[pcnt];
   }

   comppininstarray.RemoveAll();

   if (artwork_or_shape)
   {
      Attrib *attrib =  is_attvalue(doc, block->getAttributesRef(), ATT_TECHNOLOGY, 2);
      if (attrib)
		{
#if CamCadMajorMinorVersion > 406  //  > 4.6
         CString value = attrib->getStringValue();
#else
			CString value = doc->ValueArray.GetAt(attrib->getStringValueIndex());
#endif
			if (value.CompareNoCase("SMD") == 0)
				fprintf(fp,"INSERT SMD\n");
			else
				fprintf(fp,"INSERT TH\n");
		}

      double height = -1;
      attrib =  is_attvalue(doc, block->getAttributesRef(), ATT_COMPHEIGHT, 2);

      if (attrib)
      {
         height = atof( get_attvalue_string(doc, attrib));
         if (height > 0)
            fprintf(fp,"HEIGHT %1.*lf\n", output_units_accuracy, height * scale);
      }

      write_attributes(fp, block->getAttributesRef(), "SHAPE", shapeattcnt);
   }
}


//--------------------------------------------------------------
// artwork_or_shape == 0 - first run just to collect artwork record, 
// artwork_or_shape == 1 then write either artwork record or shape outline
//
static int do_pcbshapes(FILE *fp, double scale, int artwork_or_shape, long *acnt)
{
   int      shapeattcnt = 0;
   //int    pcnt;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;
         
      // create necessary aperture blocks
      if ( (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT) ||
           (block->getBlockType() == BLOCKTYPE_TESTPOINT))             
         write_pcbshapes(fp, scale, artwork_or_shape, acnt, &shapeattcnt, block);      
   }

   return shapeattcnt;
}

//--------------------------------------------------------------
static int do_trackcodes(FILE *fp, double scale)
{
   BlockStruct *block;

   for (int i=0;i<doc->getNextWidthIndex();i++)
   {
      block = doc->getWidthTable()[i];

      if (block == NULL)
         continue;

      if (block->getFlags() & BL_WIDTH) 
      {
         if (doc->getWidthTable()[i]->getSizeA() == 0)
            fprintf(fp,"TRACK %d 1\n",i);       // watch out if units are user definable
         else
            fprintf(fp,"TRACK %d %1.*lf\n",i, output_units_accuracy, scale * doc->getWidthTable()[i]->getSizeA());
      }
   }

   return 1;
}

//--------------------------------------------------------------
static int is_component(const char *c)
{
   for (int i=0;i<complistcnt;i++)
   {
      if (complistarray[i]->compname.Compare(c) == 0)
         return i+1;
   }
   return 0;
}

//--------------------------------------------------------------
static int do_signals(FILE *fp, FileStruct *file, CNetList *NetList)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   int      comppinattcnt = 0;
   
   fprintf(fp,"$SIGNALS\n");

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

      fprintf(fp, "SIGNAL %s\n", gen_string(net->getNetName()));
      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();

      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (is_component(compPin->getRefDes())) // check if is was not an converted FIDUCIAL or TOOL
         {
            fprintf(fp, "NODE %s", gen_string(compPin->getRefDes()));
            fprintf(fp, " %s\n", gen_string(compPin->getPinName()));
            write_attributes(fp, compPin->getAttributesRef(), "COMPPIN", &comppinattcnt);
         }
         else // skip comppins
         {
            CString skipReason = "";
            if(compPin->IsDiePin(doc->getCamCadData()))
            {
               skipReason.Format("Skipped CompPin for Die component");
            }
            else //regular comppins
            {
               skipReason.Format("can not find PCB Component");
            }

            fprintf(flog, "SIGNAL: %s NODE %s - %s.\n", 
               gen_string(net->getNetName()), gen_string(compPin->getPinRef()), skipReason);
         }
      }

      // here assign testpoints to the nets
      do_netlist_testpoints(fp,file, net->getNetName());
   }

   fprintf(fp,"$ENDSIGNALS\n\n");

   return 1;
}

/******************************************************************************
* do_routes
*/
static int do_routes(FILE *fp, FileStruct *file, CNetList *NetList, CGenCadPadstackMap& padstackMap)
{   
   fprintf(fp,"$ROUTES\n");

   //GENCAD_GetROUTESData(fp, &(file->getBlock()->getDataList()), 
   //      file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
   //      file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct* net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

      fprintf(fp, "ROUTE %s\n", gen_string(net->getNetName()));
      wGENCAD_Graph_WidthCode(fp,-1);
      wGENCAD_Graph_Layer(fp,"");  

      // order by signal name
      GENCAD_WriteROUTESData(fp,&(file->getBlock()->getDataList()), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor,
				0, -1, net->getNetName(), padstackMap);
   }

   fprintf(fp,"$ENDROUTES\n\n");

   return 1;
}

//--------------------------------------------------------------
static void do_layerlist()
{
   LayerStruct *layer;
   int         ON, signr = 0;
   char        typ = 'D';
   CString     genlay;
   int         unknownlayer = 0;

   for (int j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL) // could have been deleted.
         continue; 

      if (doc->IsFloatingLayer(layer->getLayerIndex()))  // no floating layers, they should never be in a gencad file.
         continue;

      ON = TRUE;
      signr = 0;

      // here check if layer was in mentor.out. If not set it non visible
      genlay = layer->getName();
      typ = 'D';

      if (layer->getLayerType() == LAYTYPE_DIALECTRIC)
         continue;

      if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         genlay = "TOP";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         genlay = "BOTTOM";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         signr = LAY_ALL;
         genlay = "ALL";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         signr = LAY_INNER;
         genlay = "INNER";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_ALL)
      {
         signr = LAY_ALL;
         genlay = "ALL";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_ALL)
      {
         signr = LAY_ALL;
         genlay = "ALL";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         signr = LAY_TOP;
         genlay = "TOP";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         signr = LAY_BOT;
         genlay = "BOTTOM";
         ON = TRUE;
      }
      else if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         genlay.Format("INNER%d",signr-1);
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
      {  // must be after INNER%d
         signr = LAY_INNER;
         genlay = "INNER";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PAD_OUTER)
      {
         signr = LAY_OUTER;
         genlay = "OUTER";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SIGNAL_OUTER)
      {
         signr = LAY_OUTER;
         genlay = "OUTER";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PASTE_ALL)
      {
         signr = 0;
         genlay = "SOLDERPASTE";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PASTE_TOP)
      {
         signr = 0;
         genlay = "SOLDERPASTE_TOP";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM)
      {
         signr = 0;
         genlay = "SOLDERPASTE_BOTTOM";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_MASK_ALL)
      {
         signr = 0;
         genlay = "SOLDERMASK";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_MASK_TOP)
      {
         signr = 0;
         genlay = "SOLDERMASK_TOP";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_MASK_BOTTOM)
      {
         signr = 0;
         genlay = "SOLDERMASK_BOTTOM";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SILK_TOP)
      {
         signr = 0;
         genlay = "SILKSCREEN_TOP";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM)
      {
         signr = 0;
         genlay = "SILKSCREEN_BOTTOM";
         ON = TRUE;
      }
      else if (layer->getLayerType() == LAYTYPE_BOARD_OUTLINE)
      {
         signr = 0;
         genlay = "ALL";
         ON = TRUE;
      }
      else
      {
         if (!doc->get_layer_visible(j, -1))
            continue;

         genlay.Format("LAYER%d", unknownlayer++);
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
            //CString t;
            //t.Format("Electrical Layer [%s] not visible!\nNon Visible Layers will NOT be translated.",layer->getName());
            //MessageBox(NULL, t, "Layer Display", MB_ICONEXCLAMATION | MB_OK);
         }
      }

      //glArr.SetSizes
      GenCADLayerStruct *gl = new GenCADLayerStruct;
      gl->stackNum = signr;
      gl->layerindex = j;
      gl->on = ON;
      gl->type = typ;
      gl->oldName = layer->getName();
      gl->newName = genlay;
      glArr.SetAtGrow(maxArr, gl);
      maxArr++;
   }

   return;
}

//--------------------------------------------------------------
static int edit_layerlist()
{
   // fill array
   GenCADLayerDlg gldlg;

   gldlg.arr = &glArr;
   gldlg.maxArr = maxArr;

   if (gldlg.DoModal() != IDOK) 
      return FALSE;

   return TRUE;
}

//--------------------------------------------------------------
static void free_layerlist()
{
   for (int i=0;i<maxArr;i++)
   {
      delete glArr[i];
   }

   glArr.RemoveAll();
   maxArr = 0;

   return;
}

/****************************************************************************/
/*
*/
static int get_padstack(const char* p)
{
   //for (int i=0;i<padstackcnt;i++)
   //{
   //   if (!STRCMPI(padstackarray[i]->padname, p))
   //      return i;
   //}

   return -1;
}

/******************************************************************************
* find_testpoints_used_as_comp
*/
static void find_testpoints_used_as_comp(CDataList *DataList)
{
  // POSITION pos = DataList->GetHeadPosition();
  // while (pos != NULL)
  // {
  //    DataStruct* data = DataList->GetNext(pos);
		//if (data->getDataType() != dataTypeInsert)
		//	continue;
  //    
  //    if (data->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
  //    {
  //       BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
  //       if (block->getBlockType() == BLOCKTYPE_PADSTACK)
  //       {
  //          int pptr = get_padstack(block->getName());
  //          if (pptr > -1)
  //             padstackarray[pptr]->used_as_testcomp = TRUE;
  //       }
  //    }
  // }
  // return;
}

//--------------------------------------------------------------
// make sure that testpoints have a ATT_NETNAME attribute assigned, just like vias
static void attach_testpoints_netlist(CDataList *DataList, CNetList *NetList)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)     
         continue;

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities

            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
            {
               CString  refname;
               refname = np->getInsert()->getRefname();

               if (strlen(refname) == 0)
                  break;

               // find if it has pins in the netlist
               POSITION netPos = NetList->GetHeadPosition();
               while (netPos != NULL)
               {
                  NetStruct *net = NetList->GetNext(netPos);

                  if (net->getFlags() & NETFLAG_UNUSEDNET)
                     continue;

                  POSITION compPinPos = net->getHeadCompPinPosition();
                  while (compPinPos != NULL)
                  {
                     CompPinStruct *compPin = net->getNextCompPin(compPinPos);

                     if (compPin->getRefDes().Compare(refname) == 0)
                     {
                        CString netName = net->getNetName();

                        doc->SetAttrib(&np->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
                           VT_STRING,
                           netName.GetBuffer(0), SA_OVERWRITE, NULL); // x, y, rot, height

                        break;   // only 1 netname is allowed
                     }
                  }
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end attach_testpoints_netlist */
   return;
}

//--------------------------------------------------------------
void GENCAD_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format,
                       int PageUnits, BOOL run_silent = TRUE)
{
   FILE *wfp;
   FileStruct *file;

   int   pcb_found = FALSE;
   int   panel_found = FALSE;
   doc = Doc;
   display_error = FALSE;
   viacnt = 0;
   testpadcnt = 0;

   routesarray.SetSize(100,100);
   routescnt = 0;

   padrotarray.SetSize(10,10);
   padrotcnt = 0;

   complistarray.SetSize(100,100);
   complistcnt = 0;

   cur_artworkcnt = 0;

   glArr.SetSize(10,10);
   maxArr = 0;

   CString logFile = GetLogfilePath("GENCAD.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   
   output_units_accuracy = GetDecimals(PageUnits); 

   CString settingsFile( getApp().getExportSettingsFilePath("gencad.out") );
   load_GENCADsettings(settingsFile, PageUnits);
   unitsFactor = GetUnitsFactor(format->Scale,PageUnits);
   defaultdrillsize *= unitsFactor;
   defaultnopadsize *= unitsFactor;

   switch_on_pcbfiles_used_in_panels(doc, TRUE);   // set show

	if (do_normalize_bottom_build_geometries)
		doc->OnToolsNormalizeBottomBuildGeometries();

   do_layerlist();

   if (run_silent || edit_layerlist())
   {
      CWaitCursor wait;

      // open file for writting
      if ((wfp = fopen(filename, "wt")) == NULL)
      {
         // error - can not open the file.
         CString  tmp;
         tmp.Format("Can not open %s",filename);
         MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
         return;
      }

      wGENCAD_Graph_Init_Layout(output_units_accuracy);
      wGENCAD_Graph_File_Open_Layout();

      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);

         if (!file->isShown())
            continue;

         if (file->getBlockType() == BLOCKTYPE_PCB)  
         {
            generate_PINLOC(doc,file,0);     // this function generates the PINLOC argument for all pins.
            generate_PADSTACKACCESSFLAG(doc, 0);   // 

            int headerattcnt = 0;
            pcb_found = TRUE;

				// header
            wGENCAD_Graph_Header(wfp,"GENCAD", 1.4, file->getName(),GetUnitsString());
            write_attributes(wfp, file->getBlock()->getAttributesRef(), "HEADER", &headerattcnt);
            fprintf(wfp,"$ENDHEADER\n\n");

            // board
            fprintf(wfp,"$BOARD\n");
            Attrib *attrib =  is_attvalue(doc, file->getBlock()->getAttributesRef(), BOARD_THICKNESS, 2);
            if (attrib)
            {
               double thick = -1;
               thick = atof( get_attvalue_string(doc, attrib));

               if (thick > 0)
                  fprintf(wfp,"THICKNESS %1.3lf\n", thick * file->getScale() * unitsFactor);
            }

            // all graphic structures 
            GENCAD_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
						file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
						file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
						graphicClassBoardOutline);

            // here all board cutout
            GENCAD_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
						file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
						file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
						graphicClassBoardCutout);

            GENCAD_WriteBOARDData(wfp, &(file->getBlock()->getDataList()), 
						file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
						file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
						LAYTYPE_BOARD_OUTLINE);

            fprintf(wfp,"$ENDBOARD\n\n");


				CGenCadPadstackMap padstackMap;
				GatherPadstacksAndRoutesData(doc, file, padstackMap);
				
				//// here need to get rotated padstacks, so that they can be written in the $PADS section
    //        find_rotatedpads(file->getScale() * unitsFactor);

            // pads
            // loop thru blocks marked as PADS, PADSTACK or PADSTACKGRAPHIC
            fprintf(wfp,"$PADS\n");
            do_pads(wfp,file->getScale() * unitsFactor);
            fprintf(wfp,"$ENDPADS\n\n");

            // padstack
            fprintf(wfp,"$PADSTACKS\n");
            do_padstacks(wfp,file->getScale() * unitsFactor, padstackMap);
            fprintf(wfp,"$ENDPADSTACKS\n\n");

            // artworks - library on shapes, which are not electrical components
            fprintf(wfp,"$ARTWORKS\n");
            long acnt = cur_artworkcnt;  //cur_artworkcnt needs to be same when starting $SHAPES section
            if (write_artwork)
            {              
               int sattcnt = do_pcbshapes(wfp, file->getScale() * unitsFactor, 0, &acnt);
               write_mech_shape(wfp, file->getScale() * unitsFactor, &(file->getBlock()->getDataList()), &acnt, &sattcnt, 0);
            }
            fprintf(wfp,"$ENDARTWORKS\n\n");
         
            // shapes - only PCB shapes
            fprintf(wfp,"$SHAPES\n");
            int sattcnt = do_pcbshapes(wfp, file->getScale() * unitsFactor, 1, &cur_artworkcnt);  // now start increment cur_artworkcnt
            write_mech_shape(wfp, file->getScale() * unitsFactor, &(file->getBlock()->getDataList()), &cur_artworkcnt, &sattcnt, 1);
            if (convert_tp_comps)
            {
	            // here need to do something if a testpoint is convert_tp_comps
//               find_testpoints_used_as_comp(&(file->getBlock()->getDataList()));
               //for (int i=0;i<padstackcnt;i++)
               //{
               //   if (padstackarray[i]->used_as_testcomp)
               //   {
               //      fprintf(wfp, "SHAPE %s%s\n", testpointshapeprefix, padstackarray[i]->padname);
               //      fprintf(wfp, "PIN 1 %s 0 0 TOP 0.0 0\n", padstackarray[i]->padname);
               //      fprintf(wfp,"ATTRIBUTE SHAPE_%d \"%s\" \"%s\"\n", ++sattcnt, "USERSHAPE","SHAPE created from PADSTACK"); 
               //   }
               //}

					POSITION pos = padstackMap.GetStartPosition();
					while (pos != NULL)
					{
						CString padstackName;
						CGenCadPadstack* padstack = NULL;
						padstackMap.GetNextAssoc(pos, padstackName, padstack);
						if (padstack == NULL || !padstack->GetUseAsTestComp())
							continue;

						padstackName = gen_string(padstackName);
                  fprintf(wfp, "SHAPE %s%s\n", testpointshapeprefix, padstackName);
                  fprintf(wfp, "PIN 1 %s 0 0 TOP 0.0 0\n", padstackName);
                  fprintf(wfp,"ATTRIBUTE SHAPE_%d \"%s\" \"%s\"\n", ++sattcnt, "USERSHAPE","SHAPE created from PADSTACK"); 
					}
            }
            else
            {
               // make sure that testpoints have a ATT_NETNAME attribute assigned, just like vias
               attach_testpoints_netlist(&(file->getBlock()->getDataList()), &file->getNetList());
            }
            fprintf(wfp,"$ENDSHAPES\n\n");

            // components - only electrical components
            fprintf(wfp,"$COMPONENTS\n");
				CGenCadDeviceMap deviceMap;
            GENCAD_WriteCOMPONENTData(wfp, file, file->getBlock(), 
						file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
						file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor,
						0, -1, deviceMap);
            fprintf(wfp,"$ENDCOMPONENTS\n\n");
				deviceMap.WriteDevice(wfp);

            // signals
            do_signals(wfp, file, &file->getNetList());

            // tracks
            fprintf(wfp,"$TRACKS\n");
            do_trackcodes(wfp, unitsFactor);
            fprintf(wfp,"$ENDTRACKS\n\n");

            // layers
            fprintf(wfp,"$LAYERS\n");
            do_layers(wfp);
            fprintf(wfp,"$ENDLAYERS\n\n");

            // routes
            do_routes(wfp, file, &file->getNetList(), padstackMap);

            // mech - here place all non electrical shapes
            fprintf(wfp,"$MECH\n");
            GENCAD_WriteMECHData(wfp,&(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);  // not a panel
            fprintf(wfp,"$ENDMECH\n\n");

            // testpins            
            fprintf(wfp,"$TESTPINS\n");
            GENCAD_WriteTestProbeData(wfp,&(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);  // not a panel
            fprintf(wfp,"$ENDTESTPINS\n\n");

            fprintf(wfp,"$POWERPINS\n");
            fprintf(wfp,"$ENDPOWERPINS\n\n");

				break; // do only one pcb file
         }
      }

      wGENCAD_Graph_File_Close_Layout();

      // close write file
      fclose(wfp);

      if (!pcb_found)
      {
         MessageBox(NULL, "No PCB file found !","Gencad Output", MB_OK | MB_ICONHAND);
         return;
      }

      switch_on_pcbfiles_used_in_panels(doc, FALSE);  // set show

      if (!format->CreatePanelFile)
         return ;

      // here check it there is a panel file also
      // open file for writting
      char f[_MAX_PATH];
      char drive[_MAX_DRIVE];
      char dir[_MAX_DIR];
      char fname[_MAX_FNAME];
      char ext[_MAX_EXT];
   
      // need to do this, because panel only knows the path.ext
      _splitpath( filename, drive, dir, fname, ext );
      strcpy(f,drive);
      strcat(f,dir);
      strcat(f,fname);
      strcat(f,".pnl");
   
      if ((wfp = fopen(f, "wt")) == NULL)
      {
         // error - can not open the file.
         CString  tmp;
         tmp.Format("Can not open %s",f);
         MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
         return;
      }

      // reset status for Panel file 
      progress->SetStatus(f);

      wGENCAD_Graph_Init_Layout(output_units_accuracy);
      wGENCAD_Graph_File_Open_Layout();

      pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         file = doc->getFileList().GetNext(pos);
         if (file->getBlockType() == BLOCKTYPE_PANEL)   
         {
            if (!file->isShown())
               continue;

            panel_found = TRUE;
            int   panelattcnt = 0;

            // header
            wGENCAD_Graph_Header(wfp, "GENPANEL", 1.4, file->getName(),GetUnitsString());

            // here all attributes
            write_attributes(wfp, file->getBlock()->getAttributesRef(), "PANEL", &panelattcnt);
            fprintf(wfp,"$ENDHEADER\n\n");

            // panel
            // board
            fprintf(wfp,"$PANEL\n");

            // all graphic structures 
            GENCAD_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
               graphicClassPanelOutline);

            // all graphic structures 
            GENCAD_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
               graphicClassPanelCutout);

            // other graphic structure
            GENCAD_WriteBOARDData(wfp, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 0);
            fprintf(wfp,"$ENDPANEL\n\n");

            // artworks
            fprintf(wfp,"$ARTWORKS\n");
            fprintf(wfp,"$ENDARTWORKS\n\n");

            // mech
            fprintf(wfp,"$MECH\n");
            GENCAD_WriteMECHData(wfp, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, TRUE, -1);  // panel file
            fprintf(wfp,"$ENDMECH\n\n");

            // boards
            fprintf(wfp,"$BOARDS\n");
            GENCAD_WritePANELBOARDSData(wfp,&(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
               fname,ext);

            fprintf(wfp,"$ENDBOARDS\n\n");
         }
      }

      if (!panel_found)
      {
         // create an empty panel
         panel_found = TRUE;
         // header
         wGENCAD_Graph_Header(wfp, "GENPANEL", 1.4, file->getName(),GetUnitsString());
         fprintf(wfp,"$ENDHEADER\n\n");
         // panel
         // board
         fprintf(wfp,"$PANEL\n");
         // create a dummy panel with just this board in there.
         fprintf(wfp,"$ENDPANEL\n\n");
            // artworks
         fprintf(wfp,"$ARTWORKS\n");
         fprintf(wfp,"$ENDARTWORKS\n\n");

         // mech
         fprintf(wfp,"$MECH\n");
         fprintf(wfp,"$ENDMECH\n\n");
         // boards
         fprintf(wfp,"$BOARDS\n");
         fprintf(wfp,"BOARD BOARD1\n");
         fprintf(wfp,"FILE %s%s\n", fname, ext);
         fprintf(wfp,"FIDUCIAL 0.0 0.0\n");
         fprintf(wfp,"ROTATION 0.0\n");
         fprintf(wfp,"PLACE 0.0 0.0\n");

         fprintf(wfp,"$ENDBOARDS\n\n");
      }

      wGENCAD_Graph_File_Close_Layout();
      // close write file
      fclose(wfp);

   } // canceled from layer edit

   free_layerlist();
   fclose(flog);

   //for (int i=0;i<padstackcnt;i++)
   //{
   //   delete padstackarray[i];
   //}
   //padstackarray.RemoveAll();
   //padstackcnt = 0;

	int i=0;
   for (i=0;i<routescnt;i++)
   {
      delete routesarray[i];
   }
   routesarray.RemoveAll();
   routescnt = 0;

   for (i=0;i<padrotcnt;i++)
   {
      delete padrotarray[i];
   }
   padrotarray.RemoveAll();

   for (i=0;i<complistcnt;i++)
   {
      delete complistarray[i];
   }
   complistarray.RemoveAll();

   if (display_error)
      Logreader(logFile);

   return;
}

/*****************************************************************************/
/*
*/
static int ignore_non_manufacturing_info(DataStruct *np)
{

   if (np->getDataType() != T_INSERT)
   {
      if (np->getGraphicClass() == GR_CLASS_ANTI_ETCH)    
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)   
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)  
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)     
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT)    
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPIN)       
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)      
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPIN)       
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_ALLKEEPOUT)      
         return TRUE;
      if (np->getGraphicClass() == GR_CLASS_UNROUTE)         
         return TRUE;
      if (np->getGraphicClass() == graphicClassTestPointKeepOut)
         return TRUE;
      if (np->getGraphicClass() == graphicClassTestPointKeepIn) 
         return TRUE;
      if (np->getGraphicClass() == graphicClassPlacementRegion)
         return TRUE;
   }
   else
   {
      if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD) 
         return TRUE;
      if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)
         return TRUE;
      if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE) 
         return TRUE;

   }

   return FALSE;
}

//--------------------------------------------------------------
void GENCAD_WriteBOARDData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int layertype)
{
   Mat2x2 m;
   DataStruct *np;
   int layer;
   const char  *l;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // this is done in PRIMARY...
      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
            continue;
         if (np->getGraphicClass() == GR_CLASS_PANELOUTLINE)
            continue;
         if (np->getGraphicClass() == graphicClassBoardCutout)
            continue;
         if (np->getGraphicClass() == graphicClassPanelCutout)
            continue;
      }

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // If segment has a netname, do not write
         // here check if ATTR_NETNAME == netname
         if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))
            continue;

         if ((l = Layer_Gencad(layer)) == NULL) 
            continue;

         LayerStruct *ll = doc->FindLayer(layer);

      // if (layertype && ll->getLayerType() == layertype) // only check for class if the layertype is not equal
            //continue; // can done in PrimaryBoard
      }

      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden())
                  continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               // do not write artwork
               if (!write_artworktype(wfp, np->getAttributesRef(), l,++cur_artworkcnt))  // attrib from start of poly
                  break;
               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

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
                     wGENCAD_Graph_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                     p1 = p2;
                  }
               }
            }  // while
            //wGENCAD_Graph_Rectangle( points[0].x, points[0].y, points[2].x, points[2].y);
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         {
            if (output_graphic_data)
            {
               Point2 point2, ULPoint;
               CString multiLineStr = np->getText()->getText();
               FilterIllegalChar(multiLineStr);
               CSupString supString(multiLineStr);
               CStringArray tokenArray;

               supString.Parse(tokenArray, "\n", false);

               double space_ratio = doc->getSettings().getTextSpaceRatio();
               double textHeight = np->getText()->getHeight();
               double text_rot = rotation + np->getText()->getRotation();
               RotMat2(&m, text_rot);

               int text_mirror;
               // text is mirrored if mirror is set or text.mirror but not if none or both
               text_mirror = np->getText()->isMirrored();
               fprintf(wfp,"ARTWORK artwork%ld %s\n",++cur_artworkcnt,l);

               CExtent textBoxExtent = np->getText()->getTextBox(space_ratio);

               // Upper left corner of unrotated text block extent.
               // Origin of text block extent is already in terms of horizontal and vertical position modes.
               Point2 unrotatedUpperLeftCorner(textBoxExtent.getUL().x, textBoxExtent.getUL().y);

               // Rotate corner to actual position, still relative to text insert point (not relative to overall CCZ origin)
               Point2 rotatedUpperLeftCorner(unrotatedUpperLeftCorner);  // starts out not rotated
               TransPoint2(&rotatedUpperLeftCorner, 1, &m, 0., 0.);      // now rotate it

               /* -- not used --
               // Actual upper left corner, relative to parent origin
               double actualCornerX = np->getText()->getPnt().x + rotatedUpperLeftCorner.x;
               double actualCornerY = np->getText()->getPnt().y + rotatedUpperLeftCorner.y;
               */

               // Actual first line "Baseline" coords
               Point2 rotatedBaselineCorner(unrotatedUpperLeftCorner); // starts out not rotated
               rotatedBaselineCorner.y -= textHeight;                  // adjust for text height
               TransPoint2(&rotatedBaselineCorner, 1, &m, 0., 0.);     // now rotate it
               // The corner point rotatedBaselineCorner is relative to text origin.
               // The text origin is what is placed at getPnt().x and y.
               // Therefore to get actual baseline corner just add rotatedBaselineCorner to insert location (which is text origin)
               double actualBaselineCornerX = np->getText()->getPnt().x + rotatedBaselineCorner.x;
               double actualBaselineCornerY = np->getText()->getPnt().y + rotatedBaselineCorner.y;
               double textoffsetx = (insert_x + actualBaselineCornerX) * scale;
               double textoffsety = (insert_y + actualBaselineCornerY) * scale;

               // Get and write each line as separate TEXT entry
               for (int i = 0; i < tokenArray.GetCount(); i++)
               {
                  CString text = tokenArray.GetAt(i);
                  
                  point2.x = 0.0;
                  point2.y = (0.0 - (np->getText()->getHeight() * i * ( 1 + space_ratio ))) * scale;
                  TransPoint2(&point2, 1, &m, textoffsetx, textoffsety);

                  int text_mirror = np->getText()->isMirrored();

                  wGENCAD_Graph_Text(wfp, text.GetBuffer(), point2.x, point2.y,
                     np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                     (text_mirror)?(text_rot - PI):text_rot, text_mirror, l);
               }

               //write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt); 
            }
            /*if (output_graphic_data)
               GENCAD_WriteARTWORKData(wfp, DataList,insert_x,insert_y,rotation,
                  mirror, scale,0,layer); */
         }
         break;
      case T_INSERT:
         {
            if ((np->getInsert()->getInsertType() == INSERTTYPE_GENERICCOMPONENT && !generic_insert.CompareNoCase("Artwork")) ||
               (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT && !mechanical_insert.CompareNoCase("Artwork")) ||
               (np->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN && !unknown_insert.CompareNoCase("Artwork")))
            {
               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
               
               GENCAD_WriteARTWORKData(wfp, &block->getDataList(),np->getInsert()->getOriginX()*scale,np->getInsert()->getOriginY()*scale,np->getInsert()->getAngle(),
                  np->getInsert()->getMirrorFlags(), scale,0,np->getLayerIndex());            
            }
         }
         break;
      } // end switch
   } // end GENCAD_WriteBOARDData */
}

//--------------------------------------------------------------
void GENCAD_WritePRIMARYBOARDData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, 
      GraphicClassTag graphic_class)
{
   int cutoutcnt = 0;

   Mat2x2  m;
   Point2 point2;
   RotMat2(&m, rotation);

	bool hasOutlineGrClass = false;

	if (graphic_class == graphicClassBoardOutline || graphic_class == graphicClassPanelOutline)
	{
		// If writing BoardOutline or PanelOutline then, then check to see if there is any poly of that type
		// If there is, then set hasOutlineGrClass to true

		POSITION pos = DataList->GetHeadPosition();
		while (pos != NULL)
		{
			DataStruct *data = DataList->GetNext(pos);

			if (data->getDataType() != dataTypePoly)
				continue;

			int layer;

			if (insertLayer != -1)
				layer = insertLayer;
			else
				layer = data->getLayerIndex();

			// insert has not always a layer definition.
			if (!doc->get_layer_visible(layer, mirror))
				continue;

			// If segment has a netname, do not write
			// here check if ATTR_NETNAME == netname
			if (is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1))
				continue;

			if (Layer_Gencad(layer) == NULL) 
				continue;

			if (data->getGraphicClass() == graphic_class)
			{
				hasOutlineGrClass = true;
				break;
			}
		}
	}


   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
      {
			int layer;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         // If segment has a netname, do not write
         // here check if ATTR_NETNAME == netname
         if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))
            continue;

         if (Layer_Gencad(layer) == NULL) 
            continue;

			if (graphic_class == graphicClassBoardOutline || graphic_class == graphicClassPanelOutline)
			{
				// If writing BoardOutline or PanelOutline then
				//   1) If hasOutlineGrClass is true, then only write out that poly of that graphic class
				//   2) If hasOutlineGrClass is false, then write poly on the layer type specified

				if (hasOutlineGrClass == false)
				{
					LayerStruct *ll = doc->FindLayer(layer);

					if (graphic_class == graphicClassBoardOutline && ll->getLayerType() != layerTypeBoardOutline)
						continue;
					else if (graphic_class == graphicClassPanelOutline && ll->getLayerType() != layerTypePanelOutline)
						continue;
				}
				else if (np->getGraphicClass() != graphic_class)
				{
					continue;
				}
			}
         else if (np->getGraphicClass() != graphic_class)
			{
            continue;
			}
      }

      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            if (np->getGraphicClass() == graphicClassBoardCutout || 
                np->getGraphicClass() == graphicClassPanelCutout)
            {
               fprintf(wfp,"CUTOUT cutout_%d\n",++cutoutcnt);
            }

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden())
                  continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;
               double   cx, cy, radius;
               double   llx, lly, uux, uuy;

					if (poly->isVoid())
						fprintf(wfp,"CUTOUT cutout_%d\n",++cutoutcnt);

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  wGENCAD_Graph_Circle(wfp, cx, cy, radius );
               }
               else if (PolyIsRectangle(poly, &llx, &lly, &uux, &uuy))
               {
                  Point2   p[5];
                  int      cnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     if (cnt < 5)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        p[cnt].x = pnt->x * scale;
                        if (mirror) p[cnt].x = -p[cnt].x;
                        p[cnt].y = pnt->y * scale;
                        p[cnt].bulge = pnt->bulge;
                        TransPoint2(&p[cnt], 1, &m, insert_x, insert_y);
                        cnt++;
                     }
                     else
                     {
                        MessageBox(NULL, "Marked as Rectangle is wrong !","Gencad Output", MB_OK | MB_ICONHAND);
                        break;
                     }
                  }

                  // here rectangle
                  wGENCAD_Graph_Rectangle(wfp, p[0].x, p[0].y, p[2].x, p[2].y);
               }
               else
               {  // not rectangle
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
                        wGENCAD_Graph_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        p1 = p2;
                     }
                  }
               } // not rectangle
            }  // while
         }
         break;  // POLYSTRUCT

      case T_TEXT:
            /* No Text is Board section */
         break;
      case T_INSERT:
         {
            // do only unknown types, not pcbcomponents or other
            if (np->getInsert()->getInsertType())
               break;

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

            int block_layer = -1;

            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            // may boardoutline is part of a insert
            GENCAD_WritePRIMARYBOARDData(wfp,&(block->getDataList()), point2.x, point2.y,
               block_rot, block_mirror, scale * np->getInsert()->getScale(), embeddedLevel+1, 
               block_layer, graphic_class);
         }
         break;
      }
   } 
}

//--------------------------------------------------------------
void GENCAD_WriteARTWORKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer;
   int   artworkattcnt = 0;
   //const  char  *l;
   LayerStruct *lay = NULL;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if ((np->getFlags() & GR_CLASS_BOARD_GEOM) == GR_CLASS_BOARD_GEOM)
            continue;

         if ((np->getFlags() & GR_CLASS_ETCH)       == GR_CLASS_ETCH)
            continue;

         layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         lay = doc->FindLayer(layer);
         
         if (lay == NULL)  
            continue;

      }

      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (poly->isHidden())
                  continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               wGENCAD_Graph_MaskLevel(wfp, lay->getName());
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
                     wGENCAD_Graph_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                     // must be written after every line
                     write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
                     p1 = p2;
                  }
               }
            }  // while
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();
            wGENCAD_Graph_MaskLevel(wfp, lay->getName());
            wGENCAD_Graph_Text(wfp, np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror, lay->getName());
            write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
         }  
         break;
          
      case T_INSERT:
         {  
            int geomnum = np->getInsert()->getBlockNumber();
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            double block_rot = rotation + np->getInsert()->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());

            point2.x = (np->getInsert()->getOriginX())*scale;
            point2.y = (np->getInsert()->getOriginY())*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)   
            {
               wGENCAD_Graph_Circle(wfp, point2.x, point2.y, block->getToolSize()/2*scale);
               write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
            }
            else if( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               if (block->getShape() == T_COMPLEX)
               {
                  int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  int block_layer = -1;

                  int apptyp = GENCAD_TestComplexApertureData(wfp, &(subblock->getDataList()), 0.0, 0.0, 
                        block->getRotation()+rotation, 0, scale, 0);

                  GENCAD_WriteComplexApertureData(wfp,&(subblock->getDataList()), 
                               np->getInsert()->getOriginX()*scale, np->getInsert()->getOriginY()*scale,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               0, apptyp);
               }           
               else
               {
                  wGENCAD_Graph_Aperture(wfp, np->getInsert()->getOriginX()*scale, np->getInsert()->getOriginY()*scale,
                        block->getSizeA() * scale * np->getInsert()->getScale(),
                        block->getSizeB() * scale * np->getInsert()->getScale(),
                        point2.x, point2.y,                      
                        block->getShape(), block_rot);  
               }

               write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
            }
            else
            {              
               GENCAD_WriteARTWORKData(wfp, &block->getDataList(),point2.x, point2.y,block_rot,
                     mirror, scale,0,np->getLayerIndex());   
            }
         }

         break;
      } // end switch
   } // end GENCAD_WriteARTWORKData */
}

//--------------------------------------------------------------
static BlockStruct *get_blockpadname(CDataList *b)
{
   DataStruct *np;
   BlockStruct *bb = NULL;

   POSITION pos = b->GetHeadPosition();
   while (pos != NULL)
   {
      np = b->GetNext(pos);
      
      if (np->getDataType() == T_INSERT)        
      {
         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if (block->getBlockType() == BLOCKTYPE_PADSTACK || 
             block->getBlockType() == BLOCKTYPE_PADSHAPE)
         {
            return block;
         }
         else
         {
            bb = get_blockpadname(&(block->getDataList()));
         }
      }
   }
   return bb;
}

//--------------------------------------------------------------
static int in_component_outline(int ltype)
{
   int   i;

   for (i=0;i<component_outline_cnt;i++)
   {
      if (component_outline_layer[i] == ltype)
         return 1;
   }

   return 0;
}


//--------------------------------------------------------------
long GENCAD_WriteSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer,
      int artwork_or_shape, long acnt, int from_generic_component)
{
   Point2   point2;
   DataStruct *np;
   Mat2x2   m;
   int      layer;
   int      pinattcnt = 0;
   int      fidcnt = 0;
   int      fiducialattcnt = 0;
   const    char* l;
   LayerStruct *ll;

   wGENCAD_Graph_WidthCode(wfp, -1);
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if ((l = Layer_Gencad(layer)) == NULL)
            continue;

         ll = doc->FindLayer(layer);

      }

      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) 
                  continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               int first = TRUE;
               Point2   firstp;
               Point2   p1,p2;

               if (artwork_or_shape)   // just collect the artwork section
               {
                  if (np->getGraphicClass() == GR_CLASS_COMPOUTLINE || in_component_outline(ll->getLayerType()) || (from_generic_component && write_gen_comp_to_shape))
                  {
                     double cx, cy, radius;
                     double llx, lly, uux, uuy;

                     if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
                     {
                        Point2   p;
                        p.x = cx * scale;
                        if (mirror) p.x = -p.x;
                        p.y = cy * scale;
                        radius = radius * scale;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);
                        wGENCAD_Graph_Circle(wfp, p.x, p.y, radius );
                     }
                     else if (PolyIsRectangle(poly, &llx, &lly, &uux, &uuy))
                     {
                        Point2   p[5];
                        int      cnt = 0;
                        pntPos = poly->getPntList().GetHeadPosition();
                        while (pntPos != NULL)
                        {
                           if (cnt < 5)
                           {
                              pnt = poly->getPntList().GetNext(pntPos);
                              p[cnt].x = pnt->x * scale;
                              if (mirror) p[cnt].x = -p[cnt].x;
                              p[cnt].y = pnt->y * scale;
                              p[cnt].bulge = pnt->bulge;
                              TransPoint2(&p[cnt], 1, &m, insert_x, insert_y);
                              cnt++;
                           }
                           else
                           {
                              MessageBox(NULL, "Marked as Rectangle is wrong !","Gencad Output", MB_OK | MB_ICONHAND);
                              break;
                           }
                        }

                        // here rectangle
                        wGENCAD_Graph_Rectangle(wfp, p[0].x, p[0].y, p[2].x, p[2].y);
                     }
                     else
                     {  // not rectangle
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
                              wGENCAD_Graph_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                              p1 = p2;
                           }
                        }
                     } // is rectangle
                  }  // while
                  else
                  {
                     if (write_artwork)                  
                        fprintf(wfp,"ARTWORK artwork%ld 0 0 0 0\n",++acnt);
                  }
               }
               else
               {
                  // will be done in next run
                  if (np->getGraphicClass() == GR_CLASS_COMPOUTLINE)
                     break;
                  if (in_component_outline(ll->getLayerType()))     
                     break;
                  if (!write_artwork)                          
                     break;

                  wGENCAD_Graph_Artwork1(wfp, l,++acnt);
                  write_artworktype2(wfp, np->getAttributesRef());  // attrib from start of poly
                  wGENCAD_Graph_WidthCode(wfp, -1);   // reset
                  wGENCAD_Graph_WidthCode(wfp, poly->getWidthIndex());

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
                        wGENCAD_Graph_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                        p1 = p2;
                     }
                  }
               }  // while
            } // if artwork of shape
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();   

            if (artwork_or_shape)
            {
               /* no text in shape allowed, only in artwork */
               if (write_artwork)
                  fprintf(wfp,"ARTWORK artwork%ld 0 0 0 0\n",++acnt);
            }
            else
            {
               if (write_artwork)
               {
                  wGENCAD_Graph_Artwork1(wfp, l,++acnt);
                  wGENCAD_Graph_Text(wfp, np->getText()->getText(), point2.x, point2.y,
                              np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                              text_rot, text_mirror, l);
               }
            }
         }  
         break;
         
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

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
            {
               if (artwork_or_shape)
               {
                  const char  *padname;
                  
                  int   bot_layer = np->getInsert()->getPlacedBottom();
                  int   fid_mirror = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
                  int   fid_flip = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;

                  if (block->getBlockType() == BLOCKTYPE_PADSTACK || 
                      block->getBlockType() == BLOCKTYPE_PADSHAPE)
                  {
                     padname = block->getName();
                  }
                  else
                  {
                     BlockStruct *b = get_blockpadname(&(block->getDataList()));
                     if (b)
                        padname = b->getName();
                     else
                        padname = NULL;
                  }

                  // check here if fiducial is placed ob both
                  Attrib   *a = is_attvalue(doc, np->getAttributesRef(), ATT_FIDUCIAL_PLACE_BOTH, 1);
   
                  if (a)
                  {
                     // bottom 
                     fprintf(wfp,"FID %s%d %s %1.*lf %1.*lf %s %1.2lf %s %s\n",fiducialname,
                        ++fidcnt, (padname)?gen_string(padname):"NO_PAD",
                        output_units_accuracy, point2.x, 
                        output_units_accuracy, point2.y, 
                        "BOTTOM",
                        RadToDeg(block_rot),
                        "MIRRORX","FLIP");

                     write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);

                     // now top
                     fprintf(wfp,"FID %s%d %s %1.*lf %1.*lf %s %1.2lf %s %s\n",fiducialname,
                        ++fidcnt, (padname)?gen_string(padname):"NO_PAD",
                        output_units_accuracy, point2.x, 
                        output_units_accuracy, point2.y, 
                        "TOP",
                        RadToDeg(block_rot),
                        "0","0");

                     write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);
                  }
                  else
                  {
                     fprintf(wfp,"FID %s%d %s %1.*lf %1.*lf %s %1.2lf %s\n",fiducialname,
                        ++fidcnt, (padname)?gen_string(padname):"NO_PAD",
                        output_units_accuracy, point2.x, 
                        output_units_accuracy, point2.y, 
                        (bot_layer)?"BOTTOM":"TOP",
                        RadToDeg(block_rot),
                        (fid_mirror)?"MIRRORX":"0");	//,
                        //(fid_flip)?"FLIP":"0");

                     write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);
                  }
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)
            {
               if (artwork_or_shape)
               {
                  CString pinname = np->getInsert()->getRefname();

                  GENCADCompPinInst *c = new GENCADCompPinInst;
                  comppininstarray.SetAtGrow(comppininstcnt,c);  
                  comppininstcnt++; 
                  
                  c->padstackname = block->getName();
                  c->pinname = pinname;
                  c->rotation = block_rot;
                  c->drill = TRUE;
                  c->pinx = point2.x;
                  c->piny = point2.y;
               }
            }
            else if ( (np->getInsert()->getInsertType() == insertTypePin) ||
               (treat_mechanical_pins_as_normal_pins && np->getInsert()->getInsertType() == insertTypeMechanicalPin) )
            {
               if (artwork_or_shape)
               {
                  CString padlayer = "ALL";
                  CString pinname = np->getInsert()->getRefname();

                  GENCADCompPinInst *c = new GENCADCompPinInst;
                  comppininstarray.SetAtGrow(comppininstcnt++, c);  
                  c->padstackname = block->getName();
                  c->pinname = pinname;
                  c->drill = FALSE;
                  c->rotation = block_rot;
                  c->pinx = point2.x;
                  c->piny = point2.y;
               }
               break;
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;

               if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();
            
               acnt = GENCAD_WriteSHAPEData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer,artwork_or_shape, acnt, (np->getInsert()->getInsertType() == insertTypeGenericComponent));
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end GENCAD_WriteSHAPEData */
   return acnt;
}

//--------------------------------------------------------------
int GENCAD_TestComplexApertureData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel)
{
   Point2   point2;
   DataStruct *np;
   Mat2x2   m;
   int      apptype = 0;
   int      cnt = 0;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            POSITION polyPos;
            
            double   cx, cy, radius;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden())
                  continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
                  apptype |= PADSHAPE_CIRCLE;
               else if (closed == 0 && cnt == 0)  
                  apptype |= PADSHAPE_RECTANGULAR;
               else
               {  
                  apptype &= ~PADSHAPE_RECTANGULAR;
                  apptype |= PADSHAPE_POLYGON;
               }

               cnt++;
            }
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         break;
         
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

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
               if (block->getShape() == T_ROUND)
                  apptype |= PADSHAPE_CIRCLE;
               else
                  apptype |= PADSHAPE_POLYGON;
            }
            else // not aperture
            {
               apptype |= GENCAD_TestComplexApertureData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1);
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end GENCAD_TestComplexApertureData */
   return apptype;
}

//--------------------------------------------------------------
long GENCAD_WriteComplexApertureData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int apptyp)
{
   Point2   point2;
   DataStruct *np;
   Mat2x2   m;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (ignore_non_manufacturing_info(np))
         continue;

      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden())
                  continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               if (apptyp == PADSHAPE_CIRCLE)
               {
                  double cx, cy, radius;
                  if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
                  {
                     wGENCAD_Graph_Circle(wfp, cx, cy, radius );
                  }
               }
               else
               {
                  if (closed)
                  {
                     int first = TRUE;
                     Point2   firstp;
                     Point2   p1,p2;

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
                           wGENCAD_Graph_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                           p1 = p2;
                        }
                     } // while
                  }  // closed   
                  else
                  {
                     int first = TRUE;
                     Point2   firstp;
                     Point2   p1,p2;
                     double   w = scale * doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();

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
                           // diagonal
                           double xx1, yy1, xx2, yy2;

                           if (p1.x < p2.x)
                           {
                              xx1 = p1.x;
                              xx2 = p2.x;
                           }
                           else
                           {
                              xx2 = p1.x;
                              xx1 = p2.x;
                           }

                           if (p1.y < p2.y)
                           {
                              yy1 = p1.y;
                              yy2 = p2.y;
                           }
                           else
                           {
                              yy2 = p1.y;
                              yy1 = p2.y;
                           }

                           double halfWidth = w/2.;
                           double left   = xx1 - halfWidth;
                           double right  = xx2 + halfWidth;
                           double bottom = yy1 - halfWidth;
                           double top    = yy2 + halfWidth;

                           if (apptyp == PADSHAPE_RECTANGULAR)
                           {
                              wGENCAD_Graph_Rectangle(wfp,left,bottom,right,top);
                           }
                           else
                           {
                              wGENCAD_Graph_Line(wfp,left, bottom,0.0,right,bottom,0.0 );
                              wGENCAD_Graph_Line(wfp,right,bottom,0.0,right,top   ,0.0 );
                              wGENCAD_Graph_Line(wfp,right,top   ,0.0,left ,top   ,0.0 );
                              wGENCAD_Graph_Line(wfp,left, top   ,0.0,left ,bottom,0.0 );
                           }

                           p1 = p2;
                        }
                     } // while
                  }
               }
            }
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         break;
         
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

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {

               if (block->getShape() == T_COMPLEX)
               {
                  BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  int block_layer = -1;

                  GENCAD_WriteComplexApertureData(wfp,&(subblock->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, apptyp);
               }
               else
               {
                  if (apptyp == PADSHAPE_CIRCLE)
                  {
                     wGENCAD_Graph_Aperture(wfp, point2.x, point2.y,
                        block->getSizeA() * scale * np->getInsert()->getScale(),
                        block->getSizeB() * scale * np->getInsert()->getScale(),
                        block->getXoffset() * scale * np->getInsert()->getScale(),
                        block->getYoffset() * scale * np->getInsert()->getScale(),
                        block->getShape(), block_rot);

                  }
                  else
                  {
                     wGENCAD_Graph_ComplexAperture(wfp, point2.x, point2.y,
                        block->getSizeA() * scale * np->getInsert()->getScale(),
                        block->getSizeB() * scale * np->getInsert()->getScale(),
                        block->getXoffset() * scale * np->getInsert()->getScale(),
                        block->getYoffset() * scale * np->getInsert()->getScale(),
                        block->getShape(), block_rot);
                  }
               }
            }
            else // not aperture
            {
               GENCAD_WriteComplexApertureData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, apptyp);
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end GENCAD_WriteComplexApertureData */
   return 0;
}

//--------------------------------------------------------------
// get the first padstackblock from a geometry
//
static BlockStruct *get_padstackgeom(CDataList *DataList, int *insertlayer)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)     
         continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if ( (np->getInsert()->getInsertType() == insertTypePin) ||
               (treat_mechanical_pins_as_normal_pins && np->getInsert()->getInsertType() == insertTypeMechanicalPin) )
            {
               *insertlayer = np->getLayerIndex();
               return doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            }
         }
      }
   }

   return NULL;
}

//--------------------------------------------------------------
// get the first insert from a geometry
//
static BlockStruct *get_padgeom(CDataList *DataList, int *insertlayer)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)     
         continue;

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            *insertlayer = np->getLayerIndex();
            return doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
         }
      }
   }

   return NULL;
}

//--------------------------------------------------------------
// a testpad must refer to a $PAD padshape.
// this means it can not be a padstack with multiple layers
// return 1 = top
// return 2 = bottom
// return 0 = no good testpad
static int is_testpad(DataStruct *d, BlockStruct **padblock)
{
   BlockStruct *block = doc->Find_Block_by_Num(d->getInsert()->getBlockNumber());
   *padblock = NULL;
   int   found = 0;

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *np = block->getDataList().GetNext(pos);

      if (np->getDataType() != T_INSERT)  
         continue;

      *padblock = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
      found++;
   }

   if (found != 1)   
      return 0;

   BOOL AccessTop = block->getFlags() & BL_ACCESS_TOP || block->getFlags() & BL_ACCESS_TOP_ONLY;
   BOOL AccessBottom = block->getFlags() & BL_ACCESS_BOTTOM || block->getFlags() & BL_ACCESS_BOTTOM_ONLY;

   if (AccessTop && !AccessBottom)
   {
      // only access from top
      if (d->getInsert()->getMirrorFlags() | MIRROR_LAYERS)
         return 2;

      return 1;
   }
   else if (!AccessTop && AccessBottom)
   {
      // only access from bottom
      if (d->getInsert()->getMirrorFlags() | MIRROR_LAYERS)
         return 1;

      return 2;
   }

   return FALSE;
}

/******************************************************************************
* GENCAD_WriteROUTESData
*/
void GENCAD_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, 
		double scale, int embeddedLevel, int insertLayer, const char *netname, CGenCadPadstackMap& padstackMap)
{ 
   int routeattcnt = 0;
	int viaattcnt = 0;
	int bondpadcnt = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   for (int i=0;i<routescnt;i++)
   {
      // different netname
      if (strcmp(routesarray[i]->netname, netname))
         continue; 

if (!STRCMPI(netname, "BUND_MII(2)"))
	int a = 0;

		DataStruct* data = DataList->GetNext(routesarray[i]->pos);
      if (ignore_non_manufacturing_info(data))
            continue;

		if (data->getDataType() == dataTypePoly)
		{
         if (data->getGraphicClass() != GR_CLASS_ETCH)   
            continue;

			int layer;
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

			const char* gencadLayer = Layer_Gencad(layer);
         if (gencadLayer == NULL)
            continue;

			int shapeNum = 1;
		   int voidNum = 1;
         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            CPoly* poly = data->getPolyList()->GetNext(polyPos);
            if (poly->isHidden())
               continue;

            BOOL polyFilled = poly->isFilled();
            BOOL closed = poly->isClosed();
            BOOL boundary = poly->isFloodBoundary();

            wGENCAD_Graph_WidthCode(wfp, poly->getWidthIndex());
            wGENCAD_Graph_Layer(wfp, gencadLayer);

            if (closed && polyFilled && !poly->isVoid())
               fprintf(wfp, "PLANE shape%d\n", shapeNum++);

            if (poly->isVoid())
               fprintf(wfp, "PLANE void%d\n", voidNum++);

            int first = TRUE;
            Point2 firstp, p1, p2;
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt* pnt = poly->getPntList().GetNext(pntPos);

               p2.x = pnt->x * scale;
               if (mirror)
						p2.x = -p2.x;

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
                  wGENCAD_Graph_Line(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y, p2.bulge );
                  p1 = p2;
               }
            }

            write_attributes(wfp, data->getAttributesRef(), "ROUTE", &routeattcnt);
         }

         write_attributes(wfp, data->getAttributesRef(), "ROUTE", &routeattcnt);
      }
		else if (data->getDataType() == dataTypeInsert)
		{
			InsertStruct* insert = data->getInsert();
			if (insert == NULL || insert->getInsertType() == insertTypePcbComponent)
				continue;

         BlockStruct *block = doc->getBlockAt(insert->getBlockNumber());
         if (block == NULL)
         {
            // big error !!! 
#ifdef _DEBUG
               fprintf(flog,"Internal Structure Error at entity %ld !!!\n", data->getEntityNumber());
               display_error++;
#endif
            continue;
         }


		   Point2 point2;
         point2.x = insert->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = insert->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ insert->getMirrorFlags();
         double block_rot = rotation + insert->getAngle();
         if (mirror)
            block_rot = PI2 - (rotation +insert->getAngle());   // add 180 degree if mirrored.

         if (insert->getInsertType() == insertTypeFreePad || insert->getInsertType() == insertTypeVia
            || insert->getInsertType() == insertTypeBondPad)
         {
				CString blockName = block->getName();
            CString refName;

            if(insert->getInsertType() == insertTypeBondPad)
            {               
               DataStruct* padstackdata = block->GetFirstPinInsertData();
               if(padstackdata)
               {             
                  BlockStruct *padblock = GetBondPadPadstackBlock(doc->getCamCadData(), block);
                  if(padblock)
                  {
                     InsertStruct *padstackinsert = padstackdata->getInsert();
                     double padrout = padstackinsert->getAngle();
                     double rot = block_rot + (padstackinsert->getGraphicMirrored()?(PI2 - padrout):padrout);
                     int rotdegree = normalizeDegrees(round(RadToDeg(rot)));
                     if(rotdegree)
                        blockName.Format("%s_%d", padblock->getName(), rotdegree);                     
                     else
                        blockName = padblock->getName();

                  }

                  refName = insert->getRefname();
                  if(refName.IsEmpty())
                     refName.Format("bondpad%d",++bondpadcnt);
               }
            }
            else
            {
               refName.Format("via%d", ++viacnt);
            }

				if (insert->getGraphicMirrored())
				{
					CGenCadPadstack* padstack = NULL;
					padstackMap.Lookup(blockName, padstack);
					if (padstack != NULL)
						blockName = padstack->GetMirName();
				}
				blockName = gen_string(blockName);

            fprintf(wfp,"VIA %s %1.*lf %1.*lf ALL -2 %s\n",
						blockName, output_units_accuracy, point2.x, output_units_accuracy, point2.y, refName);

         }
			else if (insert->getInsertType() == insertTypeTestPoint || insert->getInsertType() == insertTypeTestPad)
         {
				if (convert_tp_comps)
					continue;

            if (block->getBlockType() == blockTypePcbComponent || block->getBlockType() == blockTypeTestPoint)
				{
               // here now get the block of the padstack.
					int insertlayer;
               BlockStruct* subBlock = get_padstackgeom(&block->getDataList(), &insertlayer);
               if (subBlock == NULL)
               {
                  // not a padstack, maybe an aperture
						subBlock = get_padgeom(&block->getDataList(), &insertlayer);
                  if (subBlock == NULL)
                     continue;
               }
               block = subBlock;
            }
            
            if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
            {
					bool padlayertop = block_mirror?false:true;
					bool padlayerflip = block_mirror?true:false;

               fprintf(wfp, "LAYER %s\n", (!padlayertop)?"BOTTOM":"TOP");

               // testpad references to a $PAD section, not to a PADSTACK !!!!
               fprintf(wfp, "TESTPAD %s %1.*lf %1.*lf 0 %s testpad%d\n",
							gen_string(block->getName()), output_units_accuracy, point2.x, output_units_accuracy, point2.y, 
							(padlayerflip)?"MIRRORX":"0", ++testpadcnt);	               
            }
            else if (block->getBlockType() == blockTypePadstack || block->getBlockType() == blockTypeTestPad)
            {
               BlockStruct* padblock = NULL;
               int testpadlayer; // 1 = top, 2 = bottom

               if (testpadlayer = is_testpad(data, &padblock))
               {
						bool padlayerflip = block_mirror?true:false;

						fprintf(wfp, "LAYER %s\n", (testpadlayer == 2)?"BOTTOM":"TOP");

                  // testpad references to a $PAD section, not to a PADSTACK !!!!
                  fprintf(wfp, "TESTPAD %s %1.*lf %1.*lf 0 %s testpad%d\n",
								gen_string(block->getName()), output_units_accuracy, point2.x, output_units_accuracy, point2.y, 
								(padlayerflip)?"MIRRORX":"0", ++testpadcnt);
               }
               else
               {
                  // not resolved is what a mirror of the insert was done.
                  // this must be a via, not a TESTPAD, because a via references a PADSTACK and a testpad only
                  // a shape on a single layer.

						CString blockName = block->getName();
						if (insert->getGraphicMirrored())
						{
							CGenCadPadstack* padstack = NULL;
							padstackMap.Lookup(blockName, padstack);
							if (padstack != NULL)
								blockName = padstack->GetMirName();
						}
						blockName = gen_string(blockName);

                  fprintf(wfp, "VIA %s %1.*lf %1.*lf ALL -2 via%d\n",
								blockName, output_units_accuracy, point2.x, output_units_accuracy, point2.y, ++viacnt);
               }
            }
            else
            {
               fprintf(flog, "TESTPOINT [%s] [%s] has no Padstack ?\n", insert->getRefname(), block->getName());
               display_error++;
            }
         }
         else if (!insert->getInsertType())
         {  
				// hierachies in nets are not supported.
            fprintf(flog, "Hierachical Net [%s] not supported.\n", netname);
            display_error++;
         } 
      }
   }
}

//--------------------------------------------------------------
void GENCAD_GetROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   //DataStruct *np;
   //int   layer;
   //const char  *l;
   //Attrib   *a;

   //POSITION pos1;
   //POSITION pos = DataList->GetHeadPosition();
   //while (pos != NULL)
   //{
   //   pos1 = pos; // pos is changed in getnext
   //   np = DataList->GetNext(pos);

   //   if (np->getDataType() != T_INSERT)
   //   {
   //      if (np->getGraphicClass() != GR_CLASS_ETCH)
   //         continue;

   //      if (insertLayer != -1)
   //         layer = insertLayer;
   //      else
   //         layer = np->getLayerIndex();

   //      // insert has not always a layer definition.
   //      if (!doc->get_layer_visible(layer, mirror))
   //         continue;

   //      // here check if ATTR_NETNAME == netname
   //      a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

   //      if (!a)  continue;

   //      if ((l = Layer_Gencad(layer)) == NULL)
   //         continue;
   //   }
   //   else
   //   {
   //      // here check if ATTR_NETNAME == netname

   //      // allow only vias and test points
   //      a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
   //      if (!a)  continue;
   //   }

   //   // here store it
   //   GENCADRoutes *r = new GENCADRoutes;
   //   routesarray.SetAtGrow(routescnt,r);  
   //   routescnt++;   
   //   r->netname = get_attvalue_string(doc, a);
   //   r->pos = pos1;
   //}
}


/******************************************************************************
* GENCAD_WriteCOMPONENTData
*/
void GENCAD_WriteCOMPONENTData(FILE* wfp, FileStruct* file, BlockStruct* fileBlock,
      double insert_x, double insert_y, double rotation, int mirror, double scale,
		int embeddedLevel, int insertLayer, CGenCadDeviceMap& deviceMap)
{
   Mat2x2 m;
   Point2 point2;
   int compattcnt = 0;
	int devIndex = 0;

   RotMat2(&m, rotation);

   POSITION pos = fileBlock->getHeadDataInsertPosition();
   while (pos != NULL)
   {
		DataStruct* data = fileBlock->getNextDataInsert(pos);
		InsertStruct* insert = data->getInsert();
		if (insert ==  NULL)
			continue;

		Point2 point2;
		point2.x = insert->getOriginX() * scale;
      if (mirror)
         point2.x = -point2.x;
      point2.y = insert->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ insert->getMirrorFlags();
      double block_rot = rotation + insert->getAngle();

		BlockStruct* block = doc->getBlockAt(insert->getBlockNumber());

      if (insert->getInsertType() == insertTypePcbComponent)
      {
         CString compname = insert->getRefname();

			CGenCadDevice* device = NULL;
			Attrib* attrib = NULL;
         if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_PARTNUMBER, 1))
			{
				CString partNumber = gen_string(get_attvalue_string(doc, attrib));
				device = deviceMap.AddDevice(partNumber, partNumber);
			}
			else 
				device = deviceMap.GenerateDevice();

         fprintf(wfp, "COMPONENT %s\n", compname);
         fprintf(wfp, "PLACE %1.*lf %1.*lf\n", output_units_accuracy, point2.x, output_units_accuracy, point2.y);
         fprintf(wfp, "LAYER %s\n", insert->getPlacedBottom()?"BOTTOM":"TOP");
         fprintf(wfp, "ROTATION %1.2lf\n", RadToDeg(block_rot));
         fprintf(wfp, "SHAPE %s %s %s\n", gen_string(block->getName()), 
														(block_mirror & MIRROR_FLIP)?"MIRRORY":"0",
                                          (block_mirror & MIRROR_LAYERS)?"FLIP":"0");
         fprintf(wfp, "DEVICE %s\n", device->GetName());

         GENCADCompList *c = new GENCADCompList;
         complistarray.SetAtGrow(complistcnt++, c);  
         c->compname = compname;
         c->geomname = block->getName();
         c->devicename = device->GetName();

         // attributes
         write_componentattributes(wfp, data->getAttributesRef(), "COMPONENT", &compattcnt, file, device);
      }
      else if (insert->getInsertType() == insertTypeTestPoint)
      {
         if (convert_tp_comps)   // if set to FALSE, a via record is generated.
         {
            CString compname = insert->getRefname();

            fprintf(wfp, "COMPONENT %s\n", gen_string(compname));
            fprintf(wfp, "PLACE %1.*lf %1.*lf\n", output_units_accuracy, point2.x, output_units_accuracy, point2.y);
   
            if (block->getBlockType() == blockTypePadstack)
            {
               // BL_ACCESS_TOP              
               // BL_ACCESS_BOTTOM           
               // BL_ACCESS_OUTER            
               // BL_ACCESS_NONE                
               int padlayertop = TRUE;
               int padlayerflip = 0;

               if ((block->getFlags() & BL_ACCESS_OUTER) == BL_ACCESS_OUTER)
               {
                  padlayertop = !(insert->getPlacedBottom());
                  padlayerflip = insert->getMirrorFlags() & MIRROR_FLIP;
               }
               else if (block->getFlags() & BL_ACCESS_NONE)
               {
                  padlayertop = !(insert->getPlacedBottom());
                  padlayerflip = insert->getMirrorFlags() & MIRROR_FLIP;
               }
               else if (block->getFlags() & BL_ACCESS_TOP)
               {
                  padlayertop = TRUE;
                  padlayerflip = FALSE;

                  // the idea is that the pseudo component should be placed on the layer the padstack is on
                  if (insert->getMirrorFlags() & MIRROR_FLIP && insert->getPlacedBottom())
                  {
                     padlayertop = FALSE;
                     padlayerflip = TRUE;
                  }
                  else if (insert->getPlacedBottom())
                  {
                     padlayertop = FALSE;
                     padlayerflip = FALSE;
                  }
               }
               else if (block->getFlags() & BL_ACCESS_BOTTOM)
               {
                  // the idea is that the pseudo component should be placed on the layer the padstack is on
                  if (insert->getMirrorFlags() & MIRROR_FLIP && insert->getPlacedBottom())
                  {
                     padlayertop = TRUE;
                     padlayerflip = TRUE;
                  }
                  else if (insert->getPlacedBottom())
                  {
                     padlayertop =TRUE;
                     padlayerflip = FALSE;
                  }
                  else
                  {
                     // component is not mirrored, but padstack is on bottom
                     padlayertop = FALSE;
                     padlayerflip = FALSE;
                  }
               }
               else
               {
                  // no special treatment
                  padlayertop = !(insert->getPlacedBottom());
                  padlayerflip = insert->getMirrorFlags() & MIRROR_FLIP;
               }

               fprintf(wfp, "LAYER %s\n", (!padlayertop)?"BOTTOM":"TOP");
               fprintf(wfp, "ROTATION %1.2lf\n", RadToDeg(block_rot));
               fprintf(wfp, "SHAPE %s%s %s\n", testpointshapeprefix, gen_string(block->getName()), (padlayerflip)?"MIRRORY FLIP":"0 0");
            }
            else
            {
               fprintf(wfp, "LAYER %s\n", (insert->getPlacedBottom())?"BOTTOM":"TOP");
               fprintf(wfp, "ROTATION %1.2lf\n", RadToDeg(block_rot));
               fprintf(wfp, "SHAPE %s %s\n", gen_string(block->getName()), (block_mirror & MIRROR_FLIP)?"MIRRORY FLIP":"0 0");
            }

				CGenCadDevice* device = deviceMap.GenerateDevice();
            fprintf(wfp, "DEVICE    %s\n", device->GetName());

            GENCADCompList *c = new GENCADCompList;
            complistarray.SetAtGrow(complistcnt,c);  
            complistcnt++;

            c->compname = compname;
            c->geomname = block->getName();
            c->devicename = device->GetName();

            // attributes
         write_componentattributes(wfp, data->getAttributesRef(), "COMPONENT", &compattcnt, file, device);
         }
		}
   }
   return;
}

//--------------------------------------------------------------
void GENCAD_WriteTESTPOINTData(FILE *wfp, CDataList *DataList, const char *netname)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)  
         continue;

      switch(np->getDataType())
      {
      case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
            {
               if (convert_tp_comps)   // if set to FALSE, a via record is generated.
               {
                  CString compname = np->getInsert()->getRefname();
                  Attrib   *a;
                  CString  n;

                  if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))
                  {
                     n = get_attvalue_string(doc, a);

                     if (n.Compare(netname) == 0)
                     {
                        fprintf(wfp, "NODE %s %s\n", compname, "1");
                        //write_attributes(fp, compPin->getAttributesRef(), "TESTPIN", &comppinattcnt);
                     }     
                  }
               }
               break;
            }
         } // case INSERT
         break;
      } // end switch
   } // end GENCAD_WriteTESTPOINTData */
}

static void write_mech_shape(FILE *fp, double scale, CDataList *DataList, long *acnt, int *shattcnt, int IsArtwork)
{
   DataStruct *np;
   CList<CString, CString&> CShapes;

   if (!generic_insert.CompareNoCase("mechanical") ||!mechanical_insert.CompareNoCase("mechanical") ||!unknown_insert.CompareNoCase("mechanical"))
   {
      POSITION pos = DataList->GetHeadPosition();
      while (pos != NULL)
      {
         np = DataList->GetNext(pos);   

         if (np->getDataType() != T_INSERT)     
            continue;

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if (block == NULL || block->getBlockType() == BLOCKTYPE_PCBCOMPONENT || block->getBlockType() == BLOCKTYPE_TESTPOINT)
            // if the block type is pcbcomponent or testpoint, the shape is already written, so don't write again
            continue;

         if (CShapes.Find(CString(block->getName())) != NULL) // The component is already written in shape section
            continue;

         if ((np->getInsert()->getInsertType() == INSERTTYPE_GENERICCOMPONENT && !generic_insert.CompareNoCase("mechanical")) ||
            (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT && !mechanical_insert.CompareNoCase("mechanical")) ||
            (np->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN && !unknown_insert.CompareNoCase("mechanical")))
         {  
            write_pcbshapes(fp, scale, IsArtwork, acnt, shattcnt, block);
            CShapes.AddTail(CString(block->getName()));          
         }
      }
   }
}

/******************************************************************************
* GENCAD_WriteMECHData
*/
void GENCAD_WriteMECHData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int panelfile, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   fidcnt = 0;
   int   fiducialattcnt = 0;
   CString  fidname;

   // allow different name for panel and board fiducial
   if (panelfile == FALSE)
      fidname = fiducialname;
   else
      fidname = panelfiducialname;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)     
         continue;

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

            if ((np->getInsert()->getInsertType() == INSERTTYPE_GENERICCOMPONENT && !generic_insert.CompareNoCase("Mechanical")) ||
               (np->getInsert()->getInsertType() == INSERTTYPE_MECHCOMPONENT && !mechanical_insert.CompareNoCase("Mechanical")) ||
               (np->getInsert()->getInsertType() == INSERTTYPE_UNKNOWN && !unknown_insert.CompareNoCase("Mechanical")))
            {
               fprintf(wfp,"MECHANICAL %s\n", np->getInsert()->getRefname());
               fprintf(wfp,"PLACE %1.*lf %1.*lf\n",output_units_accuracy, point2.x, output_units_accuracy, point2.y);
               fprintf(wfp,"LAYER %s\n",np->getInsert()->getPlacedBottom()?"BOTTOM":"TOP");
               fprintf(wfp,"ROTATION %1.2lf\n",RadToDeg(block_rot));
               fprintf(wfp,"SHAPE %s %s %s\n", 
                  gen_string(block->getName()), (block_mirror & MIRROR_FLIP)?"MIRRORY":"0",
                                           (block_mirror & MIRROR_LAYERS)?"FLIP":"0");
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL)
            {
               const char  *padname;
               int   bot_layer = np->getInsert()->getPlacedBottom();
               int   fid_mirror = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;
               int   fid_flip = np->getInsert()->getMirrorFlags() & MIRROR_FLIP;

               if (block->getBlockType() == BLOCKTYPE_PADSTACK || 
                   block->getBlockType() == BLOCKTYPE_PADSHAPE)
               {
                  padname = block->getName();
               }
               else
               {
                  BlockStruct *b = get_blockpadname(&(block->getDataList()));

                  if (b)
                     padname = b->getName();
                  else
                     padname = NULL;
               }
         
               // check here if fiducial is placed ob both
               Attrib   *a = is_attvalue(doc, np->getAttributesRef(), ATT_FIDUCIAL_PLACE_BOTH, 1);
               CString  refname = gen_string(np->getInsert()->getRefname());

               if (!strlen(refname))   refname = "?";

               if (a)
               {
                  // bottom 
                  fprintf(wfp,"FID %s%d %s %1.*lf %1.*lf %s %1.2lf %s %s\n",fiducialname,
                     ++fidcnt, (padname)?gen_string(padname):"NO_PAD",
                        output_units_accuracy, point2.x, 
                        output_units_accuracy, point2.y, 
                        "BOTTOM",
                        RadToDeg(block_rot),
                        "MIRRORX","FLIP");

                  fprintf(wfp,"ATTRIBUTE FIDUCIAL \"%s\" %s\n",block->getName(), refname);
                  write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);

                  // now top
                  fprintf(wfp,"FID %s%d %s %1.*lf %1.*lf %s %1.2lf %s %s\n",fiducialname,
                        ++fidcnt, (padname)?gen_string(padname):"NO_PAD",
                        output_units_accuracy, point2.x, 
                        output_units_accuracy, point2.y, 
                        "TOP",
                        RadToDeg(block_rot),
                        "0","0");

                  fprintf(wfp,"ATTRIBUTE FIDUCIAL \"%s\" %s\n",block->getName(), refname);
                  write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);
               }
               else
               {
                  fprintf(wfp,"FID %s%d %s %1.*lf %1.*lf %s %1.2lf %s %s\n",fiducialname,
                        ++fidcnt, (padname)?gen_string(padname):"NO_PAD",
                        output_units_accuracy, point2.x, 
                        output_units_accuracy, point2.y, 
                        (bot_layer)?"BOTTOM":"TOP",
                        RadToDeg(block_rot),
                        (fid_mirror)?"MIRRORX":"0",
                        (fid_flip)?"FLIP":"0");

                  // attributes
                  fprintf(wfp,"ATTRIBUTE FIDUCIAL \"%s\" %s\n",block->getName(), refname);
                  write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {
               double drill = get_drill_from_block_no_scale(doc, block) * scale;

               if (drill == 0)
               {
                  fprintf(flog, "TOOLING at [%lg, %lg] has no drill size -> set to .DEFAULTDRILLSIZE [%lg]\n", 
                     point2.x, point2.y, defaultdrillsize);
                  display_error++;
                  drill = defaultdrillsize;
               }

               fprintf(wfp,"FHOLE %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, point2.x, output_units_accuracy, point2.y, output_units_accuracy, drill);
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLHOLE)
            {
               double drill = get_drill_from_block_no_scale(doc, block)*scale;

               if (drill == 0)
               {
                  fprintf(flog, "HOLE at [%lg, %lg] has no drill size -> set to .DEFAULTDRILLSIZE [%lg]\n", 
                     point2.x, point2.y, defaultdrillsize);
                  display_error++;
                  drill = defaultdrillsize;
               }

               fprintf(wfp,"HOLE %1.*lf %1.*lf %1.*lf\n",
                  output_units_accuracy, point2.x, output_units_accuracy, point2.y, output_units_accuracy, drill);
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)  // freepads with netnames are treated as vias.
            {
               if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))
                  break;

               double drill = get_drill_from_block_no_scale(doc, block);

               // do not put out a freepad as a hole if it is not drilled.
               if (drill > 0)
               {
                  fprintf(wfp,"HOLE %1.*lf %1.*lf %1.*lf\n",
                     output_units_accuracy, point2.x, output_units_accuracy, point2.y, output_units_accuracy, drill*scale);
               }
            }
         }
         break;
      }
   }
}

/******************************************************************************
* GENCAD_WriteTestProbeData
*/
void GENCAD_WriteTestProbeData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int panelfile, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   int testPinCount = 0;
   CString  fidname;

   // allow different name for panel and board fiducial
   if (panelfile == FALSE)
      fidname = fiducialname;
   else
      fidname = panelfiducialname;

   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      // EXPLODEALL - call DoWriteData() recursively to write embedded entities
      point2.x = data->getInsert()->getOriginX() * scale;

      if (mirror)
         point2.x = -point2.x;

      point2.y = data->getInsert()->getOriginY() * scale;
      TransPoint2(&point2, 1, &m, insert_x, insert_y);

      // insert if mirror is either global mirror or block_mirror, but not if both.
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
      double block_rot = rotation + data->getInsert()->getAngle();

      if (block_mirror)
         block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (data->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)
         continue;
      
      CString netname = "-1";
      Attrib *attrib;

      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1))
      {
#if CamCadMajorMinorVersion > 406  //  > 4.6
         netname = attrib->getStringValue();
#else
         netname = doc->ValueArray[attrib->getIntValue()];
#endif
      }

      CString refName;

      if (data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
         refName = data->getInsert()->getRefname();
      else
         refName.Format("%s%d", testpinprefix, ++testPinCount);

      if(IsProbedDiePin(doc->getCamCadData(),data))
      {
         fprintf(flog, "$TESTPINS Net: %s Probe %s - Skipped Probe for Die pin.\n", netname, refName);
         continue;
      }


		// see if the probe is placed
		bool placed = false;
		WORD ppKW = doc->IsKeyWord(ATT_PROBEPLACEMENT, 0);

		if (data->lookUpAttrib(ppKW, attrib))
			placed = ((CString)get_attvalue_string(doc, attrib) == "Placed");

#if CamCadMajorMinorVersion > 406  //  > 4.6
      CString probeStyle( data->getProbeStyle(doc->getCamCadData()) );
#else
      CString probeStyle( data->getProbeStyle(*doc) );
#endif

		if (placed)
			fprintf(wfp, "TESTPIN %s %1.*lf %1.*lf %s -1 -1 %s %s\n", refName,
               output_units_accuracy, point2.x, output_units_accuracy, point2.y, netname, gen_string(probeStyle),
					data->getInsert()->getPlacedBottom()?"BOTTOM":"TOP");
		else
			fprintf(wfp, "TESTPIN %s -32767 -32767 %s -1 -1 -1 %s\n", refName,  // Case 1442, unplaced probe coords
               netname, gen_string(probeStyle), data->getInsert()->getPlacedBottom()?"BOTTOM":"TOP"); // set to -32767,-32767, not 0,0


      // attributes
      int testPinAttCount = 0;
      fprintf(wfp, "ATTRIBUTE TESTPIN \"GeometryName\" \"%s\"\n", block->getName());
      write_attributes(wfp, data->getAttributesRef(), "TESTPIN", &testPinAttCount);
   }
}

//--------------------------------------------------------------
void GENCAD_WritePANELBOARDSData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, 
      const char *filename, const char *ext)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   boardcnt = 0;
   int   boardattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)  
         continue;

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

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCB)
            {
               fprintf(wfp,"BOARD BOARD%d\n",++boardcnt);
               fprintf(wfp,"FILE %s%s\n",filename,ext);
               fprintf(wfp,"FIDUCIAL 0.0 0.0\n");
               fprintf(wfp,"ROTATION %1.2lf\n",RadToDeg(block_rot));
               fprintf(wfp,"PLACE %1.*lf %1.*lf\n",
                  output_units_accuracy, point2.x, output_units_accuracy, point2.y);

               fprintf(wfp,"ELEMENT %s\n",(block_mirror)?"MIRRORY FLIP":"0 0");

               // attributes
               CString  refname = gen_string(np->getInsert()->getRefname());
               if (!strlen(refname))   refname = "?";

               fprintf(wfp,"ATTRIBUTE CAD %s %s\n",block->getName(), refname);
               write_attributes(wfp, np->getAttributesRef(), "BOARD", &boardattcnt);
            }
         } // case INSERT
         break;
      } // end switch
   } // end GENCAD_WritePANELBOARDSData */

   if (!boardcnt) // if no board found in panel, default to 1.
   {
      fprintf(wfp,"BOARD BOARD%d\n",++boardcnt);
      fprintf(wfp,"FILE %s%s\n",filename,ext);
      fprintf(wfp,"FIDUCIAL 0.0 0.0\n");
      fprintf(wfp,"ROTATION 0\n");
      fprintf(wfp,"PLACE 0 0\n");
   }
}

/******************************************************************************
* GENCAD_WritePADSTACKData
	return 1 = top
	       2 = bottom
	       3 = all
	       4 = inner  
*/
int GENCAD_WritePADSTACKData(FILE *wfp, const CString padstkname, CDataList *DataList, double rotation, int mirror,
									  double scale, int embeddedLevel, int insertLayer, bool toppad, bool dupMirPadstack)
{
   int padattcnt = 0;
   int pl = 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = DataList->GetNext(pos);      
      if (data == NULL || data->getDataType() != dataTypeInsert)  
         continue;

		InsertStruct* insert = data->getInsert();
		if (insert == NULL)
			continue;

      if (insert->getInsertType() == insertTypeThermalPad || insert->getInsertType() == insertTypeClearancePad ||
			 insert->getInsertType() == insertTypeObstacle   || insert->getInsertType() == insertTypeDrillSymbol)
         continue;

	   BlockStruct* block = doc->getBlockAt(insert->getBlockNumber());
      if (block->getFlags() & BL_TOOL) 
         continue;

      // insert if mirror is either global mirror or block_mirror, but not if both.
      double block_rot = rotation;
      if (mirror)
         block_rot = PI2 - rotation;   // add 180 degree if mirrored.

      int block_layer = -1;
      if (insertLayer != -1 && doc->IsFloatingLayer(data->getLayerIndex()))
         block_layer = insertLayer;
      else if (data->getLayerIndex() != -1)
         block_layer = data->getLayerIndex();

		int r = round(RadToDeg(insert->getAngle()));
      while (r < 0)     r += 360;
      while (r >= 360)  r -= 360;

		CString padname;
      if (r)
			padname.Format("%s_%d", block->getName(), r);
		else
			padname = strlen(block->getOriginalName()) ? block->getOriginalName() : block->getName();

      if (insertLayer == -1 && doc->IsFloatingLayer(data->getLayerIndex()))
      {
         // block_layer is not set.
         if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            if (block->getShape() == T_COMPLEX)
               block = doc->getBlockAt((int)block->getSizeA());

            block_layer = Get_PADSTACKLayer(doc, &block->getDataList());
         }
      }

      CString layername = Layer_Gencad(block_layer);
      LayerStruct *layerPtr = doc->FindLayer(block_layer);
		if (dupMirPadstack)
			layerPtr = doc->getLayerAt(layerPtr->getMirroredLayerIndex());

		if (layerPtr != NULL)
		{
			if (toppad)
			{
				// if only top padstacks are defined, skipp bottom only stuff
				if (layerPtr->getMirrorOnly() || (layerPtr->getLayerFlags() & LY_BOTTOMONLY))
					continue;   
			}

         // do not translate a drill layer or drill drawing layer
         if (layerPtr->getLayerType() == layerTypeDrill)
            continue;

         if (layerPtr->getLayerType() == layerTypePadTop || layerPtr->getLayerType() == layerTypeSignalTop)
         {
            layername = "TOP";
            pl |= 1;
         }
         else if ( layerPtr->getLayerType() == layerTypePadBottom || layerPtr->getLayerType() == layerTypeSignalBottom)
         {
            layername = "BOTTOM";
            pl |= 2;
         }
         else if (layerPtr->getLayerType() == layerTypePadAll || layerPtr->getLayerType() == layerTypeSignalAll)
         {
            layername = "ALL";
            pl |= 3;
         }
         else if (layerPtr->getLayerType() == layerTypePadOuter || layerPtr->getLayerType() == layerTypeSignalOuter)
         {
            layername = "OUTER";
            pl |= 3;
         }
         else if (layerPtr->getLayerType() == layerTypePasteTop)
         {
            layername = "SOLDERPASTE_TOP";
         }
         else if (layerPtr->getLayerType() == layerTypePasteBottom)
         {
            layername = "SOLDERPASTE_BOTTOM";
         }
         else if (layerPtr->getLayerType() == layerTypePasteAll)
         {
            layername = "SOLDERPASTE";
         }
         else if (layerPtr->getLayerType() == layerTypeMaskTop)
         {
            layername = "SOLDERMASK_TOP";
         }
         else if (layerPtr->getLayerType() == layerTypeMaskBottom)
         {
            layername = "SOLDERMASK_BOTTOM";
         }
         else if (layerPtr->getLayerType() == layerTypeMaskAll)
         {
            layername = "SOLDERMASK";
         }
         else if (layerPtr->getLayerType() == layerTypePadInner || layerPtr->getLayerType() == layerTypeSignalInner)
         {
            if (layerPtr->getElectricalStackNumber())
               layername.Format("INNER%d", layerPtr->getElectricalStackNumber());
            else
               layername = "INNER";
            pl |= 4;
         }
		}

      // should have a pointx,y reference
      if (strlen(layername))
      {
         // here is because the layername could have been done in layer map.
         // only TOP, INNER, INNERx, BOTTOM, ALL allowed
         if (!STRCMPI(layername, "TOP"))   
            pl |= 1;
         else if (!STRCMPI(layername, "BOTTOM"))   
            pl |= 2;
         else if (!STRCMPI(layername, "ALL"))   
            pl |= 3;
         else if (!STRCMPI(layername, "OUTER"))    
            pl |= 3;
         else if (!STRNICMP(layername, "INNER", 5))   
            pl |= 4;
         else if (!STRCMPI(layername, "SOLDERMASK")			|| !STRCMPI(layername, "SOLDERMASK_TOP")  ||    
						!STRCMPI(layername,"SOLDERMASK_BOTTOM")	|| !STRCMPI(layername, "SOLDERPASTE")		||    
						!STRCMPI(layername, "SOLDERPASTE_TOP") 	|| !STRCMPI(layername, "SOLDERPASTE_BOTTOM"))   
			{
            // no flag
			}
         else
         {
            fprintf(flog, "PADSTACK [%s] PAD [%s] Layer [%s] not translated\n", padstkname, block->getName(), layername);
            display_error++;
         }

         if (!STRCMPI(layername, "SOLDERMASK"))  
         {
            fprintf(wfp, "PAD %s SOLDERMASK_TOP %1.2lf 0\n", gen_string(padname), RadToDeg(block_rot));
            fprintf(wfp, "PAD %s SOLDERMASK_BOTTOM %1.2lf 0\n",gen_string(padname), RadToDeg(block_rot));
         }
         else if (!STRCMPI(layername, "SOLDERPASTE"))    
         {
            fprintf(wfp, "PAD %s SOLDERPASTE_TOP %1.2lf 0\n", gen_string(padname), RadToDeg(block_rot));
            fprintf(wfp, "PAD %s SOLDERPASTE_BOTTOM %1.2lf 0\n",gen_string(padname), RadToDeg(block_rot));
         }
         else if (!STRCMPI(layername, "OUTER"))    
         {
            fprintf(wfp, "PAD %s TOP %1.2lf 0\n", gen_string(padname), RadToDeg(block_rot));
            fprintf(wfp, "PAD %s BOTTOM %1.2lf 0\n",gen_string(padname), RadToDeg(block_rot));
         }
         else
         {
            fprintf(wfp, "PAD %s %s %1.2lf 0\n", gen_string(padname), layername, RadToDeg(block_rot));
         }

         write_attributes(wfp, data->getAttributesRef(), "PAD", &padattcnt);
      } 
	}
	
   return pl;
}

/******************************************************************************
* GENCAD_RotatedPadstackData
*/
int GENCAD_RotatedPadstackData(CDataList *DataList, DbUnit insertAngle, CGenCadPadstackMap* padstackMap)
{
   int rotate = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = DataList->GetNext(pos);
		if (data == NULL || data->getDataType() != dataTypeInsert)
			continue;

		InsertStruct* insert = data->getInsert();
		if (insert == NULL)
			continue;

      if (insert->getInsertType() == insertTypeThermalPad || insert->getInsertType() == insertTypeClearancePad ||
			 insert->getInsertType() == insertTypeObstacle   || insert->getInsertType() == insertTypeDrillSymbol)
         continue;

		int r = normalizeDegrees(round(RadToDeg(insert->getAngle() + insertAngle)));

		if (r)
		{
			BlockStruct* block = doc->getBlockAt(insert->getBlockNumber());
			if (get_padrot(block->getName(), r) < 0)
			{
            CString padstackname = block->getName();
            if(padstackMap)
            {
				   CGenCadPadstack* padstack = NULL;
               if (padstackMap->Lookup(block->getName(), padstack) && padstack != NULL)
                  padstackname =  padstack->GetMirName();
            }

				CString newpadstackname;
				newpadstackname.Format("%s_%d", padstackname, r);

				GENCADPadRotStruct *p = new GENCADPadRotStruct;
				padrotarray.SetAtGrow(padrotcnt++, p);  
				p->padname = padstackname;
				p->newpadname = newpadstackname;
				p->rotation = r;
			}
		}
   } 

   return rotate;
}

/******************************************************************************
* CGenCadDeviceMap::AddDevice
*/
CGenCadDevice* CGenCadDeviceMap::AddDevice(const CString deviceName, const CString partNumber)
{
	CGenCadDevice* device = NULL;
	if (!this->Lookup(deviceName, device))
	{
		device = new CGenCadDevice(deviceName, partNumber);
		this->SetAt(deviceName, device);
	}

	return device;
}

/******************************************************************************
* CGenCadDeviceMap::GenerateDevice
*/
CGenCadDevice* CGenCadDeviceMap::GenerateDevice()
{
	CString deviceName;
	deviceName.Format("Device%d", ++genericDeviceCnt);
	
	return AddDevice(deviceName, deviceName);
}

/******************************************************************************
* CGenCadDeviceMap::WriteDevice
*/
void CGenCadDeviceMap::WriteDevice(FILE *fp)
{
   fprintf(fp,"$DEVICES\n");

	POSITION pos = this->GetStartPosition();
	while (pos != NULL)
	{
		CString deviceName;
		CGenCadDevice* device = NULL; 
		this->GetNextAssoc(pos, deviceName, device);
		if (device == NULL)
			continue;

      fprintf(fp, "DEVICE %s\n", device->GetName());

		if (!device->GetPartNubmer().IsEmpty())
			fprintf(fp, "PART %s\n", device->GetPartNubmer());

		if (!device->GetValue().IsEmpty())
			fprintf(fp, "VALUE %s\n", device->GetValue());

		if (!device->GetPlusTol().IsEmpty())
			fprintf(fp, "PTOL %s\n", device->GetPlusTol());

		if (!device->GetMinusTol().IsEmpty())
			fprintf(fp, "NTOL %s\n", device->GetMinusTol());

		if (!device->GetTolerance().IsEmpty())
			fprintf(fp, "TOL %s\n", device->GetTolerance());
	}

   fprintf(fp,"$ENDDEVICES\n\n");
}

/******************************************************************************
* GatherPadstacksAndRoutesData
*/
void GatherPadstacksAndRoutesData(CCEtoODBDoc* pDoc, FileStruct* pPCBFile, CGenCadPadstackMap& padstackMap)
{
	// Gather padstack
	for (int i=0; i<pDoc->getMaxBlockIndex(); i++)
	{
      BlockStruct* block = doc->getBlockAt(i);
      if (block == NULL || block->getFlags() & BL_WIDTH)
         continue;

      if (block->getBlockType() == blockTypePadstack || block->getFlags() & BL_TOOL)
		{
			CGenCadPadstack* padstack = new CGenCadPadstack(block);
			padstackMap.SetAt(padstack->GetName(), padstack);

			// Gather pad shape that has rotation inside a padstack
			if (block->getBlockType() == blockTypePadstack)
				GENCAD_RotatedPadstackData(&block->getDataList(),0,&padstackMap);
		}
	}

	// Gather routes data and check if testpoint is convert to comp or
	BlockStruct* fileBlock = pPCBFile->getBlock();
	POSITION pos = fileBlock->getDataList().GetHeadPosition();
	while (pos != NULL)
	{
		POSITION curPos = pos;
		DataStruct* data = fileBlock->getDataList().GetNext(pos);
		if (data == NULL || data->getAttributes() == NULL)
			continue;

		Attrib* attrib = is_attvalue(doc, data->getAttributes(), ATT_NETNAME, 1);
		if (attrib == NULL)
			continue;

		if (data->getDataType() == dataTypePoly)
		{
			// Check to make sure data is an ETCH and has proper layer
			if (data->getGraphicClass() != graphicClassEtch)
				continue;

			int layerIndex = data->getLayerIndex();
			if (!doc->get_layer_visible(layerIndex, pPCBFile->isMirrored()))
				continue;

			if (Layer_Gencad(layerIndex) == NULL)
				continue;
		}
		else if (data->getDataType() == dataTypeInsert)
		{
			InsertStruct* insert = data->getInsert();
			if (insert == NULL)
				continue;

			if (convert_tp_comps && insert->getInsertType() == insertTypeTestPoint)
			{
				// Insert is a testpoint and is going to be converted to component;
				// therefore mark the padstack as being use as testpoint to component  
				// so later in the "$SHAPES" section it will generate a shape in the
				// output file and don't add to routesarray

				BlockStruct *block = pDoc->getBlockAt(insert->getBlockNumber());
				CGenCadPadstack* padstack = NULL;
				if (padstackMap.Lookup(block->getName(), padstack) && padstack != NULL)
					padstack->SetUseAsTestComp(true);

				continue;
			}
			else if (insert->getInsertType() == insertTypeFreePad		|| insert->getInsertType() == insertTypeVia ||
						insert->getInsertType() == insertTypeTestPoint	|| insert->getInsertType() == insertTypeTestPad ||
                  insert->getInsertType() == insertTypeBondPad)
			{
				if (insert->getGraphicMirrored())
				{
					// Insert is of type via, freepad, testpoint, testpad and is inserted mirror
					// so mark the padstack as needing a mirrored version of itself

					BlockStruct *block = pDoc->getBlockAt(insert->getBlockNumber());
					CGenCadPadstack* padstack = NULL;
					if (padstackMap.Lookup(block->getName(), padstack) && padstack != NULL)
						padstack->SetNeedMirVersion(true);
				}

            if (insert->getInsertType() == insertTypeBondPad)
            {
               BlockStruct *block = pDoc->getBlockAt(insert->getBlockNumber());
               GENCAD_RotatedPadstackData(&block->getDataList(), insert->getAngle(), &padstackMap);
                  
               BlockStruct* padsblock = GetBondPadPadstackBlock(pDoc->getCamCadData(), block);
               CGenCadPadstack* padstack = NULL;
				   if (padsblock && padstackMap.Lookup(padsblock->getName(), padstack) && padstack != NULL)
               {
                  padstack->SetInsertLayer(data->getLayerIndex());
               }
            }
			}
		}

      GENCADRoutes *r = new GENCADRoutes;
      routesarray.SetAtGrow(routescnt++, r);  
      r->netname = get_attvalue_string(doc, attrib);
      r->pos = curPos;
	}
}

static void FilterIllegalChar(CString & textData)
{
   for(int i = 0; i <textData.GetLength() ; i++)
   {
      if(((' ' <= textData[i]) && ('~' >= textData[i]))
         || (13 == textData[i]) // Carriage return (ASCII decimal 13) 
         || (10 == textData[i])) // line feed (ASCII decimal 10)
      {
      }
      else
         textData.Delete(i);
   }
}

double GetUnitsFactor(double scale,int PageUnits)
{
   double unitsFactor = Units_Factor(PageUnits,UNIT_MILS) * scale;
   if(!outputUnit.CompareNoCase("INCH"))
      unitsFactor = Units_Factor(PageUnits,UNIT_INCHES) * scale;
   else if(!outputUnit.CompareNoCase("THOU"))
      unitsFactor = Units_Factor(PageUnits,UNIT_MILS) * scale;
   else if(!outputUnit.CompareNoCase("MM"))
      unitsFactor = Units_Factor(PageUnits,UNIT_MM) * scale;
   else if(!outputUnit.CompareNoCase("MM100"))
      unitsFactor = Units_Factor(PageUnits,UNIT_MM) * scale * 100;
   else if(!outputUnit.CompareNoCase("USER"))
   {
      unitsFactor = Units_Factor(PageUnits,UNIT_INCHES) * scale * outputPInt;
   }
   else if(!outputUnit.CompareNoCase("USERM"))
   {
      unitsFactor = Units_Factor(PageUnits,UNIT_MM) * scale * outputPInt * 0.1;
   }
   else if(!outputUnit.CompareNoCase("USERMM"))
   {
      unitsFactor = Units_Factor(PageUnits,UNIT_MM) * scale * outputPInt;
   }
   return unitsFactor;
}

CString GetUnitsString()
{
   CString unitsString;
   if(!outputUnit.CompareNoCase("INCH"))
      unitsString = outputUnit;
   else if(!outputUnit.CompareNoCase("THOU"))
      unitsString = outputUnit;
   else if(!outputUnit.CompareNoCase("MM"))
      unitsString = outputUnit;
   else if(!outputUnit.CompareNoCase("MM100"))
      unitsString = outputUnit;
   else if(!outputUnit.CompareNoCase("USER"))
   {
      unitsString = outputUnit;
      CString outputPIntStr;
      outputPIntStr.Format("%d",outputPInt);
      unitsString += " " + outputPIntStr;      
   }
   else if(!outputUnit.CompareNoCase("USERM"))
   {
      unitsString = outputUnit;
      CString outputPIntStr;
      outputPIntStr.Format("%d",outputPInt);
      unitsString += " " + outputPIntStr;
   }
   else if(!outputUnit.CompareNoCase("USERMM"))
   {
      unitsString = outputUnit;
      CString outputPIntStr;
      outputPIntStr.Format("%d",outputPInt);
      unitsString += " " + outputPIntStr;
   }
   else
      unitsString = "USER 1000"; //Default
   return unitsString.MakeUpper();
}
/****************************************************************************/
/*
   end GENCAD_O.CPP
*/
/****************************************************************************/

