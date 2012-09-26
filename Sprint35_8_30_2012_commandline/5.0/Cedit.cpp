
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include "cedit.h"
#include "geomlib.h"
#include "license.h"
#include "ccview.h"
#include "crypt.h"
#include "measure.h"
#include "graph.h"
#include <math.h>
#include <float.h>
#include "status.h"
#include "filelist.h"
#include "draw.h"
#include "polylib.h"
#include "EntityNumber.h"
#include "mainfrm.h"
#include "GerberEducatorToolbar.h"
#include "CCEtoODB.h"

extern CView      *activeView;
extern CDialogBar *CurrentSettings; // from MAINFRM.CPP
extern CStatusBar *StatusBar; // from MAINFRM.CPP
extern BOOL       Editing; // from SELECT.CPP
//extern MeasureStruct measure; // MEASURE.CPP

void CloseEditDlg();
void FillWidthCB(CComboBox *m_widthCB, short penWidthIndex, CCEtoODBDoc *doc);
void InsertModifyUndo(CCEtoODBDoc *doc, BOOL Head);
void InsertCopyUndo(CCEtoODBDoc *doc);
void ResetButtons();
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);
//static void OffsetLayer(CCEtoODBDoc *doc, int layerNum, double x, double y);
//static void moveOrCopySelectedToAnotherFile(CCEtoODBDoc& camCadDoc, const bool moveSelected);

/******************************************************************************
* InitCurrentSettings
*/
void InitCurrentSettings()
{
   CurrentSettings->SetWindowText("Current Settings");

   CEdit *CurrentScale = (CEdit*)CurrentSettings->GetDlgItem(IDC_SCALE);
   CurrentScale->SetWindowText("1.00");

   CEdit *CurrentRotation = (CEdit*)CurrentSettings->GetDlgItem(IDC_ROTATION);
   CurrentRotation->SetWindowText("0.0");

   CEdit *CurrentTextHeight = (CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT);
   CurrentTextHeight->SetWindowText("0.10");

   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
   CurrentLayer->ResetContent();
   CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
   CurrentWidth->ResetContent();
   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   CurrentFile->ResetContent();
} 

/******************************************************************************
* UpdateLayers
*/
/*void UpdateLayers(CCEtoODBDoc *doc)
{
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);

   int sel = CurrentLayer->GetCurSel();
   LayerStruct *selLayer = NULL;
   if (sel != LB_ERR)
      selLayer = (LayerStruct*)CurrentLayer->GetItemDataPtr(sel);

   CurrentLayer->ResetContent();

   if (!doc)
      return;

   int newSel = 0;

   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];

      if (layer == NULL)
         continue;

      if (layer->isVisible() && layer->isEditable())
      {
         int temp = CurrentLayer->AddString(layer->getName());

         CurrentLayer->SetItemDataPtr(temp, layer);

         if (layer == selLayer)
            newSel = temp;
      }
   }

   CurrentLayer->SetCurSel(newSel);
}*/

/******************************************************************************
* FillCurrentSettings
*/
/*void FillCurrentSettings(CCEtoODBDoc *doc)
{
   POSITION pos;
   FileStruct *fp;
   int temp;

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   temp = CurrentFile->GetCurSel();
   CurrentFile->ResetContent();
   pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      fp = doc->getFileList().GetNext(pos);
      if (fp->isHidden() || !fp->isShown())
         continue;

      char name[_MAX_FNAME], ext[_MAX_EXT];
      _splitpath(fp->getName(), NULL, NULL, name, ext);
      CString fname = name;
      fname += ext;
      CurrentFile->SetItemDataPtr(CurrentFile->AddString(fname), fp);
      temp = 0;   // set to the first visible file
   }
   CurrentFile->SetCurSel(temp);

   //UpdateLayers(doc);

   CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
   temp = CurrentWidth->GetCurSel();
   if (temp < 0) temp = 0;
   CurrentWidth->ResetContent();
   FillWidthCB(CurrentWidth, temp, doc);
}*/

/******************************************************************************
* SetCurrentLayer
*/
/*void SetCurrentLayer(int num)
{
   CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);

   int count = CurrentLayer->GetCount();

   int sel = -1;
   for (int i = 0; i < count; i++)
   {
      LayerStruct *layer = (LayerStruct *)(CurrentLayer->GetItemDataPtr(i));

      if (layer == NULL)
         continue;

      if (layer->getLayerIndex() == num)
      {
         sel = i;
         break;
      }
   }
   if (sel > -1)  // ??? Wolf
      CurrentLayer->SetCurSel(sel);
}
*/
/******************************************************************************
* SetCurrentFile
*/
/*void SetCurrentFile(int num)
{
   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   int count = CurrentFile->GetCount();
   int sel = -1;

   if (count)  // preset to first file
   {
      FileStruct *file = (FileStruct*)CurrentFile->GetItemDataPtr(0);
      sel = file->getFileNumber();
   }

   BOOL FileFound = FALSE;
   for (int i = 0; i < count; i++)
   {
      FileStruct *file = (FileStruct*)CurrentFile->GetItemDataPtr(i);
      if (file->getFileNumber() == num)
      {
         sel = i;
         FileFound = TRUE;
         break;
      }
   }
   CurrentFile->SetCurSel(sel);
}*/

/******************************************************************************
* SetCurrentWidth
*/
/*void SetCurrentWidth(BlockStruct *b)
{
   CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
   int count = CurrentWidth->GetCount();

   for (int i = 0; i < count; i++)
   {
      if (CurrentWidth->GetItemDataPtr(i) == b)
      {
         CurrentWidth->SetCurSel(i);
         return;
      }
   }
}*/

/******************************************************************************
* SetCurrentScale
*/
/*void SetCurrentScale(double s)
{
   CString buf;
   buf.Format("%.2lf", s);
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_SCALE))->SetWindowText(buf);
}
*/
/******************************************************************************
* SetCurrentRotation
*/
/*void SetCurrentRotation(double r)
{
   CString buf;
   buf.Format("%.1lf", RadToDeg(r));
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_ROTATION))->SetWindowText(buf);
}*/                   

/******************************************************************************
* SetCurrentTextHeight
*/
/*void SetCurrentTextHeight(double h)
{
   CString buf;
   buf.Format("%.2lf", h);
   ((CEdit*)CurrentSettings->GetDlgItem(IDC_TEXT_HEIGHT))->SetWindowText(buf);
}*/

/******************************************************************************
* SetCurrentMirror
*/
/*void SetCurrentMirror(BOOL m)
{
   ((CButton*)CurrentSettings->GetDlgItem(IDC_MIRROR))->SetCheck(m?1:0);
}*/


/////////////////////////////////////////////////////////////////////////////
// CSDialogBar dialog
IMPLEMENT_DYNAMIC(CSDialogBar, CDialogBar)

BEGIN_MESSAGE_MAP(CSDialogBar, CDialogBar)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
END_MESSAGE_MAP()

CSDialogBar::CSDialogBar() : CDialogBar()
{ }

BOOL CSDialogBar::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);

      switch (nID)
      {
      case IDC_LAYERCB:
         {
            CComboBox *CurrentLayer = (CComboBox*)CurrentSettings->GetDlgItem(IDC_LAYERCB);
            int sel = CurrentLayer->GetCurSel();
            if (sel != CB_ERR)
            {
               CurrentLayer->GetLBText(sel, hint);
               if (!hint.IsEmpty())
                  pTTT->lpszText = hint.GetBuffer(0);
            }
         }
         break;

      case IDC_FILECB:
         {
            CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
            int sel = CurrentFile->GetCurSel();
            if (sel != CB_ERR)
            {
               CurrentFile->GetLBText(sel, hint);
               if (!hint.IsEmpty())
                  pTTT->lpszText = hint.GetBuffer(0);
            }
         }
         break;

      case IDC_WIDTHCB:
         {
            CComboBox *CurrentWidth = (CComboBox*)CurrentSettings->GetDlgItem(IDC_WIDTHCB);
            int sel = CurrentWidth->GetCurSel();
            if (sel != CB_ERR)
            {
               CurrentWidth->GetLBText(sel, hint);
               if (!hint.IsEmpty())
                  pTTT->lpszText = hint.GetBuffer(0);
            }
         }
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}


/******************************************************************************
* IsLicensed
*/
/*static BOOL IsLicensed(CCEtoODBDoc *doc) 
{
   /*if (getApp().getCamcadLicense().isLicensed(camcadProductPrint))
      return TRUE;*/

   /*if (getApp().getCamcadLicense().isLicensed(camcadProductRedline)) 
   {
      POSITION pos = doc->SelectList.GetHeadPosition();
      while (pos != NULL)
      {
         SelectStruct *selectStruct = doc->SelectList.GetNext(pos);

         if (selectStruct->getData()->getDataType() == T_INSERT || doc->getLayerArray()[selectStruct->getData()->getLayerIndex()]->getLayerType() != LAYTYPE_REDLINE)
         {
            ErrorMessage("Can not edit Non-Redline entities.", "There are Non-Redline entities selected.");
            return FALSE;
         }
      }
      return TRUE;
   }

   ErrorAccess("You do not have a License to Edit!");
   return FALSE;
}*/

/*void CCEtoODBDoc::OnSelectAllWithSameGeometry() 
{
   /*if (!IsLicensed(this))
      return;

   CWaitCursor wait;

   if (SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   SelectStruct* selectStruct = SelectList.GetHead();
   DataStruct* selectedData = selectStruct->getData();

   if (selectedData->getDataType() == dataTypeInsert)
   {
      InsertStruct* selectedInsert = selectedData->getInsert();
      int selectedBlockNumber = selectedInsert->getBlockNumber();

      int selectListModificationCount = SelectList.getModificationCount();

      for (POSITION filePos = getFileList().GetHeadPosition(); filePos != NULL;)
      {
         FileStruct* file = getFileList().GetNext(filePos);

         if (!file->isShown() || file->isHidden())
         {
            continue;
         }

         double fileRot = file->getRotation();

         if (getBottomView())
         {
            fileRot = -fileRot;
         }

         for (CDataListIterator insertIterator(*(file->getBlock()),dataTypeInsert);insertIterator.hasNext();)
         {
            DataStruct* data = insertIterator.getNext();

            if (!data->isVisible(getCamCadData()))
            {
               continue;
            }

            InsertStruct* insert = data->getInsert();

            if (insert->getBlockNumber() == selectedBlockNumber)
            {
               data->setMarked(true);
               SelectStruct* selectStruct = InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), file->getScale(), fileRot, 
                     file->getResultantMirror(getBottomView()), &(file->getBlock()->getDataList()), TRUE, FALSE);
               DrawEntity(selectStruct,drawStateAccordingToDataState, FALSE);
            }
         }

         //SelectStruct* selectStruct = InsertInSelectList(selectedData, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), file->getScale(), fileRot, 
         //      file->getResultantMirror(Bottom), &(file->getBlock()->getDataList()), TRUE, FALSE);
         //DrawEntity(selectStruct,drawStateHighlighted, FALSE);
      }

      updateGerberEducator(selectListModificationCount);
   }
}

void CCEtoODBDoc::OnSelectAllFlashesWithSameDCode() 
{
   /*if (!IsLicensed(this))
      return;

   CWaitCursor wait;

   if (SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   SelectStruct* selectStruct = SelectList.GetHead();
   DataStruct* selectedData = selectStruct->getData();

   if (selectedData->getDataType() == dataTypeInsert)
   {
      InsertStruct* selectedInsert = selectedData->getInsert();
      int selectedBlockNumber = selectedInsert->getBlockNumber();
      BlockStruct* selectedBlock = getBlockAt(selectedBlockNumber);

      if ( selectedBlock != NULL )
      {
         int selectedDcode = selectedBlock->getDcode();

         if (selectedBlock->isAperture())
         {
            int selectListModificationCount = SelectList.getModificationCount();

            for (POSITION filePos = getFileList().GetHeadPosition(); filePos != NULL;)
            {
               FileStruct* file = getFileList().GetNext(filePos);

               if (file->isShown() && !file->isHidden())
               {

                  double fileRot = file->getRotation();

                  if (getBottomView())
                  {
                     fileRot = -fileRot;
                  }

                  for (CDataListIterator insertIterator(*(file->getBlock()),dataTypeInsert);insertIterator.hasNext();)
                  {
                     DataStruct* data = insertIterator.getNext();

                     if (data->isVisible(getCamCadData()))
                     {

                        InsertStruct* insert = data->getInsert();
                        BlockStruct* block = getBlockAt(insert->getBlockNumber());

                        if (block != NULL && block->isAperture() && block->getDcode() == selectedDcode)
                        {
                           data->setMarked(true);
                           SelectStruct* selectStruct = InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), file->getScale(), fileRot, 
                                 file->getResultantMirror(getBottomView()), &(file->getBlock()->getDataList()), TRUE, FALSE);
                           DrawEntity(selectStruct,drawStateAccordingToDataState, FALSE);
                        }
                     }
                  }
               }

               //SelectStruct* selectStruct = InsertInSelectList(selectedData, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), file->getScale(), fileRot, 
               //      file->getResultantMirror(Bottom), &(file->getBlock()->getDataList()), TRUE, FALSE);
               //DrawEntity(selectStruct,drawStateHighlighted, FALSE);
            }

            updateGerberEducator(selectListModificationCount);
         }
      }
   }
}

void CCEtoODBDoc::OnSelectAllDrawsWithSameDCode() 
{
   /*if (!IsLicensed(this))
      return;

   CWaitCursor wait;

   if (SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   SelectStruct* selectStruct = SelectList.GetHead();
   DataStruct* selectedData = selectStruct->getData();

   if (selectedData->getDataType() == dataTypePoly)
   {
      int selectedWidthIndex = selectedData->getPolyList()->getCommonWidthIndex();

      if (selectedWidthIndex > 0)
      {
         BlockStruct* selectedWidthBlock = getWidthBlock(selectedWidthIndex);
         int selectedDcode = selectedWidthBlock->getDcode();   

         int selectListModificationCount = SelectList.getModificationCount();

         for (POSITION filePos = getFileList().GetHeadPosition(); filePos != NULL;)
         {
            FileStruct* file = getFileList().GetNext(filePos);

            if (!file->isShown() || file->isHidden())
            {
               continue;
            }

            double fileRot = file->getRotation();

            if (getBottomView())
            {
               fileRot = -fileRot;
            }

            for (CDataListIterator polyIterator(*(file->getBlock()),dataTypePoly);polyIterator.hasNext();)
            {
               DataStruct* data = polyIterator.getNext();

               if (!data->isVisible(getCamCadData())/* || data == selectedData)
               /*{
                  continue;
               }

               bool markFlag = true;
               CPolyList* polyList = data->getPolyList();
               int widthIndex = polyList->getCommonWidthIndex();

               if (widthIndex > 0)
               {
                  BlockStruct* widthBlock = getWidthBlock(widthIndex);
                  
                  if (widthBlock->getDcode() == selectedDcode)
                  {
                     data->setMarked(true);
                     SelectStruct* selectStruct = InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), file->getScale(), fileRot, 
                           file->getResultantMirror(getBottomView()), &(file->getBlock()->getDataList()), TRUE, FALSE);
                     DrawEntity(selectStruct,drawStateAccordingToDataState, FALSE);
                  }
               }
            }

            //SelectStruct* selectStruct = InsertInSelectList(selectedData, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), file->getScale(), fileRot, 
            //      file->getResultantMirror(Bottom), &(file->getBlock()->getDataList()), TRUE, FALSE);
            //DrawEntity(selectStruct,drawStateHighlighted, FALSE);
         }

         updateGerberEducator(selectListModificationCount);
      }
   }
}*/

/* OnClearSelected ***********************************************************/
/*void CCEtoODBDoc::OnClearSelected() 
{
   clearSelected(true,true);
}*/

/*void CCEtoODBDoc::clearSelected(bool redrawFlag,bool checkVisibleEntitiesOnlyFlag) 
{
   CWaitCursor wait;

   int selectListModificationCount = SelectList.getModificationCount();

   unselectAll(redrawFlag,checkVisibleEntitiesOnlyFlag);

   updateGerberEducator(selectListModificationCount);
	CGerberEducatorUi::getGerberEducatorUi(*this).updateSelectedEntity();
}
*/
/***********************************************************
* ReverseMarked 
*
*  - Clears all marked entities (only redrawing visible marked) and marks all non-marked entities that are visible
*/
/*void CCEtoODBDoc::ReverseMarked()
{
   CWaitCursor wait;

   int selectListModificationCount = SelectList.getModificationCount();

   BlockStruct *subblock;

   // free memory for select list (items will be unmarked/unselected in Reverse routine)
   POSITION selectPos = SelectList.GetHeadPosition();
   while (selectPos != NULL)
   {
      SelectStruct *selectStruct = SelectList.GetNext(selectPos);
      int layer = selectStruct->getData()->getLayerIndex();
      if (IsEntityVisible(selectStruct->getData(), &subblock, this, (selectStruct->mirror?MIRROR_FLIP:0), &layer, FALSE))
         DrawEntity(selectStruct, 0, FALSE);
      DrawEntity(selectStruct, 0, FALSE);
      delete selectStruct;
   }
   SelectList.RemoveAll();
   getSelectStack().empty();

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      double fileX = file->getInsertX();
      double fileRot = file->getRotation();
      if (getBottomView())
      {
         fileX = -fileX;
         fileRot = -fileRot;
      }

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         int layer = data->getLayerIndex();

         if (!IsEntityVisible(data, &subblock, this, (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0), &layer, FALSE))
            continue;

         if (data->isMarked() || data->isSelected())
         {
            data->setMarked(false);
            data->setSelected(false);
         }
         else
         {
            SelectStruct *selectStruct = InsertInSelectList(data, file->getBlock()->getFileNumber(), fileX, file->getInsertY(), file->getScale(), 
                  fileRot, (file->getResultantMirror(getBottomView()) ? MIRROR_FLIP : 0), &(file->getBlock()->getDataList()), TRUE, TRUE);
            DrawEntity(selectStruct, 2, FALSE);
         }
      }
   }

   updateGerberEducator(selectListModificationCount);
}*/

/*void CCEtoODBDoc::unmarkAndUnselectEntity() 
{
   if (SelectList.IsEmpty()) return;

   int selectListModificationCount = SelectList.getModificationCount();

   SelectStruct *selectStruct = SelectList.GetHead();

   if (selectStruct->getData()->isSelected() || selectStruct->getData()->isMarked())
   {
      selectStruct->getData()->setSelected(false);  
      getSelectStack().empty();

      selectStruct->getData()->setMarked(false);
      DrawEntity(selectStruct, 0, FALSE);
      SelectList.RemoveHead();
      delete selectStruct;
   }

   updateGerberEducator(selectListModificationCount);
}*/

/*
        Before     |      After      |
   selected marked | selected marked |
       0       0   |     1      1    |    
       0       1   |     0      0    |    
       1       0   |     0      0    |    
       1       1   |     0      0    |    

*/
/*void CCEtoODBDoc::toggleMarkSelection(bool previouslySelectedFlag) 
{
   if (SelectList.IsEmpty()) return;

   int selectListModificationCount = SelectList.getModificationCount();

   SelectStruct *selectStruct = SelectList.GetHead();

   if (previouslySelectedFlag || selectStruct->getData()->isMarked())
   {
      selectStruct->getData()->setSelected(false);  
      getSelectStack().empty();

      selectStruct->getData()->setMarked(false);
      DrawEntity(*selectStruct,drawStateNormal);
      SelectList.RemoveHead();
      delete selectStruct;
   }
   else
   {
      selectStruct->getData()->setMarked(true);
   }

   updateGerberEducator(selectListModificationCount);
}*/

/*****************************************************************************
* MarkEntity */
/*void CCEtoODBDoc::MarkEntity() 
{
   if (SelectList.IsEmpty()) return;

   int selectListModificationCount = SelectList.getModificationCount();

   SelectStruct *selectStruct = SelectList.GetHead();

   if (selectStruct->getData()->isSelected())
   {
      selectStruct->getData()->setSelected(false);  
      getSelectStack().empty();

      if (selectStruct->getData()->isMarked())
      {
         selectStruct->getData()->setMarked(false);
         DrawEntity(selectStruct, 0, FALSE);
         SelectList.RemoveHead();
         delete selectStruct;
      }
      else
      {
         selectStruct->getData()->setMarked(true);
         DrawEntity(selectStruct, 2, FALSE);
      }
   }

   updateGerberEducator(selectListModificationCount);
}*/

/*void CCEtoODBDoc::OnChangeLayer() 
{
   /*if (!IsLicensed(this))
      return;

   if (SelectList.IsEmpty())
   {
      ErrorMessage("Nothing is selected or marked!", "Change Layer");
      return;
   }

   SelectLayerDlg dlg;
   dlg.doc = this;

   if (dlg.DoModal() != IDOK)
      return;

   bool copyFlag = dlg.getCopy();

   InsertModifyUndo(this, FALSE);

   SelectStruct *selectStruct;
   
   for (POSITION pos = SelectList.GetHeadPosition();pos != NULL;)
   {
      selectStruct = SelectList.GetNext(pos);

      DataStruct* data = selectStruct->getData();

      data->setSelected(false);
      data->setMarked(false);

      if (copyFlag)
      {
         data = getCamCadData().getNewDataStruct(*data);
         selectStruct->getParentDataList()->AddTail(data);
      }

      data->setLayerIndex(dlg.layer);

      SelectList.RemoveHead();

      DrawEntity(selectStruct, 0, FALSE);

      if (copyFlag)
      {
         selectStruct->setData(data);
         DrawEntity(selectStruct, 0, FALSE);
      }

      delete selectStruct;
   }

   UnselectAll(FALSE);

   if (Editing)
      OnEditEntity();
}

void CCEtoODBDoc::OnDeleteSelected() 
{
   /*if (!IsLicensed(this))
   {
      return;
   }

   if (getApp().getRunMode() == runModeDataProtectedModelessDialog)
   {
      return;
   }

   CWaitCursor wait;

   ((CCEtoODBView*)activeView)->lastCommand = LC_Delete;

   if (SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   PrepareThisUndoLevel();
   undo[undo_level] = new UndoItemList;

   SelectStruct *selectStruct;
   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      POSITION dataPos, tempPos;

      selectStruct = SelectList.GetNext(pos);
      DrawEntity(selectStruct, -1, FALSE);

      dataPos = selectStruct->getParentDataList()->Find(selectStruct->getData());
      ASSERT(dataPos);
      SelectList.RemoveHead();
      selectStruct->getData()->setSelected(false);
      selectStruct->getData()->setMarked(false);

      FileStruct* fileStruct = Find_File(selectStruct->filenum);

      if (fileStruct != NULL)
      {
         fileStruct->getBlock()->resetExtent();
      }

      // update undo array
      UndoItemStruct *i = new UndoItemStruct;
      i->type = Edit_Delete;
      i->DataList = selectStruct->getParentDataList();
      tempPos = dataPos; // a copy so we don't modify dataPos
      i->DataList->GetPrev(tempPos);
      if (tempPos)
         i->num = i->DataList->GetAt(tempPos)->getEntityNumber();
      else
         i->num = 0;
      // move data to undo structure, so no need to free memory
      i->data = selectStruct->getParentDataList()->GetAt(dataPos);
      selectStruct->getParentDataList()->RemoveAt(dataPos);

      undo[undo_level]->AddHead(i);

      delete selectStruct;
   }
   getSelectStack().empty();

   undo_level = redo_max = (undo_level + 1) % MAX_UNDO_LEVEL;
   ClearNextUndoLevel();

   if (Editing)
      OnEditEntity();
}*/

/******************************************************************************
* OnMoveSelected
*/
/*void CCEtoODBView::OnMoveSelected() 
{
   /*if (!IsLicensed(GetDocument()))
      return;

   if (zoomMode != NoZoom) 
      return;

   if (getApp().getRunMode() == runModeDataProtectedModelessDialog)
   {
      return;
   }

   lastCommand = LC_Move;

   ResetButtons();
   if (cursorMode == Move)
   {
      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   if (GetDocument()->SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   InsertModifyUndo(GetDocument(), FALSE);

   cursorAnchored = FALSE;
   cursorFirstDrag = TRUE;
   HideSearchCursor();
   cursorMode = Move;
   ///ShowSearchCursor();
}
*/
/*void CCEtoODBView::OnMouseMove_MoveSelected(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   CCEtoODBDoc *doc = GetDocument();
   
   // erase entities
   POSITION selPos = doc->SelectList.GetHeadPosition();
   while (selPos)
   {
      SelectStruct *selectStruct = doc->SelectList.GetNext(selPos);
      doc->DrawEntity(selectStruct, -2, FALSE);
   }

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;
   selPos = doc->SelectList.GetHeadPosition();
   while (selPos)
   {
      SelectStruct *selectStruct = doc->SelectList.GetNext(selPos);

      // untransform the difference according to this entity's placement
      Point2 temp;
      temp.x = factor * (pnt->x - cursorPrevPnt.x);
      temp.y = factor * (pnt->y - cursorPrevPnt.y);

      temp.x += selectStruct->insert_x;
      temp.y += selectStruct->insert_y;

      Untransform(doc, &temp, selectStruct);

      switch (selectStruct->getData()->getDataType())
      {
      case T_POINT:
         selectStruct->getData()->getPoint()->x += (DbUnit)temp.x;         
         selectStruct->getData()->getPoint()->y += (DbUnit)temp.y;         
         break;
      case T_BLOB:
         selectStruct->getData()->getBlob()->pnt.x += (DbUnit)temp.x;         
         selectStruct->getData()->getBlob()->pnt.y += (DbUnit)temp.y;         
         break;
      case T_POLY:
         {
            POSITION polyPos, pntPos;
            CPoly *poly;
            CPnt *pnt;

            polyPos = selectStruct->getData()->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = selectStruct->getData()->getPolyList()->GetNext(polyPos);
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);

                  pnt->x += (DbUnit)temp.x;        
                  pnt->y += (DbUnit)temp.y;        
               }
            }
         }
         break;
      case T_TEXT:
         selectStruct->getData()->getText()->incPntX((DbUnit)temp.x);         
         selectStruct->getData()->getText()->incPntY((DbUnit)temp.y);         
         break;
      case T_INSERT:
         selectStruct->getData()->getInsert()->incOriginX(temp.x);       
         selectStruct->getData()->getInsert()->incOriginY(temp.y);       
         break;
      }
      doc->DrawEntity(selectStruct, -2, FALSE);
   }

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;
}

void CCEtoODBView::OnLButtonDown_MoveSelected(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();

   if (!cursorAnchored) // if picking cursorAnchor point
   {
      cursorAnchored = TRUE;
      cursorPrevPnt.x = pnt->x;
      cursorPrevPnt.y = pnt->y;

      // draw entities in background color
      POSITION selectPos = doc->SelectList.GetHeadPosition();
      while (selectPos)
      {
         SelectStruct *selectStruct = doc->SelectList.GetNext(selectPos);

         doc->DrawEntity(selectStruct, (lastCommand==LC_Copy)?0:-1, FALSE);
      }

      MovingDatas = TRUE;

      selectPos = doc->SelectList.GetHeadPosition();
      while (selectPos)
      {
         SelectStruct *selectStruct = doc->SelectList.GetNext(selectPos);
         doc->DrawEntity(selectStruct, -2, FALSE);
      }
   }
   else // else placing entities
   {
      //OnMouseMove_MoveSelected(dc, pnt);

      POSITION selectPos = doc->SelectList.GetHeadPosition();
      while (selectPos)
      {
         SelectStruct *selectStruct = doc->SelectList.GetNext(selectPos);
         doc->DrawEntity(selectStruct, -2, FALSE);
      }

      MovingDatas = FALSE;

      if (lastCommand == LC_Copy)
         InsertCopyUndo(doc);

      selectPos = doc->SelectList.GetHeadPosition();
      while (selectPos)
      {
         SelectStruct *selectStruct = doc->SelectList.GetNext(selectPos);

         FileStruct *file = doc->Find_File(selectStruct->filenum);

			// if there no specific file the edited geometry belongs to, then reset all file's extents
			if (file == NULL)
			{
				POSITION filePos = doc->getFileList().GetHeadPosition();
				while (filePos)
				{
					file = doc->getFileList().GetNext(filePos);
					file->getBlock()->resetExtent();
				}
			}
			else
				file->getBlock()->resetExtent();
         doc->DrawEntity(selectStruct, -2, FALSE); // erase 
         doc->DrawEntity(selectStruct, (selectStruct->getData()->isSelected() ? 3 : 2), FALSE);
      }

      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
   }
}*/

/*static bool verifyLicenseFileAndSelection(CCEtoODBDoc& doc)
{
   bool retval = true;

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      retval = false;
   }
   else
   {
      CMessageFilter messageFilterByMessage(messageFilterTypeMessage);
      if (doc.getFileList().GetCount() < 2)
      {
         messageFilterByMessage.formatMessageBoxApp("Need more than one file to perform this function.");
         retval = false;
      }
      else if (doc.getSelectList().GetCount() == 0)
      {
         messageFilterByMessage.formatMessageBoxApp("Nothing is selected.  Items for this operation must be selected first, then select the operation.");
         retval = false;
      }
   }

   return retval;
}*/

/*static FileStruct* getSelectedFile(CFileList& fileList, CString prompt)
{
   FileStruct* selectedFile = NULL;

   CSelectDlg dlg(prompt, false, true);
   for (POSITION pos = fileList.GetHeadPosition(); pos != NULL;)
   {
      FileStruct* file = fileList.GetNext(pos);
      if (file != NULL)
      {
         CVoidPtrSelItem* voidPtrSelItem = new CVoidPtrSelItem(file->getName(), (void*)file, false);
         dlg.AddItem(voidPtrSelItem);
      }
   }

   if (dlg.DoModal() == IDOK)
   {
      CVoidPtrSelItem* voidPtrSelItem = (CVoidPtrSelItem*)dlg.GetSelItemHead();
      if (voidPtrSelItem != NULL)
      {
         selectedFile = (FileStruct*)voidPtrSelItem->GetVoidPtrItem();
      }
   }

   return selectedFile;
}*/

/*void CCEtoODBView::OnMoveSelectedToAnotherFile()
{
   CCEtoODBDoc *doc = GetDocument();
   moveOrCopySelectedToAnotherFile(*doc,true);
}*/

/*void CCEtoODBView::OnCopySelectedToAnotherFile()
{
   CCEtoODBDoc *doc = GetDocument();
   moveOrCopySelectedToAnotherFile(*doc,false);
}*/

/*void moveOrCopySelectedToAnotherFile(CCEtoODBDoc& camCadDoc, const bool moveSelected)
{
   if (verifyLicenseFileAndSelection(camCadDoc))
   {
      FileStruct* selectedFile = getSelectedFile(camCadDoc.getFileList(), moveSelected?"Select file to move to":"Select file to copy to");
      if (selectedFile == NULL)
         return;

      BlockStruct* fileBlock = selectedFile->getBlock();
      if (fileBlock != NULL)
      {
         int levelIndex = camCadDoc.getSelectStack().getLevelIndex();
         if (levelIndex == 1)
         {
            // User has stepped into a board from a panel level or a geometry from a board level or a pin from a geometry level,
            // but only the first level of step into (F4) is allowed

            SelectStruct* curSelectStruct = camCadDoc.getSelectList().GetHead();
            SelectStruct* subSelectStruct = camCadDoc.getSelectStack().getAtLevel();
            if (curSelectStruct != NULL && subSelectStruct != NULL)
            {
               DataStruct* curSelectDataStruct = curSelectStruct->getData();
               DataStruct* subSelectDataStruct = subSelectStruct->getData();
               if (curSelectDataStruct != NULL && subSelectDataStruct != NULL)
               {
                  if (curSelectDataStruct->getDataType() == dataTypeInsert && curSelectDataStruct->getInsert() != NULL)
                  {
                     if (curSelectDataStruct->getInsert()->getInsertType() == insertTypePcb || curSelectDataStruct->getInsert()->getInsertType() == insertTypePcbComponent)
                     {
                        CTMatrix transformMatrix(curSelectDataStruct->getInsert()->getTMatrix());
                        
                        DataStruct* newDataStruct = NULL;
                 
                        subSelectDataStruct->setSelected(false);
                        subSelectDataStruct->setMarked(false);

                        if (moveSelected)
                        {
                           newDataStruct = subSelectDataStruct;

                           // MUST draw to remove selected data image before remove the data from the list
                           camCadDoc.DrawEntity(subSelectStruct, -1, FALSE);

                           POSITION dataPos = subSelectStruct->getParentDataList()->Find(subSelectDataStruct);
                           subSelectStruct->getParentDataList()->RemoveAt(dataPos);

                           if ( camCadDoc.Find_File(subSelectStruct->filenum) != NULL )
                           {
                              camCadDoc.Find_File(subSelectStruct->filenum)->getBlock()->resetExtent();  
                           }

                           camCadDoc.getSelectStack().pop();
                        }
                        else
                        {
                           newDataStruct = camCadDoc.getCamCadData().getNewDataStruct(*subSelectDataStruct, true);
                        }

                        if (newDataStruct != NULL)
                        {
                           newDataStruct->transform(transformMatrix, &(camCadDoc.getCamCadData()), true);
                           fileBlock->getDataList().AddHead(newDataStruct); 
                        }
                     }
                     else
                     {
                        CMessageFilter message(messageFilterTypeMessage);
                        message.formatMessageBox("Can only Move/Copy selected item inside a Pcb Component or Pcb Board.");
                     }
                  }
               }
            }
         }
         else if (levelIndex == 0)
         {
            CSelectList& selectList = camCadDoc.getSelectList();
            for (POSITION pos = selectList.GetHeadPosition(); pos != NULL;)
            {
               POSITION curPos = pos;
               SelectStruct* selectStruct = selectList.GetNext(pos);
               if (selectStruct == NULL)
                  continue;

               DataStruct* dataStruct = selectStruct->getData();
               if (dataStruct == NULL)
                  continue;

               dataStruct->setSelected(false);
               dataStruct->setMarked(false);

               DataStruct* newDataStruct = NULL;
               if (moveSelected)
               {
                  newDataStruct = dataStruct;

                  // MUST draw to remove selected data image before remove the data from the list
                  camCadDoc.DrawEntity(selectStruct, -1, FALSE);
                  selectList.RemoveAt(curPos);

                  POSITION dataPos = selectStruct->getParentDataList()->Find(selectStruct->getData());
                  selectStruct->getParentDataList()->RemoveAt(dataPos);

                  if ( camCadDoc.Find_File(selectStruct->filenum) != NULL )
                  {
                     camCadDoc.Find_File(selectStruct->filenum)->getBlock()->resetExtent();
                  }

                  delete selectStruct;
               }
               else
               {
                  newDataStruct = camCadDoc.getCamCadData().getNewDataStruct(*dataStruct, true);
               }

               fileBlock->getDataList().AddHead(newDataStruct);
            }
         }

         if (moveSelected)
         {
            camCadDoc.getSelectStack().empty();
            if (Editing)
               camCadDoc.OnEditEntity(); 
         }

         camCadDoc.OnFitPageToImage();
      }
   }
}*/


/******************************************************************************
* OnMoveFile
*/
static FileStruct *moveFile;
static SelectStruct selStruct;
static double offset_x, offset_y;
/*void CCEtoODBView::OnMoveFile() 
{
   /*if (!IsLicensed(GetDocument()))
      return;

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == MoveFiles)
   {
      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   CCEtoODBDoc *doc = GetDocument();

   int count = 0;
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      moveFile = doc->getFileList().GetNext(pos);

      if (moveFile->isShown())
         count++;
   }

   if (!count)
   {
      ErrorMessage("There are no visible files", "");
      return;
   }

   if (count > 1)
   {
      SelectFileDlg dlg;
      dlg.doc = doc;
      if (dlg.DoModal() != IDOK)
         return;
      moveFile = dlg.file;
   }

   cursorAnchored = FALSE;
   cursorFirstDrag = TRUE;
   HideSearchCursor();
   cursorMode = MoveFiles;
//   ShowSearchCursor();
}
*/
/*void CCEtoODBView::OnMouseMove_MoveFile(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   CCEtoODBDoc *doc = GetDocument();
   
   // erase entities
   if (!cursorFirstDrag)
   {
      POSITION dataPos = moveFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         selStruct.setData(moveFile->getBlock()->getDataList().GetNext(dataPos));

         doc->DrawEntity(&selStruct, -2, FALSE);
      }
   }
   cursorFirstDrag = FALSE;

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;
   selStruct.insert_x = (DbUnit)(offset_x + pnt->x * factor);
   selStruct.insert_y = (DbUnit)(offset_y + pnt->y * factor);

   POSITION dataPos = moveFile->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      selStruct.setData(moveFile->getBlock()->getDataList().GetNext(dataPos));

      doc->DrawEntity(&selStruct, -2, FALSE);
   }

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;
}*/

/*void CCEtoODBView::OnLButtonDown_MoveFile(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();

   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;

   if (!cursorAnchored) // if picking cursorAnchor point
   {
      cursorAnchored = TRUE;
      offset_x = moveFile->getInsertX() - pnt->x * factor;
      offset_y = moveFile->getInsertY() - pnt->y * factor;

      selStruct.setParentDataList(&moveFile->getBlock()->getDataList());
      selStruct.filenum = moveFile->getFileNumber();
      selStruct.insert_x = moveFile->getInsertX();
      selStruct.insert_y = moveFile->getInsertY();
      selStruct.scale = moveFile->getScale();
      selStruct.mirror = moveFile->isMirrored();
      selStruct.rotation = moveFile->getRotation();
      
      // erase entities
      POSITION dataPos = moveFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         selStruct.setData(moveFile->getBlock()->getDataList().GetNext(dataPos));

         doc->DrawEntity(&selStruct, -1, FALSE);
      }

      MovingDatas = TRUE;
   }
   else // else placing entities
   {
      moveFile->setInsertX((DbUnit)(offset_x + pnt->x * factor));
      moveFile->setInsertY((DbUnit)(offset_y + pnt->y * factor));

      MovingDatas = FALSE;

      POSITION dataPos = moveFile->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         selStruct.setData(moveFile->getBlock()->getDataList().GetNext(dataPos));

         doc->DrawEntity(&selStruct, 0, FALSE);
      }

      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
   }
}
*/
/******************************************************************************
* OnCopySelected
*/
/*void CCEtoODBView::OnCopySelected() 
{
   if (!IsLicensed(GetDocument()))
      return;

   if (zoomMode != NoZoom) 
      return;

   if (getApp().getRunMode() == runModeDataProtectedModelessDialog)
   {
      return;
   }

   lastCommand = LC_Copy;

   ResetButtons();
   if (cursorMode == Move)
   {
      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   CCEtoODBDoc *doc = GetDocument();
   CCamCadData& camCadData = doc->getCamCadData();

   if (doc->SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   // make copies of selected datas
   POSITION pos = doc->SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *selectStruct = doc->SelectList.GetNext(pos);
      DataStruct* newData = camCadData.getNewDataStruct(*(selectStruct->getData()));
      selectStruct->getParentDataList()->AddTail(newData);
      selectStruct->getData()->setSelected(false);
      selectStruct->getData()->setMarked(false);
      selectStruct->setData(newData);
   }

   cursorAnchored = FALSE;
   cursorFirstDrag = TRUE;
   HideSearchCursor();
   cursorMode = Move;
//   ShowSearchCursor();
}*/

/* OnRotateSelected **************************************************************/
/*void CCEtoODBView::OnRotateSelected() 
{
   /*if (!IsLicensed(GetDocument()))
      return;

   if (zoomMode != NoZoom) 
      return;

   if (getApp().getRunMode() == runModeDataProtectedModelessDialog)
   {
      return;
   }

   lastCommand = LC_Rotate;

   ResetButtons();
   if (cursorMode == RotateSelected)
   {
      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   if (GetDocument()->SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   InsertModifyUndo(GetDocument(), FALSE);

   cursorAnchored = FALSE;
   cursorFirstDrag = TRUE;
   HideSearchCursor();
   cursorMode = RotateSelected;
   //ShowSearchCursor();
}
*/
//static CEditBar *bar;

/*void CCEtoODBView::OnMouseMove_RotateSelected(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   int size = 0; //4 * GetCursorSize();

   dc->SetROP2(R2_NOT);

   // erase entities
   if (!cursorFirstDrag)
   {
      dc->MoveTo(cursorAnchor);
      dc->LineTo(cursorPrevPnt);
      dc->Arc(cursorAnchor.x - size, cursorAnchor.y + size, cursorAnchor.x + size, cursorAnchor.y - size, cursorAnchor.x + size, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);
   }

   cursorFirstDrag = FALSE;
   
   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   dc->MoveTo(cursorAnchor);
   dc->LineTo(cursorPrevPnt);
   dc->Arc(cursorAnchor.x - size, cursorAnchor.y + size, cursorAnchor.x + size, cursorAnchor.y - size, cursorAnchor.x + size, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);

   double angle = RadToDeg(ArcTan2(cursorPrevPnt.y - cursorAnchor.y, cursorPrevPnt.x - cursorAnchor.x));
   CString buf;
   buf.Format("%.1lf°", angle);

   //bar->SetWindowText(buf);
   getDefinedRotationAngleEditBar().SetWindowText(buf);
}*/

//void RotateSelectedEntities(CCEtoODBView *view, DbUnit angle, double baseX, double baseY);
/*void CCEtoODBView::OnLButtonDown_RotateSelected(CDC *dc, CPoint *pnt)
{
   int size = 0; //8 * GetCursorSize();
   if (!cursorAnchored) // if picking cursorAnchor point
   {
      //bar = new CEditBar("Rotation Angle", 10);
      getDefinedRotationAngleEditBar();

      cursorAnchored = TRUE;
      cursorAnchor.x = pnt->x;
      cursorAnchor.y = pnt->y;

      dc->SetROP2(R2_NOT);
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x + size, cursorAnchor.y);
   }
   else // else placing entities
   {
      //delete bar;
      releaseRotationAngleEditBar();

      CCEtoODBDoc *doc = GetDocument();

      HideSearchCursor();

      // erase ghost
      dc->SetROP2(R2_NOT);
      if (!cursorFirstDrag)
      {
         dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
         dc->LineTo(cursorPrevPnt);
         int size = 0; //4 * GetCursorSize();
         dc->Arc(cursorAnchor.x - size, cursorAnchor.y + size, cursorAnchor.x + size, cursorAnchor.y - size, cursorAnchor.x + size, cursorAnchor.y, cursorPrevPnt.x, cursorPrevPnt.y);
      }
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x + size, cursorAnchor.y);

      DbUnit angle = (DbUnit)ArcTan2(pnt->y - cursorAnchor.y, pnt->x - cursorAnchor.x);
      double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;
      //RotateSelectedEntities(this, angle, factor * cursorAnchor.x, factor * cursorAnchor.y);

      cursorMode = Search;
      //ShowSearchCursor();
   }
}
*/
typedef CArray<double, double> LOLeftArray;
typedef CArray<double, double> LOBottomArray;

/******************************************************************************
* OnLayerOffsetAuto
*/
/*void CCEtoODBDoc::OnLayerOffsetAuto() 
{
   LOLeftArray leftArr;
   LOBottomArray botArr;

   leftArr.SetSize(getMaxLayerIndex());
   botArr.SetSize(getMaxLayerIndex());

	int i=0;
   for (i=0; i<getMaxLayerIndex(); i++)
   {
      leftArr[i] = DBL_MAX;
      botArr[i] = DBL_MAX;
   }

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getLayerIndex() == -1)
            continue;

         double left = DBL_MAX, bot = DBL_MAX;
         switch (data->getDataType())
         {
         case T_INSERT:
            if (data->getInsert()->getOriginX() < leftArr[data->getLayerIndex()])
               leftArr[data->getLayerIndex()] = data->getInsert()->getOriginX();
            if (data->getInsert()->getOriginY() < botArr[data->getLayerIndex()])
               botArr[data->getLayerIndex()] = data->getInsert()->getOriginY();
            break;
         case T_POINT:
            if (data->getPoint()->x < leftArr[data->getLayerIndex()])
               leftArr[data->getLayerIndex()] = data->getPoint()->x;
            if (data->getPoint()->y < botArr[data->getLayerIndex()])
               botArr[data->getLayerIndex()] = data->getPoint()->y;
            break;
         case T_POLY:
            {
               POSITION polyPos = data->getPolyList()->GetHeadPosition();
               while (polyPos)
               {
                  CPoly *poly = data->getPolyList()->GetNext(polyPos);

                  POSITION pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntPos);

                     if (pnt->x < leftArr[data->getLayerIndex()])
                        leftArr[data->getLayerIndex()] = pnt->x;
                     if (pnt->y < botArr[data->getLayerIndex()])
                        botArr[data->getLayerIndex()] = pnt->y;
                  }
               }
            }
            break;
         }
      }
   }  

   for (i=0; i<getMaxLayerIndex(); i++)
   {
      if (leftArr[i] < DBL_MAX-1)
         break;
   }

   for (int j=i+1; j<getMaxLayerIndex(); j++)
   {
      if (leftArr[j] < DBL_MAX-1)
         OffsetLayer(this, j, leftArr[i] - leftArr[j], botArr[i] - botArr[j]);
   }

   UpdateAllViews(NULL);
}                                                   
*/
/******************************************************************************
* OnLayerOffset
*/
static int layerNum;
/*void CCEtoODBView::OnLayerOffset() 
{
   /*if (!IsLicensed(GetDocument()))
      return;

   lastCommand = LC_LayerOffset;

   CCEtoODBDoc *doc = ((CCEtoODBDoc*)GetDocument());

   if (doc->SelectList.IsEmpty())
   {
      ErrorMessage("Nothing is selected!", "Layer Offset");
      return;
   }

   SelectStruct *sel = doc->getSelectStack().getAtLevel();
   layerNum = sel->getData()->getLayerIndex();
   
   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == LayerOffset)
   {
      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorAnchored = FALSE;
   cursorFirstDrag = TRUE;
   HideSearchCursor();
   cursorMode = LayerOffset;
   //ShowSearchCursor();

   if (StatusBar)
      StatusBar->SetWindowText("Choose Reference Point.");
}
*/
/*void CCEtoODBView::OnMouseMove_LayerOffset(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
   dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
}

void CCEtoODBView::OnLButtonDown_LayerOffset(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) // if picking cursorAnchor point
   {
      cursorAnchored = TRUE;
      cursorAnchor.x = pnt->x;
      cursorAnchor.y = pnt->y;

      if (StatusBar)
         StatusBar->SetWindowText("Choose Move-To Point.");
   }
   else // else placing entities
   {
      CCEtoODBDoc *doc = GetDocument();

      double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;

      OffsetLayer(doc, layerNum, factor * (pnt->x - cursorAnchor.x), factor * (pnt->y - cursorAnchor.y));

      doc->UnselectAll(FALSE);
      doc->UpdateAllViews(NULL);

      HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
   }
}
*/
/*static void OffsetLayer(CCEtoODBDoc *doc, int layerNum, double x, double y)
{
   if (fabs(x) < SMALLNUMBER && fabs(y) < SMALLNUMBER)
      return;

   doc->PrepareThisUndoLevel();
   doc->undo[doc->undo_level] = new UndoItemList;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      SelectStruct selectStruct;
      selectStruct.setParentDataList(&file->getBlock()->getDataList());
      selectStruct.filenum = file->getFileNumber();
      selectStruct.insert_x = file->getInsertX();
      selectStruct.insert_y = file->getInsertY();
      selectStruct.scale = file->getScale();
      selectStruct.mirror = file->isMirrored();
      selectStruct.rotation = file->getRotation();

      Point2 temp;
      // untransform the difference according to this entity's placement
      temp.x = x;
      temp.y = y;

      temp.x += selectStruct.insert_x;
      temp.y += selectStruct.insert_y;

      Untransform(doc, &temp, &selectStruct);

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getLayerIndex() != layerNum)
            continue;

         file->getBlock()->resetExtent();

         UndoItemStruct *item = new UndoItemStruct;
         item->type = Edit_Modify;
         item->DataList = &file->getBlock()->getDataList();
         item->num = data->getEntityNumber();

         // insert a new copy of data in undo list
         item->data = doc->getCamCadData().getNewDataStruct(*data,true,item->num);
         //item->data->setEntityNumber(item->num);

         doc->undo[doc->undo_level]->AddTail(item);

         switch (data->getDataType())
         {
         case T_POINT:
            data->getPoint()->x += (DbUnit)temp.x;         
            data->getPoint()->y += (DbUnit)temp.y;         
            break;
         case T_BLOB:
            data->getBlob()->pnt.x += (DbUnit)temp.x;         
            data->getBlob()->pnt.y += (DbUnit)temp.y;         
            break;
         case T_POLY:
            {
               POSITION polyPos = data->getPolyList()->GetHeadPosition();
               while (polyPos)
               {
                  CPoly *poly = data->getPolyList()->GetNext(polyPos);
                  POSITION pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos)
                  {
                     CPnt *pnt = poly->getPntList().GetNext(pntPos);

                     pnt->x += (DbUnit)temp.x;        
                     pnt->y += (DbUnit)temp.y;        
                  }
               }
            }
            break;
         case T_TEXT:
            data->getText()->incPntX((DbUnit)temp.x);         
            data->getText()->incPntY((DbUnit)temp.y);         
            break;
         case T_INSERT:
            data->getInsert()->incOriginX(temp.x);       
            data->getInsert()->incOriginY(temp.y);       
            break;
         }
      }
   }

   doc->undo_level = doc->redo_max = (doc->undo_level + 1) % MAX_UNDO_LEVEL;
   doc->ClearNextUndoLevel();
}*/

/*void TransformPoint(CCEtoODBDoc *doc, SelectStruct *selectStruct, DbUnit *x, DbUnit *y, double angle, double baseX, double baseY)
{  
   Point2 p;
   Mat2x2 m;


   // calculate where point appears
   p.x = *x * selectStruct->scale;
   if (selectStruct->mirror & MIRROR_FLIP)  p.x = -p.x;
   p.y = *y * selectStruct->scale;
   RotMat2(&m, selectStruct->rotation);
   TransPoint2(&p, 1, &m, selectStruct->insert_x, selectStruct->insert_y);


   // calculate where point should end up
   p.x -= baseX;
   p.y -= baseY;

   RotMat2(&m, angle);
   TransPoint2(&p, 1, &m, baseX, baseY);


   // untransform to find coordinate that would transform to where point should be
   Untransform(doc, &p, selectStruct);


   // modify point
   *x = (DbUnit)p.x;
   *y = (DbUnit)p.y;
}*/

/*void RotateSelectedEntities(CCEtoODBView *view, DbUnit angle, double baseX, double baseY)
{
   CCEtoODBDoc *doc = view->GetDocument();
   POSITION pos = doc->SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *selectStruct = doc->SelectList.GetNext(pos);

      if ( doc->Find_File(selectStruct->filenum) != NULL )
      {
         doc->Find_File(selectStruct->filenum)->getBlock()->resetExtent();
      }
      doc->DrawEntity(selectStruct, -1, TRUE);

      switch (selectStruct->getData()->getDataType())
      {
      case T_POINT:
         TransformPoint(doc, selectStruct, &selectStruct->getData()->getPoint()->x, &selectStruct->getData()->getPoint()->y, angle, baseX, baseY);
         break;

      case T_POLY:
         {
            POSITION polyPos = selectStruct->getData()->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               CPoly *poly = selectStruct->getData()->getPolyList()->GetNext(polyPos);

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);
                  TransformPoint(doc, selectStruct, &pnt->x, &pnt->y, angle, baseX, baseY);
               }
            }
         }
         break;

      case T_TEXT:
         TransformPoint(doc, selectStruct, &selectStruct->getData()->getText()->getPntRef().x, &selectStruct->getData()->getText()->getPntRef().y, angle, baseX, baseY);
         if (selectStruct->mirror)
            selectStruct->getData()->getText()->setRotation(selectStruct->getData()->getText()->getRotation() - angle);
         else 
            selectStruct->getData()->getText()->setRotation(selectStruct->getData()->getText()->getRotation() + angle);
         break;

      case T_INSERT:
         PointStruct origin(selectStruct->getData()->getInsert()->getOrigin());
         TransformPoint(doc, selectStruct, &origin.x, &origin.y, angle, baseX, baseY);
         selectStruct->getData()->getInsert()->setOrigin(origin);

         if (selectStruct->mirror)
            selectStruct->getData()->getInsert()->incAngle(-angle);
         else 
            selectStruct->getData()->getInsert()->incAngle(angle);
         break;
      }

      doc->DrawEntity(selectStruct, (selectStruct->getData()->isSelected() ? 1 : 2), FALSE);
   }
}*/

/******************************************************************************
* OnCleanSelectedPolys()
*/
/*void CCEtoODBDoc::OnCleanSelectedPolys() 
{
   CWaitCursor wait;

   POSITION selPos = SelectList.GetHeadPosition();
   while (selPos != NULL)
   {
      SelectStruct *selectStruct = SelectList.GetNext(selPos);
      if (selectStruct->getData()->getDataType() == T_POLY)
      {
         POSITION polyPos = selectStruct->getData()->getPolyList()->GetHeadPosition();
         while (polyPos != NULL)
            CleanPoly(selectStruct->getData()->getPolyList()->GetNext(polyPos), 0.0001);
      }
   }
}
*/
/******************************************************************************
* OnSelfIntersect()
*/
/*void CCEtoODBDoc::OnSelfIntersect() 
{
   CWaitCursor wait;

   if (nothingIsSelected())
   {
      ErrorMessage("Nothing Selected");
      return;
   }

   SelectStruct *selectStruct = getSelectStack().getAtLevel();

   if (selectStruct->getData()->getDataType() != T_POLY)
   {
      ErrorMessage("Not a poly");
      return;
   }

   POSITION pos = selectStruct->getData()->getPolyList()->GetHeadPosition();
   while (pos)
   {
      CPoly *poly = selectStruct->getData()->getPolyList()->GetNext(pos);
      if (PolySelfIntersects(&poly->getPntList()))
         ErrorMessage("Poly Self-Intersects.", "Self-Intersect", MB_ICONSTOP);
      else
         ErrorMessage("Poly is good.", "Self-Intersect");
   }
}
*/
/******************************************************************************
* OnFixFreeVoids
*/
/*void CCEtoODBDoc::OnFixFreeVoids() 
{
   CWaitCursor wait;

   int failedCount = 0;

   POSITION pos = getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = getFileList().GetNext(pos);

      int res = FixFreeVoids(this, &file->getBlock()->getDataList());
      failedCount += res;
   }

   if (failedCount)
   {
      CString buf;
      buf.Format("%d voids failed to find parents", failedCount);
      ErrorMessage(buf, "Warning");
   }
}*/

/******************************************************************************
* CCEtoODBDoc::OnExplodePolylines
*/
/*void CCEtoODBDoc::OnExplodePolylines() 
{
   CWaitCursor wait;

   /*if (!IsLicensed(this))
      return;

#ifndef SHAREWARE
   if (SelectList.IsEmpty())
   {
      if (StatusBar)
      {
         StatusBar->SetWindowText("Nothing is selected or marked!");
      }

      return;
   }

   if (getActiveView()->getMeasure() != NULL)
   {
      getActiveView()->getMeasure()->setMeasureOn(false);
   }

   StatusBar->SetWindowText("No UNDO available for Polyline Explode.");
   ClearUndo();
   CloseEditDlg();

   SelectStruct* selectStruct;
   
   for (POSITION pos = SelectList.GetHeadPosition();pos != NULL;)
   {
      selectStruct = SelectList.GetNext(pos);

      selectStruct->getData()->setSelected(false);
      selectStruct->getData()->setMarked(false);

      DataStruct& polyStruct = *(selectStruct->getData());

      if (polyStruct.getDataType() != dataTypePoly)
      {
         continue;
      }

      DataStruct* emptyPolyStruct = getCamCadData().getNewDataStruct(polyStruct);
      emptyPolyStruct->getPolyList()->empty();

      CPolyList& polyList = *(polyStruct.getPolyList());
      CDataList& parentDataList = *(selectStruct->getParentDataList());

      for (POSITION polyPos = polyList.GetHeadPosition();polyPos != NULL;)
      {
         CPoly* poly = polyList.GetNext(polyPos);

         CPoly emptyPoly(*poly);
         emptyPoly.empty();
         emptyPoly.setFilled(false);
         emptyPoly.setClosed(false);

         CPnt* prevPnt = NULL;

         for (POSITION vertexPos = poly->getHeadVertexPosition();vertexPos != NULL;)
         {
            CPnt* pnt = poly->getNextVertex(vertexPos);

            if (prevPnt != NULL)
            {
               DataStruct* newPolyStruct = getCamCadData().getNewDataStruct(*emptyPolyStruct);
               parentDataList.AddTail(newPolyStruct);

               CPoly* newPoly = new CPoly(emptyPoly);
               newPolyStruct->getPolyList()->AddTail(newPoly);

               newPoly->addVertex(prevPnt->x,prevPnt->y,prevPnt->bulge);
               newPoly->addVertex(pnt->x,pnt->y);
            }

            prevPnt = pnt;
         }
      }

      delete emptyPolyStruct;

      //if (selectStruct->p->getDataType() == dataTypePoly)
      //{
      //   // add polygons to file polyline border is in
      //   FileStruct *file = Find_File(selectStruct->filenum);
      //   PrepareAddEntity(file);

      //   CPoly *poly;
      //   CPnt *pnt;
      //   double x1, y1, x2, y2, bulge;
      //   DataStruct *np;
      //   
      //   for (POSITION polyPos = selectStruct->p->getPolyList()->GetHeadPosition();polyPos != NULL;)
      //   {
      //      poly = selectStruct->p->getPolyList()->GetNext(polyPos);
      //      POSITION pntPos = poly->getPntList().GetHeadPosition();

      //      if (pntPos != NULL)
      //      {
      //         pnt = poly->getPntList().GetNext(pntPos);
      //         x2 = pnt->x;
      //         y2 = pnt->y;
      //         bulge = pnt->bulge;

      //         while (pntPos != NULL)
      //         {
      //            pnt = poly->getPntList().GetNext(pntPos);
      //            x1 = x2;
      //            y1 = y2;
      //            x2 = pnt->x;
      //            y2 = pnt->y;
      //            
      //            if (fabs(bulge) > SMALLNUMBER) // arc
      //            {
      //               double da, sa, r, cx, cy;
      //               da = atan(bulge) * 4;
      //               ArcPoint2Angle(x1, y1, x2, y2, da, &cx, &cy, &r, &sa);
      //               np = Graph_Arc(selectStruct->p->getLayerIndex(), cx, cy, r, sa, da, 0L,
      //                     poly->getWidthIndex(), selectStruct->p->isNegative());
      //            }
      //            else // line
      //            {
      //               np = Graph_Line(selectStruct->p->getLayerIndex(), x1, y1, x2, y2, 0L, poly->getWidthIndex(), selectStruct->p->isNegative());
      //            }

      //            np->setGraphicClass(selectStruct->p->getGraphicClass());
      //            np->getAttributesRef() = NULL;
      //            CopyAttribs(&np->getAttributesRef(), selectStruct->p->getAttributesRef());
      //            
      //            bulge = pnt->bulge;
      //         }
      //      }
      //   }

      RemoveOneEntityFromDataList(this, selectStruct->getParentDataList(), selectStruct->getData());
   }

   SelectList.RemoveAll();
   getSelectStack().empty();

   UpdateAllViews(NULL);
#endif
}
*/
/* OnUnlinkPolys **************************************************************/
/*void CCEtoODBDoc::OnUnlinkPolys() 
{
   CWaitCursor wait;

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }

   // are there any selected entities
   if (SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");

      return;
   }

   if (getActiveView()->getMeasure() != NULL)
   {
      getActiveView()->getMeasure()->setMeasureOn(false);
   }

   // loop selected entities
   SelectStruct* selectStruct;
   
   for (POSITION selectPos = SelectList.GetHeadPosition();selectPos != NULL;)
   {
      //POSITION selectStructPos = selectPos;
      selectStruct = SelectList.GetNext(selectPos);
      DataStruct* selectedData = selectStruct->getData();

      selectedData->setSelected(false);
      selectedData->setMarked(false);

      if (selectedData->getDataType() == T_POLY)
      {
         if (selectedData->getPolyList()->GetCount() >= 2)
         {

            // add polylines to file polyline is in
            FileStruct *file = Find_File(selectStruct->filenum);

            if ( file != NULL )
            {
               PrepareAddEntity(file);
            }

            CPolyList* selectedPolyList = selectedData->getPolyList();
            CDataList* selectedDataList = selectStruct->getParentDataList();
            POSITION selectedDataPos = selectedDataList->Find(selectedData);

            while (selectedPolyList->GetCount() > 1)
            {
               // remove the poly from the selectedPolyList
               CPoly* poly = selectedPolyList->removeTail();

               // create a new DataStruct and put the poly in it
               DataStruct* newData = getCamCadData().getNewDataStruct(dataTypePoly);
               newData->getPolyList()->AddTail(poly);

               // put the new DataStruct into the selectStruct's data list after the original data
               selectedDataList->InsertAfter(selectedDataPos,newData);
            }
         }
      }

      delete selectStruct;
   }

   SelectList.RemoveAll();
   getSelectStack().empty();

   UpdateAllViews(NULL);
}*/

/////////////////////////////////////////////////////////////////////////////
// SelectLayerDlg dialog
SelectLayerDlg::SelectLayerDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
   : CDialog(nIDTemplate, pParent)
   , m_copy(FALSE)
{
   //{{AFX_DATA_INIT(SelectLayerDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void SelectLayerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectLayerDlg)
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Check(pDX, IDC_CopyEntities, m_copy);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelectLayerDlg, CDialog)
   //{{AFX_MSG_MAP(SelectLayerDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectLayerDlg message handlers
BOOL SelectLayerDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      if (doc->getLayerArray()[i] == NULL)  continue;
      m_layerCB.SetItemData(m_layerCB.AddString(doc->getLayerArray()[i]->getName()), i);
   }
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectLayerDlg::OnOK() 
{
   int sel = m_layerCB.GetCurSel();
   if (sel == CB_ERR)
      return;

   layer = m_layerCB.GetItemData(sel);
   
   CDialog::OnOK();
}

// end CEDIT.CPP
