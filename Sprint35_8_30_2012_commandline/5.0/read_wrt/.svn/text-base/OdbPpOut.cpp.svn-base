
#include "stdafx.h"
#include "ccdoc.h"
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "pfwlib.h"
#include <math.h>
#include <direct.h>
#include "CCEtoODB.h"
#include "RwUiLib.h"
#include "DcaVariant.h"
#include "OdbPpOut.h"
#include "DcaPolyLib.h"
#include "DcaVariant.h"
#include "net_util.h"
#include "PolyLib.h"
#include <sys/stat.h>
#include <io.h>
#include "DcaBbLib.h"
#include "outln_rb.h"
#include "Graph.h"
#include "DcaDftCommon.h"

static   CCEtoODBDoc        *OdbGlobalDoc;
static   int               display_error;
static   int               page_units;
static   double            pageUnitsToInchFactor;
static   double            pageUnitsToMilsFactor;
static   FILE              *flog;
static OdbJob * OdbGlobalJobPtr;
static OdbOutSettingsFile odbSettingsFile;
static CPntList *Outline_GetOutline(CPolyList* partList, int *returnCode);
extern CPoly *GetSegmentOutline(double ax, double ay, double bx, double by, double bulge, double widthRadius, int widthIndex);
extern BOOL CanUnion(CPolyList* partList);
#define NONE_NET "$NONE$"

CString GetODBLayerName(CString camcadLayerName, bool mirrorLayer = false);
CString GetODBLayerName(int layerIndex, bool mirrorLayer = false);
CString GetODBLayerName(LayerStruct* layer, bool mirrorLayer = false);
CString GetODBElectricalLayerName(LayerStruct* layer, bool mirrorLayer = false);

void CacheAndReportNameChange(CString entityTypeName, CString originalName, CString validatedName);
bool IsValidODBEntityName(const CString& name, CString & validName, CString entityTypeName, int entityID, bool attributeName = false);
bool IsValidODBNetName(const CString & name, CString & validName, CString entityTypeName, int entityID);
bool IsValidOdbLayerName(LayerStruct *layer, CString &validatedName);
map<CString, CString> invalidValidNameMap;
vector<CAMCADAttribute*> GetCAMCADAttributeList(CAttributes*& attributeMap);

#define DIRECTORY_CLEANUP_FAILED 2

/*****************************************************************************/

class LayerExportLog
{
public:
   set<CString > camcadExportedLayers;

public:
   void LogExportedLayer(LayerStruct *layer, bool layerMirrored);
};

void LayerExportLog::LogExportedLayer(LayerStruct *layer, bool layerMirrored)
{
   if (NULL != layer)
   {
      camcadExportedLayers.insert(layer->getName());
      if (layerMirrored)
      {
         LayerStruct &layerMir = layer->getMirroredLayer();
         if (layer->getLayerIndex() != layerMir.getLayerIndex())
         {
            camcadExportedLayers.insert(layerMir.getName());
         }
      }
   }
}

static LayerExportLog *LayerLog = NULL;

/*****************************************************************************/
/*****************************************************************************/

static   int   make_dir(const char *buildpath)
{
   // see if it already exist
   if( _chdir( buildpath ) == 0  )
   {
      // dino does not care 
   }
   else
   {
      // we need to create directory one at a time
      if( _mkdir( buildpath ) != 0 )   
      {
         CString  tmp;
         tmp.Format( "Error creating directory '%s'" , buildpath);
         ErrorMessage(tmp, "ODB++ Write", MB_ICONEXCLAMATION | MB_OK);
         return 0;
      }
   }
   return 1;
}

/*****************************************************************************/
/* 
   Specify the Decimal places accuracy of ODB++ file 
*/
static int GetFormatDecimalPlaces(PageUnitsTag units)
{
   // The decimal can be adjusted based on 
   // the accurancy of the calculation in various unit
   // It will affect the result of ODB++ file
   int decimals = 5; // default. 
   
   switch (units)
   {
   case pageUnitsInches:         decimals = 5; break;
   case pageUnitsMils:           decimals = 3; break;
   }

   return decimals;
}

static CString GetFormatDecimalValue(double value, PageUnitsTag units)
{
   int decimals = GetFormatDecimalPlaces(units);

   CString formatVal;
   formatVal.Format("%.*lf", decimals, value);
   return formatVal;
}

//-----------------------------------------------------------------------------

static double GetOdbThresholdValue()
{
   int decimals = GetFormatDecimalPlaces(pageUnitsInches);
   if (decimals < 1)
      decimals = 5;
   double odbUnitsThreshold = 1.0 / pow(10.0, decimals);

   return odbUnitsThreshold;
}

static void MakeValidOdbUnitsExtent(CExtent &extent, double scaleFactor)
{
   // Extent is in page units coming in, convert to final ODB++ output units,
   // and ensure extent has non-zero size in both X and Y.

   // Smallest non-zero value that can be used.
   double odbUnitsThreshold = GetOdbThresholdValue();

   // Init min/max to smallest valid extent.
   double xmin = -odbUnitsThreshold;
   double xmax =  odbUnitsThreshold;
   double ymin = -odbUnitsThreshold;
   double ymax =  odbUnitsThreshold;

   // Update min/max with values from extent if extent is valid.
   if (extent.isValid())
   {
      // Convert values in extent to final ODB++ output units.
      // Do not use extent.scale(f), it scales the AREA of the extent, not the min/max directly.
      // Do not do in-place reset (e.g. setXmin( getXmin + mods ), because validation during
      // update in setters may change the other value before it gets used, i.e. setting min
      // may alter max before we get to use max.
      xmin = extent.getXmin() * pageUnitsToInchFactor * scaleFactor;
      xmax = extent.getXmax() * pageUnitsToInchFactor * scaleFactor;
      ymin = extent.getYmin() * pageUnitsToInchFactor * scaleFactor;
      ymax = extent.getYmax() * pageUnitsToInchFactor * scaleFactor;
   }

   // Update extent with values, now  in ODB units.
   extent.set(xmin, ymin, xmax, ymax);

   // Check extent.
   // If the size of the extent is too small then expand it.
   // Move existing min and max by one threshold away from each other, to maintain the
   // same center. Remember, the center is not necessarily evently in between the min and max.
   if (extent.getXsize() < odbUnitsThreshold)
   {
      // fix
      extent.setXmin( extent.getXmin() - odbUnitsThreshold );
      extent.setXmax( extent.getXmax() + odbUnitsThreshold );
   }
   if (extent.getYsize() < odbUnitsThreshold)
   {
      // fix
      extent.setYmin( extent.getYmin() - odbUnitsThreshold );
      extent.setYmax( extent.getYmax() + odbUnitsThreshold );
   }
}

static void MakeValidOdbUnitsRC(double &odbllx, double &odblly, double &odbxsize, double &odbysize)
{
   // Values are in ODB output units.
   // Adjust values if necessary to ensure they define a rectangle with non-zero size in both X and Y.

   double odbUnitsThreshold = GetOdbThresholdValue();

   // If the size of the extent is too small then expand it.
   // Since size is too small we can consider it zero, which makes the lower left xy
   // equivalent to center. Move the lower left down and left one threshold unit,
   // and expand size to two threshold units. That should center our new RC at the old llx,lly.

   if (odbxsize < odbUnitsThreshold)
   {
      // fix
      odbllx = odbllx - odbUnitsThreshold;
      odbxsize = 2. * odbUnitsThreshold;
   }

   if (odbysize < odbUnitsThreshold)
   {
      // fix
      odblly = odblly - odbUnitsThreshold;
      odbysize = 2. * odbUnitsThreshold;
   }
}

//-----------------------------------------------------------------------------

void CreateSumFiles(CString filePath, CString fileName)
{
 struct _stat buff;
  
  
  long double size;
  if(_stat(filePath, &buff) != 0)
      size =0;
  else
    size = buff.st_size;
  
  CString folder = dirname(filePath);
  CString sumFile = folder;
  sumFile += "/";
  sumFile += ".";
  sumFile += fileName;
  sumFile +=".sum";
  
  FILE *pSumFile = NULL;
  if(pSumFile = fopen(sumFile, "w"))
  {
	fprintf(pSumFile, "SIZE = %lg\nSUM = 0 \nDATE = \nTIME = \nVERSION = CAMCAD V.%s\nUSER = ODB++", size,getApp().getVersionString());
	fclose (pSumFile);
  }
}

OdbPadUsageTag InsertTypeToPadUsage(InsertTypeTag insertType)
{
   OdbPadUsageTag padUsage = padUsageUndefined;
   switch (insertType)
   {
   case insertTypePin:              padUsage = padUsageToeprint;        break;
   case insertTypeVia:              padUsage = padUsageVia;             break;
   case insertTypeFiducial:         padUsage = padUsageGlobalFiducial;  break;
   case insertTypeDrillTool:        padUsage = padUsageToolingHole;     break;
   }

   return padUsage;
}

void GtarGzipJobFiles()
{
    bool succ =true;

    CString pack_name;
    pack_name = OdbGlobalJobPtr->GetJobPath() + ".tgz";

    CString gtarExePath = getApp().getUserPath() + "gtar.exe";
    CString gzipExePath = getApp().getUserPath() + "gzip.exe";

    if((-1 != _access(gtarExePath,00)) && (-1 != _access(gzipExePath,00)))
    {
       CString tmpStr = OdbGlobalJobPtr->GetJobPath();
       tmpStr.TrimRight("/\\");
       CFilePath jobPath = tmpStr;
       CString jobName = jobPath.getFileName();
       //Now we have the EXEcutables....
       // Run it now...
       // Assemble the command line.
       CString cmdLine;

       cmdLine.Format("\"%s\" --use-compress-program=\"%s\" --force-local -cf \"%s\"  \"%s\"", 
          gtarExePath, gzipExePath, pack_name, jobName); //zippath);

       _chdir(jobPath.getDirectoryPath());
       // Execute the command.
       if(-1 != _access(pack_name,00))// zip file present, remove it first
          DeleteFile(pack_name);

       if (ExecAndWait(cmdLine, SW_HIDE, NULL))
       {
          fprintf(flog,"There was an error using gzip.exe and gtar.exe.\n");
       }
    }
    else if(-1 == _access(gzipExePath,00))
    {
       fprintf(flog,"\nNot able to find gzip executable \"%s\"",gzipExePath);
    }
    else if(-1 == _access(gtarExePath,00))
    {
       fprintf(flog,"\nNot able to find gtar executable \"%s\"",gtarExePath);
    }

}

/******************************************************************************
* Outline_GetOutline 
* - This function copied from Access Analysis, the difference is function here uses RubberBand
*
* returnCode
*   0 = generated good outline
*   1 = returned closed poly
*   2 = simple segment, returned segment outline
*  -1 = empty
*  -2 = no result
*  -3 = self-intersecting result
*  -4 = simple segment, no width
*/
CPntList *Outline_GetOutline(CPolyList* partList, int *returnCode)
{
   int count = partList->GetCount();
   CPntList *result;
   CCEtoODBDoc *doc = NULL;
   double accuracy;
   if(NULL != getActiveView())
      doc = getActiveView()->GetDocument();
   if(NULL != doc)
      accuracy = get_accuracy(doc);

   if (!count)
   {
      *returnCode = -1;

      //delete partList;
      //partList = NULL;

      return NULL;
   }

   if (count == 1)
   {
      CPoly *poly = partList->GetHead();


      if (poly->isClosed() && (poly->getPntList().GetCount() > 0) && !PolySelfIntersects(&poly->getPntList()))
      {
         result = new CPntList;

         while (poly->getPntList().GetCount())
            result->AddTail(poly->getPntList().RemoveHead());

         //delete partList;
         //partList = NULL;

         *returnCode = 1;
         return result;
      }
      else if (poly->getPntList().GetCount() == 2)   // is a simple segment or arc
      {
         // if it has no width, you are out of luck         
         if ((NULL != doc) && (OdbGlobalDoc->getWidthTable()[poly->getWidthIndex()]->getSizeA() > SMALLNUMBER*2))
         {
            CPnt *head, *tail;
            head = poly->getPntList().GetHead();
            tail = poly->getPntList().GetTail();

            CPoly *newPoly = GetSegmentOutline(head->x, head->y, tail->x, tail->y, head->bulge, OdbGlobalDoc->getWidthTable()[poly->getWidthIndex()]->getSizeA()/2, 0);

            result = new CPntList;
            while (newPoly->getPntList().GetCount())
               result->AddTail(newPoly->getPntList().RemoveHead());

            delete newPoly;
            //delete partList;
            //partList = NULL;

            *returnCode = 2;
            return result;
         }
         else
         {
            *returnCode = -4;
            return NULL; 
         }
      }
   }

   result = NULL;
   
   if (/*AllowUnion && */CanUnion(partList))
      result = BuildUnion(partList);

   if (!result && CanRubberBand(partList, accuracy))
      result = RubberBand(partList, accuracy);

   //FreePolyList(partList);

   if (!result)
   {
      *returnCode = -2;
      return NULL;
   }

   CleanPntList(result, accuracy);

   if (PolySelfIntersects(result))
   {
      FreePntList(result);
      result = NULL;
      *returnCode = -3;
      return NULL;
   }

   *returnCode = 0;
   return result;
}

void RecursivePolyData(BlockStruct *block, GraphicClassTag graphicClass, bool &closedPolys, int &numberOfPolys, CPolyList*& polyList)
{
   for (POSITION blockpos = (block)?block->getDataList().GetHeadPosition():NULL;blockpos;)
   {
      DataStruct *data = block->getDataList().GetNext(blockpos);
      
      if(!data) continue;
      if((data->getDataType() == T_POLY) && (data->getGraphicClass() ==  graphicClass))
      {
         POSITION datapos = data->getPolyList()->GetHeadPosition();
         while (datapos)
         {
            CPoly* poly = data->getPolyList()->GetNext(datapos);
            if (poly)
            {
               int layerIndex = data->getLayerIndex();
               LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
               
               // This code is being left here as a comment in case someone sometime looks at this processing
               // and compares it to other places that are treating layers and restricting them to those that
               // are mapped to ODB layers. The types PANEL_OUTLINE and BOARD_OUTLINE are not mapped to ODB
               // layers in the usual fashion, they end up in "profile" definitions. So we do want to
               // process them.
               // DO NOT DO THIS !
               //CString odbLayerName = GetODBLayerName(layerIndex, false /*layerMirrored*/);
               //if (odbLayerName.IsEmpty())
               //   continue;

               LayerLog->LogExportedLayer(layer, false);

               ++numberOfPolys;
               
               CPoly* newpoly = new CPoly(*poly);
               polyList->AddHead(newpoly);
               // DR 738292 This does not work well enough becacue ->isClosed() is only checking a flag.
               // Case has poly that is close in actual data, but does not have flag. That causes us to go
               // off and rubberband it later. What matters is if poly data is actually closed.
               // Update flag to match actual data, it's important to later processing of the poly.
               newpoly->setClosed( newpoly->isClosedInData() );  
               if (!newpoly->isClosed())
                  closedPolys = false;               
            }
         }      
      }
      else if(data->getDataType() == T_INSERT)
      {
         BlockStruct *insertblock = OdbGlobalDoc->getBlockAt(data->getInsert()->getBlockNumber());
         RecursivePolyData(insertblock, graphicClass,closedPolys, numberOfPolys, polyList);
      }
   }
}

CPolyList* GetClosedPolygons(BlockStruct* block,GraphicClassTag graphicClass)
{
   if(NULL == block)
      return NULL;

   CPolyList* polyList = new CPolyList;
   bool closedPolys = true;
   int numberOfPolys = 0;
   
   RecursivePolyData(block, graphicClass,closedPolys, numberOfPolys, polyList);

   // Right now we'll do this special stuff only for board outline.
   // WI 21083
   if (graphicClass == graphicClassBoardOutline && odbSettingsFile.GetUseOutlineAutoJoin())
   {
      PolylistCompressor polycomp(*OdbGlobalDoc, *polyList);
      CPolyList resultPolylist;
      int polyJoinResult = polycomp.JoinPolys(resultPolylist);
      if (polyJoinResult != 0)
      {
         int polyCount = resultPolylist.GetCount();
         int closedCount = resultPolylist.getClosedPolyCount();

         if (closedCount > 0)
         {
            *polyList = resultPolylist;
            numberOfPolys = resultPolylist.GetCount();
         }
      }
   }

   int returnCode;  
   if(numberOfPolys)
   {
      if(!closedPolys && numberOfPolys)
      {
         CPntList *CPntList = Outline_GetOutline(polyList,&returnCode);
         if(CPntList)
         {
            POSITION pos = CPntList->GetHeadPosition();
            CPoly* poly = new CPoly();
            while (pos)
            {
               CPnt *pnt = CPntList->GetNext(pos);
               poly->getPntList().AddTail(pnt);
            }
            while(!polyList->isEmpty())
            {
               polyList->removeHead();
            }
            polyList->AddTail(poly);
            poly->closePolyWithCoincidentEndPoints();
         }
         else
         {
            CExtent extent = polyList->getExtent();
            CPoly *poly = new CPoly;

            poly->addVertex(extent.getLL().x, extent.getLL().y);
            poly->addVertex(extent.getUL().x, extent.getUL().y);
            poly->addVertex(extent.getUR().x, extent.getUR().y);
            poly->addVertex(extent.getLR().x, extent.getLR().y);
            poly->addVertex(extent.getLL().x, extent.getLL().y);

            poly->closePolyWithCoincidentEndPoints();
            while(!polyList->isEmpty())
            {
               CPoly *junk = polyList->removeHead();
               //delete junk;  *rcf Looks like a memory leak here and we should have this delete, but don't have a test case in hand. Find one!!!
            }
            polyList->AddTail(poly);
         }
      }
   }
   else
   {
      CExtent extent = block->getExtent();
      if (!extent.isValid())
         extent = OdbGlobalDoc->blockExtents(&block->getDataList(), 0.0, 0.0, 0.0, 0, 1, -1, false);
      CPoly *poly = new CPoly;
      if (extent.isValid())
      {
         poly->addVertex(extent.getLL().x, extent.getLL().y);
         poly->addVertex(extent.getUL().x, extent.getUL().y);
         poly->addVertex(extent.getUR().x, extent.getUR().y);
         poly->addVertex(extent.getLR().x, extent.getLR().y);
         poly->addVertex(extent.getLL().x, extent.getLL().y);

         poly->closePolyWithCoincidentEndPoints();
      }
      else
      {
         // Minimum size poly.
         double odbThreshold = GetOdbThresholdValue();
         double minCoord = odbThreshold / pageUnitsToInchFactor;

         poly->addVertex(-minCoord, -minCoord);  // LL
         poly->addVertex(-minCoord,  minCoord);  // UL
         poly->addVertex( minCoord,  minCoord);  // UR
         poly->addVertex( minCoord, -minCoord);  // LR
         poly->addVertex(-minCoord, -minCoord);  // LL

         poly->closePolyWithCoincidentEndPoints();
      }

      // Clean out the crud then save the new poly
      while(!polyList->isEmpty())
      {
         polyList->removeHead();
      }
      polyList->AddTail(poly);

   }
   return polyList;
}


static BOOL ConvertToForwardSlash(CString& filePath)
{
    filePath.Replace('\\','/');

    return TRUE;    
}


void WritePolygon(CPoly* poly, FILE* file)
{
   if(NULL == poly || NULL == file)
      return;

   // DR 851203 - Poly with no pnts crashes loop at .GetNext() that does not check pntPos.
   if (poly->getVertexCount() < 1)
      return;

   CPnt *p1, *p2;
   bool firstPoint = true;
   if(poly->isClosed())
   {
      bool isVoid = poly->isVoid();
      bool inland = true;
      if(isVoid)
         inland = false;
      POSITION pntPos = poly->getPntList().GetHeadPosition();
      p2 = poly->getPntList().GetNext(pntPos);  // DR 851203 - Here.
      while (NULL != pntPos)
      {
         p1 = p2;
         if(firstPoint)
         {
            fprintf(file,"OB %s %s %s\n",
                                            GetFormatDecimalValue(p1->x * pageUnitsToInchFactor, pageUnitsInches),
                                            GetFormatDecimalValue(p1->y * pageUnitsToInchFactor, pageUnitsInches),
                                            inland ? "I" : "H");
            firstPoint = false;
         }
         p2 = poly->getPntList().GetNext(pntPos);

         if (fabs(p1->bulge) > SMALLNUMBER)
         {
            double sa, da, cx, cy, r;
            da = atan(p1->bulge) * 4;
            char clockWise = ( p1->bulge < 0.0 ) ? 'Y' : 'N';
            ArcPoint2Angle(p1->x, p1->y, p2->x, p2->y, da, &cx, &cy, &r, &sa);
            fprintf(file,"OC %s %s %s %s %c\n",
                                                      GetFormatDecimalValue(p2->x * pageUnitsToInchFactor, pageUnitsInches),
                                                      GetFormatDecimalValue(p2->y * pageUnitsToInchFactor, pageUnitsInches),
                                                      GetFormatDecimalValue(cx * pageUnitsToInchFactor, pageUnitsInches),
                                                      GetFormatDecimalValue(cy * pageUnitsToInchFactor, pageUnitsInches),
                                                      clockWise );
         }
         else
         {
            fprintf(file,"OS %s %s\n",
                                         GetFormatDecimalValue(p2->x * pageUnitsToInchFactor, pageUnitsInches),
                                         GetFormatDecimalValue(p2->y * pageUnitsToInchFactor, pageUnitsInches));
         }
      }
      if(false == firstPoint)
         fprintf(file,"OE\n");
   }
}


void WriteCircle(double xc, double yc, double radius,FILE* file)
{
   // Incoming values in CCZ Page Units, convert to output units and write record.

   if(NULL == file)
      return;
   fprintf(file,"CR %s %s %s\n",
      GetFormatDecimalValue(xc * pageUnitsToInchFactor, pageUnitsInches),
      GetFormatDecimalValue(yc * pageUnitsToInchFactor, pageUnitsInches),
      GetFormatDecimalValue(radius * pageUnitsToInchFactor, pageUnitsInches));
}

void WriteSquare(double xc, double yc, double halfside,FILE* file)
{
   // Incoming values in CCZ Page Units, convert to output units and write record.

   if(NULL == file)
      return;
   fprintf(file,"SQ %s %s %s\n",
      GetFormatDecimalValue(xc * pageUnitsToInchFactor, pageUnitsInches),
      GetFormatDecimalValue(yc * pageUnitsToInchFactor, pageUnitsInches),
      GetFormatDecimalValue(halfside * pageUnitsToInchFactor, pageUnitsInches));
}

void WriteRectangle(double llx, double lly, double xsize, double ysize, FILE* file)
{
   // Incoming values in CCZ Page Units, convert to output units and write record.

   if(NULL == file)
      return;

   double odbllx = llx * pageUnitsToInchFactor;
   double odblly = lly * pageUnitsToInchFactor;
   double odbxsize = xsize * pageUnitsToInchFactor;
   double odbysize = ysize * pageUnitsToInchFactor;

   MakeValidOdbUnitsRC(odbllx, odblly, odbxsize, odbysize);

   fprintf(file,"RC %s %s %s %s\n",
      GetFormatDecimalValue(odbllx, pageUnitsInches),
      GetFormatDecimalValue(odblly, pageUnitsInches),
      GetFormatDecimalValue(odbxsize, pageUnitsInches),
      GetFormatDecimalValue(odbysize, pageUnitsInches));
}

void WritePolygons(CPolyList* polyList,FILE* file, bool contour)
{
   POSITION pos = polyList->GetHeadPosition();

   bool firstPoly = true;
   while (pos)
   {
      CPoly* poly = polyList->GetNext(pos);
      if (poly)
      {
         if(firstPoly)
         {
            if(!contour)
               fprintf(file,"S P 0\n");
            else
               fprintf(file,"CT\n");
            firstPoly = false;
         }
         WritePolygon(poly, file);
      }
   }

   if(false == firstPoly)
   {
      if(!contour)
         fprintf(file,"SE\n");
      else
         fprintf(file,"CE\n");
   }
}


CPolyList* CopyPoly(CPolyList* srcPoly)
{
   POSITION polyPos, pntPos;
   CPoly *poly, *newPoly;
   CPnt *pnt, *newPnt;

   CPolyList* destPoly = new CPolyList;

   polyPos = srcPoly->GetHeadPosition();
   while (NULL != polyPos)
   {
      poly = srcPoly->GetNext(polyPos);
      newPoly = new CPoly;
      newPoly->setWidthIndex(poly->getWidthIndex());
      newPoly->setFilled(poly->isFilled());
      newPoly->setClosed(poly->isClosed());
      newPoly->setVoid(poly->isVoid());
      newPoly->setThermalLine(poly->isThermalLine());
      newPoly->setFloodBoundary(poly->isThermalLine());
      newPoly->setHidden(poly->isHidden());
      newPoly->setHatchLine(poly->isHatchLine());

      destPoly->AddTail(newPoly);

      pntPos = poly->getPntList().GetHeadPosition();
      while (NULL != pntPos)
      {
         pnt = poly->getPntList().GetNext(pntPos);
         newPnt = new CPnt(*pnt);
         newPoly->getPntList().AddTail(newPnt);
      }
   }
   return destPoly;
}


CString GetODBLayerType(int electricalStackupNum)
{
   CString ODBLayerType = "";
   for (int layerIndex = 0;layerIndex < OdbGlobalDoc->getMaxLayerIndex();layerIndex++)
   {
      LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
      if( (layer != NULL) && (layer->getElectricalStackNumber() == electricalStackupNum))
      {
         CString tmpODBLayerType;
         switch(layer->getLayerType())
         {
         case layerTypePowerNegative:
         case layerTypePowerPositive:
            {
               tmpODBLayerType = "POWER_GROUND";
            }
            break;
         case layerTypeSplitPlane:
            {
               tmpODBLayerType = "MIXED";
            }
            break;
         default:
            tmpODBLayerType = "SIGNAL";
         }

         if(ODBLayerType.IsEmpty())
         {
            ODBLayerType = tmpODBLayerType;
         }
         else if((!ODBLayerType.IsEmpty()) && (ODBLayerType != tmpODBLayerType))
            ODBLayerType = "MIXED";
      }
   }
   return ODBLayerType;
}


/*****************************************************************************/
/*

Generel ODB++ structure for a basic readable file

<job_name>
matrix
matrix
misc        (empty)
symbols     (empty)
steps
pcb
layers
artwork_top
stackups    (empty)
forms       (empty)
flows       (empty)
wheels      (empty)
fonts       (empty)
input       (empty)
output      (empty)
user        (empty)
ext         (empty)
*/


int OdbJob::CreateJobDirStruct() // Miten -- the entire directory sub structure is created by this function
{
   if(-1 != _access(m_jobPath,00))
   {
      if(!removeDirectoryTree(m_jobPath)) 
         return DIRECTORY_CLEANUP_FAILED;
   }

   if (!make_dir(m_jobPath))  return 0;

   CString buildpath = m_jobPath;
   buildpath += "/matrix";
   if (!make_dir(buildpath))  return 0;

   buildpath = m_jobPath;
   buildpath += "/misc";
   if (!make_dir(buildpath))  return 0;

   buildpath = m_jobPath;
   buildpath += "/steps";
   if (!make_dir(buildpath))  return 0;

   buildpath = m_jobPath;
   buildpath += "/fonts";
   if (!make_dir(buildpath))  return 0;

   buildpath = m_jobPath;
   buildpath += "/symbols";
   if (!make_dir(buildpath))  return 0;

   return 1;
}

void WriteLayerMappingInfoToLog()
{     
   fprintf(flog, "\n#---------------------------------------\n");
   fprintf(flog,"ODB++ layer mapping\n");
   fprintf(flog, "#---------------------------------------\n");
   set<CString> camcadNotExportedLayers;
   for (int layerIndex = 0;layerIndex < OdbGlobalDoc->getMaxLayerIndex();layerIndex++)
   {
      LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

      if (layer != NULL)
      {
         if (!layer->isFloating()) // Floating layer is not a real user data layer, skip it/them altogether in report.
         {
            set<CString>::iterator it = LayerLog->camcadExportedLayers.find(layer->getName());
            if(it != LayerLog->camcadExportedLayers.end())
            {
               CString ODBLayerName = GetODBLayerName(layer);
               fprintf(flog,"Layer \"%s\" is mapped to ODB++ layer \"%s\".\n",layer->getName(),ODBLayerName);
            }
            else
            {
               camcadNotExportedLayers.insert(layer->getName());
            }
         }
      }
   }
   fprintf(flog,"\n\n");
   set<CString>::iterator it = camcadNotExportedLayers.begin();
   for( ; it != camcadNotExportedLayers.end(); it++)
   {
      fprintf(flog,"Layer \"%s\" not exported.\n",(*it));
   }   
}

// Miten -- writing of the odb file function
void ODB_WriteFiles(const char *pathname, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits)
{
   char  curdir[_MAX_PATH];

   OdbGlobalDoc = Doc;
   page_units = pageUnits;
   pageUnitsToInchFactor = Units_Factor(pageUnits, UNIT_INCHES);
   pageUnitsToMilsFactor = Units_Factor(pageUnits, UNIT_MILS);
   CString settingsFilePath = getApp().getExportSettingsFilePath("odb.out");

   /* Get the current working directory: */
   if(NULL == _getcwd( curdir, _MAX_PATH ))   
   {
      ErrorMessage("Error getting current directory.", "ODB++ Write", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString odbppLogFile;
   flog = getApp().OpenOperationLogFile("odb.log", odbppLogFile);
   if (flog == NULL) // error message already issued, just return.
      return;
   WriteStandardExportLogHeader(flog, "ODB++");
	fprintf(flog, "---------------------------------------\n\n");
   display_error++;

   if (LayerLog != NULL)
      delete LayerLog;
   LayerLog = new LayerExportLog;

   // Possible memory leak, do contents need to be deleted first?
   invalidValidNameMap.clear();

   int   pcbFound = 0;
   int panelFound = 0;

   CStringSupArray jobNames;
   POSITION pos = OdbGlobalDoc->getFileList().GetHeadPosition();
   while (NULL != pos)
   {
      FileStruct *file = OdbGlobalDoc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
         pcbFound++;
      else if(file->getBlockType() == BLOCKTYPE_PANEL)
         panelFound++;

      // Some importers put the whole original CAD file path as the <file> name. What we want
      // here is just the leaf filename. We'll keep the extension too.
      char name[_MAX_FNAME], ext[_MAX_EXT];
      _splitpath(file->getName(), NULL, NULL, name, ext);
      CString baseJobName(name);
      baseJobName += ext;

      // Can't be blank.
      if (baseJobName.IsEmpty())
         baseJobName = "unnamed";

      // Ensure unique file naming, insures unique ODB++ job names.
      // This is critical when we export multiple jobs in one shot.
      CString newJobName(baseJobName);
      int dupJobCounter = 0;
      while (jobNames.FindNoCase(newJobName) > -1)
      {
         newJobName.Format("%s_%d", baseJobName, ++dupJobCounter);
      }
      jobNames.Add(newJobName);
      // Update the file to have the possibly adjusted job name as its own name.
      file->setName(newJobName);

   }
   jobNames.RemoveAll(); // Clean it out, not needed any longer.

   if ((panelFound == 0) && (pcbFound == 0))
   {
      ErrorMessage("No visible PCB or Panel file found.\nNo Output written.", "ODB++ Write", MB_ICONHAND | MB_OK);
   }

   int totalVisibleFilesCount = pcbFound + panelFound;

   pos = OdbGlobalDoc->getFileList().GetHeadPosition();
   while (NULL != pos)
   {
      FileStruct *file = OdbGlobalDoc->getFileList().GetNext(pos);
      if (!file->isShown())
         continue;
      if (file->getBlockType() != BLOCKTYPE_PCB && file->getBlockType() != BLOCKTYPE_PANEL)
         continue;

      bool panelDesign = false;
      if (file->getBlockType() == BLOCKTYPE_PANEL)
         panelDesign = true;

      CString jobPath = pathname;
      jobPath += "\\";
      CString jobName = file->getName();
      CString validJobName;
      IsValidODBEntityName(jobName,validJobName, "Job", file->getFileNumber());
      jobName = validJobName; // Just always use validated name.

      jobPath += jobName + "odb";
      ConvertToForwardSlash(jobPath);
      OdbGlobalJobPtr = new OdbJob(jobPath);
      if(NULL == OdbGlobalJobPtr) //Should never happen
      {         
         CString message;
         message.Format("Failed to create the job database");
         fprintf(flog,message);
         return;
         fprintf(flog, "\n#-----------END OF LOGFILE-----------\n");
         if (!odbppLogFile.IsEmpty())
            fclose(flog);
         // set it back to the current directory.
         _chdir( curdir );
      }
      int returnValue = OdbGlobalJobPtr->CreateJobDirStruct();
      if(returnValue == DIRECTORY_CLEANUP_FAILED)
      {
         CString msg;
         msg.Format("Error deleting existing \"%s\" job directory.\nSome files inside the directory are being used by some application.\n",jobPath);
         ErrorMessage(msg, "ODB++ Write", MB_ICONSTOP | MB_OK);
         fprintf(flog, msg);         
         if(NULL != OdbGlobalJobPtr)
         {
            delete OdbGlobalJobPtr;
            OdbGlobalJobPtr = NULL;
         }
         fprintf(flog, "\n#-----------END OF LOGFILE-----------\n");
         if (!odbppLogFile.IsEmpty())
            fclose(flog);

         if (LayerLog != NULL)
            delete LayerLog;
         LayerLog = NULL;

         // set it back to the current directory.
         _chdir( curdir );
         return;
      }
      odbSettingsFile.LoadSettingsFile(settingsFilePath);
      OdbGlobalJobPtr->FillOdbLayerList();

      CString stepPath = jobPath;
      stepPath += "/steps";
      CString stepName(file->getName());
      stepPath += "/";      
      CString validStepName;
      IsValidODBEntityName(stepName,validStepName, "Step", file->getFileNumber());
      stepName = validStepName; // Just always use validated name.

      stepPath += stepName;
      OdbStep * step = new OdbStep(file, stepPath, stepName, panelDesign,1);      
      OdbGlobalJobPtr->AddStep(step);

      try
      {
         step->Process();
         OdbGlobalJobPtr->WriteMatrixFile();
         OdbGlobalJobPtr->WriteSymbolFiles();
         OdbGlobalJobPtr->CopyStandardFontsFile();
         WriteLayerMappingInfoToLog();
         if(true == odbSettingsFile.GetCompressOutput())
            GtarGzipJobFiles();
      }
      catch (CException* e)
      {
         char exceptionMsg[512];
         e->GetErrorMessage(exceptionMsg, 512);
         CString msg("Export aborted, encountered exception: ");
         msg += exceptionMsg;
         ErrorMessage(msg, "ODB++ Write", MB_OK);
      }


      if(NULL != OdbGlobalJobPtr)
      {
         delete OdbGlobalJobPtr;
         OdbGlobalJobPtr = NULL;
      }
   }
   fprintf(flog, "\n#-----------END OF LOGFILE-----------\n");

   // Close only if local log file  was opened. If it is the system log
   // then leave it be. If the local log file name is empty then the system
   // log file is in use.
   if (!odbppLogFile.IsEmpty())
      fclose(flog);

   if (LayerLog != NULL)
      delete LayerLog;
   LayerLog = NULL;

   // set it back to the current directory.
   _chdir( curdir );

   if (display_error && !odbppLogFile.IsEmpty())
      Logreader(odbppLogFile);

   return;
}

//==================================================================================

BomData::~BomData()
{
   vector<BomPartEntry* >::iterator bomPartListIterator = m_bomPartList.begin();
   for( ; bomPartListIterator != m_bomPartList.end() ; bomPartListIterator++)
   {
      if(NULL != (*bomPartListIterator))
      {
         delete (*bomPartListIterator);
         (*bomPartListIterator) = NULL;
      }
   }
}

//-----------------------------------------------------------------------------

BomPartEntry::BomPartEntry(ODBComponent* comp, FileStruct *file, CVariantItem *variantItem)
{
   m_comp = comp;

   DataStruct *data = m_comp->GetCompDataStruct();

   WORD descripKW = OdbGlobalDoc->IsKeyWord(ATT_PART_DESCRIPTION, 0);
   WORD descripLegacyKW = OdbGlobalDoc->IsKeyWord("Description (Legacy)", 0);  // A vPlan thing. Not sure we really want to keep this.
   WORD devicetypeKW  = OdbGlobalDoc->IsKeyWord(ATT_DEVICETYPE, 0);
   WORD valueKW  = OdbGlobalDoc->IsKeyWord(ATT_VALUE, 0);
   WORD plustolKW  = OdbGlobalDoc->IsKeyWord(ATT_PLUSTOLERANCE, 0);
   WORD minustolKW  = OdbGlobalDoc->IsKeyWord(ATT_MINUSTOLERANCE, 0);
   WORD subclassKW  = OdbGlobalDoc->IsKeyWord(ATT_SUBCLASS, 0);
   WORD pinmapKW  = OdbGlobalDoc->IsKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0);
   WORD partnumberKW  = OdbGlobalDoc->IsKeyWord(ATT_PARTNUMBER, 0);
   WORD loadedKW  = OdbGlobalDoc->IsKeyWord(ATT_LOADED, 0);

   CAttribute *attrib;

   // Get standard BOM settings from component attributes.
   if (data->getAttributes() != NULL)
   {
      // PARTNUMBER
      if (data->getAttributes()->Lookup(partnumberKW, attrib))
         m_partNumber = attrib->getStringValue();

      // DESCRIPTION
      if (data->getAttributes()->Lookup(descripKW, attrib))
         m_description = attrib->getStringValue();

      // DESCRIPTION (LEGACY)
      if (data->getAttributes()->Lookup(descripLegacyKW, attrib))
         m_descriptionL = attrib->getStringValue();

      // DEVICETYPE
      if (data->getAttributes()->Lookup(devicetypeKW, attrib))
         m_devicetype = attrib->getStringValue();

      // LOADED
      if (data->getAttributes()->Lookup(loadedKW, attrib))
         m_loaded = attrib->getStringValue();

      // VALUE
      if (data->getAttributes()->Lookup(valueKW, attrib))
         m_value = attrib->getStringValue();

      // +TOL
      if (data->getAttributes()->Lookup(plustolKW, attrib))
         m_plustol = attrib->getStringValue();

      // -TOL
      if (data->getAttributes()->Lookup(minustolKW, attrib))
         m_minustol = attrib->getStringValue();

      // SUBCLASS
      if (data->getAttributes()->Lookup(subclassKW, attrib))
         m_subclass = attrib->getStringValue();
   }

   // Get possible overrides from variantItem
   if (variantItem != NULL && variantItem->getAttributes() != NULL)
   {
      // PARTNUMBER
      if (variantItem->getAttributes()->Lookup(partnumberKW, attrib))
         m_partNumber = attrib->getStringValue();

      // DESCRIPTION
      if (variantItem->getAttributes()->Lookup(descripKW, attrib))
         m_description = attrib->getStringValue();

      // DESCRIPTION (LEGACY)
      if (variantItem->getAttributes()->Lookup(descripLegacyKW, attrib))
         m_descriptionL = attrib->getStringValue();

      // DEVICETYPE
      if (variantItem->getAttributes()->Lookup(devicetypeKW, attrib))
         m_devicetype = attrib->getStringValue();

      // LOADED
      if (variantItem->getAttributes()->Lookup(loadedKW, attrib))
         m_loaded = attrib->getStringValue();

      // VALUE
      if (variantItem->getAttributes()->Lookup(valueKW, attrib))
         m_value = attrib->getStringValue();

      // +TOL
      if (variantItem->getAttributes()->Lookup(plustolKW, attrib))
         m_plustol = attrib->getStringValue();

      // -TOL
      if (variantItem->getAttributes()->Lookup(minustolKW, attrib))
         m_minustol = attrib->getStringValue();

      // SUBCLASS
      if (variantItem->getAttributes()->Lookup(subclassKW, attrib))
         m_subclass = attrib->getStringValue();
   }

   // There is not attribute in CAMCAD that has the entire pinmap. The mapping for individual pins
   // is on the CompPin. The overall mapping collection is collected and managed by
   // Data Doctor, and is stored in the Part DB only. We don't want any dependency on
   // CAMCAD Part DB or Data Doctor here. E.g. we might have gotten a pinmap
   // from vPlan, put it in CCZ, now need it back. No CAMCAD Part DB is ever
   // involved in that scenario.
   // So here we'll just iterate on the CompPins and build our own pinmap string.
   // The one in ODB is somewhat like the one in CAMCAD, but separators are different.
   m_pinmap.Empty();
   if (file != NULL)
   {
      vector<ODBCompPin* > compPinList = comp->GetCompPinList();

      for(unsigned int pinNum = 0; pinNum < compPinList.size(); pinNum++)
      {
         ODBCompPin* compPin = compPinList[pinNum];
         if(NULL != compPin)
         {
            CompPinStruct *cp = file->getNetList().getCompPin(comp->GetRefName(), compPin->m_pinName);
            if (cp != NULL)
            {
               if (cp->getAttributes() && cp->getAttributes()->Lookup(pinmapKW, attrib))
               {
                  CString pinFunction( attrib->getStringValue() );
                  if (!pinFunction.IsEmpty())
                  {
                     if (!m_pinmap.IsEmpty())
                        m_pinmap += ",";
                     CString pinMapEntry;
                     pinMapEntry.Format("%s:%s", compPin->m_pinName, pinFunction);
                     m_pinmap += pinMapEntry;
                  }
               }
            }
         }
      }
   }

   // User attribs. Fill out the values array to match the name settings array. If
   // attrib is not present make a blank value entry.
   int attrCnt = odbSettingsFile.GetBomAttribNameCount();
   for (int attrI = 0; attrI < attrCnt; attrI++)
   {
      // Check to see if it is a keyword already, if it is then the entry will dictate type.
      // If it isn't a keyword already then there are no attribs in the data for it, so no need to bother with it.
      CString attribName( odbSettingsFile.GetBomAttribNameAt(attrI) );
      WORD userKW = OdbGlobalDoc->IsKeyWord(attribName, 0);
      if (userKW > -1)
      {
         if (data->getAttributes() && data->getAttributes()->Lookup(userKW, attrib))
            this->AddUserAttribVal( attrib->getStringValue() );
         else
            this->AddUserAttribVal(""); // No attrib on comp, blank value placeholder.
      }
      else
      {
         // No attrib with this name in data at all. Make blank value placeholder.
         this->AddUserAttribVal("");
      }
   }

}


BomPartEntry* BomData::AddBomPartEntry(ODBComponent* comp, FileStruct *file, CVariantItem *variantItem)
{
   BomPartEntry* bomPartEntry = NULL;

   // Can only have BOM Part entries when there is a partnumer available.

   // If we have a comp that is loaded then get the partnumber.
   CString partnum;
   if (comp != NULL && comp->IsLoaded())
      partnum = comp->GetPartNumber();

   // If we have variantItem then override with partnumber from variant.
   if (variantItem != NULL)
   {
      int kw = OdbGlobalDoc->getCamCadData().getAttributeKeywordIndex(standardAttributePartNumber);
      Attrib *attrib;
      if (variantItem->LookUpAttrib(kw, attrib))
      {
         CString candidate( attrib->getStringValue() );
         candidate.Trim();
         if (!candidate.IsEmpty())
            partnum = candidate;
      }
   }

   // If we have a partnumber then add the BOM entry.
   if (!partnum.IsEmpty())
   {
      int vectorIndex = FindBomPartIndex(partnum);
      if(vectorIndex != -1)
      {
         //We already have a record for this PN.
         // Increment usage count and return it.
         bomPartEntry = m_bomPartList[vectorIndex];
         bomPartEntry->AddRefDes(comp->GetRefName());
      }
      else
      {
         //Create a new record and add it to vector. Update the map.
         bomPartEntry = new BomPartEntry(comp, file, variantItem);
         bomPartEntry->AddRefDes(comp->GetRefName());
         m_bomPartList.push_back(bomPartEntry);
         vectorIndex = m_bomPartList.size() - 1;
         m_bomPartIndexMap.insert(std::make_pair<const CString,int>(partnum,vectorIndex));      
      }
   }

   return bomPartEntry;
}

//-----------------------------------------------------------------------------

//returns -1 if not found
int BomData::FindBomPartIndex(CString partnum)
{
   map<CString, int>::iterator it = m_bomPartIndexMap.find(partnum);
   if(m_bomPartIndexMap.end() != it)
   {
      //We found a record for this Net
      return (*it).second;
   }
   else
      return -1;
}

//==================================================================================

char GetFeatureUseTypeChar(FeatureUseType featureUseType)
{
   switch(featureUseType)
   {
   case Copper: return 'C';
      break;
   case Laminate: return 'L';
      break;
   case Hole: return 'H';
      break;
   default: return 'C';
   }
}

EdaDataNet* EdaData::AddEdaNet(NetStruct *net, CString netName, int& index)
{
   // WI 21401
   // If the netname is blank, that is really bad, makes invalid syntax in ODB.
   // It is really really bad if more than one net has blank  name. In order
   // to keep multiple nets with blank name from colliding, without having to
   // specifically track such nets, we'll generate a name based on net
   // entity number... these ought to be unique.
   netName.Trim();
   if (netName.IsEmpty())
   {
      // Default, in case net is null.
      netName = "$$unnamed$$";

      // Try for net entity number.
      if (net != NULL)
      {
         netName.Format("$$Entity%d$$", net->getEntityNumber());
      }
   }

   EdaDataNet* temp = NULL;
   //$NONE$ named net is special.Corresponding NetStruct will be empty.
   //As per the ODB++ spec all the Pin/Feature which are not associated 
   //with any net should go to net with name $NONE$.
   if((NULL == net) && (netName.CompareNoCase(NONE_NET)))
      return NULL;

   int vectorIndex = FindEdaNetNumber(netName);
   if(vectorIndex != -1)
   {
      //We already have a record for this Net
      index = vectorIndex;
      return m_edaDataNetList[vectorIndex];      
   }
   else
   {
      //Create a new record and add it to vector. Update the map.
      temp = new EdaDataNet(net,netName);
      m_edaDataNetList.push_back(temp);
      index = m_edaDataNetList.size() - 1;
      m_netNameIndexMap.insert(std::make_pair<const CString,int>(netName,index));      
      return temp;
   }
   return temp;
}


//returns -1 if not found
int EdaData::FindEdaNetNumber(CString cczNetName)
{
   map<CString, int>::iterator it = m_netNameIndexMap.find(cczNetName);
   if(m_netNameIndexMap.end() != it)
   {
      //We found a record for this Net
      return (*it).second;
   }
   else
      return -1;
}


EdaDataPkg* EdaData::AddEdaPkg(BlockStruct *compGeomBlock, bool isBottomBuilt, double scale, int& index)
{
   EdaDataPkg* temp = NULL;
   if(NULL != compGeomBlock)
   { 
      CString pkgName( EdaDataPkg::GetValidatedName(compGeomBlock) );

      int vectorIndex = FindEdaPkgNumber(compGeomBlock);
      if(vectorIndex != -1)
      {
         //We already have a record for this pkg
         index = vectorIndex;
         return m_edaDataPkgList[vectorIndex];
      }
      else
      {
         //Create a new record and add it to edaDataPkgList vector
         //Update the map PkgNameIndexMap.
         temp = new EdaDataPkg(compGeomBlock, isBottomBuilt, scale);
         m_edaDataPkgList.push_back(temp);
         index = m_edaDataPkgList.size() - 1;
         m_pkgNameIndexMap.insert(std::make_pair<const CString,int>(pkgName,index));
         return temp;
      }
   }
   return temp;
}


//returns -1 if not found
int EdaData::FindEdaPkgNumber(BlockStruct *compGeomBlock)
{
   CString pkgName( EdaDataPkg::GetValidatedName(compGeomBlock) );

   if (!pkgName.IsEmpty())
   {
      map<CString, int>::iterator it = m_pkgNameIndexMap.find(pkgName);
      if(m_pkgNameIndexMap.end() != it)
      {
         //Found a record for this pkg
         return (*it).second;
      }
   }

   return -1;
}


int EdaData::AddNetAttributeName(CString attributeName)
{
   int vectorIndex = FindNetAttributeNameNumber(attributeName);
   if(vectorIndex != -1)
   {
      //We already have a record for this Attribute Name
      return vectorIndex;
   }
   else
   {
      //Attribute Name Not found.Add and Update the map.
      m_netAttributeNames.push_back(attributeName);
      int index = m_netAttributeNames.size() - 1;
      m_netAttributeNameIndexMap.insert(std::make_pair<const CString,int>(attributeName,index));      
      return index;
   }
}


int EdaData::FindNetAttributeNameNumber(const CString & attributeName)
{
   map<CString, int>::iterator it = m_netAttributeNameIndexMap.find(attributeName);
   if(m_netAttributeNameIndexMap.end() != it)
   {
      //We found a record for this Attribute Name
      return (*it).second;
   }
   else
      return -1;
}


int EdaData::AddNetAttributeStringValue(CString attributeStringValue)
{
   int vectorIndex = FindNetAttributeStringValueNumber(attributeStringValue);
   if(vectorIndex != -1)
   {
      //We already have a record for this Attribute string value
      return vectorIndex;
   }
   else
   {
      //Attribute string value not found.Add and Update the map.
      m_netAttributeStringValues.push_back(attributeStringValue);
      int index = m_netAttributeStringValues.size() - 1;
      m_netAttributeStringValueIndexMap.insert(std::make_pair<const CString,int>(attributeStringValue,index));      
      return index;
   }
}


int EdaData::FindNetAttributeStringValueNumber(const CString & attributeStringValue)
{
   map<CString, int>::iterator it = m_netAttributeStringValueIndexMap.find(attributeStringValue);
   if(m_netAttributeStringValueIndexMap.end() != it)
   {
      //We found a record for this Attribute string value
      return (*it).second;
   }
   else
      return -1;
}


void EdaData::WriteOutputFile(CString stepPath)
{
   COperationProgress progress;
   progress.updateStatus("Writing \"eda/data\" file");
   fprintf(flog,"Writing \"eda/data\" file...\n");
   CString edaDataFilePath = stepPath + "/";
   edaDataFilePath += "eda/data";
   FILE* file = fopen(edaDataFilePath,"w");
   if(NULL != file)
   {
      CTime t;
      t = t.GetCurrentTime();
      fprintf(file,"#%s\n",t.Format("Date :%A, %B %d, %Y at %H:%M:%S"));
      fprintf(file,"#Created by CCE to ODB++ V.%s\n", getApp().getVersionString());
      fprintf(file,"#\n\n");
      OutputHeader(file);
      OutputLayerList(file);
      OutputNetAttributeNames(file);
      OutputNetAttributeStringValues(file);      
      OutputNetRecord(file);      
      OutputPkgSection(file);      
      fclose(file);
      CreateSumFiles(edaDataFilePath, "data");
   }
}


void EdaData::OutputHeader(FILE* file)
{
   if(NULL == file)
      return;

   fprintf(file,"HDR MENTOR CAMCAD V.%s\n", getApp().getVersionString()); //TODO
}


void EdaData::OutputLayerList(FILE* file)
{
   if(NULL == file)
      return;

   OdbLayerList odbLayerList = OdbGlobalJobPtr->GetOdbLayerList();
   if(odbLayerList.size())
      fprintf(file,"LYR");

   for(unsigned int i = 0; i < odbLayerList.size(); i++)
   {
      OdbLayerStruct *odbLayer = odbLayerList[i];
      fprintf(file," %s",odbLayer->name);
   }

   fprintf(file,"\n\n");
}


void EdaData::OutputNetAttributeNames(FILE* file)
{
   if(NULL == file)
      return;
   COperationProgress progress;
   progress.updateStatus("Writing net attribute names");
   fprintf(flog,"Writing net attribute names...\n");
   fprintf(file,"#\n");
   fprintf(file,"#Net attribute names\n");
   fprintf(file,"#\n");
   for(unsigned int i = 0; i < m_netAttributeNames.size(); i++)
   {
      fprintf(file,"#@%d %s\n",i,m_netAttributeNames[i]);
   }
   fprintf(file,"\n\n");
}


void EdaData::OutputNetAttributeStringValues(FILE* file)
{
   if(NULL == file)
      return;
   
   COperationProgress progress;
   progress.updateStatus("Writing net attribute string values");
   fprintf(flog,"Writing net attribute string values...\n");

   fprintf(file,"#\n");
   fprintf(file,"#Net attribute text strings\n");
   fprintf(file,"#\n");
   for(unsigned int i = 0; i < m_netAttributeStringValues.size(); i++)
   {
      fprintf(file,"#&%d %s\n",i,m_netAttributeStringValues[i]);
   }
   fprintf(file,"\n\n");
}


void EdaData::OutputNetRecord(FILE* file)
{
   if(NULL == file)
      return;
   COperationProgress progress;
   progress.updateStatus("Writing NET records", m_edaDataNetList.size());
   fprintf(flog,"Writing NET records...\n");
   for(unsigned int netNum = 0; netNum < m_edaDataNetList.size(); netNum++)
   {
      progress.incrementProgress();
      if(NULL == m_edaDataNetList[netNum])
         return;

      fprintf(file,"#NET %d\n",netNum);
      fprintf(file,"NET %s",m_edaDataNetList[netNum]->GetOdbNetName());

      //Write Attributes
      std::multimap<int, ODBAttribute* > attributeList =  m_edaDataNetList[netNum]->GetAttributeList();
      if(attributeList.size())
      {
         fprintf(file," ;");
         for(std::multimap<int, ODBAttribute* >::iterator it = attributeList.begin();
            it != attributeList.end();)
         {            
            ODBAttribute* attribute = (*it).second;
            if(NULL == attribute)
               continue;
            if(attribute->m_isString)
               fprintf(file,"%d=%d",attribute->m_attributeNum,attribute->m_attributeStringNum);
            else
               fprintf(file,"%d=%s",attribute->m_attributeNum,attribute->m_value);
            if(++it != attributeList.end())
               fprintf(file,",");
         }
      }
      fprintf(file,"\n");

      vector<SNTToePrintRecord *> toePrintRecordList = m_edaDataNetList[netNum]->GetToePrintRecordList();
      for(unsigned int toePrintNum = 0; toePrintNum < toePrintRecordList.size(); toePrintNum++)
      {
         SNTToePrintRecord* snttoePrintRecord = toePrintRecordList[toePrintNum];
         if(NULL == snttoePrintRecord)
            continue; // Should never happen
         if(snttoePrintRecord->m_featureIDList.size())
         {
            fprintf(file,"SNT TOP %c %d %d\n"
                                             ,snttoePrintRecord->m_isTop ? 'T' : 'B'
                                             ,snttoePrintRecord->m_compNum
                                             ,snttoePrintRecord->m_pinNum);
            for(unsigned int featureNum = 0; featureNum < snttoePrintRecord->m_featureIDList.size(); featureNum++)
            {
               FeatureIDRecord* featureIDRecord = snttoePrintRecord->m_featureIDList[featureNum];
               if(NULL != featureIDRecord)
                  OutputFeatureID(file, featureIDRecord);
            }
         }
      }

      vector<SNTViaRecord *> viaRecordList = m_edaDataNetList[netNum]->GetViaRecordList();
      for(unsigned int viaNum = 0; viaNum < viaRecordList.size(); viaNum++)
      {
         SNTViaRecord* sntViaRecord = viaRecordList[viaNum];
         if(NULL == sntViaRecord)
            continue; // Should never happen
         if(sntViaRecord->m_featureIDList.size())
         {
            fprintf(file,"SNT VIA\n");
            for(unsigned int featureNum = 0; featureNum < sntViaRecord->m_featureIDList.size(); featureNum++)
            {
               FeatureIDRecord* featureIDRecord = sntViaRecord->m_featureIDList[featureNum];
               if(NULL != featureIDRecord)
                  OutputFeatureID(file, featureIDRecord);
            }
         }

      }

      vector<FeatureIDRecord* > traceRecordList = m_edaDataNetList[netNum]->GetTraceRecordList();
      if(traceRecordList.size())
         fprintf(file,"SNT TRC\n");
      for(unsigned int traceNum = 0; traceNum < traceRecordList.size(); traceNum++)
      {
         FeatureIDRecord* featureIDRecord = traceRecordList[traceNum];
         if(NULL != featureIDRecord)
            OutputFeatureID(file, featureIDRecord);
      }

   }
   fprintf(file,"\n\n");
}


void EdaData::OutputFeatureID(FILE* file, FeatureIDRecord* featureIDRecord)
{
   if((NULL == featureIDRecord) || (NULL == file))
      return;

   // Record is not correct without layer num >= 0, so rather than make bad
   // output (it crashes the vPlan ODB reader) just skip the record. I'd rather get
   // a complaint about missing data than crashing ODB reader.
   if (featureIDRecord->m_layerNum < 0)
      return;

   char featureUseType = GetFeatureUseTypeChar(featureIDRecord->m_useType);
   fprintf(file,"FID %c %d %d\n",
                                featureUseType,
                                featureIDRecord->m_layerNum,
                                featureIDRecord->m_featureNum);
}

//---------------------------------------------------------------------

CString CInsertDataStructOb::GetSortableRefdes()
{
   if (m_sortableRefdes.IsEmpty())
   {
      // Could be done in one "if" clause above, but this way the test is faster
      // once the member string has been set.
      if (m_data != NULL && m_data->getDataType() == dataTypeInsert)
      {
         m_sortableRefdes = m_data->getInsert()->getSortableRefDes();
      }
   }

   return m_sortableRefdes;
}

//---------------------------------------------------------------------

int CInsertDataStructOb::CompareNoCase(CInsertDataStructOb &other)
{
   return this->GetSortableRefdes().CompareNoCase(other.GetSortableRefdes());
}

//---------------------------------------------------------------------

void CSortedInsertDataStructObMap::AddInsertData(DataStruct *data)
{
   // Put in all pins, even if we have dup refnames. Fudge the refname of dups.

   if (data != NULL && data->getDataType() == dataTypeInsert)
   {
      CString refname(data->getInsert()->getRefname());
      CInsertDataStructOb *dataOb = NULL;

      while (this->Lookup(refname, dataOb))
         refname += "X"; // Append X to fudge dup refname in map

      this->SetAt(refname, new CInsertDataStructOb(data));
   }
   // else other data types are now allowed.
}

//---------------------------------------------------------------------

int CSortedInsertDataStructObMap::AscendingRefnameSortFunc(const void *a, const void *b)
{
   CInsertDataStructOb* itemA = (CInsertDataStructOb*)(((SElement*) a )->pObject->m_object);
   CInsertDataStructOb* itemB = (CInsertDataStructOb*)(((SElement*) b )->pObject->m_object);

   return itemA->CompareNoCase(itemB);
}


//---------------------------------------------------------------------
//*rcf This needs work and refinement, to share with other aperture output section.
//*rcf But for now it is crude, as start of big refactoring.

void WriteAperture(FILE *file, ApertureShapeTag outputPadShape, double outputSizeA, double outputSizeB, InsertStruct *pinInsert, DataStruct *copperPadInsertData, CExtent &pinExtent, CPolyList *complexAperturePolyList, double rotationRadians)
{
   // Write out an aperture shape.
   // Incoming values are ccz pageunits.
   
   if (outputPadShape == apertureUndefined || outputPadShape == apertureUnknown || outputPadShape == apertureBlank)
   {
      // Output the overall extent because specific shape was not determined.
      WriteRectangle(pinExtent.getXmin(), pinExtent.getYmin(), pinExtent.getXsize(), pinExtent.getYsize(), file);
   }
   else
   {
      // Offset for pad that may not be inserted at 0,0 in padstack.
      double offsetX = 0.;
      double offsetY = 0.;
      if (copperPadInsertData != NULL)
      {
         offsetX = copperPadInsertData->getInsert()->getOriginX();
         offsetY = copperPadInsertData->getInsert()->getOriginY();
      }
      // Location of pad relative to component origin
      double padCenterX = pinInsert->getOriginX() + offsetX;
      double padCenterY = pinInsert->getOriginY() + offsetY;

      // Output specific shape
      if (outputPadShape == apertureRound)
      {
         // CR xc yc radius
         // sizeA is diameter, need to pass radius
         WriteCircle(padCenterX, padCenterY, outputSizeA / 2., file);
      }
      else if (outputPadShape == apertureSquare)
      {
         // SQ xc yc halfside
         // sizeA is length of side, need to pass half
         WriteSquare(padCenterX, padCenterY, outputSizeA / 2., file);
      }
      else if (outputPadShape == apertureRectangle)
      {
         // RC llx lly width length
         // sizeA is X size, sizeB is Y size
         //WriteSquare(pinInsert->getOriginX(), pinInsert->getOriginY(), outputSizeA / 2., file);
         double halfWidth = outputSizeA / 2.;
         double halfHeight = outputSizeB / 2.;
         WriteRectangle(padCenterX - halfWidth, padCenterY - halfHeight, outputSizeA, outputSizeB, file);
      }
      else if (outputPadShape == apertureComplex)
      {
         // Contour
         CTMatrix tmat;
         tmat.rotateRadians(rotationRadians);
         tmat.translate(padCenterX, padCenterY);
         complexAperturePolyList->transform(tmat);
         WritePolygons(complexAperturePolyList, file, true);
         delete complexAperturePolyList;
         complexAperturePolyList = NULL;
      }
   }
}

//------------

void ResolveAperture(EdaDataPkg* edaPkg, BlockStruct *insertedPad, bool padIsRotated, ApertureShapeTag &outputPadShape, double &outputSizeA, double &outputSizeB, CPolyList **complexAperturePolyList)
{
   // Get aperture shape and size to output, if simple aperture can be retained.
   // Otherwise get polylist to output and treat as complex aperture.

   outputSizeA = insertedPad->getSizeA();
   outputSizeB = insertedPad->getSizeB(); 

   // Now mark shape as supported or not, by setting outputPadShape or not.
   ApertureShapeTag insertedPadShape = insertedPad->getShape();

   // ODB++ can't do rotated simple apertures, so if pad is rotation (pin, pad in padstack, aperture) then
   // force handling to be complex style, i.e. output a contour ultimately, here make the polylist.
   if (padIsRotated)
      insertedPadShape = apertureComplex;

   switch (insertedPadShape)
   {
      // ODB++ can do these directly as parameterized simple apertures.
   case apertureRound:
   case apertureSquare:
   case apertureRectangle:
      outputPadShape = insertedPadShape;
      break;

      // These do not work out in standard complex aperture handling
      // because there is not a single non-overlapping poly that will
      // them. We could go bounding-box, but that is ugly. These are
      // all basically circular in shape, so we will coerce them to
      // circle. We need to fudge the dimensions a little to make
      // the right size circle..
   case apertureTarget:
      // For this one sizeA should be okay as-is.
      outputPadShape = apertureRound;
      break;
   case apertureThermal:
   case apertureDonut:
      // For these, use outer diameter, Usually sizeA but CAMCAD is flexible on
      // that. Just choose larger of sizes A and B.
      if (outputSizeB > outputSizeA)
         outputSizeA = outputSizeB;
      outputPadShape = apertureRound;
      break;

      // These work out okay when they are treated as complex because
      // they are single outer boundary shapes. Let fall through to
      // complex aperture handling.
   case apertureOblong:
   case apertureOctagon:
      // no break here on purpose.

      // Complex Aperture -- The Jack Of All Shapes
   case apertureComplex:
      {
         // ODB++ can do Contour for these.
         *complexAperturePolyList = insertedPad->getAperturePolys(OdbGlobalDoc->getCamCadData(), true);
         // Accept only if there is exactly one poly in the geometry.
         if ((*complexAperturePolyList) != NULL && (*complexAperturePolyList)->GetCount() == 1)
         {
            outputPadShape = apertureComplex;
         }
         else
         {
            // Can't use poly result, tell caller we failed.
            outputPadShape = apertureUndefined;
         }
      }
      break;

      // These are nothing
   case apertureBlank:
   case apertureUndefined:
   case apertureUnknown:
   default:
      {
         int jj = 0;
      }
      break;
   }
}
//------------

void EdaData::OutputPkgSection(FILE* file)
{
   if(NULL == file)
      return;

   COperationProgress progress;
   progress.updateStatus("Writing PKG records", m_edaDataPkgList.size());
   fprintf(flog,"Writing PKG records...\n");

   CString tempBlockName("TempEdaDataBlock"); // Used in case we need to mirror a block.
   int tempBlockFileNumber = -1;
   BlockTypeTag tempBlockType = blockTypeUndefined;
   
   double accuracy = get_accuracy(OdbGlobalDoc);
   int technologyKeywordIndex = OdbGlobalDoc->getStandardAttributeKeywordIndex(standardAttributeTechnology);
   for(unsigned int pkgNum = 0; pkgNum < m_edaDataPkgList.size(); pkgNum++)
   {
      progress.incrementProgress();
      EdaDataPkg* edaPkg = m_edaDataPkgList[pkgNum];
      if(NULL == edaPkg)
         continue;

      BlockStruct* compBlock = edaPkg->getCamcadBlockStruct();
      if(NULL == compBlock)
         continue;

      // Need to mirror geom if bottom built.
      if (edaPkg->GetIsBottomBuilt())
      {
         tempBlockFileNumber = compBlock->getFileNumber();
         tempBlockType = compBlock->getBlockType();
         BlockStruct *mirroredBlk = Graph_Block_On(GBO_OVERWRITE, tempBlockName, compBlock->getFileNumber(), 0L, compBlock->getBlockType());
         Graph_Block_Copy(compBlock, 0.0, 0.0, 0.0, TRUE /*mirror*/, 1, -1, FALSE, FALSE);
         Graph_Block_Off();
         compBlock = mirroredBlk;
      }

      double scaleFactor = edaPkg->GetScaleFactor();
      CTMatrix m;
      m.scale(scaleFactor);

      double pitch = get_pinpitch(compBlock, accuracy, OdbGlobalDoc);
      CExtent compExtent = compBlock->getExtent();
      if (!compExtent.isValid())
         compExtent = OdbGlobalDoc->blockExtents(&compBlock->getDataList(), 0.0, 0.0, 0.0, 0, 1, -1, false);

      // Ensure non-zero package size, vPlan import of ODB++ will quit with exception if
      // package has zero-size in either X or Y. Test against final output units value, not
      // internal page units val. Non-zero internal val that is small may turn into zero value
      // in final units.
      MakeValidOdbUnitsExtent(compExtent, scaleFactor);

      // Get the corners of the extent.
      double odbPkgSizeXMin = compExtent.getXmin();
      double odbPkgSizeXMax = compExtent.getXmax();
      double odbPkgSizeYMin = compExtent.getYmin();
      double odbPkgSizeYMax = compExtent.getYmax();

      // Write pkg header record.
      CString pkgName = edaPkg->GetName();
      fprintf(file,"#PKG %d\n", pkgNum);
      fprintf(file,"PKG %s %s %s %s %s %s",
         pkgName,
         GetFormatDecimalValue(pitch * pageUnitsToInchFactor * scaleFactor, pageUnitsInches),
         GetFormatDecimalValue(odbPkgSizeXMin, pageUnitsInches),
         GetFormatDecimalValue(odbPkgSizeYMin, pageUnitsInches),
         GetFormatDecimalValue(odbPkgSizeXMax, pageUnitsInches),
         GetFormatDecimalValue(odbPkgSizeYMax, pageUnitsInches));

      //Write Attributes, aka Properties
      vector<ODBAttribute* > attributeList =  edaPkg->GetAttributeList();
      if(attributeList.size())
      {
         fprintf(file," ;");
         unsigned int size = attributeList.size();
         for(unsigned int attributeNum = 0; attributeNum < size; attributeNum++)
         {            
            ODBAttribute* attribute = attributeList[attributeNum];
            if(attribute->m_isString)
               fprintf(file,"%d=%d",attribute->m_attributeNum,attribute->m_attributeStringNum);
            else
               fprintf(file,"%d=%s",attribute->m_attributeNum,attribute->m_value);
            if(attributeNum < size)
               fprintf(file,",");
         }
      }
      fprintf(file,"\n");

      // Write Outline.
      // Must have a non-zero size outline. Use poly from component outline if it
      // exists, and makes an acceptable poly. If not, then use overall extent to output
      // a rectangle. First shot at this used polyExtent area > 0, but that still failed
      // for a case that had okay width but height in 1E-8 range. So instead check
      // height and width separately, each must be greater than ODB threshold.
      CPolyList* orgPolyList = GetClosedPolygons(compBlock,graphicClassComponentOutline);

      // There can be only one outline, hence only one poly in the polylist. If there is 
      // more than one then keep the poly with the largest area. Could rubber band it if
      // this ends up failing, but this is much faster.
      if (NULL != orgPolyList && orgPolyList->GetCount() > 1)
      {
         while (orgPolyList->GetCount() > 1)
         {
            POSITION posH = orgPolyList->GetHeadPosition();
            POSITION posT = orgPolyList->GetTailPosition();
            CPoly *polyH = orgPolyList->GetAt(posH);
            CPoly *polyT = orgPolyList->GetAt(posT);
            double areaH = polyH->getArea();
            double areaT = polyT->getArea();

            if (areaH >= areaT)  // favor keeping the head
               orgPolyList->deleteAt(posT); // delete polyT
            else
               orgPolyList->deleteAt(posH); // delete polyH
         }
      }

      // Try shape from outline poly.
      // A poly has to have at least 3 points to make a decent closed outline shape.
      bool polyOutlineFailed = true; // Pessimist, prove otherwise.
      if (orgPolyList->hasPoints(3))
      {
         CExtent polyExtent = orgPolyList->getExtent();
         if (polyExtent.isValid())
         {
            // Poly size in ODB output units.
            double polyXSize = polyExtent.getXsize() * pageUnitsToInchFactor * scaleFactor;
            double polyYSize = polyExtent.getYsize() * pageUnitsToInchFactor * scaleFactor;
            // Threshold value for ODB output units.
            double odbThreshold = GetOdbThresholdValue();

            double llx, lly, urx, ury;
            if (orgPolyList->GetCount() == 1 && PolyIsRectangle(orgPolyList->GetHead(), &llx, &lly, &urx, &ury))
            {
               fprintf(file,"RC %s %s %s %s\n",
                  GetFormatDecimalValue(llx * pageUnitsToInchFactor * scaleFactor, pageUnitsInches),  // LL X
                  GetFormatDecimalValue(lly * pageUnitsToInchFactor * scaleFactor, pageUnitsInches),  // LL Y
                  GetFormatDecimalValue(polyXSize                                , pageUnitsInches),  // Width
                  GetFormatDecimalValue(polyYSize                                , pageUnitsInches)); // Height
               polyOutlineFailed = false;  // Output of RC based on poly counts as success.
            }
            else if (polyXSize >= odbThreshold && polyYSize >= odbThreshold)
            {
               // Comp outline based poly is okay, use it.
               CPolyList* polyList = CopyPoly(orgPolyList);
               polyList->transform(m);
               WritePolygons(polyList,file,true);
               FreePolyList(polyList);
               polyOutlineFailed = false;
            }
         }
      }
      
      // If polyoutline failed then use basic rectangle, size of component.
      if (polyOutlineFailed)
      {
         // Output rect based on overall extent.
         fprintf(file,"RC %s %s %s %s\n",
            GetFormatDecimalValue(compExtent.getXmin(), pageUnitsInches),   // LL X
            GetFormatDecimalValue(compExtent.getYmin(), pageUnitsInches),   // LL Y
            GetFormatDecimalValue(compExtent.getXsize(), pageUnitsInches),  // Width
            GetFormatDecimalValue(compExtent.getYsize(), pageUnitsInches)); // Height
      }

      // Done with orgPolyList, delete it.
      if (orgPolyList != NULL)
         delete orgPolyList;
      orgPolyList = NULL;

      // Process pin inserts.
      // Original code here output the pins in order they occured in datalist.
      // Mark wants them sorted by refname.
      CSortedInsertDataStructObMap sortedPins;
      POSITION compBlockDataPos = compBlock->getHeadDataInsertPosition();
      while (compBlockDataPos)
      {
         DataStruct *pinData = compBlock->getNextDataInsert(compBlockDataPos);
         if(pinData != NULL && pinData->isInsertType(insertTypePin))
         {
            sortedPins.AddInsertData(pinData);
         }
      }
      sortedPins.setSortFunction(CSortedInsertDataStructObMap::AscendingRefnameSortFunc);
      sortedPins.Sort();

      // Now process them in sorted order.
      CString *pinRefname;
      CInsertDataStructOb *insertDataOb;
      int unnamedPinCount = 0;
      for (sortedPins.GetFirstSorted(pinRefname, insertDataOb); insertDataOb != NULL; sortedPins.GetNextSorted(pinRefname, insertDataOb))
      {
         DataStruct *pinData = insertDataOb->GetData();

         if(pinData != NULL && pinData->isInsertType(insertTypePin))  // paranoid redundancy
         {
            InsertStruct* pinInsert = pinData->getInsert();
            if (NULL == pinInsert)
               continue;         
            BlockStruct* pinBlock = OdbGlobalDoc->Find_Block_by_Num(pinInsert->getBlockNumber());

            Attrib *attrib;
            CString technology = "S"; //Default is SMD
            if(NULL != pinData->getAttributesRef())
            {
               attrib = is_attvalue(OdbGlobalDoc, pinData->getAttributesRef(), ATT_TECHNOLOGY, 1);
               if(NULL != attrib)
               {
                  technology = OdbGlobalDoc->getAttributeStringValue(attrib);
                  technology = !technology.CompareNoCase("THRU") ? "T" : "S";
               }
            }

            // DR 851234 Brought us a CCZ with pins with blank refname. ODB can't handle that. 
            // Ensure non-blank pin name. If it has no name then it can't be in netlist, so this
            // is probably not a big deal.
            // Keep this in sync with other place that has this comment. DR 851234.
            CString validPinName(pinInsert->getRefname());
            validPinName.Trim();
            if (validPinName.IsEmpty())
               validPinName.Format("Unnamed%02d", ++unnamedPinCount);
            // Can't have white space in the pin name since record is space delimited, and apparantly
            // we can't just quote it.
            validPinName.Replace(" ",  "_");
            validPinName.Replace("\t", "_");

            fprintf(file,"PIN %s %s %s %s 0 U U\n",
               validPinName,
               technology,
               GetFormatDecimalValue(pinInsert->getOriginX() * pageUnitsToInchFactor * scaleFactor, pageUnitsInches),
               GetFormatDecimalValue(pinInsert->getOriginY() * pageUnitsToInchFactor * scaleFactor, pageUnitsInches));

            // This will get used later, but is set here for the proximity to the PIN record output.
            // Notice that the PIN output above has xy position, but does not have a rotation.
            // The shape defined in the eda/data for the pin is the final resolved single outline.
            // We can propogate the simple aperture types to ODB++ so long as we do not have to
            // rotate them. CCZ might have rotation in three places:
            // - the pin insert (aka padstack) may be inserted with rotation
            // - the pad insert within the padstack may have rotation.
            // - the aperture itself may be defined with rotation (e.g. rect 3x5 rotated 22 degrees).
            bool pinInsertRotated = pinInsert->getAngleRadians() != 0.;
            double resolvedRotation = pinInsert->getAngleRadians();


            // Get page units extent of pin block.
            CExtent pinExtent;

            // Favor output of exact shape. If that does not work out then output padstack extent as RC shape.
            ApertureShapeTag outputPadShape = apertureUndefined;
            double outputSizeA = 0.;
            double outputSizeB = 0.;
            DataStruct *copperPadInsertData = NULL;
            CPolyList *complexAperturePolyList = NULL;

            if (NULL != pinBlock)
            {
               // Get surface pad

               if (getCopperPad(OdbGlobalDoc->getCamCadData(),
                  /*DataStruct& component,*/ *pinBlock, true /*top*/, pinData->getLayerIndex(), copperPadInsertData))
               {
                  bool padInsertInPadstackRotated = copperPadInsertData->getInsert()->getAngleRadians() != 0.;
                  resolvedRotation += copperPadInsertData->getInsert()->getAngleRadians();
                  
                  BlockStruct *insertedPad = OdbGlobalDoc->getBlockAt( copperPadInsertData->getInsert()->getBlockNumber() );
                  if (insertedPad != NULL)
                  {
                     // That block is the aperture, so find out if aperture is rotated.
                     bool apertureRotated = insertedPad->getRotationRadians() != 0.;
                     // -> Not this one, it is handled in ap->polylist getter   resolvedRotation += insertedPad->getRotationRadians();

                     // We can now determine if pad is rotated in any of those three ways.
                     bool padIsRotated = pinInsertRotated || padInsertInPadstackRotated || apertureRotated;

                     // Just get all the sizes, some not relevant depending on shape, that is okay here.
                     //outputSizeA = insertedPad->getSizeA();
                     //outputSizeB = insertedPad->getSizeB();

                     // If pad is rotated we have to output contour. That is essentially same
                     // outputting a complex aperture.
                     //if (padIsRotated)
                     //{
                        // Pad is rotated, ODB++ can not do with with simple apertures. Force to complex/contour.
                        // Right now the test is rotated or not. We actually could do some things like...
                        // If circle rotation does not matter.
                        // If square rotation by 90's does not matter.
                        // If rectangle rotation by 180 does not matter.
                        // But for now  it is all or nothing.
                      //  int jj = 0;
                     //}
                     //else
                     {
                        // No rotation, so chance we can keep simple aps, or get outline polylist if we can't.
                        ResolveAperture(edaPkg, insertedPad, padIsRotated, outputPadShape, outputSizeA, outputSizeB, &complexAperturePolyList);
                        if (outputPadShape == apertureUnknown || outputPadShape == apertureUndefined)
                        {
                           // Resolve failed
                           int jjjj = 0;
                        }
                        else
                        {
                           // Success
                           int jjjjk = 0;
                        }
                     }
                  }
               }
               else
               {
                  int jj = 0;
               }
            }

            // Get extents of papdstack
            if(NULL != pinBlock)
            {
               pinBlock->calculateBlockExtents(OdbGlobalDoc->getCamCadData(), false, NULL);
               pinExtent = pinBlock->getExtent(OdbGlobalDoc->getCamCadData());
            }
            if (!pinExtent.isValid())
               pinExtent = OdbGlobalDoc->blockExtents(&compBlock->getDataList(), 0.0, 0.0, 0.0, 0, 1, -1, false);

            // Transform pin block extent to position in component.
            CTMatrix mt = pinInsert->getTMatrix();
            pinExtent.transform(mt);

            // Write it
            // Negation of angle accounts to +rotation being opposite direction between ODB and CCZ.
            double normalizedRads = normalizeRadians( -resolvedRotation );
            WriteAperture(file, outputPadShape, outputSizeA, outputSizeB, 
               pinInsert, copperPadInsertData, pinExtent, complexAperturePolyList, normalizedRads);

         }
      }

   }

   // Clean up, delete temp block if we made one.
   // Maybe later.
   //BlockStruct *tempBlock = Graph_Block_Exists(doc, tempBlockName, tempBlockFileNumber, tempBlockType));
}



bool EdaData::getCopperPad(CCamCadData& camCadData,
   BlockStruct& padstackGeometry, bool placedTopFlag, bool topFlag, int insertlayer, DataStruct*& copperPad) const
{
   //  topFlag   placedTopFlag   surfaceType
   //   f                f          top
   //   f                t          bottom
   //   t                f          bottom
   //   t                t          top

   bool topSurfaceTypeFlag = (topFlag == placedTopFlag);

   BlockStruct *surfacePad = NULL;
   DataStruct *surfacePadInsertData = NULL;

   for (CDataListIterator dataListIterator(padstackGeometry,dataTypeInsert); surfacePadInsertData == NULL && dataListIterator.hasNext(); )
   {
      DataStruct* pad = dataListIterator.getNext();
      InsertStruct* padInsert = pad->getInsert();
      BlockStruct* padGeometry = camCadData.getBlock(padInsert->getBlockNumber());

      if (! padGeometry->isAperture())
      {
         continue;
      }

      LayerStruct* layer = camCadData.getLayer(pad->getLayerIndex());
      if(layer && layer->isFloating() && insertlayer != -1)
      {
         layer = camCadData.getLayer(insertlayer);
      }

      if (! placedTopFlag)
      {
         layer = camCadData.getLayer(layer->getMirroredLayerIndex());
      }

      bool notInstantiatedFlag = ( 
         ((layer->getLayerFlags() & LY_TOPONLY    ) != 0 && !placedTopFlag) ||
         ((layer->getLayerFlags() & LY_BOTTOMONLY ) != 0 &&  placedTopFlag) ||
         (layer->getNeverMirror() && !placedTopFlag) ||
         (layer->getMirrorOnly()  &&  placedTopFlag)     );

      if (notInstantiatedFlag)
      {
         continue;
      }

      bool instantiatedOnSurfaceLayerFlag = false;

      switch (layer->getLayerType())
      {
      case LAYTYPE_SIGNAL_TOP:
      case LAYTYPE_PAD_TOP:
      case LAYTYPE_TOP:
         instantiatedOnSurfaceLayerFlag =  topFlag;
         break;

      case LAYTYPE_SIGNAL_BOT:
      case LAYTYPE_PAD_BOTTOM:
      case LAYTYPE_BOTTOM:
         instantiatedOnSurfaceLayerFlag = !topFlag;
         break;

      case LAYTYPE_SIGNAL_ALL:
      case LAYTYPE_PAD_ALL:
      case LAYTYPE_ALL:
      case LAYTYPE_PAD_OUTER:
      case LAYTYPE_SIGNAL_OUTER:
         instantiatedOnSurfaceLayerFlag = true;
         break;
      }

      if (!instantiatedOnSurfaceLayerFlag)
      {
         continue;
      }

      surfacePadInsertData = pad;
   }

   if (surfacePadInsertData != NULL)
   {
      //copperPad = camCadDatabase.getCamCadData().getNewDataStruct(*(surfacePad.getPad()));
      surfacePad = camCadData.getBlock( surfacePadInsertData->getInsert()->getBlockNumber() );
   }
   copperPad = surfacePadInsertData;

   return (copperPad != NULL);
}

bool EdaData::getCopperPad(CCamCadData& camCadData,
   /*DataStruct& component,*/ BlockStruct& padstackGeometry, bool topFlag, int insertlayer, DataStruct*& copperPad) const
{
   bool placedTopFlag = true;

   return getCopperPad(camCadData, padstackGeometry, placedTopFlag, topFlag, insertlayer, copperPad);
}

EdaData::~EdaData()
{
   vector<EdaDataNet* >::iterator edaDataNetListIterator = m_edaDataNetList.begin();
   for( ; edaDataNetListIterator != m_edaDataNetList.end() ; edaDataNetListIterator++)
   {
      if(NULL != (*edaDataNetListIterator))
      {
         delete (*edaDataNetListIterator);
         (*edaDataNetListIterator) = NULL;
      }
   }

   vector<EdaDataPkg* >::iterator edaDataPkgListIterator = m_edaDataPkgList.begin();
   for( ; edaDataPkgListIterator != m_edaDataPkgList.end() ; edaDataPkgListIterator++)
   {
      if(NULL != (*edaDataPkgListIterator))
      {
         delete (*edaDataPkgListIterator);
         (*edaDataPkgListIterator) = NULL;
      }
   }  
}

CString GetODBLayerName(CString camcadLayerName, bool mirrorLayer)
{
   CString ODBLayerName;
   LayerStruct* layer = OdbGlobalDoc->getLayerNoCase(camcadLayerName);
   ODBLayerName = GetODBLayerName(layer, mirrorLayer);
   return ODBLayerName;

}


CString GetODBLayerName(int layerIndex, bool mirrorLayer)
{
   CString ODBLayerName;
   LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
   ODBLayerName = GetODBLayerName(layer, mirrorLayer);
   return ODBLayerName;
}


CString GetODBLayerName(LayerStruct* layer, bool objectMirrored)
{
   CString ODBLayerName;// = ORPHAN_LAYER_NAME;

   if (layer != NULL)
   {
      if(layer->getElectricalStackNumber() > 0)
         return GetODBElectricalLayerName(layer, objectMirrored);

      int key = objectMirrored ? layer->getMirroredLayerIndex() : layer->getLayerIndex();
      OdbLayerStruct *odbLayerPtr = NULL;
      if (OdbGlobalJobPtr->m_layerMap.Lookup(key, (void*&)odbLayerPtr))
         if (odbLayerPtr != NULL)
            ODBLayerName = odbLayerPtr->name;


      // Just because the item we are getting a layer for is mirrored, we can't
      // assume we really want the mirror of the given layer. If the CCZ
      // layer does not actually have a mirror layer set up, then in CCZ
      // the item sticks with the assigned layer, and so ODB should as well.
      // If mirrored layer index is same as layer index, the layer has
      // no mirror (is mirrored to self).
#ifdef DEADCODE
      // Keeping for easy reference for a little while, while these big layer handing changes settle.

      int mirrorLayerIndx = layer->getMirroredLayerIndex();
      bool doLayerMirror = objectMirrored && (mirrorLayerIndx != layer->getLayerIndex());

      switch (layer->getLayerType())
      {
      case layerTypeSilkTop:
         {
            ODBLayerName = "sst";
            if(doLayerMirror)
               ODBLayerName = "ssb";
         }
         break;

      case layerTypeSilkBottom:
         {
            ODBLayerName = "ssb";
            if(doLayerMirror)
               ODBLayerName = "sst";
         }
         break;

      case layerTypeMaskTop:
         {
            ODBLayerName = "smt";
            if(doLayerMirror)
               ODBLayerName = "smb";
         }
         break;

      case layerTypeMaskBottom:
         {
            ODBLayerName = "smb";
            if(doLayerMirror)
               ODBLayerName = "smt";
         }
         break;

      case layerTypePasteTop:
         {
            ODBLayerName = "spt";
            if(doLayerMirror)
               ODBLayerName = "spb";
         }
         break;

      case layerTypePasteBottom:
         {
            ODBLayerName = "spb";
            if(doLayerMirror)
               ODBLayerName = "spt";
         }
         break;

      case layerTypeDrill:
         ODBLayerName = "drill";
         break;

      case layerTypeDocumentation:
         // Use the CCZ layer name for the ODB layer name for documentation layers.
         {
            CString validatedName;
            IsValidOdbLayerName(layer, validatedName);
            ODBLayerName = validatedName;
         }
         break;

      default:
         // Rout layer is type generic all, but could be other layers of that type too, so mapping by
         // layer type is not safe for that one. If the ccz layer name is "rout" then keep it (and if it is
         // then odds are this ccz was imported from ODB++ to start with). All others that come through
         // here get sent to ORPHAN_LAYER_NAME.
         if (layer->getName().CompareNoCase("rout") == 0)
            ODBLayerName = "rout";
         else
            ODBLayerName = ORPHAN_LAYER_NAME;
         break;
      }
#endif
   }


   return ODBLayerName;
}

CString GetODBElectricalLayerName(LayerStruct* layer, bool mirrored)
{
   CString ODBLayerName;// = ORPHAN_LAYER_NAME;  // Safe, legal name, would rather be "invalidElectLayer" but not
                                              // sure that is okay with ODB++.

   if(layer != NULL && layer->getElectricalStackNumber() > 0)
   {
      int key = mirrored ? layer->getMirroredLayerIndex() : layer->getLayerIndex();
      OdbLayerStruct *odbLayerPtr = NULL;
      if (OdbGlobalJobPtr->m_layerMap.Lookup(key, (void*&)odbLayerPtr))
         if (odbLayerPtr != NULL)
            ODBLayerName = odbLayerPtr->name;


#ifdef DEADCODE
      // Keeping for easy reference for a little while, while these big layer handing changes settle.

      //Electrical stackup Layer
      int stackupNumber;
      int maxElectricalStackupNum = OdbGlobalDoc->getMaxElectricalLayerNumber();
      if(!mirrored)
         stackupNumber = layer->getElectricalStackNumber();
      else
      {
         // This is mirroring the top and bottom electrical stack, but leaving all 
         // the inner layers as original. That is how I found it. It does not seem
         // correct, but since we don't have a complaint (yet) I am leaving it
         // as found.
         int electricalStackupNum = layer->getElectricalStackNumber();
         if(electricalStackupNum == maxElectricalStackupNum)
            stackupNumber = 1;
         else if(electricalStackupNum == 1)
            stackupNumber = maxElectricalStackupNum;
         else
            stackupNumber = electricalStackupNum;
      }

      CString stacknumString;
      stacknumString.Format("%d", stackupNumber);
      ODBLayerName = "layer_" + stacknumString;
#endif

   }

   return ODBLayerName;
}

int GetODBLayerNumber(CString odbLayerName)
{
   OdbLayerList& odbLayerList = OdbGlobalJobPtr->GetOdbLayerList();
   for(unsigned int i = 0; i < odbLayerList.size(); i++)
   {
      OdbLayerStruct *odbLayer = odbLayerList[i];
      if(!odbLayer->name.CompareNoCase(odbLayerName))
         return i;
   }
   return -1; //odbLayerList.size() - 1; // Relies on ORPHAN_LAYER_NAME being the last one in the list.
}

//--------------------------------------------------------------------

void OdbJob::FillOdbLayerList()
{
   if (odbSettingsFile.GetUseCadLayerNames())
      FillOdbLayerListFROMDATA();
   else
      FillOdbLayerListCLASSIC();
}

//--------------------------------------------------------------------

void OdbJob::FillOdbLayerListFROMDATA()
{
   // FROMDATA uses the CAD data layer names, as closely as possible.
   // Some alterations for name validity may be applied.
   // Add the layer and build the map at the same time.

   // These two are normal, necessary, standard layers special to ODB, not likely in CAD. Just add them.
   m_matrixLayerArray.push_back(new OdbLayerStruct("comp_+_top", "BOARD", "COMPONENT",    "POSITIVE", "", "", "", layerTypeUnknown, 0) );
   m_matrixLayerArray.push_back(new OdbLayerStruct("comp_+_bot", "BOARD", "COMPONENT",    "POSITIVE", "", "", "", layerTypeUnknown, 0) );

   // Layers from CAD.

   // We could do this in passes, say electrical first, then others. Or some other kind of sorting.
   // For now we do it in order encountered.
   // If we get a 'rout" we'll need top and bottom electrical layer names, so do a scan and get 
   // those now.
   CString layer_1;
   CString layer_n;
   for (int layerIndex = 0; layerIndex < OdbGlobalDoc->getMaxLayerIndex(); layerIndex++)
   {
      LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
      if (layer != NULL)
      {
         if (layer->getElectricalStackNumber() == 1)
            IsValidOdbLayerName(layer, layer_1);
         else if (layer->getElectricalStackNumber() == OdbGlobalDoc->getMaxElectricalLayerNumber())
            IsValidOdbLayerName(layer, layer_n);
      }
   }

   // Now process the layers.

   for (int layerIndex = 0; layerIndex < OdbGlobalDoc->getMaxLayerIndex(); layerIndex++)
   {
      LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

      // Floating layers are excempt, they are special for CCZ format not really part of CAD.
      if (layer != NULL && !layer->isFloating())
      {
         CString validatedName;
         IsValidOdbLayerName(layer, validatedName);

         OdbLayerStruct *newOdbLayer = NULL;

         if (layer->getLayerType() == layerTypeUnknown)
         {
            // Special case for "rout", CCZ does not have a layer type, so special handling applied to this layer name.
            if (layer->getName().CompareNoCase("rout") == 0)
            {
               // Abusing layerTypeRedLine to stand in for non-existent layerTypeRout.
               newOdbLayer = new OdbLayerStruct("rout",       "BOARD", "ROUT",         "POSITIVE", layer_1, layer_n, "", layerTypeRedLine, 0);
               m_matrixLayerArray.push_back(newOdbLayer);
            }
            else
            {
               newOdbLayer = new OdbLayerStruct(validatedName, "MISC", "DOCUMENT",    "POSITIVE", "", "", "", layerTypeDocumentation, 0);
               m_matrixLayerArray.push_back(newOdbLayer);
            }
         }
         else if (layer->getElectricalStackNumber() > 0)
         {
            int electricalStackupNum = layer->getElectricalStackNumber();
            int maxElectricalStackupNum = OdbGlobalDoc->getMaxElectricalLayerNumber();

            newOdbLayer = new OdbLayerStruct;
            newOdbLayer->name = validatedName;
            newOdbLayer->context = "BOARD";
            newOdbLayer->cczLayerType = layerTypeSignal;
            newOdbLayer->electricalLayerNumber = electricalStackupNum;
            if((electricalStackupNum == 1) || (electricalStackupNum == maxElectricalStackupNum))
               newOdbLayer->type = "SIGNAL";
            else
            {
               CString odbLayerType = GetODBLayerType(electricalStackupNum);
               newOdbLayer->type = odbLayerType;
            }

            // Based on Spec, A dialog box appears informing the user that 
            // the unknown layer will be changed to DOCUMENT and the context 
            // of the layer changed to MISC. 
            // As of ODB++ version 7.0, the layer type MASK is fully supported 
            // along with user-defined layer subtypes.
            if(newOdbLayer->type.IsEmpty())
            {
               newOdbLayer->type = "MASK";
               newOdbLayer->addType = "UNKOWN GRAPHIC";
               newOdbLayer->context = "MISC";
            }
            newOdbLayer->polarity = "POSITIVE";
            newOdbLayer->startName ="";
            newOdbLayer->endName="";
            m_matrixLayerArray.push_back(newOdbLayer);
         }
         else
         {
            CString odbLayerType( GetOdbLayerType(layer->getLayerType()) );
            CString odbLayerContext("BOARD");
            if (odbLayerType.CompareNoCase("DOCUMENT") == 0)
               odbLayerContext = "MISC";

            newOdbLayer = new OdbLayerStruct(validatedName, odbLayerContext, odbLayerType,    "POSITIVE", "", "", "", layer->getLayerType(), 0);
            m_matrixLayerArray.push_back(newOdbLayer);
         }

         // Note that not all layers are specifically and directly mapped. The
         // newOdbLayer may be NULL sometimes, that is expected and okay.

         m_layerMap.SetAt(layer->getLayerIndex(), newOdbLayer);

      }
   }

}

//--------------------------------------------------------------------

CString OdbJob::GetOdbLayerType(LayerTypeTag cczLayerType)
{
   // Returns name of ODB type if there is a specific mapping.
   // Otherwise DOCUMENT layer type is the default catch-all.

   switch (cczLayerType)
   {
   case layerTypeSilkTop:     return "SILK_SCREEN";
   case layerTypeSilkBottom:  return "SILK_SCREEN";

   case layerTypePasteTop:    return "SOLDER_PASTE";
   case layerTypePasteBottom: return "SOLDER_PASTE";

   case layerTypeMaskTop:     return "SOLDER_MASK";
   case layerTypeMaskBottom:  return "SOLDER_MASK";
   }

   // Default
   return "DOCUMENT";
}



void OdbJob::FillOdbLayerListCLASSIC()
{
   // CLASSIC is the original RSI version that creates a fixed set of layers that match ODB++ 
   // conventions... NOTE that is conventions, not specifications. In reality layer names are
   // user definable and can be most anything, but this set is the one we started with, based
   // on the set seen most often in data samples in the past.

   // Add all the layers first, then create the map.

   for (OdbLayerList::iterator it = m_matrixLayerArray.begin(); it != m_matrixLayerArray.end(); it++)
   {
      delete (*it);
   }
   m_matrixLayerArray.clear();

   // Always add this one, it is where components go, it is fabricated during output, not found in CCZ.
   m_matrixLayerArray.push_back(new OdbLayerStruct("comp_+_top", "BOARD", "COMPONENT",    "POSITIVE", "", "", "", layerTypeUnknown, 0) );

   // Add these only if CCZ has such a layer.
   if (OdbGlobalDoc->getLayerArray().getLayerCount(layerTypeSilkTop) > 0)
      m_matrixLayerArray.push_back(new OdbLayerStruct("sst"       , "BOARD", "SILK_SCREEN",  "POSITIVE", "", "", "", layerTypeSilkTop, 0) );

   if (OdbGlobalDoc->getLayerArray().getLayerCount(layerTypePasteTop) > 0)
      m_matrixLayerArray.push_back(new OdbLayerStruct("spt"       , "BOARD", "SOLDER_PASTE", "POSITIVE", "", "", "", layerTypePasteTop, 0) );

   if (OdbGlobalDoc->getLayerArray().getLayerCount(layerTypeMaskTop) > 0)
      m_matrixLayerArray.push_back(new OdbLayerStruct("smt"       , "BOARD", "SOLDER_MASK",  "POSITIVE", "", "", "", layerTypeMaskTop, 0) );

   // Electrical layers, default names like layer_1, layer_2.. layer_n.
   int maxElectricalStackupNum = OdbGlobalDoc->getMaxElectricalLayerNumber();
   for(int electricalStackupNum = 1; electricalStackupNum <= maxElectricalStackupNum; electricalStackupNum++)
   {      
      OdbLayerStruct* matrixLayerPtr = new OdbLayerStruct;
      CString temp;
      temp.Format("%d",electricalStackupNum);
      CString odbLayerName = "layer_" + temp;
      matrixLayerPtr->name = odbLayerName;
      matrixLayerPtr->context = "BOARD";
      matrixLayerPtr->cczLayerType = layerTypeSignal;
      matrixLayerPtr->electricalLayerNumber = electricalStackupNum;
      if((electricalStackupNum == 1) || (electricalStackupNum == maxElectricalStackupNum))
         matrixLayerPtr->type = "SIGNAL";
      else
      {
         CString odbLayerType = GetODBLayerType(electricalStackupNum);
         matrixLayerPtr->type = odbLayerType;
      }

      // Based on Spec, A dialog box appears informing the user that 
      // the unknown layer will be changed to DOCUMENT and the context 
      // of the layer changed to MISC. 
      // As of ODB++ version 7.0, the layer type MASK is fully supported 
      // along with user-defined layer subtypes.
      if(matrixLayerPtr->type.IsEmpty())
      {
         matrixLayerPtr->type = "MASK";
         matrixLayerPtr->addType = "UNKOWN GRAPHIC";
         matrixLayerPtr->context = "MISC";
      }
      matrixLayerPtr->polarity = "POSITIVE";
      matrixLayerPtr->startName ="";
      matrixLayerPtr->endName="";
      m_matrixLayerArray.push_back(matrixLayerPtr);
   }


   // Add these only if CCZ has such a layer.
   if (OdbGlobalDoc->getLayerArray().getLayerCount(layerTypeMaskBottom) > 0)
      m_matrixLayerArray.push_back(new OdbLayerStruct("smb"       , "BOARD", "SOLDER_MASK",  "POSITIVE", "", "", "", layerTypeMaskBottom, 0) );

   if (OdbGlobalDoc->getLayerArray().getLayerCount(layerTypePasteBottom) > 0)
      m_matrixLayerArray.push_back(new OdbLayerStruct("spb"       , "BOARD", "SOLDER_PASTE", "POSITIVE", "", "", "", layerTypePasteBottom, 0) );
   
   if (OdbGlobalDoc->getLayerArray().getLayerCount(layerTypeSilkBottom) > 0)
      m_matrixLayerArray.push_back(new OdbLayerStruct("ssb"       , "BOARD", "SILK_SCREEN",  "POSITIVE", "", "", "", layerTypeSilkBottom, 0) );

   // Always add this one, it is where components go, it is fabricated during output, not found in CCZ.
   m_matrixLayerArray.push_back(new OdbLayerStruct("comp_+_bot", "BOARD", "COMPONENT",    "POSITIVE", "", "", "", layerTypeUnknown, 0) );

   CString layer_n;
   layer_n.Format("layer_%d",maxElectricalStackupNum);

   if (OdbGlobalDoc->getLayerArray().getLayerCount(layerTypeDrill) > 0)
   {
      m_matrixLayerArray.push_back(new OdbLayerStruct("drill",      "BOARD", "DRILL",        "POSITIVE", "layer_1", layer_n, "", layerTypeDrill, 0) );
   }

   // Abusing layerTypeRedLine to stand in for non-existent layerTypeRout.
   if (OdbGlobalDoc->getLayerNoCase("rout") != NULL)
   {
      LayerStruct *layer = OdbGlobalDoc->getLayerNoCase("rout");
      CString validatedName;
      IsValidOdbLayerName(layer, validatedName);
      m_matrixLayerArray.push_back(new OdbLayerStruct(validatedName,       "BOARD", "ROUT",         "POSITIVE", "layer_1", layer_n, "", layerTypeRedLine, 0) );
   }

   // WI 15765 Documentation Layers
   // Add any CCZ layers of type DOCUMENTATION as-is.
   // Also add DFT Top and Bottom as Documentation layers.
   // And Exposed Metal layers too, they are ccz layer type Unknown, so have to use name to recognize them.
   for (int layerIndex = 0; layerIndex < OdbGlobalDoc->getMaxLayerIndex(); layerIndex++)
   {
      LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

      if (layer != NULL && (
         layer->getLayerType() == layerTypeDocumentation ||
         layer->getLayerType() == layerTypeDftTop        ||
         layer->getLayerType() == layerTypeDftBottom     ||
         layer->getName().Compare(DFT_LAYER_EXPOSE_METAL_TOP) == 0 ||
         layer->getName().Compare(DFT_LAYER_EXPOSE_METAL_BOT) == 0
         ))
      {
         CString validatedName;
         IsValidOdbLayerName(layer, validatedName);
         m_matrixLayerArray.push_back(new OdbLayerStruct(validatedName, "MISC", "DOCUMENT",    "POSITIVE", "", "", "", layer->getLayerType(), 0) );
      }
   }

   // Catch-all Misc Document layer -- THIS SHOULD ALWAYS BE THE LAST LAYER ADDED, the lookup depends on it !!!
//   m_matrixLayerArray.push_back(new OdbLayerStruct(ORPHAN_LAYER_NAME, "MISC", "DOCUMENT",    "POSITIVE", "", "", "", layerTypeUnknown, 0) );

   // Fill the layer map, ccz layer index to ODBLayer ptr.
   CreateCczToOdbLayerMap();
   ApplyCczLayerNames();
   FixFromToLayerNames();

}

void OdbJob::CreateCczToOdbLayerMap()
{
   // The function above has increased in complexity a lot recently. And creating the map separately has
   // too, and has become an issue to keep them in sync. It looks inviting to move creation to here, just
   // create as needed as we go. The problem is that the order the ODB layers are created is important, the
   // board layers have to be created in the order they exist on the board. I.e. the top layers first,
   // then the electrical layers in order, then the bottom layers. The doc layers could come first or 
   // last, that part doe not seem to matter much.
   // On the other hand, the layers in CCZ are not ordered, so it would not always work to just create
   // them in the order they are in the CCZ layer array.
   // So we'll keep the two step process for now.

   for (int layerIndex = 0; layerIndex < OdbGlobalDoc->getMaxLayerIndex(); layerIndex++)
   {
      LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
      if (layer != NULL)
      {
         OdbLayerStruct *selectedOdbLayer = NULL;

         // Special case for "rout", CCZ does not have a layer type Rout, so map this one by name.
         if (layer->getName().CompareNoCase("rout") == 0)
         {
            selectedOdbLayer = GetOdbLayerByType(layerTypeRedLine); // Abusing layerTypeRedLine for the map, not expecting to really find this.
         }
         // Special case for Exposed Metal layers, too bad they don't have a specific type, AA makes them as Unknown.
         else if (layer->getName().Compare(DFT_LAYER_EXPOSE_METAL_TOP) == 0 ||
            layer->getName().Compare(DFT_LAYER_EXPOSE_METAL_BOT) == 0)
         {
            CString validatedName;
            IsValidOdbLayerName(layer, validatedName);
            selectedOdbLayer = GetOdbLayerByName(validatedName);
         }
         else if (layer->getElectricalStackNumber() > 0)
         {
            selectedOdbLayer = GetOdbLayerByElectricalStackup(layer->getElectricalStackNumber());
         }
         else if (layer->getLayerType() == layerTypeDocumentation ||
            layer->getLayerType() == layerTypeDftTop        ||
            layer->getLayerType() == layerTypeDftBottom)
         {
            // There can be mulitple layers of doc or dft types, find the odb layer for these by validated name.
            CString validatedName;
            IsValidOdbLayerName(layer, validatedName);
            selectedOdbLayer = GetOdbLayerByName(validatedName);
         }
         else if (layer->getLayerType() != layerTypeUnknown)  // Type unknown does not work for lookup by type, will get first in list which is probably wrong.
         {
            // Direct mapping of type
            selectedOdbLayer = GetOdbLayerByType(layer->getLayerType());
         }
         // Else no entry in map, layer is not exported as a direct layer mapping.

         // If no specific mapping by type then it gets the orphan layer.
   //      if (selectedOdbLayer == NULL) 
   //         selectedOdbLayer = m_matrixLayerArray[ m_matrixLayerArray.size()-1 ]; // The orphaned items layer.

         m_layerMap.SetAt(layer->getLayerIndex(), selectedOdbLayer);

      }
   }
}

void OdbJob::ApplyCczLayerNames()
{
   // Update ODB layer names to CCZ layer names for selected layer types.
   // The CCZ layer name can be used only when there is a single CCZ layer of given type present.
   // Otherwise, multiple CCZ layers are mapped to same ODB layer, so none stand out as the one to keep.
   // In that case, use the traditional ODB layer name, which is what the ODB layers are originally created with.
   // Signal layers get additional special treatment, further separated by electrical stackup number.

   for(unsigned int it = 0 ; it < m_matrixLayerArray.size(); it++)
   {
      OdbLayerStruct *odbLayer = m_matrixLayerArray[it];

      LayerTypeTag cczLayerType = odbLayer->cczLayerType;

      switch (cczLayerType)
      {
         // These layers can be processed strictly on layer type.
      case layerTypeSilkTop:
      case layerTypePasteTop:
      case layerTypeMaskTop:
      case layerTypeMaskBottom:
      case layerTypePasteBottom:
      case layerTypeSilkBottom:
      case layerTypeDrill:
         {
            int layerCount = OdbGlobalDoc->getLayerArray().getLayerCount(cczLayerType);
            // Update layer name only if there is exactly one of this type of layer in ccz layer list.
            if (layerCount == 1)
            {
               LayerStruct *cczLayer = OdbGlobalDoc->getLayerArray().getLayer(cczLayerType);
               if (cczLayer != NULL) // jic
               {
                  CString odbValidatedName;
                  IsValidOdbLayerName(cczLayer, odbValidatedName);                 
                  odbLayer->name = odbValidatedName;
               }
            }
         }
         break;

      case layerTypeSignal:
         {
            int electStackupNum = odbLayer->electricalLayerNumber;
            int layerCount = OdbGlobalDoc->getLayerArray().getElectLayerCount(electStackupNum);
            if (layerCount == 1)
            {
               LayerStruct *cczLayer = OdbGlobalDoc->getLayerArray().getElectLayer(electStackupNum);
               if (cczLayer != NULL)
               {
                  CString odbValidatedName;
                  IsValidOdbLayerName(cczLayer, odbValidatedName);                 
                  odbLayer->name = odbValidatedName;
               }
            }
         }
         break;

      default:
         // The rest leave as-is.
         break;
      }

   }
}

void OdbJob::FixFromToLayerNames()
{
   // Layer names may have changed after applying CCZ names so update the From and To
   // layers for Drill and Rout. These layers have From/To (aka Start/End) set to
   // the top and bottom electrical layer names.

   for(unsigned int it = 0 ; it < m_matrixLayerArray.size(); it++)
   {
      OdbLayerStruct *odbLayer = m_matrixLayerArray[it];
      if (odbLayer->cczLayerType == layerTypeDrill || odbLayer->cczLayerType == layerTypeRedLine)
      {
         OdbLayerStruct *startLayer = this->GetOdbLayerByElectricalStackup(1);
         OdbLayerStruct *endLayer = this->GetOdbLayerByElectricalStackup(OdbGlobalDoc->getMaxElectricalLayerNumber());

         odbLayer->startName = startLayer->name;
         odbLayer->endName = endLayer->name;
      }
   }
}

OdbLayerStruct *OdbJob::GetOdbLayerByType(LayerTypeTag cczLayerType)
{
   for(unsigned int it = 0 ; it < m_matrixLayerArray.size(); it++)
   {
      OdbLayerStruct *odbLayer = m_matrixLayerArray[it];

      if (odbLayer->cczLayerType == cczLayerType)
         return odbLayer;
   }

   return NULL; // Not found.
}

OdbLayerStruct *OdbJob::GetOdbLayerByName(CString odbLayerName)
{
   for(unsigned int it = 0 ; it < m_matrixLayerArray.size(); it++)
   {
      OdbLayerStruct *odbLayer = m_matrixLayerArray[it];

      if (odbLayer->name.CompareNoCase(odbLayerName) == 0)
         return odbLayer;
   }

   return NULL; // Not found.
}

OdbLayerStruct *OdbJob::GetOdbLayerByElectricalStackup(int electStackNum)
{
   for(unsigned int it = 0 ; it < m_matrixLayerArray.size(); it++)
   {
      OdbLayerStruct *odbLayer = m_matrixLayerArray[it];

      if (odbLayer->electricalLayerNumber == electStackNum)
         return odbLayer;
   }

   return NULL; // Not found.
}


void OdbJob::WriteMatrixFile()
{
   // While ODB in general favors lower case names, the content of the matrix file
   // is upper case in most samples we've seen. Odd. Okay, we'll do the same.

   COperationProgress progress;
   progress.updateStatus("Writing \"matrix\" file");
   fprintf(flog,"Writing \"matrix\" file...\n");
   
   CString matrixFilePath = m_jobPath;
   matrixFilePath += "/" ;
   matrixFilePath += "matrix";
   matrixFilePath += "/";
   matrixFilePath += "matrix";
   FILE * file = fopen(matrixFilePath,"w");
   if(NULL != file)
   {
      for(unsigned int it = 0 ; it < m_stepList.size(); it++)
      {
         if(NULL != m_stepList[it])
         {
            fprintf(file,"STEP {\n");
            fprintf(file,"   COL=%d\n",it+1);
            CString upperStepName(m_stepList[it]->GetStepName());
            upperStepName.MakeUpper();
            fprintf(file,"   NAME=%s\n", upperStepName);
            fprintf(file,"}\n\n");
         }
      }

      for(unsigned int it = 0 ; it < m_matrixLayerArray.size(); it++)
      {
         fprintf(file,"LAYER {\n");
         fprintf(file,"   ROW=%d\n",it+1);
         fprintf(file,"   CONTEXT=%s\n",m_matrixLayerArray[it]->context);
         fprintf(file,"   TYPE=%s\n",m_matrixLayerArray[it]->type);
         if(!m_matrixLayerArray[it]->addType.IsEmpty()) 
            fprintf(file,"   ADD_TYPE=%s\n",m_matrixLayerArray[it]->addType);

         CString upperLayerName(m_matrixLayerArray[it]->name);
         upperLayerName.MakeUpper();
         fprintf(file,"   NAME=%s\n",upperLayerName);

         fprintf(file,"   POLARITY=%s\n",m_matrixLayerArray[it]->polarity);

         CString upperStartName(m_matrixLayerArray[it]->startName);
         upperStartName.MakeUpper();
         fprintf(file,"   START_NAME=%s\n", upperStartName);

         CString upperEndName(m_matrixLayerArray[it]->endName);
         upperEndName.MakeUpper();
         fprintf(file,"   END_NAME=%s\n", upperEndName);

         fprintf(file,"   OLD_NAME=\n");
         fprintf(file,"}\n\n");
      }

      fclose(file);
      CreateSumFiles(matrixFilePath, "matrix");
   }
}


void OdbJob::WriteSymbolFiles()
{
   map<CString, OdbSymbol*>::iterator it = m_SymbolList.begin();
   CString symbolsFolder = m_jobPath;
   symbolsFolder += "/symbols";

   COperationProgress progress;
   progress.updateStatus("Writing job symbol files", m_SymbolList.size());

   // DR 765049 - A symbol that had no name resulted in a skipped folder creation.
   // That is, the nothing was appended to the path, but it was nothing. Then we
   // made a feature and .feature.sum file in the symbols folder level. 
   // It turns out vPlan does not like it when those files are there.
   // So now we are putting out a unnamed name with count. Not sure if
   // that is okay (changes ref in other data?) or if perhaps it already
   // couldn't be used, so should just be dropped?
   int unnamedCnt = 0;

   fprintf(flog,"Writing job symbol files...\n");
   for( ; it != m_SymbolList.end(); it++)
   {
      progress.incrementProgress();
      CString symName = (*it).first;

      OdbSymbol* odbSymbol = (*it).second;
      if(NULL == odbSymbol)
         continue;

      double scaleFactor = odbSymbol->GetScale();
      BlockStruct * block = odbSymbol->GetGeomBlock();
      if(NULL == block)
         continue;

      CString symbolFolder = symbolsFolder;
      symbolFolder += "/" + symName;
      if (!make_dir(symbolFolder))  continue;
      CString symbolFeatureFile = symbolFolder + "/";
      symbolFeatureFile += "features";
      FILE* file = fopen(symbolFeatureFile,"w");
      if(NULL != file)
      {
         CString status;
         status.Format("Writing feature file of symbol \"%s\"",symName);
         progress.updateStatus(status);
         fprintf(flog,"%s...\n",status);
         CPolyList* polyList = NULL;
         if(block->isAperture())
         {
            polyList = block->getAperturePolys(OdbGlobalDoc->getCamCadData()); // Will return NULL if shape is T_UNDEFINED
         }
         else
         {
            POSITION blockDataPos = block->getDataList().GetHeadPosition();
            while (blockDataPos)
            {
               DataStruct *blockData = block->getDataList().GetNext(blockDataPos);
               if(NULL == blockData)
                  continue;

               if(dataTypePoly == blockData->getDataType())
               {
                  CPolyList* srcPolyList = blockData->getPolyList();
                  if(NULL == srcPolyList)
                     continue;

                  if(NULL == polyList)
                     polyList = new CPolyList;

                  POSITION polyPos, pntPos;
                  CPoly *poly, *newPoly;
                  CPnt *pnt, *newPnt;
                  polyPos = srcPolyList->GetHeadPosition();
                  while (polyPos != NULL)
                  {
                     poly = srcPolyList->GetNext(polyPos);
                     newPoly = new CPoly;
                     newPoly->setWidthIndex(poly->getWidthIndex());
                     newPoly->setFilled(poly->isFilled());
                     newPoly->setClosed(poly->isClosed());
                     newPoly->setVoid(poly->isVoid());
                     newPoly->setThermalLine(poly->isThermalLine());
                     newPoly->setFloodBoundary(poly->isThermalLine());
                     newPoly->setHidden(poly->isHidden());
                     newPoly->setHatchLine(poly->isHatchLine());

                     polyList->AddTail(newPoly);

                     pntPos = poly->getPntList().GetHeadPosition();
                     while (pntPos != NULL)
                     {
                        pnt = poly->getPntList().GetNext(pntPos);
                        newPnt = new CPnt(*pnt);
                        newPoly->getPntList().AddTail(newPnt);
                     }
                  }
               }
            }
         }

         // Now if we have a polyList of the symbol write it out.
         // If block is aperture and shape is T_UNDEINFED, or aperture is complex but geometry
         // block is empty, the polyList can be NULL. Only write the stuff to output if
         // we actually got a polylist. EMpty features are okay (missing features are not okay).

         OdbFeatureFile* featureFile = new OdbFeatureFile;
         featureFile->AddSymbol("r0.005");

         if (polyList != NULL)
         {
            CTMatrix m;
            m.scale(scaleFactor);
            polyList->transform(m);
            char polarity = 'P';
            POSITION pos = polyList->GetHeadPosition();
            CPolyList* surfaceRecordPolyList = NULL;
            while (pos)
            {
               CPoly* poly = polyList->GetNext(pos);
               if (poly)
               {
                  if(poly->isClosed() && poly->isFilled())
                  {
                     //Accumulate all closed/filled poly is one polyList;
                     if(NULL == surfaceRecordPolyList)
                        surfaceRecordPolyList = new CPolyList;
                     surfaceRecordPolyList->AddTail(poly);
                  }
                  else
                  {
                     CPnt *p1, *p2;
                     POSITION pntPos = poly->getPntList().GetHeadPosition();
                     p2 = poly->getPntList().GetNext(pntPos);
                     int symNum = 0; // Default is zero width
                     double width;
                     if(poly->getWidthIndex() > -1)
                     {
                        width = OdbGlobalDoc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
                        if(width > SMALLNUMBER)
                        {
                           double widthInMils = pageUnitsToMilsFactor * width;
                           CString widthString;
                           widthString.Format("%s",GetFormatDecimalValue(widthInMils, pageUnitsMils));
                           CString symName = "r" + widthString;
                           symNum = featureFile->AddSymbol(symName);
                        }
                     }
                     while (NULL != pntPos)
                     {
                        p1 = p2;
                        p2 = poly->getPntList().GetNext(pntPos);

                        if (fabs(p1->bulge) > SMALLNUMBER)
                        {                     
                           //ARC add a Arc record
                           double sa, da, cx, cy, r;
                           da = atan(p1->bulge) * 4;
                           ArcPoint2Angle(p1->x, p1->y, p2->x, p2->y, da, &cx, &cy, &r, &sa);
                           FeatureRecord* feature = new FeatureRecord(FEATURE_ARC);
                           ArcRecord* recPtr = (ArcRecord*) feature->GetFeatureTypePtr();
                           recPtr->m_startPoint = CPoint2d(p1->x, p1->y);
                           recPtr->m_endPoint = CPoint2d(p2->x, p2->y);
                           recPtr->m_centerPoint = CPoint2d(cx, cy);
                           char clockWise = ( p1->bulge < 0.0 ) ? 'Y' : 'N';
                           recPtr->m_clockWise = clockWise;
                           recPtr->m_dcode = 0; //TODO may be
                           recPtr->m_polarity = polarity;
                           recPtr->m_symNum = symNum;
                           int index = featureFile->AddFeatureRecord(feature);                     
                        }
                        else
                        {
                           //Line record
                           FeatureRecord* feature = new FeatureRecord(FEATURE_LINE);
                           LineRecord* recPtr = (LineRecord*) feature->GetFeatureTypePtr();
                           recPtr->m_startPoint = CPoint2d(p1->x,p1->y);
                           recPtr->m_endPoint = CPoint2d(p2->x, p2->y);
                           recPtr->m_dcode = 0; //TODO may be
                           recPtr->m_polarity = polarity;
                           recPtr->m_symNum = symNum;
                           int index = featureFile->AddFeatureRecord(feature);                     
                        }
                     }
                  }
               }
            }

            if(NULL != surfaceRecordPolyList)
            {
               FeatureRecord* feature = new FeatureRecord(FEATURE_SURFACE);
               SurfaceRecord* recPtr = (SurfaceRecord*) feature->GetFeatureTypePtr();
               recPtr->m_polyList = surfaceRecordPolyList;
               recPtr->m_polarity = polarity;
               recPtr->m_dcode = 0; //TODO may be               
               int index = featureFile->AddFeatureRecord(feature);
            }
         }

         // Finalize feature
         featureFile->OutputToFile(file, &progress);
         delete featureFile;
         featureFile = NULL;
         fclose(file);
         CreateSumFiles(symbolFeatureFile, "features");
      }
   }
}


void OdbJob::CopyStandardFontsFile()
{
   // There is only one destination, but there are two possible sources.
   // The "classic" source for a standard install of CAMCAD is where
   // getUserPath() says. Usually this would be the install location for
   // CAMCAD.
   // But then along comes vPlan, with its non-standard install of CAMCAD.
   // The file is still where the exe is, but getUserPath() doesn't give that
   // location. (Maybe that is a bug that will need fixing, we'll see as we
   // work more with vPlan.)
   // So what we are going to do is try two places, first the classic, if that fails
   // then try more specifically where the exe is.

   // Destination, there is only one, always the same.
   CString odbFontsFolder( m_jobPath + "/fonts" );
   CString destFile = odbFontsFolder + "/standard";

   // Classic source.
   CString classicSrcFile = getApp().getUserPath();
   ConvertToForwardSlash(classicSrcFile);
   classicSrcFile += "/standard.odb";

   // Alternate source.
   CString altSrcFile = getApp().getCamcadExeFolderPath();
   ConvertToForwardSlash(altSrcFile);
   altSrcFile += "/standard.odb";

   // Try classic first.
   bool copySucceeded = (CopyFile(classicSrcFile, destFile, false) !=  0);
   if(!copySucceeded)
   {
      // Classic didn't work, try alternate.
      copySucceeded = (CopyFile(altSrcFile, destFile, false) !=  0);
      if(!copySucceeded)
      {
         CString message;
         message.Format("\nFailed to create \"standard\" font file in ODB++ output.\n");
         fprintf(flog,message); 

         message.Format("(*) Could not copy [%s] to [%s].\n", classicSrcFile, destFile);
         fprintf(flog,message);

         message.Format("(*) Could not copy [%s] to [%s].\n", altSrcFile, destFile);
         fprintf(flog,message);
      }
   }      
}


OdbJob::OdbJob(CString jobPath)
{
   m_jobPath = jobPath;
}

OdbJob::~OdbJob()
{
   OdbLayerList::iterator matrixLayerArrayIterator = m_matrixLayerArray.begin();
   for( ; matrixLayerArrayIterator != m_matrixLayerArray.end(); matrixLayerArrayIterator++)
   {
      if(NULL != (*matrixLayerArrayIterator))
      {
         delete (*matrixLayerArrayIterator);
         (*matrixLayerArrayIterator) = NULL;
      }
   }

   map<CString,OdbSymbol*>::iterator symbolListIterator = m_SymbolList.begin();
   for( ; symbolListIterator != m_SymbolList.end(); symbolListIterator++)
   {
      if( NULL != (*symbolListIterator).second)
      {
         delete (*symbolListIterator).second;
         (*symbolListIterator).second = NULL;
      }
   }
   
   vector<OdbStep* >::iterator stepListIterator = m_stepList.begin();
   for( ; stepListIterator != m_stepList.end(); stepListIterator++)
   {
      if(NULL != (*stepListIterator))
      {
         delete (*stepListIterator);
         (*stepListIterator) = NULL;
      }
   }
}

void OdbJob::AddStep(OdbStep* step)
{
   if(NULL != step)
      m_stepList.push_back(step);

}


CString OdbJob::GetJobPath()
{
   return m_jobPath;
}


bool OdbJob::IsPCBAlreadyProcessed(FileStruct* file)
{
   for(unsigned int it = 0 ; it < m_stepList.size(); it++)
   {
      if((NULL != m_stepList[it]) && (m_stepList[it]->GetStepFileStruct() == file))
      {
         return true;
      }
   }
   return false;
}


OdbLayerList& OdbJob::GetOdbLayerList()
{
   return m_matrixLayerArray;
}


void OdbJob::AddSymbol(CString symName, OdbSymbol* odbSymbol)
{
   if (symName.IsEmpty())
      symName = "panic";

   if(!IsSymbolPresent(symName))
      m_SymbolList.insert(std::make_pair<const CString,OdbSymbol*>(symName, odbSymbol));
}


bool OdbJob::IsSymbolPresent(CString symName)
{
   map<CString, OdbSymbol*>::iterator it = m_SymbolList.find(symName);
   if(m_SymbolList.end() == it)
      return false;
   else
      return true;
}


OdbStep::OdbStep(FileStruct* file, CString stepPath, CString stepName, bool panelStep, double scaleFactor)
: m_file(file)
, m_stepPath(stepPath)
, m_stepName(stepName)
, m_panelStep(panelStep)
, m_scaleFactor(scaleFactor)
{
   m_componentTopLayerFeatureFile = new ComponentLayerFeatureFile;
   m_componentBottomLayerFeatureFile = new ComponentLayerFeatureFile;
}

OdbStep::~OdbStep()
{
   map<CString,OdbFeatureFile* >::iterator layerFeatureFileListIterator = m_layerFeatureFileList.begin();
   for( ; layerFeatureFileListIterator != m_layerFeatureFileList.end(); layerFeatureFileListIterator++)
   {
      if(NULL != (*layerFeatureFileListIterator).second)
      {
         delete (*layerFeatureFileListIterator).second;
         (*layerFeatureFileListIterator).second = NULL;
      }
   }

   if(NULL != m_edaData)
   {
      delete m_edaData;
      m_edaData = NULL;
   }

   if(NULL != m_bomData)
   {
      delete m_bomData;
      m_bomData = NULL;
   }
   
   vector<StepRepeatInfo*>::iterator stepRepeatInfoIterator = m_stepRepeatInfo.begin();
   for( ; stepRepeatInfoIterator != m_stepRepeatInfo.end(); stepRepeatInfoIterator++)
   {
      if(NULL != (*stepRepeatInfoIterator))
      {
         delete (*stepRepeatInfoIterator);
         (*stepRepeatInfoIterator) = NULL;
      }
   }

   if(NULL != m_componentTopLayerFeatureFile)
   {
      delete m_componentTopLayerFeatureFile;
      m_componentTopLayerFeatureFile = NULL;
   }

   if(NULL != m_componentBottomLayerFeatureFile)
   {
      delete m_componentBottomLayerFeatureFile;
      m_componentBottomLayerFeatureFile = NULL;
   }   
}

void OdbStep::WriteProfileFile()
{
   DataStruct *panelOutline = NULL;
   GraphicClassTag graphicClass;
   if(m_panelStep)
      graphicClass = graphicClassPanelOutline;
   else
      graphicClass = graphicClassBoardOutline;

   CString profileFile = m_stepPath + "/";
   profileFile += "profile";
   CPolyList* orgPolyList = GetClosedPolygons(m_file->getBlock(), graphicClass);
   CPolyList* polyList = CopyPoly(orgPolyList);
   CTMatrix m;
   m.scale(m_scaleFactor);

   // It seems we may get too many polys sometimes. We are finding data that
   // has items of graphicClassBoardOutline that really are not part of the
   // outline per se, that is, not part of the actual exterior outline. Sometimes
   // they are things like fids with a circle of this class, sometimes they
   // are apparantly random "other graphics" that are not part of the actual
   // PCB outline at all. When vPlan imports the ODB++, it acts only on the 
   // first poly in this profile. Sometimes those other polys are first, and
   // so upon load into vPlan the PCB is oddly zoomed.
   //
   // There are potentially many ways to deal with this. The first one that comes
   // to mind is to favor polys that are in PCB datalist, over polys that are in
   // some inserted block's data list. That would work for the data set that
   // led to this work.
   //
   // The second one that comes to mind is select the largest poly and output that 
   // one and skip the rest.
   // A variation is to output the largest one first, and then the rest. 
   //
   // Both schemes are rather arbitrary. None of the characteristics being checked
   // guarantee that we are getting the correct, or best, poly. But both schemes
   // will work for the data set in hand. We're going with the poly size test
   // for now. It is not clear if there is any value in exporting the rest here at all, 
   // so we will go with the largest one only for now, and skip the rest.

#define LIMIT_TO_ONE_OUTLINE_POLY
#ifdef  LIMIT_TO_ONE_OUTLINE_POLY
   // Find largest poly, output only that one.
   // We'll use area as measure of polygon size.
   // Since the WritePolygons func wants a polylist, we'll work the existing
   // temp list. While there is more than one poly in the polylist, we
   // compare the size of the head and tail polys, and delete which ever is smaller.
   // We use the head and tail because their positions are easy to get.
   // At the end the remainging poly is the winner.
   //
   // It may seem a little wasteful to re-calc the area of same poly
   // multiple times. But we do not expect there to be very many (there are 3 in
   // one sample, 4 in the other, that led to this work) and all this happens only
   // one time per Step (once per PCB, once per Panel).

   if((NULL != polyList) && (!polyList->isEmpty()))
   {
      while (polyList->GetCount() > 1)
      {
         POSITION posH = polyList->GetHeadPosition();
         POSITION posT = polyList->GetTailPosition();
         CPoly *polyH = polyList->GetAt(posH);
         CPoly *polyT = polyList->GetAt(posT);
         double areaH = polyH->getArea();
         double areaT = polyT->getArea();
        
         if (areaH >= areaT)  // favor keeping the head
            polyList->deleteAt(posT); // delete polyT
         else
            polyList->deleteAt(posH); // delete polyH
      }
   }
#endif

   // Outputs all the polys in polylist
   if((NULL != polyList) && (!polyList->isEmpty()))
   {
      polyList->transform(m);

      POSITION posH = polyList->GetHeadPosition();
      CPoly *polyH = polyList->GetAt(posH);
      double areaH = polyH->getArea();

      FILE* file = fopen(profileFile,"w");
      if(NULL != file)
      {    
         //GetClosedPolygons will always return atleast one closed polygon.
         //Code to write the polygon record.
         fprintf(file,"#\n");
         fprintf(file,"#Layer features\n");
         fprintf(file,"#\n");
         WritePolygons(polyList, file,false);
         fclose(file);
         CreateSumFiles(profileFile, "profile");         
      }      
   }



   // Clean up at the end
   while(!polyList->isEmpty())
   {
      polyList->removeHead();
   }
   delete polyList;
}


int OdbStep::CreateStepDirStruct()
{
   if (!make_dir(m_stepPath))  return 0;


   CString buildpath = m_stepPath;
   buildpath += "/layers";
   if (!make_dir(buildpath))  return 0;

   OdbLayerList& odbLayerList = OdbGlobalJobPtr->GetOdbLayerList();
   for(OdbLayerList::iterator it = odbLayerList.begin(); it != odbLayerList.end(); it++)
   {
      OdbLayerStruct *odbLayer = *it;

      CString layerFolderPath = buildpath + "/";
      layerFolderPath += odbLayer->name;
      if(!make_dir(layerFolderPath)) return 0;
   }

   buildpath = m_stepPath;
   buildpath += "/netlists";
   if (!make_dir(buildpath))  return 0;

   buildpath += "/cadnet";
   if (!make_dir(buildpath))  return 0;

   buildpath = m_stepPath;
   buildpath += "/eda";
   if (!make_dir(buildpath))  return 0;
   
   return 1;
}


CString OdbStep::GetStepPath()
{
   return m_stepPath;
}


CString OdbStep::GetStepName()
{
   return m_stepName;
}


FileStruct* OdbStep::GetStepFileStruct()
{
   return m_file;
}

CString OdbStep::GetCurrentVariantName()
{
   // Get current Variant name from the current file.

	CString name;
   
   int keyword = OdbGlobalDoc->getStandardAttributeKeywordIndex(standardAttributeCurrentVariant);
   Attrib* attrib = NULL;
   if (m_file->getBlock() != NULL && m_file->getBlock()->lookUpAttrib(keyword, attrib))
   {
      name = attrib->getStringValue();
   }

   return name;
}

void OdbStep::Process()
{
   //Basic infrastructure of the step
   CreateStepDirStruct();
   WriteProfileFile();
   InitializeStepLayerFeatureFileList();
   m_edaData = new EdaData;

   // Affecting BOM is the current variant from CCZ if there is one. If not
   // call affecting BOM "camcad_bom".
   CString affectingBomName( this->GetCurrentVariantName() );
   if (affectingBomName.IsEmpty())
      affectingBomName = "camcad_bom";
   m_bomData = new BomData(affectingBomName);  // Primary BOM aka Affecting BOM.

   CString fileName = m_file->getName();
   CString statusString;
   statusString.Format("Processing file : %s",fileName);
   COperationProgress progress;   
   progress.updateStatus(statusString, m_file->getBlock()->getDataCount());

   // The file's mirror flag indicates both layer and graphic mirror.
   bool fileGraphicMirrored = m_file->isMirrored();
   bool fileLayerMirrored = fileGraphicMirrored;
	
   CTMatrix boardMatrix;
   boardMatrix.scale(m_scaleFactor);
   POSITION pos = m_file->getBlock()->getDataList().GetHeadPosition();
   while (NULL != pos)
   {
      progress.incrementProgress();
      DataStruct *data = m_file->getBlock()->getDataList().GetNext(pos);
      if(NULL == data)
         continue;

      switch(data->getDataType())
      {
      case dataTypeInsert:
         {
            if(NULL == data->getInsert())
               continue;

            // Experimental hack. This plus usage of insertedBlockType below makes THOLE in Orcad_6 appear in
            // ODB, previously it vanished. But this makes it into a component completely. What we really need
            // is to fix ProcessOtherInsertTypes to do a better job. The basic problem is some of the output
            // we want to keep is on COMPONENTOUTLINE layer, and we are not actually keeping that layer.
            // Fundementally it is a crummy way to create the data in the first place. Thanks to our Orcad reader.
            // No one else has noticed this problem yet, i.e. the THOLE being gone. Now atleast it is not gone.
            // Not clear yet if there is going to be an  issue with the type it ends up being this way.
            BlockTypeTag insertedBlockType = blockTypeUnknown;
            BlockStruct *insertedBlock = OdbGlobalDoc->getBlockAt( data->getInsert()->getBlockNumber() );
            if (insertedBlock != NULL)
               insertedBlockType = insertedBlock->getBlockType();


            if (data->getInsert()->getInsertType() == insertTypePcb)   
            {
               ProcessInsertTypePcb(data,boardMatrix);
            }
            else if(data->getInsert()->getInsertType() == insertTypeVia)
            {
               ProcessInsertTypeVia(data,boardMatrix);
            }
            else if (IsOdbComponentType(data))
            {
               ProcessInsertTypePcbComponent(data,boardMatrix);
            }
            else if((data->getInsert()->getInsertType() == insertTypeFiducial)
               || (data->getInsert()->getInsertType() == insertTypeDrillTool))
            {
               ProcessOtherPads(data,boardMatrix);
            }
            else if (insertedBlockType == blockTypePcbComponent)
            {
               ProcessRandomInsertTypeAbusingPcbComponentGeometry(data,boardMatrix);
            }
            else
            {
               ProcessOtherInsertTypes(data,boardMatrix);
            }
         }
         break;
      case dataTypeDraw:
         {
            //No output
         }
         break;
      case dataTypePoly:
         {
            ProcessPoly(data,boardMatrix, fileLayerMirrored);
         }
         break;
      case dataTypeText:
         {
            ProcessText(data,boardMatrix);
         }
         break;
      case dataTypeTool:
         {
            //No output
         }
         break;
      case dataTypePoint:
         {
            //No output
         }
         break;
      case dataTypeBlob:
         {
            //No output
         }
         break;
      case dataTypeUndefined:
         {
            //No output
         }
         break;
      default:
         {
         }
      }
   }
   fprintf(flog,"\nWriting files of step \"%s\"\n",m_stepName);
   m_edaData->WriteOutputFile(m_stepPath);
   WriteStepRepeatInfo();
   WriteComponentLayerFeatureFile(true);   //comp_+_top
   WriteComponentLayerFeatureFile(false);  //comp_+_bot
   WriteLayerFeatureFile();
   WriteCadNetlistInfo();
   
   // Primary BOM
   // BOM written only for PCB steps, and if used (not empty).
   if (!this->m_panelStep && this->m_bomData->IsUsed())
      WriteBOMFile(this->m_bomData);

   // Variant BOMs
   // Only for PCB steps, like primary BOM.
   if (!this->m_panelStep && this->ProcessVariants())
      WriteVariantBOMFiles();

   fprintf(flog,"\n");
}

bool OdbStep::IsOdbComponentType(DataStruct *compInsertData)
{
   // Returns true if insert type is to be processed as component for ODB.

   if (compInsertData != NULL)
   {
      return (
         compInsertData->isInsertType(insertTypePcbComponent)        || 
         compInsertData->isInsertType(insertTypeGenericComponent)    ||
         compInsertData->isInsertType(insertTypeMechanicalComponent)
         );
   }

   return false;
}

void OdbStep::ProcessInsertTypePcb(DataStruct *data, const CTMatrix& boardMatrix)
{
   if(NULL == data)
      return;

   int pcbBlockNumber = -1;
   FileStruct * file;

   pcbBlockNumber = data->getInsert()->getBlockNumber();
   for (POSITION pos = OdbGlobalDoc->getFileList().GetHeadPosition();NULL != pos;)
   {
      file = OdbGlobalDoc->getFileList().GetNext(pos);
      if ((blockTypePcb == file->getBlockType()) && (file->getBlock()->getBlockNumber() == pcbBlockNumber))
      {
         //CString stepName( this->GetStepName() + "_" + file->getName() );
         CString stepName( file->getName() );

         CString validStepName;
         IsValidODBEntityName(stepName, validStepName, "Step", file->getFileNumber());
         stepName = validStepName; // Just always use validated name.

         StepRepeatInfo* stepRepeatInfoPtr = new StepRepeatInfo;
         stepRepeatInfoPtr->name = stepName;
         stepRepeatInfoPtr->flip = data->getInsert()->getPlacedBottom();
         stepRepeatInfoPtr->angle = data->getInsert()->getAngleDegrees();
         stepRepeatInfoPtr->X = data->getInsert()->getOriginX();
         stepRepeatInfoPtr->Y = data->getInsert()->getOriginY();
         stepRepeatInfoPtr->mirror = data->getInsert()->getGraphicMirrored();
         AddStepRepeatInfo(stepRepeatInfoPtr);

         if(!OdbGlobalJobPtr->IsPCBAlreadyProcessed(file))
         {
            CString stepPath = OdbGlobalJobPtr->GetJobPath();
            stepPath += "/steps/";
            stepPath += stepName;               
            OdbStep * step = new OdbStep(file, stepPath, stepName, false, data->getInsert()->getScale());      
            OdbGlobalJobPtr->AddStep(step);
            step->Process();               
         }
      }
   }
}

void OdbStep::ProcessOtherInsertTypes(DataStruct *parentData, const CTMatrix& boardMatrix, bool propagatedLayerMirrored, bool keepSilkscreenOnly)
{
   if(NULL == parentData)
      return;

   InsertStruct *parentInsert = parentData->getInsert();
   int blockNumber = parentInsert->getBlockNumber();
   CTMatrix dataMatrix = parentData->getInsert()->getTMatrix();
   //dataMatrix.scale(boardMatrix.getScale());

   CTMatrix ex = dataMatrix * boardMatrix;
   dataMatrix = ex;


   BlockStruct* block = OdbGlobalDoc->Find_Block_by_Num(blockNumber);
   if(NULL != block)
   {
      if (block->isAperture())
      {
         InsertStruct *insert = parentData->getInsert();
         CTMatrix resultantMatrix = dataMatrix;  

         CPolyList* polyList = NULL;
         int layerIndex = parentData->getLayerIndex();
         LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

         bool layerIsSilkscreen = false;
         if (layer != NULL)
            layerIsSilkscreen = layer->getLayerType() == layerTypeSilkTop || layer->getLayerType() == layerTypeSilkBottom;
         if (keepSilkscreenOnly && !layerIsSilkscreen)
            return;
         
         // DR 822367 - Presents data where bottom side comps have LayerMirror=true and GraphicMirror=false, so stuff came out on wrong side.
         // Namely, buttom pads and comp outline came out on top side, because next line of code used GraphicMirror flag to
         // determine LayerMirror.
         // CString odbLayerName = GetODBLayerName(layerIndex, resultantMatrix.getBasesVector().getMirror()); // This line was a layer bug, layer is depending on graphic mirror, ignoring layer mirror.
         // BasesVector currently does not support layer mirror, only graphic mirror, but that is abused in a lot of places, like old line of code above.
         // Need to make layer determination based on LayerMirror, not GraphicMirror.
         CString odbLayerName = GetODBLayerName(layerIndex, insert->getLayerMirrored());
         if (odbLayerName.IsEmpty())
            return;

         LayerLog->LogExportedLayer(layer, insert->getLayerMirrored());

         if (resultantMatrix.getBasesVector().getMirror())
         {                  
            CBasesVector padbv = resultantMatrix.getBasesVector();
            padbv.set(padbv.getOrigin(),resultantMatrix.getBasesVector().getRotationDegrees(),false,padbv.getScale());
            resultantMatrix = padbv.getTransformationMatrix();                  
         }

         polyList = block->getAperturePolys(OdbGlobalDoc->getCamCadData());
         ProcessPoly(polyList, resultantMatrix, odbLayerName);


      }
      else
      {
         POSITION childDataPos = block->getDataList().GetHeadPosition();
         while (childDataPos)
         {
            DataStruct *childData = block->getDataList().GetNext(childDataPos);
            if(NULL == childData)
               continue;

            switch(childData->getDataType())
            {
            case dataTypeInsert:
               {
                  InsertStruct *childInsert = childData->getInsert();
                  if(NULL == childInsert)
                     continue;

                  CBasesVector basesVector = childInsert->getBasesVector();
                  basesVector.transform(dataMatrix);
                  CTMatrix resultantMatrix = basesVector.getTransformationMatrix();  
                  BlockStruct* subBlock = OdbGlobalDoc->Find_Block_by_Num(childInsert->getBlockNumber());
                  if(NULL == subBlock)
                     continue;
                  CPolyList* polyList = NULL;
                  int layerIndex = childData->getLayerIndex();
                  LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

                  // DR 822367 - was using graphic mirror to determine layer, must use layer mirror.
                  bool resultantLayerMirrored = parentInsert->getLayerMirrored() ^ childInsert->getLayerMirrored();

                  CString odbLayerName = GetODBLayerName(layerIndex, resultantLayerMirrored);
                  // No, don't check here, e.g. need to let floating layer through.
                  //if (odbLayerName.IsEmpty())
                  //   continue;

                  

                  if(resultantMatrix.getBasesVector().getMirror())
                  {                  
                     CBasesVector padbv = resultantMatrix.getBasesVector();
                     padbv.set(padbv.getOrigin(),resultantMatrix.getBasesVector().getRotationDegrees(),false,padbv.getScale());
                     resultantMatrix = padbv.getTransformationMatrix();                  
                  }

                  if (subBlock->isAperture())
                  {
                     bool layerIsSilkscreen = false;
                     if (layer != NULL)
                        layerIsSilkscreen = layer->getLayerType() == layerTypeSilkTop || layer->getLayerType() == layerTypeSilkBottom;
                     if (keepSilkscreenOnly && !layerIsSilkscreen)
                        continue;

                     if (!odbLayerName.IsEmpty())
                     {
                        LayerLog->LogExportedLayer(layer, resultantLayerMirrored);

                        polyList = subBlock->getAperturePolys(OdbGlobalDoc->getCamCadData());
                        ProcessPoly(polyList, resultantMatrix, odbLayerName);
                     }
                  }
                  else
                  {
                     // This all is not quite right, it is not dealing with floating layer properly. Not just here, all over the place.
                     // If someting is floating then we need it's parent's layer, as floater floats to parents layer.
                     ProcessOtherInsertTypes(childData, resultantMatrix);
                  }
               }
               break;
            case dataTypeDraw:
               {
                  //No output
               }
               break;
            case dataTypePoly:
               {
                  if (keepSilkscreenOnly)
                  {
                     int layerIndex = childData->getLayerIndex();
                     LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
                     if (layer == NULL || (layer->getLayerType() != layerTypeSilkTop && layer->getLayerType() != layerTypeSilkBottom))
                        continue;
                  }

                  bool resultantLayerMirrored = parentInsert->getLayerMirrored() ^ propagatedLayerMirrored;

                  ProcessPoly(childData, dataMatrix, resultantLayerMirrored);
               }
               break;
            case dataTypeText:
               {
                  if (keepSilkscreenOnly)
                  {
                     int layerIndex = childData->getLayerIndex();
                     LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
                     if (layer == NULL || (layer->getLayerType() != layerTypeSilkTop && layer->getLayerType() != layerTypeSilkBottom))
                        continue;
                  }

                  ProcessText(childData, dataMatrix);
               }
               break;
            case dataTypeTool:
               {
                  //No output
               }
               break;
            case dataTypePoint:
               {
                  //No output
               }
               break;
            case dataTypeBlob:
               {
                  //No output
               }
               break;
            case dataTypeUndefined:
               {
                  //No output
               }
               break;
            default:
               {
               }
            }
         }
      }
   }
}


void OdbStep::ProcessOtherPadstack(DataStruct *parentData, DataStruct *padstackData, const CTMatrix& boardMatrix)
{
   if (parentData != NULL && padstackData != NULL && padstackData->isInsertType(insertTypePin))
   {
      BlockStruct *padstackBlock = OdbGlobalDoc->Find_Block_by_Num(padstackData->getInsert()->getBlockNumber());

      if (padstackBlock != NULL)
      {
         POSITION padstackPos = padstackBlock->getHeadDataInsertPosition();
         while (padstackPos != NULL)
         {
            DataStruct *padInsertData = padstackBlock->getNextDataInsert(padstackPos);
            ProcessOnePad(parentData, padInsertData, boardMatrix);
         }
      }
   }
}

void OdbStep::ProcessOtherPads(DataStruct *parentData, const CTMatrix& boardMatrix)
{
   // Find constituent pads of Fiducials and Toolings, create feature record, add to layerFeature file
   // Update EdaDataNet record with the feature numbers of the record added to LayerFeature file.
   int blockNumber = parentData->getInsert()->getBlockNumber();
   CTMatrix padMatrix = parentData->getInsert()->getTMatrix();
   padMatrix.scale(boardMatrix.getScale());

   BlockStruct* padBlock = OdbGlobalDoc->Find_Block_by_Num(blockNumber); // Might be a padstack, not just a pad.
   if(NULL != padBlock)
   {
      POSITION padBlockDataPos = padBlock->getDataList().GetHeadPosition();
      while (padBlockDataPos)
      {
         DataStruct *padBlockData = padBlock->getDataList().GetNext(padBlockDataPos);
         if(NULL == padBlockData)
            continue;

         switch(padBlockData->getDataType())
         {
         case dataTypeInsert:
            {
               if (padBlockData->getInsert()->getInsertType() == insertTypePin)
               {
                  // Is a padstack, not just a pad
                  ProcessOtherPadstack(parentData, padBlockData, padMatrix);
               }
               else if (padBlockData->getInsert()->getInsertType() == insertTypePad ||
                  padBlockData->getInsert()->getInsertType() == insertTypeUnknown)
               {
                  // The usual normal pads.
                  ProcessOnePad(parentData, padBlockData, padMatrix);
               }
               // else ignore it.
            }
            break;
         case dataTypePoly:
            {
               ProcessPoly(padBlockData,padMatrix,parentData->getInsert()->getLayerMirrored());
            }
            break;
         case dataTypeText:
            {
               ProcessText(padBlockData,padMatrix);
            }
            break;
         default:
            {
            }
         }
      }
   }
}

void OdbStep::ProcessOnePad(DataStruct *parentData, DataStruct *padData, const CTMatrix& padMatrix)
{
   InsertStruct *padInsert = padData->getInsert();
   if(NULL == padInsert)
      return;

   BlockStruct* padBlock = OdbGlobalDoc->Find_Block_by_Num(padInsert->getBlockNumber());
   if(NULL == padBlock)
      return;

   OdbPadUsageTag padUsageType = InsertTypeToPadUsage(parentData->getInsert()->getInsertType());
   OdbFeatureFile* featureFilePtr = NULL;

   CPoint2d point = padInsert->getOrigin2d();
   double rotation = padInsert->getAngleDegrees();
   //padMatrix.transform(rotation);
   padMatrix.transform(point);
   double scale = padMatrix.getScale();
   int symNum, odbLayerNum;
   FeatureUseType featureUseType;

   // Two kinds of mirror flags in CCZ, each has separate affect on ODB.
   bool graphicMirrored = parentData->getInsert()->getGraphicMirrored();
   bool layerMirrored = parentData->getInsert()->getLayerMirrored();

   //In ODB++ this rotation if clockwise
   if(graphicMirrored)
      rotation = normalizeDegrees(360.0 + rotation); 
   else
      rotation = normalizeDegrees(360.0 - rotation); 

   int layerIndex = padData->getLayerIndex();
   LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

   if(padBlock->isDrillHole())
   {
      OdbLayerStruct *odbLayer = OdbGlobalJobPtr->GetOdbLayerByType(layerTypeDrill);
      if (odbLayer != NULL)
      {
         odbLayerNum = GetODBLayerNumber(odbLayer->name);
         featureFilePtr = GetLayerFeatureFilePtr(odbLayer->name);
      }

      if (odbLayerNum < 0)
         return;

      if(NULL == featureFilePtr)
         return;

      LayerLog->LogExportedLayer(layer, false);

      double toolSize = padBlock->getToolSize();
      double toolSizeInMils = pageUnitsToMilsFactor * toolSize;
      CString toolSizeString;
      toolSizeString.Format("%s",GetFormatDecimalValue(toolSizeInMils, pageUnitsMils));
      CString symName = "r" + toolSizeString;
      symNum = featureFilePtr->AddSymbol(symName);

      featureUseType = Hole;
      padUsageType = padUsageToolingHole;
   }                
   else
   {
      CString odbLayerName = GetODBLayerName(layerIndex, layerMirrored);
      if (odbLayerName.IsEmpty())
         return;

      //odbLayerNum = GetODBLayerNumber(odbLayerName);
      featureFilePtr = GetLayerFeatureFilePtr(odbLayerName);
      if(NULL == featureFilePtr)
         return;

      LayerLog->LogExportedLayer(layer, layerMirrored);

      // Use standard symbol if aperture params allow, otherwise define
      // a custom symbol.
      CString symName( GetOdbStandardSymbolName(padBlock) );
      if (symName.IsEmpty())
      {
         symName = GetOdbSymbolName(padBlock,scale);

         if(!OdbGlobalJobPtr->IsSymbolPresent(symName))
         {
            OdbSymbol* odbSymbol = new OdbSymbol(symName, padBlock, scale);
            OdbGlobalJobPtr->AddSymbol(symName, odbSymbol);
         }
      }
      symNum = featureFilePtr->AddSymbol(symName);
      featureUseType = Copper;
   }

   FeatureRecord* featureRecordPtr = new FeatureRecord(FEATURE_PAD);
   PadRecord* recPtr = (PadRecord*) featureRecordPtr->GetFeatureTypePtr();               
   recPtr->m_centerPoint = point;
   recPtr->m_mirror = padInsert->getGraphicMirrored();
   recPtr->m_rotation = rotation;
   recPtr->m_dcode = 0;
   recPtr->m_polarity = 'P'; 
   recPtr->m_symNum = symNum;
   recPtr->m_padUsageType = padUsageType;
   int index = featureFilePtr->AddFeatureRecord(featureRecordPtr);
}

CString OdbStep::GetOdbSymbolName(BlockStruct *symBlock, double scale)
{
   // Returns name of block if non-blank, otherwise made up name based on block number.
   // DR 765049, this is the essence of the fix for.

   // DR 851236, Make sure name does not collide with standard ODB symbol name format.
   // User symbols can't have names that look like ODB standard symbol names.

   CString symName;

   if (symBlock != NULL)
   {
      symName = symBlock->getName();
      symName.Trim();
      // DR 765049 - Can't have blank symbol names, make non-blank.
      if (symName.IsEmpty())
      {
         symName.Format("unnamedGeom%d", symBlock->getBlockNumber());
         // Update CCZ?   symBlock->setName(symName);
      }

      // DR 851236
      if (IsOdbStandardSymbolName(symName))
         symName.Format("usersym_%s", symBlock->getName());

      CString validSymbolName;
      IsValidODBEntityName(symName,validSymbolName, "Symbol", symBlock->getBlockNumber());
      symName = validSymbolName;  // Just always use validated name.

      if(fabs(scale - 1.0) > SMALLNUMBER)
      {
         symName += "_";
         symName += "scale_";
         CString scaleFactor;
         scaleFactor.Format("%f",scale);
         symName +=scaleFactor;
      }
   }
   return symName;
}

bool OdbStep::IsUsefulPadstackData(DataStruct *data)
{
   // The original thought for a name here was IsPadOrDrill(), but having an insertTypeUnknown
   // returned as pad=true seems too weird. It is weird, and that is actually correct.
   // It is usually the case that a pad within a padstack data list is an insertTypeUnknown.

   if (data != NULL)
   {
      // Obvious
      if (data->isInsertType(insertTypePad))
         return true;

      // Looks strange, but in context of data list within a padstack, pads are almost always
      // insertTypeUnknown. In retrospect, that was a lame idea.
      if (data->isInsertType(insertTypeUnknown))
         return true;

      // Obvious
      if (data->isInsertType(insertTypeDrillHole))
         return true;
   }

   // All else.
   return false;
}

void OdbStep::ProcessInsertTypeVia(DataStruct *data, const CTMatrix& boardMatrix)
{
   if(NULL == data)
      return;

   Attrib *attrib = NULL;
   NetStruct* net = NULL;
   WORD netNameKey = OdbGlobalDoc->IsKeyWord(ATT_NETNAME, 0);
   CString netName = "";
   if (data->getAttributes() && data->getAttributes()->Lookup(netNameKey, attrib))
   {
      netName = attrib->getStringValue();
      net = m_file->getNetList().getNet(netName);
      if(NULL == net)
      {
         netName = NONE_NET;
      }
      else if(net->getFlags() & NETFLAG_UNUSEDNET)
      {
         net = NULL;
         netName = NONE_NET;
      }
   }
   else
   {
      netName = NONE_NET;
      net = NULL;
   }
   int index;
   EdaDataNet* dataNetPtr = m_edaData->AddEdaNet(net, netName,index);
   OdbPadUsageTag padUsageType = InsertTypeToPadUsage(data->getInsert()->getInsertType());
   
   // Find constituent pads of via, create feature record, add to layerFeature file
   // Update EdaDataNet record with the feature numbers of the record added to LayerFeature file.
   int blockNumber = data->getInsert()->getBlockNumber();
   CTMatrix viaMatrix = data->getInsert()->getTMatrix();
   viaMatrix.scale(boardMatrix.getScale());

   BlockStruct* viaBlock = OdbGlobalDoc->Find_Block_by_Num(blockNumber);
   if(NULL != viaBlock)
   {
      SNTViaRecord* sntViaRecord = new SNTViaRecord;
      POSITION viaBlockDataPos = viaBlock->getDataList().GetHeadPosition();
      while (viaBlockDataPos)
      {
         DataStruct *viaBlockData = viaBlock->getDataList().GetNext(viaBlockDataPos);
         if(NULL == viaBlockData)
            continue;

         switch(viaBlockData->getDataType())
         {
         case dataTypeInsert:
            {
               if(!IsUsefulPadstackData(viaBlockData))
                  continue;

               OdbFeatureFile* featureFilePtr = NULL;
               InsertStruct *viaInsert = viaBlockData->getInsert();
               if(NULL == viaInsert)
                  continue;
               CPoint2d point = viaInsert->getOrigin2d();
               double rotation = viaBlockData->getInsert()->getAngleDegrees();
               //viaMatrix.transform(rotation);
               viaMatrix.transform(point);
               double scale = viaMatrix.getScale();
               int symNum;
               int odbLayerNum = -1;
               FeatureUseType featureUseType;
               
               bool graphicMirrored = data->getInsert()->getGraphicMirrored();
               bool layerMirrored = data->getInsert()->getLayerMirrored();

               if(graphicMirrored)
                  rotation = normalizeDegrees(360.0 + rotation); 
               else
                  rotation = normalizeDegrees(360.0 - rotation); 
               
               BlockStruct* padBlock = OdbGlobalDoc->Find_Block_by_Num(viaInsert->getBlockNumber());
               if(NULL == padBlock)
                  continue;

               int layerIndex = viaBlockData->getLayerIndex();
               LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);
 
               if(padBlock->isDrillHole())
               {
                  OdbLayerStruct *odbLayer = OdbGlobalJobPtr->GetOdbLayerByType(layerTypeDrill);
                  if (odbLayer != NULL)
                  {
                     odbLayerNum = GetODBLayerNumber(odbLayer->name);
                     featureFilePtr = GetLayerFeatureFilePtr(odbLayer->name);
                  }

                  if (odbLayerNum < 0)
                     continue;

                  if(NULL == featureFilePtr)
                     continue;

                  LayerLog->LogExportedLayer(layer, false);

                  double toolSize = padBlock->getToolSize();
                  double toolSizeInMils = pageUnitsToMilsFactor * toolSize;
                  CString toolSizeString;
                  toolSizeString.Format("%s",GetFormatDecimalValue(toolSizeInMils, pageUnitsMils));
                  CString symName = "r" + toolSizeString;
                  symNum = featureFilePtr->AddSymbol(symName);
                  featureUseType = Hole;
               }                
               else
               {
                  CString odbLayerName = GetODBLayerName(layerIndex, layerMirrored);
                  if (odbLayerName.IsEmpty())
                     continue;

                  LayerLog->LogExportedLayer(layer, layerMirrored);

                  odbLayerNum = GetODBLayerNumber(odbLayerName);
                  featureFilePtr = GetLayerFeatureFilePtr(odbLayerName);
                  if(NULL == featureFilePtr)
                     continue;

                  // Use standard symbol if aperture params allow, otherwise define
                  // a custom symbol.
                  CString symName( GetOdbStandardSymbolName(padBlock) );
                  if (symName.IsEmpty())
                  {
                     symName = GetOdbSymbolName(padBlock,scale);

                     if(!OdbGlobalJobPtr->IsSymbolPresent(symName))
                     {
                        OdbSymbol* odbSymbol = new OdbSymbol(symName, padBlock, scale);
                        OdbGlobalJobPtr->AddSymbol(symName, odbSymbol);
                     }
                  }
                  symNum = featureFilePtr->AddSymbol(symName);
                  featureUseType = Copper;
               }
               FeatureRecord* featureRecordPtr = new FeatureRecord(FEATURE_PAD);
               PadRecord* recPtr = (PadRecord*) featureRecordPtr->GetFeatureTypePtr();               
               recPtr->m_centerPoint = point;
               recPtr->m_mirror = viaInsert->getGraphicMirrored();
               recPtr->m_rotation = rotation;
               recPtr->m_dcode = 0;
               recPtr->m_polarity = 'P'; 
               recPtr->m_symNum = symNum;
               recPtr->m_padUsageType = padUsageType;
               int index = featureFilePtr->AddFeatureRecord(featureRecordPtr);
               FeatureIDRecord* featureIDRecordPtr = new FeatureIDRecord;
               featureIDRecordPtr->m_featureNum = index;
               featureIDRecordPtr->m_layerNum = odbLayerNum;
               featureIDRecordPtr->m_useType = featureUseType;
               sntViaRecord->m_featureIDList.push_back(featureIDRecordPtr);
            }
            break;
         case dataTypeDraw:
            {
               //No output
            }
            break;
         case dataTypePoly:
            {
               ProcessPoly(viaBlockData,viaMatrix,data->getInsert()->getLayerMirrored());
            }
            break;
         case dataTypeText:
            {
               ProcessText(viaBlockData,viaMatrix);
            }
            break;
         case dataTypeTool:
            {
               //No output
            }
            break;
         case dataTypePoint:
            {
               //No output
            }
            break;
         case dataTypeBlob:
            {
               //No output
            }
            break;
         case dataTypeUndefined:
            {
               //No output
            }
            break;
         default:
            {
            }
         }
      }
      //Add FeatureID's to net
      if(sntViaRecord->m_featureIDList.size())
         dataNetPtr->AddSNTViaRecord(sntViaRecord);
      else
      {
         delete sntViaRecord;
         sntViaRecord = NULL;
      }
   }
}

CString OdbStep::GetOdbSizeStr(double cczPageUnitsVal)
{
   double sizeInMils = pageUnitsToMilsFactor * cczPageUnitsVal;
   CString sizeStr;
   sizeStr.Format("%s",GetFormatDecimalValue(sizeInMils, pageUnitsMils));
   return sizeStr;
}

CStringArray &OdbStep::GetOdbStandardShapeNames()
{
   // Populate on first call.
   if (m_odbShapeNames.GetCount() == 0)
   {
      m_odbShapeNames.Add("rect");   // Rectangle
      m_odbShapeNames.Add("oval");  // Oval
      m_odbShapeNames.Add("di");   // Diamond
      m_odbShapeNames.Add("oct");   // Octagon
      m_odbShapeNames.Add("donut_r"); // Round Donut
      m_odbShapeNames.Add("donut_s");  // Square Donut
      m_odbShapeNames.Add("donut_sr"); // Square/Round Donut
      m_odbShapeNames.Add("donut_rc"); // Rectangle Donut
      m_odbShapeNames.Add("donut_o"); // Oval Donut
      m_odbShapeNames.Add("hex_l");  // Horizontal Hexagon
      m_odbShapeNames.Add("hex_s"); // Vertical Hexagon
      m_odbShapeNames.Add("bfr");  // Butterfly
      m_odbShapeNames.Add("bfs");  // Square Butterfly
      m_odbShapeNames.Add("tri");   // Triangle
      m_odbShapeNames.Add("oval_h"); // Half Oval
      m_odbShapeNames.Add("thr");   // Round Thermal (Rounded)
      m_odbShapeNames.Add("ths");   // Round Thermal (Squared)
      m_odbShapeNames.Add("s_ths");  // Square Thermal
      m_odbShapeNames.Add("s_tho");  // Square Thermal (Open Corners)
      m_odbShapeNames.Add("sr_ths"); // Square-Round Thermal
      m_odbShapeNames.Add("rc_ths"); // Rectangular Thermal
      m_odbShapeNames.Add("rc_tho"); // Rectangular Thermal (Open Corners)
      m_odbShapeNames.Add("s_ths");  // Rounded Square Thermal
      m_odbShapeNames.Add("o_ths");  // Oval Thermal
      m_odbShapeNames.Add("el");    // Ellipse
      m_odbShapeNames.Add("moire"); // Moire
      m_odbShapeNames.Add("hole"); // Hole
   }

   return m_odbShapeNames;
}

bool OdbStep::IsOdbStandardSymbolName(CString name)
{
   // Returns true is name is found to be in form of standard ODB symbol name, otherwise false.
   // Is incomplete implementation, expand as we find the need.

   
   if (name.GetLength() > 1)
   {
      // Like s40 or r40.
      // If first char is r or s and remainder is digits it is a standard symbol name form
      // for Square and Round. Since these are single letter shape identifiers, need to
      // take the name apart to see if it fits.
      char firstChar = name.GetAt(0);
      if (firstChar == 's' || firstChar == 'S' || firstChar == 'r' || firstChar == 'R')
      {
         bool allDigits = true;  // Presumptuous, prove otherwise.
         for (int i = 1; i < name.GetLength() && allDigits; i++)
         {
            char c = name.GetAt(i);
            if (c < '0' || c > '9')
               allDigits = false;
         }
         // If all were digits after s or r, is a standard ODB symbol name.
         if (allDigits)
            return true;
      }
   }

   // For the rest, consider it a match if first part of the name is a
   // match, regardless of the format of the rest.

   CStringArray &odbShapeNames = this->GetOdbStandardShapeNames();

   for (int i = 0; i < odbShapeNames.GetCount(); i++)
   {
      CString odbShapeName( odbShapeNames.GetAt(i) );

      // If names matches starting at first char...
      if (name.Find(odbShapeName) == 0)
      {
         // If shape name is all there is in name, no other chars, we'll call it a match.
         if (odbShapeName.GetLength() == name.GetLength())
            return true;

         // If char after name match length is a digit then name matches up to
         // first parameter, we'll call this a match.
         int indx = odbShapeName.GetLength();
         char c = name.GetAt(indx);
         if (c >= '0' && c <= '9') // Is digit
            return true;
      }
   }

   // Either it is not a standard ODB symbol name or it is one that we
   // don't yet detect.
   return false;
}

CString OdbStep::GetOdbStandardSymbolName(BlockStruct *apertureBlock)
{
   // Format the ODB style aperture (symbol) name if we can.
   // If not, return blank name.
   // Implemented for selected shapes, and must have zero offset and rotation.

   CString symName;

   if (apertureBlock != NULL && apertureBlock->isSimpleAperture())
   {
      CPoint2d offset = apertureBlock->getOffset();
      if (offset.x == 0. && offset.y == 0. && apertureBlock->getRotation() == 0.)
      {
         ApertureShapeTag apShape = apertureBlock->getShape();

         double sizeA = apertureBlock->getSizeA();
         double sizeB = apertureBlock->getSizeB();

         switch (apShape)
         {
         case apertureRound:
            symName = "r" + GetOdbSizeStr(sizeA);
            break;

         case apertureSquare:
            symName = "s" + GetOdbSizeStr(sizeA);
            break;

         case apertureRectangle:
            symName = "rect" + GetOdbSizeStr(sizeA) + "x" + GetOdbSizeStr(sizeB);
            break;

         case apertureOblong:
            symName = "oval" + GetOdbSizeStr(sizeA) + "x" + GetOdbSizeStr(sizeB);
            break;

         case apertureDonut:
            {
               double sizeMin = min(sizeA, sizeB);
               double sizeMax = max(sizeA, sizeB);
               symName = "donut_r" + GetOdbSizeStr(sizeMax) + "x" + GetOdbSizeStr(sizeMin);
            }
            break;

         case apertureOctagon:
            {
               // The halfSide calc is from BlockStruct::convertApertureToPoly().
               double halfSide = sizeA / 4.83; // size of half of one side of octagon
               // Now convert to ODB style.
               double fullSide = halfSide * 2.;
               double whatsLeft = sizeA - fullSide;
               double cornerSize = whatsLeft / 2.; // Two of these per side, so half of whatsLeft is ODB corner size
               symName = "oct" + GetOdbSizeStr(sizeA) + "x" + GetOdbSizeStr(sizeA) + "x" + GetOdbSizeStr(cornerSize);
            }
            break;
         }
      }
   }

   return symName;
}

ODBComponent *OdbStep::GetOdbComponent(CString refName)
{
   // Look on top, if not there look on bottom, if not there... its not there.
   ComponentLayerFeatureFile* topFeatureFile = GetComponentTopLayerFeatureFile();
   ODBComponent *odbComp = topFeatureFile->FindComponent(refName);
   if (odbComp == NULL)
   {
      ComponentLayerFeatureFile* botFeatureFile = GetComponentBottomLayerFeatureFile();
      odbComp = botFeatureFile->FindComponent(refName);
   }
   return odbComp;
}

void OdbStep::ResolveOdbNet(CompPinStruct *comppin, CString &odbNetName, NetStruct **net)
{
   if (comppin != NULL)
   {
      if(NULL == (*net))
      {
         odbNetName = NONE_NET;
      }
      else if ((*net)->getFlags() & NETFLAG_UNUSEDNET)
      {
         *net = NULL;
         odbNetName = NONE_NET;
      }
      else
         odbNetName = (*net)->getNetName();
   }
   else
   {
      *net = NULL;
      odbNetName = NONE_NET;
   }
}

void OdbStep::ProcessPinPadstack(DataStruct *componentData, InsertStruct *pinInsert, BlockStruct *pinBlock, CTMatrix pinMatrix, const CTMatrix& boardMatrix, SNTToePrintRecord* sntToePrintRecord,
                                 double pinInsertAngleDegrees, double componentInsertAngleDegrees, double compScaleFactor, EdaDataNet* edaDataNetPtr)
{
   if (pinInsert == NULL || pinBlock == NULL)
      return;

   OdbPadUsageTag padUsageType = InsertTypeToPadUsage(pinInsert->getInsertType());
   InsertStruct *componentInsert = componentData->getInsert();

   POSITION pinBlockDataPos = pinBlock->getDataList().GetHeadPosition();
   while (pinBlockDataPos)
   {
      DataStruct *pinBlockData = pinBlock->getDataList().GetNext(pinBlockDataPos);
      if(NULL == pinBlockData)
         continue;

      switch(pinBlockData->getDataType())
      {
      case dataTypeInsert:
         {
            if (!IsUsefulPadstackData(pinBlockData))
               continue;

            OdbFeatureFile* featureFilePtr = NULL;
            InsertStruct *padInsert = pinBlockData->getInsert();
            if(NULL == padInsert)
               continue;
            CPoint2d point = padInsert->getOrigin2d();
            double padInsertAngleDegrees = pinBlockData->getInsert()->getAngleDegrees();

            bool graphicMirrored = false;
            if(pinInsert->getGraphicMirrored() && componentData->getInsert()->getGraphicMirrored())
               graphicMirrored = false;
            else if(pinInsert->getGraphicMirrored() || componentData->getInsert()->getGraphicMirrored())
               graphicMirrored = true;

            // Not entirely sure if layer mirror should get the same treatment as graphic mirror. We'll try it and see.
            bool layerMirrored = false;
            if(pinInsert->getLayerMirrored() && componentData->getInsert()->getLayerMirrored())
               layerMirrored = false;
            else if(pinInsert->getLayerMirrored() || componentData->getInsert()->getLayerMirrored())
               layerMirrored = true;

            double padRotRelToPCB = padInsertAngleDegrees + pinInsertAngleDegrees + componentInsertAngleDegrees;
            pinMatrix.transform(point);
            int symNum, odbLayerNum;
            FeatureUseType featureUseType;
            //In ODB++ this rotation is clockwise
            if(graphicMirrored)
               padRotRelToPCB = normalizeDegrees(360.0 + padRotRelToPCB);
            else
               padRotRelToPCB = normalizeDegrees(360.0 - padRotRelToPCB);

            BlockStruct* padBlock = OdbGlobalDoc->Find_Block_by_Num(padInsert->getBlockNumber());
            if(NULL == padBlock)
               continue;

            if(padBlock->isDrillHole())
            {
               OdbLayerStruct *odbLayer = OdbGlobalJobPtr->GetOdbLayerByType(layerTypeDrill);
               if (odbLayer != NULL)
               {
                  odbLayerNum = GetODBLayerNumber(odbLayer->name);
                  featureFilePtr = GetLayerFeatureFilePtr(odbLayer->name);
               }

               if (odbLayerNum < 0)
                  continue;

               if(NULL == featureFilePtr)
                  continue;

               double toolSize = padBlock->getToolSize();
               double toolSizeInMils = pageUnitsToMilsFactor * toolSize;
               CString toolSizeString;
               toolSizeString.Format("%s",GetFormatDecimalValue(toolSizeInMils, pageUnitsMils));
               CString symName = "r" + toolSizeString;
               symNum = featureFilePtr->AddSymbol(symName);
               featureUseType = Hole;
               padUsageType = padUsageToolingHole;
            }                
            else
            {
               int layerIndex = pinBlockData->getLayerIndex();
               LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

               CString odbLayerName = GetODBLayerName(layerIndex, layerMirrored);
               if (odbLayerName.IsEmpty())
                  continue;

               LayerLog->LogExportedLayer(layer, layerMirrored);

               odbLayerNum = GetODBLayerNumber(odbLayerName);
               featureFilePtr = GetLayerFeatureFilePtr(odbLayerName);
               if(NULL == featureFilePtr)
                  continue;

               // Use standard symbol if aperture params allow, otherwise define
               // a custom symbol.
               CString symName( GetOdbStandardSymbolName(padBlock) );
               if (symName.IsEmpty())
               {
                  symName = GetOdbSymbolName(padBlock,compScaleFactor);

                  if(!OdbGlobalJobPtr->IsSymbolPresent(symName))
                  {
                     OdbSymbol* odbSymbol = new OdbSymbol(symName, padBlock, compScaleFactor);
                     OdbGlobalJobPtr->AddSymbol(symName, odbSymbol);
                  }
               }

               symNum = featureFilePtr->AddSymbol(symName);
               featureUseType = Copper;
            }

            FeatureRecord* featureRecordPtr = new FeatureRecord(FEATURE_PAD);
            PadRecord* recPtr = (PadRecord*) featureRecordPtr->GetFeatureTypePtr();                           
            recPtr->m_centerPoint = point;
            recPtr->m_mirror = graphicMirrored;
            recPtr->m_rotation = padRotRelToPCB;
            recPtr->m_dcode = 0;
            recPtr->m_polarity = 'P'; 
            recPtr->m_symNum = symNum;
            recPtr->m_padUsageType = padUsageType;
            int index = featureFilePtr->AddFeatureRecord(featureRecordPtr);

            // The sntToePrintRecord is for pins of actual components only. If a geometry of type
            // blockTypePcbComponent was inserted as something that is not an actual component then
            // we do not want this, and sntToePrintRecord will be NULL, so just skip it.
            if (sntToePrintRecord != NULL)
            {
               FeatureIDRecord* featureIDRecordPtr = new FeatureIDRecord;
               featureIDRecordPtr->m_featureNum = index;
               featureIDRecordPtr->m_layerNum = odbLayerNum;
               featureIDRecordPtr->m_useType = featureUseType;
               sntToePrintRecord->m_featureIDList.push_back(featureIDRecordPtr);
            }
         }
         break;
      case dataTypeDraw:
         {
            //No output
         }
         break;
      case dataTypePoly:
         {
            ProcessPoly(pinBlockData, boardMatrix, (pinInsert->getLayerMirrored() ^ componentInsert->getLayerMirrored()) );
         }
         break;
      case dataTypeText:
         {
            ProcessText(pinBlockData, boardMatrix);
         }
         break;
      case dataTypeTool:
         {
            //No output
         }
         break;
      case dataTypePoint:
         {
            //No output
         }
         break;
      case dataTypeBlob:
         {
            //No output
         }
         break;
      case dataTypeUndefined:
         {
            //No output
         }
         break;
      default:
         {
         }
      }
   }

   //Add FeatureID's to net
   if (sntToePrintRecord != NULL)
   {
      if(sntToePrintRecord->m_featureIDList.size() && edaDataNetPtr != NULL)
         edaDataNetPtr->AddSNTToePrintRecord(sntToePrintRecord);
      else
      {
         delete sntToePrintRecord;
         sntToePrintRecord = NULL;
      }
   }
}

void OdbStep::ProcessInsertTypePcbComponent(DataStruct *componentData, const CTMatrix& boardMatrix)
{
   // This is for inserts that represent actual PCB Components.
   // It adds stuff to EDA and BOM and net comppins and stuff.

   if (NULL == componentData)
      return;

   InsertStruct *componentInsert = componentData->getInsert();
   if (NULL == componentInsert)
      return;

   CString refName = componentInsert->getRefname();
   if(refName.IsEmpty())
   {
      refName.Format("$Ent_%d", componentData->getEntityNumber());
   }

   // A comp with this refname should not have been already processed. A dup here means
   // multiple instances of same comp refdes insert, i.e. two or more C2260 comps in design.
   ODBComponent *existingComp = this->GetOdbComponent(refName);
   if (existingComp != NULL )
   {
      // Error - Duplicate insert instances for given refname.
      fprintf(flog, "Duplicate instances of component [%s] found. Exporting entity [%d], skipping entity [%d]\n",
         refName, existingComp->GetCompDataEntityNumber(), componentData->getEntityNumber());

      return;
   }

   BlockStruct* insertedComponentBlock = OdbGlobalDoc->Find_Block_by_Num(componentInsert->getBlockNumber());
   if (NULL == insertedComponentBlock)
      return;

   double boardScale = boardMatrix.getScale();
   CTMatrix componentMatrix = componentInsert->getTMatrix();
   componentMatrix.scale(boardScale);

   ODBComponent* comp = ProcessBlockTypePcbComponent(true, refName, componentData, componentMatrix, boardMatrix, boardScale);

   if (comp != NULL)
   {
      // Make BOM entry. This has to be after comp is fully constructed, this will
      // access comp fields.
      BomPartEntry *bomPart = m_bomData->AddBomPartEntry(comp, this->GetStepFileStruct());
   }
}

void OdbStep::ProcessRandomInsertTypeAbusingPcbComponentGeometry(DataStruct *componentData, const CTMatrix& boardMatrix)
{
   // This is for things that are inserting a blockTypePcbComponent, but are not actually components.
   // We don't want these in EDA or BOM data, nor adding comppins to nets, or anything like that.
   // We only want the graphics out of these.
   // Usual component rules like uniqueness of refname does not matter for these.

   if (NULL == componentData)
      return;

   InsertStruct *componentInsert = componentData->getInsert();
   if (NULL == componentInsert)
      return;

   CString refName = componentInsert->getRefname();
   if(refName.IsEmpty())
   {
      refName.Format("$Ent_%d", componentData->getEntityNumber());
   }

   BlockStruct* insertedComponentBlock = OdbGlobalDoc->Find_Block_by_Num(componentInsert->getBlockNumber());
   if (NULL == insertedComponentBlock)
      return;

   double boardScale = boardMatrix.getScale();
   CTMatrix componentMatrix = componentInsert->getTMatrix();
   componentMatrix.scale(boardScale);

   ODBComponent* comp = ProcessBlockTypePcbComponent(false, refName, componentData, componentMatrix, boardMatrix, boardScale);

}

ODBComponent* OdbStep::ProcessBlockTypePcbComponent(bool isActualComponent, CString refName, DataStruct *componentData, const CTMatrix& componentMatrix, const CTMatrix& boardMatrix, double boardScale)
{
   InsertStruct *componentInsert = componentData != NULL ? componentData->getInsert() : NULL;
   BlockStruct* insertedComponentBlock = componentInsert != NULL ? OdbGlobalDoc->Find_Block_by_Num(componentInsert->getBlockNumber()) : NULL;

   if (NULL != insertedComponentBlock)
   {
      // Select top or bottom feature file.
      ComponentLayerFeatureFile* featureFile;
      if(componentInsert->getPlacedTop())
         featureFile = GetComponentTopLayerFeatureFile();
      else
         featureFile = GetComponentBottomLayerFeatureFile();

      // The usual, conventional geometries are "top built" and inserted "plain" on top and
      // inserted LayerMirrored and GraphicMirrored on bottom. If the component is placed bottom
      // and is LayerMirrored but not GraphicMirrored, it is a sort of BottomBuilt style.
      // A more genuine BottomBuilt would not MirrorLayers either, it would use bottom layers directly, that
      // would be a "pure" BottomBuilt. So I guess what we have here is more pre-mirrored than bottom-built.
      // Anyway, we need to detect this so we can GraphicMirror the geom for EDA/DATA.
      bool isBottomBuiltGeom = (componentInsert->getPlacedBottom() && !componentInsert->getGraphicMirrored());

      int packageNum = -1;
      int compNumber = -1;
      ODBComponent* comp = NULL;
      if (isActualComponent)
      {
         EdaDataPkg *edaPkg = m_edaData->AddEdaPkg(insertedComponentBlock, isBottomBuiltGeom, componentMatrix.getScale(), packageNum);
         comp = featureFile->AddODBComponent(componentData, refName, packageNum, boardScale, compNumber);
         if (comp == NULL)
            return NULL;
      }

      // Proceed with building the component for CMP output.
      POSITION blockDataPos = insertedComponentBlock->getDataList().GetHeadPosition();
      while (blockDataPos)
      {
         DataStruct *blockData = insertedComponentBlock->getDataList().GetNext(blockDataPos);
         if(NULL == blockData)
            continue;

         switch(blockData->getDataType())
         {
         case dataTypeInsert:
            if ((NULL != blockData->getInsert()) && (blockData->getInsert()->getInsertType() == insertTypePin))
            {
               //Pin found
               InsertStruct *pinInsert = blockData->getInsert();
               CString pinName = pinInsert->getRefname();

               NetStruct *net = NULL;
               CompPinStruct *comppin = FindCompPin(m_file, refName, pinName, &net);

               CString netName;
               ResolveOdbNet(comppin, netName, &net);

               CPoint2d c = pinInsert->getOrigin2d();
               componentMatrix.transform(c);

               double pinInsertAngleDegrees = pinInsert->getAngleDegrees();
               double componentInsertAngleDegrees = componentInsert->getAngleDegrees();

               double pinRotRelToPcb; // rotation relative to PCB
               // DR 844415 Bottom side parts are not always mirrored so this does not always work: if(componentData->getInsert()->getGraphicMirrored())
               // If it is placed problem it probably is mirrored, but not necessarily. If it is
               // mirrored it is not necessarily placed bottom. Need to check for each.
               if (componentData->getInsert()->getPlacedBottom() || componentData->getInsert()->getGraphicMirrored()) 
                  pinRotRelToPcb = normalizeDegrees(360.0 + componentInsertAngleDegrees + pinInsertAngleDegrees); //Comp Rotation as per ODB++
               else
                  pinRotRelToPcb = normalizeDegrees(360.0 - componentInsertAngleDegrees - pinInsertAngleDegrees); //Comp Rotation as per ODB++ 

               int netIndex = -1;
               int pinNumber = -1;
               EdaDataNet* edaDataNetPtr = NULL;
               if (isActualComponent)
               {
                  edaDataNetPtr = m_edaData->AddEdaNet(net, netName, netIndex);
                  ODBCompPin* pin = new ODBCompPin(pinName, c.x, c.y, pinRotRelToPcb, pinInsert->getGraphicMirrored() ? "M" : "N", netIndex, 0 /*subNetNumber*/);
                  pinNumber = comp->AddCompPin(pin);
               }

               // Find constituent pads of pin, create feature record, add to layerFeature file
               // Update EdaDataNet record with the feature numbers of the record added to LayerFeature file.
               double compScaleFactor = componentMatrix.getScale();
               int pinBlockNumber = pinInsert->getBlockNumber();
               CBasesVector pinBasesVector = pinInsert->getBasesVector();

               pinBasesVector.transform(componentMatrix);
               CTMatrix pinMatrix = pinBasesVector.getTransformationMatrix();             

               BlockStruct* pinBlock = OdbGlobalDoc->Find_Block_by_Num(pinBlockNumber);
               if(NULL != pinBlock)
               {
                  SNTToePrintRecord* sntToePrintRecord = NULL;
                  if (isActualComponent)
                  {
                     sntToePrintRecord = new SNTToePrintRecord;
                     sntToePrintRecord->m_compNum = compNumber;
                     sntToePrintRecord->m_pinNum = pinNumber;
                     sntToePrintRecord->m_isTop = componentData->getInsert()->getPlacedTop();
                  }

                  ProcessPinPadstack(componentData, pinInsert, pinBlock, pinMatrix, boardMatrix,
                     sntToePrintRecord,
                     pinInsertAngleDegrees, componentInsertAngleDegrees, compScaleFactor, edaDataNetPtr);
               }
            }
            else
            {
               // Other inserts besides pin. Extract only Silkscreen.
               InsertTypeTag insertType = blockData->getInsert()->getInsertType();

               CBasesVector pinBasesVector = blockData->getInsert()->getBasesVector();
               pinBasesVector.transform(componentMatrix);
               CTMatrix pinMatrix = pinBasesVector.getTransformationMatrix(); 

               ProcessOtherInsertTypes(blockData, pinMatrix, componentInsert->getLayerMirrored(), isActualComponent/*keepSilkscreenOnly*/);
            }
            break;
         case dataTypeDraw:
            {
               //No output
            }
            break;
         case dataTypePoly:
            {
               ProcessPoly(blockData,componentMatrix,componentInsert->getLayerMirrored());
            }
            break;
         case dataTypeText:
            {
               ProcessText(blockData,componentMatrix);
            }
            break;
         case dataTypeTool:
            {
               //No output
            }
            break;
         case dataTypePoint:
            {
               //No output
            }
            break;
         case dataTypeBlob:
            {
               //No output
            }
            break;
         case dataTypeUndefined:
            {
               //No output
            }
            break;
         default:
            {
            }
         }
      }

      return comp;
   }
   return NULL;
}

bool OdbStep::ProcessVariants()
{
   // Returns true if processing variants made one or more variant BOMs.
   // One BOM will have already been built for "current variant" if there was one.
   // We don't need two of them, so here skip variant with same name.
   
   CVariantList &variantList = this->m_file->getVariantList();

   POSITION variantPos = variantList.GetHeadPosition();
   while (variantPos != NULL)
   {
      CVariant *variant = variantList.GetNext(variantPos);
      // If this variant is not already the Affecting BOM variant...
      if (BomData::GetValidOdbBomName(variant->GetName()).CompareNoCase(this->m_bomData->GetBomName()) != 0)
         ProcessVariant(variant);
   }

   
   // The "~Default Variant~" is not in that list with regular variants, it is a special
   // variant in the variantList. Export it too, but only if there were regular variants
   // in this variantList.
   if (m_bomVariantList.GetCount() > 0) // Got some from loop above
   {
      CVariant *defaultVariant = variantList.GetDefaultVariant();
      ProcessVariant(defaultVariant);
   }

   return m_bomVariantList.GetCount() > 0;
}

bool OdbStep::ProcessVariant(CVariant *variant)
{
   // Returns true if variant processing produced a BOM, otherwise false.

   if (variant != NULL && variant->GetCount() > 0)
   {
      CString variantName(variant->GetName());
      BomData *variantBom = new BomData(variantName);
      m_bomVariantList.AddTail(variantBom);

      POSITION insertDataPos = m_file->getBlock()->getHeadDataInsertPosition();
      while (insertDataPos != NULL)
      {
         DataStruct *insertData = m_file->getBlock()->getNextDataInsert(insertDataPos);

         if (IsOdbComponentType(insertData))
         {
            InsertStruct *insert = insertData->getInsert();
            CVariantItem *variantItem = variant->FindItem(insert->getRefname());
            if (variantItem == NULL)
            {
               // Seems like this ought to be an error, all refnames should be in variant.
               fprintf(flog, "Variant [%s] does not contain component [%s].", variantName, insert->getRefname());
            }
            else
            {
               ODBComponent *odbComp = this->GetOdbComponent(insert->getRefname());
               if (odbComp != NULL)
               {
                  variantBom->AddBomPartEntry(odbComp, this->m_file, variantItem);
               }
            }
         }
      }

      return true;
   }

   return false;
}

void OdbStep::ProcessText(DataStruct *data, const CTMatrix& transMatrix)
{
   TextStruct* text = data->getText();
   if(NULL == text)
      return;
   
   // If text has zero height or zero width then vPlan will not like it.
   // It is not really visible anyway, so just skip it.
   if (text->getHeight() <= 0. || text->getWidth() <= 0.)
      return;

   CPoint2d point = text->getOrigin();
   double rotation = text->getRotationDegrees();
   transMatrix.transform(rotation);
   transMatrix.transform(point);
   double InsertRotation = transMatrix.getBasesVector().getRotationDegrees();
   if(transMatrix.getBasesVector().getMirror())
      rotation = normalizeDegrees(360.0 + rotation); 
   else
      rotation = normalizeDegrees(360.0 - rotation);

   FeatureRecord* feature = new FeatureRecord(FEATURE_TEXT);
   if(NULL == feature)
      return;

   // Text bug here. Not handling multi-line text notes, i.e. those with newline chars embedded.
   // Need to break them up into separate text features per line, from spec it looks like no way
   // to use escape chars or such to make a single text multi-line. Left as it was before
   // the hack here, we made incorrect ODB++ syntax, the newlines were treated literally.

   TextRecord* recPtr = (TextRecord*) feature->GetFeatureTypePtr();
   recPtr->m_polarity = data->isNegative() ? 'N' : 'P';
   recPtr->m_rotation = rotation;
   recPtr->m_text = text->getText();
   recPtr->m_textLocation = point;

   // Hack to make legal ODB++ file. This is going to drop the 2nd to nth line
   // of text, only keep the first line. This section needs to be a loop that breaks up
   // the multilines to individual line text features. I.e. create a FEATURE_TEXT for
   // each individual line.
   int newlineIndx = recPtr->m_text.Find("\n");
   if (newlineIndx > -1)
      recPtr->m_text.Truncate(newlineIndx);

   // Text width in CCZ is for single char cell, does not include inter-char spacing.
   // Text width in ODB includes inter-char space. This was determined empirically, not
   // actually found in the spec. So, to compensate, extend width by inter-char
   // spacing factor. Applies only to width, aka xsize.
   ///// This would have been preferred, but the settings are never initialized, I guess Kurt never got
   ///// around to it in DCA conversion times, and no one else has stepped up, including me, today.
   /////double textSpaceRatio = this->m_file->getCamCadData().getCamCadDataSettings().getTextSpacingRatio();
   double textSpaceRatio = (OdbGlobalDoc != NULL) ? OdbGlobalDoc->getSettings().getTextSpaceRatio() : 0.0;
   double totalCharWidthFactor = 1.0 + textSpaceRatio;
   recPtr->m_xsize = text->getWidth() * transMatrix.getScale() * totalCharWidthFactor;
   recPtr->m_ysize = text->getHeight() * transMatrix.getScale();
   recPtr->m_penWidth = OdbGlobalDoc->getWidth(text->getPenWidthIndex());
   recPtr->m_mirror = false; 
   // DR 822367
   // TextStruct carries only GraphicMirror (text->isMirrored()), so it can flip the text
   // but it can't change the layer. Now, LayerMirror on the parent owning this text could
   // flip it, but as of now, such is not even being considered. The transMatrix mirror
   // should only be able to mirror graphic, since transformation matrices carry no info
   // about layer. 
   // CONCLUSION: the mirror flag calc'd here should not be affecting layer name choice.
   if(text->isMirrored() && transMatrix.getMirror())
      recPtr->m_mirror = false; 
   else if(text->isMirrored() || transMatrix.getMirror())
      recPtr->m_mirror = true;

   int layerIndex = data->getLayerIndex();
   LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

   CString odbLayerName = GetODBLayerName(layerIndex, false); // DR 822367, considering graphic mirror here is a bug, needs layer mirror, was:  , recPtr->m_mirror);
   if (odbLayerName.IsEmpty())
   {
      delete feature;
      return;
   }

   LayerLog->LogExportedLayer(layer, false);

   OdbFeatureFile* featureFilePtr = GetLayerFeatureFilePtr(odbLayerName);
   if(NULL != featureFilePtr)
      featureFilePtr->AddFeatureRecord(feature);
   else
      delete feature;
}

void OdbStep::ProcessPoly(DataStruct *data, const CTMatrix& transMatrix, bool parentLayerMirrored)
{
   CPolyList* srcPolyList = data->getPolyList();
   if(NULL == srcPolyList)
      return;

   GraphicClassTag graphicClass = data->getGraphicClass();

   int layerIndex = data->getLayerIndex();   
   LayerStruct* layer = OdbGlobalDoc->getLayer(layerIndex);

   CString odbLayerName = GetODBLayerName(layerIndex, parentLayerMirrored);
   if (odbLayerName.IsEmpty())
      return;

   LayerLog->LogExportedLayer(layer, parentLayerMirrored);

   // No need to process Board Outline or Panel Outline.
   // Oops... unless they are on a silkscreen layer. Outlines with these graphic classes
   // become part of the (item (panel, board, or component) definition, but we have no control of layer there.
   // If these are on silkscreen layer the user still wants to see them on silkscreen layer.
   // CCZ can do both with the same poly, ODB++ can not. So go ahead and output the
   // poly if it is on silkscreen layer, even though we already output it as object outline elsewhere.
   if( (graphicClassBoardOutline == graphicClass)
     || (graphicClassComponentOutline == graphicClass)
      || (graphicClassPanelOutline == graphicClass))
   {
      if (layer != NULL)
      {
         LayerTypeTag layerType = layer->getLayerType();
         if (layerType != layerTypeSilkTop && layerType != layerTypeSilkBottom)
            return;
         // else keep going, go ahead and output the poly on sst/ssb as appropriate.
      }
   }

   bool lyrNeverMir = layer->getNeverMirror();
   bool lyrMirOnly  = layer->getMirrorOnly();

   if (lyrNeverMir || lyrMirOnly)
   {
      LayerStruct &mirlyr = layer->getMirroredLayer();
      
      // BasesVector has graphic mirror only. (Can't tell anything about layer mirroring from BasesVector.)
      bool objectGraphicMirrored = transMatrix.getBasesVector().getMirror();

      // If object is mirrored and on a Never Mirror layer, then object
      // is not visible, skip it.
      // If object is not mirrored and is on an Mirror Only layer, then
      // (you guessed it) object is not visible, skip it.
      if (objectGraphicMirrored)
      {
         if (lyrNeverMir)
         {     
            return;  // Mirrored object on Never Mirror layer, skip it.
         }
      }
      else
      {
         if (lyrMirOnly)
         {
            return;  // Non-mirrored object on Mirror Only layer, skip it.
         }
      }
      
   }

   bool isNet = false;
   EdaDataNet* dataNetPtr = NULL;


   if(data->getGraphicClass() == graphicClassEtch)
   {
      isNet = true;
      Attrib *attrib = NULL;
      NetStruct* net = NULL;
      WORD netNameKey = OdbGlobalDoc->IsKeyWord(ATT_NETNAME, 0);
      CString netName = "";
      if (data->getAttributes() && data->getAttributes()->Lookup(netNameKey, attrib))
      {
         netName = attrib->getStringValue();
         net = m_file->getNetList().getNet(netName);
         if(NULL == net)
         {
            netName = NONE_NET;
         }
         else if(net->getFlags() & NETFLAG_UNUSEDNET)
         {
            net = NULL;
            netName = NONE_NET;
         }
      }
      else
      {
         netName = NONE_NET;
         net = NULL;
      }
      int index;
      dataNetPtr = m_edaData->AddEdaNet(net, netName, index);
      vector<CAMCADAttribute*> camcadAttributeList = GetCAMCADAttributeList(data->getAttributesRef());
      for(unsigned int camcadAttributeNum = 0; camcadAttributeNum < camcadAttributeList.size() ; camcadAttributeNum++)
      {
         CAMCADAttribute* camcadAttribute = camcadAttributeList[camcadAttributeNum];
         if(NULL == camcadAttribute)
            continue;
         ODBAttribute* odbAttribute = new ODBAttribute;
         int attributeNum = -1;
         int attributeStringNum = -1;
         attributeNum = m_edaData->AddNetAttributeName(camcadAttribute->m_attributeName);

         CString validatedValue(  camcadAttribute->m_value );
         odbAttribute->m_isString = false;
         if(camcadAttribute->m_isString == true)
         {
            odbAttribute->m_isString = true;
            
            if (camcadAttribute->m_attributeName.CompareNoCase("NETNAME") == 0)
               validatedValue = dataNetPtr->GetOdbNetName();

            attributeStringNum = m_edaData->AddNetAttributeStringValue(validatedValue);
         }

         odbAttribute->m_attributeNum = attributeNum;
         odbAttribute->m_attributeStringNum = attributeStringNum;
         odbAttribute->m_value = validatedValue;

         dataNetPtr->AddODBAttribute(odbAttribute);
      }
   }


   ProcessPoly(srcPolyList,transMatrix, odbLayerName, dataNetPtr, data->isNegative());    
}

void OdbStep::ProcessPoly(CPolyList* srcPolyList,const CTMatrix &transMatrix, CString odbLayerName, EdaDataNet* dataNetPtr, bool isNegative)
{
   if(NULL == srcPolyList)
      return;

   OdbFeatureFile* featureFilePtr = GetLayerFeatureFilePtr(odbLayerName);
   int odbLayerNum = GetODBLayerNumber(odbLayerName);
   if(NULL != featureFilePtr)
   {
      CPolyList* polyList = CopyPoly(srcPolyList);
      CBasesVector basesVector = transMatrix.getBasesVector();   
      int mirror = basesVector.getMirror() ? 1 : 0;
      double rotationRadians;
      if(mirror)
         rotationRadians = - basesVector.getRotationRadians();
      else
         rotationRadians = basesVector.getRotationRadians();

      TransformPolyList(polyList, basesVector.getX(), basesVector.getY(), rotationRadians, mirror, basesVector.getScale());   
      char polarity = isNegative ? 'N' : 'P';
      POSITION pos = polyList->GetHeadPosition();
      CPolyList* surfaceRecordPolyList = NULL;
      while (pos)
      {
         CPoly* poly = polyList->GetNext(pos);
         if (poly)
         {
            if(poly->isClosed() && poly->isFilled())
            {
               // Accumulate all closed/filled poly into one polyList.
               if(NULL == surfaceRecordPolyList)
                  surfaceRecordPolyList = new CPolyList;
               surfaceRecordPolyList->AddTail(poly);
            }
            else
            {
               CPnt *p1, *p2;
               int symNum = 0;     // Default is zero width
               double width = 0.0;

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               // Init poly, get width
               if (pntPos != NULL)
               {
                  p2 = poly->getPntList().GetNext(pntPos);
                  int widthIndx = poly->getWidthIndex();
                  if(widthIndx > -1 && widthIndx < OdbGlobalDoc->getWidthTable().GetSize())
                  {
                     BlockStruct *widthBlk = OdbGlobalDoc->getWidthBlock(widthIndx);
                     if (widthBlk != NULL)
                        width = widthBlk->getSizeA();
                     if(width > SMALLNUMBER)
                     {
                        double widthInMils = pageUnitsToMilsFactor * width;
                        CString widthString;
                        widthString.Format("%s",GetFormatDecimalValue(widthInMils, pageUnitsMils));
                        CString symName = "r" + widthString;
                        symNum = featureFilePtr->AddSymbol(symName);
                     }
                  }
               }
               // Process poly pnts
               while (NULL != pntPos)
               {
                  p1 = p2;
                  p2 = poly->getPntList().GetNext(pntPos);

                  if (fabs(p1->bulge) > SMALLNUMBER)
                  {                     
                     //ARC add a Arc record
                     double sa, da, cx, cy, r;
                     da = atan(p1->bulge) * 4;
                     ArcPoint2Angle(p1->x, p1->y, p2->x, p2->y, da, &cx, &cy, &r, &sa);
                     FeatureRecord* feature = new FeatureRecord(FEATURE_ARC);
                     ArcRecord* recPtr = (ArcRecord*) feature->GetFeatureTypePtr();
                     recPtr->m_startPoint = CPoint2d(p1->x, p1->y);
                     recPtr->m_endPoint = CPoint2d(p2->x, p2->y);
                     recPtr->m_centerPoint = CPoint2d(cx, cy);
                     char clockWise = ( p1->bulge < 0.0 ) ? 'Y' : 'N';
                     recPtr->m_clockWise = clockWise;
                     recPtr->m_dcode = 0; //TODO may be
                     recPtr->m_polarity = polarity;
                     recPtr->m_symNum = symNum;
                     int index = featureFilePtr->AddFeatureRecord(feature);
                     if(NULL != dataNetPtr)
                     {
                        FeatureIDRecord* feature = new FeatureIDRecord;
                        feature->m_featureNum = index;
                        feature->m_layerNum = odbLayerNum;
                        feature->m_useType = Copper;
                        dataNetPtr->AddSNTTraceRecord(feature);
                     }
                  }
                  else
                  {
                     //Line record
                     FeatureRecord* feature = new FeatureRecord(FEATURE_LINE);
                     LineRecord* recPtr = (LineRecord*) feature->GetFeatureTypePtr();
                     recPtr->m_startPoint = CPoint2d(p1->x,p1->y);
                     recPtr->m_endPoint = CPoint2d(p2->x, p2->y);
                     recPtr->m_dcode = 0; //TODO may be
                     recPtr->m_polarity = polarity;
                     recPtr->m_symNum = symNum;
                     int index = featureFilePtr->AddFeatureRecord(feature);
                     if(NULL != dataNetPtr)
                     {
                        FeatureIDRecord* feature = new FeatureIDRecord;
                        feature->m_featureNum = index;
                        feature->m_layerNum = odbLayerNum;
                        feature->m_useType = Copper;
                        dataNetPtr->AddSNTTraceRecord(feature);
                     }
                  }
               }
            }
         }
      }

      // If we collected a surfaceRecordPolylist then write it.
      // A useful surface is a closed poly, need at least three points. If less then skip it.
      if (NULL != surfaceRecordPolyList)
      {
         if (!surfaceRecordPolyList->hasPoints(3))
         {
            delete surfaceRecordPolyList;
         }
         else
         {
            FeatureRecord* feature = new FeatureRecord(FEATURE_SURFACE);
            SurfaceRecord* recPtr = (SurfaceRecord*) feature->GetFeatureTypePtr();
            recPtr->m_polyList = surfaceRecordPolyList;
            recPtr->m_polarity = polarity;
            recPtr->m_dcode = 0; //TODO may be               
            int index = featureFilePtr->AddFeatureRecord(feature);
            if(NULL != dataNetPtr)
            {
               FeatureIDRecord* feature = new FeatureIDRecord;
               feature->m_featureNum = index;
               feature->m_layerNum = odbLayerNum;
               feature->m_useType = Copper;
               dataNetPtr->AddSNTTraceRecord(feature);
            }
         }
         surfaceRecordPolyList = NULL;
      }
   }
}

void OdbStep::InitializeStepLayerFeatureFileList()
{
   OdbLayerList& odbLayerList = OdbGlobalJobPtr->GetOdbLayerList();
   for(OdbLayerList::iterator it = odbLayerList.begin(); it != odbLayerList.end(); it++)
   {
      OdbLayerStruct *odbLayer = *it;
      CString layerName = odbLayer->name;
      OdbFeatureFile* layerFeatureFilePtr = new OdbFeatureFile;
      //Add zero width symbol
      layerFeatureFilePtr->AddSymbol("r0.005");
      m_layerFeatureFileList.insert(std::make_pair<const CString,OdbFeatureFile* > (layerName,layerFeatureFilePtr));
   }
}


OdbFeatureFile* OdbStep::GetLayerFeatureFilePtr(CString odbLayerName)
{
   map<CString,OdbFeatureFile* >::iterator it = m_layerFeatureFileList.find(odbLayerName);
   if(it != m_layerFeatureFileList.end())
      return (*it).second;
   else
   {
      //This should never be the case.
      // DISABLED because it is better to fail than to do it wrong. This on-the-fly feature 
      // file creation would always be wrong.
      OdbFeatureFile* layerFeatureFilePtr = new OdbFeatureFile;
      m_layerFeatureFileList.insert(std::make_pair<const CString,OdbFeatureFile* > (odbLayerName,layerFeatureFilePtr));
      return layerFeatureFilePtr;
   }

}


void OdbStep::AddStepRepeatInfo(StepRepeatInfo* stepRepeatInfoPtr)
{
   m_stepRepeatInfo.push_back(stepRepeatInfoPtr);
}

void OdbStep::WriteCadNetlistInfo()
{
   CString cadNetlistFile = m_stepPath + "/";
   cadNetlistFile += "netlists/cadnet/netlist";

   int serialNum = 0;
   FILE* fp = fopen(cadNetlistFile,"w");
   if(NULL != fp)
   {
      // Header line.
      fprintf(fp, "H optimize n staggered n\n");  // Was advised to also write 'staggered n' even though such does not appear in ODB spec.

      // First pass through nets writes net serial number, which is just 
      // position in output, correlated to net name.
      POSITION netlistPos = m_file->getNetList().GetHeadPosition();
      while (netlistPos != NULL)
      {
         NetStruct *net = m_file->getNetList().GetNext(netlistPos);
         if (net != NULL)
         {
            fprintf(fp, "$%d %s\n", serialNum++, net->getNetName());
         }
      }

      // This comment is not in the ODB spec, but it is in a sample ODB exported
      // from vPlan, so we'll make it too.
      fprintf(fp, "#\n");
      fprintf(fp, "#Netlist points\n");
      fprintf(fp, "#\n");

      // Second pass through the nets, be sure to use same order, write all the
      // pads on the net. That's what the samples look like anyway. ODB spec is not explicit
      // about what this stuff is.
      serialNum = 0; // Reset

      /*POSITION*/ netlistPos = m_file->getNetList().GetHeadPosition();
      while (netlistPos != NULL)
      {
         NetStruct *net = m_file->getNetList().GetNext(netlistPos);
         if (net != NULL)
         {
            WriteCadNetlistComppins(fp, serialNum, net);
            WriteCadNetlistVias(fp, serialNum, net);
         }
         serialNum++;
      }

      fclose(fp);
      CreateSumFiles(cadNetlistFile, "netlist");
   }
}

void OdbStep::WriteCadNetlistComppins(FILE *fp, int netSerialNum, NetStruct *net)
{
   POSITION cpPos = net->getHeadCompPinPosition();
   while (cpPos != NULL)
   {
      CompPinStruct *cp = net->getNextCompPin(cpPos);
      bool cpReadyToGo = cp->getPinCoordinatesComplete();
      CPoint2d cpLoc = cp->getOrigin();
      CString side = cp->isVisibleBoth()?"B": cp->isVisibleBottom()?"D" : "T";
      BlockStruct *padstackBlock = m_file->getCamCadData().getBlockAt(cp->getPadstackBlockNumber());

      // We need the insert type of the geometry that has this compPin. What we need to find out is
      // if we are processing a TestPoint or a regular pin. We use the parental type as the insertType
      // parameter, not the actual insert type of the specific padstack, which will be insertTypePin
      // regardless of whether it is a component or a testpoint.
      InsertTypeTag insertTypeParam = insertTypePcbComponent; // Default to assume it is a regular component.
      // Check for insert type Test Point
      DataStruct *insertedCompData = m_file->FindInsertData(cp->getRefDes());
      if (insertedCompData != NULL && insertedCompData->isInsertType(insertTypeTestPoint))
         insertTypeParam = insertTypeTestPoint;

      WriteOneCadNetlistPoint(fp, netSerialNum, cpLoc, side, padstackBlock, insertTypeParam);
   }
}

void OdbStep::WriteCadNetlistVias(FILE *fp, int netSerialNum, NetStruct *net)
{
   BlockStruct *fileBlock = m_file->getBlock();
   if (fileBlock != NULL)
   {
      int netnameKw = OdbGlobalDoc->getCamCadData().getAttributeKeywordIndex(standardAttributeNetName);
      int technologyKw = OdbGlobalDoc->getCamCadData().getAttributeKeywordIndex(standardAttributeTechnology);

      CString netName(net->getNetName());

      POSITION dataPos = fileBlock->getHeadDataInsertPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = fileBlock->getNextDataInsert(dataPos);
         if (data != NULL && data->isInsertType(insertTypeVia))
         {
            Attrib *attrib;
            if (data->lookUpAttrib(netnameKw, attrib))
            {
               CString viaNetname = attrib->getStringValue();
               if (netName.CompareNoCase(viaNetname) == 0)
               {
                  InsertStruct *insert = data->getInsert();

                  bool placedTop = insert->getPlacedTop();
                  CString side = insert->getPlacedBottom()?"D": "T";  // Default to simple top or bottom.
                  // If THRU then change side to "B"oth.
                  Attrib *techAttrib;
                  if (data->lookUpAttrib(technologyKw, techAttrib))
                  {
                     CString tech = techAttrib->getStringValue();
                     if (tech.CompareNoCase("THRU") == 0)
                        side = "B"; // Both
                  }

                  CPoint2d insertLoc = insert->getOrigin2d();
                  BlockStruct *insertedBlock = m_file->getCamCadData().getBlockAt(insert->getBlockNumber());

                  WriteOneCadNetlistPoint(fp, netSerialNum, insertLoc, side, insertedBlock, insertTypeVia);
               }
            }
         }
      }
   }
}


void OdbStep::WriteOneCadNetlistPoint(FILE *fp, int netSerialNum, CPoint2d insertLoc, CString side, BlockStruct *insertedBlock, InsertTypeTag insertType)
{
   CExtent padExtent; 
   CString drillStr("0"); // Default to no drill.

   // Get drill hole size and pad size from geommetry block.
   if (insertedBlock == NULL)
   {
      // unlikely error, missing inserted block for via
   }
   else
   {
      // Pad size
      padExtent = insertedBlock->getExtent(m_file->getCamCadData());

      // Drill size
      BlockStruct *drillBlock = GetDrillInGeometry(m_file->getCamCadData(), insertedBlock);
      if (drillBlock != NULL) // Has drill. Having drill is not guaranteed, e.g. is item is SMD.
      {
         double drillDiameter = drillBlock->getToolSize(); // In CAMCAD drill size is DIAMETER.
         if (drillDiameter > SMALLNUMBER)
         {
            // In ODB here, drill size is RADIUS
            drillStr = GetFormatDecimalValue((drillDiameter / 2.0) * pageUnitsToInchFactor, pageUnitsInches);
         }
      }
      else
      {
         // Non-conformance to spec here, but samples from vPlan seem to do this. This causes
         // all points to be drawn as circle, so lack of rotation support does not make bad looking
         // images. Circles look the same at any rotation. Take the smaller of X and Y extent, treat
         // as diameter, get radius.
         double fakeDiameter = padExtent.getMinSize();
         // In ODB here, drill size is RADIUS
         drillStr = GetFormatDecimalValue((fakeDiameter / 2.0) * pageUnitsToInchFactor, pageUnitsInches);
      }
   }

   CString xStr = GetFormatDecimalValue(insertLoc.x * pageUnitsToInchFactor, pageUnitsInches);
   CString yStr = GetFormatDecimalValue(insertLoc.y * pageUnitsToInchFactor, pageUnitsInches);

   fprintf(fp, "%d", netSerialNum);
   fprintf(fp, " %s", drillStr);
   fprintf(fp, " %s", xStr);
   fprintf(fp, " %s", yStr);

   fprintf(fp, " %s", side);

   // if (drillradius is zero)
   if (drillStr.Compare("0") == 0)
   {
      // write pad width and height
      // Note that format has no support for rotations here.
      CString widthStr = GetFormatDecimalValue(padExtent.getXsize() * pageUnitsToInchFactor, pageUnitsInches);
      CString heightStr = GetFormatDecimalValue(padExtent.getYsize() * pageUnitsToInchFactor, pageUnitsInches);
      fprintf(fp, " %s %s", widthStr, heightStr);
   }

   fprintf(fp, " e"); // epoint - Roni says value not important, okay to always write "e".
   fprintf(fp, " e"); // exp - Roni says value not important, okay to always write "e".

   if (insertType == insertTypeVia)
      fprintf(fp, " v");
   else if (insertType == insertTypeTestPoint)
      fprintf(fp, " t");
   // else regular comppin, no special char tag param for that.

   // Done with record.
   fprintf(fp, "\n");
}

//================================================================================================

void OdbStep::WriteStepRepeatInfo()
{
   CString stephdrFile = m_stepPath + "/";
   stephdrFile += "stephdr";
   FILE* file = fopen(stephdrFile,"w");
   if(NULL != file)
   {
      fprintf(file,"X_DATUM=0\n");
      fprintf(file,"Y_DATUM=0\n");
      fprintf(file,"X_ORIGIN=0\n");
      fprintf(file,"Y_ORIGIN=0\n\n");
      for(unsigned int i = 0; i < m_stepRepeatInfo.size(); i++)
      {
         if(NULL != m_stepRepeatInfo[i])
         {
            fprintf(file,"STEP-REPEAT {\n");
            fprintf(file,"NAME=%s\n",m_stepRepeatInfo[i]->name);
            fprintf(file,"X=%s\n",GetFormatDecimalValue(m_stepRepeatInfo[i]->X * pageUnitsToInchFactor, pageUnitsInches));
            fprintf(file,"Y=%s\n",GetFormatDecimalValue(m_stepRepeatInfo[i]->Y * pageUnitsToInchFactor, pageUnitsInches));
            fprintf(file,"DX=0\n");
            fprintf(file,"DY=0\n");
            fprintf(file,"NX=1\n");
            fprintf(file,"NY=1\n");
            if(m_stepRepeatInfo[i]->flip)
               fprintf(file,"FLIP=YES\n");
            else
               fprintf(file,"FLIP=NO\n");
            double angle;
            if(m_stepRepeatInfo[i]->flip)
               angle = normalizeDegrees(360.0 + m_stepRepeatInfo[i]->angle);
            else
               angle = normalizeDegrees(360.0 - m_stepRepeatInfo[i]->angle);
            fprintf(file,"ANGLE=%s\n",GetFormatDecimalValue(angle, pageUnitsInches));
            fprintf(file,"MIRROR=NO\n");
            fprintf(file,"}\n\n");
         }
      } // End of for loop

      // BOM Reference
      if (!this->m_panelStep && this->m_bomData->IsUsed())
         fprintf(file, "AFFECTING_BOM=%s\n", this->m_bomData->GetBomName());

      fclose(file);
      CreateSumFiles(stephdrFile, "stephdr");
   }
}

void BomData::WriteBomCompEntries(FILE *fp)
{
   if (fp != NULL)
   {
      int itemCount = 0;

      for(unsigned int partIndx = 0; partIndx < m_bomPartList.size(); partIndx++)
      {
         //progress.incrementProgress();
         BomPartEntry* bomPart = m_bomPartList[partIndx];

         if (bomPart != NULL)
         {
            for (int refdesI = 0; refdesI < bomPart->GetCount(); refdesI++)
            {
               // NoPop valud is opposite of Loaded.
               int noPopVal = (bomPart->GetLoaded().CompareNoCase("true") == 0) ? 0 : 1;

               fprintf(fp, "REF %s\n",           bomPart->GetRefDes(refdesI));
               fprintf(fp, "CPN %s_Generated\n", bomPart->GetPartNumber());
               fprintf(fp, "NO_POP %d\n",        noPopVal);
               fprintf(fp, "\n");
            }
         }
      }
   }
}

BomData::BomData(CString bomName)
{
   // This validates the name up front, so we can just get and use it later.
   m_bomName = GetValidOdbBomName(bomName);
}

CString BomData::GetValidOdbBomName(CString bomName)
{
   CString validName;
   IsValidODBEntityName(bomName, validName, "BOM", 0);
   validName.Trim(" _");
   return validName;
}

void BomData::WriteBomPartEntries(FILE *fp)
{
   if (fp != NULL)
   {
      int itemCount = 0;

      for(unsigned int partIndx = 0; partIndx < m_bomPartList.size(); partIndx++)
      {
         //progress.incrementProgress();
         BomPartEntry* bomPart = m_bomPartList[partIndx];

         if (bomPart != NULL)
         {
            fprintf(fp, "CPN %s_Generated\n", bomPart->GetPartNumber());
            fprintf(fp, "LNFILE 0\n");
            fprintf(fp, "IPN %s\n", bomPart->GetPartNumber());
            fprintf(fp, "LNFILE 0\n");

            // These need to be written in same order as the DESC_ALIASES are defined.
            // Look for DESC_ALIASES in output elsewhere to see order.
            fprintf(fp, "DSC %s\n", bomPart->GetDescription());  // Description -1-
            fprintf(fp, "LNFILE 0\n");
            
            fprintf(fp, "DSC %s\n", bomPart->GetDeviceType());   // DeviceType  -2-
            fprintf(fp, "LNFILE 0\n");
            
            fprintf(fp, "DSC %s\n", bomPart->GetValue());        // Value       -3-
            fprintf(fp, "LNFILE 0\n");
            
            fprintf(fp, "DSC %s\n", bomPart->GetPlusTol());      // +Tol        -4-
            fprintf(fp, "LNFILE 0\n");
            
            fprintf(fp, "DSC %s\n", bomPart->GetMinusTol());     // -Tol        -5-
            fprintf(fp, "LNFILE 0\n");
            
            fprintf(fp, "DSC %s\n", bomPart->GetSubclass());     // Subclass    -6-
            fprintf(fp, "LNFILE 0\n");
            
            fprintf(fp, "DSC %s\n", bomPart->GetPinMap());       // PinMap      -7-
            fprintf(fp, "LNFILE 0\n");

            // User attribs                                                     -8-9-10-
            // Limit of 3 is ODB BOM format limit               (7 above + 3 here == 10 limit)
            int odbLimit = 10 - 7; // What is left after we used the 7 above.
            int userSettingCnt = odbSettingsFile.GetBomAttribNameCount();
            for (int attrI = 0; attrI < odbLimit && attrI < userSettingCnt; attrI++)
            {
               fprintf(fp, "DSC %s\n", bomPart->GetUserAttribValAt(attrI));
               fprintf(fp, "LNFILE 0\n");
            }

            fprintf(fp, "PKG\n"); // keyword only, data intentionally left blank
            fprintf(fp, "LNFILE 0\n");
            fprintf(fp, "QNT %.6f\n", (double)bomPart->GetCount());
            fprintf(fp, "ITEM %d\n", ++itemCount);
            fprintf(fp, "\n");
         }
      }
   }
}

void OdbStep::WriteVariantBOMFiles()
{
   POSITION vBomPos = this->m_bomVariantList.GetHeadPosition();
   while (vBomPos != NULL)
   {
      BomData *vBom = this->m_bomVariantList.GetNext(vBomPos);
      WriteBOMFile(vBom);
   }
}

void OdbStep::WriteBOMFile(BomData *bomData)
{
   // Only write if BOM is used, i.e. has some content.
   if (!bomData->IsUsed())
      return;

   // BOM structure has all these folders
   CString bomAllDirsPath( m_stepPath + "/boms/" + bomData->GetBomName() + "/files" );

   if (mkdirtree(bomAllDirsPath) == 0) // success
   {
      // The "files" folder remains empty.
      // Our "bom" file goes in folder named for BOM.
      CString bomFilePath( m_stepPath + "/boms/" + bomData->GetBomName() + "/bom" );

      FILE* fp = fopen(bomFilePath,"w");
      if(NULL != fp)
      {
         // Header
         fprintf(fp, "# header parameters\n");
         fprintf(fp, "HEADER\n");
         fprintf(fp, "BRD\n");
         fprintf(fp, "REV\n");
         fprintf(fp, "HEADER_END\n");
         fprintf(fp, "\n");

         fprintf(fp, "#description aliases\n");
         fprintf(fp, "DESC_ALIASES\n");
         fprintf(fp, "LANG\n");
         fprintf(fp, "INDEX 1\n");
         fprintf(fp, "CPN Description\n");        // Description
         fprintf(fp, "CPN Description\n");
         fprintf(fp, "INDEX 2\n");
         fprintf(fp, "CPN ElectricalType\n");     // DeviceType
         fprintf(fp, "CPN ElectricalType\n");
         fprintf(fp, "INDEX 3\n");
         fprintf(fp, "CPN ElectricalValue\n");    // Value
         fprintf(fp, "CPN ElectricalValue\n");
         fprintf(fp, "INDEX 4\n");
         fprintf(fp, "CPN PositiveTolerance\n");  // +Tol
         fprintf(fp, "CPN PositiveTolerance\n");
         fprintf(fp, "INDEX 5\n");
         fprintf(fp, "CPN NegativeTolerance\n");  // -Tol
         fprintf(fp, "CPN NegativeTolerance\n");
         fprintf(fp, "INDEX 6\n");
         fprintf(fp, "CPN Subclass\n");           // Subclass
         fprintf(fp, "CPN Subclass\n");
         fprintf(fp, "INDEX 7\n");
         fprintf(fp, "CPN PinMappings\n");        // PinMap
         fprintf(fp, "CPN PinMappings\n");

         // Can be up to three more aliases from user.
         int userAttribCnt = odbSettingsFile.GetBomAttribNameCount();
         for (int attrI = 0; attrI < 3 && attrI < userAttribCnt; attrI++)
         {
            fprintf(fp, "INDEX %d\n", (attrI + 8));
            fprintf(fp, "CPN %s\n", odbSettingsFile.GetBomAttribNameAt(attrI));
            fprintf(fp, "CPN %s\n", odbSettingsFile.GetBomAttribNameAt(attrI));
         }

         fprintf(fp, "DESC_ALIASES_END\n");

         // Components section (entry per refdes)
         fprintf(fp, "# reference descriptors and their matching customer parts\n");
         fprintf(fp, "RD_CPN\n");
         fprintf(fp, "\n");
         bomData->WriteBomCompEntries(fp);
         fprintf(fp, "RD_CPN_END\n");
         fprintf(fp, "\n");

         // Parts section (entry per partnumber)
         fprintf(fp, "# customer parts and their descriptions\n");
         fprintf(fp, "CP\n");
         fprintf(fp, "\n");
         bomData->WriteBomPartEntries(fp);
         fprintf(fp, "CP_END\n");

         fprintf(fp, "FILE_END\n");

         fclose(fp);
      }
   }
}

void OdbStep::WriteComponentLayerFeatureFile(bool top)
{
   ComponentLayerFeatureFile* featurePtr = NULL;
   CString featureFile = m_stepPath + "/";
   featureFile += "layers/";
   COperationProgress progress;
   if(top)
   {
      featurePtr = m_componentTopLayerFeatureFile;
      featureFile += "comp_+_top/components";
      CString status;
      status.Format("Writing components file of layer \"comp_+_top\"");
      progress.updateStatus(status);
      fprintf(flog,"%s...\n",status);
         
   }
   else
   {
      featurePtr = m_componentBottomLayerFeatureFile;
      featureFile += "comp_+_bot/components";
      CString status;      
      status.Format("Writing components file of layer \"comp_+_bot\"");
      progress.updateStatus(status);
      fprintf(flog,"%s...\n",status);
   }

   if(NULL != featurePtr)
   {
      FILE* file = fopen(featureFile,"w");
      if(NULL != file)
      {
         featurePtr->OutputToFile(file, &progress);
         fclose(file);
         CreateSumFiles(featureFile, "components");
      }
   }
}


void OdbStep::WriteLayerFeatureFile()
{
   OdbFeatureFile* featurePtr = NULL;
   map<CString,OdbFeatureFile* >::iterator it = m_layerFeatureFileList.begin();
   for( ; it != m_layerFeatureFileList.end(); it++)
   {
      CString featureFile = m_stepPath + "/";
      featureFile += "layers/";
      featureFile += (*it).first;
      featureFile += "/features";
      featurePtr = (*it).second;
      if(NULL != featurePtr)
      {
         FILE* file = fopen(featureFile,"w");
         if(NULL != file)
         {
            CString status;
            COperationProgress progress;
            status.Format("Writing feature file of layer \"%s\"",(*it).first);
            progress.updateStatus(status);
            fprintf(flog,"%s...\n",status);
            featurePtr->OutputToFile(file, &progress);
            fclose(file);
            CreateSumFiles(featureFile, "features");
         }
      }
   }
}


OdbFeatureFile::~OdbFeatureFile()
{
   vector<FeatureRecord* >::iterator featureListIterator = m_featureList.begin();
   for( ; featureListIterator != m_featureList.end() ; featureListIterator++)
   {
      if(NULL != (*featureListIterator))
      {
         delete (*featureListIterator);
         (*featureListIterator) = NULL;
      }
   }
}

int OdbFeatureFile::AddFeatureRecord(FeatureRecord *featureRecord)
{
   m_featureList.push_back(featureRecord);
   return m_featureList.size() - 1;
}


int OdbFeatureFile::AddSymbol(CString symName)
{
   // Name should never be blank.

   int vectorIndex = FindSymbolNumber(symName);
   if(vectorIndex != -1)
   {
      //We already have a record for this symbol
      return vectorIndex;
   }
   else
   {
      //symbol Not found.Add and Update the map.
      m_symbolTable.push_back(symName);
      int index = m_symbolTable.size() - 1;
      m_symNameIndexMap.insert(std::make_pair<const CString,int>(symName,index));      
      return index;
   }
}


int OdbFeatureFile::FindSymbolNumber(CString symName)
{
   map<CString, int>::iterator it = m_symNameIndexMap.find(symName);
   if(m_symNameIndexMap.end() != it)
   {
      //We found a record for this symbol
      return (*it).second;
   }
   else
      return -1;
}


int OdbFeatureFile::AddFeatureAttributeName(CString attributeName)
{
   int vectorIndex = FindFeatureAttributeNameNumber(attributeName);
   if(vectorIndex != -1)
   {
      //We already have a record for this Attribute Name
      return vectorIndex;
   }
   else
   {
      //Attribute Name Not found.Add and Update the map.
      m_featureAttributeNames.push_back(attributeName);
      int index = m_featureAttributeNames.size() - 1;
      m_featureAttributeNameIndexMap.insert(std::make_pair<const CString,int>(attributeName,index));      
      return index;
   }
}


int OdbFeatureFile::FindFeatureAttributeNameNumber(CString attributeName)
{
   map<CString, int>::iterator it = m_featureAttributeNameIndexMap.find(attributeName);
   if(m_featureAttributeNameIndexMap.end() != it)
   {
      //We found a record for this Attribute Name
      return (*it).second;
   }
   else
      return -1;
}


int OdbFeatureFile::AddFeatureAttributeStringValue(CString attributeStringValue)
{
   int vectorIndex = FindFeatureAttributeStringValueNumber(attributeStringValue);
   if(vectorIndex != -1)
   {
      //We already have a record for this Attribute string value
      return vectorIndex;
   }
   else
   {
      //Attribute string value not found.Add and Update the map.
      m_featureAttributeStringValues.push_back(attributeStringValue);
      int index = m_featureAttributeStringValues.size() - 1;
      m_featureAttributeStringValueIndexMap.insert(std::make_pair<const CString,int>(attributeStringValue,index));      
      return index;
   }
}


int OdbFeatureFile::FindFeatureAttributeStringValueNumber(CString attributeStringValue)
{
   map<CString, int>::iterator it = m_featureAttributeStringValueIndexMap.find(attributeStringValue);
   if(m_featureAttributeStringValueIndexMap.end() != it)
   {
      //We found a record for this Attribute string value
      return (*it).second;
   }
   else
      return -1;
}

void OdbFeatureFile::AddFeatureAttributeNames()
{
   AddFeatureAttributeName(".geometry");
   AddFeatureAttributeName(".pad_usage");
}

void OdbFeatureFile::OutputToFile(FILE* file, COperationProgress* progress)
{
   if(NULL == file)
      return;
   
   if(m_symbolTable.size())
   {
      fprintf(file,"#\n");
      fprintf(file,"#Feature symbol names\n");
      fprintf(file,"#\n");
   }

   for(unsigned int symbolNum = 0; symbolNum < m_symbolTable.size(); symbolNum++)
   {

      fprintf(file,"$%d %s\n",symbolNum,m_symbolTable[symbolNum]);
   }

   //Add Feature Attribute Names
   AddFeatureAttributeNames();
   if(m_featureAttributeNames.size())
   {
      fprintf(file,"#\n");
      fprintf(file,"#Feature attribute names\n");
      fprintf(file,"#\n");
   }
   
   for(unsigned int attributeNum = 0; attributeNum < m_featureAttributeNames.size(); attributeNum++)
   {
      fprintf(file,"@%d %s\n",attributeNum,m_featureAttributeNames[attributeNum]);
   }

   if(m_featureAttributeStringValues.size())
   {
      fprintf(file,"#\n");
      fprintf(file,"#Feature attribute text strings\n");
      fprintf(file,"#\n");
   }

   for(unsigned int attributeNameNum = 0; attributeNameNum < m_featureAttributeStringValues.size(); attributeNameNum++)
   {
      fprintf(file,"&%d %s\n",attributeNameNum,m_featureAttributeStringValues[attributeNameNum]);
   }

   if(m_featureList.size())
   {
      fprintf(file,"#\n");
      fprintf(file,"#Layer features\n");
      fprintf(file,"#\n");
   }
   
   if(NULL != progress)
      progress->setLength(m_featureList.size());
   for(unsigned int featureNum = 0; featureNum< m_featureList.size(); featureNum++)
   {
      if(NULL != progress)
         progress->incrementProgress();
      FeatureRecord* feature = m_featureList[featureNum];
      if(NULL == feature)
         continue; //This should never happen
      void* ptr = feature->GetFeatureTypePtr();
      vector<ODBAttribute* > attributeList;
      attributeList.empty();
      if(feature->GetFeatureType() == FEATURE_LINE)
      {
         LineRecord *lineRecord = (LineRecord*)ptr;
         if(NULL == lineRecord)
            continue;
         fprintf(file,"L %s %s %s %s %d %c %d",
                                                      GetFormatDecimalValue(lineRecord->m_startPoint.x * pageUnitsToInchFactor, pageUnitsInches),
                                                      GetFormatDecimalValue(lineRecord->m_startPoint.y * pageUnitsToInchFactor, pageUnitsInches),
                                                      GetFormatDecimalValue(lineRecord->m_endPoint.x * pageUnitsToInchFactor, pageUnitsInches),
                                                      GetFormatDecimalValue(lineRecord->m_endPoint.y * pageUnitsToInchFactor, pageUnitsInches),
                                                      lineRecord->m_symNum,
                                                      lineRecord->m_polarity,
                                                      lineRecord->m_dcode);
         attributeList = lineRecord->m_attributeList;
      }
      else if(feature->GetFeatureType() == FEATURE_PAD)
      {
         PadRecord* padRecord = (PadRecord*)ptr;
         if(NULL == padRecord)
            continue;

         // DR 844415 was a real head spinner about pad rotation. It led to this handling of the rotation
         // instead of what was here before, which was to leave the angle as-is and use 8 for non-mirror
         // and 9 for mirrored case. It looked okay in vPlan, but did not import back to CAMCAD. I suspect
         // the angle representation was not correct but worked anyway in vPlan. With this mod, it reads
         // into both vPlan and CAMCAD and looks right.
         int rotationCode = 8;
         double rotationAngle = padRecord->m_rotation;
         if (padRecord->m_mirror)
            rotationAngle = normalizeDegrees(360.0 - padRecord->m_rotation);

         fprintf(file,"P %s %s %d %c %d %d %s;%d=%d",
                                                     GetFormatDecimalValue(padRecord->m_centerPoint.x * pageUnitsToInchFactor, pageUnitsInches),
                                                     GetFormatDecimalValue(padRecord->m_centerPoint.y * pageUnitsToInchFactor, pageUnitsInches),
                                                     padRecord->m_symNum,
                                                     padRecord->m_polarity,
                                                     padRecord->m_dcode,
                                                     rotationCode,
                                                     GetFormatDecimalValue(rotationAngle, pageUnitsInches),
                                                     FindFeatureAttributeNameNumber(".pad_usage"),
                                                     padRecord->m_padUsageType);
         attributeList = padRecord->m_attributeList;
      }
      else if(feature->GetFeatureType() == FEATURE_ARC)
      {
         ArcRecord* arcRecord = (ArcRecord*)ptr;
         if(NULL == arcRecord)
            continue;
         fprintf(file,"A %s %s %s %s %s %s %d %c %d %c",
                                                                   GetFormatDecimalValue(arcRecord->m_startPoint.x * pageUnitsToInchFactor, pageUnitsInches),
                                                                   GetFormatDecimalValue(arcRecord->m_startPoint.y * pageUnitsToInchFactor, pageUnitsInches),
                                                                   GetFormatDecimalValue(arcRecord->m_endPoint.x * pageUnitsToInchFactor, pageUnitsInches),
                                                                   GetFormatDecimalValue(arcRecord->m_endPoint.y * pageUnitsToInchFactor, pageUnitsInches),
                                                                   GetFormatDecimalValue(arcRecord->m_centerPoint.x * pageUnitsToInchFactor, pageUnitsInches),
                                                                   GetFormatDecimalValue(arcRecord->m_centerPoint.y * pageUnitsToInchFactor, pageUnitsInches),
                                                                   arcRecord->m_symNum,
                                                                   arcRecord->m_polarity,
                                                                   arcRecord->m_dcode,
                                                                   arcRecord->m_clockWise);
         attributeList = arcRecord->m_attributeList;
      }
      else if(feature->GetFeatureType() == FEATURE_TEXT)
      {
         CString font = "standard";
         TextRecord* textRecord = (TextRecord*)ptr;
         if(NULL == textRecord)
            continue;
         int rotation;
         if(!textRecord->m_mirror)
            rotation = 8;
         else
            rotation = 9;
         int version = 1;

         // Width Factor seems to ignore units for rest of ODB job.
         // Quote from ODB spec: width of character segment (in units of 12 mils) i.e. 1 = 12 mils, 0.5 = 6 mils
         double penWidthInches = textRecord->m_penWidth * pageUnitsToInchFactor;
         double penWidthMils = penWidthInches * 1000.0;
         double widthFactor = penWidthMils / 12.0;
         // DR 851214 Says width factor can not be zero, must be positive.
         // Text with zerowidth pen will get us into this situation.
         if (widthFactor < 0.001)
            widthFactor = 0.335;  // Nothing special about this value. Is relatively small, positive value.

         fprintf(file,"T %s %s %s %c %d %s %s %s %0.3f '%s' %d",
                                                                    GetFormatDecimalValue(textRecord->m_textLocation.x * pageUnitsToInchFactor, pageUnitsInches),
                                                                    GetFormatDecimalValue(textRecord->m_textLocation.y * pageUnitsToInchFactor, pageUnitsInches),
                                                                    font,
                                                                    textRecord->m_polarity,
                                                                    rotation,
                                                                    GetFormatDecimalValue(textRecord->m_rotation, pageUnitsInches),
                                                                    GetFormatDecimalValue(textRecord->m_xsize * pageUnitsToInchFactor, pageUnitsInches),
                                                                    GetFormatDecimalValue(textRecord->m_ysize * pageUnitsToInchFactor, pageUnitsInches),
                                                                    widthFactor,
                                                                    textRecord->m_text,
                                                                    version);
         attributeList = textRecord->m_attributeList;

      }
      else if(feature->GetFeatureType() == FEATURE_SURFACE)
      {
         SurfaceRecord* surfaceRecord = (SurfaceRecord*)ptr;
         if(NULL == surfaceRecord || NULL == surfaceRecord->m_polyList)
            continue;

         // DR 851203 Polylist must have one close poly to make valid "Surface" (S).
         // Closed poly must have atleast 3 pts; Here, to There, back to Here.
         if (!surfaceRecord->m_polyList->hasPoints(3))
            continue;

         // Ok, start Surface.
         fprintf(file,"S %c %d",surfaceRecord->m_polarity,surfaceRecord->m_dcode);
         attributeList = surfaceRecord->m_attributeList;
      }
      //Write Attributes
      if(attributeList.size())
      {
         fprintf(file," ;");
         bool first = true;
         unsigned int size = attributeList.size();
         for(unsigned int attributeNum = 0; attributeNum < size; attributeNum++)
         {            
            ODBAttribute* attribute = attributeList[attributeNum];
            if(attribute->m_isString)
               fprintf(file,"%d=%d",attribute->m_attributeNum,attribute->m_attributeStringNum);
            else
               fprintf(file,"%d=%s",attribute->m_attributeNum,attribute->m_value);
            if(attributeNum < size)
               fprintf(file,",");
         }
      }

      fprintf(file,"\n");

      if(feature->GetFeatureType() == FEATURE_SURFACE)
      {
         SurfaceRecord* surfaceRecord = (SurfaceRecord*)ptr;
         if(NULL != surfaceRecord->m_polyList && surfaceRecord->m_polyList->hasPoints(3))
         {
            POSITION pos = surfaceRecord->m_polyList->GetHeadPosition();
            while (pos)
            {
               CPoly* poly = surfaceRecord->m_polyList->GetNext(pos);
               if (poly)
               {
                  WritePolygon(poly, file);
               }
            }
            // End surface.
            fprintf(file,"SE\n");
         }
      }
   }

}


ComponentLayerFeatureFile* OdbStep::GetComponentTopLayerFeatureFile()
{
   if(NULL != m_componentTopLayerFeatureFile)
      return m_componentTopLayerFeatureFile;

   m_componentTopLayerFeatureFile = new ComponentLayerFeatureFile;
   return m_componentTopLayerFeatureFile;
}


ComponentLayerFeatureFile* OdbStep::GetComponentBottomLayerFeatureFile()
{
   if(NULL != m_componentBottomLayerFeatureFile)
      return m_componentBottomLayerFeatureFile;

   m_componentBottomLayerFeatureFile = new ComponentLayerFeatureFile;
   return m_componentBottomLayerFeatureFile;
}

ComponentLayerFeatureFile::~ComponentLayerFeatureFile()
{
   vector<ODBComponent*>::iterator componentListIterator = m_componentList.begin();
   for( ; componentListIterator != m_componentList.end(); componentListIterator++)
   {
      if(NULL != (*componentListIterator))
      {
         delete (*componentListIterator);
         (*componentListIterator) = NULL;
      }
   }
}

ODBComponent* ComponentLayerFeatureFile::AddODBComponent(DataStruct* data, CString refName, int pkgNum,double boardScaleFactor, int & index)
{
   ODBComponent* temp = NULL;
   if(NULL != data)
   {      
      int vectorIndex = FindComponentNumber(refName);
      if(vectorIndex != -1)
      {
         //We already have a record for this component.
         ODBComponent *existingComp = FindComponent(refName);
         fprintf(flog, "Duplicate instance of component [%s] found. Keeping entity [%d], skipping entity [%d]\n",
            refName, existingComp->GetCompDataEntityNumber(), data->getEntityNumber());
         return NULL;
      }
      else
      {
         //Create a new record and add it to m_componentList vector
         //Update the map m_compNameIndexMap.
         temp = new ODBComponent(data, pkgNum, boardScaleFactor, refName);
         m_componentList.push_back(temp);
         index = m_componentList.size() - 1;
         m_compNameIndexMap.insert(std::make_pair<const CString,int>(refName,index));
         return temp;
      }
   }
   return temp;
}


int ComponentLayerFeatureFile::FindComponentNumber(CString compName)
{
   map<CString, int>::iterator it = m_compNameIndexMap.find(compName);
   if(m_compNameIndexMap.end() != it)
   {
      //Found a record for this component
      return (*it).second;
   }
   else 
      return -1;
}

ODBComponent *ComponentLayerFeatureFile::FindComponent(CString compName)
{
   int indx = FindComponentNumber(compName);
   if (indx > -1 && indx < (int)m_componentList.size())
   {
      return this->m_componentList[indx];
   }

   return NULL;
}


void ComponentLayerFeatureFile::OutputToFile(FILE* file, COperationProgress* progress)
{   
   if(NULL != progress)
      progress->setLength(m_componentList.size());

   int compMountTypeAttribIndx = 0;

   fprintf(file, "#\n");
   fprintf(file, "#Component attribute names\n");
   fprintf(file, "#\n");
   fprintf(file, "@%d .comp_mount_type\n", compMountTypeAttribIndx);
   fprintf(file, "\n");

   for(unsigned int compNum = 0; compNum < m_componentList.size(); compNum++)
   {
      if(NULL != progress)
         progress->incrementProgress();
      ODBComponent* comp = m_componentList[compNum];
      if(NULL != comp)
      {
         DataStruct* compData = comp->GetCompDataStruct();
         if(NULL != compData)
         {
            fprintf(file,"#CMP %d\n",compNum);
            int pkgNum = comp->GetPkgNum();
            double originX = compData->getInsert()->getOriginX() * comp->GetBoardScaleFactor();
            double originY = compData->getInsert()->getOriginY() * comp->GetBoardScaleFactor();

            // It seems strange to me that we always use N (no) for mirror, even when component is
            // placed bottom and mirrored. But it seems to work this way, and has been seen to not
            // work when we tried M here. So we always go with N.
            CString mirror("N");

            double compRotation;
            if(compData->getInsert()->getGraphicMirrored())
               compRotation = normalizeDegrees(360.0 + compData->getInsert()->getAngleDegrees());
            else
               compRotation = normalizeDegrees(360.0 - compData->getInsert()->getAngleDegrees());
            CString refName = comp->GetRefName();

            CString partNumber = "";
            Attrib *attrib = NULL;
            attrib = is_attvalue(OdbGlobalDoc, compData->getAttributesRef(), ATT_PARTNUMBER, 1);
            if(NULL != attrib)
            {
               partNumber = OdbGlobalDoc->getAttributeStringValue(attrib);
            }
            
            if(partNumber.IsEmpty())
            {
               attrib = is_attvalue(OdbGlobalDoc, compData->getAttributesRef(), ATT_TYPELISTLINK, 1);
               if(NULL != attrib)
               {
                  partNumber = OdbGlobalDoc->getAttributeStringValue(attrib);
               }
            }
            if(partNumber.IsEmpty())
            {
               partNumber = "PART_" + refName;
            }
            //partNumber.Replace(" ", "_"); // Don't allow blanks in field

            // Mount Type
            int compMountTypeAttribValue = 0; // 0 = Other, which we will not actually output.
            attrib = is_attvalue(OdbGlobalDoc, compData->getAttributesRef(), ATT_TECHNOLOGY, 1);
            if(NULL != attrib)
            {
               CString techVal(attrib->getStringValue());
               if (techVal.CompareNoCase("SMD") == 0)
                  compMountTypeAttribValue = 1; // SMT in ODB
               else
                  compMountTypeAttribValue = 2; // THMT in ODB
            }

            // Basic CMP record.
            fprintf(file,"CMP %d %s %s %s %s %s '%s'",
                                                       pkgNum,
                                                       GetFormatDecimalValue(originX * pageUnitsToInchFactor, pageUnitsInches),
                                                       GetFormatDecimalValue(originY * pageUnitsToInchFactor, pageUnitsInches),
                                                       GetFormatDecimalValue(compRotation, pageUnitsInches),
                                                       mirror,
                                                       refName,
                                                       partNumber);

            // Add comp_mount_type if it was set.
            if (compMountTypeAttribValue != 0)
            {
               fprintf(file, " ; %d=%d", compMountTypeAttribIndx, compMountTypeAttribValue);
            }

            fprintf(file, "\n");

            //Write Attributes, these are PROPERTIES as opposed to the .comp_mount_type attributes.
            vector<CAMCADAttribute*> propertyList = GetCAMCADAttributeList(compData->getAttributesRef());
            for(unsigned int propertyNum = 0 ; propertyNum < propertyList.size(); propertyNum++)
            {
               CAMCADAttribute* camcadAttribute = propertyList[propertyNum];
               if(NULL == camcadAttribute)
                  continue;
               fprintf(file,"PRP %s \'%s\'\n",camcadAttribute->m_attributeName,camcadAttribute->m_value);
            }
            // Cleanup, free mem
            for(unsigned int propertyNum = 0 ; propertyNum < propertyList.size(); propertyNum++)
            {
               CAMCADAttribute* camcadAttribute = propertyList[propertyNum];
               //cleanup
               if(NULL != camcadAttribute)
               {
                  delete camcadAttribute;
                  propertyList[propertyNum] = NULL;
               }
            }
            
            int unnamedPinCount = 0;
            vector<ODBCompPin* > compPinList = comp->GetCompPinList();
            for(unsigned int pinNum = 0; pinNum < compPinList.size(); pinNum++)
            {
               ODBCompPin* compPin = compPinList[pinNum];
               if(NULL != compPin)
               {
                  // DR 851234 Brought us a CCZ with pins with blank refname. ODB can't handle that. 
                  // Ensure non-blank pin name. If it has no name then it can't be in netlist, so this
                  // is probably not a big deal.
                  // Keep this in sync with other place that has this comment. DR 851234.
                  CString validPinName(compPin->m_pinName);
                  validPinName.Trim();
                  if (validPinName.IsEmpty())
                     validPinName.Format("Unnamed%02d", ++unnamedPinCount);
                  // Can't have white space in the pin name since record is space delimited, and apparantly
                  // we can't just quote it.
                  validPinName.Replace(" ",  "_");
                  validPinName.Replace("\t", "_");

                  fprintf(file,"TOP %d %s %s %s %s %d %d %s\n",
                                                               pinNum,
                                                               GetFormatDecimalValue(compPin->m_X * pageUnitsToInchFactor, pageUnitsInches),
                                                               GetFormatDecimalValue(compPin->m_Y * pageUnitsToInchFactor, pageUnitsInches),
                                                               GetFormatDecimalValue(compPin->m_rotation, pageUnitsInches),
                                                               compPin->m_mirror,
                                                               compPin->m_netNumber,
                                                               compPin->m_subNetNumber,
                                                               validPinName);
               }
            }
            fprintf(file,"\n\n");
         }
      }
   }
}


ODBComponent::ODBComponent(DataStruct *data, int pkgNum, double scale, CString refName)
{
   m_data = data;
   m_pkgNum = pkgNum;
   m_boardScaleFactor = scale;
   m_refName = refName;
   m_loaded = true; // until proven false by LOADED attrib

   WORD partnumKW = OdbGlobalDoc->IsKeyWord(ATT_PARTNUMBER, 0);
   WORD loadedKW  = OdbGlobalDoc->IsKeyWord(ATT_LOADED, 0);

   CAttribute *attrib;

   if (data->getAttributes() && data->getAttributes()->Lookup(partnumKW, attrib))
   {
      this->m_partnumber = attrib->getStringValue();

      // ODB++ does not differentiate PNs by upper/lower case, though CAMCAD does.
      // For ODB++ export, force PNs to upper case here, to avoid outputting 
      // separate upper and lower case versions later, and having vPlan fail to import it.
      this->m_partnumber.MakeUpper(); 
   }

   if (data->getAttributes() && data->getAttributes()->Lookup(loadedKW, attrib))
   {
      this->m_loaded = (attrib->getStringValue().CompareNoCase("FALSE") != 0);
   }
}
 
ODBComponent::~ODBComponent()
{
   vector<ODBCompPin* >::iterator compPinListIterator = m_CompPinList.begin();
   for( ; compPinListIterator != m_CompPinList.end(); compPinListIterator++ )
   {
      if(NULL != (*compPinListIterator))
      {
         delete (*compPinListIterator);
         (*compPinListIterator) = NULL;
      }
   }
}
int ODBComponent::AddCompPin(ODBCompPin* compPin)
{
   m_CompPinList.push_back(compPin);
   return m_CompPinList.size() - 1;
}


FeatureRecord::FeatureRecord(FeatureType type)
{
   m_type = type;
   switch(m_type)
   {
   case FEATURE_LINE :
      m_featureRecordPtr = (void*) new LineRecord;
      break;
   case FEATURE_PAD :
      m_featureRecordPtr = (void*) new PadRecord;
      break;
   case FEATURE_ARC :
      m_featureRecordPtr = (void*) new ArcRecord;
      break;
   case FEATURE_TEXT :
      m_featureRecordPtr = (void*) new TextRecord;
      break;
   case FEATURE_SURFACE :
      m_featureRecordPtr = (void*) new SurfaceRecord;
      break;
   default:
      {
      }
   }
}

FeatureRecord::~FeatureRecord()
{
   if(NULL != m_featureRecordPtr)
   {
      switch(m_type)
      {
      case FEATURE_LINE :
         {
            LineRecord* lineRecord = (LineRecord*)m_featureRecordPtr;
            if( NULL != lineRecord)
            {
               vector<ODBAttribute* >::iterator attributeListIterator = lineRecord->m_attributeList.begin();
               for( ; attributeListIterator != lineRecord->m_attributeList.end(); attributeListIterator++)
               {
                  if(NULL != (*attributeListIterator))
                  {
                     delete (*attributeListIterator);
                     (*attributeListIterator) = NULL;
                  }
               }
               delete lineRecord;
            }
         }
         break;
      case FEATURE_PAD :
         {
            PadRecord* padRecord = (PadRecord*)m_featureRecordPtr;
            if(NULL != padRecord)
            {
               vector<ODBAttribute* >::iterator attributeListIterator = padRecord->m_attributeList.begin();
               for( ; attributeListIterator != padRecord->m_attributeList.end(); attributeListIterator++)
               {
                  if(NULL != (*attributeListIterator))
                  {
                     delete (*attributeListIterator);
                     (*attributeListIterator) = NULL;
                  }
               }
               delete padRecord;
            }
         }
         break;
      case FEATURE_ARC :
         {
            ArcRecord* arcRecord = (ArcRecord*)m_featureRecordPtr;
            if(NULL != arcRecord)
            {
               vector<ODBAttribute* >::iterator attributeListIterator = arcRecord->m_attributeList.begin();
               for( ; attributeListIterator != arcRecord->m_attributeList.end(); attributeListIterator++)
               {
                  if(NULL != (*attributeListIterator))
                  {
                     delete (*attributeListIterator);
                     (*attributeListIterator) = NULL;
                  }
               }
               delete arcRecord;
            }
         }
         break;
      case FEATURE_TEXT :
         {
            TextRecord* textRecord = (TextRecord*)m_featureRecordPtr;
            if(NULL != textRecord)
            {
               vector<ODBAttribute* >::iterator attributeListIterator = textRecord->m_attributeList.begin();
               for( ; attributeListIterator != textRecord->m_attributeList.end(); attributeListIterator++)
               {
                  if(NULL != (*attributeListIterator))
                  {
                     delete (*attributeListIterator);
                     (*attributeListIterator) = NULL;
                  }
               }
               delete textRecord;
            }
         }
         break;
      case FEATURE_SURFACE :
         {
            SurfaceRecord* surfaceRecord = (SurfaceRecord*)m_featureRecordPtr;
            if(NULL != surfaceRecord)
            {
               vector<ODBAttribute* >::iterator attributeListIterator = surfaceRecord->m_attributeList.begin();
               for( ; attributeListIterator != surfaceRecord->m_attributeList.end(); attributeListIterator++)
               {
                  if(NULL != (*attributeListIterator))
                  {
                     delete (*attributeListIterator);
                     (*attributeListIterator) = NULL;
                  }
               }
               if(NULL != surfaceRecord->m_polyList)
               {
                  FreePolyList(surfaceRecord->m_polyList);
                  surfaceRecord->m_polyList = NULL;
               }
               delete surfaceRecord;
            }
         }
         break;
      default:
         {
         }
      }
      m_featureRecordPtr = NULL;
   }
}

bool IsValidOdbLayerName(LayerStruct *layer, CString &validatedName)
{
   // Return true if layer name is okay as-is, false if not.
   // Always just use validatedName return val, only need to check if original
   // was valid if that is what is specifically needed out there.

   validatedName.Empty();
   bool retval = false;

   if (layer != NULL)
   {
      retval = IsValidODBEntityName(layer->getName(), validatedName, "Layer", layer->getLayerIndex()); // just get and use validated name, don't care if original was valid
      // DR 842556 - Can't have two layers with same name, add layer index to name if it matches our orphan layer.
      if (validatedName.CompareNoCase(ORPHAN_LAYER_NAME) == 0)
      {
         validatedName.Format("%s%d", ORPHAN_LAYER_NAME, layer->getLayerIndex());
         retval = false; // Was not valid as-is due to orphan layer conflict, not due to ODB name limitations.
      }
   }

   return retval;
}

bool IsValidODBNetName(const CString & name, CString & validName, CString entityTypeName, int entityID)
{
   // Similar to IsValidODBEntityName, except no limitation on first char
   // and mixed upper/lower case allowed.

   validName = name;
   bool isValidChar = true;
   map<CString,CString>::iterator it = invalidValidNameMap.find(name);
   if(it != invalidValidNameMap.end())
   {
      validName = (*it).second;
      return false;
   }
 
   // DR 850639 - Enforce 64 char limit for entity names, an ODB limit.
   if (validName.GetLength() > 64)
   {
      // Name length limit exceeded. Convert to generic name built from entity type and ID.
      validName.Format("%s_%d", entityTypeName, entityID);
      isValidChar = false;
   }
   else
   {
      // Valid chars besides digits and upper/lower case alpha.
      CString otherValidChars("!@#$%^&*()_+-{}[]|:,.");

      // Name length okay, scan for and convert invalid characters.
      for (int i = 0; i < validName.GetLength(); i++)
      {      
         char character = validName.GetAt(i);

         bool isDigit       = (character >= '0' && character <= '9');  // digits 0..9
         bool isLowerAlpha  = (character >= 'a' && character <= 'z');  // lower case a..z
         bool isUpperAlpha  = (character >= 'A' && character <= 'Z');  // upper case A..Z
         bool isOtherOkChar = (otherValidChars.Find(character) > -1);

         bool isValidChar = isDigit || isLowerAlpha || isUpperAlpha || isOtherOkChar;

         if (!isValidChar)
         {
            validName.SetAt(i, '_');
         }
      }
   }

   // Cache mapping of name to validName.
   // Report name change to log.
   if (false == isValidChar)
   {
      CacheAndReportNameChange(entityTypeName, name, validName);
   }

   return isValidChar;
}

void CacheAndReportNameChange(CString entityTypeName, CString originalName, CString validatedName)
{
   // Cache
   invalidValidNameMap.insert(std::make_pair<const CString,CString>(originalName,validatedName));

   // Log
   CString message;
   //message.Format("Invalid %s name \"%s\"\n", entityTypeName, name);
   //fprintf(flog,message);
   message.Format("%s name changed from \"%s\" to \"%s\"\n", entityTypeName, originalName, validatedName);
   fprintf(flog,message);
}

bool IsValidODBEntityName(const CString & name, CString & validName, CString entityTypeName, int entityID, bool attributeName)
{
   validName = name;
   bool isValidChar = true;
   map<CString,CString>::iterator it = invalidValidNameMap.find(name);
   if(it != invalidValidNameMap.end())
   {
      validName = (*it).second;
      return false;
   }
 
   // DR 850639 - Enforce 64 char limit for entity names, an ODB limit.
   if (validName.GetLength() > 64)
   {
      // Name length limit exceeded. Convert to generic name built from entity type and ID.
      entityTypeName.MakeLower(); // ODB doesn't like upper case in general.
      validName.Format("%s_%d", entityTypeName, entityID);
      isValidChar = false;
   }
   else
   {
      // Name length okay, scan for and convert invalid characters.
      for (int i = 0; i < validName.GetLength(); i++)
      {      
         int character = validName.GetAt(i);

         // If not an attribute name then first char can not be dot, minus, or plus.
         if((i == 0) 
            && !attributeName 
            && ((character == 46 /*.*/) || (character == 45 /*-*/) || (character == 43 /*+*/)))
         {
            isValidChar = false;
            validName.SetAt(i,'_');
         }

         // Valid chars are plus, minus, dot, underscore, digits, and lower case chars.
         // If not one of these then convert it.
         if (!(character == 43     /*+*/
            || character == 45  /*-*/
            || character == 46  /*.*/ 
            || character == 95  /*_*/
            || (character >=48 && character <= 57)       // digits 0..9
            || (character >= 97 && character <= 122)))   // lower case a..z
         {
            isValidChar = false;
            //Upper case Character,make it lower case
            if(character >= 65 && character <= 90)   // upper case A..Z
               validName.SetAt(i, tolower(character));
            else
               validName.SetAt(i, '_');
         }
      }
   }

   // Cache mapping of name to validName.
   // Report name change to log.
   if (false == isValidChar)
   {
      CacheAndReportNameChange(entityTypeName, name, validName);
   }
      
   return isValidChar;
}


EdaDataNet::EdaDataNet(NetStruct* net, CString netName)
{
   m_camcadNetPtr = net;
   m_cczNetName = netName;

   CString validNetName;
   IsValidODBNetName(netName, validNetName, "Net", net?net->getEntityNumber():99999);
   m_odbNetName = validNetName;
}   


EdaDataNet::~EdaDataNet()
{
   vector<SNTToePrintRecord *>::iterator toePrintRecordListIterator = m_toePrintRecordList.begin();
   for( ; toePrintRecordListIterator != m_toePrintRecordList.end(); toePrintRecordListIterator++)
   {
      if(NULL != (*toePrintRecordListIterator))
      {
         vector<FeatureIDRecord* >::iterator featureIDListIterator = (*toePrintRecordListIterator)->m_featureIDList.begin();
         for( ; featureIDListIterator != (*toePrintRecordListIterator)->m_featureIDList.end(); featureIDListIterator++)
         {
            if(NULL != (*featureIDListIterator))
            {
               delete (*featureIDListIterator);
               (*featureIDListIterator) = NULL;
            }
         }
         delete (*toePrintRecordListIterator);
         (*toePrintRecordListIterator) = NULL;
      }
   }

   vector<SNTViaRecord *>::iterator viaRecordListIterator = m_viaRecordList.begin();
   for( ; viaRecordListIterator != m_viaRecordList.end(); viaRecordListIterator++)
   {
      if(NULL != (*viaRecordListIterator))
      {
         vector<FeatureIDRecord* >::iterator featureIDListIterator = (*viaRecordListIterator)->m_featureIDList.begin();
         for( ; featureIDListIterator != (*viaRecordListIterator)->m_featureIDList.end(); featureIDListIterator++)
         {
            if(NULL != (*featureIDListIterator))
            {
               delete (*featureIDListIterator);
               (*featureIDListIterator) = NULL;
            }
         }
         delete (*viaRecordListIterator);
         (*viaRecordListIterator) = NULL;
      }
   }

   vector<FeatureIDRecord* >::iterator traceRecordListIterator = m_traceRecordList.begin();
   for( ; traceRecordListIterator != m_traceRecordList.end(); traceRecordListIterator++)
   {
      if(NULL != (*traceRecordListIterator))
      {
         delete (*traceRecordListIterator);
         (*traceRecordListIterator) = NULL;
      }
   }

   multimap<int, ODBAttribute* >::iterator attributeListIterator = m_attributeList.begin();
   for( ; attributeListIterator != m_attributeList.end() ; attributeListIterator++)
   {
      if(NULL != (*attributeListIterator).second)
      {
         delete (*attributeListIterator).second;
         (*attributeListIterator).second = NULL;
      }
   }
}


void EdaDataNet::AddSNTTraceRecord(FeatureIDRecord* record)
{
   m_traceRecordList.push_back(record);
}


void EdaDataNet::AddSNTToePrintRecord(SNTToePrintRecord* record)
{
   m_toePrintRecordList.push_back(record);
}


void EdaDataNet::AddSNTViaRecord(SNTViaRecord* record)
{
   m_viaRecordList.push_back(record);
}


void EdaDataNet::AddODBAttribute(ODBAttribute * odbAttribute)
{
   if(NULL == odbAttribute)
      return;
   std::multimap<int, ODBAttribute* >::iterator it = m_attributeList.find(odbAttribute->m_attributeNum);
   if(m_attributeList.end() == it)
      m_attributeList.insert(std::make_pair<const int,ODBAttribute *>(odbAttribute->m_attributeNum, odbAttribute));
   else
   {
      std::multimap<int, ODBAttribute* >::iterator lowerBound = m_attributeList.lower_bound(odbAttribute->m_attributeNum);
      std::multimap<int, ODBAttribute* >::iterator upperBound = m_attributeList.upper_bound(odbAttribute->m_attributeNum);
      bool sameElement = false;
      for(;lowerBound != upperBound;lowerBound++)
      {
         if(((*lowerBound).second->m_value == odbAttribute->m_value)
            && ((*lowerBound).second->m_attributeStringNum == odbAttribute->m_attributeStringNum)
            && ((*lowerBound).second->m_isString == odbAttribute->m_isString))
         {
            sameElement = true;
            break;
         }
      }
      if(!sameElement)
         m_attributeList.insert(std::make_pair<const int,ODBAttribute *>(odbAttribute->m_attributeNum, odbAttribute));
   }
}


EdaDataPkg::EdaDataPkg(BlockStruct *blockPtr, bool isBottomBuilt, double scaleFactor)
{
   m_block = blockPtr;
   m_scaleFactor = scaleFactor;
   m_isBottomBuilt = isBottomBuilt;
}


EdaDataPkg::~EdaDataPkg()
{
   vector<ODBAttribute* >::iterator attributeListIterator = m_attributeList.begin();
   for( ; attributeListIterator != m_attributeList.end(); attributeListIterator++)
   {
      if(NULL != (*attributeListIterator))
      {
         delete (*attributeListIterator);
         (*attributeListIterator) = NULL;
      }
   }
}

CString EdaDataPkg::GetName()
{
   // If first time then validate the member block's name and cache it.
   // Return the validated name.

   if (m_validatedName.IsEmpty())
   {
      m_validatedName = GetValidatedName(m_block);

      if (m_validatedName.Find("PkgBlock") == 0)
      {
         fprintf(flog,"Geometry name [%s] too long for ODB Package, reassigned to [%s]\n",
            m_block->getName(), m_validatedName);
      }
   }

   return m_validatedName;
}

CString EdaDataPkg::GetValidatedName(BlockStruct *geomBlock)
{
   // Can not have blanks in name. Records in ODB are blank delimited.
   // Replace blanks with underscore.

   // DR 842552 - Can't be more than 64 chars.

   CString name;
   if (geomBlock != NULL)
   {
      name = geomBlock->getName();
      name.Replace(" ", "_");

      // DR 842552 - ODB has a name length limit of 64 chars on package names.
      if (name.GetLength() > 64)
      {
         name.Format("PkgBlock_%d", geomBlock->getBlockNumber());
      }
   }

   return name;
}

BlockStruct* EdaDataPkg::getCamcadBlockStruct()
{
   return m_block;
}


vector<ODBAttribute* > EdaDataPkg::GetAttributeList()
{
   return m_attributeList;
}

void EdaDataPkg::AddODBAttribute(ODBAttribute* odbAttribute)
{
   m_attributeList.push_back(odbAttribute);
}

double EdaDataPkg::GetScaleFactor()
{
   return m_scaleFactor;
}

bool EdaDataPkg::GetIsBottomBuilt()
{
   return m_isBottomBuilt;
}


vector<CAMCADAttribute*> GetCAMCADAttributeList(CAttributes*& attributeMap)
{
   Attrib* parentAttrib;
   CString buf;
   WORD word;
   CString keyword;
   vector<CAMCADAttribute*> camcadAttributeList;
   camcadAttributeList.empty(); //Just a precaution
   CAttributeMapWrap* map = new CAttributeMapWrap(attributeMap);
   if(NULL == map)
      return camcadAttributeList;
   int decimals = GetDecimals(OdbGlobalDoc->getSettings().getPageUnits());
   for (POSITION pos = map->GetStartPosition();NULL != pos;)
   {
      map->GetNextAssoc(pos, word, parentAttrib);
      bool multipleInstanceFlag = (parentAttrib->getCount() > 1);
      int instanceNumber = 0;

      AttribIterator attribIterator(parentAttrib);
      Attrib attrib(parentAttrib->getCamCadData());

      while (attribIterator.getNext(attrib))
      {
         const KeyWordStruct* keywordStruct = OdbGlobalDoc->getKeyWordArray()[word];

         keyword = keywordStruct->getCCKeyword();
         CAMCADAttribute* camcadAttribute = new CAMCADAttribute;
         camcadAttribute->m_isString = false;
         camcadAttribute->m_attributeName = keyword;
         switch(attrib.getValueType())
         {
         case valueTypeNone:
            buf = "";
            break;
         case valueTypeInteger:
            buf.Format("%d", attrib.getIntValue());
            break;
         case valueTypeDouble:
         case valueTypeUnitDouble:
            buf.Format("%.*lf", decimals, attrib.getDoubleValue());
            break;
         case valueTypeString:
            {
               camcadAttribute->m_isString = true;
               if (attrib.getStringValueIndex() == -1)
               {
                  buf = "";
               }
               else
               {
                  char *tok, *temp = STRDUP(attrib.getStringValue());

                  if (NULL == (tok = strtok(temp, "\n")))
                  {
                     buf = "";
                  }
                  else
                  {
                     buf.Format("%s", tok);
                     tok = strtok(NULL, "\n");

                     while (NULL != tok)
                     {
                        buf += tok;
                        tok = strtok(NULL, "\n");
                     }
                  }

                  free(temp);
               }
            }
            break;
         case valueTypeEmailAddress:
            buf = "";

            if (attrib.getStringValue().Left(7) != "mailto:")
               buf = "mailto:";
            if (attrib.getStringValueIndex() != -1)
               buf += attrib.getStringValue();
            break;
         case valueTypeWebAddress:
            buf = "";

            if (attrib.getStringValue().Left(7) != "http://")
               buf = "http://";
            if (attrib.getStringValueIndex() != -1)
               buf += attrib.getStringValue();
            break;
         }
         camcadAttribute->m_value = buf;
         camcadAttributeList.push_back(camcadAttribute);
      }
   }
   if(NULL != map)
   {
      delete map;
      map = NULL;
   }
   return camcadAttributeList;
}

OdbOutSettingsFile::OdbOutSettingsFile()
: m_compressOutput(true)
, m_useOutlineAutoJoin(true)
, m_useCadLayerNames(false)
{
}

void OdbOutSettingsFile::LoadSettingsFile(CString settingsFilePath)
{
   FILE *fp;
   char line[255];
   char* lp;

   SetCompressOutput(true);
   SetUseOutlineAutoJoin(true);
   this->m_bomAttrNames.RemoveAll();

   if ((fp = fopen(settingsFilePath, "rt")) == NULL)
   {
      // Failed to read the settings file            
      fprintf(flog,"Failed to read ODB++ settings file \"%s\"\n",settingsFilePath);
      return;
   }
              
   while (fgets(line, 255, fp))
   {
      if ((lp = get_string(line, " \t\n")) == NULL)
         continue;

      if (lp[0] == '.')
      {
         if (!STRICMP(lp, ".COMPRESS_OUTPUT"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'N')
					SetCompressOutput(false);
            else
               SetCompressOutput(true);
         } 
         else if (!STRICMP(lp, ".OUTLINE_AUTOJOIN"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

            if (toupper(lp[0]) == 'N')
					SetUseOutlineAutoJoin(false);
            else
               SetUseOutlineAutoJoin(true);
         }
         else if (!STRICMP(lp, ".USE_CAD_LAYER_NAMES"))
         {
            if ((lp = get_string(NULL, " ;\t\n")) == NULL)
               continue;

            bool yesno = (toupper(lp[0]) == 'Y');
            SetUseCadLayerNames( yesno );
         }
         else if (!STRICMP(lp, ".BOM_ATTRIB"))
         {
            // We want all the chars to semi-colon or end of line, do not break
            // fields on white space, like other settings usually do.
            // Strip off quotes if we find some.
            if ((lp = get_string(NULL, ";\n")) == NULL)
               continue;

            CString attribName(lp);
            attribName.Trim();    // Trim leading and trailing white space.
            attribName.Trim('"'); // Trim double quotes.

            if (!attribName.IsEmpty())
               AddBomAttribName(attribName);
         }
		}
	}
	fclose(fp);

   Validate();
}

void OdbOutSettingsFile::Validate()
{
   // Put messages in log for problems identified.

   // As of this writing (Feb 2012) ODB supports up to 10 DSC (Description) records per part.
   // We export seven of the standard CAMCAD BOM attribs in the first seven locations.
   // These are the same locations that vPlan exports these fields to ODB.
   // That leaves three available for user's random usage.
   // And that usage is through the .BOM_ATTRIB command.
   // We are supporting an unlimited list as far as parsing and general implementation, so
   // the number we export is easily increased.
   // But currently the output is limited to only the first three in the list,
   // because along with the other seven, that's the ODB limit.

   if (GetBomAttribNameCount() > 3)  // aka 7 + 3 > 10
   {
      fprintf(flog, "Too many .BOM_ATTRIB commands. Due to ODB++ format limitation only three attributes can be sent to BOM using these commands.\n");
      fprintf(flog, "These attributes will be output: ");
      for (int i = 0; (i < 3) && (i < GetBomAttribNameCount()); i++)
         fprintf(flog, "%s%s", (i==0)?"":", ", this->GetBomAttribNameAt(i));
      fprintf(flog, "\n");
      fprintf(flog, "These attributes will not be output: ");
      for (int i = 3; i < GetBomAttribNameCount(); i++)
         fprintf(flog, "%s%s", (i==3)?"":", ", this->GetBomAttribNameAt(i));
      fprintf(flog, "\n\n");
   }
}

//******************************************************************************

PolylistCompressor::PolylistCompressor(CCEtoODBDoc &doc, CPolyList &inputPolylist)
: m_doc(doc)
, m_inputPolylist(inputPolylist)
, m_variance(SMALLNUMBER)
{
}

//----------------------------------------------------------------

bool PolylistCompressor::PntsOverlap(CPnt *p1, CPnt *p2, double variance)
{ return ((fabs(p1->x - p2->x) < variance) && (fabs(p1->y - p2->y) < variance)); }

//----------------------------------------------------------------

int PolylistCompressor::CheckForBadVariance(CPoly *poly1, CPoly* poly2, double variance)
{
   CPnt *p1a, *p1b, *p2a, *p2b;
   p1a = poly1->getPntList().GetHead();
   p1b = poly1->getPntList().GetTail();
   p2a = poly2->getPntList().GetHead();
   p2b = poly2->getPntList().GetTail();
   if ((sqrt(pow((p1a->x - p1b->x), 2) + pow((p1a->y - p1b->y), 2)) < variance) ||
         (sqrt(pow((p2a->x - p2b->x), 2) + pow((p2a->y - p2b->y), 2)) < variance))
   {
      return 1;
   }
   return 0;
}

//----------------------------------------------------------------

bool PolylistCompressor::JoinPolys(CPolyList &resultPolylist)
{
   // We want to join all the polys that we can.
   // Best result is if we can make closed polys from the joined polys.

   // If there is only one poly in the list then there is nothing to join.
   if (m_inputPolylist.GetCount() < 2)
      return false;

   resultPolylist = m_inputPolylist;

   bool moreToDo = true;
   bool atleastOneClosedPoly = false;

   while (moreToDo)
   {
      int result = ProcessPolylist(resultPolylist);

      if (result == 2)
         atleastOneClosedPoly = true;

      moreToDo = (result != 0);
   }


   return atleastOneClosedPoly;
}

//----------------------------------------------------------------

JoinResult PolylistCompressor::ProcessPolylist(CPolyList &polyList)
{
   // Pick first non-closed poly as base.
   CPoly *basePoly = NULL;
   POSITION polyPos = polyList.GetHeadPosition();
   while (polyPos != NULL && basePoly == NULL)
   {
      CPoly *testPoly = polyList.GetNext(polyPos);
      if (!testPoly->isClosed())
         basePoly = testPoly;
   }

   if (basePoly != NULL)
   {
      bool connectToHead = true;
      while (true)
      {
         JoinResult result = TryToConnect(connectToHead, basePoly, polyList, m_variance);

         if (result == joinResultClosed) // If it is closed we are done with this one.
            return result;

         if (result == joinResultNothing) // couldn't connect anymore to this end
         {
            if (connectToHead)
               connectToHead = false;
            else
               break;
         }
      }

      // The only good result is a closed result. If we did not
      // get the closed result above (and we would not still be here if
      // we did) then nuke the basePoly.
      polyList.remove(basePoly);
      delete basePoly;
      return joinResultJoined;
   }

   return joinResultNothing;
}

//----------------------------------------------------------------

JoinResult PolylistCompressor::TryToConnect(bool connectToHead, CPoly *basePoly, CPolyList &candidatePolylist, double variance)
{
   CPoly *poly = basePoly;
   CPnt *pnt;

   if (connectToHead)
      pnt = poly->getPntList().GetHead();
   else 
      pnt = poly->getPntList().GetTail();

   CPoly *connectPoly = NULL;
   bool connectCandidateFromHead;

   POSITION polyPos = candidatePolylist.GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *testPoly = candidatePolylist.GetNext(polyPos);

      if (testPoly == poly)
         continue; // Skip self (self is basePoly).

      if (testPoly->isClosed())
         continue; // This one is already closed, try another.

      if (poly->getWidthIndex() != testPoly->getWidthIndex())
         continue;

      CPnt *testPnt = testPoly->getPntList().GetHead();
      if (PntsOverlap(pnt, testPnt, variance))
      {
         if (connectPoly)
         {
            int decimals = GetDecimals(OdbGlobalDoc->getSettings().getPageUnits());
            if (CheckForBadVariance(connectPoly, testPoly, variance))
            {
               //CString  tmp;
               //tmp.Format("It appears that the Auto Join Snap Size %1.*lf is too large.\nGo to System Settings to change",
               //   decimals, variance);
               // ErrorMessage(tmp, "Warning");
            }

            //CString buf;
            //buf.Format("Reached a fork at (%+.*lf, %+.*lf).\nUse manual join functions to continue.\n(If there doesn't seem to be a fork, there may be overlapping lines.)", 
            //      decimals, pnt->x, decimals, pnt->y);
            // ErrorMessage(buf, "Reached a Decision");
            return joinResultNothing; // decision
         }
         else
         {
            connectPoly = testPoly;
            connectCandidateFromHead = true;
         }
      }
      else
      {
         testPnt = testPoly->getPntList().GetTail();
         if (PntsOverlap(pnt, testPnt, variance))
         {
            if (connectPoly)
            {
               int decimals = GetDecimals(OdbGlobalDoc->getSettings().getPageUnits());
               if (CheckForBadVariance(connectPoly, testPoly, variance))
               {
                  //CString  tmp;
                  //tmp.Format("It appears that the Auto Join Snap Size %1.*lf is too large.\nGo to System Settings to change",
                  //   decimals, variance);
                  // ErrorMessage(tmp, "Warning");
               }
               //CString buf;
               //buf.Format("Reached a fork at (%+.*lf, %+.*lf).\nUse manual join functions to continue.\n(If there doesn't seem to be a fork, there may be overlapping lines.)", 
               //      decimals, pnt->x, decimals, pnt->y);
               // ErrorMessage(buf, "Reached a Decision");
               return joinResultNothing; // decision
            }
            else
            {
               connectPoly = testPoly;
               connectCandidateFromHead = false;
            }
         }
      }
   }

   if (!connectPoly)
      return joinResultNothing;

   // Connect polys

   // Absorb connectPoly into poly
   ConnectPolys(poly, connectToHead, connectPoly, connectCandidateFromHead);

   // The connectPoly has been consumed, take it out of candidate list.
   candidatePolylist.remove(connectPoly);  // Removes from list but does not delete.
   delete connectPoly; // So we'll delete it here.

   // Closed poly?
   if (TryToClosePoly(poly))
      return joinResultClosed;

   return joinResultJoined;
}

//----------------------------------------------------------------

void PolylistCompressor::ConnectPolys(CPoly *polyToKeep, bool connectToKeeperHead, CPoly *polyToConsume, bool connectConsumedHead)
{
   if (connectToKeeperHead)
   {
      if (connectConsumedHead)
         ReversePoly(&polyToConsume->getPntList());

      POSITION pos = polyToConsume->getPntList().GetTailPosition();
      if (pos)
         polyToConsume->getPntList().GetPrev(pos);
      while (pos)
      {
         CPnt *newPnt = new CPnt(*(polyToConsume->getPntList().GetPrev(pos)));
         polyToKeep->getPntList().AddHead(newPnt);
      }
   }
   else
   {
      if (!connectConsumedHead)
         ReversePoly(&polyToConsume->getPntList());

      polyToKeep->getPntList().GetTail()->bulge = polyToConsume->getPntList().GetHead()->bulge;

      POSITION pos = polyToConsume->getPntList().GetHeadPosition();
      if (pos)
         polyToConsume->getPntList().GetNext(pos);
      while (pos)
      {
         CPnt *newPnt = new CPnt(*(polyToConsume->getPntList().GetNext(pos)));
         polyToKeep->getPntList().AddTail(newPnt);
      }
   }
}

bool PolylistCompressor::TryToClosePoly(CPoly *poly)
{
   if (poly != NULL)
   {
      CPnt *headPnt = poly->getPntList().GetHead();
      CPnt *tailPnt = poly->getPntList().GetTail();
      if (PntsOverlap(headPnt, tailPnt, m_variance))
      {
         tailPnt->x = headPnt->x;
         tailPnt->y = headPnt->y;
         poly->setClosed(true);
         return true;  // Poly is closed.
      }
   }

   return false; // Poly is not closed.
}

