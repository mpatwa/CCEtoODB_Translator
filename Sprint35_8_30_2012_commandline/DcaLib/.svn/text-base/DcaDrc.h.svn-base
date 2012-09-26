
#if !defined(__DcaDrc_h__)
#define __DcaDrc_h__

#pragma once

#include "Dca.h"
#include "DcaPoint2d.h"
#include "DcaContainer.h"

// DRC subClass 
#define  DRC_CLASS_SIMPLE                    0     // simple DRC, voidPtr is NULL
#define  DRC_CLASS_MEASURE                   1     // measure 2 items, voidPtr is DRC_MeasureStruct
#define  DRC_CLASS_NETS                      2     // Nets, voidPtr is CStringList of NetNames
#define  DRC_CLASS_NO_MARKER                 3     // simple DRC, voidPtr is NULL, No Inserted Marker

class CAttributes;
class CCamCadData;
class FileStruct;
class CDRCList;

//_____________________________________________________________________________
class DRC_MeasureStruct
{  // warning: this structure is copied using memcpy in API::CopyFile() - knv 20070628
public:
   int type1, type2;       // type of entity descibed by enitityNum
   long entity1, entity2;  // database entityNum
   DbUnit x1, y1, x2, y2;
};

//_____________________________________________________________________________
class DRCStruct
{
friend CTypedListContainer<CPtrList,DRCStruct*>;
friend CDRCList;

private:
   long           m_entityNumber; // entity;           // database entity number
   CString        m_string      ; //string;           // drc text visible per DRC marker instance.

   CPoint2d       m_origin;
   //DbUnit         x, y;             // xy location 

   int            m_drcClass; // drcClass;         // type of the void ptr. This is independend from the algIndex or algtype
                                    // net_name_type, measure_type

   void*          m_voidPtr; // voidPtr;          // depends on the subclass

   char           m_priority; // priority;         // 0=Low, 1=Medium, 2=High
   char           m_reviewed; // reviewed;         // 0=FALSE, 1=TRUE
   char           m_failureRange; // failureRange;     // -1 = no range, 0=critical, 1=marginal, 2=acceptable

   CString        m_comment; // comment;

   int            m_algorithmIndex; // algIndex;         // internal index from algNameArray - test name (example test_noload_components ...)
                                    // this is the heading of the DRC list box !
                                       
   int            m_algorithmType; // algType;          // as defined ALGTYPE_xxx. This is used as a fixed index, so that
                                    // internal CAMCAD software can understand the type of test which was performed.

   int            m_insertEntityNumber; // insertEntity;     // link to the DRC marker INSERT in database
                                    // link 0 is a DRC without a marker (Net without access)
   CAttributes*   m_attributes;

   CCamCadData& m_camCadData;

public:
   DRCStruct(CCamCadData& camCadData,const CString& string,int entityNumber);

private:
   ~DRCStruct();

public:
   // accessors
   long getEntityNumber() const;
   //void setEntityNumber(long entityNumber);

   CString getString() const;
   void setString(const CString& drcString);

   const CPoint2d& getOrigin() const;
   void setOrigin(const CPoint2d& origin);
   void setOriginX(double originX);
   void setOriginY(double originY);

   int getDrcClass() const;
   void setDrcClass(int drcClass);

   void* getVoidPtr() const;
   void setVoidPtr(void* voidPtr);

   int getPriority() const;
   void setPriority(int priority);

   int getReviewed() const;
   void setReviewed(int reviewed);

   int getFailureRange() const;
   void setFailureRange(int failureRange);

   CString getComment() const;
   void setComment(const CString& comment);

   int getAlgorithmIndex() const;
   void setAlgorithmIndex(int index);

   int getAlgorithmType() const;
   void setAlgorithmType(int type);

   int getInsertEntityNumber() const;
   void setInsertEntityNumber(int entityNumber);

   CAttributes*& getAttributesRef();
   CAttributes*  getAttributes() const;
   CAttributes&  attributes();

   // operations
   void copyData(const DRCStruct& other);

   void RebuildDrcString(int decimals);

	//void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CDRCList
{
private:
   CTypedPtrListContainer<DRCStruct*> m_drcList;

   FileStruct& m_fileStruct;

public:
   CDRCList(FileStruct& fileStruct);

   void empty();

   CCamCadData& getCamCadData() const;

   DRCStruct* addDrc(const CString& string,int entityNumber = -1);
   void deleteAt(POSITION pos);

   POSITION GetHeadPosition() const;
   DRCStruct* GetNext(POSITION& pos) const;
   DRCStruct* GetAt(POSITION pos) const;
   POSITION Find(DRCStruct* drc) const;
   int GetCount() const;
   bool IsEmpty() const;

   void takeData(CDRCList& otherList);

   void Scale(double factor, int decimals);

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

#endif
