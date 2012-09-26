
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

struct IPCLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString Name;        // CAMCAD name
};
typedef CTypedPtrArray<CPtrArray, IPCLayerStruct*> CIPCLayerArray;


typedef struct
{
   int            block_num;
   CString        name;
   double         drill;
   int            typ; // 0x1 top, 0x2 bottom, 0x4 drill
   int            shapetypetop;
   int            shapetypebot;
   double         xsizetop, ysizetop, xofftop, yofftop, toprotation;
   double         xsizebot, ysizebot, xoffbot, yoffbot, botrotation;
}IPCPadstack;
typedef CArray<IPCPadstack, IPCPadstack&> CIPCPadstackArray;

class IPCVia
{
public:
   CString  m_netname;
   DataStruct* m_data;

   IPCVia(CString netname, DataStruct *data) { m_netname = netname; m_data = data; }
};

typedef CTypedPtrArray<CPtrArray, IPCVia*> IPCViaArray;

//-----------------------------------------------------------------------------
// CIPCPadstack
//-----------------------------------------------------------------------------
class CIPCPadstack
{
public:
   CIPCPadstack(const CString name);
   ~CIPCPadstack();

private:
   CString m_name;
   int m_type; // 0x1 top, 0x2 bottom, 0x4 drill
   int m_shapeTypeTop, m_shapeTypeBottom;
   double m_drillSize;
   bool   m_drillPlated;
   double m_xSizeTop, m_xSizeBottom;
   double m_ySizeTop, m_ySizeBottom;
   double m_xOffsetTop, m_xOffsetBottom;
   double m_yOffsetTop, m_yOffsetBottom;
   double m_rotationTop, m_rotationBottom;
   int m_topMostLayerNum;     // For support of blind/buried vias, need layer range
   int m_bottomMostLayerNum;  // For support of blind/buried vias, need layer range

public:
   CString getName() const                      { return m_name; }
   int getType() const                          { return m_type; }
   int getShapeTypeTop() const                  { return m_shapeTypeTop; }
   int getShapeTypeBottom() const               { return m_shapeTypeBottom; }
   double getDrillSize() const                  { return m_drillSize; }
   bool   getDrillPlated() const                { return m_drillPlated; }
   double getXSizeTop() const                   { return m_xSizeTop; }
   double getXSizeBottom() const                { return m_xSizeBottom; }
   double getYSizeTop() const                   { return m_ySizeTop; }
   double getYSizeBottom() const                { return m_ySizeBottom; }
   double getXOffsetTop() const                 { return m_xOffsetTop; }
   double getXOffsetBottom() const              { return m_xOffsetBottom; }
   double getYOffsetTop() const                 { return m_yOffsetTop; }
   double getYOffsetBottom() const              { return m_yOffsetBottom; }
   double getRotationTop() const                { return m_rotationTop; }
   double getRotationBottom() const             { return m_rotationBottom; }
   int getTopMostLayerNum() const               { return m_topMostLayerNum; }
   int getBottomMostLayerNum() const            { return m_bottomMostLayerNum; }


   void setType(const int type)                 { m_type = type; }
   void setShapeTypeTop(const int shapeType)    { m_shapeTypeTop = shapeType; }
   void setShapeTypeBottom(const int shapeType) { m_shapeTypeBottom = shapeType; }
   void setDrillSize(const double size)         { m_drillSize = size; }
   void setDrillPlated(const bool flag)         { m_drillPlated = flag; }
   void setXSizeTop(const double size)          { m_xSizeTop = size; }
   void setXSizeBottom(const double size)       { m_xSizeBottom = size; }
   void setYSizeTop(const double size)          { m_ySizeTop = size; }
   void setYSizeBottom(const double size)       { m_ySizeBottom = size; }
   void setXOffsetTop(const double size)        { m_xOffsetTop = size ; }
   void setXOffsetBottom(const double size)     { m_xOffsetBottom = size; }
   void setYOffsetTop(const double size)        { m_yOffsetTop = size ; }
   void setYOffsetBottom(const double size)     { m_yOffsetBottom = size; }
   void setRotationTop(const double rotation)   { m_rotationTop = rotation; }
   void setRotationBottom(const double rotation){ m_rotationBottom = rotation; }
   void setTopMostLayerNum(const int n)         { m_topMostLayerNum = n; }
   void setBottomMostLayerNum(const int n)      { m_bottomMostLayerNum = n; }
};

//-----------------------------------------------------------------------------
// CIPCPadstackMap
//-----------------------------------------------------------------------------
class CIPCPadstackMap
{
public:
   CIPCPadstackMap();
   ~CIPCPadstackMap();

private:
   CTypedMapStringToPtrContainer<CIPCPadstack*> m_padstackMap;

public:
   void empty();
   CIPCPadstack* addPadstack(BlockStruct* block);
   CIPCPadstack* findPadstack(const CString name);
};