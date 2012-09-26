/****************************** DXLIND.H ***********************************/ 
#ifndef     DXLIND
#define     DXLIND
typedef     struct  dx_line_struct {
            DXF_ENTITY_HEADER_S    line_entity_header ;
            REAL    line_pt0[3] ;
            REAL    line_pt1[3] ;
            } DX_LINE_S ;
typedef     DX_LINE_S *DX_LINE ;
#endif

