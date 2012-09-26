// $Header: /CAMCAD/5.0/Search.cpp 95    6/21/07 8:27p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2004. All Rights Reserved.
*/
 
#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "ccview.h"
#include "attrib.h"
#include "extents.h"
#include "response.h"
#include "tmstate.h"
#include "xform.h"
#include "apertur2.h"
#include "SelectStackDialog.h"

extern CView *activeView; // from CCVIEW.CPP
extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern BOOL Editing;

CPoly *selPoly;

void ShowNothingDlg();              
bool FoundAPoly(CCEtoODBDoc *doc, SelectStruct *selectStruct, CPnt *Pnt, CPoly *&Poly, BOOL Before);
//BOOL CancelDraw();

struct DoubleRect
{ double left, right, top, bottom; };

static bool HitPolyList(double xLoc, double yLoc, CPolyList *polyList, CCEtoODBDoc *doc, DTransform xForm, Point2 &startSeg, Point2 &endSeg, CPnt *&startPoint);
static BOOL HitArc(Point2 *center, double radius, double sa, double da, double width, 
            double x, double y, double range);
// also used in other function like gerberpcb
BOOL HitLine(Point2 *start, Point2 *end, double width, double x, double y, double range);
static bool HitVisibleAttrib(CCEtoODBDoc *doc, double x, double y, double range, CAttributes* map, Point2 *pnt, 
      double block_scale, double block_rot, int block_mir);

//static double x, y, range;

class CSearchStatics
{
private:
   static double m_x;
   static double m_y;
   static double m_range;

public:
   static double getX() { return m_x; }
   static double getY() { return m_y; }
   static double getRange() { return m_range; }

   static void setX(double x) { m_x = x; }
   static void setY(double y) { m_y = y; }
   static void setRange(double range) { m_range = range; }
};

double CSearchStatics::m_x;
double CSearchStatics::m_y;
double CSearchStatics::m_range;

static int entityCount;
#define BREAK_SEARCH_RATE  40

/*****************************************************************************
* ClearSubSelectArray
*
* DESCRIPTION:
*  - clears SubSelectArray up to and including upThroughLevel
*  - to clean all, pass 0 for upThroughLevel
*/
void CCEtoODBDoc::ClearSubSelectArray(int upThroughLevel)
{
   //// start at end and clean up to upThroughLevel (upThroughLevel=1 -> 3..2..1 but leave 0 alone)
   //for (int i=getSelectStackTopIndex(); i >= upThroughLevel ; i--)
   //{
   //   delete getSelectStack()[i];
   //   getSelectStack()[i] = NULL;
   //}

   //setSelectStackTopIndex(upThroughLevel - 1);

   //setSelectStackLevelIndex(upThroughLevel);
   getSelectStack().popLevel(upThroughLevel);
}                       

/*****************************************************************************
* IsCoordInBlockExtents 
*/
bool IsCoordInBlockExtents(double x, double y, double range, 
      BlockStruct *block, double insert_x, double insert_y, double scale, double rotation, int mirror)
{
   if (block == NULL)
      return false;
   
   if (!block->extentIsValid())
      return true;

   CExtent extent;

   double xmin, xmax, ymin, ymax;
   xmin = ymin = FLT_MAX;
   xmax = ymax = -FLT_MAX;

   Mat2x2 m;
   RotMat2(&m, rotation);
   Point2 p;

   { // (xmin, ymin)
      p.x = block->getXmin() * scale;
      p.y = block->getYmin() * scale;
      if (mirror & MIRROR_FLIP)  p.x = -p.x;
      TransPoint2(&p, 1, &m, insert_x, insert_y);

      extent.update(p.x,p.y);
   }

   { // (xmin, ymax)
      p.x = block->getXmin() * scale;
      p.y = block->getYmax() * scale;
      if (mirror & MIRROR_FLIP)  p.x = -p.x;
      TransPoint2(&p, 1, &m, insert_x, insert_y);
   
      extent.update(p.x,p.y);
   }

   { // (xmax, ymax)
      p.x = block->getXmax() * scale;
      p.y = block->getYmax() * scale;
      if (mirror & MIRROR_FLIP)  p.x = -p.x;
      TransPoint2(&p, 1, &m, insert_x, insert_y);
   
      extent.update(p.x,p.y);
   }

   { // (xmax, ymin)
      p.x = block->getXmax() * scale;
      p.y = block->getYmin() * scale;
      if (mirror & MIRROR_FLIP)  p.x = -p.x;
      TransPoint2(&p, 1, &m, insert_x, insert_y);
   
      extent.update(p.x,p.y);
   }

   if (x - range > extent.getXmax())
      return false;

   if (x + range < extent.getXmin())
      return false;

   if (y - range > extent.getYmax())
      return false;

   if (y + range < extent.getYmin())
      return false;

   return true;
}

/*****************************************************************************
* Search */
bool CCEtoODBDoc::Search(double X, double Y, double Range,bool MarkLast,bool SearchAgain)
{
   bool retval = false;

   while (true)
   {
   #ifdef SHAREWARE
      ErrorMessage("If you had a C_QUERY or C_EDIT license, you could select, query and modify entities.", 
            "Shareware Version Only");
      break;
   #endif

      CWaitCursor wait;

      CSearchStatics::setX(X);
      CSearchStatics::setY(Y);
      CSearchStatics::setRange(Range);
      entityCount = 0;
      bool hasSelectedEntity = false;

      // possibly unselect last selected
      if (!SearchAgain && somethingIsSelected())
      {
         if (SelectList.IsEmpty())
         {
            ErrorMessage("Something is wrong");
         }
         else
         {
            SelectStruct *selectStruct = SelectList.GetHead();

            if (selectStruct->getData()->isSelected())
            {
               hasSelectedEntity = true;
               selectStruct->getData()->setSelected(false);
               SelectList.incrementModificationCount();

               if (selectStruct->getData()->isMarked() || MarkLast)  // if its marked, it stays; if not, its removed
               {
                  selectStruct->getData()->setMarked(true);
                  DrawEntity(selectStruct, 2, FALSE); // redraw as marked
               }
               else
               {
                  DrawEntity(selectStruct, 0, FALSE); // redraw last selected entity if not marked
                  SelectList.RemoveHead();
                  delete selectStruct;
               }
            }
         }
      }

      // Clear SelectedArray
      getSelectStack().empty();
      bool madeSelectionOrSearchCancelledFlag = false;
      
      for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL && !madeSelectionOrSearchCancelledFlag;)
      {
         FileStruct *file = getFileList().GetNext(filePos);

         if (!file->isShown() || file->isHidden())
            continue;

         if (file->getBlock() == NULL)
            continue;

         double fileX = file->getInsertX();
         double fileRot = file->getRotation();
         int fileMirror = (getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

         if (getBottomView())
         {
            fileX = -fileX;
            fileRot = -fileRot;
         }

         if (!IsCoordInBlockExtents(CSearchStatics::getX(),CSearchStatics::getY(),CSearchStatics::getRange(),
            file->getBlock(), fileX, file->getInsertY(), file->getScale(), fileRot, fileMirror))
         {
            continue;
         }

         bool filterChild = false;
         int result = SearchBlockLevel0(&file->getBlock()->getDataList(), file->getBlock()->getDataList().GetHeadPosition(), 
                        file->getBlock()->getFileNumber(),fileX, file->getInsertY(), file->getScale(), fileRot, fileMirror, -1, filterChild);

         if (result == 1)  // something found
         {
            // if search Filter_Pin, do not send the found (component) API, because
            // the api does not know that a pin filter is send afterwards.
            if (getSettings().FilterTypes == Filter_Pins)
            {
               getSelectStack().limitLevelIndex(1);
               //setSelectStackLevelIndex(1);

               // Case 2125 - An earlier change to "refactor" CSelectStack (see 4.5.1827) broke
               // the "pins filter" mode. The setSelectStackLevelIndex(1) call was replaced
               // by the .limitLevelIndex(1) call above it. The new call does not necessarily
               // set the current level index. Need to set index to "drilled down" one level.
               if (getSelectStack().getLevelIndex() < 1)
                  getSelectStack().incrementLevelIndex();
            }

            SelectEntity(SearchAgain);

            madeSelectionOrSearchCancelledFlag = true;
            retval = true;
         }
         else if (result == -1)  // search cancelled
         {
            getSelectStack().empty();

            madeSelectionOrSearchCancelledFlag = true;
            retval = false;
         }
      }

      if (!madeSelectionOrSearchCancelledFlag)
      {
         // nothing found or search cancelled
         if (Editing)
            ShowNothingDlg();

         ::SendMessage(StatusBar->m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)"Nothing found.");

         CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

         if (hasSelectedEntity)
            pApp->getSchematicLinkController().clearCrossProbedColors();

         SendResponse("", "");

         retval = false;
      }

      break;
   }

#ifdef ImplementSelectStackDialog
   CSelectStackDialog::update(this);
#endif

   return retval;
}

/*****************************************************************************
* SearchAgain */
void CCEtoODBDoc::SearchAgain()                  // Function key F3 uses this
{
   m_selectStack.assertValid();

   findNextEntityAtThisLevel();
   updateGerberEducator();
}

void CCEtoODBDoc::findNextEntityAtThisLevel()    // Function key F3 uses this
{
   while (true)
   {
      CWaitCursor wait;

      // if nothing is selected, F3 does not make sense
      if (nothingIsSelected())
         break;

      entityCount = 0;
      bool filterChild = (getSettings().FilterTypes == Filter_Pins && getSelectStack().getLevelIndex() > 1);

      // if top level
      if (getSelectStack().getLevelIndex() == 0)
      {
         // unselect last selected
         SelectStruct *selectStruct = SelectList.GetHead();

         if (selectStruct->getData()->isSelected() != 0)
         {
            selectStruct->getData()->setSelected(false);
            SelectList.incrementModificationCount();
         }

         if (selectStruct->getData()->isMarked())  // if its marked, it stays; if not, its removed
         {
            DrawEntity(selectStruct, 2, FALSE); // redraw as marked
         }
         else
         {
            DrawEntity(selectStruct, 0, FALSE); // redraw last selected entity if not marked
            SelectList.RemoveHead();

            delete selectStruct;
         }

         // remember what we left off on
         SelectStruct temp(*(getSelectStack().getAt(0)));
         getSelectStack().empty();
         
         // loop files
         POSITION dataPos;
         FileStruct *fp;
         BOOL FoundFile = FALSE;
         bool foundOrCancelledFlag = false;
         
         for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
         {
            fp = getFileList().GetNext(filePos);
      
            if (!fp->isShown() || fp->isHidden())
               continue;
      
            if (fp->getBlock() == NULL)
               continue;
      
            // Start with the file we left off at, or accept if it is the temp edit file
            if (!FoundFile)
            {
               // Test by filenum does not work anymore, not since move to DCA (circa Nov 2008) because we can't
               // alter filenum willy nilly anymore.
               // if (fp->getFileNumber() != -1 && fp->getFileNumber() != temp.filenum) // -1 filenum for block-edit file
               // Use specific flag to determine if this is the temp edit file.
               if (!fp->IsTempGeomEditFile() && fp->getFileNumber() != temp.filenum)
                  continue;
      
               // find start position
               dataPos = fp->getBlock()->getDataList().Find(temp.getData());
               fp->getBlock()->getDataList().GetNext(dataPos);
            }
            else
            {
               dataPos = fp->getBlock()->getDataList().GetHeadPosition();
            }

            FoundFile = TRUE;
      
            int result = SearchBlockLevel0(&(fp->getBlock()->getDataList()), dataPos,fp->getBlock()->getFileNumber(),
                                       fp->getInsertX(), fp->getInsertY(), fp->getScale(), fp->getRotation(), 
                                       (getBottomView() ? MIRROR_FLIP : 0) ^ (fp->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0),-1,
                                       filterChild);

            if (result == 1)
            {  // found something
               if (getSettings().FilterTypes == Filter_Pins)
               {
                  getSelectStack().limitLevelIndex(1);
               }

               SelectEntity(TRUE);
               foundOrCancelledFlag = true;

               break;
            }
            else if (result == -1)
            {  // search cancelled
               getSelectStack().empty();
               foundOrCancelledFlag = true;
               break;
            }
         }

         if (foundOrCancelledFlag)
         {
            break;
         }

         // loop back and continue searching 
         Search(CSearchStatics::getX(),CSearchStatics::getY(),CSearchStatics::getRange(),false,true); 
      }   
      else  // Find next entity at this level
      {
         // find file we left off at
         FileStruct *fp;
         
         for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
         {
            fp = getFileList().GetNext(filePos);

            if (fp->getFileNumber() == getSelectStack().getAt(0)->filenum)
            {
               break;
            }
         }

         // discard selections above this one on the stack
         getSelectStack().popAboveLevelIndex();

         //int level = getSelectStackLevelIndex();
         //// clear sub levels of the entity at this level
         //ClearSubSelectArray(getSelectStackLevelIndex() + 1);
         //setSelectStackLevelIndex(level);

         // remember the SelectStruct of the item at this level
         SelectStruct *selectStruct = getSelectStack().getAtLevel();

         // find start pos
         POSITION startPos = selectStruct->getParentDataList()->Find(selectStruct->getData());
         POSITION headPos  = selectStruct->getParentDataList()->GetHeadPosition();
         POSITION dataPos;

         for (int pass = 0;pass < 2;pass++)
         {
            if (pass == 0)
            {
               dataPos = startPos;
               selectStruct->getParentDataList()->GetNext(dataPos);
            }
            else
            {
               dataPos = headPos;
            }

            CSelectStructListContainer selectStructList;

            int result = SearchBlock(selectStructList,selectStruct->getParentDataList(), dataPos, getSelectStack().getLevelIndex() == 0, 
                                     selectStruct->filenum,selectStruct->insert_x, selectStruct->insert_y, selectStruct->scale,
                                     selectStruct->rotation, selectStruct->mirror, selectStruct->layer, filterChild);

            if (result == -1)
            {
               getSelectStack().empty();

               break;
            }
            else if (result == 1)
            {
               getSelectStack().decrementLevelIndex();
               getSelectStack().pop();
               getSelectStack().push(selectStructList);
               getSelectStack().incrementLevelIndex();

               break;
            }
         }
         
         SelectEntity(TRUE);

         //CSelectStructListContainer selectStructList;

         //int result = SearchBlock(selectStructList,selectStruct->getParentDataList(), dataPos, getSelectStack().getLevelIndex() == 0, selectStruct->filenum,
         //      selectStruct->insert_x, selectStruct->insert_y, selectStruct->scale, selectStruct->rotation, selectStruct->mirror, selectStruct->layer,
         //      NULL, 0);

         //if (result == -1)
         //{
         //   getSelectStack().empty();

         //   break;
         //}
         //else if (result == 0)
         //{
         //   selectStructList.empty();

         //   if (SearchBlock(selectStructList,selectStruct->getParentDataList(), selectStruct->getParentDataList()->GetHeadPosition(), 
         //                   getSelectStack().getLevelIndex() == 0, selectStruct->filenum,
         //                   selectStruct->insert_x, selectStruct->insert_y, selectStruct->scale,
         //                   selectStruct->rotation, selectStruct->mirror, selectStruct->layer,NULL, 0) == 1)
         //   {
         //      getSelectStack().decrementLevelIndex();
         //      getSelectStack().pop();
         //      getSelectStack().pushOnTop(selectStructList);
         //      getSelectStack().incrementLevelIndex();

         //      SelectEntity(TRUE);

         //      break;
         //   }
         //   else
         //   {
         //      SelectEntity(TRUE);
         //   }
         //}
         //else if (result == 1)
         //{
         //   getSelectStack().decrementLevelIndex();
         //   getSelectStack().pop();
         //   getSelectStack().pushOnTop(selectStructList);
         //   getSelectStack().incrementLevelIndex();

         //   SelectEntity(TRUE);

         //   break;
         //}

         // free the overwritten level that we remembered
         //delete selectStruct; // knv 20060616.1321 - this statement caused a bug by deleting a SelectStruct contained in m_selectStack

         //SelectEntity(TRUE);
      }

      break;
   }

#ifdef ImplementSelectStackDialog
   CSelectStackDialog::update(this);
#endif
}

/*****************************************************************************
* SearchBlock 
*
* PARAMETERS:
*
*   return value:
*   -1 if the search was cancelled
*   TRUE if a selection was made
*   FALSE if no selection was made
*/
int CCEtoODBDoc::SearchBlock(CSelectStructListContainer& selectStructList,CDataList* DataList, POSITION pos, const bool levelZeroFlag, const int filenum,
      const double insert_x, const double insert_y, const double scale, const double rotation, const int mirror, const int insertLayerIndex,
      bool parentFilterChild)
{
   static int callLevel = 0;

   callLevel++;

   int debugCallLevel = 0;

   switch (callLevel)
   {
   case 1:  debugCallLevel = callLevel;  break;
   case 2:  debugCallLevel = callLevel;  break;
   case 3:  debugCallLevel = callLevel;  break;
   case 4:  debugCallLevel = callLevel;  break;
   case 5:  debugCallLevel = callLevel;  break;
   case 6:  debugCallLevel = callLevel;  break;
   case 7:  debugCallLevel = callLevel;  break;
   case 8:  debugCallLevel = callLevel;  break;
   }

   int retval = FALSE;

   selPoly = NULL;

   bool localFilterChild = parentFilterChild;

   Mat2x2 rotationMat;
   RotMat2(&rotationMat, rotation);

   bool mirroredFlag = ((mirror & MIRROR_LAYERS) != 0);

   // loop entities at this level
   while (pos != NULL)
   {
      DataStruct *data = DataList->GetNext(pos);

      if (data->getEntityNumber() == 291)
      {
         int iii = 3;
      }

      if (entityCount++ > BREAK_SEARCH_RATE) // How often to flush message queue to check for ESCAPE
      {
         entityCount = 0;

         /*if (CancelDraw())
         {
            retval = -1;
            break;
         }*/
      }

      if (data->isHidden())
      {
         continue;
      }

      // -- Blob -- Blob -- Blob -- Blob -- Blob -- Blob -- Blob -- Blob -- Blob -- Blob -- Blob -- Blob
      // Is there a reason for placing the blob selection before the layer filtering? - knv 20050722
      if (data->getDataType() == dataTypeBlob)
      {
         Point2 point2;
         point2.x = data->getBlob()->pnt.x * scale;
         point2.y = data->getBlob()->pnt.y * scale;

         if (mirror & MIRROR_FLIP)  point2.x = -point2.x;

         TransPoint2(&point2, 1, &rotationMat, insert_x, insert_y);

         if (point2.x < CSearchStatics::getX() && point2.x + data->getBlob()->width  * scale > CSearchStatics::getX() && 
             point2.y < CSearchStatics::getY() && point2.y + data->getBlob()->height * scale > CSearchStatics::getY())
         {
            selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);

            retval = TRUE;
            break;
         }

         continue;
      }

      // figure LAYER
      int layerIndex = data->getLayerIndex();
      if(data->HasLayerSpan())
      {
         int spanlayer = getLayerArray().FindFirstVisLayerIndxInStackupRange(data->GetPhysicalStartLayer(), data->GetPhysicalEndLayer(), mirroredFlag);
         if(spanlayer > -1)
         {
            layerIndex = spanlayer;
         }
      }
      if (IsFloatingLayer(layerIndex) && insertLayerIndex != -1)
      {
         layerIndex = insertLayerIndex;
      }

      // -- insert -- insert -- insert -- insert -- insert -- insert -- insert -- insert -- insert -- insert -- insert
      if (data->getDataType() == dataTypeInsert)
      {
         if (!getInsertTypeFilter().contains(data->getInsert()->getInsertType()))
         {
            continue;
         }
      }
      else
      {  
         if (data->getGraphicClass() >= 0 && !getGraphicsClassFilter().contains(data->getGraphicClass())) // DFM Debugging... GC < 0
         {
            continue;
         }

         if (!getSelectLayerFilterStack().contains(getMirroredLayerIndex(layerIndex,mirroredFlag)))
         {
            continue;
         }
      }

      BlockStruct *subblock = NULL;

      if (data->getDataType() == dataTypeInsert)
      {
         subblock = Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (subblock == NULL)
         {
            continue;   // block deleted ???
         }
      }

      // Check Layer Visible
      if ( 
         (data->getDataType() != dataTypeInsert) || 
         (subblock != NULL && subblock->isTool() && !subblock->getToolDisplay()) || 
         (subblock != NULL && subblock->isSimpleAperture())
         )
      {
         // only check layer for non insert entities
         if (!get_layer_editable(layerIndex, (GeometryEditing && levelZeroFlag ? -1 : mirror)))
         {
            continue;
         }

         if (!getSelectLayerFilterStack().contains(getMirroredLayerIndex(layerIndex,mirroredFlag)))
         {
            continue;
         }
      }
      else if (subblock != NULL)
      {
         // adjust for LAYER = -1 
         // only for Block Apertures and Block Tools when insertLayerIndex = -1
         if (layerIndex == -1 && (subblock->isAperture() || subblock->isTool()))
         {
            if (!subblock->getDataList().IsEmpty() )
            {
               layerIndex = subblock->getDataList().GetHead()->getLayerIndex();

               if (!get_layer_editable(layerIndex, (GeometryEditing && levelZeroFlag ? -1 : mirror)))
               {
                  continue;
               }

               if (!getSelectLayerFilterStack().contains(getMirroredLayerIndex(data->getLayerIndex(),mirroredFlag)))
               {
                  continue;
               }
            }
         }
      }

      bool processedFlag = false;
   
      switch (data->getDataType()) 
      {
      case dataTypePoint:
         {
            if (getSettings().FilterTypes != Filter_All && !localFilterChild)
               continue;

            Point2 point;
            point.x = data->getPoint()->x * scale;
            point.y = data->getPoint()->y * scale;

            if (mirror & MIRROR_FLIP) point.x = -point.x;

            TransPoint2(&point, 1, &rotationMat, insert_x, insert_y);

            if (sqrt(fabs(CSearchStatics::getX() - point.x) * fabs(CSearchStatics::getX() - point.x) + 
                     fabs(CSearchStatics::getY() - point.y) * fabs(CSearchStatics::getY() - point.y)) < CSearchStatics::getRange())
            {
               selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);

               retval = TRUE;
               processedFlag = true;
               break;
            }
         }

         break;

      case dataTypePoly:
         {
            if (getSettings().FilterTypes != Filter_All && !localFilterChild)
               continue;

            Point2 startSeg, endSeg;
            CPnt *pnt = NULL;

            if (HitPolyList(CSearchStatics::getX(),CSearchStatics::getY(), data->getPolyList(),
                            this, DTransform(insert_x, insert_y, scale, rotation, mirror), startSeg, endSeg, pnt))
            {
               // Case dts0100377851
               // It would appear this is another defect caused by the refactoring of CSelectStack.
               // See case 2125, circa rev 4.5.1827. I will not bother to prove such, but it seems likely.
               // We are not getting values of 0 through here in tests I've tried, namely go into
               // poly edit mod and try to select a poly. We are getting -1. It is not clear
               // if both 0 and -1 should be allowed, or if only -1. We'll allow both, until
               // some evidence is encountered that shows we shouldn't.
               // OLD CODE: bool newSelectionFlag = (getSelectStack().getLevelIndex() == 0);
               bool newSelectionFlag = (getSelectStack().getLevelIndex() < 1);

               SelectStruct *selectStruct = selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);

               if (newSelectionFlag)
               {
                  BOOL Before = sqrt(pow(CSearchStatics::getX() - endSeg.x  ,2) + pow(CSearchStatics::getY() - endSeg.y  ,2)) < 
                                sqrt(pow(CSearchStatics::getX() - startSeg.x,2) + pow(CSearchStatics::getY() - startSeg.y,2));

                  bool lastOneDeleted = FoundAPoly(this, selectStruct, pnt, selPoly, Before);
                  if(lastOneDeleted) //if no polys in selectStruct, remove selectStruct
                     selectStruct = selectStructList.takeHead();
               }

               retval = TRUE;
               processedFlag = true;
               break;
            }
         }

         break;

      case dataTypeText:
         {
            if (getSettings().FilterTypes != Filter_All && !localFilterChild)
               continue;

            if (SearchTextData(selectStructList,DataList, data, filenum, layerIndex, insert_x, insert_y, scale, rotation, mirror))
            {
               retval = TRUE;
               processedFlag = true;
               break;
            }
         }

         break;

      case dataTypeInsert:
         {
            InsertTypeTag thisInsertType = data->getInsert()->getInsertType();

            if (!localFilterChild &&
                  getSettings().FilterTypes == Filter_Pins && 
                  data->getInsert()->getInsertType() != insertTypePin && 
                  data->getInsert()->getInsertType() != insertTypePcbComponent)
            {
               continue;
            }

            if (getSettings().FilterTypes == Filter_Pins && 
                data->getInsert()->getInsertType() == insertTypePin)
            {
               localFilterChild = true;
            }

            // calculate point
            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * scale;
            point2.y = data->getInsert()->getOriginY() * scale;

            if (mirror & MIRROR_FLIP)  point2.x = -point2.x;

            TransPoint2(&point2, 1, &rotationMat, insert_x, insert_y);

            bool DrawAp       = false;
            bool DrawSubblock = false;
            bool IsTool       = (subblock->getFlags() & BL_TOOL    ) || (subblock->getFlags() & BL_BLOCK_TOOL    );
            bool IsAp         = (subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE);

            double _x     = point2.x;
            double _y     = point2.y;
            double _rot   = rotation;
            double _scale = scale;

            int _mirror   = mirror;
            int _layer    = layerIndex;

            //if (recurslevel == 0)
            //   rootdata = data;
   
            // TOOL
            if (IsTool)
            {
               if (subblock->getToolDisplay()) // use another block's graphics to display this tool
               {
                  subblock = Find_Block_by_Num(subblock->getToolBlockNumber());
   
                  if (subblock != NULL)
                  {
                     if ((subblock->getFlags() & BL_APERTURE) || (subblock->getFlags() & BL_BLOCK_APERTURE))
                     {
                        DrawAp = true;
                     }
                     else
                     {
                        DrawSubblock = true;
                     }
                  }
               }
               else // draw generic tool
               {
                  double toolradius = 0.0;

                  if (subblock->getToolSize() > SMALLNUMBER)
                  {
                     toolradius = subblock->getToolSize() / 2;
                  }

                  double xmin = point2.x - toolradius;
                  double xmax = point2.x + toolradius;
                  double ymin = point2.y - toolradius;
                  double ymax = point2.y + toolradius;

                  if (CSearchStatics::getX() > xmin - CSearchStatics::getRange() && CSearchStatics::getX() < xmax + CSearchStatics::getRange() && 
                      CSearchStatics::getY() > ymin - CSearchStatics::getRange() && CSearchStatics::getY() < ymax + CSearchStatics::getRange())
                  {
                     if (fabs(CSearchStatics::getX() - point2.x) < CSearchStatics::getRange() + toolradius && 
                         fabs(CSearchStatics::getY() - point2.y) < CSearchStatics::getRange() + toolradius    )
                     {
                        selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);

                        retval = TRUE;
                        processedFlag = true;
                        break;
                     }
                  }

                  break;
               }
            } // end TOOL

            // APERTURE
            if (IsAp || DrawAp)
            {
               // COMPLEX APERTURE
               if (subblock->getShape() == T_COMPLEX)
               {
                  if (mirror & MIRROR_FLIP)
                  {
                     _rot = rotation - subblock->getRotation() - data->getInsert()->getAngle();
                  }
                  else
                  {
                     _rot = rotation + subblock->getRotation() + data->getInsert()->getAngle();
                  }

                  subblock = Find_Block_by_Num((int)(subblock->getSizeA()));
                  DrawSubblock = TRUE;
               }
               else // NORMAL APERTURE
               {
                  Point2 startSeg, endSeg;
                  CPnt *startPoint = NULL;
                  
                  bool hitAperture = false;

                  CTMatrix ctMat;
                  ctMat.rotateRadians(rotation);
                  ctMat.translate(insert_x, insert_y);

                  /// Mirroring here fixes selection of an Aperture in a file that
                  /// had been mirrored (tools | Files | Modify/Mirror) but has not
                  /// had "restructure around origin" run. But it breaks selection for
                  /// more common situations, like a top side view with ALL Layers On.
                  /// In that case, one can not select items with apertures on the
                  /// bottom/mirrored side anymore. Not sure what to do to fix it so
                  /// both cases work. But what is clear is that this by itself won't
                  /// do.

                  //if (rootdata && rootdata->getInsert()->getInsertType() == insertTypeUnknown)
                  // This works okay so far, but seem far from ideal
                  //if (recurslevel == 0)
                  //{
                  //   ctMat.mirrorAboutYAxis(mirror & MIRROR_FLIP);
                  //}

                  CPolyList *polyList = ApertureToPoly(subblock, data->getInsert(), 0);

                  bool blExValid = subblock->extentIsValid();  

                  if (!blExValid)
                  {
                     this->CalcBlockExtents(subblock);
                  }

                  if (polyList != NULL)
                  {
                     polyList->transform(ctMat);
                     CExtent extent = polyList->getExtent();

                     // if extents of aperture intersect extents of search range
                     if (extent.intersects(CExtent(CPoint2d(CSearchStatics::getX(),CSearchStatics::getY()),CSearchStatics::getRange())))
                     {
                        hitAperture = polyList->isPointInside(CPoint2d(CSearchStatics::getX(),CSearchStatics::getY()),getPageUnits()); // if point is inside poly

                        // if point is outside poly but search range touches
                        if (!hitAperture)
                           hitAperture = (CSearchStatics::getRange() > polyList->distanceTo(CPoint2d(CSearchStatics::getX(),CSearchStatics::getY())));
                     }

                     delete polyList;
                  }

                  if (hitAperture)
                  {
                     // knv - Scenario - a pin on a pcb component is selected and f4 was pressed until the stack level is at the top of the stack.
                     //       When SearchAgain() is called, findNextEntityAtThisLevel() calls SearchBlock() which executes the line below,
                     //       which then pushes the found selection onto the bottom of the stack instead of popping the top of the stack
                     //       and then pushing this selection onto the top of the stack
                     selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);

                     retval = TRUE;
                     processedFlag = true;
                     break;
                  }
               } // end NORMAL APERTURE
            } // end APERTURE
   
            // GRAPHIC BLOCK
            if (!IsAp && !IsTool) 
            {
               DrawSubblock = TRUE;
   
               if (mirror & MIRROR_FLIP)
               {
                  _rot = rotation - data->getInsert()->getAngle();
               }
               else
               {
                  _rot = rotation + data->getInsert()->getAngle();
               }
   
               _mirror = mirror ^ data->getInsert()->getMirrorFlags();
               _scale = scale * data->getInsert()->getScale();
            }   
   
            // Call Recursion
            if (DrawSubblock && subblock != NULL)
            {
               if (IsCoordInBlockExtents(CSearchStatics::getX(),CSearchStatics::getY(),CSearchStatics::getRange(),
                                         subblock, point2.x, point2.y, _scale, _rot, _mirror))
               {
                  int result = SearchBlock(selectStructList,&(subblock->getDataList()), subblock->getDataList().GetHeadPosition(), false, subblock->getFileNumber(),
                        point2.x, point2.y, _scale, _rot, _mirror, layerIndex, localFilterChild);

                  if (result == 1)
                  {
                     selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);

                     retval = TRUE;
                     processedFlag = true;
                     break;
                  }
                  else if (result == -1)
                  {
                     retval = -1;
                     processedFlag = true;
                     break;
                  }
               }
            } // end Draw Block

            localFilterChild = parentFilterChild;

            // check visible attribs
            if (getSettings().FilterTypes == Filter_All && getShowAttribs() && CFontList::fontIsLoaded() && 
                data->getAttributesRef() && !data->getAttributesRef()->IsEmpty())
            {
               if (HitVisibleAttrib(this, CSearchStatics::getX(),CSearchStatics::getY(),CSearchStatics::getRange(),
                                    data->getAttributesRef(), &point2, _scale, _rot, _mirror))
               {
                  selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);

                  retval = TRUE;
                  processedFlag = true;
                  break;
               }
            }
         }

         break;
      } // end switch type

      if (processedFlag)
      {
         break;
      }
   
      selPoly = NULL;
      retval = FALSE;
   } // end loop datas

   callLevel--;

   return retval;
} // end SearchBlock()

int CCEtoODBDoc::SearchBlockLevel0(CDataList* DataList, POSITION pos, const int filenum,
      const double insert_x, const double insert_y, const double scale, const double rotation, 
      const int mirror, const int insertLayerIndex, bool filterChild)
{
   getSelectStack().empty();

   CSelectStructListContainer selectStructList;

   int retval = SearchBlock(selectStructList,DataList,pos,true,filenum,insert_x,insert_y,scale,rotation,mirror,insertLayerIndex, filterChild);

   getSelectStack().push(selectStructList);

   if (getSelectStack().getSize() > 0)
   {
      SelectStruct* selectStruct = getSelectStack().getAt(0);
      InsertInSelectList(*selectStruct,false,false,false);
   }

   return retval;
}


/******************************************************************************
* SearchTextData
*/
bool CCEtoODBDoc::SearchTextData(CSelectStructListContainer& selectStructList,CDataList *DataList, DataStruct *data, int filenum, int layer,
      double insert_x, double insert_y, double scale, double rotation, int mirror)
{
   if (data->getDataType() != T_TEXT)
      return false;

   bool searchResult = SearchText(data->getText()->getText(), data->getText()->getPenWidthIndex(), insert_x, insert_y, scale, rotation, mirror,
         data->getText()->getPnt().x, data->getText()->getPnt().y, data->getText()->isMirrored()?TRUE:FALSE, data->getText()->getRotation(),
         data->getText()->getFontNumber(), data->getText()->getWidth(), data->getText()->getHeight(), data->getText()->isProportionallySpaced(),
         data->getText()->getHorizontalPosition(), data->getText()->getVerticalPosition());

   if (searchResult)
   {
      selectStructList.addHead(DataList,data,filenum,insert_x,insert_y,scale,rotation,mirror);
   }

   return searchResult;
}

/******************************************************************************
* SearchText
*/
bool CCEtoODBDoc::SearchText(const char *text, int penWidthIndex, double insert_x, double insert_y,
                           double scale, double rotation, int mirror,
                           double text_x, double text_y, int text_mirror, double text_rotation, short text_fontnum,
                           double width, double height, BOOL proportional,
                           int textAlignment, int lineAlignment)
{
   const double spaceratio = 0.01 * getSettings().TextSpaceRatio; // turn percent into a decimal
   FontStruct* fs = CFontList::getFontList().getFont(text_fontnum);
   double penWidth = getWidthTable()[penWidthIndex]->getSizeA();

   int charCount = strlen(text);
   char *newLine = strchr((char*)text, '\n');
   if (newLine)
      charCount = newLine - text;

   TextStruct textStruct(text,proportional != 0);
   textStruct.setWidth(width);

   // Calculate the width of the string taking into account the proportional flag
   double stringWidth = textStruct.getMaxLineLength(spaceratio, penWidth);

   double x_offset, y_offset;
   switch (textAlignment)
   {
   case horizontalPositionCenter:
      if (text_mirror)
         x_offset = stringWidth / 2;
      else
         x_offset = -stringWidth / 2;
      break;
   case horizontalPositionRight:
      if (text_mirror)
         x_offset = stringWidth;
      else
         x_offset = -stringWidth;
      break;
   case horizontalPositionLeft:
   default:
      x_offset = 0;
      break;
   }

   int lineCount = 1;
   for (unsigned int i = 0; i < strlen(text); i++)
   {
      if (text[i] == '\n')
         lineCount++;
   }

   switch (lineAlignment)
   {
   case verticalPositionCenter:
      y_offset = -(height / 2) + (height * (1.0 + spaceratio) * (lineCount - 1) / 2);
      break;
   case verticalPositionTop:
      y_offset = -height;
      break;
   case verticalPositionBottom:
      y_offset = height * (1.0 + spaceratio) * (lineCount - 1);
      break;
   case verticalPositionBaseline:
   default:
      y_offset = 0.0;
      break;
   }

   // DO NOT REMOVE:  This is used for debugging purposes
   //    Draws the extents of the character used to determine if it is in the search range
   //   so we can see what is being calculated.
   //CCEtoODBView *view = NULL;
   //POSITION pos = GetFirstViewPosition();
   //if (pos != NULL)
   //   view = (CCEtoODBView *)GetNextView(pos);
   //CClientDC dc(view);
   //view->OnPrepareDC(&dc);
   //CGdiObject *oldPen = dc.SelectStockObject(WHITE_PEN);
   //CGdiObject *oldBrush = dc.SelectStockObject(HOLLOW_BRUSH);

   for (unsigned int i = 0; i < strlen(text); i++)
   {
      unsigned char c = text[i];

      if (c == '\n')
      {
         y_offset -= height * (1.0 + spaceratio);

         char *newLine = strchr((char*)(text + i + 1), '\n');
         if (newLine)
            charCount = newLine - (text + i) - 1;
         else 
            charCount = strlen(text) - i - 1;
         double curStringWidth = (1.0 + spaceratio) * charCount * width;
         switch (textAlignment)
         {
         case 0: // left align
         default:
            x_offset = 0;
            break;
         case 1: // center align
            x_offset = -curStringWidth / 2;
            break;
         case 2: // right align
            x_offset = -curStringWidth;
            break;
         }
         continue;
      }

      if (c == 13)
         continue;

      int textMode = 0;

      if (c == '%')
      {
         ++i;
         c = text[i];
         if (c == 'O')
         {
            textMode |= TEXT_OVERLINE; // next char
            continue;   // fetch next char
         }
         else if (c == 'U')
         {
            textMode |= TEXT_UNDERLINE;   // next char
            continue;   // fetch next char
         }
         else
         {
            // this is nothing
         }        
      }
      if (c == ' ')
      {
         if (proportional)
            x_offset += 2.0 * spaceratio * width; 
         else
            x_offset += width * (1.0 + spaceratio); 
         textMode = 0;  
         continue;
      }

      // calc bar if needed
      double barLength;
      if (proportional && fs)
         barLength = (fs->getCharacterWidth(c) * 2.0 + spaceratio) * width;
      else // not proportional
         barLength = width * (1.0 + spaceratio);

      Point2 offset;
      offset.x = x_offset;
      offset.y = y_offset;

      if (text_mirror)
      {
         switch (textAlignment)
         {
         case 0: // left align
         default:
            offset.x = -offset.x;
            break;
         case 1: // center align
            offset.x = -offset.x + stringWidth;
            break;
         case 2: // right align
            offset.x = -offset.x + (stringWidth * 2);
            break;
         }
      }

      CTMatrix textMatrix;
      textMatrix.scale(text_mirror?-1.0:1.0, 1.0);
      textMatrix.translate(offset.x, offset.y);
      textMatrix.rotateRadians(text_rotation);
      textMatrix.translate(text_x, text_y);

      textMatrix.translate(insert_x, insert_y);
      textMatrix.rotateRadians(rotation);
      textMatrix.scale(((mirror&MIRROR_FLIP) != 0) ? -1.0 : 1.0,1.0);

      CExtent charExt(0, 0, width, height);
      charExt.expand(penWidth);
      charExt.transform(textMatrix);

      // DO NOT REMOVE:  This is used for debugging purposes
      //dc.MoveTo(charExt.getXmin() * view->scaleFactor, charExt.getYmin() * view->scaleFactor);
      //dc.LineTo(charExt.getXmin() * view->scaleFactor, charExt.getYmax() * view->scaleFactor);
      //dc.LineTo(charExt.getXmax() * view->scaleFactor, charExt.getYmax() * view->scaleFactor);
      //dc.LineTo(charExt.getXmax() * view->scaleFactor, charExt.getYmin() * view->scaleFactor);
      //dc.LineTo(charExt.getXmin() * view->scaleFactor, charExt.getYmin() * view->scaleFactor);

      if (CSearchStatics::getX() > charExt.getXmin() - CSearchStatics::getRange() && 
          CSearchStatics::getX() < charExt.getXmax() + CSearchStatics::getRange() && 
          CSearchStatics::getY() > charExt.getYmin() - CSearchStatics::getRange() && 
          CSearchStatics::getY() < charExt.getYmax() + CSearchStatics::getRange()    )
      {
         return true;
      }

      // Find offset from insertion point for this character
      if (proportional && fs)
         x_offset += (fs->getCharacterWidth(c) * 2.0 + spaceratio) * width + penWidth; // in .fnt file, chars are 4x8, so full width is 0.5
      else // not proportional
         x_offset += width * (1.0 + spaceratio) + penWidth;

      textMode = 0;  // must be on the end.
   }

   // DO NOT REMOVE:  This is used for debugging purposes
   //dc.SelectObject(oldPen);
   //dc.SelectObject(oldBrush);

   return false;
}

/*****************************************************************************
* HitVisibleAttrib
*/
bool HitVisibleAttrib(CCEtoODBDoc *doc, double x, double y, double range, CAttributes* map, Point2 *pnt, 
      double block_scale, double block_rot, int block_mir)
{
   bool retval = false;
   
   for (POSITION attribPos = map->GetStartPosition();attribPos != NULL && !retval;)
   {
      Attrib* attrib;
      WORD keyword;
      map->GetNextAssoc(attribPos, keyword, attrib);

      if (!attrib->isVisible() || attrib->getValueType() != VT_STRING || !doc->get_layer_visible(attrib->getLayerIndex(), block_mir))
         continue;

      FontStruct* fs = CFontList::getFontList().getFirstFont();
      CPoint2d point(attrib->getCoordinate());

      CTMatrix mat;
      mat.rotateRadians(block_rot);

      if (block_mir & MIRROR_FLIP)
         mat.scale(-1.0, 1.0);

      mat.transform(point);

      retval = doc->SearchText(attrib->getStringValue(), attrib->getPenWidthIndex(), pnt->x, pnt->y,
         block_scale, block_rot, block_mir, point.x, point.y, 0, attrib->getRotationRadians(), fs->getFontNumber(),
         attrib->getWidth(), attrib->getHeight(), attrib->isProportionallySpaced(),
         attrib->getHorizontalPosition(), attrib->getVerticalPosition());
   }

   return retval;
}

/******************************************************************************
* HitPolyList
*/
static bool HitPolyList(double xLoc, double yLoc, CPolyList *polyList, CCEtoODBDoc *doc, DTransform xForm, Point2 &startSeg, Point2 &endSeg, CPnt *&startPoint)
{
   POSITION polyPos, pntPos;
   BOOL FirstPoint;
   double bulge;
   BlockStruct *polyWidth;
   double width;

   polyPos = polyList->GetHeadPosition();
   while (polyPos != NULL)
   {
      selPoly = polyList->GetNext(polyPos);

      if (selPoly->getWidthIndex() < 0 || !doc->getWidthTable()[selPoly->getWidthIndex()])
         width = 0;
      else
      {
         polyWidth = doc->getWidthTable()[selPoly->getWidthIndex()];
         width = polyWidth->getSizeA() / 2 * xForm.scale;
         if (polyWidth->getShape() == T_RECTANGLE && polyWidth->getSizeB() > polyWidth->getSizeA())
            width = doc->getWidthTable()[selPoly->getWidthIndex()]->getSizeB() / 2 * xForm.scale;
      }

      if (!selPoly->getPntList().GetCount()) 
         continue;

      FirstPoint = TRUE;

      pntPos = selPoly->getPntList().GetHeadPosition();
      while (pntPos != NULL)
      {
         startPoint = selPoly->getPntList().GetNext(pntPos);

         startSeg.x = startPoint->x;
         startSeg.y = startPoint->y;
         xForm.TransformPoint(&startSeg);
         if (FirstPoint)
            FirstPoint = FALSE;
         else
         {
            if (fabs(bulge) < SMALLNUMBER)
            {
               if (HitLine(&endSeg, &startSeg, width, xLoc, yLoc,CSearchStatics::getRange()))
                  return true;
            }
            else
            {
               Point2 center;
               double da, sa, r;
               da = atan(bulge) * 4;
               if (xForm.mirror & MIRROR_FLIP) da = -da;
               ArcPoint2Angle(endSeg.x, endSeg.y, startSeg.x, startSeg.y, da, 
                     &center.x, &center.y, &r, &sa);
               if (HitArc(&center, r, sa, da, width, xLoc, yLoc,CSearchStatics::getRange()))
                  return true;
            }
         }
         bulge = startPoint->bulge;
         endSeg.x = startSeg.x;
         endSeg.y = startSeg.y;
      }
   }
   selPoly = NULL;

   return false;
}

/*****************************************************************************
* HitLine */
BOOL HitLine(Point2 *start, Point2 *end, double width, double x, double y, double range)
{
   DoubleRect extents;

   extents.left = ( (start->x-width <= end->x-width) ? start->x-width : end->x-width);
   extents.right = ( (start->x+width >= end->x+width) ? start->x+width : end->x+width);
   extents.top = ( (start->y+width >= end->y+width) ? start->y+width : end->y+width);
   extents.bottom = ( (start->y-width <= end->y-width) ? start->y-width : end->y-width);

   if (x > extents.left - range && x < extents.right + range && 
         y > extents.bottom - range && y < extents.top + range)
   {
      double rise, run, distance; // distance from point to line
   
      rise = end->y - start->y;
      run = end->x - start->x;
      if (run == 0.0) // undefined slope
         distance = fabs(start->x - x);
      else if (rise == 0.0) // zero slope
         distance = fabs(start->y - y);
      else
      {      
         double px, py, // point on line closest to cursor point
            slope, perpendicular, // slope of line and perpendicular to line
            b1, b2; // b is y intercept
         slope = rise / run;
         perpendicular = - run / rise;
         b1 = start->y - slope * start->x;
         b2 = y - perpendicular * x;
         px = (b2 - b1) / (slope - perpendicular);
         py = perpendicular * px + b2;
         distance = sqrt((x-px)*(x-px) + (y-py)*(y-py)); // pathagorian theorem
      }
      return ( (distance - width) < range );
   }
   return FALSE;
}

/*****************************************************************************
* HitArc */
BOOL HitArc(Point2 *center, double radius, double sa, double da, double width, 
            double x, double y, double range)
{
   // if no delta, then no arc
   if (fabs(da) < SMALLNUMBER)
      return FALSE;

   // distance from search point to center should be close to radius
   double distance = sqrt((x-center->x)*(x-center->x) + (y-center->y)*(y-center->y)); // pathagorian

   // circle check
   if ( fabs(distance - radius) < range + width)
   {
      // if delta >= 2PI, then circle
      if (da >= PI*2)
         return TRUE;

      // normalize arc to 
      // 0.0 <= start < 2PI
      // 0.0 < end < 4PI
      // end > start
      double start, end;

      if (da > 0.0) // positive delta
      {
         start = sa;
         end = start + da;
      }
      else // negative delta
      {
         end = sa;
         start = end + da;
      }

      while (start < 0.0)
      {
         start += PI*2;
         end += PI*2;
      }
      while (start >= PI*2)
      {
         start -= PI*2;
         end -= PI*2;
      }

      double angle = ArcTan2(y-center->y, x-center->x); // returns -PI..+PI
      if (angle < 0.0)
         angle += PI*2; // 0 <= angle < 2PI

      double thicknessAngle;
      double thickness = width+range;
      if (thickness > radius)
         thicknessAngle = 1; // approximation
      else
         thicknessAngle = asin((width+range)/radius);

      return ( (angle > start-thicknessAngle && angle < end+thicknessAngle) || // start <= angle <= end
         (angle + PI*2 > start-thicknessAngle && angle + PI*2 < end+thicknessAngle) ); // end > 2PI, angle < start but on arc
   }

   return FALSE;
}

void CCEtoODBDoc::OnStepInto() 
{
   //if (getSelectStackLevelIndex() < getSelectStackTopIndex())
   if (getSelectStack().incrementLevelIndex())
   {
      //setSelectStackLevelIndex(getSelectStackLevelIndex() + 1);
      DrawEntity(getSelectStack().getAt(0), 1, TRUE);
      SelectEntity(TRUE);
      
      if (Editing)
         OnEditEntity();
   }
   else if (StatusBar)
   {
      StatusBar->SetWindowText("Can not Step In.  Currently at bottom level.");
   }

#ifdef ImplementSelectStackDialog
   CSelectStackDialog::update(this);
#endif
}

void CCEtoODBDoc::OnStepOutOf() 
{
   //if (getSelectStackLevelIndex() > 0)
   if (getSelectStack().decrementLevelIndex())
   {
      //setSelectStackLevelIndex(getSelectStackLevelIndex() - 1);
      DrawEntity(getSelectStack().getAt(0), 1, TRUE);
      SelectEntity(TRUE);

      if (Editing)
         OnEditEntity();
   }
   else if (StatusBar)
   {
      StatusBar->SetWindowText("Can not Step Out.  Currently at top level.");
   }

#ifdef ImplementSelectStackDialog
   CSelectStackDialog::update(this);
#endif
}

// end SEARCH.CPP
