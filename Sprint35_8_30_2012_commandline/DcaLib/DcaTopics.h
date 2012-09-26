// $Header: /CAMCAD/5.0/dca/DcaMarkups.h 4     9/17/07 12:00p Kurt Van Ness $

#if !defined(__DcaTopics_h__)
#define __DcaTopics_h__

#pragma once

#include "DcaContainer.h"
#include "DcaPnt.h"

enum TopicNodeTypeTag
{
   T_NULL,
   T_StickNote,
   T_FreeText,
   T_Line,
   T_Polyline,
   T_FreehandDraw,
   T_Rectangle,
   T_XOutRectangle,
   T_Ellipse,
   T_XOutEllipse,
   T_Cloud,
   T_Dimension,
   T_LeaderNote,
};

class CTopicNodeTemplate
{
private:
   TopicNodeTypeTag m_nodetype;
   CPntList m_pntlist;

   double m_left;
   double m_right;
   double m_top;
   double m_bottom;
   double m_x1;
   double m_y1;
   double m_x2;
   double m_y2;
   double m_OverideThicknessSize;

   CString m_text;
   CString m_title;
   CString m_user;
   CString m_geometryName;

   int m_visible;
   int m_OverideStickyColor;
   int m_StickyBackColor;
   int m_color;
   int m_OverideColor;
   int m_OverideThickness;
   int m_EndArrow; 
   int m_StartArrow; 
   int m_OverideArrow; 
   int m_ItemColor;

public:
   CTopicNodeTemplate();
   virtual ~CTopicNodeTemplate();

   TopicNodeTypeTag getNodeType(){return m_nodetype;}
   CPntList& getPointList(){return m_pntlist;}

   // Set parameters
   void setNodeType(TopicNodeTypeTag val){m_nodetype = val;}
   void setNoteLeft(double val){m_left = val;}
   void setNoteRight(double val){m_right = val;}
   void setNoteTop(double val){m_top = val;}
   void setNoteBottom(double val){m_bottom = val;}
   void setx1(double val){m_x1 = val;}
   void sety1(double val){m_y1 = val;}
   void setx2(double val){m_x2 = val;}
   void sety2(double val){m_y2 = val;}
   void setOverideThicknessSize(double val){m_OverideThicknessSize = val;}

   // Set parameters
   void setText(const CString &val){m_text = val;}
   void setTitle(const CString &val){m_title = val;}
   void setUser(const CString &val){m_user = val;}
   void setGeometryName(const CString &val){m_geometryName = val;}
   
   void setOverideStickyColor(int val){m_OverideStickyColor = val;}
   void setStickyBackColor(int val){m_StickyBackColor = val;}
   void setColor(int val){m_color = val;}
   void setVisible(int val){m_visible = val;}
   void setOverideColor(int val){m_OverideColor = val;}
   void setOverideThickness(int val){m_OverideThickness = val;}
   void setEndArrow(int val){m_EndArrow = val;}
   void setStartArrow(int val){m_StartArrow = val;}
   void setOverideArrow(int val){m_OverideArrow = val;}
   void setItemColor(int val){m_ItemColor = val;}

   // Get parameters
   double getNoteLeft(){return m_left;}
   double getNoteRight(){return m_right;}
   double getNoteTop(){return m_top;}
   double getNoteBottom(){return m_bottom;}
   double getx1(){return m_x1;}
   double gety1(){return m_y1;}
   double getx2(){return m_x2;}
   double gety2(){return m_y2;}
   double getOverideThicknessSize(){return m_OverideThicknessSize;}

    // Get parameters
   CString& getText(){return m_text;}
   CString& getTitle(){return m_title;}
   CString& getUser(){return m_user;}
   CString& getGeometryName(){return m_geometryName;}

   int getOverideStickyColor(){return m_OverideStickyColor;}
   int getStickyBackColor(){return m_StickyBackColor;}
   int getColor(){return m_color;}
   int getVisible(){return m_visible;}
   int getOverideColor(){return m_OverideColor;}
   int getOverideThickness(){return m_OverideThickness;}
   int getEndArrow(){return m_EndArrow;}
   int getStartArrow(){return m_StartArrow;}
   int getOverideArrow(){return m_OverideArrow;}
   int getItemColor(){return m_ItemColor;}
};


class CMarkups : public CTypedPtrArrayContainer<CTopicNodeTemplate*>
{
public:
   ~CMarkups();
   void removeChildren();
};

class CIssue : public CMarkups
{
private:
   int  m_isGeneralIssue;
   CString m_title;
   CString m_user;
public:
   // Set parameters
   void setisGeneralIssue(int val){ m_isGeneralIssue = val; }
   void setTitle(const CString &val){ m_title = val; }
   void setUser(const CString &val){ m_user = val; }   
   // Get parameters
   int IsGeneralIssue(){ return m_isGeneralIssue; }
   CString& getTitle(){return m_title;}
   CString& getUser(){return m_user;}
};

class CTopic : public CTypedPtrArrayContainer<CIssue *>
{
private:
   CString m_title;
   CString m_user;
public:
   ~CTopic();
   // Set parameters
   void setTitle(const CString &val){ m_title = val; }
   void setUser(const CString &val){ m_user = val; }
   // Get parameters
   CString& getTitle(){return m_title;}
   CString& getUser(){return m_user;}

   void removeChildren();
};

class CTopicsList : public CTypedPtrArrayContainer<CTopic*>
{
private:
   PageUnitsTag m_pageUnit;
public:
   ~CTopicsList();
   void removeChildren();

   void setPageUnits(PageUnitsTag pageunit) {m_pageUnit = pageunit;}
   PageUnitsTag getPageUnits() {return m_pageUnit; }

};
#endif/*__DcaTopics_h__*/