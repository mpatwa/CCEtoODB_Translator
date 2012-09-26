
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "ccdoc.h"
#include "Juki_Out.h"
#include "PcbUtil.h"
#include "CCEtoODB.h"
#include "centroid.h"
#include "extents.h"
#include "InFile.h"
#include "xform.h"

static CString createOutfileName(CString basename, CString filenameSuffix);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define NormalizeZero(x) fpeq(x,0.,JUKI_SMALLVALUE)?0.:x

void Juki_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   CString topFileName = createOutfileName(filename, "_top");
   CString botFileName = createOutfileName(filename, "_bot");
   
   
   JukiWrite writer(&doc, format->Scale);

   if (!writer.Write(topFileName, botFileName))
   {
      ErrorMessage("CAMCAD could not finish writing the Juki file.", "", MB_OK);
   }
}

//---------------------------------------------------------------------------

static CString createOutfileName(CString basename, CString filenameSuffix)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath(basename, drive, dir, fname, ext );

	if ((strlen(fname) + strlen(filenameSuffix)) >= _MAX_FNAME)
		fname[ _MAX_FNAME - strlen(filenameSuffix) - 1] = '\0';

	strcat(fname, filenameSuffix);

   char constructedName[_MAX_PATH];

   _makepath(constructedName, drive, dir, fname, ext);

	return constructedName;
}

////////////////////////////////////////////////////////////
// CJukiSetting
////////////////////////////////////////////////////////////
CJukiSetting::CJukiSetting(CCEtoODBDoc* doc)
: m_pDoc(doc)
, m_attribMap(*doc)
{
}

void CJukiSetting::LoadDefaultSettings()
{
   // default attribute and value to look for
	m_attribMap.SetDefaultAttribAndValue();
   m_exportUNLoaded = true;
   m_flattenOutput = true;
   m_customAttribute.Empty();
   m_dUnitFactor = 1;
   m_Units = DEFAULT_JUKI_UNITS;
   m_PackageName.Empty();
   m_pcbthickness = 1.6;//1.6 mm
   m_badmarkType = "L";
}

bool CJukiSetting::LoadStandardSettingsFile(CString FileName)
{
   LoadDefaultSettings();

   CInFile inFile;
   if(!inFile.open(FileName))
   {
      CString tmp;
      tmp.Format("File [%s] is not found", FileName);
      ErrorMessage( tmp, "Juki Write Settings", MB_OK | MB_ICONHAND);
      return FALSE;
   }

   while (inFile.getNextCommandLine())
   {
      if (inFile.isCommand(".PACKAGE_NAME",2)) m_PackageName = inFile.getParam(1);
      if (inFile.isCommand(".EXPORT_UNLOADED_PARTS",2))  inFile.parseYesNoParam(1,m_exportUNLoaded,true);
      if (inFile.isCommand(".FLATTEN_OUTPUT",2)) inFile.parseYesNoParam(1,m_flattenOutput,true);
      if (inFile.isCommand(".UNITS",2)) 
      {          
         m_Units = StringToPageUnitsTag(inFile.getParam(1));
         switch(m_Units)
         {
         case pageUnitsInches:
         case pageUnitsMils:
         case pageUnitsMilliMeters:
         case pageUnitsCentimeters:
            break;
         default:
            m_Units = pageUnitsMils;
              break;
         }
      }
      if (inFile.isCommand(".OUTPUT_ATTRIBUTE",2)) m_attribMap.SetKeyword(inFile.getParam(1));
      if (inFile.isCommand(".OUTPUT_VALUE",2)) m_attribMap.AddValue(inFile.getParam(1));
      if (inFile.isCommand(".CUSTOM_ATTRIBUTE",2)) m_customAttribute = inFile.getParam(1);
      if (inFile.isCommand(".PCB_THICKNESS",2)) m_pcbthickness = atof(inFile.getParam(1));
      if (inFile.isCommand(".BBR_SETTING",2)) m_badmarkType = inFile.getParam(1);      
   }

   m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), m_Units);
   m_pcbthickness = m_pcbthickness * Units_Factor(pageUnitsMilliMeters, m_Units);;
   inFile.close();
  
   return TRUE;   
}

////////////////////////////////////////////////////////////
// JukiWrite
////////////////////////////////////////////////////////////
JukiWrite::JukiWrite(CCEtoODBDoc *document, double l_Scale)
: m_pDoc(document)
, p_Scale(l_Scale)
, m_Settings(document)
, m_camCadData(document->getCamCadData())
, m_lowerRightCornerX(0.)
{
   m_panelOffset.x = 0.;
   m_panelOffset.y = 0.;
}

BOOL JukiWrite::Write(CString topFileName, CString botFileName)
{
   if (!topFile.Open(topFileName, CFile::modeCreate|CFile::modeWrite) || !botFile.Open(botFileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage("File could not be opened", "", MB_OK);
      return FALSE;
   }
     
   // generate centroid in order to get centroid of the insert.
   m_pDoc->OnGenerateComponentCentroid();

   LoadSettings(getApp().getExportSettingsFilePath("Juki.out"));

   if(!ProcessPCB())
      ProcessPanel();

   return TRUE;
}

int JukiWrite::LoadSettings(CString settingsFile)
{
   bool relVal = m_Settings.LoadStandardSettingsFile(settingsFile);
   
   m_dUnitFactor = m_Settings.getUnitFactor() * p_Scale;
   m_customAttribute = m_Settings.getCustomAttribute();

   return relVal;
}

void JukiWrite::WriteDataLine(CStringArray &DataList)
{
   for(int idx = 0 ;idx < DataList.GetCount(); idx++)
      WriteDataLine(DataList.GetAt(idx));     
}

void JukiWrite::WriteDataLine(CString DataLine)
{
   WriteDataLine(&topFile, DataLine);
   WriteDataLine(&botFile, DataLine);
}

void JukiWrite::WriteDataLine(CFormatStdioFile *baseFile, CStringArray &DataList)
{
   for(int idx = 0 ;idx < DataList.GetCount(); idx++)
      WriteDataLine(baseFile, DataList.GetAt(idx));     
}

void JukiWrite::WriteDataLine(CFormatStdioFile *baseFile, CString DataLine)
{
   baseFile->WriteString (DataLine);
}

void JukiWrite::WritePanelOffset()
{
   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};
   CStringArray OffsetDataList;
   CString dataString;
   int errboard = 0;

   OffsetDataList.Add("PANEL\n");
   OffsetDataList.Add("Information\tX\tY\tThickness\n");
   dataString.Format("PWB ID\t%s \n",m_PCBDataList.getBoardName());
   OffsetDataList.Add(dataString);
   WriteDataLine(OffsetDataList);   

   for(int isBottom = 0; isBottom < 2; isBottom++)
   {
      OffsetDataList.RemoveAll();
      double Width = 0.,Height = 0.;
      int panelCnt = 0;

      for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
      {
         CString pcbName;
         CJukiPCBData *pcbData =NULL;
         
         m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
         if(pcbData && pcbData->getPlacedBoardType() == blockTypePanel && !panelCnt)
         {
            ExtentRect extent;

            if(GetOutlineExtent(pcbData,&extent, GR_CLASS_PANELOUTLINE))
            {
               Width = extent.right - extent.left;
               Height = extent.top - extent.bottom;
               m_panelOffset.x = (!isBottom)?extent.right:0.;
               m_panelOffset.y = (!isBottom)?extent.bottom:0.;
            }
            else // if panel outline does not exist, use panel block
            {
               errboard ++;
               CExtent bkextent = pcbData->getGeometryBlock()->getExtent();
               Width = getJukiUnits(bkextent.getSize().cx);
               Height = getJukiUnits(bkextent.getSize().cy);
               m_panelOffset.x = (!isBottom)?bkextent.getLR().x:0.;
               m_panelOffset.y = 0.;
            }

            panelCnt++;
         }
      }//for

      dataString.Format("PWB Dimension\t%.*lf\t%.*lf\t%.*lf\n", m_loc_accuracy, Width, m_loc_accuracy, Height, m_loc_accuracy, m_PCBDataList.getBoardThickness());
      OffsetDataList.Add(dataString);

      dataString.Format("PWB Layout Offset\t%.*lf\t%.*lf\n", m_loc_accuracy, m_panelOffset.x, m_loc_accuracy, m_panelOffset.y);
      OffsetDataList.Add(dataString);
      OffsetDataList.Add("\n\n");         

      //Write data to file
      WriteDataLine(m_inputFile[isBottom],OffsetDataList);
   }

   if(errboard)
      ErrorMessage("Panel Outline does not exist, using Panel block extent instead.", "", MB_OK);
  
}

void JukiWrite::WriteFiducialOffset()
{
   WriteDataLine("PANEL FIDUCIALS\n");
   WriteFiducialList(blockTypePanel,false);
   WriteDataLine("\n\n");
}

void JukiWrite::WriteBadMark()
{
   //Bad Board Mark
   WriteDataLine("Bad Mark\tx\ty\ttype\n");
   WriteXOutList(blockTypePanel,false,false);
   WriteDataLine("\n\n");
}

void JukiWrite::WriteMatrixMethod()
{
   CStringArray OffsetDataList;
   CString dataString;

   int fidCount[2] = {0,0};
   CJukiPCBData *pcbData =NULL;
   if(m_PCBDataList.Lookup(m_PCBDataList.getBoardName(),pcbData) && pcbData)
   {
      fidCount[0] = pcbData->getFiducialDataList().getInsertCount(false); //Bottom
      fidCount[1] = pcbData->getFiducialDataList().getInsertCount(true); //Top
   }

   OffsetDataList.Add("STEP& REPEAT\n");
   OffsetDataList.Add("PCB type\tPCB Positioning Reference\tBOC mark type\n");
   OffsetDataList.Add("N\tS\tN\n");

   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};
   for(int isBottom = 0; isBottom < 2; isBottom++)
   {
      dataString.Format("N\tS\t%s\n",(fidCount[isBottom] < 2)?"N":"P");
      OffsetDataList.SetAt(2,dataString);
      WriteDataLine(m_inputFile[isBottom],OffsetDataList);
      WriteDataLine("\n\n");
   }
}

void JukiWrite::WriteComponentHeader()
{
   CString dataline;
   dataline.Format("Ref Designator\tX\tY\tRotation\tPart Number%s",!m_customAttribute.IsEmpty()?"\t" + m_customAttribute:"");
   if(!m_Settings.getFlattenOutput())
      dataline.Append("\tlocFid1 X\tlocFid1 Y\tlocFid2 X\tlocFid2 Y\tlocFid3 X\tlocFid3 Y");
   dataline.Append("\n");
   WriteDataLine(dataline);        
}

void JukiWrite::WriteBoardOffset()
{
   CStringArray OffsetDataList;
   CString dataString;
   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};
  
   //Write Stencil Block
   BlockStruct *targetblock = m_targetPCBdata->getGeometryBlock();   
   dataString.Format("Information\tX\tY\trotation\t%s\n\n",targetblock->getName());
   OffsetDataList.Add(dataString);
   WriteDataLine(OffsetDataList);   
   
   for(int isBottom = 0; isBottom < 2; isBottom++)
   {
      double Width = 0.,Height = 0.,shiftX = 0.,shiftY = 0.;
      ExtentRect extent;
      OffsetDataList.RemoveAll();

      if(GetOutlineExtent(m_targetPCBdata,&extent,GR_CLASS_BOARDOUTLINE))
      {
         Width = extent.right - extent.left;
         Height = extent.top - extent.bottom;
         shiftX = extent.left * ((isBottom)?-1:1);
         shiftY = extent.bottom;
      }
      else // if board outline does not exist, use board block
      {
         ErrorMessage("Board Outline does not exist, use Board block instead", "", MB_OK);
         
         CExtent bkextent = targetblock->getExtent();
         Width = getJukiUnits(bkextent.getSize().cx);
         Height = getJukiUnits(bkextent.getSize().cy);
         shiftX = 0.;
         shiftY = 0.;
      }

      dataString.Format("Circuit Dimension and Layout Offset\t%.*lf\t%.*lf\t%.*lf\t%.*lf\n", 
         m_loc_accuracy, Width, m_loc_accuracy, Height, m_loc_accuracy, shiftX, m_loc_accuracy, shiftY);
      OffsetDataList.Add(dataString);

      //Write data to file
      WriteDataLine(m_inputFile[isBottom],OffsetDataList);
   }

   //Write PCB blocks in Panel
   OffsetDataList.RemoveAll();
   int boardCnt = 1;
   for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
   {
      CString pcbName;
      CJukiPCBData *pcbData = NULL;

      m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData && pcbData->getPlacedBoardType() == blockTypePcb)
      {         
         if(boardCnt == 1) m_targetPCBName = pcbName;
         if(targetblock && targetblock == pcbData->getGeometryBlock())
         {
            CString xStr,yStr,rot;
            xStr.Format("%.*lf", m_loc_accuracy, pcbData->getOriginX());
            yStr.Format("%.*lf", m_loc_accuracy, pcbData->getOriginY());
            rot.Format("%.*lf",m_rot_accuracy,pcbData->getRotation());

            dataString.Format("Circuit\t%s\t%s\t%s\t%s\n", xStr, yStr, rot, pcbName);
            OffsetDataList.Add(dataString);
            boardCnt ++;
         }
      }
   }//for

   OffsetDataList.Add("\n\n");
   WriteDataLine(OffsetDataList);
}

void JukiWrite::WriteXOutList(BlockTypeTag boardType, bool isShift, bool isExtraAttribute)
{  
   for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
   {
      CString pcbName;
      CJukiPCBData *pcbData =NULL;
      m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
      WriteXOut(pcbData,boardType,isShift,isExtraAttribute);
   }
}

void JukiWrite::WriteXOut(CJukiPCBData *pcbData, BlockTypeTag boardType, bool isShift, bool isExtraAttribute)
{
   int Xoutcnt[2] = {0,0};
   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};

   int xoutCnt = (pcbData)?pcbData->getXOutDataList().GetCount():0;
   int MaxXoutCnt = 1; // only allow one bad board 

   for(int xoutIndex = 0; xoutIndex < xoutCnt; xoutIndex++)
   {
      CJukiInsertData *xOutdata = pcbData->getXOutDataList().GetAt(xoutIndex);
      if(xOutdata && xOutdata->getInsertData() && (xOutdata->getPlacedBoardType() == boardType || boardType == blockTypeUndefined))
      {
         bool isBottom = (xOutdata->getInsertData()->getInsert()->getGraphicMirrored());        
         if(Xoutcnt[isBottom] < MaxXoutCnt)
         {
            CString xStr,yStr;
            xStr.Format("%.*lf", m_loc_accuracy, xOutdata->getX() - ((isShift)?m_panelOffset.x:0));
            yStr.Format("%.*lf", m_loc_accuracy, xOutdata->getY()- ((isShift)?m_panelOffset.y:0));
             
            m_inputFile[isBottom]->WriteString("%s\t%s\t%s\t%s\n",xOutdata->getRefName(), xStr, yStr, m_Settings.getBadMarkType());
         }

         Xoutcnt[isBottom] ++;
      }
   }   
}

void JukiWrite::WriteFiducialList(BlockTypeTag boardType, bool isExtraAttribute)
{  
   for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
   {
      CString pcbName;
      CJukiPCBData *pcbData =NULL;
      m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
      WriteFiducial(pcbData,boardType, isExtraAttribute);
   }
}

void JukiWrite::WriteFiducial(CJukiPCBData *pcbData, BlockTypeTag boardType, bool isExtraAttribute)
{
   int Fidcnt[2] = {0,0};
   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};

   int fidCnt = (pcbData)?pcbData->getFiducialDataList().GetCount():0;
   int MaxFidCnt = (!m_Settings.getFlattenOutput() && boardType == blockTypePanel)?MAXFIDSIZE:fidCnt;

   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex++)
   {
      CJukiInsertData *fiddata = pcbData->getFiducialDataList().GetAt(fidIndex);
      if(fiddata && fiddata->getInsertData() && (fiddata->getPlacedBoardType() == boardType || boardType == blockTypeUndefined))
      {
         bool isBottom = (fiddata->getInsertData()->getInsert()->getGraphicMirrored());        
         if(Fidcnt[isBottom] < MaxFidCnt)
         {
            CString xStr,yStr,rot,attribute;
            xStr.Format("%.*lf", m_loc_accuracy, fiddata->getX() );
            yStr.Format("%.*lf", m_loc_accuracy, fiddata->getY() );
            rot.Format("%.*lf",m_rot_accuracy,fiddata->getRotation());

             
            m_inputFile[isBottom]->WriteString("%s\t%s\t%s\t%s\t%s",fiddata->getRefName(), xStr, yStr, rot,fiddata->getPartNumber());
            m_inputFile[isBottom]->WriteString("%s\n",(getCustomAttribute(fiddata->getInsertData(),attribute) && isExtraAttribute)?"\t"+attribute:"");
         }
         Fidcnt[isBottom] ++;
      }
   }   
}

void JukiWrite::WriteComponentList(CString targetPcbName)
{
   CJukiPCBData *pcbData =NULL;
   if(!m_Settings.getFlattenOutput() && m_PCBDataList.Lookup(m_targetPCBName,pcbData) && pcbData)
   {
      WriteComponent(pcbData);
   }
   else
   {
      for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
      {
         CString pcbName;
         m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
         WriteComponent(pcbData);         
      }
   }//if
}

void JukiWrite::WriteComponent(CJukiPCBData *pcbData)
{
   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};

   int compCnt = (pcbData)?pcbData->getComponentDataList().GetCount():0;
   for(int compIndex = 0; compIndex < compCnt; compIndex++)
   {
      CJukiInsertData *cmpdata = pcbData->getComponentDataList().GetAt(compIndex);
      if(cmpdata && cmpdata->getInsertData())
      {
         CString xStr,yStr,rot,attribute;
         xStr.Format("%.*lf", m_loc_accuracy, cmpdata->getX());
         yStr.Format("%.*lf", m_loc_accuracy, cmpdata->getY());
         rot.Format("%.*lf", m_rot_accuracy, cmpdata->getRotation());

         bool isBottom = (cmpdata->getInsertData()->getInsert()->getGraphicMirrored());
         m_inputFile[isBottom]->WriteString("%s\t%s\t%s\t%s\t%s", cmpdata->getRefName(), xStr, yStr, rot, cmpdata->getPartNumber());
         m_inputFile[isBottom]->WriteString("%s",getCustomAttribute(cmpdata->getInsertData(),attribute)?"\t"+attribute:"");
         if(!m_Settings.getFlattenOutput())
         {
            WriteComponentFiducials(cmpdata, isBottom);
         }
         m_inputFile[isBottom]->WriteString("\n");
      }
   }//for 
}

void JukiWrite::WriteComponentFiducials(CJukiInsertData *cmpdata, bool isBottom)
{
   int Fidcnt[2] = {0,0};
   CFormatStdioFile *m_inputFile[2] = {&topFile, &botFile};

   int fidCnt = (cmpdata && cmpdata->getCompFidDataList())?cmpdata->getCompFidDataList()->GetCount():0;
   int MaxFidCnt = MAXFIDSIZE;

   for(int fidIndex = 0; fidIndex < fidCnt; fidIndex++)
   {
      CJukiInsertData *fiddata = (CJukiInsertData *)cmpdata->getCompFidDataList()->GetAt(fidIndex);
      if(fiddata && fiddata->getInsertData() && fiddata->getPlacedBoardType() == blockTypePcbComponent)
      {
         if(Fidcnt[isBottom] < MaxFidCnt)
         {
            CString xStr,yStr,rot,attribute;
            xStr.Format("%.*lf", m_loc_accuracy, fiddata->getX());
            yStr.Format("%.*lf", m_loc_accuracy, fiddata->getY());
            m_inputFile[isBottom]->WriteString("\t%s\t%s", xStr, yStr);
         }
         Fidcnt[isBottom]++;
      }
   }
}

void JukiWrite::TranslateFile(CTMatrix &l_panelMatrix, FileStruct *fileStruct)
{   
   l_panelMatrix.scale(fileStruct->getScale() * (fileStruct->getResultantMirror(False)?-1:1), fileStruct->getScale());

   l_panelMatrix.rotateRadians(fileStruct->getRotation());

   l_panelMatrix.translate(fileStruct->getInsertX(), fileStruct->getInsertY());
}

void JukiWrite::TranslateBoard(CTMatrix &l_panelMatrix, CTMatrix &l_boardMatrix, DataStruct *data)
{
   CPoint2d boardXY = data->getInsert()->getOrigin2d();
   l_panelMatrix.transform(boardXY);

   l_boardMatrix.scale(data->getInsert()->getScale() * (data->getInsert()->getGraphicMirrored()?-1:1), data->getInsert()->getScale());
   
   l_boardMatrix.rotateRadians(data->getInsert()->getAngle());

   l_boardMatrix.translate(boardXY);
}

CString JukiWrite::getPartNumber(DataStruct *data)
{
   Attrib *attrib = NULL;
   CString partNumber;

   attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
   partNumber = attrib?attrib->getStringValue():"No_Part_Number";

   if ((data->getInsert()->getInsertType() == insertTypeFiducial) && partNumber == "No_Part_Number")
      partNumber = "FID";

   return partNumber;
}

bool JukiWrite::IsLoaded(DataStruct *data)
{
   Attrib *attrib = NULL;
   bool loaded = true;

	if ((attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_LOADED, 1)) && attrib)
	{
		CString value = attrib->getStringValue();
      loaded = (value.CompareNoCase("true") == 0);
	}
   return loaded;
}

bool JukiWrite::IsWritePart(DataStruct *data)
{
   //rot.Format("%.2f", 360 - normalizeDegrees(RadToDeg(data->getInsert()->getAngle())));
   bool writePart = true;
   //Check for optional filter command from OUT file settings
   if(m_Settings.getAttributeMap().IsValid()) // if using OUT file command
   {
      //Check filter, part must have a value that is present in set of filter values
      writePart = (data->getAttributesRef() != NULL && m_Settings.getAttributeMap().HasValue(&data->getAttributesRef()));
   }

   return writePart;
}

double JukiWrite::getCczCentroidRotation(DataStruct *data)
{
   double dbu = 0.;
   DataStruct *centroidData = centroid_exist_in_block(m_pDoc->getBlockAt(data->getInsert()->getBlockNumber()));
   if (centroidData)
   {
       dbu = normalizeDegrees(RadToDeg( centroidData->getInsert()->getAngle() ));
   }

   return dbu;
}

double JukiWrite::getJukiCentroidRotation(DataStruct *data)
{
   if (data != NULL && data->getDataType() == dataTypeInsert)
   {
      double centRotDeg = getCczCentroidRotation(data);
      double insertRotDeg = data->getInsert()->getAngleDegrees();

      double adjustedInsertRot = (data->getInsert()->getGraphicMirrored()) ? (360. - insertRotDeg) : (insertRotDeg);

      return round( normalizeDegrees( centRotDeg + adjustedInsertRot ) );;
   }

   return 0.; // An error to get here.
}

bool JukiWrite::getCustomAttribute(DataStruct *data, CString& attribValue)
{
   if( m_Settings.getCustomAttribute().IsEmpty() || !data)
      return FALSE;

   if(!m_Settings.getCustomAttribute().CompareNoCase("Geometry"))
   {
      if(!data->getInsert()) return FALSE;
      BlockStruct *block = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber());
      attribValue = block->getName();
   }
   else
   {
      if(!data->getAttributesRef()) return FALSE;

      Attrib *attrib = NULL;
      if((attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_Settings.getCustomAttribute(), 0))) 
         attribValue = attrib->getStringValue();
      else
         return FALSE;
   }

   return TRUE;
}

double JukiWrite::getBoardThickness(BlockStruct *block)
{
   double thickness = m_Settings.getBoardThickness();
   Attrib* attrib = NULL;

   if(attrib = is_attvalue(m_pDoc, block->getAttributesRef(), BOARD_THICKNESS, 2))
      thickness = getJukiUnits(attrib->getDoubleValue());

   return thickness;
}

bool JukiWrite::GetOutlineExtent(CJukiPCBData *pcbData, ExtentRect *outlineExtent, int GRClass) 
{
   // Returns extent already converted to Juki units.

   if (!outlineExtent || !pcbData || !pcbData->getGeometryBlock())
		return false;

   BlockStruct *block = pcbData->getGeometryBlock();

   outlineExtent->left = FLT_MAX;
	outlineExtent->right = -FLT_MAX;
	outlineExtent->top = -FLT_MAX;
	outlineExtent->bottom = FLT_MAX;

   Mat2x2 matrix;
   RotMat2(&matrix, pcbData->getRotation());

   BOOL Found = FALSE;

   // find lower-left of visible Primary Board Outlines
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_POLY && data->getGraphicClass() == GRClass)
      {  

         Found = TRUE;

         ExtentRect extents;   
         PolyExtents(m_pDoc, data->getPolyList(), &extents, m_dUnitFactor, (pcbData->getMirror() ? MIRROR_FLIP | MIRROR_LAYERS : 0), pcbData->getOriginX(), pcbData->getOriginY(), &matrix, FALSE);

         if (extents.left < outlineExtent->left)
            outlineExtent->left = NormalizeZero(extents.left);

         if (extents.right > outlineExtent->right)
            outlineExtent->right = NormalizeZero(extents.right);

         if (extents.bottom < outlineExtent->bottom)
            outlineExtent->bottom = NormalizeZero(extents.bottom);

         if (extents.top > outlineExtent->top)
            outlineExtent->top = NormalizeZero(extents.top);

         break;
      }      
   }

   if (Found)
		return true;
	return false;
}

bool JukiWrite::ProcessPCB()
{
   FileStruct * lFile = m_pDoc->getFileList().GetFirstShown(blockTypePcb);
   if (lFile == NULL)
      return FALSE;

   CString lFileName = lFile->getName();
   BlockStruct* block = lFile->getBlock();
   
   if(!block) return FALSE;

   //only one PCB data
   if(!block->getName().IsEmpty()) m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.setPanelFlag(false);   
   m_PCBDataList.setPCBThickness(getBoardThickness(block));

   double OriginX = getJukiUnits(lFile->getInsertX());
   double OriginY = getJukiUnits(lFile->getInsertY());
   double Rot = normalizeDegrees(RadToDeg(lFile->getRotation()));

   CJukiPCBData *pcbData = new CJukiPCBData(OriginX, OriginY, Rot, lFile->isMirrored(), blockTypePcb,NULL,block);
   m_PCBDataList.SetAt(m_PCBDataList.getBoardName(),pcbData);

   ExtentRect pcbExtent;
   if(GetOutlineExtent(pcbData, &pcbExtent, GR_CLASS_BOARDOUTLINE))
   {
      // This is correct for size, but seems not correct for what we want to do.
      //m_pcbSizeX = pcbExtent.right - pcbExtent.left;
      // Instead let's try just LR X coord instead of true size.
      m_lowerRightCornerX = pcbExtent.right; // Already in Juki units
   }
   else // if board outline does not exist, use board block
   {
      CExtent bkextent = block->getExtent();
      if (bkextent.isValid())
         m_lowerRightCornerX = getJukiUnits(bkextent.getXmax()); // Need to convert to Juki units
   }

   TraversePCB(block, lFile->getTMatrix());

   m_rot_accuracy = 2;
   m_loc_accuracy = 2;

   if(!m_Settings.getFlattenOutput())
   {     
      m_targetPCBdata = pcbData;
      
      //write pcb information
      WritePanelOffset();
      WriteFiducialOffset();
      WriteBadMark();
      WriteMatrixMethod();
      WriteBoardOffset();
   }

   WriteComponentHeader();
   WriteComponentList("");

   //write End of File
   if(!m_Settings.getFlattenOutput())
      WriteDataLine("\nEOF\n");   

   return TRUE;
}

bool JukiWrite::TraverseComponent(BlockStruct* block, CTMatrix& l_boardMatrix, CJukiInsertData *insertData)
{
   if(!insertData || !insertData->getCompFidDataList() || !insertData->getInsertData()) return false;
   CJukiInsertDataList *compFidDataList = (CJukiInsertDataList *)insertData->getCompFidDataList();

   int intFidID = 0;
   for (POSITION dataPos = block->getHeadDataInsertPosition();dataPos;)
   {
      DataStruct* data = block->getNextDataInsert(dataPos);
      if(data && data->getInsert()->getInsertType() == insertTypeFiducial)
      {
         CTMatrix l_compMatrix(l_boardMatrix);
         CPoint2d compCentroid;
         CString refName, partNumber;

         int isNagtive = ((!insertData->getInsertData()->getInsert()->getGraphicMirrored())?1:-1);
         TranslateBoard(l_boardMatrix, l_compMatrix, insertData->getInsertData());    
 			compCentroid.x =  isNagtive * data->getInsert()->getOriginX();
			compCentroid.y  = data->getInsert()->getOriginY();
         l_compMatrix.transform(compCentroid);

         double xStr = isNagtive * getJukiUnits(compCentroid.x);
         double yStr = getJukiUnits(compCentroid.y);
         double rot = insertData->getRotation() + normalizeDegrees(RadToDeg(data->getInsert()->getAngle()));
         partNumber = getPartNumber(data);  
         refName.Format("FID_C%d", ++intFidID);   		

         compFidDataList->Add(data, xStr, yStr, rot, refName, partNumber, blockTypePcbComponent);
      }
   }

   return true;
}

bool JukiWrite::TraversePCB(BlockStruct* block, CTMatrix& l_boardMatrix)
{   
   CJukiPCBData *pcbData = NULL;
   if(!m_PCBDataList.Lookup(block->getName(),pcbData) || !pcbData)
      return FALSE;

	int intFidID = 0;
   for (POSITION dataPos = block->getHeadDataInsertPosition();dataPos;)
   {
      CPoint2d compCentroid;
      DataStruct* data = block->getNextDataInsert(dataPos);
      
      // if i used 'data != NULL' instead of !data, nothing is written.     
      CString refName ="";
      if(data && data->getInsert()->getInsertType() == insertTypeFiducial)
      {
         refName.Format("FID%d", ++intFidID);
         bool loaded = IsLoaded(data);
         if (loaded == true || m_Settings.getExportUNLoaded() == true)
         {      
            CString partNumber = getPartNumber(data);
            double x = getJukiUnits(data->getInsert()->getOriginX());
            double y = getJukiUnits(data->getInsert()->getOriginY());
            double rot = getJukiCentroidRotation(data);
           
            if (IsWritePart(data))
            {
               pcbData->getFiducialDataList().Add(data, x, y, rot, refName, partNumber, blockTypePcb);   
            }
         }
      }
      else if(data && data->getInsert()->getInsertType() == insertTypePcbComponent)
      {
         refName  = data->getInsert()->getRefname();
         bool loaded = IsLoaded(data);
         if (loaded == true || m_Settings.getExportUNLoaded() == true)
         {
            CPoint2d compLocation = data->getInsert()->getOrigin2d(); // Default location is insert point.
            // But centroid is preferred location.
            CPoint2d compCentroid;
			   if (data->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid))
               compLocation = compCentroid;

            CString partNumber = getPartNumber(data);
            bool isBottom = data->getInsert()->getGraphicMirrored();
            double xStr = applyJukiOriginX( getJukiUnits(compCentroid.x), isBottom );
            double yStr = getJukiUnits( compCentroid.y );
            double rot = getJukiCentroidRotation(data);
                     
            if (IsWritePart(data))
            {
               pcbData->getComponentDataList().Add(data, xStr, yStr, rot, refName, partNumber, blockTypePcb);   
               TraverseComponent(m_pDoc->getBlockAt(data->getInsert()->getBlockNumber()), l_boardMatrix, pcbData->getComponentDataList().getTail());
            }
         }//if
      }
   }//for

   return TRUE;
}

double JukiWrite::applyJukiOriginX(double jukiX, bool isBottom)
{
   double x = jukiX;

   if (isBottom)
   {
      x = m_lowerRightCornerX - jukiX;
   }

   return x;
}


bool JukiWrite::ProcessPanel()
{     
   FileStruct * fileStruct = m_pDoc->getFileList().GetOnlyShown(blockTypePanel);     
   if(!fileStruct)
      return FALSE;

   CTMatrix l_panelMatrix;    
   TranslateFile(l_panelMatrix, fileStruct);   
   BlockStruct* block = fileStruct->getBlock();   
   if(!block)
      return FALSE;

   //only one PCB data
   if(!block->getName().IsEmpty()) m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.setPanelFlag(true);
   m_PCBDataList.setPCBThickness(getBoardThickness(block));

   double OriginX = getJukiUnits(fileStruct->getInsertX());
   double OriginY = getJukiUnits(fileStruct->getInsertY());
   double Rot = normalizeDegrees(RadToDeg(fileStruct->getRotation()));
   CJukiPCBData *pcbData = new CJukiPCBData(OriginX, OriginY, Rot, fileStruct->isMirrored(), blockTypePanel,NULL,block);
   m_PCBDataList.SetAt(m_PCBDataList.getBoardName(),pcbData);

   ExtentRect pcbExtent;
   if(GetOutlineExtent(pcbData, &pcbExtent, GR_CLASS_PANELOUTLINE))
   {
      // This is correct for size, but seems not correct for what we want to do.
      //m_pcbSizeX = pcbExtent.right - pcbExtent.left;
      // Instead let's try just LR X coord instead of true size.
      m_lowerRightCornerX = pcbExtent.right; // Already in Juki units
   }
   else // if board outline does not exist, use board block
   {
      CExtent bkextent = block->getExtent();
      if (bkextent.isValid())
         m_lowerRightCornerX = getJukiUnits(bkextent.getXmax()); // Need to convert to Juki units
   }
  
   //build pcbdatalist with data in panel
   if(!TraversePanel(l_panelMatrix,block))
      return FALSE;
   
   m_rot_accuracy = 1;
   m_loc_accuracy = 3;

   if(!m_Settings.getFlattenOutput())
   {
      CTMatrix l_boardMatrix;

      //lookup stencil pcbblock
      LookupStencilBlock(m_targetPCBdata, l_boardMatrix);
       
      //write panel and pcb information
      WritePanelOffset();
      WriteFiducialOffset();
      WriteBadMark();
      WriteMatrixMethod();
      WriteBoardOffset();

      //rebuild pcbdatalist with data in stencil pcbblock
      ProcessStencilPCB(m_targetPCBdata, l_boardMatrix);
   }

 
   WriteComponentHeader();

   //write fiducials
   if(m_Settings.getFlattenOutput())
      WriteFiducialList(blockTypeUndefined,true);
   
   //write components
   WriteComponentList(m_targetPCBName);

   //write End of File
   if(!m_Settings.getFlattenOutput())
      WriteDataLine("\nEOF\n");  
   return TRUE;      
}

bool JukiWrite::TraversePanel(CTMatrix &l_panelMatrix, BlockStruct* block)
{   
   BlockStruct *targetBlock = NULL;  

   int errBoard = 0;
   int intFidID = 0;

   CString l_pcbName = "";
   CString partNumber = "";
	CString refName = "";
   CString panelName = m_PCBDataList.getBoardName();
   
   for (POSITION dataPos = block->getHeadDataInsertPosition();dataPos;)
   {
      CTMatrix l_boardMatrix(l_panelMatrix);
      
      DataStruct* data = block->getNextDataInsert(dataPos);
      // if i used 'data != NULL' instead of !data, nothing is written.
      InsertTypeTag insertType = (data && data->getInsert())?data->getInsert()->getInsertType():insertTypeUnknown;
      if (insertType == insertTypeFiducial || insertType == insertTypeXout)
		{
         CJukiPCBData *pcbData = NULL;
         if(m_PCBDataList.Lookup(panelName,pcbData) && pcbData)
         {
            double rot = normalizeDegrees(RadToDeg(data->getInsert()->getAngle()));

            bool isBottom = data->getInsert()->getGraphicMirrored();
            double xStr = applyJukiOriginX( getJukiUnits(data->getInsert()->getOriginX()), isBottom );
            double yStr = getJukiUnits(data->getInsert()->getOriginY());
            
            switch(insertType)
            {
            case insertTypeFiducial:
               partNumber = "FID"; 	
               refName.Format("FID%d", ++intFidID);
               pcbData->getFiducialDataList().Add(data,xStr,yStr,rot,refName,partNumber,blockTypePanel);      
               break;
            case insertTypeXout:
               partNumber =  getPartNumber(data); 	
               refName = data->getInsert()->getRefname();
               pcbData->getXOutDataList().Add(data,xStr,yStr,rot,refName,partNumber,blockTypePanel);   
               break;
            }//switch
         }
		}      
      else if(insertType == insertTypePcb)
      {		      
         l_pcbName = data->getInsert()->getRefname();

         TranslateBoard(l_panelMatrix, l_boardMatrix, data);             
         BlockStruct* pcb_Block = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber());
         if (!pcb_Block)
            continue;
         
         //evaluate multiple boards or mirror board in panel
         if(!m_Settings.getFlattenOutput())
         {
            if(!targetBlock) 
               targetBlock = pcb_Block;
            else if(targetBlock != pcb_Block)
               errBoard ++;

            if(data->getInsert()->getGraphicMirrored())
               errBoard ++;
         }

         double OriginX = getJukiUnits(data->getInsert()->getOriginX()) * ((data->getInsert()->getGraphicMirrored())?-1:1);
         double OriginY = getJukiUnits(data->getInsert()->getOriginY());
         double Rot = normalizeDegrees(data->getInsert()->getAngleDegrees());

         CJukiPCBData *pcbData = new CJukiPCBData(OriginX, OriginY, Rot, data->getInsert()->getGraphicMirrored(), blockTypePcb, data, pcb_Block);
         m_PCBDataList.SetAt(l_pcbName,pcbData);

         // If flattening then use the panel corners for component insert origin.
         // That is the value already set up for applyJukiOriginX(). If not flattening then
         // we want to use the PCB outline corners. Save current setting, reset to pcb outline, do
         // the processing, then set origin back to panel corner.
         double prevLowerRightCornerX = this->m_lowerRightCornerX;
         if (!m_Settings.getFlattenOutput())  // if not flattening
         {
            ExtentRect pcbExtent;
            if(GetOutlineExtent(pcbData, &pcbExtent, GR_CLASS_BOARDOUTLINE))
            {
               // This is correct for size, but seems not correct for what we want to do.
               //m_pcbSizeX = pcbExtent.right - pcbExtent.left;
               // Instead let's try just LR X coord instead of true size.
               m_lowerRightCornerX = pcbExtent.right;  // Already in Juki units
            }
            else // if board outline does not exist, use board block
            {
               CExtent bkextent = block->getExtent();
               if (bkextent.isValid())
                  m_lowerRightCornerX = getJukiUnits(bkextent.getXmax()); // Need to convert to Juki units
            }
         }
         
         // This is looking like a time waster for non-flattened output.
         // For flattened, this is what flattens the component inserts in pcb.
         // For non-flattened, this will end up discarded and the collection done over again.
         // See ProcessStencilPCB.

         for (POSITION dataPos = pcb_Block->getHeadDataInsertPosition();dataPos;)
         {            
            DataStruct* compdata = pcb_Block->getNextDataInsert(dataPos);
            // if i used 'data != NULL' instead of !data, nothing is written.
            if (!compdata && compdata->getInsert())
                  continue;
            
            refName  = compdata->getInsert()->getRefname();
			   if (compdata->getInsert()->getInsertType() == insertTypeFiducial )
            {
               refName.Format("FID%d", ++intFidID);

               CPoint2d insertPnt = compdata->getInsert()->getOrigin2d();

               if (m_Settings.getFlattenOutput()) // if flattened translate coords to panel origin
				      l_boardMatrix.transform(insertPnt);
   				
               bool isBottom = compdata->getInsert()->getGraphicMirrored();
               double xStr = applyJukiOriginX( getJukiUnits(insertPnt.x), isBottom );
               double yStr = getJukiUnits(insertPnt.y);
               double rot = normalizeDegrees(RadToDeg(compdata->getInsert()->getAngle()));
               partNumber = getPartNumber(compdata);  

               pcbData->getFiducialDataList().Add(compdata,xStr,yStr,rot,((m_Settings.getFlattenOutput())?l_pcbName + "_":"") + refName,partNumber,blockTypePcb); 

            }
			   else if (compdata->getInsert()->getInsertType() == insertTypePcbComponent)
			   {
               bool loaded = IsLoaded(compdata);
               if (loaded == true || m_Settings.getExportUNLoaded() == true)
               {
                  CPoint2d compLocation = compdata->getInsert()->getOrigin2d(); // Default location is insert point.
                  // But centroid is preferred location.
                  CPoint2d compCentroid;
                  if (compdata->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid))
                     compLocation = compCentroid;

                  if (m_Settings.getFlattenOutput()) // if flattened translate coords to panel origin
                     l_boardMatrix.transform(compLocation);

                  bool isBottom = compdata->getInsert()->getGraphicMirrored();
                  double xStr = applyJukiOriginX(getJukiUnits(compLocation.x), isBottom);
                  double yStr = getJukiUnits(compLocation.y);
                  double rot = normalizeDegrees(RadToDeg(compdata->getInsert()->getAngle()));
                  partNumber = getPartNumber(compdata); 

                  if (IsWritePart(compdata))
                  {
                     pcbData->getComponentDataList().Add(compdata,xStr,yStr,rot,((m_Settings.getFlattenOutput())?l_pcbName + "_" :"")+ refName,partNumber,blockTypePcb);     
                  }
               }
            }
         }

         // Reset the lower right corner to panel corner.
         m_lowerRightCornerX = prevLowerRightCornerX;
      }//if
   }   
   
   //Check error boards
   if(errBoard)
   {
      ErrorMessage("Juki does not support multiple boards or mirrored board in panel.", "Panel Board", MB_OK);
      return FALSE;
   }   

   //Check Fiducial Number
   if(!m_Settings.getFlattenOutput() && !VerifyFiducials(panelName,l_pcbName,2,3))
      return FALSE;

   return TRUE;      
}

bool JukiWrite::LookupStencilBlock(CJukiPCBData* &targetPCBdata, CTMatrix &l_boardMatrix)
{
   targetPCBdata = NULL;

   BlockStruct *targetBlock = NULL;
   for(POSITION pcbPos = m_PCBDataList.GetStartPosition();pcbPos;)
   {
      CString pcbName;
      CJukiPCBData *pcbData =NULL;
      
      m_PCBDataList.GetNextAssoc(pcbPos,pcbName,pcbData);
      if(pcbData && pcbData->getPlacedBoardType() == blockTypePcb)
      {
         targetBlock = pcbData->getGeometryBlock();
         break;
      }
   }

   if(!targetBlock) return FALSE;

   FileStruct * fileStruct = m_pDoc->getFileList().FindByBlockNumber(targetBlock->getBlockNumber());
   if(fileStruct)
   {
      double OriginX = getJukiUnits(fileStruct->getInsertX());
      double OriginY = getJukiUnits(fileStruct->getInsertY());
      double Rot = normalizeDegrees(RadToDeg(fileStruct->getRotation()));

      targetPCBdata = new CJukiPCBData(OriginX, OriginY, Rot, fileStruct->isMirrored(), blockTypePanel,NULL,fileStruct->getBlock());
      l_boardMatrix = fileStruct->getTMatrix();

      return TRUE;
   }   

   return FALSE;
}

bool JukiWrite::ProcessStencilPCB(CJukiPCBData* &targetPCBdata, CTMatrix &l_boardMatrix)
{
   if(!targetPCBdata || !targetPCBdata->getGeometryBlock()) return FALSE;

   //reset PCBData list as StencilPCB
   m_PCBDataList.empty();
   BlockStruct *block = targetPCBdata->getGeometryBlock();
   m_PCBDataList.setPanelFlag(false);

   m_PCBDataList.setBoardName(block->getName());
   m_PCBDataList.SetAt(m_PCBDataList.getBoardName(),targetPCBdata);
   m_targetPCBName = m_PCBDataList.getBoardName();

   // If flattening then use the panel corners for component insert origin.
   // That is the value already set up for applyJukiOriginX(). If not flattening then
   // we want to use the PCB outline corners. Save current setting, reset to pcb outline, do
   // the processing, then set origin back to panel corner.
   double prevLowerRightCornerX = this->m_lowerRightCornerX;
   if (!m_Settings.getFlattenOutput())  // if not flattening
   {
      ExtentRect pcbExtent;
      if(GetOutlineExtent(targetPCBdata, &pcbExtent, GR_CLASS_BOARDOUTLINE))
      {
         // This is correct for size, but seems not correct for what we want to do.
         //m_pcbSizeX = pcbExtent.right - pcbExtent.left;
         // Instead let's try just LR X coord instead of true size.
         m_lowerRightCornerX = pcbExtent.right;  // Already in Juki units
      }
      else // if board outline does not exist, use board block
      {
         CExtent bkextent = block->getExtent();
         if (bkextent.isValid())
            m_lowerRightCornerX = getJukiUnits(bkextent.getXmax());  // Need to convert to Juki units
      }
   }
   
   TraversePCB(block, l_boardMatrix);

   // Now reset corner
   m_lowerRightCornerX = prevLowerRightCornerX;

   return TRUE;
}

bool JukiWrite::VerifyFiducials(CString panelName, CString targetPCBName, int MinFidSize, int MaxFidSize)
{
   CJukiPCBData *pcbData = NULL;
   if(panelName.IsEmpty() || !m_PCBDataList.Lookup(panelName, pcbData) || !pcbData) 
      return TRUE;

   CJukiPCBData *stencilPCBData = NULL;
   m_PCBDataList.Lookup(targetPCBName, stencilPCBData);

   for(int isBottom = 0; isBottom < 2; isBottom ++)
   {
      int fidCnt = pcbData->getFiducialDataList().getInsertCount(isBottom);
      int compCnt = (stencilPCBData)? stencilPCBData->getComponentDataList().getInsertCount(isBottom):0;

      if( fidCnt < MinFidSize && compCnt)
      {
         if(formatMessageBox(MB_ICONQUESTION | MB_YESNO,
            "Found %d Fiducial on %s when %d or %d Fiducials are required.\n Do you want to continue?",
               fidCnt, (isBottom)?"Bottom":"Top", MinFidSize, MaxFidSize) == IDNO)
            return FALSE;
      }
   }

   return TRUE;
}
////////////////////////////////////////////////////////////
// JukiOutputAttrib
////////////////////////////////////////////////////////////

JukiOutputAttrib::JukiOutputAttrib(CCEtoODBDoc& camCadDoc) 
:  m_camCadDatabase(camCadDoc)
,  m_attribKeyword(-1)
{
	SetDefaultAttribAndValue();
}

JukiOutputAttrib::~JukiOutputAttrib()
{
	m_valuesMap.RemoveAll();
}

int JukiOutputAttrib::GetKeyword() const
{
	return m_attribKeyword;
}

bool JukiOutputAttrib::HasValue(CAttributes** attributes)
{
	CString value, tmpValue;
	m_camCadDatabase.getAttributeStringValue(value, attributes, m_attribKeyword);
	value.MakeLower();
	return (m_valuesMap.Lookup(value, tmpValue)==TRUE)?true:false;
}

//void JukiOutputAttrib::SetKeyword(const CString keyword)
void JukiOutputAttrib::SetKeyword(const CString keyword)
{
	m_attribKeyword = m_camCadDatabase.getKeywordIndex(keyword);
	m_valuesMap.RemoveAll();
}

void JukiOutputAttrib::AddValue(const CString value)
{
	CString tmpValue = value;
	m_valuesMap.SetAt(tmpValue.MakeLower(), tmpValue.MakeLower());
}

void JukiOutputAttrib::SetDefaultAttribAndValue()
{
   //NO COMMANDS by default for Juki
   //Commands are in the OUT file, or there are no commands at all.
   //No second guessing user by supplying "hidden" default commands.
	//SetKeyword(ATT_TECHNOLOGY);
	//AddValue("SMD");
   //AddValue("THRU");
}

////////////////////////////////////////////////////////////
// CJukiInsertData
////////////////////////////////////////////////////////////
CJukiInsertData::CJukiInsertData()
{
   m_posX = 0.;
   m_posY = 0.;
   m_rotation = 0.;
   m_InsertData = NULL;
   m_placedBoardType = blockTypeUndefined;
   m_CompFidDataList = NULL;

   m_refName.Empty();
   m_PartNumber.Empty();
}

CJukiInsertData::~CJukiInsertData()
{
   if(m_CompFidDataList)
   {
      delete m_CompFidDataList;
      m_CompFidDataList = NULL;
   }
}

CTypedPtrArrayContainer<CJukiInsertData*>* CJukiInsertData::getCompFidDataList()
{
   if(!m_CompFidDataList)
   {
      m_CompFidDataList = new CJukiInsertDataList();
   }
   return m_CompFidDataList;
}

////////////////////////////////////////////////////////////
// CJukiInsertDataList
////////////////////////////////////////////////////////////
CJukiInsertDataList::CJukiInsertDataList()
{
   m_topFids = 0;
   m_botFids = 0;
}

CJukiInsertDataList::~CJukiInsertDataList()
{
   empty();
}

void CJukiInsertDataList::Add(DataStruct* data, double posX, double posY, double rotation, CString refName, 
                              CString partNumber, BlockTypeTag boardtype)
{
   CJukiInsertData *insertdata = new CJukiInsertData();
   insertdata->setInsertData(data);
   insertdata->setPartNumber(partNumber);
   insertdata->setRefName(refName);
   insertdata->setPlacedBoardType(boardtype);
   insertdata->setX(posX);
   insertdata->setY(posY);
   insertdata->setRotation(rotation);
   
   if(data && data->getInsert())
   {
      if(data->getInsert()->getPlacedBottom())
         m_botFids ++;
      else 
         m_topFids ++;
   }

   SetAtGrow(GetCount(), insertdata);
}

CJukiInsertData * CJukiInsertDataList::getTail()
{
   int tailPos = GetCount() - 1;
   return GetAt(tailPos);
}

////////////////////////////////////////////////////////////
// CJukiPCBData
////////////////////////////////////////////////////////////
CJukiPCBData::CJukiPCBData(double x, double y, double rot, int mirror, BlockTypeTag boardType, DataStruct *data, BlockStruct *geometry)
: m_originX(x)
, m_originY(y)
, m_rotation(rot)
, m_mirror(mirror)
, m_boardType(boardType)
, m_insertData(data)
, m_geometryBlock(geometry)
{
   m_FidDataList.RemoveAll();
   m_CompDataList.RemoveAll();

}

CJukiPCBData::~CJukiPCBData()
{
   m_FidDataList.empty();
   m_CompDataList.empty();
}

////////////////////////////////////////////////////////////
// CJukiPCBDataList
////////////////////////////////////////////////////////////
CJukiPCBDataList::CJukiPCBDataList()
{
   m_Panelflag = false;
   m_BoardName = "Board";
}

CJukiPCBDataList::~CJukiPCBDataList()
{
  empty();
}

void CJukiPCBDataList::empty()
{
   m_Panelflag = false;
   m_BoardName = "Board";

  for(POSITION pcbPos = GetStartPosition(); pcbPos; )
  {
     CString pcbName;
     CJukiPCBData *pcbData =NULL;
     GetNextAssoc(pcbPos,pcbName,pcbData);
     if(pcbData)
        delete pcbData;
  }
  RemoveAll();
}
