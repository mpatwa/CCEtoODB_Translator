
#include "StdAfx.h"
#include "DcaFile.h"
#include "DcaFileType.h"
#include "DcaDft.h"
#include "DcaMachine.h"
#include "DcaVariant.h"
#include "DcaBlock.h"
#include "DcaTMatrix.h"
#include "DcaDataType.h"
#include "DcaInsert.h"
#include "DcaCamCadData.h"

//_____________________________________________________________________________
//

int FileStruct::m_nextId = 0;

FileStruct::FileStruct(CCamCadData& camCadData,int fileNumber,const CString& name,FileTypeTag sourceFileType,BlockStruct* fileBlock)
: m_camCadData(camCadData)
, m_fileNumber(fileNumber)
, m_name(name)
, m_sourceFileType(sourceFileType)
, m_block(fileBlock)
, m_deviceTypeDirectory(camCadData)
, m_netList(this->getCamCadData())
, m_busList(camCadData)
, m_drcList(*this)
, m_insertX(0.0)
, m_insertY(0.0)
, m_rotationRadians(0.0)
, m_scale(1.0)
, m_mirrorFlag(false)
, m_showFlag(true)
, m_hiddenFlag(false)
, m_notPlacedYetFlag(true)
, m_originalBlock(fileBlock)
, m_variantList(NULL)
, m_viaSpansSection(NULL)
, m_tempGeomEditFile(false)
{
   m_id = m_nextId++;
   if (NULL == m_block)
   {
      m_block = &(m_camCadData.getNewBlock(m_name,m_fileNumber,blockTypeUnknown));
      m_block->setFlags(BL_FILE);
      m_originalBlock = m_block;
   }
}

FileStruct::~FileStruct()
{
   //delete m_dftSolutionList;
   //delete m_machineList;

   getCamCadData().getBlockDirectory().removeFile(*this);

   if (m_viaSpansSection != NULL)
   {
      delete m_viaSpansSection;
      m_viaSpansSection = NULL;
   }

   delete m_variantList;
}

CCamCadData& FileStruct::getCamCadData() const
{
   return m_camCadData;
}

//=======================
int FileStruct::getFileNumber() const
{
   return m_fileNumber;
}

//void FileStruct::setFileNumber(int fileNumber)
//{
//   m_fileNumber = fileNumber;
//}

const CString& FileStruct::getName() const
{
   return m_name;
}

void FileStruct::setName(const CString& name)
{
   m_name = name;
}

const CString& FileStruct::getRefName() const
{
   return m_refName;
}

void FileStruct::setRefName(const CString& refName)
{
   m_refName = refName;
}

const CString& FileStruct::getCurrentDftSolutionName() const
{
   return m_curDftSolutionName;
}

void FileStruct::setCurrentDftSolutionName(const CString& name)
{
   m_curDftSolutionName = name;
}

const double& FileStruct::getInsertX() const
{
   return m_insertX;
}

void FileStruct::setInsertX(double x)
{
   m_insertX = x;
}

const double& FileStruct::getInsertY() const
{
   return m_insertY;
}

void FileStruct::setInsertY(double y)
{
   m_insertY = y;
}

const double& FileStruct::getScale() const
{
   return m_scale;
}

void FileStruct::setScale(double scale)
{
   m_scale = scale;
}

const double& FileStruct::getRotation() const
{
   return m_rotationRadians;
}

const double& FileStruct::getRotationRadians() const
{
   return m_rotationRadians;
}

void FileStruct::setRotation(double rotation)
{
   m_rotationRadians = rotation;
}

void FileStruct::incrementRotation(double increment)
{
   m_rotationRadians += increment;
}

bool FileStruct::isMirrored() const
{
   return m_mirrorFlag;
}

void FileStruct::setMirrored(bool mirrored)
{
   m_mirrorFlag = mirrored;
}

void FileStruct::setMirrored(BOOL mirrored)
{
   m_mirrorFlag = mirrored != 0;
}

bool FileStruct::getResultantMirror(bool mirrored) const
{
   return m_mirrorFlag != mirrored;
}

bool FileStruct::getResultantMirror(BOOL mirrored) const
{
   return m_mirrorFlag != (mirrored != 0);
}

bool FileStruct::isShown() const
{
   return m_showFlag;
}

void FileStruct::setShow(bool showFlag)
{
   m_showFlag = showFlag;
}

void FileStruct::setShow(BOOL showFlag)
{
   m_showFlag = (showFlag != 0);
}

bool FileStruct::isHidden() const
{
   return m_hiddenFlag;
}

void FileStruct::setHidden(bool hideFlag)
{
   m_hiddenFlag = hideFlag;
}

void FileStruct::setHidden(BOOL hideFlag)
{
   m_hiddenFlag = (hideFlag != 0);
}

bool FileStruct::notPlacedYet() const
{
   return m_notPlacedYetFlag;
}

void FileStruct::setNotPlacedYet(bool notPlacedFlag)
{
   m_notPlacedYetFlag = notPlacedFlag;
}

void FileStruct::setNotPlacedYet(BOOL notPlacedFlag)
{
   m_notPlacedYetFlag = (notPlacedFlag != 0);
}

BlockStruct* FileStruct::getBlock() const
{
   return m_block;
}

void FileStruct::setBlock(BlockStruct* block)
{
   m_block = block;
}

BlockStruct* FileStruct::getOriginalBlock() const
{
   return m_originalBlock;
}

void FileStruct::setOriginalBlock(BlockStruct* block)
{
   m_originalBlock = block;
}

BlockTypeTag FileStruct::getBlockType() const
{
   BlockTypeTag blockType = ((m_block != NULL) ? m_block->getBlockType() : blockTypeUnknown);

   return blockType;
}

void FileStruct::setBlockType(BlockTypeTag blockType)
{
   // I think it is safe to presume if one is setting the block type of the file then one would
   // want the actual block referenced to have the same type. I think it is a mistake to have
   // two different block type settings, one in the file and one in the block, but I am not
   // going to pursue that right now. Instead, when one set's the file block type, then also
   // set the block's block type. Failure to set block type at both levels has been the cause
   // of multiple Importer errors (e.g. case 2203) which do set the newly created file's
   // block type, but never set the block type in the block itself. - rcf

   if (getBlock() != NULL)
   {
      getBlock()->setBlockType(blockType);
   }
}

FileTypeTag FileStruct::getCadSourceFormat() const
{
   return m_sourceFileType;
}

void FileStruct::setCadSourceFormat(FileTypeTag sourceFormat)
{
   m_sourceFileType = sourceFormat;
}

CString FileStruct::getComment() const
{
   return m_comment;
}

void FileStruct::setComment(const CString& comment)
{
   m_comment = comment;
}

CNetList& FileStruct::getNetList()
{
   return m_netList;
}

const CNetList& FileStruct::getNetList() const
{
   return m_netList;
}

CBusList& FileStruct::getBusList()
{
   return m_busList;
}

const CBusList& FileStruct::getBusList() const
{
   return m_busList;
}

CGTabTableList& FileStruct::getTableList()
{
   return m_tableList;
}

const CGTabTableList& FileStruct::getTableList() const
{
   return m_tableList;
}

CDeviceTypeDirectory& FileStruct::getTypeList()
{
   return m_deviceTypeDirectory;
}

const CDeviceTypeDirectory& FileStruct::getTypeList() const
{
   return m_deviceTypeDirectory;
}

CDeviceTypeDirectory& FileStruct::getDeviceTypeDirectory()
{
   return m_deviceTypeDirectory;
}

const CDeviceTypeDirectory& FileStruct::getDeviceTypeDirectory() const
{
   return m_deviceTypeDirectory;
}

CDRCList& FileStruct::getDRCList()
{
   return m_drcList;
}

const CDRCList& FileStruct::getDRCList() const
{
   return m_drcList;
}

//CVariantList&       getVariantList();

//const CVariantList& FileStruct::getVariantList() const;

POSITION FileStruct::getHeadNetPosition() const
{
   return m_netList.GetHeadPosition();
}

NetStruct* FileStruct::getNextNet(POSITION& pos) const
{
   return m_netList.GetNext(pos);
}

int FileStruct::getNetCount() const
{
   return m_netList.GetCount();
}

//CDcaDftSolutionList* FileStruct::getDftSolutionList()
//{
//   return m_dftSolutionList;
//}
//
//const CDcaDftSolutionList* FileStruct::getDftSolutionList() const
//{
//   return m_dftSolutionList;
//}
//
//CDcaDftSolution* FileStruct::getCurrentDftSolution()
//{
//   return m_currentDftSolution;
//}
//
//const CDcaDftSolution* FileStruct::getCurrentDftSolution() const
//{
//   return m_currentDftSolution;
//}
//
//CDcaMachineList* FileStruct::getMachineList()
//{
//   return m_machineList;
//}
//
//const CDcaMachineList* FileStruct::getMachineList() const
//{
//   return m_machineList;
//}
//
//CDcaMachine* FileStruct::getCurrentMachine()
//{
//   return m_currentMachine;
//}
//
//const CDcaMachine* FileStruct::getCurrentMachine() const
//{
//   return m_currentMachine;
//}

//=======================


CVariantList& FileStruct::getVariantList()
{
   if (m_variantList == NULL)
   {
      m_variantList = new CVariantList(getCamCadData());
   }

   return *m_variantList; 
}

const CVariantList& FileStruct::getVariantList() const
{
   if (m_variantList == NULL)
   {
      m_variantList = new CVariantList(getCamCadData());
   }

   return *m_variantList; 
}

CTMatrix FileStruct::getTMatrix()
{
   CTMatrix matrix;

   matrix.translateCtm(m_insertX,m_insertY);
   matrix.rotateRadiansCtm(m_rotationRadians);
   matrix.scaleCtm(m_mirrorFlag ? -m_scale : m_scale,m_scale);

   return matrix;
}


DataStruct* FileStruct::FindInsertData(CString refname)
{
   // Patterned after the many findComponentData() functions, most of which
   // don't really limit themselves to components but rather will get any insert
   // with matching name, like this one.

   for (POSITION pos = this->getBlock()->getDataList().GetHeadPosition();pos != NULL;)
   {
      DataStruct *data = this->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() == dataTypeInsert && data->getInsert() != NULL)
      {
         if (data->getInsert()->getRefname().CompareNoCase(refname) == 0)
         {
            return data;
         }
      }
   }

   return NULL;
}

DataStruct* FileStruct::FindInsertData(CString refname, InsertTypeTag insertType)
{
   return this->getBlock()->FindInsertData(refname, insertType);
}

CViaSpansSection* FileStruct::GetViaSpansSection()
{
   if (m_viaSpansSection == NULL)
      m_viaSpansSection = new CViaSpansSection();

   return m_viaSpansSection;
}

const CViaSpansSection* FileStruct::GetViaSpansSection() const
{
   if (m_viaSpansSection == NULL)
      m_viaSpansSection = new CViaSpansSection();

   return m_viaSpansSection;
}

void FileStruct::SetViaSpansSection(CViaSpansSection &other)
{
   m_viaSpansSection = &other;
}

//_____________________________________________________________________________
CFileList::CFileList(CCamCadData& camCadData)
: m_camCadData(camCadData)
, m_nextFileNumber(1)
, m_pLastAddedFile(NULL)
{
}

CFileList::~CFileList()
{
}

CCamCadData& CFileList::getCamCadData()
{
   return m_camCadData;
}

void CFileList::empty()
{
   m_fileList.RemoveAll();
   m_fileStack.RemoveAll();
   m_fileArray.empty();
}

const CCamCadData& CFileList::getCamCadData() const
{
   return m_camCadData;
}

FileStruct* CFileList::addNewFile(const CString& name,FileTypeTag sourceFileType)
{
   int fileNumber = m_nextFileNumber++;

   FileStruct* newFile = new FileStruct(m_camCadData,fileNumber,name,sourceFileType,NULL);

   m_fileArray.SetAtGrow(fileNumber,newFile);
   addToFileListTail(newFile);

   m_pLastAddedFile = newFile;

   return newFile;
}

FileStruct* CFileList::getFileAt(int fileNumber)
{
   FileStruct* fileStruct = NULL;

   if (fileNumber >= 0 && fileNumber < m_fileArray.GetSize())
   {
      fileStruct = m_fileArray.GetAt(fileNumber);
   }

   return fileStruct;
}

FileStruct& CFileList::getDefinedFileAt(int fileNumber,const CString& name,FileTypeTag sourceFileType,BlockStruct* fileBlock)
{
   FileStruct* fileStruct = NULL;

   if (fileNumber < 0)
   {
      fileNumber = 0;
   }

   if (fileNumber >= 0 && fileNumber < m_fileArray.GetSize())
   {
      fileStruct = m_fileArray.GetAt(fileNumber);
   }

   if (fileStruct == NULL)
   {
      fileStruct = new FileStruct(m_camCadData,fileNumber,name,sourceFileType,fileBlock);

      fileStruct->setOriginalBlock(fileStruct->getBlock());

      m_fileArray.SetAtGrow(fileNumber,fileStruct);
      addToFileListTail(fileStruct);

      m_pLastAddedFile = fileStruct;

      if (fileNumber >= m_nextFileNumber)
         m_nextFileNumber = fileNumber + 1; // This accounts for possible skips in file numbers
   }

   return *fileStruct;
}

FileStruct& CFileList::pushNewFile(const CString& name,FileTypeTag sourceFileType,BlockStruct* fileBlock)
{
   FileStruct* newFile = addNewFile(name, sourceFileType);
   newFile->setBlock(fileBlock);
   newFile->setOriginalBlock(fileBlock);

   m_fileStack.AddTail(newFile);

   return *newFile;
}

void CFileList::popFile()
{
   if (m_fileStack.GetCount() > 0)
   {
      FileStruct* file = m_fileStack.GetTail();
      m_fileStack.RemoveTail();
      
      deleteFile(file);
   }
}

void CFileList::deleteFile(FileStruct* file)
{
   if (file != NULL)
   {
      POSITION pos = m_fileList.Find(file);

      if (pos != NULL)
      {
         m_fileList.RemoveAt(pos);
      }

      int fileNumber = file->getFileNumber();

      if (fileNumber >= 0 && fileNumber < m_fileArray.GetSize())
      {
         FileStruct* referencedFile = m_fileArray.GetAt(fileNumber);

         if (referencedFile == file)
         {
            m_fileArray.SetAt(fileNumber,NULL);
         }
      }

      if (file == m_pLastAddedFile)
      {
         m_pLastAddedFile = NULL;
      }

      delete file;
      file = NULL;
   }
}

FileStruct* CFileList::GetFirst(BlockTypeTag blockType)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      FileStruct *fileStruct = GetNext(pos);

      if (fileStruct->getBlockType() == blockType)
      {
         return fileStruct;
      }
   }

   return NULL;
}

FileStruct* CFileList::GetFirstShown(BlockTypeTag blockType)
{
   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      FileStruct *fileStruct = GetNext(pos);

      if (fileStruct->isShown() && 
         (fileStruct->getBlockType() == blockType || blockType == blockTypeUndefined) )
      {
         return fileStruct;
      }
   }

   return NULL;
}

FileStruct* CFileList::GetExclusivelyShown(BlockTypeTag blockType)
{
   // Returns file of given type if it is the one and only file visible.
   // E.g if on PCB file an done Panel file are visible, this returns NULL
   // because two files are visible, whereas GetOnlyShown(pcb) would return
   // the pcb file and ignore the panel.

   FileStruct *file = this->GetOnlyShown(blockTypeUndefined);

   if (file != NULL && (file->getBlockType() == blockType || blockType == blockTypeUnknown))
      return file;  // Only one file visible and it is correct type

   return NULL; // None found
}

FileStruct* CFileList::GetOnlyShown(BlockTypeTag blockType)
{
   // Returns file if it is the only file of the given type shown.
   // Files of other types may be shown, these are ignored.
   // E.g. getting the only shown PCB file does not guarantee that
   // a panel is not also currently shown.
   // If type is blockTypeUndefined, then this returns the one
   // and only visible file, or NULL if there is more than one.

   FileStruct* fileStruct = NULL;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      FileStruct *curFile = GetNext(pos);

      if (curFile->isShown() && 
         (curFile->getBlockType() == blockType || blockType == blockTypeUndefined) )
      {
         if (fileStruct != NULL)
         {
            return NULL;
         }
         else
         {
            fileStruct = curFile;
         }
      }
   }

   return fileStruct;
}

FileStruct* CFileList::GetOnlyShown(BlockTypeTag blockType, BlockTypeTag excludeBlock)
{
   FileStruct* fileStruct = NULL;
   int visiblecnt = 0;

   for (POSITION pos = GetHeadPosition();pos != NULL && visiblecnt < 2;)
   {
      FileStruct *curFile = GetNext(pos);

      if (curFile->isShown() && 
         (curFile->getBlockType() == blockType || blockType == blockTypeUndefined || 
         curFile->getBlockType() == excludeBlock))
      {
         if(curFile->getBlockType() == blockType)
         {            
            fileStruct = curFile;
         }
         visiblecnt++;
      }
   }

   return (visiblecnt == 1)? fileStruct: NULL;
}

FileStruct* CFileList::GetLastShown(BlockTypeTag blockType)
{
   FileStruct* fileStruct = NULL;

   for (POSITION pos = GetTailPosition();pos != NULL;)
   {
      fileStruct = GetPrev(pos);

      if (fileStruct->isShown() && 
         (fileStruct->getBlockType() == blockType || blockType == blockTypeUndefined) )
      {
         break;
      }

      fileStruct = NULL;
   }

   return fileStruct;
}

int CFileList::GetVisibleCount(int &pcbCount, int &panelCount, int &otherCount)
{
   // Determine counts for VISIBLE files of each type.
   // Main interest is pcb and panel files, all the rest are grouped into other.
   // Return value is total visible file count.

   pcbCount = 0;
   panelCount = 0;
   otherCount = 0;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      FileStruct *curFile = GetNext(pos);

      if (curFile->isShown())
      {
         switch (curFile->getBlockType())
         {
         case blockTypePcb:      pcbCount++;       break;
         case blockTypePanel:    panelCount++;     break;
         default:                otherCount++;     break;
         }
      }
   }

   return pcbCount + panelCount + otherCount;
}

int CFileList::GetVisibleCount(BlockTypeTag blockType)
{
   int visibleCount = 0;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      FileStruct *curFile = GetNext(pos);

      if (!curFile->isShown() || (blockType != blockTypeGeometryEdit && curFile->getBlockType() != blockType))
         continue;

      visibleCount++;
   }

   return visibleCount;
}

int CFileList::GetFileCount(BlockTypeTag blockType)
{
   int fileCount = 0;

   if (blockType == blockTypeUndefined)
      fileCount = this->GetCount();
   else
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         FileStruct *curFile = GetNext(pos);
         if (curFile == NULL || curFile->getBlockType() != blockType)
            continue;

         fileCount++;
      }
   }

   return fileCount;
}

FileStruct* CFileList::FindByBlockNumber(int blockNumber) const
{
   for (POSITION pos = m_fileList.GetHeadPosition();pos != NULL;)
   {
      FileStruct* curFile = m_fileList.GetNext(pos);

      if (curFile->getBlock()->getBlockNumber() == blockNumber)
         return curFile;
   }

   return NULL;
}

FileStruct* CFileList::FindByFileNumber(int fileNumber) const
{
   FileStruct* file = NULL;

   for (POSITION pos = m_fileList.GetHeadPosition();pos != NULL;)
   {
      file = m_fileList.GetNext(pos);

      if (file->getFileNumber() == fileNumber)
      {
         break;
      }

      file = NULL;
   }

   return file;
}

FileStruct* CFileList::FindByName(CString fileName) const
{
   for (POSITION pos = m_fileList.GetHeadPosition();pos != NULL;)
   {
      FileStruct* curFile = m_fileList.GetNext(pos);

      if (curFile != NULL && curFile->getName().CompareNoCase(fileName) == 0)
         return curFile;
   }

   return NULL;
}

void CFileList::SetShowAll(bool showFlag)
{
   for (POSITION pos=GetHeadPosition(); pos!=NULL; GetNext(pos))
   {
      FileStruct *curFile = GetAt(pos);
      if (curFile != NULL)
         curFile->setShow(showFlag);
   }
}

POSITION CFileList::GetHeadPosition() const
{
   return m_fileList.GetHeadPosition();
}

POSITION CFileList::GetTailPosition() const
{
   return m_fileList.GetTailPosition();
}

POSITION CFileList::Find(FileStruct* file) const
{
   return m_fileList.Find(file);
}

FileStruct* CFileList::GetHead() const
{
   return m_fileList.GetHead();
}

FileStruct* CFileList::GetTail() const
{
   return m_fileList.GetTail();
}

FileStruct* CFileList::GetNext(POSITION& pos) const
{
   FileStruct* fileStruct = m_fileList.GetNext(pos);

   if (fileStruct == NULL)
   {
      int iii = 3;
   }

   return fileStruct;
}

FileStruct* CFileList::GetPrev(POSITION& pos) const
{
   return m_fileList.GetPrev(pos);
}

FileStruct* CFileList::GetAt(POSITION pos)    const
{
   return m_fileList.GetAt(pos);
}

int CFileList::GetCount()
{
   return m_fileList.GetCount();
}

FileStruct* CFileList::GetLastAdded() const
{
   return m_pLastAddedFile;
}

void CFileList::addToFileListTail(FileStruct* file)
{
   if (file != NULL)
   {
      m_fileList.AddTail(file);
   }
   else
   {
      int iii = 3;
   }
}
