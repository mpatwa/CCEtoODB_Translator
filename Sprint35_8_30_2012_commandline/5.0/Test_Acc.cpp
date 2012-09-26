// $Header: /CAMCAD/5.0/Test_Acc.cpp 28    6/17/07 8:54p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "test_acc.h"
#include "graph.h"
#include "attrib.h"
#include "pcbutil.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ResetButtons();
void Untransform(CCEtoODBDoc *doc, Point2 *pnt, SelectStruct *file);

extern CView *activeView; // from CCVIEW.CPP

static int testPreference = 10;
static BlockStruct *testBlock = NULL;
static BOOL OffsetTop, OffsetBottom;
   
static BOOL PlaceAccess(CCEtoODBDoc *doc, BOOL Top);

/******************************************************************************
* OnTaShowTestOffsetsBottom 
*/
void CCEtoODBDoc::OnTaShowTestOffsetsBottom() 
{
/* if (!QueryLicense)
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   } */

   showTAOffsetsBottom = !showTAOffsetsBottom;

   UpdateAllViews(NULL);
}

/******************************************************************************
* OnTaShowTestOffsetsTop
*/
void CCEtoODBDoc::OnTaShowTestOffsetsTop() 
{
/* if (!QueryLicense)
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   } */

   showTAOffsetsTop = !showTAOffsetsTop;

   UpdateAllViews(NULL);
}

/******************************************************************************
* OnTaPlaceTestOffsets
*/
void CCEtoODBView::OnTaPlaceOffsetTop() 
{
   OffsetTop = TRUE;
   OffsetBottom = FALSE;
   TaPlaceOffsets();

   CCEtoODBDoc *doc = GetDocument();
   if (!doc->showTAOffsetsTop)
   {
      doc->showTAOffsetsTop = TRUE;  
      doc->UpdateAllViews(NULL);
   }
}

/******************************************************************************
* CCEtoODBView::OnTaPlaceOffsetBottom
*/
void CCEtoODBView::OnTaPlaceOffsetBottom() 
{
   OffsetTop = FALSE;
   OffsetBottom = TRUE; 
   TaPlaceOffsets();

   CCEtoODBDoc *doc = GetDocument();
   if (!doc->showTAOffsetsBottom)
   {
      doc->showTAOffsetsBottom = TRUE;
      doc->UpdateAllViews(NULL);
   }
}

/******************************************************************************
* CCEtoODBView::OnTaPlaceOffsetBoth
*/
void CCEtoODBView::OnTaPlaceOffsetBoth() 
{
   OffsetTop = TRUE;
   OffsetBottom = TRUE; 
   TaPlaceOffsets();
   CCEtoODBDoc *doc = GetDocument();

   if (!doc->showTAOffsetsTop && !doc->showTAOffsetsBottom)
   {
      doc->showTAOffsetsTop = doc->showTAOffsetsBottom = TRUE;
      doc->UpdateAllViews(NULL);
   }
}

/******************************************************************************
* CCEtoODBView::TaPlaceOffsets
*/
void CCEtoODBView::TaPlaceOffsets() 
{
   CCEtoODBDoc *doc = GetDocument();

   if (doc->nothingIsSelected())
   {
      ErrorMessage("Select a Feature.", "Nothing is Selected");
      return;
   }

   SelectStruct *s = doc->getSelectStack().getAtLevel();

   if (s->getData()->getDataType() != T_INSERT)
   {
      ErrorMessage("Select a legal feature.", "Not a legal feature");
      return;
   }

   if (zoomMode != NoZoom) 
      return;

   ResetButtons();
   if (cursorMode == TAPlaceOffset)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   //HideSearchCursor();
   cursorMode = TAPlaceOffset;
   //ShowSearchCursor();
// ((CMainFrame*)AfxGetMainWnd())->m_PolyToolBar.GetToolBarCtrl().PressButton(ID_EDIT_BULGE);
}

/******************************************************************************
* CCEtoODBView::OnLButtonDown_TAPlaceOffset
*/
void CCEtoODBView::OnLButtonDown_TAPlaceOffset(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   SelectStruct *s = doc->getSelectStack().getAtLevel();

   SelectStruct pinSelect;
   pinSelect.insert_x = s->getData()->getInsert()->getOriginX();
   pinSelect.insert_y = s->getData()->getInsert()->getOriginY();
   pinSelect.rotation = s->getData()->getInsert()->getAngle();
   pinSelect.scale = s->getData()->getInsert()->getScale();
   pinSelect.mirror = s->getData()->getInsert()->getMirrorFlags();

   Point2 point;
   point.x = x;
   point.y = y;
   Untransform(doc, &point, &pinSelect);

   WORD keyword;
   if (OffsetTop)
   {
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_X, 0);
      doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&point.x, SA_OVERWRITE, NULL);

      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_Y, 0);
      doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&point.y, SA_OVERWRITE, NULL);
   }
   if (OffsetBottom)
   {
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_X, 0);
      doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&point.x, SA_OVERWRITE, NULL);

      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_Y, 0);
      doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&point.y, SA_OVERWRITE, NULL);
   }

   //HideSearchCursor();
   doc->DrawEntity(s, 3, FALSE);
   cursorMode = Search;
   //ShowSearchCursor();
}

/******************************************************************************
* OnAddTA_Defaults
*/
void CCEtoODBDoc::OnAddTA_Defaults() 
{
   TestAccessPlacementDefaults dlg;
   dlg.doc = this;
   dlg.m_testPreference = testPreference;
   dlg.testBlock = testBlock;
   if (dlg.DoModal() == IDOK)
   {
      testBlock = dlg.testBlock;
      testPreference = dlg.m_testPreference;
   }
}

/******************************************************************************
* OnAddTA_PlaceAuto
*/
void CCEtoODBDoc::OnAddTA_PlaceAuto() 
{
   ErrorMessage("Auto Not Implemented Yet", "Nothing Done");   
}

/******************************************************************************
* OnAddTA_PlaceTop
*/
void CCEtoODBDoc::OnAddTA_PlaceTop() 
{
   if (!testBlock)
   {
      ErrorMessage("You must select a Test Acces Geometry in the Test Access Settings Dialog.");
      return;
   }
   
   PlaceAccess(this, TRUE);
}

/******************************************************************************
* OnAddTA_PlaceBottom
*/
void CCEtoODBDoc::OnAddTA_PlaceBottom() 
{
   if (!testBlock)
   {
      ErrorMessage("You must select a Test Acces Geometry in the Test Access Settings Dialog.");
      return;
   }

   PlaceAccess(this, FALSE);
}

/******************************************************************************
* OnAddTA_PlaceBoth
*/
void CCEtoODBDoc::OnAddTA_PlaceBoth() 
{
   if (!testBlock)
   {
      ErrorMessage("You must select a Test Acces Geometry in the Test Access Settings Dialog.");
      return;
   }

   PlaceAccess(this, TRUE);
   PlaceAccess(this, FALSE);
}
   
/******************************************************************************
* PlaceAccess
*/
BOOL PlaceAccess(CCEtoODBDoc *doc, BOOL Top)
{   
   SelectStruct *s = doc->getSelectStack().getAtLevel();

   if (s == NULL)
   {
      ErrorMessage("Nothing Selected", "Test Access Offset");
      return FALSE;
   }

   Point2 point;
   Mat2x2 m;
   RotMat2(&m, s->rotation);

   if (s->getData()->getDataType() != T_INSERT)
      return FALSE;

   point.x = s->getData()->getInsert()->getOriginX() * s->scale;

   if (s->mirror) 
      point.x = -point.x;

   point.y = s->getData()->getInsert()->getOriginY() * s->scale;
   TransPoint2(&point, 1, &m, s->insert_x, s->insert_y);

   double _x = point.x, _y = point.y;
   Attrib* attrib;
   WORD keyword;

   if (Top)
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_X, 0);
   else
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_X, 0);

   if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
   {
      _x += attrib->getDoubleValue();
   }

   if (Top)
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_Y, 0);
   else
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_Y, 0);

   if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
   {
      _y += attrib->getDoubleValue();
   }

   FileStruct *file = doc->Find_File(doc->getSelectStack().getAt(0)->filenum);
   doc->PrepareAddEntity(file);
   // TODO : Offsets
   DataStruct *t = Graph_Block_Reference(testBlock->getName(), "", testBlock->getFileNumber(), _x, _y, 0, (!Top), 1, -1, TRUE);
   t->getInsert()->setInsertType(insertTypeTestAccessPoint);

   int entitynum = -1;
   CString netname;

   if (doc->getSelectStack().getLevelIndex() == 0)
   {
      WORD keyword = doc->IsKeyWord(ATT_NETNAME, 0);

      if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         netname = attrib->getStringValue();
      }

      entitynum = s->getData()->getEntityNumber();
   }
   else // comp pin
   {
      // TODO : Comp Pins

   }

   keyword = doc->IsKeyWord(ATT_NETNAME, 0);
   doc->SetAttrib(&t->getAttributesRef(), keyword, VT_STRING, (void*)((const char*)netname), SA_OVERWRITE, NULL);

   keyword = doc->IsKeyWord(ATT_DDLINK, 0);
   doc->SetAttrib(&t->getAttributesRef(), keyword, VT_INTEGER, &entitynum, SA_OVERWRITE, NULL);

   keyword = doc->IsKeyWord(ATT_TESTPREFERENCE, 0);
   doc->SetAttrib(&t->getAttributesRef(), keyword, VT_INTEGER, &testPreference, SA_OVERWRITE, NULL);

   SelectStruct newS;
   newS.setData(t);
   newS.setParentDataList(&file->getBlock()->getDataList());
   newS.filenum = file->getFileNumber();
   newS.insert_x = file->getInsertX();
   newS.insert_y = file->getInsertY();
   newS.scale = file->getScale();
   newS.rotation = file->getRotation();
   newS.mirror = file->isMirrored();
   newS.layer = -1;
   newS.filenum = file->getFileNumber();

   doc->DrawEntity(&newS, 0, TRUE);

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// TestAccessPlacementDefaults dialog
TestAccessPlacementDefaults::TestAccessPlacementDefaults(CWnd* pParent /*=NULL*/)
   : CDialog(TestAccessPlacementDefaults::IDD, pParent)
{
   //{{AFX_DATA_INIT(TestAccessPlacementDefaults)
   m_testPreference = 0;
   //}}AFX_DATA_INIT
}

void TestAccessPlacementDefaults::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TestAccessPlacementDefaults)
   DDX_Control(pDX, IDC_GEOMETRY_LB, m_geometryLB);
   DDX_Text(pDX, IDC_TEST_PREFERENCE, m_testPreference);
   DDV_MinMaxUInt(pDX, m_testPreference, 0, 999);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TestAccessPlacementDefaults, CDialog)
   //{{AFX_MSG_MAP(TestAccessPlacementDefaults)
   ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestAccessPlacementDefaults message handlers
BOOL TestAccessPlacementDefaults::OnInitDialog() 
{
   CDialog::OnInitDialog();

   FillListBox();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void TestAccessPlacementDefaults::FillListBox() 
{
   m_geometryLB.ResetContent();

	int i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;
      if (block->getFlags() & BL_FILE || block->getFlags() & BL_WIDTH || block->getFlags() & BL_APERTURE || 
            block->getFlags() & BL_BLOCK_APERTURE || block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL)
         continue;

      if (block->getBlockType() == BLOCKTYPE_TEST_ACCESSPOINT)
         m_geometryLB.SetItemDataPtr(m_geometryLB.AddString(block->getName()), block);
   }

   for (i=0; i<m_geometryLB.GetCount(); i++)
   {
      BlockStruct *block = (BlockStruct*)m_geometryLB.GetItemDataPtr(i);
      if (block == testBlock)
      {
         m_geometryLB.SetCurSel(i);
         //m_geometryLB.SetTopIndex(n);
      }
   }
}

void TestAccessPlacementDefaults::OnOK() 
{
   int sel = m_geometryLB.GetCurSel();
   if (sel == LB_ERR)
   {
      ErrorMessage("Select a Geometry in the List Box", "Select Test Access Geometry");
      return;
   }

   testBlock = (BlockStruct*)m_geometryLB.GetItemDataPtr(sel);
   
   CDialog::OnOK();
}

void TestAccessPlacementDefaults::OnGenerate() 
{
   CString  name;
   double   size;

   name = "TEST_ACCESS";

   // default 0.05 inches
   size = 0.05 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
   generate_TestAccessGeometry(doc, name, size);

   // only show BLOCKTYPE_TEST_ACCESSPOINT
   FillListBox();
   m_geometryLB.SelectString(-1, name);
}


/******************************************************************************
* OnTaInheritFromFeature
*
void CCEtoODBDoc::OnTaInheritFromFeature() 
{
   if (SelectList.IsEmpty())
   {
      ErrorMessage("You must select a Feature, then all other inserts of that Feature will inherit the offsets.", "Nothing Selected");
      return;
   }

   SelectStruct *s = SelectList.GetHead();
   if (s->p->getDataType() != T_INSERT)
   {
      ErrorMessage("You must select a Feature, then all other inserts of that Feature will inherit the offsets.", "Selected is not an Insert");
      return;
   }


   BOOL Top, Bot;
   double topX, topY, botX, botY;
   WORD keyword;
   void *voidPtr;

   Top = Bot = FALSE;
   topX = topY = botX = botY = 0;

   keyword = IsKeyWord(ATT_TEST_OFFSET_TOP_X, 0);
   if (s->p->getAttributesRef() && s->p->getAttributesRef()->Lookup(keyword, voidPtr))
   {
      Attrib* attrib = (Attrib*)voidPtr;
      topX = attrib->getDoubleValue();
      Top = TRUE;
   }

   keyword = IsKeyWord(ATT_TEST_OFFSET_TOP_Y, 0);
   if (s->p->getAttributesRef() && s->p->getAttributesRef()->Lookup(keyword, voidPtr))
   {
      Attrib* attrib = (Attrib*)voidPtr;
      topY = attrib->getDoubleValue();
      Top = TRUE;
   }

   keyword = IsKeyWord(ATT_TEST_OFFSET_BOT_X, 0);
   if (s->p->getAttributesRef() && s->p->getAttributesRef()->Lookup(keyword, voidPtr))
   {
      Attrib* attrib = (Attrib*)voidPtr;
      botX = attrib->getDoubleValue();
      Bot = TRUE;
   }

   keyword = IsKeyWord(ATT_TEST_OFFSET_BOT_Y, 0);
   if (s->p->getAttributesRef() && s->p->getAttributesRef()->Lookup(keyword, voidPtr))
   {
      Attrib* attrib = (Attrib*)voidPtr;
      botY = attrib->getDoubleValue();
      Bot = TRUE;
   }

   if (!Top && !Bot)
   {
      if (ErrorMessage("Do you want to set all instances back to Zero Offset?", "Selected feature has no offset attributes.", MB_YESNO) != IDYES)
         return;

      Top = Bot = TRUE;
   }

   POSITION pos = s->DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = s->DataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getBlockNumber() != s->p->getInsert()->getBlockNumber())
         continue;

      if (Top)
      {
         keyword = IsKeyWord(ATT_TEST_OFFSET_TOP_X, 0);
         SetAttrib(&data->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&topX, SA_OVERWRITE, NULL);

         keyword = IsKeyWord(ATT_TEST_OFFSET_TOP_Y, 0);
         SetAttrib(&data->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&topY, SA_OVERWRITE, NULL);
      }

      if (Bot)
      {
         keyword = IsKeyWord(ATT_TEST_OFFSET_BOT_X, 0);
         SetAttrib(&data->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&botX, SA_OVERWRITE, NULL);

         keyword = IsKeyWord(ATT_TEST_OFFSET_BOT_Y, 0);
         SetAttrib(&data->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&botY, SA_OVERWRITE, NULL);
      }
   }

   UpdateAllViews(NULL);
}  */

/******************************************************************************
* OnTaPlaceOffsets
*/
void CMainFrame::OnTaPlaceOffsets() 
{
   TestAccess_Offsets *dlg = new TestAccess_Offsets;
   dlg->Create(IDD_TESTACCESS_OFFSETS, NULL);
   dlg->ShowWindow(SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
// TestAccess_Offsets dialog
TestAccess_Offsets::TestAccess_Offsets(CWnd* pParent /*=NULL*/)
   : CDialog(TestAccess_Offsets::IDD, pParent)
{
   //{{AFX_DATA_INIT(TestAccess_Offsets)
   m_botX = _T("");
   m_botY = _T("");
   m_getBot = TRUE;
   m_getTop = TRUE;
   m_topX = _T("");
   m_topY = _T("");
   //}}AFX_DATA_INIT
}

void TestAccess_Offsets::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TestAccess_Offsets)
   DDX_Text(pDX, IDC_BOT_X, m_botX);
   DDX_Text(pDX, IDC_BOT_Y, m_botY);
   DDX_Check(pDX, IDC_GET_BOT, m_getBot);
   DDX_Check(pDX, IDC_GET_TOP, m_getTop);
   DDX_Text(pDX, IDC_TOP_X, m_topX);
   DDX_Text(pDX, IDC_TOP_Y, m_topY);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TestAccess_Offsets, CDialog)
   //{{AFX_MSG_MAP(TestAccess_Offsets)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   ON_BN_CLICKED(IDC_GET, OnGet)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestAccess_Offsets message handlers
void TestAccess_Offsets::OnApply() 
{
   if (!activeView)
      return;

   CCEtoODBDoc *doc = (CCEtoODBDoc*)activeView->GetDocument();

   if (doc->SelectList.IsEmpty())
   {
      ErrorMessage("Nothing is Selected.");
      return;
   }

   UpdateData();

   WORD keyword;
   POSITION pos = doc->SelectList.GetHeadPosition();
   while (pos)
   {
      SelectStruct *s = doc->SelectList.GetNext(pos);

      if (s->getData()->getDataType() != T_INSERT)
         continue;

      double num;

      if (!m_topX.IsEmpty())
      {
         num = atof(m_topX);
         keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_X, 0);
         doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&num, SA_OVERWRITE, NULL);
      }

      if (!m_topY.IsEmpty())
      {
         num = atof(m_topY);
         keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_Y, 0);
         doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&num, SA_OVERWRITE, NULL);
      }

      if (!m_botX.IsEmpty())
      {
         num = atof(m_botX);
         keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_X, 0);
         doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&num, SA_OVERWRITE, NULL);
      }

      if (!m_botY.IsEmpty())
      {
         num = atof(m_botY);
         keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_Y, 0);
         doc->SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_UNIT_DOUBLE, (void*)&num, SA_OVERWRITE, NULL);
      }
   }

   doc->UpdateAllViews(NULL);
}

void TestAccess_Offsets::OnGet() 
{
   UpdateData();

   if (!m_getTop)
      m_topX = m_topY = "";
   
   if (!m_getBot)
      m_botX = m_botY = "";

   UpdateData(FALSE);

   if (!activeView)
      return;

   CCEtoODBDoc *doc = (CCEtoODBDoc*)activeView->GetDocument();

   if (doc->nothingIsSelected())
   {
      ErrorMessage("Nothing is Selected.");
      return;
   }

   SelectStruct *s = doc->getSelectStack().getAtLevel();

   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   WORD keyword;
   Attrib* attrib;
   double num;

   if (m_getTop)
   {
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_X, 0);
      num = 0;

      if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         num = attrib->getDoubleValue();
      }

      m_topX.Format("%.*lf", decimals, num);

      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_TOP_Y, 0);
      num = 0;

      if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         num = attrib->getDoubleValue();
      }

      m_topY.Format("%.*lf", decimals, num);
   }

   if (m_getBot)
   {
      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_X, 0);
      num = 0;

      if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         num = attrib->getDoubleValue();
      }

      m_botX.Format("%.*lf", decimals, num);

      keyword = doc->IsKeyWord(ATT_TEST_OFFSET_BOT_Y, 0);
      num = 0;

      if (s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         num = attrib->getDoubleValue();
      }

      m_botY.Format("%.*lf", decimals, num);
   }

   UpdateData(FALSE);
}

void TestAccess_Offsets::OnCancel() 
{
   DestroyWindow();  
}

void TestAccess_Offsets::PostNcDestroy() 
{
   delete this;
   CDialog::PostNcDestroy();
}
