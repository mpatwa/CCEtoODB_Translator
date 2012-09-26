// $Header: /CAMCAD/DcaLib/DcaLayer.cpp 6     6/15/07 7:39p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaLayer.h"
#include "DcaColors.h"
#include "DcaLayerType.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"
#include "DcaWriteFormat.h"
#include "DcaLib.h"

//_____________________________________________________________________________
int LayerStruct::m_nextId = 0;

LayerStruct::LayerStruct(CLayerArray& layerArray,short layerIndex,const CString& layerName)
: m_layerArray(layerArray)
{
   m_id   = m_nextId++;

   m_layerIndex         = layerIndex;
   m_mirroredLayerIndex = m_layerIndex;

   setName(layerName);

   COLORREF layerColor = colorRed; // init just to make Klocwork happy

   switch (layerIndex % 6)
   {
   case 0:  layerColor = colorRed;      break;
   case 1:  layerColor = colorYellow;   break;
   case 2:  layerColor = colorGreen;    break;
   case 3:  layerColor = colorCyan;     break;
   case 4:  layerColor = colorBlue;     break;
   case 5:  layerColor = colorMagenta;  break;
   }

   color    = layerColor;
   visible  = TRUE;
   editable = TRUE;
   originalColor    = layerColor;
   originalVisible  = TRUE;
   originalEditable = TRUE;

   m_floatingFlag = false;
   negative = FALSE;

   attr      = 0;
   layertype = 0;
   worldView = TRUE;
   artworkstacknumber    = 0;
   electricalstacknumber = 0;
   physicalstacknumber   = 0;
   zheight = 0.0;

   AttribMap = NULL;
}

LayerStruct::~LayerStruct()
{
}

void LayerStruct::setName(const CString& layerName)
{
   m_name = layerName;

   m_layerArray.invalidateLayerMap();
}

void LayerStruct::setFloating(bool flag)
{
   if (flag != m_floatingFlag)
   {
      m_layerArray.invalidateFirstFloatingLayer();

      m_floatingFlag = flag;
   }
}

void LayerStruct::setLayerType(LayerTypeTag layerType)
{
   layertype = layerType;
}

void LayerStruct::setLayerType(int layerType)
{
   if (layerType >= layerTypeLowerBound && layerType <= layerTypeUpperBound)
   {
      layertype = layerType;
   }
   else
   {
      layertype = layerTypeUndefined;
   }
}

CAttributes*& LayerStruct::getDefinedAttributes()
{
   if (AttribMap == NULL)
   {
      AttribMap = new CAttributes();
   }

   return AttribMap;
}

CAttributes& LayerStruct::attributes()
{
   if (AttribMap == NULL)
   {
      AttribMap = new CAttributes();
   }

   return *AttribMap;
}

bool LayerStruct::isElectricalAll()
{
	if (layertype == layerTypeSignalAll || layertype == layerTypeSignalOuter ||
		 layertype == layerTypePadAll		|| layertype == layerTypePadOuter)
      return true;
   else
      return false;
}

bool LayerStruct::isElectricalTop()
{
	if (layertype == layerTypeSignalAll || layertype == layerTypeSignalOuter	|| layertype == layerTypeSignalTop ||
		 layertype == layerTypePadAll		|| layertype == layerTypePadOuter		|| layertype == layerTypePadTop)
		return true;
	else
		return false;
}

bool LayerStruct::isElectricalInner()
{
	if (layertype == layerTypeSignalInner || layertype == layerTypePadInner)
		return true;
	else
		return false;
}

bool LayerStruct::isElectricalBottom()
{
	if (layertype == layerTypeSignalAll || layertype == layerTypeSignalOuter	|| layertype == layerTypeSignalBottom ||
		 layertype == layerTypePadAll		|| layertype == layerTypePadOuter		|| layertype == layerTypePadBottom)
		return true;
	else
		return false;
}

bool LayerStruct::isElectrical()
{
   return (isElectricalAll() || isElectricalTop() || isElectricalInner() || isElectricalBottom()); 
}

bool LayerStruct::isLayerTypeTopSurface()
{
   // Return true if pecifically a Top layer
   // "All" and "Outer" do not qualify.

   switch (this->layertype)
   {
   case layerTypeCentroidTop:
   case layerTypeComponentDftTop:
   case layerTypeDftTop:
   case layerTypeFluxTop:
   case layerTypeMaskTop:
   case layerTypePackageBodyTop:
   case layerTypePackagePinFootTop:
   case layerTypePackagePinLegTop:
   case layerTypePadTop:
   case layerTypePasteTop:
   case layerTypeSignalTop:
   case layerTypeSilkTop:
   case layerTypeStencilTop:
   case layerTypeTop:
      return true;
   }

   return false;
}
   
bool LayerStruct::isLayerTypeBottomSurface()
{
   // Return true if pecifically a Bottom layer
   // "All" and "Outer" do not qualify.

   switch (this->layertype)
   {
   case layerTypeCentroidBottom:
   case layerTypeComponentDftBottom:
   case layerTypeDftBottom:
   case layerTypeFluxBottom:
   case layerTypeMaskBottom:
   case layerTypePackageBodyBottom:
   case layerTypePackagePinFootBottom:
   case layerTypePackagePinLegBottom:
   case layerTypePadBottom:
   case layerTypePasteBottom:
   case layerTypeSignalBottom:
   case layerTypeSilkBottom:
   case layerTypeStencilBottom:
   case layerTypeBottom:
      return true;
   }

   return false;
}

bool LayerStruct::mirrorWithLayer(LayerStruct& otherLayer)
{
   return m_layerArray.mirrorLayers(*this,otherLayer);
}

LayerStruct& LayerStruct::getMirroredLayer()
{
   LayerStruct* mirroredLayer = m_layerArray.GetAt(m_mirroredLayerIndex);

   if (mirroredLayer == NULL)
   {
      mirroredLayer = this;
   }

   return *mirroredLayer;
}

bool LayerStruct::setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr)
{
   return (camCadData.setAttribute(attributes(), keywordIndex, valueType, value, updateMethod, attribPtr) ?  true : false); 
}

void LayerStruct::dump(CWriteFormat& writeFormat,int depth)
{
   depth--;

   writeFormat.writef(
"LayerStruct\n"
"{\n"
"   m_id=%d\n"
"   num=%d\n"
"   m_mirroredLayerIndex=%d\n"
"   name='%s'\n"
"   comment='%s'\n"
"   layertype='%s'\n"
"   artworkstacknumber=%d\n"
"   electricalstacknumber=%d\n"
"   physicalstacknumber=%d\n",
m_id,
m_layerIndex,
m_mirroredLayerIndex,
getName(),
comment,
layerTypeToString(intToLayerTypeTag(layertype)),
artworkstacknumber,
electricalstacknumber,
physicalstacknumber
);


   //if (depth != 0)
   //{
   //   writeFormat.pushHeader("   ");

   //   writeFormat.popHeader();
   //}

   writeFormat.writef("}\n");
}

//_____________________________________________________________________________
CLayerArray::CLayerArray()
: m_layerMap(NULL)
, m_layerMapNoCase(NULL)
, m_firstFloatingLayer(NULL)
{
   initializeCamCadLayers();
}

void CLayerArray::empty()
{
   invalidateLayerMap();

   m_layerArray.empty();
}

void CLayerArray::initializeCamCadLayers()
{
   m_camCadLayerNames[ccLayerBoardOutline           ] = "Board Outline";
   m_camCadLayerNames[ccLayerFloat                  ] = "Floating Layer";
   m_camCadLayerNames[ccLayerDrillHoles             ] = "Drill Holes";
   m_camCadLayerNames[ccLayerDummyPads              ] = "Dummy Pads";  
   m_camCadLayerNames[ccLayerPadTop                 ] = "Pad Layer Top";
   m_camCadLayerNames[ccLayerPadBottom              ] = "Pad Layer Bottom";
   m_camCadLayerNames[ccLayerPadTopPd               ] = CString("Pad Layer Top"   ) + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadBottomPd            ] = CString("Pad Layer Bottom") + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadTopPdM              ] = CString("Pad Layer Top"   ) + getPlacementDependantMirroredLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadBottomPdM           ] = CString("Pad Layer Bottom") + getPlacementDependantMirroredLayerNameSuffix();
   m_camCadLayerNames[ccLayerPadInner               ] = "Pad Layer Inner";
   m_camCadLayerNames[ccLayerPadAll                 ] = "Pad Layer All";
   m_camCadLayerNames[ccLayerPadFlash               ] = "Pad Flash";
   m_camCadLayerNames[ccLayerPadClearance           ] = "Pad Clearance";
   m_camCadLayerNames[ccLayerMaskTop                ] = "Solder Mask Top";
   m_camCadLayerNames[ccLayerMaskBottom             ] = "Solder Mask Bottom";
   m_camCadLayerNames[ccLayerMaskTopPd              ] = CString("Solder Mask Top"   ) + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerMaskBottomPd           ] = CString("Solder Mask Bottom") + getPlacementDependantLayerNameSuffix();
   m_camCadLayerNames[ccLayerMaskTopPdM             ] = CString("Solder Mask Top"   ) + getPlacementDependantMirroredLayerNameSuffix();
   m_camCadLayerNames[ccLayerMaskBottomPdM          ] = CString("Solder Mask Bottom") + getPlacementDependantMirroredLayerNameSuffix();

   m_camCadLayerNames[ccLayerSilkTop                ] = "Silkscreen Top";
   m_camCadLayerNames[ccLayerSilkBottom             ] = "Silkscreen Bottom";
   m_camCadLayerNames[ccLayerSilk                   ] = "Silkscreen";
   m_camCadLayerNames[ccLayerAssemblyTop            ] = "Assembly Top";
   m_camCadLayerNames[ccLayerAssemblyBottom         ] = "Assembly Bottom";
   m_camCadLayerNames[ccLayerAssembly               ] = "Assembly";
   m_camCadLayerNames[ccLayerPinAssemblyTop         ] = "Pin Assembly Top";
   m_camCadLayerNames[ccLayerPinAssemblyBottom      ] = "Pin Assembly Bottom";
   m_camCadLayerNames[ccLayerPinAssembly            ] = "Pin Assembly";
   m_camCadLayerNames[ccLayerPlaceTop               ] = "Place Top";
   m_camCadLayerNames[ccLayerPlaceBottom            ] = "Place Bottom";
   m_camCadLayerNames[ccLayerPlace                  ] = "Place";
   m_camCadLayerNames[ccLayerMountTop               ] = "Mount Top";
   m_camCadLayerNames[ccLayerMountBottom            ] = "Mount Bottom";
   m_camCadLayerNames[ccLayerMount                  ] = "Mount";
   m_camCadLayerNames[ccLayerLargeViaKeepOutTop     ] = "Large Via Keepout Top";
   m_camCadLayerNames[ccLayerLargeViaKeepOutBottom  ] = "Large Via Keepout Bottom";
   m_camCadLayerNames[ccLayerLargeViaKeepOut        ] = "Large Via Keepout";
   m_camCadLayerNames[ccLayerResistTop              ] = "Resist Top";
   m_camCadLayerNames[ccLayerResistBottom           ] = "Resist Bottom";
   m_camCadLayerNames[ccLayerResist                 ] = "Resist";
   m_camCadLayerNames[ccLayerResistInhibitTop       ] = "Resist Inhibit Top";
   m_camCadLayerNames[ccLayerResistInhibitBottom    ] = "Resist Inhibit Bottom";
   m_camCadLayerNames[ccLayerResistInhibit          ] = "Resist Inhibit";
   m_camCadLayerNames[ccLayerPasteTop               ] = "Solder Paste Top";
   m_camCadLayerNames[ccLayerPasteBottom            ] = "Solder Paste Bottom";
   m_camCadLayerNames[ccLayerStencilTop             ] = "Stencil Top";
   m_camCadLayerNames[ccLayerStencilBottom          ] = "Stencil Bottom";
   m_camCadLayerNames[ccLayerCentroidTop            ] = "Centroid Top";
   m_camCadLayerNames[ccLayerCentroidBottom         ] = "Centroid Bottom";
   m_camCadLayerNames[ccLayerGluePointTop           ] = "GLUEPOINT_TOP";
   m_camCadLayerNames[ccLayerGluePointBottom        ] = "GLUEPOINT_BOTTOM";
   m_camCadLayerNames[ccLayerEducatorBoardOutline   ] = "Educator Board Outline";
   m_camCadLayerNames[ccLayerEducatorCentroidTop    ] = "Educator Centroid Top";
   m_camCadLayerNames[ccLayerEducatorCentroidBottom ] = "Educator Centroid Bottom";
   m_camCadLayerNames[ccLayerEducatorMatchedTop     ] = "Educator Matched Top";
   m_camCadLayerNames[ccLayerEducatorMatchedBottom  ] = "Educator Matched Bottom";
   m_camCadLayerNames[ccLayerEducatorUnmatchedTop   ] = "Educator Unmatched Top";
   m_camCadLayerNames[ccLayerEducatorUnmatchedBottom] = "Educator Unmatched Bottom";
   m_camCadLayerNames[ccLayerEducatorSilkTop        ] = "Educator Silkscreen Top";
   m_camCadLayerNames[ccLayerEducatorSilkBottom     ] = "Educator Silkscreen Bottom";

   m_camCadLayerNames[ccLayerViaKeepOutN            ] = "Via Keepout";
   m_camCadLayerNames[ccLayerRouteKeepInN           ] = "Route Keepin";
   m_camCadLayerNames[ccLayerLineInhibitN           ] = "Line Inhibit";
   m_camCadLayerNames[ccLayerCopperInhibitN         ] = "Copper Inhibit";
   m_camCadLayerNames[ccLayerNegativeCopperN        ] = "Negative Copper";
   m_camCadLayerNames[ccLayerNegativeCopperInhibitN ] = "Negative Copper Inhibit";

   for (int ind = 0;ind < ccLayerUndefined;ind++)
   {
      m_camCadLayerIndexes[ind] = -1;
   }
}

int CLayerArray::GetSize() const
{
   return m_layerArray.GetSize();
}

int CLayerArray::GetCount() const
{
   return m_layerArray.GetCount();
}

LayerStruct* CLayerArray::GetAt(int index) const
{
   LayerStruct* layer = NULL;

   if (index >= 0 && index < m_layerArray.GetSize())
   {
      layer = m_layerArray.GetAt(index);
   }

   return layer;
}

LayerStruct* CLayerArray::operator[](int index) const
{
   return GetAt(index);
}

LayerStruct* CLayerArray::getLayer(const CString& layerName) const
{
   validateLayerMap();

   LayerStruct* layer = lookupLayer(layerName);

   return layer;
}

LayerStruct* CLayerArray::getLayerNoCase(const CString& layerName) const
{
   validateLayerMap();

   LayerStruct* layer = lookupLayerNoCase(layerName);

   return layer;
}

LayerStruct& CLayerArray::getDefinedFloatingLayer()
{
   if (m_firstFloatingLayer == NULL)
   {
      LayerStruct* firstFloatingLayer = NULL;

      for (int layerIndex = 0;layerIndex < GetSize();layerIndex++)
      {
         firstFloatingLayer = GetAt(layerIndex);

         if (firstFloatingLayer != NULL)
         {
            if (firstFloatingLayer->getFloating())
            {
               break;
            }

            firstFloatingLayer = NULL;
         }
      }

      if (firstFloatingLayer == NULL)
      {
         firstFloatingLayer = &(getDefinedLayer("Floating Layer"));
         firstFloatingLayer->setFloating(true);
      }

      m_firstFloatingLayer = firstFloatingLayer;
   }

   return *m_firstFloatingLayer;
}

LayerStruct& CLayerArray::getDefinedLayerAt(int layerIndex)
{
   LayerStruct* layer = NULL;

   if (layerIndex >= 0 && layerIndex < m_layerArray.GetSize())
   {
      layer = m_layerArray.GetAt(layerIndex);
   }

   if (layer == NULL)
   {
      CString layerName;
      layerName.Format("Layer %d",layerIndex);

      layer = new LayerStruct(*this,layerIndex,layerName);
      m_layerArray.SetAtGrow(layerIndex,layer);

      invalidateLayerMap();
   }

   return *layer;
}

LayerStruct& CLayerArray::getDefinedLayer(const CString& layerName)
{
   validateLayerMap();

   LayerStruct* layer = lookupLayer(layerName);

   if (layer == NULL)
   {
      int layerIndex = m_layerArray.GetSize();

      layer = new LayerStruct(*this,layerIndex,layerName);

      m_layerArray.SetAtGrow(layerIndex,layer);

      setAtMap(*layer);
   }

   return *layer;
}

LayerStruct& CLayerArray::getDefinedLayer(CString layerName,bool floatingFlag,LayerTypeTag layerType)
{
   layerName.Trim();

   if (layerName.IsEmpty())
   {
      layerName.Format("Layer %d",GetCount() + 1);
   }

   LayerStruct& layer = getDefinedLayer(layerName);

   if (floatingFlag)
   {
      layer.setFloating(true);
   }

   if (layerType != layerTypeUnknown && layer.getLayerType() == layerTypeUnknown)
   {
      layer.setLayerType(layerType);
   }

   return layer;
}

LayerStruct& CLayerArray::getDefinedLayer(CamCadLayerTag layerTag)
{
   if (layerTag == ccLayerUndefined)
   {
      layerTag = ccLayerDummyPads;
   }

   if (m_camCadLayerIndexes[layerTag] < 0)
   {
      initializeCamCadLayer(layerTag);
   }

   LayerStruct* layerStruct = GetAt(m_camCadLayerIndexes[layerTag]);

   return *layerStruct;
}

LayerStruct& CLayerArray::getDefinedLayerNoCase(const CString& layerName)
{
   validateLayerMap();

   LayerStruct* layer = lookupLayerNoCase(layerName);

   if (layer == NULL)
   {
      int layerIndex = m_layerArray.GetSize();

      layer = new LayerStruct(*this,layerIndex,layerName);

      m_layerArray.SetAtGrow(layerIndex,layer);

      setAtMap(*layer);
   }

   return *layer;
}

LayerStruct& CLayerArray::getNewLayer(const CString& layerName)
{
   LayerStruct& layer = getDefinedLayerAt(m_layerArray.GetSize());
   layer.setName(layerName);  // will invalidate map

   //invalidateLayerMap();

   return layer;
}

bool CLayerArray::mirrorLayers(LayerStruct& layer0,LayerStruct& layer1) const
{
   bool retval = (containsLayer(layer0) && containsLayer(layer1));

   if (retval)
   {
      if (layer0.getMirroredLayerIndex() != layer1.getLayerIndex() ||
          layer1.getMirroredLayerIndex() != layer0.getLayerIndex()     )
      {
         retval = true;
      {
         LayerStruct* mirroredLayer0 = GetAt(layer0.getMirroredLayerIndex());
         LayerStruct* mirroredLayer1 = GetAt(layer1.getMirroredLayerIndex());

         if (mirroredLayer0 != NULL)
         {
            unmirrorLayer(*mirroredLayer0);
         }

         if (mirroredLayer1 != NULL)
         {
            unmirrorLayer(*mirroredLayer1);
         }

         layer0.setMirroredLayerIndex(layer1.getLayerIndex());
         layer1.setMirroredLayerIndex(layer0.getLayerIndex());
         }
      }
   }

   return retval;
}

bool CLayerArray::unmirrorLayer(LayerStruct& layer) const
{
   bool retval = layer.hasMirroredLayer() && containsLayer(layer);

   if (layer.hasMirroredLayer())
   {
      LayerStruct* mirroredLayer = GetAt(layer.getMirroredLayerIndex());

      layer.setMirroredLayerIndex(layer.getLayerIndex());

      if (mirroredLayer != NULL && mirroredLayer->getMirroredLayerIndex() == layer.getLayerIndex())
      {
         mirroredLayer->setMirroredLayerIndex(mirroredLayer->getLayerIndex());
      }
   }

   return retval;
}

bool CLayerArray::containsLayer(LayerStruct& layer) const
{
   LayerStruct* containedLayer = GetAt(layer.getLayerIndex());

   bool retval = (&layer == containedLayer);

   return retval;
}

bool CLayerArray::removeLayer(int layerIndex)
{
   bool retval = false;

   if (layerIndex >= 0 && layerIndex < m_layerArray.GetSize())
   {
      LayerStruct* layer = m_layerArray.GetAt(layerIndex);
      
      if (layer != NULL)
      {
         unmirrorLayer(*layer);
         
         m_layerArray.SetAt(layerIndex,NULL);
         delete layer;

         if (layerIndex == m_layerArray.GetSize() - 1)
         {
            m_layerArray.RemoveAt(layerIndex);
         }

         invalidateLayerMap();

         retval = true;
      }
   }

   return retval;
}

LayerStruct* CLayerArray::lookupLayer(const CString& layerName) const
{
   LayerStruct* layer = NULL;

   validateLayerMap();

   if (! m_layerMap->Lookup(layerName,layer))
   {
      layer = NULL;
   }

   return layer; 
}

LayerStruct* CLayerArray::lookupLayerNoCase(const CString& layerName) const
{
   LayerStruct* layer = NULL;

   validateLayerMap();

   CString layerNameKey(layerName);
   layerNameKey.MakeLower();

   if (! m_layerMapNoCase->Lookup(layerNameKey,layer))
   {
      layer = NULL;
   }

   return layer; 
}

void CLayerArray::setAtMap(LayerStruct& layer) const
{
   if (m_layerMap != NULL && m_layerMapNoCase != NULL)
   {
      CString layerNameKey(layer.getName());

      layerNameKey.MakeLower();
      LayerStruct* existingLayer;

      if (! m_layerMapNoCase->Lookup(layerNameKey,existingLayer))
      {
         m_layerMapNoCase->SetAt(layerNameKey,&layer);
      }

      if (! m_layerMap->Lookup(layer.getName(),existingLayer))
      {
         m_layerMap->SetAt(layer.getName(),&layer);
      }
   }
}

void CLayerArray::validateLayerMap() const
{
   if (m_layerMap == NULL || m_layerMapNoCase == NULL)
   {
      delete m_layerMap;
      delete m_layerMapNoCase;

      m_layerMap       = new CTypedPtrMap<CMapStringToPtr,CString,LayerStruct*>();
      m_layerMapNoCase = new CTypedPtrMap<CMapStringToPtr,CString,LayerStruct*>();

      int mapSize = nextPrime2n(max(15,(6*m_layerArray.GetSize())/5));

      m_layerMap->InitHashTable(mapSize);
      m_layerMapNoCase->InitHashTable(mapSize);

      LayerStruct* existingLayer;

      for (int layerIndex = 0;layerIndex < m_layerArray.GetSize();layerIndex++)
      {
         LayerStruct* layer = m_layerArray.GetAt(layerIndex);

         if (layer != NULL)
         {
            CString layerNameKey(layer->getName());
            layerNameKey.MakeLower();

            if (! m_layerMapNoCase->Lookup(layerNameKey,existingLayer))
            {
               m_layerMapNoCase->SetAt(layerNameKey,layer);
            }

            if (! m_layerMap->Lookup(layer->getName(),existingLayer))
            {
               m_layerMap->SetAt(layer->getName(),layer);
            }
         }
      }
   }
}

void CLayerArray::invalidateLayerMap() const
{
   delete m_layerMap;
   m_layerMap = NULL;

   delete m_layerMapNoCase;
   m_layerMapNoCase = NULL;
}

void CLayerArray::invalidateFirstFloatingLayer() const
{
   m_firstFloatingLayer = NULL;
}

void CLayerArray::dump(CWriteFormat& writeFormat,int depth) const
{
   depth--;

   writeFormat.writef(
"CLayerArray\n"
"{\n"
"   Count=%d\n",
GetCount());

   if (depth != 0)
   {
      writeFormat.pushHeader("   ");

      for (int layerIndex = 0;layerIndex < GetSize();layerIndex++)
      {
         LayerStruct* layer = GetAt(layerIndex);

         if (layer != NULL)
         {
            layer->dump(writeFormat,depth);
         }
      }

      writeFormat.popHeader();
   }

   writeFormat.writef("}\n");
}

CString CLayerArray::getPlacementDependantLayerNameSuffix()
{
   return " (Placement Dependant)";
}

CString CLayerArray::getPlacementDependantMirroredLayerNameSuffix()
{
   return " (Placement Dependant) mirrored";
}

void CLayerArray::initializeCamCadLayer(CamCadLayerTag layerTag)
{
   if (layerTag != ccLayerUndefined)
   {
      LayerTypeTag layerType = getCamCadLayerType(layerTag);
      bool floatFlag = (layerTag == ccLayerFloat);

      int layerIndex = getDefinedLayer(m_camCadLayerNames[layerTag],floatFlag,layerType).getLayerIndex();
      m_camCadLayerIndexes[layerTag] = layerIndex;

      CamCadLayerTag oppositeLayerTag = getOppositeCamCadLayerTag(layerTag);

      if (oppositeLayerTag != ccLayerUndefined)
      {
         LayerTypeTag oppositeLayerType = getCamCadLayerType(oppositeLayerTag);
         int oppositeLayerIndex = getDefinedLayer(m_camCadLayerNames[oppositeLayerTag],false,oppositeLayerType).getLayerIndex();
         m_camCadLayerIndexes[oppositeLayerTag] = oppositeLayerIndex;

         LayerStruct* layer = GetAt(layerIndex);
         LayerStruct* oppositeLayer = GetAt(oppositeLayerIndex);

         if (layer != NULL && oppositeLayer != NULL)
         {
            switch (layerTag)
            {
            case ccLayerMaskTop:          
            case ccLayerMaskBottom:       
            case ccLayerPadTop:           
            case ccLayerPadBottom:        
            case ccLayerSilkTop:          
            case ccLayerSilkBottom:       
            case ccLayerAssemblyTop:   
            case ccLayerAssemblyBottom:
            case ccLayerPinAssemblyTop:   
            case ccLayerPinAssemblyBottom:
            case ccLayerEducatorCentroidTop:   
            case ccLayerEducatorCentroidBottom:
            case ccLayerGluePointTop:   
            case ccLayerGluePointBottom:
               //case ccLayerStencilTop:     // mirroring the stencil layers breaks the stencil generator - 20060425.1851 - knv
               //case ccLayerStencilBottom:
               layer->mirrorWithLayer(*oppositeLayer);
               break;
            }

            unsigned long attributes = getCamCadLayerAttributes(layerTag);

            if (attributes != 0)
            {
               layer->setFlagBits(attributes);
               oppositeLayer->setFlagBits(attributes);
            }
         }
      }
   }
}

LayerTypeTag CLayerArray::getCamCadLayerType(CamCadLayerTag layerTag)
{
   LayerTypeTag layerType = layerTypeUnknown;

   switch (layerTag)
   {
   case ccLayerBoardOutline:             layerType = layerTypeBoardOutline;           break;
   case ccLayerFloat:                    layerType = layerTypeUnknown;                break;
   case ccLayerDrillHoles:               layerType = layerTypeDrill;                  break;
   case ccLayerDummyPads:                layerType = layerTypeUnknown;                break;     
   case ccLayerPadTop:                   layerType = layerTypePadTop;                 break;
   case ccLayerPadBottom:                layerType = layerTypePadBottom;              break;
   case ccLayerPadTopPd:                 layerType = layerTypePadTop;                 break;
   case ccLayerPadBottomPd:              layerType = layerTypePadBottom;              break;
   case ccLayerPadTopPdM:                layerType = layerTypePadTop;                 break;
   case ccLayerPadBottomPdM:             layerType = layerTypePadBottom;              break;
   case ccLayerPadInner:                 layerType = layerTypePadInner;               break;
   case ccLayerPadAll:                   layerType = layerTypePadAll;                 break;
   case ccLayerPadFlash:                 layerType = layerTypeUnknown;                break;
   case ccLayerPadClearance:             layerType = layerTypePlaneClearance;         break;

   case ccLayerMaskTop:                  layerType = layerTypeMaskTop;                break;
   case ccLayerMaskBottom:               layerType = layerTypeMaskBottom;             break;
   case ccLayerGluePointTop:             layerType = layerTypeTop;                    break;
   case ccLayerGluePointBottom:          layerType = layerTypeBottom;                 break;
   case ccLayerMaskTopPd:                layerType = layerTypeMaskTop;                break;
   case ccLayerMaskBottomPd:             layerType = layerTypeMaskBottom;             break;
   case ccLayerMaskTopPdM:               layerType = layerTypeMaskTop;                break;
   case ccLayerMaskBottomPdM:            layerType = layerTypeMaskBottom;             break;
   case ccLayerSilkTop:                  layerType = layerTypeSilkTop;                break;
   case ccLayerSilkBottom:               layerType = layerTypeSilkBottom;             break;
   case ccLayerSilk:                     layerType = layerTypeUnknown;                break;
   case ccLayerAssemblyTop:              layerType = layerTypeComponentDftTop;        break;
   case ccLayerAssemblyBottom:           layerType = layerTypeComponentDftBottom;     break;
   case ccLayerAssembly:                 layerType = layerTypeUnknown;                break;
   case ccLayerPinAssemblyTop:           layerType = layerTypeTop;                    break;
   case ccLayerPinAssemblyBottom:        layerType = layerTypeBottom;                 break;
   case ccLayerPinAssembly:              layerType = layerTypeAll;                    break;
   case ccLayerPlaceTop:                 layerType = layerTypeTop;                    break;
   case ccLayerPlaceBottom:              layerType = layerTypeBottom;                 break;
   case ccLayerPlace:                    layerType = layerTypeUnknown;                break;
   case ccLayerMountTop:                 layerType = layerTypeTop;                    break;
   case ccLayerMountBottom:              layerType = layerTypeBottom;                 break;
   case ccLayerMount:                    layerType = layerTypeUnknown;                break;
   case ccLayerLargeViaKeepOutTop:       layerType = layerTypeTop;                    break;
   case ccLayerLargeViaKeepOutBottom:    layerType = layerTypeBottom;                 break;
   case ccLayerLargeViaKeepOut:          layerType = layerTypeUnknown;                break;
   case ccLayerResistTop:                layerType = layerTypeMaskTop;                break;
   case ccLayerResistBottom:             layerType = layerTypeMaskBottom;             break;
   case ccLayerPasteTop:                 layerType = layerTypePasteTop;               break;
   case ccLayerPasteBottom:              layerType = layerTypePasteBottom;            break;
   case ccLayerStencilTop:               layerType = layerTypeStencilTop;             break;
   case ccLayerStencilBottom:            layerType = layerTypeStencilBottom;          break;
   case ccLayerCentroidTop:              layerType = layerTypeCentroidTop;            break;
   case ccLayerCentroidBottom:           layerType = layerTypeCentroidBottom;         break;

   case ccLayerEducatorBoardOutline:     layerType = layerTypeBoardOutline;           break;
   case ccLayerEducatorCentroidTop:      layerType = layerTypeCentroidTop;            break;
   case ccLayerEducatorCentroidBottom:   layerType = layerTypeCentroidBottom;         break;
   case ccLayerEducatorMatchedTop:       layerType = layerTypeTop;                    break;
   case ccLayerEducatorMatchedBottom:    layerType = layerTypeBottom;                 break;
   case ccLayerEducatorUnmatchedTop:     layerType = layerTypeTop;                    break;
   case ccLayerEducatorUnmatchedBottom:  layerType = layerTypeBottom;                 break;
   case ccLayerEducatorSilkTop:          layerType = layerTypeSilkTop;                break;
   case ccLayerEducatorSilkBottom:       layerType = layerTypeSilkBottom;             break;

   case ccLayerResist:                   layerType = layerTypeUnknown;                break;
   case ccLayerResistInhibitTop:         layerType = layerTypeMaskTop;                break;
   case ccLayerResistInhibitBottom:      layerType = layerTypeMaskBottom;             break;
   case ccLayerResistInhibit:            layerType = layerTypeUnknown;                break;
   }

   return layerType;
}

int CLayerArray::getLayerCount(const LayerTypeTag layerType)
{
   // Return count of layers that have this type.
   int count = 0;

   for(int layerIdx = 0; layerIdx < GetCount(); layerIdx++)
   {
      LayerStruct *layer = GetAt(layerIdx);
      if (layer != NULL && layer->getLayerType() == layerType)
         count++;
   }

   return count;
}

LayerStruct* CLayerArray::getLayer(const LayerTypeTag layerType) const
{
   // Get first layer in list with given type.

   for(int layerIdx = 0; layerIdx < GetCount(); layerIdx++)
   {
      LayerStruct *layer = GetAt(layerIdx);
      if (layer != NULL && layer->getLayerType() == layerType)
         return layer;
   }

   return NULL;
}

int CLayerArray::getElectLayerCount(const int electLayerStackupNum)
{
   // Return count of layers with this elect stackup num.
   int count = 0;

   for(int layerIdx = 0; layerIdx < GetCount(); layerIdx++)
   {
      LayerStruct *layer = GetAt(layerIdx);
      if (layer != NULL && layer->getElectricalStackNumber() == electLayerStackupNum)
         count++;
   }

   return count;
}

LayerStruct* CLayerArray::getElectLayer(const int electLayerStackupNum) const
{
   // Get first layer in list with given electrical stackup number.

   for(int layerIdx = 0; layerIdx < GetCount(); layerIdx++)
   {
      LayerStruct *layer = GetAt(layerIdx);
      if (layer != NULL && layer->getElectricalStackNumber() == electLayerStackupNum)
         return layer;
   }

   return NULL;
}

CamCadLayerTag CLayerArray::getOppositeCamCadLayerTag(CamCadLayerTag layerTag)
{
   CamCadLayerTag oppositeLayerTag = ccLayerUndefined;

   switch (layerTag)
   {
   case ccLayerMaskTop:                  oppositeLayerTag = ccLayerMaskBottom;               break;
   case ccLayerMaskBottom:               oppositeLayerTag = ccLayerMaskTop;                  break;
   case ccLayerGluePointTop:             oppositeLayerTag = ccLayerGluePointBottom;          break;
   case ccLayerGluePointBottom:          oppositeLayerTag = ccLayerGluePointTop;             break;
   case ccLayerPadTop:                   oppositeLayerTag = ccLayerPadBottom;                break;
   case ccLayerPadBottom:                oppositeLayerTag = ccLayerPadTop;                   break;
   case ccLayerSilkTop:                  oppositeLayerTag = ccLayerSilkBottom;               break;
   case ccLayerSilkBottom:               oppositeLayerTag = ccLayerSilkTop;                  break;
   case ccLayerPadTopPd:                 oppositeLayerTag = ccLayerPadBottomPd;              break;
   case ccLayerPadBottomPd:              oppositeLayerTag = ccLayerPadTopPd;                 break;
   case ccLayerPadTopPdM:                oppositeLayerTag = ccLayerPadBottomPdM;             break;
   case ccLayerPadBottomPdM:             oppositeLayerTag = ccLayerPadTopPdM;                break;
   case ccLayerMaskTopPd:                oppositeLayerTag = ccLayerMaskBottomPd;             break;
   case ccLayerMaskBottomPd:             oppositeLayerTag = ccLayerMaskTopPd;                break;
   case ccLayerMaskTopPdM:               oppositeLayerTag = ccLayerMaskBottomPdM;            break;
   case ccLayerMaskBottomPdM:            oppositeLayerTag = ccLayerMaskTopPdM;               break;
   case ccLayerAssemblyTop:              oppositeLayerTag = ccLayerAssemblyBottom;           break;
   case ccLayerAssemblyBottom:           oppositeLayerTag = ccLayerAssemblyTop;              break;
   case ccLayerPinAssemblyTop:           oppositeLayerTag = ccLayerPinAssemblyBottom;        break;
   case ccLayerPinAssemblyBottom:        oppositeLayerTag = ccLayerPinAssemblyTop;           break;
   case ccLayerPlaceTop:                 oppositeLayerTag = ccLayerPlaceBottom;              break;
   case ccLayerPlaceBottom:              oppositeLayerTag = ccLayerPlaceTop;                 break;
   case ccLayerMountTop:                 oppositeLayerTag = ccLayerMountBottom;              break;
   case ccLayerMountBottom:              oppositeLayerTag = ccLayerMountTop;                 break;
   case ccLayerLargeViaKeepOutTop:       oppositeLayerTag = ccLayerLargeViaKeepOutBottom;    break;
   case ccLayerLargeViaKeepOutBottom:    oppositeLayerTag = ccLayerLargeViaKeepOutTop;       break;
   case ccLayerResistTop:                oppositeLayerTag = ccLayerResistBottom;             break;
   case ccLayerResistBottom:             oppositeLayerTag = ccLayerResistTop;                break;
   case ccLayerPasteTop:                 oppositeLayerTag = ccLayerPasteBottom;              break;
   case ccLayerPasteBottom:              oppositeLayerTag = ccLayerPasteTop;                 break;
   case ccLayerStencilTop:               oppositeLayerTag = ccLayerStencilBottom;            break;
   case ccLayerStencilBottom:            oppositeLayerTag = ccLayerStencilTop;               break;
   case ccLayerCentroidTop:              oppositeLayerTag = ccLayerCentroidBottom;           break;
   case ccLayerCentroidBottom:           oppositeLayerTag = ccLayerCentroidTop;              break;
   case ccLayerEducatorCentroidTop:      oppositeLayerTag = ccLayerEducatorCentroidBottom;   break;
   case ccLayerEducatorCentroidBottom:   oppositeLayerTag = ccLayerEducatorCentroidTop;      break;
   case ccLayerEducatorMatchedTop:       oppositeLayerTag = ccLayerEducatorMatchedBottom;    break;
   case ccLayerEducatorMatchedBottom:    oppositeLayerTag = ccLayerEducatorMatchedTop;       break;
   case ccLayerEducatorUnmatchedTop:     oppositeLayerTag = ccLayerEducatorUnmatchedBottom;  break;
   case ccLayerEducatorUnmatchedBottom:  oppositeLayerTag = ccLayerEducatorUnmatchedTop;     break;
   case ccLayerEducatorSilkTop:          oppositeLayerTag = ccLayerEducatorSilkBottom;       break;
   case ccLayerEducatorSilkBottom:       oppositeLayerTag = ccLayerEducatorSilkTop;          break;
   case ccLayerResistInhibitTop:         oppositeLayerTag = ccLayerResistInhibitBottom;      break;
   case ccLayerResistInhibitBottom:      oppositeLayerTag = ccLayerResistInhibitTop;         break;
   }

   return oppositeLayerTag;
}

unsigned long CLayerArray::getCamCadLayerAttributes(CamCadLayerTag layerTag)
{
   unsigned long attributes = 0;

   switch (layerTag)
   {
   case ccLayerPadTopPd:
   case ccLayerPadBottomPd:
   case ccLayerMaskTopPd:
   case ccLayerMaskBottomPd:
      attributes = LY_NEVERMIRROR;
      break;
   case ccLayerPadTopPdM:
   case ccLayerPadBottomPdM:
   case ccLayerMaskTopPdM:
   case ccLayerMaskBottomPdM:
      attributes = LY_MIRRORONLY;
      break;
   }

   return attributes;
}

LayerStruct* CLayerArray::FindFirstVisLayerInStackupRange(int startPhyLayer, int endPhyLayer, bool layerMirror)
{
   int testStart = min(startPhyLayer, endPhyLayer);
   int testEnd = max(startPhyLayer, endPhyLayer);
   LayerStruct *keeper = NULL;

   for(int layerIdx = 0; layerIdx < GetCount(); layerIdx++)
   {
      LayerStruct *layer = GetAt(layerIdx);
      if(layer && layer->getLayerType() != layerTypeStackLevelTop && layer->getLayerType() != layerTypeStackLevelBottom)
      {
         int phystackupNumber = layer->getPhysicalStackNumber();
         if (phystackupNumber >= testStart && phystackupNumber <= testEnd)
         {
            if (keeper == NULL || (abs(startPhyLayer - phystackupNumber) <  abs(endPhyLayer - phystackupNumber)))
            {
               if((!layerMirror && layer->isVisible()) || (layerMirror && layer->getMirroredLayer().getVisible()))
                  keeper = layer;              
            }           
         }
      }
   }

   return keeper;
}

int CLayerArray::FindFirstVisLayerIndxInStackupRange(int startPhyLayer, int endPhyLayer, bool layerMirror)
{
   LayerStruct* layer = FindFirstVisLayerInStackupRange(startPhyLayer,endPhyLayer, layerMirror);
   return (layer)?layer->getLayerIndex(): -1;
}

int CLayerArray::GetPhysicalStackupMirror(int physicalStackupNumer) 
{
   if(physicalStackupNumer < 1) return -1;
   
   //Find stackup number of mirror layer
   for(int layerIdx = 0; layerIdx < GetCount(); layerIdx++)
   {
      LayerStruct *layer = GetAt(layerIdx);
      if(layer && layer->getLayerType() != layerTypeStackLevelTop && layer->getLayerType() != layerTypeStackLevelBottom
         && layer->getPhysicalStackNumber() == physicalStackupNumer)
      {
         if(layer->hasMirroredLayer() && layer->getMirroredLayer().getPhysicalStackNumber() > 0) 
            return layer->getMirroredLayer().getPhysicalStackNumber();
      }
   }

   return -1;
}

//_____________________________________________________________________________
int CLayerFilter::m_nextId = 1;

CLayerFilter::CLayerFilter(bool initialValue)
: CMaskBool(initialValue)
{
   m_id = m_nextId++;
}

CLayerFilter::CLayerFilter(const CLayerFilter& other)
{
   CMaskBool::operator=(other);

   m_id = m_nextId++;
}

CLayerFilter::~CLayerFilter()
{
}

int CLayerFilter::getId() const
{
   return m_id;
}

//_____________________________________________________________________________
CLayerFilterStack::CLayerFilterStack()
{
}

int CLayerFilterStack::push(CLayerFilter& layerFilter)
{
   m_layerFilterStack.AddTail(new CLayerFilter(layerFilter));

   return m_layerFilterStack.GetCount();
}

int CLayerFilterStack::moveToTop(int layerFilterId)
{
   int retval = -1;

   for (POSITION pos = m_layerFilterStack.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CLayerFilter* layerFilter = m_layerFilterStack.GetNext(pos);

      if (layerFilter->getId() == layerFilterId)
      {
         m_layerFilterStack.RemoveAt(oldPos);
         m_layerFilterStack.AddTail(layerFilter);

         retval = m_layerFilterStack.GetCount();
         break;
      }
   }

   return retval;
}

bool CLayerFilterStack::remove(int layerFilterId)
{
   bool retval = false;

   for (POSITION pos = m_layerFilterStack.GetHeadPosition();pos != NULL;)
   {
      POSITION oldPos = pos;
      CLayerFilter* layerFilter = m_layerFilterStack.GetNext(pos);

      if (layerFilter->getId() == layerFilterId)
      {
         m_layerFilterStack.RemoveAt(oldPos);
         delete layerFilter;

         retval = true;
         break;
      }
   }

   return retval;
}

bool CLayerFilterStack::pop()
{
   bool retval = (m_layerFilterStack.GetCount() > 0);

   if (retval)
   {
      CLayerFilter* layerFilter = m_layerFilterStack.RemoveTail();
      delete layerFilter;
   }

   return retval;
}

bool CLayerFilterStack::hasTop()
{
   bool retval = (m_layerFilterStack.GetCount() > 0);

   return retval;
}

const CLayerFilter* CLayerFilterStack::getTop()
{
   CLayerFilter* layerFilter = NULL;

   if (m_layerFilterStack.GetCount() > 0)
   {
      layerFilter = m_layerFilterStack.GetTail();
   }

   return layerFilter;
}

bool CLayerFilterStack::contains(int layerIndex)
{
   bool retval = true;

   if (m_layerFilterStack.GetCount() > 0)
   {
      CLayerFilter* layerFilter = m_layerFilterStack.GetTail();

      retval = layerFilter->contains(layerIndex);
   }

   return retval;
}


