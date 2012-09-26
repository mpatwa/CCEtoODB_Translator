// $Header: /CAMCAD/5.0/Net_Util.h 17    5/22/07 6:32p Rick Faltersack $
     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#pragma once

#include "file.h"
#include "RwLib.h"  // for COperationProgress

NetStruct *FindNet(FileStruct *file, CString netname);
NetStruct *FindNet(CCEtoODBDoc *doc, FileStruct *file, CString attribName, CString searchValue);
BOOL RemoveCompPin(FileStruct *file, CString comp, CString pin);
BOOL RemoveCompPins(FileStruct *file, CString comp);
BOOL RemoveNet(FileStruct *file,  NetStruct *Net);
BOOL FindAndMoveCompPin(FileStruct *file, CString comp, CString pin, NetStruct *newNet);
BOOL MoveCompPin(NetStruct *oldNet, NetStruct *newNet, CompPinStruct *cp);
CompPinStruct *FindCompPin(FileStruct *file, CString comp, CString pin, NetStruct **net);

// this is the name for a single pin net
CString GenerateSinglePinNetname(const char *comp, const char *pin);
int ExplodeNcPins(CCEtoODBDoc *doc, FileStruct *file, COperationProgress* progress = NULL);
void MergeNcPins(CCEtoODBDoc *doc, FileStruct *file);

class CNonConnectedPin;
class CNonConnectedPoly;
class CNonConnectedVia;


#ifdef NOT_USED__BUT_MIGHT_BE_LATER_SO_SAVE_AWHILE
//-------------------------------------------------------------------------------------

class CSchCompPin
{
private:
   CString m_refname;
   CString m_pinname;

public:
   CSchCompPin(CString refname, CString pinname) { m_refname = refname; m_pinname = pinname; }

   CString GetRefName()    { return m_refname; }
   CString GetPinName()    { return m_pinname; }
};

class CSchCompPinList : public CTypedPtrArrayContainer<CSchCompPin*>
{
};

//-------------------------------------------------------------------------------------

class CSchNet
{
private:
   CString m_netname;
   CSchCompPinList m_compPinList;

public:
   CSchNet(CString netname)   { m_netname = netname; }

   void AddCompPin(CSchCompPin *schCompPin)             { m_compPinList.Add(schCompPin); }
   void AddCompPin(CString refname, CString pinname)    { AddCompPin( new CSchCompPin(refname, pinname) ); }

   int GetCompPinCount()               { return m_compPinList.GetCount(); }
   CSchCompPin *GetCompPinAt(int i)    { return ((i > 0 && i < GetCompPinCount()) ? m_compPinList.GetAt(i) : NULL); }

};

class CSchNetList : public CTypedPtrArrayContainer<CSchNet*>
{
};

#endif // NOT_USED_...

//-------------------------------------------------------------------------------------

class CSchematicBuilder
{
private:
   FILE *m_logfp;
   CString m_logFilename;
   int m_nextSheetNumber;

   // On each CompPin in file place standard Schematic attributes compDesignator and]
   // pinDesignator. Attribs used by (required by) Schematic Navigator and visECAD.
   void GenerateCompDesAndPinDesAttribs(CCamCadData &ccData, FileStruct *file);
   
   // Get Pin aka Port graphic block, define it if not already present.
   BlockStruct *GetDefinedPortBlock(CCamCadDatabase &ccdb, FileStruct *file, double segmentLen, double &portLen);

   // Get the graphic block for component symbol with given pin count.
   BlockStruct *GenerateSchematicSymbol(CCamCadDatabase &ccdb, FileStruct *file, CString symbolName, int pinCount);

   // Create signal (aka trace) graphics for nets attached to comppins on symData.
   void GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file, CDataList &dataList, DataStruct *symData);

   // Create signal (aka trace) graphics for nets attached to comppins on symData.
   void GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file, BlockStruct *sheetBlk, DataStruct *symData);

   // Create signal (aka trace) graphics for nets attached to comppins.
   void GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file, BlockStruct *sheetBlk);

   // Create signal (aka trace) graphics for nets attached to comppins for all sheets in doc.
   void GenerateSignalTraces(CCamCadDatabase &ccdb, FileStruct *file);

   // Yet another incarnation of GetOutlineSize  (see, e.g. QD_Out.cpp)
   void GetOutlineSize(CCamCadData &ccData, BlockStruct *block, int graphicClass, double *xSize, double *ySize);
   
   // Place the symbols, draw the traces, etc., for NetList currently defined in file.
   void GenerateSchematicGraphics(CCEtoODBDoc &doc, CCamCadData &ccData, FileStruct *file);
   
   // Define layers used in schematic graphics/data.
   void DefineSchematicLayers(CCamCadData &ccData);

   // Get the next new sheet name
   CString GetNextNewSheetName();

   // Get the next new sheet
   BlockStruct *GetNextNewSheet(CCamCadDatabase &ccdb, CCEtoODBDoc &doc, FileStruct *file);

   // Create symbol insert
   DataStruct *PlaceSymbol(CCamCadDatabase &ccdb, BlockStruct *sheetBlk, BlockStruct *symbolBlk, CString refname, double symX, double symY, int schematicLayer);
   DataStruct *PlaceSymbol(CCamCadDatabase &ccdb,  CDataList  &dataList, BlockStruct *symbolBlk, CString refname, double symX, double symY, int schematicLayer);
   
   // Place column of symbol inserts into sheet with no overlap
   bool PlaceColumnInSheet(CCamCadDatabase &ccdb, FileStruct *file, BlockStruct **sheetBlk, CDataList &columnDataList);

   // Copy attributes from source to destination, covers situation where source and destination
   // belong to different CCamCadData object, in which case the CAttributes->CopyAll() will not work correctly.
   void CopyAttributes(CCEtoODBDoc &destDoc, CAttributes &destAttribs, CAttributes &sourceAttribs);

   
public:
   CSchematicBuilder();
   ~CSchematicBuilder();

   // Define a new CCEtoODBDoc, create a file of type Sheet, return doc and file ptrs
   CCEtoODBDoc *GetNewSchematicCczDoc(CString schematicCczFileName, FileStruct **schematicFile);

   // Create a new schematic file within given doc
   FileStruct *GetNewSchematicFile(CCEtoODBDoc *doc);

   void GenerateSchematic(CCEtoODBDoc *doc, FileStruct *selectedFile);

   // Generate complete new drawing: new ccz document with filename
   CCEtoODBDoc *GenerateSchematicDrawing(CString schematicCczFilename, CNetList *sourceNetList);

   // Generate schematic content in given camcad doc
   FileStruct *GenerateSchematicDrawing(CCEtoODBDoc *doc, FileStruct *schematicFile, CNetList *sourceNetlist);

   void CloseLogFile()           { if (m_logfp != NULL) fclose(m_logfp); m_logfp = NULL; }
   CString GetLogFileName()      { return m_logFilename; }

   // Create netlist in doc/file that matches the one defined in sourceNetList.
   int  CreateSchematicNetList(CCEtoODBDoc *doc, FileStruct *file, CNetList *sourceNetList);

   // Scan netlist for CompPins, tally the refnames and pincount into componentList.
   void DetermineComponentList(CCamCadData &ccData, FileStruct *file, CMapStringToInt &componentList);

   // Get rid of 2nd to nth blocks of type Schematic Sheet.
   // Keep the file's block, but purge its datalist.
   void PurgeSchematicSheets(CCEtoODBDoc *doc, FileStruct *file);

};

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

class CNonConnectedPinList : public CTypedPtrListContainer<CNonConnectedPin*>
{
public:
   CNonConnectedPinList(bool isContainer=true) : CTypedPtrListContainer<CNonConnectedPin*>(isContainer)
   {
   }
   ~CNonConnectedPinList()
   {
      empty();
   }
};

class CNonConnectedPolyList : public CTypedPtrListContainer<CNonConnectedPoly*>
{
public:
   CNonConnectedPolyList(bool isContainer=true) : CTypedPtrListContainer<CNonConnectedPoly*>(isContainer)
   {
   }
   ~CNonConnectedPolyList() 
   {
      empty();
   }
};

class CNonConnectedViaList : public CTypedPtrListContainer<CNonConnectedVia*>
{
public:
   CNonConnectedViaList(bool isContainer=true) : CTypedPtrListContainer<CNonConnectedVia*>(isContainer)
   {
   }
   ~CNonConnectedViaList()
   {
      empty();
   }
};

//-----------------------------------------------------------------------------
// CNonConnectedPin
//-----------------------------------------------------------------------------
class CNonConnectedPin
{
public:
   CNonConnectedPin(CCamCadDatabase& camcadDatabase, CompPinStruct* compPin);
   ~CNonConnectedPin();

private:
   CCamCadDatabase& m_camcadDatabase;
   CString m_netName;
   CompPinStruct* m_compPin;
   CNonConnectedPolyList m_polyList;
   CArray<int, int> m_padstackLayerIndexArray;

   void fillPadstackLayerIndexArray();

public:
   CString getCompName() const { return m_compPin==NULL?"":m_compPin->getRefDes(); }
   CString getPinName() const { return m_compPin==NULL?"":m_compPin->getPinName(); }
   CPoint2d getOrigin() const { return m_compPin==NULL?CPoint2d(0.0, 0.0):m_compPin->getOrigin(); }
   CString getNetName() const { return m_netName; }
   NetStruct* setNetName(const CString netName, FileStruct* pcbFile);
   long getEntityNumber() const { return m_compPin==NULL?0:m_compPin->getEntityNumber(); }

   bool isDone() const;
   bool isLayerInPadstack(int layerIndex);
   void addNonConnectedPoly(CNonConnectedPoly* nonConnectedPoly);
};

//-----------------------------------------------------------------------------
// CNonConnectedVia
//-----------------------------------------------------------------------------
class CNonConnectedVia
{
public:
   CNonConnectedVia(CCamCadDatabase& camcadDatabase, DataStruct* viaData);
   ~CNonConnectedVia();

private:
   CCamCadDatabase& m_camcadDatabase;
   CString m_netName;
   DataStruct* m_viaData;
   CNonConnectedPolyList m_polyList;
   CArray<int, int> m_padstackLayerIndexArray;

public:
   CString getNetName() const { return m_netName; }
   void setNetName(const CString netName, FileStruct* pcbFile);
   CPoint2d getOrigin() const { return m_viaData==NULL?CPoint2d(0.0, 0.0):m_viaData->getInsert()->getOrigin2d(); }

   bool isDone() const;
   bool isLayerInPadstack(int layerIndex);
   void addNonConnectedPoly(CNonConnectedPoly* nonConnectedPoly);
};

//-----------------------------------------------------------------------------
// CNonConnectedPoly
//-----------------------------------------------------------------------------
class CNonConnectedPoly
{
public:
   CNonConnectedPoly(CCamCadDatabase& camcadDatabase, DataStruct* polyData);
   ~CNonConnectedPoly();

private:
   CCamCadDatabase& m_camcadDatabase;
   CString m_netName;
   DataStruct* m_polyData;
   CNonConnectedPinList m_pinList;
   CNonConnectedViaList m_viaList;

public:
   CString getNetName() const { return m_netName; }
   void setNetName(const CString netName, FileStruct* pcbFile);

   bool isDone() const;
   void addNonConnectedPin(CNonConnectedPin* nonConnectedPin) { m_pinList.AddTail(nonConnectedPin); }
   void addNonConnectedVia(CNonConnectedVia* nonConnectedVia) { m_viaList.AddTail(nonConnectedVia); }
};

// end net_util.h