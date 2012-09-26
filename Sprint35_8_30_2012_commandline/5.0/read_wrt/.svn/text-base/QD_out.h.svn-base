// $Header: /CAMCAD/4.6/read_wrt/QD_out.h 31    4/25/07 4:32p Lynn Phung $

/****************************************************************************/
/*  
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/           

#pragma once

#include "CamCadDatabase.h"
#include "afxwin.h"
#include "PrepDlg.h"

enum ERefPnt
{
	noneRefPnt					= 0,
	manualRefPnt				= 1,
	fiducialRefPnt				= 2,
};



enum EQdSurface
{
   qdSurfaceTop = 0,
   qdSurfaceBottom = 1,
};

//------------------------------------------------------------------------

class QDOutputAttrib
{
public:
	QDOutputAttrib(CCEtoODBDoc& camCadDoc);
	~QDOutputAttrib();

private:
	CCamCadDatabase m_camCadDatabase; 
	int m_attribKeyword;
	CMapStringToString m_valuesMap;

public:
	int GetKeyword() const;
	bool HasValue(CAttributes** attributes);
	void SetKeyword(const CString keyword);
	void AddValue(const CString value);
	void SetDefaultAttribAndValue();
};



//------------------------------------------------------------------------

class CQDOutFile
{
public:
   CQDOutFile();
   void Reset();

	int maxPanelLength;
	int maxPanelWidth;
	int panelThickness;
	int boardThickness;
	int panelFidSymbolNum;
	int boardFidSymbolNum;
	bool glueTop;
	bool glueBottom;
   CString boardIDPrefix;
   int boardIDStartNumber;
   int boardIDLength;
   CString topPanelLocationDefault;
   CString bottomPanelLocationDefault;
   int boardTolerance;
   int panelTolerance;
   bool nameFiles;
   bool use_unterseite;
   CString sequenceAttribName;
};

//------------------------------------------------------------------------

// This is purposely a NON-CONTAINER map.
// This ends up with ptrs to pcb insert data, where data is owned by
// CCZ Panel object. It is a temp list, not an owner, don't use a container class.

class QDPcbMap : public CTypedPtrMap<CMapStringToPtr,CString,DataStruct*>
{
};

//------------------------------------------------------------------------

class QDExport : public CObject
{
public:
	ExportFileSettings m_topPanelLocation;
	ExportFileSettings m_botPanelLocation;

public:
	QDExport(CCEtoODBDoc* Doc) : CObject()
		, m_outputAttrib(*Doc)
	{
		// Prep writer for exporting current file view
		m_doc = Doc;
		initializeLocalVariables();
	}

	~QDExport()
	{
	}

	void DoQDExport();

private: // structures
	struct Machine_Origin
	{
		double x;
		double y;
	};

	QDOutputAttrib m_outputAttrib;

private: // constants
	/* Define Section ***********************************************************/
	// These are German command that are used in the output file.  The Siemens
	// machines will read these commands.  Do not change it unless it it require.
	const char* QD_PCB_STRUCTURE                     ; // = "LP STRUKTUR"                   
	const char* QD_PASSIVE                           ; // = "PASSIV"				       
	const char* QD_COORDINATE                        ; // = "KOORDINATEN"		       
	const char* QD_COMMENT                           ; // = "KOMMENTAR"			       
	const char* QD_OFFSET                            ; // = "OFFSET"				       
	const char* QD_ANGLE                             ; // = "WINKEL"				       
	const char* QD_BASIS                             ; // = "BASIS"                  
	const char* QD_BASIS_LINK                        ; // = "BASIS LINK"				 
	const char* QD_PCB_DATA                          ; // = "LP DATEN"					 
	const char* QD_BOARD                             ; // = "NUTZEN"						 
	const char* QD_UPPERSIDE                         ; // = "OBERSEITE"					 
	const char* QD_UNDERSIDE                         ; // = "UNTERSEITE"				 
	const char* QD_PROCESSING                        ; // = "BEARBEITUNG"            
	const char* QD_INK_DOT_RECOGNITION               ; // = "INKPUNKT ERKENNUNG"     
	const char* QD_INK_DOT_DARK                      ; // = "INKPUNKT DUNKEL"			 
	const char* QD_INK_DOT_LIGHT                     ; // = "INKPUNKT HELL"			 
	const char* QD_WITH_INK_DOT_PLACEMENT            ; // = "BEI INKPUNKT BESTUECKEN"
	const char* QD_PCB_POSITION_RECOGNITION          ; // = "LP LAGE ERKENNUNG"      
	const char* QD_COMPONENT_POSITION_RECOGNITION    ; // = "BE LAGE ERKENNUNG"      
	const char* QD_BLOCK                             ; // = "SPERREN"                
	const char* QD_PLACEMENT                         ; // = "BESTUECKEN"				 
	const char* QD_GLUE                              ; // = "KLEBEN"						 
	const char* QD_SOLDER_PASTE_DISPENSER            ; // = "LOTPASTEN DISPENSOR"	 
	const char* QD_AFTER_PROCESSING                  ; // = "NACH BEARBEITUNG"		 
	const char* QD_LENGTH                            ; // = "LAENGE"						 
	const char* QD_WIDTH                             ; // = "BREITE"                 
	const char* QD_THICKNESS                         ; // = "DICKE"						 
	const char* QD_SOLDER_PASTE_THICKNESS            ; // = "LOTPASTEN DICKE"			 
	const char* QD_CORNER                            ; // = "ECKE"						 
	const char* QD_TOLERANCE                         ; // = "TOLERANZ"					 
	const char* QD_FIDUCIAL_SET_LIST                 ; // = "PM SET LISTE"				 
	const char* QD_FIDUCIAL_LIST                     ; // = "PM LISTE"               
	const char* QD_CODE                              ; // = "CODE"						 
	const char* QD_PCB_POSITION                      ; // = "LP POSITION"				 
	const char* QD_FIDUCIAL_SET_VALID                ; // = "PM SET GUELTIG"			 
	const char* QD_INK_DOT                           ; // = "INKPUNKT"					 
	const char* QD_FIDUCIAL_VALID                    ; // = "PM GUELTIG"				 
	const char* QD_PROGRAM_PLACEMENT                 ; // = "PROGRAMM BESTUECKEN"    
	const char* QD_COMPONENT                         ; // = "BE"
   const char* QD_PLACEMENT_SEQUENCE                ; // = "EBENE"
																										 
	// These are symbol that are used in the output file.								 
	const char* QD_LEFT_PARENTHESIS                  ; // = "("
	const char* QD_RIGHT_PARENTHESIS                 ; // = ")"
	const char* QD_LEFT_SQUARE_BRACKET               ; // = "["
	const char* QD_RIGHT_SQUARE_BRACKET              ; // = "]"
	const char* QD_LEFT_CURLY_BRACKET                ; // = "{"
	const char* QD_RIGHT_CURLY_BRACKET               ; // = "}"
	const char* QD_SIGNLE_QUOTE                      ; // = "'"
																			
																	  
private: // fields										  
	FILE *fp;
	FILE *flog;                                                   
	CCEtoODBDoc *m_doc;
	FormatStruct format;
	FileStruct *m_panelFile;
	QDPcbMap pcbDataList;
	CQDOutFile m_outFileSettings;
	CString indent;
	int indentCount;
	int display_error;
	CString headerLine;
	CString commentLine;
	CString PARTNUMBER_KEYWORD;
	CString Machine_Origin_Top;
	CString Machine_Origin_Bot;  
	Machine_Origin Machine_Top;
	Machine_Origin Machine_Bot;
	CString Machine_Origin_Top_Type;
	CString Machine_Origin_Bot_Type;
	CString ROTATION_OFFSET;
	CString XOFFSET;
	CString YOFFSET;
	bool XYOFFSET_ROTATION_BY_CENTROID;
	bool SUPPRESS_UNLOADED_COMPS;
	bool CADIF_ROTATION;
	BOOL SUPPRESS_BOARDINFO;
	BOOL FIRST_BOARD;
	BOOL LAST_BOARD;

private: // functions
	void LoadQDSettings();
   void LoadSettingFromAttribute();
   void SaveSettingToAttribute();
	void WritePCBStruct(EQdSurface surface);
	void WritePassive(EQdSurface surface, bool alreadyMirrored = false);
	void WriteCoordinate(CString comment, int xOffset, int yOffset, int rotation, EQdSurface surface);
	void WriteBasis(EQdSurface surface, CString basisID);
	void WritePassiveList(EQdSurface surface, CString passiveListID, CString basisListID, BOOL basisLink, DataStruct *pcbData);
	void WritePCBData(EQdSurface surface);
	void WriteBasisData(EQdSurface surface, CString basisDataID, BlockStruct* boardGeom, BOOL mirror, BOOL boardLevel);
	void WriteBoardDescription(EQdSurface surface, BlockStruct* boardGeom, BOOL mirror);
	void WriteFiducialSetList(EQdSurface surface, BlockStruct* boardGeom, BOOL mirror, BOOL panelFiducial, BOOL hasXOUT);
	void WriteFiducialList(CString fiducialID, DataStruct* fiducialData, BOOL mirror, EQdSurface surface, BOOL panelFiducial);
	void WriteComponentPlacement(EQdSurface surface, BlockStruct* boardGeom, BOOL mirror);
	void GetSize(BlockStruct *block, int graphicClass, double *xSize, double *ySize);
	void GetLowerLeftCorner(BlockStruct *block, int graphicClass, double *xMin, double *yMin);
	void GetPanelCorner(BlockStruct *block, int graphicClass, ECCorner corner, double *xMin, double *yMin);
	void IndentPlus(); 
	void IndentMinus();
	int NormalizeRotationQDOut(double rotation);
	int QD_Unit(double number);
	double Inverse_QD_Unit(int number);
	void Find_Machine_Origin();
	void CheckForXOUTandFiducial(EQdSurface surface, BlockStruct* boardGeom, bool& hasXOUT, bool& hasFiducial);
	
	//BOOL CheckForXOUT(BlockStruct *boardGeom);

	bool CreateLogFile(CString filePath, CString *logFile);
	bool GetExportDetails(CString *pathname);
	bool FileValidatedForExport();
	void initializeLocalVariables();

};
#pragma once



