/*******************************  QGLPRIV.H ********************************/
#ifndef     QGLPRIV
#define     QGLPRIV
#define     BBSPRIV
#include <qgldefs.h>
extern  ANY     qgl_malloc __(( INT DUMMY0 )) ;
extern  void    qgl_free __(( ANY DUMMY0 )) ;
extern  ANY     qgl_create __(( INT DUMMY0 )) ;
extern  void    qgl_kill __(( ANY DUMMY0 )) ;
extern  void    qgl_print_real __(( FILE* DUMMY0 , REAL DUMMY1 )) ; 
extern  void    qgl_print_int __(( FILE* DUMMY0 , INT DUMMY1 )) ; 
extern  void    qgl_print_boolean __(( FILE* DUMMY0 , BOOLEAN DUMMY1 )) ; 
extern  void    qgl_print_string __(( FILE* DUMMY0 , STRING DUMMY1 )) ; 
extern  void    qgl_print_tab __(( FILE* DUMMY0 )) ; 
extern  void    qgl_print_tabs __(( FILE* DUMMY0 , INT DUMMY1 )) ; 
extern  void    qgl_print_new_line __(( FILE* DUMMY0 )) ; 
extern  REAL    qgl_get_real __(( void )) ; 
extern  INT     qgl_get_int __(( void )) ; 
extern  void    qgl_print_malloc __(( void )) ;
extern  void    qgl_print_create __(( void )) ;

#endif /* QGLPRIV*/

