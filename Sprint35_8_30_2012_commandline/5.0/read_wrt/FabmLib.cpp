// $Header: /CAMCAD/4.5/read_wrt/FabmLib.cpp 17    4/28/06 6:51p Kurt Van Ness $

/*****************************************************************************/
/*    
   Output for FABMASTER Repoer-Files
*/

#include "stdafx.h"
#include "ccdoc.h"               
#include "graph.h"
#include "fabmlib.h"
#include "attrib.h"
#include "geomlib.h"
#include <math.h>
#include <float.h>
#include "crypt.h"
#include <afxtempl.h>
#include "gauge.h"

static int     output_units = 3;

/*****************************************************************************/
/*
*/
static double wFabm_Units(int format, double x)
{
   CString  t;
   t.Format("%1.*lf", format, x);
   return atof(t);
}

/******************************************************************************
* FABM_WriteFormat
*/
static int FABM_WriteFormat(FILE* wfp, CCEtoODBDoc *doc, CDataList *DataList,
                    double insert_x, double insert_y, double rotation, int mirror, double scale, int insertLayer)
{
   Mat2x2 m;
   int layer;
   int boardoutline = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = DataList->GetNext(pos);
		if (data == NULL)
			continue;

      if (insertLayer != -1)
         layer = insertLayer;
      else
         layer = data->getLayerIndex();


      if (data->getDataType() == dataTypeInsert)
      {
			InsertStruct* insert = data->getInsert();
			if (insert == NULL)
				continue;

         if (insert->getInsertType() == insertTypePin || insert->getInsertType() == insertTypePcbComponent)
				continue;

         Point2 point2;
         point2.x = insert->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = insert->getOriginY() * scale;

         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         BlockStruct *block = doc->getBlockAt(insert->getBlockNumber());
			if (block == NULL)
				continue;

			if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ insert->getMirrorFlags();
            double block_rot = rotation + insert->getAngle();

            if (mirror)
               block_rot = PI2 - (rotation + insert->getAngle());   // add 180 degree if mirrored.

            boardoutline += FABM_WriteFormat(wfp, doc, &(block->getDataList()), point2.x, point2.y,
										block_rot, block_mirror, scale * insert->getScale(), layer);
         }
      }
		else if (data->getDataType() == dataTypePoly)
		{
         // nothing with a netname
         Attrib *attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
         if (attrib)
				continue;
   
         if (!doc->get_layer_visible(layer, mirror))
            continue;

         int lnr = doc->get_layer_mirror(layer, mirror);
         int boardfound = FALSE;

         // A boardoutline is graphic marked as Primary Boardoutline or placed on a layer->type LAYTYPE_BOARDOUTLINE
         LayerStruct *lstr = doc->getLayerArray()[lnr];
         if (lstr->getLayerType() == LAYTYPE_BOARD_OUTLINE || data->getGraphicClass() == graphicClassBoardOutline)
            boardfound = TRUE;

         // loop thru polys
         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            CPoly* poly = data->getPolyList()->GetNext(polyPos);
				if (poly == NULL)
					continue;

            BOOL polyFilled = poly->isFilled();
            BOOL closed     = poly->isClosed();
            BOOL voidpoly   = poly->isVoid();

            // A boardoutline is graphic marked as Primary Boardoutline or placed on a layer->type LAYTYPE_BOARDOUTLINE
            if (boardfound)
            {
               if (!closed)
                  boardfound = FALSE;
               else
                  boardoutline++;

               if (boardoutline > 1)
                  boardfound = FALSE;
            }

            if (!boardfound)
					break;

				BlockStruct* widthBlock = doc->getWidthTable()[poly->getWidthIndex()];
				if (widthBlock == NULL)
					continue;

            double lineWidth = widthBlock->getSizeA()*scale;
            double oldda = 0;
				Point2 p, p2;

            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt* pnt = poly->getPntList().GetNext(pntPos);
					if (pnt == NULL)
						continue;

               p.x = pnt->x * scale;
               if (mirror)
						p.x = -p.x;
               p.y = pnt->y * scale;
               p.bulge = pnt->bulge;

               TransPoint2(&p, 1, &m, insert_x, insert_y);
               double da = atan(p.bulge) * 4;

               if (fabs(oldda) > SMALLANGLE)
               {
                  double radius, cx, cy, sa;
                  ArcPoint2Angle(p2.x, p2.y, p.x, p.y, oldda, &cx, &cy, &radius, &sa);

                  fprintf(wfp," %7.3lf    %7.3lf    %7.3lf\n", p.x, p.y, radius);
               }
               else
               {
                  fprintf(wfp," %7.3lf    %7.3lf    %7.3lf\n", p.x, p.y, 0.0);
               }

               oldda = da;
               p2 = p;
            }
         }
      }
   }

   return boardoutline;
}

/*****************************************************************************/
/*
 Board Outline Contour             INCH units               15-Sep-1999 16:51

 User Datum  X  0.000,  Y  0.000,  Rotation   0.0

      X           Y         Radius

   -2.725      -1.475       0.000
   -2.725       9.250       0.000
   11.806       9.250       0.000
   11.806      -1.475       0.000
   -2.725      -1.475       0.000
*/
int wFABM_Format_ASC(FILE* wfp, CCEtoODBDoc *Doc, FileStruct *file, int page_units, int output_units)
{
   double unitsFactor = Units_Factor(page_units, output_units);

   CTime t;
   t = t.GetCurrentTime();

   // 15-Sep-1999 16:51
   fprintf(wfp," Board Outline Contour             %s units               %s\n",
      GetUnitName(output_units), t.Format("%d-%b-%Y  %H:%M"));

   fprintf(wfp,"      X           Y         Radius\n");
   fprintf(wfp,"\n");

   // board outline in FORMAT.ASC 
   FABM_WriteFormat(wfp, Doc, &(file->getBlock()->getDataList()), 
      file->getInsertX(), file->getInsertY(),
      file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, -1);

   return 1;
}

/*****************************************************************************/
/*
 Part Pins List          0/967  Selected Parts             15-Sep-1999 16:49
                                                           INCH units

Part        T/B
Pin   Name      X         Y     Layer  Net               Nail(s)

Part U1     (T)

   1    1    -0.759     5.550     1    +12V              624
   2    2    -0.759     5.500     1    SP_RSLD0          38
   3    3    -0.759     5.450     1    SP_DSR0           37
   4    4    -0.759     5.400     1    SP_RXD0           65
   5    5    -0.759     5.350     1    SP_RTS0           36
   6    6    -0.759     5.300     1    SP_TXD0           64
   7    7    -0.759     5.250     1    SP_CTS0           35
   8    8    -0.759     5.200     1    SP_DTR0           63
   9    9    -0.759     5.150     1    SP_RI0            34
  10   10    -0.759     5.100     1    -12V              369
  11   11    -0.391     5.100     1    GROUND            529
  12   12    -0.391     5.150     1    RI0#              96
  13   13    -0.391     5.200     1    DTR0#             62
  14   14    -0.391     5.250     1    CTS0#             94
  15   15    -0.391     5.300     1    TX0               95
  16   16    -0.391     5.350     1    RTS0#             228
  17   17    -0.391     5.400     1    RX0               110
  18   18    -0.391     5.450     1    DSR0#             127
  19   19    -0.391     5.500     1    RLSD0#            128
  20   20    -0.391     5.550     1    +5V               533

*/
int wFABM_Pins_ASC_Header(FILE* wfp, CCEtoODBDoc *Doc, FileStruct *file, 
                          int page_units, int output_units)
{
   double unitsFactor = Units_Factor(page_units, output_units);

   CTime t;
   t = t.GetCurrentTime();

   fprintf(wfp,"Part Pins List                  Selected Parts             %s\n",t.Format("%d-%b-%Y  %H:%M"));
   fprintf(wfp,"                                                           %s units\n", 
               GetUnitName(output_units));
   fprintf(wfp,"\n");
   fprintf(wfp,"Part        T/B\n");
   fprintf(wfp,"Pin   Name      X         Y     Layer  Net               Nail(s)\n");
   fprintf(wfp,"\n");

   //  PINS.ASC 

   return 1;
}


// end FABMLIB.C

