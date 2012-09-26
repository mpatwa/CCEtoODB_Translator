/*
$Workfile: Db_const.h $

$Header: /MeasureMind Plus/Rev 11/include/Db_const.h 36    6/08/01 11:12a Lsz $

$Modtime: 6/08/01 10:17a $

$Log: /MeasureMind Plus/Rev 11/include/Db_const.h $
 * 
 * 36    6/08/01 11:12a Lsz
 * Fixed SCR 13976. Only allow the 1x lens and the 2x lens radio buttons
 * be visible/selectable.  Renamed some pound defines to make more sense
 * (1x lens instead of none).
 * 
 * 35    5/31/01 10:58a Jpk
 * Added laser speed factor for SCR# 13891
 * 
 * 34    5/03/01 12:01p Jnc
 * SCR12676:upped the database version for saving the stream template to
 * the file
 * 
 * 33    5/02/01 5:26p Jpk
 * Added constants for focus before scan.
 * 
 * SCR# 13681
 * 
 * 32    4/03/01 4:53p Ttc
 * SCR 13411.  In order to solve problems with plane auto-path, we need to
 * store the original boundry points of the plane so that they can be sent
 * into the clean up functions over and over.
 * 
 * 31    3/22/01 8:37a Jnc
 * SCR13258:increased the version of the database to 35
 * 
 * 30    3/12/01 3:33p Jpk
 * Added new lens codes for Telecentric zoom and added some constants for
 * SCR# 13195
 * 
 * 29    2/21/01 9:30a Jnc
 * ReqDoc Datum Recall Feature: inserted DATUM_RECALL to feature list
 * 
 * 28    2/19/01 1:32p Mjl
 * Added faces and trims.
 * 
 * 27    2/09/01 10:11a Lsz
 * Bumped database version for new touchprobe parameter.
 * 
 * 26    1/09/01 2:58p Ttc
 * Add constants for safe point and link (EMK inserted safe) point.
 * 
 * 25    1/04/01 8:36a Lsz
 * Bumped database version - added new parameter for touchprobe
 * information.
 * 
 * 24    12/22/00 11:46a Lsz
 * Added new flags for automatic probe path generation.
 * 
 * 23    12/12/00 8:27a Lsz
 * Bumped DB version for touch probe data.
 * 
 * 22    10/24/00 1:29p Jpk
 * 
 * 21    10/13/00 10:13a Jpk
 * Added Min / Max Z for contour features.  Now we can use contour with
 * laser.
 * 
 * 20    10/06/00 1:49p Ttc
 * SCR 11792.  Add output precision override to export and stats.  Major
 * output overhaul.
 * 
 * 18    8/29/00 1:03p Lsz
 * Added flags for vectored or unvectored touchprobe points.
 * 
 * 17    8/14/00 8:02a Jpk
 * Added extended data pointer type functions and declarations.
 * 
 * 16    8/08/00 9:42a Jpk
 * Added laser scan information.
 * 
 * 15    4/26/00 3:18p Wei
 * FILLET(ASME radius) related changes
 * 
 * 14    1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 13    10/25/99 2:57p Ttc
 * Bump db version for compensated point location and point direction
 * vector addition.
 * 
 * 12    7/06/99 3:10p Ttc
 * SCR 6627.  Add the z location for the start, middle and end weak edge /
 * edge trace points.  Weak edge and edge trace need the full 3D values
 * for all points so that they work after a plane align.
 * 
 * 11    6/03/99 3:01p Ttc
 * DN 9723 version 4. Bump db version to 25 for Line, Circle, Plane, Cone
 * enhancements.
 * 
 * 10    4/30/99 2:18p Ttc
 * Increment database version.  Any db with a version less than 23 will
 * need to be converted to a version 24.  This conversion includes
 * changing Min/Max/Avg features to Contours; changes to plane, including
 * possible insert of a datum plane step.
 * 
 * 9     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 8     4/13/99 11:15a Ttc
 * Bump database version to 22.  Add angle orientations to result
 * structure.
 * 
 * 7     4/12/99 5:48p Ttc
 * Add #define for smartreport and data export true position output.
 * 
 * 6     4/12/99 11:23a Ttc
 * Add cone and plane align feature types.
 * 
 * 5     4/02/99 1:18p Acm
 * 
 * 4     3/17/99 9:03a Ttc
 * Add direction to results.
 * 
 *    Rev 10.6   24 Jul 1998 08:17:46   ttc
 * Bump db version to 19 for addition of datum transforms.
 * 
 *    Rev 10.5   09 Jun 1998 10:47:04   ttc
 * Change name of edge trace feature to contour.  Add centroid as last display.
 * 
 *    Rev 10.4   20 May 1998 15:31:10   ttc
 * Bump database version for f2 read and write.
 * 
 *    Rev 10.3   14 May 1998 16:06:14   ttc
 * Add f2 #defines.
 * 
 *    Rev 10.2   05 May 1998 08:42:22   acm
 * added LASER_SWEEP_FOCUS
 * 
 *    Rev 10.1   14 Apr 1998 11:55:00   ttc
 * Bump database rev for edge trace.
 * 
 *    Rev 10.0   20 Mar 1998 12:17:26   softadmn
 * Initial revision.
 * 
 *    Rev 9.7   25 Aug 1997 08:58:44   ttc
 * Bump database version for TP200 dock station in every step.
 * Move focus #defines to this file (they were in tar_ctrl.c - stupid).
 * 
 *    Rev 9.6   18 Apr 1997 16:10:08   dmf
 * Add SmartReport
 * 
 *    Rev 9.5   26 Sep 1996 08:34:10   dmf
 * Add #defines for the new rotary
 * 
 *    Rev 9.4   25 Jun 1996 16:46:56   ttc
 * Add no lens code #define.
 * 
 *    Rev 9.3   18 Jun 1996 13:24:00   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.2   18 May 1996 08:46:00   ttc
 * Add LED light.
 * 
 *    Rev 9.1   08 Apr 1996 16:12:20   ttc
 * Bump database version #, add #defines for touch probe.
 * 
 *    Rev 9.0   11 Mar 1996 11:21:52   softadmn
 * Initial revision.
 * 
 *    Rev 8.8   07 Feb 1996 09:39:18   dmf
 * Added #define USER_INPUT and increased MAX_FEATURE_NUM to 35
 * 
 *    Rev 8.7   13 Dec 1995 09:37:46   ttc
 * Add #define for locate point.
 * 
 *    Rev 8.6   08 Dec 1995 16:18:02   dmf
 * Added #defines ITM_HEIGHT_DEPTH and HEIGHT_DEPTH for pt. to plane
 * 
 *    Rev 8.4   05 Dec 1995 14:55:54   ttc
 * Bumped database version.
 * 
 *    Rev 8.3   12 Sep 1995 15:29:28   dmf
 * Add profile tolerance
 * 
 *    Rev 8.2   01 Aug 1995 13:45:44   dmf
 * Started adding stuff for data stream and point elimination
 * 
 *    Rev 8.1   27 Feb 1995 15:27:18   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 14:57:24   softadmn
 * Initial revision.
*/

/* Author: Terry Cramer										 Date: 01/16/1991 */
/* Purpose: General constant header file									  */

#ifndef GENERAL_CONST
#define GENERAL_CONST

#define DATABASE_VERSION 38
#define ID_STRING "SmartScope                      "

#define BAD_VALUE (-9999.99999)

#define NUM_HEADER_LINES 5
#define NUM_SETUP_LINES 15

#define NO_FEATURE	0
#define POINT 		1
#define MIDPOINT	2
#define LINE		3
#define CIRCLE		4
#define SPHERE		5
// Need to add an actual CENTROID type here of value 6
#define ORIGIN		11
#define AXIS		12
#define DISTANCE	13
#define PLANE		14
#define PT_PLANE	15
#define WIDTH		21
#define INTERSECT	22
#define GAGE_DIA	23
#define GAGE_BALL	24
#define HOLD_FEATURE 25		// USED TO HOLD SPACE IN THE DB
#define MIN_MAX_AVG 26		// DO NOT USE - OBSOLETE FEATURE TYPE
#define ANGULARITY	27
#define PERPENDICULARITY 28
#define PARALLELISM	29
#define ROTARY_FT	30
#define DIGITAL_IO	31
#define MATH		32
#define BRANCH		33
#define PROFILE		34
#define USER_INPUT  35
#define CONTOUR		36
#define CYLINDER	37
#define CONE		38
#define DATUM_PLANE	39
#define FILLET	    40
#define FACE		41
#define TRIM		42
#define DATUM_RECALL 43
#define MAX_FEATURE_NUM 43

#define NO_PLANE	0
#define X_PLANE		1 
#define Y_PLANE		2
#define XY_PLANE	3
#define Z_PLANE		4
#define XZ_PLANE	5
#define YZ_PLANE	6
#define XYZ_PLANE	7

#define TRUE 1
#define FALSE 0

#define MAX_LIGHT 255
#define MAX_LED_STATE 255
#define MAX_COMMENT_LENGTH 60
#define MAX_EXPRESSION_LENGTH 80

#define INCH 0
#define MM 1

#define CART 0
#define POLAR 1

#define MEASURE 0
#define CONSTRUCT 1

#define DECIMAL_ANGLE 0
#define DEG_MIN_SEC 1

#define RADIUS 1
#define DIAMETER 2
#define	MAX_DIAMETER 3
#define	MIN_DIAMETER 4
#define	MAX_RADIUS 5
#define	MIN_RADIUS 6

#define ANGLE1 1
#define ANGLE2 2
#define ANGLE3 3
#define ANGLE4 4
#define INTERSECT1 5
#define INTERSECT2 6

#define CENTER_WIDTH 1
#define MIN_WIDTH 2
#define MAX_WIDTH 3

#define STRAIGHT_LINE 1
#define CONCENTRICITY 3

#define DIG_OUTPUT 1
#define DIG_INPUT 2

#define PERPEND_DIST 1
#define HEIGHT_DEPTH 2

#define ROTARY_DEGREE 0
#define ROTARY_RESULT 1

#define MAXIMUM_X 0
#define MINIMUM_X 1
#define MAXIMUM_Y 2
#define MINIMUM_Y 3
#define MAXIMUM_ANGLE 4
#define MINIMUM_ANGLE 5
#define CENTROID 6
#define MAXIMUM_Z 7
#define MINIMUM_Z 8

#define ANGLE_XY 1
#define ANGLE_YZ 2
#define ANGLE_ZX 3
#define ANGLE_3D 4
#define ANGLE_NXY 5
#define ANGLE_NYZ 6
#define ANGLE_NZX 7
#define ANGLE_N3D 8

#define RFS 0
#define MMC 1
#define LMC 2
#define ID 0
#define OD 1

#define FLAG_X		1
#define FLAG_Y		2
#define FLAG_Z		4
#define FLAG_FEAT	8
#define FLAG_OTHER	16
#define FLAG_ANOTHER	32
#define FLAG_ELEV	64
#define FLAG_TP		128

#define STANDARD_TOL 0
#define ISO_TOL 1

#define SEEK_EDGE_MAX_CONTRAST 0

#define SEEK_EDGE_X 1
#define SEEK_EDGE_Y 2

#define SEEK_EDGE_RIGHT 3
#define SEEK_EDGE_LEFT 4
#define SEEK_EDGE_UP 5
#define SEEK_EDGE_DOWN 6
#define SEEK_LOCATE 7
#define SEEK_Z_DOWN 8
#define SEEK_Z_UP 9

#define SAFE_POINT 0		// user specified safe point (he hit the enter button)
#define LINK_POINT -1		// safe point inserted by TTPMoveSequence call

#define TOP_DOWN_FOCUS		0
#define BOTTOM_UP_FOCUS		1
#define MAX_CONTRAST_FOCUS	2
#define MAX_CONTRAST_GRID	3
#define LASER_SWEEP_FOCUS	4
#define TOP_DOWN_GRID		5
#define BOTTOM_UP_GRID		6

#define LENS_CODE_SMALL_NONE 0		/* small SmartScope style none (1x) */
#define LENS_CODE_SMALL_P5 1		/* small SmartScope style .5x add-on */
#define LENS_CODE_SMALL_P75 2		/* small SmartScope style .75x add-on */
#define LENS_CODE_SMALL_1P5 3		/* small SmartScope style 1.5x add-on */
#define LENS_CODE_SMALL_2P0 4		/* small SmartScope style 2x add-on */
#define LENS_CODE_MIT_P5 5			/* Avant style Mititoyo .5x mag sense */
#define LENS_CODE_MIT_1 6			/* Avant style Mititoyo 1x mag sense */
#define LENS_CODE_MIT_2P5 7			/* Avant style Mititoyo 2.5x mag sense */
#define LENS_CODE_MIT_5 8			/* Avant style Mititoyo 5x mag sense */
#define LENS_CODE_MIT_10 9			/* Avant style Mititoyo 10x mag sense */
#define LENS_CODE_MIT_25 10			/* Avant style Mititoyo 25x mag sense */
#define LENS_CODE_MIT_50 11			/* Avant style Mititoyo 50x mag sense */
#define LENS_CODE_TOUCHPROBE 12		/* Touch Probe */
#define LENS_NO_CODE 13				/* No code stored - used for old routines */
#define LENS_CODE_TZM_MIT_2 14
#define LENS_CODE_TZM_MIT_4 15
#define LENS_CODE_TZM_MIT_8 16
#define LENS_CODE_TZM_MIT_20 17
#define LENS_CODE_TZM_MIT_40 18
#define LENS_CODE_TZM_MIT_1 19
						

#define DONT_VECTOR -1				/* user has selected "Points", don't vector! */
#define VECTOR_CLEAN 2
#define AUTO_CLEAN 3
#define AUTO_CLEAN_5 4
#define AUTO_CLEAN_9 5
#define AUTO_CLEAN_25 6
#define AUTO_CLEAN_SEC 7
#define AUTO_CLEAN_HEL 8
#define AUTO_CLEAN_LONG 9
#define AUTO_CLEAN_PER 10
#define AUTO_CLEAN_GRID 11

#define LASER_INVALID_SCAN 0
#define LASER_LINEAR_SCAN 1
#define LASER_CIRCULAR_SCAN 2
#define LASER_ARC_SCAN 3
#define LASER_AREA_SCAN 4
#define LASER_BOX_SCAN 5
#define LASER_SINE_SCAN 6
#define LASER_RASTER_SCAN 7
#define LASER_SPIRAL_SCAN 8

#define LASER_NUM_INVALID_SCAN_POINTS  0
#define LASER_NUM_LINEAR_SCAN_POINTS   2
#define LASER_NUM_CIRCULAR_SCAN_POINTS 3
#define LASER_NUM_BOX_SCAN_POINTS      4
#define LASER_NUM_SINE_SCAN_POINTS     4
#define LASER_NUM_RASTER_SCAN_POINTS   4
#define LASER_NUM_SPIRAL_SCAN_POINTS   3

// #defines for determining whether we are tracking and or focusing before scans
#define LASER_NO_TRACK_NO_FOCUS_FIRST 0x0
#define LASER_TRACK 0x01
#define LASER_FOCUS_FIRST 0x02


#endif /* GENERAL_CONST */