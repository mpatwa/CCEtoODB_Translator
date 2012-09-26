/*
$Workfile: Db_probe.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_probe.cpp 20    4/21/01 7:44a Ttc $

$Modtime: 4/21/01 7:43a $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_probe.cpp $
 * 
 * 20    4/21/01 7:44a Ttc
 * Declare all local variables at top of function so I can compile this as
 * a .c file for SmartFit.
 * 
 * 19    4/03/01 4:53p Ttc
 * SCR 13411.  In order to solve problems with plane auto-path, we need to
 * store the original boundry points of the plane so that they can be sent
 * into the clean up functions over and over.
 * 
 * 18    3/26/01 4:42p Lsz
 * Changed initialization of distance to top and bottom values from -1's
 * to 0's.
 * 
 * 17    3/14/01 3:55p Lsz
 * Undo my last changes.
 * 
 * 16    3/14/01 3:08p Lsz
 * Need to set default approach distance to 0.1 inches.
 * 
 * 15    3/06/01 3:08p Lsz
 * Need to default values to -1.
 * 
 * 14    3/01/01 9:18a Lsz
 * Reset some of the default parameters for touch probe data.
 * 
 * 13    2/20/01 5:59p Lsz
 * Changed default for plus/minus parameter.
 * 
 * 12    2/13/01 2:38p Lsz
 * Fixed bug - overwriting ongitudinal scans value with sectional scans
 * value.
 * 
 * 11    2/09/01 10:11a Lsz
 * Added new touch probe parameter.
 * 
 * 10    1/04/01 7:47a Lsz
 * Added a read and write of one more touch probe parameter.
 * 
 * 9     12/22/00 11:43a Lsz
 * Made change to default clean flag.
 * 
 * 8     12/12/00 8:23a Lsz
 * Added funtions for handling touch probe extended data information.
 * 
 * 7     11/13/00 1:56p Lsz
 * Fixed so that a feature is not cleaned up if the user does not click
 * into strategy window.
 * 
 * 6     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 5     10/30/00 3:06p Ttc
 * Fix errors found during cpp compile.
 * 
 * 4     9/20/00 3:36p Lsz
 * Fixed "Gets" to return 0.0 instead of an error.  Added function to
 * check whether Extended probe data exists.
 * 
 * 3     8/29/00 1:01p Lsz
 * Added functions for getting and putting touchprobe data.  This file
 * also includes the copy of touchprobe data.
*/

#include <stdlib.h>

#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

#include "err_hand.h"

int dbInitTouchProbeData(Feature *this_feature)
{
	char *loc_func_name = "dbInitTouchProbeData";

	if( this_feature->data.extended_data == NULL )
	{
		Touch_Probe_Info *probe_info;

		probe_info = (Touch_Probe_Info *)malloc(sizeof(Touch_Probe_Info));
		if (probe_info == NULL)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"touch probe data",NO_VAL);
			return(GOT_ERROR);
		}
	
		probe_info->isClean = -1;
		probe_info->useSafe = 0;
		probe_info->spacingflag = -1;
		probe_info->rowspacingflag = -1;
		probe_info->xyangle_flag = 1;
		probe_info->elevation_flag = 1;
		probe_info->safe = 0.0;
		probe_info->approach = 0.0;
		probe_info->xyangle = -1.0;
		probe_info->elevation = 0.0;
		probe_info->plus = -1;
		probe_info->spacing = -1.0;
		probe_info->points = -1;
		probe_info->dist_top = 0.0;
		probe_info->dist_bottom = 0.0;
		probe_info->internal = -1;
		probe_info->start_angle = 0.0;
		probe_info->end_angle = 360.0;
		probe_info->row_spacing = 0.0;
		probe_info->total_rows = 0;
		probe_info->total_scans = 0;
		probe_info->total_long_scans = 0;
		probe_info->total_revs = 0.0;
		probe_info->start_elevation = 0.0;
		probe_info->end_elevation = 90.0;
		probe_info->number_boundary_points = 0;
		probe_info->plane_boundary = NULL;
		this_feature->data.extended_data = probe_info;
		this_feature->data.extended_data_type = EXTENDED_DATA_TOUCHPROBE;
	}

	return(NO_ERROR);
}

int dbTouchProbeDataExist(Feature *this_feature)
{
	int exists = FALSE;
		
	if (this_feature != NULL)
	{
		if ((this_feature->data.extended_data != NULL) && 
			(this_feature->data.extended_data_type == EXTENDED_DATA_TOUCHPROBE))
			exists = TRUE;
	}
	return(exists);
}

int dbGetIsCleanFlag(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->isClean);
	}
	else
		return(DONT_VECTOR);
}

int dbPutIsCleanFlag(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutIsCleanFlag";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->isClean = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetSafeFlag(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->useSafe);
	}
	else
		return(FALSE);
}

int dbPutSafeFlag(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutSafeFlag";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->useSafe = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetSpacingFlag(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->spacingflag);
	}
	else
		return(FALSE);
}

int dbPutSpacingFlag(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutSpacingFlag";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->spacingflag = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetRowSpacingFlag(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->rowspacingflag);
	}
	else
		return(FALSE);
}

int dbPutRowSpacingFlag(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutRowSpacingFlag";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->rowspacingflag = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetXYAngleFlag(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->xyangle_flag);
	}
	else
		return(FALSE);
}

int dbPutXYAngleFlag(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutXYAngleFlag";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->xyangle_flag = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetElevationFlag(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->elevation_flag);
	}
	else
		return(FALSE);
}

int dbPutElevationFlag(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutElevationFlag";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->elevation_flag = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetSafeValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->safe);
	}
	else
		return(0.0);
}

int dbPutSafeValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutSafeValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->safe = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetApproachValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->approach);
	}
	else
		return(0.0);
}

int dbPutApproachValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutApproachValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->approach = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetXYAngleValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->xyangle);
	}
	else
		return(0.0);
}

int dbPutXYAngleValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutXYAngleValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->xyangle = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetElevationValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->elevation);
	}
	else
		return(0.0);
}

int dbPutElevationValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutElevationValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->elevation = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetPlusValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->plus);
	}
	else
		return(-1);
}

int dbPutPlusValue(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutPlusValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->plus = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetSpacingValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->spacing);
	}
	else
		return(0.0);
}

int dbPutSpacingValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutSpacingValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->spacing = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetPointsValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->points);
	}
	else
		return(0);
}

int dbPutPointsValue(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutPointsValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->points = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetDistTopValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->dist_top);
	}
	else
		return(0.0);
}

int dbPutDistTopValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutDistTopValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->dist_top = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetDistBottomValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->dist_bottom);
	}
	else
		return(0.0);
}

int dbPutDistBottomValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutDistBottomValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->dist_bottom = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetInternalValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->internal);
	}
	else
		return(-1);
}

int dbPutInternalValue(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutInternalValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->internal = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetStartAngleValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->start_angle);
	}
	else
		return(0.0);
}

int dbPutStartAngleValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutStartAngleValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->start_angle = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetEndAngleValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->end_angle);
	}
	else
		return(0.0);
}

int dbPutEndAngleValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutEndAngleValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->end_angle = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetStartElevationValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->start_elevation);
	}
	else
		return(0.0);
}

int dbPutStartElevationValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutStartElevationValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->start_elevation = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetEndElevationValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->end_elevation);
	}
	else
		return(0.0);
}

int dbPutEndElevationValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutEndElevationValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->end_elevation = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetRowSpacingValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->row_spacing);
	}
	else
		return(0.0);
}

int dbPutRowSpacingValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutRowSpacingValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->row_spacing = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetTotalRowValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->total_rows);
	}
	else
		return(0);
}

int dbPutTotalRowValue(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutTotalRowValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->total_rows = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetTotalScansValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->total_scans);
	}
	else
		return(0);
}

int dbPutTotalScansValue(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutTotalScansValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->total_scans = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetTotalLongScansValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->total_long_scans);
	}
	else
		return(0);
}

int dbPutTotalLongScansValue(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutTotalLongScansValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->total_long_scans = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetTotalRevsValue(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->total_revs);
	}
	else
		return(0);
}

int dbPutTotalRevsValue(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutTotalRevsValue";
	Touch_Probe_Info *probe_info = NULL;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		probe_info->total_revs = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetNumberBoundaryPoints(Feature *this_feature)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		return(probe_info->number_boundary_points);
	}
	else
		return(0);
}

int dbPutNumberBoundaryPoints(Feature *this_feature, int number_points)
{
	char *loc_func_name = "dbPutNumberBoundaryPoints";
	Touch_Probe_Info *probe_info = NULL;
	int i = 0;
	
	if (dbInitTouchProbeData(this_feature) == NO_ERROR)
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		if ( (number_points <= 0) ||
			 (number_points != probe_info->number_boundary_points) )
		{
			probe_info->number_boundary_points = 0;
			if (probe_info->plane_boundary != NULL)
			{
				free(probe_info->plane_boundary);
				probe_info->plane_boundary = NULL;
			}
		}
		if (number_points > 0)
		{
			probe_info->plane_boundary = (Xyz_Vector *)calloc(sizeof(Xyz_Vector),number_points);
			if (probe_info->plane_boundary == NULL)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"touch probe plane boundary",number_points);
				return(GOT_ERROR);
			}
			probe_info->number_boundary_points = number_points;
			for (i = 0; i < number_points; i++)
			{
				probe_info->plane_boundary[i].x = 0.0;
				probe_info->plane_boundary[i].y = 0.0;
				probe_info->plane_boundary[i].z = 0.0;
			}
		}
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"touch probe init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetPlaneBoundaryPointX(Feature *this_feature, int point_num)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		if ( (probe_info->number_boundary_points > 0) && (point_num > 0) &&
			 (point_num <= probe_info->number_boundary_points) )
		{
			return(probe_info->plane_boundary[point_num - 1].x);
		}
	}
	return(BAD_VALUE);
}

double dbGetPlaneBoundaryPointY(Feature *this_feature, int point_num)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		if ( (probe_info->number_boundary_points > 0) && (point_num > 0) &&
			 (point_num <= probe_info->number_boundary_points) )
		{
			return(probe_info->plane_boundary[point_num - 1].y);
		}
	}
	return(BAD_VALUE);
}

double dbGetPlaneBoundaryPointZ(Feature *this_feature, int point_num)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		if ( (probe_info->number_boundary_points > 0) && (point_num > 0) &&
			 (point_num <= probe_info->number_boundary_points) )
		{
			return(probe_info->plane_boundary[point_num - 1].z);
		}
	}
	return(BAD_VALUE);
}

int dbPutPlaneBoundaryPointX(Feature *this_feature, int point_num, double boundary_x)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		if ( (probe_info->number_boundary_points > 0) && (point_num > 0) &&
			 (point_num <= probe_info->number_boundary_points) )
		{
			probe_info->plane_boundary[point_num - 1].x = boundary_x;
			return(NO_ERROR);
		}
	}
	return(GOT_ERROR);
}

int dbPutPlaneBoundaryPointY(Feature *this_feature, int point_num, double boundary_y)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		if ( (probe_info->number_boundary_points > 0) && (point_num > 0) &&
			 (point_num <= probe_info->number_boundary_points) )
		{
			probe_info->plane_boundary[point_num - 1].y = boundary_y;
			return(NO_ERROR);
		}
	}
	return(GOT_ERROR);
}

int dbPutPlaneBoundaryPointZ(Feature *this_feature, int point_num, double boundary_z)
{
	Touch_Probe_Info *probe_info = NULL;

	if (dbTouchProbeDataExist(this_feature))
	{
		probe_info = (Touch_Probe_Info *)this_feature->data.extended_data;
		if ( (probe_info->number_boundary_points > 0) && (point_num > 0) &&
			 (point_num <= probe_info->number_boundary_points) )
		{
			probe_info->plane_boundary[point_num - 1].z = boundary_z;
			return(NO_ERROR);
		}
	}
	return(GOT_ERROR);
}

int dbCopyTouchProbeData(Feature *destination, Feature *source)
{
	int error = 0;
	int num_boundary_pts = 0;
	int i = 0;

	if( source->data.extended_data != NULL )
	{		
		error |= dbPutIsCleanFlag(destination, dbGetIsCleanFlag(source));
		error |= dbPutSafeFlag(destination, dbGetSafeFlag(source));
		error |= dbPutSpacingFlag(destination, dbGetSpacingFlag(source));
		error |= dbPutRowSpacingFlag(destination, dbGetRowSpacingFlag(source));
		error |= dbPutXYAngleFlag(destination, dbGetXYAngleFlag(source));
		error |= dbPutElevationFlag(destination, dbGetElevationFlag(source));
		error |= dbPutSafeValue(destination, dbGetSafeValue(source));
		error |= dbPutApproachValue(destination, dbGetApproachValue(source));
		error |= dbPutXYAngleValue(destination, dbGetXYAngleValue(source));
		error |= dbPutElevationValue(destination, dbGetElevationValue(source));
		error |= dbPutPlusValue(destination, dbGetPlusValue(source));
		error |= dbPutSpacingValue(destination, dbGetSpacingValue(source));
		error |= dbPutPointsValue(destination, dbGetPointsValue(source));
		error |= dbPutDistTopValue(destination, dbGetDistTopValue(source));
		error |= dbPutDistBottomValue(destination, dbGetDistBottomValue(source));
		error |= dbPutInternalValue(destination, dbGetInternalValue(source));
		error |= dbPutStartAngleValue(destination, dbGetStartAngleValue(source));
		error |= dbPutEndAngleValue(destination, dbGetEndAngleValue(source));
		error |= dbPutRowSpacingValue(destination, dbGetRowSpacingValue(source));
		error |= dbPutTotalRowValue(destination, dbGetTotalRowValue(source));
		error |= dbPutTotalScansValue(destination, dbGetTotalScansValue(source));
		error |= dbPutTotalLongScansValue(destination, dbGetTotalLongScansValue(source));
		error |= dbPutTotalRevsValue(destination, dbGetTotalRevsValue(source));
		error |= dbPutStartElevationValue(destination, dbGetStartElevationValue(source));
		error |= dbPutEndElevationValue(destination, dbGetEndElevationValue(source));
		num_boundary_pts = dbGetNumberBoundaryPoints(source);
		error |= dbPutNumberBoundaryPoints(destination, num_boundary_pts);
		for (i = 1; i <= num_boundary_pts; i++)
		{
			error |= dbPutPlaneBoundaryPointX(destination, i, dbGetPlaneBoundaryPointX(source, i));
			error |= dbPutPlaneBoundaryPointY(destination, i, dbGetPlaneBoundaryPointY(source, i));
			error |= dbPutPlaneBoundaryPointZ(destination, i, dbGetPlaneBoundaryPointZ(source, i));
		}
	}
	return error;
}
