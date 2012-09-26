
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2011. All Rights Reserved.
*/

#pragma once

// layer flags for connect layer table
#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4

typedef struct
{
   long     entitynum;
   CString  vianame;    // TPorViaxx1
   double   xloc, yloc;
   CString  padstackname;
   int      layer;   // -1 == top, -2 == bottom, -4 both where the DRC error occured.
}HP5DXViaDrcList;
typedef CTypedPtrArray<CPtrArray, HP5DXViaDrcList*> ViaDrcArray;

typedef struct
{
   double   pinx, piny;
   double   rotation;
   BlockStruct *insertedPinGeometry; // aka the padstack block
   int      mirror;
   CString  pinname; // This name may be modified in order to keep all pin named unique.
   CString  originalPinname; // This is original pin insert refname.
}HP5DXCompPinInst;

class CompPinInstArray : public CTypedPtrArray<CPtrArray, HP5DXCompPinInst*>
{
public:
   bool HasCompPin(CString refname);
};

// 
struct HP5DXLayerStruct
{
   int stackNum;        // XRF electrical Number 1..n including power layers
   int layerindex;      // layerindex for CAMCAD.
   BOOL on;
   char type;           // S = Signal, D = Document, P = Power
   CString Name;        // CAMCAD name
};
typedef CTypedPtrArray<CPtrArray, HP5DXLayerStruct*> HP5DXLayerArray;

//---------------------------------------------------------------------------
//
// These are not really a complete padstack, these only have
// the top and bottom surface pads.
//
class HP5DXPadstack;
typedef CTypedPtrArray<CPtrArray, HP5DXPadstack*> PadstackArray;

class HP5DXPadstack
{
private:
   int               m_block_num;  //
   CString           m_name;
   double            m_drill;
   int               m_layerType;   // 0x1 top, 0x2 bottom, 0x4 drill
   ApertureShapeTag  m_shapetypetop;
   ApertureShapeTag  m_shapetypebot;
   double            m_xsizetop, m_ysizetop, m_xofftop, m_yofftop, m_toprotation;
   double            m_xsizebot, m_ysizebot, m_xoffbot, m_yoffbot, m_botrotation;

   HP5DXPadstack     *m_parentPS; // owner of this object if this is a subpad, otherwise NULL

   // insert pt different from offset, the rotation is applied to the offset, but not to the insert pt
   double   m_insertx;  // Same for top and bottom
   double   m_inserty;

   PadstackArray  m_subpadarray;

   void GetPADSTACKData(BlockStruct *padstackBlock, int insertLayer, double scale, int punits);
   bool DissectComplexAperture(int pageUnits, BlockStruct *cmplxAppSubBlock, int layerType, double insertX, double insertY, double insertRotRad);

   bool AddPolyListSubPads(int pageUnits, CPolyList *polyList, int layerType, double insertX, double insertY, double insertRotRad);

   bool HackComplexAperture(int pageUnits, BlockStruct *cmplxAppSubBlock, double &sizeX, double &sizeY, double &offsetX, double &offsetY);
   void HackComplexAperture(int pageUnits, CPoly *poly, double &sizeX, double &sizeY, double &offsetX, double &offsetY);

   void Set   (int layerType, ApertureShapeTag shape, double sizeA, double sizeB, double offsetX, double offsetY, double rotationRadians, double insertX, double insertY);
   void SetTop(               ApertureShapeTag shape, double sizeA, double sizeB, double offsetX, double offsetY, double rotationRadians);
   void SetBot(               ApertureShapeTag shape, double sizeA, double sizeB, double offsetX, double offsetY, double rotationRadians);

public:
   HP5DXPadstack(BlockStruct &padstackBlock, int insertLayer, double scale, int punits);
   HP5DXPadstack(CString name, HP5DXPadstack *parentPS);

   PadstackArray &GetSubPadArray() { return m_subpadarray; }

   int GetType()              { return m_layerType; }

   ApertureShapeTag GetShapeTypeTop()      { return m_shapetypetop; }
   ApertureShapeTag GetShapeTypeBot()      { return m_shapetypebot; }
   ApertureShapeTag GetShapeType()         { return (m_layerType == 1) ? m_shapetypetop : m_shapetypebot; }

   int GetBlockNum()       { return m_block_num; }
   CString GetName()       { return m_name; }

   double GetDrill()       { return (m_parentPS != NULL) ? m_parentPS->m_drill : m_drill; }

   double GetXSizeTop()    { return m_xsizetop; }
   double GetYSizeTop()    { return m_ysizetop; }
   double GetXOffTop()     { return m_xofftop; }
   double GetYOffTop()     { return m_yofftop; }
   double GetTopRotation() { return m_toprotation; }

   double GetXSizeBot()    { return m_xsizebot; }
   double GetYSizeBot()    { return m_ysizebot; }
   double GetXOffBot()     { return m_xoffbot; }
   double GetYOffBot()     { return m_yoffbot; }
   double GetBotRotation() { return m_botrotation; }

   double GetInsertX()     { return m_insertx; }
   double GetInsertY()     { return m_inserty; }

   bool GetOutputValues(double scale, char  &formchar, double &dimx, double &dimy, double &offx, double &offy, double &aprotRadians, double &insertX, double &insertY);

};

class HP5DX_HackedAperture
{
private:
   double m_sizeA;
   double m_sizeB;
   double m_offsetX;
   double m_offsetY;

public:
   HP5DX_HackedAperture(double sizeA, double sizeB, double offsetX, double offsetY);

   void GetValues(double &sizeA, double &sizeB, double &offsetX, double &offsetY);

};
typedef CTypedMapWordToPtrContainer<HP5DX_HackedAperture*> HackedApertureMap;

typedef struct
{
   CString  refname;
   int      geomnum;
   double   x,y;
   int      rotation;         // 0..3
   int      mirror;
}HP5DXPCBList;

typedef struct
{
   CString  compname;
   int      mirror;
   int      blocknum;
}HP5DXCompInst;
typedef CTypedPtrArray<CPtrArray, HP5DXCompInst*> CompInstArray;