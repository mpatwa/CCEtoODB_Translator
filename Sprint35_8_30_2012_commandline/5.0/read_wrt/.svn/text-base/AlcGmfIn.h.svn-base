// $Header: /CAMCAD/4.3/read_wrt/AlcGmfIn.h 11    11/19/03 9:17p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if ! defined (__AlcGmfIn_h__)
#define __AlcGmfIn_h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AlienDatabase.h"
#include "RwLib.h"

//_____________________________________________________________________________

enum AlcatelGmfFileSectionTag
{
   sectFil, sectMng, sectRev, sectBom, sectTec, sectAsm, sectPpn, sectNet, 
   sectEcd, sectLtp, sectPlb, sectIct, sectDrl, sectRut, sectOns,
   sectUndefined
};

enum OutlineKindTag
{
   olBoard, 
   olPanel, 
   olPartMountingArea, 
   olMountingInhibitArea,
   olResistArea, 
   olResistInhibitArea, 
   olRoutingArea, 
   olNegativeCopperPowerLayerArea,
   olLineInhibitArea, 
   olViaInhibitArea, 
   olCopperInhibitArea, 
   olNegativeCopperPowerLayerInhibitArea,
   olDrawingFigure,
   olSilkFigure,
   olUndefined
};

enum LayerCategoryTag
{
   lcNone,
   lcOne,
   lcTop, 
   lcBottom, 
   lcOuter,
   lcAll, 
   //lcMounted,
   lcMountedTop,
   lcMountedBottom,
   //lcMountedOpposite, 
   lcMountedTopOpposite,
   lcMountedBottomOpposite,
   lcUndefined
};

enum EntityTypeTag
{
   entityLine,
   entityCircle,
   entityArc,
   entityPoly,
   entityRectangle,
   entityOval,
   entityTrimmedRectangle,
   entityRoundedRectangle,
   entityUndefined
};

enum AttributeIndexTag
{
   attributeIndexNetName             ,
   attributeIndexValue               ,
   attributeIndexSmdShape            ,
   attributeIndexVoltage             ,
   attributeIndexTolerance           ,
   attributeIndexNumberOfComps       ,
   attributeIndexEsd                 ,
   attributeIndexHeatMoisture        ,
   attributeIndexGenericName         ,
   attributeIndexPhysicalRefDes      ,
   attributeIndexTypeListLink        ,
   attributeIndexCompHeight          ,
   attributeIndexLayerPowerPos       ,
   attributeIndexLayerSignalTop      ,
   attributeIndexLayerSignalBottom   ,
   attributeIndexLayerSignalInner    ,
   attributeIndexLayerSignalStack    ,
   attributeIndexPlated              ,
   attributeIndexPowerNet            ,
   attributeIndexCompPinnr           ,
   attributeIndexPinFunction
};

enum AlcatelGmfFileRecordTypeTag
{
   alcatelKwArea,
   alcatelKwAsm,
   alcatelKwBkpotara,
   alcatelKwBoard,
   alcatelKwBom,
   alcatelKwComent,
   alcatelKwCompnt,
   alcatelKwCopihara,
   alcatelKwCopper,
   alcatelKwCutdat,
   alcatelKwDate,
   alcatelKwDdrwfig,
   alcatelKwDist,
   alcatelKwDocum,
   alcatelKwDrl,
   alcatelKwEcd,
   alcatelKwEnd,
   alcatelKwFil,
   alcatelKwFile,
   alcatelKwFormat,
   alcatelKwHead,
   alcatelKwIct,
   alcatelKwJmpdat,
   alcatelKwKpoutara,
   alcatelKwLand,
   alcatelKwLayer,
   alcatelKwLayno,
   alcatelKwLayout,
   alcatelKwLine,
   alcatelKwLinihara,
   alcatelKwLndfig,
   alcatelKwLtp,
   alcatelKwLvaihara,
   alcatelKwMng,
   alcatelKwNcpihara,
   alcatelKwNet,
   alcatelKwNetdef,
   alcatelKwNetend,
   alcatelKwNetlst,
   alcatelKwOns,
   alcatelKwOrigin,
   alcatelKwPanel,
   alcatelKwPddef,
   alcatelKwPddrwfig,
   alcatelKwPdent,
   alcatelKwPin,
   alcatelKwPlb,
   alcatelKwPosmk,
   alcatelKwPpn,
   alcatelKwPrtdef,
   alcatelKwPrtend,
   alcatelKwPtndef,
   alcatelKwPtnend,
   alcatelKwResihara,
   alcatelKwRev,
   alcatelKwRottyp,
   alcatelKwRut,
   alcatelKwSilkfig,
   alcatelKwSystem,
   alcatelKwTec,
   alcatelKwTstpnt,
   alcatelKwTypdef,
   alcatelKwTypend,
   alcatelKwUnit,
   alcatelKwVia,
   alcatelKwViaihara,
   alcatelKwView,

   alcatelKwUndefined
};

AlcatelGmfFileSectionTag stringToAlcatelGmfFileSectionTag(const CString& tagString);

class CAlcatelGmfDatabase;

//_____________________________________________________________________________
class CCompArchitypePin : public CObject
{
private:
   int m_sequenceNumber;
   CString m_pinName;
   CString m_function;

public:
   CCompArchitypePin(int sequenceNumber,const CString& pinName,const CString& function);

   int getSequenceNumber() { return m_sequenceNumber; }
   CString getPinName()    { return m_pinName; }
   CString getFunction()   { return m_function; }

   bool operator==(const CCompArchitypePin& other);
};

//_____________________________________________________________________________
class CCompArchitype : public CObject
{
private:
   CString m_plbName;
   CString m_partNumber;
   CString m_refdes;
   CTypedPtrArray<CObArray,CCompArchitypePin*> m_pins;

public:
   CCompArchitype(const CString& plbName,const CString& partNumber,const CString& refdes);
   ~CCompArchitype();

   CString getName();
   CString getPlbName()    { return m_plbName; }
   CString getPartNumber() { return m_partNumber; }
   CString getRefdes()     { return m_refdes; }
   CString getPopName();
   bool popName();

   int getNumPins() { return (int)m_pins.GetCount(); }
   CCompArchitypePin* addPin(int sequenceNumber,const CString& pinName,const CString& function);
   CCompArchitypePin* getPinAt(int sequenceNumber);
   bool pinsEqual(const CCompArchitype& other) const;
};

//_____________________________________________________________________________
class CCompArchitypes : public CObject
{
private:
   CString m_name;
   CTypedPtrMap<CMapStringToOb,CString,CCompArchitype*> m_architypes;          // maps architype names to the architype
   CTypedPtrMap<CMapStringToOb,CString,CCompArchitype*> m_architypesByRefdes;  // maps refdes to the architype

public:
   CCompArchitypes();
   ~CCompArchitypes();

   CCompArchitype* addArchitype(const CString& plbName,const CString& partNumber,const CString& refdes);
   CCompArchitype* addPin(const CString& refdes,int sequenceNumber,const CString& pinName,const CString& function);
   bool removeRedundancies(const CString& refdes);
   int getPlbNames(CStringArray& plbNames,const CString& plbName);
   bool getArchitype(const CString& name,CCompArchitype*& architype) { return (m_architypes.Lookup(name,architype) != 0); }
};

//_____________________________________________________________________________
class CAlcatelGmfRecord : public CObject
{
private:
   CAlcatelGmfDatabase* m_alcatelGmfDatabase;
   CSupString m_recordString;
   AlcatelGmfFileRecordTypeTag m_recordType;
   static const int m_maxParams = 40000;
   CStringArray m_params;

public:
   CAlcatelGmfRecord();
   CString getRecordString() { return m_recordString; }
   int getParamCount() { return (int)(m_params.GetCount()); }
   AlcatelGmfFileRecordTypeTag getRecordType() { return m_recordType; }
   void setDatabase(CAlcatelGmfDatabase* alcatelGmfDatabase) { m_alcatelGmfDatabase = alcatelGmfDatabase; }
   bool isGood(AlcatelGmfFileSectionTag section,AlcatelGmfFileRecordTypeTag recordType,CWriteFormat& log);

   CString getParam(int index);
   CPoint2d getCoord(int index);
   CPoint2d getCoordInInches(int index);
   int getInt(int index);
   int getIntInches(int index);
   double getDouble(int index);
   double getDegrees(int index);
   double getRadians(int index) { return degreesToRadians(getDegrees(index)); }
   double getInches(int index);

   CAlcatelGmfRecord& operator=(const CString&);
};

//_____________________________________________________________________________
class CAlcatelGmfDatabase : public CAlienDatabase
{
private:
   CAlcatelGmfRecord m_record;
   CCompArchitypes m_compArchitypes;
   AlcatelGmfFileSectionTag m_currentSection;
   double m_inchesPerUnit;

   BlockStruct* m_plbBlock;
   CStringArray m_plbNames;
   BlockStruct* m_ltpBlock;
   CString m_topLayerName;
   CString m_bottomLayerName;
   CString m_netName;
   CString m_ppnRefdes;
   CUIntArray m_attributeIndexDirectory;

   bool m_thruHoleFlag;
   int m_layerRelatedType;
   int m_topLayerIndex;
   int m_bottomLayerIndex;
   int m_minSignalLayerIndex;
   int m_maxSignalLayerIndex;
   int m_numberOfElectricalLayers;
   int m_signalLayerCount;

   // Rut section
   CString m_rutNetName;
   CString m_rutLayerName;
   double m_rutLineWidth;
   DataStruct* m_rutPolyStruct;
   CPoly* m_rutPoly;
   int m_rutVertexCount;
   CPoint2d m_rutFirstVertexPoint;
   CPoint2d m_rutLastVertexPoint;
   CPnt* m_rutFirstVertex;
   CPnt* m_rutLastVertex;

public:
   CAlcatelGmfDatabase(CCamCadDatabase& camCadDatabase);

   virtual bool read(const CString& filePath,PageUnitsTag pageUnits,CWriteFormat& errorLog);

public:
   CString getTopLayerName() { return m_topLayerName; }
   CString getBottomLayerName() { return m_bottomLayerName; }

   CPoint2d parseCoordinate(const char* coordinateString);
   CPoint2d parseCoordinateToInches(const char* coordinateString);

private:
   void rectifyOuterLayers();
   void setTopLayerIndex(int topLayerIndex);
   void setBottomLayerIndex(int topLayerIndex);
   int getMinSignalLayerIndex();
   int getMaxSignalLayerIndex();

   bool readRecord(CStdioFile& inputFile,int& lineNumber);
   void initializeElectricalLayers(CWriteFormat& errorLog);
   void initializeCamCadLayers();
   void initializeCamCadLayerN(CamCadLayerTag layerTag,int layerType);
   bool getLayerNamesL1orLn(CStringArray& layerNames,const CString& layerName,LayerCategoryTag layerCategory,
      CamCadLayerTag topLayerTag,CamCadLayerTag bottomLayerTag,CamCadLayerTag unrelatedLayerTag,
      CWriteFormat& errorLog);
   bool getLayerNamesL1thruLn(CStringArray& layerNames,const CString& layerName,
      LayerCategoryTag layerCategory,CamCadLayerTag layerTag,CWriteFormat& errorLog);

   int getAttributeIndex(AttributeIndexTag attributeIndexTag);

   void processFilRecord(CWriteFormat& errorLog);
   void processMngRecord(CWriteFormat& errorLog);
   void processRevRecord(CWriteFormat& errorLog);
   void processBomRecord(CWriteFormat& errorLog);
   void processTecRecord(CWriteFormat& errorLog);
   void processAsmRecord(CWriteFormat& errorLog);
   void processPpnRecord(CWriteFormat& errorLog);
   void processNetRecord(CWriteFormat& errorLog);
   void processEcdRecord(CWriteFormat& errorLog);
   void processLtpRecord(CWriteFormat& errorLog);
   void processPlbRecord(CWriteFormat& errorLog);
   void processIctRecord(CWriteFormat& errorLog);
   void processDrlRecord(CWriteFormat& errorLog);
   void processRutRecord(CWriteFormat& errorLog);
   void processOnsRecord(CWriteFormat& errorLog);

   void addOutlineSegment(BlockStruct* parentBlock,int layerIndex,int widthIndex,GraphicClassTag graphicClass,
      double x1,double y1,double x2,double y2,double bulge,bool fillFlag,bool voidFlag);
   void addOutlineArcSegment(BlockStruct* parentBlock,int layerIndex,int widthIndex,GraphicClassTag graphicClass,
      CPoint2d& arcCenter,double radius,double startAngleDegrees,double stopAngleDegrees,bool fillFlag,bool voidFlag);
   void addOutlineEntity(BlockStruct* parentBlock,int layerIndex,int widthIndex,GraphicClassTag graphicClass,
      CString entityType,int entityParamIndex,bool fillFlag=false,bool voidFlag=false);
   bool addArea(BlockStruct* parentBlock,const CString& layerName,LayerCategoryTag layerCategory,OutlineKindTag outlineKind,
      EntityTypeTag entityType,int entityParameterIndex,CWriteFormat& errorLog,
      double width=0.,bool fillFlag=true);
   bool addAreaPoly(CPoly* poly,EntityTypeTag entityType,int entityParameterIndex,CWriteFormat& errorLog);
   int getCamCadLayerNames(CStringArray& layerNames,int layerRelatedType,const CString& objectCodeString,bool thruHoleFlag);
   int getElectricalLayerNames(CStringArray& layerNames);

   bool instantiateFlattenedVia(const CString& viaName,const CString& ltpName,
                                const CString& layer1name,const CString& layer2name,bool topFlag);
   void addBoardOutlineEntity(BlockStruct* parentBlock,GraphicClassTag graphicClass);
};

#endif


