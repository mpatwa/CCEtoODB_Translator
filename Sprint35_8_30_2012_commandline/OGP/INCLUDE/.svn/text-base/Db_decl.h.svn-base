/*
$Workfile: Db_decl.h $

$Header: /MeasureMind Plus/Rev 11/include/Db_decl.h 33    7/13/01 4:35p Ttc $

$Modtime: 7/13/01 4:14p $

$Log: /MeasureMind Plus/Rev 11/include/Db_decl.h $
 * 
 * 33    7/13/01 4:35p Ttc
 * Delete unused focus offsets.
 * 
 * 32    5/31/01 10:58a Jpk
 * Added laser speed factor for SCR# 13891
 * 
 * 31    4/03/01 4:53p Ttc
 * SCR 13411.  In order to solve problems with plane auto-path, we need to
 * store the original boundry points of the plane so that they can be sent
 * into the clean up functions over and over.
 * 
 * 30    3/30/01 3:27p Mjl
 * Moved the Ref_Feature struct into emkfeats.h
 * 
 * 29    3/26/01 4:35p Mjl
 * Added a Ref_Feature struct scr #13310
 * 
 * 28    3/26/01 8:31a Jpk
 * Changed name of new flag for laser from dock to deploy.
 * 
 * 27    3/22/01 8:25a Jpk
 * Added new flag for deployable laser to the flags structure.
 * 
 * 26    2/09/01 10:11a Lsz
 * Added new touchprobe parameter.
 * 
 * 25    1/04/01 7:47a Lsz
 * Added another parameter to Touch_Probe_Info
 * 
 * 24    12/12/00 8:28a Lsz
 * Added more touch probe extended data information.  
 * 
 * 23    10/06/00 1:49p Ttc
 * SCR 11792.  Add output precision override to export and stats.  Major
 * output overhaul.
 * 
 * 22    8/29/00 1:03p Lsz
 * Added touchprobe info to the database.
 * 
 * 21    8/14/00 8:02a Jpk
 * Added extended data pointer type functions and declarations.
 * 
 * 20    8/08/00 10:42a Lsz
 * Added a new structure -  TP_Point for model functions, particularly
 * EMKCleanUpPointList.
 * 
 * 19    8/08/00 9:42a Jpk
 * Added laser scan information.
 * 
 * 18    1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 17    11/23/99 1:41p Ttc
 * Delete 2 unused point list structure members.
 * 
 * 16    11/11/99 10:26a Ttc
 * Add direction vector to the smear structure also.  No bump in db
 * version number, so any rtn stored between 10/29/99 and today (11/11/99)
 * is trash.
 * 
 * 15    10/25/99 2:57p Ttc
 * Add point direction vector.
 * 
 * 14    9/28/99 7:52a Ttc
 * Add feature compensated point location.
 * 
 * 13    7/06/99 3:10p Ttc
 * SCR 6627.  Add the z location for the start, middle and end weak edge /
 * edge trace points.  Weak edge and edge trace need the full 3D values
 * for all points so that they work after a plane align.
 * 
 * 12    6/03/99 3:02p Ttc
 * DN 9723 version 4. Only need 3 orientation angles.  Need 8 labels for
 * report and export.  Add nominal size 3 with tolerances.
 * 
 * 11    5/26/99 11:04a Ttc
 * Change Auxilary size and tolerance function names and internal variable
 * names to nominal size2, and upper and lower size2 tols in preparation
 * to add a nominal size 3 with tolerances for cone.
 * 
 * 10    5/25/99 10:47a Ttc
 * Missed one.
 * 
 * 9     5/25/99 9:57a Ttc
 * Delete QCHECK ifdef code and ifndefs.
 * 
 * 8     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 7     4/13/99 11:15a Ttc
 * Bump database version to 22.  Add angle orientations to result
 * structure.
 * 
 * 6     4/02/99 1:18p Acm
 * 
 * 5     3/17/99 9:03a Ttc
 * Add direction to results.
 * 
 *    Rev 10.6   24 Jul 1998 08:18:24   ttc
 * Add datum transforms to feature structure.
 * 
 *    Rev 10.5   14 May 1998 16:06:58   ttc
 * Delete unneeded f2 structure members.
 * 
 *    Rev 10.4   16 Apr 1998 16:31:00   ttc
 * Add mega point number to mega point.
 * Add last accessed mega point pointer.
 * 
 *    Rev 10.3   15 Apr 1998 07:47:32   ttc
 * Put max points back to 999.  9999 is causing stack faults.
 * 
 *    Rev 10.2   14 Apr 1998 11:55:18   ttc
 * Increase maximum number of points to 9999 for edge trace.
 * Add extra dimension for edge trace.
 * 
 *    Rev 10.1   20 Mar 1998 16:05:50   ttc
 * Add f2 focus structure (not written to disk yet).
 * 
 *    Rev 10.0   20 Mar 1998 12:17:26   softadmn
 * Initial revision.
 * 
 *    Rev 9.7   25 Aug 1997 09:00:12   ttc
 * Add dock station number for TP200.
 * 
 *    Rev 9.6   18 Apr 1997 16:10:08   dmf
 * Add SmartReport
 * 
 *    Rev 9.5   28 Jun 1996 08:34:06   ttc
 * Add led flag for basic/advanced window and color on.
 * 
 *    Rev 9.4   18 Jun 1996 13:24:02   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.3   18 May 1996 08:46:02   ttc
 * Add LED light.
 * 
 *    Rev 9.2   08 Apr 1996 16:14:00   ttc
 * Add lens code and # measured points.
 * 
 *    Rev 9.1   18 Mar 1996 12:47:34   ttc
 * Delete safe point crap.  Add miscellaneous flag.  Delete standard deviation
 * flag (not used).
 * 
 *    Rev 9.0   11 Mar 1996 11:21:56   softadmn
 * Initial revision.
 * 
 *    Rev 8.3   27 Feb 1996 10:13:12   ttc
 * Add ifndefs for QCHECK.
 * 
 *    Rev 8.2   01 Aug 1995 13:46:38   dmf
 * Added some new type defs for touch probe
 * 
 *    Rev 8.1   27 Feb 1995 15:27:20   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 14:57:28   softadmn
 * Initial revision.
*/

/* Author: Terry Cramer										 Date: 01/16/1991 */
/* Purpose: Declaration header file 										  */

#ifndef DATABASE_DECL
#define DATABASE_DECL

#define MAX_POINTS 9999
#define MAX_SIZES 4
#define MAX_LOCATIONS 2
#define MAX_ANGLES 3
#define MAX_REPEAT_OFFSETS 25
#define MAX_LED_SECTORS 8
#define MAX_LED_RINGS 8
#define MAX_FLAG_OPTIONS 8
// This will need to be bumped at the same time as in tar_laser.h!
#define MAX_LASER_DEFINITION_POINTS 5 

// Extended data types for the feature data.  Do these really belong here?
#define EXTENDED_DATA_NONE 0
#define EXTENDED_DATA_LASER 1
#define EXTENDED_DATA_TOUCHPROBE 2

typedef struct _Xyz_Vector {/* x y z coordinate set */
	double x;
	double y;
	double z;
} Xyz_Vector;

typedef struct {
	int backlight;
	int ringlight;
	int auxlight;
	int led[MAX_LED_SECTORS][MAX_LED_RINGS];
	int led_flag;
} Point_Lights;

typedef struct {
	int type;
	int color;
	int spacing1;
	int spacing2;
	char *filename;
	void *extra;
} Target_Type;

typedef struct {
	Xyz_Vector take_point;
	Xyz_Vector act_point;
	double zoom;
	Point_Lights lights;
	Target_Type target;
	int edge_seek;
	int side_flag;
	int missed;
	Xyz_Vector comp_point;
	Xyz_Vector direction_vector;
} Point_List;

typedef struct _Mega_Point {
	int mega_point_number;
	Point_List *point_ptr;
	int feature_number;
	struct _Mega_Point *next;
} Mega_Point;

typedef struct {
	int in_mm;			/* 0 = inch, 1 = millimeter */
	int cart_polar;		/* 0 = cartesian, 1 = polar */
	int meas_const;		/* 0 = measure, 1 = construct */
	int dec_dms;		/* 0 = decimal, 1 = d:m:s */
	int print;
	int stats;
	int report;
	int exports;
	int data_stream;
	int miscellaneous;	/* miscellaneous flag for various features */
	int failed;			/* never stored in routine file */
	int deploy_laser;
} Flags;

typedef struct {
	int smr_type;
	Xyz_Vector smr_screen_center;
	double smr_zoom_pos;
	Point_Lights lights;
	Xyz_Vector smr_start;
	Xyz_Vector smr_end;
	Xyz_Vector smr_mid;
	Xyz_Vector smr_vector;
	int smr_direction;
	int smr_bounds;
	int smr_extent;
	int smr_min_quality;
	int smr_constrast;
	int smr_wgt_nominal;
	int smr_wgt_constrast;
	int smr_wgt_first;
	int smr_wgt_last;
	int smr_wgt_second;
	int smr_wgt_second_last;
	int smr_std_dev;
} Smear_Info;

typedef struct {
	int point_frequency_type;
	double point_frequency;
	int scan_strategy;
	int tracking_scan;
	int line_frequency_type;
	double line_frequency;
	Xyz_Vector definition_points[MAX_LASER_DEFINITION_POINTS];
	int laser_threshold;
	int laser_exposure_time;
	int num_points_to_average;
	int laser_id_num;
	int scan_speed_factor;
} Laser_Scan_Info;

typedef struct {
	int isClean;
	int useSafe;
	int spacingflag;
	int rowspacingflag;
	double safe;
	double approach;
	double xyangle;
	double elevation;
	int plus;
	double spacing;
	int points;
	double dist_top;
	double dist_bottom;
	int internal;
	double start_angle;
	double end_angle;
	double row_spacing;
	int total_rows;
	int total_scans;
	int total_long_scans;
	double total_revs;
	int xyangle_flag;
	int elevation_flag;
	double start_elevation;
	double end_elevation;
	int number_boundary_points;
	Xyz_Vector *plane_boundary;
} Touch_Probe_Info;

typedef struct {
	int noise;
	int step_size;
	int backoff;
	int optimize;
} Focus_Info;

typedef struct {
	int number;
	int type;
	int plane;
	int lens_code;
	int dock_location;
	int number_points;
	int missed_points;
	int measured_points;
	Mega_Point mega_points;
	Mega_Point *last_mega_point_accessed;
	int last_display;
	int last_display2;
	int last_display3;
	double basic_dim;
	double extra_dim;
	Flags flags;
	Smear_Info smear_data;
	char *comment;
	char *prompt;
	char *label[MAX_FLAG_OPTIONS];
	char *fieldname[MAX_FLAG_OPTIONS];
	char *expression;
	int extended_data_type;
	void *extended_data;
} Feature_Data;

typedef struct {
	int num_locs;
	Xyz_Vector actual_location[MAX_LOCATIONS];
	int num_sizes;
	double actual_size[MAX_SIZES];
	Xyz_Vector direction;
	double orientation[MAX_ANGLES];
	double form_deviation;
	double position_tol;
	double modified_tol;
} Feature_Result;

typedef struct {
	int nominals_entered;
	int tolerance_type;
	Xyz_Vector nom_loc;
	Xyz_Vector upper_loc_tol;
	Xyz_Vector lower_loc_tol;
	double nom_size;
	double upper_size_tol;
	double lower_size_tol;
	double nom_size2;
	double upper_size2_tol;
	double lower_size2_tol;
	double elev_angle;
	double upper_elev_tol;
	double lower_elev_tol;
	double nom_size3;
	double upper_size3_tol;
	double lower_size3_tol;
	double form_tol;
	double position_tol;
	int nom_flag1;
	int nom_flag2;
	int search_on_steps[3];
} Feature_Nominals;

typedef struct _Matrix
{
	Xyz_Vector a;
	Xyz_Vector b;
	Xyz_Vector c;
} Matrix;

typedef struct _Transform
{
	Xyz_Vector d;		/* 3x1 translation */
	Matrix m;			/* 3x3 transform */
} Transform;

typedef struct _Database_Transform
{
	Transform transform;
	double nominal_part_axis;
	double actual_part_axis;
} Database_Transform;

typedef struct _Feature {
	struct _Feature *next_feature;
	struct _Feature *prev_feature;
	Feature_Data data;
	Feature_Result result;
	Feature_Nominals nominals;
	Database_Transform part_transform;
} Feature;

#endif /* DATABASE_DECL */
