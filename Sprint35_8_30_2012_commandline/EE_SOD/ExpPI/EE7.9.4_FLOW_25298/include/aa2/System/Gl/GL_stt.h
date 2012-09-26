/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
// This is a part of the PowerSYS System C++ library.
// Copyright (C) 1996 PADS Software Inc.
// All rights reserved.

// Name:				GL_stt.h
// Date:				10.04.98
// Author:			Alexander Zabolotnikov	
// Description:	Statistics collection

#ifndef __SAS_STT_H__
#define __SAS_STT_H__
#include <time.h>

#define LEN_PROC_NAME 60

const unsigned long STT_COMPRESS		=	0x00000001;	// for high frequency functions
const unsigned long STT_SEPARATOR	=	0x00000002;	// do statistics for each call

// to store one event
class SttItem
{					
public:
	double	t;			// event time
	int		num;		// for compressing
	short	proc_idx;	// index of event process
	short	tmp;		// is not used
};

// describe local statistics between two processes
class SttCrossItem
{
public:
	double	t;
	double	tw;
	int		count;
	int		ord;
};

class ProcItem
{
public:
	char		name[LEN_PROC_NAME + 1];	// process name
	int		count;							// event number
	double	inum;								// input data
	double	onum;								// output data
	unsigned long status;					// priority of process
	short		ord;								// process ord, calculated at the end
	short		hid;								// is the process hidden
	short		*idx_ptr;						//	for cleaning ?

	double	t_crn;							// current time
	double	full_time;						// full time
	double	mid_time;						// midle time
	
	double	reg_time;						// full time in region
	double	reg_mid;							// middle time in region
	int		reg_count;						// count in reg
	int		reg_inum;						// input number in the region
	int		reg_onum;						// output number in the region
};

class DrwFill;	// For FILLING PARAMETERS EXTENTION (temporary)

// to collect statistics for all processes
class SYSGL_API glStt
{
public:
	static int	used_proc;						// used processes
	static int	used_event;						// the number of events in static buffer
	static int	num_reg;							// the number of time regions
	static int	mode;								// full// compressed //...
	static ProcItem table[];					// for statistics collection
	static SttItem buf_item[];					// events buffer
	static short stt_id;							// own index of glStt:: process
	static int	comp_id;							// current index of compressed part
	static double all_start;					// starting time of measuring
	static double one_clock_time;				// time of one clock
	static double start_sec;
	static SttCrossItem *CrossTable;			// one item collect statistics between two process
	static double st_time;
	static int is_start_stt;
	static char	header_string[256];			// the object of measuring
	static int tmp_data[];					// 

public:
	static	void InitAll(char *file_name = NULL);
	static	void StopAll();
	static	void SetNumReg(int num) {num_reg = num;}

	static	void Start(char *name, short &idx);
	static	void End();

	static void SetInCount(double num);
	static void SetOutCount(double num);
	static void LocalParam(char *pname, double num);
	static void GlobalParam(char *pname, double num);
	
	static	void CadReport(const char *file_name = NULL);
	static	void BreReport(const char *file_name = NULL);
	static	void PcbReport(const char *file_name = NULL);
	static	void CrsReport(const char *file_name = NULL);
	static	int	 IsActive();

	// internal
	static	void SortTable();
	static	void CollectLocal();
	static	void CollectStt(char *file_name = NULL);
	static	void MoveToFile();
	static	void Compress();
	static	int  GetFirstLastEvent(char *f_name,SttItem &s0, SttItem &sl, ProcItem *table, int &num);
	static	void MoveTime(double &t1, double &t2);

	static	int cad_report;
	static	int bre_report;
	static	int pcb_report;
	static	int crs_report;
	static	int in_file_mode;

////////////////////////// FILLING PARAMETERS EXTENTION (temporary) //////////////////////////
public:
	static DrwFill *drw_fill;
	static DrwFill *GetFilling() { return(drw_fill);}
	static void SetFilling(DrwFill *a_drw_fill) { drw_fill = a_drw_fill;}
};


#ifndef UNIX
#pragma warning(disable:4035)
__inline __int64 TimeStampCounter(void)
{
	__asm {
		;RDTSC // Read Time Stamp Counter
		_emit 0Fh
		_emit 31h
	}		
}
#pragma warning(default:4035)

inline 	double GetCrnTime()
{
	return (double)TimeStampCounter();
}
#else // UNIX

inline 	double GetCrnTime()
{
	return (double)clock();
}

#endif // UNIX

inline 	double GetSecTime(double t)
{
	return t * (glStt::one_clock_time);
}

class SttSensor
{
public:
	SttSensor(char *f_name, short &i)	{ glStt::Start(f_name, i); id = &i;};
	~SttSensor()	{glStt::End();};
	short *id;
};

#define STT_SENSOR(a,b) static short stt_sen_idx = (b);SttSensor stt_sen_itm((a), stt_sen_idx)
#define STM_SENSOR(a,b) SttSensor stm_sen_itm((a), (b))

#endif	// __SAS_STT_H__

//
// End of file
//

