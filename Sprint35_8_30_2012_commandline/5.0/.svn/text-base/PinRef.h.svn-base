// $Header: /CAMCAD/4.4/read_wrt/PinRef.h 9     12/19/08 2:03a Sharry $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__PinRef_h__)
#define __PinRef_h__

#pragma once

#include "CCDOC.h"
#include "Graph.h"

enum Rotation_AdjustType
{
   Rotation_UnKnown,
   Rotation_Mirror,
   Rotation_RightReading,
};

/******************************************************************************
* CPinRef
*/
class CPinRef
{
private:
   CString  m_ComponentName;
   CString  m_PinNumber;
   CString  m_UserdefString;
   CString  m_fontName;
   CString  m_layerName;

   double   m_locationX;
   double   m_locationY;
   double   m_Rotation;
   double   m_Width;
   double   m_Height;
   double   m_Pen;

   int      m_sysPageUnit;
   bool     m_Visible;

   HorizontalPositionTag   m_horizontalposition;
   VerticalPositionTag     m_verticalposition;
   CMapStringToInt         m_unitMap;
   Rotation_AdjustType     m_rotationType;

public:
   CPinRef(int sysPageUnit);
   ~CPinRef(void);

   //Parameter
   double getScaleFactor(CString unitName);
   
   Rotation_AdjustType getRotationType(){return m_rotationType;}
   void setRotationType(Rotation_AdjustType val){m_rotationType = val;}

   CString& getComponentName(){ return m_ComponentName;}
   void setComponentName(CString val) {m_ComponentName = val;}

   CString& getPinNumber(){ return m_PinNumber;}
   void setPinNumber(CString val) {m_PinNumber = val;}

   int getSystemPageUnit(){return m_sysPageUnit;}
   void setSystemPageUnit(int val){m_sysPageUnit = val;}

   CString& getUserdefString(){ return m_UserdefString;}
   void setUserdefString(CString val){m_UserdefString = val;}

   CString& getLayerName(){return m_layerName;}
   void setLayerName(CString val){m_layerName = val;}

   CString& getFontName(){return m_fontName;}
   void setFontName(CString val){m_fontName = val;}
   
   bool isVisible(){return m_Visible;}
   void setVisible(bool val){m_Visible = val;}   

   double getX(){ return m_locationX;}
   void setX(double val){m_locationX = val;}

   double getY(){ return m_locationY;}
   void setY(double val){m_locationY = val;}

   double getWidth(){ return m_Width;}
   void setWidth(double val){m_Width = val;}

   double getHeight(){ return m_Height;}
   void setHeight(double val){m_Height = val;}

   double getPen(){ return m_Pen;}
   void setPen(double val){m_Pen = val;}

   HorizontalPositionTag getHorizontalPosition(){return m_horizontalposition;}
   void setHorizontalPosition(HorizontalPositionTag val){m_horizontalposition = val;}

   VerticalPositionTag getVerticalPosition(){return m_verticalposition;}
   void setVerticalPosition(VerticalPositionTag val){m_verticalposition = val;}

   double getRotationDegrees(){return m_Rotation;}
   double getRotationRadians(){return degreesToRadians(m_Rotation);}
   void setRotationDegrees(double val){m_Rotation = val;}
   
   //virtual function: allow different type of pinref files to handle its own data
   virtual double getAdjustedAngleDegree(double componentAngle, double pinAngle){ return 0.;}
   virtual double getAdjustedAngleRadians(double componentAngle, double pinAngle){ return 0.;}
   virtual bool setPinRef(CString CompName, CString PinNumber, CStringArray &propertyArray);
};

/******************************************************************************
* CPinRefListMap
*/
class CPinRefListMap : public CTypedPtrMap<CMapStringToPtr,CString,CPinRef*>
{
private:
   CString m_CompName;
public:
   CPinRefListMap(CString CompName){m_CompName = CompName;}
   ~CPinRefListMap();
   void removeAllNodes();

   void AddPinRefAt(CString pinNumber, int sysPageUnit, CPinRef *pinRef, CStringArray &refDesArray);
};

/******************************************************************************
* CCompToPinRefListMap
*/
class CCompToPinRefListMap : public CTypedPtrMap<CMapStringToPtr,CString,CPinRefListMap*>
{
private:
   CCEtoODBDoc *m_Doc;
   int         m_SysPageUnit;
   double      m_compAngle;

public:
   CCompToPinRefListMap(CCEtoODBDoc *doc, int sys_pageunit);
   ~CCompToPinRefListMap();
   void removeAllNodes();

   void AddPinRefListAt(CString CompName, CPinRefListMap *&pinRefList);
   BlockStruct *FindComponentGeometry(FileStruct *pcbFile, CString compName, DataStruct* &component);
   BlockStruct *CloneComponentGeometry(BlockStruct *componentGeometry, CString compName);
   bool LookupPinRef(CString compName, CString pinNumber, CPinRef *&pinRef);

   void AddPinRefs(CString compName, CString pinNumber, CPinRef *pinRef, CStringArray &refDesArray);
   bool UpdatePinRefToAttributes(DataStruct* newPinRefdata, CPinRef* pinref);
   void UpdatePinRefAttributeToCAMCAD(FileStruct *pcbFile,CStringArray& logmessage);
};

#endif /*__PinRef_h__*/