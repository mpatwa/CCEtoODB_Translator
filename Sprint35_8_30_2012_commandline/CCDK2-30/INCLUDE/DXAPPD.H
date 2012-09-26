/****************************** DXAPPD.H ***********************************/ 
#ifndef     DXAPPD 
#define     DXAPPD 
#ifdef __ACAD11__
typedef     struct      dx_appid_struct {
            DX_STRING   appid_name ;
            INT         appid_flags ;
            } DX_APPID_S ;
typedef     DX_APPID_S  *DX_APPID ;
#endif
#endif

