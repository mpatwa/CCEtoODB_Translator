// $Header: /CAMCAD/5.0/Dca/DcaMachine.h 2     3/19/07 4:34a Kurt Van Ness $

#if !defined(__DcaMachine_h__)
#define __DcaMachine_h__

//#pragma once

#include "DcaPoint2d.h"
#include "DcaContainer.h"

#define MACHINE_TYPE_ICT_STR		"ICT"
#define MACHINE_TYPE_FPT_STR		"FPT"
#define MACHINE_TYPE_MDA_STR		"MDA"
#define MACHINE_TYPE_AXI_STR		"AXI"
#define MACHINE_TYPE_AOI_STR		"AOI"
#define MACHINE_TYPE_PNP_STR		"PNP"
#define MACHINE_TYPE_CUT_STR		"CUSTOM"

#define MACHINE_TYPE_ICT			1			// might use DFT
#define MACHINE_TYPE_FPT			2			// might use DFT
#define MACHINE_TYPE_MDA			4			// might use DFT
#define MACHINE_TYPE_AXI			8			// do not use DFT
#define MACHINE_TYPE_AOI			16			// do not use DFT
#define MACHINE_TYPE_PNP			32			// do not use DFT
#define MACHINE_TYPE_CUT			64			// do not use DFT

class CDcaDftSolution;
class FileStruct;

enum FileTypeTag;

int getMachineType(FileTypeTag fileType);
CString getMachineName(FileTypeTag fileType);
CString getMachineTypeString(int machineType);
CString getMachineTypeString(FileTypeTag fileType);
bool isTestMachine(FileTypeTag fileType);
FileTypeTag getFileType(CString machineName);
//_____________________________________________________________________________
class CDcaMachine
{
public:
   CDcaMachine(FileTypeTag fileType);
   ~CDcaMachine();

private:
   FileTypeTag m_eFileType;
   CDcaDftSolution* m_dftSolution;
   CPoint2d m_ptOrigin;

public:
   CString GetName() const;
   CString GetMachineTypeString() const;
   int GetMachineType() const;

   FileTypeTag GetFileType()         const { return m_eFileType;            };
   CDcaDftSolution* GetDftSolution() const { return m_dftSolution;          };
   CPoint2d GetOrigin()              const { return m_ptOrigin;             };
   
   void SetDftSolution(CDcaDftSolution* solution)  { m_dftSolution = solution;      };
   void SetOrigin(const CPoint2d& origin)          { m_ptOrigin = origin;           };
};

//_____________________________________________________________________________
class CDcaMachineList : public CTypedPtrListContainer<CDcaMachine*>
{
public:
	CDcaMachineList(FileStruct* pPCBFile);
	~CDcaMachineList();

public:
	//CDcaMachine* FindMachine(CString name);

	//void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress);
	//int LoadXML(CString xmlString, CCEtoODBDoc *doc, FileStruct *file);
	//void Scale(double factor);
};

#endif
