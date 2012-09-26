/****************************** DXUCSM.H ***********************************/ 
/*****************  Include file for DXF file manipulation  ****************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
/*!!!!!!!!       (C) Copyright 1989 - 1995       Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 
#ifndef     DXUCSM 
#define     DXUCSM 
#define     DXF_UCS_NAME(E)         (DX_TE_UCS(E)->ucs_name)
#define     DXF_UCS_FLAGS(E)        (DX_TE_UCS(E)->ucs_flags)
#define     DXF_UCS_ORIGIN(E)       (DX_TE_UCS(E)->ucs_origin)
#define     DXF_UCS_X_AXIS(E)       (DX_TE_UCS(E)->ucs_x_axis)
#define     DXF_UCS_Y_AXIS(E)       (DX_TE_UCS(E)->ucs_y_axis)
#endif
