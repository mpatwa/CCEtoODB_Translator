// $Header: /CAMCAD/5.0/read_wrt/ViTechAoiWriter.cpp 18    6/21/07 8:29p Kurt Van Ness $

/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

#include "StdAfx.h"
#include "ViTechAoiWriter.h"
#include "RwLib.h"
#include "CCEtoODB.h"
#include "Centroid.h"
#include "OutFile.h"
#include "RwUiLib.h"

//_____________________________________________________________________________
CViTechAoiWriterPcb::CViTechAoiWriterPcb(CCamCadDatabase& camCadDatabase,BlockStruct& pcbGeometry)
: m_camCadDatabase(camCadDatabase)
, m_pcbGeometry(pcbGeometry)
, m_pcbOutline(camCadDatabase.getPageUnits())
{
}

CExtent CViTechAoiWriterPcb::getPcbOutlineExtent()
{
   initialize();

   return m_outlineExtent;
}

CPolygon CViTechAoiWriterPcb::getPcbOutline()
{
   initialize();

   return m_pcbOutline;
}

void CViTechAoiWriterPcb::initialize()
{
   if (! m_outlineExtent.isValid())
   {
      double maxArea = 0;
      CPoly* maxPoly = NULL;

      for (CDataListIterator outlineIterator(m_pcbGeometry,dataTypePoly);outlineIterator.hasNext();)
      {
         DataStruct* polyStruct = outlineIterator.getNext();

         if (polyStruct->getGraphicClass() == graphicClassBoardOutline)
         {
            CPolyList* polyList = polyStruct->getPolyList();

            m_outlineExtent.update(polyList->getExtent(m_camCadDatabase.getCamCadData()));

            for (POSITION pos = polyList->GetHeadPosition();pos != NULL;)
            {
               CPoly* poly = polyList->GetNext(pos);

               if (poly->isClosed())
               {
                  double area = poly->getArea();

                  if (area > maxArea)
                  {
                     maxArea = area;
                     maxPoly = poly;
                  }
               }
            }
         }
      }

      if (! m_outlineExtent.isValid())
      {
         m_outlineExtent.update(-1.,-1.);
         m_outlineExtent.update( 1., 1.);
      }

      if (maxPoly != NULL)
      {
         CPolygon pcbOutline(*maxPoly,m_camCadDatabase.getPageUnits());
         m_pcbOutline = pcbOutline;
      }
      else
      {
         m_pcbOutline.addVertex(CPoint2d(m_outlineExtent.getXmin(),m_outlineExtent.getYmin()));
         m_pcbOutline.addVertex(CPoint2d(m_outlineExtent.getXmin(),m_outlineExtent.getYmax()));
         m_pcbOutline.addVertex(CPoint2d(m_outlineExtent.getXmax(),m_outlineExtent.getYmax()));
         m_pcbOutline.addVertex(CPoint2d(m_outlineExtent.getXmax(),m_outlineExtent.getYmin()));
      }
   }
}

//_____________________________________________________________________________
CViTechAoiWriterPcbMap::CViTechAoiWriterPcbMap()
{
   m_pcbMap.InitHashTable(nextPrime2n(200));
}

CViTechAoiWriterPcb& CViTechAoiWriterPcbMap::getPcb(CCamCadDatabase& camCadDatabase,DataStruct& pcbData)
{
   CViTechAoiWriterPcb* pcb = NULL;
   int blockNumber = pcbData.getInsert()->getBlockNumber();

   if (! m_pcbMap.Lookup(blockNumber,pcb))
   {
      BlockStruct* pcbGeometry = camCadDatabase.getBlock(blockNumber);
      pcb = new CViTechAoiWriterPcb(camCadDatabase,*pcbGeometry);

      m_pcbMap.SetAt(blockNumber,pcb);
   }

   return *pcb;
}

//_____________________________________________________________________________
CViTechAoiWriterPcbInstance::CViTechAoiWriterPcbInstance(CViTechAoiWriterPcb& pcb,DataStruct& pcbData)
: m_pcb(pcb)
, m_pcbData(pcbData)
, m_xOutData(NULL)
{
}

//_____________________________________________________________________________
CViTechAoiWriterPcbInstanceList::CViTechAoiWriterPcbInstanceList()
{
}

CViTechAoiWriterPcbInstance* CViTechAoiWriterPcbInstanceList::addPcbInstance(CCamCadDatabase& camCadDatabase,CViTechAoiWriterPcbMap& pcbMap,DataStruct& pcbData)
{
   CViTechAoiWriterPcb& pcb = pcbMap.getPcb(camCadDatabase,pcbData);

   CViTechAoiWriterPcbInstance* pcbInstance = new CViTechAoiWriterPcbInstance(pcb,pcbData);
   m_pcbInstanceList.AddTail(pcbInstance);

   return pcbInstance;
}

POSITION CViTechAoiWriterPcbInstanceList::getHeadPosition() const
{
   return m_pcbInstanceList.GetHeadPosition();
}

CViTechAoiWriterPcbInstance* CViTechAoiWriterPcbInstanceList::getNext(POSITION& pos) const
{
   return m_pcbInstanceList.GetNext(pos);
}

int CViTechAoiWriterPcbInstanceList::getCount() const
{
   return m_pcbInstanceList.GetCount();
}

CViTechAoiWriterPcbInstance* CViTechAoiWriterPcbInstanceList::find(const CString& refDes)
{
   CViTechAoiWriterPcbInstance* pcbInstance = NULL;

   for (POSITION pos = m_pcbInstanceList.GetHeadPosition();pos != NULL;)
   {
      pcbInstance = m_pcbInstanceList.GetNext(pos);

      if (pcbInstance->getPcbData().getInsert()->getRefname().CompareNoCase(refDes) == 0)
      {
         break;
      }

      pcbInstance = NULL;
   }

   return pcbInstance;
}

//_____________________________________________________________________________
CViTechAoiWriter::CViTechAoiWriter(CCEtoODBDoc& camCadDoc,const CString& filePath)
: m_camCadDatabase(camCadDoc)
, m_filePath(filePath)
, m_aoiFile(512)
, m_xOutDataList(false)
, m_aoiUnitDecimalPointString(".")
, m_jedecKeywordIndex(-1)
, m_enableXOutSkips(true)
, m_mirrorBottomRotations(true)
{
   m_logFile       = NULL;
   m_messageFilter = NULL;
   m_panelFile     = NULL;

   // settings
   m_aoiUnits                = pageUnitsInches;
   m_aoiUnitPrecision        = 3;
   m_exportUnloadedPartsFlag = false;
}

CViTechAoiWriter::~CViTechAoiWriter()
{
   delete m_messageFilter;
   delete m_logFile;
}

CCEtoODBDoc& CViTechAoiWriter::getCamCadDoc()
{
   return m_camCadDatabase.getCamCadDoc();
}

CCamCadData& CViTechAoiWriter::getCamCadData()
{
   return m_camCadDatabase.getCamCadData();
}

CWriteFormat& CViTechAoiWriter::getLogFile()
{
   if (m_logFile == NULL)
   {
      CFilePath logFilePath(GetLogfilePath("ViTechAoi.log"));
      //logFilePath.pushLeaf("ViTechAoi.log");

      CStdioFileWriteFormat* logFile = new CStdioFileWriteFormat(logFilePath.getPath(),512);
      m_logFile = logFile;
   }

   return *m_logFile;
}

CMessageFilter& CViTechAoiWriter::getMessageFilter()
{
   if (m_messageFilter == NULL)
   {
      m_messageFilter = new CMessageFilter(messageFilterTypeMessage);
   }

   return *m_messageFilter;
}

CProgressDlg& CViTechAoiWriter::getProgressDialog()
{
   extern CProgressDlg* progress;

   return *progress;
}

bool CViTechAoiWriter::parseYesNo(bool& flag,CString value)
{
   if (value.GetLength() > 0)
   {
      value = value.Left(1);

      if (value.CompareNoCase("y") == 0)
      {
         flag = true;
      }
      else if (value.CompareNoCase("n") == 0)
      {
         flag = false;
      }
   }

   return flag;
}

void CViTechAoiWriter::loadSettings()
{
   CString outFilePath( getApp().getExportSettingsFilePath("vi_tech.out") );

   COutFile outFile;

   if (outFile.open(outFilePath))
   {
      while (outFile.getNextCommandLine())
      {
         if (outFile.isCommand(".EXPORT_UNLOADED_PARTS",2))
         {
            outFile.parseYesNoParam(1,m_exportUnloadedPartsFlag,false);
         }
         else if (outFile.isCommand(".ExportUnloadedParts",2))
         {
            outFile.parseYesNoParam(1,m_exportUnloadedPartsFlag,false);
         }
         else if (outFile.isCommand(".Units",2))
         {
            CString unitsString = outFile.getParam(1);

            if (unitsString.CompareNoCase("mm") == 0)
            {
               m_aoiUnits = pageUnitsMilliMeters;
            }
            else if ((unitsString.CompareNoCase("inch") == 0) || (unitsString.CompareNoCase("inches") == 0))
            {
               m_aoiUnits = pageUnitsInches;
            }
         }
         else if (outFile.isCommand(".UnitPrecision",2))
         {
            m_aoiUnitPrecision = outFile.getIntParam(1);

            if (m_aoiUnitPrecision < 0)
            {
               m_aoiUnitPrecision = 0;
            }
            else if (m_aoiUnitPrecision > 10)
            {
               m_aoiUnitPrecision = 10;
            }
         }
         else if (outFile.isCommand(".UnitDecimalPoint",2))
         {
            CString aoiUnitDecimalPointString = outFile.getParam(1);

            if (! aoiUnitDecimalPointString.IsEmpty())
            {
               m_aoiUnitDecimalPointString = aoiUnitDecimalPointString;
            }
         }
         else if (outFile.isCommand(".JedecAttributeName",2))
         {
            m_jedecAttributeName = outFile.getParam(1);
            const KeyWordStruct* jedecKeyword = m_camCadDatabase.getKeyword(m_jedecAttributeName);

            if (jedecKeyword != NULL)
            {
               m_jedecKeywordIndex = jedecKeyword->getIndex();
            }
         }
         else if (outFile.isCommand(".EnableXOutSkips",2))
         {
            outFile.parseYesNoParam(1,m_enableXOutSkips,true);
         }
         else if (outFile.isCommand(".MirrorBottomComponentRotations",2))
         {
            outFile.parseYesNoParam(1,m_mirrorBottomRotations,true);
         }
      }
   }
}

void CViTechAoiWriter::writeAoiFiles()
{
   m_panelFile = m_camCadDatabase.getSingleVisiblePanel();

   if (m_panelFile == NULL)
   {
      formatMessageBox("One and only one panel must be visible; output terminated");
   }
   else
   {
      loadSettings();

      // generate centroids for PCB component geometries
      getCamCadDoc().OnGenerateComponentCentroid();

      scanData();
      writeAoiFile(true);
      writeAoiFile(false);
   }
}

void CViTechAoiWriter::writeAoiFile(bool topFlag)
{
   m_topFlag = topFlag;

   CFilePath filePath(m_filePath);
   CString fileName = filePath.getBaseFileName();

   if (fileName.GetLength() > 2)
   {
      CString fileNameSuffix = fileName.Right(2).MakeUpper();

      if (fileNameSuffix == "_T" || fileNameSuffix == "_B")
      {
         fileName.GetBufferSetLength(fileName.GetLength() - 2);
      }
   }

   filePath.setBaseFileName(fileName + (topFlag ? "_T" : "_B"));

   if (!m_aoiFile.open(filePath))
   {
      formatMessageBox("Could not open the file '%s' for writing",filePath);
   }
   else
   {
      m_aoiFile.writef("# VI Technology .vis file format\n");
      m_aoiFile.writef("# Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
      m_aoiFile.writef("# Generation date.time: %s\n",CTime::GetCurrentTime().Format("%Y%m%d.%H%M"));

      m_panelMatrix = m_panelFile->getTMatrix();

      if (!topFlag)
      {
         m_panelMatrix.mirrorAboutYAxisCtm();
      }

      writeGeneralInformationSection();
      writeInformationOnThePanelSection();
      writeBoardArrangementSection();
      writeFiducialAndSkipArrangementSection();
      writeCadCoordinatesSection();

      m_aoiFile.close();
   }
}

void CViTechAoiWriter::scanData()
{
   // find extents of panel outline
   BlockStruct* panelGeometry = m_panelFile->getBlock();

   for (CDataListIterator outlineIterator(*panelGeometry,dataTypePoly);outlineIterator.hasNext();)
   {
      DataStruct* polyStruct = outlineIterator.getNext();

      if (polyStruct->getGraphicClass() == graphicClassPanelOutline)
      {
         m_panelExtent.update(polyStruct->getPolyList()->getExtent(getCamCadData()));
      }
   }

   // find board instances and XOut instances
   for (CDataListIterator insertIterator(*panelGeometry,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* data = insertIterator.getNext();

      if (data->getInsert()->getInsertType() == insertTypePcb)
      {
         m_pcbInstanceList.addPcbInstance(m_camCadDatabase,m_pcbMap,*data);
      }
      else if (data->getInsert()->getInsertType() == insertTypeXout)
      {
         m_xOutDataList.AddTail(data);
      }
   }

   // find extents of board instances
   for (POSITION pos = m_pcbInstanceList.getHeadPosition();pos != NULL;)
   {
      CViTechAoiWriterPcbInstance* pcbInstance = m_pcbInstanceList.getNext(pos);
      DataStruct& pcbData = pcbInstance->getPcbData();
      CViTechAoiWriterPcb& pcb = pcbInstance->getPcb();

      CPolygon boardOutline = pcb.getPcbOutline();

      CTMatrix matrix = pcbData.getInsert()->getTMatrix();
      boardOutline.transform(matrix);
      CExtent pcbExtent = boardOutline.getExtent();

      m_boardInstancesExtent.update(pcbExtent);
   }

   if (!m_panelExtent.isValid())
   {
      m_panelExtent = m_boardInstancesExtent;
   }
}

CString CViTechAoiWriter::panelUnits(double pcbUnits)
{
   CString panelUnits(fpfmt(getCamCadDoc().convertPageUnitsTo(m_aoiUnits,pcbUnits),m_aoiUnitPrecision));

   panelUnits.Replace(".",m_aoiUnitDecimalPointString);

   return panelUnits;
}

CPoint2d CViTechAoiWriter::convertPageUnitsToMachineUnits(const CPoint2d& point)
{
   CPoint2d machinePoint = point + m_gOffset;

   return machinePoint;
}

void CViTechAoiWriter::writeGeneralInformationSection()
{
   m_aoiFile.writef("\n# Section: General Information\n\n");

   CString panelName = m_panelFile->getName();

   if (panelName.IsEmpty())
   {
      panelName = "Test";
   }

   panelName.MakeUpper();

   m_aoiFile.writef("PANEL_NAME %s\n",panelName);
   m_aoiFile.writef("CLEAR\n");
   m_aoiFile.writef("UNIT %s\n",(m_aoiUnits == pageUnitsInches) ? "inch" : "mm");

   CExtent panelExtent(m_panelExtent);
   panelExtent.transform(m_panelMatrix);

   m_gOffset = -panelExtent.getLR();

   m_aoiFile.writef("G_OFFSET %s %s\n",panelUnits(m_gOffset.x),panelUnits(m_gOffset.y));

   m_aoiFile.writef("SIDE_NUMBER %s\n",(m_topFlag ? "TP" : "BT"));
   m_aoiFile.writef("BARCODE TOP\n");
}

void CViTechAoiWriter::writeInformationOnThePanelSection()
{
   m_aoiFile.writef("\n# Section: Information on the Panel\n\n");

   CExtent panelExtent(m_panelExtent);
   panelExtent.transform(m_panelMatrix);

   CPoint2d llPoint = convertPageUnitsToMachineUnits(panelExtent.getLL());
   CPoint2d urPoint = convertPageUnitsToMachineUnits(panelExtent.getUR());

   m_aoiFile.writef("PANEL_DIM %s %s %s %s\n",
      panelUnits(llPoint.x),
      panelUnits(llPoint.y),
      panelUnits(urPoint.x),
      panelUnits(urPoint.y)  );

   CExtent boardInstancesExtent(m_boardInstancesExtent);
   boardInstancesExtent.transform(m_panelMatrix);

   llPoint = convertPageUnitsToMachineUnits(boardInstancesExtent.getLL());
   urPoint = convertPageUnitsToMachineUnits(boardInstancesExtent.getUR());

   m_aoiFile.writef("USED_DIM %s %s %s %s\n",
      panelUnits(llPoint.x),
      panelUnits(llPoint.y),
      panelUnits(urPoint.x),
      panelUnits(urPoint.y)  );

   m_aoiFile.writef("NB_BOARD %d\n",m_pcbInstanceList.getCount());
   m_aoiFile.writef("COUNTER_CLOCKWISE\n");
}

void CViTechAoiWriter::writeBoardArrangementSection()
{
   m_aoiFile.writef("\n# Section: Board Arrangement\n");

   int boardIndex = 1;

   for (POSITION pos = m_pcbInstanceList.getHeadPosition();pos != NULL;)
   {
      CViTechAoiWriterPcbInstance* pcbInstance = m_pcbInstanceList.getNext(pos);
      DataStruct& pcbData = pcbInstance->getPcbData();
      CViTechAoiWriterPcb& pcb = pcbInstance->getPcb();
      CPolygon boardOutline = pcb.getPcbOutline();

      // use premultiplication
      CTMatrix matrix = pcbData.getInsert()->getTMatrix() * m_panelMatrix;
      boardOutline.transform(matrix);

      m_aoiFile.writef("\n# Pcb Refname='%s'\n",pcbData.getInsert()->getRefname());
      m_aoiFile.writef("BOARD_POLYGON %d %d",boardIndex,boardOutline.getVertexCount());

      for (POSITION pos = boardOutline.getHeadPointPosition();pos != NULL;)
      {
         CPoint2d vertexPoint = boardOutline.getNextPoint(pos);
         vertexPoint = convertPageUnitsToMachineUnits(vertexPoint);

         m_aoiFile.writef(" %s %s",panelUnits(vertexPoint.x),
                                   panelUnits(vertexPoint.y)  );
      }

      m_aoiFile.writef("\n");

      boardIndex++;
   }
}

void CViTechAoiWriter::writeFiducialAndSkipArrangementSection()
{
   m_aoiFile.writef("\n# Section: Fiducial and Skip Arrangement\n");

   // output panel fiducials
   int fiducialIndex = 1;
   BlockStruct* panelGeometry = m_panelFile->getBlock();

   for (CDataListIterator fiducialIterator(*panelGeometry,insertTypeFiducial);fiducialIterator.hasNext();)
   {
      DataStruct* fiducialData = fiducialIterator.getNext();
      CBasesVector fiducialBasesVector = fiducialData->getInsert()->getBasesVector();
      fiducialBasesVector.transform(m_panelMatrix);
      bool placedTopFlag = !fiducialBasesVector.getMirror();

      if (placedTopFlag)
      {
         m_aoiFile.writef("\n# Panel Fiducial RefDes='%s'\n",fiducialData->getInsert()->getRefname());
         m_aoiFile.writef("FM %d %s %s\n",
            fiducialIndex,
            panelUnits(fiducialBasesVector.getOrigin().x),
            panelUnits(fiducialBasesVector.getOrigin().y)  );

         fiducialIndex++;
      }
   }

   // output board fiducials
   int boardIndex = 1;

   for (POSITION pos = m_pcbInstanceList.getHeadPosition();pos != NULL;)
   {
      CViTechAoiWriterPcbInstance* pcbInstance = m_pcbInstanceList.getNext(pos);
      DataStruct& pcbData = pcbInstance->getPcbData();
      CViTechAoiWriterPcb& pcb = pcbInstance->getPcb();
      CPolygon boardOutline = pcb.getPcbOutline();

      // use premultiplication
      CTMatrix matrix = pcbData.getInsert()->getTMatrix() * m_panelMatrix;

      fiducialIndex = 1;

      BlockStruct* pcbGeometry = m_camCadDatabase.getBlock(pcbData.getInsert()->getBlockNumber());

      for (CDataListIterator fiducialIterator(*pcbGeometry,insertTypeFiducial);fiducialIterator.hasNext();)
      {
         DataStruct* fiducialData = fiducialIterator.getNext();
         CBasesVector fiducialBasesVector = fiducialData->getInsert()->getBasesVector();
         fiducialBasesVector.transform(matrix);
         bool placedTopFlag = !fiducialBasesVector.getMirror();

         if (placedTopFlag)
         {
            m_aoiFile.writef("\n# Board Refname='%s', Board Fiducial RefDes='%s'\n",
               pcbData.getInsert()->getRefname(),fiducialData->getInsert()->getRefname());
            m_aoiFile.writef("FMB %d %d %s %s\n",
               boardIndex,
               fiducialIndex,
               panelUnits(fiducialBasesVector.getOrigin().x),
               panelUnits(fiducialBasesVector.getOrigin().y)  );

            fiducialIndex++;
         }
      }
      
      boardIndex++;
   }

   // output skips
   if (m_enableXOutSkips)
   {
      for (POSITION pos = m_xOutDataList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* xOutData = m_xOutDataList.GetNext(pos);
         CString refDes = xOutData->getInsert()->getRefname();

         CViTechAoiWriterPcbInstance* pcbInstance = m_pcbInstanceList.find(refDes);

         if (pcbInstance != NULL)
         {
            if (pcbInstance->getXOutData() == NULL)
            {
               pcbInstance->setXOutData(xOutData);
            }
         }
      }

      boardIndex = 1;
      int skipIndex = 1; // for now, only use the first xout encountered for the board

      for (POSITION pos = m_pcbInstanceList.getHeadPosition();pos != NULL;)
      {
         CViTechAoiWriterPcbInstance* pcbInstance = m_pcbInstanceList.getNext(pos);
         DataStruct* xOutData = pcbInstance->getXOutData();

         if (xOutData != NULL)
         {
            CBasesVector xOutBasesVector = xOutData->getInsert()->getBasesVector();
            xOutBasesVector.transform(m_panelMatrix);
            bool placedTopFlag = !xOutBasesVector.getMirror();

            if (placedTopFlag)
            {
               m_aoiFile.writef("\n# Board Skip RefDes='%s'\n",xOutData->getInsert()->getRefname());
               m_aoiFile.writef("SKIP %d %d %s %s\n",
                  boardIndex,
                  skipIndex,
                  panelUnits(xOutBasesVector.getOrigin().x),
                  panelUnits(xOutBasesVector.getOrigin().y)  );
            }
         }

         boardIndex++;
      }
   }
}

bool CViTechAoiWriter::isPcbComponentLoaded(DataStruct& pcbComponent)
{
   int loadedKeywordIndex = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeLoaded);
   CString loadedValue;

   m_camCadDatabase.getAttributeStringValue(loadedValue,pcbComponent.attributes(),loadedKeywordIndex);

   bool isLoaded = (loadedValue.CompareNoCase("FALSE") != 0);

   return isLoaded;
}

CString CViTechAoiWriter::getJedec(DataStruct& pcbComponent)
{
   BlockStruct* pcbComponentGeometry = m_camCadDatabase.getBlock(pcbComponent.getInsert()->getBlockNumber());
   CString jedec = pcbComponentGeometry->getName();

   if (m_jedecKeywordIndex > -1)
   {
      m_camCadDatabase.getAttributeStringValue(jedec,pcbComponent.attributes(),m_jedecKeywordIndex);
   }

   return jedec;
}

void CViTechAoiWriter::writeCadCoordinatesSection()
{
   m_aoiFile.writef("\n# Section: CAD Coordinates\n\n");
   m_aoiFile.writef("CAD_IMP XYAPTJ\n");

   int boardIndex = 1;
   int partNumberKeywordIndex = getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributePartNumber);

   for (POSITION pos = m_pcbInstanceList.getHeadPosition();pos != NULL;)
   {
      CViTechAoiWriterPcbInstance* pcbInstance = m_pcbInstanceList.getNext(pos);
      DataStruct& pcbData = pcbInstance->getPcbData();
      CViTechAoiWriterPcb& pcb = pcbInstance->getPcb();
      CPolygon boardOutline = pcb.getPcbOutline();

      m_aoiFile.writef("\n# Board Refname='%s'\n",pcbData.getInsert()->getRefname());
      m_aoiFile.writef("COMP %d\n",boardIndex);

      // use premultiplication
      CTMatrix pcbMatrix = pcbData.getInsert()->getTMatrix() * m_panelMatrix;
      CBasesVector pcbBasesVector;
      pcbBasesVector.transform(pcbMatrix);
      bool mirrorFlag = pcbBasesVector.getMirror();

      BlockStruct* pcbGeometry = m_camCadDatabase.getBlock(pcbData.getInsert()->getBlockNumber());

      for (CDataListIterator pcbComponentIterator(*pcbGeometry,insertTypePcbComponent);pcbComponentIterator.hasNext();)
      {
         DataStruct* componentData = pcbComponentIterator.getNext();

         if (m_exportUnloadedPartsFlag || isPcbComponentLoaded(*componentData))
         {
            CBasesVector componentBasesVector = componentData->getInsert()->getBasesVector();
            componentBasesVector.transform(pcbMatrix);
            bool placedTopFlag = !componentBasesVector.getMirror();
            CString refDes = componentData->getInsert()->getRefname();

            if (placedTopFlag)
            {
               BlockStruct* componentGeometry = m_camCadDatabase.getBlock(componentData->getInsert()->getBlockNumber());
               DataStruct* centroidData = centroid_exist_in_block(componentGeometry);

               if (centroidData != NULL)
               {
                  CBasesVector componentBasesVector = centroidData->getInsert()->getBasesVector();

                  // use premultiplication
                  CTMatrix componentMatrix = componentData->getInsert()->getTMatrix() * pcbMatrix;
                  componentBasesVector.transform(componentMatrix);

                  CString partNumber("Unknown");
                  m_camCadDatabase.getAttributeStringValue(partNumber,componentData->attributes(),partNumberKeywordIndex);

                  CString jedec  = getJedec(*componentData);
                  double rotation = componentBasesVector.getRotationDegrees();

                  if (mirrorFlag && m_mirrorBottomRotations)
                  {
                     rotation = 180. - rotation;
                  }

                  rotation = normalizeDegrees(atof(fpfmt(rotation,3)));

                  m_aoiFile.writef("%s %s %s %s %s %s\n",
                     panelUnits(componentBasesVector.getOrigin().x),
                     panelUnits(componentBasesVector.getOrigin().y),
                     fpfmt(rotation,3),
                     partNumber,
                     refDes,
                     quoteString(jedec));
               }
               else
               {
                  getLogFile().writef("centroid instance missing from pcb component geometry '%s'\n",componentGeometry->getName());
               }
            }
         }
      }
      
      boardIndex++;
   }
}






