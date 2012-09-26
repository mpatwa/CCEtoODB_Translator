/*
$Workfile: Db_targt.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/Db_targt.c 4     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/Db_targt.c $
 * 
 * 4     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 3     5/28/99 11:45a Ttc
 * Move prototypes outside functions.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:10   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:30   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:02   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:18   softadmn
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


Point_List *dbGetPointListPtr(Feature *, int);/* get point struct pointer */

Target_Type *dbGetPointTarget(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 01/29/1991 */
/* Purpose: Find the specified target structure in the linked list of points. */
/* Returns: A pointer to the specified point_number target structure.		  */
{
	char *loc_func_name = "dbGetPointTarget";
	Point_List *this_point_ptr;				  /* pointer to a point structure */

	/* get the pointer to the specified point's point structure				  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					  /* found the correct point? */
	{
		return(&this_point_ptr->target);   /* return pointer to target struct */
	}
	else				 /* couldn't find the specified point_number'th point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}

int dbPutPointTargetType(Feature *this_feature, int point_number, int new_type)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store a target type for a specified point.						  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointTargetType";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)				/* was the specified point found? */
	{
		this_target->type = new_type;				 /* store the target type */
		return(NO_ERROR);							  /* return no error code */
	}
	/* couldn't find the specified point									  */
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
	return(GOT_ERROR);									 /* return error code */
}

int dbGetPointTargetType(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Extract the target type for a specified point from the feature.	  */
/* Returns: The target type.												  */
{
	char *loc_func_name = "dbGetPointTargetType";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		return(this_target->type);				 /* return this target's type */
	}
	else						  /* couldn't find the specified point target */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbPutPointTargetColor(Feature *this_feature, int point_number, 
						  int new_color)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store the target color for a specified point in the feature.	  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not. 						  */
{
	char *loc_func_name = "dbPutPointTargetColor";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		this_target->color = new_color;				/* store the target color */
		return(NO_ERROR);							  /* return no error code */
	}
	else						  /* couldn't find the specified point target */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointTargetColor(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Extract the target color for a specified point in the feature.	  */
/* Returns: The target color.												  */
{
	char *loc_func_name = "dbGetPointTargetColor";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		return(this_target->color);				   /* return the target color */
	}
	else						  /* couldn't find the specified point target */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbPutPointTargetSpacing(Feature *this_feature, int point_number, 
							int new_spacing1, int new_spacing2)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store the target spacing for a specified point in the feature.	  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointTargetSpacing";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		this_target->spacing1 = new_spacing1;	 /* store the target spacings */
		this_target->spacing2 = new_spacing2;
		return(NO_ERROR);							  /* return no error code */
	}
	else						  /* couldn't find the specified point target */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointTargetSpacing1(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Extract the target spacing from a specified point in the feature. */
/* Returns: The first target spacing.										  */
{
	char *loc_func_name = "dbGetPointTargetSpacing1";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		return(this_target->spacing1);			 /* return the target spacing */
	}
	else						  /* couldn't find the specified point target */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointTargetSpacing2(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Extract the target spacing from a specified point in the feature. */
/* Returns: The second target spacing.										  */
{
	char *loc_func_name = "dbGetPointTargetSpacing2";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		return(this_target->spacing2);			 /* return the target spacing */
	}
	else						  /* couldn't find the specified point target */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbPutPointTargetFilename(Feature *this_feature, int point_number, 
							 char *user_filename)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store a target filename for a specified point in the feature.	  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointTargetFilename";
	Target_Type *this_target;				 /* pointer to a target structure */
	unsigned int length;					 /* length of the string to store */
	unsigned int i;				 /* loop control variable and array subscript */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		length = strlen(user_filename) + 1;		  /* find the filename length */
		if (length == 1)							 /* no length to filename */
		{
			if (this_target->filename != NULL) /* is there already a filename */
			{
				free(this_target->filename);					   /* free it */
				this_target->filename = NULL;			 /* clear the pointer */
			}
		}
		else										 /* filename has a length */
		{
			/* allocate a character array to store the filename in			  */
			this_target->filename = (char *)calloc(length,sizeof(char));
			if (this_target->filename != NULL)	/* if allocate was successful */
			{
				for (i = 0; i < length; i++)/* for each character in filename */
				{
					this_target->filename[i] = user_filename[i]; /* save char */
				}
			}
			else
			{
				/* couldn't allocate a new string array, return error		  */
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(NO_MEMORY);
			}
		}
	}
	else
	{
		/* couldn't find the specified point target, return error			  */
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
	return(NO_ERROR);								  /* return no error code */
}

char *dbGetPointTargetFilename(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/06/1991 */
/* Purpose: Extract the target filename from a specified point in the feature.*/
/* Returns: A pointer to the target filename character array.				  */
{
	char *loc_func_name = "dbGetPointTargetFilename";
	Target_Type *this_target;				 /* pointer to a target structure */

	/* get a pointer to the specified point's target structure				  */
	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)					  /* found the correct point? */
	{
		return(this_target->filename);	  /* return a pointer to the filename */
	}
	else						  /* couldn't find the specified point target */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}
