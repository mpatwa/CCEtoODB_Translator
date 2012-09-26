#include "StdAfx.h"
#include "DcaTopics.h"

//-------------------------------------------
//CTopicNodeTemplate
//-------------------------------------------
CTopicNodeTemplate::CTopicNodeTemplate()
{
   m_nodetype = T_NULL;
   m_left = 0;
   m_right = 0;
   m_top = 0;
   m_bottom = 0;
   m_visible = 0;
   
   m_text = "";
   m_title = "";
   m_user = "";
   m_geometryName = "";
   
   m_OverideStickyColor = 0;
   m_StickyBackColor = 0;
   m_color = 0;
   
   m_x1 = 0;
   m_y1 = 0;
   m_x2 = 0;
   m_y2 = 0;
   
   m_OverideThicknessSize = 0;
   m_OverideColor = 0;
   m_OverideThickness = 0;
   m_EndArrow = 0; 
   m_StartArrow = 0; 
   m_OverideArrow = 0; 
   m_ItemColor = 0;
   m_pntlist.empty();
}

CTopicNodeTemplate::~CTopicNodeTemplate()
{
   while (m_pntlist.GetCount())
   {
      delete m_pntlist.RemoveHead();
   }
}

//-------------------------------------------
//CMarkups
//-------------------------------------------
CMarkups::~CMarkups()
{
   this->removeChildren();
}

void CMarkups::removeChildren()
{
   for(int i = 0; i < this->GetCount(); i++)
   {
      CTopicNodeTemplate *node = this->GetAt(i);
      if(node)
      {
         delete node;
      }
   }
   this->RemoveAll();
}

//-------------------------------------------
//CTopic
//-------------------------------------------
CTopic::~CTopic()
{
   this->removeChildren();
}

void CTopic::removeChildren()
{
   for(int i = 0; i < this->GetCount(); i++)
   {
      CIssue *issuenode  = this->GetAt(i);
      if(issuenode)
      {
         issuenode->removeChildren();
         delete issuenode;
      }
   }
   this->RemoveAll();
}

//-------------------------------------------
//CTopicsList
//-------------------------------------------
CTopicsList::~CTopicsList()
{
   this->removeChildren();
}

void CTopicsList::removeChildren()
{
   for(int i =0; i < this->GetCount(); i++)
   {
      CTopic *topicnode = this->GetAt(i);
      if(topicnode)
      {
         topicnode->removeChildren();      
         delete topicnode;
      }
   }
   this->RemoveAll();
}