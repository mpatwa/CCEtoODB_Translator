// $Header: /CAMCAD/DcaLib/DcaEntity.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaEntity_h__)
#define __DcaEntity_h__

#pragma once

#define EN_UNDEFINED 0
#define EN_GEOMETRY  1
#define EN_DATA      2
#define EN_FILE      3
#define EN_NET       4
#define EN_COMPPIN   5
#define EN_DRC       6
#define EN_MAX       7

enum EEntityType
{
	entityTypeUndefined = EN_UNDEFINED,
	entityTypeGeometry  = EN_GEOMETRY,
	entityTypeData      = EN_DATA,
	entityTypeFile      = EN_FILE,
	entityTypeNet       = EN_NET,
	entityTypeCompPin   = EN_COMPPIN,
	entityTypeDRC       = EN_DRC,
	entityTypeMIN       = EN_UNDEFINED,
	entityTypeMAX       = EN_MAX,
};

class DataStruct;
class CompPinStruct;
class NetStruct;
class DRCStruct;
class CAttributes;
class BlockStruct;
class FileStruct;
class CCamCadData;

//_____________________________________________________________________________
class CEntity
{
private:
   EEntityType m_entityType;
   void* m_entity;

private:
   static int m_accessErrorCount;

public:
   CEntity();
   CEntity(DataStruct* data);
   CEntity(CompPinStruct* compPin);
   CEntity(NetStruct* net);
   CEntity(DRCStruct* drc);
   CEntity(const CEntity& other);

   EEntityType getEntityType() const;

   void setData(DataStruct* data);
   DataStruct* getData() const;

   void setCompPin(CompPinStruct* compPin);
   CompPinStruct* getCompPin() const;

   void setNet(NetStruct* net);
   NetStruct* getNet() const;

   void setDrc(DRCStruct* drc);
   DRCStruct* getDrc() const;

   void* getEntity() const;
   CAttributes** getAttributesRef() const;

public:
   static DataStruct*    findDataEntity(   CCamCadData& camCadData,long entityNumber,BlockStruct** foundInBlock=NULL);
   static NetStruct*     findNetEntity(    CCamCadData& camCadData,long entityNumber,FileStruct** foundInFile=NULL);
   static CompPinStruct* findCompPinEntity(CCamCadData& camCadData,long entityNumber,FileStruct** foundInFile=NULL,NetStruct** foundInNet=NULL);
   static DRCStruct*     findDrcEntity(    CCamCadData& camCadData,long entityNumber,FileStruct** foundInFile=NULL);

   static CEntity findEntity(CCamCadData& camCadData,long entityNumber,EEntityType entityType);
   static CEntity findEntity(CCamCadData& camCadData,long entityNumber);

private:
   static void triggerAccessError();

};

#endif
