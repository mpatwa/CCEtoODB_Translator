// $Header: /CAMCAD/DcaLib/DcaLayer.h 8     6/15/07 7:39p Kurt Van Ness $

#if !defined(__DcaLayer_h__)
#define __DcaLayer_h__

#pragma once

#include "Dca.h"
#include "DcaContainer.h"
#include "DcaMaskBool.h"

// ATTR Defintions for Layers
// these mirror/nomirror flags are used to do what MENTOR and other CAD systems can do. This is how
// they defined different graphic, dependend if a component is mirrored or not.
#define LY_NEVERMIRROR                 0x00000001  // display this layer only if the entity is not mirrored
#define LY_MIRRORONLY                  0x00000002  // display this layer only if the entity is mirrored.
#define LY_TOPONLY                     0x00000004  // display this layer only if the entity is on top
#define LY_BOTTOMONLY                  0x00000008  // display this layer only if the entity is on bottom

class CLayerArray;
class CAttributes;
class CCamCadData;
class CAttribute;
class CWriteFormat;

enum LayerTypeTag;
enum ValueTypeTag;
enum AttributeUpdateMethodTag;

//_____________________________________________________________________________
enum CamCadLayerTag
{
   ccLayerBoardOutline           , ccLayerFloat                   , ccLayerDummyPads            ,
   ccLayerDrillHoles             ,
   ccLayerPadTop                 , ccLayerPadBottom               ,
   ccLayerPadTopPd               , ccLayerPadBottomPd             ,
   ccLayerPadTopPdM              , ccLayerPadBottomPdM            ,
   ccLayerPadInner               , ccLayerPadAll                  ,
   ccLayerPadFlash               , ccLayerPadClearance            ,
   ccLayerMaskTop                , ccLayerMaskBottom              ,
   ccLayerMaskTopPd              , ccLayerMaskBottomPd            ,
   ccLayerMaskTopPdM             , ccLayerMaskBottomPdM           ,
   ccLayerPasteTop               , ccLayerPasteBottom             ,
   ccLayerStencilTop             , ccLayerStencilBottom           ,
   ccLayerCentroidTop            , ccLayerCentroidBottom          ,
   ccLayerGluePointTop           , ccLayerGluePointBottom         ,

   ccLayerEducatorBoardOutline   ,
   ccLayerEducatorCentroidTop    , ccLayerEducatorCentroidBottom  ,
   ccLayerEducatorMatchedTop     , ccLayerEducatorMatchedBottom   ,
   ccLayerEducatorUnmatchedTop   , ccLayerEducatorUnmatchedBottom ,
   ccLayerEducatorSilkTop        , ccLayerEducatorSilkBottom      ,

   ccLayerSilkTop                , ccLayerSilkBottom              , ccLayerSilk                  ,
   ccLayerAssemblyTop            , ccLayerAssemblyBottom          , ccLayerAssembly              ,
   ccLayerPinAssemblyTop         , ccLayerPinAssemblyBottom       , ccLayerPinAssembly           ,
   ccLayerPlaceTop               , ccLayerPlaceBottom             , ccLayerPlace                 ,
   ccLayerMountTop               , ccLayerMountBottom             , ccLayerMount                 ,
   ccLayerLargeViaKeepOutTop     , ccLayerLargeViaKeepOutBottom   , ccLayerLargeViaKeepOut       ,
   ccLayerResistTop              , ccLayerResistBottom            , ccLayerResist                ,
   ccLayerResistInhibitTop       , ccLayerResistInhibitBottom     , ccLayerResistInhibit         ,

   ccLayerViaKeepOutN            , 
   ccLayerRouteKeepInN           , 
   ccLayerLineInhibitN           ,
   ccLayerCopperInhibitN         ,
   ccLayerNegativeCopperN        ,
   ccLayerNegativeCopperInhibitN ,

   ccLayerUndefined  // must always be last enumerated value 
};

//_____________________________________________________________________________
class LayerStruct : public CObject
{
private:
   CLayerArray& m_layerArray;

   static int        m_nextId;
   int               m_id;

   CString           m_name;
   CString           comment;

   short             m_layerIndex;
   short             m_mirroredLayerIndex;  // mirror layer index number

   COLORREF          color;
   COLORREF          originalColor;

   unsigned char     visible:1;
   unsigned char     editable:1;
   unsigned char     negative:1;    // this complete layer is negative
   bool              m_floatingFlag:1;
   unsigned char     worldView:1;
   unsigned char     subset:1;      // used for layerstack dialog
   unsigned char     used:1;
   unsigned char     originalVisible:1;
   unsigned char     originalEditable:1;
   
   short             layertype;     // see dbutil.h dbutil.cpp

   double            zheight;       // layer z height, normally starts 0 SIGNAL_BOT and adds all
                                    // physical layers to SIGNAL_TOP
                                    // is taken from CAD input and should be displayed in layer 
                                    // dialog.

   short             artworkstacknumber;     // how a artwork is build starts with toppater, topsilk, topmask, ...
   short             electricalstacknumber;  // electrical stacking of layers, start with 1 (Top) to n (Bottom)
   short             physicalstacknumber;    // physical manufacturing stacking of layers, 
                                             // signaltop / dialectric / signal_1 / dia / power_1 .... / solder
   
   unsigned long     attr;          // layer attributes

   CAttributes*      AttribMap;

public:
   LayerStruct(CLayerArray& layerArray,short layerIndex,const CString& layerName);
   ~LayerStruct();

public:
   // accessors
   CLayerArray& getLayerArray()              { return m_layerArray; }

   const CString& getName() const            { return m_name; }
   //CString& getNameRef()                     { return name; }
   void setName(const CString& layerName);

   const CString& getComment() const         { return comment; }
   void setComment(const CString& comment)   { this->comment = comment; }

   short getLayerIndex() const               { return m_layerIndex; }
   //void setLayerIndex(short layerIndex)      { num = layerIndex; }

   short getMirroredLayerIndex() const       { return m_mirroredLayerIndex; }
   void setMirroredLayerIndex(short layerIndex) { m_mirroredLayerIndex = layerIndex; }
   bool hasMirroredLayer() const             { return (m_layerIndex != m_mirroredLayerIndex); }

   unsigned long getLayerFlags() const       { return attr; }
   void setLayerFlags(unsigned long flags)   { attr = flags; }
   void setFlagBits(unsigned long mask)      { attr |= mask; }
   void clearFlagBits(unsigned long mask)    { attr &= ~mask; }

   // display this layer only if the entity is not mirrored
   bool getNeverMirror() const               { return ((attr & LY_NEVERMIRROR) != 0); }

   // display this layer only if the entity is mirrored.
   bool getMirrorOnly()  const               { return ((attr & LY_MIRRORONLY ) != 0); }

   // display this layer only if the entity is on top
   bool getTopOnly()     const               { return ((attr & LY_TOPONLY    ) != 0); }

   // display this layer only if the entity is on bottom
   bool getBottomOnly()  const               { return ((attr & LY_BOTTOMONLY ) != 0); }

   bool isVisible() const                    { return (visible != 0); }
   bool getVisible() const                   { return (visible != 0); }
   void setVisible(bool flag)                { visible = flag; }
   void setVisible(BOOL flag)                { visible = flag; }

   bool getOriginalVisible() const           { return (originalVisible != 0); }
   void setOriginalVisible(bool flag)        { originalVisible = flag; }
   void setOriginalVisible(BOOL flag)        { originalVisible = flag; }

   bool isEditable() const                   { return (editable != 0); }
   void setEditable(bool flag)               { editable = flag; }
   void setEditable(BOOL flag)               { editable = flag; }

   bool getOriginalEditable() const          { return (originalEditable != 0); }
   void setOriginalEditable(bool flag)       { originalEditable = flag; }
   void setOriginalEditable(BOOL flag)       { originalEditable = flag; }

   bool isNegative() const                   { return (negative != 0); }
   void setNegative(bool flag)               { negative = flag; }
   void setNegative(BOOL flag)               { negative = flag; }

   bool isFloating() const                   { return m_floatingFlag; }
   bool getFloating() const                  { return m_floatingFlag; }
   void setFloating(bool flag);
   void setFloating(BOOL flag)               { setFloating((bool)(flag != 0)); }

   bool isWorldView() const                  { return (worldView != 0); }
   void setWorldView(bool flag)              { worldView = flag; }
   void setWorldView(BOOL flag)              { worldView = flag; }

   bool isSubset() const                     { return (subset != 0); }
   void setSubset(bool flag)                 { subset = flag; }
   void setSubset(BOOL flag)                 { subset = flag; }

   bool isUsed() const                       { return (used != 0); }
   void setUsed(bool flag)                   { used = flag; }
   void setUsed(BOOL flag)                   { used = flag; }

   COLORREF getColor() const                 { return color; }
   void setColor(COLORREF color)             { this->color = color; }

   COLORREF getOriginalColor() const         { return originalColor; }
   void setOriginalColor(COLORREF color)     { originalColor = color; }

   double getZHeight() const                 { return zheight; }
   void setZHeight(double zHeight)           { zheight = zHeight; }

   short getType() const                     { return layertype; }
   LayerTypeTag getLayerType() const         { return (LayerTypeTag)layertype; }
   void setLayerType(LayerTypeTag layerType);
   void setLayerType(int layerType);

   short getArtworkStackNumber() const           { return artworkstacknumber; }
   void setArtworkStackNumber(short stackNumber) { artworkstacknumber = stackNumber; }

   short getElectricalStackNumber() const           { return electricalstacknumber; }
   void setElectricalStackNumber(short stackNumber) { electricalstacknumber = stackNumber; }

   short getPhysicalStackNumber() const           { return physicalstacknumber; }
   void setPhysicalStackNumber(short stackNumber) { physicalstacknumber = stackNumber; }

   CAttributes* getAttributes() const        { return AttribMap; }
   CAttributes*& getAttributesRef()          { return AttribMap; }
   CAttributes*& getDefinedAttributes();
   CAttributes& attributes();

   bool isElectricalAll();
	bool isElectricalTop();
	bool isElectricalInner();
	bool isElectricalBottom();
   bool isElectrical();

   bool isLayerTypeTopSurface();
   bool isLayerTypeBottomSurface();

   bool setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void* value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr);

   // operations
   bool mirrorWithLayer(LayerStruct& otherLayer);
   LayerStruct& getMirroredLayer();

	//void WriteXML(CWriteFormat& writeFormat, CCEtoODBDoc *doc);
   void dump(CWriteFormat& writeFormat,int depth);
};

//_____________________________________________________________________________
class CLayerArray
{
private:
   CTypedPtrArrayContainer<LayerStruct*> m_layerArray;
   mutable CTypedPtrMap<CMapStringToPtr,CString,LayerStruct*>* m_layerMap;
   mutable CTypedPtrMap<CMapStringToPtr,CString,LayerStruct*>* m_layerMapNoCase;
   mutable LayerStruct* m_firstFloatingLayer;
   int m_camCadLayerIndexes[ccLayerUndefined];
   CString m_camCadLayerNames[ccLayerUndefined];

public:
   CLayerArray();

   void empty();

   // array methods
   int GetSize() const;
   int GetCount() const;
   LayerStruct* GetAt(int index) const;
   LayerStruct* operator[](int index) const;

   LayerStruct* getLayer(const CString& layerName) const;
   LayerStruct* getLayer(const LayerTypeTag layerType) const;  // Get first layer in list with given type.
   LayerStruct* getLayerNoCase(const CString& layerName) const;
   LayerStruct& getDefinedLayer(const CString& layerName);
   LayerStruct& getDefinedLayer(CString layerName,bool floatingFlag,LayerTypeTag layerType);
   LayerStruct& getDefinedLayer(CamCadLayerTag layerTag);
   LayerStruct& getDefinedLayerNoCase(const CString& layerName);
   LayerStruct& getDefinedLayerAt(int layerIndex);
   LayerStruct& getNewLayer(const CString& layerName);
   bool removeLayer(int layerIndex);
   bool mirrorLayers(LayerStruct& layer0,LayerStruct& layer1) const;
   bool unmirrorLayer(LayerStruct& layer) const;
   bool containsLayer(LayerStruct& layer) const;

   LayerStruct* FindFirstVisLayerInStackupRange(int startPhyLayer, int endPhyLayer, bool layerMirror = 0);
   int FindFirstVisLayerIndxInStackupRange(int startPhyLayer, int endPhyLayer, bool layerMirror = 0);
   int GetPhysicalStackupMirror(int physicalStackupNumer);

   LayerStruct& getDefinedFloatingLayer();
	//void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
	//void MarkUnusedLayers(CCEtoODBDoc *doc);

   void initializeCamCadLayer(CamCadLayerTag layerTag);
   void invalidateLayerMap() const;
   void invalidateFirstFloatingLayer() const;
   void dump(CWriteFormat& writeFormat,int depth) const;

   int getLayerCount(const LayerTypeTag layerType); // Return count of layers that have this type.
   int getElectLayerCount(const int electLayerStackupNum);
   LayerStruct* getElectLayer(const int electLayerStackupNum) const;

   static LayerTypeTag getCamCadLayerType(CamCadLayerTag layerTag);
   static CamCadLayerTag getOppositeCamCadLayerTag(CamCadLayerTag layerTag);
   static unsigned long getCamCadLayerAttributes(CamCadLayerTag layerTag);
   static CString getPlacementDependantLayerNameSuffix();
   static CString getPlacementDependantMirroredLayerNameSuffix();


private:
   void validateLayerMap() const;
   LayerStruct* lookupLayer(const CString& layerName) const;
   LayerStruct* lookupLayerNoCase(const CString& layerName) const;
   void setAtMap(LayerStruct& layer) const;
   void initializeCamCadLayers();
};

//_____________________________________________________________________________
class CLayerList : public CTypedPtrList<CPtrList, LayerStruct*>
{
};

//_____________________________________________________________________________
class CLayerFilter : public CMaskBool
{
private:
   static int m_nextId;
   int m_id;

public:
   CLayerFilter(bool initialValue);
   CLayerFilter(const CLayerFilter& other);
   ~CLayerFilter();

   int getId() const;
};

//_____________________________________________________________________________
class CLayerFilterList : public CTypedPtrListContainer<CLayerFilter*>
{
};

//_____________________________________________________________________________
class CLayerFilterStack
{
private:
   CLayerFilterList m_layerFilterStack;

public:
   CLayerFilterStack();

   int push(CLayerFilter& layerFilter);
   int moveToTop(int layerFilterId);
   bool pop();
   bool remove(int layerFilterId);
   bool hasTop();
   const CLayerFilter* getTop();
   bool contains(int layerIndex);
};

#endif
