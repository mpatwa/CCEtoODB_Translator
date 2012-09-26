
#if !defined(_YESTECHAOI_OUT_H)
#define _YESTECHAOI_OUT_H


#pragma once

#include "BaseAoiExporter.h"

#define QYESTECH_BOARDNAME "Board"
#define QGEOM_NAME  "GEOM_NAME"


////////////////////////////////////////////////////////////
// CYestechAOIInsertData
////////////////////////////////////////////////////////////
class CYestechAOIInsertData 
{
private:
   double   m_rotation;
   double   m_posX;
   double   m_posY;
   CString  m_PartNumber;
   CString  m_Description;
   CString  m_refName;
   int      m_pinCount;
   bool     m_placedBottom;

   DataStruct *m_InsertData;
   BlockStruct *m_insertedBlock;

   BlockTypeTag   m_BoardType;
   CCEtoODBDoc&    m_camCadDoc;
   CCamCadDatabase   m_camCadDatabase;

public:
   CYestechAOIInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data);

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

   void setDescription(CString desc){m_Description = desc;}
   CString &getDescription(){return m_Description;}

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
// CYestechAOIInsertDataList
////////////////////////////////////////////////////////////
class CYestechAOIInsertDataList : public CTypedPtrArrayContainer<CYestechAOIInsertData*>
{
private:
   int m_topInserts;
   int m_botInserts;
   CCEtoODBDoc& m_camCadDoc;

public:
   CYestechAOIInsertDataList(CCEtoODBDoc& camCadDoc);
   ~CYestechAOIInsertDataList();
   void  Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, CString partNumber, CString description, BlockTypeTag boardType, bool placedBottom);
   int   getInsertsCountBySide(bool isBottom){ return ((isBottom)?m_botInserts:m_topInserts);}
};

////////////////////////////////////////////////////////////
// CYestechAOIPCBData
////////////////////////////////////////////////////////////
class CYestechAOIPCBData
{
private:
   CYestechAOIInsertDataList    m_FidDataList;
   CYestechAOIInsertDataList   m_CompDataList;

   double   m_originX;
   double   m_originY;
   BlockTypeTag m_boardType;
   BlockStruct*   m_geometryBlock;
   DataStruct*    m_insertData;

public:
   CYestechAOIPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry);
   ~CYestechAOIPCBData();
   CYestechAOIInsertDataList &getFiducialDataList(){return m_FidDataList;}
   CYestechAOIInsertDataList &getComponentDataList(){return m_CompDataList;}
   double  getOriginX(){return m_originX;}
   double  getOriginY(){return m_originY;}

   BlockTypeTag getBoardType(){return m_boardType;}
   BlockStruct* getGeometryBlock(){return m_geometryBlock;}

   int getFidCount(bool isBottom)      { return m_FidDataList.getInsertsCountBySide(isBottom);  }
   int getCompCount(bool isBottom)     { return m_CompDataList.getInsertsCountBySide(isBottom); }

};

////////////////////////////////////////////////////////////
// CYestechAOIPCBDataList
////////////////////////////////////////////////////////////
class CYestechAOIPCBDataList : public CTypedPtrMap<CMapStringToPtr,CString,CYestechAOIPCBData*>
{
private:
   bool  m_Panelflag;
   CString m_BoardName;

public:
   CYestechAOIPCBDataList();
   ~CYestechAOIPCBDataList();

   void  empty();
   bool  IsPanel(){return m_Panelflag;}
   void  setPanelFlag(bool flag){m_Panelflag = flag;}
   void  setBoardName(CString boardname){m_BoardName = (!boardname.IsEmpty())?boardname:QYESTECH_BOARDNAME;}
   void  SetBoardAt(CString boardName, CYestechAOIPCBData *pcbdata);

   CString getBoardName(){ return m_BoardName;}
};


////////////////////////////////////////////////////////////
// CSakiAOISettings
////////////////////////////////////////////////////////////
class YestechAOISettings : public BaseAoiExporterSettings
{
private:
   CString                 m_pnLibName;
   CString                 m_centralLibName;
   CString                 m_pkgLibName;
   CString                 m_pkgNameMapName;
   CString                 m_pkgNameAttrib;  // Attrib to use for PKG in output, if blank or GEOMETRY use geometry name.
   bool                    m_columnDelimiterTab;  // true = use tab, false (default) use space

public:
   YestechAOISettings(CCEtoODBDoc& doc);
   ~YestechAOISettings();

   virtual void SetLocalDefaults();
   virtual bool ParseLocalCommand(CInFile &inFile);

   CString getPnLibName()        { return m_pnLibName; }
   CString getCentralLibName()   { return m_centralLibName; }
   CString getPkgLibName()       { return m_pkgLibName; }
   CString getPkgNameMapName()   { return m_pkgNameMapName; }
   CString getPkgNameAttrib()    { return m_pkgNameAttrib;  }
   bool getUseTabDelimiter()     { return m_columnDelimiterTab; }
   CString getFieldDelimiter()   { return m_columnDelimiterTab ? "\t" : " "; }
};



class NewYestechAOIWriter : public BaseAoiExporter
{
private:
   FileStruct           *m_activeFile;
   int m_compRecordNum;  // A counter during output
   int                  m_deviceTypeKW;  // device type attribute keyword index

   FileStruct *getActiveFile()   { return m_activeFile; }

   double getYestechAOIUnits(double value){ return ConvertToExporterUnits(value); }  //*rcf When getMydataUnits name if fixed up, get rid of getSakiUnits

   bool  WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom);
   void  WriteHeader(CFormatStdioFile &File, bool isBottom);
   void  WriteComponentList(CFormatStdioFile &File, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void  WriteComponents(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void  WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom);
   void  WriteComponentRecord(CFormatStdioFile &File, int boardIdx, BaseAoiExporterInsertData *sakiCmpData, bool isBottom);

   void  WriteComponentRecord(CFormatStdioFile &File, CString f1, CString f2, CString f3, CString f4, CString f5, CString f6, CString f7);

   bool WritePanelFile(BaseAoiExporterPCBData *pcbData, bool isBottom);

   YestechAOISettings *GetSettings()  { return (YestechAOISettings*)(BaseAoiExporter::GetSettings()); }

   CString getYestechOutputUnitsValue(PageUnitsTag outputUnits);

   CString  getDescription(DataStruct* data);
   CString  getDeviceType(DataStruct* data);
   CString  getValue(DataStruct* data);

   CString  getGenericAttributeValue(DataStruct* data, CString attributeName);

public:
   NewYestechAOIWriter(CCEtoODBDoc &doc, double l_Scale);
   ~NewYestechAOIWriter();

   virtual int GetDecimals(PageUnitsTag pageUnits);

   virtual bool WriteLayoutFiles() { return true; /* no-op in this format */ }
   virtual bool WritePanelFiles(BaseAoiExporterPCBData *panelData);
   virtual bool WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName);
   virtual CString RenameFiducial(CString fidname, int intFidID, bool isPanelFid);

   FileStruct *DetermineActiveCCFile(CCEtoODBDoc *doc);

};






#endif /*_YestechAOI_OUT_H*/
