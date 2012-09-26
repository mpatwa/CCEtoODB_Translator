// $Header: /CAMCAD/4.5/read_wrt/Takaya_o.h 48    2/14/07 8:09p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once
#include "afxwin.h"
#include "CamCadDatabase.h"

#define  MAX_BOARDS  500

#define PIN_MAP_POSITIVE				"Positive"
#define PIN_MAP_NEGATIVE				"Negative"
#define PIN_MAP_ANODE					"Anode"
#define PIN_MAP_CATHODE					"Cathode"
#define PIN_MAP_DRAIN					"Drain"
#define PIN_MAP_GATE						"Gate"
#define PIN_MAP_SOURCE					"Source"
#define PIN_MAP_COLLECTOR				"Collector"
#define PIN_MAP_BASE						"Base"
#define PIN_MAP_EMITTER					"Emitter"

#define TK_REF_REPLACEMENT				"%ref"	// refdes
#define TK_PTOL_REPLACEMENT			"%ptol"	// positive tolerance
#define TK_NTOL_REPLACEMENT			"%ntol"	// negative tolerance
#define TK_PART_REPLACEMENT         "%part"  // part

#define TK_ATT_GENPARTCODE				"PARTNUMBER"

#define TK_SHORTS_ANALYSIS				"ANALYSIS"
#define TK_SHORTS_PIN_TO_PIN			"PIN_TO_PIN"

#define TK_DEFAULT_OUTLINE_SHRINK_DISTANCE	1	// Default value is in millimeters

enum TakayaOriginMode
{
   cczCadOrigin,
   relativeToReferencePoint
};


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CAptModelTest
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CAptTest
{
public:
	CAptTest() { m_cElement = '\0';};
	~CAptTest() {};

private:
   int m_sDeviceType;
	CString m_sRefdes;
	CString m_sValue;
	CString m_sComment;
	CString m_sLocation;
	CString m_sOption;
	char m_cElement;
	CString m_sPinNum1;
	CString m_sPinNum2;
   CString m_sPinNum3;
   CString m_sPinNum4;

public:
   void SetDeviceType(const int deviceType)  { m_sDeviceType = deviceType; }
	void SetRefdes(const CString refdes)		{ m_sRefdes = refdes;		}
	void SetValue(const CString value)			{ m_sValue = value;			}
	void SetCommment(const CString comment)	{ m_sComment = comment;		}
	void SetLocation(const CString location)	{ m_sLocation = location;	}
	void SetOption(const CString option)		{ m_sOption = option;		}
	void SetElement(const char element)			{ m_cElement = element;		}
	void SetPinNum1(const CString pinNum1)		{ m_sPinNum1 = pinNum1;		}
	void SetPinNum2(const CString pinNum2)		{ m_sPinNum2 = pinNum2;		}
	void SetPinNum3(const CString pinNum3)		{ m_sPinNum3 = pinNum3;		}
	void SetPinNum4(const CString pinNum4)		{ m_sPinNum4 = pinNum4;		}

   int GetDeviceType() const                 { return m_sDeviceType;    }
	CString GetRefdes() const						{ return m_sRefdes;			}
	CString GetValue() const						{ return m_sValue;			}
	CString GetComment() const						{ return m_sComment;			}
	CString GetLocation() const					{ return m_sLocation;		}
	CString GetOption() const						{ return m_sOption;			}
	char GetElement() const							{ return m_cElement;			}
	CString GetPinNum1() const						{ return m_sPinNum1;			}
	CString GetPinNum2() const						{ return m_sPinNum2;			}
	CString GetPinNum3() const						{ return m_sPinNum3;			}
	CString GetPinNum4() const						{ return m_sPinNum4;			}
};


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CAptModel
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CAptModel : public CTypedPtrListContainer<CAptTest*>
{
public:
	CAptModel(CString name)							
	{ 
		m_sName = name;
	};
	~CAptModel() {};

private:
	CString m_sName;

public:
	CString GetName() const							{ return m_sName;				}
};


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CVariantList
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CAptModelMap : public CTypedMapStringToPtrContainer<CAptModel*>
{
public:
	CAptModelMap() {};
	~CAptModelMap() {};
};


/* TypeStruct *****************************************************************************/
typedef struct
{
   char  *name;
}TKdevices;


//_____________________________________________________________________________
class TKComp
{
private:
   int m_index;

public:
   CString  name;          // list of components in the datalist
   CString  takayarefname;
   CString  value;
   CString  voltage;       // for zener
   CString  device;
   int      takayadevicetype; 
   int      takayadevicesubclass;   
   int      smdflag;       // if ATT_SMD was set
   int      inserttype;    // np stored inserttype;
   int      bottom;        // placed on bottom;
   int      pincnt;
   CString  positiveTol;
   CString  negativeTol;
   CString  partNumber;
   CString  GridLoc;
	CString  mergedStatus;
	CString  mergedValue;
	CString  aptModelName;
   CString  testStrategy;
	bool		not_implemented_takayaclass;
	bool		writeSelectVision;
	bool		isICOpenTest;
	InsertStruct* compInsert;
	CString	subClass;
   CString  IcOpenDescription;
   CString  comment;

public:
   TKComp(int index);

   int getIndex() const { return m_index; }
};


//_____________________________________________________________________________
class TKCompArray : public CTypedPtrArrayWithMapContainer<TKComp>
{
public:
   TKCompArray(int size);

   TKComp* getDefined(const CString& name);
};


//_____________________________________________________________________________
typedef CTypedMapStringToPtrContainer<CPoint2d*> CPinLocationMap;
class ICOpenTestGeom
{
public:
	ICOpenTestGeom(CCamCadData& camCadData, BlockStruct* compGeomBlock, BlockStruct *realPartPkgBlock, double sensorDiameter);
	~ICOpenTestGeom();

private:
	CString m_name;
   double m_sensorDiameter; // aka shrink distance, in page units
   bool m_shrinkOkay;  // true if shrink operation processing sensorDiameter succeeded, otherwise false.
	CPinLocationMap m_pinLocationMap;

	void createPinLocationMap(CCamCadData& camCadData, BlockStruct* compGeomBlock, BlockStruct* realPartPkgBlock);

public:
	CString GetName() const { return m_name; }
	CPinLocationMap* CreatePinLocationMapForInsert(InsertStruct* insert, int testsurface);
   bool HasShrinkError()   { return !m_shrinkOkay; }
};


//_____________________________________________________________________________
class ICOpenTestGeomMap
{
public:
	ICOpenTestGeomMap();
	~ICOpenTestGeomMap();

private:
	CTypedMapStringToPtrContainer<ICOpenTestGeom*> m_openTestGeomMap;

public:
	ICOpenTestGeom* GetICOpenTestGeom(CCamCadData& camCadData, BlockStruct* compGeomBlock, BlockStruct *realPartPkgBlock, double sensorDiameter);
};


//_____________________________________________________________________________
// this is the internal test koo array, which the Takaya maschine needs. 
typedef struct
{
   CString  pinname;
	CString	pinMap;
   char     groundnet;
   int      testprobeptr;
   int      already_done;
   long     x;
   long     y;
}TK_testkoo;
typedef CTypedPtrArray<CPtrArray, TK_testkoo*> TKTestkooArray;

//_____________________________________________________________________________
// this is the internal test koo array, which the Takaya maschine needs.
class TK_short
{
public:
   CString  netname1;
   CString  netname2;
   int      tested;

public:
   TK_short(const CString& netname1,const CString& netname2);

};

class TKShortArray : public CTypedPtrArrayWithMapContainer<TK_short>
{
public:
   TKShortArray();

   bool hasShort(const CString& netname1,const CString& netname2);
   TK_short* addShort(const CString& netname1,const CString& netname2);
};

//_____________________________________________________________________________
// this keeps track of netlist status
class TK_netname
{
private:
   int m_index;

public:
   CString  netname;
   char     maxhits_exceeded;
   char     groundnet;
   int      probecnt;

public:
   TK_netname(int index);

   int getIndex() const { return m_index; }
};

//_____________________________________________________________________________
class TKNetnameArray : public CTypedPtrArrayWithMapContainer<TK_netname>
{
public:
   TKNetnameArray(int size);

   TK_netname* getDefined(const CString& name);
};

//_____________________________________________________________________________
//class CMapEntityToCompPin 
//{
//private:
//   CTypedMapIntToPtrContainer<CompPinStruct*> m_compPinMap;
//
//public:
//   CMapEntityToCompPin();
//
//   CompPinStruct* getAt(int entityNumber);
//};

//

enum ETestFlag
{
	testNone		= 0,
	testTop		= 1,	
	testBottom	= 2,
	testBoth		= 3,
};

// this is the internal test koo array, which the Takaya maschine needs.
typedef struct
{
   CString		pin;
   CString		comp;
   ETestFlag	tested;
   char			probelayer;    // 0 none, 1 = top, 2 bottom
   int			netnameindex;
   int			testprobeptr;  // -1 not found, -2 not initialized, -3 not groundnet
}TK_pintest;
typedef CTypedPtrArray<CPtrArray, TK_pintest*> CPintestArray;

// Testprobe is from CC file. This is normally populated with TA.
typedef struct
{
   CString  geomname;   
   CString  refname; 
	long		entityNumber;
   int      netindex;
   char     bottom;  // 0 = top, 1 = bottom
   char     groundnet;
   double   x,y;
   int      usecnt;
   //int      testpref;
	int		targetTypePriority;
	bool		hasTakayaAccessAttrib;
}TK_testaccess;
typedef CTypedPtrArray<CPtrArray, TK_testaccess*> CTestaccessArray;

typedef struct
{
   CString  pinfunction;      // base, collector, emitter
   CString  pinname; 
}TK_pinfunction;
typedef CTypedPtrArray<CPtrArray, TK_pinfunction*> CPinfunctionArray;

typedef struct
{
   CString  refname;
   int      geomnum;
   double   x,y;
   double   rotation;         // in radians
   int      mirror;
}TK_PCBList;

typedef CArray<Point2, Point2&> CPolyArray;
#pragma once



#define QRefPntChoice					"RefPntChoice"
#define QManualX1							"ManualX1"
#define QManualY1							"ManualY1"
#define QManualX2							"ManualX2"
#define QManualY2							"ManualY2"
#define QFiducial1						"Fiducial1"
#define QFiducial2						"Fiducial2"
#define QICCapacitance					"ICCapacitance"
#define QICDiode							"ICDiode"
#define QICGroundNet						"ICGroundNet"

enum ERefPntChoice
{
	refPntUndefine				= -1,
	refPntTopBoard				= 0,
	refPntTopAuxilliary		= 1,
	refPntBottomBoard			= 2,
	refPntBottomAuxilliary	= 3,
};

CString refPntChoiceToString(ERefPntChoice refPntChoice);
ERefPntChoice stringToRefPntChoice(CString refPntString);

enum ERefPnt
{
	noneRefPnt					= 0,
	manualRefPnt				= 1,
	fiducialRefPnt				= 2,
};

//_____________________________________________________________________________
class CCamCadPinEntityMap
{
private:
   CTypedMapIntToPtrContainer<CCamCadPin*> m_camCadPinEntities;

public:
   CCamCadPinEntityMap(CCamCadPinMap& camCadPinMap);

   CCamCadPin* getAt(int entityNumber);
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// CRefPntSetting
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
class CRefPntSetting
{
public:
	CRefPntSetting(bool isAuxilliarry);
	~CRefPntSetting();

private:
	ERefPnt m_eRefPnt;	// 0=NONE  1=MANUAL  2=FIDUCIAL
	CString m_iManualX1;
	CString m_iManualY1;
	CString m_iManualX2;
	CString m_iManualY2;
	DataStruct* m_pFiducial1;
	DataStruct* m_pFiducial2;
	bool m_bIsAuxilliary;

public:
	void SetRefPnt(ERefPnt refPnt)					{ m_eRefPnt = refPnt;		}
	void SetManualX1(CString value)					{ m_iManualX1 = value;		}
	void SetManualY1(CString value)					{ m_iManualY1 = value;		}
	void SetManualX2(CString value)					{ m_iManualX2 = value;		}
	void SetManualY2(CString value)					{ m_iManualY2 = value;		}
	void SetFiducial1(DataStruct* fiducial)		{ m_pFiducial1 = fiducial;	}
	void SetFiducial2(DataStruct* fiducial)		{ m_pFiducial2 = fiducial;	}

	ERefPnt GetRefPnt() const							{ return m_eRefPnt;			}
	CString GetManualX1() const						{ return m_iManualX1;		}
	CString GetManualY1() const						{ return m_iManualY1;		}
	CString GetManualX2() const						{ return m_iManualX2;		}
	CString GetManualY2() const						{ return m_iManualY2;		}

	DataStruct* GetFiducial1()	const					{ return m_pFiducial1;		}
	DataStruct* GetFiducial2()	const					{ return m_pFiducial2;		}

	CString GetRefPntCommand(bool isBottom);
   CPoint2d GetReferencePoint();
};


// TakayaBoardPointSettingsDlg dialog
class TakayaBoardPointSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(TakayaBoardPointSettingsDlg)

public:
	TakayaBoardPointSettingsDlg(CCamCadData& camCadData, TakayaOriginMode originMode, CWnd* pParent = NULL);   // standard constructor
	//TakayaBoardPointSettingsDlg(CCEtoODBDoc& doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~TakayaBoardPointSettingsDlg();
	enum { IDD = IDD_TAKAYA_BOARD_REFERENCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
   CCamCadData& m_camCadData;
	//CCEtoODBDoc& m_camCadDoc;
	ERefPntChoice m_eRefPntChoice;
	ERefPnt m_eRefPnt;
   TakayaOriginMode m_originMode;
	BOOL m_bICCapacitance;
	BOOL m_bICDiode;
	CComboBox m_cboTestNetStatus;
	CComboBox m_cboFiducial1;
	CComboBox m_cboFiducial2;

	// These variables keep track of the setting for top and bottom
	CRefPntSetting* m_topBrdRefPntSetting;
	CRefPntSetting* m_topAuxRefPntSetting;
	CRefPntSetting* m_botBrdRefPntSetting;
	CRefPntSetting* m_botAuxRefPntSetting;

	DataStruct* findFiducial(CString fiducialName);

	void loadSettingFromAttribute();
	void fillFiducialCombo();
	void diableControls();
	bool saveSetting();
	void saveSettingToAttribute();
	CString getSettingString(CRefPntSetting* refPntSetting, ERefPntChoice refPntChoice);

public:
	CString GetTopBrdRefPntCommand() const			{ return m_topBrdRefPntSetting->GetRefPntCommand(false);		}
	CString GetTopAuxRefPntCommand() const			{ return m_topAuxRefPntSetting->GetRefPntCommand(false);		}
	CString GetBotBrdRefPntCommand() const			{ return m_botBrdRefPntSetting->GetRefPntCommand(true);		}
	CString GetBotAuxRefPntCommand() const			{ return m_botAuxRefPntSetting->GetRefPntCommand(true);		}

   CPoint2d GetTopBrdRefPnt() const             { return m_topBrdRefPntSetting->GetReferencePoint(); }
   CPoint2d GetTopAuxRefPnt() const             { return m_topAuxRefPntSetting->GetReferencePoint(); }
   CPoint2d GetBotBrdRefPnt() const             { return m_botBrdRefPntSetting->GetReferencePoint(); }
   CPoint2d GetBotAuxRefPnt() const             { return m_botAuxRefPntSetting->GetReferencePoint(); }

   CPoint2d GetTopBrdOrigin() const             { return (m_originMode == relativeToReferencePoint) ? GetTopBrdRefPnt() : CPoint2d(0.,0.); }
   CPoint2d GetBotBrdOrigin() const             { return (m_originMode == relativeToReferencePoint) ? GetBotBrdRefPnt() : CPoint2d(0.,0.); }


	BOOL GetICCapacitance()		{ return m_bICCapacitance; }
	BOOL GetICDiode()				{ return m_bICDiode;			}

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedRefPointChoice();
	afx_msg void OnBnClickedTakayaRadioNone();
	afx_msg void OnBnClickedTakayaRadioManual();
	afx_msg void OnBnClickedTakayaRadioFiducial();
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeTakayaComboFiducial1();
	afx_msg void OnCbnSelchangeTakayaComboFiducial2();
};


//-----------------------------------------------------------------------------
// CPinToPinShort
//-----------------------------------------------------------------------------
class CPcbComponentPinToPinShort : public CObject
{
public:
	CPcbComponentPinToPinShort(CString pinRefdes1, CString pinRefdes2)
	{
		m_pinRefdes1 = pinRefdes1;
		m_pinRefdes2 = pinRefdes2;
	};
	~CPcbComponentPinToPinShort(){};

private:
	CString m_pinRefdes1;
	CString m_pinRefdes2;

public:
	CString getPinRefdes1() const			{ return m_pinRefdes1;	}
	CString getPinRefdes2() const			{ return m_pinRefdes2;	}
};


//-----------------------------------------------------------------------------
// CPinToPinShortAnalyzer
//-----------------------------------------------------------------------------
class CPcbComponentPinToPinAnalyzer : public CTypedMapSortStringToObContainer<CPcbComponentPinToPinShort>  //: public CPcbComponentPinAnalyzer
{
public:
	CPcbComponentPinToPinAnalyzer(BlockStruct& block);
	~CPcbComponentPinToPinAnalyzer();

private:
   BlockStruct& m_block;
//	CTypedMapStringToPtrContainer<CPcbComponentPinToPinShort*> m_pinToPinShortArray;

public:
	void analysisPinToPinShort();
};


//-----------------------------------------------------------------------------
// CPowerRailShortOutput
//-----------------------------------------------------------------------------
class CPowerRailShortOutput
{
public:
	CPowerRailShortOutput();
	~CPowerRailShortOutput();

private:
	CString m_refdes;
	CString m_netName1; 
	CString m_netName2;
	long m_testAccessX1;
	long m_testAccessY1;
	long m_testAccessX2;
	long m_testAccessY2;
	long m_testAccessEntityNumber1;
	long m_testAccessEntityNumber2;
	bool m_bottom;

public:
	void setRefdes(const CString refdes)							{ m_refdes = refdes;									}
	CString getRefdes() const											{ return m_refdes;									}

	void setNetName1(const CString netName)						{ m_netName1 = netName;								}
	CString getNetName1() const										{ return m_netName1;									}

	void setNetName2(const CString netName)						{ m_netName2 = netName;								}
	CString getNetName2() const										{ return m_netName2;									}

	void setTestAccess1(const long x, const long y)				{ m_testAccessX1 = x; m_testAccessY1 = y;		}
	long getTestAccessX1() const										{ return m_testAccessX1;							}
	long getTestAccessY1() const										{ return m_testAccessY1;							}

	void setTestAccess2(const long x, const long y)				{ m_testAccessX2 = x; m_testAccessY2 = y;		}
	long getTestAccessX2() const										{ return m_testAccessX2;							}
	long getTestAccessY2() const										{ return m_testAccessY2;							}

	void setTestAccessEntityNumber1(const long entityNumber)	{ m_testAccessEntityNumber1 = entityNumber;	}
	long getTestAccessEntityNumber1() const						{ return m_testAccessEntityNumber1;				}

	void setTestAccessEntityNumber2(const long entityNumber)	{ m_testAccessEntityNumber2 = entityNumber;	}
	long getTestAccessEntityNumber2() const						{ return m_testAccessEntityNumber2;				}

	void setBottom(const bool isBottom)								{ m_bottom = isBottom;								}
	bool isBottom() const												{ return m_bottom;									}
};


//-----------------------------------------------------------------------------
// CPcbComponentPinAnalyzer
//-----------------------------------------------------------------------------
class CPinToPinOutput
{
public:
	CPinToPinOutput();
	~CPinToPinOutput();

private:
	CString m_compRefdes;
	CString m_partNumber;
	CString m_pinRefdes1;
	CString m_pinRefdes2;
	CString m_netName1;
	CString m_netName2;
	long m_testAccessX1;
	long m_testAccessY1;
	long m_testAccessX2;
	long m_testAccessY2;
	long m_testAccessEntityNumber1;
	long m_testAccessEntityNumber2;
	bool m_bottom;

public:
	
	void setCompRefdes(const CString compRefdes)					{ m_compRefdes = compRefdes;						}
	CString getCompRefdes() const										{ return m_compRefdes;								}

	void setPartNumber(const CString partNumber)					{ m_partNumber = partNumber;						}
	CString getPartNumber() const										{ return m_partNumber;								}

	void setPinRefdes1(const CString pinRefdes)					{ m_pinRefdes1 = pinRefdes;						}
	CString getPinRefdes1() const										{ return  m_pinRefdes1;								}

	void setPinRefdes2(const CString pinRefdes)					{ m_pinRefdes2 = pinRefdes;						}
	CString getPinRefdes2() const										{ return  m_pinRefdes2;								}

	void setNetName1(const CString netName)						{ m_netName1 = netName;								}
	CString getNetName1() const										{ return m_netName1;									}

	void setNetName2(const CString netName)						{ m_netName2 = netName;								}
	CString getNetName2() const										{ return m_netName2;									}

	void setTestAccess1(const long x, const long y)				{ m_testAccessX1 = x; m_testAccessY1 = y;		}
	long getTestAccessX1() const										{ return m_testAccessX1;							}
	long getTestAccessY1() const										{ return m_testAccessY1;							}

	void setTestAccess2(const long x, const long y)				{ m_testAccessX2 = x; m_testAccessY2 = y;		}
	long getTestAccessX2() const										{ return m_testAccessX2;							}
	long getTestAccessY2() const										{ return m_testAccessY2;							}

	void setTestAccessEntityNumber1(const long entityNumber)	{ m_testAccessEntityNumber1 = entityNumber;	}
	long getTestAccessEntityNumber1() const						{ return m_testAccessEntityNumber1;				}

	void setTestAccessEntityNumber2(const long entityNumber)	{ m_testAccessEntityNumber2 = entityNumber;	}
	long getTestAccessEntityNumber2() const						{ return m_testAccessEntityNumber2;				}

	//void setTestAccess1(const double x, const double y)	{ m_testAccess1.x = x; m_testAccess1.y = y;	}
	//void setTestAccess1(const CPoint2d testAccessPoint)	{ m_testAccess1 = testAccessPoint;				}
	//CPoint2d getTestAccess1() const								{ return m_testAccess1;								}
	//
	//void setTestAccess2(const double x, const double y)	{ m_testAccess2.x = x; m_testAccess2.y = y;	}
	//void setTestAccess2(const CPoint2d testAccessPoint)	{ m_testAccess2 = testAccessPoint;				}
	//CPoint2d getTestAccess2() const								{ return m_testAccess2;								}

	void setBottom(const bool isBottom)							{ m_bottom = isBottom;								}
	bool isBottom() const											{ return m_bottom;									}
};


//-----------------------------------------------------------------------------
// CPinToPinShortGeomMap
//-----------------------------------------------------------------------------
class CPinToPinShortTest
{
public:
	CPinToPinShortTest(CCamCadDatabase& camcadDatabase);
	~CPinToPinShortTest();

private:
   CCamCadDatabase& m_camCadDatabase;
	CTypedMapIntToPtrContainer<CPcbComponentPinToPinAnalyzer*> m_pcbComponentPinToPinAnalyzerMap;
	CTypedPtrListContainer<CPowerRailShortOutput*> m_powerRailOutputList;
	CTypedPtrListContainer<CPinToPinOutput*> m_pinToPinOutputList;
	CMapStringToString m_testedComponentMap;
	CMapStringToString m_testedPowerGroundNetMap;

	int m_powerNetCount;
	int m_groundNetCount;


	CPcbComponentPinToPinAnalyzer* getPinToPinShortGeom(int blockNumber);
	void doPowerRailShortTest(FileStruct& pcbFile, int maxhit, bool bottom);
	void doPinToPinShortTest(FileStruct& pcbFile, int maxhit, bool bottom);

public:
	void doShortTests(FileStruct& pcbFile, int maxhit, bool bottom);;
	void writePinToPinOutput(FILE* outputFile, bool bottom);
};

//-----------------------------------------------------------------------------
// CIcDiodeCommand
//-----------------------------------------------------------------------------
class CIcDiodeCommand
{
public:
   CIcDiodeCommand(const CString partNumber, const CString pinName, const bool swap)
   {
      m_partNumber = partNumber;
      m_pinName = pinName;
      m_swap = swap;
   }
   ~CIcDiodeCommand() {};

private:
   CString m_partNumber;
   CString m_pinName;
   bool m_swap;

public:
   CString getPartNumber() const { return m_partNumber; }
   CString getPinName() const { return m_pinName; }
   bool isSwap() const { return m_swap; }
};

//-----------------------------------------------------------------------------
// CIcDiodeCommandMap
//-----------------------------------------------------------------------------
class CIcDiodeCommandMap
{
public:
   CIcDiodeCommandMap();
   ~CIcDiodeCommandMap();

private:
   CTypedMapStringToPtrContainer<CIcDiodeCommand*> m_icDiodeCommandMap;

public:
   void addCommand(const CString partNumber, const CString pinName, const bool swap);
   int getGroundPin(const CString partNumber, const CString compName, const int maxhits, const int testSurface, long& groundPinX, long& groundPinY, bool& swap);

   void empty() { m_icDiodeCommandMap.empty(); }
};


//-----------------------------------------------------------------------------
// CPinToPinTestComp
//-----------------------------------------------------------------------------
//class CPinToPinTestComp
//{
//public:
//	CPinToPinTestComp();
//	~CPinToPinTestComp();
//
//private:
//	CString m_refdes;
//
//};



////-----------------------------------------------------------------------------
//// CComponentPinToPinShort
////-----------------------------------------------------------------------------
//class CComponentPinToPinShort
//{
//public:
//	CComponentPinToPinShort(DataStruct& dataStruct);
//	~CComponentPinToPinShort();
//
//private:
//	DataStruct& m_dataStruct;
//}
//

//class CTKTestAccess
//{
//public:
//	CTKTestAccess();
//	~CTKTestAccess();
//
//private:
//	CPoint2d m_coordinate;
//	int m_priority;
//	int m_hitCount;
//	int m_topBottom;
//}
//
//
////-----------------------------------------------------------------------------
//// CTKNetAccess
////-----------------------------------------------------------------------------
//class CTKNetAccess
//{
//public:
//	CTKNetAccess();
//	~CTKNetAccess();
//
//private:
//	CString m_name;
//	CTypedPtrArrayContainer<CTKTestAccess*> m_testAccessArray;
//}

//
//class CPinToPinTest
//{
//public:
//	CPinToPinTest();
//	~CPinToPinTest();
//
//private:
//
//	
//};
