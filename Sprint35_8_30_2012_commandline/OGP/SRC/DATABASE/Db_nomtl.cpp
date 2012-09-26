/*
$Workfile: Db_nomtl.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/Db_nomtl.c 7     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/Db_nomtl.c $
 * 
 * 7     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 6     1/24/00 3:38p Ttc
 * Add search on.
 * 
 * 5     6/03/99 3:05p Ttc
 * DN 9723 version 4. Add nominal size 3 and tolerance access functions.
 * 
 * 4     5/26/99 11:04a Ttc
 * Change Auxilary size and tolerance function names and internal variable
 * names to nominal size2, and upper and lower size2 tols in preparation
 * to add a nominal size 3 with tolerances for cone.
 * 
 * 3     4/20/99 6:05p Ttc
 * Add last display 3 flag, nominal, upper and lower elevation angle,
 * actual true position, FLAG_ELEV for elevation angle flagging.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:16   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:42   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:10   softadmn
 * Initial revision.
 * 
 *    Rev 8.0   18 Jan 1995 15:25:56   softadmn
 * Initial revision.
*/

/* OGP includes */
#include "err_hand.h"

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


void dbPutNominalLocX(Feature *this_feature, double nom_x)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Save the nominal X location in the feature nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nom_loc.x = nom_x;		   /* save the x location */
	return;
}

double dbGetNominalLocX(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the X nominal location from the feature structure.		  */
/* Returns: The X location.													  */
{
	return(this_feature->nominals.nom_loc.x);			  /* return the X loc */
}

void dbPutNominalLocY(Feature *this_feature, double nom_y)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Save the nominal Y location in the feature nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nom_loc.y = nom_y;		   /* save the y location */
	return;
}

double dbGetNominalLocY(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the Y nominal location from the feature structure.		  */
/* Returns: The Y location.													  */
{
	return(this_feature->nominals.nom_loc.y);			  /* return the Y loc */
}

void dbPutNominalLocZ(Feature *this_feature, double nom_z)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Save the nominal Z location in the feature nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nom_loc.z = nom_z;		   /* save the z location */
	return;
}

double dbGetNominalLocZ(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the Z nominal location from the feature structure.		  */
/* Returns: The Z location.													  */
{
	return(this_feature->nominals.nom_loc.z);			  /* return the Z loc */
}

void dbPutUpperLocTolX(Feature *this_feature, double x_tol)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the upper location x tolerance in the feature structure.    */
/* Returns: Nothing.														  */
{
	this_feature->nominals.upper_loc_tol.x = x_tol;	  /* save the upper x tol */
	return;
}

double dbGetUpperLocTolX(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the upper location x tolerance from the feature structure.*/
/* Returns: The upper location x tolerance.									  */
{
	return(this_feature->nominals.upper_loc_tol.x);	/* return the upper x tol */
}

void dbPutLowerLocTolX(Feature *this_feature, double x_tol)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the lower location x tolerance in the feature structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.lower_loc_tol.x = x_tol;	  /* save the lower x tol */
	return;
}

double dbGetLowerLocTolX(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the lower location x tolerance from the feature structure.*/
/* Returns: The lower location x tolerance.										  */
{
	return(this_feature->nominals.lower_loc_tol.x);	/* return the lower x tol */
}

void dbPutUpperLocTolY(Feature *this_feature, double y_tol)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the upper location y tolerance in the feature structure.    */
/* Returns: Nothing.														  */
{
	this_feature->nominals.upper_loc_tol.y = y_tol;	  /* save the upper y tol */
	return;
}

double dbGetUpperLocTolY(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Eytract the upper location y tolerance from the feature structure.*/
/* Returns: The upper location y tolerance.									  */
{
	return(this_feature->nominals.upper_loc_tol.y);	/* return the upper y tol */
}

void dbPutLowerLocTolY(Feature *this_feature, double y_tol)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the lower location y tolerance in the feature structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.lower_loc_tol.y = y_tol;	  /* save the lower y tol */
	return;
}

double dbGetLowerLocTolY(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Eytract the lower location y tolerance from the feature structure.*/
/* Returns: The lower location y tolerance.										  */
{
	return(this_feature->nominals.lower_loc_tol.y);	/* return the lower y tol */
}

void dbPutUpperLocTolZ(Feature *this_feature, double z_tol)
/* Author: Terrz T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the upper location z tolerance in the feature structure.    */
/* Returns: Nothing.														  */
{
	this_feature->nominals.upper_loc_tol.z = z_tol;	  /* save the upper z tol */
	return;
}

double dbGetUpperLocTolZ(Feature *this_feature)
/* Author: Terrz T. Cramer									 Date: 03/28/1991 */
/* Purpose: Eztract the upper location z tolerance from the feature structure.*/
/* Returns: The upper location z tolerance.									  */
{
	return(this_feature->nominals.upper_loc_tol.z);	/* return the upper z tol */
}

void dbPutLowerLocTolZ(Feature *this_feature, double z_tol)
/* Author: Terrz T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the lower location z tolerance in the feature structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.lower_loc_tol.z = z_tol;	  /* save the lower z tol */
	return;
}

double dbGetLowerLocTolZ(Feature *this_feature)
/* Author: Terrz T. Cramer									 Date: 03/28/1991 */
/* Purpose: Eztract the lower location z tolerance from the feature structure.*/
/* Returns: The lower location z tolerance.										  */
{
	return(this_feature->nominals.lower_loc_tol.z);	/* return the lower z tol */
}

void dbPutNominalSize(Feature *this_feature, double nom_size)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the nominal feature size in the feature nominal structure.  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nom_size = nom_size;
	return;
}

double dbGetNominalSize(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the nominal feature size from the feature nominals.		  */
/* Returns: The feature size.												  */
{
	return(this_feature->nominals.nom_size);				   /* return size */
}

void dbPutUpperSizeTol(Feature *this_feature, double size_tol)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the upper size tolerance in the feature nominal structure.  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.upper_size_tol = size_tol;	/* save the tolerance */
	return;
}

double dbGetUpperSizeTol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the upper size tolerance from the feature structure.	  */
/* Returns: The upper size tolerance.										  */
{
	return(this_feature->nominals.upper_size_tol);	  /* return the tolerance */
}

void dbPutLowerSizeTol(Feature *this_feature, double size_tol)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the lower size tolerance in the feature nominal structure.  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.lower_size_tol = size_tol;	/* save the tolerance */
	return;
}

double dbGetLowerSizeTol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the lower size tolerance from the feature structure.	  */
/* Returns: The lower size tolerance.										  */
{
	return(this_feature->nominals.lower_size_tol);	  /* return the tolerance */
}

void dbPutNominalSize2(Feature *this_feature, double nom_size)
/* Author: Terry T. Cramer									 Date: 05/09/1991 */
/* Purpose: Store the second feature size in the feature nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nom_size2 = nom_size;
	return;
}

double dbGetNominalSize2(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 05/09/1991 */
/* Purpose: Extract the second feature size from the feature nominals.		  */
/* Returns: The second size.												  */
{
	return(this_feature->nominals.nom_size2);				   /* return size */
}

void dbPutUpperSize2Tol(Feature *this_feature, double size_tol)
/* Author: Terry T. Cramer									 Date: 05/09/1991 */
/* Purpose: Store the upper 2nd feature tolerance in the nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.upper_size2_tol = size_tol;	/* save the tolerance */
	return;
}

double dbGetUpperSize2Tol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 05/09/1991 */
/* Purpose: Return the upper 2nd feature tolerance from the nominal structure.*/
/* Returns: The first auxilary tolerance.									  */
{
	return(this_feature->nominals.upper_size2_tol);	  /* return the tolerance */
}

void dbPutLowerSize2Tol(Feature *this_feature, double size_tol)
/* Author: Terry T. Cramer									 Date: 05/09/1991 */
/* Purpose: Store the lower 2nd feature tolerance in the nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.lower_size2_tol = size_tol;	/* save the tolerance */
	return;
}

double dbGetLowerSize2Tol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 05/09/1991 */
/* Purpose: Return the lower 2nd feature tolerance from the feature structure.*/
/* Returns: The second auxilary tolerance.									  */
{
	return(this_feature->nominals.lower_size2_tol);	  /* return the tolerance */
}

void dbPutNominalElevationAngle(Feature *this_feature, double nom_elev)
{
	this_feature->nominals.elev_angle = nom_elev;
	return;
}

double dbGetNominalElevationAngle(Feature *this_feature)
{
	return(this_feature->nominals.elev_angle);
}

void dbPutUpperElevationTol(Feature *this_feature, double elev_tol)
{
	this_feature->nominals.upper_elev_tol = elev_tol;
	return;
}

double dbGetUpperElevationTol(Feature *this_feature)
{
	return(this_feature->nominals.upper_elev_tol);
}

void dbPutLowerElevationTol(Feature *this_feature, double elev_tol)
{
	this_feature->nominals.lower_elev_tol = elev_tol;
	return;
}

double dbGetLowerElevationTol(Feature *this_feature)
{
	return(this_feature->nominals.lower_elev_tol);
}

void dbPutNominalSize3(Feature *this_feature, double nom_size)
/* Author: Terry T. Cramer									 Date: 06/01/1999 */
/* Purpose: Store the third feature size in the feature nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nom_size3 = nom_size;
	return;
}

double dbGetNominalSize3(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 06/01/1999 */
/* Purpose: Extract the third feature size from the feature nominals.		  */
/* Returns: The second size.												  */
{
	return(this_feature->nominals.nom_size3);				   /* return size */
}

void dbPutUpperSize3Tol(Feature *this_feature, double size_tol)
/* Author: Terry T. Cramer									 Date: 06/01/1999 */
/* Purpose: Store the upper 3rd feature tolerance in the nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.upper_size3_tol = size_tol;	/* save the tolerance */
	return;
}

double dbGetUpperSize3Tol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 06/01/1999 */
/* Purpose: Return the upper 3rd feature tolerance from the nominal structure.*/
/* Returns: The first auxilary tolerance.									  */
{
	return(this_feature->nominals.upper_size3_tol);	  /* return the tolerance */
}

void dbPutLowerSize3Tol(Feature *this_feature, double size_tol)
/* Author: Terry T. Cramer									 Date: 06/01/1999 */
/* Purpose: Store the lower 3rd feature tolerance in the nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.lower_size3_tol = size_tol;	/* save the tolerance */
	return;
}

double dbGetLowerSize3Tol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 06/01/1999 */
/* Purpose: Return the lower 3rd feature tolerance from the feature structure.*/
/* Returns: The second auxilary tolerance.									  */
{
	return(this_feature->nominals.lower_size3_tol);	  /* return the tolerance */
}

void dbPutNominalFormTol(Feature *this_feature, double tolerance)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Store the nominal form tolerance in the feature nominal structure.*/
/* Returns: Nothing.														  */
{
	this_feature->nominals.form_tol = tolerance;		/* save the tolerance */
	return;
}

double dbGetNominalFormTol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 03/28/1991 */
/* Purpose: Extract the form tolerance from the feature structure.			  */
/* Returns: The form tolerance.												  */
{
	return(this_feature->nominals.form_tol);		  /* return the tolerance */
}

void dbPutNominalPosTol(Feature *this_feature, double pos_tol)
/* Author: Terry T. Cramer									 Date: 04/09/1991 */
/* Purpose: Store the true position tolerance in the feature structure.		  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.position_tol = pos_tol;		/* save the tolerance */
	return;
}

double dbGetNominalPosTol(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 04/09/1991 */
/* Purpose: Extract the true position tolerance from the feature structure.	  */
/* Returns: The true position tolerance.									  */
{
	return(this_feature->nominals.position_tol);	  /* return the tolerance */
}

void dbPutNominalFlags(Feature *this_feature, int flag1, int flag2)
/* Author: Terry T. Cramer									 Date: 11/21/1991 */
/* Purpose: Store the nominal flags in the feature structure.				  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nom_flag1 = flag1;
	this_feature->nominals.nom_flag2 = flag2;
	return;
}

void dbGetNominalFlags(Feature *this_feature, int *flag1, int *flag2)
/* Author: Terry T. Cramer									 Date: 11/21/1991 */
/* Purpose: Extract the true position tolerance from the feature structure.	  */
/* Returns: The true position tolerance.  Also set are the material condition */
/*			specifiers.														  */
{
	*flag1 = this_feature->nominals.nom_flag1;
	*flag2 = this_feature->nominals.nom_flag2;
	return;
}

void dbPutToleranceType(Feature *this_feature, int tol_type)
/* Author: Terry T. Cramer									 Date: 12/06/1991 */
/* Purpose: Store the tolerance type flag in the feature structure.			  */
/* Returns: NO_ERROR if the tolerance type is valid, GOT_ERROR otherwise.	  */
{
	this_feature->nominals.tolerance_type = tol_type;
	return;
}

int dbGetToleranceType(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 12/06/1991 */
/* Purpose: Extract the tolerance type flag from the feature structure.		  */
/* Returns: The tolerance type flag.										  */
{
	return(this_feature->nominals.tolerance_type);
}

void dbSetNominalsEntered(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 10/16/1994 */
/* Purpose: Set the nominals entered flag in the feature nominal structure.	  */
/* Returns: Nothing.														  */
{
	if ( (dbGetNominalLocX(this_feature) != 0.0) ||
		 (dbGetUpperLocTolX(this_feature) != 0.0) ||
		 (dbGetLowerLocTolX(this_feature) != 0.0) ||
		 (dbGetNominalLocY(this_feature) != 0.0) ||
		 (dbGetUpperLocTolY(this_feature) != 0.0) ||
		 (dbGetLowerLocTolY(this_feature) != 0.0) ||
		 (dbGetNominalLocZ(this_feature) != 0.0) ||
		 (dbGetUpperLocTolZ(this_feature) != 0.0) ||
		 (dbGetLowerLocTolZ(this_feature) != 0.0) ||
		 (dbGetNominalSize(this_feature) != 0.0) ||
		 (dbGetUpperSizeTol(this_feature) != 0.0) ||
		 (dbGetLowerSizeTol(this_feature) != 0.0) ||
		 (dbGetNominalSize2(this_feature) != 0.0) ||
		 (dbGetUpperSize2Tol(this_feature) != 0.0) ||
		 (dbGetLowerSize2Tol(this_feature) != 0.0) ||
		 (dbGetNominalFormTol(this_feature) != 0.0) ||
		 (dbGetNominalPosTol(this_feature) != 0.0) )
	{
		this_feature->nominals.nominals_entered = TRUE;
	}
	else
	{
		this_feature->nominals.nominals_entered = FALSE;
	}
	return;
}

void dbPutNominalsEntered(Feature *this_feature, int nom_entered)
/* Author: Terry T. Cramer									 Date: 10/16/1994 */
/* Purpose: Put the nominals entered flag in the feature nominal structure.	  */
/* Returns: Nothing.														  */
{
	this_feature->nominals.nominals_entered = nom_entered;
	return;
}

int dbGetNominalsEntered(Feature *this_feature)
/* Author: Terry T. Cramer									 Date: 10/16/1994 */
/* Purpose: Extract the nominals entered flag from the nominal structure.	  */
/* Returns: The nominals entered flag.										  */
{
	return(this_feature->nominals.nominals_entered);
}

int dbPutSearchOnStep(Feature *this_feature, int which, int step)
{
	if ( (which >= 1) && (which <= 3) )
	{
		this_feature->nominals.search_on_steps[which - 1] = step;
		return(NO_ERROR);
	}
	return(GOT_ERROR);
}

int dbGetSearchOnStep(Feature *this_feature, int which)
{
	int step = -1;
	if ( (which >= 1) && (which <= 3) )
		step = this_feature->nominals.search_on_steps[which - 1];
	return(step);
}
