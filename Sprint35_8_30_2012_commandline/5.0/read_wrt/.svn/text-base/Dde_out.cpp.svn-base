// $Header: /CAMCAD/5.0/read_wrt/Dde_out.cpp 40    6/17/07 8:56p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 2000. All Rights Reserved.
*/
                              
#include "stdafx.h"
#include "ccdoc.h"                                          
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include <math.h>
#include "pcbutil.h"
#include "outline.h"
#include "graph.h"
#include "ck.h"
#include "pcblayer.h"
#include "apertur2.h"
#include "polylib.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

/*****************************************************************************/
/*
*/

#define  LAY_TOP              -1
#define  LAY_BOT              -2
#define  LAY_INNER            -3
#define  LAY_ALL              -4
#define  LAY_UNKNOWN          -5

// this is for the .LAYERTYPE
#define  LAYTYPE_ELECTRICAL   -6
#define  LAYTYPE_REFNAME      -7
#define  LAYTYPE_ARTICLE      -8 
#define  LAYTYPE_TYPENAME     -9       

#define  MAX_LAYER            255
// this is for the .LAYERNAME
#define  LAYTYPE_NAME         -10

/*****************************************************************************/
/*
*/

typedef struct
{
   int   mirror;     // this array is from 0..254 255=Generic drill
}IPLLayers;

typedef struct
{
   CString  padstackname;
   int      bnum;
   int      use;        // 1 = via, 2 = other
}IPLUsedPadstackStruct;

typedef CTypedPtrArray<CPtrArray, IPLUsedPadstackStruct*> UsedPadstackArray;

typedef struct
{
   CString  netname;
   POSITION pos;
}IPLRoutes;
typedef CTypedPtrArray<CPtrArray, IPLRoutes*> IPLRoutesArray;

typedef struct
{
   CString  shapename;
   int      blocknum;
}IPLPCBShapes;
typedef CTypedPtrArray<CPtrArray, IPLPCBShapes*> IPLPcbShapesArray;

typedef struct
{
   CString  compname;
}IPLPCBComponents;
typedef CTypedPtrArray<CPtrArray, IPLPCBComponents*> IPLPcbComponentsArray;

typedef struct
{
   CString  name;
   int      blocknum;
   int      drill;         // is this a tool
}IPLPadshapes;    // apertures and tools
typedef CTypedPtrArray<CPtrArray, IPLPadshapes*> IPLPadshapesArray;

typedef struct
{
   int      stacknum;      // DDE layer index number
   int      mirrornum;
}IPLMirrorLayers;    // apertures and tools
typedef CTypedPtrArray<CPtrArray, IPLMirrorLayers*> IPLMirrorLayersArray;

typedef struct
{
   int  layernr;
   int  pshnr;
}IPLPShape;

typedef struct
{
   CString     name;
   int         stackindex;
   CString     vianame;       // vianame must be different from pad name, so this name is used for via
   int         viastackindex;
   int         bnum;
   int         smd;           // SMD attribute found
   double      drill;         // needed for holes.
   IPLPShape   shapes[MAX_LAYER];
   int         drillform;
}IPLPadstack;
typedef CTypedPtrArray<CPtrArray, IPLPadstack*> IPLPadstackArray;

typedef struct
{
   int      layertype;     // Layertype 
   int      ddelayernr;
   CString  ddelayname;

   LayerTypeTag getLayerType() const         { return (LayerTypeTag)layertype; }
   void setLayerType(LayerTypeTag layerType) { layertype = layerType; }
   void setLayerType(int layerType)          { layertype = ((layertype >= layerTypeLowerBound && layertype <= layerTypeUpperBound) ? layerType : layerTypeUndefined);; }
}DDE_LayerMap;
typedef CTypedPtrArray<CPtrArray, DDE_LayerMap*> CLayerMapArray;

typedef struct
{
   int      layertype;     // Layertype 
   CString  usertypename;
}DDE_LayerUserType;
typedef CTypedPtrArray<CPtrArray, DDE_LayerUserType*> CLayerUserTypeArray;

typedef struct // used for .LAYCOMP, DEVICE and ARTICLE
{
   int   lay;
   int   mlay;
}DDE_LayMLay;

/*****************************************************************************/
/*
*/
static   FileStruct     *cur_file;
static   CCEtoODBDoc     *doc;
static   int            output_units_accuracy = 3;
static   double         scale = 1.0;

extern   CProgressDlg			*progress;
//extern   LayerTypeInfoStruct	layertypes[];

static   IPLLayers      ipl_layers[MAX_LAYER];

static   CLayerMapArray layermaparray;
static   int            layermapcnt = 0;

static   CLayerUserTypeArray  layerusertypearray;
static   int            layerusertypecnt = 0;

static   IPLRoutesArray routesarray;   
static   int            routescnt;

static   IPLPcbShapesArray pcbshapesarray;   
static   int            pcbshapescnt;

static   UsedPadstackArray usedpadstackarray;
static   int            usedpadstackcnt;

static   IPLPadshapesArray padshapesarray;   
static   int            padshapescnt;

static   IPLMirrorLayersArray mirrorarray;   
static   int            mirrorcnt;

static   IPLPadstackArray  padstackarray; 
static   int            padstackcnt;

static   IPLPcbComponentsArray   pcbcomponentsarray;  
static   int            pcbcomponentscnt;

static   int            display_error;
static   FILE           *flog;
static   CString        fiducialname;
static   CString        panelfiducialname;

typedef  CArray<Point2, Point2&> CPolyArray;
static   CPolyArray     polyarray;
static   int            polycnt;

static   CDDELayerArray ddeArr;
static   int            maxArr = 0;
static   int            max_signalcnt;

static   DDE_LayMLay    sys_cmpsizelay;
static   DDE_LayMLay    sys_compnamelay;
static   DDE_LayMLay    sys_devicelay;
static   DDE_LayMLay    sys_articlelay;
static   int            DDE_COMPSIDE = 1;       // default layers
static   int            DDE_SOLDERSIDE = 0;     // default layers
static   int            dummypsh;               // need a zerosize dummy shape
static   int            dfacnt;

/*****************************************************************************/
/*
*/
static void IPL_WriteROUTESData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
               double rotation, int mirror, double scale, int embeddedLevel, int insertlevel,const char *netname);

static void IPL_GetROUTESData(FILE *wfp, CDataList *DataList);

static long cnv_units(double x);

static CString cnv_rotation(double radians);

static void write_iplartwork(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
                     double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static void write_iplshape(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
                     double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static void write_iplmechanicalpins(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
                     double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

static int  write_trace_arc(FILE *wfp, int trackcode, int layernum, 
                           double x1, double y1, double bulge, double x2, double y2);

static int  write_coo_cen(FILE *wfp, double x1, double y1, double bulge, double x2, double y2);

static double  round_offset(double x);

static int  Layer_DDE(int l);

static int get_compptr(const char *c);

static CString get_layerusertype( int laytyp, int signr);

static void write_componentmechanicalpin(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer,
      CString  refname);

//--------------------------------------------------------------
// .LAYERMAP command from dde.out file.
// this assignes a Layertype to a DDE stacknum 0..255
static int get_layermap(int ltype, int signr)
{
   int   i;
   int   found;


   for (i=0;i<layermapcnt;i++)
   {
      if (layermaparray[i]->getLayerType() == ltype)
         return layermaparray[i]->ddelayernr;
   }

   // this signr is for inner layer assignment
   if (signr > 0)
   {
      if (is_electricallayer(ltype))   
      {
         found = 1;  // 
         for (i=0;i<layermapcnt;i++)
         {
            if (layermaparray[i]->getLayerType() == LAYTYPE_ELECTRICAL)
            {
               found++;
               if (found == signr)
                  return layermaparray[i]->ddelayernr;
            }
         }
      } // is_electrical
      else
      {
         fprintf(flog,"Signr [%d] but not Electrical [%d] ?\n", signr, ltype);
         display_error++;
      }
   }

   return LAY_UNKNOWN;
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
static int load_iplsettings(CString fname)
{
   FILE  *fp;
   char  line[127];
   char  *lp;
   int   i;

   sys_cmpsizelay.lay = 120;
   sys_cmpsizelay.mlay = 121;
   sys_devicelay.lay   = 14;
   sys_compnamelay.lay = 111;
   sys_devicelay.lay   = 14;
   sys_articlelay.lay  = 17;
   sys_compnamelay.mlay= 111;
   sys_devicelay.mlay  = 14;
   sys_articlelay.mlay = 17;

   for (i=0;i<MAX_LAYER;i++)
      ipl_layers[i].mirror = i;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"DDE Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,127,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         if (!STRICMP(lp,".LAYCMPSIZE"))
         {
            int   lay, mlay;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            lay = atoi(lp);
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            mlay = atoi(lp);
            sys_cmpsizelay.lay = lay;
            sys_cmpsizelay.mlay = mlay;
         }
         else
         if (!STRICMP(lp,".LAYCOMPNAME"))
         {
            int   lay, mlay;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            lay = atoi(lp);
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            mlay = atoi(lp);
            sys_compnamelay.lay = lay;
            sys_compnamelay.mlay = mlay;
         }
         else
         if (!STRICMP(lp,".LAYDEVICENAME"))
         {
            int   lay, mlay;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            lay = atoi(lp);
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            mlay = atoi(lp);
            sys_devicelay.lay   = lay;
            sys_devicelay.mlay  = mlay;
         }
         else
         if (!STRICMP(lp,".LAYARTICLENAME"))
         {
            int   lay, mlay;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            lay = atoi(lp);
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            mlay = atoi(lp);
            sys_articlelay.lay = lay;
            sys_articlelay.mlay = mlay;
         }
         else
         if (!STRICMP(lp,".LAYERMAP"))
         {
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cclayer = _strupr(lp);
            cclayer.TrimLeft();
            cclayer.TrimRight();
            if ((lp = get_string(NULL," \t;\n")) == NULL)   continue; 
            int ddelayernum = atoi(lp);

            int laytype = stringToLayerTypeTag(cclayer);
            DDE_LayerMap *l = new DDE_LayerMap;
            layermaparray.SetAtGrow(layermapcnt, l);
            layermapcnt++;
            l->setLayerType(laytype);
            l->ddelayernr = ddelayernum;
            l->ddelayname = "";
            
            if (laytype == LAYTYPE_SIGNAL_TOP || laytype == LAYTYPE_PAD_TOP)
               DDE_COMPSIDE = ddelayernum;
            if (laytype == LAYTYPE_SIGNAL_BOT || laytype == LAYTYPE_PAD_BOTTOM)
               DDE_SOLDERSIDE = ddelayernum;

         }
         else
         if (!STRICMP(lp,".LAYERUSERTYPE"))
         {
            CString  cclayer, usertypename;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cclayer = _strupr(lp);
            cclayer.TrimLeft();
            cclayer.TrimRight();
            if ((lp = get_string(NULL," \t;\n")) == NULL)   continue; 
            usertypename = lp;
            usertypename.TrimLeft();
            usertypename.TrimRight();

            int laytype = stringToLayerTypeTag(cclayer);
            DDE_LayerUserType *l = new DDE_LayerUserType;
            layerusertypearray.SetAtGrow(layerusertypecnt, l);
            layerusertypecnt++;
            l->layertype = laytype;
            l->usertypename = usertypename;
         }
         else
         if (!STRICMP(lp,".LAYERTYPE"))
         {
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cclayer = _strupr(lp);
            cclayer.TrimLeft();
            cclayer.TrimRight();
            if ((lp = get_string(NULL," \t;\n")) == NULL)   continue; 
            int ddelayernum = atoi(lp);

            int laytype = LAY_UNKNOWN;

            if (!cclayer.CompareNoCase("electrical"))
               laytype = LAYTYPE_ELECTRICAL;
            else
            {
               fprintf(flog, "Unknown Layertype [%s] in [%s]\n", cclayer, fname);
               display_error++;
               continue;
            }

            DDE_LayerMap *l = new DDE_LayerMap;
            layermaparray.SetAtGrow(layermapcnt, l);
            layermapcnt++;
            l->setLayerType(laytype);
            l->ddelayernr = ddelayernum;
            l->ddelayname = "";
         }
         else
         if (!STRICMP(lp,".LAYERNAME"))
         {
            CString  cclayer;
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();
            if ((lp = get_string(NULL," \t;\n")) == NULL)   continue; 
            int ddelayernum = atoi(lp);

            int laytype = LAYTYPE_NAME;

            DDE_LayerMap *l = new DDE_LayerMap;
            layermaparray.SetAtGrow(layermapcnt, l);
            layermapcnt++;
            l->setLayerType(laytype);
            l->ddelayernr = ddelayernum;
            l->ddelayname = cclayer;
         }
      }
   }

   fclose(fp);
   return 1;
}

//--------------------------------------------------------------
static void write_attributes(FILE *fp, const char *att, CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;
   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))   continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))continue;

      switch (attrib->getValueType())
      {
         case VT_INTEGER:
            fprintf(fp,"%s %s %d\n", att, doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
            break;
         case VT_UNIT_DOUBLE:
         case VT_DOUBLE:
            fprintf(fp,"%s %s %lg\n", att, doc->getKeyWordArray()[keyword]->out, attrib->getDoubleValue());
            break;
         case VT_STRING:
         {
            if (attrib->getStringValueIndex() < 0)  break;
            char *tok, *temp = STRDUP(get_attvalue_string(doc,attrib));
            
            tok = strtok(temp,"\n");

            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  fprintf(fp,"%s %s \'%s\'\n", att, doc->getKeyWordArray()[keyword]->out, tok);
               }
               else
               {
                  fprintf(fp,"%s %s\n", att, doc->getKeyWordArray()[keyword]->out);
               }

               tok = strtok(NULL,"\n");
            }

            free(temp);
         }

         break;
      }
      // here kill mask and layer
   }

   return;
}

//--------------------------------------------------------------
static void write_DeviceAttribs(FILE *fp,CAttributes* map, 
                                const char *GENCAMname, int *attcnt )
{
   WORD keyword;
   Attrib* attrib;
   int   GENCAMcnt = *attcnt;

   if (map == NULL)  return;
   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))     continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))   continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0))      continue;

      switch (attrib->getValueType())
      {
         case VT_INTEGER:
            fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %d\n", 
                  GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
            break;
         case VT_UNIT_DOUBLE:
         case VT_DOUBLE:
            fprintf(fp,"ATTRIBUTE %s_%d \"%s\" %lg\n", 
               GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out, attrib->getDoubleValue());
            break;
         case VT_STRING:
         {
            if (attrib->getStringValueIndex() < 0)
            {
               break;
            }

            char *tok, *temp = STRDUP(attrib->getStringValue());
            
            tok = strtok(temp,"\n");

            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  if (!doc->getKeyWordArray()[keyword]->out.CompareNoCase("TOL"))
                  {
                     fprintf(fp,"%s %s\n", doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
                  }
                  else if (!doc->getKeyWordArray()[keyword]->out.CompareNoCase("NTOL"))
                  {
                     fprintf(fp,"%s %s\n", doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
                  }
                  else if (!doc->getKeyWordArray()[keyword]->out.CompareNoCase("PTOL"))
                  {
                     fprintf(fp,"%s %s\n", doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
                  }
                  else if (!doc->getKeyWordArray()[keyword]->out.CompareNoCase("DESC"))
                  {
                     fprintf(fp,"%s %s\n", doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
                  }
                  else if (!doc->getKeyWordArray()[keyword]->out.CompareNoCase("VOLTS"))
                  {
                     fprintf(fp,"%s %s\n", doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
                  }
                  else
                  {
                     fprintf(fp,"ATTRIBUTE %s_%d \"%s\" \"%s\"\n", 
                        GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out,tok); // multiple values are delimited by \n
                  }
               }
               else
               {
                  fprintf(fp,"ATTRIBUTE %s_%d \"%s\" ?\n", 
                     GENCAMname, ++GENCAMcnt, doc->getKeyWordArray()[keyword]->out);
               }

               tok = strtok(NULL,"\n");
            }

            free(temp);
         }

         break;
      }
   }

   *attcnt = GENCAMcnt;

   return;
}

//--------------------------------------------------------------
// get the mirror DDE layer
static int get_mirrorstacknum(int stacknum)
{
   int i;

   for (i=0;i<mirrorcnt;i++)
   {
#ifdef _DEBUG
   IPLMirrorLayers *mm = mirrorarray[i];
#endif
      if (mirrorarray[i]->stacknum == stacknum)
         return mirrorarray[i]->mirrornum;
   }

   return stacknum;
}

//--------------------------------------------------------------
static int get_padstackptr(int bnum)
{
   int   i;

   for (i=0;i<padstackcnt;i++)
   {
      if (padstackarray[i]->bnum == bnum)
         return i;
   }

   return -1;
}

/*****************************************************************************/
/*
*/
static int write_wlp(FILE *fp, const char *compname, const char *pinname, 
                     double insert_x, double insert_y, double pinrot,
                     int padstack_blocknum, int mirror)
{
   int      padstacknr = get_padstackptr(padstack_blocknum);
   int      compnr = get_compptr(compname)+1;

   if (compnr < 0)   return 0;   // component could not be found

   char padtyp = 'p';
   // P for smd p for throu
   if (padstackarray[padstacknr]->smd) padtyp = 'P';

   fprintf(fp,".wlp %d %ld %ld %c %s %s %s %d\n", compnr,
         cnv_units( insert_x), cnv_units( insert_y), padtyp,
         cnv_rotation(pinrot), pinname, compname, 
         padstackarray[padstacknr]->stackindex);

   if (padstackarray[padstacknr]->drillform > -1)
   {
      fprintf(fp,"     255 %d\n", padstackarray[padstacknr]->drillform);
   }

   int written = FALSE;

   for (int w=0;w<MAX_LAYER;w++)
   {
      if (padstackarray[padstacknr]->shapes[w].pshnr > 0)   
      {
         fprintf(fp,"     %d %d\n", mirror?ipl_layers[w].mirror:w,
               padstackarray[padstacknr]->shapes[w].pshnr);
         written = TRUE;
      }
   }

   // make a dummy pads
   if (!written)
   {
      // dummy shape (zero size) on TOp and BOTTOM
      fprintf(fp,"     %d %d\n", DDE_COMPSIDE, dummypsh);
      fprintf(fp,"     %d %d\n", DDE_SOLDERSIDE, dummypsh);

   }


   fprintf(fp,"\n");
   //write_attributes(fp, compPin->getAttributesRef(), "COMPPIN", &comppinattcnt);

   return 1;
}


//--------------------------------------------------------------
static int find_powerlayer(const char *netname)
{
   LayerStruct *layer;
   int         j;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.

      Attrib *a;
      if (a =  is_attvalue(doc, layer->getAttributesRef(), ATT_NETNAME, 1))
      {
         CString l;
         l = get_attvalue_string(doc, a);
         
         char  *lp;
         char  tmp[5000];
         strcpy(tmp, l);

         lp = strtok(tmp,"\n");
         while (lp)
         {
            if (!STRCMPI(lp, netname))
            {
               return Layer_DDE(layer->getLayerIndex());
            }
            lp = strtok(NULL,"\n");
         }
      }
   }

   return -1;
}

//--------------------------------------------------------------
static int write_iplroutes(FILE *fp, FileStruct *file,CNetList *NetList,
                     double insert_x, double insert_y,
                     double rotation, int mirror,
                     double scale,
                     int embeddedLevel, int insertLayer)

{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   IPL_GetROUTESData(fp,&(file->getBlock()->getDataList()));

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      fprintf(fp,".wlg 0 n %s\n", check_name('n', net->getNetName()));

      Attrib *a;
      if (a =  is_attvalue(doc, net->getAttributesRef(), ATT_POWERNET, 1))
      {
         int powerlayer = find_powerlayer(net->getNetName());
         if (powerlayer > -1)
            fprintf(fp,".wop plane %d\n", ddeArr[powerlayer]->stackNum);
      }

      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         write_wlp(fp, compPin->getRefDes(), compPin->getPinName(), compPin->getOriginX()+insert_x, compPin->getOriginY()+insert_y,
            compPin->getRotationRadians(), compPin->getPadstackBlockNumber(), compPin->getMirror());

      }

      // order by signal name
      IPL_WriteROUTESData(fp,&(file->getBlock()->getDataList()), 
            insert_x, insert_y, rotation, mirror, scale, 0, -1, net->getNetName());
      

      fprintf(fp,"\n");
   }

   // unused net
   netPos = NetList->GetHeadPosition();

   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (!(net->getFlags() & NETFLAG_UNUSEDNET))   continue;


      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {

         compPin = net->getNextCompPin(compPinPos);

         CString  cname, pname;
         cname = check_name('n', compPin->getRefDes());
         pname = check_name('n', compPin->getPinName());
         fprintf(fp,".wlg 0 n %s-%s\n", cname, pname);

         write_wlp(fp, compPin->getRefDes(), compPin->getPinName(), compPin->getOriginX()+insert_x, compPin->getOriginY()+insert_y, 
            compPin->getRotationRadians(), compPin->getPadstackBlockNumber(), compPin->getMirror());

      }
   }


   return 1;
}


//--------------------------------------------------------------
static int get_shapeptr(int blocknum)
{
   int   i;

   for (i=0;i<pcbshapescnt;i++)
   {
      if (pcbshapesarray[i]->blocknum == blocknum)
         return i;
   }

   fprintf(flog,"Shape could not be found!\n");
   display_error++;

   return -1;
}

//--------------------------------------------------------------
static int get_compptr(const char *c)
{
   int   i;

   for (i=0;i<pcbcomponentscnt;i++)
   {
      if (!pcbcomponentsarray[i]->compname.Compare(c))
         return i;
   }

   fprintf(flog,"Component [%s] could not be found!\n", c);
   display_error++;

   return -1;
}

//--------------------------------------------------------------
static int get_padshapeptr(const char *c)
{
   int   i;

   for (i=0;i<padshapescnt;i++)
   {
      if (!padshapesarray[i]->name.Compare(c))
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static int write_iplcomponents(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer)

{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   POSITION pos;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();

   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT &&
                np->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)   break;

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT && block->getBlockType() != BLOCKTYPE_TESTPOINT)
            {
               fprintf(flog,"Component [%s] and a non electrical pattern [%s] -> ignored\n",
                  np->getInsert()->getRefname(), block->getName());
               display_error++;
               break;
            }

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();   // there is no rotation offset for mirror !
            CString  refname;

            refname = check_name('c', np->getInsert()->getRefname());

            IPLPCBComponents *c = new IPLPCBComponents;
            pcbcomponentsarray.SetAtGrow(pcbcomponentscnt,c);  
            pcbcomponentscnt++;  

            c->compname = np->getInsert()->getRefname();
            CString  parttyp;

            parttyp = 'n';
            
            // do compinstance
            fprintf(wfp,".com %d %ld %ld %s %s %s 0 %d %s\n",
                  pcbcomponentscnt,
                  cnv_units(point2.x), cnv_units(point2.y),
                  cnv_rotation(block_rot),
                  (block_mirror)?"1 m":"0 n",
                  parttyp,
                  get_shapeptr(block->getBlockNumber())+1,check_name('s', block->getName()));

            CString  att_visible;

            double   attx, atty, attrot;
            // get refname attribute
            Attrib *a;
            if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_REFNAME, 1))
            {
/*
            Rotate(xn, yn, crot+dbunit_2_degree(CN_Rotation), &tx, &ty);
            xn = tx;
            yn = ty;
            tdir = crot+dbunit_2_degree(CN_Rotation);
*/
               att_visible = "LB";
               attx = point2.x;
               atty = point2.y;
               attrot = block_rot;

            }
            else
            {
               att_visible = "LBi";
               attx = point2.x;
               atty = point2.y;
               attrot = block_rot;
            }

            // .txt refname layer 
            int layer = sys_compnamelay.lay;
            if (block_mirror)
               layer = sys_compnamelay.mlay;

            fprintf(wfp,".txt %ld %ld %d %s 0 %s 10000 1 \'%s\'\n",
               cnv_units(attx), cnv_units(atty),
               layer,cnv_rotation(attrot) ,att_visible, refname);

            CString  dde_device;
            if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
            {
               dde_device = get_attvalue_string(doc, a);
/*
            Rotate(xn, yn, crot+dbunit_2_degree(CN_Rotation), &tx, &ty);
            xn = tx;
            yn = ty;
            tdir = crot+dbunit_2_degree(CN_Rotation);
*/
               att_visible = "LB";
               attx = point2.x;
               atty = point2.y;
               attrot = block_rot;

            }
            else
            {
               att_visible = "LBi";
               dde_device = "";
               attx = point2.x;
               atty = point2.y;
               attrot = block_rot;
            }

            // .txt device layer 
            layer = sys_devicelay.lay;
            if (block_mirror)
               layer = sys_devicelay.mlay;

            fprintf(wfp,".txt %ld %ld %d %s 0 %s 10000 1 \'%s\'\n",
               cnv_units(attx), cnv_units(atty),
               layer,cnv_rotation(attrot), att_visible, dde_device);

            // .txt article (partnumber)
            layer = sys_articlelay.lay;
            if (block_mirror)
               layer = sys_articlelay.mlay;

            att_visible = "LBi";

            CString  dde_value;
            if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_VALUE, 1))
            {
               dde_value = get_attvalue_string(doc, a);
/*
            Rotate(xn, yn, crot+dbunit_2_degree(CN_Rotation), &tx, &ty);
            xn = tx;
            yn = ty;
            tdir = crot+dbunit_2_degree(CN_Rotation);
*/
               att_visible = "LB";
               attx = point2.x;
               atty = point2.y;
               attrot = block_rot;

            }
            else
            {
               att_visible = "LBi";
               dde_value = "";
               attx = point2.x;
               atty = point2.y;
               attrot = block_rot;
            }

            fprintf(wfp,".txt %ld %ld %d %s 0 %s 10000 1 \'%s\'\n",
               cnv_units(attx), cnv_units(atty),
               layer,cnv_rotation(attrot), att_visible , dde_value);

            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)   
               fprintf(wfp,".cop kind TEST\n");
         
            write_attributes(wfp, ".cop", np->getAttributesRef());

            fprintf(wfp,"\n");

         } // case INSERT
         break;

      } // end switch
   } // end for

   fprintf(wfp,"\n");

   return 1;
}

/****************************************************************************/
/*
  ipl units are 1/100 mil.
*/
static long cnv_units(double x)
{
   long l = lround(x * scale * 100);
   return l;
}

/****************************************************************************/
/*
   
*/
static CString cnv_rotation(double radians)
{
   CString  r;
   int      rot = round(RadToDeg(radians));
   int      r90 = rot / 90;
   int      rxx = rot % 90;   // fraction

   if (rxx)
      r.Format("%da%d", r90, rxx);
   else
      r.Format("%d", r90);

   return r;
}

/****************************************************************************/
/*
*/
static int write_iplheader(FILE *fo, const char *fname, BlockStruct *fblock,
                           double xoffset, double yoffset)
{
   double   ver = 7;
   double   max_brd_x, max_brd_y;

   max_brd_x = fblock->getXmax() + xoffset;
   max_brd_y = fblock->getYmax() + yoffset;

   CTime t;
   t = t.GetCurrentTime();

   fprintf(fo,"\n\
     filename        : %s\n\
     operator        : Mentor Graphics\n\
     progversion     : %1.1lf\n\
     x,y,lay,mul,div : %ld %ld %d 1 / 40\n\
     date            : %s\n\
\n\
     commentfile : ''\n\
     jobdepend.  : ''\n\
     prepostpro. : '#(postpro)'\n\
\n",
      fname, ver,
      cnv_units(max_brd_x), cnv_units(max_brd_y),
      (max_signalcnt<=8)?max_signalcnt:0, 
      t.Format( "%A %B %d %H:%M:%S %Y"  ));

   fprintf(fo,"     \n\
.rem\n\
 .psh padshapes : padshapenumber drillsize plated/non filled/non shapename\n\
 .tsh tracksha  : trackshapenumber filled/non shapename\n\
 .tol tolerance string\n\
 .cle deltasizes: number track- via- pad- smddelta\n\
 .sec secret    : secret shape\n\
 .dfa def area  : kind lay shape dir spacing groupname\n\
 .coo coordinate: x y\n\
 .cir circle    : radius\n\
 .poe pol end   : end polygon name\n\
 .pst padstack  : padstacknumber drillsize plated/non filled/non \\\n\
                  stacktype padstackname\n\
 .uls lay shape : userlaytypename shapenumber\n\
 .pla plane info: deltaheat deltaiso heatgap clearance heatsymbol \\\n\
                  isosymbol calcheat calciso calcfromplotter\n\
 .pse stack end : end padstack information\n\
 .lpm lay assoc.: userlaytypename systemlaytypename polygon\n\
 .cen arc center: x y rot\n\
 .ena end area\n\
 .fon textfont \n\
 .typ type      : xref yref xmax ymax typenumber typename\n\
 .lay layattrib.: lay obstruction electric protection layname pastackname\n\
 .sys sysparms  : sysparmname sysparmvalue\n\
 .cmd iplcommand: cmdname parmname1 parmvalue 1...nameX valueX\n\
 .mir mirrormap : fromlay tolay\n\
 .bma burymap   : /fromlay;tolay;../fromlay;tolay\n\
 .com component : compnum x y dir lay mir/not fix/visi stat tnum typename\n\
 .cop comp opts : optionname optionvalue\n\
 .blo blockname : compnum compnum ... compnum\n\
 .wlg wlgroup   : status protect/non groupname\n\
 .wop wloptions : optionname optionvalue\n\
 .pul paduselay : laylist\n\
 .wlp wlistpin  : compnum x y norm/sing/diff/pstack paddir pinnum compname \\\n\
                  [padstacknumber]\n\
                  layer shape   ( 1-255 times )\n\
 .pop pin opts  : optionname optionvalue\n\
 .wle end wirelist\n\
 .pad via       : padshapenum x y layer paddirection\n\
 .bur buried via: psha,psha,.. x y lay,lay,.. paddirection\n\
 .tra track     : trackshapenum xstart ystart xend yend layer\n\
 .txt text      : textx texty layer tdir tmir options tsize trackshapenum text\n\
 .arc arc       : arcx arcy layer radius alpha beta trackshapenum\n\
 .end end      \n\
\n");

   return TRUE;
}

/*****************************************************************************/
/*
*/
static const char *get_ddelayername(int layernum)
{
   int   i;

   for (i=0;i<layermapcnt;i++)
   {
#ifdef _DEBUG
DDE_LayerMap *ll = layermaparray[i];
#endif
      if (layermaparray[i]->getLayerType() == LAYTYPE_NAME)
      {
         if (layermaparray[i]->ddelayernr == layernum)
            return layermaparray[i]->ddelayname.GetBuffer(0);
      }
   }

   return "";
}

/*****************************************************************************/
/*
*/
static int find_unused_stacknum(int signr)
{
   int   i, j;
   int   found = FALSE;

   // check is a same signalnr is already assigned to a stacknum 0..255
   if (signr)
   {
      for (i=0;i<maxArr;i++)
      {
         if (ddeArr[i]->signr == signr && ddeArr[i]->stackNum > -1)
            return ddeArr[i]->stackNum;
      }
   }

   // now find a free stacknumber 0..255, which was not assigned in .LAYERMAP in dde.out
   for (i=2;i<255;i++)  // to not do 0, 1
   {
      // if can not be in the layermaparray list
      found = FALSE;
      for (j=0;j<layermapcnt;j++)
      {
         if (layermaparray[j]->ddelayernr == i)
            found = TRUE;
      }
      if (found)  continue;

      // now check if i == next free 0..255 layer number
      found = FALSE;
      for (j=0;j<maxArr;j++)
      {
         if (ddeArr[j]->stackNum == i)
         {
            found = TRUE;
         }
      }  

      // if it was not found it must be free
      if (!found) 
         return i;
      // loop to next i number 2..254
   }
   
   // no more free layers between 0..255 free !!!
   ErrorMessage("No more Layers available");
   return LAY_UNKNOWN;
}

//--------------------------------------------------------------
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0, layernum;
   char        typ = 'D';
   CString     ddelay;

   int   j;
   int   doc_cnt = 0;
   int   highest_stacknum = 0;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.
      if (layer->getElectricalStackNumber() > highest_stacknum)
         highest_stacknum = layer->getElectricalStackNumber();
   }

   maxArr = 0;

   // set all mirror layers to visible
   for (j=0;j<doc->getMaxLayerIndex();j++)
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)      continue; // could have been deleted.

      LayerStruct *mirlayer = doc->getLayerArray()[layer->getMirroredLayerIndex()];

      if (mirlayer == NULL)   continue;

      if (layer->isVisible())  
         mirlayer->setVisible(true);
      else
      if (mirlayer->isVisible())  
         layer->setVisible(true);
   }

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.

      layernum = -1; // not assigned
      signr = 0;
      typ = 'D';
      ddelay = "";

      if (layer->isFloating())    continue;

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         layernum = get_layermap(layer->getLayerType(), signr);

         if (layer->getLayerType() == LAYTYPE_PAD_TOP ||
             layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
         {
            ddelay = "Top";
         }
         else
         if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM ||
             layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
         {
            ddelay = "Bottom";
         }
         else
         if (layer->getLayerType() == LAYTYPE_POWERPOS)
         {
            ddelay.Format("Inner %d",signr);
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER ||
             layer->getLayerType() == LAYTYPE_PAD_INNER)
         {
            ddelay.Format("Inner %d",signr);
         }
         else
         if (layer->getLayerType() == LAYTYPE_POWERNEG)
         {
            typ = 'P';
            ddelay.Format("Inner %d",signr);
         }
         else
         if (layer->getLayerType() == LAYTYPE_SPLITPLANE)
         {
            typ = 'P';
            ddelay.Format("Inner %d",signr);
         }
      }
      else
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;

         layernum = get_layermap(layer->getLayerType(), 0);
         ddelay = layer->getName();
         doc_cnt++;
      }
      

      //ddeArr.SetSizes
      DDELayerStruct *dde = new DDELayerStruct;
      dde->stackNum = layernum;  // layernum 0..255
      dde->setLayerType(layer->getLayerType());
      dde->signr = signr;        // camcad signal stack number
      dde->layerindex = j;       // Layerindex
      dde->on = TRUE;
      dde->type = typ;
      dde->oldName = layer->getName();
      dde->newName = get_ddelayername(layernum);
      ddeArr.SetAtGrow(maxArr++, dde);
   }

   // now here fill the stackNum -1 into "free" places 0..255
   for (j=0;j<maxArr;j++)
   {
#ifdef _DEBUG
      DDELayerStruct *lll = ddeArr[j];
#endif

      if (ddeArr[j]->stackNum < 0)
      {
         ddeArr[j]->stackNum = find_unused_stacknum(ddeArr[j]->signr);
         if (ddeArr[j]->stackNum < 0)
            ddeArr[j]->on = FALSE;
      }
   }

   return;
}

//--------------------------------------------------------------
static int edit_layerlist()
{
   // fill array
   DDELayer ddedlg;

   ddedlg.arr = &ddeArr;
   ddedlg.maxArr = maxArr;

   if (ddedlg.DoModal() != IDOK) 
      return FALSE;

   int   i;
   max_signalcnt = 0;

   for (i=0;i<maxArr;i++)
   {
      if (ddeArr[i]->signr > max_signalcnt)
         max_signalcnt = ddeArr[i]->signr;
   }
   return TRUE;
}

//--------------------------------------------------------------
static void free_layerlist()
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      delete ddeArr[i];
   }
   ddeArr.RemoveAll();

   return;
}


//--------------------------------------------------------------
// this gets ther component_placement_outline 
static int DDE_OutlineSHAPEData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary,
      int gr_class)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_TEXT)    continue;
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         // never do etch
         if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

         if ((layer = Layer_DDE(layer)) < 0) continue;

         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            continue;

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

         if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;

            if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
               block_layer = insertLayer;
            else
               block_layer = np->getLayerIndex();

            found += DDE_OutlineSHAPEData(&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, primary,gr_class);
         } // end else not aperture
      } // if INSERT

   } // end DDE_OutlineSHAPEData */

   return found;
}

/*****************************************************************************/
/*
*/
static int write_easyoutline(FILE *wfp, CPntList *pp, const char *kind, int ddelayer, int tshwidth)
{
   CPnt *p;
   double   scale = 1;
   
   if (pp == NULL)
   {
      return 0;
   }

   fprintf(wfp,".dfa %s %d %d 0ce1 0 *\n", kind, ddelayer, tshwidth);

   int first = TRUE;
   Point2   firstp;
   Point2   p1,p2;

   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      p2.x = p->x * scale;
      p2.y = p->y * scale;
      p2.bulge = p->bulge;
      // here deal with bulge
      if (first)
      {
         firstp = p2;
         p1 = p2;
         first = FALSE;
      }
      else
      {
         write_coo_cen(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y);
         p1 = p2;
      }
   }
   fprintf(wfp,".poe border\n");
   CString  prosa;
   prosa.Format("%s%d", kind, ++dfacnt);
   fprintf(wfp,".txt %ld %ld %d 0 %d LBi 5000 0 %s\n", 
      cnv_units(firstp.x), cnv_units(firstp.y), ddelayer, 0, prosa);

   fprintf(wfp,".ena\n");
   fprintf(wfp,"\n");

   return 1;
}

/****************************************************************************/
/*
   loop through all shapes
*/
static int write_iplshapes(FILE *fo)
{
   int      i;
   double   accuracy = get_accuracy(doc);

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( block->getBlockType() == BLOCKTYPE_PCBCOMPONENT ||  block->getBlockType() == BLOCKTYPE_TESTPOINT)
      {
         doc->CalcBlockExtents(block);
      
         IPLPCBShapes *c = new IPLPCBShapes;
         pcbshapesarray.SetAtGrow(pcbshapescnt,c);  
         pcbshapescnt++;   
         
         c->shapename = block->getName();
         c->blocknum  = block->getBlockNumber();

         CString  shapename;
         shapename = check_name('s', block->getName());

         // put to a 100 mil grid multiple
         double xoffset = 0;
         double yoffset = 0;

         if (block->getXmin() < 0)
         {
            xoffset = round_offset(-block->getXmin());
         }

         if (block->getYmin() < 0)
         {
            yoffset = round_offset(-block->getYmin());
         }

         // a shape can not be negative, to all graphics must be
         // offseted.
         fprintf(fo,".typ %ld %ld %ld %ld %d %s\n",
                       cnv_units(xoffset),
                       cnv_units(yoffset),
                       cnv_units(block->getXmax() + xoffset),
                       cnv_units(block->getYmax() + yoffset),
                       pcbshapescnt, shapename);

         write_iplshape(fo, &(block->getDataList()), xoffset, yoffset, 0.0, 0, 1, 0,-1);

         // component outline area
         // 0. find GR_CLASS_COMPOUTLINE
         // 1. find LAYTYPE_COMPONENTOUTLINE
         // 2. take all block xymin xymax
         // .cop comp height
            
         // component placement outline
         Outline_Start(doc);
         int found = DDE_OutlineSHAPEData(&(block->getDataList()), xoffset, yoffset, 0.0, 0, 
                                          1.0, 0, -1, TRUE, GR_CLASS_COMPONENT_BOUNDARY);
         if (!found)
         {
            found = DDE_OutlineSHAPEData(&(block->getDataList()), xoffset, yoffset, 0.0, 0, 
                                         1.0, 0, -1, TRUE, GR_CLASS_COMPOUTLINE);
            if (!found)
            {
               // now look for every visible outline
               int found = DDE_OutlineSHAPEData(&(block->getDataList()), xoffset, yoffset, 0.0, 0, 
                                                1.0, 0, -1, FALSE, GR_CLASS_COMPOUTLINE);
               if (!found)
               {
                  fprintf(flog,"Did not find a Component Outline for [%s].\n",block->getName());
                  display_error++;
               }
            }
         }
         int returnCode;
         if (!write_easyoutline(fo, Outline_GetOutline(&returnCode, accuracy), "cmpsize", sys_cmpsizelay.lay, 1))
         {
            fprintf(flog, "Error in easy outline for [%s]\n", block->getName());
            display_error++;
         }

         Outline_FreeResults();

         if (block->getBlockType() == BLOCKTYPE_TESTPOINT)
            fprintf(fo,".cop kind TEST\n");

         write_attributes(fo, ".cop", block->getAttributesRef());

         fprintf(fo,"\n");
      }
   }

   fprintf(fo,"\n");

   return 0;
}

/*****************************************************************************/
/*
*/
static int write_apertures(FILE *fp, BlockStruct *block, const char *polygon)
{
   double   scale = 1;

   long sizeA = cnv_units(block->getSizeA());
   long sizeB = cnv_units(block->getSizeB()); 
   long xoffset= cnv_units(block->getXoffset());
   long yoffset= cnv_units(block->getYoffset());

   switch (block->getShape())
   {
      case T_ROUND:
         fprintf(fp,".cir %ld\n", sizeA/2);
         fprintf(fp,".poe end %s\n", polygon);
      break;
      case T_OCTAGON:                  
      case T_SQUARE:
      case T_RECTANGLE:
      case T_OBLONG:    
      {
         // any other degree is complex
         POSITION polyPos, pntPos;
         CPolyList *polylist;
         CPoly *poly;
         CPnt *pnt;
         polylist = ApertureToPoly_Base(block, 0.0, 0.0, 0.0, 0); // uses block->rot

         if (!polylist)
            break;

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
                  write_coo_cen(fp, p1.x, p1.y, p1.bulge, p2.x, p2.y);
                  p1 = p2;
               }
            }
         }
         fprintf(fp,".poe end %s\n", polygon);

         FreePolyList(polylist);//this function in Polylib.cpp
      }
      break;
      case T_DONUT:                    
         fprintf(fp,".cir %ld\n", sizeA/2);
         fprintf(fp,".poe end %s\n", polygon);
      break;
      case T_COMPLEX:
      {
         fprintf(flog, "Complex Aperture [%d] not implemented\n", block->getShape());
         display_error++;

/*
         BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
         int apptyp = GENCAD_TestComplexApertureData(fp, &(subblock->getDataList()), 0.0, 0.0, rotation, 0, scale, 0);

         if (apptyp == PADSHAPE_CIRCLE)
            fprintf(fp,"PAD %s ROUND -1\n",padname);
         else
         if (apptyp == PADSHAPE_RECTANGULAR)
            fprintf(fp,"PAD %s RECTANGULAR -1\n",padname);
         else
            fprintf(fp,"PAD %s POLYGON -1\n",padname);
         GENCAD_WriteComplexApertureData(fp, &(subblock->getDataList()), 0.0, 0.0, rotation, 0, scale, 0, apptyp);
*/
      }
      break;
      default:
      {
         fprintf(flog, "Unknown Aperture [%d] for Geometry [%s]\n", block->getShape(), block->getName());
         display_error++;
      }
      break;
   }

   return 1;
}

/****************************************************************************/
/*
   .psh record
   write drills and apertures
*/
static int write_iplpads(FILE *fo, int pageUnits)
{
   int      i;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
      {
         IPLPadshapes *c = new IPLPadshapes;
         padshapesarray.SetAtGrow(padshapescnt,c);  
         padshapescnt++;   

         c->name = block->getName();
         c->blocknum  = block->getBlockNumber();
         c->drill =TRUE;
   
         double drill = get_drill_from_block_no_scale(doc, block);
         double dsize = drill * Units_Factor(pageUnits, UNIT_MM);

         if (TRUE) // plated
            fprintf(fo,".psh %d 0 n f _%1.2lfmm_p\n", padshapescnt, dsize);
         else
            fprintf(fo,".psh %d 0 n f _%1.2lfmm_np\n", padshapescnt, dsize);

/*
         // clearance used as via
         fprintf(fo,".cle 1 %ld %ld %ld %ld\n",
            inch_2_ipl(clearance_tt),
            inch_2_ipl(clearance_pt),
            inch_2_ipl(clearance_pt),
            inch_2_ipl(clearance_pt));
         // clearance used as pad
         fprintf(fo,".cle 2 %ld %ld %ld %ld\n",
            inch_2_ipl(clearance_tt),
            inch_2_ipl(clearance_pt),
            inch_2_ipl(clearance_pt),
            inch_2_ipl(clearance_pt));
         // clearance used as smd
         fprintf(fo,".cle 3 %ld %ld %ld %ld\n",
            inch_2_ipl(clearance_tt),
            inch_2_ipl(clearance_pt),
            inch_2_ipl(clearance_pt),
            inch_2_ipl(clearance_pt));
*/
         fprintf(fo,".cir %ld\n",cnv_units(drill/2));
         fprintf(fo,".poe end plotterpolygon\n");
         fprintf(fo,".cir %ld\n",cnv_units(drill/2));
         fprintf(fo,".poe end electricpolygon\n");
         fprintf(fo,".cir %ld\n",cnv_units(drill/2));
         fprintf(fo,".poe end graphicpolygon\n");

         fprintf(fo,"\n");

      }
      else
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {

         IPLPadshapes *c = new IPLPadshapes;
         padshapesarray.SetAtGrow(padshapescnt,c);  
         padshapescnt++;   
         
         c->name = block->getName();
         c->blocknum  = block->getBlockNumber();
         c->drill = FALSE;

         CString  padname;
         padname = block->getName();
         if (strlen(padname) == 0)
            padname.Format("pad_%d", padshapescnt);

         fprintf(fo,".psh %d 0 n f %s\n", padshapescnt, check_name('p', padname));

         // here apertures
         write_apertures(fo, block, "plotterpolygon");
         write_apertures(fo, block, "electricpolygon");
         write_apertures(fo, block, "graphicpolygon");

         fprintf(fo,"\n");
      }
   }


   // here make a dummy shape
   dummypsh = padshapescnt + 1;  
   fprintf(fo,".psh %d 0 p n dummy_shape\n", dummypsh);
   fprintf(fo,".cir 0\n");
   fprintf(fo,".poe end  plotterpolygon\n");
   fprintf(fo,".cir 0\n");
   fprintf(fo,".poe end  electricpolygon\n");
   fprintf(fo,".cir 0\n");
   fprintf(fo,".poe end  graphicpolygon\n");
   fprintf(fo,"\n");


   fprintf(fo,"\n");

   return 0;
}

/*****************************************************************************/
/*
*/
static double round_offset(double x)
{
   double   offset;
      
   // put to a 100 mil grid multiple
   long tmp = cnv_units(x);
   offset = (floor((double)tmp / 10000.) + 1) * 10000;
   offset = offset / cnv_units(1);

   return offset;
}

/****************************************************************************/
/*
*/
static int in_mirrorlist(int m1, int m2)
{
   int   i;

   for (i=0;i<mirrorcnt;i++)
   {
      if (mirrorarray[i]->mirrornum == m1 && mirrorarray[i]->stacknum == m2)  return TRUE;
      if (mirrorarray[i]->mirrornum == m2 && mirrorarray[i]->stacknum == m1)  return TRUE;
   }

   return FALSE;
}

/****************************************************************************/
/*
*/
static int write_ipllayers(FILE *fo)
{
   int      w, i = 0;
   int      lay255 = FALSE;
   // always write 0, 1, 255

   //update_mirrorlayer(layerlist,&layerlistcnt);

   // get every layer only once. elimitade double ipl index.
   for (w=0;w<=MAX_LAYER;w++)
   {
      for (i=0;i<maxArr;i++)
      {
         if (ddeArr[i]->on == FALSE)   continue;   // do not do switched off layers

         CString  t;
         DDELayerStruct *lll = ddeArr[i];

         if (w != ddeArr[i]->stackNum) continue;

         if (ddeArr[i]->stackNum == DDE_COMPSIDE)
            t= "oedC";  
         else
         if (ddeArr[i]->stackNum == DDE_SOLDERSIDE)
            t = "oedS";
         else
         if (is_electricallayer(ddeArr[i]->layertype))
         {
            t = "oed";

            if (ddeArr[i]->type == 'P')
               t += "P";
            else
               t += "I";          // d for display
         }
         else
            t = "d";

         if (ddeArr[i]->stackNum == 255)  // always !
         {
            fprintf(fo,".lay %d oed %s ' GENERIC255'\n", ddeArr[i]->stackNum,ddeArr[i]->newName);
            lay255 = TRUE;
         }
         else
         {
            CString lname, ltype;
            lname = ddeArr[i]->newName;
            ltype = get_layerusertype(ddeArr[i]->layertype, ddeArr[i]->signr);

            if (strlen(lname) == 0) lname.Format("L%d", ddeArr[i]->stackNum);

            fprintf(fo,".lay %d %s %s", ddeArr[i]->stackNum, t, lname);

            if (ddeArr[i]->type == 'P')
            {
               //fprintf(fo," %s", DDE_PLANE);
            }
            else
            if (ddeArr[i]->type == 'S')
            {
               //fprintf(fo," %s", DDE_INNER);
            }
            fprintf(fo," %s\n", ltype);
         }
         break;   // write every stacknumber 0..255 only once
      }
   }

   // this is the last layer
   if (!lay255) fprintf(fo,".lay 255 oed all ' GENERIC255'\n");


   fprintf(fo,"\n");

   for (w=0;w<doc->getMaxLayerIndex();w++)
   {
      LayerStruct *l = doc->getLayerArray()[w];
      if (l == NULL)    continue; // could have been deleted.

      if (l->getMirroredLayerIndex() != l->getLayerIndex())
      {
         int   m1, m2;
         m1 = Layer_DDE(l->getLayerIndex());
         m2 = Layer_DDE(l->getMirroredLayerIndex());
         if (m1 > -1 && m2 > -1)
         {
#ifdef _DEBUG
   DDELayerStruct *lm1 = ddeArr[m1];
   DDELayerStruct *lm2 = ddeArr[m2];
#endif
            // do not have combination
            // .mir 1 2
            // .mir 2 1
            if (!in_mirrorlist(m1, m2))
               fprintf(fo,".mir %d %d\n", ddeArr[m1]->stackNum, ddeArr[m2]->stackNum);
            
            ipl_layers[ddeArr[m1]->stackNum].mirror = ddeArr[m2]->stackNum;
            ipl_layers[ddeArr[m2]->stackNum].mirror = ddeArr[m1]->stackNum;

            IPLMirrorLayers *m = new IPLMirrorLayers;
            mirrorarray.SetAtGrow(mirrorcnt,m);  
            mirrorcnt++;   
            m->stacknum = m1;
            m->mirrornum = m2;
         }
      }
   }

   fprintf(fo,"\n");

   return 1;
}

/****************************************************************************/
/*
.sys layorder 13,121,120,0,100,2,101,3,102,4,103,5,104,1,122,123,23
*/
static int write_ipllayorder(FILE *fo)
{
   int      w, i = 0;
   int      first = TRUE;

   fprintf(fo,".sys layorder ");

   // get every layer only once. elimitade double ipl index.
   for (w=0;w<MAX_LAYER;w++)
   {
      for (i=0;i<maxArr;i++)
      {
         if (ddeArr[i]->on == FALSE)   continue;   // do not do switched off layers
         if (w != ddeArr[i]->stackNum) continue;
         if (!first) fprintf(fo,",");
         fprintf(fo,"%d", w);
         first = FALSE;
         break;
      }
   }
   fprintf(fo,"\n"); 

   return 1;
}

/****************************************************************************/
/*
*/
static int layerindex_by_signr(int s)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (ddeArr[i]->signr == s)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
   return -1 if not visible
*/
static int Layer_DDE(int l)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (ddeArr[i]->on == 0) continue;   // not visible
      if (ddeArr[i]->layerindex == l)
         return i;
   }

   // fprintf(flog,"Layer [%s] not translated.\n", doc->LayerArray[l]->name);
   return -1;
}

/****************************************************************************/
/*
*/
static int write_ipltrackcode(FILE *fo)
{

   int i;
   BlockStruct *block;

   for (i=0;i<doc->getNextWidthIndex();i++)
   {
      block = doc->getWidthTable()[i];

      if (block->getFlags() & BL_WIDTH) 
      {
         fprintf(fo,".tsh %d f TRACKCODE_%d\n",i,i);

         fprintf(fo,".cir %ld\n",cnv_units(doc->getWidthTable()[i]->getSizeA()/2));  // radius
         fprintf(fo,".poe end  plotterpolygon\n");
         fprintf(fo,".cir %ld\n",cnv_units(doc->getWidthTable()[i]->getSizeA()/2));
         fprintf(fo,".poe end  electricpolygon\n");
         fprintf(fo,".cir %ld\n",cnv_units(doc->getWidthTable()[i]->getSizeA()/2));
         fprintf(fo,".poe end  graphicpolygon\n");
         fprintf(fo,"\n");
      }
   }

   fprintf(fo,"\n");
   return 1;
}

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
int IPL_GetPADSTACKData(CDataList *DataList, int insertLayer, IPLPadstack *pstack)
{
   DataStruct *np;
   unsigned long  pl = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD)    break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE)    break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLSYMBOL) break;

            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (block->getFlags() & BL_TOOL) 
            {
               pstack->drill = block->getToolSize();
               pstack->drillform =  get_padshapeptr(block->getName())+1;
               break;
            }
            
            if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
            {
               // block_layer is not set.
               if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               {
                  if (block->getShape() == T_COMPLEX)
                  {
                     block = doc->Find_Block_by_Num((int)block->getSizeA());
                  }
                  block_layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
               }
            }

            int shapenr = get_padshapeptr(block->getName()) + 1;
         
            LayerStruct *l = doc->FindLayer(block_layer);
            if (l->getLayerType() == LAYTYPE_PAD_ALL)
            {
               for (int w=1;w<=max_signalcnt;w++)
               {
                  int ll = layerindex_by_signr(w);
                  if (ll > -1)
                  {
                     DDELayerStruct *lll = ddeArr[ll];

                     int lindex = ddeArr[ll]->stackNum;
                     pstack->shapes[lindex].pshnr = shapenr;
                  }
               }
            }
            else
            if (l->getLayerType() == LAYTYPE_SIGNAL_INNER || l->getLayerType() == LAYTYPE_PAD_INNER)
            {
               for (int w=2;w<=max_signalcnt-1;w++)
               {
                  int ll = layerindex_by_signr(w);
                  if (ll > -1)
                  {
                     DDELayerStruct *lll = ddeArr[ll];

                     int lindex = ddeArr[ll]->stackNum;
                     pstack->shapes[lindex].pshnr = shapenr;
                  }
               }
            }
            else
            {
               // inner, all, outer
               int ll = Layer_DDE(block_layer);
               if (ll < 0) 
                  break;
               DDELayerStruct *lll = ddeArr[ll];
               int lindex = ddeArr[ll]->stackNum;

               if (lindex < 0)   
                  break;
               pstack->shapes[lindex].pshnr = shapenr;
            }
         } // case INSERT
         break;
      } // end switch
   } 

   return 0;
} /* end IPL_GetPadstackData */

/****************************************************************************/
/*
*/
static CString get_layerusertype( int laytyp, int signr)
{
   int      i;
   CString  uname;

   for (i=0;i<layerusertypecnt;i++)
   {
      if (layerusertypearray[i]->layertype == laytyp)
      {
         if (signr)
         {
            if (laytyp == LAYTYPE_SIGNAL_INNER ||
                laytyp == LAYTYPE_PAD_INNER ||
                laytyp == LAYTYPE_POWERPOS) 
            {
               uname.Format("%s%d", layerusertypearray[i]->usertypename, signr-1);
               return uname;
            }
            else
            if (laytyp == LAYTYPE_POWERNEG || 
                laytyp == LAYTYPE_SPLITPLANE) 
            {
               uname.Format("%s%d", layerusertypearray[i]->usertypename, signr-1);
               return uname;
            }
         }

         return layerusertypearray[i]->usertypename;
      }
   }

   return "";
}

/****************************************************************************/
/*
   smd must have top and bottom
*/
static int write_ipl_uls(FILE *fp, int ptr, int smd)
{
   int   w;

   for (w=0;w<MAX_LAYER;w++)
   {
      if (padstackarray[ptr]->shapes[w].pshnr > 0) 
      {
         // this must have the user layer type name, as defined in .lpm
         CString userlayertype;
         for (int w1=0;w1<maxArr;w1++)
         {
            if (ddeArr[w1]->on == FALSE)  continue;   // do not do switched off layers
            if (w != ddeArr[w1]->stackNum) continue;
            userlayertype = get_layerusertype(ddeArr[w1]->layertype, ddeArr[w1]->signr);
            if (strlen(userlayertype))
            {
               fprintf(fp,".uls %s %d %d\n", userlayertype, 
                  padstackarray[ptr]->shapes[w].pshnr, padstackarray[ptr]->shapes[w].pshnr); // top
            }
            break;
         }
      }
   }

   // I still must evaluate mirror only / never mirror !!!
   if (smd) // must also define the bottom layer
   {
      for (w=0;w<MAX_LAYER;w++)
      {
         if (padstackarray[ptr]->shapes[w].pshnr > 0) 
         {
            int w2 = ipl_layers[w].mirror;

            if (w2 == w)   continue;   // if not mirrored, do not write it.

            // this must have the user layer type name, as defined in .lpm
            CString userlayertype;
            for (int w1=0;w1<maxArr;w1++)
            {
               if (ddeArr[w1]->on == FALSE)  continue;   // do not do switched off layers
               if (w2 != ddeArr[w1]->stackNum) continue;
               userlayertype = get_layerusertype(ddeArr[w1]->layertype, ddeArr[w1]->signr);
               if (strlen(userlayertype))
               {
                  fprintf(fp,".uls %s %d %d\n", userlayertype, 
                     padstackarray[ptr]->shapes[w].pshnr, padstackarray[ptr]->shapes[w].pshnr); // top
               }
               break;
            }
         }
      }  
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int write_iplpadstacks(FILE *fp)
{
   int   i;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         IPLPadstack *p = new IPLPadstack;
         padstackarray.SetAtGrow(padstackcnt,p);  
         padstackcnt++; 

         for (int w=0;w<MAX_LAYER;w++)
         {
            p->shapes[w].pshnr = -1;
            p->shapes[w].layernr = -1;
         }
         p->name = block->getName();
         p->vianame = block->getName();
         p->bnum = block->getBlockNumber();
         p->drill = 0;
         p->smd = 0;
         p->drillform = -1;

         if (is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 2))
            p->smd = TRUE;

         // attributes
         IPL_GetPADSTACKData(&(block->getDataList()), -1, p);

         //write_attributes(fp, block->getAttributesRef(), "PADST", &padstackattcnt);
      }
   }

   fprintf(fp,"\n");

   // here need to write .pst section
   int   stackcnt = 0;
   for (i=0;i<usedpadstackcnt;i++) // this is all uswed padstacks found in the design
   {
      if (usedpadstackarray[i]->use == 1) // only vias
         continue;

      int ptr = get_padstackptr(usedpadstackarray[i]->bnum);
#ifdef _DEBUG
      IPLUsedPadstackStruct *vv = usedpadstackarray[i];
      IPLPadstack  *pp = padstackarray[ptr];
#endif

      char  plated  = 'p';
      char  padtype = 'n';
      // p = pad n = nonplated n = nonfilled
      // s = smd
      if (padstackarray[ptr]->smd)  
      {
         padtype = 's';
         plated = 'n';
      }
      fprintf(fp,".pst %d %ld %c n %c %s\n",++stackcnt, 
         cnv_units(padstackarray[ptr]->drill), plated, padtype, usedpadstackarray[i]->padstackname);

      fprintf(fp,".pla 0 0 1182 1182 circle45 iso y y y\n");

      padstackarray[ptr]->stackindex = stackcnt;

      write_ipl_uls(fp, ptr, padstackarray[ptr]->smd);

      if (padstackarray[ptr]->drillform > -1)
      {
         fprintf(fp,".uls ' GENERIC255' %d\n",padstackarray[ptr]->drillform);
      }

      fprintf(fp,".pse\n");
      fprintf(fp,"\n");
   }

   for (i=0;i<usedpadstackcnt;i++)
   {
      if (!(usedpadstackarray[i]->use & 1))  // not used as via
         continue;

#ifdef _DEBUG
      IPLUsedPadstackStruct *vv = usedpadstackarray[i];
#endif

      CString vname;
      if (usedpadstackarray[i]->use == 1) // only vias
      {
         // keep original name
         vname = usedpadstackarray[i]->padstackname;
      }
      else
      {
         // change via_name
         vname.Format("VIA_%s", usedpadstackarray[i]->padstackname);
      }
      int ptr = get_padstackptr(usedpadstackarray[i]->bnum);
      padstackarray[ptr]->vianame = vname;

      //n = nonplated n = nonfilled
      // p = pad 
      // v = via
      // s = smd
      fprintf(fp,".pst %d %ld p n v %s\n",++stackcnt, 
         cnv_units(padstackarray[ptr]->drill), vname);

      fprintf(fp,".pla 0 0 1182 1182 circle45 iso y y y\n");
      padstackarray[ptr]->viastackindex = stackcnt;

      write_ipl_uls(fp, ptr, FALSE);

      // viastack has no 255 reference
      //if (padstackarray[ptr]->drillform > -1)
      // fprintf(fp,".uls ' GENERIC255' %d\n",padstackarray[ptr]->drillform);

      fprintf(fp,".pse\n");
      fprintf(fp,"\n");
   }

   fprintf(fp,"\n");

   return 1;
}

//--------------------------------------------------------------
static int used_as_via(const char *v, int use)
{
   int   i;

   for (i=0;i<usedpadstackcnt;i++)
   {
      if (usedpadstackarray[i]->padstackname.Compare(v) == 0)
      {
         usedpadstackarray[i]->use |= use;
         return 1;
      }
   }

   return 0;
}

//--------------------------------------------------------------
static int get_viaptr(const char *v)
{
   int   i;

   for (i=0;i<usedpadstackcnt;i++)
   {
      if (usedpadstackarray[i]->padstackname.Compare(v) == 0)
      {
         return i;
      }
   }

   return -1;
}

//--------------------------------------------------------------
static void get_vianames(CDataList *DataList)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL ||
                np->getInsert()->getInsertType() == INSERTTYPE_PIN ||
                np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN ||
                np->getInsert()->getInsertType() == INSERTTYPE_TESTPAD)
            {
               if (!used_as_via(block->getName(), 2)) 
               {
                  IPLUsedPadstackStruct *v = new IPLUsedPadstackStruct;
                  usedpadstackarray.SetAtGrow(usedpadstackcnt, v);
                  usedpadstackcnt++;
                  v->padstackname = block->getName();   
                  v->bnum = block->getBlockNumber();
                  v->use = 2;
               }
               break;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA ||
                np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {
               if (!used_as_via(block->getName(), 1)) 
               {
                  IPLUsedPadstackStruct *v = new IPLUsedPadstackStruct;
                  usedpadstackarray.SetAtGrow(usedpadstackcnt, v);
                  usedpadstackcnt++;
                  v->padstackname = block->getName();      
                  v->bnum = block->getBlockNumber();
                  v->use = 1;
               }
               break;
            }

            if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               get_vianames(&(block->getDataList()));
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end get_vianames */
   return;
}

//--------------------------------------------------------------
void DDE_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int pageUnits)
{
   FILE *wfp;

   doc = Doc;
   display_error = FALSE;
   dfacnt = 0;

   scale = Units_Factor(pageUnits, UNIT_MILS);

   routesarray.SetSize(100,100);
   routescnt = 0;

   pcbshapesarray.SetSize(100,100);
   pcbshapescnt = 0;

   padshapesarray.SetSize(100,100);
   padshapescnt = 0;

   mirrorarray.SetSize(100,100);
   mirrorcnt = 0;

   padstackarray.SetSize(100,100);
   padstackcnt = 0;

   pcbcomponentsarray.SetSize(100,100);
   pcbcomponentscnt = 0;

   layermaparray.SetSize(10,10);
   layermapcnt = 0;

   layerusertypearray.SetSize(10,10);
   layerusertypecnt = 0;

   usedpadstackarray.SetSize(10,10);
   usedpadstackcnt = 0;

	CString logFile = GetLogfilePath("dde.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }
   
   CString settingsFile( getApp().getExportSettingsFilePath("dde.out") );
   load_iplsettings(settingsFile);

   CString checkFile( getApp().getSystemSettingsFilePath("dde.chk") );
   check_init(checkFile);

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %s",filename);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }

// here do output

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file
      
      double   xoffset = 0, yoffset = 0;

      doc->CalcBlockExtents(file->getBlock());
      generate_PINLOC(doc,file,0);     // this function generates the PINLOC argument for all pins.

      get_vianames(&(file->getBlock()->getDataList()));

      if (file->getBlock()->getXmin() < 0)
      {
         xoffset = round_offset(-file->getBlock()->getXmin());
      }

      if (file->getBlock()->getYmin() < 0)
      {
         yoffset = round_offset(-file->getBlock()->getYmin());
      }

      do_layerlist();

      if (edit_layerlist())
      {
         CWaitCursor wait;

         // filename header
         write_iplheader(wfp, filename, file->getBlock(), file->getInsertX() + xoffset, file->getInsertY() + yoffset);

         // userlayers
         fprintf(wfp,".lpm ' GENERIC255' elec D\n");
         for (int w=0;w<MAX_LAYER;w++)
         {
            for (int w1=0;w1<maxArr;w1++)
            {
               if (ddeArr[w1]->on == FALSE)  continue;   // do not do switched off layers
               if (w != ddeArr[w1]->stackNum) continue;

               //if (strlen(ddeArr[w1]->newName) == 0)   continue;
               
               CString  systemlaytypename, polyg;
               systemlaytypename = "graph";
               polyg = "p";

               if (ddeArr[w1]->getLayerType() == LAYTYPE_SIGNAL_TOP ||
                   ddeArr[w1]->getLayerType() == LAYTYPE_PAD_TOP) 
                  systemlaytypename = "comp", polyg = "pc";
               else
               if (ddeArr[w1]->getLayerType() == LAYTYPE_SIGNAL_BOT || 
                   ddeArr[w1]->getLayerType() == LAYTYPE_PAD_BOTTOM) 
                  systemlaytypename = "solder", polyg = "ps";
               else
               if (ddeArr[w1]->getLayerType() == LAYTYPE_SIGNAL_INNER ||
                   ddeArr[w1]->getLayerType() == LAYTYPE_PAD_INNER ||
                   ddeArr[w1]->getLayerType() == LAYTYPE_POWERPOS) 
               {
                  systemlaytypename = "inner", polyg = "g";
               }
               else
               if (ddeArr[w1]->getLayerType() == LAYTYPE_POWERNEG || 
                   ddeArr[w1]->getLayerType() == LAYTYPE_SPLITPLANE) 
               {
                  systemlaytypename = "plane", polyg = "p";
               }
               CString userlayertype;
               userlayertype = get_layerusertype(ddeArr[w1]->layertype, ddeArr[w1]->signr);
               if (strlen(userlayertype))
                  fprintf(wfp,".lpm %s %s %s\n", userlayertype, systemlaytypename, polyg);      // solder
               break; // only find the first one !
            }
         }
         fprintf(wfp,"\n");

         write_ipllayers(wfp);

         // trackshapes
         write_ipltrackcode(wfp);

         // padshapes, write all apertures
         write_iplpads(wfp, pageUnits);

         // padstacks
         write_iplpadstacks(wfp);

         // fonts - not needed, loads IPL default

         // some sys parameters
         fprintf(wfp,".sys compnamelays %d\n",sys_compnamelay);
         fprintf(wfp,".sys devicelay %d\n",sys_devicelay);
         fprintf(wfp,".sys articlelay %d\n",sys_articlelay);
         write_ipllayorder(wfp);
         fprintf(wfp,".sys savegrpwise y\n");
         fprintf(wfp,"\n");

         // typedefs shapes
         write_iplshapes(wfp);
   
         // comp placement
         write_iplcomponents(wfp, &(file->getBlock()->getDataList()), 
                  file->getInsertX() + xoffset, file->getInsertY() + yoffset ,
                  file->getRotation(), file->isMirrored(), file->getScale(), 0,-1);

         // pin list - a pin for every component.
         // wirelist group
         write_iplroutes(wfp, file, &file->getNetList(),
                  file->getInsertX() + xoffset, file->getInsertY() + yoffset ,
                  file->getRotation(), file->isMirrored(), file->getScale(), 0,-1);

         //fprintf(fo,"\n");
         write_iplmechanicalpins(wfp, &(file->getBlock()->getDataList()), 
                  file->getInsertX() + xoffset, file->getInsertY() + yoffset ,
                  file->getRotation(), file->isMirrored(), file->getScale(), 0,-1);

         fprintf(wfp,".wle\n\n");

         // all other graphic, area text
         write_iplartwork(wfp, &(file->getBlock()->getDataList()), 
                  file->getInsertX() + xoffset, file->getInsertY() +yoffset,
                  file->getRotation(), file->isMirrored(), file->getScale(), 0,-1);
         // end
         fprintf(wfp,"\n.end\n");

         break;
      } // edit_layer
   }

   int i;
   
   for (i=0;i<maxArr;i++)
   {
      fprintf(flog,"DDE Layer %d Stacknum %d Signr %d Index %d Type '%c' CType %s CC %s DDE %s\n", i,
         ddeArr[i]->stackNum,       // dde layer number
         ddeArr[i]->signr,          // signal number, same for PAD_2 and SIGNAL_2
         ddeArr[i]->layerindex,        // layerindex for CAMCAD.
         ddeArr[i]->type,              // S = Signal, D = Document, P = Power
         layerTypeToString(ddeArr[i]->layertype),         // Layer->getLayerType()
         ddeArr[i]->oldName,           // CAMCAD name
         ddeArr[i]->newName);          // new lookup name

      display_error++;
   }

   free_layerlist();

   // close write file
   fclose(wfp);

   if (check_report(flog)) display_error++;
   check_deinit();

   fclose(flog);


   for (i=0;i<routescnt;i++)
   {
      delete routesarray[i];
   }
   routesarray.RemoveAll();
   routescnt = 0;

   for (i=0;i<pcbshapescnt;i++)
   {
      delete pcbshapesarray[i];
   }
   pcbshapesarray.RemoveAll();
   pcbshapescnt = 0;

   for (i=0;i<usedpadstackcnt;i++)
   {
      delete usedpadstackarray[i];
   }
   usedpadstackarray.RemoveAll();

   for (i=0;i<padshapescnt;i++)
   {
      delete padshapesarray[i];
   }
   padshapesarray.RemoveAll();
   padshapescnt = 0;

   for (i=0;i<mirrorcnt;i++)
   {
      delete mirrorarray[i];
   }
   mirrorarray.RemoveAll();
   mirrorcnt = 0;

   for (i=0;i<padstackcnt;i++)
   {
      delete padstackarray[i];
   }
   padstackarray.RemoveAll();
   padstackcnt = 0;

   for (i=0;i<pcbcomponentscnt;i++)
   {
      delete pcbcomponentsarray[i];
   }
   pcbcomponentsarray.RemoveAll();
   pcbcomponentscnt = 0;

   for (i=0;i<layermapcnt;i++)
   {
      delete layermaparray[i];
   }
   layermaparray.RemoveAll();
   layermapcnt = 0;

   for (i=0;i<layerusertypecnt;i++)
   {
      delete layerusertypearray[i];
   }
   layerusertypearray.RemoveAll();
   layerusertypecnt = 0;

   if (display_error)
      Logreader(logFile);

   return;
}

/*****************************************************************************/
/*
*/
static int write_ipl_freepad(FILE *wfp, BlockStruct *block, double x, double y)
{
   int stack = get_padstackptr(block->getBlockNumber());
   
   for (int w=0;w<MAX_LAYER;w++)
   {
      if (padstackarray[stack]->shapes[w].pshnr > 0)  
      {
         fprintf(wfp,".pad %d %ld %ld %d 0\n", 
            padstackarray[stack]->shapes[w].pshnr, cnv_units(x), cnv_units(y), ipl_layers[w]);
      }
   }

   if (padstackarray[stack]->drillform > -1)
   {
      fprintf(wfp,".pad %d %ld %ld 255 0\n", padstackarray[stack]->drillform, cnv_units(x), cnv_units(y));
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_ipl_bur(FILE *wfp, BlockStruct *block, double x, double y)
{

   int stack = get_padstackptr(block->getBlockNumber());

   fprintf(wfp,".bur ");
   int first = TRUE;
   int w=0;

   for (w=0;w<MAX_LAYER;w++)
   {
      if (padstackarray[stack]->shapes[w].pshnr > 0)  
      {
         if (!first) fprintf(wfp,",");
         fprintf(wfp,"%d", padstackarray[stack]->shapes[w].pshnr);
         first = FALSE;
      }
   }

   if (first)  // no shape written, i.e. only drill
   {
      fprintf(wfp,"%d,%d", dummypsh, dummypsh);
   }

   fprintf(wfp," %ld %ld ",cnv_units(x), cnv_units(y));

   first = TRUE;
   for (w=0;w<MAX_LAYER;w++)
   {
      if (padstackarray[stack]->shapes[w].pshnr > 0)  
      {
         if (!first) fprintf(wfp,",");
         fprintf(wfp,"%d", w);
         first = FALSE;
      }
   }

   if (first)  // no shape written, i.e. only drill
   {
      fprintf(wfp,"%d,%d", DDE_COMPSIDE, DDE_SOLDERSIDE);
   }

   fprintf(wfp," 0 %d", padstackarray[stack]->viastackindex);   // rotation - via is never rotated.

   if (padstackarray[stack]->drillform > -1)
   {
      fprintf(wfp," 255");
      //fprintf(wfp," %d 255",padstackarray[stack]->drillform);
   }

   fprintf(wfp,"\n");

   return 1;
}

//--------------------------------------------------------------
//
static int iselectrical_dde(int ddelayer)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
#ifdef _DEBUG
   DDELayerStruct *dd = ddeArr[i];
#endif
      //if (ddeArr[i]->newName == 0)            continue;   // not visible
      if (ddeArr[i]->stackNum != ddelayer)   continue;
      return is_electricallayer(ddeArr[i]->layertype);
   }

   return FALSE;
}

/*****************************************************************************/
/*
   .dfa
   .coo ....
   .poe border (end of a outline)
   .poe borderpart (end of boundary outline)

   best area def

   .dfa
   .coo
   .poe borderpart <=== boundary outline
   .coo
   .poe border <== visible area
   .

*/
static int write_dfa_area(FILE *wfp, CPolyList *polylist, int graphic_class, const char *netname,
                          int ddelayer, double insert_x, double insert_y,
                          double rotation, int mirror, double scale, int polyFilled)
{
   int first = TRUE;
   Point2   firstp;
   Point2   p1,p2;
   int      start_dfa = FALSE;
   CString  planetype;
   Mat2x2   m;
   POSITION polyPos, pntPos;
   CPnt     *pnt;
   CPoly    *poly;

   RotMat2(&m, rotation);

   int   startpass = 0;
   int   endpass = 0;

   int   widthindex;
   polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = polylist->GetNext(polyPos);
      widthindex = poly->getWidthIndex();
   }

   if (graphic_class == GR_CLASS_BOARDOUTLINE)  
   {
      fprintf(wfp,".dfa miller %d %d 0c 0 []\n", ddelayer, widthindex);
   }
   else
   {
      if (graphic_class == GR_CLASS_ROUTKEEPIN) 
         planetype = "tki";
      else
      if (graphic_class == GR_CLASS_ROUTKEEPOUT)   
         planetype = "tko";
      else
      if (graphic_class == GR_CLASS_PLACEKEEPIN)   
         planetype = "cki";
      else
      if (graphic_class == GR_CLASS_PLACEKEEPOUT)  
         planetype = "cko";
      else
      if (graphic_class == GR_CLASS_VIAKEEPIN)  
         planetype = "vki";
      else
      if (graphic_class == GR_CLASS_VIAKEEPOUT) 
         planetype = "vko";
      else
      if (graphic_class == GR_CLASS_ALLKEEPIN)  
      {
         //planetype = "vki+cki+tki";
         planetype = "vki";
         startpass = 1;
         endpass = 3;
      }
      else
      if (graphic_class == GR_CLASS_ALLKEEPOUT) 
      {
         //planetype = "vko+cko+tko";
         planetype = "vko";
         startpass = 4;
         endpass = 6;
      }
      else
      if (polyFilled)
         planetype = "oha+edge";
      else
         planetype = "plane";

      fprintf(wfp,".dfa %s %d %d 0c 0", planetype, ddelayer, poly->getWidthIndex());
      if (strlen(netname) == 0)
         fprintf(wfp," *\n");
      else
         fprintf(wfp," %s\n", netname);
   }
   start_dfa = TRUE;

   // need to go multiple passed for all keepout

   for (int i=startpass;i<=endpass;i++)
   {
      switch (i)
      {
         case 2:
            fprintf(wfp,".dfa %s %d %d 0c 0", "cki", ddelayer, widthindex);
            if (strlen(netname) == 0)
               fprintf(wfp," *\n");
            else
               fprintf(wfp," %s\n", netname);
         break;
         case 3:
            fprintf(wfp,".dfa %s %d %d 0c 0", "tki", ddelayer, widthindex);
            if (strlen(netname) == 0)
               fprintf(wfp," *\n");
            else
               fprintf(wfp," %s\n", netname);
         break;
         case 5:
            fprintf(wfp,".dfa %s %d %d 0c 0", "cko", ddelayer, widthindex);
            if (strlen(netname) == 0)
               fprintf(wfp," *\n");
            else
               fprintf(wfp," %s\n", netname);
         break;
         case 6:
            fprintf(wfp,".dfa %s %d %d 0c 0", "tko", ddelayer, widthindex);
            if (strlen(netname) == 0)
               fprintf(wfp," *\n");
            else
               fprintf(wfp," %s\n", netname);
         break;
         default:
            // all is done alreadcy
         break;
      }

      polyPos = polylist->GetHeadPosition();
      while (polyPos != NULL)
      {
         poly = polylist->GetNext(polyPos);

         BOOL polyFilled = poly->isFilled();
         BOOL closed     = poly->isClosed();

         // do not write the boundary
         if (poly->isFloodBoundary()) break;
         if (poly->isVoid())     break;

         double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
   
         // here the first not boundary
         // .dfa // outline 
         if (poly->isHatchLine())
         {
            // nothing
         }
         else
         if (poly->isVoid())
         {
            fprintf(wfp,".iad\n");
            start_dfa = TRUE;
         }

         double cx, cy, radius;
         if (PolyIsCircle(poly, &cx, &cy, &radius))
         {
            //Same circle described but here the radius point is to the right of the
            //center
            //.cen 100000 100000 0
            //.coo 150000 100000
            p2.x = cx * scale;
            if (mirror) p2.x = -p2.x;
            p2.y = cy * scale;
            TransPoint2(&p2, 1, &m, insert_x, insert_y);
            radius = radius * scale;
            fprintf(wfp,".cen %ld %ld 0\n", cnv_units(p2.x), cnv_units(p2.y));
            fprintf(wfp,".coo %ld %ld\n", cnv_units(p2.x-radius), cnv_units(p2.y));
         }
         else
         {
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
                  if (poly->isHatchLine())
                     write_trace_arc(wfp, poly->getWidthIndex(), ddelayer, p1.x, p1.y, p1.bulge, p2.x, p2.y);
                  else
                     write_coo_cen(wfp, p1.x, p1.y, p1.bulge, p2.x, p2.y);
                  p1 = p2;
               }
            } // polyiscircle
            // here write last one !
            if (!first && !poly->isHatchLine())
               write_coo_cen(wfp, p1.x, p1.y, p1.bulge, firstp.x, firstp.y);
         }
         fprintf(wfp,".poe border\n");
      }

      if (start_dfa) 
      {
         CString  prosa;
         prosa.Format("%s%d", "dfa", ++dfacnt);
         fprintf(wfp,".txt %ld %ld %d 0 %d LBi 5000 0 %s\n", 
            cnv_units(firstp.x), cnv_units(firstp.y), ddelayer, 0, prosa);

         fprintf(wfp,".ena\n");
      }
   } // multiple passed for ALL Keepout

   fprintf(wfp,"\n");

   return 1;
}

//--------------------------------------------------------------
//
static int write_dde_poly(FILE *wfp, DataStruct *np, int ddelayer, double insert_x, double insert_y,
      double rotation, int mirror, double scale)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   CPoly *poly;
   CPnt  *pnt;
   POSITION polyPos, pntPos;
   BOOL  closed = FALSE;
   BOOL  filled = FALSE;

   // loop thru polys
   polyPos = np->getPolyList()->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = np->getPolyList()->GetNext(polyPos);
      if (poly->isClosed()) closed = TRUE;
      if (poly->isFilled()) filled = TRUE;

   }

   int plane_eval = (iselectrical_dde(ddelayer) && filled);

   // also these classes make a .dfa 
   if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE) plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_ROUTKEEPIN)   plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_ROUTKEEPOUT)  plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_PLACEKEEPIN)  plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_PLACEKEEPOUT) plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_VIAKEEPIN)    plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_VIAKEEPOUT)   plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_ALLKEEPIN)    plane_eval = TRUE;
   if (np->getGraphicClass() == GR_CLASS_ALLKEEPOUT)   plane_eval = TRUE;

   if (closed && plane_eval)
   {
      // here check if ATTR_NETNAME == netname
      Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
      CString  netname, planetype;
      if (a)
         netname = check_name('n',get_attvalue_string(doc, a));
      else
         netname = "";

      write_dfa_area(wfp, np->getPolyList(), np->getGraphicClass(), netname, ddelayer, 
            insert_x, insert_y, rotation, mirror,  scale, filled);

   }
   else
   {
      // this is done in the route section.
      if (np->getGraphicClass() == GR_CLASS_ETCH)                  return 0;
      if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))     return 0;

      // open polyline, trace, arc
      polyPos = np->getPolyList()->GetHeadPosition();
      while (polyPos != NULL)
      {
         poly = np->getPolyList()->GetNext(polyPos);

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
               write_trace_arc(wfp, poly->getWidthIndex(), ddelayer, p1.x, p1.y, p1.bulge, p2.x, p2.y);
               p1 = p2;
            }
         }
      }  // while
   } // if closed

   return 1;
}

//--------------------------------------------------------------
static void write_iplartwork(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer, ddelayer;
   int   artworkattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if ((layer = Layer_DDE(layer)) < 0) continue;
         ddelayer = ddeArr[layer]->stackNum;

      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            write_dde_poly(wfp, np, ddelayer, insert_x, insert_y, rotation, mirror, scale);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {

            point2.x = (np->getText()->getPnt().x)*scale;
            if (mirror) point2.x = -point2.x;

            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();

            fprintf(wfp,".txt %ld %ld %d %s %d %s %ld 1 \'",
               cnv_units(point2.x), cnv_units(point2.y),
               ddelayer, cnv_rotation(text_rot), (text_mirror) ? 1: 0,
               "LB", cnv_units(np->getText()->getHeight()));

            for (int i=0; i<(int)strlen(np->getText()->getText()); i++)
            {
               if (np->getText()->getText()[i] == '\'')
                  fprintf(wfp,"\\");
               fprintf(wfp,"%c",np->getText()->getText()[i]);
            }
            fprintf(wfp,"\'\n");

            //write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
         }  
         break;
         
         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;

            if (is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1))     break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA ||
                np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {

               // .bur 63,63,63,63 297047 184719 0,1,50,51 0 46 255

               //sprintf(vname,"VIA_%s",PadStackName);
               write_ipl_bur(wfp, block, point2.x, point2.y);
               // attributes
               //write_attributes(wfp, np->getAttributesRef(), "VIA", &viaattcnt);

               break;
            }

            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
                  {
                     wpfw_Graph_Level(lname);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle());
                  }
*/
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               write_iplartwork(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end write_iplartwork */
}

/*****************************************************************************/
/*
   do one shape
*/
static void write_iplshape(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer, ddelayer;
   int   artworkattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         //if ((np->getFlags() & GR_CLASS_BOARD_GEOM) == GR_CLASS_BOARD_GEOM) continue;
         //if ((np->getFlags() & GR_CLASS_ETCH)     == GR_CLASS_ETCH)         continue;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if ((layer = Layer_DDE(layer)) < 0) continue;
         ddelayer = ddeArr[layer]->stackNum;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            write_dde_poly(wfp, np, ddelayer, insert_x, insert_y, rotation, mirror, scale);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {

            point2.x = (np->getText()->getPnt().x)*scale;
            if (mirror) point2.x = -point2.x;

            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();

            fprintf(wfp,".txt %ld %ld %d %s %d %s %ld 1 \'",
               cnv_units(point2.x), cnv_units(point2.y),
               ddelayer, cnv_rotation(text_rot), (text_mirror) ? 1: 0,
               "LB", cnv_units(np->getText()->getHeight()));

            for (int i=0; i<(int)strlen(np->getText()->getText()); i++)
            {
               if (np->getText()->getText()[i] == '\'')
                  fprintf(wfp,"\\");
               fprintf(wfp,"%c",np->getText()->getText()[i]);
            }
            fprintf(wfp,"\'\n");

            // write_attributes(wfp, np->getAttributesRef(), "ARTWORK", &artworkattcnt);
         }  
         break;
         
         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
                  {
                     wpfw_Graph_Level(lname);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle());
                  }
*/
               } // end if layer visible
            }
            else // not aperture
            if (block->getBlockType() == BLOCKTYPE_PADSTACK) // from a mechanical pad or via ???
            {
               // this is done in mechanicalpin function
            }
            else
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               write_iplshape(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end write_iplashapes */
}

/*****************************************************************************/
/*
   mechanical pins are not in the netlist
*/
static void write_iplmechanicalpins(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   artworkattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {

         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...
            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {
               write_ipl_freepad(wfp, block, point2.x, point2.y);
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               write_componentmechanicalpin(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, -1, np->getInsert()->getRefname());
            }
         } // case INSERT
         break;
      } // end switch
   } // end write_iplmechanicalpins */
}

/*****************************************************************************/
/*
   mechanical pins are not in the netlist
*/
static void write_componentmechanicalpin(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer,
      CString  refname)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   layer, ddelayer;
   int   artworkattcnt = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         //if ((np->getFlags() & GR_CLASS_BOARD_GEOM) == GR_CLASS_BOARD_GEOM) continue;
         //if ((np->getFlags() & GR_CLASS_ETCH)     == GR_CLASS_ETCH)         continue;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if ((layer = Layer_DDE(layer)) < 0) continue;
         ddelayer = ddeArr[layer]->stackNum;
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {

         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {

         }  
         break;
         
         case T_INSERT:
         {
            // only allow here generic stuff (not comps etc..) or mechanical or generic.
            // no vias, testpoints etc...

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)     
            {
               CString  pinname;
               pinname = np->getInsert()->getRefname();

               if (strlen(pinname) == 0)  pinname = "xx";

               fprintf(wfp,".wlg 0 n %s-%s\n", refname, pinname);
               write_wlp(wfp, refname, pinname, point2.x, point2.y,
                     block_rot, block->getBlockNumber(), block_mirror);
            }
            else
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               write_componentmechanicalpin(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, refname);
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end write_iplmechanicalpins */
}

//--------------------------------------------------------------
void IPL_GetROUTESData(FILE *wfp, CDataList *DataList)
{
   DataStruct *np;
   Attrib   *a;

   POSITION pos1;
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      pos1 = pos; // pos is changed in getnext
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != GR_CLASS_ETCH)      continue;

         // here check if ATTR_NETNAME == netname
         a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

         if (!a)  continue;
      }
      else
      {
         // here check if ATTR_NETNAME == netname

         // allow only vias and test points
         a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);
         if (!a)  continue;
      }

      // here store it
      IPLRoutes *r = new IPLRoutes;
      routesarray.SetAtGrow(routescnt,r);  
      routescnt++;   
      r->netname = get_attvalue_string(doc, a);
      r->pos = pos1;
   } // end IPL_GetROUTESData */
}

//--------------------------------------------------------------
void IPL_WriteROUTESData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct *np;
   int         layer, ddelayer;

   int   routeattcnt = 0, viaattcnt = 0;

   RotMat2(&m, rotation);

   for (int i=0;i<routescnt;i++)
   {
      // different netname
      if (strcmp(routesarray[i]->netname, netname))   continue; 
      np = DataList->GetNext(routesarray[i]->pos);


      if (np->getDataType() != T_INSERT)
      {
         if (np->getGraphicClass() != GR_CLASS_ETCH)      continue;

         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if ((layer = Layer_DDE(layer)) < 0) continue;
         ddelayer = ddeArr[layer]->stackNum;
      }

      // here check if ATTR_NETNAME == netname
      Attrib *a =  is_attvalue(doc, np->getAttributesRef(), ATT_NETNAME, 1);

      if (a)
      {
         if (strcmp(get_attvalue_string(doc, a),netname))
            continue;
      }  
      else
         continue;

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            BOOL closed = FALSE;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isClosed()) closed = TRUE;
            }

            if (closed) break;

            // open polyline, trace, arc
            polyPos = np->getPolyList()->GetHeadPosition();
            
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

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
                     write_trace_arc(wfp, poly->getWidthIndex(), ddelayer, p1.x, p1.y, p1.bulge, p2.x, p2.y);
                     p1 = p2;
                  }
               }
            }  // while
         }
         break;  // POLYSTRUCT

         case T_TEXT:
            // no text in route section
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)      break;

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

            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA ||
                np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {

               // .bur 63,63,63,63 297047 184719 0,1,50,51 0 46 255
               write_ipl_bur(wfp, block, point2.x, point2.y);

               break;
            }

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
               fprintf(flog,"APERTURE Reference [%s] found in Net [%s] -> not supported in DDE\n", block->getName(), netname);
               display_error++;
               break;
            }
            else // not aperture
            {
               fprintf(flog,"Hierachical Reference [%s] found in Net [%s] -> not supported in DDE\n",  block->getName(), netname);
               display_error++;

/* can not process hierachical process, because IPL_WriteROUTESData loops through the net array
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               IPL_WriteROUTESData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, netname);
*/
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end IPL_WriteROUTESData */
}

//--------------------------------------------------------------
static const char *get_blockpadname(CDataList *b)
{
   DataStruct *np;
   int   fidcnt = 0;

   POSITION pos = b->GetHeadPosition();
   while (pos != NULL)
   {
      np = b->GetNext(pos);
      
      if (np->getDataType() == T_INSERT)        
      {

         if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            return block->getName();
         }
      }
   }
   return NULL;
}

/*****************************************************************************/
/*
*/
static int write_trace_arc(FILE *wfp, int trackcode, int layernum, 
                           double x1, double y1, double bulge, double x2, double y2)
{
   double da = atan(bulge) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx, cy, r, sa, ea;

      // arc
      ArcPoint2Angle(x1, y1, x2, y2, da, &cx, &cy, &r, &sa);

      //sa = startangle
      //ea = endangle
      sa = RadToDeg(sa);
      da = RadToDeg(da);
      ea = sa + da;

      fprintf(wfp,".arc %ld %ld %d %ld %d %d %d\n",
            cnv_units(cx), cnv_units(cy), layernum, cnv_units(r), 
            round(sa), round(ea), trackcode);
   }
   else
   {
      // tra
      fprintf(wfp,".tra %d %ld %ld %ld %ld %d\n",
         trackcode, cnv_units(x1), cnv_units(y1), cnv_units(x2),  cnv_units(y2), layernum);
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int write_coo_cen(FILE *wfp, double x1, double y1, double bulge, double x2, double y2)
{
   double da = atan(bulge) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      double cx,cy,r,sa;
      // arc
      int dir = 0; // 0 is cc 1 is clock
      ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);

      if (da < 0) dir = 1;

      fprintf(wfp,".coo %ld %ld\n", cnv_units(x1), cnv_units(y1));
      fprintf(wfp,".cen %ld %ld %d\n", cnv_units(cx), cnv_units(cy), dir);
   }
   else
   {
      // tra
      fprintf(wfp,".coo %ld %ld\n", cnv_units(x1), cnv_units(y1));
   }
   return 1;
}

/****************************************************************************/
/*
   end DDE_OUT.CPP
*/
/****************************************************************************/

