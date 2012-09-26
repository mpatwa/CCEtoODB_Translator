/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

// Name:				ST_lpsm.h
// Date:				09.10.04
// Author:			Alexander Zabolotnikov	
// Description:	Layer Plaine Shape Model public interface

#ifndef _LAYER_PSM_
#define _LAYER_PSM_


class LPSM;
class GL_Node;
class ConnObj;
class GeomIter;
class PSM_Point;
class PShapes;
class cPlaneShapeFillDraw;
class cPlaneShapeOutlineDraw;
class GeomHdr;

const unsigned int LPSM_NO_ARCS	 = 0x00000001;	// mentorgon generation modes
const unsigned int LPSM_NO_HOLES = 0x00000002;	// mentorgon generation modes
const unsigned int LPSM_DEL_SRC  = 0x00000004;	// mentorgon generation modes
const unsigned int LPSM_DYN_EDIT = 0x00000008;	// dynamic adit for plane edit scetch
const unsigned int LPSM_CENT_PLN = 0x00000010;	// plane shape by the center line
const unsigned int LPSM_CONN_SHL = 0x00000020;	// connectivity by the shell
const unsigned int LPSM_CONN_MTL = 0x00000040;	// connectivity by the metal
const unsigned int LPSM_CONN_R3  = 0x00000080;	//	TODO
const unsigned int LPSM_CENT_TIE = 0x00000100;	// tie legs are started from the pad center
const unsigned int LPSM_BND_PIE	 = 0x00000200;	// the boundary type
const unsigned int LPSM_ONE_PLN  = 0x00000400;	// the restriction for the iterator
const unsigned int LPSM_BRIDGING = 0x00010000;	// == LPSM_TIE_BRIDGING

const unsigned int LPSM_MENTORGON = (LPSM_NO_ARCS | LPSM_NO_HOLES);

#define APP_DATA_MGR 1

class GL_Box;
class GL_Array;
// Layer plane shapes model
class SYSGL_API LPSM
{
public:
	LPSM();
	LPSM(int layer);
	LPSM(LPSM &lpsm);
	~LPSM();
	LPSM operator=(LPSM &lpsm);
	// Remove all objects from LPSM, initialize parameters
	int Init(int layer);
	void Clear();
	int GetLPSM(int layer);

	void SetModes(unsigned int lpsm_flag, double arcs_precision = 0.0);
	void AddModes(unsigned int lpsm_flag);
	void DelModes(unsigned int lpsm_flag);
	unsigned int GetModes();

	void SetMinCorners(int min_number_of_corners); // the minimal number of corners on the boundary
	void SetMinConnections(int min_conn);
	void SetMinArea(double min_area);
	void SetUserUnits(double k);	// 1 th == k DBU


	// Insert and remove a board object from view of the  LPSM.     
	int AddPcbThing(GL_Node *n);
	int DelPcbThing(GL_Node *n);
	
	//How many subshapes are there for this net?
   int GetNumSubShapes( int net );
	
	// These methods allow the client to access an GeomIter attached plane metal of a given net.
	// The client the can iterate of the boundaries of each subshape.
	GeomIter*	GetSubShapeIter( int net, int subshape_Id = -1);
	GeomIter*	GetSubShapesIter(GL_Node* ParentPlaneShape, int subshape_Id = -1); 
	GeomIter*	GetUpgObjects(GL_Node* Parent, int type);
	GL_Node*	GetParentPlaneShape(GeomIter *itr);
	GL_Node*	GetFirstPlaneShape(int xmin,int ymin,int xmax,int ymax,int net); 
	GL_Node*	GetNextPlaneShape();    // next equivalent input plane
	GL_Node*	GetParentThermal(GeomIter *itr, int id = 0); // for the tie legs iterator
	int			GetSubShapeId(GeomIter *itr);

	// Draw in given box.    
	int Draw(long xmin,long ymin,long xmax,long ymax);
	int DrawPS(cPlaneShapeOutlineDraw *drw, cPlaneShapeFillDraw *fill);
	
	//The client retrieves the presubnet connectivity information by using the functions
	// These methods to help the client with the calculation of subnets.
	// Pre-subnets are subnets calculated by client when ignoring connections to plane shapes.
	// These methods will allow the client to iterate through the presubnet numbers
	// of all board objects directly connected to a given subshape. 
	int GetFirstConnPreSubnet(int net,int subshape,int &presubnet);
	int GetNextConnPreSubnet(int &presubnet);
	int GetNextPair(int &presubnet, int &subshape);

	// The following two methods are to iterate through board objects
	// that are directly connected to a subshape of a given net.	
	GL_Node	*GetFirstConnected(int net);
	GL_Node	*GetNextConnected( );
	GL_Node* GetFirstConnected(GeomIter *subshape_iter, int &current_subshape);

	//calls to get connected objects (board objects) for a given subshape id. 
	int SetParentPlane(int net);		// to restrict the scaning by the given plane
    GL_Node *GetFirstObject(int shape_id);
    GL_Node *GetNextObject(int shape_id);
    GL_Node *GetNextConnObj(int &shape_id);

	//Iterate through subshapes connected to a given board object with the following two methods.
	int GetFirstConnectedSubshape(GL_Node* connThing);
	int GetNextConnectedSubshape();
	GeomIter *GetCrnSubshape(GeomIter *it);	// the current sub shape selected by GetFirstConnected & GetNextConnected()
	// new iterator will be created in case it == NULL. Otherwise it will be reinstalled to the next subshape
	int	GetVtxId();

	// Iterate though the plane_shape that intersects the box
	int GetFirstPlane( int xmin, int ymin, int xmax, int ymax, GL_Node* &plane_shape );
	int GetNextPlane( GL_Node* &plane_shape);
	
	// Iterate though the subshape that intersects the box
	GeomIter* GetFirstSubShape( int xmin, int ymin, int xmax, int ymax, int net = -1); //  -1 means all nets
	GeomIter* GetFirstInBox( int xmin, int ymin, int xmax, int ymax, int net = -1); //  -1 means all nets
	GeomIter* GetFirstSubShape( int xmin, int ymin, int xmax, int ymax, GL_Node *plane_shape );
	GeomIter* GetSubShapeBySeg( int xmin, int ymin, int xmax, int ymax, int net = -1); //  -1 means all nets
	GeomIter* GetSubShapeBySeg( int xmin, int ymin, int xmax, int ymax, GL_Node *plane_shape );
	GeomIter* GetNextSubShape(GeomIter *it); // new iterator will be created in case it == NULL
	GeomIter* GetNextInBox(GeomIter *it); // new iterator will be created in case it == NULL
	// otherwise it will be reinstalled to the next subshape

	GeomIter* GetNearestSubShape(int x, int y, int &xn, int &yn, int &rad, int &seg_id);
	// returns the iterator to the nearest polygon to the given point {x,y}
	// {xn, yn} - the nearest point on the polygon.
	// the searching is performed inside the circle {x, y,rad}

	// Iterate though the tie legs that intersects the box
	GeomIter* GetFirstTieLeg( int xmin, int ymin, int xmax, int ymax, int net = -1); //  -1 means all nets
	GeomIter* GetFirstTieLeg( int xmin, int ymin, int xmax, int ymax, GL_Node *plane_shape );
	GeomIter* GetTieLegBySeg( int xmin, int ymin, int xmax, int ymax, int net = -1); //  -1 means all nets
	GeomIter* GetTieLegBySeg( int xmin, int ymin, int xmax, int ymax, GL_Node *plane_shape );
	GeomIter* GetNextTieLeg(GeomIter *it); // new iterator will be created in case it == NULL
	//otherwise iterator "it" will be reused and set to the next object

	// attach client's node to the sub shape.
	// return 0 in case:
	// - the current subshape is not defined,
	// - or the current subshape is already attached to some application node
	// Note: don't redefine this method on the client's level
	int	AttachApplNode(GL_Node *appl_node);
	
	// the scanning will hide objects created before this point
	void SetCheckPoint();
	// remove check point & permits the scanning
	void DelCheckPoint();

	//Analyze an array of points
   int PointInside(PSM_Point *points, int num, int net, int size = -1);
   // methods for PES 
	GL_Node **GetUnusedNodes(int &num_nodes); // get unused scetches
	void Rebuild();

   // temporary
#if APP_DATA_MGR
	void	InitType(int type, int size);
	ConnObj	*NewConnObj(int type);
	void	DelConnObj(ConnObj *obj);
	int		IsLinkObjObj(ConnObj *src, ConnObj *trg);
	int		LinkObjObj(ConnObj *src, ConnObj *trg);
	int		RemoveLink(ConnObj *src, ConnObj *trg);
	void	RemoveLinks(ConnObj *src);
#endif
	GL_Box *GetEditBox();
	GL_Array *GetItersArray(GL_Box *box = NULL, int type = -1);
	void ArrToSelected(GL_Array *arr);
	void UnSelectArr(GL_Array *arr);
	void AllToSelected(int type);
	void UnSelectType(int type);
	void Dump(char *file_name);
	GeomHdr *GetFirstDBGeom();
	GeomHdr *GetNextDBGeom();
	static GeomIter* GetPolylineIter(GL_Node *poly);	// Node is the geometrical object

private:
	PShapes *lay_psm;
	int del_psm;
};

class SYSGL_API PSM_Point // : public GL_Point ???
{
public:
	PSM_Point();
	PSM_Point(long x, long y);
	int			GetAreaIndex();
	GL_Node*	GetUpItem();
	GL_Node*	GetDnItem();
	void		SetPoint(long x, long y);
   int      GetNetIndex();
public:
	long		x;
	long		y;
	GL_Node*	dn_poly;
	GL_Node*	up_poly;
	int			subshape;
}; 

#endif  // _LAYER_PSM_

