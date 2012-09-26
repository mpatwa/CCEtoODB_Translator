/*
$Workfile: Db_focus.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_focus.cpp 5     7/13/01 4:35p Ttc $

$Modtime: 7/13/01 4:16p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_focus.cpp $
 * 
 * 5     7/13/01 4:35p Ttc
 * Delete unused focus offsets.
 * 
 * 4     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 3     10/30/00 3:06p Ttc
 * Fix errors found during cpp compile.
 * 
 *    Rev 10.2   20 May 1998 15:33:54   ttc
 * Change 2 function names.
 * 
 *    Rev 10.1   14 May 1998 16:15:26   ttc
 * Delete unneeded functions.
 * 
 *    Rev 10.0   20 Mar 1998 16:07:42   ttc
 * Initial revision.
*/

/* MSC includes */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* OGP includes */
#include "err_hand.h"

#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


Target_Type *dbGetPointTarget(Feature *this_feature, int point_number);

int dbInitFocusData(Feature *this_feature, int point_number)
{
	char *loc_func_name = "dbInitFocusData";
	Target_Type *this_target;

	this_target = dbGetPointTarget(this_feature, point_number);
	
	if (this_target->extra == NULL)
	{
		Focus_Info *focus;
		
		focus = (Focus_Info *)malloc(sizeof(Focus_Info));
		if (focus == NULL)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
			return(GOT_ERROR);
		}
		focus->noise = 0;
		focus->step_size = 0;
		focus->backoff = 0;
		focus->optimize = 0;

		this_target->extra = focus;
	}
	return(NO_ERROR);
}

int dbFocusParametersExist(Feature *this_feature, int point_number)
{
	int exists = FALSE;
	Target_Type *this_target;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (this_target->extra != NULL)
		{
			exists = TRUE;
		}
	}
	return(exists);
}

int dbPutFocusNoise(Feature *this_feature, int point_number, int value)
{
	char *loc_func_name = "dbPutFocusNoise";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (dbInitFocusData(this_feature, point_number) == NO_ERROR)
		{
			focus = (Focus_Info *)this_target->extra;
			focus->noise = value;
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus init pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetFocusNoise(Feature *this_feature, int point_number)
{
	char *loc_func_name = "dbGetFocusNoise";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (this_target->extra != NULL)
		{
			focus = (Focus_Info *)this_target->extra;
			return(focus->noise);
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus extra pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
}

int dbPutFocusStepSize(Feature *this_feature, int point_number, int value)
{
	char *loc_func_name = "dbPutFocusStepSize";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (dbInitFocusData(this_feature, point_number) == NO_ERROR)
		{
			focus = (Focus_Info *)this_target->extra;
			focus->step_size = value;
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus init pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetFocusStepSize(Feature *this_feature, int point_number)
{
	char *loc_func_name = "dbGetFocusStepSize";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (this_target->extra != NULL)
		{
			focus = (Focus_Info *)this_target->extra;
			return(focus->step_size);
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus extra pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
}

int dbPutFocusBackoff(Feature *this_feature, int point_number, int value)
{
	char *loc_func_name = "dbPutFocusBackoff";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (dbInitFocusData(this_feature, point_number) == NO_ERROR)
		{
			focus = (Focus_Info *)this_target->extra;
			focus->backoff = value;
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus init pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetFocusBackoff(Feature *this_feature, int point_number)
{
	char *loc_func_name = "dbGetFocusBackoff";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (this_target->extra != NULL)
		{
			focus = (Focus_Info *)this_target->extra;
			return(focus->backoff);
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus extra pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
}

int dbPutFocusScanFlag(Feature *this_feature, int point_number, int value)
{
	char *loc_func_name = "dbPutFocusScanFlag";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (dbInitFocusData(this_feature, point_number) == NO_ERROR)
		{
			focus = (Focus_Info *)this_target->extra;
			focus->optimize = value;
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus init pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetFocusScanFlag(Feature *this_feature, int point_number)
{
	char *loc_func_name = "dbGetFocusScanFlag";
	Target_Type *this_target;
	Focus_Info *focus = NULL;

	this_target = dbGetPointTarget(this_feature, point_number);
	if (this_target != NULL)
	{
		if (this_target->extra != NULL)
		{
			focus = (Focus_Info *)this_target->extra;
			return(focus->optimize);
		}
		else
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus extra pointer",point_number);
			return(GOT_ERROR);
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"focus pointer",point_number);
		return(GOT_ERROR);
	}
}
