
#include "StdAfx.h"
#include "XmlWrite.h"
#include "CcDoc.h"
#include "Dft.h"
#include "MultipleMachine.h"

//_____________________________________________________________________________
CCamCadDcaCamCadFileWriter::CCamCadDcaCamCadFileWriter(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, CDcaCamCadFileWriter(camCadDoc.getCamCadData())
{
}

CCamCadDcaCamCadFileWriter::~CCamCadDcaCamCadFileWriter()
{
}

void CCamCadDcaCamCadFileWriter::writeExtendedXmlAttributes(CWriteFormat& writeFormat,const FileStruct&        fileStruct,CCamCadFileWriteProgress& progress)
{
   /*CDFTSolution* dftSolution = getCamCadDoc().GetCurrentDFTSolution(fileStruct);
   CMachine*     machine     = getCamCadDoc().GetCurrentMachine(fileStruct);

   writeFormat.writef(" curDFTSolution=\"%s\""    ,(dftSolution != NULL) ? convertHtmlMetaCharacters(dftSolution->GetName()) : "");
   writeFormat.writef(" curMachineSolution=\"%s\"",(machine     != NULL) ? convertHtmlMetaCharacters(machine->GetName())     : "");*/
}

void CCamCadDcaCamCadFileWriter::writeExtendedXml          (CWriteFormat& writeFormat,const FileStruct&        fileStruct,CCamCadFileWriteProgress& progress)
{
   /*CDFTSolutionList& dftSolutionList = getCamCadDoc().getDFTSolutions(fileStruct);
   CMachineList*     machineList     = getCamCadDoc().getMachineList(fileStruct);

   if (dftSolutionList.GetCount() > 0)
   {
      dftSolutionList.WriteXML(writeFormat,progress);
   }

   if (machineList != NULL && machineList->GetCount() > 0)
   {
      machineList->WriteXML(writeFormat,progress);
   }*/
}

