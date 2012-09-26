/****************************** DXFILD.H ***********************************/ 

//#ifndef fpos_t
// #define fpos_t long
//#endif

#ifndef DXFILD
#define DXFILD
typedef     INT                         DXF_FILE_STATUS ;
#define     DXF_FILE_INIT               (DXF_FILE_STATUS)0
#define     DXF_ENTITY_UNKNOWN          (DXF_FILE_STATUS)1
#define     DXF_TABLE_ENTRY_UNKNOWN     (DXF_FILE_STATUS)2
#define     DXF_MALLOC_FAILURE          (DXF_FILE_STATUS)4
#define     DXF_NO_OBJECTS_FOUND        (DXF_FILE_STATUS)8

typedef     INT             DXF_FILE_MODE ;
#define     DXF_ASCII       (DXF_FILE_MODE)1
#define     DXF_BINARY      (DXF_FILE_MODE)2
#define     DXF_READ        (DXF_FILE_MODE)4
#define     DXF_WRITE       (DXF_FILE_MODE)8

typedef     struct  dx_file_struct {
            FILE                *fp ;
            DXF_FILE_MODE       file_type ; 
            INT                 file_prec ; 
#ifdef  __ACAD11__
            INT                 file_release ; 
#endif
            DXF_FILE_STATUS     file_status ; 
            fpos_t              file_header_pos ;
            fpos_t              file_tables_pos ;
            fpos_t              file_blocks_pos ;
            fpos_t              file_entities_pos ;
            DML_LIST            file_blockpos_list ;
            DML_LIST            file_unrec_list ;
            INT                 file_flags ;
            INT                 file_splinesegs ;
            long                file_length ;
            DX_STRING           layer ;
#ifdef  __ACAD11__
            DML_LIST            file_appdef_list ;
#endif
            } DXF_FILE_S ;
typedef     DXF_FILE_S  *DXF_FILE ;
#define     DXF_SPLINESEGS      8
#endif

