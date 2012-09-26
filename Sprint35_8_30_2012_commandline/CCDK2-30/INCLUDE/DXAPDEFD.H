/****************************** DXAPDEFD.H *********************************/ 
#ifndef DXAPDEFD
#define DXAPDEFD

#ifdef __ACAD11__
#define     DX_INIT_TEMPL_SIZE      30
typedef     struct  dx_appdef_struct {
            DX_STRING           appl_name ;
            INT                 appl_templ_size ; 
            INT                 appl_curr_size ; 
            int*                appl_template ; 
            } DX_APPDEF_S ;
typedef     DX_APPDEF_S  *DX_APPDEF ;
#endif

#endif


