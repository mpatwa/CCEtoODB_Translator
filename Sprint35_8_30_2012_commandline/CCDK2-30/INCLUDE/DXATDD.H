/****************************** DXATDD.H ***********************************/
/* 06/21/93 ANN : Added ifdef "UNIX" & "ACAD!!" */

#ifndef     DXATDD 
#define     DXATDD 

typedef     struct  dx_attdef_struct {
            DXF_ENTITY_HEADER_S    attdef_entity_header ;
            REAL    attdef_pt[3] ;
            REAL    attdef_height ;
            DX_TXT_STRING attdef_value ;
            DX_STRING  attdef_prompt ;
            DX_STRING  attdef_tag ;
            INT     attdef_aflags ;
            INT     attdef_fld_len ;
            REAL    attdef_rot_ang ;
            REAL    attdef_x_scale ;
            REAL    attdef_obl_ang ;
            DX_STRING  attdef_style ;
            INT     attdef_gflags ;
#ifdef __UNIX__
            DX_INT_FILLER   attdef_filler ;
#endif
#ifdef  __ACAD11__
            INT     attdef_hor_just ;
            INT     attdef_vert_just ;
#else
            INT     attdef_just ;
#endif
#ifdef __MSC32__
            DX_INT_FILLER   attdef_filler ;
#endif
            REAL    attdef_al_pt[3] ;
            } DX_ATTDEF_S ;
typedef     DX_ATTDEF_S *DX_ATTDEF ;
#endif

