/****************************** DXUCSD.H ***********************************/ 
#ifndef     DXUCSD 
#define     DXUCSD 
typedef     struct  dx_ucs_struct {
            DX_STRING  ucs_name ;
            INT     ucs_flags ;
#ifdef __UNIX__
            DX_INT_FILLER   ucs_filler ;
#endif
#ifdef __MSC32__
            DX_INT_FILLER   ucs_filler ;
#endif
            REAL    ucs_origin[3] ;
            REAL    ucs_x_axis[3] ;
            REAL    ucs_y_axis[3] ;
            } DX_UCS_S ;
typedef     DX_UCS_S  *DX_UCS ;
#endif
