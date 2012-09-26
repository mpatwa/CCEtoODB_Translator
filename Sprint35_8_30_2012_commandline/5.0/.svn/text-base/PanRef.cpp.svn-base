// $Header: /CAMCAD/5.0/PanRef.cpp 32    6/21/07 8:26p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/

#include "stdafx.h"
#include "panref.h"
#include "api.h"
#include "extents.h"
#include <float.h>
#include "CCEtoODB.h"

extern CView *activeView; // from CCVIEW.CPP
extern CCEtoODBView *apiView; // from API.CPP
extern BOOL Editing;

int PanReference(CCEtoODBView *view, const char *ref);

PanToReference *panToReferenceDlg = NULL;

/******************************************************************************
* ComponentsExtents
*/
BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, CDblRect *rect, int SELECT_COMP)
{
	double xmin, xmax, ymin, ymax;
	BOOL retVal = ComponentsExtents(doc, string, &xmin, &xmax, &ymin, &ymax, SELECT_COMP);

	rect->xMin = xmin;
	rect->xMax = xmax;
	rect->yMin = ymin;
	rect->yMax = ymax;

	return retVal;
}

BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, double *xmin, double *xmax, double *ymin, double *ymax, int SELECT_COMP)
{
   char *buf = STRDUP(string);

   *xmin = *ymin = DBL_MAX;
   *xmax = *ymax = -DBL_MAX;
   BOOL found = FALSE;

   char *tok = strtok(buf, ", \t\n");
   while (tok)
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);
         if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
            continue;

			double fileRot = file->getRotation();
			int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

         POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
         while (dataPos != NULL)
         {
            DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

            if (data->getDataType() == T_INSERT)
            {
               if (data->getInsert()->getRefname() && !strcmp(tok, data->getInsert()->getRefname()))
               {
                  if (SELECT_COMP)
                  {
                     doc->InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), 
                        file->getScale(), file->getRotation(), fileMirror, &(file->getBlock()->getDataList()), FALSE, TRUE);
                     if (Editing)
                        doc->OnEditEntity();
                  }

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

                  found = TRUE;
                  break; // here it is found ! No need to look for more
               
               }
            }
         }
      }

      if (!found)
      {
         filePos = doc->getFileList().GetHeadPosition();
         while (filePos != NULL)
         {
            FileStruct *file = doc->getFileList().GetNext(filePos);
            if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
               continue;

            int mirror = file->getResultantMirror(doc->getBottomView());

            POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
            while (dataPos != NULL)
            {
               DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

               if (data->getDataType() == T_INSERT)
               {
                  if (data->getInsert()->getRefname() && !STRICMP(tok, data->getInsert()->getRefname()))
                  {
                     if (SELECT_COMP)
                     {
                        doc->InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), 
                           file->getScale(), file->getRotation(), mirror, &(file->getBlock()->getDataList()), FALSE, TRUE);
                        if (Editing)
                           doc->OnEditEntity();
                     }

                     Mat2x2 m;
                     RotMat2(&m, file->getRotation());

                     Point2 point2;
                     point2.x = data->getInsert()->getOriginX() * file->getScale();
                     point2.y = data->getInsert()->getOriginY() * file->getScale();
                     if (mirror & MIRROR_FLIP)  point2.x = -point2.x;
                     TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

                     double _rot;
                     if (mirror & MIRROR_FLIP)
                        _rot = file->getRotation() - data->getInsert()->getAngle();
                     else
                        _rot = file->getRotation() + data->getInsert()->getAngle();
                     int _mirror = mirror ^ data->getInsert()->getMirrorFlags();

                     BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
                     doc->validateBlockExtents(block);

                     if (block->extentIsValid())
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

                     found = TRUE;
                     break; // here it is found ! No need to look for more
                  }
               }
            }
         }
      }

      tok = strtok(NULL, ", \t\n");
   }
   free(buf);

   if (*xmin > *xmax)
      return FALSE;

   return TRUE;
}

/******************************************************************************
* ZoomExtentsOfComponents
*/
short API::ZoomExtentsOfComponents(LPCTSTR compNames) 
{
   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   double xmin, xmax, ymin, ymax;
   int Select_component = TRUE;

   if (!ComponentsExtents(doc, compNames, &xmin, &xmax, &ymin, &ymax, Select_component))
      return RC_GENERAL_ERROR;

   double margin = (xmax - xmin) * doc->getSettings().ExtentMargin / 200;
   xmin -= margin;
   xmax += margin;
   
   margin = (ymax - ymin) * doc->getSettings().ExtentMargin / 200;
   ymin -= margin;
   ymax += margin;

   ZoomWindow(xmin, ymin, xmax, ymax);

   return RC_SUCCESS;
}

/******************************************************************************
* ZoomExtentsOfComponentsMargin
*/
short API::ZoomExtentsOfComponentsMargin(LPCTSTR compNames, short margin) 
{
   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CCEtoODBDoc *doc = apiView->GetDocument();

   double xmin, xmax, ymin, ymax;
   int Select_component = TRUE;

   if (!ComponentsExtents(doc, compNames, &xmin, &xmax, &ymin, &ymax, Select_component))
      return RC_GENERAL_ERROR;

   double pageWidth = doc->getSettings().getXmax() - doc->getSettings().getXmin();
   double pageHeight = doc->getSettings().getYmax() - doc->getSettings().getYmin();
   double pageSize = max(pageWidth, pageWidth);

   double marginSize = pageSize * margin / 100;

   xmin -= marginSize;
   xmax += marginSize;
   
   ymin -= marginSize;
   ymax += marginSize;

   ZoomWindow(xmin, ymin, xmax, ymax);

   return RC_SUCCESS;
}

/******************************************************************************
* OnPanReference
*/
void CCEtoODBView::OnPanReference() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for Query or Edit!");
      return;
   }*/

   if (panToReferenceDlg)
   {
      delete panToReferenceDlg;
      panToReferenceDlg = NULL;
   }
   else
   {
      panToReferenceDlg = new PanToReference;
      panToReferenceDlg->Create(IDD_PAN_REFERENCE);
      panToReferenceDlg->ShowWindow(SW_SHOW);
   }
}

int CCEtoODBView::PanToRefName(CString refName)
{
	return PanReference(this, refName);
}

/******************************************************************************
* PanReference
*/
int PanReference(CCEtoODBView *view, const char *ref) 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for Query or Edit!");
      return RC_NO_LICENSE;
   }*/

   CCEtoODBDoc *doc = view->GetDocument();

   BOOL found = FALSE;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

		double fileRot = file->getRotation();
      int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT)
         {
            if (data->getInsert()->getRefname() && !strcmp(ref, data->getInsert()->getRefname()))
            {
               SelectStruct *s = doc->InsertInSelectList(data, file->getBlock()->getFileNumber(),
							file->getInsertX(), file->getInsertY(),  file->getScale(), file->getRotation(), fileMirror,
							&(file->getBlock()->getDataList()), FALSE, TRUE);
					doc->DrawEntity(s, 3, FALSE);

               Point2 pnt;
               pnt.x = data->getInsert()->getOriginX() * file->getScale();
               pnt.y = data->getInsert()->getOriginY() * file->getScale();
               if (fileMirror & MIRROR_FLIP)
					{
						pnt.x = -pnt.x;
						fileRot = -fileRot;
					}

					Mat2x2 m;
               RotMat2(&m, fileRot);
               TransPoint2(&pnt, 1, &m, file->getInsertX(), file->getInsertY());

               view->PanCoordinate(pnt.x, pnt.y, TRUE);
               found = TRUE;
               return RC_SUCCESS;
            }
         }
      }
   }
   if (!found)
   {
      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);
         if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
            continue;

         int mirror = file->getResultantMirror(doc->getBottomView());

         POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
         while (dataPos != NULL)
         {
            DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

            if (data->getDataType() == T_INSERT)
            {
               if (data->getInsert()->getRefname() && !STRICMP(ref, data->getInsert()->getRefname()))
               {
                  doc->InsertInSelectList(data, file->getBlock()->getFileNumber(), file->getInsertX(), file->getInsertY(), 
                        file->getScale(), file->getRotation(), mirror, &(file->getBlock()->getDataList()), FALSE, TRUE);

                  Mat2x2 m;
                  RotMat2(&m, file->getRotation());

                  Point2 pnt;
                  pnt.x = data->getInsert()->getOriginX() * file->getScale();
                  pnt.y = data->getInsert()->getOriginY() * file->getScale();
                  if (mirror & MIRROR_FLIP)  pnt.x = -pnt.x;
                  TransPoint2(&pnt, 1, &m, file->getInsertX(), file->getInsertY());

                  view->PanCoordinate(pnt.x, pnt.y, TRUE);
                  found = TRUE;
                  return RC_SUCCESS;
               }
            }
         }
      }
   }
  

   return RC_ITEM_NOT_FOUND;
}


/////////////////////////////////////////////////////////////////////////////
// PanToReference dialog
PanToReference::PanToReference(CWnd* pParent /*=NULL*/)
   : CDialog(PanToReference::IDD, pParent)
{
   //{{AFX_DATA_INIT(PanToReference)
   m_ref = _T("");
   //}}AFX_DATA_INIT
}

void PanToReference::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PanToReference)
   DDX_Control(pDX, IDC_REF, m_refCB);
   DDX_CBString(pDX, IDC_REF, m_ref);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PanToReference, CDialog)
   //{{AFX_MSG_MAP(PanToReference)
   ON_BN_CLICKED(IDC_FILL, OnFill)
   ON_BN_CLICKED(IDC_ZOOM_TO, OnZoomTo)
   ON_BN_CLICKED(IDC_PAN_TO, OnPanTo)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PanToReference::OnFill() 
{  
   m_refCB.ResetContent();
   
   if (!activeView)
      return;

   CCEtoODBDoc *doc = (CCEtoODBDoc*)activeView->GetDocument();

   CWaitCursor wait;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden() || file->notPlacedYet() || file->getBlock() == NULL)
         continue;

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT && data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()) &&
            !(DoesStringExist(data->getInsert()->getRefname())))   // not already in list
            m_refCB.AddString(data->getInsert()->getRefname());
      }
   }
   
   //m_refCB.ShowDropDown();
}


/******************************************************************************
* PanToReference::DoesStringExist

This function was added in order to be able to addsimilar ref names with different case. 
Before, if we had U1 and u1, only U1 would show in the refdes list; now, both would show up.

*/
int PanToReference::DoesStringExist(const CString& refName)
{
   int nIndex;
   nIndex = m_refCB.FindStringExact(0, refName);
   
   while (nIndex != CB_ERR) // if the string was find (case insensitive)
   {
      CString foundString;

      m_refCB.GetLBText(nIndex, foundString); // get the string that was found
      
      // compare the string in the list with the ref name (case sensitive)
      if (foundString == refName) 
         return 1; // found (case sensitive)
      else
      {
         break; // not found (case sensitive)
      }
   }
   return 0;
}


void PanToReference::OnZoomTo() 
{
   if (!activeView)
      return;

   CCEtoODBDoc *doc = (CCEtoODBDoc*)activeView->GetDocument();

   UpdateData();

   double xmin, xmax, ymin, ymax;

   if (!ComponentsExtents(doc, m_ref, &xmin, &xmax, &ymin, &ymax, TRUE))
   {
      PanReference(((CCEtoODBView*)activeView), m_ref);
      ErrorMessage("Component has no extents", "Panning");
      return;
   }

   double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * doc->getSettings().CompExtentMargin / 100;
   double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * doc->getSettings().CompExtentMargin / 100;

   double marginSize = max(marginSizeX, marginSizeY);

   xmin -= marginSize;
   xmax += marginSize;
   
   ymin -= marginSize;
   ymax += marginSize;

   ((CCEtoODBView*)activeView)->ZoomBox(xmin, xmax, ymin, ymax);
}

void PanToReference::OnPanTo() 
{
   UpdateData();
   PanReference(((CCEtoODBView*)activeView), m_ref);
}

void PanToReference::PostNcDestroy() 
{
   delete this;
   panToReferenceDlg = NULL;  
   CDialog::PostNcDestroy();
}

void PanToReference::OnCancel() 
{
   DestroyWindow();  
}

