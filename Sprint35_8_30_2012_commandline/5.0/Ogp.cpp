// $Header: /CAMCAD/5.0/Ogp.cpp 32    6/17/07 8:53p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2002. All Rights Reserved.
*/           

#include "stdafx.h"
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "crypt.h"
#include "OGP.h"
#include "attrib.h"
#include "mainfrm.h"
#include "polylib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CView *activeView; // from CCVIEW.CPP

unsigned short keywords[MAX_FEATURE_ATT+1];
void* values[MAX_FEATURE_ATT+1];

void Arrow_for_bulge(double *p1x,double *p1y,double bulge,double *p2x,double *p2y);
void DrawArrow(CDC *pDC, double x1, double y1, double x2, double y2, double scaleFactor, double arrowsize, COLORREF color);
void DrawDirectionArrow(CCEtoODBDoc *doc, CPoly* poly, BOOL Show, double insert_x, double insert_y, double scale, Mat2x2 *m, int mirror);

static char set[MAX_FEATURE_ATT+1]; // array of attributes to set
static RTN_struct defaults;
static BOOL Read_RTN_OUT();
static void Write_RTN_OUT();

/************************************************************************************
* OnOGP_ShowDirection 
*/
void CCEtoODBDoc::OnOGP_ShowDirection() 
{
   showDirection = !showDirection;
   UpdateAllViews(NULL);
}

/************************************************************************************
* OnOGP_ReverseDirection 
*/
void CCEtoODBDoc::OnOGP_ReverseDirection() 
{
   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = SelectList.GetNext(pos);
      if (s->getData()->getDataType() != T_POLY)
         continue;
      CPoly *poly = s->getData()->getPolyList()->GetHead();

      if (showDirection)
      {
         Mat2x2 m;
         RotMat2(&m, s->rotation);
         DrawDirectionArrow(this, poly, FALSE, s->insert_x, s->insert_y, s->scale, &m, s->mirror);
      }

      ReversePoly(&poly->getPntList());

      if (showDirection)
         DrawEntity(s, (s->getData()->isSelected()?3:2), FALSE);
   }
}

/********************************************************************
* DrawDirectionArrow
*/
void DrawDirectionArrow(CCEtoODBDoc *doc, CPoly* poly, BOOL Show, double insert_x, double insert_y, double scale, Mat2x2 *m, int mirror)
{
   CPnt *pnt;
   double bulge;
   Point2 p1, p2;

   POSITION pos = poly->getPntList().GetHeadPosition();
   if (pos == NULL)
      return;

   pnt = poly->getPntList().GetNext(pos);
   p1.x = pnt->x * scale;
   p1.y = pnt->y * scale;
   bulge = pnt->bulge;
   if (mirror & MIRROR_FLIP)
   {
      p1.x = -p1.x;
      bulge = -bulge;
   }
   TransPoint2(&p1, 1, m, insert_x, insert_y);

   pnt = poly->getPntList().GetNext(pos);
   p2.x = pnt->x * scale;
   p2.y = pnt->y * scale;
   if (mirror & MIRROR_FLIP)
      p2.x = -p2.x;
   TransPoint2(&p2, 1, m, insert_x, insert_y);

   COLORREF color;
   if (Show)
      color = RGB(255, 0, 0);
   else
      color = doc->getSettings().Foregrnd;

   CClientDC dc(activeView);
   activeView->OnPrepareDC(&dc);
   if (fabs(bulge) < SMALLNUMBER)
      DrawArrow(&dc, p1.x, p1.y, p2.x, p2.y, ((CCEtoODBView*)activeView)->getScaleFactor(), doc->getSettings().RL_arrowSize, color);
   else
   {
      double new1_x, new1_y, new2_x, new2_y;
      new1_x = p1.x; new1_y = p1.y;
      new2_x = p2.x; new2_y = p2.y;
      Arrow_for_bulge(&new1_x, &new1_y, bulge, &new2_x, &new2_y);
      DrawArrow(&dc, new1_x, new1_y, new2_x, new2_y, ((CCEtoODBView*)activeView)->getScaleFactor(), doc->getSettings().RL_arrowSize, color);
   }
}

/********************************************************************
* Arrow_for_bulge
*/
void Arrow_for_bulge(double *ax, double *ay, double bulge, double *bx, double *by)
{
   double mx, my;
   GetBulgeMidPoint(*ax, *ay, *bx, *by, bulge, &mx, &my);

   double midx, midy, offset;
   midx = (*ax + *bx) / 2;
   midy = (*ay + *by) / 2;
   offset = sqrt(pow(mx - midx,2) + pow(my - midy,2));
   
   double segLength = sqrt(pow(*ay - *by,2) + pow(*ax - *bx,2));

   if (fabs(*ay - *by) < SMALLNUMBER) // line has no slope
   {
      if (midy > my)
      {
         *ay -= offset;       
         *by -= offset;    
      }
      else
      {
         *ay += offset;       
         *by += offset;    
      }
   }
   else if (fabs(*ax - *bx) < SMALLNUMBER) // Undefined slope
   {  
      if (midx > mx)
      {
         *ax -= offset ;         
         *bx -= offset ;      
      }
      else
      {
         *ax += offset ;         
         *bx += offset ;      
      }
   }
   else if (((*bx > *ax) && (*by < *ay)) || ((*bx < *ax) && (*by > *ay))) // slope < 0
   {     
      // for speed reasons only calc when needed.
      double lxn = offset * fabs(*by - *ay) / segLength;
      double lyn = sqrt(pow(offset,2) - pow(lxn,2));
       if (midx > mx)
      {
         *ax -= lxn ;  *ay -= lyn;        
         *bx -= lxn ;  *by -= lyn;     
      }
      else
      {
         *ax += lxn ;  *ay += lyn;        
         *bx += lxn ;  *by += lyn;     
      }
   }
      
   else if (((*bx < *ax) && (*by < *ay)) || ((*bx > *ax) && (*by > *ay))) // slope > 0
   {     
      // for speed reasons only calc when needed.
      double lxn = offset * fabs(*by - *ay) / segLength;
      double lyn = sqrt(pow(offset,2) - pow(lxn,2));
      if (midx > mx)
      {
         *ax -= lxn ;   *ay += lyn;       
         *bx -= lxn ;   *by += lyn;    
      }
      else
      {
         *ax += lxn ;   *ay -= lyn;       
         *bx += lxn ;   *by -= lyn;    
      }
   }
}

/************************************************************************************
* InteractiveLights
*/
void CCEtoODBDoc::OnOGP_InteractiveLights() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write)) 
   {
      ErrorAccess("You do not have a License for RTN Write!");
      return;
   }*/

   OGP_Lights dlg;

   SelectStruct *s;
   // init dlg with selected item
   if (!SelectList.IsEmpty())
   {
      Attrib* attrib;
      s = SelectList.GetHead();
      int keyword;
   
      keyword = IsKeyWord(ATT_Light_Profile, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.profile = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Light_Surface, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.surface = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Light_Ring, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.ring = attrib->getIntValue();
      }
   }

   if (dlg.DoModal() != IDOK) return;

   CWaitCursor wait;

   keywords[Light_Profile] = RegisterKeyWord(ATT_Light_Profile, FALSE, VT_INTEGER);
   values[Light_Profile] = &dlg.profile;
   keywords[Light_Surface] = RegisterKeyWord(ATT_Light_Surface, FALSE, VT_INTEGER);
   values[Light_Surface] = &dlg.surface;
   keywords[Light_Ring] = RegisterKeyWord(ATT_Light_Ring, FALSE, VT_INTEGER);
   values[Light_Ring] = &dlg.ring;

   // loop selected items
   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      s = SelectList.GetNext(pos);

      // returns if existed
      for (int i = Light_Profile; i <= Light_Ring; i++)
         SetAttrib(&s->getData()->getAttributesRef(), keywords[i], VT_INTEGER, values[i], SA_OVERWRITE, NULL);
   }
}

/************************************************************************************
* InteractiveEdge
*/
void CCEtoODBDoc::OnOGP_InteractiveEdge() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write)) 
   {
      ErrorAccess("You do not have a License for RTN Write!");
      return;
   }*/

   OGP_Edge dlg;

   SelectStruct *s;
   if (!SelectList.IsEmpty())
   {
      // init dlg with selected item
      Attrib* attrib;
      s = SelectList.GetHead();
      int keyword;

      keyword = IsKeyWord(ATT_Edge_Bounds, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.bounds = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Edge_Extents, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.extents = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Edge_NearestNominal, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {     
         dlg.nearest = attrib->getIntValue();
      } 

      keyword = IsKeyWord(ATT_Edge_First, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.first = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Edge_Last, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.last = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Edge_FirstOf2, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.firstOf2 = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Edge_LastOf2, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.lastOf2 = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Edge_Contrast, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.contrast = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Edge_WeakStrong, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.weakStrong = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Edge_RoughSmooth, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.roughSmooth = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Edge_PercentFeature, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {     
         dlg.percentFeature = attrib->getIntValue();
      }  

      keyword = IsKeyWord(ATT_Edge_PercentCoverage, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.percentCoverage = attrib->getIntValue();
      }  
   }

   if (dlg.DoModal() != IDOK) return;
   
   CWaitCursor wait;

   keywords[Edge_Bounds] = RegisterKeyWord(ATT_Edge_Bounds, FALSE, VT_INTEGER);
   values[Edge_Bounds] = &dlg.bounds;
   keywords[Edge_Extents] = RegisterKeyWord(ATT_Edge_Extents, FALSE, VT_INTEGER);
   values[Edge_Extents] = &dlg.extents;
   keywords[Edge_NearestNominal] = RegisterKeyWord(ATT_Edge_NearestNominal, FALSE, VT_INTEGER);
   values[Edge_NearestNominal] = &dlg.nearest;
   keywords[Edge_First] = RegisterKeyWord(ATT_Edge_First, FALSE, VT_INTEGER);
   values[Edge_First] = &dlg.first;
   keywords[Edge_Last] = RegisterKeyWord(ATT_Edge_Last, FALSE, VT_INTEGER);
   values[Edge_Last] = &dlg.last;
   keywords[Edge_FirstOf2] = RegisterKeyWord(ATT_Edge_FirstOf2, FALSE, VT_INTEGER);
   values[Edge_FirstOf2] = &dlg.firstOf2;
   keywords[Edge_LastOf2] = RegisterKeyWord(ATT_Edge_LastOf2, FALSE, VT_INTEGER);
   values[Edge_LastOf2] = &dlg.lastOf2;
   keywords[Edge_Contrast] = RegisterKeyWord(ATT_Edge_Contrast, FALSE, VT_INTEGER);
   values[Edge_Contrast] = &dlg.contrast;
   keywords[Edge_WeakStrong] = RegisterKeyWord(ATT_Edge_WeakStrong, FALSE, VT_INTEGER);
   values[Edge_WeakStrong] = &dlg.weakStrong;
   keywords[Edge_RoughSmooth] = RegisterKeyWord(ATT_Edge_RoughSmooth, FALSE, VT_INTEGER);
   values[Edge_RoughSmooth] = &dlg.roughSmooth;
   keywords[Edge_PercentFeature] = RegisterKeyWord(ATT_Edge_PercentFeature, FALSE, VT_INTEGER);
   values[Edge_PercentFeature] = &dlg.percentFeature;
   keywords[Edge_PercentCoverage] = RegisterKeyWord(ATT_Edge_PercentCoverage, FALSE, VT_INTEGER);
   values[Edge_PercentCoverage] = &dlg.percentCoverage;

   // loop selected items
   POSITION selPos = SelectList.GetHeadPosition();
   while (selPos)
   {
      SelectStruct *s = SelectList.GetNext(selPos);

      for (int i = Edge_Bounds; i <= Edge_PercentCoverage; i++)
         SetAttrib(&s->getData()->getAttributesRef(), keywords[i], VT_INTEGER, values[i], SA_OVERWRITE, NULL);

      BOOL boolean = FALSE;
      SetAttrib(&s->getData()->getAttributesRef(), RegisterKeyWord(ATT_Centroid_IsCentroid, FALSE, VT_INTEGER), VT_INTEGER, &boolean, SA_OVERWRITE, NULL);
   }
}

/************************************************************************************
* InteractiveCentroid
*/
void CCEtoODBDoc::OnOGP_InteractiveCentroid() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write)) 
   {
      ErrorAccess("You do not have a License for RTN Write!");
      return;
   }*/

   BOOL WarnNonClosedPoly = FALSE;
   BOOL AreThereClosedPolys = FALSE;
   POSITION selPos = SelectList.GetHeadPosition();
   while (selPos)
   {
      POSITION tempPos = selPos;
      SelectStruct *s = SelectList.GetNext(selPos);

      if (s->getData()->getDataType() != T_POLY)
      {
         WarnNonClosedPoly = TRUE;
         s->getData()->setMarked(false);
         s->getData()->setSelected(false);
         DrawEntity(s, 0, FALSE);
         SelectList.RemoveAt(tempPos);
         delete s;
         continue;
      }

      CPoly *poly = s->getData()->getPolyList()->GetHead();

      if (poly->getPntList().GetCount() < 3 || !poly->isClosed())
      {
         WarnNonClosedPoly = TRUE;
         s->getData()->setMarked(false);
         s->getData()->setSelected(false);
         DrawEntity(s, 0, FALSE);
         SelectList.RemoveAt(tempPos);
         delete s;
         continue;
      }

      AreThereClosedPolys = TRUE;
   }

   if (WarnNonClosedPoly)
      AfxMessageBox("Warning : Only Closed Polys can be exported as Centroid.");

   if (!AreThereClosedPolys)
      return;

   OGP_Centroid dlg;

   if (!SelectList.IsEmpty())
   {
      // init dlg with selected item
      Attrib* attrib;
      SelectStruct *s = SelectList.GetHead();
      int keyword;

      keyword = IsKeyWord(ATT_Centroid_Threshold, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.threshold = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Centroid_MultipleValue, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.multiple = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Centroid_Filter, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.filter = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Centroid_MultipleOn, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.Multiple = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Centroid_Fill, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.Fill = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Centroid_TouchBoundary, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.TouchBoundary = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Centroid_Illumination, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.illumination = attrib->getIntValue();
      }
   }

   if (dlg.DoModal() != IDOK)
      return;
   
   CWaitCursor wait;

   keywords[Centroid_IsCentroid] = RegisterKeyWord(ATT_Centroid_IsCentroid, FALSE, VT_INTEGER);
   BOOL boolean = TRUE;
   values[Centroid_IsCentroid] = &boolean;
   keywords[Centroid_Threshold] = RegisterKeyWord(ATT_Centroid_Threshold, FALSE, VT_INTEGER);
   values[Centroid_Threshold] = &dlg.threshold;
   keywords[Centroid_MultipleValue] = RegisterKeyWord(ATT_Centroid_MultipleValue, FALSE, VT_INTEGER);
   values[Centroid_MultipleValue] = &dlg.multiple;
   keywords[Centroid_Filter] = RegisterKeyWord(ATT_Centroid_Filter, FALSE, VT_INTEGER);
   values[Centroid_Filter] = &dlg.filter;
   keywords[Centroid_MultipleOn] = RegisterKeyWord(ATT_Centroid_MultipleOn, FALSE, VT_INTEGER);
   values[Centroid_MultipleOn] = &dlg.Multiple;
   keywords[Centroid_Fill] = RegisterKeyWord(ATT_Centroid_Fill, FALSE, VT_INTEGER);
   values[Centroid_Fill] = &dlg.Fill;
   keywords[Centroid_TouchBoundary] = RegisterKeyWord(ATT_Centroid_TouchBoundary, FALSE, VT_INTEGER);
   values[Centroid_TouchBoundary] = &dlg.TouchBoundary;
   keywords[Centroid_Illumination] = RegisterKeyWord(ATT_Centroid_Illumination, FALSE, VT_INTEGER);
   values[Centroid_Illumination] = &dlg.illumination;

   // loop selected items
   selPos = SelectList.GetHeadPosition();
   while (selPos)
   {
      SelectStruct *s = SelectList.GetNext(selPos);

      for (int i = Centroid_IsCentroid; i <= Centroid_Illumination; i++)
         SetAttrib(&s->getData()->getAttributesRef(), keywords[i], VT_INTEGER, values[i], SA_OVERWRITE, NULL);

      BOOL boolean = TRUE;
      SetAttrib(&s->getData()->getAttributesRef(), RegisterKeyWord(ATT_Centroid_IsCentroid, FALSE, VT_INTEGER), VT_INTEGER, &boolean, SA_OVERWRITE, NULL);
   }
}

/************************************************************************************
* InteractiveTol
*/
void CCEtoODBDoc::OnOGP_InteractiveTol() 
{
    /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write)) 
   {
      ErrorAccess("You do not have a License for RTN Write!");
      return;
   }*/

   OGP_Tol dlg;

   SelectStruct *s;
   if (!SelectList.IsEmpty())
   {
      // init dlg with selected item
      Attrib* attrib;
      s = SelectList.GetHead();
      int keyword;
   
      keyword = IsKeyWord(ATT_Tol_ReportText, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (attrib->getStringValueIndex() != -1)
            dlg.report = attrib->getStringValue();
      }

      keyword = IsKeyWord(ATT_Tol_FeatureText, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (attrib->getStringValueIndex() != -1)
            dlg.feature = attrib->getStringValue();
      }

      keyword = IsKeyWord(ATT_Tol_fUpper, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.fUpper = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_fLower, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.fLower = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_fPrint, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.fPrint = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Tol_fStats, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.fStats = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Tol_xUpper, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.xUpper = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_xLower, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.xLower = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_xPrint, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.xPrint = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Tol_xStats, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.xStats = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Tol_yUpper, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.yUpper = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_yLower, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.yLower = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_yPrint, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.yPrint = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Tol_yStats, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.yStats = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Tol_zUpper, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.zUpper = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_zLower, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.zLower = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_zLoc, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.zLoc = attrib->getDoubleValue();
      }

      keyword = IsKeyWord(ATT_Tol_zPrint, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.zPrint = attrib->getIntValue();
      }

      keyword = IsKeyWord(ATT_Tol_zStats, FALSE);

      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         dlg.zStats = attrib->getIntValue();
      }
   }

   if (dlg.DoModal() != IDOK) return;

   CWaitCursor wait;

   keywords[Tol_ReportText] = RegisterKeyWord(ATT_Tol_ReportText, FALSE, VT_STRING);
   values[Tol_ReportText] = dlg.report.GetBuffer(0);
   keywords[Tol_FeatureText] = RegisterKeyWord(ATT_Tol_FeatureText, FALSE, VT_STRING);
   values[Tol_FeatureText] = dlg.feature.GetBuffer(0);
   keywords[Tol_fUpper] = RegisterKeyWord(ATT_Tol_fUpper, FALSE, VT_DOUBLE);
   values[Tol_fUpper] = &dlg.fUpper;
   keywords[Tol_fLower] = RegisterKeyWord(ATT_Tol_fLower, FALSE, VT_DOUBLE);
   values[Tol_fLower] = &dlg.fLower;
   keywords[Tol_fPrint] = RegisterKeyWord(ATT_Tol_fPrint, FALSE, VT_INTEGER);
   values[Tol_fPrint] = &dlg.fPrint;
   keywords[Tol_fStats] = RegisterKeyWord(ATT_Tol_fStats, FALSE, VT_INTEGER);
   values[Tol_fStats] = &dlg.fStats;
   keywords[Tol_xUpper] = RegisterKeyWord(ATT_Tol_xUpper, FALSE, VT_DOUBLE);
   values[Tol_xUpper] = &dlg.xUpper;
   keywords[Tol_xLower] = RegisterKeyWord(ATT_Tol_xLower, FALSE, VT_DOUBLE);
   values[Tol_xLower] = &dlg.xLower;
   keywords[Tol_xPrint] = RegisterKeyWord(ATT_Tol_xPrint, FALSE, VT_INTEGER);
   values[Tol_xPrint] = &dlg.xPrint;
   keywords[Tol_xStats] = RegisterKeyWord(ATT_Tol_xStats, FALSE, VT_INTEGER);
   values[Tol_xStats] = &dlg.xStats;
   keywords[Tol_yUpper] = RegisterKeyWord(ATT_Tol_yUpper, FALSE, VT_DOUBLE);
   values[Tol_yUpper] = &dlg.yUpper;
   keywords[Tol_yLower] = RegisterKeyWord(ATT_Tol_yLower, FALSE, VT_DOUBLE);
   values[Tol_yLower] = &dlg.yLower;
   keywords[Tol_yPrint] = RegisterKeyWord(ATT_Tol_yPrint, FALSE, VT_INTEGER);
   values[Tol_yPrint] = &dlg.yPrint;
   keywords[Tol_yStats] = RegisterKeyWord(ATT_Tol_yStats, FALSE, VT_INTEGER);
   values[Tol_yStats] = &dlg.yStats;
   keywords[Tol_zUpper] = RegisterKeyWord(ATT_Tol_zUpper, FALSE, VT_DOUBLE);
   values[Tol_zUpper] = &dlg.zUpper;
   keywords[Tol_zLower] = RegisterKeyWord(ATT_Tol_zLower, FALSE, VT_DOUBLE);
   values[Tol_zLower] = &dlg.zLower;
   keywords[Tol_zPrint] = RegisterKeyWord(ATT_Tol_zPrint, FALSE, VT_INTEGER);
   values[Tol_zPrint] = &dlg.zPrint;
   keywords[Tol_zStats] = RegisterKeyWord(ATT_Tol_zStats, FALSE, VT_INTEGER);
   values[Tol_zStats] = &dlg.zStats;
   keywords[Tol_zLoc] = RegisterKeyWord(ATT_Tol_zLoc, FALSE, VT_DOUBLE);
   values[Tol_zLoc] = &dlg.zLoc;

   // loop selected items
   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      s = SelectList.GetNext(pos);

      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_ReportText], VT_STRING, values[Tol_ReportText], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_FeatureText], VT_STRING, values[Tol_FeatureText], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_fUpper], VT_DOUBLE, values[Tol_fUpper], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_fLower], VT_DOUBLE, values[Tol_fLower], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_fPrint], VT_INTEGER, values[Tol_fPrint], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_fStats], VT_INTEGER, values[Tol_fStats], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_xUpper], VT_DOUBLE, values[Tol_xUpper], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_xLower], VT_DOUBLE, values[Tol_xLower], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_xPrint], VT_INTEGER, values[Tol_xPrint], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_xStats], VT_INTEGER, values[Tol_xStats], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_yUpper], VT_DOUBLE, values[Tol_yUpper], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_yLower], VT_DOUBLE, values[Tol_yLower], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_yPrint], VT_INTEGER, values[Tol_yPrint], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_yStats], VT_INTEGER, values[Tol_yStats], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_zUpper], VT_DOUBLE, values[Tol_zUpper], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_zLower], VT_DOUBLE, values[Tol_zLower], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_zPrint], VT_INTEGER, values[Tol_zPrint], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_zStats], VT_INTEGER, values[Tol_zStats], SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), keywords[Tol_zLoc], VT_DOUBLE, values[Tol_zLoc], SA_OVERWRITE, NULL);
   }
}

/************************************************************************************
* SetPointType
*/
void CCEtoODBDoc::OnOGP_SetPointType() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write)) 
   {
      ErrorAccess("You do not have a License for RTN Write!");
      return;
   }*/

   OGP_Point dlg;

   int keyword = RegisterKeyWord(ATT_PointType, FALSE, VT_STRING);
   CString value;

   SelectStruct *s;
   if (!SelectList.IsEmpty())
   {
      // init dlg with selected item
      Attrib* attrib;
      s = SelectList.GetHead();
   
      if (keyword != -1 && s->getData()->getAttributesRef() && s->getData()->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (attrib->getStringValueIndex() != -1)
            value = attrib->getStringValue();

         if (value == PT_STRONG_EDGE)
            dlg.m_type = 0;
         else if (value == PT_STRONG_EDGE_RIGHT)
            dlg.m_type = 1;
         else if (value == PT_STRONG_EDGE_LEFT)
            dlg.m_type = 2;
         else if (value == PT_STRONG_EDGE_UP)
            dlg.m_type = 3;
         else if (value == PT_STRONG_EDGE_DOWN)
            dlg.m_type = 4;
         else if (value == PT_STRONG_EDGE_LOCATE)
            dlg.m_type = 5;
         else if (value == PT_FOCUS_TOP_DOWN)
            dlg.m_type = 6;
         else if (value == PT_FOCUS_BOTTOM_UP)
            dlg.m_type = 7;
         else if (value == PT_FOCUS_M_C_FOCUS)
            dlg.m_type = 8;
         else if (value == PT_FOCUS_M_C_GRID)
            dlg.m_type = 9;
      }
   }

   if (dlg.DoModal() != IDOK)
      return;

   switch (dlg.m_type)
   {
   case 0:
      value = PT_STRONG_EDGE;
      break;
   case 1:
      value = PT_STRONG_EDGE_RIGHT;
      break;
   case 2:
      value = PT_STRONG_EDGE_LEFT;
      break;
   case 3:
      value = PT_STRONG_EDGE_UP;
      break;
   case 4:
      value = PT_STRONG_EDGE_DOWN;
      break;
   case 5:
      value = PT_STRONG_EDGE_LOCATE;
      break;
   case 6:
      value = PT_FOCUS_TOP_DOWN;
      break;
   case 7:
      value = PT_FOCUS_BOTTOM_UP;
      break;
   case 8:
      value = PT_FOCUS_M_C_FOCUS;
      break;
   case 9:
      value = PT_FOCUS_M_C_GRID;
      break;
   }

   // loop selected items
   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = SelectList.GetNext(pos);

      if (s->getData()->getDataType() == T_POINT)
         SetAttrib(&s->getData()->getAttributesRef(), keyword, VT_STRING, value.GetBuffer(0), SA_OVERWRITE, NULL);
   }
}

/************************************************************************************
* FieldOfView
*/

static double lowMag[3][10] = 
{0.3, 0.5, 0.6, 1.0, 1.3, 1.6, 3.2,  6.4, 15.9, 31.8,
 0.5, 0.7, 1.0, 1.4, 1.9, 2.4, 4.8,  9.5, 23.8, 47.5,
 1.0, 1.4, 1.9, 2.9, 3.8, 4.8, 9.5, 19.0, 47.5, 95.0};

static double highMag[3][10] = 
{1.7,  2.6,  3.5,  5.2,  7.0,  8.7, 17.4,  34.8,  87.1, 174.2,
 2.6,  3.9,  5.2,  7.8, 10.4, 13.0, 26.0,  52.0, 130.0, 260.0,
 5.2,  7.8, 10.4, 15.6, 20.8, 26.0, 52.0, 104.0, 260.0, 520.0};

static double lowSize[3][10] = 
{0.328,  0.29,    0.253731343,0.215,   0.17, 0.101492537,0.050746269,0.025373134,0.010149254,0.005074627,
 0.22,   0.195,   0.17,       0.145,   0.12, 0.068,      0.034,      0.017,      0.0068,     0.0034,
 0.11,   0.0975,  0.085,      0.07,    0.06, 0.034,      0.017,      0.0085,     0.0034,     0.0017};

static double highSize[3][10] = 
{0.052,  0.045,   0.037,   0.029,   0.02,    0.0148,  0.0074,  0.0037,  0.00148, 0.00074,
 0.035,  0.03,    0.025,   0.02,    0.015,   0.01,    0.005,   0.0025,  0.001,   0.0005,
 0.0175, 0.015,   0.0125,  0.01,    0.007,   0.005,   0.0025,  0.00125, 0.0005,  0.00025};

void CCEtoODBDoc::OnOGP_FieldOfView() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write)) 
   {
      ErrorAccess("You do not have a License for RTN Write!");
      return;
   }*/
   
/* if (SelectList.IsEmpty())
   {
      ErrorMessage("No entities are selected");
      return;
   }*/

   static OGP_FOV dlg;
   if (dlg.DoModal() != IDOK)
      return;

   int FOV_InchesKW = RegisterKeyWord(ATT_FOV_Inches, FALSE, VT_DOUBLE);
   int FOV_ZoomCountsKW = RegisterKeyWord(ATT_FOV_ZoomCounts, FALSE, VT_INTEGER);
   // loop selected items
   POSITION pos = SelectList.GetHeadPosition();
   while (pos != NULL)
   {
      SelectStruct *s = SelectList.GetNext(pos);

      SetAttrib(&s->getData()->getAttributesRef(), FOV_InchesKW, VT_DOUBLE, &dlg.inches, SA_OVERWRITE, NULL);
      SetAttrib(&s->getData()->getAttributesRef(), FOV_ZoomCountsKW, VT_INTEGER, &dlg.zoomCounts, SA_OVERWRITE, NULL);
   }
}

/////////////////////////////////////////////////////////////////////////////
// OGP_FOV dialog
OGP_FOV::OGP_FOV(CWnd* pParent /*=NULL*/)
   : CDialog(OGP_FOV::IDD, pParent)
{
   //{{AFX_DATA_INIT(OGP_FOV)
   m_fixed = FALSE;
   m_opticalMag = _T("");
   m_zoomCounts = 14500;
   m_frontLens = 2;
   m_extTube = 1;
   m_FOV_Size = _T("");
   m_zoomCount = _T("");
   //}}AFX_DATA_INIT
   scrollPos = 0;
}

void OGP_FOV::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OGP_FOV)
   DDX_Control(pDX, IDC_ZOOM_MAG, m_zoomMagSC);
   DDX_Check(pDX, IDC_FIXED, m_fixed);
   DDX_Text(pDX, IDC_OPTICAL_MAG, m_opticalMag);
   DDX_Text(pDX, IDC_ZOOM_COUNTS, m_zoomCounts);
   DDV_MinMaxInt(pDX, m_zoomCounts, 0, 32000);
   DDX_CBIndex(pDX, IDC_FRONT_LENS, m_frontLens);
   DDX_CBIndex(pDX, IDC_EXT_TUBE, m_extTube);
   DDX_Text(pDX, IDC_FOV_SIZE, m_FOV_Size);
   DDX_Text(pDX, IDC_ZOOM_COUNT, m_zoomCount);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OGP_FOV, CDialog)
   //{{AFX_MSG_MAP(OGP_FOV)
   ON_CBN_SELCHANGE(IDC_EXT_TUBE, Calculate)
   ON_WM_HSCROLL()
   ON_CBN_SELCHANGE(IDC_FRONT_LENS, Calculate)
   ON_BN_CLICKED(IDC_FIXED, Calculate)
   ON_EN_CHANGE(IDC_ZOOM_COUNTS, Calculate)
   ON_BN_CLICKED(IDC_SAVE_DEFAULTS, OnSaveDefaults)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL OGP_FOV::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_zoomMagSC.SetScrollRange(0, 24, FALSE);
   m_zoomMagSC.SetScrollPos(scrollPos);

   Calculate();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void OGP_FOV::Calculate() 
{
   UpdateData();

   double step = (highMag[m_extTube][m_frontLens] - lowMag[m_extTube][m_frontLens]) / 24;
   opticalMag = lowMag[m_extTube][m_frontLens] + step * m_zoomMagSC.GetScrollPos();
   m_opticalMag.Format("%.1lf", opticalMag);
   
   step = (lowSize[m_extTube][m_frontLens] - highSize[m_extTube][m_frontLens]) / 24;
   inches = lowSize[m_extTube][m_frontLens] - step * m_zoomMagSC.GetScrollPos();
   m_FOV_Size.Format("FOV inches = %lg", inches);

   // 1000 .. 24 steps .. m_zoomCounts - 1000
   zoomCounts = 1000 + m_zoomMagSC.GetScrollPos() * (m_zoomCounts - 2000) / 24;
   m_zoomCount.Format("Zoom Counts = %d", zoomCounts);

   UpdateData(FALSE);
}

void OGP_FOV::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int i;
   switch (nSBCode)
   {
   case SB_LEFT:
      i=0;
      break;
   case SB_RIGHT:
      i=0;
      break;
   case SB_LINELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-1);
      break;
   case SB_LINERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+1);
      break;
   case SB_PAGELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-5);
      break;
   case SB_PAGERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+5);
      break;
   case SB_THUMBPOSITION:
   case SB_THUMBTRACK:
      pScrollBar->SetScrollPos(nPos);
      break;
   case SB_ENDSCROLL:
      break;
   };

   Calculate();
   
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void OGP_FOV::OnOK() 
{
   scrollPos = m_zoomMagSC.GetScrollPos();
   Calculate();
   
   CDialog::OnOK();
}

void OGP_FOV::OnSaveDefaults() 
{
   Read_RTN_OUT();

   scrollPos = m_zoomMagSC.GetScrollPos();
   Calculate();

   set[FOV_Inches] = TRUE;
   set[FOV_ZoomCounts] = TRUE;

   defaults.zoomCounts = zoomCounts;
   defaults.FOVinches = inches;

   Write_RTN_OUT();
}

/////////////////////////////////////////////////////////////////////////////
// OGP_Lights dialog
OGP_Lights::OGP_Lights(CWnd* pParent /*=NULL*/)
   : CDialog(OGP_Lights::IDD, pParent)
{
   //{{AFX_DATA_INIT(OGP_Lights)
   m_ring = _T("");
   m_surface = _T("");
   m_profile = _T("");
   //}}AFX_DATA_INIT
   ring = surface = profile = 0;
}

void OGP_Lights::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OGP_Lights)
   DDX_Control(pDX, IDC_SC_SURFACE, m_surfaceSC);
   DDX_Control(pDX, IDC_SC_RING, m_ringSC);
   DDX_Control(pDX, IDC_SC_PROFILE, m_profileSC);
   DDX_Text(pDX, IDC_RING, m_ring);
   DDX_Text(pDX, IDC_SURFACE, m_surface);
   DDX_Text(pDX, IDC_PROFILE, m_profile);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OGP_Lights, CDialog)
   //{{AFX_MSG_MAP(OGP_Lights)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_SAVE_DEFAULTS, OnSaveDefaults)
   ON_BN_CLICKED(IDC_LOAD_DEFAULTS, OnLoadDefaults)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL OGP_Lights::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_profileSC.SetScrollRange(0, 255, FALSE);
   m_surfaceSC.SetScrollRange(0, 255, FALSE);
   m_ringSC.SetScrollRange(0, 255, FALSE);
   
   PutData();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void OGP_Lights::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int i;
   switch (nSBCode)
   {
   case SB_LEFT:
      i=0;
      break;
   case SB_RIGHT:
      i=0;
      break;
   case SB_LINELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-1);
      break;
   case SB_LINERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+1);
      break;
   case SB_PAGELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-10);
      break;
   case SB_PAGERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+10);
      break;
   case SB_THUMBPOSITION:
   case SB_THUMBTRACK:
      pScrollBar->SetScrollPos(nPos);
      break;
   case SB_ENDSCROLL:
      break;
   };

   m_profile.Format("%d", m_profileSC.GetScrollPos());
   m_surface.Format("%d", m_surfaceSC.GetScrollPos());
   m_ring.Format("%d", m_ringSC.GetScrollPos());
   UpdateData(FALSE);

   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void OGP_Lights::OnOK() 
{
   GetData();
   
   CDialog::OnOK();
}

void OGP_Lights::GetData() 
{
   UpdateData();

   profile = m_profileSC.GetScrollPos();
   surface = m_surfaceSC.GetScrollPos();
   ring = m_ringSC.GetScrollPos();
}

void OGP_Lights::OnSaveDefaults() 
{
   Read_RTN_OUT();

   GetData();

   set[Light_Profile] = TRUE;
   set[Light_Surface] = TRUE;
   set[Light_Ring] = TRUE;

   defaults.profile = profile;
   defaults.ring = ring;
   defaults.surface = surface;

   Write_RTN_OUT();
}

void OGP_Lights::PutData() 
{
   m_profileSC.SetScrollPos(profile);
   m_surfaceSC.SetScrollPos(surface);
   m_ringSC.SetScrollPos(ring);
   
   m_profile.Format("%d", m_profileSC.GetScrollPos());
   m_surface.Format("%d", m_surfaceSC.GetScrollPos());
   m_ring.Format("%d", m_ringSC.GetScrollPos());
   
   UpdateData(FALSE);
}

void OGP_Lights::OnLoadDefaults() 
{
   Read_RTN_OUT();

   if (set[Light_Profile])
      profile = defaults.profile;

   if (set[Light_Surface])
      surface = defaults.surface;

   if (set[Light_Ring])
      ring = defaults.ring;

   PutData();
}

/////////////////////////////////////////////////////////////////////////////
// OGP_Edge dialog
OGP_Edge::OGP_Edge(CWnd* pParent /*=NULL*/)
   : CDialog(OGP_Edge::IDD, pParent)
{
   //{{AFX_DATA_INIT(OGP_Edge)
   m_bounds = _T("");
   m_contrast = _T("");
   m_extents = _T("");
   m_first = _T("");
   m_firstOf2 = _T("");
   m_last = _T("");
   m_lastOf2 = _T("");
   m_nearest = _T("");
   m_roughSmooth = _T("");
   m_weakStrong = _T("");
   m_percentFeature = _T("");
   m_percentCoverage = _T("");
   //}}AFX_DATA_INIT
   bounds = contrast = extents = first = firstOf2 = last = lastOf2 = nearest = roughSmooth = percentFeature = weakStrong = 0;
}

void OGP_Edge::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OGP_Edge)
   DDX_Control(pDX, IDC_SC_PERCENT_COVERAGE, m_percentCoverageSC);
   DDX_Control(pDX, IDC_SC_PERCENT_FEATURE, m_percentFeatureSC);
   DDX_Control(pDX, IDC_SC_CONTRAST, m_contrastSC);
   DDX_Control(pDX, IDC_SC_WEAK_STRONG, m_weakStrongSC);
   DDX_Control(pDX, IDC_SC_ROUGH_SMOOTH, m_roughSmoothSC);
   DDX_Control(pDX, IDC_SC_NEAREST_NOMINAL, m_nearestSC);
   DDX_Control(pDX, IDC_SC_LAST, m_lastSC);
   DDX_Control(pDX, IDC_SC_LAST_OF_2, m_lastOf2SC);
   DDX_Control(pDX, IDC_SC_FIRST_OF_2, m_firstOf2SC);
   DDX_Control(pDX, IDC_SC_FIRST, m_firstSC);
   DDX_Control(pDX, IDC_SC_EXTENTS, m_extentsSC);
   DDX_Control(pDX, IDC_SC_BOUNDS, m_boundsSC);
   DDX_Text(pDX, IDC_BOUNDS, m_bounds);
   DDX_Text(pDX, IDC_CONTRAST, m_contrast);
   DDX_Text(pDX, IDC_EXTENTS, m_extents);
   DDX_Text(pDX, IDC_FIRST, m_first);
   DDX_Text(pDX, IDC_FIRST_OF_2, m_firstOf2);
   DDX_Text(pDX, IDC_LAST, m_last);
   DDX_Text(pDX, IDC_LAST_OF_2, m_lastOf2);
   DDX_Text(pDX, IDC_NEAREST_NOMIMAL, m_nearest);
   DDX_Text(pDX, IDC_ROUGH_SMOOTH, m_roughSmooth);
   DDX_Text(pDX, IDC_WEAK_STRONG, m_weakStrong);
   DDX_Text(pDX, IDC_PERCENT_FEATURE, m_percentFeature);
   DDX_Text(pDX, IDC_PERCENT_COVERAGE, m_percentCoverage);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OGP_Edge, CDialog)
   //{{AFX_MSG_MAP(OGP_Edge)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_SAVE_DEFAULTS, OnSaveDefaults)
   ON_BN_CLICKED(IDC_LOAD_DEFAULTS, OnLoadDefaults)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL OGP_Edge::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_boundsSC.SetScrollRange(6, 60, FALSE);
   m_extentsSC.SetScrollRange(4, 100, FALSE);
   m_nearestSC.SetScrollRange(0, 10, FALSE);
   m_firstSC.SetScrollRange(0, 10, FALSE);
   m_lastSC.SetScrollRange(0, 10, FALSE);
   m_firstOf2SC.SetScrollRange(0, 10, FALSE);
   m_lastOf2SC.SetScrollRange(0, 10, FALSE);
   m_contrastSC.SetScrollRange(0, 10, FALSE);
   m_weakStrongSC.SetScrollRange(4, 100, FALSE);
   m_roughSmoothSC.SetScrollRange(1, 100, FALSE);
   m_percentFeatureSC.SetScrollRange(1, 100, FALSE);
   m_percentCoverageSC.SetScrollRange(0, 100, FALSE);

   PutData();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void OGP_Edge::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int i;
   switch (nSBCode)
   {
   case SB_LEFT:
      i=0;
      break;
   case SB_RIGHT:
      i=0;
      break;
   case SB_LINELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-1);
      break;
   case SB_LINERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+1);
      break;
   case SB_PAGELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-10);
      break;
   case SB_PAGERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+10);
      break;
   case SB_THUMBPOSITION:
   case SB_THUMBTRACK:
      pScrollBar->SetScrollPos(nPos);
      break;
   case SB_ENDSCROLL:
      break;
   };

   m_bounds.Format("%d", m_boundsSC.GetScrollPos());
   m_extents.Format("%d", 104 - m_extentsSC.GetScrollPos());
   m_nearest.Format("%d", m_nearestSC.GetScrollPos());
   m_first.Format("%d", m_firstSC.GetScrollPos());
   m_last.Format("%d", m_lastSC.GetScrollPos());
   m_firstOf2.Format("%d", m_firstOf2SC.GetScrollPos());
   m_lastOf2.Format("%d", m_lastOf2SC.GetScrollPos());
   m_contrast.Format("%d", m_contrastSC.GetScrollPos());
   m_weakStrong.Format("%d", m_weakStrongSC.GetScrollPos());
   m_roughSmooth.Format("%d", 101 - m_roughSmoothSC.GetScrollPos());
   m_percentFeature.Format("%d", m_percentFeatureSC.GetScrollPos());
   m_percentCoverage.Format("%d", m_percentCoverageSC.GetScrollPos());
   UpdateData(FALSE);

   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void OGP_Edge::GetData() 
{
   UpdateData();

   bounds = m_boundsSC.GetScrollPos();
   extents = 104 - m_extentsSC.GetScrollPos();
   nearest = m_nearestSC.GetScrollPos();
   first = m_firstSC.GetScrollPos();
   last = m_lastSC.GetScrollPos();
   firstOf2 = m_firstOf2SC.GetScrollPos();
   lastOf2 = m_lastOf2SC.GetScrollPos();
   contrast = m_contrastSC.GetScrollPos();
   weakStrong = m_weakStrongSC.GetScrollPos();
   roughSmooth = 101 - m_roughSmoothSC.GetScrollPos();
   percentFeature = m_percentFeatureSC.GetScrollPos();
   percentCoverage = m_percentCoverageSC.GetScrollPos();
}

void OGP_Edge::PutData() 
{
   m_boundsSC.SetScrollPos(bounds);
   m_extentsSC.SetScrollPos(104 - extents);
   m_nearestSC.SetScrollPos(nearest);
   m_firstSC.SetScrollPos(first);
   m_lastSC.SetScrollPos(last);
   m_firstOf2SC.SetScrollPos(firstOf2);
   m_lastOf2SC.SetScrollPos(lastOf2);
   m_contrastSC.SetScrollPos(contrast);
   m_weakStrongSC.SetScrollPos(weakStrong);
   m_roughSmoothSC.SetScrollPos(101 - roughSmooth);
   m_percentFeatureSC.SetScrollPos(percentFeature);
   m_percentCoverageSC.SetScrollPos(percentCoverage);
   
   m_bounds.Format("%d", m_boundsSC.GetScrollPos());
   m_extents.Format("%d", 104 - m_extentsSC.GetScrollPos());
   m_nearest.Format("%d", m_nearestSC.GetScrollPos());
   m_first.Format("%d", m_firstSC.GetScrollPos());
   m_last.Format("%d", m_lastSC.GetScrollPos());
   m_firstOf2.Format("%d", m_firstOf2SC.GetScrollPos());
   m_lastOf2.Format("%d", m_lastOf2SC.GetScrollPos());
   m_contrast.Format("%d", m_contrastSC.GetScrollPos());
   m_weakStrong.Format("%d", m_weakStrongSC.GetScrollPos());
   m_roughSmooth.Format("%d", 101 - m_roughSmoothSC.GetScrollPos());
   m_percentFeature.Format("%d", m_percentFeatureSC.GetScrollPos());
   m_percentCoverage.Format("%d", m_percentCoverageSC.GetScrollPos());

   UpdateData(FALSE);
}

void OGP_Edge::OnOK() 
{
   GetData();
   
   CDialog::OnOK();
}

void OGP_Edge::OnSaveDefaults() 
{
   Read_RTN_OUT();

   GetData();

   set[Edge_Bounds] = TRUE;
   set[Edge_Extents] = TRUE;
   set[Edge_NearestNominal] = TRUE;
   set[Edge_First] = TRUE;
   set[Edge_Last] = TRUE;
   set[Edge_FirstOf2] = TRUE;
   set[Edge_LastOf2] = TRUE;
   set[Edge_Contrast] = TRUE;
   set[Edge_WeakStrong] = TRUE;
   set[Edge_RoughSmooth] = TRUE;
   set[Edge_PercentFeature] = TRUE;
   set[Edge_PercentCoverage] = TRUE;

   defaults.bounds = bounds;
   defaults.extents = extents;
   defaults.nearest = nearest;
   defaults.first = first;
   defaults.last = last;
   defaults.firstOf2 = firstOf2;
   defaults.lastOf2 = lastOf2;
   defaults.contrast = contrast;
   defaults.weakStrong = weakStrong;
   defaults.roughSmooth = roughSmooth;
   defaults.percentFeature = percentFeature;
   defaults.percentCoverage = percentCoverage;

   Write_RTN_OUT();
}

void OGP_Edge::OnLoadDefaults() 
{
   Read_RTN_OUT();

   if (set[Edge_Bounds])
      bounds = defaults.bounds;

   if (set[Edge_Extents])
      extents = defaults.extents;

   if (set[Edge_NearestNominal])
      nearest = defaults.nearest;

   if (set[Edge_First])
      first = defaults.first;

   if (set[Edge_Last])
      last = defaults.last;

   if (set[Edge_FirstOf2])
      firstOf2 = defaults.firstOf2;

   if (set[Edge_LastOf2])
      lastOf2 = defaults.lastOf2;

   if (set[Edge_Contrast])
      contrast = defaults.contrast;

   if (set[Edge_WeakStrong])
      weakStrong = defaults.weakStrong;

   if (set[Edge_RoughSmooth])
      roughSmooth = defaults.roughSmooth;

   if (set[Edge_PercentFeature])
      percentFeature = defaults.percentFeature;

   if (set[Edge_PercentCoverage])
      percentCoverage = defaults.percentCoverage;

   PutData();
}

/////////////////////////////////////////////////////////////////////////////
// OGP_Tol dialog
OGP_Tol::OGP_Tol(CWnd* pParent /*=NULL*/)
   : CDialog(OGP_Tol::IDD, pParent)
{
   //{{AFX_DATA_INIT(OGP_Tol)
   m_fLower = _T("0.0");
   m_fPrint = FALSE;
   m_fStats = FALSE;
   m_fUpper = _T("0.0");
   m_feature = _T("");
   m_report = _T("");
   m_xLower = _T("0.0");
   m_xPrint = FALSE;
   m_xStats = FALSE;
   m_xUpper = _T("0.0");
   m_yLower = _T("0.0");
   m_yPrint = FALSE;
   m_yStats = FALSE;
   m_yUpper = _T("0.0");
   m_zLoc = _T("0.0");
   m_zLower = _T("0.0");
   m_zPrint = FALSE;
   m_zStats = FALSE;
   m_zUpper = _T("0.0");
   //}}AFX_DATA_INIT
   report = feature = "";
   fLower = fUpper = xLower = xUpper = yLower = yUpper = zLower = zUpper = zLoc = 0.0;
   fPrint = fStats = xPrint = xStats = yPrint = yStats = zPrint = zStats = FALSE;
}

void OGP_Tol::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OGP_Tol)
   DDX_Text(pDX, IDC_F_LOWER, m_fLower);
   DDX_Check(pDX, IDC_F_PRINT, m_fPrint);
   DDX_Check(pDX, IDC_F_STATS, m_fStats);
   DDX_Text(pDX, IDC_F_UPPER, m_fUpper);
   DDX_Text(pDX, IDC_FEATURE, m_feature);
   DDV_MaxChars(pDX, m_feature, 50);
   DDX_Text(pDX, IDC_REPORT, m_report);
   DDV_MaxChars(pDX, m_report, 50);
   DDX_Text(pDX, IDC_X_LOWER, m_xLower);
   DDX_Check(pDX, IDC_X_PRINT, m_xPrint);
   DDX_Check(pDX, IDC_X_STATS, m_xStats);
   DDX_Text(pDX, IDC_X_UPPER, m_xUpper);
   DDX_Text(pDX, IDC_Y_LOWER, m_yLower);
   DDX_Check(pDX, IDC_Y_PRINT, m_yPrint);
   DDX_Check(pDX, IDC_Y_STATS, m_yStats);
   DDX_Text(pDX, IDC_Y_UPPER, m_yUpper);
   DDX_Text(pDX, IDC_Z_LOC, m_zLoc);
   DDX_Text(pDX, IDC_Z_LOWER, m_zLower);
   DDX_Check(pDX, IDC_Z_PRINT, m_zPrint);
   DDX_Check(pDX, IDC_Z_STATS, m_zStats);
   DDX_Text(pDX, IDC_Z_UPPER, m_zUpper);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OGP_Tol, CDialog)
   //{{AFX_MSG_MAP(OGP_Tol)
   ON_BN_CLICKED(IDC_SAVE_DEFAULTS, OnSaveDefaults)
   ON_BN_CLICKED(IDC_LOAD_DEFAULTS, OnLoadDefaults)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL OGP_Tol::OnInitDialog() 
{
   CDialog::OnInitDialog();

   PutData();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void OGP_Tol::GetData() 
{
   UpdateData();

   report = m_report;
   feature = m_feature;
   fUpper = atof(m_fUpper);
   fLower = atof(m_fLower);
   fPrint = m_fPrint;
   fStats = m_fStats;
   xUpper = atof(m_xUpper);
   xLower = atof(m_xLower);
   xPrint = m_xPrint;
   xStats = m_xStats;
   yUpper = atof(m_yUpper);
   yLower = atof(m_yLower);
   yPrint = m_yPrint;
   yStats = m_yStats;
   zUpper = atof(m_zUpper);
   zLower = atof(m_zLower);
   zPrint = m_zPrint;
   zStats = m_zStats;
   zLoc = atof(m_zLoc);
}

void OGP_Tol::PutData() 
{
   m_report = report;
   m_feature = feature;
   m_fUpper.Format("%lg", fUpper);
   m_fLower.Format("%lg", fLower);
   m_fPrint = fPrint;
   m_fStats = fStats;
   m_xUpper.Format("%lg", xUpper);
   m_xLower.Format("%lg", xLower);
   m_xPrint = xPrint;
   m_xStats = xStats;
   m_yUpper.Format("%lg", yUpper);
   m_yLower.Format("%lg", yLower);
   m_yPrint = yPrint;
   m_yStats = yStats;
   m_zUpper.Format("%lg", zUpper);
   m_zLower.Format("%lg", zLower);
   m_zPrint = zPrint;
   m_zStats = zStats;
   m_zLoc.Format("%lg", zLoc);

   UpdateData(FALSE);
}

void OGP_Tol::OnOK() 
{
   GetData();
   
   CDialog::OnOK();
}

void OGP_Tol::OnSaveDefaults() 
{
   Read_RTN_OUT();

   GetData();

   set[Tol_ReportText] = TRUE;
   set[Tol_FeatureText] = TRUE;
   set[Tol_fUpper] = TRUE;
   set[Tol_fLower] = TRUE;
   set[Tol_xUpper] = TRUE;
   set[Tol_xLower] = TRUE;
   set[Tol_yUpper] = TRUE;
   set[Tol_yLower] = TRUE;
   set[Tol_zUpper] = TRUE;
   set[Tol_zLower] = TRUE;
   set[Tol_zLoc] = TRUE;
   set[Tol_fPrint] = TRUE;
   set[Tol_fStats] = TRUE;
   set[Tol_xPrint] = TRUE;
   set[Tol_xStats] = TRUE;
   set[Tol_yPrint] = TRUE;
   set[Tol_yStats] = TRUE;
   set[Tol_zPrint] = TRUE;
   set[Tol_zStats] = TRUE;

   strncpy(defaults.report, report, 50);
   defaults.report[50] = 0;
   strncpy(defaults.feature, feature, 50);
   defaults.feature[50] = 0;
   defaults.fUpper = fUpper;
   defaults.fLower = fLower;
   defaults.fPrint = fPrint;
   defaults.fStats = fStats;
   defaults.xUpper = xUpper;
   defaults.xLower = xLower;
   defaults.xPrint = xPrint;
   defaults.xStats = xStats;
   defaults.yUpper = yUpper;
   defaults.yLower = yLower;
   defaults.yPrint = yPrint;
   defaults.yStats = yStats;
   defaults.zUpper = zUpper;
   defaults.zLower = zLower;
   defaults.zPrint = zPrint;
   defaults.zStats = zStats;
   defaults.zLoc = zLoc;

   Write_RTN_OUT();
}

void OGP_Tol::OnLoadDefaults() 
{
   Read_RTN_OUT();

   if (set[Tol_ReportText])
      report = defaults.report;

   if (set[Tol_FeatureText])
      feature = defaults.feature;

   if (set[Tol_fUpper])
      fUpper = defaults.fUpper;

   if (set[Tol_fLower])
      fLower = defaults.fLower;

   if (set[Tol_xUpper])
      xUpper = defaults.xUpper;

   if (set[Tol_xLower])
      xLower = defaults.xLower;

   if (set[Tol_yUpper])
      yUpper = defaults.yUpper;

   if (set[Tol_yLower])
      yLower = defaults.yLower;

   if (set[Tol_zUpper])
      zUpper = defaults.zUpper;

   if (set[Tol_zLower])
      zLower = defaults.zLower;

   if (set[Tol_zLoc])
      zLoc = defaults.zLoc;

   if (set[Tol_fPrint])
      fPrint = defaults.fPrint;

   if (set[Tol_fStats])
      fStats = defaults.fStats;

   if (set[Tol_xPrint])
      xPrint = defaults.xPrint;

   if (set[Tol_xStats])
      xStats = defaults.xStats;

   if (set[Tol_yStats])
      yStats = defaults.yStats;

   if (set[Tol_yPrint])
      yPrint = defaults.yPrint;

   if (set[Tol_zPrint])
      zPrint = defaults.zPrint;

   if (set[Tol_zStats])
      zStats = defaults.zStats;

   PutData();
}

/////////////////////////////////////////////////////////////////////////////
// OGP_Point dialog
OGP_Point::OGP_Point(CWnd* pParent /*=NULL*/)
   : CDialog(OGP_Point::IDD, pParent)
{
   //{{AFX_DATA_INIT(OGP_Point)
   m_type = 0;
   //}}AFX_DATA_INIT
}

void OGP_Point::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OGP_Point)
   DDX_Radio(pDX, IDC_RADIO1, m_type);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OGP_Point, CDialog)
   //{{AFX_MSG_MAP(OGP_Point)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// OGP_Centroid dialog
OGP_Centroid::OGP_Centroid(CWnd* pParent /*=NULL*/)
   : CDialog(OGP_Centroid::IDD, pParent)
{
   //{{AFX_DATA_INIT(OGP_Centroid)
   m_fill = FALSE;
   m_filter = _T("");
   m_illumination = -1;
   m_multiple = _T("");
   m_multipleCheck = FALSE;
   m_threshold = _T("");
   m_touchBoundary = FALSE;
   //}}AFX_DATA_INIT
   threshold = multiple = filter = illumination = 0;
   Multiple = Fill = TouchBoundary = FALSE;
}

void OGP_Centroid::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OGP_Centroid)
   DDX_Control(pDX, IDC_SC_THRESHOLD, m_thresholdSC);
   DDX_Control(pDX, IDC_SC_MULTIPLE, m_multipleSC);
   DDX_Control(pDX, IDC_SC_FILTER, m_filterSC);
   DDX_Check(pDX, IDC_FILL, m_fill);
   DDX_Text(pDX, IDC_FILTER, m_filter);
   DDX_Radio(pDX, IDC_ILLUMINATION, m_illumination);
   DDX_Text(pDX, IDC_MULTIPLE, m_multiple);
   DDX_Check(pDX, IDC_MULTPLE_CHECK, m_multipleCheck);
   DDX_Text(pDX, IDC_THRESHOLD, m_threshold);
   DDX_Check(pDX, IDC_TOUCH_BOUNDARY, m_touchBoundary);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OGP_Centroid, CDialog)
   //{{AFX_MSG_MAP(OGP_Centroid)
   ON_WM_HSCROLL()
   ON_BN_CLICKED(IDC_SAVE_DEFAULTS, OnSaveDefaults)
   ON_BN_CLICKED(IDC_LOAD_DEFAULTS, OnLoadDefaults)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL OGP_Centroid::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_thresholdSC.SetScrollRange(0, 255, FALSE);
   m_filterSC.SetScrollRange(0, 10, FALSE);
   m_multipleSC.SetScrollRange(0, 100, FALSE);

   PutData();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void OGP_Centroid::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int i;
   switch (nSBCode)
   {
   case SB_LEFT:
      i=0;
      break;
   case SB_RIGHT:
      i=0;
      break;
   case SB_LINELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-1);
      break;
   case SB_LINERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+1);
      break;
   case SB_PAGELEFT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-10);
      break;
   case SB_PAGERIGHT:
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+10);
      break;
   case SB_THUMBPOSITION:
   case SB_THUMBTRACK:
      pScrollBar->SetScrollPos(nPos);
      break;
   case SB_ENDSCROLL:
      break;
   };

   m_threshold.Format("%d", m_thresholdSC.GetScrollPos());
   m_filter.Format("%d", m_filterSC.GetScrollPos());
   m_multiple.Format("%d", m_multipleSC.GetScrollPos());
   UpdateData(FALSE);

   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void OGP_Centroid::GetData() 
{
   UpdateData();

   threshold = m_thresholdSC.GetScrollPos();
   multiple = m_multipleSC.GetScrollPos();
   filter = m_filterSC.GetScrollPos();

   Multiple = m_multipleCheck;
   Fill = m_fill;
   TouchBoundary = m_touchBoundary;

   switch (m_illumination) // 0=Auto(-1), 1=Light(1), 2=Dark(0)
   {
   case 0:
      illumination = -1;
      break;
   case 1:
      illumination = 1;
      break;
   case 2:
      illumination = 0;
      break;
   }
}

void OGP_Centroid::PutData() 
{
   m_thresholdSC.SetScrollPos(threshold);
   m_multipleSC.SetScrollPos(multiple);
   m_filterSC.SetScrollPos(filter);
   
   m_threshold.Format("%d", m_thresholdSC.GetScrollPos());
   m_filter.Format("%d", m_filterSC.GetScrollPos());
   m_multiple.Format("%d", m_multipleSC.GetScrollPos());

   m_multipleCheck = Multiple;
   m_fill = Fill;
   m_touchBoundary = TouchBoundary;

   switch (illumination) // 0=Auto(-1), 1=Light(1), 2=Dark(0)
   {
   case -1:
      m_illumination = 0;
      break;
   case 0:
      m_illumination = 2;
      break;
   case 1:
      m_illumination = 1;
      break;
   }

   UpdateData(FALSE);
}

void OGP_Centroid::OnSaveDefaults() 
{
   Read_RTN_OUT();

   GetData();

   set[Centroid_Threshold] = TRUE;
   set[Centroid_MultipleValue] = TRUE;
   set[Centroid_Filter] = TRUE;
   set[Centroid_MultipleOn] = TRUE;
   set[Centroid_Fill] = TRUE;
   set[Centroid_TouchBoundary] = TRUE;
   set[Centroid_Illumination] = TRUE;

   defaults.threshold = threshold;
   defaults.multiple = multiple;
   defaults.filter = filter;
   defaults.Multiple = Multiple;
   defaults.Fill = Fill;
   defaults.TouchBoundary = TouchBoundary;
   defaults.illumination = illumination;

   Write_RTN_OUT();
}

void OGP_Centroid::OnLoadDefaults() 
{
   Read_RTN_OUT();

   if (set[Centroid_Threshold])
      threshold = defaults.threshold;

   if (set[Centroid_MultipleValue])
      multiple = defaults.multiple;

   if (set[Centroid_Filter])
      filter = defaults.filter;

   if (set[Centroid_MultipleOn])
      Multiple = defaults.Multiple;

   if (set[Centroid_Fill])
      Fill = defaults.Fill;

   if (set[Centroid_TouchBoundary])
      TouchBoundary = defaults.TouchBoundary;

   if (set[Centroid_Illumination])
      illumination = defaults.illumination;

   PutData();
}

void OGP_Centroid::OnOK() 
{
   GetData();

   CDialog::OnOK();
}


/************************************************************************************
* OnOGP_LoadDefaults
*/
void CCEtoODBDoc::OnOGP_LoadDefaults() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductRtnRev11Write)) 
   {
      ErrorAccess("You do not have a License for RTN Write!");
      return;
   }*/
   
   CWaitCursor wait;

   if (!Read_RTN_OUT())
      return;
      
   keywords[Tol_ReportText] = IsKeyWord(ATT_Tol_ReportText, FALSE);
   values[Tol_ReportText] = defaults.report;
   keywords[Tol_FeatureText] = IsKeyWord(ATT_Tol_FeatureText, FALSE);
   values[Tol_FeatureText] = defaults.feature; 
   keywords[Tol_fUpper] = IsKeyWord(ATT_Tol_fUpper, FALSE);
   values[Tol_fUpper] = &defaults.fUpper;
   keywords[Tol_fLower] = IsKeyWord(ATT_Tol_fLower, FALSE);
   values[Tol_fLower] = &defaults.fLower;
   keywords[Tol_fPrint] = IsKeyWord(ATT_Tol_fPrint, FALSE);
   values[Tol_fPrint] = &defaults.fPrint;
   keywords[Tol_fStats] = IsKeyWord(ATT_Tol_fStats, FALSE);
   values[Tol_fStats] = &defaults.fStats;
   keywords[Tol_xUpper] = IsKeyWord(ATT_Tol_xUpper, FALSE);
   values[Tol_xUpper] = &defaults.xUpper;
   keywords[Tol_xLower] = IsKeyWord(ATT_Tol_xLower, FALSE);
   values[Tol_xLower] = &defaults.xLower;
   keywords[Tol_xPrint] = IsKeyWord(ATT_Tol_xPrint, FALSE);
   values[Tol_xPrint] = &defaults.xPrint;
   keywords[Tol_xStats] = IsKeyWord(ATT_Tol_xStats, FALSE);
   values[Tol_xStats] = &defaults.xStats;
   keywords[Tol_yUpper] = IsKeyWord(ATT_Tol_yUpper, FALSE);
   values[Tol_yUpper] = &defaults.yUpper;
   keywords[Tol_yLower] = IsKeyWord(ATT_Tol_yLower, FALSE);
   values[Tol_yLower] = &defaults.yLower;
   keywords[Tol_yPrint] = IsKeyWord(ATT_Tol_yPrint, FALSE);
   values[Tol_yPrint] = &defaults.yPrint;
   keywords[Tol_yStats] = IsKeyWord(ATT_Tol_yStats, FALSE);
   values[Tol_yStats] = &defaults.yStats;
   keywords[Tol_zUpper] = IsKeyWord(ATT_Tol_zUpper, FALSE);
   values[Tol_zUpper] = &defaults.zUpper;
   keywords[Tol_zLower] = IsKeyWord(ATT_Tol_zLower, FALSE);
   values[Tol_zLower] = &defaults.zLower;
   keywords[Tol_zPrint] = IsKeyWord(ATT_Tol_zPrint, FALSE);
   values[Tol_zPrint] = &defaults.zPrint;
   keywords[Tol_zStats] = IsKeyWord(ATT_Tol_zStats, FALSE);
   values[Tol_zStats] = &defaults.zStats;
   keywords[Tol_zLoc] = IsKeyWord(ATT_Tol_zLoc, FALSE);
   values[Tol_zLoc] = &defaults.zLoc;
   keywords[Edge_Bounds] = IsKeyWord(ATT_Edge_Bounds, FALSE);
   values[Edge_Bounds] = &defaults.bounds;
   keywords[Edge_Extents] = IsKeyWord(ATT_Edge_Extents, FALSE);
   values[Edge_Extents] = &defaults.extents;
   keywords[Edge_NearestNominal] = IsKeyWord(ATT_Edge_NearestNominal, FALSE);
   values[Edge_NearestNominal] = &defaults.nearest;
   keywords[Edge_First] = IsKeyWord(ATT_Edge_First, FALSE);
   values[Edge_First] = &defaults.first;
   keywords[Edge_Last] = IsKeyWord(ATT_Edge_Last, FALSE);
   values[Edge_Last] = &defaults.last;
   keywords[Edge_FirstOf2] = IsKeyWord(ATT_Edge_FirstOf2, FALSE);
   values[Edge_FirstOf2] = &defaults.firstOf2;
   keywords[Edge_LastOf2] = IsKeyWord(ATT_Edge_LastOf2, FALSE);
   values[Edge_LastOf2] = &defaults.lastOf2;
   keywords[Edge_Contrast] = IsKeyWord(ATT_Edge_Contrast, FALSE);
   values[Edge_Contrast] = &defaults.contrast;
   keywords[Edge_WeakStrong] = IsKeyWord(ATT_Edge_WeakStrong, FALSE);
   values[Edge_WeakStrong] = &defaults.weakStrong;
   keywords[Edge_RoughSmooth] = IsKeyWord(ATT_Edge_RoughSmooth, FALSE);
   values[Edge_RoughSmooth] = &defaults.roughSmooth;
   keywords[Edge_PercentFeature] = IsKeyWord(ATT_Edge_PercentFeature, FALSE);
   values[Edge_PercentFeature] = &defaults.percentFeature;
   keywords[Edge_PercentCoverage] = IsKeyWord(ATT_Edge_PercentCoverage, FALSE);
   values[Edge_PercentCoverage] = &defaults.percentCoverage;
   keywords[Light_Profile] = IsKeyWord(ATT_Light_Profile, FALSE);
   values[Light_Profile] = &defaults.profile;
   keywords[Light_Surface] = IsKeyWord(ATT_Light_Surface, FALSE);
   values[Light_Surface] = &defaults.surface;
   keywords[Light_Ring] = IsKeyWord(ATT_Light_Ring, FALSE);
   values[Light_Ring] = &defaults.ring;
   keywords[FOV_Inches] = IsKeyWord(ATT_FOV_Inches, FALSE);
   values[FOV_Inches] = &defaults.FOVinches;
   keywords[FOV_ZoomCounts] = IsKeyWord(ATT_FOV_ZoomCounts, FALSE);
   values[FOV_ZoomCounts] = &defaults.zoomCounts;
   keywords[Centroid_IsCentroid] = IsKeyWord(ATT_Centroid_IsCentroid, FALSE);
   values[Centroid_IsCentroid] = &defaults.IsCentroid;
   keywords[Centroid_Threshold] = IsKeyWord(ATT_Centroid_Threshold, FALSE);
   values[Centroid_Threshold] = &defaults.threshold;
   keywords[Centroid_MultipleValue] = IsKeyWord(ATT_Centroid_MultipleValue, FALSE);
   values[Centroid_MultipleValue] = &defaults.multiple;
   keywords[Centroid_Filter] = IsKeyWord(ATT_Centroid_Filter, FALSE);
   values[Centroid_Filter] = &defaults.filter;
   keywords[Centroid_MultipleOn] = IsKeyWord(ATT_Centroid_MultipleOn, FALSE);
   values[Centroid_MultipleOn] = &defaults.Multiple;
   keywords[Centroid_Fill] = IsKeyWord(ATT_Centroid_Fill, FALSE);
   values[Centroid_Fill] = &defaults.Fill;
   keywords[Centroid_TouchBoundary] = IsKeyWord(ATT_Centroid_TouchBoundary, FALSE);
   values[Centroid_TouchBoundary] = &defaults.TouchBoundary;
   keywords[Centroid_Illumination] = IsKeyWord(ATT_Centroid_Illumination, FALSE);
   values[Centroid_Illumination] = &defaults.illumination;

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

         if (set[Tol_ReportText])
            SetAttrib(&data->getAttributesRef(), keywords[Tol_ReportText], VT_STRING, values[Tol_ReportText], SA_OVERWRITE, NULL);

         if (set[Tol_FeatureText])
            SetAttrib(&data->getAttributesRef(), keywords[Tol_FeatureText], VT_STRING, values[Tol_FeatureText], SA_OVERWRITE, NULL);

			int i= 0;
         for (i=Tol_fUpper; i <= Tol_zLoc; i++)
         {
            if (set[i])
               SetAttrib(&data->getAttributesRef(), keywords[i], VT_DOUBLE, values[i], SA_OVERWRITE, NULL);
         }

         for (i=Tol_fPrint; i <= Light_Ring; i++)
         {
            if (set[i])
               SetAttrib(&data->getAttributesRef(), keywords[i], VT_INTEGER, values[i], SA_OVERWRITE, NULL);
         }

         if (set[FOV_Inches])
            SetAttrib(&data->getAttributesRef(), keywords[FOV_Inches], VT_DOUBLE, values[FOV_Inches], SA_OVERWRITE, NULL);

         if (set[FOV_ZoomCounts])
            SetAttrib(&data->getAttributesRef(), keywords[FOV_ZoomCounts], VT_INTEGER, values[FOV_ZoomCounts], SA_OVERWRITE, NULL);

         for (i=PointType; i <= Centroid_Illumination; i++)
         {
            if (set[i])
               SetAttrib(&data->getAttributesRef(), keywords[i], VT_INTEGER, values[i], SA_OVERWRITE, NULL);
         }
      }
   }
}

/************************************************************************************
* Read_RTN_OUT
*/
BOOL Read_RTN_OUT()
{
   memset(&defaults, 0, sizeof(RTN_struct)); // clear defaults
   memset(set, 0, MAX_FEATURE_ATT); // initialize to none set

   CString filename = getApp().getUserPath() + "rtn.out";

   // Open File
   FILE *stream = fopen(filename, "r"); // text mode by default
   if (stream == NULL)
   {                              
      MessageBox(NULL, filename, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
   }

   char line[500];
   char *tok;
   // Check File Format
   fgets(line,500,stream);
   if (strcmp(line,"CAMCAD/OGP Defaults\n"))
   {
      MessageBox(NULL, "This is not a CAMCAD/OGP Defaults file", filename, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
   }
       
   // parse file
   while (fgets(line,120,stream)!=NULL)
   {
      if ((tok = strtok(line," \t\n")) == NULL) continue;

      if (!STRCMPI(tok, "ReportText"))
      {
         set[Tol_ReportText] = TRUE;
         tok = strtok(NULL,"\n");
         if (tok == NULL)
            defaults.report[0] = 0;
         else
            strncpy(defaults.report, tok, 50);
         defaults.report[50] = 0;
         continue;
      }

      if (!STRCMPI(tok, "FeatureText"))
      {
         set[Tol_FeatureText] = TRUE;
         tok = strtok(NULL,"\n");
         if (tok == NULL)
            defaults.feature[0] = 0;
         else
            strncpy(defaults.feature, tok, 50);
         defaults.feature[50] = 0;
         continue;
      }

      if (!STRCMPI(tok, "FUpper"))
      {
         set[Tol_fUpper] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.fUpper = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "FLower"))
      {
         set[Tol_fLower] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.fLower = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "XUpper"))
      {
         set[Tol_xUpper] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.xUpper = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "XLower"))
      {
         set[Tol_xLower] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.xLower = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "YUpper"))
      {
         set[Tol_yUpper] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.yUpper = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "YLower"))
      {
         set[Tol_yLower] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.yLower = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "ZUpper"))
      {
         set[Tol_zUpper] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.zUpper = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "ZLower"))
      {
         set[Tol_zLower] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.zLower = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "ZLoc"))
      {
         set[Tol_zLoc] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.zLoc = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "FPrint"))
      {
         set[Tol_fPrint] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.fPrint = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "FStats"))
      {
         set[Tol_fStats] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.fStats = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "XPrint"))
      {
         set[Tol_xPrint] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.xPrint = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "XStats"))
      {
         set[Tol_xStats] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.xStats = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "YPrint"))
      {
         set[Tol_yPrint] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.yPrint = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "YStats"))
      {
         set[Tol_yStats] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.yStats = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "ZPrint"))
      {
         set[Tol_zPrint] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.zPrint = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "ZStats"))
      {
         set[Tol_zStats] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.zStats = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Bounds"))
      {
         set[Edge_Bounds] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.bounds = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Extents"))
      {
         set[Edge_Extents] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.extents = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "NearestNominal"))
      {
         set[Edge_NearestNominal] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.nearest = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "FirstEdge"))
      {
         set[Edge_First] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.first = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "LastEdge"))
      {
         set[Edge_Last] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.last = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "1stOf2"))
      {
         set[Edge_FirstOf2] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.firstOf2 = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "LastOf2"))
      {
         set[Edge_LastOf2] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.lastOf2 = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Contrast"))
      {
         set[Edge_Contrast] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.contrast = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "WeakStrong"))
      {
         set[Edge_WeakStrong] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.weakStrong = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "RoughSmooth"))
      {
         set[Edge_RoughSmooth] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.roughSmooth = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "PercentFeature"))
      {
         set[Edge_PercentFeature] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.percentFeature = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "PercentCoverage"))
      {
         set[Edge_PercentCoverage] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.percentCoverage = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Profile"))
      {
         set[Light_Profile] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.profile = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Surface"))
      {
         set[Light_Surface] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.surface = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Ring"))
      {
         set[Light_Ring] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.ring = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "FOVinches"))
      {
         set[FOV_Inches] = TRUE;
         tok = strtok(NULL,"\n");
         defaults.FOVinches = atof(tok);
         continue;
      }

      if (!STRCMPI(tok, "ZoomCounts"))
      {
         set[FOV_ZoomCounts] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.zoomCounts = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Threshold"))
      {
         set[Centroid_Threshold] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.threshold = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "MultipleValue"))
      {
         set[Centroid_MultipleValue] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.multiple = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Filter"))
      {
         set[Centroid_Filter] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.filter = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "MultipleOn"))
      {
         set[Centroid_MultipleOn] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.Multiple = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Fill"))
      {
         set[Centroid_Fill] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.Fill = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "TouchBoundary"))
      {
         set[Centroid_TouchBoundary] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.TouchBoundary = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "Illumination"))
      {
         set[Centroid_Illumination] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.illumination = atoi(tok);
         continue;
      }

      if (!STRCMPI(tok, "IsCentroid"))
      {
         set[Centroid_IsCentroid] = TRUE;
         tok = strtok(NULL, "\n");
         defaults.IsCentroid = atoi(tok);
         continue;
      }
   }

   fclose(stream);
   return TRUE;
}

/************************************************************************************
* Write_RTN_OUT
*/
void Write_RTN_OUT()
{
   CString filename = getApp().getUserPath() + "rtn.out";

   // Open File
   FILE *stream = fopen(filename, "w"); // text mode by default
   if (stream == NULL)
   {                              
      MessageBox(NULL, filename, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fputs("CAMCAD/OGP Defaults\n\n", stream);

   if (set[Tol_ReportText])
      fprintf(stream, "ReportText\t%s\n", defaults.report);

   if (set[Tol_FeatureText])
      fprintf(stream, "FeatureText\t%s\n", defaults.feature);

   fputs("\n", stream);

   if (set[Tol_fUpper])
      fprintf(stream, "FUpper\t%g\n", defaults.fUpper);

   if (set[Tol_fLower])
      fprintf(stream, "FLower\t%g\n", defaults.fLower);

   if (set[Tol_fPrint])
      fprintf(stream, "FPrint\t%d\n", defaults.fPrint);

   if (set[Tol_fStats])
      fprintf(stream, "FStats\t%d\n", defaults.fStats);

   fputs("\n", stream);

   if (set[Tol_xUpper])
      fprintf(stream, "XUpper\t%g\n", defaults.xUpper);

   if (set[Tol_xLower])
      fprintf(stream, "XLower\t%g\n", defaults.xLower);

   if (set[Tol_xPrint])
      fprintf(stream, "XPrint\t%d\n", defaults.xPrint);

   if (set[Tol_xStats])
      fprintf(stream, "XStats\t%d\n", defaults.xStats);

   fputs("\n", stream);

   if (set[Tol_yUpper])
      fprintf(stream, "YUpper\t%g\n", defaults.yUpper);

   if (set[Tol_yLower])
      fprintf(stream, "YLower\t%g\n", defaults.yLower);

   if (set[Tol_yPrint])
      fprintf(stream, "YPrint\t%d\n", defaults.yPrint);

   if (set[Tol_yStats])
      fprintf(stream, "YStats\t%d\n", defaults.yStats);

   fputs("\n", stream);

   if (set[Tol_zUpper])
      fprintf(stream, "ZUpper\t%g\n", defaults.zUpper);

   if (set[Tol_zLower])
      fprintf(stream, "ZLower\t%g\n", defaults.zLower);

   if (set[Tol_zPrint])
      fprintf(stream, "ZPrint\t%d\n", defaults.zPrint);

   if (set[Tol_zStats])
      fprintf(stream, "ZStats\t%d\n", defaults.zStats);

   fputs("\n", stream);

   if (set[Tol_zLoc])
      fprintf(stream, "ZLoc\t%g\n", defaults.zLoc);

   fputs("\n", stream);

   if (set[Edge_Bounds])
      fprintf(stream, "Bounds\t%d\n", defaults.bounds);

   if (set[Edge_Extents])
      fprintf(stream, "Extents\t%d\n", defaults.extents);

   if (set[Edge_NearestNominal])
      fprintf(stream, "NearestNominal\t%d\n", defaults.nearest);

   if (set[Edge_First])
      fprintf(stream, "FirstEdge\t%d\n", defaults.first);

   if (set[Edge_Last])
      fprintf(stream, "LastEdge\t%d\n", defaults.last);

   if (set[Edge_FirstOf2])
      fprintf(stream, "1stOf2\t%d\n", defaults.firstOf2);

   if (set[Edge_LastOf2])
      fprintf(stream, "LastOf2\t%d\n", defaults.lastOf2);

   if (set[Edge_Contrast])
      fprintf(stream, "Contrast\t%d\n", defaults.contrast);

   if (set[Edge_WeakStrong])
      fprintf(stream, "WeakStrong\t%d\n", defaults.weakStrong);

   if (set[Edge_RoughSmooth])
      fprintf(stream, "RoughSmooth\t%d\n", defaults.roughSmooth);

   if (set[Edge_PercentFeature])
      fprintf(stream, "PercentFeature\t%d\n", defaults.percentFeature);

   if (set[Edge_PercentCoverage])
      fprintf(stream, "PercentCoverage\t%d\n", defaults.percentCoverage);

   fputs("\n", stream);

   if (set[Light_Profile])
      fprintf(stream, "Profile\t%d\n", defaults.profile);

   if (set[Light_Surface])
      fprintf(stream, "Surface\t%d\n", defaults.surface);

   if (set[Light_Ring])
      fprintf(stream, "Ring\t%d\n", defaults.ring);

   fputs("\n", stream);

   if (set[FOV_Inches])
      fprintf(stream, "ZoomCounts\t%d\n", defaults.zoomCounts);

   if (set[FOV_ZoomCounts])
      fprintf(stream, "FOVinches\t%g\n", defaults.FOVinches);

   fputs("\n", stream);

   if (set[Centroid_IsCentroid])
      fprintf(stream, "IsCentroid\t%d\n", defaults.IsCentroid);

   if (set[Centroid_Threshold])
      fprintf(stream, "Threshold\t%d\n", defaults.threshold);

   if (set[Centroid_MultipleValue])
      fprintf(stream, "MultipleValue\t%d\n", defaults.multiple);

   if (set[Centroid_Filter])
      fprintf(stream, "Filter\t%d\n", defaults.filter);

   if (set[Centroid_MultipleOn])
      fprintf(stream, "MultipleOn\t%d\n", defaults.Multiple);

   if (set[Centroid_Fill])
      fprintf(stream, "Fill\t%d\n", defaults.Fill);

   if (set[Centroid_TouchBoundary])
      fprintf(stream, "TouchBoundary\t%d\n", defaults.TouchBoundary);

   if (set[Centroid_Illumination])
      fprintf(stream, "Illumination\t%d\n", defaults.illumination);

   fclose(stream);
}


