/*
$Workfile: Db_proto.h $

$Header: /MeasureMind Plus/Rev 11/include/Db_proto.h 35    7/13/01 4:35p Ttc $

$Modtime: 7/13/01 4:13p $

$Log: /MeasureMind Plus/Rev 11/include/Db_proto.h $
 * 
 * 35    7/13/01 4:35p Ttc
 * Delete unused focus offsets.
 * 
 * 34    7/13/01 3:33p Jnc
 * removed close database function
 * 
 * 33    7/11/01 2:52p Ttc
 * SCR 14182.  Add ability to use contours in composite spheres.
 * In order to not have to change 4 functions in 3 different files
 * everytime someone wants another feature type valid for composites, I
 * made functions that return whether a reference feature type is valid
 * for compositing.
 * 
 * 32    5/31/01 10:58a Jpk
 * Added laser speed factor for SCR# 13891
 * 
 * 31    4/03/01 4:53p Ttc
 * SCR 13411.  In order to solve problems with plane auto-path, we need to
 * store the original boundry points of the plane so that they can be sent
 * into the clean up functions over and over.
 * 
 * 30    3/26/01 8:31a Jpk
 * Changed name of new flag for laser from dock to deploy.
 * 
 * 29    3/22/01 8:38a Jnc
 * SCR13258:added prototypes getting and putting end step for repeating
 * 
 * 28    3/22/01 8:25a Jpk
 * Added new flag for deployable laser to the flags structure.
 * 
 * 27    2/09/01 10:12a Lsz
 * Added get and put for new touchprobe parameter.
 * 
 * 26    1/04/01 7:48a Lsz
 * Added two more function headers to get and set touch probe extended
 * data.
 * 
 * 25    12/12/00 8:29a Lsz
 * Function headers to get and set touch probe extended data.
 * 
 * 24    10/06/00 1:49p Ttc
 * SCR 11792.  Add output precision override to export and stats.  Major
 * output overhaul.
 * 
 * 23    9/20/00 3:52p Lsz
 * Added header for a function to check for whether or not touch probe
 * extended data exists.
 * 
 * 22    8/29/00 1:04p Lsz
 * Added headers for functions in file Db_probe.c
 * 
 * 21    8/14/00 8:02a Jpk
 * Added extended data pointer type functions and declarations.
 * 
 * 20    8/08/00 9:42a Jpk
 * Added laser scan information.
 * 
 * 19    5/22/00 1:31p Ttc
 * SCR 10474.  dbNextFeatureNumber() now returns int.
 * 
 * 18    1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 17    11/11/99 10:26a Ttc
 * Add direction vector to the smear structure also.  No bump in db
 * version number, so any rtn stored between 10/29/99 and today (11/11/99)
 * is trash.
 * 
 * 16    10/25/99 2:57p Ttc
 * Add point direction vector.
 * 
 * 15    9/28/99 7:52a Ttc
 * Add feature compensated point location.
 * 
 * 14    9/15/99 9:34a Ttc
 * Delete prototypes for PointLength and PointAngle - no such functions
 * exist.
 * 
 * 13    8/25/99 8:58a Ttc
 * Add dbCheckCircleReferenceTypes prototype.
 * 
 * 12    7/06/99 3:10p Ttc
 * SCR 6627.  Add the z location for the start, middle and end weak edge /
 * edge trace points.  Weak edge and edge trace need the full 3D values
 * for all points so that they work after a plane align.
 * 
 * 11    6/03/99 3:02p Ttc
 * DN 9723 version 4. Add prototypes for nominal size 3 and tolerances.
 * 
 * 10    5/26/99 11:04a Ttc
 * Change Auxilary size and tolerance function names and internal variable
 * names to nominal size2, and upper and lower size2 tols in preparation
 * to add a nominal size 3 with tolerances for cone.
 * 
 * 9     5/25/99 9:57a Ttc
 * Delete QCHECK ifdef code and ifndefs.
 * 
 * 8     4/29/99 2:13p Ttc
 * Fix prototype parameter names.
 * 
 * 7     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 6     4/13/99 11:15a Ttc
 * Bump database version to 22.  Add angle orientations to result
 * structure.
 * 
 * 5     4/02/99 1:18p Acm
 * 
 * 4     3/17/99 9:03a Ttc
 * Add direction to results.
 * 
 *    Rev 10.7   02 Sep 1998 13:55:50   ttc
 * Add prototype for new dbTrustedGetFeatureFromDB which returns a pointer to
 * the feature directly in the database memory, not a copy of the feature.
 * If you use this function, do not dispose of the feature or you will corrupt
 * the routine in memory.
 * 
 *    Rev 10.6   24 Jul 1998 08:18:34   ttc
 * Add datum transform get and put prototype.s
 * 
 *    Rev 10.5   20 May 1998 15:31:58   ttc
 * Change f2 focus function prototypes.
 * 
 *    Rev 10.4   14 May 1998 16:07:28   ttc
 * Delete unneeded f2 prototypes.
 * 
 *    Rev 10.3   17 Apr 1998 11:12:40   ttc
 * Change dbCheckCircleReferenceFeatures to dbCheckCompositeReferenceFeatures
 * and pass the composite feature type so I can use it for circles or lines.
 * 
 *    Rev 10.2   14 Apr 1998 11:55:50   ttc
 * Add prototypes for extra dimension.
 * 
 *    Rev 10.1   20 Mar 1998 16:06:08   ttc
 * Add f2 function prototypes.
 * 
 *    Rev 10.0   20 Mar 1998 12:17:28   softadmn
 * Initial revision.
 * 
 *    Rev 9.9   15 Dec 1997 10:27:46   ttc
 * Add prototype for checking circle reference features.
 * 
 *    Rev 9.8   25 Aug 1997 08:59:46   ttc
 * Add prototypes for docking station (tp200).
 * 
 *    Rev 9.7   18 Apr 1997 16:10:10   dmf
 * Add SmartReport
 * 
 *    Rev 9.6   28 Jun 1996 08:34:18   ttc
 * Add prototypes for led flag.
 * 
 *    Rev 9.5   18 Jun 1996 13:24:04   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.4   20 May 1996 08:36:10   ttc
 * Add protos for smear led.
 * 
 *    Rev 9.3   18 May 1996 08:46:02   ttc
 * Add LED light.
 * 
 *    Rev 9.2   08 Apr 1996 16:13:16   ttc
 * Add prototypes for lens code, # measured points.  Change dbGoodNumberOfPoints
 * prototype.
 * 
 *    Rev 9.1   18 Mar 1996 12:48:04   ttc
 * Delete safe point prototypes.  Delete standard deviation prototypes.  Add 
 * miscellaneous flag put and get prototypes.
 * 
 *    Rev 9.0   11 Mar 1996 11:21:58   softadmn
 * Initial revision.
 * 
 *    Rev 8.5   27 Feb 1996 10:13:14   ttc
 * Add ifndefs for QCHECK.
 * 
 *    Rev 8.4   12 Dec 1995 10:25:52   ttc
 * Change function names for part repeat.
 * 
 *    Rev 8.3   05 Dec 1995 14:56:08   ttc
 * Add prototypes for new repeat functionality.
 * 
 *    Rev 8.2   01 Aug 1995 13:47:42   dmf
 * Added some prototypes for datastream and point elimination and touch probe
 * 
 *    Rev 8.1   27 Feb 1995 15:27:22   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 14:57:30   softadmn
 * Initial revision.
*/

int dbGetVersion(void);
int dbInitDataBase(void);
int dbPutFeatureInDB(Feature *this_feature);
int dbGetFeatureFromDB(Feature *this_feature, int feature_number);
/* the next function will return a direct pointer to database memory */
/* if you use this function, do not dispose of the feature after you do */
Feature *dbTrustedGetFeatureFromDB(int feature_number);

int dbResetDataBase(void);
int dbGetDataBase(char *routine_name);
int dbSaveDataBase(char *routine_name);
int dbGetLastFeatureNumber(void);
int dbResetLastFeatureNumber(void);
int dbSetLastFeatureNumber(int last_feature_number);

int dbSetupRunDataBase(void);
int dbGetFeatureForRun(Feature *this_feature, int feature_number);
int dbCloseRunDataBase(void);
int dbRunLastFeatureNumber(void);

Feature *dbNewFeature(void);
void dbDisposeFeature(Feature *this_feature);
void dbInitNewFeature(Feature *feature);
void dbInitFeature(Feature *feature);
void dbInitFeatureResult(Feature *feature);
void dbCopyFeature(Feature *destination, Feature *source);

int dbDeleteFeature(int feature_number);
int dbInsertFeature(int feature_number);

int dbNextFeatureNumber(Feature *this_feature);
int dbGetFeatureNumber(Feature *this_feature);
void dbOverrideFeatureNumber(Feature *this_feature, int this_feature_number);
int dbPutFeatureType(Feature *this_feature, int feature_type);
int dbGetFeatureType(Feature *this_feature);
int dbPutFeatureMode(Feature *this_feature, int measure_construct);
int dbGetFeatureMode(Feature *this_feature);
int dbIsFeatureMeasured(Feature *this_feature);
int dbIsFeatureConstructed(Feature *this_feature);
int dbPutFeaturePlane(Feature *this_feature, int feature_plane);
int dbGetFeaturePlane(Feature *this_feature);
int dbGetLastDisplay(Feature *this_feature);
int dbPutLastDisplay(Feature *this_feature, int last_displayed);
int dbGetLastDisplay2(Feature *this_feature);
int dbPutLastDisplay2(Feature *this_feature, int last_displayed);
int dbGetLastDisplay3(Feature *this_feature);
int dbPutLastDisplay3(Feature *this_feature, int last_displayed);
int dbPutFeatureLensCode(Feature *this_feature, int lens_code);
int dbGetFeatureLensCode(Feature *this_feature);
void dbPutFeatureDockLocation(Feature *this_feature, int dock);
int dbGetFeatureDockLocation(Feature *this_feature);

int dbIncNumberPoints(Feature *this_feature);
int dbDecNumberPoints(Feature *this_feature);
int dbGetNumberPoints(Feature *this_feature);
void dbPutNumberPoints(Feature *this_feature, int number_points);
int dbMinimumFeaturePoints(Feature *this_feature);
int dbGoodLineCompositeRefFeatType(int ref_feature_type);
int dbGoodCircleCompositeRefFeatType(int ref_feature_type);
int dbGoodPlaneCompositeRefFeatType(int ref_feature_type);
int dbGoodCylinderCompositeRefFeatType(int ref_feature_type);
int dbGoodConeCompositeRefFeatType(int ref_feature_type);
int dbGoodSphereCompositeRefFeatType(int ref_feature_type);
int dbGoodContourCompositeRefFeatType(int ref_feature_type);
int dbCheckCompositeReferenceFeatures(Feature *this_feature,
									  int composite_feat_type);
int dbCheckCircleReferenceTypes(Feature *this_feature);
int dbEnoughFeaturePoints(Feature *this_feature, int number_of_points);
int dbGoodNumberOfPoints(Feature *this_feature, int number_of_points);
int dbGetNumberMissedPoints(Feature *this_feature);
void dbPutNumberMissedPoints(Feature *this_feature, int these_missed);
int dbGetNumberMeasuredPoints(Feature *this_feature);
void dbPutNumberMeasuredPoints(Feature *this_feature, int these_measured);

int dbPutPointX(Feature *this_feature, int point_number, double new_x);
double dbGetPointX(Feature *this_feature, int point_number);
int dbPutPointY(Feature *this_feature, int point_number, double new_y);
double dbGetPointY(Feature *this_feature, int point_number);
int dbPutPointZ(Feature *this_feature, int point_number, double new_z);
double dbGetPointZ(Feature *this_feature, int point_number);
int dbPutActPointX(Feature *this_feature, int point_number, double new_x);
double dbGetActPointX(Feature *this_feature, int point_number);
int dbPutActPointY(Feature *this_feature, int point_number, double new_y);
double dbGetActPointY(Feature *this_feature, int point_number);
int dbPutActPointZ(Feature *this_feature, int point_number, double new_z);
double dbGetActPointZ(Feature *this_feature, int point_number);
int dbPutCompPointX(Feature *this_feature, int point_number, double new_x);
double dbGetCompPointX(Feature *this_feature, int point_number);
int dbPutCompPointY(Feature *this_feature, int point_number, double new_y);
double dbGetCompPointY(Feature *this_feature, int point_number);
int dbPutCompPointZ(Feature *this_feature, int point_number, double new_z);
double dbGetCompPointZ(Feature *this_feature, int point_number);
int dbPutPointDirectionVectorX(Feature *this_feature, int point_number, double new_x);
double dbGetPointDirectionVectorX(Feature *this_feature, int point_number);
int dbPutPointDirectionVectorY(Feature *this_feature, int point_number, double new_y);
double dbGetPointDirectionVectorY(Feature *this_feature, int point_number);
int dbPutPointDirectionVectorZ(Feature *this_feature, int point_number, double new_z);
double dbGetPointDirectionVectorZ(Feature *this_feature, int point_number);
int dbPutPointZoom(Feature *this_feature, int point_number, double new_zoom);
double dbGetPointZoom(Feature *this_feature, int point_number);

int dbPutPointBackLight(Feature *this_feature, int point_number, int new_light);
int dbGetPointBackLight(Feature *this_feature, int point_number);
int dbPutPointRingLight(Feature *this_feature, int point_number, int new_light);
int dbGetPointRingLight(Feature *this_feature, int point_number);
int dbPutPointAuxLight(Feature *this_feature, int point_number, int new_light);
int dbGetPointAuxLight(Feature *this_feature, int point_number);
int dbPutPointLedIntensity(Feature *this_feature, int point_number,
						   int sector, int ring, int new_intensity);
int dbGetPointLedIntensity(Feature *this_feature, int point_number,
						   int sector, int ring);
int dbPutPointLedFlag(Feature *this_feature, int point_number, int flag);
int dbGetPointLedFlag(Feature *this_feature, int point_number);

int dbPutPointTargetType(Feature *this_feature, int point_number, int new_type);
int dbGetPointTargetType(Feature *this_feature, int point_number);
int dbPutPointTargetColor(Feature *this_feature, int point_number,
						  int new_color);
int dbGetPointTargetColor(Feature *this_feature, int point_number);
int dbPutPointTargetSpacing(Feature *this_feature, int point_number,
							int new_spacing1, int new_spacing2);
int dbGetPointTargetSpacing1(Feature *this_feature, int point_number);
int dbGetPointTargetSpacing2(Feature *this_feature, int point_number);
int dbPutPointTargetFilename(Feature *this_feature, int point_number,
							 char *user_filename);
char *dbGetPointTargetFilename(Feature *this_feature, int point_number);

int dbFocusParametersExist(Feature *this_feature, int point_number);
int dbInitFocusData(Feature *this_feature, int point_number);

int dbPutFocusNoise(Feature *this_feature, int point_number, int value);
int dbGetFocusNoise(Feature *this_feature, int point_number);
int dbPutFocusStepSize(Feature *this_feature, int point_number, int value);
int dbGetFocusStepSize(Feature *this_feature, int point_number);
int dbPutFocusBackoff(Feature *this_feature, int point_number, int value);
int dbGetFocusBackoff(Feature *this_feature, int point_number);
int dbPutFocusScanFlag(Feature *this_feature, int point_number, int value);
int dbGetFocusScanFlag(Feature *this_feature, int point_number);

int dbPutPointEdgeSeek(Feature *this_feature, int point_number, int new_seek);
int dbGetPointEdgeSeek(Feature *this_feature, int point_number);
int dbPutPointMissedFlag(Feature *this_feature, int point_number, int missed_it);
int dbGetPointMissedFlag(Feature *this_feature, int point_number);

int dbPutRefFeatureNumber(Feature *this_feature, int point_number,
						  int reference_feature);
int dbGetRefFeatureNumber(Feature *this_feature, int point_number);
int dbPutPointSideNum(Feature *this_feature, int point_number, int side_number);
int dbGetPointSideNum(Feature *this_feature, int point_number);

int dbPutBasicDimension(Feature *this_feature, double dimension);
double dbGetBasicDimension(Feature *this_feature);
int dbPutExtraDimension(Feature *this_feature, double dimension);
double dbGetExtraDimension(Feature *this_feature);

int dbPutInMmFlag(Feature *this_feature, int in_mm_flag);
int dbGetInMmFlag(Feature *this_feature);
int dbPutCartPolarFlag(Feature *this_feature, int cart_polar_flag);
int dbGetCartPolarFlag(Feature *this_feature);
int dbPutDecDmsFlag(Feature *this_feature, int angle_flag);
int dbGetDecDmsFlag(Feature *this_feature);
int dbPutStatisticsFlags(Feature *this_feature, int stats_flags);
int dbGetStatisticsFlags(Feature *this_feature);
int dbPutReportFlags(Feature *this_feature, int report_flags);
int dbGetReportFlags(Feature *this_feature);
int dbPutExportFlags(Feature *this_feature, int export_flags);
int dbGetExportFlags(Feature *this_feature);
int dbPutPrintFlags(Feature *this_feature, int print_flags);
int dbGetPrintFlags(Feature *this_feature);
void dbPutDataStreamFlag(Feature *this_feature, int on_or_off);
int dbGetDataStreamFlag(Feature *this_feature);
void dbPutMiscFlag(Feature *this_feature, int misc_flag);
int dbGetMiscFlag(Feature *this_feature);
void dbSetFeatureFailed(Feature *this_feature, int failed_flag);
int dbGetFeatureFailed(Feature *this_feature);

int dbPutFeatureDeployLaserFlag(Feature *this_feature, int deploy_laser);
int dbGetFeatureDeployLaserFlag(Feature *this_feature);

int dbPutFeatureComment(Feature *this_feature, char *new_comment);
char *dbGetFeatureComment(Feature *this_feature);
int dbPutFeaturePrompt(Feature *this_feature, char *new_prompt);
char *dbGetFeaturePrompt(Feature *this_feature);
int dbPutFeatureExpression(Feature *this_feature, char *new_expression);
char *dbGetFeatureExpression(Feature *this_feature);

int dbPutReportLabel(Feature *this_feature, char *new_label, int index);
char *dbGetReportLabel(Feature *this_feature, int index);
int dbPutExportFieldName(Feature *this_feature, char *new_fieldname, int index);
char *dbGetExportFieldName(Feature *this_feature, int index);

void dbPutNumberLocations(Feature *this_feature, int number_locs);
int dbGetNumberLocations(Feature *this_feature);
void dbPutActualLocX(Feature *this_feature, double new_x);
double dbGetActualLocX(Feature *this_feature);
void dbPutActualLocY(Feature *this_feature, double new_y);
double dbGetActualLocY(Feature *this_feature);
void dbPutActualLocZ(Feature *this_feature, double new_z);
double dbGetActualLocZ(Feature *this_feature);
void dbPutNumberSizes(Feature *this_feature, int number_sizes);
int dbGetNumberSizes(Feature *this_feature);
void dbPutActualSize(Feature *this_feature, double new_size);
double dbGetActualSize(Feature *this_feature, int which_one);
void dbPutActualDeviation(Feature *this_feature, double deviation);
double dbGetActualDeviation(Feature *this_feature);
void dbPutSecondLocX(Feature *this_feature, double new_x);
double dbGetSecondLocX(Feature *this_feature);
void dbPutSecondLocY(Feature *this_feature, double new_y);
double dbGetSecondLocY(Feature *this_feature);
void dbPutSecondLocZ(Feature *this_feature, double new_z);
double dbGetSecondLocZ(Feature *this_feature);

void dbPutDirectionX(Feature *this_feature, double axis_x);
double dbGetDirectionX(Feature *this_feature);
void dbPutDirectionY(Feature *this_feature, double axis_y);
double dbGetDirectionY(Feature *this_feature);
void dbPutDirectionZ(Feature *this_feature, double axis_z);
double dbGetDirectionZ(Feature *this_feature);

void dbPutActualOrientation(Feature *this_feature, double new_angle, int which_one);
double dbGetActualOrientation(Feature *this_feature, int which_one);

void dbPutActualPosTol(Feature *this_feature, double pos_tol);
double dbGetActualPosTol(Feature *this_feature);
void dbPutModifiedPosTol(Feature *this_feature, double modifier);
double dbGetModifiedPosTol(Feature *this_feature);

void dbPutNominalLocX(Feature *this_feature, double nom_x);
double dbGetNominalLocX(Feature *this_feature);
void dbPutNominalLocY(Feature *this_feature, double nom_y);
double dbGetNominalLocY(Feature *this_feature);
void dbPutNominalLocZ(Feature *this_feature, double nom_z);
double dbGetNominalLocZ(Feature *this_feature);

void dbPutUpperLocTolX(Feature *this_feature, double x_tol);
double dbGetUpperLocTolX(Feature *this_feature);
void dbPutLowerLocTolX(Feature *this_feature, double x_tol);
double dbGetLowerLocTolX(Feature *this_feature);
void dbPutUpperLocTolY(Feature *this_feature, double y_tol);
double dbGetUpperLocTolY(Feature *this_feature);
void dbPutLowerLocTolY(Feature *this_feature, double y_tol);
double dbGetLowerLocTolY(Feature *this_feature);
void dbPutUpperLocTolZ(Feature *this_feature, double z_tol);
double dbGetUpperLocTolZ(Feature *this_feature);
void dbPutLowerLocTolZ(Feature *this_feature, double z_tol);
double dbGetLowerLocTolZ(Feature *this_feature);

void dbPutNominalSize(Feature *this_feature, double nom_size);
double dbGetNominalSize(Feature *this_feature);
void dbPutUpperSizeTol(Feature *this_feature, double size_tol);
double dbGetUpperSizeTol(Feature *this_feature);
void dbPutLowerSizeTol(Feature *this_feature, double size_tol);
double dbGetLowerSizeTol(Feature *this_feature);

void dbPutNominalSize2(Feature *this_feature, double nom_size);
double dbGetNominalSize2(Feature *this_feature);
void dbPutUpperSize2Tol(Feature *this_feature, double size_tol);
double dbGetUpperSize2Tol(Feature *this_feature);
void dbPutLowerSize2Tol(Feature *this_feature, double size_tol);
double dbGetLowerSize2Tol(Feature *this_feature);

void dbPutNominalElevationAngle(Feature *this_feature, double nom_elev);
double dbGetNominalElevationAngle(Feature *this_feature);
void dbPutUpperElevationTol(Feature *this_feature, double elev_tol);
double dbGetUpperElevationTol(Feature *this_feature);
void dbPutLowerElevationTol(Feature *this_feature, double elev_tol);
double dbGetLowerElevationTol(Feature *this_feature);

void dbPutNominalSize3(Feature *this_feature, double nom_size);
double dbGetNominalSize3(Feature *this_feature);
void dbPutUpperSize3Tol(Feature *this_feature, double size_tol);
double dbGetUpperSize3Tol(Feature *this_feature);
void dbPutLowerSize3Tol(Feature *this_feature, double size_tol);
double dbGetLowerSize3Tol(Feature *this_feature);

void dbPutNominalFormTol(Feature *this_feature, double form_tol);
double dbGetNominalFormTol(Feature *this_feature);

void dbPutNominalPosTol(Feature *this_feature, double pos_tol);
double dbGetNominalPosTol(Feature *this_feature);

void dbPutNominalFlags(Feature *this_feature, int flag1, int flag2);
void dbGetNominalFlags(Feature *this_feature, int *flag1, int *flag2);

void dbPutToleranceType(Feature *this_feature, int tol_type);
int dbGetToleranceType(Feature *this_feature);

void dbSetNominalsEntered(Feature *this_feature);
void dbPutNominalsEntered(Feature *this_feature, int nom_entered);
int dbGetNominalsEntered(Feature *this_feature);

int dbPutSearchOnStep(Feature *this_feature, int which, int step);
int dbGetSearchOnStep(Feature *this_feature, int which);

int dbGetFixtureRepeatFlag(void);
void dbSetFixtureRepeatFlag(int fixture_repeat);
int dbGetRepeatOffsetOnOff(int which_offset);
void dbPutRepeatOffsetOnOff(int which_offset, int on_or_off);
double dbGetRepeatXOffset(int which_offset);
void dbPutRepeatXOffset(int which_offset, double x_offset);
double dbGetRepeatYOffset(int which_offset);
void dbPutRepeatYOffset(int which_offset, double y_offset);
int dbGetRepeatConfirmNext(void);
void dbPutRepeatConfirmNext(int to_confirm_next_or_not);

int dbGetRepeatFinishRun(void);
void dbPutRepeatFinishRun(int to_finish_or_not_to_finish);
int dbGetRepeatStartStep(void);
void dbPutRepeatStartStep(int start_step);
int dbGetRepeatEndStep(void);
void dbPutRepeatEndStep(int start_step);


int dbGetPartRepeatFlag(void);
void dbSetPartRepeatFlag(int part_repeat);
double dbGetXPartOffset(void);
void dbPutXPartOffset(double x_offset);
double dbGetYPartOffset(void);
void dbPutYPartOffset(double y_offset);
int dbGetXPartTimes(void);
void dbPutXPartTimes(int number_times);
int dbGetYPartTimes(void);
void dbPutYPartTimes(int number_times);
int dbGetYPartFirst(void);
void dbPutYPartFirst(int y_first);

void dbPutSmearType(Feature *this_feature, int type);
int dbGetSmearType(Feature *this_feature);
void dbPutSmearCenterX(Feature *this_feature, double x);
double dbGetSmearCenterX(Feature *this_feature);
void dbPutSmearCenterY(Feature *this_feature, double y);
double dbGetSmearCenterY(Feature *this_feature);
void dbPutSmearCenterZ(Feature *this_feature, double z);
double dbGetSmearCenterZ(Feature *this_feature);
void dbPutSmearZoom(Feature *this_feature, double zoom);
double dbGetSmearZoom(Feature *this_feature);
void dbPutSmearBackLight(Feature *this_feature, int back_lite);
int dbGetSmearBackLight(Feature *this_feature);
void dbPutSmearRingLight(Feature *this_feature, int ring_lite);
int dbGetSmearRingLight(Feature *this_feature);
void dbPutSmearAuxLight(Feature *this_feature, int aux_lite);
int dbGetSmearAuxLight(Feature *this_feature);
int dbPutSmearLedIntensity(Feature *this_feature, int sector, int ring,
						   int new_intensity);
int dbGetSmearLedIntensity(Feature *this_feature, int sector, int ring);
void dbPutSmearLedFlag(Feature *this_feature, int flag);
int dbGetSmearLedFlag(Feature *this_feature);

void dbPutSmearStartPtX(Feature *this_feature, double x);
double dbGetSmearStartPtX(Feature *this_feature);
void dbPutSmearStartPtY(Feature *this_feature, double y);
double dbGetSmearStartPtY(Feature *this_feature);
void dbPutSmearStartPtZ(Feature *this_feature, double z);
double dbGetSmearStartPtZ(Feature *this_feature);
void dbPutSmearEndPtX(Feature *this_feature, double x);
double dbGetSmearEndPtX(Feature *this_feature);
void dbPutSmearEndPtY(Feature *this_feature, double y);
double dbGetSmearEndPtY(Feature *this_feature);
void dbPutSmearEndPtZ(Feature *this_feature, double z);
double dbGetSmearEndPtZ(Feature *this_feature);
void dbPutSmearMidPtX(Feature *this_feature, double x);
double dbGetSmearMidPtX(Feature *this_feature);
void dbPutSmearMidPtY(Feature *this_feature, double y);
double dbGetSmearMidPtY(Feature *this_feature);
void dbPutSmearMidPtZ(Feature *this_feature, double z);
double dbGetSmearMidPtZ(Feature *this_feature);
void dbPutSmearDirectionVectorX(Feature *this_feature, double x);
double dbGetSmearDirectionVectorX(Feature *this_feature);
void dbPutSmearDirectionVectorY(Feature *this_feature, double y);
double dbGetSmearDirectionVectorY(Feature *this_feature);
void dbPutSmearDirectionVectorZ(Feature *this_feature, double z);
double dbGetSmearDirectionVectorZ(Feature *this_feature);
void dbPutSmearDirection(Feature *this_feature, int dir);
int dbGetSmearDirection(Feature *this_feature);

void dbPutSmearBounds(Feature *this_feature, int bounds);
int dbGetSmearBounds(Feature *this_feature);
void dbPutSmearExtent(Feature *this_feature, int extent);
int dbGetSmearExtent(Feature *this_feature);
void dbPutSmearMinQuality(Feature *this_feature, int min_quality);
int dbGetSmearMinQuality(Feature *this_feature);
void dbPutSmearContrast(Feature *this_feature, int contrast);
int dbGetSmearContrast(Feature *this_feature);
void dbPutSmearWgtNominal(Feature *this_feature, int wgt_nominal);
int dbGetSmearWgtNominal(Feature *this_feature);
void dbPutSmearWgtContrast(Feature *this_feature, int wgt_contrast);
int dbGetSmearWgtContrast(Feature *this_feature);
void dbPutSmearWgtFirst(Feature *this_feature, int wgt_first_edge);
int dbGetSmearWgtFirst(Feature *this_feature);
void dbPutSmearWgtLast(Feature *this_feature, int wgt_last_edge);
int dbGetSmearWgtLast(Feature *this_feature);
void dbPutSmearWgtSecond(Feature *this_feature, int wgt_second);
int dbGetSmearWgtSecond(Feature *this_feature);
void dbPutSmearWgt2Last(Feature *this_feature, int wgt_second_last);
int dbGetSmearWgt2Last(Feature *this_feature);
void dbPutSmearStdDev(Feature *this_feature, int standard_deviation);
int dbGetSmearStdDev(Feature *this_feature);

void dbPutFeaturePartTransform(Feature *this_feature, Database_Transform *this_transform);
void dbGetFeaturePartTransform(Feature *this_feature, Database_Transform *this_transform);

int dbGetFeatureExtendedDataType(Feature *this_feature);
int dbPutFeatureExtendedDataType(Feature *this_feature, int new_type);

int dbLaserScanParametersExist(Feature *this_feature);
int dbInitLaserScanData(Feature *this_feature);

int dbGetLaserPointFrequencyType(Feature *this_feature);
int dbPutLaserPointFrequencyType(Feature *this_feature, int value);
double dbGetLaserPointFrequency(Feature *this_feature);
int dbPutLaserPointFrequency(Feature *this_feature, double value);
int dbGetLaserScanStrategy(Feature *this_feature);
int dbPutLaserScanStrategy(Feature *this_feature, int value);
int dbGetLaserTrackingScan(Feature *this_feature);
int dbPutLaserTrackingScan(Feature *this_feature, int value);
int dbGetLaserLineFrequencyType(Feature *this_feature);
int dbPutLaserLineFrequencyType(Feature *this_feature, int value);
double dbGetLaserLineFrequency(Feature *this_feature);
int dbPutLaserLineFrequency(Feature *this_feature, double value);
double dbGetLaserDefinitionPointX(Feature *this_feature, int point_num);
int dbPutLaserDefinitionPointX(Feature *this_feature, int point_num, double value);
double dbGetLaserDefinitionPointY(Feature *this_feature, int point_num);
int dbPutLaserDefinitionPointY(Feature *this_feature, int point_num, double value);
double dbGetLaserDefinitionPointZ(Feature *this_feature, int point_num);
int dbPutLaserDefinitionPointZ(Feature *this_feature, int point_num, double value);
int dbGetLaserThreshold(Feature *this_feature);
int dbPutLaserThreshold(Feature *this_feature, int value);
int dbGetLaserMaxExposure(Feature *this_feature);
int dbPutLaserMaxExposure(Feature *this_feature, int value);
int dbGetLaserAverages(Feature *this_feature);
int dbPutLaserAverages(Feature *this_feature, int value);
int dbGetLaserIDNum(Feature *this_feature);
int dbPutLaserIDNum(Feature *this_feature, int value);
int dbGetLaserScanSpeedFactor(Feature *this_feature);
int dbPutLaserScanSpeedFactor(Feature *this_feature, int value);
int dbCopyLaserData(Feature *destination, Feature *source);

int dbInitTouchProbeData(Feature *this_feature);
int dbCopyTouchProbeData(Feature *destination, Feature *source);
int dbTouchProbeDataExist(Feature *this_feature);

int dbGetIsCleanFlag(Feature *this_feature);
int dbPutIsCleanFlag(Feature *this_feature, int value);
int dbGetSafeFlag(Feature *this_feature);
int dbPutSafeFlag(Feature *this_feature, int value);
double dbGetSafeValue(Feature *this_feature);
int dbPutSafeValue(Feature *this_feature, double value);
double dbGetApproachValue(Feature *this_feature);
int dbPutApproachValue(Feature *this_feature, double value);
double dbGetXYAngleValue(Feature *this_feature);
int dbPutXYAngleValue(Feature *this_feature, double value);
double dbGetElevationValue(Feature *this_feature);
int dbPutElevationValue(Feature *this_feature, double value);
int dbGetPlusValue(Feature *this_feature);
int dbPutPlusValue(Feature *this_feature, int value);
double dbGetSpacingValue(Feature *this_feature);
int dbPutSpacingValue(Feature *this_feature, double value);
int dbGetPointsValue(Feature *this_feature);
int dbPutPointsValue(Feature *this_feature, int value);
double dbGetDistTopValue(Feature *this_feature);
int dbPutDistTopValue(Feature *this_feature, double value);
double dbGetDistBottomValue(Feature *this_feature);
int dbPutDistBottomValue(Feature *this_feature, double value);
int dbGetInternalValue(Feature *this_feature);
int dbPutInternalValue(Feature *this_feature, int value);
double dbGetStartAngleValue(Feature *this_feature);
int dbPutStartAngleValue(Feature *this_feature, double value);
double dbGetEndAngleValue(Feature *this_feature);
int dbPutEndAngleValue(Feature *this_feature, double value);
double dbGetStartElevationValue(Feature *this_feature);
int dbPutStartElevationValue(Feature *this_feature, double value);
double dbGetEndElevationValue(Feature *this_feature);
int dbPutEndElevationValue(Feature *this_feature, double value);
int dbGetTotalScansValue(Feature *this_feature);
int dbPutTotalScansValue(Feature *this_feature, int value);
int dbGetTotalLongScansValue(Feature *this_feature);
int dbPutTotalLongScansValue(Feature *this_feature, int value);
double dbGetTotalRevsValue(Feature *this_feature);
int dbPutTotalRevsValue(Feature *this_feature, double value);
double dbGetRowSpacingValue(Feature *this_feature);
int dbPutRowSpacingValue(Feature *this_feature, double value);
int dbPutTotalRowValue(Feature *this_feature, int value);
int dbGetTotalRowValue(Feature *this_feature);
int dbGetSpacingFlag(Feature *this_feature);
int dbPutSpacingFlag(Feature *this_feature, int value);
int dbGetRowSpacingFlag(Feature *this_feature);
int dbPutRowSpacingFlag(Feature *this_feature, int value);
int dbGetXYAngleFlag(Feature *this_feature);
int dbPutXYAngleFlag(Feature *this_feature, int value);
int dbGetElevationFlag(Feature *this_feature);
int dbPutElevationFlag(Feature *this_feature, int value);
int dbGetNumberBoundaryPoints(Feature *this_feature);
int dbPutNumberBoundaryPoints(Feature *this_feature, int number_points);
double dbGetPlaneBoundaryPointX(Feature *this_feature, int point_num);
double dbGetPlaneBoundaryPointY(Feature *this_feature, int point_num);
double dbGetPlaneBoundaryPointZ(Feature *this_feature, int point_num);
int dbPutPlaneBoundaryPointX(Feature *this_feature, int point_num, double boundary_x);
int dbPutPlaneBoundaryPointY(Feature *this_feature, int point_num, double boundary_y);
int dbPutPlaneBoundaryPointZ(Feature *this_feature, int point_num, double boundary_z);







