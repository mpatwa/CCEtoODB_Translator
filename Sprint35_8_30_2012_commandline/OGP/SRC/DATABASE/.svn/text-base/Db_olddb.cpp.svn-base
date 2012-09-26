/*
$Workfile: Db_olddb.c $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_olddb.c 5     5/26/99 11:04a Ttc $

$Modtime: 5/26/99 8:55a $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_olddb.c $
 * 
 * 5     5/26/99 11:04a Ttc
 * Change Auxilary size and tolerance function names and internal variable
 * names to nominal size2, and upper and lower size2 tols in preparation
 * to add a nominal size 3 with tolerances for cone.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:22   softadmn
 * Initial revision.
 * 
 *    Rev 9.1   22 Jul 1996 13:53:00   ttc
 * Change NO_FILE to NO_FILE_FOUND.
 * 
 *    Rev 9.0   11 Mar 1996 11:13:00   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:26   softadmn
 * Initial revision.
 * 
 *    Rev 8.1   27 Feb 1995 15:37:02   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:00   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

/* OGP includes */
#include "gn_stdio.h"
#include "get_env.h"
#include "err_hand.h"

extern char gb_run_header_txt[][81];
extern char gb_run_setup_txt[][81];

int gb_crash_recover = FALSE;
int gb_db_flush_every = 0;

static FILE *gb_index_file = NULL;
static FILE *gb_features_file = NULL;
static FILE *gb_points_file = NULL;
static FILE *gb_actuals_file = NULL;
static FILE *gb_nominals_file = NULL;

short gb_old_db_version = 0;

int dbReadRepeatOffsets(FILE *in_file);

int dbPositionOldIndex(int feature_number)
{
	char *loc_func_name = "dbPositionOldIndex";
	long index_byte;

	index_byte = ((long) (feature_number-1) * ((long) sizeof(long) * 4)) +
				 ((long) sizeof(short) * 2 + 1);
	if (fseek(gb_index_file,index_byte,SEEK_SET) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,"index",(short) index_byte);
		return(SEEK_FAILED);
	}
	return(NO_ERROR);
}

int dbReadOldIndexEntry(short feature_number)
{
	char *loc_func_name = "dbReadOldIndexEntry";
	long file_bytes[4];

	if (dbPositionOldIndex(feature_number) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,"index",NO_VAL);
		return(SEEK_FAILED);
	}

	if (fread(file_bytes,sizeof(long),4,gb_index_file) != 4)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"index",NO_VAL);
		return(NO_READ_FILE);
	}
	if (fseek(gb_features_file,file_bytes[0],SEEK_SET) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,"features",NO_VAL);
		return(SEEK_FAILED);
	}
	if (fseek(gb_points_file,file_bytes[1],SEEK_SET) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,"points",NO_VAL);
		return(SEEK_FAILED);
	}
	if (fseek(gb_actuals_file,file_bytes[2],SEEK_SET) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,"actuals",NO_VAL);
		return(SEEK_FAILED);
	}
	if (fseek(gb_nominals_file,file_bytes[3],SEEK_SET) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,"nominals",NO_VAL);
		return(SEEK_FAILED);
	}

	return(NO_ERROR);
}

int dbReadOldFeatureEntry(Feature *this_feature, short feature_number)
{
	char *loc_func_name = "dbReadOldFeatureEntry";
	short temp_feat_number;
	short flags[8];
	double basic_dim;
	unsigned short comment_length = 0;
	char *comment = NULL;
	unsigned short prompt_length = 0;
	char *prompt = NULL;

	if (fread(&temp_feat_number,sizeof(short),1,gb_features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
		return(NO_READ_FILE);
	}
	if (temp_feat_number != feature_number)
	{
		errPost(loc_func_name,SYSTEM_ERROR,INVALID_ROUTINE,"features",NO_VAL);
		return(INVALID_ROUTINE);
	}
	if (fread(flags,sizeof(short),8,gb_features_file) != 8)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
		return(NO_READ_FILE);
	}
	if (dbPutFeatureType(this_feature,(int) flags[0]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature type",flags[0]);
		return(PUT_FAILED);
	}
	if (dbPutFeaturePlane(this_feature,(int) flags[1]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature plane",flags[1]);
		return(PUT_FAILED);
	}
	if (dbPutInMmFlag(this_feature,(int) flags[2]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"in / mm flag",flags[2]);
		return(PUT_FAILED);
	}
	if (dbPutCartPolarFlag(this_feature,(int) flags[3]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"cart / polar flag",flags[3]);
		return(PUT_FAILED);
	}
	if (dbPutFeatureMode(this_feature,(int) flags[4]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature mode",flags[4]);
		return(PUT_FAILED);
	}
	if (dbPutStatisticsFlags(this_feature,(int) flags[5]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"statistics flags",flags[5]);
		return(PUT_FAILED);
	}
	if (dbPutPrintFlags(this_feature,(int) flags[6]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"print flags",flags[6]);
		return(PUT_FAILED);
	}
	if (dbPutLastDisplay(this_feature,(int) flags[7]) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"last display",flags[7]);
		return(PUT_FAILED);
	}

	if (fread(&basic_dim,sizeof(double),1,gb_features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
		return(NO_READ_FILE);
	}
	if (dbPutBasicDimension(this_feature,basic_dim) != NO_ERROR)
	{
		errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"basic dimension",NO_VAL);
		return(PUT_FAILED);
	}

	if (fread(&comment_length,sizeof(short),1,gb_features_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
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
		if (fread(comment,sizeof(char),comment_length,gb_features_file) != comment_length)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
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

	if (gb_old_db_version >= 2)
	{
		if (fread(&prompt_length,sizeof(short),1,gb_features_file) != 1)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
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
			if (fread(prompt,sizeof(char),prompt_length,gb_features_file) != prompt_length)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
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
	}
	return(NO_ERROR);
}

int dbReadOldPointsEntry(Feature *this_feature, short feature_number)
{
	char *loc_func_name = "dbReadOldPointsEntry";
	int i;
	short temp_feat_number;
	short number_of_points;
	double locz[4];
	short lights[3];
	short target[4];
	short reference_feature;
	unsigned short target_filename_length;
	char target_filename[12];

	if (fread(&temp_feat_number,sizeof(short),1,gb_points_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
		return(NO_READ_FILE);
	}
	if (temp_feat_number != feature_number)
	{
		errPost(loc_func_name,SYSTEM_ERROR,INVALID_ROUTINE,"points",NO_VAL);
		return(INVALID_ROUTINE);
	}
	if (fread(&number_of_points,sizeof(short),1,gb_points_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
		return(NO_READ_FILE);
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
			if (fread(locz,sizeof(double),4,gb_points_file) != 4)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
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
			if (dbPutPointZoom(this_feature,i,locz[3]) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point zoom location",i);
				return(PUT_FAILED);
			}

			if (fread(lights,sizeof(short),3,gb_points_file) != 3)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
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

			if (fread(target,sizeof(short),4,gb_points_file) != 4)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
				return(NO_READ_FILE);
			}
			if (dbPutPointTargetType(this_feature,i,(int) target[0]) != NO_ERROR)
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

			if (fread(&target_filename_length,sizeof(short),1,gb_points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
				return(NO_READ_FILE);
			}
			if (target_filename_length > 0)
			{
				if (fread(target_filename,sizeof(char),target_filename_length,gb_points_file) != target_filename_length)
				{
					errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
					return(NO_READ_FILE);
				}
				if (dbPutPointTargetFilename(this_feature,i,target_filename) != NO_ERROR)
				{
					errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"point target filename",i);
					return(PUT_FAILED);
				}
			}
		}
		else
		{
			if (fread(&reference_feature,sizeof(short),1,gb_points_file) != 1)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
				return(NO_READ_FILE);
			}
			if (dbPutRefFeatureNumber(this_feature,i,(int) reference_feature) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"reference feature number",i);
				return(PUT_FAILED);
			}
		}
	}
	return(NO_ERROR);
}

int dbReadOldActualsEntry(Feature *this_feature, short feature_number)
{
	char *loc_func_name = "dbReadOldActualsEntry";
	short temp_feat_number;
	short number_of[2];
	double actual_loc_size[11];
	int i;

	if (fread(&temp_feat_number,sizeof(short),1,gb_actuals_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"actuals",NO_VAL);
		return(NO_READ_FILE);
	}
	if (temp_feat_number != feature_number)
	{
		errPost(loc_func_name,SYSTEM_ERROR,INVALID_ROUTINE,"features",NO_VAL);
		return(INVALID_ROUTINE);
	}
	if (fread(number_of,sizeof(short),2,gb_actuals_file) != 2)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"actuals",NO_VAL);
		return(NO_READ_FILE);
	}
	if (fread(actual_loc_size,sizeof(double),11,gb_actuals_file) != 11)
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
	return(NO_ERROR);
}

int dbReadOldNominalsEntry(Feature *this_feature, short feature_number)
{
	char *loc_func_name = "dbReadOldNominalsEntry";
	short temp_feat_number;
	double nom_tols[17];
	short flags[2];
	int tol_type = STANDARD_TOL;

	if (fread(&temp_feat_number,sizeof(short),1,gb_nominals_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominals",NO_VAL);
		return(NO_READ_FILE);
	}
	if (temp_feat_number != feature_number)
	{
		errPost(loc_func_name,SYSTEM_ERROR,INVALID_ROUTINE,"nominals",NO_VAL);
		return(INVALID_ROUTINE);
	}
	if (fread(nom_tols,sizeof(double),17,gb_nominals_file) != 17)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominals",NO_VAL);
		return(NO_READ_FILE);
	}
	if (fread(flags,sizeof(short),2,gb_nominals_file) != 2)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominals",NO_VAL);
		return(NO_READ_FILE);
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

	if (gb_old_db_version >= 3)
	{
		if ( (nom_tols[10] >= 0) && (nom_tols[11] <= 0) )
		{
			tol_type = STANDARD_TOL;
		}
		else
		{
			tol_type = ISO_TOL;
		}
	}
	dbPutNominalSize(this_feature,nom_tols[9]);
	dbPutUpperSizeTol(this_feature,nom_tols[10]);
	dbPutLowerSizeTol(this_feature,nom_tols[11]);
	dbPutToleranceType(this_feature,tol_type);

	dbPutNominalSize2(this_feature,nom_tols[12]);
	dbPutUpperSize2Tol(this_feature,nom_tols[13]);
	dbPutLowerSize2Tol(this_feature,nom_tols[14]);

	dbPutNominalFormTol(this_feature,nom_tols[15]);
	dbPutNominalPosTol(this_feature,nom_tols[16]);
	dbPutNominalFlags(this_feature,(int) flags[0],(int) flags[1]);

	return(NO_ERROR);
}

int dbGetFeatureFromOldDB(Feature *this_feature, short feature_number)
{
	char *loc_func_name = "dbGetFeatureFromOldDB";

	if (dbGetFeatureNumber(this_feature) != 0)
	{
		dbInitFeature(this_feature);
	}

	if (dbReadOldIndexEntry(feature_number) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"index",NO_VAL);
		return(NO_READ_FILE);
	}

	if (dbReadOldFeatureEntry(this_feature,feature_number) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"features",NO_VAL);
		return(NO_READ_FILE);
	}

	if (dbReadOldPointsEntry(this_feature,feature_number) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"points",NO_VAL);
		return(NO_READ_FILE);
	}

	if (dbReadOldActualsEntry(this_feature,feature_number) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"actuals",NO_VAL);
		return(NO_READ_FILE);
	}

	if (dbReadOldNominalsEntry(this_feature,feature_number) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,"nominals",NO_VAL);
		return(NO_READ_FILE);
	}

	this_feature->data.number = feature_number;

	return(NO_ERROR);
}

int dbCopyBytes(FILE *in_file, FILE *out_file, long file_bytes)
{
	char *loc_func_name = "dbCopyBytes";
	/* char line[COPY_BUFFER_SIZE]; */
	char *line = NULL;
	long bytes_left = file_bytes;
	long num_bytes;
	long buffer_size = 512;
	unsigned int temp = 0;

	temp = (unsigned int) buffer_size;
	line = (char *)malloc(temp);
	if (line == NULL)
	{

		errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
		return(NO_MEMORY);
	}

	do
	{
		num_bytes = buffer_size;				 /* set initial bytes to read */
		if (num_bytes > bytes_left)				/* not that many left to read */
			num_bytes = bytes_left;					   /* reset bytes to read */
		bytes_left -= num_bytes;		 /* subtract bytes to read from total */
		temp = (unsigned int)(num_bytes);	/* need unsigned int for file i/o */
		if (fread(line,sizeof(char),temp,in_file) != temp)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,NULL,NO_VAL);
			return(NO_READ_FILE);/* didn't get number of bytes like we wanted */
		}
		if (fwrite(line,sizeof(char),temp,out_file) != temp)  /* write it out */
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_WRITE_FILE,NULL,NO_VAL);
			return(NO_WRITE_FILE);
		}
	}
	while (bytes_left != 0);				   /* until no more bytes to read */
	free(line);
	return(NO_ERROR);
}

int dbGetOldDataBase(char *routine_file)
{
	char *loc_func_name = "dbGetOldDataBase";
	char ramdrive[_MAX_PATH];
	char filename[_MAX_PATH];
	FILE *in_file = NULL;
	long sizes[5] = { 0, 0, 0, 0, 0 };
	long current_pos;
	char junk;
	long number_features = 0;
	long min_actuals = 0;
	long min_nominals = 0;
	short i;
	Feature *old_feature = NULL;
	short last_db_feat_num = 0;

	in_file = fopen(routine_file,"rb");				   /* open the input file */
	if (in_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,routine_file,NO_VAL);
		return(OPEN_FAILED);
	}

	if (fread(sizes,sizeof(long),5,in_file) != 5)	   /* read the file sizes */
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,routine_file,NO_VAL);
		return(NO_READ_FILE);
	}

	if ( (current_pos = ftell(in_file)) == -1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,routine_file,NO_VAL);
		return(NO_READ_FILE);
	}
	if (fread(&gb_old_db_version,sizeof(short),1,in_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,routine_file,NO_VAL);
		return(NO_READ_FILE);
	}
	if (fread(&junk,sizeof(char),1,in_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,routine_file,NO_VAL);
		return(NO_READ_FILE);
	}
	if (fread(&last_db_feat_num,sizeof(short),1,in_file) != 1)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,routine_file,NO_VAL);
		return(NO_READ_FILE);
	}
	if (fseek(in_file,current_pos,SEEK_SET) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,SEEK_FAILED,routine_file,NO_VAL);
		return(SEEK_FAILED);
	}

	/* calculate minimum valid index.dat file size */
	/* (could be longer due to delete last feature not truncating the files) */
	number_features = sizes[0] - (2 * sizeof(short)) + 1;
	number_features = (number_features / sizeof(long)) / 4;
	min_actuals = (long) (((3 * sizeof(short)) + (11 * sizeof(double))) * number_features);
	min_nominals = (long) (((3 * sizeof(short)) + (17 * sizeof(double))) * number_features);

	/* now lets do some sanity checks */
	if ( (gb_old_db_version < 0) || (gb_old_db_version > 3) ||
		 (sizes[0] <= 0) || (sizes[1] <= 0) || (sizes[2] <= 0) ||
		 (sizes[3] <= 0) || (sizes[4] <= 0) ||
		 (sizes[3] < min_actuals) || (sizes[4] < min_nominals) ||
		 (number_features < (long)(last_db_feat_num)) )
	{
		gb_old_db_version = 0;
		return(NO_FILE_FOUND);
	}

	/* close the files if they are open from a previous get of an old routine */
	if (gb_index_file != NULL)
	{
		(void) fclose(gb_index_file);
		(void) fclose(gb_features_file);
		(void) fclose(gb_points_file);
		(void) fclose(gb_actuals_file);
		(void) fclose(gb_nominals_file);
	}

	if (getEnvVar(ramdrive,"TEMP",TEMP_DEFAULT) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,BAD_ENV_VAR,"TEMP",NO_VAL);
		return(BAD_ENV_VAR);
	}

	strcpy(filename,ramdrive);					  /* set the output file path */
	strcat(filename,"index.dat");				/* append the index file name */
	gb_index_file = fopen(filename,"w+b");			   /* open it for writing */
	if (gb_index_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,filename,NO_VAL);
		return(OPEN_FAILED);
	}
	/* copy contents of index.dat file */
	if (dbCopyBytes(in_file,gb_index_file,sizes[0]) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_COPY_FILE,filename,NO_VAL);
		return(NO_COPY_FILE);
	}
	
	strcpy(filename,ramdrive);					  /* set the output file path */
	strcat(filename,"features.dat");	 /* append the feature data file name */
	gb_features_file = fopen(filename,"w+b");
	if (gb_features_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,filename,NO_VAL);
		return(OPEN_FAILED);
	}
	if (dbCopyBytes(in_file,gb_features_file,sizes[1]) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_COPY_FILE,filename,NO_VAL);
		return(NO_COPY_FILE);
	}

	strcpy(filename,ramdrive);					  /* set the output file path */
	strcat(filename,"points.dat");		   /* append the point data file name */
	gb_points_file = fopen(filename,"w+b");
	if (gb_points_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,filename,NO_VAL);
		return(OPEN_FAILED);
	}
	if (dbCopyBytes(in_file,gb_points_file,sizes[2]) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_COPY_FILE,filename,NO_VAL);
		return(NO_COPY_FILE);
	}
	
	strcpy(filename,ramdrive);					  /* set the output file path */
	strcat(filename,"actuals.dat");		  /* append the actual data file name */
	gb_actuals_file = fopen(filename,"w+b");
	if (gb_actuals_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,filename,NO_VAL);
		return(OPEN_FAILED);
	}
	if (dbCopyBytes(in_file,gb_actuals_file,sizes[3]) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_COPY_FILE,filename,NO_VAL);
		return(NO_COPY_FILE);
	}
	
	strcpy(filename,ramdrive);					  /* set the output file path */
	strcat(filename,"nominals.dat");	/* append the nominals data file name */
	gb_nominals_file = fopen(filename,"w+b");
	if (gb_nominals_file == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,OPEN_FAILED,filename,NO_VAL);
		return(OPEN_FAILED);
	}
	if (dbCopyBytes(in_file,gb_nominals_file,sizes[4]) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_COPY_FILE,filename,NO_VAL);
		return(NO_COPY_FILE);
	}

	for (i = 0; i < NUM_HEADER_LINES; i++)
	{
		if (fread(gb_run_header_txt[i],sizeof(char),81,in_file) != 81)
		{
			(void) memset(gb_run_header_txt[i],0,81);
			i = NUM_HEADER_LINES;
		}
	}
	for (i = 0; i < NUM_SETUP_LINES; i++)
	{
		if (fread(gb_run_setup_txt[i],sizeof(char),81,in_file) != 81)
		{
			(void) memset(gb_run_setup_txt[i],0,81);
			i = NUM_SETUP_LINES;
		}
	}

	if (dbReadRepeatOffsets(in_file) != NO_ERROR)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_READ_FILE,routine_file,NO_VAL);
		return(NO_READ_FILE);
	}

	if (fclose(in_file) != 0)
	{
		errPost(loc_func_name,SYSTEM_ERROR,CLOSE_FAILED,routine_file,NO_VAL);
		return(CLOSE_FAILED);
	}
	
	old_feature = dbNewFeature();
	if (old_feature == NULL)
	{
		errPost(loc_func_name,DATABASE_ERROR,CREATE_FEAT_FAILED,NULL,NO_VAL);
		return(CREATE_FEAT_FAILED);
	}

	for (i = 1; i <= last_db_feat_num; i++)
	{
		if (i > 1)
		{
			dbInitFeature(old_feature);
		}
		if (dbGetFeatureFromOldDB(old_feature, i) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"feature",i);
			return(GET_FROM_DB_FAILED);
		}
		if (dbPutFeatureInDB(old_feature) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"feature",i);
			return(DATABASE_ERROR);
		}
	}
	dbDisposeFeature(old_feature);

	(void) fclose(gb_index_file);
	gb_index_file = NULL;
	(void) fclose(gb_features_file);
	gb_features_file = NULL;
	(void) fclose(gb_points_file);
	gb_points_file = NULL;
	(void) fclose(gb_actuals_file);
	gb_actuals_file = NULL;
	(void) fclose(gb_nominals_file);
	gb_nominals_file = NULL;
	return(NO_ERROR);
}
