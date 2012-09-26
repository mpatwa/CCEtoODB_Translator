/*
$Workfile: Db_megap.c $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_megap.c 6     10/26/99 1:28p Ttc $

$Modtime: 10/26/99 1:27p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_megap.c $
 * 
 * 6     10/26/99 1:28p Ttc
 * Clear out the point direction vector when mallocing a new mega-point.
 * 
 * 5     9/28/99 7:52a Ttc
 * Add feature compensated point location.
 * 
 * 4     5/28/99 11:47a Ttc
 * Delete unused local variable.  Initialize local variable.
 * 
 * 3     5/25/99 9:58a Ttc
 * Delete QCHECK ifdef code and ifndefs.
 * 
 *    Rev 10.2   20 May 1998 15:34:28   ttc
 * Add init of extra data pointer.
 * 
 *    Rev 10.1   16 Apr 1998 16:33:52   ttc
 * Add last accessed mega point pointer.  Initialize it and use it to speed up
 * point list access.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:06   softadmn
 * Initial revision.
 * 
 *    Rev 9.3   28 Jun 1996 08:36:04   ttc
 * Add led flag.
 * 
 *    Rev 9.2   18 Jun 1996 13:24:44   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.1   18 May 1996 08:46:30   ttc
 * Add LED light.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:20   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:55:54   softadmn
 * Initial revision.
 * 
 *    Rev 8.1   27 Feb 1996 10:14:38   ttc
 * Add ifndefs for QCHECK.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:52   softadmn
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

Point_List *dbNewPointList(void)
/* Author: Terry T. Cramer									 Date: 02/06/1991 */
/* Purpose: Allocate and initialize	a new point structure.					  */
/* Returns: A pointer to the point structure or null.						  */
{
	char *loc_func_name = "dbNewPointList";
	Point_List *new_point;					/* pointer to the point structure */
	int i,j;

	/* allocate a new point structure */
	new_point = (Point_List *)malloc(sizeof(Point_List));
	if (new_point != NULL)					  /* if enough memory to allocate */
	{
		new_point->take_point.x = 0.0;		/* zero the take point x location */
		new_point->take_point.y = 0.0;		/* zero the take point y location */
		new_point->take_point.z = 0.0;		/* zero the take point z location */
		new_point->act_point.x = 0.0;	  /* zero the actual point x location */
		new_point->act_point.y = 0.0;	  /* zero the actual point y location */
		new_point->act_point.z = 0.0;	  /* zero the actual point z location */
		new_point->zoom = 0.0;						/* zero the zoom position */
		new_point->lights.backlight = 0;	 /* zero the back light intensity */
		new_point->lights.ringlight = 0;	 /* zero the ring light intensity */
		new_point->lights.auxlight = 0;	 /* zero the auxilary light intensity */
		for (i = 0; i < MAX_LED_SECTORS; i++)
		{
			for (j = 0; j < MAX_LED_RINGS; j++)
			{
				new_point->lights.led[i][j] = 0;			  /* zero the led */
			}
		}
		new_point->lights.led_flag = 0;					 /* zero the led flag */
		new_point->target.type = 0;					  /* zero the target type */
		new_point->target.color = 0;				 /* zero the target color */
		new_point->target.spacing1 = 0;			  /* zero the target spacings */
		new_point->target.spacing2 = 0;
		new_point->target.filename = NULL; /* set the target filename pointer */
		new_point->target.extra = NULL;	 /* set the extra target info pointer */
		new_point->edge_seek = 0;			  /* set the point seek edge flag */
		new_point->side_flag = 0;				  /* zero the point side flag */
		new_point->missed = 0;				   /* clear the point missed flag */
		new_point->comp_point.x = 0.0;	   // zero feature compensated point loc
		new_point->comp_point.y = 0.0;
		new_point->comp_point.z = 0.0;
		new_point->direction_vector.x = 0.0;  // zero the point direction vector
		new_point->direction_vector.y = 0.0;
		new_point->direction_vector.z = 0.0;
	}
	else
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
	}
	return(new_point);			 /* return a pointer to the new point or null */
}

Mega_Point *dbNewMegaPoint(int mega_point_number)
/* Author: Terry T. Cramer									 Date: 02/06/1991 */
/* Purpose: Create a new mega-point.										  */
/* Returns: A pointer to the new mega-point.								  */
{
	char *loc_func_name = "dbNewMegaPoint";
	Mega_Point *new_mega_point;					 /* pointer to new mega-point */

	/* allocate a new mega-point structure */
	new_mega_point = (Mega_Point *)malloc(sizeof(Mega_Point));
	if (new_mega_point != NULL)				  /* if enough memory to allocate */
	{
		new_mega_point->mega_point_number = mega_point_number;
		new_mega_point->point_ptr = NULL;	  /* initialize the point pointer */
		new_mega_point->feature_number = 0; /* zero the reference feat number */
		new_mega_point->next = NULL;	 /* clear the next mega-point pointer */
	}
	else
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
	}
	return(new_mega_point);	  /* return the pointer to the mega-point or null */
}

int dbAddMegaPoint(Feature *this_feature, int mega_point_number)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Add a point to the linked list of mega-points.					  */
/* Returns: NO_ERROR if successful, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbAddMegaPoint";
	Mega_Point *this_point = &this_feature->data.mega_points;  /* mega-pt ptr */
	Mega_Point *new_mega_point;				   /* pointer to a new mega-point */

	if (dbGetNumberPoints(this_feature) == 0)	  /* first point being added? */
	{
		if (dbIsFeatureMeasured(this_feature))			 /* measured feature? */
		{
			this_point->point_ptr = dbNewPointList();	/* add a point struct */
			if (this_point->point_ptr == NULL)				 /* if add failed */
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(GOT_ERROR);						 /* return error code */
			}
		}/* don't need to do anything for first reference feature being added */
	}
	else						   /* not the first point being added to list */
	{
		/* allocate a new mega-point */
		new_mega_point = dbNewMegaPoint(mega_point_number);
		if (new_mega_point == NULL)						/* if allocate failed */
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
			return(GOT_ERROR);							 /* return error code */
		}
		if (dbIsFeatureMeasured(this_feature))		   /* if measured feature */
		{
			new_mega_point->point_ptr = dbNewPointList(); /* add point struct */
			if (new_mega_point->point_ptr == NULL)			/* if that failed */
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(GOT_ERROR);						 /* return error code */
			}
		}
		while (this_point->next != NULL)	 /* while the next pointer is set */
			this_point = this_point->next;	  /* point to the next mega-point */
		this_point->next = new_mega_point; /* set the next to the new mega-pt */
		this_feature->data.last_mega_point_accessed = new_mega_point;
	}
	return(NO_ERROR);								  /* return no error code */
}

int dbDeleteMegaPoint(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Delete a point from the linked list.							  */
/* Returns: NO_ERROR.														  */
{
	Mega_Point *this_point = &this_feature->data.mega_points;  /* mega-pt ptr */
	Mega_Point *last_point = NULL;	/* ptr to mega-point just before last one */

	if (dbGetNumberPoints(this_feature) == 1)	   /* only 1 point to delete? */
	{
		if (this_point->point_ptr != NULL)	 /* if the point structure exists */
		{
			free(this_point->point_ptr);						   /* free it */
			this_point->point_ptr = NULL;				 /* clear the pointer */
		}
	}
	else									/* not just one point in the list */
	{
		while(this_point->next != NULL)  /* while the next pointer is not set */
		{
			last_point = this_point;		 /* save the next to last pointer */
			this_point = this_point->next;	  /* point to the next mega-point */
		}
		if (this_point->point_ptr != NULL)	 /* if the point structure exists */
		{
			free(this_point->point_ptr);						   /* free it */
		}
		this_feature->data.last_mega_point_accessed = NULL;
		free(this_point);							  /* free this mega-point */
		last_point->next = NULL;   /* clear the ptr to the mega-pt just freed */
	}
	return(NO_ERROR);								  /* return no error code */
}

Mega_Point *dbGetMegaPoint(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/06/1991 */
/* Purpose: Get a pointer to the point_number'th mega-point.				  */
/* Returns: A pointer to the specified mega-point, or NULL if not found.	  */
{
	char *loc_func_name = "dbGetMegaPoint";
	Mega_Point *this_point = &this_feature->data.mega_points;  /* mega-pt ptr */
	int point = 1;				   /* how many mega-points we have pointed to */

	if (this_feature->data.last_mega_point_accessed != NULL)
	{
		if (this_feature->data.last_mega_point_accessed->mega_point_number == point_number)
		{
			return(this_feature->data.last_mega_point_accessed);
		}
		if (this_feature->data.last_mega_point_accessed->mega_point_number < point_number)
		{
			this_point = this_feature->data.last_mega_point_accessed;
			point = this_feature->data.last_mega_point_accessed->mega_point_number;
		}
	}

	/* while still more mega-points to go and not at end of linked list 	  */
	while ((point < point_number) && (this_point->next != NULL))
	{
		this_point = this_point->next;		  /* point to the next mega-point */
		point++;					 /* increment number of mega-points found */
	}

	if (point == point_number)			 /* if we got to the one we specified */
	{
		this_feature->data.last_mega_point_accessed = this_point;
		return(this_point);					/* return this mega-point pointer */
	}
	else		/* got to end of list before finding point_number mega-points */
	{
		this_feature->data.last_mega_point_accessed = NULL;
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"mega point pointer",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}

Point_List *dbGetPointListPtr(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 02/06/1991 */
/* Purpose: Get a pointer to the point_number'th point structure.			  */
/* Returns: A pointer to the specified point structure or NULL if not found.  */
{
	char *loc_func_name = "dbGetPointListPtr";
	Mega_Point *this_mega_point;	   /* pointer to the specified mega-point */

	/* find the point_number'th mega-point in the linked list 				  */
	this_mega_point = dbGetMegaPoint(this_feature,point_number);
	if (this_mega_point != NULL)							 /* was it found? */
	{
		return(this_mega_point->point_ptr);/* return the point struct pointer */
	}
	else							   /* nope, didn't find the one we wanted */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point list pointer",point_number);
		return(NULL);								 /* return a NULL pointer */
	}
}
