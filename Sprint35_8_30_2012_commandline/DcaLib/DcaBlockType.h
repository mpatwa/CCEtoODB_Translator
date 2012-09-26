// $Header: /CAMCAD/DcaLib/DcaBlockType.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaBlockType_h__)
#define __DcaBlockType_h__

#pragma once

enum BlockTypeTag
{
   blockTypeUndefined         = -2,  //  
   blockTypeGeometryEdit      = -1,  // BLOCKTYPE_GEOMETRY_EDIT    -1  // File only used for Geometry Edit

   blockTypeUnknown           = 0 ,  // BLOCKTYPE_UNKNOWN           0  // these are blocktype markers, needed to add
   blockTypePcb               = 1 ,  // BLOCKTYPE_PCB               1  // intelligence to a block
                                      
   blockTypePadshape          = 2 ,  // BLOCKTYPE_PADSHAPE          2  // Graphic, which is used to build a PADSTACK
                                      
                                                                       // padshape graphic can be used instead of a complex aperture. All this graphic belongs to a padstack.
                                                                       // PADSTACKGRAPHIC will be elimitanted and PADSHAPE used instead.
   blockTypePadstackGraphic   = 3 ,  // BLOCKTYPE_PADSTACKGRAPHIC   3  // No longer used
                                      
   blockTypePadstack          = 4 ,  // BLOCKTYPE_PADSTACK          4  // Padstack and Viastack, which has PADSHAPE graphic
   blockTypePcbComponent      = 5 ,  // BLOCKTYPE_PCBCOMPONENT      5  // electrical component
   blockTypeMechComponent     = 6 ,  // BLOCKTYPE_MECHCOMPONENT     6  // mechanical component   
   blockTypeGenericComponent  = 7 ,  // BLOCKTYPE_GENERICCOMPONENT  7  // graphic component
   blockTypePanel             = 8 ,  // BLOCKTYPE_PANEL             8  // this is like a new sheet  
   blockTypeDrawing           = 9 ,  // BLOCKTYPE_DRAWING           9  // this is like a new sheet  
   blockTypeFiducial          = 10,  // BLOCKTYPE_FIDUCIAL          10  
   blockTypeTooling           = 11,  // BLOCKTYPE_TOOLING           11 // this is a drill only, can have attributes and a toolgraphic
                                                                       // assigned.
                                      
   blockTypeTestPoint         = 12,  // BLOCKTYPE_TESTPOINT         12 // this must be defined like a PCBCOMPONENT with Padstack underneath.
                                      
   blockTypeDimension         = 13,  // BLOCKTYPE_DIMENSION         13 // dimensions are grouped into blocks.  
   blockTypeLibrary           = 14,  // BLOCKTYPE_LIBRARY           14 // this fileblock is a library definition, this allows  
                                                                       // multiple librarys with same name-definitions.
   blockTypeLocalPcbComp      = 15,  // BLOCKTYPE_LOCALPCBCOMP      15 // this fileblock is a library definition copy  
   blockTypeToolGraphic       = 16,  // BLOCKTYPE_TOOLGRAPHIC       16 // Graphic, which is used to show a drill  
   blockTypeSheet             = 17,  // BLOCKTYPE_SHEET             17 // Schematic Page - Changed from BLOCKTYPE_SCHEMATICPAGE to BLOCKTYPE_SHEET on 05/02/03  
   blockTypeSymbol            = 18,  // BLOCKTYPE_SYMBOL            18 // Schematic Symbol - Changed from BLOCKTYPE_GATE to BLOCKTYPE_SYMBOL on 05/02/03  
   blockTypeGatePort          = 19,  // BLOCKTYPE_GATEPORT          19 // Schematic Pin (Port) on Gate  
   blockTypeDrillHole         = 20,  // BLOCKTYPE_DRILLHOLE         20 // Free standing drill hole, this is like a padstack, but just a drill  
   blockTypeRedLine           = 21,  // BLOCKTYPE_REDLINE           21 // Redline File
                                      
   blockTypeTestProbe         = 22,  // BLOCKTYPE_TEST_PROBE        22  
   blockTypeCentroid          = 23,  // BLOCKTYPE_CENTROID          23  
   blockTypeDrcMarker         = 24,  // BLOCKTYPE_DRCMARKER         24  
                                      
   blockTypeGeomLibrary       = 25,  // BLOCKTYPE_GEOM_LIBRARY      25  
                                      
   blockTypeTestPad           = 26,  // BLOCKTYPE_TESTPAD           26 // this like a padstack  
   blockTypeAccessPoint       = 27,  // BLOCKTYPE_TEST_ACCESSPOINT  27  
   blockTypeSchemJunction     = 28,  // BLOCKTYPE_SCHEM_JUNCTION    28  
   blockTypeGluePoint         = 29,  // BLOCKTYPE_GLUEPOINT         29  
                                      
   blockTypeRejectMark        = 30,  // BLOCKTYPE_REJECTMARK        30
                                      
   blockTypeXOut              = 31,  // BLOCKTYPE_XOUT              31  
                                      
   blockTypeRealPart          = 32,  // BLOCKTYPE_REALPART          32  
   blockTypePackage           = 33,  // BLOCKTYPE_PACKAGE           33  
   blockTypePackagePin        = 34,  // BLOCKTYPE_PACKAGEPIN        34
   blockTypeComplexDrillHole  = 35,  // BLOCKTYPE_COMPLEXDRILLHOLE  35
   blockTypeCompositeComp     = 36,  // BLOCKTYPE_COMPOSITECOMP     36
   blockTypeRouteTarget       = 37,  // BLOCKTYPE_ROUTETARGET       37

   blockTypeDie               = 38,
   blockTypeDiePin            = 39,
   blockTypeBondWire          = 40,
   blockTypeBondPad           = 41,

   blockTypeTagFence              ,  // This should always be one greater than the maximum concrete block type

   blockTypeLowerBound        =  0,  //
   blockTypeUpperBound        = blockTypeTagFence - 1,

   BlockTypeTagMin            =  0,
   BlockTypeTagMax            = blockTypeTagFence - 1
};     

CString blockTypeToDisplayString(BlockTypeTag blockType);
CString blockTypeToString(int blockType);
BlockTypeTag intToBlockTypeTag(int blockType);

#endif
