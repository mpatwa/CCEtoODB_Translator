
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.
*/           
/*****************************************************************************/

#include "stdafx.h"
#include "file.h"
#include "ccdoc.h"
#include "MergeFiles.h"
#include "pcbutil.h"
#include "math.h"
#include "drc.h"
#include "TMState.h"
#include "DFT.h"
#include "Variant.h"
#include "MultipleMachine.h"
#include "EntityNumber.h"

extern CView *activeView; // from CCVIEW.CPP
extern int NextFileNum; // from GRAPH.CPP

void RegenerateDRCLocation(FileStruct *file);
void RegenerateAccessLocation(CCEtoODBDoc *doc, FileStruct *file);

//_____________________________________________________________________________
CAuxiliaryFileStruct::CAuxiliaryFileStruct(const FileStruct& fileStruct)
: m_fileStruct(fileStruct)
{
   m_pDFTSolutions   = NULL;
   m_pCurDFTSolution = NULL;
	m_pMachineList		= NULL;
	m_pCurMachine		= NULL;
}

CAuxiliaryFileStruct::~CAuxiliaryFileStruct()
{
   if (m_pDFTSolutions != NULL)
   {
      POSITION pos = m_pDFTSolutions->GetHeadPosition();
      while (pos)
      {
         CDFTSolution *pSolution = m_pDFTSolutions->GetNext(pos);
         pSolution->DestroySolutions();
      }
      delete m_pDFTSolutions;
   }

	if (m_pMachineList != NULL)
	{
		delete m_pMachineList;
		m_pMachineList = NULL;
	}
}

CDFTSolutionList& CAuxiliaryFileStruct::getDFTSolutions()
{
   if (m_pDFTSolutions == NULL)
      m_pDFTSolutions = new CDFTSolutionList();

   return *m_pDFTSolutions;
}

CDFTSolution* CAuxiliaryFileStruct::AddNewDFTSolution(CString name, bool isFlipped, PageUnitsTag pageUnits)
{
   CDFTSolution* newSolution = getDFTSolutions().AddNewDFTSolution(name, isFlipped, pageUnits);    
   return newSolution;
}

CDFTSolution* CAuxiliaryFileStruct::FindDFTSolution(CString name, bool isFlipped)
{
   if (m_pDFTSolutions == NULL)
      return NULL;
   else
      return m_pDFTSolutions->FindDFTSolution(name, isFlipped);
}

CDFTSolution* CAuxiliaryFileStruct::GetCurrentDFTSolution()
{
   // If m_pCurDFTSolution is already set, fine, just use it.
   // If it is not set then try activating the solution named in the base file.
   // If that works, fine.
   // If that does not work, leave it, do not activate any others even if the solution list is not empty.
   // It is up to the user to designate which is current, not us.

   if (this->m_pCurDFTSolution == NULL)
   {
      CString solnName( this->getFileStruct().getCurrentDftSolutionName() );
      if (!solnName.IsEmpty())
      {
         m_pCurDFTSolution = this->FindDFTSolution(solnName, false/*isFlipped*/);
      }
   }

   return this->m_pCurDFTSolution;
}

int CAuxiliaryFileStruct::SetCurrentDFTSolution(POSITION pos)
{
   if (pos == NULL)
      m_pCurDFTSolution = NULL;
   else
      m_pCurDFTSolution = getDFTSolutions().GetAt(pos);
   return 0;
}

int CAuxiliaryFileStruct::SetCurrentDFTSolution(CString name, bool isFlipped)
{
   m_pCurDFTSolution = FindDFTSolution(name, isFlipped);
   return 0;
}

int CAuxiliaryFileStruct::SetCurrentDFTSolution(CDFTSolution *dftSolution)
{
   m_pCurDFTSolution = dftSolution;
   return 0;
}

CMachineList* CAuxiliaryFileStruct::CreatMachineList(CCEtoODBDoc& camCadDoc)
{
   if (m_pMachineList == NULL)
      m_pMachineList = new CMachineList(camCadDoc,&m_fileStruct);

   return m_pMachineList;
}

CMachineList* CAuxiliaryFileStruct::getMachineList()
{
   return m_pMachineList;
}

int CAuxiliaryFileStruct::GetMachineCount()
{
   if (m_pMachineList != NULL)
      return m_pMachineList->GetCount();
   else
      return 0;
}

POSITION CAuxiliaryFileStruct::GetMachineHeadPosition()
{
   if (m_pMachineList != NULL)
      return m_pMachineList->GetHeadPosition();
   else
      return NULL;
}

POSITION CAuxiliaryFileStruct::GetMachineTailPosition()
{
   if (m_pMachineList != NULL)
      return m_pMachineList->GetTailPosition();
   else
      return NULL;
}

CMachine* CAuxiliaryFileStruct::GetMachineHead()
{
   if (m_pMachineList != NULL)
      return m_pMachineList->GetHead();
   else
      return NULL;
}

CMachine* CAuxiliaryFileStruct::GetMachineTail()
{
   if (m_pMachineList != NULL)
      return m_pMachineList->GetTail();
   else
      return NULL;
}

CMachine* CAuxiliaryFileStruct::GetMachineNext(POSITION &pos)
{
   if (m_pMachineList != NULL)
      return m_pMachineList->GetNext(pos);
   else
      return NULL;
}

CMachine* CAuxiliaryFileStruct::GetMachinePrev(POSITION &pos)
{
   if (m_pMachineList != NULL)
      return m_pMachineList->GetPrev(pos);
   else
      return NULL;
}

CMachine* CAuxiliaryFileStruct::FindMachine(CString name)
{
   if (m_pMachineList != NULL)
      return m_pMachineList->FindMachine(name);
   else
      return NULL;
}

CMachine* CAuxiliaryFileStruct::GetCurrentMachine()
{
   if (m_pCurMachine && m_pMachineList->Find(m_pCurMachine) == NULL)
      m_pCurMachine = NULL;
   return m_pCurMachine;
}

int CAuxiliaryFileStruct::SetCurrentMachine(POSITION pos)
{
   if (m_pMachineList == NULL)
      return -1;

   CMachine* machine = m_pMachineList->GetAt(pos);
   if (machine == NULL)
      return -1;

   m_pCurMachine = machine;
   return 0;
}

int CAuxiliaryFileStruct::SetCurrentMachine(CString name)
{
   if (m_pMachineList == NULL)
      return -1;

   CMachine* machine = m_pMachineList->FindMachine(name);
   if (machine == NULL)
      return -1;

   m_pCurMachine = machine;
   return 0;
}

int CAuxiliaryFileStruct::SetCurrentMachine(CMachine* machine)
{
   if (m_pMachineList == NULL || machine == NULL)
      return -1;

   // The machine is not in the list so can't set surrent machine
   // since current machine MUST be a machine in the list
   if (m_pMachineList->Find(machine) == NULL)
      return -1;

   m_pCurMachine = machine;
   return 0;
}


/******************************************************************************
* Add_File
*     - creates a new file block to use
*     - Adds a file to the filelist
*     - returns the FileStruct
*     source_cad - see FileTypeTag
*/
FileStruct *CCEtoODBDoc::Add_File(CString name, int source_cad)
{
   FileStruct *file = getCamCadData().getFileList().addNewFile(name,intToFileTypeTag(source_cad));   
   return file;
}

/*******************************************************************************
* Find_File
*        NULL if not found.
*/
FileStruct *CCEtoODBDoc::Find_File(int num)
{
   if (!num)
   {
      return NULL;
   }
   FileStruct *npf;

   
   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      npf = getFileList().GetNext(pos);

      if (npf->getFileNumber() == num)
         return npf;
   }
   return NULL;
}

/*******************************************************************************
* Find_File
*        NULL if not found.
*/
FileStruct *CCEtoODBDoc::Find_File_by_BlockPtr(BlockStruct *b)
{
   if (!b)
   {
      return NULL;
   }
   FileStruct *npf;

   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      npf = getFileList().GetNext(pos);

      if (npf->getBlock() == b)
         return npf;
   }
   return NULL;
}

/*******************************************************************************
* Find_File
*        NULL if not found.
*/
FileStruct *CCEtoODBDoc::Find_File_by_BlockGeomNum(int geomnum)
{
   FileStruct *npf;

   POSITION pos = getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      npf = getFileList().GetNext(pos);

      if (npf->getBlock()->getBlockNumber() == geomnum)
         return npf;
   }
   return NULL;
}

FileStruct* CCEtoODBDoc::FileFileByName(CString fileName)
{
	return getFileList().FindByName(fileName);
}

void CCEtoODBDoc::SetShowAllFile(bool showFlag)
{
	getFileList().SetShowAll(showFlag);
}

/*******************************************************************************
*/

bool CCEtoODBDoc::HasDuplicateFileNumbers()
{
	// Not usually very many files, just do a nested loop search.

	POSITION pos1 = getFileList().GetHeadPosition();
	while (pos1 != NULL)
	{
		FileStruct *file1 = getFileList().GetAt(pos1);
		int  num1 = file1->getFileNumber();

		POSITION pos2 = getFileList().GetHeadPosition();
		while (pos2 != NULL)
		{
			FileStruct *file2 = getFileList().GetAt(pos2);
			if (pos1 != pos2) // skip if same file entry
			{
				int  num2 = file2->getFileNumber();
				if (num1 == num2)
				{
					return true;  // different file entries, same file number
				}
			}
			getFileList().GetNext(pos2);
		}
		getFileList().GetNext(pos1);
	}

	return false;
}

/******************************************************************************
* OnRestructureFiles
*
*  DESCRIPTION:
*     - Neutralizes files so File->Placement is (0, 0) scale=1, rotation=0, no mirror
*     - Reconfigures Datas so that they end up where the were
*
*     - Only opperates on visible files
*     - Adjusts inserts of File Blocks (ex. PCBs in Panels) so they end up in the correct spot
*/
void CCEtoODBDoc::OnRestructureFiles() 
{
   ((CCEtoODBView*)activeView)->FilterFirst = TRUE;

   generate_PADSTACKACCESSFLAG(this,true);

   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown())
         continue;

      RestructureFile(this, file);
   }

   OnRegen();

   UpdateAllViews(NULL);
}

/******************************************************************************
* RestructureFile
*/
void RestructureFile(CCEtoODBDoc *doc, FileStruct *file, bool normalizeRefNameRotation)
{
   if (fabs(file->getInsertX()) < SMALLNUMBER && fabs(file->getInsertY()) < SMALLNUMBER && fabs(file->getRotation()) < SMALLNUMBER && !file->isMirrored() && fabs(file->getScale() - 1) < SMALLNUMBER)
      return;

   Mat2x2 mFile;
   RotMat2(&mFile, file->getRotation());

   // transpose data by insert data
   for (CDataListIterator dataListIterator(file->getBlock()->getDataList()); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();
 
      if(file->isMirrored() && data->getDataType() != T_INSERT )
      {
         LayerStruct *layer = doc->FindLayer(data->getLayerIndex());
         if (layer)
            data->setLayerIndex(layer->getMirroredLayerIndex());
         
         // Set Span Layers
         if(data->HasLayerSpan())
         {
            int startLayer = doc->getLayerArray().GetPhysicalStackupMirror(data->GetPhysicalStartLayer());
            int endLayer = doc->getLayerArray().GetPhysicalStackupMirror(data->GetPhysicalEndLayer());

            if(startLayer == -1 || endLayer == -1) 
               startLayer = endLayer = -1;
            
            data->SetPhysicalStartLayer(startLayer);
            data->SetPhysicalEndLayer(endLayer);        
         }
      }

      switch (data->getDataType())
      {
      case T_POINT:
         {
            Point2 point2;
            point2.x = data->getPoint()->x * file->getScale();
            if (file->isMirrored())
               point2.x = -point2.x;
            point2.y = data->getPoint()->y * file->getScale();
            TransPoint2(&point2, 1, &mFile, file->getInsertX(), file->getInsertY());

            data->getPoint()->x = (DbUnit)point2.x;
            data->getPoint()->y = (DbUnit)point2.y;
         }
         break;

      case T_POLY:
         {
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);

                  Point2 point2;
                  point2.x = pnt->x * file->getScale();
                  if (file->isMirrored())
                     point2.x = -point2.x;
                  point2.y = pnt->y * file->getScale();
                  TransPoint2(&point2, 1, &mFile, file->getInsertX(), file->getInsertY());

                  if (file->isMirrored())
                     pnt->bulge = -pnt->bulge;

                  pnt->x = (DbUnit)point2.x;
                  pnt->y = (DbUnit)point2.y;
               }
            }
         }
         break;

      case T_TEXT:
         {
            Point2 point2;
            point2.x = data->getText()->getPnt().x * file->getScale();
            if (file->isMirrored())
               point2.x = -point2.x;
            point2.y = data->getText()->getPnt().y * file->getScale();
            TransPoint2(&point2, 1, &mFile, file->getInsertX(), file->getInsertY());

            data->getText()->setPnt((DbUnit)point2.x,(DbUnit)point2.y);

            data->getText()->setHeight(data->getText()->getHeight() * file->getScale());
            data->getText()->setWidth(data->getText()->getWidth() * file->getScale());
            data->getText()->setMirrored(data->getText()->getResultantMirror(file->isMirrored()));
            if (file->isMirrored())
               data->getText()->setRotation(NormalizeAngle(file->getRotation() - data->getText()->getRotation()));
            else
               data->getText()->setRotation(NormalizeAngle(file->getRotation() + data->getText()->getRotation()));
         }
         break;
      
      case T_INSERT:
         {
            Point2 point2;
            point2.x = data->getInsert()->getOriginX() * file->getScale();
            if (file->isMirrored())
               point2.x = -point2.x;
            point2.y = data->getInsert()->getOriginY() * file->getScale();
            TransPoint2(&point2, 1, &mFile, file->getInsertX(), file->getInsertY());

            data->getInsert()->setOriginX(point2.x);
            data->getInsert()->setOriginY(point2.y);

            data->getInsert()->setScale(data->getInsert()->getScale() * file->getScale());
            if (file->isMirrored())
               data->getInsert()->setAngle(NormalizeAngle(file->getRotation() - data->getInsert()->getAngle()));
            else
               data->getInsert()->setAngle(NormalizeAngle(file->getRotation() + data->getInsert()->getAngle()));

            data->getInsert()->setMirrorFlags(data->getInsert()->getMirrorFlags() ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0));
            if (file->isMirrored())
               data->getInsert()->setPlacedBottom(!data->getInsert()->getPlacedBottom());

            if(((insertTypePcbComponent == data->getInsert()->getInsertType())
               || (insertTypeTestProbe == data->getInsert()->getInsertType()))
               && normalizeRefNameRotation)
            {
               Attrib *attrib;
               if (attrib =  is_attvalue(doc, data->getAttributesRef(), ATT_REFNAME, 1))
               {
                  if (file->isMirrored())
                     attrib->setRotationRadians(NormalizeAngle(file->getRotation() - attrib->getRotationRadians()));
                  else
                     attrib->setRotationRadians(NormalizeAngle(attrib->getRotationRadians() - file->getRotation()));
               }
            }
         }
         break;

      case T_POLY3D:
         {
            POSITION polyPos = data->getPoly3DList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly3D *poly = data->getPoly3DList()->GetNext(polyPos);

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt3D *pnt = poly->getPntList().GetNext(pntPos);

                  Point2 point2;
                  point2.x = pnt->x * file->getScale();
                  if (file->isMirrored())
                     point2.x = -point2.x;
                  point2.y = pnt->y * file->getScale();
                  TransPoint2(&point2, 1, &mFile, file->getInsertX(), file->getInsertY());

                  pnt->x = (DbUnit)point2.x;
                  pnt->y = (DbUnit)point2.y;
                  pnt->setZ(pnt->z * file->getScale());
               }
            }
         }
         break;

      }
   }

   // Fix inserts of this block
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      
      if (block == NULL)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         DataStruct *data = block->getDataList().GetNext(dataPos);
         
         if (data->getDataType() == T_INSERT && data->getInsert()->getBlockNumber() == file->getBlock()->getBlockNumber())
         {
            if (file->isMirrored() != data->getInsert()->getGraphicMirrored())
               data->getInsert()->setAngle(NormalizeAngle( file->getRotation() + data->getInsert()->getAngle()));
            else
               data->getInsert()->setAngle(NormalizeAngle(-file->getRotation() + data->getInsert()->getAngle()));


            data->getInsert()->setScale(data->getInsert()->getScale() / file->getScale());

            //*BUG: Mixing bool mirror and separate mirror flags does not work out well.
            //*BUG: data->getInsert()->setMirrorFlags(file->getResultantMirror(data->getInsert()->getMirrorFlags()));
            // If mirror gets set then set both flip and layer.
            data->getInsert()->setMirrorFlags( (file->getResultantMirror(data->getInsert()->getGraphicMirrored())) ? (MIRROR_FLIP | MIRROR_LAYERS) : 0 );

            Mat2x2 mInsrt;
            RotMat2(&mInsrt, data->getInsert()->getAngle());

            Point2 point2;
            point2.x = -file->getInsertX() * data->getInsert()->getScale();
            if (data->getInsert()->getMirrorFlags())
               point2.x = -point2.x;
            point2.y = -file->getInsertY() * data->getInsert()->getScale();
            TransPoint2(&point2, 1, &mInsrt, 0.0, 0.0);
            data->getInsert()->incOriginX(point2.x);
            data->getInsert()->incOriginY(point2.y);
         }
      }
   }

   RegenerateDRCLocation(file);
   RegenerateAccessLocation(doc,file);

   file->getBlock()->resetExtent();

   // Neutralize File
   file->setScale((DbUnit)1.0);
   file->setInsertX(0.);
   file->setInsertY(0.);
   file->setRotation((DbUnit)0.0);
   file->setMirrored(false);
   
   generate_PINLOC(doc, file, 1); // do it always
}

/******************************************************************************
* RegenerateDRCLocation
*/
void RegenerateDRCLocation(FileStruct *file)
{
   Mat2x2 m;
   RotMat2(&m, file->getRotation());

   POSITION drcPos = file->getDRCList().GetHeadPosition();
   while (drcPos )
   {
      DRCStruct *drc = file->getDRCList().GetNext(drcPos);

      Point2 point2;
      point2.x = drc->getOrigin().x * file->getScale();
      if (file->isMirrored())
         point2.x = -point2.x;
      point2.y = drc->getOrigin().y * file->getScale();
      TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());

      drc->setOriginX(point2.x);
      drc->setOriginY(point2.y);

      if (drc->getDrcClass() == DRC_CLASS_MEASURE)
      {

         DRC_MeasureStruct *measure = (DRC_MeasureStruct*)drc->getVoidPtr();

         // X1 & Y1 of measure
         point2.x = measure->x1 * file->getScale();
         if (file->isMirrored())
            point2.x = -point2.x;
         point2.y = measure->y1 * file->getScale();
         TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());
         measure->x1 = (DbUnit)point2.x;
         measure->y1 = (DbUnit)point2.y;

         // X2 & Y2 of measure
         point2.x = measure->x2 * file->getScale();
         if (file->isMirrored())
            point2.x = -point2.x;
         point2.y = measure->y2 * file->getScale();
         TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());
         measure->x2 = (DbUnit)point2.x;
         measure->y2 = (DbUnit)point2.y;
      }
   }

}

/******************************************************************************
* RegenerateAccessLocation
*/
void RegenerateAccessLocation(CCEtoODBDoc *doc, FileStruct *file)
{
   if(!doc || !file) return;

   Mat2x2 m;
   RotMat2(&m, file->getRotation());

   /*for(POSITION DFTPOS = doc->getDFTSolutions(*file).GetHeadPosition();DFTPOS;)
   {
      CDFTSolution *DFTSolution = doc->getDFTSolutions(*file).GetNext(DFTPOS);
      if(DFTSolution)
      {
         CAccessAnalysisSolution* aaSolution = DFTSolution->GetAccessAnalysisSolution();
         
         for(POSITION accessMapPos = (aaSolution)?aaSolution->GetNetAccessMap().GetStartPosition():NULL;accessMapPos;)
         {
            CString NetName;
            CAANetAccess* netAccess = NULL;
            aaSolution->GetNetAccessMap().GetNextAssoc(accessMapPos,NetName,(CObject*&)netAccess);

            for(POSITION accessPos = (netAccess)?netAccess->GetHeadPosition_AccessibleLocations():NULL;accessPos;)
            {
               CAAAccessLocation *accessLoc = netAccess->GetNext_AccessibleLocations(accessPos);
               if(accessLoc && accessLoc->GetAccessible())
               {
                  Point2 point2;
                  point2.x = accessLoc->GetAccessLocation().x * file->getScale();
                  if (file->isMirrored())
                     point2.x = -point2.x;
                  point2.y = accessLoc->GetAccessLocation().y * file->getScale();
                  TransPoint2(&point2, 1, &m, file->getInsertX(), file->getInsertY());
                  
                  accessLoc->SetAccessLocation(point2.x,point2.y);
                  if (file->isMirrored())
                  {
                     accessLoc->SetAccessSurface((accessLoc->GetAccessSurface() == testSurfaceTop)?testSurfaceBottom:testSurfaceTop);
                  }
                     
               }//AccessLocation

            }//Netaccess

         }//AccessAnalysisSolution        
 
      }//DFTSolution

   }*///DFTSolutionList
}

//_____________________________________________________________________________
CAuxiliaryFiles::CAuxiliaryFiles()
{
   m_fileArray.SetSize(0,20);
}

CAuxiliaryFileStruct& CAuxiliaryFiles::getDefinedAuxiliaryFileStruct(const FileStruct& fileStruct)
{
   int fileIndex = fileStruct.getFileNumber();

   if (fileIndex < 0)
   {
      fileIndex = 0;
   }

   CAuxiliaryFileStruct* auxiliaryFileStruct = NULL;

   if (fileIndex < m_fileArray.GetSize())
   {
      auxiliaryFileStruct = m_fileArray.GetAt(fileIndex);
   }

   if (auxiliaryFileStruct == NULL || (&(auxiliaryFileStruct->getFileStruct()) != &fileStruct))
   {
      auxiliaryFileStruct = new CAuxiliaryFileStruct(fileStruct);
      m_fileArray.setAtGrow(fileIndex,auxiliaryFileStruct);
   }

   return *auxiliaryFileStruct;
}

