#if !defined(_OMRONAOI_OUT_H)
#define _OMRONAOI_OUT_H


#pragma once

#include "BaseAoiExporter.h"



////////////////////////////////////////////////////////////
// OmronAOISettings
////////////////////////////////////////////////////////////
class OmronAOISettings : public BaseAoiExporterSettings
{
private:
   CString                 m_attribKeyword;
   CStringArray            m_attribValue;

   CString                 m_componentNumberValueFrom;   // Name of data source for Component Number field

   // Rail Width
   // Can use one or the other at a time, not both.
   // If both are specified then explicit m_railWidth takes precedence.
   int                     m_railWidth;  // A value in output units to be output directly as-is
   int                     m_railWidthMargin;  // A value to add to X dim of PCB to get rail width

public:
   OmronAOISettings(CCEtoODBDoc& doc);
   ~OmronAOISettings();

   virtual void SetLocalDefaults();
   virtual bool ParseLocalCommand(CInFile &inFile);

   CString getComponentNumberKeyword()  { return m_componentNumberValueFrom; }

   CString &getAttribKeyword(){ return m_attribKeyword;}
   CStringArray &getAttribValueArray(){ return m_attribValue;}

   int getRailWidth()         { return m_railWidth; }
   int getRailWidthMargin()   { return m_railWidthMargin; }
};

//------------------------------------------------

#define QOMRON_BOARDNAME "Board"
#define QGEOM_NAME           "GEOM_NAME"

////////////////////////////////////////////////////////////
// OmronAOIOutputAttrib
////////////////////////////////////////////////////////////
class OmronAOIOutputAttrib
{
public:
   OmronAOIOutputAttrib(CCEtoODBDoc& camCadDoc);
   ~OmronAOIOutputAttrib();

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
// COmronAOISetting
////////////////////////////////////////////////////////////
class COmronAOISetting
{
private:
   CString                 m_PartNumberKW;
   bool                    m_exportUNLoaded;
   PageUnitsTag            m_outputPageUnits;
   double                  m_outputPageUnitsScale;
   CString                 m_attribKeyword;
   CStringArray            m_attribValue;
   double                  m_originOutputUnitsX;  // Origin X in output units
   double                  m_originOutputUnitsY;  // Origin Y in output units

   CString                 m_componentNumberValueFrom;   // Name of data source for Component Number field

   // Rail Width
   // Can use one or the other at a time, not both.
   // If both are specified then explicit m_railWidth takes precedence.
   int                     m_railWidth;  // A value in output units to be output directly as-is
   int                     m_railWidthMargin;  // A value to add to X dim of PCB to get rail width

public:
   COmronAOISetting();
   ~COmronAOISetting();

   void LoadDefaultSettings();
   bool LoadStandardSettingsFile(CString FileName);

   CString getComponentNumberKeyword()  { return m_componentNumberValueFrom; }

   CString &getPartNumberKW(){return m_PartNumberKW;}
   bool getExportUNLoaded(){return m_exportUNLoaded;}

   PageUnitsTag getOutputPageUnits(){ return m_outputPageUnits;}
   //void setOutputPageUnits(PageUnitsTag pageUnits){ m_outputPageUnits = pageUnits;}

   double getOutputPageUnitsScale() { return m_outputPageUnitsScale; }

   CString &getAttribKeyword(){ return m_attribKeyword;}
   CStringArray &getAttribValueArray(){ return m_attribValue;}

   double getBoardOriginX()      { return m_originOutputUnitsX; }
   double getBoardOriginY()      { return m_originOutputUnitsY; }

   int getRailWidth()         { return m_railWidth; }
   int getRailWidthMargin()   { return m_railWidthMargin; }
};

////////////////////////////////////////////////////////////
// COmronAOIInsertData
////////////////////////////////////////////////////////////
class COmronAOIInsertData 
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
   COmronAOIInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data);

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
   CString getInsertedBlockName()        { return  m_insertedBlock != NULL ? m_insertedBlock->getName() : "NULL"; }

   InsertTypeTag getInsertType()         { return (m_InsertData != NULL && m_InsertData->getDataType() == dataTypeInsert) ? m_InsertData->getInsert()->getInsertType() : insertTypeUnknown; }
};

////////////////////////////////////////////////////////////
// COmronAOIInsertDataList
////////////////////////////////////////////////////////////
class COmronAOIInsertDataList : public CTypedPtrArrayContainer<COmronAOIInsertData*>
{
private:
   int m_topInserts;
   int m_botInserts;
   CCEtoODBDoc& m_camCadDoc;

public:
   COmronAOIInsertDataList(CCEtoODBDoc& camCadDoc);
   ~COmronAOIInsertDataList();
   void  Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, CString partNumber, BlockTypeTag boardType, bool placedBottom);
   int   getInsertsCountBySide(int isBottom){ return ((isBottom)?m_botInserts:m_topInserts);}
};

////////////////////////////////////////////////////////////
// COmronAOIPCBData
////////////////////////////////////////////////////////////
class COmronAOIPCBData
{
private:
   COmronAOIInsertDataList    m_FidDataList;
   COmronAOIInsertDataList   m_CompDataList;

   double   m_originX;
   double   m_originY;
   BlockTypeTag m_boardType;
   BlockStruct*   m_geometryBlock;
   DataStruct*    m_insertData;

public:
   COmronAOIPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry);
   ~COmronAOIPCBData();
   COmronAOIInsertDataList &getFiducialDataList(){return m_FidDataList;}
   COmronAOIInsertDataList &getComponentDataList(){return m_CompDataList;}
   double  getOriginX(){return m_originX;}
   double  getOriginY(){return m_originY;}

   BlockTypeTag getBoardType(){return m_boardType;}
   BlockStruct* getGeometryBlock(){return m_geometryBlock;}

   int getFidCount(bool isBottom)      { return m_FidDataList.getInsertsCountBySide(isBottom);  }
   int getCompCount(bool isBottom)     { return m_CompDataList.getInsertsCountBySide(isBottom); }

};

////////////////////////////////////////////////////////////
// COmronAOIPCBDataList
////////////////////////////////////////////////////////////
class COmronAOIPCBDataList : public CTypedPtrMap<CMapStringToPtr,CString,COmronAOIPCBData*>
{
private:
   bool  m_Panelflag;
   CString m_BoardName;

public:
   COmronAOIPCBDataList();
   ~COmronAOIPCBDataList();

   void  empty();
   bool  IsPanel(){return m_Panelflag;}
   void  setPanelFlag(bool flag){m_Panelflag = flag;}
   void  setBoardName(CString boardname){m_BoardName = (!boardname.IsEmpty())?boardname:QOMRON_BOARDNAME;}
   void  SetBoardAt(CString boardName, COmronAOIPCBData *pcbdata);

   CString getBoardName(){ return m_BoardName;}
};

////////////////////////////////////////////////////////////
// OmronAOIWriter
////////////////////////////////////////////////////////////
class OmronAOIWriter
{
private:
   COperationProgress     m_writeProcess;
   COmronAOISetting       m_outSetting;
   CCEtoODBDoc            &m_doc;
   FileStruct            *m_activeFile;
   COmronAOIPCBDataList   m_PCBDataList;
   OmronAOIOutputAttrib   m_attribMap;
   CLogWriteFormat        m_textLogFile;

   double               m_dUnitFactor;
   int                  m_unit_accuracy;
   CString              m_topSideFilename;
   CString              m_botSideFilename;
   CString              m_targetPCBName;

   double               m_outlineLLX;  // Outline lower left X in Saki AOI units
   double               m_outlineLLY;  // Outline lower left Y in Saki AOI units

   int                  m_deviceTypeKW;  // device type attribute keyword index

   int m_compRecordNum;  // A counter during output
   CMapStringToPtr m_usedRefnameMap; // Collects refnames used in output, during output, support making unique refnames.
   CString getUniqueName(CString name); // Handles suffixing refnames to make unique names in output.

private:
   COmronAOISetting& GetSettings(){return m_outSetting;}
   double   getOmronAOIUnitsD(double value){ return (value * m_dUnitFactor); }
   int      getOmronAOIUnits(double value) { return round(getOmronAOIUnitsD(value)); }

   void setOutlineLowerLeft(double llx, double lly)  { m_outlineLLX = llx; m_outlineLLY = lly; }
   double applyOmronOriginX(double rawSakiX);
   double applyOmronOriginY(double rawSakiY);

   CString getOutputFilename(bool isBottom)  { return isBottom ? m_botSideFilename : m_topSideFilename; }

   bool  IsLoaded(DataStruct* data);
   bool  IsWritePart(DataStruct* data);
   bool  isUniqueBoardNameOnPanel(BlockStruct* panelblock);

   double   getRotationDegree(double radious, int mirror);
   CString  getPartNumber(DataStruct* data);
   void  TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data);
   CString  ReNameFiducial(CString originalName, int intFidID);

   bool  WritePCBFile(COmronAOIPCBData *pcbData, CString boardName, bool isBottom);
   bool  WritePanelFile(COmronAOIPCBData *pcbData, bool isBottom);

   CExtent GetOutlineExtent();
   void GenerateOutputFilenames(CString incomingName, CString &topSideName, CString &botSideName);

   FileStruct *getActiveFile()   { return m_activeFile; }

   CString getComponentNumberValue(COmronAOIInsertData *sakiCmpData);
   CString getConfigurableValue(COmronAOIInsertData *sakiCmpData, CString dataSourceName);

public:
   OmronAOIWriter(CCEtoODBDoc &doc);
   ~OmronAOIWriter();

   FileStruct *DetermineActiveCCFile(CCEtoODBDoc *doc);

   bool  LoadSettingsFile();
   bool  WriteFiles(CString basefilename);
   bool  WritePCBFiles(COmronAOIPCBData *pcbData, CString boardName);
   bool  WritePanelFiles(COmronAOIPCBData *pcbData);

   void  WriteHeader(CFormatStdioFile &File, bool isBottom);
   void  WriteComponentList(CFormatStdioFile &File, COmronAOIPCBData *pcbData, bool isBottom);
   void  WriteComponents(CFormatStdioFile &File, int boardIdx, COmronAOIPCBData *pcbData, bool isBottom);
   void  WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, COmronAOIPCBData *pcbData, BlockTypeTag boardType, bool isBottom);
   void  WriteComponentRecord(CFormatStdioFile &File, int boardIdx, COmronAOIInsertData *sakiCmpData, bool isBottom);

   bool  ProcessPCBDesign();
   bool  ProcessPanel();

   void  ApplyPrepSettingToFiles(FileStruct* fileStruct, COmronAOIPCBData *pcbData, bool isBottom, bool isPanel);
   bool  TraversePCBDesign(BlockStruct* block, COmronAOIPCBData *pcbData, CTMatrix transformMatrix);
   bool  TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block);

   int GetFlg(COmronAOIInsertData *sakiCmpData);

   int GetComponentCount(bool isBottom);
};



class NewOmronAOIWriter : public BaseAoiExporter
{
private:
   FileStruct           *m_activeFile;
   int m_compRecordNum;  // A counter during output
   double               m_outlineLLX;  // Outline lower left X in Saki AOI units
   double               m_outlineLLY;  // Outline lower left Y in Saki AOI units
   int                  m_deviceTypeKW;  // device type attribute keyword index

   FileStruct *getActiveFile()   { return m_activeFile; }

   double getOmronAOIUnits(double value){ return ConvertToExporterUnits(value); }  //*rcf When getMydataUnits name if fixed up, get rid of getSakiUnits
   void   setOutlineLowerLeft(double llx, double lly)  { m_outlineLLX = llx; m_outlineLLY = lly; }
   double applySakiOriginX(double rawSakiX);
   double applySakiOriginY(double rawSakiY);

   bool  WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom);
   void  WriteHeader(CFormatStdioFile &File, bool isBottom);
   void  WriteComponentList(CFormatStdioFile &File, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void  WriteComponents(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void  WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom);
   void  WriteComponentRecord(CFormatStdioFile &File, int boardIdx, BaseAoiExporterInsertData *sakiCmpData, bool isBottom);

   bool  WritePanelFile(BaseAoiExporterPCBData *pcbData, bool isBottom);

   CString getComponentNumberValue(BaseAoiExporterInsertData *sakiCmpData);

   OmronAOISettings *GetSettings()  { return (OmronAOISettings*)(BaseAoiExporter::GetSettings()); }

public:
   NewOmronAOIWriter(CCEtoODBDoc &doc, double l_Scale);
   ~NewOmronAOIWriter();

   virtual int GetDecimals(PageUnitsTag pageUnits);

   virtual bool WriteLayoutFiles() { return true; /* no-op in this format */ }
   virtual bool WritePanelFiles(BaseAoiExporterPCBData *panelData);
   virtual bool WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName);
   virtual CString RenameFiducial(CString fidname, int intFidID, bool isPanelFid);

   FileStruct *DetermineActiveCCFile(CCEtoODBDoc *doc);

};


#endif /*_OMRONAOI_OUT_H*/
