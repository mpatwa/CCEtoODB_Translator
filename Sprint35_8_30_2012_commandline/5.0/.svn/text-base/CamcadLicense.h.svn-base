
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// CAMCADLICENSE.H

#if ! defined (__CamcadLicense_h__)
#define __CamcadLicense_h__

#pragma once

#include "DbUtil.h"
#include "TypedContainer.h"
#include "Lic.h"
#include "License.h"

// Are these license no longer use?

// Kill List for 4.6
// =================
// LIC_BNRIN_S                 16  "Unicad/BNR Read"
// LIC_CCTIN_S                 22  "CCT Design File Read"
// LIC_HP3070IN_S              41  "Agilent 3070 Read"
// LIC_PADSLIBIN_S             115 "Pads Library (.d) Read"
// LIC_PDIFSCHIN_S             51  "PCAD PDIF Schematic Read"
// LIC_PFWSCHIN_S              52  "Protel PFW Schematic Read"
// LIC_ULTIBOARDIN_S           55  "ULTIBOARD (.edf) Layout Read"
// LIC_VIEWDRAWIN_S            58  "VIEWDRAW Read"
// LIC_CADIFOUT_S              66  "Redac CADIF (.paf) Layout Write"
// LIC_FATFOUT_S               77  "FABMASTER FATF Write"
// LIC_MENTNEUTOUT_S           90  "Mentor Neutral File Write"
// LIC_ODBPPOUT_S              92  "ODB++ Write"
// LIC_RDCOUT_S                103 "RDC-Dbframe Write"
// LIC_TAKAYA8OUT_S            106 "TAKAYA .CA8 Write"
// LIC_THEDAOUT_S              110 "THEDA (.tl) Layout, Panel Write"


enum CamcadProductIdTag
{
   camcadProductMinIndex                           = 0,
   camcadProductUnknown                            = -1,

// Kill List for 4.6
// =================
   camcadProductPadsLibraryRead                    = 115,      // "Pads Library (.d) Read"
   camcadProductTakayaCa8Write                     = 106,      // "TAKAYA .CA8 Write"// LIC_ORCADLTDOUT_S           93  "Orcad Layout Plus Write"

   // Operations
   camcadProductPrint                              = 0,
   camcadProductQuery                              = 1,
   camcadProductCEdit                              = 2,     // 4.5
   camcadProductPEdit                              = 3,
   camcadProductApiLevel1                          = 4,
   camcadProductApiLevel2                          = 5,
   camcadProductApiLevel3                          = 6,
   camcadProductApiLevel4                          = 7,
   camcadProductRedline                            = 8,
   camcadProductDft                                = 9,
   camcadProductDfm                                = 10,
   camcadProductRealPartPackage                    = 116,
   camcadProductRealPartDevice                     = 117,
   camcadProductStencilGenerator                   = 119,
   camcadProductGerberEducator                     = 122,
   camcadProductStencilStepper                     = 138,
   camcadProductFixtureReuse                       = 140,

   // Readers
   camcadProductAccelPcadRead                      = 11,       // "ACCEL, PCAD200x (.pcb) Layout Read"
   camcadProductCadenceAllegroExtractRead          = 12,       // "CADENCE Allegro (Extract) Read"
   camcadProductApertureRead                       = 13,       // "Aperture Read"
   camcadProductAutocadAdiRead                     = 14,       // "Autocad ADI Read"
   camcadProductBarcoDpfRead                       = 15,       // "Barco (.dpf) Read"
   camcadProductAttributeBomRead                   = 17,       // "Attribute/BOM Read"
   camcadProductCadenceAllegroIpfRead              = 18,       // "CADENCE Allegro IPF Read"
   camcadProductCadStarCadifRead                   = 19,       // "Redac CADIF (.paf) Layout Read"
   camcadProductCasePlotRead                       = 20,       // "CASE Plot Read"
   camcadProductCalayPrismaRead                    = 21,       // "CALAY PRISMA Layout Read"
   camcadProductSciCardsRead                       = 23,       // "Scicards/Encore (CII) Layout Read"
   camcadProductZukenCR3000Read                    = 24,       // "Zuken PWS (CR3000/CR5000) Layout Read"
   camcadProductZukenCR5000Read                    = 25,       // "Zuken CR5000 Board Designer Read"
   camcadProductSupermaxDdeRead                    = 26,       // "DDE Layout Read"
   camcadProductDxfRead                            = 27,       // "DXF Read"
   camcadProductDmisV3Read                         = 28,       // "DMIS V3 Read"
   camcadProductUnidatRead                         = 29,       // "UNIDAT Read"
   camcadProductEdifRead                           = 30,       // "EDIF V 200...400 Read"
   camcadProductVeribestEifRead                    = 31,       // "VERIBEST EIF Layout Read"
   camcadProductExcellonDrillRouteRead             = 32,       // "Excellon Drill&Route Read"
   camcadProductFabmasterFatfRead                  = 33,       // "FABMASTER FATF Read"
   camcadProductFabmasterNailRead                  = 34,       // "FABMASTER NAIL Read"
   camcadProductGenCadRead                         = 35,       // "GENCAD Read"
   camcadProductGenCamRead                         = 36,       // "GenCAM V1.5 Read"
   camcadProductGerberRead                         = 37,       // "Gerber Read"
   camcadProductHughesRaytheonRead                 = 38,       // "Hughes/Raytheon Read"
   camcadProductHpEgsRead                          = 39,       // "HP EGS Archive Read"
   camcadProductHpglRead                           = 40,       // "HPGL/HPGL2 Read"
   camcadProductAgilent5DxRead                     = 42,       // "Agilent 5DX Read"
   camcadProductIpc35xRead                         = 43,       // "IPC350/356/356A Read"
   camcadProductIgesRead                           = 44,       // "IGES Read"
   camcadProductMentorNeutralRead                  = 45,       // "Mentor Neutral File Read"
   camcadProductBoardStationRead                   = 46,       // "Mentor Board Station V8 Read"
   camcadProductOdbPlusPlusRead                    = 47,       // "ODB++ Read"
   camcadProductOrcadRead                          = 48,       // "Orcad (.min) Layout Plus Read"
   camcadProductPadsRead                           = 49,       // "PADS (.asc) Layout Read"
   camcadProductPcadPdifRead                       = 50,       // "PCAD PDIF Layout Read"
   camcadProductProtelPcbRead                      = 53,       // "Protel PCB ASCII Read"
   camcadProductThedaRead                          = 54,       // "THEDA (.tl) Layout, Panel Read"
   camcadProductUnicamPdwRead                      = 56,       // "UNICAM PDW Read"
   camcadProductExpeditionAsciiRead                = 57,       // "VB ASCII (.hkp) Layout Read"
   camcadProductAlcatelDociaRead                   = 113,      // "Alcatel Docica Read"
   camcadProductAlcatelGmfRead                     = 114,      // "Alcatel GMF+ Read"
   camcadProductAgilentSjPlxRead                   = 120,      // "Agilent SJ PLX Centroid Read"
   camcadProductNailAssignmentRead                 = 127,      // "Nail Assignment Report Read (.NAR)"
   camcadProductFabmasterDeviceRead                = 134,      // "FabMaster Device.asc Read"
   camcadProductAttributeBomDelimitedRead          = 137,      // "Attribute/BOM Delimited Import"
   camcadProductGenericCentroidRead                = 144,      // "Generic Centroid Read"
   camcadProductSpectrumNailRead                   = 147,      // "Spectrum Nailreq Read"
   camcadProductAeroflexNailRead                   = 154,      // "Aeroflex Nail Read"
   camcadProductSchematicNetlistRead               = 157,      // "Schematic Net List Read"
   camcadProductCpCceRead                          = 168,      // "CCE Read"

   // Writers
   camcadProductIdfV2Write                         = 59,       // "IDF V2.0 Layout Write"
   camcadProductAccelPcadWrite                     = 60,       // "ACCEL Write, PCAD200x (.pcb) Layout"
   camcadProductCadenceAllegroScriptWrite          = 61,       // "CADENCE Allegro (Script) Write"
   camcadProductAgilentAoiWrite                    = 62,       // "Agilent AOI Write"
   camcadProductApertureWrite                      = 63,       // "Aperture Write"
   camcadProductBarcoPdfWrite                      = 64,       // "Barco (.dpf) Write"
   camcadProductAttributeBomWrite                  = 65,       // "Attribute/BOM Write"
   camcadProductGenCcmWrite                        = 67,       // "CCM Format Write"
   camcadProductCadstarCdiWrite                    = 68,       // "CADSTAR-DOS CDI Graphic Write"
   camcadProductZukenCr3000Write                   = 69,       // "Zuken CR3000 PWS Layout Write"
   camcadProductDdeWrite                           = 70,       // "DDE Layout Write"
   camcadProductDmisV3Write                        = 71,       // "DMIS V3 Write"
   camcadProductDxfWrite                           = 72,       // "DXF Write"
   camcadProductEdif200Write                       = 73,       // "EDIF 200 Graphic Write"
   camcadProductVeribestEifWrite                   = 74,       // "VERIBEST EIF Layout Write"
   camcadProductVbAsciiWrite                       = 75,       // "VB ASCII (.hkp) Layout Write"
   camcadProductExcellonDrillWrite                 = 76,       // "Excellon Drill&Route Write"
   camcadProductGenCadWrite                        = 78,       // "GENCAD Write"
   camcadProductGenCamWrite                        = 79,       // "GenCAM V1.5 Write"
   camcadProductGerberWrite                        = 80,       // "Gerber Write"
   camcadProductAgilent3070Write                   = 81,       // "Agilent 3070 Write"
   camcadProductAgilent5DxWrite                    = 82,       // "Agilent 5DX Write"
   camcadProductHpglWrite                          = 83,       // "HPGL/HPGL2 Write"
   camcadProductHuntronWrite                       = 84,       // "HAF Write"
   camcadProductHyperlynxWrite                     = 85,       // "HyperLynx BoardSim Write"
   camcadProductIgesWrite                          = 86,       // "IGES Write"
   camcadProductTriMdaWrite                        = 87,       // "TRI-MDA Write"
   camcadProductIpc35xWrite                        = 88,       // "IPC350/356/356A Write"
   camcadProductBoardStationWrite                  = 89,       // "Mentor Board Station V8 Write"
   camcadProductMxtWrite                           = 91,       // "MXT Write"
   camcadProductOrcadLayoutWrite                   = 93,       // "Orcad Layout Plus Write"
   camcadProductPadsWrite                          = 94,       // "Pads Power V3 Graphic Write"
   camcadProductPadsSchematicWrite                 = 95,       // "Pads Logic Graphic Write"
   camcadProductPcadPdifWrite                      = 96,       // "PCAD PDIF Layout Graphic Write"
   camcadProductPdifSchematicWrite                 = 97,       // "PDIF Schematic Graphic Write"
   camcadProductProtelPfwLayoutWrite               = 98,       // "Protel PFW V2.8 Layout Write"
   camcadProductProtelPfwSchematicWrite            = 99,       // "Protel PFW Schematic Write"
   camcadProductQuadWrite                          = 100,      // "QUAD Write"
   camcadProductRtnRev9Write                       = 101,      // "RTN Rev 9 Write"
   camcadProductRtnRev11Write                      = 102,      // "RTN Rev 11 Write"
   camcadProductSiemensQdWrite                     = 104,      // "Siemens QD Write"
   camcadProductTangoSeriesWrite                   = 105,      // "TANGO Series II Write"
   camcadProductTakayaCa9Write                     = 107,      // "TAKAYA .CA9 Write"
   camcadProductTeradyne7300Write                  = 108,      // "Teradyne 7300 IPL"
   camcadProductTeradyne7200Write                  = 109,      // "Teradyne 7200 I2I"
   camcadProductTriAoiWrite                        = 111,      // "TRI-AOI Write"
   camcadProductUnicamWrite                        = 112,      // "UNICAM PDW Write"
   camcadProductSpea4040Write                      = 118,      // "SPEA 4040 Write"
   camcadProductTeradyneZ1800Write                 = 121,      // "Teradyn Z18xxx IPL Write"
   camcadProductTeradyne228xCktWrite               = 123,      // "Teradyne (GenRad) 228X/TestStation CKT"
   camcadProductFixtureWrite                       = 124,      // "Fixture File"
   camcadProductViscomAoiWrite                     = 125,      // "Viscom AOI"
   camcadProductAsymtekDispenserWrite              = 126,      // "Asymtek Dispenser"
   camcadProductAeroflexCbWrite                    = 128,      // "Aeroflex .CB Write"
   camcadProductMvp3dPasteWrite                    = 129,      // "MVP 3D Paste AOI Write"
   camcadProductMvpComponentWrite                  = 130,      // "MVP Component AOI Write"
   camcadProductOrbotechAoiWrite                   = 131,      // "Orbotech AOI Write"
   camcadProductScorpionWrite                      = 132,      // "Scorpion Write"
   camcadProductCrTechnologyWrite                  = 133,      // "CR Technology RTI-6500 AOI Write"
   camcadProductSeicaParWrite                      = 135,      // "Seica .par .nod Write"
   camcadProductDigitalTestWrite                   = 136,      // "Digital Test Write"
   camcadProductTeradyneSpectrumWrite              = 139,      // "Teradyne Spectrum Write"
   camcadProductSonyAoiWrite                       = 141,      // "Sony AOI Write"
   camcadProductSiemensBoardWrite                  = 142,      // "Siemens Board Write"
   camcadProductRoyonics500Write                   = 143,      // "Royonics 500 Write"
   camcadProductJukiWrite                          = 145,      // "Juki Write"
   camcadProductViTechnologyAoiWrite               = 146,      // "VI Technology AOI Write"
   camcadProductKonradIctWrite                     = 148,      // "Konrad ICT Write"
   camcadProductAcculogicWrite                     = 149,      // "PRO Acculogic FPT WR Op SW"
   camcadProductTeradyne228xNavWrite               = 150,      // "Teradyne (GenRad) 228X/Navigator NAV"
   camcadProductTestabilityReportWrite             = 151,      // Testability Report Write
   camcadProductFabmasterFatfWrite                 = 152,      // "PRO FABMaster FATF WR"
   camcadProductTestronicsIctWrite                 = 153,      // "PRO Testronics ICT WR"
   camcadProductAgilentI1000Write                  = 155,      // "PRO Agilent i1000 Write"
   camcadProductCpCamcadCPPROBASE                  = 156,      // New standard base license, rest of Mentor knows this as CPPROBASE
   camcadProductOdbPlusPlusWrite                   = 158,      // "PRO OdbPlusPlus Write"
   camcadProductMyDataWrite                        = 159,      // "PRO MyData Write"
   camcadProductCustomAssembly                     = 160,      // "PRO Custom Assembly Write"
   camcadProductFujiFlexaWrite                     = 161,      // "PRO Fuji Flexa Write"
   camcadProductCommandLineCPCMD                   = 162,      // PRO Command Line Import/Export
   camcadProductSakiAOIWrite                       = 163,      // PRO Saki AOI Write
   camcadProductCyberopticsAOIWrite                = 164,      // PRO Cyberoptics AOI Write
   camcadProductHiokiICTWrite                      = 165,      // PRO Hioki ICT Write
   camcadProductOmronAOIWrite                      = 166,      // PRO Omron AOI Write
   camcadProductYestechAOIWrite                    = 167,      // PRO YESTECH Write

   camcadProductMaxIndex                           = 168,       // Maintain as EQUAL TO highest value
};

enum MentorProductIdTag
{
   mentorProductCpUnknown                             = -1,

   //
   // These are tags for Camcad Professional
   //
   mentorProductCpCamcadProfessional                  = 3604,      // Older base license key, supported for backward compatibility
   mentorProductCpCamcadCPPROBASE                     = 100274,    // As of 4.7 this is the new standard base license key
   //mentorProductCpCamcadCmdLineImportExportCPCMD      = 100594,    // Command line import CAD, export CCZ
   mentorProductCpCamcadCmdLineImportExportCPCMD      = 102219,
   mentorProductCpEsightDfm                           = 3599,
   mentorProductCpEsightDft                           = 3598,
   mentorProductCpEsigntFixtureReuse                  = 3597,
   mentorProductCpGerberEducatorComps                 = 3587,
   mentorProductCpRealPartPackage                     = 3560,
   mentorProductCpStencilGenerator                    = 3551,

   mentorProductCpAccelPcad200xRead                   = 3620,
   mentorProductCpAlcatelDociaRead                    = 3615,
   mentorProductCpAlcatelGmfRead                      = 3614,
   mentorProductCpAutocadAdiRead                      = 3612,
   mentorProductCpBarcoRead                           = 3611,
   mentorProductCpBoardStationRead                    = 3609,
   mentorProductCpCadenceAllegroExtractRead           = 3607,
   mentorProductCpCadenceAllegroIpfRead               = 3608,
   mentorProductCpCadStarCadifRead                    = 3606,
   mentorProductCpCalayPrismaRead                     = 3605,
   mentorProductCpCasePlotRead                        = 3603,
   mentorProductCpCceReadCPRDCCE                      = 102189,  // Encrypted CCZ aka CCE Read
   mentorProductCpEdifRead                            = 3600,
   mentorProductCpExpeditionAsciiRead                 = 3595,
   mentorProductCpFabMasterFatfRead                   = 3594,
   mentorProductCpGenCadRead                          = 3592,  
   mentorProductCpGenCamRead                          = 3590,
   mentorProductCpGenericCentroidRead                 = 3588,
   mentorProductCpHpEgsRead                           = 3582,
   mentorProductCpHughesRaytheonRead                  = 3580,
   mentorProductCpIgesRead                            = 3576,
   mentorProductCpIpc35xRead                          = 3574,
   mentorProductCpMentorNeutralRead                   = 3570,
   mentorProductCpOdbPlusPlusRead                     = 3567,
   mentorProductCpOrcadRead                           = 3565,
   mentorProductCpPadsRead                            = 3564,
   mentorProductCpPcadPdifRead                        = 3563,
   mentorProductCpProtelPcbRead                       = 3562,
   mentorProductCpSciCardsRead                        = 3558,
   mentorProductCpSupermaxDdeRead                     = 3550,
   mentorProductCpThedaRead                           = 3543,
   mentorProductCpUnicamPdwRead                       = 3540,
   mentorProductCpUnidatRead                          = 3538,
   mentorProductCpVeribestEifRead                     = 3537,
   mentorProductCpZukenCR3000Read                     = 3534,
   mentorProductCpZukenCR5000Read                     = 3533,

   mentorProductCpAeroflexCbWrite                     = 3619,
   mentorProductCpAcculogicWrite                      = 3634,
   mentorProductCpAgilent3070Write                    = 3618,
   mentorProductCpAgilent5DxWrite                     = 3617,
   mentorProductCpAgilentAoiWrite                     = 3616,
   mentorProductCpAgilentI1000Write                   = 100029,
   mentorProductCpApertureWrite                       = 3586,
   mentorProductCpAsymtekDispenserWrite               = 3613,
   mentorProductCpBarcoWrite                          = 3610,
   mentorProductCpCrTechnologyWrite                   = 3602,
   mentorProductCpCustomAssemblyCPWRCUSTOM            = 100595,  //CAMCAD 4.8: new style name, start to include internal symbol names CPWRCUSTOM
   mentorProductCpCyberopticsAoiWriteCPWRCYBER        = 101705,
   mentorProductCpDigitalTestWrite                    = 3601,
   mentorProductCpDxfWrite                            = 3585,
   mentorProductCpExcellonDrillWrite                  = 3596,
   mentorProductCpFabmasterFatfWrite                  = 3912,
   mentorProductCpFujiFlexaWriteCPWRFUJFLX            = 100596,  //CAMCAD 4.8: new style name, start to include internal symbol names CPWRFUJFLX
   mentorProductCpGencadWrite                         = 3591,
   mentorProductCpGencamWrite                         = 3589,
   mentorProductCpGenCcmWrite                         = 3593,
   mentorProductCpGerberWrite                         = 3584,
   mentorProductCpHiokiIctWriteCPWRHIOKI              = 101703,
   mentorProductCpHpglWrite                           = 3583,
   mentorProductCpHuntronWrite                        = 3579,
   mentorProductCpHyperlynxWrite                      = 3578,
   mentorProductCpIdfV2Write                          = 3577,
   mentorProductCpIgesWrite                           = 3575,
   mentorProductCpIpc35xWrite                         = 3573,
   mentorProductCpJukiWrite                           = 3572,
   mentorProductCpKonradIctWrite                      = 3571,
   mentorProductCpMvp3dPasteWrite                     = 3569,
   mentorProductCpMvpComponentWrite                   = 3568,
   mentorProductCpMyDataWriteCPWRMYDPNP               = 100597,  //CAMCAD 4.8: new style name, start to include internal symbol names CPWRMYDPNP
   mentorProductCpOdbPlusPlusWriteCPWRVALODB          = 100065,  //CAMCAD 4.8: new style name, start to include internal symbol names CPWRVALODB
   mentorProductCpOmronAoiWriteCPWROMRON              = 101708,
   mentorProductCpYestechAoiWriteCPWRYESAOI           = 101702,
   mentorProductCpOrbotechAoiWrite                    = 3566,
   mentorProductCpQuadWrite                           = 3561,
   mentorProductCpRoyonics500Write                    = 3559,
   mentorProductCpScorpionWrite                       = 3557,
   mentorProductCpSakiAOIWriteCPWRSAKI                = 101706,
   mentorProductCpSeicaParWrite                       = 3556,
   mentorProductCpSiemensBoardWrite                   = 3555,
   mentorProductCpSiemensQdWrite                      = 3554,
   mentorProductCpSonyAoiWrite                        = 3553,
   mentorProductCpSpea4040Write                       = 3552,
   mentorProductCpTakayaCa9Write                      = 3549,
   mentorProductCpTeradyne228xWrite                   = 3548,
   mentorProductCpTeradyne7200Write                   = 3547,
   mentorProductCpTeradyne7300Write                   = 3546,
   mentorProductCpTeradyneSpectrumWrite               = 3545,
   mentorProductCpTeradyneZ1800Write                  = 3544,
   mentorProductCpTestronicsIctWrite                  = 100063,
   mentorProductCpTriAoiWrite                         = 3542,
   mentorProductCpTriMdaWrite                         = 3541,
   mentorProductCpUnicamPdwWrite                      = 3539,
   mentorProductCpViscomAoiWrite                      = 3535,
   mentorProductCpViTechnologyAoiWrite                = 3536,

   //
   // These are tags for Camcad Pcb Tranlator
   //
   /*mentorProductCxCamcadPcbTranslatorAp               = 3677,

   mentorProductCxAccelPcad200xRead                   = 3686,
   mentorProductCxBoardStationRead                    = 3682,
   mentorProductCxCadenceAllegroExtractRead           = 3679,
   mentorProductCxCadStarCadifRead                    = 3689,
   mentorProductCxCalayPrismaRead                     = 3678,
   mentorProductCxExpeditionAsciiRead                 = 3684,
   mentorProductCxOrcadRead                           = 3680,
   mentorProductCxPadsRead                            = 3685,
   mentorProductCxPcadPdifRead                        = 3688,
   mentorProductCxProtelPcbRead                       = 3687,
   mentorProductCxSciCardsRead                        = 3681,
   mentorProductCxSupermaxDdeRead                     = 3683,
   mentorProductCxThedaRead                           = 3692,
   mentorProductCxZukenCR3000Read                     = 3690,
   mentorProductCxZukenCR5000Read                     = 3691,

   mentorProductCxAccelPcad2000Write                  = 3698,
   mentorProductCxBoardStationWrite                   = 3694,
   mentorProductCxCadenceAllegroScriptWrite           = 3693,
   mentorProductCxPadsWrite                           = 3697,
   mentorProductCxPcbExpeditionWrite                  = 3696,
   mentorProductCxSupermaxDdeWrite                    = 3695,*/

   //
   // These are tags for Camcad Graphic
   //
   mentorProductCgCamcadGraphicAp                     = 3635,
   mentorProductCgCedit                               = 3636,

   mentorProductCgApertureRead                        = 3637,
   mentorProductCgAttributeBomRead                    = 3638,
   mentorProductCgAutocadAdiRead                      = 3639,
   mentorProductCgBarcoRead                           = 3640,
   mentorProductCgCadenceAllegroIpfRead               = 3642,
   mentorProductCgCasePlotRead                        = 3641,
   mentorProductCgDmisV3Read                          = 3643,
   mentorProductCgExcellonDrillRouteRead              = 3644,
   mentorProductCgGenericCentroidRead                 = 3645,
   mentorProductCgGerberRead                          = 3646,
   mentorProductCgHpglRead                            = 3647,
   mentorProductCgIgesRead                            = 3648,
   mentorProductCgIpc35xRead                          = 3649,
   mentorProductCgOdbPlusPlusRead                     = 3650,

   /*mentorProductCgAccelPcadWrite                      = 3668,
   mentorProductCgApertureWrite                       = 3651,
   mentorProductCgAttributeBomWrite                   = 3652,
   mentorProductCgBarcoWrite                          = 3653,
   mentorProductCgBoardStationWrite                   = 3664,        
   mentorProductCgCadenceAllegroScriptWrite           = 3654,
   mentorProductCgCadstarCdiWrite                     = 3675,
   mentorProductCgDmisV3Write                         = 3656,
   mentorProductCgDxfWrite                            = 3657,
   mentorProductCgEdif200Write                        = 3663,
   mentorProductCgExcellonDrillRounteWrite            = 3658,
   mentorProductCgGerberWrite                         = 3659,
   mentorProductCgHpglWrite                           = 3660,
   mentorProductCgIgesWrite                           = 3661,
   mentorProductCgIpc35xWrite                         = 3662,
   mentorProductCgOrcadLayoutWrite                    = 3655,
   mentorProductCgPadsSchematicWrite                  = 3666,
   mentorProductCgPadsWrite                           = 3667,
   mentorProductCgPcadPdifWrite                       = 3671,
   mentorProductCgPdifSchematicWrite                  = 3670,
   mentorProductCgProtelPfwLayoutWrite                = 3669,
   mentorProductCgProtelPfwSchematicWrite             = 3672,
   mentorProductCgRtnRev11Write                       = 3673,
   mentorProductCgTangoSeriesWrite                    = 3674,
   mentorProductCgVeribestEifWrite                    = 3665,
   mentorProductCgZukenCr3000Write                    = 3676,*/

   //
   // These are tags for Camcad Vision
   //
   mentorProductCvCamcadVisionAp                      = 3699,

   mentorProductCvAccelPcadRead                       = 3737,
   mentorProductCvApertureRead                        = 3700,
   mentorProductCvAutocadAdiRead                      = 3701,
   mentorProductCvBarcoRead                           = 3702,
   mentorProductCvBoardStationRead                    = 3718,        
   mentorProductCvCadenceAllegroExtractRead           = 3705,
   mentorProductCvCadenceAllegroIpfRead               = 3706,
   mentorProductCvCadstarCadifRead                    = 3731,
   mentorProductCvCalayPrismaRead                     = 3703,
   mentorProductCvCasePlotRead                        = 3704,
   mentorProductCvDxfRead                             = 3708,
   mentorProductCvEdifRead                            = 3710,
   mentorProductCvExcellonDrillRouteRead              = 3709,
   mentorProductCvExpeditionAsciiRead                 = 3721,
   mentorProductCvFabmasterFatfRead                   = 3727,
   mentorProductCvGenCadRead                          = 3726,
   mentorProductCvGenCamRead                          = 3717,
   mentorProductCvGerberRead                          = 3711,
   mentorProductCvHpEgsRead                           = 3712,
   mentorProductCvHpglRead                            = 3713,
   mentorProductCvIgesRead                            = 3715,
   mentorProductCvIpc35xRead                          = 3716,
   mentorProductCvMentorNeutralRead                   = 3722,
   mentorProductCvOdbPlusPlusRead                     = 3730,
   mentorProductCvOrcadLayoutRead                     = 3707,
   mentorProductCvPadsRead                            = 3723,
   mentorProductCvPcadPdifRead                        = 3725,
   mentorProductCvProtelPcbRead                       = 3724,
   mentorProductCvSciCardsRead                        = 3714,
   mentorProductCvSupermaxDdeRead                     = 3719,
   mentorProductCvThedaRead                           = 3734,
   mentorProductCvUnicamPdwRead                       = 3728,
   mentorProductCvUnidatRead                          = 3729,
   mentorProductCvVeribestEifRead                     = 3720,
   mentorProductCvZukenCR3000Read                     = 3732,
   mentorProductCvZukenCR5000Read                     = 3733,

   mentorProductCvDxfWrite                            = 3735,
};

CString getCamcadProductDisplayString(const CamcadProductIdTag camcadProductId);

//-----------------------------------------------------------------------------
// MentorProductLicense
//-----------------------------------------------------------------------------
class MentorProductLicense
{
public:
   MentorProductLicense(const MentorProductIdTag mentorProductId);
   ~MentorProductLicense();

private:
   MentorProductIdTag m_mentorProductId;
   long m_transactionId;
   bool m_isLicenseGranted;
   bool m_isQueued;

public:
   MentorProductIdTag getMentorProductId()const { return m_mentorProductId; }

   void setTranactionId(const long transactionId) { m_transactionId = transactionId; }
   long getTranactionId()const { return m_transactionId; }

   void setLicenseGranted(const bool isGranted) { m_isLicenseGranted = isGranted; }
   bool getLicenseGranted()const { return m_isLicenseGranted; }

   void setQueued(const bool isQueued) { m_isQueued = isQueued; }
   bool isQueued()const { return m_isQueued; }

   CString getMentorProductName()const;
};

//-----------------------------------------------------------------------------
// CamcadProduct
//-----------------------------------------------------------------------------
class CamcadProduct
{
public:
   CamcadProduct(const CamcadProductIdTag camcadProductId, const MentorProductIdTag mentorProductId, const FileTypeTag camcadFileType, const long accessTypeFlag, const long productTypeFlag);
   CamcadProduct(const CamcadProductIdTag camcadProductId, const CString licenseName, const FileTypeTag camcadFileType, const long accessTypeFlag, const long productTypeFlag);
   ~CamcadProduct();

private:
   CamcadProductIdTag m_camcadProductId;
   MentorProductIdTag m_mentorProductId;
   FileTypeTag m_camcadFileType;
   long m_accessTypeFlag;
   long m_productTypeFlag; // only use to check that license is for the right product
   unsigned long m_accessCode;
   bool m_isAPI;

   unsigned long m_originalAccessCode;
   CString m_licenseName;
   bool m_allowed;
   bool m_isQueued;

public:
   CamcadProductIdTag getCamcadProductId()const { return m_camcadProductId; }
   MentorProductIdTag getMentorProductProId()const { return m_mentorProductId; }
   FileTypeTag getCamcadFileType()const { return m_camcadFileType; }

   void setAccessTypeFlag(const long accessTypeFlag) {  m_accessTypeFlag = accessTypeFlag; }
   long getAccessTypeFlag()const { return m_accessTypeFlag; }
   long getProductTypeFlag()const { return m_productTypeFlag; }

   void setAccessCode(const unsigned long accessCode) { m_accessCode = accessCode; }
   long getAccessCode()const { return m_accessCode; }

   void setAPI(const bool isAPI ) { m_isAPI = isAPI; }
   bool isAPI()const { return m_isAPI; }

   void setOriginalAccessCode(const unsigned long accessCode) { m_originalAccessCode = accessCode; }
   long getOriginalAccessCode()const { return m_originalAccessCode; }

   void setAllowed(const bool allowed) { m_allowed = allowed; }
   bool getAllowed()const { return m_allowed; }

   void setQueued(const bool queued) { m_isQueued = queued; }
   bool isQueued()const { return m_isQueued; }
   
   CString getCamcadLicenseName()const { return m_licenseName; }
   CString getCamcadDisplayName()const;
};

struct DateStruct
{  //                          legal range  true range
   unsigned int   hour:5;     // [0..23]     [0..31]
   unsigned int   minute:6;   // [0..59]     [0..63]
   unsigned int   month:4;    // [1..12]     [0..15]
   unsigned int   day:5;      // [1..31]     [0..31]
   unsigned int   year:12;    // [1997..)    [0..4095]
};

//-----------------------------------------------------------------------------
// CamcadLicenseBase
//-----------------------------------------------------------------------------
class CamcadLicenseBase
{
public:
   CamcadLicenseBase();
   ~CamcadLicenseBase();

protected:
   CString m_camcadLicenseKey;
   CTypedMapIntToPtrContainer<CamcadProduct*> m_camcadProductMap;
   int m_licensedApiLevel;
   bool m_initializedLicenses;
   bool m_checkOutLicenses;
   bool m_hasBaseLicense;

   DateStruct m_expirationDate;
   bool m_useDongle;
   bool m_hasExpiration;
   CString m_serialNumber;
   CString m_company;
   CString m_user;
   CString m_hostId;

   //void createCamcadProductMap();
   void addCamcadProduct(const CamcadProductIdTag camcadProductId, const FileTypeTag camcadFileType, const char accessTypeFlag, const long productTypeFlag);

   void activateCamcadProduct(const CamcadProductIdTag ccProductId);
   void activateBaseProductLicenses();

   void setSpecialLicenses();
   CString makeFlexLmLicenseName(const CamcadProductIdTag camcadProductId);
   CString removePuncutation(const CString sourceString);
   int isFormatLicensed(const int fileFormat, const bool import); 

public:
   void resetAllLicense();
   void initializeLicenses();
   bool checkOutLicenses();
   bool checkOutLicenses(const bool rescan);
   bool isLicensed(CamcadProductIdTag camcadProductId);
   bool hasBaseLicense()const { return m_hasBaseLicense; }
   void warnExpiration();
   int isFormatLicensedForImport(const int fileFormat) { return isFormatLicensed(fileFormat, true); }
   int isFormatLicensedForExport(const int fileFormat) { return isFormatLicensed(fileFormat, false); }

   CamcadProduct *getImporterProduct(FileTypeTag filetype);
   CamcadProduct *getExporterProduct(FileTypeTag filetype);

   short setLicenseByApi(const CamcadProductIdTag camcadProductId, const long accessCode);
   short setOEMLicenseByApi(const CString OEMName, const CamcadProductIdTag camcadProductId, const long accessCode, bool& lockLicense);

   CString getCamcadLicenseKey() const { return m_camcadLicenseKey; }
   int getLicensedApiLevel() const { return m_licensedApiLevel; }
   DateStruct getExpirationDate() const { return m_expirationDate; }
   bool getUseDongle() const { return m_useDongle; }
   bool getHasExpiration() const {return m_hasExpiration; }
   CString getSerialNubmer() const { return m_serialNumber; }
   CString getCompany() const { return m_company; }
   CString getUser() const { return m_user; }
   CString getHostId()const { return m_hostId; }
   CString getExpirationDateString() const;

   POSITION getProductStartPosition() const { return m_camcadProductMap.GetStartPosition(); }
   CamcadProduct* findCamcadProduct(const CamcadProductIdTag camcadProductId);
   CamcadProduct* getNextProduct(POSITION& pos)
   {
      CamcadProduct* camcadProduct = NULL;
      if (pos != NULL)
      {
         int camcadProductId;
         m_camcadProductMap.GetNextAssoc(pos, camcadProductId, camcadProduct);
      }

      return camcadProduct;
   }
   void addCamcadProductAccessFlag(const CamcadProductIdTag ccProductId, int accessFlag);

   // Remove these
   // serialNum - Api.cpp, Camcad.cpp, license.cpp
   // SecurityKeyUser - camcad.cpp, license.cpp
};

//-----------------------------------------------------------------------------
// CamcadLicense
//-----------------------------------------------------------------------------
class CamcadLicense : public CamcadLicenseBase
{
public:
   CamcadLicense();
   ~CamcadLicense();

private:
   CTypedMapIntToPtrContainer<MentorProductLicense*> m_mentorProductLicenseMap;
   //CTypedMapIntToPtrContainer<CamcadProduct*> m_camcadProductMap;
   CMapStringToString m_availableProductMap;
   CString m_camcadLicenseExactAccessDate;
   CString m_availableLicenseFileName;
   MentorProductIdTag m_baseLicenseProductId;

   /*void createProfessionalCamcadProductMap();
   void createPcbTranslatorProductMap();
   void createGraphicCamcadProductMap();
   void createVisionCamcadProductMap();*/
   void addCamcadProduct(const CamcadProductIdTag camcadProductId, const MentorProductIdTag mentorProductId, const FileTypeTag camcadFileType, const long accessTypeFlag, const long productTypeFlag);
   MentorProductLicense* findMentorProductLicense(const MentorProductIdTag mentorProductId);

   void loadAvailableProductMap();
   void saveAvailableProductMap();
   //long checkOutLicense(const int applicationId, long &transactionId, CString &errorMessage);
   long checkOutLicense(MentorProductLicense& mentorProductLicense, CString& errorMessage, const bool rescan);
   void setCamcadProductAllowed();
   void checkInAllLicenses();
   long checkInLicense(const long transactionId, CString &errorMessage);
   int isFormatLicensed(const int fileFormat, const bool import);

public:
   MentorProductIdTag getBaseMentorProductId() const { return m_baseLicenseProductId; }

   void initializeLicenses();
   bool checkOutLicenses();
   bool checkOutLicenses(const bool rescan);
   bool checkAvialable();
   bool isLicensed(CamcadProductIdTag camcadProductId);
   int isFormatLicensedForImport(const int fileFormat) { return isFormatLicensed(fileFormat, true); }
   int isFormatLicensedForExport(const int fileFormat) { return isFormatLicensed(fileFormat, false); }
};



#endif
