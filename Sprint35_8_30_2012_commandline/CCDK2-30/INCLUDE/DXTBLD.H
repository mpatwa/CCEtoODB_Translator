/****************************** DXTBLD.H ***********************************/ 
#ifndef     DXTBLD 
#define     DXTBLD 

typedef     INT     DXF_TABLE_ENTRY_ID ;

#define     DXF_LTYPE         (DXF_TABLE_ENTRY_ID)1
#define     DXF_LAYER         (DXF_TABLE_ENTRY_ID)2
#define     DXF_STYLE         (DXF_TABLE_ENTRY_ID)3
#define     DXF_VIEW          (DXF_TABLE_ENTRY_ID)4
#define     DXF_UCS           (DXF_TABLE_ENTRY_ID)5
#define     DXF_VPORT         (DXF_TABLE_ENTRY_ID)6
#ifdef __ACAD11__
#define     DXF_DIMSTYLE      (DXF_TABLE_ENTRY_ID)7
#define     DXF_APPID         (DXF_TABLE_ENTRY_ID)8
#endif

typedef     struct  dx_te_struct {
            DXF_ENAME       te_ename ;
            DXF_TABLE_ENTRY_ID  te_id ;
#ifdef __MSC32__
            DX_INT_FILLER   te_filler ;
#endif
            union {
                STRING      te_anytype ;
#ifdef DX_TBL_ON
                DX_LTYPE_S  te_ltype ;
#ifdef __ACAD11__
                DX_APPID_S  te_appid ;
                DX_DIMSTYLE_S  te_dimstyle ;
#endif
                DX_LAYER_S  te_layer ;
                DX_STYLE_S  te_style ;
                DX_VIEW_S   te_view ;
                DX_UCS_S    te_ucs ;
                DX_VPORT_S  te_vport ;
#endif  /* DX_TBL_ON */
            }   te_data ;
            } DXF_TABLE_ENTRY_S ;
typedef     DXF_TABLE_ENTRY_S *DXF_TABLE_ENTRY ;
#endif

