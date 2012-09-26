// $Header: /CAMCAD/5.0/Outline.cpp 38    5/21/07 7:52p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.
*/           

#include "stdafx.h"
#include <math.h>
#include "ccdoc.h"
#include "graph.h"
#include "bb_lib.h"
#include "outln_rb.h"
#include "polylib.h"
#include "pcbutil.h"
#include "outline.h"
#include "apertur2.h"
#include "ArcPoint.h"
#include "extents.h"

extern CDialogBar *CurrentSettings; // from MAINFRM.CPP

static CPolyList *partList = NULL;
static CCEtoODBDoc *doc;
static CPntList *result = NULL;
static BOOL AllowUnion;
static BOOL UseSinglePolyOutlineAsIs;

static BOOL CanUnion();
static DataStruct *GraphResultList(const char *layer);

/******************************************************************************
* Outline_Start
*/
void Outline_Start(CCEtoODBDoc *Doc, bool allowUnion, bool useSinglePolyOutlineAsIs)
{
   doc = Doc;
   AllowUnion = allowUnion;
   UseSinglePolyOutlineAsIs = useSinglePolyOutlineAsIs;
   partList = new CPolyList;
   result = NULL;
}

/******************************************************************************
* Outline_Add
*/
int Outline_Add(DataStruct *data, double scale, double insert_x, double insert_y, double rotation, BOOL mirror)
{
   if (data->getDataType() != T_POLY)
      return 0;
   
   POSITION polyPos = data->getPolyList()->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = data->getPolyList()->GetNext(polyPos);

      // DR 718797, Found when working on HKP export board outline issue.
      // Do not put void polys in the outline collection. This outline utility
      // creates outer boundary outline only. Voids ought to have had no
      // effect, but they do have an effect, entirely bad. Can cause outline
      // to fail. So just don't put them in in the first place.

      if (!poly->isVoid())
      {
         Outline_Add_Poly(poly, scale, insert_x, insert_y, rotation, mirror);
      }
   }

   return 1;
}

/******************************************************************************
* Outline_Add_Aperture
*/
int Outline_Add_Aperture(DataStruct *data, double scale, double insert_x, double insert_y, double rotation, BOOL mirror)
{
   if (data->getDataType() != T_INSERT)
      return 0;

   BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

   if (!(block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE))
      return 0;

   CPolyList* polyList = block->getAperturePolys(doc->getCamCadData(), true /*convert thick poly to outline*/);
   if (!polyList)
      return 0;

   POSITION polyPos = polyList->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = polyList->GetNext(polyPos);
      
      Outline_Add_Poly(poly, scale, insert_x, insert_y, rotation, mirror);
   }

   FreePolyList(polyList);

   return 1;
}

/******************************************************************************
* Outline_Add_Poly
*/
int Outline_Add_Poly(CPoly *poly, double scale, double insert_x, double insert_y, double rotation, BOOL mirror)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   if (poly->getPntList().GetCount() < 2)
      return 0;

   CPoly *newPoly = new CPoly;
   newPoly->setClosed(poly->isClosed());
   newPoly->setWidthIndex(poly->getWidthIndex());

   POSITION pntPos = poly->getPntList().GetHeadPosition();
   while (pntPos != NULL)
   {
      CPnt *pnt = poly->getPntList().GetNext(pntPos);

      Point2 p;

      p.x = pnt->x * scale;  
      p.y = pnt->y * scale; 
      p.bulge = pnt->bulge;
      if (mirror)
      {
         p.x = -p.x;
         p.bulge = -p.bulge;
      }
      TransPoint2 (&p, 1, &m, insert_x, insert_y);

      CPnt *newPnt = new CPnt;
      newPnt->x = (DbUnit)p.x;
      newPnt->y = (DbUnit)p.y;
      newPnt->bulge = (DbUnit)p.bulge;

      newPoly->getPntList().AddTail(newPnt);
   }

   partList->AddTail(newPoly);

   return 1;
}

/******************************************************************************
* Outline_Add_With_Width
*/
int Outline_Add_With_Width(DataStruct *data, double scale, double insert_x, double insert_y, double rotation, BOOL mirror)
{
   if (data->getDataType() != T_POLY)
      return 0;

   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION polyPos = data->getPolyList()->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = data->getPolyList()->GetNext(polyPos);

      if (!poly->getWidthIndex() || doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() < doc->getWidthTable()[0]->getSizeA() + SMALLNUMBER)
      {
         Outline_Add_Poly(poly, scale, insert_x, insert_y, rotation, mirror);
         continue;
      }

      Point2 p2;

      POSITION pntPos = poly->getPntList().GetHeadPosition();
      if (pntPos)
      {
         CPnt *pnt = poly->getPntList().GetNext(pntPos);

         p2.x = pnt->x * scale;  
         p2.y = pnt->y * scale; 
         p2.bulge = pnt->bulge;
         if (mirror)
         {
            p2.x = -p2.x;
            p2.bulge = -p2.bulge;
         }
         TransPoint2 (&p2, 1, &m, insert_x, insert_y);
      }

      while (pntPos)
      {
         CPnt *pnt = poly->getPntList().GetNext(pntPos);

         Point2 p1(p2);

         p2.x = pnt->x * scale;  
         p2.y = pnt->y * scale; 
         p2.bulge = pnt->bulge;
         if (mirror)
         {
            p2.x = -p2.x;
            p2.bulge = -p2.bulge;
         }
         TransPoint2 (&p2, 1, &m, insert_x, insert_y);

         CPoly *newPoly = GetSegmentOutline(p1.x, p1.y, p2.x, p2.y, p1.bulge, doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()/2*scale, 0);
         partList->AddTail(newPoly);
      }
   }

   return 1;
}


/******************************************************************************
* GetSegmentOutline
*/
CPoly *GetSegmentOutline(double ax, double ay, double bx, double by, double bulge, double widthRadius, int widthIndex)
{
   CPoly *poly = new CPoly;
   poly->setWidthIndex(widthIndex);
   poly->setClosed(true);
   poly->setFilled(false);

   if (bulge)
   {
      double da, sa, r, cx, cy;
      da = atan(bulge) * 4;
      ArcPoint2Angle(ax, ay, bx, by, da, &cx, &cy, &r, &sa);
      double ea = sa + da;

      if (da < 0)
      {
         double temp = sa;
         sa = ea;
         ea = temp;
         bulge = -bulge;
      }

      CPnt *pnt = new CPnt;
      pnt->x = (DbUnit)(cx + cos(sa) * (r + widthRadius));
      pnt->y = (DbUnit)(cy + sin(sa) * (r + widthRadius));
      pnt->bulge = (DbUnit)bulge;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(cx + cos(ea) * (r + widthRadius));
      pnt->y = (DbUnit)(cy + sin(ea) * (r + widthRadius));
      pnt->bulge = (DbUnit)1;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(cx + cos(ea) * (r - widthRadius));
      pnt->y = (DbUnit)(cy + sin(ea) * (r - widthRadius));
      pnt->bulge = (DbUnit)-bulge;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(cx + cos(sa) * (r - widthRadius));
      pnt->y = (DbUnit)(cy + sin(sa) * (r - widthRadius));
      pnt->bulge = (DbUnit)1;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(cx + cos(sa) * (r + widthRadius));
      pnt->y = (DbUnit)(cy + sin(sa) * (r + widthRadius));
      pnt->bulge = (DbUnit)0;
      poly->getPntList().AddTail(pnt);

      return poly;
   }

   // vertical
   else if (fabs(ax-bx) < 0.001)
   {
      CPnt *pnt = new CPnt;
      pnt->x = (DbUnit)(ax + widthRadius);
      pnt->y = (DbUnit)(ay);
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(bx + widthRadius);
      pnt->y = (DbUnit)by;
      pnt->bulge = 1;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(bx - widthRadius);
      pnt->y = (DbUnit)by;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(ax - widthRadius);
      pnt->y = (DbUnit)ay;
      pnt->bulge = 1;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(ax + widthRadius);
      pnt->y = (DbUnit)ay;
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   // horizontal
   else if (fabs(ay-by) < 0.001)
   {
      CPnt *pnt = new CPnt;
      pnt->x = (DbUnit)ax;
      pnt->y = (DbUnit)(ay - widthRadius);
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)bx;
      pnt->y = (DbUnit)(by - widthRadius);
      pnt->bulge = 1;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)bx;
      pnt->y = (DbUnit)(by + widthRadius);
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)ax;
      pnt->y = (DbUnit)(ay + widthRadius);
      pnt->bulge = 1;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)ax;
      pnt->y = (DbUnit)(ay - widthRadius);
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   else
   {
      double angle = ArcTan2(by-ay, bx-ax);

      CPnt *pnt = new CPnt;
      pnt->x = (DbUnit)(ax + sin(angle) * widthRadius);
      pnt->y = (DbUnit)(ay - cos(angle) * widthRadius);
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(bx + sin(angle) * widthRadius);
      pnt->y = (DbUnit)(by - cos(angle) * widthRadius);
      pnt->bulge = 1;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(bx - sin(angle) * widthRadius);
      pnt->y = (DbUnit)(by + cos(angle) * widthRadius);
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(ax - sin(angle) * widthRadius);
      pnt->y = (DbUnit)(ay + cos(angle) * widthRadius);
      pnt->bulge = 1.0;
      poly->getPntList().AddTail(pnt);

      pnt = new CPnt;
      pnt->x = (DbUnit)(ax + sin(angle) * widthRadius);
      pnt->y = (DbUnit)(ay - cos(angle) * widthRadius);
      pnt->bulge = 0;
      poly->getPntList().AddTail(pnt);
   }

   // fix bulges
   if (IsClockwise(&poly->getPntList()))
   {
      POSITION pos = poly->getPntList().GetHeadPosition();
      while (pos)
      {
         CPnt *pnt = poly->getPntList().GetNext(pos);
         pnt->bulge *= -1.0;
      }
   }

   return poly;
}

/******************************************************************************
* Outline_GetOutline 
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
CPntList *Outline_GetOutline(int *returnCode, double accuracy)
{
   int count = partList->GetCount();

   if (!count)
   {
      *returnCode = -1;

      delete partList;
      partList = NULL;

      return NULL;
   }

   if (count == 1)
   {
      CPoly *poly = partList->GetHead();

      if (poly->isClosed() && (UseSinglePolyOutlineAsIs || !PolySelfIntersects(&poly->getPntList())))
      {
         result = new CPntList;

         while (poly->getPntList().GetCount())
            result->AddTail(poly->getPntList().RemoveHead());

         delete partList;
         partList = NULL;

         *returnCode = 1;
         return result;
      }
      else if (poly->getPntList().GetCount() == 2)   // is a simple segment or arc
      {
         // if it has no width, you are out of luck
         if (doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() > SMALLNUMBER*2)
         {
            CPnt *head, *tail;
            head = poly->getPntList().GetHead();
            tail = poly->getPntList().GetTail();

            CPoly *newPoly = GetSegmentOutline(head->x, head->y, tail->x, tail->y, head->bulge, doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()/2, 0);

            result = new CPntList;
            while (newPoly->getPntList().GetCount())
               result->AddTail(newPoly->getPntList().RemoveHead());

            delete newPoly;
            delete partList;
            partList = NULL;

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
   
   if (AllowUnion && CanUnion())
      result = BuildUnion(partList);

   if (!result && CanRubberBand(partList, accuracy))
      result = RubberBand(partList, accuracy);

   FreePolyList(partList);

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

/******************************************************************************
* Outline_FreeResults 
*/
void Outline_FreeResults()
{
   if (result)
      FreePntList(result);
   result = NULL;
}


/******************************************************************************
* CanUnion()
*/
BOOL CanUnion()
{
   POSITION pos = partList->GetHeadPosition();
   while (pos != NULL)
   {
      CPoly *poly = partList->GetNext(pos);
      if (!poly->isClosed())
         return FALSE;

      if (PolySelfIntersects(&poly->getPntList()))
         return FALSE;
   }

   return TRUE;
}


/******************************************************************************
* OnOutlineSelected
*/
void CCEtoODBDoc::OnOutlineSelected() 
{

   if (ErrorMessage("1. Selected Elements are considered by the Outline.\n\
2. The Layer with the outline result is placed is the Current layer Selection.\n\
3. Delete selected Elements <Crtl D>.", "Generate Outline from Selected Elements", MB_OKCANCEL)!=IDOK)
      return;

   if (!SelectList.GetCount())
   {
      ErrorMessage("SelectList is Empty");
      return;
   }

   double accuracy = get_accuracy(this);
   int filenum = SelectList.GetHead()->filenum;
   POSITION pos = SelectList.GetHeadPosition();
   while (pos)
   {
      SelectStruct *s = SelectList.GetNext(pos);

      if (s->filenum != filenum)
      {
         ErrorMessage("SelectList has multiple files");
         return;
      }
   }

   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   if (CurrentLayer->GetCurSel() == CB_ERR)
   {
      ErrorMessage("No Current Layer selected");
      return;
   }

   int graphicClass = SelectList.GetHead()->getData()->getGraphicClass();
   pos = SelectList.GetHeadPosition();
   while (pos)
   {
      SelectStruct *s = SelectList.GetNext(pos);
      if (s->getData()->getGraphicClass() != graphicClass)
      {
         ErrorMessage("Outline's Graphic Class must be set manually", "SelectList has multiple Graphic Classes");
         graphicClass = 0;
         break;
      }
   }
   Outline_Start(this);

   pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = SelectList.GetNext(pos);
      Outline_Add(s->getData(), 1, 0, 0, 0, 0);
   }

   SelectStruct s(*(SelectList.GetHead()));
   //memcpy(&s, SelectList.GetHead(), sizeof(SelectStruct));

   int returnCode;
   if (Outline_GetOutline(&returnCode, accuracy))
   {
      PrepareAddEntity(Find_File(filenum));

      LayerStruct *layer = (LayerStruct*)CurrentLayer->GetItemDataPtr(CurrentLayer->GetCurSel());

      s.setData(GraphResultList(layer->getName()));

      if (s.getData() != NULL)
         InsertAddUndo(&s, this);

      s.getData()->setGraphicClass(graphicClass);
      Outline_FreeResults();
   }
}

void CCEtoODBDoc::OnConvertPolyToZeroWidth() 
{
   convertPolyToTerior(true);
}

void CCEtoODBDoc::OnConvertThickPolyToInteriorPoly() 
{
   convertPolyToTerior(false);
}

void CCEtoODBDoc::convertPolyToTerior(bool exteriorFlag) 
{

//   if (ErrorMessage(
//"1. Selected Elements are considered by the Outline.\n"
//"2. The Layer with the outline result is placed is the Current layer Selection.\n"
//"3. Delete selected Elements <Crtl D>.",
//"Generate zero width polys from Selected Elements",
//MB_OKCANCEL) != IDOK)
//      return;

   if (!SelectList.GetCount())
   {
      ErrorMessage("SelectList is Empty");
      return;
   }

   double accuracy = get_accuracy(this);
   int filenum = SelectList.GetHead()->filenum;
   
   for (POSITION pos = SelectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct *s = SelectList.GetNext(pos);

      if (s->filenum != filenum)
      {
         ErrorMessage("SelectList has multiple files");
         return;
      }
   }

   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);

   if (CurrentLayer->GetCurSel() == CB_ERR)
   {
      ErrorMessage("No Current Layer selected");
      return;
   }

   int graphicClass = SelectList.GetHead()->getData()->getGraphicClass();
   
   for (POSITION pos = SelectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* s = SelectList.GetNext(pos);

      if (s->getData()->getGraphicClass() != graphicClass)
      {
         ErrorMessage("Outline's Graphic Class must be set manually", "SelectList has multiple Graphic Classes");
         graphicClass = 0;
         break;
      }
   }

   CDataList redrawList(false);
   CDebugWriteFormat::setFilePath("c:\\DebugThickPolygon.txt");
   double tolerance = convertToPageUnits(pageUnitsInches,.00001);
   
   for (POSITION pos = SelectList.GetHeadPosition();pos != NULL;)
   {
      SelectStruct* s = SelectList.GetNext(pos);
      DataStruct* data = s->getData();

      if (data->getDataType() == dataTypePoly)
      {
         for (POSITION polyPos = data->getPolyList()->GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = data->getPolyList()->GetNext(polyPos);
            double width = getWidth(poly->getWidthIndex());

            if (width > 0.)
            {
               CPoly* thinPoly = new CPoly(*poly);

               if (exteriorFlag)
               {
                  thinPoly->convertToOutline(width,tolerance,this->getZeroWidthIndex());
               }
               else
               {
                  thinPoly->convertToInline(width,tolerance,this->getZeroWidthIndex());
               }

               thinPoly->setWidthIndex(getZeroWidthIndex());

               DataStruct* newData = getCamCadData().getNewDataStruct(*data);
               newData->getPolyList()->empty();
               newData->getPolyList()->AddTail(thinPoly);

               s->getParentDataList()->AddTail(newData);
               redrawList.AddTail(newData);
            }
         }
      }
   }

   //OnClearSelected();
   FileStruct* file = this->getFileList().FindByFileNumber(filenum);
   int fileMirror = ((getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored()) ? MIRROR_FLIP : 0);
   bool markFlag = true;

   if (file != NULL)
   {
      for (POSITION pos = redrawList.GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = redrawList.GetNext(pos);

	      SelectStruct* selectStruct = InsertInSelectList(data, file->getBlock()->getFileNumber(),
			      file->getInsertX(), file->getInsertY(),  file->getScale(), 
			      file->getRotation(), fileMirror, &(file->getBlock()->getDataList()),markFlag, TRUE);

	      DrawEntity(selectStruct, 0, FALSE);
      }
   }

   //OnClearSelected();
   CDebugWriteFormat::close();
}
  
/*************************************************************
* DEBUG
*/

/* OnEasyOutline_Debug() */
void CCEtoODBDoc::OnEasyOutline_Debug()
{
   double accuracy = get_accuracy(this);
   Outline_Start(this);

   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = SelectList.GetNext(pos);
      Outline_Add(s->getData(), s->scale, s->insert_x, s->insert_y, s->rotation, s->mirror);
//    Outline_Add_With_Width(s->p, s->scale, s->insert_x, s->insert_y, s->rotation, s->mirror);
   }

   int returnCode;
   if (Outline_GetOutline(&returnCode, accuracy))
   {
      FileStruct *file = Graph_File_Start("OUTLINE", Type_Unknown);
      Graph_File_Reference(file, 0.0, 0.0, 0.0, 0, 1.0);
      file->setNotPlacedYet(false);

      static int i = 1;
      CString layer;
      layer.Format("Outline%d", i++);

      GraphResultList(layer);
      Outline_FreeResults();
   }
}

/******************************************************************************
* GraphResultList() 
*
*  - must set file before calling
*/ 
static DataStruct *GraphResultList(const char *layer)
{
   if (!result)
      return NULL;

   doc->StoreDocForImporting();

   DataStruct *data = Graph_PolyStruct(Graph_Level(layer, NULL, 0), 0, 0);

   int err;

   int width = Graph_Aperture("OUTLINE", T_ROUND, 0.01, 0.0, 0.0, 0.0, 0.0, 0, 0L, 0, &err);
   CPoly *poly = Graph_Poly(NULL, width, 0, 0, 1); 

   POSITION pos = result->GetHeadPosition();
   while (pos != NULL)
   {
      CPnt *pnt = result->GetNext(pos);
      Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
   }

   doc->UpdateAllViews(NULL);

   return data;
}


//============================================================================

bool GetOutlineExtent(CCEtoODBDoc &doc, FileStruct *activeFile, CExtent &extent)
{
	ExtentRect outlineExtents;
   outlineExtents.left = outlineExtents.bottom = FLT_MAX;
   outlineExtents.right = outlineExtents.top = -FLT_MAX;

   Mat2x2 m;
   RotMat2(&m, 0);

   bool OutlineFound = false;

   if (activeFile != NULL)
   {
      POSITION dataPos = activeFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = activeFile->getBlock()->getDataList().GetNext(dataPos);

         if ((data->getDataType() == T_POLY) &&
            ((activeFile->getBlockType() == BLOCKTYPE_PANEL && data->getGraphicClass() == GR_CLASS_PANELOUTLINE) ||
            (activeFile->getBlockType() == BLOCKTYPE_PCB && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)))
         {
            OutlineFound = true;

            ExtentRect polyExtents;
            if (PolyExtents(&doc, data->getPolyList(), &polyExtents, 1, 0, 0, 0, &m, FALSE))
            {
               if (polyExtents.left < outlineExtents.left)
                  outlineExtents.left = polyExtents.left;
               if (polyExtents.right > outlineExtents.right)
                  outlineExtents.right = polyExtents.right;
               if (polyExtents.bottom < outlineExtents.bottom)
                  outlineExtents.bottom = polyExtents.bottom;
               if (polyExtents.top > outlineExtents.top)
                  outlineExtents.top = polyExtents.top;
            }
         }
      }
   }


   extent.set(outlineExtents.left, outlineExtents.bottom, outlineExtents.right, outlineExtents.top);

   if (!OutlineFound && activeFile != NULL)
   {
      activeFile->getBlock()->calculateVisibleBlockExtents(doc.getCamCadData());		
      extent = activeFile->getBlock()->getExtent();
   }

   return OutlineFound;
}


