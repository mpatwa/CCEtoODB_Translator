/****************************** DXARCD.H ***********************************/ 
#ifndef     DXARCD 
#define     DXARCD 
#include    <dxhdrd.h>
typedef     struct  dx_arc_struct {
            DXF_ENTITY_HEADER_S    arc_entity_header ;
            REAL    arc_ctr[3] ;
            REAL    arc_rad ;
            REAL    arc_ang0 ;
            REAL    arc_ang1 ;
            } DX_ARC_S ;
typedef     DX_ARC_S *DX_ARC ;
#endif  /* DXARCD */

