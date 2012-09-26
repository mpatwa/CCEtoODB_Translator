/*
$Workfile: Db_reslt.c $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_reslt.c 6     10/06/00 1:52p Ttc $

$Modtime: 9/29/00 2:03p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_reslt.c $
 * 
 * 6     10/06/00 1:52p Ttc
 * SCR 11792.  Add output precision override to export and stats.  Major
 * output overhaul.
 * 
 * 5     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 4     4/13/99 11:14a Ttc
 * Add angle orientations.
 * 
 * 3     3/17/99 9:05a Ttc
 * Add direction to results.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:14   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:40   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:08   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:12   softadmn
 * Initial revision.
*/

/* DataBase includes */
#include "db_decl.h"
#include "db_proto.h"
#include "db_const.h"

void dbPutNumberLocations(Feature *this_feature, int number_locs)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Set the number of valid result locations for this feature.		  */
/* Returns: Nothing.														  */
{
	if ((number_locs > 0) && (number_locs <= MAX_LOCATIONS))	/* number ok? */
		this_feature->result.num_locs = number_locs;   /* save number of locs */
	return;
}

int dbGetNumberLocations(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Extract the number of valid result locations for this feature.	  */
/* Returns: The number of locations currently stored.						  */
{
	return(this_feature->result.num_locs); /* return the number of valid locs */	
}

void dbPutActualLocX(Feature *this_feature, double new_x)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Save the result X location in the feature result structure.		  */
/* Returns: Nothing.														  */
{
	this_feature->result.actual_location[0].x = new_x; /* save the x location */
	dbPutNumberLocations(this_feature,1);	 /* set number of valid locations */
	return;
}

double dbGetActualLocX(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the X result location from the feature structure.		  */
/* Returns: The X location.													  */
{
	return(this_feature->result.actual_location[0].x);	  /* return the X loc */
}

void dbPutActualLocY(Feature *this_feature, double new_y)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Save the result Y location in the feature result structure.		  */
/* Returns: Nothing.														  */
{
	this_feature->result.actual_location[0].y = new_y; /* save the y location */
	dbPutNumberLocations(this_feature,1);	 /* set number of valid locations */
	return;
}

double dbGetActualLocY(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the Y result location from the feature structure.		  */
/* Returns: The Y location.													  */
{
	return(this_feature->result.actual_location[0].y);	  /* return the Y loc */
}

void dbPutActualLocZ(Feature *this_feature, double new_z)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Save the result Z location in the feature result structure.		  */
/* Returns: Nothing.														  */
{
	this_feature->result.actual_location[0].z = new_z; /* save the z location */
	dbPutNumberLocations(this_feature,1);	 /* set number of valid locations */
	return;
}

double dbGetActualLocZ(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the Z result location from the feature structure.		  */
/* Returns: The Z location.													  */
{
	return(this_feature->result.actual_location[0].z);	  /* return the Z loc */
}

void dbPutNumberSizes(Feature *this_feature, int number_sizes)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Set the number of valid sizes in the feature result structure.	  */
/* Returns: Nothing.														  */
{
	if ((number_sizes > 0) && (number_sizes <= MAX_SIZES))	 /* valid number? */
		this_feature->result.num_sizes = number_sizes;			   /* save it */
	return;
}

int dbGetNumberSizes(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Extract the number of valid sizes stored in this feature.		  */
/* Returns: The number of currently valid sizes.							  */
{
	return(this_feature->result.num_sizes);	 /* return number of sizes stored */
}

void dbPutActualSize(Feature *this_feature, double new_size)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Store an actual feature size in the feature result structure.	  */
/* Returns: Nothing.														  */
{
	int this_one;						   /* where to save this feature size */

	this_one = dbGetNumberSizes(this_feature);/* how many are there currently */
	this_feature->result.actual_size[this_one] = new_size;	/* store it there */
	this_one++;					/* increment the number of valid sizes stored */
	dbPutNumberSizes(this_feature,this_one);				   /* and save it */
	return;
}

double dbGetActualSize(Feature *this_feature, int which_one)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Extract the specified feature size from the feature result.		  */
/* Returns: The feature size.												  */
{
	int this_one = which_one;

	if (this_one == 0)							/* return last size displayed */
	{
		this_one = dbGetLastDisplay(this_feature); /* get last size displayed */
	}

	if (dbGetFeatureType(this_feature) == CIRCLE)
	{
		switch(this_one)
		{
			case MAX_DIAMETER:
			case MIN_DIAMETER:
				this_one = DIAMETER;
				break;

			case MAX_RADIUS:
			case MIN_RADIUS:
				this_one = RADIUS;
				break;

			default:
				break;
		}
	}

	/* if the specified size is invalid, reset it and the last displayed size */
	if ((this_one < 1) || (this_one > dbGetNumberSizes(this_feature)))
	{
		this_one = 1;
	}
	return(this_feature->result.actual_size[this_one-1]);	   /* return size */
}

void dbPutActualDeviation(Feature *this_feature, double deviation)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Store the actual form deviation in the feature result structure.  */
/* Returns: Nothing.														  */
{
	this_feature->result.form_deviation = deviation;	/* save the deviation */
	return;
}

double dbGetActualDeviation(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the form deviation from the feature structure.			  */
/* Returns: The form deviation.												  */
{
	return(this_feature->result.form_deviation);	  /* return the deviation */
}

void dbPutSecondLocX(Feature *this_feature, double new_x)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Store a secondary feature Y location in the feature structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->result.actual_location[1].x = new_x; /* save the x location */
	dbPutNumberLocations(this_feature,2);	 /* set number of valid locations */
	return;
}

double dbGetSecondLocX(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Extract the secondary feature X location from the feature struct. */
/* Returns: The secondary X location value.									  */
{
	return(this_feature->result.actual_location[1].x);	  /* return the X loc */
}

void dbPutSecondLocY(Feature *this_feature, double new_y)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Store a secondary feature Y location in the feature structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->result.actual_location[1].y = new_y; /* save the y location */
	dbPutNumberLocations(this_feature,2);	 /* set number of valid locations */
	return;
}

double dbGetSecondLocY(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Extract the secondary feature Y location from the feature struct. */
/* Returns: The secondary Y location value.									  */
{
	return(this_feature->result.actual_location[1].y);	  /* return the Y loc */
}

void dbPutSecondLocZ(Feature *this_feature, double new_z)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Store a secondary feature Z location in the feature structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->result.actual_location[1].z = new_z; /* save the z location */
	dbPutNumberLocations(this_feature,2);	 /* set number of valid locations */
	return;
}

double dbGetSecondLocZ(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Extract the secondary feature Z location from the feature struct. */
/* Returns: The secondary Z location value.									  */
{
	return(this_feature->result.actual_location[1].z);	  /* return the Z loc */
}

// Direction of feature access functions
void dbPutDirectionX(Feature *this_feature, double direction_x)
{
	this_feature->result.direction.x = direction_x;	  /* save the x direction */
	return;
}

double dbGetDirectionX(Feature *this_feature)
{
	return(this_feature->result.direction.x);		/* return the X direction */
}

void dbPutDirectionY(Feature *this_feature, double direction_y)
{
	this_feature->result.direction.y = direction_y;	  /* save the y direction */
	return;
}

double dbGetDirectionY(Feature *this_feature)
{
	return(this_feature->result.direction.y);		/* return the Y direction */
}

void dbPutDirectionZ(Feature *this_feature, double direction_z)
{
	this_feature->result.direction.z = direction_z;	  /* save the z direction */
	return;
}

double dbGetDirectionZ(Feature *this_feature)
{
	return(this_feature->result.direction.z);		/* return the Z direction */
}

void dbPutActualOrientation(Feature *this_feature, double new_angle, int which_one)
{
	if (which_one <= 0) 
		which_one = 1;
	if (which_one > MAX_ANGLES)
		which_one = MAX_ANGLES;
	this_feature->result.orientation[which_one - 1] = new_angle;
	return;
}

double dbGetActualOrientation(Feature *this_feature, int which_one)
{
	if (which_one <= 0) 
		which_one = 1;
	if (which_one > MAX_ANGLES)
		which_one = MAX_ANGLES;
	return(this_feature->result.orientation[which_one - 1]);
}

void dbPutActualPosTol(Feature *this_feature, double pos_tol)
{
	this_feature->result.position_tol = pos_tol;
	return;
}

double dbGetActualPosTol(Feature *this_feature)
{
	return(this_feature->result.position_tol);
}

void dbPutModifiedPosTol(Feature *this_feature, double modified)
{
	this_feature->result.modified_tol = modified;
	return;
}

double dbGetModifiedPosTol(Feature *this_feature)
{
	return(this_feature->result.modified_tol);
}
