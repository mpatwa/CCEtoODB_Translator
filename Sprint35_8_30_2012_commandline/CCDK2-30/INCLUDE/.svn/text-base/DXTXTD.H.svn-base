/****************************** DXTXTD.H ***********************************/ 
#ifndef     DXTXTD
#define     DXTXTD 
typedef     struct  dx_text_struct {
            DXF_ENTITY_HEADER_S    text_entity_header ;
            REAL    text_insert_pt[3] ;
            REAL    text_height ;
            DX_TXT_STRING text_value ;
            REAL    text_rot_ang ;
            REAL    text_x_scale ;
            REAL    text_obl_ang ;
            DX_STRING  text_style ;
            INT     text_flags ;
#ifdef __UNIX__
            DX_INT_FILLER   text_filler ;
#endif
#ifdef  __ACAD11__
            INT     text_hor_just ;
            INT     text_vert_just ;
#else
            INT     text_just ;
#endif 
#ifdef __MSC32__
            DX_INT_FILLER   text_filler ;
#endif
            REAL    text_al_pt[3] ;
            } DX_TEXT_S ;
typedef     DX_TEXT_S *DX_TEXT ;
#endif

