// $Header: /CAMCAD/5.0/Edit.cpp 79    6/17/07 8:52p Kurt Van Ness $

/****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 1998. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include "CCEtoODB.h"
#include "edit.h"
//#include "license.h"
#include "graph.h"
#include "cedit.h"
//#include "crypt.h"                      
//#include "font.h"
//#include "attrib.h"
#include "atrblist.h"
#include "flags.h"
#include "polylib.h"
#include "DcaEnumIterator.h"
#include ".\edit.h"

#ifdef _DEBUG
#define new DEBUG_NEW                           
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NoLayer   "* No Layer *"

BOOL Editing = FALSE;
CEditDialog *editDlg = NULL;
BOOL EditPolyDlg = FALSE;

static BOOL PlacementSet = FALSE;
static WINDOWPLACEMENT editPlacement;
static EditPoint *pointDlg;
static EditBlob *blobDlg;
static EditPoly *polyDlg;
static EditLine *lineDlg;
static EditArc *arcDlg;
static EditCircle *circleDlg;
static EditText *textDlg;
static EditInsert *insertDlg;
static EditApInsert *apInsertDlg;
static EditToolInsert *toolInsertDlg;
/*static*/ EditNothing *nothingDlg = NULL;

void ShowNothingDlg();
void InsertModifyUndo(CCEtoODBDoc *doc, BOOL Head);
bool getShowHiddenAttributes();
void DeleteActiveDlg(CEditDialog *&activeDlg);
void DeleteEditDlg();
void DeleteNothingDlg();

/******************************************************************************
* FillWidthCB
*/
void FillWidthCB(CComboBox *widthCB, short dataWidthIndex, CCEtoODBDoc *doc)
{
   CString     buf;
   BlockStruct *b;
   char        *shape;
   int         decimals = GetDecimals(doc->getSettings().getPageUnits());

   for (int i = 0; i < doc->getNextWidthIndex(); i++)
   {
      if (!(b = doc->getWidthTable()[i]))   
         continue;

      if (b->getFlags() & BL_TOOL || b->getFlags() & BL_BLOCK_TOOL || b->getFlags() & BL_COMPLEX_TOOL)
         continue; // tools have a width but do not use SizeA 

      switch (b->getShape()) 
      {
         case T_ROUND: shape = "Rnd"; break;
         case T_SQUARE: shape = "Sqr"; break;
         case T_RECTANGLE: shape = "Rect"; break;
         default: shape = "other"; break; 
      }

      buf.Format("%s : %.*lf : %s", b->getName(), decimals, b->getSizeA(), shape);
      int index = widthCB->AddString(buf);
      widthCB->SetItemDataPtr(index, b);
      if (i == dataWidthIndex)
         widthCB->SetCurSel(index);
   }
}

/******************************************************************************
* SetWidthIndex
*/
void SetWidthIndex(CComboBox *m_widthCB, short dataWidthIndex, CCEtoODBDoc *doc)
{
	if (dataWidthIndex < 0)
		return;

   for (int i=0; i<m_widthCB->GetCount(); i++)
   {
      BlockStruct *b = (BlockStruct *)m_widthCB->GetItemDataPtr(i);

      if (doc->getWidthTable()[dataWidthIndex] == b)
      {
         m_widthCB->SetCurSel(i);
         return;
      }
   }
}

/******************************************************************************
* GetWidthIndex
*/
// Int is preferred data type for width index now, but too many places
// call with short. Fix those up someday. In meantime, supply indirection.

void GetWidthIndex(CComboBox *m_widthCB, int *dataWidthIndex, CCEtoODBDoc *doc);

void GetWidthIndex(CComboBox *m_widthCB, short *dataWidthIndex, CCEtoODBDoc *doc)
{
   int intWidthIndex = *dataWidthIndex;
   GetWidthIndex(m_widthCB, &intWidthIndex, doc);
   *dataWidthIndex = intWidthIndex;
}

void GetWidthIndex(CComboBox *m_widthCB, int *dataWidthIndex, CCEtoODBDoc *doc)
{
   int   err;

   if (m_widthCB->GetCurSel() != CB_ERR) // if selection
   {
      BlockStruct *b = (BlockStruct *)m_widthCB->GetItemDataPtr(m_widthCB->GetCurSel());
      for (int i = 0; i < doc->getNextWidthIndex(); i++)
      {
         if (doc->getWidthTable()[i] == b)
         {
            *dataWidthIndex = i;
            return;
         }
      }
   }
   else // edit text
   {
      CString buf;
      m_widthCB->GetWindowText(buf);
      if (buf.IsEmpty())
      {
         *dataWidthIndex = 0;
         m_widthCB->SetCurSel(0);
         //SetCurrentWidth(doc->getWidthTable()[0]);
         return;
      }
      if (isalpha(buf[0])) // name, not size
      {
         for (int i = 0; i < doc->getNextWidthIndex(); i++)
         {
            if (!buf.CompareNoCase(doc->getWidthTable()[i]->getName()))
            {
               *dataWidthIndex = i;
               m_widthCB->SetCurSel(i);
               //SetCurrentWidth(doc->getWidthTable()[i]);
               return;
            }
         }
         // create new aperture
         doc->StoreDocForImporting();
         *dataWidthIndex = Graph_Aperture(buf, T_ROUND, 0.0, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE, &err);
         m_widthCB->ResetContent();
         FillWidthCB(m_widthCB, *dataWidthIndex, doc);
         //SetCurrentWidth(doc->getWidthTable()[*dataWidthIndex]);
      }
      else // size
      {
         double size = atof(buf);
         for (int i = 0; i < doc->getNextWidthIndex(); i++)
         {
            if (doc->getWidthTable()[i]->getName().IsEmpty())
            {
               if (size == doc->getWidthTable()[i]->getSizeA())
               {
                  *dataWidthIndex = i;
                  m_widthCB->SetCurSel(i);
                  //SetCurrentWidth(doc->getWidthTable()[i]);
                  return;
               }
            }
         }
         // create new aperture
         doc->StoreDocForImporting();
         *dataWidthIndex = Graph_Aperture("", T_ROUND, size, 0.0, 0.0, 0.0, 0.0, 0, BL_WIDTH, FALSE, &err);
         m_widthCB->ResetContent();
         FillWidthCB(m_widthCB, *dataWidthIndex, doc);
         //SetCurrentWidth(doc->getWidthTable()[*dataWidthIndex]);
      }
   }
}

/******************************************************************************
* FillLayerCB
*/
void FillLayerCB(CComboBox *layerCB, CCEtoODBDoc *doc, FileStruct *f, int layernum)
{
   for (int j=0; j< doc->getMaxLayerIndex(); j++)
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue;
      int i = layerCB->AddString(layer->getName());
      layerCB->SetItemDataPtr(i, layer);
      if (j == layernum)
         layerCB->SetCurSel(i);
   }
}

/******************************************************************************
* FillSpanLayerCB
*/
void FillSpanLayerCB(CComboBox *layerCB, CCEtoODBDoc *doc, FileStruct *f, int layernum)
{
   for (int j=0; j< doc->getMaxLayerIndex(); j++)
   {
      LayerStruct *layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue;
      if(layer->getPhysicalStackNumber() > 0 && 
         layer->getLayerType() != layerTypeStackLevelTop && layer->getLayerType() != layerTypeStackLevelBottom)
      {
         int i = layerCB->AddString(layer->getName());
         layerCB->SetItemDataPtr(i, layer);
         if (layer->getPhysicalStackNumber() == layernum)
            layerCB->SetCurSel(i);
      }
   }
}

/******************************************************************************
* SetAttribStrings
*/
static int j;
void SetAttribStrings(CListCtrl *attribLC, char *keyword, char *value,int instanceCount)
{
   LV_ITEM item;
   int actualItem;

   item.mask = LVIF_TEXT;
   item.iItem = j++;
   item.iSubItem = 0;
   item.pszText = keyword;
   actualItem = attribLC->InsertItem(&item);
   // ugly, dirty way to try to work around microsoft bug
   if (actualItem == -1)
   {
      actualItem = attribLC->InsertItem(&item);
      if (actualItem == -1)
      {
         actualItem = attribLC->InsertItem(&item);
         if (actualItem == -1)
         {
            actualItem = attribLC->InsertItem(&item);
            if (actualItem == -1)
            {
               actualItem = attribLC->InsertItem(&item);
               if (actualItem == -1)
                  actualItem = attribLC->InsertItem(&item);
            }
         }
      }
   }

//    attribLC->SetItemData(actualItem, (LPARAM)p);

   item.iItem = actualItem;

   item.iSubItem = 1;
   item.pszText = value;
   attribLC->SetItem(&item);

   CString instanceCountString;
   instanceCountString.Format("%d",instanceCount);

   item.iSubItem = 2;
   item.pszText = instanceCountString.GetBuffer();
   attribLC->SetItem(&item);
}

/******************************************************************************
* FillAttribLC
*/
void FillAttribLC(CListCtrl *attribLC, CAttributes* map, CCEtoODBDoc *doc)
{
   // columns
   LV_COLUMN column;
   CRect rect;
   attribLC->GetWindowRect(&rect);

   int instanceWidth = 20;

   column.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
   column.pszText = "Keyword";
   column.cx = (rect.Width() - instanceWidth) / 2;
   column.iSubItem = 0;
   attribLC->InsertColumn(0, &column);

   column.pszText = "Value";
   //column.cx *= 2;
   column.iSubItem = 1;
   attribLC->InsertColumn(1, &column);

   column.pszText = "#";
   column.cx = instanceWidth + 5;
   column.iSubItem = 2;
   attribLC->InsertColumn(2, &column);

   WORD word;
   Attrib* attrib;
   int instanceCount;
   CString keyword, value;
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   
   j = 0;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, word, attrib);
      instanceCount = attrib->getCount();
      const KeyWordStruct* keywordStruct = doc->getKeyWordArray()[word];
      keyword = keywordStruct->cc;

      if (keywordStruct->getHidden() && !getShowHiddenAttributes())
      {
         continue;
      }

      switch (attrib->getValueType())
      {
      case VT_NONE:
         value = "";
         break;
      case VT_INTEGER:
         value.Format("%d", attrib->getIntValue());
         break;
      case VT_DOUBLE:
      case VT_UNIT_DOUBLE:
         value.Format("%.*lf", decimals, attrib->getDoubleValue());
         break;
      case VT_STRING:
         {
            if (attrib->getStringValueIndex() == -1)
            {
               value = "\"\"";
            }
            else
            {
               char *tok, *temp = STRDUP(attrib->getStringValue());

               if ((tok = strtok(temp, "\n")) == NULL)
               {
                  value = "\"\"";
               }
               else
               {
                  value.Format("\"%s\"", tok);
                  tok = strtok(NULL, "\n");

                  while (tok != NULL)
                  {
                     value += ",\"";
                     value += tok;
                     value += "\"";
                     tok = strtok(NULL, "\n");
                  }
               }

               free(temp);
            }
         }

         break;
      case VT_EMAIL_ADDRESS:
         value = "";

         if (attrib->getStringValue().Left(7) != "mailto:")
            value = "mailto:";

         if (attrib->getStringValueIndex() != -1)
            value += attrib->getStringValue();

         break;
      case VT_WEB_ADDRESS:
         value = "";

         if (attrib->getStringValue().Left(7) != "http://")
            value = "http://";

         if (attrib->getStringValueIndex() != -1)
            value += attrib->getStringValue();

         break;
      }

      SetAttribStrings(attribLC, keyword.GetBuffer(0), value.GetBuffer(0),instanceCount);
   } // end while associations in map
}

/******************************************************************************
* CloseEditDlg
*/
void CloseEditDlg()
{
   if (editDlg)
      editDlg->OnCancel();
}

void StopEditing()
{
   CloseEditDlg();
   Editing = false;
}

//_____________________________________________________________________________
CHorizontalPositionComboBox::CHorizontalPositionComboBox()
{
   m_initialized = false;
}

bool CHorizontalPositionComboBox::initialized()
{
   if (!m_initialized && IsWindow(*this))
   {
      m_initialized = true;

      int index;

      index = AddString(horizontalPositionTagToFriendlyString(horizontalPositionLeft));
      SetItemData(index,horizontalPositionLeft);

      index = AddString(horizontalPositionTagToFriendlyString(horizontalPositionCenter));
      SetItemData(index,horizontalPositionCenter);

      index = AddString(horizontalPositionTagToFriendlyString(horizontalPositionRight));
      SetItemData(index,horizontalPositionRight);
   }

   return m_initialized;
}

HorizontalPositionTag CHorizontalPositionComboBox::getValue()
{
   HorizontalPositionTag retval = horizontalPositionLeft;

   if (initialized())
   {
      int curSel = GetCurSel();

      if (curSel >= 0)
      {
         int value = GetItemData(curSel);
         retval = intToHorizontalPositionTag(value);
      }
   }

   return retval;
}

void CHorizontalPositionComboBox::setValue(HorizontalPositionTag value)
{
   if (initialized())
   {
      for (int index = 0;;index++)
      {
         if (index < GetCount())
         {
            if (GetItemData(index) == value)
            {
               SetCurSel(index);
               break;
            }
         }
         else
         {
            SetCurSel(-1);
            break;
         }
      }
   }
}

//_____________________________________________________________________________
CVerticalPositionComboBox::CVerticalPositionComboBox()
{
   m_initialized = false;
}

bool CVerticalPositionComboBox::initialized()
{
   if (!m_initialized && IsWindow(*this))
   {
      m_initialized = true;

      int index;

      index = AddString(verticalPositionTagToFriendlyString(verticalPositionBaseline));
      SetItemData(index,verticalPositionBaseline);

      index = AddString(verticalPositionTagToFriendlyString(verticalPositionTop));
      SetItemData(index,verticalPositionTop);

      index = AddString(verticalPositionTagToFriendlyString(verticalPositionCenter));
      SetItemData(index,verticalPositionCenter);

      index = AddString(verticalPositionTagToFriendlyString(verticalPositionBottom));
      SetItemData(index,verticalPositionBottom);
   }

   return m_initialized;
}

VerticalPositionTag CVerticalPositionComboBox::getValue()
{
   VerticalPositionTag retval = verticalPositionBaseline;

   if (initialized())
   {
      int curSel = GetCurSel();

      if (curSel >= 0)
      {
         int value = GetItemData(curSel);
         retval = intToVerticalPositionTag(value);
      }
   }

   return retval;
}

void CVerticalPositionComboBox::setValue(VerticalPositionTag value)
{
   if (initialized())
   {
      for (int index = 0;;index++)
      {
         if (index < GetCount())
         {
            if (GetItemData(index) == value)
            {
               SetCurSel(index);
               break;
            }
         }
         else
         {
            SetCurSel(-1);
            break;
         }
      }
   }
}

/******************************************************************************
* OnEditEntity 
*/
void CCEtoODBDoc::OnEditEntity() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Query!");
      return;
   }*/
#ifndef SHAREWARE 

   Editing = TRUE;

   if (nothingIsSelected())
   {
      ShowNothingDlg();
      return;
   }

   SelectStruct *s = getSelectStack().getAtLevel();

   HWND active = ::GetActiveWindow();

   EditPolyDlg = FALSE;

   CString title;
   title.Format(" [%ld]", s->getData()->getEntityNumber());

   if (s->getData() != NULL)
   {
      switch (s->getData()->getDataType())
      {
      case T_POINT:
         {
            if (editDlg)
            {
               DeleteEditDlg();
            }
            else if (nothingDlg)
            {
               DeleteNothingDlg();
            }

            pointDlg = new EditPoint(*this);
            pointDlg->s = s;
            //pointDlg->doc = this;
            pointDlg->Create(IDD_EDIT_POINT);

            if (PlacementSet)
            {
               CRect rect;
               pointDlg->GetWindowRect(rect);
               editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                  rect.bottom - rect.top;
               pointDlg->SetWindowPlacement(&editPlacement);
            }
            else
            {
               pointDlg->ShowWindow(SW_SHOW);
            }

            editDlg = pointDlg;
            title.Insert(0, "Point");
            editDlg->SetWindowText(title);
            pointDlg->ShowWindow(SW_SHOWNA);
            break;
         }

      case T_BLOB:
         {
            if (editDlg)
            {
               DeleteEditDlg();
            }
            else if (nothingDlg)
            {
               DeleteNothingDlg();
            }

            blobDlg = new EditBlob(*this);
            blobDlg->s = s;
            //blobDlg->doc = this;
            blobDlg->Create(IDD_EDIT_BLOB);

            if (PlacementSet)
            {
               CRect rect;
               blobDlg->GetWindowRect(rect);
               editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                  rect.bottom - rect.top;
               blobDlg->SetWindowPlacement(&editPlacement);
            }
            else
            {
               blobDlg->ShowWindow(SW_SHOW);
            }

            editDlg = blobDlg;
            title.Insert(0, "Blob");
            editDlg->SetWindowText(title);
            blobDlg->ShowWindow(SW_SHOWNA);
            break;
         }

      case T_POLY:
         {
            if (editDlg)
            {
               DeleteEditDlg();
            }
            else if (nothingDlg)
            {
               DeleteNothingDlg();
            }

            if (s->getData()->getPolyList()->GetCount() == 1)
            {
               CPoly *poly = s->getData()->getPolyList()->GetHead();

               if (poly->getPntList().GetCount() == 2) // line or arc
               {
                  CPnt *pnt = poly->getPntList().GetHead();

                  if (fabs(pnt->bulge) > SMALLNUMBER) // arc
                  {
                     arcDlg = new EditArc(*this);
                     arcDlg->s = s;
                     //arcDlg->doc = this;
                     arcDlg->poly = poly;
                     arcDlg->Create(IDD_EDIT_ARC);

                     if (PlacementSet)
                     {
                        CRect rect;
                        arcDlg->GetWindowRect(rect);
                        editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                           rect.bottom - rect.top;
                        arcDlg->SetWindowPlacement(&editPlacement);
                     }
                     else
                     {
                        arcDlg->ShowWindow(SW_SHOW);
                     }

                     editDlg = arcDlg;
                     title.Insert(0, "Arc");
                     editDlg->SetWindowText(title);
                     arcDlg->ShowWindow(SW_SHOWNA);
                  }
                  
                  else // line
                  {
                     lineDlg = new EditLine(*this);
                     lineDlg->s = s;
                     //lineDlg->doc = this;
                     lineDlg->poly = poly;
                     lineDlg->Create(IDD_EDIT_LINE);

                     if (PlacementSet)
                     {
                        CRect rect;
                        lineDlg->GetWindowRect(rect);
                        editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                           rect.bottom - rect.top;
                        lineDlg->SetWindowPlacement(&editPlacement);
                     }
                     else
                     {
                        lineDlg->ShowWindow(SW_SHOW);
                     }

                     editDlg = lineDlg;
                     title.Insert(0, "Line");
                     editDlg->SetWindowText(title);
                     lineDlg->ShowWindow(SW_SHOWNA);
                  }
                  
                  break;
               }
      
               double cx, cy, radius;

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  circleDlg = new EditCircle(*this);
                  circleDlg->s = s;
                  //circleDlg->doc = this;

                  int decimals = GetDecimals(getSettings().getPageUnits());
                  circleDlg->m_x.Format("%+.*lf", decimals, cx);
                  circleDlg->m_y.Format("%+.*lf", decimals, cy);
                  circleDlg->m_radius.Format("%.*lf", decimals, radius);

                  circleDlg->poly = poly;

                  circleDlg->Create(IDD_EDIT_CIRCLE);

                  if (PlacementSet)
                  {
                     CRect rect;
                     circleDlg->GetWindowRect(rect);
                     editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                        rect.bottom - rect.top;
                     circleDlg->SetWindowPlacement(&editPlacement);
                  }
                  else
                  {
                     circleDlg->ShowWindow(SW_SHOW);
                  }

                  editDlg = circleDlg;
                  title.Insert(0, "Circle");
                  editDlg->SetWindowText(title);
                  circleDlg->ShowWindow(SW_SHOWNA);
                  break;
               }
            }

            polyDlg = new EditPoly(*this);
            polyDlg->s = s;
            //polyDlg->doc = this;
            polyDlg->Create(IDD_EDIT_POLYSTRUCT);

            if (PlacementSet)
            {
               CRect rect;
               polyDlg->GetWindowRect(rect);
               editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                  rect.bottom - rect.top;
               polyDlg->SetWindowPlacement(&editPlacement);
            }
            else
            {
               polyDlg->ShowWindow(SW_SHOW);
            }

            editDlg = polyDlg;
            title.Insert(0, "Poly");
            editDlg->SetWindowText(title);
            EditPolyDlg = TRUE;

            break;
         }

      case T_TEXT:
         {
            if (editDlg)
            {
               DeleteEditDlg();
            }
            else if (nothingDlg)
            {
               DeleteNothingDlg();
            }

            textDlg = new EditText(*this);
            textDlg->s = s;
            //textDlg->doc = this;
            textDlg->Create(IDD_EDIT_TEXT);

            if (PlacementSet)
            {
               CRect rect;
               textDlg->GetWindowRect(rect);
               editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                  rect.bottom - rect.top;
               textDlg->SetWindowPlacement(&editPlacement);
            }
            else
            {
               textDlg->ShowWindow(SW_SHOW);
            }

            editDlg = textDlg;
            title.Insert(0, "Text");
            editDlg->SetWindowText(title);
            textDlg->ShowWindow(SW_SHOWNA);
            break;
         }

      case T_INSERT:
         {
            BlockStruct *block = Find_Block_by_Num(s->getData()->getInsert()->getBlockNumber());

            if (block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL)
            {
               if (editDlg)
               {
                  DeleteEditDlg();
               }
               else if (nothingDlg)
               {
                  DeleteNothingDlg();
               }

               toolInsertDlg = new EditToolInsert(*this);
               toolInsertDlg->s = s;
               //toolInsertDlg->doc = this;
               toolInsertDlg->Create(IDD_EDIT_TOOL_INSERT);

               if (PlacementSet)
               {
                  CRect rect;
                  toolInsertDlg->GetWindowRect(rect);
                  editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                     rect.bottom - rect.top;
                  toolInsertDlg->SetWindowPlacement(&editPlacement);
               }
               else
               {
                  toolInsertDlg->ShowWindow(SW_SHOW);
               }

               editDlg = toolInsertDlg;
               title.Insert(0, "Tool");
               editDlg->SetWindowText(title);
               toolInsertDlg->ShowWindow(SW_SHOWNA);
            }
            else if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
            {
               if (editDlg)
               {
                  DeleteEditDlg();
               }
               else if (nothingDlg)
               {
                  DeleteNothingDlg();
               }

               apInsertDlg = new EditApInsert(*this);
               apInsertDlg->s = s;
               //apInsertDlg->doc = this;
               apInsertDlg->Create(IDD_EDIT_AP_INSERT);

               if (PlacementSet)
               {
                  CRect rect;
                  apInsertDlg->GetWindowRect(rect);
                  editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                     rect.bottom - rect.top;
                  apInsertDlg->SetWindowPlacement(&editPlacement);
               }
               else
               {
                  apInsertDlg->ShowWindow(SW_SHOW);
               }

               editDlg = apInsertDlg;
               title.Insert(0, "Aperture");
               editDlg->SetWindowText(title);
               apInsertDlg->ShowWindow(SW_SHOWNA);
            }
            else // inserted block
            {
               if (editDlg)
               {
                  DeleteEditDlg();
               }
               else if (nothingDlg)
               {
                  DeleteNothingDlg();
               }

               insertDlg = new EditInsert(*this);
               insertDlg->m_selectStruct = s;
               //insertDlg->doc = this;
               insertDlg->Create(IDD_EDIT_INSERT);

               if (PlacementSet)
               {
                  CRect rect;
                  insertDlg->GetWindowRect(rect);
                  editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
                     rect.bottom - rect.top;
                  insertDlg->SetWindowPlacement(&editPlacement);
               }
               else
               {
                  insertDlg->ShowWindow(SW_SHOW);
               }

               editDlg = insertDlg;
               title.Insert(0, "Insert");
               editDlg->SetWindowText(title);
               insertDlg->ShowWindow(SW_SHOWNA);
            }
            break;
         }

      } // end switch
   }

   ::SetActiveWindow(active);

#endif
}

void DeleteActiveDlg(CEditDialog *&activeDlg)
{
   if(activeDlg)
   {
      activeDlg->GetWindowPlacement(&editPlacement);
      PlacementSet = TRUE;
      if(activeDlg->IsWindowEnabled())
         activeDlg->DestroyWindow();

      delete activeDlg;
      activeDlg = NULL;
   }
}

void DeleteEditDlg()
{
   bool isEqualDlg = (nothingDlg == editDlg);
   DeleteActiveDlg(editDlg);
   if(isEqualDlg) nothingDlg = NULL;
}

void DeleteNothingDlg()
{
   DeleteActiveDlg((CEditDialog *&)nothingDlg);
}

void ShowNothingDlg()
{
   HWND active = ::GetActiveWindow();

   if (editDlg)
   {
      DeleteEditDlg();
   }

   CCEtoODBView* activeView = getActiveView();

   if (activeView != NULL)
   {
      nothingDlg = new EditNothing(*(activeView->GetDocument()));
      nothingDlg->Create(IDD_EDIT_NOTHING);

      if (PlacementSet)
      {
         CRect rect;
         nothingDlg->GetWindowRect(rect);
         editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
            rect.bottom - rect.top;
         nothingDlg->SetWindowPlacement(&editPlacement);
      }
      else
      {
         nothingDlg->ShowWindow(SW_SHOW);
      }

      editDlg = nothingDlg;
      EditPolyDlg = FALSE;
      nothingDlg->ShowWindow(SW_SHOWNA);
   }

   FlushQueue();

   ::SetActiveWindow(active);
}

/******************************************************************************
* IsLicensed
*/
static BOOL IsLicensed(CCEtoODBDoc *doc, SelectStruct *s) 
{
   /*if (getApp().getCamcadLicense().isLicensed(camcadProductPrint))
      return TRUE;*/

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRedline)) 
   {
      if (s->getData()->getDataType() == T_INSERT || doc->getLayerArray()[s->getData()->getLayerIndex()]->getLayerType() != LAYTYPE_REDLINE)
      {
         ErrorMessage("Can not edit Non-Redline entities.", "This is a Non-Redline entity.");
         return FALSE;
      }
      return TRUE;
   }*/

   ErrorAccess("You do not have a License to Edit!");
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// EditPoint dialog
EditPoint::EditPoint(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditPoint::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditPoint)
   m_x = _T("");
   m_y = _T("");
   m_negative = FALSE;
   m_class = -1;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC,anchorLeft,growBoth);
   addFieldControl(IDC_LAYER_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_CLASS   ,anchorLeft,growHorizontal);
}

void EditPoint::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditPoint)
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_CLASS, m_classCB);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_CBIndex(pDX, IDC_CLASS, m_class);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditPoint, CEditDialog)
   //{{AFX_MSG_MAP(EditPoint)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   ON_BN_CLICKED(IDC_FLAGS, OnFlags)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditPoint::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      }
      return TRUE;
   }

   return FALSE;
}

BOOL EditPoint::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      m_classCB.AddString(graphicClassTagToDisplayString(grclass));
   }

   m_class = s->getData()->getGraphicClass();

   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());
   m_x.Format("%+.*lf", decimals, s->getData()->getPoint()->x);
   m_y.Format("%+.*lf", decimals, s->getData()->getPoint()->y);

   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);
      if (f->getFileNumber() == s->filenum)
         break;
   }

   FillLayerCB(&m_layerCB,&getCamCadDoc(), f, s->getData()->getLayerIndex());
   
   m_negative = s->getData()->isNegative();

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditPoint::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

void EditPoint::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   s->getData()->setLayerIndex(((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())))->getLayerIndex());

   s->getData()->getPoint()->x = (DbUnit)atof(m_x);
   s->getData()->getPoint()->y = (DbUnit)atof(m_y);

   s->getData()->setNegative(m_negative);
   s->getData()->setGraphicClass(m_class);

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
}

//_____________________________________________________________________________
CEditDialog::CEditDialog(UINT nIDTemplate,CCEtoODBDoc& camCadDoc,CWnd* pParent)
: CResizingDialog(nIDTemplate, pParent)
, m_camCadDoc(camCadDoc)
{
}

bool CEditDialog::isEditable(SelectStruct& selectStruct)
{
   bool retval = true;

   if (getApp().getRunMode() == runModeDataProtectedModelessDialog)
   {
      retval = false;
   }
   /*else if (getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      retval = true;
   }*/
   /*else if (getApp().getCamcadLicense().isLicensed(camcadProductRedline)) 
   {
      if (selectStruct.getData()->getDataType() == T_INSERT || 
          getCamCadDoc().getLayerArray()[selectStruct.getData()->getLayerIndex()]->getLayerType() != LAYTYPE_REDLINE)
      {
         ErrorMessage("Can not edit Non-Redline entities.", "This is a Non-Redline entity.");

         retval = false;
      }

      retval = true;
   }*/
   else
   {
      ErrorAccess("You do not have a License to Edit!");

      retval = false;
   }

   return retval;
}

void CEditDialog::OnCancel()
{
   CResizingDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// EditLine dialog
EditLine::EditLine(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditLine::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditLine)
   m_endX = _T("");
   m_endY = _T("");
   m_startX = _T("");
   m_startY = _T("");
   m_negative = FALSE;
   m_class = -1;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC,anchorLeft,growBoth);
   addFieldControl(IDC_LAYER_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_WIDTH_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_CLASS   ,anchorLeft,growHorizontal);
}

void EditLine::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditLine)
   DDX_Control(pDX, IDC_CLASS, m_classCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Control(pDX, IDC_WIDTH_CB, m_widthCB);
   DDX_Text(pDX, IDC_END_X, m_endX);
   DDX_Text(pDX, IDC_END_Y, m_endY);
   DDX_Text(pDX, IDC_START_X, m_startX);
   DDX_Text(pDX, IDC_START_Y, m_startY);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_CBIndex(pDX, IDC_CLASS, m_class);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditLine, CEditDialog)
   //{{AFX_MSG_MAP(EditLine)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   ON_BN_CLICKED(IDC_FLAGS, OnFlags)
   ON_BN_CLICKED(IDC_POLYLINE, OnPolyline)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditLine::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      }
      return TRUE;
   }

   return FALSE;
}

BOOL EditLine::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      m_classCB.AddString(graphicClassTagToDisplayString(grclass));
   }

   m_class = s->getData()->getGraphicClass();

   CPnt *pnt1 = poly->getPntList().GetHead(), *pnt2 = poly->getPntList().GetTail();

   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());
   m_startX.Format("%+.*lf", decimals, pnt1->x);
   m_startY.Format("%+.*lf", decimals, pnt1->y);
   m_endX.Format("%+.*lf", decimals, pnt2->x);
   m_endY.Format("%+.*lf", decimals, pnt2->y);

   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);
      if (f->getFileNumber() == s->filenum)
         break;
   }

   FillLayerCB(&m_layerCB,&getCamCadDoc(), f, s->getData()->getLayerIndex());
   
   FillWidthCB(&m_widthCB, poly->getWidthIndex(),&getCamCadDoc());

   m_negative = s->getData()->isNegative();

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditLine::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   GetWidthIndex(&m_widthCB, &poly->getWidthIndexRef(),&getCamCadDoc());
   s->getData()->setLayerIndex(((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())))->getLayerIndex());

   CPnt *pnt1 = poly->getPntList().GetHead(), *pnt2 = poly->getPntList().GetTail();
   pnt1->x = (DbUnit)atof(m_startX);
   pnt1->y = (DbUnit)atof(m_startY);
   pnt2->x = (DbUnit)atof(m_endX);
   pnt2->y = (DbUnit)atof(m_endY);

   s->getData()->setNegative(m_negative);
   s->getData()->setGraphicClass(m_class);

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
}

void EditLine::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

void EditLine::OnPolyline() 
{
   lineDlg->GetWindowPlacement(&editPlacement);
   PlacementSet = TRUE;
   
   polyDlg = new EditPoly(getCamCadDoc());
   polyDlg->s = s;
   //polyDlg->doc = &getCamCadDoc();
   polyDlg->Create(IDD_EDIT_POLYSTRUCT);
   CRect rect;
   polyDlg->GetWindowRect(rect);
   editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
      rect.bottom - rect.top;
   polyDlg->SetWindowPlacement(&editPlacement);
   editDlg = polyDlg;
   polyDlg->ShowWindow(SW_SHOWNA);
   EditPolyDlg = TRUE;

   lineDlg->DestroyWindow();

   delete lineDlg;  
   lineDlg = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// EditArc dialog
EditArc::EditArc(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditArc::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditArc)
   m_centerX = _T("");
   m_centerY = _T("");
   m_delta = _T("");
   m_radius = _T("");
   m_start = _T("");
   m_negative = FALSE;
   m_class = -1;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC,anchorLeft,growBoth);
   addFieldControl(IDC_LAYER_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_WIDTH_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_CLASS   ,anchorLeft,growHorizontal);
}

void EditArc::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditArc)
   DDX_Control(pDX, IDC_CLASS, m_classCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_WIDTH_CB, m_widthCB);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Text(pDX, IDC_CENTER_X, m_centerX);
   DDX_Text(pDX, IDC_CENTER_Y, m_centerY);
   DDX_Text(pDX, IDC_DELTA, m_delta);
   DDX_Text(pDX, IDC_RADIUS, m_radius);
   DDX_Text(pDX, IDC_START, m_start);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_CBIndex(pDX, IDC_CLASS, m_class);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditArc, CEditDialog)
   //{{AFX_MSG_MAP(EditArc)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   ON_BN_CLICKED(IDC_POLYLINE, OnEditAsPolyline)
   ON_BN_CLICKED(IDC_FLAGS, OnFlags)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditArc::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      }
      return TRUE;
   }

   return FALSE;
}

BOOL EditArc::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      m_classCB.AddString(graphicClassTagToDisplayString(grclass));
   }

   m_class = s->getData()->getGraphicClass();

   CPnt *pnt1 = poly->getPntList().GetHead(), *pnt2 = poly->getPntList().GetTail();
   double da, sa, r, cx, cy;
   da = atan(pnt1->bulge) * 4;
   ArcPoint2Angle(pnt1->x, pnt1->y, pnt2->x, pnt2->y, da, &cx, &cy, &r, &sa);

   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());
   m_start.Format("%.1lf", RadToDeg(sa));
   m_delta.Format("%.1lf", RadToDeg(da));
   m_centerX.Format("%+.*lf", decimals, cx);
   m_centerY.Format("%+.*lf", decimals, cy);
   m_radius.Format("%.*lf", decimals, r);
   
   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);
      if (f->getFileNumber() == s->filenum)
         break;
   }

   FillLayerCB(&m_layerCB,&getCamCadDoc(), f, s->getData()->getLayerIndex());
   
   FillWidthCB(&m_widthCB, poly->getWidthIndex(),&getCamCadDoc());

   m_negative = s->getData()->isNegative();

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditArc::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   GetWidthIndex(&m_widthCB, &poly->getWidthIndexRef(),&getCamCadDoc());
   s->getData()->setLayerIndex(((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())))->getLayerIndex());

   double cx, cy, start, delta, radius;
   cx = (DbUnit)atof(m_centerX);
   cy = (DbUnit)atof(m_centerY);
   start = (DbUnit)DegToRad(atof(m_start));
   delta = (DbUnit)DegToRad(atof(m_delta));
   radius = (DbUnit)atof(m_radius);

   CPnt *pnt1 = poly->getPntList().GetHead(), *pnt2 = poly->getPntList().GetTail();
   pnt1->bulge = (DbUnit)tan(delta/4);
   pnt1->x = (DbUnit)(cx + radius * cos(start));
   pnt1->y = (DbUnit)(cy + radius * sin(start));
   pnt2->x = (DbUnit)(cx + radius * cos(start + delta));
   pnt2->y = (DbUnit)(cy + radius * sin(start + delta));

   s->getData()->setNegative(m_negative);
   s->getData()->setGraphicClass(m_class);

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
}

void EditArc::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

void EditArc::OnEditAsPolyline() 
{
   arcDlg->GetWindowPlacement(&editPlacement);
   PlacementSet = TRUE;
   
   polyDlg = new EditPoly(getCamCadDoc());
   polyDlg->s = s;
   //polyDlg->doc = doc;
   polyDlg->Create(IDD_EDIT_POLYSTRUCT);
   CRect rect;
   polyDlg->GetWindowRect(rect);
   editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
      rect.bottom - rect.top;
   polyDlg->SetWindowPlacement(&editPlacement);
   editDlg = polyDlg;
   polyDlg->ShowWindow(SW_SHOWNA);
   EditPolyDlg = TRUE;

   arcDlg->DestroyWindow();

   delete arcDlg;
   arcDlg = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// EditInsert dialog
EditInsert::EditInsert(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditInsert::IDD,camCadDoc, pParent)
, m_documentAngle(_T(""))
{
   //{{AFX_DATA_INIT(EditInsert)
   m_angle = _T("");
   m_x = _T("");
   m_y = _T("");
   m_documentX = _T("");
   m_documentY = _T("");
   m_refname = _T("");
   m_type = -1;
   m_top_bottom = -1;
   m_scale = _T("");
   m_negative = FALSE;
   m_mirrorLayers = FALSE;
   m_mirrorFlip = FALSE;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC,anchorLeft,growBoth);
   addFieldControl(IDC_ComboBox,anchorLeft,growHorizontal);
   addFieldControl(IDC_LAYER_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_REFNAME ,anchorLeft,growHorizontal);
   addFieldControl(IDC_TYPE    ,anchorLeft,growHorizontal);
}

EditInsert::~EditInsert()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void EditInsert::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditInsert)
   DDX_Control(pDX, IDC_TYPE, m_typeCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Control(pDX, IDC_BLOCK, m_block);
   DDX_Text(pDX, IDC_ANGLE, m_angle);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_DocumentX, m_documentX);
   DDX_Text(pDX, IDC_DocumentY, m_documentY);
   DDX_Text(pDX, IDC_DocumentAngle, m_documentAngle);
   DDX_Text(pDX, IDC_REFNAME, m_refname);
   DDX_CBIndex(pDX, IDC_TYPE, m_type);
   DDX_Radio(pDX, IDC_TOP, m_top_bottom);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_Check(pDX, IDC_MIRROR_LAYERS, m_mirrorLayers);
   DDX_Check(pDX, IDC_MIRROR_FLIP, m_mirrorFlip);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditInsert, CEditDialog)
   //{{AFX_MSG_MAP(EditInsert)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_BN_CLICKED(IDC_TOP, OnTop)
   ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   ON_BN_CLICKED(IDC_FLAGS, OnFlags)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditInsert::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   UpdateData();
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      case IDC_BLOCK:
         m_block.GetLBText(m_block.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      case IDC_REFNAME:
         pTTT->lpszText = m_refname.GetBuffer(0);
         break;
      case IDC_X:
         pTTT->lpszText = m_x.GetBuffer(0);
         break;
      case IDC_Y:
         pTTT->lpszText = m_y.GetBuffer(0);
         break;
      case IDC_DocumentX:
         pTTT->lpszText = m_documentX.GetBuffer(0);
         break;
      case IDC_DocumentY:
         pTTT->lpszText = m_documentY.GetBuffer(0);
         break;
      case IDC_DocumentAngle:
         pTTT->lpszText = m_documentAngle.GetBuffer(0);
         break;
      case IDC_ANGLE:
         pTTT->lpszText = m_angle.GetBuffer(0);
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

BOOL EditInsert::OnInitDialog() 
{
   CEditDialog::OnInitDialog();

   // tooltips
   m_tooltip.Create(this, TTS_ALWAYSTIP);
   m_tooltip.Activate(TRUE);
   m_tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   m_tooltip.AddTool(GetDlgItem(IDC_BLOCK), LPSTR_TEXTCALLBACK);
   m_tooltip.AddTool(GetDlgItem(IDC_REFNAME), LPSTR_TEXTCALLBACK);
   m_tooltip.AddTool(GetDlgItem(IDC_X), LPSTR_TEXTCALLBACK);
   m_tooltip.AddTool(GetDlgItem(IDC_Y), LPSTR_TEXTCALLBACK);
   m_tooltip.AddTool(GetDlgItem(IDC_ANGLE), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &m_tooltip;

   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      m_typeCB.AddString(insertTypeToDisplayString(insertType));
   }

   m_type = m_selectStruct->getData()->getInsert()->getInsertType();

   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());

   CBasesVector documentInsertOrigin;
   CTMatrix matrix = getCamCadDoc().getSelectStack().getTMatrixForLevelIndex();
   documentInsertOrigin.transform(matrix);

   m_documentX.Format("%+.*lf", decimals, documentInsertOrigin.getOrigin().x);
   m_documentY.Format("%+.*lf", decimals, documentInsertOrigin.getOrigin().y);
   m_documentAngle.Format("%+.*lf%s", decimals, normalizeDegrees(documentInsertOrigin.getRotationDegrees()),
      documentInsertOrigin.getMirror() ? " mirror" : "");

   m_x.Format("%+.*lf", decimals, m_selectStruct->getData()->getInsert()->getOriginX());
   m_y.Format("%+.*lf", decimals, m_selectStruct->getData()->getInsert()->getOriginY());
   m_angle.Format("%.1lf", normalizeDegrees(m_selectStruct->getData()->getInsert()->getAngleDegrees()));
   m_scale.Format("%.*lf", decimals, m_selectStruct->getData()->getInsert()->getScale());
   m_mirrorFlip = ((m_selectStruct->getData()->getInsert()->getMirrorFlags() & MIRROR_FLIP)?1:0);
   m_mirrorLayers = ((m_selectStruct->getData()->getInsert()->getMirrorFlags() & MIRROR_LAYERS)?1:0);
   m_top_bottom = m_selectStruct->getData()->getInsert()->getPlacedBottom();
   m_negative = m_selectStruct->getData()->isNegative();

   if (m_selectStruct->getData()->getInsert()->getRefname())
      m_refname = m_selectStruct->getData()->getInsert()->getRefname();
   
   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);
      if (f->getFileNumber() == m_selectStruct->filenum)
         break;
   }

   int i = m_layerCB.AddString(NoLayer);
   m_layerCB.SetItemDataPtr(i, NULL);
   m_layerCB.SetCurSel(i);
   FillLayerCB(&m_layerCB, &getCamCadDoc(), f, m_selectStruct->getData()->getLayerIndex());
   
   for (i=0; i < getCamCadDoc().getMaxBlockIndex(); i++)
   {
      BlockStruct *block = getCamCadDoc().getBlockAt(i);
      if (block == NULL)   continue;

      // not widths nor this file block (infinite recursion)
      if(!(block->getFlags() & BL_WIDTH) && (block != f->getBlock()))
      {
         int index = m_block.AddString(block->getName());
         m_block.SetItemDataPtr(index, block);

         if (m_selectStruct->getData()->getInsert()->getBlockNumber() == block->getBlockNumber())
            m_block.SetCurSel(index);
      }
   }

   if (m_selectStruct->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, m_selectStruct->getData()->getAttributesRef(), &getCamCadDoc());

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditInsert::OnTop() 
{
   UpdateData();
   m_mirrorFlip = FALSE;
   m_mirrorLayers = FALSE;
   UpdateData(FALSE);
}

void EditInsert::OnBottom() 
{
   UpdateData();
   m_mirrorFlip = TRUE;
   m_mirrorLayers = TRUE;
   UpdateData(FALSE);
}

void EditInsert::OnModify() 
{
   if (!isEditable(*m_selectStruct))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(m_selectStruct, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   LayerStruct *layer = ((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())));

   if (layer)
      m_selectStruct->getData()->setLayerIndex(layer->getLayerIndex());
   else
      m_selectStruct->getData()->setLayerIndex(-1);

   m_selectStruct->getData()->getInsert()->setOriginX(atof(m_x));
   m_selectStruct->getData()->getInsert()->setOriginY(atof(m_y));
   m_selectStruct->getData()->getInsert()->setAngle(DegToRad(atof(m_angle)));
   m_selectStruct->getData()->getInsert()->setScale(atof(m_scale));
   m_selectStruct->getData()->getInsert()->setMirrorFlags((m_mirrorFlip?MIRROR_FLIP:0) | (m_mirrorLayers?MIRROR_LAYERS:0));
   m_selectStruct->getData()->getInsert()->setPlacedBottom(m_top_bottom);
   m_selectStruct->getData()->getInsert()->setInsertType(m_type);
   m_selectStruct->getData()->setNegative(m_negative);

   if (m_refname.IsEmpty())
   {
      m_selectStruct->getData()->getInsert()->setRefname(NULL);
   }
   else
   {
      m_selectStruct->getData()->getInsert()->setRefname(STRDUP(m_refname));
   }

/* CString blockName;
   m_block.GetLBText(m_block.GetCurSel(), blockName);
   BlockStruct *b;
   POSITION pos = m_camCadDoc.BlockList.GetHeadPosition();
   while (pos != NULL)
   {
      b = m_camCadDoc.BlockList.GetNext(pos);
      if (!blockName.Compare(b->name))
      {
         s->p->getInsert().num = b->num;
         break;
      }
   }*/
   int sel = m_block.GetCurSel();

   if (sel != CB_ERR)
      m_selectStruct->getData()->getInsert()->setBlockNumber( ((BlockStruct *)(m_block.GetItemDataPtr(sel)))->getBlockNumber() );

   getCamCadDoc().DrawEntity(m_selectStruct, 1, FALSE);
#endif
}

void EditInsert::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// EditCircle dialog
EditCircle::EditCircle(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditCircle::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditCircle)
   m_radius = _T("");
   m_x = _T("");
   m_y = _T("");
   m_negative = FALSE;
   m_filled = FALSE;
   m_class = -1;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC,anchorLeft,growBoth);
   addFieldControl(IDC_LAYER_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_WIDTH_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_CLASS   ,anchorLeft,growHorizontal);
}

void EditCircle::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditCircle)
   DDX_Control(pDX, IDC_CLASS, m_classCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_WIDTH_CB, m_widthCB);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Text(pDX, IDC_RADIUS, m_radius);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_Check(pDX, IDC_FILLED, m_filled);
   DDX_CBIndex(pDX, IDC_CLASS, m_class);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditCircle, CEditDialog)
   //{{AFX_MSG_MAP(EditCircle)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   ON_BN_CLICKED(IDC_POLYLINE, OnEditAsPolyline)
   ON_BN_CLICKED(IDC_FLAGS, OnFlags)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditCircle::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

BOOL EditCircle::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      m_classCB.AddString(graphicClassTagToDisplayString(grclass));
   }

   m_class = s->getData()->getGraphicClass();

   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);
      if (f->getFileNumber() == s->filenum)
         break;
   }

   FillLayerCB(&m_layerCB,&getCamCadDoc(), f, s->getData()->getLayerIndex());
   
   FillWidthCB(&m_widthCB, poly->getWidthIndex(),&getCamCadDoc());

   m_filled = poly->isFilled();

   m_negative = s->getData()->isNegative();

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditCircle::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   GetWidthIndex(&m_widthCB, &poly->getWidthIndexRef(),&getCamCadDoc());
   s->getData()->setLayerIndex(((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())))->getLayerIndex());

   CPnt *p1, *p2, *p3;
   p1 = poly->getPntList().GetHead();
   POSITION pos = poly->getPntList().GetHeadPosition();
   poly->getPntList().GetNext(pos);
   p2 = poly->getPntList().GetAt(pos);
   p3 = poly->getPntList().GetTail();

   double cx, cy, radius;
   cx = atof(m_x);
   cy = atof(m_y);
   radius = atof(m_radius);

   p1->x = p2->x = p3->x = (DbUnit)cx;
   p1->y = p3->y = (DbUnit)(cy + radius);
   p2->y = (DbUnit)(cy - radius);

   poly->setFilled(m_filled);

   s->getData()->setNegative(m_negative);
   s->getData()->setGraphicClass(m_class);

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
}

void EditCircle::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

void EditCircle::OnEditAsPolyline() 
{
   circleDlg->GetWindowPlacement(&editPlacement);
   PlacementSet = TRUE;
   
   polyDlg = new EditPoly(getCamCadDoc());
   polyDlg->s = s;
   //polyDlg->doc = doc;
   polyDlg->Create(IDD_EDIT_POLYSTRUCT);
   CRect rect;
   polyDlg->GetWindowRect(rect);
   editPlacement.rcNormalPosition.bottom = editPlacement.rcNormalPosition.top + 
      rect.bottom - rect.top;
   polyDlg->SetWindowPlacement(&editPlacement);
   editDlg = polyDlg;
   polyDlg->ShowWindow(SW_SHOWNA);
   EditPolyDlg = TRUE;

   circleDlg->DestroyWindow();

   delete circleDlg;
   circleDlg = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// EditText dialog
EditText::EditText(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditText::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditText)
   m_height = _T("");
   m_angle = _T("");
   m_mirror = FALSE;
   m_proportional = FALSE;
   m_text = _T("");
   m_x = _T("");
   m_y = _T("");
   m_oblique = 0;
   m_negative = FALSE;
   m_class = -1;
   m_width = _T("");
   m_neverMirror = FALSE;
   //}}AFX_DATA_INIT

   CResizingDialogField *field,*keyField,*labelField,*textField;

   // proportional and mirror
   keyField = &(addFieldControl(IDC_PROPORTIONAL,anchorProportionalVertical));

   addFieldControl(IDC_MIRROR,anchorRelativeToField,keyField);

   // text
   textField = &(addFieldControl(IDC_TEXT    ,anchorLeft,growHorizontal));
   textField->getOperations().addOperation(glueBottomEdge   ,toTopEdge,keyField);

   // font
   labelField = &(addFieldControl(IDC_STATIC1,anchorRelativeToField,keyField));
   field      = &(addFieldControl(IDC_FONT   ,anchorRelativeToField,keyField,growHorizontal));
   field->getOperations().addOperation(glueRightEdge   ,toRightEdge ,textField);
   field->getOperations().addOperation(glueLeftEdge    ,toRightEdge ,labelField);

   // x and y
   addFieldControl(IDC_STATIC2,anchorRelativeToField,keyField);
   addFieldControl(IDC_X      ,anchorRelativeToField,keyField);
   addFieldControl(IDC_STATIC3,anchorRelativeToField,keyField);
   addFieldControl(IDC_Y      ,anchorRelativeToField,keyField);

   // height and width
   addFieldControl(IDC_STATIC4,anchorRelativeToField,keyField);
   addFieldControl(IDC_HEIGHT ,anchorRelativeToField,keyField);
   addFieldControl(IDC_STATIC5,anchorRelativeToField,keyField);
   addFieldControl(IDC_WIDTH  ,anchorRelativeToField,keyField);

   // angle and oblique
   addFieldControl(IDC_STATIC6,anchorRelativeToField,keyField);
   addFieldControl(IDC_ANGLE  ,anchorRelativeToField,keyField);
   addFieldControl(IDC_STATIC7,anchorRelativeToField,keyField);
   addFieldControl(IDC_OBLIQUE,anchorRelativeToField,keyField);

   // negative and never mirror
   addFieldControl(IDC_NEGATIVE    ,anchorRelativeToField,keyField);
   addFieldControl(IDC_NEVER_MIRROR,anchorRelativeToField,keyField);

   // anchor position
   addFieldControl(IDC_STATIC11                ,anchorRelativeToField,keyField);
   addFieldControl(IDC_STATIC12                ,anchorRelativeToField,keyField);
   addFieldControl(IDC_HorizontalAnchorPosition,anchorRelativeToField,keyField);
   addFieldControl(IDC_STATIC13                ,anchorRelativeToField,keyField);
   addFieldControl(IDC_VerticalAnchorPosition  ,anchorRelativeToField,keyField);

   // layer
   labelField = &(addFieldControl(IDC_STATIC8 ,anchorRelativeToField,keyField));
   field      = &(addFieldControl(IDC_LAYER_CB,anchorRelativeToField,keyField));
   field->getOperations().addOperation(glueRightEdge   ,toRightEdge ,textField);
   field->getOperations().addOperation(glueLeftEdge    ,toRightEdge ,labelField);

   // width
   addFieldControl(IDC_STATIC9 ,anchorRelativeToField,keyField);
   field      = &(addFieldControl(IDC_WIDTH_CB,anchorRelativeToField,keyField,growHorizontal));
   field->getOperations().addOperation(glueRightEdge   ,toRightEdge ,textField);
   field->getOperations().addOperation(glueLeftEdge    ,toRightEdge ,labelField);

   // class
   addFieldControl(IDC_STATIC10,anchorRelativeToField,keyField);
   field      = &(addFieldControl(IDC_CLASS   ,anchorRelativeToField,keyField,growHorizontal));
   field->getOperations().addOperation(glueRightEdge   ,toRightEdge ,textField);
   field->getOperations().addOperation(glueLeftEdge    ,toRightEdge ,labelField);

   // apply changes and attributes buttons
   addFieldControl(IDC_MODIFY ,anchorRelativeToField,keyField);
   addFieldControl(IDC_ATTRIBS,anchorRelativeToField,keyField);

   // attributes
   field = &(addFieldControl(IDC_ATTRIBLC,anchorBottomLeft,growHorizontal));
   field->getOperations().addOperation(glueTopEdge   ,toBottomEdge,keyField);
}

EditText::~EditText()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void EditText::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditText)
   DDX_Control(pDX, IDC_TEXT, m_textCtrl);
   DDX_Control(pDX, IDC_CLASS, m_classCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_FONT, m_fontCB);
   DDX_Control(pDX, IDC_WIDTH_CB, m_widthCB);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Text(pDX, IDC_HEIGHT, m_height);
   DDX_Text(pDX, IDC_ANGLE, m_angle);
   DDX_Check(pDX, IDC_MIRROR, m_mirror);
   DDX_Check(pDX, IDC_PROPORTIONAL, m_proportional);
   DDX_Text(pDX, IDC_TEXT, m_text);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_OBLIQUE, m_oblique);
   DDV_MinMaxInt(pDX, m_oblique, -75, 75);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_CBIndex(pDX, IDC_CLASS, m_class);
   DDX_Text(pDX, IDC_WIDTH, m_width);
   DDX_Check(pDX, IDC_NEVER_MIRROR, m_neverMirror);
   DDX_Control(pDX, IDC_HorizontalAnchorPosition, m_horizontalAnchorPosition);
   DDX_Control(pDX, IDC_VerticalAnchorPosition, m_verticalAnchorPosition);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditText, CEditDialog)
   //{{AFX_MSG_MAP(EditText)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditText::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      case IDC_FONT:
         m_fontCB.GetLBText(m_fontCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      case IDC_X:
         pTTT->lpszText = m_x.GetBuffer(0);
         break;
      case IDC_Y:
         pTTT->lpszText = m_y.GetBuffer(0);
         break;
      case IDC_HEIGHT:
         pTTT->lpszText = m_height.GetBuffer(0);
         break;
      case IDC_WIDTH:
         pTTT->lpszText = m_width.GetBuffer(0);
         break;
      case IDC_ANGLE:
         pTTT->lpszText = m_angle.GetBuffer(0);
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

BOOL EditText::OnInitDialog() 
{
   CEditDialog::OnInitDialog();

   m_textCtrl.FmtLines(FALSE);

   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_FONT), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_X), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_Y), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_HEIGHT), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_WIDTH), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_ANGLE), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;
   
   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      m_classCB.AddString(graphicClassTagToDisplayString(grclass));
   }

   m_class = s->getData()->getGraphicClass();

   POSITION pos;
   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());
   m_text = s->getData()->getText()->getText();
   CString oldLF, newLF;
   oldLF.Format("%c", '\n');
   newLF.Format("%c%c", 13, 10);
   m_text.Replace(oldLF, newLF);

   m_height.Format("%.*lf", decimals, s->getData()->getText()->getHeight());
   m_width.Format("%.*lf", decimals, s->getData()->getText()->getWidth());
   m_x.Format("%+.*lf", decimals, s->getData()->getText()->getPnt().x);
   m_y.Format("%+.*lf", decimals, s->getData()->getText()->getPnt().y);
   m_angle.Format("%.1lf", normalizeDegrees(RadToDeg(s->getData()->getText()->getRotation())));
	m_oblique = s->getData()->getText()->getOblique();
   m_mirror = s->getData()->getText()->isMirrored();
   m_neverMirror = s->getData()->getText()->getMirrorDisabled();
   m_proportional = s->getData()->getText()->isProportionallySpaced();

   pos = CFontList::getFontList().GetHeadPosition();

   while (pos != NULL)
   {
      m_fontCB.AddString(CFontList::getFontList().GetNext(pos)->getFontName());
   }

   m_fontCB.SetCurSel(s->getData()->getText()->getFontNumber());

   FileStruct *f;
   pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);
      if (f->getFileNumber() == s->filenum)
         break;
   }

   FillLayerCB(&m_layerCB,&getCamCadDoc(),f, s->getData()->getLayerIndex());
   
   FillWidthCB(&m_widthCB, s->getData()->getText()->getPenWidthIndex(),&getCamCadDoc());

   m_negative = s->getData()->isNegative();

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   m_horizontalAnchorPosition.setValue(s->getData()->getText()->getHorizontalPosition());
   m_verticalAnchorPosition.setValue(s->getData()->getText()->getVerticalPosition());

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditText::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   if (m_oblique > 75)
      m_oblique = 75;
   if (m_oblique < -75)
      m_oblique = -75;

   UpdateData(FALSE);

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   short penWidthIndex;
   GetWidthIndex(&m_widthCB, &penWidthIndex,&getCamCadDoc());
   s->getData()->getText()->setPenWidthIndex(penWidthIndex);

   s->getData()->setLayerIndex(((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())))->getLayerIndex());

   CString oldLF, newLF;
   newLF.Format("%c", '\n');
   oldLF.Format("%c%c", 13, 10);
   m_text.Replace(oldLF, newLF);
   s->getData()->getText()->setText(STRDUP(m_text));
   
   s->getData()->getText()->setHeight(atof(m_height));
   s->getData()->getText()->setWidth(atof(m_width));
   s->getData()->getText()->setPnt((DbUnit)atof(m_x),(DbUnit)atof(m_y));
   s->getData()->getText()->setRotation(DegToRad(atof(m_angle)));
   s->getData()->getText()->setOblique(m_oblique);
   s->getData()->getText()->setMirrored(m_mirror);
   s->getData()->getText()->setMirrorDisabled(m_neverMirror);
   s->getData()->getText()->setProportionalSpacing(m_proportional);
   s->getData()->getText()->setFontNumber(m_fontCB.GetCurSel());

   s->getData()->getText()->setHorizontalPosition(m_horizontalAnchorPosition.getValue());
   s->getData()->getText()->setVerticalPosition(m_verticalAnchorPosition.getValue());

   s->getData()->setNegative(m_negative);
   s->getData()->setGraphicClass(m_class);

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
}

void EditText::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// EditApInsert dialog
EditApInsert::EditApInsert(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditApInsert::IDD,camCadDoc, pParent)
, m_documentX(_T(""))
, m_documentY(_T(""))
{
   //{{AFX_DATA_INIT(EditApInsert)
   m_x = _T("");
   m_y = _T("");
   m_sizea = _T("");
   m_sizeb = _T("");
   m_angle = _T("");
   m_rot = _T("");
   m_type = -1;
   m_negative = FALSE;
   m_refname = _T("");

   m_top_bottom = -1;
   m_mirrorLayers = FALSE;
   m_mirrorFlip = FALSE;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC  ,anchorLeft,growBoth);
   addFieldControl(IDC_GeometryCB,anchorLeft,growHorizontal);
   addFieldControl(IDC_TYPE      ,anchorLeft,growHorizontal);
   addFieldControl(IDC_LAYER_CB  ,anchorLeft,growHorizontal);
   addFieldControl(IDC_SHAPE     ,anchorLeft,growHorizontal);
   addFieldControl(IDC_DefinitionGroup,anchorLeft,growHorizontal);
}

EditApInsert::~EditApInsert()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void EditApInsert::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditApInsert)
   DDX_Control(pDX, IDC_TYPE, m_typeCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_SHAPE, m_shapeCB);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Control(pDX, IDC_BLOCK, m_block);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_DocumentX, m_documentX);
   DDX_Text(pDX, IDC_DocumentY, m_documentY);
   DDX_Text(pDX, IDC_SIZEA, m_sizea);
   DDX_Text(pDX, IDC_SIZEB, m_sizeb);
   DDX_Text(pDX, IDC_ANGLE, m_angle);
   DDX_Text(pDX, IDC_ROT, m_rot);
   DDX_CBIndex(pDX, IDC_TYPE, m_type);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_Text(pDX, IDC_REFNAME, m_refname);

   DDX_Radio(pDX, IDC_TOP, m_top_bottom);
   DDX_Check(pDX, IDC_MIRROR_LAYERS, m_mirrorLayers);
   DDX_Check(pDX, IDC_MIRROR_FLIP, m_mirrorFlip);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditApInsert, CEditDialog)
   //{{AFX_MSG_MAP(EditApInsert)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_TOP, OnTop)
   ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
   ON_CBN_SELCHANGE(IDC_BLOCK, OnSelchangeAp)
   ON_CBN_SELCHANGE(IDC_SHAPE, OnSelchangeShape)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditApInsert::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         {
            int sel = m_layerCB.GetCurSel();
            if (sel != CB_ERR)
            {
               m_layerCB.GetLBText(sel, hint);
               pTTT->lpszText = hint.GetBuffer(0);
            }
         }
         break;
      case IDC_BLOCK:
         {
            int sel = m_block.GetCurSel();
            if (sel != CB_ERR)
            {
               m_block.GetLBText(sel, hint);
               pTTT->lpszText = hint.GetBuffer(0);
            }
         }
         break;
      case IDC_REFNAME:
         pTTT->lpszText = m_refname.GetBuffer(0);
         break;
      case IDC_X:
         pTTT->lpszText = m_x.GetBuffer(0);
         break;
      case IDC_Y:
         pTTT->lpszText = m_y.GetBuffer(0);
         break;
      case IDC_ANGLE:
         pTTT->lpszText = m_angle.GetBuffer(0);
         break;
      case IDC_SHAPE:
         m_shapeCB.GetLBText(m_shapeCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      case IDC_SIZEA:
         pTTT->lpszText = m_sizea.GetBuffer(0);
         break;
      case IDC_SIZEB:
         pTTT->lpszText = m_sizeb.GetBuffer(0);
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

BOOL EditApInsert::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_BLOCK), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_REFNAME), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_X), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_Y), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_ANGLE), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_SHAPE), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_SIZEA), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_SIZEB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      m_typeCB.AddString(insertTypeToDisplayString(insertType));
   }

   m_type = s->getData()->getInsert()->getInsertType();

   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());

   CPoint2d documentInsertOrigin;
   CTMatrix matrix = getCamCadDoc().getSelectStack().getTMatrixForLevelIndex();
   matrix.transform(documentInsertOrigin);
   m_documentX.Format("%+.*lf", decimals, documentInsertOrigin.x);
   m_documentY.Format("%+.*lf", decimals, documentInsertOrigin.y);

   m_x.Format("%+.*lf", decimals, s->getData()->getInsert()->getOriginX());
   m_y.Format("%+.*lf", decimals, s->getData()->getInsert()->getOriginY());
   m_angle.Format("%.2lf", normalizeDegrees(s->getData()->getInsert()->getAngleDegrees()));
	m_negative = s->getData()->isNegative();
   m_mirrorFlip = ((s->getData()->getInsert()->getMirrorFlags() & MIRROR_FLIP)?1:0);
   m_mirrorLayers = ((s->getData()->getInsert()->getMirrorFlags() & MIRROR_LAYERS)?1:0);
   m_top_bottom = s->getData()->getInsert()->getPlacedBottom();

   if (s->getData()->getInsert()->getRefname())
      m_refname = s->getData()->getInsert()->getRefname();
   
   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);
      if (f->getFileNumber() == s->filenum)
         break;
   }

   int i = m_layerCB.AddString(NoLayer);
   m_layerCB.SetItemDataPtr(i, NULL);
   m_layerCB.SetCurSel(i);
   FillLayerCB(&m_layerCB,&getCamCadDoc(), f, s->getData()->getLayerIndex());

   for (i = 0; i < MAX_SHAPES; i++)
      m_shapeCB.AddString(shapes[i]);

   block = NULL;

   for (i=0; i < getCamCadDoc().getMaxBlockIndex(); i++)
   {
      BlockStruct *tempBlock = getCamCadDoc().getBlockAt(i);

      if (tempBlock == NULL)  continue;

      if (s->getData()->getInsert()->getBlockNumber() == tempBlock->getBlockNumber())
         block = tempBlock;

      if ( ((tempBlock->getFileNumber() == s->filenum || tempBlock->getFileNumber() == -1) && 
           (tempBlock->getFlags() & BL_BLOCK_APERTURE)) || (tempBlock->getFlags() & BL_APERTURE) )
      {
         if (!tempBlock->getName().IsEmpty())
         {
            int index = m_block.AddString(tempBlock->getName());
            m_block.SetItemDataPtr(index, tempBlock);

            if (s->getData()->getInsert()->getBlockNumber() == tempBlock->getBlockNumber())
               m_block.SetCurSel(index);
         }
      }
   }

   shape = block->getShape();

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   FillApData();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditApInsert::FillApData()
{
   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());

   m_sizea.Format("%.*lf", decimals, block->getSizeA());
   m_sizeb.Format("%.*lf", decimals, block->getSizeB());
   m_rot.Format("%.2lf", RadToDeg(block->getRotation()));
   m_shapeCB.SetCurSel(block->getShape());

   HideSizes();

   UpdateData(FALSE);
}

void EditApInsert::HideSizes()
{
   switch (shape)
   {
   case T_COMPLEX:
      GetDlgItem(IDC_SIZEA)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_SIZEB)->ShowWindow(SW_HIDE);
      break;
   case T_ROUND:
   case T_SQUARE:
   case T_TARGET:
      GetDlgItem(IDC_SIZEA)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZEB)->ShowWindow(SW_HIDE);
      break;
   default:
      GetDlgItem(IDC_SIZEA)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_SIZEB)->ShowWindow(SW_SHOW);
      break;
   }
}

void EditApInsert::OnSelchangeAp() 
{
   block = (BlockStruct*)m_block.GetItemDataPtr(m_block.GetCurSel());

   shape = block->getShape();

   HideSizes();

   FillApData();
}

void EditApInsert::OnSelchangeShape() 
{
   shape = m_shapeCB.GetCurSel();

   HideSizes();
}

void EditApInsert::OnTop() 
{
   UpdateData();
   m_mirrorFlip = FALSE;
   m_mirrorLayers = FALSE;
   UpdateData(FALSE);
}

void EditApInsert::OnBottom() 
{
   UpdateData();
   m_mirrorFlip = TRUE;
   m_mirrorLayers = TRUE;
   UpdateData(FALSE);
}

void EditApInsert::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE

   SelectStruct tmp;
   FileStruct *file;
   DataStruct *data;
   POSITION filePos, dataPos;

   UpdateData();

   // Case 1661, old smallDiff calc always got 0.0
	// Correct smallDIff would be one half of next lesser magnitude
	// than number of decimal places kept. Standard rounding principles.
   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());
	double smallDiff = 5.0 / pow(10.0, (decimals + 1));

   // do we need to update all other inserts of this block?
   BOOL BlockModified = (fabs(block->getSizeA()-atof(m_sizea)) > smallDiff || 
         fabs(block->getSizeB()-atof(m_sizeb)) > smallDiff ||
         fabs(block->getRotation()-DegToRad(atof(m_rot))) > 0.01 ||
         block->getShape() != shape);

   // warn about modifying aperture modifies all inserts of this aperture
   if (BlockModified)
   {
      if (ErrorMessage("Modifying size or shape modifies the aperture and, therefore, all inserts of the aperture\nIf you just want to change this instance, hit CANCEL and pick a different aperture.", 
            "WARNING: Modifying all inserts of this aperture!", 
            MB_OKCANCEL | MB_ICONEXCLAMATION) != IDOK)
      {
         // reset data
         FillApData();
         return;
      }
   }

   // erase all inserts of this aperture
   getCamCadDoc().DrawEntity(s, -1, FALSE);

   if (BlockModified)
   {
      filePos = getCamCadDoc().getFileList().GetHeadPosition();

      while (filePos != NULL)
      {
         file = getCamCadDoc().getFileList().GetNext(filePos);

         if (!file->isShown()) continue;

         tmp.insert_x = file->getInsertX();
         tmp.insert_y = file->getInsertY();
         tmp.scale = file->getScale();
         tmp.rotation = file->getRotation();
         tmp.mirror = file->isMirrored();

         dataPos = file->getBlock()->getDataList().GetHeadPosition();

         while (dataPos != NULL)
         {
            data = file->getBlock()->getDataList().GetNext(dataPos);

            if (data->getDataType() == T_INSERT)
            {
               if (data->getInsert()->getBlockNumber() == block->getBlockNumber())
               {
                  tmp.setData(data);
                  getCamCadDoc().DrawEntity(&tmp, -1, FALSE);
               }
            }
         }
      }
   }

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   LayerStruct *layer = ((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())));

   if (layer)
      s->getData()->setLayerIndex(layer->getLayerIndex());
   else
      s->getData()->setLayerIndex(-1);

   s->getData()->getInsert()->setOriginX(atof(m_x));
   s->getData()->getInsert()->setOriginY(atof(m_y));
   s->getData()->getInsert()->setAngle(DegToRad(atof(m_angle)));
   s->getData()->getInsert()->setInsertType(m_type);
   s->getData()->setNegative(m_negative);
   s->getData()->getInsert()->setMirrorFlags((m_mirrorFlip?MIRROR_FLIP:0) | (m_mirrorLayers?MIRROR_LAYERS:0));
   s->getData()->getInsert()->setPlacedBottom(m_top_bottom);

   if (m_refname.IsEmpty())
   {
      s->getData()->getInsert()->setRefname(NULL);
   }
   else
   {
      s->getData()->getInsert()->setRefname(STRDUP(m_refname));
   }

   s->getData()->getInsert()->setBlockNumber(block->getBlockNumber());

   block->setShape(shape);

   if (shape != T_COMPLEX)
   {
      block->setSizeA((DbUnit)atof(m_sizea));
      block->setSizeB((DbUnit)atof(m_sizeb));
   }

   block->setRotation((DbUnit)(DegToRad(atof(m_rot))));

   // redraw all inserts of this aperture
   if (BlockModified)
   {
      filePos = getCamCadDoc().getFileList().GetHeadPosition();

      while (filePos != NULL)
      {
         file = getCamCadDoc().getFileList().GetNext(filePos);

         if (!file->isShown()) continue;

         tmp.insert_x = file->getInsertX();
         tmp.insert_y = file->getInsertY();
         tmp.scale = file->getScale();
         tmp.rotation = file->getRotation();
         tmp.mirror = file->isMirrored();

         dataPos = file->getBlock()->getDataList().GetHeadPosition();

         while (dataPos != NULL)
         {
            data = file->getBlock()->getDataList().GetNext(dataPos);

            if (data->getDataType() == T_INSERT)
            {
               if (data->getInsert()->getBlockNumber() == block->getBlockNumber())
               {
                  tmp.setData(data);
                  getCamCadDoc().DrawEntity(&tmp, (data->isMarked() ? 2 : 0), FALSE);
               }
            }
         }
      }
   }

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
}

void EditApInsert::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// EditToolInsert dialog
EditToolInsert::EditToolInsert(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditToolInsert::IDD,camCadDoc, pParent)
, m_plated(FALSE)
, m_toolsize(_T(""))
, m_documentX(_T(""))
, m_documentY(_T(""))
{
   //{{AFX_DATA_INIT(EditToolInsert)
   m_x = _T("");
   m_y = _T("");
   m_type = -1;
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC,anchorLeft,growBoth);
   addFieldControl(IDC_ToolName,anchorLeft,growHorizontal);
   addFieldControl(IDC_LAYER_CB,anchorLeft,growHorizontal);
   addFieldControl(IDC_TYPE    ,anchorLeft,growHorizontal);
}

EditToolInsert::~EditToolInsert()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void EditToolInsert::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditToolInsert)
   DDX_Control(pDX, IDC_TYPE, m_typeCB);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Control(pDX, IDC_BLOCK, m_block);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Check(pDX, IDC_PLATED, m_plated);
   DDX_Text(pDX, IDC_TOOLSIZE, m_toolsize);
   DDX_Text(pDX, IDC_DocumentX, m_documentX);
   DDX_Text(pDX, IDC_DocumentY, m_documentY);
   DDX_CBIndex(pDX, IDC_TYPE, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditToolInsert, CEditDialog)
   //{{AFX_MSG_MAP(EditToolInsert)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_NOTIFY_EX(TTN_NEEDTEXT, 0, UpdateToolTip)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditToolInsert::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      case IDC_BLOCK:
         m_block.GetLBText(m_block.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      case IDC_X:
         pTTT->lpszText = m_x.GetBuffer(0);
         break;
      case IDC_Y:
         pTTT->lpszText = m_y.GetBuffer(0);
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// EditToolInsert message handlers
BOOL EditToolInsert::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_BLOCK), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_X), LPSTR_TEXTCALLBACK);
   tooltip.AddTool(GetDlgItem(IDC_Y), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      m_typeCB.AddString(insertTypeToDisplayString(insertType));
   }

   m_type = s->getData()->getInsert()->getInsertType();

   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());

   CPoint2d documentInsertOrigin;
   CTMatrix matrix = getCamCadDoc().getSelectStack().getTMatrixForLevelIndex();
   matrix.transform(documentInsertOrigin);
   m_documentX.Format("%+.*lf", decimals, documentInsertOrigin.x);
   m_documentY.Format("%+.*lf", decimals, documentInsertOrigin.y);

   m_x.Format("%+.*lf", decimals, s->getData()->getInsert()->getOriginX());
   m_y.Format("%+.*lf", decimals, s->getData()->getInsert()->getOriginY());

   BlockStruct *block = getCamCadDoc().getBlockAt(s->getData()->getInsert()->getBlockNumber());
   m_plated = block->getToolHolePlated();
   m_toolsize.Format("%.*lf", decimals, block->getToolSize());
   
   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);

      if (f->getFileNumber() == s->filenum)
         break;
   }

   FillLayerCB(&m_layerCB,&getCamCadDoc(), f, s->getData()->getLayerIndex());

   for (int i=0; i < getCamCadDoc().getNextWidthIndex(); i++)
   {
      BlockStruct *block = getCamCadDoc().getWidthTable()[i];

      if (block == NULL)   continue;

      if (!(block->getFlags() & BL_TOOL) && !(block->getFlags() & BL_BLOCK_TOOL)) continue;

      int index = m_block.AddString(block->getName());
      m_block.SetItemData(index, i);

      if (block->getBlockNumber() == s->getData()->getInsert()->getBlockNumber())
         m_block.SetCurSel(index);
   }

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   GetDlgItem(IDC_PLATED)->EnableWindow(FALSE);
   GetDlgItem(IDC_TOOLSIZE)->EnableWindow(FALSE);

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditToolInsert::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

   s->getData()->getInsert()->setBlockNumber(getCamCadDoc().getWidthTable()[m_block.GetItemData(m_block.GetCurSel())]->getBlockNumber() );
   s->getData()->setLayerIndex(((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())))->getLayerIndex());
   s->getData()->getInsert()->setOriginX(atof(m_x));
   s->getData()->getInsert()->setOriginY(atof(m_y));
   s->getData()->getInsert()->setInsertType(m_type);  

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
}

void EditToolInsert::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// EditNothing dialog
EditNothing::EditNothing(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditNothing::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditNothing)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void EditNothing::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditNothing)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditNothing, CEditDialog)
   //{{AFX_MSG_MAP(EditNothing)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void EditNothing::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// EditPoly dialog
#define WM_DRAW_POLY    WM_USER + 100

EditPoly::EditPoly(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditPoly::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditPoly)
   m_negative = FALSE;
   m_class = -1;
   //}}AFX_DATA_INIT

   CResizingDialogField& anchorField = addFieldControl(IDC_MODIFY      ,anchorProportionalVertical);
   addFieldControl(IDC_EDIT_POLY_ITEM,anchorRelativeToField,&anchorField);
   addFieldControl(IDC_STATIC2       ,anchorRelativeToField,&anchorField);
   addFieldControl(IDC_STATIC3       ,anchorRelativeToField,&anchorField);
   addFieldControl(IDC_STATIC_START  ,anchorRelativeToField,&anchorField);
   addFieldControl(IDC_STATIC_END    ,anchorRelativeToField,&anchorField);

   addFieldControl(IDC_NEGATIVE      ,anchorRelativeToField,&anchorField);
   addFieldControl(IDC_ATTRIBS       ,anchorRelativeToField,&anchorField);
   addFieldControl(IDC_SPANLAYER     ,anchorRelativeToField,&anchorField);

   CResizingDialogField& treeField = addFieldControl(IDC_TREE      ,anchorLeft,growHorizontal);
   treeField.getOperations().addOperation(glueBottomEdge,toTopEdge,&anchorField);

   CResizingDialogField& attribField = addFieldControl(IDC_ATTRIBLC,anchorBottom,growHorizontal);
   attribField.getOperations().addOperation(glueTopEdge,toBottomEdge,&anchorField);

   CResizingDialogField& f1 = addFieldControl(IDC_LAYER_CB      ,anchorRelativeToField,&anchorField);
   f1.getOperations().addOperation(glueRightEdge,toRightEdge,&treeField);

   CResizingDialogField& f2 = addFieldControl(IDC_WIDTH_CB      ,anchorRelativeToField,&anchorField);
   f2.getOperations().addOperation(glueRightEdge,toRightEdge,&treeField);

   CResizingDialogField& f3 = addFieldControl(IDC_CLASS         ,anchorRelativeToField,&anchorField);
   f3.getOperations().addOperation(glueRightEdge,toRightEdge,&treeField);

   CResizingDialogField& f4 = addFieldControl(IDC_STARTLAYER_CB        ,anchorRelativeToField,&anchorField);
   f4.getOperations().addOperation(glueRightEdge,toRightEdge,&treeField);

   CResizingDialogField& f5 = addFieldControl(IDC_ENDLAYER_CB        ,anchorRelativeToField,&anchorField);
   f5.getOperations().addOperation(glueRightEdge,toRightEdge,&treeField);

   decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());
}

EditPoly::~EditPoly()
{
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = NULL;
   app->m_gpToolTip = NULL;
}

void EditPoly::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditPoly)
   DDX_Control(pDX, IDC_TREE, m_tree);
   DDX_Control(pDX, IDC_CLASS, m_classCB);
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Control(pDX, IDC_WIDTH_CB, m_widthCB);
   DDX_Control(pDX, IDC_LAYER_CB, m_layerCB);
   DDX_Control(pDX, IDC_STARTLAYER_CB, m_startlayerCB);
   DDX_Control(pDX, IDC_ENDLAYER_CB, m_endlayerCB);
   DDX_Control(pDX, IDC_SPANLAYER, m_chkSpanlayer);
   DDX_Check(pDX, IDC_NEGATIVE, m_negative);
   DDX_Check(pDX, IDC_SPANLAYER, m_spanlayer);
   DDX_CBIndex(pDX, IDC_CLASS, m_class);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditPoly, CEditDialog)
   //{{AFX_MSG_MAP(EditPoly)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_BN_CLICKED(IDC_EDIT_POLY_ITEM, OnEditPolyItem)
   ON_NOTIFY(NM_CLICK, IDC_TREE, OnClickTree)
   ON_MESSAGE(WM_DRAW_POLY, OnDrawPolyItem)
   ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
   ON_BN_CLICKED(IDC_SPANLAYER, OnBnClickedSpanlayer)
   ON_CBN_SELCHANGE(IDC_CLASS, OnCbnSelchangeClass)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditPoly::UpdateToolTip(UINT id, NMHDR * pNotifyStruct, LRESULT *result)
{
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNotifyStruct;    

   if (pTTT->uFlags & TTF_IDISHWND)    
   {
      // idFrom is actually the HWND of the tool
      UINT nID = ::GetDlgCtrlID((HWND)pNotifyStruct->idFrom);        

      switch (nID)
      {
      case IDC_LAYER_CB:
         m_layerCB.GetLBText(m_layerCB.GetCurSel(), hint);
         pTTT->lpszText = hint.GetBuffer(0);
         break;
      }
   
      return TRUE;
   }

   return FALSE;
}

BOOL EditPoly::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   // tooltips
   tooltip.Create(this, TTS_ALWAYSTIP);
   tooltip.Activate(TRUE);
   tooltip.AddTool(GetDlgItem(IDC_LAYER_CB), LPSTR_TEXTCALLBACK);
   CCEtoODBApp *app = ((CCEtoODBApp*)AfxGetApp());
   app->m_hwndDialog = m_hWnd;   
   app->m_gpToolTip = &tooltip;

   Erase = FALSE;

   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      m_classCB.AddString(graphicClassTagToDisplayString(grclass));
   }

   m_class = s->getData()->getGraphicClass();

   FileStruct *f;
   POSITION pos = getCamCadDoc().getFileList().GetHeadPosition(); // find file ptr for this entity's 

   while (pos != NULL)
   {
      f = getCamCadDoc().getFileList().GetNext(pos);

      if (f->getFileNumber() == s->filenum)
         break;
   }

   FillLayerCB(&m_layerCB,&getCamCadDoc(), f, s->getData()->getLayerIndex());
   
   FillWidthCB(&m_widthCB, s->getData()->getPolyList()->GetHead()->getWidthIndex(),&getCamCadDoc());

   m_negative = s->getData()->isNegative();

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   FillSpanLayerCB(&m_startlayerCB,&getCamCadDoc(), f, s->getData()->GetPhysicalStartLayer());
   FillSpanLayerCB(&m_endlayerCB,&getCamCadDoc(), f, s->getData()->GetPhysicalEndLayer());

   //Initial Span Layer
   m_spanlayer = (m_class == graphicClassCavity && s->getData()->HasLayerSpan());
   GetDlgItem(IDC_SPANLAYER)->EnableWindow((m_class == graphicClassCavity));
   EnableSpanlayer(m_spanlayer);
   OnSetSpanLayerToPoly();

   FillTree(NULL);

   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditPoly::FillTree(POSITION polyPos) 
{
   HTREEITEM root, leaf, first;
   BOOL First = TRUE;
   POSITION pntPos;
   CPoly *poly;
   CPnt *pnt;
   CString buf;

   CWaitCursor wait;
   
   // polys
   int count = 0;
	if (polyPos == NULL)
	{
		m_tree.DeleteAllItems();
		polyPos = s->getData()->getPolyList()->GetHeadPosition();
	}

   while (polyPos != NULL)
   {
      POSITION oldPolyPos = polyPos;

      poly = s->getData()->getPolyList()->GetNext(polyPos);
      count++;

      if (count > 100)
      {
         buf.Format("NEXT 100 polys...", s->getData()->getPolyList()->GetCount() - count);
         root = m_tree.InsertItem(buf);

			// Save the next poly position so if you close on "NEXT 100 polys...", it will continue to fill the tree
         m_tree.SetItemData(root, (DWORD)oldPolyPos);
         break;
      }

      if (poly->getWidthIndex() == -1)
      {
         buf = "Poly ";
      }
      else
      {
         BlockStruct *polyWidth = getCamCadDoc().getWidthTable()[poly->getWidthIndex()];
         CString shape;

         switch (polyWidth->getShape())
         {
         case T_ROUND:
            shape = "Rnd";
            break;
         case T_SQUARE:
            shape = "Sqr";
            break;
         default:
            shape = "oth";
         }

         buf.Format("Poly %s:%.*lf:%s ", polyWidth->getName(), decimals, polyWidth->getSizeA(), shape);
      }

      if (poly->isVoid()) buf += "VOID ";
      if (poly->isFilled()) buf += "FILLED ";
      if (poly->isClosed()) buf += "CLOSED ";
      if (poly->isHidden()) buf += "HIDDEN ";
      if (poly->isFloodBoundary()) buf += "BOUNDARY ";
      if (poly->isThermalLine()) buf += "THERMAL ";
      if (poly->isHatchLine()) buf += "HATCHLINE ";

      root = m_tree.InsertItem(buf);

      if (First)
      {
         first = root;
         First = FALSE;
      }

      m_tree.SetItemData(root, (DWORD)poly);

      pntPos = poly->getPntList().GetHeadPosition();

      while (pntPos != NULL)
      {
         pnt = poly->getPntList().GetNext(pntPos);

         if (fabs(pnt->bulge) > 0)
         {
            buf.Format("(%+.*lf, %+.*lf) bulge=%+.3lf", decimals, pnt->x, decimals, pnt->y, 
                  pnt->bulge);
         }
         else
         {
            buf.Format("(%+.*lf, %+.*lf)", decimals, pnt->x, decimals, pnt->y);
         }

         leaf = m_tree.InsertItem(buf, root);
         m_tree.SetItemData(leaf, (DWORD)pnt);
      }
   }

   // expand poly if there is only 1 poly
   if (s->getData()->getPolyList()->GetCount() == 1)
      m_tree.Expand(root, TVE_EXPAND);

   m_tree.SelectItem(NULL);//first);
}

void EditPoly::OnSetSpanLayerToPoly()
{
   if(m_class != graphicClassCavity) return;

   LayerStruct* startlayer = (m_startlayerCB.GetCurSel() > -1)?((LayerStruct*)(m_startlayerCB.GetItemDataPtr(m_startlayerCB.GetCurSel()))):NULL;
   LayerStruct* endlayer = (m_endlayerCB.GetCurSel() > -1)?((LayerStruct*)(m_endlayerCB.GetItemDataPtr(m_endlayerCB.GetCurSel()))):NULL;

   bool setLayer = false;
   if(m_spanlayer && startlayer && endlayer)
   {
      if(startlayer != endlayer)
      {
         s->getData()->SetPhysicalStartLayer(startlayer->getPhysicalStackNumber());
         s->getData()->SetPhysicalEndLayer(endlayer->getPhysicalStackNumber());
         m_layerCB.SelectString(0,startlayer->getName());
         setLayer = true;
      }
   }

   if(!setLayer)
   {
      CString layerName = "";
      m_layerCB.GetLBText(m_layerCB.GetCurSel(), layerName);
      if(m_startlayerCB.FindString(0,layerName) < 0 && m_startlayerCB.GetCount())
         m_startlayerCB.GetLBText(0, layerName);
      
      m_startlayerCB.SelectString(0, layerName);
      m_endlayerCB.SelectString(0, layerName);

      s->getData()->SetPhysicalStartLayer(-1);
      s->getData()->SetPhysicalEndLayer(-1);
      
      m_chkSpanlayer.SetCheck(FALSE);
      EnableSpanlayer(FALSE);
   }
}

void EditPoly::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   InsertModifyUndo(&getCamCadDoc(), TRUE); 

// GetWidthIndex(&m_widthCB, &(s->p->poly.polylist->GetHead()->widthIndex), doc);

   s->getData()->setLayerIndex(((LayerStruct*)(m_layerCB.GetItemDataPtr(m_layerCB.GetCurSel())))->getLayerIndex());
   s->getData()->setNegative(m_negative);
   s->getData()->setGraphicClass(m_class);

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
   
   OnSetSpanLayerToPoly();
}

void EditPoly::OnCancel() 
{
   if (Erase)
      getCamCadDoc().HighlightPolyItem(s, highlightPoly, highlightPnt, TRUE);

   Editing = FALSE;
   
   CEditDialog::OnCancel();
}

void EditPoly::OnEditPolyItem() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/
#ifndef SHAREWARE
   HTREEITEM item = m_tree.GetSelectedItem();
   if (item == NULL)
      return;

   if (m_tree.GetParentItem(item) == NULL) // poly
   {
      CPoly *poly = (CPoly*)m_tree.GetItemData(item);
      EditPolyItem dlg(AfxGetMainWnd());
      dlg.doc = &getCamCadDoc();
      dlg.m_filled = poly->isFilled();
      dlg.m_closed = poly->isClosed();
      dlg.m_void = poly->isVoid();
      dlg.m_hidden = poly->isHidden();
      dlg.m_boundary = poly->isFloodBoundary();
      dlg.m_thermal = poly->isThermalLine();
      dlg.m_hatch = poly->isHatchLine();
      dlg.widthIndex = poly->getWidthIndex();

      if (dlg.DoModal() == IDOK)
      {
         if (!poly->isVoid() && dlg.m_void && s->getData()->getPolyList()->GetCount() < 2)
            ErrorMessage("Changing a single poly to a Void", "Warning");

         getCamCadDoc().DrawEntity(s, -1, FALSE);
         InsertModifyUndo(&getCamCadDoc(), TRUE); 

         poly->setFilled(dlg.m_filled);
         poly->setClosed(dlg.m_closed | dlg.m_filled);
         poly->setVoid(dlg.m_void);
         poly->setHidden(dlg.m_hidden);
         poly->setWidthIndex(dlg.widthIndex);
         poly->setFloodBoundary(dlg.m_boundary);
         poly->setThermalLine(dlg.m_thermal);
         poly->setHatchLine(dlg.m_hatch);

         CString buf;
         BlockStruct *polyWidth = getCamCadDoc().getWidthTable()[poly->getWidthIndex()];
         CString shape;

         switch (polyWidth->getShape())
         {
         case T_ROUND:
            shape = "Rnd";
            break;
         case T_SQUARE:
            shape = "Sqr";
            break;
         default:
            shape = "oth";
         }
   
         buf.Format("Poly %.*lf%s ", decimals, polyWidth->getSizeA(), shape);

         if (poly->isVoid()) buf += "VOID ";
         if (poly->isFilled()) buf += "FILLED ";
         if (poly->isClosed()) buf += "CLOSED ";
         if (poly->isHidden()) buf += "HIDDEN ";
         if (poly->isFloodBoundary()) buf += "BOUNDARY ";
         if (poly->isThermalLine()) buf += "THERMAL ";
         if (poly->isHatchLine()) buf += "HATCHLINE ";

         m_tree.SetItemText(item, buf);

         // if Closed :
         //   if necessary, add closing vertex
         if (poly->isClosed())
         {
            CPnt *first, *last;
            first = poly->getPntList().GetHead();
            last = poly->getPntList().GetTail();

            if (fabs(first->x - last->x) > SMALLNUMBER || fabs(first->y - last->y) > SMALLNUMBER)
            {
               last = new CPnt;
               last->x = (DbUnit)first->x;
               last->y = (DbUnit)first->y;
               last->bulge = (DbUnit)0.0;
               poly->getPntList().AddTail(last);
               FillTree(NULL);
            }
         }

         getCamCadDoc().DrawEntity(s, 1, FALSE);
      }
   }
   else // vertex
   {
      EditPntItem dlg(AfxGetMainWnd());;

      dlg.pnt = (CPnt*)m_tree.GetItemData(item);
      dlg.decimals = decimals;
      dlg.doc = &getCamCadDoc();
      dlg.s = s;

      // get next
      CPoly *poly = (CPoly*)m_tree.GetItemData(m_tree.GetParentItem(item));
      POSITION pos = poly->getPntList().Find(dlg.pnt);
      poly->getPntList().GetNext(pos);
      dlg.next = NULL;

      if (pos != NULL)
         dlg.next = poly->getPntList().GetAt(pos);

      dlg.SetValues();
      dlg.m_type = 0;

      if (dlg.DoModal() == IDOK)
      {
         CString buf;

         if (fabs(dlg.pnt->bulge) > 0)
         {
            buf.Format("(%+.*lf, %+.*lf) bulge=%+.3lf", decimals, dlg.pnt->x, decimals, dlg.pnt->y, 
                  dlg.pnt->bulge);
         }
         else
         {
            buf.Format("(%+.*lf, %+.*lf)", decimals, dlg.pnt->x, decimals, dlg.pnt->y);
         }

         m_tree.SetItemText(item, buf);
      }
   }
#endif   // SHAREWARE
}

LRESULT EditPoly::OnDrawPolyItem(WPARAM WParam, LPARAM LParam)
{
   CWaitCursor wait;

   HTREEITEM item = m_tree.GetSelectedItem();

   if (item == NULL)
      return (LRESULT)0;

   if (Erase)
      getCamCadDoc().HighlightPolyItem(s, highlightPoly, highlightPnt, TRUE);

   getCamCadDoc().DrawEntity(s, 3, FALSE);

   void *voidPtr = (void*)m_tree.GetItemData(item); 

   if (!voidPtr) // "_ more polys...
   {
      m_widthCB.SetCurSel(-1);
      Erase = FALSE;
      return (LRESULT)0;
   }

   HTREEITEM parent = m_tree.GetParentItem(item);

   if (parent == NULL) // poly selected
   {
      highlightPoly = (CPoly*)voidPtr;
      Erase = FALSE;
      getCamCadDoc().HighlightPolyItem(s, highlightPoly, NULL, FALSE);
   }
   else // pnt selected
   {
      highlightPoly = (CPoly*)m_tree.GetItemData(parent);
      highlightPnt = (CPnt*)voidPtr;
      getCamCadDoc().HighlightPolyItem(s, highlightPoly, highlightPnt, FALSE);
      Erase = TRUE;
   }

   // show width for this poly
   SetWidthIndex(&m_widthCB, highlightPoly->getWidthIndex(),&getCamCadDoc());

   return (LRESULT)0;
}

void EditPoly::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
   *pResult = 0;

// PostMessage(WM_DRAW_POLY, 0, 0);
}

void EditPoly::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

   *pResult = 0;

	HTREEITEM item = m_tree.GetSelectedItem();

	CString buf = m_tree.GetItemText(item);

	if (!buf.CompareNoCase("NEXT 100 polys..."))
	{
      HTREEITEM lastItem = m_tree.GetNextItem(item,TVGN_PREVIOUS);

		// Get the position of the next poly in the polylist 
		POSITION polyPos = (POSITION)m_tree.GetItemData(item); 
	
		// Remove the item that says "NEXT 100 polys..."
		m_tree.DeleteItem(item);

		// Fill continue to fill the tree
		FillTree(polyPos);

      if (lastItem != NULL)
      {
         lastItem = m_tree.GetNextItem(lastItem,TVGN_NEXT);
         m_tree.Select(lastItem,TVGN_FIRSTVISIBLE);
         m_tree.SelectItem(lastItem);
		   PostMessage(WM_DRAW_POLY, 0, 0);
      }
	}
	else
	{
		PostMessage(WM_DRAW_POLY, 0, 0);
	}
}

void EditPoly::EnableSpanlayer(BOOL hasSpanLayer)
{
   GetDlgItem(IDC_STATIC_START)->SetWindowText(hasSpanLayer?"Start":"Layer"); 
   GetDlgItem(IDC_STARTLAYER_CB)->ShowWindow(hasSpanLayer);
   GetDlgItem(IDC_LAYER_CB)->ShowWindow(!hasSpanLayer);

   GetDlgItem(IDC_STATIC_END)->EnableWindow(hasSpanLayer);
   GetDlgItem(IDC_ENDLAYER_CB)->EnableWindow(hasSpanLayer);

}

void EditPoly::OnBnClickedSpanlayer()
{
   UpdateData(true);
   EnableSpanlayer(m_spanlayer);   
}


void EditPoly::OnCbnSelchangeClass()
{
   UpdateData(true);
   
   BOOL IsCavity = (m_class == graphicClassCavity);
   GetDlgItem(IDC_SPANLAYER)->EnableWindow(IsCavity);    
   EnableSpanlayer(IsCavity);
   OnSetSpanLayerToPoly();

   UpdateData(false);
}

/////////////////////////////////////////////////////////////////////////////
// EditPntItem dialog
EditPntItem::EditPntItem(CWnd* pParent /*=NULL*/)
   : CDialog(EditPntItem::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditPntItem)
   m_chord = _T("");
   m_num = _T("");
   m_rad = _T("");
   m_x = _T("");
   m_y = _T("");
   m_type = -1;
   m_da = _T("");
   //}}AFX_DATA_INIT
}

void EditPntItem::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditPntItem)
   DDX_Text(pDX, IDC_BULGE_CHORD, m_chord);
   DDX_Text(pDX, IDC_BULGE_NUM, m_num);
   DDX_Text(pDX, IDC_BULGE_RAD, m_rad);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Radio(pDX, IDC_RADIO8, m_type);
   DDX_Text(pDX, IDC_BULGE_DA, m_da);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditPntItem, CDialog)
   //{{AFX_MSG_MAP(EditPntItem)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
   ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
   ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
   ON_BN_CLICKED(IDC_RADIO8, OnRadio0)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Positive values <--> Counter-Clockwise
// Bulge Number   
//    da = atan(bulge) * 4    <-->   bulge = tan(da/4)   
//    bulge = 1 -> 180°, smaller bulge -> smaller da -> shallower arc -> larger radius when smaller than half circle, larger radius when more than half circle
// Radius
//    Always makes smaller arc because it's ambiguous (2 arcs with same radius and direction)
//    da = 2 * asin (d/2/r)
// Chord
//    da = 2 * atan (2c/d)   for chord less than radius
//    chord = r - r cos(da/2) = d/2 * tan(da/2)   for chord less than radius
void EditPntItem::SetValues() 
{
   m_x.Format("%+.*lf", decimals, pnt->x);
   m_y.Format("%+.*lf", decimals, pnt->y);
   m_num.Format("%+.3lf", pnt->bulge);

   if (next == NULL) // no next pnt
   {
      m_type = 0;
      m_da = m_num = m_rad = m_chord = "0.000";
   }
   else
   {
      if (fabs(pnt->bulge) < SMALLNUMBER)
      {
         m_da = m_num = m_rad = m_chord = "0.000";
      }
      else
      {
         double da, sa, r, cx, cy;
         da = atan(pnt->bulge) * 4;
//       if (mirror) da = -da;
         ArcPoint2Angle(pnt->x, pnt->y, next->x, next->y, da, &cx, &cy, &r, &sa);

         // delta angle
         m_da.Format("%+.1lf", RadToDeg(da));
         
         // radius
         if (pnt->bulge > 0.0)
            m_rad.Format("+%.3lf", r);
         else
            m_rad.Format("-%.3lf", r);

         // chord
         if (fabs(PI - fabs(da)) < SMALLNUMBER)
         {
            if (pnt->bulge > 0.0)
               m_chord = "+1.000";
            else
               m_chord = "-1.000";
         }
         else if (fabs(da) < PI)
         {
            double distance = sqrt(pow(pnt->x-next->x, 2) + pow(pnt->y-next->y, 2));

            if (pnt->bulge > 0.0)
               m_chord.Format("+%.3lf", distance/2 * fabs(tan(da/2)));
            else
               m_chord.Format("-%.3lf", distance/2 * fabs(tan(da/2)));
         }
         else // da > PI
         {
            if (pnt->bulge > 0.0)
               m_chord.Format("+%.3lf", r + r * cos(PI-da/2));
            else
               m_chord.Format("-%.3lf", r + r * cos(PI-da/2));
         }
      }
   }
}

void EditPntItem::GetValues() 
{
   UpdateData();

   if (s != NULL)
   {
      doc->DrawEntity(s, -1, FALSE);
      InsertModifyUndo(doc, TRUE);
   }

   CString buf;
   pnt->x = (DbUnit)atof(m_x);
   pnt->y = (DbUnit)atof(m_y);

   switch (m_type)
   {
   case 0: // bulge delat angle
      pnt->bulge = (DbUnit)tan(DegToRad(atof(m_da))/4);
      break;
   case 1: // bulge num
      pnt->bulge = (DbUnit)atof(m_num);
      break;
   case 2: // bulge radius
      {
         double r = fabs(atof(m_rad));
         double distance = sqrt(pow(pnt->x-next->x, 2) + pow(pnt->y-next->y, 2));

         if (distance/2 - r > SMALLNUMBER)
         {
            pnt->bulge = (DbUnit)0.0;
         }
         else
         {
            double ratio = distance/2/r;

            if (ratio > 1.0) ratio = 1.0;
            if (ratio < -1.0) ratio = -1.0;

            double da = 2.0 * asin(ratio);
            pnt->bulge = (DbUnit)tan(da/4);

            if (atof(m_rad) < 0.0)
               pnt->bulge = (DbUnit)-pnt->bulge;
         }
      }
      break;
   case 3: // bulge chord distance
      {
         double distance = sqrt(pow(pnt->x-next->x, 2) + pow(pnt->y-next->y, 2));
         double chord = fabs(atof(m_chord));

         if (fabs(chord) < SMALLNUMBER)
         {
            pnt->bulge = (DbUnit)0.0;
         }
         else if (fabs(chord - distance / 2) < SMALLNUMBER) // chord == distance/2
         {
            pnt->bulge = (DbUnit)1.0;
         }
         else if (chord < distance / 2)
         {
            double da = 2.0 * atan(2.0 * chord / distance);
            pnt->bulge = (DbUnit)tan(da/4);
         }
         else // chord > distance/2
         {
            double r = chord/2 + distance*distance/8/chord;
            double a = asin(distance/2/r);
            double da = PI2 - a*2;
            pnt->bulge = (DbUnit)tan(da/4);
         }

         if (atof(m_chord) < 0.0)
            pnt->bulge = (DbUnit)-pnt->bulge;
      }
      break;
   }

   if (s != NULL)
      doc->DrawEntity(s, 1, FALSE);
   else
      doc->OnRedraw();
}

void EditPntItem::OnApply() 
{
   GetValues();
   SetValues();
   UpdateData(FALSE);
}

void EditPntItem::OnOK() 
{
   GetValues();
   
   CDialog::OnOK();
}

void EditPntItem::OnRadio0() 
{
   GetDlgItem(IDC_BULGE_DA)->EnableWindow(TRUE);
   GetDlgItem(IDC_BULGE_NUM)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_RAD)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_CHORD)->EnableWindow(FALSE);
}

void EditPntItem::OnRadio1() 
{
   GetDlgItem(IDC_BULGE_DA)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_NUM)->EnableWindow(TRUE);
   GetDlgItem(IDC_BULGE_RAD)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_CHORD)->EnableWindow(FALSE);
}

void EditPntItem::OnRadio2() 
{
   GetDlgItem(IDC_BULGE_DA)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_NUM)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_RAD)->EnableWindow(TRUE);
   GetDlgItem(IDC_BULGE_CHORD)->EnableWindow(FALSE);
}

void EditPntItem::OnRadio3() 
{
   GetDlgItem(IDC_BULGE_DA)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_NUM)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_RAD)->EnableWindow(FALSE);
   GetDlgItem(IDC_BULGE_CHORD)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// EditPolyItem dialog
EditPolyItem::EditPolyItem(CWnd* pParent /*=NULL*/)
   : CDialog(EditPolyItem::IDD, pParent)
{
   //{{AFX_DATA_INIT(EditPolyItem)
   m_closed = FALSE;
   m_filled = FALSE;
   m_void = FALSE;
   m_hidden = FALSE;
   m_thermal = FALSE;
   m_boundary = FALSE;
   m_hatch = FALSE;
   //}}AFX_DATA_INIT
}

void EditPolyItem::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditPolyItem)
   DDX_Control(pDX, IDC_WIDTH, m_widthCB);
   DDX_Check(pDX, IDC_CLOSED, m_closed);
   DDX_Check(pDX, IDC_FILLED, m_filled);
   DDX_Check(pDX, IDC_VOID, m_void);
   DDX_Check(pDX, IDC_HIDDEN, m_hidden);
   DDX_Check(pDX, IDC_THERMAL, m_thermal);
   DDX_Check(pDX, IDC_BOUNDARY, m_boundary);
   DDX_Check(pDX, IDC_HATCH, m_hatch);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditPolyItem, CDialog)
   //{{AFX_MSG_MAP(EditPolyItem)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL EditPolyItem::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   FillWidthCB(&m_widthCB, widthIndex, doc);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditPolyItem::OnOK() 
{
   GetWidthIndex(&m_widthCB, &widthIndex, doc);
   
   CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// EditBlob dialog
EditBlob::EditBlob(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
: CEditDialog(EditBlob::IDD,camCadDoc, pParent)
{
   //{{AFX_DATA_INIT(EditBlob)
   m_x = _T("");
   m_y = _T("");
   m_width = _T("");
   m_height = _T("");
   //}}AFX_DATA_INIT

   addFieldControl(IDC_ATTRIBLC,anchorLeft,growBoth);
}

void EditBlob::DoDataExchange(CDataExchange* pDX)
{
   CEditDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(EditBlob)
   DDX_Control(pDX, IDC_ATTRIBLC, m_attribLC);
   DDX_Text(pDX, IDC_X, m_x);
   DDX_Text(pDX, IDC_Y, m_y);
   DDX_Text(pDX, IDC_WIDTH, m_width);
   DDX_Text(pDX, IDC_HEIGHT, m_height);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EditBlob, CEditDialog)
   //{{AFX_MSG_MAP(EditBlob)
   ON_BN_CLICKED(IDC_MODIFY, OnModify)
   ON_BN_CLICKED(IDC_ATTRIBS, OnAttribs)
   ON_BN_CLICKED(IDC_FLAGS, OnFlags)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditBlob message handlers
BOOL EditBlob::OnInitDialog() 
{
   CEditDialog::OnInitDialog();
   
   int decimals = GetDecimals(getCamCadDoc().getSettings().getPageUnits());
   m_x.Format("%+.*lf", decimals, s->getData()->getBlob()->pnt.x);
   m_y.Format("%+.*lf", decimals, s->getData()->getBlob()->pnt.y);
   m_width.Format("%+.*lf", decimals, s->getData()->getBlob()->width);
   m_height.Format("%+.*lf", decimals, s->getData()->getBlob()->height);

   if (s->getData()->getAttributesRef())
      FillAttribLC(&m_attribLC, s->getData()->getAttributesRef(),&getCamCadDoc());

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void EditBlob::OnModify() 
{
   if (!isEditable(*s))
      return;

#ifndef SHAREWARE
   UpdateData();

   getCamCadDoc().DrawEntity(s, -1, FALSE);

   s->getData()->getBlob()->pnt.x = (DbUnit)atof(m_x);
   s->getData()->getBlob()->pnt.y = (DbUnit)atof(m_y);
   s->getData()->getBlob()->width = (DbUnit)atof(m_width);
   s->getData()->getBlob()->height =  (DbUnit)atof(m_height);

   getCamCadDoc().DrawEntity(s, 1, FALSE);
#endif
   
}

void EditBlob::OnCancel() 
{
   Editing = FALSE;
   
   CEditDialog::OnCancel();
}


void EditPoint::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "POINT";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditLine::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "LINE";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditApInsert::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "FLASH APERTURE";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditArc::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "ARC";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditCircle::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "CIRCLE";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditInsert::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "INSERT";
   dlg.setMap(m_selectStruct->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditPoly::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "POLYLINE";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditText::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "TEXT";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditToolInsert::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "TOOL";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditBlob::OnAttribs() 
{
   EditAttribs dlg(AfxGetMainWnd());;
   dlg.doc = &getCamCadDoc();
   dlg.m_item = "BLOB";
   dlg.setMap(s->getData()->getAttributesRef());
   dlg.DoModal();
}

void EditArc::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

void EditCircle::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

void EditInsert::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = m_selectStruct->getData();
   dlg.DoModal(); 
}

void EditLine::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

void EditPoint::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

void EditText::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

void EditToolInsert::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

void EditBlob::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

void EditPoly::OnFlags() 
{
   FlagsDlg dlg;
   dlg.group = Data;
   dlg.item = s->getData();
   dlg.DoModal(); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Filter InsertTypes Dialog

/******************************************************************************
* CCEtoODBDoc::FilterInsertType
*/
void CCEtoODBDoc::FilterInsertType()
{
	CFilterByInsertType dlg(this,0);
	dlg.DoModal();
}

// CFilterByInsertType dialog

IMPLEMENT_DYNAMIC(CFilterByInsertType, CDialog)
CFilterByInsertType::CFilterByInsertType(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterByInsertType::IDD, pParent)
{

}

CFilterByInsertType::CFilterByInsertType(CCEtoODBDoc *document, CWnd* pParent /*=NULL*/)
	: CDialog(CFilterByInsertType::IDD, pParent)
{
	doc = document;
}

CFilterByInsertType::~CFilterByInsertType()
{
}

void CFilterByInsertType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_InsertTypeList, CInsertTypeList);
}


BEGIN_MESSAGE_MAP(CFilterByInsertType, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CFilterByInsertType message handlers

BOOL CFilterByInsertType::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (EnumIterator(InsertTypeTag,insertTypeIterator);insertTypeIterator.hasNext();)
   {
      InsertTypeTag insertType = insertTypeIterator.getNext();

      CInsertTypeList.SetItemDataPtr(CInsertTypeList.AddString(insertTypeToDisplayString(insertType)), (void*)insertType);
   }

	  return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterByInsertType::OnBnClickedOk()
{
	if (MessageBox("The selected insert types will be deleted.\nAre you sure you want to continue?",0,MB_YESNO) == IDYES)
	{
		int nCount = CInsertTypeList.GetSelCount();
		CArray<int,int> aryListBoxSel;
		//put selected items in the array
		aryListBoxSel.SetSize(nCount);
		CInsertTypeList.GetSelItems(nCount, aryListBoxSel.GetData()); 
		//go through the array of selected items and delete them.
		for (int i = aryListBoxSel.GetCount()-1; i >=0; i--)
		{
			int sel = aryListBoxSel.ElementAt(i);
			int insertType = (int)CInsertTypeList.GetItemDataPtr(sel);
			DeleteSelectionInsertType(insertType);
		}
		
		doc->UpdateAllViews(NULL);
		OnOK();
	}
}

/******************************************************************************
* CFilterByInsertType::DeleteSelectionInsertType
*/
void CFilterByInsertType::DeleteSelectionInsertType(int insertType)
{
	for (int i=0; i < doc->getMaxBlockIndex(); i++)
	{
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
		 if (data && data->getInsert() && data->getInsert()->getInsertType() == insertType)
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
      }
	}
}

/******************************************************************************
* CFilterByInsertType::OnBnClickedButton1
*/
void CFilterByInsertType::OnBnClickedButton1()
{
	CInsertTypeList.SetSel(-1,0);
}


//////////////////////////////////////////////////////////////////////////////////////////////
//Filter GraphicClass Dialog

/******************************************************************************
* CCEtoODBDoc::FilterGraphicClass
*/
void CCEtoODBDoc::FiltergraphicClass()
{
	CFilterByGraphicClass dlg(this,0);
	dlg.DoModal();
}

// CFilterByGraphicClass dialog

IMPLEMENT_DYNAMIC(CFilterByGraphicClass, CDialog)
CFilterByGraphicClass::CFilterByGraphicClass(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterByGraphicClass::IDD, pParent)
{

}

CFilterByGraphicClass::CFilterByGraphicClass(CCEtoODBDoc *document, CWnd* pParent /*=NULL*/)
	: CDialog(CFilterByGraphicClass::IDD, pParent)
{
	doc = document;
}

CFilterByGraphicClass::~CFilterByGraphicClass()
{
}

void CFilterByGraphicClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GraphicClassList, CGraphicClassList);
}


BEGIN_MESSAGE_MAP(CFilterByGraphicClass, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CFilterByGraphicClass message handlers

BOOL CFilterByGraphicClass::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (EnumIterator(GraphicClassTag, grclassIterator); grclassIterator.hasNext();)
   {
      GraphicClassTag grclass = grclassIterator.getNext();
      CGraphicClassList.SetItemDataPtr(CGraphicClassList.AddString(graphicClassTagToDisplayString(grclass)), (void*)grclass);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterByGraphicClass::OnBnClickedOk()
{
	if (MessageBox("The selected graphic classes will be deleted.\nAre you sure you want to continue?",0,MB_YESNO) == IDYES)
	{
		int nCount = CGraphicClassList.GetSelCount();
		CArray<int,int> aryListBoxSel;
		//put selected items in the array
		aryListBoxSel.SetSize(nCount);
		CGraphicClassList.GetSelItems(nCount, aryListBoxSel.GetData()); 
		//go through the array of selected items and delete them.
		for (int i = aryListBoxSel.GetCount()-1; i >=0; i--)
		{
			int sel = aryListBoxSel.ElementAt(i);
			int GraphicClass = (int)CGraphicClassList.GetItemDataPtr(sel);
			DeleteSelectionGrClass(GraphicClass);
		}
		
		doc->UpdateAllViews(NULL);
		OnOK();
	}
}

/******************************************************************************
* CFilterByGraphicClass::DeleteSelectionGrClass
*/
void CFilterByGraphicClass::DeleteSelectionGrClass(int grClass)
{
	for (int i=0; i < doc->getMaxBlockIndex(); i++)
	{
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if (data && data->getGraphicClass() == grClass)
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
      }
	}
}

/******************************************************************************
* CFilterByGraphicClass::OnBnClickedButton1
*/
void CFilterByGraphicClass::OnBnClickedButton1()
{
	CGraphicClassList.SetSel(-1,0);
}

//******************************************************************************
//##############################################################################

void CTableGrid::OnTabSelected(int tabId)
{
   SetSheetNumber(tabId);
}

void CTableGrid::OnSetup()
{
   SetTabWidth(300);
   SetSheetNumber(0);
}
//------------------------------------------------------------------------------

CTableEditDialog::CTableEditDialog()
: CResizingDialog(CTableEditDialog::IDD,NULL)
, m_valid(false)
{
   addFieldControl(IDC_TABLELIST_MSG       ,anchorLeft, growStatic);
   addFieldControl(IDC_TABLELIST_LIST      ,anchorLeft, growProportionalBoth);

   addFieldControl(IDC_READONLY_MSG        ,anchorRight, growStatic);
   addFieldControl(IDC_TABLE_GRID          ,anchorRight, growProportionalBoth);
}

void CTableEditDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_TABLELIST_LIST, m_tableListOwners);
}

BEGIN_MESSAGE_MAP(CTableEditDialog, CResizingDialog)
   ON_LBN_SELCHANGE(IDC_TABLELIST_LIST, OnLbnSelChangeTableList)
END_MESSAGE_MAP()

BOOL CTableEditDialog::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   m_tableGrid.AttachGrid(this,IDC_TABLE_GRID);

   FillTableListSelector();

   FillGrid();

   if (!m_valid)
      EndDialog(IDCANCEL);

   return TRUE;
}


void CTableEditDialog::FillTableListSelector()
{
   // In the list we put some identifier for the table list owner
   // and the number of tables in that list.

   m_tableListOwners.ResetContent();

   if(NULL != getActiveView() && NULL != getActiveView()->GetDocument())
   {
      int firstNonEmptyListIndx = -1; 

      CCEtoODBDoc *doc = getActiveView()->GetDocument();

      CGTabTableList &ccDocTableList = doc->getCamCadData().getTableList();
      int tblCnt = ccDocTableList.GetCount();

      CString selectionEntry;
      selectionEntry.Format("CCDoc (%d tables)", tblCnt);

      int itemIndx = m_tableListOwners.AddString(selectionEntry);
      m_tableListOwners.SetItemDataPtr(itemIndx, &ccDocTableList);

      if (tblCnt > 0)
         firstNonEmptyListIndx = itemIndx;

      POSITION filepos = doc->getFileList().GetHeadPosition();
      while (filepos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(filepos);

         CGTabTableList &fileTableList = file->getTableList();
         tblCnt = fileTableList.GetCount();
         selectionEntry.Format("<File> %s (%d tables)", file->getName(), tblCnt);

         itemIndx = m_tableListOwners.AddString(selectionEntry);
         m_tableListOwners.SetItemDataPtr(itemIndx, &fileTableList);

         if (firstNonEmptyListIndx < 0 && tblCnt > 0)
            firstNonEmptyListIndx = itemIndx;
      }

      if (firstNonEmptyListIndx >= 0)
         m_tableListOwners.SetCurSel(firstNonEmptyListIndx);
   }
}

void CTableEditDialog::OnLbnSelChangeTableList()
{
   FillGrid();
}

void CTableEditDialog::ResetGrid()
{
   m_tableGrid.SetPaintMode(FALSE); 
   
   //while (m_tableGrid.SetCurrentTab(0) == UG_SUCCESS)
   //{
   //   m_tableGrid.DeleteTab(0);
   //}

   // Leave tab 0
   //m_tableGrid.SetCurrentTab(0);
   int tabID = 0;
   while (m_tableGrid.DeleteTab(tabID++) == UG_SUCCESS) { /*already done deleted it*/}


   m_tableGrid.SetSheetNumber(0);

   while (m_tableGrid.GetNumberRows() > 0) 
      m_tableGrid.DeleteRow(0);

   while (m_tableGrid.GetNumberCols() > 0) 
      m_tableGrid.DeleteCol(0);

   m_tableGrid.SetNumberSheets(1);
   
   m_tableGrid.SetPaintMode(TRUE);
}

void CTableEditDialog::FillGrid()
{
   // If we add a table to the grid then we consider the grid valid for display purposes.
   // Even if the table is actually empty. If the table is in the table list then we
   // will show it, even if there is actually nothing in it.

   ResetGrid();

   if(NULL != getActiveView() && NULL != getActiveView()->GetDocument())
   {
      ///CCEtoODBDoc *doc = getActiveView()->GetDocument();

      ///CGTabTableList &tableList = doc->getCamCadData().getTableList();

      CGTabTableList *tableListPtr = NULL;
      int curSelIndx = m_tableListOwners.GetCurSel();

      if (curSelIndx >= 0)
         tableListPtr = (CGTabTableList*)m_tableListOwners.GetItemDataPtr(curSelIndx);

      if (tableListPtr != NULL)
      {
         int tblCnt = tableListPtr->GetCount();
         this->m_tableGrid.SetNumberSheets(tblCnt);

         if (tableListPtr->GetCount() > 0)
         {
            for (int tblIndx = 0; tblIndx < tblCnt; tblIndx++)
            {
               CGTabTable *table = tableListPtr->GetTableAt(tblIndx);
               if (table != NULL)
               {
                  this->m_tableGrid.AddTab(table->GetName(), tblIndx);
                  this->m_tableGrid.SetCurrentTab(tblIndx);
                  this->m_tableGrid.SetSheetNumber(tblIndx);
                  m_valid = true;

                  int colCnt = table->GetColCount();
                  int rowCnt = table->GetRowCount();

                  this->m_tableGrid.SetNumberCols(colCnt);
                  this->m_tableGrid.SetNumberRows(rowCnt);

                  // Outer loop rows inner loop cols is the usual way one thinks of it.
                  // This is outer loop cols inner loop rows so setting col header is more convenient.
                  // Either way loop hits all cells so time is the same, but presumably
                  // shorter because we can just do col heading along the way without checking
                  // if rowIndx==0 to make it happen only once.

                  // But then we added row names. So now it does not matter much which is
                  // inner and which is outer loop.

                  for (int colIndx = 0; colIndx < colCnt; colIndx++)
                  {
                     CString colHeading( table->GetColumnHeadingAt(colIndx) );
                     if (!colHeading.IsEmpty())
                     {
                        this->m_tableGrid.QuickSetText(colIndx, -1, colHeading);
                     }

                     for (int rowIndx = 0; rowIndx < rowCnt; rowIndx++)
                     {
                        // Do row name while doing first column
                        if (colIndx == 0)
                        {
                           CString rowName;
                           CGTabRow *row = table->GetRowAt(rowIndx);
                           if (row != NULL)
                              rowName = row->GetName();
                           if (rowName.IsEmpty())
                              rowName.Format("%d", rowIndx + 1);
                           this->m_tableGrid.QuickSetText(-1, rowIndx, rowName);
                        }

                        // Now the row cells content
                        CString cellValue;
                        table->GetCellValueAt(rowIndx, colIndx, cellValue);
                        this->m_tableGrid.QuickSetText(colIndx, rowIndx, cellValue);
                     }
                  }
               }
            }

            this->m_tableGrid.SetCurrentTab(0);
            this->m_tableGrid.SetSheetNumber(0);
         }
      }
      else
      {
         ErrorMessage("No tables in current item's table list.");
      }
   }
   else
   {
      ErrorMessage("No active document found.");
   }


}

