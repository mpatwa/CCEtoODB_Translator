// $Header: /CAMCAD/5.0/read_wrt/ckt_nav_out.cpp 16    6/17/07 8:55p Kurt Van Ness $

/****************************************************************************/
/*

  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1999. All Rights Reserved.


  CKTNAV is another flavor of GENCAD

  CKTNAV is WHITESPACE sensitive !!!!
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


// nav output is to be a clone of gencad with some minor changes.
// this is the raw clone

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
#include "ckt_nav_out.h"
#include "CCEtoODB.h"
#include "Find.h"
#include "Net_Util.h"
#include "ck.h"
#include "CompValues.h"
#include "RwUiLib.h"

// -----------------------------------------------------------------------
//
// Case 1967
// All attributes output from cc data is disabled, as per case notes directive.
// The code has been left intact, in case "they" change their mind.
// We just choke off the actual output.
#define CASE_1967_OUTPUT_NO_ATTRIBUTES
//
// -----------------------------------------------------------------------

// Control log file creation in name check utility

#define DO_NMCHK_LOG TRUE   // This just turns on collection of log records in check utility, do always
static bool write_name_check_log; // Controls whether check log recrods get written to exporter log file

// -----------------------------------------------------------------------
//

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
static FILE*					navLogFp;
static CString					testpinprefix;
static CString					fiducialname;
static CString					panelfiducialname;
static int						viacnt = 0;
static int						testpadcnt = 0;
static int                 bondpadcnt = 0;

static CComplistArray		complistarray;    // this is the device - mapping
static int						complistcnt;

static CompPinInstArray		comppininstarray; 
static int						comppininstcnt;

//static CKTNAVPadstackArray	padstackarray; 
//static int						padstackcnt;

static CKTNAVRoutesArray	routesarray;   
static int						routescnt;

static CCKTNAVLayerArray	glArr;
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
static int						convert_tp_comps = TRUE; // convert testpoints to component and create a shape with the 
															       // padstack as it's contents.

static CString					testpointshapeprefix;
static CString					generic_insert;
static CString					mechanical_insert;
static CString					unknown_insert;
static bool						do_normalize_bottom_build_geometries;


static void GatherPadstacksAndRoutesData(CCEtoODBDoc* pDoc, FileStruct* pPCBFile, CCKTNAVPadstackMap& padstackMap);
static int CKTNAV_RotatedPadstackData(CDataList *DataList, DbUnit insertAngle = 0., CCKTNAVPadstackMap* padstackMap = NULL);

static int CKTNAV_WritePADSTACKData(FILE *wfp, const CString padstkname, CDataList *DataList, double rotation, int mirror,
		double scale, int embeddedLevel, int insertLayer, bool toppad, bool dupMirPadstack);

// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void CKTNAV_WritePRIMARYBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, GraphicClassTag graphic_class);
// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM
static void CKTNAV_WriteBOARDData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, int layertype);
// this function only loops throu entities marked other than BL_CLASS_BOARD 
static void CKTNAV_WriteARTWORKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel);
// this function only loops throu entities marked other than BL_CLASS_BOARD 
static long CKTNAV_WriteSHAPEData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,
      int artwork_or_shape, long acnt, int from_generic_component = FALSE);

static long CKTNAV_WriteComplexApertureData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int apptyp);

static int CKTNAV_TestComplexApertureData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel);

static void CKTNAV_WriteCOMPONENTData(FILE* wfp, FileStruct* file, BlockStruct* fileBlock,
      double insert_x, double insert_y, double rotation, int mirror, double scale,
		int embeddedLevel, int insertLayer, CCKTNAVDeviceMap& deviceMap);

static void CKTNAV_WriteMECHData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int panelfile, int insertlevel);

static void CKTNAV_WriteTestProbeData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int panelfile, int insertLayer);

static void CKTNAV_WriteROUTESData(FILE *fp, FileStruct *file, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname, CCKTNAVPadstackMap& padstackMap);

static void CKTNAV_GetROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static void CKTNAV_WritePANELBOARDSData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertlevel, 
      const char *fname, const char *ext);

static void CKTNAV_WriteTESTPOINTData(FILE *wfp, CDataList *DataList, const char *netname);
static void write_pcbshapes(FILE *fp, double scale, int artwork_or_shape, long *acnt, int *shapeattcnt,  BlockStruct *block);
static void write_mech_shape(FILE *fp, double scale, CDataList *DataList, long *acnt, int *shattcnt, int IsArtwork);

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


	return name;
}

/****************************************************************************/
/*
   return NULL if not visible
*/
static const char *Layer_CKTNAV(int l)
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
static int load_CKTNAVsettings(const CString fname, int page_units)
{
   FILE  *fp;
   char  line[127];
   char  *lp;

	write_name_check_log = false;
   fiducialname = "fiducial";
   panelfiducialname = "panelfid";
   testpinprefix = "tstpin_";
   defaultdrillsize = 0.01 * Units_Factor(UNIT_INCHES, page_units);
   defaultnopadsize = 0.01 * Units_Factor(UNIT_INCHES, page_units);
   convert_tp_comps = TRUE;
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

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage(tmp,"CKTNAV Settings", MB_OK | MB_ICONHAND);
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
               fprintf(navLogFp, "Too many .COMPONENT_OUTLINE\n");
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
			/// Support for .CONVERT_TP_COMPS is disabled.
			/// Output of NAILLOC is implemented such that .CONVERT must be TRUE.
			/// Can do more work on NAILLOC if this option is desired. 
			/// What is needed is to treat testpoints like VIAS if 
			/// .CONVERT is FALSE.
         //else if (!STRCMPI(lp,".CONVERT_TP_COMPS")) 
         //{
         //   if ((lp = get_string(NULL," \t\n")) == NULL) 
         //      continue;
         //   if (lp[0] == 'Y' || lp[0] == 'y')
         //      convert_tp_comps = TRUE;
         //}
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
         else if (!STRCMPI(lp,".WRITE_NAME_CHECK_LOG"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) 
               continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
               write_name_check_log = true;
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

      l = Layer_CKTNAV(layer->getLayerIndex());
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
                                const char *CKTNAVname, int *attcnt )
{

#ifndef  CASE_1967_OUTPUT_NO_ATTRIBUTES

   if (map == NULL)
      return;

   WORD keyword;
   void *voidPtr;
   Attrib   *a;
   int   CKTNAVcnt = *attcnt;

   POSITION pos = map->GetStartPosition();
   while (pos != NULL)
   {
      map->GetNextAssoc(pos, keyword, voidPtr);

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

      a = (Attrib*)voidPtr;

      if (a->isInherited()) 
         continue;

      char *tok, *temp = strdup(get_attvalue_string(doc, a));

      if ((tok = strtok(temp,"\n")) == NULL)
      {
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"??\"\n", 
            CKTNAVname, ++CKTNAVcnt, doc->KeyWordArray[keyword]->out);  // multiple values are delimited by \n
      }
      else
      {
         while (tok)
         {
            fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
               CKTNAVname, ++CKTNAVcnt, doc->KeyWordArray[keyword]->out,tok); // multiple values are delimited by \n
            tok = strtok(NULL,"\n");
         }
      }

      free(temp);

      // here kill mask and layer
      wGENCAD_Graph_Layer(fp, "");
      wGENCAD_Graph_MaskLevel(fp, "");
   }

   *attcnt = CKTNAVcnt;
   return;

#endif
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
static int CKTNAVPinNameCompareFunc( const void *arg1, const void *arg2 )
{
   CKTNAVCompPinInst **a1, **a2;
   a1 = (CKTNAVCompPinInst**)arg1;
   a2 = (CKTNAVCompPinInst**)arg2;

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
static void CKTNAV_SortPinData(const char *s)
{
   int   done = FALSE;
   int   sortcnt = 0;

   if (comppininstcnt < 2) 
      return;

   qsort(comppininstarray.GetData(), comppininstcnt, sizeof(CKTNAVCompPinInst *), CKTNAVPinNameCompareFunc);
   return;
}

/******************************************************************************
* write_componentattributes
*/
static void write_componentattributes(FILE *fp, CAttributes* map, const CString CKTNAVname, int *attcnt,
                                FileStruct *file, CCKTNAVDevice* device)
{
	// Dual purpose, also gathers attribs to save as device parameters for later output
   if (map == NULL)
      return;

   int CKTNAVcnt = *attcnt;
   
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

      //char* temp = strdup(get_attvalue_string(doc, attrib));
      //char* tok = strtok(temp, "\n");
      while (!tok.IsEmpty())
      {
			CString outKeyword = doc->getKeyWordArray()[keyword]->out;
			CString ccKeyword = doc->getKeyWordArray()[keyword]->cc;

#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES
			// Write any attrib but ddlink
         if (outKeyword.CompareNoCase(ATT_DDLINK) != 0)
         {
            fprintf(fp, "ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
						CKTNAVname, ++CKTNAVcnt, outKeyword, tok);
         }
#endif
         
			// Save device attribs
			{
				tok = do_string(tok);

				if (!outKeyword.CompareNoCase(ATT_VALUE))
				{
					ComponentValues componentValue(tok);
					double numericValue = componentValue.GetValue();
					CString valStr = fpfmtExactPrecision(numericValue,3);
					device->SetValue(valStr);
				}

				if (!outKeyword.CompareNoCase("TOL") || !outKeyword.CompareNoCase(ATT_TOLERANCE))
					device->SetTolerance(tok);

				if (!outKeyword.CompareNoCase("NTOL")|| !outKeyword.CompareNoCase(ATT_MINUSTOLERANCE))
					device->SetMinusTol(tok);

				if (!outKeyword.CompareNoCase("PTOL")|| !outKeyword.CompareNoCase(ATT_PLUSTOLERANCE))
					device->SetPlusTol(tok);

				if (!outKeyword.CompareNoCase(ATT_DEVICETYPE))
					device->SetType(tok);

				if (!outKeyword.CompareNoCase(ATT_SUBCLASS))
					device->SetStyle(tok);
			}

			tok = attribValue.Tokenize("\n", curPos); 
         //tok = strtok(NULL, "\n");
      }

      //free(temp);
   } 

   wGENCAD_Graph_Layer(fp, "");
   wGENCAD_Graph_MaskLevel(fp, "");

   *attcnt = CKTNAVcnt;

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
                                const char *CKTNAVname, int *attcnt )
{
#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES

   if (map == NULL)
      return;

   WORD keyword;
   void *voidPtr;
   Attrib   *a;
   int   CKTNAVcnt = *attcnt;

   POSITION pos = map->GetStartPosition();
   while (pos != NULL)
   {
      map->GetNextAssoc(pos, keyword, voidPtr);

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

      a = (Attrib*)voidPtr;

      // do not write the pin attribute, if it was inhereted from the padstack.
      if (a->isInherited()) 
         continue;

      switch (a->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %d\n", 
               CKTNAVname, ++CKTNAVcnt, doc->KeyWordArray[keyword]->out, a->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %1.*lf\n", 
               CKTNAVname, ++CKTNAVcnt, doc->KeyWordArray[keyword]->out, output_units_accuracy, a->getDoubleValue());
         break;
      case VT_STRING:
         {
            if (a->getStringValueIndex() < 0)
               break;
            char *tok, *temp = strdup(get_attvalue_string(doc, a));
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               if (a->getStringValueIndex() != -1)
                  fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
                     CKTNAVname, ++CKTNAVcnt, doc->KeyWordArray[keyword]->out,tok); // multiple values are delimited by \n
               else
                  fprintf(fp,"ATTRIBUTE %s_%d %s ?\n", 
                     CKTNAVname, ++CKTNAVcnt, doc->KeyWordArray[keyword]->out);
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

   *attcnt = CKTNAVcnt;
   return;

#endif
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
         CKTNAV_RotatedPadstackData( &(block->getDataList()));
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
				fprintf(fp,"PAD %s ROUND -1\n", gen_string(padname));
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
            int apptyp = CKTNAV_TestComplexApertureData(fp, &(subblock->getDataList()), 0.0, 0.0, 
               block->getRotation()+rotation, 0, scale, 0);

            if (apptyp == PADSHAPE_CIRCLE)
               fprintf(fp,"PAD %s ROUND -1\n",gen_string(padname));
            else if (apptyp == PADSHAPE_RECTANGULAR)
               fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
            else
               fprintf(fp,"PAD %s POLYGON -1\n",gen_string(padname));

            CKTNAV_WriteComplexApertureData(fp, &(subblock->getDataList()), 0.0, 0.0, 
               block->getRotation()+rotation, 0, scale, 0, apptyp);
         }
         break;

      default:
         {
            fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
            fprintf(fp,"RECTANGLE 0.0 0.0 1.0 1.0\n");

				fprintf(navLogFp, "Unknown Aperture [%d] for Geometry [%s]\n", block->getShape(), block->getName());
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
      int apptyp = CKTNAV_TestComplexApertureData(fp, &(block->getDataList()), 0.0, 0.0, rotation, 0, scale, 0);

      if (apptyp == PADSHAPE_CIRCLE)
         fprintf(fp,"PAD %s ROUND -1\n",gen_string(padname));
      else if (apptyp == PADSHAPE_RECTANGULAR)
         fprintf(fp,"PAD %s RECTANGULAR -1\n",gen_string(padname));
      else
         fprintf(fp,"PAD %s POLYGON -1\n",gen_string(padname));

      CKTNAV_WriteComplexApertureData(fp, &(block->getDataList()), 0.0, 0.0, rotation, 0, scale, 0, apptyp);
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

      // this is done so that I can write rotated PADS which CKTNAV can not do.
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
static int do_padstacks(FILE *fp, double scale, CCKTNAVPadstackMap& padstackMap)
{
   int padstackattcnt = 0;
	POSITION pos = padstackMap.GetStartPosition();
	while (pos != NULL)
	{
		CString padstackName;
		CCKTNAVPadstack* padstack = NULL;
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
         CKTNAV_WritePADSTACKData(fp, padstackName, &block->getDataList(), 0.0, 0, scale, 0, padstack->GetInsertLayer(), true, false);
         write_attributes(fp, block->getAttributesRef(), "PADST", &padstackattcnt);


			// Write the mirrored version of the padstack if there it needs one
			if (padstack->GetNeedMirVersion())
			{
				padstackName = gen_string(padstack->GetMirName());
				fprintf(fp, "PADSTACK %s %1.*lf\n", padstackName, output_units_accuracy, drill * scale);

				// top pad only
				CKTNAV_WritePADSTACKData(fp, padstackName, &block->getDataList(), 0.0, 0, scale, 0, padstack->GetInsertLayer(), true, true);
				write_attributes(fp, block->getAttributesRef(), "PADST", &padstackattcnt);
			}

         // support bondpad rotation
         for (int ii=0; ii < padrotarray.GetCount(); ii++)
         {
            CString padrotName = (padrotarray.GetAt(ii))?padrotarray.GetAt(ii)->padname:"";
            if (!padrotName.Compare(padstackName))
            {
               fprintf(fp, "PADSTACK %s_%d %1.*lf\n", padstackName, padrotarray.GetAt(ii)->rotation, output_units_accuracy, drill * scale);
               CKTNAV_WritePADSTACKData(fp, padstackName , &block->getDataList(), DegToRad(padrotarray[ii]->rotation), 0, scale, 0, padstack->GetInsertLayer(), true, false);
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
      CKTNAV_WriteTESTPOINTData(wfp, &(file->getBlock()->getDataList()), netname);

   return 1;
}

static void write_pcbshapes(FILE *fp, double scale, int artwork_or_shape, long *acnt, int *shapeattcnt,  BlockStruct *block)
{
   if (artwork_or_shape)
      fprintf(fp,"SHAPE %s\n", gen_string(block->getName()));

   comppininstarray.SetSize(100,100);
   comppininstcnt = 0;

   *acnt = CKTNAV_WriteSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1,
      artwork_or_shape, *acnt);

   CKTNAV_SortPinData(block->getName());

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
			CString value = attrib->getStringValue();
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

static DataStruct *getTestProbeData(FileStruct *file, CString net_name, int lookingForEntityNum)
{
	if (file != NULL)
	{
		BlockStruct *fileBlock = file->getBlock();

		if (fileBlock != NULL)
		{
			WORD placedProbeKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
			WORD netNameKW = doc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);
			WORD dlinkKW = doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);

			POSITION pos = fileBlock->getHeadDataInsertPosition();
			while (pos != NULL)
			{
				DataStruct *data = fileBlock->getNextDataInsert(pos);

				if (data != NULL && data->isInsertType(insertTypeTestProbe))
				{
					InsertStruct *insert = data->getInsert();

					Attrib *a = NULL;
					CString placedAttr;
					if (data->lookUpAttrib(placedProbeKW, a))
						placedAttr = get_attvalue_string(doc, a);

					if (placedAttr == "Placed")
					{
						a = NULL;
						CString netnameAttr;
						if (data->lookUpAttrib(netNameKW, a))
							netnameAttr = get_attvalue_string(doc, a);

						if (netnameAttr.CompareNoCase(net_name) == 0)
						{
							CString featureType, targetName;

							if (data->lookUpAttrib(dlinkKW, a))
							{
								// Get access marker
                        CEntity probedEntity = CEntity::findEntity(doc->getCamCadData(), a->getIntValue());

								if (probedEntity.getEntityType() == entityTypeData)
								{
									DataStruct* ta = probedEntity.getData();

									// Get probed target
									if ( ta != NULL && ta->lookUpAttrib(dlinkKW, a))
									{
                              CEntity accessEntity = CEntity::findEntity(doc->getCamCadData(), a->getIntValue());

										if (accessEntity.getEntityType() == entityTypeData)
										{
											DataStruct* feature = accessEntity.getData();
											if (feature != NULL && feature->isInsertType(insertTypeVia))
											{
												// featureType = "VIA";
												// targetName = feature->getInsert()->getRefname());
												if (feature->getEntityNumber() == lookingForEntityNum)
												return data;
											}
											else
											{
												// Only vias should be "entityTypeData", everything else should
												// be compPin. Here is something that is neither.
												fprintf(navLogFp, "getTestProbeData(): Unexpected target insert type [%s] for refname [%s]\n",
													insertTypeToString(feature->getInsert()->getInsertType()), 
													feature->getInsert()->getRefname());
												display_error++;
											}
										}
										else if (accessEntity.getEntityType() == entityTypeCompPin)
										{
											// featureType = "PIN";
											// targetName = accessEntity.getCompPin()->getPinRef('-');

											CompPinStruct *compPin = accessEntity.getCompPin();
											int cpEntityNum = compPin->getEntityNumber();

											if (cpEntityNum == lookingForEntityNum)
												return data;
										}

									}
								}
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}

//--------------------------------------------------------------

static DataStruct *getTestProbeData(FileStruct *file, CString netName, DataStruct *featureData)
{
	if (featureData != NULL)
	{
		int lookingForEntityNum = featureData->getEntityNumber();
		return getTestProbeData(file, netName, lookingForEntityNum);
	}
	return NULL;
}

//--------------------------------------------------------------

static DataStruct *getTestProbeData(FileStruct *file, CString netName, CompPinStruct *compPin)
{
	if (compPin != NULL)
	{
		int lookingForEntityNum = compPin->getEntityNumber();
		return getTestProbeData(file, netName, lookingForEntityNum);
	}
	return NULL;
}

//--------------------------------------------------------------

static void writeProbes(FILE *fp, FileStruct *file,
							 double file_insert_x, double file_insert_y,
							 double file_rotation, int file_mirror, double file_scale,
							 CString net_name, bool vias)
{
	// Write NAILLOC recrods for probes.
	// If vias==true, do only vias, if false then do everything but vias.

	if (fp != NULL && file != NULL)
	{
		BlockStruct *fileBlock = file->getBlock();

		if (fileBlock != NULL)
		{
			WORD placedProbeKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, VT_STRING);
			WORD netNameKW = doc->RegisterKeyWord(ATT_NETNAME, 0, VT_STRING);
			WORD dlinkKW = doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);

			POSITION pos = fileBlock->getHeadDataInsertPosition();
			while (pos != NULL)
			{
				DataStruct *probeData = fileBlock->getNextDataInsert(pos);

				if (probeData != NULL && probeData->isInsertType(insertTypeTestProbe))
				{
					InsertStruct *insert = probeData->getInsert();

					Attrib *a = NULL;
					CString placedAttr;
					if (probeData->lookUpAttrib(placedProbeKW, a))
						placedAttr = get_attvalue_string(doc, a);

					if (placedAttr == "Placed")
					{
						a = NULL;
						CString netnameAttr;
						if (probeData->lookUpAttrib(netNameKW, a))
							netnameAttr = get_attvalue_string(doc, a);

						if (netnameAttr.CompareNoCase(net_name) == 0)
						{
							// CString featureType, targetName;

							if (probeData->lookUpAttrib(dlinkKW, a))
							{
								// Get access marker
                        CEntity probedEntity = CEntity::findEntity(doc->getCamCadData(), a->getIntValue());

								if (probedEntity.getEntityType() == entityTypeData)
								{
									DataStruct* ta = probedEntity.getData();

									// Get probed target
									if ( ta != NULL && ta->lookUpAttrib(dlinkKW, a))
									{
                              CEntity accessEntity = CEntity::findEntity(doc->getCamCadData(), a->getIntValue());

										if (accessEntity.getEntityType() == entityTypeData)
										{
											DataStruct* feature = accessEntity.getData();
											if (feature != NULL && feature->isInsertType(insertTypeVia) && vias)
											{
												// featureType = "VIA";
												// targetName = feature->getInsert()->getRefname();

												/// This section is currently not in actual use, vias are 
												/// handled on the fly elsewhere. This is because of a problem
												/// with via numbers aka refnames. Refnames are not reliably
												/// set, and other output section numbers vias on the fly.
												/// See CKTNAV_WriteROUTESData().

												// Write NAILLOC
												Mat2x2 m;
												RotMat2(&m, file_rotation);

												Point2 point2;
												point2.x = probeData->getInsert()->getOriginX() * file_scale;
												if (file_mirror)
													point2.x = -point2.x;

												point2.y = probeData->getInsert()->getOriginY() * file_scale;
												TransPoint2(&point2, 1, &m, file_insert_x, file_insert_y);

												fprintf(fp, "NAILLOC via%d %s", viacnt, gen_string(check_name('c', probeData->getInsert()->getRefname(), DO_NMCHK_LOG)));
												fprintf(fp, " %1.*lf %1.*lf",	output_units_accuracy, point2.x, output_units_accuracy, point2.y);
												fprintf(fp, " -1 -1 100"); /* tester assigned number, tester interface number, probe size */
												fprintf(fp, " %s\n", probeData->getInsert()->getPlacedBottom() ? "BOTTOM" : "TOP");
											}									
										}
										else if (accessEntity.getEntityType() == entityTypeCompPin && !vias)
										{
											// featureType = "PIN";
											// targetName = accessEntity.getCompPin()->getPinRef('-');

											CompPinStruct *compPin = accessEntity.getCompPin();
											int cpEntityNum = compPin->getEntityNumber();
                                 
                                 if(!compPin->IsDiePin(doc->getCamCadData()))
                                 {
											   // Write NAILLOC
											   Mat2x2 m;
											   RotMat2(&m, file_rotation);

											   Point2 point2;
											   point2.x = probeData->getInsert()->getOriginX() * file_scale;
											   if (file_mirror)
												   point2.x = -point2.x;

											   point2.y = probeData->getInsert()->getOriginY() * file_scale;
											   TransPoint2(&point2, 1, &m, file_insert_x, file_insert_y);

											   fprintf(fp, "NAILLOC %s %s %s %1.*lf %1.*lf %d %d %d %s\n",
												   check_name('c', compPin->getRefDes(), DO_NMCHK_LOG), compPin->getPinName(), probeData->getInsert()->getRefname(),
												   output_units_accuracy, point2.x, output_units_accuracy, point2.y,
												   -1, -1, 100, /* tester assigned number, tester interface number, probe size */
												   probeData->getInsert()->getPlacedBottom() ? "BOTTOM" : "TOP");
                                 }
                                 else //Skip Die Pins
                                 {
                                    fprintf(navLogFp, "$SIGNAL Net: %s CompPin %s - Skipped CompPin for Die component.\n", 
                                       net_name, compPin->getPinRef());                                    
                                 }
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------
static int do_signals(FILE *fp, FileStruct *file,
							 double file_insert_x, double file_insert_y,
							 double file_rotation, int file_mirror, double scale)

{
	CNetList *NetList = &file->getNetList();
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   int      comppinattcnt = 0;
   
	WORD cktNetnameKw = doc->RegisterKeyWord(ATT_CKT_NETNAME, 0, VT_STRING);

   fprintf(fp,"$SIGNALS\n");

#ifdef DEBUG_DUPS
	//*rcf report dup mappings for debug --------------------
   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

		CString checkedName = check_name('n', net->getNetName(), DO_NMCHK_LOG);

		POSITION netPos2 = NetList->GetHeadPosition();
		while (netPos2 != NULL)
		{
			NetStruct *net2 = NetList->GetNext(netPos2);

			if (net2->getFlags() & NETFLAG_UNUSEDNET)
				continue;

			CString checkedName2 = check_name('n', net2->getNetName(), DO_NMCHK_LOG);

			if (checkedName.Compare(checkedName2) == 0 && net->getNetName().Compare(net2->getNetName()) != 0)
			{
				fprintf(fp, "*** Checked name (%s) for (%s) and (%s)\n", checkedName, net->getNetName(), net2->getNetName());

			}

		}
	}
	//*rcf ---------------------------------------------------
#endif

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

		CString checkedName = check_name('n', net->getNetName(), DO_NMCHK_LOG);
      fprintf(fp, "SIGNAL %s\n", gen_string(checkedName));

		// Save checked name as attrib on net
      net->setAttrib(doc->getCamCadData(), cktNetnameKw, valueTypeString, checkedName.GetBuffer(0), attributeUpdateOverwrite, NULL);
	
      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();

      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (is_component(compPin->getRefDes())) // check if is was not an converted FIDUCIAL or TOOL
         {
            fprintf(fp, "NODE %s", gen_string(check_name('c', compPin->getRefDes(), DO_NMCHK_LOG)));
            fprintf(fp, " %s\n", gen_string(compPin->getPinName()));
////#define COMPPIN_NAILLOC_ON_THE_FLY
/// In particular test case, on-the-fly took 7 secs while "batched" takes only 2 secs
#ifdef COMPPIN_NAILLOC_ON_THE_FLY
				DataStruct *probeData = getTestProbeData(file, net->getNetName(), compPin);
				if (probeData != NULL)
				{
					Mat2x2 m;
					RotMat2(&m, file_rotation);

					Point2 point2;
					point2.x = probeData->getInsert()->getOriginX() * scale;
					if (file_mirror)
						point2.x = -point2.x;

					point2.y = probeData->getInsert()->getOriginY() * scale;
					TransPoint2(&point2, 1, &m, file_insert_x, file_insert_y);

					fprintf(fp, "NAILLOC %s %s %s %1.*lf %1.*lf %d %d %d %s\n",
					check_name('c', compPin->getRefDes(), DO_NMCHK_LOG), compPin->getPinName(), probeData->getInsert()->getRefname(),
					output_units_accuracy, point2.x, output_units_accuracy, point2.y,
					-1, -1, 100, /* tester assigned number, tester interface number, probe size */
					probeData->getInsert()->getPlacedBottom() ? "BOTTOM" : "TOP");
				}
#endif
            write_attributes(fp, compPin->getAttributesRef(), "COMPPIN", &comppinattcnt);
         }
      }

      // here assign testpoints to the nets
      do_netlist_testpoints(fp,file, net->getNetName());

#ifndef COMPPIN_NAILLOC_ON_THE_FLY
		// Do NAILLOCs all in a batch
		writeProbes(fp, file, file_insert_x, file_insert_y, file_rotation, file_mirror, scale,
							 net->getNetName(), false /*no  vias*/);
#endif
   }

   fprintf(fp,"$ENDSIGNALS\n\n");

   return 1;
}

/******************************************************************************
* do_routes
*/
static int do_routes(FILE *fp, FileStruct *file, CNetList *NetList, CCKTNAVPadstackMap& padstackMap)
{   
   fprintf(fp,"$ROUTES\n");

   //CKTNAV_GetROUTESData(fp, &(file->getBlock()->getDataList()), 
   //      file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
   //      file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   POSITION netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      NetStruct* net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)
         continue;

      fprintf(fp, "ROUTE %s\n", gen_string(check_name('n', net->getNetName(), DO_NMCHK_LOG)));
      wGENCAD_Graph_WidthCode(fp,-1);
      wGENCAD_Graph_Layer(fp,"");  

      // order by signal name
      CKTNAV_WriteROUTESData(fp, file, &(file->getBlock()->getDataList()), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor,
				0, -1, check_name('n', net->getNetName(), DO_NMCHK_LOG), padstackMap);
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

      if (doc->IsFloatingLayer(layer->getLayerIndex()))  // no floating layers, they should never be in a CKTNAV file.
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
            //ErrorMessage(t, "Layer Display", MB_ICONEXCLAMATION | MB_OK);
         }
      }

      //glArr.SetSizes
      CKTNAVLayerStruct *gl = new CKTNAVLayerStruct;
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
   CKTNAVLayerDlg gldlg;

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
                           netName.GetBuffer(0), attributeUpdateOverwrite, NULL); // x, y, rot, height

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

static void CKTNAV_WritePNLFile(CString filename)
{

   CFilePath ccFilePath(filename);
   CString drive = ccFilePath.getDrive();
   CString dir = ccFilePath.getDirectory();
   CString fname = ccFilePath.getBaseFileName();
   CString ext = ccFilePath.getExtension();
	FILE *wfp;
   bool   panel_found = false;

	switch_on_pcbfiles_used_in_panels(doc, FALSE);  // set show

   ccFilePath.setExtension("pnl");

   CString f = ccFilePath.getPath();
  
	if ((wfp = fopen(f, "wt")) == NULL)
	{
		// error - can not open the file.
		CString  tmp;
		tmp.Format("Can not open %s",f);
		ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
		return;
	}

	// reset status for Panel file 
	progress->SetStatus(f);

	wGENCAD_Graph_Init_Layout(output_units_accuracy);
	wGENCAD_Graph_File_Open_Layout();

	FileStruct *file = NULL;
	POSITION pos = doc->getFileList().GetHeadPosition();
	while (pos != NULL)
	{
		file = doc->getFileList().GetNext(pos);
		if (file->getBlockType() == BLOCKTYPE_PANEL)   
		{
         if (file->isShown())
         {
			   panel_found = true;
			   int   panelattcnt = 0;

			   // header
			   wGENCAD_Graph_Header(wfp, "GENPANEL", 1.4, file->getName());

			   fprintf(wfp,"ATTRIBUTE /GRA3/ PROCESSED_BY GRA3\n"); // Case 1967 calls for this fixed attribute

			   // here all attributes
			   write_attributes(wfp, file->getBlock()->getAttributesRef(), "PANEL", &panelattcnt);
			   fprintf(wfp,"$ENDHEADER\n\n");

			   // panel
			   // board
			   fprintf(wfp,"$PANEL\n");

			   // all graphic structures 
			   CKTNAV_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
				   file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				   file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
				   graphicClassPanelOutline);

			   // all graphic structures 
			   CKTNAV_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
				   file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				   file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
				   graphicClassPanelCutout);

			   // other graphic structure
			   CKTNAV_WriteBOARDData(wfp, &(file->getBlock()->getDataList()), 
				   file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				   file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 0);
			   fprintf(wfp,"$ENDPANEL\n\n");

			   // artworks
			   fprintf(wfp,"$ARTWORKS\n");
			   fprintf(wfp,"$ENDARTWORKS\n\n");

			   // mech
			   fprintf(wfp,"$MECH\n");
			   CKTNAV_WriteMECHData(wfp, &(file->getBlock()->getDataList()), 
				   file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				   file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, TRUE, -1);  // panel file
			   fprintf(wfp,"$ENDMECH\n\n");

			   // boards
			   fprintf(wfp,"$BOARDS\n");
			   CKTNAV_WritePANELBOARDSData(wfp,&(file->getBlock()->getDataList()), 
				   file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				   file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
				   fname,ext);

			   fprintf(wfp,"$ENDBOARDS\n\n");
		   }
      }
	}

	if (!panel_found)
	{
		// create an empty panel
		panel_found = true;
		// header
		wGENCAD_Graph_Header(wfp, "GENPANEL", 1.4, file != NULL ? file->getName() : "UnNamed");
		fprintf(wfp,"ATTRIBUTE /GRA3/ PROCESSED_BY GRA3\n"); // Case 1967 calls for this fixed attribute
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
		fprintf(wfp,"FILE %s.%s\n", fname, ext);
		fprintf(wfp,"FIDUCIAL 0.0 0.0\n");
		fprintf(wfp,"ROTATION 0.0\n");
		fprintf(wfp,"PLACE 0.0 0.0\n");

		fprintf(wfp,"$ENDBOARDS\n\n");
	}

	wGENCAD_Graph_File_Close_Layout();
	// close write file
	fclose(wfp);
}

static void CKTNAV_WriteNAVFile(CString filename)
{
	CWaitCursor wait;

	FileStruct *file;
   bool   pcb_found = false;

	FILE *wfp;

	// open file for writting
	if ((wfp = fopen(filename, "wt")) == NULL)
	{
		// error - can not open the file.
		CString  tmp;
		tmp.Format("Can not open %s",filename);
		ErrorMessage(tmp,"Error File open", MB_OK | MB_ICONHAND);
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
			pcb_found = true;

			// header
			wGENCAD_Graph_Header(wfp,"GENCAD", 1.4, file->getName());
			fprintf(wfp,"ATTRIBUTE /GRA3/ PROCESSED_BY GRA3\n"); // Case 1967 calls for this fixed attribute
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
			CKTNAV_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
				graphicClassBoardOutline);

			// here all board cutout
			CKTNAV_WritePRIMARYBOARDData(wfp, &(file->getBlock()->getDataList()), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
				graphicClassBoardCutout);

			CKTNAV_WriteBOARDData(wfp, &(file->getBlock()->getDataList()), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1, 
				LAYTYPE_BOARD_OUTLINE);

			fprintf(wfp,"$ENDBOARD\n\n");


			CCKTNAVPadstackMap padstackMap;
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
					CCKTNAVPadstack* padstack = NULL;
					padstackMap.GetNextAssoc(pos, padstackName, padstack);
					if (padstack == NULL || !padstack->GetUseAsTestComp())
						continue;

					padstackName = gen_string(padstackName);
					fprintf(wfp, "SHAPE %s%s\n", testpointshapeprefix, padstackName);
					fprintf(wfp, "PIN 1 %s 0 0 TOP 0.0 0\n", padstackName);
#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES
					fprintf(wfp,"ATTRIBUTE SHAPE_%d \"%s\" \"%s\"\n", ++sattcnt, "USERSHAPE","SHAPE created from PADSTACK"); 
#endif
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
			CCKTNAVDeviceMap deviceMap;
			CKTNAV_WriteCOMPONENTData(wfp, file, file->getBlock(), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor,
				0, -1, deviceMap);
			fprintf(wfp,"$ENDCOMPONENTS\n\n");
			deviceMap.WriteDevice(wfp);

			// signals
			do_signals(wfp, file, 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor);

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
			CKTNAV_WriteMECHData(wfp,&(file->getBlock()->getDataList()), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);  // not a panel
			fprintf(wfp,"$ENDMECH\n\n");

			// testpins            
			fprintf(wfp,"$TESTPINS\n");
#ifdef WRITE_TESTPINS_DATA	// .NAV output spec says don't write this section
			CKTNAV_WriteTestProbeData(wfp,&(file->getBlock()->getDataList()), 
				file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
				file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);  // not a panel
#endif
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
		ErrorMessage("No PCB file found !","CKTNAV Output", MB_OK | MB_ICONHAND);
		return;
	}


}

//--------------------------------------------------------------
void Teradyne228xNAV_WriteFiles(CString argFilename, CCEtoODBDoc *Doc, FormatStruct *format,
                     int PageUnits)
{
   doc = Doc;
   display_error = FALSE;
   viacnt = 0;
   testpadcnt = 0;
   bondpadcnt = 0;

   //bool run_silent = true; // no layer dialog

   // Set Static variables so don't have to pass them every time
   unitsFactor = Units_Factor(PageUnits, UNIT_MILS);


   routesarray.SetSize(100,100);
   routescnt = 0;

   padrotarray.SetSize(10,10);
   padrotcnt = 0;

   complistarray.SetSize(100,100);
   complistcnt = 0;

   cur_artworkcnt = 0;

   glArr.SetSize(10,10);
   maxArr = 0;

	// Ensure .nav extension. Output file chooser is (was, at least) shared with
	// CKT, and we get .ckt extension here by default.
	CString filename = argFilename;
	int dotIndx = filename.ReverseFind('.');
	if (dotIndx + -1)
		filename.Truncate(dotIndx);
	filename += ".nav";

   // Open log file.
   CString localLogFilename;
   navLogFp = getApp().OpenOperationLogFile("cktnav.log", localLogFilename);
   if (navLogFp == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(navLogFp, "Teradyne 228x NAV");
   
   output_units_accuracy = GetDecimals(PageUnits); 

   CString settingsFile( getApp().getExportSettingsFilePath("CKTNAV.out") );
   {
      CString msg;
      msg.Format("Teradyne 228x NAV: Settings file [%s].\n\n", settingsFile);
      getApp().LogMessage(msg);
   }
   load_CKTNAVsettings(settingsFile, UNIT_MILS);   // output always in MILS

   CString checkFile( getApp().getSystemSettingsFilePath("cktnav.chk") );
	check_init(checkFile);

   switch_on_pcbfiles_used_in_panels(doc, TRUE);   // set show

	if (do_normalize_bottom_build_geometries)
		doc->OnToolsNormalizeBottomBuildGeometries();

   do_layerlist();

   //if (run_silent || edit_layerlist())  Don't really need this if, but in case edit_layerlist comes back, this is where it goes.
   {
		CKTNAV_WriteNAVFile(filename);

		if (format->CreatePanelFile)
			CKTNAV_WritePNLFile(filename);			
   }

	if (write_name_check_log)
	{
		fprintf(navLogFp, "\n");
		if (check_report(navLogFp))
			display_error++;
	}

	check_deinit();

   free_layerlist();

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

   // Close log file only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!localLogFilename.IsEmpty())
   {
      fclose(navLogFp);

      // We want this to happen only for local log files, not when
      // system log file is in play.
      if (display_error)
         Logreader(localLogFilename);
   }

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
void CKTNAV_WriteBOARDData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int layertype)
{
   Mat2x2 m;
   DataStruct *np;
   int layer;
   const char  *l="";

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

         if ((l = Layer_CKTNAV(layer)) == NULL) 
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

               if (!poly->isHidden())
               {
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
               }
            }  // while
            //wGENCAD_Graph_Rectangle( points[0].x, points[0].y, points[2].x, points[2].y);
         }
         break;  // POLYSTRUCT

      case T_TEXT:
         {
            if (output_graphic_data)
            {
               Point2 point2;
               point2.x = (np->getText()->getPnt().x)*scale;
               point2.y = (np->getText()->getPnt().y)*scale;
               TransPoint2(&point2, 1, &m,insert_x,insert_y);

               double text_rot = rotation + np->getText()->getRotation();

               int text_mirror;
               // text is mirrored if mirror is set or text.mirror but not if none or both
               text_mirror = np->getText()->isMirrored();
               fprintf(wfp,"ARTWORK artwork%ld %s\n",++cur_artworkcnt,l);
               wGENCAD_Graph_Text(wfp, np->getText()->getText(), point2.x, point2.y,
                                 np->getText()->getHeight()*scale,np->getText()->getWidth()*scale,
                                 text_rot, text_mirror, l);
               //write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt); 
            }
            /*if (output_graphic_data)
               CKTNAV_WriteARTWORKData(wfp, DataList,insert_x,insert_y,rotation,
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
               
               CKTNAV_WriteARTWORKData(wfp, &block->getDataList(),np->getInsert()->getOriginX()*scale,np->getInsert()->getOriginY()*scale,np->getInsert()->getAngle(),
                  np->getInsert()->getMirrorFlags(), scale,0,np->getLayerIndex());            
            }
         }
         break;
      } // end switch
   } // end CKTNAV_WriteBOARDData */
}

//--------------------------------------------------------------
void CKTNAV_WritePRIMARYBOARDData(FILE *wfp,CDataList *DataList, double insert_x, double insert_y,
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

			if (Layer_CKTNAV(layer) == NULL) 
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

         if (Layer_CKTNAV(layer) == NULL) 
            continue;

			if (graphic_class == graphicClassBoardOutline || graphic_class == graphicClassPanelOutline)
			{
				// If writing BoardOutline or PanelOutline then
				//   1) If hasOutlineGrClass is true, then only write out that poly of that graphic class
				//   2) If hasOutlineGrClass is false, then write poly on the layer type specified

				if (hasOutlineGrClass == false)
				{
					LayerStruct *ll = doc->FindLayer(layer);

               if ( ll!=NULL )
               {
					   if ( graphic_class == graphicClassBoardOutline && (ll->getLayerType() != layerTypeBoardOutline) )
						   continue;
					   else if ( graphic_class == graphicClassPanelOutline && (ll->getLayerType() != layerTypePanelOutline) )
						   continue;
               }
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
                        ErrorMessage("Marked as Rectangle is wrong !","CKTNAV Output", MB_OK | MB_ICONHAND);
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
            CKTNAV_WritePRIMARYBOARDData(wfp,&(block->getDataList()), point2.x, point2.y,
               block_rot, block_mirror, scale * np->getInsert()->getScale(), embeddedLevel+1, 
               block_layer, graphic_class);
         }
         break;
      }
   } 
}

//--------------------------------------------------------------
void CKTNAV_WriteARTWORKData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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

         if ((np->getFlags() & graphicClassEtch)       == graphicClassEtch)
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

                  int apptyp = CKTNAV_TestComplexApertureData(wfp, &(subblock->getDataList()), 0.0, 0.0, 
                        block->getRotation()+rotation, 0, scale, 0);

                  CKTNAV_WriteComplexApertureData(wfp,&(subblock->getDataList()), 
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
               CKTNAV_WriteARTWORKData(wfp, &block->getDataList(),point2.x, point2.y,block_rot,
                     mirror, scale,0,np->getLayerIndex());   
            }
         }

         break;
      } // end switch
   } // end CKTNAV_WriteARTWORKData */
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
long CKTNAV_WriteSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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
   LayerStruct *ll = NULL;

   wGENCAD_Graph_WidthCode(wfp, -1);
   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      ll = NULL; // reset ll

      if (!ignore_non_manufacturing_info(np))
      {

         if (np->getDataType() != T_INSERT)
         {
            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               layer = insertLayer;
            else
               layer = np->getLayerIndex();

            if (doc->get_layer_visible(layer, mirror))
            {
               if ((l = Layer_CKTNAV(layer)) != NULL)
               {
                  ll = doc->FindLayer(layer);
               }
            }

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

                  if (!poly->isHidden())
                  {
                     BOOL polyFilled = poly->isFilled();
                     BOOL closed     = poly->isClosed();

                     int first = TRUE;
                     Point2   firstp;
                     Point2   p1,p2;

                     if (artwork_or_shape)   // just collect the artwork section
                     {
                        if (np->getGraphicClass() == GR_CLASS_COMPOUTLINE || (ll != NULL && in_component_outline(ll->getLayerType())) || (from_generic_component && write_gen_comp_to_shape))
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
                                    ErrorMessage("Marked as Rectangle is wrong !","CKTNAV Output", MB_OK | MB_ICONHAND);
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
                        if (ll != NULL && in_component_outline(ll->getLayerType()))     
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
                  }
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
               if(mirror)
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

                     CKTNAVCompPinInst *c = new CKTNAVCompPinInst;
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
               else if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
               {
                  if (artwork_or_shape)
                  {
                     CString padlayer = "ALL";
                     CString pinname = np->getInsert()->getRefname();

                     CKTNAVCompPinInst *c = new CKTNAVCompPinInst;
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
            
                  acnt = CKTNAV_WriteSHAPEData(wfp,&(block->getDataList()), 
                                  point2.x, point2.y,
                                  block_rot, block_mirror,
                                  scale * np->getInsert()->getScale(),
                                  embeddedLevel+1, block_layer,artwork_or_shape, acnt, (np->getInsert()->getInsertType() == insertTypeGenericComponent));
               } // end else not aperture
            } // case INSERT
            break;                                                                
         } // end switch
      }
   } // end CKTNAV_WriteSHAPEData */
   return acnt;
}

//--------------------------------------------------------------
int CKTNAV_TestComplexApertureData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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
               apptype |= CKTNAV_TestComplexApertureData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1);
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end CKTNAV_TestComplexApertureData */
   return apptype;
}

//--------------------------------------------------------------
long CKTNAV_WriteComplexApertureData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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

                  CKTNAV_WriteComplexApertureData(wfp,&(subblock->getDataList()), 
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
               CKTNAV_WriteComplexApertureData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, apptyp);
            } // end else not aperture
         } // case INSERT
         break;                                                                
      } // end switch
   } // end CKTNAV_WriteComplexApertureData */
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
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
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
* CKTNAV_WriteROUTESData
*/
void CKTNAV_WriteROUTESData(FILE *wfp, FileStruct *file, CDataList *DataList, double file_insert_x, double file_insert_y, double file_rotation, int file_mirror, 
		double file_scale, int embeddedLevel, int insertLayer, const char *netname, CCKTNAVPadstackMap& padstackMap)
{ 
   int routeattcnt = 0;
	int viaattcnt = 0;

   Mat2x2 m;
   RotMat2(&m, file_rotation);

   for (int i=0;i<routescnt;i++)
   {
      // different netname
      CString NetnameInarray = check_name('n', routesarray[i]->netname, DO_NMCHK_LOG);
      if (strcmp(NetnameInarray, netname))
         continue; 

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
         if (!doc->get_layer_visible(layer, file_mirror))
            continue;

			const char* CKTNAVLayer = Layer_CKTNAV(layer);
         if (CKTNAVLayer == NULL)
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
            wGENCAD_Graph_Layer(wfp, CKTNAVLayer);

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

               p2.x = pnt->x * file_scale;
               if (file_mirror)
						p2.x = -p2.x;

               p2.y = pnt->y * file_scale;
               p2.bulge = pnt->bulge;
               TransPoint2(&p2, 1, &m, file_insert_x, file_insert_y);

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
               fprintf(navLogFp,"Internal Structure Error at entity %ld !!!\n", data->getEntityNumber());
               display_error++;
#endif
            continue;
         }


		   Point2 point2;
         point2.x = insert->getOriginX() * file_scale;
         if (file_mirror)
            point2.x = -point2.x;
         point2.y = insert->getOriginY() * file_scale;
         TransPoint2(&point2, 1, &m, file_insert_x, file_insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = file_mirror ^ insert->getMirrorFlags();
         double block_rot = file_rotation + insert->getAngle();
         if (file_mirror)
            block_rot = PI2 - (file_rotation +insert->getAngle());   // add 180 degree if mirrored.

         if (insert->getInsertType() == insertTypeFreePad || insert->getInsertType() == insertTypeVia
            || insert->getInsertType() == insertTypeBondPad )
         {
            CString blockName = (block)?block->getName():"";
            CString refname = insert->getRefname();

            if(insert->getInsertType() == insertTypeBondPad)
            {
               DataStruct *padstackdata = block->GetFirstPinInsertData();
               if(padstackdata)
               {              
                  InsertStruct *padstackinsert = padstackdata->getInsert();
                  // Find padstack name with rotation for bondpad
                  BlockStruct *padblock = GetBondPadPadstackBlock(doc->getCamCadData(), block);
                  if(padblock)
                  {
                     double padrout = padstackinsert->getAngle();
                     double padrot = block_rot + (padstackinsert->getGraphicMirrored()?(PI2 - padrout):padrout);
                     int padrotDegree = normalizeDegrees(round(RadToDeg(padrot)));
                     if(padrotDegree)
                        blockName.Format("%s_%d", padblock->getName(), padrotDegree);                     
                     else
                        blockName = padblock->getName();

                  }
               }
               if(refname.IsEmpty())
                  refname.Format("bondpad%d", ++bondpadcnt);
            }//insertTypeBondPad
            else
            {
               refname.Format("via%d", ++viacnt);
            }

				if (insert->getGraphicMirrored())
				{
					CCKTNAVPadstack* padstack = NULL;
					padstackMap.Lookup(blockName, padstack);
					if (padstack != NULL)
						blockName = padstack->GetMirName();
				}
				blockName = gen_string(blockName);

            fprintf(wfp,"VIA %s %1.*lf %1.*lf ALL -2 %s\n",
						blockName, output_units_accuracy, point2.x, output_units_accuracy, point2.y, refname);

				// For debug, change refname so we can correlate output to ccz display in camcad
				///CString newrefname;
				///newrefname.Format("via%d", viacnt);
				///insert->setRefname(newrefname);

#define VIA_NAILLOC_ON_THE_FLY
// Vias are done on the fly because it is most convenient to get via number this way
// But is a little slower becasue every via will be examined whereas batch scheme
// looks for probes on net and then checks if any of those are on vias, less searching.
#ifdef VIA_NAILLOC_ON_THE_FLY
				DataStruct *probeData = getTestProbeData(file, netname, data);
				if (probeData != NULL)
				{
					Mat2x2 m;
					RotMat2(&m, file_rotation);

					Point2 point2;
					point2.x = probeData->getInsert()->getOriginX() * file_scale;
					if (file_mirror)
						point2.x = -point2.x;

					point2.y = probeData->getInsert()->getOriginY() * file_scale;
					TransPoint2(&point2, 1, &m, file_insert_x, file_insert_y);

					fprintf(wfp, "NAILLOC via%d %s", viacnt, gen_string(check_name('c', probeData->getInsert()->getRefname(), DO_NMCHK_LOG)));
					fprintf(wfp, " %1.*lf %1.*lf",	output_units_accuracy, point2.x, output_units_accuracy, point2.y);
					fprintf(wfp, " -1 -1 100"); /* tester assigned number, tester interface number, probe size */
					fprintf(wfp, " %s\n", probeData->getInsert()->getPlacedBottom() ? "BOTTOM" : "TOP");
				}
#endif
			
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
							CCKTNAVPadstack* padstack = NULL;
							padstackMap.Lookup(blockName, padstack);
							if (padstack != NULL)
								blockName = padstack->GetMirName();
						}
						blockName = gen_string(blockName);

                  fprintf(wfp, "VIA %s %1.*lf %1.*lf ALL -2 via%d\n",
								blockName, output_units_accuracy, point2.x, output_units_accuracy, point2.y, ++viacnt);
						// For debug, change refname so we can correlate output to ccz display in camcad
						///CString newrefname;
						///newrefname.Format("VVvia%d", viacnt);
						///insert->setRefname(newrefname);
               }
            }
            else
            {
               fprintf(navLogFp, "TESTPOINT [%s] [%s] has no Padstack ?\n", insert->getRefname(), block->getName());
               display_error++;
            }
         }
			else if (!insert->getInsertType())
			{  
				// hierachies in nets are not supported.
				fprintf(navLogFp, "Hierachical Net [%s] not supported.\n", netname);
				display_error++;
			} 


#ifndef VIA_NAILLOC_ON_THE_FLY
			// Not used because via numbers are a nuisance, not available from via itself
			writeProbes(wfp, file, file_insert_x, file_insert_y, file_rotation, file_mirror, file_scale,
				netname, true /*only vias*/);
#endif
      }
   }
}

//--------------------------------------------------------------
void CKTNAV_GetROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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

   //      if ((l = Layer_CKTNAV(layer)) == NULL)
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
   //   CKTNAVRoutes *r = new CKTNAVRoutes;
   //   routesarray.SetAtGrow(routescnt,r);  
   //   routescnt++;   
   //   r->netname = get_attvalue_string(doc, a);
   //   r->pos = pos1;
   //}
}



/******************************************************************************
* CKTNAV_WriteCOMPONENTData
*/
void CKTNAV_WriteCOMPONENTData(FILE* wfp, FileStruct* file, BlockStruct* fileBlock,
      double insert_x, double insert_y, double rotation, int mirror, double scale,
		int embeddedLevel, int insertLayer, CCKTNAVDeviceMap& deviceMap)
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

      if (insert != NULL)
      {
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

			   CCKTNAVDevice* device = NULL;
			   Attrib* attrib = NULL;
            if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_PARTNUMBER, 1))
			   {
				   CString partNumber = gen_string(get_attvalue_string(doc, attrib));
				   device = deviceMap.AddDevice(partNumber, partNumber);
			   }
			   else 
				   device = deviceMap.GenerateDevice();

			   if (device->PinMapIsEmpty())
				   device->FillPinMap(file, data);


            fprintf(wfp, "COMPONENT %s\n", check_name('c', compname, DO_NMCHK_LOG));
            fprintf(wfp, "PLACE %1.*lf %1.*lf\n", output_units_accuracy, point2.x, output_units_accuracy, point2.y);
            fprintf(wfp, "LAYER %s\n", insert->getPlacedBottom()?"BOTTOM":"TOP");
            fprintf(wfp, "ROTATION %1.2lf\n", RadToDeg(block_rot));

            if ( block != NULL )
            {
               fprintf(wfp, "SHAPE %s %s %s\n", gen_string(block->getName()), 
				      										(block_mirror & MIRROR_FLIP)?"MIRRORY":"0",
                                                (block_mirror & MIRROR_LAYERS)?"FLIP":"0");
            }
            else
            {
               fprintf(wfp, "SHAPE %s %s\n", 	(block_mirror & MIRROR_FLIP)?"MIRRORY":"0",
                                                (block_mirror & MIRROR_LAYERS)?"FLIP":"0");
            }  

            fprintf(wfp, "DEVICE %s\n", device->GetName());

            CKTNAVCompList *c = new CKTNAVCompList;
            complistarray.SetAtGrow(complistcnt++, c);  
            c->compname = compname;
            c->geomname = block?block->getName():"";
            c->devicename = device->GetName();

            // attributes
            write_componentattributes(wfp, data->getAttributesRef(), "COMPONENT", &compattcnt, file, device);
         }
         else if (insert->getInsertType() == insertTypeTestPoint)
         {
            if (convert_tp_comps)   // if set to FALSE, a via record is generated.
            {
               CString compname = insert->getRefname();

               fprintf(wfp, "COMPONENT %s\n", gen_string(check_name('c', compname, DO_NMCHK_LOG)));
               fprintf(wfp, "PLACE %1.*lf %1.*lf\n", output_units_accuracy, point2.x, output_units_accuracy, point2.y);
   
               if (block != NULL && block->getBlockType() == blockTypePadstack)
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

				   CCKTNAVDevice* device = deviceMap.GenerateDevice();
				   device->SetType("Test_Point");
               fprintf(wfp, "DEVICE    %s\n", device->GetName());

               CKTNAVCompList *c = new CKTNAVCompList;
               complistarray.SetAtGrow(complistcnt,c);  
               complistcnt++;

               c->compname = compname;
               c->geomname = block?block->getName():"";
               c->devicename = device->GetName();

               // attributes
               write_componentattributes(wfp, data->getAttributesRef(), "COMPONENT", &compattcnt, file, device);
            }
		   }
      }
   }
   return;
}

//--------------------------------------------------------------
void CKTNAV_WriteTESTPOINTData(FILE *wfp, CDataList *DataList, const char *netname)
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
                        fprintf(wfp, "NODE %s %s\n", check_name('c', compname, DO_NMCHK_LOG), "1");
                        //write_attributes(fp, compPin->getAttributesRef(), "TESTPIN", &comppinattcnt);
                     }     
                  }
               }
               break;
            }
         } // case INSERT
         break;
      } // end switch
   } // end CKTNAV_WriteTESTPOINTData */
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
* CKTNAV_WriteMECHData
*/
void CKTNAV_WriteMECHData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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
               fprintf(wfp,"MECHANICAL %s\n", check_name('c', np->getInsert()->getRefname(), DO_NMCHK_LOG));
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

#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES
                  fprintf(wfp,"ATTRIBUTE FIDUCIAL \"%s\" %s\n",block->getName(), refname);
                  write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);
#endif

                  // now top
                  fprintf(wfp,"FID %s%d %s %1.*lf %1.*lf %s %1.2lf %s %s\n",fiducialname,
                        ++fidcnt, (padname)?gen_string(padname):"NO_PAD",
                        output_units_accuracy, point2.x, 
                        output_units_accuracy, point2.y, 
                        "TOP",
                        RadToDeg(block_rot),
                        "0","0");
#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES
                  fprintf(wfp,"ATTRIBUTE FIDUCIAL \"%s\" %s\n",block->getName(), refname);
                  write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);
#endif
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
#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES
                  fprintf(wfp,"ATTRIBUTE FIDUCIAL \"%s\" %s\n",block->getName(), refname);
                  write_attributes(wfp, np->getAttributesRef(), "FIDUCIAL", &fiducialattcnt);
#endif
               }
            }
            else if (np->getInsert()->getInsertType() == INSERTTYPE_TOOLING)
            {
               double drill = get_drill_from_block_no_scale(doc, block) * scale;

               if (drill == 0)
               {
                  fprintf(navLogFp, "TOOLING at [%lg, %lg] has no drill size -> set to .DEFAULTDRILLSIZE [%lg]\n", 
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
                  fprintf(navLogFp, "HOLE at [%lg, %lg] has no drill size -> set to .DEFAULTDRILLSIZE [%lg]\n", 
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
* CKTNAV_WriteTestProbeData
*/
void CKTNAV_WriteTestProbeData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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
         netname = doc->getCamCadData().getAttributeValueDictionary().getAt(attrib->getIntValue());

      CString refName;

      if (data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
         refName = data->getInsert()->getRefname();
      else
         refName.Format("%s%d", testpinprefix, ++testPinCount);

		// see if the probe is placed
		bool placed = false;
		WORD ppKW = doc->IsKeyWord(ATT_PROBEPLACEMENT, 0);

		if (data->lookUpAttrib(ppKW, attrib))
			placed = ((CString)get_attvalue_string(doc, attrib) == "Placed");

		if (placed)
			fprintf(wfp, "TESTPIN %s %1.*lf %1.*lf %s -1 -1 100 %s\n", refName,
					output_units_accuracy, point2.x, output_units_accuracy, point2.y, 
					check_name('n', netname, DO_NMCHK_LOG),
					data->getInsert()->getPlacedBottom()?"BOTTOM":"TOP");
		else
			fprintf(wfp, "TESTPIN %s -32767 -32767 %s -1 -1 100 %s\n", refName,  // Case 1442, unplaced probe coords
					check_name('n', netname, DO_NMCHK_LOG),
					data->getInsert()->getPlacedBottom()?"BOTTOM":"TOP"); // set to -32767,-32767, not 0,0


      // attributes
      int testPinAttCount = 0;
#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES
      fprintf(wfp, "ATTRIBUTE TESTPIN \"GeometryName\" \"%s\"\n", block->getName());
      write_attributes(wfp, data->getAttributesRef(), "TESTPIN", &testPinAttCount);
#endif
   }
}

//--------------------------------------------------------------
void CKTNAV_WritePANELBOARDSData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
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
               fprintf(wfp,"FILE %s.%s\n",filename,ext);
               fprintf(wfp,"FIDUCIAL 0.0 0.0\n");
               fprintf(wfp,"ROTATION %1.2lf\n",RadToDeg(block_rot));
               fprintf(wfp,"PLACE %1.*lf %1.*lf\n",
                  output_units_accuracy, point2.x, output_units_accuracy, point2.y);

               fprintf(wfp,"ELEMENT %s\n",(block_mirror)?"MIRRORY FLIP":"0 0");

               // attributes
               CString  refname = gen_string(np->getInsert()->getRefname());
               if (!strlen(refname))   refname = "?";
#ifndef CASE_1967_OUTPUT_NO_ATTRIBUTES
               fprintf(wfp,"ATTRIBUTE CAD %s %s\n",block->getName(), refname);
               write_attributes(wfp, np->getAttributesRef(), "BOARD", &boardattcnt);
#endif
            }
         } // case INSERT
         break;
      } // end switch
   } // end CKTNAV_WritePANELBOARDSData */

   if (!boardcnt) // if no board found in panel, default to 1.
   {
      fprintf(wfp,"BOARD BOARD%d\n",++boardcnt);
      fprintf(wfp,"FILE %s.%s\n",filename,ext);
      fprintf(wfp,"FIDUCIAL 0.0 0.0\n");
      fprintf(wfp,"ROTATION 0\n");
      fprintf(wfp,"PLACE 0 0\n");
   }
}

/******************************************************************************
* CKTNAV_WritePADSTACKData
	return 1 = top
	       2 = bottom
	       3 = all
	       4 = inner  
*/
int CKTNAV_WritePADSTACKData(FILE *wfp, const CString padstkname, CDataList *DataList, double rotation, int mirror,
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
      
      if ( data!= NULL && data->getDataType() == dataTypeInsert && data->getInsert() != NULL )
      {
		   InsertStruct* insert = data->getInsert();

         if (insert->getInsertType() != insertTypeThermalPad && insert->getInsertType() != insertTypeClearancePad &&
			    insert->getInsertType() != insertTypeObstacle   && insert->getInsertType() != insertTypeDrillSymbol)
         {
	         BlockStruct* block = doc->getBlockAt(insert->getBlockNumber());

            if ( block != NULL && !(block->getFlags()&BL_TOOL))
            {

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

               CString layername = Layer_CKTNAV(block_layer);
               LayerStruct *layerPtr = doc->FindLayer(block_layer);
		         if ( layerPtr != NULL && dupMirPadstack)
			         layerPtr = doc->getLayerAt(layerPtr->getMirroredLayerIndex());

		         if (layerPtr != NULL)
		         {
                  if ( !toppad || (!layerPtr->getMirrorOnly() && !(layerPtr->getLayerFlags() & LY_BOTTOMONLY)))
                  {
                     if (layerPtr->getLayerType() != layerTypeDrill)
                     {
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
                     fprintf(navLogFp, "PADSTACK [%s] PAD [%s] Layer [%s] not translated\n", padstkname, block->getName(), layername);
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
         }
      }
	}
	
   return pl;
}

/******************************************************************************
* CKTNAV_RotatedPadstackData
*/
int CKTNAV_RotatedPadstackData(CDataList *DataList, DbUnit insertAngle, CCKTNAVPadstackMap* padstackMap)
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
				   CCKTNAVPadstack* padstack = NULL;
               if (padstackMap->Lookup(block->getName(), padstack) && padstack != NULL)
                  padstackname =  padstack->GetMirName();;
            }

				CString newpadstackname;
				newpadstackname.Format("%s_%d", padstackname, r);

				CKTNAVPadRotStruct *p = new CKTNAVPadRotStruct;
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
* CCKTNAVDeviceMap::AddDevice
*/
CCKTNAVDevice* CCKTNAVDeviceMap::AddDevice(const CString deviceName, const CString partNumber)
{
	CCKTNAVDevice* device = NULL;
	if (!this->Lookup(deviceName, device))
	{
		device = new CCKTNAVDevice(deviceName, partNumber);
		this->SetAt(deviceName, device);
	}

	return device;
}

/******************************************************************************
* CCKTNAVDeviceMap::GenerateDevice
*/
CCKTNAVDevice* CCKTNAVDeviceMap::GenerateDevice()
{
	CString deviceName;
	deviceName.Format("Device%d", ++genericDeviceCnt);
	
	return AddDevice(deviceName, deviceName);
}

/******************************************************************************
* CCKTNAVDeviceMap::WriteDevice
*/
void CCKTNAVDeviceMap::WriteDevice(FILE *fp)
{
   fprintf(fp,"$DEVICES\n");

	POSITION pos = this->GetStartPosition();
	while (pos != NULL)
	{
		CString deviceName;
		CCKTNAVDevice* device = NULL; 
		this->GetNextAssoc(pos, deviceName, device);
		if (device == NULL)
			continue;

      fprintf(fp, "DEVICE %s\n", device->GetName());

		if (!device->GetPartNubmer().IsEmpty())
			fprintf(fp, "PART %s\n", device->GetPartNubmer());

		if (!device->GetGencadType().IsEmpty())
			fprintf(fp, "TYPE %s\n", device->GetGencadType());

		if (!device->GetStyle().IsEmpty())
			fprintf(fp, "STYLE %s\n", device->GetStyle());

		if (!device->GetValue().IsEmpty())
			fprintf(fp, "VALUE %s\n", device->GetValue());

		if (!device->GetPlusTol().IsEmpty())
			fprintf(fp, "PTOL %s\n", device->GetPlusTol());

		if (!device->GetMinusTol().IsEmpty())
			fprintf(fp, "NTOL %s\n", device->GetMinusTol());

		if (!device->GetTolerance().IsEmpty())
			fprintf(fp, "TOL %s\n", device->GetTolerance());

		if (!device->PinMapIsEmpty())
		{
			POSITION pos = device->GetPinMapStartPosition();
			while (pos != NULL)
			{
				CString pinName, pinType;
				device->GetNextPinMap(pos, pinName, pinType);
				if (!pinName.IsEmpty() && !pinType.IsEmpty())
				{
					fprintf(fp, "PINFUNCT %s %s\n", pinName, device->GetGencadPinType(pinType));
				}
			}
		}
	}

   fprintf(fp,"$ENDDEVICES\n\n");
}

/******************************************************************************
* GatherPadstacksAndRoutesData
*/
void GatherPadstacksAndRoutesData(CCEtoODBDoc* pDoc, FileStruct* pPCBFile, CCKTNAVPadstackMap& padstackMap)
{
	// Gather padstack
	for (int i=0; i<pDoc->getMaxBlockIndex(); i++)
	{
      BlockStruct* block = doc->getBlockAt(i);
      if (block == NULL || block->getFlags() & BL_WIDTH)
         continue;

      if (block->getBlockType() == blockTypePadstack || block->getFlags() & BL_TOOL)
		{
			CCKTNAVPadstack* padstack = new CCKTNAVPadstack(block);
			padstackMap.SetAt(padstack->GetName(), padstack);

			// Gather pad shape that has rotation inside a padstack
			if (block->getBlockType() == blockTypePadstack)
				CKTNAV_RotatedPadstackData(&block->getDataList(), 0, &padstackMap);
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

			if (Layer_CKTNAV(layerIndex) == NULL)
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
				CCKTNAVPadstack* padstack = NULL;
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
					CCKTNAVPadstack* padstack = NULL;
					if (padstackMap.Lookup(block->getName(), padstack) && padstack != NULL)
						padstack->SetNeedMirVersion(true);
				}

            if (insert->getInsertType() == insertTypeBondPad)
            {
               BlockStruct *block = pDoc->getBlockAt(insert->getBlockNumber());
               CKTNAV_RotatedPadstackData(&block->getDataList(), insert->getAngle(), &padstackMap);
                  
               //Set Insert Layer
               BlockStruct* padsblock = GetBondPadPadstackBlock(doc->getCamCadData(), block);
               if(padsblock)
               {
                  CCKTNAVPadstack* padstack = NULL;
				      if (padsblock && padstackMap.Lookup(padsblock->getName(), padstack) && padstack != NULL)
                  {
                     padstack->SetInsertLayer(data->getLayerIndex());
                  }
               }                   
            }
			}
		}

      CKTNAVRoutes *r = new CKTNAVRoutes;
      routesarray.SetAtGrow(routescnt++, r);  
      r->netname = get_attvalue_string(doc, attrib);
      r->pos = curPos;
	}
}


/******************************************************************************
*/

void CCKTNAVDevice::FillPinMap(FileStruct *file, DataStruct *data)
{
	if (data != NULL && data->getDataType() == dataTypeInsert &&
		data->getInsert()->getInsertType() == insertTypePcbComponent)
	{
		int insertedBlockNum = data->getInsert()->getBlockNumber();
		BlockStruct *insertedBlock = doc->getBlockAt(insertedBlockNum);
		if (insertedBlock != NULL)
		{
			CString compName = data->getInsert()->getRefname();
			POSITION pos = insertedBlock->getDataList().GetHeadPosition();
			while (pos != NULL)
			{
				DataStruct *pinData = insertedBlock->getDataList().GetNext(pos);
				if (pinData != NULL && pinData->getDataType() == dataTypeInsert &&
					pinData->getInsert()->getInsertType() == insertTypePin)
				{
					CString pinName = pinData->getInsert()->getRefname();
					//
					NetStruct *net = NULL;
					CompPinStruct *cp = FindCompPin(file, compName, pinName, &net);

					if (cp != NULL)
					{
						WORD pinMapKW = doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);

						Attrib *attrib = NULL;
						if (cp->getAttributesRef() && cp->getAttributesRef()->Lookup(pinMapKW, attrib))
						{
							CString pinTypeStr = get_attvalue_string(doc, attrib);
							this->SetPinMap(pinName, pinTypeStr);
						}
					}
				}
			}
		}
	}

}

CString CCKTNAVDevice::GetGencadPinType(CString camcadPinType)
{
	// Note that pin type "No Connect" is a valid map entry, and
	// has a gencad type mapping, but it is not included in the
	// PinTypeTag enum, it appears as "unknown".

	if (camcadPinType.CollateNoCase("No Connect") == 0)
	{
		return "INTNC";
	}
	else
	{
		// All the rest are just up-cased versions of camcad type
		PinTypeTag ccPinType = stringToPinTypeTag(camcadPinType);
		if (ccPinType > pinTypeMin && ccPinType < pinTypeMax)
			return camcadPinType.MakeUpper();
	}

	return "";
}

CString CCKTNAVDevice::GetGencadType()
{
	DeviceTypeTag deviceType = stringToDeviceTypeTag(this->m_sType);

	const char* retval;

	switch(deviceType)
	{
	case deviceTypeUnknown:             retval = "";			break;
	case deviceTypeBattery:             retval = "ANALOG";	break;
	case deviceTypeCapacitor:           retval = "CAP";		break;
	case deviceTypeCapacitorArray:      retval = "CPCK";		break;
	case deviceTypeCapacitorPolarized:  retval = "PCAP";		break;
	case deviceTypeCapacitorTantalum:   retval = "TCAP";		break;
	case deviceTypeConnector:           retval = "CONN";		break;
	case deviceTypeCrystal:             retval = "XTAL";		break;
	case deviceTypeDiode:               retval = "DIODE";		break;
	case deviceTypeDiodeArray:          retval = "DPCK";		break;
	case deviceTypeDiodeLed:            retval = "LED";		break;
	case deviceTypeDiodeLedArray:       retval = "";			break;
	case deviceTypeDiodeZener:          retval = "ZENER";		break;
	case deviceTypeFilter:              retval = "ANALOG";	break;
	case deviceTypeFuse:                retval = "FUSE";		break;
	case deviceTypeIC:                  retval = "LOGIC";		break;
	case deviceTypeICDigital:           retval = "LOGIC";		break;
	case deviceTypeICLinear:            retval = "ANALOG";	break;
	case deviceTypeInductor:            retval = "IND";		break;
	case deviceTypeJumper:              retval = "JUMPER";	break;
	case deviceTypeNoTest:              retval = "OTHER";		break;
	case deviceTypeOscillator:          retval = "ANALOG";	break;
	case deviceTypePotentiometer:       retval = "VRES";		break;
	case deviceTypePowerSupply:         retval = "ANALOG";	break;
	case deviceTypeRelay:               retval = "RELAY";		break;
	case deviceTypeResistor:            retval = "RES";		break;
	case deviceTypeResistorArray:       retval = "RPCK";		break;
	case deviceTypeSpeaker:             retval = "ANALOG";	break;
	case deviceTypeSwitch:              retval = "SWITCH";	break;
	case deviceTypeTestPoint:           retval = "TESTPAD";	break;
	case deviceTypeTransformer:         retval = "XFMR";		break;
	case deviceTypeTransistor:          retval = "NPN";		break;
	case deviceTypeTransistorArray:     retval = "ANALOG";	break;
	case deviceTypeTransistorFetNpn:    retval = "NFET";		break;
	case deviceTypeTransistorFetPnp:    retval = "PFET";		break;
	case deviceTypeTransistorMosfetNpn: retval = "NJFET";		break;
	case deviceTypeTransistorMosfetPnp: retval = "NPFET";		break;
	case deviceTypeTransistorNpn:       retval = "NPN";		break;
	case deviceTypeTransistorPnp:       retval = "PNP";		break;
	case deviceTypeTransistorScr:       retval = "SCR";		break;
	case deviceTypeTransistorTriac:     retval = "TRIAC";		break;
	case deviceTypeVoltageRegulator:    retval = "VR";       break;
	case deviceTypeOpto:						retval = "OPTO";		break;
	default:										retval = "";			break;
	}

	return retval;
}


