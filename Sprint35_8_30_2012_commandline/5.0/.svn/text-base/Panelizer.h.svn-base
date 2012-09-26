
#if !defined(__Panelizer_h__)
#define __Panelizer_h__

#pragma once

#include "DcaContainer.h"
#include "DcaSortedMap.h"
#include "DcaUnits.h"
#include "DcaBasesVector.h"
#include "DcaExtent.h"
#include "FiducialGenerator.h"
#include "ODBC_Lib.h"

class CCamCadData;
class CCEtoODBDoc;
class CPanelizer;
class CWriteFormat;
class FileStruct;
class LayerStruct;
class BlockStruct;
class DataStruct;
class CDataList;
class CExtent;

enum FiducialTypeTag;

//_________________________________________________________________________________________________
enum PanelizerOriginTypeTag
{
   panelizerOriginTypeBoardOrigin,
   panelizerOriginTypeBoardCenter,
   panelizerOriginTypeUndefined,
   PanelizerOriginTypeTagMin = panelizerOriginTypeBoardOrigin,
   PanelizerOriginTypeTagMax = panelizerOriginTypeUndefined - 1,
};

CString PanelizerOriginTypeTagToString(PanelizerOriginTypeTag tagValue);
PanelizerOriginTypeTag StringToPanelizerOriginTypeTag(const CString& tagValue);

//_________________________________________________________________________________________________
enum PanelEntityTypeTag
{
   panelEntityTypeOutline,
   panelEntityTypePcbInstance,
   panelEntityTypePcbArray,
   panelEntityTypeFiducial,
   panelEntityTypeToolingHole,
   panelEntityTypeUndefined,
   PanelEntityTypeTagMin = 0,
   PanelEntityTypeTagMax = panelEntityTypeUndefined - 1
};

CString PanelEntityTypeTagToString(PanelEntityTypeTag tagValue);
PanelEntityTypeTag StringToPanelEntityTypeTag(const CString& tagValue);

//_________________________________________________________________________________________________
class CPanelTemplateEntity
{
private:
   CPanelizer& m_panelizer;
   int m_id;
   bool m_modifiedFlag;            // local update flag, goes on and off all the time, mostly controls GUI update
   bool m_modifiedSinceLoadFlag;   // tracks at higher level, for do-you-want-to-save upon Panelizer close or load a different template
   CDataList* m_entityDataList;

public:
   CPanelTemplateEntity(CPanelizer& panelizer);
   virtual ~CPanelTemplateEntity();

   int getId() const;
   void setId(int id);

   bool getModifiedFlag() const;
   void setModifiedFlag(bool flag);

   bool getModifiedSinceLoadFlag() const;
   void setModifiedSinceLoadFlag(bool flag);

   CPanelizer& getPanelizer() const;
   CCamCadData& getCamCadData();
   FileStruct& getPanelizerSubFile();
   LayerStruct& getPanelOutlineLayer();

   CString getUnitString(double value) const;
   CString getDegreesString(double value) const;
   CString getIntString(int value) const;
   CString getMirroredString(bool value) const;

   CDataList& getEmptyEntityList();

   virtual PanelEntityTypeTag getPanelEntityType() const = 0;
   virtual bool setValue(int col,const CString& stringValue,PageUnitsTag pageUnits=pageUnitsUndefined) = 0;
   virtual bool set(const CPanelTemplateEntity& other) = 0;
   virtual int getParamCount() const = 0;
   virtual CString getText(int index) const = 0;
   virtual CString getDescriptor() const;

   //virtual void mapLogicalNameToPhysicalName();

   virtual void write(CWriteFormat& writeFormat);

   virtual void regenerateFile();
   virtual void instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData);
   virtual void highlightEntity(bool highlightFlag);

   static bool parseYesNoParam(CString value,bool& flag);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityOutline : public CPanelTemplateEntity
{
private:
   double m_xOrigin;
   double m_yOrigin;
   double m_xSize;
   double m_ySize;

public:
   CPanelTemplateEntityOutline(CPanelizer& panelizer);

   CPanelTemplateEntityOutline& operator=(const CPanelTemplateEntityOutline& other);

   //double getXOrigin() const;
   //void setXOrigin(double ordinate);

   //double getYOrigin() const;
   //void setYOrigin(double ordinate);

   //double getXSize() const;
   //void setXSize(double size);

   //double getYSize() const;
   //void setYSize(double size);

   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual bool setValue(int col,const CString& stringValue,PageUnitsTag pageUnits=pageUnitsUndefined);
   virtual bool set(const CPanelTemplateEntity& other);
   virtual int getParamCount() const;
   virtual CString getText(int index) const;

   virtual void regenerateFile();
   virtual void instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData);

   CExtent GetExtent()  { CExtent extent(m_xOrigin, m_yOrigin, m_xOrigin + m_xSize, m_yOrigin + m_ySize); return extent; }

   CDBPanelTemplateOutline* AllocDBPanelTemplateOutline();
   void AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplateOutline *dboutline);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityPcbInstance : public CPanelTemplateEntity
{
private:
   CString m_name;
   double m_xOrigin;
   double m_yOrigin;
   double m_angleRadians;
   bool   m_mirrored;
   PanelizerOriginTypeTag m_originType;
   //CString m_physicalBoardName;

public:
   CPanelTemplateEntityPcbInstance(CPanelizer& panelizer);

   CPanelTemplateEntityPcbInstance& operator=(const CPanelTemplateEntityPcbInstance& other);

   //CString getName() const;
   //void setName(const CString& name);

   //double getXOrigin() const;
   //void setXOrigin(double ordinate);

   //double getYOrigin() const;
   //void setYOrigin(double ordinate);

   //double getAngle() const;
   //void setAngle(double degrees);

   //PanelizerOriginTypeTag getOriginType() const;
   //void setOriginType(PanelizerOriginTypeTag originType);

   //CString getPhysicalBoardName() const;
   //void setPhysicalBoardName(const CString& name);

   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual bool setValue(int col,const CString& stringValue,PageUnitsTag pageUnits=pageUnitsUndefined);
   virtual bool set(const CPanelTemplateEntity& other);
   virtual int getParamCount() const;
   virtual CString getText(int index) const;

   //virtual void mapLogicalNameToPhysicalName();
   void GetPlacementBasesVector(CBasesVector& placementBasesVector);

   // For display
   virtual void regenerateFile();
   // For actual panel createion
   virtual void instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData);

   CDBPanelTemplatePcbInstance* AllocDBPanelTemplatePcbInstance();
   void AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplatePcbInstance *dbpcbinst);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityPcbArray : public CPanelTemplateEntity
{
private:
   CString m_name;
   double m_xOrigin;
   double m_yOrigin;
   double m_angleRadians;
   bool   m_mirrored;
   PanelizerOriginTypeTag m_originType;
   int m_xCount;
   int m_yCount;
   double m_xSize;
   double m_ySize;
   //CString m_physicalBoardName;

public:
   CPanelTemplateEntityPcbArray(CPanelizer& panelizer);

   CPanelTemplateEntityPcbArray& operator=(const CPanelTemplateEntityPcbArray& other);

   CString getName() const;
   void setName(const CString& name);

   //double getXOrigin() const;
   //void setXOrigin(double ordinate);

   //double getYOrigin() const;
   //void setYOrigin(double ordinate);

   //double getAngle() const;
   //void setAngle(double degrees);

   //PanelizerOriginTypeTag getOriginType() const;
   //void setOriginType(PanelizerOriginTypeTag originType);

   int getXCount() const      { return m_xCount; } //*rcf If make array do violaton count then don't need this
   //void setXCount(int count);

   int getYCount() const      { return m_yCount; } //*rcf Or this
   //void setYCount(int count);

   //double getXSize() const;
   //void setXSize(double size);

   //double getYSize() const;
   //void setYSize(double size);

   //CString getPhysicalBoardName() const;
   //void setPhysicalBoardName(const CString& name);

   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual bool setValue(int col,const CString& stringValue,PageUnitsTag pageUnits=pageUnitsUndefined);
   virtual bool set(const CPanelTemplateEntity& other);
   virtual int getParamCount() const;
   virtual CString getText(int index) const;

   //virtual void mapLogicalNameToPhysicalName();
   void GetPlacementBasesVector(int colIndexIN, int rowIndexIN, CBasesVector& placementBasesVectorOUT);

   void insertArray(FileStruct& targetPanelFile,BlockStruct& geometry,CDataList& instantiatedData,bool drawGridLinesFlag);
   virtual void regenerateFile();
   virtual void instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData);

   CDBPanelTemplatePcbArray* AllocDBPanelTemplatePcbArray();
   void AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplatePcbArray *dbpcbar);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityFiducial : public CPanelTemplateEntity
{
private:
   FiducialTypeTag m_fiducialType;
   double m_size;
   double m_xOrigin;
   double m_yOrigin;
   double m_angleRadians;
   bool   m_mirrored;

public:
   CPanelTemplateEntityFiducial(CPanelizer& panelizer);

   CPanelTemplateEntityFiducial& operator=(const CPanelTemplateEntityFiducial& other);

   FiducialTypeTag getFiducialType() const;
   void setFiducialType(FiducialTypeTag fiducialType);

   //double getSize() const;
   //void setSize(double size);

   //double getXOrigin() const;
   //void setXOrigin(double ordinate);

   //double getYOrigin() const;
   //void setYOrigin(double ordinate);

   //double getAngle() const;
   //void setAngle(double degrees);

   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual bool setValue(int col,const CString& stringValue,PageUnitsTag pageUnits=pageUnitsUndefined);
   virtual bool set(const CPanelTemplateEntity& other);
   virtual int getParamCount() const;
   virtual CString getText(int index) const;

   virtual void regenerateFile();
   virtual void instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData);

   // Extent ignoring rotation
   CExtent GetExtent();

   CDBPanelTemplateFiducial* AllocDBPanelTemplateFiducial();
   void AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplateFiducial *dbfid);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityToolingHole : public CPanelTemplateEntity
{
private:
   double m_size;
   double m_xOrigin;
   double m_yOrigin;

public:
   CPanelTemplateEntityToolingHole(CPanelizer& panelizer);

   CPanelTemplateEntityToolingHole& operator=(const CPanelTemplateEntityToolingHole& other);

   //double getSize() const;
   //void setSize(double size);

   //double getXOrigin() const;
   //void setXOrigin(double ordinate);

   //double getYOrigin() const;
   //void setYOrigin(double ordinate);

   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual bool setValue(int col,const CString& stringValue,PageUnitsTag pageUnits=pageUnitsUndefined);
   virtual bool set(const CPanelTemplateEntity& other);
   virtual int getParamCount() const;
   virtual CString getText(int index) const;

   virtual void regenerateFile();
   virtual void instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData);

   CExtent GetExtent()  { double halfsize = m_size * 0.5; CExtent extent(m_xOrigin - halfsize, m_yOrigin - halfsize, m_xOrigin + halfsize, m_yOrigin + halfsize); return extent; }

   CDBPanelTemplateToolingHole* AllocDBPanelTemplateToolingHole();
   void AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplateToolingHole *dbth);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityArray
{
private:
   CPanelizer& m_panelizer;

public:
   CPanelTemplateEntityArray(CPanelizer& panelizer);
   virtual void empty() = 0;

   CPanelizer& getPanelizer();

   bool getModifiedFlag() const;
   void setModifiedFlag(bool flag);

   bool getModifiedSinceLoadFlag() const;
   void setModifiedSinceLoadFlag(bool flag);

   virtual int getSize() const = 0;
   virtual PanelEntityTypeTag getPanelEntityType() const = 0;
   virtual CPanelTemplateEntity* addEntity() = 0;
   virtual CPanelTemplateEntity* addEntityAt(int index) = 0;
   virtual CPanelTemplateEntity* addParsedEntity(const CStringArray& params,PageUnitsTag pageUnits,int offset=1);
   virtual CPanelTemplateEntity* getEntityAt(int index) const = 0;
   virtual void deleteAt(int index) = 0;
   virtual void deleteFirst();
   virtual void deleteLast();
   void copyEntities(const CPanelTemplateEntityArray& sourceArray,int insertIndex);
   void renumber();

   //virtual void setPhysicalNameForLogicalName(const CString& physicalName,const CString& logicalName);
   virtual void regenerateFile();
   virtual void instantiateFile(FileStruct& targetPanelFile);

   virtual void write(CWriteFormat& writeFormat);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityOutlineArray : public CPanelTemplateEntityArray
{
private:
   CTypedPtrArrayContainer<CPanelTemplateEntityOutline*> m_array;

public:
   CPanelTemplateEntityOutlineArray(CPanelizer& panelizer);
   virtual void empty();

   virtual int getSize() const;
   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual CPanelTemplateEntity* addEntity();
   virtual CPanelTemplateEntity* addEntityAt(int index);
   //virtual CPanelTemplateEntity* addEntity(const CStringArray& params,int offset=1);
   virtual CPanelTemplateEntity* getEntityAt(int index) const;
   virtual void deleteAt(int index);

   CPanelTemplateEntityOutline* add();
   CPanelTemplateEntityOutline* getAt(int index) const;

   CExtent GetExtent();

   bool PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate);
   bool AdoptDBValues(CDBPanelTemplate &dbpanelTemplate);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityPcbInstanceArray : public CPanelTemplateEntityArray
{
private:
   CTypedPtrArrayContainer<CPanelTemplateEntityPcbInstance*> m_array;

public:
   CPanelTemplateEntityPcbInstanceArray(CPanelizer& panelizer);
   virtual void empty();

   virtual int getSize() const;
   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual CPanelTemplateEntity* addEntity();
   virtual CPanelTemplateEntity* addEntityAt(int index);
   //virtual CPanelTemplateEntity* addEntity(const CStringArray& params,int offset=1);
   virtual CPanelTemplateEntity* getEntityAt(int index) const;
   virtual void deleteAt(int index);

   //virtual void setPhysicalNameForLogicalName(const CString& physicalName,const CString& logicalName);
   int ValidateLayout(CExtent& panelExtent);

   CPanelTemplateEntityPcbInstance* add();
   CPanelTemplateEntityPcbInstance* getAt(int index) const;   

   bool PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate);
   bool AdoptDBValues(CDBPanelTemplate &dbpanelTemplate);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityPcbArrayArray : public CPanelTemplateEntityArray
{
private:
   CTypedPtrArrayContainer<CPanelTemplateEntityPcbArray*> m_array;

public:
   CPanelTemplateEntityPcbArrayArray(CPanelizer& panelizer);
   virtual void empty();

   virtual int getSize() const;
   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual CPanelTemplateEntity* addEntity();
   virtual CPanelTemplateEntity* addEntityAt(int index);
   //virtual CPanelTemplateEntity* addEntity(const CStringArray& params,int offset=1);
   virtual CPanelTemplateEntity* getEntityAt(int index) const;
   virtual void deleteAt(int index);

   //virtual void setPhysicalNameForLogicalName(const CString& physicalName,const CString& logicalName);
   int ValidateLayout(CExtent& panelExtent);

   CPanelTemplateEntityPcbArray* add();
   CPanelTemplateEntityPcbArray* getAt(int index) const;

   bool PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate);
   bool AdoptDBValues(CDBPanelTemplate &dbpanelTemplate);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityFiducialArray : public CPanelTemplateEntityArray
{
private:
   CTypedPtrArrayContainer<CPanelTemplateEntityFiducial*> m_array;

public:
   CPanelTemplateEntityFiducialArray(CPanelizer& panelizer);
   virtual void empty();

   virtual int getSize() const;
   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual CPanelTemplateEntity* addEntity();
   virtual CPanelTemplateEntity* addEntityAt(int index);
   //virtual CPanelTemplateEntity* addEntity(const CStringArray& params,int offset=1);
   virtual CPanelTemplateEntity* getEntityAt(int index) const;
   virtual void deleteAt(int index);

   CPanelTemplateEntityFiducial* add();
   CPanelTemplateEntityFiducial* getAt(int index) const;

   int ValidateLayout(CExtent& panelExtent);

   bool PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate);
   bool AdoptDBValues(CDBPanelTemplate &dbpanelTemplate);
};

//_________________________________________________________________________________________________
class CPanelTemplateEntityToolingHoleArray : public CPanelTemplateEntityArray
{
private:
   CTypedPtrArrayContainer<CPanelTemplateEntityToolingHole*> m_array;

public:
   CPanelTemplateEntityToolingHoleArray(CPanelizer& panelizer);
   virtual void empty();

   virtual int getSize() const;
   virtual PanelEntityTypeTag getPanelEntityType() const;
   virtual CPanelTemplateEntity* addEntity();
   virtual CPanelTemplateEntity* addEntityAt(int index);
   //virtual CPanelTemplateEntity* addEntity(const CStringArray& params,int offset=1);
   virtual CPanelTemplateEntity* getEntityAt(int index) const;
   virtual void deleteAt(int index);

   CPanelTemplateEntityToolingHole* add();
   CPanelTemplateEntityToolingHole* getAt(int index) const;  

   int ValidateLayout(CExtent& panelExtent);

   bool PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate);
   bool AdoptDBValues(CDBPanelTemplate &dbpanelTemplate);
};

//_________________________________________________________________________________________________
class CPanelizerPcbSubFileEntry : public CObject
{
private:
   CPanelizer& m_panelizer;
   CString m_subFileName;
   FileStruct& m_subFile;
   BlockStruct* m_pcbOutlineGeometry;
   CExtent* m_pcbExtent;

public:
   CPanelizerPcbSubFileEntry(CPanelizer& panelizer,const CString& subFileName,FileStruct& subFile);
   ~CPanelizerPcbSubFileEntry();

   CCamCadData& getCamCadData();
   CPanelizer& getPanelizer();

   CString getSubFileName() const;

   FileStruct& getFileStruct() const;

   BlockStruct* getPcbOutlineGeometry();

   BlockStruct* constructPcbOutlineGeometry();

   const CExtent& getExtent();
};

//_________________________________________________________________________________________________
class CPanelizerPcbSubFileMap
{
private:
   CPanelizer& m_panelizer;
   CTypedMapSortStringToObContainer<CPanelizerPcbSubFileEntry> m_subFileMap;  // maps physical board names to their FileStruct
   CMapStringToString m_logicalNameToPhysicalNameMap;  // a logicalName can map to only one physical name,
                                                       // multiple logical names can map to the same physical name

public:
   CPanelizerPcbSubFileMap(CPanelizer& panelizer);

   CPanelizerPcbSubFileEntry* addEntry(FileStruct* subFile);
   CPanelizerPcbSubFileEntry* getEntry(const CString& physicalName);
   CString getPcbFilesLabelString();

   void mapLogicalNameToPhysicalName(const CString& logicalName,const CString& physicalName);
   CString getPhysicalNameForLogicalName(const CString& logicalName);

   CString getUniqueLogicalName();

   POSITION getStartPosition() const;
   const CPanelizerPcbSubFileEntry* getNext(POSITION& pos) const;
   const CPanelizerPcbSubFileEntry* getHead() const;

   int getPhysicalPcbFileCount() const;
};

//_________________________________________________________________________________________________
class CPanelTemplate
{
private:
   CString m_templateName;

   CPanelizer& m_panelizer;

   CPanelTemplateEntityOutlineArray     m_outlines;
   CPanelTemplateEntityPcbInstanceArray m_pcbInstances;
   CPanelTemplateEntityPcbArrayArray    m_pcbArrays;
   CPanelTemplateEntityFiducialArray    m_fiducials;
   CPanelTemplateEntityToolingHoleArray m_toolingHoles;

public:
   CPanelTemplate(CPanelizer& panelizer);
   void empty();

   bool getModifiedFlag() const;
   void setModifiedFlag(bool flag);

   bool getModifiedSinceLoadFlag() const;
   void setModifiedSinceLoadFlag(bool flag);

   CString GetName()          { return m_templateName; }
   void SetName(CString name) { m_templateName = name; }

   CPanelizer& getPanelizer();
   CCEtoODBDoc& getCamCadDoc();
   CCamCadData& getCamCadData();
   FileStruct& getPanelizerSubFile();

   CPanelTemplateEntityOutlineArray&     getOutlines()          { return m_outlines; }
   CPanelTemplateEntityPcbInstanceArray& getPcbInstances()      { return m_pcbInstances; }
   CPanelTemplateEntityPcbArrayArray&    getPcbArrays()         { return m_pcbArrays; }
   CPanelTemplateEntityFiducialArray&    getFicucials()         { return m_fiducials; }
   CPanelTemplateEntityToolingHoleArray& getToolingHoles()      { return m_toolingHoles; }
   CPanelTemplateEntityArray&            getEntityArray(PanelEntityTypeTag entityType);
   const CPanelTemplateEntityArray&      getEntityArray(PanelEntityTypeTag entityType) const;

   int addCopyOfEntity(const CPanelTemplateEntity& entity);
   void copyEntities(const CPanelTemplate& sourcePanelTemplate,PanelEntityTypeTag entityType,int insertIndex);
   void deleteEntity(PanelEntityTypeTag entityType,int index);

   //void setPhysicalNameForLogicalName(const CString& physicalName,const CString& logicalName);
   void regenerateFile();
   void instantiateFile(FileStruct& targetPanelFile);

   // Validation
   bool ValidateLayout(CString &msg);

   // Text file based storage, a temporary measure during development, but could be handy later
   void write(CWriteFormat& writeFormat);
   void readFile(const CString& filePath);

   // Database Storage and Retrieval
   bool PopulateDBPanelTemplate(CDBPanelTemplate &panelTemplate);
   bool SaveToDB();
   bool LoadFromDB(CString templateName);
   bool AdoptDBValues(CDBPanelTemplate &dbpanelTemplate);
};

//_________________________________________________________________________________________________
class CPanelizer
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CPanelTemplate m_panelTemplate;
   CPanelizerPcbSubFileMap m_pcbSubFileMap;
   FileStruct* m_panelizerSubFile;
   CString m_panelizerFileName;
   BlockStruct* m_defaultPcbOutlineGeometry;
   CFiducialGenerator m_fiducialGenerator;

   LayerStruct* m_panelOutlineLayer;
   LayerStruct* m_pcbOutlineLayer;
   LayerStruct* m_pcbArrayGridLayer;
   LayerStruct* m_padTopLayer;
   LayerStruct* m_padBottomLayer;
   LayerStruct* m_maskTopLayer;
   LayerStruct* m_maskBottomLayer;

public:
   CPanelizer(CCEtoODBDoc& camCadDoc);
   ~CPanelizer();

   CCEtoODBDoc& getCamCadDoc() const;
   CCamCadData& getCamCadData() const;
   CPanelTemplate& getPanelTemplate();
   FileStruct& getPanelizerSubFile();
   const CPanelizerPcbSubFileMap& getPcbSubFileMap() const;
   CFiducialGenerator& getFiducialGenerator();

   PageUnitsTag getPageUnits();

   CString getPcbFilesLabelString();

   void mapLogicalNameToPhysicalName(const CString& logicalName,const CString& physicalName);
   CString getPhysicalNameForLogicalName(const CString& logicalName);
   BlockStruct* getOutlineGeometryForLogicalName(const CString& logicalName);
   BlockStruct* getPcbGeometryForLogicalName(const CString& logicalName);
   CExtent getPcbExtentForLogicalName(const CString& logicalName);
   CString getUniqueLogicalName();
   void regenerateFile();
   FileStruct *instantiateFile(CString panelName);
   void updateModifiedPanel();
   int getPadTopLayerIndex();
   int getMaskTopLayerIndex();
   BlockStruct* createFiducialGeometry(FiducialTypeTag fiducialTypeTag, const double size);
   BlockStruct* getDefaultPcbOutlineGeometry();

   void AutoAssignPhysicalPcb();

   LayerStruct& getPanelOutlineLayer();
   LayerStruct& getPcbOutlineLayer();
   LayerStruct& getPcbArrayGridLayer();
   LayerStruct& getPadTopLayer();
   LayerStruct& getPadBottomLayer();
   LayerStruct& getMaskTopLayer();
   LayerStruct& getMaskBottomLayer();


private:
   void scanForPcbSubFiles();
};

#endif
