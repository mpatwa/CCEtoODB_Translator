// $Header: /CAMCAD/4.6/Width.cpp 15    2/14/07 4:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/

/*----------------------------------------------------------
 WIDTH.CPP  (FUNCTION FOR WOLFGANG)
   when the penwidth < data's width, this function (width.cpp) make
   the pen draw a poly to fill the data's width
   data could be line,poly,arc,circle,or filled circle 
   the draw way could be RoundEnd or SquareEnd
   ---Parameters:   CCEtoODBDoc *doc,  ( the doc which the data in it)
                    DataStruct *data, (the entety which width > penwidth)
                    double penwidth, (width of the pen)
   ---Return:       CPoly *widthpoly,(the poly which pen drawing to fill
                                       the width) 
                                       
   NOTE:
   For every data need to call:

   while (loop thru data)
   {
      (1) void widthpoly_StartNew(CCEtoODBDoc *Doc);
      (2) void Width(CCEtoODBDoc *doc, DataStruct *data,  
                  double penwidth, CPoly *widthpoly);
      (3) void widthpoly_Free();
   }

----------------------------------------------------------*/
#include "stdafx.h"
#include "width.h"

void widthpoly_StartNew(CCEtoODBDoc *Doc) //START A NEW POLY AT THE BEGINING
{  
   CCEtoODBDoc *doc;
   widthpoly = new CPoly;
   widthpoly->setClosed(false);
   widthpoly->setFilled(false); 
   widthpoly->setHatchLine(false);
   widthpoly->setHidden(false);
   widthpoly->setVoid(false); 
   widthpoly->setThermalLine(false);
   doc = Doc;
}

void widthpoly_Free() //FREE THE WIDTHPOLY FOR MEMORY
{  
   POSITION pntPos, tem_Pos;
   pntPos = widthpoly->getPntList().GetHeadPosition();
   while (pntPos != NULL)
   {
      tem_Pos = pntPos;
      delete widthpoly->getPntList().GetNext(pntPos);
      widthpoly->getPntList().RemoveAt(tem_Pos);
   }
   widthpoly->getPntList().RemoveAll();
   delete widthpoly;
}

/*******************************************************************
static void poly_for_circle_with_width(double px,double py, double width,
                     double radius,double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_circle_with_width(double px,double py,double width,
                        double radius, double penwidth,CPoly *widthpoly)
{
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   Point2 a, b, c, d;
   CPnt *pnt;
   while ( passNum * penwidth < (width - penwidth/2))
   {
      a.x = b.x = c.x = d.x = px ;
      a.y = b.y = py + (radius + width/2 - penwidth/2 - passNum*penwidth);
      c.y = d.y = py - (radius + width/2 - penwidth/2 - passNum*penwidth);
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
   a.y = b.y = py + (radius - width/2 + penwidth/2);
   c.y = d.y = py - (radius - width/2 + penwidth/2);
   passNum++;
   pnt = new CPnt;
   pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
   pnt = new CPnt;
   pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)(-1.0);
   widthpoly->getPntList().AddTail(pnt);
   pnt = new CPnt;
   pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
   pnt = new CPnt;
   pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)(-1.0);
   widthpoly->getPntList().AddTail(pnt);
   pnt = new CPnt;
   pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
}
/*******************************************************************
static void poly_for_arc_with_width_RoundEnd(double p1x,double p1y,double bulge,
                                 double p2x,double p2y, double width,
                                 double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_arc_with_width_RoundEnd(double p1x,double p1y,double bulge,
                                      double p2x,double p2y,double width,
                                       double penwidth,CPoly *widthpoly)
{
   double delta,start,radius,center_x,center_y;
   Point2 center;
   delta = atan(bulge) * 4;
   ArcPoint2Angle(p1x, p1y, p2x, p2y, delta,
   &center.x, &center.y, &radius, &start);
   center_x = center.x; center_y = center.y;
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   double theta = ArcTan2( (p2y-p1y),(p2x-p1x) );
   Point2 a, b, c, d;
   CPnt *pnt;
   while (passNum * penwidth * 2 < width)
   {
      a.x = center_x - (radius + width/2 - penwidth/2 - passNum*penwidth)
                       * (center_x - p1x) / radius;
      a.y = center_y - (radius + width/2 - penwidth/2 - passNum*penwidth)
                       * (center_y - p1y) / radius;
      b.x = center_x - (radius + width/2 - penwidth/2 - passNum*penwidth)
                       * (center_x - p2x) / radius;
      b.y = center_y - (radius + width/2 - penwidth/2 - passNum*penwidth)
                       * (center_y - p2y) / radius;
      c.x = p2x + (width/2 - penwidth/2 - passNum*penwidth)
                       * (center_x - p2x) / radius;
      c.y = p2y + (width/2 - penwidth/2 - passNum*penwidth)
                       * (center_y - p2y) / radius;
      d.x = p1x + (width/2 - penwidth/2 - passNum*penwidth)
                       * (center_x - p1x) / radius;
      d.y = p1y + (width/2 - penwidth/2 - passNum*penwidth)
                       * (center_y - p1y) / radius;
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)bulge;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; 
      if (bulge < 0.0) pnt->bulge = (DbUnit)(-1.0);
      else pnt->bulge = (DbUnit)1.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)(-bulge);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; 
      if (bulge < 0.0) pnt->bulge = (DbUnit)(-1.0);
      else pnt->bulge = (DbUnit)1.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
   pnt = new CPnt;
   pnt->x = (DbUnit)p1x; pnt->y = (DbUnit)p1y; pnt->bulge = (DbUnit)bulge;
   widthpoly->getPntList().AddTail(pnt);
   pnt = new CPnt;
   pnt->x = (DbUnit)p2x; pnt->y = (DbUnit)p2y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
}

/*******************************************************************
static void poly_for_point_with_width_RoundEnd(double px,double py, double width,
                                       double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_point_with_width_RoundEnd(double px,double py,double width,
                                       double penwidth,CPoly *widthpoly)
{
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   Point2 a, b, c, d;
   CPnt *pnt;
   while ( passNum * penwidth < (width/2 - penwidth/2))
   {
      a.x = b.x = c.x = d.x = px ;
      a.y = b.y = py + (width/2 - penwidth/2 - passNum*penwidth);
      c.y = d.y = py - (width/2 - penwidth/2 - passNum*penwidth);
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
}



/*******************************************************************
static void poly_for_line_with_width_RoundEnd(double p1x,double p1y,double p2x,
                                       double p2y, double width,
                                       double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_line_with_width_RoundEnd(double p1x,double p1y,double p2x,
                                       double p2y,double width,
                                       double penwidth,CPoly *widthpoly)
{
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   double theta = ArcTan2( (p2y-p1y),(p2x-p1x) );
   Point2 a, b, c, d;
   CPnt *pnt;
   while (passNum * penwidth * 2 < width)
   {
      a.x = p1x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      a.y = p1y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.x = p2x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.y = p2y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.x = p1x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.y = p1y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.x = p2x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.y = p2y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
   pnt = new CPnt;
   pnt->x = (DbUnit)p2x; pnt->y = (DbUnit)p2y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
}
/*******************************************************************
static void poly_for_line_with_width_SquareEnd_both(double p1x,double p1y,double p2x,
                                       double p2y, double width,
                                       double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_line_with_width_SquareEnd_both(double p1x,double p1y,double p2x,
                                       double p2y,double width,
                                       double penwidth,CPoly *widthpoly)
{
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   double theta, p1p2; 
   theta = ArcTan2( (p2y-p1y),(p2x-p1x) );
   p1p2 = sqrt(pow(p1x - p2x,2) + pow(p1y - p2y,2));
   p1x = p1x - (width/2 - penwidth/2) * (p2x - p1x) / p1p2;
   p1y = p1y - (width/2 - penwidth/2) * (p2y - p1y) / p1p2;
   p2x = p2x - (width/2 - penwidth/2) * (p1x - p2x) / (p1p2 + width/2 - penwidth/2);
   p2y = p2y - (width/2 - penwidth/2) * (p1y - p2y) / (p1p2 + width/2 - penwidth/2);
   Point2 a, b, c, d;
   CPnt *pnt;
   while (passNum * penwidth * 2 < width)
   {
      a.x = p1x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      a.y = p1y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.x = p2x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.y = p2y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.x = p1x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.y = p1y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.x = p2x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.y = p2y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
   pnt = new CPnt;
   pnt->x = (DbUnit)p2x; pnt->y = (DbUnit)p2y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
}

/*******************************************************************
static void poly_for_line_with_width_SquareEnd_head(double p1x,double p1y,double p2x,
                                       double p2y, double width,
                                       double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_line_with_width_SquareEnd_head(double p1x,double p1y,double p2x,
                                       double p2y,double width,
                                       double penwidth,CPoly *widthpoly)
{
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   double theta, p1p2; 
   theta = ArcTan2( (p2y-p1y),(p2x-p1x) );
   p1p2 = sqrt(pow(p1x - p2x,2) + pow(p1y - p2y,2));
   p1x = p1x - (width/2 - penwidth/2) * (p2x - p1x) / p1p2;
   p1y = p1y - (width/2 - penwidth/2) * (p2y - p1y) / p1p2;
   Point2 a, b, c, d;
   CPnt *pnt;
   while (passNum * penwidth * 2 < width)
   {
      a.x = p1x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      a.y = p1y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.x = p2x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.y = p2y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.x = p1x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.y = p1y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.x = p2x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.y = p2y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
   pnt = new CPnt;
   pnt->x = (DbUnit)p2x; pnt->y = (DbUnit)p2y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
}

/*******************************************************************
static void poly_for_line_with_width_SquareEnd_tail(double p1x,double p1y,double p2x,
                                       double p2y, double width,
                                       double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_line_with_width_SquareEnd_tail(double p1x,double p1y,double p2x,
                                       double p2y,double width,
                                       double penwidth,CPoly *widthpoly)
{
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   double theta, p1p2; 
   theta = ArcTan2( (p2y-p1y),(p2x-p1x) );
   p1p2 = sqrt(pow(p1x - p2x,2) + pow(p1y - p2y,2));
   p2x = p2x - (width/2 - penwidth/2) * (p1x - p2x) / p1p2;
   p2y = p2y - (width/2 - penwidth/2) * (p1y - p2y) / p1p2;
   Point2 a, b, c, d;
   CPnt *pnt;
   while (passNum * penwidth * 2 < width)
   {
      a.x = p1x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      a.y = p1y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.x = p2x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.y = p2y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.x = p1x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.y = p1y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.x = p2x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.y = p2y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)(-1.0);
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
   pnt = new CPnt;
   pnt->x = (DbUnit)p2x; pnt->y = (DbUnit)p2y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
}

/*******************************************************************
static void poly_for_point_with_width_SquareEnd(double px,double py,double width,
                                       double penwidth,CPoly *widthpoly)
********************************************************************/
static void poly_for_point_with_width_SquareEnd(double px,double py,double width,
                                       double penwidth,CPoly *widthpoly)
{
   int passNum = 0;
   // ArcTan2 is from geomlib.c and check for 0 etc ..
   double theta,p1x,p1y,p2x,p2y; 
   theta = ArcTan2( (py-py),(px-px) );
   p1x = px - (width/2 - penwidth/2) ;
   p2x = px + (width/2 - penwidth/2) ;
   p1y = p2y = py;

   Point2 a, b, c, d;
   CPnt  *pnt;

   while (passNum * penwidth * 2 < width)
   {
      a.x = p1x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      a.y = p1y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.x = p2x - sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      b.y = p2y + cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.x = p1x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      d.y = p1y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.x = p2x + sin(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      c.y = p2y - cos(theta) * (width/2 - penwidth/2 - passNum*penwidth);
      passNum++;
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)b.x; pnt->y = (DbUnit)b.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)c.x; pnt->y = (DbUnit)c.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)d.x; pnt->y = (DbUnit)d.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
      pnt = new CPnt;
      pnt->x = (DbUnit)a.x; pnt->y = (DbUnit)a.y; pnt->bulge = (DbUnit)0.0;
      widthpoly->getPntList().AddTail(pnt);
   }
   pnt = new CPnt;
   pnt->x = (DbUnit)p2x; pnt->y = (DbUnit)p2y; pnt->bulge = (DbUnit)0.0;
   widthpoly->getPntList().AddTail(pnt);
}



/*********************************************************
void Width(CCEtoODBDoc *doc,DataStruct *data, 
             double penwidth, CPoly *widthpoly)

      doc    --- the doc which has data for get width 
                                 and RonunEnd/SquareEnd
      data   ---the data could be line, arc, circle/filled circle and poly
      penwidth --- the width of pen
      return ---widthpoly which filled the width of data

**********************************************************/

void Width(CCEtoODBDoc *doc, DataStruct *data,  
           double penwidth, CPoly *widthpoly)
{
   switch(data->getDataType())
   {
      case T_POLY:
      {
         {
            CPoly *poly;
            CPnt *last_pnt, *current_pnt, *next_pnt;
            double width,p1x,p1y,bulge,p2x,p2y;
            int shape;
            POSITION pos;
            poly = data->getPolyList()->GetHead();
            width = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
            if (doc->getWidthTable()[poly->getWidthIndex()]->getShape() == T_ROUND)
               shape = 1;
            else if (doc->getWidthTable()[poly->getWidthIndex()]->getShape() == T_SQUARE)
               shape = 2;
            if (penwidth >= width)
               return;
            pos = poly->getPntList().GetHeadPosition();
            current_pnt = poly->getPntList().GetNext(pos);
            int i = 0;
            BOOL End = FALSE;
            while(pos != NULL)
            {
               last_pnt = current_pnt;
               current_pnt = poly->getPntList().GetNext(pos);
               if (End) pos = NULL;
               p1x = last_pnt->x ;  
               p1y = last_pnt->y ;
               bulge = last_pnt->bulge;
               p2x = current_pnt->x;
               p2y = current_pnt->y;
               if (pos == NULL && i == 0 && shape == 1)
               {  //poly is  a point
                  if (fabs(p1x - p2x) < SMALLNUMBER &&
                         fabs(p1y - p2y) < SMALLNUMBER)
                     poly_for_point_with_width_RoundEnd(p1x,p1y, width, 
                           penwidth,widthpoly);
                  else  //poly is a line or arc
                  {   
                     if (fabs(bulge) < SMALLNUMBER)
                        poly_for_line_with_width_RoundEnd(p1x,p1y,p2x,p2y,
                                       width,penwidth,widthpoly);
                     else //bulge != 0
                     {
                        poly_for_arc_with_width_RoundEnd(p1x, p1y, bulge,
                                       p2x, p2y, width,
                                             penwidth,widthpoly);
                     }
                  }
               }
               else if (pos == NULL && i == 0 && shape == 2)
               {  //poly is a line or a point
                  if (fabs(p1x - p2x) < SMALLNUMBER &&
                         fabs(p1y - p2y) < SMALLNUMBER)
                     poly_for_point_with_width_SquareEnd(p1x, p1y, width,
                                                penwidth,widthpoly);
                  else
                  {   
                     if (fabs(bulge) < SMALLNUMBER)
                        poly_for_line_with_width_SquareEnd_both(p1x,p1y,p2x,p2y,
                                       width,penwidth,widthpoly);
                     else //bulge != 0
                     {
                        poly_for_arc_with_width_RoundEnd(p1x, p1y, bulge,
                                       p2x, p2y,  width,
                                             penwidth,widthpoly);
                     }
                  }
               }
               else if (pos == NULL && i != 0 && shape == 2 && !poly->isClosed())
               {
                  // poly's tailline is a point
                  if (fabs(p1x - p2x) < SMALLNUMBER &&
                         fabs(p1y - p2y) < SMALLNUMBER)
                  {
                     pos = poly->getPntList().GetTailPosition(); 
                     poly->getPntList().GetPrev(pos);
                     poly->getPntList().GetPrev(pos);
                     current_pnt = poly->getPntList().GetNext(pos);
                     next_pnt = poly->getPntList().GetAt(pos);
                     while (fabs(current_pnt->x - next_pnt->x) < SMALLNUMBER
                           && fabs(current_pnt->y - next_pnt->y) < SMALLNUMBER)
                     {
                        poly->getPntList().GetPrev(pos);
                        poly->getPntList().GetPrev(pos);
                        current_pnt = poly->getPntList().GetNext(pos);
                        next_pnt = poly->getPntList().GetAt(pos);
                     }
                     End = TRUE;
                     continue;
                  }
                  else
                  { 
                     if (fabs(bulge) < SMALLNUMBER)
                     {
                        poly_for_line_with_width_SquareEnd_tail(p1x,p1y,p2x,p2y,
                                       width,penwidth,widthpoly);
                     }
                     else //bulge != 0
                     {
                        poly_for_arc_with_width_RoundEnd(p1x, p1y, bulge,
                                       p2x, p2y,  width,
                                             penwidth,widthpoly);
                     }
                  }
               }
               else if (pos != NULL && i == 0 && shape == 2 && !poly->isClosed())
               {
                  // poly's headline is  a point
                  if (fabs(p1x - p2x) < SMALLNUMBER &&
                         fabs(p1y - p2y) < SMALLNUMBER)
                        i--; 
                  else
                  { 
                     if (fabs(bulge) < SMALLNUMBER)
                     {
                        poly_for_line_with_width_SquareEnd_head(p1x,p1y,p2x,p2y,
                                       width,penwidth,widthpoly);
                     }
                     else //bulge != 0
                     {
                        poly_for_arc_with_width_RoundEnd(p1x, p1y, bulge,
                                       p2x, p2y, width,
                                             penwidth,widthpoly);
                     }
                  }
               }
               else
               {
                  // line is not a point
                  if (fabs(p1x - p2x) > SMALLNUMBER 
                       || fabs(p1y - p2y) > SMALLNUMBER)
                  {
                     if (fabs(bulge) < SMALLNUMBER)
                     {
                           poly_for_line_with_width_RoundEnd(p1x,p1y,p2x,p2y,
                                          width,penwidth,widthpoly);
                     }
                     else //bulge != 0
                     {
                        poly_for_arc_with_width_RoundEnd(p1x, p1y, bulge,
                                              p2x, p2y,  width,
                                             penwidth,widthpoly);
                     }
                  }
               }
               i++;
               if (End) break;   
            }  
         }       
      } // end case T_POLY
      break;

      default:
         return;//   continue;
   }; // end switch
      
}
//END OF WIDTH.CPP