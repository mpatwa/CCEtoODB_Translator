// $Header: /CAMCAD/4.5/ComponentSurfaceDfm.cpp 14    8/04/06 8:55p Kurt Van Ness $

#include "StdAfx.h"
#include "CamCadDatabase.h"
#include "WriteFormat.h"

//_____________________________________________________________________________
bool DFM_ComponentSurfaceCheck(CCEtoODBDoc& camCadDoc,BlockStruct& pcbBlock,
   int surfaceMask,const CString& attributeKeyword,const CString& attributeValue)
{
   bool retval = false;

   while (true)
   {
      if (pcbBlock.getBlockType() != BLOCKTYPE_PCB)
      {
         break;
      }

      CNullWriteFormat errorLog;
      CCamCadDatabase camCadDatabase(camCadDoc);
      FileStruct* pcbFile = camCadDoc.Find_File(pcbBlock.getFileNumber());

      int keywordIndex = camCadDoc.IsKeyWord(attributeKeyword,0);
      Attrib* value;
      CDataList& pcbDataList   = pcbBlock.getDataList();
      int algorithmIndexTop    = GetAlgorithmNameIndex(&camCadDoc,"Component Surface Check (Top)");
      int algorithmIndexBottom = GetAlgorithmNameIndex(&camCadDoc,"Component Surface Check (Bottom)");
      bool checkTopFlag        = ((surfaceMask & topSurface   ) != 0);
      bool checkBottomFlag     = ((surfaceMask & bottomSurface) != 0);

      // scan for components with specified attribute keyword and value
      for (POSITION pcbDataPos = pcbDataList.GetHeadPosition();pcbDataPos != NULL;)
      {
         DataStruct* pcbDataStruct = pcbDataList.GetNext(pcbDataPos);

         if (pcbDataStruct != NULL && pcbDataStruct->getDataType() == T_INSERT)
         {
            InsertStruct* pcbInsert = pcbDataStruct->getInsert();
            bool topFlag = !(pcbInsert->getPlacedBottom());

            if (pcbInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT &&
                  ((topFlag && checkTopFlag) || (!topFlag && checkBottomFlag))  )
            {
               if (pcbDataStruct->getAttributesRef()->Lookup(keywordIndex,value))
               {
                  Attrib* attribute = (Attrib*)value;
                  CString stringValue = camCadDatabase.getAttributeStringValue(attribute);

                  if (stringValue.CompareNoCase(attributeValue) == 0)
                  {
                     int algorithmIndex  = (topFlag ? algorithmIndexTop : algorithmIndexBottom);
                     CString surfaceName = (topFlag ? "Top" : "Bottom");
                     CString description;
                     description.Format("Component Not Allowed on %s surface; RefDes=%s; %s=%s",
                        (const char*)surfaceName,(const char*)pcbInsert->getRefname(),
                        (const char*)attributeKeyword,(const char*)attributeValue);

                     camCadDatabase.addDrc(*pcbFile,pcbInsert->getOriginX(),pcbInsert->getOriginY(),
                        description,drcSimple,drcCriticalFailure,algorithmIndex,drcGeneric);
                  }
               }
            }
         }
      }

      retval = true;

      break;
   }

   return retval;
}

//_____________________________________________________________________________
bool DFM_ComponentHeightCheck(CCEtoODBDoc& camCadDoc,BlockStruct& pcbBlock,
   int surfaceMask,const CString& componentHeightAttributeKeyword,
   double topCriticalHeight   ,double topMarginalHeight,
   double bottomCriticalHeight,double bottomMarginalHeight)
{
   bool retval = false;

   while (true)
   {
      if (pcbBlock.getBlockType() != BLOCKTYPE_PCB)
      {
         break;
      }

      CNullWriteFormat errorLog;
      CCamCadDatabase camCadDatabase(camCadDoc);
      FileStruct* pcbFile = camCadDoc.Find_File(pcbBlock.getFileNumber());

      int keywordIndex = camCadDoc.IsKeyWord(componentHeightAttributeKeyword,0);
      Attrib* value;
      CDataList& pcbDataList   = pcbBlock.getDataList();
      int algorithmIndexTop    = GetAlgorithmNameIndex(&camCadDoc,"Component Height Check (Top)");
      int algorithmIndexBottom = GetAlgorithmNameIndex(&camCadDoc,"Component Height Check (Bottom)");
      bool checkTopFlag        = ((surfaceMask & topSurface   ) != 0);
      bool checkBottomFlag     = ((surfaceMask & bottomSurface) != 0);

      // scan for components with specified attribute keyword
      for (POSITION pcbDataPos = pcbDataList.GetHeadPosition();pcbDataPos != NULL;)
      {
         DataStruct* pcbDataStruct = pcbDataList.GetNext(pcbDataPos);

         if (pcbDataStruct != NULL && pcbDataStruct->getDataType() == T_INSERT)
         {
            InsertStruct* pcbInsert = pcbDataStruct->getInsert();
            bool topFlag = !(pcbInsert->getPlacedBottom());

            if (pcbInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT &&
                  ((topFlag && checkTopFlag) || (!topFlag && checkBottomFlag))  )
            {
               if (pcbDataStruct->getAttributesRef()->Lookup(keywordIndex,value))
               {
                  Attrib* attribute = (Attrib*)value;
                  CString stringValue = camCadDatabase.getAttributeStringValue(attribute);
                  double height = atof(stringValue);
                  double criticalHeight = (topFlag ? topCriticalHeight : bottomCriticalHeight);
                  double marginalHeight = (topFlag ? topMarginalHeight : bottomMarginalHeight);
                  double heightLimitExceeded = 0.;

                  DrcFailureRangeTag failureRange = drcUndefinedFailure;

                  if (height >= criticalHeight) 
                  {
                     failureRange = drcCriticalFailure;
                     heightLimitExceeded = criticalHeight;
                  }
                  else if (height >= marginalHeight) 
                  {
                     failureRange = drcMarginalFailure;
                     heightLimitExceeded = marginalHeight;
                  }

                  if (failureRange != drcUndefinedFailure)
                  {
                     int algorithmIndex  = (topFlag ? algorithmIndexTop : algorithmIndexBottom);
                     CString description;
                     description.Format("Component Height %.3f >= %.3f; RefDes=%s",
                        height,heightLimitExceeded,(const char*)pcbInsert->getRefname());

                     camCadDatabase.addDrc(*pcbFile,pcbInsert->getOriginX(),pcbInsert->getOriginY(),
                        description,drcSimple,failureRange,algorithmIndex,drcGeneric);
                  }
               }
            }
         }
      }

      retval = true;

      break;
   }

   return retval;
}





