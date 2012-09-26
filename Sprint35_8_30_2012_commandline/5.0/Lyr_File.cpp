// $Header: /CAMCAD/4.5/Lyr_File.cpp 15    4/28/06 2:14p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "lyr_file.h"
#include "dirdlg.h"
#include "direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAX_FILES 255

struct LayerArrayStruct 
{
   LayerStruct *layer;
   char files[MAX_FILES];
};
static LayerArrayStruct *LayerArr;

CFileLayerList *FileLayerList;

/*************************************************************************
* 
*/
static int legal_filename(const char *n)
{
   if (strlen(n) < 1)            return FALSE;
   if (strlen(n) >= _MAX_FNAME)  return FALSE;

   // only allow a..z, 0..9 or _
   for (unsigned int i=0;i<strlen(n);i++)
   {
      if (!__iscsym(n[i]) && n[i] != 32)
         return FALSE;
   }

   return TRUE;
}

/*************************************************************************
* LayerInFile
*  returns true if (filenum,layernum) is in this files list of layers to include for this file
*/
BOOL LayerInFile(FileLayerStruct *FileLayerPtr, int layernum)
{
   LayerStruct *p;
   POSITION pos = FileLayerPtr->LayerList.GetHeadPosition();
   while (pos != NULL)
   {
      p = FileLayerPtr->LayerList.GetNext(pos);

      if (p->getLayerIndex() == layernum)
         return TRUE;
   }
   return FALSE;
}

/*************************************************************************
* Free_File_LayerMap */
void Free_File_LayerMap()
{
   FileLayerStruct *p;

   POSITION pos = FileLayerList->GetHeadPosition();
   while (pos != NULL)
   {
      p = FileLayerList->GetNext(pos); 
      p->LayerList.RemoveAll();
      delete p;
   }

   FileLayerList->RemoveAll();

   delete FileLayerList;
}

/////////////////////////////////////////////////////////////////////////////
// LayerFileMap dialog
LayerFileMap::LayerFileMap(CWnd* pParent /*=NULL*/)
   : CDialog(LayerFileMap::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerFileMap)
   m_Path = _T("");
   m_file1 = _T("");
   m_file2 = _T("");
   m_file3 = _T("");
   m_file4 = _T("");
   m_file5 = _T("");
   m_file6 = _T("");
   m_file7 = _T("");
   m_file8 = _T("");
   m_layer1 = _T("");
   m_layer2 = _T("");
   m_layer3 = _T("");
   m_layer4 = _T("");
   m_layer5 = _T("");
   m_layer6 = _T("");
   m_layer7 = _T("");
   m_layer8 = _T("");
   m_num1 = _T("");
   m_num2 = _T("");
   m_num3 = _T("");
   m_num4 = _T("");
   m_num5 = _T("");
   m_num6 = _T("");
   m_num7 = _T("");
   m_num8 = _T("");
   //}}AFX_DATA_INIT
}


void LayerFileMap::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerFileMap)
   DDX_Control(pDX, IDC_SCROLLBAR, m_scroll);
   DDX_Text(pDX, IDC_PATH, m_Path);
   DDX_Text(pDX, IDC_FILE1, m_file1);
   DDX_Text(pDX, IDC_FILE2, m_file2);
   DDX_Text(pDX, IDC_FILE3, m_file3);
   DDX_Text(pDX, IDC_FILE4, m_file4);
   DDX_Text(pDX, IDC_FILE5, m_file5);
   DDX_Text(pDX, IDC_FILE6, m_file6);
   DDX_Text(pDX, IDC_FILE7, m_file7);
   DDX_Text(pDX, IDC_FILE8, m_file8);
   DDX_Text(pDX, IDC_LAYER1, m_layer1);
   DDX_Text(pDX, IDC_LAYER2, m_layer2);
   DDX_Text(pDX, IDC_LAYER3, m_layer3);
   DDX_Text(pDX, IDC_LAYER4, m_layer4);
   DDX_Text(pDX, IDC_LAYER5, m_layer5);
   DDX_Text(pDX, IDC_LAYER6, m_layer6);
   DDX_Text(pDX, IDC_LAYER7, m_layer7);
   DDX_Text(pDX, IDC_LAYER8, m_layer8);
   DDX_Text(pDX, IDC_NUM1, m_num1);
   DDX_Text(pDX, IDC_NUM2, m_num2);
   DDX_Text(pDX, IDC_NUM3, m_num3);
   DDX_Text(pDX, IDC_NUM4, m_num4);
   DDX_Text(pDX, IDC_NUM5, m_num5);
   DDX_Text(pDX, IDC_NUM6, m_num6);
   DDX_Text(pDX, IDC_NUM7, m_num7);
   DDX_Text(pDX, IDC_NUM8, m_num8);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LayerFileMap, CDialog)
   //{{AFX_MSG_MAP(LayerFileMap)
   ON_WM_VSCROLL()
   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerFileMap message handlers

void LayerFileMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   ReadData();

   int pos = m_scroll.GetScrollPos();

   switch (nSBCode)
   {
   case SB_TOP:
      pos = 0;
      break;
   case SB_BOTTOM:
      pos = count - 8;
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

BOOL LayerFileMap::OnInitDialog() 
{
   CDialog::OnInitDialog();

   char path[_MAX_PATH];
   LayerStruct *layer;

   if (!doc->GetProjectPath().IsEmpty())
      m_Path = doc->GetProjectPath();
   else if (GetCurrentDirectory(_MAX_PATH, path))
      m_Path = path;

   if (m_Path.Right(1) != '\\')
      m_Path += '\\';

   count = 0;
	int j=0;
   for (j=0; j< doc->getMaxLayerIndex(); j++)
   {
      if (doc->getLayerArray()[j] == NULL)        continue;
      //if (doc->LayerArray[j]->floating)    continue;
      if (doc->getLayerArray()[j]->isVisible())
         count++;
   }
   if (count > 8)
      m_scroll.SetScrollRange(0, count - 8);
   else m_scroll.SetScrollRange(0, 0);
   m_scroll.SetScrollPos(0);

   LayerArr = (LayerArrayStruct *)calloc(count, sizeof(LayerArrayStruct));

   if (LayerArr == NULL)
   {
      ErrorMessage("Memory allocation failure", "LayerFileMap::OnInitDialog");
      return TRUE;
   }

   int i = 0;
   for (j=0; j< doc->getMaxLayerIndex(); j++)
   {
      if (doc->getLayerArray()[j] == NULL)        continue;
      layer = doc->getLayerArray()[j];
      //if (layer->isFloating())              continue; // need to allow, because we can not guarantee that DXF is not only using 0.
      if (!layer->isVisible()) continue;
      LayerArr[i].layer = layer;

      // open file for writing
      char drive[_MAX_DRIVE];
      char dir[_MAX_DIR];
      char fname[_MAX_FNAME];
      char ext[_MAX_EXT];

      // try to extract a good filename from the layer !
      _splitpath( layer->getName(), drive, dir, fname, ext );

      char *filename = _strlwr(fname);
      if (!strlen(filename) || !legal_filename(filename))
      {
         // here try to make a filename from the layer
         sprintf(LayerArr[i].files, "Layer%02d", i+1);
         strcat(LayerArr[i].files, extension);
      }
      else
      {
         sprintf(LayerArr[i].files, "%s", filename);
         // barco must have the .dpf extension
         if (read_write_type == Type_Barco)
         {
            if (strlen(ext) && STRCMPI(ext, extension))  // also must be NOT equal to the default ext.
            {
               if (ext[0] == '.')   ext[0] = '_';
               strcat(LayerArr[i].files, _strlwr(ext));
            }
            strcat(LayerArr[i].files, extension);
         }
         else
         {
            if (strlen(ext))
               strcat(LayerArr[i].files, _strlwr(ext));
            else
               strcat(LayerArr[i].files, extension);
         }
      }

      i++;
   }

   WriteData(0);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerFileMap::WriteData(int pos)
{
   if (count < 1) { UpdateData(FALSE); return; }
   m_num1.Format("%2d.",   LayerArr[pos+0].layer->getLayerIndex());
   m_layer1 =              LayerArr[pos+0].layer->getName();
   m_file1  =              LayerArr[pos+0].files;

   if (count < 2) { UpdateData(FALSE); return; }
   m_num2.Format("%2d.",   LayerArr[pos+1].layer->getLayerIndex());
   m_layer2 =              LayerArr[pos+1].layer->getName();
   m_file2  =              LayerArr[pos+1].files;

   if (count < 3) { UpdateData(FALSE); return; }
   m_num3.Format("%2d.",   LayerArr[pos+2].layer->getLayerIndex());
   m_layer3 =              LayerArr[pos+2].layer->getName();
   m_file3  =              LayerArr[pos+2].files;

   if (count < 4) { UpdateData(FALSE); return; }
   m_num4.Format("%2d.",   LayerArr[pos+3].layer->getLayerIndex());
   m_layer4 =              LayerArr[pos+3].layer->getName();
   m_file4  =              LayerArr[pos+3].files;

   if (count < 5) { UpdateData(FALSE); return; }
   m_num5.Format("%2d.",   LayerArr[pos+4].layer->getLayerIndex());
   m_layer5 =              LayerArr[pos+4].layer->getName();
   m_file5  =              LayerArr[pos+4].files;

   if (count < 6) { UpdateData(FALSE); return; }
   m_num6.Format("%2d.",   LayerArr[pos+5].layer->getLayerIndex());
   m_layer6 =              LayerArr[pos+5].layer->getName();
   m_file6  =              LayerArr[pos+5].files;

   if (count < 7) { UpdateData(FALSE); return; }
   m_num7.Format("%2d.",   LayerArr[pos+6].layer->getLayerIndex());
   m_layer7 =              LayerArr[pos+6].layer->getName();
   m_file7  =              LayerArr[pos+6].files;

   if (count < 8) { UpdateData(FALSE); return; }
   m_num8.Format("%2d.",   LayerArr[pos+7].layer->getLayerIndex());
   m_layer8 =              LayerArr[pos+7].layer->getName();
   m_file8  =              LayerArr[pos+7].files;

   UpdateData(FALSE);
}

void LayerFileMap::ReadData()
{
   int pos = m_scroll.GetScrollPos();

   UpdateData();

   if (count < 1) return; 
   CpyStr(LayerArr[pos+0].files, m_file1, MAX_FILES);

   if (count < 2) return; 
   CpyStr(LayerArr[pos+1].files, m_file2, MAX_FILES);

   if (count < 3) return;
   CpyStr(LayerArr[pos+2].files, m_file3, MAX_FILES);

   if (count < 4) return; 
   CpyStr(LayerArr[pos+3].files, m_file4, MAX_FILES);

   if (count < 5) return; 
   CpyStr(LayerArr[pos+4].files, m_file5, MAX_FILES);

   if (count < 6) return; 
   CpyStr(LayerArr[pos+5].files, m_file6, MAX_FILES);

   if (count < 7) return; 
   CpyStr(LayerArr[pos+6].files, m_file7, MAX_FILES);

   if (count < 8) return; 
   CpyStr(LayerArr[pos+7].files, m_file8, MAX_FILES);
}

void LayerFileMap::OnOK() 
{
   int layer;
   char *tok;
   CString file;
   POSITION fpos;
   FileLayerStruct *fp;

   ReadData();

   MakeDirectory(m_Path);

   if (m_Path.Right(1) != "\\")
      m_Path += '\\';

   // new linked list of file names
   FileLayerList = new CFileLayerList;

   // loop layers
   for (layer = 0; layer < count; layer++)
   {
      // loop files for this layer to be exported to
      tok = strtok(LayerArr[layer].files, ",;\n");
      while (tok != NULL)
      {
         BOOL Found = FALSE;
         file = m_Path;
         file += tok;

         // search files in file list
         fpos = FileLayerList->GetHeadPosition();
         while (fpos != NULL)
         {
            fp = FileLayerList->GetNext(fpos);     
            if (!fp->file.Compare(file))
            {
               Found = TRUE;
               // add layer to this file
               fp->LayerList.AddTail(LayerArr[layer].layer);
               break;
            }
         } // end search files in file list

         // if file name is not in file list, add it
         if (!Found)
         {
            FileLayerStruct *fp = new FileLayerStruct;
            fp->file = file;
            fp->LayerList.AddTail(LayerArr[layer].layer);
            FileLayerList->AddTail(fp);
         }

         tok = strtok(NULL, ",;\n ");
      } // end loop file for this layer
   } // end loop layers

   // clean up
   free(LayerArr);

   CDialog::OnOK();
}

void LayerFileMap::OnCancel() 
{
   // clean up
   free(LayerArr);
   
   CDialog::OnCancel();
}

void LayerFileMap::OnBrowse() 
{
   UpdateData();

   CBrowse dlg;
   dlg.m_strSelDir = m_Path;
   dlg.m_strTitle = "Select path for exported Gerber files";
   if (dlg.DoBrowse())
   {
      m_Path = dlg.m_strPath;
      UpdateData(FALSE);
   }
}
