// $Header: /CAMCAD/4.5/Ck.h 13    11/07/06 4:44p Rick Faltersack $

#pragma once

#include "GeneralDialog.h"   // for CFormatStdioFile

void  check_init( const char *FileNm, bool RaiseCase = true, bool IgnoreCollisions = false);
void  check_deinit( void );
char *check_name( char ReplType, const char *INm, BOOL doLog = TRUE );
int   check_report( FILE *fp );
int   check_report( CFormatStdioFile *fp );

// add a setup command.
int   check_add_command(int argc, char **argv);

// should a backslash be converted to a double backslash 
void check_addbackslash(int add_backslash);

// this function allows a normal string to be checked for the backslash 
// backslash rule is set in check_addbackslash
char  *check_fixbackslash(char *Nm);

//
// EOF ck.h
//

