/****************************** DXFILM.H ***********************************/ 
#ifndef     DXFILM
#define     DXFILM 
#define     DX_FILE_HANDLE(F)           ((F)->fp)
#define     DX_FILE_TYPE(F)             ((F)->file_type)
#define     DX_FILE_ASCII(F)            (DX_FILE_TYPE(F)&DXF_ASCII)
#define     DX_FILE_BINARY(F)           (DX_FILE_TYPE(F)&DXF_BINARY)
#define     DX_FILE_READ(F)             (DX_FILE_TYPE(F)&DXF_READ)
#define     DX_FILE_WRITE(F)            (DX_FILE_TYPE(F)&DXF_WRITE)
#define     DX_FILE_PRECISION(F)        ((F)->file_prec)
#ifdef  __ACAD11__
#define     DX_FILE_RELEASE(F)          ((F)->file_release)
#define     DX0_FILE_RELEASE(F)         \
(((F)==NULL)?DXF_ACAD_RELEASE:((F)->file_release))
#endif
#define     DX_FILE_STATUS(F)           ((F)->file_status)
#define     DX_FILE_HEADER_POS(F)       ((F)->file_header_pos)
#define     DX_FILE_TABLES_POS(F)       ((F)->file_tables_pos)
#define     DX_FILE_BLOCKS_POS(F)       ((F)->file_blocks_pos)
#define     DX_FILE_ENTITIES_POS(F)     ((F)->file_entities_pos)
#define     DX_FILE_BLOCKPOS_LIST(F)    ((F)->file_blockpos_list)
#define     DX_FILE_LENGTH(F)           ((F)->file_length)
#define     DX_FILE_SPLINESEGS(F)       ((F)->file_splinesegs)
#define     DX_FILE_UNREC_LIST(F)       ((F)->file_unrec_list)
#define     DX_FILE_FLAGS(F)            ((F)->file_flags)
#define     DX_FILE_HEADER_POS_DEF(F)   (DX_FILE_FLAGS(F)&1)
#define     DX_FILE_TABLES_POS_DEF(F)   (DX_FILE_FLAGS(F)&2)
#define     DX_FILE_BLOCKS_POS_DEF(F)   (DX_FILE_FLAGS(F)&4)
#define     DX_FILE_ENTITIES_POS_DEF(F) (DX_FILE_FLAGS(F)&8)
#define     DX_FILE_HEADER_OPEN(F)      (DX_FILE_FLAGS(F)&1)
#define     DX_FILE_TABLES_OPEN(F)      (DX_FILE_FLAGS(F)&2)
#define     DX_FILE_BLOCKS_OPEN(F)      (DX_FILE_FLAGS(F)&4)
#define     DX_FILE_ENTITIES_OPEN(F)    (DX_FILE_FLAGS(F)&8)
#define     DX_FILE_HEADER_CLOSED(F)    (DX_FILE_FLAGS(F)&16)
#define     DX_FILE_TABLES_CLOSED(F)    (DX_FILE_FLAGS(F)&32)
#define     DX_FILE_BLOCKS_CLOSED(F)    (DX_FILE_FLAGS(F)&64)
#define     DX_FILE_ENTITIES_CLOSED(F)  (DX_FILE_FLAGS(F)&128)
#define     DX_FILE_LAST_OPEN(F)        (DX_FILE_FLAGS(F)&256)
#define     DX_FILE_LAST_CLOSED(F)      (!DX_FILE_LAST_OPEN(F))
#define     DX_FILE_ENT_HNDL_ENABLED(F) (DX_FILE_FLAGS(F)&512)
#define     DX_FILE_VTCS_DISABLED(F)    (DX_FILE_FLAGS(F)&1024)
#define     DX_FILE_ATTRS_DISABLED(F)   (DX_FILE_FLAGS(F)&2048)
#define     DX_FILE_SEQ_OPEN(F)         (DX_FILE_FLAGS(F)&4096)
#define     DX_FILE_ENT_LAYER(F)        ((F)->layer)
#ifdef  __ACAD11__
#define     DX_FILE_APPDEF_LIST(F)      ((F)->file_appdef_list)
#endif
#define     DX_CREATE_FILE              (MALLOC(1,DXF_FILE_S))
#define     DX_FREE_FILE(F)             FREE(F)

#define     DX_FILE_SET_HEADER_POS(F)   {DX_FILE_FLAGS(F)|=1;}
#define     DX_FILE_SET_TABLES_POS(F)   {DX_FILE_FLAGS(F)|=2;}
#define     DX_FILE_SET_BLOCKS_POS(F)   {DX_FILE_FLAGS(F)|=4;}
#define     DX_FILE_SET_ENTITIES_POS(F) {DX_FILE_FLAGS(F)|=8;}

#define     DX_FILE_SET_HEADER_OPEN(F)      {DX_FILE_FLAGS(F)|=1;}
#define     DX_FILE_SET_TABLES_OPEN(F)      {DX_FILE_FLAGS(F)|=2;}
#define     DX_FILE_SET_BLOCKS_OPEN(F)      {DX_FILE_FLAGS(F)|=4;}
#define     DX_FILE_SET_ENTITIES_OPEN(F)    {DX_FILE_FLAGS(F)|=8;}

#define     DX_FILE_SET_HEADER_CLOSED(F)    {DX_FILE_FLAGS(F)|=16;}
#define     DX_FILE_SET_TABLES_CLOSED(F)    {DX_FILE_FLAGS(F)|=32;}
#define     DX_FILE_SET_BLOCKS_CLOSED(F)    {DX_FILE_FLAGS(F)|=64;}
#define     DX_FILE_SET_ENTITIES_CLOSED(F)  {DX_FILE_FLAGS(F)|=128;}
#define     DX_FILE_SET_ENT_HNDL_ENABLED(F) {DX_FILE_FLAGS(F)|=512;}

#define     DX_FILE_SET_LAST_OPEN(F)        {DX_FILE_FLAGS(F)|=256;}
#define     DX_FILE_SET_LAST_CLOSED(F)      {DX_FILE_FLAGS(F)&=(~256);}

#define     DX_FILE_SET_VTCS_DISABLE(F)     {DX_FILE_FLAGS(F)|=1024;}
#define     DX_FILE_SET_VTCS_ENABLE(F)      {DX_FILE_FLAGS(F)&=(~1024);}
#define     DX_FILE_SET_ATTRS_DISABLE(F)    {DX_FILE_FLAGS(F)|=2048;}
#define     DX_FILE_SET_ATTRS_ENABLE(F)     {DX_FILE_FLAGS(F)&=(~2048);}
#define     DX_FILE_SET_SEQ_OPEN(F)         {DX_FILE_FLAGS(F)|=4096;}
#define     DX_FILE_SET_SEQ_CLOSED(F)       {DX_FILE_FLAGS(F)&=(~4096);}

#endif
