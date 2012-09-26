// $Header: /CAMCAD/5.0/MeasureDlg.h 3     3/12/07 12:42p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// MeasureDlg.h

#if ! defined (__MeasureDlg_h__)
#define __MeasureDlg_h__

#pragma once

#include "CcView.h"
#include "CamCadDatabase.h"
//#include "GerberEducatorUi.h"

enum EMeasureObjectType
{
	measureObjectPoint,
	measureObjectEntity,
	measureObjectEntityCenter,
};

//-----------------------------------------------------------------------------
// CMeasureObject
//-----------------------------------------------------------------------------
class CMeasureObject
{
public:
	CMeasureObject();
	~CMeasureObject();

private:
	EMeasureObjectType m_type;
	DataStruct* m_entity;
	CPoint2d m_point;
	bool m_isSet;

public:
   void reset();

   EMeasureObjectType getType() const { return m_type; }
   void setType(const EMeasureObjectType type)  { m_type = type; }

   DataStruct* getEntity() { return m_entity; }
   void setEntity(DataStruct* entity) { m_entity = entity; }

   CPoint2d getPoint() const { return m_point; }
   void setPoint(const CPoint2d point) { m_point = point; }

   void set(const bool set) { m_isSet = set; }
   bool isSet() const { return m_isSet; }
};


//-----------------------------------------------------------------------------
// CMeasure
//-----------------------------------------------------------------------------
class CMeasure
{
public:
   CMeasure(CCEtoODBView& camCadView, CCEtoODBDoc& camCadDoc, EMeasureDialogType dialogType);
   ~CMeasure();

private:
	CCEtoODBView& m_camCadView;
   CCEtoODBDoc& m_camCadDoc;
   CCamCadDatabase m_camCadDatabase;

   CMeasureBaseDialog* m_dialog;
   EMeasureDialogType m_dialogType;

	CMeasureObject m_fromObject;
	CMeasureObject m_toObject;
   bool m_firstDrag;
   bool m_measureOn;

   double m_fromInsertX;
   double m_fromInsertY;
   double m_fromScale;
   double m_fromRotation;
   char m_fromMirror;
   CDataList* m_fromDataList;

   void deleteDialog();

public:
   void reset();
   CMeasureBaseDialog& getDialog();
   CCEtoODBView& getCamCadView() { return m_camCadView; }
   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   CCamCadData& getCamCadData() { return m_camCadDoc.getCamCadData(); }
   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }   
   CMeasureObject& getFromObject() { return m_fromObject; }
   CMeasureObject& getToObject() { return m_toObject; }
   EMeasureDialogType getDialogType() { return m_dialogType; }

   bool getFirstDrag() const { return m_firstDrag; }
   void setFirstDrag(const bool firstDrag) { m_firstDrag = firstDrag; }

   double getFromInsertX() const { return m_fromInsertX; }
   void setFromInsertX(const double x) { m_fromInsertX = x; }

   double getFromInsertY() const { return m_fromInsertY; }
   void setFromInsertY(const double y) { m_fromInsertY = y; }

   double getFromScale() const { return m_fromScale; }
   void setFromScale(const double scale) { m_fromScale = scale; }

   double getFromRotation() const { return m_fromRotation; }
   void setFromRotation(const double rotation) { m_fromRotation = rotation; }

   char getFromMirror() const { return m_fromMirror; }
   void setFromMirror(const char mirror) { m_fromMirror =  mirror; }

   CDataList* getFromDataList() { return m_fromDataList; }
   void setFromDataList(CDataList* dataList) { m_fromDataList = dataList; }

   bool isMeasureValid();
   double getDeltaX();
   double getDeltaY();
   double getDistance();
   double getAngleInDegree();
   double getAngleInRadian();

   bool isMeasureOn() { return m_measureOn; }
   void setMeasureOn(const bool on);
   
   void cleanGhosts();
   void drawMeasureGraphic(CClientDC& dc, double factor);
   void updateMeasure();
   BOOL PreTranslateMessage(MSG* pMsg);
};

//-----------------------------------------------------------------------------
// CMeasureBaseDialog
//-----------------------------------------------------------------------------
class CMeasureBaseDialog : public CResizingDialog
{
public:
   CMeasureBaseDialog(int dialogId, CMeasure& measure);
   ~CMeasureBaseDialog();

protected:
   CMeasure& m_measure;

public:
   virtual void updateMeasure() = 0;
//   virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif
