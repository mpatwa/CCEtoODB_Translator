/*
   $Header:   E:/SOURCE/LIBERATR/COMMON/LOGFILES/HASHUNIT.C_V   1.1   12 Apr 1993 15:09:22   BILL_BLEWETT  $

   Copyright (c) 1993 Four Pi Systems, Corp.

   Purpose  :  

   History  :
*/
/*--------------------------------------------------------------------------*/

#include <string.h>
#include <ctype.h>

/*--------------------------------------------------------------------------*/
/*
   This routine creates a string representation of the given number in the
   given base.  The base should not be larger than 36 since there are only
   36 letters and digits.
*/

char * BaseX( long   inum,
              int    base,
              char * name )
{
   char  carray[ 80 ];
   char  tstring[ 80 ];
   long  curval;
   int   remainder;
   int   len;
   int   i;
   int   curbindex;

   strcpy( carray, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
   curval      = inum;
   len         = 0;

   do
   {
      remainder      = curval % base;
      curval         = curval / base;
      tstring[len]   = carray[ remainder ];
      len            += 1;

   }
   while ( curval != 0 );

   name[len]   = 0;
   curbindex   = len - 1;

   for ( i = 0; i < len; i++ )
   {
      name[i]     = tstring[ curbindex ];
      curbindex   -= 1;
   }

   return( name );
}

/*--------------------------------------------------------------------------*/

static long MyHash( char * self, long p )
{
   long  z;
   int   len;
   int   i;
   char  ch;

   z     = 0;
   len   = strlen( self );

   for ( i = 0; i < len; i++ )
   {
      ch = toupper( self[i] );
      z  = ( ch + (256 * z) ) % p;
   }

   return z;
}


/*--------------------------------------------------------------------------*/

char * HashName( char * self,
                 char * name )
{
   char  string[80];

   BaseX( MyHash( self, 99991 ), 36, string ); strcpy( name, string );
   BaseX( MyHash( self,   997 ), 36, string ); strcat( name, string );
   BaseX( MyHash( self,   947 ), 36, string ); strcat( name, string );

   return name;
}

