/*
$Workfile: Db_flags.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_flags.cpp 7     3/26/01 8:32a Jpk $

$Modtime: 3/23/01 10:29a $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_flags.cpp $
 * 
 * 7     3/26/01 8:32a Jpk
 * Changed name of new laser parameter from dock to deploy.
 * 
 * 6     3/22/01 8:25a Jpk
 * Added new flag for deployable laser to the flags structure.
 * 
 * 5     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 4     6/03/99 3:04p Ttc
 * DN 9723 version 4. Allow FLAG_TP for report and export.
 * 
 * 3     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:12   softadmn
 * Initial revision.
 * 
 *    Rev 9.2   18 Apr 1997 16:02:14   dmf
 * Added SmartReport
 * 
 *    Rev 9.1   18 Mar 1996 13:03:50   ttc
 * Add miscellaneous flag.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:36   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:04   softadmn
 * Initial revision.
 * 
 *    Rev 8.2   01 Aug 1995 15:34:42   ttc
 * Delete return value in dbPutDataStreamFlag.
 * 
 *    Rev 8.1   01 Aug 1995 13:42:24   dmf
 * Stuff for the future: no effect on rev 8
 * 
 *    Rev 8.0   18 Jan 1995 15:25:46   softadmn
 * Initial revision.
*/

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


int dbPutInMmFlag(Feature *this_feature,int in_mm_flag)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Put the inch / millimeter flag in the feature structure.		  */
/* Returns: NO_ERROR if good inch / millimeter flag, GOT_ERROR otherwise.	  */
{
	char *loc_func_name = "dbPutInMmFlag";

	if ((in_mm_flag == INCH) || (in_mm_flag == MM))	  /* is this a good flag? */
	{
		this_feature->data.flags.in_mm = in_mm_flag;  /* set the feature flag */
		return(NO_ERROR);							  /* return no error code */
	}
	else										/* bad inch / millimeter flag */
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"inch / mm flag",in_mm_flag);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetInMmFlag(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/01/1991 */
/* Purpose: Get the inch / millmeter flag from the feature structure.		  */
/* Returns: The inch / millimeter flag.										  */
{
	return(this_feature->data.flags.in_mm);	 /* return the feature in/mm flag */
}

int dbPutCartPolarFlag(Feature *this_feature,int cart_polar_flag)
/* Author: Terry T. Cramer									 Date: 02/13/1991 */
/* Purpose: Set the cartesian / polar flag in the feature structure.		  */
/* Returns: NO_ERROR if good value, GOT_ERROR otherwise.					  */
{
	char *loc_func_name = "dbPutCartPolarFlag";

	if ((cart_polar_flag == CART) || (cart_polar_flag == POLAR))   /* valid ? */
	{
		/* set the feature's cartesian / polar flag 						  */
		this_feature->data.flags.cart_polar = cart_polar_flag;
		return(NO_ERROR);							  /* return no error code */
	}
	else						   /* not a good cartesian / polar flag value */
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"cart / polar flag",cart_polar_flag);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetCartPolarFlag(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 02/01/1991 */
/* Purpose: Get the cartesian / polar flag from the feature structure.		  */
/* Returns: The cartesian / polar flag.										  */
{
	return(this_feature->data.flags.cart_polar);/* return the cart/polar flag */
}

int dbPutStatisticsFlags(Feature *this_feature, int stats_flags)
{
	char *loc_func_name = "dbPutStatisticsFlags";

	if (stats_flags <= (FLAG_X | FLAG_Y | FLAG_Z | FLAG_FEAT | FLAG_OTHER | FLAG_ANOTHER | FLAG_ELEV))
	{
		this_feature->data.flags.stats = stats_flags;
		return(NO_ERROR);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"statistics flags",stats_flags);
		return(GOT_ERROR);
	}
}

int dbGetStatisticsFlags(Feature *this_feature)
{
	return(this_feature->data.flags.stats);
}

int dbPutPrintFlags(Feature *this_feature, int print_flags)
{
	char *loc_func_name = "dbPutPrintFlags";

	if (print_flags <= (FLAG_X | FLAG_Y | FLAG_Z | FLAG_FEAT | FLAG_OTHER | FLAG_ANOTHER | FLAG_ELEV))
	{
		this_feature->data.flags.print = print_flags;
		return(NO_ERROR);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"print flags",print_flags);
		return(GOT_ERROR);
	}
}

int dbGetPrintFlags(Feature *this_feature)
{
	return(this_feature->data.flags.print);
}

int dbPutReportFlags(Feature *this_feature, int report_flags)
{
	char *loc_func_name = "dbPutReportFlags";

	if (report_flags <= (FLAG_X | FLAG_Y | FLAG_Z | FLAG_FEAT | FLAG_OTHER | FLAG_ANOTHER | FLAG_ELEV | FLAG_TP))
	{
		this_feature->data.flags.report = report_flags;
		return(NO_ERROR);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"report flags",report_flags);
		return(GOT_ERROR);
	}
}

int dbGetReportFlags(Feature *this_feature)
{
	return(this_feature->data.flags.report);
}

int dbPutExportFlags(Feature *this_feature, int export_flags)
{
	char *loc_func_name = "dbPutExportFlags";

	if (export_flags <= (FLAG_X | FLAG_Y | FLAG_Z | FLAG_FEAT | FLAG_OTHER | FLAG_ANOTHER | FLAG_ELEV | FLAG_TP))
	{
		this_feature->data.flags.export = export_flags;
		return(NO_ERROR);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"export flags",export_flags);
		return(GOT_ERROR);
	}
}

int dbGetExportFlags(Feature *this_feature)
{
	return(this_feature->data.flags.export);
}

int dbPutDecDmsFlag(Feature *this_feature,int angle_flag)
/* Author: Terry T. Cramer									 Date: 09/02/1993 */
/* Purpose: Put the decimal / d:m:s flag in the feature structure.			  */
/* Returns: NO_ERROR if good decimal / d:m:s flag, GOT_ERROR otherwise.		  */
{
	char *loc_func_name = "dbPutDecDmsFlag";

	/* is this a good flag? */
	if ((angle_flag == DECIMAL_ANGLE) || (angle_flag == DEG_MIN_SEC))
	{
		this_feature->data.flags.dec_dms = angle_flag;		  /* set the flag */
		return(NO_ERROR);							  /* return no error code */
	}
	else										/* bad inch / millimeter flag */
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"decimal / dms flag",angle_flag);
		return(GOT_ERROR);								 /* return error code */
	}
}

int dbGetDecDmsFlag(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 09/02/1993 */
/* Purpose: Get the decimal / d:m:s flag from the feature structure.		  */
/* Returns: The decimal / d:m:s flag.										  */
{
	return(this_feature->data.flags.dec_dms);	   /* return the dec/dms flag */
}

void dbSetFeatureFailed(Feature *this_feature, int failed_flag)
{
	this_feature->data.flags.failed = failed_flag;
	return;
}

int dbGetFeatureFailed(Feature *this_feature)
{
	return(this_feature->data.flags.failed);
}

void dbPutDataStreamFlag(Feature *this_feature, int on_or_off)
{
	this_feature->data.flags.data_stream = on_or_off;
	return;
}

int dbGetDataStreamFlag(Feature *this_feature)
{
	return(this_feature->data.flags.data_stream);
}

void dbPutMiscFlag(Feature *this_feature, int misc_flag)
{
	this_feature->data.flags.miscellaneous = misc_flag;
	return;
}

int dbGetMiscFlag(Feature *this_feature)
{
	return(this_feature->data.flags.miscellaneous);
}

int dbPutFeatureDeployLaserFlag(Feature *this_feature, int dock_laser)
{
	char *loc_func_name = "dbPutFeatureDockLaserFlag";

	if ((dock_laser == TRUE) || (dock_laser == FALSE))	  
	{
		this_feature->data.flags.deploy_laser = dock_laser;  
		return(NO_ERROR);							  
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"dock laser flag",dock_laser);
		return(GOT_ERROR);								 
	}
}

int dbGetFeatureDeployLaserFlag(Feature *this_feature)
{
	return(this_feature->data.flags.deploy_laser);	 
}
