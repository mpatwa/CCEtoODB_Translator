// $Header: /CAMCAD/5.0/Layer.cpp 56    3/12/07 12:41p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#include "StdAfx.h"
#include "Layer.h"
#include "CcDoc.h"
#include "Colors.h"

void SetColorsetButtons(CCEtoODBDoc *doc); // from LYRTYPE.CPP

//_____________________________________________________________________________
LayerStruct* CCEtoODBDoc::getDefinedFloatingLayer()
{
   LayerStruct* layerStruct = &(getLayerArray().getDefinedFloatingLayer());

   return layerStruct;
}

/******************************************************************************
* FindLayer
*/
LayerStruct* CCEtoODBDoc::getLayer(int layerIndex) const
{
   LayerStruct* foundLayer = NULL;

   if (layerIndex >= 0 && layerIndex < getMaxLayerIndex())
   {
      foundLayer = getLayerArray()[layerIndex];
   }

   return foundLayer;
}

/******************************************************************************
* FindLayer_by_Name
*/
LayerStruct* CCEtoODBDoc::getLayer(const CString& layerName) const
{
   LayerStruct* foundLayer = NULL;

   for (int i=0;i < getMaxLayerIndex();i++)
   {
      LayerStruct* layer = getLayerArray()[i];

      if (layer == NULL)
      {
         continue;
      }

      if (layerName.Compare(layer->getName()) == 0)
      {
         foundLayer = layer;
         break;
      }
   }

   return foundLayer;
}

LayerStruct* CCEtoODBDoc::getLayerNoCase(const CString& layerName) const
{
   LayerStruct* foundLayer = NULL;

   for (int i=0;i < getMaxLayerIndex();i++)
   {
      LayerStruct* layer = getLayerArray()[i];

      if (layer == NULL)
      {
         continue;
      }

      if (layerName.CompareNoCase(layer->getName()) == 0)
      {
         foundLayer = layer;
         break;
      }
   }

   return foundLayer;
}

LayerStruct& CCEtoODBDoc::getDefinedLayer(const CString& layerName)
{
   LayerStruct* foundLayer = getLayer(layerName);

   if (foundLayer == NULL)
   {
      foundLayer = AddNewLayer(layerName);
   }

   return *foundLayer;
}

LayerStruct& CCEtoODBDoc::getDefinedLayerNoCase(const CString& layerName)
{
   LayerStruct* foundLayer = getLayerNoCase(layerName);

   if (foundLayer == NULL)
   {
      foundLayer = AddNewLayer(layerName);
   }

   return *foundLayer;
}

/******************************************************************************
* IsFloatingLayer
*/
BOOL CCEtoODBDoc::IsFloatingLayer(int layernum)
{
   if (layernum >= getMaxLayerIndex() || layernum < 0)
      return TRUE;

   LayerStruct *layer = getLayerArray()[layernum];

   if (!layer)
      return TRUE;

   return layer->isFloating();
}

/******************************************************************************
* IsNegativeLayer
*/
BOOL CCEtoODBDoc::IsNegativeLayer(int layernum)
{
   if (layernum >= getMaxLayerIndex() || layernum < 0)
      return TRUE;

   LayerStruct *layer = getLayerArray()[layernum];

   if (!layer)
      return TRUE;

   return layer->isNegative();
}

int CCEtoODBDoc::getMaxElectricalLayerNumber() const
{
   int maxElectricalLayer = -1;

   for (int layerIndex = 0;layerIndex < getMaxLayerIndex();layerIndex++)
   {
      LayerStruct* layer = getLayer(layerIndex);

      if (layer != NULL && layer->getElectricalStackNumber() > maxElectricalLayer)
      {
         maxElectricalLayer = layer->getElectricalStackNumber();
      }
   }

   return maxElectricalLayer;
}

/****************************************************************************
* get_layer_visible
*
*  - PARAMETERS
*     int mirror  (if mirror == -1, ignore mirror)
*/
bool CCEtoODBDoc::get_layer_visible(int layernum, int mirror)
{
   if (layernum >= getMaxLayerIndex() || layernum < 0)
      return false;

   LayerStruct *layer = getLayerArray()[layernum];

   if (layer == NULL) 
      return false;

   if (mirror > -1)  // ignore mirror layers
   {
      if (mirror & MIRROR_LAYERS)
      {
         layer = getLayerArray()[getLayerArray()[layernum]->getMirroredLayerIndex()];
      
         if (layer == NULL || (!GeometryEditing && layer->getNeverMirror()))
            return false;
      }
      else 
      {
         layer = getLayerArray()[layernum];

         if (layer == NULL || (!GeometryEditing && layer->getMirrorOnly()))
            return false;
      }
   }

   return layer->isVisible();
}

bool CCEtoODBDoc::isLayerVisible(int layerIndex,bool mirrorLayersFlag)
{
   bool retval = false;

   if (layerIndex >= 0 && layerIndex < getMaxLayerIndex())
   {
      LayerStruct* layer = getLayerArray()[layerIndex];

      if (layer != NULL)
      {
         if (mirrorLayersFlag)
         {
            layer = getLayerArray()[layer->getMirroredLayerIndex()];
         
            if (!GeometryEditing && layer != NULL && layer->getNeverMirror())
            {
               layer = NULL;
            }
         }
         else if (!GeometryEditing && layer->getMirrorOnly())
         {
            layer = NULL;
         }
      }

      if (layer != NULL)
      {
         retval = layer->isVisible();
      }
   }

   return retval;
}

/****************************************************************************
* get_layer_editable
*
*  - PARAMETERS
*     int mirror  (if mirror == -1, ignore mirror)
*/
BOOL CCEtoODBDoc::get_layer_editable(int layernum, int mirror)
{
   if (layernum >= getMaxLayerIndex() || layernum < 0)
      return FALSE;

   LayerStruct *layer = getLayerArray()[layernum];

   if (layer == NULL)
      return FALSE;

   if (mirror > -1)  // ignore mirror layers
   {
      if (!GeometryEditing)
      {
         if (mirror & MIRROR_LAYERS)
         {
            layer = getLayerArray()[getLayerArray()[layernum]->getMirroredLayerIndex()];

            if (layer == NULL || layer->getNeverMirror())
               return FALSE;
         }
         else 
         {
            layer = getLayerArray()[layernum];

            if (layer == NULL || layer->getMirrorOnly())
               return FALSE;
         }
      }
   }

   return (layer->isEditable() && layer->isVisible());
}

/****************************************************************************
* get_layer_visible_extents
*/
BOOL CCEtoODBDoc::get_layer_visible_extents(int layernum)
{
   if (layernum >= getMaxLayerIndex() || layernum < 0)
      return FALSE;

   LayerStruct *layer = getLayerArray()[layernum];

   if (!layer)
      return FALSE;

   if (layer->isVisible())
      return TRUE;

   if (getLayerArray()[layer->getMirroredLayerIndex()]->isVisible())
      return TRUE;

   return FALSE;
}

/******************************************************************************
* get_layer_mirror
*  - returns mirrored layernum 
*/
int CCEtoODBDoc::get_layer_mirror(int layerNum, int mirror)
{
   if (layerNum >= getMaxLayerIndex() || layerNum < 0)
      return FALSE;

   if (mirror & MIRROR_LAYERS)
   {
      LayerStruct *layer = getLayerArray()[layerNum];

      if (!layer)
         return layerNum;

      return getLayerArray()[layerNum]->getMirroredLayerIndex();
   }

   return layerNum;
}

int CCEtoODBDoc::getMirroredLayerIndex(int layerIndex,bool mirrorLayerFlag)
{
   int mirroredLayerIndex = layerIndex;

   if (mirrorLayerFlag && layerIndex >= 0 && layerIndex < getMaxLayerIndex())
   {
      LayerStruct* layer = getLayerAt(layerIndex);

      if (layer != NULL)
      {
         mirroredLayerIndex = layer->getMirroredLayerIndex();
      }
   }

   return mirroredLayerIndex;
}

/******************************************************************************
* get_layer_color
*/
unsigned long CCEtoODBDoc::get_layer_color(int layerNum, int mirror)
{
   if (layerNum >= getMaxLayerIndex() || layerNum < 0)
      return (RGB(255,0,0)); // return red if not found

   LayerStruct *layer = getLayerArray()[layerNum];

   if (!layer)
      return (RGB(255,0,0)); // return red if not found

   if (mirror & MIRROR_LAYERS)
   {
      layer = getLayerArray()[layer->getMirroredLayerIndex()];

      if (!layer)
         return (RGB(255,0,0)); // return red if not found
   }

   return layer->getColor();
}

/******************************************************************************
* Add_Layer
*  IN:  Layer Name
*  OUT: Layer Number
*  - Searches Layer List for this layer name
*  - Creates new Layer Node if not found
*/
LayerStruct* CCEtoODBDoc::Add_Layer(CString name)
{
   LayerStruct* layer;

   for (int layerIndex=0;;layerIndex++)
   {
      if (layerIndex >= getLayerArray().GetSize())
      {
         layer = AddNewLayer(name);
         break;
      }

      layer = getLayerArray().GetAt(layerIndex);

      if (layer != NULL && name.Compare(layer->getName()) == 0)
      {
         break;
      }
   }

   return layer;
}

LayerStruct* CCEtoODBDoc::getDefinedLayerAt(int layerIndex)
{
   LayerStruct* layerStruct = &(getLayerArray().getDefinedLayerAt(layerIndex));

   //if (layerIndex >= 0 && layerIndex < getLayerArray().GetSize())
   //{
   //   layerStruct = getLayerArray().GetAt(layerIndex);
   //}

   //if (layerStruct == NULL)
   //{
   //   CString layerName;
   //   layerName.Format("Layer %d",layerIndex);

   //   layerStruct = new LayerStruct(layerIndex,layerName);
   //   getLayerArray().SetAtGrow(layerIndex,layerStruct);
   //}

   return layerStruct;
}

/******************************************************************************
* AddNewLayer
*/
LayerStruct* CCEtoODBDoc::AddNewLayer(CString name)
{
   LayerStruct* layer = &(getLayerArray().getNewLayer(name));

   unsigned long RGB_color;

   switch ((this->colorCount++%6)+1) // every layer gets next color
   {
   case 1:
      RGB_color=RGB(255,0,0); // RED
      break;

   case 2:
      RGB_color=RGB(255,255,0); // YELLOW
      break;

   case 3:
      RGB_color=RGB(0,255,0); // GREEN
      break;

   case 4:
      RGB_color=RGB(0,255,255); // CYAN
      break;

   case 5:
      RGB_color=RGB(0,0,255); // BLUE
      break;

   case 6:
      RGB_color=RGB(255,0,255); // MAGENTA
      break;
   }

   layer->setColor(RGB_color);
   layer->setOriginalColor(RGB_color);

   return layer;
}

/******************************************************************************
* OnOriginalColors
*/
void CCEtoODBDoc::OnOriginalColors() 
{
   ColorSet = -1;
   SetColorsetButtons(this);

   for (int i=0; i<getMaxLayerIndex(); i++)
   {
      if (getLayerArray()[i] == NULL) continue;
      getLayerArray()[i]->setColor( getLayerArray()[i]->getOriginalColor());
      getLayerArray()[i]->setVisible( getLayerArray()[i]->getOriginalVisible());
      getLayerArray()[i]->setEditable( getLayerArray()[i]->getOriginalEditable());
   }

	FitPageKeepingZoom(getBottomView());
}

/******************************************************************************
* OnSetOriginalColors
*/
void CCEtoODBDoc::OnSetOriginalColors() 
{
   CString tmp;

   tmp = "Set Current Color to Original is not reversable!";
   tmp += "\n\nAre you sure?";
   if (ErrorMessage(tmp, "Set Color to Original", MB_YESNO | MB_DEFBUTTON2) == IDNO)
      return;

   for (int i=0; i<getMaxLayerIndex(); i++)
   {
      if (getLayerArray()[i] == NULL) continue;
      getLayerArray()[i]->setOriginalColor( getLayerArray()[i]->getColor());
      getLayerArray()[i]->setOriginalVisible( getLayerArray()[i]->isVisible());
      getLayerArray()[i]->setOriginalEditable( getLayerArray()[i]->isEditable());
   }
}

/******************************************************************************
* GetMaxElectricalStackNum
*/
int GetMaxElectricalStackNum(CCEtoODBDoc *doc)
{
	int maxElectricaStackNum = 0;
	for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {	
		LayerStruct *layer = doc->getLayerArray()[i];
		if (!layer)
			continue;

		if (layer->getElectricalStackNumber() > maxElectricaStackNum)
			maxElectricaStackNum = layer->getElectricalStackNumber();
	}

	return maxElectricaStackNum;
}

// end LAYER.CPP

