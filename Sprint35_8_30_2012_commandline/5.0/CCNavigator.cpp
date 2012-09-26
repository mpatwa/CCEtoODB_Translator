// $Header: /CAMCAD/5.0/CAMCADNavigator.cpp 174   6/17/07 8:50p Kurt Van Ness $

#include "StdAfx.h"
#include "GeneralDialog.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "MainFrm.h"
#include "CAMCADNavigator.h"
#include "find.h" 
#include "atlbase.h"
#include "atlwin.h"
#include "DFT.h"
#include "Sch_List.h"
#include "Graph.h"
#include "Edit.h"
#include "Variant.h"
#include "RealPart.h"
#include "PcbUtil.h"
#include "MultipleMachine.h"
#include "CamCadDatabase.h"
#include "odbc_lib.h"

// Case 1156: switched all override colors to use a session base highlight color on data's

extern CView *activeView; // from CCVIEW.CPP
extern CEditDialog *editDlg;
extern EditNothing *nothingDlg;
extern BOOL Editing;

BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, double *xmin, double *xmax, double *ymin, double *ymax, int SELECT_COMP);
BOOL ComponentsExtentsByDataStruct(CCEtoODBDoc *doc, DataStruct *data, FileStruct *file, double *xmin, double *xmax, double *ymin, double *ymax);
int  PanReference(CCEtoODBView *view, const char *ref) ;
int Hide_Net(CCEtoODBDoc *doc, int netValueIndex, int hide);
void Hide_Nets_All(CCEtoODBDoc *doc, int hide);
NetStruct *FindNet(FileStruct *file, CString netname);
//void *FindEntity(CCEtoODBDoc *doc, long entityNum, EEntityType &entityType);
void ShowNothingDlg();


static bool fileBlockErrorReported = false;

//typedef CTypedPtrArray<CPtrArray, DataStruct*> CNavCompArray;
//CNavCompArray navCompArray;
//int navCompArrayCount;

//typedef CTypedPtrArray<CPtrArray, NetStruct*> CNavNetArray;
//CNavNetArray navNetArray;
//int navNetArrayCount;

//typedef CTypedPtrArray<CPtrArray, DataStruct*> CNavPlacedProbeArray;
//CNavPlacedProbeArray navPlacedProbeArray;
//int navPlacedProbeArrayCount;

//typedef CTypedPtrArray<CPtrArray, DataStruct*> CNavUnplacedProbeArray;
//CNavUnplacedProbeArray navUnplacedProbeArray;
//int navUnplacedProbeArrayCount;

CMainFrame *frame = NULL;

static WINDOWPLACEMENT editPlacement;

class SortListHelper
{
public:
   void *voidptr; // could be anything, DataStruct*, NetStruct*, etc.
   int typeTag;   // to identify what it is
};

/**********************************************
* MoveTreeItem
*/
/*HTREEITEM MoveTreeItem(CTreeCtrl& tree, HTREEITEM hItem, HTREEITEM hItemTo, 
             BOOL bCopyOnly = FALSE, PFNMTICOPYDATA pfnCopyData = NULL, 
             HTREEITEM hItemPos = TVI_FIRST) 
{
   if (hItem == NULL || hItemTo == NULL)
      return NULL;

   if (hItem == hItemTo || hItemTo == frame->getNavigator().getTree()->GetParentItem(hItem))
      return hItem;

   // check we're not trying to move to a descendant
   HTREEITEM hItemParent = hItemTo;
   while (hItemParent != TVI_ROOT && (hItemParent = frame->getNavigator().getTree()->GetParentItem(hItemParent)) != NULL)
      if (hItemParent == hItem)
      return NULL;

   CCEtoODBDoc* doc = frame->getNavigator().getDoc();
   CCamCadData& camCadData = doc->getCamCadData();

   CString UserNameTimeStamp( frame->getNavigator().GetWhoAndWhen() );

   WORD PROBE_PLACEMENT = doc->RegisterKeyWord("PROBE_PLACEMENT", 0, VT_STRING);
   WORD DATALINK =        doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);
   WORD CREATEDBY =       doc->RegisterKeyWord("CREATED_BY", 0, VT_STRING);
   WORD MOVEDBY   =       doc->RegisterKeyWord("MOVED_BY", 0, VT_STRING);

   BOOL placingUnplaced = FALSE;
   TreeItemData *probeItemData = (TreeItemData*)frame->getNavigator().getTree()->GetItemData(hItem);
   DataStruct* probeData = (DataStruct*)probeItemData->voidPtr;
   Attrib *netAttrib = is_attvalue(doc, probeData->getAttributesRef(), ATT_NETNAME, 1);

   if (!netAttrib)
      return 0;

   CString netName = netAttrib->getStringValue();
   int nImage, nImageSel;
   frame->getNavigator().getTree()->GetItemImage(hItemTo, nImage, nImageSel);

   if (nImage == ICON_CP || nImage == ICON_VIA)
   {
      TreeItemData *netItemData = (TreeItemData*)frame->getNavigator().getTree()->GetItemData(frame->getNavigator().getTree()->GetParentItem(hItemTo));
      CString nodeNetName = (((NetStruct*)netItemData->voidPtr)->getNetName());

      if (netName.CompareNoCase(nodeNetName))
         return 0;

      HTREEITEM child = frame->getNavigator().getTree()->GetChildItem(hItemTo);
      int cnt = 0;

      while (child)
      {
         cnt++;
         if (cnt >=2)
            return 0;
         child = frame->getNavigator().getTree()->GetNextSiblingItem(child);
      }

      CString placement = "";
      Attrib* plcdAttrib = is_attvalue(doc, probeData->getAttributesRef(), "PROBE_PLACEMENT", 1);

      if (!plcdAttrib)
         return 0;

      placement = plcdAttrib->getStringValue();

      if (!placement.CompareNoCase("unplaced"))
      {
         //if placing an unplaced probe
         placingUnplaced = TRUE;
      }

      //if moving to CP or VIA, then place it
      CompPinStruct* cp = NULL;
      DataStruct* via = NULL;
      frame->getNavigator().getTree()->SetItemImage(hItem, ICON_PLCD, ICON_PLCD);

      if (nImage == ICON_CP)
      {
         TreeItemData *cpItemData = (TreeItemData*)frame->getNavigator().getTree()->GetItemData(hItemTo);
         cp = (CompPinStruct*)cpItemData->voidPtr;
      }
      else if (nImage == ICON_VIA)
      {
         TreeItemData *viaItemData = (TreeItemData*)frame->getNavigator().getTree()->GetItemData(hItemTo);
         via = (DataStruct*)viaItemData->voidPtr;
      }

      frame->getNavigator().getTree()->GetItemImage(hItem, nImage, nImageSel);
      Attrib *attrib = NULL;
      long entityNum = -1;
      POSITION dataPos = frame->getNavigator().getCurFile()->getBlock()->getDataList().GetHeadPosition();

      while (dataPos)
      {
         DataStruct* accessData = frame->getNavigator().getCurFile()->getBlock()->getDataList().GetNext(dataPos);

			if (accessData == NULL || !accessData->isInsertType(insertTypeTestAccessPoint))
				continue;

         attrib = is_attvalue(doc, accessData->getAttributesRef(), ATT_DDLINK, 1);

         if (!attrib)
            continue;

         if (cp || via)
         {
            if (attrib && (cp?attrib->getIntValue() == cp->getEntityNumber():0 || via?attrib->getIntValue() == via->getEntityNumber():0))
            {
               entityNum = accessData->getEntityNumber(); //link to correct access data
               probeData->getInsert()->setOrigin(accessData->getInsert()->getOrigin()); //set its location to that of the access point
               // Case dts0100465000 - set pcb side and mirror to match access point
               probeData->getInsert()->setPlacedBottom( accessData->getInsert()->getPlacedBottom() );
               probeData->getInsert()->setMirrorFlags(  accessData->getInsert()->getMirrorFlags()  );
               // Lingering bug I think, here and further below.
               // We have changed probe side, and may change access marker side (especially further below).
               // So probably should propagate change into TEST_ACCESS attrib on target. See CEditProbe::OnBnClickedOk()
               
               // when manually placing an unplaced probe, use the smallest diameter probe 
               if (placingUnplaced && doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile())) != NULL &&
                  doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan())
               {
                  POSITION topPos = NULL, botPos = NULL;
                  CDFTProbeTemplate* probeTop = doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan()->GetProbes().GetSmallest_UsedTopProbes(topPos);
                  CDFTProbeTemplate* probeBot = doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan()->GetProbes().GetSmallest_UsedBotProbes(botPos);

                  CString topGeomName = probeTop?probeTop->GetName():"";
                  BlockStruct *topBlock = !topGeomName.IsEmpty()?doc->Find_Block_by_Name(topGeomName+"_Top", -1):NULL;

                  CString botGeomName = probeBot?probeBot->GetName():"";
                  BlockStruct *botBlock = !botGeomName.IsEmpty()?doc->Find_Block_by_Name(botGeomName+"_Bot", -1):NULL;

                  if (probeData->getInsert()->getPlacedBottom() && botBlock)
                  {
                     probeData->getInsert()->setBlockNumber(botBlock->getBlockNumber());
                  }
                  else if (topBlock)
                  {
                     probeData->getInsert()->setBlockNumber(topBlock->getBlockNumber());
                  }
               }

               break;
            }
         }
      }

      if (entityNum != -1) //access point available
      {
         void* voidPtr = (void*)"Placed";
         probeData->setAttrib(camCadData, PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
         probeData->setHidden(false);
         probeData->setAttrib(camCadData, DATALINK, valueTypeInteger, (void*)&entityNum, attributeUpdateOverwrite, NULL);
         probeData->setAttrib(camCadData, MOVEDBY,  valueTypeString,  (void*)UserNameTimeStamp.GetBuffer(0), attributeUpdateOverwrite, NULL);
      }
      else //no access point available, create one
      {
			DataStruct* access = NULL;
         if (cp)
         {
            // Case dts0100465000 - set pcb side and mirror to match comp pin (not pre-existing probe, as probe might change sides)
            // Try to get an intelligent access side, remember that Access Analysis says this is NOT accessible, or we would
            // have already found an access marker in code up above.
            bool accessBottom = cp->getMirror()?true:false; // punt, default is same side as item
            // If visible on both sides then use side probe is already on
            if (cp->isVisibleBoth())
               accessBottom = probeData->getInsert()->getPlacedBottom();
            else if (cp->isVisibleTop())
               accessBottom = false;
            else if (cp->isVisibleBottom())
               accessBottom = true;
            // else it is not visible, but we're letting user run with it anyway

            access = PlaceTestAccessPoint(doc, frame->getNavigator().getCurFile()->getBlock(), "", cp->getOriginX(), cp->getOriginY(), 
               accessBottom?testSurfaceBottom:testSurfaceTop,netName, "SMD", cp->getEntityNumber(), 0, doc->getSettings().getPageUnits());   
            access->setAttrib(camCadData, CREATEDBY, valueTypeString, (void*)UserNameTimeStamp.GetBuffer(0), attributeUpdateOverwrite, NULL);
            
            void* voidPtr = (void*)"Placed";
            probeData->setAttrib(camCadData, PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
            probeData->setHidden(false);
            entityNum = access->getEntityNumber();
            probeData->setAttrib(camCadData, DATALINK, valueTypeInteger, (void*)&entityNum, attributeUpdateOverwrite, NULL);
            probeData->getInsert()->setOrigin(access->getInsert()->getOrigin());

            // Case dts0100465000 - set probe's pcb side and mirror to match access point
            probeData->getInsert()->setPlacedBottom( access->getInsert()->getPlacedBottom() );
            probeData->getInsert()->setMirrorFlags(  access->getInsert()->getMirrorFlags()  );

            probeData->setAttrib(camCadData, MOVEDBY, valueTypeString, (void*)UserNameTimeStamp.GetBuffer(0), attributeUpdateOverwrite, NULL);
            
            
            // when manually placing an unplaced probe, use the smallest diameter probe 
            if (placingUnplaced && doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile())) != NULL &&
               doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan())
            {
               POSITION topPos = NULL, botPos = NULL;
               CDFTProbeTemplate* probeTop = doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan()->GetProbes().GetSmallest_UsedTopProbes(topPos);
               CDFTProbeTemplate* probeBot = doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan()->GetProbes().GetSmallest_UsedBotProbes(botPos);

               CString topGeomName = probeTop?probeTop->GetName():"";
               BlockStruct *topBlock = !topGeomName.IsEmpty()?doc->Find_Block_by_Name(topGeomName+"_Top", -1):NULL;

               CString botGeomName = probeBot?probeBot->GetName():"";
               BlockStruct *botBlock = !botGeomName.IsEmpty()?doc->Find_Block_by_Name(botGeomName+"_Bot", -1):NULL;

               if (probeData->getInsert()->getPlacedBottom() && botBlock)
               {
                  probeData->getInsert()->setBlockNumber(botBlock->getBlockNumber());
               }
               else if (topBlock)
               {
                  probeData->getInsert()->setBlockNumber(topBlock->getBlockNumber());
               }
            }
         }
         else if (via)
         {
            // Try to get a decent access side. Remember, AA said it has NONE, else we would not get to
            // this spot in the code, so there is no point in checking TEST_ACCESS attrib. So looks like
            // all we got is insert side and technology style.
            bool accessBottom = via->getInsert()->getPlacedBottom();  // same side if SMD (more accurately, not THRU)
            int technologyKw = frame->getNavigator().getDoc()->getStandardAttributeKeywordIndex(standardAttributeTechnology);
            CString technologyValue;
            Attrib* attrib = NULL;
            if (via->lookUpAttrib(technologyKw, attrib))
               technologyValue = attrib->getStringValue();
            if (technologyValue.CompareNoCase("THRU") == 0)
               accessBottom = !via->getInsert()->getPlacedBottom();  // access opposite side for THRU

            access = PlaceTestAccessPoint(doc, frame->getNavigator().getCurFile()->getBlock(), "", via->getInsert()->getOriginX(), via->getInsert()->getOriginY(), 
               accessBottom?testSurfaceBottom:testSurfaceTop, netName, "SMD", via->getEntityNumber(), 0, doc->getSettings().getPageUnits()); 
            access->setAttrib(camCadData, CREATEDBY, valueTypeString, (void*)UserNameTimeStamp.GetBuffer(0), attributeUpdateOverwrite, NULL);

            
            void* voidPtr = (void*)"Placed";
            probeData->setAttrib(camCadData, PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
            probeData->setHidden(false);

            entityNum = access->getEntityNumber();
            probeData->setAttrib(camCadData, DATALINK, valueTypeInteger, (void*)&entityNum, attributeUpdateOverwrite, NULL);
            probeData->getInsert()->setOrigin(access->getInsert()->getOrigin());

            probeData->setAttrib(camCadData, MOVEDBY, valueTypeString, (void*)UserNameTimeStamp.GetBuffer(0), attributeUpdateOverwrite, NULL);


            // when manually placing an unplaced probe, use the smallest diameter probe 
            if (placingUnplaced && doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile())) != NULL &&
               doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan())
            {
               POSITION topPos = NULL, botPos = NULL;             
               CDFTProbeTemplate* probeTop = doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan()->GetProbes().GetSmallest_UsedTopProbes(topPos);
               CDFTProbeTemplate* probeBot = doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan()->GetProbes().GetSmallest_UsedBotProbes(botPos);

               CString topGeomName = probeTop?probeTop->GetName():"";
               BlockStruct *topBlock = !topGeomName.IsEmpty()?doc->Find_Block_by_Name(topGeomName+"_Top", -1):NULL;

               CString botGeomName = probeBot?probeBot->GetName():"";
               BlockStruct *botBlock = !botGeomName.IsEmpty()?doc->Find_Block_by_Name(botGeomName+"_Bot", -1):NULL;

               if (probeData->getInsert()->getPlacedBottom() && botBlock)
               {
                  probeData->getInsert()->setBlockNumber(botBlock->getBlockNumber());
               }
               else if (topBlock)
               {
                  probeData->getInsert()->setBlockNumber(topBlock->getBlockNumber());
               }
            }
         }

			// Case 1773, Add new Access Point to DFT solution
			// It is a maverick access point, brought into being by the whim of the user.
			// No proper access analysis was applied by CAMCAD.
			if (access != NULL)
			{
				CDFTSolution *curSolution = doc->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()));
				if (curSolution != NULL)
				{
					CAccessAnalysisSolution *aaSol = curSolution->GetAccessAnalysisSolution();
					if (aaSol != NULL)
					{
						CAANetAccess* netAccess = aaSol->GetNetAccess(netName);
						if (netAccess != NULL)
						{
							netAccess->AddMaverick(access);
						}
					}
				}
			}

		}
  }
  else if (nImage == ICON_NET)
  {
      TreeItemData *netItemData = (TreeItemData*)frame->getNavigator().getTree()->GetItemData(hItemTo);
      CString nodeNetName = ((NetStruct*)netItemData->voidPtr)->getNetName();

      if (netName.CompareNoCase(nodeNetName))
         return 0;

      //if moving to net, unplace it
      frame->getNavigator().getTree()->SetItemImage(hItem, ICON_UNPLCD, ICON_UNPLCD);
      //unplace it
      void* voidPtr = (void*)"Unplaced";
      probeData->setAttrib(camCadData, PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
      probeData->setAttrib(camCadData, MOVEDBY, valueTypeString, (void*)UserNameTimeStamp.GetBuffer(0), attributeUpdateOverwrite, NULL);


      probeData->getAttributesRef()->deleteAttribute(DATALINK);
      probeData->getInsert()->setOrigin(0.,0.);
  }
  else
  {
     return NULL;
  }

  // copy items to new location, recursively, then delete old heirarchy
  // get text, and other info
  CString sText = frame->getNavigator().getTree()->GetItemText(hItem);
  TVINSERTSTRUCT tvis;
  tvis.item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | 
         TVIF_SELECTEDIMAGE | TVIF_STATE;
  tvis.item.hItem = hItem;
  // we don't want to copy selection/expanded state etc
  tvis.item.stateMask = (UINT)-1 & ~(TVIS_DROPHILITED | TVIS_EXPANDED | 
         TVIS_EXPANDEDONCE | TVIS_EXPANDPARTIAL | TVIS_SELECTED);
  frame->getNavigator().getTree()->GetItem(&tvis.item);
  tvis.hParent = hItemTo;
  tvis.hInsertAfter = hItemPos;

  // if we're only copying, then ask for new data
  if (bCopyOnly && pfnCopyData != NULL)
     tvis.item.lParam = pfnCopyData(*frame->getNavigator().getTree(), hItem, tvis.item.lParam);

  HTREEITEM hItemNew = frame->getNavigator().getTree()->InsertItem(&tvis);
  frame->getNavigator().getTree()->SetItemText(hItemNew, sText);
  // now move children to under new item
  HTREEITEM hItemChild = frame->getNavigator().getTree()->GetChildItem(hItem);

  while (hItemChild != NULL)
  {
    HTREEITEM hItemNextChild = frame->getNavigator().getTree()->GetNextSiblingItem(hItemChild);
//    MoveTreeItem(*(frame->getNavigator().getTree()), hItemChild, hItemNew, bCopyOnly, pfnCopyData);
    hItemChild = hItemNextChild;
  }

  if (! bCopyOnly)
  {
    // clear item data, so nothing tries to delete stuff based on lParam
    //frame->getNavigator().getTree()->SetItemData(hItem, 0);
     int nImage, nSelImage;
     frame->getNavigator().getTree()->GetItemImage(hItem, nImage, nSelImage);
     frame->getNavigator().SetTreeItemData(frame->getNavigator().getTree(), hItem, frame->getNavigator().getTree()->GetItemText(hItem),
        nImage, 0);
    // no (more) children, so we can safely delete top item
    frame->getNavigator().getTree()->DeleteItem(hItem);
  }

  /*if (runEditProbe)
  {
      CEditProbe editProbeDlg(doc);
      editProbeDlg.DoModal();
  }
  
  frame->getNavigator().CountNetsWithAccess(doc);
  frame->getNavigator().UpdateProbes(doc);
  doc->UpdateAllViews(NULL);

  return hItemNew;
}

*/
/******************************************************************************
* TreeSortCompareFunc
   return negative if lParam1 comes before lParam2
*/
static int CALLBACK TreeSortCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   TreeItemData *itemData1 = (TreeItemData*)lParam1;
   TreeItemData *itemData2 = (TreeItemData*)lParam2;

   if (itemData1->sortKey.MakeUpper() < itemData2->sortKey.MakeUpper())
      return -1;
   else if (itemData1->sortKey.MakeUpper() == itemData2->sortKey.MakeUpper())
      return 0;
   else
      return 1;
}
/******************************************************************************
*listSortFunc
   return negative if lParam1 comes before lParam2
*/
static int listSortFunc(const void* elem1,const void* elem2)
{
   SElement *element1 = (SElement*)elem1, *element2 = (SElement*)elem2;


   CString item1 = *element1->pString, item2 = *element2->pString;

   if (item1.MakeUpper() < item2.MakeUpper())
      return -1;
   else if (item1.MakeUpper() == item2.MakeUpper())
      return 0;
   else
      return 1;
}


////////////////////////////////////////////////////////////

void CCEtoODBDoc::OnPcbCAMCADNavigator()
{
}

// CAMCADNavigator dialog

IMPLEMENT_DYNAMIC(CAMCADNavigator, CDialogBar)
CAMCADNavigator::CAMCADNavigator(CWnd* pParent /*=NULL*/)
   : CDialogBar()
   , m_components(false)
   , m_placedProbes(false)
   , m_unplacedProbes(false)
   , m_compMode(navigatorModeComponents)
   , ShowSelectedOnly(0)
   , m_sVariantName("")
   , m_sMachineName("")
   , m_netLookupMap(10, false)
   , m_RightClickItem(NULL)
   , m_curZoomMargin(1)
   , m_maxZoomMarginSetting(20)
{
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 32, 0);
   m_camCadPinMap = NULL;

   ResetCounts();
}

CAMCADNavigator::CAMCADNavigator(CCEtoODBDoc *document, CWnd* pParent /*=NULL*/)
   : CDialogBar()
   , m_components(false)
   , m_placedProbes(false)
   , m_unplacedProbes(false)
   , m_compMode(navigatorModeComponents)
   , ShowSelectedOnly(0)
   , m_RightClickItem(NULL)
   , m_curZoomMargin(1)
   , m_maxZoomMarginSetting(20)
{
   m_doc = document;
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 32, 0);
   m_camCadPinMap = NULL;

   ResetCounts();
}

CAMCADNavigator::~CAMCADNavigator()
{
   m_imageList->DeleteImageList();

   delete m_imageList;
   delete m_camCadPinMap;
}

void CAMCADNavigator::DoDataExchange(CDataExchange* pDX)
{
   CDialogBar::DoDataExchange(pDX);

   int compModeInt = m_compMode;

   DDX_Control(pDX, IDC_TREE_NAVIGATOR, m_tree);
   DDX_Radio(pDX, IDC_RADIO_COMPMODE, compModeInt);
   DDX_Control(pDX, IDC_EDIT_NAV_FIND, m_findText);
   DDX_Control(pDX, IDC_LIST, m_list);
   DDX_Control(pDX, IDC_ZoomSlider, m_zoomSlider);
   DDX_Text(pDX, IDC_EDIT_VARIANT, m_sVariantName);
   DDX_Control(pDX, IDC_LIST_MACHINESIDE, m_machineSide);

   if (pDX->m_bSaveAndValidate)
   {
      m_compMode = (NavigatorModeTag)compModeInt;
   }
}


BEGIN_MESSAGE_MAP(CAMCADNavigator, CDialogBar)
   ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NAVIGATOR, OnTvnSelchangedNavigatorTree)
   ON_BN_CLICKED(IDC_RADIO_NETMODE, OnBnClickedNetMode)
   ON_BN_CLICKED(IDC_RADIO_COMPMODE, OnBnClickedCompMode)
   ON_BN_CLICKED(IDC_RADIO_MANFMODE, OnBnClickedManufactMode)
   ON_BN_CLICKED(IDC_RADIO_BOARDMODE, OnBnClickedRadioBoardmode)
   ON_LBN_SELCHANGE(IDC_LIST, OnLbnSelchangeList)
   ON_BN_CLICKED(IDC_SHOW_SELECTED, OnBnClickedShowSelected)
   ON_BN_CLICKED(IDC_RADIO_UNPLACEDPROBES, OnBnClickedRadioUnplacedProbes)
   ON_BN_CLICKED(IDC_RADIO_PLACEDPROBES, OnBnClickedRadioPlacedProbes)
   ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_NAVIGATOR, OnTvnBegindragTreeNavigator)
   //ON_NOTIFY(WM_TIMER, IDC_TREE_NAVIGATOR, OnTimer)

   ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_ZoomSlider, OnNMReleasedcaptureZoomslider)

   ON_WM_TIMER()
   ON_WM_QUERYDRAGICON()
   ON_WM_MOUSEMOVE()
   ON_WM_CANCELMODE()
   ON_WM_LBUTTONUP()
   ON_NOTIFY(NM_RCLICK, IDC_TREE_NAVIGATOR, OnNMRclickTreeNavigator)
   
   ON_BN_CLICKED(IDC_NETSACCESS, OnBnClickedNetsaccess)
   ON_BN_CLICKED(IDC_NETSNOACCESS, OnBnClickedNetsnoaccess)
   ON_BN_CLICKED(IDC_BUTTON_VARIANT_CHANGE, OnBnClickedButtonVariantChange)

   ON_CBN_SELCHANGE(IDC_LIST_MACHINESIDE, OnCbnSelchangeMachineList)
END_MESSAGE_MAP()

// CAMCADNavigator message handlers

/******************************************************************************
* CAMCADNavigator::PostNcDestroy
*
void CAMCADNavigator::OnDestroy() 
{
   navigator = NULL;
   delete this;
   CDialogBar::OnClose();
   //CDialogBar::OnDestroy
}*/

/******************************************************************************
*/
void CAMCADNavigator::ResetCounts()
{
   netCount  = 0;
   compCount = 0;
   m_manfCount = 0;
   netsWithAccessCnt = 0;
   netsWithNoAccessCnt = 0;
   placedProbesCount = 0;
   unplacedProbesCount = 0;
   m_boardCount = 0;
}

/******************************************************************************
*/
void CAMCADNavigator::FillBoardList(FileStruct *panelFile)
{
   CInsertTypeMask insertTypes(insertTypePcb);
   m_boardCount = FillInsertList(panelFile, insertTypes);
}

/******************************************************************************
*/
void CAMCADNavigator::FillCompList(FileStruct *pcbFile)
{
   CInsertTypeMask insertTypes(insertTypePcbComponent, insertTypeDie);
   compCount = FillInsertList(pcbFile, insertTypes);
}

/******************************************************************************
*/
void CAMCADNavigator::FillManufactList(FileStruct *pcbFile)
{
   CInsertTypeMask insertTypes(insertTypeTestPoint, insertTypeFiducial, insertTypeDrillTool);
   m_manfCount = FillInsertList(pcbFile, insertTypes);
}

/******************************************************************************
*/
int CAMCADNavigator::FillInsertList(FileStruct *pcbFile, CInsertTypeMask insertTypes)
{
   m_list.ShowWindow(SW_HIDE);
   
   if (!pcbFile)
      return 0;

   int noNameCnt = 0;
   int itemCnt = 0;

   m_list.ResetContent();
   
   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();

   CMapStringToPtr temp;

   BlockStruct *block = pcbFile->getBlock();

   // Case 2126 - We used to check block type here, requiring block be blockTypePcb. But
   // some importers are not setting blockTypePcb on the blocks that the "file" references.
   // This caused the blocks to be skipped, and components seen on the screen not appear in
   // the Navigator. As of this writing, there seems no particular reason to require a 
   // particular block type. If the block exists, then scan it for inserts, regardless of
   // what the block type flags might indicate.

   if (block != NULL)
   {
      for (CDataListIterator insertIterator(*block,insertTypes);insertIterator.hasNext();)
      {
         DataStruct* data = insertIterator.getNext();
         CString refName = data->getInsert()->getRefname();

         if (refName.IsEmpty())
         {
            refName.Format("Blank Refname %d", ++noNameCnt);
         }

         if (!refName.IsEmpty())
         {
            SortListHelper *helper = new SortListHelper;
            helper->voidptr = data;
            helper->typeTag = dataTypeInsert;

            itemCnt++;
            temp.SetAt(refName, (void*)helper);
         }
      }
   }

   SortList(temp);
   m_list.UpdateHExtent();
   m_list.ShowWindow(SW_SHOW);
   return itemCnt;
}

/******************************************************************************
* CAMCADNavigator::FillNetList
*/
void CAMCADNavigator::FillNetList()
{
   m_list.ShowWindow(SW_HIDE);
   m_list.ResetContent();
   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();
   
   CMapStringToPtr temp;
   netCount = 0;

   POSITION netPos = m_curFile->getNetList().GetHeadPosition();

   while (netPos)
   {
      NetStruct* net = m_curFile->getNetList().GetNext(netPos);


      if (!net)
         continue;

      SortListHelper *helper = new SortListHelper;
      helper->voidptr = net;
      helper->typeTag = dataTypeUndefined; // only dataTypeInsert matters at the moment

      CString netName = net->getNetName();

      //m_list.SetItemData(m_list.AddString(netName), (DWORD)net);
      netCount++;
      temp.SetAt(netName, (void*)helper);
   }

   SortList(temp);
   m_list.UpdateHExtent();
   m_list.ShowWindow(SW_SHOW);
}

/**********************************************************************
*/
void CAMCADNavigator::ApplyMargin(int margin, double *xmin, double *xmax, double *ymin, double *ymax)
{
   ApplyMargin((double)margin, xmin, xmax, ymin, ymax);
}

 /**********************************************************************
*/
void CAMCADNavigator::ApplyMargin(double margin, double *xmin, double *xmax, double *ymin, double *ymax)
{
   
   // Reverse direction of the slider value, we want most zoom to the right, least to the left.
   margin = (double)m_maxZoomMarginSetting - margin;

   // This gives a nice feel to the slider, it was too "sensitive" at the low end when using
   // straight linear values, i.e. too much jumpt from 0 to 1, 1 to 2, and then the higher numbers
   // numbers all came out too much the same.
   margin = (margin * margin) / (double)m_maxZoomMarginSetting;

   double marginSizeX = (m_doc->getSettings().getXmax() - m_doc->getSettings().getXmin()) * margin / 100.0;
   double marginSizeY = (m_doc->getSettings().getYmax() - m_doc->getSettings().getYmin()) * margin / 100.0;

   double marginSize = max(marginSizeX, marginSizeY);

   *xmin -= marginSize;
   *xmax += marginSize;        
   *ymin -= marginSize;
   *ymax += marginSize; 
}

/**********************************************************************
*/
void CAMCADNavigator::ZoomPanToComponent(DataStruct *data)
{
   if (data != NULL && data->getDataType() == dataTypeInsert)
   {
      if (!activeView)
         return;

      if (isWindow())
         UpdateData();

      CString m_ref = data->getInsert()->getRefname();

      // Default extent is insert location, in case refname is blank
      double xmin, xmax, ymin, ymax;
      xmin = xmax = data->getInsert()->getOriginX();
      ymin = ymax = data->getInsert()->getOriginY();

      if (m_ref.IsEmpty() && (m_curFile->getResultantMirror(m_doc->getBottomView()) != data->getInsert()->getGraphicMirrored()))
               xmin = xmax = -xmax;

      if (!m_ref.IsEmpty() && !ComponentsExtents(m_doc, m_ref, &xmin, &xmax, &ymin, &ymax, FALSE))
      {
         PanReference(((CCEtoODBView*)activeView), m_ref);
         ErrorMessage("Can not zoom/pan to item, item has no extents", "Panning");
      }
      else
      {
         ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);

         // Comp (by refname) in component tree
         ZoomPanTo(xmin, xmax, ymin, ymax, data->getInsert());
      }
   }
}

/**********************************************************************
*/
void CAMCADNavigator::ZoomPanToPin(DataStruct *componentData, DataStruct *pinData)
{

   if (componentData != NULL && pinData != NULL && (pinData->isInsertType(insertTypePin) || pinData->isInsertType(insertTypeDiePin)))
   {
      if (!activeView)
         return;

      Point2 point;
      point.x = componentData->getInsert()->getOriginX();
      point.y = componentData->getInsert()->getOriginY();
      point.bulge = 0;

      DTransform xform;
      xform.x = m_curFile->getInsertX();
      /*if (doc->Bottom)
      xform.x = -xform.x;*/
      xform.y = m_curFile->getInsertY();
      xform.scale = m_curFile->getScale();
      xform.mirror = (m_curFile->getResultantMirror(m_doc->getBottomView()) ? MIRROR_FLIP : 0);
      xform.SetRotation(m_curFile->getRotation());
      xform.TransformPoint(&point);

      Mat2x2 m;

      if (xform.mirror & MIRROR_FLIP)
         RotMat2(&m, -componentData->getInsert()->getAngle());
      else
         RotMat2(&m, componentData->getInsert()->getAngle());

      Point2 point2;
      point2.x = pinData->getInsert()->getOriginX() * componentData->getInsert()->getScale();
      point2.y = pinData->getInsert()->getOriginY() * componentData->getInsert()->getScale();
      int mirror = xform.mirror ^ componentData->getInsert()->getMirrorFlags();

      if (mirror & MIRROR_FLIP)  point2.x = -point2.x;
      /*if (m_curFile->getResultantMirror(doc->Bottom ))
      point2.x = - point2.x;*/

      TransPoint2(&point2, 1, &m, point.x, point.y);

      double left, right, top, bottom;
      GetZoomRegion(point2.x, point2.y, pinData->getInsert(), &left, &right, &bottom, &top);

      // Component pin in component tree
      CPoint2d panPt(point2.x, point2.y);
      ZoomPanTo(left, right, bottom, top, point2.x, point2.y, mirror?flipToBottom:flipToTop, true /*showcross*/);
   }
}

/**********************************************************************
*/
 void CAMCADNavigator::OnTvnSelchangedNavigatorTree(NMHDR *pNMHDR, LRESULT *pResult)
{
   if (docNotPCB)
      return;

   if (!m_doc)
      return;

   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

   HTREEITEM selectedItem = m_tree.GetSelectedItem();
   m_tree.SelectDropTarget(selectedItem);

   if (!selectedItem)
      return;

   TreeItemData *itemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
   DataStruct *data = (DataStruct*)itemData->voidPtr;

   int nImage = 0;
   int nSelImage = 0;

   m_tree.GetItemImage(selectedItem, nImage, nSelImage);

   // if item selected is a component
   if (m_compMode == navigatorModeComponents ||
      m_compMode == navigatorModeManufacturing)
   {
      UnHighLightNets();

      if (!m_tree.GetParentItem(selectedItem))
      {
         ZoomPanToComponent(data);
      }
      else if (m_tree.GetParentItem(selectedItem)) //if selected is a pin
      {
         TreeItemData *itemData = (TreeItemData*)m_tree.GetItemData(m_tree.GetParentItem(selectedItem));
         DataStruct* component = (DataStruct*)itemData->voidPtr;

         TreeItemData *pinItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         DataStruct* pin = (DataStruct*)pinItemData->voidPtr;

         ZoomPanToPin(component, pin);
      }
   }
   else if (m_compMode == navigatorModeNets || m_compMode == navigatorModeNetsWithoutAccess)
   {
      if (!m_tree.GetParentItem(selectedItem)) //if item is a net
      {
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct* net = (NetStruct*)netItemData->voidPtr;

         if (AfxIsValidAddress(net,sizeof(NetStruct)))
         {
            // Make sure there is an extent before zooming
            double left, right, top, bottom;

            if (!NetExtents(m_doc, net->getNetName(), &left, &right, &bottom, &top))
            {
               ErrorMessage("Net has no extents", "Panning to Net");
            }
            else
            {
               ApplyMargin(m_curZoomMargin, &left, &right, &bottom, &top);
               ZoomPanTo(left, right, bottom, top);
            }
         }
      }
      else if (m_tree.GetParentItem(selectedItem) && (nImage == ICON_CP || nImage == ICON_VIA)) //if selected is a pin or via
      {
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct *net = (NetStruct*)netItemData->voidPtr;

         if (!net)
            return;

         if (nImage == ICON_VIA) // vias and bond pads are treated the same
         {
            TreeItemData *viaItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            DataStruct* via = (DataStruct*)viaItemData->voidPtr;

            if (via != NULL && (via->isInsertType(insertTypeVia) || via->isInsertType(insertTypeBondPad)))
            {
               double left, right, top, bottom;
               GetZoomRegion(via->getInsert(), &left, &right, &bottom, &top);
               ZoomPanTo(left, right, bottom, top, &via->getInsert()->getOrigin2d(), via->getInsert()->getGraphicMirrored(), true /*showcross*/, true /*cancel flip*/);
            }
         }
         else
         {
            // Must be a comppin

            TreeItemData *cpItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            CompPinStruct* compPin = (CompPinStruct*)cpItemData->voidPtr;

            if (compPin != NULL)
            {
               double left, right, top, bottom;
               GetZoomRegion(compPin, &left, &right, &bottom, &top);

               ZoomPanTo(left, right, bottom, top, &compPin->getOrigin(), compPin->getMirror()?true:false, true /*showcross*/);
            }
         }
      }
      else if (nImage == ICON_PLCD) //if selected is a placed probe
      {
         double xmin, xmax, ymax, ymin;
         TreeItemData* probeItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         DataStruct* probeData = (DataStruct*)probeItemData->voidPtr;

         if (probeData && activeView)
         {
            if (isWindow())
               UpdateData();

            CString m_ref = probeData->getInsert()->getRefname();

            if (!ComponentsExtents(m_doc, m_ref, &xmin, &xmax, &ymin, &ymax, FALSE))
            {
               PanReference(((CCEtoODBView*)activeView), m_ref);
               ErrorMessage("Component has no extents", "Panning");
            }
            else
            {
               // switched all override colors to use a session base highlight color on data's
               probeData->setHighlightColor(highlightColors[0]);

               ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
               ZoomPanTo(xmin, xmax, ymin, ymax, probeData->getInsert());
            }
         }
      }
   }
   else if (m_compMode == navigatorModePlacedProbes
#ifndef LOAD_ONLY_SELECTED_PROBE_IN_TREE
		      || m_compMode == navigatorModeUnplacedProbes  // in this case placed and unplaced probe trees are the same
#endif
				)
   {
      if (!m_tree.GetParentItem(selectedItem)) //if item is a net
      {
         UnHighLightNets();
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct* net = (NetStruct*)netItemData->voidPtr;

         if (net)
         {
            // Make sure there is an extent before zooming
            double left, right, top, bottom;

            if (!NetExtents(m_doc, net->getNetName(), &left, &right, &bottom, &top))
            {
               ErrorMessage("Net has no extents", "Panning to Net");
            }
            else
            {
               CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
               highlightNet(*net,RGB(255,128,128));

               ApplyMargin(m_curZoomMargin, &left, &right, &bottom, &top);
               ZoomPanTo(left, right, bottom, top);
            }
         }
      }
      else if (m_tree.GetParentItem(selectedItem) && (nImage == ICON_CP || nImage == ICON_VIA)) //if selected is a pin or via
      {
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct *net = (NetStruct*)netItemData->voidPtr;

         if (!net)
            return;

         if (nImage == ICON_VIA) // vias and bond pads are treated the same
         {
            TreeItemData *viaItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            DataStruct* via = (DataStruct*)viaItemData->voidPtr;

            if (via != NULL && (via->isInsertType(insertTypeVia) || via->isInsertType(insertTypeBondPad)))
            {
               double left, right, top, bottom;
               GetZoomRegion(via->getInsert(), &left, &right, &bottom, &top);

               ZoomPanTo(left, right, bottom, top, &via->getInsert()->getOrigin2d(), via->getInsert()->getGraphicMirrored(), true /*showcross*/, true /*cancel flip*/);
            }
         }
         else
         {
            // CompPin
            TreeItemData *cpItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            CompPinStruct* compPin = (CompPinStruct*)cpItemData->voidPtr;

            if (compPin != NULL)
            {
               double left, right, top, bottom;
               GetZoomRegion(compPin, &left, &right, &bottom, &top);

               ZoomPanTo(left, right, bottom, top, &compPin->getOrigin(), compPin->getMirror()?true:false, true /*showcross*/);
            }
         }
      }
      else if (nImage == ICON_PLCD) //if selected is a placed probe
      {
         double xmin, xmax, ymax, ymin;
         TreeItemData* probeItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         DataStruct* probeData = (DataStruct*)probeItemData->voidPtr;

         if (probeData && activeView)
         {
            if (isWindow())
               UpdateData();

            CString m_ref = probeData->getInsert()->getRefname();

            if (!ComponentsExtentsByDataStruct(m_doc, probeData, m_curFile, &xmin, &xmax, &ymin, &ymax))
            {
               PanReference(((CCEtoODBView*)activeView), m_ref);
               ErrorMessage("Component has no extents", "Panning");
            }
            else
            {
               // switched all override colors to use a session base highlight color on data's
               probeData->setHighlightColor(highlightColors[0]);

               ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
               ZoomPanTo(xmin, xmax, ymin, ymax, probeData->getInsert());
            }
         }
      }
   }
#ifdef LOAD_ONLY_SELECTED_PROBE_IN_TREE
	// not used when tree is loaded with all probes
   else if (m_compMode == navigatorModeUnplacedProbes)
   {
      if (!m_tree.GetParentItem(selectedItem)) //if item is a net
      {
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct* net = (NetStruct*)netItemData->voidPtr;

         if (net)
         {
            // Make sure there is an extent before zooming
            double left, right, top, bottom;

            if (!NetExtents(m_doc, net->getNetName(), &left, &right, &bottom, &top))
            {
               ErrorMessage("Net has no extents", "Panning to Net");
            }
            else
            {
               ApplyMargin(m_curZoomMargin, &left, &right, &bottom, &top);
               ZoomPanTo(left, right, bottom, top);
            }
         }
      }
      else if (m_tree.GetParentItem(selectedItem) && (nImage == ICON_CP || nImage == ICON_VIA)) //if selected is a pin or via
      {
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct *net = (NetStruct*)netItemData->voidPtr;

         if (!net)
            return;

         if (nImage == ICON_VIA) // vias and bond pads are treated the same
         {
            TreeItemData *viaItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            DataStruct* via = (DataStruct*)viaItemData->voidPtr;

            if (via != NULL && (via->isInsertType(insertTypeVia) || via->isInsertType(insertTypeBondPad)))
            {
               double left, right, top, bottom;
               GetZoomRegion(via->getInsert(), &left, &right, &bottom, &top);

               ZoomPanTo(left, right, bottom, top, &via->getInsert()->getOrigin2d(), via->getInsert()->getGraphicMirrored(), true /*showcross*/, true /*cancel flip*/);
            }
         }
         else
         {
            // CompPin

            TreeItemData *cpItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            CompPinStruct* compPin = (CompPinStruct*)cpItemData->voidPtr;

            if (compPin != NULL)
            {
               double left, right, top, bottom;
               GetZoomRegion(compPin, &left, &right, &bottom, &top);

               ZoomPanTo(left, right, bottom, top, &compPin->getOrigin(), compPin->getMirror()?true:false, true /*showcross*/);
            }
         }
      }
   }
#endif
   else if (m_compMode == navigatorModeNetsWithAccess)
   {
      if (!m_tree.GetParentItem(selectedItem)) //if item is a net
      {
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct* net = (NetStruct*)netItemData->voidPtr;

         if (net)
         {
            // Make sure there is an extent before zooming
            double left, right, top, bottom;

            if (!NetExtents(m_doc, net->getNetName(), &left, &right, &bottom, &top))
            {
               ErrorMessage("Net has no extents", "Panning to Net");
            }
            else
            {
               ApplyMargin(m_curZoomMargin, &left, &right, &bottom, &top);
               ZoomPanTo(left, right, bottom, top);
            }
         }
      }
      else if (m_tree.GetParentItem(selectedItem) && (nImage == ICON_CP || nImage == ICON_VIA)) //if selected is a pin or via
      {
         TreeItemData *netItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
         NetStruct *net = (NetStruct*)netItemData->voidPtr;

         if (!net)
            return;

         if (nImage == ICON_VIA) // vias and bond pads are treated the same
         {
            TreeItemData *viaItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            DataStruct* via = (DataStruct*)viaItemData->voidPtr;

            if (via != NULL && (via->isInsertType(insertTypeVia) || via->isInsertType(insertTypeBondPad)))
            {
               double left, right, top, bottom;
               GetZoomRegion(via->getInsert(), &left, &right, &bottom, &top);

               ZoomPanTo(left, right, bottom, top, &via->getInsert()->getOrigin2d(), via->getInsert()->getGraphicMirrored(), true /*showcross*/, true /*cancel flip*/);
            }
         }
         else
         {
            // CompPin
            TreeItemData *cpItemData = (TreeItemData*)m_tree.GetItemData(selectedItem);
            CompPinStruct* compPin = (CompPinStruct*)cpItemData->voidPtr;

            if (compPin != NULL)
            {
               double left, right, top, bottom;
               GetZoomRegion(compPin, &left, &right, &bottom, &top);

               ZoomPanTo(left, right, bottom, top, &compPin->getOrigin(), compPin->getMirror()?true:false, true /*showcross*/);
            }
         }
      }
      else if (nImage == ICON_ACCESS)
      {     
			if (activeView != NULL && data != NULL && data->isInsertType(insertTypeTestAccessPoint))
         {
            if (isWindow())
               UpdateData();

            CString m_ref = data->getInsert()->getRefname();

            double xmin, xmax, ymin, ymax;
            if (ComponentsExtents(m_doc, m_ref, &xmin, &xmax, &ymin, &ymax, FALSE))
            {
               ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
               ZoomPanTo(xmin, xmax, ymin, ymax, data->getInsert());
            }
            else
            {
               PanReference(((CCEtoODBView*)activeView), m_ref);
               ErrorMessage("Component has no extents", "Panning");
            }
         }
      }        
   }

   *pResult = 0;
}

DataStruct *CAMCADNavigator::FindComponentData(CString refname)
{
   if (m_curFile != NULL)
   {
      BlockStruct *pcbBlock = m_curFile->getBlock();

	   for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
	   {
		   DataStruct* data = insertIterator.getNext();
         if (data->isInsertType(insertTypePcbComponent) || data->isInsertType(insertTypeTestPoint) || data->isInsertType(insertTypeDie))
         {
            if (data->getInsert()->getRefname().CompareNoCase(refname) == 0)
               return data;
         }
      }
   }

	return NULL;
}

DataStruct *CAMCADNavigator::FindComponentPinData(DataStruct *pcbComp, CString pinName)
{
   if (pcbComp != NULL && pcbComp->getInsert() != NULL)
   {
      int insertedBlockNum = pcbComp->getInsert()->getBlockNumber();
      BlockStruct *compBlock = m_doc->getBlockAt(insertedBlockNum);

      CInsertTypeMask pinInsertTypes(insertTypePin, insertTypeDiePin);

	   for (CDataListIterator insertIterator(*compBlock, pinInsertTypes);insertIterator.hasNext();)
	   {
		   DataStruct* data = insertIterator.getNext();
         if (data->isInsertType(insertTypePin) || data->isInsertType(insertTypeDiePin)) //this should not be needed
         {
            if (data->getInsert()->getRefname().CompareNoCase(pinName) == 0)
               return data;
         }
      }
   }

   return NULL;
}


void CAMCADNavigator::GetDefaultZoomRegion(double x, double y, double *xmin, double *xmax, double *ymin, double *ymax)
{
   double standardMargin = 0.2*Units_Factor(UNIT_INCHES, m_doc->getSettings().getPageUnits());

   *xmin = x - standardMargin;
   *xmax = x + standardMargin;

   *ymin = y - standardMargin;
   *ymax = y + standardMargin;
}


void CAMCADNavigator::GetZoomRegion(InsertStruct *insert, double *xmin, double *xmax, double *ymin, double *ymax)
{
   if (insert!= NULL)
   {
      GetZoomRegion(insert->getOriginX(), insert->getOriginY(), insert, xmin, xmax, ymin, ymax);
   }
}

void CAMCADNavigator::GetZoomRegion(CompPinStruct *comppin, double *xmin, double *xmax, double *ymin, double *ymax)
{
   if (comppin!= NULL)
   {
      DataStruct *component = FindComponentData(comppin->getRefDes());
      DataStruct *pin = FindComponentPinData(component, comppin->getPinName());
      InsertStruct *pinInsert = (pin != NULL) ? pin->getInsert() : NULL;

      GetZoomRegion(comppin->getOriginX(), comppin->getOriginY(), pinInsert, xmin, xmax, ymin, ymax);
   }
}

void CAMCADNavigator::GetZoomRegion(double x, double y, InsertStruct *insert, double *xmin, double *xmax, double *ymin, double *ymax)
{
   GetDefaultZoomRegion(x, y, xmin, xmax, ymin, ymax);

   if (insert != NULL)
   {
      int insertedBlockNum = insert->getBlockNumber();
      BlockStruct *insertedBlock = m_doc->getBlockAt(insertedBlockNum);
      if (insertedBlock != NULL)
      {
         CExtent blockExt = insertedBlock->getExtent(getCamCadData());
         if (blockExt.isValid())
         {
            double maxSize = blockExt.getMaxSize();

            *xmin = x - maxSize / 2.0;
            *xmax = x + maxSize / 2.0;
            
            *ymin = y - maxSize / 2.0;
            *ymax = y + maxSize / 2.0;
         }
      }
   }

   ApplyMargin(m_curZoomMargin, xmin, xmax, ymin, ymax);
}

/**********************************************************************
* CAMCADNavigator::SelectBoard
*/
void CAMCADNavigator::SelectBoard(DataStruct* data)
{
   if (data && data->getInsert())
   {
      FileStruct * subFile = m_doc->getFileList().FindByBlockNumber(data->getInsert()->getBlockNumber());
      if(!subFile) return;

      int fileMirror = ((m_doc->getBottomView() ? MIRROR_FLIP : 0) ^ (subFile->isMirrored()) ? MIRROR_FLIP : 0);      
      SelectStruct* selectStruct = m_doc->InsertInSelectList(data, subFile->getBlock()->getFileNumber(),
			   subFile->getInsertX(), subFile->getInsertY(),  subFile->getScale(), 
			   subFile->getRotation(), fileMirror, &(subFile->getBlock()->getDataList()), FALSE, TRUE);
      
      m_doc->DrawEntity(*selectStruct,drawStateHighlighted);
   }
}

/**********************************************************************
* CCEtoODBDoc::NavInstSelectionChanged
*/
void CCEtoODBDoc::NavInstSelectionChanged(DataStruct *data)
{
   //no reaction from the board to the navigator - commented
   /*if (!data || !frame->getNavigator())
      return;
   frame->getNavigator().SelectComponent(data);*/
}

/**********************************************************************
* CCEtoODBDoc::NavNetSelectionChanged
*/
void CCEtoODBDoc::NavNetSelectionChanged(NetStruct *net)
{
   //no reaction from the board to the navigator - commented
   /*if (!net || !frame->getNavigator())
      return;
   frame->getNavigator().SelectNet(net);*/
}

/**********************************************************************
* CAMCADNavigator::SelectComponent
*/
void CAMCADNavigator::SelectComponent(DataStruct* data)
{
   if (!data)
      return;

   m_compMode = navigatorModeComponents;

   if (isWindow())
      UpdateData(FALSE);

   FillCompList(m_curFile);
   //m_tree.SelectItem(NULL);
   //m_tree.SelectDropTarget(NULL);
   UnHighLightNets();
   
   int nIndex = 0;

   while (nIndex < m_list.GetCount())
   {
      if ((DataStruct*)m_list.GetItemData(nIndex) == data)
      {
         m_list.SetSel(nIndex, TRUE);
         OnLbnSelchangeList();
         return;
      }

      nIndex++;
   }
}

/**********************************************************************
* CAMCADNavigator::SelectNet
*/
void CAMCADNavigator::SelectNet(NetStruct* net)
{
   if (!net)
      return;

   if (isWindow())
      UpdateData();

   m_compMode = navigatorModeNets;

   if (isWindow())
      UpdateData(FALSE);

   FillNetList();
   //m_tree.SelectItem(NULL);
   //m_tree.SelectDropTarget(NULL);
   UnHighLightNets();
   
   //HTREEITEM root = m_tree.GetRootItem();
   int nIndex = 0;

   while (nIndex < m_list.GetCount())
   {
      if ((NetStruct*)m_list.GetItemData(nIndex) == net)
      {
         m_list.SetSel(nIndex, TRUE);
         OnLbnSelchangeList();

         return;
      }

      nIndex++;
   }
}

/******************************************************************************
* CAMCADNavigator::OnBnClickedNetMode
*/
void CAMCADNavigator::OnBnClickedNetMode()
{
   if (docNotPCB)
      return;

   if (!m_doc)
      return;

   m_list.SetSel(-1);

   clearHighlights();

   GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(TRUE);

   m_doc->UnselectAll(FALSE);
   UnHighLightNets();
   m_list.ResetContent();
   m_compMode = navigatorModeNets;
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);

   FillNetList();
}

/******************************************************************************
   OnBnClickedRadioBoardmode
*/
void CAMCADNavigator::OnBnClickedRadioBoardmode()
{
   if (docNotPCB)
      return;

   if (m_doc != NULL)
   {
      m_list.SetSel(-1);

      clearHighlights();

      GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(FALSE);

      m_doc->UnselectAll(FALSE);
      UnHighLightNets();
      m_list.ResetContent();
      m_compMode = navigatorModeBoards;
      m_tree.SelectItem(NULL);
      m_tree.SelectDropTarget(NULL);

      FillBoardList(m_curFile);

      UpdateCountsDisplay();

      m_doc->OnFitPageToImage();
   }
}

/******************************************************************************
*/
void CAMCADNavigator::OnBnClickedCompMode()
{
   if (docNotPCB)
      return;

   if (m_doc != NULL)
   {
      m_list.SetSel(-1);

      clearHighlights();

      GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(FALSE);

      m_doc->UnselectAll(FALSE);
      UnHighLightNets();
      m_list.ResetContent();
      m_compMode = navigatorModeComponents;
      m_tree.SelectItem(NULL);
      m_tree.SelectDropTarget(NULL);

      FillCompList(m_curFile);

      UpdateCountsDisplay();
   }
}

/******************************************************************************
*/
void CAMCADNavigator::OnBnClickedManufactMode()
{
   if (docNotPCB)
      return;

   if (m_doc != NULL)
   {
      m_list.SetSel(-1);

      clearHighlights();

      GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(FALSE);

      m_doc->UnselectAll(FALSE);
      UnHighLightNets();
      m_list.ResetContent();
      m_compMode = navigatorModeManufacturing;
      m_tree.SelectItem(NULL);
      m_tree.SelectDropTarget(NULL);

      FillManufactList(m_curFile);

      UpdateCountsDisplay();
   }
}

/******************************************************************************
* CAMCADNavigator::UnHighLightNets
*/
void CAMCADNavigator::UnHighLightNets()
{
   m_doc->HighlightedNetsMap.RemoveAll();   

   for (POSITION filePos = m_doc->getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = m_doc->getFileList().GetNext(filePos);

      if (file == NULL)
         continue;      

      for (POSITION netPos = file->getNetList().GetHeadPosition();netPos != NULL;)
      {
         file->getNetList().GetNext(netPos)->setHighlighted(false);
      }
   }

   m_doc->UpdateAllViews(NULL);
}
 /******************************************************************************
*/  
void CAMCADNavigator::ZoomPanTo(double ZmLeftX, double ZmRightX, double ZmBotY, double ZmTopY, CPoint2d *PanPt, bool Mirrored, bool ShowCross, bool CancelFlip)
{
   // The Zm..X and Zm..Y are non-standard extents. This requires different handling than
   // the other two ZoomPanTo() functions, which are expecting standard extents. We have to
   // apply the top/bottom/mirror of extents here.

   double left = ZmLeftX;
   double right = ZmRightX;

   FlippingTag flip = flipToTop;
   if (Mirrored)
      flip = flipToBottom;

   if (GetFlip() && !CancelFlip)
   {
      if (flip == flipToTop)
         m_doc->OnTopview();
      else if (flip == flipToBottom)
         m_doc->OnBottomview();
   }

   double panX = PanPt->x;
   double panY = PanPt->y;

   if (m_curFile->getResultantMirror(m_doc->getBottomView()))
   {
      double temp = left;
      left = -right;
      right = -temp;
      panX = -panX;
   }         

   ZoomPanTo(left, right, ZmBotY, ZmTopY, panX, panY, flipNot /*already flipped*/, ShowCross);
}

/******************************************************************************
*/
void CAMCADNavigator::ZoomPanTo(double ZmLeftX, double ZmRightX, double ZmBotY, double ZmTopY, InsertStruct *PnInsert, bool ShowCross)
{
   double pnX = (ZmLeftX + ZmRightX) / 2.0;
   double pnY = (ZmBotY + ZmTopY) / 2.0;
   FlippingTag flip = flipNot;

   if (PnInsert != NULL)
   {
      pnX = PnInsert->getOriginX();
      pnY = PnInsert->getOriginY();

      if (PnInsert->getPlacedTop())
         flip = flipToTop;
      else
      {
         flip = flipToBottom;
         pnX = -pnX;
      }
   }

   ZoomPanTo(ZmLeftX, ZmRightX, ZmBotY, ZmTopY, pnX, pnY, flip, ShowCross);

}

/******************************************************************************
*/
void CAMCADNavigator::ZoomPanTo(double ZmLeftX, double ZmRightX, double ZmBotY, double ZmTopY, double PnX, double PnY, FlippingTag flipToSide, bool ShowCross)
{
   if (!GetPanning())
   {
      if (false)//GetFlip())
      {
         if (flipToSide == flipToTop)
            m_doc->OnTopview();
         else if (flipToSide == flipToBottom)
            m_doc->OnBottomview();
      }

      ((CCEtoODBView*)activeView)->ZoomBox(ZmLeftX, ZmRightX, ZmBotY, ZmTopY);

      // Flip after zoom, otherwise zoom goes to wrong place.
      if (GetFlip())
      {
         if (flipToSide == flipToTop)
            m_doc->OnTopview();
         else if (flipToSide == flipToBottom)
            m_doc->OnBottomview();
      }

      if (ShowCross)
         ((CCEtoODBView*)activeView)->showCross(PnX, PnY);
   }
   else
   {
      // Flip before pan, otherwise we get excessive flicker.
      // Pan seems to go to correct place this way, unlike zoom.
      if (GetFlip())
      {
         if (flipToSide == flipToTop)
            m_doc->OnTopview();
         else if (flipToSide == flipToBottom)
            m_doc->OnBottomview();
      }

      // ShowCross arg is ignored for Pan, always use ShowCross == true here
      ((CCEtoODBView*)activeView)->PanCoordinate(PnX, PnY, TRUE);

   }
}

HTREEITEM CAMCADNavigator::FindTreeSiblingItem(HTREEITEM root, CString itemName)
{
   HTREEITEM treeitem = root;

   while (treeitem != NULL)
   {
      if (!m_tree.GetItemText(treeitem).CompareNoCase(itemName))
      {
         return treeitem;
      }

      treeitem = m_tree.GetNextSiblingItem(treeitem);
   }

   return NULL;
}

HTREEITEM CAMCADNavigator::FindTreeSiblingItem(HTREEITEM root, void *itemData)
{
   HTREEITEM treeitem = root;

   while (treeitem != NULL)
   {
      TreeItemData *rawTreeData = (TreeItemData*)m_tree.GetItemData(treeitem);

      if (rawTreeData->voidPtr == itemData)
      {
         return treeitem;
      }

      treeitem = m_tree.GetNextSiblingItem(treeitem);
   }

   return NULL;
}

/******************************************************************************
* CAMCADNavigator::OnLbnSelchangeList
*/
void CAMCADNavigator::OnLbnSelchangeList()
{
   if (docNotPCB)
      return;

   if (!m_doc)
      return;

   // Sometimes big nets with lots of probes take long enough to fill the tree
   // control to be alarming, so put up that reassuring hourglass.
   CWaitCursor busybee;

   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();
   int nCount = m_list.GetSelCount();
   CArray<int,int> aryListBoxSel;

   aryListBoxSel.SetSize(nCount);
   m_list.GetSelItems(nCount, aryListBoxSel.GetData());

   clearHighlights();

   m_components.empty();
   m_placedProbes.empty();
   m_unplacedProbes.empty();
   m_nets.RemoveAll();

   if (m_compMode == navigatorModeComponents ||
      m_compMode == navigatorModeManufacturing)
   {
      for (int i=0; i< nCount; i++)
      {
         int nIndex = aryListBoxSel.GetAt(i);
         DataStruct* data = (DataStruct*)m_list.GetItemData(nIndex);

			if (data == NULL)
            return;

         int icon = ICON_COMP;
         if (data->isInsertType(insertTypePcbComponent))
            icon = ICON_COMP;
         else if (data->isInsertType(insertTypeDie))
            icon = ICON_DIE;
         else if (data->isInsertType(insertTypeTestPoint))
            icon = ICON_TESTPOINT;
         else if (data->isInsertType(insertTypeFiducial))
            icon = ICON_FID;
         else if (data->isInsertType(insertTypeDrillTool))
            icon = ICON_TOOLING;
         else
            return;

         m_components.AddTail(data);

         CString refName = data->getInsert()->getRefname();
         HTREEITEM root = m_tree.GetRootItem();

#ifdef WASTE_TIME_LOOKING_FOR_SOMETHING_THAT_AINT_THERE
         // We just emptied the tree before doing this add stuff.
         // So if there is already something in the tree with same name then we truly have
         // duplicates. This delete loop usually finds nothing in normal situations, so it
         // is just a complete scan of the tree for every item we want to add.
         // A waste of time, and these tree building operations are very time sensitive because
         // the CTreeCtrl is itself pathetically slow.

         while (root)
         {
            if (!m_tree.GetItemText(root).CompareNoCase(refName))
            {
               m_tree.DeleteItem(root);

               break;
            }

            root = m_tree.GetNextSiblingItem(root);
         }
#endif

         //HTREEITEM compItem = m_tree.InsertItem(refName, icon, icon, TVI_ROOT, TVI_SORT);
         // Just add as fast as you can, it sorts later anyway
         HTREEITEM compItem = m_tree.InsertItem(refName, icon, icon, TVI_ROOT, TVI_FIRST);
         SetTreeItemData(&m_tree, compItem, refName, icon, data);

         BlockStruct *geom = m_doc->getBlockAt(data->getInsert()->getBlockNumber());

         if (!geom)
            return;

#ifdef BLOCK_TYPE_MATTERS  // no, it really doesn't.
         // Not convinced that this check is worth while, but it was here
         if (geom->getBlockType() != BLOCKTYPE_PCBCOMPONENT &&
            geom->getBlockType() != BLOCKTYPE_FIDUCIAL &&
            geom->getBlockType() != BLOCKTYPE_TOOLING &&
            geom->getBlockType() != BLOCKTYPE_TESTPOINT)
            return;
#endif

         // Add pins only for PCB Comps, Test Points, and Die parts. 
         // Pins look silly in tree on Fids and tool holes.
         if (data->isInsertType(insertTypePcbComponent) ||
            data->isInsertType(insertTypeTestPoint) ||
            data->isInsertType(insertTypeDie))
         {
            POSITION pinPos = geom->getDataList().GetHeadPosition();

            while (pinPos)
            {
               DataStruct *pin = geom->getDataList().GetNext(pinPos);

               if (pin != NULL && (pin->isInsertType(insertTypePin) || pin->isInsertType(insertTypeDiePin)))
               {
                  CString pinItemText;
                  CString key = data->getInsert()->getRefname()+"."+pin->getInsert()->getRefname();
                  pinItemText.Format("%s (%s)",pin->getInsert()->getRefname(), GetNetNameByCompPin(key));
                  //HTREEITEM pinItem = m_tree.InsertItem(pinItemText, ICON_PIN, ICON_PIN, compItem, TVI_SORT);
                  // Just add as fast as you can, it sorts later anyway
                  HTREEITEM pinItem = m_tree.InsertItem(pinItemText, ICON_PIN, ICON_PIN, compItem, TVI_FIRST);

                  SetTreeItemData(&m_tree, pinItem, pinItemText, ICON_PIN, pin);
               }
            }
         }

         TVSORTCB pSort;
         pSort.hParent = compItem;
         pSort.lParam = 0;
         pSort.lpfnCompare = TreeSortCompareFunc;
         m_tree.SortChildrenCB(&pSort);
      }

      //Set the sort of the components
      TVSORTCB pSort;
      pSort.hParent = TVI_ROOT;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_tree.SortChildrenCB(&pSort);
      
      //select selected components
      double xmin, xmax, ymin, ymax;
      double bottom, top, left, right;
      top    = right = ymax = xmax = -100000;
      bottom = left  = ymin = xmin =  100000;
      DataStruct* data;
      int componentIndex = 0;

      for (POSITION componentPos = m_components.GetHeadPosition();componentPos != NULL;componentIndex++)
      {
         data = m_components.GetNext(componentPos);

         if (!data)
            return;

         if (!activeView)
            return;

         if (isWindow())
            UpdateData();
         
         CString m_ref = data->getInsert()->getRefname();

         if (m_ref.IsEmpty())
         {
            // ComponentsExtents will fail if ref is blank, so just snag insert location
            double insX = data->getInsert()->getOriginX();
            double insY = data->getInsert()->getOriginY();
            if (m_curFile->getResultantMirror(m_doc->getBottomView()) != data->getInsert()->getGraphicMirrored())
               insX = -insX;
            if (insX < xmin) xmin = insX;
            if (insX > xmax) xmax = insX;
            if (insY < ymin) ymin = insY;
            if (insY > ymax) ymax = insY;
         }
         else if (!ComponentsExtents(m_doc, m_ref, &xmin, &xmax, &ymin, &ymax, FALSE))
         {
            PanReference(((CCEtoODBView*)activeView), m_ref);
            ErrorMessage("Can not zoom/pan to item, it has no extents", "Panning");
            return;
         }

         data->setHighlightColor(highlightColors[componentIndex % HighlightColorCount]);
         //data->setOverrideColor(highlightColors[i%5]);
         //data->setColorOverride(true);

         ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
         left = min(left, xmin);
         right = max(right, xmax);
         bottom = min(bottom, ymin);
         top = max(top, ymax);
      }

      if (nCount)
      {
         ZoomPanTo(left, right, bottom, top, data->getInsert());
      }
   
   }
   else if (m_compMode == navigatorModeNets || m_compMode == navigatorModeNetsWithoutAccess)
   {
      m_doc->UnselectAll(FALSE);

      UnHighLightNets();

      for (int j=0; j< nCount; j++)
      {
         int nIndex = aryListBoxSel.GetAt(j);
         NetStruct* net = (NetStruct*)m_list.GetItemData(nIndex);

         if (net == NULL)
            return;

			LoadProbeTree(net);
      }     

      TVSORTCB pSort;
      pSort.hParent = TVI_ROOT;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_tree.SortChildrenCB(&pSort);

      double xmin, xmax, ymin, ymax;
      double bottom, top, left, right;
      top    = right = -100000;
      bottom = left  =  100000;
      int netIndex = 0;

      //for (int i=0; i<navNetArrayCount; i++)
      for (POSITION netPos = m_nets.GetHeadPosition();netPos != NULL;netIndex++)
      {
         //NetStruct* net = navNetArray.GetAt(i);
         NetStruct* net = m_nets.GetNext(netPos);

         if (net)
         {
            // Make sure there is an extent before zooming
            if (!NetExtents(m_doc, net->getNetName(), &xmin, &xmax, &ymin, &ymax))
            {
               ErrorMessage("Net has no extents", "Panning to Net");
               return;
            }

            CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
            highlightNet(*net,highlightColors[netIndex % HighlightColorCount]);

            ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
            left = min(left, xmin);
            right = max(right, xmax);
            bottom = min(bottom, ymin);
            top = max(top, ymax);
         }

         ZoomPanTo(left, right, bottom, top);

      }  

      Show_selected_only();
   }
   else if (m_compMode == navigatorModeNetsWithAccess)
   {
      m_doc->UnselectAll(FALSE);
      UnHighLightNets();

      for (int j = 0; j < nCount; j++)
      {
         int nIndex = aryListBoxSel.GetAt(j);
         NetStruct* net = (NetStruct*)m_list.GetItemData(nIndex);

         if (!net)
            return;

         // Collect access markers on this net

         CDataList accessDataList(false);

         for (POSITION probePos = m_curFile->getBlock()->getDataList().GetHeadPosition();probePos != NULL;)
         {
            DataStruct *data = m_curFile->getBlock()->getDataList().GetNext(probePos);

            if (data != NULL && data->isInsertType(insertTypeTestAccessPoint))
            {
               Attrib *netAttrib = is_attvalue(m_doc, data->getAttributeMap(), ATT_NETNAME, 0);

               if (netAttrib != NULL)
               {
                  if (net->getNetName().CompareNoCase(netAttrib->getStringValue()) == 0)
                  {
                     accessDataList.AddTail(data);
                  }
               }
            }
         }

         LoadBasicNetTree(net);
         HTREEITEM root = m_tree.GetRootItem();
         HTREEITEM netItem = this->FindTreeSiblingItem(root, net->getNetName());

         if (netItem != NULL)
         {
            for (POSITION accessDataPos = accessDataList.GetHeadPosition();accessDataPos != NULL;)
            {
               DataStruct* accessData = accessDataList.GetNext(accessDataPos);
               DataStruct* viaAccess = NULL;
               CompPinStruct* cpAccess = NULL;

               Attrib *attrib = is_attvalue(m_doc, accessData->getAttributesRef(), ATT_DDLINK, 1);

               if (attrib)
               {                    
                  long entityNum = (long)attrib->getIntValue();
                  CEntity accessEntity = CEntity::findEntity(getCamCadData(), entityNum);

                  if (accessEntity.getEntityType() == entityTypeData)
                  {
                     viaAccess = accessEntity.getData();
                  }
                  else if (accessEntity.getEntityType() == entityTypeCompPin)
                  {
                     cpAccess = accessEntity.getCompPin();
                  }
               }

               if (cpAccess != NULL)
               {
                  // Make sure this stays in sync with how we make the name when putting item in the tree
                  CString pinRef = cpAccess->getRefDes() + "." + cpAccess->getPinName();
                  HTREEITEM netrootChild = m_tree.GetChildItem(netItem);
                  HTREEITEM pinItem = FindTreeSiblingItem(netrootChild, pinRef);

                  if (pinItem != NULL)
                  {
#ifdef WASTE_TIME_LOOKING_FOR_SOMETHING_THAT_AINT_THERE
                     // We just emptied the tree before doing this add stuff.
                     // So if there is already something in the tree with same name then we truly have
                     // duplicates. This delete loop usually finds nothing in normal situations, so it
                     // is just a complete scan of the tree for every item we want to add.
                     // A waste of time, and these tree building operations are very time sensitive because
                     // the CTreeCtrl is itself pathetically slow.

                     HTREEITEM killroot = m_tree.GetChildItem(pinItem);

                     while (killroot)
                     {
                        if (!m_tree.GetItemText(killroot).CompareNoCase(accessData->getInsert()->getRefname()))
                        {
                           m_tree.DeleteItem(killroot);
                           break;
                        }

                        killroot = m_tree.GetNextSiblingItem(killroot);
                     }
#endif
                     //put the access under this pin
                     HTREEITEM accessItem = m_tree.InsertItem(accessData->getInsert()->getRefname(), ICON_ACCESS, ICON_ACCESS, pinItem, TVI_SORT);
                     SetTreeItemData(&m_tree, accessItem, accessData->getInsert()->getRefname(), ICON_ACCESS, accessData);
                  }
               }

               if (viaAccess != NULL)
               {
                  HTREEITEM netrootChild = m_tree.GetChildItem(netItem);
                  HTREEITEM viaItem = FindTreeSiblingItem(netrootChild, viaAccess);

                  if (viaItem != NULL)
                  {
#ifdef WASTE_TIME_LOOKING_FOR_SOMETHING_THAT_AINT_THERE
                     // We just emptied the tree before doing this add stuff.
                     // So if there is already something in the tree with same name then we truly have
                     // duplicates. This delete loop usually finds nothing in normal situations, so it
                     // is just a complete scan of the tree for every item we want to add.
                     // A waste of time, and these tree building operations are very time sensitive because
                     // the CTreeCtrl is itself pathetically slow.

                     HTREEITEM killroot = m_tree.GetChildItem(viaItem);

                     while (killroot)
                     {
                        if (!m_tree.GetItemText(killroot).CompareNoCase(accessData->getInsert()->getRefname()))
                        {
                           m_tree.DeleteItem(killroot);
                           break;
                        }

                        killroot = m_tree.GetNextSiblingItem(killroot);
                     }
#endif
                     //put the access under this via
                     HTREEITEM accessItem = m_tree.InsertItem(accessData->getInsert()->getRefname(), ICON_ACCESS, ICON_ACCESS, viaItem, TVI_SORT);
                     SetTreeItemData(&m_tree, accessItem, accessData->getInsert()->getRefname(), ICON_ACCESS, accessData);
                  }
               }
            }

         }

         m_nets.AddTail(net);
      }     

      TVSORTCB pSort;
      pSort.hParent = TVI_ROOT;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_tree.SortChildrenCB(&pSort);

      double xmin, xmax, ymin, ymax;
      double bottom, top, left, right;
      top    = right = -100000;
      bottom = left  =  100000;
      int netIndex = 0;

      //for (int i=0; i<navNetArrayCount; i++)
      for (POSITION netPos = m_nets.GetHeadPosition();netPos != NULL;netIndex++)
      {
         //NetStruct* net = navNetArray.GetAt(i);
         NetStruct* net = m_nets.GetNext(netPos);

         if (net)
         {
            // Make sure there is an extent before zooming
            if (!NetExtents(m_doc, net->getNetName(), &xmin, &xmax, &ymin, &ymax))
            {
               ErrorMessage("Net has no extents", "Panning to Net");
               return;
            }

            CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
            highlightNet(*net,highlightColors[netIndex % HighlightColorCount]);

            ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
            left = min(left, xmin);
            right = max(right, xmax);
            bottom = min(bottom, ymin);
            top = max(top, ymax);
         }

         ZoomPanTo(left, right, bottom, top);
      }  

      Show_selected_only();
   }
   else if (m_compMode == navigatorModePlacedProbes)
   {
      UnHighLightNets();

      for (int i=0; i< nCount; i++)
      {
         int nIndex = aryListBoxSel.GetAt(i);
         DataStruct* data = (DataStruct*)m_list.GetItemData(nIndex);

			if (data == NULL || !data->isInsertType(insertTypeTestProbe))
            return;

         m_placedProbes.AddTail(data);
         
         Attrib *attrib = is_attvalue(m_doc, data->getAttributesRef(), ATT_NETNAME, 1);

         if (!attrib)
            continue;

         CString netName = attrib->getStringValue();

         NetStruct* net = FindNet(m_curFile, netName);

         if (!net)
            continue;

#ifndef LOAD_ONLY_SELECTED_PROBE_IN_TREE
			// Case 1686, load all probes, including placed and unplaced
			LoadProbeTree(net);
#else
			// what it did before, load only selected probe in tree
         m_nets.AddTail(net);
         HTREEITEM root = m_tree.GetRootItem();

         while (root)
         {
            if (!m_tree.GetItemText(root).CompareNoCase(net->getNetName()))
            {
               m_tree.DeleteItem(root);

               break;
            }

            root = m_tree.GetNextSiblingItem(root);
         }

         HTREEITEM netItem = m_tree.InsertItem(net->getNetName(), ICON_NET, ICON_NET, TVI_ROOT, TVI_SORT);
         //m_tree.SetItemData(netItem, (DWORD)net);
         SetTreeItemData(&m_tree, netItem, net->getNetName(), ICON_NET, net);

         //get the access point this probe is on
         attrib = NULL;
         attrib = is_attvalue(m_doc, data->getAttributesRef(), ATT_DDLINK, 1);

         if (!attrib)
            continue;

         long entityNum = (long)attrib->getIntValue();

         DataStruct* viaAccess = NULL;
         CompPinStruct* cpAccess = NULL;
         //EEntityType type;
         //void* voidPtr = FindEntity(m_doc, entityNum, type);
         CEntity entity = CEntity::findEntity(*doc,entityNum);

         if (entity.getEntityType() == entityTypeUndefined)
            continue;

         attrib = NULL;
         attrib = is_attvalue(m_doc,*(entity.getAttributesRef()), ATT_DDLINK, 1);

         if (!attrib)
            continue;

         entityNum = (long)attrib->getIntValue();

         //voidPtr = NULL;
         //voidPtr = FindEntity(m_doc, entityNum, type);
         CEntity accessEntity = CEntity::findEntity(*doc,entityNum);

         if (accessEntity.getEntityType() == entityTypeData)
         {
            viaAccess = accessEntity.getData();
         }
         else if (accessEntity.getEntityType() == entityTypeCompPin)
         {
            cpAccess = accessEntity.getCompPin();
         }
         
         POSITION compPinPos = net->getHeadCompPinPosition();

         while (compPinPos)
         {
            CompPinStruct* compPin = net->getNextCompPin(compPinPos);

            if (!compPin)
               continue;

            HTREEITEM pinItem = m_tree.InsertItem(compPin->getRefDes()+"."+compPin->getPinName(), ICON_CP, ICON_CP, netItem, TVI_SORT);
            //m_tree.SetItemData(pinItem, (DWORD)compPin);
            SetTreeItemData(&m_tree, pinItem, compPin->getRefDes()+"."+compPin->getPinName(), ICON_CP, compPin);

            if (cpAccess && cpAccess == compPin)
            {
               //put the probe under this pin
               HTREEITEM probeItem = m_tree.InsertItem(data->getInsert()->getRefname(), ICON_PLCD, ICON_PLCD, pinItem, TVI_SORT);
               //m_tree.SetItemData(probeItem, (DWORD)data);
               SetTreeItemData(&m_tree, probeItem, data->getInsert()->getRefname(), ICON_PLCD, data);
            }
         }

         TVSORTCB pSort;
         pSort.hParent = netItem;
         pSort.lParam = 0;
         pSort.lpfnCompare = TreeSortCompareFunc;
         m_tree.SortChildrenCB(&pSort);

         if (m_curFile)
         {
            Attrib *attrib = NULL;
            CString viaName = "";
            POSITION viaPos = curFile->getBlock()->getDataList().GetHeadPosition();

            while (viaPos)
            {
               DataStruct *via = curFile->getBlock()->getDataList().GetNext(viaPos);

               if (via == NULL || !(via->isInsertType(insertTypeVia) || via->isInsertType(insertTypeBondPad)))
                  continue;

               viaName = via->getInsert()->getRefname();
               attrib = is_attvalue(m_doc, via->getAttributesRef(), ATT_NETNAME, 1);

               if (attrib && !attrib->getStringValue().CompareNoCase(net->getNetName()))
               {
                  HTREEITEM viaItem = m_tree.InsertItem(viaName.IsEmpty()?"VIA":viaName, ICON_VIA, ICON_VIA, netItem, TVI_SORT);
                  //m_tree.SetItemData(viaItem, (DWORD)via);
                  SetTreeItemData(&m_tree, viaItem, via->getInsert()->getRefname(), ICON_VIA, via);

                  if (viaAccess && viaAccess == via)
                  {
                     //put the probe under this pin
                     HTREEITEM probeItem = m_tree.InsertItem(data->getInsert()->getRefname(), ICON_PLCD, ICON_PLCD, viaItem, TVI_SORT);
                     //m_tree.SetItemData(probeItem, (DWORD)data);
                     SetTreeItemData(&m_tree, probeItem, data->getInsert()->getRefname(), ICON_PLCD, data);
                  }
               }
            }
         }
#endif
      }

      TVSORTCB pSort;
      pSort.hParent = TVI_ROOT;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_tree.SortChildrenCB(&pSort);

      double bottom, top, left, right;

      if (nCount && GetPlacedProbesZoomRegion(&left, &right, &top, &bottom))
      {

			// This strikes me as a little weird for the pan-to item, but it is
			// equivalent to what the code did when the "fill tree" was local, 
			// i.e. it was the last data item in the list from the loop.
         DataStruct *data = m_placedProbes.GetTail();
         InsertStruct *insert = data != NULL ? data->getInsert() : NULL;

         ZoomPanTo(left, right, bottom, top, insert);
      }
   }  
   else if (m_compMode == navigatorModeUnplacedProbes)
   {
      m_doc->UnselectAll(FALSE);
      UnHighLightNets();

      for (int i=0; i< nCount; i++)
      {
         int nIndex = aryListBoxSel.GetAt(i);
         DataStruct* data = (DataStruct*)m_list.GetItemData(nIndex);

			if (data == NULL || !data->isInsertType(insertTypeTestProbe))
            continue;

         //navUnplacedProbeArray.SetAtGrow(navUnplacedProbeArrayCount++, data);
         m_unplacedProbes.AddTail(data);
         Attrib *attrib = is_attvalue(m_doc, data->getAttributesRef(), ATT_NETNAME, 1);

         if (!attrib)
            continue;

         CString netName = attrib->getStringValue();

         NetStruct* net = FindNet(m_curFile, netName);

         if (!net)
            continue;

#ifndef LOAD_ONLY_SELECTED_PROBE_IN_TREE
			// Case 1686, load all probes, including placed and unplaced
			LoadProbeTree(net);
#else
			// what it did before, load only selected probe in tree
         HTREEITEM root = m_tree.GetRootItem();

         while (root)
         {
            if (!m_tree.GetItemText(root).CompareNoCase(net->getNetName()))
            {
               m_tree.DeleteItem(root);

               break;
            }

            root = m_tree.GetNextSiblingItem(root);
         }

         HTREEITEM netItem = m_tree.InsertItem(net->getNetName(), ICON_NET, ICON_NET, TVI_ROOT, TVI_SORT);
         //m_tree.SetItemData(netItem, (DWORD)net);
         SetTreeItemData(&m_tree, netItem, net->getNetName(), ICON_NET, net);

         HTREEITEM probeItem = m_tree.InsertItem(data->getInsert()->getRefname(), ICON_UNPLCD, ICON_UNPLCD, netItem, TVI_SORT);
         //m_tree.SetItemData(probeItem, (DWORD)data);
         SetTreeItemData(&m_tree, probeItem, data->getInsert()->getRefname(), ICON_UNPLCD, data);
         
         POSITION compPinPos = net->getHeadCompPinPosition();

         while (compPinPos)
         {
            CompPinStruct* compPin = net->getNextCompPin(compPinPos);

            if (!compPin)
               continue;

            HTREEITEM pinItem = m_tree.InsertItem(compPin->getRefDes()+"."+compPin->getPinName(), ICON_CP, ICON_CP, netItem, TVI_SORT);
            //m_tree.SetItemData(pinItem, (DWORD)compPin);
            SetTreeItemData(&m_tree, pinItem, compPin->getRefDes()+"."+compPin->getPinName(), ICON_CP, compPin);
         }

         TVSORTCB pSort;
         pSort.hParent = netItem;
         pSort.lParam = 0;
         pSort.lpfnCompare = TreeSortCompareFunc;
         m_tree.SortChildrenCB(&pSort);

         if (m_curFile)
         {
            Attrib *attrib = NULL;
            CString viaName = "";
            POSITION viaPos = curFile->getBlock()->getDataList().GetHeadPosition();

            while (viaPos)
            {
               DataStruct *via = curFile->getBlock()->getDataList().GetNext(viaPos);

               if (via == NULL || !(via->isInsertType(insertTypeVia) || via->isInsertType(insertTypeBondPad)))
                  continue;

               viaName = via->getInsert()->getRefname();
               attrib = is_attvalue(m_doc, via->getAttributesRef(), ATT_NETNAME, 1);

               if (attrib && !attrib->getStringValue().CompareNoCase(net->getNetName()))
               {
                  HTREEITEM viaItem = m_tree.InsertItem(viaName.IsEmpty()?"VIA":viaName, ICON_VIA, ICON_VIA, netItem, TVI_SORT);
                  //m_tree.SetItemData(viaItem, (DWORD)via);
                  SetTreeItemData(&m_tree, viaItem, via->getInsert()->getRefname(), ICON_VIA, via);
               }
            }
         }
#endif
      }
      
      TVSORTCB pSort;
      pSort.hParent = TVI_ROOT;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_tree.SortChildrenCB(&pSort);

      double bottom, top, left, right;
		if (GetUnplacedProbesZoomRegion(&left, &right, &top, &bottom))
		{
         ZoomPanTo(left, right, bottom, top);
		}


      Show_selected_only();
   }
   else if (m_compMode == navigatorModeBoards)
   {
      for(int selectIdx = 0; selectIdx < nCount; selectIdx++)
      {
         int nIndex = aryListBoxSel.GetAt(selectIdx);
         DataStruct* data = (DataStruct*)m_list.GetItemData(nIndex);
         SelectBoard(data);		   
      }
      return;
   }

   ExpandTree();
}

void CAMCADNavigator::ExpandTree()


{
  HTREEITEM root = m_tree.GetRootItem();
   while (root)
   {
      if (m_tree.ItemHasChildren(root))
      {
         m_tree.Expand(root, TVE_EXPAND);

         HTREEITEM child = m_tree.GetChildItem(root);
         while (child)
         {
            if (m_tree.ItemHasChildren(child))
            {
               m_tree.Expand(child, TVE_EXPAND);
            }

            child = m_tree.GetNextSiblingItem(child);
         }
      }

      root = m_tree.GetNextSiblingItem(root);
   }
}

//----------------------------------------------------------------------------
 
void CAMCADNavigator::LoadBasicNetTree(NetStruct *net)
{
   // Load the common things viewed for all net trees: the net, the compins, the vias.

   if (net != NULL)
   {
      // Add net to tree
      HTREEITEM root = m_tree.GetRootItem();
      HTREEITEM netItem = this->FindTreeSiblingItem(root, net->getNetName());

      if (netItem == NULL)
      {
         netItem = m_tree.InsertItem(net->getNetName(), ICON_NET, ICON_NET, TVI_ROOT, TVI_SORT);
         SetTreeItemData(&m_tree, netItem, net->getNetName(), ICON_NET, net);
      }







      // Add comppins to tree
      {
         POSITION compPinPos = net->getHeadCompPinPosition();





         while (compPinPos)
         {
            CompPinStruct* compPin = net->getNextCompPin(compPinPos);


            if (compPin != NULL)
            {
               CString pinRef = compPin->getRefDes() + "." + compPin->getPinName();
               HTREEITEM netrootChild = m_tree.GetChildItem(netItem);
               HTREEITEM pinItem = FindTreeSiblingItem(netrootChild, pinRef);

               if (pinItem == NULL)
               {
                  //pinItem = m_tree.InsertItem(pinRef, ICON_CP, ICON_CP, netItem, TVI_SORT);
                  // Just add fast, we sort later anyway
                  pinItem = m_tree.InsertItem(pinRef, ICON_CP, ICON_CP, netItem, TVI_FIRST);
                  SetTreeItemData(&m_tree, pinItem, pinRef, ICON_CP, compPin); 
               }
            }
         }
      }



      // Add vias to tree
      if (m_curFile)
      {
         Attrib *attrib = NULL;
         CString viaName = "";
         POSITION viaPos = m_curFile->getBlock()->getDataList().GetHeadPosition();







         while (viaPos)
         {
            DataStruct *via = m_curFile->getBlock()->getDataList().GetNext(viaPos);

            // Vias and Bond Pads get treated basically the same, only default name is built differently.
            if (via != NULL && (via->isInsertType(insertTypeVia) || via->isInsertType(insertTypeBondPad)))
            {
               viaName = via->getInsert()->getRefname();
               attrib = is_attvalue(m_doc, via->getAttributesRef(), ATT_NETNAME, 1);

               if (attrib && !attrib->getStringValue().CompareNoCase(net->getNetName()))
               {
                  HTREEITEM netrootChild = m_tree.GetChildItem(netItem);
                  HTREEITEM viaItem = this->FindTreeSiblingItem(netrootChild, viaName);

                  if (viaItem == NULL)
                  {
                     // Just add fast, we sort later anyway
                     CString defaultName( via->isInsertType(insertTypeBondPad) ? "BOND PAD" : "VIA" );
                     viaItem = m_tree.InsertItem(viaName.IsEmpty()?defaultName:viaName, ICON_VIA, ICON_VIA, netItem, TVI_FIRST);
                     SetTreeItemData(&m_tree, viaItem, viaName, ICON_VIA, via);
                  }
               }
            }
         }
      }

      // Sort the net's items
      TVSORTCB pSort;
      pSort.hParent = netItem;
      pSort.lParam = 0;
      pSort.lpfnCompare = TreeSortCompareFunc;
      m_tree.SortChildrenCB(&pSort);
   }
}

//----------------------------------------------------------------------------

void CAMCADNavigator::LoadProbeTree(NetStruct *net)
{
	
	if (net != NULL)
	{
      // Load net, comppins, vias
      LoadBasicNetTree(net);

		// Collect probes on this net
      CDataList probeDataList(false);         
      
      for (POSITION probePos = m_curFile->getBlock()->getDataList().GetHeadPosition();probePos != NULL;)
      {
         DataStruct *data = m_curFile->getBlock()->getDataList().GetNext(probePos);

         if (data != NULL && data->isInsertType(insertTypeTestProbe))
         {
            Attrib *netAttrib = is_attvalue(m_doc, data->getAttributeMap(), ATT_NETNAME, 0);

            if (netAttrib != NULL)
            {
               if (net->getNetName().CompareNoCase(netAttrib->getStringValue()) == 0)
               {
                  probeDataList.AddTail(data);
               }
            }
         }
		}

		HTREEITEM root = m_tree.GetRootItem();
      HTREEITEM netItem = this->FindTreeSiblingItem(root, net->getNetName());

      if (netItem != NULL)
      {
		   for (POSITION probeDataPos = probeDataList.GetHeadPosition(); probeDataPos != NULL;)
		   {
            DataStruct* probeData = probeDataList.GetNext(probeDataPos);
            DataStruct* viaAccess = NULL;
            CompPinStruct* cpAccess = NULL;

            if (probeData != NULL)
            {
               // If probe is placed it will have an access pt
               // Get the access point this probe is on
               Attrib *attrib = is_attvalue(m_doc, probeData->getAttributesRef(), ATT_DDLINK, 1);

               if (attrib != NULL)
               {
                  long entityNum = (long)attrib->getIntValue();
                  CEntity accessMarkEntity = CEntity::findEntity(getCamCadData(), entityNum);

                  if (accessMarkEntity.getEntityType() != entityTypeUndefined)
                  {
                     attrib = is_attvalue(m_doc, *(accessMarkEntity.getAttributesRef()), ATT_DDLINK, 1);

                     if (attrib != NULL)
                     {                    
                        entityNum = (long)attrib->getIntValue();
                        CEntity accessTargetEntity = CEntity::findEntity(getCamCadData(), entityNum);

                        if (accessTargetEntity.getEntityType() == entityTypeData)
                        {
                           viaAccess = accessTargetEntity.getData();
                        }
                        else if (accessTargetEntity.getEntityType() == entityTypeCompPin)
                        {
                           cpAccess = accessTargetEntity.getCompPin();
                        }
                     }
                  }
               } 

			      if (cpAccess == NULL && viaAccess == NULL && probeData != NULL)
			      {
#ifdef WASTE_TIME_LOOKING_FOR_SOMETHING_THAT_AINT_THERE
                  // We just emptied the tree before doing this add stuff.
                  // So if there is already something in the tree with same name then we truly have
                  // duplicates. This delete loop usually finds nothing in normal situations, so it
                  // is just a complete scan of the tree for every item we want to add.
                  // A waste of time, and these tree building operations are very time sensitive because
                  // the CTreeCtrl is itself pathetically slow.

				      HTREEITEM netrootChild = m_tree.GetChildItem(netItem);
				      while (netrootChild)
				      {
					      if (!m_tree.GetItemText(netrootChild).CompareNoCase(probeData->getInsert()->getRefname()))
					      {
						      m_tree.DeleteItem(root);
						      break;
					      }

					      netrootChild = m_tree.GetNextSiblingItem(netrootChild);
				      }
#endif
				      //put the probe under the net, unpplaced
				      HTREEITEM probeItem = m_tree.InsertItem(probeData->getInsert()->getRefname(), ICON_UNPLCD, ICON_UNPLCD, netItem, TVI_SORT);
				      SetTreeItemData(&m_tree, probeItem, probeData->getInsert()->getRefname(), ICON_UNPLCD, probeData);
			      }          

               if (cpAccess != NULL)
               {
                  // Make sure this stays in sync with how we make the name when putting item in the tree
                  CString pinRef = cpAccess->getRefDes() + "." + cpAccess->getPinName();
                  HTREEITEM netrootChild = m_tree.GetChildItem(netItem);
                  HTREEITEM pinItem = FindTreeSiblingItem(netrootChild, pinRef);

                  if (pinItem != NULL)
                  {
#ifdef WASTE_TIME_LOOKING_FOR_SOMETHING_THAT_AINT_THERE
                     // We just emptied the tree before doing this add stuff.
                     // So if there is already something in the tree with same name then we truly have
                     // duplicates. This delete loop usually finds nothing in normal situations, so it
                     // is just a complete scan of the tree for every item we want to add.
                     // A waste of time, and these tree building operations are very time sensitive because
                     // the CTreeCtrl is itself pathetically slow.

                     HTREEITEM killroot = m_tree.GetChildItem(pinItem);
                     while (killroot)
                     {
                        if (!m_tree.GetItemText(killroot).CompareNoCase(probeData->getInsert()->getRefname()))
                        {
                           m_tree.DeleteItem(killroot);
                           break;
                        }

                        killroot = m_tree.GetNextSiblingItem(killroot);
                     }
#endif
                     //put the probe under this pin
                     HTREEITEM probeItem = m_tree.InsertItem(probeData->getInsert()->getRefname(), ICON_PLCD, ICON_PLCD, pinItem, TVI_SORT);
                     SetTreeItemData(&m_tree, probeItem, probeData->getInsert()->getRefname(), ICON_PLCD, probeData);
                  }
               }

               if (viaAccess != NULL)
               {
                  HTREEITEM netrootChild = m_tree.GetChildItem(netItem);
                  HTREEITEM viaItem = FindTreeSiblingItem(netrootChild, viaAccess);

                  if (viaItem != NULL)
                  {
#ifdef WASTE_TIME_LOOKING_FOR_SOMETHING_THAT_AINT_THERE
                     // We just emptied the tree before doing this add stuff.
                     // So if there is already something in the tree with same name then we truly have
                     // duplicates. This delete loop usually finds nothing in normal situations, so it
                     // is just a complete scan of the tree for every item we want to add.
                     // A waste of time, and these tree building operations are very time sensitive because
                     // the CTreeCtrl is itself pathetically slow.

                     HTREEITEM killroot = m_tree.GetChildItem(viaItem);
                     while (killroot)
                     {
                        if (!m_tree.GetItemText(killroot).CompareNoCase(probeData->getInsert()->getRefname()))
                        {
                           m_tree.DeleteItem(killroot);
                           break;
                        }

                        killroot = m_tree.GetNextSiblingItem(killroot);
                     }
#endif
                     //put the access under this via
                     HTREEITEM probeItem = m_tree.InsertItem(probeData->getInsert()->getRefname(), ICON_PLCD, ICON_PLCD, viaItem, TVI_SORT);
                     SetTreeItemData(&m_tree, probeItem, probeData->getInsert()->getRefname(), ICON_PLCD, probeData);
                  }
               }
            }
         }
		}

		m_nets.AddTail(net);
	} 
}
//----------------------------------------------------------------------------
  
bool CAMCADNavigator::GetPlacedProbesZoomRegion(double *left, double *right, double *top, double *bottom)
{
   bool foundRegion = false;

   if (left != NULL && right != NULL && bottom != NULL && top != NULL)
   {
      *top    = *right = -100000;
      *bottom = *left  =  100000;

      double xmin, xmax, ymin, ymax;
      int probeIndex = 0;

      for (POSITION probePos = m_placedProbes.GetHeadPosition();probePos != NULL;probeIndex++)
      {
         DataStruct *data = m_placedProbes.GetNext(probePos);

         if (data != NULL && activeView != NULL)
         {

            if (isWindow())
               UpdateData();

            CString refname = data->getInsert()->getRefname();

            if (!ComponentsExtentsByDataStruct(m_doc, data, m_curFile, &xmin, &xmax, &ymin, &ymax))
            {
               PanReference(((CCEtoODBView*)activeView), refname);
               ErrorMessage("Component has no extents", "Panning");
            }
            else
            {

               data->setHighlightColor(highlightColors[probeIndex % HighlightColorCount]);

               ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
               *left = min(*left, xmin);
               *right = max(*right, xmax);
               *bottom = min(*bottom, ymin);
               *top = max(*top, ymax);

               foundRegion = true;
            }
         }

      }
   }
   return foundRegion;
}

//----------------------------------------------------------------------------
 
bool CAMCADNavigator::GetUnplacedProbesZoomRegion(double *left, double *right, double *top, double *bottom)
{
   if (left == NULL || right == NULL || bottom == NULL || top == NULL)
      return false;

   *top    = *right = -100000;
   *bottom = *left  =  100000;

	double xmin, xmax, ymin, ymax;
	int probeIndex = 0;

	for (POSITION probePos = m_unplacedProbes.GetHeadPosition();probePos != NULL;probeIndex++)
	{
		//DataStruct *probeData = (DataStruct*)navUnplacedProbeArray.GetAt(i);
		DataStruct* probeData = m_unplacedProbes.GetNext(probePos);

		if (!probeData)
			continue;

		Attrib *attrib = is_attvalue(m_doc, probeData->getAttributesRef(), ATT_NETNAME, 1);

		if (!attrib)
			continue;

		CString netName = attrib->getStringValue();

		NetStruct* net = FindNet(m_curFile, netName);

		if (!net)
			continue;

		// Make sure there is an extent before zooming
		if (!NetExtents(m_doc, net->getNetName(), &xmin, &xmax, &ymin, &ymax))
		{
			ErrorMessage("Net has no extents", "Panning to Net");
			return false;
		}

		CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
		highlightNet(*net,highlightColors[probeIndex % HighlightColorCount]);


      ApplyMargin(m_curZoomMargin, &xmin, &xmax, &ymin, &ymax);
      *left = min(*left, xmin);
      *right = max(*right, xmax);
      *bottom = min(*bottom, ymin);
      *top = max(*top, ymax);

	}
 
	return true;
}

//----------------------------------------------------------------------------
 
void CAMCADNavigator::OnNMReleasedcaptureZoomslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_curZoomMargin != m_zoomSlider.GetPos())
	{
      CString zoomMargin;

      /// The tooltip has been turned off. It is a slider that gives more zoom or less 
      /// zoom, and the actual value in the slider is not very meaningful for users.
      /// If you want to re-enable this code, you have to go to the RC file and set
      /// ToolTips True.	
		///zoomMargin.Format("%d", m_maxZoomMarginSetting - m_curZoomMargin);  // reverse apparant direction of slider
		///m_zoomSlider.GetToolTips()->SetWindowText(zoomMargin);

		m_curZoomMargin = m_zoomSlider.GetPos();
      zoomMargin.Format("%d", m_curZoomMargin);
      CAppRegistrySetting("PCB Navigator", "Zoom Margin").Set(zoomMargin);
	}

	*pResult = 0;
}

//----------------------------------------------------------------------------
 
void CAMCADNavigator::UpdateCountsDisplay()
{
   CString cmpCountStr;
   CString manfCountStr;
   CString netCountStr;
   CString netsWithAccessCntStr;
   CString netsWithNoAccessCntStr;
   CString placedProbesCountStr;
   CString unplacedProbesCountStr;
   CString boardCountStr;

   cmpCountStr.Format("(%d)", compCount);
   manfCountStr.Format("(%d)", m_manfCount);
   netCountStr.Format("(%d)", netCount);
   placedProbesCountStr.Format("(%d)", placedProbesCount);
   unplacedProbesCountStr.Format("(%d)", unplacedProbesCount);
   netsWithAccessCntStr.Format("(%d)", netsWithAccessCnt);
   netsWithNoAccessCntStr.Format("(%d)", netsWithNoAccessCnt);
   boardCountStr.Format("(%d)", m_boardCount);

   GetDlgItem(IDC_STATIC_CMPCNT)->SetWindowText(cmpCountStr);
   GetDlgItem(IDC_STATIC_MANFCNT)->SetWindowText(manfCountStr);
   GetDlgItem(IDC_STATIC_NETCNT)->SetWindowText(netCountStr);
   GetDlgItem(IDC_STATIC_NETACCESSCNT)->SetWindowText(netsWithAccessCntStr);
   GetDlgItem(IDC_STATIC_NETNOACCESSCNT)->SetWindowText(netsWithNoAccessCntStr);
   GetDlgItem(IDC_STATIC_PLCDCNT)->SetWindowText(placedProbesCountStr);
   GetDlgItem(IDC_STATIC_UNPLCDCNT)->SetWindowText(unplacedProbesCountStr);
   GetDlgItem(IDC_STATIC_BOARDCNT)->SetWindowText(boardCountStr);
}

/**********************************************88
*/
void CAMCADNavigator::Enable(bool flag)
{
   bool pcbflag = (m_curFile && m_curFile->getBlockType() == blockTypePanel)?false:flag;
   bool panelflag = (m_curFile && m_curFile->getBlockType() == blockTypePcb)?false:flag;

   // Mode box and radio buttons
   GetDlgItem(IDC_MODE_GROUPBOX)->EnableWindow(flag);

   // Mode box for Board
   GetDlgItem(IDC_RADIO_COMPMODE)->EnableWindow(pcbflag);
   GetDlgItem(IDC_RADIO_MANFMODE)->EnableWindow(flag);
   GetDlgItem(IDC_RADIO_NETMODE)->EnableWindow(pcbflag);
   GetDlgItem(IDC_NETSACCESS)->EnableWindow(pcbflag);
   GetDlgItem(IDC_NETSNOACCESS)->EnableWindow(pcbflag);
   GetDlgItem(IDC_RADIO_PLACEDPROBES)->EnableWindow(pcbflag);
   GetDlgItem(IDC_RADIO_UNPLACEDPROBES)->EnableWindow(pcbflag);
   
   // Mode box for Panel
   GetDlgItem(IDC_RADIO_BOARDMODE)->EnableWindow(panelflag);  

   // Counts
   GetDlgItem(IDC_STATIC_CMPCNT)->EnableWindow(pcbflag);
   GetDlgItem(IDC_STATIC_MANFCNT)->EnableWindow(flag);
   GetDlgItem(IDC_STATIC_NETCNT)->EnableWindow(pcbflag);
   GetDlgItem(IDC_STATIC_NETACCESSCNT)->EnableWindow(pcbflag);
   GetDlgItem(IDC_STATIC_NETNOACCESSCNT)->EnableWindow(pcbflag);
   GetDlgItem(IDC_STATIC_PLCDCNT)->EnableWindow(pcbflag);
   GetDlgItem(IDC_STATIC_UNPLCDCNT)->EnableWindow(pcbflag);

   // Counts for Panel
   GetDlgItem(IDC_STATIC_BOARDCNT)->EnableWindow(panelflag);

   // Zoom Margin Control
   GetDlgItem(IDC_ZoomSlider)->EnableWindow(flag);
   GetDlgItem(IDC_ZoomSliderLabel)->EnableWindow(flag);

   // Other stuff
   GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(flag);
   GetDlgItem(IDC_EDIT_NAV_FIND)->EnableWindow(flag);
   GetDlgItem(IDC_BUTTON_NAV_FIND)->EnableWindow(flag);
   GetDlgItem(IDC_BUTTON_NAV_CLEAR)->EnableWindow(flag);
   // not working ... GetDlgItem(IDC_LIST)->EnableWindow(flag);
   // so don't do this one either GetDlgItem(IDC_TREE_NAVIGATOR)->EnableWindow(flag);

   GetDlgItem(IDC_VARIANT_LABEL)->EnableWindow(flag);
   GetDlgItem(IDC_EDIT_VARIANT)->EnableWindow(flag);
   GetDlgItem(IDC_MACHINE_LABEL)->EnableWindow(flag);
}

/**********************************************88
*/
void CAMCADNavigator::OnInit()
{
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_NET));
   m_imageList->Add(app->LoadIcon(IDI_CP));
   m_imageList->Add(app->LoadIcon(IDI_CPA));
   m_imageList->Add(app->LoadIcon(IDI_COMPBLK));
   m_imageList->Add(app->LoadIcon(IDI_VIA));
   m_imageList->Add(app->LoadIcon(IDI_ICON_PLCD));
   m_imageList->Add(app->LoadIcon(IDI_ICON_UNPLCD));
   m_imageList->Add(app->LoadIcon(IDI_ICON_ACCESSMARKER));
   m_imageList->Add(app->LoadIcon(IDI_CP));
   m_imageList->Add(app->LoadIcon(IDI_Fiducial));
   m_imageList->Add(app->LoadIcon(IDI_DRILL));
   m_imageList->Add(app->LoadIcon(IDI_TARGET));
   m_imageList->Add(app->LoadIcon(IDI_DIE));
   
   PROBE_PLACEMENT = m_doc->RegisterKeyWord("PROBE_PLACEMENT", 0, VT_STRING);
   DATALINK = m_doc->RegisterKeyWord(ATT_DDLINK, 0, VT_INTEGER);
   WORD testResKW = m_doc->RegisterKeyWord("TEST_RESOURCE", 0, VT_STRING);

   // Smaller margin is greater zoom. By Mark's request, arrange control so 
   // max zoom as at right of slider, min zoom is at left. The natural thing
   // is to just make the slider values large to small, but these sliders 
   // apparantly don't support that. So upon retreiving a value, it has to be
   // reversed.
   m_zoomSlider.SetRange(0, m_maxZoomMarginSetting);
   CString zoomStr = CAppRegistrySetting("PCB Navigator", "Zoom Margin").Get();
   if (!zoomStr.IsEmpty())
      m_curZoomMargin = atoi(zoomStr);
   if (m_curZoomMargin < 0 || m_curZoomMargin > m_maxZoomMarginSetting)
      m_curZoomMargin = 1;
	m_zoomSlider.SetPos(m_curZoomMargin);

	// case 1811
	// Clear potential leftover highlights before emptying lists
	clearHighlights();

   m_components.empty();
   m_nets.RemoveAll();
   m_placedProbes.empty();
   m_unplacedProbes.empty();
   netsWithAccess.SetSize(100,100);
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();

   netCount  = 0;
   compCount = 0;
   m_manfCount = 0;
   netsWithAccessCnt = 0;
   netsWithNoAccessCnt = 0;
   placedProbesCount = 0;
   unplacedProbesCount = 0;

   //set different highlight colors for multi selection
   /*highlightColors[0] = RGB(255,128,128);
   highlightColors[1] = RGB(128,255,128);
   highlightColors[2] = RGB(255,128,64);
   highlightColors[3] = RGB(128,128,255);
   highlightColors[4] = RGB(255,0,128);*/
   //highlightColors[0] = m_doc->Settings



   m_list.ResetContent();
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);

   if (docNotPCB)
   {
      UpdateCountsDisplay();
      Enable(false);
      
      m_doc = NULL;
      m_curFile = NULL;

      return;
   }

   Enable(true);   

/* Comment out these code to fix 1105, 1108, 1166

   //case 1071
   //unselect all
   m_doc->ClearSubSelectArray(0);

   POSITION selPos = m_doc->SelectList.GetHeadPosition();
   while (selPos != NULL)
   {
      SelectStruct *s = m_doc->SelectList.GetNext(selPos);
      s->p->setSelected(false);
      s->p->setMarked(false);
      int layer = s->p->getLayerIndex();
      delete s;
   }
   m_doc->SelectList.RemoveAll();

   if (Editing && editDlg)
   {
      if (nothingDlg && IsWindow(*nothingDlg))
      {
         nothingDlg->ShowWindow(SW_HIDE);
         nothingDlg->DestroyWindow();
         delete nothingDlg;
         nothingDlg = NULL;
      }
      
      if (editDlg && IsWindow(*editDlg) && IsWindow(*nothingDlg))
      {
         editDlg->GetWindowPlacement(&editPlacement);
         editDlg->ShowWindow(SW_HIDE);

         editDlg->DestroyWindow();
         delete editDlg;
         editDlg = NULL;
      }

      nothingDlg = new EditNothing();
      nothingDlg->Create(IDD_EDIT_NOTHING);
      CRect rect;
      nothingDlg->GetWindowRect(rect);
      editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
         rect.bottom - rect.top;
      nothingDlg->SetWindowPlacement(&editPlacement);
   }*/

   UpdateProbes(m_doc);

   FillNetsWithAccess();

   if(m_curFile->getBlockType() == blockTypePcb)
   {
      FillNetsWithNoAccess();
      FillNetList();
      FillManufactList(m_curFile);
      FillCompList(m_curFile);
   }
   else // blockTypePanel
   {
      FillManufactList(m_curFile);
      FillBoardList(m_curFile);
   }
   
   m_tree.ShowWindow(SW_SHOW);
   m_list.UpdateHExtent();
   m_list.ShowWindow(SW_SHOW);

   UpdateCountsDisplay();

   GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(FALSE);
   
   if (isWindow())
      UpdateData();

   m_compMode = (m_curFile->getBlockType() == blockTypePcb)?navigatorModeComponents:navigatorModeBoards;

   if (isWindow())
      UpdateData(FALSE);
}

/************************************************
* CAMCADNavigator::CountNetsWithAccess
*/
void CAMCADNavigator::CountNetsWithAccess(CCEtoODBDoc* document)
{
   // KNOWN BUG
   // Inconsistency in net type counting.
   // CountNetsWithAccess uses the AA solution, if no AA then no nets have access, access marker presence is irrelevant.
   // FillNetsWithAccess looks at presence of access markers, regardless of AA.
   // Both update same counter var, so display of counts changes depending on operation at hand.
   // Reported to Mark 4/19/07, he said skip it for now. No case as of this writing.

   // The bug could be fixed two ways (at least).
   // One is make FillNetsWithAccess consult the AA solution and only accept access markers if there is an AA.
   // Other is to make CountNetsWithAccess ignore the AA and go ahead and react to existing access markers, like FileNets..().
   // Could be a performance issue with that, though, as that Count func gets called alot when Navigotor updates in
   // reaction to camcad changes in general. It is faster to loop on the nets than to search the ccz for all access markers.

   // I suppose one way to go is if there is an AA then use it, if not then loop on access markers.

   netsWithNoAccessCnt = 0;
   netsWithAccessCnt = 0;

   if (docNotPCB || !document)
      return;

   netsWithAccess.RemoveAll();

	CAccessAnalysisSolution* aaSolution = NULL;
	/*if (m_curFile != NULL)
		aaSolution = (getDoc()->GetCurrentDFTSolution(*m_curFile)==NULL)?NULL:getDoc()->GetCurrentDFTSolution(*m_curFile)->GetAccessAnalysisSolution();*/
   
	if (aaSolution != NULL)
	{
      for (POSITION pos=m_curFile->getNetList().GetHeadPosition(); pos!=NULL;)
      {
         NetStruct* netStruct = m_curFile->getNetList().GetNext(pos);
         if (netStruct != NULL)
         {
            CAANetAccess* netAccess = aaSolution->GetNetAccess(netStruct->getNetName());
            if (netAccess == NULL)
               continue;
            if (!netAccess->IsNetAnalyzed())
               continue;

            if (netAccess->GetTopAccessibleCount() == 0 && netAccess->GetBotAccessiblecount() == 0)
               netsWithNoAccessCnt++;
            else
               netsWithAccess.SetAtGrow(netsWithAccessCnt++, (void*)netStruct);
         }
      }
	}
   else
   {
      //Code for finding netsWithAccess count
      if (!m_curFile)
         return;
      CMapStringToPtr map;
      BlockStruct *block = m_curFile->getBlock();
      if (block != NULL && 
         (block->getBlockType() == blockTypePcb || m_curFile->getBlockType() == blockTypePcb))
      {
         Attrib* attribPtr = NULL;
         void* voidPtr = NULL;
         POSITION accessPos = block->getHeadDataInsertPosition();

         while (accessPos)
         {
            DataStruct* accessData = block->getNextDataInsert(accessPos);

            if (accessData != NULL && accessData->isInsertType(insertTypeTestAccessPoint) &&
               accessData->getAttributeMap() && accessData->getAttributeMap()->Lookup(m_doc->IsKeyWord(ATT_NETNAME, 0), attribPtr))
            {
               Attrib *attrib = is_attvalue(m_doc, accessData->getAttributeMap(), ATT_NETNAME, 0);
               CString netName = attrib?attrib->getStringValue():"";

               if (!netName.IsEmpty() && !map.Lookup(netName, voidPtr))
               {
                  NetStruct *net = FindNet(m_curFile, netName);
                  if (net)
                  {
                     SortListHelper *helper = new SortListHelper;
                     helper->voidptr = net;
                     helper->typeTag = dataTypeUndefined;
                     netsWithAccess.SetAtGrow(netsWithAccessCnt++, net);
                     map.SetAt(netName, (void*)helper);
                  }
               }
            }
         }
      }

      ////Code for finding netsWithNoAccess count
      map.RemoveAll();
      void* voidPtr = NULL;
      POSITION netPos = m_curFile->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct* net = m_curFile->getNetList().GetNext(netPos);
         if (!net || IsNetAccessible(net))
            continue;         
         if (!map.Lookup(net->getNetName(), voidPtr))
         {
            SortListHelper *helper = new SortListHelper;
            helper->voidptr = net;
            helper->typeTag = dataTypeUndefined; // nets don't matter for current purposes

            netsWithNoAccessCnt++;
            map.SetAt(net->getNetName(), (void*)helper);       
         }
      }
      map.RemoveAll();
   }

   CString netsWithAccessCntStr;
   CString netsWithNoAccessCntStr;

   netsWithAccessCntStr.Format("(%d)", netsWithAccessCnt);
   netsWithNoAccessCntStr.Format("(%d)", netsWithNoAccessCnt);

   GetDlgItem(IDC_STATIC_NETACCESSCNT)->SetWindowText(netsWithAccessCntStr);
   GetDlgItem(IDC_STATIC_NETNOACCESSCNT)->SetWindowText(netsWithNoAccessCntStr);
}

/**********************************************88
* CAMCADNavigator::setDoc)
*/
void CAMCADNavigator::setDoc(CCEtoODBDoc *document)
{
   frame = (CMainFrame*)AfxGetMainWnd();
   docNotPCB = FALSE;

   if (!document || (!document->getFileList().GetOnlyShown(blockTypePcb,blockTypePanel) && !document->getFileList().GetOnlyShown(blockTypePanel, blockTypePcb)))
   {
      ResetCounts();

      if (IsWindow(m_list))
      {
         m_list.ResetContent();
         DeleteTreeItemDatasMem();
         m_tree.DeleteAllItems();
         clearHighlights();
         m_components.empty();
         m_nets.RemoveAll();
         m_placedProbes.empty();
         m_unplacedProbes.empty();

         m_list.ResetContent();
         m_tree.SelectItem(NULL);
         m_tree.SelectDropTarget(NULL);
         UpdateCountsDisplay();

         Enable(false);

         // Variant controls
         m_sVariantName.Empty();    
//			GetDlgItem(IDC_BUTTON_VARIANT_CHANGE)->SetWindowText("");
//       GetDlgItem(IDC_BUTTON_VARIANT_CHANGE)->EnableWindow(FALSE);

         // Machine controls
         m_sMachineName.Empty();
         m_machineSide.ClearAll();
         GetDlgItem(IDC_LIST_MACHINESIDE)->EnableWindow(false);

//			GetDlgItem(IDC_BUTTON_MACHINE_CHANGE)->SetWindowText("");
//       GetDlgItem(IDC_BUTTON_MACHINE_CHANGE)->EnableWindow(FALSE);
      }

      docNotPCB = TRUE;
      m_curFile = NULL;
      m_doc = NULL;

      UpdateData(FALSE);
      return;
   }


   UpdateProbes(document);
   CountNetsWithAccess(document);

   FileStruct *selectedfile = document->getFileList().GetOnlyShown(blockTypePcb,blockTypePanel);
   if(!selectedfile) selectedfile = document->getFileList().GetOnlyShown(blockTypePanel, blockTypePcb);

   // DR 851503 - This line used to be further below, after the check for curFile not
   // changing and net count not changing. Problem is, the net count might be the same
   // but the nets themselves have changed. Failure to update leads to crash, because
   // Navigator has stale pointers in its pin map table. Since there is not convenient way
   // to know if netlist changed, just always update the CompPinNetArray.
   SetCompPinNetArray();
   
   if (m_curFile == selectedfile && document == m_doc)
   {
      if (getNetCount() == selectedfile->getNetCount())
         return;
   }

	m_curFile = selectedfile;
   m_doc = document;

   FillMachineList(m_doc, m_curFile);
   enableAndFillVariantList();
   enableAndFillMachineList();

   if (isWindow())
   {
      UpdateData(FALSE);
   }

   OnInit();
}

/**********************************************88
* CAMCADNavigator::OnBnClickedButtonFind
*/
void CAMCADNavigator::OnBnClickedButtonFind()
{
   if (docNotPCB)
      return;

   CString findStr = "";
   m_findText.GetWindowText(findStr);

   if (!findStr.IsEmpty())
   {
      OnBnClickedButtonClear();
      int nIndex = 0;

      while (nIndex < m_list.GetCount())
      {
         CString temp = "";
         m_list.GetText(nIndex, temp);

         if (temp.Left(findStr.GetLength()).CompareNoCase(findStr) == 0)
         {
            m_list.SetSel(nIndex, TRUE);
            m_list.SetTopIndex(nIndex);  // Case 1912, scroll selected item to top of listbox
            OnLbnSelchangeList();  // Case 1912, auto-select item that was found

            return;
         }

         nIndex++;
      }
   }
}

/************************************************
* CAMCADNavigator::OnBnClickedButtonClear
*/
void CAMCADNavigator::OnBnClickedButtonClear()
{
   if (docNotPCB)
      return;

   m_list.SetSel(-1, FALSE);
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();
   
   //clear net highlights
   UnHighLightNets();

   //clear comp highlights
   clearHighlights();

   if (isWindow())
      UpdateData();
}

/**********************************************88
* CAMCADNavigator::OnBnClickedShowSelected
*/
void CAMCADNavigator::OnBnClickedShowSelected()
{
   if (docNotPCB)
      return;

   ShowSelectedOnly = !ShowSelectedOnly;

   if (isWindow())
      UpdateData(FALSE);

   Show_selected_only();
}

/**********************************************88
* CAMCADNavigator::Show_selected_only
*/
void CAMCADNavigator::Show_selected_only()
{
   if (ShowSelectedOnly)
   {
      // hide all nets except selected ones
      Hide_Nets_All(m_doc, TRUE);

      POSITION pos = m_doc->HighlightedNetsMap.GetStartPosition();  // previous selected

      while (pos)
      {
         int netValueIndex;
         COLORREF color;
         m_doc->HighlightedNetsMap.GetNextAssoc(pos, netValueIndex, color);
         Hide_Net(m_doc, netValueIndex, FALSE);
      }

      //Hide_Net(m_doc, net->getNetName(), FALSE);  // current selected
   }
   else
   {
      // unhide all nets
      Hide_Nets_All(m_doc, FALSE);
   }

   //doc->UpdateAllViews(NULL);
}

void CAMCADNavigator::highlightNet(NetStruct& net,COLORREF color)
{
   highlightNet(net.getNetName(),color);
}

void CAMCADNavigator::highlightNet(const CString& netName,COLORREF color)
{
   int value = m_doc->RegisterValue(netName);
   m_doc->HighlightedNetsMap.SetAt(value,color);
   m_doc->HighlightByAttrib(FALSE, m_doc->IsKeyWord(ATT_NETNAME, 0), VT_STRING, value);
}

void CAMCADNavigator::clearHighlights()
{
   for (POSITION pos = m_components.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_components.GetNext(pos);
      data->setHighlightColor(0);
   }

   for (POSITION pos = m_placedProbes.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_placedProbes.GetNext(pos);
      data->setHighlightColor(0);
   }

   for (POSITION pos = m_unplacedProbes.GetHeadPosition();pos != NULL;)
   {
      DataStruct* data = m_unplacedProbes.GetNext(pos);
      data->setHighlightColor(0);
   }

   //for (int i=0; i<navCompArrayCount; i++)
   //{
   //   if ((DataStruct*)navCompArray.GetAt(i))
   //   {
   //      //((DataStruct*)navCompArray.GetAt(i))->setColorOverride(false);
   //      ((DataStruct*)navCompArray.GetAt(i))->setHighlightColor(0);
   //   }
   //}

   //for (int i=0; i<navPlacedProbeArrayCount; i++)
   //{
   //   if ((DataStruct*)navPlacedProbeArray.GetAt(i))
   //   {
   //      //((DataStruct*)navPlacedProbeArray.GetAt(i))->setColorOverride(false);
   //      ((DataStruct*)navPlacedProbeArray.GetAt(i))->setHighlightColor(0);
   //   }
   //}

   //for (int i=0; i<navUnplacedProbeArrayCount; i++)
   //{
   //   if ((DataStruct*)navUnplacedProbeArray.GetAt(i))
   //   {
   //      //((DataStruct*)navUnplacedProbeArray.GetAt(i))->setColorOverride(false);
   //      ((DataStruct*)navUnplacedProbeArray.GetAt(i))->setHighlightColor(0);
   //   }
   //}
}

/**********************************************88
* CAMCADNavigator::SetCompPinNetArray
*/
void CAMCADNavigator::SetCompPinNetArray()
{
   delete m_camCadPinMap;
   m_camCadPinMap = NULL;

   if (m_curFile != NULL)
   {
      m_camCadPinMap = new CCamCadPinMap(m_curFile);
      m_camCadPinMap->resync();
   }
}

/**********************************************88
* CAMCADNavigator::GetNetNameByCompPin
*/
CString CAMCADNavigator::GetNetNameByCompPin(const CString& pinRef)
{
   CString netName;

   if (m_camCadPinMap != NULL)
   {
      CCamCadPin* camCadPin = m_camCadPinMap->getCamCadPin(pinRef);

      if (camCadPin != NULL)
      {
         NetStruct *net = camCadPin->getNet();

         if (net != NULL)
         {
            netName = net->getNetName();
         }
      }
   }

   return netName;
}

CCamCadPin* CAMCADNavigator::getCamCadPin(const CString& pinRef)
{
   CCamCadPin* camCadPin = NULL;

   if (m_camCadPinMap != NULL)
   {
      camCadPin = m_camCadPinMap->getCamCadPin(pinRef);
   }

   return camCadPin;
}

CompPinStruct* CAMCADNavigator::getCompPin(const CString& pinRef)
{
   CompPinStruct* compPin = NULL;

   if (m_camCadPinMap != NULL)
   {
      CCamCadPin* camCadPin = m_camCadPinMap->getCamCadPin(pinRef);

      if (camCadPin != NULL)
      {
         compPin = camCadPin->getCompPin();
      }
   }

   return compPin;
}

/**********************************************88
* CAMCADNavigator::OnBnClickedNetsaccess
*/
void CAMCADNavigator::OnBnClickedNetsaccess()
{
   clearHighlights();
   Reset();
   FillNetsWithAccess();
   UpdateCountsDisplay();
}

/**********************************************88
* CAMCADNavigator::Reset
*/
void CAMCADNavigator::Reset()
{
	Hide_Nets_All(m_doc, FALSE);
   UnHighLightNets();
   m_list.ResetContent();
   m_tree.ShowWindow(SW_HIDE);
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();
}

/**********************************************88
* CAMCADNavigator::FillNetsWithAccess
*/
void CAMCADNavigator::FillNetsWithAccess()
{
   // KNOWN BUG
   // Inconsistency in net type counting.
   // CountNetsWithAccess uses the AA solution, if no AA then no nets have access, access marker presence is irrelevant.
   // FillNetsWithAccess looks at presence of access markers, regardless of AA.
   // Both update same counter var, so display of counts changes depending on operation at hand.
   // Reported to Mark 4/19/07, he said skip it for now. No case as of this writing.

   // The bug could be fixed two ways (at least).
   // One is make FillNetsWithAccess consult the AA solution and only accept access markers if there is an AA.
   // Other is to make CountNetsWithAccess ignore the AA and go ahead and react to existing access markers, like FileNets..().
   // Could be a performance issue with that, though, as that Count func gets called alot when Navigotor updates in
   // reaction to camcad changes in general. It is faster to loop on the nets than to search the ccz for all access markers.

   // I suppose one way to go is if there is an AA then use it, if not then loop on access markers.

   if (docNotPCB)
      return;

   if (!m_curFile)
      return;

   m_list.SetSel(-1);

   m_compMode = navigatorModeNetsWithAccess;

   netsWithAccessCnt = 0;
   netsWithAccess.RemoveAll();
   CMapStringToPtr map;
   map.RemoveAll();

   BlockStruct *block = m_curFile->getBlock();

   // Case 2203 related.
   // Some importers do not create consistent data, they make a file with
   // block type PCB but the block itself is not type PCB. This is technically
   // an importer error, but we'll roll and go with it.

   // We'll report this only once per camcad session. It lets us know there is
   // a problem, but we also tolerate that problem here. It might not be tolerated
   // elsewhere, though. Reporting once per session seems like enough.
   if (block != NULL && !fileBlockErrorReported &&
      (block->getBlockType() != m_curFile->getBlockType()))
   {
      fileBlockErrorReported = true;
      CString msg;
      msg.Format(
         "File block type setting (%s) does not match actual block's type setting (%s).\n"
         "This is most likely the result of a CAD Data Importer error.\n"
         "Please report this event with the original CAD data source file to Customer Support.\n"
         "\n"
         "CAMCAD will most likely continue to process this file correctly, there is no need to terminate this session.\n"
         "There is, however, a data inconsistency, so some operations may fail.",
         blockTypeToDisplayString(m_curFile->getBlockType()), blockTypeToDisplayString(block->getBlockType()));

      ErrorMessage(msg, "Internal Error");     
   }

   if (block != NULL && 
      (block->getBlockType() == blockTypePcb || m_curFile->getBlockType() == blockTypePcb))
   {
      Attrib* attribPtr = NULL;
      void* voidPtr = NULL;
      POSITION accessPos = block->getHeadDataInsertPosition();

      while (accessPos)
      {
         DataStruct* accessData = block->getNextDataInsert(accessPos);

			if (accessData != NULL && accessData->isInsertType(insertTypeTestAccessPoint) &&
             accessData->getAttributeMap() && accessData->getAttributeMap()->Lookup(m_doc->IsKeyWord(ATT_NETNAME, 0), attribPtr))
         {
            Attrib *attrib = is_attvalue(m_doc, accessData->getAttributeMap(), ATT_NETNAME, 0);
            CString netName = attrib?attrib->getStringValue():"";
            
            if (!netName.IsEmpty() && !map.Lookup(netName, voidPtr))
            {
               NetStruct *net = FindNet(m_curFile, netName);
               if (net)
               {
                  SortListHelper *helper = new SortListHelper;
                  helper->voidptr = net;
                  helper->typeTag = dataTypeUndefined; // nets don't matter for current purposes

                  netsWithAccess.SetAtGrow(netsWithAccessCnt++, net);
                  map.SetAt(netName, (void*)helper);
               }
            }
         }
      }
   }

   SortList(map);
   m_tree.ShowWindow(SW_SHOW);
}


/**********************************************
* CAMCADNavigator::OnBnClickedNetsnoaccess
*/
void CAMCADNavigator::OnBnClickedNetsnoaccess()
{
   clearHighlights();
   Reset();
   FillNetsWithNoAccess();
   UpdateCountsDisplay();
}

/**********************************************88
* CAMCADNavigator::FillNetsWithNoAccess
*/
void CAMCADNavigator::FillNetsWithNoAccess()
{
   if (docNotPCB)
      return;

   if (!m_doc || !m_curFile)
      return;

   m_list.SetSel(-1);

   m_compMode = navigatorModeNetsWithoutAccess;

   CMapStringToPtr map;
   map.RemoveAll();
   void* voidPtr = NULL;
   netsWithNoAccessCnt = 0;

	CAccessAnalysisSolution* aaSolution = NULL;//(getDoc()->GetCurrentDFTSolution(*m_curFile)==NULL)?NULL:getDoc()->GetCurrentDFTSolution(*m_curFile)->GetAccessAnalysisSolution();

   POSITION netPos = m_curFile->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct* net = m_curFile->getNetList().GetNext(netPos);
      if (!net || IsNetAccessible(net))
         continue;
      
		if (aaSolution != NULL)
		{
			// Do not put not analyzed net in the "Net With No Access" list
			CAANetAccess* netAccess = aaSolution->GetNetAccess(net->getNetName());
			if (netAccess == NULL || !netAccess->IsNetAnalyzed())
				continue;
		}

      if (!map.Lookup(net->getNetName(), voidPtr))
      {
         SortListHelper *helper = new SortListHelper;
         helper->voidptr = net;
         helper->typeTag = dataTypeUndefined; // nets don't matter for current purposes

         netsWithNoAccessCnt++;
         map.SetAt(net->getNetName(), (void*)helper);       
      }
   }

   SortList(map);
   m_tree.ShowWindow(SW_SHOW);
}

/**********************************************88
* CAMCADNavigator::IsNetAccessible
*/
BOOL CAMCADNavigator::IsNetAccessible(NetStruct *net)
{
   if (net == NULL)
      return FALSE;

   for (int i=0; i<netsWithAccessCnt; i++)
   {
      NetStruct* netStruct = (NetStruct*)netsWithAccess.GetAt(i);

      if (netStruct == NULL)
         continue;

      if (!net->getNetName().CompareNoCase(netStruct->getNetName()))
         return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* CAMCADNavigator::UpdateNetMap
*/
void CAMCADNavigator::UpdateNetMap(CCEtoODBDoc *newDoc)
{
   if (newDoc == NULL)
   {
      m_netLookupMap.empty();

      return;
   }

   if (newDoc == m_doc && m_curFile == newDoc->getFileList().GetOnlyShown(blockTypePcb,blockTypePanel))
      return;

   m_netLookupMap.empty();
   FileStruct *file = newDoc->getFileList().GetOnlyShown(blockTypePcb,blockTypePanel);

   if (file == NULL)
      return;

   for (POSITION pos=file->getNetList().GetHeadPosition(); pos!=NULL; file->getNetList().GetNext(pos))
   {
      NetStruct *net = file->getNetList().GetAt(pos);
      m_netLookupMap.SetAt(net->getNetName(), net);
   }
}

/**********************************************88
* CAMCADNavigator::OnBnClickedRadioPlacedProbes
*/
void CAMCADNavigator::OnBnClickedRadioPlacedProbes()
{
   if (docNotPCB)
      return;

   if (!m_doc)
      return;

   m_list.SetSel(-1);

   clearHighlights();

   GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(TRUE);

   m_doc->UnselectAll(FALSE);
   UnHighLightNets();
   m_list.ResetContent();
   m_compMode = navigatorModePlacedProbes;
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   FillPlacedProbes(m_curFile);
   UpdateCountsDisplay();
}

/**********************************************88
* CAMCADNavigator::OnBnClickedRadioUNPLACEDPROBES
*/
void CAMCADNavigator::OnBnClickedRadioUnplacedProbes()
{
   if (docNotPCB)
      return;

   if (!m_doc)
      return;

   m_list.SetSel(-1);

   clearHighlights();

   GetDlgItem(IDC_SHOW_SELECTED)->EnableWindow(TRUE);

   m_doc->UnselectAll(FALSE);
   UnHighLightNets();
   m_list.ResetContent();
   m_compMode = navigatorModeUnplacedProbes;
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);   
   FillUnplacedProbes(m_curFile);
   UpdateCountsDisplay();
}

/**********************************************88
* CAMCADNavigator::FillPlacedProbes
*/
void CAMCADNavigator::FillPlacedProbes(FileStruct *pcbFile)
{
   if (!pcbFile || !m_doc)
      return;

   m_tree.ShowWindow(SW_HIDE);
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   m_list.ResetContent();
   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();

   placedProbesCount = 0;
   
   CMapStringToPtr temp;

   BlockStruct *block = pcbFile->getBlock();

   // Case 2203 related.
   // Some importers do not create consistent data, they make a file with
   // block type PCB but the block itself is not type PCB. This is technically
   // an importer error, but we'll roll and go with it.

   if (block != NULL && 
      (block->getBlockType() == blockTypePcb || pcbFile->getBlockType() == blockTypePcb))
   {
      POSITION dataPos = block->getDataList().GetHeadPosition();

      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

			if (data == NULL || !data->isInsertType(insertTypeTestProbe))
            continue;

         Attrib* attribPtr;

         if(!data->getAttributesRef() || !data->getAttributesRef()->Lookup(PROBE_PLACEMENT, attribPtr))
            continue;
         
         Attrib *attrib = get_attvalue(data->getAttributesRef(), PROBE_PLACEMENT);

         if (attrib->getStringValue().CompareNoCase("Placed"))
            continue;

         CString refName = data->getInsert()->getRefname();

         attrib = NULL;
         CString netName = "";
         attrib = is_attvalue(m_doc, data->getAttributesRef(), ATT_NETNAME, 1);

         if (attrib)
            netName.Format("(%s)", attrib->getStringValue());

         CString itemText;
         itemText = refName + " " + netName;

         /*m_list.SetItemData(m_list.AddString(itemText), (DWORD)data);*/
         placedProbesCount++;
         SortListHelper *helper = new SortListHelper;
         helper->voidptr = data;
         helper->typeTag = dataTypeInsert;
         temp.SetAt(itemText, (void*)helper);
      }
   }

   SortList(temp);
   m_tree.ShowWindow(SW_SHOW);
}

/**********************************************88
* CAMCADNavigator::FillUnplacedProbes
*/
void CAMCADNavigator::FillUnplacedProbes(FileStruct *pcbFile)
{
   if (!pcbFile || !m_doc)
      return;

   m_tree.ShowWindow(SW_HIDE);
   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   m_list.ResetContent();
   DeleteTreeItemDatasMem();
   m_tree.DeleteAllItems();

   unplacedProbesCount = 0;

   CMapStringToPtr temp;

   BlockStruct *block = pcbFile->getBlock();

   // Case 2203 related.
   // Some importers do not create consistent data, they make a file with
   // block type PCB but the block itself is not type PCB. This is technically
   // an importer error, but we'll roll and go with it.

   if (block != NULL && 
      (block->getBlockType() == blockTypePcb || pcbFile->getBlockType() == blockTypePcb))
   {
      POSITION dataPos = block->getDataList().GetHeadPosition();

      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

			if (data == NULL || !data->isInsertType(insertTypeTestProbe))
            continue;

         Attrib* attribPtr;

         if(!data->getAttributesRef() || !data->getAttributesRef()->Lookup(PROBE_PLACEMENT, attribPtr))
            continue;
         
         Attrib *attrib = get_attvalue(data->getAttributesRef(), PROBE_PLACEMENT);

         if (attrib->getStringValue().CompareNoCase("Unplaced"))
            continue;
         
         CString refName = data->getInsert()->getRefname();

         attrib = NULL;
         CString netName = "";
         attrib = is_attvalue(m_doc, data->getAttributesRef(), ATT_NETNAME, 1);

         if (attrib)
            netName.Format("(%s)", attrib->getStringValue());

         CString itemText;
         itemText = refName + " " + netName;

         /*m_list.SetItemData(m_list.AddString(itemText), (DWORD)data);*/
         unplacedProbesCount++;
         SortListHelper *helper = new SortListHelper;
         helper->voidptr = data;
         helper->typeTag = dataTypeInsert;
         temp.SetAt(itemText, (void*)helper);
      }
   }

   SortList(temp);
   m_tree.ShowWindow(SW_SHOW);
}
/*void CAMCADNavigator::OnTvnBegindragTreeNavigator(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   // TODO: Add your control notification handler code here
   *pResult = 0;

   pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   *pResult = 0;

   m_hitemDrag = pNMTreeView->itemNew.hItem;
   m_hitemDrop = NULL;

   m_pDragImage = m_tree.CreateDragImage(m_hitemDrag);  // get the image list for dragging
   // CreateDragImage() returns NULL if no image list
   // associated with the tree view control
   if( !m_pDragImage )
      return;

   m_bLDragging = TRUE;
   m_pDragImage->BeginDrag(0, CPoint(-15,-15));
   POINT pt = pNMTreeView->ptDrag;
   ClientToScreen( &pt );
   m_pDragImage->DragEnter(NULL, pt);
   SetCapture();

}

void CAMCADNavigator::OnNMRclickTreeNavigator(NMHDR *pNMHDR, LRESULT *pResult)
{
   // TODO: Add your control notification handler code here
   *pResult = 0;
}*/

/**********************************************88
* CAMCADNavigator::OnQueryDragIcon
The system calls this to obtain the cursor to display while the user drags
the minimized window.
*/
HCURSOR CAMCADNavigator::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

/**********************************************88
* CAMCADNavigator::OnTvnBegindragTreeNavigator
*/
void CAMCADNavigator::OnTvnBegindragTreeNavigator(NMHDR* pNMHDR, LRESULT* pResult) 
{
  LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)pNMHDR;
  *pResult = 0;   // allow drag

  if (!IsDropSource(lpnmtv->itemNew.hItem))
     return;

  CImageList* piml = NULL;    /* handle of image list */
  POINT ptOffset;
  RECT rcItem;
  
  if ((piml = m_tree.CreateDragImage(lpnmtv->itemNew.hItem)) == NULL)
    return;

  /* get the bounding rectangle of the item being dragged (rel to top-left of control) */
  if (m_tree.GetItemRect(lpnmtv->itemNew.hItem, &rcItem, TRUE))
  {
    CPoint ptDragBegin;
    int nX, nY;
    /* get offset into image that the mouse is at */
    /* item rect doesn't include the image */
    ptDragBegin = lpnmtv->ptDrag;
    ImageList_GetIconSize(piml->GetSafeHandle(), &nX, &nY);
    ptOffset.x = (ptDragBegin.x - rcItem.left) + (nX - (rcItem.right - rcItem.left));
    ptOffset.y = (ptDragBegin.y - rcItem.top) + (nY - (rcItem.bottom - rcItem.top));
    /* convert the item rect to screen co-ords, for use later */
    MapWindowPoints(NULL, &rcItem);
  }
  else
  {
    GetWindowRect(&rcItem);
    ptOffset.x = ptOffset.y = 8;
  }

  BOOL bDragBegun = piml->BeginDrag(0, ptOffset);

  if (! bDragBegun)
  {
    delete piml;

    return;
  }

  CPoint ptDragEnter = lpnmtv->ptDrag;
  ClientToScreen(&ptDragEnter);

  if (!piml->DragEnter(NULL, ptDragEnter))
  {
    delete piml;

    return;
  }

  delete piml;

  /* set the focus here, so we get a WM_CANCELMODE if needed */
  SetFocus();

  /* redraw item being dragged, otherwise it remains (looking) selected */
  InvalidateRect(&rcItem, TRUE);
  UpdateWindow();

  /* Hide the mouse cursor, and direct mouse input to this window */
  SetCapture(); 
  m_hItemDrag = lpnmtv->itemNew.hItem;

  // Set up the timer
   m_nTimerID = SetTimer(1, 75, NULL);
}

/**********************************************88
* CAMCADNavigator::OnMouseMove
*/
void CAMCADNavigator::OnMouseMove(UINT nFlags, CPoint point) 
{
  if (m_hItemDrag != NULL)
  {
    CPoint pt;

    /* drag the item to the current position */
    pt = point;
    ClientToScreen(&pt);

    CImageList::DragMove(pt);
    CImageList::DragShowNolock(FALSE);

    if (CWnd::WindowFromPoint(pt) != &m_tree)
      SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
    else
    {
      SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      TVHITTESTINFO tvhti;
      tvhti.pt = pt;
      m_tree.ScreenToClient(&tvhti.pt);
      HTREEITEM hItemSel = m_tree.HitTest(&tvhti);
      m_tree.SelectDropTarget(tvhti.hItem);
    }

    CImageList::DragShowNolock(TRUE);
  }

  CDialogBar::OnMouseMove(nFlags, point);
}

/**********************************************88
* CAMCADNavigator::OnCancelMode
*/
void CAMCADNavigator::OnCancelMode() 
{
  /* stop dragging */
  FinishDragging(TRUE);
  // redraw
  RedrawWindow();

  CDialogBar::OnCancelMode();
}

/**********************************************88
* CAMCADNavigator::OnLButtonUp
*/
void CAMCADNavigator::OnLButtonUp(UINT nFlags, CPoint point) 
{
  if (m_hItemDrag != NULL)
    OnEndDrag(nFlags, point);
  else
    CDialogBar::OnLButtonUp(nFlags, point);
}

// copy our lParam, which happens to be a CString pointer
static LPARAM CopyData(const CTreeCtrl& tree, HTREEITEM hItem, LPARAM lParam)
{
  if (lParam == 0)
    return 0;

  CString* ps = (CString*)lParam;
  CString* psNew = new CString(*ps);

  return (LPARAM)psNew;
}

/**********************************************88
* CAMCADNavigator::OnEndDrag
*/
void CAMCADNavigator::OnEndDrag(UINT nFlags, CPoint point)
{
  if (m_hItemDrag == NULL || !IsDropSource(m_hItemDrag))
  {
     FinishDragging(FALSE);

     return;
  }

  CPoint pt;

  pt = point;
  ClientToScreen(&pt);

  BOOL bCopy = (GetKeyState(VK_CONTROL) & 0x10000000);

  // do drop
  HTREEITEM hItemDrop = m_tree.GetDropHilightItem();

  if (hItemDrop != NULL)
  {
    TRACE("Dropped item %s (%s) onto item %s (%s)\n", m_tree.GetItemText(m_hItemDrag), *((CString*)m_tree.GetItemData(m_hItemDrag)), 
           m_tree.GetItemText(hItemDrop), *((CString*)m_tree.GetItemData(hItemDrop)));
  }
  else
  {
    TRACE("Dropped item %s (%s) onto empty space\n", m_tree.GetItemText(m_hItemDrag), *((CString*)m_tree.GetItemData(m_hItemDrag)));
  }

  m_tree.SelectDropTarget(NULL);
  //MoveTreeItem(m_tree, m_hItemDrag, hItemDrop, bCopy, CopyData);

  FinishDragging(TRUE);

  RedrawWindow();
}

//**********************************************

CString CAMCADNavigator::GetWhoAndWhen()
{
   // Who is doing this, and when
   CString userName("Unknown");
   char acUserName[100];
   DWORD nUserName = sizeof(acUserName);
   if (GetUserName(acUserName, &nUserName))
      userName = acUserName;
   CString UserNameTimeStamp;
   CTime dateTime = CTime::GetCurrentTime();
   UserNameTimeStamp.Format("%s - %s", userName, dateTime.Format("%d %b %Y %I:%M:%S %p"));  // e.g. 12 Sep 1980 10:40:13 PM

   return UserNameTimeStamp;
}

/**********************************************
* CAMCADNavigator::FinishDragging
*/
void CAMCADNavigator::FinishDragging(BOOL bDraggingImageList)
{
  if (m_hItemDrag != NULL)
  {
    if (bDraggingImageList)
    {
      CImageList::DragLeave(NULL);
      CImageList::EndDrag();
    }

    ReleaseCapture();
    ShowCursor(TRUE);
    m_hItemDrag = NULL;
    m_tree.SelectDropTarget(NULL);
  }

  KillTimer( m_nTimerID );
}

/**********************************************88
* CAMCADNavigator::IsDropSource
*/
BOOL CAMCADNavigator::IsDropSource(HTREEITEM hItem)
{
   int nImage, nImageSel;
   m_tree.GetItemImage(hItem, nImage, nImageSel);

   if (nImage == ICON_PLCD || nImage == ICON_UNPLCD)
        return TRUE;

   return FALSE;
}

/**********************************************88
* CAMCADNavigator::UpdateProbes
*/
void CAMCADNavigator::UpdateProbes(CCEtoODBDoc *document)
{
   placedProbesCount = unplacedProbesCount = 0;

   if (!document)
      return;

   FileStruct *curFile = document->getFileList().GetOnlyShown(blockTypePcb,blockTypePanel);
   if (!curFile)
      return;

   BlockStruct *block = curFile->getBlock();

     // Case 2203 related.
   // Some importers do not create consistent data, they make a file with
   // block type PCB but the block itself is not type PCB. This is technically
   // an importer error, but we'll roll and go with it.

   if (block != NULL && 
      (block->getBlockType() == blockTypePcb || curFile->getBlockType() == blockTypePcb))
   {
      POSITION dataPos = block->getDataList().GetHeadPosition();

      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

			if (data == NULL || !data->isInsertType(insertTypeTestProbe))
            continue;

         Attrib* attribPtr;

         if(!data->getAttributesRef() || !data->getAttributesRef()->Lookup(PROBE_PLACEMENT, attribPtr))
            continue;
         
         Attrib *attrib = get_attvalue(data->getAttributesRef(), PROBE_PLACEMENT);

         if (!attrib->getStringValue().CompareNoCase("Placed"))
         {
            placedProbesCount++;

            if (m_compMode == navigatorModePlacedProbes)
            {
               CString refName = data->getInsert()->getRefname();

               attrib = NULL;
               CString netName = "";
               attrib = is_attvalue(document, data->getAttributesRef(), ATT_NETNAME, 1);

               if (attrib)
                  netName.Format("(%s)", attrib->getStringValue());

               CString itemText;
               itemText = refName + " " + netName;

               if (m_list.FindString(0, itemText) == -1)
                  m_list.SetItemData(m_list.AddString(itemText), (DWORD)data);
            }
         }
         else if (!attrib->getStringValue().CompareNoCase("Unplaced"))
         {
            unplacedProbesCount++;

            if (m_compMode == navigatorModeUnplacedProbes)
            {
               CString refName = data->getInsert()->getRefname();

               attrib = NULL;
               CString netName = "";
               attrib = is_attvalue(document, data->getAttributesRef(), ATT_NETNAME, 1);

               if (attrib)
                  netName.Format("(%s)", attrib->getStringValue());

               CString itemText;
               itemText = refName + " " + netName;
               
               if (m_list.FindString(0, itemText) == -1)
                  m_list.SetItemData(m_list.AddString(itemText), (DWORD)data);
            }
         }
      }
   }

   if (m_compMode == navigatorModePlacedProbes)
   {
      if (!placedProbesCount)
         m_list.ResetContent();
   }
   else if (m_compMode == navigatorModeUnplacedProbes)
   {
      if (!unplacedProbesCount)
         m_list.ResetContent();
   }

   CString placedProbesCountStr;
   CString unplacedProbesCountStr;
   placedProbesCountStr.Format("(%d)", placedProbesCount);
   unplacedProbesCountStr.Format("(%d)", unplacedProbesCount);
   GetDlgItem(IDC_STATIC_PLCDCNT)->SetWindowText(placedProbesCountStr);
   GetDlgItem(IDC_STATIC_UNPLCDCNT)->SetWindowText(unplacedProbesCountStr);
}

/**********************************************88
* CAMCADNavigator::OnNMRclickTreeNavigator
*/
void CAMCADNavigator::OnNMRclickTreeNavigator(NMHDR *pNMHDR, LRESULT *pResult)
{
   DWORD m_MousePosInt = GetMessagePos();
   CPoint m_MousePos((m_MousePosInt) & 0xFFFF, (m_MousePosInt >> 16) & 0xFFFF);
   m_tree.ScreenToClient(&m_MousePos);

   UINT flags = 0;
   m_RightClickItem = m_tree.HitTest(m_MousePos, &flags);
   
   if (m_RightClickItem == NULL)
      return;

   int nImage, nImageSel;
   m_tree.GetItemImage(m_RightClickItem, nImage, nImageSel);

   CMenu tmpMenu; 

   if (nImage == ICON_PLCD || nImage == ICON_UNPLCD)
   {
      tmpMenu.LoadMenu(IDR_EXP_PROBE);
   }
   else if (nImage == ICON_VIA || nImage == ICON_CP || nImage == ICON_NET || nImage == ICON_PIN)
   {
      tmpMenu.LoadMenu(IDR_EXP_NON_PROBE);
   }
   else
   {
      *pResult = 0;

      return;
   }

   m_tree.SelectItem(m_RightClickItem);

   DWORD Result = GetMessagePos();
   CPoint ItemPos(GET_X_LPARAM(Result), GET_Y_LPARAM(Result));

   CMenu* pPopup = tmpMenu.GetSubMenu(0);
   ASSERT(pPopup != NULL);

   if(tmpMenu.GetSafeHmenu( ))
      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                     ItemPos.x, ItemPos.y,
                     AfxGetMainWnd());

   *pResult = 0;
}

/**********************************************88
* CAMCADNavigator::OnCmdDeleteProbe
*/
void CAMCADNavigator::OnCmdDeleteProbe()
{
   HTREEITEM selProbeItem = m_tree.GetSelectedItem(); 

   if (!selProbeItem)
      return;

   TreeItemData *probeItemData = (TreeItemData*)m_tree.GetItemData(selProbeItem);
   DataStruct* selProbeData = (DataStruct*)probeItemData->voidPtr;

   POSITION probePos = m_placedProbes.Find(selProbeData);

   if (probePos != NULL)
   {
      m_placedProbes.RemoveAt(probePos);
   }

   probePos = m_unplacedProbes.Find(selProbeData);

   if (probePos != NULL)
   {
      m_unplacedProbes.RemoveAt(probePos);
   }

   //for (int i=0; i<navPlacedProbeArrayCount; i++)
   //{
   //   if (((DataStruct*)navPlacedProbeArray.GetAt(i)) == selProbeData)
   //      navPlacedProbeArray.SetAt(i, NULL);
   //}

   //for (int i=0; i<navUnplacedProbeArrayCount; i++)
   //{
   //   if (((DataStruct*)navUnplacedProbeArray.GetAt(i)) == selProbeData)
   //      navUnplacedProbeArray.SetAt(i, NULL);
   //}

   //m_curFile->getBlock()->RemoveDataFromList(m_doc, selProbeData);
   m_doc->removeDataFromDataList(*(m_curFile->getBlock()),selProbeData);

   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   m_tree.DeleteItem(selProbeItem);
   m_doc->UpdateAllViews(NULL);

   m_list.SetSel(-1, 0);
   int nIndex = 0;

   while (nIndex < m_list.GetCount())
   {
      if ((DataStruct*)m_list.GetItemData(nIndex) == selProbeData)
      {
         DeleteTreeItemDatasMem();
         m_tree.DeleteAllItems();
         m_list.DeleteString(nIndex);

         break;
      }

      nIndex++;
   }

   m_list.SetSel(-1, 0);
   UpdateProbes(m_doc);
}

/**********************************************88
* CAMCADNavigator::OnCmdUnplaceProbe
*/
void CAMCADNavigator::OnCmdUnplaceProbe()
{
   HTREEITEM selProbeItem = m_tree.GetSelectedItem(); 

   if (!selProbeItem)
      return;

   int nImage, nSelImage;
   m_tree.GetItemImage(selProbeItem, nImage, nSelImage);

   if (nImage != ICON_PLCD)
      return;

   TreeItemData* probeItemData = (TreeItemData*)m_tree.GetItemData(selProbeItem);
   DataStruct* probeData = (DataStruct*)probeItemData->voidPtr;

   HTREEITEM netItem = m_tree.GetParentItem(m_tree.GetParentItem(selProbeItem));
   TreeItemData *netItemData = (TreeItemData*)netItem;
   
   HTREEITEM probeItem = m_tree.InsertItem(probeData->getInsert()->getRefname(), ICON_UNPLCD, ICON_UNPLCD, netItem, TVI_FIRST);
   SetTreeItemData(&m_tree, probeItem, probeData->getInsert()->getRefname(), ICON_UNPLCD, probeData);

   m_tree.SelectItem(NULL);
   m_tree.SelectDropTarget(NULL);
   m_tree.DeleteItem(selProbeItem);

   //unplace it
   void* voidPtr = (void*)"Unplaced";
   probeData->setAttrib(getCamCadData(), PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);

   probeData->getAttributesRef()->deleteAttribute(DATALINK);
   probeData->getInsert()->setOrigin(0.,0.);
   UpdateProbes(m_doc);
   m_doc->UpdateAllViews(NULL);
}

/**********************************************88
* CAMCADNavigator::OnCreateProbe
*/
/*void CAMCADNavigator::OnCreateProbe()
{
   int nImage, nImageSel;
   HTREEITEM hItemSel = m_tree.GetSelectedItem();

   m_tree.GetItemImage(hItemSel, nImage, nImageSel);
   Attrib *attrib = NULL;
   CompPinStruct *cp = NULL;
   DataStruct* via = NULL;
   long entityNum = -1;
   CString netName = "";
   
   double x,y, rotRadians;
   int mirror;

   //get largest probe name
   CString probeName = "";
   int probeNum = -1;

   for (int i=0; i< m_doc->getMaxBlockIndex(); i++)
   {
      BlockStruct* block = m_doc->getBlockAt(i);

      if (!block)
         continue;

      POSITION dataPos = block->getHeadDataInsertPosition();

      while (dataPos)
      {
         DataStruct* data = block->getNextDataInsert(dataPos);

			if (data == NULL || !data->isInsertType(insertTypeTestProbe))
            continue;
         
         int tempProbeNum = atoi(data->getInsert()->getRefname());

         if (tempProbeNum > probeNum)
            probeNum = tempProbeNum;
      }
   }

   probeName.Format("%d",++probeNum);

   if (nImage == ICON_CP)
   {
      TreeItemData* itemData = (TreeItemData*)m_tree.GetItemData(hItemSel);
      cp = (CompPinStruct*)itemData->voidPtr;
      //get the node properties to which we wish to add the probe
      x = cp->getOrigin().x;
      y = cp->getOrigin().y;
      rotRadians = cp->getRotationRadians();
      mirror = cp->getMirror();
      TreeItemData* netItemData = (TreeItemData*)m_tree.GetItemData(m_tree.GetParentItem(hItemSel));
      netName = ((NetStruct*)netItemData->voidPtr)->getNetName();
   }
   else if (nImage == ICON_PIN)
   {
      TreeItemData* itemData = (TreeItemData*)m_tree.GetItemData(m_tree.GetParentItem(hItemSel));
      DataStruct* component = (DataStruct*)itemData->voidPtr;

      itemData = (TreeItemData*)m_tree.GetItemData(hItemSel);
      DataStruct* pin = (DataStruct*)itemData->voidPtr;

      CString pinRef = component->getInsert()->getRefname() + "." + pin->getInsert()->getRefname();

      CCamCadPin* camCadPin = getCamCadPin(pinRef);

      if (camCadPin != NULL)
      {
         cp = camCadPin->getCompPin();

         x = cp->getOrigin().x;
         y = cp->getOrigin().y;
         rotRadians = cp->getRotationRadians();
         mirror = cp->getMirror();
         netName = camCadPin->getNet()->getNetName();
      }
   }
   else if (nImage == ICON_VIA)
   {
      TreeItemData* itemData = (TreeItemData*)m_tree.GetItemData(hItemSel);
      via = (DataStruct*)itemData->voidPtr;
      x = via->getInsert()->getOrigin().x;
      y = via->getInsert()->getOrigin().y;
      rotRadians = via->getInsert()->getAngleRadians();
      mirror = via->getInsert()->getGraphicMirrored();
      TreeItemData* netItemData = (TreeItemData*)m_tree.GetItemData(m_tree.GetParentItem(hItemSel));
      netName = ((NetStruct*)netItemData->voidPtr)->getNetName();
   }
   else if (nImage == ICON_NET)
   {
      TreeItemData* netItemData = (TreeItemData*)m_tree.GetItemData(hItemSel);
      netName = ((NetStruct*)netItemData->voidPtr)->getNetName();

      //create the probe geometry
      if (getDoc()->GetCurrentDFTSolution(*m_curFile) == NULL || getDoc()->GetCurrentDFTSolution(*m_curFile)->GetTestPlan() == NULL)
      {
         ErrorMessage("There is no current DFT Solution. Cannot create Probe.", "", MB_OK);
         return;
      }

      POSITION pos = getDoc()->GetCurrentDFTSolution(*m_curFile)->GetTestPlan()->GetProbes().GetHeadPosition_TopProbes();
      
      CDFTProbeTemplate *pTemplate = getDoc()->GetCurrentDFTSolution(*m_curFile)->GetTestPlan()->GetProbes().GetLargest_UsedTopProbes(pos);

      if (!pTemplate)
         return;

      const char* sizeName = pTemplate->GetName();
      
      BlockStruct *probeBlock = CreateTestProbeGeometry(m_doc, pTemplate->GetName()+"_Top", pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()),
         pTemplate->GetName()+"_Top", pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()));         

      //create the probe data and add it to the board
      m_doc->PrepareAddEntity(m_curFile);
      DataStruct *probeData = Graph_Block_Reference(probeBlock->getName(), probeName, probeBlock->getFileNumber(), 0, 0, 0, 0, 1.0, -1, FALSE);
      probeData->getInsert()->setInsertType(insertTypeTestProbe);

      //unplaced it
      void* voidPtr = (void*)"Unplaced";
      probeData->setAttrib(getCamCadData(), PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);

      probeData->getAttributesRef()->deleteAttribute(DATALINK);
      probeData->getInsert()->setOrigin(0.,0.);

      //add it to the tree
      HTREEITEM probeItem = m_tree.InsertItem(probeName, ICON_UNPLCD, ICON_UNPLCD, hItemSel, TVI_FIRST);
      SetTreeItemData(&m_tree, probeItem, probeName, ICON_UNPLCD, probeData);

      m_doc->SetAttrib(&probeData->getAttributesRef(), m_doc->IsKeyWord(ATT_NETNAME,0), netName, attributeUpdateOverwrite, NULL);  

      //update nevigator
      UpdateProbes(m_doc);
      m_doc->UpdateAllViews(NULL);

      return;
   }
   
   //create the probe geometry
   if (getDoc()->GetCurrentDFTSolution(*m_curFile) == NULL || getDoc()->GetCurrentDFTSolution(*m_curFile)->GetTestPlan() == NULL)
   {
      ErrorMessage("There is no current DFT Solution. Cannot create Probe.", "CAMCAD", MB_OK);

      return;
   }

   POSITION pos = getDoc()->GetCurrentDFTSolution(*m_curFile)->GetTestPlan()->GetProbes().GetHeadPosition_TopProbes();
   CDFTProbeTemplate *pTemplate = NULL;

   if (pos)
      pTemplate = getDoc()->GetCurrentDFTSolution(*m_curFile)->GetTestPlan()->GetProbes().GetLargest_UsedTopProbes(pos);
   else
      return;

   if (!pTemplate)
      return;

   const char* sizeName = pTemplate->GetName();
   
   BlockStruct *probeBlock = CreateTestProbeGeometry(m_doc, pTemplate->GetName()+"_Top", pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()),
      pTemplate->GetName()+"_Top", pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()));         

   //create the probe data and add it to the board
   m_doc->PrepareAddEntity(m_curFile);
   DataStruct *probeData = Graph_Block_Reference(probeBlock->getName(), probeName, probeBlock->getFileNumber(), x, y, rotRadians, mirror, 1.0, -1, FALSE);
   probeData->getInsert()->setInsertType(insertTypeTestProbe);

   POSITION dataPos = m_curFile->getBlock()->getDataList().GetHeadPosition();

   while (dataPos)
   {
      DataStruct* accessData = m_curFile->getBlock()->getDataList().GetNext(dataPos);

		if (accessData == NULL || !accessData->isInsertType(insertTypeTestAccessPoint))
         continue;

      attrib = is_attvalue(m_doc, accessData->getAttributesRef(), ATT_DDLINK, 1);

      if (!attrib)
         continue;

      if (cp)
      {
         if (attrib && attrib->getIntValue() == cp->getEntityNumber())
         {
            entityNum = accessData->getEntityNumber();
            probeData->getInsert()->setOrigin(accessData->getInsert()->getOrigin());

            break;
         }
      }
      else if (via)
      {
         if (attrib && attrib->getIntValue() == via->getEntityNumber())
         {
            entityNum = accessData->getEntityNumber();
            probeData->getInsert()->setOrigin(accessData->getInsert()->getOrigin());

            break;
         }
      }
   }

   if (entityNum != -1) //access point available
   {
      void* voidPtr = (void*)"Placed";
      probeData->setAttrib(getCamCadData(), PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
      probeData->setHidden(false);
      //voidPtr = (void*)&entityNum;
      probeData->setAttrib(getCamCadData(), DATALINK, valueTypeInteger, (void*)&entityNum, attributeUpdateOverwrite, NULL);
      HTREEITEM probeItem = m_tree.InsertItem(probeName, ICON_PLCD, ICON_PLCD, hItemSel, TVI_FIRST);
      SetTreeItemData(&m_tree, probeItem, probeName, ICON_PLCD, probeData);
   }
   else //no access point available, create one
   {
		DataStruct *access = NULL;

      if (cp)
      {
         access = PlaceTestAccessPoint(m_doc, m_curFile->getBlock(), "", cp->getOriginX(), cp->getOriginY(), 
            probeData->getInsert()->getGraphicMirrored()?testSurfaceBottom:testSurfaceTop,netName, "SMD",cp->getEntityNumber(), 0,m_doc->getSettings().getPageUnits()); 
         void* voidPtr = (void*)"Placed";
         probeData->setAttrib(getCamCadData(), PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
         probeData->setHidden(false);
         //voidPtr = (void*)&entityNum;
         entityNum = access->getEntityNumber();
         probeData->setAttrib(getCamCadData(), DATALINK, valueTypeInteger, (void*)&entityNum, attributeUpdateOverwrite, NULL);
         probeData->getInsert()->setOrigin(access->getInsert()->getOrigin());
         HTREEITEM probeItem = m_tree.InsertItem(probeName, ICON_PLCD, ICON_PLCD, hItemSel, TVI_FIRST);
         SetTreeItemData(&m_tree, probeItem, probeName, ICON_PLCD, probeData);
      }
      else if (via)
      {
         access = PlaceTestAccessPoint(m_doc, m_curFile->getBlock(), "", via->getInsert()->getOriginX(), via->getInsert()->getOriginY(), 
            probeData->getInsert()->getGraphicMirrored()?testSurfaceBottom:testSurfaceTop,netName, "SMD",via->getEntityNumber(), 0, m_doc->getSettings().getPageUnits()); 
         void* voidPtr = (void*)"Placed";
         probeData->setAttrib(getCamCadData(), PROBE_PLACEMENT, valueTypeString, voidPtr, attributeUpdateOverwrite, NULL);
         probeData->setHidden(false);
         //voidPtr = (void*)&entityNum;
         entityNum = access->getEntityNumber();
         probeData->setAttrib(getCamCadData(), DATALINK, valueTypeInteger, (void*)&entityNum, attributeUpdateOverwrite, NULL);
         probeData->getInsert()->setOrigin(access->getInsert()->getOrigin());
         HTREEITEM probeItem = m_tree.InsertItem(probeName, ICON_PLCD, ICON_PLCD, hItemSel, TVI_FIRST);
         SetTreeItemData(&m_tree, probeItem, probeName, ICON_PLCD, probeData);
      }

		// Case 1773, Add new Access Point to DFT solution
		// It is a maverick access point, brought into being by the whim of the user.
		// No proper access analysis was applied by CAMCAD.
		if (access != NULL)
		{
			CDFTSolution *curSolution = getDoc()->GetCurrentDFTSolution(*m_curFile);
			if (curSolution != NULL)
			{
				CAccessAnalysisSolution *aaSol = curSolution->GetAccessAnalysisSolution();
				if (aaSol != NULL)
				{
					CAANetAccess* netAccess = aaSol->GetNetAccess(netName);
					if (netAccess != NULL)
					{
						netAccess->AddMaverick(access);
					}
				}
			}
		}

   }

   //case 998: did not add the netname attribute to the probe when creating it, so when moving it, 
   //the move routine tries to verify it is on the same net, but it has not net, thus not moving it.
   m_doc->SetAttrib(&probeData->getAttributesRef(), m_doc->IsKeyWord(ATT_NETNAME,0), netName, attributeUpdateOverwrite, NULL);  
   //update nevigator
	CountNetsWithAccess(m_doc);
   UpdateProbes(m_doc);

   m_doc->UpdateAllViews(NULL);
}
*/
/**********************************************88
* CAMCADNavigator::OnEditProbe
*/
void CAMCADNavigator::OnEditProbe()
{
   CEditProbe editProbeDlg(m_doc);
   editProbeDlg.DoModal();
}

/******************************************************************************
* CAMCADNavigator::SetTreeDataItem
*/
void CAMCADNavigator::SetTreeItemData(CTreeCtrl *treeview, HTREEITEM treeItem, CString itemText, int itemType, void *voidPtr)
{
   CString sortKey = "";
   CString numberString = "";
   int numberCount = 0;

   // (1) Creat the sort key
   for (int i=0; i<itemText.GetLength(); i++)
   {
      if (int(itemText[i]) >= 48 && int(itemText[i]) <= 57)
      {
         // The character is a number
         numberCount++;
         numberString.AppendChar(itemText[i]);
      }
      else 
      {
         // The character is NOT a number
         if (numberCount > 0)
         {
            // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
            while (numberCount < SCH_NUM_PATCH_LENGTH)
            {
               numberString.Insert(0, "0");
               numberCount++;
            }

            // Append the number to the sortKey
            sortKey.Append(numberString);
         }

         // Append the character to the sortKey
         sortKey.AppendChar(itemText[i]);
         numberString = "";
         numberCount = 0;
      }
   }

   // Need to check of the end of the string is number
   if (numberCount > 0)
   {
      // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
      while (numberCount < SCH_NUM_PATCH_LENGTH)
      {
         numberString.Insert(0, "0");
         numberCount++;
      }

      // Append the number to the sortKey
      sortKey.Append(numberString);
   }

   // (2) Creat a new item data
   TreeItemData *itemData = new TreeItemData;
   itemData->itemType = itemType;
   itemData->voidPtr = voidPtr;
   itemData->sortKey = sortKey;

   // (3) Add item data to the tree
   treeview->SetItemData(treeItem, (DWORD)itemData);
}

/******************************************************************************
* CAMCADNavigator::SortList
*/
void CAMCADNavigator::SortList(CMapStringToPtr& tempList)
{
   CMapSortStringToOb listItemsMap(10, false);
   CString sortKey = "";
   CString numberString = "";
   int numberCount = 0;
   
   POSITION pos = tempList.GetStartPosition();

   while (pos)
   {
      CString itemText = "";
      void* voidPtr = NULL;
      tempList.GetNextAssoc(pos, itemText, voidPtr);

      for (int i=0; i<itemText.GetLength(); i++)
      {
         if (int(itemText[i]) >= 48 && int(itemText[i]) <= 57)
         {
            // The character is a number
            numberCount++;
            numberString.AppendChar(itemText[i]);
         }
         else 
         {
            // The character is NOT a number
            if (numberCount > 0)
            {
               // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
               while (numberCount < SCH_NUM_PATCH_LENGTH)
               {
                  numberString.Insert(0, " ");
                  numberCount++;
               }

               // Append the number to the sortKey
               sortKey.Append(numberString);
            }

            // Append the character to the sortKey
            sortKey.AppendChar(itemText[i]);
            numberString = "";
            numberCount = 0;
         }
      }

      // Need to check of the end of the string is number
      if (numberCount > 0)
      {
         // If previous characters were number, then patch the number is Zero up to the number in SCH_NUM_PATCH_LENGTH
         while (numberCount < SCH_NUM_PATCH_LENGTH)
         {
            numberString.Insert(0, " ");
            numberCount++;
         }

         // Append the number to the sortKey
         sortKey.Append(numberString);
      }

      listItemsMap.SetAt(sortKey, (CObject*&)itemText);
      numberString = "";
      numberCount = 0;
      sortKey = "";
   }

   listItemsMap.setSortFunction(&listSortFunc);
   listItemsMap.Sort();

   //fill the tree list
   CString *key = NULL;
   const char *itemText = NULL;
   int nIndex = 0;
   listItemsMap.GetFirstSorted(key, (CObject*&)itemText);

   bool showSide = this->GetShowSide();
   bool showType = this->GetShowType();
   WORD placedKw = m_doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);

   while (itemText != NULL)
   { 
      SortListHelper *helper = NULL;
      tempList.Lookup(itemText, (void*&)helper);

      CString displayText(itemText);
      if (helper->typeTag == dataTypeInsert)
      {
         DataStruct *data = (DataStruct*)helper->voidptr;

         bool reallyShowSide = showSide;
         if (data->isInsertType(insertTypeTestProbe))
         {
            // Only placed probes show side
            Attrib* attrib = NULL;
            if (data->getAttributes() != NULL &&
               data->getAttributes()->Lookup(placedKw, attrib) && attrib != NULL)
            {
               if (attrib->getStringValue().CompareNoCase("Placed") != 0)
                  reallyShowSide = false;
            }
         }

         if (reallyShowSide)
         {
            CString side = data->getInsert()->getPlacedTop() ? "Top" : "Bottom";
            displayText += "  (" + side + ")";
         }

         // The insertTypeDie is purposely left out of test here. The PCBComponent and the TestProbe
         // are the "normal" things for the list, and do not need special labels. The Die parts (or
         // anything else that comes along) are different from the norm, so let this code apply
         // the type label for them, so they stand out for user to see.
         if (!data->isInsertType(insertTypePcbComponent) &&
            !data->isInsertType(insertTypeTestProbe))
         {
            CString insertTypeStr = insertTypeToDisplayString(data->getInsert()->getInsertType());
            if (!insertTypeStr.IsEmpty())
               displayText += "  (" + insertTypeStr + ") ";

         }

      }

      m_list.InsertString(nIndex, displayText);
      
      m_list.SetItemData(nIndex++, (DWORD)helper->voidptr);
      listItemsMap.GetNextSorted(key, (CObject*&)itemText);
   }

   POSITION delpos = tempList.GetStartPosition();
   while (delpos)
   {
      CString key;
      SortListHelper *helper = NULL;
      tempList.GetNextAssoc(delpos, key, (void*&)helper);
      if (helper != NULL)
         delete helper;
      tempList.SetAt(key, NULL);
   }

   listItemsMap.RemoveAll();
}

/******************************************************************************
* CAMCADNavigator::OnBnClickedBtnOptions
*/
void CAMCADNavigator::OnBnClickedBtnOptions()
{
   bool currentShowSide = this->GetShowSide();

   CNavigatorOptions optionsDlg(m_doc);
   optionsDlg.DoModal();

   // If setting changed then update the Navigator display
   if (currentShowSide != this->GetShowSide())
   {
      if (m_compMode == navigatorModeComponents)
         OnBnClickedCompMode();
      else if (m_compMode == navigatorModePlacedProbes)
         OnBnClickedRadioPlacedProbes();
      else
      {
         m_compMode = navigatorModeComponents;
         UpdateData(FALSE);
         OnBnClickedCompMode();
      }
   }
}

/******************************************************************************
* CAMCADNavigator::OnTimer
*/
void CAMCADNavigator::OnTimer(UINT nIDEvent) 
{
   if (nIDEvent != m_nTimerID )
   {
      CWnd::OnTimer(nIDEvent);
      return;
   }

   // Doesn't matter that we didn't initialize m_timerticks
   m_timerticks++;

   POINT pt;
   GetCursorPos( &pt );
   RECT rect;
   m_tree.GetClientRect( &rect );
   m_tree.ClientToScreen( &rect );

   // NOTE: Screen coordinate is being used because the call
   // to DragEnter had used the Desktop window.
   CImageList::DragMove(pt);

   HTREEITEM hitem = m_tree.GetFirstVisibleItem();

   if( pt.y < rect.top + 10 )
   {
      // We need to scroll up
      // Scroll slowly if cursor near the treeview control
      int slowscroll = 6 - (rect.top + 10 - pt.y) / 20;

      if( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
      {
         CImageList::DragShowNolock(FALSE);
         m_tree.SendMessage( WM_VSCROLL, SB_LINEUP);
         m_tree.SelectDropTarget(hitem);
         m_hitemDrop = hitem;
         CImageList::DragShowNolock(TRUE);
      }
   }
   else if( pt.y > rect.bottom - 10 )
   {
      // We need to scroll down
      // Scroll slowly if cursor near the treeview control
      int slowscroll = 6 - (pt.y - rect.bottom + 10 ) / 20;

      if( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
      {
         CImageList::DragShowNolock(FALSE);
         m_tree.SendMessage( WM_VSCROLL, SB_LINEDOWN);
         int nCount = m_tree.GetVisibleCount();

         for ( int i=0; i<nCount-1; ++i )
            hitem = m_tree.GetNextVisibleItem(hitem);

         if( hitem )
            m_tree.SelectDropTarget(hitem);

         m_hitemDrop = hitem;
         CImageList::DragShowNolock(TRUE);
      }
   }
}


/******************************************************************************
* CAMCADNavigator::DeleteTreeItemDatasMem
*/
void CAMCADNavigator::DeleteTreeItemDatasMem()
{
   HTREEITEM root = m_tree.GetRootItem();

   while (root)
   {
      TreeItemData *itemData = (TreeItemData*)m_tree.GetItemData(root);
            
      if (m_tree.ItemHasChildren(root))
      {
         HTREEITEM child = m_tree.GetChildItem(root);

         while (child)
         {
            TreeItemData *itemData = (TreeItemData*)m_tree.GetItemData(child);

            if (m_tree.ItemHasChildren(child))
            {
               HTREEITEM subChild = m_tree.GetChildItem(child);
               
               while (subChild)
               {
                  TreeItemData *itemData = (TreeItemData*)m_tree.GetItemData(subChild);
                  delete itemData;                 
                  itemData = NULL;
                  subChild = m_tree.GetNextSiblingItem(subChild);
               }
            }

            delete itemData;
            itemData = NULL;
            child = m_tree.GetNextSiblingItem(child);
         }
      }

      delete itemData;
      itemData = NULL;
      root = m_tree.GetNextSiblingItem(root);
   }
}

void CAMCADNavigator::enableAndFillVariantList()
{
	m_sVariantName = "";

   // Get current Variant name
   int keyword = m_doc->getStandardAttributeKeywordIndex(standardAttributeCurrentVariant);
   Attrib* attrib = NULL;
   if (m_curFile->getBlock() != NULL && m_curFile->getBlock()->lookUpAttrib(keyword, attrib))
   {
      m_sVariantName = attrib->getStringValue();
   }
     
   GetDlgItem(IDC_BUTTON_VARIANT_CHANGE)->EnableWindow(m_curFile != NULL && m_curFile->getVariantList().GetCount() > 0);
}

void CAMCADNavigator::enableAndFillMachineList()
{
   m_sMachineName = "";
   CMachine *machine = NULL;

   /*if (m_curFile != NULL &&
      m_doc->GetMachineCount(*m_curFile) > 0 &&
      m_doc->GetCurrentMachine(*m_curFile) != NULL)
   {
      machine = m_doc->GetCurrentMachine(*m_curFile);
      m_sMachineName = machine->GetName();      
   }*/

   UpdateData(FALSE);
}

void CAMCADNavigator::OnBnClickedButtonVariantChange()
{
   if (m_curFile->getVariantList().GetCount() < 1)
      return;

   CSelectDlg dlg("Select Variant", false, true);
   CVariant* variant = m_curFile->getVariantList().GetDefaultVariant();

   if (variant != NULL)
   {
      CVariantSelItem* variantSelItem = new CVariantSelItem(variant->GetName(), variant, false);      
      dlg.AddItem(variantSelItem);
   }

   POSITION pos = m_curFile->getVariantList().GetHeadPosition();

   while (pos != NULL)
   {
      variant = m_curFile->getVariantList().GetNext(pos);

      if (variant == NULL)
         continue;

      CVariantSelItem* variantSelItem = new CVariantSelItem(variant->GetName(), variant, false);      
      dlg.AddItem(variantSelItem);
   }

   if (dlg.DoModal() != IDOK)
      return;

   // Get the selected variant name
   variant = ((CVariantSelItem*)dlg.GetSelItemHead())->GetVariant();

   if (variant == NULL || variant->GetName().CompareNoCase(m_sVariantName) == 0)
      return;

   CString defaultVariantName;

   if (m_curFile->getVariantList().GetDefaultVariant() != NULL)
      defaultVariantName = m_curFile->getVariantList().GetDefaultVariant()->GetName();

   // Find the variant
   if (!defaultVariantName.IsEmpty() && variant->GetName().CompareNoCase(defaultVariantName) == 0)
      variant = m_curFile->getVariantList().GetDefaultVariant();

   // Apply the variant to the board
   if (variant != NULL)
   {
      POSITION pos = variant->GetStartPosition();

      while (pos != NULL)
      {
         CVariantItem* item = variant->GetNext(pos);

         if (item == NULL)
            continue;

         DataStruct* data = m_curFile->getBlock()->FindData(item->GetName());

         if (data == NULL)
            continue;

         if (data->getAttributes() == NULL)
         {
            data->getAttributesRef() = new CAttributes(*(item->getAttributes()));
         }
         else
         {
            int technologyKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeTechnology);
            int smdKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeSmd);
            int refnameKw = m_doc->getStandardAttributeKeywordIndex(standardAttributeRefName);
            CString technologyValue = "";
            bool isSMD = false;
            Attrib *refnameAttrib = NULL;

            // Save the TECHNOLOGY, SMD  and REFNAME attribute from the component
            Attrib* attrib = NULL;
            if (data->lookUpAttrib(technologyKw, attrib) && attrib)
            {
               technologyValue = attrib->getStringValue();
            }
            if (data->lookUpAttrib(smdKw, attrib) && attrib)
            {
               isSMD = true;
            }
            if (data->lookUpAttrib(refnameKw, attrib) && attrib)
            {
               refnameAttrib = attrib->allocateCopy();
            }

            // Copy the entire map from Variant to component
            *data->getAttributesRef() = *item->getAttributes();

            // Re-apply the TECHNOLOGY, SMD and REFNAME attribute back onto the component
            if (!technologyValue.IsEmpty())
            {
               data->setAttrib(getCamCadData(), technologyKw, valueTypeString, technologyValue.GetBuffer(0), attributeUpdateOverwrite, NULL);
            }
            if (isSMD)
            {
               data->setAttrib(getCamCadData(), smdKw, valueTypeNone, NULL, attributeUpdateOverwrite, NULL);
            }
            if(refnameAttrib)
            {
               Attrib* orgAttrib = data->getAttributesRef()->lookupAttribute(refnameKw);
               if(orgAttrib) data->getAttributesRef()->deleteAttribute(refnameKw);
               data->getAttributesRef()->SetAt(refnameKw, refnameAttrib);
            }
         }
      }

      //MessageBox("Applied Variant \"" + variant->GetName() + "\" to Board Data.", "Applied Variant");
      
      if (HasRealPart(*m_curFile))
      {
         //m_curFile->HideRealpartsForUnloadedComponents(*m_doc);
         m_doc->HideRealpartsForUnloadedComponents(*m_curFile);
      }
      else
         color_ComponentNotLoadedData(m_doc, m_curFile, 120, 120, 120);

      // Set current Variant name on file
      if (m_curFile->getBlock() != NULL)
      {
         int keyword = m_doc->getStandardAttributeKeywordIndex(standardAttributeCurrentVariant);
         m_curFile->getBlock()->setAttrib(getCamCadData(), keyword, valueTypeString, variant->GetName().GetBuffer(0), attributeUpdateOverwrite, NULL);
      }

      m_doc->UpdateAllViews(NULL);

      m_sVariantName = variant->GetName();
      UpdateData(FALSE);
   }
   else
   {
      MessageBox("The selected Variant \"" + variant->GetName() + "\" is not found.", "Applied Variant Error");
   }
}

void CAMCADNavigator::OnHighlightNet()
{
   int iii = 3;

   if (m_compMode == navigatorModeComponents && m_RightClickItem != NULL)
   {
      HTREEITEM parentItem = m_tree.GetParentItem(m_RightClickItem);

      if (parentItem != NULL)
      {
         TreeItemData* componentItemData = (TreeItemData*)m_tree.GetItemData(parentItem);
         DataStruct* component = (DataStruct*)componentItemData->voidPtr;

         if (component != NULL)
         {
            TreeItemData* pinItemData = (TreeItemData*)m_tree.GetItemData(m_RightClickItem);
            DataStruct* pin = (DataStruct*)pinItemData->voidPtr;

            CString pinRef = component->getInsert()->getRefname() + "." + pin->getInsert()->getRefname();
            CString netName = GetNetNameByCompPin(pinRef);

            int componentIndex = 0;

            for (POSITION componentPos = m_components.GetHeadPosition();componentPos != NULL;componentIndex++)
            {
               DataStruct* componentData = m_components.GetNext(componentPos);

               if (componentData == component)
               {
                  break;
               }
            }

            COLORREF color =  highlightColors[componentIndex % HighlightColorCount];

            //clearHighlights();

            // Make sure there is an extent before zooming
            double left, right, top, bottom;

            if (!NetExtents(m_doc,netName, &left, &right, &bottom, &top))
            {
               ErrorMessage("Net has no extents", "Panning to Net");
               return;
            }

            ApplyMargin(m_curZoomMargin, &left, &right, &bottom, &top);
            ZoomPanTo(left, right, bottom, top);

            highlightNet(netName,color);
         }
      }
   }
}

void CAMCADNavigator::OnBnClickedButtonMachineChange()
{
   /*if (m_doc->GetMachineCount(*m_curFile) == 0)
      return;

   CSelectDlg dlg("Select Machine", false, true);
   POSITION pos = m_doc->GetMachineHeadPosition(*m_curFile);

   while (pos != NULL)
   {
      CMachine* machine = m_doc->GetMachineNext(*m_curFile,pos);

      if (machine == NULL)
         continue;

      CMachineSelItem* machineSelItem = new CMachineSelItem(machine->GetName(), machine, false);      
      dlg.AddItem(machineSelItem);
   }

   if (dlg.DoModal() != IDOK)
      return;

   // Get machine name
   CMachine* machine = ((CMachineSelItem*)dlg.GetSelItemHead())->GetMachine();

   if (machine == NULL) 
      return;

   // Find the machine
   if (machine != NULL)
   {
      OnBnClickedButtonClear();

      if(machine->IsDFTSolutionMachine())
      {
         ApplyDFTSolutionToMachine(machine);      
      }
      else if(machine->IsPrepSettingMachine())
      {
         ApplyPrepSettingToMachine();
      }

      UpdateData(FALSE);
	   setDoc(m_doc);

   }*/
}

void CAMCADNavigator::OnCbnSelchangeMachineList()
{
   // TODO: Add your control notification handler code here
   UpdateData(TRUE);
   
   int selectIndex = m_machineSide.GetCurSel();
   CMachine *machine = (selectIndex > -1)?(CMachine *)m_machineSide.GetItemData(selectIndex):NULL;

    // Find the machine
   if (machine != NULL)
   {
      OnBnClickedButtonClear();

      if(machine->IsDFTSolutionMachine())
      {
         //ApplyDFTSolutionToMachine(machine);      
      }
      else if(machine->IsPrepSettingMachine())
      {
         ApplyPrepSettingToMachine();
      }
      
      UpdateData(FALSE);
	   setDoc(m_doc);

   }
}

/*bool CAMCADNavigator::ApplyDFTSolutionToMachine(CMachine *machine)
{
   if(machine->GetName().CompareNoCase(m_sMachineName) == 0 || m_curFile->getBlockType() == blockTypePanel)
      return FALSE;

   // Remove the current access points and probes
   RemoveTestAccessAttributes(m_doc, m_curFile);
   DeleteAllTestAccessPoints(m_doc, m_curFile);
   DeleteAllProbes(m_doc, m_curFile);   

   if (machine->GetDFTSolution() != NULL && machine->GetDFTSolution() != getDoc()->GetCurrentDFTSolution(*m_curFile))
   {
      getDoc()->SetCurrentDFTSolution(*m_curFile,machine->GetDFTSolution());

      CMapWordToPtr accessPointerMap;
      CAccessAnalysisSolution* aaSolution = machine->GetDFTSolution()->GetAccessAnalysisSolution();
      CProbePlacementSolution* ppSolution = machine->GetDFTSolution()->GetProbePlacementSolution();
      
      if (aaSolution != NULL)
         aaSolution->PlaceAccessSolution(&accessPointerMap);

      if (ppSolution != NULL)
         ppSolution->PlaceProbesResultOnBoard(accessPointerMap);

      accessPointerMap.RemoveAll();
   }
   else
   {
      getDoc()->SetCurrentDFTSolution(*m_curFile,"");
   } 

   m_doc->UpdateAllViews(NULL);

   //m_doc->SetCurrentMachine(*m_curFile,machine);
   m_sMachineName = machine->GetName();

   return TRUE;
}*/

/******************************************************************************
* CAMCADNavigator::RemoveNonExistMachines
*/
void CAMCADNavigator::RemoveNonExistCustomMachines(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   CDBInterface database;
   if(!database.IsConnected())
      database.Connect();

   if (database.IsConnected())
   {
      /*for(POSITION machinePos = doc->GetMachineHeadPosition(*pcbFile);machinePos;)
      {
         CMachine *machine = doc->GetMachineNext(*pcbFile,machinePos);
         if(machine && machine->GetMachineType() == MACHINE_TYPE_CUT)
         {
            CString machineName = machine->GetName();
            CDBCustomMachineList *machineList = database.LookupCustomMachines(machineName);
            if(machineList && !machineList->GetCount())
            {
               doc->DeleteMachineOrigin(pcbFile, machineName);
               //doc->getMachineList(*pcbFile)->DeleteMachines(machineName);
            }
            delete machineList;
         }
      } */ 
   }
}


/******************************************************************************
* CAMCADNavigator::FillMachineList
*/
void CAMCADNavigator::FillMachineList(CCEtoODBDoc *doc, FileStruct *pcbFile)
{
   CString curMachineData = "";
   int selectIdx = m_machineSide.GetCurSel();
   if(selectIdx > -1)   m_machineSide.GetLBText(selectIdx, curMachineData);

   m_machineSide.ClearAll();
   if(!doc || !pcbFile)  return;

   RemoveNonExistCustomMachines(doc,pcbFile);

   int macIndex = 0;
   /*for(POSITION machinePos = doc->GetMachineHeadPosition(*pcbFile);machinePos;)
   {
      CMachine *machine = doc->GetMachineNext(*pcbFile,machinePos);
      if(machine)
      {
         if(machine->IsDFTSolutionMachine() && machine->GetDFTSolution())
         {
            m_machineSide.AddString(machine->GetName());
            m_machineSide.SetItemData(macIndex++,(DWORD)machine);
         }
         else if(machine->IsPrepSettingMachine() && machine->GetMachineAttribute(doc,pcbFile,false))
         {
            if(!machine->IsBothSideSetting())
            {
               m_machineSide.AddString(machine->GetName() + QMACHINESIDE_TOP);
               m_machineSide.SetItemData(macIndex++,(DWORD)machine);
               m_machineSide.AddString(machine->GetName() + QMACHINESIDE_BOT);
               m_machineSide.SetItemData(macIndex++,(DWORD)machine);
            }
            else
            {
               m_machineSide.AddString(machine->GetName());
               m_machineSide.SetItemData(macIndex++,(DWORD)machine);
            }

         }
      }
   }*/
   
   GetDlgItem(IDC_LIST_MACHINESIDE)->EnableWindow(m_machineSide.GetCount()?TRUE:FALSE);
   
   //CMachine* machine = doc->GetCurrentMachine(*pcbFile);
   /*if(machine)
   {
      if(machine->IsDFTSolutionMachine())
      {
         if(curMachineData.IsEmpty()) curMachineData = machine->GetName();
         m_machineSide.SelectString(0,curMachineData);
      }
      else
      {
         if(curMachineData.IsEmpty()) curMachineData = machine->GetName() + QMACHINESIDE_TOP;
         m_machineSide.SelectString(0,curMachineData);
         ApplyPrepSettingToMachine();
      }
   }
   else
   {      
      //Reset Machine and DFT solution
      m_machineSide.SetCurSel(-1);
      m_sMachineName.Empty();
      // No... wait! DON'T reset the DFT Solution! Doing the reset caused DR 806308.
      // I don't see a reason for this in the first place, so not seeing the downside of leaving this out.
      // It is left here as a comment for awhile, in case we end up needing it back. And then we'll need
      // something else done altogether. Hope we don't get there.
      /////   getDoc()->SetCurrentDFTSolution(*pcbFile,"");
   }*/
   
   UpdateData(FALSE);
}

bool CAMCADNavigator::ApplyPrepSettingToMachine()
{
   int selIndex = m_machineSide.GetCurSel();
   if(selIndex < 0) 
   {  
      if(m_machineSide.GetCount())
      {
         selIndex = 0;
         m_machineSide.SetCurSel(selIndex);
      }
      else
         return FALSE;
   }

   CMachine *machine = (CMachine *)m_machineSide.GetItemData(selIndex);
   if(!machine || !machine->IsPrepSettingMachine() || !machine->HasConfiguration()) return FALSE;

   CString surface= "";
   m_machineSide.GetLBText(selIndex,surface);
   
   bool isBottomSide = (surface.Find(QMACHINESIDE_BOT) > -1)?true:false;   
   Attrib *attrib =  machine->GetMachineAttribute(m_doc, m_curFile,isBottomSide);   
   
   if(attrib)
   {
      ExportFileSettings PCBLocation;
      PCBLocation = attrib->getStringValue();
      MoveToBoardOrigin(PCBLocation,isBottomSide);   
      
   }

   //m_doc->SetCurrentMachine(*m_curFile,machine);
   m_sMachineName = machine->GetName();
   
   m_doc->UpdateAllViews(NULL);
   m_doc->OnFitPageToImage();
   return TRUE;
}

bool CAMCADNavigator::MoveToBoardOrigin(ExportFileSettings& PCBLocation, bool bottomSide)
{      
   double fileInsertX = m_curFile->getInsertX(), fileInsertY = m_curFile->getInsertY();
   double fileangle = m_curFile->getRotationRadians();

   bool isMirror = bottomSide;
   bool mirroringRequired = false;
   bool isFileRestructed = false;
   bool isOriginExisted = false;
   
   isOriginExisted = PCBLocation.GetMachineOriginOffset(m_doc,m_curFile,fileInsertX,fileInsertY,fileangle,isMirror,mirroringRequired,bottomSide);
   
   if(isOriginExisted)
   {
      isFileRestructed = true;      
      m_curFile->setMirrored(isMirror);
      m_curFile->setRotation(fileangle);
      m_curFile->setInsertX(fileInsertX);
      m_curFile->setInsertY(fileInsertY);
      
      // Save the machine origin so file can rotate back to view origin
      RestructureFile(m_doc,m_curFile);
   }

   return isOriginExisted;
}

///////////////////////////////////////////////////////////////////////
// CEditProbe dialog

IMPLEMENT_DYNAMIC(CEditProbe, CDialog)
CEditProbe::CEditProbe(CWnd* pParent /*=NULL*/)
   : CDialog(CEditProbe::IDD, pParent)
   , m_side(0)
   , m_type(0)
{
}

CEditProbe::CEditProbe(CCEtoODBDoc *document, CWnd* pParent /*=NULL*/)
   : CDialog(CEditProbe::IDD, pParent)
   , m_side(0)
   , m_type(0)
{
   m_doc = document;
}

CEditProbe::~CEditProbe()
{
}

void CEditProbe::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Radio(pDX, IDC_RADIO_SIDETOP, m_side);
   DDX_Radio(pDX, IDC_RADIO_TYPEPI, m_type);
   DDX_Control(pDX, IDC_PROBE_SIZE, m_sizeList);
}


BEGIN_MESSAGE_MAP(CEditProbe, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk) //case 991: message map for done was not available, function was not called
   ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
   ON_BN_CLICKED(IDC_RADIO_SIDETOP, OnBnClickedRadioSidetop)
   ON_BN_CLICKED(IDC_RADIO_SIDEBOT, OnBnClickedRadioSidebot)
END_MESSAGE_MAP()


// CEditProbe message handlers

/**********************************************88
* CEditProbe::OnInitDialog
*/
/*BOOL CEditProbe::OnInitDialog()
{
   CDialog::OnInitDialog();
   UpdateData();
   HTREEITEM itemSel = frame->getNavigator().getTree()->GetSelectedItem();

   if (!itemSel)
      return FALSE;

   TreeItemData *itemData = (TreeItemData*)frame->getNavigator().getTree()->GetItemData(itemSel);
   DataStruct* probe = (DataStruct*)itemData->voidPtr;

   if (!probe)
      return FALSE;

   //get side
   probe->getInsert()->getGraphicMirrored()?m_side=1:m_side=0;
   UpdateData(FALSE);

   if (!m_side)
   {
      if (getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile())) == NULL ||
          getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan() == NULL)
         return 0;

      CTestPlan* testPlan = getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan();
      POSITION topPos = testPlan->GetProbes().GetHeadPosition_TopProbes();

      while (topPos)
      {
         CDFTProbeTemplate *pTemplate = testPlan->GetProbes().GetNext_TopProbes(topPos);

         if (!pTemplate)
            continue;

         const char* sizeName = pTemplate->GetName();
         //if the geom doesn't exist yet, create it
         BlockStruct *probeBlock = CreateTestProbeGeometry(m_doc, pTemplate->GetName()+"_Top", pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()),
            pTemplate->GetName()+"_Top", pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()));

         if (m_sizeList.FindString(0, sizeName) == -1)
         {
            //if the geom exists, add its name to the list
            m_sizeList.SetItemData(m_sizeList.AddString(sizeName), (DWORD)sizeName);
         }

      }
   }
   else
   {
      if (getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile())) == NULL ||
          getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan() == NULL)
         return 0;

      CTestPlan* testPlan = getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan();
      POSITION botPos = testPlan->GetProbes().GetHeadPosition_BotProbes();

      while (botPos)
      {
         CDFTProbeTemplate *pTemplate = testPlan->GetProbes().GetNext_TopProbes(botPos);

         if (!pTemplate)
            continue;

         const char* sizeName = pTemplate->GetName();

         //if the geom doesn't exist yet, create it
         BlockStruct *probeBlock = CreateTestProbeGeometry(m_doc, pTemplate->GetName()+"_Bot", pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()),
            pTemplate->GetName()+"_Bot", pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()));
         
         if (m_sizeList.FindString(0, sizeName) == -1)
         {
            //add its name to the list
            m_sizeList.SetItemData(m_sizeList.AddString(sizeName), (DWORD)sizeName);
         }
      }
   }

   //get size
   int geomNum = probe->getInsert()->getBlockNumber();
   BlockStruct* geom = m_doc->Find_Block_by_Num(geomNum);

   if (!geom)
      return FALSE;

   int nIndex = -1;
   const char* sizeName = geom->getName();
   CString sName = sizeName;

   if (sName.FindOneOf("_") != -1)
      sName = sName.Left(sName.FindOneOf("_"));

   if ((nIndex = m_sizeList.FindString(0, sName)) == -1)
   {
      //add its name to the list and select it
      m_sizeList.SetItemData(m_sizeList.AddString(sName), (DWORD)&sName);
      m_sizeList.SetCurSel(m_sizeList.FindString(0, sName));
   }  
   else
   {
      m_sizeList.SetCurSel(nIndex);
   }

   //get type
   Attrib *attrib = is_attvalue(m_doc, probe->getAttributeMap(), "TEST_RESOURCE", 0);

   if (attrib && attrib->getValueType() == valueTypeString)
   {
      CString type = "";
      type = attrib->getStringValue();

      if (!type.CompareNoCase("Power Injection"))
      {
         m_type = 0;
         UpdateData(FALSE);
      }
      else if (!type.CompareNoCase("Test"))
      {
         m_type = 1;
         UpdateData(FALSE);
      }
      else
      {
      }
   }

   //get probe message
   Attrib *attribProbeMessage = is_attvalue(m_doc, probe->getAttributeMap(), "PROBE_MESSAGE", 0);

   if (attribProbeMessage && attribProbeMessage->getValueType() == valueTypeString)
   {
      CString probeMessage = "";
      probeMessage = attribProbeMessage->getStringValue();
      GetDlgItem(IDC_EDIT_PROBE_MESSAGE)->SetWindowText(probeMessage);
      UpdateData(FALSE);
   }

   return TRUE;
}*/

/**********************************************88
* CEditProbe::OnBnClickedOk
*/
void CEditProbe::OnBnClickedOk()
{
   HTREEITEM itemSel = frame->getNavigator().getTree()->GetSelectedItem();

   if (!itemSel)
      return;

   TreeItemData *itemData = (TreeItemData*)frame->getNavigator().getTree()->GetItemData(itemSel);
   DataStruct* probe = (DataStruct*)itemData->voidPtr;

   if (!probe)
      return;
   
   UpdateData();
   m_size = "";

   if (m_sizeList.GetCurSel() != -1)
      m_size = (const char*)m_sizeList.GetItemData(m_sizeList.GetCurSel());

   //get the geom and set the probe's blockNum to it.
   CString geomName = m_size;
   geomName += m_side?"_Bot":"_Top";
   BlockStruct* geom = m_doc->Find_Block_by_Name(geomName, -1);

   if (geom && m_size)
   {
      probe->getInsert()->setBlockNumber(geom->getBlockNumber());
   }
   
   if (m_side == 0)
   {
      probe->getInsert()->setMirrorFlags(0);
      probe->getInsert()->setPlacedBottom(false);
   }
   else if (m_side == 1)
   {
      probe->getInsert()->setMirrorFlags(3);
      probe->getInsert()->setPlacedBottom(true);
   }
 
   CString probeMessage;
   GetDlgItem(IDC_EDIT_PROBE_MESSAGE)->GetWindowText(probeMessage);

   if (!probeMessage.IsEmpty())
   {
      int probeMessageKey = m_doc->RegisterKeyWord("PROBE_MESSAGE", 0, valueTypeString);
      probe->setAttrib(getCamCadData(), probeMessageKey, valueTypeString, probeMessage.GetBuffer(0), attributeUpdateOverwrite, NULL);
   }

   CString testResourceType;

   if (m_type == 0)
      testResourceType = "Power Injection";
   else if (m_type == 1)
      testResourceType = "Test";

   WORD testResKW = getCamCadData().getAttributeKeywordIndex(standardAttributeTestResource);
   probe->setAttrib(getCamCadData(), testResKW, valueTypeString, testResourceType.GetBuffer(0), attributeUpdateOverwrite, NULL);

   // Make sure probe and access marker are on same side (side of probe dictates side for access marker)
   WORD DATALINK  = getCamCadData().getAttributeKeywordIndex(standardAttributeDataLink);
   Attrib *datalinkAttrib = NULL;
   if (probe->lookUpAttrib(DATALINK, datalinkAttrib))
   {
      CEntity accessEntity = CEntity::findEntity(getCamCadData(), datalinkAttrib->getIntValue());
      if (accessEntity.getEntityType() == entityTypeData)
      {
         DataStruct *accessData = accessEntity.getData();
         if (accessData != NULL)
         {
            // Match the inserts
            accessData->getInsert()->setPlacedBottom( probe->getInsert()->getPlacedBottom() );
            accessData->getInsert()->setMirrorFlags(  probe->getInsert()->getMirrorFlags()  );

            // Update the access attrib
            if (accessData->lookUpAttrib(DATALINK, datalinkAttrib))
            {
               CEntity targetEntity = CEntity::findEntity(getCamCadData(), datalinkAttrib->getIntValue());
               CAttributes **attrRef = targetEntity.getAttributesRef();

               int testAccessKW = getCamCadData().getAttributeKeywordIndex(standardAttributeTestAccess);

               Attrib *testAccessAttrib = NULL;
               if (attrRef != NULL && (*attrRef) != NULL && (*attrRef)->Lookup(testAccessKW, testAccessAttrib))
               {
                  CString testAccessVal = get_attvalue_string(m_doc, testAccessAttrib);
                  if (testAccessVal.CompareNoCase("BOTH") != 0)
                  {
                     CString accessSideStr = probe->getInsert()->getPlacedTop() ? "TOP" : "BOTTOM";
                     m_doc->SetAttrib(attrRef, testAccessKW, accessSideStr.GetBuffer(0));
                  }
               }
            }

            // Tag Whodunnit
            int MOVEDBY = getCamCadData().getDefinedAttributeKeywordIndex("MOVED_BY", valueTypeString);
            accessData->setAttrib(getCamCadData(), MOVEDBY, valueTypeString, (void*)frame->getNavigator().GetWhoAndWhen().GetBuffer(0), attributeUpdateOverwrite, NULL);
         }
      }
   }
   
   m_doc->UpdateAllViews(NULL);

   OnOK();
}

/**********************************************88
* CEditProbe::OnBnClickedButton1
*/
void CEditProbe::OnBnClickedButton1()
{
   CDialog::OnCancel();
}

/**********************************************88
* CEditProbe::UpdateSizeList
*/
/*void CEditProbe::UpdateSizeList()
{
   m_sizeList.ResetContent();

   if (!m_side)  //if top
   {
      if (getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile())) == NULL ||
          getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan() == NULL)
         return;

      CTestPlan* testPlan = getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan();
      POSITION topPos = testPlan->GetProbes().GetHeadPosition_TopProbes();

      while (topPos)
      {
         CDFTProbeTemplate *pTemplate = testPlan->GetProbes().GetNext_TopProbes(topPos);

         if (!pTemplate)
            continue;

         const char* sizeName = pTemplate->GetName();
         //if the geom doesn't exist yet, create it
         BlockStruct *probeBlock = CreateTestProbeGeometry(m_doc, pTemplate->GetName()+"_Top", pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()),
            pTemplate->GetName()+"_Top", pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()));  

         if (m_sizeList.FindString(0, sizeName) == -1)
         {
            //if the geom exists, add its name to the list
            m_sizeList.SetItemData(m_sizeList.AddString(sizeName), (DWORD)sizeName);
         }
      }
   }
   else  //else if bottom
   {
      if (getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile())) == NULL ||
          getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan() == NULL)
         return;

      CTestPlan* testPlan = getCamCadDoc()->GetCurrentDFTSolution(*(frame->getNavigator().getCurFile()))->GetTestPlan();
      POSITION botPos = testPlan->GetProbes().GetHeadPosition_BotProbes();

      while (botPos)
      {
         CDFTProbeTemplate *pTemplate = testPlan->GetProbes().GetNext_TopProbes(botPos);

         if (!pTemplate)
            continue;

         const char* sizeName = pTemplate->GetName();

         //if the geom doesn't exist yet, create it
         BlockStruct *probeBlock = CreateTestProbeGeometry(m_doc, pTemplate->GetName()+"_Bot", pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()),
            pTemplate->GetName()+"_Bot", pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_doc->getSettings().getPageUnits()));
         
         if (m_sizeList.FindString(0, sizeName) == -1)
         {
            //add its name to the list
            m_sizeList.SetItemData(m_sizeList.AddString(sizeName), (DWORD)sizeName);
         }
      }
   }
}*/

/**********************************************88
* CEditProbe::OnBnClickedRadioSidetop
*/
void CEditProbe::OnBnClickedRadioSidetop()
{
   m_side = 0;
   UpdateData();
   //UpdateSizeList(); 
}

/**********************************************88
* CEditProbe::OnBnClickedRadioSidebot
*/
void CEditProbe::OnBnClickedRadioSidebot()
{
   m_side = 1;
   UpdateData();
   //UpdateSizeList();
}

///////////////////////////////////////////////////////////////////////
// CNavigatorOptions dialog

IMPLEMENT_DYNAMIC(CNavigatorOptions, CDialog)
CNavigatorOptions::CNavigatorOptions(CWnd* pParent /*=NULL*/)
   : CDialog(CNavigatorOptions::IDD, pParent)
   , m_Pan(0)
   , m_Flip(FALSE)
{
}

CNavigatorOptions::CNavigatorOptions(CCEtoODBDoc *document, CWnd* pParent /*=NULL*/)
   : CDialog(CNavigatorOptions::IDD, pParent)
   , m_Pan(0)
   , m_Flip(FALSE)
   , m_ShowSide(FALSE)
{
   m_doc = document;
}

CNavigatorOptions::~CNavigatorOptions()
{
}

void CNavigatorOptions::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Radio(pDX, IDC_RADIO_PAN, m_Pan);
   DDX_Check(pDX, IDC_CHK_FLIP, m_Flip);
   DDX_Check(pDX, IDC_CHK_SHOWSIDE, m_ShowSide);
}

BEGIN_MESSAGE_MAP(CNavigatorOptions, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// CNavigatorOptions message handlers

/**********************************************88
* CNavigatorOptions::OnInitDialog
*/
BOOL CNavigatorOptions::OnInitDialog()
{
   CDialog::OnInitDialog();
   m_Pan = !frame->getNavigator().GetPanning();
   m_Flip = frame->getNavigator().GetFlip();
   m_ShowSide = frame->getNavigator().GetShowSide();

   UpdateData(FALSE);
   
   return TRUE;
}

/**********************************************88
* CNavigatorOptions::OnBnClickedOk
*/
void CNavigatorOptions::OnBnClickedOk()
{
   UpdateData();

   //pan/zoom options
   if (m_Pan == 0)
      frame->getNavigator().SetPanning(true);
   else if (m_Pan == 1)
      frame->getNavigator().SetPanning(false);

   frame->getNavigator().SetFlip(m_Flip?true:false);

   frame->getNavigator().SetShowSide(m_ShowSide?true:false);

   OnOK();
}

BOOL ComponentsExtentsByDataStruct(CCEtoODBDoc *doc, DataStruct *data, FileStruct *file, double *xmin, double *xmax, double *ymin, double *ymax)
{
   *xmin = *ymin = DBL_MAX;
   *xmax = *ymax = -DBL_MAX;
   if(NULL != file && file->isShown() && !file->isHidden() && !file->notPlacedYet() && file->getBlock() != NULL)
   {
      double fileRot = file->getRotation();
      int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

      Point2 point2;
      point2.x = data->getInsert()->getOriginX() * file->getScale();
      point2.y = data->getInsert()->getOriginY() * file->getScale();
      if (fileMirror & MIRROR_FLIP)
      {
         point2.x = -point2.x;
         fileRot = -fileRot;
      }

      Mat2x2 m;
      RotMat2(&m, fileRot);
      TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

      double _rot;
      if (fileMirror & MIRROR_FLIP)
         _rot = file->getRotation() - data->getInsert()->getAngle();
      else
         _rot = file->getRotation() + data->getInsert()->getAngle();
      int _mirror = fileMirror ^ data->getInsert()->getMirrorFlags();

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      doc->validateBlockExtents(block);

      if (block != NULL && block->extentIsValid())
      {
         CExtent extents = UseInsertExtents(block, point2.x, point2.y,
            _rot, (_mirror?MIRROR_FLIP:0), file->getScale() * data->getInsert()->getScale());

         if (extents.getXmin() < *xmin)
            *xmin = extents.getXmin();
         if (extents.getXmax() > *xmax)
            *xmax = extents.getXmax();
         if (extents.getYmin() < *ymin)
            *ymin = extents.getYmin();
         if (extents.getYmax() > *ymax)
            *ymax = extents.getYmax();
      }
      else
      {
         if (point2.x < *xmin)
            *xmin = point2.x;
         if (point2.x > *xmax)
            *xmax = point2.x;
         if (point2.y < *ymin)
            *ymin = point2.y;
         if (point2.y > *ymax)
            *ymax = point2.y;
      }
   }
   if (*xmin > *xmax)
      return FALSE;

   return TRUE;
}

