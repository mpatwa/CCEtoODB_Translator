/****************************** DXHDVRD.H **********************************/ 
#ifndef     DXHDVRD 
#define     DXHDVRD 
typedef     struct  dx_hdr_var {
            union {
                REAL        hdr_var_real[3] ;
                INT         hdr_var_int ;
                DX_STRING   hdr_var_string ;
                } dx_hdr_data ;
            } DXF_HDR_VAR_S,  *DXF_HDR_VAR ;
#endif


