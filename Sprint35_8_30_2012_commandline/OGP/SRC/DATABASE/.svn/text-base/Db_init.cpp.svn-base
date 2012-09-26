/*
$Workfile: Db_init.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_init.cpp 19    3/26/01 8:32a Jpk $

$Modtime: 3/22/01 2:00p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_init.cpp $
 * 
 * 19    3/26/01 8:32a Jpk
 * Changed name of new laser parameter from dock to deploy.
 * 
 * 18    3/22/01 8:25a Jpk
 * Added new flag for deployable laser to the flags structure.
 * 
 * 17    12/07/00 11:48a Ttc
 * Forgot to initialize the modified position tolerance value.
 * 
 * 16    8/14/00 8:03a Jpk
 * Added extended data pointer type value to eliminate database errors and
 * expand the usefullness of the pointer.
 * 
 * 15    8/08/00 9:39a Jpk
 * Initialize the laser feature data.
 * 
 * 14    1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 13    11/11/99 10:26a Ttc
 * Add direction vector to the smear structure also.  No bump in db
 * version number, so any rtn stored between 10/29/99 and today (11/11/99)
 * is trash.
 * 
 * 12    7/06/99 3:10p Ttc
 * SCR 6627.  Add the z location for the start, middle and end weak edge /
 * edge trace points.  Weak edge and edge trace need the full 3D values
 * for all points so that they work after a plane align.
 * 
 * 11    6/04/99 9:33a Ttc
 * DN 9723 version 4. Forgot the init of the nominal 3 and tolerances.
 * 
 * 10    5/28/99 11:46a Ttc
 * Delete unused local variables.
 * 
 * 9     5/26/99 11:04a Ttc
 * Change Auxilary size and tolerance function names and internal variable
 * names to nominal size2, and upper and lower size2 tols in preparation
 * to add a nominal size 3 with tolerances for cone.
 * 
 * 8     5/25/99 9:58a Ttc
 * Delete QCHECK ifdef code and ifndefs.
 * 
 * 7     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 6     4/19/99 10:13a Ttc
 * SCR 6379. Free data that wasn't.
 * 
 * 5     4/13/99 11:14a Ttc
 * Add angle orientations.
 * 
 * 4     3/17/99 9:05a Ttc
 * Add direction to results.
 * 
 *    Rev 10.4   24 Jul 1998 08:22:56   ttc
 * Add init of feature part transforms.
 * 
 *    Rev 10.3   20 May 1998 15:34:04   ttc
 * Add free of extra data pointer.
 * 
 *    Rev 10.2   16 Apr 1998 16:32:16   ttc
 * Initialize the base mega point number and the last accessed mega point ptr.
 * Rewrite free all megapoints so it doesn't use recursion and cause a stack
 * fault.
 * 
 *    Rev 10.1   14 Apr 1998 12:02:06   ttc
 * Add extra dimension for edge trace.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:00   softadmn
 * Initial revision.
 * 
 *    Rev 9.7   25 Aug 1997 09:15:36   ttc
 * Add tp200 dock station.
 * 
 *    Rev 9.6   18 Apr 1997 16:02:16   dmf
 * Added SmartReport
 * 
 *    Rev 9.5   28 Jun 1996 08:36:02   ttc
 * Add led flag.
 * 
 *    Rev 9.4   18 Jun 1996 13:24:40   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.3   20 May 1996 14:07:20   ttc
 * Add smear led lights.
 * 
 *    Rev 9.2   08 Apr 1996 16:48:52   ttc
 * Add init of lens code and number measured points.c
 * 
 *    Rev 9.1   18 Mar 1996 13:03:50   ttc
 * Add miscellaneous flag.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:08   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:55:46   softadmn
 * Initial revision.
 * 
 *    Rev 8.3   27 Feb 1996 10:14:36   ttc
 * Add ifndefs for QCHECK.
 * 
 *    Rev 8.2   01 Aug 1995 13:41:30   dmf
 * Some stuff for the future: has no effect on rev 8
 * 
 *    Rev 8.1   27 Feb 1995 15:36:56   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:48   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stddef.h>
#include <stdlib.h>

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

void dbInitNewFeatData(Feature_Data *feature)
/* Author: Terry T. Cramer									 Date: 01/29/1991 */
/* Purpose: Initialize a newly created feature structure.					  */
/* Returns: Nothing.														  */
{
	int i,j;

	/* initialize all numerics to zero, all pointers to NULL				  */
	feature->number = 0;									/* feature number */
	feature->type = NO_FEATURE;								  /* feature type */
	feature->plane = NO_PLANE;						/* planes to calculate in */
	feature->lens_code = LENS_CODE_SMALL_NONE;		/* code of lens installed */
	feature->dock_location = 0;								 /* dock location */
	feature->number_points = 0;				   /* number of points in feature */
	feature->missed_points = 0;			/* number of missed points in feature */
	feature->measured_points = 0;	  /* number of measured points in feature */
	feature->mega_points.mega_point_number = 1;
	feature->mega_points.point_ptr = NULL;		/* pointer to point structure */
	feature->mega_points.feature_number = 0;	  /* reference feature number */
	feature->mega_points.next = NULL;			/* pointer to next mega-point */
	feature->last_mega_point_accessed = NULL;
	feature->last_display = 0;				   /* last feature size displayed */
	feature->last_display2 = 0;
	feature->last_display3 = 0;
	feature->basic_dim = 0.0;						  /* basic dimension size */
	feature->extra_dim = 0.0;						  /* extra dimension size */
	feature->flags.in_mm = 0;						/* inch / millimeter flag */
	feature->flags.cart_polar = 0;					/* cartesian / polar flag */
	feature->flags.meas_const = 0;				  /* measure / construct flag */
	feature->flags.stats = 0;				   /* items to send to stats flag */
	feature->flags.print = 0;						   /* items to print flag */
	feature->flags.report = 0;						  /* items to report flag */
	feature->flags.export = 0;						  /* items to export flag */
	feature->flags.dec_dms = 0;							/* decimal / dms flag */
	feature->flags.data_stream = 0;
	feature->flags.miscellaneous = 0;
	feature->flags.failed = 0;				 /* clear the feature failed flag */
	feature->flags.deploy_laser = 0; /* Deploy or retract the deployable laser */
	feature->smear_data.smr_type = 0;
	feature->smear_data.smr_screen_center.x = 0.0;
	feature->smear_data.smr_screen_center.y = 0.0;
	feature->smear_data.smr_screen_center.z = 0.0;
	feature->smear_data.smr_zoom_pos = 0.0;
	feature->smear_data.lights.backlight = 0;
	feature->smear_data.lights.ringlight = 0;
	feature->smear_data.lights.auxlight = 0;
	for (i = 0; i < MAX_LED_SECTORS; i++)
	{
		for (j = 0; j < MAX_LED_RINGS; j++)
		{
			feature->smear_data.lights.led[i][j] = 0;
		}
	}
	feature->smear_data.lights.led_flag = 0;
	feature->smear_data.smr_start.x = 0.0;
	feature->smear_data.smr_start.y = 0.0;
	feature->smear_data.smr_start.z = 0.0;
	feature->smear_data.smr_end.x = 0.0;
	feature->smear_data.smr_end.y = 0.0;
	feature->smear_data.smr_end.z = 0.0;
	feature->smear_data.smr_mid.x = 0.0;
	feature->smear_data.smr_mid.y = 0.0;
	feature->smear_data.smr_mid.z = 0.0;
	feature->smear_data.smr_vector.x = 0.0;
	feature->smear_data.smr_vector.y = 0.0;
	feature->smear_data.smr_vector.z = 0.0;
	feature->smear_data.smr_direction = 0;
	feature->smear_data.smr_bounds = 0;
	feature->smear_data.smr_extent = 0;
	feature->smear_data.smr_min_quality = 0;
	feature->smear_data.smr_constrast = 0;
	feature->smear_data.smr_wgt_nominal = 0;
	feature->smear_data.smr_wgt_constrast = 0;
	feature->smear_data.smr_wgt_first = 0;
	feature->smear_data.smr_wgt_last = 0;
	feature->smear_data.smr_wgt_second = 0;
	feature->smear_data.smr_wgt_second_last = 0;
	feature->smear_data.smr_std_dev = 0;
	feature->comment = NULL;			 /* pointer to feature comment string */
	feature->prompt = NULL;				  /* pointer to feature prompt string */
	feature->expression = NULL;		  /* pointer to feature expression string */
	feature->extended_data_type = EXTENDED_DATA_NONE;
	feature->extended_data = NULL;
	for(i = 0; i < MAX_FLAG_OPTIONS; i++)
	{
		feature->label[i]=NULL;
		feature->fieldname[i]=NULL;
	}
	return;
}

void dbFreeAllMegaPoints(Mega_Point *this_point)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Free all memory allocated in the mega-point linked list.		  */
/* Returns: Nothing.														  */
{
	Mega_Point *free_point = NULL;
	do
	{
		if (this_point->point_ptr != NULL)
		{
			if (this_point->point_ptr->target.extra != NULL)
			{
				free(this_point->point_ptr->target.extra);
			}
			free(this_point->point_ptr);
		}
		free_point = this_point;
		this_point = this_point->next;
		free(free_point);
	} while (this_point != NULL);
	return;
}

void dbInitFeatData(Feature_Data *feature)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Initialize a feature thats been used before.					  */
/* Returns: Nothing.														  */
{
	int i;

	/* if a point is attached, free the point structure */
	if (feature->mega_points.point_ptr != NULL)
	{
		if (feature->mega_points.point_ptr->target.extra != NULL)
		{
			free(feature->mega_points.point_ptr->target.extra);
		}
		free(feature->mega_points.point_ptr);
	}
	if (feature->mega_points.next != NULL)	 /* a next mega-point is attached */
	{
		dbFreeAllMegaPoints(feature->mega_points.next);	  /* free all of them */
	}
	/* if a comment is hanging off the structure, free the character vector */
	if (feature->comment != NULL)
	{
		free(feature->comment);
	}
	/* if a prompt is hanging off the structure, free the character vector */
	if (feature->prompt != NULL)
	{
		free(feature->prompt);
	}
	for (i = 0; i < MAX_FLAG_OPTIONS; i++)
	{
		if (feature->label[i] != NULL)
		{
			free(feature->label[i]);
		}
		if (feature->fieldname[i] != NULL)
		{
			free(feature->fieldname[i]);
		}
	}
	/* if a expression is hanging off the structure, free that too */
	if (feature->expression != NULL)
	{
		free(feature->expression);
	}

	/* if there's extended info hanging off get rid of it */
	if (feature->extended_data != NULL)
	{
		free(feature->extended_data);
	}

	dbInitNewFeatData(feature);	  /* initialize all variables to zero or null */
	return;
}

void dbInitFeatResult(Feature_Result *result)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Initialize a feature's result structure.						  */
/* Returns: Nothing.														  */
{
	int i;/* loop control variable */

	for (i = 0; i < MAX_LOCATIONS; i++)					 /* for each location */
	{
		result->actual_location[i].x = 0.0;	   /* zero the x, y, and z values */
		result->actual_location[i].y = 0.0;
		result->actual_location[i].z = 0.0;
	}
	result->num_locs = 0;			   /* zero the number of locations stored */
	for (i = 0; i < MAX_SIZES; i++)							 /* for each size */
		result->actual_size[i] = 0.0;						 /* zero the size */
	result->num_sizes = 0;				   /* zero the number of sizes stored */
	result->direction.x = 0.0;					 /* zero the direction vector */
	result->direction.y = 0.0;
	result->direction.z = +1.0;
	for (i = 0; i < MAX_ANGLES; i++)
		result->orientation[i] = 0.0;		   /* zero the orientation angles */
	result->form_deviation = 0.0;				   /* zero the form deviation */
	result->position_tol = 0.0;
	result->modified_tol = 0.0;

	return;
}

void dbInitFeatNoms(Feature_Nominals *nominals)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Initialize the feature nominals structure.						  */
/* Returns: Nothing.														  */
{
	int i;

	nominals->nominals_entered = 0;		   /* clear the nominals entered flag */
	nominals->tolerance_type = 0;			  /* zero the tolerance type flag */
	nominals->nom_loc.x = 0.0;				   /* zero the nominal x location */
	nominals->nom_loc.y = 0.0;				   /* zero the nominal y location */
	nominals->nom_loc.z = 0.0;				   /* zero the nominal z location */
	nominals->upper_loc_tol.x = 0.0;   /* zero the upper location x tolerance */
	nominals->upper_loc_tol.y = 0.0;   /* zero the lower location x tolerance */
	nominals->upper_loc_tol.z = 0.0;   /* zero the upper location y tolerance */
	nominals->lower_loc_tol.x = 0.0;   /* zero the lower location y tolerance */
	nominals->lower_loc_tol.y = 0.0;   /* zero the upper location z tolerance */
	nominals->lower_loc_tol.z = 0.0;   /* zero the lower location z tolerance */
	nominals->nom_size = 0.0;				 /* zero the nominal feature size */
	nominals->upper_size_tol = 0.0;		  /* zero the upper feature tolerance */
	nominals->lower_size_tol = 0.0;		  /* zero the lower feature tolerance */
	nominals->nom_size2 = 0.0;		  /* zero the second nominal feature size */
	nominals->upper_size2_tol = 0.0;  /* zero the upper 2nd feature tolerance */
	nominals->lower_size2_tol = 0.0;  /* zero the lower 2nd feature tolerance */
	nominals->elev_angle = 0.0;
	nominals->upper_elev_tol = 0.0;
	nominals->lower_elev_tol = 0.0;
	nominals->nom_size3 = 0.0;		   /* zero the third nominal feature size */
	nominals->upper_size3_tol = 0.0;  /* zero the upper 3rd feature tolerance */
	nominals->lower_size3_tol = 0.0;  /* zero the lower 3rd feature tolerance */
	nominals->form_tol = 0.0;			   /* zero the nominal form tolerance */
	nominals->position_tol = 0.0;	   /* zero the nominal position tolerance */
	nominals->nom_flag1 = 0;					 /* zero the nominal flag one */
	nominals->nom_flag2 = 0;					 /* zero the nominal flag two */
	for (i = 0; i < 3; i++)
	{
		nominals->search_on_steps[i] = 0;
	}
	return;
}

void dbInitFeatTransform(Database_Transform *part_transform)
{
	part_transform->transform.d.x = 0.0;
	part_transform->transform.d.y = 0.0;
	part_transform->transform.d.z = 0.0;
	part_transform->transform.m.a.x = 1.0;
	part_transform->transform.m.a.y = 0.0;
	part_transform->transform.m.a.z = 0.0;
	part_transform->transform.m.b.x = 0.0;
	part_transform->transform.m.b.y = 1.0;
	part_transform->transform.m.b.z = 0.0;
	part_transform->transform.m.c.x = 0.0;
	part_transform->transform.m.c.y = 0.0;
	part_transform->transform.m.c.z = 1.0;
	part_transform->nominal_part_axis = 0.0;
	part_transform->actual_part_axis = 0.0;
	return;
}

void dbInitFeature(Feature *feature)
/* Author: Terry T. Cramer									 Date: 01/25/1991 */
/* Purpose: Initialize the feature structure.								  */
/* Returns: Nothing.														  */
{
	dbInitFeatData(&feature->data);	 /* initialize the feature data structure */
	dbInitFeatResult(&feature->result);	   /* initialize the result structure */
	dbInitFeatNoms(&feature->nominals);	 /* initialize the nominals structure */
	dbInitFeatTransform(&feature->part_transform);
	feature->next_feature = NULL;		   /* pointer to next feature in list */
	feature->prev_feature = NULL;	   /* pointer to previous feature in list */
	return;
}

void dbInitNewFeature(Feature *feature)
/* Author: Terry T. Cramer									 Date: 01/29/1991 */
/* Purpose: Initialize a new feature structure (just allocated, never used).  */
/* Returns: Nothing.														  */
{
	dbInitNewFeatData(&feature->data);/* initialize the new feature structure */
	dbInitFeatResult(&feature->result);	   /* initialize the result structure */
	dbInitFeatNoms(&feature->nominals);	 /* initialize the nominals structure */
	dbInitFeatTransform(&feature->part_transform);
	feature->next_feature = NULL;		   /* pointer to next feature in list */
	feature->prev_feature = NULL;	   /* pointer to previous feature in list */
	return;
}

void dbInitFeatureResult(Feature *feature)
{
	dbInitFeatResult(&feature->result);	   /* initialize the result structure */
	return;
}

Feature *dbNewFeature(void)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Allocate and initialize a new feature structure.				  */
/* Returns: A pointer to the new feature structure.							  */
{
	Feature *new_feature;				  /* pointer to new feature structure */

	/* allocate a new feature structure */
	new_feature = (Feature *)malloc(sizeof(Feature));
	if (new_feature != NULL)			   /* if enough memory to allocate it */
		dbInitNewFeature(new_feature);/* initialize the new feature structure */
	return(new_feature);/* return the new feature structure pointer (or null) */
}

void dbDisposeFeature(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/07/1991 */
/* Purpose: Free of a feature structure and all allocate space in lists.	  */
/* Returns: Nothing.														  */
{
	dbInitFeature(this_feature);		  /* initialize the feature structure */
	free(this_feature);									/* free the structure */
	return;
}
