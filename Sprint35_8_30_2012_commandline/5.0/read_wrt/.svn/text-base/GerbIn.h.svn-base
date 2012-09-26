// $Header: /CAMCAD/4.6/read_wrt/GerbIn.h 19    6/01/07 6:55a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__GerbIn_h__)
#define __GerbIn_h__

#pragma once

#include "TypedContainer.h"


#define  MAX_MACROCOMMAND  200000   // max 2 hundred thousand individual commands per macro -> crazy // but valor does it.

#define  MAX_LINE          8000     /* Max line length */
#define  MAX_MACRO         10000    // max 9999 apertures
#define  MAX_AP_RENAME     10000    

#define  PENUNDEFINED      0
#define  PENDOWN           1
#define  PENUP             2
#define  PENFLASH          3

// macro operations
#define  OP_ASSIGN         0
#define  OP_ADD            1
#define  OP_MINUS          2
#define  OP_DIV            3
#define  OP_MUL            4

// G4 comment type
#define  G4TYPE_UNKNOWN    0
#define  G4TYPE_GERBTOOL   1

#define  BLOCKENDCHAR      (126)    // high signed char as a placeholder for * etc..

#define newMacroProcessing

enum PolygonApertureMacroOriginTypeTag
{
   polygonApertureMacroOriginFromCircumCircle,
   polygonApertureMacroOriginFromExtent,
};

enum PolygonApertureMacroDiameterTypeTag
{
   polygonApertureMacroDiameterFromCircumCircle,
   polygonApertureMacroODiameterFromInCircle,
   polygonApertureMacroDiameterFromExtentWidth,
};

//enum PolygonApertureMacroOffsetAngleTypeTag
//{
//   polygonApertureMacroOffsetAngleZero,
//   polygonApertureMacroOffsetAnglePiOverN,
//};

//_____________________________________________________________________________
typedef struct
{
   int equation;        // the equation $1=$1-$2 is split in
   CString left;        // $1
   CString right;       // $1-$2
} GerbMStruct; // macro struct
typedef CTypedPtrArray<CPtrArray, GerbMStruct*> MArray;

//_____________________________________________________________________________
class CApertureMacro
{
private:
   CString m_macroString;
   CMapStringToString m_symbolTable;
   CStringArray m_statements;
   CStringArray m_primitives;

public:
   CApertureMacro(const CString& macroString);

   CString getCleanMacroString();

private:
   void parse();
   CString evaluateExpression(const CString& expression,const CString& variableName=CString());
};

//_____________________________________________________________________________

typedef struct
{
   int   filenum;    // there can be same macro names in multiple files with multiple meaning
   char  *name;
   char  *command;
} AMacro;

//typedef struct       // ap_rename structure is used, if 2 or more gerber files have
//                     // same dcode and different values. Happens only on Gerber_X files
//{
//   CString m_name;
//   CString m_originalName;
//   //char  *name;      // new name
//   //char  *orig;      // original name
//} Ap_Rename;

class CApertureRenameEntry
{
private:
   CString m_originalName;
   CString m_name;

public:
   CApertureRenameEntry(const CString& name,const CString& originalName);

   CString getOriginalName() const { return m_originalName; }
   void setOriginalName(const CString& name) { m_originalName = name; }
   CString getName() const { return m_name; }
   void setName(const CString& name) { m_name = name; }
};

class CApertureRenameTable 
{
private:
   CTypedPtrListContainer<CApertureRenameEntry*> m_entries;

public:
   void empty() { m_entries.empty(); }

   bool renameAperture(const CString& newApertureName,const CString& originalApertureName);
   CString getRenamedApertureName(const CString& apertureName);
};

typedef struct  // gerber point with bulge
{
   double x;
   double y;
   double bulge;
} GPoint;
typedef CTypedPtrArray<CPtrArray, GPoint*> GPointArray;

typedef struct
{
   double   cur_x,cur_y;
   double   old_x,old_y;
   double   cur_i,cur_j;
   int      lpd;        // true is dark, false is lpc
   int      cur_gcode;
   int      cur_app;
   int      cur_widthindex;
   int      vor_komma,nach_komma;
   int      absolute;
   int      drawmode;   /* 1 = pendown */
                        /* 2 = penup */
                        /* 3 = flash */
                        /* 4 = used as vias */
   int      G36fill;       // Gerber autofill
   int      arc_interpolation, tmp_arc_interpolation; // tmp_arc can temporaryly suspend an Arc.
   int      counterclock;
   int      G75code;
   int      GerberX;        // read applist inside gerber file.
   int      Fire9000;
   int      layernum;
   char     done;
   char     libmode;
   int      zeroleading;      // 1 is trailing zeros, 0 = leading zeros supression
   int      apr_units;
   long     sequencecnt;
   int      lpcnt;            // counts the number of LPC or LPD commands 
   int      width0index;
   int      parstart, apstart;
} GerberLocal;

//_____________________________________________________________________________
class CCoordinateBreakPoint
{
private:
   static CExtent m_extent;
   static double m_searchTolerance;

public:
   static void setExtent(double x0,double y0,double x1,double y1);
   static void setSearchTolerance(double searchTolerance);
   static void setSearchPoint(double x,double y);
   static void setSearchPoint(double x,double y,double searchTolerance);
   static bool breakOnCoordinate(double x,double y);
};

//_____________________________________________________________________________
//class CGerberFile
//{
//private:
//   CString m_filePath;
//   CStdioFile m_file;
//
//public:
//   CGerberFile();
//
//   bool open(const CString& filePath);
//   void rewind();
//};

//_____________________________________________________________________________
class CGerberMacroCommand
{
private:
   CString m_command;
   CStringArray m_params;
   CDoubleArray m_variableParams;
   CDoubleArray m_paramValues;

public:
   CGerberMacroCommand();
   CGerberMacroCommand(const CString& command);

   CString getCommand() const { return m_command; }

   void setVariableParams(CDoubleArray& variableParams);
   int getCount() const;
   int getStandardParamCount();
   int getPrimitive() { return round(getAt(0)); }
   bool getOnFlag()   { return (round(getAt(1)) != 0); }
   bool getOffFlag()  { return (round(getAt(1)) == 0); }
   double getAt(int index);
   void setAt(int index,double value);
};

//_____________________________________________________________________________
class CGerberMacroCommands
{
private:
   CTypedPtrArrayContainer<CGerberMacroCommand*> m_commands;
   CDoubleArray m_commandArray;

public:
   CGerberMacroCommands(const CString& commands);
   void empty() { m_commands.empty(); }

   int getCount() const { return m_commands.GetCount(); }
   CGerberMacroCommand* getAt(int index);
   
   CDoubleArray& getCommandParameters();
};

#ifdef newMacroProcessing

//_____________________________________________________________________________
class CGerberMacroSymbolTable
{
private:
   CMapStringToString m_symbolTable;

public:
   CGerberMacroSymbolTable(const CStringArray& symbols);

   void expand(CString& expressionValue);
   void setAt(const CString& name,double value);
   bool assign(const CString& name,const CString& expression);
   int getIntResult(const CString& expression);
   double getDoubleResult(const CString& expression);
   CString getIntResultString(const CString& expression);
   CString getDoubleResultString(const CString& expression);
};

//_____________________________________________________________________________
enum GerberMacroStatementTypeTag
{
   gerberMacroStatementTypeCirclePrimitive,
   gerberMacroStatementTypeLinePrimitive,
   gerberMacroStatementTypeCenteredRectanglePrimitive,
   gerberMacroStatementTypeCorneredRectanglePrimitive,
   gerberMacroStatementTypeOutlinePrimitive,
   gerberMacroStatementTypePolygonPrimitive,
   gerberMacroStatementTypeMoirePrimitive,
   gerberMacroStatementTypeThermalPrimitive,
   gerberMacroStatementTypeExpression,
   gerberMacroStatementTypeUndefined
};

//_____________________________________________________________________________
class CGerberMacroStatement
{
protected:
   CStringArray m_parameters;
   CString m_statement;

public:
   CGerberMacroStatement(const CStringArray& parameters,const CString& statement);

   CString getStringAt(int index);

   virtual GerberMacroStatementTypeTag getType() const = 0;
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable) = 0;
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable) = 0;

   static CString getApertureDefinitionParameterDelimeter();
   static CString getApertureDefinitionDelimeter();
};

//_____________________________________________________________________________
class CGerberMacroPrimitive : public CGerberMacroStatement
{
public:
   CGerberMacroPrimitive(const CStringArray& parameters,const CString& statement);

   bool getOnFlag() const;
   bool getOffFlag() const;

   //virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroCirclePrimitive : public CGerberMacroPrimitive
{
public:
   CGerberMacroCirclePrimitive(const CStringArray& parameters,const CString& statement);

   int getDiameter() const;
   CPoint getCenter() const;

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeCirclePrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroLinePrimitive : public CGerberMacroPrimitive
{
private:
   int    m_exposure;
   double m_lineWidth;
   double m_xStart;
   double m_yStart;
   double m_xEnd;
   double m_yEnd;
   double m_degrees ;

public:
   CGerberMacroLinePrimitive(const CStringArray& parameters,const CString& statement);

   int getWidth() const;
   CPoint getStartPoint() const;
   CPoint getEndPoint() const;

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeLinePrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroCenteredRectanglePrimitive : public CGerberMacroPrimitive
{
public:
   CGerberMacroCenteredRectanglePrimitive(const CStringArray& parameters,const CString& statement);

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeCenteredRectanglePrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroCorneredRectanglePrimitive : public CGerberMacroPrimitive
{
public:
   CGerberMacroCorneredRectanglePrimitive(const CStringArray& parameters,const CString& statement);

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeCorneredRectanglePrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroOutlinePrimitive : public CGerberMacroPrimitive
{
public:
   CGerberMacroOutlinePrimitive(const CStringArray& parameters,const CString& statement);

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeOutlinePrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroPolygonPrimitive : public CGerberMacroPrimitive
{
public:
   CGerberMacroPolygonPrimitive(const CStringArray& parameters,const CString& statement);

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypePolygonPrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroMoirePrimitive : public CGerberMacroPrimitive
{
public:
   CGerberMacroMoirePrimitive(const CStringArray& parameters,const CString& statement);

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeMoirePrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroThermalPrimitive : public CGerberMacroPrimitive
{
public:
   CGerberMacroThermalPrimitive(const CStringArray& parameters,const CString& statement);

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeThermalPrimitive; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacroExpression : public CGerberMacroStatement
{
public:
   CGerberMacroExpression(const CStringArray& parameters,const CString& statement);

   virtual GerberMacroStatementTypeTag getType() const { return gerberMacroStatementTypeExpression; }
   virtual void evaluate(CGerberMacroSymbolTable& symbolTable);
   virtual CString getStatement(CGerberMacroSymbolTable& symbolTable);
};

//_____________________________________________________________________________
class CGerberMacro
{
private:
   CString m_name;
   CTypedPtrListContainer<CGerberMacroStatement*> m_statements;

public:
   CGerberMacro(const CString& macroDefinitionString);

   CString getName() const { return m_name; }

   void instantiateMacro(CCamCadDatabase& camCadDatabase,CGerberMacroSymbolTable& symbolTable,int fileNumber,int dcode) const;
   CString getMacroString(CCamCadDatabase& camCadDatabase,CGerberMacroSymbolTable& symbolTable) const;
   //bool instantiateOblongAperture(CCamCadDatabase& camCadDatabase,CGerberMacroCirclePrimitive& circle1,CGerberMacroCirclePrimitive& circle2,CGerberMacroLinePrimitive& line1);
   //bool instantiateDonutAperture(CCamCadDatabase& camCadDatabase,CGerberMacroCirclePrimitive& circle1,CGerberMacroCirclePrimitive& circle2);
   //bool instantiateThermalAperture(CCamCadDatabase& camCadDatabase,CGerberMacroCirclePrimitive& circle1,CGerberMacroCirclePrimitive& circle2,CGerberMacroLinePrimitive& line1,CGerberMacroLinePrimitive& line2);
   BlockStruct* instantiatePrimitiveAperture(CCamCadDatabase& camCadDatabase,int fileNumber,CGerberMacroStatement& gerberMacroStatement,CBasesVector& apertureBasesVector) const;

private:
   CGerberMacroStatement& addStatement(const CString& statementString);

};

//_____________________________________________________________________________
class CGerberMacroMap
{
private:
   CTypedMapStringToPtrContainer<CGerberMacro*> m_map;

public:
   CGerberMacro& getDefinedMacro(const CString& macroName);
   CGerberMacro* getMacro(const CString& macroName);
   bool hasMacro(const CString& macroName);
   //void addMacro(const CString& macroName,const CString& macroDefinition);
   CGerberMacro* addMacroDefinition(const CString& macroDefinition);
};

//_____________________________________________________________________________
class CGerberMacros
{
private:
   static CGerberMacros* m_gerberMacros;
   static CCamCadDatabase* m_allocatedCamCadDatabase;

   CCamCadDatabase& m_camCadDatabase;
   CTypedPtrArrayContainer<CGerberMacroMap*> m_array;

public:
   CGerberMacros(CCamCadDatabase& camCadDatabase);
   void empty();

   CGerberMacro* addMacroString(const CString& macroString,int fileIndex);
   void instantiateMacros(const CString& definitionsString,int fileIndex);
   CString getMacrosDefinitions(const CString& definitionsString,int fileIndex);
   CString getMacrosPrimitiveStrings(const CString& definitionsString,int fileIndex);
   CString getMacroPrimitivesString(const CString& definitionString,int fileIndex,bool& standardApertureFlag);

   static CGerberMacros& getGerberMacros();
   static CGerberMacros& getGerberMacros(CCamCadDatabase& camCadDatabase);
   static void releaseGerberMacros();

private:
   CGerberMacro& getDefinedMacro(const CString& macroName,int fileIndex);
   void instantiateMacro(const CString& definitionString,int fileIndex);
   void instantiateCircleStandardAperture(        const CStringArray& modifiers,int dcode) const;
   void instantiateRectangleStandardAperture(     const CStringArray& modifiers,int dcode) const;
   void instantiateObroundStandardAperture(       const CStringArray& modifiers,int dcode) const;
   void instantiateRegularPolygonStandardAperture(const CStringArray& modifiers,int dcode) const;

};

#endif  // #ifdef newMacroProcessing

//_____________________________________________________________________________

#endif
