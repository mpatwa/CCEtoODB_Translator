// $Header: /CAMCAD/5.0/read_wrt/ViTechAoiWriter.h 12    2/26/07 7:19p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__ViTechAoiWriter_h__)
#define __ViTechAoiWriter_h__

#pragma once

#include "CamCadDatabase.h"

//_____________________________________________________________________________
class CViTechAoiWriterPcb
{
private:
   CCamCadDatabase& m_camCadDatabase;
   BlockStruct& m_pcbGeometry;
   CExtent m_outlineExtent;
   CPolygon m_pcbOutline;
   //CBasesVector m_pcbOrigin;

public:
   CViTechAoiWriterPcb(CCamCadDatabase& camCadDatabase,BlockStruct& pcbGeometry);

   CExtent getPcbOutlineExtent();
   CPolygon getPcbOutline();
   //CBasesVector getPcbOrigin() const { return m_pcbOrigin; }

private:
   void initialize();
};

//_____________________________________________________________________________
class CViTechAoiWriterPcbMap
{
private:
   CTypedMapIntToPtrContainer<CViTechAoiWriterPcb*> m_pcbMap;

public:
   CViTechAoiWriterPcbMap();

   CViTechAoiWriterPcb& getPcb(CCamCadDatabase& camCadDatabase,DataStruct& pcbData);
};

//_____________________________________________________________________________
class CViTechAoiWriterPcbInstance
{
private:
   CViTechAoiWriterPcb& m_pcb;
   DataStruct& m_pcbData;
   DataStruct* m_xOutData;

public:
   CViTechAoiWriterPcbInstance(CViTechAoiWriterPcb& pcb,DataStruct& pcbData);

   CViTechAoiWriterPcb& getPcb() { return m_pcb; }
   DataStruct& getPcbData() { return m_pcbData; }

   DataStruct* getXOutData() const { return m_xOutData; }
   void setXOutData(DataStruct* xOutData) { m_xOutData = xOutData; }
};

//_____________________________________________________________________________
class CViTechAoiWriterPcbInstanceList
{
private:
   CTypedPtrListContainer<CViTechAoiWriterPcbInstance*> m_pcbInstanceList;

public:
   CViTechAoiWriterPcbInstanceList();

   CViTechAoiWriterPcbInstance* addPcbInstance(CCamCadDatabase& camCadDatabase,
      CViTechAoiWriterPcbMap& pcbMap,DataStruct& pcbData);
   POSITION getHeadPosition() const;
   CViTechAoiWriterPcbInstance* getNext(POSITION& pos) const;
   int getCount() const;
   CViTechAoiWriterPcbInstance* find(const CString& refDes);
};

//_____________________________________________________________________________
class CViTechAoiWriter
{
private:
   CCamCadDatabase m_camCadDatabase;
   CString m_filePath;
   CWriteFormat* m_logFile;
   CMessageFilter* m_messageFilter;
   CStdioFileWriteFormat m_aoiFile;
   bool m_exportUnloadedPartsFlag;
   PageUnitsTag m_aoiUnits;
   int m_aoiUnitPrecision;
   CString m_aoiUnitDecimalPointString;
   CString m_jedecAttributeName;
   int m_jedecKeywordIndex;
   bool m_enableXOutSkips;
   bool m_mirrorBottomRotations;

   FileStruct* m_panelFile;
   CViTechAoiWriterPcbInstanceList m_pcbInstanceList;
   CViTechAoiWriterPcbMap m_pcbMap;
   CDataList m_xOutDataList;
   CExtent m_panelExtent;
   CExtent m_boardInstancesExtent;
   CTMatrix m_panelMatrix;
   bool m_topFlag;
   CPoint2d m_gOffset;

public:
   CViTechAoiWriter(CCEtoODBDoc& m_camCadDoc,const CString& filePath);
   virtual ~CViTechAoiWriter();

   //void set(FormatStruct& format);
   void writeAoiFiles();

private:
   CWriteFormat& getLogFile();
   CProgressDlg& getProgressDialog();
   CMessageFilter& getMessageFilter();
   CCamCadData& getCamCadData();
   CCEtoODBDoc& getCamCadDoc();

   bool parseYesNo(bool& flag,CString value);
   void loadSettings();

   void writeAoiFile(bool topFlag);
   void scanData();
   void writeGeneralInformationSection();
   void writeInformationOnThePanelSection();
   void writeBoardArrangementSection();
   void writeFiducialAndSkipArrangementSection();
   void writeCadCoordinatesSection();
   CString panelUnits(double pcbUnits);
   CPoint2d convertPageUnitsToMachineUnits(const CPoint2d& point);
   bool isPcbComponentLoaded(DataStruct& pcbComponent);
   CString getJedec(DataStruct& pcbComponent);
};


#endif
