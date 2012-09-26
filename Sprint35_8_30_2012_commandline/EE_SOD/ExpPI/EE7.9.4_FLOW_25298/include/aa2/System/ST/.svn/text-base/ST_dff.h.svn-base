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

// Name:				ST_dff.h
// Date:				02.01.00
// Author:			Alexander Zabolotnikov	
// Description:	dff checking interface

#ifndef __SAS_DFF_H__
#define __SAS_DFF_H__

#include "ST_algb.h"
#include "ST_area.h"

#define ARR_START_SIZE 1024

class SYSGL_API DFF
{
public:
	DFF();
	~DFF();
	void Init();
	inline void Activate();
	virtual int SetRegion(int layer,	GL_Box	*box);	     // Get data from ApplDB
	virtual int SetMaskRegion(int layer, GL_Box	*box);	  // Get mask data from ApplDB
	virtual int SetThermalRegions(int layer, GL_Box *box); // Get thermal regions from ApplDB
	virtual int WriteErrors();											     // Write errors to ApplDB

	// Finds acid traps in specified region
	// Returns	number of errors found 
	int	FindAcidTraps
	(
		int		layer,		// copper layer to check
		GL_Box			*box,			// region to check	
		long		width,		// width	the maximum width of an area
											// that is to be considered an acid trap,
											// i.e. if you have a slot in the copper
											// and its width is less than the specified
											// size then it is an acid trap. If the width
											// is greater then it is not an acid trap.
		double		angle			// angle	the maximum angle of an area that is
											// to be considered an acid trap, i.e. if you
											// have a triangular indentation in the copper
											// what is the maximum angle at the bottom
											// of the indentation that makes the
											// indentation an acid trap.
		//DFF_ErrorArray	&errors		// array of DFF_Error objects
	);

	// Finds copper and mask slivers in specified region
	// Returns	number of errors found 
	int	FindSlivers
	(
		int		layer,		//	layer to check.
		GL_Box			*box,			// region to check.
		long		width			// the maximum width of a sliver
		//DFF_ErrorArray	&errors		// 	array of DFF_Error objects
	);

	// Finds solder bridges in specified region 
	// Returns			number of errors found 
	int FindSolderBridges
	(
		int		copper_layer,	// copper layer to check.
		int 		mask_layer,		// solder mask layer to check.
		GL_Box			*box,           // region to check.
		long		size          // the maximum size of a bridge
		//DFF_ErrorArray	&errors        // array of DFF_Error objects
	);

	////////////////////////////////////////////////////////////////
	// Finds starved thermals in specified region 
	// Returns	number of errors found 
	int FindStarvedThermals
	(
		int		layer,		// copper/thermal layer to check.
		GL_Box			*box,       // region to check.
		int     	min_ties		// 0 == all ties must be clear regardless of the
											// number of ties that exist, i.e. if the minimum
											// number of ties is specified as 3, then for a 4
											// tie thermal 1 tie could be impinged and the
											// thermal still would not be considered starved,
											// for 6 tie thermal then 3 ties could be impinged.
											// On the other hand, if 'All' is specified then
											// all ties must be clear regardless
											// of the number of ties in the thermal.
		//DFF_ErrorArray	&errors     // array of DFF_Error objects
	);

	// Finds pin holes in specified region
	// Returns	number of errors found 
	int	FindPinHoles
	(
		int		layer,		//	layer to check.
		GL_Box			*box,			// region to check.
		long		width,		// the maximum width of a hole
		bool		ismasklayer	= FALSE // take as mask
	);

	int	FindSilkScreenOverPads
	(
		int		layer1,		// copper layer1 to check
		int		layer2,		// copper layer2 to check
		long		min_gap,		// the min gap
		GL_Box			*box = NULL	// region to check.
	);

	int CheckMinAngle
	(
		GeomHdr	*PolyList,		// polylines fo checking
		double		&angle,			// ABS(angle) < 180
											// if angle > 0 - left side checking (internal for area)
		GeomHdr **ErrList		// error markers
	);
	
	glAreaData			*ad;
	int				is_active;
	int				ovl_geom;
	int				ovl_mask;
	int				ovl_treg;
	GeomHdr		*first_geom;
	GeomHdr		*first_mask;
	GeomHdr		*first_treg;
	GeomHdr		*error_list;
	double			min_len;
	double			slv_len;
	double			len_ratio;
	long		trap_dist;
	long		sliv_dist;

	long		trap_dist2;
	long		sliv_dist2;
	long		fix_dist;
	
	double			t_angle;
	double			s_angle;
	int				fix_err;

	int     	min_regs;

	GL_Array		*iter_db;
	int				start_idd, num_idd;
	GL_Array		*iter_mask;
	int				start_idm, num_idm;
	GL_Array		*iter_reg;
	int				start_idr, num_idr;
	////////// temp ///////////
	int				chk_trap;
	int				chk_slv;
	int				chk_bridges;
	int				chk_thermals;
	int				chk_angles;
	int				chk_window;
	int				chk_pin_hole;

	int				out_poly;
	double			t_dist;
	double			s_dist;
	GeomGroup	*gg;
	GeomHdr		temp_geom;
	GeomHdr		*last_g;

	inline GeomHdr *CreateList();
	void CreateHdrList(GeomGroup	*gg);
	int GetDffParam(char *ini_dff_file);
	IterPolyCross *GetIters(GL_Box *box, stScan *scan, int &num);
	GeomHdr *CreateReg(GL_Point *p1, double fi, double len1, double len2, double w);
	GeomHdr *CreateReg(GL_Point *p1, double fi, double len, double w);
	void MakeWinList(GeomHdr *first, int dir, GeomHdr *last = NULL);
	void MakeAreaList(GeomHdr *first, int dir, GeomHdr *last = NULL);
	void InverseArea(GeomHdr *first);
	void InverseWindow(GeomHdr *first);

	GeomHdr *Merge(GeomHdr *master, GeomHdr *poly);
	GeomHdr *Subtract(GeomHdr *master, GeomHdr *poly);
	GeomHdr *Cross(GeomHdr *master, GeomHdr *poly, int use_width = 1);
	GeomHdr *ExclusiveOr(GeomHdr *a_master, GeomHdr *a_poly);
	
	GeomHdr *CrossBoxPoly(GeomHdr *box, GeomHdr *poly, int is_list = 0);

	GeomHdr *Merge(GeomHdr **a_master, GeomHdr **a_poly);
	GeomHdr *Subtract(GeomHdr **a_master, GeomHdr **a_poly);
	GeomHdr *Cross(GeomHdr **a_master, GeomHdr **a_poly);
	GeomHdr *ExclusiveOr(GeomHdr **a_master, GeomHdr **a_poly);

	GeomHdr *Expand(GeomHdr *gh, long w1 = 0, long w2 = 0, int use_w = 1);
	GeomHdr *Expand(GeomHdr **gh, long w1 = 0, long w2 = 0, int use_w = 1);
	inline GeomHdr *DeletePolyList(GeomHdr *gh);
	
	GeomHdr *MergeToGroup(GeomHdr **a_master);
	inline void SetLenRatio(double &scale);
	inline void SetAngle(double	angle);

	GeomHdr *Merge(DffLayer &cl);
	GeomHdr *Subtract(DffLayer &cl, GeomHdr *poly);
	int AddBox(GL_Box &box, long	dist = 0, int dir = 1);
	void SeparateLines(GeomHdr **obj, GeomHdr **msk);
	void CrsFilter(GeomHdr **obj, GeomHdr **msk);
};
inline void DFF::Activate()
{
	if (is_active == 0) {
		is_active = 1;
		Init();
	}
}


inline void DFF::SetLenRatio(double &scale) { len_ratio = scale;}

inline	GeomHdr *DFF::CreateReg(GL_Point *p1, double fi, double len, double w)
{
	return CreateReg(p1, fi, 0.0, len, w);
}

inline GeomHdr *DFF::CreateList()
{
	ad->CrossArea();
	temp_geom.next_geom = ad->out_list;
	ad->in_list	= DelPolyList(ad->in_list);
	ad->out_list = NULL;
	return temp_geom.next_geom;
}
inline GeomHdr *DFF::DeletePolyList(GeomHdr *gh)
{
	return DelPolyList(gh);
}
//~~*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class DB_Area : public	ConnObj
{
public:
	DB_Area();
	~DB_Area();
	int		AddObj(GL_Node *obj);
	int		DelObj(GL_Node *obj);
	int		Clear();
	void	Destroy();
	
	DB_Area	*Invert();
	DB_Area	*Merge		(DB_Area *a, DB_Area *b);
	DB_Area	*Subtract	(DB_Area *a, DB_Area *b);
	DB_Area	*Cross		(DB_Area *a, DB_Area *b);
	DB_Area	*ExclusiveOr(DB_Area *a, DB_Area *b);
	
	DB_Area	*Expand(double dist);
	DB_Area	*ChamferedExpand(double dist, double corner_dist, double max_angle = 91.0);
	DB_Area	*RemoveHoles();	// Convert to mentorgon()
	DB_Area	*RemoveArcs(double eps, int mode);

	GeomIter	*GetAreaIter();
	long		*GetHatchArray(int &num);    
	double		GetLength();
	double		GetArea();

	GeomHdr *AddList(GeomHdr *gh);
	GeomHdr *GetLast(GeomHdr *gh);
	GeomHdr *GiveBack();
private:

	static	DFF dc;
	static	GL_Array arr;
	GeomHdr			area_geom;
	int				area_ovl;
	unsigned int	area_flag;
};

#endif // __SAS_DFF_H__

