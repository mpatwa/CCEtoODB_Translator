// $Header: /CAMCAD/4.6/Attr_Ass.cpp 41    12/07/06 12:00p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "attr_ass.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "DftAttributeCalculator.h"
#include "DcaEnumIterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static FileStruct *pcbFile;


/******************************************************************************
* OnAutomatedAttribAssignment
*/
void CCEtoODBDoc::OnAutomatedAttribAssignment() 
{
   int stat = 0; // 0 = no PCB file, 1 = PCB, but not visible, 2 = PCB and visible

   pcbFile = NULL;
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);
      if (file->getBlockType() == BLOCKTYPE_PCB)
      {
         if (file->isShown())
         {
            stat |= 2;
            pcbFile = file;
            break;
         }
         else
         {
            stat |= 1;
         }
      }
      if (!stat && file->isShown() && !pcbFile)
         pcbFile = file;
   }

   if (!pcbFile)
   {
      if (stat == 1)
         ErrorMessage("PCB File loaded, but not visible -> will be ignored!");
      else
         ErrorMessage("No PCB File loaded!");
      return;
   }

   AttributeAssignment dlg;
   dlg.doc = this;
   dlg.DoModal();
}

/**************************************************************************
* OnTestpointAssignment
*/
void CCEtoODBDoc::OnTestpointAssignment() 
{
   int stat = 0; // 0 = no PCB file, 1 = PCB, but not visible, 2 = PCB and visible

   pcbFile = NULL;
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);
      if (file->getBlockType() == BLOCKTYPE_PCB)
      {
         if (file->isShown())
         {
            stat |= 2;
            pcbFile = file;
            break;
         }
         else
         {
            stat |= 1;
         }
      }
   }

   if (!pcbFile)
   {
      if (stat == 1)
         ErrorMessage("PCB File loaded, but not visible -> will be ignored!");
      else
         ErrorMessage("No PCB File loaded!");
      return;
   }

   AttributeAssignment dlg;
   dlg.doc = this;
   dlg.AssigningTestAttrib = TRUE;
   dlg.DoModal();
}

void CCEtoODBDoc::OnAssignDftAttributes() 
{
   calculatePcbValidationAttributes(true);
}

/******************************************************************************
* AttributeAssignment Dialog
*/
AttributeAssignment::AttributeAssignment(CWnd* pParent /*=NULL*/)
   : CResizingDialog(AttributeAssignment::IDD, pParent)
{
   //{{AFX_DATA_INIT(AttributeAssignment)
   m_keyword = _T("");
   m_value = _T("");
   m_type = 0;
   m_qualifierText = _T("");
   m_selectionText = _T("");
   m_valueType = _T("");
   m_assignCount = 0;
   m_deleteCount = 0;
   //}}AFX_DATA_INIT
   AssigningTestAttrib = FALSE;

   // section 3
   CResizingDialogField* attributeGroup = &(addFieldControl(IDC_STATIC5     ,anchorBottomLeft,growHorizontal));
   addFieldControl(IDC_KEYWORD_CB  ,anchorBottomLeft,growHorizontal);
   addFieldControl(IDC_STATIC6     ,anchorBottomLeft);
   addFieldControl(IDC_STATIC7     ,anchorBottomLeft);
   addFieldControl(IDC_STATIC9     ,anchorBottomLeft);
   addFieldControl(IDC_STATIC10    ,anchorBottomLeft);
   addFieldControl(IDC_VALUETYPE   ,anchorBottomLeft);
   addFieldControl(IDC_VALUE       ,anchorBottomLeft);
   addFieldControl(IDC_ASSIGN      ,anchorBottomLeft);
   addFieldControl(IDC_DELETE      ,anchorBottomLeft);
   addFieldControl(IDC_COUNT       ,anchorBottomLeft);
   addFieldControl(IDC_DELETE_COUNT,anchorBottomLeft);

   addFieldControl(IDCANCEL   ,anchorBottomRight);

   // section 1&2
   CResizingDialogField* sectionGroup = &(addFieldControl(IDC_STATIC1,anchorLeft,growHorizontal));
   sectionGroup->getOperations().addOperation(glueBottomEdge      ,toTopEdge   ,attributeGroup);

   // section 2
   CResizingDialogField* selectionField = &(addFieldControl(IDC_SELECTION         ,anchorProportionalBoth,growProportionalBoth));
   selectionField->getOperations().addOperation(glueBottomEdge      ,toBottomEdge   ,sectionGroup);

   CResizingDialogField* selectionTextField = &(addFieldControl(IDC_SELECTION_TEXT));
   selectionTextField->getOperations().addOperation(glueTopEdge      ,toTopEdge   ,selectionField);
   selectionTextField->getOperations().addOperation(glueBottomEdge   ,toTopEdge   ,selectionField);
   selectionTextField->getOperations().addOperation(glueLeftEdge     ,toLeftEdge  ,selectionField);
   selectionTextField->getOperations().addOperation(glueRightEdge    ,toRightEdge ,selectionField);

   CResizingDialogField* qualifierField = &(addFieldControl(IDC_QUALIFIER    ));
   qualifierField->getOperations().addOperation(glueTopEdge   ,toTopEdge   ,selectionField);
   qualifierField->getOperations().addOperation(glueBottomEdge,toBottomEdge,selectionField);
   qualifierField->getOperations().addOperation(glueRightEdge ,toRightEdge ,sectionGroup);
   qualifierField->getOperations().addOperation(glueLeftEdge  ,toRightEdge ,selectionField);
   qualifierField->getOperations().addOperation(glueBottomEdge,toBottomEdge,sectionGroup);

   CResizingDialogField* qualifierTextField = &(addFieldControl(IDC_QUALIFIER_TEXT));
   qualifierTextField->getOperations().addOperation(glueTopEdge      ,toTopEdge   ,qualifierField);
   qualifierTextField->getOperations().addOperation(glueBottomEdge   ,toTopEdge   ,qualifierField);
   qualifierTextField->getOperations().addOperation(glueLeftEdge     ,toLeftEdge  ,qualifierField);
   qualifierTextField->getOperations().addOperation(glueRightEdge    ,toRightEdge ,qualifierField);

   CResizingDialogField* byGroup = &(addFieldControl(IDC_STATIC3,anchorLeft    ));
   byGroup->getOperations().addOperation(glueTopEdge   ,toTopEdge   ,selectionField);
   byGroup->getOperations().addOperation(glueRightEdge ,toLeftEdge  ,selectionField);
   byGroup->getOperations().addOperation(glueBottomEdge,toBottomEdge,selectionField);

   CResizingDialogField* byItemRadioField = &(addFieldControl(IDC_TYPE,anchorLeft    ));
   byItemRadioField->getOperations().addOperation(glueTopEdge   ,toTopEdge   ,byGroup);
   byItemRadioField->getOperations().addOperation(glueBottomEdge,toTopEdge   ,byGroup);

   CResizingDialogField* byItemField = &(addFieldControl(IDC_ITEM_CB,anchorLeft    ));
   byItemField->getOperations().addOperation(glueTopEdge   ,toTopEdge   ,byGroup);
   byItemField->getOperations().addOperation(glueBottomEdge,toTopEdge   ,byGroup);
   byItemField->getOperations().addOperation(glueRightEdge ,toRightEdge ,byGroup);

   CResizingDialogField* byAttributeRadioField = &(addFieldControl(IDC_TYPE_ATTRIB,anchorLeft    ));
   byAttributeRadioField->getOperations().addOperation(glueTopEdge   ,toTopEdge   ,byGroup);
   byAttributeRadioField->getOperations().addOperation(glueBottomEdge,toTopEdge   ,byGroup);

   CResizingDialogField* byAttribField = &(addFieldControl(IDC_ATTRIB_CB,anchorLeft    ));
   byAttribField->getOperations().addOperation(glueTopEdge   ,toTopEdge   ,byGroup);
   byAttribField->getOperations().addOperation(glueBottomEdge,toTopEdge   ,byGroup);
   byAttribField->getOperations().addOperation(glueRightEdge ,toRightEdge ,byGroup);

   // section 1
   CResizingDialogField* insertTypesGroup = &(addFieldControl(IDC_STATIC4      ,anchorLeft,growHorizontal));
   insertTypesGroup->getOperations().addOperation(glueBottomEdge   ,toTopEdge   ,selectionField);

   CResizingDialogField* insertTypesField = &(addFieldControl(IDC_INSERTTYPE_LB,anchorLeft,growHorizontal));
   insertTypesField->getOperations().addOperation(glueBottomEdge   ,toTopEdge   ,selectionField);

   addFieldControl(IDC_IT_ON       ,anchorRight);
   addFieldControl(IDC_IT_OFF      ,anchorRight);
}

void AttributeAssignment::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AttributeAssignment)
   DDX_Control(pDX, IDC_QUALIFIER, m_qualifierLB);
   DDX_Control(pDX, IDC_SELECTION, m_selectionLB);
   DDX_Control(pDX, IDC_INSERTTYPE_LB, m_itLB);
   DDX_Control(pDX, IDC_ITEM_CB, m_itemCB);
   DDX_Control(pDX, IDC_ATTRIB_CB, m_attribCB);
   DDX_Control(pDX, IDC_KEYWORD_CB, m_keywordCB);
   DDX_CBString(pDX, IDC_KEYWORD_CB, m_keyword);
   DDX_Text(pDX, IDC_VALUE, m_value);
   DDX_Radio(pDX, IDC_TYPE, m_type);
   DDX_Text(pDX, IDC_QUALIFIER_TEXT, m_qualifierText);
   DDX_Text(pDX, IDC_SELECTION_TEXT, m_selectionText);
   DDX_Text(pDX, IDC_VALUETYPE, m_valueType);
   DDX_Text(pDX, IDC_COUNT, m_assignCount);
   DDX_Text(pDX, IDC_DELETE_COUNT, m_deleteCount);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AttributeAssignment, CResizingDialog)
   //{{AFX_MSG_MAP(AttributeAssignment)
   ON_BN_CLICKED(IDC_ASSIGN, OnAssign)
   ON_BN_CLICKED(IDC_TYPE_ATTRIB, OnTypeAttrib)
   ON_BN_CLICKED(IDC_TYPE, OnTypeItem)
   ON_CBN_SELCHANGE(IDC_ITEM_CB, FillSelections)
   ON_LBN_SELCHANGE(IDC_SELECTION, OnSelchangeSelection)
   ON_BN_CLICKED(IDC_IT_OFF, OnItOff)
   ON_BN_CLICKED(IDC_IT_ON, OnItOn)
   ON_CBN_SELCHANGE(IDC_KEYWORD_CB, OnSelchangeKeywordCb)
   ON_LBN_SELCHANGE(IDC_QUALIFIER, OnSelchangeQualifier)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   ON_CBN_SELCHANGE(IDC_ATTRIB_CB, FillSelections)
   ON_LBN_SELCHANGE(IDC_INSERTTYPE_LB, OnUpdateByInserttype)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL AttributeAssignment::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      int sel;
      switch (nID)
      {
      case IDC_SELECTION:
         sel = m_selectionLB.GetCurSel();
         if (sel != LB_ERR)
         {
            m_selectionLB.GetText(sel, hint); 
            pTTT->lpszText = hint.GetBuffer(0);
         }
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

BOOL AttributeAssignment::OnInitDialog() 
{
   CResizingDialog::OnInitDialog();

   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_SELECTION), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   for (int i=0; i<doc->getKeyWordArray().GetCount(); i++)
      m_keywordCB.AddString(doc->getKeyWordArray()[i]->cc);

   GetDlgItem(IDC_ATTRIB_CB)->EnableWindow(FALSE);

   m_itemCB.AddString("Ref Des Prefix");
   m_itemCB.AddString("Inserts");
   if (!AssigningTestAttrib)
   {
      m_itemCB.AddString("Geometry Definitions");
      m_itemCB.AddString("Nets");
      m_itemCB.AddString("Comp/Pins by Net");
      m_itemCB.AddString("Comp/Pins by Geometry");
      m_itemCB.AddString("Types");
      m_itemCB.AddString("Layers");
      m_itemCB.AddString("Datas");
   }
   m_itemCB.SetCurSel(0);

   m_attribCB.AddString("Inserts");
   if (!AssigningTestAttrib)
   {
      m_attribCB.AddString("Geometry Definitions");
      m_attribCB.AddString("Nets");
      m_attribCB.AddString("Comp/Pins");
      m_attribCB.AddString("Types");
      m_attribCB.AddString("Layers");
      m_attribCB.AddString("Datas");
   }

   m_selectionText = "";
   m_qualifierText = "";   
   m_assignCount = 0;
   m_deleteCount = 0;

   decimals = GetDecimals(doc->getSettings().getPageUnits());

   if (AssigningTestAttrib)
   {
      m_keyword = ATT_TEST;
      GetDlgItem(IDC_KEYWORD_CB)->EnableWindow(FALSE);
      m_value = "";
      m_valueType = "Values = BOTH, TOP, BOTTOM, or NONE";
      UpdateData(FALSE);
   }

   FillInsertTypeLB();

   EnableInsertTypes(FALSE);
   
   UpdateData(FALSE);

   OnItOn();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void AttributeAssignment::OnCancel() 
{
   CleanLBMem(&m_qualifierLB);
   CleanLBMem(&m_selectionLB);

   CResizingDialog::OnCancel();
}

void AttributeAssignment::CleanLBMem(CListBox *lb) 
{
   int count = lb->GetCount();
   for (int i=0; i<count; i++)
      delete ((CPtrList*)lb->GetItemDataPtr(i));
}

void AttributeAssignment::OnTypeAttrib() 
{
   UpdateData();

   GetDlgItem(IDC_ATTRIB_CB)->EnableWindow();
   GetDlgItem(IDC_ITEM_CB)->EnableWindow(FALSE);
   m_itemCB.SetCurSel(-1);

   GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_HIDE);

   CleanLBMem(&m_qualifierLB);
   m_qualifierLB.ResetContent();
   CleanLBMem(&m_selectionLB);
   m_selectionLB.ResetContent();

   m_selectionText = "";
   m_qualifierText = "";   
   m_assignCount = 0;
   m_deleteCount = 0;

   UpdateData(FALSE);
}

void AttributeAssignment::OnTypeItem() 
{
   UpdateData();

   GetDlgItem(IDC_ITEM_CB)->EnableWindow();
   GetDlgItem(IDC_ATTRIB_CB)->EnableWindow(FALSE);
   m_attribCB.SetCurSel(-1);

   GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_HIDE);

   CleanLBMem(&m_qualifierLB);
   m_qualifierLB.ResetContent();
   CleanLBMem(&m_selectionLB);
   m_selectionLB.ResetContent();

   m_selectionText = "";
   m_qualifierText = "";
   m_assignCount = 0;
   m_deleteCount = 0;

   UpdateData(FALSE);
}

void AttributeAssignment::AddItemToStorageLB(CListBox *lb, const char *string, void *ptr) 
{
   int index = lb->FindStringExact(-1, string);
   if (index == LB_ERR)
   {
      int index = lb->AddString(string);
      CPtrList *list = new CPtrList;
      list->AddTail(ptr);
      lb->SetItemDataPtr(index, list);
   }
   else
   {
      CPtrList *list = (CPtrList*)lb->GetItemDataPtr(index);
      list->AddTail(ptr);
   }
}

void AttributeAssignment::FillInsertTypeLB()
{  
   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      m_itLB.SetItemData(m_itLB.AddString(insertTypeToDisplayString(insertType)),insertType);
   }

   OnItOn();
}

void AttributeAssignment::OnItOn() 
{
   m_itLB.SelItemRange(TRUE, 0, m_itLB.GetCount());
   OnUpdateByInserttype();
}

void AttributeAssignment::OnItOff() 
{
   m_itLB.SelItemRange(FALSE, 0, m_itLB.GetCount());
   OnUpdateByInserttype();
}

BOOL AttributeAssignment::InsertFilter(int inserttype) 
{
   int count = m_itLB.GetSelCount();
   if (!count)
      return FALSE;

   int *arr = (int*)calloc(count, sizeof(int));
   m_itLB.GetSelItems(count, arr);

   for (int i=0; i<count; i++)
   {
      if (inserttype == m_itLB.GetItemData(arr[i]))
      {
         free(arr);
         return TRUE;
      }
   }

   free(arr);
   return FALSE;
}

void AttributeAssignment::OnUpdateByInserttype() 
{
   CString selString;
   int sel = m_selectionLB.GetCurSel();
   if (sel != CB_ERR)
      m_selectionLB.GetText(sel, selString);

   if (!m_type) // by Item
   {
      int item = m_itemCB.GetCurSel();
      if (item == 0 || item == 1)
         FillSelections();
   }
   else // by Attribute
   {
      FillSelections();
   }

   if (sel != LB_ERR)
   {
      sel = m_selectionLB.FindStringExact(-1, selString);
      if (sel != LB_ERR)
      {
         m_selectionLB.SetCurSel(sel);
         OnSelchangeSelection();
      } 
   }
}

void AttributeAssignment::FillAttribKeywords(CAttributes** map)
{
   if (*map == NULL)
      return;

   for (POSITION attribPos = (*map)->GetStartPosition();attribPos != NULL;)
   {
      Attrib* attrib;
      WORD keyword;

      (*map)->GetNextAssoc(attribPos, keyword, attrib);
      
      AddItemToStorageLB(&m_selectionLB, doc->getKeyWordArray()[keyword]->cc, map);
   }
}

void AttributeAssignment::EnableInsertTypes(BOOL enable) 
{
   GetDlgItem(IDC_INSERTTYPE_LB)->EnableWindow(enable);
}

void AttributeAssignment::FillSelections() 
{
   UpdateData();

   CleanLBMem(&m_qualifierLB);
   m_qualifierLB.ResetContent();
   CleanLBMem(&m_selectionLB);
   m_selectionLB.ResetContent();

   m_selectionText = "";
   m_qualifierText = "";   
   m_assignCount = 0;
   m_deleteCount = 0;

   if (!m_type) // by Item
   {
      switch (m_itemCB.GetCurSel())
      {
      case 0: // Ref Des Prefix
         {
            m_selectionText = "Prefix";
            m_qualifierText = "Ref Des";
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            EnableInsertTypes(TRUE);

            POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
            while (dataPos != NULL)
            {
               DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);

               if (data->getDataType() != T_INSERT) continue;
               if (!InsertFilter(data->getInsert()->getInsertType())) continue;
               if (data->getInsert()->getRefname())
               {
                  CString refDes = data->getInsert()->getRefname();
						int i=0;
                  for (i=0; i<refDes.GetLength(); i++)
                     if (isdigit(refDes[i]))
                        break;

                  CString prefix = refDes.Left(i);
                  if (prefix.IsEmpty()) continue;

                  AddItemToStorageLB(&m_selectionLB, prefix, data);
               }
            }
         }
         break;

      case 1: // Inserts
         {
            m_selectionText = "Geometries";
            m_qualifierText = "Ref Des";
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            EnableInsertTypes(TRUE);

            POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
            while (dataPos != NULL)
            {
               DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);

               if (data->getDataType() != T_INSERT) continue;
               if (!InsertFilter(data->getInsert()->getInsertType())) continue;

               AddItemToStorageLB(&m_selectionLB, doc->Find_Block_by_Num(data->getInsert()->getBlockNumber())->getName(), data);
            }
         }
         break;

      case 2: // Blocks
         {
            m_qualifierText = "Geometry Names";
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            EnableInsertTypes(FALSE);

            for (int i=0; i<doc->getMaxBlockIndex(); i++)
            {
               BlockStruct *block = doc->getBlockAt(i);
               if (block == NULL)   continue;
               if (block->getFlags() & BL_WIDTH 
                  || block->getFlags() & BL_FILE 
                  || block->getFlags() & BL_TOOL
                  || block->getFlags() & BL_COMPLEX_TOOL)
                  continue;
               AddItemToStorageLB(&m_qualifierLB, block->getName(), &block->getAttributesRef());
            }
         }
         break;

      case 3: // Nets
         {
            m_qualifierText = "Net Names";
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
            EnableInsertTypes(FALSE);

            POSITION netPos = pcbFile->getNetList().GetHeadPosition();
            while (netPos != NULL)
            {
               NetStruct *net = pcbFile->getNetList().GetNext(netPos);
               AddItemToStorageLB(&m_qualifierLB, net->getNetName(), &net->getAttributesRef());
            }
         }
         break;

      case 4: // Comp Pins by Net
         {
            m_selectionText = "Net Names";
            m_qualifierText = "Comp/Pin";
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_SHOW);
            EnableInsertTypes(FALSE);

            POSITION netPos = pcbFile->getNetList().GetHeadPosition();
            while (netPos != NULL)
            {
               NetStruct *net = pcbFile->getNetList().GetNext(netPos);
               AddItemToStorageLB(&m_selectionLB, net->getNetName(), net);
            }
         }
         break;

      case 5: // Comp Pins by Geometry
         {
            m_selectionText = "Geometry";
            m_qualifierText = "Comp/Pin";
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_SHOW);
            EnableInsertTypes(FALSE);

            POSITION netPos = pcbFile->getNetList().GetHeadPosition();
            while (netPos != NULL)
            {
               NetStruct *net = pcbFile->getNetList().GetNext(netPos);

               POSITION cpPos = net->getHeadCompPinPosition();
               while (cpPos != NULL)
               {
                  CompPinStruct *cp = net->getNextCompPin(cpPos);
                  if (cp->getPadstackBlockNumber() != -1)
                  {
                     BlockStruct *geom = doc->getBlockAt(cp->getPadstackBlockNumber());

                     if (geom)
                        AddItemToStorageLB(&m_selectionLB, geom->getName(), geom);
                  }
               }
            }
         }
         break;

      case 6: // Types
         {
            m_qualifierText = "Types";
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
            EnableInsertTypes(FALSE);

            POSITION pos = pcbFile->getTypeList().GetHeadPosition();
            while (pos != NULL)
            {
               TypeStruct *type = pcbFile->getTypeList().GetNext(pos);
               AddItemToStorageLB(&m_qualifierLB, type->getName(), &type->getAttributesRef());
            }
         }
         break;

      case 7: // Layers
         {
            m_qualifierText = "Layers";
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
            EnableInsertTypes(FALSE);

            for (int i=0; i<doc->getMaxLayerIndex(); i++)
            {
               LayerStruct *layer = doc->getLayerArray()[i];
               if (layer == NULL)   continue;
               AddItemToStorageLB(&m_qualifierLB, layer->getName(), &layer->getAttributesRef());
            }
         }
         break;

      case 8: // Datas
         {
            m_qualifierText = "Graphic Classes";
            GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
            EnableInsertTypes(FALSE);

            POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
            while (dataPos != NULL)
            {
               DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);

               AddItemToStorageLB(&m_qualifierLB, graphicClassTagToDisplayString(data->getGraphicClass()), &data->getAttributesRef());
            }
         }
         break;

      case CB_ERR:
      default:
         GetDlgItem(IDC_SELECTION)->ShowWindow(SW_HIDE);
         GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_HIDE);
      }
   }

   else // by Attrib
   {
      m_selectionText = "Keyword";
      m_qualifierText = "Value";
      GetDlgItem(IDC_SELECTION)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_QUALIFIER)->ShowWindow(SW_SHOW);

      switch (m_attribCB.GetCurSel())
      {
      case 0: // Inserts
         {
            EnableInsertTypes(TRUE);

            POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
            while (dataPos != NULL)
            {
               DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);
   
               if (data->getDataType() != T_INSERT) continue;
               if (!InsertFilter(data->getInsert()->getInsertType())) continue;
               if (!data->getAttributesRef()) continue;

               FillAttribKeywords(&data->getAttributesRef());
            }
         }
         break;

      case 1: // Blocks
         {
            EnableInsertTypes(FALSE);

            for (int i=0; i<doc->getMaxBlockIndex(); i++)
            {
               BlockStruct *block = doc->getBlockAt(i);
               if (block == NULL)   continue;
               FillAttribKeywords(&block->getAttributesRef());
            }
         }
         break;

      case 2: // Nets
         {
            EnableInsertTypes(FALSE);

            POSITION netPos = pcbFile->getNetList().GetHeadPosition();
            while (netPos != NULL)
            {
               NetStruct *net = pcbFile->getNetList().GetNext(netPos);
               FillAttribKeywords(&net->getAttributesRef());
            }
         }
         break;

      case 3: // Comp Pins
         {
            EnableInsertTypes(FALSE);

            POSITION netPos = pcbFile->getNetList().GetHeadPosition();
            while (netPos != NULL)
            {
               NetStruct *net = pcbFile->getNetList().GetNext(netPos);
               POSITION compPinPos = net->getHeadCompPinPosition();
               while (compPinPos != NULL)
               {
                  CompPinStruct *compPin = net->getNextCompPin(compPinPos);
                  FillAttribKeywords(&compPin->getAttributesRef());
               }
            }
         }
         break;

      case 4: // Types
         {
            EnableInsertTypes(FALSE);

            POSITION pos = pcbFile->getTypeList().GetHeadPosition();
            while (pos != NULL)
            {
               TypeStruct *type = pcbFile->getTypeList().GetNext(pos);
               FillAttribKeywords(&type->getAttributesRef());
            }
         }
         break;

      case 5: // Layers
         {
            EnableInsertTypes(FALSE);

            for (int i=0; i<doc->getMaxLayerIndex(); i++)
            {
               LayerStruct *layer = doc->getLayerArray()[i];
               if (layer == NULL)   continue;
               FillAttribKeywords(&layer->getAttributesRef());
            }
         }
         break;

      case 6: // Datas
         {
            EnableInsertTypes(FALSE);

            POSITION dataPos = pcbFile->getBlock()->getDataList().GetHeadPosition();
            while (dataPos != NULL)
            {
               DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(dataPos);
   
               if (!data->getAttributesRef()) continue;

               FillAttribKeywords(&data->getAttributesRef());
            }
         }
         break;

      }
   }
   
   UpdateData(FALSE);
}

void AttributeAssignment::OnSelchangeQualifier() 
{
   m_assignCount = 0;
   m_deleteCount = 0;

   UpdateData(FALSE);
}

void AttributeAssignment::OnSelchangeSelection() 
{
   CleanLBMem(&m_qualifierLB);
   m_qualifierLB.ResetContent();
   m_assignCount = 0;
   m_deleteCount = 0;

   if (!m_type) // by Item
   {
      switch (m_itemCB.GetCurSel())
      {
      case 0: // Ref Des Prefix
      case 1: // Inserts
         {
            CPtrList* list = (CPtrList*)m_selectionLB.GetItemDataPtr(m_selectionLB.GetCurSel());
            POSITION pos = list->GetHeadPosition();
            while (pos != NULL)
            {
               DataStruct *data = (DataStruct*)list->GetNext(pos);
               if (data->getInsert()->getRefname())
                  AddItemToStorageLB(&m_qualifierLB, data->getInsert()->getRefname(), &data->getAttributesRef());
               else
                  AddItemToStorageLB(&m_qualifierLB, "* No Ref Des *", &data->getAttributesRef());
            }

            m_qualifierLB.SetSel(-1);
         }
         break;

      case 4: // Comp Pins by Net
         {
            CPtrList* list = (CPtrList*)m_selectionLB.GetItemDataPtr(m_selectionLB.GetCurSel());
            NetStruct *net = (NetStruct*)list->GetHead();
            POSITION pos = net->getHeadCompPinPosition();
            while (pos != NULL)
            {
               CompPinStruct *compPin = net->getNextCompPin(pos);
               CString buf;
               buf.Format("%s %s", compPin->getRefDes(), compPin->getPinName());
               AddItemToStorageLB(&m_qualifierLB, buf, &compPin->getAttributesRef());
            }
         }
         break;

      case 5: // Comp Pins by Geometry
         {
            CPtrList* list = (CPtrList*)m_selectionLB.GetItemDataPtr(m_selectionLB.GetCurSel());
            BlockStruct *geom = (BlockStruct*)list->GetHead();
            POSITION netPos = pcbFile->getNetList().GetHeadPosition();
            while (netPos != NULL)
            {
               NetStruct *net = pcbFile->getNetList().GetNext(netPos);

               POSITION cpPos = net->getHeadCompPinPosition();
               while (cpPos != NULL)
               {
                  CompPinStruct *cp = net->getNextCompPin(cpPos);
                  if (cp->getPadstackBlockNumber() == geom->getBlockNumber())
                  {
                     CString buf;
                     buf.Format("%s %s", cp->getRefDes(), cp->getPinName());
                     AddItemToStorageLB(&m_qualifierLB, buf, &cp->getAttributesRef());
                  }
               }
            }
         }
         break;
         
      }
   }

   else // by Attrib
   {
      CString buf;
      m_selectionLB.GetText(m_selectionLB.GetCurSel(), buf); 
      WORD keyword = doc->IsKeyWord(buf, 1);

      CPtrList* list = (CPtrList*)m_selectionLB.GetItemDataPtr(m_selectionLB.GetCurSel());
      POSITION pos = list->GetHeadPosition();
      while (pos != NULL)
      {
         CAttributes** attribMap = (CAttributes**)list->GetNext(pos);
         Attrib* attrib;

         (*attribMap)->Lookup(keyword, attrib);

         switch (attrib->getValueType())
         {
         case VT_STRING:
         case VT_EMAIL_ADDRESS:
         case VT_WEB_ADDRESS:
            buf.Format("\"%s\"", attrib->getStringValue());
            AddItemToStorageLB(&m_qualifierLB, buf, attribMap);
            break;
         case VT_INTEGER:
            buf.Format("%+d", attrib->getIntValue());
            AddItemToStorageLB(&m_qualifierLB, buf, attribMap);
            break;
         case VT_UNIT_DOUBLE:
         case VT_DOUBLE:
            buf.Format("%+.*lf", decimals, attrib->getDoubleValue());
            AddItemToStorageLB(&m_qualifierLB, buf, attribMap);
            break;
         case VT_NONE:
            buf = " * No Value *";
            AddItemToStorageLB(&m_qualifierLB, buf, attribMap);
            break;
         }
      }
   }

   UpdateData(FALSE);
}

void AttributeAssignment::OnDelete() 
{
   UpdateData();
   m_assignCount = 0;
   m_deleteCount = 0;
   UpdateData(FALSE);

   if (m_keyword.IsEmpty())
   {
      ErrorMessage("Select an Attribute to Assign");
      m_keywordCB.ShowDropDown();
      return;
   }

   WORD keyword = doc->IsKeyWord(m_keyword, 0);
   Attrib* attrib;

   int count;
   int *indices;

   count = m_qualifierLB.GetSelCount();

   if (!count)
   {
      ErrorMessage("You must select item(s) in the right listbox.", "Nothing Selected");
      return;
   }

   CWaitCursor wait;
   
   indices = (int*)calloc(count, sizeof(int));
   m_qualifierLB.GetSelItems(count, indices);

   BOOL AskUser = TRUE;

   for (int i=0; i<count; i++)
   {
      CPtrList *list = (CPtrList*)m_qualifierLB.GetItemDataPtr(indices[i]);

      POSITION pos = list->GetHeadPosition();
      while (pos != NULL)
      {
         CAttributes** attribMap = (CAttributes**)list->GetNext(pos);

         if (*attribMap && (*attribMap)->Lookup(keyword, attrib))
         {
            RemoveAttrib(keyword, attribMap);
            m_deleteCount++;
            UpdateData(FALSE);
         }
      }
   }

   free(indices);
}

void AttributeAssignment::OnAssign() 
{
   UpdateData();
   m_assignCount = 0;
   m_deleteCount = 0;
   UpdateData(FALSE);

   if (m_keyword.IsEmpty())
   {
      ErrorMessage("Select an Attribute to Assign");
      m_keywordCB.ShowDropDown();
      return;
   }

   WORD keyword = doc->IsKeyWord(m_keyword, 0);
   int valueType = doc->getKeyWordArray()[keyword]->getValueType();

   void *value = NULL;
   double   valuedouble;   // must be a pointer in scope
   int      valueint;

   if (valueType == VT_UNIT_DOUBLE || valueType == VT_DOUBLE || valueType == VT_INTEGER)
   {
      if (!is_number(m_value))
      {
         ErrorMessage("Numeric Value Required");
         return;
      }
   }

   if (valueType != VT_NONE && valueType != VT_STRING && m_value.IsEmpty())
   {
      ErrorMessage("Value Required");
      return;
   }

   if (!m_value.IsEmpty())
   {
      switch (valueType)
      {
      case VT_STRING:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
         value = m_value.GetBuffer(0);
         break;
      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         {
            valuedouble = atof(m_value);
            value = &valuedouble;
         }
         break;
      case VT_INTEGER:
         {
            valueint = atoi(m_value);
            value = &valueint;
         }
         break;
      case VT_NONE:
         {
            value = NULL;
         }
         break;
      default:
         valueType = VT_STRING;
         value = m_value.GetBuffer(0);
         break;
      }
   }

   if (!m_keyword.Compare(ATT_TEST))
   {
      if (!m_value.CompareNoCase("don't care"))
         value = "";
      else if (!m_value.IsEmpty() && m_value.CompareNoCase("top") && m_value.CompareNoCase("bottom") &&
               m_value.CompareNoCase("none") && m_value.CompareNoCase("both"))
      {
         ErrorMessage("TEST Attribute must have a value of \"BOTH\", \"TOP\", \"BOTTOM\", \"NONE\", or \"DON'T CARE\"", "Illegal Value");
         return;
      }
   }

   int count;
   int *indices;

   count = m_qualifierLB.GetSelCount();
   if (!count)
   {
      ErrorMessage("You must select item(s) in the right listbox.", "Nothing Selected");
      return;
   }

   CWaitCursor wait;
   
   indices = (int*)calloc(count, sizeof(int));
   m_qualifierLB.GetSelItems(count, indices);

   BOOL AskUser = TRUE;
   int method;
   for (int i=0; i<count; i++)
   {
      CPtrList *list = (CPtrList*)m_qualifierLB.GetItemDataPtr(indices[i]);

      POSITION pos = list->GetHeadPosition();
      while (pos != NULL)
      {
         CAttributes** attribMap = (CAttributes**)list->GetNext(pos);

         if (doc->SetAttrib(attribMap, keyword, valueType, value, SA_RETURN, NULL))
         {
            if (valueType != VT_NONE)
            {
               const KeyWordStruct *kw = doc->getKeyWordArray()[keyword];
               Attrib* temp;

               (*attribMap)->Lookup(keyword, temp);

               if (AskUser)
               {
                  OverwriteAttrib overwriteDlg;
                  overwriteDlg.doc = doc;
                  overwriteDlg.attrib = temp;
                  overwriteDlg.valueType = kw->getValueType();
                  overwriteDlg.m_keyword.Format("Keyword : %s", kw->cc);
                  overwriteDlg.DoModal();

                  method = overwriteDlg.method;
                  AskUser = !overwriteDlg.All;
               }

               if (method != SA_RETURN)
               {
                  doc->SetAttrib(attribMap, keyword, valueType, value, method, NULL);
                  m_assignCount++;
                  UpdateData(FALSE);
               }
            }
         }
         else 
         {
            m_assignCount++;
            UpdateData(FALSE);
         }
      }
   }
   free(indices);
}

void AttributeAssignment::OnSelchangeKeywordCb() 
{
   UpdateData();
   m_valueType = "";
   m_assignCount = 0;
   m_deleteCount = 0;

   if (!m_keyword.IsEmpty())
   {
      m_valueType = "Value Type = ";
      switch (doc->getKeyWordArray()[doc->IsKeyWord(m_keyword, 0)]->getValueType())
      {
      case VT_NONE:
         m_valueType += "None";
         m_value = "";
         GetDlgItem(IDC_VALUE)->EnableWindow(FALSE);
         break;
      case VT_STRING:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
         m_valueType += "String";
         GetDlgItem(IDC_VALUE)->EnableWindow(TRUE);
         break;
      case VT_INTEGER:
         m_valueType += "Integer";
         GetDlgItem(IDC_VALUE)->EnableWindow(TRUE);
         break;
      case VT_DOUBLE:
         m_valueType += "Floating Point";
         GetDlgItem(IDC_VALUE)->EnableWindow(TRUE);
         break;
      case VT_UNIT_DOUBLE:
         m_valueType += "Unit-Dependent Floating Point";
         GetDlgItem(IDC_VALUE)->EnableWindow(TRUE);
         break;
      }
   }

   UpdateData(FALSE);
}

