
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#ifndef _JUKI_OUT_H
#define _JUKI_OUT_H

#pragma once
#include "ccdoc.h"
#include "General.h"
#include "CamCadDatabase.h"
#include "afxwin.h"

#define DEFAULT_JUKI_UNITS	pageUnitsMilliMeters
#define JUKI_SMALLVALUE 1.0E-4
#define MAXFIDSIZE 3

////////////////////////////////////////////////////////////
// JukiOutputAttrib
////////////////////////////////////////////////////////////
class JukiOutputAttrib
{
public:
	JukiOutputAttrib(CCEtoODBDoc& camCadDoc);
   JukiOutputAttrib();
	~JukiOutputAttrib();

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
// CJukiInsertData
////////////////////////////////////////////////////////////
class CJukiInsertData 
{
private:
   double   m_rotation;
   double   m_posX;
   double   m_posY;
   CString  m_PartNumber;
   CString  m_refName;
   DataStruct *m_InsertData;
   BlockTypeTag m_placedBoardType;
   CTypedPtrArrayContainer<CJukiInsertData*> *m_CompFidDataList;

public:
   CJukiInsertData();
   ~CJukiInsertData();

   DataStruct* getInsertData(){return m_InsertData;}
   void setInsertData(DataStruct *data){m_InsertData = data;}

   void setRotation(double rotation){m_rotation = rotation;}
   double getRotation(){return m_rotation;}

   void setX(double pos){m_posX = pos;}
   double getX(){return m_posX;}

   void setY(double pos){m_posY = pos;}
   double getY(){return m_posY;}

   void setPartNumber(CString part){m_PartNumber = part;}
   CString &getPartNumber(){return m_PartNumber;}

   void setRefName(CString refname){m_refName = refname;}
   CString &getRefName(){return m_refName;}

   void setPlacedBoardType(BlockTypeTag type){m_placedBoardType = type;}
   BlockTypeTag getPlacedBoardType(){return m_placedBoardType;}
   
   CTypedPtrArrayContainer<CJukiInsertData*>* getCompFidDataList();
};

////////////////////////////////////////////////////////////
// CJukiInsertDataList
////////////////////////////////////////////////////////////
class CJukiInsertDataList : public CTypedPtrArrayContainer<CJukiInsertData*>
{
private:
   int   m_topFids;
   int   m_botFids;

public:
   CJukiInsertDataList();
   ~CJukiInsertDataList();
   void Add(DataStruct* data, double posX, double posY, double rotation, CString refName, CString partNumber, BlockTypeTag boardtype);
   int getInsertCount(int isBottom){return ((isBottom)?m_botFids:m_topFids);}
   CJukiInsertData *getTail();
};

////////////////////////////////////////////////////////////
// CJukiPCBData
////////////////////////////////////////////////////////////
class CJukiPCBData
{
private:
   CJukiInsertDataList    m_FidDataList;
   CJukiInsertDataList   m_CompDataList;
   CJukiInsertDataList   m_XOutDataList;
   double   m_originX;
   double   m_originY;
   double   m_rotation;
   int      m_mirror;

   BlockTypeTag   m_boardType;
   BlockStruct*   m_geometryBlock;
   DataStruct*    m_insertData;

public:
   CJukiPCBData(double x, double y, double rot, int mirror, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry);
   ~CJukiPCBData();
   CJukiInsertDataList &getFiducialDataList(){return m_FidDataList;}
   CJukiInsertDataList &getComponentDataList(){return m_CompDataList;}
   CJukiInsertDataList &getXOutDataList(){return m_XOutDataList;}

   double   getOriginX(){return m_originX;}
   double   getOriginY(){return m_originY;}
   double   getRotation(){return m_rotation;}
   int      getMirror(){return m_mirror;}

   BlockTypeTag getPlacedBoardType(){return m_boardType;}
   DataStruct* getInsertData(){return m_insertData;}
   BlockStruct* getGeometryBlock(){return m_geometryBlock;}

};

////////////////////////////////////////////////////////////
// CJukiPCBDataList
////////////////////////////////////////////////////////////
class CJukiPCBDataList : public CTypedPtrMap<CMapStringToPtr,CString,CJukiPCBData*>
{
private:
   bool  m_Panelflag;
   CString m_BoardName;
   double m_pcbThickness;

public:
   CJukiPCBDataList();
   ~CJukiPCBDataList();
   void empty();

   bool  IsPanel(){return m_Panelflag;}
   void  setPanelFlag(bool flag){m_Panelflag = flag;}
   
   CString getBoardName(){ return m_BoardName;}
   void setBoardName(CString boardname){m_BoardName = boardname;}

   double getBoardThickness(){return m_pcbThickness;}
   void setPCBThickness(double thickness){m_pcbThickness = thickness;}
};

////////////////////////////////////////////////////////////
// CJukiSetting
////////////////////////////////////////////////////////////
class CJukiSetting
{
private:
   CCEtoODBDoc           *m_pDoc;

   JukiOutputAttrib     m_attribMap;
   PageUnitsTag         m_Units;
	double               m_dUnitFactor;
   double               m_pcbthickness;
   CString              m_customAttribute;
   CString              m_PackageName;
   CString              m_badmarkType;
	bool                 m_exportUNLoaded;
   bool                 m_flattenOutput;

public:
   CJukiSetting(CCEtoODBDoc* doc);
   void LoadDefaultSettings();
   bool LoadStandardSettingsFile(CString FileName);

   double  getUnitFactor(){ return m_dUnitFactor;}
   double getBoardThickness(){return m_pcbthickness;}

   bool  getExportUNLoaded(){return m_exportUNLoaded;}
   bool  getFlattenOutput(){return m_flattenOutput;}

   CString  &getCustomAttribute(){return m_customAttribute;}
   JukiOutputAttrib  &getAttributeMap(){return m_attribMap;}

   CString getBadMarkType(){return m_badmarkType;}
};

////////////////////////////////////////////////////////////
// JukiWrite
////////////////////////////////////////////////////////////
class JukiWrite
{
private:
	CFormatStdioFile topFile;
	CFormatStdioFile botFile;
	CCEtoODBDoc *m_pDoc;
   CCamCadData& m_camCadData;

	double   p_Scale;
	double   m_dUnitFactor;
   CString  m_customAttribute;

   CJukiSetting      m_Settings;
   CJukiPCBDataList  m_PCBDataList;
   CPoint2d          m_panelOffset;
   CString           m_targetPCBName;
   CJukiPCBData      *m_targetPCBdata;

   int   m_rot_accuracy;
   int   m_loc_accuracy;

   double m_lowerRightCornerX;  // In Juki output units  (not ccz page units)

public:
	JukiWrite(CCEtoODBDoc *document, double l_Scale);
   JukiWrite();   

	BOOL Write(CString newTopFile, CString newBotFile);
	int LoadSettings(CString settingsFile);

   bool ProcessPCB();
   bool TraversePCB(BlockStruct* block, CTMatrix& l_boardMatrix);
	bool ProcessPanel();
   bool TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block);
   bool ProcessStencilPCB(CJukiPCBData* &targetPCBdata, CTMatrix& l_boardMatrix);
   bool LookupStencilBlock(CJukiPCBData* &targetPCBdata, CTMatrix& l_boardMatrix);  
   bool TraverseComponent(BlockStruct* block, CTMatrix& l_boardMatrix, CJukiInsertData *insertData);

   void WriteDataLine(CStringArray &DataList);
   void WriteDataLine(CString DataLine);
   void WriteDataLine(CFormatStdioFile *baseFile, CStringArray &DataList);
   void WriteDataLine(CFormatStdioFile *baseFile, CString DataLine);

   void WritePanelOffset();
   void WriteFiducialOffset();
   void WriteBadMark();
   void WriteMatrixMethod();
   void WriteBoardOffset();
   void WriteComponentHeader();

   void WriteFiducialList(BlockTypeTag boardType, bool isExtraAttribute);
   void WriteFiducial(CJukiPCBData *pcbData, BlockTypeTag boardType, bool isExtraAttribute);
   void WriteComponentList(CString targetPcbName);
   void WriteComponent(CJukiPCBData *pcbData);
   void WriteXOutList(BlockTypeTag boardType, bool isShift, bool isExtraAttribute);
   void WriteXOut(CJukiPCBData *pcbData, BlockTypeTag boardType, bool isShift, bool isExtraAttribute);
   void WriteComponentFiducials(CJukiInsertData *cmpdata, bool isShift);

   void TranslateFile(CTMatrix &l_panelMatrix, FileStruct *fileStruct);
   void TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data);

   bool IsLoaded(DataStruct *data);
   bool IsWritePart(DataStruct *data);
   bool GetOutlineExtent(CJukiPCBData *pcbData, ExtentRect *outlineExtent,int GRClass);
   bool getCustomAttribute(DataStruct *data, CString& attribValue);
   bool VerifyFiducials(CString panelName, CString targetPCBName, int MinFidSize, int MaxFidSize);

   double getCczCentroidRotation(DataStruct *data);
   double getJukiCentroidRotation(DataStruct *data);
   CString getPartNumber(DataStruct *data);
   CString getFid(int intFidID);
   double getJukiUnits(double x){ return x * m_dUnitFactor;}
   double getBoardThickness(BlockStruct *block);

   double applyJukiOriginX(double jukiX, bool isBottom); // Incoming jukiX coord should already be in Juki output units.
};

#endif /*_JUKI_OUT_H*/
