// $Header: /CAMCAD/4.5/Lyr_Lyr.cpp 13    4/28/06 2:14p Kurt Van Ness $

/****************************************************************************
* Project CAMCAD
* Router Solutions Inc.
* Copyright © 1994-99. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "lyr_lyr.h"
#include "pcbutil.h"


struct LayerMapStruct
{
   LayerStruct* layer;
   CString newLayer;
};

static CTypedArrayContainer<CPtrArray, LayerMapStruct*> layerMapArr(false);

/****************************************************************************
* Get_Layer_Layername
*   function to be used in DXF to get the export name of each layer
*  -- NOTE : if two layers go to the same layer, must be taken care of in DXF/HP OUT.CPP
*/
const char *Get_Layer_Layername(int layerNum)
{
   for (int i=0; i<layerMapArr.GetCount(); i++)
   {
      LayerMapStruct* layerMap = layerMapArr.GetAt(i);
      if (layerMap != NULL && layerMap->layer != NULL && layerMap->layer->getLayerIndex() == layerNum)
         return layerMap->newLayer;
   }

   return NULL;  
}

/******************************************************************************
* Free_Layer_Layername
*  Frees the array used to store layername info during exporting
*  Should be called after exporting is complete
*/
void Free_Layer_Layername()
{ 
   layerMapArr.empty();
}

void LoadLayers(CCamCadData& camCadData)
{
   for (int i=0; i<camCadData.getLayerArray().GetCount(); i++)
   {
      LayerStruct* layer = camCadData.getLayer(i);
      if (layer != NULL && !layer->isFloating() && layer->isVisible())
      {
         LayerMapStruct* layerMap = new LayerMapStruct;
         layerMap->layer = layer;
         layerMap->newLayer = layer->getName();
         
         layerMapArr.SetAtGrow(layerMapArr.GetCount(), layerMap);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// Layer_LayerMap dialog
Layer_LayerMap::Layer_LayerMap(CWnd* pParent /*=NULL*/)
   : CDialog(Layer_LayerMap::IDD, pParent)
{
   //{{AFX_DATA_INIT(Layer_LayerMap)
   m_layer1 = _T("");
   m_layer2 = _T("");
   m_layer3 = _T("");
   m_layer4 = _T("");
   m_layer5 = _T("");
   m_layer6 = _T("");
   m_layer7 = _T("");
   m_layer8 = _T("");
   //}}AFX_DATA_INIT
}

void Layer_LayerMap::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(Layer_LayerMap)
   DDX_Control(pDX, IDC_NEW_LAYER8, m_newLayer8);
   DDX_Control(pDX, IDC_NEW_LAYER7, m_newLayer7);
   DDX_Control(pDX, IDC_NEW_LAYER6, m_newLayer6);
   DDX_Control(pDX, IDC_NEW_LAYER5, m_newLayer5);
   DDX_Control(pDX, IDC_NEW_LAYER4, m_newLayer4);
   DDX_Control(pDX, IDC_NEW_LAYER3, m_newLayer3);
   DDX_Control(pDX, IDC_NEW_LAYER2, m_newLayer2);
   DDX_Control(pDX, IDC_NEW_LAYER1, m_newLayer1);
   DDX_Control(pDX, IDC_SCROLLBAR1, m_scroll);
   DDX_Text(pDX, IDC_LAYER1, m_layer1);
   DDX_Text(pDX, IDC_LAYER2, m_layer2);
   DDX_Text(pDX, IDC_LAYER3, m_layer3);
   DDX_Text(pDX, IDC_LAYER4, m_layer4);
   DDX_Text(pDX, IDC_LAYER5, m_layer5);
   DDX_Text(pDX, IDC_LAYER6, m_layer6);
   DDX_Text(pDX, IDC_LAYER7, m_layer7);
   DDX_Text(pDX, IDC_LAYER8, m_layer8);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(Layer_LayerMap, CDialog)
   //{{AFX_MSG_MAP(Layer_LayerMap)
   ON_WM_VSCROLL()
   ON_BN_CLICKED(IDC_LOAD, OnLoad)
   ON_BN_CLICKED(IDC_SAVE, OnSave)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Layer_LayerMap message handlers
BOOL Layer_LayerMap::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   LoadLayers(doc->getCamCadData());

   if (layerMapArr.GetCount() > 8)
      m_scroll.SetScrollRange(0, layerMapArr.GetCount() - 8);
   else 
      m_scroll.SetScrollRange(0, 0);
   m_scroll.SetScrollPos(0);

   WriteData(0);

   FillComboBoxes();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

#define MAX_LAYERARRAY        500
#define MAX_LAYERNAMELENGTH   50

void Layer_LayerMap::FillComboBoxes() 
{
   char array[MAX_LAYERARRAY][MAX_LAYERNAMELENGTH];
   int i = 0;
   FILE *stream;
   char line[500];

   CString filename( getApp().getSystemSettingsFilePath("default.lyr") );

   stream = fopen(filename, "r"); // text mode by default
   if (stream == NULL)
   {
      MessageBox("Unable to Open File", filename, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fgets(line, 500, stream);
   if (STRCMPI(line,"! Layer Map File\n") != 0)
   {
      MessageBox("Wrong File Format", filename, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   char* tok;

   while (fgets(line,500,stream)!=NULL)
   {
      if (line[0] != '.') continue;

      if ((tok = get_string(line, " \t\n")) == NULL) continue;

      if (!STRCMPI(tok, ".Layer"))
      {
         if ((tok = get_string(NULL, " \t\n")) == NULL) continue;
         if (!keyword.Compare(tok))
         {
            char  tmp[80];
            if ((tok = get_string(NULL, "\n")) == NULL) continue;
            strcpy(tmp,tok);
            clean_blank(tmp);
            if (strlen(tmp) == 0)  continue;
               CpyStr(array[i++], tmp, MAX_LAYERNAMELENGTH);
         }
      }
   }

   for (int j = 0; j < i; j++)
   {
      m_newLayer1.AddString(array[j]);
      m_newLayer2.AddString(array[j]);
      m_newLayer3.AddString(array[j]);
      m_newLayer4.AddString(array[j]);
      m_newLayer5.AddString(array[j]);
      m_newLayer6.AddString(array[j]);
      m_newLayer7.AddString(array[j]);
      m_newLayer8.AddString(array[j]);
   }

   fclose(stream);
}

void WriteLayerName(CString *out, int i, CString name)
{
   if (name.GetLength() > 18)
      out->Format("%2d. %s...", i, name.Left(18));
   else
      out->Format("%2d. %s", i, name);
}

void Layer_LayerMap::WriteData(int pos) 
{
   int count = layerMapArr.GetCount();

   if (count < 1) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer1, pos+1, layerMapArr.GetAt(pos)->layer->getName());
   m_newLayer1.SetWindowText(layerMapArr.GetAt(pos)->newLayer);

   if (count < 2) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer2, pos+2, layerMapArr.GetAt(pos+1)->layer->getName());
   m_newLayer2.SetWindowText(layerMapArr.GetAt(pos+1)->newLayer);

   if (count < 3) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer3, pos+3, layerMapArr.GetAt(pos+2)->layer->getName());
   m_newLayer3.SetWindowText(layerMapArr.GetAt(pos+2)->newLayer);

   if (count < 4) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer4, pos+4, layerMapArr.GetAt(pos+3)->layer->getName());
   m_newLayer4.SetWindowText(layerMapArr.GetAt(pos+3)->newLayer);

   if (count < 5) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer5, pos+5, layerMapArr.GetAt(pos+4)->layer->getName());
   m_newLayer5.SetWindowText(layerMapArr.GetAt(pos+4)->newLayer);

   if (count < 6) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer6, pos+6, layerMapArr.GetAt(pos+5)->layer->getName());
   m_newLayer6.SetWindowText(layerMapArr.GetAt(pos+5)->newLayer);

   if (count < 7) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer7, pos+7, layerMapArr.GetAt(pos+6)->layer->getName());
   m_newLayer7.SetWindowText(layerMapArr.GetAt(pos+6)->newLayer);

   if (count < 8) { UpdateData(FALSE); return; }
   WriteLayerName(&m_layer8, pos+8, layerMapArr.GetAt(pos+7)->layer->getName());
   m_newLayer8.SetWindowText(layerMapArr.GetAt(pos+7)->newLayer);

   UpdateData(FALSE);
}

void Layer_LayerMap::ReadData()
{
   UpdateData();

   int pos = m_scroll.GetScrollPos();
   int count = layerMapArr.GetCount();

   if (count < 1) return; 
   m_newLayer1.GetWindowText(layerMapArr.GetAt(pos)->newLayer);

   if (count < 2) return; 
   m_newLayer2.GetWindowText(layerMapArr.GetAt(pos+1)->newLayer);
                                             
   if (count < 3) return;
   m_newLayer3.GetWindowText(layerMapArr.GetAt(pos+2)->newLayer);

   if (count < 4) return; 
   m_newLayer4.GetWindowText(layerMapArr.GetAt(pos+3)->newLayer);

   if (count < 5) return; 
   m_newLayer5.GetWindowText(layerMapArr.GetAt(pos+4)->newLayer);

   if (count < 6) return; 
   m_newLayer6.GetWindowText(layerMapArr.GetAt(pos+5)->newLayer);

   if (count < 7) return; 
   m_newLayer7.GetWindowText(layerMapArr.GetAt(pos+6)->newLayer);

   if (count < 8) return; 
   m_newLayer8.GetWindowText(layerMapArr.GetAt(pos+7)->newLayer);
}

void Layer_LayerMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   ReadData();

   int pos = m_scroll.GetScrollPos();

   switch (nSBCode)
   {
   case SB_TOP:
      pos = 0;
      break;
   case SB_BOTTOM:
      pos = layerMapArr.GetCount() - 8;
      break;

   case SB_PAGEDOWN:
      pos += 8;
      break;
   case SB_PAGEUP:
      pos -= 8;
      break;

   case SB_LINEDOWN:
      pos++;
      break;
   case SB_LINEUP:
      pos--;
      break;

   case SB_THUMBPOSITION:
   case SB_THUMBTRACK:
      pos = nPos;
      break;
   }
   m_scroll.SetScrollPos(pos);
   WriteData(m_scroll.GetScrollPos());
}

void Layer_LayerMap::OnOK() 
{
   ReadData();
   EndDialog(IDOK);
}

void Layer_LayerMap::OnCancel() 
{
   Free_Layer_Layername(); 
   CDialog::OnCancel();
}

void Layer_LayerMap::OnLoad() 
{
   FILE *stream;
   char line[81];
   char *tok;

   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(TRUE, "LYR", "*.LYR",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "CAMCAD Data File (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());

   // Open File
   stream = fopen(FileName, "r"); // text mode by default
   if (stream == NULL)
   {
      ErrorMessage(FileName, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // Check File Format
   fgets(line, 80, stream);
   if (strcmp(line,"* Layer Map File\n"))
   {
      ErrorMessage("This is Not a Layer Map File", FileName, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   while (fgets(line, 80, stream) != NULL)
   {
      // get original layer name
      if ((tok = get_string(line, " \t\n")) == NULL)
         continue;

      // loop through layer names to find a match
      for (int i=0; i < layerMapArr.GetCount(); i++)
      {
         LayerMapStruct* layerMap = layerMapArr.GetAt(i);
         if (layerMap == NULL || layerMap->layer == NULL)
            continue;

         if (!strcmp(tok, layerMap->layer->getName())) // if found a match
         {
            if ((tok = get_string(NULL, " \t\n")) == NULL)
               break; // get ->

            if ((tok = get_string(NULL, "\n")) == NULL)
               break; // get new name (allow spaces in name)

            layerMap->newLayer = tok;
            layerMap->newLayer.Trim();
            break;
         }
      }
   }
   fclose(stream);
   WriteData(m_scroll.GetScrollPos());
}

void Layer_LayerMap::OnSave() 
{
   ReadData();
   FILE *stream;
   char FileName[OFS_MAXPATHNAME];
   CFileDialog FileDialog(FALSE, "LYR", "*.LYR",
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Layer Map Files (*.LYR)|*.LYR|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   strcpy(FileName, FileDialog.GetPathName());

   stream = fopen(FileName, "w+t");
   if (stream == NULL)
   {
      ErrorMessage(FileName, "Unable To Open File", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fputs("* Layer Map File\n", stream);

   for (int i=0; i<layerMapArr.GetCount(); i++)
   {
      LayerMapStruct* layerMap = layerMapArr.GetAt(i);
      if (layerMap != NULL && layerMap->layer != NULL)
      {
         fprintf(stream, "%-20s -> %s\n", layerMap->layer->getName(), layerMap->newLayer);
      }
   }

   fclose(stream);
}

void Layer_LayerMap::NoShow() 
{
   OnInitDialog();
   ReadData();
}
