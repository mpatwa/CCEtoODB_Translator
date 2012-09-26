/****************************** DXVPTD.H ***********************************/ 
#ifndef     DXVPTD 
#define     DXVPTD 
typedef     struct  dx_vport_struct {
            DX_STRING  vport_name ;
            INT     vport_flags ;
#ifdef __UNIX__
            DX_INT_FILLER   vport_filler ;
#endif
#ifdef __MSC32__
            DX_INT_FILLER   vport_filler ;
#endif
            REAL    vport_ll[2] ;
            REAL    vport_ur[2] ;
            REAL    vport_ctr[2] ;
            REAL    vport_snap_bpt[2] ;
            REAL    vport_snap_spc[2] ;
            REAL    vport_grid_spc[2] ;
            REAL    vport_dir[3] ;
            REAL    vport_tgt_pt[3] ;
            REAL    vport_height ;
            REAL    vport_ar ;
            REAL    vport_lens_len ;
            REAL    vport_fr_clip ;
            REAL    vport_bk_clip ;
            REAL    vport_snap_ang ;
            REAL    vport_twist ;
#ifdef __ACAD11__
            INT     vport_status ;
            INT     vport_id ;
#endif
            INT     vport_mode ;
            INT     vport_czoom ;
            INT     vport_fzoom ;
            INT     vport_ucsicon ;
            INT     vport_snap ;
            INT     vport_grid ;
            INT     vport_snap_style ;
            INT     vport_snap_isopair ;
            } DX_VPORT_S ;
typedef     DX_VPORT_S  *DX_VPORT ;
#endif
