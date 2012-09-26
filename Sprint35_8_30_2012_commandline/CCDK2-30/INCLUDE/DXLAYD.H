/****************************** DXLAYD.H ***********************************/ 
#ifndef     DXLAYD 
#define     DXLAYD 
typedef     struct  dx_layer_struct {
            DX_STRING  layer_name ;
            INT     layer_flags ;
            INT     layer_color ;
            DX_STRING  layer_ltype ;
            } DX_LAYER_S ;
typedef     DX_LAYER_S  *DX_LAYER ;
#endif

