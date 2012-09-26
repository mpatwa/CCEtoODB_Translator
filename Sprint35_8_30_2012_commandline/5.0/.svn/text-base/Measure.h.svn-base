// $Header: /CAMCAD/4.5/Measure.h 15    12/23/05 7:00p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#if !defined(__Measure_h__)
#define __Measure_h__

#pragma once

#include "ccview.h"
#include "data.h"             
#include "xform.h"
#include "MeasureDlg.h"

//void EndMeasure();

// Measure Functions
double FindClosestPoints(CCEtoODBDoc *doc, 
      DataStruct *data1, DTransform *xform1, BOOL Centerline1, 
      DataStruct *data2, DTransform *xform2, BOOL Centerline2,
      Point2 *result1, Point2 *result2);

double FindClosestPoint(CCEtoODBDoc *doc, Point2 *Point, DTransform *pointXform,
      DataStruct *data, DTransform *dataXform, BOOL Centerline,
      Point2 *result);


// Entity to Entity
double MeasurePolyListToPolyList(CCEtoODBDoc *doc, 
      CPolyList *polylist1, DTransform *xform1, BOOL Centerline1, 
      CPolyList *polylist2, DTransform *xform2, BOOL Centerline2,       
      Point2 *result1, Point2 *result2);

double MeasurePolyListToPoly(CCEtoODBDoc *doc, 
      CPolyList *polylist, DTransform *plXform, BOOL plCenterline,  
      CPoly *poly, DTransform *pXform, BOOL pCenterline,       
      Point2 *plResult, Point2 *pResult);

double MeasurePolyToPoly(CCEtoODBDoc *doc, 
      CPoly *poly1, DTransform *xform1, BOOL Centerline1, 
      CPoly *poly2, DTransform *xform2, BOOL Centerline2,
      Point2 *result1, Point2 *result2);

double MeasurePolyToPoly( 
      const CPoly* poly1,const DTransform* xform1, BOOL Centerline1, double width1, 
      const CPoly* poly2,const DTransform* xform2, BOOL Centerline2, double width2,
      Point2 *result1, Point2 *result2);

double MeasurePolyToLine(const CPoly *poly, double pWidth, const DTransform *pXform, 
      const CPnt *la, const CPnt *lb, double lWidth, const DTransform *lXform, 
      Point2 *pResult, Point2 *lResult);

double MeasurePolyToArc(const CPoly *poly, double pWidth, const DTransform *pXform, 
      const CPnt *c, double r, double sa, double da, double aWidth, const DTransform *aXform, 
      Point2 *pResult, Point2 *aResult);

double MeasureLineToLine(const CPnt *l1a, const CPnt *l1b, double width1, const DTransform *xform1, 
      const CPnt *l2a, const CPnt *l2b, double width2, const DTransform *xform2,
      Point2 *result1, Point2 *result2);

double MeasureLineToArc(const CPnt *la, const CPnt *lb, double lWidth, const DTransform *lXform,
                        const CPnt *c, double r, double sa, double da, double aWidth, const DTransform *aXform,
                        Point2 *lResult, Point2 *aResult);

double MeasureArcToArc(const CPnt *c1, double r1, double sa1, double da1, double width1, const DTransform *xform1, 
                       const CPnt *c2, double r2, double sa2, double da2, double width2, const DTransform *xform2,
                       Point2 *result1, Point2 *result2);


double MeasureApertureToAperture(CCEtoODBDoc *doc,
      BlockStruct *b1, double x1, double y1, double rotation1, int mirror1,
      BlockStruct *b2, double x2, double y2, double rotation2, int mirror2,
      Point2 *result1, Point2 *result2);

// Point to Entity
double MeasurePointToPolylist(CCEtoODBDoc *doc, Point2 *point,
      CPolyList *polylist, DTransform *polylistXform, BOOL Centerline,
      Point2 *result);

double MeasurePointToPoly(CCEtoODBDoc *doc, Point2 *point,
      CPoly *poly, DTransform *polyXform, BOOL Centerline, 
      Point2 *result);

double MeasurePointToPoly(Point2 *point,
      const CPoly *poly, const DTransform *polyXform, BOOL Centerline, double width,
      Point2 *result);

double MeasurePointToPntList(Point2 *point,
      const CPntList *pntList, double width, const DTransform *polyXform, BOOL Centerline, 
      Point2 *result);

double MeasurePointToLine(Point2 *point,
      const CPnt *a, const CPnt *b, double width, const DTransform *lXform,
      Point2 *result);

double MeasurePointToArc(Point2 *point,
      const CPnt *c, double r, double sa, double da, double aWidth, const DTransform *aXform,
      Point2 *result);

//struct MeasureStruct
//{
//   BOOL Measuring,   // if in measure mode
//      FromPoint,     // From Point or Entity
//      ToPoint,       // To Point or Entity
//      FromSet,       // From is Set
//      ToSet,         // To is Set
//      FromCenter,    // From Centerline
//      ToCenter,      // To Centerline
//      FirstDrag;
//   double FromX, FromY,    // if FromPoint
//      ToX, ToY;            // if ToPoint
//   DataStruct *FromEntity, // if !FromPoint
//      *ToEntity;           // if !ToPoint
//   CCEtoODBView *MeasureView;
//
//   double From_insert_x, From_insert_y, From_scale, From_rotation;
//   char From_mirror;
//        //From_centerline;//fawn
//   CDataList *From_DataList;
//};

/////////////////////////////////////////////////////////////////////////////
// CMeasureDlg dialog
class CMeasureDlg : public CMeasureBaseDialog  // CDialog
{
// Construction
public:
   CMeasureDlg(CMeasure& measureL);   // standard constructor

// Dialog Data
   //{{AFX_DATA(CMeasureDlg)
   enum { IDD = IDD_MEASURE };
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMeasureDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual void OnCancel();
   void InitMeasureDlg(BOOL UpdateRadioButtons);
   void updateMeasure();
   void updateMeasureFromObject();
   void updateMeasureToObject();
   BOOL PreTranslateMessage(MSG* pMsg);

protected:
   // Generated message map functions
   //{{AFX_MSG(CMeasureDlg)
   afx_msg void OnNewMeasurement();
   afx_msg void OnFromCenter();
   afx_msg void OnFromEntity();
   afx_msg void OnFromPoint();
   afx_msg void OnToCenter();
   afx_msg void OnToEntity();
   afx_msg void OnToPoint();
   afx_msg void OnDone();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

// end MEASURE.H

#endif
