// $Header: /CAMCAD/4.3/Pcb_Net.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-2000. All Rights Reserved.
*/

#pragma once

#include <afxtempl.h>

#define RESULT_FOUND       1
#define RESULT_GIVEUP      2
#define RESULT_UNCONNECTED 3
#define RESULT_HITUSED     4

// ENDPOINT
struct Net_Endpoint
{
   int      id;            // id 
   double   x, y;          // position
   UINT64      layermap;      // bitmap representing layers for this endpoint
   BOOL done;              // do not search from this endpoint anymore
};
typedef CTypedPtrList<CPtrList, Net_Endpoint*> CEndpointList;

// VIA
struct Net_Via
{
   int      id;            // id
   double   x, y;          // position
   UINT64   layermap;      // bitmap representing layers for this via
   char used;
};
typedef CTypedPtrArray<CPtrArray, Net_Via*> CViaArray;

// SIMPLE SEGMENT 
struct Net_Segment
{
   double   x1, y1, x2, y2;         // endpoints of segment
   char     layer;                  // layer this segment is on
   short    widthIndex;             // pass through widthIndex
   char     used;
};
typedef CTypedPtrArray<CPtrArray, Net_Segment*> CSegmentArray;

// PATH between endpoints is an array of segments and vias
#define VIA_NODE        1
#define SEGMENT_NODE    2
#define ENDPOINT_NODE   3
struct Net_Path
{
   char type;
   int index_id;
   BOOL reverse; // segment goes from p2 to p1
};
typedef CTypedPtrList<CPtrList, Net_Path*> CPathList;

struct Net_Result
{
   CPathList *path;
};
typedef CTypedPtrList<CPtrList, Net_Result*> CResultList;   // list of paths from endpoint to endpoint

void SetMarginTrace(double search_margin);
void SetNetTrace_ConnectUnusedToEndpoints(BOOL connect);    // FALSE means that unused traces are not connected 
                                                            // to pins, TRUE means unused and this could lead to
                                                            // "ring connections" 
void ClearTrace();
CResultList *FindShortestNetList();
int  LoadEndpoint(int id, double x, double y, UINT64 layermap);
int  LoadSegment(double x1, double y1, double x2, double y2, char layer, short widthIndex);
int  LoadPoly(CPoly *poly, char layer, short widthIndex);
int  LoadVia(int id, double x, double y, UINT64 layermap);

void DumpResult(FILE *stream, CResultList *resultList);
  
void Write_Traces(FILE *stream, CResultList *resultList, const char *netname);

// end PCBUTIL.H
