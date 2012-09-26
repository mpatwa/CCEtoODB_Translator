/*
$Workfile: Db_utils.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_utils.cpp 21    3/26/01 8:32a Jpk $

$Modtime: 3/22/01 2:02p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_utils.cpp $
 * 
 * 21    3/26/01 8:32a Jpk
 * Changed name of new laser parameter from dock to deploy.
 * 
 * 20    3/22/01 8:25a Jpk
 * Added new flag for deployable laser to the flags structure.
 * 
 * 19    10/06/00 1:52p Ttc
 * SCR 11792.  Add output precision override to export and stats.  Major
 * output overhaul.
 * 
 * 18    8/29/00 1:02p Lsz
 * Added call to copy of touchprobe data.
 * 
 * 17    8/14/00 8:03a Jpk
 * Added extended data pointer type value to eliminate database errors and
 * expand the usefullness of the pointer.
 * 
 * 16    8/08/00 9:38a Jpk
 * Added copy of laser data to the feature copy function.
 * 
 * 15    1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 14    11/11/99 10:26a Ttc
 * Add direction vector to the smear structure also.  No bump in db
 * version number, so any rtn stored between 10/29/99 and today (11/11/99)
 * is trash.
 * 
 * 13    10/25/99 2:58p Ttc
 * Add point direction vector for rotary integration.
 * 
 * 12    9/28/99 7:52a Ttc
 * Add feature compensated point location.
 * 
 * 11    7/06/99 3:10p Ttc
 * SCR 6627.  Add the z location for the start, middle and end weak edge /
 * edge trace points.  Weak edge and edge trace need the full 3D values
 * for all points so that they work after a plane align.
 * 
 * 10    6/04/99 11:15a Ttc
 * Fix copy of nominal size 3 and tolerances.
 * 
 * 9     6/04/99 9:33a Ttc
 * DN 9723 version 4. Forgot the copy of nominal 3 and tolerances.
 * 
 * 8     5/26/99 11:04a Ttc
 * Change Auxilary size and tolerance function names and internal variable
 * names to nominal size2, and upper and lower size2 tols in preparation
 * to add a nominal size 3 with tolerances for cone.
 * 
 * 7     5/25/99 9:58a Ttc
 * Delete QCHECK ifdef code and ifndefs.
 * 
 * 6     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 * 5     4/13/99 11:14a Ttc
 * Add angle orientations.
 * 
 * 4     3/17/99 9:05a Ttc
 * Add direction to results.
 * 
 *    Rev 10.3   24 Jul 1998 08:21:58   ttc
 * Add copy of feature datum transforms.
 * 
 *    Rev 10.2   20 May 1998 15:34:40   ttc
 * Add copy of f2 focus parameters.
 * 
 *    Rev 10.1   14 Apr 1998 12:02:08   ttc
 * Add extra dimension for edge trace.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:18   softadmn
 * Initial revision.
 * 
 *    Rev 9.8   25 Aug 1997 09:15:36   ttc
 * Add tp200 dock station.
 * 
 *    Rev 9.7   18 Apr 1997 16:02:16   dmf
 * Added SmartReport
 * 
 *    Rev 9.6   28 Jun 1996 08:36:08   ttc
 * Add led flag.
 * 
 *    Rev 9.5   18 Jun 1996 13:24:58   ttc
 * Change led ring light to handle sector or ring control.
 * 
 *    Rev 9.4   20 May 1996 14:07:24   ttc
 * Add smear led lights.
 * 
 *    Rev 9.3   18 May 1996 08:46:32   ttc
 * Add LED light.
 * 
 *    Rev 9.2   08 Apr 1996 16:49:14   ttc
 * Add copy of number measured points and lens code.
 * 
 *    Rev 9.1   18 Mar 1996 13:03:52   ttc
 * Add miscellaneous flag.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:50   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:16   softadmn
 * Initial revision.
 * 
 *    Rev 8.3   27 Feb 1996 10:14:40   ttc
 * Add ifndefs for QCHECK.
 * 
 *    Rev 8.2   01 Aug 1995 13:39:52   dmf
 * Some stuff for the future: touch probe, directional point, data stream
 * 
 *    Rev 8.1   27 Feb 1995 15:37:06   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:24   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stdio.h>

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_decl.h"
#include "db_proto.h"

void dbCopyPointData(Feature *destination, Feature *source)
{
	char *loc_func_name = "dbCopyPointData";
	int number_of_points;
	int i,j,k;
	char *target_filename = NULL;

	/* copy the feature point list from the source to the destination */
	number_of_points = dbGetNumberPoints(source);
	for (i = 1; i <= number_of_points; i++)
	{
		if (dbGetNumberPoints(destination) < i)
		{
			if (dbIncNumberPoints(destination) != i)
			{
				errPost(loc_func_name,DATABASE_ERROR,INC_POINT_FAILED,NULL,i);
			}
		}
		if (dbIsFeatureMeasured(source))
		{
			if (dbPutPointX(destination,i,dbGetPointX(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point location",i);
			}
			if (dbPutPointY(destination,i,dbGetPointY(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point location",i);
			}
			if (dbPutPointZ(destination,i,dbGetPointZ(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point location",i);
			}
			if (dbPutActPointX(destination,i,dbGetActPointX(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point location",i);
			}
			if (dbPutActPointY(destination,i,dbGetActPointY(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point location",i);
			}
			if (dbPutActPointZ(destination,i,dbGetActPointZ(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point location",i);
			}
			if (dbPutCompPointX(destination,i,dbGetCompPointX(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point location",i);
			}
			if (dbPutCompPointY(destination,i,dbGetCompPointY(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point location",i);
			}
			if (dbPutCompPointZ(destination,i,dbGetCompPointZ(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point location",i);
			}
			if (dbPutPointDirectionVectorX(destination,i,dbGetPointDirectionVectorX(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"x point vector",i);
			}
			if (dbPutPointDirectionVectorY(destination,i,dbGetPointDirectionVectorY(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"y point vector",i);
			}
			if (dbPutPointDirectionVectorZ(destination,i,dbGetPointDirectionVectorZ(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"z point vector",i);
			}
			if (dbPutPointZoom(destination,i,dbGetPointZoom(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point zoom location",i);
			}
			if (dbPutPointBackLight(destination,i,dbGetPointBackLight(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point back light",i);
			}
			if (dbPutPointRingLight(destination,i,dbGetPointRingLight(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point ring light",i);
			}
			if (dbPutPointAuxLight(destination,i,dbGetPointAuxLight(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point aux light",i);
			}
			for (j = 1; j <= MAX_LED_SECTORS; j++)
			{
				for (k = 1; k <= MAX_LED_RINGS; k++)
				{
					if (dbPutPointLedIntensity(destination,i,j,k,dbGetPointLedIntensity(source,i,j,k)) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point led sector intensity",i);
					}
				}
			}
			if (dbPutPointLedFlag(destination,i,dbGetPointLedFlag(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point led sector flag",i);
			}
			if (dbPutPointTargetType(destination,i,dbGetPointTargetType(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target type",i);
			}
			if (dbPutPointTargetColor(destination,i,dbGetPointTargetColor(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target color",i);
			}
			if (dbPutPointTargetSpacing(destination,i,dbGetPointTargetSpacing1(source,i),dbGetPointTargetSpacing2(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target spacing",i);
			}
			target_filename = dbGetPointTargetFilename(source,i);
			if (target_filename != NULL)
			{
				if (dbPutPointTargetFilename(destination,i,target_filename) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target filename",i);
				}
			}
			if (dbFocusParametersExist(source,i))
			{
				if ((j = dbGetFocusBackoff(source,i)) != NO_ERROR)
				{
					if (dbPutFocusBackoff(destination,i,j) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus backoff",i);
					}
				}
				if ((j = dbGetFocusStepSize(source,i)) != NO_ERROR)
				{
					if (dbPutFocusStepSize(destination,i,j) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus step size",i);
					}
				}
				if ((j = dbGetFocusNoise(source,i)) != NO_ERROR)
				{
					if (dbPutFocusNoise(destination,i,j) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus noise",i);
					}
				}
				if ((j = dbGetFocusScanFlag(source,i)) != NO_ERROR)
				{
					if (dbPutFocusScanFlag(destination,i,j) != NO_ERROR)
					{
						errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point focus scan flag",i);
					}
				}
			}
			if (dbPutPointEdgeSeek(destination,i,dbGetPointEdgeSeek(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point edge seek",i);
			}
			if (dbPutPointSideNum(destination,i,dbGetPointSideNum(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point side num",i);
			}
			if (dbPutPointMissedFlag(destination,i,dbGetPointMissedFlag(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point missed flag",i);
			}
		}
		else
		{
			if (dbPutRefFeatureNumber(destination,i,dbGetRefFeatureNumber(source,i)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"reference feature number",i);
			}
		}
	}
	dbPutNumberMissedPoints(destination,dbGetNumberMissedPoints(source));
	dbPutNumberMeasuredPoints(destination,dbGetNumberMeasuredPoints(source));
	return;
}

void dbCopySmearData(Feature *destination, Feature *source)
{
	char *loc_func_name = "dbCopySmearData";
	int i,j;

	dbPutSmearType(destination,dbGetSmearType(source));
	dbPutSmearCenterX(destination,dbGetSmearCenterX(source));
	dbPutSmearCenterY(destination,dbGetSmearCenterY(source));
	dbPutSmearCenterZ(destination,dbGetSmearCenterZ(source));
	dbPutSmearZoom(destination,dbGetSmearZoom(source));
	dbPutSmearBackLight(destination,dbGetSmearBackLight(source));
	dbPutSmearRingLight(destination,dbGetSmearRingLight(source));
	dbPutSmearAuxLight(destination,dbGetSmearAuxLight(source));
	for (i = 1; i <= MAX_LED_SECTORS; i++)
	{
		for (j = 1; j <= MAX_LED_RINGS; j++)
		{
			if (dbPutSmearLedIntensity(destination,i,j,dbGetSmearLedIntensity(source,i,j)) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"weak edge led sector intensity",i);
			}
		}
	}
	dbPutSmearLedFlag(destination,dbGetSmearLedFlag(source));
	dbPutSmearStartPtX(destination,dbGetSmearStartPtX(source));
	dbPutSmearStartPtY(destination,dbGetSmearStartPtY(source));
	dbPutSmearStartPtZ(destination,dbGetSmearStartPtZ(source));
	dbPutSmearEndPtX(destination,dbGetSmearEndPtX(source));
	dbPutSmearEndPtY(destination,dbGetSmearEndPtY(source));
	dbPutSmearEndPtZ(destination,dbGetSmearEndPtZ(source));
	dbPutSmearMidPtX(destination,dbGetSmearMidPtX(source));
	dbPutSmearMidPtY(destination,dbGetSmearMidPtY(source));
	dbPutSmearMidPtZ(destination,dbGetSmearMidPtZ(source));
	dbPutSmearDirectionVectorX(destination,dbGetSmearDirectionVectorX(source));
	dbPutSmearDirectionVectorY(destination,dbGetSmearDirectionVectorY(source));
	dbPutSmearDirectionVectorZ(destination,dbGetSmearDirectionVectorZ(source));
	dbPutSmearDirection(destination,dbGetSmearDirection(source));
	dbPutSmearBounds(destination,dbGetSmearBounds(source));
	dbPutSmearExtent(destination,dbGetSmearExtent(source));
	dbPutSmearMinQuality(destination,dbGetSmearMinQuality(source));
	dbPutSmearContrast(destination,dbGetSmearContrast(source));
	dbPutSmearWgtNominal(destination,dbGetSmearWgtNominal(source));
	dbPutSmearWgtContrast(destination,dbGetSmearWgtContrast(source));
	dbPutSmearWgtFirst(destination,dbGetSmearWgtFirst(source));
	dbPutSmearWgtLast(destination,dbGetSmearWgtLast(source));
	dbPutSmearWgtSecond(destination,dbGetSmearWgtSecond(source));
	dbPutSmearWgt2Last(destination,dbGetSmearWgt2Last(source));
	dbPutSmearStdDev(destination,dbGetSmearStdDev(source));
	return;
}

void dbCopyFeatureData(Feature *destination, Feature *source)
{
	char *loc_func_name = "dbCopyFeatureData";
	char *comment = NULL;
	char *prompt = NULL;
	char *expression = NULL;
	int i;
	int extended_data_type = 0;

	/* copy all the feature data from the source to the destination */
	dbOverrideFeatureNumber(destination,dbGetFeatureNumber(source));
	if (dbPutFeatureType(destination,dbGetFeatureType(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature type",dbGetFeatureType(source));
	}
	if (dbPutFeaturePlane(destination,dbGetFeaturePlane(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature plane",dbGetFeaturePlane(source));
	}
	if (dbPutFeatureLensCode(destination,dbGetFeatureLensCode(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature lens code",dbGetFeatureLensCode(source));
	}
	dbPutFeatureDockLocation(destination,dbGetFeatureDockLocation(source));
	if (dbPutLastDisplay(destination,dbGetLastDisplay(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature last display",dbGetLastDisplay(source));
	}
	if (dbPutLastDisplay2(destination,dbGetLastDisplay2(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature last display 2",dbGetLastDisplay2(source));
	}
	if (dbPutLastDisplay3(destination,dbGetLastDisplay3(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature last display 3",dbGetLastDisplay3(source));
	}
	if (dbPutBasicDimension(destination,dbGetBasicDimension(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"basic dimension",NO_VAL);
	}
	if (dbPutExtraDimension(destination,dbGetExtraDimension(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"extra dimension",NO_VAL);
	}
	if (dbPutInMmFlag(destination,dbGetInMmFlag(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"in/mm flag",dbGetInMmFlag(source));
	}
	if (dbPutCartPolarFlag(destination,dbGetCartPolarFlag(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"cart/polar flag",dbGetCartPolarFlag(source));
	}
	if (dbPutFeatureMode(destination,dbGetFeatureMode(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature mode",dbGetFeatureMode(source));
	}
	if (dbPutPrintFlags(destination,dbGetPrintFlags(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"print flags",dbGetPrintFlags(source));
	}
	if (dbPutStatisticsFlags(destination,dbGetStatisticsFlags(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"statistics flags",dbGetStatisticsFlags(source));
	}
	if (dbPutDecDmsFlag(destination,dbGetDecDmsFlag(source)) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"dec/dms flag",dbGetDecDmsFlag(source));
	}
	dbPutDataStreamFlag(destination,dbGetDataStreamFlag(source));
	dbPutMiscFlag(destination,dbGetMiscFlag(source));
	dbPutReportFlags(destination,dbGetReportFlags(source));
	dbPutExportFlags(destination,dbGetExportFlags(source));
	dbSetFeatureFailed(destination,dbGetFeatureFailed(source));
	dbPutFeatureDeployLaserFlag(destination,dbGetFeatureDeployLaserFlag(source));

	dbCopySmearData(destination,source);
	dbCopyPointData(destination,source);

	for(i = 0; i < MAX_FLAG_OPTIONS; i++)
	{
		if (dbGetReportLabel(source, i) != NULL)
		{
			if (dbPutReportLabel(destination,dbGetReportLabel(source,i),i) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"report label", i);
			}
		}
		if (dbGetExportFieldName(source, i) != NULL)
		{
			if (dbPutExportFieldName(destination,dbGetExportFieldName(source,i),i) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"export fieldname", i);
			}
		}
	}

	comment = dbGetFeatureComment(source);
	if (comment != NULL)
	{
		if (dbPutFeatureComment(destination, comment) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature comment",NO_VAL);
		}
	}
	prompt = dbGetFeaturePrompt(source);
	if (prompt != NULL)
	{
		if (dbPutFeaturePrompt(destination, prompt) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature prompt",NO_VAL);
		}
	}
	expression = dbGetFeatureExpression(source);
	if (expression != NULL)
	{
		if (dbPutFeatureExpression(destination, expression) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"expression",NO_VAL);
		}
	}

	extended_data_type = dbGetFeatureExtendedDataType(source);
	if (dbPutFeatureExtendedDataType(destination, extended_data_type) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"expression",NO_VAL);
	}
	if( extended_data_type == EXTENDED_DATA_LASER )
	{
		if( dbCopyLaserData(destination, source) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"laser data",NO_VAL);
		}
	}
	else if ( extended_data_type == EXTENDED_DATA_TOUCHPROBE )
	{
		if( dbCopyTouchProbeData(destination, source) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"touchprobe data",NO_VAL);
		} 
	}
	return;
}

void dbCopyActuals(Feature *destination, Feature *source)
{
	int i;

	/* copy all the actuals from the source to the destination */
	dbPutActualLocX(destination,dbGetActualLocX(source));
	dbPutActualLocY(destination,dbGetActualLocY(source));
	dbPutActualLocZ(destination,dbGetActualLocZ(source));
	if (dbGetNumberLocations(source) > 1)
	{
		dbPutSecondLocX(destination,dbGetSecondLocX(source));
		dbPutSecondLocY(destination,dbGetSecondLocY(source));
		dbPutSecondLocZ(destination,dbGetSecondLocZ(source));
	}
	for (i = 1; i <= dbGetNumberSizes(source); i++)
	{
		dbPutActualSize(destination,dbGetActualSize(source,i));
	}
	dbPutActualDeviation(destination,dbGetActualDeviation(source));
	dbPutDirectionX(destination,dbGetDirectionX(source));
	dbPutDirectionY(destination,dbGetDirectionY(source));
	dbPutDirectionZ(destination,dbGetDirectionZ(source));
	for (i = 1; i <= MAX_ANGLES; i++)
	{
		dbPutActualOrientation(destination,dbGetActualOrientation(source,i),i);
	}
	dbPutActualPosTol(destination,dbGetActualPosTol(source));
	dbPutModifiedPosTol(destination,dbGetModifiedPosTol(source));
	return;
}

void dbCopyNominals(Feature *destination, Feature *source)
{
	int flag1,flag2;									   /* tolerance flags */
	int i;

	/* copy all the nominals from the source to the destination */
	dbPutNominalsEntered(destination,dbGetNominalsEntered(source));
	dbPutToleranceType(destination,dbGetToleranceType(source));
	dbPutNominalLocX(destination,dbGetNominalLocX(source));
	dbPutNominalLocY(destination,dbGetNominalLocY(source));
	dbPutNominalLocZ(destination,dbGetNominalLocZ(source));
	dbPutUpperLocTolX(destination,dbGetUpperLocTolX(source));
	dbPutUpperLocTolY(destination,dbGetUpperLocTolY(source));
	dbPutUpperLocTolZ(destination,dbGetUpperLocTolZ(source));
	dbPutLowerLocTolX(destination,dbGetLowerLocTolX(source));
	dbPutLowerLocTolY(destination,dbGetLowerLocTolY(source));
	dbPutLowerLocTolZ(destination,dbGetLowerLocTolZ(source));
	dbPutNominalSize(destination,dbGetNominalSize(source));
	dbPutUpperSizeTol(destination,dbGetUpperSizeTol(source));
	dbPutLowerSizeTol(destination,dbGetLowerSizeTol(source));
	dbPutNominalSize2(destination,dbGetNominalSize2(source));
	dbPutUpperSize2Tol(destination,dbGetUpperSize2Tol(source));
	dbPutLowerSize2Tol(destination,dbGetLowerSize2Tol(source));
	dbPutNominalElevationAngle(destination,dbGetNominalElevationAngle(source));
	dbPutUpperElevationTol(destination,dbGetUpperElevationTol(source));
	dbPutLowerElevationTol(destination,dbGetLowerElevationTol(source));
	dbPutNominalSize3(destination,dbGetNominalSize3(source));
	dbPutUpperSize3Tol(destination,dbGetUpperSize3Tol(source));
	dbPutLowerSize3Tol(destination,dbGetLowerSize3Tol(source));
	dbPutNominalFormTol(destination,dbGetNominalFormTol(source));
	dbPutNominalPosTol(destination,dbGetNominalPosTol(source));
	dbGetNominalFlags(source,&flag1,&flag2);
	dbPutNominalFlags(destination,flag1,flag2);
	for (i = 1; i <= 3; i++)
	{
		(void) dbPutSearchOnStep(destination,i,dbGetSearchOnStep(source, i));
	}
	return;
}

void dbCopyFeatureTransform(Feature *destination, Feature *source)
{
	Database_Transform copy_of_transform;

	dbGetFeaturePartTransform(source, &copy_of_transform);
	dbPutFeaturePartTransform(destination, &copy_of_transform);
	return;
}

void dbCopyFeature(Feature *destination, Feature *source)
{
	dbCopyFeatureData(destination,source);
	dbCopyActuals(destination,source);
	dbCopyNominals(destination,source);
	dbCopyFeatureTransform(destination,source);
	return;
}
