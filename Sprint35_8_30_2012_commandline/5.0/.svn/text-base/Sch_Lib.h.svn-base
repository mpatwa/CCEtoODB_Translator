// $Header: /CAMCAD/4.6/Sch_Lib.h 21    4/12/07 3:34p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__Sch_Lib__h__)
#define __Sch_Lib__h__

#pragma once

#include "ccdoc.h"
#include "file.h"
#include "afxwin.h"

/* Function Prototypes *********************************************************/

void ConvertLogicToPhysicalSchematic(CCEtoODBDoc *docPtr, FileStruct *filePtr);
int CheckMissingOrDuplicateDesignator(CCEtoODBDoc *docPtr, FileStruct *filePtr, FILE *errorFile, long &errorCount);
void RemoveHierarchSheetAndComppin(CCEtoODBDoc *docPtr, FileStruct *filePtr, CMapStringToPtr *hierarchySheetToRemove,
			CMapStringToString *comppinToRemoveMap);
void AutoDetectSchematicDesignator(CCEtoODBDoc& camcadDoc, FileStruct& schFile, CString& symbolDesignatorKeyword, CString& portDesignatorKeyword, CStringArray& symbolDesignatorBackAnnotateArray, CStringArray& portDesignatorBackannotateArray);

void RenameCompAndPinDesignator(CCEtoODBDoc *doc, int fileNum, CString refdesAttrib, CString pinnoAttrib,
         CString refdesLocAttrib, CString pinnoLocAttrib);

void RenameCompAndPinDesignator(CCEtoODBDoc *doc, int fileNum, CStringArray& symbolDesignatorBackAnnotateArray, CString pinnoAttrib,
         CString refdesLocAttrib, CString pinnoLocAttrib);



/* Define Section *********************************************************/

// Delimiter used to in all Edif reader, Schematic Navigator, Schematic Link
#define SCH_DELIMITER               char(32)

// This is used to temperary store the port in the netlist
// The port refname is the comp, and SCH_PORT_COMPPIN is the pin
#define SCH_PORT_COMPPIN         "$$"

// These are Edif attribute keywords that are used very often
#define SCH_ATT_PARENTCELL          "PARENT_CELL"
#define SCH_ATT_PARENTLIBRARY       "PARENT_LIBRARY"
#define SCH_ATT_PARENTCLUSTER       "PARENT_CLUSTER"
#define SCH_ATT_REFDES_MAP          "REFDES_MAP"
#define SCH_ATT_PINNO_MAP           "PINNO_MAP"
#define SCH_ATT_DESIGNATOR          "Designator"
#define SCH_ATT_HIERARCHYGEOMNUM    "HierarchyGeomNum"
#define SCH_ATT_HIERARCHGEOM        "HierarchyGeom"
#define SCH_ATT_CELL                "Cell"
#define SCH_ATT_DIRECTION           "Direction"
#define SCH_ATT_INSTANCE            "Instance"
#define SCH_ATT_PORTNAME            "PortName"
#define SCH_ATT_DISPLAYNAME         "DisplayName"
#define SCH_ATT_UNUNSED             "Ununsed"
#define SCH_ATT_NETNAME             "NetName"
#define SCH_ATT_LIBRARYNAME         "LibraryName"
#define SCH_ATT_CELLNAME            "CellName"
#define SCH_ATT_CLUSTERNAME         "ClusterName"
#define SCH_ATT_INSTANCENAME        "InstanceName"
#define SCH_ATT_SECONDDESIGNATOR    "$Designator#2$"
#define SCH_ATT_COMPDESIGNATOR      "compDesignator"
#define SCH_ATT_PINDESIGNATOR       "pinDesignator"
#define SCH_ATT_PRIMARYNAME			"PrimaryName"
#define SCH_ATT_WIRERULE				"wire________RULE"	// There are exactly 8 underscore, cannot be more or less.
																			// This is given by Don Roman specifically coming from "mgsiwrite" Edif Writer

// These constants are used for indicating if token is a left or right parenthesis
#define SCH_PARENTHESIS_LEFT        0
#define SCH_PARENTHESIS_RIGHT       1 

// These constants are used for justification
#define SCH_JUSTIFY_LEFT            0
#define SCH_JUSTIFY_RIGHT           2
#define SCH_JUSTIFY_CENTER          1
#define SCH_JUSTIFY_BOTTOM          0
#define SCH_JUSTIFY_TOP             2

// These constants are used for indicating there are designator error
#define SCH_ERR_DESIGNATOR_OK				0
#define SCH_ERR_DESIGNATOR_BLANK			1
#define SCH_ERR_DESIGNATOR_DUPLICATE	2

// These numbers is use for calculating font size in points, and it is obtained from research not arbitrary picked
// 1 point(Postscript measurement) is approximately .3527777778 milimeter
#define SCH_MM_PER_POINT            .3527777778

// There are the layer name for Edif 200
#define SCH_LAYER_TEXT              "Text"
#define SCH_LAYER_PORT_DESIGNATOR   "PortDesignator"
#define SCH_LAYER_SYMBOL_DESIGNATOR "SymbolDesignator"
#define SCH_LAYER_PORT_GRAPHIC      "PortGraphic"
#define SCH_LAYER_SYMBOL_GRAPHIC    "SymbolGraphic"
#define SCH_LAYER_SIGNAL_GRAPHIC    "SignalGraphic"
#define SCH_LAYER_BORDER_GRAPHIC    "BorderGraphic"
#define SCH_LAYER_ATTRIBUTE         "Attribute"

enum EEdifProgram
{
   edifProgramC2esch,               // Edif 300&200 Files from this program will automatically exchange XY scale
   edifProgramMgsiwrite,            // Edif 300 files from this program will cause creation of bus derived name to all nets connected to bus 
   edifProgramUndefined,            // Default value
};

////////////////////////////////////////////////////////////////////////////////////////////
// CSchSelRefDes dialog
////////////////////////////////////////////////////////////////////////////////////////////
class CSchSelRefDes : public CDialog
{
   DECLARE_DYNAMIC(CSchSelRefDes)

public:
   CSchSelRefDes(CWnd* pParent = NULL);   // standard constructor
   virtual ~CSchSelRefDes();

// Dialog Data
   enum { IDD = IDD_SCHLINK_CHOOSEREFDES };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()
private:
   void fillAttributes();
   void fillValues();

public:
   BOOL forPcb;
   CCEtoODBDoc *docPtr;
   FileStruct *filePtr;
   CString newRefDesValue;
   BOOL m_significantAttrib;
   CListBox m_attribList;
   CListBox m_valueList;
   afx_msg void OnLbnSelchangeAttribList();
   afx_msg void OnBnClickedSignificantattrib();
   CString m_messageLabel;
   CString m_titleLabel;
   CString dialogCaption;
   afx_msg void OnBnClickedOk();
};

///////////////////////////////////////////////////////////////
// CSchSelAttributes dialog                                  //
///////////////////////////////////////////////////////////////

class CSchSelAttributes : public CDialog
{
   DECLARE_DYNAMIC(CSchSelAttributes)

public:
   CSchSelAttributes(CWnd* pParent = NULL);   // standard constructor
   virtual ~CSchSelAttributes();

// Dialog Data
   enum { IDD = IDD_SCH_CHOOSEATTRIB };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()

private:
   void fillAttribValueMaps();
   void fillAttribListboxes();
   void fillValueListboxes(BOOL fillRefdes);
   void freeAttribValueMap();

public:
   CMapWordToPtr refdesAttribValueMap;
   CMapWordToPtr pinnoAttribValueMap;
   CCEtoODBDoc *docPtr;
   FileStruct *filePtr;
   CString newRefDesAttrib;
   CString newPinnoAttrib;

   CListBox m_refdesAttribList;
   CListBox m_pinnoAttribList;
   BOOL m_uniqueAttrib;
   afx_msg void OnLbnSelchangeRefdesAttribList();
   afx_msg void OnLbnSelchangePinnoAttribList();
   afx_msg void OnBnClickedOk();
   afx_msg void OnBnClickedUniqueattrib();
   CListBox m_refdesValueList;
   CListBox m_pinnoValueList;
};

//---------------------------------------------------------------------------------------
// KeywordValues
//---------------------------------------------------------------------------------------
enum KeywordType
{
   keywordTypeAlpha,
   keywordTypeAlphaNumeric,
   keywordTypeNumeric,
   keywordTypeNumericAlpha,
   keywordTypeSymbol,
   keywordTypeKeyword,
   keywordTypeUnknown,
};

class KeywordValues
{
public:
   KeywordValues(const int keywordIndex, const CString name);
   ~KeywordValues();

private:
   int m_keywordIndex;
   CString m_name;
   CMapStringToString m_originalValueMap;
   CMapStringToInt m_valueCountMap;
   int m_usageCount;

public:
   //CString getName() const { return m_name; }
   int getKeywordIndex() const { return m_keywordIndex; }
   int getUsageCount() const { return m_usageCount; }

   void addValue(const CString value);
   int getUniquePercentage();
   bool checkValidLogicalSymbolDesignatorValue();
   bool checkValidPortInstanceDesignatorValue();
};

//---------------------------------------------------------------------------------------
// UniqueKeywordMap
//---------------------------------------------------------------------------------------
class UniqueDesignatorKeywordMap
{
public:
   UniqueDesignatorKeywordMap();
   ~UniqueDesignatorKeywordMap();

private:
   CTypedMapIntToPtrContainer<KeywordValues*> m_uniqueKeywordMap;

public:
   void addKeywords(CCEtoODBDoc& camcadDoc, CAttributes& attributes);
   KeywordValues* lookup(int keywordIndex);
   CString getLogicalSymbolDesignatorKeyword(CCEtoODBDoc& camcadDoc, const int logicalSymbolCount);
   void getPortInstanceDesignatorKeyword(CCEtoODBDoc& camcadDoc, CMapStringToString& uniqueDesignatorMap);
};


#endif // __Sch_Lib__h__
