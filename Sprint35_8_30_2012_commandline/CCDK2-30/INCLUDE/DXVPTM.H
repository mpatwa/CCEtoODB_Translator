/****************************** DXVPTM.H ***********************************/ 
#ifndef     DXVPTM 
#define     DXVPTM 
#define     DXF_VP_NAME(E)       (DX_TE_VPORT(E)->vport_name)
#define     DXF_VP_FLAGS(E)      (DX_TE_VPORT(E)->vport_flags)
#define     DXF_VP_LL(E)         (DX_TE_VPORT(E)->vport_ll)
#define     DXF_VP_UR(E)         (DX_TE_VPORT(E)->vport_ur)
#define     DXF_VP_CTR(E)        (DX_TE_VPORT(E)->vport_ctr)
#define     DXF_VP_SNAP_BPT(E)   (DX_TE_VPORT(E)->vport_snap_bpt)
#define     DXF_VP_SNAP_SPC(E)   (DX_TE_VPORT(E)->vport_snap_spc)
#define     DXF_VP_GRID_SPC(E)   (DX_TE_VPORT(E)->vport_grid_spc)
#define     DXF_VP_DIR(E)        (DX_TE_VPORT(E)->vport_dir)
#define     DXF_VP_TGT_PT(E)     (DX_TE_VPORT(E)->vport_tgt_pt)
#define     DXF_VP_HEIGHT(E)     (DX_TE_VPORT(E)->vport_height)
#define     DXF_VP_AR(E)         (DX_TE_VPORT(E)->vport_ar)
#define     DXF_VP_LENS_LEN(E)   (DX_TE_VPORT(E)->vport_lens_len)
#define     DXF_VP_FR_CLIP(E)    (DX_TE_VPORT(E)->vport_fr_clip)
#define     DXF_VP_BK_CLIP(E)    (DX_TE_VPORT(E)->vport_bk_clip)
#define     DXF_VP_SNAP_ANG(E)   (DX_TE_VPORT(E)->vport_snap_ang)
#define     DXF_VP_TWIST(E)      (DX_TE_VPORT(E)->vport_twist)
#ifdef __ACAD11__
#define     DXF_VP_STATUS(E)     (DX_TE_VPORT(E)->vport_status)
#define     DXF_VP_ID(E)         (DX_TE_VPORT(E)->vport_id)
#endif
#define     DXF_VP_MODE(E)       (DX_TE_VPORT(E)->vport_mode)
#define     DXF_VP_CZOOM(E)      (DX_TE_VPORT(E)->vport_czoom)
#define     DXF_VP_FZOOM(E)      (DX_TE_VPORT(E)->vport_fzoom)
#define     DXF_VP_UCSICON(E)    (DX_TE_VPORT(E)->vport_ucsicon)
#define     DXF_VP_SNAP(E)       (DX_TE_VPORT(E)->vport_snap)
#define     DXF_VP_GRID(E)       (DX_TE_VPORT(E)->vport_grid)
#define     DXF_VP_SNAP_STYLE(E) (DX_TE_VPORT(E)->vport_snap_style)
#define     DXF_VP_SNAP_ISOPAIR(E) (DX_TE_VPORT(E)->vport_snap_isopair)
#endif
