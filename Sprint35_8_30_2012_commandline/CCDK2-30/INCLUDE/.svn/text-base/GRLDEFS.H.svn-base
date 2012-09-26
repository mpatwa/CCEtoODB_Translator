/******************************* GRLDEFS.H *********************************/
/****************** Lowest level Graphics prototypes ***********************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!   (C) Copyright 1989 - 1995             Building Block Software!!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef GRLDEFS
#define GRLDEFS

#include <bbscmpl.h>

#ifdef __WINDOWS__
#include <windows.h>
#endif 

#include <grdefs.h>

extern  void grl_moveto __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 )) ;
extern  void grl_lineto __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 )) ;
extern  void grl_line __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 , 
	    GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  void grl_rectangle __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
	    GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  void grl_fill_rectangle __(( GR_PIX_COORD DUMMY0 , 
	    GR_PIX_COORD DUMMY1 , GR_PIX_COORD DUMMY2 , 
	    GR_PIX_COORD DUMMY3 )) ;
extern  void grl_outtext __(( STRING DUMMY0 )) ;
extern  void grl_outtext_xy __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
	    STRING DUMMY2 )) ;
extern  BOOLEAN grl_set_videomode __(( INT DUMMY0 )) ;
extern  INT     grl_videomode __(( void )) ;
extern  void    grl_exit_videomode __(( void )) ;
extern  void    grl_set_color __(( GR_COLOR DUMMY0 )) ;
extern  GR_COLOR   grl_get_color __(( void )) ;
extern  void    grl_set_textposition __(( GR_PIX_COORD DUMMY0 , 
	    GR_PIX_COORD DUMMY1 )) ;
extern  void    grl_clip __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
	    GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  void    grl_unclip __(( void )) ;
extern  void    grl_set_patt __(( GR_PATT DUMMY0 )) ;
extern  void    grl_set_solid_patt __(( void )) ;
extern  GR_PATT    grl_get_patt __(( void )) ;
extern  void    grl_clear_screen __(( void )) ;
extern  GR_PIX_COORD grl_get_max_x __(( void )) ;
extern  GR_PIX_COORD grl_get_max_y __(( void )) ;
extern  GR_PIX_COORD grl_get_min_x __(( void )) ;
extern  GR_PIX_COORD grl_get_min_y __(( void )) ;
extern  GR_PIX_COORD grl_get_numtextcols __(( void )) ;
extern  GR_PIX_COORD grl_get_numtextrows __(( void )) ;
extern  void    grl_floodfill __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
	    GR_COLOR DUMMY2 )) ;
extern  void    grl_putimage __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
	    ANY DUMMY2 )) ;
extern  ANY     grl_getimage __(( GR_PIX_COORD DUMMY0 , GR_PIX_COORD DUMMY1 ,
	     GR_PIX_COORD DUMMY2 , GR_PIX_COORD DUMMY3 )) ;
extern  REAL    grl_get_screen_ratio __(( void )) ;
extern  void    grl_set_screen_ratio __(( REAL DUMMY0 )) ;

//windows stuff
#ifdef __WINDOWS__

#define PEN     1 
#define BRUSH   2 
#define BITMAP  3 
#define FONT    4 
#define REGION  5 
typedef int     GR_TOOL ;                 

#define WINFO_CLIPRECT(x)       ( ( x ) -> cliprect )
#define WINFO_WINDOW(x)         ( ( x ) -> window )
#define WINFO_DC(x)                     ( ( x ) -> curDC )
#define WINFO_MAXX(x)           ( ( x ) -> maxx )
#define WINFO_MAXY(x)           ( ( x ) -> maxy )
#define WINFO_PEN(x)            ( ( x ) -> pen )
#define WINFO_BRUSH(x)          ( ( x ) -> brush )
#define WINFO_COLOR(x)          ( ( x ) -> curColor ) 
#define WINFO_PATT(x)           ( ( x ) -> curLinePatt )
#define WINFO_XSCALE(x)         ( ( x ) -> XScale )
#define WINFO_YSCALE(x)         ( ( x ) -> YScale )
#define WINFO_XTEXTPOS(x)       ( ( x ) -> XTextPos )
#define WINFO_YTEXTPOS(x)       ( ( x ) -> YTextPos )
#define WINFO_TOOL(x)           ( ( x ) -> tool )
#define WINFO_CLIPRGN(x)		  ( ( x ) -> cliprgn )

extern void grl_init ( void );
void grl_exit ( void );
extern void grl_set_hwnd ( HWND hwnd );
void grl_unset_hdc __(( void ));
void grl_set_hdc __(( HWND hwnd, HDC dc ));
HDC grl_get_hdc __(( void ))	;
void grl_set_hpen __(( HPEN pen ));
HPEN grl_get_hpen     __(( void )) ;
void grl_set_hbrush __(( HBRUSH brush));
HBRUSH grl_get_hbrush __(( void));


extern void grl_set_extents __(( void )) ;
extern void grl_set_tool __(( GR_TOOL tool )) ;
//extern void grl_set_curDC __(( HDC dc )) ;
//extern HDC grl_get_curDC __(( void )) ;
//extern void grl_set_curPen __(( HPEN pen )) ;
//extern HPEN grl_get_curPen      __(( void )) ;
//extern void grl_set_object __(( GR_TOOL tool )) ;
//extern void grl_get_object __(( GR_TOOL tool )) ;
//extern void grl_set_brush __(( HBRUSH brush)) ;
//extern HBRUSH grl_get_brush __(( void )) ;
//extern void grl_set_cur_window __(( HWND w )) ;
//extern HWND grl_get_cur_window __(( void )) ;

#endif /* WINDOWS */

#endif /* GRLDEFS */

