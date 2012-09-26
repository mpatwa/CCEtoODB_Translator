/*
$Workfile: Db_featn.c $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_featn.c 5     6/07/00 8:07a Ttc $

$Modtime: 5/26/00 3:29p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_featn.c $
 * 
 * 5     6/07/00 8:07a Ttc
 * SCR 10714.  We have to check for the very last feature instead of just
 * the last feature.
 * 
 * 4     5/22/00 1:40p Ttc
 * SCR 10474. dbNextFeatureNumber now returns an int.
 * 
 * 3     5/28/99 11:46a Ttc
 * Move prototypes outside functions.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:02   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:10   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:55:48   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:40   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stddef.h>
#include <limits.h>

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

Mega_Point *dbGetMegaPoint(Feature *, int);		/* get the mega-point pointer */
int dbVeryLastFeatureNumber(void);

int dbNextFeatureNumber(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Set the feature number to the last feature in the DB plus 1.	  */
/* Returns: Nothing.														  */
{
	int error = NO_ERROR;

	/* set this feature's number to one more than the last one in the DB 	  */
	if (dbVeryLastFeatureNumber() < SHRT_MAX)
		this_feature->data.number = dbGetLastFeatureNumber() + 1;
	else
		error = NO_MEMORY;
	return(error);
}

int dbGetFeatureNumber(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the feature number from the feature structure.			  */
/* Returns: The feature number.												  */
{
	return(this_feature->data.number);		  /* return this feature's number */
}

void dbOverrideFeatureNumber(Feature *this_feature, int this_feature_number)
/* Author: Terry T. Cramer									 Date: 04/17/1991 */
/* Purpose: Set the feature number to the specified feature number.			  */
/* Returns: Nothing.														  */
{
	/* set this feature's number to the one specified by the caller.	 	  */
	this_feature->data.number = this_feature_number;
	return;
}

int dbPutRefFeatureNumber(Feature *this_feature, int point_number,
						  int reference_feature)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Put a reference feature number in the feature structure.		  */
/* Returns: GOT_ERROR if error in finding the point, NO_ERROR if no error.	  */
{
	char *loc_func_name = "dbPutRefFeatureNumber";
	Mega_Point *this_point;	  /* pointer to the mega-point in the linked list */

	/* get a pointer to the mega-point in which to store this feature number  */
	this_point = dbGetMegaPoint(this_feature,point_number);
	if (this_point != NULL)					  /* found the correct mega-point */
	{
		this_point->feature_number = reference_feature;    /* save the number */
		return(NO_ERROR);							  /* return no error code */
	}
	else		/* couldn't find the correct mega-point or allocate a new one */
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"reference feature",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetRefFeatureNumber(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/12/1991 */
/* Purpose: Get a reference feature number from the feature structure.		  */
/* Returns: The reference feature number, -1 if couldn't find the mega-point. */
{
	char *loc_func_name = "dbGetRefFeatureNumber";
	Mega_Point *this_point;		/* pointer to a mega-point in the linked list */
	Mega_Point *dbGetMegaPoint(Feature *, int);		/* get mega-point pointer */

	/* get the mega-point pointer to the point containing the feature number  */
	this_point = dbGetMegaPoint(this_feature,point_number);
	if (this_point != NULL)						  /* found correct mega-point */
	{
		return(this_point->feature_number);		 /* return the feature number */
	}
	else		   /* couldn't find the correct mega-point in the linked list */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"reference feature",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}
