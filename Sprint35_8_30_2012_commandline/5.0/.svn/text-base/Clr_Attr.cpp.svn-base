// $Header: /CAMCAD/5.0/Clr_Attr.cpp 28    6/17/07 8:50p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "clr_attr.h"
#include "ccdoc.h"
#include "hilite.h"
#include "PersistantColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define KEY_GEOMETRY "GEOMETRY"

struct ColorByAttribStruct
{
   BOOL IsData;
   void *ptr;
};

typedef CTypedPtrList<CPtrList, ColorByAttribStruct*> CColorByAttribList;

/******************************************************************************
* OnColorByAttribute
*/
void CCEtoODBDoc::OnColorByAttribute() 
{
   ColorByAttrib dlg;
   dlg.doc = this;
   dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// ColorByAttrib dialog
ColorByAttrib::ColorByAttrib(CWnd* pParent /*=NULL*/)
   : CDialog(ColorByAttrib::IDD, pParent)
{
   //{{AFX_DATA_INIT(ColorByAttrib)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void ColorByAttrib::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ColorByAttrib)
   DDX_Control(pDX, IDC_VALUE_LB, m_valueLB);
   DDX_Control(pDX, IDC_KEYWORD_LB, m_keywordLB);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ColorByAttrib, CDialog)
   //{{AFX_MSG_MAP(ColorByAttrib)
   ON_BN_CLICKED(IDC_COLOR, OnColor)
   ON_BN_CLICKED(IDC_UNCOLOR, OnUncolor)
   ON_BN_CLICKED(IDC_CLEAR, OnClear)
   ON_LBN_SELCHANGE(IDC_KEYWORD_LB, OnSelchangeKeywordLb)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ColorByAttrib message handlers

BOOL ColorByAttrib::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;

      // Datas
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getAttributesRef())
            FillAttribKeywords(data->getAttributesRef(), TRUE, data);
         
         //Fill Geometry Name
         FillGeometryNames(data);
      }      
   }

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      
      if (file->isShown() && !file->isHidden())
      {
/*       // datas
         POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

            if (data->getAttributesRef())
               FillAttribKeywords(data->getAttributesRef(), TRUE, data);
         }*/

         // cp
         POSITION netPos = file->getNetList().GetHeadPosition();
         while (netPos)
         {
            NetStruct *net = file->getNetList().GetNext(netPos);

            POSITION cpPos = net->getHeadCompPinPosition();
            while (cpPos)
            {
               CompPinStruct *cp = net->getNextCompPin(cpPos);

               if (cp->getAttributesRef())
                  FillAttribKeywords(cp->getAttributesRef(), FALSE, cp);
            }
         }
      }
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ColorByAttrib::FillAttribKeywords(CAttributes* map, BOOL IsData, void *ptr)
{
   if (!map)
      return;

   for (POSITION attribPos = map->GetStartPosition();attribPos != NULL;)
   {
      Attrib* attrib;
      WORD keyword;

      map->GetNextAssoc(attribPos, keyword, attrib);

		if (keyword < 0 || keyword >= doc->getKeyWordArray().GetCount())
			continue;

		CString attribKey = doc->getKeyWordArray()[keyword]->getCCKeyword();

      int i;
      CMapStringToPtr *valueMap = NULL;

      if ((i = m_keywordLB.FindStringExact(0, attribKey)) != CB_ERR)
      {
         valueMap = (CMapStringToPtr*)m_keywordLB.GetItemDataPtr(i);
      }
      else
      {
         i = m_keywordLB.AddString(attribKey);
         valueMap = new CMapStringToPtr;
         m_keywordLB.SetItemDataPtr(i, valueMap);
      }

      CString buf;
		ValueTypeTag valueTypeTag = attrib->getValueType();

      switch (valueTypeTag)
      {
      case VT_STRING:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
         buf.Format("\"%s\"", attrib->getStringValue());
         break;
      case VT_INTEGER:
         buf.Format("%+d", attrib->getIntValue());
         break;
      case VT_DOUBLE:
      case VT_UNIT_DOUBLE:	// TOM - Case 1212 - We weren't taking care of default case or UNIT_DOUBLE, so I added them
         {
            int decimals = GetDecimals(doc->getSettings().getPageUnits());
				double attribDouble = attrib->getDoubleValue();
            buf.Format("%+.*lf", decimals, attribDouble);
         }
         break;
      case VT_NONE:
		default:
         buf = " * No Value *";
         break;
      }

      CColorByAttribList *list;
      void* voidPtr;

      if (valueMap->Lookup(buf, voidPtr))
      {
         list = (CColorByAttribList*)voidPtr;
      }
      else
      {
         list = new CColorByAttribList;
         valueMap->SetAt(buf, list);
      }

      ColorByAttribStruct *cba = new ColorByAttribStruct;
      cba->IsData = IsData;
      cba->ptr = ptr;
      list->AddTail(cba);
   }
}

void ColorByAttrib::FillGeometryNames(DataStruct *data)
{
   if(!data || !data->isInsertType(insertTypePcbComponent)) 
      return;

   // Add GEOMETRY to Keyword list box
   int idx = 0;
   CMapStringToPtr *geometryMap = NULL;
   if ((idx = m_keywordLB.FindStringExact(0, KEY_GEOMETRY)) != CB_ERR)
   {
      geometryMap = (CMapStringToPtr*)m_keywordLB.GetItemDataPtr(idx);
   }
   else
   {
      idx = m_keywordLB.AddString(KEY_GEOMETRY);
      geometryMap = new CMapStringToPtr;
      m_keywordLB.SetItemDataPtr(idx, geometryMap);
   }

   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
   if(!block || block->getName().IsEmpty()) return;   
   CString key = block->getName();

   // Add geometry name of component inserts
   CColorByAttribList *list = NULL;
   void* voidPtr = NULL;

   if (geometryMap->Lookup(key, voidPtr))
   {
      list = (CColorByAttribList*)voidPtr;
   }
   else
   {
      list = new CColorByAttribList;
      geometryMap->SetAt(key, list);
   }   

   ColorByAttribStruct *cba = new ColorByAttribStruct;
   cba->IsData = TRUE;
   cba->ptr = (void*)data;
   list->AddTail(cba);
}

void ColorByAttrib::OnSelchangeKeywordLb() 
{
   m_valueLB.ResetContent();

   CMapStringToPtr *valueMap = (CMapStringToPtr*)m_keywordLB.GetItemDataPtr(m_keywordLB.GetCurSel());
   
   POSITION pos = valueMap->GetStartPosition();
   while (pos)
   {
      void *voidPtr;
      CString string;

      valueMap->GetNextAssoc(pos, string, voidPtr);
      m_valueLB.SetItemDataPtr(m_valueLB.AddString(string), voidPtr);
   }
}

void ColorByAttrib::OnColor() 
{
   int count = m_valueLB.GetSelCount();
   if (!count)
      return;

   COLORREF color = RGB(255, 255, 255);
   CPersistantColorDialog dialog(color);
   if (dialog.DoModal() != IDOK)
      return;
   color = dialog.GetColor();

   int *arr = (int*)calloc(count, sizeof(int));

   if ( arr != NULL )
   {
      m_valueLB.GetSelItems(count, arr);

      for (int i=0; i<count; i++)
      {
         CColorByAttribList *list = (CColorByAttribList*)m_valueLB.GetItemData(arr[i]);
         POSITION pos = list->GetHeadPosition();
         while (pos)
         {
            ColorByAttribStruct *cba = list->GetNext(pos);
            if (cba->IsData)
            {
               DataStruct *data = (DataStruct*)cba->ptr;
               data->setOverrideColor(color);
               data->setColorOverride(true);
            }
            else
            {
               CompPinStruct *cp = (CompPinStruct*)cba->ptr;
               HighlightAPin(doc, cp->getRefDes(), cp->getPinName(), color);
            }
         }
      }

      free(arr);

      doc->UpdateAllViews(NULL);
   }
}

void ColorByAttrib::OnUncolor() 
{
   int count = m_valueLB.GetSelCount();
   if (!count)
      return;

   int *arr = (int*)calloc(count, sizeof(int));

   if ( arr != NULL )
   {
      m_valueLB.GetSelItems(count, arr);

      for (int i=0; i<count; i++)
      {
         CColorByAttribList *list = (CColorByAttribList*)m_valueLB.GetItemData(arr[i]);
         POSITION pos = list->GetHeadPosition();
         while (pos)
         {
            ColorByAttribStruct *cba = list->GetNext(pos);
            if (cba->IsData)
            {
               DataStruct *data = (DataStruct*)cba->ptr;
               data->setColorOverride(false);
            }
            else
            {
               CompPinStruct *cp = (CompPinStruct*)cba->ptr;
               HighlightAPinClear(doc, cp->getRefDes(), cp->getPinName());
            }
         }
      }

      free(arr);

      doc->UpdateAllViews(NULL);
   }
}

void ColorByAttrib::OnClear() 
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block) 
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         data->setColorOverride(false);
      }
   }

   HighlightAPinClearAll(doc);

   doc->UpdateAllViews(NULL);
}

void ColorByAttrib::OnCancel() 
{
   int count = m_keywordLB.GetCount();
   for (int i=0; i<count; i++)
   {
      CMapStringToPtr *map = (CMapStringToPtr*)m_keywordLB.GetItemDataPtr(i);
      POSITION valuePos = map->GetStartPosition();
      while (valuePos)
      {
         void *voidPtr;
         CString string;

         map->GetNextAssoc(valuePos, string, voidPtr);
         CColorByAttribList *list = (CColorByAttribList*)voidPtr;
         POSITION listPos = list->GetHeadPosition();
         while (listPos)
         {
            ColorByAttribStruct *cba = list->GetNext(listPos);
            delete cba;
         }

         delete list;
      }

      delete map;
   }
   
   CDialog::OnCancel();
}
