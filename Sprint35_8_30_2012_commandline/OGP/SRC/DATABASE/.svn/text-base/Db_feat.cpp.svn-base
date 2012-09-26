/*
$Workfile: Db_feat.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_feat.cpp 8     7/13/01 3:33p Jnc $

$Modtime: 7/13/01 2:35p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_feat.cpp $
 * 
 * 8     7/13/01 3:33p Jnc
 * removed close database function
 * 
 * 7     1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 6     4/30/99 2:11p Ttc
 * Change COPY_FEATURE to HOLD_FEATURE.  This type holds the feature open
 * while the insert is being performed.
 * 
 *    Rev 10.3   02 Sep 1998 13:59:10   ttc
 * Add new function to return a pointer to a feature directly in the db memory.
 * Use of this function should be tempered as disposing of or changing the 
 * feature, changes it in the db memory directly.
 * 
 *    Rev 10.2   27 Jul 1998 10:39:04   ttc
 * Add some safety checks for the new indexed ptr array when getting features.
 * 
 *    Rev 10.1   24 Jul 1998 08:23:40   ttc
 * Add array of feature points for direct access to the feature.  The next and
 * previous are not really needed anymore.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:18   softadmn
 * Initial revision.
 * 
 *    Rev 9.1   26 Sep 1996 13:30:52   dmf
 * Added new functionality to rotary
 * 
 *    Rev 9.0   11 Mar 1996 11:12:48   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:14   softadmn
 * Initial revision.
 * 
 *    Rev 8.5   06 Oct 1995 16:54:54   ttc
 * Forgot to free temporary storage when editing math references.
 * 
 *    Rev 8.4   20 Sep 1995 15:45:40   ttc
 * When inserting a feature, set the feature type of the new inserted blank
 * feature to COPY_FEATURES so that turning on the model doesn't give an error
 * when it rescans the database to draw the model.
 * 
 *    Rev 8.3   20 Sep 1995 12:00:52   ttc
 * Add check before allocating 0 number of reference features when decrementing
 * reference features.  Construct point and math can have 0 reference feature
 * s.
 * 
 *    Rev 8.2   14 Apr 1995 10:49:08   ttc
 * Add check to see if expression has already been deleted when decrementing
 * reference features.
 * 
 *    Rev 8.1   27 Feb 1995 15:36:52   ttc
 * Add math/branch.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:36   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/* OGP includes */
#include "gn_stdio.h"
#include "get_env.h"
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"

static Feature *gb_first_feature = NULL;
static Feature *gb_last_feature = NULL;
static int gb_last_feature_number = 0;
static Feature *gb_feature_ptrs[SHRT_MAX] = { NULL };

int dbGetLastFeatureNumber(void)
{
	return(gb_last_feature_number);
}

int dbVeryLastFeatureNumber(void)
{
	if (gb_last_feature != NULL)
	{
		return(dbGetFeatureNumber(gb_last_feature));
	}
	else
	{
		return(0);
	}
}

int dbResetLastFeatureNumber(void)
{
	gb_last_feature_number = dbVeryLastFeatureNumber();
	return(NO_ERROR);
}

int dbSetLastFeatureNumber(int last_feature_number)
{
	gb_last_feature_number = last_feature_number;
	return(NO_ERROR);
}

int dbInitDataBase(void)
{
	Feature *save_feature;
	int i;

	/* next feature is 1 */
	gb_last_feature_number = 0;

	/* while the last feature is pointing to a feature */
	while (gb_last_feature != NULL)
	{
		/* the the previous feature's address */
		save_feature = gb_last_feature->prev_feature;
		/* dispose of this feature */
		dbDisposeFeature(gb_last_feature);
		/* point to the previous feature */
		gb_last_feature = save_feature;
	}
	/* initialize the first feature */
	gb_first_feature = NULL;
	for (i = 0; i < SHRT_MAX; i++)
		gb_feature_ptrs[i] = NULL;

	return(NO_ERROR);
}

#ifdef NEVER
int dbCloseDataBase(void)
{
	return(dbInitDataBase());
}
#endif

int dbPutFeatureInDB(Feature *save_feature)
{
	char *loc_func_name = "dbPutFeatureInDB";
	Feature *new_feature = NULL;		/* pointer to a new feature structure */
	Feature *this_feature = gb_first_feature;	  /* start at top of the list */

	/* does the feature have the minimum number of points */
	if (dbMinimumFeaturePoints(save_feature))
	{
		/* get a new feature structure */
		new_feature = dbNewFeature();
		if (new_feature == NULL)
		{
			errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
			return(GOT_ERROR);
		}

		/* copy the feature to save into the new feature structure */
		dbCopyFeature(new_feature,save_feature);
		new_feature->next_feature = NULL;
		new_feature->prev_feature = NULL;
		gb_feature_ptrs[dbGetFeatureNumber(new_feature)] = new_feature;

		/* is there anything in the feature list yet? */
		if ( (gb_first_feature == NULL)	&& (gb_last_feature == NULL) )
		{	/* no features in database yet */
			gb_first_feature = new_feature;
			gb_last_feature = new_feature;
			gb_last_feature_number = 1;

			return(NO_ERROR);
		}
		/* are we adding this feature to the end of the list? */
		else if (dbGetFeatureNumber(save_feature) ==
				 (dbGetFeatureNumber(gb_last_feature) + 1))
		{	/* features in list, add this one to the end */
			new_feature->prev_feature = gb_last_feature;
			gb_last_feature->next_feature = new_feature;
			gb_last_feature = new_feature;

			/* increment the next feature number */
			gb_last_feature_number++;

			return(NO_ERROR);
		}
		else
		{	/* copy this feature over one already in the list (edit) */
			while (this_feature != NULL)
			{
				/* look for feature to overwrite this one with */
				if (dbGetFeatureNumber(this_feature) == dbGetFeatureNumber(save_feature))
				{
					/* save the next and previous feature pointers */
					new_feature->next_feature = this_feature->next_feature;
					new_feature->prev_feature = this_feature->prev_feature;

					/* if this is the first feature in the list */
					if (gb_first_feature == this_feature)
					{
						/* overwrite the first feature pointer */
						gb_first_feature = new_feature;
					}

					/* if this is the last feature in the list */
					if (gb_last_feature == this_feature)
					{
						/* overwrite the last feature pointer */
						gb_last_feature = new_feature;
					}

					/* dispose of the old copy of the feature */
					dbDisposeFeature(this_feature);

					/* if the new feature has a previous feature */
					if (new_feature->prev_feature != NULL)
					{
						/* set to it and change that feature's next pointer */
						this_feature = new_feature->prev_feature;
						this_feature->next_feature = new_feature;
					}

					/* if the new feature has a next feature */
					if (new_feature->next_feature != NULL)
					{
						/* set to it and change that feature's previous ptr */
						this_feature = new_feature->next_feature;
						this_feature->prev_feature = new_feature;
					}

					if (dbGetFeatureNumber(new_feature) ==
						(gb_last_feature_number + 1))
					{
						gb_last_feature_number++;
					}

					/* found the feature we were looking for, get out of here */
					return(NO_ERROR);
				}

				/* set to the next feature in the list */
				this_feature = this_feature->next_feature;
			}
		}
	}
	return(GOT_ERROR);		   /* if we got here then there's something wrong */
}

int dbGetFeatureFromDB(Feature *get_feature, int feature_number)
{
	Feature *this_feature = gb_first_feature;		/* first feature in list */

	/* is the feature structure uninitialized? */
	if (dbGetFeatureNumber(get_feature) != 0)
	{
		dbInitFeature(get_feature);
	}
	
	this_feature = gb_feature_ptrs[feature_number];
	/* just in case the array of pointers isn't right */
	if (this_feature != NULL)
	{
		if (dbGetFeatureNumber(this_feature) != feature_number)
		{
			this_feature = gb_first_feature;
		}
	}
	else
	{
		this_feature = gb_first_feature;
	}

	/* while we haven't reached the end of the list */
	while (this_feature != NULL)
	{
		/* is this the feature we want to get? */
		if (dbGetFeatureNumber(this_feature) == feature_number)
		{
			/* copy the feature into the return feature structure */
			/* never give them access into our linked list */
			dbCopyFeature(get_feature,this_feature);
			get_feature->next_feature = NULL;
			get_feature->prev_feature = NULL;

			/* found the feature we were looking for, get out of here */
			return(NO_ERROR);
		}

		/* go to the next feature in the list */
		this_feature = this_feature->next_feature;
	}

	/* if we got here we reached the end and didn't find it. return GOT_ERROR */
	return(GOT_ERROR);
}

Feature *dbTrustedGetFeatureFromDB(int feature_number)
{
	Feature *this_feature = gb_first_feature;		/* first feature in list */

	this_feature = gb_feature_ptrs[feature_number];
	/* just in case the array of pointers isn't right */
	if (this_feature != NULL)
	{
		if (dbGetFeatureNumber(this_feature) != feature_number)
		{
			this_feature = gb_first_feature;
		}
	}
	else
	{
		this_feature = gb_first_feature;
	}

	/* while we haven't reached the end of the list */
	while (this_feature != NULL)
	{
		/* is this the feature we want to get? */
		if (dbGetFeatureNumber(this_feature) == feature_number)
		{
			/* found the feature we were looking for, get out of here */
			/* give the trusted caller a direct pointer into the db memory */
			return(this_feature);
		}

		/* go to the next feature in the list */
		this_feature = this_feature->next_feature;
	}

	/* if we got here we reached the end and didn't find it. return NULL */
	return(NULL);
}

int dbRunLastFeatureNumber(void)
{
	return(dbVeryLastFeatureNumber());
}

int dbSetupRunDataBase(void)
{
	gb_last_feature_number = 0;
	return(NO_ERROR);
}

int dbGetFeatureForRun(Feature *get_feature, int feature_number)
{
	return(dbGetFeatureFromDB(get_feature,feature_number));
}

int dbCloseRunDataBase(void)
{
	return(dbResetLastFeatureNumber());
}

void dbDecMathFeatureNumbers(Feature *edit_feature, int delete_feat_num)
{
	char *loc_func_name = "dbDecMathFeatureNumbers";
	char *expression = NULL;
	char *new_expression = NULL;
	int len = 0;
	int i = 0;
	int j = 0;
	int this_char = 0;
	int next_char = 0;
	int ref_feat_num = 0;
	char ref_feat[MINI_STRING];

	expression = dbGetFeatureExpression(edit_feature);
	if (expression == NULL)
	{	/* expression already deleted, get out */
		return;
	}

	len = strlen(expression);
	/* just give us a few extra characters */
	new_expression = (char *)calloc((unsigned)(len + 2),sizeof(char));
	if (new_expression == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"new expression",len);
		return;
	}
	(void) memset(new_expression,0,(len + 2));
	for (i = 0; i <= len; i++)
	{
		this_char = (int) expression[i];
		next_char = (int) expression[i + 1];
		if ( (this_char == 27) && (next_char > 27) )	/* math tag */
		{
			ref_feat_num = atoi(&expression[i + 2]);
			if (ref_feat_num == delete_feat_num)
			{	/* deleted feature in expression - delete expression */
				dbPutFeatureExpression(edit_feature,"");
				return;
			}
			else if ( (ref_feat_num > delete_feat_num) &&
					  (ref_feat_num <= dbVeryLastFeatureNumber()) )
			{	/* decrement the reference feature number */
				/* transfer the escape and tag code */
				new_expression[j] = expression[i];
				new_expression[j + 1] = expression[i + 1];
				i += 2;
				j += 2;
				/* get past the feature number in the old string */
				while (isdigit(expression[i + 1]) != 0)
				{
					i++;
				}
				ref_feat_num--;
				sprintf(ref_feat,"%-d",ref_feat_num);
				strcat(new_expression,ref_feat);
				j += strlen(ref_feat);
			}
			else	/* leave the reference feature number alone */
			{
				new_expression[j] = expression[i];
				j++;
			}
		}
		else
		{
			new_expression[j] = expression[i];
			j++;
		}
	}
	dbPutFeatureExpression(edit_feature,new_expression);
	free(new_expression);
	return;
}

void dbDecRefFeatureNumbers(Feature *edit_feature, int delete_feat_num)
{
	char *loc_func_name = "dbDecRefFeatureNumbers";
	int number_of_points;
	int *feature_numbers = NULL;
	int new_number_points;
	int i;
	int ref_feat_num;

	number_of_points = dbGetNumberPoints(edit_feature);
	if (number_of_points == 0)	/* construct point, math */
	{
		return;
	}
	feature_numbers = (int *)calloc((unsigned)number_of_points,sizeof(int));
	if (feature_numbers == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"feature numbers",number_of_points);
		return;
	}
	new_number_points = 0;
	for (i = number_of_points; i >= 1; i--)
	{
		if ((ref_feat_num = dbGetRefFeatureNumber(edit_feature,i)) < 0)
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"reference feature number",i);
		}
		if (dbDecNumberPoints(edit_feature) < 0)
		{
			errPost(loc_func_name,DATABASE_ERROR,DELETE_FAILED,"feature",i);
		}
		if (ref_feat_num != delete_feat_num)
		{
			if ( (ref_feat_num > delete_feat_num) &&
				 (ref_feat_num <= dbVeryLastFeatureNumber()) )
			{
				ref_feat_num--;
			}
			feature_numbers[new_number_points] = ref_feat_num;
			new_number_points++;
		}
	}
		
	for (i = new_number_points; i >= 1; i--)
	{
		if ((number_of_points = dbIncNumberPoints(edit_feature)) <= 0)
		{
			errPost(loc_func_name,DATABASE_ERROR,INC_FEAT_FAILED,NULL,i);
		}

		if (dbPutRefFeatureNumber(edit_feature,number_of_points,feature_numbers[i-1]) != NO_ERROR)
		{
			errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"reference feature number",number_of_points);
		}
	}

	free(feature_numbers);
	return;
}

int dbDeleteFeature(int feature_number)
{
	Feature *this_feature = gb_last_feature;  /* start at end of feature list */
	Feature *feat_ptr = NULL;		   /* pointer to next or previous feature */
	int this_number;								/* current feature number */
	int feature_type;
	int ref_number;
	int last_feature_number = 0;
	int i;

	/* get and save the last feature number before touching anything */
	last_feature_number = dbVeryLastFeatureNumber();

	/* as long as we haven't reached the beginning of the list */
	while (this_feature != NULL)
	{
		/* get the feature number from this feature */
		this_number = dbGetFeatureNumber(this_feature);

		/* this feature number bigger than feature number to delete ? */
		if (this_number > feature_number)
		{
			/* decrement feature number and override this feature number */
			this_number--;
			dbOverrideFeatureNumber(this_feature,this_number);
			if (dbIsFeatureConstructed(this_feature))
			{
				feature_type = dbGetFeatureType(this_feature);
				if ( (feature_type == MATH) || (feature_type == BRANCH) )
				{
					dbDecMathFeatureNumbers(this_feature,feature_number);
				}
				dbDecRefFeatureNumbers(this_feature,feature_number);
			}
			else
			{
				feature_type = dbGetFeatureType(this_feature);
				ref_number = dbGetMiscFlag(this_feature);
				if ((feature_type == ROTARY_FT) && (dbGetLastDisplay(this_feature) == ROTARY_RESULT))
				{
					/* Check to make sure that this feature will be pointing to a math block */
					if (ref_number == feature_number)
					{
						dbPutMiscFlag(this_feature, 0);
					}
					else if (ref_number > feature_number)
					{
						dbPutMiscFlag(this_feature, ref_number - 1);
					}
				}
				for (i = 1; i <= 3; i++)
				{
					ref_number = dbGetSearchOnStep(this_feature, i);
					if (ref_number == feature_number)
					{
						dbPutSearchOnStep(this_feature, i, 0);
					}
					else if (ref_number > feature_number)
					{
						dbPutSearchOnStep(this_feature, i, ref_number - 1);
					}
				}
			}
		}
		else if (this_number == feature_number)
		{
			/* we've found the feature to delete */
			/* if there is a previous feature */
			if (this_feature->prev_feature != NULL)
			{
				/* set to the previous feature in the list */
				feat_ptr = this_feature->prev_feature;

				/* and change the previous feature's next feature pointer */
				/* to the deleted feature's next pointer */
				feat_ptr->next_feature = this_feature->next_feature;
			}
			else	/* this is the first feature in the list */
			{
				/* overwrite the first feature pointer */
				gb_first_feature = this_feature->next_feature;
			}

			/* if there is a next feature */
			if (this_feature->next_feature != NULL)
			{
				/* set to the next feature in the list */
				feat_ptr = this_feature->next_feature;

				/* and change the next feature's previous feature pointer */
				/* to the deleted feature's previous pointer */
				feat_ptr->prev_feature = this_feature->prev_feature;
			}
			else	/* this is the last feature in the list */
			{
				/* overwrite the last feature pointer */
				gb_last_feature = this_feature->prev_feature;
			}

			/* dispose of the deleted feature */
			dbDisposeFeature(this_feature);

			if (feature_number == last_feature_number)
			{
				gb_feature_ptrs[feature_number] = NULL;
			}
			else
			{
				memmove(&gb_feature_ptrs[feature_number],&gb_feature_ptrs[feature_number + 1],
						sizeof(Feature *) * (last_feature_number - feature_number));
			}

			/* found the feature we were looking for, get out of here */
			return(NO_ERROR);
		}
		else
		{	/* we've gone too far forward in the list, just break out */
			break;
		}
		/* set the current feature pointer to the previous feature */
		this_feature = this_feature->prev_feature;
	}

	/* if we get here, we didn't find the feature to delete, return GOT_ERROR */
	return(GOT_ERROR);
}

void dbIncMathFeatureNumbers(Feature *edit_feature, int insert_feat_num)
{
	char *loc_func_name = "dbIncMathFeatureNumbers";
	char *expression = NULL;
	char *new_expression = NULL;
	int len = 0;
	int i = 0;
	int j = 0;
	int this_char = 0;
	int next_char = 0;
	int ref_feat_num = 0;
	char ref_feat[MINI_STRING];

	expression = dbGetFeatureExpression(edit_feature);
	len = strlen(expression);
	new_expression = (char *)calloc((unsigned)(len * 2),sizeof(char));
	if (new_expression == NULL)
	{
		errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,"new expression",len);
		return;
	}
	(void) memset(new_expression,0,(len * 2));
	for (i = 0; i <= len; i++)
	{
		this_char = (int) expression[i];
		next_char = (int) expression[i + 1];
		if ( (this_char == 27) && (next_char > 27) )	/* math tag */
		{
			ref_feat_num = atoi(&expression[i + 2]);
			if ( (ref_feat_num >= insert_feat_num) &&
				 (ref_feat_num <= dbVeryLastFeatureNumber()) )
			{	/* increment the reference feature number */
				/* transfer the escape and tag code */
				new_expression[j] = expression[i];
				new_expression[j + 1] = expression[i + 1];
				i += 2;
				j += 2;
				/* get past the feature number in the old string */
				while (isdigit(expression[i + 1]) != 0)
				{
					i++;
				}
				ref_feat_num++;
				sprintf(ref_feat,"%-d",ref_feat_num);
				strcat(new_expression,ref_feat);
				j += strlen(ref_feat);
			}
			else	/* leave the reference feature number alone */
			{
				new_expression[j] = expression[i];
				j++;
			}
		}
		else
		{
			new_expression[j] = expression[i];
			j++;
		}
	}
	dbPutFeatureExpression(edit_feature,new_expression);
	free(new_expression);
	return;
}

void dbIncRefFeatureNumbers(Feature *edit_feature, int insert_feat_num)
{
	char *loc_func_name = "dbIncRefFeatureNumbers";
	int number_of_points;
	int i;
	int ref_feat_num;

	number_of_points = dbGetNumberPoints(edit_feature);
	for (i = 1; i <= number_of_points; i++)
	{
		if ((ref_feat_num = dbGetRefFeatureNumber(edit_feature,i)) < 0)
		{
			errPost(loc_func_name,DATABASE_ERROR,GET_FROM_DB_FAILED,"reference feature number",i);
		}
		if ( (ref_feat_num >= insert_feat_num) &&
			 (ref_feat_num <= dbVeryLastFeatureNumber()) )
		{
			ref_feat_num++;
			if (dbPutRefFeatureNumber(edit_feature,i,ref_feat_num) != NO_ERROR)
			{
				errPost(loc_func_name,DATABASE_ERROR,PUT_FAILED,"reference feature number",ref_feat_num);
			}
		}
	}
	return;
}

int dbInsertFeature(int insert_feature_number)
{
	char *loc_func_name = "dbInsertFeature";
	Feature *this_feature = gb_last_feature;  /* start at end of feature list */
	Feature *new_feature = NULL;		/* pointer to a new feature structure */
	int this_number;								/* current feature number */
	int feature_type;
	int ref_number;
	int i;

	/* trying to insert a new feature at the end, don't bother */
	if (insert_feature_number > dbGetFeatureNumber(gb_last_feature))
	{
		return(NO_ERROR);
	}

	/* as long as we haven't reached the beginning of the list */
	while (this_feature != NULL)
	{
		/* get the feature number from this feature */
		this_number = dbGetFeatureNumber(this_feature);

		/* this feature number bigger or equal to feature number to insert ? */
		if (this_number >= insert_feature_number)
		{
			/* increment and override this feature number */
			dbOverrideFeatureNumber(this_feature,this_number + 1);
			if (dbIsFeatureConstructed(this_feature))
			{
				feature_type = dbGetFeatureType(this_feature);
				if ( (feature_type == MATH) || (feature_type == BRANCH) )
				{
					dbIncMathFeatureNumbers(this_feature,insert_feature_number);
				}
				dbIncRefFeatureNumbers(this_feature,insert_feature_number);
			}
			else
			{
				feature_type = dbGetFeatureType(this_feature);
				if ((feature_type == ROTARY_FT) && (dbGetLastDisplay(this_feature) == ROTARY_RESULT))
				{
					ref_number = dbGetMiscFlag(this_feature);
					/* increment reference to math block */
					if (ref_number >= insert_feature_number)
						dbPutMiscFlag(this_feature, ref_number + 1);
				}
				for (i = 1; i <= 3; i++)
				{
					ref_number = dbGetSearchOnStep(this_feature, i);
					if (ref_number >= insert_feature_number)
					{
						dbPutSearchOnStep(this_feature, i, ref_number + 1);
					}
				}
			}
		}

		if (this_number == insert_feature_number)
		{
			/* we've found the feature to insert the new feature before */
			/* get a new feature structure */
			new_feature = dbNewFeature();
			if (new_feature == NULL)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(GOT_ERROR);
			}

			/* override the new feature number */
			dbOverrideFeatureNumber(new_feature,insert_feature_number);

			/* set a feature type so we don't get an error when we turn on */
			/* the model after inserting during step but before finishing */
			/* the feature to be inserted. */
			(void) dbPutFeatureType(new_feature,HOLD_FEATURE);	// ONLY OCCURANCE

			/* change the new feature's previous feature pointer */
			/* to point to this feature's previous feature */
			new_feature->prev_feature = this_feature->prev_feature;

			/* change the new feature's next feature pointer */
			/* to this feature */
			new_feature->next_feature = this_feature;

			/* now change this feature's next feature pointer */
			/* to point to the new feature */
			this_feature->prev_feature = new_feature;

			/* if there is a next feature (ie. not at the end of the list) */
			if (new_feature->prev_feature == NULL)
			{
				/* overwrite the first feature pointer */
				gb_first_feature = new_feature;
			}
			else
			{
				this_feature = new_feature->prev_feature;
				this_feature->next_feature = new_feature;
			}

			memmove(&gb_feature_ptrs[insert_feature_number + 1],&gb_feature_ptrs[insert_feature_number],
					sizeof(Feature *) * (gb_last_feature_number - insert_feature_number + 1));
			gb_feature_ptrs[insert_feature_number] = new_feature;

			return(NO_ERROR);
		}

		/* set the current feature pointer to the previous feature */
		this_feature = this_feature->prev_feature;
	}

	/* if we get here, we didn't find the feature to delete, return GOT_ERROR */
	return(GOT_ERROR);
}

#ifdef NEVER
/* 10/7/93 - changed insert for QCHECK (needs to be able to insert block 1) */
/* this is the old version of dbInsertFeature */
int dbInsertFeature(int feature_number)
{
	char *loc_func_name = "dbInsertFeature";
	Feature *this_feature = gb_last_feature;  /* start at end of feature list */
	Feature *new_feature = NULL;		/* pointer to a new feature structure */
	int this_number;								/* current feature number */

	/* as long as we haven't reached the beginning of the list */
	while (this_feature != NULL)
	{
		/* get the feature number from this feature */
		this_number = dbGetFeatureNumber(this_feature);

		/* this feature number bigger or equal to feature number to insert ? */
		if (this_number >= feature_number)
		{
			/* increment feature number and override this feature number */
			this_number++;
			dbOverrideFeatureNumber(this_feature,this_number);
			if (dbIsFeatureConstructed(this_feature))
			{
				dbIncRefFeatureNumbers(this_feature,feature_number);
			}
		}
		else	/* this feature number is 1 less than this feature number */
		{	/* can only get here the first time this feature is 1 less than */
			/* the feature number we want to insert */
			/* we've found the feature to insert the new feature after */
			/* get a new feature structure */
			new_feature = dbNewFeature();
			if (new_feature == NULL)
			{
				errPost(loc_func_name,SYSTEM_ERROR,NO_MEMORY,NULL,NO_VAL);
				return(GOT_ERROR);
			}

			/* override the new feature number */
			dbOverrideFeatureNumber(new_feature,feature_number);

			/* change the new feature's previous feature pointer */
			/* to point to this feature */
			new_feature->prev_feature = this_feature;

			/* change the new feature's next feature pointer */
			/* to this feature's next feature pointer */
			new_feature->next_feature = this_feature->next_feature;

			/* now change this feature's next feature pointer */
			/* to point to the new feature */
			this_feature->next_feature = new_feature;

			/* if there is a next feature (ie. not at the end of the list) */
			if (new_feature->next_feature != NULL)
			{
				/* go back down to the next feature in the list */
				this_feature = new_feature->next_feature;

				/* and change that feature's previous feature pointer */
				/* to point to the new feature */
				this_feature->prev_feature = new_feature;
			}
			else	/* this is the last feature in the list */
			{
				/* overwrite the last feature pointer */
				gb_last_feature = new_feature;
			}

			return(NO_ERROR);
		}
		/* set the current feature pointer to the previous feature */
		this_feature = this_feature->prev_feature;
	}

	/* if we get here, we didn't find the feature to delete, return GOT_ERROR */
	return(GOT_ERROR);
}
#endif

void dbDeleteLastFeatureNumber(void)
{
	Feature *save_ptr = NULL;

	if ( (gb_last_feature_number > 0) && (gb_last_feature != NULL) )
	{
		if (gb_last_feature_number == dbGetFeatureNumber(gb_last_feature))
		{
			gb_feature_ptrs[gb_last_feature_number] = NULL;
			gb_last_feature_number--;
			save_ptr = gb_last_feature->prev_feature;
			dbDisposeFeature(gb_last_feature);
			gb_last_feature = save_ptr;
			if (gb_last_feature != NULL)
			{
				gb_last_feature->next_feature = NULL;
			}
			else
			{
				gb_first_feature = NULL;
			}
		}
	}
	return;
}

int dbTraverseFeatures(int start_at_head)
{
	static Feature *this_feature = NULL;
	int feat_num = 0;

	if (start_at_head == TRUE)
	{
		this_feature = gb_first_feature;
	}
	else
	{
		this_feature = this_feature->next_feature;
	}
	if (this_feature != NULL)
	{
		feat_num = dbGetFeatureNumber(this_feature);
	}
	return(feat_num);
}
