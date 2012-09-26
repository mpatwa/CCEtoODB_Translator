// $Header: /CAMCAD/4.5/LedaPolygon.cpp 6     1/12/05 8:12p Kurt Van Ness $

#include "StdAfx.h"
#include "FlashRecognizer.h"
#include "LedaPolygon.h"

#if defined(UseLedaLibrary)

//_____________________________________________________________________________
CLedaPolygon1::CLedaPolygon1()
{
   //m_ledaPolygon = NULL;
}

CLedaPolygon1::CLedaPolygon1(CPolygon& polygon)
{
}

CLedaPolygon1::~CLedaPolygon1()
{
   //delete m_ledaPolygon;
}

void CLedaPolygon1::addPoly(CPoly& poly)  // used
{
   poly.close();

   if (poly.getVertexCount() > 0)
   {
      leda::list<leda::rat_point> pointList;
      CPnt* pnt;
      POSITION lastPos = poly.getPntList().GetTailPosition();

      for (POSITION pos = poly.getHeadVertexPosition();pos != NULL && pos != lastPos;)
      {
         pnt = poly.getNextVertex(pos);
         leda::rat_point p(pnt->x,pnt->y);
         pointList.append(p);
      }

      leda::rat_gen_polygon ledaPolygon1(pointList);
      leda::rat_gen_polygon ledaPolygon2(m_ledaPolygon);

      //CLedaPolygonState ledaPolygon1state(ledaPolygon1);
      //CLedaPolygonState ledaPolygon2state(ledaPolygon2);

      m_ledaPolygon = ledaPolygon2.unite(ledaPolygon1);

      //CLedaPolygonState ledaPolygonState(m_ledaPolygon);

      ///////
      //if (ledaPolygonState.getSize() == 0)
      //{
      //   int iii = 3;
      //}
      //////

      if (m_ledaPolygon.size() == 0)
      {
         m_ledaPolygon = ledaPolygon2;
      }
   }
}

//void CLedaPolygon1::addPoly(CLedaPolygon1& ledaPolygon)
//{
//}

int CLedaPolygon1::getPolygonCount()  // used
{
   return 0;
}

bool CLedaPolygon1::getPolys(CPolyList& polyList)  // used
{
   bool retval = false;
   using leda::list_item;

   polyList.empty();

   CPoly* poly;
   leda::rat_polygon polygon;
   leda::rat_point point;

   forall_polygons (polygon,m_ledaPolygon)
   {
      if (polygon.is_simple())
      {
         poly = new CPoly();
         polyList.AddTail(poly);

         forall_vertices (point,polygon)
         {
            double x = point.xcoordD();
            double y = point.ycoordD();

            if (_isnan(x) || _isnan(y))
            {
               int iii = 3;
            }
            else
            {
               CPoint2d p(x,y);

               poly->addVertex(p);
            }
         }

         poly->close();
         poly->setFilled(true);
      }
   }

   return retval;
}

//_____________________________________________________________________________
CLedaPolygon::CLedaPolygon()
{
}

CLedaPolygon::CLedaPolygon(CPolygon& polygon)
{
}

CLedaPolygon::~CLedaPolygon()
{
}

void CLedaPolygon::addPoly(CPoly& poly)  // used
{
   poly.close();

   if (poly.getVertexCount() > 0)
   {
      leda::list<leda::rat_point> pointList;
      CPnt* pnt;
      POSITION lastPos = poly.getPntList().GetTailPosition();

      for (POSITION pos = poly.getHeadVertexPosition();pos != NULL && pos != lastPos;)
      {
         pnt = poly.getNextVertex(pos);
         leda::rat_point p(pnt->x,pnt->y);
         pointList.append(p);
      }

      leda::rat_gen_polygon ledaPolygon(pointList);
      m_polygonList.append(ledaPolygon);
   }
}

void CLedaPolygon::addPolygon(CPolygon& polygon)
{
   if (polygon.isValid() && polygon.getVertexCount() > 0)
   {
      leda::list<leda::rat_point> pointList;

      for (POSITION pos = polygon.getHeadPointPosition();pos != NULL;)
      {
         CPoint2d point = polygon.getNextPoint(pos);
         leda::rat_point p(point.x,point.y);
         pointList.append(p);
      }

      leda::rat_gen_polygon ledaPolygon(pointList);
      m_polygonList.append(ledaPolygon);
   }
}

//void CLedaPolygon::addPoly(CLedaPolygon1& ledaPolygon)
//{
//}

int CLedaPolygon::getPolygonCount()  // used
{
   return 0;
}

bool CLedaPolygon::getPolys(CPolyList& polyList)  // used
{
   bool retval = false;
   using leda::list_item;

   leda::rat_gen_polygon ledaPolygon(m_polygonList);

   polyList.empty();

   CPoly* poly;
   leda::rat_polygon polygon;
   leda::rat_point point;

   forall_polygons (polygon,ledaPolygon)
   {
      if (polygon.is_simple())
      {
         poly = new CPoly();
         polyList.AddTail(poly);

         forall_vertices (point,polygon)
         {
            double x = point.xcoordD();
            double y = point.ycoordD();

            if (_isnan(x) || _isnan(y))
            {
               int iii = 3;
            }
            else
            {
               CPoint2d p(x,y);

               poly->addVertex(p);
            }
         }

         poly->close();
         poly->setFilled(true);
      }
   }

   return retval;
}

//_____________________________________________________________________________
CLedaCircularEdgePolygon1::CLedaCircularEdgePolygon1()
{
   //m_ledaPolygon = NULL;
}

CLedaCircularEdgePolygon1::CLedaCircularEdgePolygon1(CPolygon& polygon)
{
}

CLedaCircularEdgePolygon1::~CLedaCircularEdgePolygon1()
{
   //delete m_ledaPolygon;
}

void CLedaCircularEdgePolygon1::addPoly(CPoly& poly)  // used
{
   poly.close();

   if (poly.getVertexCount() > 0)
   {
      leda::list<leda::r_circle_segment> segmentList;
      CPnt* prevPnt = NULL;

      for (POSITION pos = poly.getHeadVertexPosition();pos != NULL;)
      {
         CPnt* pnt = poly.getNextVertex(pos);

         if (prevPnt != NULL)
         {
            leda::rat_point p0(prevPnt->x,prevPnt->y);
            leda::rat_point p1(pnt->x    ,pnt->y    );

            if (prevPnt->bulge == 0.)
            {
               leda::r_circle_segment segment(p0,p1);
               segmentList.append(segment);
            }
            else
            {
               double cx = pnt->x - prevPnt->x;
               double cy = pnt->y - prevPnt->y;
               double ratio = prevPnt->bulge / 2.;
               double sx = ( cy) * ratio;
               double sy = (-cx) * ratio;
               double x2 = ((pnt->x + prevPnt->x)/2.) + sx; 
               double y2 = ((pnt->y + prevPnt->y)/2.) + sy; 

               leda::rat_point p2(x2,y2);
               leda::r_circle_segment segment(p0,p2,p1);
               segmentList.append(segment);
            }
         }

         prevPnt = pnt;
      }

      leda::r_circle_gen_polygon ledaPolygon1(segmentList);
      leda::r_circle_gen_polygon ledaPolygon2(m_ledaPolygon);

      CLedaPolygonState ledaPolygon1state(ledaPolygon1);
      CLedaPolygonState ledaPolygon2state(ledaPolygon2);

      m_ledaPolygon = ledaPolygon2.unite(ledaPolygon1);

      CLedaPolygonState ledaPolygonState(m_ledaPolygon);

      ///////
      if (ledaPolygonState.getSize() == 0)
      {
         int iii = 3;
      }
      //////

      if (m_ledaPolygon.size() == 0)
      {
         m_ledaPolygon = ledaPolygon2;
      }
   }
}

//void CLedaCircularEdgePolygon1::addPoly(CLedaCircularEdgePolygon1& ledaPolygon)
//{
//}

int CLedaCircularEdgePolygon1::getPolygonCount()  // used
{
   return 0;
}

bool CLedaCircularEdgePolygon1::getPolys(CPolyList& polyList)  // used
{
   bool retval = false;

   polyList.empty();

   CPoly* poly;
   leda::list<leda::r_circle_segment> ledaPointList;
   leda::r_circle_polygon circlePolygon;

   ///////
   leda::r_circle_gen_polygon::KIND kind = m_ledaPolygon.kind();
   bool trivial = m_ledaPolygon.is_trivial();
   bool empty   = m_ledaPolygon.is_empty();
   bool full    = m_ledaPolygon.is_full();
   bool simple  = m_ledaPolygon.is_simple();
   bool weak    = m_ledaPolygon.is_weakly_simple();
   int  size    = m_ledaPolygon.size();
   //////

   forall_polygons (circlePolygon,m_ledaPolygon)
   {
      if (circlePolygon.kind() == leda::r_circle_polygon::NON_TRIVIAL)
      {
         poly = new CPoly();
         polyList.AddTail(poly);

         leda::r_circle_segment circleSegment;

         int segmentIndex = 0;
         CPnt* prevPnt;
         CPnt* pnt;
         CPoint2d p0;

         forall_segments (circleSegment,circlePolygon)
         {
            if (segmentIndex == 0)
            {
               leda::r_circle_point ledaCircleSource = circleSegment.source();
               p0 = CPoint2d(ledaCircleSource.xcoord().to_double(),ledaCircleSource.ycoord().to_double());
               prevPnt = poly->addVertex(p0);
            }

            leda::r_circle_point ledaCircleTarget = circleSegment.target();
            CPoint2d p1(ledaCircleTarget.xcoord().to_double(),ledaCircleTarget.ycoord().to_double());
            pnt = poly->addVertex(p1);

            if (!circleSegment.is_straight_segment())
            {
               leda::rat_point ledaCircleCenter = circleSegment.center();
               CPoint2d pc(ledaCircleCenter.xcoordD(),ledaCircleCenter.ycoordD());

               CPoint2d p3((p0.x + p1.x)/2.,(p0.y + p1.y)/2.);
               CPoint2d pr1 = p1 - pc;
               double r = pr1.getRadius();

               CPoint2d pa = p3 - pc;
               double a = pa.getRadius();
               double s = r - a;

               CPoint2d pchord = p1 - p0;
               double chord = pchord.getRadius();

               double bulge = s / (chord / 2.);

               prevPnt->setBulge(bulge);
            }

            segmentIndex++;
            prevPnt = pnt;
            p0 = p1;
         }

         poly->close();
         poly->setFilled(true);
      }
   }

   return retval;
}

void CLedaCircularEdgePolygon1::getLedaPointList(leda::list<leda::r_circle_segment>& ledaPointList,CPoly& poly)
{
   poly.close();
   leda::rat_point *point0=NULL,*point1;
   int index = 0;

   for (POSITION pos = poly.getHeadVertexPosition();pos != NULL;index++)
   {
      CPnt* pnt = poly.getNextVertex(pos);

      if (index == 0)
      {
         point0 = new leda::rat_point((double)pnt->x,(double)pnt->y);
      }
      else
      {
         point1 = new leda::rat_point((double)pnt->x,(double)pnt->y);

         if (pnt->bulge != 0.)
         {
            leda::r_circle_segment circleSegment(*point0,*point1);
            ledaPointList.append(circleSegment);
         }

         delete point0;
         point0 = point1;
      }

      //ledaPointList.append(leda::POINT(point2d->x,point2d->y));
   }

   delete point0;
}

//_____________________________________________________________________________
CLedaCircularEdgePolygon2::CLedaCircularEdgePolygon2()
{
}

CLedaCircularEdgePolygon2::CLedaCircularEdgePolygon2(CPolygon& polygon)
{
}

CLedaCircularEdgePolygon2::~CLedaCircularEdgePolygon2()
{
}

void CLedaCircularEdgePolygon2::addPoly(CPoly& poly)  // used
{
   poly.close();

   if (poly.getVertexCount() > 0)
   {
      leda::list<leda::r_circle_segment> segmentList;
      CPnt* prevPnt = NULL;

      for (POSITION pos = poly.getHeadVertexPosition();pos != NULL;)
      {
         CPnt* pnt = poly.getNextVertex(pos);

         if (prevPnt != NULL)
         {
            leda::rat_point p0(prevPnt->x,prevPnt->y);
            leda::rat_point p1(pnt->x    ,pnt->y    );

            if (prevPnt->bulge == 0.)
            {
               leda::r_circle_segment segment(p0,p1);
               segmentList.append(segment);
            }
            else
            {
               double cx = pnt->x - prevPnt->x;
               double cy = pnt->y - prevPnt->y;
               double ratio = prevPnt->bulge / 2.;
               double sx = ( cy) * ratio;
               double sy = (-cx) * ratio;
               double x2 = ((pnt->x + prevPnt->x)/2.) + sx; 
               double y2 = ((pnt->y + prevPnt->y)/2.) + sy; 

               leda::rat_point p2(x2,y2);
               leda::r_circle_segment segment(p0,p2,p1);
               segmentList.append(segment);
            }
         }

         prevPnt = pnt;
      }

      //leda::r_circle_gen_polygon ledaPolygon1(segmentList);
      ////leda::r_circle_gen_polygon ledaPolygon2(m_ledaPolygon);

      //CLedaPolygonState ledaPolygon1state(ledaPolygon1);
      //CLedaPolygonState ledaPolygon2state(ledaPolygon2);

      //m_ledaPolygon = ledaPolygon2.unite(ledaPolygon1);

      //CLedaPolygonState ledaPolygonState(m_ledaPolygon);

      /////////
      //if (ledaPolygonState.getSize() == 0)
      //{
      //   int iii = 3;
      //}
      ////////

      //if (m_ledaPolygon.size() == 0)
      //{
      //   m_ledaPolygon = ledaPolygon2;
      //}
   }
}

//void CLedaCircularEdgePolygon2::addPoly(CLedaCircularEdgePolygon2& ledaPolygon)
//{
//}

int CLedaCircularEdgePolygon2::getPolygonCount()  // used
{
   return 0;
}

bool CLedaCircularEdgePolygon2::getPolys(CPolyList& polyList)  // used
{
   bool retval = false;

   polyList.empty();

   CPoly* poly;
   leda::list<leda::r_circle_segment> ledaPointList;
   leda::r_circle_polygon circlePolygon;

   ///////
   //leda::r_circle_gen_polygon::KIND kind = m_ledaPolygon.kind();
   //bool trivial = m_ledaPolygon.is_trivial();
   //bool empty   = m_ledaPolygon.is_empty();
   //bool full    = m_ledaPolygon.is_full();
   //bool simple  = m_ledaPolygon.is_simple();
   //bool weak    = m_ledaPolygon.is_weakly_simple();
   //int  size    = m_ledaPolygon.size();
   //////
   leda::r_circle_gen_polygon ledaPolygon;

   forall_polygons (circlePolygon,ledaPolygon)
   {
      if (circlePolygon.kind() == leda::r_circle_polygon::NON_TRIVIAL)
      {
         poly = new CPoly();
         polyList.AddTail(poly);

         leda::r_circle_segment circleSegment;

         int segmentIndex = 0;
         CPnt* prevPnt;
         CPnt* pnt;
         CPoint2d p0;

         forall_segments (circleSegment,circlePolygon)
         {
            if (segmentIndex == 0)
            {
               leda::r_circle_point ledaCircleSource = circleSegment.source();
               p0 = CPoint2d(ledaCircleSource.xcoord().to_double(),ledaCircleSource.ycoord().to_double());
               prevPnt = poly->addVertex(p0);
            }

            leda::r_circle_point ledaCircleTarget = circleSegment.target();
            CPoint2d p1(ledaCircleTarget.xcoord().to_double(),ledaCircleTarget.ycoord().to_double());
            pnt = poly->addVertex(p1);

            if (!circleSegment.is_straight_segment())
            {
               leda::rat_point ledaCircleCenter = circleSegment.center();
               CPoint2d pc(ledaCircleCenter.xcoordD(),ledaCircleCenter.ycoordD());

               CPoint2d p3((p0.x + p1.x)/2.,(p0.y + p1.y)/2.);
               CPoint2d pr1 = p1 - pc;
               double r = pr1.getRadius();

               CPoint2d pa = p3 - pc;
               double a = pa.getRadius();
               double s = r - a;

               CPoint2d pchord = p1 - p0;
               double chord = pchord.getRadius();

               double bulge = s / (chord / 2.);

               prevPnt->setBulge(bulge);
            }

            segmentIndex++;
            prevPnt = pnt;
            p0 = p1;
         }

         poly->close();
         poly->setFilled(true);
      }
   }

   return retval;
}

void CLedaCircularEdgePolygon2::getLedaPointList(leda::list<leda::r_circle_segment>& ledaPointList,CPoly& poly)
{
   poly.close();
   leda::rat_point *point0=NULL,*point1;
   int index = 0;

   for (POSITION pos = poly.getHeadVertexPosition();pos != NULL;index++)
   {
      CPnt* pnt = poly.getNextVertex(pos);

      if (index == 0)
      {
         point0 = new leda::rat_point((double)pnt->x,(double)pnt->y);
      }
      else
      {
         point1 = new leda::rat_point((double)pnt->x,(double)pnt->y);

         if (pnt->bulge != 0.)
         {
            leda::r_circle_segment circleSegment(*point0,*point1);
            ledaPointList.append(circleSegment);
         }

         delete point0;
         point0 = point1;
      }

      //ledaPointList.append(leda::POINT(point2d->x,point2d->y));
   }

   delete point0;
}

//_____________________________________________________________________________
CLedaSegmentCluster::CLedaSegmentCluster(CSegmentCluster& segmentCluster) : 
   m_segmentCluster(segmentCluster)
{
}

CLedaSegmentCluster::~CLedaSegmentCluster()
{
}

bool CLedaSegmentCluster::getPolys(CPolyList& polyList)
{
   bool retval = false;

   polyList.empty();
   CLedaPolygon rectangularPolygon;
   leda::list<leda::rat_circle> circleList;
   CPolygon linePolygon;

   CTypedPtrMap<CMapPtrToPtr,CSegmentPoly*,CSegmentPoly*> parentPolyMap;
   parentPolyMap.InitHashTable(nextPrime2n(m_segmentCluster.getList().GetCount()));
   //CSegmentPoly* junk;

   for (POSITION pos = m_segmentCluster.getList().GetHeadPosition();pos != NULL;)
   {
      CSegmentQfe* segmentQfe = m_segmentCluster.getList().GetNext(pos);
      CSegment segment = segmentQfe->getSegment();

      CSegmentPoly* parentPoly       = segment.getParentPoly();

      //if (parentPoly != NULL && parentPoly->getWidthIndex() == zeroWidthIndex)
      //{
      //   if (!parentPolyMap.Lookup(parentPoly,junk))
      //   {
      //      parentPolyMap.SetAt(parentPoly,parentPoly);
      //      segmentPolys.AddTail(parentPoly);
      //      ledaSegmentCluster.addPoly(parentPoly);
      //   }
      //}
      //else
      {
         segment.getLinePolygon(linePolygon);
         rectangularPolygon.addPolygon(linePolygon);

         double halfWidth = segment.getWidth()/2.;

         leda::rat_circle circle0(leda::rat_point(segment.get0().x,segment.get0().y),
                                 leda::rat_point(segment.get0().x,segment.get0().y + halfWidth));

         if (!circle0.is_degenerate() && !circle0.is_trivial() && !circle0.is_line())
         {
            circleList.append(circle0);
         }
         else
         {
            int iii = 3;
         }

         leda::rat_circle circle1(leda::rat_point(segment.get1().x,segment.get1().y),
                                 leda::rat_point(segment.get1().x,segment.get1().y + halfWidth));

         if (!circle1.is_degenerate() && !circle1.is_trivial() && !circle1.is_line())
         {
            circleList.append(circle1);
         }
         else
         {
            int iii = 3;
         }
      }
   }

   leda::rat_gen_polygon rectangularEdgePolygon(rectangularPolygon.getPolygonList());
   leda::r_circle_gen_polygon circularEdgePolygon(rectangularEdgePolygon);

   leda::rat_circle circle;
   int len = circleList.length();
   int ind = 0;

   forall (circle,circleList)
   {
      leda::r_circle_gen_polygon circlePolygon(circle);
      leda::r_circle_gen_polygon unitedPolygon = circularEdgePolygon.unite(circlePolygon);

      circularEdgePolygon = unitedPolygon;
      ind++;
   }

   leda::r_circle_polygon circlePolygon;

   forall_polygons (circlePolygon,circularEdgePolygon)
   {
      if (circlePolygon.kind() == leda::r_circle_polygon::NON_TRIVIAL)
      {
         CPoly* poly = new CPoly();
         polyList.AddTail(poly);

         leda::r_circle_segment circleSegment;

         int segmentIndex = 0;
         CPnt* prevPnt;
         CPnt* pnt;
         CPoint2d p0;

         forall_segments (circleSegment,circlePolygon)
         {
            if (segmentIndex == 0)
            {
               leda::r_circle_point ledaCircleSource = circleSegment.source();
               p0 = CPoint2d(ledaCircleSource.xcoord().to_double(),ledaCircleSource.ycoord().to_double());
               prevPnt = poly->addVertex(p0);
            }

            leda::r_circle_point ledaCircleTarget = circleSegment.target();
            CPoint2d p1(ledaCircleTarget.xcoord().to_double(),ledaCircleTarget.ycoord().to_double());
            pnt = poly->addVertex(p1);

            if (!circleSegment.is_straight_segment())
            {
               leda::rat_point ledaCircleCenter = circleSegment.center();
               CPoint2d pc(ledaCircleCenter.xcoordD(),ledaCircleCenter.ycoordD());

               CPoint2d p3((p0.x + p1.x)/2.,(p0.y + p1.y)/2.);
               CPoint2d pr1 = p1 - pc;
               double r = pr1.getRadius();

               CPoint2d pa = p3 - pc;
               double a = pa.getRadius();
               double s = r - a;

               CPoint2d pchord = p1 - p0;
               double chord = pchord.getRadius();

               double bulge = s / (chord / 2.);

               prevPnt->setBulge(bulge);
            }

            segmentIndex++;
            prevPnt = pnt;
            p0 = p1;
         }

         poly->close();
         poly->setFilled(true);
      }
   }

   return retval;
}

//_____________________________________________________________________________
CLedaPolygonState::CLedaPolygonState(leda::r_circle_gen_polygon& ledaCircleGenPolygon)
{
   m_kind    = ledaCircleGenPolygon.kind();
   m_trivial = ledaCircleGenPolygon.is_trivial();
   m_empty   = ledaCircleGenPolygon.is_empty();
   m_full    = ledaCircleGenPolygon.is_full();
   m_simple  = ledaCircleGenPolygon.is_simple();
   m_weak    = ledaCircleGenPolygon.is_weakly_simple();
   m_size    = ledaCircleGenPolygon.size();
}

#endif  // defined(UseAlgoCOMsPolygonLibrary)
