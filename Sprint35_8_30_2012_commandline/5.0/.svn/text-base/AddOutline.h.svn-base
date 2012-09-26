// AddOutline.H

#if ! defined (__AddOutline_h__)
#define __AddOutline_h__

#pragma once

#include "CCDoc.h"
#include "ResizingDialog.h"
#include "CamCadDatabase.h"

//-----------------------------------------------------------------------------
// CAddOutlineDlg
//-----------------------------------------------------------------------------
class CAddOutlineDlg : public CResizingDialog
{
   // DECLARE_DYNAMIC(CAddOutlineDlg)

private:
   CCEtoODBDoc& m_doc;
   FileStruct& m_File;
   double m_xLocation;
   double m_xOffset;
   double m_yLocation;
   double m_yOffset;

   CEdit m_xLocationEditbox;
   CEdit m_yLocationEditbox;
   CEdit m_xOffsetEditbox;
   CEdit m_yOffsetEditbox;

public:
   CAddOutlineDlg(CCEtoODBDoc& doc, FileStruct& pcbFile, const double x, const double y);
   virtual ~CAddOutlineDlg();
   enum { IDD = IDD_ADD_OUTLINE };
   virtual CString GetDialogProfileEntry() { return CString("AddOutlineDlg"); }

protected:
   virtual void DoDataExchange(CDataExchange* pDX);
   virtual BOOL OnInitDialog();
   DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnBnClickedAdd();
   afx_msg void OnBnClickedClose();
   afx_msg void OnClose();
   LayerStruct * FindLayerByLayerType(int LayerType);
   bool IsLayerPresent(CString LayerName);
};

#endif
