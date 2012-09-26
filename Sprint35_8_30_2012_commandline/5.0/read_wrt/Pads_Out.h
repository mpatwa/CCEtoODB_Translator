// $Header: /CAMCAD/4.4/read_wrt/Pads_out.h 14    8/23/04 6:10p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__Pads_Out_h__)
#define __Pads_Out_h__

#pragma once

#include "TypedContainer.h"

/* Define Section *********************************************************/

#define TEXTCORRECT        1.4         // adjust real height to PADS height 

#define LAY_TOP            -1
#define LAY_BOT            -2
#define LAY_INNER          -3
#define LAY_ALL            -4
#define LAY_UNKNOWN        -5

#define MAX_PADSLAYER      30

/******************* Layer types use for via padstack **************************/
// For TOP layers
#define LAY_PADTOP_1       "PadTop_1"  // Priority 1: PAD TOP with electrical stack #1
#define LAY_PADTOP_0       "PadTop_0"  // Priority 2: PAD TOP with electrical stack #0
#define LAY_SIGTOP_1       "SigTop_1"  // Priority 3: SIGNAL TOP with electrical stack #1
#define LAY_SIGTOP_0       "SigTop_0"  // Priority 4: SIGNAL TOP with electrical stack #0
#define LAY_ELESTK_1       "EleStk_1"  // Priority 5: Any layer with electrical stack #1

// For BOTTOM layers
#define LAY_PADBOT_n       "PadBot_n"  // Priority 1: PAD BOTTOM with electrical stack #n, where n is the last eletrical number
#define LAY_PADBOT_0       "PadBot_0"  // Priority 2: PAD BOTTOM with electrical stack #0
#define LAY_SIGBOT_n       "SigBot_n"  // Priority 3: SIGNAL BOTTOM with electrical stack #n
#define LAY_SIGBOT_0       "SigBot_0"  // Priority 4: SIGNAL BOTTOM with electrical stack #0
#define LAY_ELESTK_n       "EleStk_n"  // Priority 5: Any layer with electrical stack #n

// For INNER layers
#define LAY_PADINN_N       "PadInn_N"  // Priority 1: PAD INNER with electrical stack #N, where N is 2...n-1
#define LAY_SIGINN_N       "SigInn_N"  // Priority 2: SIGNAL INNER with electrical stack #N
#define LAY_ELESTK_N       "EleStk_N"  // Priority 3: Any layer with electrical stack #N
#define LAY_PADINN_0       "PadInn_0"  // Priority 4: PAD INNER with electrical stack #0, which means it is INNER ALL
#define LAY_SIGINN_0       "SigInn_0"  // Priority 5: SIGNAL INNER with electrical stack #0, which means it is INNER ALL

// For both TOP and BOTTOM layers
#define LAY_PADOUT         "PadOut"    // Priority 6: PAD OUTTER with any electrical stack #
#define LAY_SIGOUT         "SigOut"    // Priority 7: SIGNAL OUTTER with any electical stack #

// For TOP, BOTTOM, and INNER layers
#define LAY_PADALL         "PadAll"    // Priority 8: PAD ALL with any electrical stack #
#define LAY_SIGALL         "SigAll"    // Priority 9: SIGNAL ALL with any electrical stack #
/******************************************************************************/


/* Structures Section *********************************************************/

typedef struct
{
   int            layertype;     // Layertype nt
   int            padslayernr;

   LayerTypeTag getLayerType() const         { return (LayerTypeTag)layertype; }
   void setLayerType(LayerTypeTag layerType) { layertype = layerType; }
   void setLayerType(int layerType)          { layertype = ((layerType >= layerTypeLowerBound && layerType <= layerTypeUpperBound) ? layerType : layerTypeUndefined);; }
}PADS_LayerMap;
typedef CTypedPtrArray<CPtrArray, PADS_LayerMap*> CLayerMapArray;

typedef struct
{
   int            endpointindex;    
}PADS_ConnectLinkMap;
typedef CTypedPtrArray<CPtrArray, PADS_ConnectLinkMap*> CConnectLinkArray;

typedef struct
{
   CString        vianame;
} PADS_ViaStruct;
typedef CTypedPtrArray<CPtrArray, PADS_ViaStruct*> ViaArray;

typedef struct
{
   CString        padshapename;
} PADS_ComplexErrorStruct;
typedef CTypedPtrArray<CPtrArray, PADS_ComplexErrorStruct*> ComplexErrorArray;

typedef struct
{
   CString        vianame;
} PADSTraceViaStruct;
typedef CTypedPtrArray<CPtrArray, PADSTraceViaStruct*> TraceViaArray;

typedef struct
{
   CString        compname;
   CString        pinname;
   double         x;
   double         y;
} PADSTracePinStruct;
typedef CTypedPtrArray<CPtrArray, PADSTracePinStruct*> TracePinArray;

typedef struct
{
   CString        padstackname;
   int            typ;           // 1, 2, 4
   UINT64         layermap;      // normal layer
   UINT64         mirrormap;     // mirrored layer (done for speed reasons once)
} PADS_PadStackStruct;
typedef CTypedPtrArray<CPtrArray, PADS_PadStackStruct*> PadStackArray;

typedef struct
{
   int            level;
   int            stacknr;
   double         size;
   CString        shape;
   double         idia;
   double         finori;
   double         finlength;
   double         finoffset;
   BlockStruct    *padshape;
   int            complex_error_reported;
} PADS_PadshapeStruct;
typedef CTypedPtrArray<CPtrArray, PADS_PadshapeStruct*> PadShapeArray;

typedef struct
{
   CString        pinname;
   int            pinnr;      // this is the sequence the T%d is written in DECAL 1..n
   double         x;
   double         y;
   double         rot;
   int            mirror;
   CString        padstackname;
   int            used_as_default;
   DataStruct     *pinData;
} PADS_TerminalStruct;
typedef CTypedPtrArray<CPtrArray, PADS_TerminalStruct*> TerminalArray;

typedef struct
{
   int            blocknr;    // blocknumber of decal
   CString        pinname;
   int            pinnr;      // this is the sequence the T%d is written in DECAL 1..n
   int            m_normalizedPinNumber;
} PADS_DecalPinNameStruct;

class DecalPinNameArray : public CTypedPtrArray<CPtrArray, PADS_DecalPinNameStruct*>
{
public:
   void normalizePinNumbers();
};

//_____________________________________________________________________________
class CPadsDecalPin
{
private:
   CString m_pinName;
   int m_pinNumber;

public:
   CPadsDecalPin(const CString& pinName,int pinNumber);

};

//_____________________________________________________________________________
class CPadsDecalPins : CTypedMapStringToPtrContainer<CPadsDecalPin*>
{
public:
   CPadsDecalPin& addPin(const CString& pinName,int pinNumber);
};

//_____________________________________________________________________________
class CPadsDecal
{
private:
   BlockStruct& m_block;
   CPadsDecalPins m_pins;

public:
   CPadsDecal(BlockStruct& block);

   CPadsDecalPin& addPin(const CString& pinName,int pinNumber)
      { return m_pins.addPin(pinName,pinNumber); }
   BlockStruct& getBlock() const { return m_block; }
};

//_____________________________________________________________________________
class CPadsDecals
{
private:
   CTypedMapIntToPtrContainer<CPadsDecal*> m_decals;

public:
   CPadsDecal& addDecal(BlockStruct& block);
   CPadsDecal& getDecal(BlockStruct& block);
};

//_____________________________________________________________________________
typedef struct
{
   CString        typname;
   CString        decalname;
   BlockStruct    *block;
} PADS_PartType;
typedef CTypedPtrArray<CPtrArray, PADS_PartType*> PartTypeArray;

typedef struct
{
   int            blockNum;
   DbUnit         xOffset;
   DbUnit         yOffset;
} PADS_ApertureOffset;

#endif
