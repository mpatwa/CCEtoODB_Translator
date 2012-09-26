//////////////////////////////////////////
// Name:				GL_GeomSup.h
// Date:				08/20/03
// Author:			Alexander Zabolotnikov	
// Description:	Geometry supporting methods
//////////////////////////////////////////
#ifndef __GEOM_SUP_H__
#define __GEOM_SUP_H__

#include "GL_GeomIter.h"
#include "GL_PageMem.h"
#include "../ST/ST_temp.h"
///////////////////////////////////////////
class GL_Node;
class SGN_Controller;
class ConnMap;
class ConnObjRef;
class ConnRef;
class ConnObj;
class GeomHdr;
///////////////////////////////////////////
// ------- 0 ------------- 
const int FreeObjType	= 0;
const int SrcObjType	= 1; //0x00000001 ?
// ------- 2 ------------- 
const int GeomGeomType	= 2;	 // GL_GEOM_CHK_TYPE?
#define MAX_Geom_TYPES 3

//~~~~~~~~~~~~~~~~~~~ ConnObj ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// joint block for Geom objects, it includes 
//		- ConnObjRef - relation to geometry & database 
//		- first_ref - relations to other Geom objects
class SYSGL_API ConnObj : public GL_Node
{
public:
	int		chk_id;
	ConnRef *first_ref; // relations to other Geom objects
	ConnObj *next_obj;
	ConnObj *prev_obj;
public:
	ConnObj();
	~ConnObj();
	inline ConnRef	*GetFirstRef();
	inline int	GetLabel();
	void		MoveAfter(ConnObj *prev);
	inline void	SetAfter(ConnObj *prev);
	ConnRef		*GetRefByType(int Geom_type);
	ConnObj		*GetObjByType(int Geom_type);
	GeomHdr		*GetGeom();	// 
	int			SetGeom(GeomHdr *poly);
	int			InsRef(GeomHdr *poly);
	ConnObj		*GetGeomParent();	//

	////////////// Delete /////////
	GL_Node *DelGeom();
	int		CheckForDel(ConnObj *obj);

	////////////// Debug /////////
	void Draw();
	int CheckConnObj();
};
inline ConnRef *ConnObj::GetFirstRef()
{	return first_ref;}

inline int ConnObj::GetLabel()
{	return (int)GetNodeFlags();}
inline void	ConnObj::SetAfter(ConnObj *prev)
{
	if (prev) {
		next_obj = prev->next_obj;
		prev_obj = prev;
		if (next_obj) next_obj->prev_obj = this;
		prev->next_obj = this;
	}
}


//~~~~~~~~~~~~~~~~~~~ ConnRef ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ConnRef is used to connect Geom objects.
// Two adjacent exemplars of ConnRef permit to organize arbitrary graph.

class SYSGL_API ConnRef
{
public:
	ConnObj	*GetTrgObj();		// get target object
	ConnObj	*GetSrcObj();		// get source object
	ConnRef	*GetNextItem();		// get next item
	ConnRef	*GetPrevItem();		// get previous item
	ConnRef	*GetConRel();		// get connected item from the opposite list
	ConnRef	*GetNextByType(int Geom_type);
	ConnRef	*GetPrevByType(int Geom_type);
private:
	void IncludeNextRef(ConnRef *next);

public:
	ConnRef	*next_itm;
	ConnRef	*prev_itm;	// DEBUG
	ConnObj	*trg_obj;
};

// Get direction to the fellow reference
// GeomDB's memory manager guarantees: the pointer to the first item in the pair is even number.
// This fact is used to determine fellow item.
inline int GetConDir(ConnRef *item)
{
	ConnRef *ori = NULL;
	//if (((long)(item - GeomDb::ori_ref) % 2) {}
	if (((long)(item - ori)) % 2) {
		return -1;
	} else {
		return 1;
	}
}

#define REF_PAGE_SIZE 1024
class ConnRefPage
{
public:
	ConnRefPage();
	void InitRefPage();
public:
	ConnRef RefItemArr[REF_PAGE_SIZE];
	ConnRefPage *next_ref_page;
	ConnRef *item0;
	int max_ref;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global Geom. parameters & methods
#define MAX_CONN_TYPE 256
class ConnObjList : public SGN_Controller
{
public:
    glPageMem	*ConnObjArr;
	int			obj_size;
	int			obj_type;
	//int			usr_size;
	LayerScanner lscan;
	ConnObj		Obj0;
	ConnObj		old_obj;
	ConnObj		*old_first;
public:
	void Init0();
	void InitPM(int size, int type, int page_size = -1);
	ConnObj *NewConnObj();
	void DelConnObj(ConnObj *obj, ConnMap *cm);
	void ClearPM(ConnMap *cm);
	void MoveInList(ConnObj *obj, ConnObj *prev = NULL);
	void MoveToOld();
	void ReturnOld();
};


class SYSGL_API ConnMap : public SGN_Controller
{
public:
	ConnMap();
	~ConnMap();

	ConnRefPage *first_ref_page;
	ConnRef		*free_ref;
	int			(*CmpConnObj)(ConnObj *n1, ConnObj *n2);
	ConnObjList COL[MAX_CONN_TYPE];

	int		chk_pnt_id;
	int		is_chk_pnt;
public:
	void	InitConnMap(int size);
	void	InitType(int type, int size);
	int		IsLinkObjObj(ConnObj *src, ConnObj *trg);
	int		LinkObjObj(ConnObj *src, ConnObj *trg);
	int		RemoveLink(ConnObj *src, ConnObj *trg);
	void	RemoveLinks(ConnObj *src);
	void	DelLink(ConnRef *ref);
	void	UnlinkRef(ConnRef *ref);
	void	SortRefList(ConnObj *obj);
	void	SetCmpFunc(int (*cmp_fun)(ConnObj *n1, ConnObj *n2));
	int		LinkObjRef(ConnObj *src, ConnRef *ref);
	ConnRef *MakeLinkObjObj(ConnObj *src, ConnObj *trg);
	ConnRef	*NewRef();
	void	DelRef(ConnRef *ref);
	ConnRef	*GetConRef(ConnRef *nea_ref);
	ConnObj	*NewConnObj(int type);
	void	DelConnObj(ConnObj *obj);
	int		IsItemInMem(ConnObj *obj);
	void	DelAllGeom();
	void	CheckRefList(ConnObj *obj);
	void	*GetUserData(ConnObj *obj);
	int		GetNumObjects(int type);
	int		CheckConnObjects(int type = -1);
	int		SetCheckPoint();
	int		DelCheckPoint();
	int		EnableCheckPoint();
	int		DisableCheckPoint();  // use to be DelCheckPoint
};

inline void	ConnMap::SetCmpFunc(int (*cmp_fun)(ConnObj *n1, ConnObj *n2))
{
	CmpConnObj = cmp_fun;
}


#if 0 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

///////////////////////////////////////////
///////////////////////////////////////////
#define Geom_NAME_LEN 16
class ObjectDef
{
public:
	int	Geom_type;
	int	size_of;						// for memory manager
	int	ord;							// for Delete
	int	need_ref;					// for Delete
	int	num_obj;						// debug info
	char	TypeName[Geom_NAME_LEN];	// debug info
public:
	void InitGeomDef(int type, int size, int or, int nref, char *name = NULL);
	void Dump (FILE *ff);
};
class GeomDB
{
public:
	static ObjectDef	GeomDef[MAX_Geom_TYPES];
	static glLow	lc;
	static int			Geom_precision;
	static double		scale;
	static double		move_eps;
	static GL_Point	sel_beg;
	static GL_Point	sel_end;
	static GL_Point	sel_cen;
	static GL_Coord	sel_rad;
	static int			sel_idx;
	static double		sel_val;
	static double		sel_sval;	// snap value
	static int			sel_sidx;	// snap idx
	static GL_Point	sel_spnt;	// snap point
	static GeomHdr *sel_poly;

	static GL_Flags	gen_mode;
	static GL_Flags	snap_mode;
	static GL_Flags	cmd_mode;
	static GL_Point	base_ori;
	static GL_Point	last_ori;
	static GL_DPoint vect;
	static double		base_ang;
	static double		tmp_len;
	// debug
	static int			obj_max_size;
	static int			Geom_tmp_clear;
	
	static int			GetObjMaxSize();
	static int			GetGeomParam();

public:
	static void			Init (void);
	static void			InitGeomDef();
	static void			SetAngle(double &ff, double &cs, double &sn);
	static void			SetCS(double cs, double sn);
//private:
	static void			EditConnObj(ConnObj *obj, GL_Flags gen_mode = GeomRegMode);
	static int			CheckEditObj(ConnObj *e, ConnObj *n, GL_Flags gen_mode);
	static double		GetPolySeg(GL_Point	*p, GeomHdr *h, GL_Coord e, GL_Point &f, GL_Point	&s, GL_Point &c, GL_Coord	&r);
	static int			GetPolyCrnDir(int i, GeomHdr	*g, GL_Point	&crn, GL_DPoint &f1,	GL_DPoint &f2);
	static int			PtInSector(const GL_Point *p, const GL_Point *s, const GL_Point *e, const GL_Point *c);
	static int			GetPtOnPoly(double t, GeomHdr	*g, GL_Point	&s, GL_Point &e, GL_Point	&c, GL_Coord &r, GL_Point &p);
	static void			GetPtOnSeg(double t, GL_Point &s, GL_Point &e, GL_Point &c, GL_Coord &r, GL_Point	&p);
	static double		GetNativeVal(GL_Point *p, GL_Point *f, GL_Point *s, GL_Point *c, GL_Coord	r);
	static ConnObj		*GetConnObjPoly(GeomHdr *poly);
	static char			*DoubleToStr(double v, int num);
	static void			Redraw();
	static int			UpdGeomByPoly(GeomHdr *poly);
	static int			UpdGeomByLayer(GL_Layer lyr);
	static GL_Node *ChkGeomNode(GL_Node *node);
};


//~~~~~~~~~~~~~~~~~~~ ConnObjRef ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ConnObjRef is used as a part of ConnObj. It permits to connect
// ConnObj with geometry and with database object, ConnObjRef is
// inserted in the GL_Nodes loop (GL_Node->sgn_root->sgn_root->,,,)
// This loop is used in database in order to connect database object with geometry.
// So we can get db object by geometry and we can get geometry by db object.
// Also this loop permit to attach arbitrary data with geometry.
// 
class	ConnObjRef: public GL_Node
{
public:
	ConnObjRef();
	~ConnObjRef();
	void			InitConnObjRef(ConnObj *parent = NULL);
	void			ClearConnObjRef();

public:
};

#endif	// if 0
#endif // __GEOM_SUP_H__

