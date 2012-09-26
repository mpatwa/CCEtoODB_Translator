// $Header: /CAMCAD/4.6/Port.h 60    5/01/07 3:40p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#if !defined(__Port_h__)
#define __Port_h__

#pragma once

#define FIT_PAGE     1
#define FIT_IMAGE    2
#define PLACE_ORIGIN 3
#define PLACE_MOUSE  4

//void InitFilenameFilters(int FileType, SettingsStruct *settings, CString *Filter, CString *DefFileName, CString *DefExt);
//void InitImportFormat(FormatStruct *format, SettingsStruct *settings);
//BOOL GetImportFormat(CCEtoODBDoc *doc, FormatStruct *format, SettingsStruct *settings, CString *formatString);
void InitExportFormat(FormatStruct *format, SettingsStruct *settings);
BOOL GetExportFormat(FormatStruct *format, SettingsStruct *settings, CCEtoODBDoc *doc);
void WriteImportFormatString(FormatStruct *format, CString *formatString);
//int SortImportFiles(FormatStruct *format);

// READERS

void UniversalBOMRead(CCEtoODBDoc *doc, const char *bomFile);

void ReadACCEL             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadAlcatelDocica     (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadAlcatelGMF        (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadAllegro           (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filenum, int tot_tilenum);
void ReadAgilentSjPlx      (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);    
void ReadAutocadADI        (const char *fileNames,                  FormatStruct *format, double scaleUnitsFactor);
void ReadBarco             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadBNR               (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadCADENCE_Plot      (const char *fileNames,                  FormatStruct *format, double scaleUnitsFactor);
void ReadCADIF             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadCASE_Plot         (const char *fileNames,                  FormatStruct *format, double scaleUnitsFactor);
//void ReadCCT             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadScicards          (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadCR3000            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadCR5000            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadCR5000new         (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadCXF               (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadDDE               (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadDMIS              (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadDXF               (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, double scaleUnitsFactor);
int ReadEDIF					(const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadEIF               (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filecnt, int tot_filecnt);
void ReadExcellon          (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadFabmasterNail     (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadFATF              (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadGENCAD            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadGenCAM            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadGerber            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filenum, int tot_filenum);
void ReadHP3070            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filecnt, int tot_filecnt);
void ReadHP5DX             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filecnt, int tot_filecnt);
void ReadHpEGS             (const char *fileNames,                  FormatStruct *format, double scaleUnitsFactor, int PageUnits);
void ReadHPGL              (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadHughes            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filenum, int tot_filenum);
void ReadIGES              (const char *fileNames,                  FormatStruct *format, double scaleUnitsFactor, int PageUnits);
void ReadIPC               (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
/* discontinued 9-Mar-01 
void ReadKongsberg         (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filenum, int tot_filenum);
*/
int  ReadMentorBoardStation(const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int cur_filecnt, int tot_filecnt);
void ReadMentorNeutral     (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int currentFileNum, int fileCount);
void ReadODBpp             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);    
void ReadOrcad             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadPADS              (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadPADSLib           (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int pageunits);
void ReadPDIF              (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits, int Layout); // layout = 1, schem = 0
void ReadProtelPFW         (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadProtelSchematic   (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadSpectrumNailreq   (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format);
void ReadSchematicNetlist  (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format, int currentFile, int fileCount);
void ReadAeroflexNail      (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format);
void ReadAeroflexNailWir   (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format);
void ReadTHEDA             (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadULTIBOARD         (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadUnicamPdw         (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadUNIDAT            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadVB99              (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadDDE_V9            (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, int PageUnits);
void ReadVIEWDRAW          (const char *fileNames, CCEtoODBDoc *Doc, FormatStruct *format, double scaleUnitsFactor);
void ReadNailAssignement   (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format);
void ReadFabMasterDevice   (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format);
void ReadBOMCSV			   (const char *fileName , CCEtoODBDoc *Doc, FormatStruct *format);
void ReadGenericCentroid	(const CString fileName , CCEtoODBDoc& camcadDoc, FormatStruct& format);

BOOL Is_DDE_V9(const char *path_buffer);

// WRITERS

void Accel_WriteFiles             (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits);
void Agilent_AOI_WriteFiles       (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void ALLEGRO_WriteFiles           (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, 
                                   double xmin, double xmax, double ymin, double ymax, const char *checkfile, const char *rulfile);
void BOM_WriteFiles               (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format);
void Barco_WriteFiles             (CCEtoODBDoc *Doc, FormatStruct *format);
/* retired 27-Nov-98 
void CADL_WriteFiles              (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int explodeStyle);
*/
void CADIF_WriteFiles             (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units,
                                   double xmin, double ymin, double xmax, double ymax);
void CCM_WriteFiles               (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void CDI_WriteFiles               (const char *Filename, CCEtoODBDoc *Doc, FormatStruct *Format, double xmin, double xmax, double ymin, double ymax);
void DDE_WriteFiles               (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void DMIS_WriteFiles              (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int pageunits);
void DXF_WriteFiles               (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, double scale);
void EXCELLON_WriteFiles          (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int page_units, double UnitsFactor); 
void FABMASTER_WriteFiles         (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double UnitsFactor, int page_units);
void GENCAD_WriteFiles            (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, BOOL run_silent = FALSE);
void GENCAM_WriteFiles            (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void Gerb_WriteFiles              (CCEtoODBDoc *Doc, FormatStruct *format);
void TRI_MDA_WriteFiles           (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void HP3070_WriteFiles            (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, 
                                   double UnitsFactor,const char *ruleFile, const char *boardDefaultFile, const char *nameCheckFile, const char *hp3070LogFile);
void HP5DX_WriteFiles             (const char *panelname, CCEtoODBDoc *Doc, FormatStruct *Format,int page_units,  double UnitsFactor, const char *directoryname);
void HPGL_WriteFiles              (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, double hpscale, double mmscale);
void Huntron_WriteFiles           (const char *filename, CCEtoODBDoc *Doc);
void HYPERLYNX_WriteFiles         (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, double UnitsFactor, const char *includeFile);
void IDF_Layout_WriteFiles        (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, double UnitsFactor, const char *includeFile);
void IGES_WriteFiles              (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int explodeStyle);
void IPC_WriteFiles               (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format);
void IPL_WriteFiles                (const char *filename, CCEtoODBDoc &Doc, FormatStruct *format, double UnitsFactor);
void MENTORBoardStation_WriteFiles  (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void MNEUTRAL_WriteFiles           (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void MVP_AOI_Comp_WriteFiles      (const char *fullPath, CCEtoODBDoc *Doc, FormatStruct *Format);
void MVP_AOI_Paste_WriteFiles     (const char *fullPath, CCEtoODBDoc *Doc);
void ODB_WriteFiles               (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int pageUnits);
void OGP_WriteFiles               (const char *filename, CCEtoODBDoc *doc);
/* retired 25-Nov-98 
void ORCAD386_WriteFiles          (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double UnitsFactor);
*/
void ORCADLTD_WriteFiles          (const char *fileName, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void PADS_WriteFiles              (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format, int page_units);
void PADS_WriteSchematicFiles     (const char *filename, CCEtoODBDoc *Doc, FormatStruct *format);
void PDIF_Layout_WriteFiles       (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double UnitsFactor);
void PDIF_Schematic_WriteFiles    (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double UnitsFactor);
void Protel_WriteFiles            (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double scale);
void Protel_WriteSchematicFiles   (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double scale);
void QUAD_WriteFiles              (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, double UnitsFactor, const char *includeFile);
void RDC_WriteFiles               (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, double UnitsFactor, const char *includeFile);
void SPEA4040_WriteFile           (CString     fileName, CCEtoODBDoc *doc, FormatStruct *format, int pageUnits);
void Siemens_WriteFiles           (CCEtoODBDoc *Doc);
void SOLIDWORKS_Layout_WriteFiles (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, double UnitsFactor, const char *includeFile);
void Sony_AOI_WriteFiles          (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void TAKAYA_WriteFiles            (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, BOOL Version9);
void TANGO_WriteFiles             (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double UnitsFactor);
void TERADYNE_7300_WriteFiles     (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void TERADYNE_7200_WriteFiles     (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void TESTPIN_WriteFiles           (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, double UnitsFactor);
void THEDA_WriteFiles             (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void TRI_AOI_WriteFiles           (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units);
void UNICAM_WriteFiles            (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, double UnitsFactor, int page_units);
void VB99_WriteFiles              (const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format, int page_units, 
                                   double xmin, double xmax, double ymin, double ymax, const char *checkfile, const char *rulfile);
void Teradyne228xCKT_WriteFiles   (CString     filename, CCEtoODBDoc &doc, FormatStruct *format, int page_units);
void Teradyne228xNAV_WriteFiles   (CString     filename, CCEtoODBDoc *doc, FormatStruct *format, int page_units);
void Fixture_WriteFiles           (CString     filename, CCEtoODBDoc &doc, FormatStruct *format, int page_units);
void ViscomAOI_WriteFiles         (CString     pathName, CCEtoODBDoc *doc, FormatStruct *format, int pageUnits);
void Asymtek_WriteFiles           (CString     pathName, CCEtoODBDoc &doc, FormatStruct *format, int pageUnits);
void Aeroflex_WriteFile           (CString     fileName, CCEtoODBDoc &doc, FormatStruct *format);
void Orbotech_WriteFile           (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void Scorpion_WriteFile           (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void RTI_WriteFile                (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void SeicaParNod_WriteFile        (const CString& filename, CCEtoODBDoc &doc, FormatStruct *format);
void DigitalTest_WriteFile        (const CString& filename, CCEtoODBDoc &doc, FormatStruct *format);
void KonradICT_WriteFile          (const CString& filename, CCEtoODBDoc &doc, FormatStruct *format);
void TestronicsICT_WriteFile      (const CString& filename, CCEtoODBDoc &doc, FormatStruct *format);
void Acculogic_WriteFile          (const CString& filename, CCEtoODBDoc &doc, FormatStruct *format);
void TeradyneSpectrum_WriteFiles  (const CString  filename, CCEtoODBDoc *pDoc, FormatStruct *format);
void SiemensBoard_WriteFile       (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void Royonics_WriteFile			    (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void Juki_WriteFile			       (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void TestabilityReport_WriteFile  (const CString& filename, CCEtoODBDoc &doc, FormatStruct *format);
void FABMasterFATF_WriteFiles     (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void Agilent_i1000_WriteFiles     (CString     filename, CCEtoODBDoc &doc, FormatStruct *format); 
void MyData_WriteFile             (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void CustomAssembly_WriteFile     (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void FujiFlexa_WriteFile          (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void SakiAOI_WriteFiles           (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void YestechAOI_WriteFiles        (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void Hioki_WriteFiles             (CString     filename, CCEtoODBDoc &doc, FormatStruct *Format);
void OmronAOI_WriteFile           (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);
void CyberOpticsAOI_WriteFile     (CString     fileName, CCEtoODBDoc &doc, FormatStruct *format);

//void VIT_WriteFile				  (CString     filename, CCEtoODBDoc &doc, FormatStruct *format);

void WriteGAP1                    (CCEtoODBDoc *doc, const char *FileName, int pageUnits);

#endif