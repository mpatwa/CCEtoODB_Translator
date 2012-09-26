// $Header: /CAMCAD/4.4/read_wrt/Ipc_In.h 2     10/08/04 6:49p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__Ipc_In_h__)
#define __Ipc_In_h__

#pragma once

#include "TypedContainer.h"

#define  MAX_LAYERS     30

class CIpcComponents;
class CIpcPin;

CString getDrillName(int drillIndex);

//_____________________________________________________________________________
typedef struct
{
   double   x,y;
   int      mode;    // 7 = line
                     // 8 = circle i = center i+1.x = start i+1.y = end
                     //            i+2.x = radius, i+2.y = 1(counterclock) -1 (clock)
   int      lineend; // 0 = round
                     // 1 = square
   int      fill;    // 0 not
                     // 1 yes
} IPC_Poly;

typedef struct
{
   char  *name;
} IPC_TestBlock;

typedef struct
{
   CString  shortnet;
   CString  longnet;
}IPC_LongNet;
typedef CTypedPtrArray<CPtrArray, IPC_LongNet*> LongNetArray;

typedef struct
{
   char     form;
   double   sizeA;
   double   sizeB;
   double   offsetx, offsety;
   double   rotation;
}IPC_Padform;
typedef CArray<IPC_Padform, IPC_Padform&> CPadformArray;

typedef struct
{
   double d;
   int    toolindex;
}IPC_Drill;
typedef CArray<IPC_Drill, IPC_Drill&> CDrillArray;

typedef struct
{
   CString  compname;
   CString  pinname;
   double   x,y,rot;
   int      layerindex; // Graph_Level
   int      lnr;        // A00 .. A01 ..
   int      padstack_blocknum;
   int      psindex;    // index into IPC_Paddef
}IPC_Pin;
typedef CTypedPtrArray<CPtrArray, IPC_Pin*> CPinArray;

typedef struct
{
   int   toolindex;
   int   padstack[MAX_LAYERS];
   int   typ;           // 1 top, 3 inner, 2 bottom 4 drill
   int   mirror_used;   // to place mirrored components, sometimes a mirrored version of this
                        // padstack has to be created. index+paddefcnt
}IPC_Paddef;
typedef CArray<IPC_Paddef, IPC_Paddef&> CPaddefArray;

//_____________________________________________________________________________
enum IpcPadTypeTag
{
   ipcPadTypeCircle      =  0,
   ipcPadTypeSquare      =  1,
   ipcPadTypeCircleDonut =  2,
   ipcPadTypeSquareDonut =  3,
   ipcPadTypeSubroutine  =  4,
   ipcPadTypeFeature     =  5,
   ipcPadTypeUndefined   = -1
};

IpcPadTypeTag intToIpcPadType(int padType);

//_____________________________________________________________________________
class CLayer
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CString m_layerName;
   bool m_floatingFlag;
   LayerStruct* m_layer;

public:
   CLayer(CCEtoODBDoc& camCadDoc,const CString& layerName,bool floatingFlag=false);

   LayerStruct& getLayer();
};

//_____________________________________________________________________________
class CIpcPad
{
private:
   int m_index;

   IpcPadTypeTag m_padType;
   double m_sizeA;
   double m_sizeB;
   //double m_xOffset;
   //double m_yOffset;
   //double m_rotationRadians;
   BlockStruct* m_geometry;

public:
   CIpcPad(int index,IpcPadTypeTag padType,double sizeA,double sizeB/*,
      double xOffset=0.,double yOffset=0.,double rotationRadians=0.*/);

   int getIndex()      const { return m_index; }
   double getSizeA()   const { return m_sizeA; }
   double getSizeB()   const { return m_sizeB; }
   //double getXOffset() const { return m_xOffset; }
   //double getYOffset() const { return m_yOffset; }
   //double getRotationRadians() const { return m_rotationRadians; }

   BlockStruct* getGeometry() const { return m_geometry; }
   void setGeometry(BlockStruct* geometry) { m_geometry = geometry; }

   bool matches(const CIpcPad& other) const;
   CString getGeometryName() const;
};

//_____________________________________________________________________________
class CIpcPads
{
private:
   CTypedPtrArrayContainer<CIpcPad*> m_pads;

public:
   CIpcPads();

   CIpcPad* getAt(int index);
   int getCount();
   CIpcPad* add(IpcPadTypeTag m_padType,double sizeA,double sizeB/*,
      double xOffset=0.,double yOffset=0.,double rotationRadians=0.*/);
};

//_____________________________________________________________________________
class CIpcPadArray
{
private:
   CTypedPtrArray<CPtrArray,CIpcPad*> m_pads;

public:
   CIpcPadArray();
   CIpcPadArray(const CIpcPadArray& other);

   int getCount();
   void empty();
   CIpcPad* getAt(int index);
   void setAt(int index,CIpcPad* pad);
   void add(CIpcPad* pad);
   bool matches(const CIpcPadArray& other);
};

//_____________________________________________________________________________
class CIpcPadstack
{
private:
   int m_index;
   bool m_mirrored;
   CIpcPadArray m_padArray;
   int m_drillIndex;
   BlockStruct* m_geometry;
   BlockStruct* m_mirroredGeometry;

public:
   CIpcPadstack(int index,int drillIndex,CIpcPadArray& padArray);

   int getIndex() const { return m_index; }
   BlockStruct* getGeometry(bool mirrorFlag);
   CString getGeometryName(bool mirrorFlag);
   bool matches(const CIpcPadstack& other);
};

//_____________________________________________________________________________
class CIpcPadstacks
{
private:
   CTypedPtrArrayContainer<CIpcPadstack*> m_padstacks;

public:
   CIpcPadstacks();

   int getCount();
   CIpcPadstack* instantiate(CIpcPin& pin,bool mirroredFlag);
   CIpcPadstack* add(int drillIndex,CIpcPadArray& padArray);
};

//_____________________________________________________________________________
class CIpcPin
{
private:
   CString  m_refDes;
   CString  m_pinName;
   CPoint2d m_origin;
   double   m_rotationRadians;
   //int      m_layerIndex;
   //int      m_layerNumber;
   //int      m_padstackBlockNumber;
   //int      m_padDefIndex;
   CompPinStruct* m_compPin;
   bool m_topFlag;

   CIpcPadstack& m_padstack;

public:
   CIpcPin(const CString& refDes,const CString& pinName,
      double x,double y,double rotationRadians,bool topFlag,CIpcPadstack& padstack);
      //int layerIndex,int layerNumber,int padstackBlockNumber,
      //int padDefIndex);

   CString  getRefDes() const { return m_refDes; }
   CString  getPinName() const { return m_pinName; }
   CPoint2d getOrigin()  const { return m_origin; }
   double   getRotationRadians() const { return m_rotationRadians; }
   bool     getTopFlag() const { return m_topFlag; }
   //int      getLayerIndex() const { return m_layerIndex; }
   //int      getLayerNumber() const { return m_layerNumber; }
   //int      getPadstackBlockNumber() const { return m_padstackBlockNumber; }
   //int      getPadDefIndex() const { return m_padDefIndex; }

   CIpcPadstack& getPadstack() const { return m_padstack ; }
   //void setPadstack(CIpcPadstack* padstack) { m_padstack = padstack; }
};

//_____________________________________________________________________________
class CIpcPins
{
private:
   CTypedPtrArrayContainer<CIpcPin*> m_pins;
   CTypedPtrMap<CMapStringToPtr,CString,CIpcPin*> m_pinMap;

public:
   CIpcPins();

   CIpcPin& addPin(const CString& refDes,const CString& pinName,
      double x,double y,double rotationRadians,bool topFlag,CIpcPadstack& padstack);
      //int layerIndex,int layerNumber,int padstackBlockNumber,
      //int padDefIndex);
   CIpcPin* getAt(int pinIndex);
   CIpcPin* getAt(const CString& pinName);
   int getCount() const;
};

//_____________________________________________________________________________
class CIpcComponent
{
private:
   int m_index;
   CString m_refDes;
   bool m_processedFlag;
   CIpcPins m_pins;
   BlockStruct* m_geometry;

public:
   CIpcComponent(int m_index,const CString& refDes);

   int getIndex()          const { return m_index; }
   CString getRefDes()     const { return m_refDes; }
   bool getProcessedFlag() const { return m_processedFlag; }
   BlockStruct* getGeometry() const { return m_geometry; }

   CIpcPin& addPin(const CString& refDes,const CString& pinName,
      double x,double y,double rotationRadians,bool topFlag,CIpcPadstack& padstack);

   bool process(CIpcComponents& components);
   bool matches(const CIpcComponent& other);
};

//_____________________________________________________________________________
class CIpcComponents
{
private:
   CTypedPtrArrayContainer<CIpcComponent*> m_componentArray;
   CTypedPtrMap<CMapStringToPtr,CString,CIpcComponent*> m_componentMap;

public:
   CIpcComponents();

   int getCount() const;
   CIpcComponent& getAt(const CString& refDes);
   bool findGeometry(BlockStruct*& componentGeometry,CIpcComponent& component);
   CIpcPin& addPin(const CString& refDes,const CString& pinName,
      double x,double y,double rotationRadians,bool topFlag,CIpcPadstack& padstack);
   bool process();
};

//_____________________________________________________________________________
class CIpcReaderData
{
private:
   static CIpcReaderData* m_ipcReaderData;

   CCEtoODBDoc&    m_camCadDoc;
   CIpcComponents m_components;
   CIpcPadstacks  m_padstacks;
   CIpcPads       m_pads;
   CLayer         m_floatingLayer;    // "0"
   CLayer         m_allLayer;         // "A00"
   CLayer         m_drillLayer;       // "DRILLHOLE"
   CLayer         m_topOutlineLayer;  // "OUTLINE_TOP"
   CLayer         m_topRefNameLayer;  // "REFNAME_TOP"
   CTypedPtrArrayContainer<CLayer*> m_ipcLayers;

public:
   CIpcReaderData(CCEtoODBDoc& camCadDoc);

   static CIpcReaderData& getIpcReaderData();
   static void setIpcReaderData(CIpcReaderData* ipcReaderData);
   static void releaseData();

   CIpcComponents& getComponents() { return m_components; }
   CIpcPadstacks&  getPadstacks()  { return m_padstacks;  }
   CIpcPads&       getPads()       { return m_pads;       }

   CLayer& getFloatingLayer()   { return m_floatingLayer; }
   CLayer& getAllLayer()        { return m_allLayer; }
   CLayer& getDrillLayer()      { return m_drillLayer; }
   CLayer& getTopOutlineLayer() { return m_topOutlineLayer; }
   CLayer& getTopRefNameLayer() { return m_topRefNameLayer; }
   CLayer& getIpcLayer(int ipcLayerNumber);

};


#endif
