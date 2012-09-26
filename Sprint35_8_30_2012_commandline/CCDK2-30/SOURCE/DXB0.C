/* -2 -3 -x */
/******************************* DXB0.C *********************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                      !!!!!!!!*/
/*!!!!!!!!     (C) Copyright 1989 - 1995 Building Block Software!!!!!!!!*/
/*!!!!!!!!                   All rights reserved                !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <string.h>
#include <dx0defs.h>
#include <dxetmpls.h>

#ifdef  __ACAD11__
int DXF_BLOCK_TEMPLATE_10[] = { DX_BLOCK_TEMPLATE_10 } ;
int DXF_BLOCK_TEMPLATE_11[] = { DX_BLOCK_TEMPLATE_11 } ;
#else
int DXF_BLOCK_TEMPLATE[]    = { DX_BLOCK_TEMPLATE } ;
#endif

#ifdef __ACAD11__
/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_BLOCK dx0_alloc_block ( release )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT release ;
{ 
    DXF_BLOCK block = DX_CREATE_BLOCK ;
    if ( block == NULL )
        RETURN ( NULL ) ;
    DXF_ENAME_INIT ( DXF_BLOCK_ENAME(block) ) ;
    dx0_init_template ( block, ( release == 10 ) ? 
        DXF_BLOCK_TEMPLATE_10 : DXF_BLOCK_TEMPLATE_11 ) ;
    RETURN ( block ) ;
}
#else

/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_BLOCK dx0_alloc_block ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
    DXF_BLOCK block = DX_CREATE_BLOCK ;
    if ( block == NULL )
        RETURN ( NULL ) ;
    DXF_ENAME_INIT ( DXF_BLOCK_ENAME(block) ) ;
    dx0_init_template ( block, DXF_BLOCK_TEMPLATE ) ;
    RETURN ( block ) ;
}
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dxf_free_block ( block ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_BLOCK block ;
{ 
    DX_FREE_BLOCK ( block ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE DXF_BLOCKPOS dx0_alloc_blockpos ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
    DXF_BLOCKPOS blockpos = DX_CREATE_BLOCKPOS ;
    if ( blockpos == NULL )
        RETURN ( NULL ) ;
    dx0_strcpy ( DX_BLOCKPOS_NAME(blockpos), NULL ) ;
    RETURN ( blockpos ) ;
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void dx0_free_blockpos ( blockpos ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
DXF_BLOCKPOS blockpos ;
{ 
    DX_FREE_BLOCKPOS ( blockpos ) ;
}

