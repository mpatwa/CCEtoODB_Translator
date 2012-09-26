// $Header: /CAMCAD/4.3/read_wrt/DxfIn.h 8     8/12/03 9:06p Kurt Van Ness $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/           

#pragma once

/* Define Section ***********************************************************/

#define A_UNKNOWN          0
#define A_SECTION          1
#define A_HEADER           2
#define A_ENTITIES         3
#define A_ENDSEC           4
#define A_CIRCLE           5
#define A_EOF              6
#define A_POLYLINE         7
#define A_VERTEX           8
#define A_SEQEND           9  
#define A_ARC              10
#define A_TEXT             11
#define A_TRACE            12
#define A_LINE             13
#define A_SOLID            14
#define A_INSERT           15
#define A_ATTRIB           16
#define A_DIMENSION        17
#define A_MTEXT            18
#define A_LAYER            20
#define A_BLOCK            21
#define A_ENDBLK           22
#define A_LWPOLYLINE       23
#define A_MLINE            24
#define A_3DFACE           25
#define A_LEADER           26
#define A_HATCH            27
#define A_ELLIPSE          28
#define A_POINT            29
#define A_ATTDEF           30

#define TRUE               1
#define FALSE              0
#define MAX_LINE           512   /* Max line length.       */
#define MAX_ATTRIB         100

#define  SMALL_DELTADEGREE 1     // 1 degree accurary on arcs

#define  EXTRUSION         -0.9  // there is an inaccuracy in ACAD 13 -1 is -0.99999


/* Define Section ***********************************************************/

typedef struct
{
   CString  name;          // 
   int      color;         // 
}DXFLayerlist;
typedef CTypedPtrArray<CPtrArray, DXFLayerlist*> CDXFLayerArray;

typedef struct
{
   double   x,y;
   double   w,w41;
   double   bulge;
   int      acad_72;             // 1 = line, 2 = circle, 3 = elipse (not supported), 4 = spline (not supported)
   int      acad_73, acad_93;    // this is needed for hatches, which have mulitple polylines in one structure
} DxfPoly;
typedef CArray<DxfPoly, DxfPoly&> DXFPolyArray;

typedef  struct
{
   int      acad_nr;
   int      acad_type;
   char     acad_line[MAX_LINE];    /* line etc */
   long     acad_linecnt;
   int      acad_0_type;
   int      acad_2_type;
   char     acad_1[MAX_LINE];
   char     acad_2[MAX_LINE];
   int      acad_6;        // 0: BYLAYER 1: BYBLOCK - by block means that the color in 62 is used
   char     acad_8[MAX_LINE];
   double   acad_10,acad_20,acad_30; // jb. wsp. z
   double   acad_11,acad_21,acad_31; // jb. wsp. z
   double   acad_12,acad_22,acad_32; // jb. wsp. z
   double   acad_13,acad_23,acad_33; // jb. wsp. z
   double   acad_38; // jb. elevation  30.I.94r.
   double   acad_39; // thickness
   double   acad_40,acad_41,acad_42,acad_43; // jb. wsp. z
   double   acad_50,acad_51;

   int      acad_62; // colour added
   int      acad_66;
   int      acad_70;
   int      acad_71;
   int      acad_72;
   int      acad_73;
   int      acad_74;
   int      acad_90, acad_91, acad_93;
   double   acad_210, acad_220, acad_230; //jb. extrusion direction 30.I.94r.
            
} DXFLocal;
