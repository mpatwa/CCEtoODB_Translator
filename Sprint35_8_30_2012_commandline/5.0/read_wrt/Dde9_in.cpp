// $Header: /CAMCAD/5.0/read_wrt/Dde9_in.cpp 55    6/17/07 8:55p Kurt Van Ness $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.

   according to DDE mentor manual

   Article == partnumber == ATT_TYPEDEVICE
   type    == geom       == 
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
#include "net_util.h"
#include "dde9_in.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
//extern LayerTypeInfoStruct layertypes[];
//extern void ExplodeInsert(CCEtoODBDoc *doc, DataStruct *data, CDataList *dataList); // from explode.cpp

/* Prototype Section ****************************************************/
void Initialize_All_Arrays();
void Free_All_Arrays();
void RenamePadstacksToOriginalName();
int load_iplsettings(const char *fileName);
void Start_DDE_Translation();
int go_ipl(List *tok_lst, int lst_size);
int tok_search(List *tok_lst,int tok_size);
void Add_Polygon_Data_To_PolyList(CPolyList *polyList);
void Graph_Polygon(CString polygonName, CPolyList *polyList);
void Graph_Area();
void Add_Footprint_Text(int layerRefnum, CString text, DbFlag textFlag, double xLoc, double yLoc, double height,
              double rotation, int mirror, BOOL visible, int widthIndex, BOOL isAttribute);
void Add_Component_Text(int layerRefnum, CString text, DbFlag textFlag, double xLoc, double yLoc, double height,
              double rotation, int mirror, BOOL visible, int widthIndex, BOOL isAttribute);

// These little utility functions are at the end of the file
double To_IPL_Unit(double x);
double Convert_Rotation(CString rotString);
void Get_Text_Option(CString textOption, DbFlag &flag, double &rotation, BOOL &visible);
CString Remove_Space_Quote(CString name);
DDELayerList *CreateDDELayer(int layerRefnum, CString layerFlags, CString layerName);
DDEMaterial *Find_Material_By_Name(CString name);
DDELayerList *Find_Layer_By_Refnum(int refnum);
DDEPolygon *Find_Polygon_By_Refnum(int refnum);
DDEPadstackList *Find_Padstack_By_Refnum(int refnum);
DDEFootprint *Find_Footprint_By_Refnum(int refnum);
DDEComponent *Find_Component_By_Refnum(int refnum);
DataStruct *Find_Footprint_PolyStruct(DDEFootprint *footprint, int layer);
CString Find_AttribMap(CString attrib);
double Get_Height(const char *InValue);
CString Get_Attribute_Value(Attrib* attrib);
static int Get_LayerType_By_CCLayerName(const char *layerName);
static int Get_LayerType_By_DDELayerName(const char *layerName);

/* Globals ****************************************************************/
static CCEtoODBDoc *sDoc;
static FileStruct *sFileStruct = NULL;

static FILE       *sInputFile;
static BOOL       sInputFileEnd = FALSE;
static BOOL       sIsDesignFile = FALSE;
static char       sInputLine[MAX_LINE];
static long       sInputLineCnt = 0;         // Current line number
static char       sToken[MAX_LINE];

static FILE       *sErrorFile;
static int        sDisplayErrorCnt = 0;

static BOOL       sIsBottomLayer = FALSE;
static int        sElectricalStack = 0;
static int        sCurrentState = STATE_UNKNOWN;  // the state of what section in the file
static int        sSubState = STATE_UNKNOWN;      // the state within the sCurrentState
static int        sPageUnits;
static DDEArea    sCurrentArea;
static NetStruct *sCurrentNet = NULL;
static DDE_Setting inFileSetting;

// Array definition
static CAttribMapArray     sAttribMapArray;
static int                 sAttribMapCnt;

static CDDEPolygonArray    sPolygonArray;
static int                 sPolygonCnt;

static CDDEGraphArray      sGraphArray;
static int                 sGraphCnt;

static CDDEMaterialArray   sMaterialArray;
static int                 sMaterialCnt;

static CDDELayerArray      sLayerArray;
static int                 sLayerCnt;

static CDDEPadstackArray   sPadstackArray;
static int                 sPadstackCnt;

static CDDEFootprintArray  sFootprintArray;
static int                 sFootprintCnt;

static CDDEComponentArray  sComponentArray;
static int                 sComponentCnt; 

static CDDELayerAttrArray  sLayerAttrArray;
static int                 sLayerAttrCnt;

/******************************************************************************
* Is_DDE_V9
*/
BOOL Is_DDE_V9(const char *path_buffer)
{
   sInputFile = fopen(path_buffer, "r");
   if (sInputFile == NULL)
   {
      CString msg;
      msg.Format("Error open [%s] file", path_buffer);
      ErrorMessage(msg, "Error");
      return TRUE;
   }

   char *lp;
   BOOL res = FALSE;
   while(fgets(sInputLine, MAX_LINE, sInputFile))
   {
      if ((lp = strtok(sInputLine, " \t\n")) == NULL)
         continue;

      CString tmpString = lp;
      if (!tmpString.CompareNoCase(".pcb"))
         res = TRUE;
      break;
   }
   fclose(sInputFile);
   return res;
}

/******************************************************************************
* ReadDDE_V9
*/
void ReadDDE_V9(const char *path_buffer, CCEtoODBDoc *Doc, FormatStruct *Format, int PageUnits)
{
   sDoc = Doc;
   sPageUnits = PageUnits;
   sFileStruct = NULL;
   sIsBottomLayer = FALSE;
   sCurrentState = STATE_UNKNOWN;
   sSubState = STATE_UNKNOWN;
   sCurrentNet = NULL;
   sElectricalStack = 0;

   sInputLineCnt = 0;
   sInputFileEnd = FALSE;
   sInputFile = fopen(path_buffer, "r");
   if (sInputFile == NULL)
   {
      CString msg;
      msg.Format("Error open [%s] file", path_buffer);
      ErrorMessage(msg, "Error");
      return;
   }

   CString logFile = GetLogfilePath("dde.log");
   sDisplayErrorCnt = 0;
   sErrorFile = fopen(logFile, "wt");
   if (sErrorFile == NULL)
   {
      CString message = "Error open " + logFile + " file";
      ErrorMessage(message, "Error");
      return;
   }

   Initialize_All_Arrays();

   CString settingsFile( getApp().getImportSettingsFilePath("dde.in") );
   CString settingsFileMsg;
   settingsFileMsg.Format("\nDDE9 Reader: Settings file [%s].\n", settingsFile);
   getApp().LogMessage(settingsFileMsg);
   load_iplsettings(settingsFile);

   sFileStruct = Graph_File_Start(path_buffer, Type_DDE_Layout);
   sFileStruct->setBlockType(blockTypePcb);
   sFileStruct->getBlock()->setBlockType(sFileStruct->getBlockType());
   Graph_Level("0", "", 1);

   Start_DDE_Translation();
	RenamePadstacksToOriginalName();	

   Free_All_Arrays();
   fclose(sErrorFile); 
   fclose(sInputFile);

   if (sDisplayErrorCnt > 0)
      Logreader(logFile);

   return;
}

/******************************************************************************
* Initialize_All_Arrays
*/
void Initialize_All_Arrays()
{
   sPolygonArray.SetSize(100, 100);
   sPolygonCnt = 0;

   sGraphArray.SetSize(100, 100);
   sGraphCnt = 0;

   sMaterialArray.SetSize(100, 100);
   sMaterialCnt = 0;

   sLayerArray.SetSize(100, 100);
   sLayerCnt = 0;

   sPadstackArray.SetSize(100, 100);
   sPadstackCnt = 0;

   sFootprintArray.SetSize(100, 100);
   sFootprintCnt = 0;

   sComponentArray.SetSize(100, 100);
   sComponentCnt = 0;

   sLayerAttrArray.SetSize(100, 100);
   sLayerAttrCnt = 0;
}

/******************************************************************************
* Free_All_Arrays
*/
void Free_All_Arrays()
{
   int i;

   for (i=0; i<sPolygonCnt; i++)
      delete sPolygonArray[i];
   sPolygonArray.RemoveAll();
   sPolygonCnt = 0;

   sGraphArray.RemoveAll();

   for (i=0; i<sMaterialCnt; i++)
   {
      DDEMaterial *material = sMaterialArray[i];
      for (int j=0; j<material->propertyCnt; j++)
         delete material->property[j];
      material->property.RemoveAll();
      delete material;
   }
   sMaterialArray.RemoveAll();
   sMaterialCnt = 0;

   for (i=0; i<sLayerCnt; i++)
      delete sLayerArray[i];
   sLayerArray.RemoveAll();
   sLayerCnt = 0;

   for (i=0; i<sPadstackCnt; i++)
      delete sPadstackArray[i];
   sPadstackArray.RemoveAll();
   sPadstackCnt = 0;

   for (i=0; i<sFootprintCnt; i++)
   {
      sFootprintArray[i]->polyDataList->RemoveAll();
      delete sFootprintArray[i];
   }
   sFootprintArray.RemoveAll();
   sFootprintCnt = 0;

   for (i=0; i<sComponentCnt; i++)
      delete sComponentArray[i];
   sComponentArray.RemoveAll();
   sComponentCnt = 0;

   for (i=0; i<sLayerAttrCnt; i++)
      delete sLayerAttrArray[i];
   sLayerAttrArray.RemoveAll();
   sLayerAttrCnt = 0;
}

/******************************************************************************
* RenamePadstacks
*/
void RenamePadstacksToOriginalName()
{
	for (int i=0; i<sDoc->getMaxBlockIndex(); i++)
	{
		BlockStruct *block = sDoc->getBlockAt(i);
		if (!block || block->getBlockType() != BLOCKTYPE_PADSTACK)
			continue;
		if (block->getOriginalNameRef().Trim() == "")
			continue;

		block->setName(block->getOriginalName());
	}
}

/******************************************************************************
* load_iplsettings
*/
int load_iplsettings(const char *fileName)
{
   char  line[255];
   char  *lp;

   inFileSetting.HatchLineFill = FALSE;

   FILE *filePrt = fopen(fileName, "rt");
   if (filePrt == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", fileName);
      ErrorMessage(tmp, "DDE Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line, 255, filePrt))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".ATTRIBMAP"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString attrib = lp;
            attrib.MakeUpper();
            attrib.TrimLeft();
            attrib.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            CString mapAttrib = lp;
            mapAttrib.MakeUpper();
            mapAttrib.TrimLeft();
            mapAttrib.TrimRight();

            DDEAttribMap *attribMap = new DDEAttribMap;
            sAttribMapArray.SetAtGrow(sAttribMapCnt++, attribMap);
            attribMap->attrib = attrib;
            attribMap->mapAttrib = mapAttrib;
            
         }
         else if (!STRICMP(lp, ".LAYERATTR"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString ddelayer = _strupr(lp);
            ddelayer.TrimLeft();
            ddelayer.TrimRight();

            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue; 
            CString cclayer = lp;
            cclayer.TrimLeft();
            cclayer.TrimRight();

            int layerType = Get_LayerType_By_CCLayerName(cclayer);
            DDELayerAttr *layerAttr = new DDELayerAttr;
            sLayerAttrArray.SetAtGrow(sLayerAttrCnt++, layerAttr);
            layerAttr->layerType = layerType;
            layerAttr->name = ddelayer;
         }
         else if (!STRICMP(lp, ".HATCHLINEFILL"))
         {
            if ((lp = get_string(NULL, " \t\n")) == NULL)
               continue;
            inFileSetting.HatchLineFill = atoi(lp);
         }
      }
   }

   fclose(filePrt);
   return 1;
}

/******************************************************************************
* StartDDETranslation
*/
void Start_DDE_Translation()
{
   int res = go_ipl(pcb_lst, SIZ_PCB_LST);
   if (res != NOT_PCB_DESIGN && res != NO_PROPERT_THERMAL)
   {
      update_smdpads(sDoc);
      RefreshInheritedAttributes(sDoc, SA_OVERWRITE); // smd pins are after the pin instance is done.

      for (int i=0; i<sComponentCnt; i++)
      {
         DDEComponent *comp = sComponentArray[i];
         if (comp->boardOutline)
         {
            //ExplodeInsert(sDoc, comp->data, &(sFileStruct->getBlock()->getDataList()));   // this also deletes the insert entity
         }
      }

      int err = check_primary_boardoutline(sFileStruct);
      switch (err)
      {        
      case 1:
         fprintf(sErrorFile, "Primary Boardoutline found, but not closed!\n");
         sDisplayErrorCnt++;
         break;
      case 2:
         fprintf(sErrorFile, "No Primary Boardoutline found!\n");
         sDisplayErrorCnt++;
         break;
      case 3:
         fprintf(sErrorFile, "One Primary closed Boardoutline and multiple open elements found!\n");
         fprintf(sErrorFile, "The open elements will be set to BOARD GEOMETRIES!\n");
         sDisplayErrorCnt++;
         break;
      case 4:
         fprintf(sErrorFile, "Multiple Primary closed Boardoutline and multiple open elements found!\n");
         fprintf(sErrorFile, "All elements will be set to BOARD GEOMETRIES!\n");
         sDisplayErrorCnt++;
         break;
      }

      EliminateSinglePointPolys(sDoc); 
   }       
}

/******************************************************************************
* go_ipl
*   Call function associated with next token.
*/
int go_ipl(List *tok_lst, int lst_size)
{
   while (fgets(sInputLine, MAX_LINE, sInputFile) != NULL && !sInputFileEnd)
   {
      sInputLineCnt++;
      if (strlen(sInputLine) && sInputLine[strlen(sInputLine)-1] == '\n')
         sInputLine[strlen(sInputLine)-1] = '\0';

      CString tmpInputLine = sInputLine;
      char *lp;

      if ((lp = strtok(sInputLine, " \t\n")) == NULL)
         continue;      
      if (lp[0] != '.') 
         continue;

      CpyStr(sToken, lp, MAX_LINE);
      int tokIndex = tok_search(tok_lst, lst_size);
      if (tokIndex >= 0)
      {
         int res = (*tok_lst[tokIndex].function)();
         if (res == NOT_PCB_DESIGN || res == NO_PROPERT_THERMAL)
            return res;
      }
      else
      {
         fprintf(sErrorFile, "Unknown line at %ld ->%s\n", sInputLineCnt, tmpInputLine);
      }
   } 

   return 1;
}

/******************************************************************************
* tok_search
*   Search for the token in a command token list.
*/
int tok_search(List *tok_lst, int tok_size)
{
   for ( int i=0; i<tok_size; ++i)
   {
      if (!STRNICMP(sToken, tok_lst[i].token, strlen(tok_lst[i].token)))
         return(i);
   }
   return(-1);
}

/******************************************************************************
* ipl_skip
*/
static int ipl_skip()
{
   // Skip this command because it is not needed
   return 1;
}

/******************************************************************************
* ipl_pcb

   HEADER ::= ".pcb" fileType
   fileType = string
*/
static int ipl_pcb()
{
   sCurrentState = STATE_UNKNOWN;
   char *lp;
   
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString fileType = lp;

   if (!fileType.CompareNoCase("design"))
   {
      sIsDesignFile = TRUE;
   }
   else if(!sIsDesignFile)  // current support file of type Design
   {
      ErrorMessage("This file is not of type PCB DESIGN.  DDE Version 9 currently only support design file.", "Error", MB_OK);
      return NOT_PCB_DESIGN;
   }
   return 1;
}

/******************************************************************************
* ipl_pro 

   PROPERTY ::= ".pro" name value
   name  = string
   value = string
*/
static int ipl_pro()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString key = Find_AttribMap(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString value = Remove_Space_Quote(lp);
   if (!strlen(value))
      return 0;

   if (!key.CompareNoCase("SAVESTACKINSTS"))
   {
      if (value.CompareNoCase("Y"))
      {
         ErrorMessage("Design file must be saved with design property \"SAVESTACKINSTS = Y\"", "Error");
         return NO_PROPERT_THERMAL;
      }
   }

   int state = sCurrentState;
   if (sSubState)
      state = sSubState;

   switch (state)
   {
   case STATE_MATERIAL:       
      {
         DDEMaterial *material = sMaterialArray[sMaterialCnt-1];
         DDEProperty *property = new DDEProperty;
         material->property.SetAtGrow(material->propertyCnt++, property);
         property->name = key;
         property->value = value;
      }
      break;
   case STATE_LAYER:
      {
         DDELayerList *ddeLayer = sLayerArray[sLayerCnt-1];
         LayerStruct *layer = sDoc->FindLayer(ddeLayer->layerNum);
         if (layer == NULL)
            break;

         int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
         sDoc->SetAttrib(&layer->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);             
         
         if (!key.CompareNoCase("Material"))
         {
            DDEMaterial *material = Find_Material_By_Name(value);
            if (material == NULL)
               break;

            if (material->propertyCnt < 1)
               break;

            for (int i=0; i<material->propertyCnt; i++)
            {
               DDEProperty *property = material->property[i];
               keyword = sDoc->RegisterKeyWord(property->name, 0, VT_STRING);
               sDoc->SetAttrib(&layer->getAttributesRef(), keyword, VT_STRING, property->value.GetBuffer(0), SA_OVERWRITE, NULL);               
            }
         }
      }
      break;
   case STATE_PADSTACK:       
      {
         if (!key.CompareNoCase("Clearance"))
            break;  // currently do not support this property because it is a very long string

         DDEPadstackList *padstack = sPadstackArray[sPadstackCnt-1];
         BlockStruct *block = sDoc->Find_Block_by_Num(padstack->geomNum);
         if (block)
         {
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
            sDoc->SetAttrib(&block->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);             
         }
      }
      break;
   case STATE_TYPE:           
      {
         DDEFootprint *footprint = sFootprintArray[sFootprintCnt-1];
         BlockStruct *block = sDoc->Find_Block_by_Num(footprint->geomNum);
         if (block == NULL)
            break;

         if (!key.CompareNoCase("DEVICE"))
			{
            key = ATT_DEVICETYPE;
	
				CString typeName = value;
				TypeStruct* type = AddType(sFileStruct, typeName);
				if (type->getBlockNumber() == -1)
				{
					type->setBlockNumber(block->getBlockNumber());
				}
				else if (type->getBlockNumber() != block->getBlockNumber())
				{
					typeName.AppendFormat("_%s", block->getName());
					type = AddType(sFileStruct, typeName);
					type->setBlockNumber(block->getBlockNumber());
				}
			}
         else if (!key.CompareNoCase("NAME"))
			{
            key = ATT_REFNAME;
			}

         if (!key.CompareNoCase(ATT_COMPHEIGHT) || !key.CompareNoCase("pcb_thickness"))
         {
            double height = Get_Height(value);
            if (height > 0)
            {
               int keyword = sDoc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);
               sDoc->SetAttrib(&block->getAttributesRef(), keyword, VT_UNIT_DOUBLE, &height, SA_OVERWRITE, NULL);             
            }
         }
         else if (!key.CompareNoCase(ATT_DEVICETYPE) || !key.CompareNoCase(ATT_REFNAME) || !key.CompareNoCase("ARTICLE"))
         {
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
            sDoc->SetAttrib(&block->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);             
         }
         else if (!key.CompareNoCase(ATT_TOLERANCE))
         {
            int keyword = sDoc->RegisterKeyWord(ATT_TOLERANCE, 0, VT_DOUBLE);
            sDoc->SetAttrib(&block->getAttributesRef(), keyword, VT_DOUBLE, &value, SA_OVERWRITE, NULL);             
         }
         else
         {
            sDoc->SetUnknownAttrib(&block->getAttributesRef(), key, value.GetBuffer(0), SA_OVERWRITE, NULL);
         }        
      }
      break;
   case STATE_AREA:        
      {
         // skip for now
      }
      break;
   case STATE_COMPONENT:         
      {
         DDEComponent *comp = sComponentArray[sComponentCnt-1];
         if (comp->data == NULL || comp->data->getInsert() == NULL)
            break;

         if (!key.CompareNoCase("DEVICE"))
			{
            key = ATT_DEVICETYPE;

				CString typeName = value;
				TypeStruct* type = AddType(sFileStruct, typeName);
				BlockStruct* block = sDoc->getBlockAt(comp->data->getInsert()->getBlockNumber());
				if (type->getBlockNumber() == -1)
				{
					type->setBlockNumber(block->getBlockNumber());
				}
				else if (type->getBlockNumber() != block->getBlockNumber())
				{
					typeName.AppendFormat("_%s", block->getName());
					type = AddType(sFileStruct, typeName);
					type->setBlockNumber(block->getBlockNumber());
				}

            int keyword = sDoc->RegisterKeyWord(ATT_TYPELISTLINK, 0, VT_STRING);
            sDoc->SetAttrib(&comp->data->getAttributesRef(), keyword, VT_STRING, typeName.GetBuffer(0), SA_OVERWRITE, NULL);              
			}
         else if (!key.CompareNoCase("NAME"))
			{
            key = ATT_REFNAME;
			}

         if (!key.CompareNoCase(ATT_COMPHEIGHT) || !key.CompareNoCase("pcb_thickness"))
         {
            double height = Get_Height(value);
            if (height > 0)
            {
               int keyword = sDoc->RegisterKeyWord(ATT_COMPHEIGHT, 0, VT_UNIT_DOUBLE);
               sDoc->SetAttrib(&comp->data->getAttributesRef(), keyword, VT_UNIT_DOUBLE, &height, SA_OVERWRITE, NULL);              
            }
         }
         else if (!key.CompareNoCase(ATT_DEVICETYPE) || !key.CompareNoCase(ATT_REFNAME) || !key.CompareNoCase("ARTICLE"))
         {
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
            sDoc->SetAttrib(&comp->data->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);              
         }
         else if (!key.CompareNoCase(ATT_TOLERANCE))
         {
            int keyword = sDoc->RegisterKeyWord(ATT_TOLERANCE, 0, VT_DOUBLE);
            sDoc->SetAttrib(&comp->data->getAttributesRef(), keyword, VT_DOUBLE, &value, SA_OVERWRITE, NULL);              
         }
         else
         {
            sDoc->SetUnknownAttrib(&comp->data->getAttributesRef(), key, value.GetBuffer(0), SA_OVERWRITE, NULL);
         }        
      }
      break;
   case STATE_NETLIST:        
      {
         if (!key.CompareNoCase("Clearance"))
            break;  // currently do not support this property because it is a very long string

         if (sCurrentNet)
         {
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
            sDoc->SetAttrib(&sCurrentNet->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);             
         }
      }
      break;
   default:
      break;
   }     

   return 1;
}
         
/******************************************************************************
* ipl_remark

   REMARK ::= ".rem" remark
*/
static int ipl_rem()                     
{
   sCurrentState = STATE_UNKNOWN;
   while (fgets(sInputLine, MAX_LINE, sInputFile))
   {
      char *lp;
      sInputLineCnt++;
      
      if (strlen(sInputLine) && sInputLine[strlen(sInputLine)-1] == '\n')
         sInputLine[strlen(sInputLine)-1] = '\0';  // kill lineend.
      if ((lp = strtok(sInputLine, " \t\n")) == NULL)  // no empty line
         continue;
      if (lp[0] != '.')  // every command must start with a .
         continue;
      if (!STRCMPI(lp, ".end"))
         break;
   }

   return 1;
}

/******************************************************************************
* ipl_rul

   RULESET ::= ".rul" rulesetName
   rulsetName = string
*/
static int ipl_rul()
{
   return 1;
}

/******************************************************************************
* ipl_pol

   POLYGON ::= ".pol" polygonRefnum polygonName [polygonFileName]
   polygonRefnum     = integer
   polygonName       = string
   polygonFileName   = string
*/
static int ipl_pol()
{
   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int polygonRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString polygonName = Remove_Space_Quote(lp);

   DDEPolygon *polygon = new DDEPolygon;
   sPolygonArray.SetAtGrow(sPolygonCnt++, polygon);
   polygon->index = polygonRefnum;
   polygon->name = polygonName;

   sCurrentState = STATE_POLYGON;
   return 1;
}

/******************************************************************************
* ipl_cir

  CIRCLE ::= ".cir" radius
  radius = integer
*/
static int ipl_cir()   
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double radius = To_IPL_Unit(atof(lp));

   DDEGraphList graph;
   graph.borderPart = FALSE;
   graph.clockWise = FALSE;
   graph.radius = radius;
   graph.type = GRAPH_CIR;
   graph.x = 0;
   graph.y = 0;
   sGraphArray.SetAtGrow(sGraphCnt++, graph);
   
   return 1;
}
      
/******************************************************************************
* ipl_coo

   COORDINATE ::= ".coo" xloc yLoc
   xLoc = integer
   yLoc = integer
*/
static int ipl_coo()         
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double xLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double yLoc = To_IPL_Unit(atof(lp));

   DDEGraphList graph;
   graph.borderPart = FALSE;
   graph.clockWise = FALSE;
   graph.radius = 0;
   graph.type = GRAPH_COO;
   graph.x = xLoc;
   graph.y = yLoc;
   sGraphArray.SetAtGrow(sGraphCnt++, graph);

   return 1;
}

/******************************************************************************
* ipl_cen

   CENTER ::= ".cen" xloc yLoc rotation
   xLoc     = integer
   yLoc     = integer
   rotation = integer
*/
static int ipl_cen()         
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double xLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double yLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int rotation = atoi(lp);

   DDEGraphList graph;
   graph.borderPart = FALSE;
   graph.clockWise = rotation;   // 0 = counter clockwise, 1 = clockwise
   graph.radius = 0;
   graph.type = GRAPH_CEN;
   graph.x = xLoc;
   graph.y = yLoc;
   sGraphArray.SetAtGrow(sGraphCnt++, graph);

   return 1;
}

/******************************************************************************
* ipl_poe

   SECTION END ::= ".poe" value
   value = "borderpart" | "border" | "end"
*/
static int ipl_poe()         
{
   char *lp;

   if (sGraphCnt == 0)
      return 0;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString value = Remove_Space_Quote(lp);

   DDEPolygon *tmpPolygon = sPolygonArray[sPolygonCnt-1];


   // Find the index of the last border part graph
   int start = 0;
   for (int i=0; i<sGraphCnt; i++)
   {
      DDEGraphList tmpGraph = sGraphArray[i];
      if (tmpGraph.borderPart)
         start = i + 1;
   }

   // If the current graph and the start graph is not the same, then add the start graph to the array to close the polygon
   DDEGraphList curGraph = sGraphArray[sGraphCnt - 1];
   if (curGraph.type != GRAPH_CIR)
   {
      DDEGraphList startGraph = sGraphArray[start];
      if ((fabs(startGraph.x - curGraph.x) > SMALLNUMBER || fabs(startGraph.y - curGraph.y) > SMALLNUMBER)
         && startGraph.type != GRAPH_CEN)

      {
         startGraph.type = GRAPH_COO;
         sGraphArray.SetAtGrow(sGraphCnt++, startGraph);
      }
   }

   if (!value.CompareNoCase("borderpart"))  // Indicate end of borderpart, but continue reading
   {
      sGraphArray[sGraphCnt - 1].borderPart = TRUE;
   }
   else if (!value.CompareNoCase("end") && sCurrentState == STATE_POLYGON)  // End of Polygon state
   {
      CPolyList polyList;
      Add_Polygon_Data_To_PolyList(&polyList);
      Graph_Polygon(sPolygonArray[sPolygonCnt-1]->name, &polyList);
      sGraphCnt = 0;
      sCurrentState = STATE_UNKNOWN;
   }
   else if (!value.CompareNoCase("border") && (sCurrentState == STATE_AREA || sSubState == STATE_AREA))  // end of a border of an area
   {
      Graph_Area();
      sGraphCnt = 0;
   }

   return 1;
}

/******************************************************************************
* Add_PolygonData_To_Poly
*/
void Add_Polygon_Data_To_PolyList(CPolyList *polyList)
{
   BOOL newPoly = TRUE;
   CPnt *lastVertex = NULL;
   CPoly *poly = NULL;

   for (int i=0; i<sGraphCnt; i++)
   {
      if (newPoly)
      {
         poly = new CPoly;
         polyList->AddTail(poly);
         poly->setClosed(true);
         poly->setVoid(false);
      }

      DDEGraphList curGraph = sGraphArray[i];
      switch (curGraph.type)
      {
      case GRAPH_CIR:
         {
            // Circle with radius = curGraph.x
            CPnt *pnt = new CPnt;
            pnt->bulge = 1.0;
            pnt->x = (DbUnit)(-curGraph.radius);
            pnt->y = (DbUnit)(curGraph.y);
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 1.0;
            pnt->x = (DbUnit)(curGraph.radius);
            pnt->y = (DbUnit)(curGraph.y);
            poly->getPntList().AddTail(pnt);

            pnt = new CPnt;
            pnt->bulge = 0.0;
            pnt->x = (DbUnit)(-curGraph.radius);
            pnt->y = (DbUnit)(curGraph.y);
            poly->getPntList().AddTail(pnt);
         }
         break;
      case GRAPH_COO:
         {
            CPnt *pnt = new CPnt;
            pnt->bulge = 0.0;
            pnt->x = (DbUnit)(curGraph.x);
            pnt->y = (DbUnit)(curGraph.y);
            poly->getPntList().AddTail(pnt);
            lastVertex = pnt;
         }
         break;
      case GRAPH_CEN:
         {
            if (lastVertex && !newPoly)
            {
               DDEGraphList preGraph = sGraphArray[i-1];
               DDEGraphList nextGraph = sGraphArray[i+1];
               if (fabs(preGraph.x - nextGraph.x) < SMALLNUMBER && fabs(preGraph.y - nextGraph.y) < SMALLNUMBER)
               {
                  double radius;
                  CircleCenterPt(curGraph.x, curGraph.y, nextGraph.x, nextGraph.y, &radius);

                  // Change the last point to first point in circle
                  lastVertex->bulge = 1.0;
                  lastVertex->x = (DbUnit)(curGraph.x + radius);
                  lastVertex->y = (DbUnit)(curGraph.y);

                  // Add second point to circle
                  CPnt *pnt = new CPnt;
                  pnt->bulge = 1.0;
                  pnt->x = (DbUnit)(curGraph.x - radius);
                  pnt->y = (DbUnit)(curGraph.y);
                  poly->getPntList().AddTail(pnt);

                  // Add third point to close circle
                  pnt = new CPnt;
                  pnt->bulge = 0;
                  pnt->x = (DbUnit)(curGraph.x + radius);
                  pnt->y = (DbUnit)(curGraph.y);
                  poly->getPntList().AddTail(pnt);

                  // Skip the next set of point in the array
                  i++;

                  poly->setVoid(curGraph.clockWise);
               }
               else
               {
                  double radius;
                  double startAngle;
                  double deltaAngle;

                  ArcCenter2(preGraph.x, preGraph.y, nextGraph.x, nextGraph.y, curGraph.x, curGraph.y, 
                        &radius, &startAngle, &deltaAngle, curGraph.clockWise);
                  lastVertex->bulge = (DbUnit)tan(deltaAngle / 4);
               }
            }
            else
            {
               DDEGraphList nextGraph = sGraphArray[i+1];
               double radius;
               CircleCenterPt(curGraph.x, curGraph.y, nextGraph.x, nextGraph.y, &radius);

               // Add first point to circle
               CPnt *pnt = new CPnt;
               pnt->bulge = 1.0;
               pnt->x = (DbUnit)(curGraph.x + radius);
               pnt->y = (DbUnit)(curGraph.y);
               poly->getPntList().AddTail(pnt);

               // Add second point to circle
               pnt = new CPnt;
               pnt->bulge = 1.0;
               pnt->x = (DbUnit)(curGraph.x - radius);
               pnt->y = (DbUnit)(curGraph.y);
               poly->getPntList().AddTail(pnt);

               // Add third point to close circle
               pnt = new CPnt;
               pnt->bulge = 0;
               pnt->x = (DbUnit)(curGraph.x + radius);
               pnt->y = (DbUnit)(curGraph.y);
               poly->getPntList().AddTail(pnt);

               // Skip the next set of point in the array
               i++;

               poly->setVoid(curGraph.clockWise);
            }
         }
         break;
      default:
         fprintf(sErrorFile, "Graphtype [%d] not implemented at %ld.\n", curGraph.type, sInputLineCnt);
         sDisplayErrorCnt++;
         break;
      }

      if (curGraph.borderPart)
         newPoly = TRUE;
      else
         newPoly = FALSE;
   }
}

/******************************************************************************
* Graph_Polygon
*/
void Graph_Polygon(CString polygonName, CPolyList *polyList)
{
   int err = 0;
   double radius = 0.0;
   double sizeA = 0.0;
   double sizeB = 0.0;
   double centerX = 0.0;
   double centerY = 0.0;
   double lowerLeftX = 0.0;
   double lowerLeftY = 0.0;
   double upperRightX = 0.0;
   double upperRightY = 0.0;

   if (polyList->GetCount() == 1)
   {
      CPoly *poly = polyList->removeHead();

      if (PolyIsCircle(poly, &centerX, &centerY, &radius))
      {
         Graph_Aperture(polygonName, T_ROUND, radius * 2, 0.0, centerX, centerY, 0.0, 0, BL_APERTURE, TRUE, &err);
      }
      else if (PolyIsRectangle(poly, &lowerLeftX, &lowerLeftY, &upperRightX, &upperRightY))
      {
         sizeA = upperRightX - lowerLeftX;
         sizeB = upperRightY - lowerLeftY;
         centerX = lowerLeftX + sizeA / 2;
         centerY = lowerLeftY + sizeB / 2;

         if (sizeA == sizeB)
            Graph_Aperture(polygonName, T_SQUARE, sizeA, 0.0, centerX, centerY, 0.0, 0, BL_APERTURE, TRUE, &err);
         else
            Graph_Aperture(polygonName, T_RECTANGLE, sizeA, sizeB, centerX, centerY, 0.0, 0, BL_APERTURE, TRUE, &err);
      }
      else if (PolyIsOblong(poly, &sizeA, &sizeB, &centerX, &centerY))
      {
         if (sizeA == sizeB)
            Graph_Aperture(polygonName, T_ROUND, sizeA, 0.0, centerX, centerY, 0.0, 0, BL_APERTURE, TRUE, &err);
         else if (sizeA > sizeB)
            Graph_Aperture(polygonName, T_OBLONG, sizeA, sizeB, centerX, centerY, 0.0, 0, BL_APERTURE, TRUE, &err);
         else
            Graph_Aperture(polygonName, T_OBLONG, sizeA, sizeB, centerX, centerY, DegToRad(90.0), 0, BL_APERTURE, TRUE, &err);
      }
      else
      {
         CString complexName = polygonName + "_COMPLEX";
         int layerIndex = Graph_Level("0", 0, 1);
         int widthIndex = Graph_Aperture("", T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

         BlockStruct *block = Graph_Block_On(GBO_APPEND, complexName, -1, 0);
         DataStruct *data = Graph_PolyStruct(layerIndex, 0L, FALSE);
         Graph_Poly(data, widthIndex, poly->isClosed(), poly->isVoid(), poly->isClosed());

         POSITION pntPos = poly->getPntList().GetHeadPosition();
         while (pntPos)
         {
            CPnt * pnt = poly->getPntList().GetNext(pntPos);
            Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
         }
         Graph_Block_Off();
         Graph_Complex(polygonName, 0, complexName, 0.0, 0.0, 0.0);
      }

      delete poly;
   }
   else
   {
      CString complexName = polygonName + "_COMPLEX";
      int layerIndex = Graph_Level("0", 0, 1);
      int widthIndex = Graph_Aperture("", T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

      BlockStruct *block = Graph_Block_On(GBO_APPEND, complexName, -1, 0);
      DataStruct *data = Graph_PolyStruct(layerIndex, 0L, FALSE);

      POSITION pos = polyList->GetHeadPosition();
      while(polyList->GetCount() > 0)
      {
         CPoly *poly = polyList->removeHead();

         if (poly == NULL)
            continue;

         Graph_Poly(data, widthIndex, poly->isClosed(), poly->isVoid(), poly->isClosed());

         POSITION pntPos = poly->getPntList().GetHeadPosition();
         while (pntPos)
         {
            CPnt * pnt = poly->getPntList().GetNext(pntPos);
            Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
         }

         delete poly;
      }

      Graph_Block_Off();
      Graph_Complex(polygonName, 0, complexName, 0.0, 0.0, 0.0);
   }
}

/******************************************************************************
* Graph_Area
*/
void Graph_Area()
{
   int err = 0;
   int widthIndex = Graph_Aperture("", T_ROUND, sCurrentArea.trackWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   double xOffset = sCurrentArea.xOffset;
   double yOffset = sCurrentArea.yOffset;
   BOOL filled = sCurrentArea.filled;
   BOOL newPoly = TRUE;
   CPnt *lastVertex = NULL;


   for (int i=0; i<sGraphCnt; i++)
   {
      if (newPoly)
      {
         if (inFileSetting.HatchLineFill)
         {
            CPoly *poly = Graph_Poly(sCurrentArea.polyData, widthIndex, FALSE, TRUE, TRUE);
            poly->setHidden(filled);
         }
         else
         {
            CPoly *poly = Graph_Poly(sCurrentArea.polyData, widthIndex, filled, FALSE, TRUE);
         }
      }

      DDEGraphList curGraph = sGraphArray[i];
      switch(curGraph.type)
      {
      case GRAPH_CIR:
         {
            Graph_Vertex(-curGraph.radius - xOffset, -yOffset, 1);
            Graph_Vertex(curGraph.radius - xOffset, -yOffset, 1);
            Graph_Vertex(-curGraph.radius - xOffset, -yOffset, 0);
         }
         break;
      case GRAPH_COO:
         {
            lastVertex = Graph_Vertex(curGraph.x - xOffset, curGraph.y - yOffset, 0);
         }
         break;
      case GRAPH_CEN:
         {
            DDEGraphList nextGraph = sGraphArray[i+1];
            double radius;
            double x2 = nextGraph.x - xOffset;
            double y2 = nextGraph.y - yOffset;
            double centerX = curGraph.x - xOffset;
            double centerY = curGraph.y - yOffset;
            if (lastVertex && !newPoly)
            {
               DDEGraphList preGraph = sGraphArray[i-1];
               if (fabs(preGraph.x - nextGraph.x) < SMALLNUMBER && fabs(preGraph.y - nextGraph.y) < SMALLNUMBER)
               {
                  CircleCenterPt(centerX, centerY, x2, y2, &radius);

                  // Change the last point to first point in circle
                  lastVertex->bulge = 1.0;
                  lastVertex->x = (DbUnit)(centerX + radius);
                  lastVertex->y = (DbUnit)(centerY);

                  // Add second point to circle
                  Graph_Vertex(centerX - radius, centerY, 1);
                  // Add third point to close circle
                  Graph_Vertex(centerX + radius, centerY, 0);

                  // Skip the next set of point in the array
                  i++;
               }
               else
               {
                  double startAngle;
                  double deltaAngle;
                  double x1 = preGraph.x - xOffset;
                  double y1 = preGraph.y - yOffset;

                  ArcCenter2(x1, y1, x2, y2, centerX, centerY, &radius, &startAngle, &deltaAngle, curGraph.clockWise);
                  lastVertex->bulge = (DbUnit)tan(deltaAngle / 4);
               }
            }
            else
            {
               CircleCenterPt(centerX, centerY, x2, y2, &radius);
      
               // Add first point to circle
               Graph_Vertex(centerX + radius, centerY, 1);
               // Add second point to circle
               Graph_Vertex(centerX - radius, centerY, 1);
               // Add third point to close circle
               Graph_Vertex(centerX + radius, centerY, 0);

               // Skip the next set of point in the array
               i++;
            }
         }
         break;
      default:
         {
            fprintf(sErrorFile, "Graphtype [%d] not implemented at %ld.\n", curGraph.type, sInputLineCnt);
            sDisplayErrorCnt++;
         }
         break;
      }

      if (curGraph.borderPart)
         newPoly = TRUE;
      else
         newPoly = FALSE;
   }
}

/******************************************************************************
* ipl_maf

   MATERIAL FILE ::= ".maf" materialFileName
   materialFileName = string
*/
static int ipl_maf()          
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString materialFileName = lp;

   return 1;
}
   
/******************************************************************************
* ipl_mat

   MATERIAL ::= ".mat" materialName protected dielectric resistance icapacity
   materialName   = string
   protected      = "n" | "p"
   dielectric     = float
   resistance     = float
   icapacity      = float
*/
static int ipl_mat()
{
   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString materialName = Remove_Space_Quote(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int protect = atoi(lp); 

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double dielectric = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double resistance = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double icapacity = atof(lp);

   DDEMaterial *material = new DDEMaterial;
   sMaterialArray.SetAtGrow(sMaterialCnt++, material);
   material->name =  materialName;
   material->protect =  protect;
   material->dielectric = dielectric;
   material->resistance = resistance;
   material->icapacity = icapacity;
   material->propertyCnt = 0;

   sCurrentState = STATE_MATERIAL;
   return 1;
}

/******************************************************************************
* ipl_mae

   MATERIAL END ::= ".mae"
*/
static int ipl_mae()         
{
   sCurrentState = STATE_UNKNOWN;
   return 1;
}

/******************************************************************************
* ipl_lay

   LAYER ::= ".lay" layerRefnum layerFlags layerName
   layerRefnum = integer

   layerFlags  = "n" | "d" | layerKind | layerKind + "d" | layerKind + "p" | layerKind + "dp"
   layerKind   = "e" | "eP" | "eI" | "eC" | "eS" | "ePC" | "ePS" | "H" | "G"

   layerName   = string
*/
static int ipl_lay()            
{
   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString layerFlags = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString layerName = Remove_Space_Quote(lp);
   
   CreateDDELayer(layerRefnum, layerFlags, layerName);

   sCurrentState = STATE_LAYER;
   return 1;
}

/******************************************************************************
* ipl_atr
   
   ATTRIBUTE ::= ".atr" color colorIndex redValue greenValue blueValue 
   color       = "color"
   colorIndex  = integer
   redValue    = integer (in percent)
   greenValue  = integer (in percent)
   blueValue   = integer (in percent)
*/
static int ipl_atr()
{
   char  *lp;

   if (sCurrentState != STATE_LAYER)
      return 0;  // currently only layer has attribute

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString color = lp;
   if (color.CompareNoCase("color"))   
      return 0;  // not a color attribute

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int colorIndex = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int redValue = round((atoi(lp) / 100) * 255);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int greenValue = round((atoi(lp) / 100) * 255);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int blueValue = round((atoi(lp) / 100) * 255);


   DDELayerList *ddeLayer = sLayerArray[sLayerCnt-1];
   LayerStruct *layer = sDoc->FindLayer(ddeLayer->layerNum);
   layer->setOriginalColor( RGB(redValue, greenValue, blueValue));

   return 1;
}

/******************************************************************************
* ipl_mir
         
   MIRROR MAPPING ::= ".mir" layerFromRefnum layerToRefnum
   layerFromRefnum   = integer
   layerToRefnum     = integer
*/
static int ipl_mir()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerFromRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerToRefnum = atoi(lp);

   DDELayerList *ddeLayerFrom = Find_Layer_By_Refnum(layerFromRefnum);
   DDELayerList *ddeLayerTo = Find_Layer_By_Refnum(layerToRefnum);
   ddeLayerFrom->mirrorIndex = layerToRefnum;
   ddeLayerTo->mirrorIndex = layerFromRefnum;

   CString layerFromName;
   layerFromName.Format("L%d", layerFromRefnum);
   CString layerToName;
   layerToName.Format("L%d", layerToRefnum);
   Graph_Level_Mirror(layerFromName, layerToName, "");

   return 1;
}

/******************************************************************************
* ipl_lac

   LAYER CLASS ::= ".lac" layerClassNum layerName
   layerClassNum = integer
   layerName     = string
*/
static int ipl_lac()
{
   sCurrentState = STATE_UNKNOWN;
   char *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString layerName = Remove_Space_Quote(lp);

   CreateDDELayer(layerRefnum, "", layerName);
   
   return 1;
}

/******************************************************************************
* ipl_pst

   PADSTATCK ::= ".pst" padstackRefnum drillSize plated/non filled/non stackKind padstackName
   padstackRefnum = integer
   drillSize      = integer
   plated         = "n" | "p"
   filled         = "border" | "filled" | "center" | "plus" | "x" | "box"

   stackKind      = stacktype + "/" + clearancetype
   stacktype      = "via" | "pad" | "smt" | "blind" | "buried" | "dot"
   clearancetype  = "via" | "pad" | "smt"

   padstackName   = string
*/
static int ipl_pst()
{
   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int padstackRefnum = atoi(lp);
   CString CCPadstackName = "PAD_";
   CCPadstackName += lp ;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double drillSize = To_IPL_Unit(atof(lp));
   CString drillName = "DRILL_";
   drillName += lp;

   BOOL plated = FALSE;
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   if (lp[0] == 'p' || lp[0] == 'P')
      plated = TRUE;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString padstackName = lp;

   // Create the padstack
   BlockStruct *block = Graph_Block_On(GBO_APPEND, CCPadstackName, -1, 0);
   block->setOriginalName(padstackName);
   block->setBlockType(BLOCKTYPE_PADSTACK);

	DataStruct *drillData = NULL;
   if (drillSize > 0)
   {
      // Create the drill and the drill layer, then insert into the padstack
      Graph_Tool(drillName, 0, drillSize, 0, 0, 0, 0L, plated);
      int layerIndex = Graph_Level("DRILLHOLE", "", FALSE);
      LayerStruct *Layer = sDoc->FindLayer(layerIndex);
      if (Layer)
      {
         Layer->setComment("Drill");
         Layer->setLayerType(LAYTYPE_DRILL);
      }
      drillData = Graph_Block_Reference(drillName, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, layerIndex, TRUE);
      drillData->getInsert()->setInsertType(insertTypeDrillHole);
   }
   Graph_Block_Off();

   DDEPadstackList *ddePadstack = new DDEPadstackList;
   sPadstackArray.SetAtGrow(sPadstackCnt++, ddePadstack);
   ddePadstack->index = padstackRefnum;
   ddePadstack->name = CCPadstackName;
   ddePadstack->geomNum = block->getBlockNumber();

	// Set these data so that subsequent ".psi" record can inherite them
	ddePadstack->masterBlock = block;
	ddePadstack->masterDrillData = drillData;
                                                 
   sCurrentState = STATE_PADSTACK;
   return 1;
}         

/******************************************************************************
* ipl_ali

   ALIAS ::= ".ali" aliasName
   aliasName = string
*/
static int ipl_ali()      
{
   return 1;
}

/******************************************************************************
* ipl_tol

   TOLERANCE ::= ".tol" tolerance
   tolerance = string
*/
static int ipl_tol()         
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString tolerance = lp;

   DDEPadstackList *padstack = sPadstackArray[sPadstackCnt-1];
   BlockStruct *block = sDoc->Find_Block_by_Num(padstack->geomNum);
   if (block)
   {
      int keyword = sDoc->RegisterKeyWord("DDE_TOLORANCE", 0, VT_STRING);
      sDoc->SetAttrib(&block->getAttributesRef(), keyword, VT_STRING, tolerance.GetBuffer(0), SA_OVERWRITE, NULL);
   }

   return 1;
}

/******************************************************************************
* ipl_lpl

   LAYER POLYGON ::= ".lpl" layerRefnum polygonUsage polygonRefnum
   layerRefnum    = integer
   polygonUsage   = "connected" | "unconnected" | "mir_connected " | "mir_unconnected"
   polygonRefnum  = integer
*/
static int ipl_lpl()         
{
   char  *lp;

   if (sCurrentState != STATE_PADSTACK)
      return 0;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString polygonUsage = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int polygonRefnum = atoi(lp);

   DDEPadstackList *ddePadstack = sPadstackArray[sPadstackCnt-1];
   BlockStruct *block = Graph_Block_On(GBO_APPEND, ddePadstack->name, -1, 0);

   DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);
   DDEPolygon *polygon = Find_Polygon_By_Refnum(polygonRefnum);

   DataStruct *data = Graph_Block_Reference(polygon->name, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, ddeLayer->layerNum, TRUE);
   data->getInsert()->setPlacedBottom(ddeLayer->bottom);
   Graph_Block_Off();

   return 1;
}

/******************************************************************************
* ipl_psi

   PASSTACK INSTANCE ::= ".psi" padstackInstRefnum
   padstackInstRefnum = integer
*/
static int ipl_psi()
{
   CCEtoODBDoc* doc = GraphGetDoc();

   CCamCadData& camCadData = doc->getCamCadData();

   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int padstackRefnum = atoi(lp);
   CString CCPadstackName = "PAD_";
   CCPadstackName += lp ;

   // Create the the new padstack
   BlockStruct *block = Graph_Block_On(GBO_APPEND, CCPadstackName, -1, 0);
   block->setBlockType(BLOCKTYPE_PADSTACK);
   Graph_Block_Off();

	// - The previously defined ".pst" record is the master padstack and master block
	// - The first ".psi" record after a ".pst" will inherited from the previously defined ".pst"
	// - Subsequence ".psi" record will inherited from the previously defined ".psi"

	// - Get the previously defined ".psi" record because it also share the same master padstack and master block
	DDEPadstackList *previousPadstack = sPadstackArray[sPadstackCnt - 1];

	// Copy the orignal name of the padstack
	block->getOriginalNameRef().Format("%s[%d]", previousPadstack->masterBlock->getOriginalName(), padstackRefnum);

	// Copy the drill from the previously defined padstack if there is one
	if (previousPadstack->masterDrillData)
	{
		DataStruct *drillData = camCadData.getNewDataStruct(*(previousPadstack->masterDrillData));
		block->getDataList().AddTail(drillData);
	}

	// Copy the attribute from the master block 
	sDoc->CopyAttribs(&block->getAttributesRef(), previousPadstack->masterBlock->getAttributesRef());

   DDEPadstackList *ddePadstack = new DDEPadstackList;
   sPadstackArray.SetAtGrow(sPadstackCnt++, ddePadstack);
   ddePadstack->index = padstackRefnum;
   ddePadstack->name = CCPadstackName;
   ddePadstack->geomNum = block->getBlockNumber();
	ddePadstack->masterBlock = previousPadstack->masterBlock;
	ddePadstack->masterDrillData = previousPadstack->masterDrillData;
                                                 
   sCurrentState = STATE_PADSTACK;
   return 1;
}

/******************************************************************************
* pl_typ

   TYPE (FOOTPRINT) ::= ".typ" xLoc yLoc width height footprintRefnum footprintName
   xLoc           = integer
   yLoc           = integer
   width          = integer
   height         = integer
   footprintRefnum= integer
   footprintName  = string
*/
static int ipl_typ()
{
   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double xLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double yLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double width = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double height = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int footprintRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString footprintName = Remove_Space_Quote(lp);

   char drive[_MAX_DRIVE];
   char directory[_MAX_DIR];
   char fileName[_MAX_FNAME];
   char extension[_MAX_EXT];
   _splitpath(footprintName, drive, directory, fileName, extension );
   CString name = fileName;

   if (Graph_Block_Exists(sDoc, name, -1))
   {
      name = footprintName;
      BlockStruct *block = Graph_Block_Exists(sDoc, name, -1);
      if (block)
      {
         fprintf(sErrorFile, "Shape [%s] already defined.\n", footprintName);
         sDisplayErrorCnt++;

         int count = 0;
         CString tmpName;
         while (block)
         {  
            tmpName.Format("%s_%d", name, ++count);
            block = Graph_Block_Exists(sDoc, tmpName, -1);
         }
         name = tmpName;
      }
   }
   BlockStruct *block = Graph_Block_On(GBO_APPEND, name, -1, 0);
   block->setBlockType(BLOCKTYPE_GENERICCOMPONENT);  // this is the default, change later if it is something else
   block->setComment(footprintName);
   Graph_Block_Off();

   DDEFootprint *footprint = new DDEFootprint;
   sFootprintArray.SetAtGrow(sFootprintCnt++, footprint);
   footprint->index = footprintRefnum;
   footprint->name = name;
   footprint->originalName = footprintName;
   footprint->x = xLoc;
   footprint->y = yLoc;
   footprint->height = height;
   footprint->width = width;
   footprint->geomNum = block->getBlockNumber();
   footprint->polyDataList = new CDataList(false);
   footprint->boardOutline = FALSE;

   sCurrentState = STATE_TYPE;
   return 1;
}

/******************************************************************************
* ipl_via

   VIA ::= ".via" padstackRefnum xLoc yLoc rotation
   padstackRefnum = integer
   xLoc           = integer
   yLoc           = integer
   rotation       = [0123]+[a[0-9]+[\.[0-9]+]?]? 
*/
static int ipl_via()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int padstackRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double xLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double yLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rotation = Convert_Rotation(lp);
   while (rotation < 0)
      rotation += 360;
   while (rotation >= 360)
      rotation -= 360;

   switch (sCurrentState)
   {
   case STATE_NETLIST:
      {
         // here via
         DDEPadstackList *padstack = Find_Padstack_By_Refnum(padstackRefnum);
         if (padstack == NULL)
         {
            // display message indicating there is not such padstackRefnum
            fprintf(sErrorFile, "Via with net name %s references an undefined padstack reference number %d\n",
                  sCurrentNet->getNetName(), padstackRefnum);
            sDisplayErrorCnt++;
            return 0;
         }

         DataStruct *data = Graph_Block_Reference(padstack->name, NULL, -1, xLoc, yLoc, DegToRad(rotation), 0, 
               1.0, Graph_Level("0","",1), TRUE);
         data->getInsert()->setInsertType(insertTypeVia);         
         sDoc->SetUnknownAttrib(&data->getAttributesRef(), ATT_NETNAME, sCurrentNet->getNetName(), SA_OVERWRITE, NULL);
      }
      break;
   case STATE_UNKNOWN:
      {
         // here a free pad
         DDEPadstackList *padstack = Find_Padstack_By_Refnum(padstackRefnum);
         if (padstack == NULL)
         {
            // display message indicating there is not such padstackRefnum
            fprintf(sErrorFile, "Via (free pad) references an undefined padstack reference number %d\n",
                  padstackRefnum);
            sDisplayErrorCnt++;
            return 0;
         }

         DataStruct *data = Graph_Block_Reference(padstack->name, NULL, -1, xLoc, yLoc, DegToRad(rotation), 0, 
               1.0, Graph_Level("0","",1), TRUE);
         data->getInsert()->setInsertType(insertTypeFreePad);
      }
      break;
   case STATE_TYPE:
      {
         DDEFootprint *footprint = sFootprintArray[sFootprintCnt-1];

         DDEPadstackList *padstack = Find_Padstack_By_Refnum(padstackRefnum);
         if (padstack == NULL)
         {
            // display message indicating there is not such padstackRefnum
            fprintf(sErrorFile, "Via with footprint name %s references an undefined padstack reference number %d\n",
                  footprint->name, padstackRefnum);
            sDisplayErrorCnt++;
            return 0;
         }

         Graph_Block_On(GBO_APPEND, footprint->name, -1, 0);
         DataStruct *data = Graph_Block_Reference(padstack->name, NULL, -1, xLoc, yLoc, DegToRad(rotation), 0, 
               1.0, Graph_Level("0","",1), TRUE);
         data->getInsert()->setInsertType(insertTypeVia);   
         Graph_Block_Off();
      }
      break;
   default:
      {
         // unknown status
         fprintf(sErrorFile, "Unknown .via status at %ld\n", sInputLineCnt);
         sDisplayErrorCnt++;
      }
      break;
   }

   return 1;
}

/******************************************************************************
* ipl_tra

   TRACK ::= ".tra" trackWidth startX startY endX endY layerRefnum
   trackWidth  = integer
   startX      = integer
   startY      = integer
   endX        = integer
   endY        = integer
   layerRefnum = integer
*/
static int ipl_tra()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double trackWidth = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double startX = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double startY = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double endX = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double endY = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);

   int err;
   int widthIndex = Graph_Aperture("", T_ROUND, trackWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   int state = sCurrentState;
   if (sSubState)
      state = sSubState;
   
   switch (state)
   {
   case STATE_TYPE:
      {
         DDEFootprint *footprint = sFootprintArray[sFootprintCnt-1];
         Graph_Block_On(GBO_APPEND, footprint->name, -1, 0);
         DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);
         DataStruct *polyData = Find_Footprint_PolyStruct(footprint, ddeLayer->layerNum);
         Graph_Poly(polyData, widthIndex, FALSE, FALSE, FALSE);

         startX = startX - footprint->x;
         startY = startY - footprint->y;
         endX = endX - footprint->x;
         endY = endY - footprint->y;

         Graph_Vertex(startX, startY, 0.0);
         Graph_Vertex(endX, endY, 0.0);
         Graph_Block_Off();
      }
      break;
   case STATE_AREA:
      {
         if (inFileSetting.HatchLineFill)
         {
            CPoly *poly = Graph_Poly(sCurrentArea.polyData, widthIndex, FALSE, FALSE, FALSE);
            poly->setHatchLine(true);
            Graph_Vertex(startX, startY, 0.0);
            Graph_Vertex(endX, endY, 0.0);
         }
      }
      break;
   case STATE_NETLIST:
      {
         if (sCurrentNet)
         {           
            DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);
            DataStruct *data = Graph_PolyStruct(ddeLayer->layerNum, 0L, 0);
            data->setGraphicClass(GR_CLASS_ETCH);
            sDoc->SetUnknownAttrib(&data->getAttributesRef(), ATT_NETNAME, sCurrentNet->getNetName(), SA_OVERWRITE, NULL);

            Graph_Poly(NULL, widthIndex, FALSE, FALSE, FALSE);
            Graph_Vertex(startX, startY, 0.0);
            Graph_Vertex(endX, endY, 0.0);
         }
      }
      break;
   default:
      break;
   }

   return 1;
}

/******************************************************************************
* ipl_arc

   ARC ::= ".arc" centerX centerY layerRefnum length startAngle endAngle trackWidth
   centerX     = integer
   centerY     = integer
   layerRefnum = integer
   length      = integer
   startAngle  = [0-9]+[\.[0-9]+]?
   endAngle    = [0-9]+[\.[0-9]+]?
   trackWidth  = integer
*/
static int ipl_arc()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double centerX = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double centerY = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double radius = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double startAngle = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double endAngle = atof(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double trackWidth = To_IPL_Unit(atof(lp));

   int err;
   int widthIndex = Graph_Aperture("", T_ROUND, trackWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   double deltaAngle = endAngle - startAngle;
   DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);

   if (sCurrentState == STATE_TYPE)
   {
      DDEFootprint *footprint = sFootprintArray[sFootprintCnt-1];
      Graph_Block_On(GBO_APPEND, footprint->name, -1, 0);
      centerX -= footprint->x;
      centerY -= footprint->y;
      if (fabs(deltaAngle) == 360)
         Graph_Circle(ddeLayer->layerNum, centerX, centerY, radius, 0L, widthIndex, FALSE, FALSE);
      else
         Graph_Arc(ddeLayer->layerNum, centerX, centerY, radius, DegToRad(startAngle), DegToRad(deltaAngle),
               0, widthIndex, FALSE);
      Graph_Block_Off();
   }
   else
   {
      if (fabs(deltaAngle) == 360)
         Graph_Circle(ddeLayer->layerNum, centerX, centerY, radius, 0L, widthIndex, FALSE, FALSE);
      else
         Graph_Arc(ddeLayer->layerNum, centerX, centerY, radius, DegToRad(startAngle), DegToRad(deltaAngle),
               0, widthIndex, FALSE);
   }

   return 1;
}

/******************************************************************************
* ipl_txt

   TEXT ::= ".txt" xLoc yLoc layerRefnum rotation mirror option height trackWidth text
   xLoc        = integer
   yLoc        = integer
   layerRefnum = integer
   rotation    = "0" | "1" | "2" | "3"
   mirror      = "0" | "1"
   
   option      = horizontal + vertical + textFlags
   horizontal  = "C" | "L" | "R"
   vertical    = "C" | "T" | "B"
   textFlag    = [slant] + [angle] + [stretch] + [visible] + [textKind]
   slant       = "s" + integer
   angle       = "a" + float
   stretch     = "w" + integer
   visible     = "i"
   textKind    = "M"

   height      = integer
   trackWidth  = integer
   text        = string
*/
static int ipl_txt()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double xLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double yLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rotation = 90.0 * atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int mirror = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString option = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double height = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double trackWidth = To_IPL_Unit(atoi(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString text = lp;
   if (text[0] == '\'')
   {
      if ((lp = strtok(NULL, "'\n")) != NULL)
         text += (CString)" " + lp;
   }
   text = Remove_Space_Quote(text);

   BOOL isAttribute = FALSE;
   if (strlen(text) == 0)
   {
      return 0;
   }
   else if(text[0] == '$')
   {
      text.Delete(0, 1);
      isAttribute = TRUE;
   }

   DbFlag textFlag = 0;
   double tmpRotation = 0.0;
   BOOL visible = TRUE;
   Get_Text_Option(option, textFlag, tmpRotation, visible);

   rotation += tmpRotation;
   while (rotation < 0)
      rotation += 360;
   while (rotation >= 360)
      rotation -= 360;

   int state = sCurrentState;
   if (sSubState)
      state = sSubState;

   int err = 0;
   int widthIndex = Graph_Aperture("", T_ROUND, trackWidth, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

   switch (state)
   {
   case STATE_TYPE:
      {
         Add_Footprint_Text(layerRefnum, text, textFlag, xLoc, yLoc, height, rotation, mirror, visible, widthIndex, isAttribute);
      }
      break;
   case STATE_AREA:
      {
         if (strlen(text))
            sDoc->SetUnknownAttrib(&sCurrentArea.polyData->getAttributesRef(), ATT_NAME, text, SA_OVERWRITE, NULL);
      }
      break;
   case STATE_COMPONENT:
      {
         Add_Component_Text(layerRefnum, text, textFlag, xLoc, yLoc, height, rotation, mirror, visible, widthIndex, isAttribute);
      }
      break;
   default: // such as pin and others
      {
         double tmpX = 0;
         double tmpY = 0;
         DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);

         normalize_text(&tmpX, & tmpY, textFlag, rotation, mirror, height, height * strlen(text) * TEXT_RATIO);
         xLoc += tmpX;
         yLoc += tmpY;
         DataStruct *data = Graph_Text(ddeLayer->layerNum, text, xLoc, yLoc, height, height * TEXT_RATIO, DegToRad(rotation), 0, 
               TRUE, mirror, 0, FALSE, widthIndex, 0);
         data->setHidden(!visible);
      }
      break;
   }

   return 1;
}

/******************************************************************************
* Add_Footprint_Text
*/
void Add_Footprint_Text(int layerRefnum, CString text, DbFlag textFlag, double xLoc, double yLoc, double height,
              double rotation, int mirror, BOOL visible, int widthIndex, BOOL isAttribute)
{
   DDEFootprint *footprint = sFootprintArray[sFootprintCnt-1];
   BlockStruct *block = sDoc->Find_Block_by_Num(footprint->geomNum);
   if (block == NULL)
      return;

   DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);
   int layerIndex = ddeLayer->layerNum;

   double x = xLoc - footprint->x;
   double y = yLoc - footprint->y;

   if (!isAttribute)
   {
      double tmpX = 0;
      double tmpY = 0;

      normalize_text(&tmpX, & tmpY, textFlag, rotation, mirror, height, height * strlen(text) * TEXT_RATIO);
      x += tmpX;
      y += tmpY;

      Graph_Block_On(GBO_APPEND, footprint->name, -1, 0);
      DataStruct *data = Graph_Text(layerIndex, text, x, y, height, height * TEXT_RATIO, DegToRad(rotation), 0, 
            TRUE, mirror, 0, FALSE, widthIndex, 0);
      data->setHidden(!visible);
      Graph_Block_Off();
   }
   else
   {
      CString key = Find_AttribMap(text);
      if (!key.CompareNoCase("DEVICE"))
         key = ATT_DEVICETYPE;
      else if (!key.CompareNoCase("NAME"))
         key = ATT_REFNAME;

      int keyword = sDoc->IsKeyWord(key, 0);
      Attrib* attrib = NULL;

      if (block->getAttributesRef())
         block->getAttributesRef()->Lookup(keyword, attrib);

	   if (attrib)
      {
         if (attrib->getLayerIndex() == 0 || attrib->getLayerIndex() == layerIndex)
         {
 				attrib->setX(x);
				attrib->setY(y);
				attrib->setRotationDegrees(rotation);
				attrib->setHeight(height);
				attrib->setWidth(height * TEXT_RATIO);
				attrib->setProportionalSpacing(true);
				attrib->setPenWidthIndex(widthIndex);
				attrib->setVisible(visible);
				attrib->setFlags(0);
				attrib->setLayerIndex(layerIndex);
				attrib->setMirrorDisabled(true);
         }
         else
         {
				Attrib* newAttrib = new Attrib(*attrib);
				newAttrib->setX(x);
				newAttrib->setY(y);
				newAttrib->setRotationDegrees(rotation);
				newAttrib->setHeight(height);
				newAttrib->setWidth(height * TEXT_RATIO);
				newAttrib->setProportionalSpacing(true);
				newAttrib->setPenWidthIndex(widthIndex);
				newAttrib->setVisible(visible);
				newAttrib->setFlags(0);
				newAttrib->setLayerIndex(layerIndex);
				newAttrib->setMirrorDisabled(true);

				attrib->addInstance(newAttrib);
         }
      }
      else
      {
         if (!key.CompareNoCase(ATT_COMPHEIGHT) || !key.CompareNoCase(ATT_TOLERANCE))
         {
            double value = 0.0;
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_UNIT_DOUBLE);
            sDoc->SetVisAttrib(&block->getAttributesRef(), keyword, VT_UNIT_DOUBLE, &value, x, y, 
						DegToRad(rotation), height, height * TEXT_RATIO, TRUE, widthIndex, visible, SA_RETURN,
						0, layerIndex, TRUE, horizontalPositionLeft, verticalPositionBaseline);
         }
         else if (!key.CompareNoCase(ATT_DEVICETYPE) || !key.CompareNoCase(ATT_REFNAME) || !key.CompareNoCase("ARTICLE"))
         {
            CString value = "";
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
            sDoc->SetVisAttrib(&block->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), x, y,
						DegToRad(rotation), height, height * TEXT_RATIO, TRUE, widthIndex, visible, SA_RETURN,
						0, layerIndex, TRUE, horizontalPositionLeft, verticalPositionBaseline);
         }
         else
         {
            CString value = "";
            sDoc->SetUnknownVisAttrib(&block->getAttributesRef(), key, value.GetBuffer(0), x, y, 
						DegToRad(rotation), height, height * TEXT_RATIO, TRUE, widthIndex, visible, SA_RETURN,
						0, layerIndex, TRUE, horizontalPositionLeft, verticalPositionBaseline);
         }
      }
   }
}

/******************************************************************************
* Add_Component_Text
*/
void Add_Component_Text(int layerRefnum, CString text, DbFlag textFlag, double xLoc, double yLoc, double height,
              double rotation, int mirror, BOOL visible, int widthIndex, BOOL isAttribute)
{
   DDEComponent *comp = sComponentArray[sComponentCnt-1];
   if (comp->data == NULL)
      return;

   DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);
   int layerIndex = ddeLayer->layerNum;

   double x = xLoc - comp->x;
   double y = yLoc - comp->y;

   if (!isAttribute)
   {
      double tmpX = 0;
      double tmpY = 0;

      normalize_text(&tmpX, & tmpY, textFlag, rotation, mirror, height, height * strlen(text) * TEXT_RATIO);
      xLoc += tmpX;
      yLoc += tmpY;

      DataStruct *data = Graph_Text(ddeLayer->layerNum, text, xLoc, yLoc, height, height * TEXT_RATIO, DegToRad(rotation), 0, 
            TRUE, mirror, 0, FALSE, widthIndex, 0);
      data->setHidden(!visible);
   }
   else
   {
      CString key = Find_AttribMap(text);
      if (!key.CompareNoCase("DEVICE"))
         key = ATT_DEVICETYPE;
      else if (!key.CompareNoCase("NAME"))
         key = ATT_REFNAME;

      int keyword = sDoc->IsKeyWord(key, 0);
      Attrib* attrib = NULL;

      if (comp->data->getAttributesRef())
         comp->data->getAttributesRef()->Lookup(keyword, attrib);

		if (attrib)
      {
			bool foundSameAttrib = false;
			AttribIterator attribIterator(attrib);
         Attrib tmpAttrib(attrib->getCamCadData());
			while (attribIterator.getNext(tmpAttrib))
			{
				if (tmpAttrib.getLayerIndex() == 0 || tmpAttrib.getLayerIndex() == layerIndex)
				{
					Attrib modifiedAttrib(tmpAttrib);

					foundSameAttrib = true;
					if (!key.CompareNoCase(ATT_REFNAME))
					{
						CString value = comp->name;
						if (!value.IsEmpty())
						{
							modifiedAttrib.setProperties(x, y, DegToRad(rotation), height, height * TEXT_RATIO, widthIndex, TRUE,
									visible, TRUE, FALSE, FALSE, horizontalPositionLeft, verticalPositionBaseline, layerIndex, 0);

                     modifiedAttrib.setValueFromString(sDoc->getCamCadData(), (WORD)keyword, value);
						}
					}
					else
					{	
						modifiedAttrib.setProperties(x, y, DegToRad(rotation), height, height * TEXT_RATIO, widthIndex, TRUE,
								visible, TRUE, FALSE, FALSE, horizontalPositionLeft, verticalPositionBaseline, layerIndex, 0);
					}

					attrib->updateInstanceLike(tmpAttrib, modifiedAttrib);
				}
			}

			if (!foundSameAttrib)
         {
				Attrib* newAttrib = new Attrib(*attrib);
				newAttrib->setX(x);
				newAttrib->setY(y);
				newAttrib->setRotationDegrees(rotation);
				newAttrib->setHeight(height);
				newAttrib->setWidth(height * TEXT_RATIO);
				newAttrib->setProportionalSpacing(true);
				newAttrib->setPenWidthIndex(widthIndex);
				newAttrib->setVisible(visible);
				newAttrib->setFlags(0);
				newAttrib->setLayerIndex(layerIndex);
				newAttrib->setMirrorDisabled(true);

				attrib->addInstance(newAttrib);
         }
      }
      else
      {
         if (!key.CompareNoCase(ATT_COMPHEIGHT) || !key.CompareNoCase(ATT_TOLERANCE))
         {
            double value = 0.0;
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_UNIT_DOUBLE);
            sDoc->SetVisAttrib(&comp->data->getAttributesRef(), keyword, VT_UNIT_DOUBLE, &value, x, y,
						DegToRad(rotation), height, height * TEXT_RATIO, TRUE, widthIndex, visible, SA_OVERWRITE,
						0, layerIndex, TRUE, horizontalPositionLeft, verticalPositionBaseline);
         }
         else if (!key.CompareNoCase(ATT_DEVICETYPE) || !key.CompareNoCase("ARTICLE"))
         {
            CString value = "";
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
            sDoc->SetVisAttrib(&comp->data->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), 
						x, y, DegToRad(rotation), height, height * TEXT_RATIO, TRUE, widthIndex, visible, 
						SA_OVERWRITE, 0, layerIndex, TRUE, horizontalPositionLeft, verticalPositionBaseline);
         }
         else if (!key.CompareNoCase(ATT_REFNAME))
         {
            CString value = comp->name;
            int keyword = sDoc->RegisterKeyWord(key, 0, VT_STRING);
            sDoc->SetVisAttrib(&comp->data->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), 
						x, y, DegToRad(rotation), height, height * TEXT_RATIO, TRUE, widthIndex, visible, 
						SA_OVERWRITE, 0, layerIndex, TRUE, horizontalPositionLeft, verticalPositionBaseline);
         }
         else
         {
            CString value = "";
            sDoc->SetUnknownVisAttrib(&comp->data->getAttributesRef(), key, value.GetBuffer(0), x, y,
						DegToRad(rotation), height, height * TEXT_RATIO, TRUE, widthIndex, visible, SA_OVERWRITE,
						0, layerIndex, TRUE, horizontalPositionLeft, verticalPositionBaseline);
         }        
      }
   }
}

/******************************************************************************
* ipl_dfa

   AREA ::= ".dfa" areaKind layerRefnum trackWidth areaFlag spacing areaName
   areaKind    = "ha" | "oha" | ??
   layerRefnum = integer
   trackWidth  = integer

   areaFlag    = filldir + [connected] + [plotboarder] + [viadrown] + [edgetype]
   filldir     = integer
   connected   = "c"
   plotboarder = "p"
   viadrown    = "v"
   edgetype    = "e" + integer

   spacing     = integer
   areaName    = string
*/
static int ipl_dfa()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString areaType = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double trackWidth = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;  // areaFlags
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;  // spacing
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString areaName = Remove_Space_Quote(lp);
   if (areaName == "*" || areaName == "[]" || areaName == "''")
      areaName = "";

   double xOffset = 0;
   double yOffset = 0;
   DataStruct *data;
   DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);
   if (sCurrentState == STATE_TYPE)
   {         
      DDEFootprint *comp = sFootprintArray[sFootprintCnt-1];
      Graph_Block_On(GBO_APPEND, comp->name, -1, 0);
      data = Graph_PolyStruct(ddeLayer->layerNum, 0L, 0);
      Graph_Block_Off();
      xOffset = comp->x;
      yOffset = comp->y;
   }
   else
   {
      data = Graph_PolyStruct(ddeLayer->layerNum, 0L, 0);
   }

   if (strlen(areaName))  // this .dfa is a net name
      sDoc->SetUnknownAttrib(&data->getAttributesRef(), ATT_NETNAME, areaName, SA_OVERWRITE, NULL); 

   BOOL filled = FALSE;
   if (!areaType.CompareNoCase("miller") || !areaType.CompareNoCase("outline"))
   {
      data->setGraphicClass(GR_CLASS_BOARDOUTLINE);
      LayerStruct *layer = sDoc->FindLayer(ddeLayer->layerNum);
      layer->setComment("miller");
      layer->setLayerType(LAYTYPE_BOARD_OUTLINE);

      if (sCurrentState == STATE_TYPE)
      {
         DDEFootprint *comp = sFootprintArray[sFootprintCnt-1];
         comp->boardOutline = TRUE;
      }

   }
   else if (!areaType.CompareNoCase("cmpsize"))
      data->setGraphicClass(GR_CLASS_COMPONENT_BOUNDARY);
   else if (!areaType.CompareNoCase("vko"))
      data->setGraphicClass(GR_CLASS_VIAKEEPOUT);
   else if (!areaType.CompareNoCase("vki"))
      data->setGraphicClass(GR_CLASS_VIAKEEPIN);
   else if (!areaType.CompareNoCase("tko"))
      data->setGraphicClass(GR_CLASS_ROUTKEEPOUT);
   else if (!areaType.CompareNoCase("tki"))
      data->setGraphicClass(GR_CLASS_ROUTKEEPIN);
   else if (!areaType.CompareNoCase("cko"))
      data->setGraphicClass(GR_CLASS_PLACEKEEPOUT);
   else if (!areaType.CompareNoCase("cki"))
      data->setGraphicClass(GR_CLASS_PLACEKEEPIN);
   else if (!areaType.CompareNoCase("ha") || !areaType.CompareNoCase("oha") || !areaType.CompareNoCase("ohh")||
            !areaType.CompareNoCase("ohx")|| !areaType.CompareNoCase("pha") || !areaType.CompareNoCase("eha")||
            !areaType.CompareNoCase("etch")) // hatch 
   {
      filled = TRUE;   
      // currently areaType "plane" are ignore, so the graphic_class = 0
   }

   sCurrentArea.name = areaName;
   sCurrentArea.trackWidth = trackWidth;
   sCurrentArea.polyData = data;
   sCurrentArea.xOffset = xOffset;
   sCurrentArea.yOffset = yOffset;
   sCurrentArea.filled = filled;
   sCurrentArea.polyData = data;

   if (sCurrentState == STATE_TYPE)
      sSubState = STATE_AREA;
   else
      sCurrentState = STATE_AREA;

   return 1;
}

/******************************************************************************
* ipl_iad

  INSTALLED AREA BORDER ::= ".ida" ????
*/
static int ipl_iad()
{
   return 1;
}

/******************************************************************************
* ipl_ena

   END AREA ::= ".ena"
*/
static int ipl_ena()
{
   if (sSubState ==  STATE_AREA) 
      sSubState = STATE_UNKNOWN;
   if (sCurrentState == STATE_AREA)
      sCurrentState = STATE_UNKNOWN;
   return 1;
}

/******************************************************************************
* ipl_dim

   DIMENSION ::= ".dim" dimensionKind spacing dimensionMode layerRefnum dimensionName
   dimensionKind  = "D" | "R" | "C" | "A" | "N" | "DB"
   spacing        = integer

   dimensionMode  = "A" + angle | "F" + angle
   angle          = [0-9]+[\.[0-9]+]?

   layerRefnum    = integer
   dimensionName  = string
*/
static int ipl_dim()
{
   return 1;
}

/******************************************************************************
* ipl_dob

   DIMENSION OBJECT ::= ".dob" dimensionObjKind xLoc yLoc layerRefnum dimensionObjName
   dimensionObjKind  = "A" | "P" | "C" | "F" | "R" | "AB" | "PB" | "CB" | "FB" | "RB"
   xLoc              = integer
   yLoc              = integer
   layerRefnum       = integer
   dimensionObjName  = string
*/
static int ipl_dob()
{
   return 1;
}

/******************************************************************************
* ipl_die

   DIMENSION END ::= ".die"
*/
static int ipl_die()
{
   return 1;
}

/******************************************************************************
* ipl_str

   STRIPLINE ::= ".str" layerRefnum netName
   layerRefnum = integer
   netName     = string
*/
static int ipl_str()
{
   return 1;
}

/******************************************************************************
* ipl_nod

   NODE ::= ".nod" nodeRefnum xloc yLoc
   nodeRefnum  = integer
   xLoc        = integer
   yLoc        = integer
*/
static int ipl_nod()
{
   return 1;
}

/******************************************************************************
* ipl_edg

   EDGE ::= ".edg" edgeRefnum nodeRefnum1 nodeRefnum2 [radius]
   edgeRefnum  = integer
   nodeRefnum1 = integer
   nodeRefnum2 = integer
   radius      = integer
*/
static int ipl_edg()
{
   return 1;
}

/******************************************************************************
* ipl_ste

   STRIPLINE END ::= ".ste"
*/
static int ipl_ste()
{
   return 1;
}

/******************************************************************************
* ipl_wlp

   PIN ::= ".wlp" componentRefnum xLoc yLoc rotation pinName padstackRefnum 
   componentRefnum= integer
   xLoc           = integer
   yLoc           = integer
   rotation       = [0123]+[a[0-9]+[\.[0-9]+]?]? 
   pinName        = string
   padstackRefnum = integer
*/
static int ipl_wlp()
{
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int componentRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double xLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double yLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rotation = Convert_Rotation(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString pinName = Remove_Space_Quote(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int padstackRefnum = atoi(lp);

   while (rotation < 0)
      rotation +=360;
   while (rotation >= 360)
      rotation -= 360;

   if (sCurrentState == STATE_TYPE)
   {
      DDEFootprint *footprint = sFootprintArray[sFootprintCnt-1];
      DDEPadstackList *padstack = Find_Padstack_By_Refnum(padstackRefnum);
      if (padstack == NULL)
      {
         // display message indicating there is not such padstackRefnum
         fprintf(sErrorFile, "Pin %s of %s references an undefined padstack reference number %d\n",
               pinName, footprint->name, padstackRefnum);
         sDisplayErrorCnt++;
         return 0;
      }

      int layerIndex = Graph_Level("0", "", TRUE);

      xLoc -= footprint->x;
      yLoc -= footprint->y;

      BlockStruct *block = Graph_Block_On(GBO_APPEND, footprint->name, -1, 0);
      block->setBlockType(BLOCKTYPE_PCBCOMPONENT);
      DataStruct *data = Graph_Block_Reference(padstack->name, pinName, 0, xLoc, yLoc, DegToRad(rotation), 0, 1.0, layerIndex, TRUE);
      data->getInsert()->setInsertType(insertTypePin);
      Graph_Block_Off();
   }
   else 
   {
      DDEComponent *comp = Find_Component_By_Refnum(componentRefnum);
      if (sCurrentState == STATE_NETLIST && strlen(sCurrentNet->getNetName()))
      {
         add_comppin(sFileStruct, sCurrentNet, comp->name, pinName);
      }
      else
      {
         NetStruct *net = add_net(sFileStruct, NET_UNUSED_PINS);
         add_comppin(sFileStruct, sCurrentNet, comp->name, pinName);
      }
   }

   return 1;
}

/******************************************************************************
* ipl_com

   COMPONENT ::= ".com" compRefnum xLoc yLoc rotation layerRefnum mirror compFix compStatus footprintRefnum compName
   compRefnum     = integer
   xLoc           = integer
   yLoc           = integer
   rotation       = [0123]+[a[0-9]+[\.[0-9]+]?]? 
   layerRefnum    = integer
   mirror         = "y" | "n"

   compFix        = fixFlag | fixFlag + "i" | fixFlag + "t" | fixFlag + "it"
   fixFlag        = "p" | "n" | "f" | "x" | "d"
   
   compStatus     = "0" | "1"
   footprintRefnum= integer
   compName       = string 
*/
static int ipl_com()
{
   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int compRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double xLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double yLoc = To_IPL_Unit(atof(lp));

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   double rotation = Convert_Rotation(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int layerRefnum = atoi(lp);
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   
   int mirror = 0;
   if (toupper(lp[0]) == 'Y'|| toupper(lp[0]) == 'M')
      mirror = 3;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString compFix = lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int compStatus = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   int footprintRefnum = atoi(lp);

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString compName = Remove_Space_Quote(lp);

   while (rotation < 0)
      rotation += 360;
   while (rotation >= 360)
      rotation -= 360;

   DDELayerList *ddeLayer = Find_Layer_By_Refnum(layerRefnum);
   DDEFootprint *footprint = Find_Footprint_By_Refnum(footprintRefnum);
   DataStruct *data = Graph_Block_Reference(footprint->name, compName, 0, xLoc, yLoc, DegToRad(rotation), 
         mirror, 1, ddeLayer->layerNum, TRUE);

   if (ddeLayer->bottom)
      data->getInsert()->setPlacedBottom(true);

   if (compFix.Find("t", 0) > -1)
      data->getInsert()->setInsertType(insertTypeMechanicalComponent);
   else
      data->getInsert()->setInsertType(insertTypePcbComponent);

   DDEComponent *comp = new DDEComponent;
   sComponentArray.SetAtGrow(sComponentCnt++, comp);
   comp->index = compRefnum;
   comp->name = compName;
   comp->x = xLoc;
   comp->y = yLoc;
   comp->data = data;
   comp->boardOutline = footprint->boardOutline;

   sCurrentState = STATE_COMPONENT;
   return 1;
}

/******************************************************************************
* ipl_wlg

   NETLIST ::= ".wlg" netStatus protect/non netName
   netStatus   = "0" | "1"
   protect     = "p" | "n"
   netName     = string
*/
static int ipl_wlg()
{
   sCurrentState = STATE_UNKNOWN;
   char  *lp;

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;  // status
   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   char protect = lp[0];

   if ((lp = strtok(NULL, " \t\n")) == NULL)
      return 0;
   CString netName = lp;

   if (protect == 'n' || protect == 'p')  // plane net.
   {
      netName.MakeUpper();   // all capital
      sCurrentNet = add_net(sFileStruct, netName);

      sCurrentState = STATE_NETLIST;
   }

   return 1;
}

/******************************************************************************
* ipl_wle

   NETLIST END ::= ".wle"
*/
static int ipl_wle()
{
   sCurrentState = STATE_UNKNOWN;
   return 1;
}

/******************************************************************************
* ipl_ncl
   
   NET CLASS ::= ".ncl" netClassName
   netClassName = string
*/
static int ipl_ncl()
{
   return 1;
}

/******************************************************************************
* ipl_ncc
   
   NET CLASS CLASS ::= ".ncc" netClassClassName
   netClassClassName = string
*/
static int ipl_ncc()
{
   return 1;
}

/******************************************************************************
* ipl_ccl

   COMPONENT CLASS ::= ".ccl" compClassName
   compClassName = string
*/
static int ipl_ccl()
{
   return 1;
}

/******************************************************************************
* ipl_ccc

   COMPONENT CLASS CLASS ::= ".ccc" compClassClassName
   compClassClassName = string
*/
static int ipl_ccc()
{
   return 1;
}

/******************************************************************************
* ipl_mem

   MEMBER ::= ".mem" memberName
   memberName = string
*/
static int ipl_mem()
{
   return 1;
}

/******************************************************************************
* ipl_end

   END ::= ".end"
*/
static int ipl_end()
{
   sInputFileEnd = TRUE;
   return 1;
}

/******************************************************************************
* ipl_unit
*   ipl to inch and round to 1/10 mil
*/
double To_IPL_Unit(double x)
{
   double y = x * Units_Factor(UNIT_MILS, sPageUnits) / 100;   
   return y;
}

/******************************************************************************
* Convert_Rotation
*/
double Convert_Rotation(CString rotationToConvert)
{
   CString rotString = rotationToConvert;
   rotString.TrimLeft();
   rotString.TrimRight();
   rotString.MakeUpper();

   double rotation = 0.0;
   int index = rotString.Find("A", 0);
   if (index > -1)
   {
      rotation = 90.0 * atoi(rotString.Left(index-1));
      rotString.Delete(0, index);

      if (strlen(rotString)) 
      {
			int i=0;
         for (i=0; i<(int)strlen(rotString); i++)
         {
            if (isalpha(rotString[i]))
                break;
         }
         rotation += atof(rotString.Left(i-1));
      }  
   }
   else
   {
      rotation = 90.0 * atoi(rotString);
   }

   return rotation;
}

/******************************************************************************
* Get_Text_Option
*/
void Get_Text_Option(CString textOption, DbFlag &flag, double &rotation, BOOL &visible)
{
   CString option = textOption;
   option.MakeUpper();

   flag = 0;
   rotation = 0.0;
   visible = TRUE;
   if (strlen(textOption) == 0)
      return;

   // Get the width justification
   if (option[0] == 'L')
      flag |= GRTEXT_W_L;
   else if (option[0] == 'C')
      flag |= GRTEXT_W_C;
   else if (option[0] == 'R')
      flag |= GRTEXT_W_R;

   // Get the height justification
   if (option[1] == 'B')
      flag |= GRTEXT_H_B;
   else if (option[1] == 'C')
      flag |= GRTEXT_H_C;
   else if (option[1] == 'T')
      flag |= GRTEXT_H_T;

   // Get the visibility
   if (textOption.Find("I", 0) > -1)
      visible = FALSE;

   // Get the slanted angle
   int index = option.Find("A", 0);
   if (index > -1)
   {
      option.Delete(0, index);
      if (strlen(option))
      {
			int i=0;
         for (i=0; i<(int)strlen(option); i++)
         {
            if (isalpha(option[i]))
                break;
         }
         rotation = atof(option.Left(i-1));
      }
   }
}

/******************************************************************************
* Remove_Space_Quote
*/
CString Remove_Space_Quote(CString name)
{
   CString newName = name;
   newName.TrimLeft();
   newName.TrimRight();
   if (newName.Left(1) == '\'')
      newName.Delete(0,1);
   if (newName.Right(1) == '\'')
      newName.Delete(strlen(newName) - 1, 1);

   return newName;
}

/******************************************************************************
* *Find_Material_By_Name
*/
DDEMaterial *Find_Material_By_Name(CString name)
{
   for (int i=0; i<sMaterialCnt; i++)
   {
      DDEMaterial *material = sMaterialArray[i];
      if (!material->name.CompareNoCase(name))
         return material;
   }

   return NULL;
}

/******************************************************************************
* *Find_Layer_By_Refnum
*/
DDELayerList *Find_Layer_By_Refnum(int refnum)
{
   for (int i=0; i<sLayerCnt; i++)
   {
      DDELayerList *ddeLayer = sLayerArray[i];
      if (ddeLayer->index == refnum)
         return ddeLayer;
   }

   return NULL;
}

/******************************************************************************
* *Find_Polygon_By_Refnum
*/
DDEPolygon *Find_Polygon_By_Refnum(int refnum)
{
   for (int i=0; i<sPolygonCnt; i++)
   {
      DDEPolygon *polygon = sPolygonArray[i];
      if (polygon->index == refnum)
         return polygon;
   }

   return NULL;
}

/******************************************************************************
* *Find_Padstack_By_Refnum
*/
DDEPadstackList *Find_Padstack_By_Refnum(int refnum)
{
   for (int i=0; i<sPadstackCnt; i++)
   {
      DDEPadstackList *padstack = sPadstackArray[i];
      if (padstack->index == refnum)
         return padstack;
   }

   return NULL;
}

/******************************************************************************
* *Find_Footprint_By_Refnum
*/
DDEFootprint *Find_Footprint_By_Refnum(int refnum)
{
   for (int i=0; i<sFootprintCnt; i++)
   {
      DDEFootprint *footprint = sFootprintArray[i];
      if (footprint->index == refnum)
         return footprint;
   }

   return NULL;
}

/******************************************************************************
* *Find_Component_By_Refnum
*/
DDEComponent *Find_Component_By_Refnum(int refnum)
{
   for (int i=0; i<sComponentCnt; i++)
   {
      DDEComponent *comp = sComponentArray[i];
      if (comp->index == refnum)
         return comp;
   }

   return NULL;
}


/******************************************************************************
* char *get_attribmap
*/
CString Find_AttribMap(CString attrib)
{
   for (int i=0; i<sAttribMapCnt; i++)
   {
      if (sAttribMapArray[i]->attrib.CompareNoCase(attrib) == 0)
         return sAttribMapArray[i]->mapAttrib;
   }

   CString tmp = attrib;
   tmp.MakeUpper();
   return tmp;
}

/******************************************************************************
* *Find_Footprint_PolyStruct
*/
DataStruct *Find_Footprint_PolyStruct(DDEFootprint *footprint, int layer)
{

   POSITION pos = footprint->polyDataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = footprint->polyDataList->GetNext(pos);
      if (data->getLayerIndex() == layer)
         return data;
   }

   DataStruct *data = Graph_PolyStruct(layer, 0L, FALSE);
   footprint->polyDataList->AddTail(data);
   return data;
}

/******************************************************************************
* get_height
*/
double Get_Height(const char *InValue)
{
   // mil = mil
   // M = 100 mils
   // mm = millimeters
   // um = micrometers
   // in = inch
   // cm = centimeter
   double height = atof(InValue);
   char tmp[255];
   strcpy(tmp, InValue);

   char *value = tmp;
   while (value && !isalpha(*value))
      value++;

   if (strlen(InValue))
   {
      if (!STRNICMP(value, "MM", 2))
         height *= Units_Factor(UNIT_MM, sPageUnits); 
      else if (!STRNICMP(value, "UM", 2))
         height *= Units_Factor(UNIT_MM, sPageUnits) * 1000;   
      else if (!STRNICMP(value, "CM", 2))
         height *= Units_Factor(UNIT_MM, sPageUnits) / 10;  
      else if (!STRNICMP(value, "MI", 2))
         height *= Units_Factor(UNIT_MILS, sPageUnits);  
      else if (!STRNICMP(value, "IN", 2))
         height *= Units_Factor(UNIT_INCHES, sPageUnits);   
      else if (!strcmp(value, "M"))
         height *= Units_Factor(UNIT_MILS, sPageUnits) / 100;  
   }

   return height;
}                                         

/******************************************************************************
* Get_Attribute_Value
*/
CString Get_Attribute_Value(Attrib* attrib)
{
   CString value = "";
   switch (attrib->getValueType())
   {
   case VT_INTEGER:
      {
         value.Format("%d", attrib->getIntValue());
      }
      break;
   case VT_DOUBLE:
   case VT_UNIT_DOUBLE:
   case VT_NONE:
      {
         value.Format("%d", attrib->getDoubleValue());
      }
      break;
   case VT_STRING:
   case VT_EMAIL_ADDRESS:
   case VT_WEB_ADDRESS:
      {
         value = attrib->getStringValue();
      }
      break;
   }

   return value;
}

/******************************************************************************
* Get_Layer_Type
*/
static int Get_LayerType_By_CCLayerName(const char *layerName)
{
   for (int i=0; i<MAX_LAYTYPE; i++)
   {
      if (_stricmp(layerTypeToString(i), layerName) == 0)
         return i;
   }

   return LAYTYPE_UNKNOWN; 
}

/******************************************************************************
* Get_LayerType_By_DDELayerName
*/
static int Get_LayerType_By_DDELayerName(const char *layerName)
{
   for (int i=0; i<sLayerAttrCnt; i++)
   {
      if (!sLayerAttrArray[i]->name.CompareNoCase(layerName))
         return sLayerAttrArray[i]->layerType;
   }

   return LAYTYPE_UNKNOWN; 
}

/******************************************************************************
* CreateDDELayer
*/
DDELayerList *CreateDDELayer(int layerRefnum, CString layerFlags, CString layerName)
{
   int electricalNum = 0;
   int layerType = LAYTYPE_UNKNOWN;
   if (!layerFlags.CompareNoCase("a"))
   {
      if (sIsBottomLayer)
         layerType = LAYTYPE_PASTE_BOTTOM;
      else
         layerType = LAYTYPE_PASTE_TOP;
   }
   else if (!layerFlags.CompareNoCase("s"))
   {
      if (sIsBottomLayer)
         layerType = LAYTYPE_MASK_BOTTOM;
      else
         layerType = LAYTYPE_MASK_TOP;
   }
   else if (!layerFlags.CompareNoCase("oedS"))
   {
      layerType = LAYTYPE_SIGNAL_BOT;
      sIsBottomLayer = TRUE;
      electricalNum = ++sElectricalStack;
   }
   else if (!layerFlags.CompareNoCase("oedC"))
   {
      layerType = LAYTYPE_SIGNAL_TOP;
      electricalNum = ++sElectricalStack;
   }
   else if (!layerFlags.CompareNoCase("oedI"))
   {
      layerType = LAYTYPE_SIGNAL_INNER;
      if (!sIsBottomLayer)
         electricalNum = ++sElectricalStack;
   }
   else if (!layerFlags.CompareNoCase("oedP"))
   {
      layerType = LAYTYPE_POWERNEG;
      if (!sIsBottomLayer)
         electricalNum = ++sElectricalStack;
   }
   else if (!layerFlags.CompareNoCase("G"))
   {
      layerType = LAYTYPE_DIALECTRIC;
   }
   else if (!layerFlags.CompareNoCase("dH"))
   {
      layerType = LAYTYPE_DIALECTRIC;
   }
   else if (!layerFlags.CompareNoCase("d") || !layerFlags.CompareNoCase("n"))
   {
      layerType = Get_LayerType_By_DDELayerName(layerName);
   }

   DDELayerList *ddeLayer = new DDELayerList;
   sLayerArray.SetAtGrow(sLayerCnt++, ddeLayer);
   ddeLayer->index = layerRefnum;
   ddeLayer->name = layerName;
   ddeLayer->mirrorIndex = -1;
   ddeLayer->color = 255;
   ddeLayer->layerType = layerType;
   ddeLayer->bottom = sIsBottomLayer;
   ddeLayer->electrialStackNum = electricalNum; 
   
   CString CCLayerName;
   CCLayerName.Format("L%d", layerRefnum);
   ddeLayer->layerNum =  Graph_Level(CCLayerName, "", FALSE);
   LayerStruct *layer = sDoc->FindLayer(ddeLayer->layerNum);
   layer->setComment(layerName);
   layer->setLayerType(layerType);
   layer->setElectricalStackNumber(electricalNum);

   return ddeLayer;
}