/*
$Workfile: Db_comnt.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/Db_comnt.c 4     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/Db_comnt.c $
 * 
 * 4     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 3     6/03/99 3:04p Ttc
 * DN 9723 version 4. Allow null string to be passed to clear a string.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:14   softadmn
 * Initial revision.
 * 
 *    Rev 9.1   18 Apr 1997 16:02:12   dmf
 * Added SmartReport
 * 
 *    Rev 9.0   11 Mar 1996 11:12:38   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:06   softadmn
 * Initial revision.
 * 
 *    Rev 8.1   27 Feb 1995 15:36:50   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:32   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

int dbPutFeatureComment(Feature *this_feature, char *new_comment)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store a feature comment in the feature structure.				  */
/* Returns: ERROR if store fails, NO_ERROR otherwise.						  */
{
	char *loc_func_name = "dbPutFeatureComment";
	unsigned int length;					/* length of the comment to store */

	if (this_feature->data.comment != NULL)			   /* if a comment exists */
	{
		free(this_feature->data.comment);				  /* free the comment */
		this_feature->data.comment = NULL;				 /* clear the pointer */
	}
	if (new_comment != NULL)
	{
		length = strlen(new_comment) + 1;	 /* get the length of the comment */
		if (length > 1)								/* new comment has length */
		{
			/* allocate a character vector to save the comment in			  */
			this_feature->data.comment = (char *)calloc(length,sizeof(char));
			if (this_feature->data.comment == NULL)				 /* no memory */
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(NO_MEMORY);			   /* return no memory error code */
			}

			strncpy(this_feature->data.comment,new_comment,length);
		}
	}
	return(NO_ERROR);								  /* return no error code */
}

char *dbGetFeatureComment(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the feature comment from the feature structure.			  */
/* Returns: A pointer to the character vector (NULL if no comment exists).	  */
{
	return(this_feature->data.comment);	  /* return the pointer to the string */
}

int dbPutFeaturePrompt(Feature *this_feature, char *new_prompt)
/* Author: Terry T. Cramer									 Date: 11/11/1991 */
/* Purpose: Store a feature prompt in the feature structure.				  */
/* Returns: ERROR if store fails, NO_ERROR otherwise.						  */
{
	char *loc_func_name = "dbPutFeaturePrompt";
	unsigned int length;					 /* length of the prompt to store */

	if (this_feature->data.prompt != NULL)				/* if a prompt exists */
	{
		free(this_feature->data.prompt);				   /* free the prompt */
		this_feature->data.prompt = NULL;				 /* clear the pointer */
	}
	if (new_prompt != NULL)
	{
		length = strlen(new_prompt) + 1;	  /* get the length of the prompt */
		if (length > 1)								 /* new prompt has length */
		{
			/* allocate a character vector to save the prompt in			  */
			this_feature->data.prompt = (char *)calloc(length,sizeof(char));
			if (this_feature->data.prompt == NULL)				 /* no memory */
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(NO_MEMORY);			   /* return no memory error code */
			}

			strncpy(this_feature->data.prompt,new_prompt,length);
		}
	}
	return(NO_ERROR);								  /* return no error code */
}

char *dbGetFeaturePrompt(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 11/11/1991 */
/* Purpose: Extract the feature prompt from the feature structure.			  */
/* Returns: A pointer to the character vector (NULL if no prompt exists).	  */
{
	return(this_feature->data.prompt);	  /* return the pointer to the string */
}

int dbPutFeatureExpression(Feature *this_feature, char *new_expression)
/* Author: Terry T. Cramer									 Date: 11/11/1991 */
/* Purpose: Store a feature expression in the feature structure.			  */
/* Returns: ERROR if store fails, NO_ERROR otherwise.						  */
{
	char *loc_func_name = "dbPutFeatureExpression";
	unsigned int length;				 /* length of the expression to store */

	if (this_feature->data.expression != NULL)		/* if a expression exists */
	{
		free(this_feature->data.expression);		   /* free the expression */
		this_feature->data.expression = NULL;			 /* clear the pointer */
	}
	if (new_expression != NULL)
	{
		length = strlen(new_expression) + 1;	  /* length of the expression */
		if (length > 1)							 /* new expression has length */
		{
			/* allocate a character vector to save the expression in		  */
			this_feature->data.expression = (char *)calloc(length,sizeof(char));
			if (this_feature->data.expression == NULL)			 /* no memory */
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(NO_MEMORY);			   /* return no memory error code */
			}

			strncpy(this_feature->data.expression,new_expression,length);
		}
	}
	return(NO_ERROR);								  /* return no error code */
}

char *dbGetFeatureExpression(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 11/11/1991 */
/* Purpose: Extract the feature expression from the feature structure.		  */
/* Returns: A pointer to the character vector (NULL if no expression exists). */
{
	return(this_feature->data.expression);/* return the pointer to the string */
}

int dbPutReportLabel(Feature *this_feature, char *new_label, int index)
{
	char *loc_func_name = "dbPutReportLabel";
	unsigned int length;

	if (this_feature->data.label[index] != NULL)
	{
		free(this_feature->data.label[index]);
		this_feature->data.label[index] = NULL;
	}
	if (new_label != NULL)
	{
		length = strlen(new_label) + 1;
		if (length > 1)
		{
			/* allocate a character vector to save the comment in			  */
			this_feature->data.label[index] = (char *)calloc(length,sizeof(char));
			if (this_feature->data.label[index] == NULL)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(NO_MEMORY);
			}

			strncpy(this_feature->data.label[index],new_label,length);
		}
	}
	return(NO_ERROR);
}

char *dbGetReportLabel(Feature *this_feature, int index)
{
	return(this_feature->data.label[index]);
}

int dbPutExportFieldName(Feature *this_feature, char *new_fieldname, int index)
{
	char *loc_func_name = "dbPutExportFieldName";
	unsigned int length;

	if (this_feature->data.fieldname[index] != NULL)
	{
		free(this_feature->data.fieldname[index]);
		this_feature->data.fieldname[index] = NULL;
	}
	if (new_fieldname != NULL)
	{
		length = strlen(new_fieldname) + 1;
		if (length > 1)
		{
			/* allocate a character vector to save the comment in			  */
			this_feature->data.fieldname[index] = (char *)calloc(length,sizeof(char));
			if (this_feature->data.fieldname[index] == NULL)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(NO_MEMORY);
			}

			strncpy(this_feature->data.fieldname[index],new_fieldname,length);
		}
	}
	return(NO_ERROR);
}

char *dbGetExportFieldName(Feature *this_feature, int index)
{
	return(this_feature->data.fieldname[index]);
}