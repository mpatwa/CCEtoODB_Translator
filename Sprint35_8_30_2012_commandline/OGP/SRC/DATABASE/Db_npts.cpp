/*
$Workfile: Db_npts.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_npts.cpp 36    9/11/01 10:00a Mjl $

$Modtime: 9/11/01 8:58a $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_npts.cpp $
 * 
 * 36    9/11/01 10:00a Mjl
 * Fixed the check in dbEnoughFeaturePoints for composite contours.. SCR
 * #14671.
 * 
 * 35    7/23/01 9:35a Ttc
 * SCR 14296.  Contours and edge traces must have more than 1 point to be
 * valid.
 * 
 * 34    7/11/01 2:52p Ttc
 * SCR 14182.  Add ability to use contours in composite spheres.
 * In order to not have to change 4 functions in 3 different files
 * everytime someone wants another feature type valid for composites, I
 * made functions that return whether a reference feature type is valid
 * for compositing.
 * 
 * 33    4/10/01 3:10p Mjl
 * Fixed dbCheckCompositeReferenceFeatures for faces.
 * 
 * 32    4/10/01 12:58p Ttc
 * Move Matt's declarations to the top of the function so I can convert
 * this file back to ".c" for SmartFit.
 * 
 * 31    4/10/01 11:32a Mjl
 * Fixed dbCheckCompositeReference for trims and faces.
 * scr #13515
 * 
 * 30    3/26/01 1:33p Mjl
 * Made it so a face can be constructed from one arc or contour.
 * scr #13315
 * 
 * 29    3/20/01 11:21a Ttc
 * Somehow, blank inserted features are being saved in the routine.  Add
 * case back in for enough or good number of points on these features to
 * allow 0 points because that's what they have.
 * 
 * 28    2/21/01 9:34a Jnc
 * ReqDoc Datum Recall Feature: added DATUM_RECALL to
 * dbGoodNumberOfPoints.
 * 
 * 27    2/21/01 8:01a Mjl
 * Started Adding Faces and Trims.
 * 
 * 26    1/24/01 1:08p Mjl
 * Added constructed and composite contours.
 * 
 * 25    10/23/00 1:14p Mjl
 * scr #11575 added lines, circles and contours to the list of features
 * that can be used to construct a composite plane.
 * 
 * 24    4/26/00 3:25p Wei
 * FILLET(ASME radius) related changes
 * 
 * 23    4/14/00 8:21a Ttc
 * SCR 9366. Allow composite spheres.
 * 
 * 22    12/02/99 1:42p Wei
 * Added CENTROID case
 * 
 * 21    8/27/99 2:18p Ttc
 * SCR 6264.  Allow rotary feature to only have 1 touch probe safe point.
 * 
 * 20    8/25/99 8:53a Ttc
 * SCR 6714.  Add ability to construct a composite circle from 1 cone or
 * cylinder.
 * 
 * 19    8/20/99 1:15p Wei
 * Added Cylinder and Cone types to dbCheckCircleRefernceType()
 * 
 * 18    8/19/99 11:07a Wei
 * added dbCheckCircleReferenceTypes() and put it in
 * dbEnoughFeaturePoints()
 * 
 * 17    8/18/99 3:26p Wei
 * Rollback to previous version
 * 
 * 15    7/30/99 12:43p Ttc
 * SCR 6669.  If composite circle, check number of points on reference
 * feature.  If its 3 or more, 1 reference feature is enough for the
 * composite, otherwise we need 2.
 * 
 * 14    5/28/99 11:47a Ttc
 * Move prototypes outside functions.
 * 
 * 13    5/19/99 4:10p Ttc
 * SCR 6554.  Add composite constructed planes.
 * 
 * 12    4/30/99 2:10p Ttc
 * Remove COPY_FEATURE (old obsolete feature type).
 * 
 * 11    4/28/99 8:46a Ttc
 * Add cone.
 * 
 * 10    4/24/99 12:01p Ttc
 * SCR 6451. If ref feature type is circle, must have at least 2 for a
 * composite cylinder.
 * 
 * 9     4/23/99 1:48p Ttc
 * SCR 6420. Allow circles for composite cylinders.
 * 
 * 8     4/13/99 5:41p Ttc
 * Add datum plane feature.
 * 
 * 7     4/13/99 8:27a Ttc
 * Only allow cylinder reference steps for composite cylinders.
 * 
 * 6     4/12/99 11:24a Ttc
 * Fix number of points for composite cylinders.
 * 
 * 5     4/02/99 1:18p Acm
 * 
 *    Rev 10.6   02 Sep 1998 13:59:04   ttc
 * Use new direct memory db access function.
 * 
 *    Rev 10.5   24 Jul 1998 08:22:18   ttc
 * Allow lines as composite references on circles.
 * 
 *    Rev 10.4   09 Jun 1998 10:57:40   ttc
 * Change edge trace feature to contour.  Add centroid last display setting.
 * 
 *    Rev 10.3   17 Apr 1998 11:16:08   ttc
 * Change call to dbCheckCircleReferenceFeatures to dbCheckCompositeReference
 * Features and pass the composite feature type.  Allow 1 composite ref feat 
 * for lines.
 * 
 *    Rev 10.2   16 Apr 1998 16:33:24   ttc
 * Pass the point number to add mega point so it can be saved.
 * 
 *    Rev 10.1   14 Apr 1998 12:02:48   ttc
 * Add edge trace checks.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:04   softadmn
 * Initial revision.
 * 
 *    Rev 9.3   15 Dec 1997 10:30:52   ttc
 * Allow 1 or 2 reference features for circles if the reference features are
 * circles.
 * 
 *    Rev 9.2   18 Jul 1996 09:12:26   ttc
 * Allow more than 2 points on midpoint and distance if touch probe in use.
 * 
 *    Rev 9.1   08 Apr 1996 16:49:28   ttc
 * Change dbGoodNumberOfPoints function parameter list.  Added functions for
 * get and set of number of measured points in a feature.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:18   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:55:52   softadmn
 * Initial revision.
 * 
 *    Rev 8.4   07 Feb 1996 09:46:10   dmf
 * Added user input
 * 
 *    Rev 8.3   03 Oct 1995 14:04:34   dmf
 * Added construct point
 * 
 *    Rev 8.2   12 Sep 1995 15:10:10   dmf
 * Changes for profile tolerance
 * 
 *    Rev 8.1   27 Feb 1995 15:36:58   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:58   softadmn
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

/* adds a mega-point to the linked list */
int dbAddMegaPoint(Feature *, int mega_point_number);
int dbDeleteMegaPoint(Feature *);			   /* free a mega-point structure */

int dbGoodNumberOfPoints(Feature *this_feature, int number_of_points)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Find out if this many points is valid for this feature type.	  */
/* Returns: 1 if good number of points, 0 if not.							  */
{
	int good = FALSE;					   /* set good flag to false initialy */

	switch (dbGetFeatureType(this_feature))				/* which feature type */
	{
		case ROTARY_FT:
		{
			if (dbGetFeatureMode(this_feature) == MEASURE)
			{
				if ((number_of_points >= 0) && (number_of_points <= 1))
				{
					good = TRUE;		 /* only zero or one point is allowed */
				}
			}
			else
			{
				if (number_of_points == 0)
				{
					good = TRUE;				/* can't have any points here */
				}
			}
			break;
		}	
		case POINT:
		{
			if ( (dbGetFeatureMode(this_feature) == MEASURE) &&
				 (dbGetFeatureLensCode(this_feature) == LENS_CODE_TOUCHPROBE) )
			{
				if ((number_of_points >= 0) && (number_of_points <= MAX_POINTS))
				{
					good = TRUE;			  /* zero to MAX_POINTS are valid */
				}
				break;
			} /* else fall thru */
		}
		case ORIGIN:
		case AXIS:
		case PROFILE:
		case DATUM_PLANE:
		case CENTROID:
		case DATUM_RECALL: 
		{
			if ((number_of_points >= 0) && (number_of_points <= 1))
			{
				good = TRUE;			 /* only zero or one point is allowed */
			}
			break;
		}	
		case MIDPOINT:
		case DISTANCE:
		{
			if ( (dbGetFeatureMode(this_feature) == MEASURE) &&
				 (dbGetFeatureLensCode(this_feature) == LENS_CODE_TOUCHPROBE) )
			{
				if ((number_of_points >= 0) && (number_of_points <= MAX_POINTS))
				{
					good = TRUE;			  /* zero to MAX_POINTS are valid */
				}
				break;
			} /* else fall thru */
		}
		case GAGE_DIA:
		case GAGE_BALL:
		{
			if ((number_of_points >= 0) && (number_of_points <= 2))
			{
				good = TRUE;			   /* zero, one or two points allowed */
			}
			break;
		}	
		case WIDTH:
		case INTERSECT:
		{
			if (dbGetFeatureMode(this_feature) == MEASURE)
			{
				if ((number_of_points >= 0) && (number_of_points <= MAX_POINTS))
				{
					good = TRUE;			  /* zero to MAX_POINTS are valid */
				}
			}
			else
			{
				if ((number_of_points >= 0) && (number_of_points <= 2))
				{
					good = TRUE;		   /* zero, one or two points allowed */
				}
			}
			break;
		}	
		case ANGULARITY:
		case PERPENDICULARITY:
		case PARALLELISM:
		{
			if ((number_of_points >= 0) && (number_of_points <= 2))
			{
				good = TRUE;		   	   /* zero, one or two points allowed */
			}
			break;
		}	
		case LINE:
		case CIRCLE:
		case SPHERE:
		case PLANE:
		case HOLD_FEATURE:
		case MIN_MAX_AVG:
		case CONTOUR:
		case CYLINDER:
		case CONE:
		case FILLET:
		case FACE:
		case TRIM:
		{
			if ((number_of_points >= 0) && (number_of_points <= MAX_POINTS))
			{
				good = TRUE;				  /* zero to MAX_POINTS are valid */
			}
			break;
		}	
		case PT_PLANE:
		{
			if ((number_of_points >= 0) && (number_of_points <= 4))
			{
				good = TRUE;						/* zero to 4 points valid */
			}
			break;
		}	
		case DIGITAL_IO:
		case MATH:
		case USER_INPUT:
		{
			if (number_of_points == 0)
			{
				good = TRUE;					/* can't have any points here */
			}
			break;
		}
		case BRANCH:
		{
			if ((number_of_points >= 0) && (number_of_points <= 2))
			{
				good = TRUE;			   /* zero, one or two points allowed */
			}
			break;
		}
		default:
			break;									 /* good is already false */
	}
	return(good);								   /* return good or not good */
}

int dbIncNumberPoints(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Increment number of points for feature, add any storage required. */
/* Returns: The new number of points, GOT_ERROR if not incremented.			  */
{
	char *loc_func_name = "dbIncNumberPoints";

	/* if the number of points + 1 is a good number of points				  */
	if (dbGoodNumberOfPoints(this_feature,this_feature->data.number_points + 1))
	{
		/* is adding a mega-point ok */
		if (dbAddMegaPoint(this_feature, this_feature->data.number_points + 1) == 0)
		{
			this_feature->data.number_points++;	/* increment number of points */
			return(this_feature->data.number_points);	 /* return new number */
		}
	}
	errPost(loc_func_name,DATABASE_ERROR,INC_POINT_FAILED,NULL,NO_VAL);
	return(GOT_ERROR);	   /* not ok to increment or adding mega-point failed */
}

int dbDecNumberPoints(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Decrement number of points for feature, freeing any storage.	  */
/* Returns: The new number of points, GOT_ERROR if not decremented.			  */
{
	char *loc_func_name = "dbDecNumberPoints";

	/* if the number of points - 1 is a good number of points				  */
	if (dbGoodNumberOfPoints(this_feature,this_feature->data.number_points - 1))
	{
		if (dbDeleteMegaPoint(this_feature) == NO_ERROR)/* deleted mega-point */
		{
			this_feature->data.number_points--;	/* decrement number of points */
			return(this_feature->data.number_points);	 /* return new number */
		}
	}
	errPost(loc_func_name,DATABASE_ERROR,DELETE_FAILED,NULL,NO_VAL);
	return(GOT_ERROR);	  /* not ok to decrement or freeing mega-point failed */
}

int dbGetNumberPoints(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Extract the current number of points from the feature structure.  */
/* Returns: The current number of points.									  */
{
	return(this_feature->data.number_points);  /* return the number of points */
}

void dbPutNumberPoints(Feature *this_feature, int number_points)
/* Author: Terry T. Cramer									 Date: 09/15/1994 */
/* Purpose: Set the number of points in the feature structure.				  */
/*          This should only be used during run when the number of points is  */
/*          zeroed and then set as the points are run.                        */
/* Returns: Nothing.														  */
{
	this_feature->data.number_points = number_points; /* set number of points */
	return;
}

int dbGoodLineCompositeRefFeatType(int ref_feature_type)
{
	int good = FALSE;
	if ( (ref_feature_type == LINE) ||
		 (ref_feature_type == CONTOUR) )
		good = TRUE;
	return(good);
}

int dbGoodCircleCompositeRefFeatType(int ref_feature_type)
{
	int good = FALSE;
	if ( (ref_feature_type == CIRCLE) ||
		 (ref_feature_type == LINE) ||
		 (ref_feature_type == CONTOUR) ||
		 (ref_feature_type == CYLINDER) ||
		 (ref_feature_type == CONE) )
		good = TRUE;
	return(good);
}

int dbGoodPlaneCompositeRefFeatType(int ref_feature_type)
{
	int good = FALSE;
	if ( (ref_feature_type == PLANE) ||
		 (ref_feature_type == LINE) ||
		 (ref_feature_type == CIRCLE) ||
		 (ref_feature_type == CONTOUR) )
		good = TRUE;
	return(good);
}

int dbGoodCylinderCompositeRefFeatType(int ref_feature_type)
{
	int good = FALSE;
	if ( (ref_feature_type == CYLINDER) ||
		 (ref_feature_type == CIRCLE) )
		good = TRUE;
	return(good);
}

int dbGoodConeCompositeRefFeatType(int ref_feature_type)
{
	int good = FALSE;
	if ( (ref_feature_type == CONE) ||
		 (ref_feature_type == CIRCLE) )
		good = TRUE;
	return(good);
}

int dbGoodSphereCompositeRefFeatType(int ref_feature_type)
{
	int good = FALSE;
	if ( (ref_feature_type == SPHERE) ||
		 (ref_feature_type == CIRCLE) ||
		 (ref_feature_type == CONTOUR) )
		good = TRUE;
	return(good);
}

int dbGoodContourCompositeRefFeatType(int ref_feature_type)
{
	int good = FALSE;
	if ( (ref_feature_type == CONTOUR) ||
		 (ref_feature_type == LINE) ||
		 (ref_feature_type == CIRCLE) ||
		 (ref_feature_type == PLANE) )
		good = TRUE;
	return(good);
}

// when changing this function, change the companion functions in fc_vectr.c
int dbCheckCompositeReferenceFeatures(Feature *this_feature,
									  int composite_feat_type)
{
	char *loc_func_name = "dbCheckCompositeReferenceFeatures";
	Feature *ref_feature = NULL;
	int ref_feature_num;
	int ref_feature_type;
	int i;
	int min_ref_features = 3;
	Feature *first_ref_feature = NULL;
	int first_ref_feature_num = 0;
	int first_ref_feature_type = 0;

	if (composite_feat_type == LINE)
		min_ref_features = 2;
	else if (composite_feat_type == CYLINDER)
		min_ref_features = 6;
	else if (composite_feat_type == CONE)
		min_ref_features = 6;
	else if (composite_feat_type == SPHERE)
		min_ref_features = 4;

	for (i = 1; i <= dbGetNumberPoints(this_feature); i++)
	{
		if ((ref_feature_num = dbGetRefFeatureNumber(this_feature,i)) <= 0)
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"reference feature number",i);
			return(GOT_ERROR);				   /* error in linked list access */
		}
		/* get the reference feature from the database						  */
		if ((ref_feature = dbTrustedGetFeatureFromDB(ref_feature_num)) == NULL)
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"reference feature",i);
			return(GOT_ERROR);
		}
		ref_feature_type = dbGetFeatureType(ref_feature);
		if (composite_feat_type == LINE)
		{
			if (dbGoodLineCompositeRefFeatType(ref_feature_type))
			{
				if (dbGetNumberPoints(ref_feature) >= 2)
				{
					min_ref_features = 1;
				}
			}
		}
		else if (composite_feat_type == CIRCLE)
		{
			if (dbGoodCircleCompositeRefFeatType(ref_feature_type))
			{
				if (dbGetNumberPoints(ref_feature) >= 3)
				{
					min_ref_features = 1;
				}
				else
				{
					min_ref_features = 2;
				}
			}
		}
		else if (composite_feat_type == PLANE)
		{
			if (dbGoodPlaneCompositeRefFeatType(ref_feature_type))
			{
				if (dbGetNumberPoints(ref_feature) >= 3)
				{
					min_ref_features = 1;
				}
				else
				{
					min_ref_features = 2;
				}
			}
		}
		else if (composite_feat_type == CYLINDER)
		{
			if (dbGoodCylinderCompositeRefFeatType(ref_feature_type))
			{
				if ( (ref_feature_type == CYLINDER) &&
					 (dbGetNumberPoints(ref_feature) >= 6) )
				{
					min_ref_features = 1;
				}
				else if ( (ref_feature_type == CIRCLE) &&
						  (dbGetNumberPoints(ref_feature) >= 3) )
				{
					min_ref_features = 2;
				}
			}
		}
		else if (composite_feat_type == CONE)
		{
			if (dbGoodConeCompositeRefFeatType(ref_feature_type))
			{
				if ( (ref_feature_type == CONE) &&
					 (dbGetNumberPoints(ref_feature) >= 6) )
				{
					min_ref_features = 1;
				}
				else if ( (ref_feature_type == CIRCLE) &&
						  (dbGetNumberPoints(ref_feature) >= 3) )
				{
					min_ref_features = 2;
				}
			}
		}
		else if (composite_feat_type == SPHERE)
		{
			if (dbGoodSphereCompositeRefFeatType(ref_feature_type))
			{
				if ( (ref_feature_type == SPHERE) &&
					 (dbGetNumberPoints(ref_feature) >= 4) )
				{
					min_ref_features = 1;
				}
				else if ( (ref_feature_type == CIRCLE) &&
						  (dbGetNumberPoints(ref_feature) >= 3) )
				{
					min_ref_features = 2;
				}
				else if (ref_feature_type == CONTOUR)
				{
					if (dbGetNumberPoints(ref_feature) >= 4)
					{
						min_ref_features = 1;
					}
					else if (dbGetNumberPoints(ref_feature) == 3)
					{
						min_ref_features = 2;
					}
					else if (dbGetNumberPoints(ref_feature) == 2)
					{
						min_ref_features = 3;
					}
					else
					{
						min_ref_features = 4;
					}
				}
			}
		}
		else if (composite_feat_type == CONTOUR)
		{
			if (dbGoodContourCompositeRefFeatType(ref_feature_type))
			{
				min_ref_features = 1;
			}
		}
		else if (composite_feat_type == FACE)
		{
			if ( (i == 1) &&
				 ( (ref_feature_type == PLANE) ||
				   (ref_feature_type == CYLINDER) ||
				   (ref_feature_type == CONE) ||
				   (ref_feature_type == SPHERE)) )
			{
				min_ref_features = 4;
			}
			else 
			{
				if (ref_feature_type == CIRCLE)
				{
					min_ref_features = 1;
				}
				else if (ref_feature_type == CONTOUR)
				{
					if (dbGetNumberPoints(ref_feature) >= 3)
					{
						min_ref_features = 1;
					}
					else
					{
						min_ref_features = 2;
					}
				}
				else if (ref_feature_type == LINE)
				{
					min_ref_features = 2;
				}
				// If the first feature was a primary feature, then its points don't count
				// so one extra feature will have to be picked.
				if ((first_ref_feature_num = dbGetRefFeatureNumber(this_feature,1)) <= 0)
				{
					errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"reference feature number",i);
					return(GOT_ERROR);				   /* error in linked list access */
				}
				/* get the first reference feature from the database						  */
				if ((first_ref_feature = dbTrustedGetFeatureFromDB(first_ref_feature_num)) == NULL)
				{
					errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"reference feature",i);
					return(GOT_ERROR);
				}
				first_ref_feature_type = dbGetFeatureType(first_ref_feature);
				if ( ( (first_ref_feature_type == PLANE) ||
					   (first_ref_feature_type == CYLINDER) ||
					   (first_ref_feature_type == CONE) ||
					   (first_ref_feature_type == SPHERE) ) &&
					   ( ( ref_feature_type != POINT) && ( ref_feature_type != INTERSECT) ) )
					min_ref_features++;
			}
		}
		else if (composite_feat_type == TRIM)
		{
			min_ref_features = 4;
			if (i != 1 )
			{
				if (ref_feature_type == CIRCLE)
				{
					min_ref_features = 2;
				}
				else if (ref_feature_type == CONTOUR)
				{
					if (dbGetNumberPoints(ref_feature) >= 3)
					{
						min_ref_features = 2;
					}
					else
					{
						min_ref_features = 3;
					}
				}
				else if (ref_feature_type == LINE)
				{
					min_ref_features = 3;
				}
			}
		}
	}
	return(min_ref_features);
}

int dbCheckCircleReferenceTypes(Feature *this_feature)
/* Author: Wei Wang									 Date: 08/18/1999    */
/* Purpose: Find out how many ref_features needed to construct a circle. */
/* Returns: minimum number of features.                                  */
/* should belong to Circle Feature in an Object-Oriented sense!          */
{
	Feature *ref_feature = NULL;
	int ref_feature_num;
	int ref_type, first_ref_type = 0;
	int i, n;
	int min_ref_features = 3;

	n = dbGetNumberPoints(this_feature);
	
	for (i = 1; i <= n; i++)
	{
		if ((ref_feature_num = dbGetRefFeatureNumber(this_feature,i)) <= 0)
		{
			break;				   /* error in linked list access */
		}
		/* get the reference feature from the database						  */
		if ((ref_feature = dbTrustedGetFeatureFromDB(ref_feature_num)) == NULL)
		{
			break;
		}
		ref_type = dbGetFeatureType(ref_feature);
		if (i == 1)
		{
			first_ref_type = ref_type;
			if ( (first_ref_type != SPHERE) && (first_ref_type != PLANE) )
				break;
		}
		else if ( ref_type != first_ref_type &&
				  ((ref_type == SPHERE) || (ref_type == PLANE)) )
		{
			min_ref_features = 2;
			break;
		}
	}
	return(min_ref_features);
}

// When changing this function, change its companion function meaEnoughPoints in mea_util.cpp
int dbEnoughFeaturePoints(Feature *this_feature, int number_of_points)
/* Author: Terry T. Cramer									 Date: 10/24/1994 */
/* Purpose: Find out if there are a enough points for a valid feature.		  */
/* Returns: 1 if minimum number of points, 0 if not.						  */
{
	int good = FALSE;					   /* set good flag to false initialy */
	int i;
	int side;
	int num_side1 = 0;
	int num_side2 = 0;
	int min_pts = 0;

	switch (dbGetFeatureType(this_feature))
	{
		case POINT:
		{
			if (dbIsFeatureMeasured(this_feature) == TRUE)
			{
				if (number_of_points == 1)
				{
					good = TRUE;				 /* only one point is allowed */
				}
			}
			else
			{
				if ( (number_of_points == 1) ||
					 (dbGetNominalsEntered(this_feature) == TRUE) )
				{
					good = TRUE;				  /* one or no points allowed */
				}
			}
			break;
		}	
		case ROTARY_FT:
		{
			if (dbIsFeatureMeasured(this_feature) == TRUE)
			{
				if (number_of_points == 1)
				{
					good = TRUE;				 /* only one point is allowed */
				}
			}
			else
			{
				if (number_of_points == 0)
				{
					good = TRUE;						 /* no points allowed */
				}
			}
			break;
		}	
		case ORIGIN:
		case AXIS:
		case PROFILE:
		case DATUM_PLANE:
		case CENTROID:
		case DATUM_RECALL: 
		{
			if (number_of_points == 1)
			{
				good = TRUE;					 /* only one point is allowed */
			}
			break;
		}	
		case MIDPOINT:
		case DISTANCE:
		case GAGE_DIA:
		case GAGE_BALL:
		{
			if (number_of_points == 2)
			{
				good = TRUE;			 /* must have two and only two points */
			}
			break;
		}	
		case FILLET:
		{
			if (number_of_points >= 3)
			{
				Feature *ref;
				ref = dbTrustedGetFeatureFromDB(dbGetRefFeatureNumber(this_feature, number_of_points));
				if (ref != NULL && dbGetFeatureType(ref) == LINE)
					good = TRUE;			 /* last feature must be line */
			}
			break;
		}	
		case WIDTH:
		case INTERSECT:
		{
			if (dbIsFeatureMeasured(this_feature) == TRUE)
			{
				for (i = 0; i < number_of_points; i++)
				{
					side = dbGetPointSideNum(this_feature,i+1);	/* get side # */
					if (side == 1)
					{
						num_side1++;
					}
					else
					{
						num_side2++;
					}
				}
				if ( (num_side1 >= 2) && (num_side2 >= 2) )
				{
					good = TRUE;  /* two per side or more points are required */
				}
			}
			else
			{
				if (number_of_points == 2)
				{
					good = TRUE;		 /* must have two and only two points */
				}
			}
			break;
		}
		case LINE:
		{
			min_pts = 2;				   /* two or more points are required */
			if (dbIsFeatureConstructed(this_feature))
			{		 /* allow 1 or 2 reference features for composite lines */
				if (dbGetMiscFlag(this_feature))
				{
					min_pts = dbCheckCompositeReferenceFeatures(this_feature, LINE);
				}
			}
			if (number_of_points >= min_pts)
			{
				good = TRUE;
			}
			break;
		}
		case MIN_MAX_AVG:
		{
			if (number_of_points >= 2)
			{
				good = TRUE;			   /* two or more points are required */
			}
			break;
		}
		case CIRCLE:
		{
			min_pts = 3;				 /* three or more points are required */
			if (dbIsFeatureConstructed(this_feature))
			{		 /* allow 1 or 2 reference features for composite circles */
				if (dbGetMiscFlag(this_feature))
				{
					min_pts = dbCheckCompositeReferenceFeatures(this_feature, CIRCLE);
				}
				else
					min_pts = dbCheckCircleReferenceTypes(this_feature);
			}
			if (number_of_points >= min_pts)
			{
				good = TRUE;			 /* three or more points are required */
			}
			break;
		}
		case FACE:
		{
			min_pts = dbCheckCompositeReferenceFeatures(this_feature, FACE);
			if (number_of_points >= min_pts)
			{
				good = TRUE;			 /* three or more points are required */
			}
			break;
		}
		case TRIM:
		{
			min_pts = dbCheckCompositeReferenceFeatures(this_feature, TRIM);
			if (number_of_points >= min_pts)
			{
				good = TRUE;			 /* three or more points are required */
			}
			break;
		}
		case PLANE:
		{
			min_pts = 3;				 /* three or more points are required */
			if (dbIsFeatureConstructed(this_feature))
			{		 /* allow 1 reference feature for composite planes */
				if (dbGetMiscFlag(this_feature))
				{
					min_pts = dbCheckCompositeReferenceFeatures(this_feature, PLANE);
				}
			}
			if (number_of_points >= min_pts)
			{
				good = TRUE;
			}
			break;
		}
		case SPHERE:
		{
			min_pts = 4;				  /* four or more points are required */
			if (dbIsFeatureConstructed(this_feature))
			{		 /* allow 1 or 2 reference features for composite spheres */
				if (dbGetMiscFlag(this_feature))
				{
					min_pts = dbCheckCompositeReferenceFeatures(this_feature, SPHERE);
				}
			}
			if (number_of_points >= min_pts)
			{
				good = TRUE;					/* must have 4 or more points */
			}
			break;
		}	
		case PT_PLANE:
		{
			if (number_of_points == 4)
			{
				good = TRUE;						   /* only 4 points valid */
			}
			break;
		}	
		case ANGULARITY:
		case PERPENDICULARITY:
		case PARALLELISM:
		{
			if (number_of_points == 2)
			{
				good = TRUE;						   /* only 2 points valid */
			}
			break;
		}	
		case CONTOUR:
		{
			min_pts = 2;				   /* two or more points are required */
			if (dbIsFeatureConstructed(this_feature))
			{		 /* allow 1 reference feature for composite contours */
				if (dbGetMiscFlag(this_feature))
				{
					min_pts = dbCheckCompositeReferenceFeatures(this_feature, CONTOUR);
				}
			}
			if (number_of_points >= min_pts)
			{
				good=TRUE;
			}
			break;
		}
		case CYLINDER:
		{
			min_pts = 6;				   /* six or more points are required */
			if (dbIsFeatureConstructed(this_feature))
			{		 /* allow 1 reference feature for composite cylinders */
				if (dbGetMiscFlag(this_feature))
				{
					min_pts = dbCheckCompositeReferenceFeatures(this_feature, CYLINDER);
				}
			}
			if (number_of_points >= min_pts)
			{
				good=TRUE;
			}
			break;
		}
		case CONE:
		{
			min_pts = 6;				   /* six or more points are required */
			if (dbIsFeatureConstructed(this_feature))
			{		 /* allow 1 reference feature for composite cones */
				if (dbGetMiscFlag(this_feature))
				{
					min_pts = dbCheckCompositeReferenceFeatures(this_feature, CONE);
				}
			}
			if (number_of_points >= min_pts)
			{
				good=TRUE;
			}
			break;
		}
		case DIGITAL_IO:
		case MATH:
		case USER_INPUT:
		case HOLD_FEATURE:
		{
			if (number_of_points == 0)
			{
				good = TRUE;						 	 /* no points allowed */
			}
			break;
		}	
		case BRANCH:
		{
			if ( (number_of_points == 0) ||
				 (number_of_points == 1) ||
				 (number_of_points == 2) )
			{	/* allow 0 points so that the DONE and AGAIN buttons display */
				good = TRUE;					 /* one or two points allowed */
			}
			break;
		}
		default:
			break;									 /* good is already false */
	}
	return(good);								   /* return good or not good */
}

int dbMinimumFeaturePoints(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/12/1991 */
/* Purpose: Find out if there are a enough points for a valid feature.		  */
/* Returns: 1 if minimum number of points, 0 if not.						  */
{
	int number_of_points;

	if ( (dbIsFeatureMeasured(this_feature)) &&	  /* feature measured with TP */
		 (dbGetFeatureType(this_feature) != ROTARY_FT) &&
		 (dbGetFeatureLensCode(this_feature) == LENS_CODE_TOUCHPROBE) )
	{
		number_of_points = dbGetNumberMeasuredPoints(this_feature);
	}
	else
	{
		number_of_points = dbGetNumberPoints(this_feature);
	}
	return(dbEnoughFeaturePoints(this_feature,number_of_points));
}

int dbGetNumberMissedPoints(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 09/13/1994 */
/* Purpose: Extract the number of missed points from the feature structure.	  */
/* Returns: The number of missed points.									  */
{
	return(this_feature->data.missed_points);  /* return number of missed pts */
}

void dbPutNumberMissedPoints(Feature *this_feature, int these_missed)
/* Author: Terry T. Cramer									 Date: 09/13/1994 */
/* Purpose: Set the number of missed points in the feature structure.		  */
/* Returns: Nothing.														  */
{
	this_feature->data.missed_points = these_missed;   /* set # of missed pts */
	return;
}

int dbGetNumberMeasuredPoints(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 04/01/1996 */
/* Purpose: Extract the number of measured points from the feature structure. */
/* Returns: The number of measured points.									  */
{
	return(this_feature->data.measured_points);	  /* return # of measured pts */
}

void dbPutNumberMeasuredPoints(Feature *this_feature, int these_measured)
/* Author: Terry T. Cramer									 Date: 04/01/1996 */
/* Purpose: Set the number of measured points in the feature structure.		  */
/* Returns: Nothing.														  */
{
	this_feature->data.measured_points = these_measured;/* set # measured pts */
	return;
}
