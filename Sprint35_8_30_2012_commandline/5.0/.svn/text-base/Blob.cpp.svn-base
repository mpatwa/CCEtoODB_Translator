// $Header: /CAMCAD/4.6/Blob.cpp 14    3/27/07 3:40p Lynn Phung $
     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include <math.h>
#include "ccdoc.h"
#include "graph.h"
#include "CCEtoODB.h"

extern CDialogBar *CurrentSettings; // from MAINFRM.CPP

void CCEtoODBView::OnAddBlob() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Edit!");
      return;
   }*/

   if (zoomMode != NoZoom) 
      return;

#ifndef SHAREWARE
   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   int sel = CurrentFile->GetCurSel();
   if (sel == CB_ERR)
   {
      ErrorMessage("No Current File");
      return;
   }


   if (cursorMode == AddBlob)
   {
      //HideSearchCursor();
      cursorMode = Search;
      //ShowSearchCursor();
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   //HideSearchCursor();
   cursorMode = AddBlob;
   //ShowSearchCursor();
#endif
}

void CCEtoODBView::OnMouseMove_AddBlob(CDC *dc, CPoint *pnt)
{
   if (!cursorAnchored) return;

   dc->SetROP2(R2_NOT);

   if (!cursorFirstDrag)
   {
      dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x, cursorPrevPnt.y);
      dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
      dc->LineTo(cursorPrevPnt.x, cursorAnchor.y);
      dc->LineTo(cursorAnchor.x, cursorAnchor.y);
   }
   cursorFirstDrag = FALSE;

   cursorPrevPnt.x = pnt->x;
   cursorPrevPnt.y = pnt->y;

   dc->MoveTo(cursorAnchor.x, cursorAnchor.y);
   dc->LineTo(cursorAnchor.x, cursorPrevPnt.y);
   dc->LineTo(cursorPrevPnt.x, cursorPrevPnt.y);
   dc->LineTo(cursorPrevPnt.x, cursorAnchor.y);
   dc->LineTo(cursorAnchor.x, cursorAnchor.y);
}

void CCEtoODBView::OnLButtonDown_AddBlob(CDC *dc, double x, double y)
{
   CCEtoODBDoc *doc = GetDocument();
   double factor = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) / (doc->maxXCoord - doc->minXCoord) * ScaleNum;

   if (!cursorAnchored)
   {
      cursorAnchored = TRUE;
      anchor_x = x;
      anchor_y = y;
      cursorAnchor.x = round(x / factor);
      cursorAnchor.y = round(y / factor);
      return;
   }

   cursorFirstDrag = TRUE;
   cursorAnchored = FALSE;
   //HideSearchCursor();
   cursorMode = Search;
   //ShowSearchCursor();

   CComboBox *CurrentFile = (CComboBox*)CurrentSettings->GetDlgItem(IDC_FILECB);
   int sel = CurrentFile->GetCurSel();
   if (sel == CB_ERR)
   {
      ErrorMessage("No Current File");
      return;
   }             

   FileStruct *file = (FileStruct*)(CurrentFile->GetItemDataPtr(sel));
   doc->PrepareAddEntity(file);

   CFileDialog FileDialog(TRUE, NULL, "*.BMP;*.JPG;*.JPEG",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "All Background Formats ((*.BMP;*.JPG;*.JPEG)|*.BMP;*.JPG;*.JPEG|Bitmap Files (*.BMP)|*.BMP|JPEG File Interchange Format (*.jpg;*.jpeg)|*.jpg;*.jpeg|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   CWaitCursor wait;
   CString filename = FileDialog.GetPathName();
   DataStruct *data = Graph_Blob(min(anchor_x, x), min(anchor_y, y), fabs(x - anchor_x), fabs(y - anchor_y), 0, TRUE, TRUE, filename);

   doc->UpdateAllViews(NULL);
}

