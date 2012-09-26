// $Header: /CAMCAD/4.3/Ee.cpp 8     8/12/03 3:53p Kurt Van Ness $
 
/*************************************************************************
**                                                                       **
** EE.C         Expression Evaluator                                     **
**                                                                       **
** AUTHOR:      Mark Morley                                              **
** COPYRIGHT:   (c) 1992 by Mark Morley                                  **
** DATE:        December 1991                                            **
** HISTORY:     Jan 1992 - Made it squash all command line arguments     **
**                         into one big long string.                     **
**                       - It now can set/get VMS symbols as if they     **
**                         were variables.                               **
**                       - Changed max variable name length from 5 to 15 **
**              Jun 1992 - Updated comments and docs                     **
**                                                                       **
** You are free to incorporate this code into your own works, even if it **
** is a commercial application.  However, you may not charge anyone else **
** for the use of this code!  If you intend to distribute your code,     **
** I'd appreciate it if you left this message intact.  I'd like to       **
** receive credit wherever it is appropriate.  Thanks!                   **
**                                                                       **
** I don't promise that this code does what you think it does...         **
**                                                                       **
** Please mail any bug reports/fixes/enhancments to me at:               **
**      morley@camosun.bc.ca                                             **
** or                                                                    **
**      Mark Morley                                                      **
**      3889 Mildred Street                                              **
**      Victoria, BC  Canada                                             **
**      V8Z 7G1                                                          **
**      (604) 479-7861                                                   **
**                                                                       **
 *************************************************************************/

#include "stdafx.h"
#include <math.h>
#include <float.h>

#include "ee.h"

/*****************************************************************************/
/*
   here ee.h
#include "ee.h"
*/
char* EE_ErrMsgs[] =
{
   "Syntax error",
   "Unbalanced parenthesis",
   "Division by zero",
   "Unknown variable",
   "Maximum variables exceeded",
   "Unrecognised funtion",
   "Wrong number of arguments to funtion",
   "Missing an argument",
   "Empty expression"
};

#define VARLEN          15              /* Max length of variable names */
#define MAXVARS         50              /* Max user-defined variables */
#define TOKLEN          30              /* Max token length */

#define VAR             1
#define DEL             2
#define NUM             3

typedef struct
{
   char  *name;                         /* Function name */
   int   args;                          /* Number of arguments to expect */
   double (*func)(double);              /* Pointer to function */
} EE_FUNCTION;

typedef struct
{
   char name[VARLEN + 1];               /* Variable name */
   double value;                        /* Variable value */
} EE_VARIABLE;


/* The following macros are ASCII dependant, no EBCDIC here! */
#define iswhite(c)  (c == ' ' || c == '\t')
#define isnumer(c)  ((c >= '0' && c <= '9') || c == '.')
#define isalpha(c)  ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') \
                    || c == '_')
#define isdelim(c)  (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' \
                    || c == '^' || c == '(' || c == ')' || c == ',' || c == '=')


/* end of ee.h */

/* These defines only happen if the values are not already defined!  You may
   want to add more precision here, if your machine supports it. */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif
#ifndef M_E
#define M_E     2.71828182845904523536
#endif


/*************************************************************************
**                                                                       **
** PROTOTYPES FOR CUSTOM MATH FUNCTIONS                                  **
**                                                                       **
 *************************************************************************/

static double deg( double x );
static double rad( double x );

/*************************************************************************
**                                                                       **
** VARIABLE DECLARATIONS                                                 **
**                                                                       **
 *************************************************************************/

static int   ERRORNUM;              /* The error number */
static char  ERTOK[TOKLEN + 1];     /* The token that generated the error */
static int   ERPOS;                 /* The offset from the start of the expression */
static char  *ERANC;                /* Used to calculate ERPOS */

//#define ERR(n) {ERRORNUM=n; ERPOS=expression-ERANC-1; strcpy(ERTOK,token); longjmp(jb,1);}

/*
   Add any "constants" here...  These are "read-only" values that are
   provided as a convienence to the user.  Their values can not be
   permanently changed.  The first field is the variable name, the second
   is its value.
*/

static EE_VARIABLE Consts[] =
{
   // name, value 
   { "pi",      M_PI },
   { "e",       M_E },

   { 0 }
};

/*
   Add any math functions that you wish to recognise here...  The first
   field is the name of the function as it would appear in an expression.
   The second field tells how many arguments to expect.  The third is
   a pointer to the actual function to use.
*/

// can only have 1 parameter, because C++ checks the number of arguments.
// in order to make other function, i need to make stub function SIN(arg1, arg2) {return sin(arg1)}

static EE_FUNCTION Funcs[] =
{
   { "sin",     1,    sin },
   { "cos",     1,    cos },
   { "tan",     1,    tan },
   { "asin",    1,    asin },
   { "acos",    1,    acos },
   { "atan",    1,    atan },
   { "sinh",    1,    sinh },
   { "cosh",    1,    cosh },
   { "tanh",    1,    tanh },
   { "exp",     1,    exp },
   { "log",     1,    log },
   { "log10",   1,    log10 },
   { "sqrt",    1,    sqrt },
   { "floor",   1,    floor },
   { "ceil",    1,    ceil },
   { "abs",     1,    fabs },
//   { "hypot",   2,    hypot },

   { "deg",     1,    deg },
   { "rad",     1,    rad },

   { 0 }
};


static EE_VARIABLE     Vars[MAXVARS];       /* Array for user-defined variables */
static char*           expression;          /* Pointer to the user's expression */
static char            token[TOKLEN + 1];   /* Holds the current token */
static int             type;                /* Type of the current token */
//static jmp_buf         jb;                  /* jmp_buf for errors */


static int Level1( double* r );
static int Level2( double* r );
static int Level3( double* r );
static int Level4( double* r );
static int Level5( double* r );
static int Level6( double* r );

/*************************************************************************
**                                                                       **
** Some custom math functions...   Note that they must be prototyped     **
** above (if your compiler requires it)                                  **
**                                                                       **
** deg( x )             Converts x radians to degrees.                   **
** rad( x )             Converts x degrees to radians.                   **
**                                                                       **
 *************************************************************************/

static double deg( double x )
{
   return( x * 180.0 / M_PI );
}

static double rad( double x )
{
   return( x * M_PI / 180.0 );
}


/*************************************************************************
**                                                                       **
** GetSymbol( char* s )                                                  **
**                                                                       **
** This routine obtains a value from the program's environment.          **
** This works for DOS and VMS (and other OS's???)
**                                                                       **
 ************************************************************************/

static int GetSymbol( char* s, double* v )
{
   char* e;

   e = getenv( s );
   if( !e )
      return( 0 );
   *v = atof( e );
   return( 1 );
}

/*************************************************************************
**                                                                       **
** ClearVar( char* name )                                                **
**                                                                       **
** Erases the user-defined variable that is called NAME from memory.     **
** Note that constants are not affected.                                 **
**                                                                       **
** Returns 1 if the variable was found and erased, or 0 if it didn't     **
** exist.                                                                **
**                                                                       **
 *************************************************************************/

static int ClearVar( char* name )
{
   int i;

   for( i = 0; i < MAXVARS; i++ )
      if( *Vars[i].name && ! strcmp( name, Vars[i].name ) )
      {
         *Vars[i].name = 0;
         Vars[i].value = 0;
         return( 1 );
      }
   return( 0 );
}


/*************************************************************************
**                                                                       **
** GetValue( char* name, double* value )                                 **
**                                                                       **
** Looks up the specified variable (or constant) known as NAME and       **
** returns its contents in VALUE.                                        **
**                                                                       **
** First the user-defined variables are searched, then the constants are **
** searched.                                                             **
**                                                                       **
** Returns 1 if the value was found, or 0 if it wasn't.                  **
**                                                                       **
 *************************************************************************/

static int GetValue( char* name, double* value )
{
   int i;

   /* First check for an environment variable reference... */
   if( *name == '_' )
      return( GetSymbol( name + 1, value ) );

   /* Now check the user-defined variables. */
   for( i = 0; i < MAXVARS; i++ )
      if( *Vars[i].name && ! strcmp( name, Vars[i].name ) )
      {
         *value = Vars[i].value;
         return( 1 );
      }

   /* Now check the programmer-defined constants. */
   for( i = 0; *Consts[i].name; i++ )
      if( *Consts[i].name && ! strcmp( name, Consts[i].name ) )
      {
         *value = Consts[i].value;
         return( 1 );
      }
   return( 0 );
}


/*************************************************************************
**                                                                       **
** SetValue( char* name, double* value )                                 **
**                                                                       **
** First, it erases any user-defined variable that is called NAME.  Then **
** it creates a new variable called NAME and gives it the value VALUE.   **
**                                                                       **
** Returns 1 if the value was added, or 0 if there was no more room.     **
**                                                                       **
 *************************************************************************/

static int SetValue( char* name, double* value )
{
   int  i;

   ClearVar( name );
   for( i = 0; i < MAXVARS; i++ )
      if( ! *Vars[i].name )
      {
         strcpy( Vars[i].name, name );
         Vars[i].name[VARLEN] = 0;
         Vars[i].value = *value;
         return( 1 );
      }
   return( 0 );
}


/*************************************************************************
**                                                                       **
** Parse()   Internal use only                                           **
**                                                                       **
** This function is used to grab the next token from the expression that **
** is being evaluated.                                                   **
**                                                                       **
 *************************************************************************/

static int Parse()
{
   char* t;

   type = 0;
   t = token;
   while( iswhite( *expression ) )
      expression++;
   if( isdelim( *expression ) )
   {
      type = DEL;
      *t++ = *expression++;
   }
   else if( isnumer( *expression ) )
   {
      type = NUM;
      while( isnumer( *expression ) )
         *t++ = *expression++;
   }
   else if( isalpha( *expression ) )
   {
      type = VAR;
      while( isalpha( *expression ) )
        *t++ = *expression++;
      token[VARLEN] = 0;
   }
   else if( *expression )
   {
      *t++ = *expression++;
      *t = 0;
      return E_SYNTAX ;
   }
   *t = 0;
   while( iswhite( *expression ) )
      expression++;

   return E_OK;
}


/*************************************************************************
**                                                                       **
** Level1( TYPE* r )   Internal use only                                 **
**                                                                       **
** This function handles any variable assignment operations.             **
** It returns a value of 1 if it is a top-level assignment operation,    **
** otherwise it returns 0                                                **
**                                                                       **
 *************************************************************************/

static int Level1( double* r )
{
   char t[VARLEN + 1];

   if( type == VAR )
      if( *expression == '=' )
      {
         strcpy( t, token );
         Parse();
         Parse();
         if( !*token )
         {
            ClearVar( t );
            return(1);
         }
         Level2( r );
         if( ! SetValue( t, r ) )
            return( E_MAXVARS );
         return( 1 );
      }
   Level2( r );
   return( 0 );
}


/*************************************************************************
**                                                                       **
** Level2( TYPE* r )   Internal use only                                 **
**                                                                       **
** This function handles any addition and subtraction operations.        **
**                                                                       **
 *************************************************************************/

static int Level2( double* r )
{
   double t = 0;
   char o;

   Level3( r );
   while( (o = *token) == '+' || o == '-' )
   {
      Parse();
      Level3( &t );
      if( o == '+' )
         *r = *r + t;
      else if( o == '-' )
         *r = *r - t;
   }
   return E_OK;
}


/*************************************************************************
**                                                                       **
** Level3( TYPE* r )   Internal use only                                 **
**                                                                       **
** This function handles any multiplication, division, or modulo.        **
**                                                                       **
 *************************************************************************/

static int Level3( double* r )
{
   double t;
   char o;

   Level4( r );
   while( (o = *token) == '*' || o == '/' || o == '%' )
   {
      Parse();
      Level4( &t );
      if( o == '*' )
         *r = *r * t;
      else if( o == '/' )
      {
         if( t == 0 )
            return( E_DIVZERO );
         *r = *r / t;
      }
      else if( o == '%' )
      {
         if( t == 0 )
            return( E_DIVZERO );
         *r = fmod( *r, t );
      }
   }
   return E_OK;
}


/*************************************************************************
**                                                                       **
** Level4( TYPE* r )   Internal use only                                 **
**                                                                       **
** This function handles any "to the power of" operations.               **
**                                                                       **
 *************************************************************************/

static int Level4( double* r )
{
   double t;

   Level5( r );
   if( *token == '^' )
   {
      Parse();
      Level5( &t );
      *r = pow( *r, t );
   }
   return E_OK;
}


/*************************************************************************
**                                                                       **
** Level5( TYPE* r )   Internal use only                                 **
**                                                                       **
** This function handles any unary + or - signs.                         **
**                                                                       **
 *************************************************************************/

static int Level5( double* r )
{
   char o = 0;

   if( *token == '+' || *token == '-' )
   {
      o = *token;
      Parse();
   }
   Level6( r );
   if( o == '-' )
      *r = -*r;

   return E_OK;
}


/*************************************************************************
**                                                                       **
** Level6( TYPE* r )   Internal use only                                 **
**                                                                       **
** This function handles any literal numbers, variables, or functions.   **
**                                                                       **
 *************************************************************************/

static int Level6( double* r )
{
   int  i;
   int  n;
   double a[3];

   if( *token == '(' )
   {
      Parse();
      if( *token == ')' )
         return( E_NOARG );
      Level1( r );
      if( *token != ')' )
         return( E_UNBALAN );
      Parse();
   }
   else
   {
      if( type == NUM )
      {
         *r = (double) atof( token );
         Parse();
      }
      else if( type == VAR )
      {
         if( *expression == '(' )
         {
            for( i = 0; *Funcs[i].name; i++ )
               if( ! strcmp( token, Funcs[i].name ) )
               {
                  Parse();
                  n = 0;
                  do
                  {
                     Parse();
                     if( *token == ')' || *token == ',' )
                        return( E_NOARG );
                     a[n] = 0;
                     Level1( &a[n] );
                     n++;
                  } while( n < 4 && *token == ',' );
                  Parse();
                  if( n != Funcs[i].args )
                  {
                     strcpy( token, Funcs[i].name );
                     return( E_NUMARGS );
                  }
                  //*r = Funcs[i].func( a[0], a[1], a[2] );
                  *r = Funcs[i].func( a[0] );
                  return E_OK;
               }
               if( ! *Funcs[i].name )
                  return( E_BADFUNC );
            }
            else if( ! GetValue( token, r ) )
               return( E_UNKNOWN );
         Parse();
      }
      else
         return( E_SYNTAX );
   }
   return E_OK;
}

//////////////////////////////////////////////////////////////////////////
double SIN(double x)
{
   return sin(x);
}
double COS(double x)
{
   return cos(x);
}

/*************************************************************************
**                                                                       **
** Evaluate( char* e, TYPE* result, int* a )                             **
**                                                                       **
** This function is called to evaluate the expression E and return the   **
** answer in RESULT.  If the expression was a top-level assignment, a    **
** value of 1 will be returned in A, otherwise it will contain 0.        **
**                                                                       **
** Returns E_OK if the expression is valid, or an error code.            **
**                                                                       **
 *************************************************************************/

int EE_Evaluate( char* e, double* result, int* a )
{
   //if( setjmp( jb ) )
   //   return( ERROR );
   expression = e;
   ERANC = e;
   STRLWR( expression );
   *result = 0;
   Parse();
   if( ! *token )
      return ( E_EMPTY );
   *a = Level1( result );
   return( E_OK );
}

/*****************************************************************************/
/*
   end ee.cpp
*/
/*****************************************************************************/