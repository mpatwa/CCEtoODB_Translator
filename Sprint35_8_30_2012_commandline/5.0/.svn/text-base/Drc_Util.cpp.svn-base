// $Header: /CAMCAD/4.5/Drc_Util.cpp 29    4/28/06 2:13p Kurt Van Ness $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "drc_util.h"
#include "graph.h"
#include "outline.h"
#include "attrib.h"
#include "pcbutil.h"
#include "drc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char *AlgTypes[];
//extern char *layertypes[];

/**************************************************************************/
/* 
this module is collecting of the electrical pins, vias, traces, netlist and
other electrical information. this is used in all modules which are dealing 
with checking of any kind. 

The follong modules use the drc module:

ta.cpp - test analysis
gerberpcb.cpp - derive netlist
*/
/**************************************************************************/

DRC_PadstackArray DRCpadstackarray; // this gets all padstack definition in derive connectivity
int DRCpadstackcnt;

DRC_AperturePadstackArray     DRCaperturepadstackarray;  // this gets all padstack definition in derive connectivity
int                           DRCaperturepadstackcnt;

DRC_PinkooArray               DRCpinkooarray;   // used in derive_connectivity
int                           DRCpinkoocnt;

DRC_ComponentArray            DRCcomponentarray; // used in derive_connectivity
int                           DRCcomponentcnt;

/****************************************************************************/
/*
*/
double drc_get_largestradius()
{
   int   i;
   double   largestradius = 0;

   for (i=0;i<DRCpadstackcnt;i++)
   {
      if (DRCpadstackarray[i]->shapetypetop)
      {
         if (largestradius < DRCpadstackarray[i]->sizetop)
            largestradius = DRCpadstackarray[i]->sizetop;
      }
      if (DRCpadstackarray[i]->shapetypebot)
      {
         if (largestradius < DRCpadstackarray[i]->sizebot)
            largestradius = DRCpadstackarray[i]->sizebot;
      }
      if (DRCpadstackarray[i]->shapetypeinner)
      {
         if (largestradius < DRCpadstackarray[i]->sizeinner)
            largestradius = DRCpadstackarray[i]->sizeinner;
      }
   }
   return largestradius;
}

/*****************************************************************************/
/*
   get electrical stack numbers to setup drc checking.
*/
int drc_check_layers(CCEtoODBDoc *doc, int *startstacknum, int *endstacknum)
{
   LayerStruct *layer;
   CString     mess;
   mess = "";

   *startstacknum = -1;
   *endstacknum = -1;

	int j=0;
   for (j=0; j< doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue;
      if (!layer->isVisible())    continue;
   
      if(layer->getElectricalStackNumber())
      {
         if (*startstacknum < 0) *startstacknum = layer->getElectricalStackNumber();
         if (*endstacknum < 0)      *endstacknum = layer->getElectricalStackNumber();
         if (*startstacknum > layer->getElectricalStackNumber())
            *startstacknum = layer->getElectricalStackNumber();
         if (*endstacknum < layer->getElectricalStackNumber())
            *endstacknum = layer->getElectricalStackNumber();
      }
   }

   if (*startstacknum < 0 || *endstacknum < 0)
   {
      mess = "No electrical LayerType found !\nPlease update LayerStacks before continuing.\n";
      MessageBox(NULL, mess, "Error: Layer Stackup !", MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }

   if (*endstacknum > 64)
   {
      mess = "Maximum 64 LayerStacks supported !\nPlease update LayerStacks before continuing.\n";
      MessageBox(NULL, mess, "Error: Layer Stackup !", MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue;
      if (!layer->isVisible())    continue;
   
      if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP && !layer->getElectricalStackNumber())
      {
         layer->setElectricalStackNumber(*startstacknum);
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT && !layer->getElectricalStackNumber())
      {
         layer->setElectricalStackNumber(*endstacknum);
      }
      else
      if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER && !layer->getElectricalStackNumber())
      {
         CString  tmp;
         tmp.Format("Layer [%s] is Type SIGNAL_INNER but has no ElectricalStackNumber\n.Use Type PAD_INNER if layer is used for all inner layers.",
            layer->getName());
         mess += tmp;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP && !layer->getElectricalStackNumber())
      {
         layer->setElectricalStackNumber(*startstacknum);
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM && !layer->getElectricalStackNumber())
      {
         layer->setElectricalStackNumber(*endstacknum);
      }
   }

   if (strlen(mess))
   {
      mess += "\nPlease update LayerStacks before continuing.\n";
      MessageBox(NULL, mess, "Error: Layer Stackup !", MB_ICONEXCLAMATION | MB_OK);
      return -1;
   }

   return 1;
}

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
static int DRC_GetPADSTACKData(CCEtoODBDoc *doc, CDataList *DataList, int insertLayer, 
                           double *drill, double scale, UINT64 *la,
                           int startstacknum, int endstacknum, DRC_Padstack *padstack )
{
   DataStruct  *np;
   int         layer;
   int         typ = 0; 
   UINT64      layersetall = 0, layersetinner = 0;

	int i = 0;
   for (i=startstacknum;i<=endstacknum;i++)
      layersetall |= drc_set_layerarray(i);

   for (i=startstacknum+1;i<endstacknum;i++)
      layersetinner |= drc_set_layerarray(i);

   *drill = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // no etch data.
      if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
               break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_TOOL))
            {
               *drill = block->getToolSize() * scale;
            }
            else
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               layer = np->getLayerIndex();
               if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
               {
                  // block_layer is not set.
                  if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     if (block->getShape() == T_COMPLEX)
                     {
                        block = doc->Find_Block_by_Num((int)block->getSizeA());
                     }
                     layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
                  }
               }

               if (layer == -1)
                  layer = insertLayer;

               if (layer < 0) break;
               LayerStruct *l = doc->getLayerArray()[layer];

               double   sizeA = block->getSizeA();
               double   sizeB = block->getSizeB();
               double   Xoffset = block->getXoffset();
               double   Yoffset = block->getYoffset();
               int      shape = block->getShape();

               // only evaluate this type of apertures;
               if (shape != T_ROUND && 
                   shape != T_SQUARE && 
                   shape != T_RECTANGLE && 
                   shape != T_OCTAGON && 
                   shape != T_OBLONG &&
                   shape != T_DONUT)   
               {
                  // complex aperture not evaluated in DRC - get padstack ???
                  doc->CalcBlockExtents(block);

                  sizeA = block->getXmax() - block->getXmin();
                  sizeB = block->getYmax() - block->getYmin();
                  
                  Xoffset = block->getXmin() + sizeA/2;
                  Yoffset = block->getYmin() + sizeB/2;
                  shape   = T_RECTANGLE;
               }
         
               if (l->getLayerType() == LAYTYPE_PAD_TOP ||
                   l->getLayerType() == LAYTYPE_SIGNAL_TOP)
               {
                  *la |= drc_set_layerarray(l->getElectricalStackNumber());

                  if (l->getNeverMirror()) 
                     typ |= 0x8;
                  else
                     typ |= 0x1;

                  padstack->shapetypetop = shape;
                  if (shape == T_RECTANGLE || shape == T_OBLONG)
                  {
                     if (sizeA < sizeB)
                     {
                        padstack->sizetop = sizeA;
                     }
                     else
                     {
                        padstack->sizetop = sizeB;
                     }
                  }
                  else
                  {
                     padstack->sizetop = sizeA;
                  }
                  padstack->xofftop = Xoffset;
                  padstack->yofftop = Yoffset;
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_BOTTOM ||
                   l->getLayerType() == LAYTYPE_SIGNAL_BOT)
               {
                  *la |= drc_set_layerarray(l->getElectricalStackNumber());

                  if (l->getMirrorOnly())  
                     typ |= 0x10;
                  else
                     typ |= 0x2;

                  padstack->shapetypebot = shape;

                  if (shape == T_RECTANGLE || shape == T_OBLONG)
                  {
                     if (sizeA < sizeB)
                        padstack->sizebot = sizeA;
                     else
                        padstack->sizebot = sizeB;
                  }
                  else
                     padstack->sizebot = sizeA;

                  padstack->xoffbot = Xoffset;
                  padstack->yoffbot = Yoffset;
               }
               else if (l->getLayerType() == LAYTYPE_PAD_ALL || l->getLayerType() == LAYTYPE_SIGNAL_ALL ||
                        l->getLayerType() == LAYTYPE_PAD_OUTER || l->getLayerType() == LAYTYPE_SIGNAL_OUTER)
               {
                  *la |= layersetall;

                  typ |= 0x3;
                  padstack->shapetypetop = shape;
                  double padsize = sizeA;
                  if (shape == T_RECTANGLE || shape == T_OBLONG)
                  {
                     if (sizeA < sizeB)
                        padsize = sizeA;
                     else
                        padsize = sizeB;
                  }

                  // there could have been an overwrite !
                  if (padstack->sizetop < padsize)
                     padstack->sizetop = padsize;
                  padstack->xofftop = Xoffset;
                  padstack->yofftop = Yoffset;

                  padstack->shapetypebot = shape;
                  padsize = sizeA;
                  if (shape == T_RECTANGLE || shape == T_OBLONG)
                  {
                     if (sizeA < sizeB)
                        padsize = sizeA;
                     else
                        padsize = sizeB;
                  }

                  // there could have been an overwrite !
                  if (padstack->sizebot < padsize)
                     padstack->sizebot = padsize;

                  padstack->xoffbot = Xoffset;
                  padstack->yoffbot = Yoffset;

                  padstack->shapetypeinner = shape;
                  if (shape == T_RECTANGLE || shape == T_OBLONG)
                  {
                     if (sizeA < sizeB)
                        padstack->sizeinner = sizeA;
                     else
                        padstack->sizeinner = sizeB;
                  }
                  else
                     padstack->sizeinner = sizeA;

                  padstack->xoffinner = Xoffset;
                  padstack->yoffinner = Yoffset;
               }
               else
               if (l->getLayerType() == LAYTYPE_PAD_INNER ||
                   l->getLayerType() == LAYTYPE_SIGNAL_INNER)
               {
                  if (l->getElectricalStackNumber())
                     *la |= drc_set_layerarray(l->getElectricalStackNumber());
                  else
                     *la |= layersetinner;

                  padstack->shapetypeinner = shape;
                  if (shape == T_RECTANGLE || shape == T_OBLONG)
                  {
                     if (sizeA < sizeB)
                        padstack->sizeinner = sizeA;
                     else
                        padstack->sizeinner = sizeB;
                  }
                  else
                     padstack->sizeinner = sizeA;

                  padstack->xoffinner = Xoffset;
                  padstack->yoffinner = Yoffset;
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
} /* end DRC_GetPadstackData */

//--------------------------------------------------------------
int drc_do_padstacks(CCEtoODBDoc *doc, int startstacknum, int endstacknum)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         DRC_Padstack *c = new DRC_Padstack;
         DRCpadstackarray.SetAtGrow(DRCpadstackcnt,c);  
         DRCpadstackcnt++;   

         double drillsize;
         UINT64 la = 0;

         c->name = block->getName();
         c->block_num = block->getBlockNumber();
         c->shapetypetop = 0;
         c->shapetypebot = 0;
         c->shapetypeinner = 0;
         c->sizetop = c->sizebot = c->sizeinner = 0;
         c->xofftop = c->xoffbot = c->xoffinner = 0;
         c->yofftop = c->yoffbot = c->yoffinner = 0;
         int typ = DRC_GetPADSTACKData(doc, &(block->getDataList()), -1, 
                     &drillsize, 1.0, &la, startstacknum, endstacknum, c);
         c->layerarray = la;
         c->typ = typ;
         c->drill = drillsize;
         c->smd = FALSE;

         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1);
         if (a)
            c->smd = TRUE;

      }
   }

   return 1;
}

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
static int DRC_GetAPERTUREPADSTACKData(CCEtoODBDoc *doc, CDataList *DataList, int insertLayer, 
                           double *drill, double scale, DRC_AperturePadstack *padstack )
{
   DataStruct *np;
   int layer;
   int typ = 0;   

   *drill = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      // no etch data.
      if (np->getGraphicClass() == GR_CLASS_ETCH)      continue;

      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
               break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_TOOL))
            {
               *drill = block->getToolSize() * scale;
               typ |= 0x4;
            }
            else if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               layer = np->getLayerIndex();

               if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
               {
                  // block_layer is not set.
                  if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     if (block->getShape() == T_COMPLEX)
                     {
                        block = doc->Find_Block_by_Num((int)block->getSizeA());
                     }

                     layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
                  }
               }

               if (layer == -1)
                  layer = insertLayer;

               if (layer < 0) break;

               LayerStruct *l = doc->getLayerArray()[layer];
   
               if (l->getLayerType() == LAYTYPE_PAD_TOP ||
                   l->getLayerType() == LAYTYPE_SIGNAL_TOP)
               {
                  if (l->getNeverMirror()) 
                     typ |= 0x8;
                  else
                     typ |= 0x1;

                  padstack->aperture_num_top = block->getBlockNumber();
               }
               else if (l->getLayerType() == LAYTYPE_PAD_BOTTOM ||
                        l->getLayerType() == LAYTYPE_SIGNAL_BOT)
               {
                  if (l->getMirrorOnly())  
                     typ |= 0x10;
                  else
                     typ |= 0x2;

                  padstack->aperture_num_bot = block->getBlockNumber();
               }
               else if (l->getLayerType() == LAYTYPE_PAD_ALL ||
                        l->getLayerType() == LAYTYPE_SIGNAL_ALL ||
                        l->getLayerType() == LAYTYPE_PAD_OUTER)
               {
                  typ |= 3;
                  padstack->aperture_num_top = block->getBlockNumber();
                  padstack->aperture_num_bot = block->getBlockNumber();
               }
               else if (l->getLayerType() == LAYTYPE_PAD_INNER ||
                        l->getLayerType() == LAYTYPE_SIGNAL_INNER)
               {
                  // no inner
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
} /* end DRC_GetAperturePadstackData */

//--------------------------------------------------------------
// this only work for the generic layers TOP, BOT, DRILL
int drc_do_aperturepadstacks(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         DRC_AperturePadstack *c = new DRC_AperturePadstack;
         DRCaperturepadstackarray.SetAtGrow(DRCaperturepadstackcnt,c);  
         DRCaperturepadstackcnt++;    

         double drillsize;
         unsigned long la = 0;
         c->name = block->getName();
         c->block_num = block->getBlockNumber();
         c->aperture_num_top = -1;
         c->aperture_num_bot = -1;
         int typ = DRC_GetAPERTUREPADSTACKData(doc, &(block->getDataList()), -1, &drillsize, 1.0,  c);
         c->typ = typ;
         c->drill = drillsize;
         c->smd = FALSE;

         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1);
         if (a)
            c->smd = TRUE;

      }
   }

   return 1;
}

//--------------------------------------------------------------
int drc_get_padstackindex(const char *p)
{
   int   i;
   
   for (i=0;i<DRCpadstackcnt;i++)
   {
#ifdef _DEBUG
   DRC_Padstack *ps = DRCpadstackarray[i];
#endif
      if (!DRCpadstackarray[i]->name.Compare(p))
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
int drc_get_aperturepadstackindex(const char *p)
{
   int   i;
   
   for (i=0;i<DRCaperturepadstackcnt;i++)
   {
      if (!DRCaperturepadstackarray[i]->name.Compare(p))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
UINT64 drc_set_layerarray(int nr)
{
   UINT64 l = 1L << (nr-1);
   return l;
}

//--------------------------------------------------------------
int drc_is_comp_mirror(const char *c)
{
   int   i;

   for (i=0;i<DRCcomponentcnt;i++)
   {
      if (DRCcomponentarray[i]->compname == c)
         return DRCcomponentarray[i]->mirror;
   }

   return 0;
}

//--------------------------------------------------------------
int drc_get_comp_ptr(const char *c)
{
   int   i;

   for (i=0;i<DRCcomponentcnt;i++)
   {
      if (DRCcomponentarray[i]->compname == c)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
int drc_do_aperturenetlistpinloc(FILE *flog, CCEtoODBDoc *doc, CNetList *NetList, 
                         double scale, CDataList *DataList)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         if (compPin->getPinCoordinatesComplete())
         {
            CString padstackname;

            if (compPin->getPadstackBlockNumber() > -1)
            {
               BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
               if (block)
                  padstackname = block->getName();
               else
               {
                  fprintf(flog, "Can not find a padstack for Component [%s] Pin [%s]\n",
                        compPin->getRefDes(), compPin->getPinName());
                  //display_error++;
                  continue;
               }
            }

            int pi = drc_get_aperturepadstackindex(padstackname);

            if (pi < 0)
            {
               fprintf(flog,"Padstack [%s] not found in index\n", padstackname);
               //display_error++;
               continue;
            }

            // here update DRCpinkooarray
            DRC_pinkoo *c = new DRC_pinkoo;
            DRCpinkooarray.SetAtGrow(DRCpinkoocnt,c);  
            DRCpinkoocnt++;   
            c->entitynumber = compPin->getEntityNumber();
            c->compname = compPin->getRefDes();
            c->pinname = compPin->getPinName();
            c->netname = net->getNetName();
            c->x = compPin->getOriginX() * scale;
            c->y = compPin->getOriginY() * scale;
            c->mirror = (compPin->getMirror())?1:0;
            c->rotation = compPin->getRotationRadians();
            c->aperturepadstackindex = pi;
            c->result = -1;   // marked as outside

            // just for debug
            DRC_AperturePadstack *app = DRCaperturepadstackarray[pi];

            if (drc_is_comp_mirror(compPin->getRefDes()))
            {
               if (DRCaperturepadstackarray[pi]->typ & 0x10)
               {
                  // use only bottom
                  c->layer = 2;
               }
               else
               if (DRCaperturepadstackarray[pi]->typ == 1)
                  c->layer = 2;
               else
               if (DRCaperturepadstackarray[pi]->typ == 2)
                  c->layer = 1;
               else
                  c->layer = DRCaperturepadstackarray[pi]->typ;
            }
            else
            {
               if (DRCaperturepadstackarray[pi]->typ & 0x8)
               {
                  // use only top
                  c->layer = 1;
               }
               else
               if (DRCaperturepadstackarray[pi]->typ == 1)
                  c->layer = 1;
               else
               if (DRCaperturepadstackarray[pi]->typ == 2)
                  c->layer = 2;
               else
                  c->layer = DRCaperturepadstackarray[pi]->typ;
            }
         }
      }
   }
   return 1;
}

/******************************************************************************
* drc_init
*/
void drc_init()
{
   DRCpadstackarray.SetSize(100,100);
   DRCpadstackcnt = 0;

   DRCaperturepadstackarray.SetSize(100,100);
   DRCaperturepadstackcnt = 0;

   DRCpinkooarray.SetSize(100,100);
   DRCpinkoocnt = 0;

   DRCcomponentarray.SetSize(100,100);
   DRCcomponentcnt = 0;
}

/******************************************************************************
* drc_deinit
*/
void drc_deinit()
{
   int   i;

   for (i=0;i<DRCpadstackcnt;i++)
   {
      delete DRCpadstackarray[i];
   }
   DRCpadstackarray.RemoveAll();

   for (i=0;i<DRCaperturepadstackcnt;i++)
   {
      delete DRCaperturepadstackarray[i];
   }
   DRCaperturepadstackarray.RemoveAll();

   for (i=0;i<DRCpinkoocnt;i++)
   {
      delete DRCpinkooarray[i];
   }
   DRCpinkooarray.RemoveAll();

   for (i=0;i<DRCcomponentcnt;i++)
   {
      delete DRCcomponentarray[i];
   }
   DRCcomponentarray.RemoveAll();
}

//--------------------------------------------------------------
void drc_load_allcomps(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;

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
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   
            {
               // EXPLODEALL - call DoWriteData() recursively to write embedded entities
               point2.x = np->getInsert()->getOriginX() * scale;
               if (mirror)
                  point2.x = -point2.x;
               
               point2.y = np->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               DRC_component *c = new DRC_component;
               DRCcomponentarray.SetAtGrow(DRCcomponentcnt,c);  
               DRCcomponentcnt++;   

               //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               //double block_rot = rotation + np->getInsert()->getAngle();
               c->compname = np->getInsert()->getRefname();
               c->mirror = np->getInsert()->getMirrorFlags();
               c->x = point2.x;
               c->y = point2.y;
               c->rot = np->getInsert()->getAngle();
               c->geomnum = np->getInsert()->getBlockNumber();
               c->test_connector = FALSE;
               c->bottom_placed = np->getInsert()->getPlacedBottom();

               Attrib *a;
               if (a = is_attvalue(doc, np->getAttributesRef(), ATT_TEST_CONNECTOR, 0))   
               {
                  c->test_connector = TRUE;
               }
            } // case INSERT
         }
         break;
      } // end switch
   } // end drc_load_allcomps */

   return ;
}

/*****************************************************************************/
/*
   the layername needs the stacknumber to know where it is on
*/
int drc_probable_short_pin_pin(CCEtoODBDoc *doc, FileStruct *file, const char *layername,
                           const char *comment, double x, double y, int layertype, int stackupnumber,
                           double check_value, double actual_value,
                           int pin1entity, double pin1x, double pin1y, int pin2entity, double pin2x, double pin2y)
{
   int algindex = GetAlgorithmNameIndex(doc, "Pin-Pin Probable Short");
   DataStruct *data;
   DRCStruct *drc = AddDRCAndMarker(doc, file, x, y, comment, DRC_CLASS_MEASURE, 0, algindex, 
         DRC_ALG_PROBABLESHORT_PINtoPIN, layername, &data);
   DRC_FillMeasure(drc, DRC_TYPE_COMPPIN, pin1entity, DRC_TYPE_COMPPIN, pin2entity, (DbUnit)pin1x, (DbUnit)pin1y, (DbUnit)pin2x, (DbUnit)pin2y);

// doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_DFM_TESTNAME, AlgTypes[DRC_ALG_PROBABLESHORT_PINtoPIN], SA_OVERWRITE, NULL);
// doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_DFM_LAYERTYPE, layertypes[layertype], SA_OVERWRITE, NULL);

   // ATT_DFM_FAILURE_RANGE       
// doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &check_value, SA_OVERWRITE, NULL);
// doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &actual_value, SA_OVERWRITE, NULL);

   return 1;
}

/******************************************************************************
* drc_probable_short_pin_feature
*/
int drc_probable_short_pin_feature(CCEtoODBDoc *doc, FileStruct *file, const char *layername,
                           const char *comment, double x, double y, int layertype, int stackupnumber,
                           double check_value, double actual_value,
                           int pinentity, double pinx, double piny, int viaentity, double viax, double viay)
{
   int algindex = GetAlgorithmNameIndex(doc, "Pin-Feature Probable Short");
   DataStruct *data;
   DRCStruct *drc = AddDRCAndMarker(doc, file, x, y, comment, DRC_CLASS_MEASURE, 0, algindex, 
         DRC_ALG_PROBABLESHORT_PINtoFEATURE, layername, &data);
   DRC_FillMeasure(drc, DRC_TYPE_COMPPIN, pinentity, DRC_TYPE_DATALIST, viaentity, (DbUnit)pinx, (DbUnit)piny, (DbUnit)viax, (DbUnit)viay);

// doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_DFM_TESTNAME, AlgTypes[DRC_ALG_PROBABLESHORT_PINtoFEATURE], SA_OVERWRITE, NULL);
// doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_DFM_LAYERTYPE, layertypes[layertype], SA_OVERWRITE, NULL);

   // ATT_DFM_FAILURE_RANGE       
// doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &check_value, SA_OVERWRITE, NULL);
// doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &actual_value, SA_OVERWRITE, NULL);

   return 1;
}

/******************************************************************************
* drc_probable_short_feature_feature
*/
int drc_probable_short_feature_feature(CCEtoODBDoc *doc, FileStruct *file, const char *layername,
                           const char *comment, double x, double y, int layertype, int stackupnumber,
                           double check_value, double actual_value,
                           int via1entity, double via1x, double via1y, int via2entity, double via2x, double via2y)
{
   int algindex = GetAlgorithmNameIndex(doc, "Feature-Feature Probable Short");
   DataStruct *data;
   DRCStruct *drc = AddDRCAndMarker(doc, file, x, y, comment, DRC_CLASS_MEASURE, 0, algindex, 
         DRC_ALG_PROBABLESHORT_FEATUREtoFEATURE, layername, &data);
   DRC_FillMeasure(drc, DRC_TYPE_DATALIST, via1entity, DRC_TYPE_DATALIST, via2entity, (DbUnit)via1x, (DbUnit)via1y, (DbUnit)via2x, (DbUnit)via2y);

// doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_DFM_TESTNAME, AlgTypes[DRC_ALG_PROBABLESHORT_FEATUREtoFEATURE], SA_OVERWRITE, NULL);
// doc->SetUnknownAttrib(&data->getAttributesRef(), ATT_DFM_LAYERTYPE, layertypes[layertype], SA_OVERWRITE, NULL);

   // ATT_DFM_FAILURE_RANGE       
// doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_DFM_CHECK_VALUE, 0), VT_UNIT_DOUBLE, &check_value, SA_OVERWRITE, NULL);
// doc->SetAttrib(&data->getAttributesRef(), doc->IsKeyWord(ATT_DFM_ACTUAL_VALUE, 0), VT_UNIT_DOUBLE, &actual_value, SA_OVERWRITE, NULL);

   return 1;
}

/*****************************************************************************/
/*
*/
int drc_net_without_access(CCEtoODBDoc *doc, FileStruct *file, const char *netname)
{
   int algIndex = GetAlgorithmNameIndex(doc, "Net Without Access");
   DRCStruct *drc = CreateDRC(file, netname, DRC_CLASS_NETS, 0, algIndex, DFT_ALG_NET_WITHOUT_ACCESS);
   DRC_FillNets(drc, netname);
   return 1;
}

/******************************************************************************
* drc_singlepin_without_access
*/
void drc_singlepin_without_access(CCEtoODBDoc *doc, FileStruct *file, double x, double y, const char *comp, const char *pin)
{
   int algIndex = GetAlgorithmNameIndex(doc, "Single Pin Without Access");

   CString comment;
   comment.Format("%s %s", comp, pin);

   CString layerName = "SinglePinWithoutAccess";

   DataStruct *insert;
   DRCStruct *drc = AddDRCAndMarker(doc, file, x, y, comment, DRC_CLASS_SIMPLE, 0, algIndex, DFT_ALG_NET_WITHOUT_ACCESS, layerName, &insert);
}

/*****************************************************************************/
/*
*/
int drc_net_without_probe(CCEtoODBDoc *doc, FileStruct *file, const char *netname)
{
   int algindex = GetAlgorithmNameIndex(doc, "Net Without Probe");
   DRCStruct *drc = CreateDRC(file, netname, DRC_CLASS_NETS, 0, algindex, DFT_ALG_NET_WITHOUT_PROBE);
   DRC_FillNets(drc, netname);
   return 1;
}

/******************************************************************************
* drc_singlepin_without_probe
*/
void drc_singlepin_without_probe(CCEtoODBDoc *doc, FileStruct *file, double x, double y, const char *comp, const char *pin)
{
   int algIndex = GetAlgorithmNameIndex(doc, "Net Without Probe");

   CString comment;
   comment.Format("%s %s", comp, pin);

   CString layerName = "NetWithoutAccess";

   DataStruct *insert;
   DRCStruct *drc = AddDRCAndMarker(doc, file, x, y, comment, DRC_CLASS_SIMPLE, 0, algIndex, DFT_ALG_NET_WITHOUT_PROBE, layerName, &insert);
}



