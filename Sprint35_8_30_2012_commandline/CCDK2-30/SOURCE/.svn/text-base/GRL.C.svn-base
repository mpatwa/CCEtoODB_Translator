/* -2 -3 */
/******************************** GRL.S ************************************/
/********************** Lowest Level Graphic Routines **********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifdef  __TURBOC__
#ifndef _Windows
#include <dos.h>
#include <stdlib.h>
#endif // _Windows
#endif  /*__TURBOC__*/

#include <grldefs.h>

GLOBAL  REAL    GR_SCREEN_RATIO = 1.3333333333 ;

#ifdef __TURBOC__
#ifdef __WINDOWS__
#undef __TURBOC__
#endif // __WINDOWS__
#endif // __TURBOC__

#ifdef  __GFX__
STATIC void grl_box_corners __(( GR_PIX_COORD* DUMMY0 , GR_PIX_COORD* DUMMY1
,                                                          
    GR_PIX_COORD* DUMMY2 , GR_PIX_COORD* DUMMY3 )) ;
STATIC void grl_clip_line __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 , 
    GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
GLOBAL  GR_PIX_COORD    GRL_X = 0 ;
GLOBAL  GR_PIX_COORD    GRL_Y = 0 ;
GLOBAL  GR_COLOR        GRL_COLOR = GR_BLACK ;
GLOBAL  GR_PATT         GRL_PATT = GR_SOLID_PATT ;
GLOBAL  int             GRL_VIEW_HANDLE = 0 ;
GLOBAL  int             GRL_X0, GRL_Y0, GRL_X1, GRL_Y1 ;
#endif 

//  MS Windows support

#ifdef __WINDOWS__
 
static COLORREF colors[] = {
	// Standard VGA palette        
	RGB(0,  0,  0   ),          //  0 white
	RGB(0,  0,  128 ),          //  1 blue
	RGB(0,  128,0   ),          //  2 green
	RGB(0,  128,128 ),          //  3 cyan
	RGB(128,0,  0   ),          //  4 red 
	RGB(128,0,  128 ),          //  5 magenta 
	RGB(128,128,0   ),          //  6 brown
	RGB(192,192,192 ),          //  7 light grey
	RGB(128,128,128 ),          //  8 dark grey
	RGB(0,  0,  255 ),          //  9 light blue
	RGB(0,  255,0   ),          // 10 light green 
	RGB(0,  255,255 ),          // 11 light cyan
	RGB(255,0,  0   ),          // 12 light red 
	RGB(255,0,  255 ),          // 13 light magenta
	RGB(255,255,0   ),          // 14 yellow
	RGB(255,255,255 )           // 15 white
	} ;

typedef struct _wininfo_ {
	RECT                    cliprect ;
	HWND                    window ;
	HDC                     curDC ;
	GR_PIX_COORD    		maxx ;
	GR_PIX_COORD    		maxy ;
	HPEN            		pen ;
	HBRUSH                  brush ;
	GR_COLOR        		curColor ;
	GR_PATT         		curLinePatt ;
	REAL                    XScale, YScale ;
	int                     XTextPos, YTextPos ;
	GR_TOOL                 tool ;
	HRGN					cliprgn;

   } WIN_INFO_S, *WIN_INFO ;

static WIN_INFO_S window_info ;
static WIN_INFO winfo = NULL ;

void grl_init ( void )
{
	LOGPEN lp;
	LOGBRUSH lb;

	winfo = &window_info;

	WINFO_PEN(winfo) = GetStockObject ( BLACK_PEN ) ;
	GetObject ( WINFO_PEN(winfo), sizeof (LOGPEN), (LPSTR) &lp ) ;
	WINFO_PEN(winfo) = CreatePenIndirect ( &lp ) ;

	WINFO_BRUSH(winfo) = GetStockObject ( BLACK_BRUSH ) ;
	GetObject ( WINFO_BRUSH(winfo), sizeof (LOGBRUSH), (LPSTR) &lb ) ;
	WINFO_BRUSH(winfo) = CreateBrushIndirect ( &lb ) ;
}

void grl_set_hwnd ( HWND hwnd )
{
    RECT r ; 
    REAL maxdim;

	if ( winfo != NULL )
	{
		WINFO_WINDOW(winfo) = hwnd;
	   	WINFO_CLIPRGN ( winfo ) = ( HRGN ) 0;
		WINFO_COLOR( winfo ) = -1 ;
		WINFO_PATT( winfo ) = GR_SOLID_PATT;
		WINFO_TOOL( winfo ) = 0 ;

	    GetClientRect ( hwnd, &r ) ;
	    WINFO_MAXX( winfo ) = ( GR_PIX_COORD ) r.right;
	    WINFO_MAXY( winfo ) = ( GR_PIX_COORD ) r.bottom;
	    maxdim = (r.right > r.bottom) ? r.right : r.bottom;
	    WINFO_XSCALE( winfo ) = (REAL) r.bottom / maxdim;
	    WINFO_YSCALE( winfo ) = (REAL) r.right / maxdim;
	    // set ratio used when _next_ viewport is created
	    grl_set_screen_ratio((REAL) WINFO_MAXX( winfo ) / (REAL) WINFO_MAXY( winfo ));
    }    
}

void grl_set_hdc __(( HWND hwnd, HDC dc ))
{   
    RECT r ; 
    REAL maxdim;

    WINFO_DC( winfo ) = dc ; 
	WINFO_WINDOW(winfo) = hwnd;
    GetClientRect ( hwnd, &r ) ;
    WINFO_MAXX( winfo ) = ( GR_PIX_COORD ) r.right;
    WINFO_MAXY( winfo ) = ( GR_PIX_COORD ) r.bottom;
    maxdim = (r.right > r.bottom) ? r.right : r.bottom;
    WINFO_XSCALE( winfo ) = (REAL) r.bottom / maxdim;
    WINFO_YSCALE( winfo ) = (REAL) r.right / maxdim;
    grl_set_screen_ratio((REAL) WINFO_MAXX( winfo ) / (REAL) WINFO_MAXY( winfo ));
//	grl_set_extents ( ) ;
}

void grl_unset_hdc __(( void ))
{
    WINFO_DC( winfo ) = NULL ; 
	WINFO_WINDOW(winfo) = NULL ;
}

HDC grl_get_hdc __(( void ))
{
    return WINFO_DC( winfo );
}

void grl_set_hpen __(( HPEN pen ))
{
	WINFO_PEN( winfo ) = pen ;
}

HPEN grl_get_hpen     __(( void ))
{
	return WINFO_PEN( winfo ) ;
}

void grl_set_hbrush __(( HBRUSH brush))
{
	WINFO_BRUSH( winfo ) = brush ;
}

HBRUSH grl_get_hbrush __(( void ))
{
	return WINFO_BRUSH( winfo ) ;
}


void grl_exit ( void )
{
	if ( winfo != NULL )
	{
 		DeleteObject ( WINFO_PEN(winfo) );
 		DeleteObject ( WINFO_BRUSH(winfo) );
 	}
}

#ifdef OLD_CODE 
void grl_set_cur_window __(( HWND w ))
{
    RECT r ; 
    REAL maxdim;

    if ( winfo == NULL )
    {
		winfo = &window_info ;
		WINFO_PEN( winfo ) = (HPEN) 0 ;
		WINFO_BRUSH( winfo ) = (HBRUSH) 0 ;
   		WINFO_CLIPRGN ( winfo ) = ( HRGN ) 0;
		WINFO_COLOR( winfo ) = -1 ;
		WINFO_PATT( winfo ) = GR_SOLID_PATT;
		WINFO_TOOL( winfo ) = 0 ;
    }
    WINFO_WINDOW( winfo ) = w ;
    GetClientRect ( w, &r ) ;
    WINFO_MAXX( winfo ) = ( GR_PIX_COORD ) r.right;
    WINFO_MAXY( winfo ) = ( GR_PIX_COORD ) r.bottom;
    maxdim = (r.right > r.bottom) ? r.right : r.bottom;
    WINFO_XSCALE( winfo ) = (REAL) r.bottom / maxdim;
    WINFO_YSCALE( winfo ) = (REAL) r.right / maxdim;
    // set ratio used when _next_ viewport is created
    grl_set_screen_ratio((REAL) WINFO_MAXX( winfo ) / (REAL) WINFO_MAXY( winfo ));    
}

#endif // OLD_CODE

//  MS Windows support

void grl_set_extents __(( ))
{
	WINFO_MAXX( winfo ) = grl_get_max_x ( ) ;
	WINFO_MAXY( winfo ) = grl_get_max_y ( ) ;
    grl_set_screen_ratio((REAL) WINFO_MAXX( winfo ) / (REAL) WINFO_MAXY( winfo ));    
}


// HWND grl_get_cur_window __(( ))
HWND grl_get_hwnd __(( ))
{                        
    return WINFO_WINDOW( winfo );
}    

#endif // WINDOWS (MS Windows)

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_moveto __(( GR_PIX_COORD x, GR_PIX_COORD y ))
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
//GR_PIX_COORD x, y ;
{
#ifdef __WINDOWS__
    if ( WINFO_DC( winfo ) == (HDC) 0 )
	return ;
	#ifdef __WIN32__
    MoveToEx ( WINFO_DC( winfo ), x, y, NULL ) ;
	#else
	MoveTo ( WINFO_DC( winfo ), x, y ) ;
	#endif
#endif

#ifdef __MCRGRAPH__
    _moveto ( x, y ) ;
#endif

#ifdef __TURBOC__
    moveto ( x, y ) ;
#endif

#ifdef __GFX__
    GRL_X = x + GRL_X0 ;
    GRL_Y = y + GRL_Y0 ;
#endif
}

/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_lineto ( x, y )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
{
#ifdef __WINDOWS__
    if ( WINFO_DC( winfo ) == (HDC) 0 )
		{
	return ;
		}
	else
		{
	    LineTo ( WINFO_DC( winfo ), x, y );
		}
#endif

#ifdef __MCRGRAPH__
    _lineto ( x, y ) ;
#endif

#ifdef __TURBOC__
    lineto ( x, y ) ;
#endif

#ifdef __GFX__
    x += GRL_X0 ;
    y += GRL_Y0 ;
    grl_clip_line ( GRL_X, GRL_Y, x, y ) ;
    GRL_X = x ;
    GRL_Y = y ;
#endif
}

#ifdef __WINDOWS__
void grl_polyline ( INT *pPixels, INT iNumPixels )
{
    if ( WINFO_DC( winfo ) != (HDC)NULL )
	Polyline ( WINFO_DC( winfo ), (LPPOINT)pPixels, iNumPixels );
}
#endif



#ifdef __GFX__
/*-------------------------------------------------------------------------*/
STATIC void grl_clip_line ( x0, y0, x1, y1 ) 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
    GR_PIX_COORD x, y ;
    int u0, v0, u1, v1 ;
    u0 = ( x0 >= GRL_X1 ) - ( x0 <= GRL_X0 ) ;
    v0 = ( y0 >= GRL_Y1 ) - ( y0 <= GRL_Y0 ) ;
    u1 = ( x1 >= GRL_X1 ) - ( x1 <= GRL_X0 ) ;
    v1 = ( y1 >= GRL_Y1 ) - ( y1 <= GRL_Y0 ) ;
    if ( u0 == 0 && u1 == 0 && v0 == 0 && v1 == 0 ) 
	LINE ( x0, y0, x1, y1, GRL_COLOR, DRAW_LINE, GRL_PATT ) ;
    else if ( u0 == u1 && ( u0 == 1 || u0 == -1 ) ) ;
    else if ( v0 == v1 && ( v0 == 1 || v0 == -1 ) ) ;
    else {
	if ( x1 - x0 <= 1 && x0 - x1 <= 1 && y1 - y0 <= 1 && y0 - y1 <= 1 ) 
	    RETURN ;
	x = 0.5 * ( x0 + x1 ) ;
	y = 0.5 * ( y0 + y1 ) ;
	grl_clip_line ( x0, y0, x, y ) ;
	grl_clip_line ( x, y, x1, y1 ) ;
    }
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE BOOLEAN grl_set_videomode ( mode )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
INT mode ;
{
#ifdef __TURBOC__
    int graphdriver, graphmode, result ;
    STRING bgi_directory=getenv("TC_BGI") ;
#endif
#ifdef __GFX__
    BOOLEAN status ;
#endif
    if ( mode == (INT)GR_DETECT ) 
	  RETURN ( grl_set_videomode ( GR_VGA16 ) || 
	    grl_set_videomode ( GR_EGA ) || grl_set_videomode ( GR_VGA2 ) || 
	    grl_set_videomode ( GR_EGA0 ) || grl_set_videomode ( GR_HRES16 ) || 
	    grl_set_videomode ( GR_HERC ) || grl_set_videomode ( GR_HRES0 ) ) ;
#ifdef __MCRGRAPH__
    else if ( mode == GR_VGA16 ) 
	RETURN ( _setvideomode ( _VRES16COLOR ) ) ;
    else if ( mode == GR_EGA ) 
	RETURN ( _setvideomode ( _ERESCOLOR ) ) ;
    else if ( mode == GR_VGA2 ) 
	RETURN ( _setvideomode ( _VRES2COLOR ) ) ;
    else if ( mode == GR_EGA0 ) 
	RETURN ( _setvideomode ( _ERESNOCOLOR ) ) ;
    else if ( mode == GR_HRES16 ) 
	RETURN ( _setvideomode ( _HRES16COLOR ) ) ;
    else if ( mode == GR_HERC ) 
	RETURN ( _setvideomode ( _HERCMONO ) ) ;
    else if ( mode == GR_HRES0 ) 
	RETURN ( _setvideomode ( _HRESBW ) ) ;
    else
	RETURN ( FALSE ) ;
#endif

#ifdef __TURBOC__
    else if ( mode == GR_VGA16 ) {
	graphdriver = VGA ;
	graphmode = VGAHI ;
    }
    else if ( mode == GR_EGA ) {
	graphdriver = EGA ;
	graphmode = EGAHI ;
    }
    else if ( mode == GR_VGA2 ) {
	RETURN ( FALSE ) ;
    }
    else if ( mode == GR_EGA0 ) {
	graphdriver = EGAMONO ;
	graphmode = EGAMONOHI ;
    }
    else if ( mode == GR_HRES16 ) {
	graphdriver = EGA ;
	graphmode = EGALO ;
    }
    else if ( mode == GR_HERC ) {
	graphdriver = HERCMONO ;
	graphmode = HERCMONOHI ;
    }
    else if ( mode == GR_HRES0 ) {
	graphdriver = CGA ;
	graphmode = CGAHI ;
    }
    else
	RETURN ( FALSE ) ;

    initgraph ( &graphdriver, &graphmode, bgi_directory ) ;
    result = graphresult() ;
    RETURN ( result >= 0 ) ;
#endif
#ifdef __GFX__
    else if ( mode == GR_VGA16 ) {
	_gfx.card_monitor = VGA_DISPLAY || VGA_CARD ;
	status = SCREEN ( FORCE_VGA_COLOR ) ;
    }
    else if ( mode == GR_EGA ) 
	status = SCREEN ( FORCE_EGA_COLOR ) ;
    else if ( mode == GR_VGA2 ) 
	status = SCREEN ( FORCE_VGA_MONO ) ;
    else if ( mode == GR_EGA0 ) 
	status = SCREEN ( FORCE_EGA_MONO ) ;
    else if ( mode == GR_HRES16 ) 
	status = SCREEN ( FORCE_EGA_HI_RES ) ;
    else if ( mode == GR_HERC ) {
	_gfx.card_monitor = MONO_DISPLAY || HERC_CARD ;
	status = SCREEN ( 2 ) ;   /* Auto select mode */
    }
    else if ( mode == GR_HRES0 ) 
	status = SCREEN ( HI_RES_BW ) ;
    else
	status = FALSE ;
    if ( status ) {
	GRL_X0 = 0 ;
	GRL_Y0 = 0 ;
	GRL_X1 = grl_get_max_x() ;
	GRL_Y1 = grl_get_max_y() ;
	GRL_VIEW_HANDLE = OPEN_VIEW ( 0, 0, GRL_X1, GRL_Y1, 0, 0 ) ;
    }
    RETURN ( status ) ;
#endif
#ifdef __WINDOWS__
	RETURN ( TRUE ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_exit_videomode ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software       !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    _setvideomode (_DEFAULTMODE ) ;
#endif
#ifdef __TURBOC__
    closegraph () ;
#endif
#ifdef __GFX__

    CLOSE_VIEW ( GRL_VIEW_HANDLE, 1 ) ; 
    SCREEN(0) ;
#endif
#ifdef __WINDOWS__
/* not neccesary */
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE INT grl_videomode ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    if ( v_info.mode == _VRES16COLOR )
	RETURN ( GR_VGA16 ) ;
    else if ( v_info.mode == _ERESCOLOR )
	RETURN ( GR_EGA ) ;
    else if ( v_info.mode == _VRES2COLOR )
	RETURN ( GR_VGA2 ) ;
    else if ( v_info.mode == _ERESNOCOLOR )
	RETURN ( GR_EGA0 ) ;
    else if ( v_info.mode == _HRES16COLOR )
	RETURN ( GR_HRES16 ) ;
    else if ( v_info.mode == _HERCMONO )
	RETURN ( GR_HERC ) ;
    else if ( v_info.mode == _HRESBW )
	RETURN ( GR_HRES0 ) ;
    else
	RETURN ( GR_NOVMODE ) ;
#endif
#ifdef __TURBOC__
    int gmode = getgraphmode ();

    if ( gmode == VGAHI )
	RETURN ( GR_VGA16 ) ;
    else if ( gmode == EGAHI )
	RETURN ( GR_EGA ) ;
    else if ( gmode == EGAMONOHI )
	RETURN ( GR_EGA0 ) ;
    else if ( gmode == EGALO )
	RETURN ( GR_HRES16 ) ;
    else if ( gmode == HERCMONOHI )
	RETURN ( GR_HERC ) ;
    else if ( gmode == CGAHI )
	RETURN ( GR_HRES0 ) ;
    else
	RETURN ( GR_NOVMODE ) ;

#endif
#ifdef __GFX__
    if ( _gfx.card_monitor & ( VGA_DISPLAY | VGA_CARD ) )
	RETURN ( GR_VGA16 ) ;
    else if ( _gfx.card_monitor & ( EGA_DISPLAY | EGA_CARD ) )
	RETURN ( GR_EGA ) ;
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | VGA_CARD ) )
	RETURN ( GR_VGA2 ) ;
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | EGA_CARD ) ) 
	RETURN ( GR_EGA0 ) ;
    else if ( _gfx.card_monitor & ( COLOR_DISPLAY | EGA_CARD ) ) 
	RETURN ( GR_HRES16 ) ;
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | HERC_CARD ) )
	RETURN ( GR_HERC ) ; 
    else if ( _gfx.card_monitor & ( MONO_DISPLAY | CGA_CARD ) ) 
	RETURN ( GR_HRES0 ) ;
    else
	RETURN ( FALSE ) ;
#endif
#ifdef __WINDOWS__
	HDC dchandle ;
	int ncolors ;
	dchandle = grl_get_hdc() ;
	ncolors = GetDeviceCaps ( dchandle, SIZEPALETTE ) ;
	RETURN ( ncolors );
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_color ( color )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_COLOR color ;
{
#ifdef __MCRGRAPH__
    _setcolor ( color ) ;
#endif
#ifdef __TURBOC__
    setcolor ( color ) ;
    setfillstyle ( SOLID_FILL, color ) ;
#endif
#ifdef __GFX__
    GRL_COLOR = color ;
#endif
#ifdef __WINDOWS__
	/* Create a new pen, change its colors, and delete the old one*/
	LOGPEN lp ;
	HPEN hPen, oldPen ;
	LOGBRUSH lb ;
	HBRUSH hBrush, oldBrush ;
		
	//if ( WINFO_TOOL( winfo ) == PEN ) // ### if pen != 0
	{
		WINFO_COLOR( winfo ) = color ;
		oldPen = WINFO_PEN( winfo ) ;
		/* lp (logical pen) gets attributes of current pen */
		GetObject ( oldPen, sizeof (LOGPEN), (LPSTR) &lp ) ;
		/* set the new color */
		lp.lopnColor = colors[ color ] ;
		/* create the new pen */
		hPen = CreatePenIndirect ( &lp ) ;
		/* if the current tool is the pen then delete the old pen */
		SelectObject ( WINFO_DC( winfo ), hPen ) ;
		/* update winfo */
		WINFO_PEN( winfo ) = hPen ;
		WINFO_COLOR( winfo ) = color ;
		DeleteObject ( oldPen ) ;
 //	}
 //	else if ( WINFO_TOOL( winfo ) == BRUSH ) // ### if brush != 0
 //	{
		WINFO_COLOR( winfo ) = color ;          
		oldBrush = WINFO_BRUSH( winfo ) ;
		GetObject ( oldBrush, sizeof (LOGBRUSH), (LPSTR) &lb ) ;
		lb.lbColor = colors[ color ] ;
		hBrush = CreateBrushIndirect ( &lb ) ;
		SelectObject ( WINFO_DC( winfo ), hBrush ) ;
		WINFO_BRUSH( winfo ) = hBrush ;
		DeleteObject ( oldBrush ) ;
	}
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_textposition ( x, y )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
{
#ifdef __MCRGRAPH__
	_settextposition ( ( short ) ( y + 1) , ( short ) ( x + 1 ) ) ;
#endif
#ifdef __TURBOC__
    union REGS regs ;
    regs.h.dh = y ;
    regs.h.dl = x ;
    regs.h.ah = 2 ;
    regs.h.bh = 0 ;
    int86 ( 0x10, &regs, &regs ) ;
#endif
#ifdef __GFX__
#ifdef      __HIGHC24__
    union REGS regs ;
    regs.h.dh = y ;
    regs.h.dl = x ;
    regs.h.ah = 2 ;
    regs.h.bh = 0 ;
    int86 ( 0x10, &regs, &regs ) ;
/*
    _gfx_locate ( y+1, x+1 ) ;  - has a bug 
*/
#else
    LOCATE ( y+1, x+1 ) ;
#endif
#endif
#ifdef __WINDOWS__
	WINFO_XTEXTPOS( winfo ) = x ;
	WINFO_YTEXTPOS( winfo ) = y ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_COLOR grl_get_color ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    RETURN ( _getcolor ( ) ) ;
#endif
#ifdef __TURBOC__
    RETURN ( getcolor ( ) ) ;
#endif
#ifdef __GFX__
    RETURN ( GRL_COLOR ) ;
#endif
#ifdef __WINDOWS__
	RETURN ( WINFO_COLOR( winfo ) ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_patt ( patt )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PATT patt ;
{
#ifdef __MCRGRAPH__
    _setlinestyle ( patt ) ;
#endif
#ifdef __TURBOC__
    if ( patt == 0 )
        patt = GR_SOLID_PATT;

    setlinestyle ( USERBIT_LINE, patt, 1 ) ;
#endif
#ifdef __GFX__
    GRL_PATT = patt ;
#endif
#ifdef __WINDOWS__
	/* Create a new pen, change its style, and delete the old one*/
	LOGPEN lp ;
	HPEN hPen, oldPen ;
	
	oldPen = WINFO_PEN( winfo ) ;
	/* lp (logical pen) gets attributes of current pen */
	GetObject ( oldPen, sizeof (LOGPEN), (LPSTR) &lp ) ;
	/* set the new pattern */
	if ( patt == GR_SOLID_PATT )
	{
		lp.lopnStyle = PS_SOLID ;
	}
	else if ( patt == GR_DASHED_PATT )
	{
		lp.lopnStyle = PS_DASH ;
	}
	/* create the new pen */
	hPen = CreatePenIndirect ( &lp ) ;
	/* if the current tool is the pen then delete the old pen */
	// if ( WINFO_TOOL( winfo ) == PEN ) 
	{
		SelectObject ( WINFO_DC( winfo ), hPen ) ;
	}
	/* update winfo */
	WINFO_PEN( winfo ) = hPen ;
	WINFO_PATT( winfo ) = patt ;
		
	DeleteObject ( oldPen ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_solid_patt ( )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    _setlinestyle ( GR_SOLID_PATT ) ;
#endif
#ifdef __TURBOC__
    setlinestyle ( SOLID_LINE, 0, 1 ) ;
#endif
#ifdef __GFX__
    GRL_PATT = GR_SOLID_PATT ;
#endif
#ifdef __WINDOWS__
	LOGPEN lp ;
	HPEN hPen, oldPen ;
	
	oldPen = WINFO_PEN( winfo ) ;
	/* lp (logical pen) gets attributes of current pen */
	GetObject ( oldPen, sizeof (LOGPEN), (LPSTR) &lp ) ;
	/* set the new style to solid */
	lp.lopnStyle = PS_SOLID ;
	/* create the new pen */
	hPen = CreatePenIndirect ( &lp ) ;
	/* if the current tool is the pen then delete the old pen */
	if ( WINFO_TOOL( winfo ) == PEN ) 
	{
		SelectObject ( WINFO_DC( winfo ), hPen ) ;
	}
	/* update winfo */
	WINFO_PEN( winfo ) = hPen ;
	WINFO_PATT( winfo ) = GR_SOLID_PATT ;
		
	DeleteObject ( oldPen ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PATT grl_get_patt ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    RETURN ( _getlinestyle () ) ;
#endif
#ifdef __TURBOC__
    struct linesettingstype ls_info ;
    getlinesettings(&ls_info) ;
    if ( ls_info.upattern == 0 )
        RETURN ( GR_SOLID_PATT );
    else
        RETURN ( ls_info.upattern ) ;
#endif
#ifdef __GFX__
    RETURN ( GRL_PATT ) ;
#endif
#ifdef __WINDOWS__
	return WINFO_PATT( winfo ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_clear_screen ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    _clearscreen ( _GCLEARSCREEN ) ;
#endif
#ifdef __TURBOC__
    cleardevice ( ) ;
#endif
#ifdef __GFX__
    CLS();
#endif
#ifdef __WINDOWS__
	/* clear screen is taken care of at the grr level. */
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_max_x ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software        !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numxpixels ) ;
#endif
#ifdef __TURBOC__
    RETURN ( getmaxx() ) ;
#endif
#ifdef __GFX__
    RETURN ( _gfx.max_x ) ;
#endif
#ifdef __WINDOWS__
	RECT rect ;
	GetClientRect (WINFO_WINDOW( winfo ), &rect ) ;
	return ( ( GR_PIX_COORD ) rect.right ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_max_y ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software            !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numypixels ) ;
#endif
#ifdef __TURBOC__
    RETURN ( getmaxy() ) ;
#endif
#ifdef __GFX__
    RETURN ( _gfx.max_y ) ;
#endif
#ifdef __WINDOWS__
	RECT rect ;
	GetClientRect (WINFO_WINDOW( winfo ), &rect ) ;
	return ( ( GR_PIX_COORD ) rect.bottom ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_min_x ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software            !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    RETURN ( 0 ) ;
#endif
#ifdef __TURBOC__
    RETURN ( 0 ) ;
#endif
#ifdef __GFX__
    RETURN ( 0 ) ;
#endif
#ifdef __WINDOWS__
	return 0;
#endif
}



/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_min_y ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software            !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    RETURN ( 0 ) ;
#endif
#ifdef __TURBOC__
    RETURN ( 0 ) ;
#endif
#ifdef __GFX__
    RETURN ( 0 ) ;
#endif
#ifdef __WINDOWS__
	return 0 ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_numtextcols ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989 - 1995 Building Block Software            !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numtextcols ) ;
#else 
#ifdef __WINDOWS__
		/* Works only for SYSTEM_FIXED_FONT */
		TEXTMETRIC tm ;
		HDC newhdc ;
		int cols, charwidth ;

		SaveDC ( WINFO_DC( winfo ) ) ;
		newhdc = GetDC ( WINFO_WINDOW( winfo ) ) ;
		SelectObject ( newhdc, GetStockObject ( SYSTEM_FIXED_FONT ) ) ;
		GetTextMetrics ( newhdc, &tm ) ;
		charwidth = tm.tmAveCharWidth ;
		RestoreDC ( WINFO_DC( winfo ), -1 ) ;
		cols = grl_get_max_x () / charwidth;
		return cols;
#else
    RETURN ( 80 ) ; /* temporarily */
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE GR_PIX_COORD grl_get_numtextrows ()
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __MCRGRAPH__
    struct videoconfig v_info ;
    _getvideoconfig(&v_info) ;
    RETURN ( v_info.numtextrows ) ;
#else
#ifdef __WINDOWS__
		/* Works only for SYSTEM_FIXED_FONT */
		TEXTMETRIC tm ;
		HDC newhdc ;
		int rows, charheight ;

		SaveDC ( WINFO_DC( winfo ) ) ;
		newhdc = GetDC ( WINFO_WINDOW( winfo ) ) ;
		SelectObject ( newhdc, GetStockObject ( SYSTEM_FIXED_FONT ) ) ;
		GetTextMetrics ( newhdc, &tm ) ;
		charheight = tm.tmHeight + tm.tmExternalLeading ;
		RestoreDC ( WINFO_DC( winfo ), -1 ) ;
		rows = grl_get_max_y () / charheight ;
		return rows;
#else
	RETURN ( 30 ) ; /* temporarily */
#endif
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_rectangle ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef __MCRGRAPH__
    _rectangle ( _GBORDER, x0, y0, x1, y1 ) ;
#endif
#ifdef __TURBOC__
    rectangle ( x0, y0, x1, y1 ) ;
#endif
#ifdef __GFX__
    GRL_X = x0 ;
    GRL_Y = y0 ;
    grl_box_corners ( &x0, &y0, &x1, &y1 ) ;
    LINE ( x0, y0, x1, y1, GRL_COLOR, EMPTY_BOX, GRL_PATT ) ;
#endif
#ifdef __WINDOWS__
#ifdef NOT_USED
	// Draws a hollow rectangle.
	HBRUSH wBrush ;
	HDC tempDC ;
	wBrush = GetStockObject ( WHITE_BRUSH ) ;
	SaveDC ( WINFO_DC ( winfo ) ) ;
	SelectObject ( WINFO_DC( winfo ), tempDC ) ;
	Rectangle ( WINFO_DC( winfo ), x0, y0, x1, y1 ) ;
	RestoreDC ( WINFO_DC ( winfo ), -1 ) ;
#endif // NOT_USED
	grl_moveto ( x0, y0 );
	grl_lineto ( x0, y1 );
	grl_lineto ( x1, y1 );
	grl_lineto ( x1, y0 );
   grl_lineto ( x0, y0 );
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_fill_rectangle ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef __MCRGRAPH__
    _rectangle ( _GFILLINTERIOR, x0, y0, x1, y1 ) ;
#endif
#ifdef __TURBOC__
    bar ( x0, y0, x1, y1 ) ;
#endif
#ifdef __GFX__
    GRL_X = x0 ;
    GRL_Y = y0 ;
    grl_box_corners ( &x0, &y0, &x1, &y1 ) ;
    LINE ( x0, y0, x1, y1, GRL_COLOR, FILL_BOX, GRL_PATT ) ;
#endif
#ifdef __WINDOWS__
	RECT rect ;
	SetRect ( &rect, x0, y0, x1, y1 ) ;
	Rectangle ( WINFO_DC( winfo ), x0, y0, x1, y1 ) ;
// FillRect ( WINFO_DC( winfo ), &rect, WINFO_BRUSH( winfo ) ) ;
#endif
}


#ifdef __GFX__
/*-------------------------------------------------------------------------*/
STATIC void grl_box_corners ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD *x0, *y0, *x1, *y1 ;
{
    *x0 += GRL_X0 ;
    *x1 += GRL_X0 ;
    *y0 += GRL_Y0 ;
    *y1 += GRL_Y0 ;
    if ( *x0 < GRL_X0 ) 
	*x0 = GRL_X0 ;
    else if ( *x0 > GRL_X1 ) 
	*x0 = GRL_X1 ;
    if ( *x1 < GRL_X0 ) 
	*x1 = GRL_X0 ;
    else if ( *x1 > GRL_X1 ) 
	*x1 = GRL_X1 ;
    if ( *y0 < GRL_Y0 ) 
	*y0 = GRL_Y0 ;
    else if ( *y0 > GRL_Y1 ) 
	*y0 = GRL_Y1 ;
    if ( *y1 < GRL_Y0 ) 
	*y1 = GRL_Y0 ;
    else if ( *y1 > GRL_Y1 ) 
	*y1 = GRL_Y1 ;
}
#endif


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_outtext ( buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
STRING buffer ;
{
#ifdef __MCRGRAPH__
    _outtext ( buffer ) ;
#endif
#ifdef __TURBOC__
    outtext ( buffer ) ;
#endif
#ifdef __GFX__
    printf ( "%s", buffer ) ;
#endif
#ifdef __WINDOWS__
	/*      Refers to XTextPos and YTextPos in winfo        */ 
	/*  Prints using TextOut                                                */
	int len ;
	len = lstrlen( buffer ) ;
	TextOut ( WINFO_DC( winfo ), WINFO_XTEXTPOS( winfo ), WINFO_YTEXTPOS( winfo ), 
			  buffer, len ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_outtext_xy ( x, y, buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
STRING buffer ;
{
#ifdef __MCRGRAPH__
    _settextposition ( ( short ) ( y + 1) , ( short ) ( x + 1 ) ) ;
    _outtext ( buffer ) ;
#endif
#ifdef __TURBOC__
    struct viewporttype cur_view ;
    getviewsettings ( &cur_view ) ;
    setviewport ( 0, 0, getmaxx(), getmaxy(), 0 ) ;
    outtextxy ( y+1, x+1, buffer ) ;
    setviewport ( cur_view.left, cur_view.top, cur_view.right, 
	cur_view.bottom, 0 ) ;
#endif
#ifdef __GFX__
#ifdef      __HIGHC24__
    _gfx_locate ( y+1, x+1 ) ;
#else
    LOCATE ( y+1, x+1 ) ;
#endif
    printf ( "%s", buffer ) ;
#endif
#ifdef __WINDOWS__
	/*  Prints using TextOut                                                */
	int len ;
	len = lstrlen( buffer ) ;
	TextOut ( WINFO_DC( winfo ), x, y, buffer, len ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_clip ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef __MCRGRAPH__
    _setviewport ( x0, y0, x1, y1 ) ;
#endif
#ifdef __TURBOC__
    setviewport ( x0, y0, x1, y1, 1 ) ;
#endif
#ifdef __GFX__
    GRL_X0 = x0 ;
    GRL_X1 = x1 ;
    GRL_Y0 = y0 ;
    GRL_Y1 = y1 ;
#endif
#ifdef __WINDOWS__
	RECT rect ;

	SetRect ( &rect, x0, y0, x1, y1 ) ;
	WINFO_CLIPRGN(winfo) = CreateRectRgnIndirect ( &rect );
	SelectClipRgn ( WINFO_DC(winfo), WINFO_CLIPRGN(winfo) );
#ifdef __WIN32__
	SetViewportOrgEx ( WINFO_DC(winfo), x0, y0, NULL );
#else
	SetViewportOrg ( WINFO_DC(winfo), x0, y0 );
#endif // __WIN32__
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_unclip __(( void ))
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    GR_PIX_COORD x_max, y_max ;
    x_max = grl_get_max_x();
    y_max = grl_get_max_y();
#ifdef __MCRGRAPH__
    _setviewport ( 0, 0, x_max, y_max ) ;
#endif
#ifdef __TURBOC__
    setviewport ( 0, 0, x_max, y_max, 0 ) ;
#endif
#ifdef __GFX__
    GRL_X0 = 0 ;
    GRL_X1 = x_max ;
    GRL_Y0 = 0 ;
    GRL_Y1 = y_max ;
#endif
#ifdef __WINDOWS__
	ValidateRect ( WINFO_WINDOW ( winfo ), NULL );
#ifdef __WIN32__
	SetViewportOrgEx ( WINFO_DC(winfo), 0, 0, NULL );
#else
	SetViewportOrg ( WINFO_DC(winfo), 0, 0 );
#endif
	DeleteObject ( WINFO_CLIPRGN(winfo) );
	WINFO_CLIPRGN(winfo) = NULL;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_floodfill ( x, y, color )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
GR_COLOR color ;
{
#ifdef __MCRGRAPH__
    (void) _floodfill ( x, y, color ) ;
#endif
#ifdef __TURBOC__
    (void) floodfill ( x, y, color ) ;
#endif
#ifdef __GFX__
    PAINT ( x, y, GRL_COLOR, color ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_putimage ( x, y, buffer )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x, y ;
ANY buffer ;
{
#ifndef __MSC32__

#ifdef __MCRGRAPH__
    (void) _putimage ( x, y, buffer, _GPSET ) ;
#endif
#ifdef __TURBOC__
    (void) putimage ( x, y, buffer, COPY_PUT ) ;
#endif
#ifdef __GFX__
    PUT_PIC ( x, y, buffer, 'P' ) ;
#endif

#endif /* __MSC32__ */
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE ANY grl_getimage ( x0, y0, x1, y1 )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
GR_PIX_COORD x0, y0, x1, y1 ;
{
#ifdef __MCRGRAPH__
    INT size = (INT)_imagesize ( x0, y0, x1, y1 ) ;
    ANY buffer = MALLOC ( size, char ) ;
    if ( buffer != NULL ) 
	(void) _getimage ( x0, y0, x1, y1, buffer ) ;
    RETURN ( buffer ) ;
#endif
#ifdef __TURBOC__
    INT size = (INT)imagesize ( x0, y0, x1, y1 ) ;
    ANY buffer = MALLOC ( size, char ) ;
    if ( buffer != NULL ) 
	(void) getimage ( x0, y0, x1, y1, buffer ) ;
    RETURN ( buffer ) ;
#endif
#ifdef __GFX__
#ifdef __HIGHC24__
    RETURN ( _gfx_get_pic ( x0, y0, x1, y1 ) ) ;
#else
    RETURN ( GET_PIC ( x0, y0, x1, y1 ) ) ;
#endif
#endif
#ifdef __WINDOWS__
	return ( ( ANY ) NULL ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE REAL grl_get_screen_ratio __(( void ))
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef __UNIX__
    RETURN ( GR_SCREEN_RATIO ) ;
#else
    RETURN ( GR_SCREEN_RATIO ) ;
#endif
}


/*-------------------------------------------------------------------------*/
BBS_PRIVATE void grl_set_screen_ratio ( screen_ratio )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!!      (C) Copyright 1989 - 1995 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
REAL screen_ratio ;
{
    GR_SCREEN_RATIO = screen_ratio ;
}

