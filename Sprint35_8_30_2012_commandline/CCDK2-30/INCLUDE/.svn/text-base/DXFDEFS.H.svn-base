/***************************** DXFDEFS.H ***********************************/
/*******************  Prototypes for dxf routines **************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995    Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef     DXFDEFS
#define     DXFDEFS

#include <dxents.h>
#include <dxtbls.h>

#include <dxf11.h>
#include <dxdefs.h>
#define dxf_create_face3d dxf_create_3dface 

extern  DXF_FILE        dxf_fopen __(( STRING DUMMY0 , DXF_FILE_MODE DUMMY1 )) ;
extern  BOOLEAN         dxf_fclose __(( DXF_FILE DUMMY0 )) ;
extern  DXF_FILE_STATUS dxf_file_status __(( DXF_FILE DUMMY0 )) ;
extern  DML_LIST        dxf_unrec_list __(( DXF_FILE DUMMY0 )) ;
extern  INT             dxf_read_unrec __(( DXF_FILE DUMMY0 , fpos_t DUMMY1 , 
            ANY DUMMY2 )) ;
extern  INT             dxf_get_precision __(( DXF_FILE DUMMY0 )) ;
extern  void            dxf_set_precision __(( DXF_FILE DUMMY0 , INT DUMMY1 )) ;
extern  INT             dxf_get_splinesegs __(( DXF_FILE DUMMY0 )) ;
extern  void            dxf_set_splinesegs __(( DXF_FILE DUMMY0 , 
            INT DUMMY0 )) ;
extern  char*           dxf_get_filepos __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_set_filepos __(( DXF_FILE DUMMY0 , ANY DUMMY1 ,
            DXF_FILEPOS_MODE DUMMY2 )) ;
extern  void            dxf_vtcs_disable __(( DXF_FILE DUMMY0 )) ;
extern  void            dxf_vtcs_enable __(( DXF_FILE DUMMY0 )) ;
extern  void            dxf_attrs_disable __(( DXF_FILE DUMMY0 )) ;
extern  void            dxf_attrs_enable __(( DXF_FILE DUMMY0 )) ;
extern  void            dxf_free_entity __(( DXF_ENTITY DUMMY0 )) ;
extern  void            dxf_flush __(( void )) ;
extern  void            dxf_free_table_entry __(( DXF_TABLE_ENTRY DUMMY0 )) ;
extern  ANY             dxf_get_hdr_var __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            ANY DUMMY2 )) ;
extern  BOOLEAN         dxf_read_hdr_var __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            DXF_HDR_VAR DUMMY2 )) ;
extern  DXF_ENTITY      dxf_get_entity __(( DXF_FILE DUMMY0 , 
            DXF_FILTER DUMMY1 )) ;
extern  void            dxf_set_entity_layer __(( DXF_ENTITY DUMMY0 ,
            STRING DUMMY1 )) ;
extern  void            dxf_set_entity_handle __(( DXF_ENTITY DUMMY0 ,
            STRING DUMMY1 )) ;
extern  void            dxf_set_entity_ltype __(( DXF_ENTITY DUMMY0 ,
            STRING DUMMY1 )) ;
BBS_PUBLIC void dxf_set_entity_ltype_bylayer __(( DXF_ENTITY DUMMY0 )) ;
extern  void            dxf_set_entity_thickness __(( DXF_ENTITY DUMMY0 ,
            REAL DUMMY1 )) ;
extern  void            dxf_set_entity_color __(( DXF_ENTITY DUMMY0 ,
            INT DUMMY1 )) ;
extern  BBS_PUBLIC void dxf_set_entity_color_bylayer 
            __(( DXF_ENTITY DUMMY0 )) ;
extern  void            dxf_set_entity_extrusion __(( DXF_ENTITY DUMMY0 ,
            REAL DUMMY1 [3] )) ;
extern  STRING          dxf_get_entity_layer __(( DXF_ENTITY DUMMY0 )) ;
extern  STRING          dxf_get_entity_handle __(( DXF_ENTITY DUMMY0 )) ;
extern  STRING          dxf_get_entity_ltype __(( DXF_ENTITY DUMMY0 )) ;
extern  INT             dxf_get_entity_ltype_status __(( DXF_ENTITY DUMMY0 )) ;
extern  REAL            dxf_get_entity_thickness __(( DXF_ENTITY DUMMY0 )) ;
extern  INT             dxf_get_entity_color __(( DXF_ENTITY DUMMY0 )) ;
extern  INT             dxf_get_entity_color_status __(( DXF_ENTITY DUMMY0 )) ;
extern  REAL           *dxf_get_entity_extrusion __(( DXF_ENTITY DUMMY0 )) ;
extern  DXF_ENTITY_DESC dxf_create_descriptor __(( STRING DUMMY0 ,
            STRING DUMMY1 ,  INT DUMMY2 , REAL DUMMY3 , REAL DUMMY4 [3] )) ;
extern  void            dxf_free_descriptor __(( DXF_ENTITY_DESC DUMMY0 )) ;
extern  BOOLEAN         dxf_put_line __(( DXF_FILE DUMMY0 , REAL DUMMY1 [3], 
            REAL DUMMY2 [3] )) ;
extern  BOOLEAN         dxf_put_point __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3] )) ;
extern  BOOLEAN         dxf_put_circle __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 )) ;
extern  BOOLEAN         dxf_put_arc __(( DXF_FILE DUMMY0, REAL DUMMY1 [3], 
            REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 )) ;
extern  BOOLEAN         dxf_put_trace __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3], REAL DUMMY3 [3], 
            REAL DUMMY4 [3] )) ;
extern  BOOLEAN         dxf_put_solid __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3], REAL DUMMY3 [3], 
            REAL DUMMY4 [3] )) ;
extern  BOOLEAN         dxf_put_text __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 , STRING DUMMY3 , REAL DUMMY4 , 
            REAL DUMMY5 , STRING DUMMY6 )) ;
extern  BOOLEAN         dxf_put_shape __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 , STRING DUMMY3 ,  REAL DUMMY4 , 
            REAL DUMMY5 )) ;
extern  BOOLEAN         dxf_put_insert __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 [3], REAL DUMMY3 , REAL DUMMY4 , 
            INT DUMMY5 , INT DUMMY6 , REAL DUMMY7 , REAL DUMMY8 , 
            DML_LIST DUMMY9 )) ;
extern  BOOLEAN         dxf_put_attdef __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 , STRING DUMMY3 , STRING DUMMY4 , 
            STRING DUMMY5 , INT DUMMY6 , INT DUMMY7 , REAL DUMMY8 , 
            REAL DUMMY9 , STRING DUMMY10 )) ;
extern  BOOLEAN         dxf_put_polyline __(( DXF_FILE DUMMY0 , 
            INT DUMMY1 , INT DUMMY2 , DML_LIST DUMMY3 )) ;
extern  BOOLEAN         dxf_put_line3d __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3] )) ;
extern  BOOLEAN         dxf_put_3dface __(( DXF_FILE DUMMY0 , 
            REAL DUMMY1 [3], REAL DUMMY2 [3], REAL DUMMY3 [3], 
            REAL DUMMY4 [3], INT DUMMY5 )) ;
extern  BOOLEAN         dxf_put_dimension __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 [3], REAL DUMMY3 [3], REAL DUMMY4 [3], 
            INT DUMMY5 , STRING DUMMY6 , REAL DUMMY7 [3], REAL DUMMY8 [3], 
            REAL DUMMY9 [3], REAL DUMMY10 [3], REAL DUMMY11 , REAL DUMMY12 )) ;
extern  DXF_BLOCK       dxf_create_block __(( STRING DUMMY0 , STRING DUMMY1 ,
            INT DUMMY2 , REAL DUMMY3 [3] )) ;
extern  void            dxf_free_block __(( DXF_BLOCK DUMMY0 )) ;
extern  DXF_BLOCK       dxf_get_block __(( DXF_FILE DUMMY0 , STRING DUMMY1 )) ;
extern  DML_LIST        dxf_get_blocks __(( DXF_FILE DUMMY0, 
            DML_LIST DUMMY1 )) ;
extern  DML_LIST        dxf_read_blocks __(( STRING DUMMY0, 
            DML_LIST DUMMY1 )) ;
extern  BOOLEAN         dxf_put_hdr_var __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            ANY DUMMY2 )) ;
extern  BOOLEAN         dxf_put_entity __(( DXF_FILE DUMMY0 , 
            DXF_ENTITY DUMMY1 )) ;
extern  DML_LIST        dxf_get_entities __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
             DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  DML_LIST        dxf_read_entities __(( STRING DUMMY0 , STRING DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  BOOLEAN         dxf_put_entities __(( DXF_FILE DUMMY0 , 
            DML_LIST DUMMY1 )) ;
extern  BOOLEAN         dxf_write_entities __(( STRING DUMMY0 ,  
            DXF_FILE_MODE DUMMY1 , DML_LIST DUMMY2 )) ;
extern  BOOLEAN         dxf_block_base_pt __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            REAL DUMMY2 [3] )) ;
extern  DXF_TABLE_ENTRY dxf_get_table_entry __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_put_table_entry __(( DXF_FILE DUMMY0 ,
            DXF_TABLE_ENTRY DUMMY1 )) ;
extern  DML_LIST        dxf_get_table_entries __(( DXF_FILE DUMMY0 ,
             DXF_TABLE_ENTRY_ID DUMMY1, DML_LIST DUMMY2 )) ;
extern  DML_LIST        dxf_read_table_entries __(( STRING DUMMY0 ,
            DXF_TABLE_ENTRY_ID DUMMY1, DML_LIST DUMMY2 )) ;
extern  BOOLEAN         dxf_put_table_entries __(( DXF_FILE DUMMY0 ,
            DML_LIST DUMMY1 )) ;
extern  BOOLEAN         dxf_open_header __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_open_tables __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_open_blocks __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_open_block __(( DXF_FILE DUMMY0 , STRING DUMMY1 ,
            STRING DUMMY2 ,  INT DUMMY3 , REAL DUMMY4 [3] )) ;
extern  BOOLEAN         dxf_open_entities __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_open_table __(( DXF_FILE DUMMY0 , 
            DXF_TABLE_ENTRY_ID DUMMY1 , INT DUMMY2 )) ;
extern  BOOLEAN         dxf_close_header __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_close_tables __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_close_blocks __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_close_entities __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_close_table __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_close_block __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN         dxf_put_comment __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 )) ;
extern  DML_LIST        dxf_explode __(( DXF_FILE DUMMY0 , DXF_ENTITY DUMMY1 ,
            DXF_FILTER DUMMY2, DML_LIST DUMMY3 )) ;
extern  DXF_FILTER      dxf_create_filter __(( DXF_ENTITY_ID DUMMY0 ,
            STRING DUMMY1 , INT DUMMY2 )) ;
extern  void            dxf_filter_set_type __(( DXF_FILTER DUMMY0 ,
            DXF_ENTITY_ID DUMMY1 )) ;
extern  void            dxf_filter_add_type __(( DXF_FILTER DUMMY0 ,
            DXF_ENTITY_ID DUMMY1 )) ;
extern  void            dxf_filter_set_layer __(( DXF_FILTER DUMMY0 ,
            STRING DUMMY1 )) ;
extern  void            dxf_filter_set_color __(( DXF_FILTER DUMMY0 ,
            INT DUMMY1 )) ;
extern  void            dxf_free_filter __(( DXF_FILTER DUMMY0 )) ;
extern  BOOLEAN    dxf_put_layer __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , INT DUMMY2 , BOOLEAN DUMMY3 , STRING DUMMY4 )) ;
extern  BOOLEAN    dxf_put_ltype __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , STRING DUMMY2 ,
            INT DUMMY3 , REAL DUMMY4 , INT DUMMY5 , REAL* DUMMY6 )) ;
extern  BOOLEAN    dxf_put_style __(( DXF_FILE DUMMY0 , STRING DUMMY1 , 
            INT DUMMY2 , REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 , 
            INT DUMMY6 , REAL DUMMY7 , STRING DUMMY8 , STRING DUMMY9 )) ;
extern  BOOLEAN    dxf_put_view __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 , REAL DUMMY3 , REAL DUMMY4 [3], 
            REAL DUMMY5 [3], REAL DUMMY6 [3] )) ;
extern  BOOLEAN    dxf_put_ucs __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1, REAL DUMMY2 [3],
            REAL DUMMY3 [3], REAL DUMMY4 [3] )) ;
extern  BOOLEAN    dxf_put_vport __(( DXF_FILE DUMMY0 , 
            STRING DUMMY1 , REAL DUMMY2 ,
            REAL DUMMY3 , REAL DUMMY4 , REAL DUMMY5 ,  REAL DUMMY6 [2],
            REAL DUMMY7 [3], REAL DUMMY8 [3], REAL DUMMY9 , REAL DUMMY10 )) ;
extern  INT             dxf_hdr_var_sizeof __(( STRING DUMMY0 )) ;
extern  long dxf_ftell __(( DXF_FILE DUMMY0 )) ;
extern  long dxf_filelength __(( DXF_FILE DUMMY0 )) ;
extern  REAL dxf_current_pos __(( DXF_FILE DUMMY0 )) ;
extern  BOOLEAN dxf_put_ext_entity __(( DXF_FILE DUMMY0 , DXF_ENTITY DUMMY1 , 
            INT* DUMMY2 )) ;
extern  BOOLEAN dxf_put_ext_entities __(( DXF_FILE DUMMY0 , DML_LIST DUMMY1 , 
            INT* DUMMY2 )) ;
#ifdef __ACAD11__
#include <dxfdefs1.h>
#else
#include <dxfdefs0.h>
#endif
#endif     /*DXFDEFS*/
