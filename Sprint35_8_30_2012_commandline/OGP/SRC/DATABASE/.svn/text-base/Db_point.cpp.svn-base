/*
$Workfile: Db_point.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_point.cpp 9     11/16/00 3:37p Ttc $

$Modtime: 11/16/00 3:37p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_point.cpp $
 * 
 * 9     11/16/00 3:37p Ttc
 * Fix loc_func_name in function dbGetPointMissedFlag.
 * 
 * 8     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 7     11/03/99 2:57p Ttc
 * Fix typo in set of y and z compensated point location.
 * 
 * 6     10/25/99 2:59p Ttc
 * Fix typo's (changed my mind about structure member name).
 * 
 * 5     10/25/99 2:58p Ttc
 * Add point direction vector for rotary integration.
 * 
 * 4     9/28/99 7:54a Ttc
 * 
 * 3     9/28/99 7:52a Ttc
 * Add feature compensated point location.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:06   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:22   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:55:56   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:06   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stddef.h>

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


Point_List *dbGetPointListPtr(Feature *, int);	/* get mega-point pointer */

Xyz_Vector *dbGetTakePointVector(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 01/29/1991 */
/* Purpose: Find the specified take point vector structure in the linked list.*/
/* Returns: A pointer to the specified take point vector structure.			  */
{
	char *loc_func_name = "dbGetTakePointVector";
	Point_List *this_point_ptr;		 /* pointer to the specified point vector */

	/* get the pointer to the specified point structure 					  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)		 /* if we found the right point structure */
	{
		return(&this_point_ptr->take_point);/* return ptr to the point vector */
	}
	else				/* didn't find the specified point in the linked list */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}

int dbPutPointX(Feature *this_feature, int point_number, double new_x)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store an X coordinate value for the specified point.			  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutPointX";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetTakePointVector(this_feature,point_number);
	if (this_point != NULL)				 /* found the right point in the list */
	{
		this_point->x = new_x;							 /* store the x value */
		return(NO_ERROR);							  /* return no error code */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetPointX(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the specified point's X value from the feature.			  */
/* Returns: The point's X value.											  */
{
	char *loc_func_name = "dbGetPointX";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetTakePointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->x);							/* return the x value */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutPointY(Feature *this_feature, int point_number, double new_y)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store a Y coordinate value for the specified point.				  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutPointY";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetTakePointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		this_point->y = new_y;							 /* store the y value */
		return(NO_ERROR);							  /* return no error code */
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetPointY(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the specified point's Y value from the feature.			  */
/* Returns: The point's Y value.											  */
{
	char *loc_func_name = "dbGetPointY";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetTakePointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->y);
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutPointZ(Feature *this_feature, int point_number, double new_z)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Store a Z coordinate value for the specified point.				  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutPointZ";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetTakePointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		this_point->z = new_z;							 /* store the z value */
		return(NO_ERROR);							  /* return no error code */
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetPointZ(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the specified point's Z value from the feature.			  */
/* Returns: The point's Z value.											  */
{
	char *loc_func_name = "dbGetPointZ";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetTakePointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->z);
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

Xyz_Vector *dbGetActPointVector(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 07/30/1992 */
/* Purpose: Find the specified actual point vector structure in linked list	  */
/* Returns: A pointer to the specified actual point vector structure.		  */
{
	char *loc_func_name = "dbGetActPointVector";
	Point_List *this_point_ptr;		 /* pointer to the specified point vector */

	/* get the pointer to the specified point structure 					  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)		 /* if we found the right point structure */
	{
		return(&this_point_ptr->act_point);	/* return ptr to the point vector */
	}
	else				/* didn't find the specified point in the linked list */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}

int dbPutActPointX(Feature *this_feature, int point_number, double new_x)
/* Author: Terry T. Cramer									 Date: 07/30/1992 */
/* Purpose: Store an X coordinate value for the specified actual point.		  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutActPointX";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetActPointVector(this_feature,point_number);
	if (this_point != NULL)				 /* found the right point in the list */
	{
		this_point->x = new_x;							 /* store the x value */
		return(NO_ERROR);							  /* return no error code */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetActPointX(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 07/30/1992 */
/* Purpose: Extract the specified point's actual X value from the feature.	  */
/* Returns: The point's actual X value.										  */
{
	char *loc_func_name = "dbGetActPointX";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetActPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->x);							/* return the x value */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutActPointY(Feature *this_feature, int point_number, double new_y)
/* Author: Terry T. Cramer									 Date: 07/30/1992 */
/* Purpose: Store a Y coordinate value for the specified actual point.		  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutActPointY";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetActPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		this_point->y = new_y;							 /* store the y value */
		return(NO_ERROR);							  /* return no error code */
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetActPointY(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 07/30/1992 */
/* Purpose: Extract the specified point's actual Y value from the feature.	  */
/* Returns: The point's actual Y value.										  */
{
	char *loc_func_name = "dbGetActPointY";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetActPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->y);
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutActPointZ(Feature *this_feature, int point_number, double new_z)
/* Author: Terry T. Cramer									 Date: 07/30/1992 */
/* Purpose: Store a Z coordinate value for the specified actual point.		  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutActPointZ";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetActPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		this_point->z = new_z;							 /* store the z value */
		return(NO_ERROR);							  /* return no error code */
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetActPointZ(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 07/30/1992 */
/* Purpose: Extract the specified point's actual Z value from the feature.	  */
/* Returns: The point's actual Z value.										  */
{
	char *loc_func_name = "dbGetActPointZ";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetActPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->z);
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

Xyz_Vector *dbGetCompPointVector(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 09/28/1999 */
/* Purpose: Find the specified compensated point vector structure linked list */
/* Returns: A pointer to the specified compensated point vector structure.	  */
{
	char *loc_func_name = "dbGetCompPointVector";
	Point_List *this_point_ptr;		 /* pointer to the specified point vector */

	/* get the pointer to the specified point structure 					  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)		 /* if we found the right point structure */
	{
		return(&this_point_ptr->comp_point);/* return ptr to the point vector */
	}
	else				/* didn't find the specified point in the linked list */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}

int dbPutCompPointX(Feature *this_feature, int point_number, double new_x)
/* Author: Terry T. Cramer									 Date: 09/28/1999 */
/* Purpose: Store an X coordinate value for the specified compensated point.  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutCompPointX";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetCompPointVector(this_feature,point_number);
	if (this_point != NULL)				 /* found the right point in the list */
	{
		this_point->x = new_x;							 /* store the x value */
		return(NO_ERROR);							  /* return no error code */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetCompPointX(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 09/28/1999 */
/* Purpose: Extract the specified point's compensated X value from the feature*/
/* Returns: The point's feature compensated X value.						  */
{
	char *loc_func_name = "dbGetCompPointX";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetCompPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->x);							/* return the x value */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutCompPointY(Feature *this_feature, int point_number, double new_y)
/* Author: Terry T. Cramer									 Date: 09/28/1999 */
/* Purpose: Store an Y coordinate value for the specified compensated point.  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutCompPointY";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetCompPointVector(this_feature,point_number);
	if (this_point != NULL)				 /* found the right point in the list */
	{
		this_point->y = new_y;							 /* store the y value */
		return(NO_ERROR);							  /* return no error code */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetCompPointY(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 09/28/1999 */
/* Purpose: Extract the specified point's compensated Y value from the feature*/
/* Returns: The point's feature compensated Y value.						  */
{
	char *loc_func_name = "dbGetCompPointY";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetCompPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->y);							/* return the y value */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutCompPointZ(Feature *this_feature, int point_number, double new_z)
/* Author: Terry T. Cramer									 Date: 09/28/1999 */
/* Purpose: Store an Z coordinate value for the specified compensated point.  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutCompPointZ";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetCompPointVector(this_feature,point_number);
	if (this_point != NULL)				 /* found the right point in the list */
	{
		this_point->z = new_z;							 /* store the z value */
		return(NO_ERROR);							  /* return no error code */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetCompPointZ(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 09/28/1999 */
/* Purpose: Extract the specified point's compensated Z value from the feature*/
/* Returns: The point's feature compensated Z value.						  */
{
	char *loc_func_name = "dbGetCompPointZ";
	Xyz_Vector *this_point;			 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_point = dbGetCompPointVector(this_feature,point_number);
	if (this_point != NULL)				/* found the right point in the list? */
	{
		return(this_point->z);							/* return the z value */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector pointer",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

Xyz_Vector *dbGetPointDirectionVector(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 10/25/1999 */
/* Purpose: Find the specified point vector structure in the linked list.	  */
/* Returns: A pointer to the specified point vector structure.				  */
{
	char *loc_func_name = "dbGetPointDirectionVector";
	Point_List *this_point_ptr;		 /* pointer to the specified point vector */

	/* get the pointer to the specified point structure 					  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)		 /* if we found the right point structure */
	{
		return(&this_point_ptr->direction_vector); /* return point vector ptr */
	}
	else				/* didn't find the specified point in the linked list */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}

int dbPutPointDirectionVectorX(Feature *this_feature, int point_number, double new_x)
/* Author: Terry T. Cramer									 Date: 10/25/1999 */
/* Purpose: Store an X vector value for the specified point.				  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutPointDirectionVectorX";
	Xyz_Vector *this_vector;		 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_vector = dbGetPointDirectionVector(this_feature,point_number);
	if (this_vector != NULL)			 /* found the right point in the list */
	{
		this_vector->x = new_x;							 /* store the x value */
		return(NO_ERROR);							  /* return no error code */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetPointDirectionVectorX(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 10/25/1999 */
/* Purpose: Extract the specified point's X vector value from the feature.	  */
/* Returns: The point's X vector value.										  */
{
	char *loc_func_name = "dbGetPointDirectionVectorX";
	Xyz_Vector *this_vector;		 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_vector = dbGetPointDirectionVector(this_feature,point_number);
	if (this_vector != NULL)			/* found the right point in the list? */
	{
		return(this_vector->x);							/* return the x value */
	}
	else									 /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutPointDirectionVectorY(Feature *this_feature, int point_number, double new_y)
/* Author: Terry T. Cramer									 Date: 10/25/1999 */
/* Purpose: Store a Y vector value for the specified point.					  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutPointDirectionVectorY";
	Xyz_Vector *this_vector;		 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_vector = dbGetPointDirectionVector(this_feature,point_number);
	if (this_vector != NULL)			/* found the right point in the list? */
	{
		this_vector->y = new_y;							 /* store the y value */
		return(NO_ERROR);							  /* return no error code */
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetPointDirectionVectorY(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 10/25/1999 */
/* Purpose: Extract the specified point's Y vector value from the feature.	  */
/* Returns: The point's Y vector value.										  */
{
	char *loc_func_name = "dbGetPointDirectionVectorY";
	Xyz_Vector *this_vector;		 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_vector = dbGetPointDirectionVector(this_feature,point_number);
	if (this_vector != NULL)			/* found the right point in the list? */
	{
		return(this_vector->y);
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutPointDirectionVectorZ(Feature *this_feature, int point_number, double new_z)
/* Author: Terry T. Cramer									 Date: 10/25/1999 */
/* Purpose: Store a Z vector value for the specified point.					  */
/* Returns: NO_ERROR if operation successful, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutPointDirectionVectorZ";
	Xyz_Vector *this_vector;		 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_vector = dbGetPointDirectionVector(this_feature,point_number);
	if (this_vector != NULL)			/* found the right point in the list? */
	{
		this_vector->z = new_z;							 /* store the z value */
		return(NO_ERROR);							  /* return no error code */
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

double dbGetPointDirectionVectorZ(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 10/25/1999 */
/* Purpose: Extract the specified point's Z vector value from the feature.	  */
/* Returns: The point's Z vector value.										  */
{
	char *loc_func_name = "dbGetPointDirectionVectorZ";
	Xyz_Vector *this_vector;		 /* pointer to the specified point vector */
	
	/* get the pointer to the specified point_number point in the linked list */
	this_vector = dbGetPointDirectionVector(this_feature,point_number);
	if (this_vector != NULL)			/* found the right point in the list? */
	{
		return(this_vector->z);
	}
	else								     /* didn't find the correct point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point vector",point_number);
		return(BAD_VALUE);						  /* return a bad value error */
	}
}

int dbPutPointMissedFlag(Feature *this_feature, int point_number, int missed_it)
/* Author: Terry T. Cramer									 Date: 09/12/1994 */
/* Purpose: Store the missed flag for the specified point in the feature.	  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointMissedFlag";
	Point_List *this_point_ptr;		  /* pointer to specified point structure */

	/* get the pointer to the specified point_number point structure 		  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					   /* found the correct point */
	{
		this_point_ptr->missed = missed_it;			  /* save the missed flag */
		return(NO_ERROR);							  /* return no error code */
	}
	else			   /* couldn't find the specified point's point structure */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointMissedFlag(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 09/12/1994 */
/* Purpose: Extract the missed flag from specified point in the feature.	  */
/* Returns: The missed flag if point found, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbGetPointMissedFlag";
	Point_List *this_point_ptr;		  /* pointer to specified point structure */

	/* get the pointer to the specified point_number point structure 		  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					   /* found the correct point */
	{
		return(this_point_ptr->missed);				/* return the missed flag */
	}
	else			   /* couldn't find the specified point's point structure */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

