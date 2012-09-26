
#include "stdafx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"
#include "MergeFiles.h"
#include "polylib.h"
#include "drc.h"
#include "EntityNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//void  FillCurrentSettings(CCEtoODBDoc *doc);

/*******************************************************************************
* CopyFileContents
*/
void CopyFileContents(FileStruct *from, FileStruct *to, CCEtoODBDoc *doc) 
{
   CCamCadData& camCadData = doc->getCamCadData();

   if (from == to)
   {
      return;
   }

   // copy datas   
   for (POSITION pos = from->getBlock()->getDataList().GetHeadPosition();pos != NULL;)
   {
      DataStruct *data = from->getBlock()->getDataList().GetNext(pos);
      DataStruct *newData = camCadData.getNewDataStruct(*data);
      to->getBlock()->getDataList().AddTail(newData);
   }

   // copy NetList
   
   for (POSITION pos = from->getNetList().GetHeadPosition();pos != NULL;)
   {
      NetStruct* net = from->getNetList().GetNext(pos);

      NetStruct* newNet = to->getNetList().addNet(net->getNetName());

      newNet->setHighlighted(net->isHighlighted());
      newNet->setFlags(net->getFlags());
      doc->CopyAttribs(&newNet->getAttributesRef(), net->getAttributesRef());

      // copy comp pins      
      for (POSITION cpPos = net->getHeadCompPinPosition();cpPos != NULL;)
      {
         CompPinStruct* compPinStruct = net->getNextCompPin(cpPos);

         CompPinStruct* newCompPinStruct = newNet->addCompPin(compPinStruct->getRefDes(),compPinStruct->getPinName(),
            compPinStruct->getOriginX(),compPinStruct->getOriginY(),compPinStruct->getRotationDegrees(),
            compPinStruct->getMirror(),compPinStruct->getPadstackBlockNumber(),compPinStruct->getVisible());

         doc->CopyAttribs(&newCompPinStruct->getAttributesRef(),compPinStruct->getAttributesRef());
      }
   }

   // copy DRCList   
   for (POSITION pos = from->getDRCList().GetHeadPosition();pos != NULL;)
   {
      DRCStruct* drc = from->getDRCList().GetNext(pos);
      DRCStruct* newDRC = to->getDRCList().addDrc(drc->getString());

      newDRC->copyData(*drc);
      doc->CopyAttribs(&newDRC->getAttributesRef(), drc->getAttributesRef());
   }

   // TypeList    
   for (POSITION pos = from->getTypeList().GetHeadPosition();pos != NULL;)
   {
      TypeStruct *type = from->getTypeList().GetNext(pos);

      TypeStruct* newType = to->getTypeList().addType(type->getName());

      newType->setBlockNumber( type->getBlockNumber());
      doc->CopyAttribs(&newType->getAttributesRef(), type->getAttributesRef());

      // copy typepins      
      for (POSITION tpPos = type->getTypePinList().GetHeadPosition();tpPos != NULL;)
      {
         TypePinStruct *tp = type->getTypePinList().GetNext(tpPos);

         TypePinStruct* newTP = newType->getTypePinList().addTypePin();

         *newTP = *tp;
      }
   }
}

/*******************************************************************************
* MergeFiles
*/
void CCEtoODBDoc::OnMergeFiles() 
{
   //OnClearSelected();

   MergeFiles dlg;
   dlg.doc = this;
   dlg.DoModal();
   
   //FillCurrentSettings(this);
}

/*****************************************************************************/
/*
   return TRUE if the check is OK.
*/
static int Make_MergeTo_Blocks_Global(CCEtoODBDoc *doc, FileStruct *mergeTo)
{
   // check here that a local block does not name collide with a global block
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)         continue;
      if (block->getFlags() & BL_FILE) continue;   // if block is a file, that should not be checked.

      if (block->getFileNumber() == mergeTo->getFileNumber())
      {
         // this block has to be made global
         for (int ii=0; ii<doc->getMaxBlockIndex(); ii++)
         {
            BlockStruct *bb = doc->getBlockAt(ii);
            if (bb == NULL)continue;
            if (bb->getFlags() & BL_FILE) continue;

            if (ii == i)   continue;
            if (!strcmp(block->getName(), bb->getName()))
            {
// block collision found, merge not possible
               CString  tmp;
               tmp.Format("Block [%s] in File [%s] already exist global!", 
                  block->getName(), mergeTo->getName());
               ErrorMessage(tmp,"No Merge Possible");
               return 0;
            }
         }

         block->setFileNumber(-1);
      }
   }

   return 1;
}

/*****************************************************************************/
/*
   return TRUE if the check is OK.
*/
static int CheckGeometries(FileStruct *merge, FileStruct *mergeTo, CCEtoODBDoc *doc)
{
   // there are global blocks and file dependend blocks.
   // here we can only check file dependend blocks, because global bloacks are already merged !

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *mergeblock = doc->getBlockAt(i);
      if (mergeblock == NULL) continue;

      // here we need to check if merge-file has the same name as a geometry in mergeto-file.
      if (mergeblock->getFileNumber() == merge->getFileNumber())
      {  
         // now check if the same blockname also exist in global or file->mergeto

         for (int ii=0; ii<doc->getMaxBlockIndex(); ii++)
         {
            BlockStruct *block = doc->getBlockAt(ii);
            if (block == NULL)   continue;

            // here we need to check if merge-file has the same name as a geometry in mergeto-file.
            if (!strcmp(block->getName(), mergeblock->getName()))
            {  
               // here is the same blockname in merge and mergeto
               int r = 0;
            }
         }
      }
   }
   return 1;
}

static void updateBlockFileNumber(CCEtoODBDoc *doc, BlockStruct *block, int oldFileNumber, int newFileNumber)
{
   // Update the file number in this block, and recursively in any block in the chain of
   // references that stem from this block.

   if (block)
   {
      if (block->getFileNumber() == oldFileNumber)
         block->setFileNumber(newFileNumber);

      // Update blocks referenced in Data list
      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         if (data)
         {  
            InsertStruct *insert = data->getInsert();
            if (insert)
            {

               BlockStruct *b = doc->getBlockAt(insert->getBlockNumber());
               if (b)
               {
                  updateBlockFileNumber(doc, b, oldFileNumber, newFileNumber);
               }
            }
         }
      }

      // SPECIAL CASE for Apertures
      // These have a side-step block reference, not in the Data list
      // Got to update these too, or crash later. Related to case 1741.
      if (block->isComplexAperture())
      {
         int apSubBlockNum = block->getComplexApertureSubBlockNumber();
         BlockStruct *b = doc->getBlockAt(apSubBlockNum);
         updateBlockFileNumber(doc, b, oldFileNumber, newFileNumber);
      }
   }
}

/*****************************************************************************/
/*
*/
void MergeTwoFiles(FileStruct *merge, FileStruct *mergeTo, CCEtoODBDoc *doc)
{
   // Force a redraw at the end if the merge-in is not shown and merge-to is shown
   bool doRedraw = !merge->isShown() && mergeTo->isShown();

   // On the other hand, if the merge-in is shown and merge-to is not shown,
   // nothing would be drawn and the old merge-in ghost image would remain. 
   // So set the merge-to to be shown and force the redraw.
   if (!mergeTo->isShown() && merge->isShown())
   {
      mergeTo->setShow(true);
      doRedraw = true;
   }

   // If neither merge-in nor merge-to are currently shown, then they
   // shall remain not shown. Presumably the user did that on purpose.


   // Move data lists
   while (!merge->getBlock()->getDataList().IsEmpty())
   {
      // need to transpose datas
      mergeTo->getBlock()->getDataList().AddTail(merge->getBlock()->getDataList().GetHead());
      merge->getBlock()->getDataList().RemoveHead();
   }

   // NetList
   for (POSITION netPos = merge->getNetList().getHeadPosition();netPos != NULL;)
   {
      POSITION oldPos = netPos;
      NetStruct* net = merge->getNetList().GetNext(netPos);

      NetStruct* toNet = mergeTo->getNetList().addNet(net->getNetName(),net->getEntityNumber());
      toNet->takeCompPins(*net);

      merge->getNetList().deleteAt(oldPos);
   }

   //while (!merge->getNetList().IsEmpty())
   //{
   //   mergeTo->getNetList().AddTail(merge->getNetList().GetHead());
   //   merge->getNetList().RemoveHead();
   //}

   // DRCList
   mergeTo->getDRCList().takeData(merge->getDRCList());

   //while (!merge->getDRCList().IsEmpty())
   //{
   //   mergeTo->getDRCList().AddTail(merge->getDRCList().GetHead());
   //   merge->getDRCList().RemoveHead();
   //}

   // TypeList
   mergeTo->getTypeList().takeData(merge->getTypeList());

   //while (!merge->getTypeList().IsEmpty())
   //{
   //   mergeTo->getTypeList().AddTail(merge->getTypeList().GetHead());
   //   merge->getTypeList().RemoveHead();
   //}

   // Update file numbers in blocks, otherwise FreeFile is going to nuke all
   // the old file's blocks, even though there are references in the mergetTo
   // files's data list. Case 1338.
   updateBlockFileNumber(doc, mergeTo->getBlock(), merge->getBlock()->getFileNumber(),
      mergeTo->getBlock()->getFileNumber());

   // Clean up removed files
   // It is always okay to take the file out of the file list, 
   // but first check for dup file numbers
   //bool hasDupFNs = doc->HasDuplicateFileNumbers();
   //doc->getFileList().RemoveAt(doc->getFileList().Find(merge));

   //// Case 1741: It is Not always okay to delete the file's blocks.
   //// This case presented a ccz file in which two distinct files have the same file number.
   //// FreeFile() determines block to file association by matching block file numbers.
   //// This can result is deleting the wrong blocks, i.e. blocks that belong to the
   //// "merge to" file, eventually causing camcad to crash. To resolve this we shall
   //// examine the file list. If there are any duplicated file numbers in the list of
   //// files, then do not free any file's blocks.
   //doc->FreeFile(merge, !hasDupFNs);

   doc->getFileList().deleteFile(merge);
   merge = NULL;

   if (doRedraw)
      doc->OnRedraw();
}

/////////////////////////////////////////////////////////////////////////////
// MergeFiles dialog
MergeFiles::MergeFiles(CWnd* pParent /*=NULL*/)
   : CDialog(MergeFiles::IDD, pParent)
{
   //{{AFX_DATA_INIT(MergeFiles)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void MergeFiles::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(MergeFiles)
   DDX_Control(pDX, IDC_FILES, m_files);
   DDX_Control(pDX, IDC_MERGE_TO, m_mergeTo);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(MergeFiles, CDialog)
   //{{AFX_MSG_MAP(MergeFiles)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MergeFiles message handlers
BOOL MergeFiles::OnInitDialog() 
{
   CDialog::OnInitDialog();

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = doc->getFileList().GetNext(pos);
      m_mergeTo.SetItemDataPtr(m_mergeTo.AddString(file->getName()), file);
      m_files.SetItemDataPtr(m_files.AddString(file->getName()), file);
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void MergeFiles::OnOK() 
{
   int count = m_files.GetSelCount(); // number of files to merge
   if (!count)
      return;
   
   int mergeToIndex = m_mergeTo.GetCurSel(); // index of file to merge to
   if (mergeToIndex == LB_ERR)
      return;

   FileStruct *mergeTo = (FileStruct*)m_mergeTo.GetItemDataPtr(mergeToIndex); // file to merge to
   RestructureFile(doc, mergeTo);

   int *indexArray = (int*)calloc(count, sizeof(int)); // array of indices of files to merge
   m_files.GetSelItems(count, indexArray);

   // here make sure all local blocks are declared global in the mergeto file !!!
   if (!Make_MergeTo_Blocks_Global(doc, mergeTo))  
   {
      ErrorMessage("No Merge Possible!", "Error in Merge Files");
      return;
   }

   // loop files to merge
   for (int i=0; i<count; i++)
   {
      FileStruct *file = (FileStruct*)m_files.GetItemDataPtr(indexArray[i]);

      if (file == mergeTo)
         continue;

      RestructureFile(doc, file);

      // here we need to check for duplicated block definitions.
      if (CheckGeometries(file, mergeTo, doc))
      {
         MergeTwoFiles(file, mergeTo, doc);
         mergeTo->getBlock()->resetExtent();
      }
      else
      {
         ErrorMessage("Colliding Geometry definitions","Merge Problem");
      }
   }

   // free array
   free(indexArray);

   CDialog::OnOK();
}
