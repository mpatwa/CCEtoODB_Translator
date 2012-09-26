/****************************** DXINSD.H ***********************************/ 
#ifndef     DXINSD 
#define     DXINSD 
typedef     struct  dx_insert_struct {
            DXF_ENTITY_HEADER_S    insert_entity_header ;
            INT     insert_flags ;
#ifdef __UNIX__
            DX_INT_FILLER   insert_filler ;
#endif
            DX_STRING  insert_block ;
#ifdef __MSC32__
            DX_INT_FILLER   insert_filler ;
#endif
            REAL    insert_pt[3] ;
            REAL    insert_x_scale ;
            REAL    insert_y_scale ;
            REAL    insert_z_scale ;
            REAL    insert_rot_ang ;
            INT     insert_cols_cnt ;
            INT     insert_rows_cnt ;
            REAL    insert_col_spc ;
            REAL    insert_row_spc ;
            DXF_ENAME   insert_seqend ;
            DML_LIST    insert_attrs ;
            } DX_INSERT_S ;
typedef     DX_INSERT_S *DX_INSERT ;
#endif

