// $Header: /CAMCAD/4.5/ManufacturingGrid.cpp 19    11/07/06 12:55p Rick Faltersack $

/*
$History: ManufacturingGrid.cpp $
 * 
 * *****************  Version 19  *****************
 * User: Rick Faltersack Date: 11/07/06   Time: 12:55p
 * Updated in $/CAMCAD/4.5
 * Version = "4.5.1991";   // 11/07/06 - Case 2239 - Spectrum Writer - Add
 * support for Manufacturing Grid output - rcf
 *    
 * 
 * *****************  Version 18  *****************
 * User: Kurt Van Ness Date: 4/28/06    Time: 6:02p
 * Updated in $/CAMCAD/4.5
 * 
 * *****************  Version 17  *****************
 * User: Kurt Van Ness Date: 4/28/06    Time: 2:14p
 * Updated in $/CAMCAD/4.5
 * 
 * *****************  Version 15  *****************
 * User: Kurt Van Ness Date: 3/21/04    Time: 3:37a
 * Updated in $/CAMCAD/4.4
 * 
 * *****************  Version 14  *****************
 * User: Kurt Van Ness Date: 3/15/04    Time: 7:11a
 * Updated in $/CAMCAD/4.4
 * 
 * *****************  Version 13  *****************
 * User: Kurt Van Ness Date: 3/14/04    Time: 9:27a
 * Updated in $/CAMCAD/4.4
 * 
 * *****************  Version 12  *****************
 * User: Kurt Van Ness Date: 3/09/04    Time: 9:22a
 * Updated in $/CAMCAD/4.4
 * 
 * *****************  Version 11  *****************
 * User: Kurt Van Ness Date: 3/08/04    Time: 1:53a
 * Updated in $/CAMCAD/4.4
 * 
 * *****************  Version 10  *****************
 * User: Kurt Van Ness Date: 3/07/04    Time: 12:18p
 * Updated in $/CAMCAD/4.4
 * 
 * *****************  Version 8  *****************
 * User: Kurt Van Ness Date: 3/01/04    Time: 7:36p
 * Updated in $/CAMCAD/4.3
 * 
 * *****************  Version 7  *****************
 * User: Kurt Van Ness Date: 8/15/03    Time: 12:55a
 * Updated in $/CAMCAD/4.3
 * 
 * *****************  Version 6  *****************
 * User: Kurt Van Ness Date: 8/14/03    Time: 5:54p
 * Updated in $/CAMCAD/4.3
 * 
 * *****************  Version 5  *****************
 * User: Kurt Van Ness Date: 8/12/03    Time: 5:07p
 * Updated in $/CAMCAD/4.3
*/

#include "StdAfx.h"
#include "ManufacturingGrid.h"
#include "Graph.h"
#include "Font.h"

//_____________________________________________________________________________
CManufacturingGrid::CManufacturingGrid()
{
   m_xSteps    = 0;
   m_ySteps    = 0;
   m_xStepSize = 0.;
   m_yStepSize = 0.;
}

CManufacturingGrid::CManufacturingGrid(int xSteps,double xStepSize,int ySteps,double yStepSize)
{
   m_xSteps    = abs(xSteps);
   m_ySteps    = abs(ySteps);
   m_xStepSize = fabs(xStepSize);
   m_yStepSize = fabs(yStepSize);
}

CManufacturingGrid::~CManufacturingGrid()
{
}

void CManufacturingGrid::setOrigin(const CPoint2d& origin)
{
   m_origin = origin;
}

void CManufacturingGrid::set(const CString& definitionString)
{
   CSupString definition(definitionString);
   CStringArray params;

   int numParams = definition.ParseWhite(params);

   for (int ind = 0;ind < numParams;ind++)
   {
      int pos = params[ind].Find("=");

      if (pos >= 0 && pos < params[ind].GetLength() - 1)
      {
         CString name = params[ind].Left(pos);
         CString value = params[ind].Mid(pos + 1);

         if (name.CompareNoCase("xSteps") == 0)
         {
            m_xSteps = atoi(value);
         }
         else if (name.CompareNoCase("ySteps") == 0)
         {
            m_ySteps = atoi(value);
         }
         else if (name.CompareNoCase("xStepSize") == 0)
         {
            m_xStepSize = atof(value);
         }
         else if (name.CompareNoCase("yStepSize") == 0)
         {
            m_yStepSize = atof(value);
         }
         else if (name.CompareNoCase("xOrigin") == 0)
         {
            m_origin.x = atof(value);
         }
         else if (name.CompareNoCase("yOrigin") == 0)
         {
            m_origin.y = atof(value);
         }
      }
   }
}

CString CManufacturingGrid::getDefinitionString()
{
   CString definitionString;

   definitionString.Format(
"xSteps=%d xStepSize=%f ySteps=%d yStepSize=%f xOrigin=%f yOrigin=%f",
m_xSteps,m_xStepSize,m_ySteps,m_yStepSize,m_origin.x,m_origin.y);

   return definitionString;
}

CString CManufacturingGrid::alphaDesignator(int number)
{
   char letter = number%26 + 'A';
   CString designator(letter);

   if (number >= 26)
   {
      designator = alphaDesignator((number/26) - 1) + designator;
   }

   return designator;
}

CString CManufacturingGrid::getXGridCoordinate(double x)
{
   double dx = fabs(x - m_origin.x);

   int index = 0;

   if (m_xStepSize > 0.) index = (int)(dx / m_xStepSize);

   CString retval;
   retval.Format("%d",index + 1);

   return retval;
}

CString CManufacturingGrid::getYGridCoordinate(double y)
{
   double dy = fabs(y - m_origin.y);

   int index = 0;

   if (m_yStepSize > 0.) index = (int)(dy / m_yStepSize);

   CString retval = alphaDesignator(index);

   return retval;
}

CString CManufacturingGrid::getGridCoordinate(const CPoint2d& coordinate)
{
   return getYGridCoordinate(coordinate.y) + getXGridCoordinate(coordinate.x);
}

CPoint2d CManufacturingGrid::getGridSpaceCenter(const CString& gridCoordinate)
{
   int xIndex,yIndex;
   getIndexCoordinates(xIndex,yIndex,gridCoordinate);

   CPoint2d center;
   center.x = m_origin.x + ((xIndex + .5) * m_xStepSize);
   center.y = m_origin.y + ((yIndex + .5) * m_yStepSize);

   return center;
}

void CManufacturingGrid::getIndexCoordinates(int& x,int& y,const CString& gridCoordinate)
{
   x = y = 0;

   const char* p = (const char*)gridCoordinate;

   while (isalpha(*p))
   {
      y = 26*y + (toupper(*p) - 'A');
      p++;
   }

   while (isdigit(*p))
   {
      x = 10*x + (*p - '0');
      p++;
   }

   if (x > 0) x--;
}

void CManufacturingGrid::regenerateGrid(CCEtoODBDoc& camCadDoc,
   FileStruct& pcbFile,CExtent extent,double gridLineWidth)
{
   // make grid geometry
   camCadDoc.PrepareAddEntity(&pcbFile);
   int layer = Graph_Level("Manufacturing Grid", "", FALSE);
   camCadDoc.getLayerArray()[layer]->setColor( RGB(122, 122, 122));
   camCadDoc.getLayerArray()[layer]->setEditable(false);
   BlockStruct *block = Graph_Block_On(GBO_OVERWRITE, "Manufacturing Grid", pcbFile.getFileNumber(), 0);
   int widthIndex = Graph_Aperture("", T_ROUND,gridLineWidth, 0, 0, 0, 0, 0, 0, 0, NULL);
   setOrigin(CPoint2d(extent.getXmin(),extent.getYmin()));

   BlockStruct* pcbBlock = pcbFile.getBlock();
   
   if (pcbBlock != NULL)
   {
      camCadDoc.SetUnknownAttrib(&(pcbBlock->getAttributesRef()),getAttributeName(),
         getDefinitionString(),SA_OVERWRITE,NULL);
   }
   
   FontStruct* fs = CFontList::getFontList().getFont(0);

   int vDesignatorWidth = 1;
   int hDesignatorWidth = 1;

   for (int vBase = m_ySteps;vBase > 26;vBase /= 26)
   {
      vDesignatorWidth++;
   }

   for (int hBase = m_xSteps;hBase > 10;hBase /= 10)
   {
      hDesignatorWidth++;
   }

   double charSize = 0.33 * min(m_xStepSize, m_yStepSize);

   if (hDesignatorWidth > 2) charSize = 2.*charSize / hDesignatorWidth;

	int i=0;
   for (i=1;i <= m_xSteps;i++)
   {
      double x = extent.getXmin() + m_xStepSize * i;
      double margin = 2.0 * charSize;

      if (i != m_xSteps)
      {
         Graph_Line(layer, x, extent.getYmin() - margin, x, extent.getYmax() + margin, 0, widthIndex, FALSE);
      }

      CString designator = getXGridCoordinate(x - m_xStepSize/2.);
      double textWidth = 0.;
      int textLen = designator.GetLength();

      if (textLen > 1)
      {
         double width = 0.;

         for (int charInd = 0;charInd < textLen;charInd++)
         {
            width += fs->getCharacterWidth(designator.GetAt(charInd));
         }

         textWidth = charSize * ((2.0 * width) + (0.01 * camCadDoc.getSettings().TextSpaceRatio));
      }
      else
      {
         textWidth = charSize;
      }

      DataStruct *data = Graph_Text(layer, designator, x - 0.5 * (m_xStepSize + textWidth), 
         extent.getYmin() - margin, charSize, charSize, 0, 0, TRUE, 0, 0, 0, 0, 0);
      data->getText()->setMirrorDisabled(true);
   }
   
   for (i=1;i <= m_ySteps;i++)
   {
      double y = extent.getYmin() + m_yStepSize * i;
      double margin = 2.0 * charSize;

      if (i != m_ySteps)
      {
         Graph_Line(layer, extent.getXmin() - margin, y, extent.getXmax() + margin, y, 0, widthIndex, FALSE);
      }

      CString designator = getYGridCoordinate(y - m_yStepSize/2.);
      double textWidth = 0.;
      int textLen = designator.GetLength();
      double xOrig = extent.getXmin() - margin;

      if (textLen > 1)
      {
         double width = 0.;

         for (int charInd = 0;charInd < textLen;charInd++)
         {
            width += fs->getCharacterWidth(designator.GetAt(charInd));
         }

         textWidth = charSize * ((2.0 * width) + (0.01 * camCadDoc.getSettings().TextSpaceRatio));

         xOrig -= textWidth/2.0;
      }
      else
      {
         textWidth = charSize;
      }

      DataStruct *data = Graph_Text(layer, designator, xOrig, 
         y - 0.5 * (m_yStepSize + charSize), charSize, charSize, 0, 0, 0, 0, 0, 0, 0, 0);
      data->getText()->setMirrorDisabled(true);
   }

   Graph_Block_Off();

   // insert grid
	POSITION pos = NULL;
   for (pos = pcbFile.getBlock()->getDataList().GetTailPosition();pos != NULL;)
   {
      DataStruct *data = pcbFile.getBlock()->getDataList().GetPrev(pos);

      if (data->getDataType() == T_INSERT && data->getInsert()->getBlockNumber() == block->getBlockNumber())
      {
         RemoveOneEntityFromDataList(&camCadDoc, &pcbFile.getBlock()->getDataList(), data);

         break;
      }
   }

   Graph_Block_Reference("Manufacturing Grid", "", pcbFile.getFileNumber(), 0, 0, 0, 0, 1, 0, 0);
   block->resetExtent();
   pcbFile.getBlock()->resetExtent();

   // GRID LOCATION attribute
   WORD kw = camCadDoc.RegisterKeyWord(ATT_GRID_LOCATION, 0, VT_STRING);
   pos = pcbFile.getBlock()->getDataList().GetTailPosition();

   while (pos)
   {
      DataStruct *data = pcbFile.getBlock()->getDataList().GetPrev(pos);

      // Case 2239 added vias and test points to items that receive the attribute
      if (data->isInsertType(insertTypePcbComponent) ||
         data->isInsertType(insertTypeVia) ||
         data->isInsertType(insertTypeTestProbe))
      {
         CString gridLocation = getGridCoordinate(CPoint2d(data->getInsert()->getOriginX(),data->getInsert()->getOriginY()));

         camCadDoc.SetAttrib(&data->getAttributesRef(), kw, VT_STRING, (void*)(const char*)gridLocation, SA_OVERWRITE, NULL);
//       doc->SetVisAttrib(&data->getAttributesRef(), kw, VT_STRING, gridLocation, 0, 0, 0, charSize, charSize, TRUE, 0, TRUE, SA_OVERWRITE, 0, layer, TRUE);//NULL);
      }
   }
}

