/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

// Name:				GL_GeomIter.h
// Date:				05.10.04
// Author:			Alexander Zabolotnikov	
// Description:	AF interface

#ifndef _AF_TABLE_
#define _AF_TABLE_

class PShapes;
class MetalVertex;
class AF_Cell;
class AF_Table;
const unsigned int FLT_MIN_CORN = 0x00000001;
const unsigned int FLT_MIN_CONN = 0x00000002;
const unsigned int FLT_MIN_AREA = 0x00000004;
const unsigned int FLT_CHK_ONE	= 0x00000008;
const unsigned int FLT_CHK_ALL = (FLT_MIN_CORN | FLT_MIN_CONN | FLT_MIN_AREA);

class AF_Filter
{
public:
	unsigned int flt_modes;
	int		min_crn;	// corners
	int		min_conn;	// connections
	double	min_area;	// area
	double	dbu_scale;	// dbu_area = area * dbu_scale * dbu_scale
	AF_Filter*	parent;
	AF_Table*	af;
public:
	AF_Filter();
	int InitFilter(GeomIter *itr);
	inline int	IsFilter();
	inline int	IsFilter(unsigned int flag);
	int FilterGeom(GeomHdr *&first, unsigned int chk_all = FLT_CHK_ALL);
};
inline int AF_Filter::IsFilter()
{
	if (flt_modes) return 1;
	else if (parent) return parent->IsFilter();
	return 0;
}
inline int AF_Filter::IsFilter(unsigned int flag)
{
	if (flag & flt_modes) return 1;
	else if (parent) return parent->IsFilter(flag);
	return 0;
}

/////////////////////////
class SYSGL_API AF_Table : public SGN_Controller
{
public:
	int				is_upd;
	int				is_upd_plane;
	int				is_one_parent;
	int				active_test;
	int				plane_ver;
	int				nx;			// the number of columns
	int				ny;			// the number of rows
	long			dx, dy;		// the size of the cell
	long			wb;			//the width of the boundary line
	GL_Box			TBox;		// the extend box of the table
	double			sep_w;		// the size of a sliver between cell
	double			obj_dist;	// default clearance between area and obstacle
	double			z_plane;	// height of the plane
	//stScan			sk_scan;	//	TBD
	AF_Cell			*cells;		// cells array
	LayerScanner	sdb_scan;
	AF_Table		*next_ps;
	AF_Table		*prev_ps;
	int				num_db_poly;
	int				num_ext_obj;
	int				pes_is_changed;
	int				remove_scallops;
	GeomHdr			s0_plane;	// loaded planes
	GeomHdr			s1_plane;	// active planes
	GeomHdr			s2_plane;	// plane shells
	GeomHdr			s2_obst;	// loaded obstacles
	GeomHdr			s1_obst;	// new obstacles
	GeomHdr			del_obst;	// deleted obstacles
	GeomHdr			s1_pes;		// new pes
	GeomHdr			s2_pes;		// loaded pes
	GeomHdr			pes_cut;	// subshape cut
	GeomHdr			cl_segs;	// cut lines for scallops
	GeomHdr			mg_shape;	// patches for MinGap
	GeomHdr			s1_cline;	// cut lines crossed box
	GeomHdr*		first_cbox;	// cell boxes for UpdAft
	GeomHdr*		startMerge;
	GeomHdr			sn_main;	// same net zones
	GeomHdr*		sn_loc;		// local zones
	GeomHdr*		poly_box;	// the polyline of the edited box
	GeomHdr*		plane_m;	// pseudo planes, generated in GenPBox
	GeomHdr*		second_shells;	// shells around pad in case tie leg width > hatch line width
	PShapes*		lpsm;
	PolySec*		plane_sec;
	PolySec*		dn_sec;
//privet:
	GeomIter		*plane_iter;
	IterPolyGeom	iter;		//
	GL_Box			InvBox;
	AF_Filter		af_filter;
	GL_Box			OBox;	// ext-box for obstacles only
	GL_Node			*app_sh_obj;
	static DFF		dc;
public:
	AF_Table();
	~AF_Table();
	int InitAFT(int num_x, int num_y, GL_Point *ll, GL_Point *ur);
	int AddAFT(GeomHdr *poly);
	int DelAFT(GeomHdr *poly, int del_poly = 0);
	int UpdAFT(GL_Box &box);
	void CreateBridges(GeomHdr *rez, GeomHdr *pbox);
// protected:
	////////////// Get ////////////
	inline AF_Cell *GetCell(int i, int j);
	int IsInvalid(GL_Box *box = NULL, int get_box = 0);
	int IsNumInvPoly(GL_Box *box, int get_all = 0);
	void GetIndex(GL_Point &pt, int &ix, int &iy);
	int GetActiveShp(GL_Box &box, GeomHdr **ovPoly);
	GeomHdr *GetActiveObst(GL_Box &box);
	GeomHdr *GetActiveCut(GL_Box &box);
	GeomHdr *GetActivePlanes(GL_Box &box, int &num_ovl);
	int IsActive(GeomHdr *poly);
	inline GL_Point GridPoint(int i, int j);

	////////////// Clear ////////////
	void ClearCell(int i, int j);
	void ClearCells();
	void ClearFA(GL_Box &box);
	////////////// Set //////////////
	void SetRezPoly(GeomHdr *poly);
	int SetStatus(GeomHdr *poly, int add);
	void SetValidStatus(GL_Box &box, int id);
	void CreateShell(GeomHdr *gh, long dist);
	void RemoveShell(GeomHdr *gh);
	void RemoveShells(GeomHdr *gh);
	/////////// utilities /////////////////////

	void AddToDB(GeomHdr *gh);
	void DelFromDB(GeomHdr *gh);
	void ClearDB();
	GeomHdr *GetFirstDBGeom();

	static void IncreaseBox(GL_Box &b, GL_Point &a);
	static GL_Node *GetNextAttObst(GL_Node *n);
	void RedrawFA(GL_Box &b,GL_Array &Array);
	IterPolyCross *GetIterArray(GL_Box &b, int &num, GL_Array &ItrArray);
	int DumpAFT(int to_file = -1, char *file_name = NULL);
	int DumpAFT2(char *file_name, char *mode);
	
	// SGN_Controller
	int	 CetControllerType() {return SGN_CONT_PSM;}
	void CorrectOnChange(GL_Node *n, GL_Node *prev);
	void CorrectOnDelete(GL_Node *n, GL_Node *prev);
	// PES support
	GeomHdr*	RemovePesCut(GeomHdr *gh,Poly2Pt *p0, MetalVertex *area, Poly2Pt *p2 = NULL); 
	GeomHdr*	CorrectByScetch(GeomHdr* rez);
	GeomHdr*	CreatePolyCells(int i1, int j1, int i2, int j2);
	void		ClearPolyCells(int i1, int j1, int i2, int j2);
	int			DelContour(GeomHdr *gh, int id);
	int			IsSortMonoList(GeomHdr *first = NULL, char *fileName = NULL, char *mode = NULL);
	int			IsGlobUpd(GL_Box &box);
	int			UpdateBox(GL_Box &box);
	GeomHdr*	GlueAreas(Poly2Pt *p0, int num_r1, int num_r2, GeomHdr *cpoly);
	GeomHdr*	ProcSketches(GL_Array *arr);
	int			CheckIsolated(Poly2Pt *pp0, Poly2Pt *ppl, short &mark);
	int			MoveSameNet(Poly2Pt *pp0, Poly2Pt *pp, GeomHdr *a, int is_contour);
	void		TestUpd(int nnx, int nny);
	GeomHdr*	GenPolylines(GeomHdr *bottom, int num_win, int &num, unsigned int &gen_thr);
	int			ChkMinGap(GeomHdr *ovPoly);
	int			MergePlanes();
	int			IsOneParent();
};
const unsigned int AFT_REZLT = 0x00000001;	// Dump rezulting polylines
const unsigned int AFT_NOBST = 0x00000002;	// Dump new obstacles
const unsigned int AFT_FOBST = 0x00000004;	// Dump old obstacles
const unsigned int AFT_CLBOX = 0x00000008;	// Dump cell boxes

inline GL_Point AF_Table::GridPoint(int i, int j)
{
	return GL_Point(TBox.ll.x + i*dx, TBox.ll.y + j*dy);
}

class AF_Cell : public SGN_Controller
{
public:
	int ix, iy;
	int is_valid;
	int num_poly;
	MetalVertex *mv;
	GeomHdr *PolyBox;
	AF_Table *af_parent;
public:
	void InitCell(AF_Table *af);
	int  CetControllerType() { return SGN_CONN_CELL; }
	void CorrectOnChange(GL_Node *n, GL_Node *prev);
	void CorrectOnDelete(GL_Node *n, GL_Node *prev);
	void AddVertex(MetalVertex *v);
	void DelVertex(MetalVertex *v);
	void ClearCell();
};

inline AF_Cell *AF_Table::GetCell(int i, int j)
{
	if (cells && 0 <= i && i < nx && 0 <= j && j < ny) {
		return cells + (i * ny + j);
	}
	return NULL;
}

extern bool UpdScanner(stScan *scan);
/////////////////////////////////////////////////////
///////// class HM_Hatch and auxiliary classes: /////
// 
#define PNT_PAGE_SIZE 15	// (15*2+2)*sizeof(long) = 64
class HM_YPnt
{
public:
	HM_YPnt *next_y;
	long y;
	short ovl;
	short dir;
};

class HM_XItem
{
public:
	HM_YPnt *first_y;
};

class HM_Hatch
{
public:
	long	hm_dx;	// hatch step
	GL_Box		hm_box;	// extended box, rounded by hm_dx
	glPageMem ymem;		// memory for HM_YItem
	GL_Array xmem;		// memory for HM_XItem
	//GL_Array qmem;		// memory for HM_QItem

	void AddHatch(stHatch &hhh);
	void DelHatch(stHatch &hhh);
};

////////////// iterators //////////////////////
class IterX
{
public:
	HM_XItem *xf, *xl, *xc;	// first, last, current
	long x0, dx;
	HM_XItem *InitIterX(HM_Hatch &hm);  /* returns 0 if hm is empty */
	inline HM_XItem *GetFirstX();  /**/
	inline HM_XItem *GetLastX();  /* */
	inline HM_XItem *GetNextX(); /**/
	inline long GetX();
	inline HM_XItem *GoX(long x);
};

inline HM_XItem *IterX::GetFirstX()
{
	xc = xf;
	return xc;
}
inline HM_XItem *IterX::GetLastX()
{
	xc = xl-1;
	if (xc < xf) {
		xc = xf;
		return NULL;
	}
	return xc;
}
inline HM_XItem *IterX::GetNextX()
{
	xc += 1;
	if (xc < xl) {
		return xc;
	}
	xc -= 1;
	return NULL;
}

inline HM_XItem *IterX::GoX(long x)
{
	x -= x0;
	if (x < 0) return NULL;
	xc = xf + (int)(x / dx);
	if (xc < xl) {
		return xc;
	}
	xc = xf;
	return NULL;
}
inline long IterX::GetX()
{
	return x0 + dx * (int)(xc - xf);
}

class IterY
{
public:
	HM_XItem *crn_x;
	HM_YPnt *crn_y;
	HM_YPnt *InitIterY(IterX &xi);
	inline HM_YPnt *GetFirstY();
	inline HM_YPnt *GetLastY();
	inline HM_YPnt *GetNextY();
	inline HM_YPnt *GoY(long Y);
};
inline HM_YPnt *IterY::GetFirstY()
{
	if (crn_x) {
		crn_y = crn_x->first_y;
	} else {
		crn_y = NULL;
	}
	return crn_y;
}
inline HM_YPnt *IterY::GetLastY()
{
	while (crn_y && crn_y->next_y) crn_y = crn_y->next_y;
	return crn_y;
}
inline HM_YPnt *IterY::GetNextY()
{
	crn_y = crn_y->next_y;
	return crn_y;
}
inline HM_YPnt *IterY::GoY(long y)
{
	if (crn_y->y > y) crn_y = crn_x->first_y;
	for(;crn_y && crn_y->y < y; crn_y = crn_y->next_y);
	if (!crn_y) {
		return GetFirstY();
	}
	return crn_y;
}



#if 0 // TODO; HM_YItem is not used yet
class HM_YItem
{
public:
	HM_YItem *rt_y;
	HM_YItem *up_y;
	HM_YItem *dn_y;
	int num_y;
	short rt_id;
	short balance;
	HM_YPnt yy[PNT_PAGE_SIZE];
};
#endif

/////////////////// AF_TEST ////////////////////

#define MAX_SL_ITERS 256

class SYSGL_API af_test
{
public:
   af_test();
	~af_test();
	void	Clear();
	void	OnTerminate();
	int	Initiate(GeomHdr *dbpoly = NULL);
	int	Down(GL_Point &ev_pt);
	int	MouseMove(GL_Point &ev_pt);
	void	TestClip(GL_Box &TBox);
	IterPolyCross *GetIterArray(GL_Box &b, int &num) { return aft.GetIterArray(b, num, ItrArray);}
	IterPolyCross *GetDBIterArray(GL_Box &b, int &num);
	virtual int GetDBUScale();
	virtual void GetVPort(GL_Box &b);
	virtual void RedrawDB(GL_Box *b = NULL);
	virtual void RedrawAF(GL_Box *b = NULL);
	virtual void RedrawIters(IterPolyCross *itr, int num);
	//data:
	//PCB_Database	*db;
	stScan			afdb_scan;	// db scann
	stScan			*nea_scan;
	GeomHdr		*nea_poly;
	XEvent				*nea_xi;
	stSlider		*sl[MAX_SL_ITERS];
	int num_sl;
	
	int idx;
	GL_Point	cen, pt2, old_pt;
	long	c_rad;
	int IsVsg;

	GeomHdr	*dn_poly, *up_poly;
	GL_Array ItrArray;
	GL_Box VBox;
	int active_dn, is_dn;
	static AF_Table aft;
};

class AF_Status
{
public:
	int num_db_poly;
	int num_db_hole;
	int num_db_pnt;
	int num_db_arcs;
	int num_obst;
	int num_same_net;
	int num_tie_leg;
	int num_thr_sec;
	void SetStatus(AF_Table &af);
	int EquStatus(AF_Status &s2, AF_Status &ds);
};

inline int GetAfId(AF_Table *af)
{
	int af_id = 0;
	for (; af; af = af->prev_ps) af_id++;
	return af_id;
}

#endif	// _AF_TABLE_

