/******************************* C2CXTRD.H *********************************/
#ifndef C2CXTRD
#define C2CXTRD
#ifdef      SPLINE
typedef     struct  c2_curvature_extr_rec {
            REAL            curv ;
            PARM_S          parm ;
            } C2_CURV_EXTR_S ;
typedef     C2_CURV_EXTR_S    *C2_CURV_EXTR ;
#endif  /* SPLINE */
#endif  /* C2CXTRD */

