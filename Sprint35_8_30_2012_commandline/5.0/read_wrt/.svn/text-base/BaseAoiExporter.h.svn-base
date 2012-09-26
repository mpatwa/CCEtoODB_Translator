// $Header: /CAMCAD/5.0/read_wrt/MyData_Out.h 2     01/06/09 9:51a Sharry Huang $
#if !defined(_BASE_AOI_EXPORTER_H)
#define _BASE_AOI_EXPORTER_H

#pragma once
#include "ccdoc.h"
#include "General.h"
#include "CamCadDatabase.h"
#include "DcaCamcadFileWriter.h"
#include "afxwin.h"
#include "DcaWriteFormat.h"
#include "Extents.h"
#include "InFile.h"
#include "Gauge.h"
#include "RwLib.h"

#define QDEFAULT_BOARDNAME "$Board"

#define QSUFFIX_TOP        "_top"
#define QSUFFIX_BOT        "_bot"

#define QGEOM_NAME  "GEOM_NAME"



////////////////////////////////////////////////////////////
// BaseAoiExporterOutputAttrib
////////////////////////////////////////////////////////////
class BaseAoiExporterOutputAttrib
{
public:
	BaseAoiExporterOutputAttrib(CCEtoODBDoc& camCadDoc);
	~BaseAoiExporterOutputAttrib();

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
   bool IsValid() {return ( (m_attribKeyword > -1) && (!m_valuesMap.IsEmpty()) );}
};


////////////////////////////////////////////////////////////
// BaseAoiExporterSettings
////////////////////////////////////////////////////////////
class BaseAoiExporterSettings
{
private:
   CString              m_settingsFilename;

protected:
   CString              m_PartNumberKW;
   bool                 m_exportUNLoaded;
   PageUnitsTag         m_outputPageUnits;
   double               m_outputPageUnitsScale;

   BaseAoiExporterOutputAttrib   m_attribMap;
   
public:
   BaseAoiExporterSettings(CString filename, CCEtoODBDoc& camCadDoc);

   void SetDefaults();
   bool LoadSettingsFile();

   BaseAoiExporterOutputAttrib &GetOutputAttribMap()  { return m_attribMap; }

   CString &getPartNumberKW(){return m_PartNumberKW;}

   bool getExportUNLoaded(){return m_exportUNLoaded;}
   
   PageUnitsTag getOutputPageUnits()               { return m_outputPageUnits;}
   void setOutputPageUnits(PageUnitsTag pageUnits) { m_outputPageUnits = pageUnits;}

   double getOutputPageUnitsScale() { return m_outputPageUnitsScale; }

   // This is for subclass to implement, to allow reset and
   // parsing of subclass's local settings.
   virtual void SetLocalDefaults() {}
   virtual bool ParseLocalCommand(CInFile &inFile) { return false; }
};


////////////////////////////////////////////////////////////
// BaseAoiExporterInsertData
////////////////////////////////////////////////////////////
class BaseAoiExporterInsertData 
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
   BaseAoiExporterInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data);

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
// BaseAoiExporterInsertDataList
////////////////////////////////////////////////////////////
class BaseAoiExporterInsertDataList : public CTypedPtrArrayContainer<BaseAoiExporterInsertData*>
{
private:
   int m_topInserts;
   int m_botInserts;
   CCEtoODBDoc& m_camCadDoc;

public:
   BaseAoiExporterInsertDataList(CCEtoODBDoc& camCadDoc);
   ~BaseAoiExporterInsertDataList();
   void  Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, CString partNumber, BlockTypeTag boardType, bool placedBottom);
   int   getInsertsCountBySide(int isBottom){ return ((isBottom)?m_botInserts:m_topInserts);}
};

////////////////////////////////////////////////////////////
// BaseAoiExporterPCBData
////////////////////////////////////////////////////////////
class BaseAoiExporterPCBData
{
private:
   BaseAoiExporterInsertDataList   m_FidDataList;
   BaseAoiExporterInsertDataList   m_CompDataList;
   BaseAoiExporterInsertDataList   m_XoutDataList;

   double   m_originX;
   double   m_originY;
   BlockTypeTag m_boardType;
   BlockStruct*   m_geometryBlock;
   DataStruct*    m_insertData;

public:
   BaseAoiExporterPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry);
   ~BaseAoiExporterPCBData();
   BaseAoiExporterInsertDataList &getFiducialDataList(){return m_FidDataList;}
   BaseAoiExporterInsertDataList &getComponentDataList(){return m_CompDataList;}
   BaseAoiExporterInsertDataList &getXoutDataList(){return m_XoutDataList;}

   double  getOriginX() { return m_originX; }
   double  getOriginY() { return m_originY; }
   double  getRotDeg()  { return (m_insertData != NULL) ? (m_insertData->getInsert()->getAngleDegrees()) : (0.); }

   bool  getMirrored()  { return (m_insertData != NULL) ? (m_insertData->getInsert()->getGraphicMirrored()) : false; }

   CString getRefname() { return (m_insertData != NULL) ? (m_insertData->getInsert()->getRefname()) : (""); }
   
   BlockTypeTag getBoardType(){return m_boardType;}
   BlockStruct* getGeometryBlock(){return m_geometryBlock;}

   int getFidCount(bool isBottom)      { return m_FidDataList.getInsertsCountBySide(isBottom);  }
   int getCompCount(bool isBottom)     { return m_CompDataList.getInsertsCountBySide(isBottom); }

};

////////////////////////////////////////////////////////////
// BaseAoiExporterPCBDataList
////////////////////////////////////////////////////////////
class BaseAoiExporterPCBDataList : public CTypedPtrMap<CMapStringToPtr,CString,BaseAoiExporterPCBData*>
{
private:
   bool  m_Panelflag;
   CString m_BoardName;

public:
   BaseAoiExporterPCBDataList();
   ~BaseAoiExporterPCBDataList();
   
   void  empty();
   bool  IsPanel(){return m_Panelflag;}
   void  setPanelFlag(bool flag){m_Panelflag = flag;}
   void  setBoardName(CString boardname){m_BoardName = (!boardname.IsEmpty())?boardname:QDEFAULT_BOARDNAME;} //*rcf make default board name configurable
   void  SetBoardAt(CString boardName, BaseAoiExporterPCBData *pcbdata);

   CString getBoardName(){ return m_BoardName;}
};

////////////////////////////////////////////////////////////
// BaseAoiExporter
////////////////////////////////////////////////////////////
class BaseAoiExporter
{
   //*rcf Someday... move what we can out of protected, into private
private:
   int m_minFiducialCount;
   int m_maxFiducialCount;
   bool m_flatten; // True = apply flattening to comp coords in each pcb in panel, False = leave coords as-is.
   bool m_convertUnits;

   FileStruct           *m_activeFile;  // The input CCZ <File>. Could be board or panel.

protected:
   COperationProgress   m_exportProgress;
   FileTypeTag          m_exporterFileTypeTag;  // Indicates kind of specific exporter subclass.
   //deadcode BaseAoiExporterOutputAttrib   m_attribMap;
   BaseAoiExporterSettings       *m_outSettings;
   CCEtoODBDoc           &m_pDoc;
   BaseAoiExporterPCBDataList   m_PCBDataList;
   double               m_Scale;
   double               m_dUnitFactor;
   int                  m_unit_accuracy;
   CString              m_FileName;
   CString              m_targetPCBName;
   CLogWriteFormat      m_textLogFile;
   bool                 m_useComponentCentroids; // If not, then use component insert position.
   bool                 m_mirrorBottomX;
   bool                 m_mirrorBottomRotation;
   CMapStringToPtr      m_usedRefnameMap; // For GetUniqueName(str).

   // For fiducial count verification. Zero counts turns off verification.
   void SetMinFiducialCount(int cnt)   { m_minFiducialCount = cnt; }
   void SetMaxFiducialCount(int cnt)   { m_maxFiducialCount = cnt; }
   int  GetMinFiducialCount()          { return m_minFiducialCount; }
   int  GetMaxFiducialCount()          { return m_maxFiducialCount; }

   bool Flatten()       { return m_flatten; }
   void Flatten(bool b) { m_flatten = b;    }

   // Some exporters want data from the base class in the active ccz page units.
   // Some want the data prep in base to also do the units conversion.
   // This turns conversion on and off.
   void DoUnitsConversion(bool b)   { m_convertUnits = b; }

   CExtent GetOutlineExtent(FileStruct *activeFile);
   int GetComponentCount(bool isBottom);

   CString GetUniqueName(CString refname);


   bool VerifyFiducials(BlockTypeTag blockType);
   bool isLoaded(DataStruct* data);
   bool isWritePart(DataStruct* data);
   void TranslateFile(CTMatrix &l_panelMatrix, FileStruct *fileStruct);
   void TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data);

   double getRotationDegrees(double radians, int mirror);
   CString getPartNumber(DataStruct* data);
   CString getUnitString(PageUnitsTag Pageunits);
   ApertureShapeTag getFiducialShape(DataStruct* data);
   double ConvertToExporterUnits(double valueCczPageUnits){ return (m_convertUnits)?(valueCczPageUnits * m_dUnitFactor):(valueCczPageUnits); }

   CString CreateOutfileName(CString basename, CString filenameSuffix, CString filenameExt);

   void WriteLogHeader();

public:
   BaseAoiExporter(CCEtoODBDoc &doc, double l_Scale, FileTypeTag exporterFileType, BaseAoiExporterSettings *settingsHandler, CString logFilename);
   ~BaseAoiExporter();

   bool  LoadSettingsFile();

   

   bool GetUseComponentCentroids()           { return m_useComponentCentroids; }
   void SetUseComponentCentroids(bool b)     { m_useComponentCentroids = b;    }

   //*rcf On second thought, probably don't need this, parts are never mirrored in output loop
   bool GetMirrorBottomX()           { return m_mirrorBottomX; }
   void SetMirrorBottomX(bool b)     { m_mirrorBottomX = b;    }

   bool GetMirrorBottomRotation()           { return m_mirrorBottomRotation; }
   void SetMirrorBottomRotation(bool b)     { m_mirrorBottomRotation = b;    }

   FileStruct *GetActiveFile()   { return m_activeFile; }

   // Line config aka Prep settings in CCZ
   bool HasLineConfigSettings(FileStruct *file, bool isBottom);
   bool EnforceLineConfigSettings(CCEtoODBDoc &doc);
   FileStruct *DetermineActiveCCFile(CCEtoODBDoc *doc);

   // Lookup some user-selectable attrib for whatever purpose.
   CString GetConfigurableValue(BaseAoiExporterInsertData *cmpData, CString dataSourceName);

   void  WriteDataLine(CFormatStdioFile* topFile, CFormatStdioFile* botFile, CStringArray &DataList);
   void  WriteDataLine(CFormatStdioFile* topFile, CFormatStdioFile* botFile, CString DataLine);
   void  WriteDataLine(CFormatStdioFile *baseFile, CStringArray &DataList);
   void  WriteDataLine(CFormatStdioFile *baseFile, CString DataLine);

   bool  WriteFiles(CString filename); 
   bool  WritePCBFiles();


   virtual CString RenameFiducial(CString fidname, int intFidID, bool isPanelFid);
   virtual bool WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName) { return false; }
   virtual bool WriteLayoutFiles() { return false; }
   virtual bool WritePanelFiles(BaseAoiExporterPCBData *panelData) { return false; }
   virtual bool WritePanelFile(BaseAoiExporterPCBData *panelData, bool isBottom) { return false; }
   virtual int GetDecimals(PageUnitsTag pageUnits);
   virtual CString GetDefaultLocation(bool isBottom);


   bool  ProcessPCBDesign();
   bool  TraversePCBDesign(BlockStruct* block, BaseAoiExporterPCBData *pcbData, CTMatrix transformMatrix,bool isBottom);
   bool  TraversePCBsInPanel(BlockStruct *targetBlock);

   bool  ProcessPanel();
   bool  ProcessPanelOriginalStyle();
   bool  ProcessPanelCyberStyle();
   void  TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block, bool isBottom);
   void  TraversePanelMydataStyle(CTMatrix &l_panelMatrix, BlockStruct* block, bool isBottom);
   void  TraversePanelSakiStyle(CTMatrix &l_panelMatrix, BlockStruct* block, bool isBottom);
   void  ApplyPrepSettingToFiles(FileStruct* fileStruct, BaseAoiExporterPCBData *pcbData, bool isBottom, bool isPanel);

   //deadcode bool  WritePanelFiducialSetList(CTMatrix &l_panelMatrix, BlockStruct* block);
   //deadcode bool  WritePanelComponents(CTMatrix &l_panelMatrix,BlockStruct* block);
   
   BaseAoiExporterSettings *GetSettings() { return m_outSettings; }
   void SetSettings(BaseAoiExporterSettings *settingsHandler)   { m_outSettings = settingsHandler; }

   void ShowProgressDialog(bool show) { if (m_exportProgress.getProgressDlg() != NULL) m_exportProgress.getProgressDlg()->ShowWindow(show?SW_SHOW:SW_HIDE); }
   COperationProgress &GetProgressBar()   { return m_exportProgress; }
};


#endif