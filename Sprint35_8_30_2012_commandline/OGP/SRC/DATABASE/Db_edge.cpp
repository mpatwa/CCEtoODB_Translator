/*
$Workfile: Db_edge.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/Db_edge.c 4     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/Db_edge.c $
 * 
 * 4     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 3     5/28/99 11:45a Ttc
 * Move prototypes outside functions.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:16   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:46   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:12   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:34   softadmn
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


Point_List *dbGetPointListPtr(Feature *, int);	  /* get point struct pointer */

int dbPutPointEdgeSeek(Feature *this_feature, int point_number, int new_seek)
/* Author: Terry T. Cramer									 Date: 08/17/1992 */
/* Purpose: Store the edge seek flag for the specified point in the feature.  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointEdgeSeek";
	Point_List *this_point_ptr;		  /* pointer to specified point structure */

	/* get the pointer to the specified point_number point structure 		  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					   /* found the correct point */
	{
		this_point_ptr->edge_seek = new_seek;	   /* save the edge seek flag */
		return(NO_ERROR);							  /* return no error code */
	}
	else			   /* couldn't find the specified point's point structure */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointEdgeSeek(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Extract the edge seek flag from specified point in the feature.	  */
/* Returns: The edge seek flag if point found, GOT_ERROR if not.			  */
{
	char *loc_func_name = "dbGetPointEdgeSeek";
	Point_List *this_point_ptr;		  /* pointer to specified point structure */
	Point_List *dbGetPointListPtr(Feature *, int);/* get point struct pointer */

	/* get the pointer to the specified point_number point structure 		  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					   /* found the correct point */
	{
		return(this_point_ptr->edge_seek);		 /* return the edge seek flag */
	}
	else			   /* couldn't find the specified point's point structure */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}
