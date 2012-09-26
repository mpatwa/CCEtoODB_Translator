/****************************** DXDMSD.H ***********************************/ 
#ifndef     DXDMSD 
#define     DXDMSD 
#ifdef __ACAD11__
typedef     struct  dx_dimstyle_struct { 
            DX_STRING   dimstyle_name ;
            DX_STRING   dimstyle_dimpost ;
            DX_STRING   dimstyle_dimapost ;
            char        dimstyle_dimblk[DX_HANDLE_SIZE] ;
            DX_STRING   dimstyle_dimblk1 ;
            DX_STRING   dimstyle_dimblk2 ;
            REAL        dimstyle_dimscale ;
            REAL        dimstyle_dimasz ;
            REAL        dimstyle_dimexo ;
            REAL        dimstyle_dimdli ;
            REAL        dimstyle_dimexe ;
            REAL        dimstyle_dimrnd ;
            REAL        dimstyle_dimdle ;
            REAL        dimstyle_dimtp ;
            REAL        dimstyle_dimtm ;
            INT         dimstyle_dimclrd ;
            INT         dimstyle_dimclrt ;
            INT         dimstyle_dimtol ;
            INT         dimstyle_dimlim ;
            INT         dimstyle_dimtih ;
            INT         dimstyle_dimtoh ;
            INT         dimstyle_dimse1;
            INT         dimstyle_dimse2 ;
            INT         dimstyle_dimtad ;
            INT         dimstyle_dimzin ;
            REAL        dimstyle_dimtxt ;
            REAL        dimstyle_dimcen ;
            REAL        dimstyle_dimtsz ;
            REAL        dimstyle_dimaltf ;
            REAL        dimstyle_dimlfac;
            REAL        dimstyle_dimtvp ;
            INT         dimstyle_dimalt ;
            INT         dimstyle_dimaltd ;
            INT         dimstyle_dimtofl ;
            INT         dimstyle_dimsah ;
            INT         dimstyle_dimtix ;
            INT         dimstyle_dimsoxd ;
            } DX_DIMSTYLE_S ;
typedef     DX_DIMSTYLE_S  *DX_DIMSTYLE ;
#endif
#endif

