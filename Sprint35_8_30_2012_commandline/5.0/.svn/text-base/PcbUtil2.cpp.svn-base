// $Header: /CAMCAD/5.0/PcbUtil2.cpp 40    3/09/07 8:41p Kurt Van Ness $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright ?1994-2000. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"               
#include "graph.h"
#include "pcbutil.h"
#include "pcbutil2.h"
#include "attrib.h"
#include "geomlib.h"
#include <math.h>
#include <float.h>
#include "crypt.h"
#include "RwLib.h"
#include <afxtempl.h>
#include "DFT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define TestManufacturingRefDes

#if defined(TestManufacturingRefDes)
void testManufacturingRefDes(CCEtoODBDoc& camCadDoc)
{
   static bool init = false;

   if (!init)
   {
      BlockStruct* resistorBlock = camCadDoc.Find_Block_by_Name("1206",1);
      BlockStruct* ledBlock      = camCadDoc.Find_Block_by_Name("LED_LED/RA",1);

      if (resistorBlock != NULL)
      {
         int numInserts = 36;
         double deltaAngle = 2*Pi/numInserts;
         double rTop = 1;
         double rBottom = 1.5;
         double rLedTop = 2;
         double rLedBottom = 2.5;
         CString topRefDes,bottomRefDes,ledTopRefDes,ledBottomRefDes;
         DataStruct* data;

         for (int index = 0;index < numInserts;index++)
         {
            topRefDes.Format("R%d",index);
            bottomRefDes.Format("C%d",index);
            ledTopRefDes.Format("LT%d",index);
            ledBottomRefDes.Format("LB%d",index);

            double angle = index * deltaAngle;
            double angle90 = angle + Pi/2.;
            double x = cos(angle);
            double y = sin(angle);

            data = Graph_Block_Reference(resistorBlock->getName(),topRefDes      ,1,x*rTop      ,y*rTop      ,angle,FALSE,1.,-1,0);
            data->getInsert()->setInsertType(insertTypePcbComponent);

            data = Graph_Block_Reference(resistorBlock->getName(),bottomRefDes   ,1,x*rBottom   ,y*rBottom   ,angle,TRUE ,1.,-1,0);
            data->getInsert()->setInsertType(insertTypePcbComponent);

            data = Graph_Block_Reference(resistorBlock->getName()     ,ledTopRefDes   ,1,x*rLedTop   ,y*rLedTop   ,angle90,FALSE,1.,-1,0);
            data->getInsert()->setInsertType(insertTypePcbComponent);

            data = Graph_Block_Reference(resistorBlock->getName()     ,ledBottomRefDes,1,x*rLedBottom,y*rLedBottom,angle90,TRUE ,1.,-1,0);
            data->getInsert()->setInsertType(insertTypePcbComponent);
         }
      }

      init = true;
   }
}
#endif

void update_manufacturingrefdes(CCEtoODBDoc *doc, FileStruct *file, BlockStruct& pcbBlock, 
   double minRefDesHeight, double maxRefDesHeight, int layerNumber, bool nevermirror)
{
#if defined(TestManufacturingRefDes)
   testManufacturingRefDes(*doc);
#endif

   CInsertTypeMask insertTypeMask(insertTypePcbComponent, insertTypeDie);
   for (CDataListIterator insertIterator(pcbBlock.getDataList(),insertTypeMask);insertIterator.hasNext();)
   {
      DataStruct* np = insertIterator.getNext();
      InsertStruct* insert = np->getInsert();

      CString refDes = insert->getRefname();
      int refDesLen  = refDes.GetLength();
      bool mirrorFlag = false;
      //bool mirrorFlag = (insert->getMirrorFlags() != 0) && !nevermirror;

      if (refDesLen <= 0)
      {
         break;
      }

      // already flagged as SMD skip SMD test
      BlockStruct* block = doc->getBlockAt(insert->getBlockNumber());
      doc->validateBlockExtents(block);

      // length of refdes
      double blockWidth  = fabs(block->getXmax() - block->getXmin());
      double blockHeight = fabs(block->getYmax() - block->getYmin());
      double rotationDegrees = normalizeDegrees(insert->getAngleDegrees());
      double charheight = minRefDesHeight;
      double charrotation = 0;

      double targetWidth,targetHeight;
      bool horizontalFlag = (blockWidth >= blockHeight);
      bool flipTextFlag;

      if (horizontalFlag)
      {
         targetWidth  = blockWidth;
         targetHeight = blockHeight;

         flipTextFlag = (rotationDegrees > 90 + 22 && rotationDegrees < 270 + 22);
         charrotation = 0.;
      }
      else
      {
         targetWidth  = blockHeight;
         targetHeight = blockWidth;

         flipTextFlag = (rotationDegrees > 22 && rotationDegrees < 180 + 22);
         charrotation = 270;
      }

      if (flipTextFlag != mirrorFlag)
      {
         charrotation += 180;
      }

      if (targetHeight > maxRefDesHeight)
      {
         charheight = maxRefDesHeight;
      }

      if (charheight*refDesLen > targetWidth)
      {
         charheight = targetWidth / refDesLen;
      }

      if (targetHeight < minRefDesHeight)
      {
         charheight = minRefDesHeight;
      }

      CPoint2d origin = block->getExtent().getCenter();

      doc->SetVisAttrib(&np->getAttributesRef(),doc->IsKeyWord(ATT_REFNAME, 0),
            VT_STRING,
            refDes.GetBuffer(),
            origin.x, origin.y, degreesToRadians(charrotation),
            charheight, charheight*0.8, 0, 0, TRUE, SA_OVERWRITE, 0L,  layerNumber, nevermirror,
            horizontalPositionCenter,verticalPositionCenter);
   } 

   return;
}

void update_proberefdes(CCEtoODBDoc *doc, FileStruct *file, BlockStruct& pcbBlock, bool nevermirror)
{
   // Size 0.0 results probe refname halt set to default proportion of feature size
   double textsize = 0.0;

   // Get testplan, for possible user specified text size from probe templates.
   CTestPlan *testplan = NULL;
   /*if (file != NULL && doc->GetCurrentDFTSolution(*file) != NULL)
      testplan = doc->GetCurrentDFTSolution(*file)->GetTestPlan();*/


   for (POSITION pos = pcbBlock.getHeadDataInsertPosition();pos != NULL;)
   {
      DataStruct* np = pcbBlock.getNextDataInsert(pos);
      InsertStruct* insert = np->getInsert();

      if (insert->getInsertType() == insertTypeTestProbe && !insert->getRefname().IsEmpty())   
      {
         Attrib *attrib = NULL;
         if (np->lookUpAttrib(doc->IsKeyWord(ATT_REFNAME, 0), attrib) && attrib != NULL)
         {
            // It exists, only change mirror, leave rest alone
            attrib->setMirrorDisabled(nevermirror);
         }
         else
         {
            // Doesn't exist,make one.

            double drillsize = -1.0; // considered "not set"
            double textsize = 0.0;   // will get default height relative to drill size

            if (testplan != NULL)
            {
               // Get drill and text sizes from test plan
               BlockStruct *probeBlk = doc->getBlockAt(np->getInsert()->getBlockNumber());
               CString probeTemplateName = probeBlk->getName();
               int indx = probeTemplateName.Find("_Bot");
               if (indx < 0)
                  indx = probeTemplateName.Find("_Top");

               if (indx > -1)
                  probeTemplateName.Truncate(indx);

               if (probeBlk != NULL)
               {
                  CDFTProbeTemplate *pTemplate = NULL;

                  POSITION dummyPos = NULL;
                  if (insert->getPlacedTop())
                     pTemplate = testplan->GetProbes().Find_TopProbes(probeTemplateName, dummyPos);
                  else
                     pTemplate = testplan->GetProbes().Find_BotProbes(probeTemplateName, dummyPos);

                  if (pTemplate != NULL)
                  {
                     drillsize = pTemplate->GetDrillSize() * Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
                     textsize = pTemplate->GetTextSize() * Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
                  }
               }
            }
            
            if (drillsize < 0.0)
            {
               // Try to get from existing probe
               BlockStruct *probeBlk = doc->getBlockAt(np->getInsert()->getBlockNumber());
               BlockStruct *drillBlk = GetDrillInGeometry(doc, probeBlk);
               if (drillBlk != NULL && drillBlk->isDrillHole())
                  drillsize = (double)drillBlk->getToolSize();
            }

            if (drillsize < 0.0)
               drillsize = 50.0 * Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits()); // punt

            CreateTestProbeRefnameAttr(doc, np, np->getInsert()->getRefname(), drillsize, textsize, nevermirror);
         }

      } 

   }
   return;
}

/******************************************************************************
* MakeManufacturingRefdes
*/

void CCEtoODBDoc::OnMakeManufacturingRefdes() 
{
   DoMakeManufacturingRefDes(this, TRUE);
}

void DoMakeManufacturingRefDes(CCEtoODBDoc *doc, BOOL ShowDialog, bool applyToComps, bool applyToProbes) 
{
   ManufacturingRefDes dlg;

   double minref = 0.01 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
   double maxref = 0.05 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
   int decimals = GetDecimals(doc->getSettings().getPageUnits());

   dlg.m_min.Format("%.*lf", decimals, minref);
   dlg.m_max.Format("%.*lf", decimals, maxref);

   dlg.m_bottom = "REFDES_BOT";
   dlg.m_top    = "REFDES_TOP";

   // find silkscreen top and bottom
   for (int j=0;j < doc->getMaxLayerIndex();j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      LayerStruct *layer = doc->getLayerArray()[j];

      if (layer == NULL)
			continue; // could have been deleted.

      if (layer->getLayerType() == LAYTYPE_SILK_TOP)
         dlg.m_top = layer->getName();

      if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM)
         dlg.m_bottom = layer->getName();
   }
   
	POSITION filePos = NULL;
   for (filePos=doc->getFileList().GetHeadPosition(); filePos != NULL;)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      if (file->getBlockType() != BLOCKTYPE_PCB)
         continue;

      BlockStruct* pcbBlock = file->getBlock();      
      bool done = false;

      for (POSITION pos=pcbBlock->getHeadDataInsertPosition(); !done && pos != NULL;)
      {
         DataStruct *data = pcbBlock->getNextDataInsert(pos);
      
         if (data->getInsert()->getInsertType() != insertTypePcbComponent && data->getInsert()->getInsertType() != insertTypeDie)
				break; 

         // find default ATT_REFNAME layer
         Attrib* attrib = is_attvalue(doc, data->getAttributes(), ATT_REFNAME, 0);
         if (attrib) 
         {
            if (attrib->getLayerIndex() != 0)  // must have a layer assigned
            {
               LayerStruct *attribLayer = doc->FindLayer(attrib->getLayerIndex());

               if (attribLayer == NULL)
                  continue;

               // the attribute is always from the top side, because a insert->mirror causes the attribute layer to be swapped.
               dlg.m_top = attribLayer->getName();
               attribLayer = doc->FindLayer(attribLayer->getMirroredLayerIndex());
               dlg.m_bottom = attribLayer->getName();
               done = true;
            }
         }
      }
   }

   dlg.m_applyToComponents = applyToComps;
   dlg.m_applyToProbes = applyToProbes;
   dlg.m_neverMirrorComps = FALSE;
   dlg.m_neverMirrorProbes = FALSE;

   if (ShowDialog)
   {
      if (dlg.DoModal() != IDOK)
         return;
   }

   minref = atof(dlg.m_min);
   maxref = atof(dlg.m_max);

   // do here   
   for (filePos=doc->getFileList().GetHeadPosition(); filePos != NULL;)
   {
      FileStruct *file = doc->getFileList().GetNext(filePos);

      if (file->isShown() && !file->isHidden() && file->getBlockType() == BLOCKTYPE_PCB)
      {
         doc->PrepareAddEntity(file);

         if (dlg.m_applyToComponents)
         {
            // make refdes layers
            int layerIndex = Graph_Level(dlg.m_top, "", 0);
            LayerStruct *refDesLayer = doc->FindLayer(layerIndex);
            if (refDesLayer->getLayerType() == 0) // only if no type yet assigned
               refDesLayer->setLayerType(LAYTYPE_TOP);

            layerIndex = Graph_Level(dlg.m_bottom, "", 0);
            refDesLayer = doc->FindLayer(layerIndex);
            if (refDesLayer->getLayerType() == 0) // only if no type yet assigned
               refDesLayer->setLayerType(LAYTYPE_BOTTOM);

            Graph_Level_Mirror(dlg.m_top, dlg.m_bottom, "");

            update_manufacturingrefdes(doc, file, *(file->getBlock()), minref, maxref, Graph_Level(dlg.m_top, "", 0), dlg.m_neverMirrorComps?true:false);
         }

         if (dlg.m_applyToProbes)
         {
            update_proberefdes(doc, file, *(file->getBlock()), dlg.m_neverMirrorProbes?true:false);
         }
      }
   }


   doc->UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// ManufacturingRefDes dialog
ManufacturingRefDes::ManufacturingRefDes(CWnd* pParent /*=NULL*/)
   : CDialog(ManufacturingRefDes::IDD, pParent)
   , m_applyToComponents(TRUE)
   , m_applyToProbes(FALSE)
{
   //{{AFX_DATA_INIT(ManufacturingRefDes)
   m_max = _T("");
   m_min = _T("");
   m_bottom = _T("");
   m_top = _T("");
   m_neverMirrorComps = FALSE;
   m_neverMirrorProbes = FALSE;
   //}}AFX_DATA_INIT
}

void ManufacturingRefDes::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ManufacturingRefDes)
   DDX_Text(pDX, IDC_MAX, m_max);
   DDX_Text(pDX, IDC_MIN, m_min);
   DDX_Text(pDX, IDC_BOTTOM, m_bottom);
   DDX_Text(pDX, IDC_TOP, m_top);
   DDX_Check(pDX, IDC_NEVER_MIRROR_COMPS, m_neverMirrorComps);
   DDX_Check(pDX, IDC_NEVER_MIRROR_PROBES, m_neverMirrorProbes);
   //}}AFX_DATA_MAP
   DDX_Check(pDX, IDC_CHK_COMPONENTS, m_applyToComponents);
   DDX_Check(pDX, IDC_CHK_PROBES, m_applyToProbes);
}

BEGIN_MESSAGE_MAP(ManufacturingRefDes, CDialog)
   //{{AFX_MSG_MAP(ManufacturingRefDes)
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_CHK_COMPONENTS, OnBnClickedChkComponents)
   ON_BN_CLICKED(IDC_CHK_PROBES, OnBnClickedChkProbes)
END_MESSAGE_MAP()

BOOL ManufacturingRefDes::OnInitDialog()
{
	UpdateData(FALSE);

   OnBnClickedChkComponents();
   OnBnClickedChkProbes();

   return TRUE;
}

void ManufacturingRefDes::OnBnClickedChkComponents()
{
   // Enable/disable items in group

   UpdateData(TRUE);

   GetDlgItem(IDC_MIN_LABEL)->EnableWindow(m_applyToComponents);
   GetDlgItem(IDC_MAX_LABEL)->EnableWindow(m_applyToComponents);
   GetDlgItem(IDC_TOP_LABEL)->EnableWindow(m_applyToComponents);
   GetDlgItem(IDC_BOTTOM_LABEL)->EnableWindow(m_applyToComponents);

   GetDlgItem(IDC_MIN)->EnableWindow(m_applyToComponents);
   GetDlgItem(IDC_MAX)->EnableWindow(m_applyToComponents);
   GetDlgItem(IDC_TOP)->EnableWindow(m_applyToComponents);
   GetDlgItem(IDC_BOTTOM)->EnableWindow(m_applyToComponents);

   GetDlgItem(IDC_NEVER_MIRROR_COMPS)->EnableWindow(m_applyToComponents);
}

void ManufacturingRefDes::OnBnClickedChkProbes()
{
   // Enable/disable items in group

   UpdateData(TRUE);

   GetDlgItem(IDC_NEVER_MIRROR_PROBES)->EnableWindow(m_applyToProbes);
}



/////////////////////////////////////////////////////////////////////////////
// AttribRefresh dialog


AttribRefresh::AttribRefresh(CWnd* pParent /*=NULL*/)
   : CDialog(AttribRefresh::IDD, pParent)
{
   //{{AFX_DATA_INIT(AttribRefresh)
   m_typetocomp = FALSE;
   m_method = -1;
   m_geomtocomp = FALSE;
   //}}AFX_DATA_INIT
}


void AttribRefresh::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AttribRefresh)
   DDX_Check(pDX, IDC_TYPECOMP, m_typetocomp);
   DDX_Radio(pDX, IDC_TYPETOCOMP_METHOD, m_method);
   DDX_Check(pDX, IDC_GEOM2COMP, m_geomtocomp);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AttribRefresh, CDialog)
   //{{AFX_MSG_MAP(AttribRefresh)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////

void CTypeChanger::Add(const CString& regularExpressionString, CRegularExpressionList &list)
{
   CRegularExpression* regularExpression = new CRegularExpression(CRegularExpression::fixRegularExpression(regularExpressionString),true);
   regularExpression->setCaseSensitive(false);

   list.AddTail(regularExpression);
}

bool CTypeChanger::ParseCommand(CString commandLine)
{
   // Save mapping command data.
   // Only one geometry name allowed per line, ignore anything after a good param.
   // Return true if recognized command, even if command is illformed, e.g. has no arguments, it is still recognized.
   // Return false if not a recognized command.

   bool recognized = false;

   CSupString cmd(commandLine);
   CStringArray args;
   cmd.ParseQuote(args, " \t\n", true /*trim*/);

   if (args.GetCount() > 0)
   {
      if (args.GetAt(0).CompareNoCase(".MAKE_INTO_TESTPOINT") == 0)
      {
         recognized = true;
         if (args.GetCount() > 1)
            Add(args.GetAt(1), m_testpoints);
      }
      else if (args.GetAt(0).CompareNoCase(".MAKE_INTO_FIDUCIAL") == 0)
      {
         recognized = true;
         if (args.GetCount() > 1)
            Add(args.GetAt(1), m_fiducials);
      }
      else if (args.GetAt(0).CompareNoCase(".MAKE_INTO_TOOLING") == 0)
      {
         recognized = true;
         if (args.GetCount() > 1)
            Add(args.GetAt(1), m_tooling);
      }
   }

   return recognized;
}

bool CTypeChanger::HasExpressionsToApply()
{
   int totalCnt = m_fiducials.GetCount() + m_testpoints.GetCount() + m_tooling.GetCount();
   return (totalCnt > 0);
}

void CTypeChanger::Apply(CCEtoODBDoc *doc, DataStruct *data, BlockStruct *insertedBlock, CRegularExpressionList &expressionlist, InsertTypeTag insertType)
{
   // It is already established that data is an insert and the inserted blck exists.

   CString blkName = insertedBlock->getName();
   if (expressionlist.isStringMatched(blkName))
   {
      if (data->getInsert()->getInsertType() != insertType)
      {
         // Adjust insert type to designated type.
         data->getInsert()->setInsertType(insertType);

         // Adjust inserted block, if necessary. 
         // Only fids and testpoints (so far) need adjustment, Tooling is (theoretically) okay as-is.
         switch (insertType)
         {
         case insertTypeFiducial:
            insertedBlock->setBlockType(blockTypeFiducial); // faster to just do it than test and then do it or not
            break;
         case insertTypeTestPoint:
            // testpoint validation takes longer, only do it if actually converting block
            if (insertedBlock->getBlockType() != blockTypeTestPoint)
            {
               insertedBlock->setBlockType(blockTypeTestPoint);
               doc->ValidateTestpoint(*insertedBlock);
            }
            // spec says to set this attrib too
            doc->SetUnknownAttrib(&data->getDefinedAttributes(), ATT_TEST, "", SA_OVERWRITE, NULL);
            break;
         }
      }
   }
}

bool CTypeChanger::Apply(CCEtoODBDoc *doc, BlockStruct *block)
{
   // PCB and Panel are exempt from conversion, but inserts in those blocks get converted.

   if (doc != NULL && block != NULL)
   {
      POSITION pos = block->getHeadDataInsertPosition();
      while (pos != NULL)
      {
         DataStruct *data = block->getNextDataInsert(pos);

         if (data->isInsert())
         {
            int insertedBlkNum = data->getInsert()->getBlockNumber();
            BlockStruct *insertedBlk = doc->getBlockAt(insertedBlkNum);
            if (insertedBlk != NULL)
            {
               InsertTypeTag insertType = data->getInsert()->getInsertType();
               if (insertType != insertTypePcb)
               {
                  Apply(doc, data, insertedBlk, m_testpoints, insertTypeTestPoint);
                  Apply(doc, data, insertedBlk, m_fiducials, insertTypeFiducial);
                  Apply(doc, data, insertedBlk, m_tooling, insertTypeDrillTool);
               }
            
               // Now process inserts within this inserted block
               Apply(doc, insertedBlk);

            }
         }
      }
   }

   return true; // not really checking anything yet
}

bool CTypeChanger::Apply(CCEtoODBDoc *doc, FileStruct *file)
{
   // Apply to given file in doc. These TypeChanger commands apply only
   // to PCB and Panel files. Skip any other kind.

   if (doc != NULL && file != NULL && HasExpressionsToApply() &&
      (file->getBlockType() == blockTypePanel || file->getBlockType() == blockTypePcb))
   {
      return Apply(doc, file->getBlock());
   }

   return false;
}

bool CTypeChanger::Apply(CCEtoODBDoc *doc)
{
   // Apply to all files in doc.

   bool retval = false;

   if (doc != NULL && HasExpressionsToApply())
   {
      POSITION filePos = doc->getCamCadData().getFileList().GetHeadPosition();
      while (filePos != NULL)
      {
         FileStruct *file = doc->getCamCadData().getFileList().GetNext(filePos);    
         retval |= Apply(doc, file);
      }
   }

   return retval;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
