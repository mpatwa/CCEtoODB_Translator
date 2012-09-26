// $Header: /CAMCAD/4.6/read_wrt/Rdc_out.cpp 23    4/03/07 5:52p Lynn Phung $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1998. All Rights Reserved.

   Questions : Units are in NANOMETERS
               Only 1 PCB file per DbFrame file.
               Panels
               What to do if a netname or other names have a space
               What is sig_itm
*/

// RDC_out.CPP
                              
#include "stdafx.h"
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include <math.h>
#include "CCEtoODB.h"
#include "RwUiLib.h"

/* Structures **************************************************************/

typedef  struct
{
   CString  net_name;
   int      sig_siz;    // number of comp-pins in net
} RDCNetl;
typedef CTypedPtrArray<CPtrArray, RDCNetl*> CNetlArray;

typedef  struct
{
   CString  comp_name;
   int      pincnt;
} RDCCompl;
typedef CTypedPtrArray<CPtrArray, RDCCompl*> CComplArray;

typedef  struct
{
   CString  pin_name;
   int      comp_idx;   // ptr+1 into complistarray
   int      pin_idx;    // pin index starting 1 .. n per component
   int      netptr;     // pointer into netlistarray
   long     pinx, piny;
} RDCCompPinl;
typedef CTypedPtrArray<CPtrArray, RDCCompPinl*> CCompPinlArray;

/* Function Define *********************************************************/


/* Globals ****************************************************************/

extern   CProgressDlg *progress;
//extern   char        *layertypes[];

static   CCEtoODBDoc  *doc;
static   double      unitsFactor;

static   int         display_error;
static   FILE        *flog;
static   int         PageUnits;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD
static   int         Report_UnusedPins;
static   CString     VianamePrefix;
static   CNetlArray     netlistarray;
static   int            netlistcnt = 0;
static   CComplArray    complistarray;
static   int            complistcnt = 0;
static   CCompPinlArray comppinlistarray;
static   int            comppinlistcnt = 0;

static   int            hash_otl_max = 0;
static   int            hash_sig_max = 0;
static   int            hash_cmp_max = 0;
static   int            hash_pkg_max = 0;
static   int            hash_typ_max = 0;
static   int            hash_uut_max = 0;
static   int            hash_box_max = 0;

//--------------------------------------------------------------
static long cnv_units(double x)
{
   long l;
   l = (long)floor(x * unitsFactor + 0.5);
   return l;
}

//-----------------------------------------------------------------------------
//
static int RDC_Graph_Line(FILE *wfp, const char *otl_name, int cnt,
                          double p1x, double p1y, double p1bulge, double p2x, double p2y )
{
   // otl_pr1 = x
   // otl_pr2 = y
   // otl_pr3 = start angle
   // otl_pr4 = stop angle
   // otl_pr5 = radius

   if (fabs(p1bulge) > BULGE_THRESHOLD)
   {
      double   cx,cy,r,sa;
      double   da = atan(p1bulge) * 4;

      ArcPoint2Angle(p1x,p1y,p2x,p2y,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;

      if (da < 0) 
      {
         cnt++;
         fprintf(wfp,"otl_typ %s*%d* polyline\n", otl_name, cnt);
         fprintf(wfp,"otl_pr1 %s*%d* %ld\n", otl_name, cnt, cnv_units(p2x));
         fprintf(wfp,"otl_pr2 %s*%d* %ld\n", otl_name, cnt, cnv_units(p2y));
         fprintf(wfp,"otl_pr3 %s*%d* -\n", otl_name, cnt);
         fprintf(wfp,"otl_pr4 %s*%d* -\n", otl_name, cnt);
         fprintf(wfp,"otl_pr5 %s*%d* -\n", otl_name, cnt);
         cnt++;
         fprintf(wfp,"otl_typ %s*%d* arc\n", otl_name, cnt);
         fprintf(wfp,"otl_pr1 %s*%d* %ld\n", otl_name, cnt, cnv_units(cx));
         fprintf(wfp,"otl_pr2 %s*%d* %ld\n", otl_name, cnt, cnv_units(cy));
         fprintf(wfp,"otl_pr3 %s*%d* %ld\n", otl_name, cnt, round(RadToDeg(sa-da)));
         fprintf(wfp,"otl_pr4 %s*%d* %ld\n", otl_name, cnt, round(RadToDeg(sa)));
         fprintf(wfp,"otl_pr5 %s*%d* %ld\n", otl_name, cnt, cnv_units(r));
      }
      else
      {
         cnt++;
         fprintf(wfp,"otl_typ %s*%d* arc\n", otl_name, cnt);
         fprintf(wfp,"otl_pr1 %s*%d* %ld\n", otl_name, cnt, cnv_units(cx));
         fprintf(wfp,"otl_pr2 %s*%d* %ld\n", otl_name, cnt, cnv_units(cy));
         fprintf(wfp,"otl_pr3 %s*%d* %ld\n", otl_name, cnt, round(RadToDeg(sa)));
         fprintf(wfp,"otl_pr4 %s*%d* %ld\n", otl_name, cnt, round(RadToDeg(sa+da)));
         fprintf(wfp,"otl_pr5 %s*%d* %ld\n", otl_name, cnt, cnv_units(r));
      }
   }
   else
   {
      cnt++;
      fprintf(wfp,"otl_typ %s*%d* polyline\n", otl_name, cnt);
      fprintf(wfp,"otl_pr1 %s*%d* %ld\n", otl_name, cnt, cnv_units(p1x));
      fprintf(wfp,"otl_pr2 %s*%d* %ld\n", otl_name, cnt, cnv_units(p1y));
      fprintf(wfp,"otl_pr3 %s*%d* -\n", otl_name, cnt);
      fprintf(wfp,"otl_pr4 %s*%d* -\n", otl_name, cnt);
      fprintf(wfp,"otl_pr5 %s*%d* -\n", otl_name, cnt);
   }
   
   return cnt;
}

//--------------------------------------------------------------
// write geom outline
static int RDC_WriteOutlineData(FILE *wfp, const char *geom_name, CDataList *DataList, 
      double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, long graphic_class)
{
   Mat2x2   m;
   DataStruct *np;
   Point2   point2;
   int      layer;
   int      poly_found = 0;
   CString  otl_name;

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
      
         if (graphic_class && (np->getGraphicClass() != graphic_class))
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
               otl_name.Format("%s_%d",geom_name, ++poly_found);
               fprintf(wfp,"box_onam %s*%d* %s\n", geom_name, poly_found, otl_name);
               fprintf(wfp,"otl_nam %s -\n",  otl_name);
               fprintf(wfp,"otl_idx %d %s\n",  poly_found, otl_name);

               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               int first = TRUE;
               Point2   p1,p2;
               int      outline_found = 0;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
               
                  if (first)
                  {
                     first = FALSE;
                  }
                  else
                  {
                     if (p1.bulge < 0)
                     {
                        // clockwise
                        fprintf(wfp,"otl_siz %s_%d %d\n", geom_name, poly_found, outline_found);

                        fprintf(wfp,"### Clockwise Arc found\n");
                        otl_name.Format("%s_%d",geom_name, ++poly_found);
                        fprintf(wfp,"box_onam %s*%d* %s\n", geom_name, poly_found, otl_name);
                        fprintf(wfp,"otl_nam %s -\n",  otl_name);
                        fprintf(wfp,"otl_idx %d %s\n",  poly_found, otl_name);

                        outline_found = 0;
                        outline_found = RDC_Graph_Line(wfp, otl_name, outline_found, 
                                                      p1.x, p1.y, p1.bulge, p2.x, p2.y);
                        fprintf(wfp,"otl_siz %s_%d %d\n", geom_name, poly_found, outline_found);
                        fprintf(wfp,"### End Clockwise Arc found\n");

                        otl_name.Format("%s_%d",geom_name, ++poly_found);
                        fprintf(wfp,"box_onam %s*%d* %s\n", geom_name, poly_found, otl_name);
                        fprintf(wfp,"otl_nam %s -\n",  otl_name);
                        fprintf(wfp,"otl_idx %d %s\n",  poly_found, otl_name);

                        outline_found = 0;
                     }
                     else
                     {
                        outline_found = RDC_Graph_Line(wfp, otl_name, outline_found, 
                                                      p1.x, p1.y, p1.bulge, p2.x, p2.y);
                     }
                  }
                  p1 = p2;
               }  // while

               if (first)
               {
                  if (p2.bulge < 0)
                  {
                     // clockwise
                     fprintf(wfp,"otl_siz %s_%d %d\n", geom_name, poly_found, outline_found);

                     fprintf(wfp,"### Clockwise Arc found\n");
                     otl_name.Format("%s_%d",geom_name, ++poly_found);
                     fprintf(wfp,"box_onam %s*%d* %s\n", geom_name, poly_found, otl_name);
                     fprintf(wfp,"otl_nam %s -\n",  otl_name);
                     fprintf(wfp,"otl_idx %d %s\n",  poly_found, otl_name);

                     outline_found = 0;
                  }
                  outline_found = RDC_Graph_Line(wfp, otl_name, outline_found,
                                                      p2.x, p2.y, p2.bulge, p1.x, p1.y);
               }
               fprintf(wfp,"otl_siz %s_%d %d\n", geom_name, poly_found, outline_found);
            }
         }
         break;  // POLYSTRUCT
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
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

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else 
               if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();
   
               poly_found += RDC_WriteOutlineData(wfp, geom_name, &(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, graphic_class);
            }
         }
         break;
      }  // switch

   } // end RDC_WriteOutlineData */

   return poly_found;
}

//--------------------------------------------------------------
static int do_types(FILE *fp, FileStruct *file)
{
   TypeStruct *typ;
   POSITION typPos;
   
   typPos = file->getTypeList().GetHeadPosition();
   while (typPos != NULL)
   {                                         
      typ = file->getTypeList().GetNext(typPos);
      hash_typ_max++;

      fprintf(fp, "typ_nam %s -\n", typ->getName());
      fprintf(fp, "typ_idx %d %s\n", hash_typ_max, typ->getName());
   }

   // also need to write the via "types" ???

   return 1;
}

//--------------------------------------------------------------
static int RDC_WriteCOMPONENTPinData(FILE *wfp, 
                     const char *geomname,
                     CDataList *DataList,
                     double insert_x, double insert_y,
                     double rotation, int mirror,
                     double scale,
                     int embeddedLevel, int insertLayer)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   POSITION pos;
   int      pincnt = 0;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)   continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               pincnt++;
               fprintf(wfp,"pkg_pinx %s*%d %ld\n", geomname, pincnt, cnv_units(point2.x)); 
               fprintf(wfp,"pkg_piny %s*%d %ld\n", geomname, pincnt, cnv_units(point2.y)); 

               fprintf(wfp,"pkg_pin %s*%d %d\n", geomname, pincnt, pincnt); 
               fprintf(wfp,"pkg_pter %s*%d %d\n", geomname, pincnt, pincnt); 

               Attrib *a;
               if (a =  is_attvalue(doc,block->getAttributesRef(), ATT_SMDSHAPE, 1))
                  fprintf(wfp,"pkg_patr %s*%d smd\n", geomname, pincnt);
               else
                  fprintf(wfp,"pkg_patr %s*%d thru\n", geomname, pincnt);
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return pincnt;
} /* end RDC_WriteCOMPONENTPinData */

//--------------------------------------------------------------
static int do_shapes(FILE *fp, FileStruct *file)
{

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      switch (block->getBlockType())
      {
         case BLOCKTYPE_PCBCOMPONENT:     
         {
            int   outlinecnt = 0;
            fprintf(fp,"### geometry %s\n",block->getName());
            hash_pkg_max++;
            fprintf(fp,"pkg_idx %d %s\n",hash_pkg_max, block->getName());
            fprintf(fp,"pkg_nam %s -\n", block->getName());

            Attrib *a;
            if (a =  is_attvalue(doc,block->getAttributesRef(), ATT_SMDSHAPE, 1))
               fprintf(fp,"pkg_tch %s smd\n", block->getName());
            else
               fprintf(fp,"pkg_tch %s thru\n", block->getName());

            int pincnt = RDC_WriteCOMPONENTPinData(fp, block->getName(), &(block->getDataList()), 
               0.0, 0.0, 0.0, 0, 1.0, 0, -1);
            fprintf(fp,"pkg_siz %s %d\n", block->getName(), pincnt);

            // box_idx

            // now all outlines geomname_xxx
            int outline_found = RDC_WriteOutlineData(fp, block->getName(), &(block->getDataList()), 
               0.0, 0.0, 0.0, 0, 1.0, 0, -1, 0 /*GR_CLASS_COMPOUTLINE*/);

            hash_box_max++;
            fprintf(fp,"box_nam %s -\n", block->getName());
            fprintf(fp,"box_idx %d %s\n", hash_box_max, block->getName());
            fprintf(fp,"box_siz %s %d\n", block->getName(), outline_found);
            hash_otl_max += outline_found;
            fprintf(fp,"\n");
         }
         break;
      }
   } // while

   return 1;
}

/****************************************************************************/
/*
*/
static int load_RDCsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   ComponentSMDrule = 0;
   Report_UnusedPins = FALSE;
   VianamePrefix = "V";

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"RDC Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".ComponentSMDrule"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            ComponentSMDrule = atoi(lp);
         }
         else
         if (!STRCMPI(lp,".Report_Unusedpins"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               Report_UnusedPins = TRUE;
         }
         else
         if (!STRCMPI(lp,".VianamePrefix"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            VianamePrefix = lp;
         }
      }
   }

   fclose(fp);
   return 1;
}

/****************************************************************************/
/*
*/
static int get_netnameptr(const char *n)
{
   int   i;

   for (i=0;i<netlistcnt;i++)
   {
      if (!STRCMPI(netlistarray[i]->net_name,n))
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int write_sig_siz(FILE *fp)
{
   int   i;

   for (i=0;i<netlistcnt;i++)
   {
      fprintf(fp,"sig_siz %s %d\n", netlistarray[i]->net_name, netlistarray[i]->sig_siz);
   }
   fprintf(fp,"\n");

   return 1;
}

/****************************************************************************/
/*
*/
static int write_cmp_nam(FILE *fp)
{
   int   i;

   for (i=0;i<complistcnt;i++)
   {
      fprintf(fp,"cmp_nam %s -\n", complistarray[i]->comp_name);
      fprintf(fp,"cmp_idx %d %s\n", i+1, complistarray[i]->comp_name);
   }
   fprintf(fp,"\n");

   return 1;
}

/****************************************************************************/
/*
*/
static int write_cmp_pin(FILE *fp)
{
   int   i;
   CString  cname;

   for (i=0;i<comppinlistcnt;i++)
   {
      RDCCompPinl *cp = comppinlistarray[i];
      cname = complistarray[comppinlistarray[i]->comp_idx-1]->comp_name;

      fprintf(fp,"cmp_pter %s*%d* %d\n", cname, 
         comppinlistarray[i]->pin_idx, comppinlistarray[i]->pin_idx);

      // this pin was initialized from block list, but not updated in netlist.
      if (comppinlistarray[i]->netptr < 0)   continue;

      fprintf(fp,"cmp_sig %s*%d* %s\n", cname, 
         comppinlistarray[i]->pin_idx, 
         netlistarray[comppinlistarray[i]->netptr]->net_name);

      fprintf(fp,"cmp_pin %s*%d* %s\n", cname, 
         comppinlistarray[i]->pin_idx, comppinlistarray[i]->pin_name);

      fprintf(fp,"cmp_pinx %s*%d* %ld\n", cname, comppinlistarray[i]->pin_idx,
         comppinlistarray[i]->pinx);

      fprintf(fp,"cmp_piny %s*%d* %ld\n", cname, comppinlistarray[i]->pin_idx,
         comppinlistarray[i]->piny);
   }
   fprintf(fp,"\n");

   return 1;
}

/****************************************************************************/
/*
*/
static int get_complistptr(const char *c)
{
   int   i;

   for (i=0;i<complistcnt;i++)
   {
      if (!strcmp(complistarray[i]->comp_name,c))
         return i;
   }

   RDCCompl *cc = new RDCCompl;
   complistarray.SetAtGrow(complistcnt,cc);
   cc->comp_name = c;
   cc->pincnt = 0;
   complistcnt++;

   return complistcnt-1;
}

/****************************************************************************/
/*
*/
static int update_comppinarray(const char *compname, const char *pinname)
{
   int   i;
   int   pincnt = 0;
   int   compptr;

   compptr = get_complistptr(compname);

   for (i=0;i<comppinlistcnt;i++)
   {
      if (comppinlistarray[i]->comp_idx == compptr+1)
      {
         if (!strcmp(comppinlistarray[i]->pin_name,pinname))
         {
            return i;
         }
         pincnt++;
      }
   }

   RDCCompPinl *cp = new RDCCompPinl;
   comppinlistarray.SetAtGrow(comppinlistcnt,cp);
   cp->pin_name = pinname;
   cp->pin_idx =  pincnt+1;
   cp->comp_idx = compptr+1;
   cp->netptr = -1;  // not initialized
   comppinlistcnt++;

   return comppinlistcnt-1;
}

//--------------------------------------------------------------
// sig_nam
// sig_idx
// sig_itm
// sig_siz
// sig_cmp
// sig_pin

static int do_signals(FILE *fp, FileStruct *file,CNetList *NetList)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      fprintf(fp,"### start signal %s\n", net->getNetName());

      RDCNetl  *c = new RDCNetl;
      netlistarray.SetAtGrow(netlistcnt,c);
      c->net_name = net->getNetName();
      c->sig_siz = 0;
      netlistcnt++;
      
      // netlist starts with 1, not 0
      fprintf(fp,"sig_nam %s -\n", net->getNetName()); 
      fprintf(fp,"sig_idx %d %s\n",netlistcnt, net->getNetName()); 

      //c->sig_xlt = net->getNetName();

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         c->sig_siz++;

         int cpptr = update_comppinarray(compPin->getRefDes(), compPin->getPinName());
         comppinlistarray[cpptr]->netptr = netlistcnt-1;
         // sig_cmp
         fprintf(fp,"sig_cmp %s*%d* %s\n", net->getNetName(), c->sig_siz, 
            complistarray[comppinlistarray[cpptr]->comp_idx-1]->comp_name);
         // sig_pin
         fprintf(fp,"sig_pin %s*%d* %d\n", net->getNetName(), c->sig_siz, 
            comppinlistarray[cpptr]->pin_idx);
         comppinlistarray[cpptr]->pinx = cnv_units(compPin->getOriginX()); 
         comppinlistarray[cpptr]->piny = cnv_units(compPin->getOriginY());
      }
   }

   if (Report_UnusedPins)
   {
      // here do unused net
      netPos = NetList->GetHeadPosition();
      while (netPos != NULL)
      {
         net = NetList->GetNext(netPos);
         if (!(net->getFlags() & NETFLAG_UNUSEDNET))   continue;

         fprintf(fp,"### pins without a net\n");

         // SaveAttribs(stream, &net->getAttributesRef());
         compPinPos = net->getHeadCompPinPosition();
         while (compPinPos != NULL)
         {
            compPin = net->getNextCompPin(compPinPos);

            int cpptr = update_comppinarray(compPin->getRefDes(), compPin->getPinName());

//          nname.Format("%s_%s",compPin->getRefDes(), compPin->getPinName());
            CString  nname;
            nname.Format("%s_%s",comppinlistarray[cpptr]->comp_idx, compPin->getPinName());
            fprintf(fp,"sig_nam %s -\n", nname); 

            RDCNetl  *c = new RDCNetl;
            netlistarray.SetAtGrow(netlistcnt,c);
            c->net_name = nname;
            //c->sig_xlt = nname;
            c->sig_siz = 1;
            netlistcnt++;

            comppinlistarray[cpptr]->netptr = netlistcnt-1;

            fprintf(fp,"sig_idx %d %s\n",netlistcnt, nname); 

            comppinlistarray[cpptr]->netptr = netlistcnt-1;
            // sig_cmp
            fprintf(fp,"sig_cmp %s*%d* C%d\n", nname, c->sig_siz, 
               comppinlistarray[cpptr]->comp_idx);
            // sig_pin
            fprintf(fp,"sig_pin %s*%d* %d\n", nname, c->sig_siz, 
               comppinlistarray[cpptr]->pin_idx);
            comppinlistarray[cpptr]->pinx = cnv_units(compPin->getOriginX()); 
            comppinlistarray[cpptr]->piny = cnv_units(compPin->getOriginY());
         }
      }
   } // Report_UnusedPins

   fprintf(fp,"### end signal\n\n");

   fprintf(fp,"### now all vias\n\n");
   // here do all vias
   fprintf(fp,"### end vias\n\n");

   write_cmp_nam(fp);

   write_cmp_pin(fp);

   return 1;
}

//-----------------------------------------------------------------------------
// this is done to force the pin sequence from the library and not from the 
// netlist.
static int get_comppins(const char *cname, CDataList *DataList)
{
   DataStruct *np;
   int      pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() == T_INSERT)
      {
         if (np->getInsert()->getInsertType() != INSERTTYPE_PIN)   continue;
         pincnt++;
         CString pinname = np->getInsert()->getRefname();
         int cpptr = update_comppinarray(cname, pinname);
      }
   } 

   return pincnt;
}

//--------------------------------------------------------------
static void RDC_WriteCOMPONENTData(FILE *wfp, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y,
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
            if (np->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)   break;   

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX();
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY();
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            CString compname = np->getInsert()->getRefname();
            Attrib *a;

            int   compptr = get_complistptr(compname);

            fprintf(wfp,"cmp_pkg %s %s\n", compname, block->getName());
            fprintf(wfp,"cmp_otl %s %s\n", compname, block->getName());

            fprintf(wfp,"cmp_tch C%d ", compptr+1);
            if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_SMDSHAPE, 1))
            {
               fprintf(wfp," smd\n");
            }
            else
            {
               fprintf(wfp," thru\n");
            }
            fprintf(wfp,"cmp_xpos %s %ld\n", compname, cnv_units(point2.x));
            fprintf(wfp,"cmp_ypos %s %ld\n", compname, cnv_units(point2.y));
            fprintf(wfp,"cmp_rot %s %1.0lf\n", compname, RadToDeg(block_rot));
            fprintf(wfp,"cmp_sid %s %s\n", compname, 
               (block_mirror)?"bottom":"top");

            if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
            {
               fprintf(wfp,"cmp_typ %s %s\n", compname, get_attvalue_string(doc, a));
            }

            int pincnt = get_comppins(compname, &(block->getDataList())); // get component pins in the order as they are in the database.
            complistarray[compptr]->pincnt = pincnt;

            fprintf(wfp,"cmp_siz %s %d\n", compname, pincnt);
            
            fprintf(wfp,"\n");
         } // case INSERT
         break;
      } // end switch
   } // end RDC_WriteCOMPONENTData */
}

//--------------------------------------------------------------
static int do_components(FILE *fp, FileStruct *file)
{

   RDC_WriteCOMPONENTData(fp, file, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   return 1;
}

//--------------------------------------------------------------
static void RDC_WriteVIAData(FILE *wfp, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2   m;
   Point2   point2;
   DataStruct *np;
   int      viacnt = 0;

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
            if (np->getInsert()->getInsertType() != INSERTTYPE_VIA)   break;   

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX();
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY();
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            //CString vianame = np->getInsert()->refname;
            CString netname = "";
            Attrib *a;
   
            ++viacnt;

            if (a =  is_attvalue(doc, np->getAttributesRef(),ATT_NETNAME, 1))
            {
               netname = get_attvalue_string(doc, a);
            }
         
            if (netname == "")
               break;

            CString  vianame;
            vianame.Format("%s%d",VianamePrefix, viacnt);
            int compptr = get_complistptr(vianame);
            int netptr = get_netnameptr(netname);

            netlistarray[netptr]->sig_siz += 1;
            fprintf(wfp,"sig_cmp %s*%d* %s\n", netname, netlistarray[netptr]->sig_siz, vianame);
            fprintf(wfp,"sig_pin %s*%d* 1\n", netname);
            fprintf(wfp,"cmp_nam %s -\n", vianame);
            fprintf(wfp,"cmp_typ %s %s\n", vianame, block->getName());
            fprintf(wfp,"cmp_idx %d %s\n", compptr+1, vianame);

            fprintf(wfp,"cmp_xpos %s %ld\n", vianame, cnv_units(point2.x));
            fprintf(wfp,"cmp_ypos %s %ld\n", vianame, cnv_units(point2.y));
            fprintf(wfp,"cmp_rot %s %1.0lf\n", vianame, RadToDeg(block_rot));
            fprintf(wfp,"cmp_sid %s top\n", vianame);
            fprintf(wfp,"cmp_siz %s 1\n", vianame);

            fprintf(wfp,"cmp_pter %s*1* 1\n", vianame);

            fprintf(wfp,"cmp_pin %s*1* 1\n", vianame);
            fprintf(wfp,"cmp_sig %s*1* %s\n", vianame, netname);
            fprintf(wfp,"cmp_pinx %s*1* %ld\n", vianame, cnv_units(point2.x));
            fprintf(wfp,"cmp_piny %s*1* %ld\n", vianame, cnv_units(point2.y));
            fprintf(wfp,"\n");
         } // case INSERT
         break;
      } // end switch
   } // end RDC_WriteVIAData */
}

//--------------------------------------------------------------
static int do_vias(FILE *fp, FileStruct *file)
{

   RDC_WriteVIAData(fp, file, &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   return 1;
}

//--------------------------------------------------------------
void RDC_WriteFiles(const char *filename, CCEtoODBDoc *Doc,FormatStruct *format,
                       int page_units, double UnitsFactor, const char *includeFile)
{
   FILE *wfp;
   FileStruct *file;

   int   pcb_found = FALSE;
   doc = Doc;
   display_error = FALSE;

   unitsFactor = Units_Factor(page_units, UNIT_TNMETER); // rdc units to page units

   netlistarray.SetSize(100,100);
   netlistcnt = 0;

   complistarray.SetSize(100,100);
   complistcnt = 0;

   comppinlistarray.SetSize(100,100);
   comppinlistcnt = 0;

   CString logFile = GetLogfilePath("RDC.LOG");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString settingsFile( getApp().getExportSettingsFilePath("RDC.out") );
   load_RDCsettings(settingsFile);

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %-70s",filename);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }


   hash_otl_max = 0;
   hash_sig_max = 0;
   hash_cmp_max = 0;
   hash_pkg_max = 0;
   hash_typ_max = 0;
   hash_uut_max = 0;
   hash_box_max = 0;

   fprintf(wfp,"###\n");
   fprintf(wfp,"### Mentor Graphics, RDC File\n");
   fprintf(wfp,"### RDCVersion 1.0\n");
   fprintf(wfp,"### Created by %s\n", getApp().getCamCadTitle());
   CTime t;
   t = t.GetCurrentTime();
   fprintf(wfp,"### %s\n",t.Format("Date :%A, %B %d, %Y at %H:%M:%S"));

   fprintf(wfp,"###\n\n");

   update_smdrule_geometries(doc, ComponentSMDrule);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
   
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         pcb_found = TRUE;


         fprintf(wfp,"### Start of PCB Design (%s)\n\n\n", file->getName());

         update_smdrule_components(doc, file, ComponentSMDrule);

         generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

         // do first so that compindex is in the sequence of database
         fprintf(wfp,"### component data block\n");   
         do_components(wfp,file);
         fprintf(wfp,"\n");

         fprintf(wfp,"### signal data block\n");
         do_signals(wfp, file, &file->getNetList());
         fprintf(wfp,"\n");

         fprintf(wfp,"### via data block\n");
         do_vias(wfp,file);
         fprintf(wfp,"\n");

         // here write sig_siz, must be written here because vias update the
         // size of the signal
         write_sig_siz(wfp);

         // otl_ area
         do_shapes(wfp,file);

         // typ_ area
         do_types(wfp, file);

         hash_sig_max = netlistcnt;
         hash_cmp_max = complistcnt;
         // allows to do only one PCB files
         break;
      } // if pcb file
   }
       
   fprintf(wfp,"###Dump Hash_aux\n");
   fprintf(wfp,"hash_aux otl_max %d\n", hash_otl_max);
   fprintf(wfp,"hash_aux sig_max %d\n", hash_sig_max);
   fprintf(wfp,"hash_aux cmp_max %d\n", hash_cmp_max);
   fprintf(wfp,"hash_aux pkg_max %d\n", hash_pkg_max);
   fprintf(wfp,"hash_aux typ_max %d\n", hash_typ_max);
   fprintf(wfp,"hash_aux uut_max %d\n", hash_uut_max);
   fprintf(wfp,"hash_aux box_max %d\n", hash_box_max);
   fprintf(wfp,"\n");

   fprintf(wfp,"### End of PCB Design (%s)\n\n\n", file->getName());

   if (!pcb_found)
   {
      MessageBox(NULL, "No PCB file found !","RDC Output", MB_OK | MB_ICONHAND);
      return;
   }

   fclose(flog);
   fclose(wfp);

   if (display_error)
      Logreader(logFile);

   int i;
   for (i=0;i<netlistcnt;i++)
   {
      delete netlistarray[i];
   }
   netlistarray.RemoveAll();

   for (i=0;i<complistcnt;i++)
   {
      delete complistarray[i];
   }
   complistarray.RemoveAll();

   for (i=0;i<comppinlistcnt;i++)
   {
      delete comppinlistarray[i];
   }
   comppinlistarray.RemoveAll();

   return;
}



/****************************************************************************/
/*
   end rdc_out.CPP
*/
/****************************************************************************/

