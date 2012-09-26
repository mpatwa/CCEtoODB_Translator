/*
$Workfile:   dbisland.c  $

$Header:   P:/pvcs/sscope/src/database/dbisland.c_v   3.0   07 Apr 1992 14:53:56   cramer  $

$Modtime:   07 Apr 1992 15:57:40  $

$Log:   P:/pvcs/sscope/src/database/dbisland.c_v  $
 * 
 *    Rev 3.0   07 Apr 1992 14:53:56   cramer
 * Initial revision.
*/

/*******************************************************************************
*                                                                              *
* This file, when lib'ed to the database.lib, makes the database an independant*
* library which can be linked to a program to create a SmartScope routine.     *
* Perform the following steps:                                                 *
*                                                                              *
*   add this file to a writable copy of database.mak                           *
*   make_dep dbisland.c (this creates the .dep file)                           *
*   make -f *.mak all (this will compile and lib this file in)                 *
*                                                                              *
* At the end of this file you will find a sample C program which will build a  *
* routine of 10 point features.  This should be sent to anyone who will        *
* receive this isolated library as an example.                                 *
*                                                                              *
* Also at the end of this file, the bat file to compile the example program    *
* and link it with the database.lib (Microsoft C link).                        *
*                                                                              *
* The distribution disk should contain the following files:                    *
*                                                                              *
* DATABASE.LIB - the SmartScope database library                               *
* DB_CONST.H   - database constants include file                               *
* DB_DECL.H    - database type declaration include file                        *
* DB_PROTO.H   - database prototype include file                               *
* MAIN.C       - a sample C program using the SmartScope database              *
* TEST.BAT     - a simple batch file which will compile and link the test      *
* README.1ST   - this list of files                                            *
*                                                                              *
*******************************************************************************/


/* Compiler Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void errHandler(
		char *file_name,
		int line_number,
		char *func_name,
		int error_table,
		int error_number,
		char *extra_str,
		int extra_value)
{
	FILE *stream = fopen("OGP.log", "a+");
	fprintf(stream, "file=%s, line=%d, func=%s, err_tab=%d, err_num=%d, ext_str=%s, ext_val=%d\n", 
			file_name, line_number, func_name, error_table, error_number, extra_str, extra_value);
	fclose(stream);
}

int getEnvVar(char *tmp_env, char *env_variable, char *env_dflt)
{
	unsigned int last_char = 0;
	char *env_var_ptr;

    /* Grab the ENVIRONMENT variable */
    if ( (env_var_ptr = getenv(env_variable)) == NULL )
    {
        /* Use the default ... Make sure caller has default correct */
        if ( env_dflt != NULL )
        {
           strcpy(tmp_env,env_dflt);
        }
        else
        {
           /* Something really wrong! ... */
           /* Environment variable wasn't set! ... Not even the default */
           return(-1);
        }
    }
    else
    {
        /* Save this copy */
        strcpy(tmp_env,env_var_ptr);
    }

    /* Process environment variable ... Pass back with "\" on the end */
    last_char = strlen(tmp_env);
    if (last_char > 0)
    {
       if (tmp_env[last_char-1] != '\\')
       {
          tmp_env[last_char] = '\\';
	      tmp_env[last_char + 1] = '\0';
       }
    }

	return(0);
}
