/****************************** DXBLCD.H ***********************************/ 
#ifndef     DXBLCD
#define     DXBLCD 

typedef     struct  dx_block_struct {
            DXF_ENAME    block_ename ;
            DXF_ENTITY_HEADER_S    block_entity_header ;
            DX_STRING   block_name ;    /* 2 */
            INT     block_flags ;   /* 70 */
#ifdef __UNIX__
            DX_INT_FILLER  block_filler ;
#endif
#ifdef  __ACAD11__
            DX_STRING   block_name3 ;   /* 3 */
#endif
#ifdef __MSC32__
            DX_INT_FILLER   block_filler ;
#endif
            REAL        block_base_pt[3] ;  /* 10, 20, 30 */
#ifdef  __ACAD11__
            DX_TXT_STRING block_xref_pathname ;  /* 1 */
#endif
            } DXF_BLOCK_S ;
typedef     DXF_BLOCK_S *DXF_BLOCK ;
#endif

