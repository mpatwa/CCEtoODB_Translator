
#if !defined(__DcaFile_h__)
#define __DcaFile_h__

#pragma once

#include "Dca.h"
#include "DcaNet.h"
#include "DcaBus.h"
#include "DcaType.h"
#include "DcaDrc.h"
#include "DcaBlockType.h"
#include "DcaInsertType.h"
#include "DcaViaSpan.h"
#include "DcaTable.h"

class BlockStruct;
class CVariantList;
class CDcaDftSolutionList;
class CDcaDftSolution;
class CDcaMachineList;
class CDcaMachine;
class DataStruct;
class FileStruct;

enum PageUnitsTag;
enum FileTypeTag;

//_____________________________________________________________________________
class CFileList
{
private:
   CCamCadData& m_camCadData;
   int m_nextFileNumber;   

   CTypedPtrArrayContainer<FileStruct*> m_fileArray;
   CTypedPtrList<CPtrList,FileStruct*> m_fileList;
   CTypedPtrList<CPtrList,FileStruct*> m_fileStack;

public:
   CFileList(CCamCadData& camCadData);
   ~CFileList();

private:
   FileStruct *m_pLastAddedFile;

public:
   CCamCadData& getCamCadData();
   const CCamCadData& getCamCadData() const;

   void empty();


   FileStruct* GetFirst(BlockTypeTag blockType);  // gets first one encountered, regardless of shown or count of given type present
   FileStruct* GetFirstShown(BlockTypeTag blockType);
   FileStruct* GetOnlyShown(BlockTypeTag blockType);  // gets only shown file of given type, but files of other types maybe also be visible
   FileStruct* GetLastShown(BlockTypeTag blockType);
   FileStruct* GetExclusivelyShown(BlockTypeTag blockType);  // gets file of given type if it is, exclusively, the one and only file visible
   FileStruct* GetOnlyShown(BlockTypeTag blockType, BlockTypeTag excludeBlock);
   int GetVisibleCount(BlockTypeTag blockType = blockTypePcb);
   int GetVisibleCount(int &pcbCount, int &panelCount, int &otherCount);
   int GetFileCount(BlockTypeTag blockType);

   FileStruct* FindByBlockNumber(int blockNumber) const;
   FileStruct* FindByFileNumber(int fileNumber)   const;
   FileStruct* FindByName(CString fileName)       const;
   void SetShowAll(bool showFlag);

   FileStruct* addNewFile(const CString& name,FileTypeTag sourceFileType);
   FileStruct& getDefinedFileAt(int fileNumber,const CString& name,FileTypeTag sourceFileType,BlockStruct* fileBlock);
   FileStruct& pushNewFile(const CString& name,FileTypeTag sourceFileType,BlockStruct* fileBlock);
   FileStruct* getFileAt(int fileNumber);
   void popFile();

   POSITION GetHeadPosition()      const;
   POSITION GetTailPosition()      const;
   POSITION Find(FileStruct* file) const;

   FileStruct* GetHead() const;
   FileStruct* GetTail() const;

   FileStruct* GetNext(POSITION& pos) const;
   FileStruct* GetPrev(POSITION& pos) const;
   FileStruct* GetAt(POSITION pos)    const;

   void deleteFile(FileStruct* file);
   int GetCount();
   FileStruct* GetLastAdded() const;

private:
   void addToFileListTail(FileStruct* fileStruct);
};

//_____________________________________________________________________________
class FileStruct
{
   //friend FileStruct* CFileList::addNewFile(const CString& name,FileTypeTag sourceFileType);
   //friend FileStruct& CFileList::getDefinedFileAt(int fileNumber,const CString& name,FileTypeTag sourceFileType,BlockStruct* fileBlock);
   friend CFileList;
   friend CTypedArrayContainer<CPtrList,FileStruct*>;
   friend CTypedArrayContainer<CPtrArray,FileStruct*>;

private:
   FileStruct(CCamCadData& camCadData,int fileNumber,const CString& name,FileTypeTag sourceFileType,BlockStruct* fileBlock);

private:
   ~FileStruct();

private:
   CCamCadData&      m_camCadData;

   int               m_fileNumber;
   //BlockTypeTag      m_blockType;
   FileTypeTag       m_sourceFileType;
   CString           m_name;
   CString           m_refName;
   CString           m_comment;
   double            m_insertX;
   double            m_insertY;
   double            m_scale;
   double            m_rotationRadians;
   bool              m_mirrorFlag;
   bool              m_showFlag;
   bool              m_hiddenFlag;   // used for Geometry Editing
   bool              m_notPlacedYetFlag;    // for importing

   bool              m_tempGeomEditFile; // For internal CAMCAD use, do not save to CCZ, files with with true should
                                         // not be saved to CCZ at all. This true means the file is used for the
                                         // geometry edit display (only!).

   CString           m_curDftSolutionName;  // This name is for storage of setting from ccz file. After some period of processing
                                            // the actual current solution might change, but this name will not be updated.
                                            // So do not access this name directly for any processing, use only the 
                                            // DOC::GetCurrentDFTSolution function, which will give you the actual current one
                                            // or activate one with this name if necessary.

   BlockStruct*      m_block;
   BlockStruct*      m_originalBlock;

   CNetList                  m_netList;
   CBusList                  m_busList;
   mutable CVariantList*     m_variantList;
   //CTypeList               m_typeList;
   CDeviceTypeDirectory      m_deviceTypeDirectory;
   CDRCList                  m_drcList;
   mutable CViaSpansSection* m_viaSpansSection;

   CGTabTableList            m_tableList;

private:
   static int        m_nextId;
   int               m_id;

public:
   CCamCadData& getCamCadData() const;

   int getFileNumber() const;
   //void setFileNumber(int fileNumber);   !!! Because DCA keeps directories, this is no longer allowed to change after creation !!!

   const CString& getName() const;
   void setName(const CString& name);

   const CString& getRefName() const;
   void setRefName(const CString& refName);

   const bool IsTempGeomEditFile()        { return m_tempGeomEditFile; }
   void setTempGeomEdit(bool flag)        { m_tempGeomEditFile = flag; }

   const CString& getCurrentDftSolutionName() const;
   void setCurrentDftSolutionName(const CString& name);

   const double& getInsertX() const;
   void setInsertX(double x);

   const double& getInsertY() const;
   void setInsertY(double y);

   const double& getScale() const;
   void setScale(double scale);

   const double& getRotation()        const;
   const double& getRotationRadians() const;
   void setRotation(double rotation);
   void incrementRotation(double increment);

   bool isMirrored() const;
   void setMirrored(bool mirrored);
   void setMirrored(BOOL mirrored);
   bool getResultantMirror(bool mirrored) const;
   bool getResultantMirror(BOOL mirrored) const;

   bool isShown() const;
   void setShow(bool showFlag);
   void setShow(BOOL showFlag);

   bool isHidden() const;
   void setHidden(bool hideFlag);
   void setHidden(BOOL hideFlag);

   bool notPlacedYet() const;
   void setNotPlacedYet(bool notPlacedFlag);
   void setNotPlacedYet(BOOL notPlacedFlag);

   BlockStruct* getBlock() const;
   void setBlock(BlockStruct* block);

   BlockStruct* getOriginalBlock() const;
   void setOriginalBlock(BlockStruct* block);

   BlockTypeTag getBlockType() const;
   void setBlockType(BlockTypeTag blockType);

   FileTypeTag getCadSourceFormat() const;
   void setCadSourceFormat(FileTypeTag sourceFormat);

   CString getComment() const;
   void setComment(const CString& comment);

   CNetList&        getNetList();
   const CNetList&  getNetList()  const;

   CBusList&       getBusList();
   const CBusList& getBusList()   const;

   CGTabTableList&       getTableList();
   const CGTabTableList& getTableList() const;

   CDeviceTypeDirectory&       getTypeList();
   const CDeviceTypeDirectory& getTypeList() const;

   CDeviceTypeDirectory&       getDeviceTypeDirectory();
   const CDeviceTypeDirectory& getDeviceTypeDirectory() const;

   CDRCList&        getDRCList();
   const CDRCList&  getDRCList()  const;

   CVariantList&       getVariantList();
   const CVariantList& getVariantList() const;

   POSITION getHeadNetPosition()        const;
   NetStruct* getNextNet(POSITION& pos) const;
   int getNetCount()                    const;

   CTMatrix getTMatrix();
   
   CViaSpansSection* GetViaSpansSection();
   const CViaSpansSection* GetViaSpansSection() const;
   void SetViaSpansSection(CViaSpansSection&);

   DataStruct* FindInsertData(CString refname);
   DataStruct* FindInsertData(CString refname, InsertTypeTag insertType);
};

#endif
