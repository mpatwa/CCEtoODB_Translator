/* -2 -3 */
/******************************* DX2G1.C ********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <c2ddefs.h>
#include <dxents.h>
#include <dx2defs.h>
#include <dx2priv.h>
#include <c2vmcrs.h>

STATIC C2_CURVE dx2_line_to_curve __(( DXF_ENTITY )) ;
STATIC C2_CURVE dx2_circle_to_curve __(( DXF_ENTITY )) ;
STATIC C2_CURVE dx2_arc_to_curve __(( DXF_ENTITY )) ;
STATIC C2_CURVE dx2_3dface_to_curve __(( DXF_ENTITY )) ;
STATIC C2_CURVE dx2_solid_to_curve __(( DXF_ENTITY )) ;
STATIC C2_CURVE dx2_trace_to_curve __(( DXF_ENTITY )) ;

/*-------------------------------------------------------------------------*/
BBS_PUBLIC PT2* dx2_entity_to_point ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{ 
    RETURN ( entity != NULL && DXF_ENTITY_IS_POINT(entity) ? 
        c2d_point ( DXF_POINT_PT(entity)[0], DXF_POINT_PT(entity)[1] ) : 
        NULL ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PUBLIC C2_CURVE dx2_entity_to_curve ( entity ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY entity ;
{ 
    if ( entity == NULL ) 
        RETURN ( NULL ) ;
    else if ( DXF_ENTITY_IS_LINE(entity) ) 
        RETURN ( dx2_line_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_CIRCLE(entity) ) 
        RETURN ( dx2_circle_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_ARC(entity) ) 
        RETURN ( dx2_arc_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_PLINE(entity) ) 
        RETURN ( dx2_pline_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_3DFACE(entity) ) 
        RETURN ( dx2_3dface_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_SOLID(entity) ) 
        RETURN ( dx2_solid_to_curve ( entity ) ) ;
    else if ( DXF_ENTITY_IS_TRACE(entity) ) 
        RETURN ( dx2_trace_to_curve ( entity ) ) ;
    else
        RETURN ( NULL ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_line_to_curve ( line ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY line ;
{
    RETURN ( c2d_line ( DXF_LINE_PT0(line), DXF_LINE_PT1(line) ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_circle_to_curve ( circle ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY circle ;
{
    RETURN ( c2d_circle ( DXF_CIRCLE_CTR(circle), DXF_CIRCLE_RAD(circle) ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_arc_to_curve ( arc ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY arc ;
{
    REAL ang0, sweep ;
    ang0 = PI_OVER_180 * DXF_ARC_ANG0(arc) ;
    sweep = PI_OVER_180 * ( DXF_ARC_ANG1(arc) - DXF_ARC_ANG0(arc) ) ;
    if ( sweep < 0.0 ) 
        sweep += TWO_PI ;
    RETURN ( c2d_arc ( DXF_ARC_CTR(arc), DXF_ARC_RAD(arc), ang0, sweep, 1 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_3dface_to_curve ( face ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY face ;
{
    PT2 a[4] ;

    C2V_COPY ( DXF_3DFACE_PT0(face), a[0] ) ;
    C2V_COPY ( DXF_3DFACE_PT1(face), a[1] ) ;
    C2V_COPY ( DXF_3DFACE_PT2(face), a[2] ) ;
    C2V_COPY ( DXF_3DFACE_PT3(face), a[3] ) ;

    RETURN ( c2d_pcurve_through ( a, C2V_IDENT_PTS ( a[2], a[3] ) ? 3 : 4 ) ) ;
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_solid_to_curve ( solid ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY solid ;
{
    PT2 a[5] ;

    C2V_COPY ( DXF_SOLID_PT0(solid), a[0] ) ;
    C2V_COPY ( DXF_SOLID_PT1(solid), a[1] ) ;

    if ( C2V_IDENT_PTS ( DXF_SOLID_PT2(solid), DXF_SOLID_PT3(solid) ) ) { 
        C2V_COPY ( DXF_SOLID_PT2(solid), a[2] ) ;
        C2V_COPY ( DXF_SOLID_PT0(solid), a[3] ) ;
        RETURN ( c2d_pcurve_through ( a, 4 ) ) ;
    }

    else {
        C2V_COPY ( DXF_SOLID_PT2(solid), a[3] ) ;
        C2V_COPY ( DXF_SOLID_PT3(solid), a[2] ) ;
        C2V_COPY ( DXF_SOLID_PT0(solid), a[4] ) ;
        RETURN ( c2d_pcurve_through ( a, 5 ) ) ;
    }
}


/*-------------------------------------------------------------------------*/
STATIC  C2_CURVE dx2_trace_to_curve ( trace ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_ENTITY trace ;
{
    PT2 a[4] ;

    C2V_COPY ( DXF_TRACE_PT0(trace), a[0] ) ;
    C2V_COPY ( DXF_TRACE_PT1(trace), a[1] ) ;
    C2V_COPY ( DXF_TRACE_PT2(trace), a[2] ) ;
    C2V_COPY ( DXF_TRACE_PT3(trace), a[3] ) ;

    RETURN ( c2d_pcurve_through ( a, 4 ) ) ;
}

