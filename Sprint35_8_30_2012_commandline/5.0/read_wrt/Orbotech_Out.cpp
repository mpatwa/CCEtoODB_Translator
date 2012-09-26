// $Header: /CAMCAD/5.0/read_wrt/Orbotech_Out.cpp 33    6/17/07 8:59p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "ccdoc.h"
#include "Orbotech_Out.h"
#include "PcbUtil.h"
#include "CCEtoODB.h"
#include "xform.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CString createOutfileName(CString basename, CString filenameSuffix);

void Orbotech_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{
   CString topFileName = createOutfileName(filename, "_top");
   CString botFileName = createOutfileName(filename, "_bot");
   
   OrbotechWrite writer(&doc, format->Scale);

   if (!writer.Write(topFileName, botFileName))
   {
      ErrorMessage("CAMCAD could not finish writing the Orbotech files.", "", MB_OK);
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
OrbotechWrite::OrbotechWrite(CCEtoODBDoc *document, double l_Scale)
{
   m_pDoc = document;
   p_Scale = l_Scale;
}

/*
OrbotechWrite::Write
*/
BOOL OrbotechWrite::Write(CString topFileName, CString botFileName)
{

	intFidID = 0;
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
   CString headerStr( "*RefDes\tCX\tCY\tRotation\tPartnumber\tPackage\tHland\tVland\tTech\tLoaded\n" );
   topFile.WriteString (headerStr);
   botFile.WriteString (headerStr);

 

   LoadSettings(getApp().getExportSettingsFilePath("Orbotech.out"));

   FileStruct * lFile = m_pDoc->getFileList().GetFirstShown(blockTypePcb);

   if (lFile == NULL)
   {
      OrbotechWrite::ProcessPanel();
      return TRUE;
   }

   CString lFileName = lFile->getName();
   BlockStruct* block = lFile->getBlock();
   DTransform xForm(lFile->getInsertX(), lFile->getInsertX(), lFile->getScale(), lFile->getRotation(), lFile->isMirrored());

   POSITION dataPos = block->getHeadDataInsertPosition();

   CString TopCompPin;
   CString BotCompPin;
   CString TopThruComp, BotThruComp;

   while (dataPos)
   {
      CPoint2d compCentroid;
      DataStruct* data = block->getNextDataInsert(dataPos);
      // if i used 'data != NULL' instead of !data, nothing is written.
      if (data == NULL || data->getInsert()->getInsertType() != insertTypePcbComponent && data->getInsert()->getInsertType() != insertTypeFiducial) 
         continue;
      
      Attrib *attrib;
		CString refName  = data->getInsert()->getRefname();
      CString tech;   
      
      if (/*refName.IsEmpty() && */data->getInsert()->getInsertType() == insertTypeFiducial )
      {
         CString fidID = "";
         intFidID++;
         fidID.Format("%d", intFidID);
         refName = "FID" + fidID;
         tech = "SMD";
      }
      // case 1580 required using package name from the .out file
      CString geomName;   
      if (!m_PackageName.IsEmpty())
      {
         attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_PackageName, 0);
         geomName = ((attrib != NULL) ? attrib->getStringValue() : "");
      }
      if (geomName == "")
         geomName = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())->getName():"";

      CString pkgValue;
      WORD pkgKW = m_pDoc->RegisterKeyWord("PACKAGE", 0, VT_STRING);
      
      if ( data->getAttributes() != NULL && data->getAttributes()->Lookup(pkgKW, attrib) )
         geomName = attrib->getStringValue();
            
      // end of changes for case 1580 -- 8/9/05 MAN

		///changes for case 1698 10/19/2005
		CString loaded = "TRUE";
		if (attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_LOADED, 1))
		{
			CString value = attrib->getStringValue();
         if (value.CompareNoCase("false") == 0)
				loaded = "FALSE";
		}  
     
		if (loaded == "FALSE" && exportUNLoaded == "N")
			continue;

      // Determine if technology is THRU
      bool isThruPart = false;
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(),ATT_TECHNOLOGY , 0);
      if (attrib != NULL)
      {
         CString technology = attrib->getStringValue();
         isThruPart = (technology.CompareNoCase("THRU") == 0);
      }

      // Skip THRU parts altogether if neither the part nor the pins shall be output
      if (isThruPart && !exportThru && !exportThroughHolePins)
      {
         continue;
      }

      if (data->getInsert()->getInsertType() != insertTypeFiducial)
         tech = isThruPart?"THRU":"SMD";

      CString xStr;
      CString yStr;
      CString rot;
      CString pinrot;
      CString partNumber;

      attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
      partNumber = ((attrib != NULL) ? attrib->getStringValue() : "No_Part_Number");
      
      if ((data->getInsert()->getInsertType() == insertTypeFiducial) && partNumber == "No_Part_Number")
            partNumber = "FID";

		if (data->getInsert()->getInsertType() != insertTypeFiducial)
		{
			data->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid);
			xStr.Format("%.2f", compCentroid.x * m_dUnitFactor);
			yStr.Format("%.2f", compCentroid.y * m_dUnitFactor);
		}

		else
		{
			float x = data->getInsert()->getOriginX();
			float y = data->getInsert()->getOriginY();
			xStr.Format("%.2f", x * m_dUnitFactor);
			yStr.Format("%.2f", y * m_dUnitFactor);
		}

		CBasesVector * m_pcbBasesVector = &data->getInsert()->getBasesVector();
   
		CBasesVector cmpBV(data->getInsert()->getBasesVector());
		if (m_pcbBasesVector != NULL)				
			cmpBV.transform(m_pcbBasesVector->getTransformationMatrix());

		CTMatrix compMatrix = cmpBV.getTransformationMatrix();

		int insertedBlockNum = data->getInsert()->getBlockNumber();
		BlockStruct *insertedBlock = m_pDoc->getBlockAt(insertedBlockNum);
		POSITION PinDataPos = insertedBlock->getDataList().GetHeadPosition();
		double hLand = 0;
		double vLand = 0;

      CExtent packageOutlineExtent;
      packageOutlineExtent.transform(compMatrix);
     
		while (PinDataPos)
		{
			DataStruct *pindata = insertedBlock->getDataList().GetNext(PinDataPos);
         CString pinDef;
         
         if (pindata != NULL && pindata->isInsertType(insertTypePin))
			{
				CBasesVector pinBV(pindata->getInsert()->getBasesVector());
				pinBV.transform(compMatrix);
				int pinBlockNum = pindata->getInsert()->getBlockNumber();
				BlockStruct *pinBlock = m_pDoc->getBlockAt(pinBlockNum);
           
				if (pinBlock != NULL)
				{
					CExtent *pinExtent = &pinBlock->getExtent();
					double padSizeX = pinExtent->getXsize();
					double padSizeY = pinExtent->getYsize();

               if (hLand < padSizeX)
						hLand = padSizeX;
					if (vLand < padSizeY)
						vLand = padSizeY;

               if(isThruPart == true && exportThroughHolePins == true)
               {
                  CString pinName;
                  CString padHsize("1.00");//Default size to ensure not blank 
                  CString padVsize("1.00");//Default size to ensure not blank
                  CString padName;
                  CString pinpackage;
                  if (!m_PackageName.IsEmpty())
                  {
                     attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_PackageName, 0);
                     padName = ((attrib != NULL) ? attrib->getStringValue() : "");
                  }
                  if (padName.Compare("") == 0)
                     padName = m_pDoc->getBlockAt(pindata->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(pindata->getInsert()->getBlockNumber())->getName():"";
                  
                  pinName.Format("%s_%s", refName, pindata->getInsert()->getRefname());
                  padSizeX<padSizeY?pinpackage.Format ("%.2f", padSizeX * m_dUnitFactor):pinpackage.Format ("%.2f", padSizeY * m_dUnitFactor);
                  pinrot.Format("%.2f", RadToDeg(pindata->getInsert()->getAngle()));

                  Point2 point2(data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
                  xForm.TransformPoint(&point2);
                  DTransform pinXForm(point2.x, point2.y,
                     xForm.scale * data->getInsert()->getScale(),
                     xForm.rotation + ((xForm.mirror & MIRROR_FLIP)?-data->getInsert()->getAngle():data->getInsert()->getAngle()),
                     xForm.mirror ^ data->getInsert()->getMirrorFlags());
                  Point2 pinPoint(pindata->getInsert()->getOriginX(), pindata->getInsert()->getOriginY());
                  pinXForm.TransformPoint(&pinPoint);

                  CString pinStrX;
                  CString pinStrY;
                  pinStrX.Format("%.2f", pinPoint.x);
                  pinStrY.Format("%.2f", pinPoint.y);

                  int pinBlockNum = pindata->getInsert()->getBlockNumber();
                  
                  BlockStruct *pinBlock = m_pDoc->getBlockAt(pinBlockNum);
                  if(pinBlock !=NULL)
                  {
                     if (!pinBlock->getExtent().isValid())
                        pinBlock->calculateBlockExtents(m_pDoc->getCamCadData());

                     CExtent *pinExtent = &pinBlock->getExtent();
                     double pinpadX = pinExtent->getXsize();
                     double pinpadY = pinExtent->getYsize();
                     
                     padHsize.Format("%.2f", pinpadX > 0.0 ? (pinpadX * m_dUnitFactor) : 1.0);
                     padVsize.Format("%.2f", pinpadY > 0.0 ? (pinpadY * m_dUnitFactor) : 1.0);
                  }
                                    
                  pinDef.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", pinName, pinStrX, pinStrY, pinrot, partNumber, pinpackage, padHsize, padVsize, tech, loaded);

                  // Write THRU pins to side opposite of component mount side
                  // dts0100481723
                  if (data->getInsert()->getGraphicMirrored() == true)
                  {
                     TopCompPin.Append(pinDef);
                  }
                  else
                  {
                     BotCompPin.Append(pinDef);
                  }
               }
					
				}
				
			}
		}

      CString sVLand; CString sHLand;
		
		sHLand.Format ("%.2f", hLand * m_dUnitFactor);
		sVLand.Format ("%.2f", vLand * m_dUnitFactor);
		if (data->getInsert()->getInsertType() == insertTypeFiducial)
		{
			if (o_Units == unit_o_Mils)
			{
				sHLand = "40";
				sVLand = "40";
			}
			else
			{
				sHLand = "1.0";
				sVLand = "1.0";
			}
		}
		vLand = 0; hLand = 0;  //reset for the next comp	
     
      
      rot.Format("%.2f", RadToDeg(data->getInsert()->getAngle()));
      if (!isThruPart || exportThru)
      {
         if (data->getInsert()->getGraphicMirrored() == true)
         {
            if(tech == "SMD")
               botFile.WriteString("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", refName, xStr, yStr, rot, partNumber, geomName, sHLand , sVLand, tech, loaded);
            else
            {
               //First write SMD then write THRU
               CString tmpStr;
               tmpStr.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", refName, xStr, yStr, rot, partNumber, geomName, sHLand , sVLand, tech, loaded);
               BotThruComp.Append(tmpStr);
            }
         }
         else
         {
            if(tech == "SMD")
               topFile.WriteString("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", refName, xStr, yStr, rot, partNumber, geomName, sHLand, sVLand,  tech, loaded);
            else
            {
               //First write SMD then write THRU
               CString tmpStr;
               tmpStr.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", refName, xStr, yStr, rot, partNumber, geomName, sHLand , sVLand, tech, loaded);
               TopThruComp.Append(tmpStr);
            }
         }
      }
   }
   //First write SMD then write THRU
   botFile.WriteString("%s", BotThruComp);
   topFile.WriteString("%s", TopThruComp);

   if(exportThroughHolePins)
   {
      botFile.WriteString("%s", BotCompPin);
      topFile.WriteString("%s", TopCompPin);
   }
   return TRUE;
}

int OrbotechWrite::LoadSettings(CString fileName)
{
	exportUNLoaded = "";  
	exportThru = true;
   m_dUnitFactor = 1;
   exportThroughHolePins = true;
   FILE *fp = fopen(fileName,  "rt");
   if ((fp ) == NULL)
   {
      CString tmp;
      tmp.Format("File [%s] not found", fileName);
      ErrorMessage( tmp, "Orbotech Write Settings", MB_OK | MB_ICONHAND);
      return 0;
   }

   
   o_Units = DEFAULT_ORBOTECH_UNITS;
              
   char line[255];
   while (fgets(line, 255, fp))
   {
      CString w = line;
      char *lp = get_string(line, " \t\n");
      if (lp == NULL)
         continue;

      if (lp[0] == '.')
      {
         
			 
			if (!STRICMP(lp, ".EXPORT_THRU"))
			{
				lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;
            exportThru = ((toupper(lp[0]) == 'Y') || (toupper(lp[0]) == 'T'));
			}
			if (!STRICMP(lp, ".EXPORT_UNLOADED_PARTS"))
			{
				lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

				if (!STRICMP(lp, "N"))
					exportUNLoaded = "N";
			}
			if (!STRICMP(lp, ".UNITS"))
         {
            lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;

				if (!STRICMP(lp, "MILS"))
					o_Units = unit_o_Mils;
				else if (!STRICMP(lp, "MM"))
					o_Units = unit_o_MM;
         }
         if (!STRICMP(lp, ".PACKAGE_NAME"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp == NULL)
               continue;

            m_PackageName = lp;
         }
         if (!STRICMP(lp, ".EXPORT_THROUGH_HOLE_PINS"))
         {
				lp = get_string(NULL, " \t\n");
				if (lp == NULL)
					continue;
            exportThroughHolePins = ((toupper(lp[0]) == 'Y') || (toupper(lp[0]) == 'T'));
         }
      }

   }


   if (o_Units == unit_o_Mils)
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_MILS) * p_Scale;
   else
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_MM) * p_Scale;

   fclose(fp);
   return 1;
}

int OrbotechWrite::ProcessPanel()
{  
   intFidID = 0;
   CString l_pcbName;
   CString s_conversion;
	CString xStr = "";
   CString yStr =  "";
   CString rot = "";
   double hLand;
	double vLand;
   CString partNumber = "";
	CString sVLand; CString sHLand;
	CPoint2d compCentroid;
   Attrib *attrib;
	CString geomName = "";
	CString refName = "";
	CString fidID = "";
   FileStruct * lFile = m_pDoc->getFileList().GetOnlyShown(blockTypePanel);
   CTMatrix l_panelMatrix;    
   
   if (lFile->getResultantMirror(False))
      l_panelMatrix.scale(-1 * lFile->getScale(), lFile->getScale());
   else
      l_panelMatrix.scale(lFile->getScale(), lFile->getScale());

   l_panelMatrix.rotateRadians(lFile->getRotation());

   l_panelMatrix.translate(lFile->getInsertX(), lFile->getInsertY());
   

   BlockStruct* block = lFile->getBlock();
   
   CString lFileName = lFile->getName();
   
   POSITION dataPos = block->getHeadDataInsertPosition();
   while (dataPos)
   {
      CTMatrix l_boardMatrix(l_panelMatrix);
      
      DataStruct* data = block->getNextDataInsert(dataPos);
      // if i used 'data != NULL' instead of !data, nothing is written.
      if (data->getInsert()->getInsertType() == insertTypeFiducial)
		{
			
			if (o_Units == unit_o_Mils)
			{
				sHLand = "40";
				sVLand = "40";
			}
			else
			{
				sHLand = "1.0";
				sVLand = "1.0";
			}
			rot.Format("%.1f", normalizeDegrees(RadToDeg(data->getInsert()->getAngle())));
			
			data->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid);
         l_boardMatrix.transform(compCentroid);
         
         if (data->getInsert()->getGraphicMirrored() == false)
				xStr.Format("%.3f", data->getInsert()->getOriginX() * m_dUnitFactor);
			else
				xStr.Format("%.3f", (data->getInsert()->getOriginX()) * (-1) * m_dUnitFactor);
         yStr.Format("%.3f", data->getInsert()->getOriginY() * m_dUnitFactor);
         partNumber = "FID";
			geomName = "";
			if (!m_PackageName.IsEmpty())
         {
            attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_PackageName, 0);
            geomName = ((attrib != NULL) ? attrib->getStringValue() : "");
         }
         if (geomName == "")
            geomName = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())->getName():"";

         fidID = "";
         intFidID++;
         fidID.Format("%d", intFidID);
         refName = "FID" + fidID;
       
         
			if (data->getInsert()->getGraphicMirrored() == true)
            botFile.WriteString("%s	%s	%s	%s	%s	%s	%s	%s\n",  refName, xStr, yStr, rot, partNumber, geomName,sHLand ,sVLand );
			else
            topFile.WriteString("%s	%s	%s	%s	%s	%s	%s	%s\n",  refName, xStr, yStr, rot, partNumber, geomName,sHLand ,sVLand );
      
         
		}
		
		if (data->getInsert()->getInsertType() != insertTypePcb)
         continue;
      
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
         

         DataStruct* data = pcb_Block->getNextDataInsert(dataPos);
         // if i used 'data != NULL' instead of !data, nothing is written.
         if (data->getInsert()->getInsertType() != insertTypePcbComponent && data->getInsert()->getInsertType() != insertTypeFiducial)
               continue;
         CPoint2d compXY = data->getInsert()->getOrigin2d();
         
			//changes for case 1698 10/19/2005
			CString loaded = "TRUE";
			if (attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_LOADED, 1))
			{
            CString value = attrib->getStringValue();
				if (value.CompareNoCase("false") == 0)
					loaded = "FALSE";
			}

			if (loaded == "FALSE" && exportUNLoaded == "N")
				continue;

			if (exportThru == false)
			{
				CString technology;
				attrib = is_attvalue(m_pDoc, data->getAttributeMap(),ATT_TECHNOLOGY , 0);
				technology = ((attrib != NULL) ? attrib->getStringValue() : "");
				if (technology.MakeUpper() == "THRU")
					continue;
			}

			// end changes for case 1698

         CTMatrix l_compMatrix;
         refName  = data->getInsert()->getRefname();
         //if (refName.IsEmpty() && data->getInsert()->getInsertType() == insertTypeFiducial )
			if (data->getInsert()->getInsertType() == insertTypeFiducial )
         {
            fidID = "";
            intFidID++;
            fidID.Format("%d", intFidID);
            refName = "FID" + fidID;
				compCentroid.x = data->getInsert()->getOriginX();
				compCentroid.y  = data->getInsert()->getOriginY();
				l_boardMatrix.transform(compCentroid);
				if (data->getInsert()->getGraphicMirrored() == false)
					xStr.Format("%.3f", compCentroid.x  * m_dUnitFactor);
				else
					xStr.Format("%.3f",  compCentroid.x  * (-1) * m_dUnitFactor);

				yStr.Format("%.3f",  compCentroid.y * m_dUnitFactor);
	         
         }
			else
			{
				data->getInsert()->getCentroidLocation(m_pDoc->getCamCadData(), compCentroid);
				l_boardMatrix.transform(compCentroid);
	         
				if (data->getInsert()->getGraphicMirrored() == false)
					xStr.Format("%.3f", (compCentroid.x) * m_dUnitFactor);
				else
					xStr.Format("%.3f", (compCentroid.x) * (-1) * m_dUnitFactor);

				yStr.Format("%.3f", (compCentroid.y) * m_dUnitFactor);
	         
			}
         geomName = "";
         if (!m_PackageName.IsEmpty())
         {
            attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_PackageName, 0);
            geomName = ((attrib != NULL) ? attrib->getStringValue() : "");
         }
         if (geomName == "")
            geomName = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())?m_pDoc->getBlockAt(data->getInsert()->getBlockNumber())->getName():"";

         
         attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
         partNumber = ((attrib != NULL) ? attrib->getStringValue() : "No_Part_Number");
         
         if ((data->getInsert()->getInsertType() == insertTypeFiducial) && partNumber == "No_Part_Number")
            partNumber = "FID";

         
			CBasesVector * m_pcbBasesVector = &data->getInsert()->getBasesVector();
   
			CBasesVector cmpBV(data->getInsert()->getBasesVector());
			if (m_pcbBasesVector != NULL)				
				cmpBV.transform(m_pcbBasesVector->getTransformationMatrix());

			CTMatrix compMatrix = cmpBV.getTransformationMatrix();

			int insertedBlockNum = data->getInsert()->getBlockNumber();
			BlockStruct *insertedBlock = m_pDoc->getBlockAt(insertedBlockNum);
			POSITION dataPos = insertedBlock->getDataList().GetHeadPosition();
			double pinX; 
			double pinY;

			while (dataPos)
			{
				/*if (refName == "U1")
					refName = "U1";*/
            
				DataStruct *data = insertedBlock->getDataList().GetNext(dataPos);
            
				if (data->getDataType() == T_INSERT &&
					data->getInsert()->getInsertType() == INSERTTYPE_PIN)
				{
					CBasesVector pinBV(data->getInsert()->getBasesVector());
					pinBV.transform(compMatrix);
					int pinBlockNum = data->getInsert()->getBlockNumber();
					BlockStruct *pinBlock = m_pDoc->getBlockAt(pinBlockNum);
					if (pinBlock != NULL)
					{
						CExtent *pinExtent = &pinBlock->getExtent();
						pinX = pinExtent->getXsize();
						pinY = pinExtent->getYsize();
                  
						if (hLand < pinX)
							hLand = pinX;
						if (vLand < pinY)
							vLand = pinY;
					}
					
				}
			}

				
			sHLand.Format ("%.2f", hLand * m_dUnitFactor);
			sVLand.Format ("%.2f", vLand * m_dUnitFactor);
			if (data->getInsert()->getInsertType() == insertTypeFiducial)
			{
				if (o_Units == unit_o_Mils)
				{
					sHLand = "40";
					sVLand = "40";
				}
				else
				{
					sHLand = "1.0";
					sVLand = "1.0";
				}
			}
			vLand = 0; hLand = 0;  //reset for the next comp	

         rot.Format("%.1f", normalizeDegrees(RadToDeg(data->getInsert()->getAngle())));

			if (data->getInsert()->getGraphicMirrored() == true)
            botFile.WriteString("%s	%s	%s	%s	%s	%s	%s	%s\n", l_pcbName + "_" + refName, xStr, yStr, rot, partNumber, geomName,sHLand ,sVLand );
         else
            topFile.WriteString("%s	%s	%s	%s	%s	%s	%s	%s\n", l_pcbName + "_" + refName, xStr, yStr, rot, partNumber, geomName,sHLand ,sVLand );
			
      }
   }   
   
   return TRUE;      
}






