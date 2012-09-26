/* -2 -3 */
/********************************** C2CW.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!  (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <alwdefs.h>
#include <c2gdefs.h>
#include <c2ldefs.h>
#include <c2pdefs.h>
#include <c2ndefs.h>
#include <c2cdefs.h>
#include <c2cpriv.h>
#include <c2dpriv.h>
#include <dmldefs.h>

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST c2c_read_curves ( filename, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST curves_list ;
{
    FILE *file ;

    file = fopen ( filename, "r" ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    curves_list = c2c_get_curves ( file, curves_list ) ;
    fclose ( file ) ;
    RETURN ( curves_list ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2c_read_curve ( filename ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
{
    FILE *file ;
    C2_CURVE curve ;

    file = fopen ( filename, "r" ) ;
    if ( file == NULL ) 
        RETURN ( NULL ) ;
    curve = c2c_get_curve ( file ) ;
    fclose ( file ) ;
    RETURN ( curve ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DML_LIST c2c_get_curves ( file, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
DML_LIST curves_list ;
{
    C2_CURVE curve ;

    while ( TRUE ) {
        curve = c2c_get_curve ( file ) ; 
        if ( curve == NULL ) 
            RETURN ( curves_list ) ;
        if ( curves_list == NULL ) 
            curves_list = dml_create_list () ;
        dml_append_data ( curves_list, curve ) ;
    }
#ifndef __BRLNDC__
#ifndef __UNIX__
    RETURN ( NULL ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE C2_CURVE c2c_get_curve ( file ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE *file ;
{
    C2_CURVE curve ;
    char name[16] ;

    curve = NULL ;
    while ( TRUE ) {
        if ( !alw_get_string ( file, name, 16 ) )
            RETURN ( NULL ) ;
        if ( strcmp ( name, "line" ) == 0 ) {
            curve = c2d_curve () ;
            C2_CURVE_LINE(curve) = c2l_get ( file, 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
            C2_CURVE_TYPE(curve) = C2_LINE_TYPE ;
        }
        else if ( strcmp ( name, "arc" ) == 0 ) {
            curve = c2d_curve () ;
            C2_CURVE_ARC(curve) = c2g_get ( file, 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
            C2_CURVE_TYPE(curve) = C2_ARC_TYPE ;
        }
        else if ( strcmp ( name, "pcurve" ) == 0 ) {
            curve = c2d_curve () ;
            C2_CURVE_PCURVE(curve) = c2p_get ( file, 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
            C2_CURVE_TYPE(curve) = C2_PCURVE_TYPE ;
        }
#ifdef SPLINE
        else if ( strcmp ( name, "spline" ) == 0 ||
                  strcmp ( name, "nurb" ) == 0 ) {
            curve = c2d_curve () ;
            C2_CURVE_NURB(curve) = c2n_get ( file, 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
            C2_CURVE_TYPE(curve) = C2_NURB_TYPE ;
        }
        else if ( strcmp ( name, "ellipse" ) == 0 ) {
            curve = c2d_curve () ;
            C2_CURVE_NURB(curve) = c2n_get ( file, 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
            C2_CURVE_TYPE(curve) = C2_ELLIPSE_TYPE ;
        }
#endif
        if ( curve != NULL ) {
            c2c_box ( curve ) ;
            RETURN ( curve ) ;
        }
    }
#ifndef __BRLNDC__
#ifndef __UNIX__
    RETURN ( NULL ) ;
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_write_curves ( filename, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
DML_LIST curves_list ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = c2c_put_curves ( file, curves_list ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_write_curve ( filename, curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING filename ;
C2_CURVE curve ;
{
    FILE *file ;
    BOOLEAN status ;

    file = fopen ( filename, "w" ) ;
    status = c2c_put_curve ( file, curve ) ;
    fclose ( file ) ;
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_put_curves ( file, curves_list ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
DML_LIST curves_list ;
{
    DML_ITEM item ;
    BOOLEAN status = TRUE ;

    DML_WALK_LIST ( curves_list, item ) {
        status = c2c_put_curve ( file, DML_RECORD(item) ) && status ;
    }
    RETURN ( status ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN c2c_put_curve ( file, curve ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
FILE* file ;
C2_CURVE curve ;
{
    BOOLEAN status ;

    if ( C2_CURVE_IS_LINE(curve) ) 
        status = alw_put_string ( file, "line", NULL ) && 
            c2l_put ( file, C2_CURVE_LINE(curve), 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
    else if ( C2_CURVE_IS_ARC(curve) ) 
        status = alw_put_string ( file, "arc", NULL ) && 
            c2g_put ( file, C2_CURVE_ARC(curve), 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
    else if ( C2_CURVE_IS_PCURVE(curve) ) 
        status = alw_put_string ( file, "pcurve", NULL ) && 
            c2p_put ( file, C2_CURVE_PCURVE(curve), 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
#ifdef SPLINE 
    else if ( C2_CURVE_IS_ELLIPSE(curve) ) 
        status = alw_put_string ( file, "ellipse", NULL ) && 
            c2n_put ( file, C2_CURVE_NURB(curve), 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
    else if ( C2_CURVE_IS_SPLINE(curve) ) 
        status = alw_put_string ( file, "spline", NULL ) && 
            c2n_put ( file, C2_CURVE_NURB(curve), 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
    else if ( C2_CURVE_IS_BEZIER(curve) ) 
        status = alw_put_string ( file, "bezier", NULL ) && 
            c2n_put ( file, C2_CURVE_NURB(curve), 
                C2_CURVE_PARM0(curve), C2_CURVE_PARM1(curve) ) ;
#endif
    else
        status = FALSE ;
    RETURN ( status ) ;
}

