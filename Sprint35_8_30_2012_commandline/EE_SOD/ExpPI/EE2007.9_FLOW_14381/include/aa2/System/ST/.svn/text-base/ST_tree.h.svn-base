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

// Name:				ST_tree.h
// Date:				06.25.98
// Author:			Alexander Zabolotnikov	
// Description:	area topology calculation

#ifndef __SAS_TREE_H__
#define __SAS_TREE_H__
//////////////////////////////////////////////////
class AreaTreeNode;
class GeomGroup;
class SAS_Seg;
/////////////////////////////////////////////////////////////////////
// ChainArc	-	 describes arc item of polyline.
//					 used for internal polyline representation.
/////////////////////////////////////////////////////////////////////
class ChainArc: public GL_Point
{
public:
	long	rad;		// radius of arc
	int			num;		// references counter
};

/////////////////////////////////////////////////////////////////////
// ChainPoint - describes corner or circle of polyline.
//					 used as an internal polyline representation.
//					 can be modified much easier than regular polyline.
//					 (usefull in operations with often modifications)
/////////////////////////////////////////////////////////////////////
class ChainPoint: public GL_Point
{
public:
	ChainPoint	*next;	// next corner in the chain
	ChainArc	*arc;	// pointer to arc
public:
	int MinChnVtx();
	GeomHdr *CreateMonoPoly();
};

/////////////////////////////////////////////////////////////////////
// ChainHdr	-	 describes chain of polylines
//					 used for internal polyline representation.
/////////////////////////////////////////////////////////////////////
class ChnPntHdr
{
public:
	ChainPoint	*first;
	ChnPntHdr	*next;
	unsigned long	flag;
};

/////////////////////////////////////////////////////////////////////
// ChnPoint - describes corner of area map
/////////////////////////////////////////////////////////////////////
class ChnNode
{
public:
	ChainPoint	*first;
	ChnNode		*next;
	ChnNode		*dn;		// lower left branch
	ChnNode		*up;		// upper left branch
	unsigned long	flag;
};

/////////////////////////////////////////////////////////////////////
// OpenBranch - describes the open branche - piece of boundary
//					 polyline, which must be expanded to closed boundary
//					 polygon at the end of algebraic operation.
//					 used in area tree building
/////////////////////////////////////////////////////////////////////
class OpenBranch: public GL_Point
{
public:
	OpenBranch		*next;	// next open branch
	AreaTreeNode	*node;	// associated tree node
};

/////////////////////////////////////////////////////////////////////
// BoundBranch - describes the part of the boundary and it's position
//					 in respect to other parts of area.
//					 reflects the current status of the tree area building
//
//              left_up        right_up   
//					  first >------< last
//              left_dn        right_dn   
//
/////////////////////////////////////////////////////////////////////
class BoundBranch
{
public:
	ChainPoint  *first;		// polyline start
	BoundBranch *left_dn;	// lower left branch
	BoundBranch *left_up;	// upper left branch
	
	ChainPoint  *last;		// polyline end
	BoundBranch *right_dn;	// lower right branch
	BoundBranch *right_up;	// upper right branch
	
	AreaTreeNode	*parent;	// associated tree node
	short				dir;		// direction to area
	short				index;	// misk temporary info
};

/////////////////////////////////////////////////////////////////////
// AreaTreeNode - describes the node of topology tree:
//					   each node includes polygon (outline or void)
//						and may be the top of subtree, wich associated with
//						geometry inside the polygon
/////////////////////////////////////////////////////////////////////
class AreaTreeNode
{
public:
	AreaTreeNode	*next;	// next in the cluster
	AreaTreeNode	*up;		// parent
	AreaTreeNode	*dn;		// first child
	ChainPoint		*first;	// first polyline corner
	ChainPoint		*last;		// last polyline corner
	SAS_Seg			*f_seg;		// associated with first corner 
	SAS_Seg			*l_seg;		// associated with last corner 
	short				level;		//	node level in the tree
	short				type;			// node type: void or outline
};

extern void				Extend(AreaTreeNode	*node, int level);
extern GeomHdr	*CreatePoly(ChainPoint *first);
extern void				CreatePoly(AreaTreeNode *node);
extern GeomGroup	*CreateGroup(AreaTreeNode *node);
extern void				Revers(ChnPntHdr *first);
extern void				CreateChain(AreaTreeNode *tmp, ChnPntHdr **first, glPageMem *pm);

#endif	// __SAS_TREE_H__

