/*
$Workfile: Db_basic.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/Db_basic.c 3     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/Db_basic.c $
 * 
 * 3     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 *    Rev 10.1   14 Apr 1998 12:02:28   ttc
 * Add extra dimension for edge trace.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:12   softadmn
 * Initial revision.
 * 
 *    Rev 9.1   18 Mar 1996 13:03:10   ttc
 * Delete standard deviation (point elimination) functions.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:32   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:04   softadmn
 * Initial revision.
 * 
 *    Rev 8.1   01 Aug 1995 13:44:30   dmf
 * Added function for standard deviation
 * 
 *    Rev 8.0   18 Jan 1995 15:25:30   softadmn
 * Initial revision.
*/

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

int dbPutBasicDimension(Feature *this_feature, double dimension)
/* Author: Terry T. Cramer									 Date: 02/22/1991 */
/* Purpose: Set the feature's basic dimension in the feature structure.		  */
/* Returns: NO_ERROR.														  */
{
	this_feature->data.basic_dim = dimension;	   /* store it in the feature */
	return(NO_ERROR);								  /* return no error code */
}

double dbGetBasicDimension(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/22/1991 */
/* Purpose: Extract the basic dimension from the feature structure.			  */
/* Returns: The current basic dimension value.								  */
{
	return(this_feature->data.basic_dim);		/* return the basic dimension */
}

int dbPutExtraDimension(Feature *this_feature, double dimension)
/* Author: Terry T. Cramer									 Date: 04/14/1998 */
/* Purpose: Set the feature's extra dimension in the feature structure.		  */
/* Returns: NO_ERROR.														  */
{
	this_feature->data.extra_dim = dimension;	   /* store it in the feature */
	return(NO_ERROR);								  /* return no error code */
}

double dbGetExtraDimension(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 04/14/1998 */
/* Purpose: Extract the extra dimension from the feature structure.			  */
/* Returns: The current extra dimension value.								  */
{
	return(this_feature->data.extra_dim);		/* return the basic dimension */
}
