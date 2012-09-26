// $Header: /CAMCAD/4.4/Pcb_Net.cpp 12    3/13/04 3:58p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-97. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include "data.h"
#include "pcb_net.h"

// if we do not care about an optimized output, keep smart at 0.
#define SMART  0           // SMART 1: cleares used flags of visited segments and vias to
                           // exhaust a search (always find the shortest), 
                           // SMART 0: do not exhaust a grid.

static double MARGIN = 0.0001;
static BOOL CONNECT = TRUE;
static CEndpointList *openEndpoints, *usedEndpoints;
CViaArray *viaArray;
static int maxViaArray;
CSegmentArray *segmentArray;
static int maxSegmentArray;
static CResultList *resultList;

static int FindOpenEndpoint(double fromX, double fromY, UINT64 layermap, int steps, CPathList *pathList, BOOL FirstCall);
static int TryThreshold(int steps, Net_Endpoint *endpoint, CPathList **best);
static void AddUnconnectedRoutes();
static BOOL AddUnusedToPath(CPathList *pathList, double fromX, double fromY, UINT64 layermap, BOOL Head);
static BOOL ConnectToGoodPath(CPathList *pathList, BOOL Head);
static BOOL SimpleConnect(CPathList *goodPath, CPathList *unconnectedPath, double x, double y, UINT64 layermap, BOOL Head);
static void FixPathsWithNoEndpoint();
static BOOL ComplexConnect(CPathList *goodPath, CPathList *unconnectedPath, POSITION *uncPos);
static CPathList *FindJunction(CPathList *goodPath, double x, double y, UINT64 layermap);
static void FreePath(CPathList *path);
static CPathList *CopyPath(CPathList *path);
static void RemoveDuplicated();
static BOOL Duplicated(CPathList *smaller, CPathList *larger);
static void ClearUsedFlags();

/* 
Function Calls

InitTrace() - Called once for whole program in CAMCAD.CPP InitInstance()

ClearTrace()

LoadEndpoint()
LoadPoly()
{
   LoadSegment()
}
LoadVia()

FindShortestNetList()
{
   FindOpenEndpoint()

   ClearUsedFlags()

   AddUnconnectedRoutes()
   {
      ClearUsedFlags()
      AddUnusedToPath() - called twice each loop
      {
         ConnectToGoodPath()
         {
            SimpleConnect()
         }
         AddUnusedToPath() - recursive
      }
   }
   
   FixPathsWithNoEndpoint()
   {
      ComplexConnect()
      {
         FindJunction()
      }
   }
   
   RemoveDuplicated()
}

FreeTrace() - Called once for whole program
*/

/******************************************************************************
* SetMarginTrace
*/
void SetMarginTrace(double m)
{
   MARGIN = m;
}

/******************************************************************************
* SetNetTrace_ConnectUnusedToEndpoints
*/
void SetNetTrace_ConnectUnusedToEndpoints(BOOL connect)
{
   CONNECT = connect;
}

/******************************************************************************
* InitTrace
*/
void InitTrace()
{
   openEndpoints = new CEndpointList;
   usedEndpoints = new CEndpointList;
   viaArray = new CViaArray;
   viaArray->SetSize(100, 100);
   maxViaArray = 0;
   segmentArray = new CSegmentArray;
   segmentArray->SetSize(100, 100);
   maxSegmentArray = 0;
   resultList = new CResultList;
}

/******************************************************************************
* ClearTrace
*/
void ClearTrace()
{
   POSITION pos;
   int i;

   if (openEndpoints)
   {
      pos = openEndpoints->GetHeadPosition();
      while (pos != NULL)
         delete openEndpoints->GetNext(pos);
      openEndpoints->RemoveAll();
   }

   if (usedEndpoints)
   {
      pos = usedEndpoints->GetHeadPosition();
      while (pos != NULL)
         delete usedEndpoints->GetNext(pos);
      usedEndpoints->RemoveAll();
   }

   if (viaArray)
   {
      for (i=0; i<maxViaArray; i++)
         delete viaArray->GetAt(i);
      viaArray->RemoveAll();
      maxViaArray = 0;
   }

   if (segmentArray)
   {
      for (i=0; i<maxSegmentArray; i++)
         delete segmentArray->GetAt(i);
      segmentArray->RemoveAll();
      maxSegmentArray = 0;
   }

   if (resultList)
   {
      pos = resultList->GetHeadPosition();
      while (pos != NULL)
      {
         Net_Result *res = resultList->GetNext(pos);
         POSITION pathPos = res->path->GetHeadPosition();
         while (pathPos != NULL)
         {
            Net_Path *np = res->path->GetNext(pathPos);
            delete np;
         }
         delete res->path;
         delete res;
      }
      resultList->RemoveAll();
   }
}

/******************************************************************************
* FreeTrace
*/
void FreeTrace()
{
   ClearTrace();

   delete openEndpoints;
   delete usedEndpoints;
   delete viaArray;
   delete segmentArray;
   delete resultList;
}

/******************************************************************************
* LoadEndpoint
*/
int LoadEndpoint(int id, double x, double y, UINT64 layermap)
{
   Net_Endpoint *endpoint = new Net_Endpoint;
   endpoint->id = id;
   endpoint->x = x;
   endpoint->y = y;
   endpoint->layermap = layermap;
   endpoint->done = FALSE;

   openEndpoints->AddTail(endpoint);

   return 0;
}


/******************************************************************************
* LoadSegment
*/
// errors are OR
// return 0 for OK
// return 1 for NULL segment
// return 2 for out of range layer
int LoadSegment(double x1, double y1, double x2, double y2, char layer, short widthIndex)
{
   if (layer < 1 || layer > 32)
      return 2;

   if (fabs(x1 - x2) < MARGIN && fabs(y1 - y2) < MARGIN)
      return 1;

   Net_Segment *segment = new Net_Segment;
   segment->x1 = x1;
   segment->y1 = y1;
   segment->x2 = x2;
   segment->y2 = y2;
   segment->layer = layer;
   segment->widthIndex = widthIndex;
   segment->used = FALSE;

   segmentArray->SetAtGrow(maxSegmentArray++, segment);
   return 0;
}

/******************************************************************************
* LoadPoly
*/
int LoadPoly(CPoly *poly, char layer, short widthIndex)
{
   int   err = 0;
   if (layer < 1 || layer > 32)
      return 1;

   CPnt *pnt, *lastPnt;
   POSITION pos = poly->getPntList().GetHeadPosition();
   if (pos != NULL)
      lastPnt = poly->getPntList().GetNext(pos);
   while (pos != NULL)
   {
      pnt = poly->getPntList().GetNext(pos);
      err |= LoadSegment(lastPnt->x, lastPnt->y, pnt->x, pnt->y, layer, widthIndex);
      lastPnt = pnt;
   }
   return err;
}

/******************************************************************************
* LoadVia
*/
int LoadVia(int id, double x, double y, UINT64 layermap)
{
   Net_Via *via = new Net_Via;
   via->id = id;
   via->x = x;
   via->y = y;
   via->layermap = layermap;
   via->used = FALSE;

   viaArray->SetAtGrow(maxViaArray++, via);
   return 0;
}

//#define DUMP
#undef DUMP

/******************************************************************************
* FindShortestNetList
*  - Connects Traces intelligently
*/
CResultList *FindShortestNetList()
{
   if (openEndpoints->IsEmpty())
      return NULL;

   // to start, move first open endpoint to used
   Net_Endpoint *endpoint = openEndpoints->GetHead();
   usedEndpoints->AddHead(endpoint);
   openEndpoints->RemoveHead();

   while (!openEndpoints->IsEmpty()) // while there are open endpoints
   {
      // for each used endpoint, recusively look up to n steps (n could be infinite) to find an open endpoint
      // n starts out -1, n then is set to last max steps to only find shorter nets
      CPathList *best = NULL;

      // go through assuming there exists a connection less than a threshold
/*    if (TryThreshold(20, endpoint, &best) != RESULT_FOUND)
      {
         if (!SMART) 
            ClearUsedFlags();*/
         TryThreshold(-1, endpoint, &best);
//    }

      if (!SMART) 
         ClearUsedFlags();

      // if no connections between used endpoints and open endpoints,
      // move an open to used
      // put endpoint by itself in result list
      if (best == NULL)
      {
         Net_Endpoint *endpoint = openEndpoints->GetHead();
         usedEndpoints->AddHead(endpoint);
         openEndpoints->RemoveHead();
   
         CPathList *pathList = new CPathList;
         Net_Path *node = new Net_Path;
         node->type = ENDPOINT_NODE;
         node->index_id = endpoint->id;
         pathList->AddHead(node);
         Net_Result *resultPath = new Net_Result;
         resultPath->path = pathList;
         resultList->AddTail(resultPath);

         continue;
      }
      else // found a short connection between open and used endpoints
      {
         // move found endpoint from open to used
         Net_Path *node = best->GetTail();
         POSITION pos = openEndpoints->GetHeadPosition();
         while (pos)
         {
            endpoint = openEndpoints->GetAt(pos);
            if (endpoint->id == node->index_id)
            {
               openEndpoints->RemoveAt(pos);
               break;
            }
            openEndpoints->GetNext(pos);
         }
         usedEndpoints->AddHead(endpoint);
         
         // add path to result list
         Net_Result *resultPath = new Net_Result;
         resultPath->path = best;
         resultList->AddTail(resultPath);
      }
   }

#ifdef DUMP
FILE *dump;
dump = fopen("c:\\temp\\dump1.txt", "w");
if (dump)
{
   DumpResult(dump, resultList);
   fclose(dump);
}
#endif

   AddUnconnectedRoutes();

#ifdef DUMP
dump = fopen("c:\\temp\\dump2.txt", "w");
if (dump)
{
   DumpResult(dump, resultList);
   fclose(dump);
}
#endif

   if (CONNECT)
      FixPathsWithNoEndpoint();

#ifdef DUMP
   dump = fopen("c:\\temp\\dump3.txt", "w");
if (dump)
{
   DumpResult(dump, resultList);
   fclose(dump);
}
#endif

   RemoveDuplicated();  

   return resultList;
}

/******************************************************************************
* TryThreshold
*/
int TryThreshold(int steps, Net_Endpoint *endpoint, CPathList **best)
{
   int res = RESULT_UNCONNECTED;

   POSITION pos = usedEndpoints->GetHeadPosition();
   while (pos)
   {
      endpoint = usedEndpoints->GetNext(pos);

      if (endpoint->done)
         continue;

      CPathList *pathList = new CPathList; 

      if ((res = FindOpenEndpoint(endpoint->x, endpoint->y, endpoint->layermap, steps, pathList, TRUE)) == RESULT_FOUND)
      {
         // put endpoint as first node in path
         Net_Path *node = new Net_Path;
         node->type = ENDPOINT_NODE;
         node->index_id = endpoint->id;
         pathList->AddHead(node);

         // free old best
         if ((*best) != NULL) 
         {
            POSITION tempPos = (*best)->GetHeadPosition();
            while (tempPos)
               delete (*best)->GetNext(tempPos);
            delete (*best); // free old best
         }
         (*best) = pathList;
         steps = pathList->GetCount();
      }
      else
      {
         delete pathList;
         if (res == RESULT_UNCONNECTED) // this endpoint can not connect to any open endpoints
            endpoint->done = TRUE;
      }
   }

   return res;
}

/******************************************************************************
* FindOpenEndpoint
*  - Recursively follows segments and vias to find an open endpoint
*
*  PARAMETERS:
*     fromX, fromY   - X & Y of current location
*     layermap       - current layermap
*     steps          - number of steps until give up (could be -1 for never give up)
*     pathList       - 
*     FirstCall      - TRUE when called from outside, FALSE when called itself
*
*  RETURNS:
*     RESULT_FOUND, RESULT_GIVEUP, RESULT_UNCONNECTED 
*
*  ALGORITHM:
*/
int FindOpenEndpoint(double fromX, double fromY, UINT64 layermap, int steps, CPathList *pathList, BOOL FirstCall)
{
   BOOL GaveUp = FALSE; // if we have given up since had a shorter path, need to not mark endpoint as done
   int res;

   // loop through used Endpoints to see if encounter one (therefore, this would not be a short path so we can give up)
   if (!FirstCall) // don't find the endpoint we started at
   {
      POSITION pos = usedEndpoints->GetHeadPosition();
      while (pos != NULL)
      {
         Net_Endpoint *endpoint = usedEndpoints->GetNext(pos);
         if ((endpoint->layermap & layermap) && fabs(endpoint->x - fromX) < MARGIN && fabs(endpoint->y - fromY) < MARGIN)
         {
            return RESULT_HITUSED; // found a used endpoint
         }
      }
   }

   // loop through open Endpoints to see if we found one
   POSITION pos = openEndpoints->GetHeadPosition();
   while (pos != NULL)
   {
      Net_Endpoint *endpoint = openEndpoints->GetNext(pos);
      if ((endpoint->layermap & layermap) && fabs(endpoint->x - fromX) < MARGIN && fabs(endpoint->y - fromY) < MARGIN)
      {
         Net_Path* node = new Net_Path;
         node->type = ENDPOINT_NODE;
         node->index_id = endpoint->id;
         pathList->AddHead(node);
         return RESULT_FOUND; // found an endpoint
      }
   }

   if (steps == 1) // give up because already have one with this many steps 
      return RESULT_GIVEUP; // give up on steps

   // loop through segments to find one not in the array that hits this point
	int i=0;
   for (i=0; i<maxSegmentArray; i++)
   {
      Net_Segment *seg = segmentArray->GetAt(i);
      UINT64 baseVal = 1L;
      // if matching segment
      if (!seg->used && ((baseVal << (seg->layer-1)) & layermap) && 
         ((fabs(seg->x1 - fromX) < MARGIN && fabs(seg->y1 - fromY) < MARGIN) ||
          (fabs(seg->x2 - fromX) < MARGIN && fabs(seg->y2 - fromY) < MARGIN)))
      {
         seg->used = TRUE;

         // fromX and fromY should be other end of segment
         double newFromX, newFromY;
         BOOL reverse;
         if (fabs(seg->x1 - fromX) < MARGIN && fabs(seg->y1 - fromY) < MARGIN)
         {
            newFromX = seg->x2;
            newFromY = seg->y2;
            reverse = FALSE;
         }
         else
         {
            newFromX = seg->x1;
            newFromY = seg->y1;
            reverse = TRUE;
         }

         res = FindOpenEndpoint(newFromX, newFromY, (UINT64)(1L << (seg->layer-1)), (steps==-1?-1:steps-1), pathList, FALSE);
         if (res == RESULT_FOUND)
         {
            if (SMART) seg->used = FALSE;
            Net_Path *node = new Net_Path;
            node->type = SEGMENT_NODE;
            node->index_id = i;
            node->reverse = reverse;
            pathList->AddHead(node);
            return RESULT_FOUND; // found endpoint using this segment in recursion
         }
         if (res == RESULT_GIVEUP)
            GaveUp = TRUE;

         if (SMART)
            seg->used = FALSE;
      }
   }

   // loop through segments to find one not in the array that hits this point
   for (i=0; i<maxViaArray; i++)
   {
      Net_Via *via = viaArray->GetAt(i);

      // if matching segment
      if (!via->used && (via->layermap & layermap) && fabs(via->x - fromX) < MARGIN && fabs(via->y - fromY) < MARGIN)
      {
         via->used = TRUE;

         res = FindOpenEndpoint(fromX, fromY, via->layermap, 
               (steps==-1?-1:steps-1), pathList, FALSE);
         if (res == RESULT_FOUND)
         {
            via->used = FALSE;
            Net_Path *node = new Net_Path;
            node->type = VIA_NODE;
            node->index_id = i;
            pathList->AddHead(node);
            return RESULT_FOUND; // found endpoint using this segment in recursion
         }
         if (res == RESULT_GIVEUP)
            GaveUp = TRUE;

         if (SMART)
            via->used = FALSE;
      }
   }

   if (GaveUp)
      return RESULT_GIVEUP;
   else
      return RESULT_UNCONNECTED; // dead end traces because no segments, vias, or open endpoints at this fromX & fromY (may be loop)
}

/******************************************************************************
* AddUnconnectedRoutes
*/
void AddUnconnectedRoutes()
{
   ClearUsedFlags();

   // loop results marking used segments and vias 
   Net_Path *node;
   CPathList *path;
   POSITION resPos, pathPos;
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      path = resultList->GetNext(resPos)->path;

      // loop nodes of this path
      pathPos = path->GetHeadPosition();
      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
         case ENDPOINT_NODE:
            break;
         case SEGMENT_NODE:
            segmentArray->GetAt(node->index_id)->used = TRUE;
            break;
         case VIA_NODE:
            viaArray->GetAt(node->index_id)->used = TRUE;
            break;
         }
      }
   }

   // if there are still unused segments
   int i = 0;
   Net_Segment *seg;
   while(i<maxSegmentArray)
   {
      // loop unused segments
      seg = segmentArray->GetAt(i);
   
      if (!seg->used)
      {
         seg->used = TRUE;

         // new path with this segment
         CPathList *pathList = new CPathList;
         Net_Path *node = new Net_Path;
         node->type = SEGMENT_NODE;
         node->index_id = i;
         node->reverse = 0;
         pathList->AddHead(node);

         // link this unused segment to other unused items
         if (!AddUnusedToPath(pathList, seg->x1, seg->y1, 1L << (seg->layer-1), TRUE))
            AddUnusedToPath(pathList, seg->x2, seg->y2, 1L << (seg->layer-1), FALSE);

         // add path to result list
         Net_Result *resultPath = new Net_Result;
         resultPath->path = pathList;
         resultList->AddTail(resultPath);
      }
      i++;
   }

   // if there are still unused vias
   i = 0;
   Net_Via *via;
   while(i<maxViaArray)
   {
      // loop unused segments
      via = viaArray->GetAt(i);
   
      if (!via->used)
      {
         via->used = TRUE;

         // new path with this segment
         CPathList *pathList = new CPathList;
         Net_Path *node = new Net_Path;
         node->type = VIA_NODE;
         node->index_id = i;
         pathList->AddHead(node);

         // link this unused via to other unused items
         if (!AddUnusedToPath(pathList, via->x, via->y, via->layermap, TRUE))
            AddUnusedToPath(pathList, via->x, via->y, via->layermap, FALSE);

         // add path to result list
         Net_Result *resultPath = new Net_Result;
         resultPath->path = pathList;
         resultList->AddTail(resultPath);
      }
      i++;
   }
}

/******************************************************************************
* AddUnusedToPath
*  - checks if any unused entities can be tacked on to one end of path and adds them
*/
BOOL AddUnusedToPath(CPathList *pathList, double fromX, double fromY, UINT64 layermap,
                BOOL Head)
{
   // see if we hit an endpoint
   POSITION pos = usedEndpoints->GetHeadPosition();
   while (pos != NULL)
   {
      Net_Endpoint *endpoint = usedEndpoints->GetNext(pos);
      if (((endpoint->layermap & layermap) && 
         fabs(endpoint->x - fromX) < MARGIN && fabs(endpoint->y - fromY) < MARGIN))
      {
         Net_Path *node = new Net_Path;
         node->type = ENDPOINT_NODE;
         node->index_id = endpoint->id;
         if (Head)
            pathList->AddHead(node);
         else
            pathList->AddTail(node);
         return TRUE;
      }
   }

   // see if hit a good path
   if (CONNECT)
      if (ConnectToGoodPath(pathList, Head))
         return TRUE;

   // see if hit an unused segment
   int i = 0;
   while(i<maxSegmentArray)
   {
      // loop unused segments
      Net_Segment *seg = segmentArray->GetAt(i);
      UINT64 baseVal = 1L;

      if (!seg->used && 
         ((baseVal << (seg->layer-1)) & layermap) && 
         ((fabs(seg->x1 - fromX) < MARGIN && fabs(seg->y1 - fromY) < MARGIN) ||
         (fabs(seg->x2 - fromX) < MARGIN && fabs(seg->y2 - fromY) < MARGIN)))
      {
         seg->used = TRUE;

         Net_Path *node = new Net_Path;
         node->type = SEGMENT_NODE;
         node->index_id = i;
         if (Head)
            pathList->AddHead(node);
         else
            pathList->AddTail(node);

         if (fabs(seg->x1 - fromX) < MARGIN && fabs(seg->y1 - fromY) < MARGIN)
         {
            node->reverse = Head;
            if (AddUnusedToPath(pathList, seg->x2, seg->y2, (UINT64)(1L << (seg->layer-1)), Head))
               return TRUE;
         }
         else // x2, y2
         {
            node->reverse = !Head;
            if (AddUnusedToPath(pathList, seg->x1, seg->y1, (UINT64)(1L << (seg->layer-1)), Head))
               return TRUE;
         }
         return FALSE;
      }
   
      i++;
   }     

   // see if hit an unused via
   i = 0;
   while(i<maxViaArray)
   {
      // loop unused vias
      Net_Via *via = viaArray->GetAt(i);
   
      if (!via->used && (via->layermap & layermap) && 
         fabs(via->x - fromX) < MARGIN && fabs(via->y - fromY) < MARGIN)
      {
         via->used = TRUE;

         // new path with this via
         Net_Path *node = new Net_Path;
         node->type = VIA_NODE;
         node->index_id = i;
         if (Head)
            pathList->AddHead(node);
         else
            pathList->AddTail(node);

         AddUnusedToPath(pathList, via->x, via->y, via->layermap, Head);
         return FALSE;
      }

      i++;
   }     

   return FALSE;
}

/***************************************************************************************
* ConnectToGoodPath
*
* try to connect a path that does not contain an endpoint to a path that does contain an endpoint
*
* takes an unconnected path and compares it with good results to find a connection to an endpoint
*/
BOOL ConnectToGoodPath(CPathList *unconnectedPath, BOOL Head)
{
   POSITION resPos;
   CPathList *goodPath;

   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      goodPath = resultList->GetNext(resPos)->path;

      // only check good candidates for finding a path to an endpoint
      if (goodPath->GetCount() < 2 || (goodPath->GetHead()->type != ENDPOINT_NODE && goodPath->GetTail()->type != ENDPOINT_NODE))
         continue;

      // try simple connect with this good path
      Net_Path *node;
      if (Head)
         node = unconnectedPath->GetHead(); 
      else
         node = unconnectedPath->GetTail(); 
      double x, y;
      UINT64  layermap;
      UINT64  baseVal = 1L;

      if (node->type == VIA_NODE) // node is VIA
      {
         Net_Via *via = viaArray->GetAt(node->index_id);
         x = via->x;
         y = via->y;
         layermap = via->layermap;
      }
      else // node is SEGMENT
      {
         Net_Segment *seg = segmentArray->GetAt(node->index_id);
         if (node->reverse ^ Head)
         {
            x = seg->x1;
            y = seg->y1;
         }
         else
         {
            x = seg->x2;
            y = seg->y2;
         }
         layermap = baseVal << (seg->layer-1);
      }

      if (SimpleConnect(goodPath, unconnectedPath, x, y, layermap, Head))
         return TRUE;
   }
   return FALSE;
}

/******************************************************************************
* SimpleConnect
*  - compares ends of unconnected path to items of good path
*/
BOOL SimpleConnect(CPathList *goodPath, CPathList *unconnectedPath,
                   double x, double y, UINT64 layermap, BOOL Head)
{
   POSITION pos;
   Net_Path *node, *newNode;

   // loop good entities
   pos = goodPath->GetHeadPosition();
   while (pos != NULL)
   {
      node = goodPath->GetAt(pos);

      switch (node->type)
      {
      case ENDPOINT_NODE:
         break;
      case SEGMENT_NODE:
         {
            Net_Segment *seg = segmentArray->GetAt(node->index_id);
            double segX2, segY2;
            if (!node->reverse)
            {
               segX2 = seg->x2;
               segY2 = seg->y2;
            }
            else
            {
               segX2 = seg->x1;
               segY2 = seg->y1;
            }
            UINT64 baseVal = 1L;
            // if second point matches with head of unconnected path
            if ( ((baseVal << (seg->layer-1)) & layermap) && 
                 (fabs(segX2 - x) < MARGIN && fabs(segY2 - y) < MARGIN) )
            {
               // add nodes of goodPath from pos back to head
               if (goodPath->GetHead()->type == ENDPOINT_NODE)
               {
                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     if (Head)
                     {
                        unconnectedPath->AddHead(newNode);
                     }
                     else
                     {
                        if (node->type == SEGMENT_NODE)
                           newNode->reverse = !node->reverse;
                        unconnectedPath->AddTail(newNode);
                     }
                     goodPath->GetPrev(pos);
                  }
               }
               // add nodes of goodPath from pos up to tail
               else
               {
                  goodPath->GetNext(pos);

                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     if (Head)
                     {
                        if (node->type == SEGMENT_NODE)
                           newNode->reverse = !node->reverse;
                        unconnectedPath->AddHead(newNode);
                     }
                     else
                     {
                        unconnectedPath->AddTail(newNode);
                     }
                     goodPath->GetNext(pos);
                  }
               }
               return TRUE;
            }
         }
         break;
      case VIA_NODE:
         {
            Net_Via *via = viaArray->GetAt(node->index_id);

            if ((via->layermap & layermap) && 
               fabs(via->x - x) < MARGIN && fabs(via->y - y) < MARGIN)
            {
               // add nodes to connect to HEAD of goodPath
               if (goodPath->GetHead()->type == ENDPOINT_NODE)
               {
                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     if (Head)
                     {
                        unconnectedPath->AddHead(newNode);
                     }
                     else
                     {
                        if (node->type == SEGMENT_NODE)
                           newNode->reverse = !node->reverse;
                        unconnectedPath->AddTail(newNode);
                     }
                     goodPath->GetPrev(pos);
                  }
               }
               // add nodes to connect to TAIL of goodPath
               else
               {
                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     if (Head)
                     {
                        if (node->type == SEGMENT_NODE)
                           newNode->reverse = !node->reverse;
                        unconnectedPath->AddHead(newNode);
                     }
                     else
                     {
                        unconnectedPath->AddTail(newNode);
                     }
                     goodPath->GetNext(pos);
                  }
               }
               return TRUE;
            }
         }
         break;
      }
      goodPath->GetNext(pos);
   }
   return FALSE;
}

/******************************************************************************
* FixPathsWithNoEndpoint
*     - Mark good path entities as used
*     - Loop good paths
*        - Loop unconnected paths for junction
*        - Make new good path and mark as used
*/
void FixPathsWithNoEndpoint()
{
   POSITION resPos, uncPos;
   CPathList *path, *goodPath, *uncPath;
   int unconnectedPaths = 0;

   // clear used flags
   ClearUsedFlags();

   // mark good path entities as used
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      path = resultList->GetNext(resPos)->path;

      // loop results marking good path entities as used 
      if (path->GetHead()->type == ENDPOINT_NODE || path->GetTail()->type == ENDPOINT_NODE)
      {
         Net_Path *node;
         POSITION pathPos;

         // loop nodes of this path
         pathPos = path->GetHeadPosition();
         while (pathPos != NULL)
         {
            node = path->GetNext(pathPos);
            switch (node->type)
            {
            case ENDPOINT_NODE:
               break;
            case SEGMENT_NODE:
               segmentArray->GetAt(node->index_id)->used = TRUE;
               break;
            case VIA_NODE:
               viaArray->GetAt(node->index_id)->used = TRUE;
               break;
            }
         }
      }
      else
         unconnectedPaths++;
   }

   if (!unconnectedPaths)
      return; // Done

   // loop good results and check for junction with unconnected path
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL && unconnectedPaths)
   {
      goodPath = resultList->GetAt(resPos)->path;

      if (goodPath->GetHead()->type == ENDPOINT_NODE || goodPath->GetTail()->type == ENDPOINT_NODE) // good paths only
      {
         // loop unconnected paths
         uncPos = resultList->GetHeadPosition();
         while (uncPos != NULL)
         {
            uncPath = resultList->GetAt(uncPos)->path;
            if (uncPath->GetHead()->type != ENDPOINT_NODE && uncPath->GetTail()->type != ENDPOINT_NODE) // unconnected paths only
            {
               // look for junction
               // break uncPath in two and connect
               // add new paths to end of resultList
               if (ComplexConnect(goodPath, uncPath, &uncPos))
               {
                  unconnectedPaths--;
               }
               else
                  resultList->GetNext(uncPos);
            }
            else
               resultList->GetNext(uncPos);
         }
      }
      resultList->GetNext(resPos);
   }
}

/******************************************************************************
* ComplexConnect
*  - look for junction
*  - break uncPath in two and connect
*  - add new paths to end of resultList
*/
BOOL ComplexConnect(CPathList *goodPath, CPathList *unconnectedPath, POSITION *uncPos)
{
   POSITION pos, tempPos;
   Net_Path *node;
   CPathList *newPath;

   // loop unconnected entities
   pos = unconnectedPath->GetHeadPosition();
   while (pos != NULL)
   {
      node = unconnectedPath->GetAt(pos);

      switch (node->type)
      {
      case ENDPOINT_NODE:
         break;
      case SEGMENT_NODE:
         {
            Net_Segment *seg = segmentArray->GetAt(node->index_id);
            double segX1, segY1, segX2, segY2;
            if (!node->reverse)
            {
               segX1 = seg->x1;
               segY1 = seg->y1;
               segX2 = seg->x2;
               segY2 = seg->y2;
            }
            else
            {
               segX1 = seg->x2;
               segY1 = seg->y2;
               segX2 = seg->x1;
               segY2 = seg->y1;
            }

            // if segment is first entity
            if (pos == unconnectedPath->GetHeadPosition() &&
                  (newPath = FindJunction(goodPath, segX1, segY1, 1L << (seg->layer-1))))
            {
               // connect unconnectedPath to new path
               Net_Path *newNode;
               POSITION tempPos = newPath->GetTailPosition();
               while (tempPos != NULL)
               {
                  node = newPath->GetPrev(tempPos);
                  newNode = new Net_Path;
                  memcpy(newNode, node, sizeof(Net_Path));
                  unconnectedPath->AddHead(newNode);
               }

               // put unconnectedPath at end of resultList
               resultList->AddTail(resultList->GetAt(*uncPos));
               tempPos = *uncPos;
               resultList->GetNext(*uncPos);
               resultList->RemoveAt(tempPos);

               FreePath(newPath);
               return TRUE;
            }

            if (newPath = FindJunction(goodPath, segX2, segY2, 1L << (seg->layer-1)))
            {
               // if segment is last entity
               if (pos == unconnectedPath->GetTailPosition())
               {
                  // connect unconnectedPath to new path
                  Net_Path *newNode;
                  POSITION tempPos = newPath->GetTailPosition();
                  while (tempPos != NULL)
                  {
                     node = newPath->GetPrev(tempPos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     newNode->reverse = !newNode->reverse;
                     unconnectedPath->AddTail(newNode);
                  }
   
                  // put unconnectedPath at end of resultList
                  resultList->AddTail(resultList->GetAt(*uncPos));
                  tempPos = *uncPos;
                  resultList->GetNext(*uncPos);
                  resultList->RemoveAt(tempPos);
   
                  FreePath(newPath);
                  return TRUE;
               }

               // middle of list
               CPathList *copy = CopyPath(newPath);

               // connect first half of unconnectedPath to newPath
               while (pos != NULL)
               {
                  node = unconnectedPath->GetAt(pos);
                  node->reverse = !node->reverse;
                  newPath->AddTail(node);
                  tempPos = pos;
                  unconnectedPath->GetPrev(pos);
                  unconnectedPath->RemoveAt(tempPos);
               }

               // add newPath to tail of resultList
               Net_Result *res = new Net_Result;
               res->path = newPath;
               resultList->AddTail(res);

               // connect second half of unconnectedPath to copy
               while (unconnectedPath->GetHeadPosition() != NULL)
               {
                  node = unconnectedPath->GetHead();
                  copy->AddTail(node);
                  unconnectedPath->RemoveHead();
               }

               // add copy to tail of resultList
               res = new Net_Result;
               res->path = copy;
               resultList->AddTail(res);

               // remove unconnectedPath from resultlist
               tempPos = *uncPos;
               resultList->GetNext(*uncPos);
               resultList->RemoveAt(tempPos);

               delete unconnectedPath;

               return TRUE;
            }
         }
         break;
      case VIA_NODE:
         {
            Net_Via *via = viaArray->GetAt(node->index_id);

            if (newPath = FindJunction(goodPath, via->x, via->y, via->layermap))
            {
               // if via is first entity
               if (pos == unconnectedPath->GetHeadPosition())
               {
                  // connect unconnectedPath to new path
                  Net_Path *newNode;
                  POSITION tempPos = newPath->GetTailPosition();
                  while (tempPos != NULL)
                  {
                     node = newPath->GetPrev(tempPos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     unconnectedPath->AddHead(newNode);
                  }
   
                  // put unconnectedPath at end of resultList
                  resultList->AddTail(resultList->GetAt(*uncPos));
                  tempPos = *uncPos;
                  resultList->GetNext(*uncPos);
                  resultList->RemoveAt(tempPos);
   
                  FreePath(newPath);
                  return TRUE;
               }

               // if via is last entity
               if (pos == unconnectedPath->GetTailPosition())
               {
                  // connect unconnectedPath to new path
                  Net_Path *newNode;
                  POSITION tempPos = newPath->GetTailPosition();
                  while (tempPos != NULL)
                  {
                     node = newPath->GetPrev(tempPos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     newNode->reverse = !newNode->reverse;
                     unconnectedPath->AddTail(newNode);
                  }
   
                  // put unconnectedPath at end of resultList
                  resultList->AddTail(resultList->GetAt(*uncPos));
                  tempPos = *uncPos;
                  resultList->GetNext(*uncPos);
                  resultList->RemoveAt(tempPos);
   
                  FreePath(newPath);
                  return TRUE;
               }

               // middle of list
               CPathList *copy = CopyPath(newPath);

               // connect first half of unconnectedPath to newPath
               {
                  // add via (alloc a copy so don't put same address in 2 lists)
                  node = unconnectedPath->GetAt(pos);
                  Net_Path *newNode = new Net_Path;
                  memcpy(newNode, node, sizeof(Net_Path));
                  newPath->AddTail(newNode);

                  unconnectedPath->GetPrev(pos);
                  while (pos != NULL)
                  {
                     node = unconnectedPath->GetAt(pos);
                     node->reverse = !node->reverse;
                     newPath->AddTail(node);
                     POSITION tempPos = pos;
                     node = unconnectedPath->GetPrev(pos);
                     unconnectedPath->RemoveAt(tempPos);
                  }

                  // add newPath to tail of resultList
                  Net_Result *res = new Net_Result;
                  res->path = newPath;
                  resultList->AddTail(res);
               }

               // connect second half of unconnectedPath to copy
               {
                  while (unconnectedPath->GetHeadPosition() != NULL)
                  {
                     node = unconnectedPath->GetHead();
                     copy->AddTail(node);
                     unconnectedPath->RemoveHead();
                  }
   
                  // add copy to tail of resultList
                  Net_Result *res = new Net_Result;
                  res->path = copy;
                  resultList->AddTail(res);
               }

               // remove unconnectedPath from resultlist
               tempPos = *uncPos;
               resultList->GetNext(*uncPos);
               resultList->RemoveAt(tempPos);

               return TRUE;
            }
         }
         break;
      }
      unconnectedPath->GetNext(pos);
   }
   return FALSE;
}

/******************************************************************************
* FindJunction
*  - find position in goodPath that matches
*  - returns path to an endpoint if match
*/
CPathList *FindJunction(CPathList *goodPath, double x, double y, UINT64 layermap)
{
   POSITION pos;
   Net_Path *node, *newNode;
   CPathList *newPath;

   // loop good entities
   pos = goodPath->GetHeadPosition();
   while (pos != NULL)
   {
      node = goodPath->GetAt(pos);

      switch (node->type)
      {
      case ENDPOINT_NODE:
         break;
      case SEGMENT_NODE:
         {
            Net_Segment *seg = segmentArray->GetAt(node->index_id);
            double segX1, segY1, segX2, segY2;
            if (!node->reverse)
            {
               segX1 = seg->x1;
               segY1 = seg->y1;
               segX2 = seg->x2;
               segY2 = seg->y2;
            }
            else
            {
               segX1 = seg->x2;
               segY1 = seg->y2;
               segX2 = seg->x1;
               segY2 = seg->y1;
            }

            // if second point matches with head of unconnected path
            UINT64 baseVal = 1L;
            if ( ((baseVal << (seg->layer-1)) & layermap) && 
                 ((fabs(segX1 - x) < MARGIN && fabs(segY1 - y) < MARGIN) ||
                  (fabs(segX2 - x) < MARGIN && fabs(segY2 - y) < MARGIN)) )
            {
               newPath = new CPathList;
               // add nodes of goodPath from pos back to head
               if (goodPath->GetHead()->type == ENDPOINT_NODE)
               {
                  // if pnt1 and going to Head, skip seg 
                  if (fabs(segX1 - x) < MARGIN && fabs(segY1 - y) < MARGIN)
                     goodPath->GetPrev(pos);

                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     newPath->AddHead(newNode);
                     goodPath->GetPrev(pos);
                  }
               }
               // add nodes of goodPath from pos up to tail
               else
               {
                  // if pnt2 and going to Tail, skip seg 
                  if (fabs(segX2 - x) < MARGIN && fabs(segY2 - y) < MARGIN)
                     goodPath->GetNext(pos);

                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     if (node->type == SEGMENT_NODE)
                        newNode->reverse = !node->reverse;
                     newPath->AddHead(newNode);
                     goodPath->GetNext(pos);
                  }
               }
               return newPath;
            }
         }
         break;
      case VIA_NODE:
         {
            Net_Via *via = viaArray->GetAt(node->index_id);

            if ((via->layermap & layermap) && 
               fabs(via->x - x) < MARGIN && fabs(via->y - y) < MARGIN)
            {
               newPath = new CPathList;
               // add nodes to connect to HEAD of goodPath
               if (goodPath->GetHead()->type == ENDPOINT_NODE)
               {
                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     newPath->AddHead(newNode);
                     goodPath->GetPrev(pos);
                  }
               }
               // add nodes to connect to TAIL of goodPath
               else
               {
                  while (pos != NULL)
                  {
                     node = goodPath->GetAt(pos);
                     newNode = new Net_Path;
                     memcpy(newNode, node, sizeof(Net_Path));
                     if (node->type == SEGMENT_NODE)
                        newNode->reverse = !node->reverse;
                     newPath->AddHead(newNode);
                     goodPath->GetNext(pos);
                  }
               }
               return newPath;
            }
         }
         break;
      }
      goodPath->GetNext(pos);
   }
   return NULL;
}

/********************************************************************************
* RemoveDuplicated
*/
void RemoveDuplicated()
{
   POSITION pos1, pos2;
   Net_Result *path1, *path2;

   pos1 = resultList->GetHeadPosition();
   while (pos1 != NULL)
   {
      path1 = resultList->GetAt(pos1);

      pos2 = pos1;
      resultList->GetNext(pos2);

      while (pos2 != NULL)
      {
         path2 = resultList->GetAt(pos2);

         if (path2->path->GetCount() <= path1->path->GetCount())
         {
            // if duplicated, remove path2                           
            if (Duplicated(path2->path, path1->path))
            {
               POSITION tempPos = pos2;
               resultList->GetNext(pos2);
               resultList->RemoveAt(tempPos);
               FreePath(path2->path);
               delete path2;
               continue;
            }
         }
         else
         {
            // if duplicated, remove path1
            if (Duplicated(path1->path, path2->path))
            {            
               POSITION tempPos = pos1;
               resultList->GetNext(pos1);
               resultList->RemoveAt(tempPos);
               FreePath(path1->path);
               delete path1;
               break;
            }
         }

         resultList->GetNext(pos2);
      }

      resultList->GetNext(pos1);
   }
}

/********************************************************************************
* Duplicated
*/
BOOL Duplicated(CPathList *smaller, CPathList *larger)
{
   POSITION spos, lpos, temp;
   BOOL Found = FALSE;

   // see if first node of smaller is in larger
   Net_Path *snode = smaller->GetHead(), *lnode;

   lpos = larger->GetHeadPosition();
   while (lpos != NULL && !Found)
   {
      lnode = larger->GetAt(lpos);
      if (snode->type == lnode->type && snode->index_id == lnode->index_id)
         Found = TRUE;
      else
         larger->GetNext(lpos);
   }

   if (!Found)
      return FALSE;

   // see if next in larger matches second in smaller
   temp = lpos;
   larger->GetNext(lpos);
   if (lpos != NULL)
   {
      lnode = larger->GetNext(lpos);
      spos = smaller->GetHeadPosition();
      smaller->GetNext(spos);
      if (spos == NULL)
         return TRUE;
      snode = smaller->GetNext(spos);
      if (snode->type == lnode->type && snode->index_id == lnode->index_id)
      {
         while (spos != NULL)
         {
            snode = smaller->GetNext(spos);
            if (lpos == NULL)
               return FALSE;
            lnode = larger->GetNext(lpos);
            if (snode->type != lnode->type || snode->index_id != lnode->index_id)
               return FALSE;
         }
         return TRUE;
      }
   }

   // see if prev in larger matches second in smaller
   lpos = temp;
   larger->GetPrev(lpos);
   spos = smaller->GetHeadPosition();
   if (spos != NULL)
      smaller->GetNext(spos);
   while (spos != NULL)
   {
      snode = smaller->GetNext(spos);
      if (lpos == NULL)
         return FALSE;
      lnode = larger->GetPrev(lpos);
      if (snode->type != lnode->type || snode->index_id != lnode->index_id)
         return FALSE;
   }
   return TRUE;
}

/******************************************************************************
* DumpResult
*/
void DumpResult(FILE *stream, CResultList *resultList)
{
   Net_Endpoint *endpoint;
   POSITION pos;
   fprintf(stream, "DumpResult: Open Endpoints\n");
   pos = openEndpoints->GetHeadPosition();
   while (pos != NULL)
   {
      endpoint = openEndpoints->GetNext(pos);
      fprintf(stream, "%d Endpoint (x=%+lf, y=%+lf) lm=%x\n", 
            endpoint->id, endpoint->x, endpoint->y, endpoint->layermap);
   }

   fprintf(stream, "DumpResult: Used Endpoints\n");
   pos = usedEndpoints->GetHeadPosition();
   while (pos != NULL)
   {
      endpoint = usedEndpoints->GetNext(pos);
      fprintf(stream, "%d Endpoint (x=%+lf, y=%+lf) lm=%x\n", 
            endpoint->id, endpoint->x, endpoint->y, endpoint->layermap);
   }
   fprintf(stream, "\n");

   Net_Segment *seg;
   Net_Via *via;
   int j;
   fprintf(stream, "DumpResult: SegmentArray\n");
   for (j=0; j<maxSegmentArray; j++)
   {
      seg = segmentArray->GetAt(j);
      fprintf(stream, "DumpResult: %d Segment U=%d (x1=%+lf, y1=%+lf) (x2=%+lf, y2=%+lf) l=%d w=%d used=%d\n", 
            j, seg->used, seg->x1, seg->y1, seg->x2, seg->y2, seg->layer, seg->widthIndex, seg->used); 
   }  
   fprintf(stream, "\n");

   fprintf(stream, "DumpResult: ViaArray\n");
   for (j=0; j<maxViaArray; j++)
   {
      via = viaArray->GetAt(j);
      fprintf(stream, "DumpResult: %d Via U=%d (x=%+lf, y=%+lf) lm=%x used=%d\n", 
               j, via->used, via->x, via->y, via->layermap, via->used); 
   }  
   fprintf(stream, "\n");
   
   /* struct Net_Path
      {
         char type; // VIA_NODE, SEGMENT_NODE, ENDPOINT_NODE
         int index_id;
         BOOL reverse; // segment goes from p2 to p1
      }; */
   CPathList *path;
   Net_Path *node;

   // loop result paths
   POSITION resPos, pathPos;
   resPos = resultList->GetHeadPosition();
   while (resPos != NULL)
   {
      path = resultList->GetNext(resPos)->path;

      // loop nodes of this path
      fprintf(stream, "DumpResult: Number of NODES in path:%d\n", path->GetCount());
      pathPos = path->GetHeadPosition();
      while (pathPos != NULL)
      {
         node = path->GetNext(pathPos);
         switch (node->type)
         {
         case ENDPOINT_NODE:
            fprintf(stream, "DumpResult: Endpoint ID=%d\n", node->index_id);
            break;
         case SEGMENT_NODE:
            {
//          fprintf(stream, "Segment index=%d\n", node.index_id);
            Net_Segment *seg = segmentArray->GetAt(node->index_id);
            fprintf(stream, "DumpResult: %d Segment (x1=%+lf, y1=%+lf) (x2=%+lf, y2=%+lf) l=%d w=%d r=%c used=%d\n", 
                  node->index_id, seg->x1, seg->y1, seg->x2, seg->y2, seg->layer, seg->widthIndex,
                  (node->reverse?'T':'f'), seg->used); 
            }
            break;
         case VIA_NODE:
            {
//          fprintf(stream, "Via index=%d\n", node.index_id);
            Net_Via *via = viaArray->GetAt(node->index_id);
            fprintf(stream, "DumpResult: %d Via (x=%+lf, y=%+lf) lm=%x used=%d\n", 
                  node->index_id, via->x, via->y, via->layermap, via->used); 
            }
            break;
         }
      }
      fprintf(stream, "DumpResult: end of dumpresult\n");
   }
}

/************************************************************************************
* ClearUsedFlags
*  - sets all vias and segments used flags to FALSE
*/
void ClearUsedFlags()
{
	int i=0;
   for (i=0; i<maxViaArray; i++)
      viaArray->GetAt(i)->used = 0;

   for (i=0; i<maxSegmentArray; i++)
      segmentArray->GetAt(i)->used = 0;
}

CPathList *CopyPath(CPathList *path)
{
   Net_Path *node, *newNode;
   CPathList *newPath = new CPathList;

   POSITION pos = path->GetHeadPosition();
   while (pos != NULL)
   {
      node = path->GetNext(pos);
      newNode = new Net_Path;
      memcpy(newNode, node, sizeof(Net_Path));
      newPath->AddTail(newNode);
   }
   return newPath;
}

/******************************************************************************
* FreePath
*/
void FreePath(CPathList *path)
{
   POSITION pos = path->GetHeadPosition();
   while (pos != NULL)
      delete path->GetNext(pos);

   path->RemoveAll();
   delete path;
}

// end PCB_NET.CPP
