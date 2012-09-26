/****************************** DXBLPD.H ***********************************/ 
#ifndef     DXBLPD
#define     DXBLPD 
#include    <dxstring.h>
typedef     struct  dx_blockpos_struct {
            DX_STRING   block_name ;
            fpos_t      block_position ; 
            } DXF_BLOCKPOS_S ;
typedef     DXF_BLOCKPOS_S *DXF_BLOCKPOS ;
#endif

