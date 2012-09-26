
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           

#include "stdafx.h"
#include "explode.h"
#include "graph.h"
#include "measure.h"
#include "attrib.h"
#include "crypt.h"
#include "EntityNumber.h"
#include "CCEtoODB.h"

extern CStatusBar *StatusBar; // from MAINFRM.CPP
                              
void CloseEditDlg();
void ResetButtons();
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);

/******************************************************************************
* OnExplodeBlocks
*/
void CCEtoODBDoc::OnExplodeBlocks() 
{
	
	StoreDocForImporting();

   CWaitCursor wait;

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE

   if (SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   StatusBar->SetWindowText("No UNDO available for Geometry Explode.");
   ClearUndo();
   CloseEditDlg();

   if (getActiveView()->getMeasure() != NULL)
   {
      getActiveView()->getMeasure()->setMeasureOn(false);
   }

   getSelectStack().empty();

   // loop through selected entities
   POSITION selectPos = SelectList.GetHeadPosition();
   while (selectPos)
   {
      POSITION prevPos = selectPos; // remember position of this node

      SelectStruct *s = SelectList.GetNext(selectPos); // get this node and advance position
      DrawEntity(s, 0, FALSE); // draw entity normal
      s->getData()->setSelected(false);
      s->getData()->setMarked(false); // unmark entity

      // if inserted block
      if (s->getData()->getDataType() == T_INSERT)
      {
         BlockStruct *block = getBlockAt(s->getData()->getInsert()->getBlockNumber()); // find inserted block

         if (((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE)) )
         {
            if (block->getShape() != T_COMPLEX)
               continue;

            block = getBlockAt((int)(block->getSizeA()));
         }

         POSITION dataPos = s->getParentDataList()->Find(s->getData()); // find insert in datalist

         // loop through datas in block
         POSITION insertPos = block->getDataList().GetHeadPosition();
         while (insertPos)
         {
            DataStruct *data = block->getDataList().GetNext(insertPos);
            DataStruct *newData = CopyTransposeEntity(data, s->getData()->getInsert()->getOriginX(), s->getData()->getInsert()->getOriginY(), 
                  s->getData()->getInsert()->getAngle(), s->getData()->getInsert()->getMirrorFlags(), 
                  s->getData()->getInsert()->getScale(), s->getData()->getLayerIndex(), TRUE);
            s->getParentDataList()->InsertBefore(dataPos, newData);

            if (data->getDataType() == T_INSERT)
            {
               BlockStruct *subblock = Find_Block_by_Num(data->getInsert()->getBlockNumber()); // find inserted block
               if (!((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE)) )
               {
                  SelectStruct *temp = new SelectStruct(*s);
                  //memcpy(temp, s, sizeof(SelectStruct));
                  temp->setData(newData);
                  temp->getData()->setMarked(true);
                  temp->getData()->setSelected(false);
                  SelectList.InsertBefore(prevPos, temp);
                  DrawEntity(temp, 2, FALSE);
               }
            }
         }

         RemoveOneEntityFromDataList(this, s->getParentDataList(), s->getData());
      }
   }
#endif
}

/******************************************************************************
*  ExplodeInsert
   datastruct is the insert to be exploded
   the datalist is the datalist, where the insert resides and where the data will end up.
*/
/*void ExplodeInsert(CCEtoODBDoc *doc, DataStruct *data, CDataList *dataList)
{
   if (data->getDataType() != T_INSERT)
      return;

   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
	if (block == NULL)
		return;


	// If the block is a complex aperture then explode the complex aperture. too
   if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE))
   {
      if (block->getShape() != T_COMPLEX)
         return;

      block = doc->getBlockAt((int)(block->getSizeA()));
		if (block == NULL)
			return;
   }


   POSITION dataPos = dataList->Find(data); // find insert in datalist

   // loop through datas in block
   POSITION insertPos = block->getDataList().GetHeadPosition();
   while (insertPos != NULL)
   {
      DataStruct *subData = block->getDataList().GetNext(insertPos);
      DataStruct *newData = CopyTransposeEntity(subData, data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 
            data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), 
            data->getInsert()->getScale(), data->getLayerIndex(), TRUE);
      dataList->InsertBefore(dataPos, newData);
   }

   RemoveOneEntityFromDataList(doc, dataList, data);
}*/

/************************************************************************************
* ExplodeAll 
*/
void CCEtoODBDoc::OnExplodeAll()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write) && !getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for Edit or RTN Write!");
      return;
   }*/

	BOOL can_explode = FALSE;
   can_explode = ErrorMessage("Are you sure you want to explode all geometries?", "Exploding all geometries", MB_YESNO | MB_DEFBUTTON2) == IDYES;
   

   if (!can_explode)
   {
      return;
   }

   UnselectAll(FALSE);

   POSITION filePos, dataPos, insertPos, tempPos;
   DataStruct *data, *oldData, *newData;
   FileStruct *file;

   StoreDocForImporting();

   filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      file = getFileList().GetNext(filePos);

      // loop datas in file block
      dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         data = file->getBlock()->getDataList().GetAt(dataPos);

         // explode inserts (place new data after insert so inserts inside inserts get exploded)
         if (data->getDataType() == T_INSERT)
         {
            BlockStruct *block = Find_Block_by_Num(data->getInsert()->getBlockNumber());

            if (!(block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE || 
                  block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL || block->getFlags() & BL_COMPLEX_TOOL))
            {
               // loop through datas in block
               // loop backwards because of InsertAfter
               insertPos = block->getDataList().GetTailPosition();
               while (insertPos != NULL)
               {
                  oldData = block->getDataList().GetPrev(insertPos);
                  newData = CopyTransposeEntity(oldData, data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 
                        data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex(), TRUE); 
                  file->getBlock()->getDataList().InsertAfter(dataPos, newData);
               }
      
               // advance datapos before removing INSERT node
               tempPos = dataPos;
               file->getBlock()->getDataList().GetNext(dataPos);
               file->getBlock()->getDataList().RemoveAt(tempPos);

               delete data;
            }
            else
               file->getBlock()->getDataList().GetNext(dataPos);
         }
         else
            file->getBlock()->getDataList().GetNext(dataPos);
      }
   }
}

/******************************************************************************
* CCEtoODBView::OnGenerateBlock
*/
void CCEtoODBView::OnGenerateBlock() 
{
   if (getActiveView()->getMeasure() != NULL)
   {
      getActiveView()->getMeasure()->setMeasureOn(false);
   }

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == GenerateBlock)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   CCEtoODBDoc *doc = GetDocument();

   if (doc->SelectList.IsEmpty())
   {
      if (StatusBar)
         StatusBar->SetWindowText("Nothing is selected or marked!");
      return;
   }

   // check that all entities are in same file
   int filenum = doc->SelectList.GetHead()->filenum;
   SelectStruct *s;
   POSITION pos = doc->SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      s = doc->SelectList.GetNext(pos);
      if (s->filenum != filenum)
      {
         ErrorMessage("Entities can not be in different files!");
         return;
      }
   }

   //HideSearchCursor();
   cursorMode = GenerateBlock;
   //ShowSearchCursor();

   if (StatusBar)
      StatusBar->SetWindowText("Set the Geometry's Insertion Point");
}

void CCEtoODBView::OnLButtonDown_GenerateBlock(CDC *dc, CPoint *pnt)
{
   CCEtoODBDoc *doc = GetDocument();

   if (doc->SelectList.IsEmpty())
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }
   
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord);

   // use first item's file data
   SelectStruct *s = doc->SelectList.GetHead();

   doc->StoreDocForImporting();

   GenerateBlockDlg dlg;
   static int geomnum = 1;
   dlg.m_name.Format("NewGeometry_%d", geomnum++);
   dlg.doc = doc;
   dlg.filenum = s->filenum;
   if (dlg.DoModal() != IDOK)
   {
      // turn off GenerateBlock Cursor
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   // generate a new block
   BlockStruct *block;
   block = Graph_Block_On(GBO_RETURN, dlg.m_name, s->filenum, 0);
   if (block == NULL)
   {
      // turn off GenerateBlock Cursor
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //
   Point2 temp;
   temp.x = factor * pnt->x;
   temp.y = factor * pnt->y;
   Untransform(doc, &temp, s);

   // add an insert of the block
   DataStruct *data = doc->getCamCadData().getNewDataStruct(dataTypeInsert);
   data->getAttributesRef()= NULL;  
   //data->setEntityNumber(CEntityNumber::allocate());
   //data->setDataType(dataTypeInsert);
   data->setLayerIndex(s->getData()->getLayerIndex());
   data->setSelected(false);
   data->setMarked(false);
   data->setNegative(false);
   data->setGraphicClass(graphicClassNormal);
   data->setFlags(0);
   data->setColorOverride(false);
   data->setHidden(false);

   //data->getInsert() = new InsertStruct;
   data->getInsert()->getShadingRef().On = FALSE;
   data->getInsert()->setOriginX(temp.x);
   data->getInsert()->setOriginY(temp.y);
   data->getInsert()->setScale(1.0);
   data->getInsert()->setAngle(0.);
   data->getInsert()->setInsertType(insertTypeUnknown);
   data->getInsert()->setPlacedBottom(false);
   data->getInsert()->setMirrorFlags(0);
   data->getInsert()->setRefname(NULL);
   data->getInsert()->setBlockNumber(block->getBlockNumber());
   s->getParentDataList()->InsertBefore(s->getParentDataList()->Find(s->getData()), data);

   // move entities and add to block
   POSITION dataPos = doc->SelectList.GetHeadPosition();
   while (dataPos)
   {
      s = doc->SelectList.GetNext(dataPos);

      // draw entities normal
      doc->DrawEntity(s, 0, FALSE);

      switch (s->getData()->getDataType())
      {
      case T_POLY:
         {
            POSITION polyPos, pntPos;
            CPoly *poly;
            CPnt *pnt;

            polyPos = s->getData()->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = s->getData()->getPolyList()->GetNext(polyPos);
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);

                  pnt->x -= (DbUnit)temp.x;        
                  pnt->y -= (DbUnit)temp.y;        
               }
            }
         }
         break;
      case T_TEXT:
         s->getData()->getText()->incPntX(-(DbUnit)temp.x);         
         s->getData()->getText()->incPntY(-(DbUnit)temp.y);         
         break;
      case T_INSERT:
         s->getData()->getInsert()->incOriginX(-temp.x);       
         s->getData()->getInsert()->incOriginY(-temp.y);       
         break;
      }

      s->getData()->setMarked(false);
      s->getData()->setSelected(false);
      s->getParentDataList()->RemoveAt(s->getParentDataList()->Find(s->getData()));
      block->getDataList().AddTail(s->getData());
      delete s;
   }

   // remove all entities from SelectList;
   doc->SelectList.RemoveAll();
   doc->getSelectStack().empty();
   doc->ClearUndo();

   // turn off GenerateBlock Cursor
   //HideSearchCursor();
   cursorMode = Search;
   //ShowSearchCursor();
}

/////////////////////////////////////////////////////////////////////////////
// GenerateBlockDlg dialog
GenerateBlockDlg::GenerateBlockDlg(CWnd* pParent /*=NULL*/)
   : CDialog(GenerateBlockDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(GenerateBlockDlg)
   m_name = _T("");
   //}}AFX_DATA_INIT
}

void GenerateBlockDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GenerateBlockDlg)
   DDX_Text(pDX, IDC_EDIT1, m_name);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GenerateBlockDlg, CDialog)
   //{{AFX_MSG_MAP(GenerateBlockDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GenerateBlockDlg message handlers
void GenerateBlockDlg::OnOK() 
{
   UpdateData();

   if (Graph_Block_Exists(doc, m_name, filenum) != NULL)
   {
      ErrorMessage("A geometry with this name already exisits", m_name);
      return;
   }
   
   CDialog::OnOK();
}
