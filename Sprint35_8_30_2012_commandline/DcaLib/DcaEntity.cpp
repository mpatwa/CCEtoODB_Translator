// $Header: /CAMCAD/DcaLib/DcaEntity.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaEntity.h"
#include "DcaCamCadData.h"
#include "DcaLib.h"

//_____________________________________________________________________________
CString entityTypeToString(EEntityType tagValue)
{
   const char* value;

   switch (tagValue)
   {
   case entityTypeUndefined:  value = "Undefined";  break;
   case entityTypeGeometry:   value = "Geometry";   break;
   case entityTypeData:       value = "Data";       break;
   case entityTypeFile:       value = "File";       break;
   case entityTypeNet:        value = "Net";        break;
   case entityTypeCompPin:    value = "CompPin";    break;
   case entityTypeDRC:        value = "DRC";        break;
   default:                   value = "?";          break;
   }

   return value;
}

//_____________________________________________________________________________
int CEntity::m_accessErrorCount = 0;

DataStruct* CEntity::findDataEntity(CCamCadData& camCadData,long entityNumber,BlockStruct** foundInBlock)
{
   if (entityNumber > 0)
   {
      for (int blockIndex=0;blockIndex < camCadData.getMaxBlockIndex();blockIndex++)
      {
         BlockStruct* block = camCadData.getBlockAt(blockIndex);
         
         if (block != NULL)
         {            
            for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
            {
               DataStruct* data = block->getDataList().GetNext(dataPos);

               if (data->getEntityNumber() == entityNumber)
               {
                  if (foundInBlock != NULL)
                  {
                     *foundInBlock = block;
                  }

                  return data;
               }
            }
         }
      }
   }

   return NULL;
}

NetStruct* CEntity::findNetEntity(CCamCadData& camCadData,long entityNumber,FileStruct** foundInFile)
{   
   for (POSITION filePos = camCadData.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = camCadData.getFileList().GetNext(filePos);
      
      for (POSITION netPos = file->getNetList().GetHeadPosition();netPos != NULL;)
      {
         NetStruct* net = file->getNetList().GetNext(netPos);

         if (net->getEntityNumber() == entityNumber)
         {
            if (foundInFile != NULL)
            {
               *foundInFile = file;
            }

            return net;
         }
      }
   }

   return NULL;
}

CompPinStruct* CEntity::findCompPinEntity(CCamCadData& camCadData,long entityNumber,FileStruct** foundInFile,NetStruct** foundInNet)
{
   for (POSITION filePos = camCadData.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = camCadData.getFileList().GetNext(filePos);
      
      for (POSITION netPos = file->getNetList().GetHeadPosition();netPos != NULL;)
      {
         NetStruct* net = file->getNetList().GetNext(netPos);
         
         for (POSITION compPinPos = net->getHeadCompPinPosition();compPinPos != NULL;)
         {
            CompPinStruct* compPin = net->getNextCompPin(compPinPos);

            if (compPin->getEntityNumber() == entityNumber)
            {
               if (foundInFile != NULL)
               {
                  *foundInFile = file;
               }

               if (foundInNet != NULL)
               {
                  *foundInNet = net;
               }

               return compPin;
            }
         }
      }
   }

   return NULL;
}

DRCStruct* CEntity::findDrcEntity(CCamCadData& camCadData,long entityNumber,FileStruct** foundInFile)
{
   for (POSITION filePos = camCadData.getFileList().GetHeadPosition();filePos != NULL;)
   {
      FileStruct* file = camCadData.getFileList().GetNext(filePos);
      
      for (POSITION drcPos = file->getDRCList().GetHeadPosition();drcPos != NULL;)
      {
         DRCStruct* drc = file->getDRCList().GetNext(drcPos);

         if (drc->getEntityNumber() == entityNumber)
         {
            if (foundInFile != NULL)
            {
               *foundInFile = file;
            }

            return drc;
         }
      }
   }

   return NULL;
}

CEntity CEntity::findEntity(CCamCadData& camCadData,long entityNumber,EEntityType entityType)
{
   CEntity entity;

   if (entityType == entityTypeData)
   {
      DataStruct* data = findDataEntity(camCadData,entityNumber);

      entity.setData(data);
   }
   else if (entityType == entityTypeCompPin)
   {
      CompPinStruct* compPin = findCompPinEntity(camCadData,entityNumber);

      entity.setCompPin(compPin);
   }
   else if (entityType == entityTypeNet)
   {
      NetStruct* net = findNetEntity(camCadData,entityNumber);

      entity.setNet(net);
   }
   else if (entityType == entityTypeDRC)
   {
      DRCStruct* drc = findDrcEntity(camCadData,entityNumber);

      entity.setDrc(drc);
   }

   return entity;
}

CEntity CEntity::findEntity(CCamCadData& camCadData,long entityNumber)
{
   CEntity entity;

   DataStruct* data = findDataEntity(camCadData,entityNumber);

   if (data != NULL)
   {
      entity.setData(data);
   }
   else
   {
      NetStruct* net = findNetEntity(camCadData,entityNumber);

      if (net != NULL)
      {
         entity.setNet(net);
      }
      else 
      {
         CompPinStruct* compPin = findCompPinEntity(camCadData,entityNumber);

         if (compPin != NULL)
         {
            entity.setCompPin(compPin);
         }
         else
         {
            DRCStruct* drc = findDrcEntity(camCadData,entityNumber);

            if (drc != NULL)
            {
               entity.setDrc(drc);
            }
         }
      }
   }

   return entity;
}

void CEntity::triggerAccessError()
{
   if (m_accessErrorCount == 0)
   {
      formatMessageBox(MB_ICONSTOP,
"CEntity::triggerAccessError(): An access error has occured!");
   }

   m_accessErrorCount++;
}

//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
CEntity::CEntity()
: m_entityType(entityTypeUndefined)
, m_entity(NULL)
{
}

CEntity::CEntity(const CEntity& other)
: m_entityType(other.m_entityType)
, m_entity(other.m_entity)
{
}

CEntity::CEntity(DataStruct* data)
{
   setData(data);
}

CEntity::CEntity(CompPinStruct* compPin)
{
   setCompPin(compPin);
}

CEntity::CEntity(NetStruct* net)
{
   setNet(net);
}

CEntity::CEntity(DRCStruct* drc)
{
   setDrc(drc);
}

EEntityType CEntity::getEntityType() const
{
   return m_entityType;
}

void CEntity::setData(DataStruct* data)
{
   m_entity = data;

   if (m_entity != NULL)
   {
      m_entityType = entityTypeData;
   }
   else
   {
      m_entityType = entityTypeUndefined;
   }
}

DataStruct* CEntity::getData() const
{
   DataStruct* data = NULL;

   if (m_entityType == entityTypeData)
   {
      data = (DataStruct*)m_entity;
   }
   else
   {
      triggerAccessError();
   }

   return data;
}

void CEntity::setCompPin(CompPinStruct* compPin)
{
   m_entity = compPin;

   if (m_entity != NULL)
   {
      m_entityType = entityTypeCompPin;
   }
   else
   {
      m_entityType = entityTypeUndefined;
   }
}

CompPinStruct* CEntity::getCompPin() const
{
   CompPinStruct* compPin = NULL;

   if (m_entityType == entityTypeCompPin)
   {
      compPin = (CompPinStruct*)m_entity;
   }
   else
   {
      triggerAccessError();
   }

   return compPin;
}

void CEntity::setDrc(DRCStruct* drc)
{
   m_entity = drc;

   if (m_entity != NULL)
   {
      m_entityType = entityTypeDRC;
   }
   else
   {
      m_entityType = entityTypeUndefined;
   }
}

DRCStruct* CEntity::getDrc() const
{
   DRCStruct* drc = NULL;

   if (m_entityType == entityTypeDRC)
   {
      drc = (DRCStruct*)m_entity;
   }
   else
   {
      triggerAccessError();
   }

   return drc;
}

void CEntity::setNet(NetStruct* net)
{
   m_entity = net;

   if (m_entity != NULL)
   {
      m_entityType = entityTypeNet;
   }
   else
   {
      m_entityType = entityTypeUndefined;
   }
}

NetStruct* CEntity::getNet() const
{
   NetStruct* net = NULL;

   if (m_entityType == entityTypeNet)
   {
      net = (NetStruct*)m_entity;
   }
   else
   {
      triggerAccessError();
   }

   return net;
}

void* CEntity::getEntity() const
{
   return m_entity;
}

CAttributes** CEntity::getAttributesRef() const
{
   CAttributes** pAttributes = NULL;

   switch (m_entityType)
   {
   case entityTypeData:
      pAttributes = &(getData()->getAttributesRef());
      break;
   case entityTypeCompPin:
      pAttributes = &(getCompPin()->getAttributesRef());
      break;
   case entityTypeNet:
      pAttributes = &(getNet()->getAttributesRef());
      break;
   case entityTypeDRC:
      pAttributes = &(getDrc()->getAttributesRef());
      break;
   }

   return pAttributes; 
}


