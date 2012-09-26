// $Header: /CAMCAD/5.0/read_wrt/CyberOpticsAoi_Out.h 2     05/19/09 9:51a Sharry Huang $
#if !defined(_CYBEROPTICSAOI_OUT_H)
#define _CYBEROPTICSAOI_OUT_H


#pragma once
#include "DcaWriteFormat.h"
#include "BaseAoiExporter.h"

#define QCYBERO_BOARDNAME "Board"
#define QGEOM_NAME  "GEOM_NAME"

#define INDENT_SIZE_DEFAULT 4
#define INDENT_SIZE_MIN     0
#define INDENT_SIZE_MAX    15

//=================================================================================

class SRFFWriter;

enum SrffShapeTag {
   srffShapeUnknown, // For shape-not-set, not a real SRFF shape.
   srffShapeCross,
   srffShapeDiamond,
   srffShapeDisc,
   srffShapeDonut,
   srffShapeRectangle,
   srffShapeTriangle,
   srffShapeVendorShapeLink
};

//----------------------------------------------

class CyberSegment
{
private:
   int m_id;
   bool m_line;  // True = segment is line, false = segment is arc.
   bool m_penDown; // Classic move/draw, true = down, false = not down.
   bool m_clockwise; // Applies to arcs only.
   double m_positionX;
   double m_positionY;
   double m_arcCenterX;
   double m_arcCenterY;

public:
   CyberSegment(int id, bool penDown, double x, double y);  // For creating straight line segment vertex.
   CyberSegment(int id, bool penDown, double x, double y, bool ccw, double centerX, double centerY); // For creating arc segment.

   int GetId()             { return m_id; }
   bool IsArc()            { return !m_line; }
   bool GetPenDown()       { return m_penDown; }
   bool GetClockwise()     { return m_clockwise; }

   double GetX(double factor)           { return m_positionX * factor; }
   double GetY(double factor)           { return m_positionY * factor; }
   double GetCenterX(double factor)     { return m_arcCenterX * factor; }
   double GetCenterY(double factor)     { return m_arcCenterY * factor; }
};

class CyberSegmentList : public CTypedPtrArrayContainer<CyberSegment*>
{
public:
   void AddSegment(int id, bool penDown, double x, double y);  // For creating straight line segment vertex.
   void AddSegment(int id, bool penDown, double x, double y, bool ccw, double centerX, double centerY); // For creating arc segment.

};


class CyberShape
{
   // This supports CyberOptics extension to SRFF for shapes.
private:
   int m_id;
   CString m_name;
   int m_cczBlockNum; // Aperture block num that caused this shape to exist.
   CyberSegmentList m_segmentList;

public:
   CyberShape(int id, CString name, int cczBlockNum)    { m_id = id; m_name = name; m_cczBlockNum = cczBlockNum; }

   int GetId()          { return m_id; }
   CString GetName()    { return m_name; }
   int GetCczBlockNum() { return m_cczBlockNum; }

   void AddSegment(int id, bool penDown, double x, double y)
      { m_segmentList.AddSegment(id, penDown, x, y); }
   void AddSegment(int id, bool penDown, double x, double y, bool ccw, double centerX, double centerY)
      { m_segmentList.AddSegment(id, penDown, x, y, ccw, centerX, centerY); }

   int GetSegmentCount()             { return this->m_segmentList.GetCount(); }
   CyberSegment *GetSegmentAt(int i) { return (i >= 0 && i < m_segmentList.GetCount()) ? m_segmentList.ElementAt(i) : NULL; }

};

class CyberShapeMap : public CTypedPtrArrayWithMapContainer<CyberShape>
{
};


//----------------------------------------------

class SrffShape
{
   // This supports shapes defined in SRFF.

private:
   int m_id;
   SrffShapeTag m_shapeTag;
   double m_sizeA;
   double m_sizeB;

public:
   SrffShape(int id, SrffShapeTag shapeTag, double sizeA, double sizeB)
   { m_id = id; m_shapeTag = shapeTag; m_sizeA = sizeA; m_sizeB = sizeB; }

   int GetId()                   { return m_id; }
   SrffShapeTag GetShapeTag()    { return m_shapeTag; }
   double GetSizeA()             { return m_sizeA; }
   double GetSizeB()             { return m_sizeB; }

   CString GetShapeObjectDescription(SRFFWriter *srffWriter);

   bool operator==(const SrffShape &other) const // Don't check  ID, just the rest.
   { return (m_shapeTag == other.m_shapeTag && m_sizeA == other.m_sizeA && m_sizeB == other.m_sizeB); }

};

class SrffShapeList : public CTypedPtrListContainer<SrffShape*>
{
public:
   SrffShape *AddShape(SrffShapeTag shapeTag, double sizeA, double sizeB);
   SrffShape *Lookup(SrffShape *shape);
};

//-----------------------------------------------------------------------

class SrffImage
{
private:
   int m_imageId;
   int m_cczBlockNum;   // Inserted block's number, could be board or panel
   CString m_refname;
   double m_positionX;  // in ccz page units
   double m_positionY;  // in ccz page units
   double m_positionZ;
   double m_rotationDegreesX;
   double m_rotationDegreesY;
   double m_rotationDegreesZ;
   int m_referenceImageDefinitionId;
   int m_referenceImageId;
   int m_referenceSkipMarkId;

public:
   SrffImage(int blockNum, int imageId, CString refname, double locX, double locY, double rotDeg, int imageDefinitionId);

   int GetImageId()        { return m_imageId; }
   int GetBlockNumber()    { return m_cczBlockNum; }
   CString GetRefname()    { return m_refname; }
   double GetPositionX(double factor)   { return m_positionX * factor; }
   double GetPositionY(double factor)   { return m_positionY * factor; }
   double GetRotationDeg() { return m_rotationDegreesZ; }

   int GetReferenceImageDefinitionId()    { return m_referenceImageDefinitionId; }
   int GetReferenceImageId()              { return m_referenceImageId; }
   int GetReferenceSkipMarkId()           { return m_referenceSkipMarkId; }

};

class SrffImageList : public CTypedPtrListContainer<SrffImage*>
{
};

//-----------------------------------------------------------------------

class SrffLocation
{
private:
   int m_locationId;
   CString m_refName;   // comp insert refname
   double m_positionX;  // in Cyber output units
   double m_positionY;  // in Cyber output units
   double m_positionZ;
   double m_rotationDegreesX;
   double m_rotationDegreesY;
   double m_rotationDegreesZ;
   int m_referenceComponentDefinitionId;
   int m_referenceImageDefinitionId;

public:
   SrffLocation(int id, CString refname, double locX, double locY, double rotDeg, int componentDefinitionId, int imageDefinitionId);

   int GetLocationId()     { return m_locationId; }
   CString GetRefname()    { return m_refName; }
   double GetPositionX(double factor)   { return m_positionX * factor; }
   double GetPositionY(double factor)   { return m_positionY * factor; }
   double GetRotationDeg() { return m_rotationDegreesZ; }

   int GetReferenceComponentDefinitionId()   { return m_referenceComponentDefinitionId; }
   int GetReferenceImageDefinitionId()       { return m_referenceImageDefinitionId; }
};

class SrffLocationList : public CTypedPtrListContainer<SrffLocation*>
{
};

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

class SrffImageFiducial
{
private:
   int m_id;
   CString m_refName;   // comp insert refname
   double m_positionX;  // in Cyber output units
   double m_positionY;  // in Cyber output units
   double m_rotationDegreesZ;
   int m_referenceShapeId;
   int m_referenceImageDefinitionId;

public:
   SrffImageFiducial(int id, CString refname, double locX, double locY, double rotDeg, int shapeId, int imageDefinitionId);

   int GetId()             { return m_id; }
   CString GetRefname()    { return m_refName; }
   double GetPositionX(double factor)   { return m_positionX * factor; }
   double GetPositionY(double factor)   { return m_positionY * factor; }
   double GetRotationDeg() { return m_rotationDegreesZ; }

   int GetReferenceShapeId()                 { return m_referenceShapeId; }
   int GetReferenceImageDefinitionId()       { return m_referenceImageDefinitionId; }
};

class SrffImageFiducialList : public CTypedPtrListContainer<SrffImageFiducial*>
{
};

//-----------------------------------------------------------------------

class SrffImageDefinition
{
private:
   int m_imageDefId;
   CString m_imageDefName;  // Name of pcb or panel, not a refname because not instance specific
   SrffLocationList m_locationList;
   SrffImageFiducialList m_fiducialList;

public:
   SrffImageDefinition(int imageDefId, CString imageDefName);

   int GetId()             { return m_imageDefId; }
   CString GetName()       { return m_imageDefName; }

   POSITION GetHeadLocationPosition()  { return m_locationList.GetHeadPosition(); }
   SrffLocation *GetNextLocation(POSITION &pos) { return m_locationList.GetNext(pos); }
   void AddLocation(int id, CString refname, double locX, double locY, double rotDeg, int componentDefinitionId);

   POSITION GetHeadFiducialPosition()  { return m_fiducialList.GetHeadPosition(); }
   SrffImageFiducial *GetNextFiducial(POSITION &pos) { return m_fiducialList.GetNext(pos); }
   int GetFiducialCount()              { return m_fiducialList.GetCount(); }
   void AddFiducial(int id, CString refname, double locX, double locY, double rotDeg, int shapeId);

};

//class SrffImageDefinitionMap : public CTypedMapIntToPtrContainer<SrffImageDefinition*>
class SrffImageDefinitionMap : public  CTypedPtrArrayWithMapContainer<SrffImageDefinition>
{
};

//-----------------------------------------------------------------------

class SrffComponentDefinition
{
   // Associates a part number to a package (geometry) by way of ComponentLink.

private:
   int m_id;
   CString m_partName; // aka Part Number from CCZ land
   int m_referenceComponentLinkId; // Link to record that defines Package Name (aka geometry)

public:
   SrffComponentDefinition(int id, CString partname, int linkId) { m_id = id; m_partName = partname; m_referenceComponentLinkId = linkId; }

   int GetId()             { return m_id; }
   CString GetPartName()   { return m_partName; }
   int GetReferenceComponentLinkId()   { return m_referenceComponentLinkId; }
};

// Map key is partnumber string
class SrffComponentDefinitionMap : public CTypedMapStringToPtrContainer<SrffComponentDefinition*>
{
};

//-----------------------------------------------------------------------

class SrffFeature
{
   // Associates a part number to a package (geometry) by way of ComponentLink.

private:
   int m_id;
   CString m_featureName;
   double m_positionX;
   double m_positionY;
   double m_positionZ;
   double m_rotationDeg;
   int m_referenceShapeId;

public:
   SrffFeature(int id, CString featureName, double locX, double locY, double rotDeg, int refShapeId)
   { m_id = id; m_featureName = featureName; m_positionX = locX; m_positionY = locY; m_rotationDeg = rotDeg; m_referenceShapeId = refShapeId; }

   int GetId()             { return m_id; }
   CString GetName()       { return m_featureName; }
   double GetPositionX(double factor)   { return m_positionX * factor; }
   double GetPositionY(double factor)   { return m_positionY * factor; }
   double GetRotationDeg() { return m_rotationDeg; }
   int GetRefShapeId()     { return m_referenceShapeId; }
};

// The master array is owner of the objects.
// This  is used in top level writer, to hold (and later free) the objects.
// Partly done this way because Id numbering transcends any particular
// Pattern Def that uses the features.
class SrffFeatureMasterArray : public CTypedPtrArrayContainer<SrffFeature*>
{
};

// The local array has references but is not the owner.
// This is used in Pattern Def to identify its own feature collection.
class SrffFeatureLocalArray : public CTypedPtrArray<CPtrArray, SrffFeature*>
{
};

//-----------------------------------------------------------------------

class SrffPatternDefinition
{
   // We're combining the pattern link and pattern definition into one
   // structure, one list. We can get away with that because we only have
   // patterns for components. But it is not fully generalized this way.
   // Conceivably more than one thing could use the same pattern. But we
   // are arranging our output so it will be 1 to 1.

private:
   int m_id; // pattern ID
   CString m_patternName; // usually same as package name
   int m_referenceComponentLinkId; // Link to record that defines Package Name (aka geometry)
   SrffFeatureLocalArray m_featureArray;

public:
   SrffPatternDefinition(int id, CString patternname, int compLinkId)
   { m_id = id; m_patternName = patternname; m_referenceComponentLinkId = compLinkId; }

   int GetId()                { return m_id; }
   CString GetPatternName()   { return m_patternName; }
   int GetReferenceComponentLinkId()   { return m_referenceComponentLinkId; }

   int GetFeatureCount()               { return m_featureArray.GetCount(); }
   SrffFeature *GetFeatureAt(int indx) { return ((indx >= 0 && indx < m_featureArray.GetCount()) ? m_featureArray.GetAt(indx) : NULL); }
   void AddFeature(SrffFeature *f)     { m_featureArray.Add(f); }
};

// Map key is pattern ID
class SrffPatternDefinitionMap : public CTypedMapStringToPtrContainer<SrffPatternDefinition*>
{
};

//-----------------------------------------------------------------------

class SrffComponentLink
{
   // Associates a package name (geometry) to a pattern definition.

private:
   int m_id;
   CString m_packageName; // aka Geometry name

public:
   SrffComponentLink(int id, CString packagename) { m_id = id; m_packageName = packagename; }

   int GetId()                { return m_id; }
   CString GetPackageName()   { return m_packageName; }
};

// Map key is ccz geometry block number
//class SrffComponentLinkMap : public CTypedMapIntToPtrContainer<SrffComponentLink*>
class SrffComponentLinkMap : public CTypedPtrArrayWithMapContainer<SrffComponentLink>
{
};

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

class SRFFWriter
{
private:
   int m_indentLevel;
   int m_indentSize;
   CString m_indent;
   void PushIndent();
   void PopIndent();
   CString Indent()  { return m_indent; }

   CTime m_dateTime;
   CString m_productName;
   CString m_productNote;

   CCEtoODBDoc &m_doc; // Tried not to, but finally had to, in order to get blocks.


   PageUnitsTag m_outputUnits;
   double m_outputUnitsFactor; // Converts incoming page units to output units.

   // These, and all other SRFF class values that are linear units, are stored
   // in output units (not ccz page units).
   double m_panelSizeX;
   double m_panelSizeY;

   // We currently have no process data, but we have the framework in place
   // to handle some, including schema output and upper level data section.
   // This controls whether to write any of what we got so far.
   bool m_writeProcessSection;

   SrffImageList m_imageList;
   SrffImageDefinitionMap m_imageDefinitionMap;

   SrffComponentDefinitionMap m_componentDefinitionMap;
   SrffComponentLinkMap m_componentLinkMap;

   SrffPatternDefinitionMap m_patternDefinitionMap;
   SrffFeatureMasterArray m_featureMasterArray;

   SrffShapeList m_shapeList;
   CyberShapeMap m_cyberShapeMap;
   int m_lastCyberSegmentId;

   int m_lastImageFiducialId;



   //
   // Schema
   //
   void WriteProductSectionSchema(CFormatStdioFile &file);
   void WriteSmemaProductSchema(CFormatStdioFile &file);
   void WriteComponentDefinitionSchema(CFormatStdioFile &file);
   void WriteComponentLinkSchema(CFormatStdioFile &file);
   void WriteBarcodeSchema(CFormatStdioFile &file);
   void WriteFeatureSchema(CFormatStdioFile &file);
   void WriteHeaderSchema(CFormatStdioFile &file);
   void WriteImageSchema(CFormatStdioFile &file);
   void WriteImageDefinitionSchema(CFormatStdioFile &file);
   void WriteImageFiducialSchema(CFormatStdioFile &file);
   void WriteLocalFiducialSchema(CFormatStdioFile &file);
   void WriteLocationSchema(CFormatStdioFile &file);
   void WritePanelSchema(CFormatStdioFile &file);
   void WritePatternSchema(CFormatStdioFile &file);
   void WritePatternDefinitionSchema(CFormatStdioFile &file);
   void WriteShapeSchema(CFormatStdioFile &file);
   void WriteSkipMarkSchema(CFormatStdioFile &file);
   void WriteSRFFVersionSchema(CFormatStdioFile &file);
   void WriteVendorShapeLinkSchema(CFormatStdioFile &file);
   void WritePrintAreaSchema(CFormatStdioFile &file);
   void WriteScreenPropertiesSchema(CFormatStdioFile &file);
   void WriteScreenFiducialSchema(CFormatStdioFile &file);
   void WriteCrossSchema(CFormatStdioFile &file);
   void WriteDiamondSchema(CFormatStdioFile &file);
   void WriteDiscSchema(CFormatStdioFile &file);
   void WriteDonutSchema(CFormatStdioFile &file);
   void WriteRectangleSchema(CFormatStdioFile &file);
   void WriteTriangleSchema(CFormatStdioFile &file);
   void WriteAccelerationUnitsSchema(CFormatStdioFile &file);
   void WriteAngleUnitsSchema(CFormatStdioFile &file);
   void WriteAngularAccelerationUnitsSchema(CFormatStdioFile &file);
   void WriteAngularVelocityUnitsSchema(CFormatStdioFile &file);
   void WriteDistanceUnitsSchema(CFormatStdioFile &file);
   void WriteFlowUnitsSchema(CFormatStdioFile &file);
   void WriteForceUnitsSchema(CFormatStdioFile &file);
   void WriteHumidityUnitsSchema(CFormatStdioFile &file);
   void WriteMassUnitsSchema(CFormatStdioFile &file);
   void WritePowerUnitsSchema(CFormatStdioFile &file);
   void WritePressureUnitsSchema(CFormatStdioFile &file);
   void WriteTemperatureUnitsSchema(CFormatStdioFile &file);
   void WriteTimeUnitsSchema(CFormatStdioFile &file);
   void WriteTorqueUnitsSchema(CFormatStdioFile &file);
   void WriteVelocityUnitsSchema(CFormatStdioFile &file);
   void WriteVolumeUnitsSchema(CFormatStdioFile &file);
   void WriteCyberProductSchema(CFormatStdioFile &file);
   void WriteCyberShapeSchema(CFormatStdioFile &file);
   void WriteCyberSegmentSchema(CFormatStdioFile &file);
   void WriteCyberImageNameSchema(CFormatStdioFile &file);
   void WriteProcessSectionSchema(CFormatStdioFile &file);
   void WriteSmemaProcessSectionSchema(CFormatStdioFile &file);
   void WriteFeatureGroupSchema(CFormatStdioFile &file);
   void WriteFeatureGroupOrderedSchema(CFormatStdioFile &file);
   void WriteLocationGroupSchema(CFormatStdioFile &file);
   void WriteLocationGroupOrderedSchema(CFormatStdioFile &file);
   void WriteDispenseOrderSchema(CFormatStdioFile &file);
   void WriteInspectOrderSchema(CFormatStdioFile &file);
   void WritePlacementOrderSchema(CFormatStdioFile &file);
   void WritePrintSchema(CFormatStdioFile &file);
   void WritePrinterAlignmentSchema(CFormatStdioFile &file);
   void WriteSqueegeeSchema(CFormatStdioFile &file);
   void WriteSqueegeeDefinitionSchema(CFormatStdioFile &file);
   void WriteCyberProcessSchema(CFormatStdioFile &file);

   //
   // Product Data
   //
   void WriteProductSectionData(CFormatStdioFile &file);
   void WriteSmemaProductData(CFormatStdioFile &file);
   void WriteCyberProductData(CFormatStdioFile &file);
   void WriteSRFFVersion(CFormatStdioFile &file);
   void WriteHeader(CFormatStdioFile &file);
   void WriteAngleUnits(CFormatStdioFile &file);
   void WriteDistanceUnits(CFormatStdioFile &file);
   void WritePanel(CFormatStdioFile &file);
   void WriteImage(CFormatStdioFile &file, SrffImage *image);
   void WriteImageList(CFormatStdioFile &file);
   void WriteImageDefinition(CFormatStdioFile &file, SrffImageDefinition *imageDef);
   void WriteImageDefinitionList(CFormatStdioFile &file);
   void WriteImageDefinitionLocationList(CFormatStdioFile &file, SrffImageDefinition *imageDef);
   void WriteLocation(CFormatStdioFile &file, SrffLocation *srffLoc);

   void WriteImageDefinitionFiducialList(CFormatStdioFile &file, SrffImageDefinition *imageDef);
   void WriteImageFiducial(CFormatStdioFile &file, SrffImageFiducial *srffFid);

   void WriteComponentDefinitionList(CFormatStdioFile &file);
   void WriteComponentDefinition(CFormatStdioFile &file, SrffComponentDefinition *compDef);
   void WriteComponentLinkList(CFormatStdioFile &file);
   void WriteComponentLink(CFormatStdioFile &file, SrffComponentLink *compLink);
   void WritePatternDefinitionList(CFormatStdioFile &file);
   void WritePatternDefinition(CFormatStdioFile &file, SrffPatternDefinition *patDef);
   void WriteShapeList(CFormatStdioFile &file);
   void WriteShape(CFormatStdioFile &file, SrffShape *shape);
   void WriteCyberShapeList(CFormatStdioFile &file);
   void WriteCyberShape(CFormatStdioFile &file, CyberShape *shape);
   void WriteCyberImageNameList(CFormatStdioFile &file);
   void WriteCyberImageName(CFormatStdioFile &file, SrffImage *image, int id);

   void AddPolyToCyberShape(CPoly *poly, CyberShape *shape);
   

   //
   // Process Data
   //
   void WriteProcessSectionData(CFormatStdioFile &file);

   //
   // The two basic sections, top level writers
   //
   void WriteSchema(CFormatStdioFile &file);
   void WriteData(CFormatStdioFile &file);

public:
   SRFFWriter::SRFFWriter(CCEtoODBDoc &doc);
   void WriteFile(CFormatStdioFile &file);

   BlockStruct *GetBlockAt(int indx)   { return m_doc.getBlockAt(indx); }
   double GetUnitsFactor()             { return m_outputUnitsFactor; }
   double ConvertUnits(double cczVal)  { return (cczVal * m_outputUnitsFactor); }
   void SetOutputUnits(PageUnitsTag units);
   CString GetSrffOutputUnitsName();
   int GetDecimals();
   CString FormatValueStr(double val);

   int GetScopeIndentSize()            { return m_indentSize; }
   void SetScopeIndentSize(int size)   { m_indentSize = ((size >= INDENT_SIZE_MIN && size <= INDENT_SIZE_MAX) ? size : INDENT_SIZE_DEFAULT); }

   void SetProductName(CString name)   { m_productName = name; }
   void SetProductNote(CString note)   { m_productNote = note; }
   void SetPanelSize(double sizeX, double sizeY);

   void AddImage(int cczBlockNum, CString refname, double locX, double locY, double rotDeg, int imageDefinitionId);
   SrffImageDefinition *LookupImageDefinition(BlockStruct *cczBlock, bool mirrored);
   SrffImageDefinition *AddImageDefinition(BlockStruct *cczBlock, bool mirrored);
   SrffImageDefinition *AddImageDefinition(int cczBlockNum, CString imageDefName, bool mirrored);

   SrffComponentLink *AddComponentLink(BlockStruct *pcbComponentBlock);
   SrffComponentDefinition *AddComponentDefinition(CString partnumber, int componentLinkId);
   SrffComponentDefinition *LookupComponentDefinition(CString partnumber);

   SrffPatternDefinition *AddPatternDefinition(BlockStruct *geomBlock, CString patternName, int compLinkId);
   SrffShape *AddShape(SrffShapeTag shapeTag, double sizeA, double sizeB);
   SrffShape *AddShape(BlockStruct *apertureBlock);

   SrffFeature *AddFeature(CString name, double posx, double psy, double rotDeg, int refShapeId);

   int GetNextCyberSegmentId()      { return (++m_lastCyberSegmentId); }
   int GetNextImageFiducialId()     { return (++m_lastImageFiducialId); }

};


////////////////////////////////////////////////////////////
// CyberOpticsAOIOutputAttrib
////////////////////////////////////////////////////////////
class CyberOpticsAOIOutputAttrib
{
public:
   CyberOpticsAOIOutputAttrib(CCEtoODBDoc& camCadDoc);
   ~CyberOpticsAOIOutputAttrib();

private:
   CCEtoODBDoc& m_camCadDoc; 
   int m_attribKeyword;
   CMapStringToString m_valuesMap;

public:
   int GetKeyword() const;
   bool HasValue(CAttributes** attributes);
   void SetKeyword(CString keyword);
   void AddValue(CString value);
   void SetDefaultAttribAndValue();
   bool IsValid() {return ( (m_attribKeyword > -1) && (!m_valuesMap.IsEmpty()) );}
};

////////////////////////////////////////////////////////////
// CCyberOpticsAOISettings
////////////////////////////////////////////////////////////
class CCyberOpticsAOISettings : public BaseAoiExporterSettings
{
private:
   int                     m_scopeIndentSize; // Number of spaces, 0 to 15

public:
   CCyberOpticsAOISettings(CCEtoODBDoc& doc);
   ~CCyberOpticsAOISettings();

   virtual void SetLocalDefaults();
   virtual bool ParseLocalCommand(CInFile &inFile);

   void LoadDefaultSettings();
   bool LoadStandardSettingsFile(CString FileName);

   int GetScopeIndentSize()      { return m_scopeIndentSize; }
};

////////////////////////////////////////////////////////////
// CCyberOpticsAOIInsertData
////////////////////////////////////////////////////////////
class CCyberOpticsAOIInsertData 
{
private:
   double   m_rotation;
   double   m_posX;
   double   m_posY;
   CString  m_PartNumber;
   CString  m_refName;
   int      m_pinCount;
   bool     m_placedBottom;

   DataStruct *m_InsertData;
   BlockStruct *m_insertedBlock;

   BlockTypeTag   m_BoardType;
   CCEtoODBDoc&    m_camCadDoc;
   CCamCadDatabase   m_camCadDatabase;

public:
   CCyberOpticsAOIInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data);

   DataStruct* getInsertData(){return m_InsertData;}
   void setInsertData(DataStruct *data){m_InsertData = data;}

   void setRotation(double rotation);
   double getRotation(){return m_rotation;}

   void setX(double pos){m_posX = pos;}
   double getX(){return m_posX;}

   void setY(double pos){m_posY = pos;}
   double getY(){return m_posY;}

   void setPartNumber(CString part){m_PartNumber = part;}
   CString &getPartNumber(){return m_PartNumber;}

   void setRefName(CString refname){m_refName = refname;}
   CString &getRefName(){return m_refName;}

   void setBoardType(BlockTypeTag type){m_BoardType = type;}
   BlockTypeTag getBoardType(){return m_BoardType;}

   void setPlacedBottom(bool isBottom){m_placedBottom = isBottom;}
   bool getPlacedBottom(){return m_placedBottom;}

   void setInsertedBlock(BlockStruct *b) { m_insertedBlock = b; }
   BlockStruct *GetInsertedBlock()       { return m_insertedBlock; }
   CString getInsertedBlockName()        { return  m_insertedBlock != NULL ? m_insertedBlock->getName() : "NULL"; }
   int     getInsertedBlockNumber()      { return  m_insertedBlock != NULL ? m_insertedBlock->getBlockNumber() : -1; }

   InsertTypeTag getInsertType()         { return (m_InsertData != NULL && m_InsertData->getDataType() == dataTypeInsert) ? m_InsertData->getInsert()->getInsertType() : insertTypeUnknown; }
};

////////////////////////////////////////////////////////////
// CCyberOpticsAOIInsertDataList
////////////////////////////////////////////////////////////
class CCyberOpticsAOIInsertDataList : public CTypedPtrArrayContainer<CCyberOpticsAOIInsertData*>
{
private:
   int m_topInserts;
   int m_botInserts;
   CCEtoODBDoc& m_camCadDoc;

public:
   CCyberOpticsAOIInsertDataList(CCEtoODBDoc& camCadDoc);
   ~CCyberOpticsAOIInsertDataList();
   void  Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, CString partNumber, BlockTypeTag boardType, bool placedBottom);
   int   getInsertsCountBySide(int isBottom){ return ((isBottom)?m_botInserts:m_topInserts);}
};

////////////////////////////////////////////////////////////
// CCyberOpticsAOIPCBData
////////////////////////////////////////////////////////////
class CCyberOpticsAOIPCBData
{
private:
   CCyberOpticsAOIInsertDataList    m_FidDataList;
   CCyberOpticsAOIInsertDataList   m_CompDataList;

   double   m_originX;
   double   m_originY;
   BlockTypeTag m_boardType;
   BlockStruct*   m_geometryBlock;
   DataStruct*    m_insertData;

public:
   CCyberOpticsAOIPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry);
   ~CCyberOpticsAOIPCBData();
   CCyberOpticsAOIInsertDataList &getFiducialDataList(){return m_FidDataList;}
   CCyberOpticsAOIInsertDataList &getComponentDataList(){return m_CompDataList;}
   double  getOriginX(){return m_originX;}
   double  getOriginY(){return m_originY;}
   double  getRotDeg() { return (m_insertData != NULL) ? (m_insertData->getInsert()->getAngleDegrees()) : (0.); }

   BlockTypeTag getBoardType(){return m_boardType;}
   BlockStruct* getGeometryBlock(){return m_geometryBlock;}

   int getFidCount(bool isBottom)      { return m_FidDataList.getInsertsCountBySide(isBottom);  }
   int getCompCount(bool isBottom)     { return m_CompDataList.getInsertsCountBySide(isBottom); }

};

////////////////////////////////////////////////////////////
// CCyberOpticsAOIPCBDataList
////////////////////////////////////////////////////////////
class CCyberOpticsAOIPCBDataList : public CTypedPtrMap<CMapStringToPtr,CString,CCyberOpticsAOIPCBData*>
{
private:
   bool  m_Panelflag;
   CString m_BoardName;

public:
   CCyberOpticsAOIPCBDataList();
   ~CCyberOpticsAOIPCBDataList();

   void  empty();
   bool  IsPanel(){return m_Panelflag;}
   void  setPanelFlag(bool flag){m_Panelflag = flag;}
   void  setBoardName(CString boardname){m_BoardName = (!boardname.IsEmpty())?boardname:QCYBERO_BOARDNAME;}
   void  SetBoardAt(CString boardName, CCyberOpticsAOIPCBData *pcbdata);

   CString getBoardName(){ return m_BoardName;}
};

////////////////////////////////////////////////////////////
// CyberOpticsAOIWriter
////////////////////////////////////////////////////////////

class CyberOpticsAOIWriter : public BaseAoiExporter
{
private:
   COperationProgress    m_writeProcess;
   CCEtoODBDoc           &m_doc;

   SRFFWriter m_srffTop;  // SRFF info for top file
   SRFFWriter m_srffBot;  // SRFF infor for bottom file
   SRFFWriter &GetSrffWriter(bool isBottom) { return isBottom ? m_srffBot : m_srffTop; }

   double               m_Scale;
   CString              m_topSideFilename;
   CString              m_botSideFilename;
   CString              m_targetPCBName;

   double               m_outlineLLX;  // Outline lower left X in Saki AOI units
   double               m_outlineLLY;  // Outline lower left Y in Saki AOI units

   int                  m_deviceTypeKW;  // device type attribute keyword index

   CMapStringToPtr m_usedRefnameMap; // Collects refnames used in output, during output, support making unique refnames.
   CString getUniqueName(CString name); // Handles suffixing refnames to make unique names in output.

private:
   CCyberOpticsAOISettings *GetSettings()  { return (CCyberOpticsAOISettings*)(BaseAoiExporter::GetSettings()); }

   double   getCyberOpticsAOIUnits(double value){ return (value /* * m_dUnitFactor */); }  //*rcf Delete this, not needed now.

   void setOutlineLowerLeft(double llx, double lly)  { m_outlineLLX = llx; m_outlineLLY = lly; }
   double applySakiOriginX(double rawSakiX);
   double applySakiOriginY(double rawSakiY);

   CString getOutputFilename(bool isBottom)  { return isBottom ? m_botSideFilename : m_topSideFilename; }

   double   getRotationDegree(double radius, int mirror);
   CString  getPartNumber(DataStruct* data);
   void  TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data);

   bool  WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom);

   CExtent GetOutlineExtent();
   void GenerateOutputFilenames(CString incomingName, CString &topSideName, CString &botSideName);

   void FillImageList(BaseAoiExporterPCBData *panel, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void AddImageForPcbData(SRFFWriter &srffWriter, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void AddImageForPanelData(SRFFWriter &srffWriter, BaseAoiExporterPCBData *panelData, bool isBottom);

   void AddImageDefCompLocations(SrffImageDefinition *imageDef, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void CollectComponentDefinitions(BaseAoiExporterPCBData *pcbData, bool isBottom);

   void CollectFiducials(SrffImageDefinition *imageDef, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom);

public:
   CyberOpticsAOIWriter(CCEtoODBDoc &doc, double l_Scale);
   ~CyberOpticsAOIWriter();

   void  InitSrffWriter(CFormatStdioFile &File, bool isBottom);

   void GetLineConfigSettings(FileStruct* fileStruct, bool isBottom, ExportFileSettings &exportSettings);

   int GetComponentCount(bool isBottom);

   //---------------------------------------------------------------

   virtual int GetDecimals(PageUnitsTag pageUnits);

   virtual bool WriteLayoutFiles() { return true; /* no-op in this format */ }
   virtual bool WritePanelFile(BaseAoiExporterPCBData *pcbData, bool isBottom);
   virtual bool WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName);
   virtual CString RenameFiducial(CString fidname, int intFidID, bool isPanelFid);

};



#endif /*_CyberOpticsAOI_OUT_H*/
