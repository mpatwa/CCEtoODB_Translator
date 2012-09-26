/****************************** DXVIPD.H ***********************************/
#ifndef     DXVIPD 
#define     DXVIPD 
#ifdef __ACAD11__
typedef     struct  dx_viewport_struct {
            DXF_ENTITY_HEADER_S     vp_entity_header ;
            REAL                    vp_ctr_pt[3] ;
            REAL                    vp_width ;
            REAL                    vp_height ;
            INT                     vp_id ;
            INT                     vp_status ;
            DX_STRING               vp_paper_space ;
            DX_STRING               vp_data_begin ;
            DX_STRING               vp_wnd_data ;
            INT                     vp_version ;
#ifdef __UNIX__
            DX_INT_FILLER           vp_filler1 ;
#endif
#ifdef __MSC32__
            DX_INT_FILLER           vp_filler ;
#endif
            REAL                    vp_target_pt[3] ;
            REAL                    vp_dir_vec[3] ;
            REAL                    vp_twist_angle ;
            REAL                    vp_view_height ;
            REAL                    vp_view_ctr_pt[2] ;
            REAL                    vp_lens_length ;
            REAL                    vp_front_z ;
            REAL                    vp_back_z ;
            INT                     vp_mode ;
            INT                     vp_crcl_zoom ;
            INT                     vp_fast_zoom ;
            INT                     vp_ucsicon ;
            INT                     vp_snap ;
            INT                     vp_grid ;
            INT                     vp_snap_style ;
            INT                     vp_snap_isopair ;
            REAL                    vp_snap_angle ;
            REAL                    vp_snap_pt[2] ;
            REAL                    vp_snap_spacing[2] ;
            REAL                    vp_grid_spacing[2] ;
            INT                     vp_hidden ;
#ifdef __UNIX__
            DX_INT_FILLER           vp_filler2 ;
#endif
            DX_STRING               vp_frozen_begin ;
            DML_LIST                vp_layer_list ;
            DX_STRING               vp_frozen_end ;
            DX_STRING               vp_data_end ;
            } DX_VIEWPORT_S ;
typedef     DX_VIEWPORT_S *DX_VIEWPORT ;
#endif
#endif

