// $Header: /CAMCAD/4.3/Drc_Util.h 8     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2001. All Rights Reserved.

   A good DRC should have the following info:

   DataStruct of a DRC symbol.
      ATT_DFM_TESTNAME :               
      ATT_DFM_FAILURE_RANGE       
      ATT_DFM_CHECK_VALUE         
      ATT_DFM_ACTUAL_VALUE        

*/           
 
#include <afxtempl.h>
#include "ccdoc.h"

/**************************************************************************
* 
*/

typedef struct
{
   int            block_num;  //
   CString        name;
   double         drill;
   int            smd;        // smd flag from PADSTACK
   int            typ;        // return 0x1 top
                              //        0x2 bottom
                              //        0x4 drill
                              //        0x8 top only        - layer attribute
                              //        0x10 bottom only    - layer attribute
   int            shapetypetop, shapetypebot, shapetypeinner;
   double         sizetop, xofftop, yofftop;
   double         sizebot, xoffbot, yoffbot;
   double         sizeinner, xoffinner, yoffinner;
   UINT64         layerarray;
}DRC_Padstack; // used in derive netlist only
typedef CTypedPtrArray<CPtrArray, DRC_Padstack*> DRC_PadstackArray;

// this datastructure collects apertures on top and bottom layer
typedef struct
{
   int            block_num;  //
   CString        name;
   double         drill;
   int            smd;        // smd flag from PADSTACK
   int            typ;        // return 0x1 top
                              //        0x2 bottom
                              //        0x4 drill
                              //        0x8 top only        - layer attribute
                              //        0x10 bottom only    - layer attribute

   int            aperture_num_top;    // init to -1
   int            aperture_num_bot;
}DRC_AperturePadstack;  // used in derive netlist only
typedef CTypedPtrArray<CPtrArray, DRC_AperturePadstack*> DRC_AperturePadstackArray;

// this collects all pin from the netlist data
// this is dependend on the padstack index (DRC_Padstack)
typedef struct
{
   long           entitynumber;
   CString        compname;
   CString        pinname;
   CString        netname;
   double         x;
   double         y;
   double         rotation;
   int            mirror;  // component is mirrored.
   int            layer;   // 3 all 1 top 2 bottom, which pin exist in
   int            result;  // 0 is testable, 1 is blocked
   int            aperturepadstackindex;  // this is the absolute padstack. the mirror flag on the
                                          // padstack indicates if the aperture top, bottom has
                                          // to be changed.
}DRC_pinkoo;
typedef CTypedPtrArray<CPtrArray, DRC_pinkoo*> DRC_PinkooArray;

// here are only INSERTTYPE_PCBCOMPONENT components collected.
typedef struct
{
   CString  compname;
   double   x,y,rot;
   int      geomnum;
   char     mirror;           // 3 all 1 top 2 bottom
   char     bottom_placed;    // 0 no, 1 yes
   char     test_connector;   // if the attribute ATT_TEST_CONNECTOR is assigned
}DRC_component;
typedef CTypedPtrArray<CPtrArray, DRC_component*> DRC_ComponentArray;

/*****************************************************************************/
/* 
*/

int RemoveOneDRC(CCEtoODBDoc *doc, DRCStruct *drc, FileStruct *file);

DRCStruct *add_drcmarker(CCEtoODBDoc *doc, FileStruct *file, const char *string, int drccode, double x, double y, const char *layerName);

// here are predefined DRC functions.

//the layername needs the stacknumber to know where it is on
int drc_probable_short_pin_pin(CCEtoODBDoc *doc, FileStruct *file, const char *layername,
                           const char *comment, double x, double y, int layertype, int stacknum, 
                           double check_value, double actual_value,
                           int pin1entity, double pin1x, double pin1y, int pin2entity, double pin2x, double pin2y);

int drc_probable_short_pin_feature(CCEtoODBDoc *doc, FileStruct *file, const char *layername,
                           const char *comment, double x, double y, int layertype, int stacknum, 
                           double check_value, double actual_value,
                           int pinentity, double pinx, double piny, int viaentity, double viax, double viay);

int drc_probable_short_feature_feature(CCEtoODBDoc *doc, FileStruct *file, const char *layername,
                           const char *comment, double x, double y, int layertype, int stacknum, 
                           double check_value, double actual_value,
                           int via1entity, double via1x, double via1y, int via2entity, double via2x, double via2y);

int drc_net_without_access(CCEtoODBDoc *doc, FileStruct *file, const char *netname);

void drc_singlepin_without_access(CCEtoODBDoc *doc, FileStruct *file, double x, double y, const char *comp, const char *pin);

int drc_net_without_probe(CCEtoODBDoc *doc, FileStruct *file, const char *netname);

void drc_singlepin_without_probe(CCEtoODBDoc *doc, FileStruct *file, double x, double y, const char *comp, const char *pin);

// get electrical layers
int drc_check_layers(CCEtoODBDoc *doc, int *startstacknum, int *endstacknum);

// get the padstacks with electrical top/bottom setup.
int drc_do_padstacks(CCEtoODBDoc *doc, int startstacknum, int endstacknum);
int drc_do_aperturepadstacks(CCEtoODBDoc *doc);

UINT64 drc_set_layerarray(int nr);

int drc_do_aperturenetlistpinloc(FILE *fp, CCEtoODBDoc *doc, CNetList *NetList, double scale, CDataList *DataList);

int drc_get_padstackindex(const char *p);
int drc_get_aperturepadstackindex(const char *p);

void drc_load_allcomps(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);

int drc_is_comp_mirror(const char *c);
int drc_get_comp_ptr(const char *c);

double drc_get_largestradius();

void drc_init();  // setup arrays needed
void drc_deinit();   // free them

// end drc_util.h
