// $Header: /CAMCAD/5.0/read_wrt/SakiAoi_Out.h 2     05/19/09 9:51a Sharry Huang $
#if !defined(_SAKIAOI_OUT_H)
#define _SAKIAOI_OUT_H


#pragma once
#include "DcaWriteFormat.h"
#include "BaseAoiExporter.h"


////////////////////////////////////////////////////////////
// CSakiAOISettings
////////////////////////////////////////////////////////////
class CSakiAOISettings : public BaseAoiExporterSettings
{
private:
   double                  m_originSakiUnitsX;
   double                  m_originSakiUnitsY;
   CString                 m_macroValueFrom;   // Name of data source for Macro field
   CString                 m_libValueFrom;     // Name of data source for Lib field
   CString                 m_groupName;

public:
   CSakiAOISettings(CCEtoODBDoc &doc);
   ~CSakiAOISettings();

   virtual void SetLocalDefaults();
   virtual bool ParseLocalCommand(CInFile &inFile);

   CString getMacroKeyword()  { return m_macroValueFrom; }
   CString getLibKeyword()    { return m_libValueFrom; }

   CString getGroupName()     { return m_groupName; }

   double getBoardOriginX()      { return m_originSakiUnitsX; }
   double getBoardOriginY()      { return m_originSakiUnitsY; }
};

//------------------------------------------------

#define QSAKI_BOARDNAME "Board"
#define QGEOM_NAME  "GEOM_NAME"



class SakiAOIWriter : public BaseAoiExporter
{
private:
   int m_compRecordNum;  // A counter during output
   int                  m_deviceTypeKW;  // device type attribute keyword index

   CString getMacroValue(BaseAoiExporterInsertData *sakiCmpData);
   CString getLibValue(BaseAoiExporterInsertData *sakiCmpData);

   int GetFlg(BaseAoiExporterInsertData *sakiCmpData);

   double getSakiAOIUnits(double value){ return ConvertToExporterUnits(value); }  //*rcf When getMydataUnits name if fixed up, get rid of getSakiUnits
   double applySakiOriginX(double rawSakiX);
   double applySakiOriginY(double rawSakiY);

   bool  WritePCBFile(BaseAoiExporterPCBData *pcbData, CString boardName, bool isBottom);
   void  WriteHeader(CFormatStdioFile &File, bool isBottom);
   void  WriteComponentList(CFormatStdioFile &File, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void  WriteComponents(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, bool isBottom);
   void  WriteFiducialSetList(CFormatStdioFile &File, int boardIdx, BaseAoiExporterPCBData *pcbData, BlockTypeTag boardType, bool isBottom);
   void  WriteComponentRecord(CFormatStdioFile &File, int boardIdx, BaseAoiExporterInsertData *sakiCmpData, bool isBottom);

   

   CSakiAOISettings *GetSettings()  { return (CSakiAOISettings*)(BaseAoiExporter::GetSettings()); }

public:
   SakiAOIWriter(CCEtoODBDoc &doc, double l_Scale);
   ~SakiAOIWriter();

   virtual int GetDecimals(PageUnitsTag pageUnits);

   virtual bool WriteLayoutFiles() { return true; /* no-op in this format */ }
   virtual bool WritePanelFile(BaseAoiExporterPCBData *pcbData, bool isBottom);
   virtual bool WritePCBFiles(BaseAoiExporterPCBData *pcbData, CString boardName);
   virtual CString RenameFiducial(CString fidname, int intFidID, bool isPanelFid);

};

#endif /*_SAKIAOI_OUT_H*/
