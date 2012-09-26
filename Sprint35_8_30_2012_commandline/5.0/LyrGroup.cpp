// $Header: /CAMCAD/4.5/LyrGroup.cpp 17    5/02/06 3:14p Kurt Van Ness $

#include "StdAfx.h"
#include "CCEtoODB.h"
#include "LyrGroup.h"
#include "ccdoc.h"
#include "PersistantColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*******************************************************************
* AddLayerGroup 
*/
LayerGroupStruct *CCEtoODBDoc::AddLayerGroup(const char *name)
{
   POSITION pos = LayerGroupList.GetHeadPosition();
   while (pos != NULL)
   {
      LayerGroupStruct *group = LayerGroupList.GetNext(pos);
      if (!group->name.Compare(name))
         return group;
   }

   LayerGroupStruct *group = new LayerGroupStruct;
   group->name = name;
   group->electricalstacknumber = 0;
   group->artworkstacknumber = 0;
   group->physicalstacknumber = 0;
   LayerGroupList.AddTail(group);
   return group;
}

/*******************************************************************
* FindLayerGroup 
*/

LayerGroupStruct *CCEtoODBDoc::FindLayerGroup(const char *name)
{
   POSITION pos = LayerGroupList.GetHeadPosition();
   while (pos != NULL)
   {
      LayerGroupStruct *group = LayerGroupList.GetNext(pos);
      if (!group->name.Compare(name))
         return group;
   }
   return NULL;
}

/*******************************************************************
* LayerGroup_Show 
*/
void CCEtoODBDoc::LayerGroup_Show(LayerGroupStruct *group, BOOL On)
{
   POSITION pos;
      
   pos = group->LayerList.GetHeadPosition();
   while (pos != NULL)
      group->LayerList.GetNext(pos)->setVisible(On);

   pos = group->GroupList.GetHeadPosition();
   while (pos != NULL)
   {
      LayerGroupStruct *subgroup = group->GroupList.GetNext(pos);
      LayerGroup_Show(subgroup, On);
   }
}

/*******************************************************************
* LayerGroup_Color 
*/
void CCEtoODBDoc::LayerGroup_Color(LayerGroupStruct *group, COLORREF color)
{
   POSITION pos;
      
   pos = group->LayerList.GetHeadPosition();
   while (pos != NULL)
      group->LayerList.GetNext(pos)->setColor(color);

   pos = group->GroupList.GetHeadPosition();
   while (pos != NULL)
   {
      LayerGroupStruct *subgroup = group->GroupList.GetNext(pos);
      LayerGroup_Color(subgroup, color);
   }
}

/////////////////////////////////////////////////////////////////////////////
// LayerGroupsCreate dialog
void CCEtoODBDoc::OnCreateLayerGroups() 
{
   LayerGroupsCreate dlg;
   dlg.doc = this;
   dlg.DoModal();
}

LayerGroupsCreate::LayerGroupsCreate(CWnd* pParent /*=NULL*/)
   : CDialog(LayerGroupsCreate::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerGroupsCreate)
   //}}AFX_DATA_INIT
}

void LayerGroupsCreate::OnCancel()
{
   delete m_imageList;

   // free memory for list data
   int count = m_list.GetItemCount();
   for (int index=0; index < count; index++)
      delete (GroupOrLayer*)m_list.GetItemData(index);

   // free memory for tree data
   HTREEITEM root = m_tree.GetRootItem();
   while (root != NULL)
   {
      HTREEITEM child = m_tree.GetChildItem(root);
      while (child != NULL)
      {
         delete (GroupOrLayer*)m_tree.GetItemData(child); 
         HTREEITEM temp = child;
         child = m_tree.GetNextSiblingItem(temp);
      }
      
      delete (GroupOrLayer*)m_tree.GetItemData(root); 
      HTREEITEM temp = root;
      root = m_tree.GetNextSiblingItem(temp);
   }

   CDialog::OnCancel();
}

void LayerGroupsCreate::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerGroupsCreate)
   DDX_Control(pDX, IDC_TREE, m_tree);
   DDX_Control(pDX, IDC_LIST, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LayerGroupsCreate, CDialog)
   //{{AFX_MSG_MAP(LayerGroupsCreate)
   ON_BN_CLICKED(IDC_NEW_GROUP, OnNewGroup)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_REMOVE, OnRemove)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerGroupsCreate message handlers
int InsertItemIntoList(CListCtrl *m_list, const char *name, BOOL IsAGroup)
{
   static int j = 0;
   int index = m_list->InsertItem(j, name, IsAGroup);
   // ugly, dirty way to try to work around microsoft bug
   if (index == -1)
   {
      index = m_list->InsertItem(j, name, IsAGroup);
      if (index == -1)
      {
         index = m_list->InsertItem(j, name, IsAGroup);
         if (index == -1)
         {
            index = m_list->InsertItem(j, name, IsAGroup);
            if (index == -1)
            {
               index = m_list->InsertItem(j, name, IsAGroup);
               if (index == -1)
                  index = m_list->InsertItem(j, name, IsAGroup);
            }
         }
      }
   }
   j++;
   return index;
}

BOOL LayerGroupsCreate::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // images
   m_imageList = new CImageList();
   m_imageList->Create(16, 16, TRUE, 2, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_LAYER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_list.SetImageList(m_imageList, LVSIL_SMALL);
   m_tree.SetImageList(m_imageList, TVSIL_NORMAL);

   for (int i=0; i < doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      int index = InsertItemIntoList(&m_list, layer->getName(), FALSE);
      GroupOrLayer *d = new GroupOrLayer;
      d->IsAGroup = FALSE;
      d->layer = layer;
      m_list.SetItemData(index, (LPARAM)d);
   }
   
   POSITION pos = doc->LayerGroupList.GetHeadPosition();
   while (pos != NULL)
   {
      LayerGroupStruct *group = doc->LayerGroupList.GetNext(pos);

      int index = InsertItemIntoList(&m_list, group->name, TRUE);
      GroupOrLayer *d = new GroupOrLayer;
      d->IsAGroup = TRUE;
      d->group = group;
      m_list.SetItemData(index, (LPARAM)d);
   }

   FillTree();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerGroupsCreate::FillTree() 
{
   m_tree.DeleteAllItems();

   HTREEITEM root, item;
   GroupOrLayer *d;

   // loop groups in LayerGroupList
   POSITION groupPos = doc->LayerGroupList.GetHeadPosition();
   while (groupPos != NULL)
   {
      LayerGroupStruct *group = doc->LayerGroupList.GetNext(groupPos);

      root = m_tree.InsertItem(group->name, 1, 1);
      d = new GroupOrLayer;
      d->IsAGroup = TRUE;
      d->group = group;
      m_tree.SetItemData(root, (DWORD)d);

      // loop sub-groups in this group
      POSITION subGroupPos = group->GroupList.GetHeadPosition();
      while (subGroupPos != NULL)
      {
         LayerGroupStruct *subGroup = group->GroupList.GetNext(subGroupPos);

         item = m_tree.InsertItem(subGroup->name, 1, 1, root);
         d = new GroupOrLayer;
         d->IsAGroup = TRUE;
         d->group = subGroup;
         m_tree.SetItemData(item, (DWORD)d);
      }

      // loop layers in this group
      POSITION layerPos = group->LayerList.GetHeadPosition();
      while (layerPos != NULL)
      {
         LayerStruct *layer = group->LayerList.GetNext(layerPos);

         item = m_tree.InsertItem(layer->getName(), 0, 0, root);
         d = new GroupOrLayer;
         d->IsAGroup = FALSE;
         d->layer = layer;
         m_tree.SetItemData(item, (DWORD)d);
      }
   }
}

void LayerGroupsCreate::OnNewGroup() 
{
   InputDlg input;
   input.m_prompt = "Name of Layer Group";
   if (input.DoModal() != IDOK)
      return;

   if (doc->FindLayerGroup(input.m_input))
   {
      ErrorMessage(input.m_input, "Group Name alredy exists!");
      return;
   }

   LayerGroupStruct *group = doc->AddLayerGroup(input.m_input);

   int index = InsertItemIntoList(&m_list, group->name, TRUE);
   GroupOrLayer *d = new GroupOrLayer;
   d->IsAGroup = TRUE;
   d->group = group;
   m_list.SetItemData(index, (LPARAM)d);

   HTREEITEM root = m_tree.InsertItem(group->name, 1, 1);
   d = new GroupOrLayer;
   d->IsAGroup = TRUE;
   d->group = group;
   m_tree.SetItemData(root, (DWORD)d);
}

void LayerGroupsCreate::OnAdd() 
{
   GroupOrLayer *add, *addTo;
   
   // get selected from list
   {
      int count = m_list.GetItemCount();

      if (!count)    
      {  ErrorMessage("No Layers", "Nothing Selected to Add"); return;  }

      BOOL Selected = FALSE;
		int selItem=0; 
      for (selItem=0; selItem < count; selItem++)
      {
         if (m_list.GetItemState(selItem, LVIS_SELECTED))
         {  
            Selected = TRUE;  
            break;   
         }
      }

      if (!Selected)    
      {  ErrorMessage("Select a group or layer from left box to add", "Nothing Selected to Add");  return;  }

      add = (GroupOrLayer*)m_list.GetItemData(selItem);
   }

   // get selected group from tree
   {
      HTREEITEM item = m_tree.GetSelectedItem();
      if (item == NULL) {  ErrorMessage("Select a group from right box to add item to", "No Group Selected");  return; } // nothing selected

      addTo = (GroupOrLayer*)m_tree.GetItemData(item); 
      if (!addTo->IsAGroup)   {  ErrorMessage("Select a group from right box to add item to", "No Group Selected");  return; } // nothing selected
   }  

   AddItemToGroup(addTo->group, add);
}

void LayerGroupsCreate::AddItemToGroup(LayerGroupStruct *group, GroupOrLayer *add) 
{
   // adding a group to a group
   if (add->IsAGroup)
   {
      if (add->group == group)   {  ErrorMessage(group->name, "Can Not add a group to itself"); return; } 

      // check that it's not already in there
      POSITION groupPos = group->GroupList.GetHeadPosition();
      while (groupPos != NULL)
      {
         LayerGroupStruct *tempGroup = group->GroupList.GetNext(groupPos);
         if (tempGroup == add->group)  {  ErrorMessage(tempGroup->name, "Group is already in this group");  return; } 
      }

      // add in Database
      group->GroupList.AddTail(add->group);

      // find group in tree
      HTREEITEM parent;
      parent = m_tree.GetRootItem();
      GroupOrLayer *treeGroup = (GroupOrLayer*)m_tree.GetItemData(parent);
      while (treeGroup->group != group)
      {
         HTREEITEM tempItem = parent;
         parent = m_tree.GetNextSiblingItem(tempItem);
         treeGroup = (GroupOrLayer*)m_tree.GetItemData(parent);
      }

      // insert in tree
      HTREEITEM item = m_tree.InsertItem(add->group->name, 1, 1, parent, TVI_FIRST);
      GroupOrLayer *d = new GroupOrLayer;
      d->IsAGroup = TRUE;
      d->group = add->group;
      m_tree.SetItemData(item, (DWORD)d);
      m_tree.Expand(parent, TVE_EXPAND);
   }

   // adding a layer to a group
   else
   {
      // check that it's not already in there
      POSITION layerPos = group->LayerList.GetHeadPosition();
      while (layerPos != NULL)
      {
         LayerStruct *layer = group->LayerList.GetNext(layerPos);
         if (layer == add->layer)   {  ErrorMessage(layer->getName(), "Layer is already in this group");   return; } 
      }

      // add in Database
      group->LayerList.AddTail(add->layer);

      // find group in tree
      HTREEITEM parent;
      parent = m_tree.GetRootItem();
      GroupOrLayer *treeGroup = (GroupOrLayer*)m_tree.GetItemData(parent);
      while (treeGroup->group != group)
      {
         HTREEITEM tempItem = parent;
         parent = m_tree.GetNextSiblingItem(tempItem);
         treeGroup = (GroupOrLayer*)m_tree.GetItemData(parent);
      }

      // insert in tree
      HTREEITEM item = m_tree.InsertItem(add->layer->getName(), 0, 0, parent, TVI_LAST);
      GroupOrLayer *d = new GroupOrLayer;
      d->IsAGroup = FALSE;
      d->layer = add->layer;
      m_tree.SetItemData(item, (DWORD)d);
      m_tree.Expand(parent, TVE_EXPAND);
   }
}

void LayerGroupsCreate::OnRemove() 
{
   // get selected item from tree
   HTREEITEM item = m_tree.GetSelectedItem();
   if (item == NULL) {  ErrorMessage("Select an item from right box to remove", "Nothing Selected");  return; } // nothing selected

   GroupOrLayer *remove = (GroupOrLayer*)m_tree.GetItemData(item); 
   HTREEITEM parent = m_tree.GetParentItem(item);

   // removing a leaf
   if (parent != NULL)
   {
      LayerGroupStruct *group = ((GroupOrLayer*)m_tree.GetItemData(parent))->group; 

      // if removing a group from a group
      if (remove->IsAGroup)
      {
         // find "remove" in the group's GroupList and remove it
         POSITION pos = group->GroupList.GetHeadPosition();
         while (pos != NULL)
         {
            LayerGroupStruct *subGroup = group->GroupList.GetAt(pos);
            if (subGroup == remove->group)
            {
               group->GroupList.RemoveAt(pos);
               break;
            }
            group->GroupList.GetNext(pos);
         }
      }

      // removing a layer from a group
      else
      {
         // find "remove" in the group's LayerList and remove it
         POSITION pos = group->LayerList.GetHeadPosition();
         while (pos != NULL)
         {
            LayerStruct *layer = group->LayerList.GetAt(pos);
            if (layer == remove->layer)
            {
               group->LayerList.RemoveAt(pos);
               break;
            }
            group->LayerList.GetNext(pos);
         }
      }

      delete remove; // free data memory for selected item in tree
      m_tree.DeleteItem(item); // remove item from tree
   }

   // removing a whole group
   else
   {
      DeleteAGroup(remove->group);
      delete remove;
   }
}

void LayerGroupsCreate::DeleteAGroup(LayerGroupStruct *remove) 
{
   // remove from list
   int count = m_list.GetItemCount();
   for (int index=0; index < count; index++)
   {
      GroupOrLayer *listItem = (GroupOrLayer*)m_list.GetItemData(index);
      if (listItem->IsAGroup && listItem->group == remove)
      {  
         m_list.DeleteItem(index);
         delete listItem;
         m_list.Arrange(LVA_DEFAULT);
         break;   
      }
   }

   // remove from tree
   HTREEITEM rootItem = m_tree.GetRootItem();
   while (rootItem != NULL)
   {
      GroupOrLayer *root = (GroupOrLayer*)m_tree.GetItemData(rootItem);

      // remove this whole group
      if (root->group == remove)
      {
         // free all its children leaves
         HTREEITEM childItem = m_tree.GetChildItem(rootItem);
         while (childItem != NULL)
         {
            delete (GroupOrLayer*)m_tree.GetItemData(childItem); 
            HTREEITEM temp = childItem;
            childItem = m_tree.GetNextSiblingItem(temp);
         }
   
         HTREEITEM temp = rootItem;
         rootItem = m_tree.GetNextSiblingItem(temp);
         m_tree.DeleteItem(temp); // remove item from tree
      }

      // check this group for references
      else
      {
         HTREEITEM childItem = m_tree.GetChildItem(rootItem);
         while (childItem != NULL)
         {
            GroupOrLayer *child = (GroupOrLayer*)m_tree.GetItemData(childItem); 
            if (child->IsAGroup && child->group == remove)
            {
               m_tree.DeleteItem(childItem); // remove item from tree
               delete child;
               break;
            }
            HTREEITEM temp = childItem;
            childItem = m_tree.GetNextSiblingItem(temp);
         }

         HTREEITEM temp = rootItem;
         rootItem = m_tree.GetNextSiblingItem(temp);
      }
   }

   // remove from database
   POSITION groupPos = doc->LayerGroupList.GetHeadPosition();
   while (groupPos != NULL)
   {
      LayerGroupStruct *group = doc->LayerGroupList.GetAt(groupPos);

      // remove this while group
      if (group == remove)
      {
         POSITION temp = groupPos;
         doc->LayerGroupList.GetNext(groupPos);
         doc->LayerGroupList.RemoveAt(temp);
         delete group;
      }

      // check this group for references
      else
      {
         POSITION subgroupPos = group->GroupList.GetHeadPosition();
         while (subgroupPos != NULL)
         {
            LayerGroupStruct *subgroup = group->GroupList.GetAt(subgroupPos);

            if (subgroup == remove)
            {
               group->GroupList.RemoveAt(subgroupPos);
               break;
            }

            group->GroupList.GetNext(subgroupPos);
         }

         doc->LayerGroupList.GetNext(groupPos);
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
// LayerGroupsUse dialog
void CCEtoODBDoc::OnEditLayerGroups() 
{
   LayerGroupsUse dlg;
   dlg.doc = this;
   dlg.DoModal();
   OnRegen();
   UpdateAllViews(NULL);
}

LayerGroupsUse::LayerGroupsUse(CWnd* pParent /*=NULL*/)
   : CDialog(LayerGroupsUse::IDD, pParent)
{
   //{{AFX_DATA_INIT(LayerGroupsUse)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void LayerGroupsUse::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(LayerGroupsUse)
   DDX_Control(pDX, IDC_LIST1, m_list);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LayerGroupsUse, CDialog)
   //{{AFX_MSG_MAP(LayerGroupsUse)
   ON_BN_CLICKED(IDC_ON, OnTurnOn)
   ON_BN_CLICKED(IDC_OFF, OnTurnOff)
   ON_BN_CLICKED(IDC_COLOR, OnChangeColor)
   ON_BN_CLICKED(IDC_APPLY, OnApply)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayerGroupsUse message handlers
BOOL LayerGroupsUse::OnInitDialog() 
{
   CDialog::OnInitDialog();

   POSITION pos = doc->LayerGroupList.GetHeadPosition();
   while (pos != NULL)
   {
      LayerGroupStruct *group = doc->LayerGroupList.GetNext(pos);

      m_list.SetItemDataPtr(m_list.AddString(group->name), group);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void LayerGroupsUse::OnTurnOn() 
{
   int index = m_list.GetCurSel();

   if (index == LB_ERR)
      return;

   doc->LayerGroup_Show((LayerGroupStruct*)m_list.GetItemDataPtr(index), TRUE);
}

void LayerGroupsUse::OnTurnOff() 
{
   int index = m_list.GetCurSel();

   if (index == LB_ERR)
      return;

   doc->LayerGroup_Show((LayerGroupStruct*)m_list.GetItemDataPtr(index), FALSE);
}

void LayerGroupsUse::OnChangeColor() 
{
   int index = m_list.GetCurSel();

   if (index == LB_ERR)
      return;

   CPersistantColorDialog dialog;
   if (dialog.DoModal() != IDOK)
      return;

   doc->LayerGroup_Color((LayerGroupStruct*)m_list.GetItemDataPtr(index), dialog.GetColor());
}

void LayerGroupsUse::OnApply() 
{
   doc->OnRegen();
   doc->UpdateAllViews(NULL); 
}
