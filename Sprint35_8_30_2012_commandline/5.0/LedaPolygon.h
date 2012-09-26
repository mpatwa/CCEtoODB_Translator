// $Header: /CAMCAD/4.5/LedaPolygon.h 5     1/11/05 7:49p Kurt Van Ness $

#if !defined(__LedaPolygon_h__)
#define __LedaPolygon_h__

#pragma once

#if defined(UseLedaLibrary)  //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

#include "Polygon.h"

//#include <LEDA/system.h>
//#include <LEDA/sys/win32.h>
//#include <LEDA/memory.h>
//#include <LEDA/list.h>
//#include <LEDA/handle_types.h>
//#include <LEDA/internal/Preamble.h>
//#include <LEDA/core/impl/dlist.h>
#include <LEDA/geo/Rat_polygon.h>
#include <LEDA/geo/Rat_gen_polygon.h>
#include <LEDA/geo/R_circle_gen_polygon.h>
#include <LEDA/geo/Rat_kernel_names.h>
#include <LEDA/geo/Rat_Point.h>
//#include <LEDA/real.h>

class CSegmentCluster;

//_____________________________________________________________________________
// This class accumulates a united polygon as each CPoly is added
class CLedaPolygon1
{
private:
   leda::rat_gen_polygon m_ledaPolygon;

public:
   CLedaPolygon1();
   CLedaPolygon1(CPolygon& polygon);
   virtual ~CLedaPolygon1();

   virtual void addPoly(CPoly& poly);
   virtual int getPolygonCount();
   virtual bool getPolys(CPolyList& polyList);
};

//_____________________________________________________________________________
// This class maintains a list of rat_gen_polygon as each CPoly is added.
// When the resultant united poly is needed, the list is passed to the
// rat_gen_polygon for an efficient unite operation.
class CLedaPolygon
{
private:
   leda::list<leda::rat_gen_polygon> m_polygonList;

public:
   CLedaPolygon();
   CLedaPolygon(CPolygon& polygon);
   virtual ~CLedaPolygon();

   virtual void addPoly(CPoly& poly);
   virtual void addPolygon(CPolygon& polygon);
   virtual int getPolygonCount();
   virtual bool getPolys(CPolyList& polyList);
   leda::list<leda::rat_gen_polygon>& getPolygonList() { return m_polygonList; }
};

//_____________________________________________________________________________
// This class accumulates a united polygon as each poly is added
class CLedaCircularEdgePolygon1
{
private:
   leda::r_circle_gen_polygon m_ledaPolygon;

public:
   CLedaCircularEdgePolygon1();
   CLedaCircularEdgePolygon1(CPolygon& polygon);
   ~CLedaCircularEdgePolygon1();

   void addPoly(CPoly& poly);
   //void addPoly(CLedaPolygon1& ledaPolygon);

   int getPolygonCount();

   bool getPolys(CPolyList& polyList);

private:
   void getLedaPointList(leda::list<leda::r_circle_segment>& ledaPointList,CPoly& poly);
};

//_____________________________________________________________________________
// This class was supposed to use an r_circle_gen_polygon constructor to efficiently
// unite a list of circular edge polygons.  Since LEDA does not yet support such a constructor,
// this class will have to be implemented in the future.
class CLedaCircularEdgePolygon2
{
private:
   leda::list<leda::r_circle_gen_polygon> m_polygonList;

public:
   CLedaCircularEdgePolygon2();
   CLedaCircularEdgePolygon2(CPolygon& polygon);
   ~CLedaCircularEdgePolygon2();

   void addPoly(CPoly& poly);
   //void addPoly(CLedaPolygon1& ledaPolygon);

   int getPolygonCount();

   bool getPolys(CPolyList& polyList);

private:
   void getLedaPointList(leda::list<leda::r_circle_segment>& ledaPointList,CPoly& poly);
};

//_____________________________________________________________________________
// This class maintains a CLedaPolygon and a list of r_circle_gen_polygon as each 
// CSegment is added.  For rouned segments, the circular components are added to the list, 
// and the rectangular component is added to the CLedaPolygon.
// For rectangular segments, the segment is added to the CLedaPolygon.
// When the resultant united poly is needed, the CLedaPolygon is used to get the initial
// result and then the individual circle segments are appened from the list.
class CLedaSegmentCluster
{
private:
   CSegmentCluster& m_segmentCluster;

public:
   CLedaSegmentCluster(CSegmentCluster& segmentCluster);
   ~CLedaSegmentCluster();

   int getPolygonCount();

   bool getPolys(CPolyList& polyList);

private:
};

//_____________________________________________________________________________
class CLedaPolygonState
{
private:
   leda::r_circle_gen_polygon::KIND m_kind;
   bool m_trivial;
   bool m_empty;
   bool m_full;
   bool m_simple;
   bool m_weak;
   int  m_size;

public:
   CLedaPolygonState(leda::r_circle_gen_polygon& ledaCircleGenPolygon);

   int getSize() const { return m_size; }
};

#endif  // defined(UseLedaLibrary)
#endif  // __LedaPolygon_h__
