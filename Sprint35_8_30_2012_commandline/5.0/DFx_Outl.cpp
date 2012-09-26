// $Header: /CAMCAD/5.0/DFx_Outl.cpp 40    6/17/07 8:51p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "dfm.h"
#include "lic.h"
#include "crypt.h"
#include "api.h"
#include "ccview.h"
#include "drc.h"
#include "extents.h"
#include "graph.h"
#include "outline.h"
#include "pcbutil.h"
#include "outln_rb.h"
#include "polylib.h"
#include "lyrmanip.h"
#include <float.h>
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CCEtoODBView *apiView; // from API.CPP
extern char *FailureRanges[]; // from DRC.CPP

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define CompBodyTop              "DFT_OUTLINE Comp Body Top"
#define CompBodyBottom           "DFT_OUTLINE Comp Body Bottom"
#define PinCentersTop            "DFT_OUTLINE Pin Centers Top"
#define PinCentersBottom         "DFT_OUTLINE Pin Centers Bottom"
#define PinExtentsTop            "DFT_OUTLINE Pin Extents Top"
#define PinExtentsBottom         "DFT_OUTLINE Pin Extents Bottom"
#define PinBodyExtentsTop        "DFT_OUTLINE Pin/Body Extents Top"
#define PinBodyExtentsBottom     "DFT_OUTLINE Pin/Body Extents Bottom"
#define InsidePinsTop            "DFT_OUTLINE Inside Pins Top"
#define InsidePinsBottom         "DFT_OUTLINE Inside Pins Bottom"
#define ToolingKeepoutTop        "DFT_OUTLINE Tooling Keepout Top"
#define ToolingKeepoutBottom     "DFT_OUTLINE Tooling Keepout Bottom"

void CloseEditDlg();

static LayerStruct *compBodyTopLayer = NULL;
static LayerStruct *compBodyBottomLayer = NULL;
static LayerStruct *pinCentersTopLayer = NULL;
static LayerStruct *pinCentersBottomLayer = NULL;
static LayerStruct *pinExtentsTopLayer = NULL;
static LayerStruct *pinExtentsBottomLayer = NULL;
static LayerStruct *pinBodyExtentsTopLayer = NULL;
static LayerStruct *pinBodyExtentsBottomLayer = NULL;
static LayerStruct *insidePinsTopLayer = NULL;
static LayerStruct *insidePinsBottomLayer = NULL;
static LayerStruct *toolingKeepoutTopLayer = NULL;
static LayerStruct *toolingKeepoutBottomLayer = NULL;
static double toolingKeepoutDip300A = 0.1;
static double toolingKeepoutDip300B = 0.1;
static double toolingKeepoutDip300C = 0.1;
static double toolingKeepoutDip300D = 0.1;
static double toolingKeepoutDip400A = 0.1;
static double toolingKeepoutDip400B = 0.1;
static double toolingKeepoutDip400C = 0.1;
static double toolingKeepoutDip400D = 0.1;
static double toolingKeepoutDip600A = 0.1;
static double toolingKeepoutDip600B = 0.1;
static double toolingKeepoutDip600C = 0.1;
static double toolingKeepoutDip600D = 0.1;
static double toolingKeepoutSipA = 0.1;
static double toolingKeepoutSipB = 0.1;
static double toolingKeepoutPgaA = 0.1;
static double toolingKeepoutAxialA = 0.1;
static double toolingKeepoutAxialB = 0.1;
static double toolingKeepoutAxialC = 0.1;
static double toolingKeepoutRadialA = 0.1;
static double toolingKeepoutRadialB = 0.1;
static double toolingKeepoutRadialC = 0.1;
static double toolingKeepoutRadialD = 0.1;

static void GenerateCompBodyOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass);
static void GeneratePinCentersOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass);
static void GeneratePinExtentsOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass);
static void GeneratePinBodyExtentsOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass);
static void GenerateToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass);
static void GenerateDipToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, 
      int graphicClass, double toolingKeepoutDipA, double toolingKeepoutDipB, double toolingKeepoutDipC, double toolingKeepoutDipD);
static void GenerateSipToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass);
static void GeneratePgaToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass);
static void GenerateAxialToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass);
static void GenerateRadialToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass);

static void GenerateDFxOutlineLayers(CCEtoODBDoc *doc);
static void DeleteDFXOutlines(CCEtoODBDoc *doc);
static void GeometryGenerateDFxOutlines(CCEtoODBDoc *doc, BlockStruct *geometry);
static int DFxOutlineAddData(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, long graphicClass, int layerType);
static void AddPntToPoly(CPoly *poly, double x, double y);
static void AddOutline(CCEtoODBDoc *doc, CPntList *pntList, LayerStruct *layer, int graphicClass);
static void AddExtentOutline(CCEtoODBDoc *doc, ExtentRect *extents, LayerStruct *layer, int graphicClass);
static void ReadToolingKeepoutParameters();

/******************************************************************************
* DFM::GenerateDFxOutlines
*/
SHORT DFM::GenerateDFxOutlines()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());

#ifdef SHAREWARE
   return RC_NO_LICENSE;
#endif

   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDfm)) 
   {
      ErrorAccess("You do not have a License for DFM!");
      return RC_NO_LICENSE;
   }*/

   if (apiView == NULL)
      return RC_NO_ACTIVE_DOC;

   CloseEditDlg();

   CCEtoODBDoc *doc = apiView->GetDocument();

   GenerateDFxOutlineLayers(doc);

   DeleteDFXOutlines(doc);

   ReadToolingKeepoutParameters();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *geometry = doc->getBlockAt(i);

      if (!geometry)
         continue;

      if (geometry->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
         GeometryGenerateDFxOutlines(doc, geometry);
   }

   return RC_SUCCESS;
}

/******************************************************************************
* GenerateDFxOutlineLayers
*/
static void GenerateDFxOutlineLayers(CCEtoODBDoc *doc)
{
   WORD keyword = doc->RegisterKeyWord(ATT_DFX_OUTLINE_TYPE, 0, VT_STRING);

   compBodyTopLayer = doc->Add_Layer(CompBodyTop);
   compBodyBottomLayer = doc->Add_Layer(CompBodyBottom);
   compBodyTopLayer->setMirroredLayerIndex( compBodyBottomLayer->getLayerIndex());
   compBodyBottomLayer->setMirroredLayerIndex( compBodyTopLayer->getLayerIndex());
   doc->SetAttrib(&compBodyTopLayer->getAttributesRef(), keyword, VT_STRING, "Comp Body Top", SA_OVERWRITE, NULL);
   doc->SetAttrib(&compBodyBottomLayer->getAttributesRef(), keyword, VT_STRING, "Comp Body Bottom", SA_OVERWRITE, NULL);

   pinCentersTopLayer = doc->Add_Layer(PinCentersTop);
   pinCentersBottomLayer = doc->Add_Layer(PinCentersBottom);
   pinCentersTopLayer->setMirroredLayerIndex( pinCentersBottomLayer->getLayerIndex());
   pinCentersBottomLayer->setMirroredLayerIndex( pinCentersTopLayer->getLayerIndex());
   doc->SetAttrib(&pinCentersTopLayer->getAttributesRef(), keyword, VT_STRING, "Pin Centers Top", SA_OVERWRITE, NULL);
   doc->SetAttrib(&pinCentersBottomLayer->getAttributesRef(), keyword, VT_STRING, "Pin Centers Bottom", SA_OVERWRITE, NULL);

   pinExtentsTopLayer = doc->Add_Layer(PinExtentsTop);
   pinExtentsBottomLayer = doc->Add_Layer(PinExtentsBottom);
   pinExtentsTopLayer->setMirroredLayerIndex( pinExtentsBottomLayer->getLayerIndex());
   pinExtentsBottomLayer->setMirroredLayerIndex( pinExtentsTopLayer->getLayerIndex());
   doc->SetAttrib(&pinExtentsTopLayer->getAttributesRef(), keyword, VT_STRING, "Pin Extents Top", SA_OVERWRITE, NULL);
   doc->SetAttrib(&pinExtentsBottomLayer->getAttributesRef(), keyword, VT_STRING, "Pin Extents Bottom", SA_OVERWRITE, NULL);

   pinBodyExtentsTopLayer = doc->Add_Layer(PinBodyExtentsTop);
   pinBodyExtentsBottomLayer = doc->Add_Layer(PinBodyExtentsBottom);
   pinBodyExtentsTopLayer->setMirroredLayerIndex( pinBodyExtentsBottomLayer->getLayerIndex());
   pinBodyExtentsBottomLayer->setMirroredLayerIndex( pinBodyExtentsTopLayer->getLayerIndex());
   doc->SetAttrib(&pinBodyExtentsTopLayer->getAttributesRef(), keyword, VT_STRING, "Pin/Body Extents Top", SA_OVERWRITE, NULL);
   doc->SetAttrib(&pinBodyExtentsBottomLayer->getAttributesRef(), keyword, VT_STRING, "Pin/Body Extents Bottom", SA_OVERWRITE, NULL);

/* insidePinsTopLayer = doc->Add_Layer(InsidePinsTop);
   insidePinsBottomLayer = doc->Add_Layer(InsidePinsBottom);
   insidePinsTopLayer->mirror = insidePinsBottomLayer->num;
   insidePinsBottomLayer->mirror = insidePinsTopLayer->num;
   doc->SetAttrib(&insidePinsTopLayer->getAttributesRef(), keyword, VT_STRING, "Inside Pins Top", SA_OVERWRITE, NULL);
   doc->SetAttrib(&insidePinsBottomLayer->getAttributesRef(), keyword, VT_STRING, "Inside Pins Bottom", SA_OVERWRITE, NULL);
*/
   toolingKeepoutTopLayer = doc->Add_Layer(ToolingKeepoutTop);
   toolingKeepoutBottomLayer = doc->Add_Layer(ToolingKeepoutBottom);
   toolingKeepoutTopLayer->setMirroredLayerIndex( toolingKeepoutBottomLayer->getLayerIndex());
   toolingKeepoutBottomLayer->setMirroredLayerIndex( toolingKeepoutTopLayer->getLayerIndex());
   doc->SetAttrib(&toolingKeepoutTopLayer->getAttributesRef(), keyword, VT_STRING, "Tooling Keepout Top", SA_OVERWRITE, NULL);
   doc->SetAttrib(&toolingKeepoutBottomLayer->getAttributesRef(), keyword, VT_STRING, "Tooling Keepout Bottom", SA_OVERWRITE, NULL);
}

/******************************************************************************
* DeleteDFXOutlines
*/
static void DeleteDFXOutlines(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_POLY)
            continue;

         if (data->getLayerIndex() == compBodyTopLayer->getLayerIndex() ||
               data->getLayerIndex() == pinCentersTopLayer->getLayerIndex() ||
               data->getLayerIndex() == pinExtentsTopLayer->getLayerIndex() ||
               data->getLayerIndex() == pinBodyExtentsTopLayer->getLayerIndex() ||
//             data->getLayerIndex() == insidePinsTopLayer->getLayerIndex() ||
               data->getLayerIndex() == toolingKeepoutTopLayer->getLayerIndex())
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
      }
   }
}

/******************************************************************************
* GeometryGenerateDFxOutlines
*/
static void GeometryGenerateDFxOutlines(CCEtoODBDoc *doc, BlockStruct *geometry)
{
   SetCurrentDataList(&geometry->getDataList());

//	GenerateCompBodyOutline(doc, geometry, (geometry->IsDesignedOnTop(*doc)?compBodyTopLayer:compBodyBottomLayer), graphicClassNormal);
   GenerateCompBodyOutline(doc, geometry, compBodyTopLayer, graphicClassNormal);
   GeneratePinCentersOutline(doc, geometry, pinCentersTopLayer, graphicClassNormal);
   GeneratePinExtentsOutline(doc, geometry, pinExtentsTopLayer, graphicClassNormal);
   GeneratePinBodyExtentsOutline(doc, geometry, pinBodyExtentsTopLayer, graphicClassNormal);
   GenerateToolingKeepoutOutline(doc, geometry, toolingKeepoutTopLayer, graphicClassNormal);
}

/******************************************************************************
* GenerateCompBodyOutline
*/
void GenerateCompBodyOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass)
{
   Outline_Start(doc);
         
   int itemsFound = 0;

// TODO : need id for package outline generation (XmlRealPartContent.cpp)
   // Get the package insert
   //DataStruct *data = geometry->GetPackage();
   //if (data)
   //{
   // BlockStruct *package = doc->BlockArray[data->getInsert()->getBlockNumber()];
   // if (package)
   // {
   //    itemsFound = DFxOutlineAddData(doc, &package->DataList,
   //       data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(),
   //       -1, GR_CLASS_PACKAGEBODY, 0);
   // }
   //}

   if (!itemsFound)
      itemsFound = DFxOutlineAddData(doc, &geometry->getDataList(), 0, 0, 0, 0, 1, -1, GR_CLASS_PACKAGEOUTLINE, 0);

   if (itemsFound)
   { 
      int returnCode;
      double accuracy = get_accuracy(doc);
      CPntList *pntList = Outline_GetOutline(&returnCode, accuracy);
      AddOutline(doc, pntList, layer, graphicClass);
      Outline_FreeResults();
   }
   else
   {
      Outline_FreeResults();

      CPolyList *partList = new CPolyList;
      CPoly *poly = new CPoly;
      partList->AddTail(poly);

      Mat2x2 m;
      RotMat2(&m, 0);

      POSITION dataPos = geometry->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = geometry->getDataList().GetNext(dataPos);

         if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
         {
            BlockStruct *subblock = doc->getBlockAt(data->getInsert()->getBlockNumber());

            double xmin, xmax, ymin, ymax;
            block_extents(doc, &xmin, &xmax, &ymin, &ymax, &subblock->getDataList(), data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 
                  data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex(), FALSE);

            AddPntToPoly(poly, xmin, ymax);
            AddPntToPoly(poly, xmax, ymax);
            AddPntToPoly(poly, xmax, ymin);
            AddPntToPoly(poly, xmin, ymin);
         }
      }

      if (!poly->getPntList().GetCount())
      {
         delete partList;
         return;
      }

      double accuracy = get_accuracy(doc);
      if (!CanRubberBand(partList, accuracy))
      {
         delete partList;
         return;
      }

      CPntList *pntList = RubberBand(partList, accuracy);
      FreePolyList(partList);

      AddOutline(doc, pntList, layer, graphicClass);
      FreePntList(pntList);
   }
}

/******************************************************************************
* GeneratePinCentersOutline
*/
static void GeneratePinCentersOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass)
{
   CPoly *poly = new CPoly;
   int pointCount = 0;

   POSITION dataPos = geometry->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = geometry->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
      {
         pointCount++;
         AddPntToPoly(poly, data->getInsert()->getOriginX(), data->getInsert()->getOriginY());
      }
   }

   CPntList *pntList;
   double accuracy = get_accuracy(doc);
   if (pointCount <= 2) // Case 1046 - It has been decided that a two-pin component's Pin Centers outline should use the extents of the pins
	{
		GeneratePinExtentsOutline(doc, geometry, layer, graphicClass);
		return;
	}

	CPolyList *partList = new CPolyList;
   partList->AddTail(poly);

	if (!CanRubberBand(partList, accuracy))
   {
      delete partList;
      return;
   }
   pntList = RubberBand(partList, accuracy);

	FreePolyList(partList);

   AddOutline(doc, pntList, layer, graphicClass);
   FreePntList(pntList);
}

/******************************************************************************
* GeneratePinExtentsOutline
*/
static void GeneratePinExtentsOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass)
{
   CPolyList *partList = new CPolyList;
   CPoly *poly = new CPoly;
   partList->AddTail(poly);

   Mat2x2 m;
   RotMat2(&m, 0);

   POSITION dataPos = geometry->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = geometry->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
      {
         BlockStruct *subblock = doc->getBlockAt(data->getInsert()->getBlockNumber());

         double xmin, xmax, ymin, ymax;
         block_extents(doc, &xmin, &xmax, &ymin, &ymax, &subblock->getDataList(), data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 
               data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex(), FALSE);

         AddPntToPoly(poly, xmin, ymax);
         AddPntToPoly(poly, xmax, ymax);
         AddPntToPoly(poly, xmax, ymin);
         AddPntToPoly(poly, xmin, ymin);
      }
   }

   if (!poly->getPntList().GetCount())
   {
      delete partList;
      return;
   }

   double accuracy = get_accuracy(doc);
   if (!CanRubberBand(partList, accuracy))
   {
      delete partList;
      return;
   }

   CPntList *pntList = RubberBand(partList, accuracy);
   FreePolyList(partList);

   AddOutline(doc, pntList, layer, graphicClass);
   FreePntList(pntList);
}

/******************************************************************************
* GeneratePinBodyExtentsOutline
*/
static void GeneratePinBodyExtentsOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass)
{
   CPolyList *partList = new CPolyList;
   CPoly *poly = new CPoly;
   partList->AddTail(poly);

   Mat2x2 m;
   RotMat2(&m, 0);

   POSITION dataPos = geometry->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = geometry->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_POLY)
      {
         ExtentRect extents;
         PolyExtents(doc, data->getPolyList(), &extents, 1, 0, 0, 0, &m, TRUE);

         AddPntToPoly(poly, extents.left, extents.top);
         AddPntToPoly(poly, extents.right, extents.top);
         AddPntToPoly(poly, extents.right, extents.bottom);
         AddPntToPoly(poly, extents.left, extents.bottom);
      }

      if (data->getDataType() == T_INSERT && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
      {
         BlockStruct *subblock = doc->getBlockAt(data->getInsert()->getBlockNumber());

         double xmin, xmax, ymin, ymax;
         block_extents(doc, &xmin, &xmax, &ymin, &ymax, &subblock->getDataList(), data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 
               data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), data->getInsert()->getScale(), data->getLayerIndex(), FALSE);

         AddPntToPoly(poly, xmin, ymax);
         AddPntToPoly(poly, xmax, ymax);
         AddPntToPoly(poly, xmax, ymin);
         AddPntToPoly(poly, xmin, ymin);
      }
   }

   double accuracy = get_accuracy(doc);
   if (!CanRubberBand(partList, accuracy))
   {
      delete partList;
      return;
   }

   CPntList *pntList = RubberBand(partList, accuracy);
   FreePolyList(partList);

   AddOutline(doc, pntList, layer, graphicClass);
   FreePntList(pntList);
}

/******************************************************************************
* GenerateToolingKeepoutOutline
*/
static void GenerateToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, LayerStruct *layer, int graphicClass)
{
   if (!geometry->getAttributesRef())
      return;
   
   WORD keyword = doc->RegisterKeyWord(ATT_PACKAGE_FAMILY, 0, VT_STRING);
   Attrib* attrib;

   if (!geometry->getAttributesRef()->Lookup(keyword, attrib))
      return;

   CString packageString = attrib->getStringValue();

   // extents of pin centers
   ExtentRect extents;
   extents.left = extents.bottom = FLT_MAX;
   extents.right = extents.top = -FLT_MAX;
   POSITION dataPos = geometry->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = geometry->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;
      
      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

      if (data->getInsert()->getOriginX() < extents.left)
         extents.left = data->getInsert()->getOriginX();

      if (data->getInsert()->getOriginX() > extents.right)
         extents.right = data->getInsert()->getOriginX();

      if (data->getInsert()->getOriginY() < extents.bottom)
         extents.bottom = data->getInsert()->getOriginY();

      if (data->getInsert()->getOriginY() > extents.top)
         extents.top = data->getInsert()->getOriginY();
   }

   if (!packageString.CompareNoCase("DIP_300"))
      GenerateDipToolingKeepoutOutline(doc, geometry, &extents, layer, graphicClass, toolingKeepoutDip300A, toolingKeepoutDip300B, toolingKeepoutDip300C, toolingKeepoutDip300D);
   else if (!packageString.CompareNoCase("DIP_400"))
      GenerateDipToolingKeepoutOutline(doc, geometry, &extents, layer, graphicClass, toolingKeepoutDip400A, toolingKeepoutDip400B, toolingKeepoutDip400C, toolingKeepoutDip400D);
   else if (!packageString.CompareNoCase("DIP_600"))
      GenerateDipToolingKeepoutOutline(doc, geometry, &extents, layer, graphicClass, toolingKeepoutDip600A, toolingKeepoutDip600B, toolingKeepoutDip600C, toolingKeepoutDip600D);
   else if (!packageString.CompareNoCase("SIP"))
      GenerateSipToolingKeepoutOutline(doc, geometry, &extents, layer, graphicClass);
   else if (!packageString.CompareNoCase("PGA"))
      GeneratePgaToolingKeepoutOutline(doc, geometry, &extents, layer, graphicClass);
   else if (!packageString.CompareNoCase("AXIAL"))
      GenerateAxialToolingKeepoutOutline(doc, geometry, &extents, layer, graphicClass);
   else if (!packageString.CompareNoCase("RADIAL"))
      GenerateRadialToolingKeepoutOutline(doc, geometry, &extents, layer, graphicClass);
}

/******************************************************************************
* GenerateDipToolingKeepoutOutline
*/
static void GenerateDipToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, 
      int graphicClass, double toolingKeepoutDipA, double toolingKeepoutDipB, double toolingKeepoutDipC, double toolingKeepoutDipD)
{
   if (extents->right - extents->left > extents->top - extents->bottom) // horizontal
   {
      extents->right += toolingKeepoutDipA;
      extents->left -= toolingKeepoutDipA;
      extents->top += toolingKeepoutDipC;
      extents->bottom -= toolingKeepoutDipC;
   }
   else // vertical
   {
      extents->right += toolingKeepoutDipC;
      extents->left -= toolingKeepoutDipC;
      extents->top += toolingKeepoutDipA;
      extents->bottom -= toolingKeepoutDipA;
   }

   AddExtentOutline(doc, extents, layer, graphicClass);
}

/******************************************************************************
* GenerateSipToolingKeepoutOutline
*/
static void GenerateSipToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass)
{
   if (extents->right - extents->left > extents->top - extents->bottom) // horizontal
   {
      extents->right += toolingKeepoutSipA;
      extents->left -= toolingKeepoutSipA;
      extents->top += toolingKeepoutSipB;
      extents->bottom -= toolingKeepoutSipB;
   }
   else // vertical
   {
      extents->right += toolingKeepoutSipB;
      extents->left -= toolingKeepoutSipB;
      extents->top += toolingKeepoutSipA;
      extents->bottom -= toolingKeepoutSipA;
   }

   AddExtentOutline(doc, extents, layer, graphicClass);
}

/******************************************************************************
* GeneratePgaToolingKeepoutOutline
*/
static void GeneratePgaToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass)
{
   if (extents->right - extents->left > extents->top - extents->bottom) // horizontal
   {
      extents->right += toolingKeepoutPgaA;
      extents->left -= toolingKeepoutPgaA;
      extents->top += toolingKeepoutPgaA;
      extents->bottom -= toolingKeepoutPgaA;
   }
   else // vertical
   {
      extents->right += toolingKeepoutPgaA;
      extents->left -= toolingKeepoutPgaA;
      extents->top += toolingKeepoutPgaA;
      extents->bottom -= toolingKeepoutPgaA;
   }

   AddExtentOutline(doc, extents, layer, graphicClass);
}

/******************************************************************************
* GenerateAxialToolingKeepoutOutline
*/
static void GenerateAxialToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass)
{
   if (extents->right - extents->left > extents->top - extents->bottom) // horizontal
   {
      extents->right += toolingKeepoutAxialC;
      extents->left -= toolingKeepoutAxialC;
      extents->top += toolingKeepoutAxialA;
      extents->bottom -= toolingKeepoutAxialA;
   }
   else // vertical
   {
      extents->right += toolingKeepoutAxialA;
      extents->left -= toolingKeepoutAxialA;
      extents->top += toolingKeepoutAxialC;
      extents->bottom -= toolingKeepoutAxialC;
   }

   AddExtentOutline(doc, extents, layer, graphicClass);
}

/******************************************************************************
* GenerateRadialToolingKeepoutOutline
*/
static void GenerateRadialToolingKeepoutOutline(CCEtoODBDoc *doc, BlockStruct *geometry, ExtentRect *extents, LayerStruct *layer, int graphicClass)
{
   if (extents->right - extents->left > extents->top - extents->bottom) // horizontal
   {
      extents->right += toolingKeepoutRadialC;
      extents->left -= toolingKeepoutRadialC;
      extents->top += toolingKeepoutRadialA;
      extents->bottom -= toolingKeepoutRadialA;
   }
   else // vertical
   {
      extents->right += toolingKeepoutRadialA;
      extents->left -= toolingKeepoutRadialA;
      extents->top += toolingKeepoutRadialC;
      extents->bottom -= toolingKeepoutRadialC;
   }

   AddExtentOutline(doc, extents, layer, graphicClass);
}

/******************************************************************************
* AddExtentOutline
*/
static void AddExtentOutline(CCEtoODBDoc *doc, ExtentRect *extents, LayerStruct *layer, int graphicClass)
{
   DataStruct *data = Graph_PolyStruct(layer->getLayerIndex(), 0, FALSE);
   data->setGraphicClass(graphicClass);

   Graph_Poly(NULL, 0, FALSE, FALSE, TRUE); // Closed

   Graph_Vertex(extents->left, extents->top, 0);
   Graph_Vertex(extents->right, extents->top, 0);
   Graph_Vertex(extents->right, extents->bottom, 0);
   Graph_Vertex(extents->left, extents->bottom, 0);
   Graph_Vertex(extents->left, extents->top, 0);
}

/******************************************************************************
* AddPntToPoly
*/
static void AddPntToPoly(CPoly *poly, double x, double y)
{
   CPnt *pnt = new CPnt();
   pnt->x = (DbUnit)x;
   pnt->y = (DbUnit)y;
   pnt->bulge = 0;
   poly->getPntList().AddTail(pnt);
}

/******************************************************************************
* AddOutline
*/
static void AddOutline(CCEtoODBDoc *doc, CPntList *pntList, LayerStruct *layer, int graphicClass)
{
   if (!pntList)
      return;

   DataStruct *data = Graph_PolyStruct(layer->getLayerIndex(), 0, FALSE);
   data->setGraphicClass(graphicClass);

   Graph_Poly(NULL, 0, FALSE, FALSE, TRUE); // Closed

   POSITION pntPos = pntList->GetHeadPosition();
   while (pntPos)
   {
      CPnt *pnt = pntList->GetNext(pntPos);
      Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
   }
}

/******************************************************************************
* DFxOutlineAddData
*/
static int DFxOutlineAddData(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int insertLayer, long graphicClass, int layerType)
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   int itemsFound = 0;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() == T_TEXT)
         continue;

      if (data->getDataType() != T_INSERT)
      {
         int layerNum;

         if (insertLayer != -1)
            layerNum = insertLayer;
         else
            layerNum = data->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layerNum, mirror))
            continue;

         LayerStruct *layer = doc->FindLayer(layerNum);

         if (layerType && layer->getLayerType() != layerType) // if layertype mentioned and layertype is not equal
            continue;
         
         if (graphicClass && data->getGraphicClass() != graphicClass) // if graphic_class mentioned and graphic class is not equal
            continue;

         Outline_Add(data, scale, insert_x, insert_y, rotation, mirror);
         itemsFound++;
      }
      else
      {
         if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
            continue;

         Point2 point2;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         point2.y = data->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot = rotation + data->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + data->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (!(block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE))
         {
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (data->getLayerIndex() != -1)
               block_layer = data->getLayerIndex();

            itemsFound += DFxOutlineAddData(doc, &block->getDataList(), point2.x, point2.y, block_rot, block_mirror, scale * data->getInsert()->getScale(),
                  block_layer, graphicClass, layerType);
         }
      }
   }

   return itemsFound;
}

/******************************************************************************
* ReadToolingKeepoutParameters
*/
static void ReadToolingKeepoutParameters()
{
   CString filename( getApp().getSystemSettingsFilePath("default.tko") );

   CStdioFile file;

   // Open File
   if (!file.Open(filename, file.modeRead | file.typeText))
   {
      ErrorMessage(filename, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }


   CString line;
   while (file.ReadString(line))
   {
      if (line.IsEmpty())
         continue;
   
      if (line[0] == ';')
         continue;

      int left = 0;
      int right = line.Find(',');

      CString package = line.Mid(left, right-left);

      if (!package.CompareNoCase("DIP_300"))
      {
         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip300A = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip300B = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip300C = atof(line.Mid(left, right-left));

         left = right+1;
         toolingKeepoutDip300D = atof(line.Mid(left));
      }
      if (!package.CompareNoCase("DIP_400"))
      {
         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip400A = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip400B = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip400C = atof(line.Mid(left, right-left));

         left = right+1;
         toolingKeepoutDip400D = atof(line.Mid(left));
      }
      if (!package.CompareNoCase("DIP_600"))
      {
         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip600A = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip600B = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutDip600C = atof(line.Mid(left, right-left));

         left = right+1;
         toolingKeepoutDip600D = atof(line.Mid(left));
      }
      if (!package.CompareNoCase("SIP"))
      {
         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutSipA = atof(line.Mid(left, right-left));

         left = right+1;
         toolingKeepoutSipB = atof(line.Mid(left));
      }
      if (!package.CompareNoCase("PGA"))
      {
         left = right+1;
         toolingKeepoutPgaA = atof(line.Mid(left));
      }
      if (!package.CompareNoCase("AXIAL"))
      {
         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutAxialA = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutAxialB = atof(line.Mid(left, right-left));

         left = right+1;
         toolingKeepoutAxialC = atof(line.Mid(left));
      }
      if (!package.CompareNoCase("RADIAL"))
      {
         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutRadialA = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutRadialB = atof(line.Mid(left, right-left));

         left = right+1;
         right = line.Find(',', left);
         toolingKeepoutRadialC = atof(line.Mid(left, right-left));

         left = right+1;
         toolingKeepoutRadialD = atof(line.Mid(left));
      }
   }   

   file.Close();
}
