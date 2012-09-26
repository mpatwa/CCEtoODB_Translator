// $Header: /CAMCAD/4.6/Measure.cpp 14    3/27/07 3:42p Lynn Phung $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include "resource.h"
#include "measure.h"
#include "mainfrm.h"
#include <math.h>
#include "crypt.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW               
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL Editing; // from SELECT.CPP

/******************************************************************************
* OnMeasure
*/
void CCEtoODBView::OnMeasure() 
{       
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License to Query!");
      return;
   }*/
#ifndef SHAREWARE

   //if (cursorMode == Measure)
   //{
   //   //measureDlg = NULL;
   //   OnMeasureTerminate();
   //}

	
	//CMeasure& measure = getDefineMeasure(measureDialogTypeDefault);
   if (this->m_measure != NULL)
		OnMeasureTerminate();
   else
   {
      CMeasure& measure = getDefinedMeasure(measureDialogTypeDefault);
      measure.setMeasureOn(true);
      CMeasureDlg* measureDlg = (CMeasureDlg*)&measure.getDialog();
      measureDlg->Create(IDD_MEASURE);
      measureDlg->ShowWindow(SW_SHOW);
      measureDlg->InitMeasureDlg(TRUE);
  }
#endif
}

//-----------------------------------------------------------------------------
// CMeasureDlg dialog
//-----------------------------------------------------------------------------
CMeasureDlg::CMeasureDlg(CMeasure& measure)
	: CMeasureBaseDialog(CMeasureDlg::IDD, measure)
{
   //{{AFX_DATA_INIT(CMeasureDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void CMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMeasureDlg)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMeasureDlg, CDialog)
   //{{AFX_MSG_MAP(CMeasureDlg)
   ON_BN_CLICKED(IDC_NEW, OnNewMeasurement)
   ON_BN_CLICKED(IDC_FROM_CENTER, OnFromCenter)
   ON_BN_CLICKED(IDC_FROM_ENTITY, OnFromEntity)
   ON_BN_CLICKED(IDC_FROM_POINT, OnFromPoint)
   ON_BN_CLICKED(IDC_TO_CENTER, OnToCenter)
   ON_BN_CLICKED(IDC_TO_ENTITY, OnToEntity)
   ON_BN_CLICKED(IDC_TO_POINT, OnToPoint)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasureDlg message handlers

BOOL CMeasureDlg::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg->wParam == VK_ESCAPE)   
   {
      OnNewMeasurement();
      return TRUE;
   }
   else
   {      
      return CMeasureBaseDialog::PreTranslateMessage(pMsg);
   }
}

void CMeasureDlg::PostNcDestroy() 
{
   //delete this;
   //measureDlg = NULL;
   //CDialog::PostNcDestroy();
}

void CMeasureDlg::InitMeasureDlg(BOOL UpdateRadioButtons)
{
   GetDlgItem(IDC_FROM_X)->SetWindowText("");
   GetDlgItem(IDC_FROM_Y)->SetWindowText("");
   GetDlgItem(IDC_TO_X)->SetWindowText("");
   GetDlgItem(IDC_TO_Y)->SetWindowText("");
   GetDlgItem(IDC_DISTANCE)->SetWindowText("");
   GetDlgItem(IDC_ANGLE)->SetWindowText("");
   GetDlgItem(IDC_DELTA_X)->SetWindowText("");
   GetDlgItem(IDC_DELTA_Y)->SetWindowText("");

   if (UpdateRadioButtons)
   {
      ((CButton*)GetDlgItem(IDC_FROM_POINT))->SetCheck(m_measure.getFromObject().getType() == measureObjectPoint);
      ((CButton*)GetDlgItem(IDC_FROM_ENTITY))->SetCheck(m_measure.getFromObject().getType() == measureObjectEntity || m_measure.getFromObject().getType() == measureObjectEntityCenter);
      ((CButton*)GetDlgItem(IDC_TO_POINT))->SetCheck(m_measure.getToObject().getType() == measureObjectPoint);
      ((CButton*)GetDlgItem(IDC_TO_ENTITY))->SetCheck(m_measure.getToObject().getType() == measureObjectEntity || m_measure.getToObject().getType() == measureObjectEntityCenter);
   }
   //m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_TerminateMeasure,0);

}

void CMeasureDlg::updateMeasure()
{
   CString buf;
   int decimals = GetDecimals(m_measure.getCamCadDoc().getSettings().getPageUnits());

   InitMeasureDlg(FALSE);

   if (m_measure.isMeasureValid())
   {
      buf.Format("%.*lf", decimals, m_measure.getDistance());
      GetDlgItem(IDC_DISTANCE)->SetWindowText(buf);

      buf.Format("%.2lf°", m_measure.getAngleInDegree());
      GetDlgItem(IDC_ANGLE)->SetWindowText(buf);

      buf.Format("%+.*lf", decimals, m_measure.getDeltaX());
      GetDlgItem(IDC_DELTA_X)->SetWindowText(buf);

      buf.Format("%+.*lf", decimals, m_measure.getDeltaY());
      GetDlgItem(IDC_DELTA_Y)->SetWindowText(buf);
   }

   if (m_measure.getFromObject().isSet())
   {
      buf.Format("%.*lf", decimals, m_measure.getFromObject().getPoint().x);
      GetDlgItem(IDC_FROM_X)->SetWindowText(buf);

      buf.Format("%.*lf", decimals, m_measure.getFromObject().getPoint().y);
      GetDlgItem(IDC_FROM_Y)->SetWindowText(buf);
   }

   if (m_measure.getToObject().isSet())
   {
      buf.Format("%.*lf", decimals, m_measure.getToObject().getPoint().x);
      GetDlgItem(IDC_TO_X)->SetWindowText(buf);

      buf.Format("%.*lf", decimals, m_measure.getToObject().getPoint().y);
      GetDlgItem(IDC_TO_Y)->SetWindowText(buf);
   }
}

void CMeasureDlg::updateMeasureFromObject()
{
   OnNewMeasurement();

   bool isPointChecked = ((CButton*)GetDlgItem(IDC_FROM_POINT))->GetCheck() == BST_CHECKED;
   bool isCenterChecked = ((CButton*)GetDlgItem(IDC_FROM_CENTER))->GetCheck() == BST_CHECKED;

   if (isPointChecked)
   {
      m_measure.getFromObject().setType(measureObjectPoint);
   }
   else
   {
      if (isCenterChecked)
      {
         m_measure.getFromObject().setType(measureObjectEntityCenter);
      }
      else
      {
         m_measure.getFromObject().setType(measureObjectEntity);
      }
   }
}

void CMeasureDlg::updateMeasureToObject()
{
   OnNewMeasurement();

   bool isPointChecked = ((CButton*)GetDlgItem(IDC_TO_POINT))->GetCheck() == BST_CHECKED;
   bool isCenterChecked = ((CButton*)GetDlgItem(IDC_TO_CENTER))->GetCheck() == BST_CHECKED;

   if (isPointChecked)
   {
      m_measure.getToObject().setType(measureObjectPoint);
   }
   else
   {
      if (isCenterChecked)
      {
         m_measure.getToObject().setType(measureObjectEntityCenter);
      }
      else
      {
         m_measure.getToObject().setType(measureObjectEntity);
      }
   }
}


void CMeasureDlg::OnCancel() 
{
//   m_measure.cleanGhosts();
   m_measure.getCamCadView().PostMessage(WM_COMMAND,ID_TerminateMeasure,0);
}

void CMeasureDlg::OnNewMeasurement() 
{
   //m_measure.cleanGhosts();
   //m_measure.reset();
   m_measure.setMeasureOn(true);
   InitMeasureDlg(FALSE);
}

void CMeasureDlg::OnFromCenter() 
{
   updateMeasureFromObject();
}

void CMeasureDlg::OnFromEntity() 
{
   updateMeasureFromObject();
}

void CMeasureDlg::OnFromPoint() 
{
   updateMeasureFromObject();
}

void CMeasureDlg::OnToCenter() 
{
   updateMeasureToObject();
}

void CMeasureDlg::OnToEntity() 
{
   updateMeasureToObject();
}

void CMeasureDlg::OnToPoint() 
{
   updateMeasureToObject();
}

