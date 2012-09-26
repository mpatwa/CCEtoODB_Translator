// $Header: /CAMCAD/5.0/read_wrt/Royonics_Out.cpp 26    6/17/07 8:59p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "StdAfx.h"
#include "ccdoc.h"
#include "Royonics_Out.h"
#include "PcbUtil.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(RoyonicsDLG, CDialog)
RoyonicsDLG::RoyonicsDLG(CWnd* pParent /*=NULL*/)
   : CDialog(RoyonicsDLG::IDD, pParent)
   , m_RoyBoard(_T(""))
{
}

RoyonicsDLG::~RoyonicsDLG()
{
}

void RoyonicsDLG::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDIT1, m_RoyBoard);
}


BEGIN_MESSAGE_MAP(RoyonicsDLG, CDialog)
END_MESSAGE_MAP()


// RoyonicsDLG message handlers


void Royonics_WriteFile(CString filename, CCEtoODBDoc &doc, FormatStruct *format)
{

   RoyonicsWrite writer(&doc, format->Scale);

   if (!writer.Write(filename))
   {
      ErrorMessage("CAMCAD could not finish writing the Royonics file.", "", MB_OK);
   }
}

/*
Constructor
*/
RoyonicsWrite::RoyonicsWrite(CCEtoODBDoc *document, double l_Scale)
{
   m_stringArray.SetSize(0, 100);
   m_pDoc = document;
   p_Scale = l_Scale;
}

/*
RoyonicsWrite::Write
*/

BOOL RoyonicsWrite::Write(CString RoyonicsFileName)
{

   CString SYMB_ATT = "";
   if (!royonicsFile.Open(RoyonicsFileName, CFile::modeCreate|CFile::modeWrite))
   {
      ErrorMessage("File could not be opened", "", MB_OK);
      return FALSE;
   }
   RoyonicsDLG dlg;
   dlg.DoModal();
   RoyonicsBoardName = dlg.m_RoyBoard  ;

   // generate centroid in order to get centroid of the insert.
   m_pDoc->OnGenerateComponentCentroid();
   LoadSettings(getApp().getExportSettingsFilePath("Royonics.out"));
   GetRoyonicsHeader(getApp().getSystemSettingsFilePath("Royonics.hdr"));
   royonicsFile.WriteString ("%s", RoyonicsHDR);

   /*!">  !A1 :3109501, DIODE_TRANSZORB ICTE-15 15V 5W GR.6X10 U;ICTE-15 1
     !SV17
     !X7.85 Y-0.25 R180  :V22 DIODE_TRANSZORB ICTE-15 1
     !X-9.15 Y-14.62 R180  :V2102 DIODE_TRANSZORB ICTE-
     !X-9.15 Y-15.62 R0  :V2103 DIODE_TRANSZORB ICTE-15
     !X8.85 Y31.57 R0  :V348 DIODE_TRANSZORB ICTE-15 15*/


   FileStruct * lFile = m_pDoc->getFileList().GetFirstShown(blockTypePcb);
   if (lFile == NULL)
   {

      return TRUE;
   }

   CString lFileName = lFile->getName();
   CString refName;
   CString Description;
   CString partNumber;
   CString xStr;
   CString yStr;
   CString rot;
   CString delim = (char)5;
   CString dupDelim = (char)6;
   BlockStruct* block = lFile->getBlock();

   POSITION dataPos = block->getHeadDataInsertPosition();
   while (dataPos)
   {
      Attrib *attrib;
      CPoint2d compCentroid;
      DataStruct* data = block->getNextDataInsert(dataPos);
      // if i used 'data != NULL' instead of !data, nothing is written.
      if (data == NULL || data->getInsert()->getInsertType() != insertTypePcbComponent)
         continue;

      CString loaded = "TRUE";

      if (attrib = is_attvalue(m_pDoc, data->getAttributesRef(), ATT_LOADED, 1))
      {
         CString value = attrib->getStringValue();
         if (value.CompareNoCase("false") == 0)
            loaded = "FALSE";
      }

      if (loaded == "FALSE")
         continue;

      CString surface;
      if (data->getInsert()->getGraphicMirrored() == true)
         continue;

      CString technology;
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(),m_TechnologyAtt , 0);
      technology = attrib?attrib->getStringValue():"";
      if (technology.MakeUpper() != m_TechnologyValue)
         continue;

      SYMB_ATT = "";
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(),m_SymbAtt , 0);
      SYMB_ATT = attrib?attrib->getStringValue():"";
      if (SYMB_ATT.Trim() == "")
         SYMB_ATT = "SYMB";

      int l_Priority;
      CString s_Priority;
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(),m_OrderAtt , 0);
      s_Priority = attrib?attrib->getStringValue():"0";

      l_Priority = atoi(s_Priority);


      double l_xOffSet;
      CString s_xOffSet;
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(),"Theda_Offset_X" , 0);
      s_xOffSet = attrib?attrib->getStringValue():"0";

      l_xOffSet = atof(s_xOffSet);


      double l_yOffSet;
      CString s_yOffSet;
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(),"Theda_Offset_Y" , 0);
      s_yOffSet = attrib?attrib->getStringValue():"0";

      l_yOffSet = atof(s_yOffSet);

      double l_rOffSet;
      CString s_rOffSet;
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(),"Theda_Offset_Rotation" , 0);
      s_rOffSet = attrib?attrib->getStringValue():"0";

      l_rOffSet = atof(s_rOffSet);

      refName  = data->getInsert()->getRefname();
      /*if (refName == "K1")
         refName = refName;*/
      /*CString geomName = "";
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_PackageName, 0);
      geomName = attrib?attrib->getStringValue():"";
      */



      attrib = is_attvalue(m_pDoc, data->getAttributeMap(), ATT_PARTNUMBER, 0);
      partNumber = attrib?attrib->getStringValue():"No_Part_Number";


      attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_DescAtt, 0);
      Description = attrib?attrib->getStringValue():"";

      CString DescriptionComp = "";
      attrib = is_attvalue(m_pDoc, data->getAttributeMap(), m_DescCompAtt, 0);
      DescriptionComp = attrib?attrib->getStringValue():"";

      /*data->getInsert()->getCentroidLocation(m_pDoc, compCentroid);

      xStr.Format("%.2f", (compCentroid.x + l_xOffSet) * m_dUnitFactor);
      yStr.Format("%.2f", (compCentroid.y + l_yOffSet) * m_dUnitFactor);*/

      // start pin section
      /*CBasesVector * m_pcbBasesVector = &data->getInsert()->getBasesVector();

      CBasesVector cmpBV(data->getInsert()->getBasesVector());
      if (m_pcbBasesVector != NULL)
         cmpBV.transform(m_pcbBasesVector->getTransformationMatrix());

      CTMatrix compMatrix = cmpBV.getTransformationMatrix();*/
      CTMatrix compMatrix = data->getInsert()->getTMatrix();

      int insertedBlockNum = data->getInsert()->getBlockNumber();
      BlockStruct *insertedBlock = m_pDoc->getBlockAt(insertedBlockNum);
      POSITION dataPosPin = insertedBlock->getDataList().GetHeadPosition();
      double pinX = 0;
      double pinY = 0;
      bool pinFound = false;
      while (dataPosPin && pinFound == false)
      {


         DataStruct *dataPin = insertedBlock->getDataList().GetNext(dataPosPin);
         if (dataPin->getDataType() == T_INSERT &&
            dataPin->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {

            CString refNamePin = dataPin->getInsert()->getRefname();
            int refNum;
            refNum = atoi(refNamePin);

            if (refNamePin == "1" && refNum == 1)
            {
               CTMatrix pinMatrix = dataPin->getInsert()->getTMatrix();
               CTMatrix newMatrix = pinMatrix * compMatrix;
                CPoint2d pinPoint;
               newMatrix.transform(pinPoint);
               pinX = pinPoint.x ;
               pinY = pinPoint.y ;
               pinFound = true;


            }
            else if (refNum == 0)  // alpha numeric
            {
               int z = 0;
               CString c = refNamePin.Mid(z, 1);
               refNum = atoi(c);
               while(refNum == 0)
               {
                  refNum = atoi(c);
                  if (refNum == 1)
                  {
                     CTMatrix pinMatrix = dataPin->getInsert()->getTMatrix();
                     CTMatrix newMatrix = pinMatrix * compMatrix;
                     CPoint2d pinPoint;
                     newMatrix.transform(pinPoint);
                     pinX = pinPoint.x ;
                     pinY = pinPoint.y ;
                     pinFound = true;
                     continue;
                  }
                  z++;
                  if (z == refNamePin.GetLength())
                     refNum = -1;
                  else
                     c = refNamePin.Mid(z, 1);

               }
            }

         }
      }

      xStr.Format("%.2f", (pinX + l_xOffSet) * m_dUnitFactor);
      yStr.Format("%.2f", (pinY + l_yOffSet) * m_dUnitFactor);

      // end pin section
      double deg = (RadToDeg(data->getInsert()->getAngle()) - 270 - l_rOffSet);

      rot.Format("%.2f", deg);
      deg = atof(rot);
      rot.Format("%.2f", normalizeDegrees(deg));

      CString currData,value;
      currData = "";

      if (m_stringArray.GetCount() > l_Priority)
      {
         currData = m_stringArray.GetAt(l_Priority);
         if (currData != "" )
            value = currData + dupDelim;
      }

      value += partNumber + delim + Description + delim + xStr + delim + yStr + delim + rot + delim + refName + delim + SYMB_ATT + delim + DescriptionComp;
      m_stringArray.SetAtGrow(l_Priority,value);

      //if (currData != "")
      // m_stringArray.SetAtGrow(l_Priority, m_stringArray.GetAt(l_Priority) + dupDelim + partNumber + delim + Description + delim + xStr + delim + yStr + delim + rot + delim + refName + delim + SYMB_ATT + delim + DescriptionComp);
      //else
      // m_stringArray.SetAtGrow(l_Priority, partNumber + delim + Description + delim + xStr + delim + yStr + delim + rot + delim + refName + delim + SYMB_ATT + delim + DescriptionComp);
      //royonicsFile.WriteString("%s     %s     %s     %s     %s     %s\n", refName, xStr, yStr, rot, partNumber, geomName);

   }


   int j = 0; int k = 0; int bin = 0;  int arrCount = 0; bool firstFlag = true;
   for (int i = 0; i < m_stringArray.GetCount(); i++)
   {

      CStringArray strDataTwo;
      CStringArray stringData;



      int y = 0;
      CString pn = "";
      CSupString arrayData = "";
      CSupString arrayData2 = "";
      arrayData =  m_stringArray.GetAt(i);
      if (arrayData == "")
         continue;
      arrayData.Parse (stringData, dupDelim);
      arrCount = stringData.GetCount();
      bool flag = false;
      while (flag == false)
      {
         for (k = 0; k < stringData.GetCount(); k++)
         {
            arrayData2 = stringData.GetAt(k);

            if (arrayData2.IsEmpty())
               continue;

            arrayData2 += " ";

            int dataCount2 = arrayData2.Parse(strDataTwo, delim);

            if (dataCount2 < 8)
            {
               CString debugData = stringData.GetAt(k);

               formatMessageBox("Logic error: RoyonicsWrite::Write()");
               continue;
            }

            CString tempPN = strDataTwo.GetAt(0);
            if (pn == "")
            {
               pn = tempPN;
               bin++;
            }

            if (strDataTwo.GetAt(0) == pn && firstFlag)
            {
               firstFlag = false;
               royonicsFile.WriteString("\n");
               royonicsFile.WriteString("%s%d %s%s %s %s \n", "!\">  !A", bin, " :",  pn, ",", strDataTwo.GetAt(1));
               royonicsFile.WriteString(" !%s\n", strDataTwo.GetAt(6));
               stringData.SetAt(k, "");
            }

            if (strDataTwo.GetAt(0) == pn)
            {
               royonicsFile.WriteString("%s%s %s%s %s%s %s %s %s \n", " !X",
                     strDataTwo.GetAt(2), "Y", strDataTwo.GetAt(3), "R",
                     strDataTwo.GetAt(4), " :", strDataTwo.GetAt(5),
                     strDataTwo.GetAt(7));

               stringData.SetAt(k, "");
            }


            //     !X<CX> Y<CY> R<CROT>  :<REFDES> <DESCRIPTION>
            //royonicsFile.WriteString("%s%s %s%s %s%s %s %s %s \n", "  !X", strDataTwo.GetAt(2), "Y", strDataTwo.GetAt(3), "R", strDataTwo.GetAt(4), " :", strDataTwo.GetAt(5), strDataTwo.GetAt(1));
         }
         y++;
         firstFlag = true;
         //bin++;
         pn = "";
         if (y > arrCount)
            flag = true;
      }
      //royonicsFile.WriteString("\n");
      //bin++;
   }

   royonicsFile.WriteString("\n%s", "!\"Z");
   return TRUE;
}


int RoyonicsWrite::LoadSettings(CString fileName)
{
   m_OrderAtt = "";
   m_DescAtt = "";
   m_SymbAtt = "";
   m_dUnitFactor = 1;
   m_TechnologyAtt = ATT_TECHNOLOGY ;
   m_TechnologyValue =  "THRU";

   FILE *fp = fopen(fileName,  "rt");
   if ((fp ) == NULL)
   {
      CString tmp;
      tmp.Format("File [%s] is not found", fileName);
      MessageBox(NULL, tmp, "Royonics Write Settings", MB_OK | MB_ICONHAND);
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

      /*.UNITS INCH (inches)
      .UNITS MM (millimeters)
      .UNITS T_INCH (tenths of inch)
      .UNITS T_MM (tenths of mm)*/

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".UNITS"))
         {
            lp = get_string(NULL, " \t\n");
         if (lp == NULL)
            continue;

         if (!STRICMP(lp, "INCH"))
            o_Units = unit_o_Inch;
         else if (!STRICMP(lp, "MM"))
            o_Units = unit_o_MM;
         else if (!STRICMP(lp, "T_INCH"))
            o_Units = unit_o_TInch;
         else if (!STRICMP(lp, "T_MM"))
            o_Units = unit_o_TMM;
         }
         if (!STRICMP(lp, ".ORDER"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp == NULL)
               continue;

            m_OrderAtt = lp;
         }
         if (!STRICMP(lp, ".GEOM_DESCRIPTION"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp == NULL)
               continue;

            m_DescAtt = lp;
         }
         if (!STRICMP(lp, ".COMP_DESCRIPTION"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp == NULL)
               continue;

            m_DescCompAtt = lp;
         }
         if (!STRICMP(lp, ".SYMBOL"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp == NULL)
               continue;

            m_SymbAtt = lp;
         }
         // support for the technology variable attribute name:
         // .OUTPUT_ATTRIBUTE    TECH
         //.OUTPUT_VALUE          SMD
         if (!STRICMP(lp, ".OUTPUT_ATTRIBUTE"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp == NULL)
               continue;

            m_TechnologyAtt = lp;
         }
         if (!STRICMP(lp, ".OUTPUT_VALUE"))
         {
            lp = get_string(NULL, " \t\n");
            if (lp == NULL)
               continue;

            m_TechnologyValue = lp;
         }

     }
   }

   /*#define UNIT_INCHES                 0
   #define UNIT_MILS                   1
   #define UNIT_MM                     2
   #define UNIT_HPPLOTTER              3
   #define UNIT_TNMETER                4 */

   if (o_Units == unit_o_Inch)
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_INCHES) * p_Scale;
   else if (o_Units == unit_o_MM)
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_MM) * p_Scale;
   else if (o_Units == unit_o_TMM)
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_MM) * p_Scale * 10;
   else if (o_Units == unit_o_TInch)
      m_dUnitFactor = Units_Factor(m_pDoc->getSettings().getPageUnits(), UNIT_INCHES) * p_Scale * 10;

   fclose(fp);
   return 1;
}

void RoyonicsWrite::GetRoyonicsHeader(CString fileName)
{
   RoyonicsHDR = "";
   //m_dUnitFactor = 1;
   FILE *fp = fopen(fileName,  "rt");
   if ((fp ) == NULL)
   {
      CString tmp;
      tmp.Format("File [%s] is not found", fileName);
      MessageBox(NULL, tmp, "Royonics Header File", MB_OK | MB_ICONHAND);
      return;
   }

   CString LLx; CString LLy; CString URx; CString URy;

   // poly info
   FileStruct *pPcbFile = m_pDoc->getFileList().GetFirstShown(blockTypePcb);
   CDataList& dataList = pPcbFile->getBlock()->getDataList();
   POSITION pos = dataList.GetHeadPosition();
   double maxXSaved = 0; double maxYSaved = 0; double extMaxSaved = 0;
   while(pos)
   {
      const DataStruct* data = dataList.GetNext(pos);

      if (data->getDataType() == dataTypePoly && (data->getGraphicClass() == graphicClassBoardOutline || data->getGraphicClass() == graphicClassPanelOutline))
      {
         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
         {
            const CPoly* poly = data->getPolyList()->GetNext(polyPos);
            if (poly->isHidden())
               continue;
            CPoint2d p2;
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos != NULL)
            {
               CPnt* pnt = poly->getPntList().GetNext(pntPos);

               p2.x = pnt->x;
               p2.y = pnt->y;

               double maxX = p2.x; double maxY = p2.y;
               double extMax = maxY * maxY + maxX * maxX;
               if (extMax > extMaxSaved)
               {
                  extMaxSaved = extMax;
                  maxXSaved = maxX;
                  maxYSaved = maxY;
               }
            }
         }
      }
   }
   // end poly info

   double minX = 0.0; double minY = 0.0;
   LLx.Format("%0.2f", minX * m_dUnitFactor);
   LLy.Format("%0.2f", minY * m_dUnitFactor);
   URx.Format("%0.2f", maxXSaved * m_dUnitFactor);
   URy.Format("%0.2f", maxYSaved * m_dUnitFactor);


   char line[255];
   while (fgets(line, 255, fp))
   {
      CString w = line;
      if (w.Find("<BOARDNAME>"))
         w.Replace("<BOARDNAME>", ":" + RoyonicsBoardName );

      if (w.Find("<EXTENTS>"))
         w.Replace ("<EXTENTS>", "x" + LLx + " y"  + LLy + " x" + URx + " y" + URy );

      RoyonicsHDR = RoyonicsHDR + w;
   }


   fclose(fp);
   return;
}
