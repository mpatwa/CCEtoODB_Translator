// $Header: /CAMCAD/4.5/read_wrt/FabmasterFATFWriter.h    4/03/06 12:47p Rick Faltersack $

/*****************************************************************************/

#if !defined(__FabmasterFATFWriter_h__)
#define __FabmasterFATFWriter_h__

#pragma once

//#include "ccdoc.h"
//#include "TypedContainer.h"

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfLayer : public CObject
{
private:
   int   m_fatfLayerId;
   CString m_fatfLayerType;
   LayerStruct *m_camcadLayer;

public:
   CFatfLayer(int fatfLayerId, LayerStruct *camcadLayer)  { m_fatfLayerId = fatfLayerId; m_camcadLayer = camcadLayer; }

   int GetFatfLayerId()               { return m_fatfLayerId; }
   void SetFatfLayerId(int layerId)   { m_fatfLayerId = layerId; }
   
   void SetFatfLayerType(CString type) { m_fatfLayerType = type; }

   LayerStruct *GetCamcadLayerData()  { return m_camcadLayer; }

   CString GetFatfLayerSense(int maxElectStackNum);
   CString GetFatfLayerType();
};

class CFatfLayerMap : public CMapSortedStringToOb<CFatfLayer>
{
private:
   //static int AscendingRefnameSortFunc(const void *a, const void *b);
   CCEtoODBDoc *m_doc;
   int m_maxElectricalStackNum;

public:
   void CollectLayers(CCEtoODBDoc *doc, FileStruct *pcbFile);
   int GetFatfMirrorLayerId(CFatfLayer *fatfLayer);
   int GetMaxElectricalStackNum()   { return m_maxElectricalStackNum; }
};


////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfPart : public CObject
{
private:
   int   m_fatfPartId;
   DataStruct *m_camcadPartData;

public:
   CFatfPart(int fatfPartId, DataStruct *camcadPartData)  { m_fatfPartId = fatfPartId; m_camcadPartData = camcadPartData; }

   int GetFatfPartId()              { return m_fatfPartId; }
   void SetFatfPartId(int partId)   { m_fatfPartId = partId; }

   DataStruct *GetCamcadPartData()  { return m_camcadPartData; }

   CString GetDeviceName(CCEtoODBDoc *doc);
   CString GetInsertedBlockNumberAsString();
   CString GetInsertedGeometryName(CCEtoODBDoc *doc); //*rcf May Be Obsolete, delete at end if not used

};

class CFatfPartMap : public CMapSortedStringToOb<CFatfPart>
{
private:
   static int AscendingRefnameSortFunc(const void *a, const void *b);

public:
   void CollectParts(CCEtoODBDoc *doc, FileStruct *pcbFile);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfPin : public CObject
{
private:
   int   m_fatfPinId;
   DataStruct *m_camcadPinData;

public:
   CFatfPin(int fatfPinId, DataStruct *camcadPinData)  { m_fatfPinId = fatfPinId; m_camcadPinData = camcadPinData; }

   int GetFatfPinId()             { return m_fatfPinId; }
   void SetFatfPinId(int pinId)   { m_fatfPinId = pinId; }

   DataStruct *GetCamcadPinData()  { return m_camcadPinData; }

   CString GetInsertedBlockNumberAsString();
   CString GetMountTechnology(CCEtoODBDoc *doc);
   bool IsThruHole(CCEtoODBDoc *doc);

};

class CFatfPinMap : public CMapSortedStringToOb<CFatfPin>
{
private:
   static int AscendingNameSortFunc(const void *a, const void *b);

public:
   void CollectPins(CCEtoODBDoc *doc, FileStruct *pcbFile, BlockStruct *compGeomBlk);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfPadSymbol : public CObject
{
private:
   int   m_fatfPadSymId;
   BlockStruct *m_camcadAperture;


public:
   CFatfPadSymbol(int fatfPadSymId, BlockStruct *camcadAperture)  { m_fatfPadSymId = fatfPadSymId; m_camcadAperture = camcadAperture; }

   int GetFatfPadSymbolId()                { return m_fatfPadSymId; }
   void SetFatfPadSymbolId(int padsymId)   { m_fatfPadSymId = padsymId; }

   BlockStruct *GetCamcadAperture()  { return m_camcadAperture; }
};

class CFatfPadSymbolMap : public CMapSortedStringToOb<CFatfPadSymbol>
{
private:
   static int AscendingNameSortFunc(const void *a, const void *b);

public:
   void CollectPadSymbols(CCEtoODBDoc *doc, FileStruct *pcbFile);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfPackage : public CObject
{
private:
   int   m_fatfPkgId;
   BlockStruct *m_camcadGeometryBlock;
   CFatfPinMap m_pinMap;
   CString m_fatfPkgName; // Camcad block names are not necessarily unique, these package names are unique

public:
   CFatfPackage(int fatfPkgId, BlockStruct *camcadGeomBlk)  { m_fatfPkgId = fatfPkgId; m_camcadGeometryBlock = camcadGeomBlk; }

   int GetFatfPkgId()             { return m_fatfPkgId; }
   void SetFatfPkgId(int pkgId)   { m_fatfPkgId = pkgId; }

   CString GetFatfPackageName()           { return m_fatfPkgName; }
   void SetFatfPackageName(CString name)  { m_fatfPkgName = name; }

   void CollectPins(CCEtoODBDoc *doc, FileStruct *pcbFile, BlockStruct *compGeomBlk) { this->m_pinMap.CollectPins(doc, pcbFile, compGeomBlk); }

   BlockStruct *GetCamcadGeometryBlock()  { return m_camcadGeometryBlock; }
   CFatfPinMap& GetPinMap()   { return m_pinMap; }


};

class CFatfPackageMap : public CMapSortedStringToOb<CFatfPackage>
{
private:
   static int AscendingNameSortFunc(const void *a, const void *b);

public:
   void CollectPackages(CCEtoODBDoc *doc, FileStruct *pcbFile);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfNet : public CObject
{
private:
   int   m_fatfId;
   NetStruct *m_camcadNet;

public:
   CFatfNet(int fatfId, NetStruct *camcadNet)  { m_fatfId = fatfId; m_camcadNet = camcadNet; }

   int GetFatfNetId()              { return m_fatfId; }
   void SetFatfNetId(int netId)    { m_fatfId = netId; }

   NetStruct *GetCamcadNet()  { return m_camcadNet; }

   CString GetFatfNetType(CCEtoODBDoc *doc);
};

class CFatfNetMap : public CMapSortedStringToOb<CFatfNet>
{
private:
   static int AscendingNetnameSortFunc(const void *a, const void *b);

public:
   void CollectNets(CCEtoODBDoc *doc, FileStruct *pcbFile);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfVia : public CObject
{
private:
   int   m_fatfViaId;
   int   m_fatfNetId;  // ID of net via is on, 0 if no net
   DataStruct *m_camcadViaData;

public:
   CFatfVia(int fatfViaId, int fatfNetId, DataStruct *camcadViaData)  { m_fatfViaId = fatfViaId; m_fatfNetId = fatfNetId; m_camcadViaData = camcadViaData; }

   int GetFatfViaId()              { return m_fatfViaId; }
   void SetFatfViaId(int viaId)    { m_fatfViaId = viaId; }

   int GetFatfNetId()              { return m_fatfNetId; }
   void SetFatfNetId(int netId)    { m_fatfNetId = netId; }

   DataStruct *GetCamcadViaData()  { return m_camcadViaData; }

   CString GetDeviceType(CCEtoODBDoc *doc);
   CString GetInsertedBlockNumberAsString();
   CString GetInsertedGeometryName(CCEtoODBDoc *doc); //*rcf May Be Obsolete, delete at end if not used

};

class CFatfViaMap : public CMapSortedStringToOb<CFatfVia>
{
private:
   static int AscendingNetIdSortFunc(const void *a, const void *b);

public:
   void CollectVias(CCEtoODBDoc *doc, FileStruct *pcbFile, CFatfNetMap &fatfNetMap);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfPadstack : public CObject
{
private:
   int   m_fatfPadstackId;
   BlockStruct *m_camcadGeometryBlock;
   int   m_insertlayerId; 
   double GetDrillSize(CCEtoODBDoc *doc, BlockStruct *block);

public:
   CFatfPadstack(int fatfPstkId, BlockStruct *camcadGeomBlk, int inserlayerId = -1)  { m_fatfPadstackId = fatfPstkId; m_camcadGeometryBlock = camcadGeomBlk; m_insertlayerId = inserlayerId;}

   int GetFatfPadstackId()             { return m_fatfPadstackId; }
   void SetFatfPadstackId(int pstkId)  { m_fatfPadstackId = pstkId; }

   CString GetFatfPadstackName()       { return m_camcadGeometryBlock->getName(); }

   BlockStruct *GetCamcadGeometryBlock()  { return m_camcadGeometryBlock; }

   double GetDrillSize(CCEtoODBDoc *doc);
   int GetInsertlayerId()             { return m_insertlayerId; }
};

class CFatfPadstackMap : public CMapSortedStringToOb<CFatfPadstack>
{
private:
   static int AscendingNameSortFunc(const void *a, const void *b);

public:
   void CollectPadstacks(CCEtoODBDoc *doc, FileStruct *pcbFile, CFatfViaMap &viaMap, CFatfPackageMap &packageMap);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class CFatfFont : public CObject
{
private:
   int m_fatfFontId;
   int m_fatfCharHeight;
   int m_fatfCharWidth;
   int m_fatfPenWidth;


public:
   CFatfFont(int fontId, int charHght, int charWidth, int penWidth)  { m_fatfFontId = fontId; m_fatfCharHeight = charHght; m_fatfCharWidth = charWidth; m_fatfPenWidth = penWidth; }

   int GetFatfFontId()          { return m_fatfFontId; }
   void SetFatfFontId(int id)   { m_fatfFontId = id; }

   int GetFatfCharHeight()     { return m_fatfCharHeight; }
   int GetFatfCharWidth()      { return m_fatfCharWidth; }
   int GetFatfPenWidth()       { return m_fatfPenWidth; }

   bool IsMatch(int chH, int chW, int pnW) { return( (chH == m_fatfCharHeight) && (chW == m_fatfCharWidth) && (pnW == m_fatfPenWidth) ); }
};

class CFatfFontMap : public CMapSortedStringToOb<CFatfFont>
{
public:
   static int AscendingIdSortFunc(const void *a, const void *b);

   CFatfFont *GetFatfFont(int fatfFontHeight, int fatfFontWidth, int fatfPenWidth);
};
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////


class CFabmasterFATFWriter
{
private:
   CCEtoODBDoc *m_doc;
   FileStruct *m_pcbFile;
   CFatfLayerMap m_fatfLayerMap;
   CFatfPartMap m_fatfPartMap;
   CFatfPackageMap m_fatfPackageMap;
   CFatfPartMap m_fatfTestPointMap;
   CFatfViaMap m_fatfViaMap;
   CFatfNetMap m_fatfNetMap;
   CFatfPadstackMap m_fatfPadstackMap;
   CFatfPadSymbolMap m_fatfPadSymbolMap;
   CFatfFontMap m_fatfFontMap;
   double m_linearScaleFactor;  // camcad doc units to output units

   double ArcAngleRadians(double radius);

   bool GetFatfPackageAndPin(CFatfPart *fatfPart, CString camcadPinRefname, CFatfPackage *&fatfPkg, CFatfPin *&fatfPin);
   double GetCamcadBoardThickness();

   void MarkSpecialLayers();
   void MarkCompOutlineLayers(BlockStruct *camcadGeom, bool isOutline);

   void AssignNetIDs();

   void WritePolylist(CFormatStdioFile &outFile, CPolyList *polyList, bool isNetTrace,
      double insertX, double insertY, double insertRotation, 
      int insertMirror, double insertScale);

      
   void WriteGraphicBlock(CFormatStdioFile &outFile, CFatfPackage *fatfPkg, BlockStruct *block,
      double insertX, double insertY, double insertRotation, 
      int insertMirror, double insertScale, int insertLayer, int fatfPinId, int embeddedLevel);

   void WriteHeader(CFormatStdioFile &outFile);
   void WriteBoardData(CFormatStdioFile &outFile);
   void WriteBoardOutline(CFormatStdioFile &outFile, int &recId);
   void WriteFiducials(CFormatStdioFile &outFile, int &recId);
   void WriteParts(CFormatStdioFile &outFile);
   void WritePackages(CFormatStdioFile &outFile);
   void WriteLayerNames(CFormatStdioFile &outFile);
   void WriteNets(CFormatStdioFile &outFile);
   void WriteNetTraces(CFormatStdioFile &outFile); // :LAYERS section in fatf
   void WritePadStacks(CFormatStdioFile &outFile);
   void WritePadStackPads(CFormatStdioFile &outFile, BlockStruct *padstackBlock, LayerStruct *layer, int &count);
   void WritePads(CFormatStdioFile &outFile); // aka Vias
   void WritePadSymbols(CFormatStdioFile &outFile); // aka Apertures
   void WriteLayerSets(CFormatStdioFile &outfile);
   void WriteTestPoints(CFormatStdioFile &outfile);
   void WriteFonts(CFormatStdioFile &outfile);

public:
   CFabmasterFATFWriter(CCEtoODBDoc *doc, FileStruct *pcbFile);
   void InitData();
   void WriteFile(CString filename);

   int ScaleToFatf(double camcadValue);

};


#endif
