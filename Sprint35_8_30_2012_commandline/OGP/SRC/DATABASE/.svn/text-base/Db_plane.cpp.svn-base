/*
$Workfile: Db_plane.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_plane.cpp 10    6/08/01 11:12a Lsz $

$Modtime: 6/08/01 10:17a $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_plane.cpp $
 * 
 * 10    6/08/01 11:12a Lsz
 * Fixed SCR 13976. Only allow the 1x lens and the 2x lens radio buttons
 * be visible/selectable.  Renamed some pound defines to make more sense
 * (1x lens instead of none).
 * 
 * 9     5/07/01 9:12a Mjl
 * Had to make dbGoodFeaturePlane return true for trims and faces because
 * it is used as the flag for whether or not to turn off the reference
 * features. SCR#13736.
 * 
 * 8     3/15/01 10:05a Jpk
 * Added cases for TZM optics front lenses.
 * 
 * 7     2/21/01 9:33a Jnc
 * ReqDoc Datum Recall Feature: added DATUM_RECALL to dbGoodFeaturePlane
 * case
 * 
 * 6     5/18/00 6:18p Wei
 * new lens_code
 * 
 * 5     6/03/99 3:06p Ttc
 * DN 9723 version 4. Cone can have Y_PLANE set.
 * 
 * 4     4/13/99 5:41p Ttc
 * Add datum plane feature.
 * 
 * 3     4/13/99 11:14a Ttc
 * Allow XYZ_PLANE for plane setting.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:04   softadmn
 * Initial revision.
 * 
 *    Rev 9.3   25 Aug 1997 09:15:36   ttc
 * Add tp200 dock station.
 * 
 *    Rev 9.2   28 Jun 1996 08:36:06   ttc
 * Add led flag.
 * 
 *    Rev 9.1   08 Apr 1996 16:49:06   ttc
 * Add functions for lens code.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:16   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:55:50   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:04   softadmn
 * Initial revision.
*/

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

int dbGoodFeaturePlane(int feature_plane, int feature_type)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Check the plane type for the specified feature type.			  */
/* Returns: TRUE of ok, FALSE if not ok.									  */
{
	int good = FALSE;					   /* set good flag to false initialy */

	switch (feature_plane)						/* which plane are we testing */
	{
		case NO_PLANE:
		{
			if ( (feature_type != LINE) &&
				 (feature_type != CIRCLE) &&
				 (feature_type != ORIGIN) &&
				 (feature_type != AXIS) &&
				 (feature_type != DATUM_PLANE) &&
				 (feature_type != DATUM_RECALL) ) 
			{
				good = TRUE;
				break;
			}
		}
		case XY_PLANE:						  /* standard feature plane types */
		case XZ_PLANE:
		case YZ_PLANE:
		case XYZ_PLANE:											  /* all axis */
		{
			good = TRUE;									/* these are good */
			break;
		}
		case Z_PLANE:										   /* just z axis */
		{
			if ( (feature_type == ORIGIN) ||		 /* ok for origin feature */
				 (feature_type == MIN_MAX_AVG) )		   /* and min/max/avg */
				good = TRUE;
			break;
		}
		case X_PLANE:								/* just x or just y plane */
		case Y_PLANE:
		{
			if ( (feature_type == ORIGIN) ||
				 (feature_type == AXIS) ||
				 (feature_type == CONE) ||			 /* flags diameter/radius */
				 (feature_type == MIN_MAX_AVG) ||
				 (feature_type == TRIM) ||			// Trim and Face use this to tell whether
				 (feature_type == FACE) )			// or not to turn off ref feats in the model
				good = TRUE;  /* good for origin, axis or min/max/avg feature */
			break;
		}
		default:									  /* just passing through */
			break;
	}
	return(good);										  /* return good flag */
}

int dbPutFeaturePlane(Feature *this_feature, int feature_plane)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Set the specified plane type for the current feature type.		  */
/* Returns: NO_ERROR if plane type is valid, GOT_ERROR if not valid.		  */
{
	char *loc_func_name = "dbPutFeaturePlane";

	/* is the specified plane type ok for this feature type 				  */
	if (dbGoodFeaturePlane(feature_plane,dbGetFeatureType(this_feature)))
	{
		this_feature->data.plane = feature_plane;		/* set the plane type */
		return(NO_ERROR);							  /* return no error flag */
	}
	else							/* not a good plane type for this feature */
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature plane",feature_plane);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetFeaturePlane(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the plane type from the feature structure.				  */
/* Returns: The plane type stored in the feature structure.					  */
{
	return(this_feature->data.plane);				 /* return the plane type */
}

int dbPutFeatureLensCode(Feature *this_feature, int lens_code)
/* Author: Terry T. Cramer									 Date: 03/22/1996 */
/* Purpose: Set the specified lens code for the current feature type.		  */
/* Returns: NO_ERROR if lens code is valid, GOT_ERROR if not valid.			  */
{
	char *loc_func_name = "dbPutFeatureLensCode";

	switch (lens_code)
	{
		case LENS_CODE_SMALL_NONE: 		  /* small SmartScope style none (1x) */
		case LENS_CODE_SMALL_P5: 		 /* small SmartScope style .5x add-on */
		case LENS_CODE_SMALL_P75:		/* small SmartScope style .75x add-on */
		case LENS_CODE_SMALL_1P5:		/* small SmartScope style 1.5x add-on */
		case LENS_CODE_SMALL_2P0:		  /* small SmartScope style 2x add-on */
		case LENS_CODE_MIT_P5:			/* Avant style Mititoyo .5x mag sense */
		case LENS_CODE_MIT_1:			 /* Avant style Mititoyo 1x mag sense */
		case LENS_CODE_MIT_2P5:		   /* Avant style Mititoyo 2.5x mag sense */
		case LENS_CODE_MIT_5:			 /* Avant style Mititoyo 5x mag sense */
		case LENS_CODE_MIT_10:			/* Avant style Mititoyo 10x mag sense */
		case LENS_CODE_MIT_25:			/* Avant style Mititoyo 25x mag sense */
		case LENS_CODE_MIT_50:			/* Avant style Mititoyo 50x mag sense */
		case LENS_CODE_TOUCHPROBE:							   /* Touch Probe */
		case LENS_NO_CODE:									  /* No Lens Code */
		case LENS_CODE_TZM_MIT_2:	/* Telecentric Zoom with Mititoyo 2x lens */
		case LENS_CODE_TZM_MIT_4:	/* Telecentric Zoom with Mititoyo 4x lens */
		case LENS_CODE_TZM_MIT_8:	/* Telecentric Zoom with Mititoyo 8x lens */
		case LENS_CODE_TZM_MIT_20:	/* Telecentric Zoom with Mititoyo 20x lens */
		case LENS_CODE_TZM_MIT_40:	/* Telecentric Zoom with Mititoyo 40x lens */
		case LENS_CODE_TZM_MIT_1:	/* Telecentric Zoom with Mititoyo 1x lens  */
		{
			this_feature->data.lens_code = lens_code;/* store valid lens code */
			return(NO_ERROR);
		}
		default:								/* bad lens code - post error */
		{
			if (lens_code > LENS_NO_CODE)
			{
				this_feature->data.lens_code = lens_code;/* store valid lens code */
				return(NO_ERROR);
			}
			else
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature lens code",lens_code);
				return(GOT_ERROR);
			}
		}
	}
}

int dbGetFeatureLensCode(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/22/1996 */
/* Purpose: Extract the lens code from the feature structure.				  */
/* Returns: The lens code stored in the feature structure.					  */
{
	return(this_feature->data.lens_code);
}

void dbPutFeatureDockLocation(Feature *this_feature, int dock)
/* Author: Terry T. Cramer									 Date: 08/06/1997 */
/* Purpose: Set the specified dock location for the current feature.		  */
/* Returns: Nothing.														  */
{
	this_feature->data.dock_location = dock;/* store valid lens code */
	return;
}

int dbGetFeatureDockLocation(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 08/06/1997 */
/* Purpose: Extract the dock location from the feature structure.			  */
/* Returns: The dock location stored in the feature structure.				  */
{
	return(this_feature->data.dock_location);
}
