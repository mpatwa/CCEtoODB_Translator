// $Header: /CAMCAD/4.6/Lic.cpp 96    1/15/07 5:08p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include "license.h"
#include "dbutil.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *lics[] = 
{
/*000*/     LIC_PRINT_S,
/*001*/     LIC_QUERY_S,
/*002*/     LIC_CEDIT_S,
/*003*/     LIC_PEDIT_S,
/*004*/     LIC_API_LEVEL_1_S,
/*005*/     LIC_API_LEVEL_2_S,
/*006*/     LIC_API_LEVEL_3_S,
/*007*/     LIC_API_LEVEL_4_S,
/*008*/     LIC_REDLINE_S,
/*009*/     LIC_TA_S,
/*010*/     LIC_DFM_S,
/*011*/     LIC_ACCELPCBIN_S,
/*012*/     LIC_ALLEGROIN_S,
/*013*/     LIC_APERIN_S,
/*014*/     LIC_AUTOCAD_ADIIN_S,
/*015*/     LIC_BARCOIN_S,
/*016*/     LIC_BNRIN_S,
/*017*/     LIC_BOMREAD_S,
/*018*/     LIC_CADENCEPLOT_S,
/*019*/     LIC_CADIFIN_S,
/*020*/     LIC_CASEPLOTIN_S,
/*021*/     LIC_CALAYPRISMAIN_S,
/*022*/     LIC_CCTIN_S,
/*023*/     LIC_CIIIN_S,
/*024*/     LIC_CR3000IN_S,
/*025*/     LIC_CR5000IN_S,
/*026*/     LIC_DDEIN_S,
/*027*/     LIC_DXFIN_S,
/*028*/     LIC_DMISIN_S,
/*029*/     LIC_UNIDATIN_S,
/*030*/     LIC_EDIFIN_S,
/*031*/     LIC_EIFIN_S,
/*032*/     LIC_EXCELLONIN_S,
/*033*/     LIC_FATFIN_S,
/*034*/     LIC_FAB_NAIL_IN_S,
/*035*/     LIC_GENCADIN_S,
/*036*/     LIC_GENCAMIN_S,
/*037*/     LIC_GERBERIN_S,
/*038*/     LIC_HUGHESIN_S,
/*039*/     LIC_HPEGSIN_S,
/*040*/     LIC_HPGLIN_S,
/*041*/     LIC_HP3070IN_S,
/*042*/     LIC_HP5DXIN_S,
/*043*/     LIC_IPCIN_S,
/*044*/     LIC_IGESIN_S,
/*045*/     LIC_MENNEUTIN_S,
/*046*/     LIC_MENTORIN_S,
/*047*/     LIC_ODBPPIN_S,
/*048*/     LIC_ORCADLTDIN_S,
/*049*/     LIC_PADSPCBIN_S,
/*050*/     LIC_PDIFPCBIN_S,
/*051*/     LIC_PDIFSCHIN_S,
/*052*/     LIC_PFWSCHIN_S,
/*053*/     LIC_PROTELPCBIN_S,
/*054*/     LIC_THEDAIN_S,
/*055*/     LIC_ULTIBOARDIN_S,
/*056*/     LIC_UNICAMIN_S,
/*057*/     LIC_VB99IN_S,
/*058*/     LIC_VIEWDRAWIN_S,
/*059*/     LIC_3D_PCBOUT_S,
/*060*/     LIC_ACCELPCBOUT_S,
/*061*/     LIC_ALLEGROOUT_S,
/*062*/     LIC_AGILENT_AOI_OUT_S,
/*063*/     LIC_APEROUT_S,
/*064*/     LIC_BARCOOUT_S,
/*065*/     LIC_BOMWRITE_S,
/*066*/     LIC_CADIFOUT_S,
/*067*/     LIC_CCMOUT_S,
/*068*/     LIC_CDIOUT_S,
/*069*/     LIC_CR3000OUT_S,
/*070*/     LIC_DDEOUT_S,
/*071*/     LIC_DMISOUT_S,
/*072*/     LIC_DXFOUT_S,
/*073*/     LIC_EDIFOUT_S,
/*074*/     LIC_EIFOUT_S,
/*075*/     LIC_VB99OUT_S,
/*076*/     LIC_EXCELLONOUT_S,
/*077*/     LIC_FATFOUT_S,
/*078*/     LIC_GENCADOUT_S,
/*079*/     LIC_GENCAMOUT_S,
/*080*/     LIC_GERBEROUT_S,
/*081*/     LIC_HP3070OUT_S,
/*082*/     LIC_HP5DXOUT_S,
/*083*/     LIC_HPGLOUT_S,
/*084*/     LIC_HUNTRON_S,
/*085*/     LIC_HYPERLYNXOUT_S,
/*086*/     LIC_IGESOUT_S,
/*087*/     LIC_INGUNOUT_S,
/*088*/     LIC_IPCOUT_S,
/*089*/     LIC_MENTBRDOUT_S,
/*090*/     LIC_MENTNEUTOUT_S,
/*091*/     LIC_MXTOUT_S,
/*092*/     LIC_ODBPPOUT_S,
/*093*/     LIC_ORCADLTDOUT_S,
/*094*/     LIC_PADSPCBOUT_S,
/*095*/     LIC_PADSSCHOUT_S,
/*096*/     LIC_PDIFPCBOUT_S,
/*097*/     LIC_PDIFSCHOUT_S,
/*098*/     LIC_PFWOUT_S,
/*099*/     LIC_PFWSCHOUT_S,
/*100*/     LIC_QUADOUT_S,
/*101*/     LIC_RTN9OUT_S,
/*102*/     LIC_RTN11OUT_S,
/*103*/     LIC_RDCOUT_S,
/*104*/     LIC_SIEMENS_QDOUT_S,
/*105*/     LIC_TANOUT_S,
/*106*/     LIC_TAKAYA8OUT_S,
/*107*/     LIC_TAKAYA9OUT_S,
/*108*/     LIC_TERADYNE_7300OUT_S,
/*109*/     LIC_TERADYNE_7200OUT_S,
/*110*/     LIC_THEDAOUT_S,
/*111*/     LIC_TRI_AOIOUT_S,
/*112*/     LIC_UNICAMOUT_S,
/*113*/     LIC_ALCATEL_DOCICA_S,
/*114*/     LIC_ALCATEL_GMF_S,
/*115*/     LIC_PADSLIBIN_S,
/*116*/     LIC_RP_PKG_S,
/*117*/     LIC_RP_DEV_S,
/*118*/     LIC_SPEA_4040_S,
/*119*/     LIC_IPLOUT_S,
/*120*/     LIC_AgilentSjPlx_S,
/*121*/     LIC_IPLOUT_S,
/*122*/     LIC_GerberEducator_S,
/*123*/     LIC_CKT_S,
/*124*/     LIC_FIXTURE_S,
/*125*/     LIC_VISCOM_AOI_S,
/*126*/     LIC_ASYMTEK_S,             
/*127*/     LIC_NAR_IN_S,              
/*128*/     LIC_AEROFLEX_OUT_S,              
/*129*/     LIC_MVP_3D_PASTE_AOI_OUT_S,              
/*130*/     LIC_MVP_COMPONENT_AOI_OUT_S,              
/*131*/		LIC_ORBOTECH_OUT_S,
/*132*/		LIC_SCORPION_OUT_S,
/*133*/		LIC_RTI_OUT_S,
/*134*/		LIC_FabMasterDeviceRead_S,
/*135*/		LIC_SeicaParNod_OUT_S,
/*136*/		LIC_DIGITALTEST_OUT_S,
/*137*/		LIC_BOMCSV_READ_S,
/*141*/		LIC_SONY_AOI_OUT_S,
/*142*/     LIC_SIEMENS_BOARD_WRITE_S,
/*143*/     LIC_ROYONICS_OUT_S,
/*144*/		LIC_GenericCentroid_IN_S,
/*145*/		LIC_JUKI_OUT_S,
/*146*/     LIC_VITECH_OUT_S,
/*147*/     LIC_SPECTRUMNAILREQ_IN_S,
/*148*/     LIC_KONRAD_ICT_OUT_S
};

License *licenses;
//static  char*  rsi_key;

/********************************************************************
* hash
*/
// We need to try some different types of algorithm.

// 1. multiply serial number with index
// 2. Take sn and flip every bit 0 <> 1 2 <> 3

unsigned long hash(const char *c)
{
#ifdef SHAREWARE // do not put hash routine in SHAREWARE Executable
   return 0;
#else

   long i =0;
   int keyloop = 0;
   CString camcadLicenseKey = getApp().getCamcadLicense().getCamcadLicenseKey();
   int len = strlen(camcadLicenseKey);

   while (*c)
   {
      char  cc = *c++;

      // only A..Z,0..9 not spaces etc...
      if (!isalnum(cc))  continue;
      // make it CASE independent
      if (islower(cc))   cc = toupper(cc);
      
      cc = cc - 'A' + camcadLicenseKey[keyloop] - 'a';   // new key. 12-Feb-97
      keyloop = (++keyloop) % len;

      i = (i << 1) ^ cc;
      if (i < 0)   i = -i;

   }
   
   return(i);
#endif
}

/********************************************************************
* crypt
*/
unsigned long crypt(unsigned long sn, int index)
{
#ifdef SHAREWARE // do not put crypt routine in SHAREWARE Executable
   return 0;
#else
   unsigned long newsn = sn + (sn * index);
   unsigned long csn = 0;
   int  i;

   for (i=0;i<32;i=i+2)
   {
      long l,h;
      l = ( (1L << i ) & newsn)?1:0;
      h = ( (1L << (i+1) )& newsn)?1:0;
      csn |= ( l << (i+1) );
      csn |= ( h << i );
   }

   return csn;
#endif
}

//-----------------------------------------------------------------------------
// License
//-----------------------------------------------------------------------------
License::License()
{
   originalaccesscode = 0;
   visibleName.Empty();
   licname.Empty();
   product = PRODUCT_NONE;
   type = ACCESS_NONE;
   fileType = Type_Unknown;
   api = FALSE;
   allowed = 0;
   accesscode = 0;
}

License::~License()
{
}


