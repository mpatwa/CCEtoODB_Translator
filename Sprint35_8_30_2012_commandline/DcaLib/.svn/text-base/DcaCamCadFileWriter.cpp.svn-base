// $Header: /CAMCAD/DcaLib/DcaCamCadFileWriter.cpp 8     6/30/07 2:56a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaCamCadFileWriter.h"
#include "DcaWriteFormat.h"
#include "DcaLib.h"
#include "DcaEntityNumber.h"
#include "DcaVariant.h"
#include "DcaAttributes.h"
#include "DcaDataType.h"
#include "DcaPoint.h"
#include "DcaPoly.h"
#include "DcaFillStyle.h"
#include "DcaText.h"
#include "DcaInsert.h"
#include "DcaBlob.h"
#include "DcaDft.h"
#include "DcaMachine.h"
#include "DcaUnits.h"
#include "DcaTable.h"
#include "DcaXmlContent.h"

#define QIndent "  "

//_____________________________________________________________________________
CDcaCamCadFileWriter::CDcaCamCadFileWriter(CCamCadData& camCadData)
: m_camCadData(camCadData)
{
}

CDcaCamCadFileWriter::~CDcaCamCadFileWriter()
{
}

CString CDcaCamCadFileWriter::convertHtmlMetaCharacters(const CString& string)
{
   CString tempString;
   tempString = string;

   // XML Reserves Chars
   tempString.Replace("&", "&amp;");
   tempString.Replace("\"", "&quot;");
   tempString.Replace("'", "&apos;");
   tempString.Replace("<", "&lt;");
   tempString.Replace(">", "&gt;");

   for (int i=0; i<tempString.GetLength(); i++)
   {
      unsigned char ch = tempString.GetAt(i);

      if (ch < ' ' && ch != '\t' && ch != '\n' && ch != '\r')
      {
         CString replacement;
         replacement.Format("&#%d;", ch);
         tempString.Delete(i);
         tempString.Insert(i, replacement);
      }
   }
/*
   // UNICODE
   for (int i=0; i<tempString.GetLength(); i++)
   {
      unsigned char ch = tempString.GetAt(i);
      if (ch > 127)
      {
         CString replacement;
         replacement.Format("&#x%.04X;", ch);

//char tc[1];
//char *chPtr = tempString.GetBuffer(0) + i;
//OemToCharBuff(chPtr, tc, 1);  
//replacement.Format("&#x%.04X;", *(unsigned char*)tc);
//       ErrorMessage(tempString, string);

         tempString.Delete(i);
         tempString.Insert(i, replacement);
         i += 7;
      }
   }

//UTF-8
*/

   return tempString;
}

/******************************************************************************
* getwholenumcnt
This function is used to make sure to print 6 digits after the decimals using %g. %g is used to cut trailing zeros. 
*/
int CDcaCamCadFileWriter::getWholenumCnt(double number)
{
   number = fabs(number);
   if (number >= 1)
   {     
      if (number < 10)
         return 1;
      if (number < 100)
         return 2;
      if (number < 1000)
         return 3;
      if (number < 10000)
         return 4;
      if (number < 100000)
         return 5;
      if (number < 1000000)
         return 6;
      if (number < 10000000)
         return 7;
      if (number < 100000000)
         return 8;
      if (number < 1000000000)
         return 9;
      return 10;
   }
   else  
   {
      if (number >= 0.1)
         return 0;
      if (number >= 0.01)
         return -1;
      if (number >= 0.001)
         return -2;
      if (number >= 0.0001)
         return -3;
      if (number >= 0.00001)
         return -4;
      if (number >= 0.000001)
         return -5;     
      return -6;
   }
}

CString CDcaCamCadFileWriter::floatingPointNumberString(double number)
{
   CString numberString;
   const int outputUnitsAccurary = 6;
   int width = outputUnitsAccurary + getWholenumCnt(number);

   numberString.Format("%1.*g",width,number);

   return numberString;
}

void CDcaCamCadFileWriter::writeBufferedString(CWriteFormat& writeFormat,const CString& string)
{
   bool flag = false;
   CString buf;
   CString bufData = string;
   int currSection = 2000;

   while (flag == false)
   {
      //WriteRedLineXML(writeFormat);
      buf = bufData.Left(currSection);
      currSection = bufData.StringLength(bufData) - currSection;

      if (currSection > 0)
      {           
         bufData = bufData.Right(currSection);

         if (currSection > 2000) {currSection = 2000;}
         //ErrorMessage(bufData);
         writeFormat.writef("%s", buf);
      }
      else 
      {  
         writeFormat.writef("%s", buf);
         flag = true;
      }
   }
}

void CDcaCamCadFileWriter::write(CWriteFormat& writeFormat,const CString& applicationName,const CString& applicationTitle,CDcaOperationProgress* operationProgress)
{
   int i = 0;
   writeFormat.setNewLineMode(true);

   //CString indent;
   writeFormat.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
   writeFormat.writef("<CCDoc version=\"%s\" application=\"%s\">\n",DOC_VERSION,applicationName);
   writeFormat.pushHeader(QIndent);
   writeFormat.writef("<!-- Created by %s -->\n", applicationTitle);

   CTime t;
   t = t.GetCurrentTime();
   writeFormat.writef("<!-- Created at %s -->\n", t.Format("date :%A, %B %d, %Y at %H:%M:%S"));

   // setup progress tracking
   CCamCadFileWriteProgress progress(getCamCadData(), operationProgress);
   progress.setStatus("Writing CAMCAD data file - ");

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // KEYWORDS
   //getKeyWordArray().WriteXML(writeFormat, progress);
   writeXml(writeFormat,getCamCadData().getKeyWordArray(),progress);

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // LAYER List
   //getLayerArray().WriteXML(writeFormat, progress, this);
   writeXml(writeFormat,getCamCadData().getLayerArray(),progress);

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // BLOCK List
   //getBlockArray().WriteXML(writeFormat, progress, this);
   writeXml(writeFormat,getCamCadData().getBlockArray(),progress);

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // FILE List
   //getFileList().WriteXML(writeFormat, progress, this);
   writeXml(writeFormat,getCamCadData().getFileList(),progress);

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // DRC ALGORITHMS
   if (getCamCadData().getDfmAlgorithmArraySize() > 0)
   {
      progress.incAlgorithm();

      writeFormat.writef( "<DRC_Algorithms>\n");
      writeFormat.pushHeader(QIndent);

      for (i=0; i < getCamCadData().getDfmAlgorithmArraySize(); i++)
      {
         writeFormat.writef("<DRC_Algorithm index=\"%d\" name=\"%s\"/>\n", i, convertHtmlMetaCharacters(getCamCadData().getDfmAlgorithmNamesArray()[i]));
      }

      writeFormat.popHeader();
      writeFormat.writef( "</DRC_Algorithms>\n");
   }


   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // GENERIC TABLES
   writeXml(writeFormat, getCamCadData().getTableList(), progress);


   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // WIDTH TABLE
   writeFormat.writef( "<WidthTable>\n");
   writeFormat.pushHeader(QIndent);

   for (i=0; i < getCamCadData().getNextWidthIndex(); i++)
   {
      progress.incWidth();

      if (!getCamCadData().getWidthTable()[i])
         continue;

      writeFormat.writef( "<Width index=\"%d\" geomNum=\"%d\"/>\n", i, getCamCadData().getWidthTable()[i]->getBlockNumber());
   }

   writeFormat.popHeader();
   writeFormat.writef( "</WidthTable>\n");
   
   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // Topics  
   if (!getCamCadData().getTopicsList().IsEmpty())
   {
      writeXml(writeFormat,getCamCadData().getTopicsList(),progress);
   }

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // NAMEDVIEWS
   //getNamedViewList().WriteXML(writeFormat, progress);
   writeXml(writeFormat,getCamCadData().getNamedViewList(),progress);

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // SETTINGS
   writeFormat.writef( "<Settings pageUnits=\"%d\"",getCamCadData().getPageUnits());
   writeFormat.writef( " left=\"%s\""              ,floatingPointNumberString(getCamCadData().getCamCadDataSettings().getXmin()));
   writeFormat.writef( " right=\"%s\""             ,floatingPointNumberString(getCamCadData().getCamCadDataSettings().getXmax()));
   writeFormat.writef( " top=\"%s\""               ,floatingPointNumberString(getCamCadData().getCamCadDataSettings().getYmax()));
   writeFormat.writef( " bottom=\"%s\""            ,floatingPointNumberString(getCamCadData().getCamCadDataSettings().getYmin()));
   writeFormat.writef( " bottomView=\"%d\"/>\n"    ,getCamCadData().getCamCadDataSettings().getBottomView());

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // BACKGROUND BITMAP
   const BackgroundBitmapStruct&    topBackgroundBitMap = getCamCadData().getTopBackgroundBitMap();
   const BackgroundBitmapStruct& bottomBackgroundBitMap = getCamCadData().getBottomBackgroundBitMap();

   if (!topBackgroundBitMap.getFileName().IsEmpty())
   {
      writeFormat.writef( "<BackgroundBitmapTop show=\"%d\" ",topBackgroundBitMap.getShow());
      writeFormat.writef( " left=\"%s\""                     ,floatingPointNumberString(topBackgroundBitMap.getXmin()));
      writeFormat.writef( " right=\"%s\""                    ,floatingPointNumberString(topBackgroundBitMap.getXmax()));
      writeFormat.writef( " top=\"%s\""                      ,floatingPointNumberString(topBackgroundBitMap.getYmax()));
      writeFormat.writef( " bottom=\"%s\""                   ,floatingPointNumberString(topBackgroundBitMap.getYmin()));
      writeFormat.writef( " filename=\"%s\"/>\n"             ,convertHtmlMetaCharacters(topBackgroundBitMap.getFileName()));
   }

   if (!bottomBackgroundBitMap.getFileName().IsEmpty())
   {
      writeFormat.writef( "<BackgroundBitmapBottom show=\"%d\" ",bottomBackgroundBitMap.getShow());
      writeFormat.writef( " left=\"%s\""                        ,floatingPointNumberString(bottomBackgroundBitMap.getXmin()));
      writeFormat.writef( " right=\"%s\""                       ,floatingPointNumberString(bottomBackgroundBitMap.getXmax()));
      writeFormat.writef( " top=\"%s\""                         ,floatingPointNumberString(bottomBackgroundBitMap.getYmax()));
      writeFormat.writef( " bottom=\"%s\""                      ,floatingPointNumberString(bottomBackgroundBitMap.getYmin()));
      writeFormat.writef( " filename=\"%s\"/>\n"                ,convertHtmlMetaCharacters(bottomBackgroundBitMap.getFileName()));
   }

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // Redline  
   if (! getCamCadData().getRedLineString().IsEmpty())
   {
      writeBufferedString(writeFormat,getCamCadData().getRedLineString());
   }

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // Lots  
   if (! getCamCadData().getLotsString().IsEmpty())
   {
      writeBufferedString(writeFormat,getCamCadData().getLotsString());
   }

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   // Color Sets  
   if (! getCamCadData().getColorSetsString().IsEmpty())
   {
      writeBufferedString(writeFormat,getCamCadData().getColorSetsString());
   }

   //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
   writeFormat.popHeader();
   writeFormat.writef("</CCDoc>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CKeyWordArray& keyWordArray,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Keywords>\n");

   writeFormat.pushHeader(QIndent);

   for (int keywordIndex=0;keywordIndex < keyWordArray.GetCount();keywordIndex++)
   {
      progress.incKeyWord();

      // the - on the end of the line is the continue for next keywords.
      // keywords are allowed to have blanks
      const KeyWordStruct* keywordStruct = keyWordArray.GetAt(keywordIndex);

      //keywordStruct->WriteXML(writeFormat);
      writeXml(writeFormat,*keywordStruct,progress);
   }

   writeFormat.popHeader();

   writeFormat.writef( "</Keywords>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const KeyWordStruct& keyWordStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Keyword index=\"%d\"",keyWordStruct.getIndex());
   writeFormat.writef(" valueType=\"%d\""    ,keyWordStruct.getValueType());
   writeFormat.writef(" group=\"%d\""        ,keyWordStruct.getGroup());
   writeFormat.writef(" CCKeyword=\"%s\""    ,convertHtmlMetaCharacters(keyWordStruct.getCCKeyword()));

   if (keyWordStruct.getInKeyword() != keyWordStruct.getCCKeyword())
   {
      writeFormat.writef(" inKeyword=\"%s\"",convertHtmlMetaCharacters(keyWordStruct.getInKeyword()));
   }

   if (keyWordStruct.getOutKeyword() != keyWordStruct.getCCKeyword())
   {
      writeFormat.writef(" outKeyword=\"%s\"",convertHtmlMetaCharacters(keyWordStruct.getOutKeyword()));
   }

   if (keyWordStruct.getHidden())
   {
      writeFormat.writef(" hidden=\"1\"");
   }

   writeFormat.writef(" inherit=\"%d\"/>\n", (int)(keyWordStruct.getInherited()));
}

//--------------------------------------------------------------------------------------

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat, const CGTabTableList& tableList, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Tables>\n");
   
   writeFormat.pushHeader(QIndent);

   for (int tblIndx = 0; tblIndx < tableList.GetCount(); tblIndx++)
   {
      CGTabTable *t = tableList.GetTableAt(tblIndx);

      if (t != NULL)
      {
         writeXml(writeFormat, *t, progress);
      }
   }

   writeFormat.popHeader();
   
   writeFormat.writef("</Tables>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat, const CGTabTable& table, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Table");
   if (!table.GetName().IsEmpty())   // it really wouldn't hurt to just write the empty name, not bother with check
      writeFormat.writef(" name=\"%s\"" , convertHtmlMetaCharacters(table.GetName()));
   writeFormat.writef(">\n");
   
   writeFormat.pushHeader(QIndent);

   // Column titles
   for (int i = 0; i < table.GetColumnHeadingCount(); i++)
   {
      writeFormat.writef("<Col name=\"%s\"/>\n", convertHtmlMetaCharacters( table.GetColumnHeadingAt(i) ));
   }

   // This section is optimized for speed by reducing number of "get count" calls to minumum, reducing
   // number of constructor calls to minimum, etc. It is customary throughout CAMCAD to just call
   // "get count" type functions in the for loops, and I prefer declaring local vars within their
   // inner most scope of usage. But UID tables are turning out particularly large, so this
   // loop is optimized by making it perform as close to minumum number of operations as we can get.
   // If this isn't enough, we might try buffering larger chunks to reduce number of write-to-disk calls.

   // Row data
   int rowCount = table.GetRowCount();
   for (int rowI = 0; rowI < rowCount; rowI++)
   {
      writeFormat.writef("<Row");
      CGTabRow *row = table.GetRowAt(rowI);
      if (row != NULL && !row->GetName().IsEmpty())
         writeFormat.writef(" name=\"%s\"" , convertHtmlMetaCharacters(row->GetName()));
      writeFormat.writef(">\n");


      writeFormat.pushHeader(QIndent);

      CString cellValue; // GetCellValueAt() always empties cellValue as first step, so okay to keep re-using same var
      int colCount = row->GetCellCount();
      for (int colI = 0; colI < colCount; colI++)
      {
         row->GetCellValueAt(colI, cellValue);
         writeFormat.writef("<Cell value=\"%s\"/>\n" , convertHtmlMetaCharacters(cellValue));
      }

      writeFormat.popHeader();
      writeFormat.writef("</Row>\n");  
   }

   // Attributes
   if (table.getAttributes() != NULL)
   {
      writeXml(writeFormat,*(table.getAttributes()),progress);
   }

   writeFormat.popHeader();
   
   writeFormat.writef("</Table>\n");
}
//--------------------------------------------------------------------------------------
void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CLayerArray& layerArray,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Layers>\n");
   
   writeFormat.pushHeader(QIndent);

   for (int layerIndex=0;layerIndex < layerArray.GetCount();layerIndex++)
   {
      progress.incLayer();
      LayerStruct* layer = layerArray.GetAt(layerIndex);

      if (layer != NULL)
      {
         //layer->WriteXML(writeFormat, doc);
         writeXml(writeFormat,*layer,progress);
      }
   }

   writeFormat.popHeader();
   
   writeFormat.writef("</Layers>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const LayerStruct& layerStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Layer num=\"%d\"",layerStruct.getLayerIndex());
   writeFormat.writef(" name=\"%s\""     ,convertHtmlMetaCharacters(layerStruct.getName()));
   writeFormat.writef(" visible=\"%d\""  ,(int)layerStruct.getVisible());
   writeFormat.writef(" editable=\"%d\"" ,(int)layerStruct.isEditable());
   writeFormat.writef(" color=\"%ld\""   ,layerStruct.getColor());
   writeFormat.writef(" layerType=\"%d\"",layerStruct.getLayerType());

   if (layerStruct.isNegative())
   {
      writeFormat.writef(" negative=\"%d\"",layerStruct.isNegative());
   }

   if (layerStruct.getFloating())
   {
      writeFormat.writef(" floating=\"%d\"",(int)layerStruct.getFloating());
   }

   if (layerStruct.getMirroredLayerIndex() != layerStruct.getLayerIndex())
   {
      writeFormat.writef(" mirror=\"%d\"",layerStruct.getMirroredLayerIndex());
   }

   if (layerStruct.getElectricalStackNumber() != 0)
   {
      writeFormat.writef(" electricStackup=\"%d\"",layerStruct.getElectricalStackNumber());
   }

   if (layerStruct.getArtworkStackNumber() != 0)
   {
      writeFormat.writef(" artworkStackup=\"%d\"",layerStruct.getArtworkStackNumber());
   }

   if (layerStruct.getPhysicalStackNumber() != 0)
   {
      writeFormat.writef(" physicalStackup=\"%d\"",layerStruct.getPhysicalStackNumber());
   }

   if (layerStruct.getLayerFlags() != 0)
   {
      writeFormat.writef(" attr=\"%lu\"",layerStruct.getLayerFlags());
   }

   if (layerStruct.getZHeight() != 0.)
   {
      writeFormat.writef(" zHeight=\"%s\"",floatingPointNumberString(layerStruct.getZHeight()));
   }

   if (layerStruct.getOriginalColor() != layerStruct.getColor())
   {
      writeFormat.writef(" originalColor=\"%ld\"",layerStruct.getOriginalColor());
   }

   writeFormat.writef(" originalVisible=\"%ld\"" ,(int)layerStruct.getOriginalVisible());
   writeFormat.writef(" originalEditable=\"%ld\"",(int)layerStruct.getOriginalEditable());

   if (! layerStruct.getComment().IsEmpty())
   {
      writeFormat.writef(" comment=\"%s\"",convertHtmlMetaCharacters(layerStruct.getComment()));
   }
   
   if (layerStruct.getAttributes() == NULL)
   {
      writeFormat.writef("/>\n");
   }
   else
   {
      writeFormat.writef(">\n");

      writeXml(writeFormat,*(layerStruct.getAttributes()),progress);

      writeFormat.writef("</Layer>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CBlockArray& blockArray,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef( "<Geometries>\n");

   writeFormat.pushHeader(QIndent);

   for (int blockIndex=0;blockIndex < blockArray.getSize();blockIndex++)
   {
      progress.incBlock();

      BlockStruct* block = blockArray.getBlockAt(blockIndex);

      if (block != NULL)
      {
         //block->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*block,progress);
      }
   }

   writeFormat.popHeader();

   writeFormat.writef( "</Geometries>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const BlockStruct& blockStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Geometry num=\"%d\"",blockStruct.getBlockNumber());
   writeFormat.writef(" name=\"%s\""        ,convertHtmlMetaCharacters(blockStruct.getName()));
   writeFormat.writef(" originalName=\"%s\"",convertHtmlMetaCharacters(blockStruct.getOriginalName()));
   writeFormat.writef(" fileNum=\"%d\""     ,blockStruct.getFileNumber());
   writeFormat.writef(" geomType=\"%d\""    ,blockStruct.getBlockType());

   if (blockStruct.getPackage())
   {
      writeFormat.writef(" package=\"1\"");
   }

   if (blockStruct.getFlags() != 0)
   {
      writeFormat.writef(" flag=\"%lu\"",blockStruct.getFlags());
   }

   if (blockStruct.getFlags() & BL_APERTURE || blockStruct.getFlags() & BL_BLOCK_APERTURE)
   {
      writeFormat.writef(" dCode=\"%d\""      ,blockStruct.getDcode());
      writeFormat.writef(" shape=\"%d\""      ,blockStruct.getShape());
      writeFormat.writef(" sizeA=\"%s\""      ,floatingPointNumberString(blockStruct.getSizeA()));
      writeFormat.writef(" sizeB=\"%s\""      ,floatingPointNumberString(blockStruct.getSizeB()));
      writeFormat.writef(" sizeC=\"%s\""      ,floatingPointNumberString(blockStruct.getSizeC()));
      writeFormat.writef(" sizeD=\"%s\""      ,floatingPointNumberString(blockStruct.getSizeD()));
      writeFormat.writef(" rotation=\"%s\""   ,floatingPointNumberString(blockStruct.getRotationRadians()));
      writeFormat.writef(" xOffset=\"%s\""    ,floatingPointNumberString(blockStruct.getXoffset()));
      writeFormat.writef(" yOffset=\"%s\""    ,floatingPointNumberString(blockStruct.getYoffset()));
      writeFormat.writef(" numSpokes=\"%d\""  ,blockStruct.getSpokeCount());
   }

   if (blockStruct.getFlags() & BL_TOOL || blockStruct.getFlags() & BL_BLOCK_TOOL
      || blockStruct.getFlags() & BL_COMPLEX_TOOL)
   {
      writeFormat.writef(" tCode=\"%d\""      ,blockStruct.getTcode());
      writeFormat.writef(" toolSize=\"%s\""   ,floatingPointNumberString(blockStruct.getToolSize()));
      writeFormat.writef(" display=\"%d\""    ,(int)blockStruct.getToolDisplay());
      writeFormat.writef(" geomNum=\"%d\""    ,blockStruct.getToolBlockNumber());
      writeFormat.writef(" type=\"%d\""       ,blockStruct.getToolType());
      writeFormat.writef(" plated=\"%d\""     ,(int)blockStruct.getToolHolePlated());
      writeFormat.writef(" punched=\"%d\""    ,(int)blockStruct.getToolHolePunched());
   }

   writeFormat.writef( ">\n");
   
   if (blockStruct.getAttributes() != NULL)
   {
      writeXml(writeFormat,*(blockStruct.getAttributes()),progress);
   }

   // start --- datas
   writeFormat.pushHeader(QIndent);
      
   writeXml(writeFormat,blockStruct.getDataList(),progress);

   writeFormat.popHeader();
   // end --- datas

   writeFormat.writef("</Geometry>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDataList& dataList,CCamCadFileWriteProgress& progress)
{
   if (! dataList.IsEmpty())
   {
      writeFormat.writef("<Datas>\n");
      
      writeFormat.pushHeader(QIndent);
      
      for (POSITION dataPos = dataList.GetHeadPosition();dataPos != NULL;)
      {
         progress.incData();
         DataStruct* data = dataList.GetNext(dataPos);

         //data->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*data,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</Datas>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const DataStruct& dataStruct,CCamCadFileWriteProgress& progress)
{
   CString typeTag;

   switch (dataStruct.getDataType())
   {
   case dataTypePoint:   typeTag = "Point";       break;
   case dataTypePoly:    typeTag = "PolyStruct";  break;
   case dataTypeText:    typeTag = "Text";        break;
   case dataTypeInsert:  typeTag = "Insert";      break;
   case dataTypeBlob:    typeTag = "Blob";        break;
   case dataTypePoly3D:  typeTag = "PolyStruct3D";  break;
   }

   writeFormat.writef("<%s",typeTag);

   writeFormat.writef(" entityNum=\"%ld\""  ,dataStruct.getEntityNumber());
   writeFormat.writef(" layer=\"%d\""       ,dataStruct.getLayerIndex());
   writeFormat.writef(" graphicClass=\"%d\"",dataStruct.getGraphicClass());

   if (dataStruct.isNegative())
   {
      writeFormat.writef(" negative=\"%d\"",dataStruct.isNegative());
   }

   if (dataStruct.getFlags() != 0)
   {
      writeFormat.writef(" flag=\"%lu\"",dataStruct.getFlags());
   }

   if (dataStruct.getColorOverride())
   {
      writeFormat.writef(" colorOverride=\"1\"");
      writeFormat.writef(" overrideColor=\"%lu\"",dataStruct.getOverrideColor());
   }

   if (dataStruct.getHidden())
   {
      writeFormat.writef(" hide=\"1\"");
   }

   if(dataStruct.HasLayerSpan())
   {
      writeFormat.writef(" physLyrStart=\"%d\" physLyrEnd=\"%d\"", dataStruct.GetPhysicalStartLayer(), dataStruct.GetPhysicalEndLayer());
   }

   writeFormat.pushHeader(QIndent);

   switch (dataStruct.getDataType())
   {
   case dataTypePoint:   
      if (dataStruct.getPoint() != NULL)
      {
         writeFormat.writef(" x=\"%s\" y=\"%s\">\n",floatingPointNumberString(dataStruct.getPoint()->x),floatingPointNumberString(dataStruct.getPoint()->y));
      }

      break;
   case dataTypePoly:
      if (dataStruct.getPolyList() != NULL)
      {
         if (dataStruct.getPolyList()->getFillStyle() != HS_NOT_SET)
         {
            writeFormat.writef(" fillStyle=\"%d\"",dataStruct.getPolyList()->getFillStyle());
         }

         writeFormat.writef(">\n");

         //if (m_polylist != NULL)
         //   m_polylist->WriteXML(writeFormat, progress);

         writeXml(writeFormat,*(dataStruct.getPolyList()),progress);
      }

      break;
   case dataTypeText:
      if (dataStruct.getText() != NULL)
      {
         const TextStruct& textStruct = *(dataStruct.getText());
         CString buf = textStruct.getText();
         CDcaXmlContent::SwapEscapeSequences(buf, false);

         writeFormat.writef(" text=\"%s\""            ,convertHtmlMetaCharacters(buf));
         writeFormat.writef(" x=\"%s\""               ,floatingPointNumberString(textStruct.getPnt().x));
         writeFormat.writef(" y=\"%s\""               ,floatingPointNumberString(textStruct.getPnt().y));
         writeFormat.writef(" height=\"%s\""          ,floatingPointNumberString(textStruct.getHeight()));
         writeFormat.writef(" width=\"%s\""           ,floatingPointNumberString(textStruct.getWidth()));
         writeFormat.writef(" mirror=\"%d\""          ,(int)textStruct.isMirrored());
         writeFormat.writef(" angle=\"%s\""           ,floatingPointNumberString(textStruct.getRotation()));
         writeFormat.writef(" penWidthIndex=\"%d\""   ,textStruct.getPenWidthIndex());
         writeFormat.writef(" fontnum=\"%d\""         ,textStruct.getFontNumber());
         writeFormat.writef(" proportional=\"%d\""    ,(int)textStruct.isProportionallySpaced());
         writeFormat.writef(" oblique=\"%d\""         ,textStruct.getOblique());
         writeFormat.writef(" neverMirror=\"%d\""     ,(int)textStruct.getMirrorDisabled());
         writeFormat.writef(" textAlignment=\"%d\""   ,textStruct.getHorizontalPosition());
         writeFormat.writef(" lineAlignment=\"%d\""   ,textStruct.getVerticalPosition());

         CFontProperties *fontProps = dataStruct.getText()->GetFontProperties();
         if (fontProps != NULL)
         {
            writeXml(writeFormat, *fontProps, progress);

            // facename="VeriBest Gerber 0" fonttype="2" weight="400" underlined="0" strikeout="0" orientation="0" 
            // charset="0" pitchAndFamily="0"
         
            /*
            writeFormat.writef(" facename=\"%s\""           ,convertHtmlMetaCharacters(fontProps->GetFaceName()));
            writeFormat.writef(" fonttype=\"%s\""           ,convertHtmlMetaCharacters(fontProps->GetFontType()));
            writeFormat.writef(" weight=\"%s\""             ,convertHtmlMetaCharacters(fontProps->GetWeight()));
            writeFormat.writef(" underlined=\"%s\""         ,convertHtmlMetaCharacters(fontProps->GetUnderlined()));
            writeFormat.writef(" strikeout=\"%s\""          ,convertHtmlMetaCharacters(fontProps->GetStrikeOut()));
            writeFormat.writef(" orientation=\"%s\""        ,convertHtmlMetaCharacters(fontProps->GetOrientation()));
            writeFormat.writef(" charset=\"%s\""            ,convertHtmlMetaCharacters(fontProps->GetCharSet()));
            writeFormat.writef(" pitchAndFamily=\"%s\""     ,convertHtmlMetaCharacters(fontProps->GetPitchAndFamily()));
            */
         }

         writeFormat.writef(">\n");
      }

      break;
   case dataTypeInsert:       
      if (dataStruct.getInsert() != NULL)
      {
         const InsertStruct& insertStruct = *(dataStruct.getInsert());

         writeFormat.writef(" insertType=\"%d\"" ,insertStruct.getInsertType());
         writeFormat.writef(" refName=\"%s\""    ,convertHtmlMetaCharacters(insertStruct.getRefname()));
         writeFormat.writef(" geomNum=\"%d\""    ,insertStruct.getBlockNumber());
         writeFormat.writef(" x=\"%s\""          ,floatingPointNumberString(insertStruct.getOriginX()));
         writeFormat.writef(" y=\"%s\""          ,floatingPointNumberString(insertStruct.getOriginY()));
         writeFormat.writef(" angle=\"%s\""      ,floatingPointNumberString(insertStruct.getAngle()));
         writeFormat.writef(" mirror=\"%d\""     ,insertStruct.getMirrorFlags());
         writeFormat.writef(" placeBottom=\"%d\"",insertStruct.getPlacedBottom());
         writeFormat.writef(" scale=\"%s\""      ,floatingPointNumberString(insertStruct.getScale()));

         if ((insertStruct.getFillStyle() != HS_NOT_SET))
         {
            writeFormat.writef(" fillStyle=\"%d\"", (insertStruct.getFillStyle()));
         }

         writeFormat.writef(">\n");
      }

      break;

   case dataTypeBlob:
      if (dataStruct.getBlob() != NULL)
      {
         const BlobStruct& blobStruct = *(dataStruct.getBlob());

         writeFormat.writef(" filename=\"%s\""   ,convertHtmlMetaCharacters(blobStruct.getFileName()));
         writeFormat.writef(" x=\"%s\""          ,floatingPointNumberString(blobStruct.getOrigin().x));
         writeFormat.writef(" y=\"%s\""          ,floatingPointNumberString(blobStruct.getOrigin().y));
         writeFormat.writef(" width=\"%s\""      ,floatingPointNumberString(blobStruct.getWidth()));
         writeFormat.writef(" height=\"%s\""     ,floatingPointNumberString(blobStruct.getHeight()));
         writeFormat.writef(" rotation=\"%s\""   ,floatingPointNumberString(blobStruct.getRotationRadians()));
         writeFormat.writef(" top=\"%d\""        ,blobStruct.getTopFlag());
         writeFormat.writef(" bottom=\"%d\">\n"  ,blobStruct.getBottomFlag());
      }

      break;
   case dataTypePoly3D:
      if (dataStruct.getPoly3DList() != NULL)
      {
         writeFormat.writef(">\n");
         writeXml(writeFormat,*(dataStruct.getPoly3DList()),progress);
      }

      break;
   } // end switch
   
   if (dataStruct.getAttributes() != NULL)
   {
      writeXml(writeFormat,*(dataStruct.getAttributes()),progress);
   }

   writeFormat.popHeader();

   writeFormat.writef("</%s>\n",typeTag);
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CPolyList& polyList,CCamCadFileWriteProgress& progress)
{   
   for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
   {
      const CPoly* poly = polyList.GetNext(polyPos);

      //poly->WriteXML(writeFormat, progress);
      writeXml(writeFormat,*poly,progress);
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CPoly& poly,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Poly");
   writeFormat.writef(" widthIndex=\"%d\"",poly.getWidthIndex());

   if (poly.isClosed())
   {
      writeFormat.writef(" closed=\"1\"");
   }

   if (poly.isFilled())
   {
      writeFormat.writef(" filled=\"1\"");
   }

   if (poly.isVoid())
   {
      writeFormat.writef(" void=\"1\"");
   }

   if (poly.isThermalLine())
   {
      writeFormat.writef(" thermal=\"1\"");
   }

   if (poly.isFloodBoundary())
   {
      writeFormat.writef(" boundary=\"1\"");
   }

   if (poly.isHidden())
   {
      writeFormat.writef(" hidden=\"1\"");
   }

   writeFormat.writef(">\n");

   // loop points of poly
   writeFormat.pushHeader(QIndent);

   //list.WriteXML(writeFormat, progress);
   writeXml(writeFormat,poly.getPntList(),progress);

   writeFormat.popHeader();

   writeFormat.writef("</Poly>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CPntList& pntList,CCamCadFileWriteProgress& progress)
{   
   for (POSITION pntPos = pntList.GetHeadPosition();pntPos != NULL;)
   {
      CPnt* pnt = pntList.GetNext(pntPos);

      writeFormat.writef("<Pnt");
      writeFormat.writef(" x=\"%1s\" y=\"%s\"",floatingPointNumberString(pnt->x),floatingPointNumberString(pnt->y));

      if (fabs(pnt->bulge) > BULGE_THRESHOLD)    
      {
         writeFormat.writef(" bulge=\"%s\"",floatingPointNumberString(pnt->bulge));   
      }

      writeFormat.writef("/>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CFileList& fileList,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef( "<Files>\n");

   writeFormat.pushHeader(QIndent);
   
   for (POSITION filePos = fileList.GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = fileList.GetNext(filePos);

      //file->WriteXML(writeFormat, progress, doc);
      writeXml(writeFormat,*file,progress);
   }

   writeFormat.popHeader();

   writeFormat.writef("</Files>\n");
}

void CDcaCamCadFileWriter::writeExtendedXmlAttributes(CWriteFormat& writeFormat,const FileStruct& fileStruct,CCamCadFileWriteProgress& progress)
{
   //writeFormat.writef(" curDFTSolution=\"%s\""       ,(fileStruct.getCurrentDftSolution() != NULL) ? convertHtmlMetaCharacters(fileStruct.getCurrentDftSolution()->GetName()) : "");
   //writeFormat.writef(" curMachineSolution=\"%s\">\n",(fileStruct.getCurrentMachine()     != NULL) ? convertHtmlMetaCharacters(fileStruct.getCurrentMachine()->GetName()) : "");
}

void CDcaCamCadFileWriter::writeExtendedXml(CWriteFormat& writeFormat,const FileStruct& fileStruct,CCamCadFileWriteProgress& progress)
{
   //if (fileStruct.getDftSolutionList() != NULL && fileStruct.getDftSolutionList()->GetCount() > 0)
   //{
   //   writeXml(writeFormat,*(fileStruct.getDftSolutionList()),progress);
   //}

   //if (fileStruct.getMachineList() != NULL && fileStruct.getMachineList()->GetCount() > 0)
   //{
   //   writeXml(writeFormat,*(fileStruct.getMachineList()),progress);
   //}
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const FileStruct& fileStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<File num=\"%d\"",fileStruct.getFileNumber());
   writeFormat.writef(" name=\"%s\""    ,convertHtmlMetaCharacters(fileStruct.getName()));
   writeFormat.writef(" refName=\"%s\"" ,convertHtmlMetaCharacters(fileStruct.getRefName()));
   writeFormat.writef(" geomNum=\"%d\"" ,fileStruct.getBlock()->getBlockNumber());

   if (fileStruct.getOriginalBlock() != NULL)
   {
      writeFormat.writef(" origGeomNum=\"%d\"",fileStruct.getOriginalBlock()->getBlockNumber());
   }

   writeFormat.writef(" geomType=\"%d\""             ,fileStruct.getBlockType());
   writeFormat.writef(" x=\"%s\""                 ,floatingPointNumberString(fileStruct.getInsertX()));
   writeFormat.writef(" y=\"%s\""                 ,floatingPointNumberString(fileStruct.getInsertY()));
   writeFormat.writef(" scale=\"%s\""             ,floatingPointNumberString(fileStruct.getScale()));
   writeFormat.writef(" rotation=\"%s\""          ,floatingPointNumberString(fileStruct.getRotationRadians()));
   writeFormat.writef(" mirror=\"%d\""               ,fileStruct.isMirrored());
   writeFormat.writef(" show=\"%d\""                 ,fileStruct.isShown());
   writeFormat.writef(" sourceCAD=\"%d\""            ,fileStruct.getCadSourceFormat());

   writeExtendedXmlAttributes(writeFormat,fileStruct,progress);

   writeFormat.writef(">\n");

   writeFormat.pushHeader(QIndent);

   writeXml(writeFormat, fileStruct.getNetList()        ,progress);
   writeXml(writeFormat, fileStruct.getBusList()        ,progress);
   writeXml(writeFormat, fileStruct.getVariantList()    ,progress);
   writeXml(writeFormat, fileStruct.getTypeList()       ,progress);
   writeXml(writeFormat, fileStruct.getDRCList()        ,progress);
   writeXml(writeFormat, fileStruct.GetViaSpansSection(),progress);
   writeXml(writeFormat, fileStruct.getTableList()      ,progress);
   writeExtendedXml(writeFormat,fileStruct,progress);

   //NetList.WriteXML(writeFormat, progress, doc);
   //m_busList.WriteXML(writeFormat, progress, doc);
   //getVariantList().WriteXML(writeFormat, progress, doc);
   //TypeList.WriteXML(writeFormat, progress, doc);
   //DRCList.WriteXML(writeFormat, progress, doc);
   //getDFTSolutions().WriteXML(writeFormat, progress);

   //if (GetMachineCount() > 0)
   //   m_pMachineList->WriteXML(writeFormat, progress);
   // add data here to write redLine section
   
   writeFormat.popHeader();

   writeFormat.writef("</File>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CNetList& netList,CCamCadFileWriteProgress& progress)
{
   if (netList.GetCount() > 0)
   {
      writeFormat.writef("<Nets>\n");

      writeFormat.pushHeader(QIndent);
      
      for (POSITION netPos = netList.GetHeadPosition();netPos != NULL;)
      {
         progress.incNet();

         NetStruct* net = netList.GetNext(netPos);

         //net->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*net,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</Nets>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CViaSpansSection* ViaSpansSection,CCamCadFileWriteProgress& progress)
{
   if(!ViaSpansSection->getNumOfElements())
      return;
   writeFormat.writef("<ViaSpansSection");
   writeFormat.writef(" enableGeneralClearances=\"%s\"", ViaSpansSection->getGeneralClearance().GetBuffer());
   writeFormat.writef(" enableSameNetClearances=\"%s\"", ViaSpansSection->getSameNetClearance().GetBuffer());
   writeFormat.writef(" useMntOppPadsForBbVias=\"%s\"", ViaSpansSection->getUseMntOppPads().GetBuffer());
   writeFormat.writef(">\n");

   writeFormat.pushHeader(QIndent);

   int size = ViaSpansSection->getNumOfElements();
   for (int i=0; i<size; i++)
   {
      CViaSpan *viaSpan = ViaSpansSection->getElementAt(i);
      writeFormat.writef("<ViaSpan");
      writeFormat.writef(" padstackName=\"%s\"", viaSpan->getPadstackName().GetBuffer());
      writeFormat.writef(" fromLayer=\"%d\"", viaSpan->getFromLayer());
      writeFormat.writef(" toLayer=\"%d\"", viaSpan->getToLayer());
      writeFormat.writef(" viaSpanCapacitance=\"%s\"", viaSpan->getCapacitance().GetBuffer());
      writeFormat.writef(" viaSpanInductance=\"%s\"", viaSpan->getInductance().GetBuffer());
      writeFormat.writef(" viaSpanDelay=\"%s\"", viaSpan->getDelay().GetBuffer());
      writeFormat.writef(" viaSpanGrid=\"%s\"", viaSpan->getGrid().GetBuffer());
      writeFormat.writef(" viaSpanAttributes=\"%s\"", viaSpan->getAttributes().GetBuffer());
      writeFormat.writef(" />\n");
   }
   writeFormat.popHeader();
   writeFormat.writef("</ViaSpansSection>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const NetStruct& netStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Net name=\"%s\"",convertHtmlMetaCharacters(netStruct.getNetName()));
   writeFormat.writef(" entity=\"%ld\"" ,netStruct.getEntityNumber());
   writeFormat.writef(" flag=\"%ld\">\n",netStruct.getFlags());

   writeFormat.pushHeader(QIndent);

   if (netStruct.getAttributes() != NULL)
   {
      //AttribMap->WriteXML(writeFormat, doc);
      writeXml(writeFormat,*(netStruct.getAttributes()),progress);
   }

   //CompPinList.WriteXML(writeFormat, progress, doc);
   writeXml(writeFormat,netStruct.getCompPinList(),progress);

   writeFormat.popHeader();

   writeFormat.writef("</Net>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CCompPinList& compPinList,CCamCadFileWriteProgress& progress)
{
   if (compPinList.getCount() > 0)
   {      
      for (POSITION compPinPos = compPinList.getHeadPosition();compPinPos != NULL;)
      {
         CompPinStruct* compPin = compPinList.getNext(compPinPos);

         //compPin->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*compPin,progress);
      }
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CompPinStruct& compPinStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<CompPin");
   writeFormat.writef(" comp=\"%s\""   ,convertHtmlMetaCharacters(compPinStruct.getRefDes()));
   writeFormat.writef(" pin=\"%s\""    ,convertHtmlMetaCharacters(compPinStruct.getPinName()));
   writeFormat.writef(" entity=\"%ld\"",compPinStruct.getEntityNumber());

   if (compPinStruct.getPinCoordinatesComplete())
   {
      writeFormat.writef(" pinCoords=\"1\"");
      writeFormat.writef(" x=\"%s\""              ,floatingPointNumberString(compPinStruct.getOriginX()));
      writeFormat.writef(" y=\"%s\""              ,floatingPointNumberString(compPinStruct.getOriginY()));
      writeFormat.writef(" rotation=\"%s\""       ,floatingPointNumberString(compPinStruct.getRotationRadians()));
      writeFormat.writef(" mirror=\"%d\""         ,compPinStruct.getMirror());
      writeFormat.writef(" padstackGeomNum=\"%d\"",compPinStruct.getPadstackBlockNumber());

      int visible = compPinStruct.getVisible();

      if (visible < 0)
      {
         int iii = 3;
      }

      writeFormat.writef(" visible=\"%d\""        ,visible);
   }

   writeFormat.writef(">\n");

   if (compPinStruct.getAttributes() != NULL)
   {
      writeFormat.pushHeader("  ");

      writeXml(writeFormat,*(compPinStruct.getAttributes()),progress);

      writeFormat.popHeader();
   }

   writeFormat.writef("</CompPin>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CBusList& busList,CCamCadFileWriteProgress& progress)
{
   if (busList.GetCount() > 0)
   {
      writeFormat.writef("<Busses>\n");

      writeFormat.pushHeader(QIndent);
      
      for (POSITION busPos = busList.GetHeadPosition();busPos != NULL;)
      {
         progress.incBus();

         CBusStruct* bus = busList.GetNext(busPos);

         //bus->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*bus,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</Busses>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CBusStruct& busStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Bus name=\"%s\""  ,convertHtmlMetaCharacters(busStruct.GetName()));
   writeFormat.writef(" entity=\"%ld\">\n",busStruct.GetEntityNumber());

   writeFormat.pushHeader(QIndent);

   // Write the attributes
   //CEntityWithAttributes::WriteXML(writeFormat, doc);

   if (busStruct.getAttributes() != NULL)
   {
      writeXml(writeFormat,*(busStruct.getAttributes()),progress);
   }

   // Write the list of netnames          
   for (POSITION pos = busStruct.GetHeadPositionNetName();pos != NULL;)
   {
      CString netName = busStruct.GetNextNetName(pos);

      if (! netName.IsEmpty())
      {
         writeFormat.writef("<NetRef name=\"%s\"/>\n",convertHtmlMetaCharacters(netName));
      }
   }

   // Write the list of busnames           
   for (POSITION pos = busStruct.GetHeadPositionBusName();pos != NULL;)
   {
      CString busName = busStruct.GetNextBusName(pos);

      if (! busName.IsEmpty())
      {
         writeFormat.writef("<BusRef name=\"%s\"/>\n",convertHtmlMetaCharacters(busName));
      }
   }

   writeFormat.popHeader();

   writeFormat.writef("</Bus>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CVariantList& variantList,CCamCadFileWriteProgress& progress)
{
   if (variantList.GetCount() > 0)
   {
      writeFormat.writef("<Variants>\n");

      writeFormat.pushHeader(QIndent);

      // Save the default variant if there is one
      if (variantList.GetDefaultVariant() != NULL)
      {
         //this->m_pDefaultVariant->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*(variantList.GetDefaultVariant()),progress);
      }

      // Write the variant from the list      
      for (POSITION variantPos = variantList.GetHeadPosition();variantPos != NULL;)
      {
         progress.incVariant();

         CVariant* variant = variantList.GetNext(variantPos);

         //variant->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*variant,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</Variants>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CVariant& variant,CCamCadFileWriteProgress& progress)
{
   if (variant.GetCount() > 0)
   {
      writeFormat.writef("<Variant name=\"%s\">\n",convertHtmlMetaCharacters(variant.GetName()));

      writeFormat.pushHeader(QIndent);
      
      for (POSITION itemPos = variant.GetStartPosition();itemPos != NULL;)
      {
         CVariantItem* variantItem = variant.GetNext(itemPos);

         writeXml(writeFormat,*variantItem,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</Variant>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CVariantItem& variantItem,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<VariantItem name=\"%s\">\n",convertHtmlMetaCharacters(variantItem.GetName()));

   if (variantItem.getAttributes() != NULL)
   {
      writeFormat.pushHeader("  ");

      writeXml(writeFormat,*(variantItem.getAttributes()),progress);

      writeFormat.popHeader();
   }

   writeFormat.writef("</VariantItem>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDeviceTypeDirectory& typeList,CCamCadFileWriteProgress& progress)
{
   if (typeList.GetCount() > 0)
   {
      writeFormat.writef("<Types>\n");

      writeFormat.pushHeader(QIndent);
      
      for (POSITION typePos = typeList.GetHeadPosition();typePos != NULL;)
      {
         progress.incType();

         TypeStruct* typeStruct = typeList.GetNext(typePos);

         //type->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*typeStruct,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</Types>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const TypeStruct& typeStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Type name=\"%s\"" ,convertHtmlMetaCharacters(typeStruct.getName()));
   writeFormat.writef(" entity=\"%ld\""   ,typeStruct.getEntityNumber());
   writeFormat.writef(" geomNum=\"%d\">\n",typeStruct.getBlockNumber());

   if (typeStruct.getAttributes() != NULL)
   {
      writeFormat.pushHeader("  ");

      writeXml(writeFormat,*(typeStruct.getAttributes()),progress);

      writeFormat.popHeader();
   }

   writeFormat.writef("</Type>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDRCList& drcList,CCamCadFileWriteProgress& progress)
{
   if (drcList.GetCount() > 0)
   {
      writeFormat.writef("<DRCs>\n");
      
      writeFormat.pushHeader(QIndent);
      
      for (POSITION drcPos = drcList.GetHeadPosition();drcPos != NULL;)
      {
         progress.incDrc();

         DRCStruct* drcStruct = drcList.GetNext(drcPos);

         //drc->WriteXML(writeFormat, progress, doc);
         writeXml(writeFormat,*drcStruct,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</DRCs>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const DRCStruct& drcStruct,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<DRC string=\"%s\"",convertHtmlMetaCharacters(drcStruct.getString()));
   writeFormat.writef(" entity=\"%ld\""   ,drcStruct.getEntityNumber());
   writeFormat.writef(" x=\"%s\""         ,floatingPointNumberString(drcStruct.getOrigin().x));
   writeFormat.writef(" y=\"%s\""         ,floatingPointNumberString(drcStruct.getOrigin().y));

   if (drcStruct.getPriority() != 0)
   {
      writeFormat.writef(" priority=\"%c\"",(drcStruct.getPriority() == 2 ? 'H' : (drcStruct.getPriority() == 1 ? 'M' : 'L')));
   }

   if (drcStruct.getReviewed() != 0)
   {
      writeFormat.writef(" reviewed=\"%c\"",(drcStruct.getReviewed() ? 'T' : 'F'));
   }

   if (drcStruct.getFailureRange() != 0)
   {
      writeFormat.writef(" failureRange=\"%d\"",drcStruct.getFailureRange());
   }

   writeFormat.writef(" algorithmIndex=\"%d\"",drcStruct.getAlgorithmIndex());

   if (drcStruct.getAlgorithmType() != 0)
   {
      writeFormat.writef(" algorithmType=\"%d\"",drcStruct.getAlgorithmType());
   }

   if (drcStruct.getDrcClass())
   {
      writeFormat.writef(" drcClass=\"%d\"",drcStruct.getDrcClass());
   }

   if (! drcStruct.getComment().IsEmpty())
   {
      writeFormat.writef(" comment=\"%s\"",convertHtmlMetaCharacters(drcStruct.getComment()));
   }

   if (drcStruct.getInsertEntityNumber() != 0)
   {
      writeFormat.writef(" insertEntity=\"%ld\"",drcStruct.getInsertEntityNumber());
   }

   writeFormat.writef(">\n");

   writeFormat.pushHeader("  ");

   switch (drcStruct.getDrcClass())
   {
   case DRC_CLASS_SIMPLE:
   case DRC_CLASS_NO_MARKER:
      break;
   case DRC_CLASS_MEASURE:
      if (drcStruct.getVoidPtr() != NULL)
      {
         DRC_MeasureStruct* meas = (DRC_MeasureStruct*)drcStruct.getVoidPtr();

         writeFormat.writef("<DRC_Measure");
         writeFormat.writef(" entity1=\"%ld\"",meas->entity1);
         writeFormat.writef(" entity2=\"%ld\"",meas->entity2);
         writeFormat.writef(" x1=\"%s\""      ,floatingPointNumberString(meas->x1));
         writeFormat.writef(" y1=\"%s\""      ,floatingPointNumberString(meas->y1));
         writeFormat.writef(" x2=\"%s\""      ,floatingPointNumberString(meas->x2));
         writeFormat.writef(" y2=\"%s\"/>\n"  ,floatingPointNumberString(meas->y2));
      }

      break;
   case DRC_CLASS_NETS:
      if (drcStruct.getVoidPtr() != NULL)
      {
         CStringList* list = (CStringList*)drcStruct.getVoidPtr();
         
         for (POSITION pos = list->GetHeadPosition();pos != NULL;)
         {
            CString netname = list->GetNext(pos);

            writeFormat.writef("<DRC_Net netname=\"%s\"/>\n", convertHtmlMetaCharacters(netname));
         }
      }
      break;
   }

   if (drcStruct.getAttributes() != NULL)
   {
      writeXml(writeFormat,*(drcStruct.getAttributes()),progress);
   }

   writeFormat.popHeader();

   writeFormat.writef("</DRC>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDcaDftSolutionList& dftSolutionList,CCamCadFileWriteProgress& progress)
{
   if (dftSolutionList.GetCount() > 0)
   {
      writeFormat.writef("<DFTSolutions>\n");

      writeFormat.pushHeader(QIndent);
      
      for (POSITION pos = dftSolutionList.GetHeadPosition();pos != NULL;)
      {
         CDcaDftSolution* dftSolution = dftSolutionList.GetNext(pos);

         //pSolution->WriteXML(writeFormat, progress); 
         writeXml(writeFormat,*dftSolution,progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</DFTSolutions>\n");
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDcaDftSolution& dftSolution,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<DFTSolution name=\"%s\">\n", convertHtmlMetaCharacters(dftSolution.GetName()));

   writeFormat.pushHeader(QIndent);

   //m_TestPlan.WriteXML(writeFormat, progress);
   writeXml(writeFormat,dftSolution.getTestPlan(),progress);

   if (dftSolution.getAccessAnalysisSolution() != NULL)
   {
      //m_aaSolution->WriteXML(writeFormat, progress);
      writeXml(writeFormat,*(dftSolution.getAccessAnalysisSolution()),progress);
   }

   if (dftSolution.getProbePlacementSolution() != NULL)
   {
      //m_ppSolution->WriteXML(writeFormat, progress);
      writeXml(writeFormat,*(dftSolution.getProbePlacementSolution()),progress);
   }

   writeFormat.popHeader();

   writeFormat.writef("</DFTSolution>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDcaTestPlan& testPlan,CCamCadFileWriteProgress& progress)
{
   // Re decimals, DR dts0100500371
   // The decimals settings is a user's GUI setting, the whole point of DCA is to be independant of the
   // CCEtoODBDoc and the GUI. We should perhaps add decimals resolution to the data we keep in the doc, as
   // we do the pageUnits, but it is not there at the moment.
   // So for output here we want all the decimals. CAMCAD allows the user to have up to 6 decimal places,
   // so that is the format we will write, even if user's current GUI setting is less.
   int decimals = 6;

   writeFormat.writef("<DFTTestplan>\n");

#ifdef ImplementCCamCadFileWriterWriteXml

   CString buf;

   writeFormat.pushHeader("  ");
   {
      switch (m_iSurface)
      {
      case 0:  // Top
         buf = "Top";
         break;
      case 1:  // Bottom
         buf = "Bottom";
         break;
      case 2:  // Both
         buf = "Both";
         break;
      }

      writeFormat.writef("<AccessAnalysis Surface=\"%s\"", buf);

      // Package Outline
      writeFormat.writef(" RectangularOutline=\"%s\"", m_bEnableRectangularOutline?"True":"False");
      writeFormat.writef(" OverwriteOutline=\"%s\"", m_bEnableOverwriteOutline?"True":"False");
      writeFormat.writef(" IncludeSinglePinComp=\"%s\"", m_bIncludeSinglePinComp?"True":"False");

      // Option
      switch (m_iTesterType)
      {
      case 0:  // Fixture
         buf = "Fixture";
         break;
      case 1:  // Fixtureless
         buf = "Fixtureless";
         break;
      }  

      writeFormat.writef(" TesterType=\"%s\"", buf);
      writeFormat.writef(" MultiPinNets=\"%s\"", m_bIncludeMulPinNet?"True":"False");
      writeFormat.writef(" SinglePinNets=\"%s\"", m_bIncludeSngPinNet?"True":"False");
      writeFormat.writef(" UnconnectedNets=\"%s\"", m_bIncludeNCNet?"True":"False");
      writeFormat.writef(" AccessOffset=\"%s\"", m_accessOffsetOptions.getEnableOffsetFlag()?"True":"False");

      writeFormat.writef(" ViaSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).getEnableSmdOffsetFlagString());
      writeFormat.writef(" ViaThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeVia).getEnableThOffsetString());

      writeFormat.writef(" PinSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getEnableSmdOffsetString());
      writeFormat.writef(" PinThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypePin).getEnableThOffsetString());

      writeFormat.writef(" TpSmdOffset=\"%s\"", m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp).getEnableSmdOffsetFlagString());
      writeFormat.writef(" TpThOffset=\"%s\"" , m_accessOffsetOptions.getAccessOffsetItem(accessItemTypeTp).getEnableThOffsetString());

      writeFormat.writef(" WriteIgnoredSurface=\"%s\"", m_bAAWriteIgnoredSurface?"True":"False");
      writeFormat.writef(" CreateSinglePinNets=\"%s\">\n", m_bCreateNCNet?"True":"False");

      writeFormat.pushHeader("  ");
      {
         // Height Analysis
         writeFormat.writef("<HeightAnalysis UseTop=\"%s\" UseBottom=\"%s\">\n", 
            m_heightAnalysisTop.IsUse()?"True":"False", m_heightAnalysisBot.IsUse()?"True":"False");
         {
            writeFormat.pushHeader("  ");

            //Top Height Analysis
            POSITION pos = m_heightAnalysisTop.GetStartPosition();
            while (pos != NULL)
            {
               CHeightRange* range = m_heightAnalysisTop.GetNext(pos);

               if (range == NULL)
                  continue;

               writeFormat.writef("<HeightRange Surface=\"Top\"");
               writeFormat.writef(" Name=\"%s\"", range->GetName());
               writeFormat.writef(" PackageOutlineDistance=\"%.*f\"", decimals, range->GetOutlineDistance());
               writeFormat.writef(" MinHeight=\"%.*f\"", decimals, range->GetMinHeight());
               writeFormat.writef(" MaxHeight=\"%.*f\"/>\n", decimals, range->GetMaxHeight());
            }

            //Bottom Height Analysis
            pos = m_heightAnalysisBot.GetStartPosition();
            while (pos != NULL)
            {
               CHeightRange* range = m_heightAnalysisBot.GetNext(pos);

               if (range == NULL)
                  continue;

               writeFormat.writef("<HeightRange Surface=\"Bottom\"");
               writeFormat.writef(" Name=\"%s\"", range->GetName());
               writeFormat.writef(" PackageOutlineDistance=\"%.*f\"", decimals, range->GetOutlineDistance());
               writeFormat.writef(" MinHeight=\"%.*f\"", decimals, range->GetMinHeight());
               writeFormat.writef(" MaxHeight=\"%.*f\"/>\n", decimals, range->GetMaxHeight());
            }

            writeFormat.popHeader();
         }

         writeFormat.writef("</HeightAnalysis>\n");

         // Physical Constraints
         writeFormat.writef("<PhysicalConstraints EnableSoldermask=\"%s\"", m_bEnableSolermaskAnalysis?"True":"False");
         {
            switch (m_iOutlineToUse)
            {
               case -1: // None
                  buf = "None";
                  break;
               case 0:  // Real Part outline
                  buf = "Real Part";
                  break;
               case 1:  // DFT outline
                  buf = "DFT";
                  break;
               case 2:  // Both
                  buf = "Both";
                  break;
            }

            writeFormat.writef(" OutlineToUse=\"%s\"", buf);

            switch (m_iOutlinePriority)
            {
               case 0:  // Real Part outline
                  buf = "Real Part outline";
                  break;
               case 1:  // DFT outline
                  buf = "DFT outline";
                  break;
            }  

            writeFormat.writef(" OutlinePriority=\"%s\"", buf);
            writeFormat.writef(" Ignore1PinCompOutline=\"%s\"", m_bIgnore1PinCompOutline?"True":"False");
            writeFormat.writef(" IgnoreUnloadedCompOutline=\"%s\">\n", m_bIgnoreUnloadedCompOutline?"True":"False");

            writeFormat.pushHeader("  ");
            writeFormat.writef("<BoardOutline TopEnabled=\"%s\" TopDistance=\"%.*f\" BottomEnabled=\"%s\" BottomDistance=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableBoardOutline()?"True":"False", decimals, m_constraintsTop.GetValueBoardOutline(),
               m_constraintsBot.GetEnableBoardOutline()?"True":"False", decimals, m_constraintsBot.GetValueBoardOutline());
            writeFormat.writef("<ComponentOutline TopEnabled=\"%s\" TopDistance=\"%.*f\" BottomEnabled=\"%s\" BottomDistance=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableCompOutline()?"True":"False", decimals, m_constraintsTop.GetValueCompOutline(),
               m_constraintsBot.GetEnableCompOutline()?"True":"False", decimals, m_constraintsBot.GetValueCompOutline());
            writeFormat.writef("<FeatureSize TopEnabled=\"%s\" TopSize=\"%.*f\" BottomEnabled=\"%s\" BottomSize=\"%.*f\"/>\n",
               m_constraintsTop.GetEnableFeatureSize()?"True":"False", decimals, m_constraintsTop.GetValueFeatureSize(),
               m_constraintsBot.GetEnableFeatureSize()?"True":"False", decimals, m_constraintsBot.GetValueFeatureSize());
            writeFormat.popHeader();
         }

         writeFormat.writef("</PhysicalConstraints>\n");

         // Target Types
         writeFormat.writef("<TargetType>\n");
         {
            writeFormat.pushHeader("  ");
            writeFormat.writef("<TargetTestAttribute Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableTestAttrib()?"True":"False", m_targetTypesBot.GetEnableTestAttrib()?"True":"False");
            writeFormat.writef("<TargetVia Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableVia()?"True":"False", m_targetTypesBot.GetEnableVia()?"True":"False");
            writeFormat.writef("<TargetConnector Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableConnector()?"True":"False", m_targetTypesBot.GetEnableConnector()?"True":"False");
            writeFormat.writef("<TargetSMD Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableSMD()?"True":"False", m_targetTypesBot.GetEnableSMD()?"True":"False");
            writeFormat.writef("<TargetSinglePinSMD Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableSinglePinSMD()?"True":"False", m_targetTypesBot.GetEnableSinglePinSMD()?"True":"False");
            writeFormat.writef("<TargetThroughHole Top=\"%s\" Bottom=\"%s\"/>\n",
               m_targetTypesTop.GetEnableTHUR()?"True":"False", m_targetTypesBot.GetEnableTHUR()?"True":"False");
            writeFormat.writef("<TargetCadPadstacks Top=\"%s\" Bottom=\"%s\">\n",
               m_targetTypesTop.GetEnableCADPadstack()?"True":"False", m_targetTypesBot.GetEnableCADPadstack()?"True":"False");
            
            writeFormat.pushHeader("  ");

            POSITION pos = m_targetTypesTop.GetPadstackHeadPosition();
            while (pos)
            {
               int padStack = m_targetTypesTop.GetPadstackNext(pos);

               // TODO: get padstack name (need the doc)
               writeFormat.writef("<CadPadstack Surface=\"Top\" Name=\"%d\"/>\n", padStack);
            }

            pos = m_targetTypesBot.GetPadstackHeadPosition();
            while (pos)
            {
               int padStack = m_targetTypesBot.GetPadstackNext(pos);

               // TODO: get padstack name (need the doc)
               writeFormat.writef("<CadPadstack Surface=\"Bottom\" Name=\"%d\"/>\n", padStack);
            }

            writeFormat.popHeader();

            writeFormat.writef("</TargetCadPadstacks>\n");
            writeFormat.popHeader();
         }

         writeFormat.writef("</TargetType>\n");
      }

      writeFormat.popHeader();

      writeFormat.writef("</AccessAnalysis>\n");
   }

   {
      writeFormat.writef("<ProbePlacement OutlineToUse=\"%d\"", m_iProbeStartNumber);
      writeFormat.writef(" EnableDoubleWiring=\"%s\"", m_bAllowDoubleWiring?"True":"False");
      writeFormat.writef(" PlaceOnAllAccessible=\"%s\"", m_bPlaceOnAllAccessible?"True":"False");

      switch (m_eProbeSide)
      {
      case 0:  // Top
         buf = "Top";
         break;
      case 1:  // Bottom
         buf = "Bottom";
         break;
      case 2:  // Both
         buf = "Both";
         break;
      }

      writeFormat.writef(" WriteIgnoredSurface=\"%s\"", m_bPPWriteIgnoredSurface?"True":"False");
      writeFormat.writef(" NetConditionCaseSensitive=\"%s\"", m_bNetConditionCaseSensitive?"True":"False");
      writeFormat.writef(" Surface=\"%s\">\n", buf);

      writeFormat.pushHeader("  ");
      {
         m_targetPriority.WriteXML(writeFormat/*, progress*/);
         m_probeTemplates.WriteXML(writeFormat/*, progress*/);

         writeFormat.writef("<ProbeRefname NeverMirror=\"%s\"/>\n", m_bNeverMirrorRefname?"True":"False");

         writeFormat.writef("<KelvinAnalysis ResEnable=\"%s\"", m_bUseResistor?"True":"False");
         writeFormat.writef(" ResValue=\"%0.3f\"", m_dResistorValue);
         writeFormat.writef(" CapEnable=\"%s\"", m_bUseCapacitor?"True":"False");
         writeFormat.writef(" CapValue=\"%0.3f\"", m_dCapacitorValue);
         writeFormat.writef(" InductorEnable=\"%s\"/>\n", m_bUseInductor?"True":"False");

         writeFormat.writef("<PowerInjection Usage=\"%d\" Value=\"%d\"/>\n", m_iPowerInjectionUsage, m_iPowerInjectionValue);

         m_preconditions.WriteXML(writeFormat, progress);
         m_netConditions.WriteXML(writeFormat, progress);
      }

      writeFormat.popHeader();

      writeFormat.writef("</ProbePlacement>\n");
   }

   writeFormat.popHeader();

#endif

   writeFormat.writef("</DFTTestplan>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDcaAccessAnalysisSolution& accessAnalysisSolution,CCamCadFileWriteProgress& progress)
{
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDcaProbePlacementSolution& probePlacementSolution,CCamCadFileWriteProgress& progress)
{
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CDcaMachineList& machineList,CCamCadFileWriteProgress& progress)
{
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CNamedViewList& namedViewList,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef( "<NamedViews>\n");

   writeFormat.pushHeader(QIndent);

   for (POSITION namedViewPos = namedViewList.GetHeadPosition();namedViewPos != NULL;)
   {
      CNamedView* namedView = namedViewList.GetNext(namedViewPos);

      //namedView->WriteXML(writeFormat);
      writeXml(writeFormat,*namedView,progress);
   }

   writeFormat.popHeader();

   writeFormat.writef("</NamedViews>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CNamedView& namedView,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<NamedView name=\"%s\"",convertHtmlMetaCharacters(namedView.getName()));
   writeFormat.writef(" scaleNum=\"%d\""      ,namedView.getScaleNum());
   writeFormat.writef(" scaleDenom=\"%s\""    ,floatingPointNumberString(namedView.getScaleDenom()));
   writeFormat.writef(" scrollX=\"%d\""       ,namedView.getScrollX());
   writeFormat.writef(" scrollY=\"%d\""       ,namedView.getScrollY());
   writeFormat.writef(" layerCount=\"%d\">\n" ,namedView.getLayerCount());

   writeFormat.pushHeader(QIndent);

   writeXml(writeFormat,namedView.getViewLayerDataArray(),progress);

   writeFormat.popHeader();

   writeFormat.writef("</NamedView>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CViewLayerDataArray& viewLayerDataArray,CCamCadFileWriteProgress& progress)
{
   for (int layerIndex=0;layerIndex < viewLayerDataArray.GetSize();layerIndex++)
   {
      CViewLayerData* viewLayerData = viewLayerDataArray.GetAt(layerIndex);

      if (viewLayerData != NULL)
      {
         writeFormat.writef("<NamedViewLayer num=\"%d\"",layerIndex);
         writeFormat.writef(" color=\"%ld\""            ,viewLayerData->getColor());
         writeFormat.writef(" show=\"%d\"/>\n"          ,viewLayerData->getShow());
      }
   }
}
void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CFontProperties& fontProps,CCamCadFileWriteProgress& progress)
{
   // This adds to an existing clause/scope in XML.
   // It does not start a scope or end one, no indenting, no tabs, etc.
   // Just writes the fields out.

   // facename="VeriBest Gerber 0" fonttype="2" weight="400" underlined="0" strikeout="0" orientation="0" 
   // charset="0" pitchAndFamily="0"

   writeFormat.writef(" facename=\"%s\""           ,convertHtmlMetaCharacters(fontProps.GetFaceName()));
   writeFormat.writef(" fonttype=\"%s\""           ,convertHtmlMetaCharacters(fontProps.GetFontType()));
   writeFormat.writef(" weight=\"%s\""             ,convertHtmlMetaCharacters(fontProps.GetWeight()));
   writeFormat.writef(" underlined=\"%s\""         ,convertHtmlMetaCharacters(fontProps.GetUnderlined()));
   writeFormat.writef(" strikeout=\"%s\""          ,convertHtmlMetaCharacters(fontProps.GetStrikeOut()));
   writeFormat.writef(" orientation=\"%s\""        ,convertHtmlMetaCharacters(fontProps.GetOrientation()));
   writeFormat.writef(" charset=\"%s\""            ,convertHtmlMetaCharacters(fontProps.GetCharSet()));
   writeFormat.writef(" pitchAndFamily=\"%s\""     ,convertHtmlMetaCharacters(fontProps.GetPitchAndFamily()));
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CAttributes& attributes,CCamCadFileWriteProgress& progress)
{
   writeFormat.pushHeader(QIndent);

   CSortedAttributeList sortedAttributeList(attributes);

   for (POSITION listPos = sortedAttributeList.getHeadPosition();listPos != NULL;)
   {
      CSortedAttributeListEntry* sortedAttributeListEntry = sortedAttributeList.getNext(listPos);

      int keywordIndex = sortedAttributeListEntry->getKeywordIndex();

      for (POSITION pos = sortedAttributeListEntry->getHeadPosition();pos != NULL;)
      {
         CAttribute* pAttrib = sortedAttributeListEntry->getNext(pos);

         AttribIterator attribIterator(pAttrib);
         CAttribute attrib(pAttrib->getCamCadData());

         while (attribIterator.getNext(attrib))
         {
            switch (attrib.getValueType())
            {
            case valueTypeString:
            case valueTypeEmailAddress:
            case valueTypeWebAddress:
               {
                  CString buf = convertHtmlMetaCharacters(getCamCadData().getAttributeStringValue(attrib));

#ifdef EnableStringAttributeValueSplitting
                  int foundPos = buf.Find('\n');

                  while (foundPos != -1)
                  {
                     writeFormat.writef("<Attrib key=\"%d\" val=\"%s\"/>\n",keywordIndex,buf.Left(foundPos));

                     buf = buf.Right(buf.GetLength() - foundPos - 1);

                     foundPos = buf.Find('\n');
                  }
#else
                  // Change escape from XML style to C-Style as text. 
                  CDcaXmlContent::SwapEscapeSequences(buf, false);
#endif

                  writeFormat.writef("<Attrib key=\"%d\" val=\"%s\"",keywordIndex, buf);

                  if (attrib.getFlags() != 0)
                  {
                     writeFormat.writef(" flag=\"%ld\"",attrib.getFlags());
                  }

                  if (attrib.isInherited())
                  {
                     writeFormat.writef(" inherited=\"%d\"",attrib.isInherited());
                  }

                  // ALVIN - Case 1300 - Only write if something in the attribute has been changed, meaning it may have been visible in the past
                  if (attrib.isVisible() || 
                     fabs(attrib.getX()) > SMALLNUMBER || 
                     fabs(attrib.getY()) > SMALLNUMBER ||
                     fabs(attrib.getHeight()) > SMALLNUMBER || 
                     fabs(attrib.getWidth()) > SMALLNUMBER || 
                     attrib.getPenWidthIndex() > 0 ||
                     fabs(attrib.getRotationRadians()) > SMALLNUMBER || 
                     attrib.getLayerIndex() > 0 || 
                     attrib.getMirrorDisabled() ||
                     attrib.getMirrorFlip() ||
                     attrib.getHorizontalPosition() != horizontalPositionLeft || 
                     attrib.getVerticalPosition() != verticalPositionBaseline)
                  {
                     // TOM - Case 1207 - Now we always write visibility info for string types
                     writeFormat.writef(" visible=\"%d\""      ,(int)attrib.isVisible());
                     writeFormat.writef(" x=\"%s\""            ,floatingPointNumberString(attrib.getX()));
                     writeFormat.writef(" y=\"%s\""            ,floatingPointNumberString(attrib.getY()));
                     writeFormat.writef(" height=\"%s\""       ,floatingPointNumberString(attrib.getHeight()));
                     writeFormat.writef(" width=\"%s\""        ,floatingPointNumberString(attrib.getWidth()));
                     writeFormat.writef(" penWidthIndex=\"%d\"",attrib.getPenWidthIndex());
                     writeFormat.writef(" proportional=\"%d\"" ,(int)attrib.isProportionallySpaced());
                     writeFormat.writef(" rotation=\"%s\""     ,floatingPointNumberString(attrib.getRotationRadians()));
                     writeFormat.writef(" layer=\"%d\""        ,attrib.getLayerIndex());
                     writeFormat.writef(" neverMirror=\"%d\""  ,attrib.getMirrorDisabled());
                     writeFormat.writef(" mirror=\"%d\""       ,attrib.getMirrorFlip());  // Only supports mirror flip on purpose, no mirror layer because attribs already have specific layer assignment
                     writeFormat.writef(" textAlignment=\"%d\"",attrib.getHorizontalPosition());
                     writeFormat.writef(" lineAlignment=\"%d\"",attrib.getVerticalPosition());

                     if (attrib.GetFontProperties() != NULL)
                        writeXml(writeFormat, *attrib.GetFontProperties(), progress);
                  }

                  writeFormat.writef("/>\n");
               }

               break;
            case valueTypeInteger:
               writeFormat.writef("<Attrib key=\"%d\" val=\"%d\"/>\n",keywordIndex,attrib.getIntValue());
               break;

            case valueTypeDouble:
            case valueTypeUnitDouble:
               writeFormat.writef("<Attrib key=\"%d\" val=\"%s\"/>\n",keywordIndex,floatingPointNumberString(attrib.getDoubleValue()));
               break;

            case valueTypeNone:
            default:
               writeFormat.writef("<Attrib key=\"%d\"/>\n",keywordIndex);
               break;
            }
         }
      }
   }

   writeFormat.popHeader();
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CTopicsList& topiclist,CCamCadFileWriteProgress& progress)
{
   CTime t;
   CString m_time;
   t = t.GetCurrentTime();
   int hour = t.GetHour() - ((t.GetHour() > 12)? 12: 0);
   char *timezone = (t.GetHour() > 11)?"PM":"AM";
   
   m_time.Format("%d/%d/%d %d:%02d:%02d %s",
      t.GetMonth(),t.GetDay(),t.GetYear(),hour,t.GetMinute(),t.GetSecond(),timezone);
   
   double m_unitFactor = getUnitsFactor(getCamCadData().getTopicsList().getPageUnits(),getCamCadData().getPageUnits());

   //Topics 
   writeFormat.writef("<Topics>\n");
   writeFormat.pushHeader("  ");

   for(int i = 0 ; i < topiclist.GetCount(); i++)
   {
      //Topic
      CTopic *topicnode = topiclist.GetAt(i);
      if(!topicnode) continue;

      if(topicnode->GetCount() == 0)
      {
         writeFormat.writef("<Topic title=\"%s\" user=\"%s\" dateTime=\"%s\" />\n",
            topicnode->getTitle(),topicnode->getUser(),m_time);
      }
      else // no Issues
      {
         writeFormat.writef("<Topic title=\"%s\" user=\"%s\" dateTime=\"%s\">\n",
            topicnode->getTitle(),topicnode->getUser(),m_time);
         writeFormat.pushHeader("  ");

         for(int j = 0; j < topicnode->GetCount(); j++)
         {
            //Issue
            CIssue *issuenode = topicnode->GetAt(j);        
            if(!issuenode) continue;

            char* GeneralIssueStr = (issuenode->IsGeneralIssue())?"isGeneralIssue=\"1\"":"";
            
            if(issuenode->GetCount() == 0)
            {
               writeFormat.writef("<Issue title=\"%s\" user=\"%s\" dateTime=\"%s\" %s />\n",
                  issuenode->getTitle(),issuenode->getUser(),m_time,GeneralIssueStr);
            }
            else // no Markups
            {
               writeFormat.writef("<Issue title=\"%s\" user=\"%s\" dateTime=\"%s\" %s>\n",
                  issuenode->getTitle(),issuenode->getUser(),m_time,GeneralIssueStr);
               writeFormat.pushHeader("  ");
               //Markups
               writeFormat.writef("<Markups>\n");            
               writeFormat.pushHeader("  ");

               for(int k = 0; k < issuenode->GetCount(); k++)
               {
                  CTopicNodeTemplate *topicnode = issuenode->GetAt(k); 
                  if(topicnode)                                         
                  {
                     double m_left = m_unitFactor * topicnode->getNoteLeft();
                     double m_right = m_unitFactor * topicnode->getNoteRight();
                     double m_top = m_unitFactor * topicnode->getNoteTop();
                     double m_bottom = m_unitFactor * topicnode->getNoteBottom();
                     double m_x1 = m_unitFactor * topicnode->getx1();
                     double m_y1 = m_unitFactor * topicnode->gety1();
                     double m_x2 = m_unitFactor * topicnode->getx2();
                     double m_y2 = m_unitFactor * topicnode->gety2();                     

                     switch(topicnode->getNodeType())
                     {
                     case T_StickNote:
                        writeFormat.writef("<StickyNote left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                           m_left,m_right,m_top,m_bottom);
                        writeFormat.writef("text=\"%s\" OverideStickyColor=\"%i\" StickyBackColor=\"%i\" title=\"%s\" ",
                           topicnode->getText(),topicnode->getOverideStickyColor(),topicnode->getStickyBackColor(),topicnode->getTitle());
                        writeFormat.writef("user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_FreeText:
                        writeFormat.writef("<FreeText left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                          m_left,m_right,m_top,m_bottom); 
                        writeFormat.writef("text=\"%s\" title=\"%s\" user=\"%s\" dateTime=\"%s\" ",
                           topicnode->getText(),topicnode->getTitle(),topicnode->getUser(),m_time);
                        writeFormat.writef("color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_Line:
                        writeFormat.writef("<Line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" ",
                           m_x1,m_y1,m_x2,m_y2);
                        writeFormat.writef("EndArrow=\"%i\" StartArrow=\"%i\" OverideArrow=\"%i\" OverideColor=\"%i\" OverideThickness=\"%i\" ",
                           topicnode->getEndArrow(),topicnode->getStartArrow(),topicnode->getOverideArrow(),topicnode->getOverideColor(),topicnode->getOverideThickness());
                        writeFormat.writef("OverideThicknessSize=\"%f\" ItemColor=\"%i\" title=\"%s\" user=\"%s\" dateTime=\"%s\" ",
                           topicnode->getOverideThicknessSize(),topicnode->getItemColor(),topicnode->getTitle(),topicnode->getUser(),m_time);
                        writeFormat.writef("color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_Polyline:
                        if(topicnode->getPointList().GetCount() == 0)
                        {
                           writeFormat.writef("<Polyline title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" ",
                              topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                           writeFormat.writef("EndArrow=\"%i\" StartArrow=\"%i\" OverideArrow=\"%i\" OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" />\n",
                              topicnode->getEndArrow(),topicnode->getStartArrow(),topicnode->getOverideArrow(),topicnode->getOverideColor(),topicnode->getOverideThickness(),
                              topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        }
                        else
                        {
                           writeFormat.writef("<Polyline title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" ",
                              topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                           writeFormat.writef("EndArrow=\"%i\" StartArrow=\"%i\" OverideArrow=\"%i\" OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\">\n",
                              topicnode->getEndArrow(),topicnode->getStartArrow(),topicnode->getOverideArrow(),topicnode->getOverideColor(),topicnode->getOverideThickness(),
                              topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                           writeFormat.pushHeader("  ");

                           for (POSITION pos = topicnode->getPointList().GetHeadPosition();pos != NULL;)
                           {
                              CPnt* current_pnt = topicnode->getPointList().GetNext(pos);
                              if(current_pnt)
                              {
                                 double x = m_unitFactor * current_pnt->x;
                                 double y = m_unitFactor * current_pnt->y;
                                 writeFormat.writef("<Pnt x=\"%f\" y=\"%f\" />\n",x,y);
                              }
                           }                           

                           writeFormat.popHeader();
                           writeFormat.writef("</Polyline>\n");
                        }
                        break;
                     case T_FreehandDraw:
                        if(topicnode->getPointList().GetCount() == 0)
                        {
                           writeFormat.writef("<FreehandDraw title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" ",
                              topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                           writeFormat.writef("EndArrow=\"%i\" StartArrow=\"%i\" OverideArrow=\"%i\" OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" />\n",
                              topicnode->getEndArrow(),topicnode->getStartArrow(),topicnode->getOverideArrow(),topicnode->getOverideColor(),topicnode->getOverideThickness(),
                              topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        }
                        else
                        {
                           writeFormat.writef("<FreehandDraw title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" ",
                              topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                           writeFormat.writef("EndArrow=\"%i\" StartArrow=\"%i\" OverideArrow=\"%i\" OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\">\n",
                              topicnode->getEndArrow(),topicnode->getStartArrow(),topicnode->getOverideArrow(),topicnode->getOverideColor(),topicnode->getOverideThickness(),
                              topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                           writeFormat.pushHeader("  ");
                           
                           for (POSITION pos = topicnode->getPointList().GetHeadPosition();pos != NULL;)
                           {
                              CPnt* current_pnt = topicnode->getPointList().GetNext(pos);
                              if(current_pnt)
                              {
                                 double x = m_unitFactor * current_pnt->x;
                                 double y = m_unitFactor * current_pnt->y;
                                 writeFormat.writef("<Pnt x=\"%f\" y=\"%f\" />\n",x,y);
                              }
                           }                                 
                           writeFormat.popHeader();
                           writeFormat.writef("</FreehandDraw>\n");
                        }
                        break;
                     case T_Rectangle:
                        writeFormat.writef("<Rectangle left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                            m_left,m_right,m_top,m_bottom); 
                        writeFormat.writef("OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" ",
                            topicnode->getOverideColor(),topicnode->getOverideThickness(),topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        writeFormat.writef("title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_XOutRectangle:
                        writeFormat.writef("<XOutRectangle left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                            m_left,m_right,m_top,m_bottom); 
                        writeFormat.writef("OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" ",
                            topicnode->getOverideColor(),topicnode->getOverideThickness(),topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        writeFormat.writef("title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_Ellipse:
                        writeFormat.writef("<Ellipse left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                            m_left,m_right,m_top,m_bottom); 
                        writeFormat.writef("OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" ",
                            topicnode->getOverideColor(),topicnode->getOverideThickness(),topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        writeFormat.writef("title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_XOutEllipse:
                        writeFormat.writef("<XOutEllipse left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                            m_left,m_right,m_top,m_bottom); 
                        writeFormat.writef("OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" ",
                            topicnode->getOverideColor(),topicnode->getOverideThickness(),topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        writeFormat.writef("title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_Cloud:
                        writeFormat.writef("<Cloud left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                            m_left,m_right,m_top,m_bottom); 
                        writeFormat.writef("OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" ",
                            topicnode->getOverideColor(),topicnode->getOverideThickness(),topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        writeFormat.writef("title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_Dimension:
                        writeFormat.writef("<Dimension x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" ",
                           m_x1,m_y1,m_x2,m_y2);
                        writeFormat.writef("OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" ",
                            topicnode->getOverideColor(),topicnode->getOverideThickness(),topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        writeFormat.writef("title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     case T_LeaderNote:
                        writeFormat.writef("<LeaderNote x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" ",
                           m_x1,m_y1,m_x2,m_y2);
                        writeFormat.writef("left=\"%f\" right=\"%f\" top=\"%f\" bottom=\"%f\" ",
                            m_left,m_right,m_top,m_bottom); 
                        writeFormat.writef("text=\"%s\" OverideColor=\"%i\" OverideThickness=\"%i\" OverideThicknessSize=\"%f\" ItemColor=\"%i\" ",
                            topicnode->getText(),topicnode->getOverideColor(),topicnode->getOverideThickness(),topicnode->getOverideThicknessSize(),topicnode->getItemColor());
                        writeFormat.writef("title=\"%s\" user=\"%s\" dateTime=\"%s\" color=\"%i\" visible=\"%i\" geometryName=\"%s\" />\n",
                           topicnode->getTitle(),topicnode->getUser(),m_time,topicnode->getColor(),topicnode->getVisible(),topicnode->getGeometryName());
                        break;
                     }/*switch*/
                  }/*if*/
               }/*for StickyNote*/

               writeFormat.popHeader();
               writeFormat.writef("</Markups>\n");   

               writeFormat.popHeader();
               writeFormat.writef("</Issue>\n");     
            }/*if*/

         }/*for Issue*/
         
         writeFormat.popHeader();
         writeFormat.writef("</Topic>\n");
      }/*if*/

   }/*for Topic*/

   writeFormat.popHeader();
   writeFormat.writef("</Topics>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CPoly3DList& polyList,CCamCadFileWriteProgress& progress)
{   
   for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
   {
      const CPoly3D* poly = polyList.GetNext(polyPos);

      writeXml(writeFormat,*poly,progress);
   }
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CPoly3D& poly,CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Poly3D");
   writeFormat.writef(" widthIndex=\"%d\"",poly.getWidthIndex());

   if (poly.isClosed())
   {
      writeFormat.writef(" closed=\"1\"");
   }

   if (poly.isFilled())
   {
      writeFormat.writef(" filled=\"1\"");
   }

   if (poly.isVoid())
   {
      writeFormat.writef(" void=\"1\"");
   }

   if (poly.isThermalLine())
   {
      writeFormat.writef(" thermal=\"1\"");
   }

   if (poly.isFloodBoundary())
   {
      writeFormat.writef(" boundary=\"1\"");
   }

   if (poly.isHidden())
   {
      writeFormat.writef(" hidden=\"1\"");
   }

   writeFormat.writef(">\n");

   // loop points of poly
   writeFormat.pushHeader(QIndent);

   //list.WriteXML(writeFormat, progress);
   writeXml(writeFormat,poly.getPntList(),progress);

   writeFormat.popHeader();

   writeFormat.writef("</Poly3D>\n");
}

void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CPnt3DList& pntList,CCamCadFileWriteProgress& progress)
{   
   for (POSITION pntPos = pntList.GetHeadPosition();pntPos != NULL;)
   {
      CPnt3D* pnt = pntList.GetNext(pntPos);

      writeFormat.writef("<Pnt3D");
      writeFormat.writef(" x=\"%1s\" y=\"%s\" z=\"%s\"",floatingPointNumberString(pnt->x),floatingPointNumberString(pnt->y),floatingPointNumberString(pnt->z));
      
      writeFormat.writef("/>\n");
   }
}

//void CDcaCamCadFileWriter::writeXml(CWriteFormat& writeFormat,const CNamedViewList& namedViewList,CCamCadFileWriteProgress& progress)
//{
//
//}

//_____________________________________________________________________________
CCamCadFileWriteProgress::CCamCadFileWriteProgress(CCamCadData& camCadData,bool showProgressFlag)
: m_camCadData(camCadData)
{
   init();

   m_allocatedProgress = (showProgressFlag ? new CDcaOperationProgress() : NULL);
   m_progress = m_allocatedProgress;
}

CCamCadFileWriteProgress::CCamCadFileWriteProgress(CCamCadData& camCadData,CDcaOperationProgress* operationProgress)
: m_camCadData(camCadData)
{
   init();

   m_allocatedProgress = NULL;
   m_progress = operationProgress;
}

CCamCadFileWriteProgress::~CCamCadFileWriteProgress()
{
   delete m_allocatedProgress;
}

void CCamCadFileWriteProgress::updateProgressStatus(const CString& status)
{
   if (m_progress != NULL)
   {
      m_progress->updateStatus(status);
   }
}

void CCamCadFileWriteProgress::setProgressLength(double length)
{
   if (m_progress != NULL)
   {
      m_progress->setLength(length);
   }
}

double CCamCadFileWriteProgress::updateProgress(double position)
{
   double retval = 0.;

   if (m_progress != NULL)
   {
      retval = m_progress->updateProgress(position);
   }

   return retval;
}

void CCamCadFileWriteProgress::init()
{
   m_blockUpdateInterval = 1;
   m_fileUpdateInterval  = 1;

   m_keyWordCount   = 0;
   m_layerCount     = 0;
   m_blockCount     = 0;
   m_dataCount      = 0;
   m_fileCount      = 0;
   m_algorithmCount = 0;
   m_widthCount     = 0;

   m_keyWordCapacity   = m_camCadData.getKeyWordArray().GetCount()-1;
   m_layerCapacity     = m_camCadData.getLayerArray().GetSize();
   m_blockCapacity     = m_camCadData.getMaxBlockIndex();
   m_dataCapacity      = m_camCadData.getCurrentEntityNumber();
   m_fileCapacity      = 0;
   m_algorithmCapacity = m_camCadData.getDfmAlgorithmArraySize();
   m_widthCapacity     = m_camCadData.getNextWidthIndex();

   for (POSITION pos = m_camCadData.getFileList().GetHeadPosition();pos != NULL;)
   {
      FileStruct *file = m_camCadData.getFileList().GetNext(pos);

      m_fileCapacity++;

      m_fileCapacity += file->getNetList().GetCount();
      m_fileCapacity += file->getBusList().GetCount();
      m_fileCapacity += file->getVariantList().GetCount();
      m_fileCapacity += file->getTypeList().GetCount();
      m_fileCapacity += file->getDRCList().GetCount();
   }

   //int numEntries = EntityNum;

   //numEntries += WeightKeyWord * doc->maxKeyWordArray;
   //numEntries += WeightLayer   * doc->getMaxLayerIndex();
   //numEntries += WeightBlock   * doc->maxBlockArray;

   //for (POSITION pos = doc->FileList.GetHeadPosition();pos != NULL;)
   //{
   //   FileStruct *file = doc->FileList.GetNext(pos);
   //   numEntries += WeightNetList  * file->getNetList().GetCount();
   //   numEntries += WeightTypeList * file->TypeList.GetCount();
   //   numEntries += WeightDrcList  * file->DRCList.GetCount();
   //}

   //numEntries += WeightAlgList   * doc->maxAlgNameArray;
   //numEntries += WeightWidthList * doc->nextWidthIndex;

   //int updateInterval = numEntries / 400;

   //if (updateInterval < 1) updateInterval = 1;

   //COperationProgress progress(numEntries);
   //progress.updateStatus("Writing CAMCAD data file.");
   //int entryCount = 0;
   //int updateFence = 0;

}

void CCamCadFileWriteProgress::setStatus(const CString& status)
{
   m_status = status;
}

void CCamCadFileWriteProgress::incKeyWord()
{
   if (m_keyWordCount == 0)
   {
      updateProgressStatus(m_status + "Keywords");
      setProgressLength(m_keyWordCapacity);
   }

   m_keyWordCount++;

   updateProgress(m_keyWordCount);
}

void CCamCadFileWriteProgress::incLayer()
{
   if (m_layerCount == 0)
   {
      updateProgressStatus(m_status + "Layers");
      setProgressLength(m_layerCapacity);
   }

   m_layerCount++;

   updateProgress(m_layerCount);
}

void CCamCadFileWriteProgress::incBlock()
{
   if (m_blockCount == 0)
   {
      updateProgressStatus(m_status + "Blocks");

      int capacity = m_blockCapacity + m_dataCapacity; 
      setProgressLength(capacity);
      m_blockUpdateInterval = capacity / 400;

      if (m_blockUpdateInterval < 1) m_blockUpdateInterval = 1;
   }

   m_blockCount++;

   if ((m_blockCount % m_blockUpdateInterval) == 0)
   {
      updateProgress(m_blockCount);
   }
}

void CCamCadFileWriteProgress::incData()
{
   incBlock();
}

void CCamCadFileWriteProgress::incFile()
{
   if (m_fileCount == 0)
   {
      updateProgressStatus(m_status + "File Data (nets, busses, types, DRCs)");

      setProgressLength(m_fileCapacity);
      m_fileUpdateInterval = m_fileCapacity / 400;

      if (m_fileUpdateInterval < 1) m_fileUpdateInterval = 1;
   }

   m_fileCount++;

   if ((m_fileCount % m_fileUpdateInterval) == 0)
   {
      updateProgress(m_fileCount);
   }
}

void CCamCadFileWriteProgress::incNet()
{
   incFile();
}

void CCamCadFileWriteProgress::incBus()
{
   incFile();
}

void CCamCadFileWriteProgress::incVariant()
{
   incFile();
}

void CCamCadFileWriteProgress::incType()
{
   incFile();
}

void CCamCadFileWriteProgress::incDrc()
{
   incFile();
}

void CCamCadFileWriteProgress::incAlgorithm()
{
   if (m_algorithmCount == 0)
   {
      updateProgressStatus(m_status + "Algorithm");
      setProgressLength(m_algorithmCapacity);
   }

   m_algorithmCount++;

   updateProgress(m_algorithmCount);
}

void CCamCadFileWriteProgress::incWidth()
{
   if (m_widthCount == 0)
   {
      updateProgressStatus(m_status + "Widths");
      setProgressLength(m_widthCapacity);
   }

   m_widthCount++;

   updateProgress(m_widthCount);
}
