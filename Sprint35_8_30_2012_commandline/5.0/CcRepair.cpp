// $Header: /CAMCAD/5.0/CcRepair.cpp 25    3/22/07 12:53a Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2002. All Rights Reserved.

#include "StdAfx.h"
#include "CcRepair.h"
#include "CCEtoODB.h"
#include "RwLib.h"
#include "RwUiLib.h"
#include "Graph.h"
#include <set>
#include "Gauge.h"
//_____________________________________________________________________________
// Keep
bool CCEtoODBDoc::verifyAndRepairData()
{
   CCamCadDocRepairTool repairTool(*this);

   return repairTool.verifyAndRepairData();
}

////_____________________________________________________________________________
CBlockStructList::CBlockStructList(bool isContainer)
: CTypedPtrListContainer<BlockStruct*>(isContainer)
{
}

//_____________________________________________________________________________
CCamCadDocRepairTool::CCamCadDocRepairTool(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_verificationLogFile(NULL)
, m_undefinedLayerIndex(0)
, m_floatingUndefinedBlock(NULL)
, m_undefinedBlock(NULL)
, m_recursiveCheckStack(false)
{
   m_netNameKW = m_camCadDoc.IsKeyWord(ATT_NETNAME, 0);
}

CCamCadDocRepairTool::~CCamCadDocRepairTool()
{
   delete m_verificationLogFile;
}

void CCamCadDocRepairTool::setVerificationLogFilePath(const CString& logFilePath)
{
   CHtmlFileWriteFormat* verificationLogFile = new CHtmlFileWriteFormat(1024);

   if (!verificationLogFile->open(logFilePath))
   {
      formatMessageBoxApp(MB_ICONSTOP,"Could not open the verification log, '%s'",
         logFilePath);

      delete verificationLogFile;
      m_verificationLogFile = new CNullWriteFormat();
   }
   else
   {
      m_verificationLogFile = verificationLogFile;
      m_verificationLogFile->writef("%s\n",(const char*)getApp().getCamCadSubtitle());
      m_verificationLogFile->writef(PrefixStatus,"CAMCAD verification log\n");
   }
}

CWriteFormat& CCamCadDocRepairTool::getVerificationLogFile()
{
   if (m_verificationLogFile == NULL)
   {
	   if (!m_camCadDoc.getSettings().getWriteVerificationLog())
	   {
         m_verificationLogFile = new CNullWriteFormat();
	   }
      else
      {
         CFilePath logFilePath(m_camCadDoc.CCFileName);
         CString fileName = logFilePath.getBaseFileName() + "VerificationLog";

         logFilePath.setBaseFileName(fileName);
         logFilePath.setExtension("htm");

         setVerificationLogFilePath(logFilePath.getPath());
      }
   }

   return *m_verificationLogFile;
}

BlockStruct& CCamCadDocRepairTool::getFloatingUndefinedBlock()
{  
   CString blockName("$FloatingCcRepairBlock$");

   if (m_floatingUndefinedBlock == NULL)
   {
      m_floatingUndefinedBlock = m_camCadDoc.Find_Block_by_Name(blockName,-1);

      if (m_floatingUndefinedBlock == NULL)
      {
         const int widthIndex = 0;
         m_floatingUndefinedBlock = Graph_Block_On(GBO_APPEND,blockName,-1,0);

         LayerStruct* floatingLayer = m_camCadDoc.getDefinedFloatingLayer();
         DataStruct* polyData = Graph_PolyStruct(floatingLayer->getLayerIndex(),0,false);

         double diameter = .2;
         double radius   = diameter / 2.;
         double unit     = radius   / 10.;

         Graph_Poly(polyData,widthIndex,false,false,true);
         Graph_Vertex(0., radius,1.);
         Graph_Vertex(0.,-radius,1.);
         Graph_Vertex(0., radius,0.);

         double r2 = radius / 3.;
         Graph_Poly(polyData,widthIndex,false,false,false);
         Graph_Vertex(-r2,0.,-tan(degreesToRadians(240.)/4.));
         Graph_Vertex( r2/2.,-r2*SqrtOf3/2.,tan(degreesToRadians(60.)/4.));
         Graph_Vertex( 0.,-r2*SqrtOf3,0.);

         Graph_Poly(polyData,widthIndex,true,false,true);
         Graph_Vertex( 0.,-r2*SqrtOf3 - r2/6.,1.);
         Graph_Vertex( 0.,-r2*SqrtOf3 - r2/6. - r2/6.,1.);
         Graph_Vertex( 0.,-r2*SqrtOf3 - r2/6.,1.);

         //Graph_Poly(polyData,widthIndex,false,false,true);
         //Graph_Vertex(-4.*unit, 6.*unit,1.);
         //Graph_Vertex(-4.*unit, 2.*unit,1.);
         //Graph_Vertex(-4.*unit, 6.*unit,0.);

         //Graph_Poly(polyData,widthIndex,false,false,true);
         //Graph_Vertex( 4.*unit, 6.*unit,1.);
         //Graph_Vertex( 4.*unit, 2.*unit,1.);
         //Graph_Vertex( 4.*unit, 6.*unit,0.);

         //Graph_Poly(polyData,widthIndex,false,false,false);
         //Graph_Vertex(-6.*unit,-4.*unit,.5);
         //Graph_Vertex( 6.*unit,-4.*unit,0.);

         Graph_Block_Off();
      }
   }

   return *m_floatingUndefinedBlock;
}

BlockStruct& CCamCadDocRepairTool::getUndefinedBlock()
{  
   CString blockName("$CcRepairBlock$");

   if (m_undefinedBlock == NULL)
   {
      m_undefinedBlock = m_camCadDoc.Find_Block_by_Name(blockName,-1);

      if (m_undefinedBlock == NULL)
      {
         BlockStruct& floatingUndefinedBlock = getFloatingUndefinedBlock();

         m_undefinedBlock = Graph_Block_On(GBO_APPEND,blockName,-1,0);
         Graph_Block_Reference(floatingUndefinedBlock.getName(),"",-1,0.,0.,0.,0,1.,
            getUndefinedLayer().getLayerIndex(),-1);

         Graph_Block_Off();
      }
   }

   return *m_undefinedBlock;
}

LayerStruct& CCamCadDocRepairTool::getUndefinedLayer()
{
   LayerStruct* undefinedLayer = NULL;

   if (m_undefinedLayerIndex >= m_camCadDoc.getLayerCount())
   {
      undefinedLayer = m_camCadDoc.getDefinedLayerAt(m_undefinedLayerIndex);
      undefinedLayer->setName("CC Repair Layer");
   }
   else
   {
      undefinedLayer = m_camCadDoc.getDefinedLayerAt(m_undefinedLayerIndex);
   }

   return *undefinedLayer;
}

class CStringCaseInsensitiveLess{
public:
   bool operator()(CString str1, CString str2)
   {
      str1.MakeUpper();
      str2.MakeUpper();
      return str1 < str2;
   }
};

void CCamCadDocRepairTool::verifyGeometryNames()
{
   std::set<CString, CStringCaseInsensitiveLess> namesSet;
   m_changedNameMap.clear();

   for (int blockIndex = 0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* blockPtr( m_camCadDoc.getBlockAt(blockIndex) );
      if( blockPtr == NULL )
      {
         continue;
      }

      CString originalBlockName( blockPtr->getName() );
      if( originalBlockName.IsEmpty() )
      {
         continue;
      }

      if( !namesSet.insert( originalBlockName ).second )
      {
         CString blockName( originalBlockName );
         blockName.Append("_");
         do 
         {
            blockName.Append("1");
         } while( !namesSet.insert( blockName ).second );

         blockPtr->setName( blockName );
         m_changedNameMap.insert( std::pair<CString,CString>( blockName, originalBlockName ) );
      }
   }
}

void CCamCadDocRepairTool::undoVerifyGeometryNames()
{
   if( m_changedNameMap.empty() )
   {
      return;
   }

   std::map<CString,CString>::iterator it; 
   std::map<CString,CString>::const_iterator endIt( m_changedNameMap.end() );

   for (int blockIndex = 0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* blockPtr( m_camCadDoc.getBlockAt(blockIndex) );
      if( blockPtr == NULL )
      {
         continue;
      }

      CString blockName( blockPtr->getName() );
      it = m_changedNameMap.find( blockName );
      if( it != endIt )
      {
         blockPtr->setName( it->second );
      }
   }

   m_changedNameMap.clear();
}

bool CCamCadDocRepairTool::checkRecursiveReference(BlockStruct& block)
{
   bool retval = false;

   if (! block.getCheckedFlag())
   {
      switch (block.getBlockNumber())
      {
      case 2:
         {
            int iii = 3;
         }
         break;
      case 150:
         {
            int iii = 3;
         }
         break;
      case 151:
         {
            int iii = 3;
         }
         break;
      }

      POSITION foundPos = m_recursiveCheckStack.Find(&block);
      m_recursiveCheckStack.AddTail(&block);

      if (foundPos != NULL)
      {
         retval = true;

         getVerificationLogFile().writef(PrefixError,
            "Recursive block reference found for block %d, '%s' - Removed.  Reference chain follows\n",
            block.getBlockNumber(),block.getName());

         int levelIndex = 0;

         for (POSITION refPos = foundPos;refPos != NULL;)
         {
            BlockStruct* refBlock = m_recursiveCheckStack.GetNext(refPos);

            getVerificationLogFile().writef(PrefixStatus,
               "Level = %3d;  block %6d, '%s's\n",
               levelIndex,refBlock->getBlockNumber(),refBlock->getName());

            levelIndex++;
         }
      }
      else
      {
         if (block.isComplexAperture())
         {
            BlockStruct* subBlock = m_camCadDoc.getBlockAt(block.getComplexApertureSubBlockNumber());

            if (subBlock != NULL)
            {
               if (checkRecursiveReference(*subBlock))
               {
                  block.setComplexApertureSubBlockNumber(getUndefinedBlock().getBlockNumber());
               }
            }
         }

         CDataList& dataList = block.getDataList();

         for (POSITION pos = dataList.GetHeadPosition();pos != NULL;)
         {
            POSITION oldPos = pos;
            DataStruct* data = dataList.GetNext(pos);

            if (data == NULL)
            {
               dataList.RemoveAt(oldPos);
               continue;
            }

            if (data->getDataType() == dataTypeInsert)
            {
               BlockStruct* subBlock = m_camCadDoc.getBlockAt(data->getInsert()->getBlockNumber());

               if (subBlock != NULL)
               {
                  if (checkRecursiveReference(*subBlock))
                  {
                     //dataList.RemoveDataFromList(&m_camCadDoc,data,oldPos);
                     m_camCadDoc.removeDataFromDataList(dataList,data,oldPos);
                  }
               }
            }
         }
      }

      m_recursiveCheckStack.RemoveTail();
      block.setCheckedFlag(true);
   }

   return retval;
}
// Keep
bool CCamCadDocRepairTool::verifyAndRepairData()
{
   int badLayerReferenceCount = 0;
   int badSubblockReferenceCount = 0;
   m_undefinedLayerIndex = m_camCadDoc.getLayerCount();
   //COperationProgress *progress = NULL;

   /*bool showProgressBar = ! (getApp().SilentRunning ||
      (((CCEtoODBApp*)AfxGetApp())->UsingAutomation && !AfxGetMainWnd()->IsWindowVisible()) );

   if (showProgressBar)
      progress = new COperationProgress;*/

//////#define RemoveRecursiveReferencesEnabled
#ifdef  RemoveRecursiveReferencesEnabled
   // remove recursive references
   /*if (progress != NULL)
      progress->updateStatus("Removing recursive references",m_camCadDoc.getMaxBlockIndex());*/

   for (int blockIndex = 0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = m_camCadDoc.getBlockAt(blockIndex);

      if (block != NULL)
      {
         block->setCheckedFlag(false);
      }
   }

   for (int blockIndex = 0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = m_camCadDoc.getBlockAt(blockIndex);
      /*if (progress != NULL)
         progress->incrementProgress();*/

      if (block != NULL)
      {
         m_recursiveCheckStack.empty();

         checkRecursiveReference(*block);
      }
   }
#endif

   // verify and repair bad layer references

   /*if (progress != NULL)
      progress->updateStatus("Verifying data",m_camCadDoc.getMaxBlockIndex());*/

   for (int blockIndex = 0;blockIndex < m_camCadDoc.getMaxBlockIndex();blockIndex++)
   {
      BlockStruct* block = m_camCadDoc.getBlockAt(blockIndex);
      /*if (progress != NULL)
         progress->incrementProgress();*/

      if (block != NULL)
      {
         if (block->isComplexAperture())
         {
            int subBlockNumber = block->getComplexApertureSubBlockNumber();

            if (m_camCadDoc.getBlockAt(subBlockNumber) == NULL)
            {
               badSubblockReferenceCount++;

               BlockStruct& undefinedBlock = getUndefinedBlock();

               getVerificationLogFile().writef(PrefixError,
                  "Changed reference to bad subblock index of %d to %d on complex aperture %d\n",
                  subBlockNumber,undefinedBlock.getBlockNumber(),block->getBlockNumber());

               block->setComplexApertureSubBlockNumber(undefinedBlock.getBlockNumber());
            }
         }

         for (POSITION dataPos = block->getHeadDataPosition();dataPos != NULL;)
         {
            DataStruct* data = block->getNextData(dataPos);

            int layerIndex = data->getLayerIndex();
            int newLayerIndex = layerIndex;

            if (layerIndex < 0)
            {
               if (layerIndex < -1)
               {
                  newLayerIndex = m_undefinedLayerIndex;
               }
            }
            else if (layerIndex >= m_camCadDoc.getLayerCount())
            {
               newLayerIndex = m_undefinedLayerIndex;
            }
            else
            {
               LayerStruct* layer = m_camCadDoc.getLayerAt(layerIndex);

               if (layer == NULL)
               {
                  newLayerIndex = m_undefinedLayerIndex;
               }
            }

            if (newLayerIndex != layerIndex)
            {
               badLayerReferenceCount++;
               data->setLayerIndex(newLayerIndex);

               getVerificationLogFile().writef(PrefixError,
                  "Changed bad layer index of %d to %d on data %d in block %d\n",
                  layerIndex,newLayerIndex,data->getEntityNumber(),block->getBlockNumber());
            }

            switch (data->getDataType())
            {
            case dataTypeInsert:
               {
                  int subBlockNumber = data->getInsert()->getBlockNumber();

                  if (m_camCadDoc.getBlockAt(subBlockNumber) == NULL)
                  {
                     badSubblockReferenceCount++;
                     LayerStruct* layer = m_camCadDoc.getLayerAt(data->getLayerIndex());
                     
                     BlockStruct& undefinedBlock = ((layer == NULL) || layer->getFloating() ? getUndefinedBlock() : getFloatingUndefinedBlock());

                     getVerificationLogFile().writef(PrefixError,
                        "Changed reference to bad subblock index of %d to %d on insert %d in block %d\n",
                        subBlockNumber,undefinedBlock.getBlockNumber(),data->getEntityNumber(),block->getBlockNumber());

                     data->getInsert()->setBlockNumber(undefinedBlock.getBlockNumber());
                  }
               }
               break;

            case dataTypePoly:
               {                 
                  CPolyList* polyList =  data->getPolyList();
                  int zeroWidthIndex = m_camCadDoc.getZeroWidthIndex();

                  for (POSITION polyPos = polyList->GetHeadPosition();polyPos != NULL;)
                  {
                     CPoly* poly = polyList->GetNext(polyPos);

                     int widthIndex = poly->getWidthIndex();

                     if (widthIndex < 0 || widthIndex >= m_camCadDoc.getWidthTable().GetSize())
                     {
                        getVerificationLogFile().writef(PrefixError,
                           "Changed reference to bad width index of %d to %d on polyList entity %d in block %d\n",
                           widthIndex, zeroWidthIndex, data->getEntityNumber(), block->getBlockNumber());

                        poly->setWidthIndex(zeroWidthIndex);
                     }
                  }
               }
               break;

            case dataTypeText:
               {
                  int widthIndex = data->getText()->getPenWidthIndex();
                  if (widthIndex < 0 || widthIndex >= m_camCadDoc.getWidthTable().GetSize())
                  {
                     int zeroWidthIndex = m_camCadDoc.getZeroWidthIndex();

                     getVerificationLogFile().writef(PrefixError,
                        "Changed reference to bad width index of %d to %d on text entity %d in block %d\n",
                        widthIndex, zeroWidthIndex, data->getEntityNumber(), block->getBlockNumber());

                     data->getText()->setPenWidthIndex( zeroWidthIndex );
                  }
               }
               break;
            }
         }
      }
   }

	verifyDeviceType();
   verifyNetList();

   if (badLayerReferenceCount > 0)
   {
      getUndefinedLayer();
   }

   //   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   - 
   int disasterCount = 0;
   int errorCount    = 0;
   int warningCount  = 0;

   if (m_verificationLogFile != NULL)
   {
      disasterCount = m_verificationLogFile->getPrefixCount(PrefixDisaster);
      errorCount    = m_verificationLogFile->getPrefixCount(PrefixError);
      warningCount  = m_verificationLogFile->getPrefixCount(PrefixWarning);

      if (disasterCount > 0)
      {
         m_verificationLogFile->writef(PrefixSummary,
            "%d Disaster%s\n",
            disasterCount,
            ((disasterCount != 1) ? "s" : ""));
      }

      m_verificationLogFile->writef(PrefixSummary,
         "%d Error%s\n",
         errorCount,
         ((errorCount != 1) ? "s" : ""));

      m_verificationLogFile->writef(PrefixSummary,
         "%d Warning%s\n",
         warningCount,
         ((warningCount != 1) ? "s" : ""));

      CString logFilePath = m_verificationLogFile->getFilePath();
      delete m_verificationLogFile;
      m_verificationLogFile = NULL;

#ifndef NDEBUG
      //ShellExecute(AfxGetApp()->GetMainWnd()->m_hWnd,"open",logFilePath,NULL,NULL,SW_SHOW);
#endif
   }

   /*if (progress != NULL)
   {
      delete progress;
      progress = NULL;
   }*/

   return ((disasterCount == 0) && (errorCount == 0) && (warningCount == 0));
}

void CCamCadDocRepairTool::verifyDeviceType()
{	
	for (POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();filePos != NULL;)
	{
		FileStruct* file = m_camCadDoc.getFileList().GetNext(filePos);

      if (file != NULL)
      {
		   for (POSITION typePos = file->getTypeList().GetHeadPosition();typePos != NULL;)
		   {
			   TypeStruct* type = file->getTypeList().GetNext(typePos);

			   if (type != NULL)
            {
			      if (m_camCadDoc.getBlockAt(type->getBlockNumber()) == NULL)
               {
                  BlockStruct& undefinedBlock = getUndefinedBlock();

                  getVerificationLogFile().writef(PrefixError,
                     "Changed reference to bad block index of %d to %d on device type %d[%s] in file %d\n",
                     type->getBlockNumber(),undefinedBlock.getBlockNumber(),type->getEntityNumber(),type->getName(),file->getFileNumber());

                  type->setBlockNumber( undefinedBlock.getBlockNumber());
               }
            }
         }
      }
   }
}

void CCamCadDocRepairTool::updateTraceAndViaNetNames(BlockStruct *block, CString netName)
{
   // If block contains etch poly with blank net name then update 
   // to name given. Applies only to polys  with NETNAME attrib that is
   // empty or blank. In particular does not apply when there is no
   // NETNAME attrib at all.
   // Drill down into inserted blocks, apply the same rule.

   if (block != NULL)
   {
      int netNameKW = m_camCadDoc.IsKeyWord(ATT_NETNAME, 0);

      POSITION dataPos = block->getHeadDataPosition();
      while  (dataPos != NULL)
      {
         DataStruct *data = block->getNextData(dataPos);
         if (data != NULL)
         {
            switch (data->getDataType())
            {
            case dataTypePoly:
               updateBlankNetNameAttrib(data, netName);
               break;

            case dataTypeInsert:
               if (data->getInsert() != NULL)
               {
                  InsertTypeTag insertType = data->getInsert()->getInsertType();

                  if (insertType == insertTypePcb)
                  {

                     BlockStruct *insertedBlock = m_camCadDoc.getBlockAt( data->getInsert()->getBlockNumber() );
                     updateTraceAndViaNetNames(insertedBlock, netName);
                  }
                  else if (insertType == insertTypeVia)
                  {
                     updateBlankNetNameAttrib(data, netName);
                  }
                  // else skip it
               }
               break;

            default:
               // No op.
               break;
            }
         }
      }
   }
}

void CCamCadDocRepairTool::updateBlankNetNameAttrib(DataStruct *data, CString netName)
{
   Attrib *attrib;
   if (data->lookUpAttrib(m_netNameKW, attrib))
   {
      CString val( attrib->getStringValue() );
      val.Trim();
      if (val.IsEmpty())
      {
         data->setAttrib(m_camCadDoc.getCamCadData(), m_netNameKW, valueTypeString, (void*)netName.GetBuffer(0), attributeUpdateOverwrite, NULL);
      }
   }
}

void CCamCadDocRepairTool::verifyNetList()
{
   // Clean up net names, look for nets with blank name.
   // Update to non-blank, and try to update correlating NETNAME attrib on
   // traces and vias.

   POSITION filePos = m_camCadDoc.getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = m_camCadDoc.getFileList().GetNext(filePos);
      if (file != NULL)
      {
         POSITION netPos = file->getNetList().GetHeadPosition();
         while (netPos != NULL)
         {
            NetStruct *net = file->getNetList().GetNext(netPos);
            if (net != NULL)
            {
               CString netName(net->getNetName());
               netName.Trim();
               if (netName.IsEmpty()) // not cool
               {
                  // Change to non-empty net name, patterned after Expedition's "(Net0)".
                  // since Expedition provided us the CCZ with blank net name. WI 21401.
                  netName = "(Blank)"; 
                  net->setNetName(netName);

                  // Update traces
                  updateTraceAndViaNetNames(file->getBlock(), netName);

                  // Log it.
                  CString compPinList;
                  POSITION cpPos = net->getHeadCompPinPosition();
                  while (cpPos != NULL)
                  {
                     CompPinStruct *cp = net->getNextCompPin(cpPos);
                     if (cp != NULL)
                     {
                        if (!compPinList.IsEmpty())
                           compPinList += ", ";
                        compPinList += cp->getPinRef();
                     }
                  }
                  getVerificationLogFile().writef(PrefixError,
                     "Changed blank net name to %s on net (entity# %d)(CompPin List: %s)\n",
                     netName, net->getEntityNumber(), compPinList);

               }
            }
         }
      }
   }

}