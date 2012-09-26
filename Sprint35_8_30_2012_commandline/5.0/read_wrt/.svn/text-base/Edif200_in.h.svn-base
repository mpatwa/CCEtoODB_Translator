// $Header: /CAMCAD/4.6/read_wrt/Edif200_in.h 50    9/27/06 2:32p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once


/* Define Section *********************************************************/
#define MAX_LAYERS               255         // Max number of layers
#define MAX_SHAPE                1000
#define MAX_EDIFUNITS            20

// These are the only allowable view type in Edif 200
#define SCH_VIEW_BEHAVIOR        "BEHAVIOR"
#define SCH_VIEW_DOCUMENT        "DOCUMENT"
#define SCH_VIEW_GRAPHIC         "GRAPHIC"
#define SCH_VIEW_LOGICMODEL      "LOGICMODEL"
#define SCH_VIEW_MASKLAYOUT      "MASKLAYOUT"
#define SCH_VIEW_NETLIST         "NETLIST"
#define SCH_VIEW_PCBLAYOUT       "PCBLAYOUT"
#define SCH_VIEW_SCHEMATIC       "SCHEMATIC"
#define SCH_VIEW_STRANGER        "STRANGER"
#define SCH_VIEW_SYMBOLIC        "SYMBOLIC"

// These are the only allowable cell type in Edif 200
#define SCH_CELLTYPE_GENERIC     "GENERIC"
#define SCH_CELLTYPE_TIE         "TIE"
#define SCH_CELLTYPE_RIPPER      "RIPPER"

#define SCH_HOTSPOT_RULE         "________RULE"
#define SCH_HOTSPOT_PIN          "________PIN"

/* Structures Section *********************************************************/

/*** class Parser ***/
class Parser
{
public:
   static const int MAX_LENGTH = 10000 ; 
   static const int MAX_TOKEN = 10000;
   BOOL isTokenName;
   BOOL isPeekTokenName;
   char token[MAX_TOKEN];
   char peekToken[MAX_TOKEN];
   int lineCount;

   Parser()
   {
      token[0] = '\0';
      peekToken[0] = '\0';
      file = NULL;
      lineCount = 0;
      inputLine = new char[MAX_LENGTH+1];
      inputLine[0] = '\0';
      isNewLine = TRUE;
      isPushToken = FALSE;
      isTokenName = FALSE;
      isPeekTokenName = FALSE;
   };

   ~Parser()
   {
      delete []inputLine;
   };

   BOOL OpenFile(CString fileName);
   void CloseFile();
   BOOL GetToken();
   BOOL GetPeekToken();
   void PushToken();
   int SkipCommand();
   int ParsingErr();

private:
   FILE *file;
   char *inputLine;
   BOOL isNewLine;
   BOOL isPushToken;

   BOOL GetNext();
   BOOL GetLine();
};

/*** class Unit ***/
class Unit
{
public:
   CString unitType;
   CString unitName;
   double edifUnit;
   double externalUnit;

   Unit()
   {
      unitType = "";
      unitName = "";
      edifUnit = 1;
      externalUnit = 1;
   };

   double GetExternalValue(double edifValue);
};

/*** class FigureGroup ***/
class FigureGroup
{
public :
   CString m_name;
   double m_textHeight;
   double m_textWidth;
   int m_widthIndex;
   bool m_visible;
   int m_color;

   FigureGroup();

   int DoFigureGroup();
   FigureGroup& operator=(const FigureGroup &other);
};

/*** class DisplayParam ***/
class DisplayParam
{
public:
   int m_horizontalJust;
   int m_verticalJust;
   double m_rotation;
   int m_mirror;
   double m_x;
   double m_y;
   bool m_isEmpty;
   FigureGroup m_figureGroup;

   DisplayParam();

};

/*** class Display ***/
class Display
{
public:
	CTypedPtrListContainer<DisplayParam*> displayList;

	Display() { displayList.RemoveAll(); }
	~Display() { displayList.RemoveAll(); }
   int DoDisplay();
};

/*** class Transform ***/
class Transform
{
public:
   double scaleX;
   double scaleY;
   double deltaX;
   double deltaY;
   double rotation;
   int mirror;
   double x;
   double y;
   BOOL isEmpty;

   Transform()
   {
      scaleX = 1;
      scaleY = 1;
      deltaX = 0;
      deltaY = 0;
      rotation = 0;
      mirror = 0;
      x = 0;
      y = 0;
      isEmpty = TRUE;
   }

   int DoTransform();
};

typedef CMap<CString, LPCSTR, int, int> myMapStringToInt;

/*** class EdifAttrib ***/
class EdifAttrib
{
public:
   EdifAttrib();
   ~EdifAttrib();

   int AddDefinedAttrib(CAttributes** attribMap, CString keyword, CString value, DbFlag attribFlag, Display *display, Attrib **attrib);
   int AddUndefinedAttrib(CAttributes** attribMap, CString keyword, int valueType, CString value, DbFlag attribFlag, Display *display, Attrib **attrib);

private:
   CMapStringToString edifToCCKeyword;
   myMapStringToInt edifKeyToType;
   myMapStringToInt edifKeyToIndex;
};

/*** class Port ***/
class Port
{
public:
   CString portName;

   Port()
   {
      portName = "";
   };
   ~Port(){};
   int DoPort(CAttributes** attribMap);
};

/*** class EdifFigure ***/
class EdifFigure
{
public:
   FigureGroup figureGroup;
   DataStruct *data;
   BOOL isPort;

   EdifFigure()
   {
      data = NULL;
      isPort = FALSE;
      figureGroup.m_color = 0;
      figureGroup.m_name = "";
      figureGroup.m_textHeight = 0;
      figureGroup.m_textWidth = 0;
      figureGroup.m_visible = FALSE;
      figureGroup.m_widthIndex = 0;
   };
   ~EdifFigure(){};

   int DoFigure(int layerIndex, bool filled = false);
   CPoly* CreatePoly();
   int AddPolyToPolyList(CPoly *poly, int layerIndex);
};

class CEdif200Signal;
class CEdif200;
class CEdif200Net;
class CEdif200Bus;
class CEdif200Instance;
class CEdif200Page;
class CEdif200View;
enum EEdif200ConnectorType
{
	connectorTypeGlobalPort,
	connectorTypeMasterPort,
	connectorTypeInstancePort,
	connectorTypeOnPageConnector,
	connectorTypeOffPageConnecotor,
	connectorTypeRipper,
	connectorTypeUndefined,
};

enum EEdif200SignalType
{
	signalTypeNet,
	signalTypeBus,
	signalTypeUndefined,
};

enum EEdif200RenamedBy
{
	renamedByGlobalPort,
	renamedByOnPageConnector,
	renamedByOffPageConnector,
	renamedByMasterPort,
	renamedByBus,
	renamedByAlias,
	renamedByUndefined,
};


//-----------------------------------------------------------------------------
// CEdif200ConnectorObject
//-----------------------------------------------------------------------------
class CEdif200ConnectorObject
{
public:
	CEdif200ConnectorObject(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200ConnectorType connectorType)
		: m_edif(edif)
		, m_libraryName(libraryName)
		, m_cellName(cellName)
		, m_viewName(viewName)
		, m_pageName(pageName)
		, m_name(name)
		, m_connectorType(connectorType)
	{
		m_primaryName.IsEmpty();
		m_designator.IsEmpty();
	};
	~CEdif200ConnectorObject() {};

protected:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_viewName;
	CString m_pageName;
	CString m_name;			// instance name of the port			
	CString m_primaryName;
	CString m_designator;
	EEdif200ConnectorType m_connectorType;

public:
	void setPrimaryName(const CString primaryName)		{ m_primaryName = primaryName;			}
	void setDesignator(const CString designator)			{ m_designator = designator;				}
	CString getName() const										{ return m_name;								}
	CString getPrimaryName() const							{ return m_primaryName;						}
	CString getDesignator() const								{ return m_designator;						}
	EEdif200ConnectorType getcConnectorType() const				{ return m_connectorType;					}
};

//-----------------------------------------------------------------------------
// CEdif200Port
//	 - CEdif200Port is used for global and master port
//-----------------------------------------------------------------------------
class CEdif200Port : public CEdif200ConnectorObject
{
public:
	CEdif200Port(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200ConnectorType connectorType);
	~CEdif200Port();

private:
	CString m_portRef;		// reference name of global or master port defined at interface
	DataStruct* m_portDataStruct;
	CEdif200Signal* m_connectedToSignal;

	void reset();

public:
	CString getPortRef()	const									{ return m_portRef;							}
	void setPortRef(const CString portRef)					{ m_portRef = portRef;						}

	DataStruct* getPortDataStruct()							{ return m_portDataStruct;					}
	void setPortDataStruct(DataStruct* portDataStruct)	{ m_portDataStruct = portDataStruct;	}

	CEdif200Signal* getConnectedToSignal()							{ return m_connectedToSignal;				}
	void setConnectedToSignal(CEdif200Signal* signal)			{ m_connectedToSignal = signal;			}
};

//-----------------------------------------------------------------------------
// CEdif200InstancePort
//  - CEdif200InstancePort is used for port inside each instance of symbol
//-----------------------------------------------------------------------------
class CEdif200InstancePort : public CEdif200ConnectorObject
{
public:
	CEdif200InstancePort(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString instanceName, const CString name);
	~CEdif200InstancePort();

private:
	CString m_instanceName;
	CString m_designator;            // be from instancePortAttribute() in instance() or from attribute on DataStruct
	CEdif200Signal* m_connectedToSignal;
   //CAttributes* m_attribMap;				// attribute map which contains overwriting value only
   //CAttributes* m_attribDisplayMap;    // attribute map which contains overwriting location only

	void reset();

public:
	CString getInstanceName() const						{ return m_instanceName;				}

	CString getDesignator() const							{ return m_designator;					}
	void setDesignator(const CString designator)		{ m_designator = designator;			}	

	CEdif200Signal* getConnectedToSignal()						{ return m_connectedToSignal;			}
	void setConnectedToSignal(CEdif200Signal* signal)		{ m_connectedToSignal = signal;		} 
};

//-----------------------------------------------------------------------------
// CEdif200PageConnector
//-----------------------------------------------------------------------------
class CEdif200PageConnector : public CEdif200ConnectorObject
{
public:
	CEdif200PageConnector(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200ConnectorType connectorType);
	~CEdif200PageConnector();

private:
	DataStruct* m_connectorDataStruct;
	CEdif200Signal* m_connectedToSignal;

	void reset();

public:
	DataStruct* getConnectorDataStruct()						{ return m_connectorDataStruct;						}
	void setConnectorDataStruct(DataStruct* dataStruct)	{ m_connectorDataStruct = dataStruct;				}

	CEdif200Signal* getConnectedToSignal()								{ return m_connectedToSignal;							}
	void setConnectedToSignal(CEdif200Signal* signal)				{ m_connectedToSignal = signal;						}
};

//-----------------------------------------------------------------------------
// CEdif200RipperHotspot
//-----------------------------------------------------------------------------
class CEdif200RipperHotspot
{
public:
   CEdif200RipperHotspot(const CString name);
   ~CEdif200RipperHotspot();

private:
   CString m_name;
   CString m_pin;
   CString m_rule;
   CEdif200Signal* m_connectedToSignal;

   void reset();

public:
   CString getName() const                               { return m_name;                                }
   CString getPin() const                                { return m_pin;                                 }
   void setPin(const CString pin)                        { m_pin = pin;                                  }
   CString getRule() const                               { return m_rule;                                }
   void setRule(const CString rule)                      { m_rule = rule;                                }
	CEdif200Signal* getConnectedToSignal()								{ return m_connectedToSignal;		   				}
	void setConnectedToSignal(CEdif200Signal* signal)				{ m_connectedToSignal = signal;						}
};

//-----------------------------------------------------------------------------
// CEdif200Ripper
//-----------------------------------------------------------------------------
class CEdif200Ripper : public CEdif200ConnectorObject
{
public:
	CEdif200Ripper(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200ConnectorType connectorType);
	~CEdif200Ripper();

private:
	CString m_wireRule;
   CString m_hotspotConnectedToBus;
	CEdif200Signal* m_connectedToSignal1;
	CEdif200Signal* m_connectedToSignal2;
	CTypedMapStringToPtrContainer<CEdif200RipperHotspot*> m_ripperHotspotMap;
   CAttributes* m_attribMap;									

	void reset();

public:
   CAttributes*& getAttributesRef()							   { return m_attribMap; }

	CEdif200Signal* getConnectedToSignal1()								{ return m_connectedToSignal1;						}
	void setConnectedToSignal1(CEdif200Signal* signal)				{ m_connectedToSignal1 = signal;						}
	CEdif200Signal* getConnectedToSignal2()								{ return m_connectedToSignal2;						}
	void setConnectedToSignal2(CEdif200Signal* signal)				{ m_connectedToSignal2 = signal;						}
	CEdif200Net* getConnectedToNet(CEdif200Net* fromNet);

   CEdif200RipperHotspot* addRipperHotspot(const CString ripperHotspotName);
   CEdif200RipperHotspot* findRipperHotspot(const CString ripperHotspotName);
   int getRipperHotspotCount() const                     { return m_ripperHotspotMap.GetCount();         }
   POSITION getRipperHotspotStartPosition() const        { return m_ripperHotspotMap.GetStartPosition(); }
   CEdif200RipperHotspot* getNextRipperHotspot(POSITION& pos)
   {
      CEdif200RipperHotspot* hotspot = NULL;
      CString key;
      m_ripperHotspotMap.GetNextAssoc(pos, key, hotspot);
      return hotspot;
   }
};

//-----------------------------------------------------------------------------
// CSignalObject
//-----------------------------------------------------------------------------
class CEdif200Signal
{
public:
	CEdif200Signal(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name, const EEdif200SignalType signalType);
	~CEdif200Signal();

protected:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_viewName;
   CString m_pageName;
	CString m_name;
	CString m_primaryName;
	CString m_annotateName;
   CString m_renamedName;
	EEdif200SignalType m_signalType;
	bool m_isNameUserDefined;
	bool m_isRenameDone;
	EEdif200RenamedBy m_renamedBy;
   CAttributes* m_attribMap;

	void reset();

public:
   CAttributes*& getAttributesRef()							{ return m_attribMap;                  }

   CString getName() const										{ return m_name;								}
	EEdif200SignalType getSignalType() const				{ return m_signalType;						}

	CString getPrimaryName() const							{ return m_primaryName;						}
	void setPrimaryName(const CString primaryName)		{ m_primaryName = primaryName;			}

	CString getAnnotateName() const							{ return m_annotateName;					}
	void setAnnotateName(const CString annotateName)	{ m_annotateName = annotateName;			}

   CString getRenamedName() const                     { return m_renamedName;                }
   void setRenamedName(const CString renamedName, const EEdif200RenamedBy renamedBy);

	bool isRenameDone() const									{ return m_isRenameDone;					}
	void setRenameDone(const bool done)						{ m_isRenameDone = done;					}
	EEdif200RenamedBy getRenamedBy() const					{ return m_renamedBy;							}

	void prepareForRename();
};

//-----------------------------------------------------------------------------
// CEdif200Net
//-----------------------------------------------------------------------------
class CEdif200Net : public CEdif200Signal
{
public:
	CEdif200Net(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name);
   CEdif200Net(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CEdif200Net& net);
	~CEdif200Net();
};

//-----------------------------------------------------------------------------
// CEdif200Bus
//-----------------------------------------------------------------------------
class CEdif200Bus : public CEdif200Signal
{
public:
	CEdif200Bus(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString pageName, const CString name);
   CEdif200Bus(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, CEdif200Page& page, const CEdif200Bus& bus);
	~CEdif200Bus();

private:
	CTypedMapStringToPtrContainer<CEdif200Net*> m_netMap;
	CTypedMapStringToPtrContainer<CEdif200Bus*> m_busMap;

	void reset();
	void enumerateBusName();

public:
	void addBus(CEdif200Bus* bus);
	void addNet(CEdif200Net* net);
   void createNetLis();
};

//-----------------------------------------------------------------------------
// CEdif200Instance
//-----------------------------------------------------------------------------
class CEdif200Instance
{
public:
	CEdif200Instance(CEdif200& edif, const CString libraryName, const CString cellName, const CString viewName, const CString name);
	~CEdif200Instance();
	
private:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_viewName;
	CString m_name;
	CString m_displayName;
	CString m_primaryName;
	CString m_designator;
	CString m_libraryRef;
	CString m_cellRef;
	CString m_viewRef;
	CString m_symbolRef;
	DataStruct* m_instanceDataStruct;

	// Key is port ref, use to keep track of instance port connection to net
	CTypedMapStringToPtrContainer<CEdif200InstancePort*> m_instancePortMap; 

	void reset();

public:
   CMapStringToPtr   portAttribMap;							// a map of ports' attribute map which contains overwriting value only
																		// the key is the port ref

	CString getName() const										{ return m_name;								}
	
	CString getDisplayName() const							{ return m_displayName;						}
	void setDisplayName(const CString displayName)		{ m_displayName = displayName;			}

	CString getPrimaryName() const							{ return m_primaryName;						}
	void setPrimaryName(const CString primaryName)		{ m_primaryName = primaryName;			}

	CString getDesignator() const								{ return m_designator;						}
	void setDesignator(const CString designator)			{ m_designator = designator;				}

	CString getLibraryRef() const								{ return m_libraryRef;						}
	void setLibraryRef(const CString libraryRef)			{ m_libraryRef = libraryRef.IsEmpty()?m_libraryName:libraryRef;	}

	CString getCellRef() const									{ return m_cellRef;							}
	void setCellRef(const CString cellRef)					{ m_cellRef = cellRef;						}

	CString getViewRef() const									{ return m_viewRef;							}
	void setViewRef(const CString viewRef)					{ m_viewRef = viewRef;						}
 
	CString getSymbolRef() const								{ return m_symbolRef;						}
	void setSymbolRef(const CString symbolRef)			{ m_symbolRef = symbolRef;					}

	CString getSymbolBlockName() const						
	{
		CString blockName;
		blockName.Format("%s%c%s%c%s", m_libraryRef, SCH_DELIMITER, m_cellRef, SCH_DELIMITER, m_symbolRef);
		blockName.MakeUpper();
		return blockName;
	}

	DataStruct* getInstanceDataStruct()						{ return m_instanceDataStruct;			}
	void setInstanceDataStruct(DataStruct* instance)	{ m_instanceDataStruct = instance;		}

	CEdif200InstancePort* addInstancePort(CString portRef);
	CEdif200InstancePort* findInstancePort(CString portRef);
   CEdif200InstancePort* getNextInstancePort(POSITION& pos)
   {
      CEdif200InstancePort* instancePort = NULL;
      CString key;
      m_instancePortMap.GetNextAssoc(pos, key, instancePort);
      return instancePort;
   }
   POSITION getInstancePortStartPosition() const      { return m_instancePortMap.GetStartPosition(); }

   bool isHierarchSymbol();
};

//-----------------------------------------------------------------------------
// CEdif200SymbolPort
//-----------------------------------------------------------------------------
class CEdif200SymbolPort
{
public:
	CEdif200SymbolPort(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString symbolName, const CString name);
	~CEdif200SymbolPort();

private:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_cellName;
   CString m_viewName;
	CString m_symbolName;
	CString m_name;			// instance name of the port
	CString m_portRef;		// reference name of local port defined at interface
	DataStruct* m_portDataStruct;
   CString m_targetPortName;

public:
   CString getName() const                            { return m_name;                       }

	CString getPortRef()	const									{ return m_portRef;							}
	void setPortRef(const CString portRef)					{ m_portRef = portRef;						}

	DataStruct* getPortDataStruct()							{ return m_portDataStruct;					}
	void setPortDataStruct(DataStruct* portDataStruct)	{ m_portDataStruct = portDataStruct;	}

	CString getTargetPortName()	const					   { return m_targetPortName;							}
	void setTargetPortName(const CString portName)		{ m_targetPortName = portName;					}
};

//-----------------------------------------------------------------------------
// CEdif200Symbol
//-----------------------------------------------------------------------------
class CEdif200Symbol
{
public:
	CEdif200Symbol(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString name);
   CEdif200Symbol(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, CEdif200Symbol& symbol);
	~CEdif200Symbol();

private:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_cellName;
   CString m_viewName;
	CString m_name;
	BlockStruct* m_symbolBlock;
	CTypedMapStringToPtrContainer<CEdif200SymbolPort*> m_portMap;

	void reset();

public:
	CString getLibraryName() const							{ return m_libraryName;						}
	CString getCellName() const								{ return m_cellName;							}
   CString getViewName() const                        { return m_viewName;                   }
	CString getName() const										{ return m_name;								}

	BlockStruct* getSymbolBlock()								{ return m_symbolBlock;				      }
	void setSymbolBlock(BlockStruct* symbolBlock)	{ m_symbolBlock = symbolBlock;	}

	CEdif200SymbolPort* addPort(CString portInstanceName);
	CEdif200SymbolPort* findPort(CString portInstancetName);
   CEdif200SymbolPort* getNextSymbolPort(POSITION& pos)
   {
      if (pos == NULL)
         return NULL;

      CEdif200SymbolPort* symbolPort = NULL;
      CString key;
      m_portMap.GetNextAssoc(pos, key, symbolPort);
      return symbolPort;
   }
   POSITION getSymbolPortStartPosition() const      { return m_portMap.GetStartPosition(); }

	CString generateSymbolBlockName();
   int doSymbol();
};

//-----------------------------------------------------------------------------
// CEdif200Page
//-----------------------------------------------------------------------------
class CEdif200Page
{
public:
	CEdif200Page(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString name, const CString generatedName);
   CEdif200Page(CEdif200 &edif, const CString libraryName, const CString cellName, const CString viewName, const CString name, const CString generatedName, CEdif200Page& page);
   //CEdif200Page& operator =(const CEdif200Page& page);
   ~CEdif200Page();

private:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_cellName;
   CString m_viewName;
	CString m_name;
   CString m_generatedName;
   bool m_goodHierarchySheet;
   BlockStruct* m_pageBlock;
	CTypedMapStringToPtrContainer<CEdif200PageConnector*> m_offPageConnectorMap;
	CTypedMapStringToPtrContainer<CEdif200Instance*> m_instanceMap;
	CTypedMapStringToPtrContainer<CEdif200Net*> m_netMap;
	CTypedMapStringToPtrContainer<CEdif200Bus*> m_busMap;
	CTypedMapStringToPtrContainer<CEdif200Port*> m_globalPortMap;
	CTypedMapStringToPtrContainer<CEdif200Port*> m_masterPortMap;
   
   //	CTypedMapStringToPtrContainer<CEdif200Ripper*> m_ripperMap;

   void reset();
	CString generatePageBlockName();
   void prepareBusAndNetForRename();
   void applyGlobalPortNameToBussesAndNets();
	void applyDoneToBussesAndNetsOnPageConnector();
	void applyNetsOnInstanceToHierarchyStructure();
	void createNetListFromRenamedNetsAndBusses();
	void applyRenamedNetAndBusNameToEtch();
   void applyMasterPortNameToBussesAndNets(CEdif200Instance* instance);

   int doNet(CEdif200Net** BusNet = NULL, CEdif200Bus** subBus = NULL);
   int doNetBundle();
	int doSubNet(EdifFigure figure);
	int doJoined(CEdif200Net *net);
	int doPortRef(CString &portRef, CString &instanceRef);
	int doInstanceRef(CString &instanceRef);

public:
   CString getName() const { return m_name; }
   CString getGeneratedName() const { return m_generatedName; }
   BlockStruct* getPageBlock() { return m_pageBlock; }

	// offPageConnector functions
	CEdif200PageConnector* addOffPageConnector(const CString connectorName);
	CEdif200PageConnector* findOffPageConnector(const CString connectorName);
   CEdif200PageConnector* getNextOffPageConnector(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdif200PageConnector* pageConnector = NULL;
      CString key;
      m_offPageConnectorMap.GetNextAssoc(pos, key, pageConnector);
      return pageConnector;
   }
   POSITION getOffPageConnectorStartPosition() { return m_offPageConnectorMap.GetStartPosition(); }

	// instance functions
	CEdif200Instance* findInstance(const CString instanceName);
   CEdif200Instance* getNextInstance(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdif200Instance* instance = NULL;
      CString key;
      m_instanceMap.GetNextAssoc(pos, key, instance);
      return instance;
   }
   POSITION getInstanceStartPosition() { return m_instanceMap.GetStartPosition(); }

	// net functions
	CEdif200Net* addNet(const CString netName);
	CEdif200Net* findNet(const CString netName);
   CEdif200Net* getNextNet(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdif200Net* net = NULL;
      CString key;
      m_netMap.GetNextAssoc(pos, key, net);
      return net;
   }
   POSITION getNetStartPosition() { return m_netMap.GetStartPosition(); }

	// bus functions
	CEdif200Bus* addBus(const CString busName);
	CEdif200Bus* findBus(const CString busName);
   CEdif200Bus* getNextBus(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdif200Bus* bus = NULL;
      CString key;
      m_busMap.GetNextAssoc(pos, key, bus);
      return bus;
   }
   POSITION getBusStartPosition() { return m_busMap.GetStartPosition(); }

   // globalPort functions
   CEdif200Port* addGlobalPort(const CString portName);
   CEdif200Port* findGlobalPort(const CString portName);
   CEdif200Port* getNextGlobalPort(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdif200Port* globalPort = NULL;
      CString key;
      m_globalPortMap.GetNextAssoc(pos, key, globalPort);
      return globalPort;
   }
   POSITION getGlobalPortStartPosition() { return m_globalPortMap.GetStartPosition(); }


   // masterPort functions
   CEdif200Port* addMasterPort(const CString portName);
   CEdif200Port* findMasterPort(const CString portName);
   CEdif200Port* getNextMasterPort(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdif200Port* masterPort = NULL;
      CString key;
      m_masterPortMap.GetNextAssoc(pos, key, masterPort);
      return masterPort;
   }
   POSITION getMasterPortStartPosition() { return m_masterPortMap.GetStartPosition(); }

   void convertLogicalSchematicToPhysicalSchematic();
   void createNetList();
   void propagateNetsFromInstanceToHierarchyStructure(CEdif200Instance* instance);

	int doPage();
};

//-----------------------------------------------------------------------------
// CEdif200View
//-----------------------------------------------------------------------------
class CEdif200View
{
public:
	CEdif200View(CEdif200 &edif, const CString libraryName, const CString cellName, const CString name);
	~CEdif200View();

private:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_cellName;
	CString m_name;
   CString m_viewType;

   // Edif200 variables
   CTypedPtrListContainer<CEdif200Page*> m_pageMap;
//	CTypedMapStringToPtrContainer<CEdif200Page*> m_edifPageMap;
	CTypedMapStringToPtrContainer<CEdif200Symbol*> m_symbolMap;
   CMapStringToString m_offPageConnectorNameMap;
   CEdif200Page* m_firstSchematicPage;
   CEdif200Symbol* m_currentSymbol;
   BlockStruct* m_contentBlock;
	bool m_isHierarchyStructure;
   int m_cloneCount;
	int m_derivedNumber;

	void reset();
   CString getNextCloneName();
   CString getUniquePageName(const CString pageName);
   void attachHierarchySymbolToPage(CEdif200Symbol& symbol);

	int doInterface();
   int doContents();
   int doOffPageConnector();

public:
	CTypedMapStringToPtrContainer<CAttributes*> m_portAttribMap; // might need to copy

	CString getName() const	{ return m_name;							}
   bool isHierarchyStructure() const { return m_isHierarchyStructure;    }
	void resetHierarchyStructure() { m_isHierarchyStructure = false; }
   bool isOffPageConnector(const CString portName); 
   bool isGlobalPort(const CString portName);
   CEdif200View* getCopy();
   BlockStruct* getContentBlock() { return m_contentBlock; }

	// page functions
	CEdif200Page* addPage(const CString pageName);
	CEdif200Page* findPage(const CString pageName);
	CEdif200Page* getFirstPage();
	CEdif200Page* getNextPage(POSITION& pos)
   {
      if (pos == NULL)
         return NULL;

	   CEdif200Page* page = m_pageMap.GetNext(pos);
	   return page;
   }
   POSITION getPageStartPosition() { return m_pageMap.GetHeadPosition(); }

	void setFirstSchematicPage(CEdif200Page* page)
   {
      if (m_firstSchematicPage == NULL)
         m_firstSchematicPage = page;
   }
   CEdif200Page* getFirstSchematicPage() { return m_firstSchematicPage;   }
	CString getDelimitedPageBlockNumber(const CString delimiter);

	CEdif200Symbol* addSymbol(const CString symbolName);
	CEdif200Symbol* findSymbol(const CString symbolName);
	CEdif200Symbol* getCurrentSymbol() { return m_currentSymbol;		}
	CEdif200Symbol* getNextSymbol(POSITION& pos)
   {
      if (pos == NULL)
         return NULL;

	   CEdif200Symbol* symbol = NULL;
	   CString key;
	   m_symbolMap.GetNextAssoc(pos, key, symbol);
	   return symbol;
   }
   POSITION getSymbolStartPosition() { return m_symbolMap.GetStartPosition(); }

   void convertLogicalSchematicToPhysicalSchematic();
   void removedAllPageBlocks();
   void createNetList();

// might need to remove
	void propagateNetsFromInstanceToHierarchyStructure(CEdif200Instance* instance);

   int getCloneCount() const                       { return m_cloneCount;           }
   int doView();
};

//-----------------------------------------------------------------------------
// CEdif200Cell
//-----------------------------------------------------------------------------
class CEdif200Cell
{
public:
	CEdif200Cell(CEdif200 &edif, const CString libraryName, const CString name);
	~CEdif200Cell();

private:
	CEdif200& m_edif;
	CString m_libraryName;
	CString m_name;	
   CString m_cellType;
	CTypedMapStringToPtrContainer<CEdif200View*> m_viewMap;
	CEdif200View* m_currentView;
	bool m_hasNonHierarchyStructure;

	void reset();
   int doViewMap();
   int doInstanceBackAnnotate();
   int doInstanceRef(CEdif200Instance** instance);

public:
   CString getName() const { return m_name; }
   CString getCellType() const { return m_cellType; }
   bool hasNonHierarchStructure() const { return m_hasNonHierarchyStructure;    }
 
	CEdif200View* addView(const CString viewName);
   CEdif200View* copyAndAddView(const CString viewName);
	CEdif200View* findView(const CString viewName);	
	CEdif200View* getFirstView();
   POSITION getViewStartPosition() { return m_viewMap.GetStartPosition(); }
   CEdif200View* getNextView(POSITION& pos)           
   {
      if (pos == NULL)
         return NULL;

      CEdif200View* view = NULL;
      CString key;
      m_viewMap.GetNextAssoc(pos, key, view);
      return view;
   }

	CEdif200View* getCurrentView() { return m_currentView; }

	CEdif200Page* getFirstPage();

   void convertLogicalSchematicToPhysicalSchematic();
   void removedUnusedOriginalViews();
   void createNetList();

// might need to be delete
	void propagateNetsFromInstanceToHierarchyStructure(CEdif200Instance* instance);

   int doCell();
};

//-----------------------------------------------------------------------------
// CEdif200Library
//-----------------------------------------------------------------------------
class CEdif200Library
{
public:
	CEdif200Library(CEdif200 &edif, const CString name);
	~CEdif200Library();

private:
	CEdif200& m_edif;
	CString m_name;
   CTypedMapStringToPtrContainer<Unit*> m_unitMap;
   CTypedMapStringToPtrContainer<FigureGroup*> m_figureGroupMap;
	CTypedMapStringToPtrContainer<CEdif200Cell*> m_cellMap;
	CEdif200Cell* m_currentCell;
	bool m_hasNonHierarchyStructure;

	void reset();
   int doTechnology();
   int doNumberDefinition();
   int doScale();
   
public:
	CString getName() const									{ return m_name;					}
   bool hasNonHierarchStructure() const { return m_hasNonHierarchyStructure;    }

	CEdif200Cell* addCell(const CString cellName);
	CEdif200Cell* addCell(CEdif200Cell* cell);
	CEdif200Cell* fineCell(const CString cellName);
	CEdif200Cell* getCurrentCell()							{ return m_currentCell;			}

	Unit* findUnit(const CString unitName);
	FigureGroup* findFigureGroup(const CString figureGroupName);

   void convertLogicalSchematicToPhysicalSchematic();
   void removedUnusedOriginalViews();
   void createNetList();

   int doLibrary();
};

//-----------------------------------------------------------------------------
// CEdif200
//-----------------------------------------------------------------------------
class CEdif200
{
public:
	CEdif200();
	~CEdif200();

	void reset();

private:
	CTypedMapStringToPtrContainer<CEdif200Library*> m_libraryMap;
	CEdif200Library* m_currentLibrary;

	// These two maps keep track and ensure that names of local net and bus name are unique among views
	// So if the same net or bus name is found on two different view; those net will have unique name.
	// The first occurrance will retain the original name; the second one will have "_n" append to it.
	CMapStringToString m_localUniqueNetNameMap;	
	CMapStringToString m_localUniqueBusNameMap;
	bool m_hasNonHierarchyStructure;

   int doDesign();

public:
	CEdif200Library* addLibrary(const CString libraryName);
	CEdif200Library* getCurrentLibrary() { return m_currentLibrary;			      }

	CEdif200Library* findLibrary(const CString libraryName);
	CEdif200Cell* findCell(const CString libraryName, const CString cellName);
	CEdif200View* findView(const CString libraryName, const CString cellName, const CString viewName);

   CEdif200Cell* getCurrentCell();
   CEdif200View* getCurrentView();
   CEdif200Symbol* getCurrentSymbol();

	CString getUniqueLocalNetName(const CString netName);
	CString getUniqueLocalBusName(const CString busName);

   void removedUnusedOriginalViews();
   void createNetList();

	int startEdif();
};


