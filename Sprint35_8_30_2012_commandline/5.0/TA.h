// $Header: /CAMCAD/4.3/TA.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           
 
#include <afxtempl.h>
#include "ccdoc.h"
#include "ta_optn.h"
#include "ta_probe.h"

typedef struct
{
   CString  netname;
   int      unused_net; // this is a single pin netname - normally used for UNUSED_PINS
   int      net_used;   // if the net is used on the prefered layer, do not use it again
   int      net_status;
   int      probes_needed; // 
   int      probes_done;
   int      viacnt, pincnt, test_probecnt;   // pins, vias, testpads, freepads etc...
   int      padstack_bottom_surface_access, padstack_top_surface_access;   // 0 is none, others are how many
   int      physical_bottom_surface_access, physical_top_surface_access;   // 0 is none, others are how many
   int      feature_bottom_surface_access, feature_top_surface_access;     // 0 is none, others are how many
   int      option_bottom_surface_access, option_top_surface_access;       // 0 is none, others are how many
} TA_net;
typedef CTypedPtrArray<CPtrArray, TA_net*> TA_NetArray;

typedef struct
{
   long     entitynumber;
   CString  compname;
   CString  pinname;

   // pin koos
   double   x, y;    
      
   // access absolute top
   double   access_top_x, access_top_y;
   // access absolute bottom
   double   access_bottom_x, access_bottom_y;

   // this is the distance of this pin after eval comp outline and board outline
   double   outline_top_distance;      
   double   outline_bottom_distance;

   char     cadlayer;            // padstack exist on layer - same as layer, just as it was in cad
   char     layer;               // same as cadlayer, after an eventual ATT_TEST layer is applied
   char     soldermask;          // layer 1 top, 2 bottom, 3 all
   char     net_access;          // after netaccess layer 3 all 1 top 2 b
   char     physical_accessible; // after physical 3 all 1 top 2 bottom 
   char     feature_accessible;  // after physical 3 all 1 top 2 bottom, but is counts allowed features <-- this is the result
   int      netindex;

   char     single_feature:1;    // used for single_feature functionality

   char     soldermasktop_deleted:1;      // 
   char     soldermaskbottom_deleted:1;   // 
   char     net_not_tested:1; // if TEST_NET_STATUS == NO PROBES or PROBES needed == 0, net will not be tested
   char     connector:1;      // is pin a Connector
   char     smd:1;            // is pin an SMD
   char     test:1;           // has pin a test attribute.
   char     featuresizetop_deleted:1;
   char     featuresizebottom_deleted:1;

   // records feature type not allows
   char     featureallowedviatop_deleted:1;
   char     featureallowedviabottom_deleted:1;
   char     featureallowedsmdtop_deleted:1;
   char     featureallowedsmdbottom_deleted:1;
   char     featureallowedthrutop_deleted:1;
   char     featureallowedthrubottom_deleted:1;
   char     featureallowedtesttop_deleted:1;
   char     featureallowedtestbottom_deleted:1;
   char     featureallowedconnectortop_deleted:1;
   char     featureallowedconnectorbottom_deleted:1;

   int      test_access;      // 0 none, 1 top, 2 bottom, 3 all from ATT_TESTACCESS, -1 not set
   int      test_preference;  // number 0 none high more pref
   int      result;           // 0 is testable, 1 is blocked
   int      probe_assigned;
   int      compoutline;      // 1 top, 2 bottom
   int      boardoutline;     // 1 top, 2 bottom
   int      mirror;
   int      padstackindex;    // index into DRC_Padstack
} TA_pinkoo;
typedef CTypedPtrArray<CPtrArray, TA_pinkoo*> PinkooArray;

typedef struct
{
   long     entitynumber;
   int      entity_type;      // via or testprobe or freepad
   CString  refname;          // if avail

   // absolute placement
   double   x, y;

   // access absolute top
   double   access_top_x, access_top_y;
   // access absolute bottom
   double   access_bottom_x, access_bottom_y;

   // this is the distance of this pin after eval comp outline and board outline
   double   outline_top_distance;      
   double   outline_bottom_distance;

   char     cadlayer;         // same as layer, just as it was in cad
   char     layer;            // same as cadlayer, after an eventual ATT_TEST layer is applied
   char     net_access;       // after net access filter 3 all 1 top 2 bottom
   char     soldermask;          // layer 1 top, 2 bottom, 3 all
   char     physical_accessible; // after physical filter 3 all 1 top 2 bottom
   char     feature_accessible;  // after physical filter 3 all 1 top 2 bottom, allowed features <-- this is the result
   int      netindex;

   char     single_feature:1;    // used for single_feature functionality

   char     connector:1;      // is pin a Connector
   char     smd:1;            // is pin and SMD
   char     test:1;           // has pin a test attribute.
   char     net_not_tested:1; // if TEST_NET_STATUS == NO PROBES or PROBES needed == 0, net will not be tested
   char     soldermasktop_deleted:1;      // 
   char     soldermaskbottom_deleted:1;   // 
   char     featuresizetop_deleted:1;
   char     featuresizebottom_deleted:1;
   // records feature type not allows
   char     featureallowedviatop_deleted:1;
   char     featureallowedviabottom_deleted:1;
   char     featureallowedsmdtop_deleted:1;
   char     featureallowedsmdbottom_deleted:1;
   char     featureallowedthrutop_deleted:1;
   char     featureallowedthrubottom_deleted:1;
   char     featureallowedtesttop_deleted:1;
   char     featureallowedtestbottom_deleted:1;
   char     featureallowedconnectortop_deleted:1;
   char     featureallowedconnectorbottom_deleted:1;

   int      test_access;      // 0 none, 1 top, 2 bottom, 3 all from ATT_TESTACCESS, -1 not set
   int      test_preference;  // number 0 none high more pref
   int      result;           // 0 is testable, 1 is blocked
   int      probe_assigned;
   int      compoutline;      // 1 top, 2 bottom
   int      boardoutline;     // 1 top, 2 bottom
   int      mirror;
   //
   int      padstackindex;    // index into DRC padstack -- if a free pad or testpad is used, there is no padstack
                              // assigned.

   // this is a relative top or bottom, influenced by mirror (like the padstack)
   int      apertureindextop, apertureindexbot; // DRC_Aperturepadstack
} TA_viakoo;
typedef CTypedPtrArray<CPtrArray, TA_viakoo*> ViakooArray;

typedef struct // here are the results on a test access run 
{
   int   total_nets, total_access_nets;
   int   multiportnets, multiport_access_nets;
   int   singleportnets, singleport_access_nets;
   int   net_access_total_top, net_access_total_bottom;
   int   net_access_physical_top, net_access_physical_bottom;
   int   net_access_feature_top, net_access_feature_bottom;
} TA_PCBFileTestAccess1;   // this holds the pcbfile values needed in the new TestAccessDialog;

void TA_Percent(CString *v, int v1, int v2);
void TAload_defaultsettings(CCEtoODBDoc *doc, const char *fname, TA_OptionsStruct *defaultsetting, int PageUnits);
void DeleteTestAccessNetlist(CCEtoODBDoc *doc, CNetList *NetList);
void TA_DeleteTestAccess(CCEtoODBDoc *doc, FileStruct *pcbfile);
void DeleteTestAccessData(CCEtoODBDoc *doc, CDataList *DataList);
int get_pinkoocnt_entity(long entity);
int TA_TestProbeInit(CCEtoODBDoc *doc, FileStruct *pcbFile, int eval_singlepinnets);
void TA_TestProbeDeInit();

