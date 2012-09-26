/*
$Workfile: Db_smear.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/Db_smear.c 5     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/Db_smear.c $
 * 
 * 5     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 4     11/11/99 10:26a Ttc
 * Add direction vector to the smear structure also.  No bump in db
 * version number, so any rtn stored between 10/29/99 and today (11/11/99)
 * is trash.
 * 
 * 3     7/06/99 3:10p Ttc
 * SCR 6627.  Add the z location for the start, middle and end weak edge /
 * edge trace points.  Weak edge and edge trace need the full 3D values
 * for all points so that they work after a plane align.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:20   softadmn
 * Initial revision.
 * 
 *    Rev 9.3   28 Jun 1996 08:36:06   ttc
 * Add led flag.
 * 
 *    Rev 9.2   18 Jun 1996 13:24:48   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.1   20 May 1996 14:07:22   ttc
 * Add smear led lights.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:56   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:20   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:16   softadmn
 * Initial revision.
*/

#include "err_hand.h"

#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


void dbPutSmearType(Feature *this_feature, int type)
{
	this_feature->data.smear_data.smr_type = type;
	return;
}

int dbGetSmearType(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_type);
}

void dbPutSmearCenterX(Feature *this_feature, double x)
{
	this_feature->data.smear_data.smr_screen_center.x = x;
	return;
}

double dbGetSmearCenterX(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_screen_center.x);
}

void dbPutSmearCenterY(Feature *this_feature, double y)
{
	this_feature->data.smear_data.smr_screen_center.y = y;
	return;
}

double dbGetSmearCenterY(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_screen_center.y);
}

void dbPutSmearCenterZ(Feature *this_feature, double z)
{
	this_feature->data.smear_data.smr_screen_center.z = z;
	return;
}

double dbGetSmearCenterZ(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_screen_center.z);
}

void dbPutSmearZoom(Feature *this_feature, double zoom)
{
	this_feature->data.smear_data.smr_zoom_pos = zoom;
	return;
}

double dbGetSmearZoom(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_zoom_pos);
}

void dbPutSmearBackLight(Feature *this_feature, int back_lite)
{
	this_feature->data.smear_data.lights.backlight = back_lite;
	return;
}

int dbGetSmearBackLight(Feature *this_feature)
{
	return(this_feature->data.smear_data.lights.backlight);
}

void dbPutSmearRingLight(Feature *this_feature, int ring_lite)
{
	this_feature->data.smear_data.lights.ringlight = ring_lite;
	return;
}

int dbGetSmearRingLight(Feature *this_feature)
{
	return(this_feature->data.smear_data.lights.ringlight);
}

void dbPutSmearAuxLight(Feature *this_feature, int aux_lite)
{
	this_feature->data.smear_data.lights.auxlight = aux_lite;
	return;
}

int dbGetSmearAuxLight(Feature *this_feature)
{
	return(this_feature->data.smear_data.lights.auxlight);
}

int dbPutSmearLedIntensity(Feature *this_feature, int sector, int ring,
						   int new_intensity)
{
	char *loc_func_name = "dbPutSmearLedIntensity";

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

	this_feature->data.smear_data.lights.led[sector - 1][ring - 1] = new_intensity;
	return(NO_ERROR);
}

int dbGetSmearLedIntensity(Feature *this_feature, int sector, int ring)
{
	char *loc_func_name = "dbGetSmearLedIntensity";

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

	return(this_feature->data.smear_data.lights.led[sector - 1][ring - 1]);
}

void dbPutSmearLedFlag(Feature *this_feature, int flag)
{
	this_feature->data.smear_data.lights.led_flag = flag;
	return;
}

int dbGetSmearLedFlag(Feature *this_feature)
{
	return(this_feature->data.smear_data.lights.led_flag);
}

void dbPutSmearStartPtX(Feature *this_feature, double x)
{
	this_feature->data.smear_data.smr_start.x = x;
	return;
}

double dbGetSmearStartPtX(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_start.x);
}

void dbPutSmearStartPtY(Feature *this_feature, double y)
{
	this_feature->data.smear_data.smr_start.y = y;
	return;
}

double dbGetSmearStartPtY(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_start.y);
}

void dbPutSmearStartPtZ(Feature *this_feature, double z)
{
	this_feature->data.smear_data.smr_start.z = z;
	return;
}

double dbGetSmearStartPtZ(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_start.z);
}

void dbPutSmearEndPtX(Feature *this_feature, double x)
{
	this_feature->data.smear_data.smr_end.x = x;
	return;
}

double dbGetSmearEndPtX(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_end.x);
}

void dbPutSmearEndPtY(Feature *this_feature, double y)
{
	this_feature->data.smear_data.smr_end.y = y;
	return;
}

double dbGetSmearEndPtY(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_end.y);
}

void dbPutSmearEndPtZ(Feature *this_feature, double z)
{
	this_feature->data.smear_data.smr_end.z = z;
	return;
}

double dbGetSmearEndPtZ(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_end.z);
}

void dbPutSmearMidPtX(Feature *this_feature, double x)
{
	this_feature->data.smear_data.smr_mid.x = x;
	return;
}

double dbGetSmearMidPtX(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_mid.x);
}

void dbPutSmearMidPtY(Feature *this_feature, double y)
{
	this_feature->data.smear_data.smr_mid.y = y;
	return;
}

double dbGetSmearMidPtY(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_mid.y);
}

void dbPutSmearMidPtZ(Feature *this_feature, double z)
{
	this_feature->data.smear_data.smr_mid.z = z;
	return;
}

double dbGetSmearMidPtZ(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_mid.z);
}

void dbPutSmearDirectionVectorX(Feature *this_feature, double x)
{
	this_feature->data.smear_data.smr_vector.x = x;
	return;
}

double dbGetSmearDirectionVectorX(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_vector.x);
}

void dbPutSmearDirectionVectorY(Feature *this_feature, double y)
{
	this_feature->data.smear_data.smr_vector.y = y;
	return;
}

double dbGetSmearDirectionVectorY(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_vector.y);
}

void dbPutSmearDirectionVectorZ(Feature *this_feature, double z)
{
	this_feature->data.smear_data.smr_vector.z = z;
	return;
}

double dbGetSmearDirectionVectorZ(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_vector.z);
}

void dbPutSmearDirection(Feature *this_feature, int dir)
{
	this_feature->data.smear_data.smr_direction = dir;
	return;
}

int dbGetSmearDirection(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_direction);
}

void dbPutSmearBounds(Feature *this_feature, int bounds)
{
	this_feature->data.smear_data.smr_bounds = bounds;
	return;
}

int dbGetSmearBounds(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_bounds);
}

void dbPutSmearExtent(Feature *this_feature, int extent)
{
	this_feature->data.smear_data.smr_extent = extent;
	return;
}

int dbGetSmearExtent(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_extent);
}

void dbPutSmearMinQuality(Feature *this_feature, int min_quality)
{
	this_feature->data.smear_data.smr_min_quality = min_quality;
	return;
}

int dbGetSmearMinQuality(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_min_quality);
}

void dbPutSmearContrast(Feature *this_feature, int contrast)
{
	this_feature->data.smear_data.smr_constrast = contrast;
	return;
}

int dbGetSmearContrast(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_constrast);
}

void dbPutSmearWgtNominal(Feature *this_feature, int wgt_nominal)
{
	this_feature->data.smear_data.smr_wgt_nominal = wgt_nominal;
	return;
}

int dbGetSmearWgtNominal(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_wgt_nominal);
}

void dbPutSmearWgtContrast(Feature *this_feature, int wgt_contrast)
{
	this_feature->data.smear_data.smr_wgt_constrast = wgt_contrast;
	return;
}

int dbGetSmearWgtContrast(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_wgt_constrast);
}

void dbPutSmearWgtFirst(Feature *this_feature, int wgt_first_edge)
{
	this_feature->data.smear_data.smr_wgt_first = wgt_first_edge;
	return;
}

int dbGetSmearWgtFirst(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_wgt_first);
}

void dbPutSmearWgtLast(Feature *this_feature, int wgt_last_edge)
{
	this_feature->data.smear_data.smr_wgt_last = wgt_last_edge;
	return;
}

int dbGetSmearWgtLast(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_wgt_last);
}

void dbPutSmearWgtSecond(Feature *this_feature, int wgt_second)
{
	this_feature->data.smear_data.smr_wgt_second = wgt_second;
	return;
}

int dbGetSmearWgtSecond(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_wgt_second);
}

void dbPutSmearWgt2Last(Feature *this_feature, int wgt_second_last)
{
	this_feature->data.smear_data.smr_wgt_second_last = wgt_second_last;
	return;
}

int dbGetSmearWgt2Last(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_wgt_second_last);
}

void dbPutSmearStdDev(Feature *this_feature, int standard_deviation)
{
	this_feature->data.smear_data.smr_std_dev = standard_deviation;
	return;
}

int dbGetSmearStdDev(Feature *this_feature)
{
	return(this_feature->data.smear_data.smr_std_dev);
}
