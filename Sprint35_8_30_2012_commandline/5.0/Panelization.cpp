
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

// PANELIZATION.CPP

#include "stdafx.h"
#include "panelization.h"
#include "tmstate.h"
#include "graph.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void CCEtoODBDoc::OnPanel() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   // saves a reference to the doc for graph functions
   StoreDocForImporting();

   CPanelization panelInfo(this);
   CPanelizationPropertySheet panelizationPropertySheet(panelInfo,*this);
   panelizationPropertySheet.DoModal();
}

//_____________________________________________________________________________
CPanelizationPropertySheet::CPanelizationPropertySheet(CPanelization& panelInfo,CCEtoODBDoc& camCadDoc) :
   CPropertySheet("Panelization"),
   m_manualPage(&panelInfo,&camCadDoc),
   m_arrayPage(&panelInfo,&camCadDoc)
{
   AddPage(&m_manualPage);
   AddPage(&m_arrayPage);

   m_psh.dwFlags |= PSH_NOAPPLYNOW;
   m_psh.dwFlags &= ~PSH_HASHELP;
}

BOOL CPanelizationPropertySheet::OnInitDialog()
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   CWnd *pWnd = GetDlgItem(IDCANCEL);
   if (pWnd != NULL)
      pWnd->SetWindowText("Close");

   return bResult;
}

/******************************************************************************
* CBoardInstance
/******************************************************************************
* CBoardInstance::CBoardInstance
*/
CBoardInstance::CBoardInstance(CCEtoODBDoc *doc, DataStruct *board)
{
   m_pDoc = doc;
   m_pBoardData = board;
   m_pBoard = m_pDoc->getBlockAt(m_pBoardData->getInsert()->getBlockNumber());
   m_lastLocationSetBy = LOCATION_BY_ORIGIN;

   m_boardExtents = getBoardOutlineExtents();
   m_boardCenter = m_boardExtents.getCenter();
   //extentsOffset.x = m_boardExtents.getXmin();
   //extentsOffset.y = m_boardExtents.getYmin();
}

/******************************************************************************
* //CBoardInstance::~CBoardInstance
*/
CBoardInstance::~CBoardInstance()
{
   if (m_pBoardData != NULL)
      m_pBoardData->setColorOverride(false);
}

/******************************************************************************
* CBoardInstance::DeleteFromCamcad
*/
void CBoardInstance::DeleteFromCamcad(CDataList *dataList)
{
   m_pBoardData->setColorOverride(false);

   POSITION pos = dataList->Find(m_pBoardData);
   if (pos)
   {
      dataList->RemoveAt(pos);
      delete m_pBoardData;
      m_pBoardData = NULL;
   }
}

/******************************************************************************
* CBoardInstance::Draw
*/
void CBoardInstance::Draw(double x, double y, double rotation, int mirror, double scale, COLORREF color, CDataList *dataList)
{
   SelectStruct *s = new SelectStruct;
   s->setData(m_pBoardData);
   s->setParentDataList(dataList);
   s->filenum = m_pBoard->getFileNumber();
   s->insert_x = (DbUnit)x;
   s->insert_y = (DbUnit)y;
   s->rotation = (DbUnit)rotation;
   s->mirror = mirror;
   s->scale = (DbUnit)scale;
   s->layer = m_pBoardData->getLayerIndex();

   if (color)
   {
      m_pBoardData->setColorOverride(true);
      m_pBoardData->setOverrideColor(color);
   }
   else
      m_pBoardData->setColorOverride(false);

   m_pDoc->DrawEntity(s, 0, TRUE);

   delete s;
}

/******************************************************************************
* CBoardInstance::ClearColor
*/
void CBoardInstance::ClearColor()
{
   m_pBoardData->setColorOverride(false);
}

/******************************************************************************
* CBoardInstance::recalcuateLocationFromCenter
*/
void CBoardInstance::recalcuateLocationFromCenter(double rotation, int mirror, double &x, double &y)
{
   double boardRotation = NormalizeAngle(rotation);

   if (fabs(boardRotation - DegToRad(0.0)) < SMALLANGLE)
   {
      x -= m_boardCenter.x;
      y -= m_boardCenter.y;

      if (mirror & MIRROR_ALL)
         x += m_boardCenter.x * 2.0;
   }
   else if (fabs(boardRotation - DegToRad(90.0)) < SMALLANGLE)
   {
      x += m_boardCenter.y;
      y -= m_boardCenter.x;

      if (mirror & MIRROR_ALL)
         y += m_boardCenter.x * 2.0;
   }
   else if (fabs(boardRotation - DegToRad(180.0)) < SMALLANGLE)
   {
      x += m_boardCenter.x;
      y += m_boardCenter.y;

      if (mirror & MIRROR_ALL)
         x -= m_boardCenter.x * 2.0;
   }
   else if (fabs(boardRotation - DegToRad(270.0)) < SMALLANGLE)
   {
      x -= m_boardCenter.y;
      y += m_boardCenter.x;

      if (mirror & MIRROR_ALL)
         y -= m_boardCenter.x * 2.0;
   }
}

/******************************************************************************
* CBoardInstance::getBoardOutline
*/
DataStruct *CBoardInstance::getBoardOutline()
{
   if (m_pBoard == NULL)
      return NULL;

   DataStruct *boardOutline = NULL;

   POSITION pos = m_pBoard->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = m_pBoard->getDataList().GetNext(pos);

      if (data->getDataType() == T_POLY && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
      {
         boardOutline = data;
         break;
      }
   }

   return boardOutline;
}

/******************************************************************************
* CBoardInstance::getBoardOutlineExtents
*/
CExtent CBoardInstance::getBoardOutlineExtents()
{
   DataStruct *boardOutline = getBoardOutline();

   if (boardOutline == NULL)
      return CExtent(0.0, 0.0, 0.0, 0.0);

   return boardOutline->getPolyList()->getExtent();
}

/******************************************************************************
* CBoardInstance::SetBoard
*/
void CBoardInstance::SetBoard(BlockStruct *block)
{
   if (block == NULL)
      return;

   m_pBoard = block;
   m_pBoardData->getInsert()->setBlockNumber(m_pBoard->getBlockNumber());
   m_boardExtents = getBoardOutlineExtents();
   m_boardCenter = m_boardExtents.getCenter();
}

/******************************************************************************
* CBoardInstance::GetBoardInstName
*/
CString CBoardInstance::GetBoardInstName()
{
   return m_pBoardData->getInsert()->getRefname();
}

void CBoardInstance::SetBoardInstName(CString instName)
{
   char *newName = NULL;

   if (!instName.IsEmpty())
      newName = STRDUP(instName);

   m_pBoardData->getInsert()->setRefname(STRDUP(newName));
}

/******************************************************************************
* CBoardInstance::GetXSize
*/
double CBoardInstance::GetXSize()
{
   return m_boardExtents.getXsize();
}

/******************************************************************************
* CBoardInstance::GetYSize
*/
double CBoardInstance::GetYSize()
{
   return m_boardExtents.getYsize();
}

/******************************************************************************
* CBoardInstance::GetBoardName
*/
CString CBoardInstance::GetBoardName()
{
   return m_pBoard->getName();
}

/******************************************************************************
* CBoardInstance::GetCenterPoint
*/
void CBoardInstance::GetCenterPoint(double &x, double &y)
{
   if (m_pBoardData == NULL)
      return;

   x = m_pBoardData->getInsert()->getOriginX();
   y = m_pBoardData->getInsert()->getOriginY();
   int mirror = m_pBoardData->getInsert()->getMirrorFlags();
   double boardRotation = NormalizeAngle(m_pBoardData->getInsert()->getAngle());
   if (fabs(boardRotation - DegToRad(0.0)) < SMALLANGLE)
   {
      x += (DbUnit)m_boardCenter.x;
      y += (DbUnit)m_boardCenter.y;

      if (mirror & MIRROR_ALL)
         x -= (DbUnit)m_boardCenter.x * 2.0;
   }
   else if (fabs(boardRotation - DegToRad(90.0)) < SMALLANGLE)
   {
      x -= (DbUnit)m_boardCenter.y;
      y += (DbUnit)m_boardCenter.x;

      if (mirror & MIRROR_ALL)
         y -= (DbUnit)m_boardCenter.x * 2.0;
   }
   else if (fabs(boardRotation - DegToRad(180.0)) < SMALLANGLE)
   {
      x -= (DbUnit)m_boardCenter.x;
      y -= (DbUnit)m_boardCenter.y;

      if (mirror & MIRROR_ALL)
         x += (DbUnit)m_boardCenter.x * 2.0;
   }
   else if (fabs(boardRotation - DegToRad(270.0)) < SMALLANGLE)
   {
      x += (DbUnit)m_boardCenter.y;
      y -= (DbUnit)m_boardCenter.x;

      if (mirror & MIRROR_ALL)
         y += (DbUnit)m_boardCenter.x * 2.0;
   }
}

/******************************************************************************
* CBoardInstance::GetLastSetLocationBy
*/
short CBoardInstance::GetLastSetLocationBy()
{
   return m_lastLocationSetBy;
}

/******************************************************************************
* CBoardInstance::GetLocation
*/
bool CBoardInstance::GetLocation(CPoint2d &location)
{
   if (m_pBoardData == NULL)
      return false;
   
   location = m_pBoardData->getInsert()->getOrigin2d();
   return true;
}

/******************************************************************************
* CBoardInstance::GetLocation
*/
bool CBoardInstance::GetLocation(double &x, double &y)
{
   if (m_pBoardData == NULL)
      return false;

   x = m_pBoardData->getInsert()->getOriginX();
   y = m_pBoardData->getInsert()->getOriginY();
   return true;
}

/******************************************************************************
* CBoardInstance::SetLocation
*/
void CBoardInstance::SetLocation(double x, double y, double rotation, bool mirror, bool aboutCenter)
{
   if (m_pBoardData == NULL)
      return;

   m_lastLocationSetBy = aboutCenter?LOCATION_BY_CENTER:LOCATION_BY_ORIGIN;

   m_pBoardData->getInsert()->setOriginX(x);
   m_pBoardData->getInsert()->setOriginY(y);
   m_pBoardData->getInsert()->setAngle(rotation);
   m_pBoardData->getInsert()->setMirrorFlags(mirror?MIRROR_ALL:0);
   m_pBoardData->getInsert()->setPlacedBottom(mirror);

   // calculate actual location
   if (aboutCenter)
   {
      double insX = x, insY = y;
      recalcuateLocationFromCenter(m_pBoardData->getInsert()->getAngle(), m_pBoardData->getInsert()->getMirrorFlags(), insX, insY);
      m_pBoardData->getInsert()->setOriginX(insX);
      m_pBoardData->getInsert()->setOriginY(insY);
   }
}

/******************************************************************************
* CBoardInstance::TranslateLocation
*/
void CBoardInstance::TranslateLocation(double x, double y)
{
   m_pBoardData->getInsert()->incOriginX(x);
   m_pBoardData->getInsert()->incOriginY(y);
}

//void CBoardInstance::GetBoardOffset(double &xOffset, double &yOffset)
//{
// xOffset = extentsOffset.x;
// yOffset = extentsOffset.y;
//}

/******************************************************************************
* CBoardInstance::GetRotation
*/
double CBoardInstance::GetRotation()
{
   if (m_pBoardData == NULL)
      return 0.0;

   return m_pBoardData->getInsert()->getAngle();
}

/******************************************************************************
* CBoardInstance::GetMirror
*/
bool CBoardInstance::GetMirror()
{
   if (m_pBoardData == NULL)
      return false;

   return (m_pBoardData->getInsert()->getMirrorFlags() & MIRROR_ALL)?true:false;
}

/******************************************************************************
* CBoardInstance::IsInExtents
*/
bool CBoardInstance::IsInExtents(CExtent extent)
{
   if (m_pBoardData == NULL)
      return false;

   double x, y;
   x = m_pBoardData->getInsert()->getOriginX();
   y = m_pBoardData->getInsert()->getOriginY();
   CExtent curExtents(m_boardExtents);
   if (m_pBoardData->getInsert()->getMirrorFlags() & MIRROR_ALL)
      curExtents.translate(-(curExtents.getXmax() + curExtents.getXmin()), 0.0);

   CTMatrix m;
   m.rotateRadians(m_pBoardData->getInsert()->getAngle());
   m.translate(x, y);
   curExtents.transform(m);

   //{
   // CString buf;
   // CPoint2d center = curExtents.getCenter();
   // buf.Format("Width=%.3f\nHeight=%.3f\nCenter=(%.3f , %.3f)", curExtents.getXsize(), curExtents.getYsize(), center.x, center.y);
   // MessageBox(NULL, buf, "Panalization", MB_OK);
   //}

   return extent.contains(curExtents,SMALLNUMBER);
}




/******************************************************************************
* CPanelInstance
*/
/******************************************************************************
* CPanelInstance::CPanelInstance
*/
CPanelInstance::CPanelInstance(CCEtoODBDoc *doc, FileStruct *panel)
{
   m_pDoc = doc;
   m_pPanelFile = panel;
   validPanel = false;

   if (m_pPanelFile == NULL)
      return;

   POSITION pos = m_pPanelFile->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = m_pPanelFile->getBlock()->getDataList().GetNext(pos);
      m_panelExtents = getPanelOutlineExtents();

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCB)
      {
         CBoardInstance *boardInst = new CBoardInstance(m_pDoc, data);
         
         boards.AddTail(boardInst);
      }
   }
}

/******************************************************************************
* CPanelInstance::~CPanelInstance
*/
CPanelInstance::~CPanelInstance()
{
   // delete all board instances in this panel instance
   ClearAll();
}

/******************************************************************************
* CPanelInstance::ClearAll
*/
void CPanelInstance::ClearAll(bool removeInCAMCAD)
{
   // remove all the boards
   POSITION pos = boards.GetHeadPosition();
   while (pos)
   {
      CBoardInstance *boardInst = boards.GetNext(pos);

      // remove the board from the panel
      if (removeInCAMCAD)
         boardInst->DeleteFromCamcad(&(m_pPanelFile->getBlock()->getDataList()));

      delete boardInst;
   }
   boards.RemoveAll();
}

/******************************************************************************
* CPanelInstance::RemoveBoard
*/
void CPanelInstance::RemoveBoard(CBoardInstance *boardInst, bool removeInCAMCAD)
{
   if (boardInst == NULL)
      return;

   POSITION pos = boards.Find(boardInst);
   if (!pos)
      return;

   // remove the board instance
   boards.RemoveAt(pos);

   // remove the board from the panel
   if (removeInCAMCAD)
      boardInst->DeleteFromCamcad(&(m_pPanelFile->getBlock()->getDataList()));

   delete boardInst;
}

/******************************************************************************
* CPanelInstance::IsEqual
*/
bool CPanelInstance::IsEqual(FileStruct *file)
{
   return (file == m_pPanelFile);
}

/******************************************************************************
* CPanelInstance::DeleteFromCamcad
*/
void CPanelInstance::DeleteFromCamcad(CFileList *fileList)
{
   //POSITION pos = m_pDoc->getFileList().Find(m_pPanelFile);
   //if (pos)
   //{
   //   fileList->RemoveAt(pos);

   //   delete m_pPanelFile;
   //}

   m_pDoc->getFileList().deleteFile(m_pPanelFile);
   m_pPanelFile = NULL;
}

/******************************************************************************
* CPanelInstance::DrawBoard
*/
void CPanelInstance::DrawBoard(CBoardInstance *boardInst, COLORREF color)
{
   if (boardInst == NULL)
      return;

   if (m_pPanelFile == NULL)
      return;

   double fileX = m_pPanelFile->getInsertX();
   double fileY = m_pPanelFile->getInsertY();
   double fileRot = m_pPanelFile->getRotation();
   double fileScale = m_pPanelFile->getScale();
   int fileMirror = (m_pDoc->getBottomView() ? MIRROR_FLIP : 0) ^ (m_pPanelFile->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);
   if (m_pDoc->getBottomView())
   {
      fileX = -fileX;
      fileRot = -fileRot;
   }

   boardInst->Draw(fileX, fileY, fileRot, fileMirror, fileScale, color, &(m_pPanelFile->getBlock()->getDataList()));
}

/******************************************************************************
* CPanelInstance::Validate
*/
bool CPanelInstance::Validate()
{
   validPanel = true;

   POSITION pos = boards.GetHeadPosition();
   while (pos)
   {
      CBoardInstance *boardInst = boards.GetNext(pos);

      if (!boardInst->IsInExtents(m_panelExtents))
      {
         validPanel = false;
         DrawBoard(boardInst, RGB(255, 0, 0));
      }
   }
   
   return validPanel;
}

/******************************************************************************
* CPanelInstance::ClearValidate
*/
void CPanelInstance::ClearValidate()
{
   POSITION pos = boards.GetHeadPosition();
   while (pos)
   {
      CBoardInstance *boardInst = boards.GetNext(pos);

      boardInst->ClearColor();
   }

   validPanel = false;
}

/******************************************************************************
* CPanelInstance::isValid
*/
bool CPanelInstance::isValid()
{
   return validPanel;
}

/******************************************************************************
* CPanelInstance::getPanelOutline
*/
DataStruct *CPanelInstance::getPanelOutline()
{
   if (m_pPanelFile == NULL)
      return NULL;

   DataStruct *panelOutline = NULL;

   POSITION pos = m_pPanelFile->getBlock()->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = m_pPanelFile->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() == T_POLY && data->getGraphicClass() == GR_CLASS_PANELOUTLINE)
      {
         panelOutline = data;
         break;
      }
   }

   return panelOutline;
}

/******************************************************************************
* CPanelInstance::getPanelOutlineLayer
*/
int CPanelInstance::getPanelOutlineLayer()
{
   // find the layer for panel outlines
   LayerStruct *layer = NULL;
   for (int i=0; i<m_pDoc->getMaxLayerIndex(); i++)
   {
      layer = m_pDoc->getLayerArray()[i];

      if (layer && layer->getLayerType() == LAYTYPE_PANEL_OUTLINE)
         break;

      layer = NULL;
   }

   // create the layer if we need to
   if (layer == NULL)
   {
      layer = m_pDoc->Add_Layer("PANELOUTLINE");
      layer->setLayerType(LAYTYPE_PANEL_OUTLINE);
      layer->setVisible(true);
   }
      
   return layer->getLayerIndex();
}

/******************************************************************************
* CPanelInstance::getPanelOutlineExtents
*/
CExtent CPanelInstance::getPanelOutlineExtents()
{
   DataStruct *panelOutline = getPanelOutline();

   if (panelOutline == NULL)
      return CExtent(0.0, 0.0, 0.0, 0.0);

   return panelOutline->getPolyList()->getExtent();
}

/******************************************************************************
* CPanelInstance::GetPanelName
*/
CString CPanelInstance::GetPanelName()
{
   return m_pPanelFile->getName();
}

/******************************************************************************
* CPanelInstance::SetPanelName
*/
void CPanelInstance::SetPanelName(CString panelName)
{
   m_pPanelFile->setName(panelName);
}

/******************************************************************************
* CPanelInstance::GetBoard
*/
CBoardInstance *CPanelInstance::GetBoard(CString boardName)
{
   POSITION pos = boards.GetHeadPosition();
   while (pos)
   {
      CBoardInstance *boardInst = boards.GetNext(pos);

      if (boardName.Compare(boardInst->GetBoardName()))
         return boardInst;
   }

   return NULL;
}

/******************************************************************************
* CPanelInstance::GetFirstBoard
*/
CBoardInstance *CPanelInstance::GetFirstBoard()
{
   if (boards.IsEmpty())
      return NULL;

   return boards.GetHead();
}

/******************************************************************************
* CPanelInstance::GetNextBoard
*/
CBoardInstance *CPanelInstance::GetNextBoard(CBoardInstance *&board)
{
   POSITION pos = boards.Find(board);
   if (!pos)
      return NULL;

   board = boards.GetNext(pos);
   board = (pos != NULL)?boards.GetNext(pos):NULL;

   return board;
}

/******************************************************************************
* CPanelInstance::Show
*/
void CPanelInstance::Show(bool show)
{
   m_pPanelFile->setShow(show);

   m_pDoc->OnFitPageToImage();
}

/******************************************************************************
* CPanelInstance::GetXSize
*/
double CPanelInstance::GetXSize()
{
   return m_panelExtents.getXsize();
}

/******************************************************************************
* CPanelInstance::GetYSize
*/
double CPanelInstance::GetYSize()
{
   return m_panelExtents.getYsize();
}

/******************************************************************************
* CPanelInstance::SetSize
*/
void CPanelInstance::SetSize(double xSize, double ySize, double xOrigin, double yOrigin, int widthIndex)
{
   DataStruct *panelOutline = getPanelOutline();

   // create a new poly for the panel outline if we need to
   if (panelOutline == NULL)
   {
      panelOutline = m_pDoc->getCamCadData().getNewDataStruct(dataTypePoly);
      //panelOutline->getPolyList() = new CPolyList();

      m_pPanelFile->getBlock()->getDataList().AddHead(panelOutline);

      //panelOutline->setDataType(dataTypePoly);
      panelOutline->setGraphicClass(GR_CLASS_PANELOUTLINE);
      panelOutline->setLayerIndex(getPanelOutlineLayer());
   }

   // remove all polies in the poly list
   //POSITION polyPos = panelOutline->getPolyList()->GetHeadPosition();
   //while (polyPos)
   //   delete panelOutline->getPolyList()->GetNext(polyPos);
   panelOutline->getPolyList()->empty();

   // create new panel outline
   CPoly *poly = new CPoly();
   if (widthIndex >= 0)
      poly->setWidthIndex(widthIndex);

   panelOutline->getPolyList()->AddTail(poly);

   poly->addVertex(xOrigin, yOrigin);
   poly->addVertex(xOrigin+xSize, yOrigin);
   poly->addVertex(xOrigin+xSize, yOrigin+ySize);
   poly->addVertex(xOrigin, yOrigin+ySize);
   poly->close();

   m_panelExtents = panelOutline->getPolyList()->getExtent();

   m_pDoc->OnFitPageToImage();
}

/******************************************************************************
* CPanelInstance::SetOrigin
*/
bool CPanelInstance::SetOrigin(double xOrigin, double yOrigin)
{
   DataStruct *panelOutline = getPanelOutline();

   // create a new poly for the panel outline if we need to
   if (panelOutline == NULL)
      return false;

   CPoly *poly = (CPoly*)panelOutline->getPolyList()->GetHead();
   if (m_panelExtents.isValid())
      SetSize(m_panelExtents.getXsize(), m_panelExtents.getYsize(), xOrigin, yOrigin, poly->getWidthIndex());

   return true;
}

/******************************************************************************
* CPanelInstance::CreateNewBoard
*/
CBoardInstance *CPanelInstance::CreateNewBoard(CString boardName, double x, double y, double rotation, bool mirror, bool aboutCenter, BlockStruct *boardToAdd)
{
   DataStruct *m_pBoardData = m_pDoc->getCamCadData().getNewDataStruct(dataTypeInsert);
   //m_pBoardData->getInsert() = new InsertStruct();
   //m_pBoardData->setDataType(dataTypeInsert);
   m_pBoardData->getInsert()->setRefname(STRDUP(boardName));
   m_pBoardData->getInsert()->setInsertType(insertTypePcb);
   m_pBoardData->getInsert()->setBlockNumber(boardToAdd->getBlockNumber());
   m_pPanelFile->getBlock()->getDataList().AddTail(m_pBoardData);

   CBoardInstance *boardInst = new CBoardInstance(m_pDoc, m_pBoardData);

   boardInst->SetLocation(x, y, rotation, mirror, aboutCenter);

   boards.AddTail(boardInst);

   return boardInst;
}

/******************************************************************************
* CPanelInstance::AddBoardArray
*/
bool CPanelInstance::AddBoardArray(CString baseBoardName, int xCount, int yCount, double rotation, bool mirror, bool aboutCenter, double xOffset, double yOffset, double xValue, double yValue, int spacingType, BlockStruct *boardToAdd)
{
   if (boardToAdd == NULL)
      return false;

   int boardCount = boards.GetCount()+1;
   CString boardName;
   
   for (int row=0; row<yCount; row++)
   {
      for (int col=0; col<xCount; col++)
      {
         boardName.Format("%s_%d", baseBoardName, boardCount);
         CBoardInstance *boardInst = CreateNewBoard(boardName, 0.0, 0.0, rotation, mirror, aboutCenter, boardToAdd);
         
         if (boardInst == NULL)
            continue;

         double x, y;
         switch (spacingType)
         {
         case SEPTYPE_SPACING:
            {
               double boardXSize = boardInst->GetXSize(), boardYSize = boardInst->GetYSize();

               x = xOffset + ((boardXSize + xValue) * col);
               y = yOffset + ((boardYSize + yValue) * row);
               if (fabs(RadToDeg(rotation) - 90.0) < SMALLANGLE || fabs(RadToDeg(rotation) - 270.0) < SMALLANGLE)
               {
                  x = xOffset + ((boardYSize + xValue) * col);
                  y = yOffset + ((boardXSize + yValue) * row);
               }
            }
            break;
         case SEPTYPE_OFFSET:
            {
               x = xOffset + (xValue * col);
               y = yOffset + (yValue * row);
               if (fabs(RadToDeg(rotation) - 90.0) < SMALLANGLE || fabs(RadToDeg(rotation) - 270.0) < SMALLANGLE)
               {
                  x = xOffset + (xValue * col);
                  y = yOffset + (yValue * row);
               }
            }
            break;
         }
         boardInst->SetLocation(x, y, rotation, mirror, aboutCenter);

         boardCount++;
      }
   }

   return true;
}

/******************************************************************************
* CPanelInstance::ApplyToDocPanel
*/
void CPanelInstance::ApplyToDocPanel()
{
   // TODO: apply panel to document
}



/******************************************************************************
* CPanelization
/******************************************************************************
* CPanelization::CPanelization
*/
CPanelization::CPanelization(CCEtoODBDoc *doc)
{
   m_pDoc = doc;
   pCurPanelInst = NULL;
   pLastVisiblePanel = NULL;
   pLastVisibleFile = NULL;

   m_pDoc->OnRegen();
   m_pDoc->getSelectStack().empty();

   getPanelsFromCAMCAD();

   m_pDoc->OnFitPageToImage();
}

/******************************************************************************
* CPanelization::~CPanelization
*/
CPanelization::~CPanelization()
{
   // hide all the files except for the last visible
   POSITION pos = m_pDoc->getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = m_pDoc->getFileList().GetNext(pos);

      file->setShow(false);
   }

   if (pLastVisiblePanel != NULL && pLastVisiblePanel->isValid())
      pLastVisiblePanel->Show(true);
   else if (pLastVisibleFile != NULL)
      pLastVisibleFile->setShow(true);

   // delete all panel instances in panelization
   ClearAll();

   m_pDoc->OnFitPageToImage();
}

/******************************************************************************
* CPanelization::ClearAll
*/
void CPanelization::ClearAll(bool removeInCAMCAD)
{
   // remove all the panels
   POSITION pos = panels.GetHeadPosition();
   while (pos)
   {
      CPanelInstance *panelInst = panels.GetNext(pos);

      // remove the board from the panel
      if (removeInCAMCAD)
      {
         // before deleting the panel, make sure the last visible file was not the panel we're deleting
         if (panelInst == pLastVisiblePanel)
            pLastVisiblePanel = NULL;
         if (panelInst->IsEqual(pLastVisibleFile))
            pLastVisibleFile = NULL;

         panelInst->DeleteFromCamcad(&(m_pDoc->getFileList()));
      }

      delete panelInst;
   }
   panels.RemoveAll();
}

/******************************************************************************
* CPanelization::RemovePanel
*/
void CPanelization::RemovePanel(CPanelInstance *panelInst, bool removeInCAMCAD)
{
   POSITION pos = panels.Find(panelInst);
   if (!pos)
      return;

   // remove the board instance
   panels.RemoveAt(pos);

   // remove the board from the panel
   if (removeInCAMCAD)
   {
      // before deleting the panel, make sure the last visible file was not the panel we're deleting
      if (panelInst == pLastVisiblePanel)
         pLastVisiblePanel = NULL;
      if (panelInst->IsEqual(pLastVisibleFile))
         pLastVisibleFile = NULL;

      panelInst->DeleteFromCamcad(&(m_pDoc->getFileList()));
   }

   if (pCurPanelInst == panelInst)
      pCurPanelInst = NULL;

   delete panelInst;
}

/******************************************************************************
* CPanelization::ValidateCurPanel
*/
bool CPanelization::ValidateCurPanel()
{
   if (pCurPanelInst == NULL)
      return true;

   if (pCurPanelInst->Validate())
   {
      pLastVisiblePanel = pCurPanelInst;
      return true;
   }

   return false;
}

/******************************************************************************
* CPanelization::ClearCurPanelValidation
*/
void CPanelization::ClearCurPanelValidation()
{
   if (pCurPanelInst == NULL)
      return;

   pCurPanelInst->ClearValidate();
}

/******************************************************************************
* CPanelization::getPanelsFromCAMCAD
*/
void CPanelization::getPanelsFromCAMCAD()
{
   POSITION pos = m_pDoc->getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = m_pDoc->getFileList().GetNext(pos);

      if (file->isShown())
         pLastVisibleFile = file;

      if (file->getBlockType() == blockTypePanel)
      {
         CPanelInstance * pPanelInst = new CPanelInstance(m_pDoc, file);

         // if pLastVisibleFile is this panel, set it as the current
         if (pLastVisibleFile != NULL && pPanelInst->IsEqual(pLastVisibleFile))
            pCurPanelInst = pPanelInst;
         else
            file->setShow(false);

         if (file->isShown())
            pLastVisiblePanel = pPanelInst;

         // add the panel to the list of panels
         panels.AddTail(pPanelInst);
      }
      else
         file->setShow(false);
   }
}

/******************************************************************************
* CPanelization::CreateNewPanel
*/
CPanelInstance *CPanelization::CreateNewPanel(CString panelName, double width, double height, double xOrigin, double yOrigin)
{
   if (width <= 0.0 || height <= 0.0)
      return NULL;

   if (pCurPanelInst != NULL)
      pCurPanelInst->Show(false);

   // panel aperture
   int widthIndex = Graph_Aperture("panel", T_ROUND, 0.01, 0.0,
         0.0, 0.0, 0.0, 0, 0L, FALSE, NULL); 

   // create the new panel
   FileStruct *panelFile = m_pDoc->Add_File(panelName, -1);
   panelFile->setBlockType(blockTypePanel);
   panelFile->getBlock()->setBlockType(blockTypePanel);
   panelFile->setCadSourceFormat(fileTypeCamcadUser);  // user is interactively creating panel data using camcad
   panelFile->setNotPlacedYet(false);

   // Create the Panel Instance
   pCurPanelInst = new CPanelInstance(m_pDoc, panelFile);
   pCurPanelInst->SetSize(width, height, xOrigin, yOrigin, widthIndex);
   pCurPanelInst->Show(true);

   // add the panel instance to the list
   panels.AddTail(pCurPanelInst);

   return pCurPanelInst;
}

/******************************************************************************
* CPanelization::GetCurPanel
*/
CPanelInstance *CPanelization::GetCurPanel()
{
   return pCurPanelInst;
}

/******************************************************************************
* CPanelization::SetCurPanel
*/
void CPanelization::SetCurPanel(CPanelInstance *panelInst)
{
   if (pCurPanelInst != NULL)
      pCurPanelInst->Show(false);

   if (panels.Find(panelInst))
      pCurPanelInst = panelInst;

   pCurPanelInst->Show(true);
}

/******************************************************************************
* CPanelization::GetPanel
*/
CPanelInstance *CPanelization::GetPanel(CString panelName)
{
   POSITION pos = panels.GetHeadPosition();
   while (pos)
   {
      CPanelInstance *panelInst = panels.GetNext(pos);

      if (panelName.Compare(panelInst->GetPanelName()) == 0)
         return panelInst;
   }

   return NULL;
}

/******************************************************************************
* CPanelization::GetFirstPanel
*/
CPanelInstance *CPanelization::GetFirstPanel()
{
   if (panels.IsEmpty())
      return NULL;

   return panels.GetHead();
}

/******************************************************************************
* CPanelization::GetNextPanel
*/
CPanelInstance *CPanelization::GetNextPanel(CPanelInstance *&panel)
{
   POSITION pos = panels.Find(panel);
   if (!pos)
      return NULL;

   panel = panels.GetNext(pos);
   panel = (pos != NULL)?panels.GetNext(pos):NULL;

   return panel;
}



//_____________________________________________________________________
// CAddPanelDlg dialog
IMPLEMENT_DYNAMIC(CAddPanelDlg, CDialog)
CAddPanelDlg::CAddPanelDlg(CCEtoODBDoc *doc, bool singleBoardAddition, CWnd* pParent /*=NULL*/)
   : CDialog(CAddPanelDlg::IDD, pParent)
   , m_panelName(_T(""))
   , m_xSizeStr(_T("0"))
   , m_ySizeStr(_T("0"))
{
   m_pDoc = doc;
   m_bAddSingleBoard = singleBoardAddition;
   m_pBlockRet = NULL;
}

CAddPanelDlg::~CAddPanelDlg()
{
}

void CAddPanelDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_PANEL_NAME, m_panelName);
   DDX_Text(pDX, IDC_X_SIZE, m_xSizeStr);
   DDX_Text(pDX, IDC_Y_SIZE, m_ySizeStr);
   DDX_Control(pDX, IDC_BOARD, m_boardsCB);
   DDX_Control(pDX, IDC_PLACEMENTOPTION, m_placementOption);
}


BEGIN_MESSAGE_MAP(CAddPanelDlg, CDialog)
   ON_CBN_SELCHANGE(IDC_BOARD, OnCbnSelchangeBoard)
END_MESSAGE_MAP()


// CAddPanelDlg message handlers

BOOL CAddPanelDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   if (m_bAddSingleBoard)
   {
      GetDlgItem(IDC_X_SIZE)->EnableWindow(FALSE);
      GetDlgItem(IDC_Y_SIZE)->EnableWindow(FALSE);
      m_placementOption.Clear();
      m_placementOption.AddString("Relocate board/panel origin to lower left");
      m_placementOption.AddString("Leave board/panel origins unchanged");
      m_placementOption.AddString("Relocate panel origin only to lower left");
      m_placementOption.SetCurSel(1); // case 1810, make default to leave origins alone
   }
   else
   {
      GetDlgItem(IDC_BOARD_LABEL)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_BOARD)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_PLACEMENTOPTION)->ShowWindow(SW_HIDE);
      m_placementOption.ShowWindow(SW_HIDE);
   }

   // fill m_boardsCB
   POSITION filePos = m_pDoc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = m_pDoc->getFileList().GetNext(filePos);

      if (file->getBlockType() == blockTypePanel)
         continue;

      m_boardsCB.SetItemData(m_boardsCB.AddString(file->getBlock()->getName()), (DWORD)(file->getBlock()));
   }
   if (m_boardsCB.GetCount() == 1)
      m_boardsCB.SetCurSel(0);
   OnCbnSelchangeBoard();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddPanelDlg::OnCbnSelchangeBoard()
{
   if (!m_bAddSingleBoard)
      return;

   int lastSelIndex = m_boardsCB.GetCurSel();

   UpdateData(TRUE);

   if (m_boardsCB.GetCurSel() == CB_ERR)
      return;

   int curSelIndex = m_boardsCB.GetCurSel();
   m_pBlockRet = (BlockStruct*)m_boardsCB.GetItemData(curSelIndex);

   if (!m_pBlockRet)
      return;

   DataStruct *boardOutline = getBoardOutline(m_pBlockRet);
   if (boardOutline != NULL)
   {
      CExtent extent = boardOutline->getPolyList()->getExtent();
      if (!extent.isValid())
         extent = m_pDoc->blockExtents(&m_pBlockRet->getDataList(), 0.0, 0.0, 0.0, 0, 1, -1, TRUE);
      int decimals = GetDecimals(m_pDoc->getPageUnits());
      m_xSizeStr.Format("%.*lf", decimals, extent.getXsize());
      m_ySizeStr.Format("%.*lf", decimals, extent.getYsize());
   }
   else
   {
      CString buf;
      buf.Format("%s has no board outline!  Choose another board.", m_pBlockRet->getName());
      ErrorMessage(buf);

      m_boardsCB.SetCurSel(lastSelIndex);
   }

   UpdateData(FALSE);
}

void CAddPanelDlg::OnOK()
{
   UpdateData(TRUE);

   if (m_panelName.IsEmpty())
   {
      ErrorMessage("You must enter a panel name!");
      return;
   }

   if (m_bAddSingleBoard && m_pBlockRet == NULL)
   {
      ErrorMessage("You must choose a board to add!");
      return;
   }

   if (this->GetPanelXSize() <= SMALLNUMBER || this->GetPanelYSize() <= SMALLNUMBER)
   {
      if (m_bAddSingleBoard)
         ErrorMessage("The board doesn't have valid dimensions due to a lack of a board outline!\n\nYou must choose another board.");
      else
         ErrorMessage("You must enter valid dimensions for the panel!");
      return;
   }

   m_iPlacementOption = m_placementOption.GetCurSel();

   CDialog::OnOK();
}

DataStruct *CAddPanelDlg::getBoardOutline(BlockStruct *block)
{
   if (block == NULL)
      return NULL;

   DataStruct *boardOutline = NULL;

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() == T_POLY && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
      {
         boardOutline = data;
         break;
      }
   }

   return boardOutline;
}


//_____________________________________________________________________
// CManualBoardPlacementPropPage dialog
IMPLEMENT_DYNAMIC(CManualBoardPlacementPropPage, CPropertyPage)
CManualBoardPlacementPropPage::CManualBoardPlacementPropPage(CPanelization *panelInfo, CCEtoODBDoc *doc)
   : CPropertyPage(CManualBoardPlacementPropPage::IDD)
   , m_pDoc(doc)
   , m_pPanelInfo(panelInfo)
   , m_lastDrawnBoard(NULL)
   , m_xSizeStr(_T("0"))
   , m_ySizeStr(_T("0"))
{
}

/******************************************************************************
* CManualBoardPlacementPropPage::~CManualBoardPlacementPropPage
*/
CManualBoardPlacementPropPage::~CManualBoardPlacementPropPage()
{
}

/******************************************************************************
* CManualBoardPlacementPropPage::DoDataExchange
*/
void CManualBoardPlacementPropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_PANEL_CB, m_panelCB);
   DDX_Text(pDX, IDC_XSIZE, m_xSizeStr);
   DDX_Text(pDX, IDC_YSIZE, m_ySizeStr);
   DDX_Control(pDX, IDC_BOARDS_LC, m_boardLC);
}

BEGIN_MESSAGE_MAP(CManualBoardPlacementPropPage, CPropertyPage)
   ON_CBN_SELCHANGE(IDC_PANEL_CB, OnCbnSelchangePanelCb)
   ON_BN_CLICKED(IDC_CREATE_PANEL, OnBnClickedCreatePanel)
   ON_BN_CLICKED(IDC_RENAME_PANEL, OnBnClickedRenamePanel)
   ON_BN_CLICKED(IDC_REMOVE_PANEL, OnBnClickedRemovePanel)
   ON_BN_CLICKED(IDC_ADD_BOARD, OnBnClickedAddBoard)
   ON_BN_CLICKED(IDC_EDIT_BOARD, OnBnClickedEditBoard)
   ON_BN_CLICKED(IDC_REMOVE_BOARD, OnBnClickedRemoveBoard)
   ON_NOTIFY(NM_DBLCLK, IDC_BOARDS_LC, OnNMDblclkBoardsLc)
   ON_BN_CLICKED(IDC_APPLY_PROPERTIES, OnBnClickedApplyProperties)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_BOARDS_LC, OnLvnColumnclickBoardsLc)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_BOARDS_LC, OnLvnItemchangedBoardsLc)
// ON_NOTIFY(NM_CLICK, IDC_BOARDS_LC, OnNMClickBoardsLc)
   ON_CBN_SELENDOK(IDC_PANEL_CB, OnCbnSelendokPanelCb)
   ON_BN_CLICKED(IDC_SINGLE_BOARD, OnBnClickedSingleBoard)
   ON_BN_CLICKED(IDC_UPDATE_VARIANT_LIST, OnBnClickedUpdateBoardName)
END_MESSAGE_MAP()

// CManualBoardPlacementPropPage message handlers

BOOL CManualBoardPlacementPropPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // columns
   LV_COLUMN column;
   CRect rect;
   m_boardLC.GetWindowRect(&rect);
   int width = rect.Width() / 6;
   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.cx = width;

   column.pszText = "Board Name";
   column.iSubItem = 0;
   m_boardLC.InsertColumn(0, &column);

   column.pszText = "Ref Name";
   column.iSubItem = 1;
   m_boardLC.InsertColumn(1, &column);

   column.pszText = "X Location";
   column.iSubItem = 2;
   m_boardLC.InsertColumn(2, &column);

   column.pszText = "Y Location";
   column.iSubItem = 3;
   m_boardLC.InsertColumn(3, &column);

   column.pszText = "Rotation";
   column.iSubItem = 4;
   m_boardLC.InsertColumn(4, &column);

   column.pszText = "Mirror";
   column.iSubItem = 5;
   m_boardLC.InsertColumn(5, &column);

   m_panelCB.ResetContent();
   CPanelInstance *panelInst = m_pPanelInfo->GetFirstPanel();
   for (; panelInst != NULL; m_pPanelInfo->GetNextPanel(panelInst))
      m_panelCB.AddString(panelInst->GetPanelName());

   // set the current panel if there is one
   panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst != NULL)
   {
      m_panelCB.SetCurSel(m_panelCB.FindString(0, panelInst->GetPanelName()));
      OnCbnSelchangePanelCb();
   }

   // disable the controls that edit the boards in the panel
   GetDlgItem(IDC_ADD_BOARD)->EnableWindow(panelInst != NULL);
   GetDlgItem(IDC_EDIT_BOARD)->EnableWindow(panelInst != NULL);
   GetDlgItem(IDC_REMOVE_BOARD)->EnableWindow(panelInst != NULL);
   GetDlgItem(IDC_BOARDS_LC)->EnableWindow(panelInst != NULL);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

int CManualBoardPlacementPropPage::addToBoardList(CBoardInstance *boardInst, int itemIndex)
{
   int decimals = GetDecimals(m_pDoc->getSettings().getPageUnits());
   CString boardName = boardInst->GetBoardName();
   int actualItem = itemIndex;

   LV_ITEM item;
   item.mask = LVIF_TEXT;
   item.iItem = (itemIndex < 0)?m_boardLC.GetItemCount():itemIndex;
   item.iSubItem = 0;
   item.pszText = boardName.GetBuffer(0);    //block->name.GetBuffer(0);

   if (itemIndex < 0)
   {
      actualItem = m_boardLC.InsertItem(&item);
      // ugly, dirty way to try to work around microsoft bug
      if (actualItem == -1)
      {
         actualItem = m_boardLC.InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = m_boardLC.InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = m_boardLC.InsertItem(&item);
               if (actualItem == -1)
               {
                  actualItem = m_boardLC.InsertItem(&item);
                  if (actualItem == -1)
                     actualItem = m_boardLC.InsertItem(&item);
               }
            }
         }
      }
   }
   else
      m_boardLC.SetItem(&item);
   m_boardLC.SetItemData(actualItem, (LPARAM)boardInst);

   char buf[256];

   item.iItem = actualItem;
   item.iSubItem = 1;

   if (!boardInst->GetBoardInstName().IsEmpty())
   {
      strncpy(buf, boardInst->GetBoardInstName(), 256);
      item.pszText = buf;
   }
   else
      item.pszText = "";
   m_boardLC.SetItem(&item);

   double x, y;
   boardInst->GetLocation(x, y);

   item.iItem = actualItem;
   item.iSubItem = 2;
   sprintf(buf, "%.*lf", decimals, x);
   item.pszText = buf;
   m_boardLC.SetItem(&item);

   item.iSubItem = 3;
   sprintf(buf, "%.*lf", decimals, y);
   item.pszText = buf;
   m_boardLC.SetItem(&item);

   item.iSubItem = 4;
   sprintf(buf, "%.0lf", RadToDeg(boardInst->GetRotation()));
   item.pszText = buf;
   m_boardLC.SetItem(&item);

   item.iSubItem = 5;
   if (boardInst->GetMirror())
      item.pszText = "Yes";
   else
      item.pszText = "No";
   m_boardLC.SetItem(&item);

   return actualItem;
}

/******************************************************************************
* CManualBoardPlacementPropPage::fillBoardList
*/
void CManualBoardPlacementPropPage::fillBoardList(CPanelInstance *panelInst)
{
   m_boardLC.DeleteAllItems();

   if (panelInst == NULL)
      return;

   // fill boardLC
   CBoardInstance *boardInst = panelInst->GetFirstBoard();
   for (; boardInst != NULL; panelInst->GetNextBoard(boardInst))
      addToBoardList(boardInst);
}

void CManualBoardPlacementPropPage::clearBoardColors(bool redraw)
{
   m_pPanelInfo->ClearCurPanelValidation();

   if (redraw)
      m_pDoc->UpdateAllViews(NULL);
}

void CManualBoardPlacementPropPage::OnBnClickedCreatePanel()
{
   CAddPanelDlg dlg(m_pDoc);
   if (dlg.DoModal() == IDCANCEL)
      return;

   clearBoardColors(false);

   UpdateData(TRUE);

   if (m_pPanelInfo->CreateNewPanel(dlg.GetPanelName(), dlg.GetPanelXSize(), dlg.GetPanelYSize()) == NULL)
   {
      ErrorMessage("The new panel was not created.  Please check if you have panel dimensions greater than 0.0.",
         "Panelization", MB_OK);
      return;
   }

   m_panelCB.SetCurSel(m_panelCB.AddString(dlg.GetPanelName()));
   OnCbnSelchangePanelCb();
}

void CManualBoardPlacementPropPage::OnCbnSelchangePanelCb()
{
   CString panelName;
   int curSelected = m_panelCB.GetCurSel();

   GetDlgItem(IDC_XSIZE)->EnableWindow(curSelected != CB_ERR);
   GetDlgItem(IDC_YSIZE)->EnableWindow(curSelected != CB_ERR);

   if (curSelected == CB_ERR)
      return;

   // enable the controls to edit the boards in the panel
   GetDlgItem(IDC_ADD_BOARD)->EnableWindow(TRUE);
   GetDlgItem(IDC_EDIT_BOARD)->EnableWindow(TRUE);
   GetDlgItem(IDC_REMOVE_BOARD)->EnableWindow(TRUE);
   GetDlgItem(IDC_BOARDS_LC)->EnableWindow(TRUE);

   m_panelCB.GetLBText(curSelected, panelName);

   clearBoardColors(false);

   CPanelInstance *panelInst = m_pPanelInfo->GetPanel(panelName);
   m_pPanelInfo->SetCurPanel(panelInst);

   int decimals = GetDecimals(m_pDoc->getPageUnits());
   m_xSizeStr.Format("%.*lf", decimals, panelInst->GetXSize());
   m_ySizeStr.Format("%.*lf", decimals, panelInst->GetYSize());

   UpdateData(FALSE);

   fillBoardList(panelInst);
}

void CManualBoardPlacementPropPage::OnCbnSelendokPanelCb()
{
   // TODO: Add your control notification handler code here
}

void CManualBoardPlacementPropPage::OnBnClickedRenamePanel()
{
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel", "Panelization", MB_OK);
      return;
   }

   InputDlg dlg;
   dlg.m_prompt = "Enter a panel name.";
   dlg.m_input = panelInst->GetPanelName();
   if (dlg.DoModal() == IDCANCEL)
      return;

   panelInst->SetPanelName(dlg.m_input);

   int curSelected = m_panelCB.GetCurSel();
   if (curSelected == CB_ERR)
      return;

   m_panelCB.DeleteString(curSelected);
   m_panelCB.InsertString(curSelected, dlg.m_input);
   m_panelCB.SetCurSel(curSelected);

// SetModified(TRUE);
}

void CManualBoardPlacementPropPage::OnBnClickedRemovePanel()
{
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel", "Panelization", MB_OK);
      return;
   }

   clearBoardColors(false);

   m_pPanelInfo->RemovePanel(panelInst, TRUE);

   m_panelCB.DeleteString(m_panelCB.GetCurSel());
   m_panelCB.SetCurSel(-1);

   fillBoardList(NULL);
   m_pDoc->UpdateAllViews(NULL);
}

void CManualBoardPlacementPropPage::OnBnClickedSingleBoard()
{
   CAddPanelDlg dlg(m_pDoc, true);
   if (dlg.DoModal() == IDCANCEL)
      return;

   clearBoardColors(false);

   UpdateData(TRUE);

   CPanelInstance *panelInst = m_pPanelInfo->CreateNewPanel(dlg.GetPanelName(), dlg.GetPanelXSize(), dlg.GetPanelYSize());
   if (panelInst == NULL)
   {
      ErrorMessage("The new panel was not created.  Please check if you have panel dimensions greater than 0.0.",
         "Panelization", MB_OK);
      return;
   }

   // add board
   CBoardInstance *boardInst = panelInst->CreateNewBoard("Board1", panelInst->GetXSize()/2.0, panelInst->GetYSize()/2.0, 0.0, false,
      true, dlg.GetSelBlock());


   // Placement Options:
   //   Option 0: Relocate board/panel origin to lower left
   //   Option 1: Leave board/panel origins unchanged
   //   Option 2: Relocate panel origin only to lower left
   int placementOption = dlg.GetPlacementOption();

   switch (placementOption)
   {
   case 0: // Relocate board/panel origin to lower left
      {
         CPoint2d loc;
         if (boardInst->GetLocation(loc))
         {
            if (!loc.fpeq(CPoint2d(0.0, 0.0)))
            {
               FileStruct *curBlockFile = NULL;
               POSITION pos = m_pDoc->getFileList().GetHeadPosition();
               while (pos)
               {
                  FileStruct *file = m_pDoc->getFileList().GetNext(pos);
                  if (file->getBlock() != dlg.GetSelBlock())
                     continue;

                  curBlockFile = file;
                  break;
               }
               if (curBlockFile != NULL)
               {
                  curBlockFile->setInsertX((DbUnit)loc.x);
                  curBlockFile->setInsertY((DbUnit)loc.y);
                  RestructureFile(m_pDoc, curBlockFile);
               }

               boardInst->SetBoard(dlg.GetSelBlock());
               boardInst->SetLocation(0.0, 0.0, 0.0, false, false);
            }
         }
      }
      break;
   case 1: // Leave board/panel origins unchanged
      {
         CPoint2d loc;
         if (boardInst->GetLocation(loc))
         {
            if (!loc.fpeq(CPoint2d(0.0, 0.0)))
            {
               panelInst->SetOrigin(-loc.x, -loc.y);
               boardInst->SetLocation(0.0, 0.0, 0.0, false, false);
            }
         }
      }
      break;
   case 2: // Relocate panel origin only to lower left
   default:
      break;
   }

   addToBoardList(boardInst);

   panelInst->DrawBoard(boardInst);

   m_panelCB.SetCurSel(m_panelCB.AddString(dlg.GetPanelName()));
   OnCbnSelchangePanelCb();
}

void CManualBoardPlacementPropPage::OnBnClickedApplyProperties()
{
   UpdateData(TRUE);

   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel", "Panelization", MB_OK);
      return;
   }

   double panelSizeX = this->GetPanelXSize();
   double panelSizeY = this->GetPanelYSize();

   if (fabs(panelInst->GetXSize() - panelSizeX) > SMALLNUMBER || fabs(panelInst->GetYSize() - panelSizeY) > SMALLNUMBER)
      panelInst->SetSize(panelSizeX, panelSizeY, 0.0, 0.0);

   // User may have entered extra digits. Reformat the display to keep it consistent.
   int decimals = GetDecimals(m_pDoc->getPageUnits());
   m_xSizeStr.Format("%.*lf", decimals, panelSizeX);
   m_ySizeStr.Format("%.*lf", decimals, panelSizeY);
   UpdateData(FALSE);

   //SetModified(TRUE);
}

void CManualBoardPlacementPropPage::OnLvnItemchangedBoardsLc(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

   if (pNMLV->uNewState != 3)
      return;

   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel");
      return;
   }

   int selItem =  pNMLV->iItem;
   CBoardInstance *boardInst = (CBoardInstance*)m_boardLC.GetItemData(selItem);

   panelInst->DrawBoard(m_lastDrawnBoard);
   panelInst->DrawBoard(boardInst, RGB(255, 255, 255));
   m_lastDrawnBoard = boardInst;

   *pResult = 0;
}

//void CManualBoardPlacementPropPage::OnNMClickBoardsLc(NMHDR *pNMHDR, LRESULT *pResult)
//{
// LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//
// CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
// if (panelInst == NULL)
// {
//    ErrorMessage("You must choose and existing panel");
//    return;
// }
//
// int selItem =  pNMLV->iItem;
//   CBoardInstance *boardInst = (CBoardInstance*)m_boardLC.GetItemData(selItem);
//
// panelInst->DrawBoard(lastDrawnBoard);
// panelInst->DrawBoard(boardInst, RGB(255, 255, 255));
// lastDrawnBoard = boardInst;
//
// *pResult = 0;
//}

void CManualBoardPlacementPropPage::OnNMDblclkBoardsLc(NMHDR *pNMHDR, LRESULT *pResult)
{
   OnBnClickedEditBoard();

   *pResult = 0;
}

static int CALLBACK LLCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam)
{
   return 1;

   //LayerListDlg *dlg = (LayerListDlg*)lParam;

   //int result;

   //LayerStruct *layer1 = (LayerStruct*)lParam1, *layer2 = (LayerStruct*)lParam2;

   //switch (dlg->sortBy)
   //{
   //case 1: // num
   //   result = layer1->num - layer2->num;
   //   break;
   //case 2: // artwork
   //   result = layer1->artworkstacknumber - layer2->artworkstacknumber;
   //   break;
   //case 3: // electrical
   //   result = layer1->getElectricalStackNumber() - layer2->getElectricalStackNumber();
   //   break;
   //case 4: // physical
   //   result = layer1->getPhysicalStackNumber() - layer2->getPhysicalStackNumber();
   //   break;
   //case 5: // layertype
   //   result = strcmp(layertypes[layer1->layertype], layertypes[layer2->layertype]);
   //   break;
   //case 6: // mirror
   //   {
   //      CString name1, name2;
   //      
   //      if (layer1->mirror != layer1->num)
   //         name1 = dlg->doc->LayerArray[layer1->mirror]->name;

   //      if (layer2->mirror != layer2->num)
   //         name2 = dlg->doc->LayerArray[layer2->mirror]->name;

   //      result = name1.CompareNoCase(name2);
   //   }
   //   break;
   //case 7: // comment
   //   result = layer1->comment.CompareNoCase(layer2->comment);
   //   break;
   //case 0: // name
   //default:
   //   result = layer1->name.CompareNoCase(layer2->name);
   //   break;
   //}

   //if (dlg->reverse)
   //   return -result;
   //else
   //   return result;
}

void CManualBoardPlacementPropPage::OnLvnColumnclickBoardsLc(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

   //NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   //if (sortBy == pNMListView->iSubItem)
   //   reverse = !reverse;
   //else
   //{
   //   sortBy = pNMListView->iSubItem;
   //   reverse = FALSE;
   //}

   //m_boardLC.SortItems(LLCompareFunc, (LPARAM)this);

   *pResult = 0;
}

void CManualBoardPlacementPropPage::OnBnClickedAddBoard()
{
   UpdateData();
   int decimals = GetDecimals(m_pDoc->getSettings().getPageUnits());

   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel", "Panelization", MB_OK);
      return;
   }

   PanelBoardDlg dlg;
   dlg.doc = m_pDoc;
   dlg.blockname = "";
   dlg.m_refName.Format("board_%d", m_boardLC.GetItemCount()+1);
   dlg.m_units = GetUnitName(m_pDoc->getSettings().getPageUnits());
   dlg.decimals = decimals;
   dlg.originX = 0.0;
   dlg.originY = 0.0;
   dlg.centerX = 0.0;
   dlg.centerY = 0.0;
   if (dlg.DoModal() == IDOK)
   {
      if (dlg.m_mirror)
         dlg.m_mirror = MIRROR_FLIP | MIRROR_LAYERS;
      
      CBoardInstance *boardInst = panelInst->CreateNewBoard(dlg.m_refName, atof(dlg.m_x), atof(dlg.m_y),
         DegToRad(atof(dlg.m_rotation)), (dlg.m_mirror & MIRROR_FLIP?true:false),
         (dlg.m_locationBy == LOCATION_BY_CENTER)?true:false, dlg.block);
   
      addToBoardList(boardInst);

      panelInst->DrawBoard(boardInst);

      //SetModified(TRUE);
   }
}

void CManualBoardPlacementPropPage::OnBnClickedEditBoard()
{
   UpdateData();
   int decimals = GetDecimals(m_pDoc->getSettings().getPageUnits());

   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel");
      return;
   }

   // get selected geometry
   int count = m_boardLC.GetItemCount();
   BOOL Selected = FALSE;
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_boardLC.GetItemState(selItem, LVIS_SELECTED))
      {
         Selected = TRUE;
         break;
      }
   }
   if (!Selected)
      return;

   CBoardInstance *boardInst = (CBoardInstance*)m_boardLC.GetItemData(selItem);
   bool aboutCenter = true;
   double x, y, centerX, centerY;
   boardInst->GetLocation(x, y);
   boardInst->GetCenterPoint(centerX, centerY);

   PanelBoardDlg dlg;
   dlg.doc = m_pDoc;
   dlg.blockname = boardInst->GetBoardName();
   dlg.m_refName = boardInst->GetBoardInstName();
   dlg.decimals = decimals;
   dlg.pBoardInst = boardInst;
   dlg.originX = x;
   dlg.originY = y;
   dlg.centerX = centerX;
   dlg.centerY = centerY;
   dlg.m_locationBy = boardInst->GetLastSetLocationBy();
   dlg.m_rotation.Format("%d", round(RadToDeg(boardInst->GetRotation())));
   dlg.m_mirror = (boardInst->GetMirror()?1:0);
   dlg.m_units = GetUnitName(m_pDoc->getSettings().getPageUnits());
   if (dlg.DoModal() == IDOK)
   {
      if (dlg.block->getName() != boardInst->GetBoardName())
         boardInst->SetBoard(dlg.block);
      if (!dlg.m_refName.IsEmpty())
      {
         if (dlg.m_refName.Compare(boardInst->GetBoardInstName()))
            boardInst->SetBoardInstName(dlg.m_refName);
      }
      else
         boardInst->SetBoardInstName("");
      boardInst->SetLocation(atof(dlg.m_x), atof(dlg.m_y), DegToRad(atof(dlg.m_rotation)), dlg.m_mirror?true:false, (dlg.m_locationBy == LOCATION_BY_CENTER)?true:false);

      addToBoardList(boardInst, selItem);
      m_pDoc->UpdateAllViews(NULL);

      //SetModified(TRUE);
   }
}

void CManualBoardPlacementPropPage::OnBnClickedRemoveBoard()
{
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel", "Panelization", MB_OK);
      return;
   }

   // get selected geometry
   int count = m_boardLC.GetItemCount();
   BOOL Selected = FALSE;
	int selItem=0;
   for (selItem=0; selItem < count; selItem++)
   {
      if (m_boardLC.GetItemState(selItem, LVIS_SELECTED))
      {
         Selected = TRUE;
         break;
      }
   }
   if (!Selected)
      return;

   CBoardInstance *boardInst = (CBoardInstance*)m_boardLC.GetItemData(selItem);
   if (m_lastDrawnBoard == boardInst)
      m_lastDrawnBoard = NULL;

   panelInst->RemoveBoard(boardInst, true);

   fillBoardList(panelInst);
   m_pDoc->UpdateAllViews(NULL);

   //SetModified(TRUE);
}

BOOL CManualBoardPlacementPropPage::OnSetActive()
{
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   fillBoardList(panelInst);

   return CPropertyPage::OnSetActive();
}

BOOL CManualBoardPlacementPropPage::OnApply()
{
   clearBoardColors();

   if (!m_pPanelInfo->ValidateCurPanel())
   {
      ErrorMessage("Some board(s) extend out of the panel!");
      return FALSE;
   }

// SetModified(FALSE);

   return CPropertyPage::OnApply();
}

int CManualBoardPlacementPropPage::FindUniqueBoardNameOnPanel(CString &selectItem, BlockTypeTag blcoktype)
{
   int boardcnt = m_boardLC.GetItemCount();
   int uniboardcnt = 0, selectIndex = 0;
   CMapStringToInt  m_boardMap;

   if(boardcnt)//search m_boardLC to look for unique board name
   {
      for(int idx = 0; idx < boardcnt && uniboardcnt < 2; idx ++)
      {
         CBoardInstance *boardInst = (CBoardInstance*)m_boardLC.GetItemData(idx);
         if(boardInst && boardInst->GetBoardBlock()->getBlockType() == blcoktype)
         {
            if(!m_boardMap.Lookup(boardInst->GetBoardName(),selectIndex))
            {
               uniboardcnt++;
               m_boardMap.SetAt(boardInst->GetBoardName(),idx);
            }
         }
      }
   }

   if(uniboardcnt == 1)
   {
      CBoardInstance *boardInst = (CBoardInstance*)m_boardLC.GetItemData(selectIndex);
      selectItem = (boardInst)?boardInst->GetBoardName():"";
   }

   return uniboardcnt;
}

int CManualBoardPlacementPropPage::FindAllBoardNamesOnFile(BlockTypeTag blcoktype)
{
   CMapStringToInt  m_boardMap;
   int index = 0;

   POSITION filePos = m_pDoc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = m_pDoc->getFileList().GetNext(filePos);

      if (file->getBlockType() == blcoktype) //look for all board name
      {
         CString blockname = file->getBlock()->getName();
         if(!m_boardMap.Lookup(blockname,index))
         {
            index++; 
            m_boardMap.SetAt(blockname,index);
         }
      }
   }
   return index;
}

void CManualBoardPlacementPropPage::OnBnClickedUpdateBoardName()
{
      CBoardInstance *boardInst = NULL;
      CString selectItem = "";
      UpdateBoardNameDlg dlg;

      switch(FindUniqueBoardNameOnPanel(selectItem,blockTypePcb))
      {
      case 1: // only one board name on panel
         dlg.doc = m_pDoc;
         dlg.m_existBlockName = selectItem;
         dlg.m_assignBoardType = blockTypePcb;

         if (FindAllBoardNamesOnFile(blockTypePcb) > 1)
         {
            if(dlg.DoModal() == IDOK && dlg.m_selectBlock)
            {
               //update board name in panel
               for(int i = 0; i < m_boardLC.GetItemCount(); i++)
               {
                  boardInst = (CBoardInstance*)m_boardLC.GetItemData(i);
                  if(boardInst)
                     boardInst->SetBoard(dlg.m_selectBlock);
               }
            }
            OnCbnSelchangePanelCb();
         }
         else //only one PCB boardname in file
            MessageBox("Need another PCB Design loaded to update. Close Panelization and Import another design.", 
               "Update Board Name", MB_OK);
         break;

      case 0: //no board
         MessageBox("Cannot change board name, Panel contains no PCB Designs to change. Use Panelization to add PCB Design instances to the panel.", 
            "Update Board Name", MB_OK);
         break;

      default: // multiple board name
         MessageBox("Multiple board panels are not supported with this function.", "Update Board Name", MB_OK);
         break;
      }
  
}

//_____________________________________________________________________
// CBoardArrayPlacementPropPage dialog
IMPLEMENT_DYNAMIC(CBoardArrayPlacementPropPage, CPropertyPage)
CBoardArrayPlacementPropPage::CBoardArrayPlacementPropPage(CPanelization *panelInfo, CCEtoODBDoc *doc)
   : CPropertyPage(CBoardArrayPlacementPropPage::IDD)
   , m_separationType(FALSE)
   , m_xOffset(0)
   , m_yOffset(0)
   , m_flipped(FALSE)
   , m_rows(0)
   , m_cols(0)
   , m_xValue(0)
   , m_yValue(0)
   , m_locationBy(FALSE)
{
   m_pDoc = doc;
   m_pPanelInfo = panelInfo;
   m_pPreviewBoardInst = NULL;
}

/******************************************************************************
* CBoardArrayPlacementPropPage::~CBoardArrayPlacementPropPage
*/
CBoardArrayPlacementPropPage::~CBoardArrayPlacementPropPage()
{
   if (m_pPreviewBoardInst != NULL)
   {
      CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
      if (panelInst != NULL)
         panelInst->RemoveBoard(m_pPreviewBoardInst, true);
   }
}

/******************************************************************************
* CBoardArrayPlacementPropPage::DoDataExchange
*/
void CBoardArrayPlacementPropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_BOARDNAME, m_boardCB);
   DDX_Control(pDX, IDC_ROTATION, m_rotationCB);
   DDX_Radio(pDX, IDC_BOARDSEPARATION, m_separationType);
   DDX_Text(pDX, IDC_XOFFSET, m_xOffset);
   DDX_Text(pDX, IDC_YOFFSET, m_yOffset);
   DDX_Check(pDX, IDC_FLIPPED, m_flipped);
   DDX_Text(pDX, IDC_ROWS, m_rows);
   DDX_Text(pDX, IDC_COLS, m_cols);
   DDX_Text(pDX, IDC_X_VALUE, m_xValue);
   DDX_Text(pDX, IDC_Y_VALUE, m_yValue);
   DDX_Radio(pDX, IDC_BYORIGIN, m_locationBy);
}

BEGIN_MESSAGE_MAP(CBoardArrayPlacementPropPage, CPropertyPage)
   ON_BN_CLICKED(IDC_BOARDSEPARATION, OnBnClickedBoardseparation)
   ON_BN_CLICKED(IDC_BOARDSEPARATION2, OnBnClickedBoardseparation2)
   ON_WM_PAINT()
   ON_BN_CLICKED(IDC_PREVIEW_BOARD, OnBnClickedPreviewBoard)
   ON_CBN_SELCHANGE(IDC_BOARDNAME, OnCbnSelchangeBoardname)
   ON_BN_CLICKED(IDC_APPLY_BOARD_ARRAY, OnBnClickedApplyBoardArray)
   ON_BN_CLICKED(IDC_CLEARBOARDS, OnBnClickedClearboards)
END_MESSAGE_MAP()

// CBoardArrayPlacementPropPage message handlers

BOOL CBoardArrayPlacementPropPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_rotationCB.ResetContent();
   m_rotationCB.AddString("0");
   m_rotationCB.AddString("90");
   m_rotationCB.AddString("180");
   m_rotationCB.AddString("270");

   m_rotationCB.SetCurSel(0);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CBoardArrayPlacementPropPage::drawSpacingBMP(short spacingType)
{
   // Load bitmap and get dimensions of the bitmap
   CBitmap bmp;

   if (spacingType == SEPTYPE_SPACING) // by spacing
      bmp.LoadBitmap(IDB_BOARD_SPACING);
   else if (spacingType == SEPTYPE_OFFSET) // by offset
      bmp.LoadBitmap(IDB_BOARD_OFFSET);
   
   BITMAP bmpInfo;
   bmp.GetBitmap(&bmpInfo);

   CWnd* pStatic = GetDlgItem(IDC_IMAGE);
   CRect rect;
   pStatic->GetWindowRect(&rect);
   ScreenToClient(&rect);
   bmp.SetBitmapDimension(rect.Width(), rect.Height());

   CClientDC dc(pStatic);  
   CDC dcTmp;
   dcTmp.CreateCompatibleDC(&dc);
   CBitmap *lastBMP = dcTmp.SelectObject(&bmp);

   rect.bottom = bmpInfo.bmHeight + rect.top;
   dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcTmp, 0, 0, SRCCOPY);

   dcTmp.SelectObject(lastBMP);
}

void CBoardArrayPlacementPropPage::OnCbnSelchangeBoardname()
{
   CString boardName;
   int curSelected = m_boardCB.GetCurSel();

   if (curSelected == CB_ERR)
      return;

   // enable the controls to edit the boards in the panel
   GetDlgItem(IDC_XOFFSET)->EnableWindow(TRUE);
   GetDlgItem(IDC_YOFFSET)->EnableWindow(TRUE);
   GetDlgItem(IDC_ROTATION)->EnableWindow(TRUE);
   GetDlgItem(IDC_FLIPPED)->EnableWindow(TRUE);
}

void CBoardArrayPlacementPropPage::OnBnClickedClearboards()
{
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
      return;

   if (ErrorMessage("Are you sure you want to clear all boards in this panel?", "Panelization", MB_YESNO) == IDNO)
      return;

   // clear all boards in the current panel
   panelInst->ClearAll(TRUE);
   m_pDoc->UpdateAllViews(NULL);

   // reset the previewed board
   m_pPreviewBoardInst = NULL;
}

void CBoardArrayPlacementPropPage::OnBnClickedPreviewBoard()
{
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
      return;

   int curSelected = m_boardCB.GetCurSel();
   if (curSelected == CB_ERR)
   {
      ErrorMessage("A Board must be selected!", "Panelization", MB_OK);
      return;
   }

   UpdateData(TRUE);

   CString boardName;
   CString rotation;
   bool aboutCenter = (m_locationBy == LOCATION_BY_CENTER)?true:false;
   BlockStruct *block = (BlockStruct*)m_boardCB.GetItemData(curSelected);

   m_boardCB.GetLBText(curSelected, boardName);
   m_rotationCB.GetLBText(m_rotationCB.GetCurSel(), rotation);

   if (m_pPreviewBoardInst != NULL)
   {
      m_pPreviewBoardInst->SetBoard(block);
      m_pPreviewBoardInst->SetLocation(m_xOffset, m_yOffset, DegToRad(atof(rotation)), m_flipped?true:false, aboutCenter);
   }
   else
      m_pPreviewBoardInst = panelInst->CreateNewBoard(boardName, m_xOffset, m_yOffset, DegToRad(atof(rotation)), m_flipped?true:false, aboutCenter, block);

   m_pDoc->UpdateAllViews(NULL);
}

void CBoardArrayPlacementPropPage::OnBnClickedBoardseparation()
{
   UpdateData();
   drawSpacingBMP(m_separationType);
}

void CBoardArrayPlacementPropPage::OnBnClickedBoardseparation2()
{
   UpdateData();
   drawSpacingBMP(m_separationType);
}

void CBoardArrayPlacementPropPage::OnPaint()
{
   CPaintDC dc(this); // device context for painting

   UpdateData();
   drawSpacingBMP(m_separationType);

   // Do not call CPropertyPage::OnPaint() for painting messages
}

void CBoardArrayPlacementPropPage::OnBnClickedApplyBoardArray()
{
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
      return;

   int curSelected = m_boardCB.GetCurSel();
   if (curSelected == CB_ERR)
   {
      ErrorMessage("A Board must be selected!", "Panelization", MB_OK);
      return;
   }

   UpdateData(TRUE);

   if (m_rows <= 0 || m_cols <= 0)
   {
      ErrorMessage("Number of rows and/or columns is invalid.", "Panelization", MB_OK);
      return;
   }

   CString boardName;
   CString rotation;
   BlockStruct *block = (BlockStruct*)m_boardCB.GetItemData(curSelected);

   m_boardCB.GetLBText(curSelected, boardName);
   m_rotationCB.GetLBText(m_rotationCB.GetCurSel(), rotation);

   panelInst->RemoveBoard(m_pPreviewBoardInst, TRUE);
   m_pPreviewBoardInst = NULL;

   bool aboutCenter = (m_locationBy == LOCATION_BY_CENTER)?true:false;
   panelInst->AddBoardArray("board", m_cols, m_rows, DegToRad(atof(rotation)), m_flipped?true:false, aboutCenter,
      m_xOffset, m_yOffset, m_xValue, m_yValue, m_separationType, block);

   m_pDoc->UpdateAllViews(NULL);
}

BOOL CBoardArrayPlacementPropPage::OnSetActive()
{
   BOOL retVal = CPropertyPage::OnSetActive();

   m_boardCB.ResetContent();
   CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
   if (panelInst == NULL)
   {
      ErrorMessage("You must choose and existing panel", "Panelization", MB_OK);
      return FALSE;
   }

   // fill m_boardsCB
   m_boardCB.ResetContent();
   POSITION filePos = m_pDoc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = m_pDoc->getFileList().GetNext(filePos);

      if (file->getBlockType() == blockTypePanel)
         continue;

      int addedItem = m_boardCB.AddString(file->getBlock()->getName());
      m_boardCB.SetItemData(addedItem, (DWORD)(file->getBlock()));
   }

   GetDlgItem(IDC_XOFFSET)->EnableWindow(FALSE);
   GetDlgItem(IDC_YOFFSET)->EnableWindow(FALSE);
   GetDlgItem(IDC_ROTATION)->EnableWindow(FALSE);
   GetDlgItem(IDC_FLIPPED)->EnableWindow(FALSE);

   // if there is only one board, select it
   if (m_boardCB.GetCount() == 1)
      m_boardCB.SetCurSel(0);
   OnCbnSelchangeBoardname();

   return retVal;
}

BOOL CBoardArrayPlacementPropPage::OnKillActive()
{
   if (m_pPreviewBoardInst != NULL)
   {
      CPanelInstance *panelInst = m_pPanelInfo->GetCurPanel();
      if (panelInst != NULL)
         panelInst->RemoveBoard(m_pPreviewBoardInst, true);
      
      m_pPreviewBoardInst = NULL;
      m_pDoc->UpdateAllViews(NULL);
   }

   return CPropertyPage::OnKillActive();
}

BOOL CBoardArrayPlacementPropPage::OnApply()
{
   if (!m_pPanelInfo->ValidateCurPanel())
   {
      ErrorMessage("There are some errors in the panel!");
      return FALSE;
   }

   return CPropertyPage::OnApply();
}

//_____________________________________________________________________
// PanelBoardDlg dialog
PanelBoardDlg::PanelBoardDlg(CWnd* pParent /*=NULL*/)
   : CDialog(PanelBoardDlg::IDD, pParent)
   , m_locationBy(FALSE)
{
//{{AFX_DATA_INIT(PanelBoardDlg)
   m_mirror = FALSE;
   m_refName = _T("");
   m_rotation = _T("0");
   m_x = _T("0.00");
   m_y = _T("0.00");
   m_units = _T("");
   //}}AFX_DATA_INIT

   pBoardInst = NULL;
}

void PanelBoardDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PanelBoardDlg)
   DDX_Control(pDX, IDC_BOARDS_CB, m_boardsCB);
   DDX_Check(pDX, IDC_MIRROR, m_mirror);
   DDX_Text(pDX, IDC_REF_NAME, m_refName);
   DDX_CBString(pDX, IDC_ROTATION, m_rotation);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_UNITS, m_units);
   //}}AFX_DATA_MAP
   DDX_Radio(pDX, IDC_BYORIGIN, m_locationBy);
}

BEGIN_MESSAGE_MAP(PanelBoardDlg, CDialog)
   //{{AFX_MSG_MAP(PanelBoardDlg)
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BYORIGIN, OnBnClickedByorigin)
   ON_BN_CLICKED(IDC_BYCENTER, OnBnClickedBycenter)
   ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
   ON_CBN_SELCHANGE(IDC_BOARDS_CB, OnCbnSelchangeBoardsCb)
   ON_EN_CHANGE(IDC_REF_NAME, OnEnChangeRefName)
   ON_EN_CHANGE(IDC_X, OnEnChangeX)
   ON_EN_CHANGE(IDC_Y, OnEnChangeY)
   ON_CBN_SELCHANGE(IDC_ROTATION, OnCbnSelchangeRotation)
   ON_BN_CLICKED(IDC_MIRROR, OnBnClickedMirror)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PanelBoardDlg message handlers

BOOL PanelBoardDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // fill m_boardsCB
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() == blockTypePanel)
         continue;

      m_boardsCB.SetItemData(m_boardsCB.AddString(file->getBlock()->getName()), (DWORD)(file->getBlock()));
   }
   m_boardsCB.SelectString(-1, blockname);
   
   if (m_boardsCB.GetCount() == 1)
      m_boardsCB.SetCurSel(0);

   m_x.Format("%.*lf", decimals, (m_locationBy == LOCATION_BY_ORIGIN)?originX:centerX);
   m_y.Format("%.*lf", decimals, (m_locationBy == LOCATION_BY_ORIGIN)?originY:centerY);
   UpdateData(FALSE);

   if (pBoardInst == NULL)
      GetDlgItem(IDC_APPLY)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void PanelBoardDlg::OnBnClickedByorigin()
{
   UpdateData(TRUE);

   if (fabs(originX - centerX) < SMALLNUMBER && fabs(originY - centerY) < SMALLNUMBER)
      return;

   m_x.Format("%.*lf", decimals, originX);
   m_y.Format("%.*lf", decimals, originY);
   UpdateData(FALSE);

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void PanelBoardDlg::OnBnClickedBycenter()
{
   UpdateData(TRUE);

   if (fabs(originX - centerX) < SMALLNUMBER && fabs(originY - centerY) < SMALLNUMBER)
      return;

   m_x.Format("%.*lf", decimals, centerX);
   m_y.Format("%.*lf", decimals, centerY);
   UpdateData(FALSE);

   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void PanelBoardDlg::OnBnClickedApply()
{
   UpdateData(TRUE);

   int sel = m_boardsCB.GetCurSel();
   if (sel == CB_ERR)
   {
      ErrorMessage("No PCB selected");
      return;
   }
   block = (BlockStruct*)m_boardsCB.GetItemData(sel);
   
   if (block->getName() != pBoardInst->GetBoardName())
      pBoardInst->SetBoard(block);
   if (!m_refName.IsEmpty())
   {
      if (m_refName.Compare(pBoardInst->GetBoardInstName()))
         pBoardInst->SetBoardInstName(m_refName);
   }
   else
      pBoardInst->SetBoardInstName("");
   pBoardInst->SetLocation(atof(m_x), atof(m_y), DegToRad(atof(m_rotation)), m_mirror?true:false, (m_locationBy == LOCATION_BY_CENTER)?true:false);

   doc->UpdateAllViews(NULL);

   pBoardInst->GetLocation(originX, originY);
   pBoardInst->GetCenterPoint(centerX, centerY);

   GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}

void PanelBoardDlg::OnOK() 
{
   UpdateData();

   int sel = m_boardsCB.GetCurSel();

   if (sel == CB_ERR)
   {
      ErrorMessage("No PCB selected");
      return;
   }

   if (m_refName.IsEmpty())
   {
      ErrorMessage("Reference Name required.");
      return;
   }

   block = (BlockStruct*)m_boardsCB.GetItemData(sel);
   
   CDialog::OnOK();
}

void PanelBoardDlg::OnCbnSelchangeBoardsCb()
{
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void PanelBoardDlg::OnEnChangeRefName()
{
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void PanelBoardDlg::OnEnChangeX()
{
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void PanelBoardDlg::OnEnChangeY()
{
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void PanelBoardDlg::OnCbnSelchangeRotation()
{
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void PanelBoardDlg::OnBnClickedMirror()
{
   GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

//_____________________________________________________________________
// UpdateBoardNameDlg dialog
UpdateBoardNameDlg::UpdateBoardNameDlg(CWnd* pParent /*=NULL*/)
   : CDialog(UpdateBoardNameDlg::IDD, pParent)
{
//{{AFX_DATA_INIT(UpdateBoardNameDlg)
   
   //}}AFX_DATA_INIT

}

void UpdateBoardNameDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(UpdateBoardNameDlg)
   DDX_Control(pDX, IDC_BOARDS_NEWCB, m_boardsCB);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(UpdateBoardNameDlg, CDialog)
   //{{AFX_MSG_MAP(UpdateBoardNameDlg)
   //}}AFX_MSG_MAP
   
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UpdateBoardNameDlg message handlers

BOOL UpdateBoardNameDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();   

   CString m_existName = (m_existBlockName.IsEmpty())?"NONE":m_existBlockName;
   GetDlgItem(IDC_BOARD_EXISTNAME)->SetWindowText(m_existName);

   // fill m_boardsCB
   m_boardsCB.Clear();
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->getBlockType() == m_assignBoardType) // only PCB Design
      {
         CString blockname = file->getBlock()->getName();
         if(m_existName.Find(blockname) == -1)
            m_boardsCB.SetItemData(m_boardsCB.AddString(blockname), (DWORD)(file->getBlock()));
      }
   }
   
   if (m_boardsCB.GetCount())
      m_boardsCB.SetCurSel(0);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void UpdateBoardNameDlg::OnOK() 
{   
   UpdateData();

   int sel = m_boardsCB.GetCurSel();

   if (sel == CB_ERR)
   {
      ErrorMessage("No PCB selected");
      return;
   }

   m_selectBlock = (BlockStruct*)m_boardsCB.GetItemData(sel);
   
   CDialog::OnOK();
}