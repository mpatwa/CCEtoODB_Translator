// $Header: /CAMCAD/DcaLib/DcaFileType.cpp 7     6/17/07 9:01p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaFileType.h"
#include "DcaEnumIterator.h"

FileTypeTag intToFileTypeTag(long fileType)
{
   return intToFileTypeTag((int)fileType);
}

FileTypeTag intToFileTypeTag(int fileType)
{
   FileTypeTag retval = fileTypeUnknown;

   switch (fileType)
   {
   case fileTypeUnknown:                retval = fileTypeUnknown;              break;
   case fileTypeCamcadData:             retval = fileTypeCamcadData;           break;
   case fileTypeDxf:                    retval = fileTypeDxf;                  break;
   case fileTypeHpgl:                   retval = fileTypeHpgl;                 break;
   case fileTypeGerber:                 retval = fileTypeGerber;               break;
   case fileTypeAperture:               retval = fileTypeAperture;             break;
   case fileTypeUnicam:                 retval = fileTypeUnicam;               break;
   case fileTypeCdi:                    retval = fileTypeCdi;                  break;
   case fileTypePadsLayout:             retval = fileTypePadsLayout;           break;
   case fileTypePdifLayout:             retval = fileTypePdifLayout;           break;
   case fileTypePdifSchematic:          retval = fileTypePdifSchematic;        break;
   case fileTypeProtelLayout:           retval = fileTypeProtelLayout;         break;
   case fileTypeTangoLayout:            retval = fileTypeTangoLayout;          break;
   case fileTypeMentorLayout:           retval = fileTypeMentorLayout;         break;
   case fileTypeProtelSchematic:        retval = fileTypeProtelSchematic;      break;
   case fileTypePadsSchematic:          retval = fileTypePadsSchematic;        break;
   case fileTypeExcellon:               retval = fileTypeExcellon;             break;
   case fileTypeIges:                   retval = fileTypeIges;                 break;
   case fileTypeIpc:                    retval = fileTypeIpc;                  break;
   case fileTypeEdif:                   retval = fileTypeEdif;                 break;
   case fileTypeVb99Layout:             retval = fileTypeVb99Layout;           break;
   case fileTypeCadencePlot:            retval = fileTypeCadencePlot;          break;
   case fileTypeGencad:                 retval = fileTypeGencad;               break;
   case fileTypeBarco:                  retval = fileTypeBarco;                break;
   case fileTypeOrcadLtdLayout:         retval = fileTypeOrcadLtdLayout;       break;
   case fileTypeCasePlot:               retval = fileTypeCasePlot;             break;
   case fileTypeAllegroLayout:          retval = fileTypeAllegroLayout;        break;
   case fileTypeMentorNeutralLayout:    retval = fileTypeMentorNeutralLayout;  break;
   case fileTypeBom:                    retval = fileTypeBom;                  break;
   case fileTypeHp3070:                 retval = fileTypeHp3070;               break;
   case fileTypeCadifLayout:            retval = fileTypeCadifLayout;          break;
   case fileTypeAccelLayout:            retval = fileTypeAccelLayout;          break;
   case fileTypeEifLayout:              retval = fileTypeEifLayout;            break;
   case fileTypeQuadLayout:             retval = fileTypeQuadLayout;           break;
   case fileTypeHyperlynxLayout:        retval = fileTypeHyperlynxLayout;      break;
   case fileTypeCcmLayout:              retval = fileTypeCcmLayout;            break;
   case fileTypeThedaLayout:            retval = fileTypeThedaLayout;          break;
   case fileTypeHpegs:                  retval = fileTypeHpegs;                break;
   case fileTypeAutocadAdi:             retval = fileTypeAutocadAdi;           break;
   case fileTypeScicardsEncore:         retval = fileTypeScicardsEncore;       break;
   case fileType3DLayout:               retval = fileType3DLayout;             break;
   case fileTypeRdcDbframe:             retval = fileTypeRdcDbframe;           break;
   case fileTypeCr3000Layout:           retval = fileTypeCr3000Layout;         break;
   case fileTypeCctLayout:              retval = fileTypeCctLayout;            break;
   case fileTypeRtn:                    retval = fileTypeRtn;                  break;
   case fileTypeHp5dx:                  retval = fileTypeHp5dx;                break;
   case fileTypeGencam:                 retval = fileTypeGencam;               break;
   case fileTypeBnrLayout:              retval = fileTypeBnrLayout;            break;
   case fileTypeTakaya8:                retval = fileTypeTakaya8;              break;
   case fileTypeOdbPlusPlus:            retval = fileTypeOdbPlusPlus;          break;
   case fileTypeDmis:                   retval = fileTypeDmis;                 break;
   case fileTypeHughes:                 retval = fileTypeHughes;               break;
   case fileTypeViewdraw:               retval = fileTypeViewdraw;             break;
   case fileTypeFabMaster:              retval = fileTypeFabMaster;            break;
   case fileTypeTriMda:                 retval = fileTypeTriMda;               break;
   case fileTypeAgilentAoi:             retval = fileTypeAgilentAoi;           break;
   case fileTypeTeradyne7200:           retval = fileTypeTeradyne7200;         break;
   case fileTypeTeradyne7300:           retval = fileTypeTeradyne7300;         break;
   case fileTypeDdeLayout:              retval = fileTypeDdeLayout;            break;
   case fileTypeXmlSvg:                 retval = fileTypeXmlSvg;               break;
   case fileTypeMxt:                    retval = fileTypeMxt;                  break;
   case fileTypeCr5000Layout:           retval = fileTypeCr5000Layout;         break;
   case fileTypeFabNail:                retval = fileTypeFabNail;              break;
   case fileTypeUltiboard:              retval = fileTypeUltiboard;            break;
   case fileTypeCalayPrismaLayout:      retval = fileTypeCalayPrismaLayout;    break;
   case fileTypeUnidat:                 retval = fileTypeUnidat;               break;
   case fileTypeTriAoi:                 retval = fileTypeTriAoi;               break;
   case fileTypeHuntron:                retval = fileTypeHuntron;              break;
   case fileTypeTakaya9:                retval = fileTypeTakaya9;              break;
   case fileTypeSiemensQd:              retval = fileTypeSiemensQd;            break;
   case fileTypeAlcatelDocica:          retval = fileTypeAlcatelDocica;        break;
   case fileTypeAlcatelGmf:             retval = fileTypeAlcatelGmf;           break;
   case fileTypePadsLibrary:            retval = fileTypePadsLibrary;          break;
   case fileTypeSpea4040:               retval = fileTypeSpea4040;             break;
   case fileTypeIpl:                    retval = fileTypeIpl;                  break;
   case fileTypeAgilentSjPlx:           retval = fileTypeAgilentSjPlx;         break;
   case fileTypeTeradyne228xCkt:        retval = fileTypeTeradyne228xCkt;      break;
   case fileTypeTeradyne228xNav:        retval = fileTypeTeradyne228xNav;      break;
   case fileTypeAgilentI1000ATD:        retval = fileTypeAgilentI1000ATD;      break;
   case fileTypeTestronicsICT:          retval = fileTypeTestronicsICT;        break;
   case fileTypeFixture:                retval = fileTypeFixture;              break;
   case fileTypeViscomAoi:              retval = fileTypeViscomAoi;            break;
   case fileTypeAsymtek:                retval = fileTypeAsymtek;              break;
   case fileTypeNar:                    retval = fileTypeNar;                  break;
   case fileTypeAeroflexCb:             retval = fileTypeAeroflexCb;           break;
   case fileTypeMvp3DPasteAoi:          retval = fileTypeMvp3DPasteAoi;        break;
   case fileTypeMvpComponentAoi:        retval = fileTypeMvpComponentAoi;      break;
   case fileTypeOrbotech:               retval = fileTypeOrbotech;             break;
   case fileTypeScorpion:               retval = fileTypeScorpion;             break;
   case fileTypeRti:                    retval = fileTypeRti;                  break;
   case fileTypeFabMasterDevice:        retval = fileTypeFabMasterDevice;      break;
   case fileTypeSeicaParNod:            retval = fileTypeSeicaParNod;          break;
   case fileTypeDigitalTest:            retval = fileTypeDigitalTest;          break;
   case fileTypeBOMCSV:                 retval = fileTypeBOMCSV;               break;
   case fileTypeTeradyneSpectrum:       retval = fileTypeTeradyneSpectrum;     break;
   case fileTypeSonyAoi:                retval = fileTypeSonyAoi;              break;
   case fileTypeSiemensBoard:           retval = fileTypeSiemensBoard;         break;
   case fileTypeRoyonicsOut:            retval = fileTypeRoyonicsOut;          break;
   case fileTypeGenericCentroid:        retval = fileTypeGenericCentroid;      break;
   case fileTypeJuki:                   retval = fileTypeJuki;                 break;
   case fileTypeVITech:                 retval = fileTypeVITech;               break;
   case fileTypeSpectrumNailreq:        retval = fileTypeSpectrumNailreq;      break;
   case fileTypeAeroflexNail:           retval = fileTypeAeroflexNail;         break;
   case fileTypeKonradICT:              retval = fileTypeKonradICT;            break;
   case fileTypeAcculogic:              retval = fileTypeAcculogic;            break;
   case fileTypeTestabilityReport:      retval = fileTypeTestabilityReport;    break;
   case fileTypeSchematicNetlist:       retval = fileTypeSchematicNetlist;     break;
   case fileTypeCamcadNetlist:          retval = fileTypeCamcadNetlist;        break;
   case fileTypeVbKeyinNetlist:         retval = fileTypeVbKeyinNetlist;       break;
   case fileTypeBoardstationNetlist:    retval = fileTypeBoardstationNetlist;  break;
   case fileTypeViewlogicNetlist:       retval = fileTypeViewlogicNetlist;     break;
   case fileTypePadsPowerPcbNetlist:    retval = fileTypePadsPowerPcbNetlist;  break;
   case fileTypeCamcadUser:             retval = fileTypeCamcadUser;           break;
   case fileTypeMYDATAPCB:              retval = fileTypeMYDATAPCB;            break;
   case fileTypeCustomAssembly:         retval = fileTypeCustomAssembly;       break;
   case fileTypeFujiFlexa:              retval = fileTypeFujiFlexa;            break;
   case fileTypeExpedition:             retval = fileTypeExpedition;              break;
   case fileTypeFablink:                retval = fileTypeFablink;                 break;
   case fileTypeCellEditor:             retval = fileTypeCellEditor;              break;
   case fileTypeSymbolicFloorPlanner:   retval = fileTypeSymbolicFloorPlanner;    break;
   case fileTypeVisECAD:                retval = fileTypeVisECAD;                 break;
   case fileTypeConstraintEditorSystem: retval = fileTypeConstraintEditorSystem;  break;
   case fileTypeSakiAOI:                retval = fileTypeSakiAOI;                 break;
   case fileTypeHiokiICT:               retval = fileTypeHiokiICT;                break;
   case fileTypeCyberOpticsAOI:         retval = fileTypeCyberOpticsAOI;          break;
   case fileTypeOmronAOI:               retval = fileTypeOmronAOI;                break;
   case fileTypeYestechAOI:             retval = fileTypeYestechAOI;              break;
   case fileTypeBoardStationXE:         retval = fileTypeBoardStationXE;          break;
   case fileTypeBoardsim:               retval = fileTypeBoardsim;                break;
   case fileTypeLinesym:                retval = fileTypeLinesym;                 break;
   case fileTypeHLDRC:                  retval = fileTypeHLDRC;                   break;
   }

   return retval;
}

//_____________________________________________________________________________

CString fileTypeTagToString(FileTypeTag tagValue)
{
   // These strings are all squished together, no embedded spaces.

   CString retval("Unrecognized");

   switch (tagValue)
   {
   case fileTypeUnknown:                retval = "Unknown";              break;
   case fileTypeCamcadData:             retval = "CC_or_CCZ";			 break;
   case fileTypeDxf:                    retval = "Dxf";                  break;
   case fileTypeHpgl:                   retval = "Hpgl";                 break;
   case fileTypeGerber:                 retval = "Gerber";               break;
   case fileTypeAperture:               retval = "Aperture";             break;
   case fileTypeUnicam:                 retval = "Unicam";               break;
   case fileTypeCdi:                    retval = "Cdi";                  break;
   case fileTypePadsLayout:             retval = "PadsLayout";           break;
   case fileTypePdifLayout:             retval = "PdifLayout";           break;
   case fileTypePdifSchematic:          retval = "PdifSchematic";        break;
   case fileTypeProtelLayout:           retval = "ProtelLayout";         break;
   case fileTypeTangoLayout:            retval = "TangoLayout";          break;
   case fileTypeMentorLayout:           retval = "MentorLayout";         break;
   case fileTypeProtelSchematic:        retval = "ProtelSchematic";      break;
   case fileTypePadsSchematic:          retval = "PadsSchematic";        break;
   case fileTypeExcellon:               retval = "Excellon";             break;
   case fileTypeIges:                   retval = "Iges";                 break;
   case fileTypeIpc:                    retval = "Ipc";                  break;
   case fileTypeEdif:                   retval = "Edif";                 break;
   case fileTypeVb99Layout:             retval = "Vb99Layout";           break;
   case fileTypeCadencePlot:            retval = "CadencePlot";          break;
   case fileTypeGencad:                 retval = "Gencad";               break;
   case fileTypeBarco:                  retval = "Barco";                break;
   case fileTypeOrcadLtdLayout:         retval = "OrcadLtdLayout";       break;
   case fileTypeCasePlot:               retval = "CasePlot";             break;
   case fileTypeAllegroLayout:          retval = "AllegroLayout";        break;
   case fileTypeMentorNeutralLayout:    retval = "MentorNeutralLayout";  break;
   case fileTypeBom:                    retval = "Bom";                  break;
   case fileTypeHp3070:                 retval = "Hp3070";               break;
   case fileTypeCadifLayout:            retval = "CadifLayout";          break;
   case fileTypeAccelLayout:            retval = "AccelLayout";          break;
   case fileTypeEifLayout:              retval = "EifLayout";            break;
   case fileTypeQuadLayout:             retval = "QuadLayout";           break;
   case fileTypeHyperlynxLayout:        retval = "HyperlynxLayout";      break;
   case fileTypeCcmLayout:              retval = "CcmLayout";            break;
   case fileTypeThedaLayout:            retval = "ThedaLayout";          break;
   case fileTypeHpegs:                  retval = "Hpegs";                break;
   case fileTypeAutocadAdi:             retval = "AutocadAdi";           break;
   case fileTypeScicardsEncore:         retval = "ScicardsEncore";       break;
   case fileType3DLayout:               retval = "3DLayout";             break;
   case fileTypeRdcDbframe:             retval = "RdcDbframe";           break;
   case fileTypeCr3000Layout:           retval = "Cr3000Layout";         break;
   case fileTypeCctLayout:              retval = "CctLayout";            break;
   case fileTypeRtn:                    retval = "Rtn";                  break;
   case fileTypeHp5dx:                  retval = "Hp5dx";                break;
   case fileTypeGencam:                 retval = "Gencam";               break;
   case fileTypeBnrLayout:              retval = "BnrLayout";            break;
   case fileTypeTakaya8:                retval = "Takaya8";              break;
   case fileTypeOdbPlusPlus:            retval = "OdbPlusPlus";          break;
   case fileTypeDmis:                   retval = "Dmis";                 break;
   case fileTypeHughes:                 retval = "Hughes";               break;
   case fileTypeViewdraw:               retval = "Viewdraw";             break;
   case fileTypeFabMaster:              retval = "FabMaster";            break;
   case fileTypeTriMda:                 retval = "TriMda";               break;
   case fileTypeAgilentAoi:             retval = "AgilentAoi";           break;
   case fileTypeTeradyne7200:           retval = "Teradyne7200";         break;
   case fileTypeTeradyne7300:           retval = "Teradyne7300";         break;
   case fileTypeDdeLayout:              retval = "DdeLayout";            break;
   case fileTypeXmlSvg:                 retval = "XmlSvg";               break;
   case fileTypeMxt:                    retval = "Mxt";                  break;
   case fileTypeCr5000Layout:           retval = "Cr5000Layout";         break;
   case fileTypeFabNail:                retval = "FabNail";              break;
   case fileTypeUltiboard:              retval = "Ultiboard";            break;
   case fileTypeCalayPrismaLayout:      retval = "CalayPrismaLayout";    break;
   case fileTypeUnidat:                 retval = "Unidat";               break;
   case fileTypeTriAoi:                 retval = "TriAoi";               break;
   case fileTypeHuntron:                retval = "Huntron";              break;
   case fileTypeTakaya9:                retval = "Takaya9";              break;
   case fileTypeSiemensQd:              retval = "SiemensQd";            break;
   case fileTypeAlcatelDocica:          retval = "AlcatelDocica";        break;
   case fileTypeAlcatelGmf:             retval = "AlcatelGmf";           break;
   case fileTypePadsLibrary:            retval = "PadsLibrary";          break;
   case fileTypeSpea4040:               retval = "Spea4040";             break;
   case fileTypeIpl:                    retval = "Ipl";                  break;
   case fileTypeAgilentSjPlx:           retval = "AgilentSjPlx";         break;
   case fileTypeTeradyne228xCkt:        retval = "Teradyne228xCkt";      break;
   case fileTypeTeradyne228xNav:        retval = "Teradyne228xNav";      break;
   case fileTypeAgilentI1000ATD:        retval = "AgilentI1000ATD";      break;
   case fileTypeTestronicsICT:          retval = "TestronicsICT";        break;
   case fileTypeFixture:                retval = "Fixture";              break;
   case fileTypeViscomAoi:              retval = "ViscomAoi";            break;
   case fileTypeAsymtek:                retval = "Asymtek";              break;
   case fileTypeNar:                    retval = "Nar";                  break;
   case fileTypeAeroflexCb:             retval = "AeroflexCb";           break;
   case fileTypeMvp3DPasteAoi:          retval = "Mvp3DPasteAoi";        break;
   case fileTypeMvpComponentAoi:        retval = "MvpComponentAoi";      break;
   case fileTypeOrbotech:               retval = "Orbotech";             break;
   case fileTypeScorpion:               retval = "Scorpion";             break;
   case fileTypeRti:                    retval = "Rti";                  break;
   case fileTypeFabMasterDevice:        retval = "FabMasterDevice";      break;
   case fileTypeSeicaParNod:            retval = "SeicaParNod";          break;
   case fileTypeDigitalTest:            retval = "DigitalTest";          break;
   case fileTypeBOMCSV:                 retval = "BOMCSV";               break;
   case fileTypeTeradyneSpectrum:       retval = "TeradyneSpectrum";     break;
   case fileTypeSonyAoi:                retval = "SonyAoi";              break;
   case fileTypeSiemensBoard:           retval = "SiemensBoard";         break;
   case fileTypeRoyonicsOut:            retval = "RoyonicsOut";          break;
   case fileTypeGenericCentroid:        retval = "GenericCentroid";      break;
   case fileTypeJuki:                   retval = "Juki";                 break;
   case fileTypeVITech:                 retval = "VITech";               break;
   case fileTypeSpectrumNailreq:        retval = "SpectrumNailreq";      break;
   case fileTypeAeroflexNail:           retval = "AeroflexNail";         break;
   case fileTypeKonradICT:              retval = "KonradICT";            break;
   case fileTypeAcculogic:              retval = "Acculogic";            break;
   case fileTypeTestabilityReport:      retval = "TestabilityReport";    break;
   case fileTypeSchematicNetlist:       retval = "SchematicNetlist";     break;
   case fileTypeCamcadNetlist:          retval = "CamcadNetlist";        break;
   case fileTypeVbKeyinNetlist:         retval = "VbKeyinNetlist";       break;
   case fileTypeBoardstationNetlist:    retval = "BoardstationNetlist";  break;
   case fileTypeViewlogicNetlist:       retval = "ViewlogicNetlist";     break;
   case fileTypePadsPowerPcbNetlist:    retval = "PadsPowerPcbNetlist";  break;
   case fileTypeCamcadUser:             retval = "CamcadUser";           break;
   case fileTypeMYDATAPCB:              retval = "MYDATAPCB";            break;
   case fileTypeCustomAssembly:         retval = "CustomAssembly";       break;
   case fileTypeFujiFlexa:              retval = "FujiFlexa";            break;
   case fileTypeExpedition:             retval = "Expedition";           break;
   case fileTypeFablink:                retval = "Fablink";              break;
   case fileTypeCellEditor:             retval = "CellEditor";           break;
   case fileTypeSymbolicFloorPlanner:   retval = "SymbolicFloorPlanner"; break;
   case fileTypeVisECAD:                retval = "visECAD";              break;
   case fileTypeConstraintEditorSystem: retval = "ConstraintEditorSystem"; break;
   case fileTypeSakiAOI:                retval = "SakiAOI";              break;
   case fileTypeHiokiICT:               retval = "HiokiICT";             break;
   case fileTypeCyberOpticsAOI:         retval = "CyberOpticsAOI";       break;
   case fileTypeOmronAOI:               retval = "OmronAOI";             break;
   case fileTypeYestechAOI:             retval = "YestechAOI";           break;
   case fileTypeBoardStationXE:         retval = "BoardStationXE";       break;
   case fileTypeBoardsim:               retval = "Boardsim";             break;
   case fileTypeLinesym:                retval = "Linesim";              break;
   case fileTypeHLDRC:                  retval = "HLDRC";                break;
   }

   return retval;
}

CString fileTypeTagToPrettyString(FileTypeTag tagValue)
{
   // These strings are prettier than above, can have spaces. Where the ones above tend
   // to be named after the enum symbol, these tend to be named after the original source
   // CAD sytem, company name, format, etc.

   CString retval("Unrecognized");

   switch (tagValue)
   {
   case fileTypeUnknown:                retval = "UNKNOWN CAD Source";        break;
   case fileTypeCamcadData:             retval = "CAMCAD CC or CCZ";          break;
   case fileTypeDxf:                    retval = "AutoCAD DXF";               break;
   case fileTypeHpgl:                   retval = "HPGL";                      break;
   case fileTypeGerber:                 retval = "Gerber";                    break;
   case fileTypeAperture:               retval = "Aperture";                  break;
   case fileTypeUnicam:                 retval = "Unicam";                    break;
   case fileTypeCdi:                    retval = "CADSTAR CDI";               break;
   case fileTypePadsLayout:             retval = "PADS PCB";                  break;
   case fileTypePdifLayout:             retval = "PDIF PCB";                  break;
   case fileTypePdifSchematic:          retval = "PDIF Schematic";            break;
   case fileTypeProtelLayout:           retval = "Protel PCB";                break;
   case fileTypeTangoLayout:            retval = "Tango PCB";                 break;
   case fileTypeMentorLayout:           retval = "Mentor Boardstation";       break;
   case fileTypeProtelSchematic:        retval = "Protel Schematic";          break;
   case fileTypePadsSchematic:          retval = "PADS Schematic";            break;
   case fileTypeExcellon:               retval = "Excellon";                  break;
   case fileTypeIges:                   retval = "IGES";                      break;
   case fileTypeIpc:                    retval = "IPC 350/356";               break;
   case fileTypeEdif:                   retval = "EDIF";                      break;
   case fileTypeVb99Layout:             retval = "VB ASCII";                  break;
   case fileTypeCadencePlot:            retval = "Cadence IPF";               break;
   case fileTypeGencad:                 retval = "Gencad";                    break;
   case fileTypeBarco:                  retval = "Barco";                     break;
   case fileTypeOrcadLtdLayout:         retval = "ORCAD PCB";                 break;
   case fileTypeCasePlot:               retval = "Case Plot";                 break;
   case fileTypeAllegroLayout:          retval = "CADENCE ALLEGRO";           break;
   case fileTypeMentorNeutralLayout:    retval = "Mentor Neutral";            break;
   case fileTypeBom:                    retval = "BOM";                       break;
   case fileTypeHp3070:                 retval = "HP3070";                    break;
   case fileTypeCadifLayout:            retval = "Redac CADIF";               break;
   case fileTypeAccelLayout:            retval = "Accel EDA";                 break;
   case fileTypeEifLayout:              retval = "VB EIF";                    break;
   case fileTypeQuadLayout:             retval = "Quad PCB";                  break;
   case fileTypeHyperlynxLayout:        retval = "Hyperlynx";                 break;
   case fileTypeCcmLayout:              retval = "CCM";                       break;
   case fileTypeThedaLayout:            retval = "Theda";                     break;
   case fileTypeHpegs:                  retval = "HP EGS";                    break;
   case fileTypeAutocadAdi:             retval = "Autocad ADI";               break;
   case fileTypeScicardsEncore:         retval = "Scicards CII";              break;
   case fileType3DLayout:               retval = "IDF V2.0";                  break;
   case fileTypeRdcDbframe:             retval = "RDC DB FRAME";              break;
   case fileTypeCr3000Layout:           retval = "CR 3000";                   break;
   case fileTypeCctLayout:              retval = "CCT DESIGN";                break;
   case fileTypeRtn:                    retval = "ODB RTN";                   break;
   case fileTypeHp5dx:                  retval = "HP 5DX";                    break;
   case fileTypeGencam:                 retval = "Gencam";                    break;
   case fileTypeBnrLayout:              retval = "UNICAD BNR";                break;
   case fileTypeTakaya8:                retval = "TAKAYA .CA8";               break;
   case fileTypeOdbPlusPlus:            retval = "ODB++";                     break;
   case fileTypeDmis:                   retval = "DMIS";                      break;
   case fileTypeHughes:                 retval = "Hughes 1";                  break;
   case fileTypeViewdraw:               retval = "Viewdraw";                  break;
   case fileTypeFabMaster:              retval = "FABMaster";                 break;
   case fileTypeTriMda:                 retval = "TRI-MDA";                   break;
   case fileTypeAgilentAoi:             retval = "Agilent AOI";               break;
   case fileTypeTeradyne7200:           retval = "Teradyne 7200 IPL";         break;
   case fileTypeTeradyne7300:           retval = "Teradyne 7300 IPL";         break;
   case fileTypeDdeLayout:              retval = "DDE SUPERMAX";              break;
   case fileTypeXmlSvg:                 retval = "XML/SVG";                   break;
   case fileTypeMxt:                    retval = "MXT";                       break;
   case fileTypeCr5000Layout:           retval = "CR 5000";                   break;
   case fileTypeFabNail:                retval = "FABMaster Nail";            break;
   case fileTypeUltiboard:              retval = "Ultiboard";                 break;
   case fileTypeCalayPrismaLayout:      retval = "Calay Prisma";              break;
   case fileTypeUnidat:                 retval = "Unidat";                    break;
   case fileTypeTriAoi:                 retval = "TRI-AOI";                   break;
   case fileTypeHuntron:                retval = "Huntron";                   break;
   case fileTypeTakaya9:                retval = "TAKAYA .CA9";               break;
   case fileTypeSiemensQd:              retval = "Siemens QD";                break;
   case fileTypeAlcatelDocica:          retval = "Alcatel Docica";            break;
   case fileTypeAlcatelGmf:             retval = "Alcatel GMF";               break;
   case fileTypePadsLibrary:            retval = "PADS Library";              break;
   case fileTypeSpea4040:               retval = "Spea 4040";                 break;
   case fileTypeIpl:                    retval = "Teradyne Z18xx IPL";        break;
   case fileTypeAgilentSjPlx:           retval = "Agilent SJ PLX";            break;
   case fileTypeTeradyne228xCkt:        retval = "Teradyne (GenRad)228X/TestStation CKT & NAV";      break;
   case fileTypeTeradyne228xNav:        retval = "Teradyne228xNav";           break;
   case fileTypeAgilentI1000ATD:        retval = "Agilent i1000 ATD";         break;
   case fileTypeTestronicsICT:          retval = "Testronics ICT";            break;
   case fileTypeFixture:                retval = "Fixture File";              break;
   case fileTypeViscomAoi:              retval = "Viscom AOI";                break;
   case fileTypeAsymtek:                retval = "Asymtek";                   break;
   case fileTypeNar:                    retval = "Nail Assignment Report";    break;
   case fileTypeAeroflexCb:             retval = "AeroflexCB";                break;
   case fileTypeMvp3DPasteAoi:          retval = "MVP 3D Paste AOI";          break;
   case fileTypeMvpComponentAoi:        retval = "MVP Component AOI";         break;
   case fileTypeOrbotech:               retval = "Orbotech";                  break;
   case fileTypeScorpion:               retval = "Scorpion";                  break;
   case fileTypeRti:                    retval = "RTI";                       break;
   case fileTypeFabMasterDevice:        retval = "FABMaster Device";          break;
   case fileTypeSeicaParNod:            retval = "Seica PAR NOD";             break;
   case fileTypeDigitalTest:            retval = "DigitalTest";               break;
   case fileTypeBOMCSV:                 retval = "Attribute/BOM CSV Read";    break;
   case fileTypeTeradyneSpectrum:       retval = "Teradyne Spectrum";         break;
   case fileTypeSonyAoi:                retval = "Sony AOI";                  break;
   case fileTypeSiemensBoard:           retval = "Siemens Board";             break;
   case fileTypeRoyonicsOut:            retval = "Royonics 500";              break;
   case fileTypeGenericCentroid:        retval = "Generic Centroid";          break;
   case fileTypeJuki:                   retval = "Juki";                      break;
   case fileTypeVITech:                 retval = "VI Technology";             break;
   case fileTypeSpectrumNailreq:        retval = "Spectrum Nailreq";          break;
   case fileTypeAeroflexNail:           retval = "Aeroflex Nail";             break;
   case fileTypeKonradICT:              retval = "Konrad ICT";                break;
   case fileTypeAcculogic:              retval = "Acculogic";                 break;
   case fileTypeTestabilityReport:      retval = "Testability Report";        break;
   case fileTypeSchematicNetlist:       retval = "Schematic Netlist";         break;
   case fileTypeCamcadNetlist:          retval = "Camcad Netlist";            break;
   case fileTypeVbKeyinNetlist:         retval = "VB Keyin Netlist";          break;
   case fileTypeBoardstationNetlist:    retval = "Boardstation Netlist";      break;
   case fileTypeViewlogicNetlist:       retval = "Viewlogic Netlist";         break;
   case fileTypePadsPowerPcbNetlist:    retval = "PADS PowerPCB Netlist";     break;
   case fileTypeCamcadUser:             retval = "Camcad User";               break;
   case fileTypeMYDATAPCB:              retval = "MYDATA PCB";                break;
   case fileTypeCustomAssembly:         retval = "Custom Assembly";           break;
   case fileTypeFujiFlexa:              retval = "Fuji Flexa";                break;
   case fileTypeExpedition:             retval = "Expedition";                break;
   case fileTypeFablink:                retval = "Fablink";                   break;
   case fileTypeCellEditor:             retval = "Cell Editor";               break;
   case fileTypeSymbolicFloorPlanner:   retval = "Symbolic Floor Planner";    break;
   case fileTypeVisECAD:                retval = "visECAD";                   break;
   case fileTypeConstraintEditorSystem: retval = "Constraint Editor System";  break;
   case fileTypeSakiAOI:                retval = "Saki AOI";                  break;
   case fileTypeHiokiICT:               retval = "Hioki ICT";                 break;
   case fileTypeCyberOpticsAOI:         retval = "CyberOptics AOI";           break;
   case fileTypeOmronAOI:               retval = "Omron AOI";                 break;
   case fileTypeYestechAOI:             retval = "Yestech AOI";               break;
   case fileTypeBoardStationXE:         retval = "BoardStation XE";           break;
   case fileTypeBoardsim:               retval = "Boardsim";                  break;
   case fileTypeLinesym:                retval = "Linesim";                   break;
   case fileTypeHLDRC:                  retval = "HyperLynx DRC";             break;
   }

   return retval;
}

FileTypeTag stringToFileTypeTag(CString someString)
{
   // Try the pretty strings first.
   for (EnumIterator(FileTypeTag, filetypeIterator); filetypeIterator.hasNext();)
   {
      FileTypeTag ft = filetypeIterator.getNext();
      CString ftstr( fileTypeTagToPrettyString(ft) );
      if (ftstr.CompareNoCase(someString) == 0)
         return ft;
   }

   // Try the squished strings.
   for (EnumIterator(FileTypeTag, filetypeIterator); filetypeIterator.hasNext();)
   {
      FileTypeTag ft = filetypeIterator.getNext();
      CString ftstr( fileTypeTagToString(ft) );
      if (ftstr.CompareNoCase(someString) == 0)
         return ft;
   }

   return fileTypeUnknown;
}