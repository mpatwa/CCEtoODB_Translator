// $Header: /CAMCAD/5.0/read_wrt/Mneut_o.cpp 31    6/17/07 8:58p Kurt Van Ness $
 
/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1998. All Rights Reserved.
*/

/****************************************************************************/
/*
Thank You,
Dean Faber
AE, Router Solutions, Inc.
Ph: (949) 721-1017
Fx: (949) 721-1019
-----Original Message-----
From: Michael Milner <michael_milner@mentorg.com>
To: dean@rsi-inc.com <dean@rsi-inc.com>
Cc: michaelm <michael_milner@mentorg.com>; kim_owen@mentorg.com
<kim_owen@mentorg.com>
Date: Wednesday, September 02, 1998 12:38 PM
Subject: Setting up split planes on Boardstation

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Tan Id:              10276
Product:             Board Station
Platform:
Operating System:
Created Date:        16 Apr 96
Modified Date:       16 Apr 96
Initial Version:     8.X
Final Version:       x.X
Short Desc:          Methodology for generating split planes for analysis


===============================================
Problem Text:
-----------------------------------------------


Board Station accepts several different ways of configuring split power
planes. What methodology will ensure best results during auto-routing,
testpoint generation, high speed analysis, signal integrity analysis,
and post-process artwork checking?



===============================================
Solution Text:
-----------------------------------------------


The system accepts several different ways of configuring split planes. The
following is the Mentor Graphics supported methodology which ensures best
results during auto-routing, testpoint generation, high speed analysis,
signal integrity analysis, and post-process artwork checking.

Each individual net which appears on a power plane layer must be specified
using the power_net_names attribute in the board geometry. This can be
accomplished by using the Change Geometry function for the board geometry
in Librarian. If the same net is used many times, then specify it the same
number of times.

Organize the power net names in the power net statement grouped in the order
they will use power_n layers separated by commas which indicate layer
separations. Such as:

vcc /phy/crm/syvcc gnd_3, gnd_2 ground gnd_3, vcc3_3v gnd_3, ground gnd_3


Net 'vcc' maps to power_1, net '/phy/crm/syvcc' maps to power_2, net 'gnd_3'
maps to power_3, etc.

This is done all on one line in the create/change board part pop-up (even
though the system will show a second line leave it blank).

The viewing layer is the first power plane layer for each group as defined in
the artwork order. Be sure the power net name used on multiple split power
planes is not defined as the viewing layer of a split power plane more than
once in the artwork order. This ensures that each power fill generates
successfully.

In the example power net names list above, the second net names group (gnd_2
ground gnd_3) contains ground, and the fourth net names group (ground gnd_3)
also contains ground. Net ground will not be associated with the first or
viewing layer more than once. This allows different power net names to be the
viewing layers and results in all the power fills and artwork generating correctly.

Next, use the 'Physical Layers>Set Up Physical Layers'  menu pick to create as
many power_n layers as there are net names specified in the power net names
statement on the board part.

Then create the artwork order part the example as shown below illustrates.
Take care to organize the power layers in order. If you need to add a layer
later, redo the layers stackup so they stay in order.

  1 signal_1, pad_1
  2 power_1, power_2, power_3
  3 power_4, power_5, power_6
  4 power_7, power_8
  5 power_9, power_10
  6 signal_2, pad_2

The following table illustrates an example logical layer/net name/physical
layer matrix;


     Logical   Net Name         Physical
   -
1 |  signal_1,pad_1             phys1
   -
  |  power_1*  VCC              phys2_vcc
2 |  power_2   /phy/crm/syvcc   phys2_syvcc
  |  power_3   gnd_3            phys2_gnd3
   -
  |  power_4*  gnd2             phys3_gnd2
3 |  power_5   ground           phys3_ground
  |  power_6   gnd_3            phys3_gnd3
   -
4 |  power_7*  vcc3_3v          phys4_vcc3_3v
  |  power_8   gnd3             phys4_gnd3
   -
5 |  power_9*  ground           phys5_ground
  |  power_10  gnd_3            phys5_gnd3
   -
6 |  signal_2,pad_2             phys6
   -
           *viewing layer for each group

Now add the split power planes in Layout or Fablink as described in the
'Fablink Users Manual V8.4_3' beginning on page 4-176.

Be aware that in some versions of Board Station before 8.3x there is a
bug with power planes which are completely enclosed within larger
planes.

     ---------------------
    |         B           |
    |                     |
    |     ----------      |
    |    |          |     |
    |    |    A     |     |
    |    |          |     |
    |     ----------      |
    |                     |
     ---------------------

In the above example a power fill of net A is completely enclosed by another
power fill of net B; if the power fill of net A contains pins and/or vias
belonging to net B, then those pins and/or vias are not cleared but
shorted to net A in the artwork.

The workaround for this problem is to create the power fills so that one fill
does not fully contain another fill inside of it. This requires either
digitizing a gap when creating theoutside power fill or adding a power fill
cutout to the existing outside power fill. For example;

     -------   -----------
    |       | |  B        |
    |       | |           |
    |     ----------      |
    |    |          |     |
    |    |    A     |     |
    |    |          |     |
    |     ----------      |
    |                     |
     ---------------------

Power fill checking;

Menu pick 'Check>Power Fills on Physical Layers' in Layout or Fablink must be
performed after creating split power planes or after performing any
modifications to a design after the power fills have been added to the design.
This restructures the power fills if necessary in order to eliminate any
potential shorts within the power fill areas, and reportson any pins and vias
disconnected from the power fills.  The system does NOT automatically
restructure power fills if changes were made to the layout after the power
fills were created. Some examples of these changes are adding new vias or
moving components. A separate 'Check>Power Fills on Physical Layers' command
must be issued for each power plane viewing layer in the stackup.

Check power fills by connectivity does not restructure the power fills
and only reports on pins and vias not contained within their respective
power fill

TechNotes 7029 8227 where used in the creation of this paper.

end.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mike Milner               michael_milner@mentorg.com
SDD Customer Support                  1-800-547-4303
Mentor Graphics Corp.         http://www.mentorg.com

*/

#include "stdafx.h"
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "graph.h"
#include <math.h>
#include "outline.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  SMALL_DELTA    0.001    // Smallest unit

extern CProgressDlg  *progress;
//extern char          *layertypes[];

static CCEtoODBDoc    *doc;
static double        unitsFactor;

typedef CArray<Point2, Point2&> CPolyArray;

static int           display_error;
static FILE          *flog;

static FileStruct    *cur_file;

static   long        linecnt = 0;

static   CPolyArray  polyarray;
static   int         polycnt;

static   int         ComponentSMDrule;    // 0 = take existing ATT_SMD
                                          // 1 = if all pins of comp are marked as SMD
                                          // 2 = if most pins of comp are marked as SMD
                                          // 3 = if one pin of comp are marked as SMD

static   int         SHOWOUTLINE = FALSE; // set in neutral.out

//-----------------------------------------------------------------------------//
static char *clean_text(const char *t)
{
   static   char  tmp[255];
   unsigned int   i, tcnt = 0;

   for (i=0;i<strlen(t);i++)
   {
      if (t[i] == '"')
         tmp[tcnt++] = '\\';
      tmp[tcnt++] = t[i];     
   }
   tmp[tcnt] = '\0';
   return tmp;
}

//-----------------------------------------------------------------------------
static void write_attributes(FILE *fp,CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 1))     continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 1))     continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 1))   continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 1))continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 1))    continue;
      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 1))  continue;

      int   value_keyword = FALSE;

      if (keyword == doc->IsKeyWord(ATT_VALUE, 1))    
         value_keyword = TRUE;

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp,"{attribute \"%s\" \"%d\"}\n", 
            doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp,"{attribute \"%s\" \"%lg\"}\n", 
            doc->getKeyWordArray()[keyword]->out, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            char *tok, *temp = STRDUP(attrib->getStringValue());
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  if (value_keyword)
                     fprintf(fp,"{value \"%s\"}\n", tok);   
                  else
                     fprintf(fp,"{attribute \"%s\" \"%s\"}\n", 
                        doc->getKeyWordArray()[keyword]->out,tok);  // multiple values are delimited by \n
               }
               else
                  fprintf(fp,"{attribute \"%s\" \"\"}\n", 
                     doc->getKeyWordArray()[keyword]->out);
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   return;
}

//-----------------------------------------------------------------------------
static void write_geomattributes(FILE *fp,CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;
   CString  key;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 1))     continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 1))    continue;
      if (keyword == doc->IsKeyWord(ATT_COMPHEIGHT, 1))     continue;

      CString l = get_attvalue_string(doc, attrib);

      key = doc->getKeyWordArray()[keyword]->out;
      fprintf(fp,"G_ATTR '%s' '%s'\n",key, l);
      
/*
      switch (attrib->getValueType())
      {
         case VT_INTEGER:
            fprintf(fp,"%s{attribute \"%s\" \"%d\"}\n", 
               ident, doc->KeyWordArray[keyword]->out, attrib->getIntValue());
         break;
         case VT_UNIT_DOUBLE:
         case VT_DOUBLE:
            fprintf(fp,"%s{attribute \"%s\" \"%lg\"}\n", 
               ident, doc->KeyWordArray[keyword]->out, attrib->getDoubleValue());
         break;
         case VT_STRING:
         {
            char *tok, *temp = STRDUP(attrib->getStringValue());
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  if (value_keyword)
                     fprintf(fp,"%s{value \"%s\"}\n", ident, tok);   
                  else
                     fprintf(fp,"%s{attribute \"%s\" \"%s\"}\n", 
                        ident, doc->KeyWordArray[keyword]->out,tok); // multiple values are delimited by \n
               }
               else
                  fprintf(fp,"%s{attribute \"%s\" \"\"}\n", 
                     ident, doc->KeyWordArray[keyword]->out);
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
*/
   }

   return;
}

//-----------------------------------------------------------------------------
static void write_compattributes(FILE *fp,CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 1))     continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 1))     continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 1))   continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 1))continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 1))    continue;
      if (keyword == doc->IsKeyWord("PARTNUMBER", 2))    continue;
      if (keyword == doc->IsKeyWord("SYMBOL", 2))        continue;

      int   value_keyword = FALSE;
      if (keyword == doc->IsKeyWord(ATT_VALUE, 1))    
         value_keyword = TRUE;

      // do not write inheritate attributes, which are already done in G_ATTR
      if (attrib->isInherited())       continue;

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp,"C_PROP (%s,\"%d\")\n",
            doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp,"C_PROP (%s,\"%lg\")\n",
            doc->getKeyWordArray()[keyword]->out, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            char *tok, *temp = STRDUP(attrib->getStringValue());
            
            tok = strtok(temp,"\n");

            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  fprintf(fp,"C_PROP (%s,\"%s\")\n",
                     doc->getKeyWordArray()[keyword]->out,tok);  // multiple values are delimited by \n
               }
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   return;
}

//-----------------------------------------------------------------------------
static void write_netattributes(FILE *fp,CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      if (keyword == doc->IsKeyWord(ATT_NETNAME, 1))     continue;
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 1))     continue;
      if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 1))   continue;
      if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 1))continue;
      if (keyword == doc->IsKeyWord(ATT_SMDSHAPE, 1))    continue;
      if (keyword == doc->IsKeyWord("PARTNUMBER", 2))    continue;
      if (keyword == doc->IsKeyWord("SYMBOL", 2))        continue;

      int   value_keyword = FALSE;

      if (keyword == doc->IsKeyWord(ATT_VALUE, 1))    
         value_keyword = TRUE;

      switch (attrib->getValueType())
      {
      case VT_INTEGER:
         fprintf(fp,"N_PROP (%s,\"%d\")\n",
            doc->getKeyWordArray()[keyword]->out, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         fprintf(fp,"N_PROP (%s,\"%lg\")\n",
            doc->getKeyWordArray()[keyword]->out, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            char *tok, *temp = STRDUP(attrib->getStringValue());
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  fprintf(fp,"N_PROP (%s,\"%s\")\n",
                     doc->getKeyWordArray()[keyword]->out,tok);  // multiple values are delimited by \n
               }
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   return;
}

//--------------------------------------------------------------
static int do_typelist(FILE *fp, CTypeList *TypeList, const char *type_name)
{
   TypeStruct *typ;
   POSITION typPos;
   
   //fprintf(fp,"$DEVICES\n");

   typPos = TypeList->GetHeadPosition();
   while (typPos != NULL)
   {                                         
      typ = TypeList->GetNext(typPos);

      if (strcmp(typ->getName(),type_name)) continue;

      //fprintf(fp, "DEVICE %s\n", typ->Name);
      write_attributes(fp, typ->getAttributesRef());
   }
   // fprintf(fp,"$ENDDEVICES\n\n");

   return 1;
}

//--------------------------------------------------------------
static void NEUT_WriteVIAData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, const char *netname)
{
   Mat2x2 m;
   DataStruct *np;
   int      startlayer = 1;
   int      endlayer = 8;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)   
            {
               Attrib* attrib =  is_attvalue(doc, np->getAttributesRef(),ATT_NETNAME, 1);

               if (attrib != NULL)
               {
                  if (attrib->getValueType() == VT_STRING)
                  {
                     //CString l = attrib->getStringValue();
                     if (strcmp(attrib->getStringValue(),netname))
                        continue;

                     Point2   point2;
                     // EXPLODEALL - call DoWriteData() recursively to write embedded entities
                     point2.x = np->getInsert()->getOriginX() * scale;

                     if (mirror)
                        point2.x = -point2.x;

                     point2.y = np->getInsert()->getOriginY() * scale;
                     TransPoint2(&point2, 1, &m, insert_x, insert_y);

                     // insert if mirror is either global mirror or block_mirror, but not if both.
                     //int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
                     //double block_rot = rotation + np->getInsert()->getAngle();

                     BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
                     fprintf(wfp, "N_VIA %lg %lg %s   %d  %d\n",
                        point2.x, point2.y, block->getName(), startlayer, endlayer);
                     // attributes
                     write_attributes(wfp, np->getAttributesRef());

                  }
               }  
            }  // if not written
         break;
      } // end switch
   } // end  */
   return;
}

//--------------------------------------------------------------
static int do_signals(FILE *fp, FileStruct *file,CNetList *NetList, 
                      double insert_x, double insert_y, double rot)
{
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   Mat2x2 m;
   Point2 point2;

   RotMat2(&m, rot);

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      if (net->getFlags() & NETFLAG_UNUSEDNET)   continue;

      fprintf(fp, "NET %s\n", net->getNetName());
      write_netattributes(fp, net->getAttributesRef());

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         CString  padstackname;

         point2.x = compPin->getOriginX();
         point2.y = compPin->getOriginY();
         //double pinrot = compPin->getRotationRadians();
         //int    padmirror = compPin->getMirror();
         
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         if (compPin->getPadstackBlockNumber() > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
            padstackname = block->getName();
         }

         fprintf(fp,"N_PIN %s-%s %lg %lg %s   8\n",
               compPin->getRefDes(), compPin->getPinName(), point2.x, point2.y, padstackname);
      }
      // here write vias 
      // order by signal name
      NEUT_WriteVIAData(fp,&(file->getBlock()->getDataList()), 
         insert_x, insert_y, rot, 0, file->getScale() * unitsFactor, 0, -1, net->getNetName());
   }

   return 1;
}

//--------------------------------------------------------------
static void NEUT_WriteCOMPONENTPinData(FILE *wfp, 
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
               // here pin found.
               CString pinname = np->getInsert()->getRefname();
               Attrib *a;

               fprintf(wfp,"G_PIN %s %lg %lg %s", pinname, point2.x, point2.y, block->getName());

               if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 0))
               {
                  fprintf(wfp," Surf\n");
               }
               else
               if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_BLINDSHAPE, 0))
               {
                  double drill = get_drill_from_block_no_scale(doc, block);
                  fprintf(wfp," Bur %lg\n", drill);
               }
               else
               {
                  double drill = get_drill_from_block_no_scale(doc, block);
                  fprintf(wfp," Thru %lg\n", drill);
               }
               // attributes
               //write_attributes(wfp, np->getAttributesRef());
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

} /* end NEUT_WriteCOMPONENTPinData */

/*****************************************************************************/
/*
*/
static int write_easyoutline(FILE *wfp, CPntList *pp, const char *graph_type, int cnt)
{
   CPnt *p;

   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;
   POSITION pos = pp->GetHeadPosition();
   while (pos != NULL)
   {
      p = pp->GetNext(pos);

      if (fabs(p->bulge) > BULGE_THRESHOLD)
      {
         int      i, ppolycnt;
         Point2   ppoly[255];
         double   cx,cy,r,sa;
         CPnt     *p2;
         double   da = atan(p->bulge) * 4;

         // GetNext advance to next, GetAt does not.
         if (pos == NULL)
         {
            // ErrorMessage("Poly Bulge data error !");
            fprintf(flog,"Bulge error in write_outline -> there is a bulge with no following entity\n");
            polyarray.RemoveAll();
            polycnt = 0;
            return 0;
         }
         else
            p2 = pp->GetAt(pos);
                     
         ArcPoint2Angle(p->x,p->y,p2->x,p2->y,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         ppolycnt = 255;
         // start center
         ArcPoly2(p->x,p->y,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(5));  // 5 degree
         for (i=0;i<ppolycnt;i++)
         {
            polyarray.SetAtGrow(polycnt,ppoly[i]);
            polycnt++;
         }
      }
      else
      {
         Point2 px;
         px.x = p->x;
         px.y = p->y;
         px.bulge = p->bulge;
         polyarray.SetAtGrow(polycnt, px);
         polycnt++;
      }
   }

   int done = FALSE,i;

   while (!done)
   {
      done = TRUE;
      for (i=1;i<polycnt;i++)
      {
         Point2 p1 = polyarray.ElementAt(i-1);
         Point2 p2 = polyarray.ElementAt(i);
         if (fabs(p1.x - p2.x) < SMALLNUMBER &&  
             fabs(p1.y - p2.y) < SMALLNUMBER)
         {
               polyarray.RemoveAt(i);
               done = FALSE;
               polycnt--;
               break;
         }
      }
   }

   for (i=0;i<polycnt;i++)
   {
      Point2 p;
      p = polyarray.ElementAt(i);
      if (cnt > 70)
         cnt = fprintf(wfp," -\n");
      cnt +=fprintf(wfp," %lg %lg",p.x, p.y);
   }
   fprintf(wfp,"\n");

// this is C++ debug flag - never have debug_outline running without the compiler debug.
   if (SHOWOUTLINE)
   {
      doc->PrepareAddEntity(cur_file);       
      int l = Graph_Level("DEBUG","",0);
      DataStruct *d = Graph_PolyStruct(l,0L, FALSE);  // poly is always with 0

      Graph_Poly(NULL,0, 0,0, 0);

      for (i=0;i<polycnt;i++)
      {
         Point2 p;
         p = polyarray.ElementAt(i);
         Graph_Vertex(p.x, p.y, p.bulge);    // p1 coords
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   return 1;
}

//--------------------------------------------------------------
static int NEUT_WriteOutlineData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, long graphic_class)
{
   Mat2x2   m;
   DataStruct *np;
   Point2   point2;
   int      layer;
   int      outline_found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (graphic_class && (np->getGraphicClass() != graphic_class))
         continue;

      if (np->getDataType() == T_TEXT)    continue;
      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         outline_found++;
      }
      else
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

            outline_found += NEUT_WriteOutlineData(&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, graphic_class);
         }
      }
   } // end NEUT_WriteOutlineData */

   return outline_found;
}

//--------------------------------------------------------------
static void NEUT_WriteCP(FILE *fp,  FileStruct *f, const char *comp)
{
   // otherwise add a NC net. 
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   CString  netname;

   netPos = f->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = f->getNetList().GetNext(netPos);
      netname = net->getNetName();

      if (net->getFlags() & NETFLAG_UNUSEDNET)   
         netname = "$NONE$";

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         // here comp/pin found = all done
         if (!strcmp(compPin->getRefDes(),comp)) 
         {
            CString  padshape;
            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
            if (block)
               padshape = block->getName();
            else
            {
               padshape = "NULLSHAPE";
               fprintf(flog,"Pin [%s] [%s] has no shape\n",compPin->getRefDes(), compPin->getPinName());
               display_error++;
            }

            fprintf(fp,"C_PIN %s-%s %lg %lg 21  2  0 %s %s\n",
               compPin->getRefDes(), compPin->getPinName(), compPin->getOriginX(), compPin->getOriginY(), padshape, netname);
         }
      }
   }
   return;
}

//--------------------------------------------------------------
static void NEUT_WriteCOMPONENTData(FILE *wfp, FileStruct *file,
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
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            CString compname = np->getInsert()->getRefname();
            Attrib *a;
/*
COMP C1 5227991.. CAP_TANTALUM MED_pol1206  1.93 3.48 2   0
C_PROP (INSTPAR,"1uF") (MATERIAL,"Tantalum") (REFLOC,"IN,0.05,-0.075,0,CC,0.04,0.04,0.008,std,1") (MGC_CAT,"capacitors") (VOLTAGE,"16V")
C_PROP (MODEL,"CAP_TANT_SCHEM") (MGC_REFINE_LEVEL,"physical") (MGC_LIB,"MGC Released") (MGC_INDEX,"5227991..") (INSTPARTOL,"20%")
C_PROP (DESC,"CAP 1UF 20% 16V TANT SMD 7343") (NTOL,"NTOL") (PART_TYPE,"Capacitor")
C_PIN C1-1 1.93 3.48  8  2  0 surf_rtg_80x60 +5VRF
C_PIN C1-2 1.82 3.48  8  2  0 surf_rtg_80x60 GND_R
*/
            CString  partnumber = "no_partnumber";
            CString  symbolname = "no_symbol";

            if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
               partnumber = get_attvalue_string(doc, a);
            else
            if (a =  is_attvalue(doc, np->getAttributesRef(), "PARTNUMBER", 2))
               partnumber = get_attvalue_string(doc, a);

            if (a =  is_attvalue(doc, np->getAttributesRef(), "SYMBOL", 2))
               symbolname = get_attvalue_string(doc, a);
            else
            {
               // look in file->TypeList for a symbol attribute.
               // check if there is a component height at a type for this pattern.
               TypeStruct *typ;
               POSITION typPos;

               typPos = file->getTypeList().GetHeadPosition();
               while (typPos != NULL)
               {                                         
                  typ = file->getTypeList().GetNext(typPos);

                  if (typ->getBlockNumber() == block->getBlockNumber())
                  {
                     if (a =  is_attvalue(doc, typ->getAttributesRef(), "SYMBOL", 2))
                     {
                        symbolname = get_attvalue_string(doc, a);
                        break;
                     }
                  }
               }
            }

            fprintf(wfp,"COMP %s %s %s %s", compname, partnumber, symbolname, block->getName());

            fprintf(wfp," %lg %lg",point2.x, point2.y);
            fprintf(wfp," %s",(block_mirror)?"2":"1");
            fprintf(wfp," %lg",RadToDeg(block_rot));
            fprintf(wfp,"\n");

            // attributes
            write_compattributes(wfp, np->getAttributesRef());

            NEUT_WriteCP(wfp, file, compname);

         } // case INSERT
         break;
      } // end switch
   } // end NEUT_WriteCOMPONENTData */
}

//--------------------------------------------------------------
static int do_components(FILE *fp, FileStruct *file, double xoffset, double yoffset, double rot)
{
   
   fprintf(fp,"#############################################\n");
   fprintf(fp,"###Component Information\n");
   fprintf(fp,"#############################################\n");

   NEUT_WriteCOMPONENTData(fp, file, &(file->getBlock()->getDataList()), 
         xoffset, yoffset, rot, 0, file->getScale() * unitsFactor, 0, -1);

   fprintf(fp,"#############################################\n");
   fprintf(fp,"###End of Component Information\n");
   fprintf(fp,"#############################################\n");

   return 1;
}

//--------------------------------------------------------------
static int do_shapes(FILE *fp)
{
   // do not write outline if in geometry mode. 
   int      showoutline = SHOWOUTLINE;
   SHOWOUTLINE = FALSE;
   double   accuracy = get_accuracy(doc);
   
   fprintf(fp,"#############################################\n");
   fprintf(fp,"###Geometry Information\n");
   fprintf(fp,"#############################################\n");

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      switch (block->getBlockType())
      {
         case BLOCKTYPE_PCBCOMPONENT:     
         {
            fprintf(fp,"GEOM %s\n", block->getName());

            Attrib *a;
            if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
               fprintf(fp,"G_ATTR 'COMPONENT_LAYOUT_TYPE' 'surface'\n");

            if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_COMPHEIGHT, 1))
            {
               fprintf(fp,"G_ATTR 'COMPONENT_HEIGHT' '' %lg\n", 
                        atof(get_attvalue_string(doc, a)) * unitsFactor);
            }

            // attributes
            write_geomattributes(fp, block->getAttributesRef());
                              
            Outline_Start(doc);

            int outline_found = NEUT_WriteOutlineData(&(block->getDataList()), 
               0.0, 0.0, 0.0, 0, 1.0, 0, -1,GR_CLASS_COMPOUTLINE);

            if (!outline_found)
            {
               outline_found = NEUT_WriteOutlineData(&(block->getDataList()), 
                  0.0, 0.0, 0.0, 0, 1.0, 0, -1,0);
            }

            if (!outline_found)
            {
               fprintf(flog,"Did not find a Component Outline for [%s].\n", block->getName());
               display_error++;
            }
            else
            {
               int cnt = fprintf(fp,"G_ATTR 'COMPONENT_PLACEMENT_OUTLINE' ''");

               int returnCode;
               if (!write_easyoutline(fp, Outline_GetOutline(&returnCode, accuracy),"component outline", cnt))
               {
                  fprintf(flog, "Error in easy outline for [%s]\n", block->getName());
                  display_error++;
               }
            }

            Outline_FreeResults();

            NEUT_WriteCOMPONENTPinData(fp, &(block->getDataList()),
                  0.0, 0.0, 0.0, 0, 1.0, 0, -1);

         }
         break;
      }
   } // while

   fprintf(fp,"#############################################\n");
   fprintf(fp,"###End Geometry Information\n");
   fprintf(fp,"#############################################\n");

   // reset
   SHOWOUTLINE = showoutline;

   return 1;
}

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
static int NEUT_WritePADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, double *drill)
{
   DataStruct *np;
   int layer;
   int typ = 0;   
   Mat2x2 m;


   *drill = 0;
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
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            Point2   point2;
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
            point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);
   
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
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);
/*
               fprintf(fp,"%s{padshape \"%s\"\n",ident,block->name);
               plusident();
                                                                            
               fprintf(fp,"%s{layer \"%s\"}\n",ident,layertypes[doc->LayerArray[layer]->layertype]);

               LayerStruct *l = doc->FindLayer(layer);
               if (l->getNeverMirror())          
                  fprintf(fp,"%s{layerusage \"NEVERMIRROR\"}\n",ident);
               else
               if (l->getMirrorOnly())           
                  fprintf(fp,"%s{layerusage \"MIRRORONLY\"}\n",ident);

               fprintf(fp,"%s{shape \"%s\"}\n",ident,shapes[block->getShape()]);
               fprintf(fp,"%s{loc %lf %lf}\n",ident,point2.x, point2.y);
               fprintf(fp,"%s{offset %lf %lf}\n",ident,block->getXoffset(), block->getYoffset());
               fprintf(fp,"%s{size %lf %lf}\n",ident,block->getSizeA(), block->getSizeB());

               minusident();
               fprintf(fp,"%send padshape}\n", ident);
*/
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
} /* end NEUT_WritePadstackData */

//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
static int NEUT_GetPADSTACKData(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, double *drill)
{
   DataStruct *np;
   int layer;
   int typ = 0;   
   Mat2x2 m;


   *drill = 0;
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
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            Point2   point2;
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
            point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);
   
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
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               LayerStruct *l = doc->FindLayer(layer);
               if (l->getNeverMirror())          
               {
                  typ |= 8;
               }
               else
               if (l->getMirrorOnly())           
               {
                  typ |= 16;
               }
               else
               {
                  switch (doc->getLayerArray()[layer]->getLayerType())
                  {
                     case  LAYTYPE_PAD_TOP:
                     case  LAYTYPE_SIGNAL_TOP:
                        typ |= 1;
                     break;
                     case  LAYTYPE_PAD_BOTTOM:
                     case  LAYTYPE_SIGNAL_BOT:
                        typ |= 2;
                     break;
                     case  LAYTYPE_PAD_INNER:
                     case  LAYTYPE_SIGNAL_INNER:
                        typ |= 4;
                     break;
                     case  LAYTYPE_PAD_ALL:
                     case  LAYTYPE_SIGNAL_ALL:
                        typ |= 7;
                     break;
                  }
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
} /* end NEUT_GetPadstackData */

//--------------------------------------------------------------
static int do_padstacks(FILE *fp, FileStruct *file)
{

   fprintf(fp,"#############################################\n");
   fprintf(fp,"###Pad Information\n");
   fprintf(fp,"#############################################\n");

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)         continue;
      
      // create necessary aperture blocks
      if (block->getFlags() & BL_APERTURE)   continue;
      if (block->getFlags() & BL_BLOCK_APERTURE)   continue;
   
      if (!(block->getFlags() & BL_FILE) || 
           (block->getFlags() & BL_FILE && doc->is_file_inserted(block->getBlockNumber()))) // fileblock in fileblock is now allowed.
      {
         switch (block->getBlockType())
         {
            case BLOCKTYPE_PADSTACK:  
            {
               fprintf(fp,"PAD via/pad %s smd/thru/bur\n", block->getName());

               double drill;
               Attrib *a;
               if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1))
                  fprintf(fp,"technology \"SMD\"\n");
               else
               if (a =  is_attvalue(doc, block->getAttributesRef(), ATT_BLINDSHAPE, 1))
                  fprintf(fp,"technology \"BLIND\"\n");
               else
               {
                  // here make sure the pad is on copper layers
                  int typ = NEUT_GetPADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1, &drill);
/*
                  switch (typ)
                  {
                     case 1:
                        fprintf(fp,"%s{technology \"TOP\"}\n",ident);
                     break;
                     case 2:
                        fprintf(fp,"%s{technology \"BOTTOM\"}\n",ident);
                     break;
                     case 3:
                        fprintf(fp,"%s{technology \"OUTER\"}\n",ident);
                     break;
                     case 4:
                        fprintf(fp,"%s{technology \"INNER\"}\n",ident);
                     break;
                     case 7:
                        fprintf(fp,"%s{technology \"THRU\"}\n",ident);
                     break;
                     case 8:
                        fprintf(fp,"%s{technology \"CONDITIONAL_TOP\"}\n",ident);
                     break;
                     case 16:
                        fprintf(fp,"%s{technology \"CONDITIONAL_BOT\"}\n",ident);
                     break;
                     default:
                        fprintf(fp,"%s{technology \"NONE\"}\n",ident);
                     break;
                  }
*/
               }
               NEUT_WritePADSTACKData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1.0, 0, -1, &drill);
            }
            break;
         }
      }  // if not PCB or PANEL
   } // while

   return 1;
}


/****************************************************************************/
/*
*/
static int load_NEUTsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"NEUTRAL Settings", MB_OK | MB_ICONHAND);
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
         if (!STRCMPI(lp,".SHOWOUTLINE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (lp[0] == 'Y' || lp[0] == 'y')
            {
               SHOWOUTLINE = TRUE;
            }
         }
      }
   }

   fclose(fp);
   return 1;
}

//--------------------------------------------------------------
static int do_pcbfile(FILE *wfp, FileStruct *file, double xoffset, double yoffset, 
                      double rot, int page_units)
{
   double   accuracy = get_accuracy(doc);

   // here we need to add the offset of the board if the file is a panel.
   fprintf(wfp,"#############################################\n");
   fprintf(wfp,"###Board Information\n");
   fprintf(wfp,"#############################################\n");
   fprintf(wfp,"BOARD %s OFFSET x:%lg y:%lg ORIENTATION    %1.1lf\n",
            file->getName(), xoffset, yoffset, RadToDeg(rot));
         
   // units
   switch (page_units)
   {
      case UNIT_INCHES:
         fprintf(wfp,"B_UNITS INCHES\n");
      break;
      case UNIT_MILS:
         fprintf(wfp,"B_UNITS MILS\n");
      break;
      case UNIT_MM:
         fprintf(wfp,"B_UNITS MM\n");
      break;
      default:
         MessageBox(NULL, "Error !", "Page Units must be [inches, mils, mm]", MB_ICONEXCLAMATION | MB_OK);
         return -1;
      break;
   }

   update_smdrule_components(doc, file, ComponentSMDrule);
   generate_PINLOC(doc,file,0);  // this function generates the PINLOC argument for all pins.

   fprintf(wfp,"#############################################\n");
   fprintf(wfp,"###Attribute Information\n");
   fprintf(wfp,"#############################################\n");

/*
         fprintf(wfp,"# import CAD system, not yet in data structure\n");
         fprintf(wfp,"# attributes under file\n");

         fprintf(wfp,"# layerlist with attributes\n");
*/
   fprintf(wfp,"B_ATTR 'BOARD_OUTLINE' '' ");

   Outline_Start(doc);

   int outline_found = NEUT_WriteOutlineData(&(file->getBlock()->getDataList()), 
      xoffset, yoffset, rot, 0, 1.0, 0, -1, GR_CLASS_BOARDOUTLINE);

   if (!outline_found)
   {
      outline_found = NEUT_WriteOutlineData(&(file->getBlock()->getDataList()), 
      xoffset, yoffset, rot, 0, 1.0, 0, -1, GR_CLASS_BOARD_GEOM);
   }

   if (!outline_found)
   {
      fprintf(flog,"Did not find a BoardOutline.\n");
      display_error++;
   }
   else
   {
      int   cnt = 0;
      int returnCode;
      if (!write_easyoutline(wfp, Outline_GetOutline(&returnCode, accuracy),"board outline", cnt))
      {
         fprintf(flog, "Error in easy outline for Board Outline\n");
         display_error++;
      }
   }
   Outline_FreeResults();

   do_components(wfp,file, xoffset, yoffset, rot);
   do_signals(wfp, file, &file->getNetList(), xoffset, yoffset, rot);

   return 1;
}

//--------------------------------------------------------------
void MNEUTRAL_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, 
                         int page_units)
{
#ifndef _DEBUG
   ErrorMessage("Function not implemented","Neutral Write");
#endif


   FILE *wfp;
   FileStruct *file;

   int   pcb_found = FALSE;
   doc = Doc;
   display_error = FALSE;

   linecnt = 0;

   SHOWOUTLINE = FALSE;

   CString logFile = GetLogfilePath("neutral.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   ComponentSMDrule = 0;
   
   CString settingsFile( getApp().getExportSettingsFilePath("neutral.out") );
   load_NEUTsettings(settingsFile);

   unitsFactor = 1.0;
   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %s",filename);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }

   POSITION pos = doc->getFileList().GetHeadPosition();

   polyarray.SetSize(100,100);
   polycnt = 0;

   fprintf(wfp,"#\n");
   fprintf(wfp,"# Mentor Graphics, MENTOR NEUTRAL File\n");
   fprintf(wfp,"# job \"%s\"\n", filename);
   fprintf(wfp,"#\n");
   linecnt++;

   update_smdrule_geometries(doc, ComponentSMDrule);

   fprintf(wfp,"# here padstacks\n");
   // do_padstacks(wfp,file);
   do_shapes(wfp);
   
   while (pos != NULL)
   {
      cur_file = file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
   
      if (file->getBlockType() == BLOCKTYPE_PANEL)   
      {
         // units
         // units
         switch (page_units)
         {
            case UNIT_INCHES:
               fprintf(wfp,"B_UNITS INCHES\n");
            break;
            case UNIT_MILS:
               fprintf(wfp,"B_UNITS MILS\n");
            break;
            case UNIT_MM:
               fprintf(wfp,"B_UNITS MM\n");
            break;
            default:
               MessageBox(NULL, "Error !", "Page Units must be [inches, mils, mm]", MB_ICONEXCLAMATION | MB_OK);
            return;
            break;
         }
// board
// b_attr
// net
// comp 
// holes
// b_addp

         fprintf(wfp,"# Panel data not yet implemented\n");
      }
      else
      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         double u = unitsFactor;

         do_pcbfile(wfp, file, 0.0, 0.0, 0, page_units);

         pcb_found = TRUE;

         fprintf(wfp,"#\n");  
         // allows to do more PCB files
      } // if pcb file
   }

   fprintf(wfp,"# end\n");
   fclose(wfp);

   if (!pcb_found)
   {
      MessageBox(NULL, "No PCB file found !","NEUTRAL Output", MB_OK | MB_ICONHAND);
      return;
   }

   fprintf(flog,"PAD section must be implemented.\n");
   fprintf(flog,"TEST section must be implemented.\n");
   fprintf(flog,"HOLE section must be implemented.\n");
   fprintf(flog,"PANEL section must be implemented.\n");
   display_error++;

   fclose(flog);
   polyarray.RemoveAll();

   if (display_error)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*
   end MNEUT_O.CPP
*/
/****************************************************************************/

