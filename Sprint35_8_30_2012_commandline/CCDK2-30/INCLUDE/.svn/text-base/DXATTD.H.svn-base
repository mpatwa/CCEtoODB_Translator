/****************************** DXATTD.H ***********************************/
/* 06/18/93 ANN : Added ifdef for "UNIX" & "ACAD11" as what TEXT does. */

#ifndef     DXATTD 
#define     DXATTD 

typedef     struct  dx_attrib_struct {
            DXF_ENTITY_HEADER_S    attrib_entity_header ;
            REAL    attrib_pt[3] ;
            REAL    attrib_height ;
            DX_TXT_STRING attrib_value ;
            DX_STRING  attrib_tag ;
            INT     attrib_aflags ;
            INT     attrib_fld_len ;
            REAL    attrib_rot_ang ;
            REAL    attrib_x_scale ;
            REAL    attrib_obl_ang ;
            DX_STRING  attrib_style ;
            INT     attrib_gflags ;
#ifdef __UNIX__
            DX_INT_FILLER   attrib_filler ;
#endif
#ifdef  __ACAD11__
            INT     attrib_hor_just ;
            INT     attrib_vert_just ;
#else
            INT     attrib_just ;
#endif
#ifdef __MSC32__
            DX_INT_FILLER   attrib_filler ;
#endif
            REAL    attrib_al_pt[3] ;
            } DX_ATTRIB_S ;
typedef     DX_ATTRIB_S *DX_ATTRIB ;
#endif

