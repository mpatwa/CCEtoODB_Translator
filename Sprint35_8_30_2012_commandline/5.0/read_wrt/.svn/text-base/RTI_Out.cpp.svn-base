// $Header: /CAMCAD/5.0/read_wrt/RTI_Out.cpp 14    6/17/07 8:59p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// RTI_OUT.CPP

#include "StdAfx.h"
#include "ccdoc.h"
#include "RTI_Out.h"
#include "PcbUtil.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CString createOutfileName(CString basename, CString filenameSuffix);

void RTI_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   CString topFileName = createOutfileName(filename, "_top");
   CString botFileName = createOutfileName(filename, "_bot");
   
   RTIWrite writer(&doc, format->Scale);

   if (!writer.Write(topFileName, botFileName))
   {
      ErrorMessage("CAMCAD could not finish writing the RTI CR Technology RTI files.", "", MB_OK);
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

//---------------------------------------------------------------------------

/*
Constructor
*/
RTIWrite::RTIWrite(CCEtoODBDoc *document, double l_Scale)
{
   m_pDoc = document;
   p_Scale = l_Scale;
}

/*
RTIWrite::Write
*/
BOOL RTIWrite::Write(CString topFileName, CString botFileName)
{
   if (!topFile.Open(topFileName, CFile::modeCreate|CFile::modeWrite))
   {
		CString msg;
		msg.Format("File \"%s\" could not be opened for writing.", topFileName);
      ErrorMessage(msg, "", MB_OK);
      return FALSE;
   }

   if (!botFile.Open(botFileName, CFile::modeCreate|CFile::modeWrite))
   {
		topFile.Close();

		CString msg;
		msg.Format("File \"%s\" could not be opened for writing.", botFileName);
      ErrorMessage(msg, "", MB_OK);
      return FALSE;
   }
   
   // generate centroid in order to get centroid of the insert.
   m_pDoc->OnGenerateComponentCentroid();

   //read settings file data
   CString settingsFilePath( getApp().getExportSettingsFilePath("crtech.out") );
   LoadSettings(settingsFilePath);
   
   CString s_conversion;
   FileStruct * lFile = m_pDoc->getFileList().GetFirstShown(blockTypePcb);

   if (lFile == NULL)
   {
      //ErrorMessage("No PCB file is shown", "", MB_OK);
      RTIWrite::ProcessPanel();
      return TRUE;
   }

   CString lFileName = lFile->getName();

   topFile.WriteString (".6500_CAD_EXCHANGE\n");
   topFile.WriteString("Version    2\n");
   topFile.WriteString("BoardName  "  + lFileName + "\n" );

   topFile.WriteString ("Scale    ");

   // get the scale settings
   if (o_Units == unit_r_Mils)
      s_conversion = "25400";
   else
      s_conversion = "25.4";
   topFile.WriteString (s_conversion + "\n");

   topFile.WriteString("InvertXPos    0\n");
   topFile.WriteString("InvertYPos    0\n");
   topFile.WriteString("BoardRtn      0    1\n");
   topFile.WriteString(".DATA_START\n");
   topFile.WriteString ("Ref.ID   P/N      X       Y      Width   Height  Rot'n  Cam#  Package\n");
   
   botFile.WriteString (".6500_CAD_EXCHANGE\n");
   botFile.WriteString("Version    2\n");
   botFile.WriteString("BoardName  " + lFileName + "\n");
   botFile.WriteString ("Scale    ");

   // get the scale settings
   if (o_Units == unit_r_Mils)
      s_conversion = "25400";
   else
      s_conversion = "25.4";
   botFile.WriteString (s_conversion + "\n");

   botFile.WriteString("InvertXPos    0\n");
   botFile.WriteString("InvertYPos    0\n");
   botFile.WriteString("BoardRtn      0    1\n");
   botFile.WriteString(".DATA_START\n");
   botFile.WriteString ("Ref.ID   P/N      X       Y      Width   Height  Rot'n  Cam#  Package\n");   

   //for (int i=0; i< m_pDoc->getMaxBlockIndex(); i++)
   //{

      BlockStruct* block = lFile->getBlock();
      //BlockStruct* block = m_pDoc->getBlockAt(i);
      //if (!block)
      // continue;
      POSITION dataPos = block->getHeadDataInsertPosition();
      while (dataPos)
      {
         CPoint2d compCentroid;
         DataStruct* data = block->getNextDataInsert(dataPos);
         // if i used 'data != NULL' instead of !data, nothing is written.
         if (data == NULL || data->getInsert()->getInsertType() != insertTypePcbComponent)
            continue;
         
         CString refName  = data->getInsert()->getRefname();
         CString geomName = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())->getName():"";
         CString xStr = "";
         CString yStr =  "";
         CString rot = "";
         CString width = "0";
         CString height = "0";
         CString Cam = "0";
         CString partNumber = "";

         Attrib *attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
         partNumber = attrib?attrib->getStringValue():"No_Part_Number";
         
         data->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid);

         xStr.Format("%.3f", compCentroid.x * m_dUnitFactor);
         yStr.Format("%.3f", compCentroid.y * m_dUnitFactor);
         
         rot.Format("%.1f", RadToDeg(data->getInsert()->getAngle()));

         if (data->getInsert()->getGraphicMirrored() == true)
            botFile.WriteString("%s     %s     %s     %s     %s     %s     %s     %s     %s\n", refName, partNumber, xStr, yStr, width, height, rot, Cam , geomName);
         else
            topFile.WriteString("%s     %s     %s     %s     %s     %s     %s     %s     %s\n", refName, partNumber, xStr, yStr, width, height, rot, Cam , geomName);
      }
   //}

   botFile.WriteString(".DATA_END\n");
   topFile.WriteString(".DATA_END\n");

   return TRUE;
}

int RTIWrite::LoadSettings(CString fileName)
{
   m_dUnitFactor = 1;
   FILE *fp = fopen(fileName,  "rt");
   if ((fp ) == NULL)
   {
      CString tmp;
      tmp.Format("File [%s] is not found", fileName);
      MessageBox(NULL, tmp, "CR Technology RTI Write Settings", MB_OK | MB_ICONHAND);
      return 0;
   }

   
   o_Units = DEFAULT_RTI_UNITS;
              
   char line[255];
   while (fgets(line, 255, fp))
   {
      CString w = line;
      char *lp = get_string(line, " \t\n");
      if (lp == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".UNITS"))
         {
            lp = get_string(NULL, " \t\n");
         if (lp == NULL)
            continue;

         if (!STRICMP(lp, "MILS"))
            o_Units = unit_r_Mils;
         else if (!STRICMP(lp, "INCHES"))
            o_Units = unit_r_Inches;
         }
     }
   }


   if (o_Units == unit_r_Mils)
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_MILS) * p_Scale;
   else
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_INCHES) * p_Scale;

   fclose(fp);
   return 1;
}

int RTIWrite::ProcessPanel()
{   
   CString l_pcbName;
   CString s_conversion;
   FileStruct * lFile = m_pDoc->getFileList().GetOnlyShown(blockTypePanel);
   CTMatrix l_panelMatrix;  
   
   if (lFile->getResultantMirror(False))
      l_panelMatrix.scale(-1 * lFile->getScale(), lFile->getScale());
   else
      l_panelMatrix.scale(lFile->getScale(), lFile->getScale());

   l_panelMatrix.rotateRadians(lFile->getRotation());

   l_panelMatrix.translate(lFile->getInsertX(), lFile->getInsertY());   

   BlockStruct* block = lFile->getBlock();
   //BlockStruct* block = m_pDoc->getBlockAt(i);
   //if (!block)
   // continue;
   CString lFileName = lFile->getName();

   topFile.WriteString (".6500_CAD_EXCHANGE\n");
   topFile.WriteString("Version    2\n");
   topFile.WriteString("BoardName  "  + lFileName + "\n" );

   topFile.WriteString ("Scale    ");

   // get the scale settings
   if (o_Units == unit_r_Mils)
      s_conversion = "25400";
   else
      s_conversion = "25.4";
   topFile.WriteString (s_conversion + "\n");

   topFile.WriteString("InvertXPos    0\n");
   topFile.WriteString("InvertYPos    0\n");
   topFile.WriteString("BoardRtn      0    1\n");
   topFile.WriteString(".DATA_START\n");
   topFile.WriteString ("Ref.ID   P/N      X       Y      Width   Height  Rot'n  Cam#  Package\n");
   
   botFile.WriteString (".6500_CAD_EXCHANGE\n");
   botFile.WriteString("Version    2\n");
   botFile.WriteString("BoardName  " + lFileName + "\n");
   botFile.WriteString ("Scale    ");

   // get the scale settings
   if (o_Units == unit_r_Mils)
      s_conversion = "25400";
   else
      s_conversion = "25.4";
   botFile.WriteString (s_conversion + "\n");

   botFile.WriteString("InvertXPos    0\n");
   botFile.WriteString("InvertYPos    0\n");
   botFile.WriteString("BoardRtn      0    1\n");
   botFile.WriteString(".DATA_START\n");
   botFile.WriteString ("Ref.ID   P/N      X       Y      Width   Height  Rot'n  Cam#  Package\n");

   POSITION dataPos = block->getHeadDataInsertPosition();
   while (dataPos)
   {
      CTMatrix l_boardMatrix(l_panelMatrix);
      
      DataStruct* data = block->getNextDataInsert(dataPos);
      // if i used 'data != NULL' instead of !data, nothing is written.
      if (data->getInsert()->getInsertType() != insertTypePcb)
      {
         continue;
      }  
      l_pcbName = data->getInsert()->getRefname();

      CPoint2d boardXY = data->getInsert()->getOrigin2d();
      l_panelMatrix.transform(boardXY);

      if (data->getInsert()->getGraphicMirrored())
         l_boardMatrix.scale(-1 * data->getInsert()->getScale(), data->getInsert()->getScale());
      else
         l_boardMatrix.scale(data->getInsert()->getScale(), data->getInsert()->getScale());

      l_boardMatrix.rotateRadians(data->getInsert()->getAngle());

      l_boardMatrix.translate(boardXY);

      

      BlockStruct* pcb_Block = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber());
      if (!pcb_Block)
         continue;
      POSITION dataPos = pcb_Block->getHeadDataInsertPosition();
      while (dataPos)
      {
         CPoint2d compCentroid;
         
         DataStruct* data = pcb_Block->getNextDataInsert(dataPos);
         // if i used 'data != NULL' instead of !data, nothing is written.
         if (data->getInsert()->getInsertType() != insertTypePcbComponent)
            continue;
         CPoint2d compXY = data->getInsert()->getOrigin2d();
         

         CTMatrix l_compMatrix;
         CString refName  = data->getInsert()->getRefname();
         CString geomName = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())->getName():"";
         CString xStr = "";
         CString yStr =  "";
         CString rot = "";
         CString width = "0";
         CString height = "0";
         CString Cam = "0";
         CString partNumber = "";

         Attrib *attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
         partNumber = attrib?attrib->getStringValue():"No_Part_Number";
   
         data->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid);
         l_boardMatrix.transform(compCentroid);
         
         xStr.Format("%.3f", (compCentroid.x) * m_dUnitFactor);
         yStr.Format("%.3f", (compCentroid.y) * m_dUnitFactor);
         
         rot.Format("%.1f", RadToDeg(data->getInsert()->getAngle()));

         if (data->getInsert()->getGraphicMirrored() == true)
            botFile.WriteString("%s     %s     %s     %s     %s     %s     %s     %s     %s\n", l_pcbName + "_" + refName, partNumber, xStr, yStr, width, height, rot, Cam , geomName);
         else
            topFile.WriteString("%s     %s     %s     %s     %s     %s     %s     %s     %s\n", l_pcbName + "_" + refName, partNumber, xStr, yStr, width, height, rot, Cam , geomName);
      }
   }

   botFile.WriteString(".DATA_END\n");
   topFile.WriteString(".DATA_END\n");
   
   return TRUE;
}



