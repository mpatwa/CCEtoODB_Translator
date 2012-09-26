/***************************** DX0DEFS1.H **********************************/
/*******************  Prototypes for dxf_get routines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DX0DEFS1
#define     DX0DEFS1

extern  DXF_FILE_STATUS dx0_set_file_status __(( DXF_FILE DUMMY0 , 
            INT DUMMY1 )) ;
extern  DXF_ENTITY  dx0_alloc_entity __(( DXF_ENTITY_ID DUMMY0 , 
            DXF_FILE DUMMY1 )) ;
extern  DXF_TABLE_ENTRY dx0_alloc_table_entry __(( 
            DXF_TABLE_ENTRY_ID DUMMY0 , DXF_FILE DUMMY1 )) ;
extern  BOOLEAN     dx0_put_long __(( DXF_FILE DUMMY0 , long DUMMY1 )) ;
extern  BOOLEAN     dx0_put_long_group __(( DXF_FILE DUMMY0 , int DUMMY1 ,
            long DUMMY2 )) ;
extern  BOOLEAN     dx0_put_long_mode __(( DXF_FILE DUMMY0 , int DUMMY1 ,
            int DUMMY2 , long DUMMY3 )) ;
extern  int*        dx0_table_entry_template __(( DXF_TABLE_ENTRY_ID DUMMY0 , 
            INT DUMMY1 )) ;
extern  int*        dx0_entity_template __(( DXF_ENTITY_ID DUMMY0 , 
            INT DUMMY1 )) ;
extern  void        dx0_set_entity_descriptor __(( DXF_FILE DUMMY0 ,
            DXF_ENTITY DUMMY1 , DXF_ENTITY_DESC DUMMY2 )) ;
extern  DXF_ENTITY  dx0_create_attrib __(( DXF_FILE DUMMY0 , REAL DUMMY1 [3], 
            REAL DUMMY2 , STRING DUMMY3 , STRING DUMMY4 , INT DUMMY5 , 
            INT DUMMY6 , REAL DUMMY7 , REAL DUMMY8 , STRING DUMMY9 )) ;
extern  DXF_ENTITY  dx0_create_vertex __(( DXF_FILE DUMMY0 , REAL DUMMY1 [3], 
            REAL DUMMY2 , INT DUMMY3 )) ;
extern  INT         dx0_file_release __(( DXF_FILE DUMMY0 )) ;
extern  DXF_BLOCK   dx0_alloc_block __(( INT DUMMY0 )) ;
extern  void        dx0_free_appdef __(( DX_APPDEF DUMMY0 )) ;
extern  DX_APPDEF   dx0_find_appdef __(( DXF_FILE DUMMY0 , STRING DUMMY1 )) ;
extern  ANY         dx0_create_record __(( int* DUMMY0 )) ;
extern  DX_APPDAT   dx0_create_appdat __(( DXF_FILE DUMMY0 , STRING DUMMY1 )) ;
extern  void        dx0_free_record __(( ANY DUMMY0 , int* DUMMY1 )) ;
extern  void        dx0_free_appdat __(( DX_APPDAT DUMMY0 )) ;
extern  BOOLEAN     dx0_put_appl_data __(( DXF_FILE, DXF_ENTITY )) ;
extern  void        dx0_appdat_filter __(( DML_LIST, DXF_FILTER )) ;
extern  BOOLEAN     dx0_get_appl_data __(( DXF_FILE, DXF_ENTITY )) ;
extern  STRING      dx0_appdat_name __(( DX_APPDAT )) ;
extern  DX_APPDAT   dx0_create_appdat_template __(( int* )) ;
#endif     /*DX0DEFS1*/ 

