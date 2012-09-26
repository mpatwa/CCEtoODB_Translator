
#if !defined(__UnidatIn_h__)
#define __UnidatIn_h__

#define UNIDAT_VERSION_1_2		1
#define UNIDAT_VERSION_2_1		2

typedef  struct
{
   char  *token;
   int   (*function)();
} List;

typedef struct
{
   char  *key;
   char  *val;
}UNIDATAtt;

//-----------------------------------------------------------------------
// UNIDATDrill Structures
//-----------------------------------------------------------------------
class UNIDATDrill
{
public:
   double d;   
   int    toolindex;
};
class CDrillArray : public CTypedPtrArrayContainer<UNIDATDrill*>
{
};

//-----------------------------------------------------------------------
// UNIDATAttrmap Structures
//-----------------------------------------------------------------------
class UNIDATAttrmap
{
public:
   CString  unidat_name;
   CString  cc_name;
};
class CAttrmapArray : public CTypedPtrArrayContainer<UNIDATAttrmap*>
{
};

//-----------------------------------------------------------------------
// UNIDATCompPin Structures
//-----------------------------------------------------------------------
class UNIDATCompPin
{
public:
	UNIDATCompPin(CompPinStruct *cp = NULL)
	{
		comppin = cp;
		x = y = 0.0;
		toprot = botrot = 0.0;
	};

   CompPinStruct  *comppin;      // struct from add_comppin
   double         x, y;
   CString        padtop, padbot;

private:
   double         toprot, botrot;

public:
	CompPinStruct *GetCompPin() const				{ return comppin; };
	CPoint2d GetPinLocation() const					{ return CPoint2d(x, y); };
	CString GetTopPadName() const						{ return padtop; };
	CString GetBottomPadName() const					{ return padbot; };
	double GetTopPadRotation_InDegs() const		{ return RadToDeg(toprot); };
	double GetTopPadRotation_InRads() const		{ return toprot; };
	double GetBottomPadRotation_InDegs() const	{ return RadToDeg(botrot); };
	double GetBottomPadRotation_InRads() const	{ return botrot; };

	void SetTopPadRotation_InDegs(double rot)			{ toprot = DegToRad(rot); };
	void SetTopPadRotation_InRads(double rot)			{ toprot = rot; };
	void SetBottomPadRotation_InDegs(double rot) 	{ botrot = DegToRad(rot); };
	void SetBottomPadRotation_InRads(double rot) 	{ botrot = rot; };
};
class CCompPinArray : public CTypedPtrArrayContainer<UNIDATCompPin*>
{
};

//-----------------------------------------------------------------------
// UNIDATTestProbe Structures
//-----------------------------------------------------------------------
class UNIDATTestProbe
{
public:
	UNIDATTestProbe(int pcbfilenum, CString refname, CString probetype, CString targettype, CString netname, double x, double y, CString surfTop, CString surfBot);

   int m_pcbFileNum; // pcb that this probe goes on
   CString m_refname;
   CString m_probeType;
   CString m_targetType;
   CString m_netName;
   CString m_surfaceTop;
   CString m_surfaceBot;
   double m_x;
   double m_y;

public:
	CPoint2d GetLocation() const					{ return CPoint2d(m_x, m_y); };


};
class CTestProbeArray : public CTypedPtrArrayContainer<UNIDATTestProbe*>
{
};

//-----------------------------------------------------------------------
// UNIDATToe Structures
//-----------------------------------------------------------------------
class UNIDATToe
{
public:
	UNIDATToe()
	{
		m_toprot = m_botrot = m_drillrot = m_drillDiam = 0.0;
	};

private:
   CString     m_number;
	CString		m_name;
   CPoint2d		m_location;		//relative location
   CString		m_padtop, m_padbot, m_drill;
   double		m_toprot, m_botrot, m_drillrot;
   double		m_drillDiam;

public:
   CString GetNumberStr() const                 { return m_number; }
   int   GetNumber() const                      { return atoi(m_number); }
	CString GetName() const								{ return m_name; };
	CPoint2d GetPinLocation() const					{ return m_location; };
	CString GetTopPadName() const						{ return m_padtop; };
	CString GetBottomPadName() const					{ return m_padbot; };
	CString GetDrillName() const						{ return m_drill; };
	double GetTopPadRotation_InDegs() const		{ return RadToDeg(m_toprot); };
	double GetTopPadRotation_InRads() const		{ return m_toprot; };
	double GetBottomPadRotation_InDegs() const	{ return RadToDeg(m_botrot); };
	double GetBottomPadRotation_InRads() const	{ return m_botrot; };
	double GetDrillRotation_InDegs() const			{ return RadToDeg(m_drillrot); };
	double GetDrillRotation_InRads() const			{ return m_drillrot; };
	double GetDrillDiameter(double rot) const		{ return m_drillDiam; };

   void SetNumber(CString number)                  { m_number = number; }
   void SetNumber(int number)                      { CString str; str.Format("%d", number); m_number = str; }
   void SetName(const CString name)                { m_name = name; };
	void SetTopPadName(CString name)						{ m_padtop = name; };
	void SetBottomPadName(CString name)					{ m_padbot = name; };
	void SetDrillName(CString name)						{ m_drill = name; };
	void SetLocation(double x, double y)				{ m_location.x = x;  m_location.y = y; };
	void SetTopPadRotation_InDegs(double rot)			{ m_toprot = DegToRad(rot); };
	void SetTopPadRotation_InRads(double rot)			{ m_toprot = rot; };
	void SetBottomPadRotation_InDegs(double rot) 	{ m_botrot = DegToRad(rot); };
	void SetBottomPadRotation_InRads(double rot) 	{ m_botrot = rot; };
	void SetDrillRotation_InDegs(double rot)			{ m_drillrot = DegToRad(rot); };
	void SetDrillRotation_InRads(double rot)			{ m_drillrot = rot; };
	void SetDrillDiameter(double diameter)				{ m_drillDiam = diameter; };
};
class CToeArray : public CTypedPtrArrayContainer<UNIDATToe*>
{
};

//-----------------------------------------------------------------------
// UNIDATFootPrint Structures
//-----------------------------------------------------------------------
class UNIDATFootPrint
{
public:
	UNIDATFootPrint(CString name = "")
	{
		m_name = name;
		m_used = false;
	}

private:
	CString		m_name;
	CToeArray	m_toes;
	bool			m_used;

public:
	CString GetName() const								{ return m_name; };
	bool IsUsed() const									{ return m_used; };

	void SetUsed(bool used = true)					{ m_used = used; };

	int GetToeCount() const								{ return m_toes.GetCount(); };
	UNIDATToe *GetToeAt(int index) const			{ return m_toes.GetAt(index); };

	int LoadToes();
};
class CFootPrintMap : public CTypedPtrMap<CMapStringToPtr, CString, UNIDATFootPrint*>
{
public:
	void Empty()
	{
		POSITION pos = GetStartPosition();
		while (pos)
		{
			CString key;
			UNIDATFootPrint *package = NULL;
			GetNextAssoc(pos, key, package);

			delete package;
		}
		RemoveAll();
	};
};

//-----------------------------------------------------------------------
// UNIDATPackage Structures
//-----------------------------------------------------------------------
class UNIDATPackage
{
public:
	UNIDATPackage(CString name = "")
	{
		m_name = name;
	};

private:
	CString		m_name;
   int			m_geom;

public:
	CString GetName() const								{ return m_name; };
	int GetGeometryNumber() const						{ return m_geom; };

	void SetName(CString name)							{ m_name = name; };
	void SetGeometryNumber(int num)					{ m_geom = num; };
};
class CPackageMap : public CTypedPtrMap<CMapStringToPtr, CString, UNIDATPackage*>
{
public:
	void Empty()
	{
		POSITION pos = GetStartPosition();
		while (pos)
		{
			CString key;
			UNIDATPackage *package = NULL;
			GetNextAssoc(pos, key, package);

			delete package;
		}
		RemoveAll();
	};
};

//-----------------------------------------------------------------------
// UNIDATComp Structures
//-----------------------------------------------------------------------
class UNIDATComp
{
private:
	CToeArray		m_pins;
   CString        m_refname;
   CString        m_geom;				// shape/package
	CString			m_footprint;
   int            m_mirror;
   int            m_rotation;      // degree
   DataStruct    *m_data;
   int            m_filenumber;

public:
   UNIDATComp(int filenum, CString refname, CString geom, CString footprint, int mir, int rot) {m_filenumber = filenum; m_data = NULL; m_refname = refname; m_geom = geom; m_footprint = footprint; m_mirror = mir; m_rotation = rot; }

   CString GetRefname()       { return m_refname; }
   CString GetGeom()          { return m_geom; }
   CString GetFootprint()     { return m_footprint; }
   int GetMirror()            { return m_mirror; }
   int GetRotation()          { return m_rotation; }
   DataStruct* GetData()      { return m_data; }
   int GetFileNumber()        { return m_filenumber; }

   void SetData(DataStruct *d)   { m_data = d; }

	int LoadPinList();
	int GetPinCount() const								{ return m_pins.GetCount(); };
	UNIDATToe *GetPinAt(int index) const			{ return m_pins.GetAt(index); };
};
class CCompArray : public CTypedPtrArrayContainer<UNIDATComp*>
{
public:
   UNIDATComp *Find(CString refname);
};

class CPolyArray : public CTypedPtrArrayContainer<Point2*>
{
};

//---------------------------------------------------------
class CUnidatPCBLocation
{
private:
   int m_panelfilenum;
   CString m_pcbname;  // the full name of the pcb as found in panel section in cad data, may look like a file path
   CString m_refname;
   double m_x;
   double m_y;
   double m_r;  // degrees

public:
   CUnidatPCBLocation(int panelfilenum, CString pcbgeomname, CString refname, double x, double y, double rot) 
          { m_panelfilenum = panelfilenum; m_pcbname = pcbgeomname; m_refname = refname; m_x = x; m_y = y; m_r = rot; }

   int GetPanelFileNum()      { return m_panelfilenum; }
   CString GetPcbName()       { return m_pcbname; }  // the raw full name as found in cad data, may look like file path
   CString GetPcbGeomName();                         // the geom name as will be used in camcad, leaf of path
   CString GetRefname()       { return m_refname; }
   double GetX()              { return m_x; }
   double GetY()              { return m_y; }
   double GetRot()            { return m_r; }

   void MakePCBInsert(CCEtoODBDoc *doc);
};

class CUnidatPCBList : public CTypedPtrArrayContainer<CUnidatPCBLocation*>
{
public:
   void MakePCBInserts(CCEtoODBDoc *doc);
};
//---------------------------------------------------------

static int fskip(), fnull();
static int unidat_info();
static int unidat_outline();
static int unidat_fiducials();
static int unidat_component();
static int unidat_other_drillings();
static int unidat_pad();
static int unidat_shape();
static int unidat_via();
static int unidat_track();
static int unidat_component_pin();
static int unidat_skipsection();
static int make_localcomps(FileStruct *pcbfile);
static int do_assign_layers();
static int update_padstackddrill(FileStruct *pcbfile);
static void setFileReadProgress(const CString& status);
static int unidat_pad_v21();

static List start_lst_v12[] =
{
   {"%%%INFO",             unidat_info}, 
   {"%%OUTLINE",           unidat_outline},  
   {"%%FIDUCIALS",         unidat_fiducials},  
   {"%%COMPONENT_PIN",     unidat_component_pin},  
   {"%%COMPONENT",         unidat_component},  
   {"%%OTHER_DRILLINGS",   unidat_other_drillings},  
   {"%%PAD",               unidat_pad_v21},  
   {"%%SHAPE",             unidat_shape},  
   {"%%VIA",               unidat_via},  
   {"%%TRACK",             unidat_track},  
   {"%%SUMMARY",           unidat_skipsection},  
   {"%%SUBCOMPONENT",      unidat_skipsection},  
   {"%%TESTPAD",           unidat_skipsection},  
};
#define  SIZ_START_LST_V12     (sizeof(start_lst_v12) / sizeof(List))


static int unidat_component_pin_v21();
static int unidat_footprints_v21();
static int unidat_packages_v21();
static int unidat_component_v21();
//static int unidat_pad_v21();
static int unidat_testneadles_v21();
static int unidat_via_v21();
static int unidat_testpoints_v21();
static int unidat_pcb();
static int unidat_panel();
static int unidat_panel_pcb_inserts();

static List start_lst_v21[] =
{
   {"%%%INFO",	               unidat_info},
   {"%%%PCBOARD",             unidat_pcb},
   {"%%%PANEL",               unidat_panel},               // notice the subtle difference between this
   {"%PANEL",                 unidat_panel_pcb_inserts},   // and this
   {"%%OUTLINES",	            unidat_outline},
   {"%%FIDUCIALS",            unidat_fiducials},
   {"%%COMPONENT_PINS",       unidat_component_pin_v21},
   {"%%FOOTPRINTS",           unidat_footprints_v21},
   {"%%PACKAGES",	            unidat_packages_v21},
   {"%%COMPONENTS",           unidat_component_v21},
   {"%%OTHER_DRILLINGS",      unidat_other_drillings},
	{"%%PADS",                 unidat_pad_v21},
   {"%%SHAPE",                unidat_shape},
   {"%%VIAS",                 unidat_via_v21},
   {"%%TEST_NEADLES",         unidat_testneadles_v21},  // neAdles is "correct" match to format spec
   {"%%TEST_NEEDLES",         unidat_testneadles_v21},  // some cad writer someplace thought they would "fix" the spelling of neadles
   {"%%TEST_POINTS",          unidat_testpoints_v21},
   {"%%TRACKS",               unidat_track},
   {"%%BOND_WEBS",            unidat_skipsection},
   {"%%COMPONENT_TOEPRINTS",  unidat_skipsection},
   {"%%CONDUCTIONS",          unidat_skipsection},
   {"%%DRAWINGS",             unidat_skipsection},
   {"%%SUMMARY",              unidat_skipsection},
   {"%%SUBCOMPONENT",         unidat_skipsection},
   {"%%TESTPAD",              unidat_skipsection},
};
#define  SIZ_START_LST_V21     (sizeof(start_lst_v21) / sizeof(List))

static int unidat_create_layer(CString layerName, CString prefix, BOOL floating);



#endif
