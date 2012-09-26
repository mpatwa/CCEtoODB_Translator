
#if !defined(_FUJIFLEXA_OUT_H)
#define _FUJIFLEXA_OUT_H


#pragma once
#include "DcaWriteFormat.h"

#define QFUJI_BOARDNAME "$Board"

////////////////////////////////////////////////////////////
// FujiFlexaOutputAttrib
////////////////////////////////////////////////////////////
class FujiFlexaOutputAttrib
{
public:
	FujiFlexaOutputAttrib(CCEtoODBDoc& camCadDoc);
	~FujiFlexaOutputAttrib();

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
// CFujiFlexaSetting
////////////////////////////////////////////////////////////
class CFujiFlexaSetting
{
private:
   CString                 m_PartNumberKW;
   bool                    m_exportUNLoaded;
   PageUnitsTag            m_pageUnits;
   CString                 m_fiducialPartnumber; // aka Fiducial Name
   CString                 m_attribKeyword;
   CStringArray            m_attribValue;

public:
   CFujiFlexaSetting();
   ~CFujiFlexaSetting();

   void LoadDefaultSettings();
   bool LoadStandardSettingsFile(CString FileName);

   CString &getPartNumberKW(){return m_PartNumberKW;}
   bool getExportUNLoaded(){return m_exportUNLoaded;}
   
   PageUnitsTag getDefaultPageUnits(){ return m_pageUnits;}
   void setDefaultPageUnits(PageUnitsTag pageUnits){ m_pageUnits = pageUnits;}

   CString getFiducialPartnumber()     { return m_fiducialPartnumber; }
   
   CString &getAttribKeyword(){ return m_attribKeyword;}
   CStringArray &getAttribValueArray(){ return m_attribValue;}
};

////////////////////////////////////////////////////////////
// CFujiFlexaInsertData
////////////////////////////////////////////////////////////
class CFujiFlexaInsertData 
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
   BlockTypeTag   m_BoardType;
   CCEtoODBDoc&    m_camCadDoc;
   CCamCadDatabase   m_camCadDatabase;

public:
   CFujiFlexaInsertData(CCEtoODBDoc& camCadDoc, DataStruct *data);

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
};

////////////////////////////////////////////////////////////
// CFujiFlexaInsertDataList
////////////////////////////////////////////////////////////
class CFujiFlexaInsertDataList : public CTypedPtrArrayContainer<CFujiFlexaInsertData*>
{
private:
   int m_topInserts;
   int m_botInserts;
   CCEtoODBDoc& m_camCadDoc;

public:
   CFujiFlexaInsertDataList(CCEtoODBDoc& camCadDoc);
   ~CFujiFlexaInsertDataList();
   void  Add(DataStruct* InsertData, double posX, double posY, double rotation, CString refName, CString partNumber, BlockTypeTag boardType, bool placedBottom);
   int   getInsertsCountBySide(int isBottom){ return ((isBottom)?m_botInserts:m_topInserts);}
};

////////////////////////////////////////////////////////////
// CFujiFlexaPCBData
////////////////////////////////////////////////////////////
class CFujiFlexaPCBData
{
private:
   CFujiFlexaInsertDataList    m_FidDataList;
   CFujiFlexaInsertDataList   m_CompDataList;

   double   m_originX;
   double   m_originY;
   BlockTypeTag m_boardType;
   BlockStruct*   m_geometryBlock;
   DataStruct*    m_insertData;

public:
   CFujiFlexaPCBData(CCEtoODBDoc& camCadDoc, double x, double y, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry);
   ~CFujiFlexaPCBData();
   CFujiFlexaInsertDataList &getFiducialDataList(){return m_FidDataList;}
   CFujiFlexaInsertDataList &getComponentDataList(){return m_CompDataList;}
   double  getOriginX(){return m_originX;}
   double  getOriginY(){return m_originY;}
   
   BlockTypeTag getBoardType(){return m_boardType;}
   BlockStruct* getGeometryBlock(){return m_geometryBlock;}

};

////////////////////////////////////////////////////////////
// CFujiFlexaPCBDataList
////////////////////////////////////////////////////////////
class CFujiFlexaPCBDataList : public CTypedPtrMap<CMapStringToPtr,CString,CFujiFlexaPCBData*>
{
private:
   bool  m_Panelflag;
   CString m_BoardName;

public:
   CFujiFlexaPCBDataList();
   ~CFujiFlexaPCBDataList();
   
   void  empty();
   bool  IsPanel(){return m_Panelflag;}
   void  setPanelFlag(bool flag){m_Panelflag = flag;}
   void  setBoardName(CString boardname){m_BoardName = (!boardname.IsEmpty())?boardname:QFUJI_BOARDNAME;}
   void  SetBoardAt(CString boardName, CFujiFlexaPCBData *pcbdata);

   CString getBoardName(){ return m_BoardName;}
};

////////////////////////////////////////////////////////////
// FujiFlexaWriter
////////////////////////////////////////////////////////////
class FujiFlexaWriter
{
private:
   COperationProgress      m_writeProcess;
   CFujiFlexaSetting       m_outSetting;
   CCEtoODBDoc              &m_pDoc;
   CFujiFlexaPCBDataList   m_PCBDataList;
   FujiFlexaOutputAttrib   m_attribMap;
   CLogWriteFormat         m_textLogFile;

   double               m_Scale;
   double               m_dUnitFactor;
   int                  m_unit_accuracy;
   CString              m_FileName;
   CString              m_targetPCBName;

private:
   CFujiFlexaSetting& GetSettings(){return m_outSetting;}
   double   getFujiFlexaUnits(double value){ return (value * m_dUnitFactor);}

   bool  IsLoaded(DataStruct* data);
   bool  IsWritePart(DataStruct* data);
   bool  isUniqueBoardNameOnPanel(BlockStruct* panelblock);

   double   getRotationDegree(double radious, int mirror);
   CString  getPartNumber(DataStruct* data);
   void  TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data);
   CString  ReNameFiducial(CString originalName, int intFidID);

public:
   FujiFlexaWriter(CCEtoODBDoc &doc, double l_Scale);
   ~FujiFlexaWriter();
   
   bool  LoadSettingsFile();
   bool  WriteFiles(CString filename);
   bool  WritePCBFiles(CFujiFlexaPCBData *pcbData, CString boardName);
   bool  WritePanelFiles(CFujiFlexaPCBData *pcbData);

   void  WriteHeader(CFormatStdioFile &m_File);
   void  WriteComponentList(CFormatStdioFile &m_File, CFujiFlexaPCBData *pcbData, bool isBottom);
   void  WriteComponent(CFormatStdioFile &m_File, int boardIdx, CFujiFlexaPCBData *pcbData, bool isBottom);
   void  WriteFiducialSetList(CFormatStdioFile &m_File, CFujiFlexaPCBData *pcbData, BlockTypeTag boardType, bool isBottom);

   bool  ProcessPCBDesign();
   bool  ProcessPanel();

   void  ApplyPrepSettingToFiles(FileStruct* fileStruct, CFujiFlexaPCBData *pcbData, bool isBottom, bool isPanel);
   bool  TraversePCBDesign(BlockStruct* block, CFujiFlexaPCBData *pcbData, CTMatrix transformMatrix);
   bool  TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block);
};
#endif /*_FUJIFLEXA_OUT_H*/