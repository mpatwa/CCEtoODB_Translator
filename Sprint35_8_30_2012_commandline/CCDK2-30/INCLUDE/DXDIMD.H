/****************************** DXDIMD.H ***********************************/
#ifndef     DXDIMD 
#define     DXDIMD 
typedef     struct  dx_dim_struct {
            DXF_ENTITY_HEADER_S    dim_entity_header ;
            DX_STRING  dim_name ;
#ifdef  __ACAD11__
            DX_STRING  dim_style_name ;
#endif
            REAL    dim_def_pt[3] ;
            REAL    dim_mid_pt[3] ;
            REAL    dim_insert_pt[3] ;
            INT     dim_type ;
#ifdef __UNIX__
            DX_INT_FILLER   dim_filler ;
#endif
            DX_TXT_STRING dim_text ;
#ifdef __MSC32__
            DX_INT_FILLER   dim_filler ;
#endif
            REAL    dim_def_pt3[3] ;
            REAL    dim_def_pt4[3] ;
            REAL    dim_def_pt5[3] ;
            REAL    dim_def_pt6[3] ;
            REAL    dim_ldr_len ;
            REAL    dim_ang ;
            REAL    dim_horiz ;
#ifdef __ACAD11__
            REAL    dim_obl_ang ;
            REAL    dim_rot_ang ;
#endif
            } DX_DIM_S ;
typedef     DX_DIM_S *DX_DIM ;
#endif

