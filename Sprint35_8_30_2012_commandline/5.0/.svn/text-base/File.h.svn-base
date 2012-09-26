// $Header: /CAMCAD/5.0/File.h 85    6/30/07 2:27a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994, 1997. All Rights Reserved.
*/

#if !defined(__File_h__)
#define __File_h__

#pragma once

#include <afxtempl.h>
#include "dbutil.h"
#include "layer.h"
#include "block.h"
#include "resource.h"
#include "bus.h"
#include "Units.h"

#include "DcaCompPin.h"
#include "DcaNet.h"
#include "DcaFile.h"

#define VISIBLE_NONE    0x0
#define VISIBLE_TOP     0x1
#define VISIBLE_BOTTOM  0x2
#define VISIBLE_BOTH    0x3   // VISIBLE_TOP | VISIBLE_BOTTOM

class CAttributes;
class CDFTSolution;
class CDFTSolutionList;
class CMachine;
class CMachineList;
class CVariantList;
class CProbeStyleList;

extern void RestructureFile(CCEtoODBDoc *doc, FileStruct *file, bool normalizeRefNameRotation = false);

//_____________________________________________________________________________
class CAuxiliaryFileStruct
{
private:
   const FileStruct& m_fileStruct;

   CDFTSolutionList* m_pDFTSolutions;
   CDFTSolution*     m_pCurDFTSolution;

   CMachineList*     m_pMachineList;
   CMachine*         m_pCurMachine;

public:
   CAuxiliaryFileStruct(const FileStruct& fileStruct);
   ~CAuxiliaryFileStruct();

   const FileStruct& getFileStruct() { return m_fileStruct; }

   // DFT Solution
   CDFTSolutionList& getDFTSolutions();

   // temporary, to be removed after 4.6 Access Analysis code is released to 4.5 - knv 20060406
   CDFTSolution* AddNewDFTSolution(CString name,bool isFlipped, int pageUnits) { return AddNewDFTSolution(name, isFlipped, intToPageUnitsTag(pageUnits)); }

   CDFTSolution* AddNewDFTSolution(CString name, bool isFlipped, PageUnitsTag pageUnits);
	CDFTSolution* FindDFTSolution(CString name, bool isFlipped);
   CDFTSolution* GetCurrentDFTSolution();
   int SetCurrentDFTSolution(POSITION pos);
	int SetCurrentDFTSolution(CString name, bool isFlipped);
	int SetCurrentDFTSolution(CDFTSolution* dftSolution);

	// Multiple Machine
	CMachineList* CreatMachineList(CCEtoODBDoc& camCadDoc);
   CMachineList* getMachineList();
	int GetMachineCount();
	POSITION GetMachineHeadPosition();
	POSITION GetMachineTailPosition();
	CMachine* GetMachineHead();
	CMachine* GetMachineTail();
	CMachine* GetMachineNext(POSITION &pos);
	CMachine* GetMachinePrev(POSITION &pos);
	CMachine* FindMachine(CString name);
	CMachine* GetCurrentMachine();
	int SetCurrentMachine(POSITION pos);
	int SetCurrentMachine(CString name);
	int SetCurrentMachine(CMachine* machine);
};

//_____________________________________________________________________________
class CAuxiliaryFiles
{
private:
   CTypedPtrArrayContainer<CAuxiliaryFileStruct*> m_fileArray;

public:
   CAuxiliaryFiles();

   //CAuxiliaryFileStruct* getAuxiliaryFileStruct(FileStruct& fileStruct);
   CAuxiliaryFileStruct& getDefinedAuxiliaryFileStruct(const FileStruct& fileStruct);

};


#endif
