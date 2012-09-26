/*
$Workfile: Db_rpt.cpp $

$Header: /MeasureMind Plus/Rev 11/Subproject/database/Db_rpt.cpp 5     3/22/01 8:36a Jnc $

$Modtime: 3/20/01 1:10p $

$Log: /MeasureMind Plus/Rev 11/Subproject/database/Db_rpt.cpp $
 * 
 * 5     3/22/01 8:36a Jnc
 * SCR13258:added functions to get and put end step value for repeat
 * 
 * 4     11/13/00 1:24p Jpk
 * Add include of db_proto.h for db dll.
 * 
 * 3     11/01/00 9:53a Ttc
 * SCR 11834.  Change default of finish run before repeat to be on.
 * 
 *    Rev 10.0   20 Mar 1998 12:13:20   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 11:12:52   softadmn
 * Initial revision.
 * 
 *    Rev 9.0   11 Mar 1996 10:56:18   softadmn
 * Initial revision.
 * 
 *    Rev 8.2   12 Dec 1995 10:26:32   ttc
 * Finialize part repeat.
 * 
 *    Rev 8.1   05 Dec 1995 14:56:58   ttc
 * Repeat enhancements.
 * 
 *    Rev 8.0   18 Jan 1995 15:26:14   softadmn
 * Initial revision.
*/

/* MSC includes */
#include <stdio.h>

/* Database includes */
#include "db_const.h"
#include "db_decl.h"
#include "db_proto.h"


/* OGP includes */
#include "gn_stdio.h"

int gb_fixture_repeat = FALSE;
int gb_repeat_this_offset[MAX_REPEAT_OFFSETS] = { 0 };
double gb_repeat_x_offsets[MAX_REPEAT_OFFSETS] = { 0.0 };
double gb_repeat_y_offsets[MAX_REPEAT_OFFSETS] = { 0.0 };
int gb_repeat_confirm_next = FALSE;

int gb_repeat_finish_run = TRUE;	/* shared by both */
int gb_repeat_start_step = 1;
int gb_repeat_end_step = 0;

int gb_part_repeat = FALSE;
double gb_x_part_offset = 0.0;
double gb_y_part_offset = 0.0;
int gb_x_part_times = 0;
int gb_y_part_times = 0;
int gb_repeat_y_first = 0;

/* FIXTURE REPEAT FUNCTIONS */
int dbGetFixtureRepeatFlag(void)
{
	return(gb_fixture_repeat);
}

void dbSetFixtureRepeatFlag(int fixture_repeat)
{
	gb_fixture_repeat = fixture_repeat;
	return;
}

int dbGetRepeatOffsetOnOff(int which_offset)
{
	if ( (which_offset < 0) || (which_offset > MAX_REPEAT_OFFSETS) )
	{
		which_offset = 0;
	}
	return(gb_repeat_this_offset[which_offset]);
}

void dbPutRepeatOffsetOnOff(int which_offset, int on_or_off)
{
	if ( (which_offset >= 0) && (which_offset < MAX_REPEAT_OFFSETS) )
	{
		gb_repeat_this_offset[which_offset] = on_or_off;
	}
	return;
}

double dbGetRepeatXOffset(int which_offset)
{
	if ( (which_offset < 0) || (which_offset > MAX_REPEAT_OFFSETS) )
	{
		which_offset = 0;
	}

	return(gb_repeat_x_offsets[which_offset]);
}

void dbPutRepeatXOffset(int which_offset, double x_offset)
{
	if ( (which_offset >= 0) && (which_offset < MAX_REPEAT_OFFSETS) )
	{
		gb_repeat_x_offsets[which_offset] = x_offset;
	}
	return;
}

double dbGetRepeatYOffset(int which_offset)
{
	if ( (which_offset < 0) || (which_offset > MAX_REPEAT_OFFSETS) )
	{
		which_offset = 0;
	}
	return(gb_repeat_y_offsets[which_offset]);
}

void dbPutRepeatYOffset(int which_offset, double y_offset)
{
	if ( (which_offset >= 0) && (which_offset < MAX_REPEAT_OFFSETS) )
	{
		gb_repeat_y_offsets[which_offset] = y_offset;
	}
	return;
}

int dbGetRepeatConfirmNext(void)
{
	return(gb_repeat_confirm_next);
}

void dbPutRepeatConfirmNext(int to_confirm_next_or_not)
{
	gb_repeat_confirm_next = to_confirm_next_or_not;
}

/* SHARED FUNCTIONS */
int dbGetRepeatFinishRun(void)
{
	return(gb_repeat_finish_run);
}

void dbPutRepeatFinishRun(int to_finish_or_not_to_finish)
{
	gb_repeat_finish_run = to_finish_or_not_to_finish;
}

int dbGetRepeatStartStep(void)
{
	return(gb_repeat_start_step);
}

void dbPutRepeatStartStep(int start_step)
{
	gb_repeat_start_step = start_step;
}

int dbGetRepeatEndStep(void)
{
	return(gb_repeat_end_step);
}

void dbPutRepeatEndStep(int end_step)
{
	gb_repeat_end_step = end_step;
}

/* PART REPEAT FUNCTIONS */
int dbGetPartRepeatFlag(void)
{
	return(gb_part_repeat);
}

void dbSetPartRepeatFlag(int part_repeat)
{
	gb_part_repeat = part_repeat;
	return;
}

double dbGetXPartOffset(void)
{
	return(gb_x_part_offset);
}

void dbPutXPartOffset(double x_offset)
{
	gb_x_part_offset = x_offset;
	return;
}

double dbGetYPartOffset(void)
{
	return(gb_y_part_offset);
}

void dbPutYPartOffset(double y_offset)
{
	gb_y_part_offset = y_offset;
	return;
}

int dbGetXPartTimes(void)
{
	return(gb_x_part_times);
}

void dbPutXPartTimes(int number_times)
{
	gb_x_part_times = number_times;	
	return;
}

int dbGetYPartTimes(void)
{
	return(gb_y_part_times);
}

void dbPutYPartTimes(int number_times)
{
	gb_y_part_times = number_times;
	return;
}

int dbGetYPartFirst(void)
{
	return(gb_repeat_y_first);
}

void dbPutYPartFirst(int y_first)
{
	gb_repeat_y_first = y_first;
	return;
}
