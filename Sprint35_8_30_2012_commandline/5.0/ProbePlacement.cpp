
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

#include "stdafx.h"
#include "net_util.h"
#include "panref.h"
#include "xform.h"
#include "graph.h"
#include "float.h"
#include "mainfrm.h"
#include "gauge.h"
#include "lic.h"
#include "crypt.h"
#include "CCEtoODB.h"
#include "drc_util.h"
#include "MultipleMachine.h"
#include "ProbePlacement.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// custom message to send to the Probe Placement Dialog
#define IPP_UPDATEDIALOG                  WM_USER + 25
#define IPP_SETMODIFIED                   WM_USER + 26
#define IPP_UPDATESURFACE                 WM_USER + 27
#define IPP_SETPAGE                       WM_USER + 28
#define IPP_UPDATEPAGES                   WM_USER + 29

BOOL GetLicenseString(CLSID clsid, BSTR &licStr);

void CreateTestProbeRefnameAttr(CCEtoODBDoc *doc, DataStruct *probe, CString refname, double featuresize, bool never_mirror)
{
   CreateTestProbeRefnameAttr(doc, probe, refname, featuresize, 0.0 /*textsize*/, never_mirror);
}

void CreateTestProbeRefnameAttr(CCEtoODBDoc *doc, DataStruct *probe, CString refname, double featuresize, double textsize, bool never_mirror)
{
	// Feature Size, once upon a time it was probe Diameter.
	// Then it changed to probe Drill Size.

	// Since the drill size tends to be smaller than the probe diameter, and since
	// Mark requested the refname text stay about the same as it was, the "ratio"
	// of height to "size" has been increased from .5 to .7.

	// The featuresize param is a feature size that we want to make a refname relative to, it is not
	// the size of the refname itself.

   // If textsize is greater than zero, then use it directly for height of refname.
   // Set width proportional to height. If it is not greater than zero, then use the
   // old method for height as default.
   double actualTextsize = textsize;
   if (actualTextsize <= 0.0)
      actualTextsize = featuresize * 0.70;

	if (doc != NULL && probe != NULL)
	{
		WORD refnameKW = doc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);

		// 0.36 yeilds a hypotenuese that is just over one radius of the circle, hence
		// the text does not overlap the graphic. Old value of .25 had a lot of overlap,
		// case 1825 complained about that (in a round-about manner, by complaining
		// about clutter).

		doc->SetVisAttrib(&probe->getAttributeMap(), refnameKW, valueTypeString, refname.GetBuffer(0), 
			featuresize * 0.36, featuresize * 0.36, 0.0, // x, y, rotation
			actualTextsize, actualTextsize * 0.50, // height, width
			TRUE, 0, TRUE, attributeUpdateOverwrite, 0L, Graph_Level("PROBE_TOP", "", FALSE), 
			never_mirror ? TRUE : FALSE, 
			horizontalPositionLeft, verticalPositionBaseline);

      // another way to do it
		// doc->SetUnknownVisAttrib(&data->getAttributesRef(), ATT_REFNAME, pname,
      //            drillSize*0.25, drillSize*0.25, 0.0, drillSize*0.5, drillSize*0.25, 1, 0,
      //            TRUE, attributeUpdateOverwrite, 0L, Graph_Level("PROBE_TOP", "", FALSE), nevermirror, horizontalPositionLeft, verticalPositionBaseline); 

	}
}

BlockStruct* CreateTestProbeGeometry(CCEtoODBDoc *doc, const char *name, double probediam, const char *tool, double drillsize, EProbeShape shape, COLORREF color)
{
	// Case 1825
	// Previously the graphic size was based on the probediam. Mark has asked that to change
	// to being based on drillsize.
	double basedim = drillsize;

   doc->StoreDocForImporting();

   BlockStruct *block = Graph_Block_Exists(doc, name, -1);
   if (!block)
   {
      int layerNum = Graph_Level("PROBE_TOP", "", 0);

      Graph_Level_Mirror("PROBE_TOP", "PROBE_BOT","");
      Graph_Level_Mirror("PROBEDRILL_TOP", "PROBEDRILL_BOT","");
      
      LayerStruct *layer;
      if ((layer = doc->FindLayer_by_Name("PROBE_BOT")) != NULL)
         layer->setLayerType(layerTypeDftBottom);
      if ((layer = doc->FindLayer_by_Name("PROBE_TOP")) != NULL)
         layer->setLayerType(layerTypeDftTop);
      if ((layer = doc->FindLayer_by_Name("PROBEDRILL_BOT")) != NULL)
         layer->setLayerType(layerTypeDftBottom);
      if ((layer = doc->FindLayer_by_Name("PROBEDRILL_TOP")) != NULL)
         layer->setLayerType(layerTypeDftTop);


      block = Graph_Block_On(GBO_APPEND, name, -1, 0);
      block->setBlockType(blockTypeTestProbe);

      CString toolname;
      toolname.Format("PROBE_%s", tool);
      Graph_Tool(toolname, 0, drillsize, 0, 0, 0, 0L);
      Graph_Block_Reference(toolname, NULL, 0, 0.0, 0.0, 0.0, 0, 1.0, Graph_Level("PROBEDRILL_TOP","",0), TRUE);


      CPoint2d center(0.0, 0.0);
      CExtent centerSquare(-(basedim/2.0), -(basedim/2.0), (basedim/2.0), (basedim/2.0));
      switch (shape)
      {
      case probeShapeDefaultTarget:
         {
            CString padname;
            padname.Format("%s Symbol", name);

            int err;
            // a target is drawn with a cross of the size and a circle diam of 1/2 of the size.
            // Enable OverwriteValues, otherwise changes in probe template GUI have no effect on result.		
				Graph_Aperture(padname, T_TARGET, basedim, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
            Graph_Block_Reference(padname, NULL, 0, 0, 0, 0.0, 0, 1.0, layerNum, TRUE);
         }
         break;
      case probeShapeRoundCross:
         {
            DataStruct *circle = Graph_Circle(layerNum, 0.0, 0.0, basedim/2.0, 0, 0, FALSE, FALSE);

            // +
            Graph_Poly(circle, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmax(), 0.0);

            Graph_Poly(circle, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
         }
         break;
      case probeShapeRoundX:
         {
            DataStruct *circle = Graph_Circle(layerNum, 0.0, 0.0, basedim/2.0, 0, 0, FALSE, FALSE);

            double halfSide = (centerSquare.getXsize()/2.0) * sin(DegToRad(45.0));
            CExtent squareInCircle(center.x - halfSide, center.y - halfSide, center.x + halfSide, center.y + halfSide);

            // X
            Graph_Poly(circle, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(squareInCircle.getXmax(), squareInCircle.getYmin(), 0.0);
            Graph_Vertex(squareInCircle.getXmin(), squareInCircle.getYmax(), 0.0);

            Graph_Poly(circle, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(squareInCircle.getXmin(), squareInCircle.getYmin(), 0.0);
            Graph_Vertex(squareInCircle.getXmax(), squareInCircle.getYmax(), 0.0);
         }
         break;
      case probeShapeSquareCross:
         {
            DataStruct *square = Graph_PolyStruct(layerNum, 0, FALSE);
            Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmin(), 0.0);

            // +
            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmax(), 0.0);

            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
         }
         break;
      case probeShapeSquareX:
         {
            DataStruct *square = Graph_PolyStruct(layerNum, 0, FALSE);
            Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmin(), 0.0);

            // X           
            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmax(), 0.0);

            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmin(), 0.0);
         }
         break;
      case probeShapeDiamondCross:
         {
            DataStruct *square = Graph_PolyStruct(layerNum, 0, FALSE);
            Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);

            // +
            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmax(), 0.0);

            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
         }
         break;
      case probeShapeDiamondX:
         {
            DataStruct *square = Graph_PolyStruct(layerNum, 0, FALSE);
            Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);

            // X
            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmax()/2, centerSquare.getYmin()/2, 0.0);
            Graph_Vertex(centerSquare.getXmin()/2, centerSquare.getYmax()/2, 0.0);

            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmin()/2, centerSquare.getYmin()/2, 0.0);
            Graph_Vertex(centerSquare.getXmax()/2, centerSquare.getYmax()/2, 0.0);
         }
         break;
      case probeShapeHexagonCross:
         {
            DataStruct *hexagon = Graph_PolyStruct(layerNum, 0, FALSE);
            double hexWidth = (centerSquare.getXsize() / 2.0) * (3.0 / 5.0);
            //         hexWidth
            //          |   |
            //          ----|-----
            //         /          \
            //        /            \
            //       /              \
            //       \              /
            //        \            /
            //         \          /
            //          ----------
            //

            Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x - hexWidth, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x - hexWidth, centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmin(), 0.0);

            // +
            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x, centerSquare.getYmax(), 0.0);

            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
            break;
         }
         break;
      case probeShapeHexagonX:
         {
            DataStruct *hexagon = Graph_PolyStruct(layerNum, 0, FALSE);
            double hexWidth = (centerSquare.getXsize() / 2.0) * (3.0 / 5.0);
            //         hexWidth
            //          |   |
            //          ----|-----
            //         /          \
            //        /            \
            //       /              \
            //       \              /
            //        \            /
            //         \          /
            //          ----------
            //
            Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x - hexWidth, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x - hexWidth, centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getCenter().y, 0.0);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmin(), 0.0);

            // X
            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getCenter().x - hexWidth, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmax(), 0.0);

            Graph_Poly(NULL, 0, FALSE, FALSE, FALSE);
            Graph_Vertex(centerSquare.getCenter().x + hexWidth, centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getCenter().x - hexWidth, centerSquare.getYmax(), 0.0);
         }
         break;
      case probeShapeHourglass:
         {
            DataStruct *hourglass = Graph_PolyStruct(layerNum, 0, FALSE);
            Graph_Poly(NULL, 0, FALSE, FALSE, TRUE);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmax(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmin(), centerSquare.getYmin(), 0.0);
            Graph_Vertex(centerSquare.getXmax(), centerSquare.getYmax(), 0.0);

         }
         break;
      }

      Graph_Block_Off();
   }

   return block;
}

static bool BlockHasInsert(FileStruct *pcbFile, BlockStruct *curiousBlock, bool top, bool bottom)
{
	if (pcbFile != NULL && curiousBlock != NULL)
	{
		int curiousBlockNum = curiousBlock->getBlockNumber();

		POSITION dataPos = pcbFile->getBlock()->getHeadDataInsertPosition();
		while (dataPos)
		{
			DataStruct *data = pcbFile->getBlock()->getNextDataInsert(dataPos);

         if ((top && data->getInsert()->getPlacedTop()) || (bottom && data->getInsert()->getPlacedBottom()))
         {
            if (data->getInsert() && data->getInsert()->getBlockNumber() == curiousBlockNum)
				   return true;
         }
		}
	}

	return false;
}

static bool BlockHasInsert(CCEtoODBDoc *doc, FileStruct *exceptFile, BlockStruct *curiousBlock)
{
   // We're interested in files other than exceptFile

   if (doc != NULL)
   {
      POSITION pos = doc->getFileList().GetHeadPosition();
      while (pos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(pos);
         
         if (file != exceptFile && BlockHasInsert(file, curiousBlock, true, true))
            return true;
      }
   }

   return false;
}

void DeleteTestProbes(CCEtoODBDoc *doc, FileStruct *pcbFile, bool top, bool bottom)
{
   if (doc != NULL && pcbFile != NULL)
   {
      // Remove any probe inserts that currently exist
      POSITION dataPos = pcbFile->getBlock()->getHeadDataInsertPosition();
      while (dataPos)
      {
         DataStruct *data = pcbFile->getBlock()->getNextDataInsert(dataPos);

         if (data->getInsert()->getInsertType() == insertTypeTestProbe &&
            ((top && data->getInsert()->getPlacedTop()) || (bottom && data->getInsert()->getPlacedBottom())))
         {
            //pcbFile->getBlock()->RemoveDataFromList(doc, data);
            doc->removeDataFromDataList(*(pcbFile->getBlock()),data);
         }
      }


      POSITION drcPos = pcbFile->getDRCList().GetHeadPosition();
      while (drcPos != NULL)
      {
         DRCStruct *drc = pcbFile->getDRCList().GetNext(drcPos);

         if (drc->getAlgorithmType() == DFT_ALG_NET_WITHOUT_PROBE)
            RemoveOneDRC(doc, drc, pcbFile);
      }


      // We have already removed all probe inserts. If there are still inserts of any
      // probe geometry blocks in this file, they are not test probes. 
      // In order to retain their presence
      // on the board, change the block type and block name, so it can still
      // exist and not interfere with making new probe geometries during probe placement.
      // If the block is inserted in some other file in the doc, leave it be.
      // If not in use, then remove the block.

      CString offendingProbes;

      for (int i = 0; i < doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (block != NULL && block->getBlockType() == blockTypeTestProbe)
         {
            if (BlockHasInsert(pcbFile, block, top, bottom))
            {
               // Block has insert in active file on side where probes were deleted, insert type is not Probe
               // Change name and type
               CString newname;
               newname.Format("Chameleon_%d", i);
               int j = i;
               while (doc->Find_Block_by_Name(newname, -1) != NULL)
                  newname.Format("Chameleon_%d", ++j);
               block->setName(newname);
               block->setBlockType(blockTypeUnknown);
            }
            else if (BlockHasInsert(doc, pcbFile, block))
            {
               // Block has insert in some file (other than active file) so can't
               // nuke block. Leave it be.
               if (!offendingProbes.IsEmpty())
                  offendingProbes += ", ";
               offendingProbes += block->getName();
            }
            else
            {
               // Nuke it
               doc->RemoveBlock(block);
            }
         }
      }

      if (!offendingProbes.IsEmpty())
      {
         CString msg;
         msg += "Probe Placement has detected another design in the CC document with placed probes.";
         msg += " Because probes are shared geometries, changes to sizes of these probes (e.g. drill size) in this probe";
         msg += " placement run will be ignored. Changing the probe sizes may invalidate the probe";
         msg += " placement result in the other design. The following probes are involved:\n";
         msg += offendingProbes;

         ErrorMessage(msg, "", MB_ICONINFORMATION);
      }

      CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
      frame->getNavigator().setDoc(doc);
   }

}

//******************************************************************************

bool CProbeNamer::IsStricken(CString name)
{
   for (int i = 0; i < m_offlimits.GetCount(); i++)
   {
      if (m_offlimits.GetAt(i).Compare(name) == 0)
         return true;
   }

   return false;
}

CString CProbeNamer::GetNextProbeRefname(CPPNet *ppNet, ETestResourceType resourceType)
{
   // If resource type is Power Injection then we adjust the refnames according to whether
   // the are power or ground. If not Power Injection then leave probe name as numeric, power
   // or grund does not matter, is no different than signal.

   bool power = false;
   bool ground = false;
   
   if (ppNet != NULL)
   {
      if (ppNet->IsPowerNet())
         power = true;
      if (ppNet->IsGroundNet())
         ground = true;
   }

   // Could test one less time for resourceType by putting it in power/ground
   // determination above. But it is more maintenance proof to be explicit for
   // each situation below.

   CString refname;
   if (power && resourceType == testResourceTypePowerInjection)
      refname.Format("P%d", m_powerProbeNum++);
   else if (ground && resourceType == testResourceTypePowerInjection)
      refname.Format("G%d", m_groundProbeNum++);
   else
      refname.Format("%d", m_normalProbeNum++);    

   return IsStricken(refname) ? GetNextProbeRefname(ppNet, resourceType) : refname;
}

CProbeNamer& CProbeNamer::operator=(const CProbeNamer &otherNamer)
{
   if (&otherNamer != this)
   {
      this->m_normalProbeNum = otherNamer.m_normalProbeNum;
      this->m_powerProbeNum  = otherNamer.m_powerProbeNum;
      this->m_groundProbeNum = otherNamer.m_groundProbeNum;

      // bug ! Not copying offlimits list
   }

   return *this;
}

/******************************************************************************
* CCEtoODBDoc
*/
void CCEtoODBDoc::OnDftanalysisProbeplacement()
{
   /*if (!getApp().getCamcadLicense().isLicensed(camcadProductDft)) 
   {
      ErrorAccess("Probe Placement!");
      return;
   }*/

   FileStruct *file = getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
   {
      ErrorMessage("Only one PCB file can be worked on at a time.\n\nThere is either zero or more than one PCB file visible.", "Probe Placement", MB_OK);
      return;
   }

   /*if (GetCurrentMachine(*file) != NULL )
   {
      // check to see if the select machine support DFT solution
      if (GetCurrentMachine(*file)->GetDFTSolution() == NULL)
      {
         ErrorMessage("The selected Machine " + GetCurrentMachine(*file)->GetName() + " does not use DFT Solution.\n", "Access Analysis");
         return;
      }
      else if (GetCurrentMachine(*file)->GetDFTSolution() != GetCurrentDFTSolution(*file))
      {
         // The current DFT Solution is not the same as the DFT Solution of the current machine so fix it
         SetCurrentDFTSolution(*file, GetCurrentMachine(*file)->GetDFTSolution());
      }
   }*/

   // check for DFT Solutions (must exist)
   /*if (GetCurrentDFTSolution(*file) == NULL)
   {
      ErrorMessage("There is no current DFT Solution.  Please run Access Analysis first.", "Probe Placement", MB_OK);
      return;
   }*/

   CProbePlacementDlg dlg(this, AfxGetMainWnd());
   dlg.DoModal();
   UpdateAllViews(NULL);
}



/******************************************************************************
* CDFTSolution
*/
void CDFTSolution::dumpPPToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Probe Placement Solution::\n", indent, " ");
   if (m_ppSolution != NULL)
   {
      indent += 3;
      m_ppSolution->DumpToFile(file, indent);
      indent -= 3;
   }
}

CProbePlacementSolution *CDFTSolution::CreateProbePlacementSolution(CCEtoODBDoc *doc)
{
   if (m_ppSolution != NULL)
      return m_ppSolution;

   m_ppSolution = new CProbePlacementSolution(doc, &m_TestPlan);

   return m_ppSolution;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Probe Placement Solution
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
* CPPAccessibleLocation
*/
CPPAccessibleLocation::CPPAccessibleLocation(CCEtoODBDoc *doc, DataStruct *testAccessData)
: m_pDoc(NULL)
//, m_eFeatureType(entityTypeUndefined)
//, m_pFeature(NULL)
, m_pTA(NULL)
, m_ptLocation(0, 0)
, m_eSurface(testSurfaceUnset)
, m_eFeatureCondition(featureConditionProbeable)
, m_pProbe(NULL)
, m_pDoubleWiredProbe(NULL)
, m_distToClosestPart(0.0)
{
   m_pDoc = doc;
   m_pTA = testAccessData;
   //m_bPlaced = true;

   InsertStruct *taInsert = testAccessData->getInsert();

   // get the location of the access point
   m_ptLocation = taInsert->getOrigin2d();

   // get the feature and feature type
   Attrib *dataLinkAttrib = NULL;
   long featureEntityNumber = 0;
   WORD dataLinkKw = m_pDoc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
   if (testAccessData->getAttributes() && testAccessData->getAttributes()->Lookup(dataLinkKw, dataLinkAttrib))
   {
      //EEntityType entityType;

      featureEntityNumber = dataLinkAttrib->getIntValue();
      m_featureEntity = CEntity::findEntity(m_pDoc->getCamCadData(), featureEntityNumber);
      //m_pFeature = FindEntity(m_pDoc, featureEntityNumber, entityType);
      //m_eFeatureType = entityType;
   }

   // get the target type attribute
   Attrib *targetAttrib = NULL;
   WORD targetTypeKw = m_pDoc->RegisterKeyWord(ATT_DFT_TARGETTYPE, 0, valueTypeString);
   if (testAccessData->getAttributes() && testAccessData->getAttributes()->Lookup(targetTypeKw, targetAttrib))
      m_sTargetType = get_attvalue_string(m_pDoc, targetAttrib);

   // get the target type attribute
   Attrib *netNameAttrib = NULL;
   WORD netNameKw = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
   if (testAccessData->getAttributes() && testAccessData->getAttributes()->Lookup(netNameKw, netNameAttrib))
      m_sNetName = get_attvalue_string(m_pDoc, netNameAttrib);

   // get the surface of the feature
   bool onTop = false;
   if (!taInsert->getLayerMirrored())
      m_eSurface = testSurfaceTop;
   else
      m_eSurface = testSurfaceBottom;
}

CPPAccessibleLocation::CPPAccessibleLocation(CCEtoODBDoc& camCadDoc,const CEntity& featureEntity, CPoint2d location, CString targetType, CString netName, ETestSurface surface)
{
   m_pDoc = &camCadDoc;
   //m_eFeatureType = featureType;
   //m_pFeature = feature;
   m_featureEntity = featureEntity;
   m_ptLocation = location;
   m_sTargetType = targetType;
   m_sNetName = netName;
   m_eSurface = surface;
   m_pTA = NULL;
   m_distToClosestPart = 0.0;

   m_eFeatureCondition = featureConditionProbeable;
   m_pProbe = NULL;
   m_pDoubleWiredProbe = NULL;

   //m_bPlaced = false;
}

CPPAccessibleLocation::CPPAccessibleLocation(CPPAccessibleLocation &other)
{
   *this = other;
}

CPPAccessibleLocation::~CPPAccessibleLocation()
{
}

CPPAccessibleLocation& CPPAccessibleLocation::operator=(const CPPAccessibleLocation &other)
{
   if (&other != this)
   {
      m_pDoc              = other.m_pDoc;
    //m_eFeatureType      = other.m_eFeatureType;
    //m_pFeature          = other.m_pFeature;
      m_featureEntity     = other.m_featureEntity;
    //m_bPlaced           = other.m_bPlaced;
      m_pTA               = other.m_pTA;
    //m_FeatureExtents    = other.m_FeatureExtents;
      m_ptLocation        = other.m_ptLocation;
      m_sTargetType       = other.m_sTargetType;
      m_sNetName          = other.m_sNetName;
      m_eSurface          = other.m_eSurface;
      m_distToClosestPart = other.m_distToClosestPart;

      m_eFeatureCondition = other.m_eFeatureCondition;
      m_pProbe            = other.m_pProbe;
      m_pDoubleWiredProbe = other.m_pDoubleWiredProbe;
   }

   return *this;
}

void CPPAccessibleLocation::SetFeatureCondition(EFeatureCondition condition)
{
   if (condition < featureConditionMIN && condition >= featureConditionMAX)
      return;

   m_eFeatureCondition = condition;
}

void CPPAccessibleLocation::PlaceProbe(CPPProbe *probe)
{
   m_pProbe = probe;
   probe->SetAccessibleLocationPtr(this);
}

CPPProbe *CPPAccessibleLocation::RemoveProbe()
{
   CPPProbe *probe = m_pProbe;

   // Case 1997, crashed, not always a probe on an accessible location
   if (probe != NULL)
      probe->RemoveAccessibleLocationPtr();

   m_pProbe = NULL;

   return probe;
}

void CPPAccessibleLocation::PlaceDoubleWiredProbe(CPPProbe *probe)
{
   m_pDoubleWiredProbe = probe;
   probe->SetAccessibleLocationPtr(this);
}

CPPProbe *CPPAccessibleLocation::RemoveDoubleWiredProbe()
{
   CPPProbe *probe;
   probe = m_pDoubleWiredProbe;
   probe->RemoveAccessibleLocationPtr();

   m_pDoubleWiredProbe = NULL;

   return probe;
}

void CPPAccessibleLocation::DumpToFile(CFormatStdioFile &file, int indent)
{
   CString accLoc;
   CString featureType("Undefined");

   if (m_featureEntity.getEntityType() == entityTypeCompPin)
   {
      CompPinStruct* cp = m_featureEntity.getCompPin();
      accLoc.Format("CompPin %s [%d] at 0x%08x", cp->getPinRef(), cp->getEntityNumber(), this);
      featureType = "CompPin";
   }
   else if (m_featureEntity.getEntityType() == entityTypeData)
   {
      DataStruct* data = m_featureEntity.getData();
      accLoc.Format("Via %s [%d] at 0x%08x", data->getInsert()->getRefname(), data->getEntityNumber(), this);
      featureType = "Data (Via)";
   }

   file.WriteString("%*s::Accessable Location on %s::\n", indent, " ", accLoc);

   indent += 3;

   CString buf;
   file.WriteString("%*sm_eFeatureType = %s\n", indent, " ",featureType);
   file.WriteString("%*sm_pTA = %s\n", indent, " ", (m_pTA==NULL)?"None":m_pTA->getInsert()->getRefname());
   file.WriteString("%*sm_ptLocation = (%0.3f, %0.3f)\n", indent, " ", m_ptLocation.x, m_ptLocation.y);
   file.WriteString("%*sm_sTargetType = %s\n", indent, " ", m_sTargetType);
   file.WriteString("%*sm_sNetName = %s\n", indent, " ", m_sNetName);
   file.WriteString("%*sm_eSurface = %s\n", indent, " ", (m_eSurface==testSurfaceTop)?"Top":"Bottom");

   switch (m_eFeatureCondition)
   {
   case featureConditionProbeable:
      buf = "Probable";
      break;
   case featureConditionForced:
      buf = "Forced";
      break;
   case featureConditionNoProbe:
      buf = "No Probe";
      break;
   }

   file.WriteString("%*sm_eFeatureCondition = %s\n", indent, " ", buf);
   file.WriteString("%*sm_pProbe = 0x%08x\n", indent, " ", m_pProbe);
   file.WriteString("%*sm_pDoubleWiredProbe = 0x%08x\n", indent, " ", m_pDoubleWiredProbe);

   indent -= 3;
}

/******************************************************************************
* CPPProbe
*/
CPPProbe::CPPProbe(CCEtoODBDoc *doc, ETestResourceType resType, CString netName)
   : m_pDoc(NULL)
   , m_pAccessibleLocation(NULL)
   //, m_lProbeNumber(-1)
   , m_bPlaced(false)
   //, m_pProbe(NULL)
   , m_sProbeTemplateName("")
   , m_bExisting(false)
{
   m_pDoc = doc;
   m_eResourceType = resType;
   m_sNetName = netName;
}

CPPProbe::CPPProbe(CPPProbe &probe)
{
   m_pDoc = probe.m_pDoc;
   m_eResourceType = probe.m_eResourceType;
   m_sNetName = probe.m_sNetName;
   m_pAccessibleLocation = probe.m_pAccessibleLocation;
   m_sProbeRefname = probe.m_sProbeRefname;
   m_bPlaced = probe.m_bPlaced;
   m_sProbeTemplateName = probe.m_sProbeTemplateName;
   //m_pProbe = probe.m_pProbe;
   m_bExisting = probe.m_bExisting;
}

CPPProbe::~CPPProbe()
{
}

CPPProbe& CPPProbe::operator=(const CPPProbe &probe)
{
   if (&probe != this)
   {
      m_pDoc = probe.m_pDoc;
      m_eResourceType = probe.m_eResourceType;
      m_sNetName = probe.m_sNetName;
      m_pAccessibleLocation = probe.m_pAccessibleLocation;
      m_sProbeRefname = probe.m_sProbeRefname;
      m_bPlaced = probe.m_bPlaced;
      m_sProbeTemplateName = probe.m_sProbeTemplateName;
      //m_pProbe = probe.m_pProbe;
      m_bExisting = probe.m_bExisting;
   }

   return *this;
}

void CPPProbe::SetAccessibleLocationPtr(CPPAccessibleLocation *aLocation)
{
   m_pAccessibleLocation = aLocation;
   m_bPlaced = true;
}

CPPAccessibleLocation *CPPProbe::RemoveAccessibleLocationPtr()
{
   CPPAccessibleLocation *accLoc = m_pAccessibleLocation;

   m_bPlaced = false;
   m_pAccessibleLocation = NULL;
   
   return accLoc;
}

void CPPProbe::SetProbeTemplate(CDFTProbeTemplate *probeTemplate)
{
   if (probeTemplate == NULL)
      m_sProbeTemplateName.Empty();
   else
      m_sProbeTemplateName = probeTemplate->GetName();
}

DataStruct *CPPProbe::AddProbeUnplacedToBoard(FileStruct *file, CTestPlan &testPlan)
{
   CDFTProbeTemplate *pTemplate = testPlan.GetProbes().GetTail_TopProbes();

   WORD testResKW = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
   WORD probePlacementKW = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKW = m_pDoc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);

   double diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
   double drillsize = pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
   double textsize = pTemplate->GetTextSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
   BlockStruct *probeBlock = CreateTestProbeGeometry(m_pDoc, pTemplate->GetName(), diameter, pTemplate->GetName(), drillsize);

   Graph_Block_On(file->getBlock());

   int mirror = 0;   // no mirroring

   DataStruct *probe = Graph_Block_Reference(probeBlock->getName(), this->GetProbeRefname(), -1, 0.0, 0.0, 0.0, mirror, 1.0, -1, 0);
   probe->getInsert()->setInsertType(insertTypeTestProbe);

   Graph_Block_Off();

   CString testResourceType;
   switch (m_eResourceType)
   {
   case testResourceTypeTest:
      testResourceType = "Test";
      probe->setAttrib(m_pDoc->getCamCadData(), testResKW, valueTypeString, testResourceType.GetBuffer(0), attributeUpdateOverwrite, NULL);
      probe->setAttrib(m_pDoc->getCamCadData(), probePlacementKW, valueTypeString, "Unplaced", attributeUpdateOverwrite, NULL);
      break;
   case testResourceTypePowerInjection:
      testResourceType = "Power Injection";
      probe->setAttrib(m_pDoc->getCamCadData(), testResKW, valueTypeString, testResourceType.GetBuffer(0), attributeUpdateOverwrite, NULL);
      probe->setAttrib(m_pDoc->getCamCadData(), probePlacementKW, valueTypeString, "Unplaced", attributeUpdateOverwrite, NULL);
      {
         CString probeName;
         POSITION pos = NULL;
         CDFTProbeableNet *pNet = testPlan.GetNetConditions().Find_ProbeableNet(m_sNetName, pos);
         switch (pNet->GetNetType())
         {
         case probeableNetTypePower:
            probeName = (CString)"P" + probe->getInsert()->getRefname();
            probe->getInsert()->setRefname(STRDUP(probeName));
            break;
         case probeableNetTypeGround:
            probeName = (CString)"G" + probe->getInsert()->getRefname();
            probe->getInsert()->setRefname(STRDUP(probeName));
            break;
         }
      }
      break;
   }
   probe->setAttrib(m_pDoc->getCamCadData(), netNameKW, valueTypeString, m_sNetName.GetBuffer(0), attributeUpdateOverwrite, NULL);

   CreateTestProbeRefnameAttr(m_pDoc, probe, probe->getInsert()->getRefname(), drillsize, textsize, testPlan.GetNeverMirrorRefname());

   probe->setHidden(true);

   return probe;
}

void CPPProbe::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Probe on %s at 0x%08x::\n", indent, " ", m_sNetName, this);

   indent += 3;
   file.WriteString("%*sm_eResourceType = %s Resource\n",   indent, " ", (m_eResourceType==testResourceTypeTest)?"Test":"Power Injection");
   file.WriteString("%*sm_sNetName = %s\n",                 indent, " ", m_sNetName);
   file.WriteString("%*sm_pAccessibleLocation = 0x%08x\n",  indent, " ", m_pAccessibleLocation);
   file.WriteString("%*sm_lProbeNumber = %s\n",             indent, " ", m_sProbeRefname);
   file.WriteString("%*sm_bPlaced = %s\n",                  indent, " ", m_bPlaced?"True":"False");
   file.WriteString("%*sm_pProbe = %s\n",                   indent, " ", m_sProbeTemplateName.IsEmpty()?"Null":m_sProbeTemplateName);
   file.WriteString("%*sm_bExisting = %s\n",                indent, " ", m_bExisting?"True":"False");
   indent -= 3;
}

void CPPProbe::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<Probe Name=\"%s\"", this->GetProbeRefname());
   switch (m_eResourceType)
   {
   case testResourceTypeTest:
      writeFormat.writef(" ResType=\"Test\"");
      break;
   case testResourceTypePowerInjection:
      writeFormat.writef(" ResType=\"Power Injection\"");
      break;
   }
   writeFormat.writef(" Placed=\"%s\"", m_bPlaced?"True":"False");

   if (m_bPlaced && !m_sProbeTemplateName.IsEmpty())
   {
      writeFormat.writef(" Diameter=\"%s\"", m_sProbeTemplateName);
      writeFormat.writef(" Type=\"%s\"", "Unknown");
      writeFormat.writef(" TipSize=\"%s\"", "Unknown");
      writeFormat.writef(" AccessName=\"%s\"", SwapSpecialCharacters(m_pAccessibleLocation->GetTAInsert()->getInsert()->getRefname()));
      writeFormat.writef(" Forced=\"%s\"", (m_pAccessibleLocation->GetFeatureCondition() == featureConditionForced)?"True":"False");

      if (m_pAccessibleLocation->GetFeatureCondition() == featureConditionForced)
      {
         writeFormat.writef(" ForcedFeature=\"%s\"", SwapSpecialCharacters(m_pAccessibleLocation->getFeatureEntity().getCompPin()->getPinRef()));
      }
   }

   writeFormat.writef("/>\n");
}



/******************************************************************************
* CPPProbeList
*/
void CPPProbeList::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   POSITION pos = GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = GetNext(pos);

      probe->WriteXML(writeFormat, progress);
   }
}



/******************************************************************************
* CPPNet
*/
CPPNet::CPPNet(CCEtoODBDoc *doc, CString netName)
{
   m_pDoc = doc;
   m_sNetName = netName;
   m_bNoProbe = false;
   m_eNetType = probeableNetTypeSignal;
   m_iTestResourcesNeeded = 0;
   m_iPwrInjResourcesNeeded = 0;
   m_pNet = NULL;
}

CPPNet::CPPNet(CPPNet &ppNet)
{
   m_pDoc = ppNet.m_pDoc;
   m_sNetName = ppNet.m_sNetName;
   m_bNoProbe = ppNet.m_bNoProbe;
   m_iTestResourcesNeeded = ppNet.m_iTestResourcesNeeded;
   m_iPwrInjResourcesNeeded = ppNet.m_iPwrInjResourcesNeeded;
   m_pNet = ppNet.m_pNet;

   POSITION pos = ppNet.m_accessibleLocations.GetHeadPosition();
   while (pos)
   {
      CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(*ppNet.m_accessibleLocations.GetNext(pos));
      m_accessibleLocations.AddTail(accessibleLocation);
   }

   pos = ppNet.m_probes.GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = new CPPProbe(*ppNet.m_probes.GetNext(pos));
      m_probes.AddTail(probe);
   }
}

CPPNet::~CPPNet()
{
   RemoveAll_Probes();
   RemoveAll_AccessibleLocations();
}

CPPNet& CPPNet::operator=(const CPPNet &ppNet)
{
   if (&ppNet == this)
   {
      m_pDoc = ppNet.m_pDoc;
      m_sNetName = ppNet.m_sNetName;
      m_bNoProbe = ppNet.m_bNoProbe;
      m_iTestResourcesNeeded = ppNet.m_iTestResourcesNeeded;
      m_iPwrInjResourcesNeeded = ppNet.m_iPwrInjResourcesNeeded;
      m_pNet = ppNet.m_pNet;

      POSITION pos = ppNet.m_accessibleLocations.GetHeadPosition();
      while (pos)
      {
         CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(*ppNet.m_accessibleLocations.GetNext(pos));
         m_accessibleLocations.AddTail(accessibleLocation);
      }

      pos = ppNet.m_probes.GetHeadPosition();
      while (pos)
      {
         CPPProbe *probe = new CPPProbe(*ppNet.m_probes.GetNext(pos));
         m_probes.AddTail(probe);
      }
   }

   return *this;
}

POSITION CPPNet::GetHeadPosition_AccessibleLocations(EEntityType featureType, CString targetType, CString netName, ETestSurface surface) const
{
   POSITION pos;

   for (pos = m_accessibleLocations.GetHeadPosition();pos != NULL;)
   {
      CPPAccessibleLocation* pAccLoc = m_accessibleLocations.GetAt(pos);

      if ((featureType < entityTypeMIN || pAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pAccLoc->GetSurface() == surface))
         break;

      m_accessibleLocations.GetNext(pos);
   }

   return pos;
}

POSITION CPPNet::GetTailPosition_AccessibleLocations(EEntityType featureType, CString targetType, CString netName, ETestSurface surface) const
{
   POSITION pos;

   for (pos = m_accessibleLocations.GetTailPosition();pos != NULL;)
   {
      CPPAccessibleLocation* pAccLoc = m_accessibleLocations.GetAt(pos);

      if ((featureType < entityTypeMIN || pAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pAccLoc->GetSurface() == surface))
         break;

      m_accessibleLocations.GetPrev(pos);
   }

   return pos;
}

CPPAccessibleLocation *CPPNet::GetNext_AccessibleLocations(POSITION &pos, EEntityType featureType, CString targetType, CString netName, ETestSurface surface) const
{
   CPPAccessibleLocation *pAccLoc = m_accessibleLocations.GetNext(pos);

   while (pos)
   {
      CPPAccessibleLocation *pNextAccLoc = m_accessibleLocations.GetAt(pos);

      if ((featureType < entityTypeMIN || pNextAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pNextAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pNextAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pNextAccLoc->GetSurface() == surface))
         break;

      m_accessibleLocations.GetNext(pos);
   }

   return pAccLoc;
}

int CPPNet::GetCount_TestProbes() const
{
   int probeCount = 0;
   for (POSITION pos=m_probes.GetHeadPosition(); pos!=NULL; m_probes.GetNext(pos))
   {
      CPPProbe *probe = m_probes.GetAt(pos);
      
      if (probe->GetResourceType() == testResourceTypeTest)
         probeCount++;
   }

   return probeCount;
}

int CPPNet::GetCount_PowerInjectionProbes() const
{
   int probeCount = 0;
   for (POSITION pos=m_probes.GetHeadPosition(); pos!=NULL; m_probes.GetNext(pos))
   {
      CPPProbe *probe = m_probes.GetAt(pos);
      
      if (probe->GetResourceType() == testResourceTypePowerInjection)
         probeCount++;
   }

   return probeCount;
}

bool CPPNet::HasUnplacedProbe()
{
   for (POSITION pos=m_probes.GetHeadPosition(); pos!=NULL; m_probes.GetNext(pos))
   {
      CPPProbe *probe = m_probes.GetAt(pos);
      if (!probe->IsPlaced())
         return true;      // found an unplaced probe
   }

   // Did not find any unplaced probes
   return false;
}

CPPProbe *CPPNet::AddHead_Probes(ETestResourceType resType, CString netName, CString probeRefname)
{
   CPPProbe *probe = new CPPProbe(m_pDoc, resType, netName);
   probe->SetProbeRefname(probeRefname);
   m_probes.AddHead(probe);

   return probe;
}

CPPProbe *CPPNet::AddTail_Probes(ETestResourceType resType, CString netName, CString probeRefname)
{
   CPPProbe *probe = new CPPProbe(m_pDoc, resType, netName);
   probe->SetProbeRefname(probeRefname);
   m_probes.AddTail(probe);

   return probe;
}

void CPPNet::RemoveAt_Probes(POSITION pos)
{
   CPPProbe *probe = m_probes.GetAt(pos);
   
   m_probes.RemoveAt(pos);
   delete probe;
}

void CPPNet::RemoveAll_Probes()
{
   POSITION pos = m_probes.GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = m_probes.GetNext(pos);
      delete probe;
   }
   m_probes.RemoveAll();
}

CPPAccessibleLocation *CPPNet::AddHead_AccessibleLocations(DataStruct *testAccessData)
{
   CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(m_pDoc, testAccessData);
   m_accessibleLocations.AddHead(accessibleLocation);

   return accessibleLocation;
}

CPPAccessibleLocation *CPPNet::AddTail_AccessibleLocations(DataStruct *testAccessData)
{
   CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(m_pDoc, testAccessData);
   m_accessibleLocations.AddTail(accessibleLocation);
   return accessibleLocation;
}

CPPAccessibleLocation *CPPNet::AddTail_AccessibleLocations(const CEntity& entity, CPoint2d location, CString targetType, CString netName, ETestSurface surface)
{
   CPPAccessibleLocation *accessibleLocation = new CPPAccessibleLocation(*m_pDoc, entity, location, targetType, netName, surface);
   m_accessibleLocations.AddTail(accessibleLocation);

   return accessibleLocation;
}

void CPPNet::RemoveAt_AccessibleLocations(POSITION pos)
{
   CPPAccessibleLocation *accessibleLocation = m_accessibleLocations.GetAt(pos);
   
   m_accessibleLocations.RemoveAt(pos);
   delete accessibleLocation;

   // TODO: m_LocationTree.remove( ... (???)
}

void CPPNet::RemoveAll_AccessibleLocations()
{
   POSITION pos = m_accessibleLocations.GetHeadPosition();
   while (pos)
   {
      CPPAccessibleLocation *accessibleLocation = m_accessibleLocations.GetNext(pos);
      delete accessibleLocation;
   }
   m_accessibleLocations.RemoveAll();
}

void CPPNet::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::Net %s::\n", indent, " ", m_sNetName);
   indent += 3;

   file.WriteString("%*s::Probes::\n", indent, " ");
   indent += 3;
   POSITION pos = m_probes.GetHeadPosition();
   while (pos)
   {
      CPPProbe *probe = m_probes.GetNext(pos);
      probe->DumpToFile(file, indent);
   }
   indent -= 3;

   file.WriteString("%*s::Accessable Locations::\n", indent, " ");
   indent += 3;
   pos = m_accessibleLocations.GetHeadPosition();
   while (pos)
   {
      CPPAccessibleLocation *accessibleLocation = m_accessibleLocations.GetNext(pos);
      accessibleLocation->DumpToFile(file, indent);
   }
   indent -= 3;

   switch (m_eNetType)
   {
   case probeableNetTypePower:
      file.WriteString("%*sm_eNetType = %s\n", indent, " ", "Power");
      break;
   case probeableNetTypeSignal:
      file.WriteString("%*sm_eNetType = %s\n", indent, " ", "Signal");
      break;
   case probeableNetTypeGround:
      file.WriteString("%*sm_eNetType = %s\n", indent, " ", "Ground");
      break;
   }
   file.WriteString("%*sm_iTestResourcesNeeded = %d\n", indent, " ", m_iTestResourcesNeeded);
   file.WriteString("%*sm_iPwrInjResourcesNeeded = %d\n", indent, " ", m_iPwrInjResourcesNeeded);

   indent -= 3;
}

void CPPNet::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<ProbeNet Name=\"%s\">\n", SwapSpecialCharacters(m_sNetName));

   writeFormat.pushHeader("  ");
   m_probes.WriteXML(writeFormat, progress);
   writeFormat.popHeader();

   writeFormat.writef("</ProbeNet>\n", m_sNetName);
}

/******************************************************************************
* CPPNetMap
*/
void CPPNetMap::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   POSITION pos = GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      GetNextAssoc(pos, netName, ppNet);

      ppNet->WriteXML(writeFormat, progress);
   }
}

/******************************************************************************
*/
int CPPNetMap::AscendingNetNameSortFunc(const void *a, const void *b)
{
   CPPNet *ppNetA = (CPPNet*)(((SElement*) a )->pObject->m_object);
   CPPNet *ppNetB = (CPPNet*)(((SElement*) b )->pObject->m_object);

   return (ppNetA->GetNetName().CompareNoCase(ppNetB->GetNetName()));
}


/******************************************************************************
* CProbeCount
*/
CProbeCount::CProbeCount()
{
   m_bUsedTop = false;
   m_bUsedBot = false;
   m_eSurface = testSurfaceMIN;
   m_iTestResourceCountTop = 0;
   m_iPwrInjResourceCountTop = 0;
   m_iTestResourceCountBot = 0;
   m_iPwrInjResourceCountBot = 0;
}

CProbeCount::CProbeCount(CString probeName, ETestSurface surface, bool used)
{
   m_sName = probeName;
   m_eSurface = surface;
   m_iTestResourceCountTop = 0;
   m_iPwrInjResourceCountTop = 0;
   m_iTestResourceCountBot = 0;
   m_iPwrInjResourceCountBot = 0;

   m_bUsedTop = false;
   m_bUsedBot = false;
   if (surface == testSurfaceTop)
      m_bUsedTop = used;
   else if (surface == testSurfaceBottom)
      m_bUsedBot = used;
}

CProbeCount::CProbeCount(CProbeCount &probeCount)
{
   m_bUsedTop = probeCount.m_bUsedTop;
   m_bUsedBot = probeCount.m_bUsedBot;
   m_sName = probeCount.m_sName;
   m_eSurface = probeCount.m_eSurface;
   m_iTestResourceCountTop = probeCount.m_iTestResourceCountTop;
   m_iPwrInjResourceCountTop = probeCount.m_iPwrInjResourceCountTop;
   m_iTestResourceCountBot = probeCount.m_iTestResourceCountBot;
   m_iPwrInjResourceCountBot = probeCount.m_iPwrInjResourceCountBot;
}



/******************************************************************************
* CPolyListItem
*/
CPolyListItem::CPolyListItem(CPolyListItem &listItem)
{
   m_pPolyList = listItem.m_pPolyList;
   m_eSurface = listItem.m_eSurface;
}

CPolyListItem::CPolyListItem(CPolyList *polyList, ETestSurface surface)
{
   m_pPolyList = *polyList;
   m_eSurface = surface;
}

CPolyListItem::~CPolyListItem()
{
   //POSITION pos = m_pPolyList.GetHeadPosition();
   //while (pos)
   //{
   //   delete m_pPolyList.GetNext(pos);
   //}
   //m_pPolyList.RemoveAll();
}


/******************************************************************************
* CProbeResult
*/
CPPProbeResult::CPPProbeResult(ETestResourceType resType, CString netName, CString accessName, int accessEntityNum, CString templateName, CString probeRefname, double locX, double locY)
{
   m_eResourceType = resType;
   m_sNetName = netName;
   m_sAccessName = accessName;
   m_nAccessEntityNum = accessEntityNum;
   m_sProbeTemplateName = templateName;
   m_sProbeRefname = probeRefname;

   m_locX = locX;
   m_locY = locY;
}

CPPProbeResult::CPPProbeResult(const CPPProbeResult& other)
{
   m_eResourceType = other.m_eResourceType;
   m_sNetName = other.m_sNetName;
   m_sAccessName = other.m_sAccessName;
   m_nAccessEntityNum = other.m_nAccessEntityNum;
   m_sProbeTemplateName = other.m_sProbeTemplateName;
   m_sProbeRefname = other.m_sProbeRefname;

   m_locX = other.m_locX;
   m_locY = other.m_locY;
}

CPPProbeResult::~CPPProbeResult()
{
}

CPPProbeResult * CPPProbeResult::LoadXML(CXMLNode *node)
{
   CPPProbeResult *ppRes = NULL;

   CString nodeName;
   if (node->GetName(nodeName) && nodeName == "Probe")
   {
      CString buf;

      CString probeRefname;
      if (node->GetAttrValue("Number", buf))  // Preferred traditinal RSI keyword
         probeRefname = buf;
      else if (node->GetAttrValue("Name", buf))  // vPlan did this for awhile, but is getting changed to trad RSI style.
         probeRefname = buf;

      ETestResourceType resType = testResourceTypeTest;
      if (node->GetAttrValue("TestResource", buf))
         resType = (buf=="Power Injection")?testResourceTypePowerInjection:testResourceTypeTest;

      CString accessName;
      if (node->GetAttrValue("AccessName", buf))
         accessName = buf;

      CString templateName;
      if (node->GetAttrValue("TemplateName", buf))
         templateName = buf;

      CString netName;
      if (node->GetAttrValue("NetName", buf))
         netName = buf;

      double locX;
      if (node->GetAttrValue("X", buf))
         locX = atof(buf);

      double locY;
      if (node->GetAttrValue("Y", buf))
         locY = atof(buf);

      int accessEntityNum = -1;  // Actual value not known right now, but that is okay. Value not in XML.

      ppRes = new CPPProbeResult(resType, netName, accessName, accessEntityNum, templateName, probeRefname, locX, locY);
   }

   return ppRes;
}


/******************************************************************************
* CProbePlacementSolution
*/
CPPNetResult::CPPNetResult(CString netName, bool noProbe)
{
   m_sNetName = netName;
   m_bNoProbe = noProbe;
   m_probeResultList.empty();
}

CPPNetResult::CPPNetResult(const CPPNetResult& other)
{
   m_sNetName = other.m_sNetName;
   m_bNoProbe = other.m_bNoProbe;

   POSITION pos = other.m_probeResultList.GetHeadPosition();
   while (pos != NULL)
   {
      CPPProbeResult* probeResult = new CPPProbeResult(*other.m_probeResultList.GetNext(pos));
      if (probeResult == NULL)
         continue;
      
      m_probeResultList.AddTail(probeResult);
   }
}

CPPNetResult::~CPPNetResult()
{
   m_probeResultList.empty();
}

CPPProbeResult* CPPNetResult::AddProbeResult(ETestResourceType resType, CString netName, CString accessName, int accessEntityNum, CString templateName, CString probeRefname)
{
   CPPProbeResult* probeResult = new CPPProbeResult(resType, netName, accessName, accessEntityNum, templateName, probeRefname);
   m_probeResultList.AddTail(probeResult);

   return probeResult;
}

CPPProbeResult* CPPNetResult::AddProbeResult(CPPProbeResult *ppRes)
{
   if (ppRes != NULL)
      m_probeResultList.AddTail(ppRes);
   
   return ppRes;
}

void CPPNetResult::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<ProbeNet Name=\"%s\" NoProbe=\"%s\">\n", SwapSpecialCharacters(m_sNetName), m_bNoProbe?"True":"False");
   writeFormat.pushHeader("  ");
   
   POSITION pos = m_probeResultList.GetHeadPosition();
   while (pos != NULL)
   {
      CPPProbeResult* probeResult = m_probeResultList.GetNext(pos);
      if (probeResult == NULL)
         continue;

      // The location is set in DFT Solution imported from vPlan, but in general does not exist
      // in traditional RSI DFT Solution. Write the location only if it appears to have been set.
      CString locationStr;
      CPoint2d location = probeResult->GetLocation();
      if (location.x != DBL_MIN || location.y != DBL_MIN)
         locationStr.Format(" X=\"%f\" Y=\"%f\"", location.x, location.y);

      writeFormat.writef("<Probe Number=\"%s\" TestResource=\"%s\" AccessName=\"%s\" TemplateName=\"%s\"%s/>\n",
                         probeResult->GetProbeRefname(),
                         (probeResult->GetResourceType()==testResourceTypePowerInjection)?"Power Injection":"Test",
                         SwapSpecialCharacters(probeResult->GetAccessName()),
                         SwapSpecialCharacters(probeResult->GetProbeTemplateName()),
                         locationStr);
   }

   writeFormat.popHeader();
   writeFormat.writef("</ProbeNet>\n");
}

int CPPNetResult::LoadXML(CXMLNode* node)
{
   if (node == NULL)
      return -1;

   CString nodeName;
   if (!node->GetName(nodeName) || nodeName != "ProbeNet")
      return 0;

   CString name;
   if (!node->GetAttrValue("Name", name))
      return 0;
   m_sNetName = name;

   CString buf;
   if (node->GetAttrValue("NoProbe", buf))
   {
      if (buf == "True")
         m_bNoProbe = true;
      else
         m_bNoProbe = false;
   }

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName) || nodeName != "Probe")
         continue;

      CString buf;
      if (!subNode->GetAttrValue("Number", buf))
         continue;
      CString probeRefname = buf;

      if (!subNode->GetAttrValue("TestResource", buf))
         continue;
      ETestResourceType resType = (buf=="Power Injection")?testResourceTypePowerInjection:testResourceTypeTest;

      if (!subNode->GetAttrValue("AccessName", buf))
         continue;
      CString accessName = buf;

      if (!subNode->GetAttrValue("TemplateName", buf))
         continue;
      CString templateName = buf;

      int accessEntityNum = -1;  // Actual value not known right now, but that is okay. Value not in XML.

      CPPProbeResult* probeResult = new CPPProbeResult(resType, m_sNetName, accessName, accessEntityNum, templateName, probeRefname);
      m_probeResultList.AddTail(probeResult);

      delete subNode;
   }

   return 0;
}


/******************************************************************************
* CProbePlacementSolution
*/
CProbePlacementSolution::CProbePlacementSolution(CCEtoODBDoc *doc, CTestPlan *testPlan, FileStruct *file)
{
   m_pDoc = doc;
   m_pTestPlan = testPlan;
   m_probeNamer.Reset();
   m_pProgressDlg = NULL;

   m_pFile = file;

   if (file == NULL)
      m_pFile = m_pDoc->getFileList().GetOnlyShown(blockTypePcb);

   gatherPackageOutlines();
}

CProbePlacementSolution::CProbePlacementSolution(CProbePlacementSolution &ppSolution, CTestPlan *testPlan)
{
   m_pDoc = ppSolution.m_pDoc;
   m_pFile = ppSolution.m_pFile;
   m_pTestPlan = testPlan;
   m_probeNamer = ppSolution.m_probeNamer;
   m_pProgressDlg = NULL;

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *pNet = NULL;

      ppSolution.m_ppNets.GetNextAssoc(pos, netName, pNet);
      m_ppNets.SetAt(netName, new CPPNet(*pNet));
   }

   pos = ppSolution.m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &pListItem = ppSolution.m_polyList.GetNext(pos);

      m_polyList.AddTail(pListItem);
   }

   pos = ppSolution.m_netResultList.GetHeadPosition();
   while (pos != NULL)
   {
      CPPNetResult* netResult = ppSolution.m_netResultList.GetNext(pos);
      if (netResult == NULL)
         continue;

      CPPNetResult* newNetResult = new CPPNetResult(*netResult);
      m_netResultList.AddTail(newNetResult);
   }
}

CProbePlacementSolution::~CProbePlacementSolution()
{
   RemoveAll_Nets();

   destroyProgress();
}

void CProbePlacementSolution::calculateNeededProbes()
{
   CDFTNetConditions &netConditions = m_pTestPlan->GetNetConditions();

   // start with the net conditions
   POSITION pos = netConditions.GetHeadPosition_ProbeableNets();
   while (pos)
   {
      CDFTProbeableNet *pNet = netConditions.GetNext_ProbeableNets(pos);
      CPPNet *ppNet = NULL;

      if (!m_ppNets.Lookup(pNet->GetName(), ppNet))
      {
         ppNet = new CPPNet(m_pDoc, pNet->GetName());
         m_ppNets.SetAt(pNet->GetName(), ppNet);
      }

      ppNet->SetNetType(pNet->GetNetType());
      if (pNet->GetNoProbe())
      {
         ppNet->SetNoProbeFlag(true);
         ppNet->SetTestResourcesNeeded(0);
         ppNet->SetPowerInjectionResourcesNeeded(0);
         continue;
      }
      ppNet->SetTestResourcesNeeded((pNet->GetTestResourcesPerNet()<1)?1:pNet->GetTestResourcesPerNet());
      ppNet->SetPowerInjectionResourcesNeeded(pNet->GetPowerInjectionResourcesPerNet());
   }

   // evalute the TESTNETPROBE attribute on each net
   WORD testNetProbesKW = m_pDoc->IsKeyWord(ATT_TEST_NET_PROBES, 0);
   if (testNetProbesKW >= 0)
   {
      pos = m_pFile->getNetList().GetHeadPosition();
      while (pos)
      {
         NetStruct *pNet = m_pFile->getNetList().GetNext(pos);
         CPPNet *ppNet = NULL;

         if (!m_ppNets.Lookup(pNet->getNetName(), ppNet))
         {
            ppNet = new CPPNet(m_pDoc, pNet->getNetName());
            m_ppNets.SetAt(pNet->getNetName(), ppNet);
         }
         ppNet->SetNet(pNet);

         // if set to not probe, we have no test needs
         if (ppNet->GetNoProbeFlag())
            continue;

         Attrib *attrib = NULL;
         if (!(pNet->getAttributes() && pNet->getAttributes()->Lookup(testNetProbesKW, attrib)))
            continue;

         if (ppNet->GetTestResourcesNeeded() < attrib->getIntValue())
            ppNet->SetTestResourcesNeeded(attrib->getIntValue());
      }
   }

   // calculate power injection needs
   int powerUsage = m_pTestPlan->GetPowerInjectionUsage();
   int powerValue = m_pTestPlan->GetPowerInjectionValue();

   if (powerUsage != 0) // No power injection
   {
      int newPowerInjectionValue = 0;
      
      if (powerUsage == 1)    // Probes per power rail
         newPowerInjectionValue = powerValue;
      else if (powerUsage == 3)     // Probes per Number of Nets per Power Rail
      {
         // default to one so we don't divide by zero
         if (powerValue == 0)
            powerValue = 1;
         newPowerInjectionValue = m_pFile->getNetCount() / powerValue;
      }

      POSITION pos = m_ppNets.GetStartPosition();
      while (pos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(pos, netName, ppNet);

         if (ppNet->GetNetType() != probeableNetTypePower  && ppNet->GetNetType() != probeableNetTypeGround)
            continue;

         if (ppNet->GetNoProbeFlag())
            continue;

         if (powerUsage == 2)    // Probes per Number of Connections per Power Rail
         {
            NetStruct *pNet = ppNet->GetNet();
            int connectionCount = pNet->getCompPinCount();

            // default to one so we don't divide by zero
            if (powerValue == 0)
               powerValue = 1;
            newPowerInjectionValue = connectionCount / powerValue;
         }

         if (ppNet->GetPowerInjectionResourcesNeeded() < newPowerInjectionValue)
            ppNet->SetPowerInjectionResourcesNeeded(newPowerInjectionValue);
      }
   }
}

void CProbePlacementSolution::gatherPackageOutlines()
{
   if (m_pFile != NULL)
   {
      POSITION pos = m_pFile->getBlock()->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = m_pFile->getBlock()->getNextDataInsert(pos);

         if (data->getInsert()->getInsertType() != insertTypePcbComponent)
            continue;

         BlockStruct *subBlock = m_pDoc->getBlockAt(data->getInsert()->getBlockNumber());

         POSITION subPos = subBlock->getHeadDataPosition();
         while (subPos)
         {
            DataStruct *subData = subBlock->getNextData(subPos);

            if (subData->getDataType() == dataTypePoly && subData->getGraphicClass() == graphicClassPackageOutline)
            {
               CTMatrix mat(m_pFile->getTMatrix());
               mat.translateCtm(data->getInsert()->getOrigin2d());
               mat.rotateRadiansCtm(data->getInsert()->getAngle());
               mat.scaleCtm(data->getInsert()->getGraphicMirrored() ? -data->getInsert()->getScale() : data->getInsert()->getScale(),data->getInsert()->getScale());

               ETestSurface outlineSurface = data->getInsert()->getLayerMirrored()?testSurfaceBottom:testSurfaceTop;                
               CPolyListItem polyListItem = CPolyListItem(subData->getPolyList(), outlineSurface);
               polyListItem.Transform(mat);
               m_polyList.AddTail(polyListItem);
               break;
            }
         }
      }
   }
}

void CProbePlacementSolution::applyNoProbeFeatures(CDFTPreconditions &Preconditions)
{
   POSITION noProbePos = Preconditions.GetHeadPosition_NoProbedFeatures();
   while (noProbePos)
   {
      CDFTFeature *pFeature = Preconditions.GetNext_NoProbedFeatures(noProbePos);
      CString featureComp = pFeature->GetComponent(), featurePin = pFeature->GetPin();
      bool featureFound = false;

      // see if there is a comp/pin access point for this feature
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos && !featureFound)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
         while (accPos)
         {
            CPPAccessibleLocation *pAccLoc = ppNet->GetNext_AccessibleLocations(accPos);

            if (pAccLoc->getFeatureEntity().getEntityType() != entityTypeCompPin)
               continue;

            CompPinStruct* cp = pAccLoc->getFeatureEntity().getCompPin();

            CString comp = cp->getRefDes();
            CString pin = cp->getPinName();

            if (featureComp == comp && featurePin == pin)
            {
               pAccLoc->SetFeatureCondition(featureConditionNoProbe);
               featureFound = true;
               break;
            }
         }
      }
   }
}

void CProbePlacementSolution::createForcedAccessLocations(CDFTPreconditions &Preconditions)
{
   POSITION forcedProbePos = Preconditions.GetHeadPosition_ForcedFeatures();
   while (forcedProbePos)
   {
      CDFTFeature *pFeature = Preconditions.GetNext_ForcedFeatures(forcedProbePos);
      CString featureComp = pFeature->GetComponent(), featurePin = pFeature->GetPin();
      CPPAccessibleLocation *pFoundAccLoc = NULL;
      CPPNet *ppFoundNet = NULL;

      // see if there is a comp/pin access point for this feature
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos && ppFoundNet == NULL)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations(entityTypeCompPin);
         while (accPos && pFoundAccLoc == NULL)
         {
            CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos, entityTypeCompPin);
            CompPinStruct* cp = accLoc->getFeatureEntity().getCompPin();

            CString comp = cp->getRefDes();
            CString pin = cp->getPinName();
            if (featureComp != comp || featurePin != pin)
               continue;

            pFoundAccLoc = accLoc;
            ppFoundNet = ppNet;
         }
      } // END while (netPos && !featureFound) ...

      // if there is no access location, then we need to create it (as unplaced)
      if (pFoundAccLoc == NULL)
      {
         CPoint2d pinLoc;
         BlockStruct *block = m_pFile->getBlock();
         POSITION dataPos = block->getHeadDataInsertPosition();
         while (dataPos)   
         {
            DataStruct *pcbData = block->getNextDataInsert(dataPos);
            if (pcbData->getInsert()->getInsertType() != insertTypePcbComponent)
               continue;

            if (pcbData->getInsert()->getRefname() != featureComp)
               continue;

            BlockStruct *subBlock = m_pDoc->getBlockAt(pcbData->getInsert()->getBlockNumber());
            POSITION subDataPos = subBlock->getHeadDataInsertPosition();
            while (subDataPos)
            {
               DataStruct *pinData = subBlock->getNextDataInsert(subDataPos);
               if (pinData->getInsert()->getInsertType() != insertTypePin)
                  continue;

               if (pinData->getInsert()->getRefname() != featurePin)
                  continue;

               Point2 point2(pinData->getInsert()->getOrigin().x, pinData->getInsert()->getOrigin().y);
               DTransform xform(pcbData->getInsert()->getOrigin2d().x, pcbData->getInsert()->getOrigin2d().y,
                  1, pcbData->getInsert()->getAngle(), pcbData->getInsert()->getMirrorFlags());
               xform.TransformPoint(&point2);
               pinLoc.x = point2.x;
               pinLoc.y = point2.y;
            }
         }

         NetStruct *net = NULL;
         CompPinStruct *cp = FindCompPin(m_pFile, featureComp, featurePin, &net);
         if (cp == NULL || net == NULL)
            continue;

         // Create accessible location where the pointer to the test access point is NULL.  This is how we know it doesn't
         // already exist from Access Analysis and needs to be created when placing the probe down.
         pFoundAccLoc = AddTail_AccessibleLocations(net->getNetName(), CEntity(cp), pinLoc, "", testSurfaceTop);
      }

      if (pFoundAccLoc != NULL)
         pFoundAccLoc->SetFeatureCondition(featureConditionForced);
   }
}

void CProbePlacementSolution::applyForcedFeatures(CDFTPreconditions &Preconditions)
{
   createForcedAccessLocations(Preconditions);

   POSITION forcedProbePos = Preconditions.GetHeadPosition_ForcedFeatures();
   while (forcedProbePos)
   {
      CDFTFeature *pFeature = Preconditions.GetNext_ForcedFeatures(forcedProbePos);
      CString featureComp = pFeature->GetComponent();
      CString featurePin = pFeature->GetPin();
      CPPAccessibleLocation *pFoundAccLoc = NULL;
      CPPNet *ppFoundNet = NULL;

      if (!featureComp.IsEmpty() && !featurePin.IsEmpty())
      {
         // see if there is a comp/pin access point for this feature
         POSITION netPos = m_ppNets.GetStartPosition();
         while (netPos && ppFoundNet == NULL)
         {
            CString netName;
            CPPNet *ppNet = NULL;
            m_ppNets.GetNextAssoc(netPos, netName, ppNet);

            POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations(entityTypeCompPin);
            while (accPos && pFoundAccLoc == NULL)
            {
               CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos, entityTypeCompPin);
               CompPinStruct* cp = accLoc->getFeatureEntity().getCompPin();

               CString comp = cp->getRefDes();
               CString pin = cp->getPinName();
               if (featureComp == comp && featurePin == pin)
               {
                  pFoundAccLoc = accLoc;
                  pFoundAccLoc->SetFeatureCondition(featureConditionForced);
                  ppFoundNet = ppNet;
               }
            }
         } // END while (netPos && ppFoundNet == NULL) ...


         if (pFoundAccLoc != NULL)
         {
            // determine what probe (template) we can place at this point
            int cost = 0;
            CDFTProbeTemplate *probeTemplate = NULL;
            if (pFoundAccLoc->GetSurface() == testSurfaceTop)
               probeTemplate = m_pTestPlan->GetProbes().GetTail_TopProbes();
            else
               probeTemplate = m_pTestPlan->GetProbes().GetTail_BotProbes();

            // Find a probe on the specified net and place it
            CPPProbe *pProbe = NULL;
            POSITION probePos = ppFoundNet->GetHeadPosition_Probes();
            while (probePos && pProbe == NULL)
            {
               CPPProbe *probe = ppFoundNet->GetNext_Probes(probePos);

               if (probe->IsPlaced())
                  continue;

               if (probe->GetNetName() != pFoundAccLoc->GetNetName())
                  continue;

               pProbe = probe;
               pFoundAccLoc->PlaceProbe(pProbe);
               pProbe->SetProbeTemplate(probeTemplate);
            }

            // if the probe doesn't exist, we need to create it
            if (pProbe == NULL)
            {
               CString probeRefname = m_probeNamer.GetNextProbeRefname(ppFoundNet, testResourceTypeTest);

               CPPProbe *pProbe = ppFoundNet->AddTail_Probes(testResourceTypeTest, pFoundAccLoc->GetNetName(), probeRefname);

               pFoundAccLoc->PlaceProbe(pProbe);
               pProbe->SetProbeTemplate(probeTemplate);
            }
         }
      }
   } // END while (forcedProbePos) ...
}

void CProbePlacementSolution::applyPreconditions(CDFTPreconditions &Preconditions)
{
   // use preconditions only when we aren't forcing probes on all access markers
   if (!m_pTestPlan->GetPlaceOnAllAccessible())
   {
      // For each feature specified to not be probed, set any accessible location
      // at a specific feature to never be probed.  We don't need to worry about
      // any features that don't have accessible locations because they will not
      // be probed anyway, unless they are forced.  Forced probes take precedence
      // and will create an accessible location if needed.
      applyNoProbeFeatures(Preconditions);
      applyForcedFeatures(Preconditions);
   }
}

void CProbePlacementSolution::addAllNets()
{
   RemoveAll_Nets();

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Gathering nets...");

   //////////////////////////////////////////////////
   // Gather nets
   POSITION pos = m_pFile->getNetList().GetHeadPosition();
   while (pos)
   {
      NetStruct *net = m_pFile->getNetList().GetNext(pos);

      CPPNet *ppNet = NULL;
      if (!m_ppNets.Lookup(net->getNetName(), ppNet))
      {
         m_ppNets.SetAt(net->getNetName(), new CPPNet(m_pDoc, net->getNetName()));
      }
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();
}

void CProbePlacementSolution::gatherAccesibleLocations()
{
   addAllNets();

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Gathering accessible locations...");

   //////////////////////////////////////////////////
   // Gather accessible locations
   BlockStruct *fileBlock = m_pFile->getBlock();
   if (fileBlock != NULL)
   {
      POSITION pos = fileBlock->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = fileBlock->getNextDataInsert(pos);
         if (data->isInsertType(insertTypeTestAccessPoint))
         {
            AddTail_AccessibleLocations(data);
         }
      }
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();
}

void CProbePlacementSolution::createProbes()
{
   RemoveAll_Probes();
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      // use the number of specified resources
      if (!m_pTestPlan->GetPlaceOnAllAccessible())
      {
         if (ppNet->GetNoProbeFlag())
            continue;

         for (int i=0; i<ppNet->GetTestResourcesNeeded(); i++)
            AddTail_Probes(netName, testResourceTypeTest);

         for (int i=0; i<ppNet->GetPowerInjectionResourcesNeeded(); i++)
            AddTail_Probes(netName, testResourceTypePowerInjection);
      }
      else
      {
         // we want to force a probe on all access markers, so we need to make as
         // many probes as there are markers
         for (int i=0; i<ppNet->GetCount_AccessibleLocations(); i++)
            AddTail_Probes(netName, testResourceTypeTest);
      }
   }
}

void CProbePlacementSolution::createAdditionalProbes()
{
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      // use the number of psecified resources
      if (!m_pTestPlan->GetPlaceOnAllAccessible())
      {
         if (ppNet->GetNoProbeFlag())
            continue;

         for (int i=ppNet->GetCount_TestProbes(); i<ppNet->GetTestResourcesNeeded(); i++)
            AddTail_Probes(netName, testResourceTypeTest);

         for (int i=ppNet->GetCount_PowerInjectionProbes(); i<ppNet->GetPowerInjectionResourcesNeeded(); i++)
            AddTail_Probes(netName, testResourceTypePowerInjection);
      }
      else
      {
         // we want to force a probe on all access markers, so we need to make as
         // many probes as there are markers (minus the ones that already have exist)
         for (int i=ppNet->GetCount_TestProbes(); i<ppNet->GetCount_AccessibleLocations(); i++)
            AddTail_Probes(netName, testResourceTypeTest);
      }
   }
}

void CProbePlacementSolution::gatherAndMatchProbes()
{
   int normalProbeNum = m_pTestPlan->GetProbeStartNumber();
   int powerProbeNum = 1;
   int groundProbeNum = 1;

   //////////////////////////////////////////////////
   // Gather accessible locations with probes
   BlockStruct *fileBlock = m_pFile->getBlock();
   if (fileBlock != NULL)
   {
      POSITION pos = fileBlock->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = fileBlock->getNextDataInsert(pos);
         InsertStruct *insert = data->getInsert();

         if (insert->getInsertType() != insertTypeTestProbe)
            continue;

         long entityLink = 0;
         Attrib *attrib = NULL;
         WORD ddKw = m_pDoc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);
         if (data->lookUpAttrib(ddKw, attrib))
            entityLink = attrib->getIntValue();

         CString netName;
         WORD netKw = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
         if (data->lookUpAttrib(netKw , attrib))
            netName = get_attvalue_string(m_pDoc, attrib);

         ETestResourceType testRes = testResourceTypeTest;
         WORD testResKw = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
         if (data->lookUpAttrib(testResKw , attrib))
            testRes = ((CString)"Test" == get_attvalue_string(m_pDoc, attrib))?testResourceTypeTest:testResourceTypePowerInjection;

         CPPProbe *probe = AddTail_Probes(netName, testRes);
         if (m_pTestPlan->RetainProbeNames())
         {
            probe->SetProbeRefname( insert->getRefname() );
         }
         else
         {
            bool power = false;
            bool ground = false;
            CPPNet *ppNet = NULL;
            if (!m_ppNets.Lookup(netName, ppNet))
            {
               if (ppNet->IsPowerNet())
                  power = true;
               if (ppNet->IsGroundNet())
                  ground = true;
            }
            CString refname;
            if (power)
               refname.Format("P%d", powerProbeNum++);
            else if (ground)
               refname.Format("G%d", groundProbeNum++);
            else
               refname.Format("%d", normalProbeNum++);    

            probe->SetProbeRefname(refname);
         }

         // Only "Placed" probes should be marked as existing
         bool placed = false;
         WORD testProbePlacedKw = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
         if (data->lookUpAttrib(testProbePlacedKw , attrib))
            placed = ((CString)"Placed" == ((CString)get_attvalue_string(m_pDoc, attrib)));

         probe->SetAsExisting(placed);

         ETestSurface probeSurface = testSurfaceUnset;
         if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbePositions())
         {
            // Get the access point the probe is on
            POSITION accPos = GetHeadPosition_AccessibleLocations(netName);
            while (accPos)
            {
               CPPAccessibleLocation *accLoc = GetNext_AccessibleLocations(netName, accPos);

               if (accLoc->GetTAInsert()->getEntityNumber() != entityLink)
                  continue;

               probeSurface = accLoc->GetSurface();
               if (!accLoc->IsProbed())
               {
                  accLoc->PlaceProbe(probe);
                  //((CFixtureAccLoc*)accLoc)->SetForFixtureFlag(false);
                  break;
               }
               else if (!accLoc->IsDoubleWired())
               {
                  accLoc->PlaceDoubleWiredProbe(probe);
                  //((CFixtureAccLoc*)accLoc)->SetForFixtureFlag(false);
                  break;
               }
            }
         }
         else
            probeSurface = insert->getGraphicMirrored()?testSurfaceBottom:testSurfaceTop;

         if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbeSizes() && entityLink > 0)
         {
            // Get the probe size
            BlockStruct *insBlock = m_pDoc->getBlockAt(insert->getBlockNumber());
            CString m_sTemplateName = insBlock->getName().Mid(0, insBlock->getName().Find("_"));
            if (probeSurface == testSurfaceTop)
            {
               POSITION probePos = NULL;
               CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_TopProbes(m_sTemplateName, probePos);
               if (pTemplate != NULL)
                  probe->SetProbeTemplate(pTemplate);
               //POSITION probePos = m_pTestPlan->GetProbes().GetHeadPosition_TopProbes();
               //while (probePos)
               //{
               // CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_TopProbes(probePos);

               // if (pTemplate->GetName() ==  m_sTemplateName)
               // {
               //    probe->SetProbeTemplate(pTemplate);
               //    break;
               // }
               //} // END while (probePos) ...
            }
            else if (probeSurface == testSurfaceBottom)
            {
               POSITION probePos = NULL;
               CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_BotProbes(m_sTemplateName, probePos);
               if (pTemplate != NULL)
                  probe->SetProbeTemplate(pTemplate);
               //POSITION probePos = m_pTestPlan->GetProbes().GetHeadPosition_BotProbes();
               //while (probePos)
               //{
               // CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_BotProbes(probePos);

               // if (pTemplate->GetName() ==  m_sTemplateName)
               // {
               //    probe->SetProbeTemplate(pTemplate);
               //    break;
               // }
               //} // END while (probePos) ...
            }
         }
      } // END while (pos) ...
   } // END if (fileBlock != NULL) ...
}

void CProbePlacementSolution::gatherPlacedProbes()
{
   RemoveAll_Probes();
   WORD netNameKW = m_pDoc->RegisterKeyWord(ATT_NETNAME, 0, valueTypeString);
   WORD testResKW = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
   WORD dataLinkKW = m_pDoc->RegisterKeyWord(ATT_DDLINK, 0, valueTypeInteger);

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Gathering Probes...");

   //////////////////////////////////////////////////
   // Gather accessible locations
   BlockStruct *fileBlock = m_pFile->getBlock();
   if (fileBlock != NULL)
   {
      POSITION pos = fileBlock->getHeadDataInsertPosition();
      while (pos)
      {
         DataStruct *data = fileBlock->getNextDataInsert(pos);
         InsertStruct *insert = data->getInsert();

         // make sure we have a probe
         if (insert->getInsertType() != insertTypeTestProbe)
            continue;

         // get the netname
         Attrib *attrib = NULL;
         if (!data->lookUpAttrib(netNameKW, attrib))
            continue;
         CString netname = get_attvalue_string(m_pDoc, attrib);

         // get the resource type
         if (!data->lookUpAttrib(testResKW, attrib))
            continue;
         CString testRes = get_attvalue_string(m_pDoc, attrib);;
         ETestResourceType resource = testResourceTypeTest;
         if (testRes == "Power Injection")
            resource = testResourceTypePowerInjection;

         // get the probe data link to the access marker
         if (!data->lookUpAttrib(dataLinkKW, attrib))
            continue;
         int datalink = attrib->getIntValue();

         // find the accessible location the probe is linked to
         CPPNet *ppNet = NULL;
         if (!m_ppNets.Lookup(netname, ppNet))
            continue;
         for (POSITION netPos = ppNet->GetHeadPosition_AccessibleLocations(); netPos!=NULL; ppNet->GetNext_AccessibleLocations(netPos))
         {
            CPPAccessibleLocation *pAccLoc = ppNet->GetAt_AccessibleLocations(netPos);
            if (pAccLoc->GetTAInsert()->getEntityNumber() == datalink)
            {
               CPPProbe *pProbe = AddTail_Probes(netname, resource);
               pProbe->SetProbeRefname(insert->getRefname());
               pAccLoc->PlaceProbe(pProbe);
               break;
            }
         }

      }
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();
}

CPPAccessibleLocation* CProbePlacementSolution::getAccessLocationByTargetPriority(CPPNet *ppNet, ETestSurface favored)
{
   // the lower the (non-negative) value the higher the priority
   // the highest priority is 0

   int highestPriority = -1; 
   CPPAccessibleLocation* accLocWithHighestPriority = NULL;

   POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
   while (accPos != NULL)
   {
      CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);
      if ( 
         (accLoc != NULL) &&
         (accLoc->GetFeatureCondition() == featureConditionProbeable) &&
         (accLoc->GetPlaceableProbe() == NULL) &&
         (accLoc->GetSurface() == favored)
         )
      {
         int targetPriority = getTargetTypePriority(accLoc);
         if (targetPriority >= 0)
         {
            if (highestPriority == -1 || highestPriority > targetPriority)
            {
               highestPriority = targetPriority;
               accLocWithHighestPriority = accLoc;
            }
         }
      }

   }

   return accLocWithHighestPriority;
}

CPPAccessibleLocation *CProbePlacementSolution::getAvailableAccessLocationOnSurface(CPPNet *ppNet, ETestSurface favored)
{
   POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
   while (accPos)
   {
      CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

      if (
         (accLoc->GetFeatureCondition() == featureConditionProbeable) &&
         (accLoc->GetPlaceableProbe() == NULL) &&
         (isValidTargetType(accLoc)) &&
         (accLoc->GetSurface() == favored)
         )
      {
         return accLoc;
      }
   }

   return NULL;
}

bool CProbePlacementSolution::anyProbeUsesComponentOutline()
{
   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

   if (m_pTestPlan->GetProbeSide() == testSurfaceTop || m_pTestPlan->GetProbeSide() == testSurfaceBoth)
   {
      POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_TopProbes(templatePos);
         if (pTemplate->GetUseFlag())
         {
            if (pTemplate->GetUseComponentOutline())
            {
               return true;
            }
         }
      }
   }

   if (m_pTestPlan->GetProbeSide() == testSurfaceBottom || m_pTestPlan->GetProbeSide() == testSurfaceBoth)
   {
      POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_BotProbes(templatePos);
         if (pTemplate->GetUseFlag())
         {
            if (pTemplate->GetUseComponentOutline())
            {
               return true;
            }
         }
      }
   }

   return false;
}

void CProbePlacementSolution::removeExcessUnplacedProbes()
{
	// Remove unplaced probes for nets whose total probe count
	// exceeds the resource requirement.
	// E.g. if a net has one placed and one unplaced probe, and resource
	// requirement is one, then the unplaced probe should be eliminated.

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos != NULL)
   {
      CString netName;
      CPPNet* ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      if (ppNet == NULL || ppNet->GetNoProbeFlag())
         continue;

		int resourceNeeded = ppNet->GetTestResourcesNeeded();
		
		// Count resources present
		int resourcePresent = 0;	// placed probes
		int totalPresent = 0;		// all probes
      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos != NULL)
      {
         CPPProbe* probe = ppNet->GetNext_Probes(probePos);
         if (probe != NULL)
			{
				totalPresent++;
				if (probe->GetAccessibleLocation() != NULL)
					resourcePresent++;
			}
		}

		if (totalPresent > resourceNeeded)
		{
			// Remove excess unplaced probes until total resource count is
			// equal to resources needed.
			// If probe is placed, leave it, even if there is an excess.
			// E.g. this might happen in a "retain existing probes" situation,
			// when resouce count was reduced. Keep it anyway.

			bool startover = true;
			while (totalPresent > resourceNeeded && startover)
			{
				startover = false;
				probePos = ppNet->GetHeadPosition_Probes();
				while (probePos != NULL && !startover)
				{
					CPPProbe* probe = ppNet->GetAt_Probes(probePos);
					if (probe != NULL)
					{
						if (!probe->IsPlaced() || probe->GetAccessibleLocation() == NULL)
						{
							totalPresent--;
							ppNet->RemoveAt_Probes(probePos);
							startover = true;
						}
						else
						{
							ppNet->GetNext_Probes(probePos);
						}
					}
				}
			}

		}
	}
}

void CProbePlacementSolution::placeProbes()
{
   POSITION pos = NULL;

   if (anyProbeUsesComponentOutline())
   {
      int startpercent = 0;
      if (m_pProgressDlg != NULL)
         startpercent = atoi(m_pProgressDlg->GetPercent());
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Determining access point/component proximity...");

      resetAccessPointDistances();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetPos(startpercent);
   }

   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Calculating needed probes...");
   calculateNeededProbes();
   if (m_pProgressDlg != NULL)
      m_pProgressDlg->StepIt();

   if (m_pTestPlan->GetUseExistingProbes())
   {
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Gather existing probes...");
      gatherAndMatchProbes();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pTestPlan->GetProbeUse() == probeUsageAddAdditional)
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->SetStatus("Adding additional probes...");
         createAdditionalProbes();
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
      else
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }

      if (!m_pTestPlan->RetainProbePositions() || m_pTestPlan->GetProbeUse() == probeUsageAddAdditional)
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->SetStatus("Placing additional probes...");
         placeProbesOnAccessPoints();
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
      else
      {
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
   }
   else
   {
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Creating probes...");
      createProbes();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Applying preconditions...");
      applyPreconditions(m_pTestPlan->GetPreconditions());
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Placing probes...");
      placeProbesOnAccessPoints();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
   }

   // optimize top probe locations
   if (!m_pTestPlan->GetUseExistingProbes() ||
      (m_pTestPlan->GetUseExistingProbes() && (m_pTestPlan->GetProbeUse() == probeUsageAddAdditional || !m_pTestPlan->RetainProbePositions())))
   {
      if (m_pTestPlan->GetProbeSide() == testSurfaceBoth)
      {
         // use surface priority if using both surfaces
         ETestSurface sidePref = m_pTestPlan->GetProbeSidePreference();
         ETestSurface otherSide = (sidePref==testSurfaceTop) ? testSurfaceBottom : testSurfaceTop;
         CString sidePrefName =   (sidePref == testSurfaceTop) ? "top" : "bottom";
         CString otherSideName = (otherSide == testSurfaceTop) ? "top" : "bottom";
         CString msg;

         msg.Format("Optimizing %s probe locations...", sidePrefName);
         if (m_pProgressDlg != NULL) m_pProgressDlg->SetStatus(msg);
         optimizeProbeLocationsOnSide(sidePref);

         msg.Format("Optimizing %s probe locations...", otherSideName);
         if (m_pProgressDlg != NULL) m_pProgressDlg->SetStatus(msg);
         optimizeProbeLocationsOnSide(otherSide);
      }
      else
      {
         // Only one side is active

         // Optimize top if active
         if (m_pTestPlan->CanProbeTopSide())
         {

            if (m_pProgressDlg != NULL)
               m_pProgressDlg->SetStatus("Optimizing top probe locations...");

            optimizeProbeLocationsOnSide(testSurfaceTop);
         }
         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();

         // Optimize bottom if active
         if (m_pTestPlan->CanProbeBotSide())
         {
            if (m_pProgressDlg != NULL)
               m_pProgressDlg->SetStatus("Optimizing bottom probe locations...");

            optimizeProbeLocationsOnSide(testSurfaceBottom);
         }

         if (m_pProgressDlg != NULL)
            m_pProgressDlg->StepIt();
      }
   }
   else
   {
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
   }


   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetStatus("Maximizing probe sizes...");

   if (m_pTestPlan->GetTesterType() == DFT_TESTER_FIXTURE)
   {
      int loopCount = 0;
		int removedCount = 0;
      int violationCount = 0;
      // Use loopCount to stop infinite loop.
      // Case 2022 - Force loop to happen at least once or we get undetected/unfixed probe violations.
      while (loopCount < 10 && (((removedCount = maximizeProbeSizes()) > 0) || (loopCount < 1)))
      {
         violationCount = fixViolatingProbeTemplates();
         removeTemplatesFromProbes();
         loopCount++;
      }

		// Do the following only if there are still violations,
		// I.e. don't try to fix what ain't broke
		// Cases have shown that doing so can actually make the solution worse (e.g. case 1729)
		if (removedCount > 0 || violationCount > 0)
		{
			loopCount = 0;
			while (loopCount < 10 && fixViolatingProbeTemplates() > 0)
			{
				maximizeProbeSizes();
				loopCount++;
			}
		}

      resurrectNonViolatingUnplacedProbes();

      lastChanceMaximizeProbeSize(testSurfaceTop);
      lastChanceMaximizeProbeSize(testSurfaceBottom);

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();

      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetStatus("Processing double wiring...");
      doubleWireRemainingProbes();
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->StepIt();
   }
   else
   {
      setSmallestProbeSizes();
      if (m_pProgressDlg != NULL)
      {
         // Update the progress bar for maximizing probe sizes and processing double wiring
         m_pProgressDlg->StepIt();
         m_pProgressDlg->StepIt();
      }
   }
}

void CProbePlacementSolution::optimizeProbeLocationsOnSide(ETestSurface surface)
{
   // surface should be testSurfaceTop or testSurfaceBottom.
   // testSurfaceBoth is not appropriate here.

   if ((surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide()) ||
      (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide()))
   {
      bool firstTry = true;

      POSITION pos = NULL;
      if (surface == testSurfaceTop)
         pos = m_pTestPlan->GetProbes().GetTailPosition_UsedTopProbes();
      else
         pos = m_pTestPlan->GetProbes().GetTailPosition_UsedBotProbes();

      while (pos)
      {
         CDFTProbeTemplate *pTemplate = NULL;
         if (surface == testSurfaceTop)
            pTemplate = m_pTestPlan->GetProbes().GetPrev_UsedTopProbes(pos);
         else
            pTemplate = m_pTestPlan->GetProbes().GetPrev_UsedBotProbes(pos);

         if (pTemplate)
         {
            double probeDiameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double minDistToComp = 0;
            if (pTemplate->GetUseComponentOutline())
               minDistToComp = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            while (optimizeProbeLocations(probeDiameter, minDistToComp, firstTry) > 0) ;
            firstTry = false;
         }
      }
   }
}

void CProbePlacementSolution::placeProbesOnAccessPoints()
{
   // place all probes on access points
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos != NULL)
   {
      CString netName;
      CPPNet* ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      if (ppNet == NULL || ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos != NULL)
      {
         CPPProbe* probe = ppNet->GetNext_Probes(probePos);

         // move on if already on an access point
         if (probe == NULL || probe->GetAccessibleLocation() != NULL)
            continue;
         
         CPPAccessibleLocation *accLoc = NULL;
         if (m_pTestPlan->GetProbeSide() == testSurfaceBoth)
         {
            // use surface priority if using both surfaces when initally placing probes (placeProbes)
            ETestSurface sidePref = m_pTestPlan->GetProbeSidePreference();
            ETestSurface otherSide = (sidePref==testSurfaceTop)?testSurfaceBottom:testSurfaceTop;

            if (sidePref == testSurfaceTop && haveTopProbes || sidePref == testSurfaceBottom && haveBotProbes)
               accLoc = getAccessLocationByTargetPriority(ppNet, sidePref);
            if (accLoc == NULL && (otherSide == testSurfaceTop && haveTopProbes || otherSide == testSurfaceBottom && haveBotProbes))
               accLoc = getAccessLocationByTargetPriority(ppNet, otherSide);
         }
         else
         {
            if (m_pTestPlan->GetProbeSide() == testSurfaceTop && haveTopProbes ||
               m_pTestPlan->GetProbeSide() == testSurfaceBottom && haveBotProbes)
               accLoc = getAccessLocationByTargetPriority(ppNet, m_pTestPlan->GetProbeSide());
         }

         // if there are not more accessible locations, then we are done with this net
         if (accLoc != NULL)
         {
            accLoc->PlaceProbe(probe);
#ifdef HELP_SEE_ALL
            // BAD Thing to do
            // This causes a bug in the  probe placement optimization, but it is useful
            // for enabling the viewing of all potential probes in the result. Without
            // this, some probes will not get a template, and will not make it to the
            // final stage. Although it creates a bug, it is useful for some debugging
            // purposes, and that is all.
            if (!probe->HasProbeTemplate())
            {
               CDFTProbeTemplate *probeTemplate = NULL;
               if (accLoc->GetSurface() == testSurfaceTop)
                  probeTemplate = m_pTestPlan->GetProbes().GetTail_TopProbes();
               else
                  probeTemplate = m_pTestPlan->GetProbes().GetTail_BotProbes();
               probe->SetProbeTemplate(probeTemplate);
            }
#endif
         }
      }
   }
}

int CProbePlacementSolution::getProbeDensity(CPoint2d point, double actualdiameter, ETestSurface surface, CAccessibleLocationList &foundList)
{
   int densityCount = 0;

   // Roundoff and direct float comparison is hindering density check.
   // Overlap "on the line" (ie boundary) is okay. Just subtract off
   // some fuzz from the size, to account for "close floats".
   double extentSize = actualdiameter - SMALLNUMBER;

   // It seems odd that this is using diameter rather than radius.
   // For the time being, we'll leave it, presuming it is somehow correct
   // for the algorithm. But it is suspect.
   CExtent extent(-extentSize, -extentSize, extentSize, extentSize);

   extent.translate(point.x, point.y);
   int count = 0;

   if (surface == testSurfaceTop)
      count = m_LocationTreeTop.search(extent, foundList);
   else if (surface == testSurfaceBottom)
      count = m_LocationTreeBot.search(extent, foundList);

   POSITION pos = foundList.GetHeadPosition();
   while (pos)
   {
      POSITION curPos = pos;
      CQfeAccessibleLocation *qfeAccLoc = foundList.GetNext(pos);

      CPPAccessibleLocation *accLoc = qfeAccLoc->GetAccessibleLocation();
      if (accLoc->GetPlaceableProbe() == NULL)
      {
         // No probe here, not an item for further consideration.
         foundList.RemoveAt(curPos);
      }
      else if (!qfeAccLoc->isPointWithinDistance(point, extentSize))
      {
         // Weed out false positive.
         // Rectangular extent is not great for round objects, it gives false positives.
         // But rectangular extents are convenient to use (above) due to facilities available
         // in camcad. If we're here, then item is not really within distance, remove it from 
         // found list.

         foundList.RemoveAt(curPos);
      }
      else
      {
         // It's really there, count it.
         densityCount++;
      }
   }

   return densityCount;
}

int CProbePlacementSolution::getActualProbeDensity(CPoint2d probe1_point, double probe1_diameter, ETestSurface surface, CAccessibleLocationList &foundList)
{
   int densityCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe2 = ppNet->GetNext_Probes(probePos);

         if (probe2 && probe2->IsPlaced())
         {
            CString probe2_templateName = probe2->GetProbeTemplateName();
            
            CPPAccessibleLocation *accLoc = probe2->GetAccessibleLocation();

            if (accLoc && accLoc->GetSurface() == surface &&
               accLoc->GetPlaceableProbe() != NULL)
            {
               CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

               POSITION dummyPos = NULL;
               CDFTProbeTemplate *pTemplate = pTemplates.Find_Probe(probe2->GetProbeTemplateName(), dummyPos, accLoc->GetSurface());

               if (pTemplate)
               {
                  double probe2_diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

                  double sumOfRadii = (probe1_diameter + probe2_diameter) / 2.0;
                  double distance = probe1_point.distance(probe2->GetAccessibleLocation()->GetLocation());

                  // If "on the line" (distance == sumOfRadii) they are  considered
                  // non-overlapping, don't count them. Count only true overlaps.
                  if (distance < sumOfRadii) 
                  {
                     densityCount++;
                     foundList.AddTail(new CQfeAccessibleLocation(accLoc));
                  }
                  
               }
            }
         }
      }
   }

   return densityCount;
}


int CProbePlacementSolution::getTargetTypePriority(CPPAccessibleLocation *accLoc)
{
   int priority = -1;
   int index = 0;
   CDFTTargetPriority &tPriority = m_pTestPlan->GetTargetPriority();

   int strIndex = 0;
   CString targetType = accLoc->GetTargetType();
   CString curTargetName = targetType.Tokenize(", ", strIndex);
   while (!curTargetName.IsEmpty())
   {
      if (accLoc->GetSurface() == testSurfaceTop)
      {
         CDFTTarget* target = tPriority.Find_TopTargets(curTargetName, index);
         if (target && target->GetEnabledFlag() && (index <= priority || priority == -1))
            priority = index;
      }
      else if (accLoc->GetSurface() == testSurfaceBottom)
      {
         CDFTTarget* target = tPriority.Find_BotTargets(curTargetName, index);
         if (target && target->GetEnabledFlag() && (index <= priority || priority == -1))
            priority = index;
      }

      curTargetName = targetType.Tokenize(", ", strIndex);
   }

   return priority;
}

bool CProbePlacementSolution::isValidTargetType(CPPAccessibleLocation *accLoc)
{
   bool foundTargetType = false;

   int index = 0;
   CDFTTarget *target = NULL;
   CDFTTargetPriority &tPriority = m_pTestPlan->GetTargetPriority();

   CString targetType = accLoc->GetTargetType();
   int strIndex = 0;
   CString curTargetName = targetType.Tokenize(", ", strIndex);
   while (!curTargetName.IsEmpty())
   {
      if (accLoc->GetSurface() == testSurfaceTop)
      {
         target = tPriority.Find_TopTargets(curTargetName, index);
         if (target && target->GetEnabledFlag())
            return true;
      }
      else if (accLoc->GetSurface() == testSurfaceBottom)
      {
         target = tPriority.Find_BotTargets(curTargetName, index);
         if (target && target->GetEnabledFlag())
            return true;
      }

      curTargetName = targetType.Tokenize(", ", strIndex);
   }

   return false;
}

bool CProbePlacementSolution::isTooCloseToComponentOutline(CPPAccessibleLocation *accLoc, double minDistToOutline)
{
#ifdef ORIGINAL
   double shortestDistanceToComp = DBL_MAX;
   CPoint2d location = accLoc->GetLocation();
   ETestSurface surface = accLoc->GetSurface();

   POSITION pos = m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &listItem = m_polyList.GetNext(pos);
      
      if (listItem.GetSurface() != surface)
         continue;

      if (listItem.IsPointInside(location))
      {
         shortestDistanceToComp = 0.0;
         break;
      }

      double dist = listItem.DistanceTo(location);
      if (dist < shortestDistanceToComp)
         shortestDistanceToComp = dist;
   }

   return (shortestDistanceToComp < minDistToOutline);
#else
   return (accLoc->GetClosestDistance() < minDistToOutline);
#endif
}

double CProbePlacementSolution::findDistanceToClosestComponentOutline(CPPAccessibleLocation *accLoc)
{
   double shortestDistanceToComp = DBL_MAX;
   CPoint2d location = accLoc->GetLocation();
   ETestSurface surface = accLoc->GetSurface();

   POSITION pos = m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &listItem = m_polyList.GetNext(pos);
      
      if (listItem.GetSurface() != surface)
         continue;

      if (listItem.IsPointInside(location,m_pDoc->getPageUnits()))
      {
         shortestDistanceToComp = 0.0;
         break;
      }

      double dist = listItem.DistanceTo(location);
      if (dist < shortestDistanceToComp)
         shortestDistanceToComp = dist;
   }

   return (shortestDistanceToComp);
}

void CProbePlacementSolution::DrawOutlines()
{
   POSITION pos = m_pDoc->GetFirstViewPosition();
   CCEtoODBView *view = (CCEtoODBView*)m_pDoc->GetNextView(pos);
   CClientDC dc(view);
   //view->OnPrepareDC(&dc);

   CPen topPen(PS_SOLID, 0, RGB(255, 255, 255));
   CPen botPen(PS_SOLID, 0, RGB(255, 0, 0));

   CGdiObject *oldBrush = dc.SelectStockObject(HOLLOW_BRUSH);
   pos = m_polyList.GetHeadPosition();
   while (pos)
   {
      CPolyListItem &listItem = m_polyList.GetNext(pos);

      HGDIOBJ oldPen = dc.SelectObject((listItem.GetSurface()==testSurfaceTop)?topPen:botPen);

      POSITION polyPos = listItem.GetPolyList().GetHeadPosition();
      while (polyPos)
      {
         CPoly *poly = listItem.GetPolyList().GetNext(polyPos);
         POSITION pntPos = poly->getPntList().GetHeadPosition();
         CPnt *pnt = poly->getPntList().GetNext(pntPos);
         dc.MoveTo((int)(pnt->x * view->getScaleFactor()), (int)(pnt->y * view->getScaleFactor()));
         while (pntPos)
         {
            pnt = poly->getPntList().GetNext(pntPos);
            dc.LineTo((int)(pnt->x * view->getScaleFactor()), (int)(pnt->y * view->getScaleFactor()));
         }
      }

      dc.SelectObject(oldPen);
   }
   dc.SelectObject(oldBrush);
}

void CProbePlacementSolution::resetAccessPointDistances()
{
   int netcount =  m_ppNets.GetCount();
   int netindx = 0;

   if (m_pProgressDlg != NULL)
   {
      m_pProgressDlg->SetPos(0);
      m_pProgressDlg->SetRange32((long)0, (long)100);
   }

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {        
      CString netName;
      CPPNet *ppNet = NULL;
      netindx++;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      int percentdone = DcaRound(100.0 * ((double)netindx / (double)netcount));
      if (m_pProgressDlg != NULL)
         m_pProgressDlg->SetPos(min(percentdone + 1, 99));

      if (ppNet->GetNoProbeFlag())
         continue;


      POSITION sourceAccPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (sourceAccPos)
      {
         CPPAccessibleLocation *sourceAccLoc = ppNet->GetNext_AccessibleLocations(sourceAccPos);
         if (m_pTestPlan->GetProbeSide() == testSurfaceBoth || sourceAccLoc->GetSurface() == m_pTestPlan->GetProbeSide())
         {
            sourceAccLoc->SetClosestDistance(findDistanceToClosestComponentOutline(sourceAccLoc));
         }
      }
   }

   if (m_pProgressDlg != NULL)
   {
      m_pProgressDlg->SetRange32((long)0, (long)(1 + 1 + 8));
      m_pProgressDlg->SetPos(1);
   }
}

int CProbePlacementSolution::optimizeProbeLocations(double diameter, double minDistToOutline, bool removeProbeIfNeeded)
{
   int moveCount = 0;
   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION sourceAccPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (sourceAccPos)
      {
         POSITION curSourceAccPos = sourceAccPos;
         CPPAccessibleLocation *sourceAccLoc = ppNet->GetNext_AccessibleLocations(sourceAccPos);

         if (sourceAccLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
            continue;
         if (sourceAccLoc->GetPlaceableProbe() == NULL)
            continue;
         if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbePositions() && sourceAccLoc->GetPlaceableProbe()->DidExist())
            continue;
         if (sourceAccLoc->GetFeatureCondition() == featureConditionForced)
            continue;

         bool sourceTooCloseToComponent = minDistToOutline > 0.0 && isTooCloseToComponentOutline(sourceAccLoc, minDistToOutline);

         CAccessibleLocationList foundList;
         int sourceDensity = getProbeDensity(sourceAccLoc->GetLocation(), diameter, sourceAccLoc->GetSurface(), foundList);

         POSITION destAccPos = ppNet->GetHeadPosition_AccessibleLocations();
         while (destAccPos)
         {
            if (curSourceAccPos == destAccPos)
            {
               ppNet->GetNext_AccessibleLocations(destAccPos);
               continue;
            }

            CPPAccessibleLocation *destAccLoc = ppNet->GetNext_AccessibleLocations(destAccPos);

            if (destAccLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
               continue;
            if (destAccLoc->GetSurface() == testSurfaceTop && !haveTopProbes ||
               destAccLoc->GetSurface() == testSurfaceBottom && !haveBotProbes)
               continue;
            if (destAccLoc->GetPlaceableProbe() != NULL)
               continue;
            if (minDistToOutline > 0.0 && isTooCloseToComponentOutline(destAccLoc, minDistToOutline))
               continue;
            if (!isValidTargetType(destAccLoc))
               continue;
            if (destAccLoc->GetFeatureCondition() != featureConditionProbeable)
               continue;
            
            // Side preference takes precedence over probe size, over everything.
            // But only when doing probes on "both" sides.
            // If source probe is on preferred side and dest probe is not, do not
            // move the probe, unless the source probe is too close to a component.
            if (m_pTestPlan->GetProbeSide() == testSurfaceBoth)
            {
               ETestSurface sidePref = m_pTestPlan->GetProbeSidePreference();
               if (!sourceTooCloseToComponent &&
                  sourceAccLoc->GetSurface() == sidePref &&
                  destAccLoc->GetSurface() != sidePref)
                  continue;
            }

            // dts0100552854 - Disable density optimization for Fixtureless probes.
            if (m_pTestPlan->GetTesterType() != DFT_TESTER_FIXTURELESS)
            {
               CAccessibleLocationList foundList;
               int destDensity = getProbeDensity(destAccLoc->GetLocation(), diameter, destAccLoc->GetSurface(), foundList);

               if (sourceDensity - 1 > destDensity)
               {
                  // move the probe
                  CPPProbe *probe = sourceAccLoc->RemoveProbe();
                  destAccLoc->PlaceProbe(probe);

                  moveCount++;
                  break;
               }
            }
         }

         // remove the probe if we have to and if 
         if (sourceTooCloseToComponent && sourceAccLoc->GetPlaceableProbe() != NULL && removeProbeIfNeeded)
         {
            sourceAccLoc->RemoveProbe();
            moveCount++;
         }
      }
   }

   return moveCount;
}

CDFTProbeTemplate *CProbePlacementSolution::getOptimalProbeSize(CPPAccessibleLocation *accLoc)
{
   ETestSurface surface = accLoc->GetSurface();
   POSITION pTempPos = NULL;

   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();
   if (surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide())
   {
      POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_TopProbes(templatePos);
         if (!pTemplate->GetUseFlag())
            continue;

         double probeSize = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         if (pTemplate->GetUseComponentOutline())
         {
            double minCompDist = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (isTooCloseToComponentOutline(accLoc, minCompDist))
               continue;
         }

         // return this as the current probe template
         return pTemplate;
      }
   }
   else if (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide())
   {
      POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
      while (templatePos)
      {
         CDFTProbeTemplate *pTemplate = pTemplates.GetNext_BotProbes(templatePos);
         if (!pTemplate->GetUseFlag())
            continue;

         double probeSize = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         if (pTemplate->GetUseComponentOutline())
         {
            double minCompDist = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (isTooCloseToComponentOutline(accLoc, minCompDist))
               continue;
         }

         // return this as the current probe template
         return pTemplate;
      }
   }

   return NULL;
}

void CProbePlacementSolution::getOptimalProbeCombination(CPPAccessibleLocation *accLoc1, CPPAccessibleLocation *accLoc2, CDFTProbeTemplate *&template1, CDFTProbeTemplate *&template2)
{
   ETestSurface surface = accLoc1->GetSurface();
   POSITION pTempPos = NULL;

	// In case 1729 data, we get a better result if we always let this function select
	// both templates, but doing so essentially disables a lot of logic in the
	// "maximize" call. This needs more study.
   // (Later...) In case 2029 we also get better result (the required result to close the
   // case) if we let this function pick both templates all the time.
   // It is not clear if this harms anything. Results seem to look good. Time will tell.
	template1 = template2 = NULL;

   if (template1 != NULL && template2 != NULL)
      return;

   // NOTE: possible optimization to precalculate the cost of every combinations of probes
   int curTotalCost = 0;

   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();
   if (surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide())
   {
      if (template1 == NULL && template2 == NULL)
      {
         POSITION templatePos1 = pTemplates.GetHeadPosition_TopProbes();
         while (templatePos1)
         {
            POSITION curTemplatePos = templatePos1;
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_TopProbes(templatePos1);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            POSITION templatePos2 = curTemplatePos;
            while (templatePos2)
            {
               CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_TopProbes(templatePos2);
               if (!probeTemplate2->GetUseFlag())
                  continue;

               double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (probeTemplate2->GetUseComponentOutline())
               {
                  double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                  if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                     continue;
               }

               int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
               double minDistance = (probeSize1 + probeSize2) / 2;

               // set this as the current if the current cost is less than the last set
               if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
                  (template1 == NULL || template2 == NULL  || curTotalCost > cost))
               {
                  template1 = probeTemplate1;
                  template2 = probeTemplate2;
                  curTotalCost = cost;
               }
            }
         }
      }
      else if (template1 == NULL && template2 != NULL)
      {
         CDFTProbeTemplate *probeTemplate2 = template2;

         POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_TopProbes(templatePos);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate2->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template1 == NULL || curTotalCost > cost))
            {
               template1 = probeTemplate1;
               curTotalCost = cost;
            }
         }
      }
      else if (template1 != NULL && template2 == NULL)
      {
         CDFTProbeTemplate *probeTemplate1 = template1;

         POSITION templatePos = pTemplates.GetHeadPosition_TopProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_TopProbes(templatePos);
            if (!probeTemplate2->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template2 == NULL || curTotalCost > cost))
            {
               template2 = probeTemplate2;
               curTotalCost = cost;
            }
         }
      }
   }
   else if (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide())
   {
      if (template1 == NULL && template2 == NULL)
      {
         POSITION templatePos1 = pTemplates.GetHeadPosition_BotProbes();
         while (templatePos1)
         {
            POSITION curTemplatePos = templatePos1;
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_BotProbes(templatePos1);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            POSITION templatePos2 = curTemplatePos;
            while (templatePos2)
            {
               CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_BotProbes(templatePos2);
               if (!probeTemplate2->GetUseFlag())
                  continue;

               double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (probeTemplate2->GetUseComponentOutline())
               {
                  double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                  if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                     continue;
               }

               int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
               double minDistance = (probeSize1 + probeSize2) / 2;

               // set this as the current if the current cost is less than the last set
               if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
                  (template1 == NULL || template2 == NULL || curTotalCost > cost))
               {
                  template1 = probeTemplate1;
                  template2 = probeTemplate2;
                  curTotalCost = cost;
               }
            }
         }
      }
      else if (template1 == NULL && template2 != NULL)
      {
         CDFTProbeTemplate *probeTemplate2 = template2;

         POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate1 = pTemplates.GetNext_BotProbes(templatePos);
            if (!probeTemplate1->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate2->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate2->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc2, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template1 == NULL || curTotalCost > cost))
            {
               template1 = probeTemplate1;
               curTotalCost = cost;
            }
         }
      }
      else if (template1 != NULL && template2 == NULL)
      {
         CDFTProbeTemplate *probeTemplate1 = template1;

         POSITION templatePos = pTemplates.GetHeadPosition_BotProbes();
         while (templatePos)
         {
            CDFTProbeTemplate *probeTemplate2 = pTemplates.GetNext_BotProbes(templatePos);
            if (!probeTemplate2->GetUseFlag())
               continue;

            double probeSize1 = probeTemplate1->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            double probeSize2 = probeTemplate2->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
            if (probeTemplate1->GetUseComponentOutline())
            {
               double minCompDist = probeTemplate1->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if (isTooCloseToComponentOutline(accLoc1, minCompDist))
                  continue;
            }

            int cost = probeTemplate1->GetCost() + probeTemplate2->GetCost();
            double minDistance = (probeSize1 + probeSize2) / 2;

            // set this as the current if the current cost is less than the last set
            if (minDistance < accLoc1->GetLocation().distance(accLoc2->GetLocation()) &&
               (template2 == NULL || curTotalCost > cost))
            {
               template2 = probeTemplate2;
               curTotalCost = cost;
            }
         }
      }
   }
}

int CProbePlacementSolution::resurrectNonViolatingUnplacedProbes()
{
   int resurrectionCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);

         if (probe && !probe->IsPlaced())
         {
            bool beenPlaced = false;

            POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
            while (accPos && !beenPlaced)
            {
               CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

               if (accLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
                  continue;

               if (accLoc->GetPlaceableProbe() == NULL &&
                  accLoc->GetFeatureCondition() != featureConditionNoProbe)
               {

                  POSITION dummyPos = NULL;
                  CDFTProbeTemplate *pTemplate = NULL;

                  // Try the templates from largest to smallest

                  ETestSurface surface = accLoc->GetSurface();
                  CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

                  POSITION templatePos = NULL;

                  if (surface == testSurfaceTop && m_pTestPlan->CanProbeTopSide())
                     templatePos = pTemplates.GetHeadPosition_TopProbes();
                  else if (surface == testSurfaceBottom && m_pTestPlan->CanProbeBotSide())
                     templatePos = pTemplates.GetHeadPosition_BotProbes();

#ifdef TRY_LARGEST_TO_SMALLEST
                  while (templatePos && !beenPlaced)
#endif
                  {
                     CDFTProbeTemplate *pTemplate = NULL;

#ifdef TRY_LARGEST_TO_SMALLEST
                     if (surface == testSurfaceTop)
                        pTemplate = pTemplates.GetNext_TopProbes(templatePos);
                     else if (surface == testSurfaceBottom)
                        pTemplate = pTemplates.GetNext_BotProbes(templatePos);
#else
                     // just try smallest
                     if (surface == testSurfaceTop)
                        pTemplate = pTemplates.GetSmallest_UsedTopProbes(templatePos);
                     else if (surface == testSurfaceBottom)
                        pTemplate = pTemplates.GetSmallest_UsedBotProbes(templatePos);
                     
#endif


                     if (pTemplate && pTemplate->GetUseFlag())
                     {
                        double probeDiameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

                        bool componentViolation = false;

                        if (pTemplate->GetUseComponentOutline())
                        {
                           double minDistToOutline = pTemplate->GetComponentOutlineDistance() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                           componentViolation = isTooCloseToComponentOutline(accLoc, minDistToOutline);
                        }

                        if (!componentViolation)
                        {           
                           CAccessibleLocationList foundList;
                           int probeDensity = getActualProbeDensity(accLoc->GetLocation(), probeDiameter, accLoc->GetSurface(), foundList);

                           if (probeDensity > 0)
                           {
                              // Minimize size of all violators
                              // (Last chance maximize will bring them back up if possible.)
                              POSITION foundpos = foundList.GetHeadPosition();
                              while (foundpos)
                              {
                                 CQfeAccessibleLocation *qfeAccLoc = foundList.GetNext(foundpos);

                                 CPPAccessibleLocation *neighborAccLoc = qfeAccLoc->GetAccessibleLocation();
                                 if (neighborAccLoc->GetPlaceableProbe() != NULL)
                                 {
                                    CPPProbe *nprobe = neighborAccLoc->GetPlaceableProbe();
                                    nprobe->SetProbeTemplate(pTemplate);
                                 }
                              }
                           }

                           // Now try again
                           probeDensity = getActualProbeDensity(accLoc->GetLocation(), probeDiameter, accLoc->GetSurface(), foundList);

                           if (probeDensity == 0)
                           {
                              accLoc->PlaceProbe(probe);
                              probe->SetProbeTemplate(pTemplate);
                              beenPlaced = true;
                              resurrectionCount++;
                           }

                        }

                     }
                  }
               }
            }
         }
      }
   }
   return resurrectionCount;
}

int CProbePlacementSolution::lastChanceMaximizeProbeSize(ETestSurface surface)
{
   // We want to try to evenly grow the probe sizes. That's why the
   // probe template loop is on the outside, rather than the inside.
   // It it is better to grow two probes to 75 than to grow one to
   // 100 leaving the other at 50.

   int changeCount = 0;

   CDFTProbeTemplates &pTemplates = m_pTestPlan->GetProbes();

   POSITION templatePos = NULL;

   // The check for side enabled in test plan is removed because Forced Probes may be
   // on a side that is "not enabled", but we still want to maximize them.
   // All we lose is a little time looping just to find there are no probes on a given side.
   // See test case dts0100380281.
   if (surface == testSurfaceTop) //  && m_pTestPlan->CanProbeTopSide())
      templatePos = pTemplates.GetTailPosition_TopProbes();
   else if (surface == testSurfaceBottom ) //  && m_pTestPlan->CanProbeBotSide())
      templatePos = pTemplates.GetTailPosition_BotProbes();


   while (templatePos)
   {
      CDFTProbeTemplate *pTemplate = NULL;


      if (surface == testSurfaceTop)
         pTemplate = pTemplates.GetPrev_TopProbes(templatePos);
      else if (surface == testSurfaceBottom)
         pTemplate = pTemplates.GetPrev_BotProbes(templatePos);

      if (pTemplate && pTemplate->GetUseFlag())
      {

         POSITION netPos = m_ppNets.GetStartPosition();
         while (netPos)
         {
            CString netName;
            CPPNet *ppNet = NULL;
            m_ppNets.GetNextAssoc(netPos, netName, ppNet);

            if (ppNet->GetNoProbeFlag())
               continue;

            POSITION probePos = ppNet->GetHeadPosition_Probes();
            while (probePos)
            {
               CPPProbe *probe = ppNet->GetNext_Probes(probePos);

               if (probe && probe->IsPlaced())
               {
                  CString probe_templateName = probe->GetProbeTemplateName();

                  CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

                  if (accLoc && accLoc->GetSurface() == surface &&
                     accLoc->GetPlaceableProbe() != NULL)
                  {
                     POSITION dummyPos = NULL;
                     CDFTProbeTemplate *pExistingTemplate = pTemplates.Find_Probe(probe->GetProbeTemplateName(), dummyPos, accLoc->GetSurface());

							double existing_probe_diameter = 0.0;
                     if (pExistingTemplate)
                        existing_probe_diameter = pExistingTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                        
							double new_probe_diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
                        
                     if (new_probe_diameter > existing_probe_diameter)
                     {
                        CAccessibleLocationList dummyList;
                        int density = getActualProbeDensity(accLoc->GetLocation(), new_probe_diameter, surface, dummyList);
                        if (density <= 1) {
                           probe->SetProbeTemplate(pTemplate);
                           changeCount++;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return changeCount;
}



int CProbePlacementSolution::maximizeProbeSizes()
{
   int removeCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (accPos)
      {
         CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

         if (accLoc->GetSurface() != m_pTestPlan->GetProbeSide() && m_pTestPlan->GetProbeSide() != testSurfaceBoth)
            continue;
         if (accLoc->GetPlaceableProbe() == NULL)
            continue;
         if (m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist())
            continue;

         POSITION dummyPos = NULL;
         CDFTProbeTemplate *largestProbeTemplate = NULL;
         if (accLoc->GetSurface() == testSurfaceTop)
            largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedTopProbes(dummyPos);
         else if (accLoc->GetSurface() == testSurfaceBottom)
            largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedBotProbes(dummyPos);

         double testDiameter = largestProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

         CAccessibleLocationList foundList;
         int probeDensity = getProbeDensity(accLoc->GetLocation(), testDiameter, accLoc->GetSurface(), foundList);

         // if there is nothing in the way
         if (probeDensity <= 1)
         {
            CDFTProbeTemplate *pTemplate = getOptimalProbeSize(accLoc);
            CPPProbe *probe = accLoc->GetPlaceableProbe();
            probe->SetProbeTemplate(pTemplate);
            continue;
         }

         //if (accLoc->GetPlaceableProbe()->HasProbeTemplate())
         // continue;

         POSITION tempPos;
         CDFTProbeTemplate *accTemplate = NULL;
         if (accLoc->GetPlaceableProbe()->HasProbeTemplate())
            accTemplate = m_pTestPlan->GetProbes().Find_Probe(accLoc->GetPlaceableProbe()->GetProbeTemplateName(), tempPos, accLoc->GetSurface());

         // of those found, look for the closest probe
         CPPAccessibleLocation *closestAccLoc = NULL;
         CDFTProbeTemplate *closestAccTemplate = NULL;
         double closestDistance = DBL_MAX;
         POSITION foundPos = foundList.GetHeadPosition();
         while (foundPos)
         {
            CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);
            CPPAccessibleLocation *qfeAccLoc = qfeLoc->GetAccessibleLocation();

            if (qfeAccLoc->GetPlaceableProbe() == NULL)
               continue;

            if (accLoc == qfeAccLoc)
               continue;

            double distance = accLoc->GetLocation().distance(qfeAccLoc->GetLocation());
            if (distance < closestDistance)
            {
               closestAccLoc = qfeAccLoc;
               closestDistance = distance;

               if (closestAccLoc->GetPlaceableProbe()->HasProbeTemplate())
                  closestAccTemplate = m_pTestPlan->GetProbes().Find_Probe(closestAccLoc->GetPlaceableProbe()->GetProbeTemplateName(), tempPos, accLoc->GetSurface());
               else
                  closestAccTemplate = NULL;
            }
            else if (distance - closestDistance < SMALLNUMBER && qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate())
            {
               CDFTProbeTemplate *qfeAccTemplate = NULL;
               qfeAccTemplate = m_pTestPlan->GetProbes().Find_Probe(qfeAccLoc->GetPlaceableProbe()->GetProbeTemplateName(), tempPos, accLoc->GetSurface());

               // if the distances are basically the same, favor the access point that has the largest probe template assigned
               if (closestAccTemplate != NULL && qfeAccTemplate != NULL && closestAccTemplate->GetDiameter() < qfeAccTemplate->GetDiameter())
               {
                  closestAccLoc = qfeAccLoc;
                  closestDistance = distance;
                  closestAccTemplate = qfeAccTemplate;
               }
               else if (qfeAccTemplate != NULL)
               {
                  closestAccLoc = qfeAccLoc;
                  closestDistance = distance;
                  closestAccTemplate = qfeAccTemplate;
               }
            }
         }

         if (closestAccLoc != NULL)
         {
            CDFTProbeTemplate *temp1 = accTemplate, *temp2 = closestAccTemplate;

            // if there are probe templates on both of them, make sure they don't violate each other
            if (temp1 != NULL && temp2 != NULL)
            {
               // if there is a violation, remove the template off the current probe
               double diameter1 = temp1->GetDiameter()* Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()),
                     diameter2 = temp2->GetDiameter()* Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
               if ((diameter1+diameter2)/2 > closestDistance)
                  temp1 = NULL;
            }

            // TODO: Locking - Fix maximizing of probe sized to work with Retaining Probe Sizes
            getOptimalProbeCombination(accLoc, closestAccLoc, temp1, temp2);
            if ((m_pTestPlan->RetainProbeSizes() && !accLoc->GetPlaceableProbe()->DidExist()) || !m_pTestPlan->RetainProbeSizes())
            {
               if (accLoc->GetPlaceableProbe()->HasProbeTemplate() && temp1 != NULL)
               {
                  if (temp1->GetDiameter() < accTemplate->GetDiameter())
                     accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
               }
               else
                  accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
            }

            if ((m_pTestPlan->RetainProbeSizes() && !closestAccLoc->GetPlaceableProbe()->DidExist()) || !m_pTestPlan->RetainProbeSizes())
            {
               if (closestAccLoc->GetPlaceableProbe()->HasProbeTemplate() && temp2 != NULL)
               {
                  if (temp2->GetDiameter() < closestAccTemplate->GetDiameter())
                     closestAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
               }
               else
                  closestAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
            }

            // if no available probe combination is possible, we may need to remove one so another may fit
            // remove the one with the highest density, or if equal, the current probe
            if (temp1 == NULL && temp2 == NULL)
            {
               // start with the density of the current one
               CPPAccessibleLocation *highestDensityAccLoc = NULL;
               int highestDensity = 0;

               foundPos = foundList.GetHeadPosition();
               while (foundPos)
               {
                  CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);
                  CPPAccessibleLocation *qfeAccLoc = qfeLoc->GetAccessibleLocation();

                  if (qfeAccLoc->GetPlaceableProbe() == NULL)
                     continue;

                  if (accLoc == qfeAccLoc)
                     continue;
   
                  CAccessibleLocationList tempFoundList;
                  int density = getProbeDensity(qfeAccLoc->GetLocation(), testDiameter, qfeAccLoc->GetSurface(), tempFoundList);
                  
                  if (density > highestDensity && !(m_pTestPlan->RetainProbeSizes() && qfeAccLoc->GetPlaceableProbe()->DidExist()))
                  {
                     highestDensityAccLoc = qfeAccLoc;
                     highestDensity = density;
                  }
               } // END while (foundPos) ...

               //  Remove the probe that has the highest density.  If the density of the current probe is the highest, even if
               // equal to any other, then it is deleted.  Otherwise delete the highest density probe.
               if (highestDensity > probeDensity)
                  highestDensityAccLoc->RemoveProbe();
               else
                  accLoc->RemoveProbe();

               removeCount++;
            } // END if (temp1 == NULL && temp2 == NULL) ...
            // if one of the probe combination has no valid template and the other is an original probe (not for fixture),
            // we need to remove that probe
            else if ((temp1 == NULL && m_pTestPlan->RetainProbeSizes() && closestAccLoc->GetPlaceableProbe()->DidExist()) ||
                     (temp2 == NULL && m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist()))
            {
               if (temp1 == NULL)
                  accLoc->RemoveProbe();
               else if (temp2 == NULL)
                  closestAccLoc->RemoveProbe();
            } // END if ((temp1 == NULL && !closestAccLoc->IsForFixtureFile()) || ...
         }
      } // END while (accPos) ...
   } // END while (netPos) ...

   return removeCount;
}

int CProbePlacementSolution::fixViolatingProbeTemplates()
{
   int violationCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();
         POSITION dummyPos = NULL;

         if (accLoc == NULL)
            continue;

         if (probe->HasProbeTemplate())
         {
            POSITION ptPos = NULL;
            CDFTProbeTemplate *largestProbeTemplate = NULL;
            if (accLoc->GetSurface() == testSurfaceTop)
               largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedTopProbes(dummyPos);
            else if (accLoc->GetSurface() == testSurfaceBottom)
               largestProbeTemplate = m_pTestPlan->GetProbes().GetLargest_UsedBotProbes(dummyPos);
            double testDiameter = largestProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());

            CAccessibleLocationList foundList;
            int probeDensity = getProbeDensity(accLoc->GetLocation(), testDiameter, accLoc->GetSurface(), foundList);

            // if there is nothing in the way
            if (probeDensity <= 1)
               continue;
            CDFTProbeTemplate *probeTemplate = m_pTestPlan->GetProbes().Find_Probe(probe->GetProbeTemplateName(), dummyPos, accLoc->GetSurface());

            // see if this access location violates any around it
            POSITION foundPos = foundList.GetHeadPosition();
            while (foundPos && probeTemplate)
            {
               CQfeAccessibleLocation *qfeLoc = foundList.GetNext(foundPos);
               CPPAccessibleLocation *qfeAccLoc = qfeLoc->GetAccessibleLocation();

               if (qfeAccLoc->GetPlaceableProbe() == NULL)
                  continue;

               if (!qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate())
                  continue;

               if (accLoc == qfeAccLoc)
                  continue;

               POSITION dummyPos2 = NULL;
               CDFTProbeTemplate *qfeProbeTemplate = m_pTestPlan->GetProbes().Find_Probe(qfeAccLoc->GetPlaceableProbe()->GetProbeTemplateName(), dummyPos2, accLoc->GetSurface());

               double distance = accLoc->GetLocation().distance(qfeAccLoc->GetLocation());
               double probeRadius = probeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()) / 2.0;
               double qfeRadius = qfeProbeTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits()) / 2.0;

               if (distance < (probeRadius + qfeRadius))
               {
                  CDFTProbeTemplate *temp1 = (m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist())?probeTemplate:NULL,
                                    *temp2 = (m_pTestPlan->RetainProbeSizes() && qfeAccLoc->GetPlaceableProbe()->DidExist())?qfeProbeTemplate:NULL;

                  getOptimalProbeCombination(accLoc, qfeAccLoc, temp1, temp2);
                  if (accLoc->GetPlaceableProbe()->HasProbeTemplate() && temp1 != NULL)
                  {
                     if (temp1->GetDiameter() < probeTemplate->GetDiameter())
                     {
                        accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
                        probeTemplate = temp1;
                     }
                  }
                  else
                  {
                     accLoc->GetPlaceableProbe()->SetProbeTemplate(temp1);
                     probeTemplate = temp1;
                  }

                  if (qfeAccLoc->GetPlaceableProbe()->HasProbeTemplate() && temp2 != NULL)
                  {
                     if (temp2->GetDiameter() < qfeProbeTemplate->GetDiameter())
                        qfeAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);
                  }
                  else
                     qfeAccLoc->GetPlaceableProbe()->SetProbeTemplate(temp2);

                  violationCount++;
               } // END if (distance < (probeRadius + qfeRadius)) ...
            } // END while (foundPos) ...

            if (probeTemplate == NULL && !(m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist()))
            {
               accLoc->RemoveProbe();
               break;
            }
         } // END if (probe->HasProbeTemplate()) ...
         else
         {
            accLoc->RemoveProbe();
         }
      } // END while (probePos) ...
   } // END while (netPos) ...

   return violationCount;
}

int CProbePlacementSolution::removeTemplatesFromProbes()
{
   // Leave templates on forced probes, remove the rest.

   int removeCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc != NULL && accLoc->GetFeatureCondition() != featureConditionForced && probe->HasProbeTemplate() &&
            !(m_pTestPlan->RetainProbeSizes() && accLoc->GetPlaceableProbe()->DidExist()) )
         {
            probe->SetProbeTemplate(NULL);
            removeCount++;
         }
      }
   }

   return removeCount;
}

int CProbePlacementSolution::removeProbesWithNoTemplate()
{
   int removeCount = 0;

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc != NULL && !probe->HasProbeTemplate())
         {
            accLoc->RemoveProbe();
            removeCount++;
         }
      }
   }

   return removeCount;
}

void CProbePlacementSolution::setSmallestProbeSizes()
{
   POSITION pos = NULL;
   CDFTProbeTemplate *pTemplateTop = m_pTestPlan->GetProbes().GetTail_TopProbes();
   CDFTProbeTemplate *pTemplateBot = m_pTestPlan->GetProbes().GetTail_BotProbes();

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc != NULL)
         {
            if (accLoc->GetSurface() == testSurfaceTop || accLoc->GetSurface() == testSurfaceBoth)
               probe->SetProbeTemplate(pTemplateTop);
            else if (accLoc->GetSurface() == testSurfaceBottom)
               probe->SetProbeTemplate(pTemplateBot);
         }
      }
   }
}

void CProbePlacementSolution::doubleWireRemainingProbes()
{
   if (!m_pTestPlan->GetAllowDoubleWiring())
      return;

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      if (ppNet->GetNoProbeFlag())
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);

         if (probe->GetAccessibleLocation() != NULL)
            continue;

         POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
         while (accPos)
         {
            CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);
            CPPProbe *placedProbe = accLoc->GetPlaceableProbe();

            if (placedProbe == NULL)
               continue;

            if (!accLoc->IsDoubleWired())
            {
               POSITION pTempPos = NULL;
               accLoc->PlaceDoubleWiredProbe(probe);
               probe->SetProbeTemplate(m_pTestPlan->GetProbes().Find_Probe(placedProbe->GetProbeTemplateName(), pTempPos, accLoc->GetSurface()));
               break;
            }
         }
      }
   } // END while (pos) ...
}

CProbePlacementSolution& CProbePlacementSolution::operator=(const CProbePlacementSolution &ppSolution)
{
   if (&ppSolution != this)
   {
      m_pDoc = ppSolution.m_pDoc;
      m_pTestPlan = ppSolution.m_pTestPlan;
      m_pFile = ppSolution.m_pFile;
      
      RemoveAll_Nets();
      POSITION pos = ppSolution.m_ppNets.GetStartPosition();
      while (pos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         ppSolution.m_ppNets.GetNextAssoc(pos, netName, ppNet);
         
         m_ppNets.SetAt(netName, new CPPNet(*ppNet));
      }
      
      m_polyList.RemoveAll();
      pos = ppSolution.m_polyList.GetHeadPosition();
      while (pos)
      {
         m_polyList.AddTail(ppSolution.m_polyList.GetNext(pos));
      }

      // TODO: copy net map (???)
      // TODO: copy Qfe extents lookup (???)
   }

   return *this;
}

POSITION CProbePlacementSolution::GetHeadPosition_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHeadPosition_Probes();
}

POSITION CProbePlacementSolution::GetTailPosition_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTailPosition_Probes();
}

CPPProbe *CProbePlacementSolution::GetHead_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHead_Probes();
}

CPPProbe *CProbePlacementSolution::GetTail_Probes(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTail_Probes();
}

CPPProbe *CProbePlacementSolution::GetNext_Probes(CString netName, POSITION &pos) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetNext_Probes(pos);
}

CPPProbe *CProbePlacementSolution::GetPrev_Probes(CString netName, POSITION &pos) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetPrev_Probes(pos);
}

POSITION CProbePlacementSolution::GetHeadPosition_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHeadPosition_AccessibleLocations();
}

POSITION CProbePlacementSolution::GetTailPosition_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTailPosition_AccessibleLocations();
}

CPPAccessibleLocation *CProbePlacementSolution::GetHead_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetHead_AccessibleLocations();
}

CPPAccessibleLocation *CProbePlacementSolution::GetTail_AccessibleLocations(CString netName) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetTail_AccessibleLocations();
}

CPPAccessibleLocation *CProbePlacementSolution::GetPrev_AccessibleLocations(CString netName, POSITION &pos) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;
   
   return ppNet->GetPrev_AccessibleLocations(pos);
}

CPPAccessibleLocation *CProbePlacementSolution::GetNext_AccessibleLocations(CString netName, POSITION &pos, EEntityType featureType, CString targetType, ETestSurface surface) const
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
      return NULL;

   CPPAccessibleLocation *pAccLoc = ppNet->GetNext_AccessibleLocations(pos);

   while (pos)
   {
      CPPAccessibleLocation *pNextAccLoc = ppNet->GetAt_AccessibleLocations(pos);

      if ((featureType < entityTypeMIN || pNextAccLoc->getFeatureEntity().getEntityType() == featureType) &&
         (targetType.IsEmpty() || pNextAccLoc->GetTargetType().Find(targetType) > 0) &&
         (netName.IsEmpty() || pNextAccLoc->GetNetName() == netName) &&
         (surface < testSurfaceMIN || pNextAccLoc->GetSurface() == surface))
         break;

      ppNet->GetNext_AccessibleLocations(pos);
   }

   return pAccLoc;
}

CPPProbe *CProbePlacementSolution::AddHead_Probes(CString netName, ETestResourceType resType, CString probeRefname)
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      ppNet = new CPPNet(m_pDoc, netName);
      m_ppNets.SetAt(netName, ppNet);
   }
   
   if (probeRefname.IsEmpty())
      probeRefname = m_probeNamer.GetNextProbeRefname(ppNet, resType);

   return ppNet->AddHead_Probes(resType, netName, probeRefname);
}

CPPProbe *CProbePlacementSolution::AddTail_Probes(CString netName, ETestResourceType resType, CString probeRefname)
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      ppNet = new CPPNet(m_pDoc, netName);
      m_ppNets.SetAt(netName, ppNet);
   }
   
   if (probeRefname.IsEmpty())
      probeRefname = m_probeNamer.GetNextProbeRefname(ppNet, resType);

   return ppNet->AddTail_Probes(resType, netName, probeRefname);
}

void CProbePlacementSolution::RemoveAll_Probes(CString netName)
{
   if (netName.IsEmpty())
   {
      // remove all probes
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         ppNet->RemoveAll_Probes();
         //if (ppNet->GetCount_AccessibleLocations() == 0)
         //{
         // delete ppNet;
         // m_ppNets.RemoveKey(netName);
         //}
      }
   }
   else
   {
      // remove all probes under a net
      CPPNet *ppNet = NULL;
      if (!m_ppNets.Lookup(netName, ppNet))
         return;

      ppNet->RemoveAll_Probes();
      //if (ppNet->GetCount_AccessibleLocations() == 0)
      //{
      // delete ppNet;
      // m_ppNets.RemoveKey(netName);
      //}
   }
}

CPPAccessibleLocation *CProbePlacementSolution::AddHead_AccessibleLocations(DataStruct *testAccessData)
{
   // get the net name
   CString netName;
   Attrib *attrib = NULL;
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   if (!testAccessData->getAttributes()->Lookup(netNameKW, attrib))
      return NULL;
   netName = get_attvalue_string(m_pDoc, attrib);

   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      ppNet = new CPPNet(m_pDoc, netName);
      m_ppNets.SetAt(netName, ppNet);
   }

   CPPAccessibleLocation *accessibleLocation = ppNet->AddHead_AccessibleLocations(testAccessData);

   if (accessibleLocation->GetSurface() == testSurfaceTop)
      m_LocationTreeTop.setAt(new CQfeAccessibleLocation(accessibleLocation));
   else if (accessibleLocation->GetSurface() == testSurfaceBottom)
      m_LocationTreeBot.setAt(new CQfeAccessibleLocation(accessibleLocation));

   return accessibleLocation;
}

CPPAccessibleLocation *CProbePlacementSolution::AddTail_AccessibleLocations(DataStruct *testAccessData)
{
   // get the net name
   CString netName;
   Attrib *attrib = NULL;
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   if (!testAccessData->getAttributes()->Lookup(netNameKW, attrib))
      return NULL;
   netName = get_attvalue_string(m_pDoc, attrib);

   //// make sure the net exists in the netlist
   //if (!FindNet(m_pFile, netName))
   // return NULL;

   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      return NULL;
      //ppNet = new CPPNet(m_pDoc, netName);
      //m_ppNets.SetAt(netName, ppNet);
   }

   CPPAccessibleLocation *accessibleLocation = ppNet->AddTail_AccessibleLocations(testAccessData);

   if (accessibleLocation->GetSurface() == testSurfaceTop)
      m_LocationTreeTop.setAt(new CQfeAccessibleLocation(accessibleLocation));
   else if (accessibleLocation->GetSurface() == testSurfaceBottom)
      m_LocationTreeBot.setAt(new CQfeAccessibleLocation(accessibleLocation));

   return accessibleLocation;
}

CPPAccessibleLocation *CProbePlacementSolution::AddTail_AccessibleLocations(CString netName,const CEntity& featureEntity, CPoint2d location, CString targetType, ETestSurface surface)
{
   CPPNet *ppNet = NULL;
   if (!m_ppNets.Lookup(netName, ppNet))
   {
      return NULL;
      //ppNet = new CPPNet(m_pDoc, netName);
      //m_ppNets.SetAt(netName, ppNet);
   }

   CPPAccessibleLocation *accessibleLocation = ppNet->AddTail_AccessibleLocations(featureEntity, location, targetType, netName, surface);

   if (accessibleLocation->GetSurface() == testSurfaceTop)
      m_LocationTreeTop.setAt(new CQfeAccessibleLocation(accessibleLocation));
   else if (accessibleLocation->GetSurface() == testSurfaceBottom)
      m_LocationTreeBot.setAt(new CQfeAccessibleLocation(accessibleLocation));

   return accessibleLocation;
}

void CProbePlacementSolution::RemoveAll_AccessibleLocations(CString netName)
{
   if (netName.IsEmpty())
   {
      // remove all probes
      POSITION netPos = m_ppNets.GetStartPosition();
      while (netPos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(netPos, netName, ppNet);

         ppNet->RemoveAll_AccessibleLocations();
         //if (ppNet->GetCount_Probes() == 0)
         //{
         // delete ppNet;
         // m_ppNets.RemoveKey(netName);
         //}
      }
   }
   else
   {
      // remove all probes under a net
      CPPNet *ppNet = NULL;
      if (!m_ppNets.Lookup(netName, ppNet))
         return;

      ppNet->RemoveAll_AccessibleLocations();
      //if (ppNet->GetCount_Probes() == 0)
      //{
      // delete ppNet;
      // m_ppNets.RemoveKey(netName);
      //}
   }

   m_LocationTreeTop.deleteAll();
   m_LocationTreeBot.deleteAll();
}

void CProbePlacementSolution::RemoveAll_Nets()
{
   // remove all probes
   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      delete ppNet;
   }

   m_ppNets.RemoveAll();
   m_LocationTreeTop.deleteAll();
   m_LocationTreeBot.deleteAll();
}

void CProbePlacementSolution::ShowProgress()
{
   if (m_pProgressDlg == NULL)
   {
      m_pProgressDlg = new CProgressDlg("Probe Placement");
      m_pProgressDlg->Create(AfxGetMainWnd());
      m_pProgressDlg->SetStep(1);
   }

   m_pProgressDlg->SetPos(0);
   m_pProgressDlg->ShowWindow(SW_SHOW);
}

void CProbePlacementSolution::HideProgress()
{
   if (m_pProgressDlg == NULL)
      return;

   m_pProgressDlg->ShowWindow(SW_HIDE);
}

void CProbePlacementSolution::destroyProgress()
{
   if (m_pProgressDlg == NULL)
      return;

   m_pProgressDlg->ShowWindow(SW_HIDE);
   if (m_pProgressDlg->DestroyWindow())
   {
      delete m_pProgressDlg;
      m_pProgressDlg = NULL;
   }
}

int CProbePlacementSolution::RunProbePlacement()
{
   if (m_pFile == NULL)
   {
      m_sErrMsg = "Current file pointer is NULL.";
      return -1;
   }

   // Reset probe placement solution
   m_ppNets.empty();
   m_sErrMsg.Empty();
   m_LocationTreeTop.deleteAll();
   m_LocationTreeBot.deleteAll();

   haveTopProbes = !m_pTestPlan->CanProbeTopSide();
   haveBotProbes = !m_pTestPlan->CanProbeBotSide();

   // make sure there are probe templates to use
   CDFTProbeTemplates &Templates = m_pTestPlan->GetProbes();
   POSITION pos = Templates.GetHeadPosition_TopProbes();
   while (pos && !haveTopProbes)
   {
      CDFTProbeTemplate *pTemplate = Templates.GetNext_TopProbes(pos);
      if (pTemplate->GetUseFlag())
         haveTopProbes = true;
   }

   pos = Templates.GetHeadPosition_BotProbes();
   while (pos && !haveBotProbes)
   {
      CDFTProbeTemplate *pTemplate = Templates.GetNext_BotProbes(pos);
      if (pTemplate->GetUseFlag())
         haveBotProbes = true;
   }

   if (!haveTopProbes && !haveBotProbes)
   {
      m_sErrMsg = "There were no probe templates to use.";
      return -1;
   }

   m_probeNamer.Reset(m_pTestPlan->GetProbeStartNumber(), 1, 1);

   long progressMin = 0, progressMax = 0;
   progressMax = 1 + 1 + 8;
   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetRange32(progressMin, progressMax);

   gatherAccesibleLocations();
   placeProbes();
	removeExcessUnplacedProbes();

   return 0;
}

long CProbePlacementSolution::PlaceProbesOnBoard()
{
   long placedProbeCount = 0;
   CMapWordToPtr accessPointerMap;  // Map entity num to entity
   long accPointCount = 1;
   m_netResultList.empty();

   POSITION netPos = m_ppNets.GetStartPosition();
   while (netPos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(netPos, netName, ppNet);

      // Creat net result
      CPPNetResult* netResult = new CPPNetResult(netName);
      m_netResultList.AddTail(netResult);

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);

         if (!probe->IsPlaced())
         {
            // Save unplaced proberesult
            CPPProbeResult* probeResult = netResult->AddProbeResult(probe->GetResourceType(), probe->GetNetName(), "", -1, probe->GetProbeTemplateName(), probe->GetProbeRefname());

            probe->AddProbeUnplacedToBoard(m_pFile, *m_pTestPlan);
            continue;
         }

         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

         if (accLoc == NULL)
            continue;

         // create an access point if one didn't exist (only for forced probes)
         CString accPointName;

         if (accLoc->GetTAInsert() == NULL)
         {
            // we need to create a test access point
            accPointName.Format("$$FORCEDACCESS_%d", accPointCount++);
            DataStruct *accData = PlaceTestAccessPoint(m_pDoc, m_pFile->getBlock(), accPointName, accLoc->GetLocation().x, accLoc->GetLocation().y,
               accLoc->GetSurface(), accLoc->GetNetName(), "SMD", accLoc->getFeatureEntity().getCompPin()->getEntityNumber(), 0.0,
               m_pDoc->getSettings().getPageUnits());
            accLoc->SetTAInsert(accData);
         }
         else
         {
            accPointName = accLoc->GetTAInsert()->getInsert()->getRefname();
            if (accPointName.IsEmpty())
            {
               // Access refname is blank, rather than try to figure out what the next n in $$ACCESS_n is, just
               // alter the name format to make it unique when it comes from here, and use the access marker's
               // entity number for n, since entity number is supposed to be unique for all things.
               accPointName.Format("$$ACCESS_ENTITY_%d", accLoc->GetTAInsert()->getEntityNumber());
               accLoc->GetTAInsert()->getInsert()->setRefname(accPointName);
            }
         }
         

         // Save probe result
         netResult->SetNoProbe(false);
         CPPProbeResult* probeResult = netResult->AddProbeResult(probe->GetResourceType(), probe->GetNetName(), 
            accPointName, accLoc->GetTAInsert()->getEntityNumber(), probe->GetProbeTemplateName(), probe->GetProbeRefname());

         // Map access name to access point
         accessPointerMap.SetAt((int)accLoc->GetTAInsert()->getEntityNumber(), accLoc->GetTAInsert()); 
      }
   }

   placedProbeCount = PlaceProbesResultOnBoard(accessPointerMap);
   accessPointerMap.RemoveAll();

   return placedProbeCount;
}

long CProbePlacementSolution::PlaceProbesResultOnBoard(CMapWordToPtr& accessPointMap)
{
   WORD dLinkKW = m_pDoc->IsKeyWord(ATT_DDLINK, 0);
   WORD testResKW = m_pDoc->RegisterKeyWord(ATT_TESTRESOURCE, 0, valueTypeString);
   WORD probePlacementKW = m_pDoc->RegisterKeyWord(ATT_PROBEPLACEMENT, 0, valueTypeString);
   WORD netNameKW = m_pDoc->IsKeyWord(ATT_NETNAME, 0);
   WORD refnameKW = m_pDoc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);
   long placedProbeCount = 0;

   if(!m_pFile) return placedProbeCount;
   Graph_Block_On(m_pFile->getBlock());

   POSITION netResultPos = m_netResultList.GetHeadPosition();
   while (netResultPos != NULL)
   {
      CPPNetResult* netResult = m_netResultList.GetNext(netResultPos);
      if (netResult == NULL)
         continue;

      if (netResult->IsNoProbe())
      {
         // add a drc marker if the net doesn't have any placed probes
         drc_net_without_probe(m_pDoc, m_pFile, netResult->GetNetName());
         continue;
      }

      POSITION probeResultPos = netResult->GetProbeHeadPosition();
      while (probeResultPos != NULL)
      {
         CPPProbeResult* probeResult = netResult->GetProbeNext(probeResultPos);
         if (probeResult == NULL || probeResult->GetAccessEntityNum() < 0)
            continue;

         DataStruct* accData = NULL;
         if (!accessPointMap.Lookup(probeResult->GetAccessEntityNum(), (void*&)accData) || accData == NULL)
            continue;

         InsertStruct* accInsert = accData->getInsert();
         if (accInsert == NULL)
            continue;

         int mirror = accInsert->getMirrorFlags();
         POSITION probeTemplatePos = NULL;
         CDFTProbeTemplate *pTemplate =  m_pTestPlan->GetProbes().Find_Probe(probeResult->GetProbeTemplateName(),
               probeTemplatePos, mirror?testSurfaceBottom:testSurfaceTop);
         if (pTemplate == NULL && m_pTestPlan->GetTesterType() == DFT_TESTER_FIXTURE)
            continue;


         // Create probe geometry
         CString probeName = pTemplate->GetName() + ((mirror == 0)?"_Top":"_Bot");
         double diameter = pTemplate->GetDiameter() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         double drillsize = pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         double textsize = pTemplate->GetTextSize() * Units_Factor(UNIT_MILS, m_pDoc->getSettings().getPageUnits());
         BlockStruct* probeBlock = CreateTestProbeGeometry(m_pDoc, probeName, diameter, probeName, drillsize);

         // Insert the probe 
         DataStruct *probe = Graph_Block_Reference(probeBlock->getName(), probeResult->GetProbeRefname(), -1, 
               accInsert->getOriginX(), accInsert->getOriginY(), 0.0, mirror, 1.0, -1, 0);
         probe->getInsert()->setInsertType(insertTypeTestProbe);
         placedProbeCount++;

         // Determine resource type
         CString testResourceType;
         switch (probeResult->GetResourceType())
         {
         case testResourceTypeTest:
            testResourceType = "Test";
            break;
         case testResourceTypePowerInjection:
            testResourceType = "Power Injection";
            break;
         }

         // Set attributes
         long datalink = accData->getEntityNumber();
         probe->setAttrib(m_pDoc->getCamCadData(), dLinkKW, valueTypeInteger, (void*)&datalink, attributeUpdateOverwrite, NULL);
         probe->setAttrib(m_pDoc->getCamCadData(), testResKW, valueTypeString, testResourceType.GetBuffer(0), attributeUpdateOverwrite, NULL);
         probe->setAttrib(m_pDoc->getCamCadData(), probePlacementKW, valueTypeString, "Placed", attributeUpdateOverwrite, NULL);
         probe->setAttrib(m_pDoc->getCamCadData(), netNameKW, valueTypeString, probeResult->GetNetName().GetBuffer(0), attributeUpdateOverwrite, NULL);

         CreateTestProbeRefnameAttr(m_pDoc, probe, probe->getInsert()->getRefname(), drillsize, textsize, m_pTestPlan->GetNeverMirrorRefname());

      }
   }

   Graph_Block_Off();

   return placedProbeCount;
}

int CProbePlacementSolution::ValidateAndPlaceProbes()
{
   haveTopProbes = !m_pTestPlan->CanProbeTopSide();
   haveBotProbes = !m_pTestPlan->CanProbeBotSide();

   // make sure there are probe templates to use
   CDFTProbeTemplates &Templates = m_pTestPlan->GetProbes();
   POSITION pos = Templates.GetHeadPosition_TopProbes();
   while (pos && !haveTopProbes)
   {
      CDFTProbeTemplate *pTemplate = Templates.GetNext_TopProbes(pos);
      if (pTemplate->GetUseFlag())
         haveTopProbes = true;
   }

   pos = Templates.GetHeadPosition_BotProbes();
   while (pos && !haveBotProbes)
   {
      CDFTProbeTemplate *pTemplate = Templates.GetNext_BotProbes(pos);
      if (pTemplate->GetUseFlag())
         haveBotProbes = true;
   }

   if (!haveTopProbes && !haveBotProbes)
   {
      m_sErrMsg = "There were no probe templates to use.";
      return -1;
   }

   RemoveAll_Nets();
   
   m_probeNamer.Reset(m_pTestPlan->GetProbeStartNumber(), 1, 1);

   long progressMin = 0, progressMax = 0;
   progressMax = 1 + 1 + 8;
   if (m_pProgressDlg != NULL)
      m_pProgressDlg->SetRange32(progressMin, progressMax);

   gatherAccesibleLocations();
   gatherPlacedProbes();
   maximizeProbeSizes();
   while (fixViolatingProbeTemplates() > 0)
      ;

   return 0;
}

void CProbePlacementSolution::GetProbeCounts(int &topPlaced, int &botPlaced, int &totUnplaced, int &forcedInactiveSide, CProbeCountArray &countArray,
                                             int &netsInvolved, int &netsFullyProbed, int &netsPartiallyProbed, int &netsNotProbed)
{
   topPlaced = botPlaced = totUnplaced = forcedInactiveSide = 0;
   netsInvolved = netsFullyProbed = netsPartiallyProbed = netsNotProbed = 0;
   countArray.RemoveAll();

   // get all the probe names
   POSITION pos = m_pTestPlan->GetProbes().GetHeadPosition_TopProbes();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_TopProbes(pos);
      countArray.Add(CProbeCount(pTemplate->GetName(), testSurfaceTop, m_pTestPlan->CanProbeTopSide() && pTemplate->GetUseFlag()));
   }
   pos = m_pTestPlan->GetProbes().GetHeadPosition_BotProbes();
   while (pos)
   {
      CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().GetNext_BotProbes(pos);

      bool foundTemplate = false;
      for (int i=0; i<countArray.GetCount(); i++)
      {
         CProbeCount &probeCount = countArray.GetAt(i);
         if (probeCount.GetProbeName() == pTemplate->GetName())
         {
            probeCount.SetProbeSurface(testSurfaceBoth);
            probeCount.SetBottomUsedFlag(m_pTestPlan->CanProbeBotSide() && pTemplate->GetUseFlag());
            foundTemplate = true;
            break;
         }
      }

      if (!foundTemplate)
         countArray.Add(CProbeCount(pTemplate->GetName(), testSurfaceBottom, m_pTestPlan->CanProbeBotSide() && pTemplate->GetUseFlag()));
   }

   pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      bool hasPlacedProbe = false;
      bool hasUnplacedProbe = false;
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      
      if (probePos != NULL)
         netsInvolved++;

      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);
         CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();
         if (accLoc == NULL)
         {
            hasUnplacedProbe = true;
            totUnplaced++;
         }
         else
         {
            hasPlacedProbe = true;
            if (accLoc->GetSurface() == testSurfaceTop)
               topPlaced++;
            else if (accLoc->GetSurface() == testSurfaceBottom)
               botPlaced++;

            if (accLoc->GetFeatureCondition() == featureConditionForced)
            {
               if ((accLoc->GetSurface() == testSurfaceTop && !m_pTestPlan->CanProbeTopSide()) ||
                  (accLoc->GetSurface() == testSurfaceBottom && !m_pTestPlan->CanProbeBotSide()))
               {
                  forcedInactiveSide++;
               }
            }

            POSITION pTempPos = NULL;
            CDFTProbeTemplate *pTemplate = m_pTestPlan->GetProbes().Find_Probe(probe->GetProbeTemplateName(), pTempPos, accLoc->GetSurface());
            if (pTemplate == NULL)
               continue;

            for (int i=0; i<countArray.GetCount(); i++)
            {
               CProbeCount &probeCount = countArray.GetAt(i);
               if (probeCount.GetProbeName() == pTemplate->GetName())
               {
                  if (probe->GetResourceType() == testResourceTypeTest)
                  {
                     if (accLoc->GetSurface() == testSurfaceTop)
                     {
                        probeCount.IncrementTopTestResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrementTopTestResources();
                     }
                     else if (accLoc->GetSurface() == testSurfaceBottom)
                     {
                        probeCount.IncrementBottomTestResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrementBottomTestResources();
                     }
                  }
                  else if (probe->GetResourceType() == testResourceTypePowerInjection)
                  {
                     if (accLoc->GetSurface() == testSurfaceTop)
                     {
                        probeCount.IncrememtTopPowerInjectionResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrememtTopPowerInjectionResources();
                     }
                     else if (accLoc->GetSurface() == testSurfaceBottom)
                     {
                        probeCount.IncrememtBottomPowerInjectionResources();
                        if (accLoc->IsDoubleWired())
                           probeCount.IncrememtBottomPowerInjectionResources();
                     }
                  }

                  break;
               }
            }
         }
      }

      if (hasPlacedProbe && hasUnplacedProbe) // has both placed and unplaced
         netsPartiallyProbed++;
      else if (hasPlacedProbe) // has placed and no unplaced
         netsFullyProbed++;
      else if (hasUnplacedProbe)
         netsNotProbed++;
      // else is not a participating net
   }
}

void CProbePlacementSolution::ReservePreexistingProbeRefnames()
{
   if (m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbeNames())
   {
      POSITION pos = m_ppNets.GetStartPosition();
      while (pos)
      {
         CString netName;
         CPPNet *ppNet = NULL;
         m_ppNets.GetNextAssoc(pos, netName, ppNet);

         if (!ppNet->GetNoProbeFlag())
         {
            POSITION probePos = ppNet->GetHeadPosition_Probes();
            while (probePos)
            {
               CPPProbe *probe = ppNet->GetNext_Probes(probePos);

               if (probe->DidExist())
                  m_probeNamer.Strike(probe->GetProbeRefname());
            }
         }
      }
   }
}

void CProbePlacementSolution::RenumberProbes()
{
   // Initial naming of probes is done before it is known what probes will end up placed
   // and what will end up unplaced. Mark asked for probes to be numbered in continuous
   // blocks, all placed probes first, then unplaced probes. So here we renumber aka rename
   // the probes. Watch out not to renumber any "use existing - retain probe names" probes.

   // Four passes, states are:
   // 1 - Number placed probes that are not Power Injection
   // 2 - Number unplaced probes that are not Power Injection
   // 3 - Number placed power injection probes
   // 4 - Number unplaced power injection probes

   m_probeNamer.Reset(m_pTestPlan->GetProbeStartNumber(), 1, 1);
   ReservePreexistingProbeRefnames();

   // Sort the nets first, this will result in probes assigned low to high on nets
   // in ascending alpha order, a double whammy ordering for the list box.
   m_ppNets.setSortFunction(&CPPNetMap::AscendingNetNameSortFunc);
   m_ppNets.Sort();

   for (int state = 1; state <= 4; state++)
   {
      CString *netName = NULL;
      CPPNet *ppNet = NULL;
      for (m_ppNets.GetFirstSorted(netName, ppNet); ppNet != NULL; m_ppNets.GetNextSorted(netName, ppNet))
      {
         if (!ppNet->GetNoProbeFlag())
         {
            POSITION probePos = ppNet->GetHeadPosition_Probes();
            while (probePos)
            {
               CPPProbe *probe = ppNet->GetNext_Probes(probePos);

               if (probe->DidExist() && m_pTestPlan->GetUseExistingProbes() && m_pTestPlan->RetainProbeNames())
               {
                  // Pre-existing probe, leave name alone
               }
               else
               {
                  // Rename

                  switch (state)
                  {
                  case 1:   // Placed probes that are Not power injection
                     {
                        if (probe->IsPlaced() && probe->GetResourceType() == testResourceTypeTest)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  case 2:   // Unplaced probes that are Not power injection
                     {
                        if (!probe->IsPlaced() && probe->GetResourceType() == testResourceTypeTest)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  case 3:   // Placed probes that Are power injection
                     {
                        if (probe->IsPlaced() && probe->GetResourceType() == testResourceTypePowerInjection)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  case 4:   // Unplaced probes that Are power injection
                     {
                        if (!probe->IsPlaced() && probe->GetResourceType() == testResourceTypePowerInjection)
                           probe->SetProbeRefname(m_probeNamer.GetNextProbeRefname(ppNet, probe->GetResourceType()));
                     }
                     break;
                  }
               }
            }
         }
      }
   }
}

bool CProbePlacementSolution::CreateReport(CString Filename, CAccessAnalysisSolution *aaSolution)
{
   CFormatStdioFile file;
   CFileException err;

   if (!file.Open(Filename, CFile::modeCreate|CFile::modeWrite, &err))
      return false;

   int indent = 0;
   CTime t = t.GetCurrentTime();
   file.WriteString("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   file.WriteString("+ Probe Placement Report\n");
   file.WriteString("+\n");
   file.WriteString("+ Created by %s - %s\n", getApp().getCamCadTitle(), getApp().getCopyrightString());
   file.WriteString("+ Version     : %s\n", getApp().getVersionString());
   file.WriteString("+ Date & Time : %s\n", t.Format("%A, %B %d, %Y at %H:%M:%S"));
   file.WriteString("+ PCB File    : %s\n", m_pFile->getName());
   file.WriteString("+ Units       : %s\n", GetUnitName(m_pDoc->getSettings().getPageUnits()));
   file.WriteString("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   file.WriteString("\n\n");

   file.WriteString("=====================================================================================\n");
   file.WriteString("= TEST PLAN SETTINGS\n");
   file.WriteString("=====================================================================================\n");
   m_pTestPlan->WriteReport(file, 3);
   file.WriteString("\n\n");

   int topPlaced = 0, botPlaced = 0;
   int totUnplaced = 0;
   int forcedInactiveSide = 0;
   int netsInvolved = 0, netsFullyProbed = 0, netsPartiallyProbed = 0, netsNotProbed = 0;
   CProbeCountArray countArray;
   GetProbeCounts(topPlaced, botPlaced, totUnplaced, forcedInactiveSide, countArray, netsInvolved, netsFullyProbed, netsPartiallyProbed, netsNotProbed);

   CExtent ext = m_pFile->getBlock()->getExtent();

   if (!ext.isValid())
   {
      m_pDoc->CalcBlockExtents(m_pFile->getBlock());
      ext = m_pFile->getBlock()->getExtent();
   }

   file.WriteString("=====================================================================================\n");
   file.WriteString("= BOARD STATISTICS\n");
   file.WriteString("=====================================================================================\n");
   file.WriteString("Board Size : %.3f x %.3f\n", ext.getXsize(), ext.getYsize());
   file.WriteString("Probes Placed : %d\n", topPlaced + botPlaced);
   file.WriteString("Probes Unplaced : %d\n", totUnplaced);
   if (forcedInactiveSide > 0)
      file.WriteString("Probes Forced to Inactive Side : %d\n", forcedInactiveSide);
   file.WriteString("Nets not probed : %d (%.2f%%)\n", netsNotProbed,              ((double)netsNotProbed / (double)netsInvolved) * 100.);
   file.WriteString("Nets partially probed : %d (%.2f%%)\n", netsPartiallyProbed,  ((double)netsPartiallyProbed / (double)netsInvolved) * 100.);
   file.WriteString("Nets completely probed : %d (%.2f%%)\n", netsFullyProbed,     ((double)netsFullyProbed / (double)netsInvolved) * 100.);
   file.WriteString("Nets not analyzed : %d\n", m_ppNets.GetCount() - netsInvolved);
   file.WriteString("\n");
   CString boundingLine = "-----------------------";
   CString probeLine    = "|Probes               |";
   CString divideLine   = "|                     |";
   CString divideLine2  = "|                     |";
   CString divideLine3  = "|---------------------|";
   for (int i=0; i<countArray.GetCount(); i++)
   {
      CProbeCount &probeCount = countArray.GetAt(i);

      CString buf;
      buf.Format("    %-s%*s|", probeCount.GetProbeName(), 7-probeCount.GetProbeName().GetLength(), " ");

      boundingLine += "------------";
      probeLine    += buf;
      divideLine   += "-----------|";
      divideLine2  += " TR  | PIJ |";
      divideLine3  += "-----------|";
   }
   file.WriteString("%s\n", boundingLine);
   file.WriteString("%s\n", probeLine);
   file.WriteString("%s\n", divideLine);
   file.WriteString("%s\n", divideLine2);
   file.WriteString("%s\n", divideLine3);

   // top results
   file.WriteString("|Top Probes Placed    | ");
   for (int i=0; i<countArray.GetCount(); i++)
   {
      CProbeCount &probeCount = countArray.GetAt(i);

      if (probeCount.GetTopUsedFlag())
      {
         CString buf;
         buf.Format("%d", probeCount.GetTopTestResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");

         buf.Format("%d", probeCount.GetTopPowerInjectionResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");
      }
      else
         file.WriteString("-   | -   | ");
   }
   file.WriteString("\n");

   // bottom results
   file.WriteString("|Bottom Probes Placed | ");
   for (int i=0; i<countArray.GetCount(); i++)
   {
      CProbeCount &probeCount = countArray.GetAt(i);

      if (probeCount.GetBottomUsedFlag())
      {
         CString buf;
         buf.Format("%d", probeCount.GetBottomTestResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");

         buf.Format("%d", probeCount.GetBottomPowerInjectionResourceCount());
         file.WriteString("%-s%*s| ", buf, 4-buf.GetLength(), " ");
      }
      else
         file.WriteString("-   | -   | ");
   }
   file.WriteString("\n");
   file.WriteString("%s\n", boundingLine);
   file.WriteString("\n\n");

   // This will get the Unplaced Probe report section output in
   // ascending net name order.
   m_ppNets.setSortFunction(&CPPNetMap::AscendingNetNameSortFunc);
   m_ppNets.Sort();

   WriteUnplacedProbePlacement(file, aaSolution, includeMultiPinNet);
   WriteUnplacedProbePlacement(file, aaSolution, includeSinglePinNet);
   WriteUnplacedProbePlacement(file, aaSolution, includeUnconnecedNet);

   WritePlacedProbePlacement(file, aaSolution, includeMultiPinNet);
   WritePlacedProbePlacement(file, aaSolution, includeSinglePinNet);
   WritePlacedProbePlacement(file, aaSolution, includeUnconnecedNet);

   file.Close();

   return true;
}

/******************************************************************************
*/
void CProbePlacementSolution::WriteNetAccessInfo(CFormatStdioFile &file, CAANetAccess *netAcc)
{
   if (netAcc != NULL)
   {
      POSITION netAccPos = netAcc->GetHeadPosition_AccessibleLocations();
      while (netAccPos)
      {
         CAAAccessLocation *accLoc = netAcc->GetNext_AccessibleLocations(netAccPos);

         if (m_pTestPlan->GetPPWriteIgnoreSurface() == false && m_pTestPlan->GetProbeSide() != testSurfaceBoth &&
            m_pTestPlan->GetProbeSide() != accLoc->GetAccessSurface())
            continue;

         if (!accLoc->GetAccessible())
         {
            CString violation = accLoc->GetErrorMessage();

            CString featureName;
            CPoint2d location;

            if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            {
               featureName = accLoc->getFeatureEntity().getCompPin()->getPinRef('-');
               location    = accLoc->getFeatureEntity().getCompPin()->getOrigin();
            }
            else if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            {
               featureName = accLoc->getFeatureEntity().getData()->getInsert()->getRefname();
               location    = accLoc->getFeatureEntity().getData()->getInsert()->getOrigin2d();
            }

            file.WriteString("%-15s %-8s %-8.3f %-8.3f %-s\n", featureName, (accLoc->GetAccessSurface()==testSurfaceTop)?"T":"B",
               location.x, location.y, violation);
         }
      }
   }
}

/******************************************************************************
*/
void CProbePlacementSolution::WriteNetAccessInfo(CFormatStdioFile &file, CPPNet *ppNet)
{
   if (ppNet != NULL)
   {
      POSITION accPos = ppNet->GetHeadPosition_AccessibleLocations();
      while (accPos)
      {
         CPPAccessibleLocation *accLoc = ppNet->GetNext_AccessibleLocations(accPos);

         if (m_pTestPlan->GetPPWriteIgnoreSurface() == false && m_pTestPlan->GetProbeSide() != testSurfaceBoth &&
            m_pTestPlan->GetProbeSide() != accLoc->GetSurface())
            continue;

         CString info;
         if (accLoc->GetPlaceableProbe() != NULL)
         {
            CPPProbe *secondProbe = accLoc->GetDoubleWiredProbe();
            info.Format("Placed Probe%s:  %s%s%s", 
               secondProbe != NULL ? "s" : "",   // plural in label to match number of probes
               accLoc->GetPlaceableProbe()->GetProbeRefname(),
               secondProbe != NULL ? ", " : "",  // separator between probe numbers
               secondProbe != NULL ? secondProbe->GetProbeRefname() : "");
         }
         else if (m_pTestPlan->GetProbeSide() != testSurfaceBoth && accLoc->GetSurface() != m_pTestPlan->GetProbeSide())
            info = "Surface not evaluated";
         else if (ppNet->GetNoProbeFlag())
            info = "Net set to No Probe";
         else if (accLoc->GetFeatureCondition() == featureConditionNoProbe)
            info = "Feature set to No Probe";
         else if (!isValidTargetType(accLoc))
            info = "Excluded target type";
         else if (accLoc->IsProbed() && !accLoc->IsDoubleWired() && !m_pTestPlan->GetAllowDoubleWiring())
            info = "Double wiring option not allowed";
         else
            info = "Probe-to-probe spacing violation";

         CString featureName;
         if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            featureName = accLoc->getFeatureEntity().getCompPin()->getPinRef('-');
         else if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            featureName = accLoc->getFeatureEntity().getData()->getInsert()->getRefname();

         file.WriteString("%-15s %-8s %-8.3f %-8.3f %-s\n", featureName, (accLoc->GetSurface()==testSurfaceTop)?"T":"B",
            accLoc->GetLocation().x, accLoc->GetLocation().y, info);

      }
   }
}



/******************************************************************************
* WriteProbePlacement
*/
void CProbePlacementSolution::WriteUnplacedProbePlacement(CFormatStdioFile &file, CAccessAnalysisSolution *aaSolution, EIncludedNetType netType)
{
   CString netTypeStr = "";
   if (netType == includeMultiPinNet) 
      netTypeStr = "Multiple Pin Nets";
   else if (netType == includeSinglePinNet)
      netTypeStr = "Single Pin Nets";
   else if (netType == includeUnconnecedNet)
      netTypeStr = "Unconnected Nets";

   file.WriteString("=====================================================================================\n");
   file.WriteString("= UNPLACED PROBES: %s\n", netTypeStr);
   file.WriteString("=====================================================================================\n");

   if (aaSolution == NULL)
      return;

   CString *netname = NULL;
   CPPNet *ppNet = NULL;
   for (m_ppNets.GetFirstSorted(netname, ppNet); ppNet != NULL; m_ppNets.GetNextSorted(netname, ppNet))
   {

      CAANetAccess *netAcc = aaSolution->GetNetAccess(*netname);
      if (netAcc != NULL && netAcc->GetIncludeNetType() == netType)
      {
         if (ppNet->HasUnplacedProbe())
         {
            // Net Name
            file.WriteString("NET: %s\n", *netname);

            // Unplaced probes numbers
            CString unplacedProbeNumbers;
            bool plural = false;
            POSITION probePos = ppNet->GetHeadPosition_Probes();
            while (probePos)
            {
               CPPProbe *probe = ppNet->GetNext_Probes(probePos);
               if (!probe->IsPlaced())
               {
                  if (!unplacedProbeNumbers.IsEmpty())
                  {
                     unplacedProbeNumbers += ", ";
                     plural = true;
                  }
                  unplacedProbeNumbers += probe->GetProbeRefname();
               }
            }
            file.WriteString("Unplaced probe%s: %s\n", plural ? "s" : "", unplacedProbeNumbers);

            // Net access stats
            WriteNetAccessInfo(file, netAcc);
            WriteNetAccessInfo(file, ppNet);

            file.WriteString("\n");

         }
      }
   }
   file.WriteString("\n\n");
}

void CProbePlacementSolution::WritePlacedProbePlacement(CFormatStdioFile &file, CAccessAnalysisSolution *aaSolution, EIncludedNetType netType)
{
   CString netTypeStr = "";

   if (netType == includeMultiPinNet) 
      netTypeStr = "Multiple Pin Nets";
   else if (netType == includeSinglePinNet)
      netTypeStr = "Single Pin Nets";
   else if (netType == includeUnconnecedNet)
      netTypeStr = "Unconnected Nets";

   file.WriteString("=====================================================================================\n");
   file.WriteString("= PLACED PROBES: %s\n", netTypeStr);
   file.WriteString("=====================================================================================\n");

   if (aaSolution == NULL)
      return;

   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netname;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netname, ppNet);

      CAANetAccess *netAcc = aaSolution->GetNetAccess(netname);

      if (netAcc == NULL || netAcc->GetIncludeNetType() != netType)
         continue;

      POSITION probePos = ppNet->GetHeadPosition_Probes();
      while (probePos)
      {
         CPPProbe *probe = ppNet->GetNext_Probes(probePos);

         if (probe->IsPlaced())
         {
            CString buf;
            CPPAccessibleLocation *accLoc = probe->GetAccessibleLocation();

            if (accLoc->getFeatureEntity().getEntityType() == entityTypeData)
            {
               DataStruct* data = accLoc->getFeatureEntity().getData();

               if (data->getDataType() == dataTypeInsert)
               {
                  buf = data->getInsert()->getRefname();
               }
               else
               {
                  buf = dataStructTypeToString(data->getDataType());
               }
            }
            else if (accLoc->getFeatureEntity().getEntityType() == entityTypeCompPin)
            {
               buf = accLoc->getFeatureEntity().getCompPin()->getPinRef('-');
            }

            file.WriteString("%-8s %-8s %-8.3f %-8.3f %-8s %-15s %-8s%s\n",
               probe->GetProbeRefname(), probe->GetProbeTemplateName(),
               accLoc->GetLocation().x, accLoc->GetLocation().y,
               buf, probe->GetNetName(), 
               (accLoc->GetSurface() == testSurfaceTop)?"Top":"Bottom",
               (accLoc->GetFeatureCondition() == featureConditionForced)?" Forced":"");
         }
      }
   }

   file.WriteString("\n\n");
}

void CProbePlacementSolution::DumpToFile(CFormatStdioFile &file, int indent)
{
   file.WriteString("%*s::PPNets::\n", indent, " ");
   indent += 3;
   POSITION pos = m_ppNets.GetStartPosition();
   while (pos)
   {
      CString netName;
      CPPNet *ppNet = NULL;
      m_ppNets.GetNextAssoc(pos, netName, ppNet);

      ppNet->DumpToFile(file, indent);
   }
   indent -= 3;

   CHtmlFileWriteFormat htmlLogFile(1024);
   CString path = file.GetFilePath();
   if (htmlLogFile.open(path.Mid(0, path.ReverseFind('.')) + "_metrics.htm"))
   {
      m_LocationTreeTop.printMetricsReport(htmlLogFile);
      m_LocationTreeBot.printMetricsReport(htmlLogFile);
   }
   htmlLogFile.close();
}

void CProbePlacementSolution::WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress)
{
   writeFormat.writef("<PPSolution>\n");
   writeFormat.pushHeader("  ");

   POSITION pos = m_netResultList.GetHeadPosition();
   while (pos)
   {
      CPPNetResult* netResult = m_netResultList.GetNext(pos);
      if (netResult == NULL)
         continue;

      netResult->WriteXML(writeFormat, progress);
   }

   writeFormat.popHeader();
   writeFormat.writef("</PPSolution>\n");
}

CPPNetResult *CPPNetResultList::GetResultForNet(CString netName)
{
   POSITION pos = this->GetHeadPosition();
   while (pos != NULL)
   {
      CPPNetResult* netResult = this->GetNext(pos);
      if (netResult != NULL)
      {
         if (netResult->GetNetName().CompareNoCase(netName) == 0)
            return netResult;
      }
   }

   return NULL;
}

//extern bool OdbPpIn_TranslateOdbNetName(CString& netName);

int CProbePlacementSolution::LoadXML(CXMLNode *node)
{
   if (node == NULL)
      return -1;

   CXMLNode *subNode = NULL;
   node->ResetChildList();
   while (subNode = node->NextChild())
   {
      CString nodeName;
      if (!subNode->GetName(nodeName))
         continue;

      if (nodeName == "ProbeNet")
      {
         // Traditional RSI DFT Solution style

         CString netName;
         if (!subNode->GetAttrValue("Name", netName))
            continue;

         CPPNetResult *netResult = new CPPNetResult(netName);
         m_netResultList.AddTail(netResult);

         netResult->LoadXML(subNode);
      }
      else if (nodeName == "Probe")
      {
         // vPlan style

         CPPProbeResult *ppRes = CPPProbeResult::LoadXML(subNode);
         if (ppRes != NULL)
         {
            CString netName = ppRes->GetNetName();

            // Concession to knowing this is coming from vPlan, hence ODB. Convert the ODB $None$ net name like ODB importer does.
            //if (OdbPpIn_TranslateOdbNetName(netName))
               //ppRes->SetNetName(netName);

            if (netName.IsEmpty())
            {
               delete ppRes; // Can't save a probe without the net.
            }
            else
            {
               CPPNetResult *netResult = m_netResultList.GetResultForNet(netName);
               if (netResult == NULL)
               {
                  netResult = new CPPNetResult(netName, false /*no probe*/);
                  m_netResultList.AddTail(netResult);
               }

               netResult->AddProbeResult(ppRes);
            }
         }
      }

      delete subNode;
   }

   return 0;
}


////////////////////////////////////////////////////////////////////////////////
// CProbePlacementExitDlg dialog
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CProbePlacementExitDlg, CDialog)
CProbePlacementExitDlg::CProbePlacementExitDlg(CString message, CWnd* pParent)
: CDialog(CProbePlacementExitDlg::IDD, pParent)
, m_msg(message)
{
}

BEGIN_MESSAGE_MAP(CProbePlacementExitDlg, CDialog)
   ON_BN_CLICKED(IDC_PROBE_STYLES, OnBnClickedProbeStyles)
END_MESSAGE_MAP()

BOOL CProbePlacementExitDlg::OnInitDialog()
{
   GetDlgItem(IDC_MESSAGE)->SetWindowText(m_msg);

   return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// CProbePlacementDlg dialog
////////////////////////////////////////////////////////////////////////////////
#define RESULTS_ROWS_FIXED1   0
#define RESULTS_ROWS_FIXED2   1
#define RESULTS_ROWS_TOP      2
#define RESULTS_ROWS_BOT      3
#define RESULTS_ROWS_TOTAL    4

IMPLEMENT_DYNAMIC(CProbePlacementDlg, CDialog)
CProbePlacementDlg::CProbePlacementDlg(CCEtoODBDoc *Doc, CWnd* pParent)
: CDialog(CProbePlacementDlg::IDD, pParent)
, m_probesDlg(Doc)
, m_preconditionsDlg(Doc)
, m_netConditionsDlg(Doc)
, m_DFTSolution("PPSolution_Temp", false/*isFlipped*/, Doc->getSettings().getPageUnits())
{
   lastTTTRow = -1;
   lastTTTCol = -1;

   m_pDoc = Doc;
   m_pFile = m_pDoc->getFileList().GetOnlyShown(blockTypePcb);
}

CProbePlacementDlg::~CProbePlacementDlg()
{
}

void CProbePlacementDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProbePlacementDlg, CDialog)
   ON_BN_CLICKED(IDC_PROCESS, OnBnClickedProcess)
   ON_BN_CLICKED(IDC_DisplayReport, OnBnClickedDisplayReport)
   ON_COMMAND(IPP_UPDATEDIALOG, OnUpdateFromTabDialog)
   ON_COMMAND(IPP_SETMODIFIED, OnSetModifiedPage)
   ON_COMMAND(IPP_UPDATEPAGES, OnUpdatePages)
   ON_MESSAGE(IPP_SETPAGE, OnSetPage)
   ON_BN_CLICKED(IDC_SAVE_TESTPLAN_DEBUG, OnBnClickedSaveTestplanDebug)
   ON_WM_LBUTTONDBLCLK()
   ON_BN_CLICKED(IDC_SAVE_TESTPLAN, OnBnClickedSaveTestplan)
END_MESSAGE_MAP()


// CProbePlacementDlg message handlers

BOOL CProbePlacementDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   if (m_pDoc == NULL)
      return FALSE;

   // Set up Results Grid
   m_ppResultsGrid.AttachGrid(this, IDC_PPResultsGridStatic);
   CRect rcGridWindow;
   GetDlgItem( IDC_PPResultsGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_ppResultsGrid.SizeToFit(rcGridWindow);

   if (m_pFile != NULL)
   {
      /*CDFTSolution *pSolution = m_pDoc->GetCurrentDFTSolution(*m_pFile);
      if (pSolution != NULL)
         m_DFTSolution.CopyDFTSolution(*pSolution);*/
   }

   populateDFTProbePlacementData();
   createAndPlacePropertySheet();

   m_DFTSolution.GetTestPlan()->ResetModified();
   GetDlgItem(IDC_PROCESS)->EnableWindow(TRUE);
   GetDlgItem(IDOK)->EnableWindow(FALSE);
   GetDlgItem(IDC_DisplayReport)->EnableWindow(FALSE);

   UpdateData(FALSE);
   //EnableToolTips(TRUE);
   //EnableTrackingToolTips(TRUE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CProbePlacementDlg::createAndPlacePropertySheet()
{
   if (m_DFTSolution.GetTestPlan() != NULL)
   {
      CTestPlan *testPlan = m_DFTSolution.GetTestPlan();

      // set the test plan if there is one
      m_targetPriorityDlg.SetTestPlanPointer(testPlan);
      m_probesDlg.SetTestPlanPointer(testPlan);
      m_kelvinPowerInjectionDlg.SetTestPlanPointer(testPlan);
      m_preconditionsDlg.SetTestPlanPointer(testPlan);
      m_netConditionsDlg.SetTestPlanPointer(testPlan);
      m_optionsDlg.SetTestPlanPointer(testPlan);
      m_probeLockingDlg.SetTestPlanPointer(testPlan);
   }

   // set the parent window so that we can get notification of changes to the tabbed dialogs
   m_targetPriorityDlg.SetCurrentParentWnd(this);
   m_probesDlg.SetCurrentParentWnd(this);
   m_kelvinPowerInjectionDlg.SetCurrentParentWnd(this);
   m_preconditionsDlg.SetCurrentParentWnd(this);
   m_netConditionsDlg.SetCurrentParentWnd(this);
   m_optionsDlg.SetCurrentParentWnd(this);
   m_probeLockingDlg.SetCurrentParentWnd(this);


   // Add pages to the tab control
   m_propSheet.AddPage(&m_targetPriorityDlg);
   m_propSheet.AddPage(&m_probesDlg);
   m_propSheet.AddPage(&m_kelvinPowerInjectionDlg);
   m_propSheet.AddPage(&m_preconditionsDlg);
   m_propSheet.AddPage(&m_netConditionsDlg);
   m_propSheet.AddPage(&m_optionsDlg);
   //m_propSheet.AddPage(&m_probeLockingDlg);

   // Create the property sheet
   m_propSheet.Create(this, WS_CHILD | WS_VISIBLE/* | WS_BORDER*/, 0);
   m_propSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
   m_propSheet.ModifyStyle( 0, WS_TABSTOP );

   CRect rcSheet;
   GetDlgItem( IDC_STATIC_PROPERTYSHEET )->GetWindowRect( &rcSheet );
   ScreenToClient( &rcSheet );
   m_propSheet.SetWindowPos( NULL, rcSheet.left, rcSheet.top, rcSheet.Width(), rcSheet.Height(), 
         SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
}

void CProbePlacementDlg::populateDFTProbePlacementData()
{
   if (m_pDoc == NULL || m_DFTSolution.GetTestPlan() == NULL)
      return;

   CTestPlan *testPlan = m_DFTSolution.GetTestPlan();
   FileStruct *file = m_pDoc->getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
      return;

   //////////////////////////////////////////////////
   // Gather target priorities
   CDFTTargetPriority &tPriority =  testPlan->GetTargetPriority();
   CMapStringToString targetTypeTopMap;
   CMapStringToString targetTypeBotMap;

   targetTypeTopMap.RemoveAll();
   targetTypeBotMap.RemoveAll();

   // only add target type if it's empty, which means that it was probably never loaded
   //if (tPriority.GetCount_TopTargets() == 0 && tPriority.GetCount_BotTargets() == 0)
   {
      WORD targetTypeKw = m_pDoc->RegisterKeyWord("TARGET_TYPE", 0, valueTypeString);
      BlockStruct *fileBlock = file->getBlock();
      if (fileBlock != NULL)
      {
         POSITION pos = fileBlock->getHeadDataInsertPosition();
         while (pos)
         {
            DataStruct *data = fileBlock->getNextDataInsert(pos);
            InsertStruct *insert = data->getInsert();

            if (insert->getInsertType() != insertTypeTestAccessPoint)
               continue;

            // get the target type attribute
            Attrib *a = NULL;
            if (!(data->getAttributes() && data->getAttributes()->Lookup(targetTypeKw, a)))
               continue;

            // get the surface of the feature
            BlockStruct *insBlock = m_pDoc->getBlockAt(insert->getBlockNumber());
            DataStruct *subData = insBlock->getAtData(insBlock->getHeadDataPosition());
            LayerStruct *layer = m_pDoc->getLayerAt(subData->getLayerIndex());
            CString targetName = get_attvalue_string(m_pDoc, a);

            int strIndex = 0;
            CString curTargetName = targetName.Tokenize(", ", strIndex);
            while (!curTargetName.IsEmpty())
            {
               if (layer->getType() == layerTypeDftTop)
               {
                  if (!insert->getLayerMirrored())
                  {
                     targetTypeTopMap.SetAt(curTargetName, curTargetName);
                     int index = -1;
                     if (tPriority.Find_TopTargets(curTargetName, index) == NULL)
                        tPriority.Add_TopTargets(true, curTargetName);
                  }
                  else
                  {
                     targetTypeBotMap.SetAt(curTargetName, curTargetName);
                     int index = -1;
                     if (tPriority.Find_BotTargets(curTargetName, index) == NULL)
                        tPriority.Add_BotTargets(true, curTargetName);
                  }
               }

               curTargetName = targetName.Tokenize(", ", strIndex);
            }
         }
      }
   }

   // Delete top target type from testplan that are not found
   CString targetName;
	int i=0;
   for(i=0; i<tPriority.GetCount_TopTargets();)
   {
      CDFTTarget* target = tPriority.GetAt_TopTargets(i);
      if (!targetTypeTopMap.Lookup(target->GetName(), targetName))
         tPriority.RemoveAt_TopTargets(i);
      else
         i++;
   }

   // Delete bottom target type from testplan that are not found
   for(i=0; i<tPriority.GetCount_BotTargets(); i++)
   {
      CDFTTarget* target = tPriority.GetAt_BotTargets(i);
      if (!targetTypeBotMap.Lookup(target->GetName(), targetName))
         tPriority.RemoveAt_BotTargets(i);
      else
         i++;
   }

   // set the side to use based on the access points we have
   if (tPriority.GetCount_TopTargets() > 0 && tPriority.GetCount_BotTargets() > 0)
      testPlan->SetProbeSide(testSurfaceBoth);
   else if (tPriority.GetCount_TopTargets() > 0 && tPriority.GetCount_BotTargets() == 0)
      testPlan->SetProbeSide(testSurfaceTop);
   else if (tPriority.GetCount_TopTargets() == 0 && tPriority.GetCount_BotTargets() > 0)
      testPlan->SetProbeSide(testSurfaceBottom);

   //////////////////////////////////////////////////
   // Gather all nets for NetConditions
   testPlan->GetNetConditions().Initialize(m_pDoc, file, &m_DFTSolution);
}

void CProbePlacementDlg::OnUpdateFromTabDialog()
{
   m_optionsDlg.UpdateTestPlan();
   m_targetPriorityDlg.UpdateUI();
   m_probesDlg.UpdateUI();
}

LRESULT CProbePlacementDlg::OnSetPage(WPARAM wParam, LPARAM lParam)
{
   int tabIndex = (int)lParam;
   if (tabIndex < 0 || tabIndex >= m_propSheet.GetPageCount())
      return 0;

   m_propSheet.SetActivePage(tabIndex);
   return 0;
}

void CProbePlacementDlg::OnSetModifiedPage()
{
   GetDlgItem(IDC_PROCESS)->EnableWindow(TRUE);
   GetDlgItem(IDOK)->EnableWindow(FALSE);
   GetDlgItem(IDC_DisplayReport)->EnableWindow(FALSE);
}

void CProbePlacementDlg::OnUpdatePages()
{
   m_netConditionsDlg.ReloadFromTestPlan();
}

void CProbePlacementDlg::OnBnClickedProcess()
{
   CTestPlan *testPlan = m_DFTSolution.GetTestPlan();
   if (testPlan == NULL)
   {
      ErrorMessage("There is no test plan!!!\n\nClose and restart probe placement.", "Probe Placement");
      return;
   }

   GetDlgItem(IDC_PROCESS)->EnableWindow(FALSE);

   if (!m_targetPriorityDlg.UpdateTestPlan()) return;
   if (!m_probesDlg.UpdateTestPlan()) return;
   if (!m_kelvinPowerInjectionDlg.UpdateTestPlan()) return;
   if (!m_preconditionsDlg.UpdateTestPlan()) return;
   if (!m_netConditionsDlg.UpdateTestPlan()) return;
   if (!m_optionsDlg.UpdateTestPlan()) return;

   CWaitCursor cursor;

   // do kelvin analysis
   int netAffectedCount = doKelvinAnalysis(m_pDoc, testPlan->GetUseResistor(), testPlan->GetResistorValue(),
      testPlan->GetUseCapacitor(), testPlan->GetCapacitorValue(), testPlan->GetUseInductor());

   CProbePlacementSolution *pSolution = m_DFTSolution.CreateProbePlacementSolution(m_pDoc);
   pSolution->SetCurrentTestplan(m_DFTSolution.GetTestPlan());
   pSolution->ShowProgress();
   if (pSolution->RunProbePlacement() < 0)
   {
      ErrorMessage(pSolution->GetErrorMessage(), "Probe Placement", MB_OK);
      pSolution->HideProgress();
      return;
   }
   pSolution->HideProgress();

   int topPlaced = 0, botPlaced = 0;
   int totUnplaced = 0;
   int forcedInactiveSide;
   int netsInvolved = 0, netsFullyProbed = 0, netsPartiallyProbed = 0, netsNotProbed = 0;
   CProbeCountArray countArray;
   pSolution->GetProbeCounts(topPlaced, botPlaced, totUnplaced, forcedInactiveSide, countArray, netsInvolved, netsFullyProbed, netsPartiallyProbed, netsNotProbed);


   // RESET the Results Grid
   m_ppResultsGrid.RemoveAllRows();
   m_ppResultsGrid.SetNumberCols(0);
   m_ppResultsGrid.DefineFixedColumns();

   // FILL the Results Grid
   // Overall result columns and rows.
   // Top and Bottom show Placed, leave Unplaced blank, show Placed only if side was used.
   // Total shows Placed and Unplaced, not conditional.

   // In result counts we always write non-zero results, even if the side is not enabled.
   // The user may have Forced probes to a side that is not enabled in Options.
   // Write the dash only if the count is zero and the side is disabled.

   // Top
   if (topPlaced > 0 || testPlan->CanProbeTopSide())
      m_ppResultsGrid.QuickSetNumber(m_ppResultsGrid.ColIndxPlaced(), m_ppResultsGrid.RowIndxTop(), topPlaced);
   else
      m_ppResultsGrid.QuickSetText(m_ppResultsGrid.ColIndxPlaced(),  m_ppResultsGrid.RowIndxTop(), "-");

   // Bottom
   if (botPlaced > 0 || testPlan->CanProbeBotSide())
      m_ppResultsGrid.QuickSetNumber(m_ppResultsGrid.ColIndxPlaced(), m_ppResultsGrid.RowIndxBot(), botPlaced);
   else
      m_ppResultsGrid.QuickSetText(m_ppResultsGrid.ColIndxPlaced(),  m_ppResultsGrid.RowIndxBot(), "-");

   // Total
   m_ppResultsGrid.QuickSetNumber(m_ppResultsGrid.ColIndxPlaced(),   m_ppResultsGrid.RowIndxTot(), topPlaced+botPlaced);
   m_ppResultsGrid.QuickSetNumber(m_ppResultsGrid.ColIndxUnplaced(), m_ppResultsGrid.RowIndxTot(), totUnplaced);


   // Per probe size columns
   for (int i=0; i<countArray.GetCount(); i++)
   {
      CProbeCount &probeCount = countArray.GetAt(i);

      int trColIndx = m_ppResultsGrid.DefineProbeSizeColumns(probeCount.GetProbeName());
      int pirColIndx = trColIndx + 1;

      // Top
      if (probeCount.GetTopUsedFlag())
      {
         m_ppResultsGrid.QuickSetNumber(trColIndx,  m_ppResultsGrid.RowIndxTop(), probeCount.GetTopTestResourceCount());
         m_ppResultsGrid.QuickSetNumber(pirColIndx, m_ppResultsGrid.RowIndxTop(), probeCount.GetTopPowerInjectionResourceCount());
      }
      else
      {
         m_ppResultsGrid.QuickSetText(trColIndx,  m_ppResultsGrid.RowIndxTop(), "-");
         m_ppResultsGrid.QuickSetText(trColIndx,  m_ppResultsGrid.RowIndxTop(), "-");
      }

      // Bottom
      if (probeCount.GetBottomUsedFlag())
      {
         m_ppResultsGrid.QuickSetNumber(trColIndx,  m_ppResultsGrid.RowIndxBot(), probeCount.GetBottomTestResourceCount());
         m_ppResultsGrid.QuickSetNumber(pirColIndx, m_ppResultsGrid.RowIndxBot(), probeCount.GetBottomPowerInjectionResourceCount());
      }
      else
      {
         m_ppResultsGrid.QuickSetText(trColIndx,  m_ppResultsGrid.RowIndxBot(), "-");
         m_ppResultsGrid.QuickSetText(trColIndx,  m_ppResultsGrid.RowIndxBot(), "-");
      }

      // Total
      if (probeCount.GetTopUsedFlag() || probeCount.GetBottomUsedFlag())
      {
         m_ppResultsGrid.QuickSetNumber(trColIndx,  m_ppResultsGrid.RowIndxTot(), probeCount.GetTotalTestResources());
         m_ppResultsGrid.QuickSetNumber(pirColIndx, m_ppResultsGrid.RowIndxTot(), probeCount.GetTotalPowerInjectionResources());
      }
      else
      {
         m_ppResultsGrid.QuickSetText(trColIndx,  m_ppResultsGrid.RowIndxTot(), "-");
         m_ppResultsGrid.QuickSetText(trColIndx,  m_ppResultsGrid.RowIndxTot(), "-");
      }
   }

   // Set the column widths. 
   CRect rcGridWindow;
   GetDlgItem( IDC_PPResultsGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_ppResultsGrid.SizeToFit(rcGridWindow);

   m_ppResultsGrid.RedrawAll();

   // If a side count is non-zero, then leave the background white, even if the side is not enabled in Options tab.
   // User may have forced probes to otherwise inactive side, and it is the actually placed probes that matter here,
   // not the control settings. A side gets marked gray only if the Options say to not use it and we did in fact
   // not use it.


   // Reset all rows to white
   m_ppResultsGrid.SetRowBackColor(0, m_ppResultsGrid.m_whiteBackgroundColor);
   m_ppResultsGrid.SetRowBackColor(1, m_ppResultsGrid.m_whiteBackgroundColor);
   m_ppResultsGrid.SetRowBackColor(2, m_ppResultsGrid.m_whiteBackgroundColor);

   // Selectively set disabled/not-used rows to gray
   if (topPlaced == 0 && !m_DFTSolution.GetTestPlan()->CanProbeTopSide())
      m_ppResultsGrid.SetRowBackColor(0, m_ppResultsGrid.m_lightGrayBackgroundColor);
   if (botPlaced == 0 && !m_DFTSolution.GetTestPlan()->CanProbeBotSide())
      m_ppResultsGrid.SetRowBackColor(1, m_ppResultsGrid.m_lightGrayBackgroundColor);

   for (int row=0; row<m_ppResultsGrid.GetNumberRows(); row++)
   {
      for (int col=0; col<m_ppResultsGrid.GetNumberCols(); col++)
      {
         CString text( m_ppResultsGrid.QuickGetText(col, row) );
         if (text == "-")
            m_ppResultsGrid.QuickSetBackColor(col, row, m_ppResultsGrid.m_lightGrayBackgroundColor);
      }
   }

   pSolution->RenumberProbes();
   testPlan->ResetModified();

   pSolution->CreateReport(GetReportFilename(), m_DFTSolution.GetAccessAnalysisSolution());
   GetDlgItem(IDOK)->EnableWindow(TRUE);
   GetDlgItem(IDC_DisplayReport)->EnableWindow(TRUE);
}

void CProbePlacementDlg::OnBnClickedSaveTestplan()
{
   CString dirPath = CAppRegistrySetting("DFT", "Testplan Directory").Get();

   if (dirPath.IsEmpty())
   {
      dirPath = CBackwardCompatRegistry().DFT_GetTestplanDir();

      // Update from old obsolete key to current key
      if (!dirPath.IsEmpty())
      {
         CAppRegistrySetting("DFT", "Testplan Directory").Set(dirPath);
      }
   }

   CString dir = (CString)dirPath + "\\*.pln";
   CFileDialog fileDlg(FALSE, "*.pln", dir, NULL, "DFT Testplan File (*.pln)|*.pln||", this);
   if (fileDlg.DoModal() == IDOK)
   {
      UpdateTestplan();
      if (m_DFTSolution.GetTestPlan()->SaveToFile(fileDlg.GetFileName(), m_pDoc->getSettings().getPageUnits()) != 0)
         ErrorMessage("Could not create testplan file!", "Probe Placement", MB_OK);
   }
}

void CProbePlacementDlg::OnBnClickedDisplayReport()
{
   CString reportFilePath( GetReportFilename() );

   if (! reportFilePath.IsEmpty())
   {
      if (fileExists(reportFilePath))
      {
         ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",reportFilePath,NULL,NULL,SW_SHOW);
      }
   }
}

void CProbePlacementDlg::UpdateTestplan()
{
   m_targetPriorityDlg.UpdateTestPlan();
   m_probesDlg.UpdateTestPlan();
   m_kelvinPowerInjectionDlg.UpdateTestPlan();
   m_preconditionsDlg.UpdateTestPlan();
   m_netConditionsDlg.UpdateTestPlan();
   m_optionsDlg.UpdateTestPlan();
}

void CProbePlacementDlg::OnBnClickedSaveTestplanDebug()
{
   if (m_DFTSolution.GetProbePlacementSolution())
      m_DFTSolution.GetProbePlacementSolution()->DrawOutlines();

   CFileDialog dlg(FALSE, NULL, "DebugTestPlan.txt", NULL, "All Files (*.*)|*.*||", this);
   if (dlg.DoModal() == IDCANCEL)
      return;

   UpdateTestplan();
   if (m_DFTSolution.DumpToFile(dlg.GetPathName()))
      ErrorMessage("File successfully created!!");
   else
      ErrorMessage("File not created!!");
}

void CProbePlacementDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   CWnd *pWnd = GetDlgItem(IDC_SAVE_TESTPLAN_DEBUG);

   if (pWnd->IsWindowVisible())
      pWnd->ShowWindow(SW_HIDE);
   else
      pWnd->ShowWindow(SW_SHOW);

   //CDialog::OnLButtonDblClk(nFlags, point);
}

void CProbePlacementDlg::OnOK()
{
   DeleteAllProbes(m_pDoc, m_pFile);
   int numPlaced = m_DFTSolution.GetProbePlacementSolution()->PlaceProbesOnBoard();

   CString msg;
   msg.Format("%d probes placed.", numPlaced);

   CProbePlacementExitDlg exitdlg(msg, this);
   bool presentProbeStylesDlg = (exitdlg.DoModal() == IDC_PROBE_STYLES);

   m_DFTSolution.GetTestPlan()->GetNetConditions().UpdateNetsInFile(m_pFile, m_pDoc);

   /*CDFTSolution* dftSolution = m_pDoc->GetCurrentDFTSolution(*m_pFile);
   if (dftSolution != NULL)
      dftSolution->CopyDFTSolution(m_DFTSolution);
*/   
   CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
   frame->getNavigator().setDoc(m_pDoc);

   if (presentProbeStylesDlg)
   {
      CCEtoODBView* view = getActiveView();
      if (view != NULL)
         view->PostMessage(WM_COMMAND, ID_DFTANALYSIS_PROBESTYLESMATRIX);
   }

   CDialog::OnOK();
}

BEGIN_EVENTSINK_MAP(CProbePlacementDlg, CDialog)
END_EVENTSINK_MAP()


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CTargetPriorityPP dialog
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTargetPriorityPP, CPropertyPage)
CTargetPriorityPP::CTargetPriorityPP()
   : CPropertyPage(CTargetPriorityPP::IDD)
{
   m_pTestPlan = NULL;
   m_pTargetPriority = NULL;
   m_pParent = NULL;
}

CTargetPriorityPP::~CTargetPriorityPP()
{
}

void CTargetPriorityPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTargetPriorityPP, CPropertyPage)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TOP, OnDeltaposSpinTop)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BOT, OnDeltaposSpinBot)
END_MESSAGE_MAP()


// CTargetPriorityPP message handlers

BOOL CTargetPriorityPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_targetGridTop.AttachGrid(this, IDC_ULTGRID_TOP);
   m_targetGridBot.AttachGrid(this, IDC_ULTGRID_BOT);

   CSpinButtonCtrl *topSpinner = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TOP);
   topSpinner->SetBuddy(&m_targetGridTop);

   CSpinButtonCtrl *botSpinner = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BOT);
   botSpinner->SetBuddy(&m_targetGridBot);

   fillGrids();
   UpdateUI();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CTargetPriorityPP::fillGrids()
{
   CDFTTargetPriority &tPriority =  m_pTestPlan->GetTargetPriority();

   for (int i=0; i<tPriority.GetCount_TopTargets(); i++)
   {
      CDFTTarget *target = tPriority.GetAt_TopTargets(i);
      m_targetGridTop.AddRowData(target->GetEnabledFlag(), target->GetName());
   }

   for (int i=0; i<tPriority.GetCount_BotTargets(); i++)
   {
      CDFTTarget *target = tPriority.GetAt_BotTargets(i);
      m_targetGridBot.AddRowData(target->GetEnabledFlag(), target->GetName());
   }
}

void CTargetPriorityPP::OnDeltaposSpinTop(NMHDR *pNMHDR, LRESULT *pResult)
{
   OnDeltaposSpinHandler(true, pNMHDR, pResult);
}
void CTargetPriorityPP::OnDeltaposSpinBot(NMHDR *pNMHDR, LRESULT *pResult)
{
   OnDeltaposSpinHandler(false, pNMHDR, pResult);
}

void CTargetPriorityPP::OnDeltaposSpinHandler(bool isTop, NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
   
   CSpinButtonCtrl spinner;
   spinner.Attach(pNMUpDown->hdr.hwndFrom);
   spinner.Detach();

   CTargetPriorityGrid *targPrioGrid = NULL;
   if (isTop)
      targPrioGrid = &this->m_targetGridTop;
   else
      targPrioGrid = &this->m_targetGridBot;

   int curSel = targPrioGrid->GetCurrentRow();
   if (curSel < 0)
      return;

   *pResult = 0;

   bool isChecked = false;
   CString targetName;
   targPrioGrid->GetRowData(curSel, isChecked, targetName);

   if (pNMUpDown->iDelta > 0) // clicked down
   {
      // can't move down any more
      if (targPrioGrid->GetNumberRows()-1 == curSel)
         return;

      // move the selected item down
      int otherIndx = curSel+1;
      bool otherIsChecked = false;
      CString otherName;
      targPrioGrid->GetRowData(otherIndx, otherIsChecked, otherName);

      targPrioGrid->SetRowData(curSel, otherIsChecked, otherName);
      targPrioGrid->SetRowData(otherIndx, isChecked, targetName);

      CDFTTargetPriority &tPriority = m_pTestPlan->GetTargetPriority();
      if (isTop)
         tPriority.Swap_TopTargets(curSel, curSel+1);
      else
         tPriority.Swap_BotTargets(curSel, curSel+1);
   }
   else
   {
      // can't move up any more
      if (curSel == 0)
         return;

      // move the selected item up
      int otherIndx = curSel - 1;
      bool otherIsChecked = false;
      CString otherName;
      targPrioGrid->GetRowData(otherIndx, otherIsChecked, otherName);

      targPrioGrid->SetRowData(curSel, otherIsChecked, otherName);
      targPrioGrid->SetRowData(otherIndx, isChecked, targetName);

      CDFTTargetPriority &tPriority = m_pTestPlan->GetTargetPriority();
      if (isTop)
         tPriority.Swap_TopTargets(curSel, curSel-1);
      else
         tPriority.Swap_BotTargets(curSel, curSel-1);
   }

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED, 0);
}


BEGIN_EVENTSINK_MAP(CTargetPriorityPP, CPropertyPage)
END_EVENTSINK_MAP()

bool CTargetPriorityPP::UpdateTestPlan()
{
   if (m_hWnd == NULL)
      return true;

   UpdateData();

   m_pTargetPriority->RemoveAll_TopTargets();

   for (int i = 0; i < m_targetGridTop.GetNumberRows(); i++)
   {
      bool isChecked = false;
      CString targetName;
      this->m_targetGridTop.GetRowData(i, isChecked, targetName);

      m_pTargetPriority->InsertAt_TopTargets(i, isChecked, targetName);
   }

   m_pTargetPriority->RemoveAll_BotTargets();
   for (int i = 0; i<m_targetGridBot.GetNumberRows(); i++)
   {
      bool isChecked = false;
      CString targetName;
      this->m_targetGridBot.GetRowData(i, isChecked, targetName);

      m_pTargetPriority->InsertAt_BotTargets(i, isChecked, targetName);
   }

   return true;
}

void CTargetPriorityPP::UpdateUI()
{
   if (m_hWnd == NULL)
      return;

   COLORREF go = RGB(255,255,255);      // Enabled color
   COLORREF nogo = RGB(189, 189, 189);  // Disabled color

   for (int i = 0; i < m_targetGridTop.GetNumberRows(); i++)
   {
      m_targetGridTop.SetCellReadOnly(0, i, !m_pTestPlan->CanProbeTopSide());
      m_targetGridTop.QuickSetBackColor(0, i, m_pTestPlan->CanProbeTopSide()?go:nogo);
      m_targetGridTop.QuickSetBackColor(1, i, m_pTestPlan->CanProbeTopSide()?go:nogo);
   }
   GetDlgItem(IDC_STATIC_TOP_LABEL)->EnableWindow(m_pTestPlan->CanProbeTopSide());

   for (int i = 0; i < m_targetGridBot.GetNumberRows(); i++)
   {
      m_targetGridBot.SetCellReadOnly(0, i, !m_pTestPlan->CanProbeBotSide());
      m_targetGridBot.QuickSetBackColor(0, i, m_pTestPlan->CanProbeBotSide()?go:nogo);
      m_targetGridBot.QuickSetBackColor(1, i, m_pTestPlan->CanProbeBotSide()?go:nogo);
   }
   GetDlgItem(IDC_STATIC_BOTTOM_LABEL)->EnableWindow(m_pTestPlan->CanProbeBotSide());
}

//---------------------------------------------------

void CTargetPriorityGrid::OnSetup()
{
   SetNumberCols(2);
   SetNumberRows(0);

   CUGCell cell;

   GetColDefault(0, &cell); 
   cell.SetCellType(UGCT_CHECKBOX);  // To make it a checkbox cell
   cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
   SetColDefault(0, &cell);

   SetColWidth(-1, 0); // get rid of "row heading"
   SetRowHeight(-1, 0); // get rid of column headings.

}

void CTargetPriorityGrid::AddRowData(bool checked, CString name)
{
   int rowIndx = GetNumberRows();

   SetNumberRows(rowIndx + 1);
   SetCellReadOnly(1, rowIndx, true);

   SetRowData(rowIndx, checked, name);
}

void CTargetPriorityGrid::SetRowData(int row, bool checked, CString name)
{
   if(row > -1 && row < GetNumberRows() /*&& col > -1 && col < GetNumberCols()*/)
   {
      CUGCell cell;
      GetCell(0, row, &cell);
      cell.SetBool(checked);
      SetCell(0, row, &cell);

      QuickSetText(1, row, name);  
      BestFit(0, 1, 0, UG_BESTFIT_TOPHEADINGS);
   }
}

void CTargetPriorityGrid::GetRowData(int row, bool &checked, CString &name)
{
   if(row > -1 && row < GetNumberRows() /*&& col > -1 && col < GetNumberCols()*/)
   {
      // No QuickSetBool, so do it the long way.
      CUGCell cell;
      GetCell(0, row, &cell);
      checked = cell.GetBool()?true:false;
     
      QuickGetText(1, row, &name);
      //BestFit(0, m_ColumnSize - 1, 0, UG_BESTFIT_TOPHEADINGS);
      int jj = 0;
   }
}

void CTargetPriorityGrid::SetCellReadOnly(int col, int row, bool readonly)
{
   CUGCell cell;
   GetCell(col, row, &cell);
   cell.SetReadOnly(readonly);   
   SetCell(col, row, &cell);  
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CProbesPP dialog
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define PROBES_FIXED_ROWS                 2

#define PROBES_COLS_USE                   0
#define PROBES_COLS_NAME                  1
#define PROBES_COLS_DIAMETER              2
#define PROBES_COLS_DRILLSIZE					3
#define PROBES_COLS_TEXTSIZE              4
#define PROBES_COLS_COST                  5
#define PROBES_COLS_COMPDIST_ON           6
#define PROBES_COLS_COMPDIST_DISTANCE     7
#define PROBES_COLS_COUNT                 8  // count of columns, not a column type

IMPLEMENT_DYNAMIC(CProbesPP, CPropertyPage)
CProbesPP::CProbesPP()
   : CPropertyPage(CProbesPP::IDD)
   , m_bNeverMirrorRefname(FALSE)
{
   m_pTestPlan = NULL;
   m_pProbes = NULL;
   m_pParent = NULL;

   m_pDoc = NULL;
}

CProbesPP::CProbesPP(CCEtoODBDoc *Doc)
   : CPropertyPage(CProbesPP::IDD)
   , m_bNeverMirrorRefname(FALSE)
{
   m_pTestPlan = NULL;
   m_pProbes = NULL;
   m_pParent = NULL;

   m_pDoc = Doc;
}

CProbesPP::~CProbesPP()
{
}

void CProbesPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_CHK_NEVER_MIRROR_REFNAME, m_bNeverMirrorRefname);
}


BEGIN_MESSAGE_MAP(CProbesPP, CPropertyPage)
   ON_BN_CLICKED(IDC_ADD_TOP, OnBnClickedAddTop)
   ON_BN_CLICKED(IDC_REMOVE_TOP, OnBnClickedRemoveTop)
   ON_BN_CLICKED(IDC_ADD_BOTTOM, OnBnClickedAddBottom)
   ON_BN_CLICKED(IDC_REMOVE_BOTTOM, OnBnClickedRemoveBottom)
END_MESSAGE_MAP()


// CProbesPP message handlers

BOOL CProbesPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_bNeverMirrorRefname = m_pTestPlan->GetNeverMirrorRefname();

   int decimals = GetDecimals(m_pDoc->getSettings().getPageUnits());

   // TOP
   m_topProbesGrid.AttachGrid(this, IDC_TopProbesGridStatic, false);  // false turns off bold in heading font
   
   CRect rcGridWindow;
   GetDlgItem( IDC_TopProbesGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_topProbesGrid.SizeToFit(rcGridWindow);

   FillGrid(m_topProbesGrid, testSurfaceTop);
   updateRowStates(m_topProbesGrid);

   // BOTTOM
   m_botProbesGrid.AttachGrid(this, IDC_BotProbesGridStatic, false);  // false turns off bold in heading font
   
   //CRect rcGridWindow;
   GetDlgItem( IDC_BotProbesGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_botProbesGrid.SizeToFit(rcGridWindow);

   FillGrid(m_botProbesGrid, testSurfaceBottom);
   updateRowStates(m_botProbesGrid);


   UpdateUI();

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CProbesPP::FillGrid(CProbesGrid &probesGrid, ETestSurface surface)
{
   probesGrid.RemoveAllRows();
   PageUnitsTag pageUnits = m_pDoc->getSettings().getPageUnits();
   int decimals = GetDecimals(pageUnits);

   POSITION pos = (surface == testSurfaceTop) ? m_pProbes->GetHeadPosition_TopProbes() : m_pProbes->GetHeadPosition_BotProbes();
   while (pos)
   {
      CDFTProbeTemplate *probeTemplate = (surface == testSurfaceTop) ? m_pProbes->GetNext_TopProbes(pos) : m_pProbes->GetNext_BotProbes(pos);
      probesGrid.AddProbe(probeTemplate, decimals, pageUnits);
   }

   updateRowStates(probesGrid);
}

void CProbesPP::updateRowStates(CProbesGrid &probesGrid, int Row)
{
   for (int rowIndx = 0; rowIndx < probesGrid.GetNumberRows(); rowIndx++)
   {
      bool checked = true;
      COLORREF backClr = (checked) ? RGB(255, 255, 255) : RGB(189, 189, 189);
      probesGrid.SetRowBackColor(rowIndx, backClr);
   }
}

void CProbesPP::OnBnClickedAddTop()
{
   m_topProbesGrid.AddBlankProbe();

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CProbesPP::OnBnClickedRemoveTop()
{
   m_topProbesGrid.DeleteMostRecentRow();

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CProbesPP::OnBnClickedAddBottom()
{
   m_botProbesGrid.AddBlankProbe();

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CProbesPP::OnBnClickedRemoveBottom()
{
   m_botProbesGrid.DeleteMostRecentRow();

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

BEGIN_EVENTSINK_MAP(CProbesPP, CPropertyPage)
END_EVENTSINK_MAP()


bool CProbesPP::UpdateTestPlan()
{
   if (m_hWnd == NULL)
      return true;

   UpdateData();

   m_pTestPlan->SetNeverMirrorRefname(m_bNeverMirrorRefname?true:false);

   CUGCell cell;

   m_pProbes->RemoveAll_TopProbes();
   for (int rowIndx = 0; rowIndx < m_topProbesGrid.GetNumberRows(); rowIndx++)
   {
      m_topProbesGrid.GetCell(PROBES_COLS_USE, rowIndx, &cell);
      bool useProbe = cell.GetBool()?true:false;
      CString probeName( m_topProbesGrid.QuickGetText(PROBES_COLS_NAME, rowIndx) );
      CString diameter( m_topProbesGrid.QuickGetText(PROBES_COLS_DIAMETER, rowIndx) );
      CString drillSize( m_topProbesGrid.QuickGetText(PROBES_COLS_DRILLSIZE, rowIndx) );
		CString textSize( m_topProbesGrid.QuickGetText(PROBES_COLS_TEXTSIZE, rowIndx) );
		CString cost( m_topProbesGrid.QuickGetText(PROBES_COLS_COST, rowIndx) );
      m_topProbesGrid.GetCell(PROBES_COLS_COMPDIST_ON, rowIndx, &cell);
      bool useCompDist = cell.GetBool()?true:false;
      CString compDist( m_topProbesGrid.QuickGetText(PROBES_COLS_COMPDIST_DISTANCE, rowIndx) );

      if (probeName.IsEmpty() || diameter.IsEmpty() || cost.IsEmpty() || compDist.IsEmpty())
      {
         m_pParent->SendMessage(IPP_SETPAGE, 0, 1);
         ErrorMessage("All fields for each probe must be specified.", "Probe Placement");
         return false;
      }
      else
         m_pProbes->Add_TopProbes(useProbe, probeName,
			atof(diameter) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils), 
			atof(drillSize) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils), 
			atof(textSize) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils), 
			atoi(cost), useCompDist, atof(compDist) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils));
   }

   m_pProbes->RemoveAll_BotProbes();
   for (int rowIndx = 0; rowIndx < m_botProbesGrid.GetNumberRows(); rowIndx++)
   {
      m_botProbesGrid.GetCell(PROBES_COLS_USE, rowIndx, &cell);
      bool useProbe = cell.GetBool()?true:false;
      CString probeName( m_botProbesGrid.QuickGetText(PROBES_COLS_NAME, rowIndx) );
      CString diameter( m_botProbesGrid.QuickGetText(PROBES_COLS_DIAMETER, rowIndx) );
      CString drillSize( m_botProbesGrid.QuickGetText(PROBES_COLS_DRILLSIZE, rowIndx) );
		CString textSize( m_botProbesGrid.QuickGetText(PROBES_COLS_TEXTSIZE, rowIndx) );
		CString cost( m_botProbesGrid.QuickGetText(PROBES_COLS_COST, rowIndx) );
      m_botProbesGrid.GetCell(PROBES_COLS_COMPDIST_ON, rowIndx, &cell);
      bool useCompDist = cell.GetBool()?true:false;
      CString compDist( m_botProbesGrid.QuickGetText(PROBES_COLS_COMPDIST_DISTANCE, rowIndx) );

      int ii = atoi(compDist);
      if (probeName.IsEmpty() || diameter.IsEmpty() || cost.IsEmpty() || compDist.IsEmpty())
      {
         m_pParent->SendMessage(IPP_SETPAGE, 0, 1);
         ErrorMessage("All fields for each probe must be specified.", "Probe Placement");
         return false;
      }
      else
         m_pProbes->Add_BotProbes(useProbe, probeName,
			atof(diameter) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils),
			atof(drillSize) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils), 
			atof(textSize) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils), 
			atoi(cost), useCompDist, atof(compDist) * Units_Factor(m_pDoc->getSettings().getPageUnits(), pageUnitsMils));
   }

   return true;
}

void CProbesPP::UpdateUI()
{
   if (m_hWnd == NULL)
      return;

   bool enableTop = (m_pTestPlan->CanProbeTopSide() && m_pTestPlan->GetTesterType()==DFT_TESTER_FIXTURE);
   bool enableBot = (m_pTestPlan->CanProbeBotSide() && m_pTestPlan->GetTesterType()==DFT_TESTER_FIXTURE);

   long topBackClr = (enableTop)?RGB(255, 255, 255):RGB(189, 189, 189);
   long botBackClr = (enableBot)?RGB(255, 255, 255):RGB(189, 189, 189);


   // TOP
   for (int rowIndx = 0; rowIndx < m_topProbesGrid.GetNumberRows(); rowIndx++)
   {
      m_topProbesGrid.SetRowBackColor(rowIndx, topBackClr);
   }
   
   m_topProbesGrid.EnableWindow(enableTop);
   GetDlgItem(IDC_STATIC_TOP_LABEL)->EnableWindow(enableTop);
   GetDlgItem(IDC_ADD_TOP)->EnableWindow(enableTop);
   GetDlgItem(IDC_REMOVE_TOP)->EnableWindow(enableTop);


   // BOTTOM
   for (int rowIndx = 0; rowIndx < m_botProbesGrid.GetNumberRows(); rowIndx++)
   {
      m_botProbesGrid.SetRowBackColor(rowIndx, botBackClr);
   }

   m_botProbesGrid.EnableWindow(enableBot);
   GetDlgItem(IDC_STATIC_BOTTOM_LABEL)->EnableWindow(enableBot);
   GetDlgItem(IDC_ADD_BOTTOM)->EnableWindow(enableBot);
   GetDlgItem(IDC_REMOVE_BOTTOM)->EnableWindow(enableBot);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CKelvinPowerInjPP dialog
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CKelvinPowerInjPP, CPropertyPage)
CKelvinPowerInjPP::CKelvinPowerInjPP()
   : CPropertyPage(CKelvinPowerInjPP::IDD)
   , m_useResistor(FALSE)
   , m_useCapacitor(FALSE)
   , m_useInductor(FALSE)
   , m_resistorValue(0)
   , m_capacitorValue(0)
   , m_probesPerPowerRail(0)
   , m_probesPerConn(0)
   , m_probesPerNumNets(0)
   , m_powerInjectionSetting(FALSE)
{
   m_pTestPlan = NULL;
   m_pParent = NULL;
}

CKelvinPowerInjPP::~CKelvinPowerInjPP()
{
}

void CKelvinPowerInjPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_RESISTOR_CHK, m_useResistor);
   DDX_Check(pDX, IDC_CAPACITOR_CHK, m_useCapacitor);
   DDX_Check(pDX, IDC_INDUCTOR_CHK, m_useInductor);
   DDX_Text(pDX, IDC_RESISTOR_VAL, m_resistorValue);
   DDX_Text(pDX, IDC_CAPACITOR_VAL, m_capacitorValue);
   // DDV_MinMaxInt(pDX, m_probesPerNet, 0, 20);
   DDX_Text(pDX, IDC_PROBES_PER_POWERRAIL, m_probesPerPowerRail);
   DDX_Text(pDX, IDC_PROBES_PER_CONN, m_probesPerConn);
   DDX_Text(pDX, IDC_PROBES_PER_NUMNET, m_probesPerNumNets);
   DDX_Radio(pDX, IDC_RADIO_POWER_INJ, m_powerInjectionSetting);
}


BEGIN_MESSAGE_MAP(CKelvinPowerInjPP, CPropertyPage)
   ON_BN_CLICKED(IDC_RESISTOR_CHK, OnBnClickedResistorChk)
   ON_BN_CLICKED(IDC_CAPACITOR_CHK, OnBnClickedCapacitorChk)
   ON_BN_CLICKED(IDC_INDUCTOR_CHK, OnBnClickedInductorChk)
   ON_BN_CLICKED(IDC_RADIO_POWER_INJ, OnBnClickedRadioPowerInj)
   ON_BN_CLICKED(IDC_RADIO_PER_POWERRAIL, OnBnClickedRadioPowerInj)
   ON_BN_CLICKED(IDC_RADIO_PER_CONN, OnBnClickedRadioPowerInj)
   ON_BN_CLICKED(IDC_RADIO_PER_NUMNET, OnBnClickedRadioPowerInj)
   ON_EN_CHANGE(IDC_PROBES_PER_POWERRAIL, OnEnChangeProbesPerPowerrail)
   ON_EN_CHANGE(IDC_PROBES_PER_CONN, OnEnChangeProbesPerConn)
   ON_EN_CHANGE(IDC_PROBES_PER_NUMNET, OnEnChangeProbesPerNumnet)
   ON_EN_CHANGE(IDC_RESISTOR_VAL, OnEnChangeResistorVal)
   ON_EN_CHANGE(IDC_CAPACITOR_VAL, OnEnChangeCapacitorVal)
END_MESSAGE_MAP()


// CKelvinPowerInjPP message handlers

BOOL CKelvinPowerInjPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   if (m_pTestPlan != NULL)
   {
      m_useResistor = m_pTestPlan->GetUseResistor();
      m_useCapacitor = m_pTestPlan->GetUseCapacitor();
      m_useInductor = m_pTestPlan->GetUseInductor();
      m_resistorValue = m_pTestPlan->GetResistorValue();
      m_capacitorValue = m_pTestPlan->GetCapacitorValue();

      m_powerInjectionSetting = m_pTestPlan->GetPowerInjectionUsage();
      switch (m_powerInjectionSetting)
      {
      case 1:  // Probes per Power Rail
         m_probesPerPowerRail = m_pTestPlan->GetPowerInjectionValue();
         break;
      case 2:  // Probes per Number of Connections per Power Rail
         m_probesPerConn = m_pTestPlan->GetPowerInjectionValue();
         break;
      case 3:  // Probes per Number of Nets per Power Rail
         m_probesPerNumNets = m_pTestPlan->GetPowerInjectionValue();
         break;
      case 0:  // No Power Injection
      default:
         break;
      }
   }

   GetDlgItem(IDC_RESISTOR_VAL)->EnableWindow(m_useResistor);
   GetDlgItem(IDC_RESISTOR_UNIT)->EnableWindow(m_useResistor);
   GetDlgItem(IDC_CAPACITOR_VAL)->EnableWindow(m_useCapacitor);
   GetDlgItem(IDC_CAPACITOR_UNIT)->EnableWindow(m_useCapacitor);

   GetDlgItem(IDC_PROBES_PER_POWERRAIL)->EnableWindow(FALSE);
   GetDlgItem(IDC_PROBES_PER_CONN)->EnableWindow(FALSE);
   GetDlgItem(IDC_PROBES_PER_NUMNET)->EnableWindow(FALSE);

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CKelvinPowerInjPP::OnBnClickedResistorChk()
{
   UpdateData(TRUE);

   GetDlgItem(IDC_RESISTOR_VAL)->EnableWindow(m_useResistor);
   GetDlgItem(IDC_RESISTOR_UNIT)->EnableWindow(m_useResistor);

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnBnClickedCapacitorChk()
{
   UpdateData(TRUE);

   GetDlgItem(IDC_CAPACITOR_VAL)->EnableWindow(m_useCapacitor);
   GetDlgItem(IDC_CAPACITOR_UNIT)->EnableWindow(m_useCapacitor);

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnBnClickedInductorChk()
{
   UpdateData(TRUE);

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnBnClickedRadioPowerInj()
{
   UpdateData(TRUE);

   GetDlgItem(IDC_PROBES_PER_POWERRAIL)->EnableWindow(FALSE);
   GetDlgItem(IDC_PROBES_PER_CONN)->EnableWindow(FALSE);
   GetDlgItem(IDC_PROBES_PER_NUMNET)->EnableWindow(FALSE);

   switch (m_powerInjectionSetting)
   {
   case 1:  // Probes per Power Rail
      GetDlgItem(IDC_PROBES_PER_POWERRAIL)->EnableWindow(TRUE);
      break;
   case 2:  // Probes per Number of Connections per Power Rail
      GetDlgItem(IDC_PROBES_PER_CONN)->EnableWindow(TRUE);
      break;
   case 3:  // Probes per Number of Nets per Power Rail
      GetDlgItem(IDC_PROBES_PER_NUMNET)->EnableWindow(TRUE);
      break;
   case 0:  // No Power Injection
   default:
      break;
   }

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnEnChangeResistorVal()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnEnChangeCapacitorVal()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnEnChangeProbesPerPowerrail()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnEnChangeProbesPerConn()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void CKelvinPowerInjPP::OnEnChangeProbesPerNumnet()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

bool CKelvinPowerInjPP::UpdateTestPlan()
{
   if (m_hWnd == NULL)
      return true;

   UpdateData();

   m_pTestPlan->SetUseResistor(m_useResistor);
   m_pTestPlan->SetUseCapacitor(m_useCapacitor);
   m_pTestPlan->SetUseInductor(m_useInductor);
   m_pTestPlan->SetPowerInjectionUsage(m_powerInjectionSetting);
   m_pTestPlan->SetResistorValue(m_resistorValue);
   m_pTestPlan->SetCapacitorValue(m_capacitorValue);
   switch (m_powerInjectionSetting)
   {
   case 1:  // Probes per Power Rail
      m_pTestPlan->SetPowerInjectionValue(m_probesPerPowerRail);
      break;
   case 2:  // Probes per Number of Connections per Power Rail
      if (m_probesPerConn <= 0)
      {
         m_pParent->SendMessage(IPP_SETPAGE, 0, 2);
         GetDlgItem(IDC_PROBES_PER_CONN)->SetFocus();
         ((CEdit*)GetDlgItem(IDC_PROBES_PER_CONN))->SetSel(0, -1);

         ErrorMessage("Power injection value must be greater than zero.", "Probe Placement");
         return false;
      }

      m_pTestPlan->SetPowerInjectionValue(m_probesPerConn);
      break;
   case 3:  // Probes per Number of Nets per Power Rail
      if (m_probesPerNumNets <= 0)
      {
         m_pParent->SendMessage(IPP_SETPAGE, 0, 2);
         GetDlgItem(IDC_PROBES_PER_NUMNET)->SetFocus();
         ((CEdit*)GetDlgItem(IDC_PROBES_PER_NUMNET))->SetSel(0, -1);

         ErrorMessage("Power injection value must be greater than zero.", "Probe Placement");
         return false;
      }

      m_pTestPlan->SetPowerInjectionValue(m_probesPerNumNets);
      break;
   case 0:  // No Power Injection
   default:
      break;
   }

   return true;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CPreconditionsPP dialog
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define FEATURES_FIXED_ROWS 0

IMPLEMENT_DYNAMIC(CPreconditionsPP, CPropertyPage)
CPreconditionsPP::CPreconditionsPP()
   : CPropertyPage(CPreconditionsPP::IDD)
{
   m_pDoc = NULL;
   m_pView = NULL;
   m_pTestPlan = NULL;
   m_pPreconditions = NULL;
   m_pParent = NULL;
   zoomToSelected = true;

}

CPreconditionsPP::CPreconditionsPP(CCEtoODBDoc *Doc)
   : CPropertyPage(CPreconditionsPP::IDD)
{
   m_pTestPlan = NULL;
   m_pPreconditions = NULL;
   m_pParent = NULL;
   zoomToSelected = true;

   m_pDoc = Doc;
   POSITION pos = m_pDoc->GetFirstViewPosition();
   m_pView = (CCEtoODBView*)m_pDoc->GetNextView(pos);
}

CPreconditionsPP::~CPreconditionsPP()
{
}

void CPreconditionsPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPreconditionsPP, CPropertyPage)
   ON_BN_CLICKED(IDC_AVAIL2FORCED, OnBnClickedAvail2forced)
   ON_BN_CLICKED(IDC_AVAIL2NO, OnBnClickedAvail2no)
   ON_BN_CLICKED(IDC_FORCED2AVAIL, OnBnClickedForced2avail)
   ON_BN_CLICKED(IDC_NO2AVAIL, OnBnClickedNo2avail)
END_MESSAGE_MAP()


void CPreconditionsPP::OnBnClickedAvail2forced()
{
   moveSelected(m_ppAvailableFeaturesGrid, m_ppForcedProbesGrid);
}

void CPreconditionsPP::OnBnClickedAvail2no()
{
   moveSelected(m_ppAvailableFeaturesGrid, m_ppNoProbesGrid);
}

void CPreconditionsPP::OnBnClickedForced2avail()
{
   moveSelected(m_ppForcedProbesGrid, m_ppAvailableFeaturesGrid);
}

void CPreconditionsPP::OnBnClickedNo2avail()
{
   moveSelected(m_ppNoProbesGrid, m_ppAvailableFeaturesGrid);
}

// CPreconditionsPP message handlers
BEGIN_EVENTSINK_MAP(CPreconditionsPP, CPropertyPage)
END_EVENTSINK_MAP()



void CPreconditionsPP::moveSelected(CPreconditionsGrid &sourceGrid, CPreconditionsGrid &destGrid)
{
   if (&sourceGrid == &destGrid)
      return;

   sourceGrid.SetPaintMode(FALSE);
   destGrid.SetPaintMode(FALSE);

   // This will get us the values for a single selection.
   int nCol;
   long nRow;
   int nRet = sourceGrid.EnumFirstSelected( &nCol, &nRow );

   // This will get us the values for a multi selection.
   int startSelectedCol;
   long startSelectedRow;
   int endSelectedCol;
   long endSelectedRow;
   bool hasMultiSelect = sourceGrid.GetMultiSelectStart(&startSelectedCol, &startSelectedRow);
   sourceGrid.GetMultiSelectEnd(&endSelectedCol, &endSelectedRow);

   // Now figure out if it was single or multi selection, adjust start/end if needed.
   if (!hasMultiSelect)
   {
      startSelectedCol = endSelectedCol = nCol;
      startSelectedRow = endSelectedRow = nRow;
   }

   if (hasMultiSelect || nRet == UG_SUCCESS)  // Has multi or single selection
   {
      // Possible adjustment to selection. If a comp RefDes only row (no pin name) is selected then
      // we're supposed to move all pins. Expand the selection range, if needed, to include all the pins.
      // Selection has to be contiguous, we're not supporting multi select with holes in the range.
      int adjustedEnd = -1;
      for (long targetRow = min(startSelectedRow, endSelectedRow); targetRow <= max(startSelectedRow, endSelectedRow); targetRow++)
      {
         CDFTFeature *f = (CDFTFeature*)sourceGrid.GetRowData(targetRow);
         if (f != NULL)
         {
            CString pinName( f->GetPin() );
            if (pinName.IsEmpty()) // Is comp RefDes only row
            {
               // Find last pin of this refdes
               int pinIndx = targetRow+1;
               bool keepLooking = true;
               while (pinIndx < sourceGrid.GetNumberRows() && keepLooking)
               {
                  f = (CDFTFeature*)sourceGrid.GetRowData(pinIndx);
                  pinName = (f != NULL) ? f->GetPin() : "";
                  if (pinName.IsEmpty())
                     keepLooking = false;
                  else
                     adjustedEnd = pinIndx;
                  pinIndx++;
               }
            }
         }
      }
      if (adjustedEnd > endSelectedRow)
         endSelectedRow = adjustedEnd;

      // Now copy them from source to dest.
      // Need to move these all over before deleting any rows, otherwise row index gets changed for rows not yet processed.
      for (long targetRow = min(startSelectedRow, endSelectedRow); targetRow <= max(startSelectedRow, endSelectedRow); targetRow++)
      {
         CDFTFeature *f = (CDFTFeature*)sourceGrid.GetRowData(targetRow);
         if (f != NULL)
         {
            CString pinName( f->GetPin() );

            if (!pinName.IsEmpty())
            {
               destGrid.AddFeatureToGrid(f);
            }
         }
      }

      // Now delete the rows. Delete from high to low. If we delete low to high then the rows not yet
      // processed will move.
      // Need to move these all over before deleting any rows, otherwise row index gets changed for rows left behind.
      // Original flexgrid implementation did not delete the RefDes only row. Not sure why. Probably just lazy.
      // For this pass, we also leave the RefDes only rows, because it is hard to tell if there will be any pins left or not.
      // We need to keep the RefDes only row if there are any pins left. We'll take a second pass to clean that up, to nuke
      // any RefDes Only rows that have no following pin rows.
      for (long targetRow = max(startSelectedRow, endSelectedRow); targetRow >= min(startSelectedRow, endSelectedRow); targetRow--)
      {
         CDFTFeature *f = (CDFTFeature*)sourceGrid.GetRowData(targetRow);
         if (f != NULL)
         {
            CString pinName( f->GetPin() );
            if (!pinName.IsEmpty())
            {
               sourceGrid.DeleteRow(targetRow);
               delete f;
            }
         }
      }
      // Now take a pass and delete any RefDes only row that has no following pin row. The selection start/end are no
      // longer valid, so process the whole list.
      CString prevRefDes;
      for (long rowIndx = sourceGrid.GetNumberRows()-1; rowIndx >= 0; rowIndx--)
      {
         CDFTFeature *f = (CDFTFeature*)sourceGrid.GetRowData(rowIndx);
         if (f != NULL)
         {
            CString pinName( f->GetPin() );
            CString refDes( f->GetComponent() );
            if (pinName.IsEmpty() && refDes.Compare(prevRefDes) != 0)
            {
               // Pin name on current row is empty, refdes on current row does not match
               // previous row. That means there were no pin rows with same refdes after this
               // row. Delete this row.
               sourceGrid.DeleteRow(rowIndx);
               delete f;
               prevRefDes.Empty();
            }
            else
            {
               // Pin name not empty, update refDes tracking
               prevRefDes = refDes;
            }
         }
      }
   }

   sourceGrid.ClearSelections();

   // Do not sort the source list, as doing so makes the list reset to the top, and users
   // have to scroll to where they were again (Mark's request). The dest list we have to
   // sort, because we added to the end and we need to sort the new item into place.
   // Have to... order of list is important.
   sourceGrid.SortAscending();
   destGrid.SortAscending();

   sourceGrid.SetPaintMode(TRUE);
   destGrid.SetPaintMode(TRUE);

   sourceGrid.RedrawAll();
   destGrid.RedrawAll();

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);

}

void CPreconditionsPP::zoomToComponent(CString compName)
{
   if (!zoomToSelected)
      return;

   CDblRect rect;
   if (!ComponentsExtents(m_pDoc, compName, &rect, FALSE))
   {
      m_pView->PanToRefName(compName);
      ErrorMessage("Component has no extents", "Panning");
      return;
   }

   double marginSizeX = (m_pDoc->getSettings().getXmax() - m_pDoc->getSettings().getXmin()) * m_pDoc->getSettings().ExtentMargin / 100;
   double marginSizeY = (m_pDoc->getSettings().getYmax() - m_pDoc->getSettings().getYmin()) * m_pDoc->getSettings().ExtentMargin / 100;

   double marginSize = max(marginSizeX, marginSizeY);

   rect.xMin -= marginSize;
   rect.xMax += marginSize;
      
   rect.yMin -= marginSize;
   rect.yMax += marginSize;     

   m_pView->ZoomBox(rect);
}

BOOL CPreconditionsPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // Available Features Grid
   m_ppAvailableFeaturesGrid.AttachGrid(this, IDC_AvailableFeaturesGridStatic);
   CRect rcGridWindow;
   GetDlgItem( IDC_AvailableFeaturesGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_ppAvailableFeaturesGrid.SizeToFit(rcGridWindow);

   // Forced Probes Grid
   m_ppForcedProbesGrid.AttachGrid(this, IDC_ForcedProbesGridStatic);
   //CRect rcGridWindow;
   GetDlgItem( IDC_ForcedProbesGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_ppForcedProbesGrid.SizeToFit(rcGridWindow);

   // No Probes Grid
   m_ppNoProbesGrid.AttachGrid(this, IDC_NoProbesGridStatic);
   //CRect rcGridWindow;
   GetDlgItem( IDC_NoProbesGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_ppNoProbesGrid.SizeToFit(rcGridWindow);

   m_ppAvailableFeaturesGrid.SetPaintMode(FALSE);
   m_ppForcedProbesGrid.SetPaintMode(FALSE);
   m_ppNoProbesGrid.SetPaintMode(FALSE);

   getAndFillFeatureGrid();

   m_ppAvailableFeaturesGrid.SortAscending();
   m_ppForcedProbesGrid.SortAscending();
   m_ppNoProbesGrid.SortAscending();

   m_ppAvailableFeaturesGrid.SetPaintMode(TRUE);
   m_ppForcedProbesGrid.SetPaintMode(TRUE);
   m_ppNoProbesGrid.SetPaintMode(TRUE);


   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreconditionsPP::getAndFillFeatureGrid()
{
   if (m_pDoc == NULL || m_pTestPlan == NULL)
      return;

   CDFTPreconditions &preconditions = m_pTestPlan->GetPreconditions();

   int deviceKwIndex = m_pDoc->IsKeyWord(ATT_DEVICETYPE, 0);

   // Gather all component pins for Preconditions
   FileStruct *file = m_pDoc->getFileList().GetOnlyShown(blockTypePcb);

   if (file == NULL)
      return;

   BlockStruct *fileBlock = file->getBlock();
   POSITION pos = fileBlock->getHeadDataInsertPosition();
   while (pos)
   {
      DataStruct *data = fileBlock->getNextDataInsert(pos);

      if (data->isInsertType(insertTypePcbComponent) || data->isInsertType(insertTypeTestPoint))
      {
         InsertStruct *insert = data->getInsert();

         // get the component name
         CString componentName( insert->getRefname() );

         // get the device type if there is one
         CString deviceName;
         if (deviceKwIndex >= 0)
         {
            Attrib *att = NULL;
            if (data->getAttributes() != NULL && data->getAttributes()->Lookup(deviceKwIndex, att))
            {
               deviceName = get_attvalue_string(m_pDoc, att);
            }
         }

         // get the pin name
         BlockStruct *block = m_pDoc->getBlockAt(insert->getBlockNumber());
         if (block != NULL)
         {
            POSITION insertPos = block->getHeadDataInsertPosition();
            while (insertPos != NULL)
            {
               DataStruct *insertData = block->getNextDataInsert(insertPos);
               InsertStruct *insertInsert = insertData->getInsert();

               if (insertInsert->getInsertType() == insertTypePin)
               {
                  CString pinName( insertInsert->getRefname() );

                  CString sortablePinRef( insertInsert->getSortableRefDes() );

                  if (!pinName.IsEmpty())
                  {
                     // add to Available list if not in Forced or Noprobe lists
                     POSITION tempPos = NULL;
                     if (preconditions.Find_ForcedFeatures(componentName, pinName, tempPos) == NULL &&
                        preconditions.Find_NoProbedFeatures(componentName, pinName, tempPos) == NULL)
                     {
                        m_ppAvailableFeaturesGrid.AddFeatureToGrid(componentName, deviceName, pinName);
                     }
                  }
               }
            }
         }
      }
   }

   pos = preconditions.GetHeadPosition_ForcedFeatures();
   while (pos)
   {
      CDFTFeature *feature = preconditions.GetNext_ForcedFeatures(pos);
      m_ppForcedProbesGrid.AddFeatureToGrid(feature);
   }

   pos = preconditions.GetHeadPosition_NoProbedFeatures();
   while (pos)
   {
      CDFTFeature *feature = preconditions.GetNext_NoProbedFeatures(pos);
      m_ppNoProbesGrid.AddFeatureToGrid(feature);
   }
}

bool CPreconditionsPP::UpdateTestPlan()
{
   if (m_hWnd == NULL)
      return true;

   UpdateData();

   m_pPreconditions->RemoveAll_ForcedFeatures();
   for (int i = 0; i < m_ppForcedProbesGrid.GetNumberRows(); i++)
   {
      CDFTFeature *f = (CDFTFeature*)m_ppForcedProbesGrid.GetRowData(i);
      if (f != NULL)
      {
         CString pinName( f->GetPin() );
         if (!pinName.IsEmpty())
         {
            CString compRefDes( f->GetComponent() );
            CString deviceName( f->GetDevice() );

            m_pPreconditions->AddTail_ForcedFeatures(compRefDes, pinName, deviceName);
         }
      }
   }
   
   m_pPreconditions->RemoveAll_NoProbedFeatures();
   for (int i = 0; i < m_ppNoProbesGrid.GetNumberRows(); i++)
   {
      CDFTFeature *f = (CDFTFeature*)m_ppNoProbesGrid.GetRowData(i);
      if (f != NULL)
      {
         CString pinName( f->GetPin() );

         if (!pinName.IsEmpty())
         {
            CString compRefDes( f->GetComponent() );
            CString deviceName( f->GetDevice() );

            m_pPreconditions->AddTail_NoProbedFeatures(compRefDes, pinName, deviceName);
         }
      }
   }

   return true;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CNetConditionsPP dialog
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define NETS_FIXED_ROWS 1

#define NETS_COLS_NAME              0
#define NETS_COLS_NUMOFPINS         1
#define NETS_COLS_NETTYPE           2
#define NETS_COLS_NOPROBE           3
#define NETS_COLS_TEST_RESOURCES    4
#define NETS_COLS_POWER_RESOURCES   5
#define NETS_COLS_VOLTAGE           6
#define NETS_COLS_CURRENT           7
#define NETS_COLS_TOTAL             8  // <last col> + 1

IMPLEMENT_DYNAMIC(CNetConditionsPP, CPropertyPage)
CNetConditionsPP::CNetConditionsPP()
   : CPropertyPage(CNetConditionsPP::IDD)
   , m_filter(_T(""))
   , m_caseSensitive(FALSE)
   , m_curFilter(_T("*"))
   , m_noProbe(FALSE)
   , m_voltage(_T(""))
   , m_current(_T(""))
   , m_testResources(_T(""))
   , m_powerInjectionResorces(_T(""))
   , m_netType(_T(""))
{
   m_pDoc = NULL;
   m_pView = NULL;
   m_pTestPlan = NULL;
   m_pNetConditions = NULL;
   m_pParent = NULL;
}

CNetConditionsPP::CNetConditionsPP(CCEtoODBDoc *Doc)
   : CPropertyPage(CNetConditionsPP::IDD)
   , m_filter(_T(""))
   , m_caseSensitive(FALSE)
   , m_curFilter(_T("*"))
   , m_noProbe(FALSE)
   , m_voltage(_T(""))
   , m_current(_T(""))
   , m_testResources(_T(""))
   , m_powerInjectionResorces(_T(""))
   , m_netType(_T(""))
{
   m_pDoc = Doc;
   m_pTestPlan = NULL;
   m_pNetConditions = NULL;
   m_pParent = NULL;

   POSITION pos = m_pDoc->GetFirstViewPosition();
   m_pView = (CCEtoODBView*)m_pDoc->GetNextView(pos);
}

CNetConditionsPP::~CNetConditionsPP()
{
   m_pDoc->HighlightedNetsMap.RemoveAll();
}

void CNetConditionsPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDIT_FILTER, m_filter);
   DDX_Check(pDX, IDC_CHECK_CASE_SENSITIVE, m_caseSensitive);
   DDX_Text(pDX, IDC_STATIC_CUR_FILTER, m_curFilter);
   DDX_Check(pDX, IDC_CHECK_NOPROBE, m_noProbe);
   DDX_Text(pDX, IDC_EDIT_VOLTAGE, m_voltage);
   DDX_Text(pDX, IDC_EDIT_CURRENT, m_current);
   DDX_Text(pDX, IDC_EDIT_TEST_RESOURCES, m_testResources);
   DDX_Text(pDX, IDC_EDIT_POWER_INJ_RESOURCES2, m_powerInjectionResorces);
   DDX_CBString(pDX, IDC_COMBO_NETTYPE, m_netType);
   DDX_Control(pDX, IDC_COMBO_NETTYPE, m_netTypeCombo);
}


BEGIN_MESSAGE_MAP(CNetConditionsPP, CPropertyPage)
   ON_BN_CLICKED(IDC_FILTER_NETS, OnBnClickedFilterNets)
   ON_BN_CLICKED(IDC_UNFILTER_NETS, OnBnClickedUnfilterNets)
   ON_BN_CLICKED(IDC_APPLY_TO_SELECTED, OnBnClickedApplyToSelected)
   ON_BN_CLICKED(IDC_APPLY_TO_VISIBLE, OnBnClickedApplyToVisible)
   ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
END_MESSAGE_MAP()


// CNetConditionsPP message handlers
BEGIN_EVENTSINK_MAP(CNetConditionsPP, CPropertyPage)
END_EVENTSINK_MAP()


//=====================================================================================================
// This stuff is being kept as example of how zoom-to-net was implemented when flexgrid was
// used for this tab's grid. This feature was tested in 4.10.101 circa 22 Dec 2011 and found
// to not be working at all. No zoom occurred. It is not clear if this has ever worked, and
// if it did then when did it stop working. We might be called on to make such zoom work if
// Mark finds it is something not working now that used to work. So this little bit
// of implementation is being left as comment/documentation for awhile, to aide in making
// such zoom work again if we end up asked to do so.
#ifdef POSTERITY

void CNetConditionsPP::SelChangeVsflexNets()
{
   m_pDoc->HighlightedNetsMap.RemoveAll();

   if (m_flexGridNets.get_SelectedRows() > 1)
   {
      m_pDoc->UpdateAllViews(NULL);
      return;
   }

   long curRow = m_flexGridNets.get_SelectedRow((long)0);
   CString netName = m_flexGridNets.getCell(CExtendedFlexGrid::flexcpText, curRow, NETS_COLS_NAME);

   zoomToNet(netName);
}

void CNetConditionsPP::zoomToNet(CString netName)
{
   CDblRect rect;

   if (!NetExtents(m_pDoc, netName, &rect))
   {
      ErrorMessage("Net has no extents", "Panning to Net");
      return;
   }

   int value = m_pDoc->RegisterValue(netName);
   m_pDoc->HighlightedNetsMap.SetAt(value, m_pDoc->getSettings().HighlightColor);
   m_pDoc->HighlightByAttrib(FALSE, m_pDoc->IsKeyWord(ATT_NETNAME, 0), valueTypeString, value);

   double marginSizeX = (m_pDoc->getSettings().getXmax() - m_pDoc->getSettings().getXmin()) * m_pDoc->getSettings().ExtentMargin / 100;
   double marginSizeY = (m_pDoc->getSettings().getYmax() - m_pDoc->getSettings().getYmin()) * m_pDoc->getSettings().ExtentMargin / 100;
   double marginSize = max(marginSizeX, marginSizeY);

   rect.xMin -= marginSize;
   rect.xMax += marginSize;
      
   rect.yMin -= marginSize;
   rect.yMax += marginSize;     

   m_pView->ZoomBox(rect);
}
#endif
//=====================================================================================================


BOOL  CNetConditionsPP::OnKillActive()
{
	// Update testplan with net conditions grid values
	// mainly so if user goes to Options tab and saves net Conditions
	// the latest values will be saved.
	this->UpdateTestPlan();

	return CPropertyPage::OnKillActive();
}

BOOL CNetConditionsPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_netTypeCombo.AddString("Power");
   m_netTypeCombo.AddString("Ground");
   m_netTypeCombo.AddString("Signal");


   m_netConditionsGrid.AttachGrid(this, IDC_NetConditionsGridStatic, false);  // false turns off bold in heading font
   CRect rcGridWindow;
   GetDlgItem( IDC_NetConditionsGridStatic )->GetWindowRect( &rcGridWindow );
   ScreenToClient( &rcGridWindow );
   m_netConditionsGrid.SizeToFit(rcGridWindow);

   //*rcf bug Looks like flexgrid used to sort nets by ascending name

   fillNetsGrid();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CNetConditionsPP::fillNetsGrid()
{
   if (m_pTestPlan == NULL)
      return;

   m_netConditionsGrid.RemoveAllRows();

   CString rowData;
   POSITION pos = m_pNetConditions->GetHeadPosition_ProbeableNets();
   while (pos)
   {
      CDFTProbeableNet *pNet = m_pNetConditions->GetNext_ProbeableNets(pos);

      int rowIndx = m_netConditionsGrid.GetNumberRows();
      m_netConditionsGrid.SetNumberRows(rowIndx+1);

      CString pinCountStr;
      pinCountStr.Format("%d", pNet->GetPinCount());
      CString voltageStr;
      voltageStr.Format("%.2fV", pNet->GetVoltage());
      CString currentStr;
      currentStr.Format("%.2fA", pNet->GetCurrent());

      m_netConditionsGrid.QuickSetText(0, rowIndx, pNet->GetName());
      m_netConditionsGrid.QuickSetNumber(1, rowIndx, pNet->GetPinCount());
      m_netConditionsGrid.QuickSetText(2, rowIndx, pNet->GetNetTypeName());
      m_netConditionsGrid.QuickSetText(3, rowIndx, pNet->GetNoProbe()?"Yes":"No");
      m_netConditionsGrid.QuickSetNumber(4, rowIndx, pNet->GetTestResourcesPerNet());
      m_netConditionsGrid.QuickSetNumber(5, rowIndx, pNet->GetPowerInjectionResourcesPerNet());
      m_netConditionsGrid.QuickSetText(6, rowIndx, voltageStr);
      m_netConditionsGrid.QuickSetText(7, rowIndx, currentStr);
   }
}

void CNetConditionsPP::applyFilter(CString curFilter, bool useCaseSensitivity)
{
   // Hide all the rows where the net name does not fit the filter.
   for (int rowIndx = 0; rowIndx < m_netConditionsGrid.GetNumberRows(); rowIndx++)
   {
      CString netName = m_netConditionsGrid.QuickGetText(0, rowIndx);
      CString compareName = netName;

      if (!useCaseSensitivity)
      {
         compareName.MakeUpper();
         curFilter.MakeUpper();
      }

      if (wildcmp(curFilter, netName))
         m_netConditionsGrid.SetRowHeight(rowIndx, m_netConditionsGrid.GetExpandedRowHeight());
      else
         m_netConditionsGrid.SetRowHeight(rowIndx, 0);
   }
   m_netConditionsGrid.RedrawAll();
}

bool CNetConditionsPP::applyCurrentNetSettings(CNetConditionsGrid &netsGrid, int row)
{
   if (row < 0 || row >= netsGrid.GetNumberRows())
      return false;

   // Assign VOLTAGE
   CString errMsg;
   bool res = true;
   m_voltage.Trim();
   if (!m_voltage.IsEmpty())
   {
      bool canAssign = true;
      for (int count=0; count<m_voltage.GetLength() && canAssign; count++)
      {
         if (m_voltage[count] == '+' || m_voltage[count] == '-')
         {
            if (count != 0)  // Plus/minus voltages are allowed, sign must be first char
               canAssign = false;
         }
         else if (!isdigit(m_voltage[count]) && m_voltage[count] != '.')
         {
            canAssign = false;
         }
      }

      if (canAssign)
      {
         CString voltStr;
         double voltage = atof(m_voltage);
         voltStr.Format("%.2fV", voltage);
         netsGrid.QuickSetText(NETS_COLS_VOLTAGE, row, voltStr);
      }
      else
         errMsg += "\"" + m_voltage + "\" can't assign as a voltage.\n";
      
      res = res && canAssign;
   }  
   
   // Assign CURRENT
   m_current.Trim();
   if (!m_current.IsEmpty())
   {
      bool canAssign = true;
      for (int count=0; count<m_current.GetLength() && canAssign; count++)
      {
         if (!isdigit(m_current[count]) && m_current[count] != '.')
            canAssign = false;
      }

      if (canAssign)
      {
         CString currentStr;
         double current = atof(m_current);
         currentStr.Format("%.2fV", current);
         netsGrid.QuickSetText(NETS_COLS_CURRENT, row, currentStr);
      }
      else
         errMsg += "\"" + m_current + "\" can't assign as a current.\n";

      res = res && canAssign;
   }
   
   // Assign NET TYPE
   if (!m_netType.IsEmpty())
   {
      netsGrid.QuickSetText(NETS_COLS_NETTYPE, row, m_netType);
   }

   // Assign NO PROBE
   CString noProbeStr(m_noProbe?"Yes":"No");
   netsGrid.QuickSetText(NETS_COLS_NOPROBE, row, noProbeStr);
   
   // Assign TEST RESOURCES
   m_testResources.Trim();
   if (!m_testResources.IsEmpty())
   {
      bool canAssign = true;
      for (int count=0; count<m_testResources.GetLength() && canAssign; count++)
      {
         if (!isdigit(m_testResources[count]) /* && m_testResources[count] != '.' fractional resource not allowed!*/)
            canAssign = false;
      }

      if (canAssign)
      {
         netsGrid.QuickSetNumber(NETS_COLS_TEST_RESOURCES, row, atoi(m_testResources));
      }
      else
         errMsg += "\"" + m_testResources + "\" can't assign as a test resource.\n";

      res = res && canAssign;
   }
   
   // Assign POWER INJECTION RESOURCES
   if (!m_powerInjectionResorces.IsEmpty())
   {
      bool canAssign = true;
      for (int count=0; count<m_powerInjectionResorces.GetLength() && canAssign; count++)
      {
         if (!isdigit(m_powerInjectionResorces[count]) && m_powerInjectionResorces[count] != '.')
            canAssign = false;
      }

      if (canAssign)
      {
         netsGrid.QuickSetNumber(NETS_COLS_POWER_RESOURCES, row, atoi(m_powerInjectionResorces));
      }
      else
         errMsg += "\"" + m_powerInjectionResorces + "\" can't assign as a power injection resource.\n";

      res = res && canAssign;
   }
   
   if (!errMsg.IsEmpty())
      ErrorMessage(errMsg, "Net Conditions");

   return res;
}

void CNetConditionsPP::OnBnClickedFilterNets()
{
   UpdateData(TRUE);

   CString curFilter = (CString)m_filter + " w/";
   if (m_caseSensitive)
      curFilter += " Case Sensitivity";
   else
      curFilter += "o Case Sensitivity";

   // the filter was the same, we don't need to do anything
   if (curFilter == m_curFilter)
      return;

   applyFilter(m_filter, m_caseSensitive?true:false);

   m_curFilter = curFilter;
   UpdateData(FALSE);
}

void CNetConditionsPP::OnBnClickedUnfilterNets()
{
   UpdateData(TRUE);

   m_curFilter = "*";
   applyFilter();

   UpdateData(FALSE);
}

void CNetConditionsPP::OnBnClickedApplyToSelected()
{
   UpdateData(TRUE);

   for (int rowIndx = 0; rowIndx < m_netConditionsGrid.GetNumberRows(); rowIndx++)
   {
      bool rowIsHidden = (m_netConditionsGrid.GetRowHeight(rowIndx) < 1);
      bool rowIsSelected = m_netConditionsGrid.IsSelected(NETS_COLS_NAME, rowIndx, NULL)?true:false;
      if (rowIsHidden || !rowIsSelected)
         continue;

      if (!applyCurrentNetSettings(m_netConditionsGrid, rowIndx))
         break;
   }
   m_netConditionsGrid.RedrawAll();

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED, 0);
}

void CNetConditionsPP::OnBnClickedApplyToVisible()
{
   UpdateData(TRUE);

   for (int rowIndx = 0; rowIndx < m_netConditionsGrid.GetNumberRows(); rowIndx++)
   {
      bool rowIsHidden = (m_netConditionsGrid.GetRowHeight(rowIndx) < 1);
      if (rowIsHidden)
         continue;

      if (!applyCurrentNetSettings(m_netConditionsGrid, rowIndx))
         break;
   }

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED, 0);
}

void CNetConditionsPP::OnBnClickedReset()
{
   m_noProbe = FALSE;
   m_netType.Empty();
   m_netTypeCombo.SetCurSel(-1);
   m_voltage.Empty();
   m_current.Empty();
   m_testResources.Empty();
   m_powerInjectionResorces.Empty();

   UpdateData(FALSE);
}

void CNetConditionsPP::ReloadFromTestPlan()
{
   fillNetsGrid();
}

bool CNetConditionsPP::UpdateTestPlan()
{
   if (m_hWnd == NULL)
      return true;

   UpdateData();

   for (int rowIndx = 0; rowIndx < m_netConditionsGrid.GetNumberRows(); rowIndx++)
   {
      CString netName( m_netConditionsGrid.QuickGetText(NETS_COLS_NAME, rowIndx) );
      CString voltage( m_netConditionsGrid.QuickGetText(NETS_COLS_VOLTAGE, rowIndx) );
      CString current( m_netConditionsGrid.QuickGetText(NETS_COLS_CURRENT, rowIndx) );
      CString netType( m_netConditionsGrid.QuickGetText(NETS_COLS_NETTYPE, rowIndx) );
      CString noProbe( m_netConditionsGrid.QuickGetText(NETS_COLS_NOPROBE, rowIndx) );
      CString testRes( m_netConditionsGrid.QuickGetText(NETS_COLS_TEST_RESOURCES, rowIndx) );
      CString powrRes( m_netConditionsGrid.QuickGetText(NETS_COLS_POWER_RESOURCES, rowIndx) );

      // find probeable net
      POSITION pos = NULL;
      CDFTProbeableNet *probeableNet = m_pNetConditions->Find_ProbeableNet(netName, pos);
      if (probeableNet != NULL)
      {
         probeableNet->SetVoltage(atof(voltage));
         probeableNet->SetCurrent(atof(current));

         eProbeableNetType eNetType = probeableNetTypePower;
         if (netType == "Power")
            eNetType = probeableNetTypePower;
         else if (netType == "Ground")
            eNetType = probeableNetTypeGround;
         else if (netType == "Signal")
            eNetType = probeableNetTypeSignal;
         probeableNet->SetNetType(eNetType);

         probeableNet->SetNoProbe((noProbe=="Yes")?true:false);
         probeableNet->SetTestResourcesPerNet(atoi(testRes));
         probeableNet->SetPowerInjectionResourcesPerNet(atoi(powrRes));
      }
   }

   return true;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// COptionsPP dialog
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define OPTIONS_USE_SURFACE_TOP     0
#define OPTIONS_USE_SURFACE_BOT     1
#define OPTIONS_USE_SURFACE_BOTH    2

#define OPTIONS_SURFACE_PREF_TOP    0
#define OPTIONS_SURFACE_PREF_BOT    1

IMPLEMENT_DYNAMIC(COptionsPP, CPropertyPage)
COptionsPP::COptionsPP()
   : CPropertyPage(COptionsPP::IDD)
   , m_allowDoubleWiring(FALSE)
   , m_useProbeOffsets(FALSE)
   , m_placeOnAllAccess(FALSE)
   , m_useSMDTop(FALSE)
   , m_useSMDBot(FALSE)
   , m_usePTHTop(FALSE)
   , m_usePTHBot(FALSE)
   , m_useViaTop(FALSE)
   , m_useViaBot(FALSE)
   , m_startProbeNumber(1)
   , m_useSurface(OPTIONS_USE_SURFACE_TOP)
   , m_surfacePreference(OPTIONS_SURFACE_PREF_TOP)
   , m_useCase(FALSE)
   , m_useExistingProbes(FALSE)
   , m_probeUse(FALSE)
   , m_retainProbeNames(FALSE)
   , m_retainProbePositions(FALSE)
   , m_retainProbeSizes(FALSE)
{
}

COptionsPP::~COptionsPP()
{
}

void COptionsPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_CHECK_DOUBLE_WIRES, m_allowDoubleWiring);
   DDX_Text(pDX, IDC_EDIT_PROBE_START_NUMBER, m_startProbeNumber);
   DDX_Radio(pDX, IDC_RADIO_SIDE_USE, m_useSurface);
   DDX_Radio(pDX, IDC_RADIO_SIDE_PREFERENCE, m_surfacePreference);
   DDX_Check(pDX, IDC_CASE_SENSITIVE, m_useCase);
   DDX_Check(pDX, IDC_ALL_ACCESSIBLE, m_placeOnAllAccess);
   DDX_Check(pDX, IDC_CHECK_USE_EXISTING_PROBES, m_useExistingProbes);
   DDX_Radio(pDX, IDC_RADIO_PROBE_USE, m_probeUse);
   DDX_Check(pDX, IDC_CHECK_RETAIN_PROBE_NAME, m_retainProbeNames);
   DDX_Check(pDX, IDC_CHECK_RETAIN_PROBE_POSITION, m_retainProbePositions);
   DDX_Check(pDX, IDC_CHECK_RETAIN_PROBE_SIZE, m_retainProbeSizes);
   DDX_Check(pDX, IDC_CHECK_PP_WRITE_IGNORED_SURFACE, m_chkWriteIgnoreSurface);
}


BEGIN_MESSAGE_MAP(COptionsPP, CPropertyPage)
   ON_BN_CLICKED(IDC_RADIO_SIDE_USE, OnBnClickedRadioSideUse)
   ON_BN_CLICKED(IDC_RADIO_SIDE_USE1, OnBnClickedRadioSideUse)
   ON_BN_CLICKED(IDC_RADIO_SIDE_USE2, OnBnClickedRadioSideUse)
   ON_BN_CLICKED(IDC_RADIO_SIDE_PREFERENCE, OnBnClickedRadioSidePreference)
   ON_BN_CLICKED(IDC_RADIO_SIDE_PREFERENCE2, OnBnClickedRadioSidePreference)
   ON_EN_CHANGE(IDC_EDIT_PROBE_START_NUMBER, OnEnChangeEditProbeStartNumber)
   ON_BN_CLICKED(IDC_CHECK_DOUBLE_WIRES, OnBnClickedCheckDoubleWires)
   ON_BN_CLICKED(IDC_LOAD_FEEDBACK, OnBnClickedLoadFeedback)
   ON_BN_CLICKED(IDC_SAVE_CSV, OnBnClickedSaveCSV)
   ON_BN_CLICKED(IDC_ALL_ACCESSIBLE, OnBnClickedAllAccessible)
   ON_BN_CLICKED(IDC_CHECK_USE_EXISTING_PROBES, OnBnClickedCheckUseExistingProbes)
   ON_BN_CLICKED(IDC_RADIO_PROBE_USE, OnUpdateParentSheet)
   ON_BN_CLICKED(IDC_RADIO_PROBE_USE1, OnUpdateParentSheet)
   ON_BN_CLICKED(IDC_CHECK_RETAIN_PROBE_NAME, OnUpdateParentSheet)
   ON_BN_CLICKED(IDC_CHECK_RETAIN_PROBE_POSITION, OnUpdateParentSheet)
   ON_BN_CLICKED(IDC_CHECK_RETAIN_PROBE_SIZE, OnUpdateParentSheet)
   ON_BN_CLICKED(IDC_CASE_SENSITIVE, OnBnClickedCaseSensitive)
   ON_BN_CLICKED(IDC_CHECK_PP_WRITE_IGNORED_SURFACE, OnBnClickedCheckPpWriteIgnoredSurface)
END_MESSAGE_MAP()


// COptionsPP message handlers
BOOL COptionsPP::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_allowDoubleWiring = m_pTestPlan->GetAllowDoubleWiring();
   m_useProbeOffsets = m_pTestPlan->GetUseProbeOffset();
   m_placeOnAllAccess = m_pTestPlan->GetPlaceOnAllAccessible();
   m_useSMDTop = m_pTestPlan->GetOffsetSMDTop();
   m_useSMDBot = m_pTestPlan->GetOffsetSMDBot();
   m_usePTHTop = m_pTestPlan->GetOffsetPTHTop();
   m_usePTHBot = m_pTestPlan->GetOffsetPTHBot();
   m_useViaTop = m_pTestPlan->GetOffsetVIATPTop();
   m_useViaBot = m_pTestPlan->GetOffsetVIATPBot();
   m_startProbeNumber = m_pTestPlan->GetProbeStartNumber();
   m_useSurface = m_pTestPlan->GetProbeSide();
   m_surfacePreference = m_pTestPlan->GetProbeSidePreference();
   m_placeOnAllAccess = m_pTestPlan->GetPlaceOnAllAccess();
   m_useCase = m_pTestPlan->GetNetConditionCaseSensitive();

   //m_useExistingProbes = false;
   //GetDlgItem(IDC_CHECK_USE_EXISTING_PROBES)->EnableWindow(FALSE);
   m_useExistingProbes = m_pTestPlan->GetUseExistingProbes();
   m_probeUse = m_pTestPlan->GetProbeUse();
   m_retainProbeNames = m_pTestPlan->RetainProbeNames();
   m_retainProbePositions = m_pTestPlan->RetainProbePositions();
   m_retainProbeSizes = m_pTestPlan->RetainProbeSizes();
   m_chkWriteIgnoreSurface = m_pTestPlan->GetPPWriteIgnoreSurface();

   UpdateData(FALSE);

   GetDlgItem(IDC_LOAD_FEEDBACK)->EnableWindow(TRUE);
   GetDlgItem(IDC_SAVE_CSV)->EnableWindow(TRUE);
   updateUIProbeSide();
   updateUIProbeLocking();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPP::updateUIProbeSide()
{
   UpdateData(TRUE);

   GetDlgItem(IDC_STATIC_SIDE_PREFERENCE)->EnableWindow(m_useSurface == OPTIONS_USE_SURFACE_BOTH);
   GetDlgItem(IDC_RADIO_SIDE_PREFERENCE)->EnableWindow(m_useSurface == OPTIONS_USE_SURFACE_BOTH);
   GetDlgItem(IDC_RADIO_SIDE_PREFERENCE2)->EnableWindow(m_useSurface == OPTIONS_USE_SURFACE_BOTH);
}

void COptionsPP::updateUIProbeLocking()
{
   UpdateData();

   GetDlgItem(IDC_STATIC_PROBE_USE)->EnableWindow(m_useExistingProbes);
   GetDlgItem(IDC_RADIO_PROBE_USE)->EnableWindow(m_useExistingProbes);
   GetDlgItem(IDC_RADIO_PROBE_USE1)->EnableWindow(m_useExistingProbes);
   GetDlgItem(IDC_CHECK_RETAIN_PROBE_NAME)->EnableWindow(m_useExistingProbes);
   GetDlgItem(IDC_CHECK_RETAIN_PROBE_POSITION)->EnableWindow(m_useExistingProbes);
   GetDlgItem(IDC_CHECK_RETAIN_PROBE_SIZE)->EnableWindow(m_useExistingProbes);
}

void COptionsPP::OnEnChangeEditProbeStartNumber()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void COptionsPP::OnBnClickedCheckDoubleWires()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void COptionsPP::OnBnClickedRadioSideUse()
{
   UpdateData();

   updateUIProbeSide();

   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
   m_pParent->PostMessage(WM_COMMAND, IPP_UPDATEDIALOG);
}

void COptionsPP::OnBnClickedRadioSidePreference()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void COptionsPP::OnBnClickedAllAccessible()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void COptionsPP::OnUpdateParentSheet()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void COptionsPP::OnBnClickedCaseSensitive()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void COptionsPP::OnBnClickedCheckPpWriteIgnoredSurface()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
}

void COptionsPP::OnBnClickedLoadFeedback()
{
   UpdateData();

   CFileDialog fileDlg(TRUE, ".CSV", NULL, NULL, "Comma Delimited File (*.CSV)|*.csv|Text File (*.TXT)|*.txt|All File Types (*.*)|*.*||", m_pParent);

   if (fileDlg.DoModal() == IDCANCEL)
      return;

   CString fileName = fileDlg.GetPathName();
   /*if (!m_pTestPlan->ReadTestWayFile(fileName, m_useCase?true:false))
   {
      CString msg;
      msg.Format("Error reading %s", fileName);
      ErrorMessage(msg, "Probe Placement");
      return;
   }*/

   m_pParent->PostMessage(WM_COMMAND, IPP_UPDATEPAGES);
}


void COptionsPP::OnBnClickedSaveCSV()
{
   UpdateData();

   CFileDialog fileDlg(FALSE, ".CSV", "NetConditions.csv", NULL, "Comma Delimited File (*.CSV)|*.csv|Text File (*.TXT)|*.txt|All File Types (*.*)|*.*||", m_pParent);

	if (fileDlg.DoModal() != IDCANCEL)
   {
      CString filename = fileDlg.GetPathName();
      if (!m_pTestPlan->GetNetConditions().WriteCSV(filename))
      {
         CString msg;
         msg.Format("Error writing %s", filename);
         ErrorMessage(msg, "Probe Placement");
      }
   }
}


void COptionsPP::OnBnClickedCheckUseExistingProbes()
{
   m_pParent->PostMessage(WM_COMMAND, IPP_SETMODIFIED);
   updateUIProbeLocking();
}

bool COptionsPP::UpdateTestPlan()
{
   if (m_hWnd == NULL)
      return true;

   UpdateData();

   m_pTestPlan->SetProbeStartNumber(m_startProbeNumber);
   m_pTestPlan->SetAllowDoubleWiring(m_allowDoubleWiring);
   m_pTestPlan->SetProbeSide((ETestSurface)m_useSurface);
   m_pTestPlan->SetProbeSidePreference((ETestSurface)m_surfacePreference);
   m_pTestPlan->SetUseProbeOffset(m_useProbeOffsets);
   m_pTestPlan->SetPlaceOnAllAccessible(m_placeOnAllAccess);
   m_pTestPlan->SetOffsetSMDTop(m_useSMDTop);
   m_pTestPlan->SetOffsetPTHTop(m_usePTHTop);
   m_pTestPlan->SetOffsetVIATPTop(m_useViaTop);
   m_pTestPlan->SetOffsetSMDBot(m_useSMDBot);
   m_pTestPlan->SetOffsetPTHBot(m_usePTHBot);
   m_pTestPlan->SetOffsetVIATPBot(m_useViaBot);
   m_pTestPlan->SetPlaceOnAllAccess(m_placeOnAllAccess);
   m_pTestPlan->SetUseExistingProbes(m_useExistingProbes);
   m_pTestPlan->SetProbeUse((EProbeUsage)m_probeUse);
   m_pTestPlan->SetRetainProbeNames(m_retainProbeNames);
   m_pTestPlan->SetRetainProbePositions(m_retainProbePositions);
   m_pTestPlan->SetRetainProbeSizes(m_retainProbeSizes);
   m_pTestPlan->SetNetConditionCaseSensitive(m_useCase?true:false);
   m_pTestPlan->SetPPWriteIgnoreSurface(m_chkWriteIgnoreSurface?true:false);

   return true;
}



////////////////////////////////////////////////////////////////////////////////
// CProbeLockingPP dialog
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CProbeLockingPP, CPropertyPage)
CProbeLockingPP::CProbeLockingPP()
   : CPropertyPage(CProbeLockingPP::IDD)
{
}

CProbeLockingPP::~CProbeLockingPP()
{
}

void CProbeLockingPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProbeLockingPP, CPropertyPage)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CPPResultsGrid::CPPResultsGrid()
{
   SetSortEnabled(false);  // Turn off grid sort on heading left click

   m_rowIndxTop      = 0;
   m_rowIndxBottom   = 1;;
   m_rowIndxTotal    = 2;

   m_colIndxPlaced   = 0;
   m_colIndxUnplaced = 1;
}

void CPPResultsGrid::SizeToFit(CRect &rect)
{
   int width = rect.Width();

   // 9 actual fields in standard grid
   // 12 is 9 + 3 so three fields get double wide

   int virtualNumCols = 12;  

   int fieldWidth = width / virtualNumCols;

   int colCnt = GetNumberCols();

   // Double wide for row heading, placed, and unplaced columns.
   SetColWidth(-1, fieldWidth*2);
   SetColWidth( 0, fieldWidth*2);
   SetColWidth( 1, fieldWidth*2);

   // Single wide for data columns.
   for (int colIndx = 2; colIndx < colCnt; colIndx++)
   {
      SetColWidth(colIndx, fieldWidth);
   }

   RedrawAll();
}

//----------------------------------------------------------------------------------

int CPPResultsGrid::DefineProbeSizeColumns(CString probeSizeName)
{
   // Returns column index for TR column, the PIR column is that plus one.

   int newColIndx = GetNumberCols();
   
   // Expand grid for two columns for this probe
   SetNumberCols(newColIndx+2);

   // Probe name col spans the TR and PIR cols
   SetMultiColHeading(newColIndx, -2, newColIndx+1, -2, probeSizeName);

   // One col for each of TR and PIR
   QuickSetText(newColIndx,   -1, "TR");
   QuickSetText(newColIndx+1, -1, "PIR");

   return newColIndx;
}

//----------------------------------------------------------------------------------

void CPPResultsGrid::DefineFixedColumns()
{
   SetNumberCols(2,false);  // Start with just the two fixed columns (not the per probe size columns)
   SetNumberRows(3, false); // A row for top, bottom,and total. That is all the rows.

   // Row heading column
   QuickSetText(-1, -1, "Probes");
   QuickSetText(-1, m_rowIndxTop,     "Top");
   QuickSetText(-1, m_rowIndxBottom,  "Bottom");
   QuickSetText(-1, m_rowIndxTotal,   "Total");

   // Placed column
   SetMultiColHeading(m_colIndxPlaced, -2, m_colIndxPlaced, -1,    "Placed");

   // Unplaced column
   SetMultiColHeading(m_colIndxUnplaced, -2, m_colIndxUnplaced, -1, "Unplaced");

}

//----------------------------------------------------------------------------------

void CPPResultsGrid::OnSetup()
{
#ifdef CELL_EDIT
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;
#endif

   SetNumberCols(0,false);
   SetNumberRows(0, false);

   int singleRowThHeight = GetTH_Height();
   int topRowHeight = singleRowThHeight;
   int secondRowHeight = singleRowThHeight;
   int overallHeadingHeight = topRowHeight + secondRowHeight;
   SetTH_NumberRows(2);
   SetTH_RowHeight(-1, secondRowHeight);
   SetTH_RowHeight(-2, topRowHeight);
   SetTH_Height(overallHeadingHeight);

   EnableJoins(TRUE);

   DefineFixedColumns();



   BestFit(-1, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);

   //BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   //SetColWidth(-1, 0); // get rid of "row heading"
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CProbesGrid::CProbesGrid()
{
   SetSortEnabled(false);  // Turn off grid sort on heading left click

   m_mostRecentRow = -1;

}

//----------------------------------------------------------------------------------

void CProbesGrid::SizeToFit(CRect &rect)
{
   int width = rect.Width();

   // Original flexgrid set sizes from left to right:
   // 400, 500, 800, 800, 800, 500, 800, 800.
   // No idea what those units were, and these value do not work right with UG.
   // That totals to 5400 wide.
   // So we set the columns to the fraction of width the old settings represent.
   // We can drop the hundreds for the calc.

   int fieldWidth = (int)(width * (4.0/54.0));
   SetColWidth(0, fieldWidth);

   fieldWidth = (int)(width * (5.0/54.0));
   SetColWidth(1, fieldWidth);

   fieldWidth = (int)(width * (8.0/54.0));
   SetColWidth(2, fieldWidth);

   fieldWidth = (int)(width * (8.0/54.0));
   SetColWidth(3, fieldWidth);

   fieldWidth = (int)(width * (8.0/54.0));
   SetColWidth(4, fieldWidth);

   fieldWidth = (int)(width * (5.0/54.0));
   SetColWidth(5, fieldWidth);

   fieldWidth = (int)(width * (8.0/54.0));
   SetColWidth(6, fieldWidth);

   fieldWidth = (int)(width * (8.0/54.0));
   SetColWidth(7, fieldWidth);

   RedrawAll();
}

//----------------------------------------------------------------------------------

void CProbesGrid::OnSetup()
{
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;

   SetNumberCols(8, false);
   SetNumberRows(0, false);

   int singleRowThHeight = GetTH_Height();
   int topRowHeight = singleRowThHeight;
   int secondRowHeight = singleRowThHeight;
   int overallHeadingHeight = topRowHeight + secondRowHeight;
   SetTH_NumberRows(2);
   SetTH_RowHeight(-1, secondRowHeight);
   SetTH_RowHeight(-2, topRowHeight);
   SetTH_Height(overallHeadingHeight);


   EnableJoins(TRUE);

   CUGCell cell;

   // Overall "Use" column
   GetColDefault(0, &cell); 
   cell.SetCellType(UGCT_CHECKBOX);  // To make it a checkbox cell
   cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
   SetColDefault(0, &cell);

   // Component Outline Distance "Use" column
   GetColDefault(6, &cell); 
   cell.SetCellType(UGCT_CHECKBOX);  // To make it a checkbox cell
   cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
   SetColDefault(6, &cell);


   // Grouped column headings
   SetMultiColHeading(0, -2, 0, -1, "Use");
   SetMultiColHeading(1, -2, 1, -1, "Name");
   SetMultiColHeading(2, -2, 2, -1, "Diameter");
   SetMultiColHeading(3, -2, 3, -1, "Drill Size");
   SetMultiColHeading(4, -2, 4, -1, "Text Size");
   SetMultiColHeading(5, -2, 5, -1, "Cost");
   SetMultiColHeading(6, -2, 7, -2, "Component Outline");
   QuickSetText(6, -1, "Use");
   QuickSetText(7, -1, "Distance");
   
   // Row headings - None
   SetSH_Width(0);

}


void CProbesGrid::AddProbe(CDFTProbeTemplate *probeTemplate, int decimals, PageUnitsTag pageUnits)
{
   if (probeTemplate != NULL)
   {
      int rowIndx = GetNumberRows();
      SetNumberRows(rowIndx + 1);
      SetProbe(rowIndx, probeTemplate, decimals, pageUnits);
   }
}

void CProbesGrid::AddBlankProbe()
{
   if (true)
   {
      int rowIndx = GetNumberRows();
      SetNumberRows(rowIndx + 1);
      
      // Use
      QuickSetBool(PROBES_COLS_USE, rowIndx, true);

      // Component Outline - Use
      QuickSetBool(PROBES_COLS_COMPDIST_ON, rowIndx, true);
   }
}

void CProbesGrid::SetProbe(int rowIndx, CDFTProbeTemplate *probeTemplate, int decimals, PageUnitsTag pageUnits)
{
   if (probeTemplate != NULL && rowIndx >= 0 && rowIndx < GetNumberRows())
   {
      CString buf;

      // Use
      QuickSetBool(PROBES_COLS_USE, rowIndx, probeTemplate->GetUseFlag());

      // Name
      QuickSetText(PROBES_COLS_NAME, rowIndx, probeTemplate->GetName());

      // Diameter
      buf.Format("%.*lf", decimals, probeTemplate->GetDiameter() * Units_Factor(pageUnitsMils, pageUnits));
      QuickSetText(PROBES_COLS_DIAMETER, rowIndx, buf);

      // Drill Size
      buf.Format("%.*lf", decimals, probeTemplate->GetDrillSize() * Units_Factor(pageUnitsMils, pageUnits));
      QuickSetText(PROBES_COLS_DRILLSIZE, rowIndx, buf);

      // Text Size
      buf.Format("%.*lf", decimals, probeTemplate->GetTextSize() * Units_Factor(pageUnitsMils, pageUnits));
      QuickSetText(PROBES_COLS_TEXTSIZE, rowIndx, buf);

      // Cost
      buf.Format("%d", probeTemplate->GetCost());
      QuickSetText(PROBES_COLS_COST, rowIndx, buf);

      // Component Outline - Use
      QuickSetBool(PROBES_COLS_COMPDIST_ON, rowIndx, probeTemplate->GetUseComponentOutline());

      // Component Outline - Distance
      buf.Format("%.*lf", decimals, probeTemplate->GetComponentOutlineDistance() * Units_Factor(pageUnitsMils, pageUnits));
      QuickSetText(PROBES_COLS_COMPDIST_DISTANCE, rowIndx, buf);
   }
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finished
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being canceled
//	Return:
//		TRUE - to allow the edit to proceed
//		FALSE - to force the user back to editing of that same cell
int CProbesGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   // Whether it actually changed or not... just say it did.
   // This leads to turning the Process button on again.
   this->GetParent()->PostMessage(IPP_SETMODIFIED, row, col);

   if (cancelFlag)
      return TRUE;

   return TRUE;
}

/////////////////////////////////////

void CProbesGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed)
{
   // Just want to save the row
   m_mostRecentRow = row;
}

void CProbesGrid::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
   // Just want to save the row
   m_mostRecentRow = row;
}

void CProbesGrid::DeleteMostRecentRow()
{
   if (m_mostRecentRow >= 0 && m_mostRecentRow < GetNumberRows())
   {
      DeleteRow(m_mostRecentRow);
      m_mostRecentRow = -1;
   }
   else
   {
      ErrorMessage("First click in a row, then click the Remove button.");
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CPreconditionsGrid::CPreconditionsGrid()
{
   SetSortEnabled(false);  // Turn off grid sort on heading left click
}

//----------------------------------------------------------------------------------

CPreconditionsGrid::~CPreconditionsGrid()
{
   for (int rowIndx = 0; rowIndx < GetNumberRows(); rowIndx++)
   {
      CDFTFeature *f = (CDFTFeature*)GetRowData(rowIndx);
      if (f != NULL)
         delete f;
   }
}

//----------------------------------------------------------------------------------


void CPreconditionsGrid::SizeToFit(CRect &rect)
{
   int width = rect.Width();

   SetColWidth(0, width-2);

   RedrawAll();
}

//----------------------------------------------------------------------------------

void CPreconditionsGrid::OnSetup()
{
#ifdef EDIT_CONTROL
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;
#endif

   SetNumberCols(1, false);

   SetNumberRows(1, false);
   SetExpandedRowHeight( GetRowHeight(0) );
   SetNumberRows(0, false);

   SetMultiSelectMode(TRUE);

   // Column headings - None
   SetTH_Height(0);

   // Row headings - None
   SetSH_Width(0);

   // Kill the horizontal scroll bar
   SetHS_Height(0);


}


/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finished
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being canceled
//	Return:
//		TRUE - to allow the edit to proceed
//		FALSE - to force the user back to editing of that same cell
int CPreconditionsGrid::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);

   // Whether it actually changed or not... just say it did.
   // This leads to turning the Process button on again.
   this->GetParent()->PostMessage(IPP_SETMODIFIED, row, col);

   if (cancelFlag)
      return TRUE;

   return TRUE;
}

/////////////////////////////////////

int CPreconditionsGrid::GetRowIndxContaining(CString name)
{
   for (int rowIndx = 0; rowIndx < GetNumberRows(); rowIndx++)
   {
      CDFTFeature *f = (CDFTFeature*)GetRowData(rowIndx);
      if (f != NULL)
      {
         CString existingStr( f->GetComponent() );
         if (existingStr.Compare(name) == 0)
            return rowIndx;
      }
   }

   return -1; // not found
}

void CPreconditionsGrid::AddFeatureToGrid(CDFTFeature *feature)
{
   if (feature != NULL)
      AddFeatureToGrid(feature->GetComponent(), feature->GetDevice(), feature->GetPin());
}

void CPreconditionsGrid::AddFeatureToGrid(CString compRefDes, CString deviceType, CString pinName)
{
   int compRowIndx = GetRowIndxContaining(compRefDes);

   if (compRowIndx < 0) // does not exist in grid, add it
   {
      CString refDesRowData;
      refDesRowData.Format("+  %s [%s]", compRefDes, deviceType);  // We fill the grid in "collapsed" state
      int rowIndx = GetNumberRows();
      SetNumberRows(rowIndx+1);
      QuickSetText(0, rowIndx, refDesRowData);
      // If there were more than one col:  SetRowBackColor(rowIndx, this->m_lightGrayBackgroundColor);
      QuickSetBackColor(0, rowIndx, this->m_lightGrayBackgroundColor);

      // This is for grid sorting
      CDFTFeature *feature = new CDFTFeature(compRefDes, "", deviceType);  // No pin name on this row
      SetRowData(rowIndx, feature);
   }

   CString pinRefRowData;
   pinRefRowData.Format("    %s", pinName);
   int rowIndx = GetNumberRows();
   SetNumberRows(rowIndx+1);
   QuickSetText(0, rowIndx, pinRefRowData);
   SetRowHeight(rowIndx, 0);                 // Collapsed

   CString debugPinRef;
   debugPinRef.Format("%s|%s", compRefDes, pinName);

   // This is for grid sorting
   CDFTFeature *feature = new CDFTFeature(compRefDes, pinName, deviceType);
   SetRowData(rowIndx, feature);
}

void CPreconditionsGrid::SortAscending()
{
   // We sort on the feature stored with the row, by way of SetRowData().
   // That uses the "param" value of the row heading column.
   // So this works out to sorting on column -1.

   bool sortAscending = true;
   int ug_sort_flag = sortAscending ? UG_SORT_ASCENDING : UG_SORT_DESCENDING;

   int colnum = -1;
   int retval = CUGCtrl::SortBy(colnum, ug_sort_flag);

   ResetRowHeights(0);
}

int CPreconditionsGrid::OnSortEvaluate(CUGCell *cell1,CUGCell *cell2,int flags)
{
	// if one of the cells is NULL, do not compare its text
	if ( cell1 == NULL && cell2 == NULL )
		return 0;
	else if ( cell1 == NULL )
		return 1;
	else if ( cell2 == NULL )
		return -1;

	if(flags&UG_SORT_DESCENDING)
	{
		CUGCell *ptr = cell1;
		cell1 = cell2;
		cell2 = ptr;
	}

   CDFTFeature *feature01 = (CDFTFeature*)(cell1->GetParam());
   CDFTFeature *feature02 = (CDFTFeature*)(cell2->GetParam());

   // This should not  happen. It means grid management elsewhere went wrong.
   // Basically this condition disables the sort.
   if (feature01 == NULL || feature02 == NULL)
      return 0;

   CString sortableRefDes01( CompPinStruct::getSortableReference(feature01->GetComponent()) );
   CString sortablePinDes01( CompPinStruct::getSortableReference(feature01->GetPin()) );
   CString sortablePinRef01( sortableRefDes01 + "." + sortablePinDes01 );

   CString sortableRefDes02( CompPinStruct::getSortableReference(feature02->GetComponent()) );
   CString sortablePinDes02( CompPinStruct::getSortableReference(feature02->GetPin()) );
   CString sortablePinRef02( sortableRefDes02 + "." + sortablePinDes02 );

   return sortablePinRef01.CompareNoCase(sortablePinRef02);
}



void CPreconditionsGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   // A click will get two event calls here, one for button down (updn = true) and one
   // for button up. We don't want to do this twice for a single click.
   // So react only to the button up, that is the end of the click event.
   if (updn)
      return;  // Ignore the button down.

   CDFTFeature *f = (CDFTFeature*)GetRowData(row);
   // This should never be NULL, is grid management problem if it ever is.
   if (f == NULL)
      return;

   // If pin name for this row is blank then it is a comp RefDes only row.
   // We only process the expand/collapse on RefDes only rows.
   CString pinName( f->GetPin() );

   // Toggle expand/collapse if pin name indicates this is a component RefDes only row.
   // React only if cell was first column cell.
   if (pinName.IsEmpty() && col == 0)
   {
      int rowCnt = GetNumberRows();

      CUGCell cell;
      GetCell(col, row, &cell);

      // We use the "+" and "-" in name to track expanded/collapsed state.
      // The char shows the current state.
      CString txt( cell.GetText() );
      bool isExpanded = (txt.Left(1).Compare("-") == 0);

      if (isExpanded)
      {
         // Is expanded, perform collapse.
         CollapseOneComponent(row);
         RedrawAll();
      }
      else
      {
         // Is collapsed, perform expand.
         ExpandOneComponent(row);
         RedrawAll();
      }
   }
}

void CPreconditionsGrid::ExpandOneComponent(int componentNameRowIndx)
{
   if (componentNameRowIndx >= 0 && componentNameRowIndx < GetNumberRows())
   {
      CDFTFeature *ff = (CDFTFeature*)GetRowData(componentNameRowIndx);
      if (ff == NULL)
         return;
      // Comp Row pinName should be blank. If this row's pin name is not blank then skip out.
      CString compRowPinName( ff->GetPin() );
      if (!compRowPinName.IsEmpty())
         return;

      int rowCnt = GetNumberRows();

      bool keepGoing = true;
      for (int rowIndx = componentNameRowIndx+1; rowIndx < rowCnt && keepGoing; rowIndx++)
      {
         CDFTFeature *f = (CDFTFeature*)GetRowData(rowIndx);
         CString pinName( (f != NULL) ? f->GetPin() : "" );

         if (pinName.IsEmpty())
         {
            keepGoing = false;
         }
         else
         {
            SetRowHeight(rowIndx, m_expandedRowHeight);
         }
      }

      CString componentNameText( QuickGetText(0, componentNameRowIndx) );
      componentNameText.SetAt(0, '-');  // Is now expanded, show operator to collapse.
      QuickSetText(0, componentNameRowIndx, componentNameText);
   }
}

void CPreconditionsGrid::CollapseOneComponent(int componentNameRowIndx)
{
   if (componentNameRowIndx >= 0 && componentNameRowIndx < GetNumberRows())
   {
      CDFTFeature *ff = (CDFTFeature*)GetRowData(componentNameRowIndx);
      if (ff == NULL)
         return;
      // Comp Row pinName should be blank. If this row's pin name is not blank then skip out.
      CString compRowPinName( ff->GetPin() );
      if (!compRowPinName.IsEmpty())
         return;

      int rowCnt = GetNumberRows();

      bool keepGoing = true;
      for (int rowIndx = componentNameRowIndx+1; rowIndx < rowCnt && keepGoing; rowIndx++)
      {
         CDFTFeature *f = (CDFTFeature*)GetRowData(rowIndx);
         CString pinName( (f != NULL) ? f->GetPin() : "" );

         if (pinName.IsEmpty())
         {
            keepGoing = false;
         }
         else
         {
            SetRowHeight(rowIndx, 0);
         }
      }

      CString componentNameText( QuickGetText(0, componentNameRowIndx) );
      componentNameText.SetAt(0, '+');  // Is now collapsed, show operator to expand.
      QuickSetText(0, componentNameRowIndx, componentNameText);
   }
}

void CPreconditionsGrid::ResetRowHeights(int startRow)
{
   CString curRefDes;
   bool curIsExpanded = false;
   bool curIsCollapsed = false;

   for (int rowIndx = startRow; rowIndx < GetNumberRows(); rowIndx++)
   {
      CDFTFeature *f = (CDFTFeature*)GetRowData(rowIndx);

      if (f == NULL) 
      {
         // Shouldn't happen. Make row visible.
         SetRowHeight(rowIndx, GetExpandedRowHeight());
      }
      else
      {
         CString cellTxt( QuickGetText(0, rowIndx) );
         bool isExpanded = (cellTxt.Left(1).Compare("-") == 0);
         bool isCollapsed = (cellTxt.Left(1).Compare("+") == 0);

         // Update cur only if state is explicit on this row
         if (isExpanded || isCollapsed)
         {
            curIsExpanded = isExpanded;
            curIsCollapsed = isCollapsed;
            // A row that explicitly has + or - should always be expanded.
            SetRowHeight(rowIndx, GetExpandedRowHeight());
         }
         else
         {
            // Apply current running state to this row
            if (curIsExpanded)
               SetRowHeight(rowIndx, GetExpandedRowHeight());
            else if (curIsCollapsed)
               SetRowHeight(rowIndx, 0);
            // else no change.
         }
      }
   }
}




/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CNetConditionsGrid::CNetConditionsGrid()
{
   //SetSortEnabled(false);  // Turn off grid sort on heading left click
}

//----------------------------------------------------------------------------------

void CNetConditionsGrid::SetRowCellsToCheckBox(int row)
{
   // Not the first column and not the last column, but all columns in between.
   int rowCnt = this->GetNumberRows();
   int colCnt = this->GetNumberCols();

   for (int colIndx = 1; colIndx < (colCnt-1); colIndx++)
   {
      CUGCell cell;

      GetCell(colIndx, row, &cell); 
      cell.SetCellType(UGCT_CHECKBOX);  // To make it a checkbox cell
      cell.SetCellTypeEx(UGCT_CHECKBOXCHECKMARK); // To make the checkbox use checkmark as opposed to cross mark.
      SetCell(colIndx, row, &cell);
   }
}

//----------------------------------------------------------------------------------


void CNetConditionsGrid::SizeToFit(CRect &rect)
{
   int width = rect.Width();

   int perColWidth = width / 8;

   SetColWidth(0, perColWidth);
   SetColWidth(1, perColWidth);
   SetColWidth(2, perColWidth);
   SetColWidth(3, perColWidth);
   SetColWidth(4, perColWidth);
   SetColWidth(5, perColWidth);
   SetColWidth(6, perColWidth);
   SetColWidth(7, perColWidth);

   RedrawAll();
}

//----------------------------------------------------------------------------------

void CNetConditionsGrid::OnSetup()
{
#ifdef CELL_EDIT
   DWORD dwStyle = 0;
   if ( m_myCUGEdit.GetSafeHwnd())
      m_myCUGEdit.DestroyWindow();

   dwStyle |= WS_CHILD|WS_VISIBLE;
   // create the edit control with specified style
   m_myCUGEdit.Create( dwStyle, CRect(0,0,0,0), this, 320001/*somerandom_ID*/ );
   m_myCUGEdit.SetAutoSize(TRUE);
   m_myCUGEdit.m_ctrl = this;
#endif

   SetNumberCols(8);

   SetNumberRows(1, false);
   SetExpandedRowHeight( GetRowHeight(0) );
   SetNumberRows(0, false);

   QuickSetText(0, -1, "Net");
   QuickSetText(1, -1, "# of Pins");
   QuickSetText(2, -1, "Net Type");
   QuickSetText(3, -1, "No Probe");
   QuickSetText(4, -1, "Test Resources");
   QuickSetText(5, -1, "Power Inj. Resources");
   QuickSetText(6, -1, "Voltage");
   QuickSetText(7, -1, "Current");

   // Numeric columns, right justify numbers.
   CUGCell cell;

   // # pins
   GetColDefault(1, &cell);
   cell.SetAlignment(UG_ALIGNRIGHT);
   SetColDefault(1, &cell);

   // Test Resources
   GetColDefault(4, &cell);
   cell.SetAlignment(UG_ALIGNRIGHT);
   SetColDefault(4, &cell);

   // Power Inj. Resources
   GetColDefault(5, &cell);
   cell.SetAlignment(UG_ALIGNRIGHT);
   SetColDefault(5, &cell);

   SetMultiSelectMode(TRUE);
   SetHighlightRow(TRUE) ;
   SetCurrentCellMode(UG_CELLMODE_HIGHLIGHT);
   ClearSelections();


   //BestFit(0, GetNumberCols()-1, myUG_FIT_ALL_ROWS, UG_BESTFIT_TOPHEADINGS);
   SetColWidth(-1, 0); // get rid of "row heading"
}

void CNetConditionsGrid::OnLClicked(int col,long row, int updn, RECT *rect, POINT *point, int processed) 
{
   // A click will get two event calls here, one for button down (updn = true) and one
   // for button up. We don't want to do this twice for a single click.
   // There is no in-grid cell editing for this grid. Clicks in this grid only do selection.
   // They style of selection is whole-row selection for the UI, but that is not a built-in
   // selection mode for UG. So upon button up, scan the grid for any selected cells, for
   // each selected cell select the entire row.

   

   if (updn)  // Button Down
   {
      // No op
   }
   else // Button Up
   {
      for (int rowIndx = 0; rowIndx < GetNumberRows(); rowIndx++)
      {
         bool rowFinished = false;
         for (int colIndx = 0; colIndx < GetNumberCols() && !rowFinished; colIndx++)
         {
            BOOL isSelected = IsSelected(colIndx, rowIndx, NULL);
            if (isSelected)
            {
               for (int selColIndx = 0; selColIndx < GetNumberCols(); selColIndx++)
               {
                  Select(selColIndx, rowIndx);
               }
               rowFinished = true; // Don't need to look at any more cols on this row
            }
         }
      }
   }
}

void CNetConditionsGrid::ExpandLayerGroup(int groupNameRowIndx)
{
   int rowCnt = GetNumberRows();
   int lastColIndx = GetNumberCols() - 1;

   bool keepGoing = true;
   for (int rowIndx = groupNameRowIndx+1; rowIndx < rowCnt && keepGoing; rowIndx++)
   {
      CString cellText( QuickGetText(lastColIndx, rowIndx) );
      int layerType = atoi(cellText);
      if (layerType < 0)
      {
         keepGoing = false;
      }
      else
      {
         SetRowHeight(rowIndx, m_expandedRowHeight);
      }
   }

   CString groupNameText( QuickGetText(0, groupNameRowIndx) );
   groupNameText.SetAt(0, '-');  // Is now expanded, show operator to collapse.
   QuickSetText(0, groupNameRowIndx, groupNameText);
}

 void CNetConditionsGrid::CollapseLayerGroup(int groupNameRowIndx)
 {
   int rowCnt = GetNumberRows();
   int lastColIndx = GetNumberCols() - 1;

   bool keepGoing = true;
   for (int rowIndx = groupNameRowIndx+1; rowIndx < rowCnt && keepGoing; rowIndx++)
   {
      CString cellText( QuickGetText(lastColIndx, rowIndx) );
      int layerType = atoi(cellText);
      if (layerType < 0)
      {
         keepGoing = false;
      }
      else
      {
         SetRowHeight(rowIndx, 0);
      }
   }

   CString groupNameText( QuickGetText(0, groupNameRowIndx) );
   groupNameText.SetAt(0, '+');  // Is now collapsed, show operator to expand.
   QuickSetText(0, groupNameRowIndx, groupNameText);
 }

