/****************************** DXAPDATD.H *********************************/ 
#ifndef DXAPDATD
#define DXAPDATD

#ifdef __ACAD11__
typedef     struct  dx_appdat_struct {
            int*                appl_template ; 
            ANY                 appl_data ; 
            } DX_APPDAT_S ;
typedef     DX_APPDAT_S  *DX_APPDAT ;
#endif
#endif


