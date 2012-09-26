/****************************** DXHDVRM.H **********************************/ 
#ifndef     DXHDVRM 
#define     DXHDVRM 
#define     DXF_HDR_VAR_REAL(H)     ((H)->hdr_var_data.hdr_var_real)
#define     DXF_HDR_VAR_INT(H)      ((H)->hdr_var_data.hdr_var_int)
#define     DXF_HDR_VAR_STRING(H)   ((H)->hdr_var_data.hdr_var_string)
#define     DX_CREATE_HDR_VAR       (MALLOC(1,DXF_HDR_VAR_S))
#define     DX_FREE_HDR_VAR(H)      FREE(H)
#endif


