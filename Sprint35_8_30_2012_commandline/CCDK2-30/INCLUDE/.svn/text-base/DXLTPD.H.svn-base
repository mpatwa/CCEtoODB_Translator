/****************************** DXLTPD.H ***********************************/ 
#ifndef     DXLTPD 
#define     DXLTPD 
typedef     struct      dx_ltype_struct {
            DX_STRING   ltype_name ;
            INT         ltype_flags ;
#ifdef __UNIX__
            DX_INT_FILLER   ltype_filler ;
#endif
            DX_STRING   ltype_text ;
            INT         ltype_algn ;
            INT         ltype_count ;
#ifdef __MSC32__
            DX_INT_FILLER   _filler ;
#endif
            REAL        ltype_tot_len ;
            REAL*       ltype_len ;
            } DX_LTYPE_S ;
typedef     DX_LTYPE_S  *DX_LTYPE ;
#endif 

