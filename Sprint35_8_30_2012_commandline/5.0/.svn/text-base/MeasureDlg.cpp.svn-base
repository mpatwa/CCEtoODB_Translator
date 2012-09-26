
// $Header: /CAMCAD/4.6/MeasureDlg.cpp 3     12/07/06 12:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// MeasureDlg.CPP

#include "stdafx.h"
#include "MeasureDlg.h"
#include "GerberEducatorAlignLayer.h"
#include "Measure.h"
#include "Draw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMeasure& CCEtoODBView::getDefinedMeasure(EMeasureDialogType dialogType)
{
   if (m_measure == NULL)
   {
      m_measure = new CMeasure(*this, *GetDocument(), dialogType);
   } 

   return *m_measure;
}

void CCEtoODBView::OnMeasureTerminate()
{
   delete m_measure;
   m_measure = NULL;
}

//-----------------------------------------------------------------------------
// CMeasureObject
//-----------------------------------------------------------------------------
CMeasureObject::CMeasureObject()
{
	m_type = measureObjectPoint;	
   reset();
}

CMeasureObject::~CMeasureObject()
{
   reset();
}

void CMeasureObject::reset()
{
   m_entity = NULL;
	m_point.x = m_point.y = 0.0;
	m_isSet = false;
}

//-----------------------------------------------------------------------------
// CMeasure
//-----------------------------------------------------------------------------
CMeasure::CMeasure(CCEtoODBView& camCadView, CCEtoODBDoc& camCadDoc, EMeasureDialogType dialogType)
//CMeasure::CMeasure(CCEtoODBDoc& camCadDoc, EMeasureDialogType dialogType)
   : m_camCadView(camCadView)
   , m_camCadDoc(camCadDoc)
   , m_camCadDatabase(camCadDoc)
   , m_dialogType(dialogType)
{
   m_dialog = NULL;
   deleteDialog();
   reset();
}

CMeasure::~CMeasure()
{
   deleteDialog();
//   reset();
   setMeasureOn(false);
}

void CMeasure::deleteDialog()
{
   if (m_dialog != NULL)
   {
      m_dialog->DestroyWindow();
   }

   delete m_dialog;
   m_dialog = NULL;
}

void CMeasure::reset()
{
	m_fromObject.reset();
	m_toObject.reset();
   m_fromInsertX = 0.0;
   m_fromInsertY = 0.0;
   m_fromScale = 1.0;
   m_fromRotation = 0.0;
   m_fromMirror = 0;
   m_fromDataList = NULL;
}

CMeasureBaseDialog& CMeasure::getDialog()
{
   if (m_dialog == NULL)
   {
      switch (m_dialogType)
      {
      case measureDialogTypeAlignLayer:
         {
            m_dialog =  new CGEAlignLayerDlg(*this);
//            m_dialog->Create(IDD_GE_AlignLayer);
         }
         break;
      default:
         {
            m_dialog = new CMeasureDlg(*this);
            //m_dialog->Create(IDD_MEASURE);
         }
         break;
      }
   }

   return *m_dialog;
}

bool CMeasure::isMeasureValid()
{
   return m_fromObject.isSet() && m_toObject.isSet();
}

double CMeasure::getDeltaX()
{
   double retval = 0.0;
   if (isMeasureValid())
   {
      retval =  m_toObject.getPoint().x - m_fromObject.getPoint().x ;
   }

   return retval;
}

double CMeasure::getDeltaY()
{
   double retval = 0.0;
   if (isMeasureValid())
   {
      retval = m_toObject.getPoint().y - m_fromObject.getPoint().y;
   }

   return retval;
}

double CMeasure::getDistance()
{
   double retval = 0.0;
   if (isMeasureValid())
   {
      retval = m_fromObject.getPoint().distance(m_toObject.getPoint());
   }

   return retval;
}

double CMeasure::getAngleInDegree()
{
   return RadToDeg(getAngleInRadian());
}

double CMeasure::getAngleInRadian()
{
   double retval = 0.0;
   if (isMeasureValid())
   {
      retval = ArcTan2(getDeltaY(), getDeltaX());
   }

   return retval;
}

void CMeasure::setMeasureOn(const bool on)
{
   // Must clean ghosts before reset
   cleanGhosts();
   reset();

   m_measureOn = on;

   if (on)
      m_camCadView.cursorMode = Measure;
   else
      m_camCadView.cursorMode = Search;
}

void CMeasure::cleanGhosts()
{
   if (m_fromObject.isSet())
   {
      double factor = (m_camCadDoc.getSettings().getXmax() - m_camCadDoc.getSettings().getXmin()) / (m_camCadDoc.maxXCoord - m_camCadDoc.minXCoord);
      CClientDC dc(&m_camCadView);
      //m_camCadView.OnPrepareDC(&dc);

      if (m_fromObject.getType() == measureObjectPoint)
      {
         //m_camCadView.DrawCross(&dc, round(m_fromObject.getPoint().x / factor), round(m_fromObject.getPoint().y / factor), 1.0);
         if (m_toObject.getType() == measureObjectPoint) // must clean rubber banding line in Point to Point
         {
            // diagonal
            dc.MoveTo(m_camCadView.prevPnt);
            dc.LineTo(round(m_fromObject.getPoint().x / factor), round(m_fromObject.getPoint().y / factor));

            if (m_dialogType !=  measureDialogTypeAlignLayer)
            {
               // box
               dc.LineTo(round(m_fromObject.getPoint().x / factor), m_camCadView.prevPnt.y);
               dc.LineTo(m_camCadView.prevPnt);
               dc.LineTo(m_camCadView.prevPnt.x, round(m_fromObject.getPoint().y / factor));
               dc.LineTo(round(m_fromObject.getPoint().x / factor), round(m_fromObject.getPoint().y / factor));
            }
         }
      }
      /*else
      {
         if (m_toObject.isSet())
            m_camCadView.DrawCross(&dc, round(m_fromObject.getPoint().x / factor), round(m_fromObject.getPoint().y / factor), 1.0);
      }*/

      dc.MoveTo(round(m_fromObject.getPoint().x / factor), round(m_fromObject.getPoint().y / factor));
      if (m_toObject.isSet())
      {
         dc.LineTo(round(m_toObject.getPoint().x / factor), round(m_toObject.getPoint().y / factor));
            
         // if not an intersection
         /*if (fabs(getDeltaX()) > SMALLNUMBER || fabs(getDeltaY()) > SMALLNUMBER)
            m_camCadView.DrawCross(&dc, round(m_toObject.getPoint().x / factor), round(m_toObject.getPoint().y / factor), 1.0);*/
      }
   }
}

void CMeasure::drawMeasureGraphic(CClientDC& dc, double factor)
{
   //// diagonal
   //CPoint2d fromPoint = m_fromObject.getPoint();
   //CPoint2d toPoint = m_toObject.getPoint();

   //dc.MoveTo(round(fromPoint.x / factor), round(fromPoint.y / factor));
   //dc.LineTo(prevPnt);

   //// box
   //dc.LineTo(prevPnt.x, round(measure.FromY / factor));
   //dc.LineTo(round(measure.FromX / factor), round(measure.FromY / factor));
   //dc.LineTo(round(measure.FromX / factor), prevPnt.y);
   //dc.LineTo(prevPnt);
}

void CMeasure::updateMeasure()
{
   if (m_dialog != NULL)
   {
      m_dialog->updateMeasure();
   }
}

BOOL CMeasure::PreTranslateMessage(MSG* pMsg)
{
   if (m_dialog !=  NULL)
   {
      return m_dialog->PreTranslateMessage(pMsg);
   }
   else
   {
      return FALSE;
   }
}

//-----------------------------------------------------------------------------
// CMeasureBaseDialog dialog
//-----------------------------------------------------------------------------
CMeasureBaseDialog::CMeasureBaseDialog(int dialogId, CMeasure& measure)
   : CResizingDialog(dialogId, NULL)
   , m_measure(measure)
{
}

CMeasureBaseDialog::~CMeasureBaseDialog()
{
}
