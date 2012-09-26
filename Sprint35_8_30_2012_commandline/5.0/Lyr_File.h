// $Header: /CAMCAD/4.5/Lyr_File.h 10    11/07/05 8:58a Kurt Van Ness $

#include "layer.h"
#include "ccdoc.h"

#if !defined(__Lyr_File_h__)
#define __Lyr_File_h__

#pragma once

struct FileLayerStruct
{
   CString file;
   CLayerList LayerList;

   CString getFilePath() const { return file; }
};

typedef CTypedPtrList<CPtrList, FileLayerStruct*> CFileLayerList;

void Free_File_LayerMap();
BOOL LayerInFile(FileLayerStruct *FileLayerPtr, int layernum);

/////////////////////////////////////////////////////////////////////////////
// LayerFileMap dialog

class LayerFileMap : public CDialog
{
// Construction
public:
   LayerFileMap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(LayerFileMap)
   enum { IDD = IDD_LAYER_FILE };
   CScrollBar  m_scroll;
   CString  m_Path;
   CString  m_file1;
   CString  m_file2;
   CString  m_file3;
   CString  m_file4;
   CString  m_file5;
   CString  m_file6;
   CString  m_file7;
   CString  m_file8;
   CString  m_layer1;
   CString  m_layer2;
   CString  m_layer3;
   CString  m_layer4;
   CString  m_layer5;
   CString  m_layer6;
   CString  m_layer7;
   CString  m_layer8;
   CString  m_num1;
   CString  m_num2;
   CString  m_num3;
   CString  m_num4;
   CString  m_num5;
   CString  m_num6;
   CString  m_num7;
   CString  m_num8;
   //}}AFX_DATA
   CCEtoODBDoc *doc;
   int count;
   CString extension;
   int read_write_type; // Type_xxx for read and write

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(LayerFileMap)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   void WriteData(int pos);
   void ReadData();

   // Generated message map functions
   //{{AFX_MSG(LayerFileMap)
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnBrowse();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif
