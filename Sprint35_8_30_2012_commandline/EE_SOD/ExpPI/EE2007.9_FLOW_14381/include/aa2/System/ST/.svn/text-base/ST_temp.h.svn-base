/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

// Name:			ST_temp.h
// Date:			12.01.04
// Author:			Alex Zabolotnikov
// Description:	temporal file

#ifndef __SAS__TEMP_H__
#define __SAS__TEMP_H__

class	PolyArc;
class	GeomHdr;
class	GeomGroup;
class	glCircle;
class	IterPolyGeom;
class	IterGeom;
class	IterGeomGroup;
class	LayerScanner;
class	stScan;

#define GL_MAX_DOUBLE_COORD ((double)GL_MAX_COORD)
class	SYSGL_API LayerScanner
{
public:	// data
	stScan	*scan;
	GeomHdr	*first_geom;
	GeomHdr	*last_geom;
	bool		update;
	bool		init;
	int			type;
	int			layer;
	unsigned long	scan_type;

public:	// methods
	LayerScanner();
	~LayerScanner();
	void Init0();
	void AddHeadGeom(GeomHdr * a_obj);
	void AddTailGeom(GeomHdr * a_obj);
	void RemoveGeom(GeomHdr *);
	void RemoveAllGeom();
	unsigned long GetScanType() {return scan_type;}
	void InitializeScanner();
	void SetUpdateFlag();
	void SetHiddenFlag(GeomHdr *gh);
};

int AreaDecomposition( stScan *scan, int mode);

/*-----------------------------------------/
struct	GroupData 
{
	GL_Node			*poly;
	GeomGroup			*child;
	void						*misc;
};
// This structure organize refer to up(parent) level from geom. group. 
// GeomGroup			*parent; - pointer to header of the "parent" group
//	int					ind_in_parent; - index this group in the parent group.
struct	GroupParentData 
{
	GeomGroup	*parent;
	int				ind_in_parent;
	void				*misc;
};

class	GeomGroup : public GeomHdr
{
public:
	GroupParentData			*GetParentDataPtr			()	const;	
	void AddItm	(GroupData );
	int GetItmCount () const ;
	static GeomGroup *CreateTemp(int a_count, int a_add_count); 
};

//class IterAreaVtx : public IterPolyGeom{public:};



class	IterGeomGroup : public	IterPolyGeom
{
public:
	ODB_GroupIter();
	IterGeomGroup			(const GL_Node * a_hdr);
	IterGeomGroup			(const GL_Node & a_hdr);

	GL_Node	*GetCurentGeom				()	const;
	GeomGroup	*GetCurentChild			()	const;
	bool				GetFirstGroup				();
	bool				GetNextGroup				();
	bool				GetGlobalFirstGroup		();
	bool				GetGlobalNextGroup		();
	bool				SetGroup(GeomGroup *a_group, int a_group_id = 0);
	void	AddGroupToTail(GL_Node *a_poly, GeomGroup *a_child = NULL, void	*a_misc = NULL);
};
/-----------------------------------------*/


#endif	// __SAS__TEMP_H__
//
// End of file
//

