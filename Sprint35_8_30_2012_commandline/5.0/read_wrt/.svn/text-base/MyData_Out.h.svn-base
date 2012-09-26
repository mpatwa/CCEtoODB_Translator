// $Header: /CAMCAD/5.0/read_wrt/MyData_Out.h 2     01/06/09 9:51a Sharry Huang $
#if !defined(_MYDATA_OUT_H)
#define _MYDATA_OUT_H


#pragma once
#include "ccdoc.h"
#include "General.h"
#include "CamCadDatabase.h"
#include "DcaCamcadFileWriter.h"
#include "afxwin.h"
#include "DcaWriteFormat.h"
#include "BaseAoiExporter.h"

#define QFILEEXT_PCB       ".pcb"
#define QFILEEXT_PANEL     ".pnl"
#define QFILEEXT_LAYOUT    ".lay"
#define QMYDATA_BOARDNAME "$Board"

#define DEGREESCALE  1000 //millidegree

//Mydata Commands
enum MydataCommadTag
{
   MydataCmd_Unknown = -1,
   MydataCmd_Min = 0,
   //General
   LayoutCmd = MydataCmd_Min,
   PanelCmd,
   PCBCmd,
   //Layout
   LayoutCmd_Info,
   LayoutCmd_PanelName,
   LayoutCmd_BoardName,
   LayoutCmd_Fid,
   LayoutCmd_MachineName,
   //Panel
   PanelCmd_Info,
   PanelCmd_Tool,
   PanelCmd_PanelFid,
   PanelCmd_GlueTestPos,
   PanelCmd_MountTestPos,
   PanelCmd_BadTestPos,
   PanelCmd_BoardName,
   PanelCmd_BoardFid,
   PanelCmd_AuxiliaryData,
   //PCB
   PCBCmd_Info,
   PCBCmd_Tool,
   PCBCmd_Fid,
   PCBCmd_GlueTestPos,
   PCBCmd_MountTestPos,
   PCBCmd_BadTestPos,
   PCBCmd_MountPosition,
   PCBCmd_MountPosName,
   PCBCmd_AuxiliaryData,
   MydataCmd_Last,
};
enum MydataPackageType
{
   MydataPkgMisc,
   MydataPkgSinglePinComponent, 
   MydataPkgBipolarComponent,
   MydataPkgTripolarComponent,
   MydataPkgMultipolarComponent,
   MydataPkgDualInlineComponent,
};

////////////////////////////////////////////////////////////
// CMydataComponentAnalyzer
////////////////////////////////////////////////////////////
class MydataComponentInfo
{
public:
   MydataPackageType packageType;
   double   Orientation;
   double   CentroidRotation;
   int   pinCount;
   bool  isSingleRow;

   MydataComponentInfo() { packageType = MydataPkgMisc; Orientation = 0.; CentroidRotation = 0.; pinCount = 0; isSingleRow = true; }
};

class MydataComponentAnalyzer
{
private:
   // No longer caching. Same geom can be used by more than one partnumber, may have different pin mapping.
   // Pin mapping effects orientation for some kinds of parts.
   //CTypedMapIntToPtrContainer<MydataComponentInfo*> m_ComponentStructMap;
   CCEtoODBDoc&    m_camCadDoc;
   CString m_message; // Potentially contains message(s) after analysis.

   double   getOrientationOffset(MydataPackageType packageType, bool isSingleRow);
   int      getComponentPinCount(BlockStruct* geometryBlock);
   MydataPackageType getPackageType(int pinCount);

   CompPinStruct *findCompPin(CString compRefname, CString pinName);

   bool AnalyzeCentroidRotationOffset(DataStruct *inputInsertData, BlockStruct *inputReferenceBlock, double &outputRotation, bool& isSingleRow, int& pinCount, CString &outMessage);

public:
   MydataComponentAnalyzer(CCEtoODBDoc& camCadDoc);
   ~MydataComponentAnalyzer();

   double   getMyDataCentroidRotation(DataStruct* InsertData, double rotation);

   CString getMessage()    { return m_message; }
};


class MydataSettings : public BaseAoiExporterSettings
{
   // No local settings for MyData

public:
   MydataSettings(CCEtoODBDoc& doc);
   ~MydataSettings();

   virtual void SetLocalDefaults();
   virtual bool ParseLocalCommand(CInFile &inFile);
};




class NewMyDataWriter : public BaseAoiExporter
{
private:
   MydataComponentAnalyzer m_compAnalyzer;

   MydataSettings *GetSettings()  { return (MydataSettings*)(BaseAoiExporter::GetSettings()); }

   void  WriteHeader(CString Command, CFormatStdioFile *outFile, CString boardName);
   void  WriteHeader(CString Command, CFormatStdioFile* topFile, CFormatStdioFile* botFile, CString boardName);
   void  WriteTools(CString Command, CFormatStdioFile* topFile, CFormatStdioFile* botFile);
   void  WritePCBTestPosition(CFormatStdioFile *outFile);
   void  WriteAuxiliaryPCBData(CFormatStdioFile *outFile);

   void  WritePanelTestPosition(CFormatStdioFile *m_inputFile);
   void  WriteBoardandFiducialMarks(MydataCommadTag fidCmdIdx, MydataCommadTag boardCmdIdx, BlockTypeTag boardType, CFormatStdioFile &m_topFile,CFormatStdioFile &m_botFile);
   void  WriteAuxiliaryPanelData(CFormatStdioFile *m_inputFile);

   void  WriteBadBoardMarks(CFormatStdioFile& topFile, CFormatStdioFile& botFile, CString command, BaseAoiExporterPCBData *pcbData);
   void  WriteFiducialSetList(CString Command, BaseAoiExporterPCBData *pcbData, CFormatStdioFile &m_topFile,CFormatStdioFile &m_botFile, BlockTypeTag boardType, bool isPair);
   
   void  WriteComponentList(BaseAoiExporterPCBData *pcbData, CFormatStdioFile &m_topFile,CFormatStdioFile &m_botFile);

   bool WriteMydataPNLFiles();

public:
   NewMyDataWriter(CCEtoODBDoc &doc, double l_Scale);
   ~NewMyDataWriter();

   virtual int GetDecimals(PageUnitsTag pageUnits);

   virtual bool WriteLayoutFiles();
   virtual bool WritePanelFiles(BaseAoiExporterPCBData *panelData);
   virtual bool WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName);
};


#endif /*_MYDATA_OUT_H*/