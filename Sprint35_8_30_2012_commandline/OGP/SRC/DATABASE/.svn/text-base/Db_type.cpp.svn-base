/*
$Workfile: Db_type.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_type.cpp 18    2/21/01 9:32a Jnc $

$Modtime: 2/21/01 9:02a $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_type.cpp $
 * 
 * 18    2/21/01 9:32a Jnc
 * ReqDoc Datum Recall Feature: added DATUM_RECALL as a good feature type
 * 
 * 17    2/21/01 8:01a Mjl
 * Started Adding Faces and Trims.
 * 
 * 16    11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 15    10/13/00 10:13a Jpk
 * Added Min / Max Z for contour features.  Now we can use contour with
 * laser.
 * 
 * 14    8/14/00 8:03a Jpk
 * Added extended data pointer type value to eliminate database errors and
 * expand the usefullness of the pointer.
 * 
 * 13    4/26/00 3:25p Wei
 * FILLET(ASME radius) related changes
 * 
 * 12    4/21/00 12:02p Wei
 * lastdisplay for CENTOID's max/min radius
 * 
 * 11    11/04/99 5:15p Wei
 * Added CENTROID
 * 
 * 10    6/03/99 3:06p Ttc
 * DN 9723 version 4. Allow line, circle and plane to store last display 2
 * and 3.
 * 
 * 9     4/30/99 2:10p Ttc
 * Change COPY_FEATURE to HOLD_FEATURE.  This type is used by insert as a
 * place holder for the soon to be inserted feature.
 * 
 * 8     4/28/99 8:46a Ttc
 * Add cone.
 * 
 * 7     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 6     4/13/99 5:41p Ttc
 * Add datum plane feature.
 * 
 * 5     4/13/99 11:14a Ttc
 * Add angle orientations.
 * 
 * 4     4/12/99 11:23a Ttc
 * Add cone and datum plane feature type checks.
 * 
 * 3     4/02/99 1:18p Acm
 * 
 *    Rev 10.2   09 Jun 1998 10:57:56   ttc
 * Change edge trace feature to contour.  Add centroid last display setting.
 * 
 *    Rev 10.1   14 Apr 1998 12:02:48   ttc
 * Add edge trace checks.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:02   softadmn
 * Initial revision.
 * 
 *    Rev 9.1   26 Sep 1996 13:30:50   dmf
 * Added new functionality to rotary
 * 
 *    Rev 9.0   11 Mar 1996 11:12:12   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:55:48   softadmn
 * Initial revision.
 * 
 *    Rev 8.4   07 Feb 1996 09:46:12   dmf
 * Added user input
 * 
 *    Rev 8.3   11 Dec 1995 08:36:38   dmf
 * changes PT_PLANE case statement for the enhancment of HEIGHT_DEPTH
 * 
 *    Rev 8.2   12 Sep 1995 15:10:34   dmf
 * Changes for profile tolerance
 * 
 *    Rev 8.1   27 Feb 1995 15:37:04   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:22   softadmn
 * Initial revision.
*/

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


int dbGoodFeatureType(int feature_type)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Test the specified feature type for validity.					  */
/* Returns: TRUE if feature type is ok, FALSE if not.						  */
{
	int good = FALSE;					   /* set good flag to false initialy */
	switch (feature_type)						/* which feature type is this */
	{
		case POINT:
		case MIDPOINT:
		case LINE:
		case CIRCLE:
		case SPHERE:
		case ORIGIN:
		case AXIS:
		case DISTANCE:
		case PLANE:
		case PT_PLANE:
		case ANGULARITY:
		case PERPENDICULARITY:
		case PARALLELISM:
		case WIDTH:
		case INTERSECT:
		case ROTARY_FT:
		case GAGE_DIA:
		case GAGE_BALL:
		case HOLD_FEATURE:
		case MIN_MAX_AVG:		// this is here only to allow a rtn to load
		case DIGITAL_IO:
		case MATH:
		case BRANCH:
		case PROFILE:
		case USER_INPUT:
		case CONTOUR:
		case CYLINDER:
		case CONE:
		case CENTROID:
		case FILLET:
		case DATUM_PLANE:
		case FACE:
		case TRIM:
		case DATUM_RECALL:
		{
			good = TRUE;				 /* all these are good, other are not */
			break;
		}
		default:
			break;
	}
	return(good);										  /* return good flag */
}

int dbPutFeatureType(Feature *this_feature, int feature_type)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Set the feature type in the feature structure.					  */
/* Returns: NO_ERROR if feature type is ok, GOT_ERROR if not.				  */
{
	char *loc_func_name = "dbPutFeatureType";

	if (dbGoodFeatureType(feature_type))	  /* is this a good feature type? */
	{
		this_feature->data.type = feature_type;	   /* store it in the feature */
		return(NO_ERROR);							  /* return no error code */
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature type",feature_type);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetFeatureType(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the feature type from the feature structure.			  */
/* Returns: The current feature type.										  */
{
	return(this_feature->data.type);			   /* return the feature type */
}

int dbPutFeatureMode(Feature *this_feature, int measure_construct)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Stores the feature mode in the feature structure.				  */
/* Returns: NO_ERROR if valid mode, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutFeatureMode";

	/* if the feature mode is measure or construct 							  */
	if ((measure_construct == MEASURE) || (measure_construct == CONSTRUCT))
	{
		this_feature->data.flags.meas_const = measure_construct;   /* save it */
		return(NO_ERROR);							  /* return no error code */
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature mode",measure_construct);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetFeatureMode(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Get the feature mode from the feature structure. 				  */
/* Returns: The feature mode (measured or constructed).						  */
{
	return(this_feature->data.flags.meas_const);   /* return the current mode */
}

int dbIsFeatureMeasured(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Test whether this feature is measured.							  */
/* Returns: TRUE if the feature is measured, FALSE if not.					  */
{
	int measured = FALSE;		  /* set the measured flag to false initially */
	if (this_feature->data.flags.meas_const == MEASURE)	   /* if its measured */
		measured = TRUE;							  /* set the flag to true */
	return(measured);							  /* return the measured flag */
}

int dbIsFeatureConstructed(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Tests whether this feature is constructed.						  */
/* Returns: TRUE if the feature is constructed, FALSE if not.				  */
{
	int constructed = FALSE;   /* set the constructed flag to false initially */
	if (this_feature->data.flags.meas_const == CONSTRUCT)	/* if constructed */
		constructed = TRUE;							  /* set the flag to true */
	return(constructed);					   /* return the constructed flag */
}

int dbPutLastDisplay(Feature *this_feature, int last_displayed)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Set the number of the last feature size displayed.				  */
/* Returns: NO_ERROR if no error.											  */
{
	switch(dbGetFeatureType(this_feature))		/* which feature type is this */
	{
		case POINT:
		case MIDPOINT:
		case ORIGIN:
		case AXIS:
		case ANGULARITY:
		case PERPENDICULARITY:
		case PARALLELISM:
		case MATH:
		case BRANCH:
		case PROFILE:
		case USER_INPUT:
		case DATUM_PLANE:
		case DATUM_RECALL:
			break;					   /* don't care which was last displayed */
		case LINE:
		case CONE:
		{
			/* save the last displayed number if first or second angle 		  */
			if ((last_displayed == ANGLE1) || (last_displayed == ANGLE2))
				this_feature->data.last_display = last_displayed;
			break;
		}
		case PLANE:
		{
			/* save the last displayed number if first or second angle 		  */
			if ((last_displayed == ANGLE1) || (last_displayed == ANGLE2) ||
				(last_displayed == ANGLE3))
				this_feature->data.last_display = last_displayed;
			break;
		}
		case SPHERE:
		case CYLINDER:
		{
			/* save the last displayed number if radius or diameter			  */
			if ((last_displayed == RADIUS) || (last_displayed == DIAMETER))
				this_feature->data.last_display = last_displayed;
			break;
		}
		case CIRCLE:
		case CENTROID:
		{
			/* save the last displayed number                       		  */
			if ( (last_displayed == RADIUS) ||
				 (last_displayed == MAX_RADIUS) ||
				 (last_displayed == MIN_RADIUS) ||
				 (last_displayed == DIAMETER) ||
				 (last_displayed == MAX_DIAMETER) ||
				 (last_displayed == MIN_DIAMETER) )
				this_feature->data.last_display = last_displayed;
			break;
		}
		case DIGITAL_IO:
		{
			/* save the last displayed number                       		  */
			if ( (last_displayed == DIG_OUTPUT) ||
				 (last_displayed == DIG_INPUT) )
				this_feature->data.last_display = last_displayed;
			break;
		}
		case WIDTH:
		{
			/* save the last displayed number if width, min width, max width  */
			if ((last_displayed == CENTER_WIDTH) || (last_displayed == MIN_WIDTH) || (last_displayed == MAX_WIDTH))
				this_feature->data.last_display = last_displayed;
			break;
		}
		case INTERSECT:
		{
			switch (last_displayed)				   /* what was last displayed */
			{
				case ANGLE1:
				case ANGLE2:
				case ANGLE3:
				case ANGLE4:
				case INTERSECT1:
				case INTERSECT2:
				{										/* all valid, save it */
					this_feature->data.last_display = last_displayed;
					break;
				}
				default:
					break;
			}
			break;
		}
		case GAGE_DIA:
		case GAGE_BALL:
		{
			switch (last_displayed)
			{
				case INTERSECT1:
				case INTERSECT2:
				{										/* all valid, save it */
					this_feature->data.last_display = last_displayed;
					break;
				}
				default:
					break;
			}
			break;
		}
		case DISTANCE:
		{
			/* save the last displayed number if radius or diameter			  */
			if ((last_displayed == STRAIGHT_LINE) ||
				(last_displayed == CONCENTRICITY))
				this_feature->data.last_display = last_displayed;
			break;
		}
		case PT_PLANE:
		{
			if ((last_displayed == PERPEND_DIST) ||
				(last_displayed == HEIGHT_DEPTH))
				this_feature->data.last_display = last_displayed;
			break;
		}
		case ROTARY_FT:
		{
			if ((last_displayed == ROTARY_DEGREE) ||
				(last_displayed == ROTARY_RESULT))
				this_feature->data.last_display = last_displayed;
			break;
		}
		case CONTOUR:
		{
			if ((last_displayed == MAXIMUM_X) ||
				(last_displayed == MINIMUM_X) ||
				(last_displayed == MAXIMUM_Y) ||
				(last_displayed == MINIMUM_Y) ||
				(last_displayed == MAXIMUM_Z) ||
				(last_displayed == MINIMUM_Z) ||
				(last_displayed == MAXIMUM_ANGLE) ||
				(last_displayed == MINIMUM_ANGLE) ||
				(last_displayed == CENTROID) )
				this_feature->data.last_display = last_displayed;
			break;
		}
		default:
			break;
	}
	return(NO_ERROR);								  /* return no error code */
}

int dbGetLastDisplay(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/08/1991 */
/* Purpose: Extract the feature size number that was last displayed.		  */
/* Returns: The last feature size number displayed.							  */
{
	return(this_feature->data.last_display);	 /* return the last displayed */
}

int dbPutLastDisplay2(Feature *this_feature, int last_displayed)
{
	switch(dbGetFeatureType(this_feature))		/* which feature type is this */
	{
	case LINE:
	case CIRCLE:
	case PLANE:
	case CYLINDER:
	case CONE:
		/* save the last displayed number if radius or diameter			  */
		if ( (last_displayed == ANGLE_XY) ||
			 (last_displayed == ANGLE_YZ) ||
			 (last_displayed == ANGLE_ZX) ||
			 (last_displayed == ANGLE_NXY) ||
			 (last_displayed == ANGLE_NYZ) ||
			 (last_displayed == ANGLE_NZX) )
			this_feature->data.last_display2 = last_displayed;
		break;
	default:
		break;
	}
	return(NO_ERROR);								  /* return no error code */
}

int dbGetLastDisplay2(Feature *this_feature)
{
	return(this_feature->data.last_display2);	 /* return the last displayed */
}

int dbPutLastDisplay3(Feature *this_feature, int last_displayed)
{
	switch(dbGetFeatureType(this_feature))		/* which feature type is this */
	{
	case LINE:
	case CIRCLE:
	case PLANE:
	case CYLINDER:	
	case CONE:
		/* save the last displayed number if radius or diameter			  */
		if ( (last_displayed == ANGLE_3D) ||
			 (last_displayed == ANGLE_N3D) )
			this_feature->data.last_display3 = last_displayed;
		break;
	default:
		break;
	}
	return(NO_ERROR);								  /* return no error code */
}

int dbGetLastDisplay3(Feature *this_feature)
{
	return(this_feature->data.last_display3);	 /* return the last displayed */
}

// I'm going to put this here because it seems to be the only place that makes sense.
int dbGetFeatureExtendedDataType(Feature *this_feature)
{
	return(this_feature->data.extended_data_type);
}

int dbPutFeatureExtendedDataType(Feature *this_feature, int new_type)
{
	this_feature->data.extended_data_type = new_type;
	return(NO_ERROR);
}
