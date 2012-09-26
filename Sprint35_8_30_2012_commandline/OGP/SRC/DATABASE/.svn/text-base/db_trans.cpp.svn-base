/*
$Workfile: db_trans.c $

$Header: /MeasureMind Plus/Mmp_db_dll/Source/db_trans.c 4     11/13/00 1:24p Jpk $

$Modtime: 10/19/00 4:42p $

$Log: /MeasureMind Plus/Mmp_db_dll/Source/db_trans.c $
 * 
 * 4     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 *    Rev 9.0   24 Jul 1998 08:25:06   ttc
 * Initial revision.
*/

/* DataBase includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


void dbPutFeaturePartTransform(Feature *this_feature, Database_Transform *this_transform)
/* Author: Terry T. Cramer									 Date: 07/14/1998 */
/* Purpose: Store the part transform in the feature.						  */
/* Returns: None.															  */
{
	this_feature->part_transform.transform.d.x = this_transform->transform.d.x;
	this_feature->part_transform.transform.d.y = this_transform->transform.d.y;
	this_feature->part_transform.transform.d.z = this_transform->transform.d.z;
	this_feature->part_transform.transform.m.a.x = this_transform->transform.m.a.x;
	this_feature->part_transform.transform.m.a.y = this_transform->transform.m.a.y;
	this_feature->part_transform.transform.m.a.z = this_transform->transform.m.a.z;
	this_feature->part_transform.transform.m.b.x = this_transform->transform.m.b.x;
	this_feature->part_transform.transform.m.b.y = this_transform->transform.m.b.y;
	this_feature->part_transform.transform.m.b.z = this_transform->transform.m.b.z;
	this_feature->part_transform.transform.m.c.x = this_transform->transform.m.c.x;
	this_feature->part_transform.transform.m.c.y = this_transform->transform.m.c.y;
	this_feature->part_transform.transform.m.c.z = this_transform->transform.m.c.z;
	this_feature->part_transform.nominal_part_axis = this_transform->nominal_part_axis;
	this_feature->part_transform.actual_part_axis = this_transform->actual_part_axis;
	return;
}

void dbGetFeaturePartTransform(Feature *this_feature, Database_Transform *this_transform)
/* Author: Terry T. Cramer									 Date: 07/14/1998 */
/* Purpose: Extract the part transform from the feature.					  */
/* Returns: None.															  */
{
	this_transform->transform.d.x = this_feature->part_transform.transform.d.x;
	this_transform->transform.d.y = this_feature->part_transform.transform.d.y;
	this_transform->transform.d.z = this_feature->part_transform.transform.d.z;
	this_transform->transform.m.a.x = this_feature->part_transform.transform.m.a.x;
	this_transform->transform.m.a.y = this_feature->part_transform.transform.m.a.y;
	this_transform->transform.m.a.z = this_feature->part_transform.transform.m.a.z;
	this_transform->transform.m.b.x = this_feature->part_transform.transform.m.b.x;
	this_transform->transform.m.b.y = this_feature->part_transform.transform.m.b.y;
	this_transform->transform.m.b.z = this_feature->part_transform.transform.m.b.z;
	this_transform->transform.m.c.x = this_feature->part_transform.transform.m.c.x;
	this_transform->transform.m.c.y = this_feature->part_transform.transform.m.c.y;
	this_transform->transform.m.c.z = this_feature->part_transform.transform.m.c.z;
	this_transform->nominal_part_axis = this_feature->part_transform.nominal_part_axis;
	this_transform->actual_part_axis = this_feature->part_transform.actual_part_axis;
	return;
}
