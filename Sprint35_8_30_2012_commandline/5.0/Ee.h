// $Header: /CAMCAD/4.3/Ee.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-99. All Rights Reserved.
*/

#pragma once

/* Codes returned from the evaluator */
#define E_OK           0        /* Successful evaluation */
#define E_SYNTAX       1        /* Syntax error */
#define E_UNBALAN      2        /* Unbalanced parenthesis */
#define E_DIVZERO      3        /* Attempted division by zero */
#define E_UNKNOWN      4        /* Reference to unknown variable */
#define E_MAXVARS      5        /* Maximum variables exceeded */
#define E_BADFUNC      6        /* Unrecognised function */
#define E_NUMARGS      7        /* Wrong number of arguments to funtion */
#define E_NOARG        8        /* Missing an argument to a funtion */
#define E_EMPTY        9        /* Empty expression */

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

/*
   if( (ec = EE_Evaluate( line, &result, &a )) == E_OK )
   {
      // If we didn't assign a variable, then print the result. 
      if( ! a )
         printf( "%g\n", result );
   }
   else if( ec != E_EMPTY )
   {
      // Display error info.  In this example, an E_EMPTY error is ignored. 
      printf( "ERROR: %s - %s", ErrMsgs[ERROR - 1], ERTOK );
      printf( "\n%s", ERANC );
      printf( "\n%*s^\n", ERPOS, "" );
   }
*/

int EE_Evaluate( char* e, double* result, int* a );

/* end ee.h */