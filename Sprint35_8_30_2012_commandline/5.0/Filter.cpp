// $Header: /CAMCAD/4.5/Filter.cpp 21    1/16/07 6:05p Rick Faltersack $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-8. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccview.h"
#include "ccdoc.h"
#include "float.h"
#include "mainfrm.h"
#include "draw.h"
#include "polylib.h"

extern "C"
{
#include "c2ddefs.h"
#include "c2cdefs.h"
#include "c2vdefs.h"
}

/******************************************************************************
* Filter To Types
*/
void CCEtoODBDoc::OnFiltertypesAll() 
{ getSettings().FilterTypes = Filter_All; }

void CCEtoODBDoc::OnFiltertypesPins() 
{ getSettings().FilterTypes = Filter_Pins; }

/******************************************************************************
* Filter Snap
*/
void CCEtoODBDoc::OnFilter() 
{ 
   getSettings().FilterOn = !getSettings().FilterOn;

   // Case dts0100381184 - If filter is getting turned on, then leave it
   // on until explicitly turned off. If filter is off, then return the
   // "singleTime" flag back to it's native state of true. Turns out just 
   // setting the "singleTime" to opposite of "FilterOn" does just that.
   getSettings().m_filterOnSingleTime = !getSettings().FilterOn;

   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
      ((CCEtoODBView *)GetNextView(pos))->FilterFirst = TRUE;
}

void CCEtoODBDoc::OnFilterEndpoint() 
{ getSettings().FilterMode = Filter_Endpoint; }

void CCEtoODBDoc::OnFilterEntity() 
{ getSettings().FilterMode = Filter_Entity; }

void CCEtoODBDoc::OnFilterCenter() 
{ getSettings().FilterMode = Filter_Center; }

void CCEtoODBDoc::OnFilterInserts() 
{ getSettings().FilterInserts = !getSettings().FilterInserts; }

void CCEtoODBDoc::OnFilterLines() 
{ getSettings().FilterLines = !getSettings().FilterLines; }

void CCEtoODBDoc::OnFilterPoints() 
{ getSettings().FilterPoints = !getSettings().FilterPoints; }


static void SearchFilter_Block(CCEtoODBView *view, double x, double y, PT2 *mousePt, double factor,
      BlockStruct *block, double insert_x, double insert_y, double scale, double rotation, int mirror,
      double *dist, SelectStruct *s);


/******************************************************************************
* CCEtoODBView::SearchFilter
*/
BOOL CCEtoODBView::SearchFilter(CPoint point, SelectStruct *s)
{
   CCEtoODBDoc *doc = GetDocument();

   FilterX = point.x;
   FilterY = point.y;

   double x, y, factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;
   x = factor * point.x;
   y = factor * point.y;

   PT2 mousePt;
   c2v_set(x, y, mousePt);

   double dist = DBL_MAX;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() ||file->isHidden())
         continue;

      double fileX = file->getInsertX();
      double fileRot = file->getRotation();
      int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);
      if (doc->getBottomView())
      {
         fileX = -fileX;
         fileRot = -fileRot;
      }
      SearchFilter_Block(this, x, y, &mousePt, factor,
            file->getBlock(), fileX, file->getInsertY(), file->getScale(), fileRot, fileMirror,
            &dist, s);
   } // end loop files

   return (dist < DBL_MAX - 1);
}

/******************************************************************************
* SearchFilter_Block
*/
void SearchFilter_Block(CCEtoODBView *view, double x, double y, PT2 *mousePt, double factor,
      BlockStruct *block, double insert_x, double insert_y, double scale, double rotation, int mirror,
      double *dist, SelectStruct *s)
{
   CCEtoODBDoc *doc = view->GetDocument();

   Mat2x2 m;
   RotMat2(&m, rotation);

   PT2 pt;
   DataStruct *data;
   CPoly *poly;
   CPnt *pnt;
   POSITION dataPos, polyPos, pntPos;
   double temp;
   Point2 p;
   int layer;
   BlockStruct *subblock;

   dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      data = block->getDataList().GetNext(dataPos);

/*    // figure LAYER
      if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
         layer = insertLayer;
      else*/
         layer = data->getLayerIndex();

      if(!IsEntityVisible(data, &subblock, doc, mirror, &layer, FALSE))
         continue;

      switch (data->getDataType())
      {
      case T_POINT:
         if (!doc->getSettings().FilterPoints)
            continue;

         p.x = data->getPoint()->x * scale;
         if (mirror & MIRROR_FLIP) p.x = -p.x;
         p.y = data->getPoint()->y * scale;
         TransPoint2(&p, 1, &m, insert_x, insert_y);

         temp = fabs(p.x - x) + fabs(p.y - y); // Manhattan Distance
         if ((*dist - temp) > SMALLNUMBER)
         {
            *dist = temp;
            s->setData(data);
            s->insert_x = (DbUnit)insert_x;
            s->insert_y = (DbUnit)insert_y;
            s->scale = (DbUnit)scale;
            s->mirror = mirror;
            s->rotation = (DbUnit)rotation;
            view->FilterX = round(p.x / factor);
            view->FilterY = round(p.y / factor);
         }
         break;

      case T_INSERT:
         if (!doc->getSettings().FilterInserts)
            continue;

         if (doc->getSettings().FilterMode != Filter_Center)
            continue;

         p.x = data->getInsert()->getOriginX() * scale;
         if (mirror & MIRROR_FLIP) p.x = -p.x;
         p.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&p, 1, &m, insert_x, insert_y);

         temp = fabs(p.x - x) + fabs(p.y - y); // Manhattan Distance
         if ((*dist - temp) > SMALLNUMBER)
         {
            *dist = temp;
            s->setData(data);
            s->insert_x = (DbUnit)insert_x;
            s->insert_y = (DbUnit)insert_y;
            s->scale = (DbUnit)scale;
            s->mirror = mirror;
            s->rotation = (DbUnit)rotation;
            view->FilterX = round(p.x / factor);
            view->FilterY = round(p.y / factor);
         }
         break;

      case T_POLY:
         if (!doc->getSettings().FilterLines)
            continue;

         polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            poly = data->getPolyList()->GetNext(polyPos);
            pntPos = poly->getPntList().GetHeadPosition();

            switch (doc->getSettings().FilterMode)
            {
            case Filter_Endpoint:
               {
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
         
                     p.x = pnt->x * scale;
                     if (mirror & MIRROR_FLIP) p.x = -p.x;
                     p.y = pnt->y * scale;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);

                     temp = fabs(p.x - x) + fabs(p.y - y); // Manhattan Distance
                     if (temp < *dist)
                     {
                        *dist = temp;
                        s->setData(data);
                        s->insert_x = (DbUnit)insert_x;
                        s->insert_y = (DbUnit)insert_y;
                        s->scale = (DbUnit)scale;
                        s->mirror = mirror;
                        s->rotation = (DbUnit)rotation;
                        view->FilterX = round(p.x / factor);
                        view->FilterY = round(p.y / factor);
                     }
                  }
               }
               break;

            case Filter_Entity:
               {
                  C2_CURVE curve;
                  CPnt *lastPnt;

                  lastPnt = poly->getPntList().GetNext(pntPos);

                  p.x = lastPnt->x * scale;
                  if (mirror & MIRROR_FLIP) p.x = -p.x;
                  p.y = lastPnt->y * scale;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);

                  // check first endpoint
                  {
                     temp = fabs(p.x - x) + fabs(p.y - y); // Manhattan Distance
                     if (temp < *dist)
                     {
                        *dist = temp;
                        s->setData(data);
                        s->insert_x = (DbUnit)insert_x;
                        s->insert_y = (DbUnit)insert_y;
                        s->scale = (DbUnit)scale;
                        s->mirror = mirror;
                        s->rotation = (DbUnit)rotation;
                        view->FilterX = round(p.x / factor);
                        view->FilterY = round(p.y / factor);
                     }
                  }

                  // start building curve
                  c2v_set(p.x, p.y, pt);
                  curve = c2d_pcurve_init(pt);

                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
         
                     p.x = pnt->x * scale;
                     if (mirror & MIRROR_FLIP) p.x = -p.x;
                     p.y = pnt->y * scale;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);

                     // build curve
                     c2v_set(p.x, p.y, pt);

                     if (fabs(lastPnt->bulge) < SMALLNUMBER)
                     {
                        c2d_pcurve_add_line(curve, pt);
                     }
                     else
                     {
                        double da, sa, r, cx, cy;
                        da = atan(lastPnt->bulge) * 4;
                        if (mirror & MIRROR_FLIP) da = -da;

                        Point2 temp;
                        temp.x = lastPnt->x * scale;
                        if (mirror & MIRROR_FLIP) temp.x = -temp.x;
                        temp.y = lastPnt->y * scale;
                        TransPoint2(&temp, 1, &m, insert_x, insert_y);

                        ArcPoint2Angle(temp.x, temp.y, p.x, p.y, da, &cx, &cy, &r, &sa);

                        PT2 pt_mid;
                        c2v_set(cx + r * cos(sa+da/2), cy + r * sin(sa+da/2), pt_mid);
                        c2d_pcurve_add_arc_2pts(curve, pt_mid, pt);
                     }

                     // check endpoint
                     {
                        temp = fabs(p.x - x) + fabs(p.y - y); // Manhattan Distance
                        if (temp < *dist)
                        {
                           *dist = temp;
                           s->setData(data);
                           s->insert_x = (DbUnit)insert_x;
                           s->insert_y = (DbUnit)insert_y;
                           s->scale = (DbUnit)scale;
                           s->mirror = mirror;
                           s->rotation = (DbUnit)rotation;
                           view->FilterX = round(p.x / factor);
                           view->FilterY = round(p.y / factor);
                        }
                     }

                     lastPnt = pnt;
                  }

                  // check curve
                  PT2 ptOnCurve;
                  if (c2c_project(curve, *mousePt, NULL, ptOnCurve))
                  {
                     temp = c2v_distl1(*mousePt, ptOnCurve); // Manhattan Distance

                     if (temp < *dist)
                     {
                        *dist = temp;
                        s->setData(data);
                        s->insert_x = (DbUnit)insert_x;
                        s->insert_y = (DbUnit)insert_y;
                        s->scale = (DbUnit)scale;
                        s->mirror = mirror;
                        s->rotation = (DbUnit)rotation;
                        view->FilterX = round(PT2_X(ptOnCurve) / factor);
                        view->FilterY = round(PT2_Y(ptOnCurve) / factor);
                     }
                  }

                  // free curve
                  c2d_free_curve(curve);
               }
               break;

            case Filter_Center:
               {
                  // if PolyIsCircle
                  double cx, cy, radius;
                  if (PolyIsCircle(poly, &cx, &cy, &radius))
                  {
                     p.x = cx * scale;
                     if (mirror & MIRROR_FLIP) p.x = -p.x;
                     p.y = cy * scale;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);

                     temp = fabs(p.x - x) + fabs(p.y - y); // Manhattan Distance
                     if (temp < *dist)
                     {
                        *dist = temp;
                        s->setData(data);
                        s->insert_x = (DbUnit)insert_x;
                        s->insert_y = (DbUnit)insert_y;
                        s->scale =(DbUnit) scale;
                        s->mirror = mirror;
                        s->rotation = (DbUnit)rotation;
                        view->FilterX = round(p.x / factor);
                        view->FilterY = round(p.y / factor);
                     }
                  }
               }
               break;
            }
         }
         break;
      } // end switch
   } // end loop datas
}

// end FILTER.CPP