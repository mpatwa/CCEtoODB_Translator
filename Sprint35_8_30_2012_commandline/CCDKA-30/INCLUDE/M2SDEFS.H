/******************************** M2SDEFS.C ********************************/
/********************************* Splines *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef      SPLINE
#ifndef     M2SDEFS
#define     M2SDEFS
#include    <bbsdefs.h>
#include    <dmdefs.h>
#include    <c2defs.h>
#include    <m2defs.h>

extern  BOOLEAN m2s_x_max __(( HPT2* DUMMY0 , INT DUMMY1 , REAL* DUMMY2 ,
            REAL DUMMY3 , PARM DUMMY4 , PARM DUMMY5 ,  REAL* DUMMY6 ,
            PARM DUMMY7 )) ;
extern  INT     m2s_hor_ray_int_no __(( HPT2* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 , 
            PARM DUMMY6 , PT2 DUMMY7 , BOOLEAN DUMMY8 )) ;
extern  INT     m2s_ray_int_no __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 , PARM DUMMY6 , 
            PT2 DUMMY7 , REAL DUMMY8 , REAL DUMMY9 , BOOLEAN DUMMY10 )) ;
extern  INT     m2s_xhatch_inters __(( HPT2* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 ,  PARM DUMMY5 ,
            PARM DUMMY6 , PT2 DUMMY7 , REAL DUMMY8 , REAL DUMMY9 ,
            REAL DUMMY10 , ANY DUMMY11 , DML_LIST* DUMMY12 , INT DUMMY13 )) ;
extern  REAL    m2s_area __(( HPT2* DUMMY0 , INT DUMMY1 ,
            INT DUMMY2 , REAL* DUMMY3 , REAL DUMMY4 , PARM DUMMY5 ,
            PARM DUMMY6 , PT2 DUMMY7 )) ;
extern  BOOLEAN m2s_area_mass_ctr __(( HPT2* DUMMY0 , INT DUMMY1 ,
            REAL* DUMMY2 , REAL DUMMY3 , PARM DUMMY4 ,
            PARM DUMMY5 , PT2 DUMMY6 )) ;
extern  DML_LIST m2s_appr_arc __(( HPT2* DUMMY0 , INT DUMMY1 , INT DUMMY2 , 
            REAL* DUMMY3 , REAL DUMMY4 , C2_CURV_EXTR DUMMY5 , INT DUMMY6 ,
            PARM DUMMY7 , PARM DUMMY8 , REAL DUMMY9 , DML_LIST DUMMY10 )) ;
#endif  /* M2SDEFS */
#endif  /* SPLINE */

