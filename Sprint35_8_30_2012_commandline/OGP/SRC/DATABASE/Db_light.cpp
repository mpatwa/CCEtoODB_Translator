/*
$Workfile: Db_light.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/Db_light.c 4     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/Db_light.c $
 * 
 * 4     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 3     5/28/99 11:45a Ttc
 * Move prototypes outside functions.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:10   softadmn
 * Initial revision.
 * 
 *    Rev 9.3   28 Jun 1996 08:36:02   ttc
 * Add led flag.
 * 
 *    Rev 9.2   18 Jun 1996 13:24:42   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.1   18 May 1996 08:46:28   ttc
 * Add LED light.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:28   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:00   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:50   softadmn
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

Point_Lights *dbGetPointLights(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 04/16/1991 */
/* Purpose: Find the specified light structure in the linked list of points.  */
/* Returns: A pointer to the specified point_number light structure.		  */
{
	char *loc_func_name = "dbGetPointLights";

	Point_List *this_point_ptr;				  /* pointer to a point structure */

	/* get the pointer to the specified point's point structure				  */
	this_point_ptr = dbGetPointListPtr(this_feature,point_number);
	if (this_point_ptr != NULL)					  /* found the correct point? */
	{
		return(&this_point_ptr->lights);    /* return pointer to light struct */
	}
	else				 /* couldn't find the specified point_number'th point */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(NULL);								   /* return NULL pointer */
	}
}

int dbPutPointBackLight(Feature *this_feature, int point_number, int new_light)
/* Author: Terry T. Cramer									 Date: 04/16/1991 */
/* Purpose: Store a back light intensity for a specified point.				  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointBackLight";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)					/* was the specified point found? */
	{
		this_light->backlight = new_light;			  /* store the back light */
		return(NO_ERROR);							  /* return no error code */
	}
	else
	{
		/* couldn't find the specified point								  */
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointBackLight(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 04/16/1991 */
/* Purpose: Extract the back light for a specified point from the feature.	  */
/* Returns: The back light intensity.										  */
{
	char *loc_func_name = "dbGetPointBackLight";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)						  /* found the correct point? */
	{
		return(this_light->backlight);		/* return this point's back light */
	}
	else						  /* couldn't find the specified point lights */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbPutPointRingLight(Feature *this_feature, int point_number, int new_light)
/* Author: Terry T. Cramer									 Date: 04/16/1991 */
/* Purpose: Store a ring light intensity for a specified point.				  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointRingLight";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)					/* was the specified point found? */
	{
		this_light->ringlight = new_light;			  /* store the ring light */
		return(NO_ERROR);							  /* return no error code */
	}
	else
	{
		/* couldn't find the specified point								  */
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointRingLight(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 04/16/1991 */
/* Purpose: Extract the ring light for a specified point from the feature.	  */
/* Returns: The ring light intensity.										  */
{
	char *loc_func_name = "dbGetPointRingLight";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)						  /* found the correct point? */
	{
		return(this_light->ringlight);		/* return this point's ring light */
	}
	else						  /* couldn't find the specified point lights */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbPutPointAuxLight(Feature *this_feature, int point_number, int new_light)
/* Author: Terry T. Cramer									 Date: 04/16/1991 */
/* Purpose: Store a aux light intensity for a specified point.				  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointAuxLight";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)					/* was the specified point found? */
	{
		this_light->auxlight = new_light;			   /* store the aux light */
		return(NO_ERROR);							  /* return no error code */
	}
	else
	{
		/* couldn't find the specified point								  */
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointAuxLight(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 04/16/1991 */
/* Purpose: Extract the aux light for a specified point from the feature.	  */
/* Returns: The aux light intensity.										  */
{
	char *loc_func_name = "dbGetPointAuxLight";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)						  /* found the correct point? */
	{
		return(this_light->auxlight);		 /* return this point's aux light */
	}
	else						  /* couldn't find the specified point lights */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbPutPointLedIntensity(Feature *this_feature, int point_number,
						   int sector, int ring, int new_intensity)
/* Author: Terry T. Cramer									 Date: 05/17/1996 */
/* Purpose: Store a led light intensity for a specified point.				  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointLedIntensity";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	if ( (sector < 1) || (sector > MAX_LED_SECTORS) )
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"sector",sector);
		return(GOT_ERROR);
	}
	if ( (ring < 1) || (ring > MAX_LED_RINGS) )
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"ring",ring);
		return(GOT_ERROR);
	}
	if ( (new_intensity < 0) || (new_intensity > MAX_LIGHT) )
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"led light sector intensity", new_intensity);
		return(GOT_ERROR);
	}

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)					/* was the specified point found? */
	{	/* store the led sector light intensity */
		this_light->led[sector - 1][ring - 1] = new_intensity;
		return(NO_ERROR);							  /* return no error code */
	}
	else
	{
		/* couldn't find the specified point								  */
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointLedIntensity(Feature *this_feature, int point_number,
								 int sector, int ring)
/* Author: Terry T. Cramer									 Date: 05/17/1996 */
/* Purpose: Extract the led light intensity from a specified point.			  */
/* Returns: The led light sector intensity.									  */
{
	char *loc_func_name = "dbGetPointLedIntensity";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	if ( (sector < 1) || (sector > MAX_LED_SECTORS) )
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"sector",sector);
		return(GOT_ERROR);
	}
	if ( (ring < 1) || (ring > MAX_LED_RINGS) )
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"ring",ring);
		return(GOT_ERROR);
	}

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)						  /* found the correct point? */
	{	/* return this point's led sector light intensity */
		return(this_light->led[sector - 1][ring - 1]);
	}
	else						  /* couldn't find the specified point lights */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbPutPointLedFlag(Feature *this_feature, int point_number, int flag)
/* Author: Terry T. Cramer									 Date: 06/26/1996 */
/* Purpose: Store a led light flag for a specified point.					  */
/* Returns: NO_ERROR if stored ok, GOT_ERROR if not.						  */
{
	char *loc_func_name = "dbPutPointLedFlag";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)					/* was the specified point found? */
	{	/* store the led sector light intensity */
		this_light->led_flag = flag;
		return(NO_ERROR);							  /* return no error code */
	}
	else
	{
		/* couldn't find the specified point								  */
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetPointLedFlag(Feature *this_feature, int point_number)
/* Author: Terry T. Cramer									 Date: 06/26/1996 */
/* Purpose: Extract the led light flag from a specified point.				  */
/* Returns: The led light sector flag.										  */
{
	char *loc_func_name = "dbGetPointLedFlag";
	Point_Lights *this_light;				 /* pointer to a lights structure */

	/* get a pointer to the specified point's lights structure				  */
	this_light = dbGetPointLights(this_feature, point_number);
	if (this_light != NULL)						  /* found the correct point? */
	{	/* return this point's led sector light intensity */
		return(this_light->led_flag);
	}
	else						  /* couldn't find the specified point lights */
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"point pointer",point_number);
		return(GOT_ERROR);								 /* return error code */
	}
}
