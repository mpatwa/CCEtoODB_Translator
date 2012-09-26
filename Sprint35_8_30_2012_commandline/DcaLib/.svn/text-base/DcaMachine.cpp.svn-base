// $Header: /CAMCAD/DcaLib/DcaMachine.cpp 3     5/22/07 1:37a Rick Faltersack $

#include "StdAfx.h"
#include "DcaMachine.h"
#include "DcaFileType.h"

//_____________________________________________________________________________
int getMachineType(FileTypeTag fileType)
{
   int machineType = fileTypeUnknown;

   switch (fileType)
   {
   case fileTypeAeroflexCb:
   case fileTypeHp3070:
   case fileTypeIpl:
   case fileTypeTeradyne228xCkt:
   case fileTypeTeradyne228xNav:
   case fileTypeTeradyneSpectrum: 
   case fileTypeAgilentI1000ATD:
   case fileTypeHiokiICT:
      machineType = MACHINE_TYPE_ICT;     
      break;

   case fileTypeScorpion:           
      machineType = MACHINE_TYPE_ICT | MACHINE_TYPE_FPT;
      break;

   case fileTypeSeicaParNod:
   case fileTypeSpea4040:
   case fileTypeTakaya9:            
      machineType = MACHINE_TYPE_FPT;
      break;

   case fileTypeTriMda:              
      machineType = MACHINE_TYPE_MDA;
      break;

   case fileTypeHp5dx:              
      machineType = MACHINE_TYPE_AXI;
      break;

   case fileTypeAgilentAoi:
   case fileTypeSonyAoi:
   case fileTypeRti:
   case fileTypeMvp3DPasteAoi:
   case fileTypeMvpComponentAoi:
   case fileTypeOrbotech:
   case fileTypeVITech:
   case fileTypeTeradyne7200:
   case fileTypeTeradyne7300:
   case fileTypeTriAoi:
   case fileTypeViscomAoi:
   case fileTypeSakiAOI:
   case fileTypeCyberOpticsAOI:
   case fileTypeOmronAOI:
   case fileTypeYestechAOI:
      machineType = MACHINE_TYPE_AOI;
      break;

   case fileTypeSiemensQd:
   case fileTypeJuki:
   case fileTypeMYDATAPCB:
   case fileTypeFujiFlexa:
      machineType = MACHINE_TYPE_PNP;
      break;
   case fileTypeCustomAssembly:
      machineType = MACHINE_TYPE_CUT;
      break;
   }

   return machineType;
}

//_____________________________________________________________________________
CString getMachineName(FileTypeTag fileType)
{
   CString machineName;

   switch (fileType)
   {
   case fileTypeAeroflexCb:         machineName = "Aeroflex";              break;
   case fileTypeHp3070:             machineName = "Agilent 3070";          break;
   case fileTypeIpl:                machineName = "Teradyne Z1800";        break;
   case fileTypeTeradyne228xCkt:    machineName = "Teradyne (GR) 228X CKT";    break;
   case fileTypeTeradyne228xNav:    machineName = "Teradyne (GR) 228X NAV";    break;
   case fileTypeTeradyneSpectrum:   machineName = "Teradyne Spectrum";     break;
   case fileTypeScorpion:           machineName = "Scorpion";              break;
   case fileTypeSeicaParNod:        machineName = "Seica";                 break;
   case fileTypeSpea4040:           machineName = "SPEA 4040";             break;
   case fileTypeTakaya9:            machineName = "Takaya 8400/9400";      break;
   case fileTypeTriMda:             machineName = "TRI MDA";               break;
   case fileTypeHp5dx:              machineName = "Agilent 5DX";           break;
   case fileTypeAgilentAoi:         machineName = "Agilent AOI";           break;
   case fileTypeAgilentI1000ATD:    machineName = "Agilent i1000";         break;
   case fileTypeRti:                machineName = "CR Technology";         break;
   case fileTypeMvp3DPasteAoi:      machineName = "MVP 3D Paste";          break;
   case fileTypeMvpComponentAoi:    machineName = "MVP Component AOI";     break;
   case fileTypeOrbotech:           machineName = "Orbotech AOI";          break;
   case fileTypeTeradyne7200:       machineName = "Teradyne 7200";         break;
   case fileTypeTeradyne7300:       machineName = "Teradyne 7300";         break;
   case fileTypeTriAoi:             machineName = "TRI AOI";               break;
   case fileTypeViscomAoi:          machineName = "Viscom AOI";            break;
   case fileTypeSiemensQd:          machineName = "Siemens Siplace";       break;
   case fileTypeSonyAoi:            machineName = "Sony AOI";              break;
   case fileTypeVITech:             machineName = "VI Technology AOI";     break;
   case fileTypeJuki:               machineName = "Juki";                  break;
   case fileTypeMYDATAPCB:          machineName = "MYDATA";                break;
   case fileTypeFujiFlexa:          machineName = "Fuji Flexa";            break;
   case fileTypeSakiAOI:            machineName = "Saki AOI";              break;
   case fileTypeHiokiICT:           machineName = "Hioki ICT";             break;
   case fileTypeCyberOpticsAOI:     machineName = "CyberOptics AOI";       break;
   case fileTypeOmronAOI:           machineName = "Omron AOI";             break;
   case fileTypeYestechAOI:         machineName = "Yestech AOI";           break;
   }

   return machineName;
}

//_____________________________________________________________________________
CString getMachineTypeString(int machineType)
{
   CString machineTypeStr;

   if (machineType & MACHINE_TYPE_ICT)
      machineTypeStr.AppendFormat("%s,", MACHINE_TYPE_ICT_STR);

   if (machineType & MACHINE_TYPE_FPT)
      machineTypeStr.AppendFormat("%s,", MACHINE_TYPE_FPT_STR);

   if (machineType & MACHINE_TYPE_MDA)
      machineTypeStr.AppendFormat("%s,", MACHINE_TYPE_MDA_STR);

   if (machineType & MACHINE_TYPE_AXI)
      machineTypeStr.AppendFormat("%s,", MACHINE_TYPE_AXI_STR);

   if (machineType & MACHINE_TYPE_AOI)
      machineTypeStr.AppendFormat("%s,", MACHINE_TYPE_AOI_STR);

   if (machineType & MACHINE_TYPE_PNP)
      machineTypeStr.AppendFormat("%s,", MACHINE_TYPE_PNP_STR);

   if (machineType & MACHINE_TYPE_CUT)
      machineTypeStr.AppendFormat("%s,", MACHINE_TYPE_CUT_STR);

   if (!machineTypeStr.IsEmpty())
      machineTypeStr = machineTypeStr.Left(machineTypeStr.GetLength()-1);

   return machineTypeStr;
}

//_____________________________________________________________________________
CString getMachineTypeString(FileTypeTag fileType)
{
   int machineType = getMachineType(fileType);
   CString machineTypeString = getMachineTypeString(machineType);
   
   return machineTypeString;
}

//_____________________________________________________________________________
bool isTestMachine(FileTypeTag fileType)
{
   switch (fileType)
   {
   case fileTypeAeroflexCb:
   case fileTypeHp3070:
   case fileTypeIpl:
   case fileTypeTeradyne228xCkt:
   case fileTypeTeradyne228xNav:
   case fileTypeTeradyneSpectrum:
   case fileTypeScorpion:
   case fileTypeSeicaParNod:
   case fileTypeSpea4040:
   case fileTypeTakaya9:
   case fileTypeTriMda:
   case fileTypeHp5dx:
   case fileTypeAgilentAoi:
   case fileTypeAgilentI1000ATD:
   case fileTypeSonyAoi:
   case fileTypeRti:
   case fileTypeMvp3DPasteAoi:
   case fileTypeMvpComponentAoi:
   case fileTypeOrbotech:
   case fileTypeTeradyne7200:
   case fileTypeTeradyne7300:
   case fileTypeTriAoi:
   case fileTypeViscomAoi:
   case fileTypeSiemensQd:
   case fileTypeVITech:
   case fileTypeJuki:
   case fileTypeMYDATAPCB:
   case fileTypeFujiFlexa:
   case fileTypeSakiAOI:
   case fileTypeHiokiICT:
   case fileTypeCyberOpticsAOI:
   case fileTypeOmronAOI:
   case fileTypeYestechAOI:
      return true;

   default:
      return false;
   }
}

//_____________________________________________________________________________
FileTypeTag getFileType(CString machineName)
{
   if (machineName.CompareNoCase("Aeroflex") == 0)             return fileTypeAeroflexCb;
   if (machineName.CompareNoCase("Agilent 3070") == 0)         return fileTypeHp3070;
   if (machineName.CompareNoCase("Teradyne Z1800") == 0)       return fileTypeIpl;
	if (machineName.CompareNoCase("Teradyne (GR) 228X") == 0)	return fileTypeTeradyne228xCkt;  // for backward compatibility
	if (machineName.CompareNoCase("Teradyne (GR) 228X CKT") == 0)	return fileTypeTeradyne228xCkt;
	if (machineName.CompareNoCase("Teradyne (GR) 228X NAV") == 0)	return fileTypeTeradyne228xNav; 
   if (machineName.CompareNoCase("Teradyne Spectrum") == 0)    return fileTypeTeradyneSpectrum;
   if (machineName.CompareNoCase("Scorpion") == 0)             return fileTypeScorpion;
   if (machineName.CompareNoCase("Seica") == 0)                return fileTypeSeicaParNod;
   if (machineName.CompareNoCase("SPEA 4040") == 0)            return fileTypeSpea4040;
   if (machineName.CompareNoCase("Takaya 8400/9400") == 0)     return fileTypeTakaya9;
   if (machineName.CompareNoCase("TRI MDA") == 0)              return fileTypeTriMda;
   if (machineName.CompareNoCase("Agilent 5DX") == 0)          return fileTypeHp5dx;
   if (machineName.CompareNoCase("Agilent AOI") == 0)          return fileTypeAgilentAoi;
   if (machineName.CompareNoCase("Agilent i1000") == 0)        return fileTypeAgilentI1000ATD;
   if (machineName.CompareNoCase("CR Technology") == 0)        return fileTypeRti;
   if (machineName.CompareNoCase("MVP 3D Paste") == 0)         return fileTypeMvp3DPasteAoi;
   if (machineName.CompareNoCase("MVP Component AOI") == 0)    return fileTypeMvpComponentAoi;
   if (machineName.CompareNoCase("Orbotech AOI") == 0)         return fileTypeOrbotech;
   if (machineName.CompareNoCase("Teradyne 7200") == 0)        return fileTypeTeradyne7200;
   if (machineName.CompareNoCase("Teradyne 7300") == 0)        return fileTypeTeradyne7300;
   if (machineName.CompareNoCase("TRI AOI") == 0)              return fileTypeTriAoi;
   if (machineName.CompareNoCase("Viscom AOI") == 0)           return fileTypeViscomAoi;
   if (machineName.CompareNoCase("Sony AOI") == 0)             return fileTypeSonyAoi;
   if (machineName.CompareNoCase("VI Technology AOI") == 0)    return fileTypeVITech;
   if (machineName.CompareNoCase("Siemens Siplace") == 0)      return fileTypeSiemensQd;
   if (machineName.CompareNoCase("Juki") == 0)                 return fileTypeJuki;
   if (machineName.CompareNoCase("MYDATA") == 0)               return fileTypeMYDATAPCB;
   if (machineName.CompareNoCase("Fuji Flexa") == 0)           return fileTypeFujiFlexa;
   if (machineName.CompareNoCase("Saki AOI") == 0)             return fileTypeSakiAOI;
   if (machineName.CompareNoCase("Hioki ICT") == 0)            return fileTypeHiokiICT;
   if (machineName.CompareNoCase("CyberOptics AOI") == 0)      return fileTypeCyberOpticsAOI;
   if (machineName.CompareNoCase("Omron AOI") == 0)            return fileTypeOmronAOI;
   if (machineName.CompareNoCase("Yestech AOI") == 0)          return fileTypeYestechAOI;

   return fileTypeUnknown;
}

//_____________________________________________________________________________
CDcaMachine::CDcaMachine(FileTypeTag fileType)
: m_eFileType(fileType)
, m_dftSolution(NULL)
{
}

CDcaMachine::~CDcaMachine()
{
}

CString CDcaMachine::GetName() const
{
   return getMachineName(m_eFileType);
}

CString CDcaMachine::GetMachineTypeString() const
{
   return getMachineTypeString(m_eFileType);
}

int CDcaMachine::GetMachineType() const
{
   return getMachineType(m_eFileType);
}

//_____________________________________________________________________________
CDcaMachineList::CDcaMachineList(FileStruct* pPCBFile)
{
}

CDcaMachineList::~CDcaMachineList()
{
}
