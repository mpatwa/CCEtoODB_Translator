/***************************** DX2PRIV.H ***********************************/
/*******************  Prototypes for dxf routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DX2PRIV
#define     DX2PRIV
extern  C2_CURVE    dx2_pline_to_curve __(( DXF_ENTITY DUMMY0 )) ;
#ifdef __ACAD11__
extern  DXF_ENTITY dx2_vertex_to_entity __(( DXF_FILE, PT2, REAL, INT )) ;
#ifdef SPLINE
extern  DXF_ENTITY dx2_ellnurb_to_entity __(( DXF_FILE, C2_NURB, PARM, PARM )) ;
extern  DXF_ENTITY dx2_ellipse_to_entity __(( DXF_FILE, PT2, REAL, REAL, REAL, 
            REAL, REAL )) ;
extern  DXF_ENTITY dx2_spline_to_entity __(( DXF_FILE, C2_NURB, PARM, PARM )) ;
#endif
#else
extern  DXF_ENTITY dx2_vertex_to_entity __(( PT2, REAL, INT )) ;
#ifdef SPLINE
extern  DXF_ENTITY dx2_ellnurb_to_entity __(( C2_NURB, PARM, PARM )) ;
extern  DXF_ENTITY dx2_ellipse_to_entity __(( PT2, REAL, REAL, REAL, 
            REAL, REAL )) ;
extern  DXF_ENTITY dx2_spline_to_entity __(( C2_NURB, PARM, PARM )) ;
#endif  
#endif
#endif

