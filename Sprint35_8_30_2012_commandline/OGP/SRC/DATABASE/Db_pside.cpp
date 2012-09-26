/*
$Workfile: Db_pside.c $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_pside.c 3     5/28/99 11:45a Ttc $

$Modtime: 5/28/99 9:15a $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_pside.c $
 * 
 * 3     5/28/99 11:45a Ttc
 * Move prototypes outside functions.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:24   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:13:04   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:28   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:08   softadmn
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

Point_List *dbGetPointListPtr(Feature *, int);/* get point struct pointer */

int dbPutPointSideNum(Feature *this_feature, int point_number, int side_number)
/* Author: Terry T. Cramer									 Date: 09/02/1993 */
/* Purpose: Store the side number flag for the specified point in the feature.*/
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointSideNum";
	Point_List *this_point_ptr;		  /* pointer to specified point structure */
	int feature_type;

	/* get the pointer to the specified point_number point structure 		  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					   /* found the correct point */
	{
		feature_type = dbGetFeatureType(this_feature);
		if ( ( (feature_type == WIDTH) || (feature_type == INTERSECT) ) &&
			 (dbIsFeatureMeasured(this_feature) == TRUE) )
		{								  /* measured width or intersect only */
			if ( (side_number == 1) || (side_number == 2) ) /* must be 1 or 2 */
			{
				this_point_ptr->side_flag = side_number;  /* save side number */
				return(NO_ERROR);					  /* return no error code */
			}
			else
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"side number flag",side_number);
				return(GOT_ERROR);						 /* return error code */
			}
		}
		else /* not a measured width or intersect, don't care what side it is */
		{
			this_point_ptr->side_flag = side_number;	  /* save side number */
			return(NO_ERROR);						  /* return no error code */
		}
	}
	else		/* couldn't find the correct mega-point or allocate a new one */
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"side number",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointSideNum(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Extract the side number flag from specified point in the feature. */
/* Returns: The side number flag if point found, GOT_ERROR if not.			  */
{
	char *loc_func_name = "dbGetPointSideNum";
	Point_List *this_point_ptr;		  /* pointer to specified point structure */
	Point_List *dbGetPointListPtr(Feature *, int);/* get point struct pointer */

	/* get the pointer to the specified point_number point structure 		  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					   /* found the correct point */
	{
		return(this_point_ptr->side_flag);			/* return the side number */
	}
	else		   /* couldn't find the correct mega-point in the linked list */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"side number",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}
