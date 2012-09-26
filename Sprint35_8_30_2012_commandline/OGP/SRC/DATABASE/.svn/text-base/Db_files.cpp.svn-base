/*
$Workfile: Db_files.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_files.cpp 46    7/18/01 4:48p Ttc $

$Modtime: 7/18/01 4:36p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_files.cpp $
 * 
 * 46    7/18/01 4:48p Ttc
 * SCR 14290.  If a feature is not valid (doesn't have enough points),
 * don't add it to the model and don't allow the routine to be saved.
 * 
 * 45    5/31/01 10:58a Jpk
 * Added laser speed factor for SCR# 13891
 * 
 * 44    5/03/01 12:04p Jnc
 * SCR12676:added the stream template file to the list of saved data
 * 
 * 43    4/17/01 3:46p Lsz
 * Fixed put of total revs value.  Was reading in the integer value of
 * touchprobe_ints[10] for the total revs value instead of
 * touchprobe_doubles[10]..
 * 
 * 42    4/03/01 4:53p Ttc
 * SCR 13411.  In order to solve problems with plane auto-path, we need to
 * store the original boundry points of the plane so that they can be sent
 * into the clean up functions over and over.
 * 
 * 41    3/26/01 8:32a Jpk
 * Changed name of new laser parameter from dock to deploy.
 * 
 * 40    3/22/01 8:35a Jnc
 * SCR13258:added an end step global value to list of information to save
 * for the repeat steps
 * 
 * 39    3/22/01 8:25a Jpk
 * Added new flag for deployable laser to the flags structure.
 * 
 * 38    3/12/01 3:31p Jpk
 * Added fix for SCR# 13195
 * 
 * 37    3/02/01 10:44a Lsz
 * Fixed read of elevation angle.  
 * 
 * 36    3/01/01 9:17a Lsz
 * Put in a read of the xyangle parameter for touch probes (it was
 * missing).  
 * 
 * 35    2/20/01 3:46p Lsz
 * Must save original values - do not want to default any values.
 * 
 * 34    2/20/01 8:10a Lsz
 * Must save original values - do not want to default any values.
 * 
 * 33    2/19/01 9:37a Lsz
 * Fixed SCR: 12971.  Must save original internal value - do not want to
 * default it.
 * 
 * 32    2/09/01 10:10a Lsz
 * Added read and write for new touchprobe parameter.
 * 
 * 31    1/04/01 8:49a Lsz
 * Bumped database version CORRECTLY for touch probe information.
 * 
 * 30    1/04/01 8:36a Lsz
 * Bumped database version - added new parameter for touchprobe
 * information.
 * 
 * 29    1/04/01 7:46a Lsz
 * Modified read and write for touch probe data to include one more
 * parameter.
 * 
 * 28    12/12/00 8:21a Lsz
 * Added read and write of touchprobe data for Auto Path Generation.
 * 
 * 27    10/06/00 1:52p Ttc
 * SCR 11792.  Add output precision override to export and stats.  Major
 * output overhaul.
 * 
 * 26    9/20/00 3:37p Lsz
 * Added a "put" to set the clean flag to Don't clean on legacy programs.
 * 
 * 25    9/05/00 4:25p Ttc
 * SCR 11484.  Fix read and write of laser scan doubles.
 * 
 * 24    8/29/00 1:00p Lsz
 * Added reading and writing for touchprobe data.
 * 
 * 23    8/14/00 8:03a Jpk
 * Added extended data pointer type value to eliminate database errors and
 * expand the usefullness of the pointer.
 * 
 * 22    8/08/00 9:39a Jpk
 * Read and write the laser scan parameters from and to a routine.
 * 
 * 21    1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 20    11/11/99 10:26a Ttc
 * Add direction vector to the smear structure also.  No bump in db
 * version number, so any rtn stored between 10/29/99 and today (11/11/99)
 * is trash.
 * 
 * 19    10/29/99 11:16a Ttc
 * Fix my stupid mistake in reading compensated point locations and point
 * direction vectors.
 * 
 * 18    10/25/99 3:00p Ttc
 * Add read and write of compensated point location and point direction
 * vector.
 * 
 * 17    7/06/99 3:10p Ttc
 * SCR 6627.  Add the z location for the start, middle and end weak edge /
 * edge trace points.  Weak edge and edge trace need the full 3D values
 * for all points so that they work after a plane align.
 * 
 * 16    6/04/99 9:32a Ttc
 * DN 9723 version 4. Forgot the read and write of the nominal 3 and
 * tolerances.
 * 
 * 15    6/03/99 3:07p Ttc
 * DN 9723 version 4. Only need 3 orientation angles.  Need 8 report and
 * export labels.
 * 
 * 14    5/28/99 1:57p Ttc
 * Needed parens for some typecasting.
 * 
 * 13    5/28/99 11:47a Ttc
 * Add explicit typecasts.
 * 
 * 12    5/26/99 11:04a Ttc
 * Change Auxilary size and tolerance function names and internal variable
 * names to nominal size2, and upper and lower size2 tols in preparation
 * to add a nominal size 3 with tolerances for cone.
 * 
 * 11    5/25/99 9:57a Ttc
 * Delete QCHECK ifdef code and ifndefs.
 * 
 * 10    4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 9     4/14/99 2:58p Ttc
 * SCR 6352. Need to read and write short versions of fixture repeat
 * offset flags.
 * 
 * 8     4/13/99 11:14a Ttc
 * Add angle orientations.
 * 
 * 7     3/17/99 9:05a Ttc
 * Add direction to results.
 * 
 *    Rev 10.4   02 Sep 1998 13:59:04   ttc
 * Use new direct memory db access function.
 * 
 *    Rev 10.3   24 Jul 1998 08:23:10   ttc
 * Add read and write of feature part datum transforms and actual point locs.
 * 
 *    Rev 10.2   20 May 1998 15:33:44   ttc
 * Add f2 focus parameter read and write.
 * 
 *    Rev 10.1   14 Apr 1998 12:02:04   ttc
 * Add extra dimension for edge trace.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:22   softadmn
 * Initial revision.
 * 
 *    Rev 9.13   29 Sep 1997 11:45:52   dmf
 * oops...Changed NULL to TEMP_DEFAULT
 * 
 *    Rev 9.12   29 Sep 1997 11:26:26   dmf
 * Changed hardcoded "TEMP" to TEMP_DEFAULT.
 * 
 *    Rev 9.11   25 Aug 1997 09:15:34   ttc
 * Add tp200 dock station.
 * 
 *    Rev 9.10   11 Jul 1997 10:46:20   dmf
 * change char* to char[] for smartreport
 * 
 *    Rev 9.9   25 Jun 1997 10:40:06   dmf
 * Declared some variables here instead of externing them for SmartFit.
 * 
 *    Rev 9.8   20 Jun 1997 10:13:24   dmf
 * Database changes for SmartReport
 * 
 *    Rev 9.7   18 Apr 1997 16:02:14   dmf
 * Added SmartReport
 * 
 *    Rev 9.6   28 Jun 1996 08:36:00   ttc
 * Add led flag.
 * 
 *    Rev 9.5   18 Jun 1996 13:24:38   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.4   20 May 1996 14:07:18   ttc
 * Add smear led lights.
 * 
 *    Rev 9.3   18 May 1996 08:46:32   ttc
 * Add LED light.
 * 
 *    Rev 9.2   08 Apr 1996 16:48:32   ttc
 * Add read and write of lens code and number of measured points.
 * 
 *    Rev 9.1   18 Mar 1996 13:04:10   ttc
 * Add miscellaneous flag. Delete standard deviation flag.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:58   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:22   softadmn
 * Initial revision.
 * 
 *    Rev 8.17   28 Feb 1996 08:05:54   ttc
 * Ifdef out some code for checking repeat offsets for QCheck.
 * 
 *    Rev 8.16   27 Feb 1996 14:53:38   ttc
 * Add stdlib for qcheck.
 * 
 *    Rev 8.15   27 Feb 1996 14:05:18   ttc
 * Delete unneeded language.h.
 * 
 *    Rev 8.14   27 Feb 1996 13:31:16   ttc
 * Fix repeat for QCheck.
 * 
 *    Rev 8.13   27 Feb 1996 10:14:34   ttc
 * Add ifndefs for QCHECK.
 * 
 *    Rev 8.12   10 Jan 1996 15:32:36   ttc
 * Make sure feature is dispose of if error occurs during get or save.
 * 
 *    Rev 8.11   08 Jan 1996 11:26:40   ttc
 * Add check for database versions greater than the greatest version this rev
 * could write.
 * 
 *    Rev 8.10   12 Dec 1995 10:26:30   ttc
 * Finialize part repeat.
 * 
 *    Rev 8.9   05 Dec 1995 14:56:56   ttc
 * Repeat enhancements.
 * 
 *    Rev 8.8   25 Oct 1995 10:51:30   ttc
 * Add check for database versions less than or equal to 8 to always set the
 * nominals entered flag to true for constructed points.
 * 
 *    Rev 8.7   21 Aug 1995 11:43:54   dmf
 * Save gobals for data stream override and stat override.
 * 
 *    Rev 8.6   21 Aug 1995 10:25:18   softadmn
 * Fix read of template config file name.
 * 
 *    Rev 8.5   11 Aug 1995 15:01:40   dmf
 * Initilization, loading and saving data stream to the database.
 * 
 *    Rev 8.4   01 Aug 1995 13:43:20   dmf
 * Load and save print_run_time and gb_template_filename. Also bump version
 * number upto 10
 * 
 *    Rev 8.3   23 Jun 1995 15:34:20   dmf
 * Made changes for print run time option
 * 
 *    Rev 8.2   21 Jun 1995 09:14:08   ttc
 * To ensure that lower tols are negative, negate the absolute value.  DMIS was
 * storing lower tols as negative values and we were changing them to positive.
 * 
 *    Rev 8.1   27 Feb 1995 15:36:54   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:42   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <io.h>

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

/* OGP includes */
#include "gn_stdio.h"
#include "get_env.h"
#include "err_hand.h"

#define CHARS_TO_COMPARE 10

extern int gb_fixture_repeat;
extern int gb_repeat_this_offset[];
extern double gb_repeat_x_offsets[];
extern double gb_repeat_y_offsets[];
extern int gb_repeat_confirm_next;

extern int gb_repeat_finish_run;
extern int gb_repeat_start_step;
extern int gb_repeat_end_step;

extern int gb_part_repeat;
extern double gb_x_part_offset;
extern double gb_y_part_offset;
extern int gb_x_part_times;
extern int gb_y_part_times;
extern int gb_repeat_y_first;

char gb_run_header_txt[NUM_HEADER_LINES][81];
char gb_run_setup_txt[NUM_SETUP_LINES][81];
int gb_db_version = DATABASE_VERSION;
char gb_db_header[33] = "";
char gb_stat_filename[_MAX_PATH] = "";
char gb_template_filename[_MAX_PATH] = "";
char gb_stream_filename[_MAX_PATH] = "";
char gb_stream_template_filename[_MAX_PATH] = "";
int gb_keep_stat_filename = FALSE;
int gb_keep_stream_filename = FALSE;
int gb_display_run_windows = TRUE;
int gb_stop_out_of_tol = FALSE;
int gb_printer_override = 0;
int gb_stat_override = 0;
int gb_stream_override = 0;
int gb_auto_edge_enabled = TRUE;
char gb_printer_filename[_MAX_PATH] = "";
int gb_print_to_file = FALSE;
int gb_done_during_run = FALSE;
int gb_missed_edge_override = FALSE;
int gb_print_run_time = FALSE;
int gb_rotary_in_use = FALSE;

int gb_report_override = 0;
int gb_export_override = 0;
int gb_sample_size_override = 10;
int gb_sample_size_override_on = 0;
char gb_report_type_path_override[_MAX_PATH] = "\0";
int gb_report_type_path_override_on = 0;
int gb_report_out_of_spec = FALSE;

int gb_report_precision = 0;
int gb_report_sample_size = 10;
char gb_report_type_path[_MAX_PATH]="\0";
int gb_export_every_part = 1;
char gb_export_type_path[_MAX_PATH]="\0";
char gb_export_table_name[LONG_STRING + 1]="Sheet1";
char gb_export_file_type[LONG_STRING + 1]="Excel 5.0\0";
int gb_export_precision = 0;

int dbGetOldDataBase(char *routine_name);
int dbCopyBytes(FILE *in_file, FILE *out_file, long file_bytes);

int dbGetVersion(void)
{
	return(gb_db_version);
}

int dbResetDataBase(void)
{
	char *loc_func_name = "dbResetDataBase";
	int i;

	if (dbInitDataBase() != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,INIT_FAILED,"database",NO_VAL);
		return(INIT_FAILED);
	}
	for (i = 0; i < NUM_HEADER_LINES; i++)
	{
		(void) memset(gb_run_header_txt[i],0,81);
	}
	for (i = 0; i < NUM_SETUP_LINES; i++)
	{
		(void) memset(gb_run_setup_txt[i],0,81);
	}
	return(NO_ERROR);
}

int dbWriteFeatureEntry(Feature *this_feature, FILE *features_file)
{
	char *loc_func_name = "dbWriteFeatureEntry";
	short flags[19] = { 0 };
	double dimensions[2] = { 0.0 };
	unsigned short comment_length = 0;
	char *comment = NULL;
	unsigned short prompt_length = 0;
	char *prompt = NULL;
	double smear[16] = { 0 };
	unsigned short expression_length = 0;
	char *expression = NULL;
	short led_intensity[MAX_LED_SECTORS][MAX_LED_RINGS] = { 0 };
	int i,j,start_idx;
	unsigned short temp_length = 0;
	char *temp = NULL;
	short laser_scan_ints[9] = { 0 };
	double laser_scan_doubles[17] = { 0.0 };

	short touchprobe_ints[13] = { 0 };
	double touchprobe_doubles[13] = { 0.0 };

	short extended_data_type = 0;

	flags[0] = (short) dbGetFeatureNumber(this_feature);
	flags[1] = (short) dbGetFeatureType(this_feature);
	flags[2] = (short) dbGetFeaturePlane(this_feature);
	flags[3] = (short) dbGetInMmFlag(this_feature);
	flags[4] = (short) dbGetCartPolarFlag(this_feature);
	flags[5] = (short) dbGetFeatureMode(this_feature);
	flags[6] = (short) dbGetStatisticsFlags(this_feature);
	flags[7] = (short) dbGetPrintFlags(this_feature);
	flags[8] = (short) dbGetLastDisplay(this_feature);	
	flags[9] = (short) dbGetDecDmsFlag(this_feature);	
	flags[10] = (short) dbGetDataStreamFlag(this_feature);
	flags[11] = (short) dbGetMiscFlag(this_feature);
	flags[12] = (short) dbGetFeatureLensCode(this_feature);
	flags[13] = (short) dbGetReportFlags(this_feature);
	flags[14] = (short) dbGetExportFlags(this_feature);
	flags[15] = (short) dbGetFeatureDockLocation(this_feature);
	flags[16] = (short) dbGetLastDisplay2(this_feature);
	flags[17] = (short) dbGetLastDisplay3(this_feature);
	flags[18] = (short) dbGetFeatureDeployLaserFlag(this_feature);
		
	if (fwrite(flags,sizeof(short),19,features_file) != 19)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature flags",NO_VAL);
		return(NO_WRITE_FILE);
	}

	dimensions[0] = dbGetBasicDimension(this_feature);
	dimensions[1] = dbGetExtraDimension(this_feature);
	if (fwrite(dimensions,sizeof(double),2,features_file) != 2)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"basic dimensions",NO_VAL);
		return(NO_WRITE_FILE);
	}

	smear[0] = dbGetSmearCenterX(this_feature);
	smear[1] = dbGetSmearCenterY(this_feature);
	smear[2] = dbGetSmearCenterZ(this_feature);
	smear[3] = dbGetSmearZoom(this_feature);
	smear[4] = dbGetSmearStartPtX(this_feature);
	smear[5] = dbGetSmearStartPtY(this_feature);
	smear[6] = dbGetSmearEndPtX(this_feature);
	smear[7] = dbGetSmearEndPtY(this_feature);
	smear[8] = dbGetSmearMidPtX(this_feature);
	smear[9] = dbGetSmearMidPtY(this_feature);
	smear[10] = dbGetSmearStartPtZ(this_feature);
	smear[11] = dbGetSmearEndPtZ(this_feature);
	smear[12] = dbGetSmearMidPtZ(this_feature);
	smear[13] = dbGetSmearDirectionVectorX(this_feature);
	smear[14] = dbGetSmearDirectionVectorY(this_feature);
	smear[15] = dbGetSmearDirectionVectorZ(this_feature);
	if (fwrite(smear,sizeof(double),16,features_file) != 16)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"weak edge points",NO_VAL);
		return(NO_WRITE_FILE);
	}

	flags[0] = (short) dbGetSmearType(this_feature); 
	flags[1] = (short) dbGetSmearBackLight(this_feature); 
	flags[2] = (short) dbGetSmearRingLight(this_feature); 
	flags[3] = (short) dbGetSmearAuxLight(this_feature); 
	flags[4] = (short) dbGetSmearDirection(this_feature); 
	flags[5] = (short) dbGetSmearBounds(this_feature); 
	flags[6] = (short) dbGetSmearExtent(this_feature); 
	flags[7] = (short) dbGetSmearMinQuality(this_feature); 
	flags[8] = (short) dbGetSmearWgtContrast(this_feature); 
	flags[9] = (short) dbGetSmearWgtNominal(this_feature); 
	flags[10] = (short) dbGetSmearContrast(this_feature); 
	flags[11] = (short) dbGetSmearWgtFirst(this_feature); 
	flags[12] = (short) dbGetSmearWgtLast(this_feature); 
	flags[13] = (short) dbGetSmearWgtSecond(this_feature); 
	flags[14] = (short) dbGetSmearWgt2Last(this_feature); 
	flags[15] = (short) dbGetSmearStdDev(this_feature); 

	if (fwrite(flags,sizeof(short),16,features_file) != 16)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"weak edge flags",NO_VAL);
		return(NO_WRITE_FILE);
	}

	for (i = 1; i <= MAX_LED_SECTORS; i++)
	{
		for (j = 1; j <= MAX_LED_SECTORS; j++)
		{
			led_intensity[i - 1][j - 1] = (short) dbGetSmearLedIntensity(this_feature,i,j);
		}
	}
	flags[0] = (short) dbGetSmearLedFlag(this_feature);

	if (fwrite(led_intensity,sizeof(short),(MAX_LED_SECTORS * MAX_LED_RINGS),features_file) != (MAX_LED_SECTORS * MAX_LED_RINGS))
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"weak edge led intensities",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (fwrite(flags,sizeof(short),1,features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"weak edge led flag",NO_VAL);
		return(NO_WRITE_FILE);
	}

	comment = dbGetFeatureComment(this_feature);
	if (comment != NULL)
	{
		comment_length = (unsigned short)(strlen(comment) + 1);
	}
	if (fwrite(&comment_length,sizeof(unsigned short),1,features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature comment length",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (comment_length > 0)
	{
		if (fwrite(comment,sizeof(char),comment_length,features_file) != comment_length)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature comment",NO_VAL);
			return(NO_WRITE_FILE);
		}
	}

	prompt = dbGetFeaturePrompt(this_feature);
	if (prompt != NULL)
	{
		prompt_length = (unsigned short)(strlen(prompt) + 1);
	}
	if (fwrite(&prompt_length,sizeof(unsigned short),1,features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature prompt length",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (prompt_length > 0)
	{
		if (fwrite(prompt,sizeof(char),prompt_length,features_file) != prompt_length)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature prompt",NO_VAL);
			return(NO_WRITE_FILE);
		}
	}

	expression = dbGetFeatureExpression(this_feature);
	if (expression != NULL)
	{
		expression_length = (unsigned short)(strlen(expression) + 1);
	}
	if (fwrite(&expression_length,sizeof(unsigned short),1,features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature expression length",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (expression_length > 0)
	{
		if (fwrite(expression,sizeof(char),expression_length,features_file) != expression_length)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature expression",NO_VAL);
			return(NO_WRITE_FILE);
		}
	}

	for(i = 0; i < MAX_FLAG_OPTIONS; i++)
	{
		temp = dbGetReportLabel(this_feature, i);
		temp_length = 0;
		if (temp != NULL)
		{
			temp_length = (unsigned short)(strlen(temp) + 1);
		}
		if (fwrite(&temp_length,sizeof(unsigned short),1,features_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report label length",NO_VAL);
			return(NO_WRITE_FILE);
		}
		if (temp_length > 0)
		{
			if (fwrite(temp,sizeof(char),temp_length,features_file) != temp_length)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report label",NO_VAL);
				return(NO_WRITE_FILE);
			}
		}
	}

	for(i = 0; i < MAX_FLAG_OPTIONS; i++)
	{
		temp = dbGetExportFieldName(this_feature, i);
		temp_length = 0;
		if (temp != NULL)
		{
			temp_length = (unsigned short)(strlen(temp) + 1);
		}
		if (fwrite(&temp_length,sizeof(unsigned short),1,features_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export fieldname length",NO_VAL);
			return(NO_WRITE_FILE);
		}
		if (temp_length > 0)
		{
			if (fwrite(temp,sizeof(char),temp_length,features_file) != temp_length)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export fieldname",NO_VAL);
				return(NO_WRITE_FILE);
			}
		}
	}

	// Writing of extended data should go below this point!  
	// Best to write and read the extended data type first.
	extended_data_type = (short) dbGetFeatureExtendedDataType(this_feature);
	if (fwrite(&extended_data_type,sizeof(short),1,features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"extended feature type",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if( extended_data_type == EXTENDED_DATA_LASER )
	{
		laser_scan_ints[0] = (short) dbGetLaserPointFrequencyType(this_feature);
		laser_scan_ints[1] = (short) dbGetLaserScanStrategy(this_feature);
		laser_scan_ints[2] = (short) dbGetLaserTrackingScan(this_feature);
		laser_scan_ints[3] = (short) dbGetLaserLineFrequencyType(this_feature);
		laser_scan_ints[4] = (short) dbGetLaserThreshold(this_feature);
		laser_scan_ints[5] = (short) dbGetLaserMaxExposure(this_feature);
		laser_scan_ints[6] = (short) dbGetLaserAverages(this_feature);
		laser_scan_ints[7] = (short) dbGetLaserIDNum(this_feature);
		laser_scan_ints[8] = (short) dbGetLaserScanSpeedFactor(this_feature);
		if (fwrite(laser_scan_ints,sizeof(short),9,features_file) != 9)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"laser scan ints",NO_VAL);
			return(NO_WRITE_FILE);
		}

		laser_scan_doubles[0] = dbGetLaserPointFrequency(this_feature);
		laser_scan_doubles[1] = dbGetLaserLineFrequency(this_feature);
		start_idx = 0;
		for( i = 0 ; i < MAX_LASER_DEFINITION_POINTS ; i++ )
		{		
			laser_scan_doubles[start_idx + 2] = dbGetLaserDefinitionPointX(this_feature, i);
			laser_scan_doubles[start_idx + 3] = dbGetLaserDefinitionPointY(this_feature, i);
			laser_scan_doubles[start_idx + 4] = dbGetLaserDefinitionPointZ(this_feature, i);
			start_idx += 3;
		}
		
		if (fwrite(laser_scan_doubles,sizeof(double),17,features_file) != 17)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"laser scan doubles",NO_VAL);
			return(NO_WRITE_FILE);
		}
	}
	else if( extended_data_type == EXTENDED_DATA_TOUCHPROBE )
	{
		touchprobe_ints[0] = (short) dbGetIsCleanFlag(this_feature);
		touchprobe_ints[1] = (short) dbGetSafeFlag(this_feature);
		touchprobe_ints[2] = (short) dbGetSpacingFlag(this_feature);
		touchprobe_ints[3] = (short) dbGetRowSpacingFlag(this_feature);
		touchprobe_ints[4] = (short) dbGetPlusValue(this_feature);
		touchprobe_ints[5] = (short) dbGetPointsValue(this_feature);
		touchprobe_ints[6] = (short) dbGetInternalValue(this_feature);
		touchprobe_ints[7] = (short) dbGetTotalRowValue(this_feature);
		touchprobe_ints[8] = (short) dbGetTotalScansValue(this_feature);
		touchprobe_ints[9] = (short) dbGetXYAngleFlag(this_feature);
		touchprobe_ints[10] = (short) dbGetElevationFlag(this_feature);
		touchprobe_ints[11] = (short) dbGetTotalLongScansValue(this_feature);
		touchprobe_ints[12] = (short) dbGetNumberBoundaryPoints(this_feature);

		if (fwrite(touchprobe_ints,sizeof(short),13,features_file) != 13)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"touchprobe ints",NO_VAL);
			return(NO_WRITE_FILE);
		}

		touchprobe_doubles[0] = dbGetSafeValue(this_feature);
		touchprobe_doubles[1] = dbGetApproachValue(this_feature);
		touchprobe_doubles[2] = dbGetXYAngleValue(this_feature);
		touchprobe_doubles[3] = dbGetElevationValue(this_feature);
		touchprobe_doubles[4] = dbGetSpacingValue(this_feature);
		touchprobe_doubles[5] = dbGetDistTopValue(this_feature);
		touchprobe_doubles[6] = dbGetDistBottomValue(this_feature);
		touchprobe_doubles[7] = dbGetStartAngleValue(this_feature);
		touchprobe_doubles[8] = dbGetEndAngleValue(this_feature);
		touchprobe_doubles[9] = dbGetRowSpacingValue(this_feature);
		touchprobe_doubles[10] = dbGetTotalRevsValue(this_feature);
		touchprobe_doubles[11] = dbGetStartElevationValue(this_feature);
		touchprobe_doubles[12] = dbGetEndElevationValue(this_feature);

		if (fwrite(touchprobe_doubles,sizeof(double),13,features_file) != 13)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"touchprobe doubles",NO_VAL);
			return(NO_WRITE_FILE);
		}

		for (i = 1; i <= touchprobe_ints[12]; i++)
		{
			touchprobe_doubles[0] = dbGetPlaneBoundaryPointX(this_feature,i);
			touchprobe_doubles[1] = dbGetPlaneBoundaryPointY(this_feature,i);
			touchprobe_doubles[2] = dbGetPlaneBoundaryPointZ(this_feature,i);
			if (fwrite(touchprobe_doubles,sizeof(double),3,features_file) != 3)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"touchprobe doubles",NO_VAL);
				return(NO_WRITE_FILE);
			}
		}
	}

	return(NO_ERROR);
}

int dbWritePointsEntry(Feature *this_feature, FILE *points_file)
{
	char *loc_func_name = "dbWritePointsEntry";
	int i;
	short number_of_points;
	double locz[4];
	short lights[3];
	short target[4];
	short reference_feature;
	unsigned short target_filename_length = 0;
	char *target_filename;
	short edge_seek;
	short side_flag;
	short led_intensity[MAX_LED_SECTORS][MAX_LED_RINGS] = { 0 };
	short led_flag = 0;
	int j,k;

	number_of_points = (short) dbGetNumberPoints(this_feature);
	if (fwrite(&number_of_points,sizeof(short),1,points_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"number of points",NO_VAL);
		return(NO_WRITE_FILE);
	}
	for (i = 1; i <= (int) number_of_points; i++)
	{
		if (dbIsFeatureMeasured(this_feature))
		{
			locz[0] = dbGetPointX(this_feature,i);
			locz[1] = dbGetPointY(this_feature,i);
			locz[2] = dbGetPointZ(this_feature,i);
			locz[3] = dbGetPointZoom(this_feature,i);
			if (fwrite(locz,sizeof(double),4,points_file) != 4)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point location",NO_VAL);
				return(NO_WRITE_FILE);
			}
			locz[0] = dbGetActPointX(this_feature,i);
			locz[1] = dbGetActPointY(this_feature,i);
			locz[2] = dbGetActPointZ(this_feature,i);
			if (fwrite(locz,sizeof(double),3,points_file) != 3)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"actual point location",NO_VAL);
				return(NO_WRITE_FILE);
			}
			lights[0] = (short) dbGetPointBackLight(this_feature,i);
			lights[1] = (short) dbGetPointRingLight(this_feature,i);
			lights[2] = (short) dbGetPointAuxLight(this_feature,i);
			if (fwrite(lights,sizeof(short),3,points_file) != 3)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point lights",NO_VAL);
				return(NO_WRITE_FILE);
			}
			target[0] = (short) dbGetPointTargetType(this_feature,i);
			target[1] = (short) dbGetPointTargetColor(this_feature,i);
			target[2] = (short) dbGetPointTargetSpacing1(this_feature,i);
			target[3] = (short) dbGetPointTargetSpacing2(this_feature,i);
			if (fwrite(target,sizeof(short),4,points_file) != 4)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point target",NO_VAL);
				return(NO_WRITE_FILE);
			}
			target_filename = dbGetPointTargetFilename(this_feature,i);
			if (target_filename != NULL)
			{
				target_filename_length = (unsigned short)(strlen(target_filename) + 1);
			}
			if (fwrite(&target_filename_length,sizeof(unsigned short),1,points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point target filename length",NO_VAL);
				return(NO_WRITE_FILE);
			}
			if (target_filename_length > 0)
			{
				if (fwrite(target_filename,sizeof(char),target_filename_length,points_file) != target_filename_length)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point target filename",NO_VAL);
					return(NO_WRITE_FILE);
				}
			}

			edge_seek = (short) dbGetPointEdgeSeek(this_feature,i);
			if (fwrite(&edge_seek,sizeof(short),1,points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point edge seek",NO_VAL);
				return(NO_WRITE_FILE);
			}
			side_flag = (short) dbGetPointSideNum(this_feature,i);
			if (fwrite(&side_flag,sizeof(short),1,points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point side number",NO_VAL);
				return(NO_WRITE_FILE);
			}
			for (j = 1; j <= MAX_LED_SECTORS; j++)
			{
				for (k = 1; k <= MAX_LED_RINGS; k++)
				{
					led_intensity[j - 1][k - 1] = (short) dbGetPointLedIntensity(this_feature,i,j,k);
				}
			}
			led_flag = (short) dbGetPointLedFlag(this_feature,i);
			if (fwrite(led_intensity,sizeof(short),(MAX_LED_SECTORS * MAX_LED_RINGS),points_file) != (MAX_LED_SECTORS * MAX_LED_RINGS))
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point led intensities",NO_VAL);
				return(NO_WRITE_FILE);
			}
			if (fwrite(&led_flag,sizeof(short),1,points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point led flag",NO_VAL);
				return(NO_WRITE_FILE);
			}
			if (dbFocusParametersExist(this_feature,i))
			{
				target[0] = (short) dbGetFocusBackoff(this_feature,i);
				target[1] = (short) dbGetFocusStepSize(this_feature,i);
				target[2] = (short) dbGetFocusNoise(this_feature,i);
				target[3] = (short) dbGetFocusScanFlag(this_feature,i);
			}
			else
			{
				target[0] = target[1] = target[2] = target[3] = 0;
			}
			if (fwrite(target,sizeof(short),4,points_file) != 4)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point f2 info",NO_VAL);
				return(NO_WRITE_FILE);
			}
			locz[0] = dbGetCompPointX(this_feature,i);
			locz[1] = dbGetCompPointY(this_feature,i);
			locz[2] = dbGetCompPointZ(this_feature,i);
			if (fwrite(locz,sizeof(double),3,points_file) != 3)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"compensated point location",NO_VAL);
				return(NO_WRITE_FILE);
			}
			locz[0] = dbGetPointDirectionVectorX(this_feature,i);
			locz[1] = dbGetPointDirectionVectorY(this_feature,i);
			locz[2] = dbGetPointDirectionVectorZ(this_feature,i);
			if (fwrite(locz,sizeof(double),3,points_file) != 3)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"point vector",NO_VAL);
				return(NO_WRITE_FILE);
			}
		}
		else
		{
			reference_feature = (short) dbGetRefFeatureNumber(this_feature,i);
			if (fwrite(&reference_feature,sizeof(short),1,points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"reference feature number",NO_VAL);
				return(NO_WRITE_FILE);
			}
		}
	}
	number_of_points = (short) dbGetNumberMeasuredPoints(this_feature);
	if (fwrite(&number_of_points,sizeof(short),1,points_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"number measured points",NO_VAL);
		return(NO_WRITE_FILE);
	}
	return(NO_ERROR);
}

int dbWriteActualsEntry(Feature *this_feature, FILE *actuals_file)
{
	char *loc_func_name = "dbWriteActualsEntry";
	short number_of[2] = { 0 };
	double actual_loc_size[19] = { 0.0 };
	short i;

	number_of[0] = (short) dbGetNumberLocations(this_feature);
	number_of[1] = (short) dbGetNumberSizes(this_feature);
	if (fwrite(number_of,sizeof(short),2,actuals_file) != 2)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"actual flags",NO_VAL);
		return(NO_WRITE_FILE);
	}
	actual_loc_size[0] = dbGetActualLocX(this_feature);
	actual_loc_size[1] = dbGetActualLocY(this_feature);
	actual_loc_size[2] = dbGetActualLocZ(this_feature);
	if (number_of[0] == 2)
	{
		actual_loc_size[3] = dbGetSecondLocX(this_feature);
		actual_loc_size[4] = dbGetSecondLocY(this_feature);
		actual_loc_size[5] = dbGetSecondLocZ(this_feature);
	}
	else
	{
		actual_loc_size[3] = 0.0;
		actual_loc_size[4] = 0.0;
		actual_loc_size[5] = 0.0;
	}
	for (i = 1; i <= 4; i++)
	{
		if (i <= number_of[1])
		{
			actual_loc_size[5 + i] = dbGetActualSize(this_feature,i);
		}
		else
		{
			actual_loc_size[5 + i] = 0.0;
		}
	}
	actual_loc_size[10] = dbGetActualDeviation(this_feature);
	actual_loc_size[11] = dbGetDirectionX(this_feature);
	actual_loc_size[12] = dbGetDirectionY(this_feature);
	actual_loc_size[13] = dbGetDirectionZ(this_feature);
	for (i = 1; i <= MAX_ANGLES; i++)
	{	// 14, 15, 16
		actual_loc_size[13 + i] = dbGetActualOrientation(this_feature,i);
	}
	actual_loc_size[17] = dbGetActualPosTol(this_feature);
	actual_loc_size[18] = dbGetModifiedPosTol(this_feature);
	if (fwrite(actual_loc_size,sizeof(double),19,actuals_file) != 19)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"actuals",NO_VAL);
		return(NO_WRITE_FILE);
	}
	return(NO_ERROR);
}

int dbWriteNominalsEntry(Feature *this_feature, FILE *nominals_file)
{
	char *loc_func_name = "dbWriteNominalsEntry";
	double nom_tols[17];
	int int_flags[2];
	short short_flags[4];
	short search_steps[3];
	int i;

	nom_tols[0] = dbGetNominalLocX(this_feature);
	nom_tols[1] = dbGetNominalLocY(this_feature);
	nom_tols[2] = dbGetNominalLocZ(this_feature);

	nom_tols[3] = dbGetUpperLocTolX(this_feature);
	nom_tols[4] = dbGetLowerLocTolX(this_feature);
	nom_tols[5] = dbGetUpperLocTolY(this_feature);
	nom_tols[6] = dbGetLowerLocTolY(this_feature);
	nom_tols[7] = dbGetUpperLocTolZ(this_feature);
	nom_tols[8] = dbGetLowerLocTolZ(this_feature);

	nom_tols[9] = dbGetNominalSize(this_feature);
	nom_tols[10] = dbGetUpperSizeTol(this_feature);
	nom_tols[11] = dbGetLowerSizeTol(this_feature);

	nom_tols[12] = dbGetNominalSize2(this_feature);
	nom_tols[13] = dbGetUpperSize2Tol(this_feature);
	nom_tols[14] = dbGetLowerSize2Tol(this_feature);

	nom_tols[15] = dbGetNominalFormTol(this_feature);
	nom_tols[16] = dbGetNominalPosTol(this_feature);
	dbGetNominalFlags(this_feature,&int_flags[0],&int_flags[1]);
	short_flags[0] = (short) int_flags[0];
	short_flags[1] = (short) int_flags[1];
	short_flags[2] = (short) dbGetNominalsEntered(this_feature);
	short_flags[3] = (short) dbGetToleranceType(this_feature);

	if (fwrite(nom_tols,sizeof(double),17,nominals_file) != 17)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"nominals",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (fwrite(short_flags,sizeof(short),4,nominals_file) != 4)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"nominal flags",NO_VAL);
		return(NO_WRITE_FILE);
	}

	nom_tols[0] = dbGetNominalElevationAngle(this_feature);
	nom_tols[1] = dbGetUpperElevationTol(this_feature);
	nom_tols[2] = dbGetLowerElevationTol(this_feature);
	nom_tols[3] = dbGetNominalSize3(this_feature);
	nom_tols[4] = dbGetUpperSize3Tol(this_feature);
	nom_tols[5] = dbGetLowerSize3Tol(this_feature);

	if (fwrite(nom_tols,sizeof(double),6,nominals_file) != 6)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"nominals",NO_VAL);
		return(NO_WRITE_FILE);
	}

	for (i = 1; i <= 3; i++)
	{
		search_steps[i - 1] = (short) dbGetSearchOnStep(this_feature, i);
	}
	if (fwrite(search_steps,sizeof(short),3,nominals_file) != 3)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"search steps",NO_VAL);
		return(NO_WRITE_FILE);
	}
	return(NO_ERROR);
}

int dbWriteTransformEntry(Feature *this_feature, FILE *routine_file)
{
	char *loc_func_name = "dbWriteTransformEntry";
	Database_Transform part_transform;

	dbGetFeaturePartTransform(this_feature, &part_transform);
	if (fwrite(&part_transform,sizeof(Database_Transform),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"part transform",NO_VAL);
		return(NO_WRITE_FILE);
	}
	return(NO_ERROR);
}

int dbWriteRepeatOffsets(FILE *out_file)
{
	char *loc_func_name = "dbWriteRepeatOffsets";
	short temp = 0;
	short repeat_this_offset[MAX_REPEAT_OFFSETS] = { 0 };
	int i;

	temp = (short) gb_fixture_repeat;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"fixture repeat flag",NO_VAL);
		return(NO_WRITE_FILE);
	}
	for (i = 0; i < MAX_REPEAT_OFFSETS; i++)
		repeat_this_offset[i] = (short) gb_repeat_this_offset[i];
	if (fwrite(repeat_this_offset,sizeof(short),MAX_REPEAT_OFFSETS,out_file) != MAX_REPEAT_OFFSETS)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat offset flags",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_repeat_x_offsets,sizeof(double),MAX_REPEAT_OFFSETS,out_file) != MAX_REPEAT_OFFSETS)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat x offsets",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_repeat_y_offsets,sizeof(double),MAX_REPEAT_OFFSETS,out_file) != MAX_REPEAT_OFFSETS)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat y offsets",NO_VAL);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_repeat_confirm_next;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat confirm next",NO_VAL);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_part_repeat;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"part repeat flag",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (fwrite(&gb_x_part_offset,sizeof(double),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"x part repeat offset",NO_VAL);
		return(NO_WRITE_FILE);
	}
	if (fwrite(&gb_y_part_offset,sizeof(double),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"y part repeat offset",NO_VAL);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_x_part_times;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"x part number times",NO_VAL);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_y_part_times;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"y part number times",NO_VAL);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_repeat_y_first;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat y first flag",NO_VAL);
		return(NO_WRITE_FILE);
	}
	
	temp = (short) gb_repeat_finish_run;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat finish run",NO_VAL);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_repeat_start_step;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat start step",NO_VAL);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_repeat_end_step;
	if (fwrite(&temp,sizeof(short),1,out_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat end step",NO_VAL);
		return(NO_WRITE_FILE);
	}
		
	return(NO_ERROR);
}

int dbSaveDataBase(char *routine_name)
{
	char *loc_func_name = "dbSaveDataBase";
	FILE *routine_file;
	Feature *this_feature = NULL;
	short last_feat_num = 0;
	int i;
	unsigned short j;
	short temp;
	char ramdrive[_MAX_PATH];
	char filename[_MAX_PATH];
	FILE *temp_file;
	int file_handle;

	if (getEnvVar(ramdrive,"TEMP", TEMP_DEFAULT) == NO_ERROR)
	{
		strcpy(filename,ramdrive);				  /* set the output file path */
		strcat(filename,"temp.rtn");		/* append the temporary file name */
		routine_file = fopen(filename,"wb");		   /* open it for writing */
	}
	else
	{
		routine_file = fopen(routine_name,"wb");	 /* open the routine file */
	}
	if (routine_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,routine_name,NO_VAL);
		return(OPEN_FAILED);
	}

	if (fwrite(ID_STRING,sizeof(char),strlen(ID_STRING),routine_file) != strlen(ID_STRING))
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,routine_name,NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	gb_db_version = DATABASE_VERSION;
	temp = (short) gb_db_version;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"database version",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	last_feat_num = (short) dbGetLastFeatureNumber();
	if (fwrite(&last_feat_num,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"last feature number",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	for (i = 1; i <= (int) last_feat_num; i++)
	{
		if ((this_feature = dbTrustedGetFeatureFromDB(i)) == NULL)
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"feature",i);
			(void) fclose(routine_file);
			return(GET_FROM_DB_FAILED);
		}

		if (dbMinimumFeaturePoints(this_feature) == FALSE)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"feature",i);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}

		if (dbWriteFeatureEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"features",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}

		if (dbWritePointsEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"points",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}

		if (dbWriteActualsEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"actuals",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}

		if (dbWriteNominalsEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"nominals",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}
		if (dbWriteTransformEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"transform",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}
	}

	for (i = 0; i < NUM_HEADER_LINES; i++)
	{
		if (fwrite(gb_run_header_txt[i],sizeof(char),81,routine_file) != 81)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"header line",i);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}
	}
	for (i = 0; i < NUM_SETUP_LINES; i++)
	{
		if (fwrite(gb_run_setup_txt[i],sizeof(char),81,routine_file) != 81)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"setup",i);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}
	}

	if (dbWriteRepeatOffsets(routine_file) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"repeat data",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	j = (unsigned short)(strlen(gb_stat_filename) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stat filename length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_stat_filename,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stat filename",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_keep_stat_filename;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"keep stat filename",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_display_run_windows;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"display windows",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_stop_out_of_tol;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stop out of tol",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_printer_override;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"printer override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_auto_edge_enabled;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"auto edge enable",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_print_to_file;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"print to file",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (gb_print_to_file == TRUE)
	{
		j = (unsigned short)(strlen(gb_printer_filename) + 1);
		if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"print filename length",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}
		if (fwrite(gb_printer_filename,sizeof(char),j,routine_file) != j)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"print filename",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}
	}
	temp = (short) gb_done_during_run;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"done during run",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_missed_edge_override;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"missed edge override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_print_run_time;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"print run time",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	j = (unsigned short) strlen(gb_template_filename);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"template filename length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (j > 0) 
	{
		if (fwrite(gb_template_filename,sizeof(char),j + 1,routine_file) != (unsigned short)(j + 1))
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"template filename",NO_VAL);
			(void) fclose(routine_file);
			return(NO_WRITE_FILE);
		}
	}		

	j = (unsigned short)(strlen(gb_stream_filename) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stream filename length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_stream_filename,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stream filename",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_keep_stream_filename;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"keep stream filename",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_stat_override;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stat override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_stream_override;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stream override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

		
	temp = (short) gb_report_override;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report output override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	j = (unsigned short)(strlen(gb_report_type_path_override) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report type path override length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_report_type_path_override,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report type path override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_report_type_path_override_on;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report path override on",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_sample_size_override;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"sample size override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_sample_size_override_on;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"sample size override on",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_export_override;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"data export override",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_report_sample_size;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report sample size",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_report_precision;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report precision",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	temp = (short) gb_report_out_of_spec;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"print report out of spec",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	j = (unsigned short)(strlen(gb_report_type_path) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report type path length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_report_type_path,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"report type path",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	j = (unsigned short)(strlen(gb_export_type_path) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export type path length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_export_type_path,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export type path",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	
	j = (unsigned short)(strlen(gb_export_table_name) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export table name length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_export_table_name,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export table name",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	j = (unsigned short)(strlen(gb_export_file_type) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export file type length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_export_file_type,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export file type",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_export_every_part;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export every part",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_rotary_in_use;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"rotary in use",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	temp = (short) gb_export_precision;
	if (fwrite(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"export precision",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	j = (unsigned short)(strlen(gb_stream_template_filename) + 1);
	if (fwrite(&j,sizeof(unsigned short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stream template filename length",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}
	if (fwrite(gb_stream_template_filename,sizeof(char),j,routine_file) != j)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"stream template filename",NO_VAL);
		(void) fclose(routine_file);
		return(NO_WRITE_FILE);
	}

	if (fclose(routine_file) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,CLOSE_FAILED,routine_name,NO_VAL);
		return(CLOSE_FAILED);
	}

	if (getEnvVar(ramdrive, "TEMP", TEMP_DEFAULT) == NO_ERROR)
	{
		temp_file = fopen(filename,"rb");			   /* open it for writing */
		routine_file = fopen(routine_name,"wb");	 /* open the routine file */
		if ( (temp_file == NULL) || (routine_file == NULL) )
		{
			errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,filename,NO_VAL);
			return(OPEN_FAILED);
		}
		/* copy contents of routine file to temporary file */
		file_handle = fileno(temp_file);
		if (dbCopyBytes(temp_file,routine_file,filelength(file_handle)) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_COPY_FILE,filename,NO_VAL);
			(void) fclose(temp_file);
			(void) fclose(routine_file);
			return(NO_COPY_FILE);
		}
		fclose(temp_file);
		fclose(routine_file);
	}

	return(NO_ERROR);
}

int dbReadFeatureEntry(Feature *this_feature, FILE *features_file)
{
	char *loc_func_name = "dbReadFeatureEntry";
	short flags[19] = { 0 };
	double dimensions[2] = { 0.0 };
	unsigned short num_dimensions = 1;
	unsigned short comment_length = 0;
	char *comment = NULL;
	unsigned short prompt_length = 0;
	char *prompt = NULL;
	unsigned int num_smear = 10;
	double smear[16] = { 0.0 };
	unsigned int num_flags = 9;
	unsigned short expression_length = 0;
	char *expression = NULL;
	short old_led_intensity[MAX_LED_SECTORS] = { 0 };
	short old_led_state[MAX_LED_SECTORS] = { 0 };
	short led_intensity[MAX_LED_SECTORS][MAX_LED_RINGS] = { 0 };
	int i,j,start_idx;
	unsigned short temp_length = 0;
	char *temp = NULL;
	int num_labels;
	short laser_scan_ints[9] = { 0 };
	double laser_scan_doubles[17] = { 0.0 };
	short touchprobe_ints[13] = { 0 };
	double touchprobe_doubles[13] = { 0.0 };
	int extended_data_type = 0;
	double basicDimension;
	int number_scan_def_points_needed = 0;

	if (gb_db_version >= 7)
		num_flags = 10;

	if (gb_db_version >= 10)
		num_flags = 12;

	if (gb_db_version >= 12)
		num_flags = 13;
	
	if (gb_db_version >= 15)
		num_flags = 15;

	if (gb_db_version >= 16)
		num_flags = 16;

	if (gb_db_version >= 22)
		num_flags = 17;

	if (gb_db_version >= 23)
		num_flags = 18;

	if (gb_db_version >= 35)
		num_flags = 19;

	if (fread(flags,sizeof(short),num_flags,features_file) != num_flags)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"feature flags",NO_VAL);
		return(NO_READ_FILE);
	}
	dbOverrideFeatureNumber(this_feature,(int) flags[0]);
	if (dbPutFeatureType(this_feature,(int) flags[1]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature type",(int) flags[1]);
		return(PUT_FAILED);
	}
	if (dbPutFeaturePlane(this_feature,(int) flags[2]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature plane",(int) flags[2]);
		return(PUT_FAILED);
	}
	if (dbPutInMmFlag(this_feature,(int) flags[3]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"in / mm flag",(int) flags[3]);
		return(PUT_FAILED);
	}
	if (dbPutCartPolarFlag(this_feature,(int) flags[4]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"cart / polar flag",(int) flags[4]);
		return(PUT_FAILED);
	}
	if (dbPutFeatureMode(this_feature,(int) flags[5]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature mode",(int) flags[5]);
		return(PUT_FAILED);
	}
	if (dbPutStatisticsFlags(this_feature,(int) flags[6]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"statistics flags",(int) flags[6]);
		return(PUT_FAILED);
	}
	if (dbPutPrintFlags(this_feature,(int) flags[7]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"print flags",(int) flags[7]);
		return(PUT_FAILED);
	}
	if (dbPutLastDisplay(this_feature,(int) flags[8]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"last display",(int) flags[8]);
		return(PUT_FAILED);
	}
	if (gb_db_version >= 7)
	{
		if (dbPutDecDmsFlag(this_feature,(int) flags[9]) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"dec / dms flag",(int) flags[9]);
			return(PUT_FAILED);
		}
	}
		
	if (gb_db_version >= 10)
	{
		dbPutDataStreamFlag(this_feature, (int) flags[10]);
		dbPutMiscFlag(this_feature, (int) flags[11]);
	}

	if (gb_db_version >= 12)
	{
		if (dbPutFeatureLensCode(this_feature,(int) flags[12]) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"lens code",(int) flags[12]);
			return(PUT_FAILED);
		}
	}
	else
	{
		if (dbPutFeatureLensCode(this_feature,LENS_NO_CODE) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"lens code",(int) flags[12]);
			return(PUT_FAILED);
		}
	}

	if (gb_db_version >= 15)
	{
		if (dbPutReportFlags(this_feature,(int) flags[13]) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"report output flags",(int) flags[13]);
			return(PUT_FAILED);
		}

		if (dbPutExportFlags(this_feature,(int) flags[14]) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"data export flags",(int) flags[14]);
			return(PUT_FAILED);
		}
	}

	if (gb_db_version >= 16)
	{
		dbPutFeatureDockLocation(this_feature,(int) flags[15]);
	}

	if (gb_db_version >= 22)
	{
		dbPutLastDisplay2(this_feature,(int) flags[16]);
	}
	if (gb_db_version >= 23)
	{
		dbPutLastDisplay3(this_feature,(int) flags[17]);
	}
	
	if (gb_db_version >= 35)
	{
		dbPutFeatureDeployLaserFlag(this_feature,(int) flags[18]);
	}

	if (gb_db_version >= 17)
		num_dimensions = 2;
	if (fread(dimensions,sizeof(double),num_dimensions,features_file) != num_dimensions)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
		return(NO_READ_FILE);
	}
	if (dbPutBasicDimension(this_feature,dimensions[0]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"basic dimension",NO_VAL);
		return(PUT_FAILED);
	}
	if (gb_db_version >= 17)
	{
		if (dbPutExtraDimension(this_feature,dimensions[1]) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"extra dimension",NO_VAL);
			return(PUT_FAILED);
		}
	}

	if (gb_db_version >= 6)
	{
		if (gb_db_version >= 26)
			num_smear = 13;
		if (gb_db_version >= 27)
			num_smear = 16;
		if (fread(smear,sizeof(double),num_smear,features_file) != num_smear)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge points",NO_VAL);
			return(NO_READ_FILE);
		}
		dbPutSmearCenterX(this_feature,smear[0]);
		dbPutSmearCenterY(this_feature,smear[1]);
		dbPutSmearCenterZ(this_feature,smear[2]);
		dbPutSmearZoom(this_feature,smear[3]);
		dbPutSmearStartPtX(this_feature,smear[4]);
		dbPutSmearStartPtY(this_feature,smear[5]);
		dbPutSmearEndPtX(this_feature,smear[6]);
		dbPutSmearEndPtY(this_feature,smear[7]);
		dbPutSmearMidPtX(this_feature,smear[8]);
		dbPutSmearMidPtY(this_feature,smear[9]);
		if (gb_db_version >= 26)
		{
			dbPutSmearStartPtZ(this_feature,smear[10]);
			dbPutSmearEndPtZ(this_feature,smear[11]);
			dbPutSmearMidPtZ(this_feature,smear[12]);
		}
		else
		{	// use center z for start, end and middle z
			dbPutSmearStartPtZ(this_feature,smear[2]);
			dbPutSmearEndPtZ(this_feature,smear[2]);
			dbPutSmearMidPtZ(this_feature,smear[2]);
		}
		if (gb_db_version >= 27)
		{
			dbPutSmearDirectionVectorX(this_feature,smear[13]);
			dbPutSmearDirectionVectorY(this_feature,smear[14]);
			dbPutSmearDirectionVectorZ(this_feature,smear[15]);
		}

		if (fread(flags,sizeof(short),16,features_file) != 16)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge flags",NO_VAL);
			return(NO_READ_FILE);
		}
		dbPutSmearType(this_feature,(int) flags[0]); 
		dbPutSmearBackLight(this_feature,(int) flags[1]); 
		dbPutSmearRingLight(this_feature,(int) flags[2]); 
		dbPutSmearAuxLight(this_feature,(int) flags[3]); 
		dbPutSmearDirection(this_feature,(int) flags[4]); 
		dbPutSmearBounds(this_feature,(int) flags[5]); 
		dbPutSmearExtent(this_feature,(int) flags[6]); 
		dbPutSmearMinQuality(this_feature,(int) flags[7]); 
		dbPutSmearWgtContrast(this_feature,(int) flags[8]); 
		dbPutSmearWgtNominal(this_feature,(int) flags[9]); 
		dbPutSmearContrast(this_feature,(int) flags[10]); 
		dbPutSmearWgtFirst(this_feature,(int) flags[11]); 
		dbPutSmearWgtLast(this_feature,(int) flags[12]); 
		dbPutSmearWgtSecond(this_feature,(int) flags[13]); 
		dbPutSmearWgt2Last(this_feature,(int) flags[14]); 
		dbPutSmearStdDev(this_feature,(int) flags[15]); 
	}

	if (gb_db_version == 13)
	{	/* OLD LED STORAGE */
		if (fread(old_led_intensity,sizeof(short),MAX_LED_SECTORS,features_file) != MAX_LED_SECTORS)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge led intensities",NO_VAL);
			return(NO_READ_FILE);
		}
		if (fread(old_led_state,sizeof(short),MAX_LED_SECTORS,features_file) != MAX_LED_SECTORS)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge led states",NO_VAL);
			return(NO_READ_FILE);
		}
		for (i = 1; i <= MAX_LED_SECTORS; i++)
		{
			for (j = 1; j <= MAX_LED_RINGS; j++)
			{
				if (old_led_state[i] & (1 << (j - 1)))
				{
					if (dbPutSmearLedIntensity(this_feature,i,j,(int) old_led_intensity[i - 1]) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"weak edge led intensity",i);
						return(PUT_FAILED);
					}
				}
			}
		}
	}

	if (gb_db_version >= 14)
	{
		if (fread(led_intensity,sizeof(short),(MAX_LED_SECTORS * MAX_LED_RINGS),features_file) != (MAX_LED_SECTORS * MAX_LED_RINGS))
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge led intensities",NO_VAL);
			return(NO_READ_FILE);
		}
		if (fread(flags,sizeof(short),1,features_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge led flag",NO_VAL);
			return(NO_READ_FILE);
		}
		for (i = 1; i <= MAX_LED_SECTORS; i++)
		{
			for (j = 1; j <= MAX_LED_RINGS; j++)
			{
				if (dbPutSmearLedIntensity(this_feature,i,j,(int)led_intensity[i - 1][j - 1]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"weak edge led intensity",i);
					return(PUT_FAILED);
				}
			}
		}
		dbPutSmearLedFlag(this_feature,flags[0]);
	}

	if (fread(&comment_length,sizeof(unsigned short),1,features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"feature comment length",NO_VAL);
		return(NO_READ_FILE);
	}
	if (comment_length > 0)
	{
		comment = (char *)calloc(comment_length,sizeof(char));
		if (comment == NULL)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"feature comment",(int)comment_length);
			return(NO_MEMORY);
		}
		if (fread(comment,sizeof(char),comment_length,features_file) != comment_length)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"feature comment",NO_VAL);
			free(comment);
			return(NO_READ_FILE);
		}
		if (dbPutFeatureComment(this_feature,comment) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature comment",(int) comment_length);
			free(comment);
			return(PUT_FAILED);
		}
		free(comment);
	}

	if (fread(&prompt_length,sizeof(unsigned short),1,features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"feature prompt length",NO_VAL);
		return(NO_READ_FILE);
	}
	if (prompt_length > 0)
	{
		prompt = (char *)calloc(prompt_length,sizeof(char));
		if (prompt == NULL)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"feature prompt",(int)prompt_length);
			return(NO_MEMORY);
		}
		if (fread(prompt,sizeof(char),prompt_length,features_file) != prompt_length)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"feature prompt",NO_VAL);
			free(prompt);
			return(NO_READ_FILE);
		}
		if (dbPutFeaturePrompt(this_feature,prompt) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature prompt",(int) prompt_length);
			free(prompt);
			return(PUT_FAILED);
		}
		free(prompt);
	}

	if (gb_db_version >= 9)
	{
		if (fread(&expression_length,sizeof(unsigned short),1,features_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"feature expression length",NO_VAL);
			return(NO_READ_FILE);
		}
		if (expression_length > 0)
		{
			expression = (char *)calloc(expression_length,sizeof(char));
			if (expression == NULL)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"feature expression",(int)expression_length);
				return(NO_MEMORY);
			}
			if (fread(expression,sizeof(char),expression_length,features_file) != expression_length)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"feature expression",NO_VAL);
				free(expression);
				return(NO_READ_FILE);
			}
			if (dbPutFeatureExpression(this_feature,expression) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature expression",(int) expression_length);
				free(expression);
				return(PUT_FAILED);
			}
			free(expression);
		}
		if (gb_db_version >= 15)
		{
			num_labels = MAX_FLAG_OPTIONS;
			if (gb_db_version < 25)
				num_labels = 7;
			if (gb_db_version < 23)
				num_labels = 6;
			for(i = 0; i < num_labels; i++)
			{
				if (fread(&temp_length,sizeof(unsigned short),1,features_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report label length",NO_VAL);
					return(NO_READ_FILE);

				}
				if (temp_length > 0)
				{
					temp = (char *)calloc(temp_length,sizeof(char));
					if (temp == NULL)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"report label",(int)temp_length);
						return(NO_MEMORY);
					}
					if (fread(temp,sizeof(char),temp_length,features_file) != temp_length)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report label",NO_VAL);
						free(temp);
						return(NO_READ_FILE);

					}
					if (dbPutReportLabel(this_feature,temp,i) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"report label",(int) temp_length);
						free(temp);
						return(PUT_FAILED);
					}
					free(temp);
				}
			}
			for(i = 0; i < num_labels; i++)
			{
				if (fread(&temp_length,sizeof(unsigned short),1,features_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export fieldname length",NO_VAL);
					return(NO_READ_FILE);
				}
				if (temp_length > 0)
				{
					temp = (char *)calloc(temp_length,sizeof(char));
					if (temp == NULL)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"export fieldname",(int)temp_length);
						return(NO_MEMORY);
					}
					if (fread(temp,sizeof(char),temp_length,features_file) != temp_length)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export labefieldname",NO_VAL);
						free(temp);
						return(NO_READ_FILE);
					}
					if (dbPutExportFieldName(this_feature,temp,i) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"export fieldname",(int) temp_length);
						free(temp);
						return(PUT_FAILED);
					}
					free(temp);
				}
			}
		}
	}
	if( gb_db_version >= 30 )
	{
		if(fread(&extended_data_type,sizeof(short),1,features_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"extended data type",NO_VAL);
			return(NO_READ_FILE);
		}
		if(dbPutFeatureExtendedDataType(this_feature, extended_data_type))
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"extended data type",0);
			return(PUT_FAILED);
		}
		
		if( extended_data_type == EXTENDED_DATA_LASER )
		{
			if(gb_db_version >= 38)
			{
				if(fread(laser_scan_ints,sizeof(short),9,features_file) != 9)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"laser scan ints",NO_VAL);
					return(NO_READ_FILE);
				}
			}
			else
			{
				if(fread(laser_scan_ints,sizeof(short),8,features_file) != 8)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"laser scan ints",NO_VAL);
					return(NO_READ_FILE);
				}
			}			
			if(dbPutLaserPointFrequencyType(this_feature, laser_scan_ints[0]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",0);
				return(PUT_FAILED);
			}
			if(dbPutLaserScanStrategy(this_feature, laser_scan_ints[1]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",1);
				return(PUT_FAILED);
			}
			if(dbPutLaserTrackingScan(this_feature, laser_scan_ints[2]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",2);
				return(PUT_FAILED);
			}
			if(dbPutLaserLineFrequencyType(this_feature, laser_scan_ints[3]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",3);
				return(PUT_FAILED);
			}
			if(dbPutLaserThreshold(this_feature, laser_scan_ints[4]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",4);
				return(PUT_FAILED);
			}
			if(dbPutLaserMaxExposure(this_feature, laser_scan_ints[5]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",5);
				return(PUT_FAILED);
			}
			if(dbPutLaserAverages(this_feature, laser_scan_ints[6]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",7);
				return(PUT_FAILED);
			}		
			if(dbPutLaserIDNum(this_feature, laser_scan_ints[7]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",7);
				return(PUT_FAILED);
			}
			if(gb_db_version >= 38)
			{
				if(dbPutLaserScanSpeedFactor(this_feature, laser_scan_ints[8]))
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser parameters",8);
					return(PUT_FAILED);
				}
			}
			if(fread(laser_scan_doubles,sizeof(double),17,features_file) != 17)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"laser scan doubles",NO_VAL);
				return(NO_READ_FILE);
			}			
			if(dbPutLaserPointFrequency(this_feature, laser_scan_doubles[0]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser scan doubles",0);
				return(PUT_FAILED);
			}
			if(dbPutLaserLineFrequency(this_feature, laser_scan_doubles[1]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser scan doubles",1);
				return(PUT_FAILED);
			}
			start_idx = 0;
			switch(laser_scan_ints[1])
			{
				case LASER_LINEAR_SCAN:
				{
					number_scan_def_points_needed = LASER_NUM_LINEAR_SCAN_POINTS;
					break;
				}
				case LASER_ARC_SCAN:
				case LASER_CIRCULAR_SCAN:
				{
					number_scan_def_points_needed = LASER_NUM_CIRCULAR_SCAN_POINTS;
					break;
				}
				case LASER_AREA_SCAN:
				case LASER_BOX_SCAN:
				{
					number_scan_def_points_needed = LASER_NUM_CIRCULAR_SCAN_POINTS;
					break;
				}
				case LASER_SINE_SCAN:
				{
					number_scan_def_points_needed = LASER_NUM_SINE_SCAN_POINTS;
					break;
				}
				case LASER_RASTER_SCAN:
				{
					number_scan_def_points_needed = LASER_NUM_RASTER_SCAN_POINTS;
					break;
				}
				case LASER_SPIRAL_SCAN:
				{
					number_scan_def_points_needed = LASER_NUM_SPIRAL_SCAN_POINTS;
					break;
				}
				default:
				{
					number_scan_def_points_needed = LASER_NUM_INVALID_SCAN_POINTS;
					break;
				}
			}
			for( i = 0 ; i < number_scan_def_points_needed ; i++ )
			{	
				if(dbPutLaserDefinitionPointX(this_feature, i, laser_scan_doubles[start_idx + 2]))
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser scan doubles",start_idx + 2);
					return(PUT_FAILED);
				}
				if(dbPutLaserDefinitionPointY(this_feature, i, laser_scan_doubles[start_idx + 3]))
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser scan doubles",start_idx + 3);
					return(PUT_FAILED);
				}
				if(dbPutLaserDefinitionPointZ(this_feature, i, laser_scan_doubles[start_idx + 4]))
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser scan doubles",start_idx + 4);
					return(PUT_FAILED);
				}
				start_idx += 3;
			}
		}
		else if ( extended_data_type == EXTENDED_DATA_TOUCHPROBE )
		{
			unsigned int num_ints = 1, num_doubles = 3;

			if (gb_db_version >= 32)
			{
				num_ints = 11;
				num_doubles = 11;
				
				if (gb_db_version >= 33)
				{
					num_doubles = 13;
				}
				if (gb_db_version >= 34)
				{
					num_ints = 12;
				}
				if (gb_db_version >= 36)
				{
					num_ints = 13;
				}
			}
			if(fread(touchprobe_ints,sizeof(short),num_ints,features_file) != num_ints)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"touchprobe ints",NO_VAL);
				return(NO_READ_FILE);
			}
			if(fread(touchprobe_doubles,sizeof(double),num_doubles,features_file) != num_doubles)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"touchprobe doubles",NO_VAL);
				return(NO_READ_FILE);
			}			
			if(dbPutIsCleanFlag(this_feature, touchprobe_ints[0]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe isClean parameter",0);
				return(PUT_FAILED);
			}
			if(dbPutSafeFlag(this_feature, touchprobe_ints[1]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}
			if(dbPutSpacingFlag(this_feature, touchprobe_ints[2]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}
			if(dbPutRowSpacingFlag(this_feature, touchprobe_ints[3]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}			
			if(dbPutPlusValue(this_feature, touchprobe_ints[4]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe plus parameter",2);
				return(PUT_FAILED);
			}
			if(dbPutPointsValue(this_feature, touchprobe_ints[5]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe points parameter",3);
				return(PUT_FAILED);
			}
			if(dbPutInternalValue(this_feature, touchprobe_ints[6]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe plus parameter",4);
				return(PUT_FAILED);
			}
			if(dbPutTotalRowValue(this_feature, touchprobe_ints[7]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe row parameter",4);
				return(PUT_FAILED);
			}
			if(dbPutTotalScansValue(this_feature, touchprobe_ints[8]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe scans parameter",4);
				return(PUT_FAILED);
			}
			if(dbPutXYAngleFlag(this_feature, touchprobe_ints[9]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}		
			if(dbPutElevationFlag(this_feature, touchprobe_ints[10]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}
			if(dbPutTotalLongScansValue(this_feature, touchprobe_ints[11]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe scans parameter",4);
				return(PUT_FAILED);
			}
			/* Doubles */
			if(dbPutSafeValue(this_feature, touchprobe_doubles[0]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe parameter",0);
				return(PUT_FAILED);
			}
			if(dbPutApproachValue(this_feature, touchprobe_doubles[1]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe approach parameter",1);
				return(PUT_FAILED);
			}
			if(gb_db_version >= 32)
			{
				if(dbPutXYAngleValue(this_feature, touchprobe_doubles[2]))
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe elevation parameter",3);
					return(PUT_FAILED);
				}
			}
			if(dbPutElevationValue(this_feature, touchprobe_doubles[3]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe elevation parameter",3);
				return(PUT_FAILED);
			}
			if(dbPutSpacingValue(this_feature, touchprobe_doubles[4]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe spacing parameter",4);
				return(PUT_FAILED);
			}
			if(dbPutDistTopValue(this_feature, touchprobe_doubles[5]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe dist from top parameter",5);
				return(PUT_FAILED);
			}
			if(dbPutDistBottomValue(this_feature, touchprobe_doubles[6]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe elevation parameter",6);
				return(PUT_FAILED);
			}
			if(dbPutStartAngleValue(this_feature, touchprobe_doubles[7]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe start angle parameter",7);
				return(PUT_FAILED);
			}
			if(dbPutEndAngleValue(this_feature, touchprobe_doubles[8]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe end angle parameter",8);
				return(PUT_FAILED);
			}
			if(dbPutRowSpacingValue(this_feature, touchprobe_doubles[9]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe row spacing parameter",9);
				return(PUT_FAILED);
			}
			if(dbPutTotalRevsValue(this_feature, touchprobe_doubles[10]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe scans parameter",10);
				return(PUT_FAILED);
			}
			if(dbPutStartElevationValue(this_feature, touchprobe_doubles[11]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe start elevation parameter",7);
				return(PUT_FAILED);
			}
			if(dbPutEndElevationValue(this_feature, touchprobe_doubles[12]))
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe end elevation parameter",8);
				return(PUT_FAILED);
			}
			if (gb_db_version >= 36)
			{
				if (dbPutNumberBoundaryPoints(this_feature,touchprobe_ints[12]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe boundary points",touchprobe_ints[12]);
					return(PUT_FAILED);
				}
				for (i = 1; i <= touchprobe_ints[12]; i++)
				{
					if(fread(touchprobe_doubles,sizeof(double),3,features_file) != 3)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,"touchprobe boundary point",i);
						return(NO_WRITE_FILE);
					}
					dbPutPlaneBoundaryPointX(this_feature,i,touchprobe_doubles[0]);
					dbPutPlaneBoundaryPointY(this_feature,i,touchprobe_doubles[1]);
					dbPutPlaneBoundaryPointZ(this_feature,i,touchprobe_doubles[2]);
				}
			}
		}
	}
	else 
	{
		if ((dbGetFeatureLensCode(this_feature) == LENS_CODE_TOUCHPROBE) && 
				(dbGetNominalsEntered(this_feature) == TRUE))
		{
			//* get the basic dimension 
			basicDimension = dbGetBasicDimension(this_feature);

			//* Clear Basic Dimension 
			if (dbPutBasicDimension(this_feature, 0.0) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"basic dimension",2);
				return(PUT_FAILED);
			}

			//* put the value into our probe parameters 
			if (dbPutApproachValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe approach parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutSafeValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutXYAngleValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe xyangle parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutElevationValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe elevation parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutSpacingValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe spacing parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutPlusValue(this_feature, TRUE) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe plus parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutDistTopValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe dist from top parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutDistBottomValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe dist from bottom parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutStartAngleValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe start angle parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutEndAngleValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe end angle parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutStartElevationValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe start angle parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutEndElevationValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe end angle parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutRowSpacingValue(this_feature, basicDimension) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe row spacing parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutTotalRowValue(this_feature, 0) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe total rows parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutTotalScansValue(this_feature, 0) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe row spacing parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutTotalLongScansValue(this_feature, 0) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe row spacing parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutTotalRevsValue(this_feature, 0) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe row spacing parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutPointsValue(this_feature, 0) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe points parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutInternalValue(this_feature, FALSE) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe internal parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutIsCleanFlag(this_feature, DONT_VECTOR) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe isClean parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutSafeFlag(this_feature, TRUE) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutSpacingFlag(this_feature, FALSE) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe spacing flag parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutRowSpacingFlag(this_feature, FALSE) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe row spacing flag parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutXYAngleFlag(this_feature, 1) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}
			if (dbPutElevationFlag(this_feature, 1) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe safe flag parameter",1);
				return(PUT_FAILED);
			}
		}
	}
	return(NO_ERROR);
}

int dbReadPointsEntry(Feature *this_feature, FILE *points_file)
{
	char *loc_func_name = "dbReadPointsEntry";
	int i;
	short number_of_points = 0;
	double locz[4] = { 0.0 };
	short lights[3] = { 0 };
	short target[4] = { 0 };
	short reference_feature = 0;
	unsigned short target_filename_length = 0;
	char target_filename[12] = "\0";
	short edge_seek = 0;
	short side_flag = 0;
	short old_led_intensity[MAX_LED_SECTORS] = { 0 };
	short old_led_state[MAX_LED_SECTORS] = { 0 };
	short led_intensity[MAX_LED_SECTORS][MAX_LED_RINGS] = { 0 };
	short led_flag = 0;
	int j,k;

	if (fread(&number_of_points,sizeof(short),1,points_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"number of points",NO_VAL);
		return(NO_READ_FILE);
	}
	if (dbGoodNumberOfPoints(this_feature,number_of_points) == FALSE)
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"number of points",number_of_points);
		return(GET_FROM_DB_FAILED);
	}

	for (i = 1; i <= (int) number_of_points; i++)
	{
		if (dbIncNumberPoints(this_feature) != i)
		{
			errPost(loc_func_name,DATABASE_ERROR,INC_POINT_FAILED,NULL,i);
			return(INC_POINT_FAILED);
		}
		if (dbIsFeatureMeasured(this_feature))
		{
			if (fread(locz,sizeof(double),4,points_file) != 4)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point location",NO_VAL);
				return(NO_READ_FILE);
			}
			if (dbPutPointX(this_feature,i,locz[0]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point location",i);
				return(PUT_FAILED);
			}
			if (dbPutPointY(this_feature,i,locz[1]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point location",i);
				return(PUT_FAILED);
			}
			if (dbPutPointZ(this_feature,i,locz[2]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point location",i);
				return(PUT_FAILED);
			}
			if (dbPutPointZoom(this_feature,i,locz[3]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point zoom location",i);
				return(PUT_FAILED);
			}

			if (gb_db_version >= 19)
			{
				if (fread(locz,sizeof(double),3,points_file) != 3)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point location",NO_VAL);
					return(NO_READ_FILE);
				}
			}
			if (dbPutActPointX(this_feature,i,locz[0]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point location",i);
				return(PUT_FAILED);
			}
			if (dbPutActPointY(this_feature,i,locz[1]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point location",i);
				return(PUT_FAILED);
			}
			if (dbPutActPointZ(this_feature,i,locz[2]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point location",i);
				return(PUT_FAILED);
			}

			if (fread(lights,sizeof(short),3,points_file) != 3)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point lights",NO_VAL);
				return(NO_READ_FILE);
			}
			if (dbPutPointBackLight(this_feature,i,(int) lights[0]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point back light",i);
				return(PUT_FAILED);
			}
			if (dbPutPointRingLight(this_feature,i,(int) lights[1]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point ring light",i);
				return(PUT_FAILED);
			}
			if (dbPutPointAuxLight(this_feature,i,(int) lights[2]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point aux light",i);
				return(PUT_FAILED);
			}

			if (fread(target,sizeof(short),4,points_file) != 4)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point target",NO_VAL);
				return(NO_READ_FILE);
			}
			if (dbPutPointTargetType(this_feature,i,(short) target[0]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target type",i);
				return(PUT_FAILED);
			}
			if (dbPutPointTargetColor(this_feature,i,(int) target[1]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target color",i);
				return(PUT_FAILED);
			}
			if (dbPutPointTargetSpacing(this_feature,i,(int) target[2],(int) target[3]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target spacing",i);
				return(PUT_FAILED);
			}

			if (fread(&target_filename_length,sizeof(unsigned short),1,points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point target filename length",NO_VAL);
				return(NO_READ_FILE);
			}
			if (target_filename_length > 0)
			{
				if (fread(target_filename,sizeof(char),target_filename_length,points_file) != target_filename_length)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point target filename",NO_VAL);
					return(NO_READ_FILE);
				}
				if (dbPutPointTargetFilename(this_feature,i,target_filename) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target filename",i);
					return(PUT_FAILED);
				}
			}
			if (gb_db_version >= 2)
			{
				if (fread(&edge_seek,sizeof(short),1,points_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point edge seek",NO_VAL);
					return(NO_READ_FILE);
				}
				if (dbPutPointEdgeSeek(this_feature,i,(int) edge_seek) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point edge seek",i);
					return(PUT_FAILED);
				}
			}
			if (gb_db_version >= 7)
			{
				if (fread(&side_flag,sizeof(short),1,points_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point side flag",NO_VAL);
					return(NO_READ_FILE);
				}
				if (dbPutPointSideNum(this_feature,i,(int) side_flag) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point side number",i);
					return(PUT_FAILED);
				}
			}
			if (gb_db_version == 13)
			{
				if (fread(old_led_intensity,sizeof(short),MAX_LED_SECTORS,points_file) != MAX_LED_SECTORS)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point led intensities",NO_VAL);
					return(NO_READ_FILE);
				}
				if (fread(old_led_state,sizeof(short),MAX_LED_SECTORS,points_file) != MAX_LED_SECTORS)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point led states",NO_VAL);
					return(NO_READ_FILE);
				}
				for (j = 1; j <= MAX_LED_SECTORS; j++)
				{
					for (k = 1; k <= MAX_LED_RINGS; k++)
					{
						if (old_led_state[j] & (1 << (k - 1)))
						{
							if (dbPutPointLedIntensity(this_feature,i,j,k,(int) old_led_intensity[j - 1]) != NO_ERROR)
							{
								errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point led intensity",i);
								return(PUT_FAILED);
							}
						}
					}
				}
			}
			if (gb_db_version >= 14)
			{
				if (fread(led_intensity,sizeof(short),(MAX_LED_SECTORS * MAX_LED_RINGS),points_file) != (MAX_LED_SECTORS * MAX_LED_RINGS))
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge led intensities",NO_VAL);
					return(NO_READ_FILE);
				}
				if (fread(&led_flag,sizeof(short),1,points_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"weak edge led flag",NO_VAL);
					return(NO_READ_FILE);
				}
				for (j = 1; j <= MAX_LED_SECTORS; j++)
				{
					for (k = 1; k <= MAX_LED_RINGS; k++)
					{
						if (dbPutPointLedIntensity(this_feature,i,j,k,(int)led_intensity[j - 1][k - 1]) != NO_ERROR)
						{
							errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point led intensity",i);
							return(PUT_FAILED);
						}
					}
				}
				if (dbPutPointLedFlag(this_feature,i,led_flag) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point led flag",i);
					return(PUT_FAILED);
				}
			}
			if (gb_db_version >= 18)
			{
				if (fread(target,sizeof(short),4,points_file) != 4)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point target",NO_VAL);
					return(NO_READ_FILE);
				}
				if (target[0] >= 0)
				{
					if (dbPutFocusBackoff(this_feature,i,target[0]) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus backoff",i);
						return(PUT_FAILED);
					}
					if (dbPutFocusStepSize(this_feature,i,target[1]) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus step size",i);
						return(PUT_FAILED);
					}
					if (dbPutFocusNoise(this_feature,i,target[2]) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus noise",i);
						return(PUT_FAILED);
					}
					if (dbPutFocusScanFlag(this_feature,i,target[3]) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus scan flag",i);
						return(PUT_FAILED);
					}
				}
			}
			if (gb_db_version >= 27)
			{
				if (fread(locz,sizeof(double),3,points_file) != 3)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"compensated point location",NO_VAL);
					return(NO_READ_FILE);
				}
				if (dbPutCompPointX(this_feature,i,locz[0]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point location",i);
					return(PUT_FAILED);
				}
				if (dbPutCompPointY(this_feature,i,locz[1]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point location",i);
					return(PUT_FAILED);
				}
				if (dbPutCompPointZ(this_feature,i,locz[2]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point location",i);
					return(PUT_FAILED);
				}
				if (fread(locz,sizeof(double),3,points_file) != 3)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point vector",NO_VAL);
					return(NO_READ_FILE);
				}
				if (dbPutPointDirectionVectorX(this_feature,i,locz[0]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point vector",i);
					return(PUT_FAILED);
				}
				if (dbPutPointDirectionVectorY(this_feature,i,locz[1]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point vector",i);
					return(PUT_FAILED);
				}
				if (dbPutPointDirectionVectorZ(this_feature,i,locz[2]) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point vector",i);
					return(PUT_FAILED);
				}
			}
		}
		else
		{
			if (fread(&reference_feature,sizeof(short),1,points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"point reference feature number",NO_VAL);
				return(NO_READ_FILE);
			}
			if (dbPutRefFeatureNumber(this_feature,i,(int) reference_feature) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"reference feature number",i);
				return(PUT_FAILED);
			}
		}
	}
	if (gb_db_version >= 12)
	{
		if (fread(&number_of_points,sizeof(short),1,points_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"number measured points",NO_VAL);
			return(NO_READ_FILE);
		}
		dbPutNumberMeasuredPoints(this_feature,number_of_points);
	}
	return(NO_ERROR);
}

int dbReadActualsEntry(Feature *this_feature, FILE *actuals_file)
{
	char *loc_func_name = "dbReadActualsEntry";
	short number_of[2] = { 0 };
	double actual_loc_size[23] = { 0.0 };
	unsigned int number_of_actuals = 11;
	short i;

	if (fread(number_of,sizeof(short),2,actuals_file) != 2)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"actual flags",NO_VAL);
		return(NO_READ_FILE);
	}
	if ( (number_of[0] < 0) || (number_of[0] > 2) )
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"number of locations",number_of[0]);
		return(GET_FROM_DB_FAILED);
	}
	if ( (number_of[1] < 0) || (number_of[1] > 4) )
	{
		errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"number of sizes",number_of[1]);
		return(GET_FROM_DB_FAILED);
	}
	if (gb_db_version >= 21)
		number_of_actuals = 14;
	if (gb_db_version >= 22)
		number_of_actuals = 22;
	if (gb_db_version >= 23)
		number_of_actuals = 23;
	if (gb_db_version >= 25)
		number_of_actuals = 18;
	if (gb_db_version >= 31)
		number_of_actuals = 19;

	if (fread(actual_loc_size,sizeof(double),number_of_actuals,actuals_file) != number_of_actuals)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"actuals",NO_VAL);
		return(NO_READ_FILE);
	}
	dbPutActualLocX(this_feature,actual_loc_size[0]);
	dbPutActualLocY(this_feature,actual_loc_size[1]);
	dbPutActualLocZ(this_feature,actual_loc_size[2]);
	if (number_of[0] == 2)
	{
		dbPutSecondLocX(this_feature,actual_loc_size[3]);
		dbPutSecondLocY(this_feature,actual_loc_size[4]);
		dbPutSecondLocZ(this_feature,actual_loc_size[5]);
	}
	for (i = 1; i <= number_of[1]; i++)
		dbPutActualSize(this_feature,actual_loc_size[5 + i]);
	dbPutActualDeviation(this_feature,actual_loc_size[10]);
	if (gb_db_version >= 21)
	{
		dbPutDirectionX(this_feature,actual_loc_size[11]);
		dbPutDirectionY(this_feature,actual_loc_size[12]);
		dbPutDirectionZ(this_feature,actual_loc_size[13]);
	}
	if (gb_db_version >= 22)
	{
		for (i = 1; i <= MAX_ANGLES; i++)
		{	// 14, 15, 16
			dbPutActualOrientation(this_feature,actual_loc_size[13 + i],i);
		}
	}
	if (gb_db_version >= 23)
	{
		if (gb_db_version >= 25)
			dbPutActualPosTol(this_feature,actual_loc_size[17]);
		else
			dbPutActualPosTol(this_feature,actual_loc_size[22]);
	}
	if (gb_db_version >= 31)
	{
		dbPutModifiedPosTol(this_feature,actual_loc_size[18]);
	}
	return(NO_ERROR);
}

int dbReadNominalsEntry(Feature *this_feature, FILE *nominals_file)
{
	char *loc_func_name = "dbReadNominalsEntry";
	double nom_tols[17];
	short flags[2];
	int tol_type = STANDARD_TOL;
	unsigned int number_of_nominals = 3;
	short search_steps[3];
	int i;

	if (fread(nom_tols,sizeof(double),17,nominals_file) != 17)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominals",NO_VAL);
		return(NO_READ_FILE);
	}
	if (fread(flags,sizeof(short),2,nominals_file) != 2)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominal flags",NO_VAL);
		return(NO_READ_FILE);
	}

	if (gb_db_version < 8)
	{
		if ( (nom_tols[10] >= 0) && (nom_tols[11] <= 0) )
		{
			tol_type = STANDARD_TOL;
		}
		else
		{
			tol_type = ISO_TOL;
		}
		/* make the lower x, y, z and auxiliary size tolerances negative */
		nom_tols[4] = -fabs(nom_tols[4]);
		nom_tols[6] = -fabs(nom_tols[6]);
		nom_tols[8] = -fabs(nom_tols[8]);
		nom_tols[14] = -fabs(nom_tols[14]);
	}

	dbPutNominalLocX(this_feature,nom_tols[0]);
	dbPutNominalLocY(this_feature,nom_tols[1]);
	dbPutNominalLocZ(this_feature,nom_tols[2]);

	dbPutUpperLocTolX(this_feature,nom_tols[3]);
	dbPutLowerLocTolX(this_feature,nom_tols[4]);
	dbPutUpperLocTolY(this_feature,nom_tols[5]);
	dbPutLowerLocTolY(this_feature,nom_tols[6]);
	dbPutUpperLocTolZ(this_feature,nom_tols[7]);
	dbPutLowerLocTolZ(this_feature,nom_tols[8]);

	dbPutNominalSize(this_feature,nom_tols[9]);
	dbPutUpperSizeTol(this_feature,nom_tols[10]);
	dbPutLowerSizeTol(this_feature,nom_tols[11]);

	dbPutNominalSize2(this_feature,nom_tols[12]);
	dbPutUpperSize2Tol(this_feature,nom_tols[13]);
	dbPutLowerSize2Tol(this_feature,nom_tols[14]);

	dbPutNominalFormTol(this_feature,nom_tols[15]);
	dbPutNominalPosTol(this_feature,nom_tols[16]);
	dbPutNominalFlags(this_feature,(int) flags[0],(int) flags[1]);

	if (gb_db_version >= 8)
	{
		if (fread(flags,sizeof(short),2,nominals_file) != 2)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominal flags",NO_VAL);
			return(NO_READ_FILE);
		}
		dbPutNominalsEntered(this_feature,flags[0]);
		tol_type = flags[1];
	}
	else
	{
		dbSetNominalsEntered(this_feature);
	}
	if ( (gb_db_version <= 8) &&
		 ( (dbGetFeatureMode(this_feature) == CONSTRUCT) &&
		   (dbGetFeatureType(this_feature) == POINT) &&
		   (dbGetNominalsEntered(this_feature) == FALSE) ) )
	{	/* old databases didn't set nominals entered flag for construct point */
		dbPutNominalsEntered(this_feature,TRUE);
	}
	dbPutToleranceType(this_feature,tol_type);

	if (gb_db_version >= 23)
	{
		if (gb_db_version >= 25)
			number_of_nominals = 6;
		if (fread(nom_tols,sizeof(double),number_of_nominals,nominals_file) != number_of_nominals)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominals",NO_VAL);
			return(NO_READ_FILE);
		}

		dbPutNominalElevationAngle(this_feature,nom_tols[0]);
		dbPutUpperElevationTol(this_feature,nom_tols[1]);
		dbPutLowerElevationTol(this_feature,nom_tols[2]);
		if (gb_db_version >= 25)
		{
			dbPutNominalSize3(this_feature,nom_tols[3]);
			dbPutUpperSize3Tol(this_feature,nom_tols[4]);
			dbPutLowerSize3Tol(this_feature,nom_tols[5]);
		}
	}

	if (gb_db_version >= 28)
	{
		if (fread(search_steps,sizeof(short),3,nominals_file) != 3)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"search steps",NO_VAL);
			return(NO_WRITE_FILE);
		}
		for (i = 1; i <= 3; i++)
		{
			(void) dbPutSearchOnStep(this_feature, i, (int) search_steps[i - 1]);
		}
	}

	return(NO_ERROR);
}

int dbReadTransformEntry(Feature *this_feature, FILE *routine_file)
{
	char *loc_func_name = "dbReadTransformEntry";
	Database_Transform part_transform;

	if (gb_db_version < 20)
	{
		if (fread(&part_transform.transform,sizeof(Transform),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"part transform",NO_VAL);
			return(NO_READ_FILE);
		}
		part_transform.nominal_part_axis = 0.0;
		part_transform.actual_part_axis = 0.0;
	}
	else
	{
		if (fread(&part_transform,sizeof(Database_Transform),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"part transform",NO_VAL);
			return(NO_READ_FILE);
		}
	}
	dbPutFeaturePartTransform(this_feature, &part_transform);
	return(NO_ERROR);
}

int dbReadRepeatOffsets(FILE *in_file)
{
	char *loc_func_name = "dbReadRepeatOffsets";
	short temp;
	int i;
	int repeat_on = FALSE;
	short repeat_this_offset[MAX_REPEAT_OFFSETS] = { 0 };

	if (gb_db_version <= 10)
	{
		if (fread(&temp,sizeof(short),1,in_file) == 1)
		{
			gb_repeat_finish_run = (int) temp;
			if (fread(gb_repeat_x_offsets,sizeof(double),MAX_REPEAT_OFFSETS,in_file) != MAX_REPEAT_OFFSETS)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"repeat x offsets",NO_VAL);
				return(NO_READ_FILE);
			}
			if (fread(gb_repeat_y_offsets,sizeof(double),MAX_REPEAT_OFFSETS,in_file) != MAX_REPEAT_OFFSETS)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"repeat y offsets",NO_VAL);
				return(NO_READ_FILE);
			}
			if (gb_db_version >= 8)
			{
				if (fread(&temp,sizeof(short),1,in_file) == 1)
				{
					gb_repeat_confirm_next = (int) temp;
				}
			}
			for (i = 0; i < MAX_REPEAT_OFFSETS; i++)
			{
				if ( (dbGetRepeatXOffset(i) != 0.0) ||
					 (dbGetRepeatYOffset(i) != 0.0) )
				{
					repeat_on = TRUE;
					dbPutRepeatOffsetOnOff(i,TRUE);
				}
			}
			gb_fixture_repeat = repeat_on;
			gb_repeat_start_step = 1;
			gb_repeat_end_step = 0;
			gb_part_repeat = FALSE;
			gb_x_part_offset = 0.0;
			gb_y_part_offset = 0.0;
			gb_x_part_times = 0;
			gb_y_part_times = 0;
			gb_repeat_y_first = 0;
		}
	}
	else
	{
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"fixture repeat flag",NO_VAL);
			return(NO_READ_FILE);
		}
		gb_fixture_repeat = temp;
		if (fread(repeat_this_offset,sizeof(short),MAX_REPEAT_OFFSETS,in_file) != MAX_REPEAT_OFFSETS)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"repeat offset flags",NO_VAL);
			return(NO_READ_FILE);
		}
		for (i = 0; i < MAX_REPEAT_OFFSETS; i++)
			gb_repeat_this_offset[i] = (int) repeat_this_offset[i];
		if (fread(gb_repeat_x_offsets,sizeof(double),MAX_REPEAT_OFFSETS,in_file) != MAX_REPEAT_OFFSETS)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"repeat x offsets",NO_VAL);
			return(NO_READ_FILE);
		}
		if (fread(gb_repeat_y_offsets,sizeof(double),MAX_REPEAT_OFFSETS,in_file) != MAX_REPEAT_OFFSETS)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"repeat y offsets",NO_VAL);
			return(NO_READ_FILE);
		}
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"confirm next",NO_VAL);
			return(NO_READ_FILE);
		}
		gb_repeat_confirm_next = (int) temp;
		
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"part repeat flag",NO_VAL);
			return(NO_READ_FILE);
		}
		gb_part_repeat = temp;
		if (fread(&gb_x_part_offset,sizeof(double),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"x part offset",NO_VAL);
			return(NO_READ_FILE);
		}
		if (fread(&gb_y_part_offset,sizeof(double),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"y part offset",NO_VAL);
			return(NO_READ_FILE);
		}
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"x number times",NO_VAL);
			return(NO_READ_FILE);
		}
		gb_x_part_times = (int) temp;
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"y number times",NO_VAL);
			return(NO_READ_FILE);
		}
		gb_y_part_times = (int) temp;
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"y offset first flag",NO_VAL);

			return(NO_READ_FILE);
		}
		gb_repeat_y_first = (int) temp;
		
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"finish repeat",NO_VAL);
			return(NO_READ_FILE);
		}
		gb_repeat_finish_run = (int) temp;
		if (fread(&temp,sizeof(short),1,in_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"start step",NO_VAL);
			return(NO_READ_FILE);
		}
		gb_repeat_start_step = (int) temp;
		if (gb_db_version >= 35)
		{
			if (fread(&temp,sizeof(short),1,in_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"end step",NO_VAL);
				return(NO_READ_FILE);
			}
			gb_repeat_end_step = (int) temp;
		}
	}
	
	return(NO_ERROR);
}

int dbGetDataBase(char *routine_name)
{
	char *loc_func_name = "dbGetDataBase";
	FILE *routine_file;
	Feature *this_feature = NULL;
	short number_of_features;
	int i;
	unsigned short j;
	char junk[LONG_STRING] = "";
	short temp;
	char ramdrive[_MAX_PATH];
	char filename[_MAX_PATH];
	FILE *temp_file;
	int file_handle;

	routine_file = fopen(routine_name,"rb");	 /* open the routine file */

	if ( (getEnvVar(ramdrive, "TEMP", TEMP_DEFAULT) == NO_ERROR) &&
		 (routine_file != NULL) )
	{
		strcpy(filename,ramdrive);			   /* set the temporary file path */
		strcat(filename,"temp.rtn");		/* append the temporary file name */
		temp_file = fopen(filename,"wb");			   /* open it for writing */
		if (temp_file == NULL)
		{
			errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,filename,NO_VAL);
			(void) fclose(routine_file);
			return(OPEN_FAILED);
		}
		/* copy contents of routine file to temporary file */
		file_handle = fileno(routine_file);
		if (dbCopyBytes(routine_file,temp_file,filelength(file_handle)) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_COPY_FILE,filename,NO_VAL);
			(void) fclose(routine_file);
			(void) fclose(temp_file);
			return(NO_COPY_FILE);
		}
		(void) fclose(routine_file);
		(void) fclose(temp_file);
		routine_file = fopen(filename,"rb");		   /* open it for reading */
	}

	if (routine_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,routine_name,NO_VAL);
		return(OPEN_FAILED);
	}

	if (fread(gb_db_header,sizeof(char),strlen(ID_STRING),routine_file) != strlen(ID_STRING))
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,routine_name,NO_VAL);
		(void) fclose(routine_file);
		return(NO_READ_FILE);
	}

	/* this code used to use strncmp but it would fail for no apparent reason */
	i = 0;
	strcpy(junk,ID_STRING);
	while (i < CHARS_TO_COMPARE)
	{
		if (gb_db_header[i] != junk[i])
		{
			if (fclose(routine_file) != 0)
			{
				errPost(loc_func_name,SYSTEM_ERROR,CLOSE_FAILED,routine_name,NO_VAL);
				(void) fclose(routine_file);
				return(CLOSE_FAILED);
			}
			gb_db_version = 0;
			return(dbGetOldDataBase(routine_name));
		}
		i++;
	}

	if (fread(&temp,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"database version",NO_VAL);
		(void) fclose(routine_file);
		return(NO_READ_FILE);
	}
	if (temp > DATABASE_VERSION)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"bad database version",temp);
		(void) fclose(routine_file);
		return(NO_READ_FILE);
	}
	gb_db_version = temp;
	if (fread(&number_of_features,sizeof(short),1,routine_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"number of features",NO_VAL);
		(void) fclose(routine_file);
		return(NO_READ_FILE);
	}

	this_feature = dbNewFeature();
	if (this_feature == NULL)
	{
		errPost(loc_func_name,DATABASE_ERROR,CREATE_FEAT_FAILED,NULL,NO_VAL);
		(void) fclose(routine_file);
		return(CREATE_FEAT_FAILED);
	}

	for (i = 1; i <= (int) number_of_features; i++)
	{
		if (i > 1)
		{
			dbInitFeature(this_feature);
		}
		if (dbReadFeatureEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
			dbDisposeFeature(this_feature);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		if (dbGetFeatureNumber(this_feature) != i)
		{
			errPost(loc_func_name,SYSTEM_ERROR,INVALID_ROUTINE,NULL,NO_VAL);
			dbDisposeFeature(this_feature);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}

		if (dbReadPointsEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
			dbDisposeFeature(this_feature);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}

		if (dbReadActualsEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"actuals",NO_VAL);
			dbDisposeFeature(this_feature);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}

		if (dbReadNominalsEntry(this_feature,routine_file) != NO_ERROR)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominals",NO_VAL);
			dbDisposeFeature(this_feature);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}

		if (gb_db_version >= 19)
		{
			/* read feature part transforms */
			if (dbReadTransformEntry(this_feature,routine_file) != NO_ERROR)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"transform",NO_VAL);
				dbDisposeFeature(this_feature);
				(void) fclose(routine_file);
				return(NO_READ_FILE);
			}
		}

		if (dbPutFeatureInDB(this_feature) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature",i);
			dbDisposeFeature(this_feature);
			(void) fclose(routine_file);
			return(PUT_FAILED);
		}
	}
	dbDisposeFeature(this_feature);

	for (i = 0; i < NUM_HEADER_LINES; i++)
	{
		if (fread(gb_run_header_txt[i],sizeof(char),81,routine_file) != 81)
		{
			(void) memset(gb_run_header_txt[i],0,81);
			i = NUM_HEADER_LINES;
		}
	}
	for (i = 0; i < NUM_SETUP_LINES; i++)
	{
		if (fread(gb_run_setup_txt[i],sizeof(char),81,routine_file) != 81)
		{
			(void) memset(gb_run_setup_txt[i],0,81);
			i = NUM_SETUP_LINES;
		}
	}

	if (dbReadRepeatOffsets(routine_file) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"repeat data",NO_VAL);
		(void) fclose(routine_file);
		return(NO_READ_FILE);
	}

	if (gb_db_version >= 4)
	{
		if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stat filename length",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		if (fread(gb_stat_filename,sizeof(char),j,routine_file) != j)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stat filename",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		if (fread(&temp,sizeof(short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"keep stat filename",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		gb_keep_stat_filename = temp;
		if (fread(&temp,sizeof(short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"display run windows",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		gb_display_run_windows = temp;
		if (fread(&temp,sizeof(short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stop out of tol",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		gb_stop_out_of_tol = temp;
		if (fread(&temp,sizeof(short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"printer override",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		gb_printer_override = temp;
		if (fread(&temp,sizeof(short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"auto edge enable",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		gb_auto_edge_enabled = temp;
		if (fread(&temp,sizeof(short),1,routine_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"print to file",NO_VAL);
			(void) fclose(routine_file);
			return(NO_READ_FILE);
		}
		gb_print_to_file = temp;
		if (gb_print_to_file == TRUE)
		{
			if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"print filename length",NO_VAL);
				(void) fclose(routine_file);
				return(NO_READ_FILE);
			}
			if (fread(gb_printer_filename,sizeof(char),j,routine_file) != j)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"print filename",NO_VAL);
				(void) fclose(routine_file);
				return(NO_READ_FILE);
			}
		}
		if (gb_db_version >= 5)
		{
			if (fread(&temp,sizeof(short),1,routine_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"done during run",NO_VAL);
				(void) fclose(routine_file);
				return(NO_READ_FILE);
			}
			gb_done_during_run = temp;
			if (gb_db_version >= 8)
			{
				if (fread(&temp,sizeof(short),1,routine_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"missed edge override",NO_VAL);
					(void) fclose(routine_file);
					return(NO_READ_FILE);
				}
				gb_missed_edge_override = temp;
			}	
			if (gb_db_version >= 10)
			{				
				if (fread(&temp,sizeof(short),1,routine_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"print run time",NO_VAL);
					(void) fclose(routine_file);
					return(NO_READ_FILE);
				}
				gb_print_run_time = temp;
				if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"template filename length",NO_VAL);
					(void) fclose(routine_file);
					return(NO_READ_FILE);
				}
				if (j > 0)
 				{	
 					if (fread(gb_template_filename,sizeof(char),j + 1,routine_file) != (unsigned short)(j + 1))
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"template filename",NO_VAL);
						(void) fclose(routine_file);
						return(NO_READ_FILE);
					}  
				}
				if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stream filename length",NO_VAL);
				}
				if (fread(gb_stream_filename,sizeof(char),j,routine_file) != j)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stream filename",NO_VAL);
					strcpy(gb_stream_filename, "\0");
				}
				if (fread(&temp,sizeof(short),1,routine_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"keep stream filename",NO_VAL);
					temp = 0;
				}
				gb_keep_stream_filename = temp;
				if (fread(&temp,sizeof(short),1,routine_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stat override",NO_VAL);
					temp = 0;
				}
				gb_stat_override = temp;
				if (fread(&temp,sizeof(short),1,routine_file) != 1)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stream override",NO_VAL);
					temp = 0;
				}
				gb_stream_override = temp;

				if (gb_db_version >= 15)
				{
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report output override",NO_VAL);
						temp = 0;
					}
					gb_report_override = temp;
					if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report type override path length",NO_VAL);
					}
					if (fread(gb_report_type_path_override,sizeof(char),j,routine_file) != j)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report type path override",NO_VAL);
						strcpy(gb_report_type_path_override, "\0");
					}
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report path override on",NO_VAL);
						temp = 0;
					}
					gb_report_type_path_override_on = temp;
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"sample size override",NO_VAL);
						temp = 0;
					}
					gb_sample_size_override = temp;
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"sample size override on",NO_VAL);
						temp = 0;
					}
					gb_sample_size_override_on = temp;
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"data export override",NO_VAL);
						temp = 0;
					}
					gb_export_override = temp;
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report sample size",NO_VAL);
						temp = 0;
					}
					gb_report_sample_size = temp;
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report precision",NO_VAL);
						temp = 0;
					}
					gb_report_precision = temp;
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"print report out of spec",NO_VAL);
						temp = 0;
					}
					gb_report_out_of_spec = temp;
					if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report type path length",NO_VAL);
					}
					if (fread(gb_report_type_path,sizeof(char),j,routine_file) != j)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"report type path",NO_VAL);
						strcpy(gb_report_type_path, "\0");
					}
					if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export type path length",NO_VAL);
					}
					if (fread(gb_export_type_path,sizeof(char),j,routine_file) != j)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export type path",NO_VAL);
						strcpy(gb_export_type_path, "\0");
					}
					if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export table name length",NO_VAL);
					}
					if (fread(gb_export_table_name,sizeof(char),j,routine_file) != j)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export table name",NO_VAL);
						strcpy(gb_export_table_name, "\0");
					}
					if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export file type length",NO_VAL);
					}
					if (fread(gb_export_file_type,sizeof(char),j,routine_file) != j)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export file type",NO_VAL);
						strcpy(gb_export_file_type, "\0");
					}
					if (fread(&temp,sizeof(short),1,routine_file) != 1)
					{
						errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export every part",NO_VAL);
						temp = 0;
					}
					gb_export_every_part = temp;
					if (gb_db_version >= 27)
					{
						if (fread(&temp,sizeof(short),1,routine_file) != 1)
						{
							errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"rotary in use",NO_VAL);
							temp = 0;
						}
						gb_rotary_in_use = temp;
					}
					if (gb_db_version >= 31)
					{
						if (fread(&temp,sizeof(short),1,routine_file) != 1)
						{
							errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"export precision",NO_VAL);
							temp = 0;
						}
						gb_export_precision = temp;
						if(gb_db_version >= 37)
						{
							if (fread(&j,sizeof(unsigned short),1,routine_file) != 1)
							{
								errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stream template filename length",NO_VAL);
							}
							if (fread(gb_stream_template_filename,sizeof(char),j,routine_file) != j)
							{
								errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"stream template filename",NO_VAL);
								strcpy(gb_stream_template_filename, "\0");
							}
			
						}
					}
				}
			}
		}
	}

	if (fclose(routine_file) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,CLOSE_FAILED,routine_name,NO_VAL);
		return(CLOSE_FAILED);
	}

	return(NO_ERROR);
}
