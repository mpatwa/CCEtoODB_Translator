
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "StdAfx.h"
#include "GeneralDialog.h"
#include <math.h>
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "attrib.h"
#include "ccview.h"
#include "mainfrm.h"
#include "aplist.h"                   
#include "atrblist.h"
#include "comppin.h"
#include "filelist.h"
#include "geomlist.h"
#include "toollist.h"
#include "layerlst.h"
#include "graph.h"
#include "polylib.h"
#include "pcbutil.h"
#include "pcbutil2.h"
#include "response.h"
#include "GerberThermalDfm.h"
#include "PinLabelSettings.h"
#include "GeometryTree.h"
#include "DFTFacade.h"
#include "GerberEducator.h"
#include "CAMCADNavigator.h"
#include "Crypt.h"
#include "dft.h"
#include "panref.h"
#include "StencilUi.h"
#include "StencilStepper.h"
#include ".\ccdoc.h"
#include "PaletteColorDialog.h"
#include "GerberEducatorUi.h"
#include "SelectStackDialog.h"
#include "EnumIterator.h"
//#include "DftDialog.h"
#include "RealPart.h"
#include "MultipleMachine.h"
#include "DcaJpeg.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BOOL ClosingApp; // from MAINFRM.CPP
extern BOOL LockDocuments; // from API.CPP
extern CView *activeView; // from CCVIEW.CPP
extern BOOL LoadingProjectFile; // from PORT.CPP
extern SettingsStruct GlSettings;      // from CAMCAD.CPP
extern CStatusBar *StatusBar;          // from MAINFRM.CPP
extern BOOL Editing;                   // from SELECT.CPP
extern CMultiDocTemplate* pDocTemplate; // from CAMCAD.CPP
extern CCEtoODBView *apiView; // from API.CPP
extern SelectNets *selectNetsDlg; // from NETS.CPP

BOOL LoadingDataFile = FALSE;

void ShowNothingDlg();
void LoadLayerTypeInfo(CCEtoODBDoc *doc, const char *filename);
void InitCurrentSettings();
void CloseDRCList();
int PanReference(CCEtoODBView *view, const char *ref);

//_____________________________________________________________________________
CCamCadDocData::CCamCadDocData(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
{
}

void CCamCadDocData::setMaxCoords()
{
   getCamCadDoc().SetMaxCoords();
}

void CCamCadDocData::loadBackgroundBitmap(BackgroundBitmapStruct& backgroundBitMap)
{
   getCamCadDoc().loadBackgroundBitmap(backgroundBitMap);
}

//_____________________________________________________________________________
CCEtoODBDoc* getActiveDocument()
{
   CCEtoODBDoc* document = NULL;
   CCEtoODBView* view = getActiveView();

   if (view != NULL)
   {
      document = view->GetDocument();
   }

   return document;
}

/////////////////////////////////////////////////////////////////////////////
// CCEtoODBDoc

IMPLEMENT_DYNCREATE(CCEtoODBDoc, CDocument)

BEGIN_MESSAGE_MAP(CCEtoODBDoc, CDocument)

   END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCEtoODBDoc, CDocument)
   //{{AFX_DISPATCH_MAP(CCEtoODBDoc)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICAMCAD to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {F8DE4B06-FBC2-11D1-BA40-0080ADB36DBB}
static const IID IID_ICAMCAD =
{ 0xf8de4b06, 0xfbc2, 0x11d1, { 0xba, 0x40, 0x0, 0x80, 0xad, 0xb3, 0x6d, 0xbb } };

BEGIN_INTERFACE_MAP(CCEtoODBDoc, CDocument)
   INTERFACE_PART(CCEtoODBDoc, IID_ICAMCAD, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCEtoODBDoc construction/destruction
//_____________________________________________________________________________
int CCEtoODBDoc::m_nextId = 0;

CCEtoODBDoc::CCEtoODBDoc()
: m_camCadData(*this)
, m_units(pageUnitsInches)
, m_settings(m_camCadData.getCamCadDataSettings())
, m_isTxpTestSessionDoc(false)
//, Bottom(m_camCadData.getCamCadDataSettings().getBottomView())
{
   m_id = m_nextId++;

#if defined(EnableGerberEducator)
   m_gerberEducator  = NULL;
#endif

   //m_stencilStepperDialog = NULL;

   fileLoaded = false;
   m_bLocked = false;
   m_sUserNameLock.Empty();
   m_projectPath.SetDefault();
   //m_firstTimeDraw = true;
   //EnableAutomation();

   //AfxOleLockApp();

#if !defined(DisableDftModule)
   pCDFT = new CDFTFacade(this);
#endif

   getSettings() = GlSettings;
   loadDefaultSettings();

   //SetMaxCoords();  

   //colorCount = 1;
   //ta_options_loaded = FALSE;    // option values for Test Access Analysis
   //getLayerArray().SetSize(0, 50);

   //maxBlockArray = 0;
   //m_blockArray.SetSize(0, 8192);

   //getDfmAlgorithmArraySize() = 0;
   //getDfmAlgorithmNamesArray().SetSize(0, 10);

   //getKeyWordArray().SetSize(0, 50);

   //ValueArray.SetSize(500, 500);
   //ValueArray.SetAt(0, "");
   //maxValueArray = 0;

   //setSelectStackTopIndex(-1);
   //SubSelectArray.SetSize(5, 5);
   //setSelectStackLevelIndex(0);

#if defined (EnableGeometryTreeImplementation)
   m_qfeFilesArray = NULL;
#endif

   //getGraphicsClassFilter().setToDefault();
   //for (int i = 0; i < MAX_CLASSTYPE; i++)
   //   showClasses[i] = TRUE;

   //showPinnrsTop = showPinnrsBottom = showPinnetsTop = showPinnetsBottom = showTAOffsetsTop = showTAOffsetsBottom = FALSE;

   //getInsertTypeFilter().setToDefault();
   //for (i = 0; i < MAX_INSERTTYPE; i++)
   //   showInsertTypes[i] = TRUE;

   //showInsertTypes[INSERTTYPE_RELIEFPAD] = FALSE;  // default off.

   //showSeqNums = showSeqArrows = showDirection = FALSE;

   //getWidthTable().SetSize(500, 100);

   for (int i=0; i<MAX_LAYERSETS; i++)
   {
      for (int j=0; j<MAX_LAYTYPE; j++)
      {
         LayerTypeArray[i][j].show = false;
         LayerTypeArray[i][j].color = RGB(255, 255, 255);
      }
   }

   CustomLayersetNames.SetSize(MAX_CUSTOM_COLORSETS);
   for (int i = 0; i < MAX_CUSTOM_COLORSETS; i++)
   {
      CString colorsetname;
      colorsetname.Format("Custom%d", (i+1));
      CustomLayersetNames.SetAt(i, colorsetname);
   }

   /*CustomColorsetButtonIDs[0]  = ID_COLORSETS_CUSTOM1;
   CustomColorsetButtonIDs[1]  = ID_COLORSETS_CUSTOM2;
   CustomColorsetButtonIDs[2]  = ID_COLORSETS_CUSTOM3;
   CustomColorsetButtonIDs[3]  = ID_COLORSETS_CUSTOM4;
   CustomColorsetButtonIDs[4]  = ID_COLORSETS_CUSTOM5;
   CustomColorsetButtonIDs[5]  = ID_COLORSETS_CUSTOM6;
   CustomColorsetButtonIDs[6]  = ID_COLORSETS_CUSTOM7;
   CustomColorsetButtonIDs[7]  = ID_COLORSETS_CUSTOM8;
   CustomColorsetButtonIDs[8]  = ID_COLORSETS_CUSTOM9;
   CustomColorsetButtonIDs[9]  = ID_COLORSETS_CUSTOM10;
   CustomColorsetButtonIDs[10] = ID_COLORSETS_CUSTOM11;
   CustomColorsetButtonIDs[11] = ID_COLORSETS_CUSTOM12;

   setBottomView(false);
   ColorSet = -1;
   PanelBoardView = 0;
   DontAskRegenPinLocs = FALSE;
   //getTopBackgroundBitMap().show = getBottomBackgroundBitMap().show = bgBmpOnTop = FALSE;
   currentMarker = NULL;

   // initialize global variable
   getTopBackgroundBitMap().filename = "";
   getBottomBackgroundBitMap().filename = "";
   getTopBackgroundBitMap().placementRect.xMin = getTopBackgroundBitMap().placementRect.xMax = getTopBackgroundBitMap().placementRect.yMin = getTopBackgroundBitMap().placementRect.yMax = 0;
   getBottomBackgroundBitMap().placementRect.xMin = getBottomBackgroundBitMap().placementRect.xMax = getBottomBackgroundBitMap().placementRect.yMin = getBottomBackgroundBitMap().placementRect.yMax = 0;


   seqInsertBehind = NULL;
   seqDataList = NULL;

   GeometryEditing = FALSE;
   TitleFull = FALSE;
   NegativeView = FALSE;
   FoundPolyEntity = FALSE;
   undo_level = 0;
   undo_min = 0;
   redo_max = 0;
   //m_smallWidthIndex = -1;
   //m_zeroWidthIndex  = -1;
*/
   if (!LoadingDataFile)
   {
      //getNextWidthIndex() = 0;
      getWidthTable().empty();

      // Create "Small Width"
      //BlockStruct* block = Add_Blockname("Small Width", -1,BL_WIDTH | BL_APERTURE | BL_GLOBAL | BL_SMALLWIDTH, TRUE);
      //block->setShape(T_ROUND);
      //block->setSizeA((DbUnit)Settings.SmallWidth);
      //widthTable[0] = block;
      //nextWidthIndex = 1;

      getSmallWidthIndex();
      getZeroWidthIndex();

      DrawingImported = FALSE;
   }
   else
   {
      if (!LoadCczDataFile())
      {
         // If failed, make sure to free up the Doc
         FreeDoc();
         return;
      }

      fileLoaded = true;
   }

   RegisterInternalKeywords();

   int sourceVersion = 100*getCamCadData().getSourceVersionMajor() + getCamCadData().getSourceVersionMinor();

   if (sourceVersion < 0)
   {
      // Contents not from cc file
   }
   else if (sourceVersion <= 405)
   {
      // File from CAMCAD 4.5 that is before version 4.5.316
      if (getCamCadData().getSourceVersionRevision() < 318)
      {
         GenerateBlockDesignSurfaceInfo(true);
      }
   }
   else if (sourceVersion <= 404)
   {
      // File from CAMCAD 4.4 that is before version 4.4.597 
      if (getCamCadData().getSourceVersionRevision() < 597)
      {
         GenerateBlockDesignSurfaceInfo(true);
      }
   }

   if (LoadingProjectFile)
   {
      LoadProjectFile();
   }

   //CString filename( getApp().getSystemSettingsFilePath("default.lt") );
   //LoadLayerTypeInfo(this, filename);
}

bool CCEtoODBDoc::getBottomView() const
{
   return m_camCadData.getCamCadDataSettings().getBottomView();
}

void CCEtoODBDoc::setBottomView(bool bottomViewFlag)
{
   m_camCadData.getCamCadDataSettings().setBottomView(bottomViewFlag);
}

CString CCEtoODBDoc::GetProjectPath(CString defaultPath) const
{
   // Return the project path base on the option set in "Project Path" dialog
   // If no path is set the return the incoming defaultPath

   CString path;
   /*if (m_projectPath.GetOption() == 0)
      path = m_projectPath.GetPathByFirstFile();
   else
      path = m_projectPath.GetPathByUserSelection();*/

   if (path.IsEmpty())
      return defaultPath.Trim();
   else
      return path.Trim();
}

void CCEtoODBDoc::SetProjectPathByFirstFile(const CString path)
{
   // This function will only set the project path by first file loaded becasue 
   // the project path selected by user in "Project Path" dialog is set ONLY 
   // at the CCEtoODBApp::OnSetProjectPath() event

   // Set project path by first file MUST ONLY be set once and once only during CAMCAD
   /*if (m_projectPath.GetPathByFirstFile().IsEmpty())
      m_projectPath.SetPathByFirstFile(path);*/
}

void CCEtoODBDoc::SetProjectPathByUserSelection(const CString path)
{
   //m_projectPath.SetPathByUserSelection(path);
}

void CCEtoODBDoc::SetProjectPathOption(const int option)
{
   //m_projectPath.SetOption(option);
}

bool CCEtoODBDoc::isInsertTypeVisible(InsertTypeTag insertType)
{
   return getCamCadData().isInsertTypeVisible(insertType);
}

bool CCEtoODBDoc::isGraphicClassVisible(GraphicClassTag graphicClass)
{
   return getGraphicsClassFilter().contains(graphicClass);
}

void CCEtoODBDoc::setInsertTypeVisible(InsertTypeTag insertType,bool visibleFlag)
{
   getInsertTypeFilter().set(insertType,visibleFlag);
}

void CCEtoODBDoc::setGraphicClassVisible(GraphicClassTag graphicClass,bool visibleFlag)
{
   getGraphicsClassFilter().set(graphicClass,visibleFlag);
}

int CCEtoODBDoc::getSmallWidthIndex()
{
   return getCamCadData().getSmallWidthIndex();
}

int CCEtoODBDoc::getZeroWidthIndex()
{
   return getCamCadData().getZeroWidthIndex();
}

BlockStruct* CCEtoODBDoc::getDefinedBlockAt(int index)
{
   BlockStruct* block = &(getCamCadData().getDefinedBlock(index));

   return block;
}

bool CCEtoODBDoc::swapBlocks(BlockStruct* block0,BlockStruct* block1)
{
   bool retval = getBlockArray().swapBlocks(block0,block1);

   return retval;
}

void CCEtoODBDoc::resetBlockExtents()
{
   for (int index = 0;index < getMaxBlockIndex();index++)
   {
      BlockStruct* block = getBlockAt(index);

      if (block == NULL)   continue;

      block->resetExtent();
   }
}

void CCEtoODBDoc::GenerateBlockDesignSurfaceInfo(bool regenerate)
{
   getBlockArray().GenerateDesignSurfaceInfo(getCamCadData(), regenerate);
}

void CCEtoODBDoc::SetMaxCoords()
{
   // max-Coord variables are not to exceed 16,000 because 2*MAX_-_COORD can not exceed MAX_INT
   // smaller side should be proportionally smaller than larger side
   double ratio;

   if (getSettings().getXmax() - getSettings().getXmin() > getSettings().getYmax() - getSettings().getYmin())
   {
      ratio = 1.0 * MAX_COORD / (getSettings().getXmax() - getSettings().getXmin());
   }
   else
   {
      ratio = 1.0 * MAX_COORD / (getSettings().getYmax() - getSettings().getYmin());
   }

   maxXCoord = round(ratio * getSettings().getXmax());
   minXCoord = round(ratio * getSettings().getXmin());
   maxYCoord = round(ratio * getSettings().getYmax());
   minYCoord = round(ratio * getSettings().getYmin());

   POSITION viewPos = GetFirstViewPosition();
   while (viewPos)
   {
      CCEtoODBView *view = (CCEtoODBView *)GetNextView(viewPos);
      view->SetScroll();
   }
}

int CCEtoODBDoc::propagateLayer(int parentLayerIndex,int childLayerIndex)
{
   return getCamCadData().propagateLayer(parentLayerIndex,childLayerIndex);
}

CCEtoODBDoc::~CCEtoODBDoc()
{
   AfxOleUnlockApp();

   if (selectNetsDlg)
      selectNetsDlg->OnCancel();

#if !defined(DisableDftModule)
   delete pCDFT;
   pCDFT = NULL;
#endif

   FreeDoc();
}

bool CCEtoODBDoc::HasData()
{
   if (getBlockArray().getSize() > 1)
      return true;

   return false;
}

void CCEtoODBDoc::FreeDoc()
{
   CWaitCursor hourglass;
   POSITION pos;

   UnselectAll(FALSE);

   // free FileList and NetLists
   //pos = getFileList().GetHeadPosition();
   //while (pos != NULL)
   //   FreeFile(getFileList().GetNext(pos));
   //getFileList().RemoveAll();

   getFileList().empty();

   getBlockArray().empty();

   // free LayerList
   //for (i=0; i<getMaxLayerIndex(); i++)
   //{
   //   LayerStruct *layer = getLayerArray()[i];
   //   if (layer == NULL) continue;
   //   FreeLayer(layer);
   //}

   getLayerArray().empty();

   // free layer groups
   pos = LayerGroupList.GetHeadPosition();
   while (pos != NULL)
      delete LayerGroupList.GetNext(pos);
   LayerGroupList.RemoveAll();

   // free named views
   pos = getNamedViewList().GetHeadPosition();
   while (pos != NULL)
   {
      CNamedView *view = getNamedViewList().GetNext(pos);
      //if (view->layerdata)
      //   free(view->layerdata);
      delete view;
   }
   getNamedViewList().RemoveAll();

   // free highlighted pins
   pos = HighlightedPinsList.GetHeadPosition();
   while (pos != NULL)
      delete(HighlightedPinsList.GetNext(pos));
   HighlightedPinsList.RemoveAll();


   // free widths 
   //while (getWidthTable().GetCount() > 1)
   //   getWidthTable().RemoveAt(1);     //  Make sure we leave the first one (SMALL WIDTH)
   getWidthTable().empty();

   //BlockStruct *block = Add_Blockname("Small Width", -1, 
   //      BL_WIDTH | BL_APERTURE | BL_GLOBAL | BL_SMALLWIDTH, TRUE);
   //block->setShape(T_ROUND);
   //block->setSizeA((DbUnit)getSettings().SmallWidth);
   //getWidthTable()[0] = block;
   //getNextWidthIndex() = 1;

   getZeroWidthIndex();
   getSmallWidthIndex();

   // free KeyWords
   getKeyWordArray().empty();

   // Recreate the internal keywords for subsequent data use
   RegisterInternalKeywords();

   ClearUndo();

#if defined(EnableGerberEducator)
   delete m_gerberEducator;
   m_gerberEducator = NULL;
#endif
   
   delete m_stencilStepperDialog;
   m_stencilStepperDialog = NULL;
}

void CCEtoODBDoc::loadBackgroundBitmap(BackgroundBitmapStruct& backgroundBitMap)
{
   CWaitCursor wait;

   CString filename = backgroundBitMap.getFileName();

   HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

   if (!hBitmap)
      hBitmap = LoadJPEG(filename);

   if (!hBitmap)
   {
      backgroundBitMap.show = FALSE;

      ErrorMessage(filename, "Bitmap Load Failed");
      return;
   }

   filename = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);

   backgroundBitMap.bitmap.Attach(hBitmap);
   backgroundBitMap.filename = filename;
   backgroundBitMap.show = TRUE;

   BITMAP bmpInfo;
   backgroundBitMap.bitmap.GetBitmap(&bmpInfo);
   double aspectRatio = 1.0 * bmpInfo.bmWidth / bmpInfo.bmHeight;

   backgroundBitMap.placementRect.xMin = this->getSettings().getXmin();
   backgroundBitMap.placementRect.yMin = this->getSettings().getYmin();

   if (aspectRatio > (this->getSettings().getXmax() - this->getSettings().getXmin()) / (this->getSettings().getYmax() - this->getSettings().getYmin()))
   {
      backgroundBitMap.placementRect.xMax = this->getSettings().getXmax();
      backgroundBitMap.placementRect.yMax = this->getSettings().getYmin() + round(1.0 * (this->getSettings().getXmax() - this->getSettings().getXmin()) / aspectRatio);
   }
   else
   {
      backgroundBitMap.placementRect.yMax = this->getSettings().getYmax();
      backgroundBitMap.placementRect.xMax = this->getSettings().getXmin() + round(1.0 * (this->getSettings().getYmax() - this->getSettings().getYmin()) * aspectRatio);
   }

   UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CCEtoODBDoc serialization
void CCEtoODBDoc::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
   {
      // TODO: add storing code here
   }
   else
   {
      // TODO: add loading code here
   }
}


/////////////////////////////////////////////////////////////////////////////
// CCEtoODBDoc diagnostics

#ifdef _DEBUG
void CCEtoODBDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CCEtoODBDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCEtoODBDoc commands
void CCEtoODBDoc::OnRedraw() 
{ 
   UpdateAllViews(NULL); 
}

POSITION CCEtoODBDoc::GetDocumentPosition()
{
   POSITION pos = pDocTemplate->GetFirstDocPosition();
   while (pos != NULL)
   {
      POSITION lastPos = pos;
      CDocument *doc = pDocTemplate->GetNextDoc(pos);
      if (doc == this)
         return lastPos;
   }

   return NULL;
}

void CCEtoODBDoc::saveDefaultSettings() 
{
   getSettings().setShowFillsDefault(getShowFills());
   getSettings().setShowApFillsDefault(getShowApFills());
   getSettings().setShowAttribsDefault(getShowAttribs());

   getSettings().setInsertTypeFilterDefault(getInsertTypeFilter());
   getSettings().setGraphicsClassFilterDefault(getGraphicsClassFilter());
}
// Keep
void CCEtoODBDoc::loadDefaultSettings() 
{
   setShowFills(getSettings().getShowFillsDefault());
   setShowApFills(getSettings().getShowApFillsDefault());
   setShowAttribs(getSettings().getShowAttribsDefault());

   getInsertTypeFilter()    = getSettings().getInsertTypeFilterDefault();
   getGraphicsClassFilter() = getSettings().getGraphicsClassFilterDefault();
}

void CCEtoODBDoc::OnSettingsSave() 
{
   CString file(getApp().getSystemSettingsFilePath("default.set"));
   CFileDialog FileDialog(FALSE, "SET", file,
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "CAMCAD Settings (*.set)|*.SET|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   saveDefaultSettings();   
   getSettings().SaveSettings(FileDialog.GetPathName());
}
// Keep
void CCEtoODBDoc::SortEntities()
{
   for (int i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (!block)
         continue;

      // copy all datas from the block to the temporary list
      CDataList dataList(false);
      POSITION pos = block->getHeadDataPosition();
      while (pos)
      {
         POSITION oldPos = pos;
         DataStruct *data = block->getNextData(pos);
         block->getDataList().SetAt(oldPos,NULL);
         dataList.AddTail(data);
      }
      block->getDataList().RemoveAll();

      // put all the non-inserts in the blocks list of datas
      pos = dataList.GetHeadPosition();
      while (pos)
      {
         DataStruct *data = dataList.GetNext(pos);

         if (data->getDataType() != T_INSERT)
            block->getDataList().AddTail(data);
      }

      // put all the inserts that are not components or real parts in the blocks list of datas
      pos = dataList.GetHeadPosition();
      while (pos)
      {
         DataStruct *data = dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert && data->getInsert()->getInsertType() != insertTypePcbComponent &&
            data->getInsert()->getInsertType() != insertTypeRealPart)
            block->getDataList().AddTail(data);
      }

      // put all the inserts that are components in the blocks list of datas
      pos = dataList.GetHeadPosition();
      while (pos)
      {
         DataStruct *data = dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert && data->getInsert()->getInsertType() == insertTypePcbComponent)
            block->getDataList().AddTail(data);
      }

      // put all the inserts that are real parts in the blocks list of datas
      pos = dataList.GetHeadPosition();
      while (pos)
      {
         DataStruct *data = dataList.GetNext(pos);

         if (data->getDataType() == dataTypeInsert && data->getInsert()->getInsertType() == insertTypeRealPart)
            block->getDataList().AddTail(data);
      }

      dataList.RemoveAll();
   }
}
// Keep
void CCEtoODBDoc::ConvertEtchToSignals()
{
   for (int i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (!block)
         continue;

      if (block->getBlockType() != blockTypeSheet)
         continue;

      POSITION pos = block->getHeadDataPosition();
      while (pos)
      {
         DataStruct *data = block->getNextData(pos);

         if (data->getDataType() == T_INSERT)
            continue;

         if (data->getGraphicClass() == graphicClassEtch)
            data->setGraphicClass(graphicClassSignal);
      }
   }
}

void CCEtoODBDoc::OnSettingsLoadLocal() 
{
   CString file = getApp().getUserPath() + "*.set";
   CFileDialog FileDialog(TRUE, "SET", file,
         OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
         "CAMCAD Settings (*.set)|*.SET|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   
   getSettings().LoadSettings(FileDialog.GetPathName());
   loadDefaultSettings();
   
   POSITION pos;
   CCEtoODBView *view;

   //if (!(getSettings().Xmax = fabs(getSettings().Xmax))) getSettings().Xmax = 1.0;
   //if (!(getSettings().Ymax = fabs(getSettings().Ymax))) getSettings().Ymax = 1.0;

   getSettings().setXmax(fabs(getSettings().getXmax()));

   if (getSettings().getXmax() == 0.) getSettings().setXmax(1.0);

   getSettings().setYmax(fabs(getSettings().getYmax()));

   if (getSettings().getYmax() == 0.) getSettings().setYmax(1.0);

   getSettings().setXmin(-fabs(getSettings().getXmin()));
   getSettings().setYmin(-fabs(getSettings().getYmin()));

   SetMaxCoords();
   pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      view = (CCEtoODBView *)GetNextView(pos);
      view->ScaleDenom = 1;
      view->ScaleNum = 1;
      view->UpdateScale();
   }

   // update UNITS on status bar
   if (StatusBar)
      StatusBar->SetPaneText(StatusBar->CommandToIndex(IDS_UNITS), GetUnitName(getSettings().getPageUnits()));
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnNoOperation()
{
}

void CCEtoODBDoc::OnUpdateNoOperation(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(false);
}

void CCEtoODBDoc::OnNegativeView() 
{
   NegativeView = !NegativeView;
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnShowGrid() 
{
   getSettings().Grid = !getSettings().Grid;
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnSnapToGrid() 
{
   getSettings().Snap = !getSettings().Snap;
}

void CCEtoODBDoc::OnShowOrigin() 
{
   getSettings().Crosshairs = !getSettings().Crosshairs;
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnCloseDocument() 
{
   if (Editing)
      ShowNothingDlg();
   CloseDRCList();

   //CloseSchematicList();
   activeView = NULL;
   apiView = NULL;
   POSITION pos = GetDocumentPosition();
   CString buf;
   buf.Format("%lu", pos);
   SendResponse("Document Close", buf);

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   pApp->getSchematicLinkController().deleteSchematicLink();
   
   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(NULL);

   CDocument::OnCloseDocument();
}

BOOL CCEtoODBDoc::is_file_inserted(int blocknum)
{
   for (int i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);
      if (block == NULL)   continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if (data->getDataType() != T_INSERT)
            continue;
         if (data->getInsert()->getBlockNumber() == blocknum)
            return TRUE;
      }
   }
   return FALSE;
}
                                  
void CCEtoODBDoc::mapUsedBlocks(CMapBlockNumberToBlock& map,BlockStruct& block)
{  
   BlockStruct* mappedBlock;

   if (!map.Lookup(block.getBlockNumber(),mappedBlock))
   {
      map.setAt(&block);
      BlockStruct* subBlock;

      if (block.isAperture())
      {
         if (block.getShape() == apertureComplex)
         {
            int subBlockNumber = block.getComplexApertureSubBlockNumber();

            subBlock = getBlockAt(subBlockNumber);

            if (subBlock != NULL)
            {
               mapUsedBlocks(map,*subBlock);
            }
         }
      }

      for (CDataListIterator insertIterator(block,dataTypeInsert);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         int blockNumber = data->getInsert()->getBlockNumber();

         subBlock = getBlockAt(blockNumber);

         if (subBlock != NULL)
         {
            mapUsedBlocks(map,*subBlock);
         }
      }
   }
}

void CCEtoODBDoc::purgeUnusedBlocks(bool displayReport)
{
   CWriteFormat* reportWriteFormat = NULL;

   if (displayReport)
   {
      CString LogFile = GetLogfilePath("Purged.log");
      remove(LogFile);

      CStdioFileWriteFormat stdioFileWriteFormat;

      if (!stdioFileWriteFormat.open(LogFile))
      {
         CString buf;
         buf.Format("Error open [%s] file", LogFile);
         ErrorMessage(buf, "Error");

         return;
      }

      int purgedCount = purgeUnusedBlocks(&stdioFileWriteFormat);

      stdioFileWriteFormat.close();

      if (purgedCount > 0)
      {
         Notepad(LogFile);
      }
   }
   else
   {
      purgeUnusedBlocks();
   }
}

int CCEtoODBDoc::purgeUnusedBlocks(CWriteFormat* reportWriteFormat)
{
   int purgedCount = getCamCadData().purgeUnusedBlocks(reportWriteFormat);

   return purgedCount;
}
                                  
void CCEtoODBDoc::mapUsedWidths(CMapBlockNumberToBlock& map,BlockStruct& block)
{  
   for (POSITION pos = block.getHeadDataPosition();pos != NULL;)
   {
      DataStruct* data = block.getNextData(pos);

      if (data->getDataType() == dataTypePoly)
      {
         CPolyList* polyList = data->getPolyList();

         for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
         {
            CPoly* poly = polyList->GetNext(polyPos);

            BlockStruct* width = getDefinedWidthBlock(poly->getWidthIndex());
            map.setAt(width);
         }
      }
      else if (data->getDataType() == dataTypeText)
      {
         BlockStruct* width = getWidthTable().GetAt(data->getText()->getPenWidthIndex());
         map.setAt(width);
      }
      else if (data->getDataType() == dataTypeInsert)
      {
         BlockStruct* subBlock = getBlockAt(data->getInsert()->getBlockNumber());

         if (subBlock != NULL && (subBlock->isAperture() || subBlock->isTool()))
         {
            map.setAt(subBlock);
         }
      }
   }
}

void CCEtoODBDoc::purgeUnusedWidths(bool displayReport)
{
   CWriteFormat* reportWriteFormat = NULL;

   if (displayReport)
   {
      CString LogFile = GetLogfilePath("PurgedWidths.log");
      remove(LogFile);

      CStdioFileWriteFormat stdioFileWriteFormat;

      if (!stdioFileWriteFormat.open(LogFile))
      {
         CString buf;
         buf.Format("Error open [%s] file", LogFile);
         ErrorMessage(buf, "Error");

         return;
      }

      int purgedCount = purgeUnusedWidths(&stdioFileWriteFormat);

      stdioFileWriteFormat.close();

      if (purgedCount > 0)
      {
         Notepad(LogFile);
      }
   }
   else
   {
      purgeUnusedBlocks();
   }
}

int CCEtoODBDoc::purgeUnusedWidths(CWriteFormat* reportWriteFormat)
{
   int purgedCount = getCamCadData().purgeUnusedWidths(reportWriteFormat);

   return purgedCount;
}

void CCEtoODBDoc::purgeUnusedWidthsAndBlocks(bool displayReport)
{
   CWriteFormat* reportWriteFormat = NULL;
   CStdioFileWriteFormat stdioFileWriteFormat;
   CString LogFile;

   if (displayReport)
   {
      LogFile = GetLogfilePath("PurgedWidthsAndBlocks.log");
      remove(LogFile);

      if (!stdioFileWriteFormat.open(LogFile))
      {
         CString buf;
         buf.Format("Error open [%s] file", LogFile);
         ErrorMessage(buf, "Error");

         return;
      }

      reportWriteFormat = &stdioFileWriteFormat;
   }

   int purgedCount = getCamCadData().purgeUnusedWidthsAndBlocks(reportWriteFormat);

   if (reportWriteFormat != NULL)
   {
      stdioFileWriteFormat.close();

      if (purgedCount > 0)
      {
         Notepad(LogFile);
      }
   }
}

double CCEtoODBDoc::getWidth(int widthIndex)
{
   double width = getWidthTable()[widthIndex]->getSizeA();

   return width;
}

BlockStruct* CCEtoODBDoc::getWidthBlock(int widthIndex)
{
   BlockStruct* widthBlock = getWidthTable().getAt(widthIndex);

   return widthBlock;
}

int CCEtoODBDoc::getDefinedWidthIndex(double width)
{
   int err;
   int widthIndex = this->Graph_Aperture("",T_ROUND,width,0.,0.,0.,0.,0,BL_WIDTH,0,&err);

   return widthIndex;
}

int CCEtoODBDoc::getDefinedSquareWidthIndex(double width)
{
   int err;
   int widthIndex = this->Graph_Aperture("",T_SQUARE,width,0.,0.,0.,0.,0,BL_WIDTH,0,&err);

   return widthIndex;
}

BlockStruct* CCEtoODBDoc::getDefinedWidthBlock(int widthIndex)
{
   if (widthIndex < 0 || widthIndex >= getWidthTable().GetSize())
   {
      widthIndex = getZeroWidthIndex();
   }

   BlockStruct* widthBlock = getWidthTable()[widthIndex];

   return widthBlock;
}

double CCEtoODBDoc::convertPageUnitsTo(PageUnitsTag units,double value)
{
   double retval = value * Units_Factor(getSettings().getPageUnits(),units);

   return retval;
}

double CCEtoODBDoc::convertToPageUnits(PageUnitsTag units,double value)
{
   double retval = value * Units_Factor(units,getSettings().getPageUnits());

   return retval;
}

bool CCEtoODBDoc::getShowFills() const
{
   return m_showFills;
}

void CCEtoODBDoc::setShowFills(bool flag)
{
   m_showFills = flag;
}

bool CCEtoODBDoc::getShowApFills() const
{
   return m_showApFills;
}

void CCEtoODBDoc::setShowApFills(bool flag)
{
   m_showApFills = flag;
}

bool CCEtoODBDoc::getShowAttribs() const
{
   return m_showAttribs;
}

void CCEtoODBDoc::setShowAttribs(bool flag)
{
   m_showAttribs = flag;
}

BOOL CCEtoODBDoc::SaveModified() 
{
   BOOL CanClose = FALSE;

   if (ClosingApp)
      CanClose = TRUE;
   else
   {
      if (LockDocuments || IsLocked())
      {
         CanClose = ErrorMessage("You should not close CAMCAD while another application has Locked it.\n"
                                 "Are you sure you want to override the Lock and Close this Document?",
                                 "CAMCAD is Locked",MB_ICONSTOP | MB_YESNO | MB_DEFBUTTON2) == IDYES;
      }
      else if (this->isFileLoaded() || this->DrawingImported)
      {
         int resp = ErrorMessage("Do you want to save this document?", "", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
         CanClose = resp != IDCANCEL;
         /*if (resp == IDYES)
            this->SaveDatafile();*/
      }
      else
      {
         CanClose = TRUE;
      }
   }

   if (CanClose)
   {
      getActiveView()->OnMeasureTerminate();
      OnDoneEditing();
   }

   return CanClose;
}

BOOL CCEtoODBDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
   BOOL CanClose = FALSE;

   if (ClosingApp)
      CanClose = TRUE;
   else
   {
      if (CStencilUi::getStencilUi() != NULL)
      {
         if (&(CStencilUi::getStencilUi()->getCamCadDatabase().getCamCadDoc()) == this)
         {
            ErrorMessage("A stencil generation session is in progress", "Cannot close document");
            return FALSE;
         }
      }

      if (LockDocuments || IsLocked())
      {
         CanClose = ErrorMessage("You should not close CAMCAD while another application has Locked it.\n"
                                 "Are you sure you want to override the Lock and Close this Document?",
                                 "CAMCAD is Locked",MB_ICONSTOP | MB_YESNO | MB_DEFBUTTON2) == IDYES;
      }
      else if (this->isFileLoaded() || this->DrawingImported)
      {
         int resp = ErrorMessage("Do you want to save this document?", "", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
         CanClose = resp != IDCANCEL;
         /*if (resp == IDYES)
            this->SaveDatafile();*/
      }
      else
      {
         CanClose = TRUE;
      }
   }

   if (CanClose)
   {
      getActiveView()->OnMeasureTerminate();
      OnDoneEditing();
   }

   return CanClose;
}

void CCEtoODBDoc::OnApertureList() 
{ 
   if (CApertureListDialog(this).DoModal() == IDOK) 
   {
      UpdateAllViews(NULL);
      //FillCurrentSettings(this);
   }
}

void CCEtoODBDoc::OnEditTables() 
{ 
   CTableEditDialog tableDlg;

   tableDlg.DoModal();
}


void CCEtoODBDoc::OnAttribLists() 
{
   ListAttribs dlg;
   dlg.doc = this;

   if (dlg.DoModal() == IDOK)
      UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnRefreshInheritance() 
{
   AttribRefresh  dlg;
   dlg.m_geomtocomp = TRUE;
   dlg.m_typetocomp = TRUE;
   dlg.m_method = 1;             // 0 = append, 1 = overwrite, 2 = return
   if (dlg.DoModal() != IDOK)
      return;

   StoreDocForImporting();

   if (dlg.m_geomtocomp)
   {
      RefreshInheritedAttributes(this, dlg.m_method+1, TRUE);
   }

   if (dlg.m_typetocomp)
   {
      RefreshTypetoCompAttributes(this, dlg.m_method+1);
   }
}

void CCEtoODBDoc::OnPurgeUnusedBlocks() 
{
   purgeUnusedBlocks(true);
}

void CCEtoODBDoc::OnPurgeUnusedWidths() 
{
   purgeUnusedWidths(true);
}

void CCEtoODBDoc::OnPurgeUnusedWidthsAndGeometries() 
{
   purgeUnusedWidthsAndBlocks(true);
}

void CCEtoODBDoc::OnRotateFiles() 
{
   InputDlg dlg;
   dlg.m_prompt = "Enter angle in Degrees :";
   if (dlg.DoModal() != IDOK)
      return;

   DbUnit angle = (DbUnit)DegToRad(atof(dlg.m_input));

   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      file->incrementRotation(angle);
   }  

   UnselectAll(FALSE);
   OnFitPageToImage();
}


/******************************************************************************
* OnCheckValueAttribs
*/
void CCEtoODBDoc::OnCheckValueAttribs() 
{
   check_value_attributes(this);
   ErrorMessage("Normalize Values Completed!\nAll Values Normalized.", "Check Value Attribute");
   return;
}

static int compfunc( const void *arg1, const void *arg2 )
{
   LayerStruct **a1, **a2;
   a1 = (LayerStruct**)arg1;
   a2 = (LayerStruct**)arg2;

   return (*a1)->getName().Compare((*a2)->getName());
}


/******************************************************************************
* OnCheckBrokenInserts
*/
void CCEtoODBDoc::OnCheckBrokenInserts()
{
   CString logFileName = GetLogfilePath("insert.log");
   int errorCount = 0;

   FILE *fLog = fopen(logFileName, "wt");
   if (!fLog)
   {
      CString buf = "Can not open log file : ";
      buf += logFileName;
      ErrorMessage(buf);
      return;
   }

   for (int i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (!getBlockAt(data->getInsert()->getBlockNumber()))
         {
            CString refName = data->getInsert()->getRefname() ? data->getInsert()->getRefname() : "";
            fprintf(fLog, "In Geometry #%d (%s), Insert Entity #%ld RefName:%s points to Geometry #%d which does not exist.\n", i, block->getName(), data->getEntityNumber(), refName, data->getInsert()->getBlockNumber());
            errorCount++;
         }
      }
   }

   fclose(fLog);

   if (errorCount)
      Notepad(logFileName);
   else
      ErrorMessage("No Broken Inserts");
}

/******************************************************************************
* CCEtoODBDoc::OnRemoveBrokenInserts
*/
void CCEtoODBDoc::OnRemoveBrokenInserts()
{
   for (int i=0; i<getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         if (!getBlockAt(data->getInsert()->getBlockNumber()))
            RemoveOneEntityFromDataList(this, &block->getDataList(), data);
      }
   }
}

/******************************************************************************
* CCEtoODBDoc::OnDftValidatePcb
*/
void CCEtoODBDoc::OnDftValidatePcb() 
{
#if !defined(DisableDftModule)
   pCDFT->OnDFTValidatePcb();
#endif
}

/******************************************************************************
* CCEtoODBDoc::OnDftTestPlan
*/
void CCEtoODBDoc::OnDftTestPlan() 
{
#if !defined(DisableDftModule)
   pCDFT->OnDFTTestPlan();
#endif
}

/******************************************************************************
* CCEtoODBDoc::OnDftExportNailAssignments
*/
void CCEtoODBDoc::OnDftExportNailAssignments() 
{
#if !defined(DisableDftModule)
   pCDFT->OnDFTExportNails();
#endif
}

/******************************************************************************
* CCEtoODBDoc::OnDftImportTesterProgram
*/
void CCEtoODBDoc::OnDftImportTesterProgram() 
{
#if !defined(DisableDftModule)
   pCDFT->OnDFTImportTester();
#endif
}

/******************************************************************************
* CCEtoODBDoc::OnDftFixtureInserts
*/
void CCEtoODBDoc::OnDftFixtureInserts() 
{
#if !defined(DisableDftModule)
   pCDFT->OnDFTFixtInserts();
#endif
}

/******************************************************************************
* CCEtoODBDoc::OnDftExportDrills
*/
void CCEtoODBDoc::OnDftExportDrills() 
{
#if !defined(DisableDftModule)
   pCDFT->OnExportDrills();
#endif
}

/******************************************************************************
* CCEtoODBDoc::OnDftExportWiringList
*/
void CCEtoODBDoc::OnDftExportWiringList() 
{
#if !defined(DisableDftModule)
   pCDFT->OnExportWire();
#endif
}

/******************************************************************************
* CCEtoODBDoc::OnDftOptions
*/
void CCEtoODBDoc::OnDftOptions() 
{
#if !defined(DisableDftModule)
   pCDFT->OnDFTOptions();
#endif
}

/******************************************************************************
* OnDebug
*/
void restructurePadStack(CCEtoODBDoc& camCadDoc,BlockStruct* block,
   const CString& padStackBlockName,const CString& cur_blockname);

//#include "fixture_reuse.h"
//#include ".\ccdoc.h"
#include "CCEtoODBlicense.h"

void CCEtoODBDoc::OnDebug() 
{
   //CFixtureReuse fixReuse(*this);
   //fixReuse.ApplyFixture("C:\\Development\\CAMCAD\\4.5\\Info\\Fixture\\pads.asc.top", "C:\\Development\\CAMCAD\\4.5\\Info\\Fixture\\pads.asc.bot",
   //                    "C:\\Development\\CAMCAD\\4.5\\Info\\Fixture\\FixtureReuse.log");
   //CGerberEducatorColorsDialog dialog;

   //if (dialog.DoModal() == IDOK)
   //{
   //   //COLORREF newColor = paletteColorDialog.getColor();
   //}
}

void CCEtoODBDoc::OnColorPaletteDialog() 
{
   CAppPalette appPalette;
   CPaletteColorDialog paletteColorDialog(appPalette);
   paletteColorDialog.setColor(colorWhite);

   if (paletteColorDialog.DoModal() == IDOK)
   {
      COLORREF newColor = paletteColorDialog.getColor();
   }
}

void CCEtoODBDoc::OnSelectStackDialog()
{
#ifdef ImplementSelectStackDialog
   CSelectStackDialog::getSelectStackDialog(this)->showDialog();
#endif
}

void CCEtoODBDoc::OnDfmAnalysisGerberThermalDfm()
{
   // TODO: Add your command handler code here
   CGerberThermalPropertySheet gerberThermalPropertySheet(*this);
   
   gerberThermalPropertySheet.SetWizardMode();
   //gerberThermalPropertySheet.load(*this);
   gerberThermalPropertySheet.DoModal();
}

void CCEtoODBDoc::OnUpdateDfmAnalysisGerberThermalDfm(CCmdUI *pCmdUI)
{
   // TODO: Add your command update UI handler code here
   pCmdUI->Enable(true);
}

void CCEtoODBDoc::OnPcbAssigndevicetypes()
{
#ifdef SHAREWARE
   return;
#endif

   // make sure we are showing only one board
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (file->isHidden() || !file->isShown())
         continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)
         break;

      ErrorMessage("One and only one board must be visible for Device Typer to start.", "Device Typer", MB_OK | MB_ICONHAND);
      return;
   }

   // Launch Device Typer
   CString  path( getApp().getCamcadExeFolderPath() );
   path += "DeviceTyper.exe";

   int res = WinExec(path, SW_SHOW);
   switch (res)
   {
   case 0:
      ErrorMessage("The system is out of memory or resources!", "Device Typer", MB_OK | MB_ICONHAND);
      break;
   case ERROR_BAD_FORMAT:
      ErrorMessage("The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)!", "Device Typer", MB_OK | MB_ICONHAND);
      break;
   case ERROR_FILE_NOT_FOUND:
      {
         CString buf;
         buf.Format("The specified file [%s] was not found!\n", path);
         ErrorMessage(buf, "Device Typer", MB_OK | MB_ICONHAND);
      }
      break;
   case ERROR_PATH_NOT_FOUND:
      {
         CString buf;
         buf.Format("The specified path [%s] was not found!", path);
         ErrorMessage(buf, "Device Typer", MB_OK | MB_ICONHAND);
      }
      break;
   }
}

void CCEtoODBDoc::OnToggleGeometryTree()
{
#if defined (EnableGeometryTreeImplementation)

   if (m_qfeFilesArray != NULL)
   {
      delete m_qfeFilesArray;
      m_qfeFilesArray = NULL;
   }
   else
   {
      m_qfeFilesArray = new CGeometryTreeFilesArray();
      m_qfeFilesArray->load(*this);
   }

#endif
}

void CCEtoODBDoc::OnUpdateToggleGeometryTree(CCmdUI *pCmdUI)
{
#if defined (EnableGeometryTreeImplementation)
   pCmdUI->Enable(true);
#else
   pCmdUI->Enable(false);
#endif
}

//_____________________________________________________________________________
#if defined(EnableGerberEducator)
void CCEtoODBDoc::OnGerberEducator()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator)) 
   {
      ErrorAccess("You do not have a License for the Gerber Educator!");
   }
   else
   {
      CGerberEducator& gerberEducator = getDefinedGerberEducator(educatorFunctionalityLegacy);
      gerberEducator.showDialog();

      if (!gerberEducator.isInitialized())
      {
         terminateGerberEducatorSession();
      }
   }*/
}

void CCEtoODBDoc::OnUpdateGerberEducator(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(GraphGetDoc() != NULL);
}

void CCEtoODBDoc::OnGerberCentroidMergeTool()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductAgilentAoiWrite)) 
   {
      ErrorAccess("You do not have a License for the Gerber/Centroid Merge Tool!");
   }
   else
   {
      CGerberEducator& gerberEducator = getDefinedGerberEducator(educatorFunctionalityBasic);
      gerberEducator.showDialog();

      if (!gerberEducator.isInitialized())
      {
         terminateGerberEducatorSession();
      }
   }*/
}

void CCEtoODBDoc::OnUpdateGerberCentroidMergeTool(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(GraphGetDoc() != NULL);
}

void CCEtoODBDoc::gerberEducatorCreateGeometries()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductGerberEducator))
   {
      ErrorAccess("You do not have a License for the Gerber Educator Create Geometry Tool!");
   }
   else
   {
      CGerberEducator& gerberEducator = getDefinedGerberEducator(educatorFunctionalityCreateGeometry);
      gerberEducator.showDialog();

      if (!gerberEducator.isInitialized())
      {
         terminateGerberEducatorSession();
      }
   }*/
}

//void CCEtoODBDoc::OnGerberEducatorCreateGeometries()
//{
//   gerberEducatorCreateGeometries();
//}

void CCEtoODBDoc::OnUpdateGerberEducatorCreateGeometries(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(GraphGetDoc() != NULL);
}

#endif

//_____________________________________________________________________________
void CCEtoODBDoc::OnStencilStepper() 
{
   const bool enableStencilStepperLicensing = false;

   /*if (enableStencilStepperLicensing && !getApp().getCamcadLicense().isLicensed(camcadProductStencilStepper)) 
   {
      ErrorAccess("You do not have a License for the Stencil Stepper!");
   }
   else
   {
      if (m_stencilStepperDialog == NULL)
      {
         m_stencilStepperDialog = new CStencilStepperDialog(*this);
         m_stencilStepperDialog->Create(IDD_StencilStepperUg);
      }

      m_stencilStepperDialog->ShowWindow(SW_SHOW);
      m_stencilStepperDialog->UpdateWindow();
   }*/
}

void CCEtoODBDoc::OnTerminateStencilStepper() 
{
   //if (m_stencilStepperDialog != NULL)
   //{
   //   CDebugWriteFormat::close();
   //}

   delete m_stencilStepperDialog;
   m_stencilStepperDialog = NULL;
}

//_____________________________________________________________________________
/*void CCEtoODBDoc::OnDftanalysisKelvinanalysis()
{
   //CKelvinAnalysisDialog dlg(this);
   //dlg.DoModal(); 
}*/

void CCEtoODBDoc::OnUpdateDftanalysisKelvinanalysis(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(true);
}

void CCEtoODBDoc::OnPcbConvertviatopcbcomponent()
{
   FileStruct *file = this->getFileList().GetFirstShown(blockTypePcb);

   if (file == NULL)
      return;

   CMultiSelectDlg dialog;
   dialog.SetDialogCaption("Convert Selected Inserts to PCB Components");

   bool hasFeature = false;
   CMapStringToString featureTypeMap;
   WORD keyTest = (WORD)this->IsKeyWord(ATT_TEST, 0);
   POSITION pos = file->getBlock()->getHeadDataInsertPosition();
   while (pos)
   {
      DataStruct *data = file->getBlock()->getNextDataInsert(pos);
      if (data == NULL)
         continue;

      InsertStruct *insert = data->getInsert();
      if (insert == NULL || (insert->getInsertType() != insertTypeVia && 
          insert->getInsertType() != insertTypeTestPad && insert->getInsertType() != insertTypeFreePad))
         continue;

      CString featureTypeName = "";
      EFeatureType featureType = featureUnknown;
      Attrib *attrib = NULL;
      if (data->getAttributes() != NULL && data->getAttributes()->Lookup(keyTest, attrib))
      {
         //if (data->getAttributes()->Lookup(keyTest, attrib))
         //{
            featureTypeName.Format("%s with Test Attribute", insertTypeToDisplayString(insert->getInsertType()));
            featureType = (EFeatureType)-insert->getInsertType();
         //}
      }
      else
      {
         featureTypeName = insertTypeToDisplayString(insert->getInsertType());
         featureType = (EFeatureType)insert->getInsertType();
      }

      if (!featureTypeMap.Lookup(featureTypeName, featureTypeName) && featureType != featureUnknown)
      {
         CFeatureTypeItem *item = new CFeatureTypeItem(featureTypeName, featureType, FALSE);
         dialog.AddItem((CSelItem*)item);

         featureTypeMap.SetAt(featureTypeName, featureTypeName);
         hasFeature = true;
      }
   }

   if (hasFeature == false)
   {
      ErrorMessage("\nNo Via, Testpad, or Freepad is found on the PCB. Abort Convert to PCB Component command,", "Abort");
      return;
   }

   if (dialog.DoModal() != IDOK)
      return;

   int count = 0;
   CFeatureTypeItemList featureTypeList;
   pos = dialog.GetItemHeadPosition();
   while (pos)
   {
      CFeatureTypeItem *item = (CFeatureTypeItem *)dialog.GetItemNext(pos);
      featureTypeList.AddHead(item);
   }

   ConvertFeatureToPCBComponent(&featureTypeList);
   this->OnGeneratePinloc();  

   featureTypeList.RemoveAll();
}

bool CCEtoODBDoc::IsFeatureTypeInArray(CFeatureTypeItemList *featureTypeList, DataStruct *data)
{
   if (data == NULL || data->getDataType() != dataTypeInsert)
      return false;

   InsertStruct *insert = data->getInsert();
   if (insert == NULL || (insert->getInsertType() != insertTypeVia && 
       insert->getInsertType() != insertTypeTestPad && insert->getInsertType() != insertTypeFreePad))
      return false;

   WORD keyTest = (WORD)this->IsKeyWord(ATT_TEST, 0);
   POSITION pos = featureTypeList->GetHeadPosition();
   while (pos)
   {
      CFeatureTypeItem *item = featureTypeList->GetNext(pos);
      if (item == NULL)
         continue;

      if (item->GetFeatureType() < 0)
      {
         Attrib *attrib = NULL;
         if (data->getAttributes() == NULL)
            continue;
         if (data->getAttributes()->Lookup(keyTest, attrib))
            return true;
      }
      else if (item->GetFeatureType() == data->getInsert()->getInsertType())
      {
         return true;
      }
   }

   return false;
}

void CCEtoODBDoc::ConvertFeatureToPCBComponent(CFeatureTypeItemList *featureTypeList)
{
   FileStruct *file = this->getFileList().GetFirstShown(blockTypePcb);

   if (file == NULL)
      return;

   int keywordPinnr = this->IsKeyWord(ATT_COMPPINNR, 0);
   WORD keyNetname = (WORD)this->IsKeyWord(ATT_NETNAME, 0);
   WORD testKey = (WORD)this->RegisterKeyWord(ATT_TEST, 0, VT_STRING);

   CMapStringToString viaGeom;
   CString oldGeomNum = "";
   CString newGeomNum = "";
   int viaCnt = 0;
   int testPadCnt = 0;
   int freePadCnt = 0;

   POSITION pos = file->getBlock()->getHeadDataInsertPosition();
   while (pos != NULL)
   {
      DataStruct *data = file->getBlock()->getNextDataInsert(pos);
      InsertStruct *insert = data->getInsert();

      if (IsFeatureTypeInArray(featureTypeList, data) == false)
         continue;

      if (insert->getInsertType() == insertTypeVia)
         viaCnt++;
      else if (insert->getInsertType() == insertTypeTestPad)
         testPadCnt++;
      else if (insert->getInsertType() == insertTypeFreePad)
         freePadCnt++;

      // Assign a refdes if there is not one
      if (insert->getRefname().IsEmpty())
      {
         CString refdes = "";
         if (insert->getInsertType() == insertTypeVia)
            refdes.Format("via%d", viaCnt);
         else if (insert->getInsertType() == insertTypeTestPad)
            refdes.Format("testpad%d", testPadCnt);
         else if (insert->getInsertType() == insertTypeFreePad)
            refdes.Format("freepad%d", freePadCnt);

         insert->setRefname(STRDUP(refdes));
      }

      bool mirror = false;
      Attrib* testAttrib = NULL;
      if (data->getAttributes() != NULL && data->getAttributes()->Lookup(testKey, testAttrib) && testAttrib != NULL)
      {
         CString tmp = testAttrib->getStringValue();
         if (tmp.CompareNoCase("BOTTOM") == 0)
         {
            //insert->setMirrorFlags(MIRROR_LAYERS | MIRROR_FLIP);
            insert->setPlacedBottom(true);
         }
      }


      // Create a new PCB geom for the via if it is not already created
      oldGeomNum.Format("%d", data->getInsert()->getBlockNumber());
      if (!viaGeom.Lookup(oldGeomNum, newGeomNum))
      {
         BlockStruct *block = this->getBlockAt(data->getInsert()->getBlockNumber());

         if (block == NULL)
         {
            ErrorMessage("Missing geometry block", "ConvertFeatureToPCBComponent");
         }
         else
         {
            int count = 0;
            CString geomName = block->getName() + "_viaGeom";
            while (Graph_Block_Exists(this, geomName, block->getFileNumber()))
               geomName.Format("%s_viaGeom%d", block->getName(), ++count);

            BlockStruct *newBlock = Graph_Block_On(GBO_APPEND, geomName, block->getFileNumber(), 0L);
            newBlock->setBlockType(blockTypePcbComponent);

            int pinnr = 1;
            DataStruct *tmpData = this->Graph_Block_Reference(block->getName(), "1", block->getFileNumber(), 0.0, 0.0, 0.0, FALSE, 1, -1, TRUE);
            tmpData->getInsert()->setInsertType(insertTypePin);
            tmpData->setAttrib(getCamCadData(), keywordPinnr, valueTypeInteger, &pinnr, attributeUpdateOverwrite, NULL);
            Graph_Block_Off();

            newGeomNum.Format("%d", newBlock->getBlockNumber());
            viaGeom.SetAt(oldGeomNum, newGeomNum);
         }
      }

      // Point the data insert to the new PCB geom
      insert->setBlockNumber(atoi(newGeomNum));
      insert->setInsertType(insertTypePcbComponent);

      // Add comppin to the net
      Attrib *attrib = NULL;
      if (data->lookUpAttrib(keyNetname, attrib))
      {
         CString netname = attrib->getStringValue();
         data->removeAttrib(keyNetname);

         NetStruct *net = add_net(file, netname);
         add_comppin(file, net, data->getInsert()->getRefname(), "1");
      }
   }


   // Write the result count
   CString result = "Convert to PCB Component completed!!!\n";
   result += "================================\n\n";
   result.AppendFormat("Total Via Converted          = %d\nTotal Test Pad Converted = %d\nTotal Free Pad Converted = %d\n",
                       viaCnt, testPadCnt, freePadCnt);
   ErrorMessage(result, "");
}

void CCEtoODBDoc::zoomToComponent(const CString& refDes)
{
   double xmin, xmax, ymin, ymax;

   if (!ComponentsExtents(this,refDes,&xmin,&xmax,&ymin,&ymax,TRUE))
   {
      PanReference(getActiveView(),refDes);
      ErrorMessage("Component has no extents", "Panning");
   }
   else
   {
      double marginSizeX = (this->getSettings().getXmax() - this->getSettings().getXmin()) * this->getSettings().CompExtentMargin / 100;
      double marginSizeY = (this->getSettings().getYmax() - this->getSettings().getYmin()) * this->getSettings().CompExtentMargin / 100;

      double marginSize = max(marginSizeX, marginSizeY);

      xmin -= marginSize;
      xmax += marginSize;
      
      ymin -= marginSize;
      ymax += marginSize;

      getActiveView()->ZoomBox(xmin, xmax, ymin, ymax);
   }
}

/******************************************************************************
* Scale
*/
void CCEtoODBDoc::Scale(double factor, int decimals)
{
   // The DRC List contains formatted data. Uncool, really. Because of 
   // that we need to send how many decimal places to use for linear units.
   // Can not rely on GetDecimals(this->getPageUnits()) here because we might be
   // in the midst of chaging page units and that setting may not be
   // up to date yet.

   // convert all block and datas in the blocks
   for (int i=0; i<getBlockArray().getSize(); i++)
   {
      BlockStruct *block = getBlockArray().getBlockAt(i);

      if (block == NULL)
         continue;

      block->Scale(factor);
   }

   CTMatrix mat;
   mat.scale(factor, factor);

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      file->setInsertX((DbUnit)(file->getInsertX() * factor));
      file->setInsertY((DbUnit)(file->getInsertY() * factor));
      
      file->getNetList().Scale(factor);
      file->getBusList().Scale(factor);
      //getDFTSolutions(*file).Scale(factor);
      file->getDRCList().Scale(factor, decimals);
   }
}

void CCEtoODBDoc::OnDftanalysisProbetoprobeviolationcheck()
{
   FileStruct *file = getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
   {
      ErrorMessage("More than one file is currently visible.  Please ensure that only one PCB file is showing.", "DFT Probe Placement");
      return;
   }

   ValidateProbes(*file);
   UpdateAllViews(NULL);
}

void CCEtoODBDoc::OnUpdateDftanalysisProbetoprobeviolationcheck(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(false);
}

bool CCEtoODBDoc::Lock(CString userName)
{
   if (m_bLocked && m_sUserNameLock != userName)
      return false;

   m_bLocked = true;
   m_sUserNameLock = userName;

   return true;
}

bool CCEtoODBDoc::Unlock(CString userName)
{
   if (!m_bLocked)
      return true;

   if (m_sUserNameLock != userName)
      return false;

   m_bLocked = false;
   m_sUserNameLock.Empty();

   return true;
}

void CCEtoODBDoc::OnToolsNormalizeBottomBuildGeometries()
{
   getCamCadData().normalizeBottomBuiltGeometries();
}

BlockStruct* CCEtoODBDoc::CreateTopDefinedGeomtry(int bottomBuildBlockNum, CMapBlockNumberToBlock& bottomBuildGeometryMap)
{
   BlockStruct* bottomBuildBlock = this->getBlockAt(bottomBuildBlockNum);
   if (bottomBuildBlock == NULL)
      return NULL;

   // Do not change apertures, don't need bottom version of individual apertures
   if (bottomBuildBlock->isAperture())
      return NULL;

   // Found top build so return it
   BlockStruct* topBuildBlock = NULL;
   if (bottomBuildGeometryMap.Lookup(bottomBuildBlockNum, topBuildBlock) && topBuildBlock !=  NULL)
      return topBuildBlock;


   // Create name for top build geometry
   int count = 1;
   CString topBuildGeomName = (CString)bottomBuildBlock->getName() + "_Top";
   while (true)
   {
      topBuildBlock = Graph_Block_Exists(this, topBuildGeomName, bottomBuildBlock->getFileNumber());

      // If no block with the "TOP" version of the geometry then exit loop to create one
      if (topBuildBlock == NULL) 
         break;
      
      // If there is a geometry with the "TOP" version name, and its original name match the bottom build geometry
      // name, then it is the "TOP" version return it
      if (topBuildBlock->getOriginalName().CompareNoCase(bottomBuildBlock->getName()) == 0)
         return topBuildBlock;

      // Name already used, but geometry was not derived the corresponding bottom build geometry
      topBuildGeomName.Format("%s_TOP_%d", bottomBuildBlock->getName(), ++count);
   }


   topBuildBlock = Graph_Block_On(GBO_APPEND, topBuildGeomName, bottomBuildBlock->getFileNumber(), 0, bottomBuildBlock->getBlockType());
   topBuildBlock->setBlockType(bottomBuildBlock->getBlockType());
   topBuildBlock->setOriginalName(bottomBuildBlock->getName());
   Graph_Block_Copy(bottomBuildBlock, 0.0, 0.0, 0.0, 0, 1, -1, TRUE); 
   Graph_Block_Off();   

   for (POSITION dataPos = topBuildBlock->getDataList().GetHeadPosition(); dataPos != NULL;)
   {
      DataStruct *data = topBuildBlock->getDataList().GetNext(dataPos);
      if (data == NULL)
         continue;


      switch (data->getDataType())
      {
      case dataTypeInsert:
         {
            InsertStruct* insert = data->getInsert();
            if (insert != NULL)
            {

               BlockStruct *subBlock = this->getBlockAt(insert->getBlockNumber());

               if (subBlock != NULL)
               {
                  if (insert->getGraphicMirrored())
                  {
                     m_NBBG_MirCnt++;
                  }

                  // Case 1817
                  // Recursively treat all things involved in item
                  BlockStruct *topBuiltSubBlock = CreateTopDefinedGeomtry(subBlock->getBlockNumber(), bottomBuildGeometryMap);
                  if (topBuiltSubBlock != NULL)
                  {
                     bottomBuildGeometryMap.SetAt(subBlock->getBlockNumber(), topBuiltSubBlock);
                     insert->setBlockNumber(topBuiltSubBlock->getBlockNumber());
                  }

                  // Case 1817
                  // Using recursion, alteration of item starts at bottom of hierarchy, instead
                  // of top of hierarchy as original code did. 
                  // Treat layer number if not a floating layer.
                  // Change insert/mirror only the same number of times that mirror
                  // occurs in original item hierarchy. Simply mirroring every level in
                  // hiearchy is not correct. I.e. Bottom-Bottom is not the "mirror" of Top-Top.
                  if (!this->IsFloatingLayer(data->getLayerIndex()))
                  {
                     LayerStruct* layer = this->getLayerAt(data->getLayerIndex());

                     if (layer != NULL && layer->getMirroredLayerIndex() > 0)
                        data->setLayerIndex(layer->getMirroredLayerIndex());
                  }
                  else if (m_NBBG_MirCnt > 0)
                  {
                     m_NBBG_MirCnt--;
                     if (insert->getGraphicMirrored())
                     {
                        insert->setMirrorFlags(0);
                        insert->setPlacedBottom(false);
                     }
                     else
                     {
                        insert->setMirrorFlags(MIRROR_ALL);
                        insert->setPlacedBottom(true);
                     }
                     insert->setOriginX(-insert->getOriginX());
                     insert->setAngle(-insert->getAngle());
                  }
                  // else do nothing, is "neutral" item in hierarchy

               }
            }
         }
         break;

      case dataTypePoly:
         {
            // Change the polylist X coordinate and layer
            MirrorPolyOnX(data->getPolyList());

            if (!this->IsFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
            {
               LayerStruct* layer = this->getLayerAt(data->getLayerIndex());

               if (layer != NULL && layer->getMirroredLayerIndex() > 0)
                  data->setLayerIndex(layer->getMirroredLayerIndex());
            }
         }
         break;

      case dataTypeText:
         {
            // Change the text X coordinate and layer
            data->getText()->setMirrored(false);
            data->getText()->setPntX(-data->getText()->getPnt().x);

            if (!this->IsFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
            {
               LayerStruct* layer = this->getLayerAt(data->getLayerIndex());

               if (layer != NULL && layer->getMirroredLayerIndex() > 0)
                  data->setLayerIndex(layer->getMirroredLayerIndex());
            }
         }
         break;

      default:
         break;
      }
   }

   topBuildBlock->GenerateDesignSurfaceAttribute(getCamCadData(), true);
   return topBuildBlock;
}

BlockStruct* CCEtoODBDoc::CreateReversePadstack(BlockStruct* padstack, CString newPadstackName)
{
   BlockStruct *newPadstack = Graph_Block_On(GBO_APPEND, newPadstackName, padstack->getFileNumber(), 0);
   newPadstack->setBlockType(padstack->getBlockType());
   this->CopyAttribs(&newPadstack->getAttributesRef(), padstack->getAttributesRef());
   
   POSITION pos = padstack->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct* data = padstack->getDataList().GetNext(pos);
      if (data == NULL)
         continue;

      DataStruct *copy = getCamCadData().getNewDataStruct(*data, TRUE);
      newPadstack->getDataList().AddTail(copy);

      if (!this->IsFloatingLayer(data->getLayerIndex()) && data->getLayerIndex() != -1)
      {
         LayerStruct *layer = this->getLayerAt(data->getLayerIndex());
         if (layer != NULL && layer->getMirroredLayerIndex() > 0)
            copy->setLayerIndex(layer->getMirroredLayerIndex());
      }
   }

   Graph_Block_Off();
   return newPadstack;
}

/******************************************************************************
* MirrorPolyOnX
   - change all the X coordinatior of the polylist to -X
*/
void CCEtoODBDoc::MirrorPolyOnX(CPolyList* polylist)
{
   POSITION polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly* poly = polylist->GetNext(polyPos);
      if (poly == NULL)
         continue;

      POSITION pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos)
      {
         CPnt* pnt = poly->getPntList().GetNext(pntPos);
         if (pnt != NULL)
            pnt->x = -pnt->x;
      }
   }
}

void CCEtoODBDoc::AddRedlineXMLData(CString xmlString)
{
   getRedLineString() = xmlString;
   
}
void CCEtoODBDoc::AddLotsXMLData(CString xmlString)
{
   getLotsString() = xmlString;
   
}

CString CCEtoODBDoc::GetRedlineXMLData()
{
   return getRedLineString();
   
}
CString CCEtoODBDoc::GetLotsXMLData()
{
   return getLotsString();
   
}

void CCEtoODBDoc::AddColorSetsXMLData(CString xmlString)
{
   getColorSetsString() = xmlString;
   
}

CString CCEtoODBDoc::GetColorSetsXMLData()
{
   return getColorSetsString();
   
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// methods moved from BlockStruct to CCEtoODBDoc to support dca - knv
DataStruct& CCEtoODBDoc::SetCentroid(BlockStruct& block, double x, double y, double angleDegrees)
{
   DataStruct* centroidData = block.GetCentroidData();

   if (centroidData != NULL)
   {
      centroidData->getInsert()->setOriginX(x);
      centroidData->getInsert()->setOriginY(y);
      centroidData->getInsert()->setAngle(DegToRad(angleDegrees));
   }
   else
   {
      BlockStruct* centroidGeom = GetCentroidGeometry();

      Graph_Block_On(&block);
      centroidData = Graph_Block_Reference(centroidGeom->getName(), NULL, -1, x, y, DegToRad(angleDegrees), 0, 1.0, -1, TRUE);
      centroidData->getInsert()->setInsertType(insertTypeCentroid);
      Graph_Block_Off();
   }

   return *centroidData;
}

DataStruct& CCEtoODBDoc::SetCentroid(BlockStruct& block, double x, double y, double angleDegrees, CString methodAttrVal)
{
   DataStruct& cent = SetCentroid(block, x, y, angleDegrees); 

   SetUnknownAttrib(&(cent.getDefinedAttributes()), "METHOD", methodAttrVal, attributeUpdateOverwrite, NULL);

   return cent;
}

bool CCEtoODBDoc::ValidateTestpoint(BlockStruct& block)
{
   // A block that is a testpoint must have a Pin insert.
   // If it does not, repair it by adding one.
   // Return true if testpoint seems okay, false if not.

   for (POSITION pos = block.getHeadDataInsertPosition();pos != NULL;)
   {
      DataStruct* data = block.getNextDataInsert(pos);

      if (data->isInsertType(insertTypePin))
      {
         // We'll accept that
         return true;
      }
   }

   // If still here then did not find a pin insert.
   // Make one by getting the block of the first insert, and then
   // make a pin insert that inserts that block.
   
   for (POSITION pos = block.getHeadDataInsertPosition();pos != NULL;)
   {
      DataStruct* data = block.getNextDataInsert(pos);
      InsertStruct *ins = data->getInsert();
      InsertTypeTag insType = ins->getInsertType();
      BlockStruct *insB = getBlockAt(ins->getBlockNumber());

      if (insB != NULL && !(insB->getFlags() & BL_TOOL) && !(insB->getFlags() & BL_COMPLEX_TOOL))
      {
         // Make new pin block
         CString blockName;
         blockName.Format("TestPoint_block_%d_pin", block.getBlockNumber());
         BlockStruct* pinBlock = Graph_Block_On(GBO_APPEND, blockName, -1, 0);
         // This pin inserts what ever was inserted before
         DataStruct* pinData = Graph_Block_Reference(insB->getName(), ins->getRefname(), -1, 0.0, 0.0, 0.0, 0, 1.0, data->getLayerIndex(), FALSE);
         Graph_Block_Off();

         // Update the previous insert to now insert the pin block.
         // The pin inserted this at 0,0. Leave the old insert position as-is.
         ins->setBlockNumber(pinBlock->getBlockNumber());
         ins->setInsertType(insertTypePin);
         ins->setRefname((CString)"1");  // setRefname of literal char* "1" will crash, that overload will try to delete "1"

         // All repaired
         return true;
      }
   }

   return false;
}

bool CCEtoODBDoc::removeDataFromDataList(CDataList& dataList,DataStruct* data,POSITION dataPos)
{
   bool retval = false;

   if (dataPos == NULL)
   {
      dataPos = dataList.Find(data);
   }

   if (dataPos != NULL)
   {
      dataList.RemoveAt(dataPos);
      
      for (POSITION selPos = SelectList.GetHeadPosition();selPos != NULL;)
      {
         POSITION tempPos = selPos;
         SelectStruct* selectStruct = SelectList.GetNext(selPos);

         if (selectStruct->getData() == data)
         {
            delete selectStruct;
            selectStruct = NULL;
            SelectList.RemoveAt(tempPos);
         }
      }

      getSelectStack().popData(data);

      delete data;
      data = NULL;

      retval = true;
   }

   if (Editing)
      ShowNothingDlg();

   return retval;
}

bool CCEtoODBDoc::removeDataFromDataList(BlockStruct& block,DataStruct* data,POSITION dataPos)
{
   return removeDataFromDataList(block.getDataList(),data,dataPos);
}

int CCEtoODBDoc::removeDataFromDataListByGraphicClass(CDataList& dataList,GraphicClassTag graphicClass)
{
   int removedCount = 0;

   for (CDataListIterator dataIterator(dataList);dataIterator.hasNext();)
   {
      DataStruct* data = dataIterator.getNext();

      if (data->getGraphicClass() == graphicClass)
      {
         removeDataFromDataList(dataList,data);

         removedCount++;
      }
   }

   return removedCount;
}

int CCEtoODBDoc::removeDataFromDataListByGraphicClass(BlockStruct& block,GraphicClassTag graphicClass)
{
   return removeDataFromDataListByGraphicClass(block.getDataList(),graphicClass);
}

int CCEtoODBDoc::removeDataFromDataListByInsertType(CDataList& dataList,InsertTypeTag insertType)
{
   int removedCount = 0;

   for (CDataListIterator dataIterator(dataList,insertType);dataIterator.hasNext();)
   {
      DataStruct* data = dataIterator.getNext();

      removeDataFromDataList(dataList,data);

      removedCount++;
   }

   return removedCount;
}

int CCEtoODBDoc::removeDataFromDataListByInsertType(BlockStruct& block,InsertTypeTag insertType)
{
   return removeDataFromDataListByInsertType(block.getDataList(),insertType);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// methods moved from InsertStruct to CCEtoODBDoc to support dca - knv
bool CCEtoODBDoc::validateAndRepair(DataStruct& data)
{
   // Validate and repair the insert if needed/possible.
   // So far just supports TestPoint insert type.
   // Some testpoints have been created with a pad directly in the inserted block, rather
   // than a pin. A valid Testpoint has a pin.
   // Return true is item seems okay, false if not.

   if (data.getDataType() == dataTypeInsert)
   {
      InsertStruct& insert = *(data.getInsert());

      if (insert.getInsertType() == insertTypeTestPoint)
      {
         BlockStruct* block = getBlockAt(insert.getBlockNumber());

         if (block != NULL)
         {
            ValidateTestpoint(*block);
         }
      }
   }

   // Since this is not a comprehensively implemented function, we give
   // the item the benefit of the doubt.
   return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// methods moved from FileStruct to CCEtoODBDoc to support dca - knv
void CCEtoODBDoc::ValidateProbes(FileStruct& fileStruct)
{
   /*CDFTSolution *pSolution = GetCurrentDFTSolution(fileStruct);

   if (pSolution == NULL)
      return;

   CProbePlacementSolution ppSolution(this, pSolution->GetTestPlan());

   ppSolution.ValidateAndPlaceProbes();
   DeleteAllProbes(this, &fileStruct);
   ppSolution.PlaceProbesOnBoard();*/
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// methods moved from FileStruct to CCEtoODBDoc to support dca - knv
void CCEtoODBDoc::HideRealpartsForUnloadedComponents(FileStruct& fileStruct)
{
   CComponentRealpartLinkMap crpMap(*this,fileStruct);

   crpMap.HideRealpartsForUnloadedComponents();
}

/*bool CCEtoODBDoc::LoadDFTSolutionXML(CString xmlString, FileStruct& fileStruct, bool setCurrentSolution)
{
   // In getDFTSolutions(fileStruct).LoadXML call...
   // -1 is XML error return
   // 0 .. n is number of solutions, 0 is not an error

   /*int ret = getDFTSolutions(fileStruct).LoadXML(xmlString, this, &fileStruct);
   if (ret > 0 && setCurrentSolution)
   {
      SetCurrentDFTSolution(fileStruct,getDFTSolutions(fileStruct).GetHead());
   }

   return ret != -1;  // true=success, false=fail
	return true;
}

bool CCEtoODBDoc::LoadDFTSolutionFile(CString xmlFileName, FileStruct& fileStruct, bool setCurrentSolution)
{
   // In getDFTSolutions(fileStruct).LoadXML call...
   // -1 is XML error return
   // 0 .. n is number of solutions, 0 is not an error

   /*int ret = getDFTSolutions(fileStruct).LoadXMLFile(xmlFileName, this, &fileStruct);
   if (ret > 0 && setCurrentSolution)
   {
      SetCurrentDFTSolution(fileStruct,getDFTSolutions(fileStruct).GetHead());
   }

   return ret != -1;  // true=success, false=fail
	return true;
}

void CCEtoODBDoc::LoadMultipleMachineXML(CString xmlString, FileStruct& fileStruct)
{
   /*CMachineList* machineList = CreatMachineList(fileStruct);
   machineList->LoadXML(xmlString, this, &fileStruct);
}
*/
//==================================================================================================================================================================================
// ported from FileStruct to support dca
// DFT Solution
/*
CDFTSolutionList& CCEtoODBDoc::getDFTSolutions(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.getDFTSolutions();
}

CDFTSolution* CCEtoODBDoc::AddNewDFTSolution(const FileStruct& file, const CString& name, const bool isFlipped, PageUnitsTag pageUnits)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.AddNewDFTSolution(name, isFlipped, pageUnits);
}

CDFTSolution* CCEtoODBDoc::FindDFTSolution(const FileStruct& file,const CString& name, bool isFlipped)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.FindDFTSolution(name, isFlipped);
}

CDFTSolution* CCEtoODBDoc::GetCurrentDFTSolution(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetCurrentDFTSolution();
}

int CCEtoODBDoc::SetCurrentDFTSolution(const FileStruct& file,POSITION pos)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.SetCurrentDFTSolution(pos);
}

int CCEtoODBDoc::SetCurrentDFTSolution(const FileStruct& file,const CString& name)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.SetCurrentDFTSolution(name, false/*isFlipped);
}

int CCEtoODBDoc::SetCurrentDFTSolution(const FileStruct& file,CDFTSolution* dftSolution)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.SetCurrentDFTSolution(dftSolution);
}

*/
// Multiple Machine
/*CMachineList* CCEtoODBDoc::CreatMachineList(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.CreatMachineList(*this);
}
*/
/*CMachineList* CCEtoODBDoc::getMachineList(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.getMachineList();
}*/

/*int CCEtoODBDoc::GetMachineCount(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetMachineCount();
}

POSITION CCEtoODBDoc::GetMachineHeadPosition(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetMachineHeadPosition();
}

POSITION CCEtoODBDoc::GetMachineTailPosition(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetMachineTailPosition();
}

CMachine* CCEtoODBDoc::GetMachineHead(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetMachineHead();
}

CMachine* CCEtoODBDoc::GetMachineTail(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetMachineTail();
}

CMachine* CCEtoODBDoc::GetMachineNext(const FileStruct& file,POSITION &pos)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetMachineNext(pos);
}

CMachine* CCEtoODBDoc::GetMachinePrev(const FileStruct& file,POSITION &pos)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetMachinePrev(pos);
}

CMachine* CCEtoODBDoc::FindMachine(const FileStruct& file,const CString& name)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.FindMachine(name);
}

CMachine* CCEtoODBDoc::GetCurrentMachine(const FileStruct& file)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.GetCurrentMachine();
}

int CCEtoODBDoc::SetCurrentMachine(const FileStruct& file,POSITION pos)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.SetCurrentMachine(pos);
}

int CCEtoODBDoc::SetCurrentMachine(const FileStruct& file,const CString& name)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.SetCurrentMachine(name);
}

int CCEtoODBDoc::SetCurrentMachine(const FileStruct& file,CMachine* machine)
{
   CAuxiliaryFileStruct& auxiliaryFile = m_auxiliaryFiles.getDefinedAuxiliaryFileStruct(file);

   return auxiliaryFile.SetCurrentMachine(machine);
}

void CCEtoODBDoc::DeleteMachineOrigin(FileStruct *pcbFile, CString MachineName)
{
   Attrib *atop = NULL, *abot = NULL;  
   Attrib *attribTab[] = {atop,abot};
   
   for(int isBot = 0; isBot < 2 ;isBot++)
   {
      CStringArray param;
      CString keyWord = GetMachineAttributeName(MachineName, isBot?true:false);
      attribTab[isBot] = is_attvalue(this, pcbFile->getBlock()->getDefinedAttributes(), keyWord, 0);
      if(attribTab[isBot])
      {
         CSupString supOption(attribTab[isBot]->getStringValue());
         supOption.ParseQuote(param,",; \t");

         //remove layer         
         LayerStruct *layer = this->getLayer(keyWord);
         if(layer && layer->getLayerIndex() > -1) this->getLayerArray().removeLayer(layer->getLayerIndex());

         //remove keyword and attribute
         int KeyIndex = attribTab[isBot]->getKeywordIndex();
         if(KeyIndex > -1)
         {            
            pcbFile->getBlock()->getDefinedAttributes()->removeAttribute(KeyIndex);  
         }
      }
    
      //Remove Insert and blocks
      if(param.GetCount())
      {         
         int blockNumber = Get_Block_Num(keyWord, -1, 0);
         if(blockNumber > 0)
         {
            POSITION dataPos = FindInsertDataAt(pcbFile, blockNumber);
            if(dataPos) pcbFile->getBlock()->getDataList().RemoveAt(dataPos);
            RemoveBlock(this->getBlockAt(blockNumber));
         }
      }
   }
}

POSITION CCEtoODBDoc::FindInsertDataAt(FileStruct *pcbFile, int blockNumber)
{
   if (!blockNumber) return NULL;

   for (POSITION dataPos = pcbFile->getBlock()->getHeadDataInsertPosition();dataPos;)
   {
      POSITION curPOS = dataPos;
      DataStruct  *originData = pcbFile->getBlock()->getNextDataInsert(dataPos);
      if(originData && originData->getInsert()->getBlockNumber() == blockNumber)
      {
         return curPOS;
      }
   }

   return NULL;
}

*/