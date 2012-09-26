// $Header: /CAMCAD/5.0/PcbUtil.cpp 164   6/30/07 2:32a Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"               
#include "graph.h"
#include "pcbutil.h"
#include "attrib.h"
#include "geomlib.h"
#include <math.h>
#include <float.h>
#include "crypt.h"
#include <afxtempl.h>
#include "gauge.h"
#include "extents.h"
#include "polylib.h"
#include "drc.h"
#include "apertur2.h"
#include "compvalues.h"
#include "CamCadDatabase.h"
#include "EntityNumber.h"
#include "CCEtoODB.h"
#include "dft.h"
#include "dcaCamCadData.h"
#include "RwUiLib.h"
typedef struct
{
   CString  oldvalue;
   CString  newvalue;
}CHECKVALUE_List;
typedef CTypedPtrArray<CPtrArray, CHECKVALUE_List*> CheckValueArray;

extern char       *testaccesslayers[];    // from dbutil.cpp
extern SelectNets *selectNetsDlg;   // from NETS.CPP

//extern void       ExplodeInsert(CCEtoODBDoc *doc, DataStruct *data, CDataList *dataList);

#define           MAX_DCODES     9999
#define           MAX_TCODES     9999

typedef struct
{
   int            attribute_cnt; //
   int            attribute_mem; //
   int            data_cnt;      //
   int            data_mem;      //
   int            block_cnt;     //
   int            block_mem;     //
   int            net_cnt;       //
   int            net_mem;       //
   int            comppin_cnt;   //
   int            comppin_mem;   //
   int            type_cnt;      //
   int            type_mem;      //
}MemCheck;

static void CrackPolys(CPoly *poly1, CPoly *poly2);
static int NeedToCrack(CPnt *p1a, CPnt *p1b, CPnt *p2a, CPnt *p2b, CPnt *intersection);
static int EliminateOverlappingTracesInPolys(CPoly *poly1, CPoly *poly2, DataStruct *data1, DataStruct *data2, CDataList *DataList, double accuracy);
static void RemovePolySegment(CPoly *poly, POSITION *pos, DataStruct *data);
static void clear_ComponentNotLoadedData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList);
static void color_ComponentNotLoadedData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, int red, int green, int blue);
static bool IsEqualComplexAperture(CCEtoODBDoc *doc, BlockStruct* block1, BlockStruct* block2, double accuracy);
static bool IsEqualBasicAperture(CCEtoODBDoc *doc, BlockStruct* block1, BlockStruct* block2, double accuracy);
double get_pinpitch_old(BlockStruct *block, double accuracy);
double get_pinpitch_new(BlockStruct *block, double accuracy, CCEtoODBDoc *doc);
bool isValidDrillExist(CCEtoODBDoc *doc, BlockStruct *block);
static void update_smdCOMPONENTData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList,
   int smdrule, DataStruct *compData,COperationProgress* operationProgress=NULL);
static void update_smdGEOMETRYData(CCEtoODBDoc *doc, int smdRule,COperationProgress* progress=NULL);

static FILE             *debug;
static CheckValueArray  checkvaluearray;  
static int              checkvaluecnt;

#define PointsMatch(pnt1, pnt2, accuracy)            (fabs(pnt1->x - pnt2->x) < accuracy && fabs(pnt1->y - pnt2->y) < accuracy && fabs(pnt1->bulge - pnt2->bulge) < accuracy)
#define PointsMatchIgnoreBulge(pnt1, pnt2, accuracy) (fabs(pnt1->x - pnt2->x) < accuracy && fabs(pnt1->y - pnt2->y) < accuracy )

/******************************************************************************
* convertViasToTestPoints
*
* converts all vias using the specified pad stack to instances of a newly created
* test point component.
*/
void convertViasToTestPoints(CCamCadDatabase& camCadDatabase,BlockStruct* padStackBlock,int fileNum)
{
   if ((padStackBlock != NULL) && (padStackBlock->getBlockType() == BLOCKTYPE_PADSTACK))
   {
      FileStruct* file = camCadDatabase.getFile(fileNum);
      int nextRefDesIndex = 1;
      CString pinName("1");

      BlockStruct* testPointPackageBlock = NULL;
      int netNameKeywordIndex = camCadDatabase.getKeywordIndex(ATT_NETNAME);

      for (int blockInd = 0;blockInd < camCadDatabase.getNumBlocks();blockInd++)
      {
         BlockStruct* pcbBlock = camCadDatabase.getBlock(blockInd);

         if (pcbBlock != NULL && pcbBlock->getBlockType() == BLOCKTYPE_PCB)
         {
            CDataList& pcbDataList = pcbBlock->getDataList();

            for (POSITION pcbDataPos = pcbDataList.GetHeadPosition();pcbDataPos != NULL;)
            {
               POSITION viaDataPos = pcbDataPos;
               DataStruct* pcbData = pcbDataList.GetNext(pcbDataPos);

               if (pcbData->getDataType() == T_INSERT)
               {
                  InsertStruct* viaInsert = pcbData->getInsert();

                  if (viaInsert->getInsertType() == INSERTTYPE_VIA)
                  {
                     BlockStruct* viaBlock = camCadDatabase.getBlock(viaInsert->getBlockNumber());
                     CString netName;

                     if (pcbData->getAttributesRef() != NULL)
                     {
                        Attrib* attribute = getAttribute(pcbData->getAttributesRef(),netNameKeywordIndex);

                        if (attribute != NULL)
                        {
                           netName = camCadDatabase.getAttributeStringValue(attribute);
                        }
                     }

                     if (viaBlock == padStackBlock)
                     {
                        if (testPointPackageBlock == NULL)
                        {
                           testPointPackageBlock = camCadDatabase.getNewBlock("TestPointVia","%d",blockTypePcbComponent,fileNum);
                           camCadDatabase.referenceBlock(testPointPackageBlock,viaBlock,INSERTTYPE_PIN,pinName,-1);
                        }

                        CString refDes;

                        do
                        {
                           refDes.Format("TP%d",nextRefDesIndex);
                           nextRefDesIndex++;
                        }
                        while (camCadDatabase.getPin(file,refDes,pinName) != NULL);

                        CompPinStruct* compPin = camCadDatabase.getDefinedPin(file,refDes,pinName,netName);
                        compPin->setPadstackBlockNumber( viaBlock->getBlockNumber());

                        camCadDatabase.referenceBlock(pcbBlock,testPointPackageBlock,INSERTTYPE_PCBCOMPONENT,refDes,
                           -1,viaInsert->getOriginX(),viaInsert->getOriginY(),viaInsert->getAngle(),viaInsert->getMirrorFlags() != 0,viaInsert->getScale());

                        // Remove via insert
                        RemoveOneEntityFromDataList(&(camCadDatabase.getCamCadDoc()),&pcbDataList,pcbData,viaDataPos);
                     }
                  }
               }
            }
         }
      }
   }
}

void convertSmdViasToTestPoints(CCamCadDatabase& camCadDatabase)
{
   for (int blockInd = 0;blockInd < camCadDatabase.getNumBlocks();blockInd++)
   {
      BlockStruct* pcbBlock = camCadDatabase.getBlock(blockInd);

      // process PCB blocks
      if (pcbBlock != NULL && pcbBlock->getBlockType() == BLOCKTYPE_PCB)
      {
         int maxeleclayer = 0; //initialize the Bottom Electrical Layer Stack Number
         for (int j=0; j<camCadDatabase.getNumLayers(); j++)  // Get the Bottom Electrical Layer stack number
         {
            LayerStruct *layer = camCadDatabase.getLayerAt(j);
            if (layer == NULL)
               continue; 
            if (layer->getElectricalStackNumber() > maxeleclayer)
               maxeleclayer = layer->getElectricalStackNumber();
         }
         CMapPtrToPtr smdViaBlocks;
         smdViaBlocks.InitHashTable(nextPrime2n(1000));
         CDataList& pcbDataList = pcbBlock->getDataList();

         for (POSITION pcbDataPos = pcbDataList.GetHeadPosition();pcbDataPos != NULL;)
         {
            POSITION viaDataPos = pcbDataPos;
            DataStruct* pcbData = pcbDataList.GetNext(pcbDataPos);

            if (pcbData->getDataType() == T_INSERT)
            {
               InsertStruct* viaInsert = pcbData->getInsert();

               // process vias
               if (viaInsert->getInsertType() == INSERTTYPE_VIA)
               {
                  BlockStruct* viaPadStackBlock = camCadDatabase.getBlock(viaInsert->getBlockNumber());

                  if (viaPadStackBlock->getName().GetLength() > 0)
                  {
                     CDataList& padStackDataList = viaPadStackBlock->getDataList();
                     bool topFlag    = false;
                     bool bottomFlag = false;
                     bool innerFlag  = false;

                     for (POSITION padPos = padStackDataList.GetHeadPosition();padPos != NULL;)
                     {
                        DataStruct* padData = padStackDataList.GetNext(padPos);
                        LayerStruct* layer = camCadDatabase.getLayerAt(padData->getLayerIndex());

                        if (layer != NULL)
                        {
                           if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP  ) topFlag    = true;
                           if (layer->getLayerType() == LAYTYPE_PAD_TOP     ) topFlag    = true;
                           
                           if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT  ) bottomFlag = true;
                           if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM  ) bottomFlag = true;
                           
                           if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER) innerFlag  = true;
                           if (layer->getLayerType() == LAYTYPE_PAD_INNER   ) innerFlag  = true;
                           
                           if ((layer->getLayerType() == LAYTYPE_PAD_OUTER) || (layer->getLayerType() == LAYTYPE_SIGNAL_OUTER)) 
                           {
                                topFlag    = true;
                                bottomFlag = true;
                           }
                           
                           if ((layer->getLayerType() == LAYTYPE_SIGNAL_ALL) || (layer->getLayerType() == LAYTYPE_PAD_ALL))
                           {
                                topFlag    = true;
                                innerFlag  = true;
                                bottomFlag = true;
                           }

                           if ((layer->getLayerType() == LAYTYPE_PAD_THERMAL) || (layer->getLayerType() == LAYTYPE_POWERNEG) ||
                               (layer->getLayerType() == LAYTYPE_POWERPOS) || (layer->getLayerType() == LAYTYPE_SPLITPLANE) ||
                               (layer->getLayerType() == LAYTYPE_SIGNAL))
                           {
                                if (layer->getElectricalStackNumber() == 1)
                                        topFlag    = true;
                                if ((layer->getElectricalStackNumber() > 1) && (layer->getElectricalStackNumber() < maxeleclayer))
                                        innerFlag  = true;
                                if (layer->getElectricalStackNumber() == maxeleclayer)
                                        bottomFlag = true;
                           }
                        }
                     }

                     if (!innerFlag && (topFlag != bottomFlag))
                     {
                        smdViaBlocks.SetAt(viaPadStackBlock,NULL);
                     }
                  }
               }
            }
         }

         CSupString descriptor;
         CStringArray params;
         CString value;
         BlockStruct* viaPadStackBlock;
         void* junk;

         for (POSITION pos = smdViaBlocks.GetStartPosition();pos != NULL;)
         {
            smdViaBlocks.GetNextAssoc(pos,(void*&)viaPadStackBlock,junk);

            convertViasToTestPoints(camCadDatabase,viaPadStackBlock,pcbBlock->getFileNumber());
         }
      }
   }
}

/******************************************************************************
* OnGeneratePinNumbers
*/
void CCEtoODBDoc::OnGeneratePinNumbers() 
{
   GeneratePinNumbers(this);
}

/******************************************************************************
* OnGeneratePinPitch
*/
void CCEtoODBDoc::OnGeneratePinPitch() 
{
   GeneratePinPitch(this);
}

/******************************************************************************
* GeneratePinPitch
*/
void GeneratePinPitch(CCEtoODBDoc *doc)
{
   WORD keyword = doc->IsKeyWord(ATT_PIN_PITCH, 0);
   double accuracy = get_accuracy(doc);

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      double pitch = get_pinpitch(block, accuracy, doc);
      if (pitch > 0)
         doc->SetAttrib(&block->getAttributesRef(), keyword, VT_UNIT_DOUBLE, &pitch, attributeUpdateOverwrite, NULL);
   }

   RefreshInheritedAttributes(doc, 0);
}

/******************************************************************************
* GeneratePinNumbers
*  - generates PINNR attrib if it doesn't exist
*/
void GeneratePinNumbers(CCEtoODBDoc *doc)
{
   WORD pinnrKW = doc->IsKeyWord(ATT_COMPPINNR, 0);

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getBlockType() != BLOCKTYPE_PCBCOMPONENT && block->getBlockType() != blockTypeDie)
         continue;

      // count pins
      int pinCount = 0;
      CInsertTypeMask insertTypeMask(insertTypePin, insertTypeDiePin);
      for (CDataListIterator insertIterator(block->getDataList(),insertTypeMask);insertIterator.hasNext();)
      {
         DataStruct *data = insertIterator.getNext();

         pinCount++;
      }

      if (!pinCount)
         continue;

      // fill pinNumArray with used pinNums
      char *pinNumArray = (char*)calloc(pinCount+1, sizeof(BOOL));
      for (CDataListIterator insertIterator(block->getDataList(),insertTypeMask);insertIterator.hasNext();)
      {
         DataStruct *data = insertIterator.getNext();

         Attrib *attrib;

         if (data->getAttributesRef() && data->getAttributesRef()->Lookup(pinnrKW, attrib))
         {
            if (attrib->getIntValue() <= pinCount)
            {
               if (pinNumArray[attrib->getIntValue()]) // already used pin num
                  RemoveAttrib(pinnrKW, &data->getAttributesRef());
               else
                  pinNumArray[attrib->getIntValue()] = TRUE;
            }
         }
      }

      // try to use refname
      for (CDataListIterator insertIterator(block->getDataList(),insertTypeMask);insertIterator.hasNext();)
      {
         DataStruct *data = insertIterator.getNext();

         Attrib *attrib;

         if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(pinnrKW, attrib))
         {
            if (data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
            {
               int pinNum = atoi(data->getInsert()->getRefname());

               if (pinNum && !pinNumArray[pinNum])
               {
                  doc->SetAttrib(&data->getAttributesRef(), pinnrKW, VT_INTEGER, &pinNum, attributeUpdateOverwrite, NULL);
                  pinNumArray[pinNum] = TRUE;
               }
            }
         }
      }

      // assign the rest
      for (CDataListIterator insertIterator(block->getDataList(),insertTypeMask);insertIterator.hasNext();)
      {
         DataStruct *data = insertIterator.getNext();

         Attrib* attrib;

         if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(pinnrKW, attrib))
         {
            int pinNum = 1;

            while (pinNumArray[pinNum])
               pinNum++;

            doc->SetAttrib(&data->getAttributesRef(), pinnrKW, VT_INTEGER, &pinNum, attributeUpdateOverwrite, NULL);
            pinNumArray[pinNum] = TRUE;
         }
      }

      free(pinNumArray);
   }
}

/****************************************************************************
* OnResolveBreakoutPad
*/
void CCEtoODBDoc::OnOptimizePadstack() 
{
   OptimizePadstacks(this, getSettings().getPageUnits(), TRUE);
}

/****************************************************************************
* OnResolveBreakoutPad
*/
void CCEtoODBDoc::OnResolveBreakoutPad() 
{
   ErrorMessage("Resolve Breakout Pad not implemented!");   
}

/****************************************************************************
* OnGenerateBomGraphic
*/
void CCEtoODBDoc::OnGenerateBomGraphic() 
{
   ErrorMessage("OnGenerateBomGraphic not implemented");
}


void CCEtoODBDoc::OnBomClearColor() 
{
   UnselectAll(FALSE);

   // here loop through Files and 
   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      clear_ComponentNotLoadedData(this, file,&(file->getBlock()->getDataList()));
   }  

   UpdateAllViews(NULL);

   return;
}

void CCEtoODBDoc::OnBomColorComp() 
{
   int red =120, green = 120, blue = 120;
   
   UnselectAll(FALSE);

   // here loop through Files and 
   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      if (file)
         color_ComponentNotLoadedData(this, file, red, green, blue);
   }  

   UpdateAllViews(NULL);

   return;
}

void CCEtoODBDoc::OnBomSetComponentsUnloaded() 
{
   UnselectAll(FALSE);

   // here loop through Files and 
   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      set_ComponentNotLoadedData(this, file);
   }  

   OnBomColorComp();

   return;
}

void CCEtoODBDoc::OnBomSetLoaded() 
{
   UnselectAll(FALSE);

   // here loop through Files and 
   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getFileList().GetNext(pos);
      set_ComponentLoadedData(this, file);
   }  

   OnBomColorComp();
}

/******************************************************************************
* DoAutoBoardOrigin
*/
void DoAutoBoardOrigin(CCEtoODBDoc *doc, BOOL Left, BOOL Bottom)
{
   BOOL Found = FALSE;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      Found = TRUE;
      AutoBoardOrigin(doc, file, Left, Bottom);
      break;
   }

   if (!Found)
      ErrorMessage("No Panel/Board Outline found.");
}

/****************************************************************************
* OnAutoBoardOrigin also will do Panel if visible
*/
void CCEtoODBDoc::OnAutoBoardOrigin_BL() 
{
   DoAutoBoardOrigin(this, TRUE, TRUE);
}

void CCEtoODBDoc::OnAutoBoardOrigin_BR() 
{
   DoAutoBoardOrigin(this, FALSE, TRUE);
}

void CCEtoODBDoc::OnAutoBoardOrigin_TL() 
{
   DoAutoBoardOrigin(this, TRUE, FALSE);
}

void CCEtoODBDoc::OnAutoBoardOrigin_TR() 
{
   DoAutoBoardOrigin(this, FALSE, FALSE);
}

/******************************************************************************
* AutoBoardOrigin
*/
void AutoBoardOrigin(CCEtoODBDoc *doc, FileStruct *file, BOOL Left, BOOL Bottom) 
{
   CWaitCursor wait;

   double x, y;

   if (Left)
      x = FLT_MAX;
   else
      x = -FLT_MAX;
   if (Bottom)
      y = FLT_MAX;
   else
      y = -FLT_MAX;

   Mat2x2 m;
   RotMat2(&m, file->getRotation());

   BOOL Found = FALSE;

   // find lower-left of visible Primary Board Outlines
   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_POLY)
         continue;

      if (data->getGraphicClass() != GR_CLASS_BOARDOUTLINE && data->getGraphicClass() != GR_CLASS_PANELOUTLINE)
         continue;

      Found = TRUE;

      ExtentRect extents;   
      PolyExtents(doc, data->getPolyList(), &extents, file->getScale(), (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0), file->getInsertX(), file->getInsertY(), &m, FALSE);

      if (Left)
      {
         if (extents.left < x)
            x = extents.left;
      }
      else
      {
         if (extents.right > x)
            x = extents.right;
      }

      if (Bottom)
      {
         if (extents.bottom < y)
            y = extents.bottom;
      }
      else
      {
         if (extents.top > y)
            y = extents.top;
      }
   }

   if (!Found)
   {
      CString buf = "No Board Outline nor Panel Outline Found in file ";
      buf += file->getName();
      AfxMessageBox(buf);
      return;
   }

   doc->MoveOrigin(x, y);

   doc->OnRestructureFiles();
}
/****************************************************************************
* CCEtoODBDoc::OnGeneratePin1Marker() 
*/
void CCEtoODBDoc::OnGeneratePin1Marker() 
{
   GeneratePin1Marker dlg;
   dlg.doc = this;
   if (dlg.DoModal() != IDOK)
      return;
}

/****************************************************************************
* CCEtoODBDoc::OnGeneratePadstack
*/
void CCEtoODBDoc::OnGeneratePadstack() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for PCB Edit!");
      return;
   }*/

   GeneratePadstackAccess dlg;
   dlg.m_smd = 1;

   if (dlg.DoModal() != IDOK)
      return;

   generate_PADSTACKACCESSFLAG(this, 1);

   if (dlg.m_smd)
   {
      if (ErrorMessage("Are you sure?", "Update SMD Attribute is not reversable", MB_YESNO | MB_DEFBUTTON2) == IDYES)
      {
         generate_PADSTACKSMDATTRIBUTE(this);
      }  // do you really ....
   }
}

/****************************************************************************
*/ // Keep
void CCEtoODBDoc::GenerateSmdComponentTechnologyAttribs(COperationProgress* progress, bool visibleFilesOnly) 
{
   // Generate the TECHNOLGY attrib, and propogate the 
   // attrib to compPins.

   int componentSMDrule = getSettings().ComponentSMDrule;

   if (!componentSMDrule)
      return;

   update_smdpads(this,progress);
   update_smdrule_geometries(this, componentSMDrule,progress);
   
   for (POSITION filePos = getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (visibleFilesOnly && (!file->isShown() || file->isHidden()))
         continue;

      update_smdrule_components(this, file, componentSMDrule, progress);
   }
}

void CCEtoODBDoc::OnGenerateSmdComponentVisibleFilesOnly() 
{
   // This is for use with Tools option menu, it operates only on
   // visible files. For general application of the same tool, use
   // directly the func this is calling, with visibleOnly set to false.

   GenerateSmdComponentTechnologyAttribs(NULL, true);
}

/******************************************************************************
* ProcessLoadedProbes
*/
int ProcessLoadedProbes(CCEtoODBDoc *doc)
{
	doc->OnRegen();
	doc->OnGeneratePinloc();

	CString placedStr = "Placed";
	CString unplacedStr = "UnPlaced";
	int accessCount = 0;

	WORD dLinkKW = doc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
	WORD netnameKW = doc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
	WORD probePlacementKW = doc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);

	POSITION filePos = doc->getFileList().GetHeadPosition();
	while (filePos)
	{
		FileStruct *file = doc->getFileList().GetNext(filePos);
		BlockStruct *block = file->getBlock();

		if (block->getBlockType() != blockTypePcb)
			continue;

		POSITION probePos = block->getHeadDataInsertPosition();
		while (probePos)
		{
			DataStruct *probeData = block->getNextDataInsert(probePos);
			InsertStruct *probeInsert = probeData->getInsert();

			if (probeInsert->getInsertType() != insertTypeTestProbe)
				continue;

			Attrib *attrib = NULL;

			// if there is already a link, I don't need to go on
			if (probeData->lookUpAttrib(dLinkKW, attrib))
				continue;

			// the probe needs to indicate a net
			if (!probeData->lookUpAttrib(netnameKW, attrib))
				continue;
			CString netName = get_attvalue_string(doc, attrib);

			// This is consistence with how Access Analysis generates its test access name
			CString accessName;
			accessName.Format("$$Access_%d", ++accessCount);

			// generate the test access point and link the probe to it
         BlockStruct *taGeom = generate_TestAccessGeometry(doc, "Test Access", 0.03 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits()));
         DataStruct *taData = Graph_Block_Reference(taGeom->getName(), accessName, 0, probeInsert->getOriginX(), probeInsert->getOriginY(),
												probeInsert->getAngle(), probeInsert->getMirrorFlags(), 1, -1, TRUE);
         taData->getInsert()->setInsertType(insertTypeTestAccessPoint);
			taData->setAttrib(doc->getCamCadData(), netnameKW, valueTypeString, netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
			
			int link = taData->getEntityNumber();
			probeData->setAttrib(doc->getCamCadData(), dLinkKW, valueTypeInteger, &link, attributeUpdateOverwrite, NULL);

			// GenCad peculiarity, if place location is -32767,-32767 (mils), then probe is unplaced
			// Related to case 1442
			// 1/2 mil fuzz factor
			double fuzz = 0.5 * Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
			bool unplaced = fpnear(probeInsert->getOriginX(), -32767.0 * Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits()), fuzz) && 
				fpnear(probeInsert->getOriginY(), -32767.0 * Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits()), fuzz);
			probeData->setAttrib(doc->getCamCadData(), probePlacementKW, valueTypeString, unplaced ? unplacedStr.GetBuffer(0) : placedStr.GetBuffer(0), attributeUpdateOverwrite, NULL);
		}

		POSITION taPos = block->getHeadDataInsertPosition();
		while (taPos)
		{
			DataStruct *taData = block->getNextDataInsert(taPos);
			InsertStruct *taInsert = taData->getInsert();

			if (taInsert->getInsertType() != insertTypeTestAccessPoint)
				continue;

			Attrib *attrib = NULL;
			// if there is already a link, I don't need to go on
			if (taData->lookUpAttrib(dLinkKW, attrib))
				continue;

			bool featureFound = false;

			POSITION featurePos = block->getHeadDataInsertPosition();
			while (featurePos && !featureFound)
			{
				DataStruct *featureData = block->getNextDataInsert(featurePos);
				InsertStruct *featureInsert = featureData->getInsert();

				if (featureInsert->getInsertType() != insertTypeVia)
					continue;

				BlockStruct *featureBlock = doc->getBlockAt(featureInsert->getBlockNumber());
				if (featureBlock && featureBlock->getExtent().isOnOrInside(taInsert->getOrigin2d() - featureInsert->getOrigin2d()))
				{
					int link = featureData->getEntityNumber();
					taData->setAttrib(doc->getCamCadData(), dLinkKW, valueTypeInteger, &link, attributeUpdateOverwrite, NULL);
					featureFound = true;
				}
			}

			POSITION netPos = file->getHeadNetPosition();
			while (netPos && !featureFound)
			{
				NetStruct *net = file->getNextNet(netPos);

				POSITION cpPos = net->getHeadCompPinPosition();
				while (cpPos && !featureFound)
				{
					CompPinStruct *cp = net->getNextCompPin(cpPos);

					BlockStruct *cpBlock = doc->getBlockAt(cp->getPadstackBlockNumber());
					if (cpBlock && cpBlock->getExtent().isOnOrInside(taInsert->getOrigin2d() - cp->getOrigin()))
					{
						int link = cp->getEntityNumber();
						taData->setAttrib(doc->getCamCadData(), dLinkKW, valueTypeInteger, &link, attributeUpdateOverwrite, NULL);
						featureFound = true;
					}
				}
			}
		}
	}

	return 1;
}

/******************************************************************************
* generate_TestAccessGeometry
*/
BlockStruct* generate_TestAccessGeometry(CCEtoODBDoc *doc, const char *name, double size)
{
   doc->StoreDocForImporting();
   BlockStruct *block = Graph_Block_Exists(doc, name, -1);

   if (!block)
   {
      LayerStruct *layer;
      int err;
      int lnr = Graph_Level("TEST_ACCESS_TOP","",0);

      Graph_Level_Mirror("TEST_ACCESS_TOP", "TEST_ACCESS_BOT","");
      
      if ((layer = doc->FindLayer_by_Name("TEST_ACCESS_BOT")) != NULL)
         layer->setLayerType(layerTypeDftBottom);
      if ((layer = doc->FindLayer_by_Name("TEST_ACCESS_TOP")) != NULL)
         layer->setLayerType(layerTypeDftTop);

      Graph_Aperture("ACCESSPAD", T_TARGET, size, 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
      block = Graph_Block_On(GBO_APPEND, name, -1, 0);
      block->setBlockType(BLOCKTYPE_TEST_ACCESSPOINT);
      block->setFlagBits(BL_SPECIAL);
      Graph_Block_Reference("ACCESSPAD", NULL, 0, 0, 0, 0.0, 0, 1.0, lnr, TRUE);
      Graph_Block_Off();
   }

   return block;
}

/******************************************************************************
* generate_TestProbeGeometry
*/
BlockStruct* generate_TestProbeGeometry(CCEtoODBDoc *doc, const char *name, double probediam, const char *tool, double drillsize)
{
	return CreateTestProbeGeometry(doc, name, probediam, tool, drillsize, probeShapeDefaultTarget);
}

/******************************************************************************
* init_realpart_info
*/
void init_realpart_info(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block)
         continue;

      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
      {
         // this needs to make a ATT_PIN_COUNT attribute on any PCBCOMPONENT Geometry.
         if (!is_attvalue(doc, block->getAttributesRef(), ATT_PIN_COUNT, 0))
         {
            int pcnt = get_pincnt(&block->getDataList());
            doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_PIN_COUNT, TRUE), VT_INTEGER, &pcnt, attributeUpdateOverwrite, NULL);
         }

         // this needs to make a ATT_PIN_PITCH attribute on any PCBCOMPONENT Geometry.
         if (!is_attvalue(doc, block->getAttributesRef(), ATT_PIN_PITCH, 0))
         {
            double accuracy = get_accuracy(doc);
            double pitch = get_pinpitch(block, accuracy, doc);
            if (pitch > 0)
               doc->SetAttrib(&block->getAttributesRef(), doc->IsKeyWord(ATT_PIN_PITCH, TRUE), VT_UNIT_DOUBLE, &pitch, attributeUpdateOverwrite, NULL);
         }
      }
   }

   // also this here
   generate_CENTROIDS(doc);
}

/******************************************************************************
* get_accuracy
*/
double get_accuracy(CCEtoODBDoc *doc)
{
   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   double accuracy = 1.0;
   
   for (int i=0; i<decimals; i++)
      accuracy *= 0.1;

   return accuracy;
}

/****************************************************************************
* EliminateDuplicateVias
*
* PARAMETERS:
*           ---BlockStruct *block
*                 the block which need to remove the duplicate vias.
* RESULT:   
*           ---BlockStruct *block
*                 the block after this function is there has no same  
*                 insert via, every insert_via is unique.
* DESCRIPTION:
*           ---Find the duplicate insert via which have same name and point
*              then remove them.
*              loop through data list, if the data is insert, check the insertList,
*              if the insert already in the insertList, delete it from block. 
* NOTES:
*           need a list to memory the insert_via so can loop the list
*           check if it is the duplicate one.
*
***************************************************************************/  
void EliminateDuplicateVias(CCEtoODBDoc *Doc, BlockStruct *block) 
{

#ifdef DEBUG_DUMP
debug = fopen("debug.txt", "w+t");
CTime t;
t = t.GetCurrentTime();
fprintf(debug, "* In EliminateDuplicateVias() for block \"%s\" at %s\n", block->getName(), t.Format("%H:%M"));
fflush(debug);
#endif

#define UseOld_EliminateDuplicateVias

#ifdef UseOld_EliminateDuplicateVias
   CMapWordToPtr map; // Map of BlockNumber to DataList of Inserts to that BlockNumber
   
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = block->getDataList().GetNext(pos);
		if (data->getDataType() != T_INSERT || data->getInsert()->getInsertType() != INSERTTYPE_VIA)
			continue;

		void *voidptr;
      if (map.Lookup(data->getInsert()->getBlockNumber(), voidptr)) // is there already a list of inserts to this blocknum
      {
         CDataList *datalist = (CDataList*)voidptr;
         BOOL Duplicate = FALSE;

         // loop all inserts of this blocknum to see if any are duplicates
         POSITION listPos = datalist->GetHeadPosition();
         while (listPos != NULL)
         {
            DataStruct *p = datalist->GetNext(listPos);
            if (fabs(p->getInsert()->getOriginX() - data->getInsert()->getOriginX()) < SMALLNUMBER &&
                  fabs(p->getInsert()->getOriginY() - data->getInsert()->getOriginY()) < SMALLNUMBER &&
                  fabs(p->getInsert()->getAngle() - data->getInsert()->getAngle()) < SMALLNUMBER &&
                  fabs(p->getInsert()->getScale() - data->getInsert()->getScale()) < SMALLNUMBER &&
                  p->getInsert()->getMirrorFlags() == data->getInsert()->getMirrorFlags() &&
                  p->getLayerIndex() == data->getLayerIndex())
            {
               Duplicate = TRUE;
               break;
            }
         } 

         if (Duplicate)
            RemoveOneEntityFromDataList(Doc, &block->getDataList(), data);
         else 
            datalist->AddTail(data);
      }
      else 
      {
			// need to make a new datalist for inserts to this blocknum
         CDataList *datalist = new CDataList(false);
         datalist->AddTail(data);
         map.SetAt(data->getInsert()->getBlockNumber(), datalist);
      }
   } 

   // free datalists
   POSITION mapPos = map.GetStartPosition();
   while (mapPos != NULL)
   {
		WORD key;
		void *voidPtr;
      map.GetNextAssoc(mapPos, key, voidPtr);

		CDataList* datalist = (CDataList*)voidPtr;
		//datalist->RemoveAll();
      delete datalist;
   }
#else
   // The following code should be much faster than the original code above,
   // however, no testing has been done on it - knv 20051020.1726

   double searchRadius = Doc->convertToPageUnits(pageUnitsMils,.01);
   
   CTypedQfeTree<CDataList> viaDataListTree;

   for (CDataListIterator viaIterator(*block,insertTypeVia);viaIterator.hasNext();)
   {
      DataStruct* via = viaIterator.getNext();

      bool duplicateFlag = false;
      CDataList* viaDataList = NULL;

      CTypedQfeList<CDataList> foundViaDataListList;
      CPoint2d viaOrigin = via->getInsert()->getOrigin2d();
      CExtent searchExtent(viaOrigin,searchRadius);

      viaDataListTree.search(searchExtent,foundViaDataListList);

      if (foundViaDataListList.GetCount() == 0)
      {
         viaDataList = new CDataList(false);
         CTypedQfe<CDataList>* qfe = new CTypedQfe<CDataList>(viaOrigin,viaDataList);
         viaDataListTree.setAt(qfe);
      }
      else
      {
         for (POSITION viaListListPos = foundViaDataListList.GetHeadPosition();viaListListPos != NULL && !duplicateFlag;)
         {
            CTypedQfe<CDataList>* qfe = foundViaDataListList.GetNext(viaListListPos);
            CDataList* dataList = qfe->getObject();
            viaDataList = dataList;

            // loop all inserts of this blocknum to see if any are duplicates         
            for (POSITION listPos = dataList->GetHeadPosition();listPos != NULL;)
            {
               DataStruct* foundVia = dataList->GetNext(listPos);

               if (foundVia->getInsert()->getBlockNumber() == via->getInsert()->getBlockNumber() && 
                   fabs(foundVia->getInsert()->getOriginX() - via->getInsert()->getOriginX()) < SMALLNUMBER &&
                   fabs(foundVia->getInsert()->getOriginY() - via->getInsert()->getOriginY()) < SMALLNUMBER &&
                   fabs(foundVia->getInsert()->getAngle()   - via->getInsert()->getAngle())   < SMALLNUMBER &&
                   fabs(foundVia->getInsert()->getScale()   - via->getInsert()->getScale())   < SMALLNUMBER &&
                   foundVia->getInsert()->getMirrorFlags() == via->getInsert()->getMirrorFlags() &&
                   foundVia->getLayerIndex()               == via->getLayerIndex())
               {
                  duplicateFlag = true;

                  break;
               }
            }
         }

         if (duplicateFlag)
         {
            RemoveOneEntityFromDataList(Doc, &block->getDataList(), via);
         }
         else if (viaDataList != NULL) 
         {
            viaDataList->AddTail(via);
         }
      }
   }
#endif

#ifdef DEBUG_DUMP
t = t.GetCurrentTime();
fprintf(debug, "* done at %s\n\n", t.Format("%H:%M"));
fflush(debug);
#endif

}  // end of EliminateDuplicateVias

struct CrackStruct
{
   float xmin, xmax, ymin, ymax;
   DataStruct *data;
};
typedef CTypedPtrList<CPtrList, CrackStruct*> CCrackList;

/* InsertExtents */
void InsertExtents(CrackStruct *c)
{
   c->xmin = c->ymin = FLT_MAX;
   c->xmax = c->ymax = FLT_MIN;

   POSITION polyPos = c->data->getPolyList()->GetHeadPosition();
   while (polyPos != NULL)
   {
      CPoly *poly = c->data->getPolyList()->GetNext(polyPos);

      POSITION pos = poly->getPntList().GetHeadPosition();
      while (pos != NULL)
      {
         CPnt *pnt = poly->getPntList().GetNext(pos);
         if (pnt->x < c->xmin) c->xmin = pnt->x; 
         if (pnt->x > c->xmax) c->xmax = pnt->x; 
         if (pnt->y < c->ymin) c->ymin = pnt->y; 
         if (pnt->y > c->ymax) c->ymax = pnt->y; 
      }
   }
}

/* ExtentsTouch */
BOOL ExtentsTouch(CrackStruct *c1, CrackStruct *c2)
{
   if (c1->xmin > c2->xmax) return FALSE;
   if (c1->xmax < c2->xmin) return FALSE;
   if (c1->ymin > c2->ymax) return FALSE;
   if (c1->ymax < c2->ymin) return FALSE;

   return TRUE;
}

/****************************************************************************
* void Crack(CCEtoODBDoc *doc, BlockStruct *block) 
*
* PARAMETERS:
*     --- BlockStruct *block = block to crack lines of
*
* DESCRIPTION:
*     --- Crack intersecting polys that are etches on the same layer
*/  
void Crack(CCEtoODBDoc *doc, BlockStruct *block, int etch_only, COperationProgress* operationProgress) 
{

#ifdef DEBUG_DUMP
CTime t;
CTime t1;
CTime t2;
CTimeSpan ts;
t = t.GetCurrentTime();
fprintf(debug, "* In Crack() for block \"%s\" at %s\n", block->getName(), t.Format("%H:%M:%S"));
fflush(debug);
#endif

   CMapWordToPtr map; // map NetName (index in ValueArray) to list of polys in that net

   WORD keyword = doc->IsKeyWord(ATT_NETNAME, TRUE);

   // loop datas, put in correct list, set extents
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos) // loop datas in the block
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // only crack etch polys
      if (data->getDataType() != T_POLY) continue;

      int netnameIndex;

      if (etch_only)
      {
         if (data->getGraphicClass() != GR_CLASS_ETCH) 
            continue;

         // get netname
         Attrib *attrib;

         if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(keyword, attrib)) continue; // no netname attribute

         if (attrib->getValueType() != VT_STRING) continue; // netnames are always strings

         netnameIndex = attrib->getStringValueIndex();
      }
      else
      {
         netnameIndex = 0;
      }

      // lookup cracklist
      CCrackList *cracklist;
      void *voidPtr;
      if (map.Lookup(netnameIndex, voidPtr)) // is there already a list of traces for this net
         cracklist = (CCrackList*)voidPtr;
      else // need to make a new datalist for traces of this net
      {
         cracklist = new CCrackList;
         map.SetAt(netnameIndex, cracklist);
      }

      // make CrackStruct
      CrackStruct *c = new CrackStruct;
      c->data = data;
      cracklist->AddTail(c);
      InsertExtents(c);
   }



   // loop nets in map
   CrackStruct *c1, *c2;
   CPoly *poly1, *poly2;
   POSITION crackPos1, crackPos2;

	// Determine job size
	long jobSize = 0;
   POSITION mapPos = map.GetStartPosition();
   while (mapPos)
   {
      WORD w;
      void *voidPtr;
      map.GetNextAssoc(mapPos, w, voidPtr);
      CCrackList *cracklist = (CCrackList*)voidPtr;
		jobSize += cracklist->GetCount();
	}

	// If the job is big enough and we have a progress bar, then use it
	bool trackingProgress = (jobSize > 1000) && (operationProgress != NULL);

#define PROGRESS_STYLE_ONE
	if (trackingProgress)
	{
#ifdef PROGRESS_STYLE_ONE
		operationProgress->updateStatus("Optimizing net traces",0);
#endif
      operationProgress->setLength(jobSize);
      operationProgress->updateProgress(0.);
	}

	long jobPosition = 0;
   mapPos = map.GetStartPosition();
   while (mapPos)
   {
      WORD w;
      void *voidPtr;
      map.GetNextAssoc(mapPos, w, voidPtr);
      CCrackList *cracklist = (CCrackList*)voidPtr;

#ifdef PROGRESS_STYLE_TWO
	if (trackingProgress)
	{
		CString msg;
		msg.Format("Optimizing trace polygons for net: %s", doc->ValueArray[w]);
		operationProgress->updateStatus(msg);
	}
#endif

#ifdef DEBUG_DUMP
t = t.GetCurrentTime();
t1 = t1.GetCurrentTime();
t2 = t2.GetCurrentTime();
CString debugNetname = doc->ValueArray[w];
fprintf(debug, "\n*** Net=%s at %s\n", doc->ValueArray[w], t.Format("%H:%M:%S"));
fprintf(debug, "*** Cracklist size %d at %s\n", cracklist->GetCount(), t.Format("%H:%M:%S"));
fflush(debug);
#endif

      crackPos1 = cracklist->GetHeadPosition();
      while (crackPos1 != NULL)
      {
         c1 = cracklist->GetNext(crackPos1);
			jobPosition++;
			if (trackingProgress)
			{
				//operationProgress->setLength(jobSize);
				operationProgress->updateProgress(jobPosition);
			}
#ifdef DEBUG_DUMPXXXXXXX
t = t.GetCurrentTime();
fprintf(debug, " (%s)  c1 Entity=%d at %s\n", debugNetname, c1->data->getEntityNumber(), t.Format("%H:%M"));
fflush(debug);
#endif

         crackPos2 = crackPos1; // don't need to compare 1 with 2 AND 2 with 1
         while (crackPos2 != NULL)
         {
            c2 = cracklist->GetNext(crackPos2);

#ifdef DEBUG_DUMP
counter0++;
if (counter0 == 1000000)
{
counter1++;
t2 = t2.GetCurrentTime();
ts = t2 - t1;
fprintf(debug, " (%s)  (%d) 1000000 processed (%d cracked) at %s (elapsed %s)\n", debugNetname, counter1, counter2, t2.Format("%H:%M:%S"), ts.Format("%H:%M:%S"));
fflush(debug);
t1 = t2;
counter0 = 0;
counter2 = 0;
}
#endif
         
            // only crack lines that are on same layer
            if (c1->data->getLayerIndex() != c2->data->getLayerIndex()) continue;

            // check extents
            if (!ExtentsTouch(c1, c2)) continue;

            // loop through polys of polystruct
            POSITION polyPos1, polyPos2;

            polyPos1 = c1->data->getPolyList()->GetHeadPosition();
            while (polyPos1 != NULL)
            {
               poly1 = c1->data->getPolyList()->GetNext(polyPos1);
   
               if (poly1->isClosed() || poly1->isFilled()) continue;
   
               polyPos2 = c2->data->getPolyList()->GetHeadPosition();
               while (polyPos2 != NULL)
               {
                  poly2 = c2->data->getPolyList()->GetNext(polyPos2);
   
                  if (poly2->isClosed() || poly2->isFilled()) continue;
               
                  CrackPolys(poly1, poly2);
#ifdef DEBUG_DUMP
counter2++;
#endif

               } // end loop polys of data2
            } // end loop polys of data1
         } // end loop data2 to compare to data1
         delete c1;
      } // end loop data1
      delete cracklist;
   } // end loop map of nets

#ifdef DEBUG_DUMP
t = t.GetCurrentTime();
fprintf(debug, "* done at %s\n\n", t.Format("%H:%M"));
fflush(debug);
#endif
} // end Crack()

/****************************************************************************
* void CrackPolys(CPoly *poly1, CPoly *poly2)
*
* DESCRIPTION:
*     --- adds points (cracks) at intersections of the two polys
*/  
void CrackPolys(CPoly *poly1, CPoly *poly2)
{
   CPnt *p1a, *p1b;
   POSITION pos1 = poly1->getPntList().GetHeadPosition();
   if (pos1 != NULL)
      p1b = poly1->getPntList().GetNext(pos1);
   while (pos1 != NULL)
   {
      p1a = p1b;
      p1b = poly1->getPntList().GetNext(pos1);

      CPnt *p2a, *p2b;
      POSITION pos2 = poly2->getPntList().GetHeadPosition();
      if (pos2 != NULL)
         p2b = poly2->getPntList().GetNext(pos2);
      while (pos2 != NULL)
      {
         p2a = p2b;
         p2b = poly2->getPntList().GetNext(pos2);

         // figure slopes and y-intercepts for special case (overlapping parallel lines -> possible multiple cracks)
         double m1, m2, b1, b2;
         BOOL Undefined1 = FALSE, Undefined2 = FALSE;
         BOOL SameLine;

         // line 1
         if (fabs(p1a->x - p1b->x) < SMALLNUMBER) 
            Undefined1 = TRUE;
         else
            m1 = (p1a->y - p1b->y) / (p1a->x - p1b->x);

         // line 2
         if (fabs(p2a->x - p2b->x) < SMALLNUMBER) 
            Undefined2 = TRUE;
         else
            m2 = (p2a->y - p2b->y) / (p2a->x - p2b->x);

         // On same infinite line?
         if (Undefined1 ^ Undefined2) // one undefined, one not
            SameLine = FALSE;
         else if (Undefined1 && Undefined2) // both undefined
            SameLine = (fabs(p1a->x - p2a->x) < SMALLNUMBER); // check x-intercept
         else if (fabs(m1 - m2) < ALMOSTZERO) // both have slope
         {
            b1 = p1a->y - m1 * p1a->x;
            b2 = p2a->y - m2 * p2a->x;
            SameLine = (fabs(b1 - b2) < ALMOSTZERO); // check y-intercept
         }
         else 
            SameLine = FALSE;

         if (SameLine) // special case (overlapping parallel lines -> possible multiple cracks)
         {
            double min1, max1, min2, max2;
            CPnt *min1pnt, *max1pnt, *min2pnt, *max2pnt;
            if (!Undefined1)
            {
               if (p1a->x <= p1b->x)
               {
                  min1pnt = p1a;
                  max1pnt = p1b;
                  min1 = p1a->x;
                  max1 = p1b->x;
               }
               else
               {
                  min1pnt = p1b;
                  max1pnt = p1a;
                  min1 = p1b->x;
                  max1 = p1a->x;
               }

               if (p2a->x <= p2b->x)
               {
                  min2pnt = p2a;
                  max2pnt = p2b;
                  min2 = p2a->x;
                  max2 = p2b->x;
               }
               else
               {
                  min2pnt = p2b;
                  max2pnt = p2a;
                  min2 = p2b->x;
                  max2 = p2a->x;
               }
            }
            else
            {
               if (p1a->y <= p1b->y)
               {
                  min1pnt = p1a;
                  max1pnt = p1b;
                  min1 = p1a->y;
                  max1 = p1b->y;
               }
               else
               {
                  min1pnt = p1b;
                  max1pnt = p1a;
                  min1 = p1b->y;
                  max1 = p1a->y;
               }

               if (p2a->y <= p2b->y)
               {
                  min2pnt = p2a;
                  max2pnt = p2b;
                  min2 = p2a->y;
                  max2 = p2b->y;
               }
               else
               {
                  min2pnt = p2b;
                  max2pnt = p2a;
                  min2 = p2b->y;
                  max2 = p2a->y;
               }
            }
            
            if (min1 > min2 + SMALLNUMBER && min1 < max2 - SMALLNUMBER) // crack line 2 at min1pnt
            {
               CPnt *newPnt = new CPnt;
               newPnt->x = (DbUnit)min1pnt->x;
               newPnt->y = (DbUnit)min1pnt->y;
               newPnt->bulge = (DbUnit)0.0;
               poly2->getPntList().InsertAfter(poly2->getPntList().Find(p2a), newPnt);
            }
            if (min2 > min1 + SMALLNUMBER && min2 < max1 - SMALLNUMBER) // crack line 1 at min2pnt
            {
               CPnt *newPnt = new CPnt;
               newPnt->x = (DbUnit)min2pnt->x;
               newPnt->y = (DbUnit)min2pnt->y;
               newPnt->bulge = (DbUnit)0.0;
               poly1->getPntList().InsertAfter(poly1->getPntList().Find(p1a), newPnt);
            }
            if (max1 < max2 - SMALLNUMBER && max1 > min2 + SMALLNUMBER) // crack line 2 at max1pnt
            {
               CPnt *newPnt = new CPnt;
               newPnt->x = (DbUnit)max1pnt->x;
               newPnt->y = (DbUnit)max1pnt->y;
               newPnt->bulge = (DbUnit)0.0;
               poly2->getPntList().InsertBefore(poly2->getPntList().Find(p2b), newPnt);
            }
            if (max2 < max1 - SMALLNUMBER && max2 > min1 + SMALLNUMBER) // crack line 1 at max2pnt
            {
               CPnt *newPnt = new CPnt;
               newPnt->x = (DbUnit)max2pnt->x;
               newPnt->y = (DbUnit)max2pnt->y;
               newPnt->bulge = (DbUnit)0.0;
               poly1->getPntList().InsertBefore(poly1->getPntList().Find(p1b), newPnt);
            }
         }
         else // normal case (one crack possible in each line)
         {
            CPnt intersection;
            int result;
            if (result = NeedToCrack(p1a, p1b, p2a, p2b, &intersection))
            {
               if (result & 1) // need to crack p1a, p1b
               {
                  CPnt *newPnt = new CPnt;
                  newPnt->x = (DbUnit)intersection.x;
                  newPnt->y = (DbUnit)intersection.y;
                  newPnt->bulge = (DbUnit)0.0;
                  poly1->getPntList().InsertAfter(poly1->getPntList().Find(p1a), newPnt);
   
                  // need to work on one segment at a time
                  pos1 = poly1->getPntList().Find(p1b);
                  p1b = newPnt;
               }
   
               if (result & 2) // need to crack p2a, p2b
               {
                  CPnt *newPnt = new CPnt;
                  newPnt->x = (DbUnit)intersection.x;
                  newPnt->y = (DbUnit)intersection.y;
                  newPnt->bulge = (DbUnit)0.0;
                  poly2->getPntList().InsertAfter(poly2->getPntList().Find(p2a), newPnt);
   
                  // need to work on one segment at a time
                  pos2 = poly2->getPntList().Find(p2b);
                  p2b = newPnt;
               }
            }
         }
      }
   }
}

/****************************************************************************
* int NeedToCrack(CPnt *p1a, CPnt *p1b, CPnt *p2a, CPnt *p2b, CPnt *intersection)
*
* DESCRIPTION:
*     --- decides if a crack is necessary
*
* PARAMETERS:
*     --- p1a, p1b, p2a, p2b : endpoints of lines
*     --- intersection : intersecting point if they intersect
*
* RETURNS:
*     --- 0 : No need to crack
*     --- 1 : need to crack line1 with intersection point
*     --- 2 : need to crack line2 with intersection point
*     --- 3 : need to crack both lines with intersection point
*/  
int NeedToCrack(CPnt *A1, CPnt *A2, CPnt *B1, CPnt *B2, CPnt *intersection)
{
   int result = 0;

   // do not crack on arcs
   if (fabs(A1->bulge) > SMALLNUMBER)  return 0;
   if (fabs(B1->bulge) > SMALLNUMBER)  return 0;

   Point2 a1, a2, b1, b2, pi;
   a1.x = A1->x;     a1.y = A1->y;
   a2.x = A2->x;     a2.y = A2->y;
   b1.x = B1->x;     b1.y = B1->y;
   b2.x = B2->x;     b2.y = B2->y;
   if (IntersectSeg2(&a1, &a2, &b1, &b2, &pi))
   {
      intersection->x = (DbUnit)pi.x;
      intersection->y = (DbUnit)pi.y;

      // if intersectin point is not on an endpoint, need to crack
      if ((fabs(a1.x - pi.x) > ALMOSTZERO || fabs(a1.y - pi.y) > ALMOSTZERO) && // p1a is different
         (fabs(a2.x - pi.x) > ALMOSTZERO || fabs(a2.y - pi.y) > ALMOSTZERO))  // and p1b is different
         result |= 1;
      if ((fabs(b1.x - pi.x) > ALMOSTZERO || fabs(b1.y - pi.y) > ALMOSTZERO) && // p1a is different
         (fabs(b2.x - pi.x) > ALMOSTZERO || fabs(b2.y - pi.y) > ALMOSTZERO))   // and p1b is different
         result |= 2;
   }
   return result;
}

/****************************************************************************
* void EliminateOverlappingTraces(CCEtoODBDoc *Doc, BlockStruct *block, int etch_only)
*
* PARAMETERS:
*     --- BlockStruct *block = block to eliminate traces of
*
* DESCRIPTION:
*     if etch_only then only polys with netnames are evaluated.
*
*/  
void EliminateOverlappingTraces(CCEtoODBDoc *Doc, BlockStruct *Block, int etch_only, double accuracy) 
{
#ifdef DEBUG_DUMP
CTime t;
t = t.GetCurrentTime();
fprintf(debug, "* In EliminateOverlappingTraces() for block \"%s\" at %s\n", Block->getName(), t.Format("%H:%M"));
fflush(debug);
#endif

   CMapWordToPtr map; // map NetName (index in ValueArray) to list of polys in that net
   WORD keyword = Doc->IsKeyWord(ATT_NETNAME, TRUE);

   // loop datas, put in correct list, set extents
   POSITION dataPos = Block->getDataList().GetHeadPosition();
   while (dataPos) // loop datas in the block
   {
      DataStruct *data = Block->getDataList().GetNext(dataPos);

      // only crack etch polys
      if (data->getDataType() != T_POLY) continue;

      int netnameIndex;

      if (etch_only)
      {
         if (data->getGraphicClass() != GR_CLASS_ETCH) 
            continue;

         // get netname
         Attrib *attrib;

         if (!data->getAttributesRef() || !data->getAttributesRef()->Lookup(keyword, attrib)) continue; // no netname attribute

         if (attrib->getValueType() != VT_STRING) continue; // netnames are always strings

         netnameIndex = attrib->getStringValueIndex();
      }
      else
      {
         netnameIndex = 0;
      }

      // lookup cracklist
      CCrackList *cracklist;
      void *voidPtr;
      if (map.Lookup(netnameIndex, voidPtr)) // is there already a list of traces for this net
         cracklist = (CCrackList*)voidPtr;
      else // need to make a new datalist for traces of this net
      {
         cracklist = new CCrackList;
         map.SetAt(netnameIndex, cracklist);
      }

      // make CrackStruct
      CrackStruct *c = new CrackStruct;
      c->data = data;
      cracklist->AddTail(c);
      InsertExtents(c);
   }


   // loop nets in map
   CrackStruct *c1, *c2;
   CPoly *poly1, *poly2;
   POSITION crackPos1, crackPos2;

   POSITION mapPos = map.GetStartPosition();
   while (mapPos)
   {
      WORD w;
      void *voidPtr;
      map.GetNextAssoc(mapPos, w, voidPtr);
      CCrackList *cracklist = (CCrackList*)voidPtr;

#ifdef DEBUG_DUMP
t = t.GetCurrentTime();
fprintf(debug, "  - Net=%s at %s\n", (w?Doc->ValueArray[w]:""), t.Format("%H:%M"));
fflush(debug);
#endif

      BOOL DoNotAdvance1, DoNotAdvance2;
      crackPos1 = cracklist->GetHeadPosition();
      while (crackPos1 != NULL)
      {
         c1 = cracklist->GetAt(crackPos1);

         DoNotAdvance1 = FALSE;

#ifdef DEBUG_DUMP
t = t.GetCurrentTime();
fprintf(debug, "    Entity=%d at %s\n", c1->data->getEntityNumber(), t.Format("%H:%M"));
fflush(debug);
#endif

         crackPos2 = crackPos1; // don't need to compare A with B AND B with A
         cracklist->GetNext(crackPos2);
         while (crackPos2 != NULL)
         {
            c2 = cracklist->GetAt(crackPos2);
         
            DoNotAdvance2 = FALSE;

            // only crack lines that are on same layer
            if (c1->data->getLayerIndex() != c2->data->getLayerIndex()) 
            {
               cracklist->GetNext(crackPos2);
               continue;
            }

            // check extents
            if (!ExtentsTouch(c1, c2)) 
            {
               cracklist->GetNext(crackPos2);
               continue;
            }

            // loop through polys of polystruct
            POSITION polyPos1, polyPos2;

            polyPos1 = c1->data->getPolyList()->GetHeadPosition();
            while (polyPos1 != NULL)
            {
               poly1 = c1->data->getPolyList()->GetAt(polyPos1);
   
               if (poly1->isClosed() || poly1->isFilled())
               {
                  poly1 = c1->data->getPolyList()->GetNext(polyPos1);
                  continue;
               }
   
               polyPos2 = c2->data->getPolyList()->GetHeadPosition();
               while (polyPos2 != NULL)
               {
                  poly2 = c2->data->getPolyList()->GetAt(polyPos2);
   
                  if (poly2->isClosed() || poly2->isFilled()) 
                  {
                     c2->data->getPolyList()->GetNext(polyPos2);
                     continue;
                  }

                  int result = EliminateOverlappingTracesInPolys(poly1, poly2, c1->data, c2->data, 
                     &Block->getDataList(), accuracy);              
                  switch (result)
                  {
                  case 1: // removed a poly from c1
                     {
                        if (c1->data->getPolyList()->isEmpty())
                        {
                           //if (c1->data->getAttributesRef())
                           //{
                           //   FreeAttribs(c1->data->getAttributesRef());
                           //   delete c1->data->getAttributesRef();
                           //}
                           Block->getDataList().RemoveAt(Block->getDataList().Find(c1->data));
                           //delete c1->data->getPolyList();
                           delete c1->data;

                           POSITION tempPos = crackPos1;
                           cracklist->GetNext(crackPos1);
                           cracklist->RemoveAt(tempPos);
                           DoNotAdvance1 = TRUE;
                           delete c1;
                        }

                        polyPos1 = NULL; // break out of loop to check these polys against each other
                        polyPos2 = NULL;
                        crackPos2 = NULL;
                        break;
                     }
                  case 2: // removed a poly from c2
                     {
                        if (c2->data->getPolyList()->isEmpty())
                        {
                           //if (c2->data->getAttributesRef())
                           //{
                           //   FreeAttribs(c2->data->getAttributesRef());
                           //   delete c2->data->getAttributesRef();
                           //}
                           Block->getDataList().RemoveAt(Block->getDataList().Find(c2->data));
                           //delete c2->data->getPolyList();
                           delete c2->data;

                           POSITION tempPos = crackPos2;
                           cracklist->GetNext(crackPos2);
                           cracklist->RemoveAt(tempPos);
                           DoNotAdvance2 = TRUE;
                           delete c2;
                        }

                        polyPos1 = NULL;
                        polyPos2 = NULL;
                        break;
                     }
                  case 0: // did not remove a poly
                     {
                        c2->data->getPolyList()->GetNext(polyPos2);
                        break;
                     }
                  }
               } // end loop polys of data2
               if (polyPos1 != NULL)
                  c1->data->getPolyList()->GetNext(polyPos1);
            } // end loop polys of data1
            if (crackPos2 != NULL && !DoNotAdvance2)
               cracklist->GetNext(crackPos2);
         } // end loop data2 to compare to data1
         if (!DoNotAdvance1)
            cracklist->GetNext(crackPos1);
      } // end loop data1

      // free this net
      crackPos1 = cracklist->GetHeadPosition();
      while (crackPos1 != NULL)
         delete cracklist->GetNext(crackPos1);
      delete cracklist;
   } // end loop map of nets

#ifdef DEBUG_DUMP
t = t.GetCurrentTime();
fprintf(debug, "* done at %s\n\n", t.Format("%H:%M"));
fclose(debug);
#endif
} // end EliminateOverlappingTraces()

/****************************************************************************
* FreeOnePoly()
*/
static void FreeOnePoly(CPoly *poly, DataStruct *data)
{
   // free pnts in poly
   POSITION pos = poly->getPntList().GetHeadPosition();
   while (pos != NULL)
      delete poly->getPntList().GetNext(pos);
   poly->getPntList().RemoveAll();

   // remove poly from polylist
   data->getPolyList()->deleteAt(data->getPolyList()->Find(poly));
   //delete poly;
}

/******************************************************************************
* EliminateSinglePointPolys
*/
void EliminateSinglePointPolys(CCEtoODBDoc *Doc) 
{
   for (int i=0; i<Doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = Doc->getBlockAt(i);

      if (block == NULL)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         POSITION tempPos = dataPos;
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_POLY)
            continue;
      
         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos)
         {
            CPoly *poly = data->getPolyList()->GetNext(polyPos);

            if (poly->getPntList().GetCount() < 2)
               FreeOnePoly(poly, data);
         }

         // does data have any polys left
         if (data->getPolyList()->GetCount() == 0)
         {
            // delete the complete data struct
            RemoveOneEntityFromDataList(Doc, &block->getDataList(), data, tempPos);
         }
      }
   }
}

/******************************************************************************
* EliminateSmallPolys
   this function eliminates vertex points in polys, which are smaller steps than accuracy
*/
void EliminateSmallPolys(CCEtoODBDoc *Doc, double accuracy) 
{
   for (int i=0; i<Doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = Doc->getBlockAt(i);

      if (block == NULL)
         continue;

      // loop datas, put in correct list, set extents
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_POLY)
            continue;
      
         POSITION polyPos = data->getPolyList()->GetHeadPosition();
         while (polyPos)
         {
            CPoly *poly = data->getPolyList()->GetNext(polyPos);

            int cnt = poly->getPntList().GetCount();
            if (cnt < 3)
               continue;   // must be part of a polyline !

            CPnt *lastpnt;
            int first = TRUE;
            POSITION pntPos = poly->getPntList().GetHeadPosition();
            while (pntPos)
            {
               POSITION tempPos = pntPos;
               CPnt *pnt = poly->getPntList().GetNext(pntPos);

               if (!first)
               {
                  if (fabs(lastpnt->x - pnt->x) < accuracy && fabs(lastpnt->y - pnt->y) < accuracy)
                  {
                     delete pnt;
                     poly->getPntList().RemoveAt(tempPos);
                     continue; // do not update lastpnt with pnt
                  }
               }
               first = FALSE;
               lastpnt = pnt;
            }
         }
      }
   }

   EliminateSinglePointPolys(Doc);
}


/****************************************************************************
*
*/  
void BreakSpikePolys(BlockStruct *Block) 
{
   CString logFilePath = GetLogfilePath("PintoPin.log");
   CStdioFileWriteFormat reportWriteFormat(logFilePath);

   if (Block->getCamCadData() != NULL)
   {
      CCamCadData& camCadData = *(Block->getCamCadData());

      POSITION dataPos = Block->getDataList().GetHeadPosition();
      while (dataPos != NULL) 
      {
         DataStruct *data = Block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_POLY) continue;
         
         CPoly *poly = data->getPolyList()->GetHead();
         if (poly->getPntList().GetCount() < 3) continue;

         //skip over polys that have Closed Filled flags
         if ((poly->isFilled() && poly->isClosed())
            //To avoid memory overhead, skip over polys that have over 10000 points 
            || poly->getPntList().GetCount() > 10000 )  
         {
            POSITION pos = poly->getPntList().GetHeadPosition();
            CPnt *curpos = poly->getPntList().GetAt(pos);
            reportWriteFormat.writef("Warning: %s Poly at (%f,%f,%d) has %d points is skipped\n",
               (poly->isFilled() && poly->isClosed())?"Closed and Filled":"",curpos->x,curpos->y,
               data->getLayerIndex(),poly->getPntList().GetCount());
            continue;
         }         

         // do not deal with a circle
         double cx, cy, rad;
         if (PolyIsCircle(poly, &cx, &cy, &rad))   continue;

         CPnt *prevPnt, *midPnt, *nextPnt;
         POSITION pos = poly->getPntList().GetHeadPosition();
         midPnt = poly->getPntList().GetNext(pos);
         nextPnt = poly->getPntList().GetNext(pos);
         BOOL Broke = FALSE;
         while (pos && !Broke)
         {
            prevPnt = midPnt;
            midPnt = nextPnt;
            nextPnt = poly->getPntList().GetAt(pos);

            if (fabs(prevPnt->x - nextPnt->x) < SMALLNUMBER && fabs(prevPnt->y - nextPnt->y) < SMALLNUMBER)
            {
               poly->setFilled(false);
               poly->setClosed(false);
               DataStruct *newData = camCadData.getNewDataStruct(*data);
               Block->getDataList().AddTail(newData);

               // CopyEntity makes a copy of the polylist, so 2 copies of all points, so remove some points from one and other points from other
               POSITION tempPos = pos;
               poly->getPntList().GetPrev(tempPos);
               while (poly->getPntList().GetTailPosition() != tempPos)
                  delete poly->getPntList().RemoveTail();

               CPoly *newPoly = newData->getPolyList()->GetHead();
               for (int i=1; i<poly->getPntList().GetCount(); i++)
                  delete newPoly->getPntList().RemoveHead();

               Broke = TRUE;
               continue;
            }

            poly->getPntList().GetNext(pos);
         }
      }
   }

   reportWriteFormat.close();

}

/****************************************************************************
*
*/  
void JoinLinePolys(BlockStruct *Block, double accuracy) 
{
   POSITION dataPos = Block->getDataList().GetHeadPosition();
   while (dataPos != NULL) 
   {
      DataStruct *data = Block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_POLY) continue;
      
      CPoly *poly = data->getPolyList()->GetHead();
      if (poly->getPntList().GetCount() < 3) continue;

      // do not deal with a circle
      double cx, cy, rad;
      if (PolyIsCircle(poly, &cx, &cy, &rad))   continue;

      CPnt *prevPnt, *midPnt, *nextPnt;
      POSITION pos = poly->getPntList().GetHeadPosition();
      midPnt = poly->getPntList().GetNext(pos);
      nextPnt = poly->getPntList().GetNext(pos);
      while (pos)
      {
         prevPnt = midPnt;
         midPnt = nextPnt;
         nextPnt = poly->getPntList().GetAt(pos);

         if (fabs(prevPnt->bulge) > ALMOSTZERO) continue;
         if (fabs(midPnt->bulge) > ALMOSTZERO)  continue;
         if (fabs(nextPnt->bulge) > ALMOSTZERO) continue;

         /*** check line slopes ***/
         double m1, m2;
         BOOL Undefined1 = FALSE, Undefined2 = FALSE;

         // line 1
         if (fabs(prevPnt->x - midPnt->x) < accuracy) 
            Undefined1 = TRUE;
         else
            m1 = (prevPnt->y - midPnt->y) / (prevPnt->x - midPnt->x);

         // line 2
         if (fabs(midPnt->x - nextPnt->x) < accuracy) 
            Undefined2 = TRUE;
         else
            m2 = (midPnt->y - nextPnt->y) / (midPnt->x - nextPnt->x);

         // compare
         if (Undefined1 ^ Undefined2) // one undefined slope and one not
            continue;

         if (!(Undefined1 && Undefined2)) // if not both undefined slopes
         {
            if (fabs(m1 - m2) > ALMOSTZERO) // if different slopes
               continue;
         }
         // kill mid point


      }
   }
}

/****************************************************************************
* int EliminateOverlappingTracesInPolys(CPoly *poly1, CPoly *poly2, 
      DataStruct *data1, DataStruct *data2, CDataList *DataList)
*
* DESCRIPTION:
* 
* PARAMETERS:
*     --- data1, data2 - used when breaking a poly in two
* RETURNS:
*     --- 0 Broke a poly but did not need to remove it
*     --- 1 Removes poly1
*     --- 2 Removes poly2
*/  
int EliminateOverlappingTracesInPolys(CPoly *poly1, CPoly *poly2, 
                              DataStruct *data1, DataStruct *data2, CDataList *DataList,
                              double accuracy)
{
   CPnt *p1a, *p1b;
   POSITION pos1 = poly1->getPntList().GetHeadPosition();
   if (pos1 != NULL)
      p1b = poly1->getPntList().GetNext(pos1);
   while (pos1 != NULL)
   {
      p1a = p1b;
      p1b = poly1->getPntList().GetAt(pos1);

      CPnt *p2a, *p2b;
      POSITION pos2 = poly2->getPntList().GetHeadPosition();
      if (pos2 != NULL)
         p2b = poly2->getPntList().GetNext(pos2);
      while (pos2 != NULL)
      {
         p2a = p2b;
         p2b = poly2->getPntList().GetAt(pos2);

         int result = SegmentsOverlap(p1a->x, p1a->y, p1b->x, p1b->y, p2a->x, p2a->y, p2b->x, p2b->y, accuracy);
         if (result == OVERLAP_1in2)
         {
            if (poly1->getPntList().GetCount() < 3)
            {
               FreeOnePoly(poly1, data1);
               return 1;
            }
            else
            {
               RemovePolySegment(poly1, &pos1, data1);
               break; // removed this segment, so jump out of this to check next segment
            }
         }
         if (result == OVERLAP_2in1)
         {
            if (poly2->getPntList().GetCount() < 3)
            {
               FreeOnePoly(poly2, data2);
               return 2;
            }
            else
            {
               RemovePolySegment(poly2, &pos2, data2);
            }
         }

         if (pos2 != NULL)
            poly2->getPntList().GetNext(pos2); 
      }

      if (pos1 != NULL)
         poly1->getPntList().GetNext(pos1); 
   }

   return 0;
}

/****************************************************************************
* void BreakPoly(CPoly *poly, POSITION pos, DataStruct *data)
*
* DESCRIPTION:
*     --- removes a duplicate segment breaking poly
*
* PARAMETERS:
*     --- 
*     --- data - used when breaking a poly in two
*/  
void BreakPoly(CPoly *poly, POSITION pos, DataStruct *data)
{
   CPoly *newPoly = new CPoly;
   data->getPolyList()->AddTail(newPoly);
   newPoly->setHatchLine(false);
   newPoly->setHidden(false);
   newPoly->setClosed(false);
   newPoly->setFilled(false);
   newPoly->setVoid(false);
   newPoly->setThermalLine(false);
   newPoly->setFloodBoundary(false);
   newPoly->setWidthIndex(poly->getWidthIndex());

   // break poly
   POSITION tempPos;
   while (pos != NULL)
   {
      tempPos = pos;
      newPoly->getPntList().AddHead(poly->getPntList().GetNext(pos));
      poly->getPntList().RemoveAt(tempPos);
   }
}

/****************************************************************************
* void RemovePolySegment(CPoly *poly, POSITION *pos, DataStruct *data)
*
* DESCRIPTION:
*     --- correctly removes a duplicate segment (breaks poly if necessary)
*
* PARAMETERS:
*     --- 
*     --- data - used when breaking a poly in two
*/  
void RemovePolySegment(CPoly *poly, POSITION *pos, DataStruct *data)
{
   // fix poly
   poly->setClosed(false);
   poly->setFilled(false);

   // if last segment
   if (*pos == poly->getPntList().GetTailPosition())
   {
      delete poly->getPntList().GetTail();
      poly->getPntList().RemoveTail();
      *pos = NULL;
      return;
   }

   // if first segment
   POSITION tempPos = *pos;
   poly->getPntList().GetPrev(tempPos);
   if (tempPos == poly->getPntList().GetHeadPosition())
   {
      delete poly->getPntList().GetHead();
      poly->getPntList().RemoveHead();
      return;
   }

   // break poly to two polys
   BreakPoly(poly, *pos, data);
   *pos = NULL;
}

/****************************************************************************/
void CCEtoODBDoc::OnHiglightSelectedNet() 
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductPrint))
   {
      ErrorAccess("You do not have a License for Query or Edit!");
      return;
   }*/

   // if nothing selected
   if (SelectList.IsEmpty())
      return;

   SelectStruct *s = SelectList.GetHead();

   int kw = IsKeyWord(ATT_NETNAME, FALSE);

   // if no "NetName"
   Attrib *attrib;

   if (!s->getData()->getAttributesRef() || !s->getData()->getAttributesRef()->Lookup(kw, attrib))
      return;

   // not a string value
   if (attrib->getValueType() != VT_STRING)
      return;

   HighlightedNetsMap.SetAt(attrib->getStringValueIndex(), RGB(255, 255, 255));
   HighlightByAttrib(FALSE, kw, VT_STRING, attrib->getStringValueIndex());

   // mark net as highlighted
   FileStruct *file = Find_File(s->filenum);
   if (file == NULL) return;
   NetStruct *net;
   BOOL found = FALSE;

   POSITION pos = file->getNetList().GetHeadPosition();
   while (pos != NULL)
   {
      net = file->getNetList().GetNext(pos);

      if (attrib->getStringValueIndex() == RegisterValue(net->getNetName()))
      {
         net->setHighlighted(true);
         found = TRUE;
         break;
      }
   }
   if (!found) return;

	OnSelectNets();

	// update SelectNets dialog
   if (selectNetsDlg)
      selectNetsDlg->MarkNet(net, RGB(255,255,255));
}

/******************************************************************************
* find_net
*/
NetStruct *find_net(FileStruct *file, const char *net_name)
{
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);
      if (!net->getNetName().Compare(net_name))
         return net;
   }

   return NULL;
}

/******************************************************************************
* add_net
*/
NetStruct *add_net(FileStruct *file, const char *net_name)
{
   NetStruct* net = file->getNetList().addNet(net_name);

   //NetStruct *net = find_net(file, net_name);
   //if (net)
   //   return net;

   //net = new NetStruct;
   //net->setEntityNumber(CEntityNumber::allocate());
   //net->getAttributesRef() = NULL;
   //net->setNetName(net_name);
   //net->setFlags(0);
   //net->setHighlighted(false);
   //file->getNetList().AddTail(net);

   return net;
}

/****************************************************************************/
/*
*/                    
CompPinStruct *add_comppin(FileStruct *fl,
                        NetStruct *n, const char *comp, const char *pin)
{
   if (n == NULL) return NULL;
   if (strlen(comp) == 0)     return NULL;
   if (strlen(pin) == 0)      return NULL;

   CompPinStruct* compPinStruct = n->addCompPin(comp,pin);

   //CompPinStruct *c = new CompPinStruct;
   //c->setEntityNumber(CEntityNumber::allocate());
   //c->getAttributesRef() = NULL;
   //c->setOrigin(0.,0.);
   //c->setRotationRadians(0.);  
   //c->setMirror(0); 
   //c->setPinCoordinatesComplete(false);   
   //c->setVisible(VISIBLE_BOTH);
   //
   //c->setPadstackBlockNumber(-1);

   //n->getCompPinList().AddTail(c);
   //c->setRefDes(comp);
   //c->setPinName(pin);

   return compPinStruct;
}

/******************************************************************************
* GenerateMissingTypes
*/
void GenerateMissingTypes(CCEtoODBDoc *doc, FileStruct *file)
{
   WORD keyword = doc->IsKeyWord(ATT_TYPELISTLINK, 0);

   int unknownTypeNum = 1;

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
         continue;

      Attrib *attrib;

      if (data->getAttributesRef() && data->getAttributesRef()->Lookup(keyword, attrib))
      {
         if (FindType(file, attrib->getStringValue()))
            continue;
      }

      BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());

      TypeStruct *type = AddType(file, block->getName());
      type->setBlockNumber( data->getInsert()->getBlockNumber());

      doc->SetAttrib(&data->getAttributesRef(), keyword, VT_STRING, block->getNameRef().GetBuffer(0), attributeUpdateOverwrite, NULL);
   }

}

/******************************************************************************
* AddType
*/                 
TypeStruct *AddType(FileStruct *file, CString typeName)
{
   TypeStruct* typeStruct = file->getTypeList().findType(typeName);

   if (typeStruct == NULL)
   {
      typeStruct = file->getTypeList().addType(typeName);

      //typeStruct->setEntityNumber(CEntityNumber::allocate());
      typeStruct->setBlockNumber( -1); // no name assigned
   }

   //POSITION typePos = file->getTypeList().GetHeadPosition();
   //while (typePos)
   //{
   //   TypeStruct *type = file->getTypeList().GetNext(typePos);

   //   if (!type->getName().Compare(typeName))
   //      return type;
   //}
   //
   //TypeStruct *type = new TypeStruct;
   //type->setEntityNumber(CEntityNumber::allocate());
   //type->getAttributesRef() = NULL;
   //type->setName(typeName);
   //type->setBlockNumber( -1); // no name assigned

   //file->getTypeList().AddTail(type);

   return typeStruct;
}

/******************************************************************************
* FindType
*/
TypeStruct *FindType(FileStruct *file, CString typeName)
{
   POSITION typePos = file->getTypeList().GetHeadPosition();
   while (typePos)
   {
      TypeStruct *type = file->getTypeList().GetNext(typePos);

      if (!type->getName().Compare(typeName))
         return type;
   }
   
   return NULL;
}

/****************************************************************************/
/*
   after a type is assigned to a geom, the typepinlist gets filled with the physical pin name.

   return number of pins found

*/    
int update_typepin_from_geompin(TypeStruct *tl, int blocknr)
{
   int   pins_found = 0;

   ErrorMessage("update_typepin_from_geompin not implemented");

   return pins_found;
}

/****************************************************************************/
/*
   get a physical type pin pointer 
*/    
TypePinStruct *add_typepin(TypeStruct *tl, const char *pin)
{
   TypePinStruct *tp = NULL;

   ErrorMessage("add_typepin not implemented");

   return tp;
}

//
//
CompPinStruct *check_add_comppin(const char *c, const char *p, FileStruct *f)
{
   // now here loop thru the netlist, if found return
   // otherwise add a NC net. 
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   netPos = f->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = f->getNetList().GetNext(netPos);
      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         // here comp/pin found = all done
         if (!strcmp(compPin->getRefDes(),c) && !strcmp(compPin->getPinName(),p)) 
            return compPin;
      }
   }

   // here add comppin into UNUSED_NET
   NetStruct *n = add_net(f, NET_UNUSED_PINS);
   n->setFlagBits(NETFLAG_UNUSEDNET);
   return add_comppin(f, n, c,p);
}

// Just test if a pin already exist in the netlist !
//
CompPinStruct *test_add_comppin(const char *c, const char *p, FileStruct *f)
{
   // now here loop thru the netlist, if found return
   // otherwise add a NC net. 
   NetStruct *net;
   CompPinStruct *compPin;
   POSITION compPinPos, netPos;
   
   netPos = f->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = f->getNetList().GetNext(netPos);
      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         // here comp/pin found = all done
         if (!strcmp(compPin->getRefDes(),c) && !strcmp(compPin->getPinName(),p)) 
            return compPin;
      }
   }

   return NULL;
}

void CCEtoODBDoc::generatePinLocations(bool displayLogFileFlag, COperationProgress* progress) 
{
   CString logFilePath = GetLogfilePath("Pinloc.log");
   remove(logFilePath);

   CStdioFileWriteFormat reportWriteFormat(logFilePath);

   int errorCount = getCamCadData().generatePinLocations(&reportWriteFormat,progress);

   reportWriteFormat.close();

   if (displayLogFileFlag && errorCount > 0)
   {
      Notepad(logFilePath);
   }
}

/*********************************************************************
* OnGeneratePinloc
*/
void CCEtoODBDoc::OnGeneratePinloc() 
{
   generatePinLocations();
}

//--------------------------------------------------------------
// If a PANEL is used, all files contained in this PANEL must 
// also be used.
// loop file (for PANELS)
// get np->getInsert()-.num
// loop file1 all file and find the corresponding PCB files
// and switch show on.
//
void switch_on_pcbfiles_used_in_panels(CCEtoODBDoc *doc, int onoff)
{
   FileStruct *file, *file1;
   DataStruct *np;

//BLOCKTYPE_PANEL
//BLOCKTYPE_PCB

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlockType() == BLOCKTYPE_PANEL)
      {
         POSITION pos1 = file->getBlock()->getDataList().GetHeadPosition();
         while (pos1 != NULL)
         {
            np = file->getBlock()->getDataList().GetNext(pos1);
            if (np->getDataType() != T_INSERT)        
               continue;
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCB)   
            {
               // find file to switch on
               POSITION pos2 = doc->getFileList().GetHeadPosition();
               while (pos2 != NULL)
               {
                  file1 = doc->getFileList().GetNext(pos2);
                  if (file1->getBlock()->getBlockNumber() == np->getInsert()->getBlockNumber())
                  {
                     file1->setShow(onoff);
                  }
               }
            }
         }
      }
   }

   return;
}

//--------------------------------------------------------------
// this function updates the netlist with the NC non connect net.
// it loop through all component (INSERTPCBCOMPONENT) and inserts
// all INSERTPIN from 
void generate_NCnet(CCEtoODBDoc *doc, FileStruct *f,COperationProgress* progress)
{
   if (doc != NULL && f != NULL)
   {
      doc->getCamCadData().generateNoConnectNets(f->getFileNumber(),progress);
   }

   //// loop thru the PCB file
   //DataStruct *np;
   //const CDataList& fileDataList = f->getBlock()->getDataList();

   //if (progress != NULL)
   //{
   //   progress->setLength(fileDataList.GetCount());
   //}
   //
   //for (POSITION pos = fileDataList.GetHeadPosition();pos != NULL;)
   //{
   //   np = fileDataList.GetNext(pos);

   //   if (progress != NULL)
   //   {
   //      progress->incrementProgress();
   //   }
   //   
   //   if (np->getDataType() != T_INSERT)        continue;

   //   switch(np->getDataType())
   //   {
   //      case T_INSERT:
   //      {
   //         if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT ||
   //             np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT )
   //         {
   //            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
   //            CString compname = np->getInsert()->getRefname();

   //            // here loop thru block and find INSERTPIN
   //            DataStruct *bp;
   //            POSITION bpos = block->getDataList().GetHeadPosition();
   //            while (bpos != NULL)
   //            {
   //               bp = block->getDataList().GetNext(bpos);
   //   
   //               if (bp->getDataType() != T_INSERT)        continue;

   //               switch (bp->getDataType())
   //               {
   //                  case T_INSERT:
   //                  {
   //                     if (bp->getInsert()->getInsertType() == INSERTTYPE_PIN)
   //                     {
   //                        // here is a pin
   //                        CString pinname = bp->getInsert()->getRefname();
   //                        check_add_comppin(compname,pinname,f);
   //                     }
   //                  }
   //               }
   //            } // end loop thru block
   //         }
   //      } // case INSERT
   //      break;
   //   } // end switch
   //} // end 

   //return;     
}

//------------------------------------------------------------
void delete_visible_root_graphic(CCEtoODBDoc *doc,bool componentsFlag,bool graphicsFlag)
{
   // loop thru the PCB file   
   for (POSITION filePos = doc->getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);
      bool regenFlag = false;

      if (!file->isShown())
      {
         continue;
      }
      
      for (POSITION pos = file->getBlock()->getDataList().GetHeadPosition();pos != NULL;)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(pos);

         if (data->getDataType() != dataTypeInsert)
         {  
            if (!graphicsFlag) continue;

            if (!doc->isGraphicClassVisible(data->getGraphicClass()))
               continue;

            // only delete visible graphic.
            if (!doc->get_layer_visible(data->getLayerIndex(), 0)) 
               continue;

            if (data->getGraphicClass() == graphicClassEtch) // never delete etch.
               continue;

            if (data->getGraphicClass() == graphicClassBoardOutline) // never delete board.
               continue;

            if (data->getGraphicClass() == graphicClassPanelOutline) // never delete board.
               continue;

            if (data->getGraphicClass() == graphicClassBoardGeometry) // never delete board.
               continue;

            if (data->getGraphicClass() == graphicClassMillingPath) // never delete board.
               continue;

            RemoveOneEntityFromDataList(doc, &file->getBlock()->getDataList(), data);

            regenFlag = true;
         }
         else
         {
            if (!componentsFlag) continue;   // do not go into hierachies.

            if (data->getInsert()->getInsertType() != insertTypeGenericComponent &&
                data->getInsert()->getInsertType() != insertTypeUnknown )
               continue;

            if (!doc->isInsertTypeVisible(data->getInsert()->getInsertType()))
               continue;

            RemoveOneEntityFromDataList(doc, &file->getBlock()->getDataList(), data);

            regenFlag = true;
         }
      }  // data loop

      if (regenFlag)
      {
         file->getBlock()->resetExtent();
      }
   }  // file loop

   return;     
}

//--------------------------------------------------------------
void delete_traces(CCEtoODBDoc *doc, int traces, int vias)
{
   // loop throu the PCB file
   DataStruct *np;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      POSITION bpos = block->getDataList().GetHeadPosition();
      while (bpos != NULL)
      {
         np = block->getDataList().GetNext(bpos);

         if (np->getDataType() != T_INSERT)
         {  
            if (traces == 0)                             continue;   // do not delete traces
            if (np->getGraphicClass() != GR_CLASS_ETCH)      continue;
            if (traces == 1)
            {
               // delete all
               RemoveOneEntityFromDataList(doc, &block->getDataList(), np);
            }
            else
            {
               // here check if it has a net attribute
               Attrib *a =  get_attvalue(np->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE));
               if (a)
               {
                  RemoveOneEntityFromDataList(doc, &block->getDataList(), np);
               }
            }
         }
         else
         {
            // must be a via
            if (vias == 0)                               continue;
            if (np->getInsert()->getInsertType() != INSERTTYPE_VIA)   continue;

            if (vias == 1)
            {
               RemoveOneEntityFromDataList(doc, &block->getDataList(), np);
            }
            else
            {
               // here check if it has a net attribute
               Attrib *a =  get_attvalue(np->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, TRUE));
               if (a)
               {
                  RemoveOneEntityFromDataList(doc, &block->getDataList(), np);
               }
            }
         }
      }  // while throu the datalist
   }  // while all blocks
   return;     
}

/******************************************************************************
* cnt_COMPONENTPinData
*/
static void cnt_COMPONENTPinData(CCEtoODBDoc *doc, CDataList *DataList, int *totalPinCount, int *smdPinCount)
{
   *totalPinCount = 0;
   *smdPinCount = 0;

	WORD smdShapeKw = doc->RegisterKeyWord(ATT_SMDSHAPE, 0, VT_NONE);
	WORD technologyKw = doc->RegisterKeyWord(ATT_TECHNOLOGY, 0, VT_STRING);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (block == NULL)   
         break;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PIN || 
         data->getInsert()->getInsertType() == insertTypeDiePin)
      {
         (*totalPinCount)++;

			Attrib* attrib = NULL;
			if (block->getAttributes() == NULL)
			{
				(*smdPinCount)++;
			}
			else
			{
				if (block->getAttributes()->Lookup(technologyKw, attrib))
				{
					CString technology= attrib->getStringValue();
					if (technology.CompareNoCase("THRU") != 0)
						(*smdPinCount)++;
				}
				else if (block->getAttributes()->Lookup(smdShapeKw, attrib))
				{
					(*smdPinCount)++;
				}
			}
      }
   }
}

/******************************************************************************
* update_smdGEOMETRYData
*/
bool update_smdGEOMETRYData(CCEtoODBDoc& doc,BlockStruct& block,int smdRule)
{
   bool retval = false;

   int smdKeyword  = doc.getStandardAttributeKeywordIndex(standardAttributeSmd);
   int techKeyword = doc.getStandardAttributeKeywordIndex(standardAttributeTechnology);

   while (true)
   {
      if (block.getBlockType() != BLOCKTYPE_PCBCOMPONENT && block.getBlockType() != BLOCKTYPE_TESTPOINT &&
         block.getBlockType() != blockTypeDie)
         break;

      // already flagged as SMD skip SMD test
	   Attrib *attrib = NULL;

      if (block.getAttributesRef() && (block.getAttributesRef()->Lookup((WORD)smdKeyword, attrib) || block.getAttributesRef()->Lookup((WORD)techKeyword, attrib)))
	   {
		   CString value = attrib->getStringValue();

		   if (!value.CompareNoCase("SMD"))
			   break;
	   }

      int totalPinCount = 0;
      int smdPinCount = 0;
      cnt_COMPONENTPinData(&doc, &block.getDataList(), &totalPinCount, &smdPinCount);

      if (totalPinCount == 0)
         break;

      if ( (smdRule == 1 && totalPinCount == smdPinCount) || // all
           (smdRule == 2 && smdPinCount > totalPinCount/2) || // most
           (smdRule == 3 && smdPinCount) ) // one
	   {
		   CString value = "SMD";
         doc.SetAttrib(&block.getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
         doc.SetAttrib(&block.getAttributesRef(), smdKeyword, VT_NONE, NULL, attributeUpdateOverwrite, NULL);
	   }
	   else
	   {
		   CString value = "THRU";
         doc.SetAttrib(&block.getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
	   }

      retval = true;
      break;
   }

   return retval;
}

static void update_smdGEOMETRYData(CCEtoODBDoc *doc, int smdRule,COperationProgress* progress)
{
   if (smdRule == 0 || doc == NULL)
      return;

   if (progress != NULL)
   {
      progress->updateStatus("Updating smd gemometry data",doc->getMaxBlockIndex());
   }

   for (int i=0;i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct* block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      update_smdGEOMETRYData(*doc,*block,smdRule);
   }
}

/******************************************************************************
* update_smdCOMPONENTData
*/
static void update_smdCOMPONENTData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList,
   int smdrule, DataStruct *compData,COperationProgress* operationProgress)
{
   static int callLevel = 0;
   int smdKeyword = doc->IsKeyWord(ATT_SMDSHAPE, 0);
	int techKeyword = doc->IsKeyWord(ATT_TECHNOLOGY, 0);

   callLevel++;

   if (operationProgress != NULL && callLevel == 1)
   {
      CString message;
      message.Format("Updating smd component data for file %d",file->getFileNumber());
      operationProgress->updateStatus(message,DataList->GetCount());
   }

   // a component can only be SMD if the geometry is SMD.   
   for (POSITION dataPos = DataList->GetHeadPosition();dataPos != NULL;)
   {
      if (operationProgress != NULL && callLevel == 1)
      {
         operationProgress->incrementProgress();
      }

      DataStruct *data = DataList->GetNext(dataPos);

      if (data == NULL || data->getDataType() != T_INSERT)
         continue;

		InsertStruct *insert = data->getInsert();

		if (insert == NULL)
			continue;

		BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

		if (block == NULL)
			continue;

		if (insert->getInsertType() == insertTypePcb)
		{
			update_smdCOMPONENTData(doc, file, &block->getDataList(), smdrule, NULL,operationProgress);
		}
		else if (insert->getInsertType() == insertTypePcbComponent || insert->getInsertType() == insertTypeTestPoint ||
					insert->getInsertType() == insertTypePin || insert->getInsertType() == insertTypeVia || 
               insert->getInsertType() == insertTypeDie || insert->getInsertType() == insertTypeDiePin ||
               insert->getInsertType() == insertTypeBondPad)
		{
         // Add technology to Bondpad
         if(insert->getInsertType() == insertTypeBondPad)
         {
            update_smdCOMPONENTData(doc, file, &block->getDataList(), smdrule, NULL);
            block = GetBondPadPadstackBlock(doc->getCamCadData(), block);
         }

			// remove attribute, because the update could have changed a previous attribute SMD to a thru comp
			Attrib *attrib = NULL;

			if (data->getAttributesRef() && data->getAttributesRef()->Lookup((WORD)smdKeyword, attrib))
				RemoveAttrib(smdKeyword, &data->getAttributesRef());
	         
			Attrib *smdAttrib = NULL;
			Attrib *technologyAttrib = NULL;

			if (block->getAttributesRef())
			{
				block->getAttributesRef()->Lookup((WORD)techKeyword, technologyAttrib);
				block->getAttributesRef()->Lookup((WORD)smdKeyword, smdAttrib);
			}

			if (technologyAttrib)
			{
				// Check TECHNOLOGY first
				CString value = technologyAttrib->getStringValue(); 
				doc->SetAttrib(&data->getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);

				if (value.CompareNoCase("SMD") == 0)
					doc->SetAttrib(&data->getAttributesRef(), smdKeyword, VT_NONE, NULL, attributeUpdateOverwrite, NULL);
			}
			else if (smdAttrib)
			{
				// Then check SMD
				CString value = "SMD";
				doc->SetAttrib(&data->getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
				doc->SetAttrib(&data->getAttributesRef(), smdKeyword, VT_NONE, NULL, attributeUpdateOverwrite, NULL);
			}
			else
			{
				// Otherwise, it is THRU
				CString value = "THRU";
				doc->SetAttrib(&data->getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
			}

			if (insert->getInsertType() == insertTypeTestPoint)
			{
				update_smdCOMPONENTData(doc, file, &block->getDataList(), smdrule, NULL);
			}
			else if (insert->getInsertType() == insertTypePcbComponent || insert->getInsertType() == insertTypeDie)
			{
				update_smdCOMPONENTData(doc, file, &block->getDataList(), smdrule, data);
			}
			else if (insert->getInsertType() == insertTypePin || insert->getInsertType() == insertTypeDiePin)
			{
				// Update the comppin in the netlist
				if (compData != NULL && compData->getInsert() != NULL && 
               (compData->getInsert()->getInsertType() == insertTypePcbComponent || compData->getInsert()->getInsertType() == insertTypeDie))
				{
					CString comp = compData->getInsert()->getRefname();
					CString pin = data->getInsert()->getRefname();
					NetStruct *net = NULL;
					CompPinStruct *comppin = FindCompPin(file, comp, pin, &net);

					if (comppin == NULL)
						continue;

					Attrib *attrib = NULL;

					if (comppin->getAttributesRef() && comppin->getAttributesRef()->Lookup((WORD)smdKeyword, attrib))
						RemoveAttrib(smdKeyword, &comppin->getAttributesRef());

					if (technologyAttrib)
					{
						// Check TECHNOLOGY first
						CString value = technologyAttrib->getStringValue(); 
						doc->SetAttrib(&comppin->getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);

						if (value.CompareNoCase("SMD") == 0)
							doc->SetAttrib(&comppin->getAttributesRef(), smdKeyword, VT_NONE, NULL, attributeUpdateOverwrite, NULL);
					}
					else if (smdAttrib)
					{
						// Then check SMD
						CString value = "SMD";
						doc->SetAttrib(&comppin->getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
						doc->SetAttrib(&comppin->getAttributesRef(), smdKeyword, VT_NONE, NULL, attributeUpdateOverwrite, NULL);
					}
					else
					{
						// Otherwise, it is THRU
						CString value = "THRU";
						doc->SetAttrib(&comppin->getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
					}
				}
			}
		}
   }

   callLevel--;
}

/******************************************************************************
* color_ComponentNotLoadedData
*/
void color_ComponentNotLoadedData(CCEtoODBDoc *doc, FileStruct *file, int red, int green, int blue)
{
	
   POSITION pos = file->getBlock()->getHeadDataInsertPosition();
   while (pos != NULL)
   {
      DataStruct* data = file->getBlock()->getNextDataInsert(pos);
      data->setColorOverride(false);
			
		Attrib* attrib = NULL;
      if (attrib = is_attvalue(doc, data->getAttributesRef(), ATT_LOADED, 1))   
      {              
         CString tmp = get_attvalue_string(doc, attrib);
         if (tmp.CompareNoCase("FALSE") == 0)
         {
            data->setOverrideColor(RGB(red, green, blue));
            data->setColorOverride(true);
         }
      }
   }
}

/*****************************************************************************/
/*
*/
void set_ComponentLoadedData(CCEtoODBDoc *doc, FileStruct *file)
{
   DataStruct *np;
	CDataList *DataList = &(file->getBlock()->getDataList());

// a component can only be SMD if the geometry is SMD.
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               doc->SetUnknownAttrib(&np->getAttributesRef(), ATT_LOADED, "TRUE", attributeUpdateOverwrite, NULL);
            }
         } // case INSERT
         break;
      } // end switch
   } // end update_NotinBomData */
   return;
}

/*****************************************************************************/
/*
*/
void set_ComponentNotLoadedData(CCEtoODBDoc *doc, FileStruct *file)
{
   DataStruct *np;
	CDataList *DataList = &(file->getBlock()->getDataList());

// a component can only be SMD if the geometry is SMD.
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               doc->SetUnknownAttrib(&np->getAttributesRef(), ATT_LOADED, "FALSE", attributeUpdateOverwrite, NULL);
            }
         } // case INSERT
         break;
      } // end switch
   } // end update_NotinBomData */
   return;
}

void set_UnloadedComponentPNAttribute(CCEtoODBDoc *doc, FileStruct *file, CString DefaultPN)
{
   DataStruct *np;
	CDataList *DataList = &(file->getBlock()->getDataList());

   int loadedKeywordIndex = doc->getStandardAttributeKeywordIndex(standardAttributeLoaded);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np != NULL && np->isInsertType(insertTypePcbComponent))
      { 
         CString loadedValue;
         if (doc->getAttributeStringValue(loadedValue, &np->getAttributesRef(), loadedKeywordIndex))
         {
            if (loadedValue.CompareNoCase("FALSE") == 0)
            {
               doc->SetUnknownAttrib(&np->getAttributesRef(), "PARTNUMBER", DefaultPN, attributeUpdateOverwrite, NULL);
            }
         }
      }
   }
}

/*****************************************************************************/
/*
*/
static void clear_ComponentNotLoadedData(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList)
{
   DataStruct *np;
   Attrib     *a;

// a component can only be SMD if the geometry is SMD.
   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (a = is_attvalue(doc, np->getAttributesRef(),ATT_LOADED, 1))   
            {              
               CString tmp;
               tmp = get_attvalue_string(doc, a);
               if (tmp.CompareNoCase("FALSE") == 0)
               {
                  np->setColorOverride(false);
               }
            }
         } // case INSERT
         break;
      } // end switch
   } // end update_NotinBomData */
   return;
}

//-----------------------------------------------------------------------------
// this updates components with an SMD flag.
int update_smdrule_components(CCEtoODBDoc *doc, FileStruct *file, int SMDrule,COperationProgress* operationProgress)
{
   // do nothing
   if (SMDrule == 0)
		return 0;

   if (file)
		update_smdCOMPONENTData(doc, file, &(file->getBlock()->getDataList()), SMDrule, NULL,operationProgress);

   return 1;
}


// used by update_smdpads
//--------------------------------------------------------------
// return 0x1 top
//        0x2 bottom
//        0x4 drill
//
static int GetPADSTACKData(CCEtoODBDoc *doc, CDataList *DataList, int insertLayer)
                     
{
   DataStruct  *np;
   int         typ = 0; 
   int         layer;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPOINT)
               break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA)
               break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_TOOL))
            {
               typ |= 4;
            }
            else
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               layer = np->getLayerIndex();
               if (insertLayer == -1 && doc->IsFloatingLayer(np->getLayerIndex()))
               {
                  // block_layer is not set.
                  if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
                  {
                     if (block->getShape() == T_COMPLEX)
                     {
                        block = doc->Find_Block_by_Num((int)block->getSizeA());
                     }
                     layer = Get_PADSTACKLayer(doc, &(block->getDataList()));
                  }
               }

               if (layer == -1)
                  layer = insertLayer;

               if (layer < 0) break;
               LayerStruct *l = doc->getLayerArray()[layer];

               if (l->getLayerType() == LAYTYPE_PAD_TOP)
                  typ |= 0x1;
               if (l->getLayerType() == LAYTYPE_PAD_BOTTOM)
                  typ |= 0x2;
               if (l->getLayerType() == LAYTYPE_SIGNAL_TOP)
                  typ |= 0x1;
               if (l->getLayerType() == LAYTYPE_SIGNAL_BOT)
                  typ |= 0x2;
               if (l->getLayerType() == LAYTYPE_PAD_ALL)
                  typ |= 0x4;
               if (l->getLayerType() == LAYTYPE_SIGNAL)
                  typ |= 0x4;
               if (l->getLayerType() == LAYTYPE_SPLITPLANE)
                  typ |= 0x4;
               if (l->getLayerType() == LAYTYPE_POWERNEG)
                  typ |= 0x4;
            }
         } // case INSERT
         break;
      } // end switch
   } // end for

   return typ;
} /* end GetPadstackData */

/******************************************************************************
* update_smdpads
*/
int update_smdpads(CCEtoODBDoc *doc,COperationProgress* progress)
{
   generate_PADSTACKACCESSFLAG(doc, TRUE,progress);
   generate_PADSTACKSMDATTRIBUTE(doc,true,progress);

   return 1;
}

/******************************************************************************
* update_smdrule_geometries
*  - this updates pcb geometries with an SMD flag.
*/
int update_smdrule_geometries(CCEtoODBDoc *doc, int smdRule,COperationProgress* progress)
{
   if (!smdRule)
      return 0;

   update_smdGEOMETRYData(doc, smdRule,progress);

   return 1;
}

/******************************************************************************
* update_smdrule
*  - this updates pcb geometries and components with an SMD flag.
*/
int update_smdrule(CCEtoODBDoc *doc, int smdRule)
{
   if (!smdRule)
      return 0;

   update_smdGEOMETRYData(doc, smdRule);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);

      update_smdCOMPONENTData(doc, file, &file->getBlock()->getDataList(), smdRule, NULL);
   }

   return 1;
}


// Return 1 on ok and 0 on error.
// clears Gerber apertures
int Clear_DCodes(CCEtoODBDoc *Doc)
{
   int   i;
   for (i=0; i < Doc->getNextWidthIndex(); i++)
   {
      if (Doc->getWidthTable()[i])
      {
         Doc->getWidthTable()[i]->setDcode(0);
      }
   }
   return 1;
}

// Return 1 on ok and 0 on error.
// makes unique Gerber apertures
int Generate_Unique_DCodes(CCEtoODBDoc *Doc)
{
   // automatically generate unique default DCodes for any assigned to D0
   char used[MAX_DCODES];
   int i, default_DCode = 10;

   memset(used, 0, MAX_DCODES);

   // set which dcodes have been used
   for (i=0; i < Doc->getNextWidthIndex(); i++)
   {
      if (Doc->getWidthTable()[i])
      {
         used[Doc->getWidthTable()[i]->getDcode()] = TRUE; // set as used
      }
   }

   for (i=0; i < Doc->getNextWidthIndex(); i++)
   {
      if (Doc->getWidthTable()[i] != NULL)
      {
         if (Doc->getWidthTable()[i]->getDcode() == 0)
         {  
            // find next unused dcode
            while (default_DCode < (MAX_DCODES - 1) && used[default_DCode])
            {
               default_DCode++;
            }

            Doc->getWidthTable()[i]->setDcode(default_DCode);

            if (default_DCode < (MAX_DCODES-1)) default_DCode++;
         }
      }
   }

   return 1;
}

// Return 1 on ok and 0 on error.
// makes unique Tool codes
int Generate_Unique_TCodes(CCEtoODBDoc *Doc)
{
   // set default toolcodes
   char used[MAX_TCODES];

   // default toolcodes 
   int default_TCode = 1;

   memset(used, 0, MAX_TCODES);

   // set which toolcodes have been used
	int i=0;
   for (i=0; i<Doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = Doc->getBlockAt(i);

      if (block == NULL)   continue;

      if (!(block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL)) continue;

      if (used[block->getTcode()]) // if already used
         block->setTcode(0); // so it will get set to an unused toolcode
      else
         used[block->getTcode()] = TRUE; // set as used
   }

   for (i=0; i<Doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = Doc->getBlockAt(i);

      if (block == NULL)   continue;

      if (!(block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL)) continue;

      if (block->getTcode() == 0)
      {
         // find next unused dcode
         while (default_TCode < (MAX_TCODES-1) && used[default_TCode])
         {
            default_TCode++;
         }

         block->setTcode(default_TCode);

         if (default_TCode < (MAX_TCODES-1)) default_TCode++;
      }
   }

   return 1;
}

//--------------------------------------------------------------
// Default insert type dependend in block type
//
int default_inserttype(int blocktype)
{
   switch (blocktype)
   {
      case BLOCKTYPE_PCB:              return   INSERTTYPE_PCB;   
      case BLOCKTYPE_PCBCOMPONENT:     return   INSERTTYPE_PCBCOMPONENT;
      case BLOCKTYPE_MECHCOMPONENT:    return   INSERTTYPE_MECHCOMPONENT;
      case BLOCKTYPE_GENERICCOMPONENT: return   INSERTTYPE_GENERICCOMPONENT;
      case BLOCKTYPE_FIDUCIAL:         return   INSERTTYPE_FIDUCIAL;
      case BLOCKTYPE_TOOLING:          return   INSERTTYPE_TOOLING;
      case BLOCKTYPE_TESTPOINT:        return   INSERTTYPE_TESTPOINT;
      case BLOCKTYPE_SYMBOL:           return   INSERTTYPE_SYMBOL;
      case BLOCKTYPE_GATEPORT:         return   INSERTTYPE_PORTINSTANCE;
      case BLOCKTYPE_DRILLHOLE:        return   INSERTTYPE_DRILLHOLE;
      case BLOCKTYPE_COMPLEXDRILLHOLE: return   INSERTTYPE_COMPLEXEDRILLHOLE;
      case BLOCKTYPE_TEST_PROBE:       return   INSERTTYPE_TEST_PROBE;
      case BLOCKTYPE_CENTROID:         return   INSERTTYPE_CENTROID;
      case BLOCKTYPE_GLUEPOINT:        return   INSERTTYPE_GLUEPOINT;
      case BLOCKTYPE_TEST_ACCESSPOINT: return   INSERTTYPE_TEST_ACCESSPOINT;
      case BLOCKTYPE_DRCMARKER:        return   INSERTTYPE_DRCMARKER;
      case BLOCKTYPE_TESTPAD:          return   INSERTTYPE_TESTPAD;
      case BLOCKTYPE_SCHEM_JUNCTION:   return   INSERTTYPE_SCHEM_JUNCTION;
      case BLOCKTYPE_REJECTMARK:       return   INSERTTYPE_REJECTMARK;
   }

   return 0;
}

static LayerStruct *getUltimateLayer(CCEtoODBDoc *doc, DataStruct *insertData)
{
   // We're expecting the insertData at first call to be a padstack.
   // We're looking for the layer used by a pad, any pad. 
   // There could be more than one pad and one layer involved, but we aren't being that thorough.
   // For our purposes here we want a sample, the first one found will do.

   if (doc != NULL && insertData != NULL)
   {
      LayerStruct *lp = doc->getLayer( insertData->getLayerIndex() );
      if (lp != NULL)
      {
         return lp;
      }
      else
      {
         int insertedBlkNum = insertData->getInsert()->getBlockNumber();
         BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);

         if (insertedBlk != NULL)
         {
            POSITION pos = insertedBlk->getHeadDataInsertPosition();
            while (pos != NULL)
            {
               DataStruct *d = insertedBlk->getNextDataInsert(pos);
               lp = getUltimateLayer(doc, d);
               if (lp != NULL)
               {
                  return lp;
               }
            }
         }
      }
   }

   return NULL;
}

/******************************************************************************
* generate_PINLOC
*/
//--------------------------------------------------------------
// this function generates the PINPOS attribute record for all pins.
// it loop through all component (INSERTPCBCOMPONENT) and inserts
// all PINLOC if a PINLOC could not be found. (or later if the component was modified
// 
void generate_PINLOC(CCEtoODBDoc *doc, FileStruct *file, BOOL Recalculate,COperationProgress* progress)
{
   if (doc != NULL && file != NULL && file->getBlock() != NULL)
   {
      CString logFileName = GetLogfilePath("pinloc.txt");
      CStdioFileWriteFormat reportWriteFormat(logFileName);

      int errorsWrittenCount = doc->getCamCadData().generatePinLocation(file->getFileNumber(),Recalculate != 0,&reportWriteFormat,progress);

      //CWaitCursor wait;

      //FILE *fLog = NULL;
      //CString logFileName = getApp().getUserPath() + "pinloc.txt";

      //POSITION compDataPos = file->getBlock()->getDataList().GetHeadPosition();

      //if (progress != NULL)
      //{
      //   progress->setLength(file->getBlock()->getDataList().GetCount());
      //   progress->updateProgress(0.);
      //}

      //while (compDataPos)
      //{
      //   if (progress != NULL)
      //   {
      //      progress->incrementProgress();
      //   }

      //   DataStruct *compData = file->getBlock()->getDataList().GetNext(compDataPos);

      //   if (compData->getDataType() != T_INSERT)
      //      continue;

      //   if (compData->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT && compData->getInsert()->getInsertType() != INSERTTYPE_TESTPOINT)
      //      continue;

      //   CString compName = compData->getInsert()->getRefname();

      //   BlockStruct *compGeom = doc->getBlockAt(compData->getInsert()->getBlockNumber());

      //   if (!compGeom)
      //   {
      //      if (!fLog)
      //         fLog = fopen(logFileName, "wt");

      //      fprintf(fLog, "Bad Geometry Reference : Comp[%s] to Geometry #%d\n", compName.GetBuffer(0), compData->getInsert()->getBlockNumber());
      //      continue;
      //   }

      //   double insert_x = compData->getInsert()->getOriginX();
      //   double insert_y = compData->getInsert()->getOriginY();
      //   double insert_rot = compData->getInsert()->getAngle();
      //   bool insert_mirror       = ((compData->getInsert()->getMirrorFlags() & MIRROR_FLIP  ) != 0);  // flips koos
      //   bool insert_mirror_layer = ((compData->getInsert()->getMirrorFlags() & MIRROR_LAYERS) != 0);  // mirrors layers
      //   double scale = 1;
      //
      //   // has no reference designator
      //   if (compName.IsEmpty())
      //      continue;

      //   // here loop throu block (component definition) and find PINPOS
      //   POSITION pinDataPos = compGeom->getDataList().GetHeadPosition();

      //   while (pinDataPos)
      //   {
      //      DataStruct *pinData = compGeom->getDataList().GetNext(pinDataPos);

      //      if (pinData->getDataType() != T_INSERT)
      //         continue;

      //      if (pinData->getInsert()->getInsertType() != INSERTTYPE_PIN)
      //         continue;

      //      CString pinName = pinData->getInsert()->getRefname();

      //      if (pinName.IsEmpty())
      //         continue;

      //      CompPinStruct *cp = check_add_comppin(compName, pinName, file);

      //      if (!cp)
      //         continue;

      //      if (!cp->getPinCoordinatesComplete() || Recalculate)
      //      {
      //         Point2 point2;
      //         Mat2x2 m;
      //         RotMat2(&m, insert_rot);

      //         point2.x = pinData->getInsert()->getOriginX() * scale;
      //         if (insert_mirror)
      //            point2.x = -point2.x;
      //         point2.y = pinData->getInsert()->getOriginY() * scale;
      //         TransPoint2(&point2, 1, &m, insert_x, insert_y);
      //                                   
      //         cp->setPinCoordinatesComplete(true);
      //         cp->setOrigin(point2.x,point2.y);
      //         cp->setRotationRadians( insert_rot + pinData->getInsert()->getAngle());   // pin rotation plus comp rotation.
      //         cp->setMirror( compData->getInsert()->getMirrorFlags() ^ pinData->getInsert()->getMirrorFlags());
      //      }

      //      if (cp->getPadstackBlockNumber() < 0 || Recalculate)
      //         cp->setPadstackBlockNumber( pinData->getInsert()->getBlockNumber());

      //      if (!cp->getVisible() || Recalculate)
      //      {
      //         BlockStruct *pinGeom = doc->getBlockAt(pinData->getInsert()->getBlockNumber());

      //         if (!pinGeom)
      //         {
      //            if (!fLog)
      //               fLog = fopen(logFileName, "wt");

      //            fprintf(fLog, "Bad Geometry Reference : Pin[%s] to Geometry #%d\n", pinName.GetBuffer(0), pinData->getInsert()->getBlockNumber());
      //            continue;
      //         }

      //         bool accessTop        = ((pinGeom->getFlags() & BL_ACCESS_TOP        ) != 0);
      //         bool accessBottom     = ((pinGeom->getFlags() & BL_ACCESS_BOTTOM     ) != 0);
      //         bool accessTopOnly    = ((pinGeom->getFlags() & BL_ACCESS_TOP_ONLY   ) != 0);
      //         bool accessBottomOnly = ((pinGeom->getFlags() & BL_ACCESS_BOTTOM_ONLY) != 0);

      //         bool visibleTop,visibleBottom;

      //         bool pinLayerMirrored = pinData->getInsert()->getLayerMirrored();
      //         LayerStruct *pinLayer = getUltimateLayer(doc, pinData);
      //         bool layerHasMirror = pinLayer != NULL ? (pinLayer->getMirroredLayerIndex() != pinLayer->getLayerIndex()) : false;

      //         if (!insert_mirror_layer || !layerHasMirror) // component insert is not mirrored
      //         {
      //            if (pinLayerMirrored && layerHasMirror)
      //            {
      //               visibleTop    = accessBottom;
      //               visibleBottom = accessTop || accessBottomOnly;
      //            }
      //            else
      //            {
      //               visibleTop    = accessTop || accessTopOnly;
      //               visibleBottom = accessBottom;
      //            }
      //         }
      //         else  // component insert is mirrored
      //         {
      //            if (pinLayerMirrored && layerHasMirror)
      //            {
      //               visibleTop    = accessTop || accessTopOnly;
      //               visibleBottom = accessBottom;
      //            }
      //            else
      //            {
      //               visibleTop    = accessBottom;
      //               visibleBottom = accessTop || accessBottomOnly;
      //            }
      //         }

      //         if ((pinGeom->getFlags() & BL_ACCESS_NONE) != 0)
      //         {
      //            cp->setVisible(VISIBLE_NONE);
      //         }
      //         else if (visibleTop && visibleBottom)
      //         {
      //            cp->setVisible(VISIBLE_BOTH);
      //         }
      //         else if (visibleTop)
      //         {
      //            cp->setVisible(VISIBLE_TOP);
      //         }
      //         else if (visibleBottom)
      //         {
      //            cp->setVisible(VISIBLE_BOTTOM);
      //         }
      //         else
      //         {
      //            cp->setVisible(VISIBLE_BOTH);
      //         }
      //      }

      //      doc->CopyAttribs(&cp->getAttributesRef(), pinData->getAttributesRef());
      //   }
      //}

      //if (fLog)  
      //{
      //   fclose(fLog);
      //   Notepad(logFileName);
      //}

      if (errorsWrittenCount > 0)  
      {
         Notepad(logFileName);
      }
   }
}

/******************************************************************************
* Get_PADSTACKLayer
*  returns layer index
*/
int Get_PADSTACKLayer(CCEtoODBDoc *doc, CDataList *DataList)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case INSERTTYPE_RELIEFPAD:
      case INSERTTYPE_CLEARPAD:
      case INSERTTYPE_OBSTACLE:
      case INSERTTYPE_DRILLSYMBOL:
         continue;
      }

      if (!doc->IsFloatingLayer(data->getLayerIndex()))
         return data->getLayerIndex();

      BlockStruct *insertedGeom = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      // block_layer is not set.
      if (insertedGeom->getFlags() & BL_APERTURE || insertedGeom->getFlags() & BL_BLOCK_APERTURE)
      {
         if (insertedGeom->getShape() == T_COMPLEX)
            insertedGeom = doc->Find_Block_by_Num((int)insertedGeom->getSizeA());
      }

      return Get_PADSTACKLayer(doc, &insertedGeom->getDataList()); // recursive
   }

   return -1;
}
//--------------------------------------------------------------
// sets all layers to the floatlayer !
static int Set_PADSTACKLayer(CCEtoODBDoc *doc, CDataList *DataList, int floatlayer)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() == T_INSERT)
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)   continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD)    continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE)    continue;
         if (np->getInsert()->getInsertType() == INSERTTYPE_DRILLSYMBOL) continue;
      }

      if (!doc->IsFloatingLayer(np->getLayerIndex()))
         np->setLayerIndex(floatlayer);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            // block_layer is not set.
            if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               if (block->getShape() == T_COMPLEX)
               {
                  block = doc->Find_Block_by_Num((int)block->getSizeA());
               }
            }
            Set_PADSTACKLayer(doc, &(block->getDataList()), floatlayer);
         } // case INSERT
         break;
      } // end switch
   } // end Set_PADSTACKLayer */
   return -1;
}

/******************************************************************************
* Get_PADSTACKACCESSLayer
// 0 = none 
// 1 = top
// 2 = bottom
// 4 = mask top
// 8 = mask bot
// 16 = top only
// 32 = bot only
*/
int Get_PADSTACKACCESSLayer(CCEtoODBDoc *doc, CDataList *DataList)
{
   int retval = 0;

   if (doc != NULL)
   {
      retval = doc->getCamCadData().getPadstackAccessLayer(*DataList);
   }

   return retval;
}

//--------------------------------------------------------------
// always means, recalculate even if a BL_ACCESS flag already exist.
//
void generate_TRUESHAPEAPERTUREFLAG(CCEtoODBDoc *doc)
{
   ErrorMessage("True Shape not implemented");

   return;
}

/******************************************************************************
* generate_PADSTACKSMDATTRIBUTE
// always means, recalculate even if a BL_ACCESS flag already exist.
*/
bool generate_PADSTACKSMDATTRIBUTE(CCEtoODBDoc& doc,BlockStruct& block,bool recalculateAccessFlags)
{ 
   bool retval = false;

   int smdShapeKeywordIndex   = doc.getStandardAttributeKeywordIndex(standardAttributeSmd);
   int blindShapeKeywordIndex = doc.getStandardAttributeKeywordIndex(standardAttributeBlind);
   int techKeyword            = doc.getStandardAttributeKeywordIndex(standardAttributeTechnology);

   // here mark SMD according to flag
   while (true)
   {
      if (block.getBlockType() != BLOCKTYPE_PADSTACK)
      {
         break;
      }

      if (isValidDrillExist(&doc,&block))
      {  
			CString value = "THRU";
         doc.SetAttrib(&block.getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
         RemoveAttrib(smdShapeKeywordIndex, &block.getAttributesRef());
         break;
      }

      int flag = Get_PADSTACKACCESSLayer(&doc, &block.getDataList());

      if (recalculateAccessFlags)
      {
         block.setFlags(0);
      }

      // knv - (TSR 3978)
      bool topPad        = ((flag & padStackAccessTop       ) != 0);
      bool bottomPad     = ((flag & padStackAccessBottom    ) != 0);
      bool topPadOnly    = ((flag & padStackAccessTopOnly   ) != 0);
      bool bottomPadOnly = ((flag & padStackAccessBottomOnly) != 0);
      bool innerPad      = ((flag & padStackAccessInner     ) != 0);

      if ((topPad    && (bottomPad || bottomPadOnly)) ||
          (bottomPad && (topPad    || topPadOnly   ))     )
      {
         // through hole
			CString value = "THRU";
         doc.SetAttrib(&block.getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
      }
      else 
		{
			// Anything else it is an SMD padstack
			CString value = "SMD";
         doc.SetAttrib(&block.getAttributesRef(), techKeyword, VT_STRING, value.GetBuffer(0), attributeUpdateOverwrite, NULL);
         doc.SetAttrib(&block.getAttributesRef(), smdShapeKeywordIndex, VT_NONE, NULL, attributeUpdateOverwrite, NULL);
      }

      retval = true;
      break;
   }

   return retval;
}

void generate_PADSTACKSMDATTRIBUTE(CCEtoODBDoc *doc,bool recalculateAccessFlags,COperationProgress* progress)
{ 
   if (doc != NULL)
   {
      if (progress != NULL)
      {
         progress->updateStatus("Calculating padstack smd attributes",doc->getMaxBlockIndex());
      }

      // here mark SMD according to flag
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (progress != NULL)
         {
            progress->incrementProgress();
         }

         if (block == NULL)
            continue;

         generate_PADSTACKSMDATTRIBUTE(*doc,*block,recalculateAccessFlags);
      }
   }
}

/******************************************************************************
* isValidDrillExist
*/
bool isValidDrillExist(CCEtoODBDoc *doc, BlockStruct *block)
{
	bool drillFound = false;
	if (doc != NULL && block != NULL)
	{
		POSITION pos = block->getHeadDataInsertPosition();
		while (pos)
		{
			DataStruct *data = block->getNextDataInsert(pos);
			if (data->getInsert()->getInsertType() == insertTypeDrillHole)
			{
				drillFound = true;
				break;
			}

			BlockStruct *subblock = doc->getBlockAt(data->getInsert()->getBlockNumber());
			
			double minimumSize = doc->getSettings().m_dMinimumTHDrillSize * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
			if (subblock->isDrillHole() && subblock->getToolSize() >= minimumSize)
			{
				drillFound = true;
				break;
			}
		}
	}

	return drillFound;
}

/******************************************************************************
* generate_pin_markers
*/
void generate_pin_markers(CCEtoODBDoc *doc, BOOL square, double size)
{
    Graph_Level_Mirror("PIN1_TOP", "PIN1_BOT", "");

   for (int i=0; i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (!block || block->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if (data->getDataType() == T_INSERT)         
         {
            if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               if (!STRICMP(data->getInsert()->getRefname(), "1"))
               {
                  //Add the markers on Pin1_top and Pin1_bot
                  double x = data->getInsert()->getOriginX();
                  double y = data->getInsert()->getOriginY();
                  int err, index;

                  if (!square)
                     index = Graph_Aperture("Pin1Mark",T_ROUND, size, 0, 0, 0, 0, 0, 0, 1, &err);
                  else
                     index = Graph_Aperture("Pin1Mark",T_SQUARE, size, 0, 0, 0, 0, 0, 0, 1, &err);

                  Graph_Block_On(2,block->getName(),block->getFileNumber(),0);
                  DataStruct *newData = Graph_Block_Reference("Pin1Mark", "", block->getFileNumber(), x,y,
                     0,0,1.0,(doc->FindLayer_by_Name("PIN1_TOP"))->getLayerIndex(),0);
                  Graph_Block_Off();
               }
         }
      }
   }
}


/******************************************************************************
* generate_PADSTACKACCESSFLAG
// always means, recalculate even if a BL_ACCESS flag already exist.
*/
void generate_PADSTACKACCESSFLAG(CCEtoODBDoc *doc, BOOL Always,CDcaOperationProgress* progress)
{
   if (doc != NULL)
   {
      doc->getCamCadData().generatePadstackAccessFlag(Always != 0,progress);
   }
}

//--------------------------------------------------------------
// always means, recalculate even if a BL_ACCESS flag already exist.
//
void generate_SOLDERMASKATTRIBUTE(CCEtoODBDoc *doc, int always)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {
         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SOLDERMASK, 1);

         if (always || !a )
         {
            if (a) // delete attribute if exist
            {
               int keyindex = doc->IsKeyWord(ATT_SOLDERMASK, TRUE);
               RemoveAttrib(keyindex, &block->getAttributesRef());
            }

            int accessLayer = Get_PADSTACKACCESSLayer(doc, &(block->getDataList()));
            accessLayer = accessLayer & (padStackAccessMaskTop | padStackAccessMaskBottom); // mask out electrical layers
            int testAccess = ATT_TEST_ACCESS_NONE;

            switch (accessLayer)
            {
            case padStackAccessMaskTop | padStackAccessMaskBottom:  testAccess = ATT_TEST_ACCESS_ALL;  break;
            case padStackAccessMaskTop:                             testAccess = ATT_TEST_ACCESS_TOP;  break;
            case padStackAccessMaskBottom:                          testAccess = ATT_TEST_ACCESS_BOT;  break;
            }

            doc->SetUnknownAttrib(&block->getAttributesRef(), ATT_SOLDERMASK, testaccesslayers[testAccess], attributeUpdateOverwrite, NULL);
         }
      }
   }

   return;     
}

//--------------------------------------------------------------
// the tolerance keyword is checked for a good value.
// allowed values are:
// 10
// 10%
// +-10%
// -+10%
// +10%-10%
// -10%+10%
// -10% +10%
// +10% -10%
// -20+80%
//
// NOTE: *** when the tolerance is a negative value that means invalid tolerance
//
int check_tolerance(const char *att, double *mtol, double *ptol, double *tol)
{
   *tol = -1;
   *mtol = -1;
   *ptol = -1;

   if (strlen(att) == 0)
		return -1;
   CString  ws;
   
   ws = "";

   // check only number
   unsigned int i;

   for (i=0; i<strlen(att); i++)
   {
      if (isspace(att[i])) 
			continue;
      // checks for letter or _
      if (__iscsymf(att[i]))
			continue;   
      ws += att[i];
   }

   if (strlen(ws) == 0)
		return -1;

// ===> here all 2 argc

   // +10%-10%
   if ((i = sscanf(ws, "+%lf%%-%lf%%",ptol, mtol)) == 2)
   {
      *tol = -1;   // need to reinit 
      return 0;
   }
   // -10%+10%
   if ((i = sscanf(ws, "-%lf%%+%lf%%",mtol, ptol)) == 2)
   {
      *tol = -1;
      return 0;
   }

   // +10-10
   if ((i = sscanf(ws, "+%lf-%lf",ptol, mtol)) == 2)
   {
      *tol = -1;
      return 0;
   }
   // -10+10
   if ((i = sscanf(ws, "-%lf+%lf",mtol, ptol)) == 2)
   {
      *tol = -1;
      return 0;
   }

// ===> now all 1 arg

   // 10 or 10%
   if ((i = sscanf(ws, "%lf",tol)) == 1)
   {
      *mtol = -1;
      *ptol = -1;
      return 0;     
   }

   // +-10
   if ((i = sscanf(ws, "+-%lf",tol)) == 1)
   {
      *mtol = -1;
      *ptol = -1;
      return 0;
   }
   // -+10
   if ((i = sscanf(ws, "-+%lf",tol)) == 1)
   {
      *mtol = -1;
      *ptol = -1;
      return 0;
   }


   // if all failed, just go for a number
   ws = "";
   for (i=0; i<strlen(att); i++)
   {
      if (isdigit(att[i])) 
         ws += att[i];
   }

   *mtol = -1;
   *ptol = -1;
   *tol = atof(ws);

   return 0;
}

// this generates a global defined centroid geometry, which will be used in placement program
// generation. It returns the centroid definition name
BlockStruct* generate_Gluepoint_Geom(CCEtoODBDoc *doc)
{
   BlockStruct *b = Graph_Block_Exists(doc, GLUEPOINTGEOM, -1);

   if (b == NULL)
   {
      b = Graph_Block_On(GBO_APPEND,GLUEPOINTGEOM,-1,0);
      b->setBlockType(BLOCKTYPE_GLUEPOINT);
      b->setFlagBits(BL_SPECIAL);
      int l = Graph_Level("GLUEPOINT_BOT", "", 0);
      LayerStruct *lp = doc->FindLayer(l);
      lp->setLayerType(LAYTYPE_BOTTOM);
      lp->setVisible(false);

      l = Graph_Level("GLUEPOINT_TOP", "", 0);
      lp = doc->FindLayer(l);
      lp->setLayerType(LAYTYPE_TOP);
      lp->setVisible(false);

      Graph_Level_Mirror("GLUEPOINT_TOP", "GLUEPOINT_BOT", "");

      // O-->  this is the centroid graphic
      double f = Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());

      Graph_Circle(l, 0, 0, 50*f, 0L, 0 , FALSE, TRUE);  // filled
      Graph_Line(l,0,0,50*f,0, 0L, 0 , FALSE); 
      Graph_PolyStruct(l,0,FALSE);
      Graph_Poly(NULL,0, 0,0,1);
      Graph_Vertex(50*f, 25*f,0.0);
      Graph_Vertex(50*f, -25*f,0.0);
      Graph_Vertex(100*f,0,0.0);
      Graph_Vertex(50*f,25*f,0.0);
      
      Graph_Block_Off();
   }
   
   int l = Graph_Level("GLUEPOINT_TOP", "", 0);
   LayerStruct *lp = doc->FindLayer(l);
   lp->setVisible(true);
   l = Graph_Level("GLUEPOINT_BOT", "", 0);
   lp = doc->FindLayer(l);
   lp->setVisible(true);

   return b;
}

//--------------------------------------------------------------
double get_drill_from_block_no_scale(CCEtoODBDoc *doc, BlockStruct *block)
{
   DataStruct *np;
   double      drill = 0.0;

   // if the block is a drill tool.
   if ( (block->getFlags() & BL_TOOL))
   {
      return block->getToolSize();
   }
   else // drill could be hiddden in a complex aperture.
   if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
   {
      if (block->getShape() == T_COMPLEX)
      {
         BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
         drill = get_drill_from_block_no_scale(doc, subblock);
      }
   }

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = block->getDataList().GetNext(pos);
      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_TOOL))
            {
              return block->getToolSize();
            }
            else
            {
               drill = get_drill_from_block_no_scale(doc, block);
            }
         } // case INSERT
         break;                                                                
      } // end switch
   }
   return drill;
}

/******************************************************************************
* GetDrillInGeometry
*/

BlockStruct *GetDrillInGeometry(CCEtoODBDoc *doc, BlockStruct *block)
{
   if (doc != NULL && block != NULL)
      return GetDrillInGeometry(doc->getCamCadData(), block);

   return NULL;
}

//--------------------------------------------------------

BlockStruct *GetDrillInGeometry(CCamCadData& ccdata, BlockStruct *block)
{
   if (block != NULL)
   {
      // if the block is a drill tool.
      if (block->getFlags() & BL_TOOL)
         return block;

      // drill could be hiddden in a complex aperture.
      if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      {
         if (block->getShape() == T_COMPLEX)
         {
            BlockStruct *subblock = ccdata.getBlockAt((int)(block->getSizeA()));
            BlockStruct *drill = GetDrillInGeometry(ccdata, subblock);
            if (drill)
               return drill;
         }
      }

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data->getDataType() != T_INSERT)
            continue;

         BlockStruct *subblock = ccdata.getBlockAt(data->getInsert()->getBlockNumber());

         if (subblock->getFlags() & BL_TOOL)
            return subblock;

         if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
            continue;

         BlockStruct *drill = GetDrillInGeometry(ccdata, subblock);
         if (drill)
            return drill;
      }
   }

   return NULL;
}

/******************************************************************************
* GetDrillInsertInGeometry
*/
InsertStruct* GetDrillInsertInGeometry(CCEtoODBDoc* doc, BlockStruct* block)
{
   // if the block is a drill tool.
   if (block->getFlags() & BL_TOOL)
      return NULL;

   // drill could be hiddden in a complex aperture.
   if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
   {
      if (block->getShape() == T_COMPLEX)
      {
         BlockStruct* subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
         InsertStruct* drill = GetDrillInsertInGeometry(doc, subblock);
         if (drill)
            return drill;
      }
   }

   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct* data = block->getDataList().GetNext(dataPos);
      if (data->getDataType() != T_INSERT || data->getInsert() == NULL)
         continue;

      BlockStruct* subblock = doc->getBlockAt(data->getInsert()->getBlockNumber());

      if (subblock->getFlags() & BL_TOOL)
        return data->getInsert();

      if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
         continue;

      InsertStruct* drill = GetDrillInsertInGeometry(doc, subblock);
      if (drill)
         return drill;
   }

   return NULL;
}


//--------------------------------------------------------------
BlockStruct *get_padstack_block(CCEtoODBDoc *doc, BlockStruct *block)
{
   DataStruct *np;
   BlockStruct *db = NULL;

   // if the block is a drill tool. 

   block->setBlockType(BLOCKTYPE_PADSTACK);

   if (block->getBlockType())
   {
      return block;
   }

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = block->getDataList().GetNext(pos);
      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            block->setBlockType(BLOCKTYPE_PADSTACK);

            if (block->getBlockType())
            {
              return block;
            }
            else
            {
               db = get_padstack_block(doc, block);
            }
         } // case INSERT
         break;                                                                
      } // end switch
   }
   return db;
}

/******************************************************************************
* datastruct_from_refdes
*/
DataStruct *datastruct_from_refdes(CCEtoODBDoc *doc, BlockStruct *block, const char *refdes)
{
	if (block == NULL)
		return NULL;

	return block->FindData(refdes);
}


//--------------------------------------------------------------
static void loop_inserttypes_by_block(CCEtoODBDoc *doc, BlockStruct *changeblock, BlockStruct *block, 
                                      int blocktype, int inserttype)
{
   DataStruct *np;

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      np = block->getDataList().GetNext(pos);
      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *b = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (b->getBlockNumber() == changeblock->getBlockNumber())
            {
               np->getInsert()->setInsertType(inserttype);
            }
            else
            {
               loop_inserttypes_by_block(doc, changeblock, b, blocktype, inserttype);
            }
         } // case INSERT
         break;                                                                
      } // end switch
   }
   return;
}

/*******************************************************************************/
/* 
*/
void change_inserttypes_by_block(CCEtoODBDoc *doc, BlockStruct *block,int inserttype)
{
   
   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())    continue;
      loop_inserttypes_by_block(doc, block, file->getBlock(), block->getBlockType(), inserttype);
   }
   return;
}

/******************************************************************************
* is_electricallayer
*/
int is_electricallayer(int layerType)
{
   switch (layerType)
   {
   case LAYTYPE_SIGNAL_TOP:
   case LAYTYPE_SIGNAL_BOT:
   case LAYTYPE_SIGNAL_INNER:
   case LAYTYPE_SIGNAL_ALL:
   case LAYTYPE_PAD_TOP:
   case LAYTYPE_PAD_BOTTOM:
   case LAYTYPE_PAD_INNER:
   case LAYTYPE_PAD_ALL:
   case LAYTYPE_PAD_THERMAL:
   case LAYTYPE_POWERNEG:
   case LAYTYPE_POWERPOS:
   case LAYTYPE_SPLITPLANE:
   case LAYTYPE_SIGNAL:
   case LAYTYPE_PAD_OUTER:
      return TRUE;
   }

   return FALSE;
}

/******************************************************************************
* is_layerbottom
*/
int is_layerbottom(int layerType)
{
   switch (layerType)
   {
   case LAYTYPE_SIGNAL_BOT:
   case LAYTYPE_PAD_BOTTOM:
   case LAYTYPE_PASTE_BOTTOM:
   case LAYTYPE_MASK_BOTTOM:
   case LAYTYPE_SILK_BOTTOM:
   case LAYTYPE_BOTTOM:
   case LAYTYPE_FLUX_BOTTOM:
   case LAYTYPE_PKG_PIN_LEG_BOT:
   case LAYTYPE_PKG_PIN_FOOT_BOT:
      return TRUE;
   }

   return FALSE;
}


/******************************************************************************
* get_string()
*  - Use this instead of strtok !!!
*  - Use this when the backslash character is an escape character only for 
*        specific characters
*  - Escape character is assumed to be a backslash
*
*  delimeter is always a space
*  " xxx " is done
*  " xx\" " is ok as long as " is in the list of charsToEnforceEscape
*/
static char *pcbUtilString;
static CString pcbUtilTok;
char *get_string(char *line, const char *delimeter, char *charsToEnforceEscape)
{
	bool findEndQuote = false;

   if (line != NULL)
      pcbUtilString = line;

   while (*pcbUtilString && strchr(delimeter, *pcbUtilString))
      *pcbUtilString++;

   if (*pcbUtilString == NULL)
      return NULL;

   pcbUtilTok = "";

   // check if quotes
   if (*pcbUtilString == '\"') // go until hit end quote
	{
		findEndQuote = true;
      *pcbUtilString++; // skip first quote
	}


	while (*pcbUtilString && 
			(
				(findEndQuote && *pcbUtilString != '\"') ||				// Go until find valid end-quote
				(!findEndQuote && !strchr(delimeter, *pcbUtilString))  // Go until find delimiter
			))
	{
		// If current character is a backslash and we have an escape list to enforce
		if (*pcbUtilString == '\\' && charsToEnforceEscape != NULL)
		{
			// If the next character is one to escape on use it instead of the slash
			if (strchr(charsToEnforceEscape, *++pcbUtilString) == NULL)
				*pcbUtilString--;	// If the next character is NOT one to escape on then use the slash
		}

		// append the token
		pcbUtilTok += *pcbUtilString++;
	}

   if (*pcbUtilString)
      *pcbUtilString++;

   return pcbUtilTok.GetBuffer(0);
}

/******************************************************************************
* get_string()
*  - Use this instead of strtok !!!
*
*  delimeter is always a space
*  " xxx " is done
*  " xx\" " is ok
*/
char *get_string(char *line, const char *delimeter, BOOL BackslashIsEscape)
{
   if (line != NULL)
      pcbUtilString = line;

   while (*pcbUtilString && strchr(delimeter, *pcbUtilString))
      *pcbUtilString++;

   if (*pcbUtilString == NULL)
      return NULL;

   pcbUtilTok = "";

   // check if quotes
   if (*pcbUtilString == '\"') // ignore delimeters within quotes and just go until hit end quote
   {
      *pcbUtilString++; // skip first quote

      while (*pcbUtilString && *pcbUtilString != '\"')
      {
         if (BackslashIsEscape && *pcbUtilString == '\\')
            *pcbUtilString++;
         pcbUtilTok += *pcbUtilString++;
      }
   }
   else
   {
      while (*pcbUtilString && !strchr(delimeter, *pcbUtilString))
      {
         if (BackslashIsEscape && *pcbUtilString == '\\')
            *pcbUtilString++;
         pcbUtilTok += *pcbUtilString++;
      }
   }

   if (*pcbUtilString)
      *pcbUtilString++;

   return pcbUtilTok.GetBuffer(0);
}

/*****************************************************************************/
/*
   used in remove_zero_size_pads
*/
static void remove_zero_apertures(CCEtoODBDoc *doc, BlockStruct *block)
{
   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);
      BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (data->getDataType() != T_INSERT)   continue;
      if (block->getFlags() & BL_TOOL || block->getFlags() & BL_BLOCK_TOOL)    continue;

      if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
      {
         if (subblock->getShape() == T_BLANK)
         {
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
         }
         else
         if (subblock->getShape() == T_UNDEFINED)
         {
            RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
         }
         else
         if (subblock->getShape() == T_COMPLEX)
         {
            // 
            BlockStruct *b = doc->Find_Block_by_Num(int(subblock->getSizeA()));
            remove_zero_apertures(doc, b);
         }
         else
         {
            if (subblock->getSizeA() < SMALLNUMBER)
               RemoveOneEntityFromDataList(doc, &block->getDataList(), data);
         }
      }
      else
      {
         remove_zero_apertures(doc, subblock);
      }
   }
   return;
}

/*****************************************************************************/
/* 
   remove_zero_size_pads
   
   1. this function removes apertures which have no size 
   2, also removes complex apertures, which have no contens
   3. T_BLANK apertures will be killed !
   3. T_UNKNOWN apertures will be killed !

*/
void remove_zero_size_pads(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)         continue;

      if (block->getBlockType() != BLOCKTYPE_PADSTACK)
         continue;

      remove_zero_apertures(doc, block);
   }
   return;
}

/*****************************************************************************/
/* 
   flatten_padstack
   
   this function moves inserts under padstack to toplevel
   layers Apertures and Complex Apertures

   1. loop a padstack.
   2. find a insert.
   3. go into the insert and explode all contens.

*/
void flatten_padstack(CCEtoODBDoc *doc, BlockStruct *block)
{

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)   continue;

      if ((block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE))
      {
         if (block->getShape() == T_COMPLEX)
         {

         }
         else
         {
            BlockStruct *subblock = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         }
      }
   }
   return;
}

/****************************************************************************/
/*
   err = 1 Primary Boardoutline found, but not closed!

   err = 2 No Primary Boardoutline found!

   err = 3 One Primary closed Boardoutline and multiple open elements found!
           The open elements will be set to BOARD GEOMETRIES!

   err = 4 Multiple Primary closed Boardoutline and multiple open elements found!
           All elements will be set to BOARD GEOMETRIES!
*/
int check_primary_boardoutline(FileStruct *file)
{
   int         err = 0;
   int         closed_found = 0;
   int         open_found = 0;

   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);
      if (data->getDataType() == T_POLY)
      {
         if (data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)  
         {
            // count thru polys
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);
               if (poly->isClosed())
                  closed_found++;
               else
                  open_found++;

            }
         }
      }
   }

   if (closed_found == 0)
   {
      if (open_found)
         err = 1;
      else
         err = 2;
   }  
   else
   if (closed_found == 1)
   {
      if (open_found)
      {
         err = 3;

         POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);
            if (data->getDataType() == T_POLY)
            {
               if (data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)  
               {
                  // count thru polys
                  POSITION polyPos = data->getPolyList()->GetHeadPosition();
                  while (polyPos)
                  {
                     CPoly *poly = data->getPolyList()->GetNext(polyPos);
                     if (!poly->isClosed())
                        data->setGraphicClass(GR_CLASS_BOARD_GEOM);
                     break;
                  }
               }
            }
         }
      }
   }
   else
   {
      // multiple closed boardoutlines found.
      err = 4;
      POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);
         if (data->getDataType() == T_POLY)
         {
            if (data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)  
               data->setGraphicClass(GR_CLASS_BOARD_GEOM);
         }
      }  
   }

   return err;
}

/******************************************************************************
* round_accuracy
*/
double round_accuracy(double x, double accuracy)
{
   return round(x / accuracy) * accuracy;
}

/******************************************************************************
* Complex_2Circles1Rectangle 
*  - return TRUE for optimized, FALSE for not
*/
int Complex_2Circles1Rectangle(CDataList *DataList, double accuracy,
   double *sizeA, double *sizeB, double *orient, double *xoffset, double *yoffset)
{
   int circlecnt = 0;
   int rectcnt = 0;

   double x, y, radius, cx[2], cy[2], rad[2];
   double llx, lly, uux, uuy;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      // it must contain only poly lines
      if (data->getDataType() != T_POLY)
         return FALSE;

      POSITION polyPos = data->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = data->getPolyList()->GetNext(polyPos);

         if (PolyIsCircle(poly, &x, &y, &radius)) // circle
         {
            circlecnt++;
            if (circlecnt > 2)
               return FALSE;

            cx[circlecnt-1] = x;
            cy[circlecnt-1] = y;
            rad[circlecnt-1] = radius;
         }
         else if (PolyIsRectangle(poly, &llx, &lly, &uux, &uuy))
         {
            rectcnt++;

            if (rectcnt > 1)
               return FALSE;
         }
         else
         {
            // other stuff
            return FALSE;  // if any other than Retangle and Circle, it is not a good oblong
         }
      }
   }

   if (circlecnt == 2 && rectcnt == 1)
   {
      int pos = 0;   // 0 unassigned, 1 right, 2 left, 3 up, 4 down
         
      if (fabs(rad[0] - rad[1]) > accuracy)  // check both circle radius are same
         return FALSE;

      if (fabs(llx - cx[0]) < accuracy)      // is first circle left or right
         pos = 2;    // first circle is left
      else if (fabs(uux - cx[0]) < accuracy) 
         pos = 1; // first circle is right
      else if (fabs(lly - cy[0]) < accuracy) // or is first circle up or down
         pos = 4;    // first circle is down
      else if (fabs(uuy - cy[0]) < accuracy) 
         pos = 3; // first circle is up
      else
      {
         // first circle is neither left or right or up or down
         return FALSE;
      }

      switch (pos)
      {
         case 1:  // first circle is right
            if (fabs(llx - cx[1]) < accuracy)      // check that second circle is on left
            {
               if ((fabs(uuy - lly)/2 - rad[0]) < accuracy)    // check that radius matches
               {
                  // this is it
                  *sizeA = round_accuracy(uux - llx + rad[0] + rad[1], accuracy);
                  *sizeB = round_accuracy(uuy - lly, accuracy);
                  *orient = 0;
                  *xoffset = round_accuracy(llx + (uux - llx)/2, accuracy);
                  *yoffset = round_accuracy(lly + (uuy - lly)/2, accuracy);
                  return TRUE;
               }
            }
         break;
         case 2:  // first circle is left
            if (fabs(uux - cx[1]) < accuracy)      // check that second circle is on right
            {
               if ((fabs(uuy - lly)/2 - rad[0]) < accuracy)    // check that radius matches
               {
                  // this is it
                  // this is it
                  *sizeA = round_accuracy(uux - llx + rad[0] + rad[1], accuracy);
                  *sizeB = round_accuracy(uuy - lly, accuracy);
                  *orient = 0;
                  *xoffset = round_accuracy(llx + (uux - llx)/2, accuracy);
                  *yoffset = round_accuracy(lly + (uuy - lly)/2, accuracy);
                  return TRUE;
               }
            }
         break;
         case 3:  // first circle is up
            if (fabs(lly - cy[1]) < accuracy)      // check that second circle is on down
            {
               if ((fabs(uux - llx)/2 - rad[0]) < accuracy)    // check that radius matches
               {
                  // this is it, a and b are swapped, fixed by rotation
                  *sizeB = round_accuracy(uux - llx, accuracy);
                  *sizeA = round_accuracy(uuy - lly + rad[0] + rad[1], accuracy);
                  *orient = DegToRad(90);
                  *yoffset = round_accuracy(llx + (uux - llx)/2, accuracy);
                  *xoffset = round_accuracy(lly + (uuy - lly)/2, accuracy);
                  return TRUE;
               }
            }
         break;
         case 4:  // first circle is down
            if (fabs(uuy - cy[1]) < accuracy)      // check that second circle is on up
            {
               if ((fabs(uux - llx)/2 - rad[0]) < accuracy)    // check that radius matches
               {
                  // this is it, a and b are swapped, fixed by rotation
                  *sizeB = round_accuracy(uux - llx , accuracy);
                  *sizeA = round_accuracy(uuy - lly + rad[0] + rad[1], accuracy);
                  *orient = DegToRad(90);
                  *yoffset = round_accuracy(llx + (uux - llx)/2, accuracy);
                  *xoffset = round_accuracy(lly + (uuy - lly)/2, accuracy);
                  return TRUE;
               }
            }
         break;
         default:
         break;
      }
   }

   return FALSE;
}

/******************************************************************************
* Complex_1Rectangle 
*  - return TRUE for optimized, FALSE for not
*/
int Complex_1Rectangle(CCEtoODBDoc *doc, CDataList *DataList, double accuracy,
   double *sizeA, double *sizeB, double *orient, double *xoffset, double *yoffset)
{
   int rectCount = 0;

   double left, bottom, right, top, angle;

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_POLY)
         return FALSE;

      POSITION polyPos = data->getPolyList()->GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = data->getPolyList()->GetNext(polyPos);

         double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();

         if (!PolyIsRectangleAngle(poly, lineWidth, accuracy, &left, &bottom, &right, &top, &angle))
            return FALSE;

         rectCount++;
      }
   } 

   if (rectCount != 1)
      return FALSE;

   *sizeA = round_accuracy(right - left, accuracy);
   *sizeB = round_accuracy(top - bottom, accuracy);
   *orient = angle;
   *xoffset = round_accuracy(left + (right - left)/2, accuracy);
   *yoffset = round_accuracy(bottom + (top - bottom)/2, accuracy);

   return TRUE;
}

/******************************************************************************
* Complex_1Circle 
*  - return TRUE for optimized, FALSE for not
*/
int Complex_1Circle(CCEtoODBDoc *doc, CDataList *dataList, double accuracy, 
  double *sizeA, double *xoffset, double *yoffset)
{
   if (dataList->GetCount() != 1)
      return FALSE;

   DataStruct *data = dataList->GetHead();

   if (data->getDataType() != T_POLY)
      return FALSE;

   // Case 2148
   // If there is more than one anything then it can not be replaced by one circle.
   // (Though we could consider keeping only largest if all concentric circles, not sure if
   //  that is a good thing, so it is not done.)
   if (data->getPolyList()->GetCount() > 1)
      return FALSE;

   double cx, cy, rad;
   POSITION polyPos = data->getPolyList()->GetHeadPosition();
   while (polyPos)
   {
      CPoly *poly = data->getPolyList()->GetNext(polyPos);

      if (!PolyIsCircle(poly, &cx, &cy, &rad))
         return FALSE;
   }

   *sizeA = round_accuracy(rad*2, accuracy);
   *xoffset = round_accuracy(cx, accuracy);
   *yoffset = round_accuracy(cy, accuracy);

   return TRUE;
}

/******************************************************************************
* make_path_aperture
*
   return TRUE is a aperture. This is if a path is only 2 points. That results in a oval.
*/
static int make_path_aperture(Point2 p1, Point2 p2, double width, double *sizea, double *sizeb,
      double *offsetx, double *offsety, double *rot)
{
   if (fabs(p1.bulge) > SMALLNUMBER || fabs(p2.bulge) > SMALLNUMBER)
      return 0;

   *rot = atan2(p2.y - p1.y, p2.x - p1.x);

   double length = sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
   *sizea = length + width;
   *sizeb = width;

   *offsetx = length / 2;
   *offsety = 0;

   return 1;
}

/******************************************************************************
* Complex_2PointLine
*  return TRUE for optimized, FALSE for not
*/
int Complex_2PointLine(CCEtoODBDoc *doc, CDataList *DataList, double accuracy, 
   double *sizeA, double *sizeB, double *orient, double *xoffset, double *yoffset, int *shape)
{
   *shape = T_OBLONG;

   if (DataList->GetCount() != 1)
      return FALSE;

   DataStruct *data = DataList->GetHead();

   // it must contain only poly lines
   if (data->getDataType() != T_POLY)
      return FALSE;

   if (data->getPolyList()->GetCount() != 1)
      return FALSE;

   CPoly *poly = data->getPolyList()->GetHead();

   if (poly->getPntList().GetCount() != 2) // if any other than Retangle and Circle, it is not a good oblong
      return FALSE;

   if (doc->getWidthTable()[poly->getWidthIndex()]->getShape() == T_SQUARE || doc->getWidthTable()[poly->getWidthIndex()]->getShape() == T_RECTANGLE) // for gerber a rectangle can be a square
      *shape = T_RECTANGLE;   
   else
      *shape = T_OBLONG;

   CPnt *pnt1 = poly->getPntList().GetHead();
   CPnt *pnt2 = poly->getPntList().GetTail();

   Point2 p1;
   p1.x = pnt1->x;
   p1.y = pnt1->y;
   p1.bulge = 0;

   Point2 p2;
   p2.x = pnt2->x;
   p2.y = pnt2->y;
   p2.bulge = 0;

   double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
   double length = sqrt((pnt1->x - pnt2->x) * (pnt1->x - pnt2->x) + (pnt1->y - pnt2->y) * (pnt1->y - pnt2->y));
      
   *sizeA = round_accuracy(length + lineWidth, accuracy);
   *sizeB = round_accuracy(lineWidth, accuracy);
   *orient = atan2(pnt2->y - pnt1->y, pnt2->x - pnt1->x);
   double xrot, yrot;
   Rotate(p1.x, p1.y, -RadToDeg(*orient), &xrot, &yrot); 
   p1.x = xrot;
   p1.y = yrot;
   Rotate(p2.x, p2.y, -RadToDeg(*orient), &xrot, &yrot); 
   p2.x = xrot;
   p2.y = yrot;

   double left, right, top, bottom;
   if (p1.x < p2.x)
   {
      left = p1.x;
      right = p2.x;
   }
   else
   {
      left = p2.x;
      right = p1.x;
   }
   if (p1.y < p2.y)
   {
      bottom = p1.y;
      top = p2.y;
   }
   else
   {
      bottom = p2.y;
      top = p1.y;
   }

   *xoffset = round_accuracy(left + (right - left)/2, accuracy);
   *yoffset = round_accuracy(bottom + (top - bottom)/2, accuracy);

   return TRUE;
}

/*****************************************************************************/
/*
   this exchanges a COMPLEX aperture with the copyblock and removes the complex
   aperture.
*/
int Exchange_SubBlock(CCEtoODBDoc *doc, int subblocknum, BlockStruct *copyblock)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
      {
         if (block->getShape() == T_COMPLEX)
         {
            // here find all the complex aperture which have the same subblock.
            // this many not happen to often, but....
            if (subblocknum == block->getSizeA())
            {
               block->setShape(copyblock->getShape());
               block->setSizeA(copyblock->getSizeA());
               block->setSizeB(copyblock->getSizeB());
               block->setSizeC(copyblock->getSizeC());
               block->setSizeD(copyblock->getSizeD());
               block->setXoffset(copyblock->getXoffset());
               block->setYoffset(copyblock->getYoffset());
               block->setRotation(block->getRotation() + copyblock->getRotation());
            }
         }
      }
   }

   BlockStruct *subblock = doc->Find_Block_by_Num(subblocknum);
   if (subblock)
      doc->RemoveBlock(subblock);

   return 1;
}

/******************************************************************************
* OptimizeRotation
   rounds to 1 degree accuracy
*/
double OptimizeRotation(double rad)
{
   int r = (int)floor(RadToDeg(rad) + 0.5);

   while (r < 0)
      r += 360;

   while (r >= 360)
      r -= 360;

   return DegToRad(r);
}

/******************************************************************************
* OptimizeComplexApertures_Blocks
*/
int OptimizeComplexApertures_Blocks(CCEtoODBDoc *doc, BlockStruct *block, int insertLayer, double accuracy, BOOL DoPowerLayers)
{
   if (block->getFlags() & BL_TOOL)
      return 0;

   if (!(block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE))
      return 0;

   if (block->getShape() != T_COMPLEX)
      return 0;

   int subblockNum = (int)(block->getSizeA());
   BlockStruct *subblock = doc->Find_Block_by_Num(subblockNum);

   double sizeA = 0;
   double sizeB = 0;
   double orient = 0;
   double xoffset = 0;
   double yoffset = 0;

   if (DoPowerLayers)
   {
      int layerNum;
      if (block->getDataList().IsEmpty()) // if loaded aperture (no data elements in block)
         layerNum = insertLayer;
      else // there were datas before block was converted to an aperture
         layerNum = block->getDataList().GetHead()->getLayerIndex(); // assign to layer of first entity
                  
      if (layerNum < 0)
         return 0;

      LayerStruct *layer = doc->getLayerArray()[layerNum];

      if (layer->getLayerType() == LAYTYPE_POWERNEG || layer->getLayerType() == LAYTYPE_SPLITPLANE)
      {
         int show = layer->isVisible();
         doc->CalcBlockExtents(subblock);
         layer->setVisible(show);

         double smaller = min(subblock->getXmax() - subblock->getXmin(), subblock->getYmax() - subblock->getYmin());

         block->setShape(T_THERMAL);
         block->setSizeA(smaller);
         block->setSizeB((DbUnit)(smaller * 0.7));
         block->setSizeC(0);
         block->setSizeD(0);
         block->setXoffset((DbUnit)0.0);
         block->setYoffset((DbUnit)0.0);
         block->setRotation((DbUnit)0.0);

         // need to exchange all subblockNum
         Exchange_SubBlock(doc, subblockNum, block);
      
         return 1;
      }
   }

   if (Complex_2Circles1Rectangle(&subblock->getDataList(), accuracy, &sizeA, &sizeB, &orient, &xoffset, &yoffset))
   {
      if (fabs(sizeA - sizeB) < accuracy && fabs(xoffset) < accuracy && fabs(yoffset) < accuracy)
      {
         block->setShape(T_ROUND);
         block->setSizeA((DbUnit)sizeA);
         block->setSizeB(0);
         block->setSizeC(0);
         block->setSizeD(0);
         block->setXoffset((DbUnit)xoffset);
         block->setYoffset((DbUnit)yoffset);
         //block->rotation += 0;
      }
      else
      {
         block->setShape(T_OBLONG);
         block->setSizeA((DbUnit)sizeA);
         block->setSizeB((DbUnit)sizeB);
         block->setSizeC(0);
         block->setSizeD(0);
         block->setXoffset((DbUnit)xoffset);
         block->setYoffset((DbUnit)yoffset);
         block->setRotation(block->getRotation() + (DbUnit)OptimizeRotation(-orient));
      }

      if (fabs(xoffset) < accuracy)
         block->setXoffset(0);
      if (fabs(yoffset) < accuracy)
         block->setYoffset(0);

      // need to exchange all subblocknum
      Exchange_SubBlock(doc, subblockNum, block);
      
      return 1;
   }
   
   if (Complex_1Rectangle(doc, &subblock->getDataList(), accuracy, &sizeA, &sizeB, &orient, &xoffset, &yoffset))
   {
      if (fabs(sizeA - sizeB) < accuracy)
      {
         block->setShape(T_SQUARE);
         block->setSizeA((DbUnit)sizeA);
         block->setSizeB(0);
         block->setSizeC(0);
         block->setSizeD(0);
         block->setXoffset((DbUnit)xoffset);
         block->setYoffset((DbUnit)yoffset);
         block->setRotation(block->getRotation() + (DbUnit)OptimizeRotation(orient));
      }
      else
      {
         block->setShape(T_RECTANGLE);
         block->setSizeA((DbUnit)sizeA);
         block->setSizeB((DbUnit)sizeB);
         block->setSizeC(0);
         block->setSizeD(0);
         block->setXoffset((DbUnit)xoffset);
         block->setYoffset((DbUnit)yoffset);
         block->setRotation(block->getRotation() + (DbUnit)OptimizeRotation(orient));
      }     

      if (fabs(xoffset) < accuracy)
         block->setXoffset(0);
      if (fabs(yoffset) < accuracy)
         block->setYoffset(0);

      // need to exchange all subblocknum
      Exchange_SubBlock(doc, subblockNum, block);
      
      return 1;
   }

   if (Complex_1Circle(doc, &subblock->getDataList(),accuracy, &sizeA, &xoffset, &yoffset))
   {
      block->setShape(T_ROUND);
      block->setSizeA((DbUnit)sizeA);
      block->setSizeB(0);
      block->setSizeC(0);
      block->setSizeD(0);
      block->setXoffset((DbUnit)xoffset);
      block->setYoffset((DbUnit)yoffset);
      //block->rotation += 0.0;
      
      if (fabs(xoffset) < accuracy)
         block->setXoffset(0);
      if (fabs(yoffset) < accuracy)
         block->setYoffset(0);
               
      // need to exchange all subblocknum
      Exchange_SubBlock(doc, subblockNum, block);
      return 1;
   }

   int shape;  // can be either rectangle or oblong
   if (Complex_2PointLine(doc, &subblock->getDataList(), accuracy, &sizeA, &sizeB, &orient, &xoffset, &yoffset, &shape))
   {
      if (fabs(xoffset) < accuracy) xoffset = 0;
      if (fabs(yoffset) < accuracy) yoffset = 0;

      if (shape == T_OBLONG && fabs(sizeA - sizeB) < accuracy)
      {
         block->setShape(T_ROUND);
         block->setSizeA((DbUnit)sizeA);
         block->setSizeB(0);
         block->setSizeC(0);
         block->setSizeD(0);
         block->setXoffset((DbUnit)xoffset);
         block->setYoffset((DbUnit)yoffset);
         //block->rotation += 0;
      }
      else
      {
         block->setShape(shape);
         block->setSizeA((DbUnit)sizeA);
         block->setSizeB((DbUnit)sizeB);
         block->setSizeC(0);
         block->setSizeD(0);
         block->setXoffset((DbUnit)xoffset);
         block->setYoffset((DbUnit)yoffset);
         block->setRotation(block->getRotation() + (DbUnit)OptimizeRotation(orient));
      }     

      if (fabs(xoffset) < accuracy)
         block->setXoffset(0);
      if (fabs(yoffset) < accuracy)
         block->setYoffset(0);

      // need to exchange all subblocknum
      Exchange_SubBlock(doc, subblockNum, block);
      
      return 1;
   }

   return 0;
}

/******************************************************************************
* OptimizeComplexApertures
*/
void OptimizeComplexApertures(CCEtoODBDoc *doc, CDataList *padstackDataList, double accuracy,
   BOOL DoPowerLayers,COperationProgress* operationProgress)
{
   if (operationProgress != NULL)
   {
      operationProgress->updateStatus("Optimizing complex apertures",padstackDataList->GetCount());
   }

   POSITION dataPos = padstackDataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = padstackDataList->GetNext(dataPos);

      if (operationProgress != NULL)
      {
         operationProgress->incrementProgress();
      }

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *insertedGeom = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      OptimizeComplexApertures_Blocks(doc, insertedGeom, data->getLayerIndex(), accuracy, DoPowerLayers);
   }
}


/******************************************************************************
* update_aperture_attribute
*/
static void update_aperture_attribute(CCEtoODBDoc *doc, CAttributes* map, int keepWidthIndex, int killWidthIndex)
{
   if (map == NULL)
      return;

   for (POSITION attribPos = map->GetStartPosition();attribPos != NULL;)
   {
      WORD keyword;
      Attrib* attrib;

      map->GetNextAssoc(attribPos, keyword, attrib);

      if (attrib->getPenWidthIndex() == killWidthIndex)
         attrib->setPenWidthIndex(keepWidthIndex);
   }
}

/******************************************************************************
* update_aperture
*/
static void update_aperture(CCEtoODBDoc *doc, BlockStruct *block, int keepAp, int keepWidthIndex, int killAp, int killWidthIndex)
{
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      update_aperture_attribute(doc, data->getAttributesRef(), keepWidthIndex, killWidthIndex);

      switch(data->getDataType())
      {
      case T_POLY:
         {
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);
               if (poly->getWidthIndex() == killWidthIndex)
                  poly->setWidthIndex(keepWidthIndex);
            }
         }
         break;
      case T_TEXT:
         if (data->getText()->getPenWidthIndex() == killWidthIndex)
            data->getText()->setPenWidthIndex(keepWidthIndex);
         break;
      case T_INSERT:
         if (data->getInsert()->getBlockNumber() == killAp)
            data->getInsert()->setBlockNumber(keepAp);
         break;
      }
   }
}

/******************************************************************************
* ExchangeAperture
*/
void ExchangeAperture(CCEtoODBDoc *doc, int keepBlockNum, int killBlockNum, bool destroyKillBlock)
{
   int keepWidthIndex = -1;
   int killWidthIndex = -1;
   
   // kill it in widthtable
	int i = 0;
   for (i = 0; i<doc->getNextWidthIndex(); i++)
   {
      BlockStruct *block = doc->getWidthTable()[i];

      if (!block)
         continue;

      if (doc->getWidthTable()[i]->getBlockNumber() == killBlockNum)
      {
         doc->getWidthTable().SetAt(i,NULL);
         killWidthIndex = i;
      }
      else if (doc->getWidthTable()[i]->getBlockNumber() == keepBlockNum)
      {
         keepWidthIndex = i;
      }
   }

   // kill the killAperture block
   if (destroyKillBlock)
   {
      BlockStruct *killAperture = doc->getBlockAt(killBlockNum);

      if (killAperture->getShape() == T_COMPLEX)
      {
         BlockStruct *killComplexAperture = doc->getBlockAt((int)killAperture->getSizeA());
         doc->RemoveBlock(killComplexAperture);
      }

      doc->RemoveBlock(killAperture);
   }

   // loop through all non-aperture blocks and look at insert
   // now find all instances which may use the killapp and change it 
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      update_aperture(doc, block, keepBlockNum, keepWidthIndex, killBlockNum, killWidthIndex);
   }
}

/******************************************************************************
* OptimizeDuplicateApertures
*
*  - Takes inserts of different apertures that are the same and makes them insert the same aperture and removes the duplicate
*  - do not forget to kill an eliminated aperture from the width list.
*/
void OptimizeDuplicateApertures(CCEtoODBDoc *doc, double accuracy)
{
   COperationProgress progress;
   progress.updateStatus("Consolidating duplicate apertures",doc->getMaxBlockIndex());

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block1 = doc->getBlockAt(i);

      progress.incrementProgress();

      if (!block1)
         continue;

      if (!(block1->getFlags() & BL_APERTURE) && !(block1->getFlags() & BL_BLOCK_APERTURE))
         continue;
      
      for (int ii=i+1; ii<doc->getMaxBlockIndex(); ii++)
      {
         BlockStruct *block2 = doc->getBlockAt(ii);

         if (!block2)
            continue;

         if (!(block2->getFlags() & BL_APERTURE) && !(block2->getFlags() & BL_BLOCK_APERTURE))
            continue;

         if (IsEqualAperture(doc, block1, block2, accuracy))
            ExchangeAperture(doc, i, ii);
      }
   }
}

/******************************************************************************
* IsEqualAperture
*/
bool IsEqualAperture(CCEtoODBDoc *doc, BlockStruct* block1, BlockStruct* block2, double accuracy)
{
   if (block1->getShape() == T_COMPLEX && block2->getShape() == T_COMPLEX)
   {
      if (IsEqualComplexAperture(doc, block1, block2, accuracy))
         return true;
   }
   else if (block1->getShape() != T_COMPLEX && block2->getShape() != T_COMPLEX)
   {
      if (IsEqualBasicAperture(doc, block1, block2, accuracy))
         return true;
   }

   return false;
}

static bool isPolyClosed(CPoly *poly, double accuracy)
{
   CPnt *startPnt, *endPnt;
   POSITION pos = poly->getPntList().GetHeadPosition();
   startPnt = poly->getPntList().GetAt(pos);
   pos = poly->getPntList().GetTailPosition();
   endPnt = poly->getPntList().GetAt(pos);

   // Ignore Bulge !
   // What matters here is that start XY and end XY are the same.
   // It could be that the poly starts at A and is a curve to 2nd pt, so
   // has non-zero bulge. The last vector from Z back to A might be straight,
   // so it has zero bulge. They are still the same location, the poly is
   // still closed, but an exact test on X, Y, and Bulge will report non-match.
   // Not cool. For purposes here, match on XY is all we want or need.
   
   return (startPnt && endPnt)?PointsMatchIgnoreBulge(startPnt,endPnt,accuracy):false;
}

static int FindMatchedPointInPolys(CPoly *polydst,CPnt *pntsrc, double accuracy)
{
   int pntrcIndex = 0;

   POSITION pos = polydst->getPntList().GetHeadPosition();
   while (pos)
   {
      CPnt *pntdst = polydst->getPntList().GetNext(pos);
      if (PointsMatch(pntsrc,pntdst,accuracy))
         return pntrcIndex;

      pntrcIndex ++;
   }
   return -1;
}

static bool OrderedPolysMatch(bool isClosed, CPoly *poly1, CPoly *poly2, double accuracy)
{
   int basePoly2Index = -1;
 
   // Search first point of poly1 in poly2
   POSITION pos1 = poly1->getPntList().GetHeadPosition();
   if(pos1)
   {
      CPnt *pnt1 = poly1->getPntList().GetAt(pos1);
      basePoly2Index = FindMatchedPointInPolys(poly2,pnt1,accuracy);
   }

   if(basePoly2Index < 0)
      return false;

   //Check if the matched points in poly1 and poly2 have the same order
   int Poly1Cnt = poly1->getPntList().GetCount();
   if (isClosed)
      Poly1Cnt -= 1;  // Skip redundant last point, same as first point

   for(int Poly1Index = 0; Poly1Index < Poly1Cnt; Poly1Index++)
   {
      POSITION pos1 = poly1->getPntList().FindIndex(Poly1Index);
      if(!pos1)
         return false;

      CPnt *pnt1 = poly1->getPntList().GetAt(pos1);

      //Search the correspondent point in poly2 based on sequence of poly1
      int shiftPoly2Index = basePoly2Index + Poly1Index;
      if(isClosed)
         shiftPoly2Index = shiftPoly2Index % Poly1Cnt; // Use count adjusted for isClosed, not raw count,
                                                       // this compensates for different redundant
                                                       // start & end pts in the polys.

      POSITION pos2 = poly2->getPntList().FindIndex(shiftPoly2Index);
      if(!pos2)
         return false;

      CPnt *pnt2 = poly2->getPntList().GetAt(pos2);
      if(!PointsMatch(pnt1,pnt2,accuracy))
         return false;
   }

   return true;
}

bool PolysMatch(CCEtoODBDoc *doc, CPoly *poly1, CPoly *poly2, double accuracy)
{
   // Return true if polys are equivalent, otherwise false

   // If exactly the same then they are equivalent, that includes both NULL
   if (poly1 == poly2)
      return true;

   if (poly1 != NULL && poly2 != NULL)
   {
      // If polys have different point count then they are different
      if (poly1->getPntList().GetCount() != poly2->getPntList().GetCount())
         return false;

      // Both polys must have same Closed flag
      if (poly1->isClosed() != poly2->isClosed())
         return false;

      // Both polys must have same Fill flag
      if (poly1->isFilled() != poly2->isFilled())
         return false;

      // Polys must have same pen width, but not necesarily same width index
      if (poly1->getWidthIndex() != poly2->getWidthIndex())
      {
         // Though width indices are not the same, actual width still may be, check it out
         if (poly1->getWidthIndex() > -1 && poly2->getWidthIndex() > -1)
		   {
			   BlockStruct *width1 = doc->getWidthTable()[poly1->getWidthIndex()];
            BlockStruct *width2 = doc->getWidthTable()[poly2->getWidthIndex()];
            if (width1 == NULL || width2 == NULL || width1->getSizeA() != width2->getSizeA())
            {
               return false;
            }
         }
         else
         {
            // One has a width and one does not, clearly not the same
            return false;
         }
      }

      //check if poly are closed by start and end points in poly   
      bool isClosed = isPolyClosed(poly1, accuracy);
      if( isClosed != isPolyClosed(poly2, accuracy))
         return false;      

      // Polys passed all the tests, apparantly are equivalent
      if(OrderedPolysMatch(isClosed, poly1, poly2, accuracy))
         return true;
      
      //Reverse Poly and test if polys are equivalent
      CPoly tmpPoly = *poly2;
      tmpPoly.reverse();
      if(OrderedPolysMatch(isClosed, poly1, &tmpPoly, accuracy))
         return true;
   }

   // If we get here then one poly is NULL and the other is not, clearly not the same
   return false;
}

/****************************************************************************/

// THIS IS WEAK
// It is in use because it resolves a particular problem for a particular 
// family of data sets, namely for Zuken Reader imported data. It needs
// much work in order to become truly general and robust.
// For example, it is not recursive, it does not dig deep into inserted
// blocks. It is looking for matching inserted block number, where a truly
// robust version would not stop at that and instead would dig into the 
// inserted blocks to see if content matched.

static bool BlockHasData(CCEtoODBDoc *doc, BlockStruct *b2, DataStruct *d1, double accuracy)
{
   if (b2 != NULL && d1 != NULL)
   {
      POSITION pos = b2->getHeadDataPosition();
      while (pos != NULL)
      {
         DataStruct *d2 = b2->getNextData(pos);
         if (d2->getLayerIndex() == d1->getLayerIndex())
         {
            DataTypeTag dt1 = d1->getDataType();
            DataTypeTag dt2 = d2->getDataType();
            if (dt1 == dataTypeInsert && dt2 == dataTypeInsert)
            {
               InsertStruct *ins1 = d1->getInsert();
               InsertStruct *ins2 = d2->getInsert();
               if (ins1->getBlockNumber() == ins2->getBlockNumber() &&
                  ins1->getRefname().CompareNoCase(ins2->getRefname()) == 0)
               {
                  // SMALLNUMBER is too big, case 2043 had "same" geometry definitions
                  // with differences at .0001 resolution
                  if ((fabs(ins1->getOriginX() - ins2->getOriginX()) < 0.0005) &&
                     (fabs(ins1->getOriginY() - ins2->getOriginY()) < 0.0005))
                  {
                     // Might have to get more specific, we'll try this much for now
                     return true;
                  }
               }
            }
            else if (dt1 == dataTypePoly && dt2 == dataTypePoly)
            {
               // WEAKNESS - only one poly in polylist is currently supported
               // This matches the functionality that was in IsEqualComplexAperture()
               // Prior to case dts0100419984. 
               // This could be improved to support polylists of arbitrary count.

               if (d1->getPolyList()->GetCount() == d2->getPolyList()->GetCount())
               {
                  if (d1->getPolyList()->GetCount() == 1) // both already known to have same counts
                  {
                     CPoly *poly1 = d1->getPolyList()->GetHead();
                     CPoly *poly2 = d2->getPolyList()->GetHead();
                     if (PolysMatch(doc, poly1, poly2, accuracy))
                     {
                        return true;
                     }
                  }
               }
            }
            else if (dt1 == dt2)
            {
               // really weak. Not an insert and not a poly, since both have "one of these"
               // we'll call it close enough for now.
               return true;
            }
         }
      }
   }

   return false;
}

/****************************************************************************/

bool BlocksMatch(CCEtoODBDoc *doc, BlockStruct *b1, BlockStruct *b2, double accuracy)
{
   // Return true if block content matches, false otherwise. Block name does not matter.

   if (b1 != NULL && b2 != NULL)
   {
      // Check that block 2 has all that 1 has
      POSITION pos1 = b1->getHeadDataPosition();
      while (pos1 != NULL)
      {
         DataStruct *d1 = b1->getNextData(pos1);
         if (!BlockHasData(doc, b2, d1, accuracy))
            return false;
      }

      // And 1 has all that 2 has
      POSITION pos2 = b2->getHeadDataPosition();
      while (pos2 != NULL)
      {
         DataStruct *d2 = b2->getNextData(pos2);
         if (!BlockHasData(doc, b1, d2, accuracy))
            return false;
      }
   }

   return true; // no differences found
}

/******************************************************************************
* IsEqualComplex
*/
static bool IsEqualComplexAperture(CCEtoODBDoc *doc, BlockStruct* apblock1, BlockStruct* apblock2, double accuracy)
{
   // If both aperture blocks reference the same geometry block then they are clearly the same
   if (apblock1->getComplexApertureSubBlockNumber() == apblock2->getComplexApertureSubBlockNumber())
      return true;

   BlockStruct *complex1 = doc->getBlockAt((int)apblock1->getComplexApertureSubBlockNumber());
   BlockStruct *complex2 = doc->getBlockAt((int)apblock2->getComplexApertureSubBlockNumber());

   // If the geom blocks do not have the same data count then they can not be the same
   if (complex1->getDataList().GetCount() != complex2->getDataList().GetCount())
      return false;

   return BlocksMatch(doc, complex1, complex2, accuracy);
}


/******************************************************************************
* IsEqualBasic
*/
static bool IsEqualBasicAperture(CCEtoODBDoc *doc, BlockStruct* block1, BlockStruct* block2, double accuracy)
{
   if (block1->getShape() != block2->getShape())
      return false;

   if (block1->getSpokeCount() != block2->getSpokeCount())
      return false;

   if (fabs(block1->getSizeA() - block2->getSizeA()) > accuracy)
      return false;
   if (fabs(block1->getSizeB() - block2->getSizeB()) > accuracy)
      return false;
   if (fabs(block1->getSizeC() - block2->getSizeC()) > accuracy)
      return false;
   if (fabs(block1->getSizeD() - block2->getSizeD()) > accuracy)
      return false;
   if (fabs(block1->getSizeD() - block2->getSizeD()) > accuracy)
      return false;

   if (fabs(block1->getXoffset() - block2->getXoffset()) > accuracy)
      return false;
   if (fabs(block1->getYoffset() - block2->getYoffset()) > accuracy)
      return false;

   if (fabs(block1->getRotation() - block2->getRotation()) > accuracy)
      return false;

   return true;
}

/******************************************************************************
* OptimizeCoveredApertures
*/
void OptimizeCoveredApertures(CCEtoODBDoc *doc, CDataList *dataList)
{
   Mat2x2 m;
   RotMat2(&m, 0.0);

   POSITION dataPos1 = dataList->GetHeadPosition();
   while (dataPos1)
   {
      DataStruct *data1 = dataList->GetAt(dataPos1); // GetAt() because dataPos2 might remove the next entity, then dataPos1 would be bad

      if (data1->getDataType() != dataTypeInsert)
      {
         dataList->GetNext(dataPos1);
         continue;
      }

      BlockStruct *block1 = doc->getBlockAt(data1->getInsert()->getBlockNumber());
      if (!(block1->getFlags() & BL_APERTURE || block1->getFlags() & BL_BLOCK_APERTURE))
      {
         dataList->GetNext(dataPos1);
         continue;
      }

      ExtentRect rect1;
      BOOL skipAperture = FALSE;

      switch (block1->getShape())
      {
      case T_COMPLEX:
         {
            BlockStruct *subblock = doc->getBlockAt((int)block1->getSizeA());

            // Check to see if the polyline is closed, only closed poly is use to check for covered apertures
            POSITION dataPos = subblock->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = subblock->getDataList().GetNext(dataPos);
               
               if (data->getDataType() != T_POLY)
                  continue;

               // only check for first poly of the first poly structure
               // because the purpose of this check is for simple single polyline 
               CPoly *poly = data->getPolyList()->GetHead();
               if (!poly->isClosed())
               {
                  skipAperture = TRUE;
                  break;
               }
            }

            if (skipAperture)
            {
               dataList->GetNext(dataPos1);
               continue;
            }

            // Poly is closed, so get extent to check for covered apertures
            doc->validateBlockExtents(subblock);

            rect1.left   = data1->getInsert()->getOriginX() + subblock->getXmin();
            rect1.right  = data1->getInsert()->getOriginX() + subblock->getXmax();
            rect1.bottom = data1->getInsert()->getOriginY() + subblock->getYmin();
            rect1.top    = data1->getInsert()->getOriginY() + subblock->getYmax();
         }
         break;
      //case T_SQUARE:
      //case T_ROUND:
      //   rect1.left = data1->getInsert()->getOriginX() - (block1->getSizeA()/2 + block1->xoffset);
      //   rect1.right = data1->getInsert()->getOriginX() + (block1->getSizeA()/2 + block1->xoffset);
      //   rect1.bottom = data1->getInsert()->getOriginY() - (block1->getSizeA()/2 + block1->yoffset);
      //   rect1.top = data1->getInsert()->getOriginY() + (block1->getSizeA()/2 + block1->yoffset);
      //   break;
      //case T_RECTANGLE:
      //case T_OBLONG:
      //   rect1.left = data1->getInsert()->getOriginX() - (block1->getSizeA()/2 + block1->xoffset);
      //   rect1.right = data1->getInsert()->getOriginX() + (block1->getSizeA()/2 + block1->xoffset);
      //   rect1.bottom = data1->getInsert()->getOriginY() - (block1->getSizeB()/2 + block1->yoffset);
      //   rect1.top = data1->getInsert()->getOriginY() + (block1->getSizeB()/2 + block1->yoffset);
      //   break;
      //default:
      //   dataList->GetNext(dataPos1);
      //   continue;
      default:
         {
            int degrees = round(RadToDeg(block1->getRotation()));
            if (degrees % 90) // do not do non-orthogonal apertures
            {
               dataList->GetNext(dataPos1);
               continue;
            }

            CPolyList *polylist = ApertureToPoly(block1, data1->getInsert(), 0);
            if (polylist)
            {
               PolyExtents(doc, polylist, &rect1, 1, 0, 0, 0, &m, TRUE);
               FreePolyList(polylist);
            }
         }
      }


      POSITION dataPos2 = dataList->GetHeadPosition();
      while (dataPos2)
      {
         DataStruct *data2 = dataList->GetNext(dataPos2);

         if (data2 == data1)
            continue;

         if (data2->getDataType() != dataTypeInsert)
            continue;

         if (data2->getLayerIndex() != data1->getLayerIndex())
            continue;

         BlockStruct *block2 = doc->getBlockAt(data2->getInsert()->getBlockNumber());
         if (! ((block2->getFlags() & BL_APERTURE) || (block2->getFlags() & BL_BLOCK_APERTURE)) )
            continue;

         ExtentRect rect2;

         switch (block2->getShape())
         {
         case T_COMPLEX:
            {
               BlockStruct *subblock = doc->getBlockAt((int)block2->getSizeA());
               doc->validateBlockExtents(subblock);

               rect2.left   = data2->getInsert()->getOriginX() + subblock->getXmin();
               rect2.right  = data2->getInsert()->getOriginX() + subblock->getXmax();
               rect2.bottom = data2->getInsert()->getOriginY() + subblock->getYmin();
               rect2.top    = data2->getInsert()->getOriginY() + subblock->getYmax();
            }
            break;
         //case T_SQUARE:
         //case T_ROUND:
         //   rect2.left = data2->getInsert()->getOriginX() - (block2->getSizeA()/2 + block2->xoffset);
         //   rect2.right = data2->getInsert()->getOriginX() + (block2->getSizeA()/2 + block2->xoffset);
         //   rect2.bottom = data2->getInsert()->getOriginY() - (block2->getSizeA()/2 + block2->yoffset);
         //   rect2.top = data2->getInsert()->getOriginY() + (block2->getSizeA()/2 + block2->yoffset);
         //   break;
         //case T_RECTANGLE:
         //case T_OBLONG:
         //   rect2.left = data2->getInsert()->getOriginX() - (block2->getSizeA()/2 + block2->xoffset);
         //   rect2.right = data2->getInsert()->getOriginX() + (block2->getSizeA()/2 + block2->xoffset);
         //   rect2.bottom = data2->getInsert()->getOriginY() - (block2->getSizeB()/2 + block2->yoffset);
         //   rect2.top = data2->getInsert()->getOriginY() + (block2->getSizeB()/2 + block2->yoffset);
         //   break;
         //default:
         //   continue;
         default:
            {
               CPolyList *polylist = ApertureToPoly(block2, data2->getInsert(), 0);
               if (polylist)
               {
                  PolyExtents(doc, polylist, &rect2, 1, 0, 0, 0, &m, TRUE);
                  FreePolyList(polylist);
               }
            }
         }

         // remove if rect1 is slightliy bigger
         if (rect1.right > rect2.right + SMALLNUMBER && rect1.left < rect2.left - SMALLNUMBER && 
             rect1.top > rect2.top + SMALLNUMBER && rect1.bottom < rect2.bottom - SMALLNUMBER)
            RemoveOneEntityFromDataList(doc, dataList, data2);
      }

      dataList->GetNext(dataPos1);
   }
}

/******************************************************************************
* Normalize_PADSTACKLayers
*  this puts all layers to the top insert and floats all existing once
*/
static int Normalize_PADSTACKLayers(CCEtoODBDoc *doc, BlockStruct *block )
{
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      // this first insert is on a layer -- stop all other stuff
      if (!doc->IsFloatingLayer(data->getLayerIndex()))
         return 1;

      if (data->getDataType() != T_INSERT)
         continue;

      switch (data->getInsert()->getInsertType())
      {
      case INSERTTYPE_RELIEFPAD:
      case INSERTTYPE_CLEARPAD:
      case INSERTTYPE_OBSTACLE:
      case INSERTTYPE_DRILLSYMBOL:
         continue;
      }

      BlockStruct *insertedGeom = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      // block_layer is not set.

      // a padstack must start with a aperture (it also can have a drill and attributes)
      if (insertedGeom->getFlags() & BL_APERTURE || insertedGeom->getFlags() & BL_BLOCK_APERTURE)
      {
         if (insertedGeom->getShape() == T_COMPLEX)
            insertedGeom = doc->Find_Block_by_Num((int)insertedGeom->getSizeA());

         // find a layer inside this aperture structure. it normally can only happen on complex aperture, but just in case...
         int layerNum = Get_PADSTACKLayer(doc, &insertedGeom->getDataList());

         // now put this layernumber on the insert and float all other layers.
         Set_PADSTACKLayer(doc, &insertedGeom->getDataList(), 0);
         data->setLayerIndex(layerNum);
      }
   }

   return 1;
}

/******************************************************************************
* Make_Complex_from_PadstackGraphic
*/
static int Make_Complex_from_PadstackGraphic(CCEtoODBDoc *doc, BlockStruct *padstackGeom)
{
   int cnt = 0;
   int floatingLayerNum = Graph_Level("0", "", 1);

   POSITION dataPos = padstackGeom->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = padstackGeom->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_POLY)
         continue;

      // make a geom with layername
      CString padGeomName, complexGeomName;
      padGeomName.Format("%s_PADSHAPE_%d", padstackGeom->getName(), ++cnt);
      complexGeomName.Format("%s_COMPLEX_%d", padstackGeom->getName(), cnt);

      BlockStruct *padGeom = Graph_Block_On(GBO_APPEND, padGeomName, padstackGeom->getFileNumber(), 0);
      int layerNum = data->getLayerIndex();   
      
      MoveEntityToAnotherDataList(data, &padstackGeom->getDataList(), &padGeom->getDataList()); // move data into geom
      
      data->setLayerIndex(floatingLayerNum); // set layer to floating
      
      Graph_Block_Off(); // close geom

      Graph_Complex(padGeom->getFileNumber(), complexGeomName, 0, padGeomName, 0.0, 0.0, 0.0); // make complex graph

      Graph_Block_Reference(complexGeomName, NULL, padstackGeom->getFileNumber(), 0.0, 0.0, 0.0, 0, 1.0, layerNum, TRUE); // attach to block.
   }

   return 0;
}

/******************************************************************************
* Resolve_Hierach_PadStack
*/
static int Resolve_Hierach_PadStack(CCEtoODBDoc *doc, BlockStruct *block)
{
   BOOL Found = FALSE;
   
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *insertedGeom = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

      if (insertedGeom->getFlags() & BL_TOOL || insertedGeom->getFlags() & BL_BLOCK_TOOL)
      {
         // do not explode drill
      }
      else if (insertedGeom->getFlags() & BL_APERTURE || insertedGeom->getFlags() & BL_BLOCK_APERTURE)
      {
         // do not explode aperture
      }
      else
      {
         //ExplodeInsert(doc, data, &block->getDataList());
         Found = TRUE;
      }
   }

   return Found;
}

/******************************************************************************
* resolve_hierachical_padstack
   a padstack can not have another hierachical insert
*/
static int resolve_hierachical_padstack(CCEtoODBDoc *doc)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *padstackGeom = doc->getBlockAt(i);

      if (padstackGeom == NULL)
         continue;

      if (padstackGeom->getBlockType() != BLOCKTYPE_PADSTACK)
         continue;

      while (Resolve_Hierach_PadStack(doc, padstackGeom))
         ; // do the resolve until done.

      // now make complex apertures from the graphic pieces
      Graph_Block_On(GBO_APPEND, padstackGeom->getName(), padstackGeom->getFileNumber(), 0);
      Make_Complex_from_PadstackGraphic(doc, padstackGeom);
      Graph_Block_Off();
   }

   return 1;
}

/******************************************************************************
* OptimizePadstacks 
   Mentor defines mostly oblong pads as a 
   1. rectangle with 2 adjacent circles

   this will be optimized into a T_OBLONG APERTURE
   if set, complex power planes will be converted to thermals
*/
void OptimizePadstacks(CCEtoODBDoc *doc, int pageunits, BOOL DoPowerLayers,COperationProgress* operationProgress)
{
   doc->StoreDocForImporting();

   resolve_hierachical_padstack(doc);

   // accuracy is dependend on Page units
   // on INCH 0.001 is OK
   // on MILS 1
   // on MM   0.0254
   double accuracy = get_accuracy(doc);

   if (operationProgress != NULL)
   {
      operationProgress->setLength(doc->getMaxBlockIndex());
   }

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      if (operationProgress != NULL)
      {
         operationProgress->incrementProgress();
      }

      BlockStruct *padstackGeom = doc->getBlockAt(i);

      if (padstackGeom == NULL)
         continue;

      if (padstackGeom->getBlockType() != BLOCKTYPE_PADSTACK)
         continue;

      Normalize_PADSTACKLayers(doc, padstackGeom);
      OptimizeComplexApertures(doc, &padstackGeom->getDataList(), accuracy, DoPowerLayers);
   }
}

/******************************************************************************
* get_pindata_from_comppinnr
*/
DataStruct *get_pindata_from_comppinnr(CCEtoODBDoc *doc, BlockStruct *block, int pinnr)
{
   POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);
      
      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN && data->getInsert()->getInsertType() != INSERTTYPE_MECHANICALPIN)
         continue;

      // this can not be compares against refname but ATT_COMPPINNR, because if 1 2 is changed to 2 1 then it will be wrong.

      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_COMPPINNR, 0);
      if (attrib && attrib->getValueType() == VT_INTEGER && pinnr == attrib->getIntValue())
         return data;
   }

   return NULL;
}

/******************************************************************************
* already_checkvalue
*/
static int already_checkvalue(const char *value)
{
   for (int i=0; i<checkvaluecnt; i++)
   {
      CHECKVALUE_List *checkValue = checkvaluearray[i];
      if (!checkValue->oldvalue.CompareNoCase(value))
         return i;
   }
   return -1;
}

/******************************************************************************
* clean_attribute
*/
static int clean_attribute(CCEtoODBDoc *doc, CAttributes** map, CString refname)
{
   WORD keyword = doc->IsKeyWord(ATT_VALUE, 0);

   Attrib *attrib = is_attvalue(doc, *map, ATT_VALUE, 0);
   if (!attrib)
      return -1;

   CString value = get_attvalue_string(doc, attrib);
   if (!value.Compare("0"))
   {
      RemoveAttrib(keyword, map);
      return 0;
   }
   if (value.IsEmpty())
   {
      RemoveAttrib(keyword, map);
      return 0;
   }

   CString valueUnit, valueString;
   double valueNumber;
	ComponentValues compValue(value);
   int errorCount = 0;

   if (compValue.GetValue(valueNumber, valueUnit))
   {
		if (compValue.GetPrintableString(valueString))
         attrib->setStringValueIndex(doc->RegisterValue(valueString));
      else
      {
         CString tmp;
         tmp.Format("Can not Pretty Print [%s]", value);
         ErrorMessage(tmp, "Pretty Print Component Value");
      }
   }
   else 
   {
      int res = already_checkvalue(value);
      if (res > -1)
      {
         if (strlen(checkvaluearray[res]->newvalue))
            attrib->setStringValueIndex(doc->RegisterValue(checkvaluearray[res]->newvalue));
         else
            RemoveAttrib(keyword, map);
      }

      CString tmp;
      if (refname.IsEmpty())
         tmp.Format("Can not normalize Value [%s]", value);
      else
         tmp.Format("Can not normalize Value [%s] for [%s]", value, refname);

      tmp += "\n\nPlease enter the Value.";
      tmp += "\n(Empty Input fields will eliminate the VALUE field)!";

      InputDlg dlg;
      dlg.m_prompt = tmp;
   
      if (dlg.DoModal() == IDOK)
      {
         if (dlg.m_input.IsEmpty())
            RemoveAttrib(keyword, map);
         else
            attrib->setStringValueIndex(doc->RegisterValue(dlg.m_input));

         CHECKVALUE_List *checkValue = new CHECKVALUE_List;
         checkvaluearray.SetAtGrow(checkvaluecnt++, checkValue);
         checkValue->oldvalue = value;
         checkValue->newvalue = dlg.m_input;
      }

      errorCount++;
   }

   return errorCount;
}

/******************************************************************************
* check_value_attributes
*/
void check_value_attributes(CCEtoODBDoc *doc)
{
   CWaitCursor wait;
   int total_error;
   int skipDeviceType = FALSE;

   checkvaluearray.SetSize(100,100);
   checkvaluecnt = 0;

	int i=0;
   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)
         continue;

      total_error = 1; // just to start
      while (total_error > 0)
      {
         total_error = 0;
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);
            if (data->getDataType() != T_INSERT)
               continue;

            total_error = clean_attribute(doc, &(data->getAttributesRef()), data->getInsert()->getRefname());
            if (total_error > -1)
               skipDeviceType = TRUE;
         }
      }
   }

   if (!skipDeviceType) //If no ATT_VALUE is found at insert level, then check device type level
   {
      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos)
      {
         FileStruct *file = doc->getFileList().GetNext(pos);
		   if (!file)
			   continue;

         total_error = TRUE;
         while (total_error > 0)
         {
            POSITION typePos = file->getTypeList().GetHeadPosition();
            total_error = 0;
            while (typePos)
            {                                         
               TypeStruct *type = file->getTypeList().GetNext(typePos);
               total_error = clean_attribute(doc, &(type->getAttributesRef()), type->getName());
            }
         }
      }
   }

   for (i=0; i<checkvaluecnt; i++)
      delete checkvaluearray[i];
   checkvaluearray.RemoveAll();
}

/******************************************************************************
* get_pincnt
*/
int get_pincnt(CDataList *dataList)
{
   int pincnt = 0;

   POSITION pos = dataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = dataList->GetNext(pos);
      if (data->getDataType() == T_INSERT)         
      {
         if (data->getInsert()->getInsertType() == INSERTTYPE_PIN)
            pincnt++;
      }
   }
   return pincnt;
}

class PinData
{
public:
   DataStruct *data;
   CDblRect pinExtent;

   PinData() {data = NULL;};
   ~PinData() {};
};

int get_insidepadsextents_twopins(CCEtoODBDoc *doc, CDataList *dataList, 
                                  double *xmin, double *ymin, double *xmax, double *ymax)
{
   // Get pin data
   PinData pnData[2];

   int i = 0;
   POSITION pos = dataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = dataList->GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN && data->getInsert()->getInsertType() != insertTypeDiePin)
         continue;

      // Make sure we don't get more than 2 pins
      if (i >= 2)
         return FALSE;

      double lxmin, lxmax, lymin, lymax;
      BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
      block_extents(doc, &lxmin, &lxmax, &lymin, &lymax, &(block->getDataList()), 0.0, 0.0, 0.0, 0, 1, -1, FALSE);

      pnData[i].data = data;
      pnData[i].pinExtent.xMin = lxmin + data->getInsert()->getOriginX();
      pnData[i].pinExtent.xMax = lxmax + data->getInsert()->getOriginX();
      pnData[i].pinExtent.yMin = lymin + data->getInsert()->getOriginY();
      pnData[i].pinExtent.yMax = lymax + data->getInsert()->getOriginY();

      i++;
   }

   // make sure we got two pins
   if (!pnData[0].data || !pnData[1].data)
      return FALSE;
   
   if (fabs(pnData[0].data->getInsert()->getOriginX() - pnData[1].data->getInsert()->getOriginX()) <= SMALLNUMBER)
   {
      // If they are lined up vertically
      *xmin = min(pnData[0].pinExtent.xMin, pnData[1].pinExtent.xMin);
      *xmax = max(pnData[0].pinExtent.xMax, pnData[1].pinExtent.xMax);

      if (pnData[0].pinExtent.yMin < pnData[1].pinExtent.yMin)
      {
         *ymin = pnData[0].pinExtent.yMax;
         *ymax = pnData[1].pinExtent.yMin;
      }
      else
      {
         *ymin = pnData[1].pinExtent.yMax;
         *ymax = pnData[0].pinExtent.yMin;
      }
   }
   else if (fabs(pnData[0].data->getInsert()->getOriginY() - pnData[1].data->getInsert()->getOriginY()) <= SMALLNUMBER)
   {
      // If they are lined up horzonally
      *ymin = min(pnData[0].pinExtent.yMin, pnData[1].pinExtent.yMin);
      *ymax = max(pnData[0].pinExtent.yMax, pnData[1].pinExtent.yMax);

      if (pnData[0].pinExtent.xMin < pnData[1].pinExtent.xMin)
      {
         *xmin = pnData[0].pinExtent.xMax;
         *xmax = pnData[1].pinExtent.xMin;
      }
      else
      {
         *xmin = pnData[1].pinExtent.xMax;
         *xmax = pnData[0].pinExtent.xMin;
      }
   }
   else
      return FALSE;

   return TRUE;
}


/******************************************************************************
 ******************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////
// RowList Class
////////////////////////////////////////////////////////////////////////////////////////
class RowList
{
public:
   double row_coord;
   CPtrList pinExtentList;

   RowList(double coord) {row_coord = coord;};
   ~RowList()
   {
      POSITION pos = pinExtentList.GetHeadPosition();
      while (pos)
      {
         CDblRect *rect = (CDblRect*)pinExtentList.GetNext(pos);

         delete rect;
      }
      pinExtentList.RemoveAll();
   };
};

////////////////////////////////////////////////////////////////////////////////////////
// PinItems Class
//		Used to get pin inner extents of pads
////////////////////////////////////////////////////////////////////////////////////////
class PinItems
{
public:
	PinItems(CCEtoODBDoc* cur_doc)
	{
		doc = cur_doc;
	};

	~PinItems()
	{
		// clean up everything
		for (int i=0; i<pinRows.GetCount(); i++)
		{
			RowList *rowItem = (RowList*)pinRows[i];
			
			rowItem->pinExtentList.RemoveAll();

			delete rowItem;
		}
		pinRows.RemoveAll();

		// clean up everything
		for (int i=0; i<pinCols.GetCount(); i++)
		{
			RowList *colItem = (RowList*)pinCols[i];

			colItem->pinExtentList.RemoveAll();

			delete colItem;
		}
		pinCols.RemoveAll();

      POSITION pos = pinExtentLib.GetHeadPosition();
      while (pos)
      {
         CDblRect *rect = (CDblRect*)pinExtentLib.GetNext(pos);

         delete rect;
      }
      pinExtentLib.RemoveAll();
	};

	void AddItem(DataStruct *data, double x, double y, double rot, int mir, double scale, int layer);
	void CleanItems();
	int GetInnerExtents(double *xmin, double *ymin, double *xmax, double *ymax);
private:
	CPtrList pinExtentLib;
	CPtrArray pinRows;
	CPtrArray pinCols;

	CCEtoODBDoc* doc;

	int getInnerExtents_twoRows(double *xmin, double *ymin, double *xmax, double *ymax);
	int getInnerExtents_twoCols(double *xmin, double *ymin, double *xmax, double *ymax);
	int getInnerExtents_twoRowsAndCols(double *xmin, double *ymin, double *xmax, double *ymax);
};

/******************************************************************************
* PinItems::getInnerExtents_twoRows
*/
int PinItems::getInnerExtents_twoRows(double *xmin, double *ymin, double *xmax, double *ymax)
{
	if (pinRows.GetCount() != 2 || pinCols.GetCount() != 0)
		return FALSE;

	// find the inner pin extents
   CDblRect innerPinExtent(FALSE);
	innerPinExtent.xMax = innerPinExtent.xMin = (*xmax + *xmin) / 2;

	// Use rows of pin extents to calculate the inner extents, if possible
   if (pinRows.GetCount() == 2)
   {
      // which row of pins is on top
      int topRow = ((((RowList*)pinRows[0])->row_coord > ((RowList*)pinRows[1])->row_coord)?0:1);

      // process the top row
      for (int i=0; i<=1; i++)
      {
         RowList *rowItem = (RowList*)pinRows[i];

         POSITION pos = rowItem->pinExtentList.GetHeadPosition();
         while (pos)
         {
            CDblRect *rect = (CDblRect*)rowItem->pinExtentList.GetNext(pos);
            
            // make sure we take the full width of the pins
            if (innerPinExtent.xMin > rect->xMin)
               innerPinExtent.xMin = rect->xMin;
            if (innerPinExtent.xMax < rect->xMax)
               innerPinExtent.xMax = rect->xMax;

            if (i == topRow)
            {
               if (innerPinExtent.yMax > rect->yMin)
                  innerPinExtent.yMax = rect->yMin;
            }
            else
            {
               if (innerPinExtent.yMin < rect->yMax)
                  innerPinExtent.yMin = rect->yMax;
            }
         }
      }
   }

	if (innerPinExtent.xMax <= innerPinExtent.xMin || innerPinExtent.yMax <= innerPinExtent.yMin)
      return FALSE;

	*xmin = innerPinExtent.xMin;
   *ymin = innerPinExtent.yMin;
   *xmax = innerPinExtent.xMax;
   *ymax = innerPinExtent.yMax;

	return TRUE;
}

/******************************************************************************
* PinItems::getInnerExtents_twoCols
*/
int PinItems::getInnerExtents_twoCols(double *xmin, double *ymin, double *xmax, double *ymax)
{
	if (pinRows.GetCount() != 0 || pinCols.GetCount() != 2)
		return FALSE;

	// find the inner pin extents
   CDblRect innerPinExtent(FALSE);
   innerPinExtent.yMax = innerPinExtent.yMin = (*ymax + *ymin) / 2;

   // Use cols of pin extents to calculate the inner extents, if possible
   if (pinCols.GetCount() == 2)
   {
      // which row of pins is on top
      int topRow = ((((RowList*)pinCols[0])->row_coord > ((RowList*)pinCols[1])->row_coord)?0:1);

      // process the top row
      for (int i=0; i<=1; i++)
      {
         RowList *rowItem = (RowList*)pinCols[i];

         POSITION pos = rowItem->pinExtentList.GetHeadPosition();
         while (pos)
         {
            CDblRect *rect = (CDblRect*)rowItem->pinExtentList.GetNext(pos);
            
            // make sure we take the full height of the pins
            if (innerPinExtent.yMin > rect->yMin)
               innerPinExtent.yMin = rect->yMin;
            if (innerPinExtent.yMax < rect->yMax)
               innerPinExtent.yMax = rect->yMax;

            if (i == topRow)
            {
               if (innerPinExtent.xMax > rect->xMin)
                  innerPinExtent.xMax = rect->xMin;
            }
            else
            {
               if (innerPinExtent.xMin < rect->xMax)
                  innerPinExtent.xMin = rect->xMax;
            }
         }
      }
   }

	if (innerPinExtent.xMax <= innerPinExtent.xMin || innerPinExtent.yMax <= innerPinExtent.yMin)
      return FALSE;

	*xmin = innerPinExtent.xMin;
   *ymin = innerPinExtent.yMin;
   *xmax = innerPinExtent.xMax;
   *ymax = innerPinExtent.yMax;

	return TRUE;
}

/******************************************************************************
* PinItems::getInnerExtents_twoRowsAndCols
*/
int PinItems::getInnerExtents_twoRowsAndCols(double *xmin, double *ymin, double *xmax, double *ymax)
{
	if (pinRows.GetCount() != 2 || pinCols.GetCount() != 2)
		return FALSE;

	// find the inner pin extents
   CDblRect innerPinExtent(FALSE);

	// Use rows of pin extents to calculate the inner extents, if possible
   if (pinRows.GetCount() == 2)
   {
      // which row of pins is on top
      int topRow = ((((RowList*)pinRows[0])->row_coord > ((RowList*)pinRows[1])->row_coord)?0:1);

      // process the top row
      for (int i=0; i<=1; i++)
      {
         RowList *rowItem = (RowList*)pinRows[i];

         POSITION pos = rowItem->pinExtentList.GetHeadPosition();
         while (pos)
         {
            CDblRect *rect = (CDblRect*)rowItem->pinExtentList.GetNext(pos);
            
            if (i == topRow)
            {
               if (innerPinExtent.yMax > rect->yMin)
                  innerPinExtent.yMax = rect->yMin;
            }
            else
            {
               if (innerPinExtent.yMin < rect->yMax)
                  innerPinExtent.yMin = rect->yMax;
            }
         }
      }
   }

   // Use cols of pin extents to calculate the inner extents, if possible
   if (pinCols.GetCount() == 2)
   {
      // which row of pins is on top
      int topRow = ((((RowList*)pinCols[0])->row_coord > ((RowList*)pinCols[1])->row_coord)?0:1);

      // process the top row
      for (int i=0; i<=1; i++)
      {
         RowList *rowItem = (RowList*)pinCols[i];

         POSITION pos = rowItem->pinExtentList.GetHeadPosition();
         while (pos)
         {
            CDblRect *rect = (CDblRect*)rowItem->pinExtentList.GetNext(pos);
            
            if (i == topRow)
            {
               if (innerPinExtent.xMax > rect->xMin)
                  innerPinExtent.xMax = rect->xMin;
            }
            else
            {
               if (innerPinExtent.xMin < rect->xMax)
                  innerPinExtent.xMin = rect->xMax;
            }
         }
      }
   }

	if (innerPinExtent.xMax <= innerPinExtent.xMin || innerPinExtent.yMax <= innerPinExtent.yMin)
      return FALSE;

   *xmin = innerPinExtent.xMin;
   *ymin = innerPinExtent.yMin;
   *xmax = innerPinExtent.xMax;
   *ymax = innerPinExtent.yMax;

	return TRUE;
}

/******************************************************************************
* PinItems::AddItem
*/
void PinItems::AddItem(DataStruct *data, double x, double y, double rot, int mir, double scale, int layer)
{
   // find the row this pin belongs too
   int row_cnt;
   for (row_cnt=0; row_cnt<pinRows.GetCount(); row_cnt++)
   {
      RowList *rowItem = (RowList*)pinRows[row_cnt];
      if (fabs(data->getInsert()->getOriginY() - rowItem->row_coord) <= SMALLNUMBER)
         break;
   }

   // find the col this pin belongs too
   int col_cnt;
   for (col_cnt=0; col_cnt<pinCols.GetCount(); col_cnt++)
   {
      RowList *colItem = (RowList*)pinCols[col_cnt];
      if (fabs(data->getInsert()->getOriginX() - colItem->row_coord) <= SMALLNUMBER)
         break;
   }

	// get the extents of the pin
   double lxmin, lxmax, lymin, lymax;
   BlockStruct *block = doc->getBlockAt(data->getInsert()->getBlockNumber());
   block_extents(doc, &lxmin, &lxmax, &lymin, &lymax, &(block->getDataList()), x, y, rot, mir, scale, layer, FALSE);
   CDblRect *rect = new CDblRect(lxmin + data->getInsert()->getOriginX(), lxmax + data->getInsert()->getOriginX(), 
                                 lymin + data->getInsert()->getOriginY(), lymax + data->getInsert()->getOriginY());

	// Add to the list of all pin extents
	pinExtentLib.AddTail(rect);

	// add the pin to the list of rows
   if (row_cnt < pinRows.GetCount())
   {
      // if we've found a row, add the pin extents to it
      RowList *rowItem = (RowList*)pinRows[row_cnt];

      rowItem->pinExtentList.AddTail(rect);
   }
   else
   {
      // if we didn't find a row, add a new row and add the pin extents to that row
      RowList *rowItem = new RowList(data->getInsert()->getOriginY());
      pinRows.Add(rowItem);

      rowItem->pinExtentList.AddTail(rect);
   }

	// add the pin to the list of cols
   if (col_cnt < pinCols.GetCount())
   {
      // if we've found a row, add the pin extents to it
      RowList *rowItem = (RowList*)pinCols[col_cnt];

      rowItem->pinExtentList.AddTail(rect);
   }
   else
   {
      // if we didn't find a row, add a new row and add the pin extents to that row
      RowList *rowItem = new RowList(data->getInsert()->getOriginX());
      pinCols.Add(rowItem);

      rowItem->pinExtentList.AddTail(rect);
   }
}

/******************************************************************************
* PinItems::CleanItems
*/
void PinItems::CleanItems()
{
   // remove rows that have only two items or less
   int row_cnt;
   for (row_cnt=0; row_cnt<pinRows.GetCount(); row_cnt++)
   {
      RowList *rowItem = (RowList*)pinRows[row_cnt];

		if (rowItem->pinExtentList.GetCount() < 3)
		{
			pinRows.RemoveAt(row_cnt--);

			rowItem->pinExtentList.RemoveAll();
			delete rowItem;
		}
   }

   // remove cols that have only two items or less
   int col_cnt;
   for (col_cnt=0; col_cnt<pinCols.GetCount(); col_cnt++)
   {
      RowList *colItem = (RowList*)pinCols[col_cnt];
		if (colItem->pinExtentList.GetCount() < 3)
		{
			pinCols.RemoveAt(col_cnt--);

			colItem->pinExtentList.RemoveAll();
			delete colItem;
		}
   }
}

/******************************************************************************
* PinItems::GetInnerExtents
*/
int PinItems::GetInnerExtents(double *xmin, double *ymin, double *xmax, double *ymax)
{
	int gotOutline = getInnerExtents_twoRows(xmin, ymin, xmax, ymax);

	if (!gotOutline)
		gotOutline = getInnerExtents_twoCols(xmin, ymin, xmax, ymax);

	if (!gotOutline)
		gotOutline = getInnerExtents_twoRowsAndCols(xmin, ymin, xmax, ymax);

	return gotOutline;
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************
/******************************************************************************
* getInsidePadsExtents_RowsCols
*		Get the pad's inner extents depending on two distict rows or columns or both
*/
int getInsidePadsExtents_RowsCols(CCEtoODBDoc *doc, CDataList *dataList, 
											 double *xmin, double *ymin, double *xmax, double *ymax)
{
   PinItems itemList(doc);

   // Pin inner outlines only work on pins with two rows or two pin components
   POSITION pinPos = dataList->GetHeadPosition();
   while (pinPos)
   {
      DataStruct *data = dataList->GetNext(pinPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN && data->getInsert()->getInsertType() != insertTypeDiePin)
         continue;

		itemList.AddItem(data, 0.0, 0.0, 0.0, 0, 1, -1);
   }

	// remove any rows and cols that have less than three items
	itemList.CleanItems();

	return itemList.GetInnerExtents(xmin, ymin, xmax, ymax);
}

/******************************************************************************
* get_insidepadsextents
* min and max is preset by pin extension
* now it needs to get shrinked.
*/
int get_insidepadsextents(CCEtoODBDoc *doc, CDataList *dataList, 
                          double *xmin, double *ymin, double *xmax, double *ymax)
{
   if (dataList->GetCount() == 1)
      return FALSE;
   
   BOOL gotOutline = get_insidepadsextents_twopins(doc, dataList, xmin, ymin, xmax, ymax);

   if (!gotOutline)
      return getInsidePadsExtents_RowsCols(doc, dataList, xmin, ymin, xmax, ymax);

	return gotOutline;
}

/******************************************************************************
* GetPinExtents
*/
int GetPinExtents(CDataList *dataList, double *xmin, double *ymin, double *xmax, double *ymax)
{
   int pinCount = 0;

   *xmin = *ymin = DBL_MAX;
   *xmax = *ymax = -DBL_MAX;

   POSITION dataPos = dataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = dataList->GetNext(dataPos);
      
      if (data->getDataType() == T_INSERT && 
         (data->getInsert()->getInsertType() == INSERTTYPE_PIN || data->getInsert()->getInsertType() == insertTypeDiePin))
      {
         if (data->getInsert()->getOriginX() < *xmin)
            *xmin = data->getInsert()->getOriginX();
         if (data->getInsert()->getOriginY() < *ymin)
            *ymin = data->getInsert()->getOriginY();
         if (data->getInsert()->getOriginX() > *xmax)
            *xmax = data->getInsert()->getOriginX();
         if (data->getInsert()->getOriginY() > *ymax)
            *ymax = data->getInsert()->getOriginY();

         pinCount++;
      }
   }

   return pinCount;
}

/******************************************************************************
* GetPinCenterExtents
*/
int GetPinCenterExtents(const CDataList& dataList, CInsertTypeMask includeInsertTypeMask, double *xmin, double *ymin, double *xmax, double *ymax)
{
   *xmin = *ymin = DBL_MAX;
   *xmax = *ymax = -DBL_MAX;

   // Always include electrical pin
   includeInsertTypeMask.add(insertTypePin);

   int pinCount = 0;
   for (CDataListIterator dataList(dataList, includeInsertTypeMask); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();

      if (data->getInsert()->getOriginX() < *xmin)
         *xmin = data->getInsert()->getOriginX();
      if (data->getInsert()->getOriginY() < *ymin)
         *ymin = data->getInsert()->getOriginY();
      if (data->getInsert()->getOriginX() > *xmax)
         *xmax = data->getInsert()->getOriginX();
      if (data->getInsert()->getOriginY() > *ymax)
         *ymax = data->getInsert()->getOriginY();

      pinCount++;
   }

   return pinCount;
}

/******************************************************************************
* get_pinpitch
*
*  This assumes the structure of CAMCAD PCB Components
*     DataList containing:    Silkscreen and Pads
*/
double get_pinpitch(BlockStruct *block, double accuracy, CCEtoODBDoc *doc)
{
	CPntList pntList;

	POSITION dataPos = block->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = block->getDataList().GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;

      if (data->getInsert()->getInsertType() != INSERTTYPE_PIN)
         continue;

		CPnt *pnt = new CPnt(data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), 0);
		pntList.AddTail(pnt);
	}

   double minPitch = FLT_MAX;

	// special handling for 3 pin components
	if (pntList.GetCount() == 3)
	{
		POSITION pntPos = pntList.GetHeadPosition();
		CPnt *pnt1 = pntList.GetNext(pntPos);
		CPnt *pnt2 = pntList.GetNext(pntPos);
		CPnt *pnt3 = pntList.GetNext(pntPos);

		// if not all colinear
		if (!(pnt1->y == pnt2->y && pnt1->y == pnt3->y || pnt1->x == pnt2->x && pnt1->x == pnt3->x))
		{
			if (fabs(pnt1->y - pnt2->y) < SMALLNUMBER)
				return round_accuracy(fabs(pnt1->x - pnt3->x), accuracy);
			else if (fabs(pnt1->y - pnt3->y) < SMALLNUMBER)
				return round_accuracy(fabs(pnt1->x - pnt2->x), accuracy);
			else if (fabs(pnt2->y - pnt3->y) < SMALLNUMBER)
				return round_accuracy(fabs(pnt2->x - pnt1->x), accuracy);
			else if (fabs(pnt1->x - pnt2->x) < SMALLNUMBER)
				return round_accuracy(fabs(pnt1->y - pnt3->y), accuracy);
			else if (fabs(pnt1->x - pnt3->x) < SMALLNUMBER)
				return round_accuracy(fabs(pnt1->y - pnt2->y), accuracy);
			else if (fabs(pnt2->x - pnt3->x) < SMALLNUMBER)
				return round_accuracy(fabs(pnt2->y - pnt1->y), accuracy);
		}
	}

	POSITION pntPos = pntList.GetHeadPosition();
   CPnt *pnt1;
	if (pntPos)
      pnt1 = pntList.GetNext(pntPos);
   while (pntPos)
   {
      CPnt *pnt2 = pntList.GetNext(pntPos);

      double dx = round_accuracy(fabs(pnt1->x - pnt2->x), accuracy);
      double dy = round_accuracy(fabs(pnt1->y - pnt2->y), accuracy);

      double distance = sqrt(dx*dx + dy*dy);

      if (distance < SMALLNUMBER)
         continue;

      if (distance < minPitch)
         minPitch = distance;
	}

   return minPitch == FLT_MAX ? 0 : minPitch;
}

/******************************************************************************
* mem_count_attrib
*/
static int mem_count_attrib(CAttributes* map, MemCheck *memcheck)
{
   if (map == NULL)
      return 0;

   WORD keyword;
   Attrib* attrib;

   int count = 0;
   count += sizeof(CAttributes);

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      count += sizeof(Attrib);

      memcheck->attribute_cnt++;
   }

   memcheck->attribute_mem += count;

   return count;
}

/******************************************************************************
* 
*/
static int mem_count(CCEtoODBDoc *doc, CDataList *DataList, MemCheck *memcheck)
{
   int cnt = 0;
   POSITION pos = DataList->GetHeadPosition();
   while (pos)
   {
      DataStruct *data = DataList->GetNext(pos);
      cnt += sizeof(DataStruct);
      cnt += mem_count_attrib(data->getAttributesRef(), memcheck );

      memcheck->data_cnt++;
      memcheck->data_mem += sizeof(DataStruct);

      switch(data->getDataType())
      {
         case T_POLY:
         {
            cnt += sizeof(CPolyList);
            memcheck->data_mem += sizeof(CPolyList);

            // count thru polys
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);

               cnt += sizeof(CPoly);
               memcheck->data_mem += sizeof(CPoly);

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);

                  cnt += sizeof(CPnt);
                  memcheck->data_mem += sizeof(CPnt);
               }
            }  
         }
         break;
         case T_TEXT:
            cnt += sizeof(TextStruct);
            memcheck->data_mem += sizeof(TextStruct);
         break;
         case T_INSERT:
            cnt += sizeof(InsertStruct);
            memcheck->data_mem += sizeof(InsertStruct);
         break;
      }
   }

   return cnt;
}

/*******************************************************************************
* 
*/
static int mem_count_netlist(FileStruct *file, MemCheck *memcheck)
{
   int   c = 0;

   //netPos = file->getNetList().Dump
   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct *net = file->getNetList().GetNext(netPos);

      c += sizeof(NetStruct);
      c += mem_count_attrib(net->getAttributesRef(), memcheck);

      // SaveAttribs(stream, &net->getAttributesRef());
      POSITION compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         CompPinStruct *compPin = net->getNextCompPin(compPinPos);

         c += sizeof(CompPinStruct);
         c += mem_count_attrib(compPin->getAttributesRef(), memcheck);
      }
   }

   return c;
}

/*******************************************************************************
* 
*/
void data_mem_calc(CCEtoODBDoc *doc)
{
   int byte;

   byte = sizeof(DataStruct);          // 28
   byte = sizeof(BlockStruct);         // 136
   byte = sizeof(FileStruct);          // 148
   byte = sizeof(Attrib);              // 72
   byte = sizeof(CompPinStruct);       // 32
   byte = sizeof(CPolyList);           // 28
   byte = sizeof(CPntList);            // 28
   byte = sizeof(CPnt);                // 12
   byte = sizeof(TextStruct);          // 32
   byte = sizeof(InsertStruct);        // 36
   byte = sizeof(PointStruct);         // 8
   byte = sizeof(BlobStruct);          // 32
   byte = sizeof(DRCStruct);           // 44
   byte = sizeof(DRC_MeasureStruct);   // 32

   MemCheck  memcheck;

   memcheck.attribute_cnt = 0;   
   memcheck.attribute_mem = 0;   
   memcheck.data_cnt = 0;     
   memcheck.data_mem = 0;     
   memcheck.block_cnt = 0;    
   memcheck.block_mem = 0;    
   memcheck.net_cnt = 0;         
   memcheck.net_mem = 0;         
   memcheck.comppin_cnt = 0;  
   memcheck.comppin_mem = 0;  
   memcheck.type_cnt = 0;     
   memcheck.type_mem = 0;     

   int mc = 0;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      memcheck.block_cnt++;
      memcheck.block_mem += sizeof(BlockStruct);

      mc += mem_count(doc, &(block->getDataList()), &memcheck);
      mc += mem_count_attrib(block->getAttributesRef(), &memcheck );
   }

   // here loop through Files and 
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);

      // netlist
      mc += mem_count_netlist(file, &memcheck);

      // typelist
      // drclist
   }

   return;
}

/*****************************************************************************/
/*
*/ // Check if we require all of these
int scale_default_units(SettingsStruct *Settings, double unitfactor)
{
   // here update settings variables with new scale, so that redline etc... all changes.
   Settings->setSmallWidth(Settings->getSmallWidth() * unitfactor);
   Settings->Current_TextSize *= unitfactor;
   Settings->RL_arrowSize *= unitfactor;
   Settings->RL_textHeight *= unitfactor;
   Settings->RL_WS *= unitfactor;
   Settings->RL_WM *= unitfactor;
   Settings->RL_WL *= unitfactor;

   Settings->GridOffX *= unitfactor;
   Settings->GridOffY *= unitfactor;
   Settings->GridSpaceX *= unitfactor;
   Settings->GridSpaceY *= unitfactor;
   Settings->SnapOffX *= unitfactor;
   Settings->SnapOffY *= unitfactor;
   Settings->SnapSpaceX *= unitfactor;
   Settings->SnapSpaceY *= unitfactor;
   Settings->JOIN_chamfer *= unitfactor;
   Settings->JOIN_radius *= unitfactor;
   Settings->DRC_MAX_ARROWSIZE *= unitfactor;
   Settings->DRC_MIN_ARROWSIZE *= unitfactor;

   return 1;            
}

/*****************************************************************************/
/* GetMaxStackupNum 
*/
int GetMaxStackupNum(CCEtoODBDoc *doc)
{
   int maxStackupNum = 0;
   for (int i=0; i<doc->getMaxLayerIndex(); i++)
   {
      LayerStruct *layer = doc->getLayerArray()[i];
      if (layer == NULL)   continue;
      if (layer->getElectricalStackNumber() > maxStackupNum)
         maxStackupNum = layer->getElectricalStackNumber();
   }

   POSITION groupPos = doc->LayerGroupList.GetHeadPosition();
   while (groupPos != NULL)
   {
      LayerGroupStruct *group = doc->LayerGroupList.GetNext(groupPos);
      if (group->electricalstacknumber > maxStackupNum)
         maxStackupNum = group->electricalstacknumber;
   }
   return maxStackupNum;
}

/******************************************************************************
* GetDataLink
*/
int GetDataLink(CCamCadData &camCadData, DataStruct *data)
{
   if(!data) return DCA_NO_ENTITY_NUMBER;
   Attrib *attrib = NULL;

   int linkKW = camCadData.getAttributeKeywordIndex(standardAttributeDataLink);
   if ((attrib = get_attvalue(data->getAttributesRef(), linkKW)))
   {   
      return attrib->getIntValue();
   }

   return DCA_NO_ENTITY_NUMBER;
}

/******************************************************************************
* IsProbedDiePin
*/
bool IsProbedDiePin(CCamCadData &camCadData, int probedatalink)
{
   if(probedatalink == DCA_NO_ENTITY_NUMBER) return false;

   //Get access markers
   CEntity accessEntity = CEntity::findEntity(camCadData, probedatalink);
   if (accessEntity.getEntityType() == entityTypeData)
   {
      DataStruct *testAccessPointData = accessEntity.getData();
      if(testAccessPointData)
      {
         CEntity probedEntity = CEntity::findEntity(camCadData, GetDataLink(camCadData,testAccessPointData));
         //Get comppins
         if (probedEntity.getEntityType() == entityTypeCompPin)
         {
            //Fine Die Pins
            if(probedEntity.getCompPin())
               return probedEntity.getCompPin()->IsDiePin(camCadData);
         
         }//if probedEntity
      }
   }//if accessEntity
 
   return false;
}

/******************************************************************************
* IsProbedDiePin
*/
bool IsProbedDiePin(CCamCadData &camCadData, DataStruct *data)
{
   int datalink = GetDataLink(camCadData, data);
   return IsProbedDiePin(camCadData, datalink);
}

/******************************************************************************
* GetBondPadPadstackBlock
*/
BlockStruct *GetBondPadPadstackBlock(CCamCadData &camCadData, BlockStruct *block)
{
   if(!block || block->getBlockType() != blockTypeBondPad) return NULL; 
   
   return block->GetFirstPinInsertPadstackGeometry(camCadData);
}

/////////////////////////////////////////////////////////////////////////////
// PCB_HighlightNet dialog
PCB_HighlightNet::PCB_HighlightNet(CWnd* pParent /*=NULL*/)
   : CDialog(PCB_HighlightNet::IDD, pParent)
{
   //{{AFX_DATA_INIT(PCB_HighlightNet)
   m_color = FALSE;
   //}}AFX_DATA_INIT
}

void PCB_HighlightNet::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PCB_HighlightNet)
   DDX_Control(pDX, IDC_NETNAME, m_nameCB);
   DDX_Check(pDX, IDC_COLOR, m_color);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PCB_HighlightNet, CDialog)
   //{{AFX_MSG_MAP(PCB_HighlightNet)
   ON_BN_CLICKED(ID_APPLY, OnApply)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PCB_HighlightNet message handlers
BOOL PCB_HighlightNet::OnInitDialog() 
{
   CDialog::OnInitDialog();

   int kw = doc->IsKeyWord(ATT_NETNAME, TRUE); // remember index to "NetName" in KeyWordArray 

   int valueDictionarySize = doc->getCamCadData().getAttributeValueDictionary().getSize();

   // array of 1 or 0.  Used to remember all value Strings used as netnames
   CByteArray arr;
   arr.SetSize(valueDictionarySize + 1);

   // init to 0
	int i = 0;
   for (i = 0; i <= valueDictionarySize; i++)
      arr[i] = 0;
   
   Attrib *attrib;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos != NULL)
      {
         DataStruct *d = block->getDataList().GetNext(dataPos);

         if (d->getAttributesRef() && d->getAttributesRef()->Lookup(kw, attrib)) // if this entity has a "NetName"
         {
            if (attrib->getValueType() == VT_STRING)
               arr[attrib->getStringValueIndex()] = 1; // mark this value as used as a "NetName"
         }
      }
   }

   for (i=0; i <= valueDictionarySize; i++)
   {
      if (arr[i])
         m_nameCB.SetItemData(m_nameCB.AddString(doc->getCamCadData().getAttributeValueDictionary().getAt(i)), i); 
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void PCB_HighlightNet::OnOK() 
{
   int sel = m_nameCB.GetCurSel();
   if (sel == CB_ERR)
   {
      CDialog::OnCancel();
      return;
   }
   
   index = m_nameCB.GetItemData(sel);

   CDialog::OnOK();
}

void PCB_HighlightNet::OnApply() 
{
   UpdateData();

   int sel = m_nameCB.GetCurSel();
   if (sel == CB_ERR)
      return;

   index = m_nameCB.GetItemData(sel);

   doc->HighlightByAttrib(FALSE, doc->IsKeyWord(ATT_NETNAME, TRUE), VT_STRING, index);
}

//--------------------------------------------------------------
void CCEtoODBDoc::OnDeleteTraces() 
{
   DeleteTraces dlg;
   dlg.m_traces = 0; // 0 none 1 all 2 only with netnames
   dlg.m_vias = 0;   // 0 none 1 all 2 only with netnames
   if (dlg.DoModal() == IDOK)
   {
      if (dlg.m_traces || dlg.m_vias)
      {
         if (ErrorMessage("Warning - This function can't be undone, and the undo buffer will be cleared. Prior steps will be cleared and can't be undone.\nDo you wish to continue?", "Delete Traces", MB_YESNO | MB_DEFBUTTON2)==IDYES)
         {
            BeginWaitCursor();
            delete_traces(this, dlg.m_traces, dlg.m_vias);
            EndWaitCursor();
            UnselectAll(FALSE);
            UpdateAllViews(NULL);
         }
      }
   }
   return;  
}

/////////////////////////////////////////////////////////////////////////////
// DeleteTraces dialog
DeleteTraces::DeleteTraces(CWnd* pParent /*=NULL*/)
   : CDialog(DeleteTraces::IDD, pParent)
{
   //{{AFX_DATA_INIT(DeleteTraces)
   m_traces = -1;
   m_vias = -1;
   //}}AFX_DATA_INIT
}

void DeleteTraces::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DeleteTraces)
   DDX_Radio(pDX, IDC_TRACES, m_traces);
   DDX_Radio(pDX, IDC_VIAS, m_vias);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DeleteTraces, CDialog)
   //{{AFX_MSG_MAP(DeleteTraces)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*************************************************************************
* Delete DOC Graphic
*/
void CCEtoODBDoc::OnDeleteDocGraphic() 
{
   GenericDelete dlg;
   dlg.m_component = TRUE; // init checkbox
   dlg.m_graphic = TRUE;   // init checkbox
   if (dlg.DoModal() != IDOK) return;

   //  = dlg.m_component;
   //  = dlg.m_graphic;
   
   if (dlg.m_component || dlg.m_graphic)
   {
      if (ErrorMessage("Warning - This function can't be undone, and the undo buffer will be cleared. Prior steps will be cleared and can't be undone.\nDo you wish to continue?", "Delete Documentation Graphic", MB_YESNO | MB_DEFBUTTON2)==IDYES)
      {
         BeginWaitCursor();
         delete_visible_root_graphic(this,dlg.m_component != 0,dlg.m_graphic != 0);
         EndWaitCursor();
         UnselectAll(FALSE);
         UpdateAllViews(NULL);
      }
   }
   return;
}

/////////////////////////////////////////////////////////////////////////////
// GenericDelete dialog
GenericDelete::GenericDelete(CWnd* pParent /*=NULL*/)
   : CDialog(GenericDelete::IDD, pParent)
{
   //{{AFX_DATA_INIT(GenericDelete)
   m_component = FALSE;
   m_graphic = FALSE;
   //}}AFX_DATA_INIT
}

void GenericDelete::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GenericDelete)
   DDX_Check(pDX, IDC_COMPONENT, m_component);
   DDX_Check(pDX, IDC_GRAPHIC, m_graphic);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GenericDelete, CDialog)
   //{{AFX_MSG_MAP(GenericDelete)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GeneratePadstackAccess dialog
GeneratePadstackAccess::GeneratePadstackAccess(CWnd* pParent /*=NULL*/)
   : CDialog(GeneratePadstackAccess::IDD, pParent)
{
   //{{AFX_DATA_INIT(GeneratePadstackAccess)
   m_smd = FALSE;
   //}}AFX_DATA_INIT
}

void GeneratePadstackAccess::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GeneratePadstackAccess)
   DDX_Check(pDX, IDC_SMD, m_smd);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GeneratePadstackAccess, CDialog)
   //{{AFX_MSG_MAP(GeneratePadstackAccess)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// GeneratePin1Marker dialog
GeneratePin1Marker::GeneratePin1Marker(CWnd* pParent /*=NULL*/)
   : CDialog(GeneratePin1Marker::IDD, pParent)
{
   //{{AFX_DATA_INIT(GeneratePin1Marker)
//   m_square = 0;
   m_circle = 0;
   m_size   = "0.01"; 
   //}}AFX_DATA_INIT
}

void GeneratePin1Marker::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GeneratePin1Marker)
//   DDX_Radio(pDX, IDC_GEN_SQR, m_square);
   DDX_Radio(pDX, IDC_GEN_CIR, m_circle);
    DDX_Text(pDX, IDC_EDIT_SIZE, m_size);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GeneratePin1Marker, CDialog)
   //{{AFX_MSG_MAP(GeneratePin1Marker)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GeneratePin1Marker message handlers
BOOL GeneratePin1Marker::OnInitDialog() 
{
   CDialog::OnInitDialog();
   UpdateData(FALSE);
   return TRUE;
}

/****************************************************************************
* GeneratePin1Marker::OnOK()
*/
void GeneratePin1Marker::OnOK()
{
   UpdateData();
   generate_pin_markers(doc, m_circle, atof(m_size));
   doc->UpdateAllViews(NULL);
   CDialog::OnOK();

}
