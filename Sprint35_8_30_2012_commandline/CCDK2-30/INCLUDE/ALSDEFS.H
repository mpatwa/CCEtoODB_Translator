/****************************** ALSDEFS.H **********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef  SPLINE
#ifndef ALSDEFS
#define ALSDEFS
#include <aldefs.h>

extern  BOOLEAN als_parm_adjust __(( INT DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            PARM DUMMY3 )) ;
extern  BOOLEAN als_interp_tan __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 , REAL* DUMMY5 , REAL* DUMMY6 )) ;
extern  BOOLEAN als_interp_clsd __(( REAL* DUMMY0 , INT DUMMY1 , INT DUMMY2 ,
            REAL* DUMMY3 , REAL* DUMMY4 )) ;
extern  void    als_eval __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            INT DUMMY3 , REAL DUMMY4 , INT DUMMY5 , REAL* DUMMY6 )) ;
extern  void    als_insert_knot __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            INT DUMMY3 , REAL DUMMY4 , REAL* DUMMY5 )) ;
extern  void    als_insert_knot0 __(( REAL* DUMMY0 , INT DUMMY1 , 
            REAL* DUMMY2 , INT DUMMY3 , REAL* DUMMY4 )) ;
extern  void    als_insert_knot1 __(( REAL* DUMMY0 , INT DUMMY1 , 
            REAL* DUMMY2 , INT DUMMY3 , REAL* DUMMY4 )) ;
extern  void    als_eval0 __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            INT DUMMY3 , REAL DUMMY4 , INT DUMMY5 )) ;
extern  void    als_conv_bez __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            INT DUMMY3 , REAL* DUMMY4 )) ;
extern  void    als_4ord_close_end __(( REAL* DUMMY0 , REAL* DUMMY1 ,
            REAL* DUMMY2 ,  REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,
            REAL DUMMY6 , REAL DUMMY7 , INT DUMMY8 , REAL* DUMMY9 ,
            REAL* DUMMY10 )) ;
extern  void    als_4ord_open_end __(( REAL* DUMMY0 , REAL* DUMMY1 ,
            REAL* DUMMY2 ,  REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,
            REAL DUMMY6 , REAL DUMMY7 , INT DUMMY8 , REAL* DUMMY9 ,
            REAL* DUMMY10 )) ;
extern  void    als_uniform_knots __(( INT DUMMY0 , INT DUMMY1 , 
            REAL* DUMMY2 )) ;
extern  void    als_clsc_uni_knots __(( INT DUMMY0 , INT DUMMY1 , 
            REAL* DUMMY2 )) ;
extern  void    als_ellipse_knots __(( INT DUMMY0 , REAL* DUMMY1 )) ;
extern  void    als_trim __(( REAL* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 , 
            INT DUMMY3 , PARM DUMMY4 , PARM DUMMY5 , REAL* DUMMY6 , 
            INT DUMMY7 , REAL* DUMMY8 )) ;
extern  BOOLEAN als_ellipse_break_t __(( REAL DUMMY0 , REAL DUMMY1 , 
            REAL DUMMY2 , REAL* DUMMY3 )) ;
#endif /* ALSDEFS */
#endif /* SPLINE */

