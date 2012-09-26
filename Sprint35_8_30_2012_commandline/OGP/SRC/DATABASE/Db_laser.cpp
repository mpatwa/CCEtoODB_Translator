
// Includes copied from db_focus.c

/* MSC includes */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* OGP includes */
#include "err_hand.h"

#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


int dbInitLaserScanData(Feature *this_feature)
{
	char *loc_func_name = "dbInitLaserScanData";
	int i;
	if( this_feature->data.extended_data == NULL )
	{
		Laser_Scan_Info *laser_info;

		laser_info = (Laser_Scan_Info *)malloc(sizeof(Laser_Scan_Info));
		if (laser_info == NULL)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
			return(GOT_ERROR);
		}
		for( i = 0; i < MAX_LASER_DEFINITION_POINTS ; i++)
		{
			laser_info->definition_points[i].x = 0.0;
			laser_info->definition_points[i].y = 0.0;
			laser_info->definition_points[i].z = 0.0;
		}
		laser_info->laser_exposure_time = 0;
		laser_info->laser_threshold = 0;
		laser_info->line_frequency = 0.0;
		laser_info->line_frequency_type = 0;
		laser_info->num_points_to_average = 0;
		laser_info->point_frequency = 0;
		laser_info->point_frequency_type = 0;
		laser_info->scan_strategy = 0;
		laser_info->tracking_scan = 0;
		laser_info->laser_id_num = 0;
		laser_info->scan_speed_factor = 100;
		this_feature->data.extended_data = laser_info;
		this_feature->data.extended_data_type = EXTENDED_DATA_LASER;
	}
	return(NO_ERROR);
}

int dbLaserScanParametersExist(Feature *this_feature)
{
	int exists = FALSE;
		
	if (this_feature != NULL)
	{
		if ((this_feature->data.extended_data != NULL) && 
			(this_feature->data.extended_data_type == EXTENDED_DATA_LASER))
		{
			exists = TRUE;
		}
	}
	return(exists);
}

int dbGetLaserPointFrequencyType(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserPointFrequencyType";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->point_frequency_type);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserPointFrequencyType(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserPointFrequencyType";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->point_frequency_type = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetLaserPointFrequency(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserPointFrequency";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->point_frequency);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserPointFrequency(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutLaserPointFrequency";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->point_frequency = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserScanStrategy(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserScanStrategy";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->scan_strategy);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserScanStrategy(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserScanStrategy";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->scan_strategy = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserTrackingScan(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserTrackingScan";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->tracking_scan);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserTrackingScan(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserTrackingScan";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->tracking_scan = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserLineFrequencyType(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserLineFrequencyType";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->line_frequency_type);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserLineFrequencyType(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserLineFrequencyType";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->line_frequency_type = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetLaserLineFrequency(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserLineFrequency";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->line_frequency);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserLineFrequency(Feature *this_feature, double value)
{
	char *loc_func_name = "dbPutLaserLineFrequency";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->line_frequency = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetLaserDefinitionPointX(Feature *this_feature, int point_num)
{
	char *loc_func_name = "dbGetLaserDefinitionPointX";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->definition_points[point_num].x);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserDefinitionPointX(Feature *this_feature, int point_num, double value)
{
	char *loc_func_name = "dbPutLaserDefinitionPointX";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->definition_points[point_num].x = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetLaserDefinitionPointY(Feature *this_feature, int point_num)
{
	char *loc_func_name = "dbGetLaserDefinitionPointY";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->definition_points[point_num].y);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserDefinitionPointY(Feature *this_feature, int point_num, double value)
{
	char *loc_func_name = "dbPutLaserDefinitionPointY";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->definition_points[point_num].y = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

double dbGetLaserDefinitionPointZ(Feature *this_feature, int point_num)
{
	char *loc_func_name = "dbGetLaserDefinitionPointZ";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->definition_points[point_num].z);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserDefinitionPointZ(Feature *this_feature, int point_num, double value)
{
	char *loc_func_name = "dbPutLaserDefinitionPointZ";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->definition_points[point_num].z = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserThreshold(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserThreshold";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->laser_threshold);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserThreshold(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserThreshold";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->laser_threshold = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserMaxExposure(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserMaxExposure";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->laser_exposure_time);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserMaxExposure(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserMaxExposure";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->laser_exposure_time = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserAverages(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserAverages";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->num_points_to_average);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserAverages(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserAverages";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->num_points_to_average = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserIDNum(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserIDNum";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->laser_id_num);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserIDNum(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserIDNum";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->laser_id_num = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbGetLaserScanSpeedFactor(Feature *this_feature)
{
	char *loc_func_name = "dbGetLaserScanSpeedFactor";	
	Laser_Scan_Info *laser_info = NULL;

	if (dbLaserScanParametersExist(this_feature))
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		return(laser_info->scan_speed_factor);
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser scan pointer",0);
		return(GOT_ERROR);
	}
}

int dbPutLaserScanSpeedFactor(Feature *this_feature, int value)
{
	char *loc_func_name = "dbPutLaserScanSpeedFactor";
	Laser_Scan_Info *laser_info = NULL;
	
	if (dbInitLaserScanData(this_feature) == NO_ERROR)
	{
		laser_info = (Laser_Scan_Info *)this_feature->data.extended_data;
		laser_info->scan_speed_factor = value;
	}
	else
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"laser init pointer",0);
		return(GOT_ERROR);
	}
	return(NO_ERROR);
}

int dbCopyLaserData(Feature *destination, Feature *source)
{
	int i,error = 0;
	if( dbLaserScanParametersExist(source) )
	{		
		error |= dbPutLaserPointFrequencyType(destination, dbGetLaserPointFrequencyType(source));
		error |= dbPutLaserPointFrequency(destination, dbGetLaserPointFrequency(source));
		error |= dbPutLaserScanStrategy(destination, dbGetLaserScanStrategy(source));
		error |= dbPutLaserTrackingScan(destination, dbGetLaserTrackingScan(source));
		error |= dbPutLaserLineFrequencyType(destination, dbGetLaserLineFrequencyType(source));
		error |= dbPutLaserLineFrequency(destination, dbGetLaserLineFrequency(source));
		for( i = 0 ; i < MAX_LASER_DEFINITION_POINTS ; i++ )
		{
			error |= dbPutLaserDefinitionPointX(destination, i, dbGetLaserDefinitionPointX(source, i));
			error |= dbPutLaserDefinitionPointY(destination, i, dbGetLaserDefinitionPointY(source, i));
			error |= dbPutLaserDefinitionPointZ(destination, i, dbGetLaserDefinitionPointZ(source, i));
		}
		error |= dbPutLaserThreshold(destination, dbGetLaserThreshold(source));
		error |= dbPutLaserMaxExposure(destination, dbGetLaserMaxExposure(source));
		error |= dbPutLaserAverages(destination, dbGetLaserAverages(source));
		error |= dbPutLaserIDNum(destination, dbGetLaserIDNum(source));
		error |= dbPutLaserScanSpeedFactor(destination, dbGetLaserScanSpeedFactor(source));
	}
	return error;
}
